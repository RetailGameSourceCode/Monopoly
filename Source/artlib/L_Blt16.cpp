/*****************************************************************************
 * FILE:        L_Blt16.cpp
 *
 * DESCRIPTION: Obsolete bitmap copying routines for 16 bit mode.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Blt16.cpp 1     1/16/99 4:47p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Blt16.cpp $
 * 
 * 1     1/16/99 4:47p Agmsmith
 * Put back old blitter routines so that they can optinally be used to
 * avoid / demonstrate bugs in the new ones.
 ****************************************************************************/


#include "l_inc.h"

#if CE_ARTLIB_EnableSystemGrafix2D && CE_ARTLIB_BlitUseOldRoutines


void LI_BLT16BitBlt16to16(LPBYTE,int,int,int,LPBYTE,int,LPRECT);
void LI_BLT16BitBlt8to16(LPBYTE,int,int,int,LPBYTE,int,LPRECT,register LPLONG);
void LI_BLT16ColorKeyBitBlt8to16(LPBYTE,int,int,int,LPBYTE,int,LPRECT,register LPLONG);
void LI_BLT16AlphaBitBlt8to16(LPBYTE,int,int,int,LPBYTE,int,LPRECT,register LPLONG,int);
void LI_BLT16AlphaBitBlt16to16(LPBYTE,int,int,int,LPBYTE,int,LPRECT,int);
void LI_BLT16ColorBlt(LPBYTE,int,LPRECT,DWORD,BOOL);
void LI_BLT16StretchBlt8to16(LPBYTE,int,LPRECT,LPRECT,LPBYTE,int,LPRECT,register LPLONG);
void LI_BLT16ColorKeyStretchBlt8to16(LPBYTE,int,LPRECT,LPRECT,LPBYTE,int,LPRECT,register LPLONG);
void LI_BLT16AlphaStretchBlt8to16(LPBYTE,int,LPRECT,LPRECT,LPBYTE,int,LPRECT,register LPLONG,int);
void LI_BLT16ColorKeyPureGreenBitBlt16to16(LPBYTE,int,int,int,LPBYTE,int,LPRECT);

WORD LI_BLT16ConvertColorRefTo16BitColor(COLORREF);
void LI_BLT16ConvertRGBColorTableTo16BitColor(LPLONG,int);
void LI_BLT16ConvertRGBValuesTo16BitColor(LPBYTE,LPBYTE,int,int,int,int);


/*******************************************************************************
*
*   Name:         LI_BLT16BitBlt16to16
*
*   Description:  blts from a 16 bit bitmap to a 16 bit bitmap
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
void LI_BLT16BitBlt16to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;        // pointer to source scanline position to begin blt
  int nXSourceWidthInBytes;       // source bitmap width in bytes
  int nBytesAcrossToBlt;          // blt width in bytes
  int nLinesToBlt;                // blt height in pixels

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_BlitBoundsCheck
  // Check to see if blt will trample anything
  LI_BLT16BreaksBounds ( lpDestinationBits, nXDestinationWidthInPixels,
    nXBltStartCoordinate, nYBltStartCoordinate, lprSourceBltRect );
#endif

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
  nXSourceWidthInBytes = ((nXSourceWidthInPixels * 2) + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 2;

  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left * 2;

  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nBytesAcrossToBlt = (lprSourceBltRect->right - lprSourceBltRect->left) * 2;

  while (nLinesToBlt > 0)
  {
    // copy bytes for each scanline
    memcpy(lpDestinationBeginBlt,lpSourceBeginBlt,nBytesAcrossToBlt);
    // update scanline position to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt += nXSourceWidthInBytes;
    nLinesToBlt --;
  }
}


/*******************************************************************************
*
*   Name:         LI_BLT16BitBlt8to16
*
*   Description:  blts from an 8 bit paletted bitmap to a 16 bit native
*                 bitmap
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 nXBltStartCoordinate - X coord, ul corner of blt RECT
*                 nYBltStartCoordinate - Y coord, ul corner of blt RECT
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - source blt RECT
*                 lpColorTable - pointer to 8 to 16 bit color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT16BitBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  register LPLONG lpColorTable)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;        // pointer to source scanline position to begin blt
  int nXSourceWidthInBytes;       // source bitmap width in bytes
  int nPixelsAcrossToBlt;         // blt width in pixels

  register int nLinesToBlt;                 // blt height in pixels
  register int nPixelCounter;               // pixel counter
  register LPBYTE lpDestinationBltPointer;  // pointer to current blt destination
  register LPBYTE lpSourceBltPointer;       // pointer to current blt source
  register DWORD dwSourceData;              // holder for source data
  register DWORD dwDestinationData;         // holder for destination data

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_BlitBoundsCheck
  // Check to see if blt will trample anything
  LI_BLT16BreaksBounds ( lpDestinationBits, nXDestinationWidthInPixels,
    nXBltStartCoordinate, nYBltStartCoordinate, lprSourceBltRect );
#endif

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 2;

  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left;

  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

  while (nLinesToBlt > 0)
  {
    // initialize current blt pointers
    lpDestinationBltPointer = lpDestinationBeginBlt;
    lpSourceBltPointer = lpSourceBeginBlt;

    // initialize pixel counter
    nPixelCounter = nPixelsAcrossToBlt;
    while (nPixelCounter > 0)
    {
      // process four pixels at a time if possible
      if (nPixelCounter >= 4)
      {
        // get four pixels of source data
        dwSourceData = *((LPLONG)lpSourceBltPointer);
        lpSourceBltPointer += 4;

        // convert 8 bit indices to 16 bit color values
        // and store converted data at destination
        dwDestinationData = (WORD)lpColorTable[(dwSourceData & 0xff) * 2];
        dwSourceData >>= 7;
        dwDestinationData |= lpColorTable[dwSourceData & 0x1fe] << 16;
        dwSourceData >>= 8;
        *((LPLONG)lpDestinationBltPointer) = dwDestinationData;
        lpDestinationBltPointer += 4;
        dwDestinationData = (WORD)lpColorTable[dwSourceData & 0x1fe];
        dwSourceData >>= 8;
        dwDestinationData |= lpColorTable[dwSourceData & 0x1fe] << 16;
        *((LPLONG)lpDestinationBltPointer) = dwDestinationData;
        lpDestinationBltPointer += 4;
        nPixelCounter -= 4;
      }

      // process two pixels at a time if possible
      else if (nPixelCounter >= 2)
      {
        // get two pixels of source data
        dwSourceData = *((LPWORD)lpSourceBltPointer);
        lpSourceBltPointer += 2;

        // convert 8 bit indices to 16 bit color values
        // and store converted data at destination
        dwDestinationData = (WORD)lpColorTable[(dwSourceData & 0xff) * 2];
        dwSourceData >>= 7;
        dwDestinationData |= lpColorTable[dwSourceData & 0x1fe] << 16;
        *((LPLONG)lpDestinationBltPointer) = dwDestinationData;
        lpDestinationBltPointer += 4;
        nPixelCounter -= 2;
      }

      // process one pixel
      else
      {
        // get one pixel of source data
        dwSourceData = *((LPBYTE)lpSourceBltPointer);
        lpSourceBltPointer += 1;

        // convert 8 bit index to 16 bit color values
        // and store converted data at destination
        dwDestinationData = lpColorTable[(dwSourceData & 0xff) * 2];
        *((LPWORD)lpDestinationBltPointer) = (WORD)dwDestinationData;
        lpDestinationBltPointer += 2;
        nPixelCounter -= 1;
      }
    }
    // update pointers to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt += nXSourceWidthInBytes;
    nLinesToBlt --;
  }
}


/*******************************************************************************
*
*   Name:                   LI_BLT16ColorKeyBitBlt8to16
*
*   Description:  blts from an 8 bit paletted bitmap to a 16 bit native
*                 bitmap, treating the color index 0 as transparent
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 nXBltStartCoordinate - X coord, ul corner of blt RECT
*                 nYBltStartCoordinate - Y coord, ul corner of blt RECT
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - source blt RECT
*                 lpColorTable - pointer to 8 to 16 bit color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT16ColorKeyBitBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  register LPLONG lpColorTable)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;        // pointer to source scanline position to begin blt
  int nXSourceWidthInBytes;       // source bitmap width in bytes
  int nPixelsAcrossToBlt;         // blt width in pixels

  register int nLinesToBlt;                 // blt height in pixels
  register int nPixelCounter;               // pixel counter
  register LPBYTE lpDestinationBltPointer;  // pointer to current blt destination
  register LPBYTE lpSourceBltPointer;       // pointer to current blt source
  register DWORD dwSourceData;              // holder for source data

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_BlitBoundsCheck
  // Check to see if blt will trample anything
  LI_BLT16BreaksBounds ( lpDestinationBits, nXDestinationWidthInPixels,
    nXBltStartCoordinate, nYBltStartCoordinate, lprSourceBltRect );
#endif

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 2;

  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left;

  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

  while (nLinesToBlt > 0)
  {
    // initialize current blt pointers
    lpDestinationBltPointer = lpDestinationBeginBlt;
    lpSourceBltPointer = lpSourceBeginBlt;

    // initialize pixel counter
    nPixelCounter = nPixelsAcrossToBlt;
    while (nPixelCounter > 0)
    {
      // process four pixels at a time if possible
      if (nPixelCounter >= 4)
      {
        // get four pixels of source data
        dwSourceData = *((LPLONG)lpSourceBltPointer);
        lpSourceBltPointer += 4;

        // convert 8 bit indices to 16 bit color values
        // and store converted data at destination if not zero
        if ((dwSourceData & 0xff) != 0)
          *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[(dwSourceData & 0xff) * 2];
        lpDestinationBltPointer += 2;
        dwSourceData >>= 7;

        if ((dwSourceData & 0x1fe) != 0)
          *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[dwSourceData & 0x1fe];
        lpDestinationBltPointer += 2;
        dwSourceData >>= 8;

        if ((dwSourceData & 0x1fe) != 0)
          *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[dwSourceData & 0x1fe];
        lpDestinationBltPointer += 2;
        dwSourceData >>= 8;

        if ((dwSourceData & 0x1fe) != 0)
          *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[dwSourceData & 0x1fe];
        lpDestinationBltPointer += 2;
        nPixelCounter -= 4;
      }

      // process two pixels at a time if possible
      else if (nPixelCounter >= 2)
      {
        // get two pixels of source data
        dwSourceData = *((LPWORD)lpSourceBltPointer);
        lpSourceBltPointer += 2;

        // convert 8 bit indices to 16 bit color values
        // and store converted data at destination if not zero
        if ((dwSourceData & 0xff) != 0)
          *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[(dwSourceData & 0xff) * 2];
        lpDestinationBltPointer += 2;
        dwSourceData >>= 7;

        if ((dwSourceData & 0x1fe) != 0)
          *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[dwSourceData & 0x1fe];
        lpDestinationBltPointer += 2;
        nPixelCounter -= 2;
      }

      // process one pixel
      else
      {
        // get one pixel of source data
        dwSourceData = *((LPBYTE)lpSourceBltPointer);
        lpSourceBltPointer += 1;

        // convert 8 bit index to 16 bit color values
        // and store converted data at destination if not zero
        if ((dwSourceData & 0xff) != 0)
          *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[(dwSourceData & 0xff) * 2];
        lpDestinationBltPointer += 2;
        nPixelCounter -= 1;
      }
    }
    // update pointers to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt += nXSourceWidthInBytes;
    nLinesToBlt --;
  }
}


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

/*  The following is the old C code. */
/*
void LI_BLT16AlphaBitBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  register LPLONG lpColorTable,
  int nAlphaInColorTable)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;        // pointer to source scanline position to begin blt
  int nXSourceWidthInBytes;       // source bitmap width in bytes
  int nPixelsAcrossToBlt;         // blt width in pixels

  register int nLinesToBlt;                 // blt height in pixels
  register int nPixelCounter;               // pixel counter
  register LPBYTE lpDestinationBltPointer;  // pointer to current blt destination
  register LPBYTE lpSourceBltPointer;       // pointer to current blt source
  register WORD wSourceData;                // holder for source data
  register WORD wDestinationData;           // holder for destination data
  register WORD wAlphaData;                 // holder source and data blended
  register BYTE cData;                      // 8 bit source index
  register BYTE cAlpha;                     // 8 bit alpha channel

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 2;

  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left;

  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

  while (nLinesToBlt > 0)
  {
    // initialize current blt pointers
    lpDestinationBltPointer = lpDestinationBeginBlt;
    lpSourceBltPointer = lpSourceBeginBlt;

    // initialize pixel counter
    nPixelCounter = nPixelsAcrossToBlt;
    while (nPixelCounter > 0)
    {
      // get one pixel of source data
      cData = *((LPBYTE)lpSourceBltPointer);
      lpSourceBltPointer ++;

      // convert 8 bit index to 32 bit color value
      // and store converted data at destination if not zero
      if (cData != 0)
      {
        if (cData >= nAlphaInColorTable)
        {
          *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[cData * 2];
        }
        else
        {
          cAlpha = (BYTE)lpColorTable[(cData * 2) + 1];
          wDestinationData = (WORD)lpColorTable[cData * 2];
          wSourceData = *((LPWORD)lpDestinationBltPointer);
          if (cAlpha == ALPHA_OPAQUE06_25)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + (wSourceData & LE_BLT_dwRedMask) - ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + (wSourceData & LE_BLT_dwGreenMask) - ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + (wSourceData & LE_BLT_dwBlueMask) - ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE12_50)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + (wSourceData & LE_BLT_dwRedMask) - ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + (wSourceData & LE_BLT_dwGreenMask) - ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + (wSourceData & LE_BLT_dwBlueMask) - ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE18_75)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE25_00)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE31_25)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 3) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 3) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 3) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE37_50)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE43_75)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE50_00)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE56_25)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) - ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) - ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) - ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE62_50)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE68_75)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE75_00)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE81_25)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 3) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 3) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 3) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE87_50)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE93_75)
          {
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }

          *((LPWORD)lpDestinationBltPointer) = wAlphaData;
        }
      }
      lpDestinationBltPointer += 2;
      nPixelCounter --;
    }
    // update pointers to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt += nXSourceWidthInBytes;
    nLinesToBlt --;
  }
}
*/

    /* Here is the new inline assembly version. */

