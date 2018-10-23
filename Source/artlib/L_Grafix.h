#ifndef __L_GRAFIX_H__
#define __L_GRAFIX_H__

/*****************************************************************************
 * FILE:        L_Grafix.h
 *
 * DESCRIPTION: Graphics system.  Routines for handling bitmaps and accessing
 *              the screen (the BLT module is conceptually a part of this).
 *              There is also some glue here for using PC3D for 3D modes.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Grafix.h 24    6/07/99 3:03p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Grafix.h $
 * 
 * 24    6/07/99 3:03p Agmsmith
 * Rearranged init code so more stuff shared between 2D and 3D, added a
 * function for converting rectangles from virtual to real screen
 * coordinates.
 *
 * 23    5/31/99 15:32 Davew
 * Fixed the 3D version of the ChangeVideoMode function so it now properly
 * reinitializes the global dimensioning variables dealing with the
 * primary and rendering surfaces.
 *
 * 22    5/08/99 12:27p Agmsmith
 * Can now explicitly ask for full screen or windowed, or ask for the
 * default based on the .INI file.
 *
 * 21    4/25/99 1:06p Agmsmith
 * Updated 2D DirectDraw to work with Direct X 6.1 and DaveW's header
 * file.
 *
 * 20    4/22/99 15:00 Davew
 * Updated to DX v6
 *
 * 19    4/11/99 3:48p Agmsmith
 * Now postload converts BMPs into native screen depth bitmaps, rather
 * than having the sequencer do it.  So remove all BMP things everywhere.
 *
 * 18    3/29/99 5:04p Agmsmith
 * Export the visible window region clipper.
 *
 * 17    3/29/99 3:30p Agmsmith
 * Added a second clipper just for keeping track of the window's visible
 * region.
 *
 * 16    3/28/99 5:29p Agmsmith
 * Move copying of working screen to main screen into graphics module, and
 * add clipping to window's visible region so it doesn't trash other
 * things on the screen.
 *
 * 15    3/24/99 7:50p Timp
 * Added LE_GRAFIX_MakeColorRefAlpha/LEG_MCRA macros.
 *
 * 14    12/30/98 4:51p Agmsmith
 * Better naming - virtual screen size separate from real screen size.
 *
 * 13    12/30/98 11:10a Agmsmith
 * Now does source and destination rectangle setup better, and makes the
 * working surface the requested size even though the real screen may be
 * different.
 *
 * 12    12/26/98 3:00p Agmsmith
 * Now can restore GBMs, and lock and unlock them.
 *
 * 11    12/24/98 3:40p Agmsmith
 * Took out some unused generic bitmap flags.
 *
 * 10    12/23/98 4:27p Agmsmith
 * Code for creating a generic bitmap from a BMP or TAB under construction
 * (compiles but not finished or tested).
 *
 * 9     12/20/98 7:27p Agmsmith
 * Working on loading generic bitmaps from BMPs and UAPs.
 *
 * 8     12/18/98 5:42p Agmsmith
 * New global names, cleaned up graphics init code.
 ****************************************************************************/

/****************************************************************************/
/* C O N S T A N T S   A N D   T Y P E S                                    */
/*--------------------------------------------------------------------------*/

#define LE_GRAFIX_ObjectTransparent     FALSE
#define LE_GRAFIX_ObjectNotTransparent  TRUE
// Used to specify if an object (native screen depth bitmap) is to
// be transparent or not and therefore how to treat the pure green pixels.


#define LE_GRAFIX_MakeColorRef(r,g,b) ((((b) & 255) << 16) | (((g) & 255) << 8) | ((r) & 255))
#define LEG_MCR(r,g,b)                LE_GRAFIX_MakeColorRef(r,g,b)
// Short and long forms for a macro that converts 3 red, green, blue
// values into a 32 bit ColorRef.

