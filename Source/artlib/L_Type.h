#ifndef __L_TYPE_H__
#define __L_TYPE_H__

/*****************************************************************************
 *
 * FILE:        L_Type.h
 * DESCRIPTION: Standard small data types header.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Type.h 14    6/11/99 1:38p Davide $
 *
 * Various standard things are defined in L_Type.h.  Depending on the target
 * system, some of them change internally, but you don't need to be aware of
 * it too much.  For example, strings are made of 16 bit characters on the PC
 * and 8 bit on the PSX.  See C_ArtLib.h for the controlling defines.
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Type.h $
 * 
 * 14    6/11/99 1:38p Davide
 * 
 * 13    5/27/99 3:03p Davide
 * 
 * 13		May 27 1999	3:00 Davide
 *  Added a macro for the unicode functions for the getcwd funtion
 *
 * 12    5/19/99 1:16p Fredds
 * Add VC 6.0 pre-processing step to exclude INT32 datatype from
 * compilation
 * 
 * 11    26/04/99 14:52 Timp
 * Another handy define  (Asprintf)
 * 
 * 10    23/04/99 15:32 Timp
 * Added handy A_T define.  (See MSVC _T docs)
 * 
 * 9     4/22/99 14:59 Davew
 * Removed the old DX 5 defines
 * 
 * 8     4/14/99 2:41p Agmsmith
 * Restored corrupted file.
 * 
 * 8     4/05/99 10:33a Lzou
 * 
 * 7     1/25/99 9:16p Agmsmith
 * Can't use unnamed struct fields in C++.
 * 
 * 6     12/18/98 5:41p Agmsmith
 * Abstract version independent DirectDraw type added.
 * 
 * 5     12/10/98 4:50p Agmsmith
 * Switch to DirectX 5.0 surfaces.
 * 
 * 4     10/21/98 10:28a Agmsmith
 * Added L_TYPE_volatile.
 ****************************************************************************/

#include <limits.h>  /* For minimum and maximum values of integers and others. */



/*****************************************************************************
 * The Artech international character.  Can be 16 bits long in Unicode systems,
 * 8 bit in Playstation (which uses Shift-JIS rather than Unicode).  Use the
 * ordinary "char" for strings you know will be 8 bit.
 */

#if CE_ARTLIB_UnicodeAlphabet
  #define ACHAR wchar_t
  #define Astrlen wcslen
  #define AStrByteSize(x) (wcslen (x) * 2 + 2)  /* Size of string in bytes including NUL. */
  #define Astrcpy wcscpy
  #define Astrncpy wcsncpy
  #define Astrcat wcscat
  #define Astrncat wcsncat
  #define Astrcmp wcscmp
  #define Astricmp wcsicmp
  #define A_T(x) L ## x
  #define Asprintf swprintf
	#define Agetcwd	_wgetcwd
  #define Afgets fgetws
#else /* Just stuck with ASCII. */
  #define ACHAR char
  #define Astrlen strlen
  #define AStrByteSize(x) (strlen (x) + 1)  /* Size of string in bytes including NUL. */
  #define Astrcpy strcpy
  #define Astrncpy strncpy
  #define Astrcat strcat
  #define Astrncat strncat
  #define Astrcmp strcmp
  #define Astricmp stricmp
  #define A_T(x) x
  #define Asprintf sprintf
	#define Agetcwd	_getcwd
	#define Afgets fgets 
#endif



/*****************************************************************************
 * Standard number types that should be portable.
 */

/* 1 bit numbers */
#ifndef TRUE
  typedef unsigned char BOOL, *BOOLPointer; /* Small storage space big enough to hold 0 and 1 */
  #define TRUE 1
  #define FALSE 0
#endif


/* 8 bit numbers */
typedef unsigned char   UNS8, *UNS8Pointer;
typedef signed char     INT8, *INT8Pointer;
#define INT8_MAX SCHAR_MAX
#define INT8_MIN SCHAR_MIN
#define UNS8_MAX UCHAR_MAX
#ifndef BYTE
  #define BYTE UNS8
#endif

/* 16 bit numbers */
typedef unsigned short  UNS16, *UNS16Pointer;
typedef signed short    INT16, *INT16Pointer;
#define INT16_MAX SHRT_MAX
#define INT16_MIN SHRT_MIN
#define UNS16_MAX USHRT_MAX
#ifndef WORD
  #define WORD UNS16
#endif

/* 32 bit numbers */
typedef unsigned long   UNS32, *UNS32Pointer;

#if ! (_MSC_VER == 1200)
	typedef signed long     INT32, *INT32Pointer;