void LI_BLT16AlphaBitBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  register LPLONG lpColorTable,
  int nAlphaInColorTable)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;        // pointer to source scanline position to begin blt
  int nXSourceWidthInBytes;       // source bitmap width in bytes
  int nPixelsAcrossToBlt;         // blt width in pixels
  int nLinesToBlt;                  // blt height in pixels
    static DWORD  jump_array[17];       // Keeps offsets to alpha routines we jump to
    static BOOL  set_offsets = FALSE;   // Keeps track of whether we set the jumps or not
    DWORD  *jump_array_point;           // Can't get the location of jump_array in assembly.
                                        // Hacking my way around it.

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_BlitBoundsCheck
  // Check to see if blt will trample anything
  LI_BLT16BreaksBounds ( lpDestinationBits, nXDestinationWidthInPixels,
    nXBltStartCoordinate, nYBltStartCoordinate, lprSourceBltRect );
#endif

  // Run the MMX version of the routine if enabled.
#if CE_ARTLIB_EnableSystemMMX
    if(LE_MMX_Is_Available())
    {
        LI_MMXAlphaBitBlt8to16(lpDestinationBits, nXDestinationWidthInPixels,
                                    nXBltStartCoordinate, nYBltStartCoordinate,
                                    lpSourceBits, nXSourceWidthInPixels,
                                    lprSourceBltRect, lpColorTable,
                                    nAlphaInColorTable);
        return;
    }