#define LE_GRAFIX_MakeColorRefAlpha(r,g,b,a) ((((a) & 255) << 24) | (((b) & 255) << 16) | (((g) & 255) << 8) | ((r) & 255))
#define LEG_MCRA(r,g,b,a)                    LE_GRAFIX_MakeColorRefAlpha(r,g,b,a)
// Short and long forms for a macro that converts 4 red, green, blue, alpha
// values into a 32 bit ColorRef.


typedef enum LE_GRAFIX_DepthInBytesEnum
{
  LE_GRAFIX_BAD_DEPTH = 0,
  LE_GRAFIX_8_DEEP = 1, /* Coincidentally on purpose this is the number of bytes per pixel. */
  LE_GRAFIX_16_DEEP,
  LE_GRAFIX_24_DEEP,
  LE_GRAFIX_32_DEEP,
}
#if !CE_ARTLIB_UseEnumDataTypes
; typedef UNS8  /* For compilers which don't like enums, use an int instead. */
#endif
LE_GRAFIX_DepthInBytesValue;


#define LE_GRAFIX_8_DEEP_BIT (1 << LE_GRAFIX_8_DEEP)
#define LE_GRAFIX_16_DEEP_BIT (1 << LE_GRAFIX_16_DEEP)
#define LE_GRAFIX_24_DEEP_BIT (1 << LE_GRAFIX_24_DEEP)
#define LE_GRAFIX_32_DEEP_BIT (1 << LE_GRAFIX_32_DEEP)

typedef UNS8 LE_GRAFIX_DepthSet;
  // A bitset using things like LE_GRAFIX_16_DEEP_BIT | LE_GRAFIX_24_DEEP_BIT.

typedef enum LE_GRAFIX_VideoMemoryTypeEnum
{
  LE_GRAFIX_LOCAL_VIDEO_MEMORY = (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM),
  LE_GRAFIX_NONLOCAL_VIDEO_MEMORY /* Otherwise known as AGP memory */ = (DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM),
  LE_GRAFIX_SYSTEM_MEMORY = DDSCAPS_SYSTEMMEMORY
}
#if !CE_ARTLIB_UseEnumDataTypes
; typedef DWORD  /* For compilers which don't like enums, use an int instead. */
#endif
LE_GRAFIX_VideoMemoryType;


//----------------------------------------------------------------------------
// The In-memory generic bitmap data structure.  Works for all depths and even
// can refer to DirectDraw surfaces (for the real screen and 3D textures).

#define LE_GRAFIX_GBM_FLAGS_GLOBALIMAGEALPHA        0x0001
  // This flag is set if the bitmap should be drawn with an overall single
  // alpha transparency level from the globalAlphaLevel field applied on
  // top of other transparency options implied by the DirectDraw surface
  // settings (which contains alpha and colour key etc).

#define LE_GRAFIX_GBM_FLAGS_NATIVEPIXELS            0x0002
  // Pixel bits match the screen format in both depth and the red, green
  // and blue bit shifts.  Of course, if you change the video mode, this
  // won't work right.

#define LE_GRAFIX_GBM_FLAGS_TEXTURE                 0x0004
  // The bitmap is also a special texture, usable by the 3D rendering
  // system.  On the PC this means the directdraw surface is allocated
  // a bit differently than for a plain bitmap.

#define LE_GRAFIX_GBM_FLAGS_RENDERSURFACE           0x0008
  // The bitmap is also useable as a 3D renderer, you can get the system
  // (hardware or software) to draw on it.  This also influences its
  // allocation.

#define LE_GRAFIX_GBM_FLAGS_ORIGINALLY_BMP_24       0x0010
  // This flag is set if the original data came from a 24 bit BMP file,
  // used by the data reloading code so that it doesn't have to reparse
  // the BMP file header (or even read it in).

#define LE_GRAFIX_GBM_FLAGS_ORIGINALLY_BMP_8        0x0020
  // This flag is set if the original data came from an 8 bit BMP file.

#define LE_GRAFIX_GBM_FLAGS_ORIGINALLY_UAP          0x0040
  // Used to mark the original data as being from a UAP (8 bit with
  // alpha in the palette) image.


