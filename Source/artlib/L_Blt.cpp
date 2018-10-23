/*****************************************************************************
 *
 * FILE:        L_BLT.CPP
 * DESCRIPTION: Bit block transfer (blit) image copying and manipulation
 *              routines and related low level graphics globals.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Blt.cpp 10    8/17/99 12:00 Davew $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Blt.cpp $
 * 
 * 10    8/17/99 12:00 Davew
 * Fixed a warning.
 * 
 * 9     7/23/99 1:17p Lzou
 * Nothing important.
 * 
 * 8     7/23/99 1:07p Lzou
 * Added blt (CopyUnityRaw) 16to24 and 32to24 bit colour depth. Now, while
 * working in 16, 24, and 32 bit screen colour depth, you can save your
 * working bitmap in a 24 bit bitmap file.
 * 
 * 7     7/22/99 1:37p Lzou
 * Added blt (CopyUnityRaw) from 16 to 24 bit colour depth.
 * 
 * 6     5/27/99 6:13p Fredds
 * Added shift values for RGB components that indicate how many bits to
 * shift a given component until it's in the LSB.
 * 
 * 5     3/26/99 5:21p Lzou
 * Have made several blt functions backward compatible.
 * 
 * 4     3/24/99 7:51p Timp
 * Removed Alpha parameter from LE_BLT_GetNearestColourAlpha.  Alpha
 * should be encoded in the high byte of ColourRef.  (See LEG_MCRA)
 * 
 * 3     3/24/99 7:08p Agmsmith
 * Check in latest file after Li corrupts the database yet again.
 * 
 * 4     3/24/99 3:47p Lzou
 * Have implemented bitmap unity copy with alpha channel effects for 16 to
 * 16 bit colour depth, 24 to 24 bit colour depth, and 32 to 32 colour
 * depth.
 * 
 * 3     3/22/99 10:30a Timp
 * Added LE_BLT_GetNearestColourAlpha
 * 
 * 2     1/22/99 4:08p Agmsmith
 * Fix the glue again.
 * 
 * 1     1/22/99 3:44p Agmsmith
 * Replace damaged SourceSafe history.
 * 
 * 17    1/22/99 1:25p Lzou
 * Now, we do have 8 to 16 bit unity bitmap copy with holes.
 * 
 * 16    1/16/99 4:39p Agmsmith
 * Changing blitter routines to also use old routines if desired.
 * 
 * 15    1/11/99 4:28p Agmsmith
 * Apply same changes to 24 and 32 bit code.
 * 
 * 14    1/11/99 3:23p Timp
 * Fixed LI_BLT16ColorKeyBitBlt8to16 to not stretch/position destination
 * incorrectly
 * 
 * 13    12/24/98 3:07p Agmsmith
 * Use the rotate instruction so that we can shift bits left or right
 * (negative shifts work).
 * 
 * 12    12/21/98 5:34p Agmsmith
 * Destination bytes per line should be 32 bits long.
 *
 * 11    12/21/98 5:23p Agmsmith
 * Added a destination palette to LE_BLT_CopySolidUnityRaw.
 *
 * 10    12/21/98 4:30p Agmsmith
 * Now handles 24 bit to 16, 24, 32 bits.  Not super efficient (someone
 * should implement bulk conversions), but not as bad as previously.
 *
 * 9     12/20/98 7:26p Agmsmith
 * Add a more generic bitmap copying function which can also copy
 * different depths and do clipping and flip upside down.
 *
 * 8     12/18/98 5:37p Agmsmith
 * Add L_BLT in front of global variables, dwRedShift is just too
 * anonymous.
 *
 * 7     11/05/98 10:44a Lzou
 * Stretching mode with holes.
 *
 * 6     11/05/98 10:29a Lzou
 * As LI_BLT_Copy8To16HolesUnityRaw () is not implemented yet, use
 * LI_BLT_Copy8To16HolesStretchRaw () instead in
 * LI_BLT16ColorKeyBitBlt8to16 ().
 *
 * 5     11/02/98 5:24p Agmsmith
 * Added back the ConvertRGB to native series of functions.
 *
 * 4     9/22/98 1:20p Agmsmith
 * Moved blitter compatability stubs to L_Blt from L_Grafix.
 *
 * 3     9/21/98 6:11p Agmsmith
 * Use Artech library error message system.
 *
 * 2     9/17/98 4:18p Agmsmith
 * Added colour filling functions, compiles under C++ now.
 *
 * 5     8/10/98 4:33p Lzou
 * Have modified function argument list to include DestInvalidRectangle
 * and DestBoundingRectangle in a bid to make blt faster.
 *
 * 1     6/10/98 4:56p Agmsmith
 * New generic blitter functions, for all depths in a pair of files.
 ****************************************************************************/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemGrafix


/*****************************************************************************
 *****************************************************************************
 ** M A C R O   D E F I N I T I O N S                                       **
 *****************************************************************************
 ****************************************************************************/



/*****************************************************************************
 *****************************************************************************
 ** G L O B A L   V A R I A B L E S                                         **
 *****************************************************************************
 ****************************************************************************/

short LE_BLT_nRedShift;    // number of first bit set in LE_BLT_dwRedMask (1 - 32)
short LE_BLT_nGreenShift;  // number of first bit set in LE_BLT_dwGreenMask (1 - 32)
short LE_BLT_nBlueShift;   // number of first bit set in LE_BLT_dwBlueMask (1 - 32)
DWORD LE_BLT_dwRedMask;    // mask of red bits for direct draw surfaces
DWORD LE_BLT_dwGreenMask;  // mask of green bits for direct draw surfaces
DWORD LE_BLT_dwBlueMask;   // mask of blue bits for direct draw surfaces

// number of bits to shift to get the repective component to the LSB 
UNS8 LE_BLT_nRedShiftValue;   
UNS8 LE_BLT_nGreenShiftValue; 
UNS8 LE_BLT_nBlueShiftValue;  


UNS8  LE_BLT_KeyRedLimit = 128;
UNS8  LE_BLT_KeyGreenLimit = 128;
UNS8  LE_BLT_KeyBlueLimit = 128;
  // These control the limits used for defining a colour as transparent.  For
  // example, if we are looking for greenish, we look for red < redLimit,
  // green > greenLimit, blue < blueLimit.  CE_ARTLIB_BlitUseGreenColourKey
  // and CE_ARTLIB_BlitUseBlueColourKey define which general colour we are
  // looking for.  Note that the alpha blit still uses the old
  // pure green only code (there's too much stuff to fix), only the
  // ColorKeyPureGreenBitBlt series of functions has been modified to
  // use greenish etc.


BYTE *LI_BLT_BoundCheckScreenStart;
BYTE *LI_BLT_BoundCheckScreenPastEnd;
  // When doing boundary checking on bitmap copies, these identify the
  // screen or other non-Artech library memory area to the bound checker.
  // Otherwise, when drawing to the screen, it will complain that you
  // are drawing to unallocated memory.



/*****************************************************************************
 *****************************************************************************
 ** S T A T I C   V A R I A B L E S                                         **
 *****************************************************************************
 ****************************************************************************/

// Here comes the (private global) stretch table definition.

#define MAXSTRETCHDESTINATIONSIZE 2048

typedef struct StretchTableStruct
{
   short destinationStartOffset;
      /* The coordinate where drawing will start in the destination
      image.  Will be 0 or greater.  Computed from your specified
      destination limits modified by clipping. */

   short destinationStretchSize;
      /* The width/height of the stretched image in the destination image,
      in pixel units.  May be reduced from the user specified width
      due to clipping against the source or destination image sides.
      Set to zero if the image gets completely clipped off.  This is
      also the size of the pixelOffsets array that contains data. */

   short sourceStartOffset;
      /* Specifies the number of pixels to skip in the source image to get
      to the first pixel that will be drawn. */

   short sourceStretchSize;
      /* The number of source pixels that are in the area being stretched.
      This value is only approximate, it may be incorrect if clipping
      was done.  So, don't use it unless you fix up the calculations. */


   unsigned char pixelOffsets [MAXSTRETCHDESTINATIONSIZE];
      /* Before drawing the i'th output pixel, the value in this array
      is added to the previous source pixel index (X or Y coordinate as
      appropriate) to get the source pixel for the ith destination pixel.
      The very first source pixel is at sourceStartOffset plus
      pixelOffsets[0].  Normally pixelOffsets[0] will be zero, a bit of
      a waste, but we want to keep the offsets byte sized thus the special
      longer offset for the starting pixel.  The byte size also means
      that funny things will happen if you scale down a really wide
      source image into a really small destination image.  But then you
      won't notice it since the destination will be so small. */

} StretchTableRecord, *StretchTablePointer;
   /* This table of values is used for speeding up the stretching of
   bitmaps by precalculating the source pixel offset for every
   destination pixel.  Since it is a large table, it probably shouldn't
   be allocated on the stack. */


//static StretchTableRecord XStretchTable;

//static StretchTableRecord YStretchTable;
     /* Our private stretch tables for use in bitmap stretching functions.
     Allocated here as static module globals so that they don't take up
     space on the stack and so they can be far in the PC version (since
     they take quite a few bytes to store). */


/******************************************************************************
*******************************************************************************
**   F U N C T I O N   P R O T O T Y P E                                     **
*******************************************************************************
*******************************************************************************/
void CalculateStretchTable(  // one dimension each time
  StretchTablePointer StretchTablePntr,
  int SourceImageSize, int SourceStartOffset, int SourceStretchSize,
  int DestinationImageSize, int DestinationStartOffset,
  int DestinationStretchSize);

BOOL CopyToBitmap(  // no stretch, bitmaps must be of the same color depth
  UNS8Pointer SourceBitmap, int SourceX, int SourceY,
  int SourceBitmapWidth, int SourceBitmapHeight,
  UNS8Pointer DestinationBitmap, int DestinationX, int DestinationY,
  int DestinationBitmapWidth, int DestinationBitmapHeight,
  int CopyWidth, int CopyHeight, short BitmapColorDepth);

