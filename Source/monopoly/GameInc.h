#ifndef __GAMEINC_H__
#define __GAMEINC_H__

/*****************************************************************************
 *
 * FILE:        GameInc.h
 * DESCRIPTION: The include files used by this game.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 ****************************************************************************/


// Library code, which also includes the OS includes and
// standard C stuff and your C_ArtLib.h too...
#include "../artlib/l_inc.h"


// From PC3D.h - z buffer ranges.  They are not modifiable yet, these values looked good.
//#define NEAR_Z 25.0
//#define FAR_Z 1350

/************************************************************/
/* COMPILATION CONTROL SWITCHES                             */
/************************************************************/
#define FOREMAILVERSION 0
#define FORHOTSEAT      0
#define FORWINDOWS      1
#define FORWIN95        1
#define FORMACINTOSH    0

#define	ALTERNATE_DEED_SCREEN			0  // turn on to have SOLD and MORTGAGED displayed
											// in the deed status screen
#define MAYBEHUGE
#define MAYBEFAR
/* Message log defines                                                   */
#define MESS_REPORT_ACTION_MESSAGES       0
#define MESS_REPORT_ACTION_FILENAME       "RuleLog.txt"

// Misc defines
#if FORHOTSEAT
  #define ApplicationInstance hAppInstance
  #define MainWindowHandle hwndMain
#else
  #define ApplicationInstance LE_MAIN_Hinst
  #define MainWindowHandle LE_MAIN_HwndMainWindow
#endif

#define USE_PRESET_DICE_ROLLS             0  // In mono2, will cheat for the first few dice rolls
                                                              // off to disable the preset game loading.
#define USE_OPENING_MOVIES                1                   // Used to play the opening movies.
#define USE_HOTKEYS                       0 // Means developer keys are active/inactive.
#define USE_HOTKEYS2                      0 // Developer keys, but ones OK to send out (they dont screw up when you enter a players name, IE (w)ireframe)

#define USA_VERSION                       1 // Will flag the US version - different city card logic


#define TICK_TIME_DELAY 1000
#define MAIN_GAME_TIMER 0

#define DAT_MAIN    2
#define DAT_PAT     3
//#define DAT_CGE     4
#define DAT_BOARD   6
#define DAT_BOARD2  7
#define DAT_3D      8
#define DAT_LANG    9
#define DAT_LANG2   5
#define DAT_LANGDIALOG  10


/************************************************************/
/* SOURCE INCLUDE FILES                                     */
/************************************************************/
// Datafile headers...
#include "dat_mon/dat_main.h"
#include "dat_mon/dat_pat.h"
//#include "dat_mon/dat_cge.h"

//#include "dat_mon/dat_brd2.h" // Note: only the first entry (0) is directly accessed - probably can skip the header.
#define TAB_mnmip01pc0000               0x0000 // by doing this.
#include "dat_mon/dat_3d.h"
#include "dat_mon/dat_lang.h"
#if USA_VERSION
#include "dat_mon/dat_bord.h"
#include "dat_mon/dat_lm01.h"
#include "dat_mon/dat_lk01.h"
#else
#include "dat_mon/dat_borde.h"
#include "dat_mon/dat_lm02.h" // No deeds
#include "dat_mon/dat_lk02.h" // Extra card reading sound
#endif


#define STRICT 1
#include <Windows.h>
//#include <WindowsX.h>
//#include <Memory.h>
#include <StdLib.h>
#include <StdIO.h>
//#include <Assert.h>
//#include <CommCtrl.h>
//#include "BasicTyp.h"
#include <math.h>
#include <time.h>
#include <CrtDbg.h>

#include "..\PC3D\Debug.h"

// Engine
#include "MDef.h"
#include "Lang.h"
//#include "LangIDs.h"  // Now dat_lang.h
#include "Rule.h"
#include "Mess.h"
#include "Trade.h"
#include "DebugArt.h"
#include "Resource.h"
#include "Tickler.h"
#include "Unility.h"
#include "ai.h"
#include "ai_util.h"
#include "ai_trade.h"
#include "ai_load.h"
// Interface
#include "UDAuct.h"
#include "UDUtils.h"  //Out of order, udboard needs it.
#include "UDBoard.h"
#include "UDCGE.h"
#include "UDChat.h"
#include "UDIBar.h"
#include "UserIfce.h"
#include "UDOpts.h"
#include "UDPenny.h"
#include "UDPieces.h"
#include "UDPSel.h"
#include "UDSound.h"
#include "UDStats.h"
#include "UDTrade.h"
// Display.h needs WHATEVER_MAX from enums and other data from your header files.
// If you must have something from display.h in your header file, just put the
//  whole thing in display.h - no chicken vs egg stuff - we let the egg win (parent last).
#include "display.h"

#if FOREMAILVERSION
#include "Email.h"
// Email API library includes.
#include "CommonHEG.h"
#include "crc32.h"
#include "ServerRequestMove.h"
#include "SMAPIClient.h"
#include "stdafx.h"
#endif


/* And some global hack defines to replace missing Artech library stuff. */
#define DBUG_DisplayNonFatalErrorMessage(x) DisplayMessage ("Monopoly Error!", x)
#define LE_ERROR_ErrorMsg(x) DisplayMessage ("Monopoly Error!", x)


#endif  //__GAMEINC_H__