typedef struct LE_GRAFIX_GenericBitmapStruct
{
  PDDSURFACE directDrawSurfacePntr;
    // Points to the DirectDraw surface that actually contains the bitmap
    // data (making this structure just a wrapper).  Locked and unlocked
    // in parallel with the locking and unlocking of this generic bitmap.
    // If the image has a palette, a DirectDrawPalette is allocated and
    // attached to the surface.  Destroying the surface will then destroy
    // the palette automatically, and you can get access to the palette
    // through the surface.  You can also find out the exact colour
    // masks and other things about the surface in the usual way.

  PD3DTEXTURE directDrawTexturePntr;
    // If this is the kind of bitmap which is a texture, then this will
    // be non-NULL and have a pointer to the texture interface corresponding
    // to the DirectDraw surface interface (same underlying object).

#if DIRECTDRAW_VERSION < 0x0600
  HRESULT directDrawTextureHandle;
    // DirectX 5.0 also needs a texture handle to access the texture,
    // later versions use the texture pointer directly.  NULL if the
    // texture isn't ready, also needs to be associated with a
    // D3DDevice to get the handle.
#endif

  UNS8 *bitmapBits;
    // Points to the raw bits in the bitmap.  Presumably it is a bunch of
    // scan lines in order from the top down to the bottom.  Each scan line
    // is bytesToNextScanLine long, though it may only use the first part
    // for actual pixels and the rest of the line for padding.  This will
    // be NULL if the surface isn't locked (unless we implement bitmaps
    // which aren't surfaces).  If it is locked, it will be a pointer to
    // the bits in the surface obtained from the lock function.

  UNS32 bytesToNextScanLine;
    // Bitmap "pitch", add this number of bytes to a pixel pointer to move
    // down one scan line in the image.  This includes the padding bytes
    // at the end of a scan line (usually they are padded to a multiple of
    // 4 bytes so that the next scan line starts on a DWORD address boundary
    // which is easier for 32 bit hardware to read from).  Sometimes, it
    // will have huge amounts of padding, not just to a multiple of four,
    // for DirectDraw screen surfaces (see DDSURFACEDESC) due to video
    // memory weirdness.

  UNS16 flags;
    // A binary combination of flag bits from LE_GRAFIX_GBM_FLAGS.

  UNS16 width;
  UNS16 height;
    // Bitmap width and height in pixel units.  Note that this is the true
    // width in pixels, not including pad bytes which pad out each scan line
    // to a multiple of 4 bytes (or larger - see bytesToNextScanLine).

  UNS8 bitsPerPixel;
    // Currently we support 8, 24 and 32 for non-native and
    // 16, 24 and 32 for native (screen) depths.

  UNS8 globalAlphaLevel;
    // If the LE_GRAFIX_GBM_FLAGS_GLOBALIMAGEALPHA flag is on, then this
    // is the alpha transparency level to apply to the whole bitmap on
    // top of the other transparency options (useful for fading in or out
    // an image).

  UNS16 originalDataBitmapStartOffset;
    // Offset from the start of the original data where the bitmap
    // image starts.  This is used for reloading the original data
    // into the surface - by skipping past this part of the data
    // and loading the rest directly into the surface (if possible).
    // The amount to reload can be determined from the width and
    // height and original depth of the bitmap.

} LE_GRAFIX_GenericBitmapRecord, LEG_GBMR, *LE_GRAFIX_GenericBitmapPointer, *LEG_GBMP;



/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

extern PDDRAW LE_GRAFIX_DirectDrawObjectN;
  // DirectDraw object corresponding to the video board we are using.
  // The N means that it is in some variable version of DirectDraw,
  // see L_TYPE.H for the version being used.

extern PDDSURFACE LE_GRAFIX_DDPrimarySurfaceN;
  // DirectDraw surface which represents the real screen memory,
  // on the video board.  Also see LE_GRAFIX_ScreenGBM.

extern LE_DATA_DataId LE_GRAFIX_ScreenGBM;
  // A wrapper around the DirectDraw screen memory, LE_DATA_EmptyItem
  // if system isn't open.  You can get a pointer to the screen memory
  // by locking the GBM.