#endif

    // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 2;

  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left;

  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

    // Need this to look up the jump array in assembly code.
    jump_array_point = jump_array;

    // Assembly inline code for writing the pixels.
    __asm
    {
        // First, save registers we'll be messing up that would not normally
        //   be messed up.

        push ESI
        push EDI

        cmp byte ptr [set_offsets], 0
        jnz StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov eax, jump_array_point
            mov ebx, 4
            mov dword ptr [eax], OFFSET Write_Alpha_Data
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add eax, ebx
            mov dword ptr [eax], OFFSET Write_Alpha_Data

            // Make sure we remember that the offsets have been set.
            mov set_offsets, 1

StartRoutine:
        // We need to set up the loops.
        mov ecx, dword Ptr [nLinesToBlt]

ScanLineLoop:
        cmp ecx, 1
        jl  EndOfBlit

            // Subtract one from the lines we have left.
            dec ecx

            // Hold destination in DI, source in SI
            mov esi, dword ptr [lpSourceBeginBlt]
            push ecx        // Push here for efficiency
            mov edi, dword ptr [lpDestinationBeginBlt]

            // Use ECX as a counter of how many pixels we
            // have written. Reset it for the next line.
            xor ecx, ecx

        StartLineBlit:
                cmp ecx, dword ptr [nPixelsAcrossToBlt]
                jge EndOfLineBlit

                // Get the next pixels, if we need to.
        test  ecx, 03h
                jz  GetNextPixel
                jmp DontGetNextPixels

        GetNextPixel:
                // Get the next 4 pixels.
                mov edx, dword ptr [esi]
                add esi, 4

        DontGetNextPixels:
                // Move the next pixel into al.
                mov eax, edx
                ror edx, 8

                // Mask the lower 8 bits - that is the next pixel.
                and eax, 0ffh

                // Check to make sure it is not transparent.
                cmp eax, 0
                jz FinishedWithPixel

                    // Now check to see that the pixel is not greater than nAlphaInColorTable.
                    cmp eax, dword ptr[nAlphaInColorTable]
                    jl  CheckForAlphaModes

                        // Not special - just look up in color table.
                        shl eax, 3   // Multiply by 8.
                        mov ebx, dword ptr [lpColorTable]
                        add ebx, eax

                        // Now copy a word over.
                        mov eax, dword ptr [ebx]
                        mov [edi], ax

                        inc edi         /* Repeating this code here for speed. */
                        inc ecx
                        inc edi
                        jmp StartLineBlit

            CheckForAlphaModes:
                        // Get the alpha pixel, and set up for alpha tests.
                        shl eax, 3
                        add eax, dword ptr [lpColorTable]

                        push ecx    // Clear up cx, need it!

                        // Load up the alpha byte in al, destination word in bx,
                        // and source word in cx.
                        mov ecx, eax
                        mov ebx, dword ptr [ecx]
                        add ecx, 4
                        and ebx, 0ffffh

                        xor eax, eax
                        mov eax, dword ptr [ecx]
                        mov ecx, dword ptr [edi]
                        and eax, 0ffh
                        and ecx, 0ffffh

                        // Jump to the appropriate function
                        shr eax, 4
                        inc eax
                        shl eax, 2
                        add eax, jump_array_point
                        mov eax, dword ptr [eax]
                        jmp eax

                DO_ALPHA_OPAQUE06_25:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            add eax, ecx
                            shr ecx, 4
                            sub eax, ecx

                            pop ecx // Pop placed here for pairing

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            add eax, ecx
                            shr ecx, 4
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            add eax, ecx
                            shr ecx, 4
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE12_50:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE18_75:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE25_00:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE31_25:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE37_50:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE43_75:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE50_00:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE56_25:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE62_50:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE68_75:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE75_00:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 2
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE81_25:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 3
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 3
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 3
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE87_50:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 3
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 3
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 3
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE93_75:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 4
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 4
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
                            shr ecx, 4
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.


        Write_Alpha_Data:
                      // Write the new pixel.
                    mov word ptr [edi], ax
                    pop ecx

    FinishedWithPixel:
            inc edi
            inc ecx
            inc edi
            jmp StartLineBlit

EndOfLineBlit:
        mov eax, dword ptr [nXDestinationWidthInBytes]
        add dword ptr [lpDestinationBeginBlt], eax

        mov eax, dword ptr [nXSourceWidthInBytes]
        add dword ptr [lpSourceBeginBlt], eax

        pop ecx // Restore the loop count.
        jmp ScanLineLoop

EndOfBlit:

        // Restore the registers we messed up.
        pop EDI
        pop ESI

    };

}


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
/* Original C version
void LI_BLT16AlphaBitBlt16to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;        // pointer to source scanline position to begin blt
  int nXSourceWidthInBytes;       // source bitmap width in bytes
  int nPixelsAcrossToBlt;         // blt width in pixels

  register int nLinesToBlt;                 // blt height in pixels
  register int nPixelCounter;               // pixel counter
  register LPBYTE lpDestinationBltPointer;  // pointer to current blt destination
  register LPBYTE lpSourceBltPointer;       // pointer to current blt source
  register WORD wSourceData;                // holder for source data
  register WORD wDestinationData;           // holder for destination data
  register WORD wAlphaData;                 // holder source and data blended
  register WORD wPureGreen;

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
  nXSourceWidthInBytes = ((nXSourceWidthInPixels * 2) + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 2;

  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left * 2;

  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

  wPureGreen = LI_BLT16ConvertColorRefTo16BitColor(LE_GRAFIX_MakeColorRef(0,255,0));

  while (nLinesToBlt > 0)
  {
    // initialize current blt pointers
    lpDestinationBltPointer = lpDestinationBeginBlt;
    lpSourceBltPointer = lpSourceBeginBlt;

    // initialize pixel counter
    nPixelCounter = nPixelsAcrossToBlt;
    while (nPixelCounter > 0)
    {
      // get one pixel of source data
      wDestinationData = *((LPWORD)lpSourceBltPointer);
      lpSourceBltPointer += 2;
      if (wDestinationData != wPureGreen)
      {
        wSourceData = *((LPWORD)lpDestinationBltPointer);
        switch (nAlphaValue)
        {
          case ALPHA_OPAQUE06_25:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 4) + (wSourceData & LE_BLT_dwRedMask) - ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 4) + (wSourceData & LE_BLT_dwGreenMask) - ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 4) + (wSourceData & LE_BLT_dwBlueMask) - ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE12_50:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 3) + (wSourceData & LE_BLT_dwRedMask) - ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 3) + (wSourceData & LE_BLT_dwGreenMask) - ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 3) + (wSourceData & LE_BLT_dwBlueMask) - ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE18_75:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 3) + ((wDestinationData & LE_BLT_dwRedMask) >> 4) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 3) + ((wDestinationData & LE_BLT_dwGreenMask) >> 4) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 3) + ((wDestinationData & LE_BLT_dwBlueMask) >> 4) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE25_00:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE31_25:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 2) + ((wDestinationData & LE_BLT_dwRedMask) >> 4) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 3) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 2) + ((wDestinationData & LE_BLT_dwGreenMask) >> 4) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 3) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 2) + ((wDestinationData & LE_BLT_dwBlueMask) >> 4) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 3) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE37_50:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 2) + ((wDestinationData & LE_BLT_dwRedMask) >> 3) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 2) + ((wDestinationData & LE_BLT_dwGreenMask) >> 3) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 2) + ((wDestinationData & LE_BLT_dwBlueMask) >> 3) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE43_75:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 1) - ((wDestinationData & LE_BLT_dwRedMask) >> 4) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 1) - ((wDestinationData & LE_BLT_dwGreenMask) >> 4) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 1) - ((wDestinationData & LE_BLT_dwBlueMask) >> 4) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE50_00:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 1)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 1)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 1)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE56_25:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 1) + ((wDestinationData & LE_BLT_dwRedMask) >> 4) + ((wSourceData & LE_BLT_dwRedMask) >> 1) - ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 1) + ((wDestinationData & LE_BLT_dwGreenMask) >> 4) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) - ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 1) + ((wDestinationData & LE_BLT_dwBlueMask) >> 4) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) - ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE62_50:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 1) + ((wDestinationData & LE_BLT_dwRedMask) >> 3) + ((wSourceData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 1) + ((wDestinationData & LE_BLT_dwGreenMask) >> 3) + ((wSourceData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 1) + ((wDestinationData & LE_BLT_dwBlueMask) >> 3) + ((wSourceData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE68_75:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 1) + ((wDestinationData & LE_BLT_dwRedMask) >> 3) + ((wDestinationData & LE_BLT_dwRedMask) >> 4) + ((wSourceData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 1) + ((wDestinationData & LE_BLT_dwGreenMask) >> 3) + ((wDestinationData & LE_BLT_dwGreenMask) >> 4) + ((wSourceData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 1) + ((wDestinationData & LE_BLT_dwBlueMask) >> 3) + ((wDestinationData & LE_BLT_dwBlueMask) >> 4) + ((wSourceData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE75_00:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 1) + ((wDestinationData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 1) + ((wDestinationData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 1) + ((wDestinationData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE81_25:
            wAlphaData = (((wDestinationData & LE_BLT_dwRedMask) >> 1) + ((wDestinationData & LE_BLT_dwRedMask) >> 2) + ((wDestinationData & LE_BLT_dwRedMask) >> 4) + ((wSourceData & LE_BLT_dwRedMask) >> 3) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwGreenMask) >> 1) + ((wDestinationData & LE_BLT_dwGreenMask) >> 2) + ((wDestinationData & LE_BLT_dwGreenMask) >> 4) + ((wSourceData & LE_BLT_dwGreenMask) >> 3) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= (((wDestinationData & LE_BLT_dwBlueMask) >> 1) + ((wDestinationData & LE_BLT_dwBlueMask) >> 2) + ((wDestinationData & LE_BLT_dwBlueMask) >> 4) + ((wSourceData & LE_BLT_dwBlueMask) >> 3) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE87_50:
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) - ((wDestinationData & LE_BLT_dwRedMask) >> 3) + ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) - ((wDestinationData & LE_BLT_dwGreenMask) >> 3) + ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) - ((wDestinationData & LE_BLT_dwBlueMask) >> 3) + ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE93_75:
            wAlphaData = ((wDestinationData & LE_BLT_dwRedMask) - ((wDestinationData & LE_BLT_dwRedMask) >> 4) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwGreenMask) - ((wDestinationData & LE_BLT_dwGreenMask) >> 4) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            wAlphaData |= ((wDestinationData & LE_BLT_dwBlueMask) - ((wDestinationData & LE_BLT_dwBlueMask) >> 4) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE100_0:
            wAlphaData = wDestinationData;
            break;
          default:
            wAlphaData = wSourceData;
            break;
        }
        *((LPWORD)lpDestinationBltPointer) = wAlphaData;
      }
      lpDestinationBltPointer += 2;
      nPixelCounter --;
    }
    // update pointers to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt += nXSourceWidthInBytes;
    nLinesToBlt --;
  }
}
//End Original C version
*/
/* New ASM version. */