BOOL CopyToBitmapStretch( // stretch mapping
  UNS8Pointer SourceBitmap, int SourceX, int SourceY,
  int SourceWidth, int SourceHeight,
  int SourceBitmapWidth, int SourceBitmapHeight,
  UNS8Pointer DestinationBitmap, int DestinationX, int DestinationY,
  int DestinationWidth, int DestinationHeight,
  int DestinationBitmapWidth, int DestinationBitmapHeight,
  short BitmapColorDepth);

UNS8Pointer GetBitmapBeginBltPtr(UNS8Pointer lpBitmapBits, int nStartX, int nStartY,
  int nXBitampWidthInPixels, short nColorDepth);

int ReturnBitmapWidthInBytes(int nXBitampWidthInPixels, short nColorDepth);



/*****************************************************************************
 *****************************************************************************
 ** S U P P O R T   F U N C T I O N S                                       **
 *****************************************************************************
 ****************************************************************************/

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
WORD LI_BLT_ConvertColorRefTo16BitColor(COLORREF crColorToFillWith)
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
void LI_BLT_ConvertRGBColorTableTo16BitColor(
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
*   Name:         LI_BLT24ConvertColorRefTo24BitColor
*
*   Description:  converts a COLORREF to a 24 bit color
*
*   Arguments:    crColorToFillWith - COLORREF to convert to 24 bit color
*
*   Globals:      LE_BLT_nRedShift - number of first bit set in LE_BLT_dwRedMask (1 to 24)
*                 LE_BLT_nGreenShift -  number of first bit set in LE_BLT_dwGreenMask (1 - 24)
*                 LE_BLT_nBlueShift - number of first bit set in LE_BLT_dwBlueMask (1 - 24)
*                 LE_BLT_dwRedMask - mask of red bits for 24 bit DIBs
*                 LE_BLT_dwGreenMask - mask of green bits for 24 bit DIBs
*                 LE_BLT_dwBlueMask - mask of blue bits for 24 bit DIBs
*
*   Returns:      24 bit color
*
*******************************************************************************/
DWORD LI_BLT_ConvertColorRefTo24BitColor(
  COLORREF crColorToFillWith)
{
  DWORD dw24BitColor;   // converted 24 bit color (RETURNED)
  short nShift24;       // how many bits to shift values from COLORREF

  // shift and mask red bit values from COLORREF
  nShift24 = LE_BLT_nRedShift - 8;
  if (nShift24 >= 0)
    dw24BitColor = ((crColorToFillWith & 0xff) << nShift24) & LE_BLT_dwRedMask;
  else
    dw24BitColor = ((crColorToFillWith & 0xff) >> -nShift24) & LE_BLT_dwRedMask;

  // shift and mask green bit values from COLORREF
  nShift24 = LE_BLT_nGreenShift - 16;
  if (nShift24 >= 0)
    dw24BitColor |= ((crColorToFillWith & 0xff00) << nShift24) & LE_BLT_dwGreenMask;
  else
    dw24BitColor |= ((crColorToFillWith & 0xff00) >> -nShift24) & LE_BLT_dwGreenMask;

  // shift and mask blue bit values from COLORREF
  nShift24 = LE_BLT_nBlueShift - 24;
  if (nShift24 >= 0)
    dw24BitColor |= ((crColorToFillWith & 0xff0000) << nShift24) & LE_BLT_dwBlueMask;
  else
    dw24BitColor |= ((crColorToFillWith & 0xff0000) >> -nShift24) & LE_BLT_dwBlueMask;

  // return converted 24 bit color
  return(dw24BitColor);
}


/*******************************************************************************
*
*   Name:         LI_BLT24ConvertRGBColorTableTo24BitColor
*
*   Description:  converts an RGBQUAD color table to 24 bit colors
*
*   Arguments:    lpRGBColorTable - pointer to RGBQUAD color table
*                 nColorTableEntries - number of entries in color table
*
*   Globals:      LE_BLT_nRedShift - number of first bit set in LE_BLT_dwRedMask (1 to 24)
*                 LE_BLT_nGreenShift -  number of first bit set in LE_BLT_dwGreenMask (1 - 24)
*                 LE_BLT_nBlueShift - number of first bit set in LE_BLT_dwBlueMask (1 - 24)
*                 LE_BLT_dwRedMask - mask of red bits for 24 bit DIBs
*                 LE_BLT_dwGreenMask - mask of green bits for 24 bit DIBs
*                 LE_BLT_dwBlueMask - mask of blue bits for 24 bit DIBs
*
*   Returns:      none
*
*******************************************************************************/
void LI_BLT_ConvertRGBColorTableTo24BitColor(
  LPLONG lpRGBColorTable,
  int nColorTableEntries)
{
  int nColorTableIndex;       // color table index
  DWORD dw24BitColor;         // 24 bit color value
  DWORD dwRGBValue;           // RGBQUAD value
  short nShift24;             // how many bits to shift values from RGBQUAD

  // store 24 bit table in same location as RGBQUAD table
  for (nColorTableIndex = 0; nColorTableIndex < nColorTableEntries; nColorTableIndex ++)
  {
    // get RGBQUAD value as a DWORD
    dwRGBValue = *lpRGBColorTable;

    // shift and mask blue bit values from RGBQUAD
    nShift24 = LE_BLT_nBlueShift - 8;
    if (nShift24 >= 0)
      dw24BitColor = ((dwRGBValue & 0xff) << nShift24) & LE_BLT_dwBlueMask;
    else
      dw24BitColor = ((dwRGBValue & 0xff) >> -nShift24) & LE_BLT_dwBlueMask;

    // shift and mask green bit values from RGBQUAD
    nShift24 = LE_BLT_nGreenShift - 16;
    if (nShift24 >= 0)
      dw24BitColor |= ((dwRGBValue & 0xff00) << nShift24) & LE_BLT_dwGreenMask;
    else
      dw24BitColor |= ((dwRGBValue & 0xff00) >> -nShift24) & LE_BLT_dwGreenMask;

    // shift and mask red bit values from RGBQUAD
    nShift24 = LE_BLT_nRedShift - 24;
    if (nShift24 >= 0)
      dw24BitColor |= ((dwRGBValue & 0xff0000) << nShift24) & LE_BLT_dwRedMask;
    else
      dw24BitColor |= ((dwRGBValue & 0xff0000) >> -nShift24) & LE_BLT_dwRedMask;

    // store 24 bit color value
    *lpRGBColorTable = dw24BitColor;
    // increment color table pointer
    lpRGBColorTable ++;
    lpRGBColorTable ++;
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
DWORD LI_BLT_ConvertColorRefTo32BitColor(
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
void LI_BLT_ConvertRGBColorTableTo32BitColor(
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
* Name:        LE_BLT_GetNearestColourAlpha
*
* Description: Given a palette and a colour (both with alpha), finds and returns
*              the index of the closest matching colour in the palette.
*              Colour 0 (ie: transparent) is not included in the search.
*
* Arguments:   Palette    - Pointer to LE_BLT_AlphaPaletteEntryRecord table
*              NumColours - Number of colours in table
*              ColourRef  - Colour to look for in table
*              (Alpha is encoded in the high byte - see LEG_MCRA)
*
* Globals:     LE_BLT_nRedShift   - Num of first bit set in LE_BLT_dwRedMask
*              LE_BLT_nGreenShift - Num of first bit set in LE_BLT_dwGreenMask
*              LE_BLT_nBlueShift  - Num of first bit set in LE_BLT_dwBlueMask
*              LE_BLT_dwRedMask   - Mask of red bits
*              LE_BLT_dwGreenMask - Mask of green bits
*              LE_BLT_dwBlueMask  - Mask of blue bits
*
* Returns:     Index of closest colour in palette.  0 if none found.
*              (If an empty palette was passed in, for example)
*******************************************************************************/
UNS16 LE_BLT_GetNearestColourAlpha(
  LE_BLT_AlphaPaletteEntryPointer Palette,
  UNS16 NumColours,
  COLORREF ColourRef)
{
  UNS16 i, ColourIndex = 0;
  DWORD Colour, Alpha;
  UNS32 distance, mindistance = UNS32_MAX;
  long r1, r2, g1, g2, b1, b2, a1, a2;
  // Get minimum shift value. (See which is rightmost of r/g/b)
  short minShift = min(LE_BLT_nRedShift,
    min(LE_BLT_nGreenShift, LE_BLT_nBlueShift));
  short redShift = LE_BLT_nRedShift - minShift,
        greenShift = LE_BLT_nGreenShift - minShift,
        blueShift = LE_BLT_nBlueShift - minShift,
        alphaShift = 8 - minShift;

  // Get Alpha from ColourRef and then remove it.
  Alpha = (ColourRef & 0xff000000) >> 24;
  ColourRef &= 0x00ffffff;

  // Convert COLORREF to internal colour representation
  if (LE_GRAFIX_ScreenBitsPerPixel == 16)
    Colour = LI_BLT_ConvertColorRefTo16BitColor(ColourRef);
  else if (LE_GRAFIX_ScreenBitsPerPixel == 24)
    Colour = LI_BLT_ConvertColorRefTo24BitColor(ColourRef);
  else if (LE_GRAFIX_ScreenBitsPerPixel == 32)
    Colour = LI_BLT_ConvertColorRefTo32BitColor(ColourRef);

  // Get separate RGBA components of colour to look for
  r1 = (Colour & LE_BLT_dwRedMask) >> redShift;
  g1 = (Colour & LE_BLT_dwGreenMask) >> greenShift;
  b1 = (Colour & LE_BLT_dwBlueMask) >> blueShift;
  a1 = Alpha >> alphaShift;

  for (i = 1; i < NumColours; i++)
  {
    // Get separate RGBA components of colour to compare with
    r2 = (Palette[i].lowDWORD.dwColour & LE_BLT_dwRedMask) >> redShift;
    g2 = (Palette[i].lowDWORD.dwColour & LE_BLT_dwGreenMask) >> greenShift;
    b2 = (Palette[i].lowDWORD.dwColour & LE_BLT_dwBlueMask) >> blueShift;
    a2 = Palette[i].dwAlpha >> alphaShift;

    // Square the distances, and apply luminance factors to RGB.
    // Give alpha a factor too so it has a proper effect on the result.
    distance = 299 * (r1 - r2) * (r1 - r2) +
               587 * (g1 - g2) * (g1 - g2) +
               114 * (b1 - b2) * (b1 - b2) +
               333 * (a1 - a2) * (a1 - a2);

    // If we have a new winner, choose him
    if (distance < mindistance)
    {
      mindistance = distance;
      ColourIndex = i;
    }
  }
  return(ColourIndex);
}



/******************************************************************************
 * This is a utility function for the other bitmap stretching functions that
 * precalulates the source pixel needed for every destination pixel
 * when given source and destination sizes.
 */

static void CalculateStretchTable (StretchTablePointer StretchTablePntr,
  int SourceImageSize, int SourceStartOffset, int SourceStretchSize,
  int DestinationImageSize, int DestinationStartOffset,
  int DestinationStretchSize)
{
  long                 DDADifference;
  long                 DDAInitialValue;
  long                 DDASourceIncrement;
  long                 DDADestinationDecrement;
  int                  Excess;
  short                OffsetCount;
  unsigned char       *OffsetPntr;
  int                  RemainingDestinationStretchSize;
  int                  RemainingSourceStretchSize = 0;
  int                  X;

  if (StretchTablePntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_ErrorMsg ("CalculateStretchTable: Null arguments");
#endif
    return;
  }

  /* Set up the DDA (Digital difference analyser) which keeps track of
  the relative proportions of source and destination positions along
  the dimension being stretched.  Every time SourceStartOffset is
  increased by 1 pixel, the difference value has DDASourceIncrement
  added to it.  Similarly when DestinationStartOffset is incremented,
  DDADestinationDecrement is subtracted.  To keep the source and
  destination aligned, the difference should be kept between zero and
  DDASourceIncrement - 1.

  Point (SSO=0, DSO=0) is on the line of matching source and destination
  pixels, so is the last point (SourceStretchSize, DestinationStretchSize)
  which almost but not exactly gets reached (drawing stops just as it is
  reached). */

  DDASourceIncrement = DestinationStretchSize * 2;
  DDADestinationDecrement = SourceStretchSize * 2;
  DDAInitialValue = DDASourceIncrement;

  /* Avoid divide by zero, size zero bitmaps will be clipped later. */
  if (DDASourceIncrement == 0) DDASourceIncrement = 1;
  if (DDADestinationDecrement == 0) DDADestinationDecrement = 1;

  /* Figure out what has to be copied; clip to edges of common part
  of images.  As source and destination are clipped, update the DDA. */

  if (SourceStartOffset < 0) /* Can't start source off left/top of bitmap. */
  {
    /* Remove part off left/top edge of source. */
    Excess = -SourceStartOffset;
    SourceStartOffset = 0;
    SourceStretchSize -= Excess; /* Reduce width/height to be copied. */
    DDAInitialValue += Excess * DDASourceIncrement; /* Increases DDA. */
    if (DDAInitialValue > DDASourceIncrement)
    {
      Excess = (int) (DDAInitialValue / DDADestinationDecrement);
      DestinationStartOffset += Excess;
      DestinationStretchSize -= Excess;
      DDAInitialValue -= Excess * DDADestinationDecrement;
    }
  }

  if (DestinationStartOffset < 0) /* Can't start destination off bitmap. */
  {
    Excess = -DestinationStartOffset;
    DestinationStartOffset = 0;
    DestinationStretchSize -= Excess; /* Reduce width/height to be copied. */
    DDAInitialValue -= Excess * DDADestinationDecrement; /* Decreases DDA. */
    if (DDAInitialValue < 0)
    {
      Excess = (int) (DDAInitialValue / -DDASourceIncrement);
      SourceStartOffset += Excess;
      SourceStretchSize -= Excess;
      DDAInitialValue += Excess * DDASourceIncrement;
    }
  }

  X = SourceStartOffset + SourceStretchSize; /* Where is end of source? */
  Excess = X - SourceImageSize;
  if (Excess > 0) /* Right hand side of source area is past edge of bitmap. */
  {
    SourceStretchSize -= Excess; /* Remove part off right hand source side. */
                                 /* Also have to update destination since it drives the whole thing.
    Agressive round-down here to make sure it doesn't overflow. */
    DestinationStretchSize = (int) ((SourceStretchSize * DDASourceIncrement) /
      DDADestinationDecrement);
  }

  X = DestinationStartOffset + DestinationStretchSize; /* Destination end. */
  Excess = X - DestinationImageSize;
  if (Excess > 0) /* Right hand side of destination area is past edge. */
  {
    DestinationStretchSize -= Excess; /* Remove part off right hand side. */
                                      /* Don't bother updating source since it isn't in control and would
                                         just make a lot of work figuring out integer aliasing. */
  }

  /* Don't overflow our stretch table array! */

  if (DestinationStretchSize > MAXSTRETCHDESTINATIONSIZE)
  {
    DestinationStretchSize = MAXSTRETCHDESTINATIONSIZE;
    /* Don't bother updating source since it isn't in control and would
    just make a lot of work figuring out integer aliasing. */
  }

  /* Save our calculated clipped size values. */

  StretchTablePntr->destinationStartOffset = DestinationStartOffset;
  StretchTablePntr->destinationStretchSize = DestinationStretchSize;
  StretchTablePntr->sourceStartOffset = SourceStartOffset;
  StretchTablePntr->sourceStretchSize = SourceStretchSize;

  if (SourceStartOffset >= SourceImageSize ||
    DestinationStartOffset >= DestinationImageSize ||
    SourceStretchSize <= 0 || DestinationStretchSize <= 0)
  {
    /* Nothing to stretch, mark the stretch operation as empty. */

    StretchTablePntr->destinationStretchSize = 0;
    StretchTablePntr->sourceStretchSize = 0;
    return;
  }

  /* Now stretch a range of pixels. */

  OffsetPntr = StretchTablePntr->pixelOffsets;
  DDADifference = DDAInitialValue;
  RemainingDestinationStretchSize = DestinationStretchSize;

#if CE_ARTLIB_EnableDebugMode
  RemainingSourceStretchSize = SourceStretchSize;
#endif

  while (RemainingDestinationStretchSize > 0)
  {
  /* Skip to the next source pixel.  Basically, while DDADifference is
     less than zero, add DDASourceIncrement to it and move one source
     pixel forwards for each add. */

#if FALSE /* Use this code if bitmaps are mostly shrunk. */
    if (DDADifference < 0)
    {
      OffsetCount = (short) ((DDASourceIncrement - 1 - DDADifference) /
        DDASourceIncrement);
      DDADifference += OffsetCount * DDASourceIncrement;
    }
    else
      OffsetCount = 0;
#else /* This version is faster for bitmaps that get larger. */
    OffsetCount = 0;
    while (DDADifference < 0)
    {
      DDADifference += DDASourceIncrement;
      OffsetCount++;
    }
#endif

#if CE_ARTLIB_EnableDebugMode
    RemainingSourceStretchSize -= OffsetCount;
#endif
    /* Mark down the relative offset for this destination pixel, and
    increment to the next destination. */

    *OffsetPntr++ = (unsigned char) OffsetCount;

    /* Update the DDA for the destination increment. */

    DDADifference -= DDADestinationDecrement;
    RemainingDestinationStretchSize--;
  }
#if CE_ARTLIB_EnableDebugMode
  if (RemainingSourceStretchSize < 0)
    LE_ERROR_ErrorMsg ("CalculateStretchTable: Source width exceeded.");
  if (RemainingDestinationStretchSize < 0)
    LE_ERROR_ErrorMsg ("CalculateStretchTable: Destination width exceeded.");
#endif
}



/***************************************************************************
*
*  BOOL LI_BLT_GetBitmapClipped(
*    int *SourceX, int *SourceY,
*    int *SourceRectWidth,
*    int *SourceRectHeight,
*    int SourceBitmapWidth,
*    int SourceBitmapHeight,
*    int *DestinationX, int *DestinationY,
*    int DestinationBitmapWidth,
*    int DestinationBitmapHeight)
*
*  Description:
*    Clips to the common edges of both source and destination images.
*    Any off screen parts are clipped off.
*
*  Arguments:
*     SourceX, SourceY:            offset of ul corner of source rectangle
*                                  from ul corner of source bitmap
*     SourceRectWidth:             width of source rectangle
*     SourceRectHeight:            height of source rectangle
*     SourceBitmapWidth:           bitmap width in pixels
*     SourceBitmapHeight:          bitmap height in pixels
*     DestinationX, DestinationY:  offset of destination rectangle from
*                                  the destination bitmap
*     DestinationBitmapWidth:      bitmap width in pixels
*     DestinationBitmapHeight:     bitmap height in pixels
*
*  Return:
*     FALSE if something went wrong.
*
*  - 13:58:00 Friday  3 July 1998
*
****************************************************************************/
BOOL LI_BLT_GetBitmapClipped(
  int *SourceX, int *SourceY,
  int *SourceRectWidth,
  int *SourceRectHeight,
  int SourceBitmapWidth,
  int SourceBitmapHeight,
  int *DestinationX, int *DestinationY,
  int DestinationBitmapWidth,
  int DestinationBitmapHeight)
{
  register int   X;
  register int   Y;
  register int   SourceOffsetX;         // X offset of source rectangle
  register int   SourceOffsetY;         // Y offset of source rectangle
  register int   DestinationOffsetX;    // X offset from the destination bitmap
  register int   DestinationOffsetY;    // Y offset from the destination bitmap
  register int   CopyWidth;             // width of source bitmap to copy
  register int   CopyHeight;            // height of source bitmap to copy
  register int   Excess;                // used for image clipping

  SourceOffsetX = *SourceX;
  SourceOffsetY = *SourceY;
  CopyWidth = *SourceRectWidth;
  CopyHeight = *SourceRectHeight;
  DestinationOffsetX = *DestinationX;
  DestinationOffsetY = *DestinationY;

  // Figure out what has to be copied; clip to edges of common part
  // of images.  First consider the widths and X positions.

  if(SourceOffsetX < 0) /* Can't start source off left side of bitmap. */
  {
    /* Remove part off left edge of source.  Note SourceX is negative. */
    CopyWidth += SourceOffsetX;     /* Reduce width to be copied. */
    DestinationOffsetX -= SourceOffsetX;  /* Starts writing furthur to the right. */
    SourceOffsetX = 0;
  }

  X = SourceOffsetX + CopyWidth; /* Where is right hand side of source? */
  Excess = X - SourceBitmapWidth;
  if (Excess > 0) /* Right hand side of source area is past edge of bitmap. */
    CopyWidth -= Excess; /* Remove part off right hand source side. */

  if (DestinationOffsetX < 0)  /* Area starts past left side of destination? */
  {
    /* Remove part off left edge.  Note DestinationX is negative. */
    CopyWidth += DestinationOffsetX;
    SourceOffsetX -= DestinationOffsetX;
    DestinationOffsetX = 0;
  }

  X = DestinationOffsetX + CopyWidth; /* Where is right hand side of destination? */
  Excess = X - DestinationBitmapWidth;
  if(Excess > 0) /* Right hand side of destination area is past edge. */
    CopyWidth -= Excess; /* Remove part off right hand destination side. */

  if(SourceOffsetX >= SourceBitmapWidth || DestinationOffsetX >= DestinationBitmapWidth ||
    CopyWidth <= 0)
    return FALSE; /* Nothing to do. */

  /* Clip the heights and starting height positions. */

  if(SourceOffsetY < 0) /* Can't start source off top of bitmap. */
  {
    /* Remove part off top of source.  Note SourceY is negative. */
    CopyHeight += SourceOffsetY; /* Reduce height to be copied. */
    DestinationOffsetY -= SourceOffsetY; /* Starts writing furthur down. */
    SourceOffsetY = 0;
  }

  Y = SourceOffsetY + CopyHeight; /* Where is bottom of source? */
  Excess = Y - SourceBitmapHeight;
  if (Excess > 0) /* Bottom of source area is past bottom of bitmap. */
    CopyHeight -= Excess; /* Remove part off bottom source. */

  if (DestinationOffsetY < 0)  /* Area starts past top of destination? */
  {
    /* Remove part off top.  Note DestinationY is negative. */
    CopyHeight += DestinationOffsetY; /* Reduce height. */
    SourceOffsetY -= DestinationOffsetY; /* Drop source start down (increases Y). */
    DestinationOffsetY = 0;
  }

  Y = DestinationOffsetY + CopyHeight; /* Where is bottom of destination? */
  Excess = Y - DestinationBitmapHeight;

  if (Excess > 0) /* Bottom of destination area is past edge. */
    CopyHeight -= Excess; /* Remove part off bottom destination. */

  if (SourceOffsetY >= SourceBitmapHeight || DestinationOffsetY >= DestinationBitmapHeight ||
    CopyHeight <= 0)
    return FALSE; /* Nothing to do. */

  // get the clipped final dimensions

  *SourceX = SourceOffsetX;
  *SourceY = SourceOffsetY;
  *SourceRectWidth = CopyWidth;
  *SourceRectHeight = CopyHeight;
  *DestinationX = DestinationOffsetX;
  *DestinationY = DestinationOffsetY;

  return TRUE;
}



/*****************************************************************************
 *****************************************************************************
 ** B I T M A P   C O P Y I N G   F U N C T I O N S                         **
 *****************************************************************************
 ****************************************************************************/

/*****************************************************************************
 * Copies the source bitmap to the given spot in the destination bitmap
 * (with clipping if it doesn't fit) and converts the pixel formats as it
 * goes.
 *
 * Source/destination Width and Height are the width and height of the
 * whole bitmap in pixels.  This is important for clipping.
 *
 * BitsPerPixel is what it says.  The code currently supports values of 8,
 * 16, 24 and 32.  Not all source and destination combinations are
 * implemented.
 *
 * If SourceHasBMPColours is non-zero then the source colours are assumed
 * to be in BMP format.  If in 24 bits then it has RGB pixels (blue then
 * green then red bytes in increasing memory order).  If it is 8 bit then
 * the palette has the same RGB colour interpretation.  The actual
 * destination bitmasks are assumed to be the same as the screen's bit
 * assignments (see LE_BLT_dwRedMask).  Non-BMP true colour images (this
 * flag is FALSE) have their pixels already converted to screen format,
 * including the palette.
 *
 * BytesPerLine is the scan line pitch - the number of bytes to add to a
 * pixel address to go down one scan line in the bitmap to the next row of
 * pixels and be at the same X position as the original pixel.  Usually
 * this is the width in bytes rounded up to the next multiple of 4 (since
 * this is a 32 bit processor which accesses things faster if they start at
 * an address which is an even multiple of 4, and they add pad bytes to the
 * end of each line to make this happen).
 *
 * The Palette is an array of 32 bit values (use a NULL pointer for images
 * which aren't 8 bits deep), each one containing a direct pixel colour
 * value (except when in BMP format).  This value (low 16 bits in 16 bit
 * mode, low 24 in 24, all 32 in 32) is written directly to the destination
 * pixel, without any conversions.  Since this is a solid copy operation,
 * the Alpha (translucency level) is ignored (unless copying to a 32 bit
 * bitmap).
 *
 * PaletteEntryByteSize is the number of bytes to add to advance to
 * the next colour entry in the palette.  It is usually 4, since there are
 * 4 bytes in a colour entry.  However, some palettes have alpha data words
 * alternating with the colours, so this would be 8 for that kind of
 * palette.
 *
 * CopyToDestX and CopyToDestY specify where the top left corner of the
 * source image ends up in the destination bitmap (or bottom left if
 * flipping).
 *
 * CopyWidth and CopyHeight put a limit to the size of the copied data.  If
 * you want to copy the whole source bitmap then just make these as large
 * as possible ((UNS16) -1) and ignore them.  Otherwise, they move in the
 * right and bottom edges to get the size you want (no stretching).
 *
 * FlipVertically is used to flip the bitmap upside down as it copies
 * (useful for BMP formats).
 *
 * Returns TRUE if successful.
 */

BOOL LE_BLT_CopySolidUnityRaw (
LPBYTE  SourceBitsPntr,         UNS32 SourceBytesPerLine,
UNS8    SourceBitsPerPixel,     BOOL  SourceHasBMPColours,
UNS16   SourceWidth,            UNS16 SourceHeight,
UNS32  *SourcePalettePntr,      UNS8  SourcePaletteEntryByteSize,
LPBYTE  DestinationBitsPntr,    UNS32 DestinationBytesPerLine,
UNS8    DestinationBitsPerPixel,
UNS16   DestinationWidth,       UNS16 DestinationHeight,
UNS32  *DestinationPalettePntr, UNS8  DestinationPaletteEntryByteSize,
INT16   CopyToDestX,            INT16 CopyToDestY,
UNS16   CopyToWidth,            UNS16 CopyToHeight,
BOOL    FlipVertically)
{
  int       ByteBlueShift = (LE_BLT_nBlueShift - 8) & (sizeof (unsigned long) * 8 - 1);
  int       ByteGreenShift = (LE_BLT_nGreenShift - 8) & (sizeof (unsigned long) * 8 - 1);
  int       ByteRedShift = (LE_BLT_nRedShift - 8) & (sizeof (unsigned long) * 8 - 1);
  INT16     CopyHeight;
  INT16     CopyWidth;
  UNS32     DestDWord;
  LPBYTE    DestPntr;
  UNS16     DestX;
  UNS16     DestY;
  int       i;
  UNS32     LocalBlueMask = LE_BLT_dwBlueMask;  // Local copies for speed.
  UNS32     LocalGreenMask = LE_BLT_dwGreenMask;
  UNS32     LocalRedMask = LE_BLT_dwRedMask;
  UNS32     SourceDWord;
  LPBYTE    SourcePntr;
  UNS16     SourceX;
  UNS16     SourceY;
  LPBYTE    TempDestPntr;
  LPBYTE    TempSourcePntr;

  // Figure out coordinates and amount to copy with clipping.

  if (CopyToWidth > SourceWidth)
    CopyWidth = SourceWidth;
  else
    CopyWidth = CopyToWidth;

  if (CopyToHeight > SourceHeight)
    CopyHeight = SourceHeight;
  else
    CopyHeight = CopyToHeight;

  SourceX = 0;
  SourceY = 0;

  if (CopyToDestX < 0)
  {
    DestX = 0;
    SourceX -= CopyToDestX; // Effectively adding it.
  }
  else
    DestX = CopyToDestX;

  if (CopyToDestY < 0)
  {
    DestY = 0;
    SourceY -= CopyToDestY; // Effectively adding it.
  }
  else
    DestY = CopyToDestY;

  if (DestX + CopyWidth > DestinationWidth)
    CopyWidth = DestinationWidth - DestX;

  if (DestY + CopyHeight > DestinationHeight)
    CopyHeight = DestinationHeight - DestY;

  if (CopyWidth <= 0 || CopyHeight <= 0)
    return TRUE; // Nothing to do.


  DestPntr = DestinationBitsPntr +
    (DestX * (UNS32) (DestinationBitsPerPixel / 8) +
    DestY * DestinationBytesPerLine);

  if (FlipVertically)
  {
    SourcePntr = SourceBitsPntr +
      (SourceX * (UNS32) (SourceBitsPerPixel / 8) +
      (SourceY + CopyHeight - 1) * SourceBytesPerLine);
  }
  else
  {
    SourcePntr = SourceBitsPntr +
      (SourceX * (UNS32) (SourceBitsPerPixel / 8) +
      SourceY * SourceBytesPerLine);
  }

  while (CopyHeight > 0)
  {
    // Now convert pixel formats (depths) and flip upside down (if needed)
    // while copying the original data to the surface.

    switch (SourceBitsPerPixel)
    {
    case 8: // Original image is 8 bits deep.  Assume destination is too.
      {
        switch (DestinationBitsPerPixel)
        {
        case 8:  // Source 8 bits, destination 8 bits.
          memcpy (DestPntr, SourcePntr, CopyWidth);
          break;

        default:
          return FALSE;
        }
      }
      break;


    case 16: // Original image is 16 bits deep.
      {
        switch (DestinationBitsPerPixel)
        {
        case 16:  // Source 16 bits, destination 16 bits.
          memcpy (DestPntr, SourcePntr, CopyWidth * 2);
          break;

          // doing blt from 16 to 24 bit colour depth, assuming that we know
          // the colour masks/shifts for the 16 bit screen colour depth
        case 24: // Source 16 bit, destination 24 bit
          {
            if (!SourceHasBMPColours)
              return FALSE; // Sorry, we only convert 16 bit BMP's to 24 bit.
            
            i = CopyWidth;
            TempDestPntr = DestPntr;
            TempSourcePntr = SourcePntr;

            int  nShift;

            while (i > 0)
            {
              // get one 16 bit pixel from source
              SourceDWord = * ((LPWORD)TempSourcePntr);
              TempSourcePntr += 2;
              
              // colour mask the source data and copy it to the destination

              // for blue colour component
              if ( (nShift = LE_BLT_nBlueShift - 8) >= 0 )
                DestDWord = (BYTE) ((SourceDWord & LocalBlueMask) >> nShift);
              else
                DestDWord = (BYTE) ((SourceDWord & LocalBlueMask) << -nShift);
              
              // for green colour component
              if ( (nShift = LE_BLT_nGreenShift - 16) >= 0 )
                DestDWord |= (WORD) ((SourceDWord & LocalGreenMask) >> nShift);
              else
                DestDWord |= (WORD) ((SourceDWord & LocalGreenMask) << -nShift);
              
              // for Red colour component
              if ( (nShift = LE_BLT_nRedShift - 24) >= 0 )
                DestDWord |= (DWORD) ((SourceDWord & LocalRedMask) >> nShift);
              else
                DestDWord |= (DWORD) ((SourceDWord & LocalRedMask) << -nShift);

              *(LPWORD)TempDestPntr = (WORD) DestDWord;
              TempDestPntr += 2;
              *TempDestPntr = (BYTE) (DestDWord >> 16);
              TempDestPntr ++;
              
              i--;
            }
          }
          break;

        default:
          return FALSE;
        }
      }
      break;


    case 24: // Original image is 24 bits deep.
      {
        switch (DestinationBitsPerPixel)
        {
        case 16:  // Source 24 bits, destination 16 bits.
          {
            if (!SourceHasBMPColours)
              return FALSE; // Sorry, we only convert 24 bit BMP's to 16 bit.

            // Bleeble - besides doing bulk operations using DWORD memory
            // accesses, this could be sped up by special cases for the
            // standard 5-5-5 and 5-6-5 RGB pixel formats that most video
            // cards use.

            i = CopyWidth;
            TempDestPntr = DestPntr;
            TempSourcePntr = SourcePntr;

            while (i > 0)
            {
              SourceDWord = *TempSourcePntr++;
              DestDWord = (_lrotl (SourceDWord, ByteBlueShift) & LocalBlueMask);
              SourceDWord = *TempSourcePntr++;
              DestDWord |= (_lrotl (SourceDWord, ByteGreenShift) & LocalGreenMask);
              SourceDWord = *TempSourcePntr++;
              DestDWord |= (_lrotl (SourceDWord, ByteRedShift) & LocalRedMask);
              * (UNS16 *) TempDestPntr = (UNS16) DestDWord;
              TempDestPntr += 2;
              i--;
            }
          }
          break;

        case 24:  // Source 24 bits, destination 24 bits.
          {
            // If source is same RGB/BGR format as destination.
            if (!SourceHasBMPColours || LocalBlueMask == 0x0000FF)
              memcpy (DestPntr, SourcePntr, CopyWidth * 3);
            else if (LocalBlueMask == 0xFF0000)
            {
              // Opposite (reversed) RGB bytes in the format.

              i = CopyWidth;
              TempDestPntr = DestPntr;
              TempSourcePntr = SourcePntr;

              // Bleeble - improve this code by DWORD aligning
              // the source pointer (do a few single pixels until
              // the pointer is a multiple of 4) and then batch
              // converting 4 pixels at a time (using DWORD accesses
              // to memory for source and dest).

              while (i > 0)
              {
                // Note use of (UNS8) and double shifts to extract the
                // bytes, the compiler generates more efficient
                // code for that then for anding with 0x00FF00.

                SourceDWord = * (UNS16 *) TempSourcePntr;
                TempSourcePntr += 2;
                DestDWord = (((UNS8) SourceDWord) << 16);
                DestDWord |= (((UNS8) (SourceDWord >> 8)) << 8);
                DestDWord |= * (UNS8 *) TempSourcePntr++;
                * (UNS16 *) TempDestPntr = (UNS16) DestDWord;
                TempDestPntr += 2;
                * (UNS8 *) TempDestPntr++ = (UNS8) (DestDWord >> 16);
                i--;
              }
            }
            else // Weird screen format, do it the slow general way.
            {
              i = CopyWidth;
              TempDestPntr = DestPntr;
              TempSourcePntr = SourcePntr;

              while (i > 0)
              {
                SourceDWord = *TempSourcePntr++;
                DestDWord = (_lrotl (SourceDWord, ByteBlueShift) & LocalBlueMask);
                SourceDWord = *TempSourcePntr++;
                DestDWord |= (_lrotl (SourceDWord, ByteGreenShift) & LocalGreenMask);
                SourceDWord = *TempSourcePntr++;
                DestDWord |= (_lrotl (SourceDWord, ByteRedShift) & LocalRedMask);
                * (UNS16 *) TempDestPntr = (UNS16) DestDWord;
                TempDestPntr += 2;
                * (UNS8 *) TempDestPntr++ = (UNS8) (DestDWord >> 16);
                i--;
              }
            }
          }
          break;

        case 32:  // Source 24 bits, destination 32 bits.
          {
            if (!SourceHasBMPColours)
              return FALSE; // Sorry, we only convert BMP's to 32 bit.

            i = CopyWidth;
            TempDestPntr = DestPntr;
            TempSourcePntr = SourcePntr;

            if (LocalBlueMask == 0x0000FF)
            {
              // Just expand BGR into BGR0.
              while (i > 0)
              {
                DestDWord = * (UNS16 *) TempSourcePntr;
                TempSourcePntr += 2;
                SourceDWord = * (UNS8 *) TempSourcePntr++;
                DestDWord |= (SourceDWord << 16);
                * (UNS32 *) TempDestPntr = DestDWord;
                TempDestPntr += 4;
                i--;
              }
            }
            else if (LocalBlueMask == 0xFF0000)
            {
              // Reversing BGR into RGB0.
              while (i > 0)
              {
                SourceDWord = * (UNS16 *) TempSourcePntr;
                TempSourcePntr += 2;
                DestDWord = ((SourceDWord & 0xFF) << 16);
                DestDWord |= (((UNS8) (SourceDWord >> 8)) << 8);
                DestDWord |= * (UNS8 *) TempSourcePntr++;
                * (UNS32 *) TempDestPntr = DestDWord;
                TempDestPntr += 4;
                i--;
              }
            }
            else // Weird screen format, do it the slow general way.
            {
              while (i > 0)
              {
                SourceDWord = *TempSourcePntr++;
                DestDWord = (_lrotl (SourceDWord, ByteBlueShift) & LocalBlueMask);
                SourceDWord = *TempSourcePntr++;
                DestDWord |= (_lrotl (SourceDWord, ByteGreenShift) & LocalGreenMask);
                SourceDWord = *TempSourcePntr++;
                DestDWord |= (_lrotl (SourceDWord, ByteRedShift) & LocalRedMask);
                * (UNS32 *) TempDestPntr = DestDWord;
                TempDestPntr += 4;
                i--;
              }
            }
          }
          break;

        default:
          return FALSE;
        }
      }
      break;


    case 32: // Original image is 32 bits deep.
      {
        switch (DestinationBitsPerPixel)
        {
          // doing blt from 32 to 24 bit colour depth, assuming that we know
          // the colour masks/shifts for the 32 bit screen colour depth
        case 24: // Source 32 bit, destination 24 bit
          {
            if (!SourceHasBMPColours)
              return FALSE; // Sorry, we only convert 32 bit BMP's to 24 bit.
            
            i = CopyWidth;
            TempDestPntr = DestPntr;
            TempSourcePntr = SourcePntr;
            
            int  nShift;
            
            while (i > 0)
            {
              // get one 32 bit pixel from source
              SourceDWord = * ((LPDWORD)TempSourcePntr);
              TempSourcePntr += 4;
              
              // colour mask the source data and copy it to the destination

              // for blue colour component
              if ( (nShift = LE_BLT_nBlueShift - 8) >= 0 )
                DestDWord = (BYTE) ((SourceDWord & LocalBlueMask) >> nShift);
              else
                DestDWord = (BYTE) ((SourceDWord & LocalBlueMask) << -nShift);
              
              // for green colour component
              if ( (nShift = LE_BLT_nGreenShift - 16) >= 0 )
                DestDWord |= (WORD) ((SourceDWord & LocalGreenMask) >> nShift);
              else
                DestDWord |= (WORD) ((SourceDWord & LocalGreenMask) << -nShift);
              
              // for Red colour component
              if ( (nShift = LE_BLT_nRedShift - 24) >= 0 )
                DestDWord |= (DWORD) ((SourceDWord & LocalRedMask) >> nShift);
              else
                DestDWord |= (DWORD) ((SourceDWord & LocalRedMask) << -nShift);

              *(LPWORD)TempDestPntr = (WORD) DestDWord;
              TempDestPntr += 2;
              *(LPBYTE)TempDestPntr = (BYTE) (DestDWord >> 16);
              TempDestPntr ++;
              
              i--;
            }
          }
          break;

        case 32:  // Source 32 bits, destination 32 bits.
          memcpy (DestPntr, SourcePntr, CopyWidth * 4);
          break;

        default:
          return FALSE;
        }
      }
      break;


    default: // Original image is unhandled number of bits deep.
      return FALSE;
    }

    // Advance to the next scan line.

    CopyHeight--;
    DestPntr += DestinationBytesPerLine;
    if (FlipVertically)
      SourcePntr -= SourceBytesPerLine;
    else
      SourcePntr += SourceBytesPerLine;
  }

  return TRUE;
}



/****************************************************************************/
/* G E N E R A T E D   B I T M A P   C O P Y I N G   F U N C T I O N S      */
/*--------------------------------------------------------------------------*/

// All major BLT functions are generated by L_BLTCPY.c and L_BLTINC.c and
// included into this module via L_BLTINC.i

#include "L_BLTINC.i"



#if !CE_ARTLIB_BlitUseOldRoutines
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
  LPLONG lpColorTable)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy8To16SolidUnityRaw (
    lpSourceBits,
    (UNS16) nXSourceWidthInPixels,
    (UNS16) lprSourceBltRect->bottom /* Fake the height */,
    lpColorTable,
    lprSourceBltRect,
    lpDestinationBits,
    nXDestinationWidthInPixels,
    (UNS16) DestBoundingRect.bottom /* Fake the height */,
    NULL,
    &DestBoundingRect,
    nXBltStartCoordinate,
    nYBltStartCoordinate);
}



void LI_BLT24BitBlt8to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy8To24SolidUnityRaw (
    lpSourceBits,
    (UNS16) nXSourceWidthInPixels,
    (UNS16) lprSourceBltRect->bottom /* Fake the height */,
    lpColorTable,
    lprSourceBltRect,
    lpDestinationBits,
    nXDestinationWidthInPixels,
    (UNS16) DestBoundingRect.bottom /* Fake the height */,
    NULL,
    &DestBoundingRect,
    nXBltStartCoordinate,
    nYBltStartCoordinate);
}



void LI_BLT32BitBlt8to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy8To32SolidUnityRaw (
    lpSourceBits,
    (UNS16) nXSourceWidthInPixels,
    (UNS16) lprSourceBltRect->bottom /* Fake the height */,
    lpColorTable,
    lprSourceBltRect,
    lpDestinationBits,
    nXDestinationWidthInPixels,
    (UNS16) DestBoundingRect.bottom /* Fake the height */,
    NULL,
    &DestBoundingRect,
    nXBltStartCoordinate,
    nYBltStartCoordinate);
}



void LI_BLT16ColorKeyBitBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = lprSourceBltRect->left + nXBltStartCoordinate;
  DestBoundingRect.right = lprSourceBltRect->right + nXBltStartCoordinate;
  DestBoundingRect.top = lprSourceBltRect->top + nYBltStartCoordinate;
  DestBoundingRect.bottom = lprSourceBltRect->bottom + nYBltStartCoordinate;

  LI_BLT_Copy8To16HolesUnityRaw (
    lpSourceBits,
    (UNS16) nXSourceWidthInPixels,
    (UNS16) lprSourceBltRect->bottom /* Fake the height */,
    lpColorTable,
    lprSourceBltRect,
    lpDestinationBits,
    nXDestinationWidthInPixels,
    (UNS16) DestBoundingRect.bottom /* Fake the height */,
    NULL,
    &DestBoundingRect,
    nXBltStartCoordinate,
    nYBltStartCoordinate);
}