extern PDDCLIPPER LI_GRAFIX_WindowVisibleRegionClipper;
  // This is used only for getting the visible region from the window.
  // Initialised when this module starts up, deallocated when it shuts down.

extern PDDSURFACE LE_GRAFIX_DDWorkingSurfaceN;
  // DirectDraw surface for the compositing area, same size as the
  // screen but usually in system memory (except in 3D mode when it
  // may have to be video so that the hardware can render to it).
  // NULL if not used (some games don't use the work area).  Also
  // see LE_GRAFIX_WorkGBM.

extern LE_DATA_DataId LE_GRAFIX_WorkGBM;
  // Wrapper around the working memory DirectDraw surface,
  // LE_DATA_EmptyItem if it doesn't exist.

extern BOOL LE_GRAFIX_WindowedMode;
  // TRUE if currently running in windowed mode, FALSE if we have the
  // full screen (and can thus switch video modes etc).  Set by the
  // screen init code.

extern LE_GRAFIX_VideoMemoryType LE_GRAFIX_VideoMemoryForGBMTextures;
  // What kind of video memory to use when creating textures
  // (8 bit images, maybe with alpha too).  Generally this gets
  // set when the library is initialised for a software or a
  // hardware renderer.

extern LE_GRAFIX_VideoMemoryType LE_GRAFIX_VideoMemoryForGBMPictures;
  // What kind of video memory to use when creating generic pictures
  // (usually true colour images at current screen depth).

extern BOOL LE_GRAFIX_SlowScrolling;
  // TRUE if slow scrolling hardware was detected (video memory to same video
  // memory blit is slow).  Some workarounds for not scrolling the screen will
  // be used.

#if CE_ARTLIB_EnableMultitasking
extern LPCRITICAL_SECTION LE_GRAFIX_LockedSurfaceCriticalSectionPntr;
  // Provides mutual exclusion around the locking of any surface,
  // so that other threads wait for the one doing drawing to finish.
  // Note that all surfaces share the same critical section, so only
  // one thread can do any drawing at a time.  The alternative would
  // be to have a critical section in every generic bitmap.  Unfortunately
  // the DirectDraw lock operation doesn't use critical sections - it
  // busy waits until the surface is unlocked by another thread.
  // NULL if the graphics system is shut down.
#endif

extern RECT LE_GRAFIX_ScreenSourceRect;
extern RECT LE_GRAFIX_ScreenDestinationRect;
  // The image you see is taken from the source rectangle, in virtual screen
  // coordinates and copied to the destination rectangle in actual screen
  // coordinates.  The virtual screen has coordinates (0,0) at the top left
  // and (LE_GRAFIX_VirtualScreenWidthInPixels, LE_GRAFIX_VirtualScreenHeightInPixels) in
  // the bottom right corner.  Note that coordinates used are for the grid
  // between the pixels, so a pixel at (X,Y) is really the teensy-weensy
  // rectangle between (X,Y) and (X+1, Y+1).  This past-the-edge coordinate
  // usage helps avoid +1 and -1 problems when finding the width of a
  // rectangle; it is just RightX - LeftX.  Anyway, back to the virtual
  // screen.  The sequencer and other systems use the full virtual screen
  // when doing their drawing, and quite often it is exactly the same as
  // the real screen.  However, you can display a portion of the virtual
  // screen by making the SourceRect smaller, and scroll around in your
  // virtual world by moving the SourceRect around.  This is separate from
  // sequencer camera moves, which do another kind of scrolling.  If the
  // DestRect is smaller or larger than the SourceRect then the image in
  // the virtual screen SourceRect area is blown up or shrunk to fit into
  // the real screen's DestRect.  To avoid roundoff glitches, the destination
  // width and height should be integer multiples of the source width and
  // height (each pixel gets blown up into N pixels on the real screen).

