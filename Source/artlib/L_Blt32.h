#ifndef __L_BLT32_H__
#define __L_BLT32_H__
/*****************************************************************************
 * FILE:        L_Blt16.h
 *
 * DESCRIPTION: Obsolete bitmap copying routines for 16 bit mode.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Blt32.h 1     1/16/99 4:47p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Blt32.h $
 * 
 * 1     1/16/99 4:47p Agmsmith
 * Put back old blitter routines so that they can optinally be used to
 * avoid / demonstrate bugs in the new ones.
 ****************************************************************************/



/*******************************************************************************
*
*   Name:           LI_BLT32BitBlt32to32
*
*   Description:    Blts from a 32 bit bitmap to a 32 bit bitmap.
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
void LI_BLT32BitBlt32to32
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
*   Name:           LI_BLT32BitBlt8to32
*
*   Description:    Blts from an 8 bit paletted bitmap to a 32 bit native bitmap.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   nXBltStartCoordinate        - X coord, ul corner of blt RECT
*                   nYBltStartCoordinate        - Y coord, ul corner of blt RECT
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - source blt RECT
*                   lpColorTable                - pointer to 8 to 32 bit color table
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT32BitBlt8to32
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
*   Name:           LI_BLT32ColorKeyBitBlt8to32
*
*   Description:    Blts from an 8 bit paletted bitmap to a 32 bit native bitmap, treating the color index 0 as transparent.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   nXBltStartCoordinate        - X coord, ul corner of blt RECT
*                   nYBltStartCoordinate        - Y coord, ul corner of blt RECT
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - source blt RECT
*                   ilpColorTable               - pointer to 8 to 32 bit color table
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT32ColorKeyBitBlt8to32
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
*   Name:         LI_BLT32AlphaBitBlt8to32
*
*   Description:  blts from an 8 bit paletted bitmap to a 32 bit native
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
*                 lpColorTable - pointer to 8 to 32 bit color table
*                 nAlphaInColorTable - number of alpha channel entries
*                                       in color table
*
*   Globals:      dwRedMask32 - mask of red bits for 32 bit DIBs
*                 dwGreenMask32 - mask of green bits for 32 bit DIBs
*                 dwBlueMask32 - mask of blue bits for 32 bit DIBs
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT32AlphaBitBlt8to32(
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
*   Name:         LI_BLT32AlphaBitBlt32to32
*
*   Description:  blts from a 32 bit bitmap to a 32 bit bitmap,
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
void LI_BLT32AlphaBitBlt32to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue);


/*******************************************************************************
*
*   Name:           LI_BLT32ColorBlt
*
*   Description:    Fills a destination RECT with a given color.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   lprDestinationBltRect       - destination blt RECT
*                   crColorToFillWith           - color to fill with, may be COLORREF or may be 32 bit native color
*                   bColorRefIs32BitColor       - flag: given color is 32 bit color
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT32ColorBlt
(
    LPBYTE          lpDestinationBits,
    int             nXDestinationWidthInPixels,
    LPRECT          lprDestinationBltRect,
    DWORD           crColorToFillWith,
    BOOL            bColorRefIs32BitColor
);


/*******************************************************************************
*
*   Name:         LI_BLT32StretchBlt8to32
*
*   Description:  stretch blts an 8 bit paletted source bitmap to a 32 bit
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
*                                     should exectly enclose entire source bitmap
*                                     used to determine amount to stretch
*                                     in X and Y, NO CLIPPING, clipping is done
*                                     at destination
*                 lpColorTable - pointer to 8 to 32 bit color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT32StretchBlt8to32(
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
*   Name:           LI_BLT32ColorKeyStretchBlt8to32
*
*   Description:    Stretch blts an 8 bit paletted source bitmap to a 32 bit destination bitmap with clipping, pixel value 0 is considered to be transparent.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   lprDestinationBltRect       - destination blt RECT without clipping used to determine amount to stretch in X and Y
*                   lprDestinationClipRect      - destination clip RECT, may be NULL
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - pointer to source blt RECT, should exectly enclose entire source bitmap used to determine amount to stretch in X and Y, NO CLIPPING, clipping is done at destination
*                   lpColorTable                - pointer to 8 to 32 bit color table
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT32ColorKeyStretchBlt8to32
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
*   Name:         LI_BLT32AlphaStretchBlt8to32
*
*   Description:  stretch blts an 8 bit paletted source bitmap to a 32 bit
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
*                                     should exectly enclose entire source bitmap
*                                     used to determine amount to stretch
*                                     in X and Y, NO CLIPPING, clipping is done
*                                     at destination
*                 lpColorTable - pointer to 8 to 32 bit color table
*                 nAlphaInColorTable - number of alpha channel entries
*                                       in color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT32AlphaStretchBlt8to32(
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
*   Name:           LI_BLT32ConvertColorRefTo32BitColor
*
*   Description:    Converts a COLORREF to a 32 bit color.
*
*   Arguments:      crColorToFillWith   - COLORREF to convert to 32 bit color
*
*   Globals:        nRedShift32         - number of first bit set in dwRedMask32 (1 to 32)
*                   nGreenShift32       -  number of first bit set in dwGreenMask32 (1 - 32)
*                   nBlueShift32        - number of first bit set in dwBlueMask32 (1 - 32)
*                   dwRedMask32         - mask of red bits for 32 bit DIBs
*                   dwGreenMask32       - mask of green bits for 32 bit DIBs
*                   dwBlueMask32        - mask of blue bits for 32 bit DIBs
*
*   Returns:        32 bit color
*
*******************************************************************************/
DWORD LI_BLT32ConvertColorRefTo32BitColor
(
    COLORREF        crColorToFillWith
);


