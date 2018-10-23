/*****************************************************************************
 *
 * FILE:        L_BLT.H
 * DESCRIPTION: Bit block transfer (blit) image copying and manipulation
 *              routines and related low level graphics globals.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Blt.h 19    8/03/99 3:18p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Blt.h $
 * 
 * 19    8/03/99 3:18p Agmsmith
 * Go back to today's date to avoid lots of useless recompiles due to
 * makefile dependency with a future date.
 *
 * 18    4/01/:0 2:54p Agmsmith
 * Testing Year 2000 capability.
 *
 * 17    6/11/99 12:01p Fredds
 * Added headers for LE_BLTWARP_16to16, 24to24 and 32to32
 *
 * 16    5/27/99 6:13p Fredds
 * Added shift values for RGB components that indicate how many bits to
 * shift a given component until it's in the LSB.
 *
 * 15    5/12/99 4:22p Agmsmith
 * Typos fixed.
 *
 * 14    5/11/99 12:58p Fredds
 * Now includes LE_BLTWARP_8to24.
 *
 * 13    5/07/99 5:57p Fredds
 * Added LE_BLTWARP_8to16 and 8to32 to stretch and contort a bitmap.
 *
 * 12    3/29/99 3:18p Lzou
 * Have implemented bitmap stretch copy with alpha channel effects for
 * 16to16, 24to24, and 32to32 bit colour depths.
 *
 * 11    3/24/99 7:51p Timp
 * Removed Alpha parameter from LE_BLT_GetNearestColourAlpha.  Alpha
 * should be encoded in the high byte of ColourRef.  (See LEG_MCRA)
 *
 * 10    3/24/99 7:08p Agmsmith
 * Check in latest file after Li corrupts the database yet again.
 *
 * 11    3/24/99 5:48p Lzou
 * Have implemented bitmap unity copy with alpha channel effects for 16 to
 * 16 bit colour depth, 24 to 24 bit colour depth, and 32 to 32 bit colour
 * depth.
 *
 * 10    3/22/99 10:30a Timp
 * Added LE_BLT_GetNearestColourAlpha
 *
 * 9     1/16/99 4:39p Agmsmith
 *
 * 8     12/21/98 5:34p Agmsmith
 * Destination bytes per line should be 32 bits long.
 *
 * 7     12/21/98 5:23p Agmsmith
 * Added a destination palette to LE_BLT_CopySolidUnityRaw.
 *
 * 6     12/20/98 7:26p Agmsmith
 * Add a more generic bitmap copying function which can also copy
 * different depths and do clipping and flip upside down.
 *
 * 5     12/18/98 5:37p Agmsmith
 * Add L_BLT in front of global variables, dwRedShift is just too
 * anonymous.
 *
 * 4     9/22/98 1:20p Agmsmith
 * Moved blitter compatability stubs to L_Blt from L_Grafix.
 *
 * 3     9/21/98 6:11p Agmsmith
 * Export some support functions, add prototypes for hole copies.
 *
 * 2     9/17/98 4:18p Agmsmith
 * Added colour filling functions, compiles under C++ now.
 *
 * 1     9/14/98 12:34p Agmsmith
 * Initial version copied over from ArtLib98 and renamed from .C to .CPP.
 *
 * 8     8/14/98 3:12p Lzou
 * I am doing Holes & Stretching bitmap copy.
 *
 * 1     6/10/98 4:56p Agmsmith
 * New generic blitter functions, for all depths in a pair of files.
 ****************************************************************************/

#ifndef __L_BLT_H__
#define __L_BLT_H__


/*****************************************************************************
 *****************************************************************************
 ** T Y P E   D E F I N I T I O N S                                         **
 *****************************************************************************
 ****************************************************************************/


/*****************************************************************************
 *****************************************************************************
 ** G L O B A L   V A R I A B L E S                                         **
 *****************************************************************************
 ****************************************************************************/

extern short LE_BLT_nRedShift;   // number of first bit set in dwRedMask (1 - 32)
extern short LE_BLT_nGreenShift; // number of first bit set in dwGreenMask (1 - 32)
extern short LE_BLT_nBlueShift;  // number of first bit set in dwBlueMask (1 - 32)
extern DWORD LE_BLT_dwRedMask;   // mask of red bits for direct draw surfaces
extern DWORD LE_BLT_dwGreenMask; // mask of green bits for direct draw surfaces
extern DWORD LE_BLT_dwBlueMask;  // mask of blue bits for direct draw surfaces