#endif

#define INT32_MAX LONG_MAX
#define INT32_MIN LONG_MIN
#define UNS32_MAX ULONG_MAX
#ifndef DWORD /* Doubleword: 4 bytes */
  #define DWORD UNS32
#endif

/* 64 bit numbers */
#if 1
  typedef unsigned __int64  UNS64, *UNS64Pointer;
  typedef signed __int64    INT64, *INT64Pointer;
  #define INT64_MAX _I64_MAX
  #define INT64_MIN _I64_MIN
  #define UNS64_MAX _UI64_MAX
  #ifndef QWORD /* Quadword: 8 bytes */
    #define QWORD UNS64
  #endif
#endif

/* 128 bit numbers */
#if 0
  typedef unsigned __int128  UNS128, *UNS128Pointer;
  typedef signed __int128    INT128, *INT128Pointer;
  #define INT128_MAX _I128_MAX
  #define INT128_MIN _I128_MIN
  #define UNS128_MAX _UI128_MAX
  #ifndef OWORD /* Octoword: 16 bytes */
    #define OWORD UNS128
  #endif
#endif

/* Some error checking for when you switch compilers or processors. */
#if ! (UNS8_MAX == 255)
	#error "Improperly declared UNS8"
#endif
#if ! (UNS16_MAX == 0xffff )
	#error "Improperly declared UNS16"
#endif
#if ! (UNS32_MAX == 0xffffffff )
	#error "Improperly declared UNS32"
#endif



/*****************************************************************************
 * Time is measured in ticks.  Each tick is 1/60 second or 1/50 depending on
 * PAL or NTSC video modes (see CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ).  This sets
 * the upper limit on video update rates and input device sampling rates, but
 * 60hz should be good enough.  It's also better to set the video refresh rate
 * to 60 too if you are running the engine at 60 (don't bother if the engine
 * is slower).  An unsigned short (16 bits) can hold a duration of up to 18
 * minutes.  Unsigned 32 bits lasts a bit over 2.25 years.
 */

typedef UNS32 TYPE_Tick, *TYPE_TickPointer;
  /* Unsigned so that wrapping around is safer. */



/*****************************************************************************
 * Various types for use in 2D and 3D.  Things like angles, positions,
 * matrices and the like, but initially only those ones that are used
 * in function declarations.
 */

