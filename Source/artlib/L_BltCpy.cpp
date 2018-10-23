/*****************************************************************************
 *
 * FILE:        L_BLTCPY.C
 * DESCRIPTION: A snippet of C source code for copying pixels between bitmaps,
 *              it is repeatedly included by L_BLT.c with various different
 *              #defines to control code generation of multiple varieties of
 *              the same pixel copying function.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_BltCpy.cpp 12    6/25/99 1:07p Lzou $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_BltCpy.cpp $
 * 
 * 12    6/25/99 1:07p Lzou
 * Fixed a bug in blt with alpha channel effects. The bug sometimes causes
 * wrong colour effects along edges in 16 bit colour depth.
 * 
 * 11    4/07/99 5:10p Lzou
 * Enabled (8 bit) blit stretch copy routines to use full 255 levels of
 * alpha channel effects.
 * 
 * 10    4/07/99 4:56p Lzou
 * Enabled (8 bit) blit unity copy routines to use full 255 levels of
 * alpha channel effects.
 * 
 * 9     4/05/99 5:12p Lzou
 * Have implemented bitmap stretch copy with full 255 levels of alpha
 * channel effects for 16to16, 24to24, and 32to32 bit colour depths.
 * 
 * 8     4/03/99 4:09p Agmsmith
 * Fix corrupted SourceSafe history.
 * 
 * 8     4/01/99 5:35p Lzou
 * Have implemented bitmap unity copy with 255 alpha levels for 16to16,
 * 24to24, and 32to32 bit colour depths.
 * 
 * 7     3/30/99 12:35p Lzou
 * Some small changes.
 * 
 * 6     3/30/99 12:20p Lzou
 * Fixed one potential bug for 24 bit colour depth.
 * 
 * 5     3/29/99 3:22p Lzou
 * Have implemented bitmap stretch copy with alpha channel effects for
 * 16to16, 24to24, and 32to32 bit colour depths.
 * 
 * 4     3/26/99 5:23p Lzou
 * Fixed one bug in bitmap stretch copy with alpha channel effects from 8
 * to 24 bit colour depth.
 * 
 * 3     3/24/99 5:39p Lzou
 * Have implemented bitmap unity copy with alpha channel effects for 16 to
 * 16 bit colour depth, 24 to 24 bit colour depth, and 32 to 32 bit colour
 * depth.
 * 
 * 2     2/03/99 1:45p Lzou
 * Fixed one bug in unity bitmap copy with holes from 8 to 24 bit colour
 * depth.
 * 
 * 1     2/03/99 1:45p Agmsmith
 * SourceSafe went corrupt again, deleted and replaced this file.
 * 
 * 9     2/02/99 10:24a Lzou
 * We have both colour key and holes copy modes from 8 to 16, 24, and 32
 * bit colour depth.
 * 
 * 8     1/29/99 5:06p Lzou
 * Fixed a bug in alpha channel unity bitmap copy from 8 to 24 bit colour
 * depth.
 * 
 * 7     1/28/99 11:39a Lzou
 * Fixed a bug in unity raw bitmap copy from 8 to 24 bit colour depth.
 * 
 * 6     1/22/99 4:47p Lzou
 * Fixed a bug in unity bitmap copy with holes from 8 to 24 bit colour
 * depth.
 * 
 * 5     1/22/99 1:09p Lzou
 * Added in unity bitmap copy with holes from 8 to screen native colour
 * depth.
 * 
 * 4     12/18/98 5:37p Agmsmith
 * Add L_BLT in front of global variables, dwRedShift is just too
 * anonymous.
 * 
 * 3     9/17/98 4:18p Agmsmith
 * Added colour filling functions, compiles under C++ now.
 * 
 * 2     9/15/98 11:56a Agmsmith
 * Converted to use C++
 * 
 * 17    9/10/98 10:41a Agmsmith
 * So include C_Main.h since it is needed.
 * 
 * 16    8/17/98 5:04p Lzou
 * Have done a proof reading. A bug is fixed.
 * 
 * 15    8/17/98 1:25p Lzou
 * Have implemented Holes & Stretching bitmap copy.
 * 
 * 14    8/14/98 3:10p Lzou
 * I am doing Holes & Stretching bitmap copy.
 * 
 * 13    8/13/98 5:29p Lzou
 * Have implemented Solid with Colour Key & Unity bitmap copy.
 * 
 * 12    8/13/98 2:06p Lzou
 * A bug is fixed. The problem is that C_MAIN.h is not included during one
 * of the two levels of preprocessing.
 * 
 * 11    8/13/98 10:18a Lzou
 * Test for the colour key bitmap copy.
 * 
 * 10    8/12/98 5:46p Lzou
 * Debugging the colour key blt.
 * 
 * 9     8/12/98 4:56p Lzou
 * Have implemented Solid with Colour Key & Stretching bitmap copy (from 8
 * bit to 16, 24, and 32 bit colour depth).
 * 
 * 8     8/12/98 1:02p Lzou
 * Have implemented Solid with Colour Key & Stretching bitmap copy.
 * 
 * 7     8/12/98 10:15a Lzou
 * I am now working on Solid with Colour Key & Unity/Stretching bitmap
 * copy.
 * 
 * 6     8/11/98 1:00p Lzou
 * A bug is fixed in bitmap 24 to 24 bit stretching copy.
 * 
 * 5     8/11/98 11:23a Lzou
 * The BLT should now be faster than it was yesterday.
 * 
 * 4     8/10/98 4:34p Lzou
 * Have modified function argument list to include DestInvalidRectangle
 * and DestBoundingRectangle in a bid to make blt faster.
 * 
 * 1     8/10/98 11:39a Lzou
 * Working on BLT speed improving. This is a temporary check in.
 * 
 * 3     8/05/98 5:52p Lzou
 * Have done proof reading.
 * 
 * 2     8/04/98 3:05p Lzou
 * Check my BLT routines into Artech production library.
 * 
 * 16    8/04/98 2:26p Lzou
 * Have finished testing Solid & Unity/Stretching and Alpha &
 * Unity/Stretching BLT routines.
 * 
 * 15    7/27/98 4:50p Lzou
 * Have removed most of function calls from the Stretching & Solid bitmap
 * copy. Use inline code instead.
 * 
 * 14    7/23/98 4:42p Lzou
 * Have removed function calls for the same source/destination colour
 * depth in the Solid & Unity bitmap copy.
 * 
 * 13    7/22/98 3:05p Lzou
 * Have done the Alpha & Stretching bitmap copy.
 * 
 * 12    7/22/98 1:09p Lzou
 * Nothing really important.
 * 
 * 11    7/22/98 1:02p Lzou
 * Have finished the Alpha & Unity bitmap copy.
 * 
 * 10    7/21/98 4:09p Lzou
 * Temporarily check in before implementing the Alpha & Unity, Alpha &
 * Stretching bitmap copies.
 * 
 * 9     7/16/98 6:20p Lzou
 * Using inline code to replace function calls in a bid to make the blt
 * faster.
 * 
 * 8     7/15/98 12:53p Lzou
 * Minute change.
 * 
 * 7     7/15/98 12:39p Lzou
 * Have added in some comments on and descriptions of both the Solid &
 * Unity and Solid & Stretching bitmap copies.
 * 
 * 6     7/15/98 12:17p Lzou
 * Have finished the Solid & Stretching bitmap copy.
 * 
 * 5     7/14/98 2:32p Lzou
 * Have done all of the Unity & Solid bitmap copy. Have partly done the
 * Stretching & Solid bitmap copy.
 * 
 * 4     7/10/98 1:32p Lzou
 * Have finished implementing the solid & unity bitmap copy from 8 bit
 * colour depth to 16 bit, 24 bit, and 32 bit colour depth.
 * 
 * 3     7/06/98 1:06p Lzou
 * A temporarily check in.
 * 
 * 2     7/03/98 5:27p Lzou
 * a bit is done.
 * 
 * 1     7/02/98 3:22p Lzou
 * Implement the BLT module
 * 
 * 1     6/12/98 3:40p Agmsmith
 * Preprocessor code generation of the various different blitter copy
 * functions.
 ****************************************************************************/


// First make up the function name, it is of the form
// LI_BLT_Copy(source depth)To(dest depth)(options).
//
// The invoker specifies #defines with these names to set the options:
// INC_SOURCE_DEPTH - pixel source depth: 8 (for TABs only), 16, 24, 32.
// INC_DEST_DEPTH - pixel destination bitmap depth: 16, 24, 32.
// INC_COPYMODE - one of INC_SOLID, INC_HOLES, INC_ALPHA.
// INC_COLOURKEY - either TRUE (1) for green is transparent or FALSE (0).
// INC_STRETCHMODE - INC_UNITY for no stretching or INC_STRETCH for stretching.
// INC_SOURCEFORMAT - INC_GENERAL for NEWBITMAPHEADER format source bitmap or
//                    INC_RAWBITMAP for pointer to bits and separate width and
//                    height for the source bitmap.
//
// The options in the function name are:
// Solid - Copy with no transparency tests at all.
// Holes - Copy with colour index zero being transparent, rest solid.
// Alpha - Copy with blended transparency, either from palette or as a number.
// ColourKey - Copy with greenish or some other colour being transparent.
// General - Uses the bitmap header to figure it out.
// Raw - You specify the bitmap width, height and palette separately.
// Unity - Source and destination pixels are copied at a 1 to 1 ratio, no stretch.
// Stretch - Source and destination can be different sizes.


#if INC_COPYMODE == INC_SOLID
  #if INC_COLOURKEY
    #define COPY_OPTION SolidColourKey
  #else
    #define COPY_OPTION Solid
  #endif
#elif INC_COPYMODE == INC_HOLES
  #define COPY_OPTION Holes
#elif INC_COPYMODE == INC_ALPHA
  #if INC_COLOURKEY
    #define COPY_OPTION AlphaColourKey
  #else
    #define COPY_OPTION Alpha
  #endif
#else
  #error Invalid definition of the copy mode define, you used INC_COPYMODE.
#endif

/*  this bit doesn't work
#if INC_COLOURKEY
  #define COPY_OPTION_WITH_KEY COPY_OPTION ## ColourKey
#else
  #define COPY_OPTION_WITH_KEY COPY_OPTION
#endif
*/

#if INC_STRETCHMODE == INC_UNITY
  #define STRETCH_OPTION Unity
#elif INC_STRETCHMODE == INC_STRETCH
  #define STRETCH_OPTION Stretch
#else
  #error Invalid definition of the stretch mode define, you used INC_STRETCHMODE.
#endif

#if INC_SOURCEFORMAT == INC_GENERAL
  #define SOURCE_FORMAT_OPTION General
#elif INC_SOURCEFORMAT == INC_RAWBITMAP
  #define SOURCE_FORMAT_OPTION Raw
#else
  #error Invalid definition of the source format define, you used INC_SOURCEFORMAT.
#endif

// LateDefine FUNCTION_NAME LI_BLT_Copy ## INC_SOURCE_DEPTH ## To ## INC_DEST_DEPTH ## COPY_OPTION_WITH_KEY ## STRETCH_OPTION ## SOURCE_FORMAT_OPTION
LateDefine FUNCTION_NAME LI_BLT_Copy ## INC_SOURCE_DEPTH ## To ## INC_DEST_DEPTH ## COPY_OPTION ## STRETCH_OPTION ## SOURCE_FORMAT_OPTION
LateDefine NAME_TO_STRING(a,b,c,d,e) "LI_BLT_Copy" #a "To" #b #c #d #e
// LateDefine NAME_AS_STRING NAME_TO_STRING (INC_SOURCE_DEPTH, INC_DEST_DEPTH, COPY_OPTION_WITH_KEY, STRETCH_OPTION, SOURCE_FORMAT_OPTION)
LateDefine NAME_AS_STRING NAME_TO_STRING (INC_SOURCE_DEPTH, INC_DEST_DEPTH, COPY_OPTION, STRETCH_OPTION, SOURCE_FORMAT_OPTION)

LatePragma message ("Now compiling " NAME_AS_STRING)


/* Generate the function header.  It looks something like this:
BOOL LI_BLT_Copy8To16AlphaStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LE_BLT_AlphaPaletteEntryPointer SourceColourTable,
  UNS16 NumberOfAlphaColours,
  TYPE_RectPointer SourceRectangle, 
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle)
*/

BOOL FUNCTION_NAME (

// First the arguments describing the source bitmap.
#if INC_SOURCEFORMAT == INC_GENERAL
  LPNEWBITMAPHEADER SourceBitmap,
#else // Raw source bitmap.
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  #if INC_SOURCE_DEPTH == 8
    LPLONG SourceColorTable,
    #if INC_COPYMODE == INC_ALPHA
      UNS16 NumberOfAlphaColors,
    #endif
  #elif INC_COPYMODE == INC_ALPHA
    INT16 nAlphaValue,
  #endif
#endif
  TYPE_RectPointer SourceRectangle, 

// Then the destination bitmap description.
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
#if INC_STRETCHMODE == INC_STRETCH
  TYPE_RectPointer DestBoundingRectangle)