void LI_BLT16AlphaBitBlt16to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue)
{
  LPBYTE  lpDestinationBeginBlt;    //destination Scan Line Pointer
  int   nXDestinationWidthInBytes;  //destination Bitmap Width
  LPBYTE  lpSourceBeginBlt;   //source Scan Line Pointer
  int   nXSourceWidthInBytes;   //source Bitmap Width
  int   nPixelsAcrossToBlt;   //# pixels across to blt
  int nLinesToBlt;      //# lines down to blt
  int   wPureGreen;     //Pure green color cache
  static BOOL set_offsets = FALSE;  //Check to see if offsets set
  static DWORD  jump_array[17];   //Keeps offsets to alpha
            //routines we jump to
  DWORD * jump_array_point;   //So inline assembly can find
            //the Jump Array

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_BlitBoundsCheck
  // Check to see if blt will trample anything
  LI_BLT16BreaksBounds ( lpDestinationBits, nXDestinationWidthInPixels,
    nXBltStartCoordinate, nYBltStartCoordinate, lprSourceBltRect );
#endif

  // Run the MMX version of the routine if enabled.

#if CE_ARTLIB_EnableSystemMMX
    if(LE_MMX_Is_Available())
    {
        LI_MMXAlphaBitBlt16to16(lpDestinationBits,
                nXDestinationWidthInPixels,
                nXBltStartCoordinate,
                nYBltStartCoordinate,
                lpSourceBits,
                nXSourceWidthInPixels,
                lprSourceBltRect,
                nAlphaValue);
        return;
    }
#endif

  wPureGreen = LI_BLT16ConvertColorRefTo16BitColor(
      LE_GRAFIX_MakeColorRef( 0, 255, 0 ));
  //Setup variables
  nXDestinationWidthInBytes = (( nXDestinationWidthInPixels * 2 ) + 3 )
          & 0xFFFFFFFC;
  nXSourceWidthInBytes = (( nXSourceWidthInPixels * 2 ) + 3 )
          & 0xFFFFFFFC;
  jump_array_point = jump_array;
  //Source and destination widths are built like this because they are
  //DIBitmaps and are aligned along 4 byte boundaries...
  lpDestinationBeginBlt = lpDestinationBits +
        ( nYBltStartCoordinate *
        nXDestinationWidthInBytes ) +
        ( nXBltStartCoordinate * 2 );
  lpSourceBeginBlt = lpSourceBits +
        ( lprSourceBltRect->top *
        nXSourceWidthInBytes ) +
        ( lprSourceBltRect->left * 2 );
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;
  __asm
  {
  //AG: Code borrowed from the 8 -> 16 Alpha blt. (thanks AS)
        // First, save registers we'll be messing up that would not normally
        //   be messed up.

        push ESI
        push EDI

        cmp byte ptr [set_offsets], 0
        jnz StartRoutine

        // Set up a jump array for alpha mode.  We'll make it static,
        // so won't have to set it twice.
            mov eax, jump_array_point
            mov ebx, 4
            mov dword ptr [eax], OFFSET Write_Alpha_Data
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE06_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE12_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE18_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE25_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE31_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE37_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE43_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE50_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE56_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE62_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE68_75
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE75_00
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE81_25
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE87_50
            add eax, ebx
            mov dword ptr [eax], OFFSET DO_ALPHA_OPAQUE93_75
            add eax, ebx
            mov dword ptr [eax], OFFSET Write_Alpha_Data

            // Make sure we remember that the offsets have been set.
            mov set_offsets, 1

StartRoutine:
        // We need to set up the loops.
        mov ecx, dword Ptr [nLinesToBlt]

ScanLineLoop:
  //Jump and compare here for jump prediction (follow thru)
        cmp ecx, 1
        jl  EndOfBlit

            // Subtract one from the lines we have left.
            dec ecx

            // Hold destination in DI, source in SI
            mov esi, dword ptr [lpSourceBeginBlt]
            push ecx        // Push here for efficiency
            mov edi, dword ptr [lpDestinationBeginBlt]

            // Use ECX as a counter of how many pixels we
            // have written. Reset it for the next line.
            xor ecx, ecx

        StartLineBlit:
                cmp ecx, dword ptr [nPixelsAcrossToBlt]
                jge EndOfLineBlit

                // Get the next pixels, if we need to.
        test  ecx, 01h //AG
    //We are getting two pixels every second go round
                jz  GetNextPixel
                jmp DontGetNextPixels

        GetNextPixel:
                // Get the next 2 pixels.
                mov edx, dword ptr [esi]
                add esi, 4

        DontGetNextPixels:
                // Move the next pixel into ax.
                mov eax, edx
                ror edx, 16

        // Mask the lower 16 bits - that is the next pixel.
                and eax, 0FFFFh //AG

                // Check to make sure it is not transparent.
                cmp eax, wPureGreen
                jz FinishedWithPixel
        //To recap....nAlphaValue is the alpha value...AX has pixel
        //value. For the following to work....the nAlphaValue has to be
        //in eax and the pixel value in ebx...soooo...

        mov ebx, eax
        mov eax, nAlphaValue
        //We still have to get the destination pixel into CX so grab ECX and
        //mask it.
        push ecx
        mov ecx, dword ptr [edi]
        and ecx, 0FFFFh

                // Jump to the appropriate function
        shr eax, 4
        inc eax
        shl eax, 2
        add eax, jump_array_point
        mov eax, dword ptr [eax]
        jmp eax

                DO_ALPHA_OPAQUE06_25:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 4
              mov eax, ebx
                            add eax, ecx
                            shr ecx, 4
                            sub eax, ecx

                            pop ecx // Pop placed here for pairing

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 4
                            mov eax, ebx
                            add eax, ecx
                            shr ecx, 4
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            shr ebx, 4
              mov eax, ebx
                            add eax, ecx
                            shr ecx, 4
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE12_50:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 3
                            mov eax, ebx
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 3
                            mov eax, ebx
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 3
                            mov eax, ebx
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE18_75:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 3
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 3
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 3
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE25_00:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 2
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 2
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 2
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE31_25:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 2
                            mov eax, ebx
              shr ebx, 2
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 2
                            mov eax, ebx
              shr ebx, 2
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 2
                            mov eax, ebx
              shr ebx, 2
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE37_50:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 2
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 2
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 2
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE43_75:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 3
              sub eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 3
              sub eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 3
              sub eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE50_00:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE56_25:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 3
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 3
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 3
              add eax, ebx
                            shr ecx, 1
                            add eax, ecx
                            shr ecx, 3
                            sub eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE62_50:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 2
              add eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 2
              add eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 2
              add eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE68_75:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 2
              add eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 2
              add eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 2
              add eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 2
                            add eax, ecx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE75_00:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 2
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 2
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE81_25:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 3
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 3
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
              and edx, 0FFFFh
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
              shr ebx, 1
                            mov eax, ebx
              shr ebx, 1
              add eax, ebx
                            shr ecx, 3
                            add eax, ecx
                            shr ecx, 1
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE87_50:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
              shr ebx, 3
              sub eax, ebx
                            shr ecx, 3
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
              shr ebx, 3
              sub eax, ebx
                            shr ecx, 3
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
              shr ebx, 3
              sub eax, ebx
                            shr ecx, 3
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.

                DO_ALPHA_OPAQUE93_75:
                            push edx    // Need another register!

                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwRedMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
              shr ebx, 4
              sub eax, ebx
                            shr ecx, 4
                            add eax, ecx

                            pop ecx // Pop placed here for pairing/

                            // Mask the result.
                            and eax, edx

                            pop ebx
                            push eax     // save the result.

                            // Now do green.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwGreenMask]
                            push ebx
                            and edx, 0ffffh
                            push ecx

                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
              shr ebx, 4
              sub eax, ebx
                            shr ecx, 4
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop ecx
                            pop ebx
                            pop edx
                            or eax, edx
                            push eax     // save the result.

                            // finally, do blue.
                            // Mask 'em.
                            mov edx, dword ptr [LE_BLT_dwBlueMask]
                            and ebx, edx
                            and ecx, edx

                            // Add 'em.
                            mov eax, ebx
              shr ebx, 4
              sub eax, ebx
                            shr ecx, 4
                            add eax, ecx

                            // Mask the result.
                            and eax, edx

                            pop edx  // Pop the last result in dx.
                            or eax, edx   // Or it into our result, and we are done!
                            pop edx     // Restore edx before this calculation.

                            jmp Write_Alpha_Data    // Write the word.


        Write_Alpha_Data:
                      // Write the new pixel.
                    mov word ptr [edi], ax
                    pop ecx

    FinishedWithPixel:
            inc edi
            inc ecx
            inc edi
            jmp StartLineBlit

EndOfLineBlit:
        mov eax, dword ptr [nXDestinationWidthInBytes]
        add dword ptr [lpDestinationBeginBlt], eax

        mov eax, dword ptr [nXSourceWidthInBytes]
        add dword ptr [lpSourceBeginBlt], eax

        pop ecx // Restore the loop count.
        jmp ScanLineLoop

EndOfBlit:

        // Restore the registers we messed up.
        pop EDI
        pop ESI
  };
}


