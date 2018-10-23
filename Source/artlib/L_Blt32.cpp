/*****************************************************************************
 * FILE:        L_Blt32.cpp
 *
 * DESCRIPTION: Obsolete bitmap copying routines for 32 bit mode.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Blt32.cpp 1     1/16/99 4:47p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Blt32.cpp $
 * 
 * 1     1/16/99 4:47p Agmsmith
 * Put back old blitter routines so that they can optinally be used to
 * avoid / demonstrate bugs in the new ones.
 ****************************************************************************/


#include "l_inc.h"

#if CE_ARTLIB_EnableSystemGrafix2D && CE_ARTLIB_BlitUseOldRoutines


void LI_BLT32BitBlt32to32(LPBYTE,int,int,int,LPBYTE,int,LPRECT);
void LI_BLT32BitBlt8to32(LPBYTE,int,int,int,LPBYTE,int,LPRECT,register LPLONG);
void LI_BLT32ColorKeyBitBlt8to32(LPBYTE,int,int,int,LPBYTE,int,LPRECT,register LPLONG);
void LI_BLT32AlphaBitBlt8to32(LPBYTE,int,int,int,LPBYTE,int,LPRECT,register LPLONG,int);
void LI_BLT32AlphaBitBlt32to32(LPBYTE,int,int,int,LPBYTE,int,LPRECT,int);
void LI_BLT32ColorBlt(LPBYTE,int,LPRECT,DWORD,BOOL);
void LI_BLT32StretchBlt8to32(LPBYTE,int,LPRECT,LPRECT,LPBYTE,int,LPRECT,register LPLONG);
void LI_BLT32ColorKeyStretchBlt8to32(LPBYTE,int,LPRECT,LPRECT,LPBYTE,int,LPRECT,register LPLONG);
void LI_BLT32AlphaStretchBlt8to32(LPBYTE,int,LPRECT,LPRECT,LPBYTE,int,LPRECT,register LPLONG,int);
void LI_BLT32ColorKeyPureGreenBitBlt32to32(LPBYTE,int,int,int,LPBYTE,int,LPRECT);

DWORD LI_BLT32ConvertColorRefTo32BitColor(COLORREF);
void LI_BLT32ConvertRGBColorTableTo32BitColor(LPLONG,int);
void LI_BLT32ConvertRGBValuesTo32BitColor(LPBYTE,LPBYTE,int,int,int,int);


/*******************************************************************************
*
*   Name:         LI_BLT32BitBlt32to32
*
*   Description:  blts from a 32 bit bitmap to a 32 bit bitmap
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
void LI_BLT32BitBlt32to32(
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

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = nXSourceWidthInPixels * 4;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 4;

  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left * 4;

  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nBytesAcrossToBlt = (lprSourceBltRect->right - lprSourceBltRect->left) * 4;

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
*   Name:         LI_BLT32BitBlt8to32
*
*   Description:  blts from an 8 bit paletted bitmap to a 32 bit native
*                 bitmap
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 nXBltStartCoordinate - X coord, ul corner of blt RECT
*                 nYBltStartCoordinate - Y coord, ul corner of blt RECT
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - source blt RECT
*                 lpColorTable - pointer to 8 to 32 bit color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT32BitBlt8to32(
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

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 4;

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

        // convert 8 bit indices to 32 bit color values
        // and move data to destination bitmap
        *((LPLONG)lpDestinationBltPointer) = lpColorTable[(dwSourceData & 0xff) * 2];
        lpDestinationBltPointer += 4;
        dwSourceData >>= 7;
        *((LPLONG)lpDestinationBltPointer) = lpColorTable[dwSourceData & 0x1fe];
        lpDestinationBltPointer += 4;
        dwSourceData >>= 8;
        *((LPLONG)lpDestinationBltPointer) = lpColorTable[dwSourceData & 0x1fe];
        lpDestinationBltPointer += 4;
        dwSourceData >>= 8;
        *((LPLONG)lpDestinationBltPointer) = lpColorTable[dwSourceData & 0x1fe];
        lpDestinationBltPointer += 4;
        nPixelCounter -= 4;
      }

      // process two pixels at a time if possible
      else if (nPixelCounter >= 2)
      {
        // get two pixels of source data
        dwSourceData = *((LPWORD)lpSourceBltPointer);
        lpSourceBltPointer += 2;

        // convert 8 bit indices to 32 bit color values
        // and move data to destination bitmap
        *((LPLONG)lpDestinationBltPointer) = lpColorTable[(dwSourceData & 0xff) * 2];
        lpDestinationBltPointer += 4;
        dwSourceData >>= 7;
        *((LPLONG)lpDestinationBltPointer) = lpColorTable[dwSourceData & 0x1fe];
        lpDestinationBltPointer += 4;
        nPixelCounter -= 2;
      }

      // process one pixel
      else
      {
        // get one pixel of source data
        dwSourceData = *((LPBYTE)lpSourceBltPointer);
        lpSourceBltPointer += 1;

        // convert 8 bit index to 32 bit color value
        // and move data to destination bitmap
        *((LPLONG)lpDestinationBltPointer) = lpColorTable[(dwSourceData & 0xff) * 2];
        lpDestinationBltPointer += 4;
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
*   Name:                   LI_BLT32ColorKeyBitBlt8to32
*
*   Description:  blts from an 8 bit paletted bitmap to a 32 bit native
*                 bitmap, treating the color index 0 as transparent
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 nXBltStartCoordinate - X coord, ul corner of blt RECT
*                 nYBltStartCoordinate - Y coord, ul corner of blt RECT
*                 lpSourceBits - pointer to source bitmap bits
*                 nXSourceWidthInPixels - source bitmap width
*                 lprSourceBltRect - source blt RECT
*                 lpColorTable - pointer to 8 to 32 bit color table
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT32ColorKeyBitBlt8to32(
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

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 4;

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

        // convert 8 bit index to 32 bit color value
        // and store converted data at destination if not zero
        if ((dwSourceData & 0xff) != 0)
          *((LPLONG)lpDestinationBltPointer) = lpColorTable[(dwSourceData & 0xff) * 2];
        // otherwise slip over destination pixel
        lpDestinationBltPointer += 4;
        dwSourceData >>= 7;

        // convert 8 bit index to 32 bit color value
        // and store converted data at destination if not zero
        if ((dwSourceData & 0x1fe) != 0)
          *((LPLONG)lpDestinationBltPointer) = lpColorTable[dwSourceData & 0x1fe];
        // otherwise slip over destination pixel
        lpDestinationBltPointer += 4;
        dwSourceData >>= 8;

        // convert 8 bit index to 32 bit color value
        // and store converted data at destination if not zero
        if ((dwSourceData & 0x1fe) != 0)
          *((LPLONG)lpDestinationBltPointer) = lpColorTable[dwSourceData & 0x1fe];
        // otherwise slip over destination pixel
        lpDestinationBltPointer += 4;
        dwSourceData >>= 8;

        // convert 8 bit index to 32 bit color value
        // and store converted data at destination if not zero
        if ((dwSourceData & 0x1fe) != 0)
          *((LPLONG)lpDestinationBltPointer) = lpColorTable[dwSourceData & 0x1fe];
        // otherwise slip over destination pixel
        lpDestinationBltPointer += 4;
        nPixelCounter -= 4;
      }

      // process two pixels at a time if possible
      else if (nPixelCounter >= 2)
      {
        // get two pixels of source data
        dwSourceData = *((LPWORD)lpSourceBltPointer);
        lpSourceBltPointer += 2;

        // convert 8 bit index to 32 bit color value
        // and store converted data at destination if not zero
        if ((dwSourceData & 0xff) != 0)
          *((LPLONG)lpDestinationBltPointer) = lpColorTable[(dwSourceData & 0xff) * 2];
        // otherwise slip over destination pixel
        lpDestinationBltPointer += 4;
        dwSourceData >>= 7;

        // convert 8 bit index to 32 bit color value
        // and store converted data at destination if not zero
        if ((dwSourceData & 0x1fe) != 0)
          *((LPLONG)lpDestinationBltPointer) = lpColorTable[dwSourceData & 0x1fe];
        // otherwise slip over destination pixel
        lpDestinationBltPointer += 4;
        nPixelCounter -= 2;
      }

      // process one pixel
      else
      {
        // get one pixel of source data
        dwSourceData = *((LPBYTE)lpSourceBltPointer);
        lpSourceBltPointer += 1;

        // convert 8 bit index to 32 bit color value
        // and store converted data at destination if not zero
        if ((dwSourceData & 0xff) != 0)
          *((LPLONG)lpDestinationBltPointer) = lpColorTable[(dwSourceData & 0xff) * 2];
        // otherwise slip over destination pixel
        lpDestinationBltPointer += 4;
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
*   Globals:      LE_BLT_dwRedMask - mask of red bits for 32 bit DIBs
*                 LE_BLT_dwGreenMask - mask of green bits for 32 bit DIBs
*                 LE_BLT_dwBlueMask - mask of blue bits for 32 bit DIBs
*
*   Returns:      none
*
*******************************************************************************/