#else
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY)
#endif
{

/*************************************************************************
 *
 *            Solid and Unity Bitmap Copy
 *
 *  DESCRIPTION:
 *     Copies a source bitmap to a destination bitmap. No stretching
 *     is attempted.
 *
 *     The copy mode includes:
 *         from 8  bit to 16 bit colour depth
 *         from 8  bit to 24 bit colour depth
 *         from 8  bit to 32 bit colour depth
 *         from 16 bit to 16 bit colour depth
 *         from 24 bit to 24 bit colour depth
 *         from 32 bit to 32 bit colour depth
 *
 *  RETURN:
 *     TRUE for success, FALSE for failure.
 *
 *  10:28:00  Tuesday 14 July 1998
 *
 * ---------------------------------------------------------------------
 *
 *    Solid with Colour Key (holes for 8 bit) and Unity Bitmap Copy
 *
 *  DESCRIPTION:
 *     Copies a source bitmap to a destination bitmap. No stretching
 *     is attempted. Colour key effect is applied.
 *
 *     The colour key copy mode includes:
 *         from 8  bit to 16 bit colour depth
 *         from 8  bit to 24 bit colour depth
 *         from 8  bit to 32 bit colour depth
 *         from 16 bit to 16 bit colour depth
 *         from 24 bit to 24 bit colour depth
 *         from 32 bit to 32 bit colour depth
 *
 *     The hole copy mode includes:
 *         from 8  bit to 16 bit colour depth
 *         from 8  bit to 24 bit colour depth
 *         from 8  bit to 32 bit colour depth
 *
 *  RETURN:
 *     TRUE for success, FALSE for failure.
 *
 *  14:24:55  Thursday 13 August 1998
 *  11:28:10  Friday   22 January 1999 (add in Unity bitmap copy with holes)
 *
 *************************************************************************/
#if (INC_STRETCHMODE == INC_UNITY && INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0) || \
    (INC_STRETCHMODE == INC_UNITY && INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1) || \
    (INC_STRETCHMODE == INC_UNITY && INC_COPYMODE == INC_HOLES && INC_COLOURKEY == 0)
  #if INC_SOURCEFORMAT == INC_RAWBITMAP

    // Solid (or wiht holes) & Unity bitmap copy

    #if (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16) ||  \
        (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24) ||  \
        (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 32) ||  \
        (INC_SOURCE_DEPTH == 16 && INC_DEST_DEPTH == 16) ||  \
        (INC_SOURCE_DEPTH == 24 && INC_DEST_DEPTH == 24) ||  \
        (INC_SOURCE_DEPTH == 32 && INC_DEST_DEPTH == 32)
      {
        LPBYTE  lpSourceScanlineBeginBlt;      // pointer to source scanline position to begin blt
        int     SourceBitmapWidthInBytes;      // source bitmap width in bytes
        LPBYTE  lpDestinationScanlineBeginBlt; // pointer to destination scanline position to begin blt
        int     DestinationBitmapWidthInBytes; // destination bitmap width in bytes

        #if (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0)  // no colour key defined
          #if (INC_SOURCE_DEPTH == INC_DEST_DEPTH)  // same source/destination colour depth
            int     BytesAcrossToBlt;               // blt width in bytes
          #else  // different source/destination colour depths
            int             PixelsAcrossToBlt;        // blt width in pixels
            register int    PixelCounter;             // pixel counter

            register DWORD  dwSourceData;             // holder for source data

            #if INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16
              register DWORD   dwDestinationData1;                // holders for destination data
            #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24
              register DWORD   dwDestinationData1;                // holders for destination data
              register DWORD   dwDestinationData2;
              register DWORD   dwDestinationData3;
            #endif
            register LPBYTE  lpSourceBltPointer;       // pointer to current blt source
            register LPBYTE  lpDestinationBltPointer;  // pointer to current blt destination
          #endif
        #elif (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1)  // colour key is defined
          int             PixelsAcrossToBlt;        // blt width in pixels
          register int    PixelCounter;             // pixel counter
          register DWORD  redLimit;
          register DWORD  greenLimit;
          register DWORD  blueLimit;
          register DWORD  redMask = LE_BLT_dwRedMask;
          register DWORD  greenMask = LE_BLT_dwGreenMask;
          register DWORD  blueMask = LE_BLT_dwBlueMask;
          register DWORD  rawPixel;
          register LPBYTE  lpSourceBltPointer;       // pointer to current blt source
          register LPBYTE  lpDestinationBltPointer;  // pointer to current blt destination
        #elif (INC_COPYMODE == INC_HOLES)  // holes copy mode
          register DWORD   rawPixel;
          register LPBYTE  lpSourceBltPointer;       // pointer to current blt source
          register LPBYTE  lpDestinationBltPointer;  // pointer to current blt destination
          register int     PixelsAcrossToBlt;
          register int     PixelCounter;             // pixel counter
        #endif

        register int  LinesToBlt;      // blt height in pixels

        RECT  SourceRectangleToUpdate; // source rectangle to be updated
        int   SourceOffsetX;           // offset in pixels used for image clipping
        int   SourceOffsetY;           // used for image clipping
        int   CopyWidth;               // copy width after clipping
        int   CopyHeight;              // copy height after clipping
        int   DestinationOffsetX;      // used for image clipping
        int   DestinationOffsetY;      // used for image clipping

        // validate the source/destination bitmap pointers

        if( SourceBitmapBits == NULL )
        {
        #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, NAME_AS_STRING ": SourceBitmapBits == NULL\n\n");
          LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
        #endif
          return FALSE;  // invalid source bitmap pointer
        }

        if( DestinationBitmapBits == NULL )
        {
        #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, NAME_AS_STRING ": DestinationBitmapBits == NULL\n\n");
          LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
        #endif
          return FALSE;  // invalid destination bitmap pointer
        }

        // find the actual source area (inside source rectangle) to be updated
        // using destination bounding rectangle and destination invalid rectangle

        if( DestInvalidRectangle != NULL )
        {
          SourceRectangleToUpdate.left = SourceRectangle->left +
            (DestInvalidRectangle->left - DestBoundingRectangle->left);
          SourceRectangleToUpdate.right = SourceRectangle->left +
            (DestInvalidRectangle->right - DestBoundingRectangle->left);
          SourceRectangleToUpdate.top = SourceRectangle->top +
            (DestInvalidRectangle->top - DestBoundingRectangle->top);
          SourceRectangleToUpdate.bottom = SourceRectangle->top +
            (DestInvalidRectangle->bottom - DestBoundingRectangle->top);

          // initialise clipping parameters
          SourceOffsetX = (int) SourceRectangleToUpdate.left;
          SourceOffsetY = (int) SourceRectangleToUpdate.top;
          CopyWidth  = (int) (SourceRectangleToUpdate.right - SourceRectangleToUpdate.left);
          CopyHeight = (int) (SourceRectangleToUpdate.bottom - SourceRectangleToUpdate.top);
        }
        else // don't know the DestInvalidRectangle
        {
          // initialise clipping parameters
          SourceOffsetX = (int) SourceRectangle->left;
          SourceOffsetY = (int) SourceRectangle->top;
          CopyWidth  = (int) (SourceRectangle->right - SourceRectangle->left);
          CopyHeight = (int) (SourceRectangle->bottom - SourceRectangle->top);
        }

        DestinationOffsetX = DestinationX;
        DestinationOffsetY = DestinationY;

        // get both source and destination bitmaps clipped
        LI_BLT_GetBitmapClipped(&SourceOffsetX, &SourceOffsetY,
          &CopyWidth, &CopyHeight, SourceBitmapWidth, SourceBitmapHeight,
          &DestinationOffsetX, &DestinationOffsetY,
          DestinationBitmapWidth, DestinationBitmapHeight);

        // calculate bitmap widths in bytes
        // force all pixels aligned in DWORD boundary

        #if INC_SOURCE_DEPTH == 8
          SourceBitmapWidthInBytes = (SourceBitmapWidth + 3) & 0xfffffffc;
        #elif INC_SOURCE_DEPTH == 16
          SourceBitmapWidthInBytes = (SourceBitmapWidth * 2 + 3) & 0xfffffffc;
        #elif INC_SOURCE_DEPTH == 24
          SourceBitmapWidthInBytes = (SourceBitmapWidth * 3 + 3) & 0xfffffffc;
        #elif INC_SOURCE_DEPTH == 32
          SourceBitmapWidthInBytes = SourceBitmapWidth * 4;
        #endif

        #if INC_DEST_DEPTH == 16
          DestinationBitmapWidthInBytes = (DestinationBitmapWidth * 2 + 3) & 0xfffffffc;
        #elif INC_DEST_DEPTH == 24
          DestinationBitmapWidthInBytes = (DestinationBitmapWidth * 3 + 3) & 0xfffffffc;
        #elif INC_DEST_DEPTH == 32
          DestinationBitmapWidthInBytes = DestinationBitmapWidth * 4;
        #endif
        
        // calculate start position of blt in source bitmap
        #if INC_SOURCE_DEPTH == 8
          lpSourceScanlineBeginBlt = SourceBitmapBits +
            SourceOffsetY * SourceBitmapWidthInBytes;
          lpSourceScanlineBeginBlt += SourceOffsetX;
        #elif INC_SOURCE_DEPTH == 16
          lpSourceScanlineBeginBlt = SourceBitmapBits +
            SourceOffsetY * SourceBitmapWidthInBytes;
          lpSourceScanlineBeginBlt += SourceOffsetX * 2;
        #elif INC_SOURCE_DEPTH == 24
          lpSourceScanlineBeginBlt = SourceBitmapBits +
            SourceOffsetY * SourceBitmapWidthInBytes;
          lpSourceScanlineBeginBlt += SourceOffsetX * 3;
        #elif INC_SOURCE_DEPTH == 32
          lpSourceScanlineBeginBlt = SourceBitmapBits +
            SourceOffsetY * SourceBitmapWidthInBytes;
          lpSourceScanlineBeginBlt += SourceOffsetX * 4;
        #endif

        // calculate start position of blt in destination bitmap
        #if INC_DEST_DEPTH == 16
          lpDestinationScanlineBeginBlt = DestinationBitmapBits +
            DestinationOffsetY * DestinationBitmapWidthInBytes;
          lpDestinationScanlineBeginBlt += DestinationOffsetX * 2;
        #elif INC_DEST_DEPTH == 24
          lpDestinationScanlineBeginBlt = DestinationBitmapBits +
            DestinationOffsetY * DestinationBitmapWidthInBytes;
          lpDestinationScanlineBeginBlt += DestinationOffsetX * 3;
        #elif INC_DEST_DEPTH == 32
          lpDestinationScanlineBeginBlt = DestinationBitmapBits +
            DestinationOffsetY * DestinationBitmapWidthInBytes;
          lpDestinationScanlineBeginBlt += DestinationOffsetX * 4;
        #endif

        // get the image width and height after clipping
        LinesToBlt = CopyHeight;

        #if (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0)  // no colour key defined
          #if (INC_SOURCE_DEPTH == INC_DEST_DEPTH)  // the same source/destination colour depth
            #if INC_SOURCE_DEPTH == 16
              BytesAcrossToBlt = CopyWidth * 2;
            #elif INC_SOURCE_DEPTH == 24
              BytesAcrossToBlt = CopyWidth * 3;
            #elif INC_SOURCE_DEPTH == 32
              BytesAcrossToBlt = CopyWidth * 4;
            #endif
          #else   // different source/destination colour depths
            PixelsAcrossToBlt = CopyWidth;
          #endif
        #elif (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1)  // colour key is defined
          PixelsAcrossToBlt = CopyWidth;
        #elif (INC_COPYMODE == INC_HOLES)  // holes copy mode
          PixelsAcrossToBlt = CopyWidth;
        #endif

        // If colour key is defined, work out colour limits
        #if INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1
          // find the colour limits corresponding to the current colour depth.
          #if INC_DEST_DEPTH == 16
            redLimit = LI_BLT_ConvertColorRefTo16BitColor (
              LE_GRAFIX_MakeColorRef (LE_BLT_KeyRedLimit, 0, 0));
            greenLimit = LI_BLT_ConvertColorRefTo16BitColor (
              LE_GRAFIX_MakeColorRef (0, LE_BLT_KeyGreenLimit, 0));
            blueLimit = LI_BLT_ConvertColorRefTo16BitColor (
              LE_GRAFIX_MakeColorRef (0, 0, LE_BLT_KeyBlueLimit));
          #elif INC_DEST_DEPTH == 24
            redLimit = LI_BLT_ConvertColorRefTo24BitColor (
              LE_GRAFIX_MakeColorRef (LE_BLT_KeyRedLimit, 0, 0));
            greenLimit = LI_BLT_ConvertColorRefTo24BitColor (
              LE_GRAFIX_MakeColorRef (0, LE_BLT_KeyGreenLimit, 0));
            blueLimit = LI_BLT_ConvertColorRefTo24BitColor (
              LE_GRAFIX_MakeColorRef (0, 0, LE_BLT_KeyBlueLimit));
          #elif INC_DEST_DEPTH == 32
            redLimit = LI_BLT_ConvertColorRefTo32BitColor (
              LE_GRAFIX_MakeColorRef (LE_BLT_KeyRedLimit, 0, 0));
            greenLimit = LI_BLT_ConvertColorRefTo32BitColor (
              LE_GRAFIX_MakeColorRef (0, LE_BLT_KeyGreenLimit, 0));
            blueLimit = LI_BLT_ConvertColorRefTo32BitColor (
              LE_GRAFIX_MakeColorRef (0, 0, LE_BLT_KeyBlueLimit));
          #endif
        #endif

        while (LinesToBlt > 0)
        {
        #if (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0)  // no colour key defined

          #if (INC_SOURCE_DEPTH == INC_DEST_DEPTH) // the same source/destination colour depth
              memcpy(lpDestinationScanlineBeginBlt, lpSourceScanlineBeginBlt, BytesAcrossToBlt);
          #else  // different source/destination colour depths
            // initialise current blt pointers
            lpDestinationBltPointer = lpDestinationScanlineBeginBlt;
            lpSourceBltPointer = lpSourceScanlineBeginBlt;
          
            // initialise pixel counter
            PixelCounter = PixelsAcrossToBlt;
            while (PixelCounter > 0)
            {
              // process four pixels at a time if possible
              if (PixelCounter >= 4)
              {
                // get four pixels of source data
                #if INC_SOURCE_DEPTH == 8
                  dwSourceData = *((LPLONG)lpSourceBltPointer);
                  lpSourceBltPointer += 4;
                #endif
              
                #if INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16
                  // convert 8 bit indices to 16 bit color values
                  dwDestinationData1 = (WORD)SourceColorTable[(dwSourceData & 0xff) * 2];
                  dwSourceData >>= 8;
                  dwDestinationData1 |= SourceColorTable[(dwSourceData & 0xff) * 2] << 16;
                  dwSourceData >>= 8;

                  // copy the converted data to destination bitmap
                  *((LPLONG)lpDestinationBltPointer) = dwDestinationData1;
                  lpDestinationBltPointer += 4;
                  dwDestinationData1 = (WORD)SourceColorTable[(dwSourceData & 0xff) * 2];
                  dwSourceData >>= 8;
                  dwDestinationData1 |= SourceColorTable[(dwSourceData & 0xff) * 2] << 16;
                  *((LPLONG)lpDestinationBltPointer) = dwDestinationData1;
                  lpDestinationBltPointer += 4;

                #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24
                  // convert 8 bit indices to 24 bit color values
                  dwDestinationData1 = SourceColorTable[(dwSourceData & 0xff) * 2];
                  dwSourceData >>= 8;
                  dwDestinationData1 |= SourceColorTable[(dwSourceData & 0xff) * 2] << 24;
                  dwDestinationData2 = SourceColorTable[(dwSourceData & 0xff) * 2] >> 8;
                  dwSourceData >>= 8;
                  dwDestinationData2 |= SourceColorTable[(dwSourceData & 0xff) * 2] << 16;
                  dwDestinationData3 = SourceColorTable[(dwSourceData & 0xff) * 2] >> 16;
                  dwSourceData >>= 8;
                  dwDestinationData3 |= SourceColorTable[(dwSourceData & 0xff) * 2] << 8;

                  // copy the converted data to destination bitmap
                  *((LPLONG)lpDestinationBltPointer) = dwDestinationData1;
                  lpDestinationBltPointer += 4;
                  *((LPLONG)lpDestinationBltPointer) = dwDestinationData2;
                  lpDestinationBltPointer += 4;
                  *((LPLONG)lpDestinationBltPointer) = dwDestinationData3;
                  lpDestinationBltPointer += 4;

                #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 32
                  // convert 8 bit indices to 32 bit color values
                  // and copy the converted data to destination bitmap
                  *((LPLONG)lpDestinationBltPointer) = SourceColorTable[(dwSourceData & 0xff) * 2];
                  lpDestinationBltPointer += 4;
                  dwSourceData >>= 8;
                  *((LPLONG)lpDestinationBltPointer) = SourceColorTable[(dwSourceData & 0xff) * 2];
                  lpDestinationBltPointer += 4;
                  dwSourceData >>= 8;
                  *((LPLONG)lpDestinationBltPointer) = SourceColorTable[(dwSourceData & 0xff) * 2];
                  lpDestinationBltPointer += 4;
                  dwSourceData >>= 8;
                  *((LPLONG)lpDestinationBltPointer) = SourceColorTable[(dwSourceData & 0xff) * 2];
                  lpDestinationBltPointer += 4;
                #endif

                  // decrement the PixelCounter by 4
                  PixelCounter -= 4;
              }
              // process two pixels at a time if possible
              else if (PixelCounter >= 2)
              {
                // get two pixels of source data
                #if INC_SOURCE_DEPTH == 8
                  dwSourceData = *((LPWORD)lpSourceBltPointer);
                  lpSourceBltPointer += 2;
                #endif
              
                #if INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16
                  // convert 8 bit indices to 16 bit color values
                  // and copy the converted data to destination bitmap
                  dwDestinationData1 = (WORD)SourceColorTable[(dwSourceData & 0xff) * 2];
                  dwSourceData >>= 8;
                  dwDestinationData1 |= SourceColorTable[(dwSourceData & 0xff) * 2] << 16;
                  *((LPLONG)lpDestinationBltPointer) = dwDestinationData1;
                  lpDestinationBltPointer += 4;

                #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24
                  // convert 8 bit indices to 24 bit color values
                  dwDestinationData1 = SourceColorTable[(dwSourceData & 0xff) * 2];
                  dwSourceData >>= 8;
                  dwDestinationData1 |= SourceColorTable[(dwSourceData & 0xff) * 2] << 24;
                  dwDestinationData2 = SourceColorTable[(dwSourceData & 0xff) * 2] >> 8;

                  // copy the converted data to destination bitmap
                  *((LPLONG)lpDestinationBltPointer) = dwDestinationData1;
                  lpDestinationBltPointer += 4;
                  *((LPWORD)lpDestinationBltPointer) = (WORD)dwDestinationData2;
                  lpDestinationBltPointer += 2;

                #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 32
                  // convert 8 bit indices to 32 bit color values
                  // and copy the converted data to destination bitmap
                  *((LPLONG)lpDestinationBltPointer) = SourceColorTable[(dwSourceData & 0xff) * 2];
                  lpDestinationBltPointer += 4;
                  dwSourceData >>= 8;
                  *((LPLONG)lpDestinationBltPointer) = SourceColorTable[(dwSourceData & 0xff) * 2];
                  lpDestinationBltPointer += 4;
                #endif

                  // decrement the pixel counter by 2
                  PixelCounter -= 2;
              }
              // process one pixel
              else
              {
                // get one pixel of source data
                #if INC_SOURCE_DEPTH == 8
                  dwSourceData = *((LPBYTE)lpSourceBltPointer);
                  lpSourceBltPointer += 1;
                #endif
              
                #if INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16
                  // convert 8 bit indices to 16 bit color values
                  // and copy the converted data to destination bitmap
                  dwDestinationData1 = SourceColorTable[(dwSourceData & 0xff) * 2];
                  *((LPWORD)lpDestinationBltPointer) = (WORD)dwDestinationData1;
                  lpDestinationBltPointer += 2;

                #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24
                  // convert 8 bit indices to 24 bit color values
                  dwDestinationData1 = SourceColorTable[(dwSourceData & 0xff) * 2];

                  // copy the converted data to destination bitmap
                  *((LPWORD)lpDestinationBltPointer) = (WORD)(dwDestinationData1);
                  lpDestinationBltPointer += 2;
                  *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dwDestinationData1 >> 16);
                  lpDestinationBltPointer += 1;

                #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 32
                  // convert 8 bit indices to 32 bit color values
                  // and copy the converted data to destination bitmap
                  *((LPLONG)lpDestinationBltPointer) = SourceColorTable[(dwSourceData & 0xff) * 2];
                  lpDestinationBltPointer += 4;
                #endif

                  // decrement the pixel counter by 1
                  PixelCounter -= 1;
              }
            }
          #endif

        #elif (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1)  // colour key is defined

          // initialise current blt pointers
          lpDestinationBltPointer = lpDestinationScanlineBeginBlt;
          lpSourceBltPointer = lpSourceScanlineBeginBlt;
        
          // initialise pixel counter
          PixelCounter = PixelsAcrossToBlt;
          while (PixelCounter > 0)
          {
          #if (INC_SOURCE_DEPTH == INC_DEST_DEPTH)  // the same source/destination colour depth

            // for 16 bit colour depth
            #if INC_SOURCE_DEPTH == 16
              // get one pixel of source data
              rawPixel = *((LPWORD)lpSourceBltPointer);
              lpSourceBltPointer += 2;

              // see if the pixel needs to be copied
              #if CE_ARTLIB_BlitUseGreenColourKey
                if( (rawPixel & redMask) >= redLimit  ||
                  (rawPixel & greenMask) < greenLimit ||
                  (rawPixel & blueMask) >= blueLimit )
              #elif CE_ARTLIB_BlitUseBlueColourKey
                if( (rawPixel & redMask) >= redLimit   ||
                  (rawPixel & greenMask) >= greenLimit ||
                  (rawPixel & blueMask) < blueLimit )
              #else // Just use absolute black as transparent.
                if( (rawPixel & 0x00FFFFFF) != 0 )
              #endif
              {
                // copy this pixel to destination bitmap
                *((LPWORD)lpDestinationBltPointer) = (WORD) rawPixel;
              }

              // update DestinationBitsPntr
              lpDestinationBltPointer += 2;

            // for 24 bit colour depth
            #elif INC_SOURCE_DEPTH == 24
              // get one pixel of source data
//              rawPixel = *((LPLONG)lpSourceBltPointer);
//              lpSourceBltPointer += 3;
              rawPixel = *((LPWORD)lpSourceBltPointer);
              lpSourceBltPointer += 2;
              rawPixel |= ((DWORD) *((LPBYTE)lpSourceBltPointer) << 16);
              lpSourceBltPointer ++;

              // see if the pixel needs to be copied
              #if CE_ARTLIB_BlitUseGreenColourKey
                if( (rawPixel & redMask) >= redLimit  ||
                  (rawPixel & greenMask) < greenLimit ||
                  (rawPixel & blueMask) >= blueLimit )
              #elif CE_ARTLIB_BlitUseBlueColourKey
                if( (rawPixel & redMask) >= redLimit   ||
                  (rawPixel & greenMask) >= greenLimit ||
                  (rawPixel & blueMask) < blueLimit )
              #else // Just use absolute black as transparent.
                if( (rawPixel & 0x00FFFFFF) != 0 )
              #endif
              {
                // copy this pixel to destination bitmap
                *((LPWORD)lpDestinationBltPointer) = (WORD) rawPixel;
                lpDestinationBltPointer += 2;
                *((LPBYTE)lpDestinationBltPointer) = (BYTE) (((DWORD) rawPixel) >> 16);
                lpDestinationBltPointer ++;
              }
              else
              {
                // if the pixel is not copied (transparent),
                // advance DestinationBitsPntr by 3 bytes
                lpDestinationBltPointer += 3;
              }

            // for 32 bit colour depth
            #elif INC_SOURCE_DEPTH == 32
              // get one pixel of source data
              rawPixel = *((LPLONG)lpSourceBltPointer);
              lpSourceBltPointer += 4;

              // see if the pixel needs to be copied
              #if CE_ARTLIB_BlitUseGreenColourKey
                if( (rawPixel & redMask) >= redLimit  ||
                  (rawPixel & greenMask) < greenLimit ||
                  (rawPixel & blueMask) >= blueLimit )
              #elif CE_ARTLIB_BlitUseBlueColourKey
                if( (rawPixel & redMask) >= redLimit   ||
                  (rawPixel & greenMask) >= greenLimit ||
                  (rawPixel & blueMask) < blueLimit )
              #else // Just use absolute black as transparent.
                if( (rawPixel & 0x00FFFFFF) != 0 )
              #endif
              {
                // copy this pixel to destination bitmap
                *((LPLONG)lpDestinationBltPointer) = (DWORD) rawPixel;
              }

              // update DestinationBitsPntr
              lpDestinationBltPointer += 4;
            #endif  // end of same colour depth case

          #else  // different source/destination colour depths
                // lzou - Here does one pixel case only. Two and four pixel cases
                //      - will be implemented later on.

            // get one pixel of source data
            #if INC_SOURCE_DEPTH == 8
              rawPixel = *((LPBYTE)lpSourceBltPointer);
              lpSourceBltPointer += 1;
            #endif

            // from 8 bit to 16 bit colour depth
            #if INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16

              // convert 8 bit indices to 16 bit color values
              // and copy the converted data to destination bitmap

              rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];

              // see if the pixel needs to be copied
              #if CE_ARTLIB_BlitUseGreenColourKey
                if( (rawPixel & redMask) >= redLimit  ||
                  (rawPixel & greenMask) < greenLimit ||
                  (rawPixel & blueMask) >= blueLimit )
              #elif CE_ARTLIB_BlitUseBlueColourKey
                if( (rawPixel & redMask) >= redLimit   ||
                  (rawPixel & greenMask) >= greenLimit ||
                  (rawPixel & blueMask) < blueLimit )
              #else // Just use absolute black as transparent.
                if( (rawPixel & 0x00FFFFFF) != 0 )
              #endif
              {
                *((LPWORD)lpDestinationBltPointer) = (WORD) rawPixel;
              }

              // update DestinationBitsPntr
              lpDestinationBltPointer += 2;  // advanced 2 bytes

            // from 8 bit to 24 bit colour depth
            #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24

              // convert 8 bit indices to 24 bit color values
              // and copy the converted data to destination bitmap

              rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];

              // see if the pixel needs to be copied
              #if CE_ARTLIB_BlitUseGreenColourKey
                if( (rawPixel & redMask) >= redLimit  ||
                  (rawPixel & greenMask) < greenLimit ||
                  (rawPixel & blueMask) >= blueLimit )
              #elif CE_ARTLIB_BlitUseBlueColourKey
                if( (rawPixel & redMask) >= redLimit   ||
                  (rawPixel & greenMask) >= greenLimit ||
                  (rawPixel & blueMask) < blueLimit )
              #else // Just use absolute black as transparent.
                if( (rawPixel & 0x00FFFFFF) != 0 )
              #endif
              {
                *((LPWORD)lpDestinationBltPointer) = (WORD) rawPixel;
                lpDestinationBltPointer += 2;
                *((LPBYTE)lpDestinationBltPointer) = (BYTE) (((DWORD) rawPixel) >> 16);
                lpDestinationBltPointer ++;
              }
              else
              {
                // if the pixel is not copied,
                // advance DestinationBitsPntr 3 bytes
                lpDestinationBltPointer += 3;
              }

            // from 8 bit to 32 bit colour depth
            #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 32

              // convert 8 bit indices to 32 bit color values
              // and copy the converted data to destination bitmap

              rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];

              // see if the pixel needs to be copied
              #if CE_ARTLIB_BlitUseGreenColourKey
                if( (rawPixel & redMask) >= redLimit  ||
                  (rawPixel & greenMask) < greenLimit ||
                  (rawPixel & blueMask) >= blueLimit )
              #elif CE_ARTLIB_BlitUseBlueColourKey
                if( (rawPixel & redMask) >= redLimit   ||
                  (rawPixel & greenMask) >= greenLimit ||
                  (rawPixel & blueMask) < blueLimit )
              #else // Just use absolute black as transparent.
                if( (rawPixel & 0x00FFFFFF) != 0 )
              #endif
              {
                *((LPLONG)lpDestinationBltPointer) = (DWORD) rawPixel;
              }

              // update DestinationBitsPntr
              lpDestinationBltPointer += 4;

            #endif  // end of different colour depth case
          #endif  // end of all colour depth cases

            // decrement the pixel counter by 1
            PixelCounter -= 1;
          }
        #elif (INC_COPYMODE == INC_HOLES)  // holes copy mode

          // Here, We deal with bitmap copy (with holes)
          //    from 8 to 16 bit colour depth
          //    from 8 to 24 bit colour depth
          //    from 8 to 32 bit colour depth

          #if (INC_SOURCE_DEPTH != INC_DEST_DEPTH)
            // initialise current blt pointers
            lpDestinationBltPointer = lpDestinationScanlineBeginBlt;
            lpSourceBltPointer = lpSourceScanlineBeginBlt;
          
            // initialise pixel counter
            PixelCounter = PixelsAcrossToBlt;
            while (PixelCounter > 0)
            {
              // get four pixels of source data
              #if INC_SOURCE_DEPTH == 8
                rawPixel= *((LPBYTE)lpSourceBltPointer);
                lpSourceBltPointer += 1;
              #endif

              // from 8 to 16 bit colour depth
              #if INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16
                // check to see if rawPixel is nonzero.
                // If it is zero (transparent or hole), skip over it.
                // If it is nonzero (solid), copy it to destination bitmap
                if( rawPixel != 0 )
                {
                  rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];
                  *((LPWORD)lpDestinationBltPointer) = (WORD) rawPixel;
                }
                
                // update the destination blt pointer
                lpDestinationBltPointer += 2;

              #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24
                // check to see if rawPixel is nonzero.
                // If it is zero (transparent or hole), skip over it.
                // If it is nonzero (solid), copy it to destination bitmap
                if( rawPixel != 0 )
                {
                  rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];
                  *((LPWORD)lpDestinationBltPointer) = (WORD) rawPixel;
                  lpDestinationBltPointer += 2;
                  rawPixel >>= 16;
                  *((LPBYTE)lpDestinationBltPointer) = (BYTE) rawPixel;
                  lpDestinationBltPointer += 1;
                }
                else
                {
                  // skip one pixel (transparent pixel)
                  lpDestinationBltPointer += 3;
                }
                
              #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 32
                // check to see if rawPixel is nonzero.
                // If it is zero (transparent or hole), skip over it.
                // If it is nonzero (solid), copy it to destination bitmap
                if( rawPixel != 0 )
                {
                  rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];
                  *((LPLONG)lpDestinationBltPointer) = (DWORD) rawPixel;
                }
                
                // update the destination blt pointer
                lpDestinationBltPointer += 4;
              #endif

                // decrement the PixelCounter by 4
                PixelCounter --;
            }
          // lzou
          #endif
        #endif // end of bitmap copy with holes

          // update pointers to the next scanline
          lpDestinationScanlineBeginBlt += DestinationBitmapWidthInBytes;
          lpSourceScanlineBeginBlt += SourceBitmapWidthInBytes;
          LinesToBlt --;
        }

        // successful
        return TRUE;   // from 8 bit to 16, 24, and 32 bit colour depths
      }
    #endif // (Solid & Unity) || (Solid with Colour Key/Holes & Unity)
  #endif // raw source/destination bitmap
