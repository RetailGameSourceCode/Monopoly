/*****************************************************************************
 * FILE:        L_Grafix.cpp
 *
 * DESCRIPTION: Graphics system.  Routines for handling bitmaps and accessing
 *              the screen (the BLT module is conceptually a part of this).
 *              There is also some glue here for using PC3D for 3D modes.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Grafix.cpp 65    18/09/99 4:44p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Grafix.cpp $
 * 
 * 65    18/09/99 4:44p Agmsmith
 * 
 * 64    99/09/10 10:48a Agmsmith
 * Request screen redraw after a string of blit failures.
 *
 * 63    99/08/30 4:07p Agmsmith
 * Avoid divide by zero and other problems when window minimized (screen
 * destination rectangle is -32000 in all coordinates).
 *
 * 62    8/09/99 15:35 Davew
 * Removed a call that is only valid in WinNT
 *
 * 61    7/05/99 12:22p Mikeh
 * Added debug mode checks for display attempts straight to the
 * former Artech background (LED_EI). Display a message box when
 * attempted. The following functions were modified.
 *
 * LE_GRAFIX_ColourArea
 * LE_GRAFIX_Show_TCB
 * LE_GRAFIX_Show_TCB_Alpha
 * LE_GRAFIX_Show24
 * LE_GRAFIX_ShowObject
 *
 * 60    6/25/99 11:55 Davew
 * Fixed the PC3D dependencies
 *
 * 59    6/22/99 12:50 Davew
 * Dependency changes from pc3d
 *
 * 58    6/18/99 15:01 Davew
 * The surface class changed so I propagated the changes here.
 *
 * 57    6/07/99 3:03p Agmsmith
 * Rearranged init code so more stuff shared between 2D and 3D, added a
 * function for converting rectangles from virtual to real screen
 * coordinates.
 *
 * 56    6/03/99 11:55 Davew
 * Changed the ChangeVideoMode function so it cleans things up nicely in
 * 3D mode.
 *
 * 55    6/01/99 9:45a Agmsmith
 * Removed useless error messages.
 *
 * 54    5/31/99 15:32 Davew
 * Fixed the 3D version of the ChangeVideoMode function so it now properly
 * reinitializes the global dimensioning variables dealing with the
 * primary and rendering surfaces.
 *
 * 53    5/31/99 15:00 Davew
 * Removed some #pragma messages
 *
 * 52    5/31/99 14:58 Davew
 * Enabled the ChangeVideoMode() call for the 3D system and had to change
 * its parameters as well.
 *
 * 51    5/31/99 14:36 Davew
 * Updated the ChangeVideoMode function so it behaves accordingly if we
 * are using the pc3D stuff
 *
 * 50    5/27/99 6:13p Fredds
 * Added shift values for RGB components that indicate how many bits to
 * shift a given component until it's in the LSB.
 *
 * 49    5/20/99 3:22p Agmsmith
 * Need windowed mode in 2D debug mode.
 *
 * 48    5/08/99 12:27p Agmsmith
 * Can now explicitly ask for full screen or windowed, or ask for the
 * default based on the .INI file.
 *
 * 47    5/04/99 12:13p Agmsmith
 * LE_GRAFIX_LockObjectDataPtr should use real screen surface, not working
 * surface.
 *
 * 46    5/03/99 15:49 Davew
 * Added a field in the initialization for multitasking
 *
 * 45    4/29/99 14:20 Davew
 * Fixed window size difference problems with the 3d stuff
 *
 * 44    4/28/99 16:50 Davew
 * Propogated changes made to the 3D directX init stuff
 *
 * 43    4/25/99 3:06p Agmsmith
 * Remove commented out obsolete code.
 *
 * 42    4/25/99 1:09p Agmsmith
 * Now works with DirectX 6.1 and uses DaveW's header file.  Also fixed
 * ATI init bug in 2D init.
 *
 * 41    4/25/99 11:32a Agmsmith
 * Added function for unloading all data.
 *
 * 40    4/23/99 13:09 Davew
 * Move the screen surface dealocation so it doesn't conflict with the
 * destruction code in Pc3D
 *
 * 39    4/23/99 11:25a Agmsmith
 *
 * 38    4/23/99 11:24a Agmsmith
 * Need to zero memory for runtime created memory GBMs, data system no
 * longer does it.
 *
 * 37    4/22/99 16:22 Davew
 * Fixed a problem where a pointer was not being properly initialized to
 * NULL
 *
 * 36    4/22/99 15:00 Davew
 * Updated to DX v6
 *
 * 35    4/15/99 11:07a Agmsmith
 * Updated get any bitmap width / height to also work with generic
 * bitmaps.
 *
 * 34    4/12/99 3:41p Agmsmith
 * Use GetLoadedDataType instead of current, unless you know what you are
 * doing.
 *
 * 33    4/11/99 3:48p Agmsmith
 * Now postload converts BMPs into native screen depth bitmaps, rather
 * than having the sequencer do it.  So remove all BMP things everywhere.
 *
 * 32    3/29/99 3:30p Agmsmith
 * Added a second clipper just for keeping track of the window's visible
 * region.
 *
 * 31    3/28/99 5:28p Agmsmith
 * Move copying of working screen to main screen into graphics module, and
 * add clipping to window's visible region so it doesn't trash other
 * things on the screen.
 *
 * 30    3/24/99 12:47p Timp
 * Added warning/check for DirectDraw bug to LE_GRAFIX_FillGBM.
 *
 * 29    3/20/99 4:53p Agmsmith
 * Work around a DirectDraw bug with pitch of surfaces being wrong for 24
 * and 32 bits at certain sizes.  Discovered by Tim Park.
 *
 * 28    3/10/99 7:05p Timp
 * Added GetGBMWidth/Height and FillGBM
 *
 * 27    1/31/99 11:27a Agmsmith
 * Get screen surface description error check bug fix.
 *
 * 26    12/30/98 4:51p Agmsmith
 * Better naming - virtual screen size separate from real screen size.
 *
 * 25    12/30/98 2:58p Agmsmith
 * 3D init changes.
 *
 * 24    12/30/98 11:10a Agmsmith
 * Now does source and destination rectangle setup better, and makes the
 * working surface the requested size even though the real screen may be
 * different.
 *
 * 23    12/26/98 4:47p Agmsmith
 * Check for right data type in generic bitmap routines in debug mode.
 *
 * 22    12/26/98 3:00p Agmsmith
 * Now can restore GBMs, and lock and unlock them.
 *
 * 21    12/24/98 3:42p Agmsmith
 * Now works with DirectDraw quirks in 8 bit and native screen depth,
 * texture or plain surface.  Also now loads in the bitmap pixels too.
 *
 * 20    12/23/98 5:34p Agmsmith
 * Fixed some bugs in screen initialisation.
 *
 * 19    12/23/98 4:27p Agmsmith
 * Code for creating a generic bitmap from a BMP or TAB under construction
 * (compiles but not finished or tested).
 *
 * 18    12/21/98 5:38p Agmsmith
 * Generic bitmap loader now uses LE_BLT_CopySolidUnityRaw.
 ****************************************************************************/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemGrafix
  #if CE_ARTLIB_EnableSystemGrafix3D
    #include "../pc3d/PC3DHdr.h"
  #endif



/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

PDDRAW LE_GRAFIX_DirectDrawObjectN;
  // DirectDraw object corresponding to the video board we are using.
  // The N means that it is in some variable version of DirectDraw,
  // see L_TYPE.H for the version being used.

PDDSURFACE LE_GRAFIX_DDPrimarySurfaceN;
  // DirectDraw surface which represents the real screen memory,
  // on the video board.  Also see LE_GRAFIX_ScreenGBM.

LE_DATA_DataId LE_GRAFIX_ScreenGBM;
  // A wrapper around the DirectDraw screen memory, LE_DATA_EmptyItem
  // if system isn't open.  You can get a pointer to the screen memory
  // by locking the GBM.

PDDCLIPPER LI_GRAFIX_WindowVisibleRegionClipper;
  // This is used only for getting the visible region from the window.
  // Initialised when this module starts up, deallocated when it shuts down.

PDDSURFACE LE_GRAFIX_DDWorkingSurfaceN;
  // DirectDraw surface for the compositing area, same size as the
  // screen but usually in system memory (except in 3D mode when it
  // may have to be video so that the hardware can render to it).
  // NULL if not used (some games don't use the work area).  Also
  // see LE_GRAFIX_WorkGBM.

LE_DATA_DataId LE_GRAFIX_WorkGBM;
  // Wrapper around the working memory DirectDraw surface,
  // LE_DATA_EmptyItem if it doesn't exist.

BOOL LE_GRAFIX_WindowedMode;
  // TRUE if currently running in windowed mode, FALSE if we have the
  // full screen (and can thus switch video modes etc).  Set by the
  // screen init code.

LE_GRAFIX_VideoMemoryType LE_GRAFIX_VideoMemoryForGBMTextures = LE_GRAFIX_SYSTEM_MEMORY;
  // What kind of video memory to use when creating textures
  // (8 bit images, maybe with alpha too).  Generally this gets
  // set when the library is initialised for a software or a
  // hardware renderer.

LE_GRAFIX_VideoMemoryType LE_GRAFIX_VideoMemoryForGBMPictures = LE_GRAFIX_SYSTEM_MEMORY;
  // What kind of video memory to use when creating generic pictures
  // (usually true colour images at current screen depth).

BOOL LE_GRAFIX_SlowScrolling;
  // TRUE if slow scrolling hardware was detected (video memory to same video
  // memory blit is slow).  Some workarounds for not scrolling the screen will
  // be used.

#if CE_ARTLIB_EnableMultitasking
LPCRITICAL_SECTION LE_GRAFIX_LockedSurfaceCriticalSectionPntr;
  // Provides mutual exclusion around the locking of any surface,
  // so that other threads wait for the one doing drawing to finish.
  // Note that all surfaces share the same critical section, so only
  // one thread can do any drawing at a time.  The alternative would
  // be to have a critical section in every generic bitmap.  Unfortunately
  // the DirectDraw lock operation doesn't use critical sections - it
  // busy waits until the surface is unlocked by another thread.
  // NULL if the graphics system is shut down.
#endif

RECT LE_GRAFIX_ScreenSourceRect = {0, 0, 800, 600};
RECT LE_GRAFIX_ScreenDestinationRect = {0, 0, 800, 600};
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

int LE_GRAFIX_VirtualScreenWidthInPixels = 800;
int LE_GRAFIX_VirtualScreenHeightInPixels = 600;
  // Size of your virtual screen.  Usually the same as the real screen size
  // which you actually got (may only be close to your requested values
  // (CE_ARTLIB_InitialScreenWidth etc) if the user's hardware doesn't
  // support your exact requested size).  All drawing internally is done
  // in this virtual screen area, stuff outside it is clipped.  This is
  // also the size of the work area bitmap, which doesn't have to be the
  // same size as the real screen.

int LE_GRAFIX_RealScreenWidthInPixels = 800;
int LE_GRAFIX_RealScreenHeightInPixels = 600;
int LE_GRAFIX_ScreenBitsPerPixel = 8;
  // Size of the real hardware screen.  The LE_GRAFIX_ScreenDestinationRect
  // must be fully contained in this area otherwise things won't work.
  // Note that you may get a different size than you asked for.



/****************************************************************************/
/* S T A T I C   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

#if CE_ARTLIB_EnableMultitasking
static CRITICAL_SECTION LE_GRAFIX_LockedSurfaceCriticalSection;
  // The actual critical section storage space, see
  // LE_GRAFIX_LockedSurfaceCriticalSectionPntr for details.
  // Initialised when the graphics system starts up.
#endif

static PDDCLIPPER LI_GRAFIX_InvalidAreaClipper;
  // This is used for only drawing part of the screen in the
  // LE_GRAFIX_CopyWorkAreaToOutputArea function.  Initialised when
  // this module starts up, deallocated when it shuts down.

static WORD           LI_GRAFIX_BestVideoModeScore;
static DDSURFDESC     LI_GRAFIX_BestVideoModeSurfaceDesc;
  // Higher scores are better, when choosing video modes.
  // The surface matches the best mode found so far.

static BOOL LI_GRAFIX_FoundSuitableDisplayMode = FALSE;
  // Found a video mode close enough to our desires.

static LE_GRAFIX_DepthInBytesValue LI_GRAFIX_DesiredScreenDepth;
static LE_GRAFIX_DepthSet LI_GRAFIX_AcceptableScreenDepthSet;
static BOOL LI_GRAFIX_AcceptLargerScreenSizes;
static BOOL LI_GRAFIX_AcceptSmallerScreenSizes;
static WORD LI_GRAFIX_DesiredScreenWidth;
static WORD LI_GRAFIX_DesiredScreenHeight;


// Used for finding a video mode like the Windows one.
// These are the values the user has for his Windows
// settings when the game started.

static WORD WindowsScreenWidth;
static WORD WindowsScreenHeight;
static WORD WindowsScreenDepth;
static WORD WindowsScreenRefreshRate;



/*****************************************************************************
 * Move the screen pixels around using the video board's blitter (or software)
 * and meaures the time it takes.  Sets the LE_GRAFIX_SlowScrolling flag
 * appropriately, or leaves it alone if an error happens.
 */

