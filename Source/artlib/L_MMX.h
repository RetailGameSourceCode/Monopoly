/*************************************************************
*
*   FILE:           l_mmx.h
*   DESCRIPTION:    MMX detection and 16 bit mmx blt routines
*
*   (C) Copyright 1995/96 Artech Digital Entertainments.
*                         All rights reserved.
*
*************************************************************/

/************************************************************/
/* OVERVIEW                                                 */
/*----------------------------------------------------------*

************************************************************/

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
extern BOOL  LI_MMX_Available;  // Keeps track of whether the CPU is
                                // MMX enabled.
/************************************************************/

/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

BOOL LI_MMX_DetectMMX (void);   // TRUE of FALSE, depending on whether the CPU
                                // supports MMX or not.
BOOL    LE_MMX_Is_Available(void);  // Used to determine whether the CPU is MMX enabled,
                                    // as detected by the last call to LI_MMX_DetectMMX.
                                    // (ie. returns the value of LI_MMX_Available.)

void LI_MMX_InitSystem(void); // Initializes MMX system.

#if CE_ARTLIB_BlitUseOldRoutines // Unused MMX blitter code.

void LI_MMXAlphaBitBlt8to16(LPBYTE lpDestinationBits, int nXDestinationWidthInPixels,
  int nXBltStartCoordinate, int nYBltStartCoordinate, LPBYTE lpSourceBits,
  int nXSourceWidthInPixels, LPRECT lprSourceBltRect, register LPLONG lpColorTable,
  int nAlphaInColorTable);    // mmx version of the alpha 8-16 blit routine.

void LI_MMXAlphaBitBlt8to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  register LPLONG lpColorTable,
  int nAlphaInColorTable);  // mmx version of the alpha 8-32 blit routine.

void LI_MMXAlphaBitBlt8to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  register LPLONG lpColorTable,
  int nAlphaInColorTable);  // mmx version of the alpha 8-24 blit routine.

/*******************************************************************************
*
*   Name:         LI_MMXAlphaBitBlt16to16
*
*   Description:  blts from a 16 bit bitmap to a 16 bit bitmap,
*         using a passed transparency value. Using inline MMX
*         assembly code.
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - destination bitmap width
*         nXBltStartCoordinate - X coord, ul corner of blt RECT
*         nYBltStartCoordinate - Y coord, ul corner of blt RECT
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - source bitmap width
*         lprSourceBltRect - source blt RECT
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXAlphaBitBlt16to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue
);                // mmx version of the alpha 16 to 16 blt routine.

/*******************************************************************************
*
*   Name:         LI_MMXAlphaBitBlt24to24
*
*   Description:  blts from a 24 bit bitmap to a 24 bit bitmap,
*         using a passed transparency value. Using inline MMX
*         assembly code.
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - destination bitmap width
*         nXBltStartCoordinate - X coord, ul corner of blt RECT
*         nYBltStartCoordinate - Y coord, ul corner of blt RECT
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - source bitmap width
*         lprSourceBltRect - source blt RECT
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXAlphaBitBlt24to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue
);                // mmx version of the alpha 24 to 24 bit routine

/*******************************************************************************
*
*   Name:         LI_MMXAlphaBitBlt32to32
*
*   Description:  blts from a 32 bit bitmap to a 32 bit bitmap,
*         using a passed transparency value. Using inline MMX
*         assembly code.
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - destination bitmap width
*         nXBltStartCoordinate - X coord, ul corner of blt RECT
*         nYBltStartCoordinate - Y coord, ul corner of blt RECT
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - source bitmap width
*         lprSourceBltRect - source blt RECT
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXAlphaBitBlt32to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue);     // mmx version of the alpha 32 to 32 bit routine

/*******************************************************************************
*
*   Name:         LI_MMXFade16
*
*   Description:  blts 16 bitmap to a 16 bitmap, fading to black (0,0,0).
*         using a passed transparency value.
*         ie: (putting the bitmap over a black background making the
*         image more and more transparent (or less and less for a
*         fade in)
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - width in pixels of the destination
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - width in pixels of the source
*         lprSourceBltRect - source blit rectangle (in case source is bigger
*                   than the destination, need to clip it)
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXFade16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue);

/*******************************************************************************
*
*   Name:         LI_MMXFade24
*
*   Description:  blts 24 bitmap to a 24 bitmap, fading to black (0,0,0).
*         using a passed transparency value.
*         ie: (putting the bitmap over a black background making the
*         image more and more transparent (or less and less for a
*         fade in)
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - width in pixels of the destination
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - width in pixels of the source
*         lprSourceBltRect - source blit rectangle (in case source is bigger
*                   than the destination, need to clip it)
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXFade24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue);

/*******************************************************************************
*
*   Name:         LI_MMXFade32
*
*   Description:  blts 32 bitmap to a 32 bitmap, fading to black (0,0,0).
*         using a passed transparency value.
*         ie: (putting the bitmap over a black background making the
*         image more and more transparent (or less and less for a
*         fade in)
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - width in pixels of the destination
*         lpSourceBits - pointer to source bitmap bits
*         nXSourceWidthInPixels - width in pixels of the source
*         lprSourceBltRect - source blit rectangle (in case source is bigger
*                   than the destination, need to clip it)
*         nAlphaValue - transparency percentage as in l_anim.h
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LI_MMXFade32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue);

#endif // Unused blitter code.
