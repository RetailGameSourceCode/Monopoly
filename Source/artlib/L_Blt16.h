#ifndef __L_BLT16_H__
#define __L_BLT16_H__
/*****************************************************************************
 * FILE:        L_Blt16.h
 *
 * DESCRIPTION: Obsolete bitmap copying routines for 16 bit mode.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Blt16.h 1     1/16/99 4:47p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Blt16.h $
 * 
 * 1     1/16/99 4:47p Agmsmith
 * Put back old blitter routines so that they can optinally be used to
 * avoid / demonstrate bugs in the new ones.
 ****************************************************************************/



/*******************************************************************************
*
*   Name:           LI_BLT16BitBlt16to16
*
*   Description:    Blts from a 16 bit bitmap to a 16 bit bitmap.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   nXBltStartCoordinate        - X coord, ul corner of blt RECT
*                   nYBltStartCoordinate        - Y coord, ul corner of blt RECT
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - source blt RECT
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT16BitBlt16to16
(
    LPBYTE          lpDestinationBits,
    int             nXDestinationWidthInPixels,
    int             nXBltStartCoordinate,
    int             nYBltStartCoordinate,
    LPBYTE          lpSourceBits,
    int             nXSourceWidthInPixels,
    LPRECT          lprSourceBltRect
);


/*******************************************************************************
*
*   Name:           LI_BLT16BitBlt8to16
*
*   Description:    Blts from an 8 bit paletted bitmap to a 16 bit native bitmap.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   nXBltStartCoordinate        - X coord, ul corner of blt RECT
*                   nYBltStartCoordinate        - Y coord, ul corner of blt RECT
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - source blt RECT
*                   lpColorTable                - pointer to 8 to 16 bit color table
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT16BitBlt8to16
(
    LPBYTE          lpDestinationBits,
    int             nXDestinationWidthInPixels,
    int             nXBltStartCoordinate,
    int             nYBltStartCoordinate,
    LPBYTE          lpSourceBits,
    int             nXSourceWidthInPixels,
    LPRECT          lprSourceBltRect,
    register LPLONG lpColorTable
);


/*******************************************************************************
*
*   Name:           LI_BLT16ColorKeyBitBlt8to16
*
*   Description:    Blts from an 8 bit paletted bitmap to a 16 bit native bitmap, treating the color index 0 as transparent.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   nXBltStartCoordinate        - X coord, ul corner of blt RECT
*                   nYBltStartCoordinate        - Y coord, ul corner of blt RECT
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - source blt RECT
*                   lpColorTable                - pointer to 8 to 16 bit color table
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT16ColorKeyBitBlt8to16
(
    LPBYTE          lpDestinationBits,
    int             nXDestinationWidthInPixels,
    int             nXBltStartCoordinate,
    int             nYBltStartCoordinate,
    LPBYTE          lpSourceBits,
    int             nXSourceWidthInPixels,
    LPRECT          lprSourceBltRect,
    register LPLONG lpColorTable
);


/*******************************************************************************
*
*   Name:         LI_BLT16AlphaBitBlt8to16
*
*   Description:  blts from an 8 bit paletted bitmap to a 16 bit native
*                 bitmap, using alpha channel data in the palette to
*                 generate transparent pixels
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 nXBltStartCoordinate - X coord, ul corner of blt RECT
*                 nYBltStartCoordinate - Y coord, ul corner of blt RECT
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - source blt RECT
*                 lpColorTable - pointer to 8 to 16 bit color table
*                 nAlphaInColorTable - number of alpha channel entries
*                                       in color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT16AlphaBitBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  register LPLONG lpColorTable,
  int nAlphaInColorTable);


/*******************************************************************************
*
*   Name:         LI_BLT16AlphaBitBlt16to16
*
*   Description:  blts from a 16 bit bitmap to a 16 bit bitmap,
*                 using a passed transparency value
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 nXBltStartCoordinate - X coord, ul corner of blt RECT
*                 nYBltStartCoordinate - Y coord, ul corner of blt RECT
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - source blt RECT
*                 nAlphaValue - transparency percentage as in l_anim.h
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT16AlphaBitBlt16to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue
);


/*******************************************************************************
*
*   Name:           LI_BLT16ColorBlt
*
*   Description:    Fills a destination RECT with a given color.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   lprDestinationBltRect       - destination blt RECT
*                   crColorToFillWith           - color to fill with, may be COLORREF or may be 16 bit color
*                   bColorRefIs16BitColor       - flag: given color is 16 bit color
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT16ColorBlt
(
    LPBYTE          lpDestinationBits,
    int             nXDestinationWidthInPixels,
    LPRECT          lprDestinationBltRect,
    DWORD           crColorToFillWith,
    BOOL            bColorRefIs16BitColor
);


/*******************************************************************************
*
*   Name:         LI_BLT16StretchBlt8to16
*
*   Description:  stretch blts an 8 bit paletted source bitmap to a 16 bit
*                 destination bitmap with clipping
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 lprDestinationBltRect - destination blt RECT without clipping
*                                         used to determine amount to stretch
*                                         in X and Y
*                 lprDestinationClipRect - destination clip RECT, may be NULL
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - pointer to source blt RECT,
*                                     should exactly enclose entire source bitmap
*                                     used to determine amount to stretch
*                                     in X and Y, NO CLIPPING, clipping is done
*                                     at destination
*                 lpColorTable - pointer to 8 to 16 bit color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT16StretchBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  LPRECT lprDestinationBltRect,
  LPRECT lprDestinationClipRect,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  register LPLONG lpColorTable);


/*******************************************************************************
*
*   Name:           LI_BLT16ColorKeyStretchBlt8to16
*
*   Description:    Stretch blts an 8 bit paletted source bitmap to a 16 bit destination bitmap with clipping, pixel value 0 is considered to be transparent.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   lprDestinationBltRect       - destination blt RECT without clipping used to determine amount to stretch in X and Y
*                   lprDestinationClipRect      - destination clip RECT, may be NULL
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - pointer to source blt RECT, should exectly enclose entire source bitmap used to determine amount to stretch in X and Y, NO CLIPPING, clipping is done at destination
*                   lpColorTable                - pointer to 8 to 16 bit color table
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT16ColorKeyStretchBlt8to16
(
    LPBYTE          lpDestinationBits,
    int             nXDestinationWidthInPixels,
    LPRECT          lprDestinationBltRect,
    LPRECT          lprDestinationClipRect,
    LPBYTE          lpSourceBits,
    int             nXSourceWidthInPixels,
    LPRECT          lprSourceBltRect,
    register LPLONG lpColorTable
);


/*******************************************************************************
*
*   Name:         LI_BLT16AlphaStretchBlt8to16
*
*   Description:  stretch blts an 8 bit paletted source bitmap to a 16 bit
*                 destination bitmap with clipping, using alpha channel data
*                 in the palette to generate transparent pixels
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 lprDestinationBltRect - destination blt RECT without clipping
*                                         used to determine amount to stretch
*                                         in X and Y
*                 lprDestinationClipRect - destination clip RECT, may be NULL
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - pointer to source blt RECT,
*                                     should exactly enclose entire source bitmap
*                                     used to determine amount to stretch
*                                     in X and Y, NO CLIPPING, clipping is done
*                                     at destination
*                 lpColorTable - pointer to 8 to 16 bit color table
*                 nAlphaInColorTable - number of alpha channel entries
*                                       in color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT16AlphaStretchBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  LPRECT lprDestinationBltRect,
  LPRECT lprDestinationClipRect,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  register LPLONG lpColorTable,
  int nAlphaInColorTable);


/*******************************************************************************
*
*   Name:           LI_BLT16ConvertColorRefTo16BitColor
*
*   Description:    Converts a COLORREF to a 16 bit color.
*
*   Arguments:      crColorToFillWith   - COLORREF to convert to 16 bit color
*
*   Globals:        nRedShift16         - number of first bit set in wRedMask16 (1 to 16)
*                   nGreenShift16       -  number of first bit set in wGreenMask16 (1 - 16)
*                   nBlueShift16        - number of first bit set in wBlueMask16 (1 - 16)
*                   wRedMask16          - mask of red bits for 16 bit DIBs
*                   wGreenMask16        - mask of green bits for 16 bit DIBs
*                   wBlueMask16         - mask of blue bits for 16 bit DIBs
*
*   Returns:        16 bit color
*
*******************************************************************************/
WORD LI_BLT16ConvertColorRefTo16BitColor
(
    COLORREF crColorToFillWith
);