#endif // unity and solid

/*************************************************************************
 *
 *            Alpha and Unity Bitmap Copy
 *
 *  DESCRIPTION:
 *     Copies a source bitmap to a destination bitmap with alpha channel
 *     effect. No stretching is attempted.
 *
 *     The copy mode includes:
 *         from  8 bit to 16 bit colour depth
 *         from  8 bit to 24 bit colour depth
 *         from  8 bit to 32 bit colour depth
 *         from 16 bit to 16 bit colour depth
 *         from 24 bit to 24 bit colour depth
 *         from 32 bit to 32 bit colour depth
 *
 *  RETURN:
 *     TRUE for success, FALSE for failure.
 *
 *  16:49:25  Tuesday 21 July 1998
 *
 *************************************************************************/
#if INC_STRETCHMODE == INC_UNITY && INC_COPYMODE == INC_ALPHA
  #if INC_SOURCEFORMAT == INC_RAWBITMAP

    // (Alpha and Unity) bitmap copy
    // do alpha & unity bitmap copy from 8 bit color depth
    // to 16, 24, and 32 bit colour depths, raw source bitmap

    #if (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16) ||  \
        (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24) ||  \
        (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 32) ||  \
        (INC_SOURCE_DEPTH == 16 && INC_DEST_DEPTH == 16) ||  \
        (INC_SOURCE_DEPTH == 24 && INC_DEST_DEPTH == 24) ||  \
        (INC_SOURCE_DEPTH == 32 && INC_DEST_DEPTH == 32)
    {
      LPBYTE  lpSourceScanlineBeginBlt;      // pointer to source scanline position to begin blt
      int     SourceBitmapWidthInBytes;      // source bitmap width in bytes
      LPBYTE  lpDestinationScanlineBeginBlt; // pointer to destination scanline position to begin blt
      int     DestinationBitmapWidthInBytes; // destination bitmap width in bytes
      int     PixelsAcrossToBlt;             // blt width in pixels

      register int     LinesToBlt;               // blt height in pixels
      register int     PixelCounter;             // pixel counter
      register LPBYTE  lpSourceBltPointer;       // pointer to current blt source
      register DWORD   dwSourceData;             // holder for source data
      register LPBYTE  lpDestinationBltPointer;  // pointer to current blt destination
      register DWORD   dwDestinationData;        // holders for destination data

      register DWORD dwAlphaData;   // holder source and data blended
      register DWORD dwAlpha;       // 32 bit alpha channel

      RECT  SourceRectangleToUpdate; // source rectangle to be updated
      int   SourceOffsetX;           // offset in pixels used for image clipping
      int   SourceOffsetY;           // used for image clipping
      int   CopyWidth;               // copy width after clipping
      int   CopyHeight;              // copy height after clipping
      int   DestinationOffsetX;      // used for image clipping
      int   DestinationOffsetY;      // used for image clipping
      #if INC_SOURCE_DEPTH != 8
        WORD  wPureGreen;            // to filter out pure green colour
      #endif

      // validate the source/destination bitmap pointers

      if( SourceBitmapBits == NULL )
      {
      #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, NAME_AS_STRING ": SourceBitmapBits == NULL\n\n");
        LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      #endif
        return FALSE;  // invalid source bitmap pointer
      }

      if( DestinationBitmapBits == NULL )
      {
      #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, NAME_AS_STRING ": DestinationBitmapBits == NULL\n\n");
        LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      #endif
        return FALSE;  // invalid destination bitmap pointer
      }

      // find out the actual source rectangle to be updated using
      // destination bounding rectangle and destination invalid rectangle

      if( DestInvalidRectangle != NULL )
      {
        SourceRectangleToUpdate.left = SourceRectangle->left +
          (DestInvalidRectangle->left - DestBoundingRectangle->left);
        SourceRectangleToUpdate.right = SourceRectangle->left +
          (DestInvalidRectangle->right - DestBoundingRectangle->left);
        SourceRectangleToUpdate.top = SourceRectangle->top +
          (DestInvalidRectangle->top - DestBoundingRectangle->top);
        SourceRectangleToUpdate.bottom = SourceRectangle->top +
          (DestInvalidRectangle->bottom - DestBoundingRectangle->top);

        // initialise clipping parameters
        SourceOffsetX = (int) SourceRectangleToUpdate.left;
        SourceOffsetY = (int) SourceRectangleToUpdate.top;
        CopyWidth  = (int) (SourceRectangleToUpdate.right - SourceRectangleToUpdate.left);
        CopyHeight = (int) (SourceRectangleToUpdate.bottom - SourceRectangleToUpdate.top);
      }
      else // don't know the DestInvalidRectangle
      {
        // initialise clipping parameters
        SourceOffsetX = (int) SourceRectangle->left;
        SourceOffsetY = (int) SourceRectangle->top;
        CopyWidth  = (int) (SourceRectangle->right - SourceRectangle->left);
        CopyHeight = (int) (SourceRectangle->bottom - SourceRectangle->top);
      }

      DestinationOffsetX = DestinationX;
      DestinationOffsetY = DestinationY;
      
      // get both source and destination bitmaps clipped
      LI_BLT_GetBitmapClipped(&SourceOffsetX, &SourceOffsetY,
        &CopyWidth, &CopyHeight, SourceBitmapWidth, SourceBitmapHeight,
        &DestinationOffsetX, &DestinationOffsetY,
        DestinationBitmapWidth, DestinationBitmapHeight);
      
      // calculate bitmap widths in bytes
      // force all pixels aligned by 4 bytes boundary

      #if INC_SOURCE_DEPTH == 8
        SourceBitmapWidthInBytes = (SourceBitmapWidth + 3) & 0xfffffffc;
      #elif INC_SOURCE_DEPTH == 16
        SourceBitmapWidthInBytes = (SourceBitmapWidth * 2 + 3) & 0xfffffffc;
      #elif INC_SOURCE_DEPTH == 24
        SourceBitmapWidthInBytes = (SourceBitmapWidth * 3 + 3) & 0xfffffffc;
      #elif INC_SOURCE_DEPTH == 32
        SourceBitmapWidthInBytes = SourceBitmapWidth * 4;
      #endif

      #if INC_DEST_DEPTH == 16
        DestinationBitmapWidthInBytes = (DestinationBitmapWidth * 2 + 3) & 0xfffffffc;
      #elif INC_DEST_DEPTH == 24
        DestinationBitmapWidthInBytes = (DestinationBitmapWidth * 3 + 3) & 0xfffffffc;
      #elif INC_DEST_DEPTH == 32
        DestinationBitmapWidthInBytes = DestinationBitmapWidth * 4;
      #endif
      
      // calculate start position of blt in source bitmap
      lpSourceScanlineBeginBlt = SourceBitmapBits + SourceOffsetY *
        SourceBitmapWidthInBytes;
      #if INC_SOURCE_DEPTH == 8
        lpSourceScanlineBeginBlt += SourceOffsetX;
      #elif INC_SOURCE_DEPTH == 16
        lpSourceScanlineBeginBlt += SourceOffsetX * 2;
      #elif INC_SOURCE_DEPTH == 24
        lpSourceScanlineBeginBlt += SourceOffsetX * 3;
      #elif INC_SOURCE_DEPTH == 32
        lpSourceScanlineBeginBlt += SourceOffsetX * 4;
      #endif

      // calculate start position of blt in destination bitmap
      lpDestinationScanlineBeginBlt = DestinationBitmapBits +
        DestinationOffsetY * DestinationBitmapWidthInBytes;
      #if INC_DEST_DEPTH == 16
        lpDestinationScanlineBeginBlt += DestinationOffsetX * 2;
      #elif INC_DEST_DEPTH == 24
        lpDestinationScanlineBeginBlt += DestinationOffsetX * 3;
      #elif INC_DEST_DEPTH == 32
        lpDestinationScanlineBeginBlt += DestinationOffsetX * 4;
      #endif

      // If not 8 bit colour depth, we need to filter out
      // all pure green colour pixels.
      #if INC_SOURCE_DEPTH == 16
        // make a COLORREF for pure green colour
        wPureGreen = (WORD) LI_BLT_ConvertColorRefTo16BitColor(
          (COLORREF) RGB (0, 255, 0));
      #elif INC_SOURCE_DEPTH == 24
        // make a COLORREF for pure green colour
        wPureGreen = (WORD) LI_BLT_ConvertColorRefTo24BitColor(
          (COLORREF) RGB (0, 255, 0));
      #elif INC_SOURCE_DEPTH == 32
        // make a COLORREF for pure green colour
        wPureGreen = (WORD) LI_BLT_ConvertColorRefTo32BitColor(
          (COLORREF) RGB (0, 255, 0));
      #endif
        
      #if INC_SOURCE_DEPTH != 8
        // get the alpha data
        dwAlpha = (DWORD) nAlphaValue;

        #if CE_ARTLIB_BlitUse255AlphaLevels // use full 255 alpha levels
          // check the validity of dwAlpha
          if ( dwAlpha < 0 || dwAlpha > 255 )
          {
            if ( dwAlpha < 0 )
              dwAlpha = 0;
            else
              dwAlpha = 255;
          }
        #endif
      #endif

      // get the image width and height after clipping
      LinesToBlt = CopyHeight;
      PixelsAcrossToBlt = CopyWidth;
      
      while (LinesToBlt > 0)
      {
        // initialise current blt pointers
        lpDestinationBltPointer = lpDestinationScanlineBeginBlt;
        lpSourceBltPointer = lpSourceScanlineBeginBlt;
        
        // initialise pixel counter
        PixelCounter = PixelsAcrossToBlt;
        while (PixelCounter > 0)
        {
          // process one pixel each time

          // get one pixel of source data
          #if INC_SOURCE_DEPTH == 8
            dwSourceData = *((LPBYTE)lpSourceBltPointer);
            lpSourceBltPointer ++;
          #elif INC_SOURCE_DEPTH == 16
            dwDestinationData = *((LPWORD)lpSourceBltPointer);
            dwSourceData = dwDestinationData;
            lpSourceBltPointer += 2;
          #elif INC_SOURCE_DEPTH == 24
//            dwDestinationData = *((LPDWORD)lpSourceBltPointer);
//            dwSourceData = dwDestinationData &= 0x00ffffff;
//            lpSourceBltPointer += 3;
            dwDestinationData = *((LPWORD)lpSourceBltPointer);
            lpSourceBltPointer += 2;
            dwDestinationData |= (((DWORD) *((LPBYTE)lpSourceBltPointer)) << 16);
            lpSourceBltPointer ++;
            dwSourceData = dwDestinationData;
          #elif INC_SOURCE_DEPTH == 32
            dwSourceData = dwDestinationData = *((LPLONG)lpSourceBltPointer);
            lpSourceBltPointer += 4;
          #endif

        #if INC_SOURCE_DEPTH == 8
          if( dwSourceData != 0 )          // use hole technique
        #else
          if( dwSourceData != wPureGreen ) // use pure green colour key
        #endif
          {
          #if INC_SOURCE_DEPTH == 8  // 8 bit (colour depth) only
            if ( dwSourceData >= NumberOfAlphaColors )
            {
              #if INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 16
                *((LPWORD)lpDestinationBltPointer) = (WORD)SourceColorTable[(dwSourceData & 0xff) * 2];
                lpDestinationBltPointer += 2;

              #elif INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 24
                // convert 8 bit indices to 24 bit colour values
                dwDestinationData = SourceColorTable[(dwSourceData & 0xff) * 2];

                // copy the converted data to destination bitmap
                *((LPWORD)lpDestinationBltPointer) = (WORD)(dwDestinationData);
                lpDestinationBltPointer += 2;
                *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dwDestinationData >> 16);
                lpDestinationBltPointer += 1;

              #elif INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 32
                // convert 8 bit indices to 32 bit color values
                // and copy the converted data to destination bitmap
                *((LPLONG)lpDestinationBltPointer) = SourceColorTable[(dwSourceData & 0xff) * 2];
                lpDestinationBltPointer += 4;
              #endif
            }
            // apply alpha channel effect to both the source/destination bitmaps
            // ( see l_type.h for details )
            else
          #endif
            {
              // get the alpha channel data from the source bitmap
              #if INC_SOURCE_DEPTH == 8
                dwAlpha = SourceColorTable[(dwSourceData & 0xff) * 2 + 1];
                dwDestinationData = SourceColorTable[(dwSourceData & 0xff) * 2];
              #endif

              // get one pixel data from the destination bitmap
              #if INC_DEST_DEPTH == 16
                dwSourceData = *((LPWORD)lpDestinationBltPointer) & 0x0000ffff;
              #elif INC_DEST_DEPTH == 24
                dwSourceData = *((LPLONG)lpDestinationBltPointer) & 0x00ffffff;
              #elif INC_DEST_DEPTH == 32
                dwSourceData = *((LPLONG)lpDestinationBltPointer);
              #endif

              // do alpha channel effect here ...

            #if !CE_ARTLIB_BlitUse255AlphaLevels
              // for normal 16 alpha levels
              // Note that we deal with 8 bit colour depth differently from
              // other colour depths. For 8 bit colour depth, the palette
              // colours are pre-multiplied by the alpha value to save runtime
              // computation (so a 50% transparent red has a RGB value entry
              // of (127,0,0) rather than the 255,0,0 that you would expect).
              // But for other colour depths, we do not have the alpha value
              // premultiplied with the bitmap data. So, we have to apply
              // alpha channel effect to both source and destination bitmaps.
              switch( (int) dwAlpha )
              {
              #if INC_SOURCE_DEPTH == 8
                case ALPHA_OPAQUE06_25:   // ((1*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE12_50:  // ((2*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE18_75:  // ((3*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE25_00:  // ((4*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE31_25:  // ((5*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 2) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 2) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 2) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE37_50:  // ((6*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                    ((dwSourceData & LE_BLT_dwRedMask) >> 1) +
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 1) +
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE43_75:  // ((7*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                    ((dwSourceData & LE_BLT_dwRedMask) >> 1) +
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 1) +
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 1) +
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE50_00:  // ((8*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 1)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 1)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 1)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE56_25:  // ((9*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 1) - 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 1) - 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 1) - 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE62_50:  // ((10*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE68_75:  // ((11*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE75_00:  // ((12*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE81_25:  // ((13*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE87_50:  // ((14*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE93_75:  // ((15*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                default:
                  dwAlphaData = dwDestinationData;  // if no match, just get the source pixel data only
              // For other colour depths, we need to apply the alpha value
              // to both source and destination bitmaps.
              #else   // use only 16 alpha levels
                case ALPHA_OPAQUE06_25:   // ((1*255)/16)
                  dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 4) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4))
                    & LE_BLT_dwRedMask;
                  dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 4) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 4) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE12_50:  // ((2*255)/16)
                  dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 3) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                  dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 3) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 3) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE18_75:  // ((3*255)/16)
                  dwAlphaData =
                    (((dwDestinationData & LE_BLT_dwRedMask) >> 3) +
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 4) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwGreenMask) >> 3) +
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwBlueMask) >> 3) +
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE25_00:  // ((4*255)/16)
                  dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 2) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
                  dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE31_25:  // ((5*255)/16)
                  dwAlphaData =
                    (((dwDestinationData & LE_BLT_dwRedMask) >> 2) +
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 4) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 2) -
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) +
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 2) -
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) +
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 2) -
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE37_50:  // ((6*255)/16)
                  dwAlphaData =
                    (((dwDestinationData & LE_BLT_dwRedMask) >> 2) +
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 3) +
                    ((dwSourceData & LE_BLT_dwRedMask) >> 1) +
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) +
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) +
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 1) +
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + 
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE43_75:  // ((7*255)/16)
                  dwAlphaData =
                    (((dwDestinationData & LE_BLT_dwRedMask) >> 2) +
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 3) +
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 4) +
                    ((dwSourceData & LE_BLT_dwRedMask) >> 1) +
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) + 
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) +
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) +
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 1) +
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + 
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + 
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 1) +
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE50_00:  // ((8*255)/16)
                  dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 1)) & LE_BLT_dwRedMask;
                  dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 1)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 1)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE56_25:  // ((9*255)/16)
                  dwAlphaData =
                    (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + 
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 1) - 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + 
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 1) - 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + 
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 1) - 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE62_50:  // ((10*255)/16)
                  dwAlphaData =
                    (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + 
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + 
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + 
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE68_75:  // ((11*255)/16)
                  dwAlphaData =
                    (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + 
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 3) + 
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + 
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + 
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |=
                    (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + 
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + 
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE75_00:  // ((12*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) -
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) -
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) -
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE81_25:  // ((13*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) -
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 3) - 
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 4) +
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) -
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) -
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) -
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) -
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE87_50:  // ((14*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) -
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) -
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) -
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 3)+ 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                  break;

                case ALPHA_OPAQUE93_75:  // ((15*255)/16)
                  dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) -
                    ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) -
                    ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                  dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) -
                    ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + 
                    ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                  break;

                default:
                  // if no match, just get the source pixel data only
                  dwAlphaData = dwDestinationData;
              #endif
              }

            #else  // do full 255 alpha levels
              #if INC_SOURCE_DEPTH == 8
                // work out the final alpha effect -- dwAlphaData