void LE_GRAFIX_TestForSlowScrolling (void)
{
  DDBLTFX       BlitEffects;
  TYPE_Rect     DestRect;
  DWORD         ElapsedTime;
  DWORD         EndTime;
  HRESULT       ErrorCode;
  int           i;
  TYPE_Rect     SourceRect;
  DWORD         StartTime;

  if (LE_GRAFIX_DDPrimarySurfaceN == NULL)
    return;

  // Fill the screen with colour - solid black in release mode, a nice
  // colourful stripey picture in debug mode.

  memset (&BlitEffects, 0, sizeof (BlitEffects));
  BlitEffects.dwSize = sizeof (BlitEffects);
  BlitEffects.dwFillColor= 0;

  for (DestRect = LE_GRAFIX_ScreenDestinationRect;
  DestRect.left < LE_GRAFIX_ScreenDestinationRect.right;
  DestRect.left++, DestRect.right = DestRect.left + 1)
  {
    ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->Blt (
      &DestRect, NULL, NULL,
      DDBLT_WAIT | DDBLT_COLORFILL, &BlitEffects);

    if (ErrorCode != DS_OK)
    {
#if CE_ARTLIB_EnableDebugMode
      LE_ERROR_DebugMsg ("LE_GRAFIX_TestForSlowScrolling: "
      "Some other kind of blitter error.\r\n", LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return;
    }

#if CE_ARTLIB_EnableDebugMode
    // Colourful pattern in debug mode so you can see it happening.
    BlitEffects.dwFillColor += 0x01030507;
#else
    // Use boring black in release mode.
    BlitEffects.dwFillColor = 0;
#endif
  }

  // Do scrolling which shifts the whole screen over in 7 pixel increments,
  // a nice odd number to test the video hardware with.  See how many scrolls
  // we can do in one second, need at least 60 to be smooth.

  SourceRect = LE_GRAFIX_ScreenDestinationRect;
  SourceRect.left += 7;

  DestRect = LE_GRAFIX_ScreenDestinationRect;
  DestRect.right -= 7;

  StartTime = GetTickCount ();

  for (i = 0; i < 60; i++)
  {
    ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->BltFast (
        DestRect.left, DestRect.top,
        LE_GRAFIX_DDPrimarySurfaceN,
        &SourceRect,
        DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

    if (ErrorCode != DD_OK)
    {
#if CE_ARTLIB_EnableDebugMode
      LE_ERROR_DebugMsg ("LE_GRAFIX_TestForSlowScrolling: "
      "Some sort of blitter error.\r\n", LE_ERROR_DebugMsgToFileAndScreen);
#endif
       return;
    }

    EndTime = GetTickCount ();
    ElapsedTime = EndTime - StartTime;
    if (ElapsedTime > 1000)
    {
      // Oops, have already taken up 1 second of real time, this
      // must be a slow scrolling video board.

      LE_GRAFIX_SlowScrolling = TRUE;
      return;
    }
  }

  // It's fast if we get here.

  LE_GRAFIX_SlowScrolling = FALSE;
}



#if !CE_ARTLIB_EnableSystemGrafix3D
/*****************************************************************************
 * This is called by the DirectDraw enumerate display modes.  We give each
 * mode a score based on how close it is to the user's desired mode, and
 * keep track of the best one so far.
 */

HRESULT WINAPI LI_GRAFIX_EnumDisplayModesCallback (
  PDDSURFDESC lpDDSurfaceDesc, LPVOID lpContext)
{
  WORD Score = 0;
  int   PercentError;

  // Has to match the requested width and height.  We also require
  // information on the depth, and a depth which does true colour
  // (don't support 8 bit or less any more).

  if ((lpDDSurfaceDesc->dwFlags & (DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT)) == 0 ||
  (lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_RGB) == 0)
    goto NormalExit; // Don't examine this mode if it doesn't tell us about it.

  // Check for screen size being too small or too large.

  if (!LI_GRAFIX_AcceptLargerScreenSizes)
  {
    if (lpDDSurfaceDesc->dwHeight > LI_GRAFIX_DesiredScreenHeight ||
    lpDDSurfaceDesc->dwWidth > LI_GRAFIX_DesiredScreenWidth)
      goto NormalExit;
  }

  if (!LI_GRAFIX_AcceptSmallerScreenSizes)
  {
    if (lpDDSurfaceDesc->dwHeight < LI_GRAFIX_DesiredScreenHeight ||
    lpDDSurfaceDesc->dwWidth < LI_GRAFIX_DesiredScreenWidth)
      goto NormalExit;
  }

  // Check for the screen depth being acceptable.

  if (((1 << (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount / 8)) &
  LI_GRAFIX_AcceptableScreenDepthSet) == 0)
    goto NormalExit;

#if CE_ARTLIB_EnableDebugMode // Debug mode MUST use current screen depth.
  if (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount != WindowsScreenDepth)
    goto NormalExit;
#endif

  // Modify the score for the height.  Correct height adds 100, ones which
  // are off by a bit add less.

  PercentError = 100 * ((int) lpDDSurfaceDesc->dwHeight - LI_GRAFIX_DesiredScreenHeight) /
    LI_GRAFIX_DesiredScreenHeight;
  if (PercentError < 0) PercentError = -PercentError;
  if (PercentError > 100) PercentError = 100;
  Score += 100 - PercentError;

  // Score the width.

  PercentError = 100 * ((int) lpDDSurfaceDesc->dwWidth - LI_GRAFIX_DesiredScreenWidth) /
    LI_GRAFIX_DesiredScreenWidth;
  if (PercentError < 0) PercentError = -PercentError;
  if (PercentError > 100) PercentError = 100;
  Score += 100 - PercentError;

  // Score the screen depth.

  if (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount ==
  (DWORD) (LI_GRAFIX_DesiredScreenDepth * 8))
    Score += 100; // Exact match is worth a bit too.

  // Give a slight nudge to the values - prefer 16 bit for speed,
  // then 32 bit for good looks and slight slowness then 24 bits
  // for good looks and lots of slowness.

  if (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount == 16)
    Score += 3;
  else if (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount == 32)
    Score += 2;
  else if (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount == 24)
    Score += 1;

  if (Score > LI_GRAFIX_BestVideoModeScore)
  {
    LI_GRAFIX_BestVideoModeScore = Score;
    LI_GRAFIX_FoundSuitableDisplayMode = TRUE;
    LI_GRAFIX_BestVideoModeSurfaceDesc = *lpDDSurfaceDesc;
  }

NormalExit:
  return DDENUMRET_OK; // Keep on enumerating.
}
#endif



/*****************************************************************************
 * Deallocate video screen surfaces and other things which are dependent on
 * the video mode (like converted native bitmaps), but not the DirectDraw
 * object itself (since we may be merely changing video mode, not video
 * boards).
 */

static void LI_GRAFIX_DeallocateScreenDepthRelatedThings (void)
{
  // Stop all running sequences since they may have references to
  // bitmaps and other things, which we want to unload.

  LE_SEQNCR_StopAllRP ((LE_REND_RenderSlotSet) -1, 0 /* Priority */);
  LE_SEQNCR_ProcessUserCommands ();

  // We need to uninstall all of the render slots, so that we can
  // later deallocate any surfaces they may be using.

  for (LE_REND_RenderSlot i = 0; i < CE_ARTLIB_RendMaxRenderSlots; i++)
    LE_SEQNCR_UninstallRenderSlot(i);

  // Get rid of our convenience generic bitmaps.

  if (LE_GRAFIX_ScreenGBM != LE_DATA_EmptyItem)
  {
    LE_DATA_FreeRuntimeDataID (LE_GRAFIX_ScreenGBM);
    LE_GRAFIX_ScreenGBM = LE_DATA_EmptyItem;
  }

  if (LE_GRAFIX_WorkGBM != LE_DATA_EmptyItem)
  {
    LE_DATA_FreeRuntimeDataID (LE_GRAFIX_WorkGBM);
    LE_GRAFIX_WorkGBM = LE_DATA_EmptyItem;
  }

  // Unload all things, in particular unload the bitmaps which have been
  // converted to the current screen depth, since the screen depth may
  // be changing after this point.

  LE_DATA_UnloadEverything (FALSE /* UnloadByUser */);

  // Release the graphics library reference to the screen surfaces.

  if (LE_GRAFIX_DDPrimarySurfaceN != NULL)
    LE_GRAFIX_DDPrimarySurfaceN->Release ();
  LE_GRAFIX_DDPrimarySurfaceN = NULL;

  if (LE_GRAFIX_DDWorkingSurfaceN != NULL)
    LE_GRAFIX_DDWorkingSurfaceN->Release ();
  LE_GRAFIX_DDWorkingSurfaceN = NULL;
}



/*****************************************************************************
 * Get the details about the screen mode we actually got (depth, colour
 * bitfield assignments).  Copies the surface description of the screen
 * into LI_GRAFIX_BestVideoModeSurfaceDesc too.
 */

static void LI_GRAFIX_SetGlobalScreenFormatFromPrimarySurface (void)
{
  DWORD dwMaskBit;
  UNS8 Count;

  ZeroMemory(&LI_GRAFIX_BestVideoModeSurfaceDesc,
    sizeof(LI_GRAFIX_BestVideoModeSurfaceDesc));

  LI_GRAFIX_BestVideoModeSurfaceDesc.dwSize =
    sizeof(LI_GRAFIX_BestVideoModeSurfaceDesc);

  if (LE_GRAFIX_DDPrimarySurfaceN->GetSurfaceDesc(
  &LI_GRAFIX_BestVideoModeSurfaceDesc) != DD_OK)
    return;

  // Get screen bits per pixel and related info.

  LE_GRAFIX_ScreenBitsPerPixel = LI_GRAFIX_BestVideoModeSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
  LE_BLT_dwRedMask = LI_GRAFIX_BestVideoModeSurfaceDesc.ddpfPixelFormat.dwRBitMask;
  LE_BLT_dwGreenMask = LI_GRAFIX_BestVideoModeSurfaceDesc.ddpfPixelFormat.dwGBitMask;
  LE_BLT_dwBlueMask = LI_GRAFIX_BestVideoModeSurfaceDesc.ddpfPixelFormat.dwBBitMask;

  if ((LE_BLT_dwRedMask == 0) || (LE_BLT_dwGreenMask == 0) || (LE_BLT_dwBlueMask == 0))
    LE_ERROR_ErrorMsg ("LI_GRAFIX_SetGlobalScreenFormatFromPrimarySurface: "
    "Color masks are all zero in the pixel format, "
    "something is wrong with your video board driver.");

  // Get position of mask bits in each mask.
  dwMaskBit = 0x80000000;
  LE_BLT_nRedShift = 32;
  while ((dwMaskBit & LE_BLT_dwRedMask) == 0)
  {
    LE_BLT_nRedShift --;
    dwMaskBit >>= 1;
  }

  dwMaskBit = 0x80000000;
  LE_BLT_nGreenShift = 32;
  while ((dwMaskBit & LE_BLT_dwGreenMask) == 0)
  {
    LE_BLT_nGreenShift --;
    dwMaskBit >>= 1;
  }

  dwMaskBit = 0x80000000;
  LE_BLT_nBlueShift = 32;
  while ((dwMaskBit & LE_BLT_dwBlueMask) == 0)
  {
    LE_BLT_nBlueShift --;
    dwMaskBit >>= 1;
  }

  // RED shift value
  Count=0;
  __asm
  {
    mov eax,LE_BLT_dwRedMask

Red_Mask_Loop:
    ror eax,1
    inc Count
    jnc Red_Mask_Loop

    dec Count
  }
  LE_BLT_nRedShiftValue=Count;


  // GREEN shift value
  Count=0;
  __asm
  {
    mov eax,LE_BLT_dwGreenMask

Green_Mask_Loop:
    ror eax,1
    inc Count
    jnc Green_Mask_Loop

    dec Count
  }
  LE_BLT_nGreenShiftValue=Count;


  // BLUE shift value
  Count=0;
  __asm
  {
    mov eax,LE_BLT_dwBlueMask

Blue_Mask_Loop:
    ror eax,1
    inc Count
    jnc Blue_Mask_Loop

    dec Count
  }
  LE_BLT_nBlueShiftValue=Count;
}



/*****************************************************************************
 * Switch the screen to a video mode which matches the requested one, returns
 * TRUE if successful, FALSE if it fails (or displays a message and ends
 * the program if HaltOnError is TRUE).  The various variables that reflect
 * the screen size are updated with the new values.  Note that you shouldn't
 * be using the old screen since it gets closed (shut down render slots etc).
 */

BOOL LE_GRAFIX_ChangeVideoMode (int ScreenWidth, int ScreenHeight,
BOOL AcceptLargerSizes, BOOL AcceptSmallerSizes,
LE_GRAFIX_DepthInBytesValue DesiredDepthValue,
LE_GRAFIX_DepthSet AcceptableDepthSet,
int WorkWidth, int WorkHeight, BOOL HaltOnError)
{
  char  ErrorMessage[512];
  BOOL  ReturnCode = FALSE;

  LI_GRAFIX_DeallocateScreenDepthRelatedThings ();

#if CE_ARTLIB_EnableMultitasking
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
#endif

#if CE_ARTLIB_EnableSystemGrafix3D
  // Set the display mode using the new dimensions.  We will keep the same
  // colour depth though
  if (!DDRAW_SetMode(pc3D::IsWindowed(), ScreenWidth, ScreenHeight))
  {
    if (!DDRAW_SetMode(pc3D::IsWindowed(),
                       LE_GRAFIX_VirtualScreenWidthInPixels,
                       LE_GRAFIX_VirtualScreenHeightInPixels))
    {
      sprintf(ErrorMessage, "FATAL ERROR!  Attempt to change the screen "
              "resolution failed and could not restore the old mode\r\n");
      goto ErrorExit;
    }
  }

  // Grab the direct draw object (probably redundant) & render surfaces,
  // add a reference to show we are using them (the shutdown code releases
  // them whether created by PC3D or not).

  LE_GRAFIX_DirectDrawObjectN = DDRAW_GetDirectDraw();

  LE_GRAFIX_DDWorkingSurfaceN = DDRAW_GetRenderSurface();
  LE_GRAFIX_DDWorkingSurfaceN->AddRef ();

  LE_GRAFIX_DDPrimarySurfaceN = DDRAW_GetPrimarySurface();
  LE_GRAFIX_DDPrimarySurfaceN->AddRef ();

  LI_GRAFIX_SetGlobalScreenFormatFromPrimarySurface ();

#else // !CE_ARTLIB_EnableSystemGrafix3D

  DDSURFDESC            DDSurfaceDesc;
  HRESULT               hDDReturnValue;

  AcceptableDepthSet |= (1 << DesiredDepthValue); // Just for sanity.

  // Look for a video mode close to what the user wants.

  LI_GRAFIX_FoundSuitableDisplayMode = FALSE;
  LI_GRAFIX_BestVideoModeScore = 0;
  LI_GRAFIX_DesiredScreenDepth = DesiredDepthValue;
  LI_GRAFIX_AcceptableScreenDepthSet = AcceptableDepthSet;
  LI_GRAFIX_AcceptLargerScreenSizes = AcceptLargerSizes;
  LI_GRAFIX_AcceptSmallerScreenSizes = AcceptSmallerSizes;
  LI_GRAFIX_DesiredScreenWidth = ScreenWidth;
  LI_GRAFIX_DesiredScreenHeight = ScreenHeight;

  // First try asking directly for the desired mode, due to bugs in the
  // ATI video drivers in release mode (full screen exclusive), it
  // reports only the depths of the desktop as being available when
  // enumerating (and reports the unavailable real depths when enumerating
  // while in shared screen mode), but it works if you ask for the mode
  // directly.

  hDDReturnValue = LE_GRAFIX_DirectDrawObjectN->SetDisplayMode (
    ScreenWidth, ScreenHeight, DesiredDepthValue * 8 /* in bits */,
    CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ, 0);

  if (hDDReturnValue != DD_OK)
    hDDReturnValue = LE_GRAFIX_DirectDrawObjectN->SetDisplayMode (
    ScreenWidth, ScreenHeight, DesiredDepthValue * 8 /* in bits */,
    0 /* OK, try the default refesh rate then */, 0);

  if (hDDReturnValue != DD_OK)
  {
    // Well, that didn't work.  Ask for a mode close to the desired one.

    hDDReturnValue = LE_GRAFIX_DirectDrawObjectN->EnumDisplayModes (
      0 /* Don't ask for all refresh rate variations */,
      NULL,NULL,LI_GRAFIX_EnumDisplayModesCallback);

    if ((hDDReturnValue != DD_OK) || !LI_GRAFIX_FoundSuitableDisplayMode)
    {
      sprintf (ErrorMessage, "LE_GRAFIX_ChangeVideoMode: "
        "Unable to find a display mode which is %dx%d and "
        "can do %d bits per pixel color.  "
        "Try changing your video board to a better one, or use the DirectX "
        "control panel to switch the primary DirectDraw device to "
        "another video board if you have more than video board.\r\n",
        (int) ScreenWidth, (int) ScreenHeight, (int) DesiredDepthValue * 8);
      goto ErrorExit;
    }

    // Switch the screen into the mode we found, or one similar to it
    // if DirectDraw feels like it (might get different RGB bitfield
    // arrangements than we expect).

    hDDReturnValue = LE_GRAFIX_DirectDrawObjectN->SetDisplayMode (
      LI_GRAFIX_BestVideoModeSurfaceDesc.dwWidth,
      LI_GRAFIX_BestVideoModeSurfaceDesc.dwHeight,
      LI_GRAFIX_BestVideoModeSurfaceDesc.ddpfPixelFormat.dwRGBBitCount,
      LI_GRAFIX_BestVideoModeSurfaceDesc.dwRefreshRate,
      0);
    if (hDDReturnValue != DD_OK)
    {
      sprintf (ErrorMessage, "LE_GRAFIX_ChangeVideoMode: "
        "Unable to set display mode to %dx%d with %d bit depth.  Try changing "
        "your Windows display properties to be close to %d by %d with %d bit color "
        "depth and run this game again.\r\n",
        (int) LI_GRAFIX_BestVideoModeSurfaceDesc.dwWidth,
        (int) LI_GRAFIX_BestVideoModeSurfaceDesc.dwHeight,
        (int) LI_GRAFIX_BestVideoModeSurfaceDesc.ddpfPixelFormat.dwRGBBitCount,
        ScreenWidth, ScreenHeight, (int) (DesiredDepthValue * 8));
      goto ErrorExit;
    }
  }

  // Make the primary surface - the real screen memory.

  ZeroMemory (&DDSurfaceDesc, sizeof (DDSurfaceDesc));
  DDSurfaceDesc.dwSize = sizeof (DDSurfaceDesc);
  DDSurfaceDesc.dwFlags = DDSD_CAPS;
  DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
  hDDReturnValue = LE_GRAFIX_DirectDrawObjectN->CreateSurface (
    &DDSurfaceDesc, &LE_GRAFIX_DDPrimarySurfaceN, NULL);
  if (hDDReturnValue != DD_OK)
  {
    strcpy (ErrorMessage, "LE_GRAFIX_ChangeVideoMode: Unable to create a primary "
      "surface on your video board.  Is there some other DirectX game or "
      "program using the video board?");
    goto ErrorExit;
  }

  LI_GRAFIX_SetGlobalScreenFormatFromPrimarySurface ();

  // Now make the working surface, for off-screen preparations.
  // This can be a different size from the real screen, see the
  // LE_GRAFIX_ScreenSourceRect comments for more details.

  ZeroMemory (&DDSurfaceDesc, sizeof (DDSurfaceDesc));
  DDSurfaceDesc.dwSize = sizeof (DDSurfaceDesc);
  DDSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  DDSurfaceDesc.dwHeight = WorkHeight;
  DDSurfaceDesc.dwWidth = WorkWidth;
  DDSurfaceDesc.ddpfPixelFormat = LI_GRAFIX_BestVideoModeSurfaceDesc.ddpfPixelFormat;
  DDSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
  hDDReturnValue = LE_GRAFIX_DirectDrawObjectN->CreateSurface (
    &DDSurfaceDesc, &LE_GRAFIX_DDWorkingSurfaceN, NULL);
  if (hDDReturnValue != DD_OK)
  {
    strcpy (ErrorMessage, "LE_GRAFIX_ChangeVideoMode: "
      "Unable to create an off-screen surface.  Is your system low on memory "
      "or free disk space for the swap file to grow into?");
    goto ErrorExit;
  }
#endif // CE_ARTLIB_EnableSystemGrafix3D

  // 2D and 3D stuff continues in common here.

#if CE_ARTLIB_EnableDebugMode
  if (LI_GRAFIX_BestVideoModeSurfaceDesc.dwWidth != (DWORD) ScreenWidth ||
  LI_GRAFIX_BestVideoModeSurfaceDesc.dwHeight != (DWORD) ScreenHeight ||
  LI_GRAFIX_BestVideoModeSurfaceDesc.ddpfPixelFormat.dwRGBBitCount != (DWORD) (DesiredDepthValue * 8))
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_ChangeVideoMode: "
      "Didn't get your exact display mode, instead of %dx%d with %d bit depth, "
      "you got %dx%d with %d bits.  Alex says: Have fun debugging!\r\n",
      ScreenWidth, ScreenHeight, (int) (DesiredDepthValue * 8),
      (int) LI_GRAFIX_BestVideoModeSurfaceDesc.dwWidth,
      (int) LI_GRAFIX_BestVideoModeSurfaceDesc.dwHeight,
      (int) LI_GRAFIX_BestVideoModeSurfaceDesc.ddpfPixelFormat.dwRGBBitCount);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  ReturnCode = TRUE;
  goto NormalExit;


ErrorExit:
  if (HaltOnError)
    LE_ERROR_ErrorMsg (ErrorMessage);
  else
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg (ErrorMessage, LE_ERROR_DebugMsgToFileAndScreen);
#endif
  }
  LI_GRAFIX_DeallocateScreenDepthRelatedThings ();

NormalExit:
#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
#endif
  return ReturnCode;
}



/*****************************************************************************
 * This initialises the graphics system.  Besides setting up critical sections
 * and other internal data structures, it sets up the DirectDraw screen.
 * Returns TRUE if successful, FALSE if it fails (or if you use HaltOnError
 * then it displays a message and exits the program).
 */

BOOL LE_GRAFIX_InitSystem (int ScreenWidth, int ScreenHeight,
BOOL AcceptLargerSizes, BOOL AcceptSmallerSizes,
LE_GRAFIX_DepthInBytesValue DesiredDepthValue,
LE_GRAFIX_DepthSet AcceptableDepthSet,
BOOL HaltOnError, WindowedMode nWindowMode /*= WINDOWED_USE_DEFAULT*/)
{
  char                  ErrorMessage [256];
  BOOL                  ReturnCode = FALSE;
  HDC                   ScreenDC;
  int                   WorkHeight;
  int                   WorkWidth;

#if CE_ARTLIB_EnableSystemGrafix3D
  BOOL                  Successful;
#else
  DWORD                 CoopFlags;
  PDDRAW1               lpDirectDraw1Object = NULL;
#endif

  // Unload all things, in particular unload the bitmaps which have been
  // converted to the current screen depth, since the screen depth may
  // be changing after this point.

  LI_GRAFIX_DeallocateScreenDepthRelatedThings ();

  // Virtual screen is the requested screen size, may be different from
  // what you actually get, especially when running in windowed mode.

  WorkWidth = ScreenWidth;
  WorkHeight = ScreenHeight;

  strcpy (ErrorMessage, "LE_GRAFIX_InitSystem: No error message specified!");

#if CE_ARTLIB_EnableMultitasking
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL)
  {
    InitializeCriticalSection (&LE_GRAFIX_LockedSurfaceCriticalSection);
    LE_GRAFIX_LockedSurfaceCriticalSectionPntr = &LE_GRAFIX_LockedSurfaceCriticalSection;
  }
#endif

  // Don't switch screen size if running in a window, otherwise the
  // window will fill the screen and then why bother using a window?
  // However, switch to a true colour depth if in palette (8 bit) mode.

  ScreenDC = GetDC (NULL);
  if (ScreenDC != NULL)
  {
    WindowsScreenWidth = GetDeviceCaps (ScreenDC, HORZRES);
    WindowsScreenHeight = GetDeviceCaps (ScreenDC, VERTRES);
    WindowsScreenDepth = GetDeviceCaps (ScreenDC, BITSPIXEL);

    // NOTE: VREFRESH is only valid under WindowsNT.
//    WindowsScreenRefreshRate = GetDeviceCaps (ScreenDC, VREFRESH);
    WindowsScreenRefreshRate = 60;

    ReleaseDC (NULL, ScreenDC);
  }
  else
  {
    WindowsScreenWidth = 800;
    WindowsScreenHeight = 600;
    WindowsScreenDepth = 8;
    WindowsScreenRefreshRate = 60;
  }

  // For the 3D system, use their DirectDraw init,
  // since it sets up the 3D rendering device too.
  // It also decides if windowed or full screen mode is used.

#if CE_ARTLIB_EnableSystemGrafix3D
  Successful = DDRAW_Init(LE_MAIN_HwndMainWindow, ScreenWidth, ScreenHeight,
                          DesiredDepthValue * 8 /* Bits per pixel */,
                          TRUE /* Use Z buffer */,
                          CE_ARTLIB_EnableMultitasking,
                          nWindowMode);
  if (!Successful)
  {
    strcpy(ErrorMessage, "LE_GRAFIX_InitSystem: Unable to create the 3D DirectDraw stuff.");
    goto ErrorExit;
  }

  LE_GRAFIX_WindowedMode = pc3D::IsWindowed();

  // .......... return the direct draw object & main render surface (possibly in VIDEO RAM)
  LE_GRAFIX_DirectDrawObjectN = DDRAW_GetDirectDraw();
  LE_GRAFIX_DDWorkingSurfaceN = DDRAW_GetRenderSurface();
  LE_GRAFIX_DDWorkingSurfaceN->AddRef ();
  LE_GRAFIX_DDPrimarySurfaceN = DDRAW_GetPrimarySurface();
  LE_GRAFIX_DDPrimarySurfaceN->AddRef ();

  LI_GRAFIX_SetGlobalScreenFormatFromPrimarySurface();

#else // Not using 3D graphics DirectDraw init, just what you see here.

  HRESULT               hDDReturnValue;

  if (nWindowMode == WINDOWED_USE_DEFAULT)
    LE_GRAFIX_WindowedMode = CE_ARTLIB_EnableDebugMode;
  else
    LE_GRAFIX_WindowedMode = (nWindowMode == WINDOWED_USE_WINDOW);

  // Override requested screen settings with current ones for windowed mode.

  if (LE_GRAFIX_WindowedMode)
  {
    ScreenWidth = WindowsScreenWidth;
    ScreenHeight = WindowsScreenHeight;
    if (WindowsScreenDepth > 8) // Don't switch depth if already true colour.
      DesiredDepthValue = (LE_GRAFIX_DepthInBytesValue) (WindowsScreenDepth / 8);
  }

  // Create the old style DirectDraw object which
  // represents the default video board.

  hDDReturnValue = DirectDrawCreate (NULL, &lpDirectDraw1Object, NULL);
  if (hDDReturnValue != DD_OK)
  {
    strcpy (ErrorMessage, "LE_GRAFIX_InitSystem: Unable to create the DirectDraw "
      "object (for accessing your video board), is DirectX installed?");
    goto ErrorExit;
  }

  // Get the new style DirectDraw object.

  LE_GRAFIX_DirectDrawObjectN = NULL;
  hDDReturnValue = lpDirectDraw1Object->QueryInterface (
    CURRENT_DDRAW_GUID, (LPVOID *) &LE_GRAFIX_DirectDrawObjectN);
  if (hDDReturnValue != DD_OK)
  {
    strcpy (ErrorMessage, "LE_GRAFIX_InitSystem: Unable to get the latest DirectDraw "
      "interface, is DirectX 6 or later installed?");
    goto ErrorExit;
  }

  // Set the cooperative mode - do we get our own screen and video mode
  // or do we use what the current screen is set to, and share it
  // with Windows?  Have to share if the debugger is being used.

  CoopFlags = DDSCL_ALLOWREBOOT;
  if (CE_ARTLIB_EnableMultitasking)
    CoopFlags |= DDSCL_MULTITHREADED;
  if (LE_GRAFIX_WindowedMode)
    CoopFlags |= DDSCL_NORMAL;
  else
    CoopFlags |= DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;

  hDDReturnValue = LE_GRAFIX_DirectDrawObjectN->SetCooperativeLevel (
    LE_MAIN_HwndMainWindow, CoopFlags);
  if (hDDReturnValue != DD_OK)
  {
    strcpy (ErrorMessage, "LE_GRAFIX_InitSystem: Unable to set cooperative level "
      "for the DirectDraw object.  Is some other program running that takes "
      "over the screen?");
    goto ErrorExit;
  }

  // Optionally switch video modes and create the screen surfaces.

  if (!LE_GRAFIX_ChangeVideoMode (ScreenWidth, ScreenHeight, AcceptLargerSizes,
  AcceptSmallerSizes, DesiredDepthValue, AcceptableDepthSet,
  WorkWidth, WorkHeight, HaltOnError))
  {
    strcpy (ErrorMessage, "LE_GRAFIX_InitSystem: Unable to switch video modes.\r\n");
    goto ErrorExit;
  }
#endif // CE_ARTLIB_EnableSystemGrafix3D

  // Common stuff for finishing up 2D and 3D initialisation.

  // The real screen bitmap size is taken directly from the
  // output surface size.

  ReturnCode = LI_GRAFIX_PostSurfaceInit (TRUE  /* Do slow scroll test */);

  if (!ReturnCode)
  {
    strcpy (ErrorMessage, "LE_GRAFIX_InitSystem: PostSurfaceInit has failed for some reason.\r\n");
    goto ErrorExit;
  }

  goto NormalExit;


ErrorExit:
  if (HaltOnError)
    LE_ERROR_ErrorMsg (ErrorMessage);
  else
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg (ErrorMessage, LE_ERROR_DebugMsgToFileAndScreen);
#endif
  }
  LI_GRAFIX_RemoveSystem ();

NormalExit:
#if !CE_ARTLIB_EnableSystemGrafix3D
  if (lpDirectDraw1Object != NULL)
    lpDirectDraw1Object->Release ();
#endif

  return ReturnCode;
}



