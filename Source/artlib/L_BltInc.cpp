/*****************************************************************************
 *
 * FILE:        L_BLTINC.CPP
 * DESCRIPTION: Generates code for bit block transfer (blit) image copying
 *              and manipulation routines.  This file repeatedly includes
 *              the L_BLTCPY.cpp file and uses the C pre-processor to create
 *              the L_BLTINC.i file (containing many variations of the basic
 *              pixel copying function) which is then used by the main
 *              L_BLT.c file.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_BltInc.cpp 6     3/29/99 3:20p Lzou $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_BltInc.cpp $
 * 
 * 6     3/29/99 3:20p Lzou
 * Have implemented bitmap stretch copy with alpha channel effects for
 * 16to16, 24to24, and 32to32 bit colour depths.
 * 
 * 5     3/25/99 8:59a Agmsmith
 * Restore corrupted SourceSafe files.
 * 
 * 5     3/24/99 5:40p Lzou
 * Have implemented bitmap unity copy with alpha channel effects for 16 to
 * 16 bit colour depth, 24 to 24 bit colour depth, and 32 to 32 bit colour
 * depth.
 * 
 * 4     9/21/98 6:11p Agmsmith
 * Added hole copies.
 * 
 * 3     9/17/98 4:18p Agmsmith
 * Added colour filling functions, compiles under C++ now.
 * 
 * 2     9/15/98 11:56a Agmsmith
 * Converted to use C++
 * 
 * 5     8/14/98 3:10p Lzou
 * I am doing Holes & Stretching bitmap copy.
 * 
 * 4     8/13/98 5:29p Lzou
 * Have implemented Solid with Colour Key & Unity bitmap copy.
 * 
 * 3     8/12/98 4:04p Lzou
 * Have implemented Solid with Colour Key & Stretching bitmap copy.
 * 
 * 2     8/04/98 3:05p Lzou
 * Check my BLT routines into Artech production library.
 * 
 * 3     7/27/98 4:54p Lzou
 * Have finished Alpha & Unity, and Alpha & Stretching bitmap copy.
 * 
 * 2     7/14/98 2:32p Lzou
 * Have done all of the Unity & Solid bitmap copy. Have partly done the
 * Stretching & Solid bitmap copy.
 * 
 * 1     7/02/98 3:22p Lzou
 * Implement the BLT module
 * 
 * 1     6/12/98 3:40p Agmsmith
 * Preprocessor code generation of the various different blitter copy
 * functions.
 ****************************************************************************/

/*****************************************************************************
 *****************************************************************************
 ** I N C L U D E   F I L E S                                               **
 *****************************************************************************
 ****************************************************************************/

#include <c_artlib.h>
// Needed for CE_ARTLIB_BlitUseGreenColourKey and other defines.



/*****************************************************************************
 *****************************************************************************
 ** M A C R O   D E F I N I T I O N S                                       **
 *****************************************************************************
 ****************************************************************************/

#define LateDefine #define
#define LatePragma #pragma
#define LateUndef  #undef
  // Used for making defines which get done in the second pass, when the
  // L_BLTINC.i file is included by L_BLT.c.  Needed because the ## token
  // concatenator can't take symbolic arguments.

// Some symbolic defines used to control code generation.  Can't use an enum,
// since this is for the preprocessor.

#define INC_SOLID      1
#define INC_HOLES      2
#define INC_ALPHA      3
#define INC_GENERAL    4
#define INC_RAWBITMAP  5
#define INC_UNITY      6
#define INC_STRETCH    7

/*****************************************************************************
 *****************************************************************************
 ** G L O B A L   V A R I A B L E S                                         **
 *****************************************************************************
 ****************************************************************************/

/*****************************************************************************
 *****************************************************************************
 ** S T A T I C   V A R I A B L E S                                         **
 *****************************************************************************
 ****************************************************************************/



/*****************************************************************************
 *****************************************************************************
 ** I M P L E M E N T A T I O N   C O D E                                   **
 *****************************************************************************
 ****************************************************************************/

// Generate blitter copy functions by repeatedly including L_BLTCPY.c with
// assorted different defines for source and destination depths and other
// settings.

//
// =============================================================
//
//          Unity & Solid, Raw bitmap copy
//