#if CE_ARTLIB_PlatformWin95

  #define TYPE_Coord2D LONG
  #define TYPE_Coord2DPointer LPLONG
    /* In the IBM PC clone 2D system coordinates are long integer values. */

  typedef float TYPE_Angle2D, *TYPE_Angle2DPointer;
  #define TYPE_FULL_CIRCLE_2D_ANGLE 6.2831853F
  #define TYPE_HALF_CIRCLE_2D_ANGLE 3.1415926F
  #define TYPE_QUARTER_CIRCLE_2D_ANGLE 1.5707963F
    /* Angles in 2D are a float in radian units (360 degrees
    is 2 * pi radians) on the PC.  Positive rotations go clockwise. */

  typedef float TYPE_Scale2D, *TYPE_Scale2DPointer;
  #define TYPE_UnitScale2D 1.0F
    /* For scaling things in the 2D world.  This is used for magnification
    and zooming out control.  A scale factor of 1.0 means keep the size
    the same; 2.0 makes something twice as big; 0.5 makes it smaller, half
    size to be precise. */

  #define TYPE_Point2D POINT
  #define TYPE_Point2DPointer LPPOINT
    /* A two dimensional position or offset value.  Has fields "x" and "y"
    which are long integers.  X is for left to right (increases to the right),
    Y is top to bottom (increases going downwards). */

  #define TYPE_Rect RECT
  #define TYPE_RectPointer LPRECT
    /* Has "left", "right", "top", "bottom" long integer fields.  The rectangle
    width is right - left, which means that pixels have X coordinates from left
    up to (right - 1), but it doesn't include the pixels with X=right.  Same sort
    of thing for the Y direction, pixels are from top to (bottom - 1). */

  typedef float TYPE_2DMatrixValue, *TYPE_2DMatrixValuePointer;
  #define TYPE_2DMatrixUnitValue 1.0F
    /* Matrices for containing 2D transformations have this type as their
    basic element.  Needs to be able to represent fractional numbers, such
    as the ones returned from Sine and Cosine functions. */

  typedef struct TYPE_Matrix2DStruct
  {
    TYPE_2DMatrixValue  _11, _12, _13;
    TYPE_2DMatrixValue  _21, _22, _23;
    TYPE_2DMatrixValue  _31, _32, _33;
  } TYPE_Matrix2D, *TYPE_Matrix2DPointer;
    /* A 3x3 matrix structure.  Usually floating point values.  Named as
    "_RC" where R is the row number from 1 to 3 and C is the column, also
    from 1 to 3.  Homogenious coordinates are used, so you can put in a
    translation as well as rotations and scaling. */

  #define TYPE_Coord3D        D3DVALUE
  #define TYPE_Coord3DPointer LPD3DVALUE
    /* Single scalars used for describing coordinates in the 3D system.  The PC
    uses 32 bit floats, which aren't inefficient with circa 1998 processors. */

  typedef D3DVALUE TYPE_Angle3D, *TYPE_Angle3DPointer;
  #define TYPE_FULL_CIRCLE_3D_ANGLE 6.2831853F
  #define TYPE_HALF_CIRCLE_3D_ANGLE 3.1415926F
  #define TYPE_QUARTER_CIRCLE_3D_ANGLE 1.5707963F
    /* Angles in 3D are a float in radian units (360 degrees
    is 2 * pi radians) on the PC.  A left handed coordinate
    system is used, which means that a positive rotation around
    an axis goes clockwise when looking down the axis from
    positive coordinates towards the origin. */

  typedef D3DVALUE TYPE_Scale3D, *TYPE_Scale3DPointer;
  #define TYPE_UnitScale3D 1.0F
    /* For scaling up and down things in the 3D world. */

  #define TYPE_Point3D        D3DVECTOR
  #define TYPE_Point3DPointer LPD3DVECTOR
    /* A 3 element vector with fields of TYPE_COORD.  The individual
    elements are named "x", "y" and "z".  A left handed coordinate
    system is used, which means that X increases to the right, Y
    increases going up and Z increases as you go into the screen. */

  typedef D3DVALUE TYPE_3DMatrixValue, *TYPE_3DMatrixValuePointer;
  #define TYPE_3DMatrixUnitValue 1.0F
    /* Matrices for containing 3D transformations have this type as their
    basic element.  Needs to be able to represent fractional numbers, such
    as the ones returned from Sine and Cosine functions. */

  #define TYPE_Matrix3D        D3DMATRIX
  #define TYPE_Matrix3DPointer LPD3DMATRIX
    /* A 4x4 matrix structure.  Usually floating point values.  Named as
    "_RC" where R is the row number from 1 to 4 and C is the column, also
    from 1 to 4.  Here is how Direct3D declares it:
    typedef struct _D3DMATRIX {
        D3DVALUE _11, _12, _13, _14;
        D3DVALUE _21, _22, _23, _24;
        D3DVALUE _31, _32, _33, _34;
        D3DVALUE _41, _42, _43, _44;
    } D3DMATRIX, *LPD3DMATRIX;
    We will use similar naming for the other types of computer. */

#elif CE_ARTLIB_PlatformPSX
  /* Bleeble - PSX to do. */
#endif



/*****************************************************************************
 * Rename our Direct Draw surface stuff to whatever is selected, since there
 * are so many DirectDraw versions and we want things to compile sometimes
 * with one version (DirectX 3 for NT 4 compatibility) and sometimes with
 * another version (DirectX 5 for 3D graphics).  Or a later version.
 */


//#define LPDDRAWN      LPDIRECTDRAW2         /* DirectX 5.0 */
//#define LPDDSURFN     LPDIRECTDRAWSURFACE3  /* DirectX 5.0 */
//#define LPDDTEXTUREN  LPDIRECT3DTEXTURE2    /* DirectX 5.0 */



/*****************************************************************************
 * In the multitasking version of the library, some variables need to be
 * declared as volatile (so that the compiler doesn't optimize away load
 * and save instructions which would confuse the other threads trying to
 * look at the current state of the variable).  But we don't want it to
 * be volatile in the non-multitasking version (wastes instructions reloading
 * and resaving it when it could hold the variable in a register).
 */

#if CE_ARTLIB_EnableMultitasking
  #define L_TYPE_volatile volatile
#else // Not multitasking, convert the L_TYPE_volatile symbol into blanks.
  #define L_TYPE_volatile
#endif



/*****************************************************************************
 * Some Artech file format descriptions.
 */