// ===========================================================================
// Function:    LI_GRAFIX_PostSurfaceInit
//
// Description: Called to initialized the global dimensioning variables
//              having to do with the primary and rendering surfaces.
//
// Params:      bTestForSlowScrolling:  Should we do the scrolling test
//
// Returns:     BOOL
//
// Comments:    Returns TRUE if successful and FALSE otherwise.
// ===========================================================================
BOOL LI_GRAFIX_PostSurfaceInit(BOOL bTestForSlowScrolling)
{
  HRESULT               hDDReturnValue;
  DDSURFDESC            WorkingScreenSurfaceDesc;

  // The real screen bitmap size is taken directly from the
  // output surface size.

  LE_GRAFIX_RealScreenWidthInPixels = LI_GRAFIX_BestVideoModeSurfaceDesc.dwWidth;
  LE_GRAFIX_RealScreenHeightInPixels = LI_GRAFIX_BestVideoModeSurfaceDesc.dwHeight;

  // Find the source and destination rectangles.  The source rectangle
  // starts out with the size of the virtual screen, and the destination
  // rectangle is the size of the window which is displaying the "screen"
  // when in windowed mode.

  if (LE_GRAFIX_WindowedMode)
  {
    GetClientRect (LE_MAIN_HwndMainWindow, &LE_GRAFIX_ScreenDestinationRect);
    ClientToScreen (LE_MAIN_HwndMainWindow, (LPPOINT) &LE_GRAFIX_ScreenDestinationRect.left);
    ClientToScreen (LE_MAIN_HwndMainWindow, (LPPOINT) &LE_GRAFIX_ScreenDestinationRect.right);
  }
  else // Use the real screen when in full screen mode.
  {
    LE_GRAFIX_ScreenDestinationRect.left = 0;
    LE_GRAFIX_ScreenDestinationRect.right = LE_GRAFIX_RealScreenWidthInPixels;
    LE_GRAFIX_ScreenDestinationRect.top = 0;
    LE_GRAFIX_ScreenDestinationRect.bottom = LE_GRAFIX_RealScreenHeightInPixels;
  }

  // Set the source rectangle and virtual screen size to the size
  // of the working surface bitmap.
  ZeroMemory(&WorkingScreenSurfaceDesc, sizeof(DDSURFDESC));
  WorkingScreenSurfaceDesc.dwSize = sizeof(DDSURFDESC);
  hDDReturnValue = LE_GRAFIX_DDWorkingSurfaceN->GetSurfaceDesc(&WorkingScreenSurfaceDesc);
  if (DD_OK != hDDReturnValue)
    return(FALSE);

  LE_GRAFIX_VirtualScreenWidthInPixels = WorkingScreenSurfaceDesc.dwWidth;
  LE_GRAFIX_VirtualScreenHeightInPixels = WorkingScreenSurfaceDesc.dwHeight;

  LE_GRAFIX_ScreenSourceRect.left = 0;
  LE_GRAFIX_ScreenSourceRect.right = LE_GRAFIX_VirtualScreenWidthInPixels;
  LE_GRAFIX_ScreenSourceRect.top = 0;
  LE_GRAFIX_ScreenSourceRect.bottom = LE_GRAFIX_VirtualScreenHeightInPixels;

#if CE_ARTLIB_EnableDebugMode
  if (LE_GRAFIX_ScreenSourceRect.right - LE_GRAFIX_ScreenSourceRect.left !=
  LE_GRAFIX_ScreenDestinationRect.right - LE_GRAFIX_ScreenDestinationRect.left ||
  LE_GRAFIX_ScreenSourceRect.bottom - LE_GRAFIX_ScreenSourceRect.top !=
  LE_GRAFIX_ScreenDestinationRect.bottom - LE_GRAFIX_ScreenDestinationRect.top)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_InitSystem: "
      "Source rectangle (%d,%d)-(%d,%d) different size than "
      "destination rectangle (%d,%d)-(%d,%d).  Scaling will be done.\r\n",
      (int) LE_GRAFIX_ScreenSourceRect.left,
      (int) LE_GRAFIX_ScreenSourceRect.top,
      (int) LE_GRAFIX_ScreenSourceRect.right,
      (int) LE_GRAFIX_ScreenSourceRect.bottom,
      (int) LE_GRAFIX_ScreenDestinationRect.left,
      (int) LE_GRAFIX_ScreenDestinationRect.top,
      (int) LE_GRAFIX_ScreenDestinationRect.right,
      (int) LE_GRAFIX_ScreenDestinationRect.bottom);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  // Test for slow scrolling and set up global generic bitmaps for the
  // screen and working surface.
  if (bTestForSlowScrolling) LE_GRAFIX_TestForSlowScrolling();

  if (LE_GRAFIX_ScreenGBM != NULL)
    LE_DATA_FreeRuntimeDataID (LE_GRAFIX_ScreenGBM);
  LE_GRAFIX_ScreenGBM = LE_GRAFIX_CreateGBMFromDDSurface (LE_GRAFIX_DDPrimarySurfaceN);

  if (LE_GRAFIX_WorkGBM != NULL)
    LE_DATA_FreeRuntimeDataID (LE_GRAFIX_WorkGBM);
  LE_GRAFIX_WorkGBM = LE_GRAFIX_CreateGBMFromDDSurface (LE_GRAFIX_DDWorkingSurfaceN);

  // Allocate space for the invalid area clipping list.

  if (LI_GRAFIX_InvalidAreaClipper != NULL)
    LI_GRAFIX_InvalidAreaClipper->Release(), LI_GRAFIX_InvalidAreaClipper = NULL;

  if (DD_OK != DirectDrawCreateClipper (0, &LI_GRAFIX_InvalidAreaClipper, NULL))
    return(FALSE);

  // Allocate space for the window visible region tracking clipper, and connect
  // it to the window which we are using for determining the visible region.
  if (LI_GRAFIX_WindowVisibleRegionClipper != NULL)
    LI_GRAFIX_WindowVisibleRegionClipper->Release(), LI_GRAFIX_WindowVisibleRegionClipper = NULL;

  if (DD_OK != DirectDrawCreateClipper (0, &LI_GRAFIX_WindowVisibleRegionClipper, NULL))
    return(FALSE);

  if (LI_GRAFIX_WindowVisibleRegionClipper->SetHWnd(0, LE_MAIN_HwndMainWindow) != DD_OK)
    return(FALSE);

  return(TRUE);
}



/*************************************************************
*
* void LI_GRAFIX_RemoveSystem(void);
*
*   Description:
*       GRAFIX system cleanup and removal procedure.
*
*   Arguments:
*       None.
*
*   Returns:
*       None.
*
*   Notes, Bugs, Fixes, Changes, etc:
*       - 13:55:59 Tue  11-28-1995 (MMH)
*         Initial empty function.
*************************************************************/
void LI_GRAFIX_RemoveSystem(void)
{
  LI_GRAFIX_DeallocateScreenDepthRelatedThings ();

#if CE_ARTLIB_EnableMultitasking
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr != NULL)
    EnterCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
#endif

  if (LI_GRAFIX_InvalidAreaClipper != NULL)
  {
    LI_GRAFIX_InvalidAreaClipper->Release ();
    LI_GRAFIX_InvalidAreaClipper = NULL;
  }

  if (LI_GRAFIX_WindowVisibleRegionClipper != NULL)
  {
    LI_GRAFIX_WindowVisibleRegionClipper->SetHWnd (0, NULL);
    LI_GRAFIX_WindowVisibleRegionClipper->Release ();
    LI_GRAFIX_WindowVisibleRegionClipper = NULL;
  }

#if CE_ARTLIB_EnableSystemGrafix3D
  {
    BOOL  Successful;

    // destroys Direct Draw/3D resources, including the draw object itself
    Successful = DDRAW_Cleanup();
    assert (Successful);
    LE_GRAFIX_DirectDrawObjectN = NULL;
  }
#else // Running 2D system.

  if (LE_GRAFIX_DirectDrawObjectN != NULL)
    LE_GRAFIX_DirectDrawObjectN->Release ();
  LE_GRAFIX_DirectDrawObjectN = NULL;
#endif

#if CE_ARTLIB_EnableMultitasking
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr != NULL)
  {
    LE_GRAFIX_LockedSurfaceCriticalSectionPntr = NULL;
    DeleteCriticalSection (&LE_GRAFIX_LockedSurfaceCriticalSection);
  }
#endif
}



/*****************************************************************************
 * Invalidates the given rectangle in screen coordinates.  Usually used by
 * the window refresh code to redraw the screen after a switch to some other
 * program.  Use NULL for the whole screen.
 */

void LE_GRAFIX_InvalidateRect (RECT *RectPntr)
{
#if CE_ARTLIB_EnableSystemSequencer
  LE_SEQNCR_InvalidateScreenRect (RectPntr);
#endif
}



/*****************************************************************************
 * Convert the rectangles in the array from virtual screen coordinates (the
 * ones used by the working surface) to real screen coordinates.  Adds the
 * panning offset and scales them by the zoom factor.
 */

void LE_GRAFIX_ConvertRectanglesToScreenCoordinates (
LPRECT RectangleArray, int NumberOfRectangles)
{
  int     DestHeight;
  int     DestOffsetX;
  int     DestOffsetY;
  int     DestWidth;
  double  HeightScale;
  int     i;
  RECT   *RectPntr;
  int     SourceHeight;
  int     SourceOffsetX;
  int     SourceOffsetY;
  int     SourceWidth;
  double  WidthScale;

  // Transform the rectangles from virtual screen coordinates to real
  // screen coordinates only if needed.

  if (memcmp (&LE_GRAFIX_ScreenSourceRect, &LE_GRAFIX_ScreenDestinationRect,
  sizeof (LE_GRAFIX_ScreenDestinationRect)) != 0)
  {
    SourceWidth = LE_GRAFIX_ScreenSourceRect.right - LE_GRAFIX_ScreenSourceRect.left;
    if (SourceWidth <= 0) SourceWidth = 1;
    SourceHeight = LE_GRAFIX_ScreenSourceRect.bottom - LE_GRAFIX_ScreenSourceRect.top;
    if (SourceHeight <= 0) SourceHeight = 1;
    DestWidth = LE_GRAFIX_ScreenDestinationRect.right - LE_GRAFIX_ScreenDestinationRect.left;
    DestHeight = LE_GRAFIX_ScreenDestinationRect.bottom - LE_GRAFIX_ScreenDestinationRect.top;
    WidthScale = (double) DestWidth / (double) SourceWidth;
    HeightScale = (double) DestHeight / (double) SourceHeight;
    SourceOffsetX = LE_GRAFIX_ScreenSourceRect.left;
    SourceOffsetY = LE_GRAFIX_ScreenSourceRect.top;
    DestOffsetX = LE_GRAFIX_ScreenDestinationRect.left;
    DestOffsetY = LE_GRAFIX_ScreenDestinationRect.top;

    // Convert the rectangles to cover the destination screen area.
    // Round off to enlarge the rectangles so that we don't miss pixels.

    for (RectPntr = RectangleArray, i = NumberOfRectangles;
    i > 0;
    i--, RectPntr++)
    {
      RectPntr->left = (long) floor (((double) (RectPntr->left - SourceOffsetX)) * WidthScale);
      RectPntr->left += DestOffsetX;
      RectPntr->right = (long) ceil (((double) (RectPntr->right - SourceOffsetX)) * WidthScale);
      RectPntr->right += DestOffsetX;
      RectPntr->top = (long) floor (((double) (RectPntr->top - SourceOffsetY)) * HeightScale);
      RectPntr->top += DestOffsetY;
      RectPntr->bottom = (long) ceil (((double) (RectPntr->bottom - SourceOffsetY)) * HeightScale);
      RectPntr->bottom += DestOffsetY;
    }
  }
}



/*****************************************************************************
 * Copy the changed portions (identified by the region, NULL for nothing)
 * from the working surface to the output surface.  If the copy fails,
 * it will schedule a full update after the next successful copy (in case
 * there are several failures in a row).
 */

BOOL LE_GRAFIX_CopyWorkAreaToOutputArea (HRGN CopyPixelsInThisRegion)
{
  struct MyRegionStruct {
    RGNDATA header;
    BYTE    buffer[LI_REND_REGION_RECTANGLE_BUFFER_SPACE-1];
  };

  DDBLTFX               BlitEffects;
  BOOL                  ClipperAttached = FALSE;
  BOOL                  CriticalSectionEntered = FALSE;
  HRESULT               ErrorCode;
  static BOOL           LastFewBlitsFailed;
  DWORD                 MyDWORD;
  struct MyRegionStruct RegionData;
  BOOL                  ReturnCode = FALSE;
  HRGN                  TempRegion = NULL;
  HRGN                  WindowRegion = NULL;

  if (CopyPixelsInThisRegion == NULL ||
  LE_GRAFIX_ScreenDestinationRect.right - LE_GRAFIX_ScreenDestinationRect.left <= 0 ||
  LE_GRAFIX_ScreenDestinationRect.bottom - LE_GRAFIX_ScreenDestinationRect.top <= 0)
  {
    ReturnCode = TRUE; // Nothing needs drawing.
    goto NormalExit;
  }

  // Transform the clip list from virtual screen coordinates to real
  // screen coordinates - since we need to attach the clipper to the
  // output surface (you can attach it to the input surface but then
  // it takes a long time to copy it and copies stuff it shouldn't).

  if (memcmp (&LE_GRAFIX_ScreenSourceRect, &LE_GRAFIX_ScreenDestinationRect,
  sizeof (LE_GRAFIX_ScreenDestinationRect)) != 0)
  {
    // Get the user provided clipping region as rectangles.

    ErrorCode  = GetRegionData (CopyPixelsInThisRegion,
      LI_REND_REGION_RECTANGLE_BUFFER_SPACE, &RegionData.header);
    if (ErrorCode == 0 || RegionData.header.rdh.iType != RDH_RECTANGLES)
      goto ErrorExit; // Can't convert region to rectangles.

    // Convert the rectangles to cover the destination screen area.
    // Round off to enlarge the rectangles so that we don't miss pixels.

    LE_GRAFIX_ConvertRectanglesToScreenCoordinates (
      (LPRECT) (RegionData.header.Buffer + 0),
      RegionData.header.rdh.nCount);

    // Convert it back into a region.

    TempRegion = ExtCreateRegion (NULL /* Transformation matrix */,
      sizeof (RegionData.header) + RegionData.header.rdh.nCount * sizeof (RECT),
      &RegionData.header);

    if (TempRegion == NULL)
      goto ErrorExit;
  }

  // Protect global variables from multiple threads.

#if CE_ARTLIB_EnableMultitasking
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL)
    goto ErrorExit; // System is closed.
  EnterCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL)
    goto ErrorExit; // Closed while we waited.
  CriticalSectionEntered = TRUE;
#endif

  // Combine the user's redrawing area with the part of
  // the screen we are allowed to draw on.  But first need
  // to trick the system into giving us the "visible region",
  // which you normally can't get access to.

  MyDWORD = LI_REND_REGION_RECTANGLE_BUFFER_SPACE;
  ErrorCode = LI_GRAFIX_WindowVisibleRegionClipper->GetClipList (
    NULL /* No bounding rectangle */, &RegionData.header, &MyDWORD);
  if (ErrorCode != DD_OK) goto ErrorExit;

  WindowRegion = ExtCreateRegion (NULL /* Transformation matrix */,
    sizeof (RegionData.header) + RegionData.header.rdh.nCount * sizeof (RECT),
    &RegionData.header);
  if (WindowRegion == NULL) goto ErrorExit;

  // Now combine the window drawable region with the stuff we want to draw.

  ErrorCode = CombineRgn (WindowRegion, WindowRegion,
    (TempRegion == NULL) ? CopyPixelsInThisRegion : TempRegion, RGN_AND);
  if (ErrorCode == RGN_ERROR) goto ErrorExit;
  if (ErrorCode == NULLREGION)
  {
    ReturnCode = TRUE; // No drawing needed.
    goto NormalExit;
  }

  // Convert the result into rectangles.

  ErrorCode  = GetRegionData (WindowRegion,
    LI_REND_REGION_RECTANGLE_BUFFER_SPACE, &RegionData.header);
  if (ErrorCode == 0 || RegionData.header.rdh.iType != RDH_RECTANGLES)
    goto ErrorExit; // Can't convert region to rectangles.

  // Convert them into a DirectDraw clipper.

  ErrorCode = LI_GRAFIX_InvalidAreaClipper->SetClipList (
    &RegionData.header, 0);

  // Attach the clipper to the output surface.

  ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->SetClipper (
    LI_GRAFIX_InvalidAreaClipper);
  if (ErrorCode != DD_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("L_Rend2D.c CopyWorkAreaToOutputArea: "
      "Unable to attach clipper to output surface.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }
  ClipperAttached = TRUE;

  // Copy the whole area from working to output, but through the
  // clipper so that only the changed parts get drawn.  The rest
  // of the working surface contains garbage so we don't want to
  // actually copy the whole working screen.

#if CE_ARTLIB_GrafixShowBoundingBoxes
  {
    // Show the update area on the screen as a bunch of coloured rectangles.
    // The colour changes subtley every frame.  First colour the whole
    // redrawn area on the main screen then wait a while, then do the
    // rest of the drawing which will overwrite the colour with images.

    static DWORD UpdateColour = (DWORD) -1;

    memset (&BlitEffects, 0, sizeof (BlitEffects));
    BlitEffects.dwSize = sizeof (BlitEffects);
    BlitEffects.dwFillColor = UpdateColour++;

    ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->Blt (
      NULL /* whole surface */, NULL, NULL,
      DDBLT_WAIT | DDBLT_COLORFILL, &BlitEffects);

    LE_TIMER_Delay (CE_ARTLIB_GrafixShowBoundingBoxesDelay);
  }
#endif

  memset (&BlitEffects, 0, sizeof (BlitEffects));
  BlitEffects.dwSize = sizeof (BlitEffects);
  BlitEffects.dwROP = SRCCOPY;

  ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->Blt (
    &LE_GRAFIX_ScreenDestinationRect, LE_GRAFIX_DDWorkingSurfaceN,
    &LE_GRAFIX_ScreenSourceRect, DDBLT_WAIT | DDBLT_ROP, &BlitEffects);

  // Remove the clipper.  Have to do that otherwise the lock operations will
  // fail when drawing to the surface using a memory pointer.

  LE_GRAFIX_DDPrimarySurfaceN->SetClipper (NULL);
  ClipperAttached = FALSE;

  // Ok, now check the result of the blit, did it work?  If it failed
  // because the surface was lost, restore the surface and request a
  // complete redraw if it was the real screen.

  if (ErrorCode == DDERR_SURFACELOST)
  {
    if (DDERR_SURFACELOST == LE_GRAFIX_DDPrimarySurfaceN->IsLost ())
    {
      LE_GRAFIX_DDPrimarySurfaceN->Restore ();
      LE_SEQNCR_InvalidateScreenRect (NULL); // Was drawing to screen.
    }

    // Also check the working surface.

    if (DDERR_SURFACELOST == LE_GRAFIX_DDWorkingSurfaceN->IsLost ())
      LE_GRAFIX_DDWorkingSurfaceN->Restore ();
  }
  else if (ErrorCode != DD_OK)
    goto ErrorExit;

  ReturnCode = TRUE; // Successful!
  goto NormalExit;


ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  LE_ERROR_DebugMsg ("LE_GRAFIX_CopyWorkAreaToOutputArea: "
    "Can't draw on output bitmap.\r\n", LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:

  if (ClipperAttached)
    LE_GRAFIX_DDPrimarySurfaceN->SetClipper (NULL);

#if CE_ARTLIB_EnableMultitasking
  if (CriticalSectionEntered)
    LeaveCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
#endif

  if (TempRegion != NULL)
    DeleteObject (TempRegion);

  if (WindowRegion != NULL)
    DeleteObject (WindowRegion);

  // If we were successful this time and had failed before, then schedule
  // a full screen update to make up for the failed blits.  If we weren't
  // successful then set the flag to do a full update later, when DirectX
  // is working again.

  if (ReturnCode)
  {
    if (LastFewBlitsFailed)
    {
      LE_GRAFIX_InvalidateRect (NULL);
      LastFewBlitsFailed = FALSE;
    }
  }
  else // We failed, try later.
    LastFewBlitsFailed = TRUE;

  return ReturnCode;
}



/************************************************************/
/* LE_GRAFIX_ObjectCreate                                   */
/*                                                          */
/* This function allocates and initializes a memory region  */
/* of the appropriate size for the object size passed in.   */
/* The return value is a imaginary datafile id and to be    */
/* cleaned up or removed deallocate the return_value or use */
/* it in a call to LE_GRAFIX_ObjectRemove.                  */
/*                                                          */
/* w = width of the object in pixels.                       */
/* h = height of the object in pixels.                      */
/* memflag = mem priority to allocate the object with.      */
/* transp = the initial transparency value for the object.  */
/*   use LE_GRAFIX_ObjectTransparent or                     */
/*       LE_GRAFIX_ObjectNotTransparent                     */
/************************************************************/
LE_DATA_DataId LE_GRAFIX_ObjectCreate(short w, short h, WORD transp)
{
  LE_DATA_DataId return_value = LE_DATA_EmptyItem;
  long malloc_size = 0;
  LPNEWBITMAPHEADER lpBMH = NULL;

  // calculate bitmap bits size
  if      (LE_GRAFIX_ScreenBitsPerPixel == 16) malloc_size = (((long)w * 2) + 3) & 0xfffffffc;
  else if (LE_GRAFIX_ScreenBitsPerPixel == 32) malloc_size =   (long)w * 4;
  else if (LE_GRAFIX_ScreenBitsPerPixel == 24) malloc_size = (((long)w * 3) + 3) & 0xfffffffc;
  else /* Assume 8 bits per pixel */           malloc_size =  ((long)w + 3) & 0xfffffffc;

  malloc_size *= h;
  malloc_size += sizeof(NEWBITMAPHEADER);

  return_value = LE_DATA_AllocMemoryDataID (malloc_size, LE_DATA_DataNative);

  if (return_value == LE_DATA_EmptyItem)
    return LE_DATA_EmptyItem; // Out of memory.

  lpBMH = (LPNEWBITMAPHEADER) LE_DATA_Use (return_value);

  lpBMH->nXBitmapWidth    = w;
  lpBMH->nYBitmapHeight   = h;
  lpBMH->nXOriginOffset   = 0;
  lpBMH->nYOriginOffset   = 0;
  lpBMH->dwFlags          = transp ? BITMAP_NOTRANSPARENCY: 0;
  lpBMH->nColors          = 0;
  lpBMH->nAlpha           = ALPHA_OPAQUE100_0;  // from l_anim.h

  // Initialize the data for this object - appropriate colour.

  if (transp == LE_GRAFIX_ObjectNotTransparent)
    LE_GRAFIX_ColorArea(return_value, 0, 0, w, h, LEG_MCR(0,0,0));
  else
    LE_GRAFIX_ColorArea(return_value, 0, 0, w, h,
#if CE_ARTLIB_BlitUseGreenColourKey
    LEG_MCR(0,255,0));
#elif CE_ARTLIB_BlitUseBlueColourKey
    LEG_MCR(0,0,255));
#else
    LEG_MCR(0,0,0));
#endif

  return (return_value);
}



/************************************************************/
/* LE_GRAFIX_ObjectCreateAlpha                              */
/************************************************************/
LE_DATA_DataId LE_GRAFIX_ObjectCreateAlpha(short w, short h, WORD transp, short ALPHA_TRANS_VALUE)
{
    LE_DATA_DataId return_value = LE_DATA_EmptyItem;
    long malloc_size = 0;
    LPNEWBITMAPHEADER lpBMH = NULL;

  // calculate bitmap bits size
    if (LE_GRAFIX_ScreenBitsPerPixel == 8)       malloc_size =  ((long)w + 3) & 0xfffffffc;
    else if (LE_GRAFIX_ScreenBitsPerPixel == 16) malloc_size = (((long)w * 2) + 3) & 0xfffffffc;
    else if (LE_GRAFIX_ScreenBitsPerPixel == 24) malloc_size = (((long)w * 3) + 3) & 0xfffffffc;
    else if (LE_GRAFIX_ScreenBitsPerPixel == 32) malloc_size =   (long)w * 4;

    malloc_size *= h;
    malloc_size += sizeof(NEWBITMAPHEADER);

    return_value = LE_DATA_AllocMemoryDataID (malloc_size, LE_DATA_DataNative);
    if (return_value == LE_DATA_EmptyItem)
        return LE_DATA_EmptyItem; // Out of memory.

    lpBMH = (LPNEWBITMAPHEADER) LE_DATA_Use (return_value);

    lpBMH->nXBitmapWidth    = w;
    lpBMH->nYBitmapHeight   = h;
    lpBMH->nXOriginOffset   = 0;
    lpBMH->nYOriginOffset   = 0;
    lpBMH->dwFlags          = transp ? BITMAP_NOTRANSPARENCY: 0;
    lpBMH->nColors          = 0;
    lpBMH->nAlpha           = ALPHA_TRANS_VALUE;

    // Initialize the data for this object.
    if (transp == LE_GRAFIX_ObjectNotTransparent) LE_GRAFIX_ColorArea(return_value, 0, 0, w, h, LEG_MCR(0,0,0));
    else
    {
        if (LE_GRAFIX_ScreenBitsPerPixel == 8) LE_GRAFIX_ColorArea(return_value, 0, 0, w, h, LEG_MCR(0,0,0));
        else                          LE_GRAFIX_ColorArea(return_value, 0, 0, w, h, LEG_MCR(0,255,0));
    }

    return (return_value);
}
/************************************************************/