void LI_BLT24ColorKeyBitBlt8to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = lprSourceBltRect->left + nXBltStartCoordinate;
  DestBoundingRect.right = lprSourceBltRect->right + nXBltStartCoordinate;
  DestBoundingRect.top = lprSourceBltRect->top + nYBltStartCoordinate;
  DestBoundingRect.bottom = lprSourceBltRect->bottom + nYBltStartCoordinate;

  LI_BLT_Copy8To24HolesUnityRaw (
    lpSourceBits,
    (UNS16) nXSourceWidthInPixels,
    (UNS16) lprSourceBltRect->bottom /* Fake the height */,
    lpColorTable,
    lprSourceBltRect,
    lpDestinationBits,
    nXDestinationWidthInPixels, // UNS16 DestinationBitmapWidth
  	(UNS16) (DestBoundingRect.bottom - DestBoundingRect.top) + nYBltStartCoordinate,  // UNS16 DestinationBitmapHeight (Fake it)
    NULL,
    &DestBoundingRect,
    nXBltStartCoordinate,
    nYBltStartCoordinate);
}

void LI_BLT32ColorKeyBitBlt8to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = lprSourceBltRect->left + nXBltStartCoordinate;
  DestBoundingRect.right = lprSourceBltRect->right + nXBltStartCoordinate;
  DestBoundingRect.top = lprSourceBltRect->top + nYBltStartCoordinate;
  DestBoundingRect.bottom = lprSourceBltRect->bottom + nYBltStartCoordinate;

  LI_BLT_Copy8To32HolesUnityRaw (
    lpSourceBits,
    (UNS16) nXSourceWidthInPixels,
    (UNS16) lprSourceBltRect->bottom /* Fake the height */,
    lpColorTable,
    lprSourceBltRect,
    lpDestinationBits,
    (UNS16) nXDestinationWidthInPixels,       // UNS16 DestinationBitmapWidth
  	(UNS16) (DestBoundingRect.bottom - DestBoundingRect.top) + nYBltStartCoordinate,  // UNS16 DestinationBitmapHeight (Fake it)
    NULL,
    &DestBoundingRect,
    nXBltStartCoordinate,
    nYBltStartCoordinate);
}