/*******************************************************************************
*
*   Name:                   LI_BLT16ColorBlt
*
*   Description:  fills a destination RECT with a given color
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 lprDestinationBltRect - destination blt RECT
*                 crColorToFillWith - color to fill with, may be COLORREF
*                                     or may be 16 bit color
*                 bColorRefIs16BitColor - flag: given color is 16 bit color
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT16ColorBlt(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  LPRECT lprDestinationBltRect,
  DWORD crColorToFillWith,
  BOOL bColorRefIs16BitColor)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  int nPixelsAcrossToBlt;         // blt width in pixels

  register LPBYTE lpDestinationBltPointer;  // pointer to current blt destination
  register int nLinesToBlt;                 // blt height in pixels
  register int nPixelCounter;               // pixel counter



  // convert COLORREF to 16 bit color (if necessary)
  if (!bColorRefIs16BitColor)
    crColorToFillWith = LI_BLT16ConvertColorRefTo16BitColor(crColorToFillWith);
  // duplicate 16 bit color in upper word of LONG
  crColorToFillWith &= 0xffff;
  crColorToFillWith |= crColorToFillWith << 16;

  // calculate destination bitmap width in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                    (lprDestinationBltRect->top * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += lprDestinationBltRect->left * 2;

  // calculate dimensions of blt
  nLinesToBlt = lprDestinationBltRect->bottom - lprDestinationBltRect->top;
  nPixelsAcrossToBlt = lprDestinationBltRect->right - lprDestinationBltRect->left;
  while (nLinesToBlt > 0)
  {
    // initialize current blt pointer
    lpDestinationBltPointer = lpDestinationBeginBlt;

    // initialize pixel counter
    nPixelCounter = nPixelsAcrossToBlt;
    while (nPixelCounter > 0)
    {
      // fill two pixels at once if possible
      if (nPixelCounter >= 2)
      {
        *((LPLONG)lpDestinationBltPointer) = crColorToFillWith;
        lpDestinationBltPointer += 4;
        nPixelCounter -= 2;
      }
      // fill one pixel
      else
      {
        *((LPWORD)lpDestinationBltPointer) = (WORD)crColorToFillWith;
        lpDestinationBltPointer += 2;
        nPixelCounter -= 1;
      }
    }
    // update pointer to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    nLinesToBlt --;
  }
}


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
*                 lprDestinationClipRect - destination clip RECT
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
  register LPLONG lpColorTable)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;        // pointer to start of blt RECT in source bitmap
                                  // without clipping
  LPBYTE lpSourceBeginLineBlt;    // pointer to start of scanline to blt, with clipping
  int nXSourceWidthInBytes;       // source bitmap width in bytes
  int nPixelsAcrossToBlt;         // destination blt width in pixels
  int nLinesToBlt;                // destination blt height in pixels

  register LPBYTE lpDestinationBltPointer;  // current destination blt pointer
  register LPBYTE lpSourceBltPointer;       // current source blt pointer
  register int nPixelCounter;               // destination pixel counter
  register int nLinesCounter;               // destination lines counter
  register int nFractionalLines;            // 16:16 fractional lines,
                                            // amount to increment source pointer
                                            // for every line in destination
  register int nFractionalPixels;           // 16:16 fractional pixels,
                                            // amount to increment source pointer
                                            // for every pixel in destination
  register int nTotalFractionalLines;       // 16:16 fractional line total so far
  register int nTotalFractionalPixels;      // 16:16 fractional pixel total so far
  register BYTE cData;                      // bitmap data byte

  // calculate 16:16 fractional ratio between destination and source scanlines
  nFractionalLines = (lprSourceBltRect->bottom - lprSourceBltRect->top) - 1;
  nFractionalLines <<= 16;
  nFractionalLines /= (lprDestinationBltRect->bottom - lprDestinationBltRect->top) - 1;

  // calculate 16:16 fractional ratio between destination and source scan widths
  nFractionalPixels = (lprSourceBltRect->right - lprSourceBltRect->left) - 1;
  nFractionalPixels <<= 16;
  nFractionalPixels /= (lprDestinationBltRect->right - lprDestinationBltRect->left) - 1;

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap using clip RECT
  lpDestinationBeginBlt = lpDestinationBits +
                          (lprDestinationClipRect->top * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += lprDestinationClipRect->left * 2;

  // calculate start position of blt in source bitmap WITHOUT clipping
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left;

  // calculate dimensions of clipped blt
  nLinesToBlt = lprDestinationClipRect->bottom - lprDestinationClipRect->top;
  nPixelsAcrossToBlt = lprDestinationClipRect->right - lprDestinationClipRect->left;

  // calculate total fractional lines including clipping
  nTotalFractionalLines = lprDestinationClipRect->top - lprDestinationBltRect->top;
  nTotalFractionalLines *= nFractionalLines;

  // calculate start of source line, with Y clipping
  lpSourceBeginLineBlt = lpSourceBeginBlt;
  lpSourceBeginLineBlt += HIWORD(nTotalFractionalLines + 0x8000) * nXSourceWidthInBytes;
  for (nLinesCounter = 0; nLinesCounter < nLinesToBlt; nLinesCounter ++)
  {
    // initialize destination blt pointer
    lpDestinationBltPointer = lpDestinationBeginBlt;

    // calculate total fractional pixels including clipping
    nTotalFractionalPixels = lprDestinationClipRect->left - lprDestinationBltRect->left;
    nTotalFractionalPixels *= nFractionalPixels;

    // initialize source blt pointer including clipping
    lpSourceBltPointer = lpSourceBeginLineBlt;
    lpSourceBltPointer += HIWORD(nTotalFractionalPixels + 0x8000);
    for (nPixelCounter = 0; nPixelCounter < nPixelsAcrossToBlt; nPixelCounter ++)
    {
      // move source data to destination
      cData = *((LPBYTE)lpSourceBltPointer);
      *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[cData * 2];

      // update destination pointer to next destination pixel
      lpDestinationBltPointer += 2;

      // update source pointer using fractional pixels value
      nTotalFractionalPixels += nFractionalPixels;
      lpSourceBltPointer = lpSourceBeginLineBlt;
      lpSourceBltPointer += HIWORD(nTotalFractionalPixels + 0x8000);
    }
    // update destination pointer to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;

    // update source pointer using fractional scanlines value
    nTotalFractionalLines += nFractionalLines;
    lpSourceBeginLineBlt = lpSourceBeginBlt;
    lpSourceBeginLineBlt += HIWORD(nTotalFractionalLines + 0x8000) * nXSourceWidthInBytes;
  }
}


/*******************************************************************************
*
*   Name:         LI_BLT16ColorKeyStretchBlt8to16
*
*   Description:  stretch blts an 8 bit paletted source bitmap to a 16 bit
*                 destination bitmap with clipping, pixel value 0 is considered
*                 to be transparent
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 lprDestinationBltRect - destination blt RECT without clipping
*                                         used to determine amount to stretch
*                                         in X and Y
*                 lprDestinationClipRect - destination clip RECT
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
void LI_BLT16ColorKeyStretchBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  LPRECT lprDestinationBltRect,
  LPRECT lprDestinationClipRect,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  register LPLONG lpColorTable)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;        // pointer to start of blt RECT in source bitmap
                                  // without clipping
  LPBYTE lpSourceBeginLineBlt;    // pointer to start of scanline to blt, with clipping
  int nXSourceWidthInBytes;       // source bitmap width in bytes
  int nPixelsAcrossToBlt;         // destination blt width in pixels
  int nLinesToBlt;                // destination blt height in pixels

  register LPBYTE lpDestinationBltPointer;  // current destination blt pointer
  register LPBYTE lpSourceBltPointer;       // current source blt pointer
  register int nPixelCounter;               // destination pixel counter
  register int nLinesCounter;               // destination lines counter
  register int nFractionalLines;            // 16:16 fractional lines,
                                            // amount to increment source pointer
                                            // for every line in destination
  register int nFractionalPixels;           // 16:16 fractional pixels,
                                            // amount to increment source pointer
                                            // for every pixel in destination
  register int nTotalFractionalLines;       // 16:16 fractional line total so far
  register int nTotalFractionalPixels;      // 16:16 fractional pixel total so far
  register BYTE cData;                      // bitmap data byte

  // calculate 16:16 fractional ratio between destination and source scanlines
  nFractionalLines = (lprSourceBltRect->bottom - lprSourceBltRect->top) - 1;
  nFractionalLines <<= 16;
  nFractionalLines /= (lprDestinationBltRect->bottom - lprDestinationBltRect->top) - 1;

  // calculate 16:16 fractional ratio between destination and source scan widths
  nFractionalPixels = (lprSourceBltRect->right - lprSourceBltRect->left) - 1;
  nFractionalPixels <<= 16;
  nFractionalPixels /= (lprDestinationBltRect->right - lprDestinationBltRect->left) - 1;

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap using clip RECT
  lpDestinationBeginBlt = lpDestinationBits +
                          (lprDestinationClipRect->top * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += lprDestinationClipRect->left * 2;

  // calculate start position of blt in source bitmap WITHOUT clipping
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left;

  // calculate dimensions of clipped blt
  nLinesToBlt = lprDestinationClipRect->bottom - lprDestinationClipRect->top;
  nPixelsAcrossToBlt = lprDestinationClipRect->right - lprDestinationClipRect->left;

  // calculate total fractional lines including clipping
  nTotalFractionalLines = lprDestinationClipRect->top - lprDestinationBltRect->top;
  nTotalFractionalLines *= nFractionalLines;

  // calculate start of source line, with Y clipping
  lpSourceBeginLineBlt = lpSourceBeginBlt;
  lpSourceBeginLineBlt += HIWORD(nTotalFractionalLines + 0x8000) * nXSourceWidthInBytes;
  for (nLinesCounter = 0; nLinesCounter < nLinesToBlt; nLinesCounter ++)
  {
    // initialize destination blt pointer
    lpDestinationBltPointer = lpDestinationBeginBlt;

    // calculate total fractional pixels including clipping
    nTotalFractionalPixels = lprDestinationClipRect->left - lprDestinationBltRect->left;
    nTotalFractionalPixels *= nFractionalPixels;

    // initialize source blt pointer including clipping
    lpSourceBltPointer = lpSourceBeginLineBlt;
    lpSourceBltPointer += HIWORD(nTotalFractionalPixels + 0x8000);
    for (nPixelCounter = 0; nPixelCounter < nPixelsAcrossToBlt; nPixelCounter ++)
    {
      // only move source data if source byte is non zero
      cData = *((LPBYTE)lpSourceBltPointer);
      if (cData != 0)
        *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[cData * 2];

      // update destination pointer to next destination pixel
      lpDestinationBltPointer += 2;

      // update source pointer using fractional pixels value
      nTotalFractionalPixels += nFractionalPixels;
      lpSourceBltPointer = lpSourceBeginLineBlt;
      lpSourceBltPointer += HIWORD(nTotalFractionalPixels + 0x8000);
    }
    // update destination pointer to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;

    // update source pointer using fractional scanlines value
    nTotalFractionalLines += nFractionalLines;
    lpSourceBeginLineBlt = lpSourceBeginBlt;
    lpSourceBeginLineBlt += HIWORD(nTotalFractionalLines + 0x8000) * nXSourceWidthInBytes;
  }
}


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
*                 lprDestinationClipRect - destination clip RECT
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
  int nAlphaInColorTable)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;        // pointer to start of blt RECT in source bitmap
                                  // without clipping
  LPBYTE lpSourceBeginLineBlt;    // pointer to start of scanline to blt, with clipping
  int nXSourceWidthInBytes;       // source bitmap width in bytes
  int nPixelsAcrossToBlt;         // destination blt width in pixels
  int nLinesToBlt;                // destination blt height in pixels

  register LPBYTE lpDestinationBltPointer;  // current destination blt pointer
  register LPBYTE lpSourceBltPointer;       // current source blt pointer
  register int nPixelCounter;               // destination pixel counter
  register int nLinesCounter;               // destination lines counter
  register int nFractionalLines;            // 16:16 fractional lines,
                                            // amount to increment source pointer
                                            // for every line in destination
  register int nFractionalPixels;           // 16:16 fractional pixels,
                                            // amount to increment source pointer
                                            // for every pixel in destination
  register int nTotalFractionalLines;       // 16:16 fractional line total so far
  register int nTotalFractionalPixels;      // 16:16 fractional pixel total so far
  register WORD wSourceData;                // holder for source data
  register WORD wDestinationData;           // holder for destination data
  register WORD wAlphaData;                 // holder source and data blended
  register BYTE cData;                      // 8 bit source index
  register BYTE cAlpha;                     // 8 bit alpha channel

  // calculate 16:16 fractional ratio between destination and source scanlines
  nFractionalLines = (lprSourceBltRect->bottom - lprSourceBltRect->top) - 1;
  nFractionalLines <<= 16;
  nFractionalLines /= (lprDestinationBltRect->bottom - lprDestinationBltRect->top) - 1;

  // calculate 16:16 fractional ratio between destination and source scan widths
  nFractionalPixels = (lprSourceBltRect->right - lprSourceBltRect->left) - 1;
  nFractionalPixels <<= 16;
  nFractionalPixels /= (lprDestinationBltRect->right - lprDestinationBltRect->left) - 1;

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap using clip RECT
  lpDestinationBeginBlt = lpDestinationBits +
                          (lprDestinationClipRect->top * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += lprDestinationClipRect->left * 2;

  // calculate start position of blt in source bitmap WITHOUT clipping
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left;

  // calculate dimensions of clipped blt
  nLinesToBlt = lprDestinationClipRect->bottom - lprDestinationClipRect->top;
  nPixelsAcrossToBlt = lprDestinationClipRect->right - lprDestinationClipRect->left;

  // calculate total fractional lines including clipping
  nTotalFractionalLines = lprDestinationClipRect->top - lprDestinationBltRect->top;
  nTotalFractionalLines *= nFractionalLines;

  // calculate start of source line, with Y clipping
  lpSourceBeginLineBlt = lpSourceBeginBlt;
  lpSourceBeginLineBlt += HIWORD(nTotalFractionalLines + 0x8000) * nXSourceWidthInBytes;
  for (nLinesCounter = 0; nLinesCounter < nLinesToBlt; nLinesCounter ++)
  {
    // initialize destination blt pointer
    lpDestinationBltPointer = lpDestinationBeginBlt;

    // calculate total fractional pixels including clipping
    nTotalFractionalPixels = lprDestinationClipRect->left - lprDestinationBltRect->left;
    nTotalFractionalPixels *= nFractionalPixels;

    // initialize source blt pointer including clipping
    lpSourceBltPointer = lpSourceBeginLineBlt;
    lpSourceBltPointer += HIWORD(nTotalFractionalPixels + 0x8000);
    for (nPixelCounter = 0; nPixelCounter < nPixelsAcrossToBlt; nPixelCounter ++)
    {
      // get one pixel of source data
      cData = *((LPBYTE)lpSourceBltPointer);

      // convert 8 bit index to 32 bit color value
      // and store converted data at destination if not zero
      if (cData != 0)
      {
        if (cData >= nAlphaInColorTable)
        {
          *((LPWORD)lpDestinationBltPointer) = (WORD)lpColorTable[cData * 2];
        }
        else
        {
          cAlpha = (BYTE)lpColorTable[(cData * 2) + 1];
          wDestinationData = (WORD)lpColorTable[cData * 2];
          wSourceData = *((LPWORD)lpDestinationBltPointer);
          if (cAlpha == ALPHA_OPAQUE06_25)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + (wSourceData & LE_BLT_dwRedMask) - ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + (wSourceData & LE_BLT_dwGreenMask) - ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + (wSourceData & LE_BLT_dwBlueMask) - ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE12_50)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + (wSourceData & LE_BLT_dwRedMask) - ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + (wSourceData & LE_BLT_dwGreenMask) - ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + (wSourceData & LE_BLT_dwBlueMask) - ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE18_75)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE25_00)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE31_25)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 3) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 3) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 3) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE37_50)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE43_75)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE50_00)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE56_25)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 1) - ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 1) - ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 1) - ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE62_50)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE68_75)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 2) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 2) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 2) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE75_00)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE81_25)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 3) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 3) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 3) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE87_50)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask);
          }
          else if (cAlpha == ALPHA_OPAQUE93_75)
          {
            wAlphaData = (WORD)(((wDestinationData & LE_BLT_dwRedMask) + ((wSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwGreenMask) + ((wSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask);
            wAlphaData |= (WORD)(((wDestinationData & LE_BLT_dwBlueMask) + ((wSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask);
          }
          *((LPWORD)lpDestinationBltPointer) = wAlphaData;
        }
      }
      // update destination pointer to next destination pixel
      lpDestinationBltPointer += 2;

      // update source pointer using fractional pixels value
      nTotalFractionalPixels += nFractionalPixels;
      lpSourceBltPointer = lpSourceBeginLineBlt;
      lpSourceBltPointer += HIWORD(nTotalFractionalPixels + 0x8000);
    }
    // update destination pointer to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;

    // update source pointer using fractional scanlines value
    nTotalFractionalLines += nFractionalLines;
    lpSourceBeginLineBlt = lpSourceBeginBlt;
    lpSourceBeginLineBlt += HIWORD(nTotalFractionalLines + 0x8000) * nXSourceWidthInBytes;
  }
}


/*******************************************************************************
*
*   Name:         LI_BLT16ConvertColorRefTo16BitColor
*
*   Description:  converts a COLORREF to a 16 bit color
*
*   Arguments:    crColorToFillWith - COLORREF to convert to 16 bit color
*
*   Globals:      LE_BLT_nRedShift - number of first bit set in LE_BLT_dwRedMask (1 to 16)
*                 LE_BLT_nGreenShift -  number of first bit set in LE_BLT_dwGreenMask (1 - 16)
*                 LE_BLT_nBlueShift - number of first bit set in LE_BLT_dwBlueMask (1 - 16)
*                 LE_BLT_dwRedMask - mask of red bits for 16 bit DIBs
*                 LE_BLT_dwGreenMask - mask of green bits for 16 bit DIBs
*                 LE_BLT_dwBlueMask - mask of blue bits for 16 bit DIBs
*
*   Returns:      16 bit color
*
*******************************************************************************/
WORD LI_BLT16ConvertColorRefTo16BitColor(
  COLORREF crColorToFillWith)
{
  WORD w16BitColor;   // 16 bit color (RETURNED)
  short nShift16;     // how many bits to shift values from COLORREF

  // shift and mask red bit values from COLORREF
  nShift16 = LE_BLT_nRedShift - 8;
  if (nShift16 >= 0)
    w16BitColor = (WORD)(((crColorToFillWith & 0xff) << nShift16) & LE_BLT_dwRedMask);
  else
    w16BitColor = (WORD)(((crColorToFillWith & 0xff) >> -nShift16) & LE_BLT_dwRedMask);

  // shift and mask green bit values from COLORREF
  nShift16 = LE_BLT_nGreenShift - 16;
  if (nShift16 >= 0)
    w16BitColor |= (WORD)(((crColorToFillWith & 0xff00) << nShift16) & LE_BLT_dwGreenMask);
  else
    w16BitColor |= (WORD)(((crColorToFillWith & 0xff00) >> -nShift16) & LE_BLT_dwGreenMask);

  // shift and mask blue bit values from COLORREF
  nShift16 = LE_BLT_nBlueShift - 24;
  if (nShift16 >= 0)
    w16BitColor |= (WORD)(((crColorToFillWith & 0xff0000) << nShift16) & LE_BLT_dwBlueMask);
  else
    w16BitColor |= (WORD)(((crColorToFillWith & 0xff0000) >> -nShift16) & LE_BLT_dwBlueMask);

  // return converted 16 bit color
  return(w16BitColor);
}