#if 0  // This bit code sometimes causes wrong colour effects aound edges with anti-aliases on.
                dwAlphaData = 
                  ((dwDestinationData & LE_BLT_dwRedMask) +
                  (dwSourceData & LE_BLT_dwRedMask) -
                  (dwSourceData & LE_BLT_dwRedMask) * dwAlpha / 255)
                  & LE_BLT_dwRedMask;
                dwAlphaData |=
                  ((dwDestinationData & LE_BLT_dwGreenMask) +
                  (dwSourceData & LE_BLT_dwGreenMask) -
                  (dwSourceData & LE_BLT_dwGreenMask) * dwAlpha / 255)
                  & LE_BLT_dwGreenMask;
                dwAlphaData |=
                  ((dwDestinationData & LE_BLT_dwBlueMask) +
                  (dwSourceData & LE_BLT_dwBlueMask) -
                  (dwSourceData & LE_BLT_dwBlueMask) * dwAlpha / 255)
                  & LE_BLT_dwBlueMask;
#else
                dwAlphaData = 
                  ((dwDestinationData & LE_BLT_dwRedMask) +
                  (dwSourceData & LE_BLT_dwRedMask) * (255 - dwAlpha) / 255)
                  & LE_BLT_dwRedMask;
                dwAlphaData |=
                  ((dwDestinationData & LE_BLT_dwGreenMask) +
                  (dwSourceData & LE_BLT_dwGreenMask) * (255 - dwAlpha) / 255)
                  & LE_BLT_dwGreenMask;
                dwAlphaData |=
                  ((dwDestinationData & LE_BLT_dwBlueMask) +
                  (dwSourceData & LE_BLT_dwBlueMask) * (255 - dwAlpha) / 255)
                  & LE_BLT_dwBlueMask;