// UCP - Uncommon palette bitmap struct.  This is what a TAB gets converted
// to by TAB2UCP.  Basically an 8 bit image with a native mode palette
// (palette converted to screen depth usage upon loading).
// 
// The palette seems to be interleaved with native bits at even indices and
// the corresponding alpha at odd indices.  The palette entries are BGR0 in
// increasing byte order in the file before conversion to native, which
// means blue in the low byte of an Intel format DWORD, then green, then
// red and zero in the high byte.  The alpha entries are in the low byte of
// a DWORD, in memory you see an alpha byte followed by 3 zeroes.  To save
// on computation, only 17 different alpha values are distinguished, see
// the ALPHA_* defines below (you can use other values, but the low bits
// will be dropped because it looks up the alpha in a table and calls a
// routine for each particular alpha value, rather than doing the multiply
// each time).  Also, the palette colours are pre-multiplied by the alpha
// value to save runtime computation (so a 50% transparent red has a RGB
// entry of (127,0,0) rather than the 255,0,0 that you would expect).
// 
// Colour index zero is always fully transparent.  If the index is < nAlpha
// then its alpha value is used for controling the level of transparency.
// If the 8 bit pixel index is >= nAlpha then it is drawn as solid colour.
// 
// The OriginOffset is used for showing where this particular bitmap is in
// the bigger mostly transparent bitmap that the whole animation is drawn
// in (think of it as being the size of the sheet of paper that was used by
// the artist to draw the cartoon and our TAB being the sub-rectangle part
// which all the actual pencil markings fit in).

#define BITMAP_NOTRANSPARENCY   0x01  // Set if colour zero is solid and no alpha.
#define BITMAP_ALPHACHANNEL     0x02  // Set if this bitmap has alpha (nAlpha > 1).
#define BITMAP_NOTRECOLORED     0x04  // A runtime flag for recolouring ponies.

#define ALPHA_TRANSPARENT       ((0*255)/16)
#define ALPHA_OPAQUE06_25       ((1*255)/16)
#define ALPHA_OPAQUE12_50       ((2*255)/16)
#define ALPHA_OPAQUE18_75       ((3*255)/16)
#define ALPHA_OPAQUE25_00       ((4*255)/16)
#define ALPHA_OPAQUE31_25       ((5*255)/16)
#define ALPHA_OPAQUE37_50       ((6*255)/16)
#define ALPHA_OPAQUE43_75       ((7*255)/16)
#define ALPHA_OPAQUE50_00       ((8*255)/16)
#define ALPHA_OPAQUE56_25       ((9*255)/16)
#define ALPHA_OPAQUE62_50       ((10*255)/16)
#define ALPHA_OPAQUE68_75       ((11*255)/16)
#define ALPHA_OPAQUE75_00       ((12*255)/16)
#define ALPHA_OPAQUE81_25       ((13*255)/16)
#define ALPHA_OPAQUE87_50       ((14*255)/16)
#define ALPHA_OPAQUE93_75       ((15*255)/16)
#define ALPHA_OPAQUE100_0       ((16*255)/16)

// Note that you may also use a full 255 alpha levels
// instead of the above 16 alpha levels. For the new
// 255 alpha levels, see CE_ARTLIB_BlitUse255AlphaLevels
// define in your C_ArtLib.h.

typedef struct LE_BLT_AlphaPaletteEntryStruct
{
  union ColourUnion
  {
    struct ByteColourStruct
    {
      UNS8 blue;
      UNS8 green;
      UNS8 red;
      UNS8 reserved;
    } colours;
    DWORD dwColour;
  } lowDWORD;

  DWORD dwAlpha; // From 0 to 255.
} LE_BLT_AlphaPaletteEntryRecord, *LE_BLT_AlphaPaletteEntryPointer;

#define NEWBITMAP_COLOUR_ENTRY_SIZE  8  /* Number of bytes in one colour entry. */

#pragma warning(disable:4200) /* Disable warning about zero size arrays being an extension. */
typedef struct NewBitmapHeaderStruct
{
  UNS16 nXBitmapWidth;    // bitmap width in pixels
  UNS16 nYBitmapHeight;   // bitmap height in pixels
  INT16 nXOriginOffset;   // X offset to bitmap origin from ul corner in pixels
  INT16 nYOriginOffset;   // Y offset to bitmap origin from ul corner in pixels
  DWORD dwFlags;          // flags: see BITMAP_* above
  UNS16 nColors;          // number of entries in color table, 8 bytes each.
  UNS16 nAlpha;           // number of entries in color table that have alpha channel
  DWORD dwColorTable[0];  // color table, size is 8 bytes * nColors.
  //BYTE  cBitmapData[0]; // 8 bit bitmap data padded to 4 bytes per line
} NEWBITMAPHEADER, *LPNEWBITMAPHEADER;
#pragma warning(default:4200)


#endif /* __L_TYPE_H__ */