/************************************************************/
/* LE_GRAFIX_ChangeObjectAlpha                              */
/************************************************************/
void LE_GRAFIX_ChangeObjectAlpha(LE_DATA_DataId object_handle, short new_ALPHA_TRANS_VALUE)
{
    LPNEWBITMAPHEADER lpBMH;

    lpBMH = (LPNEWBITMAPHEADER) LE_DATA_Use (object_handle);

    if (lpBMH != NULL)
      lpBMH->nAlpha = new_ALPHA_TRANS_VALUE;
};
/************************************************************/

/************************************************************/
/* LE_GRAFIX_LockObjectDataPtr                                */
/************************************************************/
LPBYTE LE_GRAFIX_LockObjectDataPtr(LE_DATA_DataId object_handle)
{
  DDSURFDESC        DDObjectSurfaceDescriptor;
  LPBYTE            lpColorTable;
  LPBYTE            lpSurfaceBits;
  LPNEWBITMAPHEADER TCB_ptr;

  if (object_handle == LE_DATA_EmptyItem) // Accessing the real screen.
  {
#if CE_ARTLIB_EnableMultitasking
    if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return NULL; // System is closed.
    EnterCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
    if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return NULL; // Closed while we waited.
#endif

    lpSurfaceBits = NULL;

    if (LE_GRAFIX_DDPrimarySurfaceN != NULL)
    {
      DDObjectSurfaceDescriptor.dwSize = sizeof (DDSURFDESC);
      DDObjectSurfaceDescriptor.dwFlags = 0;

      if (DD_OK == LE_GRAFIX_DDPrimarySurfaceN->Lock (
      NULL /* Whole surface */, &DDObjectSurfaceDescriptor,
      DDLOCK_SURFACEMEMORYPTR, NULL))
        lpSurfaceBits = (LPBYTE) DDObjectSurfaceDescriptor.lpSurface;
    }
    return lpSurfaceBits;
  }

  TCB_ptr = (LPNEWBITMAPHEADER) LE_DATA_Use (object_handle);
  if (TCB_ptr != NULL)
    lpColorTable = (LPBYTE)&TCB_ptr->dwColorTable;
  else
    lpColorTable = NULL;

  return lpColorTable;
}



void LE_GRAFIX_UnlockObjectDataPtr(LE_DATA_DataId object_handle)
{
  if (object_handle == LE_DATA_EmptyItem)
  {
    if (LE_GRAFIX_DDPrimarySurfaceN != NULL)
      LE_GRAFIX_DDPrimarySurfaceN->Unlock (NULL /* Whole surface */);


#if CE_ARTLIB_EnableMultitasking
    if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr != NULL)
      LeaveCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
#endif
  }
}



/************************************************************/

/************************************************************
 * LE_GRAFIX_CopyObject
 ************************************************************/
LE_DATA_DataId LE_GRAFIX_CopyObject (WORD Transparency, LE_DATA_DataId SourceObject )
{
  LE_DATA_DataId  NewObject;
  RECT            Rect;
  LPBYTE          SourceBits,     NewBits;
  short           Width,          Height;

  Width = LE_GRAFIX_ReturnObjectWidth( SourceObject );
  Height = LE_GRAFIX_ReturnObjectHeight( SourceObject );
  SetRect( &Rect, 0, 0, Width, Height );
  NewObject = LE_GRAFIX_ObjectCreate( Width, Height, Transparency );
  if ( NewObject != LED_EI )
  {
    SourceBits = LE_GRAFIX_LockObjectDataPtr( SourceObject );
    NewBits = LE_GRAFIX_LockObjectDataPtr( NewObject );
    if ( LE_GRAFIX_ScreenBitsPerPixel == 16 )
    {
      LI_BLT_Copy16To16SolidUnityRaw (SourceBits, Width, Height, &Rect, NewBits, Width, Height, &Rect, &Rect, 0, 0);
    }
    else if ( LE_GRAFIX_ScreenBitsPerPixel == 24 )
    {
      LI_BLT_Copy24To24SolidUnityRaw (SourceBits, Width, Height, &Rect, NewBits, Width, Height, &Rect, &Rect, 0, 0);
    }
    else if ( LE_GRAFIX_ScreenBitsPerPixel == 32 )
    {
      LI_BLT_Copy32To32SolidUnityRaw (SourceBits, Width, Height, &Rect, NewBits, Width, Height, &Rect, &Rect, 0, 0);
    }
    LE_GRAFIX_UnlockObjectDataPtr( SourceObject );
    LE_GRAFIX_UnlockObjectDataPtr( NewObject );
  }
  return NewObject;
}



/************************************************************/
/* LE_GRAFIX_ReturnObjectWidth                              */
/************************************************************/
WORD LE_GRAFIX_ReturnObjectWidth(LE_DATA_DataId object_handle)
{
  if (object_handle == LE_DATA_EmptyItem)
  {
    return LE_GRAFIX_RealScreenWidthInPixels;
  }
  else
  {
    WORD width;
    LPNEWBITMAPHEADER tt;

    tt = (LPNEWBITMAPHEADER) LE_DATA_Use (object_handle);
    if (tt != NULL)
      width = tt->nXBitmapWidth;
    else
      width = 0;

#if CE_ARTLIB_EnableDebugMode
    if ((!width)||(width == 0xffff))
    {
      sprintf(LE_ERROR_DebugMessageBuffer,"LE_GRAFIX_ReturnObjectWidth: Width = zero.\r\n");
      LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    }
#endif

    return width;
  }
}



/************************************************************/
/* LE_GRAFIX_ReturnObjectHeight                             */
/************************************************************/
WORD LE_GRAFIX_ReturnObjectHeight(LE_DATA_DataId object_handle)
{
  WORD height;
  LPNEWBITMAPHEADER tt;

  if (object_handle == LE_DATA_EmptyItem)
  {
    return LE_GRAFIX_RealScreenHeightInPixels;
  }
  else
  {
    tt = (LPNEWBITMAPHEADER) LE_DATA_Use (object_handle);
    if (tt != NULL)
      height = tt->nYBitmapHeight;
    else
      height = 0;

#if CE_ARTLIB_EnableDebugMode
    if ((!height)||(height == 0xffff))
    {
      sprintf(LE_ERROR_DebugMessageBuffer,"LE_GRAFIX_ReturnObjectHeight: Height = zero.\r\n");
      LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    }
#endif

    return height;
  }
}



/************************************************************/
/* LE_GRAFIX_ReturnObjectTransparency                       */
/************************************************************/
DWORD LE_GRAFIX_ReturnObjectTransparency(LE_DATA_DataId object_handle)
{
  LPNEWBITMAPHEADER tt;

  if (object_handle == LE_DATA_EmptyItem)
    return(LE_GRAFIX_ObjectNotTransparent);
  else
  {
    tt = (LPNEWBITMAPHEADER) LE_DATA_Use (object_handle);
    if (tt == NULL)
      return LE_GRAFIX_ObjectNotTransparent;
    else
      return (tt->dwFlags & BITMAP_NOTRANSPARENCY);
  }
}



/************************************************************/
/* LE_GRAFIX_ColorArea                                      */
/*                                                          */
/* object_handle = A previously and still valid object      */
/*                 created with LE_GRAFIX_ObjectCreate.     */
/*                 Pass in LE_DATA_EmptyItem if you wish    */
/*                 the effect to affect the current         */
/*                 background instead of an object.         */
/* x = top left destination coordinate (in pixels).         */
/* y = top left destination coordinate (in pixels).         */
/* w = width in pixels of area to color.                    */
/* h = height in pixels of area to color.                   */
/* color = color to use, must be a COLORREF                 */
/************************************************************/
void LE_GRAFIX_ColorArea(LE_DATA_DataId object_handle, short x, short y, short w, short h, DWORD color)
{
    LPBYTE dest_ptr = NULL;
    WORD dest_width = 0;
    WORD dest_height = 0;
    RECT dest_rect;

    #if CE_ARTLIB_EnableDebugMode
        if (object_handle == LED_EI)
        {
            static BOOL first_time = TRUE;

            wsprintf(LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_ColorArea: Display straight to the former background (LE_DATA_EmptyItem/LED_EI) is no longer supported!");

            if (first_time) LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
            else            LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);

            first_time = FALSE;
        }
    #endif

    dest_ptr    = LE_GRAFIX_LockObjectDataPtr(object_handle);
    if (dest_ptr == NULL)
      return; // Failed to lock.

    dest_width  = LE_GRAFIX_ReturnObjectWidth(object_handle);
    dest_height = LE_GRAFIX_ReturnObjectHeight(object_handle);

    dest_rect.left   = x;
    dest_rect.top    = y;
    dest_rect.right  = x+w;
    dest_rect.bottom = y+h;

    // Clip the rectangle to fit the destination object.
    if (dest_rect.left < 0) dest_rect.left = 0;
    if (dest_rect.right > dest_width) dest_rect.right = dest_width;
    if (dest_rect.top < 0) dest_rect.top = 0;
    if (dest_rect.bottom > dest_height) dest_rect.bottom = dest_height;
    if (dest_rect.left < dest_rect.right && dest_rect.top < dest_rect.bottom)
    {
      // If there is something left after clipping, draw it.
      if (LE_GRAFIX_ScreenBitsPerPixel == 16) LI_BLT_Fill16Raw (color, FALSE, dest_ptr, dest_width, dest_height, &dest_rect);
      else if (LE_GRAFIX_ScreenBitsPerPixel == 24) LI_BLT_Fill24Raw (color, FALSE, dest_ptr, dest_width, dest_height, &dest_rect);
      else if (LE_GRAFIX_ScreenBitsPerPixel == 32) LI_BLT_Fill32Raw (color, FALSE, dest_ptr, dest_width, dest_height, &dest_rect);
    }

    LE_GRAFIX_UnlockObjectDataPtr(object_handle);
}
/************************************************************/