// number ob bits to shift to get the repective component to the LSB
extern UNS8 LE_BLT_nRedShiftValue;
extern UNS8 LE_BLT_nGreenShiftValue;
extern UNS8 LE_BLT_nBlueShiftValue;

extern UNS8  LE_BLT_KeyRedLimit;
extern UNS8  LE_BLT_KeyGreenLimit;
extern UNS8  LE_BLT_KeyBlueLimit;
  // These control the limits used for defining a colour as transparent.  For
  // example, if we are looking for greenish, we look for red < redLimit,
  // green > greenLimit, blue < blueLimit.  CE_ARTLIB_BlitUseGreenColourKey
  // and CE_ARTLIB_BlitUseBlueColourKey define which general colour we are
  // looking for.  Note that the alpha blit still uses the old
  // pure green only code (there's too much stuff to fix), only the
  // ColorKeyPureGreenBitBlt series of functions has been modified to
  // use greenish etc.


extern BYTE *LI_BLT_BoundCheckScreenStart;
extern BYTE *LI_BLT_BoundCheckScreenPastEnd;
  // When doing boundary checking on bitmap copies, these identify the
  // screen or other non-Artech library memory area to the bound checker.
  // Otherwise, when drawing to the screen, it will complain that you
  // are drawing to unallocated memory.



/*****************************************************************************
 *****************************************************************************
 ** F U N C T I O N   P R O T O T Y P E S                                   **
 *****************************************************************************
 ****************************************************************************/

extern WORD  LI_BLT_ConvertColorRefTo16BitColor (COLORREF crColorToFillWith);
extern DWORD LI_BLT_ConvertColorRefTo24BitColor (COLORREF crColorToFillWith);
extern DWORD LI_BLT_ConvertColorRefTo32BitColor (COLORREF crColorToFillWith);
  // Uses the bit masks and shift values to convert a standard colour into the
  // pixel bits that will show that colour on the screen in the current display
  // mode.

extern void LI_BLT_ConvertRGBColorTableTo16BitColor (LPLONG lpRGBColorTable, int nColorTableEntries);
extern void LI_BLT_ConvertRGBColorTableTo24BitColor (LPLONG lpRGBColorTable, int nColorTableEntries);
extern void LI_BLT_ConvertRGBColorTableTo32BitColor (LPLONG lpRGBColorTable, int nColorTableEntries);
// Converts a palette from 24 bit entries into ones which match the current
// display mode (bits per pixel etc), so that the palette entries can be
// written directly to the display to get the desired colour pixel.

extern UNS16 LE_BLT_GetNearestColourAlpha(
  LE_BLT_AlphaPaletteEntryPointer Palette, UNS16 NumColours,
  COLORREF ColourRef);
// Given a palette and a colour (both with alpha), finds and returns
// the index of the closest matching colour in the palette.
// Colour 0 (ie: transparent) is not included in the search.


extern BOOL LE_BLT_CopySolidUnityRaw (
  LPBYTE SourceBitsPntr, UNS32 SourceBytesPerLine,
  UNS8 SourceBitsPerPixel, BOOL SourceHasBMPColours,
  UNS16 SourceWidth, UNS16 SourceHeight,
  UNS32 *SourcePalettePntr, UNS8 SourcePaletteEntryByteSize,
  LPBYTE DestinationBitsPntr, UNS32 DestinationBytesPerLine,
  UNS8 DestinationBitsPerPixel,
  UNS16 DestinationWidth, UNS16 DestinationHeight,
  UNS32 *DestinationPalettePntr, UNS8 DestinationPaletteEntryByteSize,
  INT16 CopyToDestX, INT16 CopyToDestY,
  UNS16 CopyToWidth, UNS16 CopyToHeight,
  BOOL FlipVertically);
