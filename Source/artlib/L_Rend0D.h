#ifndef __L_REND0D_H__
#define __L_REND0D_H__

/*****************************************************************************
 *
 * FILE:        L_REND0D.h
 * DESCRIPTION: Rendering zero dimensional stuff, under sequencer command.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Rend0D.h 4     1/05/99 5:00p Agmsmith $
 ****************************************************************************/

/************************************************************/
/* DEFINES                                                  */
/*----------------------------------------------------------*/

/************************************************************/
/* MACROS                                                   */
/*----------------------------------------------------------*/

/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/

/************************************************************/
/* GLOBAL VARIABLES - see L_REND0D.c for details            */
/*----------------------------------------------------------*/


/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

extern void LI_REND0D_InitSystem (void);
extern void LI_REND0D_RemoveSystem (void);

extern BOOL LE_REND0D_InstallInRenderSlot (
  LE_REND_RenderSlot RenderSlot);

#endif // __L_REND0D_H__