#endif
              #else
                // work out the final alpha effect -- dwAlphaData
                dwAlphaData = 
                  ((dwDestinationData & LE_BLT_dwRedMask) * dwAlpha / 255 +
                  (dwSourceData & LE_BLT_dwRedMask) * (255 - dwAlpha) / 255)
                  & LE_BLT_dwRedMask;
                dwAlphaData |=
                  ((dwDestinationData & LE_BLT_dwGreenMask) * dwAlpha / 255 +
                  (dwSourceData & LE_BLT_dwGreenMask) * (255 - dwAlpha) / 255)
                  & LE_BLT_dwGreenMask;
                dwAlphaData |=
                  ((dwDestinationData & LE_BLT_dwBlueMask) * dwAlpha / 255 +
                  (dwSourceData & LE_BLT_dwBlueMask) * (255 - dwAlpha) / 255)
                  & LE_BLT_dwBlueMask;
              #endif
            #endif  // end of full 255 alpha levals

              // copy the resultant dwAlphaData to destination bitmap
              #if INC_DEST_DEPTH == 16
                *((LPWORD)lpDestinationBltPointer) = (WORD)dwAlphaData;
                lpDestinationBltPointer += 2;
              #elif INC_DEST_DEPTH == 24
                *((LPWORD)lpDestinationBltPointer) = (WORD)dwAlphaData;
                lpDestinationBltPointer += 2;
                *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dwAlphaData >> 16);
                lpDestinationBltPointer += 1;
              #elif INC_DEST_DEPTH == 32
                *((LPLONG)lpDestinationBltPointer) = dwAlphaData;
                lpDestinationBltPointer += 4;
              #endif
            }
          }
          else
          {
            // update the destination bitmap pointer
            #if INC_DEST_DEPTH == 16
              lpDestinationBltPointer += 2;
            #elif INC_DEST_DEPTH == 24
              lpDestinationBltPointer += 3;
            #elif INC_DEST_DEPTH == 32
              lpDestinationBltPointer += 4;
            #endif
          }

          // get the pixel counter updated
          PixelCounter --;
        }

        // update pointers to the next scanline
        lpDestinationScanlineBeginBlt += DestinationBitmapWidthInBytes;
        lpSourceScanlineBeginBlt += SourceBitmapWidthInBytes;
        LinesToBlt --;
      }

      // successful
      return TRUE;   // from 8 bit to 16, 24, and 32 bit colour depths
    }
    #endif // (Alpha and Unity)
  #endif // raw source bitmap
#endif // alpha & unity

/*************************************************************************
 *
 *      Solid and Stretching Bitmap Copy
 *
 *  DESCRIPTION:
 *     Copies a source bitmap to a destination bitmap, stretches
 *     it when needed.
 *
 *     The copy mode includes:
 *         from 8  bit to 16 bit colour depth
 *         from 8  bit to 24 bit colour depth
 *         from 8  bit to 32 bit colour depth
 *         from 16 bit to 16 bit colour depth
 *         from 24 bit to 24 bit colour depth
 *         from 32 bit to 32 bit colour depth
 *
 *  RETURN:
 *     TRUE for success, FALSE for failure.
 *
 *  10:28:00  Wednesday 15 July 1998
 *
 *  ---------------------------------------------------------------------
 *
 *      Solid with Colour Key/Holes and Stretching Bitmap Copy
 *
 *  DESCRIPTION:
 *     Copies a source bitmap to a destination bitmap with colour key effect,
 *     stretches it when needed.
 *
 *     The colour key copy mode includes:
 *         from 8  bit to 16 bit colour depth
 *         from 8  bit to 24 bit colour depth
 *         from 8  bit to 32 bit colour depth
 *         from 16 bit to 16 bit colour depth
 *         from 24 bit to 24 bit colour depth
 *         from 32 bit to 32 bit colour depth
 *
 *     The hole copy mode includes:
 *         from 8  bit to 16 bit colour depth
 *         from 8  bit to 24 bit colour depth
 *         from 8  bit to 32 bit colour depth
 *
 *  RETURN:
 *     TRUE for success, FALSE for failure.
 *
 *  10:20:55  Wednesday 12 August 1998
 *
 *************************************************************************/