// Copies the source bitmap to the given spot in the destination bitmap
// (with clipping if it doesn't fit) and converts the pixel formats as it
// goes.
//
// Source/destination Width and Height are the width and height of the
// whole bitmap in pixels.  This is important for clipping.
//
// BitsPerPixel is what it says.  The code currently supports values of 8,
// 16, 24 and 32.  Not all source and destination combinations are
// implemented.
//
// If SourceHasBMPColours is non-zero then the source colours are assumed
// to be in BMP format.  If in 24 bits then it has RGB pixels (blue then
// green then red bytes in increasing memory order).  If it is 8 bit then
// the palette has the same RGB colour interpretation.  The actual
// destination bitmasks are assumed to be the same as the screen's bit
// assignments (see LE_BLT_dwRedMask).  Non-BMP true colour images (this
// flag is FALSE) have their pixels already converted to screen format,
// including the palette.
//
// BytesPerLine is the scan line pitch - the number of bytes to add to a
// pixel address to go down one scan line in the bitmap to the next row of
// pixels and be at the same X position as the original pixel.  Usually
// this is the width in bytes rounded up to the next multiple of 4 (since
// this is a 32 bit processor which accesses things faster if they start at
// an address which is an even multiple of 4, and they add pad bytes to the
// end of each line to make this happen).
//
// The Palette is an array of 32 bit values (use a NULL pointer for images
// which aren't 8 bits deep), each one containing a direct pixel colour
// value (except when in BMP format).  This value (low 16 bits in 16 bit
// mode, low 24 in 24, all 32 in 32) is written directly to the destination
// pixel, without any conversions.  Since this is a solid copy operation,
// the Alpha (translucency level) is ignored (unless copying to a 32 bit
// bitmap).
//
// SourcePaletteEntryByteSize is the number of bytes to add to advance to
// the next colour entry in the palette.  It is usually 4, since there are
// 4 bytes in a colour entry.  However, some palettes have alpha data words
// alternating with the colours, so this would be 8 for that kind of
// palette.
//
// CopyToDestX and CopyToDestY specify where the top left corner of the
// source image ends up in the destination bitmap (or bottom left if
// flipping).
//
// CopyToWidth and CopyToHeight put a limit to the size of the copied data.
// If you want to copy the whole source bitmap then just make these as large
// as possible ((UNS16) -1) and ignore them.  Otherwise, they move in the
// right and bottom edges to get the size you want (no stretching).
//
// FlipVertically is used to flip the bitmap upside down as it copies
// (useful for BMP formats).
//
// Returns TRUE if successful.


//
// ==================================================================
//

// Solid - Copy with no transparency tests at all.
// Holes - Copy with colour index zero being transparent, rest solid.
// Alpha - Copy with blended transparency, either from palette or as a number.
// ColourKey - Copy with greenish or some other colour being transparent.
// General - Uses the bitmap header to figure it out.
// Raw - raw bitmap with separate palette, width and height.
// Unity - Source and destination pixels are copied at a 1 to 1 ratio, no stretch.
// Stretch - Source and destination can be different sizes.


//
// ==================================================================
//
//      Unity & Solid, bitmap copy, Raw
//

extern BOOL LI_BLT_Copy8To16SolidUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy8To24SolidUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy8To32SolidUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy16To16SolidUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy24To24SolidUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy32To32SolidUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

//
// ==================================================================
//
//      Stretching & Solid, bitmap copy, Raw
//

extern BOOL LI_BLT_Copy8To16SolidStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy8To24SolidStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy8To32SolidStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy16To16SolidStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy24To24SolidStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy32To32SolidStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

//
//
// ==================================================================
//
//      Unity & Solid with Colour Key, bitmap copy, Raw
//

extern BOOL LI_BLT_Copy8To16SolidColourKeyUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy8To24SolidColourKeyUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy8To32SolidColourKeyUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy16To16SolidColourKeyUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy24To24SolidColourKeyUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy32To32SolidColourKeyUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

//
// ==================================================================
//
//      Stretching & Solid with Colour Key, bitmap copy, Raw
//

extern BOOL LI_BLT_Copy8To16SolidColourKeyStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy8To24SolidColourKeyStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy8To32SolidColourKeyStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy16To16SolidColourKeyStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy24To24SolidColourKeyStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy32To32SolidColourKeyStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

//
// ==================================================================
//
//      Alpha & Unity, bitmap copy, Raw
//