/*  Here's the C version. */
/*
void LI_BLT32AlphaBitBlt8to32(
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
  register DWORD dwSourceData;              // holder for source data
  register DWORD dwDestinationData;         // holder for destination data
  register DWORD dwAlphaData;               // holder source and data blended
  register BYTE cData;                      // 8 bit source index
  register BYTE cAlpha;                     // 8 bit alpha channel

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 4;

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
      cData = *lpSourceBltPointer;
      lpSourceBltPointer ++;

      // convert 8 bit index to 24 bit color value
      // and store converted data at destination if not zero
      if (cData != 0)
      {
        if (cData >= nAlphaInColorTable)
        {
          *((LPLONG)lpDestinationBltPointer) = lpColorTable[cData * 2];
        }
        else
        {
          cAlpha = (BYTE)lpColorTable[(cData * 2) + 1];
          dwDestinationData = lpColorTable[cData * 2];
          dwSourceData = *((LPLONG)lpDestinationBltPointer);
          if (cAlpha == ALPHA_OPAQUE06_25)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + (dwSourceData & LE_BLT_dwRedMask) - ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + (dwSourceData & LE_BLT_dwGreenMask) - ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + (dwSourceData & LE_BLT_dwBlueMask) - ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE12_50)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + (dwSourceData & LE_BLT_dwRedMask) - ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + (dwSourceData & LE_BLT_dwGreenMask) - ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + (dwSourceData & LE_BLT_dwBlueMask) - ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE18_75)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE25_00)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE31_25)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 3) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE37_50)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE43_75)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE50_00)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE56_25)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) - ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) - ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) - ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE62_50)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE68_75)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE75_00)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE81_25)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 3) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE87_50)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE93_75)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          *((LPLONG)lpDestinationBltPointer) = dwAlphaData;
        }
      }
      lpDestinationBltPointer += 4;
      nPixelCounter --;
    }
    // update pointers to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt += nXSourceWidthInBytes;
    nLinesToBlt --;
  }
}
*/