void LI_BLT16AlphaBitBlt8to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable,
  int nAlphaInColorTable)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = lprSourceBltRect->left + nXBltStartCoordinate;
  DestBoundingRect.right = lprSourceBltRect->right + nXBltStartCoordinate;
  DestBoundingRect.top = lprSourceBltRect->top + nYBltStartCoordinate;
  DestBoundingRect.bottom = lprSourceBltRect->bottom + nYBltStartCoordinate;

  LI_BLT_Copy8To16AlphaUnityRaw (
    lpSourceBits,
    (UNS16) nXSourceWidthInPixels,
    (UNS16) lprSourceBltRect->bottom /* Fake the height */,
    lpColorTable,
    (UNS16) nAlphaInColorTable,
    lprSourceBltRect,
    lpDestinationBits,
    (UNS16) nXDestinationWidthInPixels,       // UNS16 DestinationBitmapWidth
  	(UNS16) (DestBoundingRect.bottom - DestBoundingRect.top) + nYBltStartCoordinate,  // UNS16 DestinationBitmapHeight (Fake it)
    NULL,
    &DestBoundingRect,
    nXBltStartCoordinate,
    nYBltStartCoordinate);
}

void LI_BLT24AlphaBitBlt8to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable,
  int nAlphaInColorTable)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = lprSourceBltRect->left + nXBltStartCoordinate;
  DestBoundingRect.right = lprSourceBltRect->right + nXBltStartCoordinate;
  DestBoundingRect.top = lprSourceBltRect->top + nYBltStartCoordinate;
  DestBoundingRect.bottom = lprSourceBltRect->bottom + nYBltStartCoordinate;

  LI_BLT_Copy8To24AlphaUnityRaw (
    lpSourceBits,
    (UNS16) nXSourceWidthInPixels,
    (UNS16) lprSourceBltRect->bottom /* Fake the height */,
    lpColorTable,
    (UNS16) nAlphaInColorTable,
    lprSourceBltRect,
    lpDestinationBits,
    (UNS16) nXDestinationWidthInPixels,       // UNS16 DestinationBitmapWidth
  	(UNS16) (DestBoundingRect.bottom - DestBoundingRect.top) + nYBltStartCoordinate,  // UNS16 DestinationBitmapHeight (Fake it)
    NULL,
    &DestBoundingRect,
    nXBltStartCoordinate,
    nYBltStartCoordinate);
}