/************************************************************/
/* LE_GRAFIX_ShowTCB                                        */
/*                                                          */
/*                                                          */
/* object_handle = A previously and still valid object      */
/*                 created with LE_GRAFIX_ObjectCreate.     */
/*                 Pass in LE_DATA_EmptyItem if you wish    */
/*                 the effect to affect the current         */
/*                 background instead of an object.         */
/* x = top left destination coordinate (in pixels).         */
/* y = top left destination coordinate (in pixels).         */
/************************************************************/
void LE_GRAFIX_ShowTCB (LE_DATA_DataId object_handle,
short dest_x, short dest_y, LE_DATA_DataId TCB_tag)
{
  LPBYTE   dest_ptr;
  WORD     dest_width;
  WORD     dest_height;
  RECT     src_rect;
  short    src_w;
  short    src_h;
  LPBYTE   lpColorTable;
  DWORD    notransp;
  LPBYTE   lpBitmapBits;
  LPNEWBITMAPHEADER TCB_ptr;

    #if CE_ARTLIB_EnableDebugMode
        if (object_handle == LED_EI)
        {
            static BOOL first_time = TRUE;

            wsprintf(LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_ShowTCB: Display straight to the former background (LE_DATA_EmptyItem/LED_EI) is no longer supported!");

            if (first_time) LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
            else            LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);

            first_time = FALSE;
        }
    #endif

  // Get or make sure TCB into memory
  TCB_ptr = (LPNEWBITMAPHEADER) LE_DATA_Use (TCB_tag);
  if (TCB_ptr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    wsprintf (LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf (LE_ERROR_DebugMessageBuffer,"LE_GRAFIX_ShowTCB(): Failed to load TCB. TCB_ptr = NULL.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf (LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
#endif
  }
  else // Sucessfully loaded the 8 bit image data.
  {
    // Get info from logo
    lpColorTable  = (LPBYTE) &TCB_ptr->dwColorTable;
    lpBitmapBits  = lpColorTable + (TCB_ptr->nColors * sizeof(LONG) * 2);
    src_w         = TCB_ptr->nXBitmapWidth;
    src_h         = TCB_ptr->nYBitmapHeight;
    notransp      = TCB_ptr->dwFlags & BITMAP_NOTRANSPARENCY;

    src_rect.left   = 0;
    src_rect.top    = 0;
    src_rect.right  = src_w;
    src_rect.bottom = src_h;

    dest_ptr    = LE_GRAFIX_LockObjectDataPtr(object_handle);
    dest_width  = LE_GRAFIX_ReturnObjectWidth(object_handle);
    dest_height = LE_GRAFIX_ReturnObjectHeight(object_handle);

    // Account for possible transparency offsets in image.
    // Mike: Deal with this sometime.
    //        dest_x += TCB_ptr->nXOriginOffset;
    //        dest_y += TCB_ptr->nYOriginOffset;

    // Clip the source rectangle so that it includes only the
    // stuff that fits in the destination.  Fortunately we aren't
    // doing scaling here.

    if (dest_x + src_rect.right > dest_width)
      src_rect.right = dest_width - dest_x;
    if (dest_y + src_rect.bottom > dest_height)
      src_rect.bottom = dest_height - dest_y;
    if (dest_x < 0)
    {
      src_rect.left -= dest_x;
      dest_x = 0;
    }
    if (dest_y < 0)
    {
      src_rect.top -= dest_y;
      dest_y = 0;
    }

    if (src_rect.top < src_rect.bottom && src_rect.left < src_rect.right)
    {
      // There are some pixels still in the source rectangle, copy them.

      if (LE_GRAFIX_ScreenBitsPerPixel == 16)
      {
        if (notransp)           LI_BLT16BitBlt8to16         (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
        else                    LI_BLT16ColorKeyBitBlt8to16 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
      }
      else if (LE_GRAFIX_ScreenBitsPerPixel == 24)
      {
        if (notransp)           LI_BLT24BitBlt8to24         (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
        else                    LI_BLT24ColorKeyBitBlt8to24 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
      }
      else if (LE_GRAFIX_ScreenBitsPerPixel == 32)
      {
        if (notransp)           LI_BLT32BitBlt8to32         (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
        else                    LI_BLT32ColorKeyBitBlt8to32 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
      }
    }
    LE_GRAFIX_UnlockObjectDataPtr(object_handle);
  }
}
/************************************************************/

/************************************************************/
/* LE_GRAFIX_ShowTCB_Alpha                                  */
/************************************************************/
void LE_GRAFIX_ShowTCB_Alpha (LE_DATA_DataId object_handle,
short dest_x, short dest_y, LE_DATA_DataId TCB_tag)
{
  LPBYTE   dest_ptr;
  WORD     dest_width;
  WORD     dest_height;
  RECT     src_rect;
  short    src_w;
  short    src_h;
  LPBYTE   lpColorTable;
  int      nAlpha;
  DWORD    notransp;
  DWORD    bAlphaChannel;
  LPBYTE   lpBitmapBits;
  LPNEWBITMAPHEADER TCB_ptr;

    #if CE_ARTLIB_EnableDebugMode
        if (object_handle == LED_EI)
        {
            static BOOL first_time = TRUE;

            wsprintf(LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_ShowTCB_Alpha: Display straight to the former background (LE_DATA_EmptyItem/LED_EI) is no longer supported!");

            if (first_time) LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
            else            LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);

            first_time = FALSE;
        }
    #endif

  // Get or make sure TCB into memory
  TCB_ptr = (LPNEWBITMAPHEADER) LE_DATA_Use (TCB_tag);
  if (TCB_ptr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    wsprintf (LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf (LE_ERROR_DebugMessageBuffer,"LE_GRAFIX_ShowTCBAlpha(): Failed to load TCB. TCB_ptr = NULL.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf (LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
#endif
  }
  else // Sucessfully loaded the 8 bit image data.
  {
    // Get info from logo
    lpColorTable  = (LPBYTE)&TCB_ptr->dwColorTable;
    lpBitmapBits  = lpColorTable + (TCB_ptr->nColors * sizeof(LONG) * 2);
    nAlpha        = TCB_ptr->nAlpha;
    src_w         = TCB_ptr->nXBitmapWidth;
    src_h         = TCB_ptr->nYBitmapHeight;
    notransp      = TCB_ptr->dwFlags & BITMAP_NOTRANSPARENCY;
    bAlphaChannel = TCB_ptr->dwFlags & BITMAP_ALPHACHANNEL;

    src_rect.left   = 0;
    src_rect.top    = 0;
    src_rect.right  = src_w;
    src_rect.bottom = src_h;

    dest_ptr    = LE_GRAFIX_LockObjectDataPtr(object_handle);
    dest_width  = LE_GRAFIX_ReturnObjectWidth(object_handle);
    dest_height = LE_GRAFIX_ReturnObjectHeight(object_handle);

    // Account for possible transparency offsets in image.
    // Mike: Deal with this sometime.
    //        dest_x += TCB_ptr->nXOriginOffset;
    //        dest_y += TCB_ptr->nYOriginOffset;

    // Clip the source rectangle so that it includes only the
    // stuff that fits in the destination.  Fortunately we aren't
    // doing scaling here.

    if (dest_x + src_rect.right > dest_width)
      src_rect.right = dest_width - dest_x;
    if (dest_y + src_rect.bottom > dest_height)
      src_rect.bottom = dest_height - dest_y;
    if (dest_x < 0)
    {
      src_rect.left -= dest_x;
      dest_x = 0;
    }
    if (dest_y < 0)
    {
      src_rect.top -= dest_y;
      dest_y = 0;
    }

    if (src_rect.top < src_rect.bottom && src_rect.left < src_rect.right)
    {
      // There are some pixels still in the source rectangle, copy them.

      if (LE_GRAFIX_ScreenBitsPerPixel == 16)
      {
        if (notransp)           LI_BLT16BitBlt8to16         (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
        else if (bAlphaChannel) LI_BLT16AlphaBitBlt8to16    (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable, nAlpha);
        else                    LI_BLT16ColorKeyBitBlt8to16 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
      }
      else if (LE_GRAFIX_ScreenBitsPerPixel == 24)
      {
        if (notransp)           LI_BLT24BitBlt8to24         (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
        else if (bAlphaChannel) LI_BLT24AlphaBitBlt8to24    (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable, nAlpha);
        else                    LI_BLT24ColorKeyBitBlt8to24 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
      }
      else if (LE_GRAFIX_ScreenBitsPerPixel == 32)
      {
        if (notransp)           LI_BLT32BitBlt8to32         (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
        else if (bAlphaChannel) LI_BLT32AlphaBitBlt8to32    (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable, nAlpha);
        else                    LI_BLT32ColorKeyBitBlt8to32 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, (LPLONG)lpColorTable);
      }
    }
    LE_GRAFIX_UnlockObjectDataPtr(object_handle);
  }
}
/************************************************************/

/************************************************************/
/* LE_GRAFIX_TCBwidth                                       */
/************************************************************/
short LE_GRAFIX_TCBwidth(LE_DATA_DataId tcb_handle)
{
    LPNEWBITMAPHEADER lpbi = NULL;

    lpbi = (LPNEWBITMAPHEADER) LE_DATA_Use (tcb_handle);
    if (lpbi != NULL) return(lpbi->nXBitmapWidth);
    else return(0);
}
/************************************************************/

/************************************************************/
/* LE_GRAFIX_TCBheight                                      */
/************************************************************/
short LE_GRAFIX_TCBheight(LE_DATA_DataId tcb_handle)
{
    LPNEWBITMAPHEADER lpbi = NULL;

    lpbi = (LPNEWBITMAPHEADER) LE_DATA_Use (tcb_handle);
    if (lpbi != NULL) return(lpbi->nYBitmapHeight);
    else return(0);
}
/************************************************************/

/************************************************************/
/* LE_GRAFIX_AnyBitmapHeight                                */
/* Returns the height of any bitmap, including FUL, TAB and */
/* dynamic "Object" bitmaps.  Returns zero if it fails.     */
/************************************************************/
short LE_GRAFIX_AnyBitmapHeight (LE_DATA_DataId DataID)
{
  LE_DATA_DataType DataType;

  // Is it an in-memory "object" allocated by the user?

  if (DataID == LE_DATA_EmptyItem /* The real screen */)
      return LE_GRAFIX_ReturnObjectHeight (DataID);

  // Ok, something in a data file.  Data type should describe it.

  DataType = LE_DATA_GetLoadedDataType (DataID);

  if (DataType == LE_DATA_DataNative)
      return LE_GRAFIX_ReturnObjectHeight (DataID);

  if (DataType == LE_DATA_DataUAP)
      return LE_GRAFIX_TCBheight (DataID);

  if (DataType == LE_DATA_DataGenericBitmap)
    return LE_GRAFIX_GetGBMHeight (DataID);

  return 0; // Don't know what this is.
}
/************************************************************/


/************************************************************/
/* LE_GRAFIX_AnyBitmapWidth                                 */
/* Returns the width of any bitmap, including FUL, TAB and  */
/* dynamic "Object" bitmaps.  Returns zero if it fails.     */
/************************************************************/
short LE_GRAFIX_AnyBitmapWidth (LE_DATA_DataId DataID)
{
  LE_DATA_DataType DataType;

  // Is it an in-memory "object" allocated by the user?

  if (DataID == LE_DATA_EmptyItem /* The real screen */)
      return LE_GRAFIX_ReturnObjectWidth (DataID);

  // Ok, something in a data file.  Data type should describe it.

  DataType = LE_DATA_GetLoadedDataType (DataID);

  if (DataType == LE_DATA_DataNative)
      return LE_GRAFIX_ReturnObjectWidth (DataID);

  if (DataType == LE_DATA_DataUAP)
      return LE_GRAFIX_TCBwidth (DataID);

  if (DataType == LE_DATA_DataGenericBitmap)
    return LE_GRAFIX_GetGBMWidth (DataID);

  return 0; // Don't know what this is.
}
/************************************************************/



/************************************************************
 * LE_GRAFIX_StoreArea
 ************************************************************/
LE_DATA_DataId LE_GRAFIX_StoreArea(short x, short y, short w, short h)
{
  RECT              AreaRect;
  LPNEWBITMAPHEADER Header;
  LE_DATA_DataId    NewObject;
  LPBYTE            NewObjectBitsPointer;
  LPBYTE            ScreenBits;

  /* Create an initial object of the correct size. */

  NewObject = LE_GRAFIX_ObjectCreate( w, h, LE_GRAFIX_ObjectNotTransparent );
  if ( NewObject == LED_EI )
    return LED_EI;

  // Stash the screen position of the top left corner of the object
  // in the offset fields of the structure.

  Header = (LPNEWBITMAPHEADER) LE_DATA_Use (NewObject);
  Header->nXOriginOffset = x;
  Header->nYOriginOffset = y;

  /* Keep a pointer to the screen. */

  ScreenBits = LE_GRAFIX_LockObjectDataPtr (LE_DATA_EmptyItem);
  if (ScreenBits == NULL)
    return LED_EI;

  /* Keep a pointer to the bits of the new object. */

  NewObjectBitsPointer = LE_GRAFIX_LockObjectDataPtr( NewObject );
  if (NewObjectBitsPointer == NULL)
  {
    LE_GRAFIX_UnlockObjectDataPtr (LE_DATA_EmptyItem);
    return LED_EI;
  }

  /* Copy screen bits into the object. */

  SetRect( &AreaRect, x, y, x + w, y + h );

  if (LE_GRAFIX_ScreenBitsPerPixel == 16)
    LI_BLT16BitBlt16to16(NewObjectBitsPointer,
    w,
    0, 0,
    ScreenBits,
    LE_GRAFIX_VirtualScreenWidthInPixels,
    &AreaRect );
  else if (LE_GRAFIX_ScreenBitsPerPixel == 24)
    LI_BLT24BitBlt24to24(NewObjectBitsPointer,
    w,
    0, 0,
    ScreenBits,
    LE_GRAFIX_VirtualScreenWidthInPixels,
    &AreaRect );
  else if (LE_GRAFIX_ScreenBitsPerPixel == 32)
    LI_BLT32BitBlt32to32(NewObjectBitsPointer,
    w,
    0, 0,
    ScreenBits,
    LE_GRAFIX_VirtualScreenWidthInPixels,
    &AreaRect );

  /* Free up stuff, we're done. */

  LE_GRAFIX_UnlockObjectDataPtr (LE_DATA_EmptyItem);
  LE_GRAFIX_UnlockObjectDataPtr (NewObject);

  return NewObject;
}



/************************************************************/
/* LE_GRAFIX_RetrieveArea                                   */
/************************************************************/

void LE_GRAFIX_RetrieveArea(LE_DATA_DataId HR)
{
  LPNEWBITMAPHEADER Header;
  WORD              src_width;
  RECT              src_rect;
  LPBYTE            src_ptr = NULL;
  LPBYTE            dest_ptr = NULL;
  WORD              dest_width;
  RECT              dest_rect;

  if (HR == LE_DATA_EmptyItem)
    return; // Nothing to restore from.

  Header = (LPNEWBITMAPHEADER) LE_DATA_Use(HR);
  if (Header == NULL)
    return;

  src_width = Header->nXBitmapWidth;

  src_rect.left   = 0;
  src_rect.top    = 0;
  src_rect.right  = Header->nXBitmapWidth;
  src_rect.bottom = Header->nYBitmapHeight;

  dest_rect.left   = Header->nXOriginOffset;
  dest_rect.top    = Header->nYOriginOffset;
  dest_rect.right  = Header->nXOriginOffset + Header->nXBitmapWidth;
  dest_rect.bottom = Header->nYOriginOffset + Header->nYBitmapHeight;

  src_ptr = LE_GRAFIX_LockObjectDataPtr(HR);

  dest_ptr     = LE_GRAFIX_LockObjectDataPtr(LED_EI);
  dest_width   = LE_GRAFIX_ReturnObjectWidth(LED_EI);

  if (LE_GRAFIX_ScreenBitsPerPixel == 16)
    LI_BLT16BitBlt16to16 (dest_ptr, dest_width, dest_rect.left, dest_rect.top, src_ptr, src_width, &src_rect);
  else if (LE_GRAFIX_ScreenBitsPerPixel == 24)
    LI_BLT24BitBlt24to24 (dest_ptr, dest_width, dest_rect.left, dest_rect.top, src_ptr, src_width, &src_rect);
  else if (LE_GRAFIX_ScreenBitsPerPixel == 32)
    LI_BLT32BitBlt32to32 (dest_ptr, dest_width, dest_rect.left, dest_rect.top, src_ptr, src_width, &src_rect);

  LE_GRAFIX_UnlockObjectDataPtr(LED_EI);
  LE_GRAFIX_UnlockObjectDataPtr(HR);
}



/************************************************************/
/* LE_GRAFIX_ForceUpdateRect                                */
/************************************************************/
void LE_GRAFIX_ForceUpdateRect(RECT *display)
{
}
/************************************************************/

/************************************************************/

/************************************************************/
/* LE_GRAFIX_Show24                                         */
/************************************************************/
void LE_GRAFIX_Show24(LE_DATA_DataId object_handle, short dest_x, short dest_y, LE_DATA_DataId FUL_tag, BOOL transp)
{
    LPBYTE              final_ptr = NULL;
    WORD                final_width = 0;
    WORD                final_height = 0;
    RECT                src_rect;
    RECT                final_rect;
    short               src_w;
    short               src_h;
    LPBYTE              FUL_colordata_ptr = NULL;
    LPBITMAPFILEHEADER  FUL_fh_ptr = NULL;
    LPBITMAPINFOHEADER  FUL_info_ptr = NULL;
    long                malloc_size = 0;
    LPBYTE              transp_alloc_ptr = NULL;
    LE_DATA_DataId      transp_alloc_handle = LE_DATA_EmptyItem;
    LPBYTE              lpBitmapBits;

    #if CE_ARTLIB_EnableDebugMode
        if (object_handle == LED_EI)
        {
            static BOOL first_time = TRUE;

            wsprintf(LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_Show24: Display straight to the former background (LE_DATA_EmptyItem/LED_EI) is no longer supported!");

            if (first_time) LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
            else            LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);

            first_time = FALSE;
        }
    #endif

    // Get or make sure FUL into memory
    FUL_fh_ptr = (LPBITMAPFILEHEADER)LE_DATA_Use(FUL_tag);
    if (FUL_fh_ptr == NULL)
    {
        #if CE_ARTLIB_EnableDebugMode
            wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
            LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
            wsprintf(LE_ERROR_DebugMessageBuffer,"LE_GRAFIX_Show24(): Failed to load FUL. FUL_fh_ptr = NULL.\r\n");
            LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
            wsprintf(LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n\r\n");
            LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
        #endif
    }
    else
    {
        // Get info from logo
        FUL_info_ptr            = (LPBITMAPINFOHEADER) (((LPBYTE) FUL_fh_ptr) + sizeof(BITMAPFILEHEADER));
        FUL_colordata_ptr       = (LPBYTE)(((LPBYTE)FUL_fh_ptr)+FUL_fh_ptr->bfOffBits);

        src_w                   = (short)FUL_info_ptr->biWidth;
        src_h                   = (short)FUL_info_ptr->biHeight;

        src_rect.left           = 0;
        src_rect.top            = 0;
        src_rect.right          = src_w;
        src_rect.bottom         = src_h;

        final_ptr               = LE_GRAFIX_LockObjectDataPtr(object_handle);
        final_width             = LE_GRAFIX_ReturnObjectWidth(object_handle);
        final_height            = LE_GRAFIX_ReturnObjectHeight(object_handle);

        final_rect.left         = dest_x;
        final_rect.top          = dest_y;
        final_rect.right        = final_rect.left + final_width;
        final_rect.bottom       = final_rect.top  + final_height;

        // calculate bitmap bits size
        if (LE_GRAFIX_ScreenBitsPerPixel == 16) malloc_size = (((long) src_w * 2) + 3) & 0xfffffffc;
        else if (LE_GRAFIX_ScreenBitsPerPixel == 24) malloc_size = (((long) src_w * 3) + 3) & 0xfffffffc;
        else if (LE_GRAFIX_ScreenBitsPerPixel == 32) malloc_size =   (long) src_w * 4;
        malloc_size *= src_h;

        // If transp then we have to allocate a buffer to perform transparancy with/on
        if (transp)
        {
            transp_alloc_handle     = LE_DATA_AllocMemoryDataID (malloc_size, LE_DATA_DataNative);
            if (transp_alloc_handle == LED_EI)
            {
                #if CE_ARTLIB_EnableDebugMode
                    wsprintf(LE_ERROR_DebugMessageBuffer,"LE_GRAFIX_Show24(): Failed to alloc space for transparent buffer.\r\n");
                    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
                #endif
            }
            else
            {
                transp_alloc_ptr   = (LPBYTE)LE_DATA_Use (transp_alloc_handle);
                if (transp_alloc_ptr == NULL)
                {
                    #if CE_ARTLIB_EnableDebugMode
                        wsprintf(LE_ERROR_DebugMessageBuffer,"LE_GRAFIX_Show24(): Transp_alloc_ptr equals NULL.\r\n");
                        LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
                    #endif
                }
            }
        };

        if (LE_GRAFIX_ScreenBitsPerPixel == 16)
        {
            lpBitmapBits = FUL_colordata_ptr + (FUL_info_ptr->biClrUsed * sizeof(LONG));

            if (transp)
            {
                if (transp_alloc_ptr != NULL)
                {
                    LI_BLT16ConvertRGBValuesTo16BitColor(transp_alloc_ptr, lpBitmapBits, src_w, src_h, src_w, src_h);
                    LI_BLT16ColorKeyPureGreenBitBlt16to16(final_ptr, final_width, dest_x, dest_y, transp_alloc_ptr, src_w, &src_rect);
                }
            }
            else
            {
                final_ptr += ((dest_y*(((final_width*2)+3)& 0xfffffffc))+(dest_x*2));
                LI_BLT16ConvertRGBValuesTo16BitColor
                (
                    final_ptr,
                    lpBitmapBits,
                    src_w,
                    src_h,
                    final_width,
                    final_height
                );
            }
        }
        else if (LE_GRAFIX_ScreenBitsPerPixel == 24)
        {
            lpBitmapBits = FUL_colordata_ptr + (FUL_info_ptr->biClrUsed * sizeof(LONG));

            if (transp)
            {
                if (transp_alloc_ptr != NULL)
                {
        // flip 24 bit DIB
                    LI_BLT24ConvertRGBValuesTo24BitColor(transp_alloc_ptr, lpBitmapBits, src_w, src_h, src_w, src_h);
                    LI_BLT24ColorKeyPureGreenBitBlt24to24(final_ptr, final_width, dest_x, dest_y, transp_alloc_ptr, src_w, &src_rect);
                }
            }
            else
            {
                final_ptr += ((dest_y*(((final_width*3)+3)& 0xfffffffc))+(dest_x*3));
                LI_BLT24ConvertRGBValuesTo24BitColor(final_ptr, lpBitmapBits, src_w, src_h, final_width, final_height);
            }
        }
        else if (LE_GRAFIX_ScreenBitsPerPixel == 32)
        {
            lpBitmapBits = FUL_colordata_ptr + (FUL_info_ptr->biClrUsed * sizeof(LONG));

            if (transp)
            {
                if (transp_alloc_ptr != NULL)
                {
                    LI_BLT32ConvertRGBValuesTo32BitColor(transp_alloc_ptr, lpBitmapBits, src_w, src_h, src_w, src_h);
                    LI_BLT32ColorKeyPureGreenBitBlt32to32(final_ptr, final_width, dest_x, dest_y, transp_alloc_ptr, src_w, &src_rect);
                }
            }
            else
            {
                final_ptr += ((dest_y*final_width*4)+(dest_x*4));
                LI_BLT32ConvertRGBValuesTo32BitColor(final_ptr, lpBitmapBits, src_w, src_h, final_width, final_height);
            }
        }

        LE_GRAFIX_UnlockObjectDataPtr(object_handle);
        if (transp_alloc_handle != LED_EI) LE_DATA_FreeRuntimeDataID (transp_alloc_handle);
    }
}
/************************************************************/

/************************************************************/
/* LE_GRAFIX_ShowObject                                     */
/************************************************************/
void LE_GRAFIX_ShowObject (LE_DATA_DataId dest_object_handle,
short dest_x, short dest_y, LE_DATA_DataId src_object_handle)
{
  LPBYTE   dest_ptr;
  WORD     dest_width;
  WORD     dest_height;
  RECT     src_rect;
  short    src_w;
  short    src_h;
  int      nAlpha;
  DWORD    notransp;
  DWORD    bAlphaChannel;
  LPBYTE   lpBitmapBits;
  LPNEWBITMAPHEADER ObjectPntr;

    #if CE_ARTLIB_EnableDebugMode
        if (dest_object_handle == LED_EI)
        {
            static BOOL first_time = TRUE;

            wsprintf(LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_ShowObject: Display straight to the former background (LE_DATA_EmptyItem/LED_EI) is no longer supported!");

            if (first_time) LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
            else            LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);

            first_time = FALSE;
        }
    #endif

  // Get a pointer to the object (a native bit depth bitmap).
  ObjectPntr = (LPNEWBITMAPHEADER) LE_DATA_Use (src_object_handle);
  if (ObjectPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    wsprintf (LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf (LE_ERROR_DebugMessageBuffer,"LE_GRAFIX_ShowObject(): "
      "Failed to load native bitmap for ID $%08X.\r\n", src_object_handle);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
    wsprintf (LE_ERROR_DebugMessageBuffer,"-----------------------------\r\n\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
#endif
  }
  else // Sucessfully loaded the native bit depth image data.
  {
    // Get info from logo
    lpBitmapBits  = (LPBYTE) &ObjectPntr->dwColorTable;
    nAlpha        = ObjectPntr->nAlpha;
    src_w         = ObjectPntr->nXBitmapWidth;
    src_h         = ObjectPntr->nYBitmapHeight;
    notransp      = ObjectPntr->dwFlags & BITMAP_NOTRANSPARENCY;
    bAlphaChannel = (ObjectPntr->nAlpha != ALPHA_OPAQUE100_0);

    src_rect.left   = 0;
    src_rect.top    = 0;
    src_rect.right  = src_w;
    src_rect.bottom = src_h;

    dest_ptr    = LE_GRAFIX_LockObjectDataPtr(dest_object_handle);
    dest_width  = LE_GRAFIX_ReturnObjectWidth(dest_object_handle);
    dest_height = LE_GRAFIX_ReturnObjectHeight(dest_object_handle);

    // Clip the source rectangle so that it includes only the
    // stuff that fits in the destination.  Fortunately we aren't
    // doing scaling here.

    if (dest_x + src_rect.right > dest_width)
      src_rect.right = dest_width - dest_x;
    if (dest_y + src_rect.bottom > dest_height)
      src_rect.bottom = dest_height - dest_y;
    if (dest_x < 0)
    {
      src_rect.left -= dest_x;
      dest_x = 0;
    }
    if (dest_y < 0)
    {
      src_rect.top -= dest_y;
      dest_y = 0;
    }

    if (src_rect.top < src_rect.bottom && src_rect.left < src_rect.right)
    {
      // There are some pixels still in the source rectangle, copy them.

      if (LE_GRAFIX_ScreenBitsPerPixel == 16)
      {
        if (notransp)           LI_BLT16BitBlt16to16      (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect);
        else if (bAlphaChannel) LI_BLT16AlphaBitBlt16to16 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, nAlpha);
        else        LI_BLT16ColorKeyPureGreenBitBlt16to16 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect);
      }
      else if (LE_GRAFIX_ScreenBitsPerPixel == 24)
      {
        if (notransp)           LI_BLT24BitBlt24to24      (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect);
        else if (bAlphaChannel) LI_BLT24AlphaBitBlt24to24 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, nAlpha);
        else        LI_BLT24ColorKeyPureGreenBitBlt24to24 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect);
      }
      else if (LE_GRAFIX_ScreenBitsPerPixel == 32)
      {
        if (notransp)           LI_BLT32BitBlt32to32      (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect);
        else if (bAlphaChannel) LI_BLT32AlphaBitBlt32to32 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect, nAlpha);
        else        LI_BLT32ColorKeyPureGreenBitBlt32to32 (dest_ptr, dest_width, dest_x, dest_y, lpBitmapBits, src_w, &src_rect);
      }
    }
    LE_GRAFIX_UnlockObjectDataPtr(dest_object_handle);
  }
}



/*****************************************************************************
 * Converts a POINT from logical (virtual screen - what the game programmer
 * sees) to display (real screen, what the hardware sees) coordinates.  See
 * the description of LE_GRAFIX_ScreenSourceRect and
 * LE_GRAFIX_ScreenDestinationRect for more details.
 */

void LE_GRAFIX_PointLogicalToDisplay (LPPOINT lpPoint)
{
  lpPoint->x = (((lpPoint->x - LE_GRAFIX_ScreenSourceRect.left) *
    (LE_GRAFIX_ScreenDestinationRect.right - LE_GRAFIX_ScreenDestinationRect.left)) /
    (LE_GRAFIX_ScreenSourceRect.right - LE_GRAFIX_ScreenSourceRect.left)) +
    LE_GRAFIX_ScreenDestinationRect.left;

  lpPoint->y = (((lpPoint->y - LE_GRAFIX_ScreenSourceRect.top) *
    (LE_GRAFIX_ScreenDestinationRect.bottom - LE_GRAFIX_ScreenDestinationRect.top)) /
    (LE_GRAFIX_ScreenSourceRect.bottom - LE_GRAFIX_ScreenSourceRect.top)) +
    LE_GRAFIX_ScreenDestinationRect.top;
}



/*****************************************************************************
 * Converts a RECT from logical to display coordinates.
 */

void LE_GRAFIX_RectLogicalToDisplay (LPRECT lpRectangle)
{
  POINT ptPoint;

  ptPoint.x = lpRectangle->left;
  ptPoint.y = lpRectangle->top;
  LE_GRAFIX_PointLogicalToDisplay(&ptPoint);
  lpRectangle->left = ptPoint.x;
  lpRectangle->top = ptPoint.y;

  ptPoint.x = lpRectangle->right;
  ptPoint.y = lpRectangle->bottom;
  LE_GRAFIX_PointLogicalToDisplay(&ptPoint);
  lpRectangle->right = ptPoint.x;
  lpRectangle->bottom = ptPoint.y;
}



/*****************************************************************************
 * Converts a POINT from display to logical coordinates.
 */

void LE_GRAFIX_PointDisplayToLogical (LPPOINT lpPoint)
{
  int DestHeight;
  int DestWidth;

  DestWidth = LE_GRAFIX_ScreenDestinationRect.right - LE_GRAFIX_ScreenDestinationRect.left;
  DestHeight = LE_GRAFIX_ScreenDestinationRect.bottom - LE_GRAFIX_ScreenDestinationRect.top;

  if (DestWidth <= 0 || DestHeight <= 0)
  {
    lpPoint->x = 0;
    lpPoint->y = 0;
  }
  else
  {
    lpPoint->x = (((lpPoint->x - LE_GRAFIX_ScreenDestinationRect.left) *
      (LE_GRAFIX_ScreenSourceRect.right - LE_GRAFIX_ScreenSourceRect.left)) /
      DestWidth) + LE_GRAFIX_ScreenSourceRect.left;

    lpPoint->y = (((lpPoint->y - LE_GRAFIX_ScreenDestinationRect.top) *
      (LE_GRAFIX_ScreenSourceRect.bottom - LE_GRAFIX_ScreenSourceRect.top)) /
      DestHeight) + LE_GRAFIX_ScreenSourceRect.top;
  }
}



/*****************************************************************************
 * Converts a RECT from display to logical coordinates.
 */

void LE_GRAFIX_RectDisplayToLogical (LPRECT lpRectangle)
{
  POINT ptPoint;

  ptPoint.x = lpRectangle->left;
  ptPoint.y = lpRectangle->top;
  LE_GRAFIX_PointDisplayToLogical(&ptPoint);
  lpRectangle->left = ptPoint.x;
  lpRectangle->top = ptPoint.y;

  ptPoint.x = lpRectangle->right;
  ptPoint.y = lpRectangle->bottom;
  LE_GRAFIX_PointDisplayToLogical(&ptPoint);
  lpRectangle->right = ptPoint.x;
  lpRectangle->bottom = ptPoint.y;
}



/*******************************************************************************
*
*   Name:         LE_GRAFIX_RecolorUAP
*
*   Description:  Recolors a UAP substituting on color for another
*
*   Arguments:    hUAPData - handle to UAP data
*                 lprRecolorClipRect - pointer to RECT area to recolor
*                 crFromColor - color to substitute for
*                 crToColor - new color to substitute
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
BOOL LE_GRAFIX_RecolorUAP(
  LE_DATA_DataId hUAPData,
//  LPRECT lprRecolorClipRect,
  COLORREF crFromColor,
  COLORREF crToColor)
{
  BYTE cFromColorIndex;
  LPNEWBITMAPHEADER lpBitmapHeader;
  DWORD dwNativeColor;
  int nColorIndex;
  BOOL bFoundColor;

  switch (LE_GRAFIX_ScreenBitsPerPixel)
  {
    case 16:
    case 24:
    case 32:
      lpBitmapHeader = (LPNEWBITMAPHEADER) LE_DATA_Use(hUAPData);
      switch (LE_GRAFIX_ScreenBitsPerPixel)
      {
        case 16:
          dwNativeColor = LI_BLT_ConvertColorRefTo16BitColor(crFromColor);
          break;
        case 24:
          dwNativeColor = LI_BLT_ConvertColorRefTo24BitColor(crFromColor);
          break;
        case 32:
          dwNativeColor = LI_BLT_ConvertColorRefTo32BitColor(crFromColor);
          break;
        default:
          dwNativeColor = 0;
          break;
      }
      bFoundColor = FALSE;
      for (nColorIndex = 0; !bFoundColor && (nColorIndex < lpBitmapHeader->nColors); nColorIndex ++)
      {
        if (dwNativeColor == lpBitmapHeader->dwColorTable[nColorIndex * 2])
        {
          bFoundColor = TRUE;
          cFromColorIndex = nColorIndex;
          switch (LE_GRAFIX_ScreenBitsPerPixel)
          {
            case 16:
              dwNativeColor = LI_BLT_ConvertColorRefTo16BitColor(crToColor);
              break;
            case 24:
              dwNativeColor = LI_BLT_ConvertColorRefTo24BitColor(crToColor);
              break;
            case 32:
              dwNativeColor = LI_BLT_ConvertColorRefTo32BitColor(crToColor);
              break;
          }
          lpBitmapHeader->dwColorTable[nColorIndex * 2] = dwNativeColor;
          return(TRUE);
        }
      }
      break;
  }
  return(FALSE);
}


/*******************************************************************************
*
*   Name:         LE_GRAFIX_RecolorObject
*
*   Description:  Recolors a native bitmap substituting on color for another
*
*   Arguments:    hNativeBitmapData - handle to native bitmap data
*                 lprRecolorClipRect - pointer to RECT area to recolor
*                 crFromColor - color to substitute for
*                 crToColor - new color to substitute
*
*   Globals:      none
*
*   Returns:      none
*
*******************************************************************************/
BOOL LE_GRAFIX_RecolorObject(
  LE_DATA_DataId hNativeBitmapData,
//  LPRECT lprRecolorClipRect,
  COLORREF crFromColor,
  COLORREF crToColor)
{
  LPNEWBITMAPHEADER lpBitmapHeader;
  int nXBitmapWidthInBytes;
  int nXColumnIndex;
  int nYRowIndex;
  BYTE *BitmapBitsPntr;

  lpBitmapHeader = (LPNEWBITMAPHEADER) LE_DATA_Use(hNativeBitmapData);
  nXBitmapWidthInBytes = ((lpBitmapHeader->nXBitmapWidth * (LE_GRAFIX_ScreenBitsPerPixel / 8)) + 3) & 0xfffffffc;
  BitmapBitsPntr = (LPBYTE) (lpBitmapHeader->dwColorTable + (2 * lpBitmapHeader->nColors));

  switch (LE_GRAFIX_ScreenBitsPerPixel)
  {
    case 16:
    {
      WORD wNativeFromColor;
      WORD wNativeToColor;

      wNativeFromColor = LI_BLT_ConvertColorRefTo16BitColor(crFromColor);
      wNativeToColor = LI_BLT_ConvertColorRefTo16BitColor(crToColor);
      for (nYRowIndex = 0; nYRowIndex < lpBitmapHeader->nYBitmapHeight; nYRowIndex ++)
      {
        LPWORD lpPixelPointer;

        lpPixelPointer = (LPWORD)(BitmapBitsPntr + (nYRowIndex * nXBitmapWidthInBytes));
        for (nXColumnIndex = 0; nXColumnIndex < lpBitmapHeader->nXBitmapWidth; nXColumnIndex ++)
        {
          if (wNativeFromColor == *lpPixelPointer)
            *lpPixelPointer = wNativeToColor;
          lpPixelPointer ++;
        }
      }
      break;
    }
    case 24:
    {
      DWORD dwNativeFromColor;
      DWORD dwNativeToColor;

      dwNativeFromColor = LI_BLT_ConvertColorRefTo24BitColor(crFromColor);
      dwNativeToColor = LI_BLT_ConvertColorRefTo24BitColor(crToColor);
      for (nYRowIndex = 0; nYRowIndex < lpBitmapHeader->nYBitmapHeight; nYRowIndex ++)
      {
        LPBYTE lpPixelPointer;
        DWORD dwPixelValue;

        lpPixelPointer = BitmapBitsPntr + (nYRowIndex * nXBitmapWidthInBytes);
        for (nXColumnIndex = 0; nXColumnIndex < lpBitmapHeader->nXBitmapWidth; nXColumnIndex ++)
        {
          dwPixelValue = *(DWORD *)lpPixelPointer;
          dwPixelValue &= 0xffffff;
          if (dwNativeFromColor == dwPixelValue)
          { //AX: Fixed this to properly change colors in 24 bits
            *(LPWORD)lpPixelPointer = (WORD)dwNativeToColor;
            lpPixelPointer += 2;
            *lpPixelPointer = (BYTE)(dwNativeToColor >> 16)
              ;
            lpPixelPointer ++;
          }
          else
            lpPixelPointer += 3;
        }
      }
      break;
    }
    case 32:
    {
      DWORD dwNativeFromColor;
      DWORD dwNativeToColor;

      dwNativeFromColor = LI_BLT_ConvertColorRefTo32BitColor(crFromColor);
      dwNativeToColor = LI_BLT_ConvertColorRefTo32BitColor(crToColor);
      for (nYRowIndex = 0; nYRowIndex < lpBitmapHeader->nYBitmapHeight; nYRowIndex ++)
      {
        LPDWORD lpPixelPointer;

        lpPixelPointer = (LPDWORD) (BitmapBitsPntr + (nYRowIndex * nXBitmapWidthInBytes));
        for (nXColumnIndex = 0; nXColumnIndex < lpBitmapHeader->nXBitmapWidth; nXColumnIndex ++)
        {
          if (dwNativeFromColor == *lpPixelPointer)
            *lpPixelPointer = dwNativeToColor;
          lpPixelPointer ++;
        }
      }
      break;
    }
  }
  return(TRUE);
}


#if 0 // Disabled code.
/*******************************************************************************
*
*   Name:         LE_GRAFIX_FadeToBlack
*
*   Description:  If MMX is available, smoothly fades the native bitmap
*         provided to black. If not available, flips the screen
*         to black. This assumes that all animations etc.. have
*         stopped so it can write to the bits. It is assumed that
*         the bitmaps are not transparent (ie: no bright green
*         pixels that are assumed to be transparent)
*
*   Arguments:    hNativeBitmapData - handle to native bitmap data
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LE_GRAFIX_FadeToBlack( void )
{
  int i;
  LPBYTE sourceAndDestination;
  int widthOfBitmap;
  int heightOfBitmap;
  RECT myRect;
  widthOfBitmap = LE_GRAFIX_ReturnObjectWidth(LED_EI);
  heightOfBitmap = LE_GRAFIX_ReturnObjectHeight(LED_EI);
  myRect.top = 0;
  myRect.left = 0;
  myRect.bottom = heightOfBitmap;
  myRect.right = widthOfBitmap;

  if ( LE_MMX_Is_Available() && ( LE_GRAFIX_ScreenBitsPerPixel != 8 ))
  {
    //Ranges of alpha opacity from l_anim.h (ALPHA_OPAQUE constants)
    //From full opaque to fully transparent.
    for ( i = 0xFF ; i > 0x0F ; i-=0x1F )
    {
      sourceAndDestination = LE_GRAFIX_LockObjectDataPtr(LED_EI);

      switch ( LE_GRAFIX_ScreenBitsPerPixel )
      {
      case 16:
        LI_MMXFade16(sourceAndDestination, widthOfBitmap,
          sourceAndDestination, widthOfBitmap, &myRect, ALPHA_OPAQUE81_25);
        break;
      case 24:
        LI_MMXFade24(sourceAndDestination, widthOfBitmap,
          sourceAndDestination, widthOfBitmap, &myRect, ALPHA_OPAQUE81_25);
        break;
      case 32:
        LI_MMXFade32(sourceAndDestination, widthOfBitmap,
          sourceAndDestination, widthOfBitmap, &myRect, ALPHA_OPAQUE81_25);
        break;
      }

      LE_GRAFIX_UnlockObjectDataPtr(LED_EI);
    }
  }
  else
  {
    //Go to black straight away..
    LE_GRAFIX_ColorArea(LED_EI,
      (short)0, (short)0, (short) widthOfBitmap, (short) heightOfBitmap,
      LE_GRAFIX_MakeColorRef(0, 0, 0));
  }

}

/*******************************************************************************
*
*   Name:         LE_GRAFIX_FadeFromBlack
*
*   Description:  If MMX is available, smoothly fades the native bitmap
*         provided from black. If not available, flips the screen
*         to the provided data. This assumes that all animations
*         etc.. have stopped so it can write to the bits
*
*   Arguments:    hNativeBitmapData - handle to native bitmap data
*
* Globals:    none
*
*   Returns:    none
*
*******************************************************************************/
void LE_GRAFIX_FadeFromBlack(LE_DATA_DataId hNativeBitmapData)
{
  int i;
//    LPBYTE source;
  LPBYTE destination;
  int widthOfBitmap;
  int heightOfBitmap;
//    int bitsPerPixel;
  //if (LE_MMX_Is_Available())
  if (0==1)
  {

    widthOfBitmap = LE_GRAFIX_ReturnObjectWidth(LED_EI);
    heightOfBitmap = LE_GRAFIX_ReturnObjectHeight(LED_EI);
    //Ranges of alpha opacity from l_anim.h (ALPHA_OPAQUE constants)
    //From fully transparent to fully opaque.
    for (i=0xFF;i>0x0F;--i)
    {
      destination = LE_GRAFIX_LockObjectDataPtr(LED_EI);
      switch (LE_GRAFIX_ScreenBitsPerPixel)
      {
      case 16:
        /*
        LI_MMXFade16(
          destinationBits,
          widthOfBitmap,
          destination,
          widthOfSourceInPixels,
          LPRECT lprSourceBltRect,
          i)*/
        break;
      case 24:
        //LI_MMXFade24(sourceAndDest, sourceAndDest, widthOfBitmap,
        //  heightOfBitmap, i);
        break;
      case 32:
        //LI_MMXFade32(sourceAndDest, sourceAndDest, widthOfBitmap,
        //  heightOfBitmap, i);
        break;
      default:
        break;
      }
      LE_GRAFIX_UnlockObjectDataPtr(LED_EI);
    }
  }
  else
  {
    //Go to color straight away...
  }
}
#endif 0 // Disabled code.



/*******************************************************************************
*
*   Name:         LE_GRAFIXBulkRecolorTAB
*
*   Description:  Recolors a TAB (8 bit image with palette), marks
*                 bitmaps to avoid recoloring any bitmaps twice
*
*   Arguments:    nBitmapID - data file ID of the bitmap.
*                 lpColorMap - pointer to an array of COLORMAP structures
*                              describing recoloring (from/to colour pairs).
*                 nColorsToRemap - number of entries in lpColorMap
*
*   Globals:    none
*
*   Returns:    TRUE if sucessful
*
*******************************************************************************/

BOOL LE_GRAFIXBulkRecolorTAB (LE_DATA_DataId nBitmapID,
LPCOLORMAP lpColorMap, int nColorsToRemap)
{
  LPNEWBITMAPHEADER lpBitmap;
  BYTE              cRed;
  BYTE              cGreen;
  BYTE              cBlue;
  BYTE              cAlpha;
  BOOL              bIsFillColor;
  int               nColorMapIndex;
  int               nColorTableIndex;
  DWORD             dwNativeColor;
  BOOL              bFoundColor;

  if (nBitmapID == LE_DATA_EmptyItem)
    return FALSE; // Need a valid DataID!

  // get pointer to bitmap data.
  lpBitmap = (LPNEWBITMAPHEADER) LE_DATA_Use(nBitmapID);
  if (lpBitmap == NULL)
    return FALSE; // No data available!

  // if frame bitmap has not been recolored
  if ((lpBitmap->dwFlags & BITMAP_NOTRECOLORED) == 0)
    return TRUE; // Bitmap has already been recoloured.

  // unmark bitmap
  lpBitmap->dwFlags &= ~BITMAP_NOTRECOLORED;

  // process all colors in color map
  for (nColorMapIndex = 0; nColorMapIndex < nColorsToRemap; nColorMapIndex ++)
  {
    // get RGB values of from color
    cRed = GetRValue(lpColorMap[nColorMapIndex].from);
    cGreen = GetGValue(lpColorMap[nColorMapIndex].from);
    cBlue = GetBValue(lpColorMap[nColorMapIndex].from);

    // Fill color?  Primary pure colours mark areas to be filled.
    bIsFillColor = ((cRed == 0x00) || (cRed == 0xff)) &&
      ((cGreen == 0x00) || (cGreen == 0xff)) &&
      ((cBlue == 0x00) || (cBlue == 0xff));

    // if fill color, skip over alpha values in color table
    if (!bIsFillColor)
    {
      // if outline color, check outline color for matches
      // with alpha values in color table
      for (nColorTableIndex = 0; nColorTableIndex < lpBitmap->nAlpha; nColorTableIndex ++)
      {
        // convert "from" color to post alpha-multiplied RGB, using
        // the alpha from this entry in the TAB's palette.
        cAlpha = (BYTE)lpBitmap->dwColorTable[(nColorTableIndex * 2) + 1];
        cRed = GetRValue(lpColorMap[nColorMapIndex].from);
        cGreen = GetGValue(lpColorMap[nColorMapIndex].from);
        cBlue = GetBValue(lpColorMap[nColorMapIndex].from);
        cRed = (BYTE)(((int)cAlpha * (int)cRed) / 255);
        cGreen = (BYTE)(((int)cAlpha * (int)cGreen) / 255);
        cBlue = (BYTE)(((int)cAlpha * (int)cBlue) / 255);

        // convert post multiplied "from" color to native
        switch(LE_GRAFIX_ScreenBitsPerPixel)
        {
        case 16:
          dwNativeColor = LI_BLT_ConvertColorRefTo16BitColor(RGB(cRed,cGreen,cBlue));
          break;
        case 24:
          dwNativeColor = LI_BLT_ConvertColorRefTo24BitColor(RGB(cRed,cGreen,cBlue));
          break;
        case 32:
          dwNativeColor = LI_BLT_ConvertColorRefTo32BitColor(RGB(cRed,cGreen,cBlue));
          break;
        default:
          dwNativeColor = 0;
          break;
        }

        // compare to color table entry
        if (dwNativeColor == lpBitmap->dwColorTable[nColorTableIndex * 2])
        {
          // if match replace entry to to color
          // convert to color to post multiplied RGB
          cRed = GetRValue(lpColorMap[nColorMapIndex].to);
          cGreen = GetGValue(lpColorMap[nColorMapIndex].to);
          cBlue = GetBValue(lpColorMap[nColorMapIndex].to);
          cRed = (BYTE)(((int)cAlpha * (int)cRed) / 255);
          cGreen = (BYTE)(((int)cAlpha * (int)cGreen) / 255);
          cBlue = (BYTE)(((int)cAlpha * (int)cBlue) / 255);
          // convert post multiplied to color to native
          switch(LE_GRAFIX_ScreenBitsPerPixel)
          {
          case 16:
            dwNativeColor = LI_BLT_ConvertColorRefTo16BitColor(RGB(cRed,cGreen,cBlue));
            break;
          case 24:
            dwNativeColor = LI_BLT_ConvertColorRefTo24BitColor(RGB(cRed,cGreen,cBlue));
            break;
          case 32:
            dwNativeColor = LI_BLT_ConvertColorRefTo32BitColor(RGB(cRed,cGreen,cBlue));
            break;
          }
          // replace from color with to color in color table
          lpBitmap->dwColorTable[nColorTableIndex * 2] = dwNativeColor;
        }
      }
    }

    // process non translucent portion of color table
    bFoundColor = FALSE;
    // convert from color to native
    switch(LE_GRAFIX_ScreenBitsPerPixel)
    {
    case 16:
      dwNativeColor = LI_BLT_ConvertColorRefTo16BitColor(lpColorMap[nColorMapIndex].from);
      break;
    case 24:
      dwNativeColor = LI_BLT_ConvertColorRefTo24BitColor(lpColorMap[nColorMapIndex].from);
      break;
    case 32:
      dwNativeColor = LI_BLT_ConvertColorRefTo32BitColor(lpColorMap[nColorMapIndex].from);
      break;
    }

    // compare from color to all colors in color table
    for (nColorTableIndex = lpBitmap->nAlpha;
    !bFoundColor && (nColorTableIndex < lpBitmap->nColors);
    nColorTableIndex ++)
    {
      // if match, replace color with to color
      if (dwNativeColor == lpBitmap->dwColorTable[nColorTableIndex * 2])
      {
        // short circuit
        bFoundColor = TRUE;
        // convert to color to native
        switch(LE_GRAFIX_ScreenBitsPerPixel)
        {
        case 16:
          dwNativeColor = LI_BLT_ConvertColorRefTo16BitColor(lpColorMap[nColorMapIndex].to);
          break;
        case 24:
          dwNativeColor = LI_BLT_ConvertColorRefTo24BitColor(lpColorMap[nColorMapIndex].to);
          break;
        case 32:
          dwNativeColor = LI_BLT_ConvertColorRefTo32BitColor(lpColorMap[nColorMapIndex].to);
          break;
        }
        // replace from color with to color
        lpBitmap->dwColorTable[nColorTableIndex * 2] = dwNativeColor;
      }
    }
  }
  return TRUE;
}



/*****************************************************************************
 * This function creates a generic bitmap runtime data item using the
 * given DirectDraw surface.  It adds a reference to the surface while
 * the GBM exists.  Returns LE_DATA_EmptyItem if it fails.
 */

LE_DATA_DataId LE_GRAFIX_CreateGBMFromDDSurface (PDDSURFACE DDSurfacePntr)
{
  DDSURFDESC                      DDSurfaceDesc;
  DWORD                           DesiredAttributeFlags;
  DWORD                           NewRefCount;
  LE_DATA_DataId                  GBMID = LE_DATA_EmptyItem;
  LE_GRAFIX_GenericBitmapPointer  GBMPntr = NULL;
  UNS32                           LikelyPitch;
  PD3DTEXTURE                     TexturePntr;

  if (DDSurfacePntr == NULL)
    goto ErrorExit;

  GBMID = LE_DATA_AllocMemoryDataID (sizeof (LE_GRAFIX_GenericBitmapRecord),
    LE_DATA_DataGenericBitmap);
  if (GBMID == LE_DATA_EmptyItem)
    goto ErrorExit;

  GBMPntr = (LE_GRAFIX_GenericBitmapPointer) LE_DATA_Use (GBMID);
  if (GBMPntr == NULL)
    goto ErrorExit;

  // Need to zero memory now that data system doesn't do it.
  memset (GBMPntr, 0, sizeof (LE_GRAFIX_GenericBitmapRecord));

  NewRefCount = DDSurfacePntr->AddRef ();
  GBMPntr->directDrawSurfacePntr = DDSurfacePntr;

  // Find out about the surface - is it textured, a render surface,
  // native depth etc...

  memset (&DDSurfaceDesc, 0, sizeof (DDSurfaceDesc));
  DDSurfaceDesc.dwSize = sizeof (DDSurfaceDesc);
  DesiredAttributeFlags =
    (DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT);
  DDSurfaceDesc.dwFlags = DesiredAttributeFlags;

  if (DD_OK != DDSurfacePntr->GetSurfaceDesc (&DDSurfaceDesc))
    goto ErrorExit; // Something went wrong.

  if ((DDSurfaceDesc.dwFlags & DesiredAttributeFlags) != DesiredAttributeFlags)
    goto ErrorExit; // Missing needed info about the surface.

  if ((DDSurfaceDesc.ddpfPixelFormat.dwFlags & DDPF_RGB) == 0)
    goto ErrorExit; // We only understand RGB colour schemes.

  if (DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_3DDEVICE)
    GBMPntr->flags |= LE_GRAFIX_GBM_FLAGS_RENDERSURFACE;

  if (DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_TEXTURE)
  {
    GBMPntr->flags |= LE_GRAFIX_GBM_FLAGS_TEXTURE;

    if (FAILED (DDSurfacePntr->QueryInterface (
    CURRENT_D3DTEXTURE_GUID, (LPVOID *) &TexturePntr)))
      goto ErrorExit; // Doesn't support our desired texture type.

    // Note that we go through TexturePntr here so that type checking
    // matches our hard coded IID with the texture type version.

    GBMPntr->directDrawTexturePntr = TexturePntr;
  }
  // else just an ordinary bitmap, not a texture or rendering surface.

  GBMPntr->width = (UNS16) DDSurfaceDesc.dwWidth;
  GBMPntr->height = (UNS16) DDSurfaceDesc.dwHeight;
  GBMPntr->bitsPerPixel = (UNS8) DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
  GBMPntr->globalAlphaLevel = 255;

  // Pitch is usually set when the surface is locked, but if they
  // have it we'll store it.  Note that DirectDraw 5 has bugs with
  // calculating the pitch for 24 and 32 bit images, at some sizes,
  // such as with width 156, claiming an extra 4 bytes in the pitch.

  if (DDSurfaceDesc.dwFlags & DDSD_PITCH)
  {
    LikelyPitch = DDSurfaceDesc.dwWidth *
      (DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8);
    LikelyPitch = ((LikelyPitch + 3) & 0xFFFFFFFC); // Make into multiple of 4.
    if ((UNS32) DDSurfaceDesc.lPitch == LikelyPitch + 4)
      GBMPntr->bytesToNextScanLine = LikelyPitch; // Likely the bug happened.
    else
      GBMPntr->bytesToNextScanLine = DDSurfaceDesc.lPitch;
  }

  if (DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount == (DWORD) LE_GRAFIX_ScreenBitsPerPixel &&
  DDSurfaceDesc.ddpfPixelFormat.dwRBitMask == LE_BLT_dwRedMask &&
  DDSurfaceDesc.ddpfPixelFormat.dwGBitMask == LE_BLT_dwGreenMask &&
  DDSurfaceDesc.ddpfPixelFormat.dwBBitMask == LE_BLT_dwBlueMask)
    GBMPntr->flags |= LE_GRAFIX_GBM_FLAGS_NATIVEPIXELS;

  // Success!
  goto NormalExit;

ErrorExit: // Print a warning message and undo allocations and return failure.

#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_CreateGBMFromDDSurface: "
    "Unable to create the generic bitmap for some reason (use the debugger "
    "to find out why; there are just too many reasons).\r\n");
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

  if (GBMPntr != NULL)
  {
    if (GBMPntr->directDrawTexturePntr != NULL)
      GBMPntr->directDrawTexturePntr->Release ();


    if (GBMPntr->directDrawSurfacePntr != NULL)
      GBMPntr->directDrawSurfacePntr->Release ();
  }

  if (GBMID != LE_DATA_EmptyItem)
  {
    LE_DATA_FreeRuntimeDataID (GBMID);
    GBMID = LE_DATA_EmptyItem;
  }

NormalExit:
  return GBMID;
}



/*****************************************************************************
 * Fills in a new generic bitmap record and creates the associated surface
 * from the given BMP (8 or 24 bit) or TAB (8 bit with alpha) data.
 * Returns TRUE if successful.
 */

BOOL LI_GRAFIX_CreateGBMFromImageBuffer (BYTE *BufferPntr,
UNS32 BufferSize, BOOL CreateTexture,
LE_GRAFIX_GenericBitmapPointer GBMPntr)
{
  DWORD                           AlphaDWord;
#if DIRECTDRAW_VERSION >= 0x0600
  BOOL                            AlphaInPalette = FALSE;
#endif
  BOOL                            BMPFormat;
  BITMAPFILEHEADER               *BMPFileHeaderPntr;
  BITMAPINFO                     *BMPInfoPntr;
  DWORD                           ColourDWord;
  PDDPALETTE                      DDPalette = NULL;
  PDDSURFACE                      DDSurfacePntr = NULL;
  DWORD                          *DWordPntr;
  HRESULT                         ErrorCode;
  BOOL                            FlipVertically;
  int                             i;
  UNS32                           LikelyPitch;
  UNS8                            OriginalBitsPerPixel;
  BYTE                           *OriginalBitsPntr;
  UNS32                           OriginalBytesPerScanLine;
  PALETTEENTRY                    PaletteEntry;
  UNS16                           PaletteNumberOfColours;
  PALETTEENTRY                   *PalettePntr;
  BOOL                            ReturnCode = FALSE;
  BYTE                           *SurfaceBitsPntr;
  UNS32                           SurfaceBytesPerScanLine;
  DDSURFDESC                      SurfaceDescription;
  BOOL                            SurfaceLocked = FALSE;
  PALETTEENTRY                    TempPalette [256];
  PD3DTEXTURE                     TexturePntr;
  NEWBITMAPHEADER                *UAPPntr;

  if (BufferPntr == NULL || BufferSize < sizeof (BITMAPFILEHEADER) ||
  GBMPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_CreateGBMFromImageBuffer: "
      "Bad input parameters.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Not enough data to determine anything.
  }

  if (LE_GRAFIX_DirectDrawObjectN == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_CreateGBMFromImageBuffer: "
      "DirectDraw isn't open.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // DirectDraw isn't operational.
  }

  memset (GBMPntr, 0, sizeof (LE_GRAFIX_GenericBitmapRecord));

  BMPFileHeaderPntr = (BITMAPFILEHEADER *) BufferPntr;
  BMPFormat = (BMPFileHeaderPntr->bfType == 'MB');

  if (BMPFormat)
  {
    // Double check that it is a BMP, and not a TAB with a width == 'BM'.

    if (BMPFileHeaderPntr->bfReserved1 != 0 ||
    BMPFileHeaderPntr->bfReserved2 != 0 ||
    BMPFileHeaderPntr->bfSize > BufferSize ||
    BMPFileHeaderPntr->bfOffBits > BufferSize)
      BMPFormat = FALSE;
  }

  if (BMPFormat)
  {
    // More double checking for BMP format.

    BMPInfoPntr = (BITMAPINFO *) (BufferPntr + sizeof (BITMAPFILEHEADER));
    if (BMPInfoPntr->bmiHeader.biSize < sizeof (BITMAPINFOHEADER))
      BMPFormat = FALSE;
  }

  if (BMPFormat)
  {
    // Is this a BMP format we can handle?

    if (BMPInfoPntr->bmiHeader.biCompression != BI_RGB)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_CreateGBMFromImageBuffer: "
        "We don't handle compressed BMPs, we have one with biCompression = %d.\r\n",
        (int) BMPInfoPntr->bmiHeader.biCompression);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }
  }

  if (BMPFormat)
  {
    GBMPntr->width = (UNS16) BMPInfoPntr->bmiHeader.biWidth;
    GBMPntr->height = abs (BMPInfoPntr->bmiHeader.biHeight);
    OriginalBitsPerPixel = (UNS8) BMPInfoPntr->bmiHeader.biBitCount;
    GBMPntr->originalDataBitmapStartOffset = (UNS16) BMPFileHeaderPntr->bfOffBits;
    FlipVertically = (BMPInfoPntr->bmiHeader.biHeight >= 0);
    if (OriginalBitsPerPixel <= 8)
      GBMPntr->flags |= LE_GRAFIX_GBM_FLAGS_ORIGINALLY_BMP_8;
    else
      GBMPntr->flags |= LE_GRAFIX_GBM_FLAGS_ORIGINALLY_BMP_24;
  }
  else // UAP format
  {
    UAPPntr = (LPNEWBITMAPHEADER) BufferPntr;
    GBMPntr->width = UAPPntr->nXBitmapWidth;
    GBMPntr->height = UAPPntr->nYBitmapHeight;
    OriginalBitsPerPixel = 8;
    GBMPntr->originalDataBitmapStartOffset =
      sizeof (NEWBITMAPHEADER) + 8 * UAPPntr->nColors;
    FlipVertically = FALSE;
    GBMPntr->flags |= LE_GRAFIX_GBM_FLAGS_ORIGINALLY_UAP;
  }

  // Set up description of new surface.

  memset (&SurfaceDescription, 0, sizeof (SurfaceDescription));
  SurfaceDescription.dwSize = sizeof (SurfaceDescription);
  SurfaceDescription.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  SurfaceDescription.dwWidth = GBMPntr->width;
  SurfaceDescription.dwHeight = GBMPntr->height;
  SurfaceDescription.ddpfPixelFormat.dwSize = sizeof (SurfaceDescription.ddpfPixelFormat);
  SurfaceDescription.ddpfPixelFormat.dwFlags = DDPF_RGB;
  if (CreateTexture)
    SurfaceDescription.ddsCaps.dwCaps =
    DDSCAPS_TEXTURE | LE_GRAFIX_VideoMemoryForGBMTextures;
  else // Just a picture, not used as a texture.
    SurfaceDescription.ddsCaps.dwCaps =
    DDSCAPS_OFFSCREENPLAIN | LE_GRAFIX_VideoMemoryForGBMPictures;

  if (OriginalBitsPerPixel <= 8)
  {
    GBMPntr->bitsPerPixel = 8;
    SurfaceDescription.ddpfPixelFormat.dwRGBBitCount = 8;
    SurfaceDescription.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;
    // Note - we don't add the DDSCAPS_PALETTE flag since that doesn't
    // seem to work (create surface fails), guess it only works for
    // real hardware screens with a palette.  But we can still make
    // our own palette and attach it to the surface.

    if (BMPFormat)
    {
      // Convert the BMP's palette into PALETTEENTRY records.
      // It will always be non-transparent (no alpha or colourkey).

      PaletteNumberOfColours = (UNS16) BMPInfoPntr->bmiHeader.biClrUsed;
      DWordPntr = (DWORD *) (BMPInfoPntr->bmiColors + 0);
      PalettePntr = TempPalette + 0;
      for (i = PaletteNumberOfColours; i > 0; i--)
      {
        ColourDWord = *DWordPntr++;
        PaletteEntry.peBlue = (UNS8) ColourDWord;
        PaletteEntry.peGreen = (UNS8) (ColourDWord >> 8);
        PaletteEntry.peRed = (UNS8) (ColourDWord >> 16);
        PaletteEntry.peFlags = 0; // No alpha involved, special codes instead.
        *PalettePntr++ = PaletteEntry;
      }
    }
    else // A UAP format.
    {
      // Copy the palette into a standard format.  Set alpha later.

      PaletteNumberOfColours = UAPPntr->nColors;
      DWordPntr = UAPPntr->dwColorTable + 0;
      PalettePntr = TempPalette + 0;
      for (i = PaletteNumberOfColours; i > 0; i--)
      {
        ColourDWord = *DWordPntr;
        DWordPntr += 2; // Each entry is 8 bytes = 2 DWORDS.
        PaletteEntry.peBlue = (UNS8) ColourDWord;
        PaletteEntry.peGreen = (UNS8) (ColourDWord >> 8);
        PaletteEntry.peRed = (UNS8) (ColourDWord >> 16);
        PaletteEntry.peFlags = 0;
        *PalettePntr++ = PaletteEntry;
      }

      if (UAPPntr->dwFlags & BITMAP_NOTRANSPARENCY)
      {
        // Nothing more to do for solid colour mode.
      }
      else // Transparency (colour zero transparent) and maybe translucency.
      {
        // We will be using alpha values in the palette, also UAPs
        // have premultiplied colours (colour toned down by multiplying
        // with the alpha level).  Note that we can't use the
        // DDPF_ALPHAPIXELS flag since that only works for true colour
        // pixels with alpha data in them.

#if DIRECTDRAW_VERSION >= 0x0600
        SurfaceDescription.ddpfPixelFormat.dwFlags |= DDPF_ALPHAPREMULT;
        AlphaInPalette = TRUE;
#endif
        // Colour 0 is always fully transparent.

        TempPalette[0].peFlags = 0;

        // Set the alpha values in the palette, starting at Palette[1].
        // Under DirectX 5 these alpha values are ignored (only used
        // for hardware palette options in the screen surface), but we
        // can retrieve them and use them for our own purposes.

        DWordPntr = UAPPntr->dwColorTable + 3;
        PalettePntr = TempPalette + 1;
        for (i = 1; i < PaletteNumberOfColours; i++)
        {
          AlphaDWord = *DWordPntr;
          DWordPntr += 2;
          if (i >= UAPPntr->nAlpha)
            (PalettePntr++)->peFlags = 255; // Non-alpha colour, is solid.
          else // Has an alpha value.
            (PalettePntr++)->peFlags = (UNS8) AlphaDWord;
        }

#if DIRECTDRAW_VERSION < 0x0600
        // For DirectX 5.0 and earlier, fake transparency by
        // making colour zero transparent using the colour key
        // facility.  The rest of the translucent colours just
        // will come out solid.  I think it compares actual
        // pixel values here, not RGB.  Need to test it...

        SurfaceDescription.dwFlags |= DDSD_CKSRCBLT;
        SurfaceDescription.ddckCKSrcBlt.dwColorSpaceLowValue = 0;
        SurfaceDescription.ddckCKSrcBlt.dwColorSpaceHighValue = 0;
#endif
      }
    }

    // OK, the palette table is ready to use for creating a
    // DirectDraw palette.

    ErrorCode = LE_GRAFIX_DirectDrawObjectN->CreatePalette (
#if DIRECTDRAW_VERSION >= 0x0600
      (AlphaInPalette ? DDPCAPS_ALPHA : 0) |
#endif
      DDPCAPS_8BIT | DDPCAPS_ALLOW256 | DDPCAPS_INITIALIZE,
      TempPalette, &DDPalette, NULL);

    if (ErrorCode != DD_OK)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_CreateGBMFromImageBuffer: "
        "Unable to create DirectDraw palette, error code 0x%08X.\r\n",
        (unsigned int) ErrorCode);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }
  }
  else // True colour image will be converted to native screen depth.  No alpha.
  {
    GBMPntr->bitsPerPixel = LE_GRAFIX_ScreenBitsPerPixel;
    SurfaceDescription.ddpfPixelFormat.dwRGBBitCount = LE_GRAFIX_ScreenBitsPerPixel;
    SurfaceDescription.ddpfPixelFormat.dwRBitMask = LE_BLT_dwRedMask;
    SurfaceDescription.ddpfPixelFormat.dwGBitMask = LE_BLT_dwGreenMask;
    SurfaceDescription.ddpfPixelFormat.dwBBitMask = LE_BLT_dwBlueMask;
  }

  // Try to create the surface.

  ErrorCode = LE_GRAFIX_DirectDrawObjectN->CreateSurface (
    &SurfaceDescription, &DDSurfacePntr, NULL);
  if (ErrorCode != DD_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_CreateGBMFromImageBuffer: "
      "Unable to create DirectDraw surface, error code 0x%08X.\r\n",
      (unsigned int) ErrorCode);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  GBMPntr->directDrawSurfacePntr = DDSurfacePntr;
  DDSurfacePntr = NULL; // Has new owners now.

  // Associate the palette and surface.  This adds a reference to the palette,
  // so it sticks around after this function's exit code releases it.

  if (DDPalette != NULL)
  {
    ErrorCode = GBMPntr->directDrawSurfacePntr->SetPalette (DDPalette);
    if (ErrorCode != DD_OK)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_CreateGBMFromImageBuffer: "
        "DirectDraw SetPalette failed with error code 0x%08X.\r\n",
        (unsigned int) ErrorCode);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }
  }

  // Get the texture reference if this is a texture surface.

  if (CreateTexture)
  {
    ErrorCode = GBMPntr->directDrawSurfacePntr->QueryInterface (
      CURRENT_D3DTEXTURE_GUID, (LPVOID *) &TexturePntr);
    if (ErrorCode != DD_OK)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_CreateGBMFromImageBuffer: "
        "Unable to get texture interface from the surface, "
        "error code 0x%08X.\r\n",
        (unsigned int) ErrorCode);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Copy the texture pointer, this way a compiler error
    // will happen if interface versions don't match.

    GBMPntr->directDrawTexturePntr = TexturePntr;
  }

  // Prepare to copy the bitmap.  First lock the whole surface so
  // that we can write to it.

  ErrorCode = GBMPntr->directDrawSurfacePntr->Restore ();
  if (ErrorCode != DD_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_CreateGBMFromImageBuffer: "
      "Unable to restore our newly created DirectDraw surface, error 0x%08X.\r\n",
      (unsigned int) ErrorCode);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  SurfaceDescription.dwSize = sizeof (SurfaceDescription);
  SurfaceDescription.dwFlags = 0;

  ErrorCode = GBMPntr->directDrawSurfacePntr->Lock (NULL /* Whole surface */,
    &SurfaceDescription,
    DDLOCK_NOSYSLOCK /* Don't halt Windows - usually no conflict with GDI access */ |
    DDLOCK_WRITEONLY /* Hint to DirectDraw about what we are doing */ |
    DDLOCK_WAIT /* Wait for Blits in progress to finish */ |
    DDLOCK_SURFACEMEMORYPTR /* We want a pointer to memory */,
    NULL);

  if (ErrorCode != DD_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_CreateGBMFromImageBuffer: "
      "Unable to lock the surface, error 0x%08X.\r\n", (unsigned int) ErrorCode);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  SurfaceLocked = TRUE;
  SurfaceBitsPntr = (BYTE *) SurfaceDescription.lpSurface;

  // Compensate for a DirectDraw bug with calculating pitch.

  LikelyPitch = SurfaceDescription.dwWidth *
      (SurfaceDescription.ddpfPixelFormat.dwRGBBitCount / 8);
  LikelyPitch = ((LikelyPitch + 3) & 0xFFFFFFFC); // Make into multiple of 4.
  if ((UNS32) SurfaceDescription.lPitch == LikelyPitch + 4)
    SurfaceBytesPerScanLine = LikelyPitch; // Likely the bug happened.
  else
    SurfaceBytesPerScanLine = SurfaceDescription.lPitch;

  OriginalBytesPerScanLine = GBMPntr->width * (OriginalBitsPerPixel / 8);
  OriginalBytesPerScanLine = ((OriginalBytesPerScanLine + 3) & 0xFFFFFFFC); // Make it a multiple of 4.
  OriginalBitsPntr = BufferPntr + GBMPntr->originalDataBitmapStartOffset;

  // Now convert pixel formats (depths) and flip upside down (if needed)
  // while copying the original data to the surface.

  if (!LE_BLT_CopySolidUnityRaw (OriginalBitsPntr, OriginalBytesPerScanLine,
  OriginalBitsPerPixel, TRUE /* Colours in BMP's BGR format */,
  GBMPntr->width, GBMPntr->height,
  NULL /* No palette needed, even in 8 bit mode */, 4 /* Palette entry size */,
  SurfaceBitsPntr, SurfaceBytesPerScanLine,
  GBMPntr->bitsPerPixel,
  GBMPntr->width, GBMPntr->height,
  NULL /* No palette */, 4 /* Palette entry size */,
  0 /* CopyToX */, 0 /* CopyToY */,
  UNS16_MAX /* Copy width */, UNS16_MAX /* Copy height */,
  FlipVertically))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_CreateGBMFromImageBuffer: "
      "Unable to copy the bitmap, converting from %d to %d bits per pixel.\r\n",
      (int) OriginalBitsPerPixel, (int) GBMPntr->bitsPerPixel);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  GBMPntr->directDrawSurfacePntr->Unlock (NULL /* SurfaceBitsPntr */);
  SurfaceLocked = FALSE;