/* Now with inline assembly. */
void LI_BLT32AlphaBitBlt8to32(
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

    // Run the MMX version of the routine if enabled.
#if CE_ARTLIB_EnableSystemMMX
    if(LE_MMX_Is_Available())
    {
        LI_MMXAlphaBitBlt8to32(lpDestinationBits, nXDestinationWidthInPixels,
                                    nXBltStartCoordinate, nYBltStartCoordinate,
                                    lpSourceBits, nXSourceWidthInPixels,
                                    lprSourceBltRect, lpColorTable,
                                    nAlphaInColorTable);
        return;
    }
#endif

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 4;

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
                        mov [edi], eax

                        inc ecx
            add edi, 4
                        jmp StartLineBlit

            CheckForAlphaModes:
                        // Get the alpha pixel, and set up for alpha tests.
                        shl eax, 3
                        add eax, dword ptr [lpColorTable]

                        push ecx    // Clear up cx, need it!

                        // Load up the alpha byte in al, destination dword in bx,
                        // and source dword in cx.
                        mov ecx, eax
                        mov ebx, dword ptr [ecx]
                        add ecx, 4

                        xor eax, eax
                        mov eax, dword ptr [ecx]
                        mov ecx, dword ptr [edi]
                        and eax, 0ffh

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
                    mov dword ptr [edi], eax
                    pop ecx

    FinishedWithPixel:
            add edi, 4
            inc ecx
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
/*
//Original C version
void LI_BLT32AlphaBitBlt32to32(
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
  register DWORD dwSourceData;              // holder for source data
  register DWORD dwDestinationData;         // holder for destination data
  register DWORD dwAlphaData;               // holder source and data blended
  register DWORD dwPureGreen;

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = nXSourceWidthInPixels * 4;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 4;

  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left * 4;

  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

  dwPureGreen = LI_BLT32ConvertColorRefTo32BitColor(LE_GRAFIX_MakeColorRef(0,255,0));

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
      dwDestinationData = *((LPLONG)lpSourceBltPointer);
      lpSourceBltPointer += 4;
      if (dwDestinationData != dwPureGreen)
      {
        dwSourceData = *((LPLONG)lpDestinationBltPointer);
        switch (nAlphaValue)
        {
          case ALPHA_OPAQUE06_25:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 4) + (dwSourceData & LE_BLT_dwRedMask) - ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + (dwSourceData & LE_BLT_dwGreenMask) - ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + (dwSourceData & LE_BLT_dwBlueMask) - ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE12_50:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 3) + (dwSourceData & LE_BLT_dwRedMask) - ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + (dwSourceData & LE_BLT_dwGreenMask) - ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + (dwSourceData & LE_BLT_dwBlueMask) - ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE18_75:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 3) + ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE25_00:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE31_25:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 2) + ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 3) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) + ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE37_50:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 2) + ((dwDestinationData & LE_BLT_dwRedMask) >> 3) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) + ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE43_75:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 1) - ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) - ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) - ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE50_00:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 1)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE56_25:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) - ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) - ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) - ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE62_50:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + ((dwDestinationData & LE_BLT_dwRedMask) >> 3) + ((dwSourceData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE68_75:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + ((dwDestinationData & LE_BLT_dwRedMask) >> 3) + ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + ((dwSourceData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE75_00:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + ((dwDestinationData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + ((dwDestinationData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + ((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE81_25:
            dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + ((dwDestinationData & LE_BLT_dwRedMask) >> 2) + ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + ((dwSourceData & LE_BLT_dwRedMask) >> 3) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + ((dwDestinationData & LE_BLT_dwGreenMask) >> 2) + ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + ((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE87_50:
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) - ((dwDestinationData & LE_BLT_dwRedMask) >> 3) + ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) - ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) - ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE93_75:
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) - ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) - ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) - ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
            break;
          case ALPHA_OPAQUE100_0:
            dwAlphaData = dwDestinationData;
            break;
          default:
            dwAlphaData = dwSourceData;
            break;
        }
        *((LPLONG)lpDestinationBltPointer) = dwAlphaData;
      }
      lpDestinationBltPointer += 4;
      nPixelCounter --;
    }
    // update pointers to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt += nXSourceWidthInBytes;
    nLinesToBlt --;
  }
}
*/
void LI_BLT32AlphaBitBlt32to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue)
{
  LPBYTE lpDestinationBeginBlt; // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;// destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;      // pointer to source scanline position to begin blt
  int nXSourceWidthInBytes;     // source bitmap width in bytes
  int nPixelsAcrossToBlt;       // blt width in pixels
  int nLinesToBlt;              // blt height in pixels
  DWORD dwPureGreen;            // Pure Green Cache..
  static DWORD  jump_array[17]; // Keeps offsets to alpha routines we jump to
  static BOOL  set_offsets = FALSE; // Keeps track of whether we set the jumps or not
  int dwMask132=0xFF00FF00;     // Bitmask 1
  int dwMask232=0x00FF00FF;     // Bitmask 2
  DWORD  *jump_array_point;     // Can't get the location of jump_array in assembly.
                                // Hacking my way around it.

  //Use MMX if available
  #if CE_ARTLIB_EnableSystemMMX
    if(LE_MMX_Is_Available())
    {
        LI_MMXAlphaBitBlt32to32(lpDestinationBits,
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

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = nXSourceWidthInPixels * 4;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                          (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 4;

  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left * 4;

  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

  dwPureGreen = LI_BLT32ConvertColorRefTo32BitColor(LE_GRAFIX_MakeColorRef(0,255,0));

  jump_array_point = jump_array;

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
    //Set the jump_array_point equal to where we need to jump to.
    mov eax, nAlphaValue
        shr eax, 4
        inc eax
        shl eax, 2
        add eax, jump_array_point
        mov eax, dword ptr [eax]
        mov jump_array_point, eax
    // Assume at least one line to blt(will change later if necessary)
    // Set source pointer.
    mov esi, lpSourceBeginBlt
    // Set destination pointer
    mov edi, lpDestinationBeginBlt
    //Assume at least one pixel to blt (ie: minimum blt >= (1,1))
StartLineBlt:
    mov ecx, nPixelsAcrossToBlt
    push ecx
GetNextPixel:
    //Snag the first pixel DX
    mov edx, [esi]
    add esi, 4
    //Move the jump point into eax
    mov eax, jump_array_point
    cmp edx, dwPureGreen
    je DoneWithPixel
    //Move the destination bits into CX
    mov ecx, [edi]
    //Jump to point in jump table.
    jmp eax
    //Warning, the following may be hard to follow as registers get re-used many
    //times over in a rotating fashion *GACK*
DO_ALPHA_OPAQUE06_25:
    //Do Mask1 Source (shortcut straight to ax)
    mov eax, edx
    and eax, dwMask132

    //Shift and add mask1 into ax
    shr eax, 4

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx (shortcut...keep it in dx since no work)
    shr edx, 4

    //Mask the result
    and edx, dwMask232

    //Or it into AX.
    or eax, edx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    mov edx, ebx
    shr ebx, 4
    sub edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    mov ebx, ecx
    shr ecx, 4
    sub ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination mask1 and mask2 with the source in ax
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE12_50:
    //Do Mask1 Source (shortcut, slap it into ax directly since no work)
    mov eax, edx
    and eax, dwMask132

    //Shift and add mask1 into ax
    shr eax, 3

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx (shortcut, keep it in dx)
    shr edx, 3

    //Mask the result.
    and edx, dwMask232

    //Or it into AX.
    or eax, edx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    mov edx, ebx
    shr ebx, 3
    sub edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    mov ebx, ecx
    shr ecx, 3
    sub ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination mask1 and mask2 with the source in ax
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE18_75:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    shr ebx, 3
    mov eax, ebx
    shr ebx, 1
    add eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx
    shr edx, 3
    mov ebx, edx
    shr edx, 1
    add ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    shr ebx, 1
    mov edx, ebx
    shr ebx, 1
    add edx, ebx
    shr ebx, 2
    add edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    shr ecx, 1
    mov ebx, ecx
    shr ecx, 1
    add ebx, ecx
    shr ecx, 2
    add ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination with the source
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE25_00:
    //Do Mask1 Source (shortcut, keep it in ax)
    mov ebx, edx
    and eax, dwMask132

    //Shift and add mask1 into ax
    shr eax, 2

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx
    shr edx, 2

    //Mask the result.
    and edx, dwMask232

    //Or it into AX.
    or eax, edx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    shr ebx, 1
    mov edx, ebx
    shr ebx, 1
    add edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    shr ecx, 1
    mov ebx, ecx
    shr ecx, 1
    add ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination with the source
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE31_25:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    shr ebx, 2
    mov eax, ebx
    shr ebx, 2
    add eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx

    shr edx, 2
    mov ebx, edx
    shr edx, 2
    add ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    shr ebx, 1
    mov edx, ebx
    shr ebx, 2
    add edx, ebx
    shr ebx, 1
    add edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    shr ecx, 1
    mov ebx, ecx
    shr ecx, 2
    add ebx, ecx
    shr ecx, 1
    add ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination with the source
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE37_50:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    shr ebx, 2
    mov eax, ebx
    shr ebx, 1
    add eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx
    shr edx, 2
    mov ebx, edx
    shr edx, 1
    add ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    shr ebx, 1
    mov edx, ebx
    shr ebx, 2
    add edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    shr ecx, 1
    mov ebx, ecx
    shr ecx, 2
    add ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination with the source
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE43_75:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    shr ebx, 1
    mov eax, ebx
    shr ebx, 3
    sub eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx
    shr edx, 1
    mov ebx, edx
    shr edx, 3
    sub ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    shr ebx, 1
    mov edx, ebx
    shr ebx, 3
    add edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    shr ecx, 1
    mov ebx, ecx
    shr ecx, 3
    add ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination with the source
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE50_00:
    //Do Mask1 Source (shortcut straight into ax)
    mov eax, edx
    and eax, dwMask132

    //Shift and add mask1 into ax
    shr eax, 1

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx (shortcut, keep in dx)
    shr edx, 1

    //Mask the result.
    and edx, dwMask232

    //Or it into AX.
    or eax, edx

    //Keep ax clean. (shortcut, keep it in dx)
    mov edx, ecx
    and edx, dwMask132

    //Shift and add into dx
    shr edx, 1

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx. (shortcut, keep in cx)
    shr ecx, 1

    //Mask the result in bx off
    and ecx, dwMask232

    //Merge the destination with the source
    add eax, ecx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE56_25:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    shr ebx, 1
    mov eax, ebx
    shr ebx, 3
    add eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx
    shr edx, 1
    mov ebx, edx
    shr edx, 3
    add ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    shr ebx, 1
    mov edx, ebx
    shr ebx, 3
    sub edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    shr ecx, 1
    mov ebx, ecx
    shr ecx, 3
    sub ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination with the source
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE62_50:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    shr ebx, 1
    mov eax, ebx
    shr ebx, 2
    add eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx
    shr edx, 1
    mov ebx, edx
    shr edx, 2
    add ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    shr ebx, 2
    mov edx, ebx
    shr ebx, 1
    add edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    shr ecx, 2
    mov ebx, ecx
    shr ecx, 2
    add ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination with the source
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE68_75:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    shr ebx, 1
    mov eax, ebx
    shr ebx, 2
    add eax, ebx
    shr ebx, 1
    add eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx
    shr edx, 1
    mov ebx, edx
    shr edx, 2
    add ebx, edx
    shr edx, 1
    add ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    shr ebx, 2
    mov edx, ebx
    shr ebx, 2
    add edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    shr ecx, 2
    mov ebx, ecx
    shr ecx, 2
    add ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination with the source
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE75_00:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    shr ebx, 1
    mov eax, ebx
    shr ebx, 1
    add eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx
    shr edx, 1
    mov ebx, edx
    shr edx, 1
    add ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean. (shortcut, keep it in dx)
    mov edx, ecx
    and edx, dwMask132

    //Shift and add into dx
    shr edx, 2

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx. (shortcut, keep it in cx)
    shr ecx, 2

    //Mask the result in bx off
    and ecx, dwMask232

    //Merge the destination with the source
    add eax, ecx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE81_25:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    shr ebx, 1
    mov eax, ebx
    shr ebx, 1
    add eax, ebx
    shr ebx, 2
    add eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx
    shr edx, 1
    mov ebx, edx
    shr edx, 1
    add ebx, edx
    shr edx, 2
    add ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean.
    mov ebx, ecx
    and ebx, dwMask132

    //Shift and add into dx
    shr ebx, 3
    mov edx, ebx
    shr ebx, 1
    mov edx, ebx

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx.
    shr ecx, 3
    mov ebx, ecx
    shr ecx, 1
    add ebx, ecx

    //Mask the result in bx off
    and ebx, dwMask232

    //Merge the destination with the source
    add eax, ebx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE87_50:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    mov eax, ebx
    shr ebx, 3
    sub eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add green into bx
    mov ebx, edx
    shr edx, 3
    sub ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean. (Shortcut, keep in dx)
    mov edx, ecx
    and edx, dwMask132

    //Shift and add into dx
    shr edx, 3

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx. (shortcut, keep in cx)
    shr ecx, 3

    //Mask the result in bx off
    and ecx, dwMask232

    //Merge the destination with the source
    add eax, ecx
    add eax, edx
    jmp Write_Alpha_Data

DO_ALPHA_OPAQUE93_75:
    //Do Mask1 Source
    mov ebx, edx
    and ebx, dwMask132

    //Shift and add mask1 into ax
    mov eax, ebx
    shr ebx, 4
    sub eax, ebx

    //Mask the result.
    and eax, dwMask132

    //Do Mask2 Source, since we don't need it anymore, we can do it in place
    and edx, dwMask232

    //Shift and add Mask2 into bx
    mov ebx, edx
    shr edx, 4
    sub ebx, edx

    //Mask the result.
    and ebx, dwMask232

    //Or it into AX.
    or eax, ebx

    //Keep ax clean. (Shortcut, keep in dx)
    mov edx, ecx
    and edx, dwMask132

    //Shift and add into dx
    shr edx, 4

    //Mask the result
    and edx, dwMask132

    //Do Mask2, we don't need the bits any more so can do it in place
    and ecx, dwMask232

    //Shift and add into bx. (shortcut, keep in cx)
    shr ecx, 4

    //Mask the result in bx off
    and ecx, dwMask232

    //Merge the destination with the source
    add eax, ecx
    add eax, edx
    jmp Write_Alpha_Data

Write_Alpha_Data:
    mov [edi], eax
DoneWithPixel:
    add edi, 4
    pop ecx //Counter Pop.
    dec ecx
    jz EndLineBlt
    push ecx //Counter Push.
    jmp GetNextPixel
EndLineBlt:
    dec nLinesToBlt
    jz EndBlt
    //Get source and destination pointers, and increment them
    mov esi, lpSourceBeginBlt
    mov edi, lpDestinationBeginBlt
    add esi, nXSourceWidthInBytes
    add edi, nXDestinationWidthInBytes
    mov lpSourceBeginBlt, esi
    mov lpDestinationBeginBlt, edi
    jmp StartLineBlt
EndBlt:
    //Restore pointers...
    pop EDI
    pop ESI
  };
}

/*******************************************************************************
*
*   Name:                   LI_BLT32ColorBlt
*
*   Description:  fills a destination RECT with a given color
*
*   Arguments:    lpDestinationBits - pointer to destination bitmap bits
*                 nXDestinationWidthInPixels - destination bitmap width
*                 lprDestinationBltRect - destination blt RECT
*                 crColorToFillWith - color to fill with, may be COLORREF
*                                     or may be 32 bit native color
*                 bColorRefIs32BitColor - flag: given color is 32 bit color
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT32ColorBlt(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  LPRECT lprDestinationBltRect,
  DWORD crColorToFillWith,
  BOOL bColorRefIs32BitColor)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  int nPixelsAcrossToBlt;         // blt width in pixels

  register LPBYTE lpDestinationBltPointer;  // pointer to current blt destination
  register int nLinesToBlt;                 // blt height in pixels
  register int nPixelCounter;               // pixel counter

  // convert COLORREF to 32 bit color (if necessary)
  if (!bColorRefIs32BitColor)
    crColorToFillWith = LI_BLT32ConvertColorRefTo32BitColor(crColorToFillWith);

  // calculate destination bitmap width in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
                    (lprDestinationBltRect->top * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += lprDestinationBltRect->left * 4;
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
      // fill destination scanline with DWORDs
      *((LPLONG)lpDestinationBltPointer) = crColorToFillWith;
      lpDestinationBltPointer += 4;
      nPixelCounter -= 1;
    }
    // update pointer to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    nLinesToBlt --;
  }
}


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
*                 lprDestinationClipRect - destination clip RECT
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

  // if destination RECT is NULL, do nothing
  if (((lprDestinationBltRect->bottom - lprDestinationBltRect->top) - 1) <= 0)
    return;
  if (((lprDestinationBltRect->right - lprDestinationBltRect->left) - 1) <= 0)
    return;

  // calculate 16:16 fractional ratio between destination and source scanlines
  nFractionalLines = (lprSourceBltRect->bottom - lprSourceBltRect->top) - 1;
  nFractionalLines <<= 16;
  nFractionalLines /= (lprDestinationBltRect->bottom - lprDestinationBltRect->top) - 1;

  // calculate 16:16 fractional ratio between destination and source scan widths
  nFractionalPixels = (lprSourceBltRect->right - lprSourceBltRect->left) - 1;
  nFractionalPixels <<= 16;
  nFractionalPixels /= (lprDestinationBltRect->right - lprDestinationBltRect->left) - 1;

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap using clip RECT
  lpDestinationBeginBlt = lpDestinationBits +
                          (lprDestinationClipRect->top * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += lprDestinationClipRect->left * 4;

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
      *((LPLONG)lpDestinationBltPointer) = lpColorTable[cData * 2];
      // otherwise skip over destination pixel
      lpDestinationBltPointer += 4;

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
*   Name:         LI_BLT32ColorKeyStretchBlt8to32
*
*   Description:  stretch blts an 8 bit paletted source bitmap to a 32 bit
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
void LI_BLT32ColorKeyStretchBlt8to32(
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

  // if destination RECT is NULL, do nothing
  if (((lprDestinationBltRect->bottom - lprDestinationBltRect->top) - 1) <= 0)
    return;
  if (((lprDestinationBltRect->right - lprDestinationBltRect->left) - 1) <= 0)
    return;

  // calculate 16:16 fractional ratio between destination and source scanlines
  nFractionalLines = (lprSourceBltRect->bottom - lprSourceBltRect->top) - 1;
  nFractionalLines <<= 16;
  nFractionalLines /= (lprDestinationBltRect->bottom - lprDestinationBltRect->top) - 1;

  // calculate 16:16 fractional ratio between destination and source scan widths
  nFractionalPixels = (lprSourceBltRect->right - lprSourceBltRect->left) - 1;
  nFractionalPixels <<= 16;
  nFractionalPixels /= (lprDestinationBltRect->right - lprDestinationBltRect->left) - 1;

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap using clip RECT
  lpDestinationBeginBlt = lpDestinationBits +
                          (lprDestinationClipRect->top * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += lprDestinationClipRect->left * 4;

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
        *((LPLONG)lpDestinationBltPointer) = lpColorTable[cData * 2];
      // otherwise skip over destination pixel
      lpDestinationBltPointer += 4;

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
*                 lprDestinationClipRect - destination clip RECT
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
  register DWORD dwSourceData;              // holder for source data
  register DWORD dwDestinationData;         // holder for destination data
  register DWORD dwAlphaData;               // holder source and data blended
  register BYTE cData;                      // 8 bit source index
  register BYTE cAlpha;                     // 8 bit alpha channel

  // if destination RECT is NULL, do nothing
  if (((lprDestinationBltRect->bottom - lprDestinationBltRect->top) - 1) <= 0)
    return;
  if (((lprDestinationBltRect->right - lprDestinationBltRect->left) - 1) <= 0)
    return;

  // calculate 16:16 fractional ratio between destination and source scanlines
  nFractionalLines = (lprSourceBltRect->bottom - lprSourceBltRect->top) - 1;
  nFractionalLines <<= 16;
  nFractionalLines /= (lprDestinationBltRect->bottom - lprDestinationBltRect->top) - 1;

  // calculate 16:16 fractional ratio between destination and source scan widths
  nFractionalPixels = (lprSourceBltRect->right - lprSourceBltRect->left) - 1;
  nFractionalPixels <<= 16;
  nFractionalPixels /= (lprDestinationBltRect->right - lprDestinationBltRect->left) - 1;

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap using clip RECT
  lpDestinationBeginBlt = lpDestinationBits +
                          (lprDestinationClipRect->top * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += lprDestinationClipRect->left * 4;

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
      {
        if (cData >= nAlphaInColorTable)
        {
          *((LPLONG)lpDestinationBltPointer) = lpColorTable[cData * 2];
        }
        else
        {
          cAlpha = (BYTE)lpColorTable[(cData * 2) + 1];
          dwDestinationData = lpColorTable[cData * 2];
          dwSourceData = *((LPLONG)lpDestinationBltPointer);
          if (cAlpha == ALPHA_OPAQUE06_25)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + (dwSourceData & LE_BLT_dwRedMask) - ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + (dwSourceData & LE_BLT_dwGreenMask) - ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + (dwSourceData & LE_BLT_dwBlueMask) - ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE12_50)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + (dwSourceData & LE_BLT_dwRedMask) - ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + (dwSourceData & LE_BLT_dwGreenMask) - ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + (dwSourceData & LE_BLT_dwBlueMask) - ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE18_75)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE25_00)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE31_25)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 3) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE37_50)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE43_75)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE50_00)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE56_25)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 1) - ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 1) - ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 1) - ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE62_50)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE68_75)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 2) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE75_00)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE81_25)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 3) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE87_50)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
          }
          else if (cAlpha == ALPHA_OPAQUE93_75)
          {
            dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
            dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
          }
          *((LPLONG)lpDestinationBltPointer) = dwAlphaData;
        }
      }
      // otherwise skip over destination pixel
      lpDestinationBltPointer += 4;

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
*   Name:         LI_BLT32ConvertColorRefTo32BitColor
*
*   Description:  converts a COLORREF to a 32 bit color
*
*   Arguments:    crColorToFillWith - COLORREF to convert to 32 bit color
*
*   Globals:      LE_BLT_nRedShift - number of first bit set in LE_BLT_dwRedMask (1 to 32)
*                 LE_BLT_nGreenShift -  number of first bit set in LE_BLT_dwGreenMask (1 - 32)
*                 LE_BLT_nBlueShift - number of first bit set in LE_BLT_dwBlueMask (1 - 32)
*                 LE_BLT_dwRedMask - mask of red bits for 32 bit DIBs
*                 LE_BLT_dwGreenMask - mask of green bits for 32 bit DIBs
*                 LE_BLT_dwBlueMask - mask of blue bits for 32 bit DIBs
*
*   Returns:      32 bit color
*
*******************************************************************************/
DWORD LI_BLT32ConvertColorRefTo32BitColor(
  COLORREF crColorToFillWith)
{
  DWORD dw32BitColor;   // converted 32 bit color (RETURNED)
  short nShift32;       // how many bits to shift values from COLORREF

  // shift and mask red bit values from COLORREF
  nShift32 = LE_BLT_nRedShift - 8;
  if (nShift32 >= 0)
    dw32BitColor = ((crColorToFillWith & 0xff) << nShift32) & LE_BLT_dwRedMask;
  else
    dw32BitColor = ((crColorToFillWith & 0xff) >> -nShift32) & LE_BLT_dwRedMask;

  // shift and mask green bit values from COLORREF
  nShift32 = LE_BLT_nGreenShift - 16;
  if (nShift32 >= 0)
    dw32BitColor |= ((crColorToFillWith & 0xff00) << nShift32) & LE_BLT_dwGreenMask;
  else
    dw32BitColor |= ((crColorToFillWith & 0xff00) >> -nShift32) & LE_BLT_dwGreenMask;

  // shift and mask blue bit values from COLORREF
  nShift32 = LE_BLT_nBlueShift - 24;
  if (nShift32 >= 0)
    dw32BitColor |= ((crColorToFillWith & 0xff0000) << nShift32) & LE_BLT_dwBlueMask;
  else
    dw32BitColor |= ((crColorToFillWith & 0xff0000) >> -nShift32) & LE_BLT_dwBlueMask;

  // return converted 32 bit color
  return(dw32BitColor);
}


/*******************************************************************************
*
*   Name:         LI_BLT32ConvertRGBColorTableTo32BitColor
*
*   Description:  converts an RGBQUAD color table to 32 bit colors
*
*   Arguments:    lpRGBColorTable - pointer to RGBQUAD color table
*                 nColorTableEntries - number of entries in color table
*
*   Globals:      LE_BLT_nRedShift - number of first bit set in LE_BLT_dwRedMask (1 to 32)
*                 LE_BLT_nGreenShift -  number of first bit set in LE_BLT_dwGreenMask (1 - 32)
*                 LE_BLT_nBlueShift - number of first bit set in LE_BLT_dwBlueMask (1 - 32)
*                 LE_BLT_dwRedMask - mask of red bits for 32 bit DIBs
*                 LE_BLT_dwGreenMask - mask of green bits for 32 bit DIBs
*                 LE_BLT_dwBlueMask - mask of blue bits for 32 bit DIBs
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT32ConvertRGBColorTableTo32BitColor(
  LPLONG lpRGBColorTable,
  int nColorTableEntries)
{
  int nColorTableIndex;       // color table index
  DWORD dw32BitColor;         // 32 bit color value
  DWORD dwRGBValue;           // RGBQUAD value
  short nShift32;             // how many bits to shift values from RGBQUAD

  // store 32 bit table in same location as RGBQUAD table
  for (nColorTableIndex = 0; nColorTableIndex < nColorTableEntries; nColorTableIndex ++)
  {
    // get RGBQUAD value as a DWORD
    dwRGBValue = *lpRGBColorTable;

    // shift and mask blue bit values from RGBQUAD
    nShift32 = LE_BLT_nBlueShift - 8;
    if (nShift32 >= 0)
      dw32BitColor = ((dwRGBValue & 0xff) << nShift32) & LE_BLT_dwBlueMask;
    else
      dw32BitColor = ((dwRGBValue & 0xff) >> -nShift32) & LE_BLT_dwBlueMask;

    // shift and mask green bit values from RGBQUAD
    nShift32 = LE_BLT_nGreenShift - 16;
    if (nShift32 >= 0)
      dw32BitColor |= ((dwRGBValue & 0xff00) << nShift32) & LE_BLT_dwGreenMask;
    else
      dw32BitColor |= ((dwRGBValue & 0xff00) >> -nShift32) & LE_BLT_dwGreenMask;

    // shift and mask red bit values from RGBQUAD
    nShift32 = LE_BLT_nRedShift - 24;
    if (nShift32 >= 0)
      dw32BitColor |= ((dwRGBValue & 0xff0000) << nShift32) & LE_BLT_dwRedMask;
    else
      dw32BitColor |= ((dwRGBValue & 0xff0000) >> -nShift32) & LE_BLT_dwRedMask;

    // store 32 bit color value
    *lpRGBColorTable = dw32BitColor;
    // increment color table pointer
    lpRGBColorTable ++;
    lpRGBColorTable ++;
  }
}

/*******************************************************************************
*
*   Name:                   LI_BLT32ColorKeyPureGreenBitBlt32to32
*
*   Description:  blts from a 32 bit bitmap to a 32 bit bitmap
*                 A color of pure green is considered to be transparent.
*                 Well, we now allow nearly green or blue.
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

void LI_BLT32ColorKeyPureGreenBitBlt32to32(
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
  LPDWORD curSourceBlt;                   // mmh
  LPDWORD curDestinationBlt;              // mmh
  DWORD   redLimit;
  DWORD   greenLimit;
  DWORD   blueLimit;
  DWORD   redMask = LE_BLT_dwRedMask;
  DWORD   greenMask = LE_BLT_dwGreenMask;
  DWORD   blueMask = LE_BLT_dwBlueMask;
  DWORD   rawPixel;
  
  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = nXDestinationWidthInPixels * 4;
  nXSourceWidthInBytes = nXSourceWidthInPixels * 4;
  
  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits + (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate * 4;
  
  // calculate start position of blt in source bitmap
  lpSourceBeginBlt = lpSourceBits + (lprSourceBltRect->top * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left * 4;
  
  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nBytesAcrossToBlt  = (lprSourceBltRect->right - lprSourceBltRect->left) * 4;
  nPixelsAcrossToBlt = (lprSourceBltRect->right - lprSourceBltRect->left);
  
  // Find the colour limits as values corresponding to the current pixel depth.
  redLimit = LI_BLT32ConvertColorRefTo32BitColor (
    LE_GRAFIX_MakeColorRef (LE_BLT_KeyRedLimit, 0, 0));
  greenLimit = LI_BLT32ConvertColorRefTo32BitColor (
    LE_GRAFIX_MakeColorRef (0, LE_BLT_KeyGreenLimit, 0));
  blueLimit = LI_BLT32ConvertColorRefTo32BitColor (
    LE_GRAFIX_MakeColorRef (0, 0, LE_BLT_KeyBlueLimit));
  
  while (nLinesToBlt > 0)
  {
    curDestinationBlt = (DWORD*)lpDestinationBeginBlt;
    curSourceBlt      = (DWORD*)lpSourceBeginBlt;
    pixelcount = 0;
    while (pixelcount < nPixelsAcrossToBlt)
    {
#if CE_ARTLIB_BlitUseGreenColourKey
      if (((rawPixel = *curSourceBlt) & redMask) >= redLimit ||
      (rawPixel & greenMask) < greenLimit ||
      (rawPixel & blueMask) >= blueLimit)
#elif CE_ARTLIB_BlitUseBlueColourKey
      if (((rawPixel = *curSourceBlt) & redMask) >= redLimit ||
      (rawPixel & greenMask) >= greenLimit ||
      (rawPixel & blueMask) < blueLimit)
#else // Just use absolute black as transparent.
      if ((rawPixel = *curSourceBlt) != 0)
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
/************************************************************/

/*******************************************************************************
*
*   Name:         LI_BLT32ConvertRGBValuesTo32BitColor
*
*   Description:  converts a 24 bit DIB to 32 bit DIB
*
*   Arguments:    lp32BitData - pointer to 32 bit DIB bits
*                 lp24BitData - pointer to 24 bit DIB bits
*                                   sWidthBitmapInPixels - width of source DIB in pixels
*                                   sHeightBitmapInPixels - height of source DIB in pixels
*                                   dWidthBitmapInPixels - width of dest DIB in pixels
*                                   dHeightBitmapInPixels - height of dest DIB in pixels
*
*   Globals:      LE_BLT_nRedShift - number of first bit set in LE_BLT_dwRedMask (1 to 32)
*                 LE_BLT_nGreenShift -  number of first bit set in LE_BLT_dwGreenMask (1 - 32)
*                 LE_BLT_nBlueShift - number of first bit set in LE_BLT_dwBlueMask (1 - 32)
*                 LE_BLT_dwRedMask - mask of red bits for 32 bit DIBs
*                 LE_BLT_dwGreenMask - mask of green bits for 32 bit DIBs
*                 LE_BLT_dwBlueMask - mask of blue bits for 32 bit DIBs
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT32ConvertRGBValuesTo32BitColor(
  LPBYTE lp32BitData,
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
    register DWORD dw32BitColor;                // converted 32 bit pixel
    register DWORD dw24BitData1;                // source data storage
  register DWORD dw24BitData2;
  register DWORD dw24BitData3;
    register short nShift32;                    // how many bits to shift values from 24 bit DIB

  // calculate bitmap widths in bytes
    nXDestinationWidthInBytes = dest_w_pixels * 4;
    nXSourceWidthInBytes      = ((src_w_pixels * 3) + 3) & 0xfffffffc;

  // initialize start scanline pointers
  lpDestinationBeginBlt = lp32BitData;
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

        // convert 24 bits of data to 32 bits
        nShift32 = LE_BLT_nBlueShift - 8;
        if (nShift32 >= 0)
          dw32BitColor = ((dw24BitData1 & 0xff) << nShift32) & LE_BLT_dwBlueMask;
        else
          dw32BitColor = ((dw24BitData1 & 0xff) >> -nShift32) & LE_BLT_dwBlueMask;
        nShift32 = LE_BLT_nGreenShift - 16;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData1 & 0xff00) << nShift32) & LE_BLT_dwGreenMask;
        else
          dw32BitColor |= ((dw24BitData1 & 0xff00) >> -nShift32) & LE_BLT_dwGreenMask;
        nShift32 = LE_BLT_nRedShift - 24;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData1 & 0xff0000) << nShift32) & LE_BLT_dwRedMask;
        else
          dw32BitColor |= ((dw24BitData1 & 0xff0000) >> -nShift32) & LE_BLT_dwRedMask;

        // store 32 bit color
        *((LPLONG)lpDestinationBltPointer) = dw32BitColor;
        lpDestinationBltPointer += 4;

        // convert 24 bits of data to 32 bits
        nShift32 = LE_BLT_nBlueShift - 32;
        if (nShift32 >= 0)
          dw32BitColor = ((dw24BitData1 & 0xff000000) << nShift32) & LE_BLT_dwBlueMask;
        else
          dw32BitColor = ((dw24BitData1 & 0xff000000) >> -nShift32) & LE_BLT_dwBlueMask;
        nShift32 = LE_BLT_nGreenShift - 8;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData2 & 0xff) << nShift32) & LE_BLT_dwGreenMask;
        else
          dw32BitColor |= ((dw24BitData2 & 0xff) >> -nShift32) & LE_BLT_dwGreenMask;
        nShift32 = LE_BLT_nRedShift - 16;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData2 & 0xff00) << nShift32) & LE_BLT_dwRedMask;
        else
          dw32BitColor |= ((dw24BitData2 & 0xff00) >> -nShift32) & LE_BLT_dwRedMask;

        // store 32 bit color
        *((LPLONG)lpDestinationBltPointer) = dw32BitColor;
        lpDestinationBltPointer += 4;

        // convert 24 bits of data to 32 bits
        nShift32 = LE_BLT_nBlueShift - 24;
        if (nShift32 >= 0)
          dw32BitColor = ((dw24BitData2 & 0xff0000) << nShift32) & LE_BLT_dwBlueMask;
        else
          dw32BitColor = ((dw24BitData2 & 0xff0000) >> -nShift32) & LE_BLT_dwBlueMask;
        nShift32 = LE_BLT_nGreenShift - 32;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData2 & 0xff000000) << nShift32) & LE_BLT_dwGreenMask;
        else
          dw32BitColor |= ((dw24BitData2 & 0xff000000) >> -nShift32) & LE_BLT_dwGreenMask;
        nShift32 = LE_BLT_nRedShift - 8;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData3 & 0xff) << nShift32) & LE_BLT_dwRedMask;
        else
          dw32BitColor |= ((dw24BitData3 & 0xff) >> -nShift32) & LE_BLT_dwRedMask;

        // store 32 bit color
        *((LPLONG)lpDestinationBltPointer) = dw32BitColor;
        lpDestinationBltPointer += 4;

        // convert 24 bits of data to 32 bits
        nShift32 = LE_BLT_nBlueShift - 16;
        if (nShift32 >= 0)
          dw32BitColor = ((dw24BitData3 & 0xff00) << nShift32) & LE_BLT_dwBlueMask;
        else
          dw32BitColor = ((dw24BitData3 & 0xff00) >> -nShift32) & LE_BLT_dwBlueMask;
        nShift32 = LE_BLT_nGreenShift - 24;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData3 & 0xff0000) << nShift32) & LE_BLT_dwGreenMask;
        else
          dw32BitColor |= ((dw24BitData3 & 0xff0000) >> -nShift32) & LE_BLT_dwGreenMask;
        nShift32 = LE_BLT_nRedShift - 32;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData3 & 0xff000000) << nShift32) & LE_BLT_dwRedMask;
        else
          dw32BitColor |= ((dw24BitData3 & 0xff000000) >> -nShift32) & LE_BLT_dwRedMask;

        // store 32 bit color
        *((LPLONG)lpDestinationBltPointer) = dw32BitColor;
        lpDestinationBltPointer += 4;
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

        // convert 24 bits of data to 32 bits
        nShift32 = LE_BLT_nBlueShift - 8;
        if (nShift32 >= 0)
          dw32BitColor = ((dw24BitData1 & 0xff) << nShift32) & LE_BLT_dwBlueMask;
        else
          dw32BitColor = ((dw24BitData1 & 0xff) >> -nShift32) & LE_BLT_dwBlueMask;
        nShift32 = LE_BLT_nGreenShift - 16;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData1 & 0xff00) << nShift32) & LE_BLT_dwGreenMask;
        else
          dw32BitColor |= ((dw24BitData1 & 0xff00) >> -nShift32) & LE_BLT_dwGreenMask;
        nShift32 = LE_BLT_nRedShift - 24;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData1 & 0xff0000) << nShift32) & LE_BLT_dwRedMask;
        else
          dw32BitColor |= ((dw24BitData1 & 0xff0000) >> -nShift32) & LE_BLT_dwRedMask;

        // store 32 bit color
        *((LPLONG)lpDestinationBltPointer) = dw32BitColor;
        lpDestinationBltPointer += 4;

        // convert 24 bits of data to 32 bits
        nShift32 = LE_BLT_nBlueShift - 32;
        if (nShift32 >= 0)
          dw32BitColor = ((dw24BitData1 & 0xff000000) << nShift32) & LE_BLT_dwBlueMask;
        else
          dw32BitColor = ((dw24BitData1 & 0xff000000) >> -nShift32) & LE_BLT_dwBlueMask;
        nShift32 = LE_BLT_nGreenShift - 8;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData2 & 0xff) << nShift32) & LE_BLT_dwGreenMask;
        else
          dw32BitColor |= ((dw24BitData2 & 0xff) >> -nShift32) & LE_BLT_dwGreenMask;
        nShift32 = LE_BLT_nRedShift - 16;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData2 & 0xff00) << nShift32) & LE_BLT_dwRedMask;
        else
          dw32BitColor |= ((dw24BitData2 & 0xff00) >> -nShift32) & LE_BLT_dwRedMask;

        // store 32 bit color
        *((LPLONG)lpDestinationBltPointer) = dw32BitColor;
        lpDestinationBltPointer += 4;
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

        // convert 24 bits of data to 32 bits
        nShift32 = LE_BLT_nBlueShift - 8;
        if (nShift32 >= 0)
          dw32BitColor = ((dw24BitData1 & 0xff) << nShift32) & LE_BLT_dwBlueMask;
        else
          dw32BitColor = ((dw24BitData1 & 0xff) >> -nShift32) & LE_BLT_dwBlueMask;
        nShift32 = LE_BLT_nGreenShift - 16;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData1 & 0xff00) << nShift32) & LE_BLT_dwGreenMask;
        else
          dw32BitColor |= ((dw24BitData1 & 0xff00) >> -nShift32) & LE_BLT_dwGreenMask;
        nShift32 = LE_BLT_nRedShift - 8;
        if (nShift32 >= 0)
          dw32BitColor |= ((dw24BitData2 & 0xff) << nShift32) & LE_BLT_dwRedMask;
        else
          dw32BitColor |= ((dw24BitData2 & 0xff) >> -nShift32) & LE_BLT_dwRedMask;

        // store 32 bit color
        *((LPLONG)lpDestinationBltPointer) = dw32BitColor;
        lpDestinationBltPointer += 4;
        nPixelCounter -= 1;
      }
    }
    // update pointers to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt -= nXSourceWidthInBytes;
  }
}
/************************************************************/

#endif // CE_ARTLIB_EnableSystemGrafix2D && CE_ARTLIB_BlitUseOldRoutines