/*******************************************************************************
*
*   Name:         LI_BLT16ConvertRGBColorTableTo16BitColor
*
*   Description:  converts an RGBQUAD color table to 16 bit colors
*
*   Arguments:    lpRGBColorTable - pointer to RGBQUAD color table
*                 nColorTableEntries - number of entries in color table
*
*   Globals:      LE_BLT_nRedShift - number of first bit set in LE_BLT_dwRedMask (1 to 16)
*                 LE_BLT_nGreenShift -  number of first bit set in LE_BLT_dwGreenMask (1 - 16)
*                 LE_BLT_nBlueShift - number of first bit set in LE_BLT_dwBlueMask (1 - 16)
*                 LE_BLT_dwRedMask - mask of red bits for 16 bit DIBs
*                 LE_BLT_dwGreenMask - mask of green bits for 16 bit DIBs
*                 LE_BLT_dwBlueMask - mask of blue bits for 16 bit DIBs
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT16ConvertRGBColorTableTo16BitColor(
  LPLONG lpRGBColorTable,
  int nColorTableEntries)
{
  int nColorTableIndex;       // color table index
  DWORD dw16BitColor;         // 16 bit color value
  DWORD dwRGBValue;           // RGBQUAD value
  short nShift16;             // how many bits to shift values from RGBQUAD

  // store 16 bit table in same location as RGBQUAD table
  for (nColorTableIndex = 0; nColorTableIndex < nColorTableEntries; nColorTableIndex ++)
  {
    // get RGBQUAD value as a DWORD
    dwRGBValue = *lpRGBColorTable;

    // shift and mask blue bit values from RGBQUAD
    nShift16 = LE_BLT_nBlueShift - 8;
    if (nShift16 >= 0)
      dw16BitColor = ((dwRGBValue & 0xff) << nShift16) & LE_BLT_dwBlueMask;
    else
      dw16BitColor = ((dwRGBValue & 0xff) >> -nShift16) & LE_BLT_dwBlueMask;

    // shift and mask green bit values from RGBQUAD
    nShift16 = LE_BLT_nGreenShift - 16;
    if (nShift16 >= 0)
      dw16BitColor |= ((dwRGBValue & 0xff00) << nShift16) & LE_BLT_dwGreenMask;
    else
      dw16BitColor |= ((dwRGBValue & 0xff00) >> -nShift16) & LE_BLT_dwGreenMask;

    // shift and mask red bit values from RGBQUAD
    nShift16 = LE_BLT_nRedShift - 24;
    if (nShift16 >= 0)
      dw16BitColor |= ((dwRGBValue & 0xff0000) << nShift16) & LE_BLT_dwRedMask;
    else
      dw16BitColor |= ((dwRGBValue & 0xff0000) >> -nShift16) & LE_BLT_dwRedMask;

    // store 16 bit color value
    *lpRGBColorTable = dw16BitColor;

    // increment color table pointer
    lpRGBColorTable ++;
    lpRGBColorTable ++;
  }
}


/*******************************************************************************
*
*   Name:                   LI_BLTColorKeyPureGreen16BitBlt16to16
*
*   Description:  blts from a 16 bit bitmap to a 16 bit bitmap
*                 A color of pure green is considered to be transparent.
*                 Well, now things close to green or blue are transparent.
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
void LI_BLT16ColorKeyPureGreenBitBlt16to16(
  LPBYTE  lpDestinationBits,
  int     nXDestinationWidthInPixels,
  int     nXBltStartCoordinate,
  int     nYBltStartCoordinate,
  LPBYTE  lpSourceBits,
  int     nXSourceWidthInPixels,
  LPRECT  lprSourceBltRect)
{
  LPBYTE  lpDestinationBeginBlt;          // pointer to destination scanline position to begin blt
  int     nXDestinationWidthInBytes;      // destination bitmap width in bytes
  LPBYTE  lpSourceBeginBlt;               // pointer to source scanline position to begin blt
  int     nXSourceWidthInBytes;           // source bitmap width in bytes
  int     nBytesAcrossToBlt;              // blt width in bytes
  int     nLinesToBlt;                    // blt height in pixels
  int     nPixelsAcrossToBlt;             // blt width in pixels  // mmh
  int     pixelcount = 0;                 // mmh
  LPWORD  curSourceBlt;                   // mmh
  LPWORD  curDestinationBlt;              // mmh
  WORD    redLimit;
  WORD    greenLimit;
  WORD    blueLimit;
  WORD    redMask = (WORD) LE_BLT_dwRedMask;
  WORD    greenMask = (WORD) LE_BLT_dwGreenMask;
  WORD    blueMask = (WORD) LE_BLT_dwBlueMask;
  WORD    rawPixel;
  
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_BlitBoundsCheck
  // Check to see if blt will trample anything
  LI_BLT16BreaksBounds ( lpDestinationBits, nXDestinationWidthInPixels,
    nXBltStartCoordinate, nYBltStartCoordinate, lprSourceBltRect );
#endif
  
  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((nXDestinationWidthInPixels * 2) + 3) & 0xfffffffc;
  nXSourceWidthInBytes = ((nXSourceWidthInPixels * 2) + 3) & 0xfffffffc;
  
  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits + (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 2;
  
  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left * 2;
  
  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nBytesAcrossToBlt = (lprSourceBltRect->right - lprSourceBltRect->left) * 2;
  nPixelsAcrossToBlt = (lprSourceBltRect->right - lprSourceBltRect->left);
  
  // Find the colour limits as values corresponding to the current pixel depth.
  redLimit = LI_BLT16ConvertColorRefTo16BitColor (
    LE_GRAFIX_MakeColorRef (LE_BLT_KeyRedLimit, 0, 0));
  greenLimit = LI_BLT16ConvertColorRefTo16BitColor (
    LE_GRAFIX_MakeColorRef (0, LE_BLT_KeyGreenLimit, 0));
  blueLimit = LI_BLT16ConvertColorRefTo16BitColor (
    LE_GRAFIX_MakeColorRef (0, 0, LE_BLT_KeyBlueLimit));
  while (nLinesToBlt > 0)
  {
    curDestinationBlt = (WORD*)lpDestinationBeginBlt;
    curSourceBlt      = (WORD*)lpSourceBeginBlt;
    pixelcount = 0;
    while (pixelcount < nPixelsAcrossToBlt)
    {
#if CE_ARTLIB_BlitUseGreenColourKey
      if (((rawPixel = *curSourceBlt) & redMask) >= redLimit ||
      (rawPixel & greenMask) < greenLimit ||
      (rawPixel & blueMask) >= blueLimit)
#elif LE_CMAIN_BlitUseBlueColourKey
      if (((rawPixel = *curSourceBlt) & redMask) >= redLimit ||
      (rawPixel & greenMask) >= greenLimit ||
      (rawPixel & blueMask) < blueLimit)
#else // Just use absolute black as transparent.
      if (((rawPixel = *curSourceBlt) & 0x00FFFFFF) != 0)
#endif
      {
        *curDestinationBlt = rawPixel;
      }
      curDestinationBlt ++;
      curSourceBlt ++;
      pixelcount ++;
    }
    
    // update scanline position to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt += nXSourceWidthInBytes;
    nLinesToBlt --;
  }
}



/*******************************************************************************
*
*   Name:           LI_BLT16ConvertRGBValuesTo16BitColor
*
*   Description:    converts a 24 bit DIB to 16 bit DIB, source and dest can be of different sizes
*
*   Arguments:      lp16BitData     - pointer to 16 bit DIB bits
*                   lp24BitData     - pointer to 24 bit DIB bits
*                   src_w_pixels    - width of source in pixels
*                   src_h_pixels    - height of source in pixels
*                   dest_w_pixels   - width of source in pixels
*                   dest_h_pixels   - height of source in pixels
*
*   Globals:      LE_BLT_nRedShift - number of first bit set in LE_BLT_dwRedMask (1 to 16)
*                 LE_BLT_nGreenShift -  number of first bit set in LE_BLT_dwGreenMask (1 - 16)
*                 LE_BLT_nBlueShift - number of first bit set in LE_BLT_dwBlueMask (1 - 16)
*                 wRedMask - mask of red bits for 16 bit DIBs
*                 wGreenMask - mask of green bits for 16 bit DIBs
*                 wBlueMask - mask of blue bits for 16 bit DIBs
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT16ConvertRGBValuesTo16BitColor(
  LPBYTE lp16BitData,
  LPBYTE lp24BitData,
    int src_w_pixels,
    int src_h_pixels,
    int dest_w_pixels,
    int dest_h_pixels)
{
    LPBYTE lpDestinationBeginBlt;               // pointer to destination scanline for conversion
    int nXDestinationWidthInBytes;              // destination bitmap width in bytes
    LPBYTE lpSourceBeginBlt;                    // pointer to source scanline for conversion
    int nXSourceWidthInBytes;                   // source bitmap width in bytes

  register LPBYTE lpDestinationBltPointer;  // pointer to current conversion destination
    register LPBYTE lpSourceBltPointer;         // pointer to current conversion source
    register int nPixelCounter;                 // pixel counter
    register int nLinesCounter;                 // line counter
    register WORD w16BitColor;                  // converted 16 bit pixel
    register DWORD dw24BitData1;                // source data storage
  register DWORD dw24BitData2;
  register DWORD dw24BitData3;
    register short nShift16;                    // how many bits to shift values from 24 bit DIB

  // calculate bitmap widths in bytes
    nXDestinationWidthInBytes = ((dest_w_pixels * 2) + 3) & 0xfffffffc;
    nXSourceWidthInBytes      = ((src_w_pixels * 3) + 3) & 0xfffffffc;

  // initialize start scanline pointers
  lpDestinationBeginBlt = lp16BitData;
    lpSourceBeginBlt = lp24BitData + (nXSourceWidthInBytes * (src_h_pixels - 1));

    for (nLinesCounter = 0; nLinesCounter < src_h_pixels; nLinesCounter ++)
  {
    // initialize current conversion pointers
    lpDestinationBltPointer = lpDestinationBeginBlt;
    lpSourceBltPointer = lpSourceBeginBlt;

    // initialize pixel counter
        nPixelCounter = src_w_pixels;
    while (nPixelCounter > 0)
    {
      // process four pixels at a time if possible
      if (nPixelCounter >= 4)
      {
        // get four 24 bit pixels of data
        dw24BitData1 = *((LPLONG)lpSourceBltPointer);
        lpSourceBltPointer += 4;
        dw24BitData2 = *((LPLONG)lpSourceBltPointer);
        lpSourceBltPointer += 4;
        dw24BitData3 = *((LPLONG)lpSourceBltPointer);
        lpSourceBltPointer += 4;

        // convert 24 bits of data to 16 bits
        nShift16 = LE_BLT_nBlueShift - 8;
        if (nShift16 >= 0)
          w16BitColor = (WORD)(((dw24BitData1 & 0xff) << nShift16) & LE_BLT_dwBlueMask);
        else
          w16BitColor = (WORD)(((dw24BitData1 & 0xff) >> -nShift16) & LE_BLT_dwBlueMask);
        nShift16 = LE_BLT_nGreenShift - 16;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData1 & 0xff00) << nShift16) & LE_BLT_dwGreenMask);
        else
          w16BitColor |= (WORD)(((dw24BitData1 & 0xff00) >> -nShift16) & LE_BLT_dwGreenMask);
        nShift16 = LE_BLT_nRedShift - 24;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData1 & 0xff0000) << nShift16) & LE_BLT_dwRedMask);
        else
          w16BitColor |= (WORD)(((dw24BitData1 & 0xff0000) >> -nShift16) & LE_BLT_dwRedMask);

        // store 16 bit color
        *((LPWORD)lpDestinationBltPointer) = w16BitColor;
        lpDestinationBltPointer += 2;

        // convert 24 bits of data to 16 bits
        nShift16 = LE_BLT_nBlueShift - 32;
        if (nShift16 >= 0)
          w16BitColor = (WORD)(((dw24BitData1 & 0xff000000) << nShift16) & LE_BLT_dwBlueMask);
        else
          w16BitColor = (WORD)(((dw24BitData1 & 0xff000000) >> -nShift16) & LE_BLT_dwBlueMask);
        nShift16 = LE_BLT_nGreenShift - 8;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff) << nShift16) & LE_BLT_dwGreenMask);
        else
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff) >> -nShift16) & LE_BLT_dwGreenMask);
        nShift16 = LE_BLT_nRedShift - 16;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff00) << nShift16) & LE_BLT_dwRedMask);
        else
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff00) >> -nShift16) & LE_BLT_dwRedMask);

        // store 16 bit color
        *((LPWORD)lpDestinationBltPointer) = w16BitColor;
        lpDestinationBltPointer += 2;

        // convert 24 bits of data to 16 bits
        nShift16 = LE_BLT_nBlueShift - 24;
        if (nShift16 >= 0)
          w16BitColor = (WORD)(((dw24BitData2 & 0xff0000) << nShift16) & LE_BLT_dwBlueMask);
        else
          w16BitColor = (WORD)(((dw24BitData2 & 0xff0000) >> -nShift16) & LE_BLT_dwBlueMask);
        nShift16 = LE_BLT_nGreenShift - 32;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff000000) << nShift16) & LE_BLT_dwGreenMask);
        else
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff000000) >> -nShift16) & LE_BLT_dwGreenMask);
        nShift16 = LE_BLT_nRedShift - 8;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData3 & 0xff) << nShift16) & LE_BLT_dwRedMask);
        else
          w16BitColor |= (WORD)(((dw24BitData3 & 0xff) >> -nShift16) & LE_BLT_dwRedMask);

        // store 16 bit color
        *((LPWORD)lpDestinationBltPointer) = w16BitColor;
        lpDestinationBltPointer += 2;

        // convert 24 bits of data to 16 bits
        nShift16 = LE_BLT_nBlueShift - 16;
        if (nShift16 >= 0)
          w16BitColor = (WORD)(((dw24BitData3 & 0xff00) << nShift16) & LE_BLT_dwBlueMask);
        else
          w16BitColor = (WORD)(((dw24BitData3 & 0xff00) >> -nShift16) & LE_BLT_dwBlueMask);
        nShift16 = LE_BLT_nGreenShift - 24;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData3 & 0xff0000) << nShift16) & LE_BLT_dwGreenMask);
        else
          w16BitColor |= (WORD)(((dw24BitData3 & 0xff0000) >> -nShift16) & LE_BLT_dwGreenMask);
        nShift16 = LE_BLT_nRedShift - 32;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData3 & 0xff000000) << nShift16) & LE_BLT_dwRedMask);
        else
          w16BitColor |= (WORD)(((dw24BitData3 & 0xff000000) >> -nShift16) & LE_BLT_dwRedMask);
        *((LPWORD)lpDestinationBltPointer) = w16BitColor;

        // store 16 bit color
        lpDestinationBltPointer += 2;
        nPixelCounter -= 4;
      }

      // process two pixels at a time if possible
      else if (nPixelCounter >= 2)
      {
        // get two 24 bit pixels of data
        dw24BitData1 = *((LPLONG)lpSourceBltPointer);
        lpSourceBltPointer += 4;
        dw24BitData2 = *((LPWORD)lpSourceBltPointer);
        lpSourceBltPointer += 2;

        // convert 24 bits of data to 16 bits
        nShift16 = LE_BLT_nBlueShift - 8;
        if (nShift16 >= 0)
          w16BitColor = (WORD)(((dw24BitData1 & 0xff) << nShift16) & LE_BLT_dwBlueMask);
        else
          w16BitColor = (WORD)(((dw24BitData1 & 0xff) >> -nShift16) & LE_BLT_dwBlueMask);
        nShift16 = LE_BLT_nGreenShift - 16;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData1 & 0xff00) << nShift16) & LE_BLT_dwGreenMask);
        else
          w16BitColor |= (WORD)(((dw24BitData1 & 0xff00) >> -nShift16) & LE_BLT_dwGreenMask);
        nShift16 = LE_BLT_nRedShift - 24;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData1 & 0xff0000) << nShift16) & LE_BLT_dwRedMask);
        else
          w16BitColor |= (WORD)(((dw24BitData1 & 0xff0000) >> -nShift16) & LE_BLT_dwRedMask);

        // store 16 bit color
        *((LPWORD)lpDestinationBltPointer) = w16BitColor;
        lpDestinationBltPointer += 2;

        // convert 24 bits of data to 16 bits
        nShift16 = LE_BLT_nBlueShift - 32;
        if (nShift16 >= 0)
          w16BitColor = (WORD)(((dw24BitData1 & 0xff000000) << nShift16) & LE_BLT_dwBlueMask);
        else
          w16BitColor = (WORD)(((dw24BitData1 & 0xff000000) >> -nShift16) & LE_BLT_dwBlueMask);
        nShift16 = LE_BLT_nGreenShift - 8;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff) << nShift16) & LE_BLT_dwGreenMask);
        else
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff) >> -nShift16) & LE_BLT_dwGreenMask);
        nShift16 = LE_BLT_nRedShift - 16;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff00) << nShift16) & LE_BLT_dwRedMask);
        else
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff00) >> -nShift16) & LE_BLT_dwRedMask);

        // store 16 bit color
        *((LPWORD)lpDestinationBltPointer) = w16BitColor;
        lpDestinationBltPointer += 2;
        nPixelCounter -= 2;
      }

      // process one pixel
      else
      {
        // get one 24 bit pixel of data
        dw24BitData1 = *((LPWORD)lpSourceBltPointer);
        lpSourceBltPointer += 2;
        dw24BitData2 = *((LPBYTE)lpSourceBltPointer);
        lpSourceBltPointer += 1;

        // convert 24 bits of data to 16 bits
        nShift16 = LE_BLT_nBlueShift - 8;
        if (nShift16 >= 0)
          w16BitColor = (WORD)(((dw24BitData1 & 0xff) << nShift16) & LE_BLT_dwBlueMask);
        else
          w16BitColor = (WORD)(((dw24BitData1 & 0xff) >> -nShift16) & LE_BLT_dwBlueMask);
        nShift16 = LE_BLT_nGreenShift - 16;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData1 & 0xff00) << nShift16) & LE_BLT_dwGreenMask);
        else
          w16BitColor |= (WORD)(((dw24BitData1 & 0xff00) >> -nShift16) & LE_BLT_dwGreenMask);
        nShift16 = LE_BLT_nRedShift - 8;
        if (nShift16 >= 0)
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff) << nShift16) & LE_BLT_dwRedMask);
        else
          w16BitColor |= (WORD)(((dw24BitData2 & 0xff) >> -nShift16) & LE_BLT_dwRedMask);

        // store 16 bit color
        *((LPWORD)lpDestinationBltPointer) = w16BitColor;
        lpDestinationBltPointer += 2;
        nPixelCounter -= 1;
      }
    }
    // update pointers to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt -= nXSourceWidthInBytes;
  }
}


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
*   Returns:    TRUE if the blt will blow the bounds of the destination.
*         FALSE if the blt will fit inside the bounds of the destination.
*
*******************************************************************************/
BOOL LI_BLT16BreaksBounds
(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPRECT lprSourceBltRect
)
{
  LPBYTE endOfARTECHMemoryBlock;
  LPBYTE endOfBltBlock;
  int nLinesToBlt;
  int nXPixelsAcrossToBlt;
  int nXBytesAcrossToBlt;
  int nXDestinationWidthInBytes;

  //If we're starting the blt higher up than or to the left of the upper
  //left hand corner (0, 0). We will be trampling memory before the block.
  if( nXBltStartCoordinate < 0 )
  {
    LI_MEMORY_BoundFailure( lpDestinationBits );
    wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf(LE_ERROR_DebugMessageBuffer,"LI_BLT16BreaksBounds: Blt will overrun the left edge of the destination\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    return( TRUE );
  }

  if( nYBltStartCoordinate < 0 )
  {
    LI_MEMORY_BoundFailure( lpDestinationBits );
    wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf(LE_ERROR_DebugMessageBuffer,"LI_BLT16BreaksBounds: Blt will overrun the top edge of the destination\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    return( TRUE );
  }

  //Get the number of pixels, then bits across that will be blitted
  nXPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;
  nXBytesAcrossToBlt = 2 * nXPixelsAcrossToBlt;

  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;

  //If the right hand edge of the BLT will overrun the right hand edge, error..
  if( nXBltStartCoordinate + nXPixelsAcrossToBlt > nXDestinationWidthInPixels )
  {
    LI_MEMORY_BoundFailure( lpDestinationBits );
    wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf(LE_ERROR_DebugMessageBuffer,"LI_BLT16BreaksBounds: Blt will overrun the right edge of the destination\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    return( TRUE );
  }

  //Get the destination width in bytes, time to do the bottom test.
  nXDestinationWidthInBytes = (( nXDestinationWidthInPixels * 2 ) + 3 ) & 0xFFFFFFFC;

  endOfBltBlock = lpDestinationBits + ( nYBltStartCoordinate * nXDestinationWidthInBytes );
  endOfBltBlock += (nLinesToBlt - 1) * nXDestinationWidthInBytes;
  endOfBltBlock += 2 * nXBltStartCoordinate;
  endOfBltBlock += nXBytesAcrossToBlt;

  // Quick check to see if the destination is the screen, which isn't
  // part of the Artech memory system.

  if (lpDestinationBits >= LI_BLT_BoundCheckScreenStart &&
  lpDestinationBits < LI_BLT_BoundCheckScreenPastEnd)
    endOfARTECHMemoryBlock = LI_BLT_BoundCheckScreenPastEnd;
  else
  {
    endOfARTECHMemoryBlock = LI_MEMORY_TestBlockAndReturnBottomPtr( lpDestinationBits );
    endOfARTECHMemoryBlock++; // So it points just past the end of the block.
  }

  if( endOfARTECHMemoryBlock == NULL )
  {
    LI_MEMORY_BoundFailure( lpDestinationBits );
    wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf(LE_ERROR_DebugMessageBuffer,"LI_BLT16BreaksBounds: Attempting to Blt to unallocated memory\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    return( TRUE );
  }

  //This is a bit nebulous, as the destination could be really wide and not too
  //high, the block could be overruning the bottom of the bitmap, but still fit
  //within the physical space of the bitmap itself. But should be good enough
  //given no one is purposly trying to break something.
  if( endOfBltBlock > (LPBYTE)endOfARTECHMemoryBlock )
  {
    LI_MEMORY_BoundFailure( lpDestinationBits );
    wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf(LE_ERROR_DebugMessageBuffer,"LI_BLT16BreaksBounds: Blt will overrun the bottom of the destination\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    return( TRUE );
  }

  //If we're here, the BLT will fit inside the memory chunk.
  return( FALSE );
}
#endif // if CE_ARTLIB_EnableDebugMode



#endif // CE_ARTLIB_EnableSystemGrafix2D && CE_ARTLIB_BlitUseOldRoutines