#if 0 // See what kind of surface we got.
  {
    RECT SourceRect;
    char  TempString [1024];

    GBMPntr->directDrawSurfacePntr->GetSurfaceDesc (&SurfaceDescription);

    sprintf (TempString, "SurfaceFlags %08X =", (int) SurfaceDescription.dwFlags);
    if (SurfaceDescription.dwFlags & DDSD_CAPS) strcat (TempString, " DDSD_CAPS");
    if (SurfaceDescription.dwFlags & DDSD_HEIGHT) strcat (TempString, " DDSD_HEIGHT");
    if (SurfaceDescription.dwFlags & DDSD_WIDTH) strcat (TempString, " DDSD_WIDTH");
    if (SurfaceDescription.dwFlags & DDSD_PITCH) strcat (TempString, " DDSD_PITCH");
    if (SurfaceDescription.dwFlags & DDSD_BACKBUFFERCOUNT) strcat (TempString, " DDSD_BACKBUFFERCOUNT");
    if (SurfaceDescription.dwFlags & DDSD_ZBUFFERBITDEPTH) strcat (TempString, " DDSD_ZBUFFERBITDEPTH");
    if (SurfaceDescription.dwFlags & DDSD_ALPHABITDEPTH) strcat (TempString, " DDSD_ALPHABITDEPTH");
    if (SurfaceDescription.dwFlags & DDSD_LPSURFACE) strcat (TempString, " DDSD_LPSURFACE");
    if (SurfaceDescription.dwFlags & DDSD_PIXELFORMAT) strcat (TempString, " DDSD_PIXELFORMAT");
    if (SurfaceDescription.dwFlags & DDSD_CKDESTOVERLAY) strcat (TempString, " DDSD_CKDESTOVERLAY");
    if (SurfaceDescription.dwFlags & DDSD_CKDESTBLT) strcat (TempString, " DDSD_CKDESTBLT");
    if (SurfaceDescription.dwFlags & DDSD_CKSRCOVERLAY) strcat (TempString, " DDSD_CKSRCOVERLAY");
    if (SurfaceDescription.dwFlags & DDSD_CKSRCBLT) strcat (TempString, " DDSD_CKSRCBLT");
    if (SurfaceDescription.dwFlags & DDSD_MIPMAPCOUNT) strcat (TempString, " DDSD_MIPMAPCOUNT");
    if (SurfaceDescription.dwFlags & DDSD_REFRESHRATE) strcat (TempString, " DDSD_REFRESHRATE");
    if (SurfaceDescription.dwFlags & DDSD_LINEARSIZE) strcat (TempString, " DDSD_LINEARSIZE");

    sprintf (TempString, "PixelFlags %08X =", (int) SurfaceDescription.ddpfPixelFormat.dwFlags);
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) strcat (TempString, " DDPF_ALPHAPIXELS");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_ALPHA) strcat (TempString, " DDPF_ALPHA");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_FOURCC) strcat (TempString, " DDPF_FOURCC");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4) strcat (TempString, " DDPF_PALETTEINDEXED4");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXEDTO8) strcat (TempString, " DDPF_PALETTEINDEXEDTO8");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) strcat (TempString, " DDPF_PALETTEINDEXED8");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_RGB) strcat (TempString, " DDPF_RGB");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_COMPRESSED) strcat (TempString, " DDPF_COMPRESSED");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_RGBTOYUV) strcat (TempString, " DDPF_RGBTOYUV");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_YUV) strcat (TempString, " DDPF_YUV");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_ZBUFFER) strcat (TempString, " DDPF_ZBUFFER");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED1) strcat (TempString, " DDPF_PALETTEINDEXED1");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED2) strcat (TempString, " DDPF_PALETTEINDEXED2");
    if (SurfaceDescription.ddpfPixelFormat.dwFlags & DDPF_ZPIXELS) strcat (TempString, " DDPF_ZPIXELS");

    sprintf (TempString, "ddsCaps %08X =", (int) SurfaceDescription.ddsCaps.dwCaps);
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_RESERVED1) strcat (TempString, " DDSCAPS_RESERVED1");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_ALPHA) strcat (TempString, " DDSCAPS_ALPHA");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER) strcat (TempString, " DDSCAPS_BACKBUFFER");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_COMPLEX) strcat (TempString, " DDSCAPS_COMPLEX");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_FLIP) strcat (TempString, " DDSCAPS_FLIP");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER) strcat (TempString, " DDSCAPS_FRONTBUFFER");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN) strcat (TempString, " DDSCAPS_OFFSCREENPLAIN");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_OVERLAY) strcat (TempString, " DDSCAPS_OVERLAY");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_PALETTE) strcat (TempString, " DDSCAPS_PALETTE");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE) strcat (TempString, " DDSCAPS_PRIMARYSURFACE");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACELEFT) strcat (TempString, " DDSCAPS_PRIMARYSURFACELEFT");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) strcat (TempString, " DDSCAPS_SYSTEMMEMORY");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_TEXTURE) strcat (TempString, " DDSCAPS_TEXTURE");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_3DDEVICE) strcat (TempString, " DDSCAPS_3DDEVICE");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) strcat (TempString, " DDSCAPS_VIDEOMEMORY");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_VISIBLE) strcat (TempString, " DDSCAPS_VISIBLE");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_WRITEONLY) strcat (TempString, " DDSCAPS_WRITEONLY");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_ZBUFFER) strcat (TempString, " DDSCAPS_ZBUFFER");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_OWNDC) strcat (TempString, " DDSCAPS_OWNDC");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_LIVEVIDEO) strcat (TempString, " DDSCAPS_LIVEVIDEO");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_HWCODEC) strcat (TempString, " DDSCAPS_HWCODEC");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_MODEX) strcat (TempString, " DDSCAPS_MODEX");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_MIPMAP) strcat (TempString, " DDSCAPS_MIPMAP");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_RESERVED2) strcat (TempString, " DDSCAPS_RESERVED2");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_ALLOCONLOAD) strcat (TempString, " DDSCAPS_ALLOCONLOAD");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) strcat (TempString, " DDSCAPS_VIDEOPORT");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM) strcat (TempString, " DDSCAPS_LOCALVIDMEM");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) strcat (TempString, " DDSCAPS_NONLOCALVIDMEM");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_STANDARDVGAMODE) strcat (TempString, " DDSCAPS_STANDARDVGAMODE");
    if (SurfaceDescription.ddsCaps.dwCaps & DDSCAPS_OPTIMIZED) strcat (TempString, " DDSCAPS_OPTIMIZED");
    strcpy (TempString, "Done.");

    // Now copy it to the screen, beep if it fails.

    SourceRect.left = 0;
    SourceRect.top = 0;
    SourceRect.right = GBMPntr->width;
    if (SourceRect.right > LE_GRAFIX_VirtualScreenWidthInPixels)
      SourceRect.right = LE_GRAFIX_VirtualScreenWidthInPixels;
    SourceRect.bottom = GBMPntr->height;
    if (SourceRect.bottom > LE_GRAFIX_VirtualScreenHeightInPixels)
      SourceRect.bottom = LE_GRAFIX_VirtualScreenHeightInPixels;