extern BOOL LI_BLT_Copy8To16AlphaUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  UNS16 NumberOfAlphaColors,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy8To24AlphaUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  UNS16 NumberOfAlphaColors,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy8To32AlphaUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  UNS16 NumberOfAlphaColors,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

// The function works kind of alpha channel + pure green colour key.
extern BOOL LI_BLT_Copy16To16AlphaUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  INT16 nAlphaValue,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

// The function works kind of alpha channel + pure green colour key.
extern BOOL LI_BLT_Copy24To24AlphaUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  INT16 nAlphaValue,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

// The function works kind of alpha channel + pure green colour key.
extern BOOL LI_BLT_Copy32To32AlphaUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  INT16 nAlphaValue,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

//
// ==================================================================
//
//      Alpha & Stretching, bitmap copy, Raw
//

extern BOOL LI_BLT_Copy8To16AlphaStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  UNS16 NumberOfAlphaColors,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy8To24AlphaStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  UNS16 NumberOfAlphaColors,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy8To32AlphaStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  UNS16 NumberOfAlphaColors,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

// The function works kind of alpha channel + pure green colour key.
extern BOOL LI_BLT_Copy16To16AlphaStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  INT16 nAlphaValue,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

// The function works kind of alpha channel + pure green colour key.
extern BOOL LI_BLT_Copy24To24AlphaStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  INT16 nAlphaValue,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

// The function works kind of alpha channel + pure green colour key.
extern BOOL LI_BLT_Copy32To32AlphaStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  INT16 nAlphaValue,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

//
// ==================================================================
//
//      Unity & Holes, bitmap copy
//

extern BOOL LI_BLT_Copy8To16HolesUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColorTable,
  LPRECT SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  LPRECT DestInvalidRectangle,
  LPRECT DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy8To24HolesUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColorTable,
  LPRECT SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  LPRECT DestInvalidRectangle,
  LPRECT DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);

extern BOOL LI_BLT_Copy8To32HolesUnityRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColorTable,
  LPRECT SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  LPRECT DestInvalidRectangle,
  LPRECT DestBoundingRectangle,
  INT16 DestinationX,
  INT16 DestinationY);


//
// ==================================================================
//
//      Stretching & Holes, bitmap copy, Raw
//

extern BOOL LI_BLT_Copy8To16HolesStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy8To24HolesStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

extern BOOL LI_BLT_Copy8To32HolesStretchRaw (
  UNS8Pointer SourceBitmapBits,
  UNS16 SourceBitmapWidth,
  UNS16 SourceBitmapHeight,
  LPLONG SourceColourTable,
  TYPE_RectPointer SourceRectangle,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestInvalidRectangle,
  TYPE_RectPointer DestBoundingRectangle);

//
// =============================================================
//
//          Solid Colour Filling
//

extern BOOL LI_BLT_Fill16Raw (
  DWORD ColourToFillWith,
  BOOL  ColourIsNative,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestRectangle);

extern BOOL LI_BLT_Fill24Raw (
  DWORD ColourToFillWith,
  BOOL  ColourIsNative,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestRectangle);

extern BOOL LI_BLT_Fill32Raw (
  DWORD ColourToFillWith,
  BOOL  ColourIsNative,
  UNS8Pointer DestinationBitmapBits,
  UNS16 DestinationBitmapWidth,
  UNS16 DestinationBitmapHeight,
  TYPE_RectPointer DestRectangle);



/****************************************************************************/
/* C O M P A T A B I L I T Y   S T U B   F U N C T I O N S                  */
/*--------------------------------------------------------------------------*/

void LI_BLT16BitBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable);

void LI_BLT24BitBlt8to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable);

void LI_BLT32BitBlt8to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable);

void LI_BLT16ColorKeyBitBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable);

void LI_BLT24ColorKeyBitBlt8to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable);

void LI_BLT32ColorKeyBitBlt8to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable);

void LI_BLT16AlphaBitBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable,
  int nAlphaInColorTable);

void LI_BLT24AlphaBitBlt8to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable,
  int nAlphaInColorTable);

void LI_BLT32AlphaBitBlt8to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable,
  int nAlphaInColorTable);