#define INC_SOURCE_DEPTH  8
#define INC_SOURCEFORMAT  INC_RAWBITMAP
#define INC_DEST_DEPTH    16
#define INC_COPYMODE      INC_SOLID
#define INC_STRETCHMODE   INC_UNITY
#define INC_COLOURKEY     0
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To16SolidUnity */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To24SolidUnity */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To32SolidUnity */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  16
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    16
#include "L_BLTCPY.cpp"     /* L_BLT_Copy16To16SolidUnity */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  24
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy24To24SolidUnity */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  32
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy32To32SolidUnity */

//
//
// =============================================================
//
//          Unity & Solid with Colour Key, Raw bitmap copy
//

#undef  INC_SOURCE_DEPTH
#undef  INC_SOURCEFORMAT
#undef  INC_DEST_DEPTH
#undef  INC_COPYMODE
#undef  INC_STRETCHMODE
#undef  INC_COLOURKEY

#define INC_SOURCE_DEPTH  8
#define INC_SOURCEFORMAT  INC_RAWBITMAP
#define INC_DEST_DEPTH    16
#define INC_COPYMODE      INC_SOLID
#define INC_STRETCHMODE   INC_UNITY
#define INC_COLOURKEY     1
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To16SolidColourKeyUnity */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To24SolidColourKeyUnity */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To32SolidColourKeyUnity */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  16
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    16
#include "L_BLTCPY.cpp"     /* L_BLT_Copy16To16SolidColourKeyUnity */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  24
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy24To24SolidColourKeyUnity */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  32
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy32To32SolidColourKeyUnity */

//
// =============================================================
//
//          Unity & Alpha, Raw bitmap copy
//

#undef  INC_SOURCE_DEPTH
#undef  INC_SOURCEFORMAT
#undef  INC_DEST_DEPTH
#undef  INC_COPYMODE
#undef  INC_STRETCHMODE
#undef  INC_COLOURKEY

#define INC_SOURCE_DEPTH  8
#define INC_SOURCEFORMAT  INC_RAWBITMAP
#define INC_DEST_DEPTH    16
#define INC_COPYMODE      INC_ALPHA
#define INC_STRETCHMODE   INC_UNITY
#define INC_COLOURKEY     0
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To16AlphaUnity */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To24AlphaUnity */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To32AlphaUnity */

#undef  INC_SOURCE_DEPTH
#undef  INC_DEST_DEPTH
#define INC_SOURCE_DEPTH  16
#define INC_DEST_DEPTH    16
#include "L_BLTCPY.cpp"     /* L_BLT_Copy16To16AlphaUnity */

#undef  INC_SOURCE_DEPTH
#undef  INC_DEST_DEPTH
#define INC_SOURCE_DEPTH  24
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy24To24AlphaUnity */

#undef  INC_SOURCE_DEPTH
#undef  INC_DEST_DEPTH
#define INC_SOURCE_DEPTH  32
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy32To32AlphaUnity */


//
//
// =============================================================
//
//          Stretching & Solid, Raw bitmap copy
//

#undef  INC_SOURCE_DEPTH
#undef  INC_SOURCEFORMAT
#undef  INC_DEST_DEPTH
#undef  INC_COPYMODE
#undef  INC_STRETCHMODE
#undef  INC_COLOURKEY

#define INC_SOURCE_DEPTH  8
#define INC_SOURCEFORMAT  INC_RAWBITMAP
#define INC_DEST_DEPTH    16
#define INC_COPYMODE      INC_SOLID
#define INC_STRETCHMODE   INC_STRETCH
#define INC_COLOURKEY     0
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To16SolidStretch */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To24SolidStretch */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To32SolidStretch */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  16
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    16
#include "L_BLTCPY.cpp"     /* L_BLT_Copy16To16SolidStretch */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  24
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy24To24SolidStretch */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  32
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy32To32SolidStretch */

//
//
// =============================================================
//
//          Stretching & Solid with Colour Key, Raw bitmap copy
//

#undef  INC_SOURCE_DEPTH
#undef  INC_SOURCEFORMAT
#undef  INC_DEST_DEPTH
#undef  INC_COPYMODE
#undef  INC_STRETCHMODE
#undef  INC_COLOURKEY