void LI_BLT32AlphaBitBlt8to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  LPLONG lpColorTable,
  int nAlphaInColorTable)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = lprSourceBltRect->left + nXBltStartCoordinate;
  DestBoundingRect.right = lprSourceBltRect->right + nXBltStartCoordinate;
  DestBoundingRect.top = lprSourceBltRect->top + nYBltStartCoordinate;
  DestBoundingRect.bottom = lprSourceBltRect->bottom + nYBltStartCoordinate;

  LI_BLT_Copy8To32AlphaUnityRaw (
    lpSourceBits,
    (UNS16) nXSourceWidthInPixels,
    (UNS16) lprSourceBltRect->bottom /* Fake the height */,
    lpColorTable,
    (UNS16) nAlphaInColorTable,
    lprSourceBltRect,
    lpDestinationBits,
    (UNS16) nXDestinationWidthInPixels,       // UNS16 DestinationBitmapWidth
  	(UNS16) (DestBoundingRect.bottom - DestBoundingRect.top) + nYBltStartCoordinate,  // UNS16 DestinationBitmapHeight (Fake it)
    NULL,
    &DestBoundingRect,
    nXBltStartCoordinate,
    nYBltStartCoordinate);
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
  LPBYTE lpDestinationBeginBlt;             // pointer to destination scanline for conversion
  int nXDestinationWidthInBytes;            // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;                  // pointer to source scanline for conversion
  int nXSourceWidthInBytes;                 // source bitmap width in bytes

  register LPBYTE lpDestinationBltPointer;  // pointer to current conversion destination
  register LPBYTE lpSourceBltPointer;       // pointer to current conversion source
  register int nPixelCounter;               // pixel counter
  register int nLinesCounter;               // line counter
  register WORD w16BitColor;                // converted 16 bit pixel
  register DWORD dw24BitData1;              // source data storage
  register DWORD dw24BitData2;
  register DWORD dw24BitData3;
  register short nShift16;                  // how many bits to shift values from 24 bit DIB

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