void LI_BLT16ConvertRGBValuesTo16BitColor(
  LPBYTE lp24DestinationBitData,
  LPBYTE lp16SourceBitData,
    int src_w_pixels,
    int src_h_pixels,
    int dest_w_pixels,
    int dest_h_pixels);

void LI_BLT24ConvertRGBValuesTo24BitColor(
  LPBYTE lp24DestinationBitData,
  LPBYTE lp24SourceBitData,
    int src_w_pixels,
    int src_h_pixels,
    int dest_w_pixels,
    int dest_h_pixels);

void LI_BLT32ConvertRGBValuesTo32BitColor(
  LPBYTE lp32DestinationBitData,
  LPBYTE lp24SourceBitData,
    int src_w_pixels,
    int src_h_pixels,
    int dest_w_pixels,
    int dest_h_pixels);

void LI_BLT16BitBlt16to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect);

void LI_BLT24BitBlt24to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect);

void LI_BLT32BitBlt32to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect);

void LI_BLT16ColorKeyPureGreenBitBlt16to16(
  LPBYTE  lpDestinationBits,
  int     nXDestinationWidthInPixels,
  int     nXBltStartCoordinate,
  int     nYBltStartCoordinate,
  LPBYTE  lpSourceBits,
  int     nXSourceWidthInPixels,
  LPRECT  lprSourceBltRect);

void LI_BLT24ColorKeyPureGreenBitBlt24to24(
  LPBYTE  lpDestinationBits,
  int     nXDestinationWidthInPixels,
  int     nXBltStartCoordinate,
  int     nYBltStartCoordinate,
  LPBYTE  lpSourceBits,
  int     nXSourceWidthInPixels,
  LPRECT  lprSourceBltRect);

void LI_BLT32ColorKeyPureGreenBitBlt32to32(
  LPBYTE  lpDestinationBits,
  int     nXDestinationWidthInPixels,
  int     nXBltStartCoordinate,
  int     nYBltStartCoordinate,
  LPBYTE  lpSourceBits,
  int     nXSourceWidthInPixels,
  LPRECT  lprSourceBltRect);

void LI_BLT16AlphaBitBlt16to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue);

void LI_BLT24AlphaBitBlt24to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue);

void LI_BLT32AlphaBitBlt32to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue);