/*******************************************************************************
*
*   Name:           LI_BLT16ConvertRGBColorTableTo16BitColor
*
*   Description:    Converts an RGBQUAD color table to 16 bit colors.
*
*   Arguments:      lpRGBColorTable     - pointer to RGBQUAD color table nColorTableEntries - number of entries in color table
*
*   Globals:        nRedShift16         - number of first bit set in wRedMask16 (1 to 16)
*                   nGreenShift16       -  number of first bit set in wGreenMask16 (1 - 16)
*                   nBlueShift16        - number of first bit set in wBlueMask16 (1 - 16)
*                   wRedMask16          - mask of red bits for 16 bit DIBs
*                   wGreenMask16        - mask of green bits for 16 bit DIBs
*                   wBlueMask16         - mask of blue bits for 16 bit DIBs
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT16ConvertRGBColorTableTo16BitColor
(
    LPLONG          lpRGBColorTable,
    int             nColorTableEntries
);


/*******************************************************************************
*
*   Name:           LI_BLT16ConvertRGBValuesTo16BitColor
*
*   Description:    Converts a 24 bit DIB to 16 bit DIB.
*
*   Arguments:      lp16BitData             - pointer to 16 bit DIB bits
*                   lp24BitData             - pointer to 24 bit DIB bits
*                   sWidthBitmapInPixels    - width of source DIB in pixels
*                   sHeightBitmapInPixels   - height of source DIB in pixels
*                   dWidthBitmapInPixels    - width of dest DIB in pixels
*                   dHeightBitmapInPixels   - height of dest DIB in pixels
*
*   Globals:        nRedShift16             - number of first bit set in wRedMask16 (1 to 16)
*                   nGreenShift16           - number of first bit set in wGreenMask16 (1 - 16)
*                   nBlueShift16            - number of first bit set in wBlueMask16 (1 - 16)
*                   wRedMask16              - mask of red bits for 16 bit DIBs
*                   wGreenMask16            - mask of green bits for 16 bit DIBs
*                   wBlueMask16             - mask of blue bits for 16 bit DIBs
*
*   Returns:            None.
*
*******************************************************************************/
void LI_BLT16ConvertRGBValuesTo16BitColor
(
    LPBYTE          lp16BitData,
    LPBYTE          lp24BitData,
    int             sWidthBitmapInPixels,
    int             sHeightBitmapInPixels,
    int             dWidthBitmapInPixels,
    int             dHeightBitmapInPixels
);