#if (INC_STRETCHMODE == INC_STRETCH && INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0) || \
    (INC_STRETCHMODE == INC_STRETCH && INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1) || \
    (INC_STRETCHMODE == INC_STRETCH && INC_COPYMODE == INC_HOLES && INC_COLOURKEY == 0)
  #if INC_SOURCEFORMAT == INC_RAWBITMAP

    // do solid (holes) & stretching bitmap copy for 8, 16, 24, and 32 bit colour depths, raw source bitmap

    #if (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16) ||  \
        (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24) ||  \
        (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 32) ||  \
        (INC_SOURCE_DEPTH == 16 && INC_DEST_DEPTH == 16) ||  \
        (INC_SOURCE_DEPTH == 24 && INC_DEST_DEPTH == 24) ||  \
        (INC_SOURCE_DEPTH == 32 && INC_DEST_DEPTH == 32)
      {
        int   SourceOffsetX;         // offset in pixels
        int   SourceOffsetY;         // offset in pixels
        int   DestinationOffsetX;    // offset in pixels
        int   DestinationOffsetY;    // offset in pixels

        int   SourceWidth;           // width of source rectangle
        int   SourceHeight;          // height of souce rectangle
        int   DestinationWidth;      // width of destination rectangle
        register int   DestinationHeight;     // height of destination rectangle

        int   SourceBitmapWidthInBytes;       // source bitmap width in bytes
        int   DestinationBitmapWidthInBytes;  // destination bitmap width in bytes

        register UNS8Pointer  SourceScanlineBltPtr;       // pointer to each scan line to blt
        register UNS8Pointer  DestinationScanlineBltPtr;  // pointer to each scan line to blt

        register UNS8Pointer  SourceBitsPntr;       // pointer to current source pixel to copy from
        register UNS8Pointer  DestinationBitsPntr;  // pointer to current destination pixel to copy to

        register UNS8Pointer   XOffsetPntr;
        register UNS8Pointer   YOffsetPntr;

        register unsigned char  Counter;
        register int   XPixelCounter;
        register int   YPixelCounter;
        register int   CurrentRemainingPixel;
        register int   CurrentDestinationWidth;

        #if ( INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0 ) // no colour key effect
          #if (INC_SOURCE_DEPTH != INC_DEST_DEPTH)
            register DWORD dwSourceData;           // holder for source data
            #if INC_DEST_DEPTH == 16 || INC_DEST_DEPTH == 24
              register DWORD   dwDestinationData1; // holders for destination data
            #endif
          #endif
        #elif ( INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1 )  // colour key is defined
          register DWORD  redLimit;
          register DWORD  greenLimit;
          register DWORD  blueLimit;
          register DWORD  redMask = LE_BLT_dwRedMask;
          register DWORD  greenMask = LE_BLT_dwGreenMask;
          register DWORD  blueMask = LE_BLT_dwBlueMask;
          register DWORD  rawPixel;
        #elif (INC_COPYMODE == INC_HOLES)  // holes copy mode
          register DWORD  rawPixel;
        #endif

        StretchTableRecord XStretchTable;
        StretchTableRecord YStretchTable;

        // validate the source/destination bitmap pointers

        if( SourceBitmapBits == NULL )
        {
        #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, NAME_AS_STRING ": SourceBitmapBits == NULL\n\n");
          LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
        #endif
          return FALSE;  // invalid source bitmap pointer
        }

        if( DestinationBitmapBits == NULL )
        {
        #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, NAME_AS_STRING ": DestinationBitmapBits == NULL\n\n");
          LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
        #endif
          return FALSE;  // invalid destination bitmap pointer
        }

        // prepare some parameters for image clipping and function calls

        SourceOffsetX = (int) SourceRectangle->left;
        SourceOffsetY = (int) SourceRectangle->top;
        SourceWidth   = (int) (SourceRectangle->right - SourceRectangle->left);
        SourceHeight  = (int) (SourceRectangle->bottom - SourceRectangle->top);

        DestinationOffsetX = (int) DestBoundingRectangle->left;
        DestinationOffsetY = (int) DestBoundingRectangle->top;
        DestinationWidth   = (int) (DestBoundingRectangle->right - DestBoundingRectangle->left);
        DestinationHeight  = (int) (DestBoundingRectangle->bottom - DestBoundingRectangle->top);

        // If colour key is defined, work out colour limits
        #if INC_COLOURKEY == 1
          // find the colour limits corresponding to the current colour depth.
          #if INC_DEST_DEPTH == 16
            redLimit = LI_BLT_ConvertColorRefTo16BitColor (
              LE_GRAFIX_MakeColorRef (LE_BLT_KeyRedLimit, 0, 0));
            greenLimit = LI_BLT_ConvertColorRefTo16BitColor (
              LE_GRAFIX_MakeColorRef (0, LE_BLT_KeyGreenLimit, 0));
            blueLimit = LI_BLT_ConvertColorRefTo16BitColor (
              LE_GRAFIX_MakeColorRef (0, 0, LE_BLT_KeyBlueLimit));
          #elif INC_DEST_DEPTH == 24
            redLimit = LI_BLT_ConvertColorRefTo24BitColor (
              LE_GRAFIX_MakeColorRef (LE_BLT_KeyRedLimit, 0, 0));
            greenLimit = LI_BLT_ConvertColorRefTo24BitColor (
              LE_GRAFIX_MakeColorRef (0, LE_BLT_KeyGreenLimit, 0));
            blueLimit = LI_BLT_ConvertColorRefTo24BitColor (
              LE_GRAFIX_MakeColorRef (0, 0, LE_BLT_KeyBlueLimit));
          #elif INC_DEST_DEPTH == 32
            redLimit = LI_BLT_ConvertColorRefTo32BitColor (
              LE_GRAFIX_MakeColorRef (LE_BLT_KeyRedLimit, 0, 0));
            greenLimit = LI_BLT_ConvertColorRefTo32BitColor (
              LE_GRAFIX_MakeColorRef (0, LE_BLT_KeyGreenLimit, 0));
            blueLimit = LI_BLT_ConvertColorRefTo32BitColor (
              LE_GRAFIX_MakeColorRef (0, 0, LE_BLT_KeyBlueLimit));
          #endif
        #endif

        // Special Case Handling -- no stretching is needed, if both
        // the source and destination rectangles are of the same size
        if( DestinationWidth == SourceWidth && DestinationHeight == SourceHeight )
        {
        #if INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0  // colour key is not defined
          // from 8 bit to 16 bit colour depth
          #if ( INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 16 )
            return LI_BLT_Copy8To16SolidUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceColorTable,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 8 bit to 24 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 24 )
            return LI_BLT_Copy8To24SolidUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceColorTable,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 8 bit to 32 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 32 )
            return LI_BLT_Copy8To32SolidUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceColorTable,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 16 bit to 16 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 16 && INC_DEST_DEPTH == 16 )
            return LI_BLT_Copy16To16SolidUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 24 bit to 24 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 24 && INC_DEST_DEPTH == 24 )
            return LI_BLT_Copy24To24SolidUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 32 bit to 32 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 32 && INC_DEST_DEPTH == 32 )
            return LI_BLT_Copy32To32SolidUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);
          #endif
        #elif INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1  // colour key is defined
          // from 8 bit to 16 bit colour depth
          #if ( INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 16 )
            return LI_BLT_Copy8To16SolidColourKeyUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceColorTable,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 8 bit to 24 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 24 )
            return LI_BLT_Copy8To24SolidColourKeyUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceColorTable,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 8 bit to 32 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 32 )
            return LI_BLT_Copy8To32SolidColourKeyUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceColorTable,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 16 bit to 16 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 16 && INC_DEST_DEPTH == 16 )
            return LI_BLT_Copy16To16SolidColourKeyUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 24 bit to 24 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 24 && INC_DEST_DEPTH == 24 )
            return LI_BLT_Copy24To24SolidColourKeyUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 32 bit to 32 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 32 && INC_DEST_DEPTH == 32 )
            return LI_BLT_Copy32To32SolidColourKeyUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);
          #endif
        #endif
        }  // end of special case, no stretching needed

        // If stretching is needed, then, work out the stretch mapping
        // from source to destination pixels

        // first, initialise the two StretchTable variables
        memset(&XStretchTable, 0, sizeof(StretchTableRecord));
        memset(&YStretchTable, 0, sizeof(StretchTableRecord));

        CalculateStretchTable(&XStretchTable, SourceBitmapWidth, SourceOffsetX,
          SourceWidth, DestinationBitmapWidth, DestinationOffsetX, DestinationWidth);
        
        CalculateStretchTable(&YStretchTable, SourceBitmapHeight, SourceOffsetY,
          SourceHeight, DestinationBitmapHeight, DestinationOffsetY, DestinationHeight);

        // validate the stretch table calculation
        if(XStretchTable.destinationStretchSize <= 0 ||
          YStretchTable.destinationStretchSize <= 0)
          return FALSE;  // Nothing to be done.
        
        // get the final results from the above two calculated stretch tables
        //
        // Note: If the DestInvalidRectangle is given (not NULL), then, work
        //       out the corresponding source area (inside SourceRectangle)
        //       to update.

        if( DestInvalidRectangle != NULL )
        {
          // get the true start offset within the destination rectangle
          DestinationOffsetX = (int) DestInvalidRectangle->left;
          DestinationOffsetY = (int) DestInvalidRectangle->top;

          // width and height to blt
          DestinationWidth = (int) (DestInvalidRectangle->right - DestInvalidRectangle->left);
          DestinationHeight = (int) (DestInvalidRectangle->bottom - DestInvalidRectangle->top);

          // now, find the corresponding start offset in the source rectangle

          // first do X direction (horizontal)

          if( DestInvalidRectangle->left == 0 )
          {
            CurrentRemainingPixel = 0;
          }
          else if( DestInvalidRectangle->left > 0 && DestBoundingRectangle->left >=0 )
          {
            CurrentRemainingPixel = (int) (DestInvalidRectangle->left - DestBoundingRectangle->left);
          }
          else
          {
            CurrentRemainingPixel = (int) DestInvalidRectangle->left;
          }

          XPixelCounter = 0;
          XOffsetPntr = XStretchTable.pixelOffsets;
          while( CurrentRemainingPixel > 0 )
          {
            // Skip the source pixel to the next closest column.
            if( (Counter = *XOffsetPntr++) != 0 )
            {
              XPixelCounter += Counter;
            }

            // update the CurrentRemainingPixel
            CurrentRemainingPixel--;
          }

          // get the source start offset in X direction
          SourceOffsetX = XStretchTable.sourceStartOffset + XPixelCounter;

          // now do Y direction (vertical)

          if( DestInvalidRectangle->top == 0 )
          {
            CurrentRemainingPixel = 0;
          }
          else if( DestInvalidRectangle->top > 0 && DestBoundingRectangle->top >= 0 )
          {
            CurrentRemainingPixel = (int) (DestInvalidRectangle->top - DestBoundingRectangle->top);
          }
          else
          {
            CurrentRemainingPixel = (int) DestInvalidRectangle->top;
          }

          YPixelCounter = 0;
          YOffsetPntr = YStretchTable.pixelOffsets;
          while( CurrentRemainingPixel > 0 )
          {
            // Skip the source pixel to the next closest row.
            if( (Counter = *YOffsetPntr++) != 0 )
            {
              YPixelCounter += Counter;
            }

            // update the CurrentRemainingPixel
            CurrentRemainingPixel--;
          }

          // get the source start offset in Y direction
          SourceOffsetY = YStretchTable.sourceStartOffset + YPixelCounter;
        }
        else  // don't know the DestInvalidRectangle
        {
          SourceOffsetX = XStretchTable.sourceStartOffset;
          SourceOffsetY = YStretchTable.sourceStartOffset;
          
          DestinationOffsetX = XStretchTable.destinationStartOffset;
          DestinationOffsetY = YStretchTable.destinationStartOffset;
          
          DestinationWidth = XStretchTable.destinationStretchSize;
          DestinationHeight = YStretchTable.destinationStretchSize;
        }

        // find bitmap width in bytes for both source and destination bitmaps
        // and force pixels to align by 4 bytes boundary

        #if INC_SOURCE_DEPTH == 8
          SourceBitmapWidthInBytes = (SourceBitmapWidth + 3) & 0xfffffffc;
        #elif INC_SOURCE_DEPTH == 16
          SourceBitmapWidthInBytes = (SourceBitmapWidth * 2 + 3) & 0xfffffffc;
        #elif INC_SOURCE_DEPTH == 24
          SourceBitmapWidthInBytes = (SourceBitmapWidth * 3 + 3) & 0xfffffffc;
        #elif INC_SOURCE_DEPTH == 32
          SourceBitmapWidthInBytes = SourceBitmapWidth * 4;
        #endif

        #if INC_DEST_DEPTH == 16
          DestinationBitmapWidthInBytes = (DestinationBitmapWidth * 2 + 3) & 0xfffffffc;
        #elif INC_DEST_DEPTH == 24
          DestinationBitmapWidthInBytes = (DestinationBitmapWidth * 3 + 3) & 0xfffffffc;
        #elif INC_DEST_DEPTH == 32
          DestinationBitmapWidthInBytes = DestinationBitmapWidth * 4;
        #endif

        // get pointers to their scan lines to begin blt
        // for both source and destination bitmaps

        #if INC_SOURCE_DEPTH == 8
          SourceScanlineBltPtr = SourceBitmapBits +
            SourceBitmapWidthInBytes * SourceOffsetY;
          SourceScanlineBltPtr += SourceOffsetX;
        #elif INC_SOURCE_DEPTH == 16
          SourceScanlineBltPtr = SourceBitmapBits +
            SourceBitmapWidthInBytes * SourceOffsetY;
          SourceScanlineBltPtr += SourceOffsetX * 2;
        #elif INC_SOURCE_DEPTH == 24
          SourceScanlineBltPtr = SourceBitmapBits +
            SourceBitmapWidthInBytes * SourceOffsetY;
          SourceScanlineBltPtr += SourceOffsetX * 3;
        #elif INC_SOURCE_DEPTH == 32
          SourceScanlineBltPtr = SourceBitmapBits +
            SourceBitmapWidthInBytes * SourceOffsetY;
          SourceScanlineBltPtr += SourceOffsetX * 4;
        #endif

        #if INC_DEST_DEPTH == 16
          DestinationScanlineBltPtr = DestinationBitmapBits +
            DestinationBitmapWidthInBytes * DestinationOffsetY;
          DestinationScanlineBltPtr += DestinationOffsetX * 2;
        #elif INC_DEST_DEPTH == 24
          DestinationScanlineBltPtr = DestinationBitmapBits +
            DestinationBitmapWidthInBytes * DestinationOffsetY;
          DestinationScanlineBltPtr += DestinationOffsetX * 3;
        #elif INC_DEST_DEPTH == 32
          DestinationScanlineBltPtr = DestinationBitmapBits +
            DestinationBitmapWidthInBytes * DestinationOffsetY;
          DestinationScanlineBltPtr += DestinationOffsetX * 4;
        #endif

        // find pixel offsets for DestInvalidRectangle

        YPixelCounter = 0;
        if( DestInvalidRectangle != NULL )
        {
          if( DestInvalidRectangle->top == 0 )
          {
            YPixelCounter = 0;
          }
          else if( DestInvalidRectangle->top > 0 && DestBoundingRectangle->top >= 0 )
          {
            YPixelCounter = (int) (DestInvalidRectangle->top - DestBoundingRectangle->top);
          }
          else
          {
            YPixelCounter = (int) DestInvalidRectangle->top;
          }
        }

        YOffsetPntr = YStretchTable.pixelOffsets + YPixelCounter;

        XPixelCounter = 0;
        if( DestInvalidRectangle != NULL )
        {
          if( DestInvalidRectangle->left == 0 )
          {
            XPixelCounter = 0;
          }
          else if( DestInvalidRectangle->left > 0 && DestBoundingRectangle->left >= 0 )
          {
            XPixelCounter = (int)(DestInvalidRectangle->left - DestBoundingRectangle->left);
          }
          else
          {
            XPixelCounter = (int) DestInvalidRectangle->left;
          }
        }

        // Copy the scan lines as fast as possible.

        while(DestinationHeight > 0)
        {
          // Skip the source row pointer to the next closest row.
          if( (Counter = *YOffsetPntr++) != 0 )
            SourceScanlineBltPtr += Counter * SourceBitmapWidthInBytes;
          
          // Now copy a scan line, stretching as we go.
          
          // get to the pixel to copy from (in source bitmap) and to (in destination bitmap)

          DestinationBitsPntr = DestinationScanlineBltPtr;
          SourceBitsPntr = SourceScanlineBltPtr;

          XOffsetPntr = (UNS8Pointer) XStretchTable.pixelOffsets + XPixelCounter;

          CurrentDestinationWidth = DestinationWidth;

          // copy a scan line, stretch as we go

          while (CurrentDestinationWidth > 0)
          {
            // Skip the source pointer to the next source pixel.
            
            if ((Counter = *XOffsetPntr++) != 0)
            {
              // Special cases for each of the depths we handle.
              #if INC_SOURCE_DEPTH == 8
                SourceBitsPntr += Counter;
              #elif INC_SOURCE_DEPTH == 16
                SourceBitsPntr += Counter * 2;
              #elif INC_SOURCE_DEPTH == 24
                SourceBitsPntr += Counter * 3;
              #elif INC_SOURCE_DEPTH == 32
                SourceBitsPntr += Counter * 4;
              #endif
            }
            
            // Copy a pixel from source to destination and position the
            // destination pointer at the next destination pixel.

            #if (INC_SOURCE_DEPTH == INC_DEST_DEPTH)  // the same source/destination colour depth

              // for 16 bit colour depth
              #if INC_SOURCE_DEPTH == 16
                #if INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0  // no colour key defined
                  *((LPWORD)DestinationBitsPntr) = *((LPWORD)SourceBitsPntr);
                  DestinationBitsPntr += 2;
                #elif INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1  // with colour key
                  // get one pixel of source data
                  rawPixel = *((LPWORD)SourceBitsPntr);

                  // see if the pixel needs to be copied
                  #if CE_ARTLIB_BlitUseGreenColourKey
                    if( (rawPixel & redMask) >= redLimit  ||
                      (rawPixel & greenMask) < greenLimit ||
                      (rawPixel & blueMask) >= blueLimit )
                  #elif CE_ARTLIB_BlitUseBlueColourKey
                    if( (rawPixel & redMask) >= redLimit   ||
                      (rawPixel & greenMask) >= greenLimit ||
                      (rawPixel & blueMask) < blueLimit )
                  #else // Just use absolute black as transparent.
                    if( (rawPixel & 0x00FFFFFF) != 0 )
                  #endif
                  {
                    // copy this pixel to destination bitmap
                    *((LPWORD)DestinationBitsPntr) = (WORD) rawPixel;
                  }

                  // update DestinationBitsPntr
                  DestinationBitsPntr += 2;
                #endif  // end of colour key

              // for 24 bit colour depth
              #elif INC_SOURCE_DEPTH == 24
                #if INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0  // no colour key defined
                  *((LPWORD)DestinationBitsPntr) = *((LPWORD)SourceBitsPntr);
                  SourceBitsPntr += 2;
                  DestinationBitsPntr += 2;
                  *((LPBYTE)DestinationBitsPntr) = *((LPBYTE)SourceBitsPntr);
                  DestinationBitsPntr += 1;

                  // set SourceBitsPntr back 2 bytes ( restore it )
                  SourceBitsPntr -= 2;
                #elif INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1  // with colour key
                  // get one pixel of source data
                  rawPixel = *((LPLONG)SourceBitsPntr);

                  // see if the pixel needs to be copied
                  #if CE_ARTLIB_BlitUseGreenColourKey
                    if( (rawPixel & redMask) >= redLimit  ||
                      (rawPixel & greenMask) < greenLimit ||
                      (rawPixel & blueMask) >= blueLimit )
                  #elif CE_ARTLIB_BlitUseBlueColourKey
                    if( (rawPixel & redMask) >= redLimit   ||
                      (rawPixel & greenMask) >= greenLimit ||
                      (rawPixel & blueMask) < blueLimit )
                  #else // Just use absolute black as transparent.
                    if( (rawPixel & 0x00FFFFFF) != 0 )
                  #endif
                  {
                    *((LPWORD)DestinationBitsPntr) = (WORD) rawPixel;
                    DestinationBitsPntr += 2;
                    *((LPBYTE)DestinationBitsPntr) = (BYTE) (((DWORD) rawPixel) >> 16);
                    DestinationBitsPntr ++;
                  }
                  else
                  {
                    // if this pixel is not copied (transparent),
                    // update DestinationBitsPntr
                    DestinationBitsPntr += 3;  // advanced 3 bytes
                  }
                #endif  // end of colour key

              // for 32 bit colour depth
              #elif INC_SOURCE_DEPTH == 32
                #if INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0  // no colour key defined
                  *((LPLONG)DestinationBitsPntr) = *((LPLONG)SourceBitsPntr);
                  DestinationBitsPntr += 4;
                #elif INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1  // with colour key
                  // get one pixel of source data
                  rawPixel = *((LPLONG)SourceBitsPntr);

                  // see if the pixel needs to be copied
                  #if CE_ARTLIB_BlitUseGreenColourKey
                    if( (rawPixel & redMask) >= redLimit  ||
                      (rawPixel & greenMask) < greenLimit ||
                      (rawPixel & blueMask) >= blueLimit )
                  #elif CE_ARTLIB_BlitUseBlueColourKey
                    if( (rawPixel & redMask) >= redLimit   ||
                      (rawPixel & greenMask) >= greenLimit ||
                      (rawPixel & blueMask) < blueLimit )
                  #else // Just use absolute black as transparent.
                    if( (rawPixel & 0x00FFFFFF) != 0 )
                  #endif
                  {
                    *((LPLONG)DestinationBitsPntr) = rawPixel;
                  }

                  // update DestinationBitsPntr
                  DestinationBitsPntr += 4;  // advanced 4 bytes
                #endif  // end of colour key
              #endif
            #else   // different source/destination colour depths

              // get one pixel of source data
              #if  INC_SOURCE_DEPTH == 8
                #if (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0)    // no colour key defined
                  dwSourceData = *SourceBitsPntr;
                #elif (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1)  // colour key defined */
                  rawPixel = *SourceBitsPntr;
                #elif (INC_COPYMODE == INC_HOLES)  // holes copy mode
                  rawPixel = *SourceBitsPntr;
                #endif
              #endif

              // from 8 bit to 16 bit colour depth
              #if INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16

                // convert 8 bit indices to 16 bit color values
                // and copy the converted data to destination bitmap

                #if( INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0 )  // no colour key effect
                  dwDestinationData1 = SourceColorTable[(dwSourceData & 0xff) * 2];
                  *((LPWORD)DestinationBitsPntr) = (WORD)dwDestinationData1;
                  DestinationBitsPntr += 2;

                #elif( INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1 ) // colour key is defined
                  rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];

                  // see if the pixel needs to be copied
                  #if CE_ARTLIB_BlitUseGreenColourKey
                    if( (rawPixel & redMask) >= redLimit  ||
                      (rawPixel & greenMask) < greenLimit ||
                      (rawPixel & blueMask) >= blueLimit )
                  #elif CE_ARTLIB_BlitUseBlueColourKey
                    if( (rawPixel & redMask) >= redLimit   ||
                      (rawPixel & greenMask) >= greenLimit ||
                      (rawPixel & blueMask) < blueLimit )
                  #else // Just use absolute black as transparent.
                    if( (rawPixel & 0x00FFFFFF) != 0 )
                  #endif
                  {
                    *((LPWORD)DestinationBitsPntr) = (WORD) rawPixel;
                  }

                  // update DestinationBitsPntr
                  DestinationBitsPntr += 2;  // advanced 2 bytes

                #elif (INC_COPYMODE == INC_HOLES)
                  // check to see if rawPixel is nonzero.
                  // If it is zero (transparent or hole), skip over it.
                  // If it is nonzero (solid), copy it to destination bitmap
                  if( rawPixel != 0 )
                  {
                    rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];
                    *((LPWORD)DestinationBitsPntr) = (WORD) rawPixel;
                  }

                  // update DestinationBitsPntr
                  DestinationBitsPntr += 2;  // advanced 2 bytes
                #endif  // end of colour key

              // from 8 bit to 24 bit colour depth
              #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24

                #if (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0 )  // no colour key defined
                  // convert 8 bit indices to 24 bit color values
                  dwDestinationData1 = SourceColorTable[(dwSourceData & 0xff) * 2];

                  // copy the converted data to destination bitmap
                  *((LPWORD)DestinationBitsPntr) = (WORD)(dwDestinationData1);
                  DestinationBitsPntr += 2;
                  *((LPBYTE)DestinationBitsPntr) = (BYTE)(dwDestinationData1 >> 16);
                  DestinationBitsPntr += 1;

                #elif (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1 )  // colour key defined
                  rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];

                  // see if the pixel needs to be copied
                  #if CE_ARTLIB_BlitUseGreenColourKey
                    if( (rawPixel & redMask) >= redLimit  ||
                      (rawPixel & greenMask) < greenLimit ||
                      (rawPixel & blueMask) >= blueLimit )
                  #elif CE_ARTLIB_BlitUseBlueColourKey
                    if( (rawPixel & redMask) >= redLimit   ||
                      (rawPixel & greenMask) >= greenLimit ||
                      (rawPixel & blueMask) < blueLimit )
                  #else // Just use absolute black as transparent.
                    if( (rawPixel & 0x00FFFFFF) != 0 )
                  #endif
                  {
                    *((LPWORD)DestinationBitsPntr) = (WORD) rawPixel;
                    DestinationBitsPntr += 2;
                    *((LPBYTE)DestinationBitsPntr) = (BYTE) (((DWORD) rawPixel) >> 16);
                    DestinationBitsPntr ++;
                  }
                  else
                  {
                    // if the pixel is not copied (transparent),
                    // update DestinationBitsPntr
                    DestinationBitsPntr += 3;
                  }

                #elif (INC_COPYMODE == INC_HOLES)
                  // check to see if rawPixel is nonzero.
                  // If it is zero (transparent or hole), skip over it.
                  // If it is nonzero (solid), copy it to destination bitmap
                  if( rawPixel != 0 )
                  {
                    rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];

                    *((LPWORD)DestinationBitsPntr) = (WORD) rawPixel;
                    DestinationBitsPntr += 2;
                    *((LPBYTE)DestinationBitsPntr) = (BYTE) (((DWORD) rawPixel) >> 16);
                    DestinationBitsPntr ++;
                  }
                  else
                  {
                    // it is a hole, advance DestinationBitsPntr 3 bytes
                    DestinationBitsPntr += 3;
                  }
                #endif  // end of colour key

              // from 8 bit to 32 bit colour depth
              #elif INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 32
                // convert 8 bit indices to 32 bit color values
                // and copy the converted data to destination bitmap

                #if (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 0 )  // no colour key defined
                  *((LPLONG)DestinationBitsPntr) = SourceColorTable[(dwSourceData & 0xff) * 2];
                  DestinationBitsPntr += 4;

                #elif (INC_COPYMODE == INC_SOLID && INC_COLOURKEY == 1 )  // colour key defined
                  rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];

                  // see if the pixel needs to be copied
                  #if CE_ARTLIB_BlitUseGreenColourKey
                    if( (rawPixel & redMask) >= redLimit  ||
                      (rawPixel & greenMask) < greenLimit ||
                      (rawPixel & blueMask) >= blueLimit )
                  #elif CE_ARTLIB_BlitUseBlueColourKey
                    if( (rawPixel & redMask) >= redLimit   ||
                      (rawPixel & greenMask) >= greenLimit ||
                      (rawPixel & blueMask) < blueLimit )
                  #else // Just use absolute black as transparent.
                    if( (rawPixel & 0x00FFFFFF) != 0 )
                  #endif
                  {
                    *((LPLONG)DestinationBitsPntr) = (DWORD) rawPixel;
                  }

                  // update DestinationBitsPntr
                  DestinationBitsPntr += 4;

                #elif (INC_COPYMODE == INC_HOLES)
                  // check to see if rawPixel is nonzero.
                  // If it is zero (transparent or hole), skip over it.
                  // If it is nonzero (solid), copy it to destination bitmap
                  if( rawPixel != 0 )
                  {
                    rawPixel = SourceColorTable[(rawPixel & 0xff) * 2];

                    *((LPLONG)DestinationBitsPntr) = (DWORD) rawPixel;
                  }

                  // update DestinationBitsPntr
                  DestinationBitsPntr += 4;
                #endif  // end of colour key
              #endif
            #endif  // end of different source/destination colour depths

            // update the CurrentDestinationWidth
            CurrentDestinationWidth--;
          }
          
          // Move down to the next destination scan line.
          
          DestinationScanlineBltPtr += DestinationBitmapWidthInBytes;
          DestinationHeight--;
        }
        return TRUE;
      }
    #endif // for 8, 16, 24, and 32 bit colour depths
  #endif // raw source bitmap