#if CE_ARTLIB_EnableMultitasking
    if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return FALSE; // System is closed.
    EnterCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
    if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
#endif
    ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->BltFast (0 /* X */, 0 /* Y */,
      GBMPntr->directDrawSurfacePntr, &SourceRect, DDBLTFAST_NOCOLORKEY);

    if (ErrorCode != DD_OK)
      MessageBeep (-1);
#if CE_ARTLIB_EnableMultitasking
    LeaveCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
#endif
  }
#endif // See what kind of surface we got.

  ReturnCode = TRUE;
  goto NormalExit;


ErrorExit:
  if (SurfaceLocked) // Have to unlock before destroying.
    GBMPntr->directDrawSurfacePntr->Unlock (NULL /* SurfaceBitsPntr */);

  LI_GRAFIX_DestroyGBM (GBMPntr);

NormalExit:
  if (DDPalette != NULL)
    DDPalette->Release ();

  return ReturnCode;
}



/*****************************************************************************
 * Deallocate the DirectDraw surfaces for a generic bitmap, used by the
 * DataID unloading code.  User code should just free the item.
 */

BOOL LI_GRAFIX_DestroyGBM (LE_GRAFIX_GenericBitmapPointer GBMPntr)
{
  if (GBMPntr == NULL)
    return TRUE; // Nothing to do.

  if (GBMPntr->directDrawTexturePntr != NULL)
  {
    GBMPntr->directDrawTexturePntr->Release ();
    GBMPntr->directDrawTexturePntr = NULL;
  }


  if (GBMPntr->directDrawSurfacePntr != NULL)
  {
    GBMPntr->directDrawSurfacePntr->Release ();
    GBMPntr->directDrawSurfacePntr = NULL;
  }

  return TRUE;
}