extern int LE_GRAFIX_VirtualScreenWidthInPixels;
extern int LE_GRAFIX_VirtualScreenHeightInPixels;
  // Size of your virtual screen.  Usually the same as the real screen size
  // which you actually got (may only be close to your requested values
  // (CE_ARTLIB_InitialScreenWidth etc) if the user's hardware doesn't
  // support your exact requested size).  All drawing internally is done
  // in this virtual screen area, stuff outside it is clipped.  This is
  // also the size of the work area bitmap, which doesn't have to be the
  // same size as the real screen.

extern int LE_GRAFIX_RealScreenWidthInPixels;
extern int LE_GRAFIX_RealScreenHeightInPixels;
extern int LE_GRAFIX_ScreenBitsPerPixel;
  // Size of the real hardware screen.  The LE_GRAFIX_ScreenDestinationRect
  // must be fully contained in this area otherwise things won't work.
  // Note that you may get a different size than you asked for.



/************************************************************/

/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

//----------------------------------------------------------------------------
// System control functions.

extern void LI_GRAFIX_RemoveSystem(void);

extern BOOL LE_GRAFIX_InitSystem (int ScreenWidth, int ScreenHeight,
  BOOL AcceptLargerSizes, BOOL AcceptSmallerSizes,
  LE_GRAFIX_DepthInBytesValue DesiredDepthValue,
  LE_GRAFIX_DepthSet AcceptableDepthSet,
  BOOL HaltOnError, WindowedMode nWindowMode = WINDOWED_USE_DEFAULT);

extern BOOL LE_GRAFIX_ChangeVideoMode (int ScreenWidth, int ScreenHeight,
  BOOL AcceptLargerSizes, BOOL AcceptSmallerSizes,
  LE_GRAFIX_DepthInBytesValue DesiredDepthValue,
  LE_GRAFIX_DepthSet AcceptableDepthSet,
  int WorkWidth, int WorkHeight, BOOL HaltOnError);

extern BOOL LI_GRAFIX_PostSurfaceInit(BOOL bTestForSlowScrolling);

extern void LE_GRAFIX_InvalidateRect (RECT *RectPntr);
// Invalidates the given rectangle in screen coordinates.  Usually used by
// the window refresh code to redraw the screen after a switch to some other
// program.  You can use NULL to redraw the whole screen.

void LE_GRAFIX_ConvertRectanglesToScreenCoordinates (
  LPRECT RectangleArray, int NumberOfRectangles);
// Convert from virtual (work screen) to screen (real screen) coordinates.

extern BOOL LE_GRAFIX_CopyWorkAreaToOutputArea (HRGN CopyPixelsInThisRegion);

extern void LE_GRAFIX_TestForSlowScrolling (void);
// Does a 1 second long scroll test.

extern void LE_GRAFIX_SetRGB_Tran_Value(short R, short G, short B);
// Sets the value of R_TRANS_VALUE, G_TRANS_VALUE and B_TRANS_VALUE to affect
// what the greenishblt functions assume is a transparent pixel.

extern void LE_GRAFIX_PointLogicalToDisplay(LPPOINT lpPoint);
// Converts a POINT from logical (or virtual screen) to display coordinates.

extern void LE_GRAFIX_RectLogicalToDisplay(LPRECT lpRectangle);
// Converts a RECT from logical to display coordinates.

extern void LE_GRAFIX_PointDisplayToLogical(LPPOINT lpPoint);
// Converts a POINT from display to logical coordinates.

extern void LE_GRAFIX_RectDisplayToLogical(LPRECT lpRectangle);
// Converts a RECT from display to logical coordinates.

//----------------------------------------------------------------------------
// "Object" bitmap functions.  These are native screen depth bitmaps.

extern LPBYTE LE_GRAFIX_LockObjectDataPtr(LE_DATA_DataId object_handle);
extern void LE_GRAFIX_UnlockObjectDataPtr(LE_DATA_DataId object_handle);

extern LE_DATA_DataId LE_GRAFIX_ObjectCreate(short w, short h, WORD transp);
// This function creates an object (native screen mode bitmap) of the given
// width and height.  How pure green pixels are treated is dependent on the
// setting of the transp flag.  A handle is returned by this function and
// that handle is later used as an input for any function that requires an
// object_handle.  Remember you must deallocate the handle later (with
// LE_DATA_FreeRuntimeDataID) to free the memory used by the object when
// you are finished with it.

