#ifndef __L_PRINT_H__
#define __L_PRINT_H__

/*************************************************************
*
*   FILE:           l_PRINT.h
*   DESCRIPTION:    PRINT system header.
*
*   (C) Copyright 1998 Artech Digital Entertainments.
*                         All rights reserved.
*
*************************************************************/

// Start Doc
// L_PRINT

// - Add Documentation here.

// End Doc

/************************************************************/
/* DEFINES                                                  */
/*----------------------------------------------------------*/

/************************************************************/

/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/

/************************************************************/

/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/
/************************************************************/

/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

extern BOOL    LE_PRINT_Setting_UsePrintDialog;
extern BOOL    LE_PRINT_Setting_Landscape;
extern LPSTR   LE_PRINT_Setting_PrintJobName;
extern float   LE_PRINT_Setting_XOffsetInInches;
extern float   LE_PRINT_Setting_YOffsetInInches;
extern float   LE_PRINT_Setting_WidthInInches;
extern float   LE_PRINT_Setting_HeightInInches;

void	LI_PRINT_InitSystem( void );
void	LI_PRINT_RemoveSystem( void );
BOOL	LE_PRINT_PrintImage( LPBYTE SourceBitsPointer,
							int Width,
							int Height,
							int	BitsPerPixel,
							LPBYTE PalettePointer,
							DWORD RedMask,
							DWORD GreenMask,
							DWORD BlueMask
							);
void LE_PRINT_Blt16to24( LPBYTE DestinationBitsPointer,
							int DestinationWidthInPixels,
							int XBltStartCoord,
							int YBltStartCoord,
							LPBYTE SourceBitsPointer,
							int SourceWidthInPixels,
							LPRECT SourceBltRect,
							DWORD RedMask,
							DWORD GreenMask,
							DWORD BlueMask,
							BOOL FlipTopToBottom,
							BOOL MirrorRightToLeft );
/************************************************************/

typedef enum {
    SAFP_FROM_DISPLAYSURFACE = 0,
    SAFP_FROM_WORKINGSURFACE,
    SAFP_FROM_BACKGROUNDSURFACE,
} SAFP_FROM_SURFACE;

/************************************************************/

#endif // __L_PRINT_H__