/*--------------------------------------------------------------------------*/
/* END OF C O M P A T A B I L I T Y   S T U B   F U N C T I O N S           */
/****************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_8to16
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intents and purposes this is an affine texture mapper.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
//
// NOTE: That the corners of the source image will always correspond with these 4 points.
// i.e: (X0,Y0) will reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height)
// and so on, counter-clockwise around the bitmap.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void LE_BLTWARP_8to16
(
 void *Src_Pixel_Pntr,
 UNS16 Src_Pitch,
 UNS16 Src_Width,
 UNS16 Src_Height,
 LE_BLT_AlphaPaletteEntryRecord *Src_Palette,
 UNS16 Num_Colors,
 UNS16 Num_Alpha,
 void *Dest_Pixel_Pntr,
 UNS16 Dest_Pitch,
 UNS16 Dest_Width,
 UNS16 Dest_Height,
 INT32 X0,INT32 Y0,
 INT32 X1,INT32 Y1,
 INT32 X2,INT32 Y2,
 INT32 X3,INT32 Y3
);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_8to24
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intents and purposes this is an affine texture mapper.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
// NOTE: That the corners of the source image will always correspond with these 4 points.
// i.e: (X0,Y0) will reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height)
// and so on, counter-clockwise around the bitmap.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void LE_BLTWARP_8to24
(
 void *Src_Pixel_Pntr,
 UNS16 Src_Pitch,
 UNS16 Src_Width,
 UNS16 Src_Height,

 LE_BLT_AlphaPaletteEntryRecord *Src_Palette,
 UNS16 Num_Colors,
 UNS16 Num_Alpha,

 void *Dest_Pixel_Pntr,
 UNS16 Dest_Pitch,
 UNS16 Dest_Width,
 UNS16 Dest_Height,

 INT32 X0,INT32 Y0,
 INT32 X1,INT32 Y1,
 INT32 X2,INT32 Y2,
 INT32 X3,INT32 Y3
);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_8to32
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intents and purposes this is an affine texture mapper.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
// NOTE: That the corners of the source image will always correspond with these 4 points.
// i.e: (X0,Y0) will reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height)
// and so on, counter-clockwise around the bitmap.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void LE_BLTWARP_8to32
(
 void *Src_Pixel_Pntr,
 UNS16 Src_Pitch,
 UNS16 Src_Width,
 UNS16 Src_Height,

 LE_BLT_AlphaPaletteEntryRecord *Src_Palette,
 UNS16 Num_Colors,
 UNS16 Num_Alpha,

 void *Dest_Pixel_Pntr,
 UNS16 Dest_Pitch,
 UNS16 Dest_Width,
 UNS16 Dest_Height,

 INT32 X0,INT32 Y0,
 INT32 X1,INT32 Y1,
 INT32 X2,INT32 Y2,
 INT32 X3,INT32 Y3
);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_32to32
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intents and purposes this is an affine texture mapper.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
// NOTE: That the corners of the source image will always correspond with these 4 points.
// i.e: (X0,Y0) will reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height)
// and so on, counter-clockwise around the bitmap.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void LE_BLTWARP_32to32
(
 void *Src_Pixel_Pntr,
 UNS16 Src_Pitch,
 UNS16 Src_Width,
 UNS16 Src_Height,

 void *Dest_Pixel_Pntr,
 UNS16 Dest_Pitch,
 UNS16 Dest_Width,
 UNS16 Dest_Height,

 INT32 X0,INT32 Y0,
 INT32 X1,INT32 Y1,
 INT32 X2,INT32 Y2,
 INT32 X3,INT32 Y3
);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_16to16
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intents and purposes this is an affine texture mapper.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
// NOTE: That the corners of the source image will always correspond with these 4 points.
// i.e: (X0,Y0) will reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height)
// and so on, counter-clockwise around the bitmap.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void LE_BLTWARP_16to16
(
 void *Src_Pixel_Pntr,
 UNS16 Src_Pitch,
 UNS16 Src_Width,
 UNS16 Src_Height,

 void *Dest_Pixel_Pntr,
 UNS16 Dest_Pitch,
 UNS16 Dest_Width,
 UNS16 Dest_Height,

 INT32 X0,INT32 Y0,
 INT32 X1,INT32 Y1,
 INT32 X2,INT32 Y2,
 INT32 X3,INT32 Y3
);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE_BLTWARP_24to24
//
// DESCRIPTION: Warps and stretches the source bitmap onto a destination buffer bounded by
// the four vertex points.  For all intents and purposes this is an affine texture mapper.
//
// ARGUMENTS:
// Src_Pixel_Pntr -- The pointer to the bitmap image data to stretch.
// Src_Pitch -- The width of the data padded to a multiple of 4 bytes
// Src_Width -- The width of the data in pixels
// Src_Height -- The height of the data in pixels
//
// ==== for 8-bit Src's only =========================
// Src_Pallete -- The 8-bit pallete for the src
// Num_Colors -- Number of colors in the pallete
// Num_Alpha -- Number of alpha values in the pallete
// ============================================
//
// Dest_Pixel_Pntr -- A pointer to the destination surface to plot the output to.
// Dest_Pitch -- The width of the destination padded to a multiple of 4 bytes
// Dest_Width -- The width of the destination
// Dest_Height -- The height of the destination
//
// X0,Y0,X1,Y1,X2,Y2,X2,Y3 -- The 4 bounding vertices to stretch the image to.
// NOTE: That the corners of the source image will always correspond with these 4 points.
// i.e: (X0,Y0) will reference pixel (0,0) in the bitmap. (X1,Y1) will reference pixel (0,Src_Height)
// and so on, counter-clockwise around the bitmap.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void LE_BLTWARP_24to24
(
 void *Src_Pixel_Pntr,
 UNS16 Src_Pitch,
 UNS16 Src_Width,
 UNS16 Src_Height,

 void *Dest_Pixel_Pntr,
 UNS16 Dest_Pitch,
 UNS16 Dest_Width,
 UNS16 Dest_Height,

 INT32 X0,INT32 Y0,
 INT32 X1,INT32 Y1,
 INT32 X2,INT32 Y2,
 INT32 X3,INT32 Y3
);

#endif /* __L_BLT_H__ */