extern LE_DATA_DataId LE_GRAFIX_ObjectCreateAlpha(short w, short h,
  WORD transp, short ALPHA_TRANS_VALUE);
// The same as the above function except a new parameter ALPHA_TRANS_VALUE
// is required.  This parameter sets the alpha channel value for the object
// so the whole thing is semi-transparent.  Legal alpha channel values are
// ALPHA_TRANSPARENT and ALPHA_OPAQUE06_25 to ALPHA_OPAQUE100_0.

extern LE_DATA_DataId LE_GRAFIX_CopyObject(WORD Transparency,
  LE_DATA_DataId SourceObject);
// Given an object makes a copy of it.  Returns LE_DATA_EmptyItem on failure.
// Users must free the memory when done, same as with other object create
// functions.

extern void LE_GRAFIX_ChangeObjectAlpha(LE_DATA_DataId object_handle,
  short new_ALPHA_TRANS_VALUE);
// Function changes the alpha trans value of an object.  Programmer must
// force the update of display for this object to show the change.

extern WORD LE_GRAFIX_ReturnObjectWidth(LE_DATA_DataId object_handle);
extern WORD LE_GRAFIX_ReturnObjectHeight(LE_DATA_DataId object_handle);
extern DWORD LE_GRAFIX_ReturnObjectTransparency(LE_DATA_DataId object_handle);
// These functions return various info about an object that was created earlier.

extern void LE_GRAFIX_ShowObject(LE_DATA_DataId dest_object_handle,
  short x, short y, LE_DATA_DataId src_object_handle);
// Code that will display an object to the background or on to another
// object depending on the value used for dest_object_handle.

extern void LE_GRAFIX_ColorArea(LE_DATA_DataId object_handle,
  short x, short y, short w, short h, DWORD color);
// This function colours a rectangle of the given dimensions with the color
// passed in.  The color is a 32 bit colorref value (see
// LE_GRAFIX_MakeColorRef(r,g,b)).  If object_handle is LE_DATA_EmptyItem the
// coloring will take place on the current display, other wise it will take
// place on the given object.  Make sure the dimensions of the object are
// large enough for the ColorArea dimensions.  The display location is
// relative to the top left corner of the destination.

extern void LE_GRAFIX_ShowTCB(LE_DATA_DataId object_handle,
  short x, short y, LE_DATA_DataId logo_tag);
// This function displays a TCB data item either to the current display or
// to an object (depending on whether object_handle is LE_DATA_EmptyItem).
// The display location is relative to the top left corner of the destination.

extern void LE_GRAFIX_ShowTCB_Alpha(LE_DATA_DataId object_handle,
  short dest_x, short dest_y, LE_DATA_DataId TCB_tag);
// As above but preserves alpha channel if there is one.

//----------------------------------------------------------------------------
// Miscellaneous graphics functions.  Some obsolete.

extern short LE_GRAFIX_TCBwidth(LE_DATA_DataId tcb_handle);
extern short LE_GRAFIX_TCBheight(LE_DATA_DataId tcb_handle);
// These functions return the width and height respectively for a TCB data item.

extern short LE_GRAFIX_AnyBitmapHeight (LE_DATA_DataId DataID);
extern short LE_GRAFIX_AnyBitmapWidth (LE_DATA_DataId DataID);
// These return the width and height of any bitmap, including FUL,
// TAB and dynamic "Object" bitmaps.  Returns zero if it fails.

extern LE_DATA_DataId LE_GRAFIX_StoreArea(short x, short y, short w, short h);
// This function stores the given rect from the screen in an
// allocated block of memory that can later be restored with
// LE_GRAFIX_RetrieveArea().  Users must free the allocated memory
// (indicated by the DataId returned).  If this function fails the
// return value will be equal to LE_DATA_EmptyItem.