#endif // (stretching and solid) || (solid with colour key & stretching)

/*************************************************************************
 *
 *            Alpha and Stretching Bitmap Copy
 *
 *  DESCRIPTION:
 *     Copies a source bitmap to a destination bitmap, with alpha channel
 *     effect, stretches when needed.
 *
 *     The copy mode includes:
 *         from 8  bit to 16 bit colour depth
 *         from 8  bit to 24 bit colour depth
 *         from 8  bit to 32 bit colour depth
 *         from 16 bit to 16 bit colour depth
 *         from 24 bit to 24 bit colour depth
 *         from 32 bit to 32 bit colour depth
 *
 *  RETURN:
 *     TRUE for success, FALSE for failure.
 *
 *  14:05:00  Wednesday 22 July 1998
 *
 *************************************************************************/
#if INC_STRETCHMODE == INC_STRETCH && INC_COPYMODE == INC_ALPHA
  #if INC_SOURCEFORMAT == INC_RAWBITMAP

    // do alpha & stretching bitmap copy from 8 bit to 16, 24, and 32 bit
    // colour depths, raw source bitmap

    #if (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 16) ||  \
        (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 24) ||  \
        (INC_SOURCE_DEPTH == 8  && INC_DEST_DEPTH == 32) ||  \
        (INC_SOURCE_DEPTH == 16 && INC_DEST_DEPTH == 16) ||  \
        (INC_SOURCE_DEPTH == 24 && INC_DEST_DEPTH == 24) ||  \
        (INC_SOURCE_DEPTH == 32 && INC_DEST_DEPTH == 32)
      {
        int   SourceOffsetX;         // offset in pixels
        int   SourceOffsetY;         // offset in pixels
        int   DestinationOffsetX;    // offset in pixels
        int   DestinationOffsetY;    // offset in pixels

        int   SourceWidth;           // width of source rectangle
        int   SourceHeight;          // height of souce rectangle
        int   DestinationWidth;      // width of destination rectangle
        int   DestinationHeight;     // height of destination rectangle

        int   SourceBitmapWidthInBytes;       // source bitmap width in bytes
        int   DestinationBitmapWidthInBytes;  // destination bitmap width in bytes

        UNS8Pointer  SourceScanlineBltPtr;       // pointer to each scan line to blt
        UNS8Pointer  DestinationScanlineBltPtr;  // pointer to each scan line to blt

        register UNS8Pointer  lpSourceBltPointer;       // pointer to current source pixel to copy from
        register UNS8Pointer  lpDestinationBltPointer;  // pointer to current destination pixel to copy to

        register UNS8Pointer   XOffsetPntr;
        register UNS8Pointer   YOffsetPntr;

        register unsigned char  Counter;
        register int   XPixelCounter;
        register int   YPixelCounter;
        register int   CurrentRemainingPixel;
        register int   CurrentDestinationWidth;

        register DWORD dwSourceData;        // holder for source data
        register DWORD dwDestinationData;   // holders for destination data

        register DWORD dwAlphaData;   // holder source and data blended
        register DWORD dwAlpha;       // 32 bit alpha channel
        #if INC_SOURCE_DEPTH != 8
          WORD  wPureGreen;            // to filter out pure green colour
        #endif

        StretchTableRecord  XStretchTable;  // stretch table, horizontal
        StretchTableRecord  YStretchTable;  // strctch table, vertical

        // validate the source/destination bitmap pointers

        if( SourceBitmapBits == NULL )
        {
        #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, NAME_AS_STRING ": SourceBitmapBits == NULL\n\n");
          LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
        #endif
          return FALSE;  // invalid source bitmap pointer
        }

        if( DestinationBitmapBits == NULL )
        {
        #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, NAME_AS_STRING ": DestinationBitmapBits == NULL\n\n");
          LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
        #endif
          return FALSE;  // invalid destination bitmap pointer
        }

        // prepare some parameters for image clipping and function calls

        SourceOffsetX = (int) SourceRectangle->left;
        SourceOffsetY = (int) SourceRectangle->top;
        SourceWidth   = (int) (SourceRectangle->right - SourceRectangle->left);
        SourceHeight  = (int) (SourceRectangle->bottom - SourceRectangle->top);

        DestinationOffsetX = (int) DestBoundingRectangle->left;
        DestinationOffsetY = (int) DestBoundingRectangle->top;
        DestinationWidth   = (int) (DestBoundingRectangle->right - DestBoundingRectangle->left);
        DestinationHeight  = (int) (DestBoundingRectangle->bottom - DestBoundingRectangle->top);

        // Special Case Handling -- no stretching is needed, if both
        // the source and destination rectangles are of the same size
        if( DestinationWidth == SourceWidth && DestinationHeight == SourceHeight )
        {
          // from 8 bit to 16 bit colour depth
          #if ( INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 16 )
            return LI_BLT_Copy8To16AlphaUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceColorTable,
              NumberOfAlphaColors,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 8 bit to 24 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 24 )
            return LI_BLT_Copy8To24AlphaUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceColorTable,
              NumberOfAlphaColors,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 8 bit to 32 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 32 )
            return LI_BLT_Copy8To32AlphaUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              SourceColorTable,
              NumberOfAlphaColors,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 16 bit to 16 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 16 && INC_DEST_DEPTH == 16 )
            return LI_BLT_Copy16To16AlphaUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              nAlphaValue,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 24 bit to 24 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 24 && INC_DEST_DEPTH == 24 )
            return LI_BLT_Copy24To24AlphaUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              nAlphaValue,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);

          // from 32 bit to 32 bit colour depth
          #elif ( INC_SOURCE_DEPTH == 32 && INC_DEST_DEPTH == 32 )
            return LI_BLT_Copy32To32AlphaUnityRaw (
              SourceBitmapBits,
              SourceBitmapWidth,
              SourceBitmapHeight,
              nAlphaValue,
              SourceRectangle,
              DestinationBitmapBits,
              DestinationBitmapWidth,
              DestinationBitmapHeight,
              DestInvalidRectangle,
              DestBoundingRectangle,
              (UNS16)DestinationOffsetX,
              (UNS16)DestinationOffsetY);
          #endif
        }  // end of special case, no stretching needed

        // If stretching is needed, then, work out the stretch mapping
        // from source to destination pixels

        // first, initialise the two StretchTable variables
        memset(&XStretchTable, 0, sizeof(StretchTableRecord));
        memset(&YStretchTable, 0, sizeof(StretchTableRecord));

        // work out the stretch mapping from source to destination pixels

        CalculateStretchTable(&XStretchTable, SourceBitmapWidth, SourceOffsetX,
          SourceWidth, DestinationBitmapWidth, DestinationOffsetX, DestinationWidth);
        
        CalculateStretchTable(&YStretchTable, SourceBitmapHeight, SourceOffsetY,
          SourceHeight, DestinationBitmapHeight, DestinationOffsetY, DestinationHeight);
        
        // validate the stretch table calculation
        if(XStretchTable.destinationStretchSize <= 0 ||
          YStretchTable.destinationStretchSize <= 0)
          return FALSE;  // Nothing to be done.
        
        // get the final results from the above two calculated stretch tables
        //
        // Note: If the DestInvalidRectangle is given (not NULL), then, work
        //       out the corresponding source area (inside SourceRectangle)
        //       to update.

        if( DestInvalidRectangle != NULL )
        {
          // get the true start offset within the destination rectangle
          DestinationOffsetX = (int) DestInvalidRectangle->left;
          DestinationOffsetY = (int) DestInvalidRectangle->top;

          // width and height to blt
          DestinationWidth = (int) (DestInvalidRectangle->right - DestInvalidRectangle->left);
          DestinationHeight = (int) (DestInvalidRectangle->bottom - DestInvalidRectangle->top);

          // now, work out the corresponding start offset in the source rectangle

          // first do X direction (horizontal)

          if( DestInvalidRectangle->left == 0 )
          {
            CurrentRemainingPixel = 0;
          }
          else if( DestInvalidRectangle->left > 0 && DestBoundingRectangle->left >= 0)
          {
            CurrentRemainingPixel = (int) (DestInvalidRectangle->left - DestBoundingRectangle->left);
          }
          else
          {
            CurrentRemainingPixel = (int) DestInvalidRectangle->left;
          }

          XPixelCounter = 0;
          XOffsetPntr = XStretchTable.pixelOffsets;
          while( CurrentRemainingPixel > 0 )
          {
            // Skip the source pixel to the next closest column.
            if( (Counter = *XOffsetPntr++) != 0 )
            {
              XPixelCounter += Counter;
            }

            // update the CurrentRemainingPixel
            CurrentRemainingPixel--;
          }

          // get the source start offset in X direction
          SourceOffsetX = XStretchTable.sourceStartOffset + XPixelCounter;

          // now do Y direction (vertical)

          if( DestInvalidRectangle->top == 0 )
          {
            CurrentRemainingPixel = 0;
          }
          else if( DestInvalidRectangle->top > 0 && DestBoundingRectangle->top >= 0 )
          {
            CurrentRemainingPixel = (int) (DestInvalidRectangle->top - DestBoundingRectangle->top);
          }
          else
          {
            CurrentRemainingPixel = (int) DestInvalidRectangle->top;
          }

          YPixelCounter = 0;
          YOffsetPntr = YStretchTable.pixelOffsets;
          while( CurrentRemainingPixel > 0 )
          {
            // Skip the source pixel to the next closest row.
            if( (Counter = *YOffsetPntr++) != 0 )
            {
              YPixelCounter += Counter;
            }

            // update the CurrentRemainingPixel
            CurrentRemainingPixel--;
          }

          // get the source start offset in Y direction
          SourceOffsetY = YStretchTable.sourceStartOffset + YPixelCounter;
        }
        else  // don't know the DestInvalidRectangle
        {
          SourceOffsetX = XStretchTable.sourceStartOffset;
          SourceOffsetY = YStretchTable.sourceStartOffset;
          
          DestinationOffsetX = XStretchTable.destinationStartOffset;
          DestinationOffsetY = YStretchTable.destinationStartOffset;
          
          DestinationWidth = XStretchTable.destinationStretchSize;
          DestinationHeight = YStretchTable.destinationStretchSize;
        }
        
        // find bitmap width in bytes for both source and destination bitmaps
        // and force pixels to align by 4 bytes boundary

        #if INC_SOURCE_DEPTH == 8
          SourceBitmapWidthInBytes = (SourceBitmapWidth + 3) & 0xfffffffc;
        #elif INC_SOURCE_DEPTH == 16
          SourceBitmapWidthInBytes = (SourceBitmapWidth * 2 + 3) & 0xfffffffc;
        #elif INC_SOURCE_DEPTH == 24
          SourceBitmapWidthInBytes = (SourceBitmapWidth * 3 + 3) & 0xfffffffc;
        #elif INC_SOURCE_DEPTH == 32
          SourceBitmapWidthInBytes = SourceBitmapWidth * 4;
        #endif

        #if INC_DEST_DEPTH == 16
          DestinationBitmapWidthInBytes = (DestinationBitmapWidth * 2  + 3) & 0xfffffffc;
        #elif INC_DEST_DEPTH == 24
          DestinationBitmapWidthInBytes = (DestinationBitmapWidth * 3 + 3) & 0xfffffffc;
        #elif INC_DEST_DEPTH == 32
          DestinationBitmapWidthInBytes = DestinationBitmapWidth * 4;
        #endif

        // get pointers to their scan lines to begin blt
        // for both source and destination bitmaps

        SourceScanlineBltPtr = SourceBitmapBits +
          SourceBitmapWidthInBytes * SourceOffsetY;
        #if INC_SOURCE_DEPTH == 8
          SourceScanlineBltPtr += SourceOffsetX;
        #elif INC_SOURCE_DEPTH == 16
          SourceScanlineBltPtr += SourceOffsetX * 2;
        #elif INC_SOURCE_DEPTH == 24
          SourceScanlineBltPtr += SourceOffsetX * 3;
        #elif INC_SOURCE_DEPTH == 32
          SourceScanlineBltPtr += SourceOffsetX * 4;
        #endif

        DestinationScanlineBltPtr = DestinationBitmapBits +
          DestinationBitmapWidthInBytes * DestinationOffsetY;
        #if INC_DEST_DEPTH == 16
          DestinationScanlineBltPtr += DestinationOffsetX * 2;
        #elif INC_DEST_DEPTH == 24
          DestinationScanlineBltPtr += DestinationOffsetX * 3;
        #elif INC_DEST_DEPTH == 32
          DestinationScanlineBltPtr += DestinationOffsetX * 4;
        #endif

        // find pixel offsets for DestInvalidRectangle

        YPixelCounter = 0;
        if( DestInvalidRectangle != NULL )
        {
          if( DestInvalidRectangle->top == 0 )
          {
            YPixelCounter = 0;
          }
          else if( DestInvalidRectangle->top > 0 && DestBoundingRectangle->top >= 0 )
          {
            YPixelCounter = (int) (DestInvalidRectangle->top - DestBoundingRectangle->top);
          }
          else
          {
            YPixelCounter = (int) DestInvalidRectangle->top;
          }
        }

        YOffsetPntr = YStretchTable.pixelOffsets + YPixelCounter;

        XPixelCounter = 0;
        if( DestInvalidRectangle != NULL )
        {
          if( DestInvalidRectangle->left == 0 )
          {
            XPixelCounter = 0;
          }
          else if( DestInvalidRectangle->left > 0 && DestBoundingRectangle->left >= 0 )
          {
            XPixelCounter = (int)(DestInvalidRectangle->left - DestBoundingRectangle->left);
          }
          else
          {
            XPixelCounter = (int) DestInvalidRectangle->left;
          }
        }

        // If not 8 bit colour depth, we need to filter out
        // all pure green colour pixels.
        #if INC_SOURCE_DEPTH == 16
          // make a COLORREF for pure green colour
          wPureGreen = (WORD) LI_BLT_ConvertColorRefTo16BitColor(
            (COLORREF) RGB (0, 255, 0));
        #elif INC_SOURCE_DEPTH == 24
          // make a COLORREF for pure green colour
          wPureGreen = (WORD) LI_BLT_ConvertColorRefTo24BitColor(
            (COLORREF) RGB (0, 255, 0));
        #elif INC_SOURCE_DEPTH == 32
          // make a COLORREF for pure green colour
          wPureGreen = (WORD) LI_BLT_ConvertColorRefTo32BitColor(
            (COLORREF) RGB (0, 255, 0));
        #endif

        #if INC_SOURCE_DEPTH != 8
          // get the alpha data
          dwAlpha = (DWORD) nAlphaValue;

          #if CE_ARTLIB_BlitUse255AlphaLevels // use full 255 alpha levels
            // check the validity of dwAlpha
            if ( dwAlpha < 0 || dwAlpha > 255 )
            {
              if ( dwAlpha < 0 )
                dwAlpha = 0;
              else
                dwAlpha = 255;
            }
          #endif
        #endif

        // Copy the scan lines as fast as possible.

        while(DestinationHeight > 0)
        {
          // Skip the source row pointer to the next closest row.
          if( (Counter = *YOffsetPntr++) != 0 )
            SourceScanlineBltPtr += Counter * SourceBitmapWidthInBytes;
          
          // Now copy a scan line, stretching as we go.
          
          // get to the pixel to copy from (in source bitmap) and to (in destination bitmap)

          lpDestinationBltPointer = DestinationScanlineBltPtr;
          lpSourceBltPointer = SourceScanlineBltPtr;

          XOffsetPntr = (UNS8Pointer) XStretchTable.pixelOffsets + XPixelCounter;

          CurrentDestinationWidth = DestinationWidth;

          // copy a scan line, stretch as we go

          while (CurrentDestinationWidth > 0)
          {
            // Skip the source pointer to the next source pixel.
            
            if ((Counter = *XOffsetPntr++) != 0)
              #if  INC_SOURCE_DEPTH == 8
                lpSourceBltPointer += Counter;
              #elif  INC_SOURCE_DEPTH == 16
                lpSourceBltPointer += Counter * 2;
              #elif  INC_SOURCE_DEPTH == 24
                lpSourceBltPointer += Counter * 3;
              #elif  INC_SOURCE_DEPTH == 32
                lpSourceBltPointer += Counter * 4;
              #endif
            
            // Copy a pixel from source to destination and position the
            // destination pointer at the next destination pixel.

            // process one pixel each time

            // get one pixel of source data
            #if  INC_SOURCE_DEPTH == 8
              dwSourceData = *lpSourceBltPointer;
            #elif  INC_SOURCE_DEPTH == 16
              dwDestinationData = *((LPWORD)lpSourceBltPointer);
              dwSourceData = dwDestinationData;
            #elif  INC_SOURCE_DEPTH == 24
//              dwDestinationData = *((LPLONG)lpSourceBltPointer);
//              dwSourceData = dwDestinationData &= 0x00ffffff;
              dwDestinationData = *((LPWORD)lpSourceBltPointer);
              lpSourceBltPointer += 2;
              dwDestinationData |= ((DWORD) *((LPBYTE)lpSourceBltPointer) << 16);
              lpSourceBltPointer -= 2; // restore the pointer position
              dwSourceData = dwDestinationData;
            #elif  INC_SOURCE_DEPTH == 32
              dwSourceData = dwDestinationData = *((LPLONG)lpSourceBltPointer);
            #endif

          #if INC_SOURCE_DEPTH == 8
            if( dwSourceData != 0 )           // use hole technique
          #else
            if( dwSourceData != wPureGreen )  // use pure green colour key
          #endif
            {
            #if INC_SOURCE_DEPTH == 8  // 8 bit (colour depth) only
              if ( dwSourceData >= NumberOfAlphaColors )
              {
                #if INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 16
                  *((LPWORD)lpDestinationBltPointer) = (WORD)SourceColorTable[(dwSourceData & 0xff) * 2];
                  lpDestinationBltPointer += 2;

                #elif INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 24
                  // convert 8 bit indices to 24 bit colour values
                  dwDestinationData = SourceColorTable[(dwSourceData & 0xff) * 2];

                  // copy the converted data to destination bitmap
                  *((LPWORD)lpDestinationBltPointer) = (WORD)(dwDestinationData);
                  lpDestinationBltPointer += 2;
                  *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dwDestinationData >> 16);
                  lpDestinationBltPointer += 1;

                #elif INC_SOURCE_DEPTH == 8 && INC_DEST_DEPTH == 32
                  // convert 8 bit indices to 32 bit color values
                  // and copy the converted data to destination bitmap
                  *((LPLONG)lpDestinationBltPointer) = SourceColorTable[(dwSourceData & 0xff) * 2];
                  lpDestinationBltPointer += 4;
                #endif
              }
              // apply alpha channel effect to both the source/destination bitmaps
              // ( see l_type.h for details )
              else
            #endif
              {
                //  get the alpha channel data from the source bitmap
                #if INC_SOURCE_DEPTH == 8
                  dwAlpha = SourceColorTable[(dwSourceData & 0xff) * 2 + 1];
                  dwDestinationData = SourceColorTable[(dwSourceData & 0xff) * 2];
                #endif

                // get one pixel data from the destination bitmap
                #if INC_DEST_DEPTH == 16
                  dwSourceData = *((LPWORD)lpDestinationBltPointer);
                #elif INC_DEST_DEPTH == 24
                  dwSourceData = *((LPLONG)lpDestinationBltPointer) & 0xffffff;
                #elif INC_DEST_DEPTH == 32
                  dwSourceData = *((LPLONG)lpDestinationBltPointer);
                #endif

                // do alpha channel effect here ...

              #if !CE_ARTLIB_BlitUse255AlphaLevels
                // Note that we deal with 8 bit colour depth differently from
                // other colour depths. For 8 bit colour depth, the palette
                // colours are pre-multiplied by the alpha value to save runtime
                // computation (so a 50% transparent red has a RGB value entry
                // of (127,0,0) rather than the 255,0,0 that you would expect).
                // But for other colour depths, we do not have the alpha value
                // premultiplied with the bitmap data. So, we have to apply
                // alpha channel effect to both source and destination bitmaps.
                switch( (int) dwAlpha )
                {
                #if INC_SOURCE_DEPTH == 8
                  case ALPHA_OPAQUE06_25:   // ((1*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                      (dwSourceData & LE_BLT_dwRedMask) - ((dwSourceData & LE_BLT_dwRedMask) >> 4))
                      & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                      (dwSourceData & LE_BLT_dwGreenMask) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) +
                      (dwSourceData & LE_BLT_dwBlueMask) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE12_50:  // ((2*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                      (dwSourceData & LE_BLT_dwRedMask) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                      (dwSourceData & LE_BLT_dwGreenMask) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) +
                      (dwSourceData & LE_BLT_dwBlueMask) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE18_75:  // ((3*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                      (dwSourceData & LE_BLT_dwRedMask) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                      (dwSourceData & LE_BLT_dwGreenMask) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) +
                      (dwSourceData & LE_BLT_dwBlueMask) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE25_00:  // ((4*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                      (dwSourceData & LE_BLT_dwRedMask) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                      (dwSourceData & LE_BLT_dwGreenMask) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) +
                      (dwSourceData & LE_BLT_dwBlueMask) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE31_25:  // ((5*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                      (dwSourceData & LE_BLT_dwRedMask) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 2) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                      (dwSourceData & LE_BLT_dwGreenMask) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 2) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) +
                      (dwSourceData & LE_BLT_dwBlueMask) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 2) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE37_50:  // ((6*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                      ((dwSourceData & LE_BLT_dwRedMask) >> 1) +
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) +
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 1) +
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE43_75:  // ((7*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) +
                      ((dwSourceData & LE_BLT_dwRedMask) >> 1) +
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 1) +
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 1) +
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE50_00:  // ((8*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 1)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 1)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 1)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE56_25:  // ((9*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 1) - 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 1) - 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 1) - 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE62_50:  // ((10*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE68_75:  // ((11*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE75_00:  // ((12*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE81_25:  // ((13*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE87_50:  // ((14*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE93_75:  // ((15*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  default:
                    dwAlphaData = dwDestinationData;  // if no match, just get the source pixel data only
                // For other colour depths, we need to apply the alpha value
                // to both source and destination bitmaps.
                #else
                  case ALPHA_OPAQUE06_25:   // ((1*255)/16)
                    dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 4) +
                      (dwSourceData & LE_BLT_dwRedMask) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4))
                      & LE_BLT_dwRedMask;
                    dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 4) +
                      (dwSourceData & LE_BLT_dwGreenMask) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 4) +
                      (dwSourceData & LE_BLT_dwBlueMask) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE12_50:  // ((2*255)/16)
                    dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 3) +
                      (dwSourceData & LE_BLT_dwRedMask) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                    dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 3) +
                      (dwSourceData & LE_BLT_dwGreenMask) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 3) +
                      (dwSourceData & LE_BLT_dwBlueMask) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE18_75:  // ((3*255)/16)
                    dwAlphaData =
                      (((dwDestinationData & LE_BLT_dwRedMask) >> 3) +
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 4) +
                      (dwSourceData & LE_BLT_dwRedMask) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwGreenMask) >> 3) +
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) +
                      (dwSourceData & LE_BLT_dwGreenMask) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwBlueMask) >> 3) +
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) +
                      (dwSourceData & LE_BLT_dwBlueMask) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE25_00:  // ((4*255)/16)
                    dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 2) +
                      (dwSourceData & LE_BLT_dwRedMask) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
                    dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) +
                      (dwSourceData & LE_BLT_dwGreenMask) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) +
                      (dwSourceData & LE_BLT_dwBlueMask) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE31_25:  // ((5*255)/16)
                    dwAlphaData =
                      (((dwDestinationData & LE_BLT_dwRedMask) >> 2) +
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 4) +
                      (dwSourceData & LE_BLT_dwRedMask) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 2) -
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) +
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) +
                      (dwSourceData & LE_BLT_dwGreenMask) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 2) -
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) +
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) +
                      (dwSourceData & LE_BLT_dwBlueMask) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 2) -
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE37_50:  // ((6*255)/16)
                    dwAlphaData =
                      (((dwDestinationData & LE_BLT_dwRedMask) >> 2) +
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 3) +
                      ((dwSourceData & LE_BLT_dwRedMask) >> 1) +
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) +
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) +
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 1) +
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + 
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 1) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE43_75:  // ((7*255)/16)
                    dwAlphaData =
                      (((dwDestinationData & LE_BLT_dwRedMask) >> 2) +
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 3) +
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 4) +
                      ((dwSourceData & LE_BLT_dwRedMask) >> 1) +
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwGreenMask) >> 2) + 
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) +
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) +
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 1) +
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + 
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + 
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 1) +
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE50_00:  // ((8*255)/16)
                    dwAlphaData = (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 1)) & LE_BLT_dwRedMask;
                    dwAlphaData |= (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 1)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 1)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE56_25:  // ((9*255)/16)
                    dwAlphaData =
                      (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + 
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 1) - 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + 
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 1) - 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + 
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 1) - 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE62_50:  // ((10*255)/16)
                    dwAlphaData =
                      (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + 
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + 
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + 
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE68_75:  // ((11*255)/16)
                    dwAlphaData =
                      (((dwDestinationData & LE_BLT_dwRedMask) >> 1) + 
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 3) + 
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwGreenMask) >> 1) + 
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + 
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |=
                      (((dwDestinationData & LE_BLT_dwBlueMask) >> 1) + 
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) + 
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE75_00:  // ((12*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) -
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 2)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) -
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 2)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) -
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 2) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 2)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE81_25:  // ((13*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) -
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 3) - 
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 4) +
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) -
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) -
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) -
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 3) -
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE87_50:  // ((14*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) -
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 3)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) -
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 3) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 3)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) -
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 3)+ 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 3)) & LE_BLT_dwBlueMask;
                    break;

                  case ALPHA_OPAQUE93_75:  // ((15*255)/16)
                    dwAlphaData = ((dwDestinationData & LE_BLT_dwRedMask) -
                      ((dwDestinationData & LE_BLT_dwRedMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwRedMask) >> 4)) & LE_BLT_dwRedMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwGreenMask) -
                      ((dwDestinationData & LE_BLT_dwGreenMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwGreenMask) >> 4)) & LE_BLT_dwGreenMask;
                    dwAlphaData |= ((dwDestinationData & LE_BLT_dwBlueMask) -
                      ((dwDestinationData & LE_BLT_dwBlueMask) >> 4) + 
                      ((dwSourceData & LE_BLT_dwBlueMask) >> 4)) & LE_BLT_dwBlueMask;
                    break;

                  default:
                    dwAlphaData = dwDestinationData;  // if no match, just get the source pixel data only
                #endif
                }

              #else  // do full 255 alpha levels
                #if INC_SOURCE_DEPTH == 8
