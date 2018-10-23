#ifndef __L_BLT24_H__
#define __L_BLT24_H__
/*****************************************************************************
 * FILE:        L_Blt24.h
 *
 * DESCRIPTION: Obsolete bitmap copying routines for 24 bit mode.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Blt24.h 1     1/16/99 4:47p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Blt24.h $
 * 
 * 1     1/16/99 4:47p Agmsmith
 * Put back old blitter routines so that they can optinally be used to
 * avoid / demonstrate bugs in the new ones.
 ****************************************************************************/



/*******************************************************************************
*
*   Name:           LI_BLT24BitBlt24to24
*
*   Description:    Blts from a 24 bit bitmap to a 24 bit bitmap.
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
void LI_BLT24BitBlt24to24
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
*   Name:           LI_BLT24BitBlt8to24
*
*   Description:    Blts from an 8 bit paletted bitmap to a 24 bit native bitmap.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   nXBltStartCoordinate        - X coord, ul corner of blt RECT
*                   nYBltStartCoordinate        - Y coord, ul corner of blt RECT
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - source blt RECT
*                   lpColorTable                - pointer to 8 to 24 bit color table
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT24BitBlt8to24
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
*   Name:           LI_BLT24ColorKeyBitBlt8to24
*
*   Description:    Blts from an 8 bit paletted bitmap to a 24 bit native bitmap, treating the color index 0 as transparent.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   nXBltStartCoordinate        - X coord, ul corner of blt RECT
*                   nYBltStartCoordinate        - Y coord, ul corner of blt RECT
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - source blt RECT
*                   lpColorTable                - pointer to 8 to 24 bit color table
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT24ColorKeyBitBlt8to24
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
*   Name:         LI_BLT24AlphaBitBlt8to24
*
*   Description:  blts from an 8 bit paletted bitmap to a 24 bit native
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
*                 lpColorTable - pointer to 8 to 24 bit color table
*                 nAlphaInColorTable - number of alpha channel entries
*                                       in color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT24AlphaBitBlt8to24(
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
*   Name:         LI_BLT24AlphaBitBlt24to24
*
*   Description:  blts from a 24 bit bitmap to a 24 bit bitmap,
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
void LI_BLT24AlphaBitBlt24to24(
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
*   Name:           LI_BLT24ColorBlt
*
*   Description:    Fills a destination RECT with a given color.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   lprDestinationBltRect       - destination blt RECT
*                   crColorToFillWith           - color to fill with, may be COLORREF or may be native 24 bit color
*                   bColorRefIs24BitBGRColor    - flag: given color is 24 bit BGR color
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT24ColorBlt
(
    LPBYTE          lpDestinationBits,
    int             nXDestinationWidthInPixels,
    LPRECT          lprDestinationBltRect,
    DWORD           crColorToFillWith,
    BOOL            bColorRefIs24BitBGRColor
);


/*******************************************************************************
*
*   Name:         LI_BLT24StretchBlt8to24
*
*   Description:  stretch blts an 8 bit paletted source bitmap to a 24 bit
*                 destination bitmap with clipping,
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
*                 lpColorTable - pointer to 8 to 24 bit color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT24StretchBlt8to24(
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
*   Name:           LI_BLT24ColorKeyStretchBlt8to24
*
*   Description:    Stretch blts an 8 bit paletted source bitmap to a 24 bit destination bitmap with clipping, pixel value 0 is considered to be transparent.
*
*   Arguments:      lpDestinationBits           - pointer to destination bitmap bits
*                   nXDestinationWidthInPixels  - destination bitmap width
*                   ylprDestinationBltRect      - destination blt RECT without clipping used to determine amount to stretch in X and Y
*                   lprDestinationClipRect      - destination clip RECT, may be NULL
*                   lpSourceBits                - pointer to source bitmap bits
*                   nXSourceWidthInPixels       - source bitmap width
*                   lprSourceBltRect            - pointer to source blt RECT, should exectly enclose entire source bitmap used to determine amount to stretch in X and Y, NO CLIPPING, clipping is done at destination
*                   lpColorTable                - pointer to 8 to 24 bit color table
*
*   Returns:        None.
*
*******************************************************************************/
void LI_BLT24ColorKeyStretchBlt8to24
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
*   Name:         LI_BLT24AlphaStretchBlt8to24
*
*   Description:  stretch blts an 8 bit paletted source bitmap to a 24 bit
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
*                 lpColorTable - pointer to 8 to 24 bit color table
*                 nAlphaInColorTable - number of alpha channel entries
*                                       in color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT24AlphaStretchBlt8to24(
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
*   Name:           LI_BLT24ConvertColorRefTo24BitColor
*
*   Description:    converts a COLORREF which is 24 bit RGB to 24 bit BGR i.e. blue value in the low order byte, which is the 24 DIB native mode.
*
*   Arguments:      crColorToFillWith       - COLORREF to convert to BGR
*
*   Returns:        24 bit BGR value in the low order 3 bytes of a DWORD
*
*******************************************************************************/
DWORD LI_BLT24ConvertColorRefTo24BitColor
(
    COLORREF crColorToFillWith
);


/*******************************************************************************
*
*   Name:           LI_BLT24ColorKeyPureGreenBitBlt24to24
*
*   Description:    Blts from a 24 bit bitmap to a 24 bit bitmap.
*                   A color of pure green is treated as transparent.
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
void LI_BLT24ColorKeyPureGreenBitBlt24to24
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
*   Name:         LI_BLT24ColorKeyGreenishBitBlt24to24
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
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
//void LI_BLT24ColorKeyGreenishBitBlt24to24
//(
//  LPBYTE lpDestinationBits,
//  int nXDestinationWidthInPixels,
//  int nXBltStartCoordinate,
//  int nYBltStartCoordinate,
//  LPBYTE lpSourceBits,
//  int nXSourceWidthInPixels,
//  LPRECT lprSourceBltRect
//);


/*******************************************************************************
*
*   Name:         LI_BLT24ConvertRGBValuesTo24BitColor
*
*   Description:  converts a 24 bit DIB to 24 bit DIB
*
*   Arguments:    lp24DestinationBitData - pointer to 24 bit DIB bits
*                 lp24SourceBitData - pointer to 24 bit DIB bits
*                                   sWidthBitmapInPixels - width of source DIB in pixels
*                                   sHeightBitmapInPixels - height of source DIB in pixels
*                                   dWidthBitmapInPixels - width of dest DIB in pixels
*                                   dHeightBitmapInPixels - height of dest DIB in pixels
*
*   Globals:      nRedShift - number of first bit set in dwRedMask (1 to 24)
*                 nGreenShift -  number of first bit set in dwGreenMask (1 - 24)
*                 nBlueShift - number of first bit set in dwBlueMask (1 - 24)
*                 dwRedMask - mask of red bits for 24 bit DIBs
*                 dwGreenMask - mask of green bits for 24 bit DIBs
*                 dwBlueMask - mask of blue bits for 24 bit DIBs
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT24ConvertRGBValuesTo24BitColor(
  LPBYTE lp24DestinationBitData,
  LPBYTE lp24SourceBitData,
    int src_w_pixels,
    int src_h_pixels,
    int dest_w_pixels,
    int dest_h_pixels);

#endif __L_BLT24_H__