/*****************************************************************************
 * First restores the DirectDraw surface used by the generic bitmap.  Then
 * optionally loads the generic bitmap's surface pixels from the given
 * data ID's underlying data file (not the in-memory buffer), possibly
 * directly into the surface memory if the bitmap format matches.  Doesn't
 * load the palette or anything else.  Currently handles original data in
 * BMP and UAP format.  Returns TRUE if successful.
 */

static BOOL LI_GRAFIX_ReloadGBMBitmapData (
LE_GRAFIX_GenericBitmapPointer GBMPntr, LE_DATA_DataId GBM_ID)
{
  UNS32                           AmountActuallyLoaded;
  UNS32                           AmountToLoad;
  BOOL                            DataFormatSameAsSurfaceFormat = FALSE;
  HRESULT                         ErrorCode;
  BOOL                            FlipVertically = FALSE;
  UNS32                           LikelyPitch;
  UNS8                            OriginalBitsPerPixel = 0;
  BYTE                           *OriginalBitsPntr = NULL;
  UNS32                           OriginalBytesPerScanLine;
  BOOL                            ReturnCode = FALSE;
  BYTE                           *SurfaceBitsPntr;
  UNS32                           SurfaceBytesPerScanLine;
  DDSURFDESC                      SurfaceDescription;
  BOOL                            SurfaceLocked = FALSE;

  if (GBMPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_ReloadGBMBitmapData: "
      "NULL generic bitmap pointer specified.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  if (GBMPntr->directDrawSurfacePntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    char TempString [2048];
    LE_DATA_DescribeDataID (GBM_ID, TempString, sizeof (TempString));
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_ReloadGBMBitmapData: "
      "Uninitialised generic bitmap, no surface assigned to it, for %s.\r\n",
      TempString);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // First restore the surface if needed.  This doesn't need a valid
  // DataID for reading data, it just reallocates the surface.

  ErrorCode = GBMPntr->directDrawSurfacePntr->IsLost ();

  if (ErrorCode == DDERR_SURFACELOST)
  {
    ErrorCode = GBMPntr->directDrawSurfacePntr->Restore ();
    if (ErrorCode != DD_OK)
    {
#if CE_ARTLIB_EnableDebugMode
      char TempString [2048];
      LE_DATA_DescribeDataID (GBM_ID, TempString, sizeof (TempString));
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_ReloadGBMBitmapData: "
        "Unable to restore the surface, error 0x%08X for %s.\r\n",
        (unsigned int) ErrorCode, TempString);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
    }
  }
  else if (ErrorCode != DD_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    char TempString [2048];
    LE_DATA_DescribeDataID (GBM_ID, TempString, sizeof (TempString));
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_ReloadGBMBitmapData: "
      "Something is wrong with the surface, can't find out if it is lost.  "
      "From %s.\r\n", TempString);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // If this bitmap doesn't have a backup data item
  // to reload the data from, then we are all done.

  if (GBM_ID == LE_DATA_EmptyItem)
  {
    ReturnCode = TRUE;
    goto NormalExit;
  }

  // See what format the backup data is in, and if there was any data.

  if (GBMPntr->flags & LE_GRAFIX_GBM_FLAGS_ORIGINALLY_BMP_24)
  {
    OriginalBitsPerPixel = 24;
    FlipVertically = TRUE;

    // Is it the right kind of 24 bit mode?  We want the blue
    // to be in the low byte of the pixel, which is what the
    // BMP format uses.  Some video boards get it backwards,
    // yes we look at the global setting since we assume that
    // 24 bit BMPs get converted into native screen mode when
    // loaded and that's the same as the screen.

    // Never mind, it always needs to be flipped so can't load directly.
    // if (GBMPntr->bitsPerPixel == 24 && LE_BLT_dwBlueMask == 255)
    //  DataFormatSameAsSurfaceFormat = TRUE;
  }
  else if (GBMPntr->flags & LE_GRAFIX_GBM_FLAGS_ORIGINALLY_BMP_8)
  {
    OriginalBitsPerPixel = 8;
    FlipVertically = TRUE;

    // Oops, BMPs are always upside down.
    // if (GBMPntr->bitsPerPixel == 8)
    //   DataFormatSameAsSurfaceFormat = TRUE;
  }
  else if (GBMPntr->flags & LE_GRAFIX_GBM_FLAGS_ORIGINALLY_UAP)
  {
    OriginalBitsPerPixel = 8;

    if (GBMPntr->bitsPerPixel == 8)
      DataFormatSameAsSurfaceFormat = TRUE;
  }
  else // Not originally from a known format, a runtime memory object?
  {
    ReturnCode = TRUE; // Nothing more to do.
    goto NormalExit;
  }

  // Now lock the whole surface so that we can write to it.

  SurfaceDescription.dwSize = sizeof (SurfaceDescription);
  SurfaceDescription.dwFlags = 0;

  ErrorCode = GBMPntr->directDrawSurfacePntr->Lock (NULL /* Whole surface */,
    &SurfaceDescription,
    DDLOCK_NOSYSLOCK /* Don't halt Windows - usually no conflict with GDI access */ |
    DDLOCK_WRITEONLY /* Hint to DirectDraw about what we are doing */ |
    DDLOCK_WAIT /* Wait for Blits in progress to finish */ |
    DDLOCK_SURFACEMEMORYPTR /* We want a pointer to memory */,
    NULL);

  if (ErrorCode != DD_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    char TempString [2048];
    LE_DATA_DescribeDataID (GBM_ID, TempString, sizeof (TempString));
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_ReloadGBMBitmapData: "
      "Unable to lock the surface, error 0x%08X.  "
      "From %s.\r\n", (unsigned int) ErrorCode, TempString);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  SurfaceLocked = TRUE;
  SurfaceBitsPntr = (BYTE *) SurfaceDescription.lpSurface;

  // Compensate for a DirectDraw bug with calculating pitch.

  LikelyPitch = SurfaceDescription.dwWidth *
      (SurfaceDescription.ddpfPixelFormat.dwRGBBitCount / 8);
  LikelyPitch = ((LikelyPitch + 3) & 0xFFFFFFFC); // Make into multiple of 4.
  if ((UNS32) SurfaceDescription.lPitch == LikelyPitch + 4)
    SurfaceBytesPerScanLine = LikelyPitch; // Likely the bug happened.
  else
    SurfaceBytesPerScanLine = SurfaceDescription.lPitch;

  // See if the number of bytes per scan line in the original image data
  // is the same as in our surface.  If not, then we can't copy them
  // directly.  Assume original image is same size as our generic bitmap.

  OriginalBytesPerScanLine = GBMPntr->width * (OriginalBitsPerPixel / 8);
  OriginalBytesPerScanLine = ((OriginalBytesPerScanLine + 3) & 0xFFFFFFFC); // Make it a multiple of 4.

  if (OriginalBytesPerScanLine != SurfaceBytesPerScanLine)
    DataFormatSameAsSurfaceFormat = FALSE;

  // OK, if the original data format is the same as the surface format,
  // we can just bulk load it directly into the surface memory.  Zoom!

  if (DataFormatSameAsSurfaceFormat)
  {
    AmountToLoad = GBMPntr->height * OriginalBytesPerScanLine;

    AmountActuallyLoaded = LE_DATA_LoadRawIntoBuffer (GBM_ID,
      SurfaceBitsPntr, AmountToLoad,
      GBMPntr->originalDataBitmapStartOffset);

    if (AmountActuallyLoaded != AmountToLoad)
    {
#if CE_ARTLIB_EnableDebugMode
      char TempString [2048];
      LE_DATA_DescribeDataID (GBM_ID, TempString, sizeof (TempString));
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_ReloadGBMBitmapData: "
        "Only loaded %u bytes when trying to load %u directly into the "
        "surface memory for %s.\r\n",
        (unsigned int) AmountActuallyLoaded, AmountToLoad, TempString);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }
  }
  else // Have to convert pixel formats.
  {
    // First load the image's pixel data (not header stuff or palettte)
    // into a temporary buffer.

    AmountToLoad = GBMPntr->height * OriginalBytesPerScanLine;

    OriginalBitsPntr = (BYTE *) LI_MEMORY_AllocLibraryBlock (AmountToLoad);

    AmountActuallyLoaded = LE_DATA_LoadRawIntoBuffer (GBM_ID,
      OriginalBitsPntr, AmountToLoad,
      GBMPntr->originalDataBitmapStartOffset);

    if (AmountActuallyLoaded != AmountToLoad)
    {
#if CE_ARTLIB_EnableDebugMode
      char TempString [2048];
      LE_DATA_DescribeDataID (GBM_ID, TempString, sizeof (TempString));
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_ReloadGBMBitmapData: "
        "Only loaded %u bytes when trying to load %u into a temp buffer, "
        "for %s.\r\n", (unsigned int) AmountActuallyLoaded,
        (unsigned int) AmountToLoad, TempString);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Now convert pixel formats (depths) and flip upside down (if needed)
    // while copying the original data to the surface.

    if (!LE_BLT_CopySolidUnityRaw (OriginalBitsPntr, OriginalBytesPerScanLine,
    OriginalBitsPerPixel, TRUE /* Colours in BMP's BGR format */,
    GBMPntr->width, GBMPntr->height,
    NULL /* No palette needed, even in 8 bit mode */, 4 /* Palette entry size */,
    SurfaceBitsPntr, SurfaceBytesPerScanLine,
    GBMPntr->bitsPerPixel,
    GBMPntr->width, GBMPntr->height,
    NULL /* No palette */, 4 /* Palette entry size */,
    0 /* CopyToX */, 0 /* CopyToY */,
    UNS16_MAX /* Copy width */, UNS16_MAX /* Copy height */,
    FlipVertically))
    {
#if CE_ARTLIB_EnableDebugMode
      char TempString [2048];
      LE_DATA_DescribeDataID (GBM_ID, TempString, sizeof (TempString));
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_GRAFIX_ReloadGBMBitmapData: "
        "Unable to copy the bitmap, converting from %d to %d bits per pixel.  "
        "For %s.\r\n", (int) OriginalBitsPerPixel,
        (int) GBMPntr->bitsPerPixel, TempString);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }
  }

  // Successful!
  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
NormalExit:
  if (SurfaceLocked)
    GBMPntr->directDrawSurfacePntr->Unlock(NULL/*SurfaceBitsPntr*/);

  if (OriginalBitsPntr != NULL)
    LI_MEMORY_DeallocLibraryBlock (OriginalBitsPntr);

  return ReturnCode;
}



/*****************************************************************************
 * Restores the DirectDraw surface of a given generic bitmap.  If
 * necessary, reloads the image from the data file.  Returns the
 * Generic Bitmap pointer if successful (and you can then use the
 * DirectDraw surface pointer in it).  Returns NULL on failure.
 */

LE_GRAFIX_GenericBitmapPointer LE_GRAFIX_RestoreGBM (
LE_DATA_DataId GenericBitmapDataID)
{
  HRESULT                         ErrorCode;
  LE_GRAFIX_GenericBitmapPointer  GBMPntr;

#if CE_ARTLIB_EnableDebugMode
  if (LE_DATA_GetCurrentDataType (GenericBitmapDataID) != LE_DATA_DataGenericBitmap)
  {
    char TempString [2048];
    LE_DATA_DescribeDataID (GenericBitmapDataID, TempString, sizeof (TempString));
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_RestoreGBM: "
      "Data item isn't a generic bitmap.  For %s.\r\n", TempString);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  GBMPntr = (LE_GRAFIX_GenericBitmapPointer) LE_DATA_Use (GenericBitmapDataID);
  if (GBMPntr == NULL)
    return NULL; // Bad DataID.

  ErrorCode = GBMPntr->directDrawSurfacePntr->IsLost ();

  if (ErrorCode == DD_OK)
    return GBMPntr; // Surface is still available.

  if (LI_GRAFIX_ReloadGBMBitmapData (GBMPntr, GenericBitmapDataID))
    return GBMPntr; // Restore surface and optionally load original image.

  return NULL; // Loading failed.
}



/*****************************************************************************
 * Locking a generic bitmap forces it to be in memory and sets the pointers
 * to the pixel data.  You need to unlock it after using it.  Also don't
 * lock the screen while debugging - it won't be able to switch to the
 * debugger if you hit a breakpoint, and your computer will die.  Returns
 * a pointer to the generic bitmap if successful, NULL if it fails.
 *
 * When using multitasking, please try to avoid having two different threads
 * locking the same surface, use the LE_GRAFIX_LockedSurfaceCriticalSectionPntr
 * if you have to avoid that problem (the animation engines periodically lock
 * the screen and uses this critical section to do it, so keep your lock time
 * short to avoid animation jerkyness).  Also, locking a surface makes it
 * unBLITable, so if you are just copying surfaces, just use the BLT
 * functions and don't lock at all.
 */

LE_GRAFIX_GenericBitmapPointer LE_GRAFIX_RestoreAndLockGBM (
LE_DATA_DataId GenericBitmapDataID)
{
  HRESULT                         ErrorCode;
  LE_GRAFIX_GenericBitmapPointer  GBMPntr;
  UNS32                           LikelyPitch;
  DDSURFDESC                   SurfaceDescription;

#if CE_ARTLIB_EnableDebugMode
  if (LE_DATA_GetCurrentDataType (GenericBitmapDataID) != LE_DATA_DataGenericBitmap)
  {
    char TempString [2048];
    LE_DATA_DescribeDataID (GenericBitmapDataID, TempString, sizeof (TempString));
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_RestoreAndLockGBM: "
      "Data item isn't a generic bitmap.  For %s.\r\n", TempString);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return NULL;
  }
#endif

  GBMPntr = (LE_GRAFIX_GenericBitmapPointer) LE_DATA_Use (GenericBitmapDataID);
  if (GBMPntr == NULL || GBMPntr->directDrawSurfacePntr == NULL)
    return NULL; // Bad DataID or uninitialised GBM.

  // Check for already locked.

  if (GBMPntr->bitmapBits != NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    char TempString [2048];
    LE_DATA_DescribeDataID (GenericBitmapDataID, TempString, sizeof (TempString));
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_RestoreAndLockGBM: "
      "Unable to lock the bitmap because it is already locked!  "
      "Sorry, we don't do nested locks.  For %s.\r\n", TempString);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL;
  }

  // First restore the surface if it needs it.

  ErrorCode = GBMPntr->directDrawSurfacePntr->IsLost ();

  if (ErrorCode != DD_OK)
  {
    if (!LI_GRAFIX_ReloadGBMBitmapData (GBMPntr, GenericBitmapDataID))
      return NULL; // Failed to restore the surface.
  }

  // Do the locking.

  SurfaceDescription.dwSize = sizeof (SurfaceDescription);
  SurfaceDescription.dwFlags = 0;

  ErrorCode = GBMPntr->directDrawSurfacePntr->Lock (NULL /* Whole surface */,
    &SurfaceDescription,
    DDLOCK_NOSYSLOCK /* Don't halt Windows - usually no conflict with GDI access */ |
    DDLOCK_WRITEONLY /* Hint to DirectDraw about what we are doing */ |
    DDLOCK_WAIT /* Wait for Blits in progress to finish */ |
    DDLOCK_SURFACEMEMORYPTR /* We want a pointer to memory */,
    NULL);

  if (ErrorCode != DD_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    char TempString [2048];
    LE_DATA_DescribeDataID (GenericBitmapDataID, TempString, sizeof (TempString));
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_RestoreAndLockGBM: "
      "Unable to lock the surface, error 0x%08X.  "
      "From %s.\r\n", (unsigned int) ErrorCode, TempString);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL;
  }

  GBMPntr->bitmapBits = (UNS8 *) SurfaceDescription.lpSurface;

  // Compensate for DirectDraw bug with pitch in 24 and 32 bit modes.

  LikelyPitch = SurfaceDescription.dwWidth *
    (SurfaceDescription.ddpfPixelFormat.dwRGBBitCount / 8);
  LikelyPitch = ((LikelyPitch + 3) & 0xFFFFFFFC); // Make into multiple of 4.
  if ((UNS32) SurfaceDescription.lPitch == LikelyPitch + 4)
    GBMPntr->bytesToNextScanLine = LikelyPitch; // Likely the bug happened.
  else
    GBMPntr->bytesToNextScanLine = SurfaceDescription.lPitch;

  return GBMPntr; // Successful!
}



/*****************************************************************************
 * Unlocking it lets the system (DirectDraw too) move things around and dump
 * unused data.  It should only be locked momentarily, while the bitmap is
 * being changed.
 */

BOOL LE_GRAFIX_UnlockGBM (LE_DATA_DataId GenericBitmapDataID)
{
  HRESULT                         ErrorCode;
  LE_GRAFIX_GenericBitmapPointer  GBMPntr;

#if CE_ARTLIB_EnableDebugMode
  if (LE_DATA_GetCurrentDataType (GenericBitmapDataID) != LE_DATA_DataGenericBitmap)
  {
    char TempString [2048];
    LE_DATA_DescribeDataID (GenericBitmapDataID, TempString, sizeof (TempString));
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_UnlockGBM: "
      "Data item isn't a generic bitmap.  For %s.\r\n", TempString);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  GBMPntr = (LE_GRAFIX_GenericBitmapPointer) LE_DATA_Use (GenericBitmapDataID);
  if (GBMPntr == NULL || GBMPntr->directDrawSurfacePntr == NULL)
    return NULL; // Bad DataID or uninitialised GBM.

  // Check for already unlocked.

  if (GBMPntr->bitmapBits == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    char TempString [2048];
    LE_DATA_DescribeDataID (GenericBitmapDataID, TempString, sizeof (TempString));
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_UnlockGBM: "
      "Unable to unlock the bitmap because it is already unlocked!  "
      "Sorry, we don't do nested locks.  For %s.\r\n", TempString);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // Do the unlocking.

  ErrorCode = GBMPntr->directDrawSurfacePntr->Unlock(NULL/*GBMPntr->bitmapBits*/);
  GBMPntr->bitmapBits = NULL;

  if (ErrorCode != DD_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    char TempString [2048];
    LE_DATA_DescribeDataID (GenericBitmapDataID, TempString, sizeof (TempString));
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_UnlockGBM: "
      "Unable to unlock the surface, error 0x%08X.  "
      "From %s.\r\n", (unsigned int) ErrorCode, TempString);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL;
  }

  return TRUE; // Successful!
}



/*****************************************************************************
 * Return the generic bitmap's width.  Return 0 on error.
 */

UNS16 LE_GRAFIX_GetGBMWidth (LE_DATA_DataId GenericBitmapDataID)
{
  LE_GRAFIX_GenericBitmapPointer GBMPntr;

  GBMPntr = (LE_GRAFIX_GenericBitmapPointer)LE_DATA_Use(GenericBitmapDataID);
  if (GBMPntr == NULL)
    return(0);
  return(GBMPntr->width);
}



/*****************************************************************************
 * Return the generic bitmap's height.  Return 0 on error.
 */

UNS16 LE_GRAFIX_GetGBMHeight (LE_DATA_DataId GenericBitmapDataID)
{
  LE_GRAFIX_GenericBitmapPointer GBMPntr;

  GBMPntr = (LE_GRAFIX_GenericBitmapPointer)LE_DATA_Use(GenericBitmapDataID);
  if (GBMPntr == NULL)
    return(0);
  return(GBMPntr->height);
}



/*****************************************************************************
 * This function fills the given rectangle with the colour passed in.  The
 * colour is a 32 bit colorref value (see LE_GRAFIX_MakeColorRef(r,g,b)).
 * If you go over the edge of the bitmap, it will be clipped to fit.
 */

void LE_GRAFIX_FillGBM (LE_DATA_DataId GenericBitmapDataID,
  int X, int Y, int Width, int Height, UNS32 Colour)
{
  LE_GRAFIX_GenericBitmapPointer GBMPntr;
  PDDSURFACE directDrawSurface;
  RECT cliprect;
  HDC hDC;
  DDSURFDESC DDSurfaceDesc;
  UNS32 LikelyPitch;
  UNS16 gbmwidth = LE_GRAFIX_GetGBMWidth(GenericBitmapDataID);
  UNS16 gbmheight = LE_GRAFIX_GetGBMHeight(GenericBitmapDataID);

  // Get the DirectDraw surface
  GBMPntr = (LE_GRAFIX_GenericBitmapPointer)LE_DATA_Use(GenericBitmapDataID);
  if (GBMPntr == NULL)
    return;
  directDrawSurface = GBMPntr->directDrawSurfacePntr;

  // Clip the rectangle
  cliprect.right = X + Width;
  cliprect.bottom = Y + Height;

  if ((cliprect.right < 0) || (cliprect.bottom < 0) ||
      (X > LE_GRAFIX_GetGBMWidth(GenericBitmapDataID)) ||
      (Y > LE_GRAFIX_GetGBMHeight(GenericBitmapDataID)))
    return;

  cliprect.left = X;
  cliprect.top = Y;

  if (cliprect.left < 0)            cliprect.left = 0;
  if (cliprect.right > gbmwidth)    cliprect.right = gbmwidth;
  if (cliprect.top < 0)             cliprect.top = 0;
  if (cliprect.bottom > gbmheight)  cliprect.bottom = gbmheight;

  // Draw the rectangle in the generic bitmap
  LE_GRAFIX_RestoreGBM(GenericBitmapDataID);

  // Find out about the surface...  (Check for DirectDraw bug)
  memset (&DDSurfaceDesc, 0, sizeof (DDSurfaceDesc));
  DDSurfaceDesc.dwSize = sizeof (DDSurfaceDesc);
  DDSurfaceDesc.dwFlags = (DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT);

  if (DD_OK != directDrawSurface->GetSurfaceDesc (&DDSurfaceDesc))
  {
#if CE_ARTLIB_EnableDebugMode
    wsprintf(LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_FillGBM(): "
      "Couldn't get surface description for GBM DataID $%08X.\r\n", GenericBitmapDataID);
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
    return;
  }

  // Look out for DirectDraw bug with calculating the pitch
  // for 24 and 32 bit images, at some sizes claiming an
  // extra 4 bytes in the pitch. (ie: < 215 pixel width)
  LikelyPitch = DDSurfaceDesc.dwWidth *
    (DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8);
  LikelyPitch = ((LikelyPitch + 3) & 0xFFFFFFFC); // Make into multiple of 4.
  if ((UNS32) DDSurfaceDesc.lPitch == LikelyPitch + 4)
  {
    // Likely the bug happened.
#if CE_ARTLIB_EnableDebugMode
    wsprintf(LE_ERROR_DebugMessageBuffer, "LE_GRAFIX_FillGBM(): "
      "The GBM DataID $%08X is subject to a DirectDraw bug.\r\n"
      "The box drawn with parameters X:%d Y:%d Width:%d Height:%d Colour:$%08X probably doesn't look right.\r\n\r\n",
      GenericBitmapDataID, X, Y, Width, Height, Colour);
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
  }

  directDrawSurface->GetDC(&hDC);
  FillRect(hDC, &cliprect, CreateSolidBrush(Colour));
  directDrawSurface->ReleaseDC(hDC);
}

#endif /* CE_ARTLIB_EnableSystemGrafix */

/* end of file */