/*******************************************************************************
*
*   Name:           LI_BLT32ConvertRGBColorTableTo32BitColor
*
*   Description:    Converts an RGBQUAD color table to 32 bit colors.
*
*   Arguments:      lpRGBColorTable     - pointer to RGBQUAD color table nColorTableEntries - number of entries in color table
*
*   Globals:        nRedShift32         - number of first bit set in dwRedMask32 (1 to 32)
*                   nGreenShift32       -  number of first bit set in dwGreenMask32 (1 - 32)
*                   nBlueShift32        - number of first bit set in dwBlueMask32 (1 - 32)
*                   dwRedMask32         - mask of red bits for 32 bit DIBs
*                   dwGreenMask32       - mask of green bits for 32 bit DIBs
*                   dwBlueMask32        - mask of blue bits for 32 bit DIBs
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT32ConvertRGBColorTableTo32BitColor
(
    LPLONG          lpRGBColorTable,
    int             nColorTableEntries
);


/*******************************************************************************
*
*   Name:           LI_BLT32ConvertRGBValuesTo32BitColor
*
*   Description:    Converts a 24 bit DIB to 32 bit DIB.
*
*   Arguments:      lp32BitData             - pointer to 32 bit DIB bits
*                   lp24BitData             - pointer to 24 bit DIB bits
*                   nWidthBitmapInPixels    - width of DIBs in pixels
*                   nHeightBitmapInPixels   - height of DIBs in pixels
*
*   Globals:        nRedShift32             - number of first bit set in dwRedMask32 (1 to 32)
*                   nGreenShift32           -  number of first bit set in dwGreenMask32 (1 - 32)
*                   nBlueShift32            - number of first bit set in dwBlueMask32 (1 - 32)
*                   dwRedMask32             - mask of red bits for 32 bit DIBs
*                   dwGreenMask32           - mask of green bits for 32 bit DIBs
*                   dwBlueMask32            - mask of blue bits for 32 bit DIBs
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT32ConvertRGBValuesTo32BitColor
(
    LPBYTE          lp32BitData,
    LPBYTE          lp24BitData,
    int             sWidthBitmapInPixels,
    int             sHeightBitmapInPixels,
    int             dWidthBitmapInPixels,
    int             dHeightBitmapInPixels
);


/*******************************************************************************
*
*   Name:           LI_BLT32ColorKeyPureGreenBitBlt32to32
*
*   Description:    Blts from a 32 bit bitmap to a 32 bit bitmap.
*                   A color of pure green is considered to be transparent.
*                   Or near greenish or bluish.
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
void LI_BLT32ColorKeyPureGreenBitBlt32to32
(
    LPBYTE          lpDestinationBits,
    int             nXDestinationWidthInPixels,
    int             nXBltStartCoordinate,
    int             nYBltStartCoordinate,
    LPBYTE          lpSourceBits,
    int             nXSourceWidthInPixels,
    LPRECT          lprSourceBltRect
);


#endif // __L_BLT32_H__