extern void LE_GRAFIX_RetrieveArea(
  LE_DATA_DataId handle_returned_from_previous_store_area_call);
// This function restores to the background an area previously stored with
// the above call.  Remember the user is required to free this area when
// finished.  This function does not free the memory and therefore the same
// block can be retrieved over and over and over and over.

extern void LE_GRAFIX_Show24(LE_DATA_DataId object_handle,
  short dest_x, short dest_y, LE_DATA_DataId FUL_tag, BOOL transp);
// Displays a FUL data item to either an exising object or the display
// area depending on the status of the object_handle parameter.

extern BOOL LE_GRAFIX_RecolorUAP(LE_DATA_DataId hUAPData,
  COLORREF crFromColor, COLORREF crToColor);
// Recolors a UAP by substituting one color for another.

extern BOOL LE_GRAFIX_RecolorObject (LE_DATA_DataId hNativeBitmapData,
  COLORREF crFromColor, COLORREF crToColor);
// Recolors an object (native bitmap) by substituting one color for another

extern BOOL LE_GRAFIXBulkRecolorTAB (LE_DATA_DataId nBitmapID,
  LPCOLORMAP lpColorMap, int nColorsToRemap);
// recolours a TAB (UAP) using a table of from/to colour pairs and
// also treating primary colours as being special.  Used in Pony recolouring.

//----------------------------------------------------------------------------
// Generic bitmap functions - wrappers around DirectDraw surfaces, thus
// multiple bit depths are possible.  The last few aren't implemented yet.

extern LE_DATA_DataId LE_GRAFIX_CreateGBMFromDDSurface (PDDSURFACE DDSurfacePntr);
// This function creates a generic bitmap runtime data item using the
// given DirectDraw surface.  It adds a reference to the surface while
// the GBM exists.  Returns LE_DATA_EmptyItem if it fails.  Normally
// user code just has an image in a datafile and the data loader does
// the surface creation for you when you LE_DATA_Use the data item,
// so this is only for special purposes, like making a GBM from the
// screen surface.

extern BOOL LI_GRAFIX_CreateGBMFromImageBuffer (BYTE *BufferPntr,
  UNS32 BufferSize, BOOL CreateTexture,
  LE_GRAFIX_GenericBitmapPointer GBMPntr);
// Fills in a new generic bitmap record and creates the associated surface
// from the given BMP (8 or 24 bit) or TAB (8 bit with alpha) data.
// Returns TRUE if successful.  Used by the data system to create your
// generic bitmap for you, no need to call this directly.

extern BOOL LI_GRAFIX_DestroyGBM (LE_GRAFIX_GenericBitmapPointer GBMPntr);
// Deallocate the DirectDraw surfaces for a generic bitmap, used by the
// DataID unloading code.  User code should just free the data item.

extern LE_GRAFIX_GenericBitmapPointer LE_GRAFIX_RestoreGBM (
  LE_DATA_DataId GenericBitmapDataID);
// Restores the DirectDraw surface of a given generic bitmap.  If
// necessary, reloads the image from the data file.  Returns the
// Generic Bitmap pointer if successful (and you can then use the
// DirectDraw surface pointer in it).  Returns NULL on failure.

extern LE_GRAFIX_GenericBitmapPointer LE_GRAFIX_RestoreAndLockGBM (
  LE_DATA_DataId GenericBitmapDataID);
// Locking a generic bitmap forces it to be in memory and sets the pointers
// to the pixel data.  You need to unlock it after using it.  Also don't
// lock the screen while debugging - it won't be able to switch to the
// debugger if you hit a breakpoint, and your computer will die.  Returns
// a pointer to the generic bitmap if successful, NULL if it fails.
//
// When using multitasking, please try to avoid having two different threads
// locking the same surface, use the LE_GRAFIX_LockedSurfaceCriticalSectionPntr
// if you have to avoid that problem (the animation engines periodically lock
// the screen and uses this critical section to do it, so keep your lock time
// short to avoid animation jerkyness).  Also, locking a surface makes it
// unBLITable, so if you are just copying surfaces, just use the BLT
// functions and don't lock at all.

