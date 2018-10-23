#ifndef __MDEF_H__
#define __MDEF_H__

/*************************************************************
*
*   FILE:             MDEF.H
*
*   (C) Copyright 97 Artech Digital Entertainments.
*                    All rights reserved.
*
*   Monopoly Game Global Define and Variable Header.
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/

// Supported Language Defines
#define MDEF_Unsupported    0
#define MDEF_English_US     1
#define MDEF_English_UK     2
#define MDEF_French         3
#define MDEF_German         4
#define MDEF_Spanish        5
#define MDEF_Dutch          6
#define MDEF_Swedish        7
#define MDEF_Finnish        8
#define MDEF_Danish         9
#define MDEF_Norwegian      10
#define MDEF_Max            11

// Current game version, only one should be enabled
#define MDEF_Monopoly_StarWars      1
//#define MDEF_Monopoly_Classic       1
//#define MDEF_Monopoly_Millenium     1
//#define MDEF_Monopoly_PGA           1

// Defines for file info
#define MDEF_MAX_PATH_LENGTH _MAX_PATH

#define MDEF_TICK_RESOLUTION    1000    // delay between tick messages in milleseconds

/************************************************************/

/************************************************************/
/* EXTERNAL VARIABLES                                       */
/************************************************************/

extern BYTE     MDEF_CurrentLanguage;
extern BOOL     MDEF_CompleteInitializationSuccessful;
/************************************************************/

#endif // __MDEF_H__