#define INC_SOURCE_DEPTH  8
#define INC_SOURCEFORMAT  INC_RAWBITMAP
#define INC_DEST_DEPTH    16
#define INC_COPYMODE      INC_SOLID
#define INC_STRETCHMODE   INC_STRETCH
#define INC_COLOURKEY     1
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To16SolidColourKeyStretch */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To24SolidColourKeyStretch */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To32SolidColourKeyStretch */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  16
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    16
#include "L_BLTCPY.cpp"     /* L_BLT_Copy16To16SolidColourKeyStretch */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  24
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy24To24SolidColourKeyStretch */

#undef INC_SOURCE_DEPTH
#define INC_SOURCE_DEPTH  32
#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy32To32SolidColourKeyStretch */

//
//
// =============================================================
//
//          Stretching & Alpha, Raw bitmap copy
//

#undef  INC_SOURCE_DEPTH
#undef  INC_SOURCEFORMAT
#undef  INC_DEST_DEPTH
#undef  INC_COPYMODE
#undef  INC_STRETCHMODE
#undef  INC_COLOURKEY

#define INC_SOURCE_DEPTH  8
#define INC_SOURCEFORMAT  INC_RAWBITMAP
#define INC_DEST_DEPTH    16
#define INC_COPYMODE      INC_ALPHA
#define INC_STRETCHMODE   INC_STRETCH
#define INC_COLOURKEY     0
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To16AlphaStretch */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To24AlphaStretch */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To32AlphaStretch */

#undef  INC_SOURCE_DEPTH
#undef  INC_DEST_DEPTH
#define INC_SOURCE_DEPTH  16
#define INC_DEST_DEPTH    16
#include "L_BLTCPY.cpp"     /* L_BLT_Copy16To16AlphaStretch */

#undef  INC_SOURCE_DEPTH
#undef  INC_DEST_DEPTH
#define INC_SOURCE_DEPTH  24
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy24To24AlphaStretch */

#undef  INC_SOURCE_DEPTH
#undef  INC_DEST_DEPTH
#define INC_SOURCE_DEPTH  32
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy32To32AlphaStretch */


//
//
// =============================================================
//
//          Unity with Holes, Raw bitmap copy
//

#undef  INC_SOURCE_DEPTH
#undef  INC_SOURCEFORMAT
#undef  INC_DEST_DEPTH
#undef  INC_COPYMODE
#undef  INC_STRETCHMODE
#undef  INC_COLOURKEY

#define INC_SOURCE_DEPTH  8
#define INC_SOURCEFORMAT  INC_RAWBITMAP
#define INC_DEST_DEPTH    16
#define INC_COPYMODE      INC_HOLES
#define INC_STRETCHMODE   INC_UNITY
#define INC_COLOURKEY     0
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To16HolesUnity */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To24HolesUnity */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To32HolesUnity */


//
//
// =============================================================
//
//          Stretching & Holes, Raw bitmap copy
//
#if 1
#undef  INC_SOURCE_DEPTH
#undef  INC_SOURCEFORMAT
#undef  INC_DEST_DEPTH
#undef  INC_COPYMODE
#undef  INC_STRETCHMODE
#undef  INC_COLOURKEY

#define INC_SOURCE_DEPTH  8
#define INC_SOURCEFORMAT  INC_RAWBITMAP
#define INC_DEST_DEPTH    16
#define INC_COPYMODE      INC_HOLES
#define INC_STRETCHMODE   INC_STRETCH
#define INC_COLOURKEY     0
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To16HolesStretch */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To24HolesStretch */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "L_BLTCPY.cpp"     /* L_BLT_Copy8To32HolesStretch */
#endif

//
//
// =============================================================
//
//          Solid Colour Filling
//

#undef  INC_SOURCE_DEPTH
#undef  INC_SOURCEFORMAT
#undef  INC_DEST_DEPTH
#undef  INC_COPYMODE
#undef  INC_STRETCHMODE
#undef  INC_COLOURKEY

#define INC_DEST_FORMAT  INC_RAWBITMAP
#define INC_DEST_DEPTH    16
#include "l_bltfil.cpp"     /* L_BLT_Fill16Raw */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    24
#include "l_bltfil.cpp"     /* L_BLT_Fill24Raw */

#undef  INC_DEST_DEPTH
#define INC_DEST_DEPTH    32
#include "l_bltfil.cpp"     /* L_BLT_Fill32Raw */

// Bleeble - add the rest of the variations here...
