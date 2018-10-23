/*****************************************************************************
 *
 * FILE:        L_BltFil.cpp
 * DESCRIPTION: A snippet of C source code for filling in a rectangular
 *              area in a bitmap.  Various defines control which variety
 *              of fill code is generated.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/LibraryCode/L_BltFil.cpp 2     9/17/98 4:18p Agmsmith $
 *
 * The invoker specifies #defines with these names to set the options:
 * INC_DEST_DEPTH - pixel destination bitmap depth: 16, 24, 32.
 * INC_DEST_FORMAT - INC_GENERAL for NEWBITMAPHEADER format bitmap or
 *                    INC_RAWBITMAP for pointer to bits and separate width and
 *                    height for the source bitmap.
 *
 * The options in the function name are:
 * General - Uses the bitmap header to figure it out.
 * Raw - You specify the bitmap width, height and palette separately.
 *****************************************************************************
 * $Log: /Artlib_99/LibraryCode/L_BltFil.cpp $
 * 
 * 2     9/17/98 4:18p Agmsmith
 * Added colour filling functions, compiles under C++ now.
 * 
 * 1     9/17/98 4:04p Agmsmith
 * Code fragment for filling in solid colour rectangles.
 ****************************************************************************/

#define BYTES_PER_PIXEL (INC_DEST_DEPTH / 8)
  // A convenience define.

// First make up the function name, it is of the form
// LI_BLT_Fill(dest depth)(options).

#if INC_DEST_FORMAT == INC_RAWBITMAP
  #define DEST_FORMAT_OPTION Raw
#else
  #error Invalid definition of the destination format define, you used INC_DEST_FORMAT.
#endif

LateDefine FUNCTION_NAME LI_BLT_Fill ## INC_DEST_DEPTH ## DEST_FORMAT_OPTION
LateDefine NAME_TO_STRING(a,b) "LI_BLT_Fill" #a #b
LateDefine NAME_AS_STRING NAME_TO_STRING (INC_DEST_DEPTH, DEST_FORMAT_OPTION)
LateDefine CONVERT_COLOUR_REF_FUNCTION LI_BLT_ConvertColorRefTo ## INC_DEST_DEPTH ## BitColor

LatePragma message ("Now compiling " NAME_AS_STRING)


/* Generate the function header.  It looks something like this:
BOOL LI_BLT_Fill16Raw (
  DWORD ColourToFillWith, // Either a native depth pixel or a COLORREF.
  BOOL  ColourIsNative,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestRectangle)
*/

BOOL FUNCTION_NAME (
  DWORD ColourToFillWith,
  BOOL  ColourIsNative,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestRectangle)
{
  LPBYTE lpDestinationBeginBlt;   // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes;  // destination bitmap width in bytes
  int nPixelsAcrossToBlt;         // blt width in pixels

  register LPBYTE lpDestinationBltPointer;  // pointer to current blt destination
  register int nLinesToBlt;                 // blt height in pixels
  register int nPixelCounter;               // pixel counter

  // Convert COLORREF to native pixel if necessary.
  if (!ColourIsNative)
    ColourToFillWith = CONVERT_COLOUR_REF_FUNCTION (ColourToFillWith);

  // calculate destination bitmap width in bytes
  nXDestinationWidthInBytes = ((DestinationBitmapWidth * BYTES_PER_PIXEL) + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = DestinationBitmapBits +
                    (DestRectangle->top * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += DestRectangle->left * BYTES_PER_PIXEL;

  // calculate dimensions of blt
  nLinesToBlt = DestRectangle->bottom - DestRectangle->top;
  nPixelsAcrossToBlt = DestRectangle->right - DestRectangle->left;
  while (nLinesToBlt > 0)
  {
    // initialize current blt pointer
    lpDestinationBltPointer = lpDestinationBeginBlt;

    // initialize pixel counter
    nPixelCounter = nPixelsAcrossToBlt;
    while (nPixelCounter > 0)
    {
      // Inefficiently fill the pixels.  Would be better to have code for each
      // separate depth here, but this is an unimportant function so it doesn't
      // matter that much.

      *lpDestinationBltPointer++ = (BYTE) ColourToFillWith;
      *lpDestinationBltPointer++ = (BYTE) (ColourToFillWith >> 8);
#if BYTES_PER_PIXEL >= 3
      *lpDestinationBltPointer++ = (BYTE) (ColourToFillWith >> 16);
#endif
#if BYTES_PER_PIXEL >= 4
      *lpDestinationBltPointer++ = (BYTE) (ColourToFillWith >> 24);
#endif
      nPixelCounter -= 1;
    }
    // update pointer to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    nLinesToBlt --;
  }
  return TRUE;
}


// Cleanup - undo all the definitions we made in this file.

#undef DEST_FORMAT_OPTION
#undef BYTES_PER_PIXEL
LateUndef FUNCTION_NAME
LateUndef NAME_TO_STRING
LateUndef NAME_AS_STRING
LateUndef CONVERT_COLOUR_REF_FUNCTION