#if 0  // This bit code sometimes causes wrong colour effects along edges.
                  // work out the final alpha effect -- dwAlphaData
                  dwAlphaData = 
                    ((dwDestinationData & LE_BLT_dwRedMask) +
                    (dwSourceData & LE_BLT_dwRedMask) -
                    (dwSourceData & LE_BLT_dwRedMask) * dwAlpha / 255)
                    & LE_BLT_dwRedMask;
                  dwAlphaData |=
                    ((dwDestinationData & LE_BLT_dwGreenMask) +
                    (dwSourceData & LE_BLT_dwGreenMask) -
                    (dwSourceData & LE_BLT_dwGreenMask) * dwAlpha / 255)
                    & LE_BLT_dwGreenMask;
                  dwAlphaData |=
                    ((dwDestinationData & LE_BLT_dwBlueMask) +
                    (dwSourceData & LE_BLT_dwBlueMask) -
                    (dwSourceData & LE_BLT_dwBlueMask) * dwAlpha / 255)
                    & LE_BLT_dwBlueMask;
#else
                  dwAlphaData = 
                    ((dwDestinationData & LE_BLT_dwRedMask) +
                    (dwSourceData & LE_BLT_dwRedMask) * (255 - dwAlpha) / 255)
                    & LE_BLT_dwRedMask;
                  dwAlphaData |=
                    ((dwDestinationData & LE_BLT_dwGreenMask) +
                    (dwSourceData & LE_BLT_dwGreenMask) * (255 - dwAlpha) / 255)
                    & LE_BLT_dwGreenMask;
                  dwAlphaData |=
                    ((dwDestinationData & LE_BLT_dwBlueMask) +
                    (dwSourceData & LE_BLT_dwBlueMask) * (255 - dwAlpha) / 255)
                    & LE_BLT_dwBlueMask;
#endif
                #else
                  // work out the final alpha effect -- dwAlphaData
                  dwAlphaData = 
                    ((dwDestinationData & LE_BLT_dwRedMask) * dwAlpha / 255 +
                    (dwSourceData & LE_BLT_dwRedMask) * (255 - dwAlpha) / 255)
                    & LE_BLT_dwRedMask;
                  dwAlphaData |=
                    ((dwDestinationData & LE_BLT_dwGreenMask) * dwAlpha / 255 +
                    (dwSourceData & LE_BLT_dwGreenMask) * (255 - dwAlpha) / 255)
                    & LE_BLT_dwGreenMask;
                  dwAlphaData |=
                    ((dwDestinationData & LE_BLT_dwBlueMask) * dwAlpha / 255 +
                    (dwSourceData & LE_BLT_dwBlueMask) * (255 - dwAlpha) / 255)
                    & LE_BLT_dwBlueMask;
                #endif
              #endif  // end of full 255 alpha levals

                // copy the resultant dwAlphaData to destination bitmap
                #if INC_DEST_DEPTH == 16
                  *((LPWORD)lpDestinationBltPointer) = (WORD)dwAlphaData;
                  lpDestinationBltPointer += 2;
                #elif INC_DEST_DEPTH == 24
                  *((LPWORD)lpDestinationBltPointer) = (WORD)dwAlphaData;
                  lpDestinationBltPointer += 2;
                  *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dwAlphaData >> 16);
                  lpDestinationBltPointer += 1;
                #elif INC_DEST_DEPTH == 32
                  *((LPLONG)lpDestinationBltPointer) = dwAlphaData;
                  lpDestinationBltPointer += 4;
                #endif
              }
            }
            else
            {
              // update the destination bitmap pointer
              #if INC_DEST_DEPTH == 16
                lpDestinationBltPointer += 2;
              #elif INC_DEST_DEPTH == 24
                lpDestinationBltPointer += 3;
              #elif INC_DEST_DEPTH == 32
                lpDestinationBltPointer += 4;
              #endif
            }

            // update the CurrentDestinationWidth
            CurrentDestinationWidth--;
          }

          // Move down to the next destination scan line.
        
          DestinationScanlineBltPtr += DestinationBitmapWidthInBytes;
          DestinationHeight--;
        }
      }
      return TRUE;
    #endif // from 8 bit colour depth to 16, 24, and 32 bit colour depths
  #endif // raw source bitmap
#endif // stretching and alpha

  // successful
  return TRUE;
}

// Cleanup - undo all the definitions we made in this file.

#undef COPY_OPTION
#undef COPY_OPTION_WITH_KEY
#undef STRETCH_OPTION
LateUndef FUNCTION_NAME
LateUndef NAME_TO_STRING
LateUndef NAME_AS_STRING