/*******************************************************************************
*
*   Name:           LI_BLTColorKeyPureGreen16BitBlt16to16
*
*   Description:    Blts from a 16 bit bitmap to a 16 bit bitmap. A color of
*                   pure green is transparent
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   nXBltStartCoordinate        - X coord, ul corner of blt RECT
*                   nYBltStartCoordinate        - Y coord, ul corner of blt RECT
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - source blt RECT
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT16ColorKeyPureGreenBitBlt16to16
(
    LPBYTE          lpDestinationBits,
    int             nXDestinationWidthInPixels,
    int             nXBltStartCoordinate,
    int             nYBltStartCoordinate,
    LPBYTE          lpSourceBits,
    int             nXSourceWidthInPixels,
    LPRECT          lprSourceBltRect
);


/*******************************************************************************
*
*   Name:         LI_BLT16ColorKeyGreenishBitBlt16to16
*
*   Description:
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 nXBltStartCoordinate - X coord, ul corner of blt RECT
*                 nYBltStartCoordinate - Y coord, ul corner of blt RECT
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - source blt RECT
*
*   Globals:      nRedShift16 - number of first bit set in wRedMask16
*                 nGreenShift16 - number of first bit set in wGreenMask16
*                 nBlueShift16 - number of first bit set in wBlueMask16
*                 wRedMask16 - mask of red bits for 16 bit DIBs
*                 wGreenMask16 - mask of green bits for 16 bit DIBs
*                 wBlueMask16 - mask of blue bits for 16 bit DIBs
*
*   Returns:      none
*
*******************************************************************************/

//void LI_BLT16ColorKeyGreenishBitBlt16to16
//(
//  LPBYTE lpDestinationBits,
//  int nXDestinationWidthInPixels,
//  int nXBltStartCoordinate,
//  int nYBltStartCoordinate,
//  LPBYTE lpSourceBits,
//  int nXSourceWidthInPixels,
//  LPRECT lprSourceBltRect
//);

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_BlitBoundsCheck
/*******************************************************************************
*
*   Name:     LI_BLT16BreaksBounds
*
*   Description:  Returns TRUE if a blt starting in ARTECH memory defined by
*         lpDestinationBits. At Coordinates nX and nY. For rect
*         lprSourceRect will blow the bounds of the ARTECH memory block.
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*         nXDestinationWidthInPixels - destination bitmap width
*         nXBltStartCoordinate - X coord, ul corner of blt RECT
*         nYBltStartCoordinate - Y coord, ul corner of blt RECT
*         lprSourceBltRect - source blt RECT
*
*   Returns:    TRUE or FALSE
*
*******************************************************************************/
BOOL LI_BLT16BreaksBounds
(
    LPBYTE lpDestinationBits,
    int nXDestinationWidthInPixels,
    int nXBltStartCoordinate,
    int nYBltStartCoordinate,
   LPRECT lprSourceBltRect
);
#endif // CE_ARTLIB_EnableDebugMode && CE_ARTLIB_BlitBoundsCheck

/************************************************************/


#endif // __L_BLT16_H__