/*******************************************************************************
*
*   Name:         LI_BLT24ConvertRGBValuesTo24BitColor
*
*   Description:  converts a 24 bit DIB to 24 bit DIB
*
*   Arguments:    lp24DestinationBitData - pointer to 24 bit DIB bits
*                 lp24SourceBitData - pointer to 24 bit DIB bits
*                 sWidthBitmapInPixels - width of source DIB in pixels
*                 sHeightBitmapInPixels - height of source DIB in pixels
*                 dWidthBitmapInPixels - width of dest DIB in pixels
*                 dHeightBitmapInPixels - height of dest DIB in pixels
*
*   Globals:      LE_BLT_nRedShift - number of first bit set in LE_BLT_dwRedMask (1 to 24)
*                 LE_BLT_nGreenShift -  number of first bit set in LE_BLT_dwGreenMask (1 - 24)
*                 LE_BLT_nBlueShift - number of first bit set in LE_BLT_dwBlueMask (1 - 24)
*                 LE_BLT_dwRedMask - mask of red bits for 24 bit DIBs
*                 LE_BLT_dwGreenMask - mask of green bits for 24 bit DIBs
*                 LE_BLT_dwBlueMask - mask of blue bits for 24 bit DIBs
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
  int dest_h_pixels)
{
  LPBYTE lpDestinationBeginBlt;             // pointer to destination scanline for conversion
  int nXDestinationWidthInBytes;            // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;                  // pointer to source scanline for conversion
  int nXSourceWidthInBytes;                 // source bitmap width in bytes

  register LPBYTE lpDestinationBltPointer;  // pointer to current conversion destination
  register LPBYTE lpSourceBltPointer;       // pointer to current conversion source
  register int nPixelCounter;               // pixel counter
  register int nLinesCounter;               // line counter
  register DWORD dw24BitColor;              // converted 24 bit pixel
  register DWORD dw24BitData1;              // source data storage
  register DWORD dw24BitData2;
  register DWORD dw24BitData3;
  register short nShift24;                  // how many bits to shift values from 24 bit DIB

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = ((dest_w_pixels * 3) + 3) & 0xfffffffc;
  nXSourceWidthInBytes      = ((src_w_pixels * 3) + 3) & 0xfffffffc;

  // initialize start scanline pointers
  lpDestinationBeginBlt = lp24DestinationBitData;
  lpSourceBeginBlt = lp24SourceBitData + (nXSourceWidthInBytes * (src_h_pixels - 1));

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

        // convert 24 bits of data to 24 bits
        nShift24 = LE_BLT_nBlueShift - 8;
        if (nShift24 >= 0)
          dw24BitColor = ((dw24BitData1 & 0xff) << nShift24) & LE_BLT_dwBlueMask;
        else
          dw24BitColor = ((dw24BitData1 & 0xff) >> -nShift24) & LE_BLT_dwBlueMask;
        nShift24 = LE_BLT_nGreenShift - 16;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData1 & 0xff00) << nShift24) & LE_BLT_dwGreenMask;
        else
          dw24BitColor |= ((dw24BitData1 & 0xff00) >> -nShift24) & LE_BLT_dwGreenMask;
        nShift24 = LE_BLT_nRedShift - 24;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData1 & 0xff0000) << nShift24) & LE_BLT_dwRedMask;
        else
          dw24BitColor |= ((dw24BitData1 & 0xff0000) >> -nShift24) & LE_BLT_dwRedMask;

        // store 24 bit color
        *((LPWORD)lpDestinationBltPointer) = (WORD)dw24BitColor;
        lpDestinationBltPointer += 2;
        *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dw24BitColor >> 16);
        lpDestinationBltPointer += 1;

        // convert 24 bits of data to 24 bits
        nShift24 = LE_BLT_nBlueShift - 32;
        if (nShift24 >= 0)
          dw24BitColor = ((dw24BitData1 & 0xff000000) << nShift24) & LE_BLT_dwBlueMask;
        else
          dw24BitColor = ((dw24BitData1 & 0xff000000) >> -nShift24) & LE_BLT_dwBlueMask;
        nShift24 = LE_BLT_nGreenShift - 8;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData2 & 0xff) << nShift24) & LE_BLT_dwGreenMask;
        else
          dw24BitColor |= ((dw24BitData2 & 0xff) >> -nShift24) & LE_BLT_dwGreenMask;
        nShift24 = LE_BLT_nRedShift - 16;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData2 & 0xff00) << nShift24) & LE_BLT_dwRedMask;
        else
          dw24BitColor |= ((dw24BitData2 & 0xff00) >> -nShift24) & LE_BLT_dwRedMask;

        // store 24 bit color
        *((LPWORD)lpDestinationBltPointer) = (WORD)dw24BitColor;
        lpDestinationBltPointer += 2;
        *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dw24BitColor >> 16);
        lpDestinationBltPointer += 1;

        // convert 24 bits of data to 24 bits
        nShift24 = LE_BLT_nBlueShift - 24;
        if (nShift24 >= 0)
          dw24BitColor = ((dw24BitData2 & 0xff0000) << nShift24) & LE_BLT_dwBlueMask;
        else
          dw24BitColor = ((dw24BitData2 & 0xff0000) >> -nShift24) & LE_BLT_dwBlueMask;
        nShift24 = LE_BLT_nGreenShift - 32;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData2 & 0xff000000) << nShift24) & LE_BLT_dwGreenMask;
        else
          dw24BitColor |= ((dw24BitData2 & 0xff000000) >> -nShift24) & LE_BLT_dwGreenMask;
        nShift24 = LE_BLT_nRedShift - 8;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData3 & 0xff) << nShift24) & LE_BLT_dwRedMask;
        else
          dw24BitColor |= ((dw24BitData3 & 0xff) >> -nShift24) & LE_BLT_dwRedMask;

        // store 24 bit color
        *((LPWORD)lpDestinationBltPointer) = (WORD)dw24BitColor;
        lpDestinationBltPointer += 2;
        *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dw24BitColor >> 16);
        lpDestinationBltPointer += 1;

        // convert 24 bits of data to 24 bits
        nShift24 = LE_BLT_nBlueShift - 16;
        if (nShift24 >= 0)
          dw24BitColor = ((dw24BitData3 & 0xff00) << nShift24) & LE_BLT_dwBlueMask;
        else
          dw24BitColor = ((dw24BitData3 & 0xff00) >> -nShift24) & LE_BLT_dwBlueMask;
        nShift24 = LE_BLT_nGreenShift - 24;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData3 & 0xff0000) << nShift24) & LE_BLT_dwGreenMask;
        else
          dw24BitColor |= ((dw24BitData3 & 0xff0000) >> -nShift24) & LE_BLT_dwGreenMask;
        nShift24 = LE_BLT_nRedShift - 32;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData3 & 0xff000000) << nShift24) & LE_BLT_dwRedMask;
        else
          dw24BitColor |= ((dw24BitData3 & 0xff000000) >> -nShift24) & LE_BLT_dwRedMask;

        // store 24 bit color
        *((LPWORD)lpDestinationBltPointer) = (WORD)dw24BitColor;
        lpDestinationBltPointer += 2;
        *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dw24BitColor >> 16);
        lpDestinationBltPointer += 1;
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

        // convert 24 bits of data to 24 bits
        nShift24 = LE_BLT_nBlueShift - 8;
        if (nShift24 >= 0)
          dw24BitColor = ((dw24BitData1 & 0xff) << nShift24) & LE_BLT_dwBlueMask;
        else
          dw24BitColor = ((dw24BitData1 & 0xff) >> -nShift24) & LE_BLT_dwBlueMask;
        nShift24 = LE_BLT_nGreenShift - 16;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData1 & 0xff00) << nShift24) & LE_BLT_dwGreenMask;
        else
          dw24BitColor |= ((dw24BitData1 & 0xff00) >> -nShift24) & LE_BLT_dwGreenMask;
        nShift24 = LE_BLT_nRedShift - 24;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData1 & 0xff0000) << nShift24) & LE_BLT_dwRedMask;
        else
          dw24BitColor |= ((dw24BitData1 & 0xff0000) >> -nShift24) & LE_BLT_dwRedMask;

        // store 24 bit color
        *((LPWORD)lpDestinationBltPointer) = (WORD)dw24BitColor;
        lpDestinationBltPointer += 2;
        *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dw24BitColor >> 16);
        lpDestinationBltPointer += 1;

        // convert 24 bits of data to 24 bits
        nShift24 = LE_BLT_nBlueShift - 32;
        if (nShift24 >= 0)
          dw24BitColor = ((dw24BitData1 & 0xff000000) << nShift24) & LE_BLT_dwBlueMask;
        else
          dw24BitColor = ((dw24BitData1 & 0xff000000) >> -nShift24) & LE_BLT_dwBlueMask;
        nShift24 = LE_BLT_nGreenShift - 8;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData2 & 0xff) << nShift24) & LE_BLT_dwGreenMask;
        else
          dw24BitColor |= ((dw24BitData2 & 0xff) >> -nShift24) & LE_BLT_dwGreenMask;
        nShift24 = LE_BLT_nRedShift - 16;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData2 & 0xff00) << nShift24) & LE_BLT_dwRedMask;
        else
          dw24BitColor |= ((dw24BitData2 & 0xff00) >> -nShift24) & LE_BLT_dwRedMask;

        // store 24 bit color
        *((LPWORD)lpDestinationBltPointer) = (WORD)dw24BitColor;
        lpDestinationBltPointer += 2;
        *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dw24BitColor >> 16);
        lpDestinationBltPointer += 1;
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

        // convert 24 bits of data to 24 bits
        nShift24 = LE_BLT_nBlueShift - 8;
        if (nShift24 >= 0)
          dw24BitColor = ((dw24BitData1 & 0xff) << nShift24) & LE_BLT_dwBlueMask;
        else
          dw24BitColor = ((dw24BitData1 & 0xff) >> -nShift24) & LE_BLT_dwBlueMask;
        nShift24 = LE_BLT_nGreenShift - 16;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData1 & 0xff00) << nShift24) & LE_BLT_dwGreenMask;
        else
          dw24BitColor |= ((dw24BitData1 & 0xff00) >> -nShift24) & LE_BLT_dwGreenMask;
        nShift24 = LE_BLT_nRedShift - 8;
        if (nShift24 >= 0)
          dw24BitColor |= ((dw24BitData2 & 0xff) << nShift24) & LE_BLT_dwRedMask;
        else
          dw24BitColor |= ((dw24BitData2 & 0xff) >> -nShift24) & LE_BLT_dwRedMask;

        // store 24 bit color
        *((LPWORD)lpDestinationBltPointer) = (WORD)dw24BitColor;
        lpDestinationBltPointer += 2;
        *((LPBYTE)lpDestinationBltPointer) = (BYTE)(dw24BitColor >> 16);
        lpDestinationBltPointer += 1;
        nPixelCounter -= 1;
      }
    }
    // update pointers to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt -= nXSourceWidthInBytes;
  }
}



/*******************************************************************************
*
*   Name:         LI_BLT32ConvertRGBValuesTo32BitColor
*
*   Description:  converts a 24 bit DIB to 32 bit DIB
*
*   Arguments:    lp32BitData - pointer to 32 bit DIB bits
*                 lp24BitData - pointer to 24 bit DIB bits
*                 sWidthBitmapInPixels - width of source DIB in pixels
*                 sHeightBitmapInPixels - height of source DIB in pixels
*                 dWidthBitmapInPixels - width of dest DIB in pixels
*                 dHeightBitmapInPixels - height of dest DIB in pixels
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
  LPBYTE lpDestinationBeginBlt;             // pointer to destination scanline for conversion
  int nXDestinationWidthInBytes;            // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;                  // pointer to source scanline for conversion
  int nXSourceWidthInBytes;                 // source bitmap width in bytes

  register LPBYTE lpDestinationBltPointer;  // pointer to current conversion destination
  register LPBYTE lpSourceBltPointer;       // pointer to current conversion source
  register int nPixelCounter;               // pixel counter
  register int nLinesCounter;               // line counter
  register DWORD dw32BitColor;              // converted 32 bit pixel
  register DWORD dw24BitData1;              // source data storage
  register DWORD dw24BitData2;
  register DWORD dw24BitData3;
  register short nShift32;                  // how many bits to shift values from 24 bit DIB

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


void LI_BLT16BitBlt16to16(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy16To16SolidUnityRaw (
	  lpSourceBits,
	  (UNS16) nXSourceWidthInPixels,
	  (UNS16) lprSourceBltRect->bottom,
	  lprSourceBltRect,
	  lpDestinationBits,
	  (UNS16) nXDestinationWidthInPixels,
	  (UNS16) DestBoundingRect.bottom,
	  NULL,
	  &DestBoundingRect,
	  nXBltStartCoordinate,
	  nYBltStartCoordinate);
}

void LI_BLT24BitBlt24to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy24To24SolidUnityRaw (
	  lpSourceBits,
	  (UNS16) nXSourceWidthInPixels,
	  (UNS16) lprSourceBltRect->bottom,
	  lprSourceBltRect,
	  lpDestinationBits,
	  (UNS16) nXDestinationWidthInPixels,
	  (UNS16) DestBoundingRect.bottom,
	  NULL,
	  &DestBoundingRect,
	  nXBltStartCoordinate,
	  nYBltStartCoordinate);
}

void LI_BLT32BitBlt32to32(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy32To32SolidUnityRaw (
	  lpSourceBits,
	  (UNS16) nXSourceWidthInPixels,
	  (UNS16) lprSourceBltRect->bottom,
	  lprSourceBltRect,
	  lpDestinationBits,
	  (UNS16) nXDestinationWidthInPixels,
	  (UNS16) DestBoundingRect.bottom,
	  NULL,
	  &DestBoundingRect,
	  nXBltStartCoordinate,
	  nYBltStartCoordinate);
}

void LI_BLT16ColorKeyPureGreenBitBlt16to16(
  LPBYTE  lpDestinationBits,
  int     nXDestinationWidthInPixels,
  int     nXBltStartCoordinate,
  int     nYBltStartCoordinate,
  LPBYTE  lpSourceBits,
  int     nXSourceWidthInPixels,
  LPRECT  lprSourceBltRect)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy16To16SolidColourKeyUnityRaw (
  lpSourceBits,
  (UNS16) nXSourceWidthInPixels,
  (UNS16) lprSourceBltRect->bottom,
  lprSourceBltRect,
  lpDestinationBits,
  (UNS16) nXDestinationWidthInPixels,
  (UNS16) DestBoundingRect.bottom,
  NULL,
  &DestBoundingRect,
  nXBltStartCoordinate,
  nYBltStartCoordinate);
}

void LI_BLT24ColorKeyPureGreenBitBlt24to24(
  LPBYTE  lpDestinationBits,
  int     nXDestinationWidthInPixels,
  int     nXBltStartCoordinate,
  int     nYBltStartCoordinate,
  LPBYTE  lpSourceBits,
  int     nXSourceWidthInPixels,
  LPRECT  lprSourceBltRect)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy24To24SolidColourKeyUnityRaw (
  lpSourceBits,
  (UNS16) nXSourceWidthInPixels,
  (UNS16) lprSourceBltRect->bottom,
  lprSourceBltRect,
  lpDestinationBits,
  (UNS16) nXDestinationWidthInPixels,
  (UNS16) DestBoundingRect.bottom,
  NULL,
  &DestBoundingRect,
  nXBltStartCoordinate,
  nYBltStartCoordinate);
}

void LI_BLT32ColorKeyPureGreenBitBlt32to32(
  LPBYTE  lpDestinationBits,
  int     nXDestinationWidthInPixels,
  int     nXBltStartCoordinate,
  int     nYBltStartCoordinate,
  LPBYTE  lpSourceBits,
  int     nXSourceWidthInPixels,
  LPRECT  lprSourceBltRect)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy32To32SolidColourKeyUnityRaw (
  lpSourceBits,
  (UNS16) nXSourceWidthInPixels,
  (UNS16) lprSourceBltRect->bottom,
  lprSourceBltRect,
  lpDestinationBits,
  (UNS16) nXDestinationWidthInPixels,
  (UNS16) DestBoundingRect.bottom,
  NULL,
  &DestBoundingRect,
  nXBltStartCoordinate,
  nYBltStartCoordinate);
}

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
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy16To16AlphaUnityRaw (
  lpSourceBits,
  (UNS16) nXSourceWidthInPixels,
  (UNS16) lprSourceBltRect->bottom,
  (INT16) nAlphaValue,
  lprSourceBltRect,
  lpDestinationBits,
  (UNS16) nXDestinationWidthInPixels,
  (UNS16) DestBoundingRect.bottom,
  NULL,
  &DestBoundingRect,
  nXBltStartCoordinate,
  nYBltStartCoordinate);
}

void LI_BLT24AlphaBitBlt24to24(
  LPBYTE lpDestinationBits,
  int nXDestinationWidthInPixels,
  int nXBltStartCoordinate,
  int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels,
  LPRECT lprSourceBltRect,
  int nAlphaValue)
{
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy24To24AlphaUnityRaw (
  lpSourceBits,
  (UNS16) nXSourceWidthInPixels,
  (UNS16) lprSourceBltRect->bottom,
  (INT16) nAlphaValue,
  lprSourceBltRect,
  lpDestinationBits,
  (UNS16) nXDestinationWidthInPixels,
  (UNS16) DestBoundingRect.bottom,
  NULL,
  &DestBoundingRect,
  nXBltStartCoordinate,
  nYBltStartCoordinate);
}

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
  RECT  DestBoundingRect;

  DestBoundingRect.left = DestBoundingRect.top = 0;
  DestBoundingRect.right = nXDestinationWidthInPixels;
  DestBoundingRect.bottom = (lprSourceBltRect->bottom - lprSourceBltRect->top) + nYBltStartCoordinate; // Fake it.

  LI_BLT_Copy32To32AlphaUnityRaw (
  lpSourceBits,
  (UNS16) nXSourceWidthInPixels,
  (UNS16) lprSourceBltRect->bottom,
  (INT16) nAlphaValue,
  lprSourceBltRect,
  lpDestinationBits,
  (UNS16) nXDestinationWidthInPixels,
  (UNS16) DestBoundingRect.bottom,
  NULL,
  &DestBoundingRect,
  nXBltStartCoordinate,
  nYBltStartCoordinate);
}


/*--------------------------------------------------------------------------*/
/* END OF C O M P A T A B I L I T Y   S T U B   F U N C T I O N S           */
/****************************************************************************/
#endif // !CE_ARTLIB_BlitUseOldRoutines

#endif // if CE_ARTLIB_EnableSystemGrafix