extern BOOL LE_GRAFIX_UnlockGBM (LE_DATA_DataId GenericBitmapDataID);
// Unlocking it lets the system (DirectDraw too) move things around and dump
// unused data.  It should only be locked momentarily, while the bitmap is
// being changed.

extern UNS32 LE_GRAFIX_WriteBMPFromGBM (
  LE_DATA_DataId GenericBitmapDataID, void *Buffer, UNS32 BufferSize);
// Convert from GBMs to BMPs, returns size written.

extern LE_DATA_DataId LE_GRAFIX_CreateGBM (UNS16 Width, UNS16 Height,
  BOOL SolidColour, UNS16 OverallAlpha);
// This function creates a generic bitmap runtime data item of the given width
// and height and the current screen depth.  The memory is allocated in the
// user's pool.  If SolidColour is FALSE then pixels which match the colour
// key (usually greenish pixels) are treated as transparent.  OverallAlpha
// controls the overall transparency of the bitmap, normally you just use
// LE_GRAFIX_ALPHA_OPAQUE100_0.  Remember you must deallocate the DataID later
// to free the memory used by the generic bitmap when you are finished with it.
// Returns LE_DATA_EmptyItem if it fails (out of memory usually).

extern BOOL LE_GRAFIX_ChangeGBMOverallAlpha (
  LE_DATA_DataId GenericBitmapDataID, UNS8 NewOverallAlpha);
// This function changes the overall alpha transparency value of an object,
// if you want it to show on the screen then you need to redraw the bitmap
// or get the animation system to redraw it for you.

extern UNS16 LE_GRAFIX_GetGBMWidth (LE_DATA_DataId GenericBitmapDataID);
extern UNS16 LE_GRAFIX_GetGBMHeight (LE_DATA_DataId GenericBitmapDataID);
extern BOOL LE_GRAFIX_GetGBMSolidColour (LE_DATA_DataId GenericBitmapDataID);
extern UNS8 LE_GRAFIX_GetGBMOverallAlpha (LE_DATA_DataId GenericBitmapDataID);
// These functions return various info about a generic bitmap.  You can
// also just get the LE_GRAFIX_GenericBitmapPointer and directly look
// at the fields in it.

extern void LE_GRAFIX_CopyGBM (LE_DATA_DataId SourceGenericBitmapDataID,
  LE_DATA_DataId DestinationGenericBitmapDataID, int X, int Y);
// Draw the source object into the destination object with the top left
// corner of the source appearing at (X,Y) in the destination (can be
// negative).  Does clipping too so you can go half off the edge of the
// destination.

extern LE_DATA_DataId LE_GRAFIX_StoreGBM (
  LE_DATA_DataId SourceGenericBitmapDataID,
  int X, int Y, int Width, int Height, BOOL SolidColour, UNS16 OverallAlpha);
// Much like the old StoreArea, this creates a new generic bitmap and copies
// the given rectangular area from the source bitmap into it (parts not filled
// due to clipping will be transparent coloured - usually green).  The X and Y
// coordinates are also stored into the xOffset and yOffset fields of the GBM
// for later use by LE_GRAFIX_RetrieveGBM.

extern BOOL LE_GRAFIX_RetrieveGBM (LE_DATA_DataId SourceGenericBitmapDataID,
  LE_DATA_DataId DestinationGenericBitmapDataID);
// Much like the old RetrieveArea, this copies the source bitmap into the
// destination bitmap, using the xOffset and yOffset fields of the GBM
// to place it in the destination from the original spot it came from.
// Remember to deallocate it after you have finished with it!

extern void LE_GRAFIX_FillGBM (LE_DATA_DataId GenericBitmapDataID,
  int X, int Y, int Width, int Height, UNS32 Colour);
// This function fills the given rectangle with the colour passed in.  The
// colour is a 32 bit colorref value (see LE_GRAFIX_MakeColorRef(r,g,b)).
// If you go over the edge of the bitmap, your colouring will be clipped
// to fit.


#endif // __L_GRAFIX_H__
