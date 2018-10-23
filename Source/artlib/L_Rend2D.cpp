/*****************************************************************************
 * FILE:        L_Rend2D.cpp
 *
 * DESCRIPTION: Draws the bitmaps defined by an animation sequence tree
 *              onto the screen.  Works by keeping an invalid screen area
 *              (Region in Windows, collection of rectangles elsewhere)
 *              and then redrawing the sequence tree for the bitmaps that
 *              intersect the invalid area.  Whenever a bitmap appears,
 *              disappears, or moves, the bounding box of the bitmap
 *              is added to the area.  Its private sequence data holds
 *              the calculated bounding boxes.  All the bitmaps are drawn
 *              through the invalid area to an off-screen bitmap and then
 *              the invalid areas are copied from the off-screen bitmap
 *              to the real screen.
 *
 * Render Cycle and Invalid Areas:
 *
 * The sequencer first moves around the sequences and adds new ones and
 * deletes old ones.  It calls the render slot for each of these changed
 * sequences individually.  The bounding boxes for the old moved position
 * and for deleted sequences are added to the invalid area of the render
 * slot and to the invalid area of related render slots (ones overlapping
 * on the screen - like the mouse pointer one).  New positions and new
 * sequences get marked as needing redraw by the sequencer so we don't
 * add their invalid boxes here (but the box coordinates get calculated).
 *
 * Next, the sequencer calls the render slot function to update all invalid
 * areas.  The renderer updates its camera settings (possibly scrolling)
 * and then goes through all of its sequences that have been marked by the
 * sequencer as needing a redraw and adds their rectangles to its own invalid
 * area (but not to overlapping render slots).  Well, if the camera moved too
 * much then it marks its whole screen area as invalid and doesn't bother
 * examining the sequences.  The resulting invalid area is then added to all
 * related render slots (an order n-squared operation so don't stack up too
 * many render slots).
 *
 * Finally, the sequencer calls the render slot function to draw on the
 * screen (or other output surface).  All things in the slot's invalid
 * areas (which now includes contributions from overlapping slots) get
 * drawn to the working surface.  The topmost render slot in a bunch of
 * overlapping ones copies its cumulative invalid area from the working
 * surface to the output surface, if the output is different from the
 * working surface.  Then invalid area gets cleared and that's it for
 * one cycle.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Rend2D.cpp 47    16/09/99 5:24p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Rend2D.cpp $
 * 
 * 47    16/09/99 5:24p Agmsmith
 * Adding Bink video player.
 * 
 * 46    8/20/99 11:28a Agmsmith
 * Fixing SetViewport and SetCamera commands.
 *
 * 45    8/18/99 8:15p Agmsmith
 * Added SetCamera sequencer command.
 *
 * 44    8/16/99 5:23p Agmsmith
 * Adding viewport sequencer command.
 *
 * 43    7/19/99 16:20 Davew
 * Fixed a problem when locking surfaces and printing error messages.
 * Can't be done during a WinMutex lock.
 *
 * 42    7/12/99 10:39a Agmsmith
 * MouseLatched stuff removed in favour of more accurate UIMsg events.
 *
 * 41    6/14/99 1:52p Agmsmith
 * Handle case of bitmap rotated 180 degrees.
 *
 * 40    6/14/99 12:02p Agmsmith
 * Added accidentally deleted code back in.
 *
 * 39    6/14/99 11:32a Fredds
 * Now includes support for rotation of native bitdepths: 16to16, 24to24
 * and 32to32.
 *
 * 38    6/07/99 4:50p Agmsmith
 * Make it so that invalid areas show bounding boxes for selected
 * sequences too.
 *
 * 37    6/07/99 3:19p Agmsmith
 * Added camera capability.
 *
 * 36    5/26/99 3:48p Agmsmith
 * Updated for new camera chunk fields.
 *
 * 35    5/13/99 10:50a Agmsmith
 * Add clipping to draw only the changed part of screen in rotation calls.
 *
 * 34    5/12/99 4:57p Agmsmith
 * Now can rotate solid bitmaps.
 *
 * 33    4/27/99 1:33p Agmsmith
 * Number of render slots is now user defined.
 *
 * 32    4/22/99 15:00 Davew
 * Updated to DX v6
 *
 * 31    4/12/99 3:42p Agmsmith
 * Use GetLoadedDataType instead of current, unless you know what you are
 * doing.
 *
 * 30    4/12/99 3:05p Agmsmith
 * Need to get bounding rect for a particular sequence for hit location.
 *
 * 29    4/12/99 12:12p Agmsmith
 * Added dragging on the camera plane.
 *
 * 28    3/29/99 4:18p Lzou
 * Have implemented bitmap stretch copy with alpha channel effects for
 * 16to16, 24to24, and 32to32 bit colour depths.
 *
 * 27    3/28/99 5:29p Agmsmith
 * Move copying of working screen to main screen into graphics module, and
 * add clipping to window's visible region so it doesn't trash other
 * things on the screen.
 *
 * 26    3/28/99 12:11p Agmsmith
 * Only set pitch when a sound starts up, volume and panning are
 * calculated when it moves.
 *
 * 25    3/27/99 12:50p Agmsmith
 * Don't update sequenceClock while feeding.
 *
 * 24    3/21/99 3:30p Agmsmith
 * Changes to allow all sequences to have child sequences.
 *
 * 23    3/13/99 2:29p Agmsmith
 * Added iteration over all active sequences and a way of restricting that
 * to ones under a given mouse position.
 *
 * 22    2/18/99 12:54p Agmsmith
 * Speed up rendering loops so they can handle more bitmaps on the screen.
 *
 * 21    2/17/99 5:51p Agmsmith
 * Showing bounding boxes should leave final image on screen.
 *
 * 20    2/17/99 10:46a Agmsmith
 * To avoid frame update glitches, feed the video in the sequencer for
 * videos which don't draw directly to the screen, and only draw the
 * bitmap in the renderer.  Direct to screen still feeds and draws in the
 * renderer.
 *
 * 19    2/14/99 5:12p Agmsmith
 * Draw a box around selected sequences.
 *
 * 18    2/09/99 1:41p Agmsmith
 * Fix drawing to an off-screen bitmap with the new video player.
 *
 * 17    2/02/99 1:46p Agmsmith
 * Video API changes again.
 *
 * 16    1/31/99 5:32p Agmsmith
 * Changes for new improved video API.
 *
 * 15    1/31/99 3:26p Agmsmith
 * Rearranged video player code so that feeding was done during
 * renderering, not in the sequencer, so that it can draw directly on the
 * screen.
 *
 * 14    1/16/99 4:40p Agmsmith
 * Changing blitter routines to also use old routines if desired.
 *
 * 2     9/15/98 12:12p Agmsmith
 * Added Li Zou's blitter calls.
 *
 * 54    9/04/98 3:55p Agmsmith
 * Correct camera scale factor conversion bug.
 *
 * 32    6/12/98 10:44a Agmsmith
 * Deallocate temporary Windows regions when exiting.
 ****************************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemRend2D


/************************************************************/
/* MACRO DEFINITIONS                                        */
/*----------------------------------------------------------*/

#define LI_REND2D_SHOW_BOUNDING_BOXES (0 && CE_ARTLIB_EnableDebugMode)
#define LI_REND2D_SHOW_BOUNDING_BOXES_DELAY 6
  // Set this to 1 if you want to see the bounding boxes being drawn
  // on the screen, and cut it down to the given number of ticks
  // per update (must be an even number since it gets divided by 2).
  // 60 is a good value since the animation engine stops skipping
  // frames when it delays over 1/5 second per frame.



// One instance of this structure is allocated for each active 2D sequence.

typedef struct PrivateSequenceDataStruct
{
  union {
    struct RectangleStructs
    {
      TYPE_Point2D  topLeft;
      TYPE_Point2D  topRight;
      TYPE_Point2D  bottomRight;
      TYPE_Point2D  bottomLeft;
    } named;
    TYPE_Point2D asArray [4];
  } boundingBox;
    // The bounding box for the sequence.  Since it can be rotated,
    // the four corners are stored rather than just a rectangle.
    // The coordinates are in screen coordinates.  As usual, the
    // top and left sides include their pixels and the bottom and
    // right don't (the off by one problem).  The bitmap will be
    // drawn in this area, stretching it if needed.

} PrivateSequenceDataRecord, *PrivateSequenceDataPointer;



// An instance of this class is associated with each active 2D render slot.

typedef class LI_REND2D_SlotDataClass :
  public LI_REND_CommonScreenWriterRenderSlotClass
{
public:
  virtual BOOL SequenceStartUp (LE_SEQNCR_RuntimeInfoPointer SequencePntr);
  virtual void SequenceShutDown (LE_SEQNCR_RuntimeInfoPointer SequencePntr);
  virtual BOOL SequenceMoved (LE_SEQNCR_RuntimeInfoPointer SequencePntr);
  virtual BOOL CanHandleSequenceType (LE_SEQNCR_SequenceType SequenceType, int Dimensionality);
  virtual void UpdateInvalidAreas (LE_SEQNCR_RuntimeInfoPointer RootPntr);
  virtual void DrawInvalidAreas (LE_SEQNCR_RuntimeInfoPointer RootPntr);
  virtual void AddInvalidArea (HRGN InvalidAreaRegion, PDDSURFACE ScreenSurface);
  virtual HRGN GetSolidArea (LE_SEQNCR_RuntimeInfoPointer RootPntr, PDDSURFACE ScreenSurface);
  virtual BOOL SequenceTouchesScreenRectangle (LE_SEQNCR_RuntimeInfoPointer SequencePntr, TYPE_RectPointer TouchingRectanglePntr);
  virtual BOOL GetScreenBoundingRectangle (LE_SEQNCR_RuntimeInfoPointer SequencePntr, TYPE_RectPointer ScreenRectPntr);
  virtual BOOL DragOnCameraPlane (LE_SEQNCR_RuntimeInfoPointer SequencePntr, TYPE_Point2DPointer DragFromVirtualScreenPointPntr, TYPE_Point2DPointer DragToVirtualScreenPointPntr, TYPE_Point3DPointer ResultingDeltaVector);
  virtual UNS8 GetCameraLabelNumber (void);

  virtual PDDSURFACE GetWorkingSurface (void);
  virtual BOOL SetViewport (const TYPE_Rect *ViewportRectPntr);
  virtual BOOL SetCamera (UNS8 CameraNumber, TYPE_Point3DPointer Position, TYPE_Point3DPointer Forwards, TYPE_Point3DPointer Up, TYPE_Angle3D FovAngle, TYPE_Coord3D NearZ, TYPE_Coord3D FarZ);
  virtual BOOL Uninstall (void);

  TYPE_Point2D cameraCenterWorldPosition;
    // The center of the screen will be at this world coordinate.
    // Move it around to do scrolling.  If a labeled camera is
    // used, this gets updated with the world position of the
    // labeled camera sequence.

  TYPE_Angle2D cameraScreenRotation;
    // Angle of rotation for this camera.  This is derived from the
    // negative of the camera sequence's angle of rotation in the world,
    // if a labeled camera exists.  Zero for straight up, positive
    // rotates the world clockwise relative to the camera/screen.

  TYPE_Scale2D cameraScaleFactor;
    // This controls the zooming in or out of the camera.  If a
    // labeled camera sequence can be found, the scale factor will
    // be taken from it.  Otherwise just use the setting here.
    // A value of 1.0 means pixel to pixel rendering of the bitmaps,
    // 2.0 doubles the size (each source bitmap pixel becomes four
    // screen pixels).  0.5 shrinks things by half.  The labeled camera
    // sequence's enlargement or shrinking of coordinate systems is
    // ignored, only the position and angle are used.

  TYPE_Coord2D cameraOffScreenSoundQuietDistance;
    // The distance off the screen that 2D sounds are still audible.
    // They fade from full volume on the screen to partial left only
    // or right only volume until they are this many extra pixels off
    // the screen edge, when they become quiet.

  TYPE_Matrix2D worldToScreenMatrix;
    // This converts world coordinates into screen coordinates.  It is
    // recalculated every time the camera moves to reflect the various
    // camera attributes in the rest of this record.

  TYPE_Matrix2D screenToWorldMatrix;
    // The inverse of the worldToScreenMatrix.  Mostly used for converting
    // mouse clicks into world coordinates.  Updated whenever
    // worldToScreenMatrix is updated.

  TYPE_Matrix2D previousWorldToScreenMatrix;
    // This one is the one used at the last update.  It is compared with
    // worldToScreenMatrix to see if anything has changed since the last
    // updated.

} *LI_REND2D_SlotDataPointer;



/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

#define LI_REND2D_MAX_PRIVATE_SEQUENCE_DATA (CE_ARTLIB_SeqncrMax2DSequences * CE_ARTLIB_RendMaxRenderSlots)

static PrivateSequenceDataRecord LI_REND2D_PrivateSequenceDataArray [LI_REND2D_MAX_PRIVATE_SEQUENCE_DATA];
  // Auxiliary private data for each 2D sequence is dynamically
  // allocated from this array.  Free nodes are in a singly linked
  // list using the same space.

static PrivateSequenceDataPointer NextFreePrivateDataRecordPntr;
  // Next element in the free list, NULL if no free records.



static LI_REND2D_SlotDataClass SlotDataArray [CE_ARTLIB_RendMaxRenderSlots];

  // Each render slot can have data associated with it.  This
  // wastes a bit of space for slots used by other renderers,
  // but it isn't much.


static HRGN GlobalTempRegionForInvalidArea;
static HRGN GlobalTempRegionForRelatedAreas;
  // These temporary regions are left allocated while the renderer is running
  // so that we can reuse it without reallocating it all the time (instead,
  // the SetRectRgn function is used to change its shape).  Only used for
  // functions that are called many times per render update.  NULL when not
  // yet allocated.  Hopefully this isn't used by recursive functions or
  // called from multiple threads.



/*****************************************************************************
 * Allocate a PrivateSequenceDataRecord, returns NULL when out of memory.
 * Note that we have our own allocation system to avoid fragmenting memory
 * and thus going faster while rendering (all the memory accesses will be
 * nearby and often in the same memory page).
 */

static PrivateSequenceDataPointer LI_REND2D_AllocPrivateData (void)
{
  PrivateSequenceDataPointer ReturnValue;

  if ((ReturnValue = NextFreePrivateDataRecordPntr) == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND2D_AllocPrivateData: Out of memory.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Out of memory.
  }

  // Remove the allocated record from the free list.

  NextFreePrivateDataRecordPntr =
    *((PrivateSequenceDataPointer *) ReturnValue);

  // Clear it to all zeroes, which gives you an empty bounding area.

  memset (ReturnValue, 0, sizeof (PrivateSequenceDataRecord));

  return ReturnValue;
}



/*****************************************************************************
 * Deallocate a PrivateSequenceDataRecord, returns it to free list.
 */

static void LI_REND2D_FreePrivateData (PrivateSequenceDataPointer FreePntr)
{
  if (FreePntr != NULL)
  {
    // Add the record to the free list.

    *((PrivateSequenceDataPointer *) FreePntr) = NextFreePrivateDataRecordPntr;
    NextFreePrivateDataRecordPntr = FreePntr;
  }
}



/*****************************************************************************
 * Combine the various camera settings into a matrix that goes from world
 * coordinates to screen coordinates.  The camera's cameraCenterWorldPosition
 * gets mapped to the center of the cameraRenderArea rectangle on the screen.
 * Scale and rotation are set too.  Also updates the inverse matrix.
 */

static void RecalculateWorldToScreenMatrix (LI_REND2D_SlotDataPointer SlotPntr)
{
  TYPE_Matrix2D MatrixA;
  TYPE_Matrix2D MatrixB;
  TYPE_Matrix2D MatrixC;
  TYPE_Point2D  MoveOrigin;

  // Move the world so that the camera center position goes to the origin.

  MoveOrigin.x = - SlotPntr->cameraCenterWorldPosition.x;
  MoveOrigin.y = - SlotPntr->cameraCenterWorldPosition.y;
  LE_MATRIX_2D_SetTranslate (&MoveOrigin, &MatrixA);

  // Blow up the world by the scale factor.

  LE_MATRIX_2D_SetScale (SlotPntr->cameraScaleFactor, &MatrixB);
  LE_MATRIX_2D_MatrixMultiply (&MatrixA, &MatrixB, &MatrixC);

  // Rotate around the origin.

  LE_MATRIX_2D_SetRotate (SlotPntr->cameraScreenRotation, &MatrixA);
  LE_MATRIX_2D_MatrixMultiply (&MatrixC, &MatrixA, &MatrixB);

  // Move the whole mess so that the origin is at the center of the
  // cameraRenderArea rectangle on the screen.

  MoveOrigin.x = SlotPntr->cameraRenderArea.left +
    (SlotPntr->cameraRenderArea.right - SlotPntr->cameraRenderArea.left) / 2;
  MoveOrigin.y = SlotPntr->cameraRenderArea.top +
    (SlotPntr->cameraRenderArea.bottom - SlotPntr->cameraRenderArea.top) / 2;
  LE_MATRIX_2D_SetTranslate (&MoveOrigin, &MatrixA);
  LE_MATRIX_2D_MatrixMultiply (&MatrixB, &MatrixA,
    &SlotPntr->worldToScreenMatrix);

  // Find the inverse so that you can convert mouse coordinates (same as
  // screen coordinates) into world coordinates.  If no inverse exists,
  // just use the identity matrix.

  if (!LE_MATRIX_2D_MatrixInverse (&SlotPntr->worldToScreenMatrix,
  &SlotPntr->screenToWorldMatrix))
    LE_MATRIX_2D_SetIdentity (&SlotPntr->screenToWorldMatrix);
}



/*****************************************************************************
 * Recalculates the position of a 2D object in screen coordinates and update
 * the bounding box in the private data to match.  Doesn't add the box to
 * invalid areas or do anything else with it.
 */

static void RecalculateBoundingBox (
  LI_REND2D_SlotDataPointer SlotPntr,
  LE_SEQNCR_RuntimeInfoPointer SequencePntr,
  PrivateSequenceDataPointer PrivateDataPntr)
{
  int                             i;
  LE_SEQNCR_Runtime2DInfoPointer  Info2DPntr;
  TYPE_Point2DPointer             PointPntr;
  TYPE_Matrix2D                   SequenceToScreenMatrix;

  Info2DPntr = SequencePntr->dimensionSpecificData.d2;

#if CE_ARTLIB_EnableDebugMode
  if (Info2DPntr == NULL || SequencePntr->dimensionality != 2)
    LE_ERROR_ErrorMsg ("L_Rend2D.c RecalculateBoundingBox: "
    "No 2D attached data!\r\n");
#endif

  // First get the sequence to screen coordinates transformation.
  // Unfortunately each sequence has its own coordinate system and
  // thus we need to do a matrix multiplication for each sequence.

  LE_MATRIX_2D_MatrixMultiply (
    &Info2DPntr->sequenceToWorldTransformation.matrix2D,
    &SlotPntr->worldToScreenMatrix,
    &SequenceToScreenMatrix);

  // Convert the bounding rectangle into four corner points, which we need
  // because the result may be a rotated box, which isn't a rectangle.

  PrivateDataPntr->boundingBox.named.topLeft.x =
    PrivateDataPntr->boundingBox.named.bottomLeft.x =
    Info2DPntr->boundingBox.left;

  PrivateDataPntr->boundingBox.named.topRight.x =
    PrivateDataPntr->boundingBox.named.bottomRight.x =
    Info2DPntr->boundingBox.right;

  PrivateDataPntr->boundingBox.named.topLeft.y =
    PrivateDataPntr->boundingBox.named.topRight.y =
    Info2DPntr->boundingBox.top;

  PrivateDataPntr->boundingBox.named.bottomLeft.y =
    PrivateDataPntr->boundingBox.named.bottomRight.y =
    Info2DPntr->boundingBox.bottom;

  // Transform the four corners to get our screen coordinates,
  // and thus the bounding box.  Gee, I hate using words like
  // "thus", it sounds too academic.  But what can you do?

  for (i = 0; i < 4; i++)
  {
    PointPntr = PrivateDataPntr->boundingBox.asArray + i;
    LE_MATRIX_2D_VectorMultiply (PointPntr, &SequenceToScreenMatrix, PointPntr);
  }
}



/*****************************************************************************
 * Goes through all 2D items in the tree and recalculates their position
 * in screen coordinates and updates their bounding boxes.  Assumes that
 * the render slot sets in the tree are inherited from parents, so we
 * can cut off traversal at nodes that aren't for this renderer.
 */

static void RecalculateAllBoundingBoxes (LE_SEQNCR_RuntimeInfoPointer RootPntr,
LI_REND2D_SlotDataPointer SlotPntr)
{
  BOOL                          InOurSlot;
  PrivateSequenceDataPointer    PrivateDataPntr;
  LE_REND_RenderSlotSet         RenderMask;
  LE_REND_RenderSlot            RenderSlot;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

  RenderSlot = SlotPntr->mySlotNumber;
  RenderMask = SlotPntr->mySlotSetMask;
  SequencePntr = RootPntr;

  while (TRUE)
  {
    // Process the current node.

    InOurSlot = ((SequencePntr->renderSlotSet & RenderMask) != 0);

    if (InOurSlot && SequencePntr->dimensionality == 2)
    {
      PrivateDataPntr = (PrivateSequenceDataPointer) SequencePntr->renderSlotData[RenderSlot];
#if CE_ARTLIB_EnableDebugMode
      if (PrivateDataPntr == NULL)
        LE_ERROR_ErrorMsg ("L_Rend2D.c RecalculateAllBoundingBoxes: "
        "Sequence is missing its private render slot data.\r\n");
#endif

      RecalculateBoundingBox (SlotPntr, SequencePntr, PrivateDataPntr);
    }

    // Traverse down the tree, then do siblings, then back up to the
    // next parent's sibling.  If this node isn't rendered by this
    // renderer then its children certainly don't need to be.  That's
    // why the 2D renderer also accepts 0D grouping sequences in
    // LI_REND2D_CanHandleSequence, since they can contain children
    // that are 2D.

    if (SequencePntr->child != NULL && InOurSlot)
      SequencePntr = SequencePntr->child;
    else
    {
      while (SequencePntr != NULL && SequencePntr->sibling == NULL)
        SequencePntr = SequencePntr->parent;
      if (SequencePntr == NULL)
        break; // All done.
      SequencePntr = SequencePntr->sibling;
    }
  }
}



/*****************************************************************************
 * Calculates the upright bounding rectangle in screen coordinates from the
 * given bounding corners in the private data record.  If the bitmap is
 * rotating then the rectangle will be a bit bigger to hold it all.
 */

inline void CalculateEnclosingScreenRect (
PrivateSequenceDataPointer PrivateData, TYPE_RectPointer EnclosingRectanglePntr)
{
  int             i;
  TYPE_Rect       Rectangle;
  LONG            X, Y;

  // First find the outer limits of the bounding box, which
  // could be rotated and thus is not as obvious as you'd think.

  Rectangle.left = Rectangle.right = PrivateData->boundingBox.named.topLeft.x;
  Rectangle.top = Rectangle.bottom = PrivateData->boundingBox.named.topLeft.y;

  for (i = 1; i < 4; i++)
  {
    X = PrivateData->boundingBox.asArray[i].x;
    if (X < Rectangle.left) Rectangle.left = X;
    if (X > Rectangle.right) Rectangle.right = X;

    Y = PrivateData->boundingBox.asArray[i].y;
    if (Y < Rectangle.top) Rectangle.top = Y;
    if (Y > Rectangle.bottom) Rectangle.bottom = Y;
  }

  *EnclosingRectanglePntr = Rectangle;
}



/*****************************************************************************
 * Adds a rectangle to this render slots's invalid area region.  This is
 * called frequently so make it efficient.
 */

static void AddRectangleToInvalidArea (LI_REND2D_SlotDataPointer SlotPntr,
TYPE_RectPointer RectanglePntr)
{
  if (GlobalTempRegionForInvalidArea == NULL)
  {
    GlobalTempRegionForInvalidArea = CreateRectRgnIndirect (RectanglePntr);
    if (GlobalTempRegionForInvalidArea == NULL)
      return; // Failed to create region.
  }
  else // Use existing region - avoids an extra allocation.
    SetRectRgn (GlobalTempRegionForInvalidArea,
    RectanglePntr->left, RectanglePntr->top,
    RectanglePntr->right, RectanglePntr->bottom);

  if (SlotPntr->invalidArea == NULL)
  {
    SlotPntr->invalidArea = GlobalTempRegionForInvalidArea;
    GlobalTempRegionForInvalidArea = NULL; // Give them our region.
  }
  else // Have existing region to combine it with.
  {
    CombineRgn (SlotPntr->invalidArea,
      SlotPntr->invalidArea, GlobalTempRegionForInvalidArea, RGN_OR);
  }
}



/*****************************************************************************
 * Adds a region to this render slots's invalid area region.
 *
 * This function is called by the sequencer or another render slot when
 * it wants to add some invalid area (in screen coordinates) to a renderer.
 * This is useful for overlapping render slots, such as the one drawing
 * the mouse over the whole screen - when the mouse moves, the underlying
 * render slots need to be told to redraw the affected areas.  The
 * ScreenSurface is passed in so that the render slot can decide if it
 * is drawing to the same screen as the caller is drawing to (if it is
 * different then it ignores the invalid areas, NULL to always update
 * the area).
 */

void LI_REND2D_SlotDataClass::AddInvalidArea (
HRGN InvalidAreaRegion, PDDSURFACE ScreenSurface)
{
  if (ScreenSurface != NULL && outputSurface != ScreenSurface)
    return; // Some other screen surface, ignore this bogus update.

  if (InvalidAreaRegion == NULL)
    return; // Nothing to do.

  if (invalidArea == NULL)
  {
    invalidArea = CreateRectRgn (0, 0, 0, 0);
    CombineRgn (invalidArea, InvalidAreaRegion, InvalidAreaRegion, RGN_COPY);
  }
  else // Have existing region to combine it with.
  {
    CombineRgn (invalidArea, invalidArea, InvalidAreaRegion, RGN_OR);
  }
}



/*****************************************************************************
 * Adds a region to the invalid area of all related render slots.
 */

static void AddRegionToRelatedRenderSlotsInvalidArea (
LI_REND2D_SlotDataPointer SlotPntr, HRGN InvalidAreaRegion)
{
  LI_REND_AbstractRenderSlotPointer OtherSlotPntr;
  LE_REND_RenderSlot                RenderSlot;

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
  {
    if (SlotPntr->overlappingRenderSlotSet & (1 << RenderSlot))
    {
      OtherSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];
      if (OtherSlotPntr != NULL)
        OtherSlotPntr->AddInvalidArea (InvalidAreaRegion,
        SlotPntr->outputSurface);
    }
  }
}



/*****************************************************************************
 * Adds a rectangle to the invalid area of all related render slots.
 * This one gets called a lot so make it efficient.
 */

static void AddRectangleToRelatedRenderSlotsInvalidArea (LI_REND2D_SlotDataPointer SlotPntr,
TYPE_RectPointer InvalidAreaRectPntr)
{
  if (SlotPntr->overlappingRenderSlotSet == 0)
    return; // Nothing to do, no related slots.

  if (GlobalTempRegionForRelatedAreas == NULL)
  {
    GlobalTempRegionForRelatedAreas = CreateRectRgnIndirect (InvalidAreaRectPntr);
    if (GlobalTempRegionForRelatedAreas == NULL)
      return; // Failed to create a region.
  }
  else // Use existing region - avoids an extra allocation.
    SetRectRgn (GlobalTempRegionForRelatedAreas,
    InvalidAreaRectPntr->left, InvalidAreaRectPntr->top,
    InvalidAreaRectPntr->right, InvalidAreaRectPntr->bottom);

  AddRegionToRelatedRenderSlotsInvalidArea (SlotPntr, GlobalTempRegionForRelatedAreas);
}



/*****************************************************************************
 * Returns a region containing all the screen area occupied by this render
 * slot.  Just goes through all sequences in this render slot and adds
 * their bounding rectangles to the region, then clips by the rectangular
 * cameraRenderArea as well as the visible part of the virtual screen.
 * The given screen surface needs to match our output surface too
 * (or be NULL).  Ignores the area occupied by sounds, since they don't
 * get drawn to the screen.
 */

HRGN LI_REND2D_SlotDataClass::GetSolidArea (
LE_SEQNCR_RuntimeInfoPointer RootPntr, PDDSURFACE ScreenSurface)
{
  TYPE_Rect                     EnclosingRect;
  BOOL                          InOurSlot;
  PrivateSequenceDataPointer    PrivateDataPntr;
  LE_REND_RenderSlotSet         RenderMask;
  HRGN                          ReturnedRegion;
  HRGN                          TempRegion;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

  ReturnedRegion = CreateRectRgn (0, 0, 0, 0);

  if (ScreenSurface != NULL && outputSurface != ScreenSurface ||
  ReturnedRegion == NULL)
    return ReturnedRegion; // Some other screen surface, not relevant to us.

  TempRegion = CreateRectRgn (0, 0, 0, 0);
  if (TempRegion == NULL)
    return ReturnedRegion;

  RenderMask = mySlotSetMask;
  SequencePntr = RootPntr;

  while (TRUE)
  {
    // Process the current node.

    InOurSlot = ((SequencePntr->renderSlotSet & RenderMask) != 0);

    if (InOurSlot &&
    (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_2DBITMAP ||
    SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO ||
    SequencePntr->drawSelectionBox))
    {
      // Add the bitmap's on-screen area to the collected area.

      PrivateDataPntr = (PrivateSequenceDataPointer) SequencePntr->renderSlotData[mySlotNumber];
      CalculateEnclosingScreenRect (PrivateDataPntr, &EnclosingRect);

      // Trim off the stuff which isn't visible in the current display,
      // that includes stuff off the screen and off the camera's
      // rendering area (which could be off the screen too if we
      // are running in windowed mode).

      IntersectRect (&EnclosingRect, &EnclosingRect,
        &LE_GRAFIX_ScreenSourceRect);

      if (IntersectRect (&EnclosingRect, &EnclosingRect, &cameraRenderArea))
      {
        SetRectRgn (TempRegion,
          EnclosingRect.left, EnclosingRect.top,
          EnclosingRect.right, EnclosingRect.bottom);

        CombineRgn (ReturnedRegion, ReturnedRegion, TempRegion, RGN_OR);
      }
    }

    // Traverse down the tree, then do siblings, then back up to
    // the next parent's sibling.

    if (SequencePntr->child != NULL && InOurSlot)
      SequencePntr = SequencePntr->child;
    else // Move along to the next sibling.
    {
      while (SequencePntr != NULL && SequencePntr->sibling == NULL)
        SequencePntr = SequencePntr->parent;
      if (SequencePntr == NULL)
        break; // All done.
      SequencePntr = SequencePntr->sibling;
    }
  }

  DeleteObject (TempRegion);
  return ReturnedRegion;
}



/*****************************************************************************
 * Go through all related slots and get their solid (non-transparent) areas.
 * Return the combined resulting solid area clipped to our camera area
 * rectangle and visible screen portion.
 */

static HRGN GetRelatedSlotSolidAreas (LI_REND2D_SlotDataPointer SlotPntr,
LE_SEQNCR_RuntimeInfoPointer RootPntr)
{
  HRGN                                      CameraAreaRegion;
  LI_REND_AbstractRenderSlotPointer         OtherSlotPntr;
  LE_REND_RenderSlot                        RenderSlot;
  HRGN                                      ReturnedRegion;
  RECT                                      TempRect;
  HRGN                                      TempRegion;

  if (SlotPntr->overlappingRenderSlotSet == 0)
    return NULL; // Nothing to do, no related slots.

  ReturnedRegion = CreateRectRgn (0, 0, 0, 0);
  if (ReturnedRegion == NULL)
    return ReturnedRegion;

  if (!IntersectRect (&TempRect, &SlotPntr->cameraRenderArea,
  &LE_GRAFIX_ScreenSourceRect))
    return ReturnedRegion; // Camera drawing area is totally off screen.

  CameraAreaRegion = CreateRectRgnIndirect (&TempRect);
  if (CameraAreaRegion == NULL)
    return ReturnedRegion;

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
  {
    if (SlotPntr->overlappingRenderSlotSet & (1 << RenderSlot))
    {
      OtherSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];
      if (OtherSlotPntr != NULL)
      {
        TempRegion = OtherSlotPntr->GetSolidArea (RootPntr,
          SlotPntr->outputSurface);

        if (TempRegion != NULL)
        {
          CombineRgn (TempRegion, TempRegion, CameraAreaRegion, RGN_AND);
          CombineRgn (ReturnedRegion, ReturnedRegion, TempRegion, RGN_OR);
          DeleteObject (TempRegion);
        }
      }
    }
  }

  DeleteObject (CameraAreaRegion);
  return ReturnedRegion;
}



/*****************************************************************************
 * Add the current bounding box in the private data to the invalid area that
 * needs to be redrawn.  Returns TRUE if any of it is on screen.  Normally
 * only 2D sequences that actually draw stuff should have their bounding
 * boxes invalidated - bitmaps but not groups.
 */

static BOOL InvalidateBoundingBox (PrivateSequenceDataPointer PrivateData,
LI_REND2D_SlotDataPointer SlotPntr, BOOL AlsoUpdateOverlappingRenderers)
{
  TYPE_Rect Rectangle;

  CalculateEnclosingScreenRect (PrivateData, &Rectangle);

  // Pre-clip the bounding box by the screen area we are using, cheaper
  // to do it here than wasting space keeping track of off-screen areas.

  if (!IntersectRect (&Rectangle, &Rectangle, &SlotPntr->cameraRenderArea))
    return FALSE; // Empty bounding box.

  // Also clip it to the part of the virtual screen which is visible.

  if (!IntersectRect (&Rectangle, &Rectangle, &LE_GRAFIX_ScreenSourceRect))
    return FALSE; // Empty bounding box.

  AddRectangleToInvalidArea (SlotPntr, &Rectangle);

  if (AlsoUpdateOverlappingRenderers)
    AddRectangleToRelatedRenderSlotsInvalidArea (SlotPntr, &Rectangle);

  return TRUE; // Yes, this is on the screen.
}



/*****************************************************************************
 * Add the whole screen (drawing area really) to the invalid area that
 * needs to be redrawn.
 */

static void InvalidateScreen (LI_REND2D_SlotDataPointer SlotPntr,
BOOL AlsoUpdateOverlappingRenderers)
{
  RECT  TempRect;

  if (IntersectRect (&TempRect, &SlotPntr->cameraRenderArea,
  &LE_GRAFIX_ScreenSourceRect))
  {
    if (SlotPntr->invalidArea != NULL)
      SetRectRgn (SlotPntr->invalidArea,
      TempRect.left, TempRect.top,
      TempRect.right, TempRect.bottom);
    else
      SlotPntr->invalidArea = CreateRectRgnIndirect (&TempRect);

    if (AlsoUpdateOverlappingRenderers)
      AddRectangleToRelatedRenderSlotsInvalidArea (SlotPntr,
      &TempRect);
  }
}



/*****************************************************************************
 * Go through the tree of sequences and add the bounding boxes of the ones
 * that have their redraw flag set to the area needing redrawing.  Traversal
 * is cut off at nodes that don't have the redraw flag set (their children
 * are assumed to not need redrawing).  Non-drawing nodes (like grouping ones
 * or 2D sound effects) don't have their bounding boxes added to the invalid
 * area, unless selected (useful for the dope sheet where we see their
 * bounding boxes being drawn).  Only this render slot's invalid area is
 * used, it is more efficient to copy the final area to other slots than
 * updating each one individually.
 */

static void InvalidateAllSequencesNeedingRedraw (
LE_SEQNCR_RuntimeInfoPointer RootPntr, LI_REND2D_SlotDataPointer SlotPntr)
{
  PrivateSequenceDataPointer    PrivateDataPntr;
  LE_REND_RenderSlotSet         RenderMask;
  LE_REND_RenderSlot            RenderSlot;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

  RenderSlot = SlotPntr->mySlotNumber;
  RenderMask = SlotPntr->mySlotSetMask;
  SequencePntr = RootPntr;

  while (TRUE)
  {
    // Process the current node.  This is optimised for handling the usual
    // case of lots of nodes where only a few need redrawing.  So efficiently
    // traverse the tree and simplify the initial tests.

    if (SequencePntr->needsRedraw && (SequencePntr->renderSlotSet & RenderMask))
    {
      if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_2DBITMAP ||
      SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO ||
      SequencePntr->drawSelectionBox)
      {
        PrivateDataPntr = (PrivateSequenceDataPointer) SequencePntr->renderSlotData[RenderSlot];
  #if CE_ARTLIB_EnableDebugMode
        if (PrivateDataPntr == NULL)
          LE_ERROR_ErrorMsg ("L_Rend2D.c InvalidateAllSequencesNeedingRedraw: "
          "Sequence is missing its private render slot data.\r\n");
  #endif

        InvalidateBoundingBox (PrivateDataPntr, SlotPntr, FALSE /* Related Slots */);
      }

      // Advance to the next sequence, which includes children.

      if (SequencePntr->child != NULL)
        SequencePntr = SequencePntr->child;
      else // Move along to the next sibling.
      {
        while (SequencePntr != NULL && SequencePntr->sibling == NULL)
          SequencePntr = SequencePntr->parent;
        if (SequencePntr == NULL)
          break; // All done.
        SequencePntr = SequencePntr->sibling;
      }
    }
    else
    {
      // Advance to the next sequence, excluding children since we know
      // the current sequence isn't interesting to us therefore its
      // children are also irrelevant.

      while (SequencePntr != NULL && SequencePntr->sibling == NULL)
        SequencePntr = SequencePntr->parent;
      if (SequencePntr == NULL)
        break; // All done.
      SequencePntr = SequencePntr->sibling;
    }
  }
}



/*****************************************************************************
 * Move the screen pixels and offset all bounding boxes and invalidation
 * areas to match.  Also invalidate the freshly revealed area on the screen.
 */

static void ScrollScreen (LI_REND2D_SlotDataPointer SlotPntr,
TYPE_Point2DPointer TranslationPntr)
{
  struct CombinedRectanglesStruct
  {
    TYPE_Rect   DestRect;
    TYPE_Rect   SourceRect;
  } RectArray;

  HRESULT       ErrorCode;
  TYPE_Coord2D  Height;
  TYPE_Rect     Rectangle;
  TYPE_Rect     VisibleScreenRect;
  TYPE_Coord2D  Width;

  // Find out the visible part of the camera area on the screen.

  if (!IntersectRect (&VisibleScreenRect, &SlotPntr->cameraRenderArea,
  &LE_GRAFIX_ScreenSourceRect))
    return; // This render slot isn't visible at all.  No drawing needed.

  // First move the whole invalid area by the translation distance,
  // so that it will still be valid after the translation.

  if (SlotPntr->invalidArea != NULL)
    OffsetRgn (SlotPntr->invalidArea, TranslationPntr->x, TranslationPntr->y);

  // Add on an invalid area for the sliver(s) of new space that appear
  // at the edge(s) of the screen. First handle left or right sliver.

  Rectangle = VisibleScreenRect;
  Width = Rectangle.right - Rectangle.left;
  if (TranslationPntr->x < 0)
  {
    // Shifting things left, mark a sliver on the right for redraw.
    // Move left side of whole screen rectangle rightwards.

    Rectangle.left += Width + TranslationPntr->x;
    AddRectangleToInvalidArea (SlotPntr, &Rectangle);
  }
  else if (TranslationPntr->x > 0)
  {
    // Shifting things right, mark a sliver on the left for redraw.
    // Move right side of whole screen rectangle leftwards.

    Rectangle.right -= Width - TranslationPntr->x;
    AddRectangleToInvalidArea (SlotPntr, &Rectangle);
  }

  // Now add an invalid area for the top or bottom sliver.

  Rectangle = VisibleScreenRect;
  Height = Rectangle.bottom - Rectangle.top;
  if (TranslationPntr->y < 0)
  {
    // Shifting things up, mark a sliver on the bottom for redraw.
    // Move top side of whole screen rectangle downwards.

    Rectangle.top += Height + TranslationPntr->y;
    AddRectangleToInvalidArea (SlotPntr, &Rectangle);
  }
  else if (TranslationPntr->y > 0)
  {
    // Shifting things down, mark a sliver on the top for redraw.
    // Move bottom side of whole screen rectangle upwards.

    Rectangle.bottom -= Height - TranslationPntr->y;
    AddRectangleToInvalidArea (SlotPntr, &Rectangle);
  }

  // Physically move the pixels on the screen by the translation distance.
  // Since this is a video memory to video memory operation, it may be fast.
  // However, the user can see it happening.  Avoid going outside the
  // rendering area (manually clip the scrolling to the area).

  Rectangle = VisibleScreenRect;
  OffsetRect (&Rectangle, TranslationPntr->x, TranslationPntr->y);
  if (IntersectRect (&RectArray.DestRect, &Rectangle, &VisibleScreenRect))
  {
    // Yes, there is part of the original screen image still on screen.

    RectArray.SourceRect = RectArray.DestRect;
    OffsetRect (&RectArray.SourceRect, -TranslationPntr->x, -TranslationPntr->y);

    // Convert to true screen coordinates, to compensate for windowing
    // and pixel zooming.

    LE_GRAFIX_ConvertRectanglesToScreenCoordinates (&RectArray.DestRect,
      2 /* NumberOfRectangles */);

    // Waiting for vertical blank just makes the tearing glitch appear
    // at one spot on the screen.  And slows the animation down.
    // So don't bother with it.  Well, unless you have a smaller
    // screen size, say 320x200?  Then it could do the scrolling
    // faster than it draws the image on the CRT screen.

#if CE_ARTLIB_SeqncrUseVerticalBlank
    LE_GRAFIX_DirectDrawObjectN->WaitForVerticalBlank (DDWAITVB_BLOCKBEGIN, NULL);
#endif

    ErrorCode = SlotPntr->outputSurface->BltFast (
        RectArray.DestRect.left, RectArray.DestRect.top,
        SlotPntr->outputSurface,
        &RectArray.SourceRect,
        DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

#if CE_ARTLIB_EnableDebugMode
    if (ErrorCode == DDERR_UNSUPPORTED)
      LE_ERROR_ErrorMsg ("L_Rend2D.c ScrollScreen: "
      "Unsupported Blit error, did you have a clipper attached to the screen?\r\n");
    else if (ErrorCode != DD_OK)
      LE_ERROR_ErrorMsg ("L_Rend2D.c ScrollScreen: "
      "Some sort of blitter error.\r\n");
#endif
  }
}



/*****************************************************************************
 * Draws a single 2D bitmap type of object to the working area.  Only the
 * part that needs to be redrawn is actually drawn.  This redrawn part
 * is actually a list of rectangles taken from a region.  Assumes that the
 * working area has been locked down (workingBitsPntr is valid).
 */

static void DrawBitmapToWorkArea (
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr,
  LI_REND2D_SlotDataPointer     SlotPntr,
  PrivateSequenceDataPointer    PrivateDataPntr,
  TYPE_RectPointer              BoundingRectPntr,
  HRGN                          RedrawRegion)
{
  struct MyRegionStruct {
    RGNDATA header;
    BYTE    buffer[LI_REND_REGION_RECTANGLE_BUFFER_SPACE-1];
  };

  enum DrawModeEnum {
    DRAW_SOLID,
    DRAW_ALPHA,
    DRAW_COLOUR_KEY,
    DRAW_INVISIBLE
  };

  UNS16                             BitmapAlphaCount = 0;
  BYTE                             *BitmapBitsPntr = NULL;
  UNS16                             BitmapBytesPerScanLine;
  UNS16                             BitmapSolidColourCount = 0;
  void                             *BitmapColourTablePntr = NULL;
  UNS8                              BitmapDepth = 0;
  UNS8                              BitmapDrawMode = DRAW_INVISIBLE;
  LE_GRAFIX_GenericBitmapPointer    BitmapGBMPntr = NULL;
  UNS16                             BitmapHeight;
  BITMAPV4HEADER                    BitmapInfo;
  BITMAPINFOHEADER                 *BitmapInfoPntr;
  LPNEWBITMAPHEADER                 BitmapUCPHeaderPntr = NULL;
  UNS16                             BitmapOffsetX;
  UNS16                             BitmapOffsetY;
  RECT                              BitmapRect;
  UNS16                             BitmapWidth;
  int                               BoxSide;
  LE_DATA_DataType                  DataType;
  BOOL                              DrawSelectionBox;
  DWORD                             ErrorCode;
  int                               i;
  LPRECT                            InvalidRectPntr;
  struct MyRegionStruct             RegionData;
  UNS16                             RotateFakeDestBitmapWidth;
  UNS16                             RotateFakeDestBitmapHeight;
  LPBYTE                            RotateFakeDestBitmapBitsPntr;
  PrivateSequenceDataRecord         RotateFakeCorners;
  BOOL                              Rotating;
  BOOL                              Scaling;
  TYPE_Rect                         SelectionBoxRectangle;
  RECT                              SourceRect;
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

  // Find the actual size of the bitmap.  If this is a TAB file,
  // there is a hidden origin offset in the bitmap data.  We also
  // get pointers to data here.

  if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_2DBITMAP)
  {
    DataType = LE_DATA_GetLoadedDataType (SequencePntr->contentsDataID);

    if (DataType == LE_DATA_DataUAP)
    {
      // Recolour the TAB if it is in the auto-recolour range.

#if CE_ARTLIB_SeqncrMaxRecolourRanges > 0
      int                           i;
      LE_SEQNCR_AutoRecolourPointer RecolourPntr;

      for (i = 0, RecolourPntr = LE_SEQNCR_AutoRecolourRangesArray + 0;
      i < CE_ARTLIB_SeqncrMaxRecolourRanges;
      i++, RecolourPntr++)
      {
        if (RecolourPntr->firstInRange == LE_DATA_EmptyItem ||
        SequencePntr->contentsDataID < RecolourPntr->firstInRange ||
        SequencePntr->contentsDataID > RecolourPntr->lastInRange ||
        RecolourPntr->colourMapArray == NULL ||
        RecolourPntr->colourMapSize <= 0)
          continue; // Nope, don't want to recolour this one.

        LE_GRAFIXBulkRecolorTAB (SequencePntr->contentsDataID,
          RecolourPntr->colourMapArray, RecolourPntr->colourMapSize);
      }
#endif /* CE_ARTLIB_SeqncrMaxRecolourRanges > 0 */

      BitmapUCPHeaderPntr = (LPNEWBITMAPHEADER)
        LE_DATA_Use (SequencePntr->contentsDataID);
      if (BitmapUCPHeaderPntr == NULL)
      {
#if CE_ARTLIB_EnableDebugMode
        // Note that the screen may be locked (if working surface is the same
        // as the output surface and is using video memory), and error message
        // display will cause Windows to hang in that situation.
        sprintf (LE_ERROR_DebugMessageBuffer, "L_Rend2D.c DrawBitmapToWorkArea: "
          "Unreadable TAB bitmap at data ID $%08X.\r\n",
          (int) SequencePntr->contentsDataID);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
        goto ErrorExit;
      }

      BitmapDepth = 8; // TABs are always 8 bit with palette.
      BitmapBitsPntr = (LPBYTE) (BitmapColourTablePntr = &BitmapUCPHeaderPntr->dwColorTable);
      BitmapBitsPntr += NEWBITMAP_COLOUR_ENTRY_SIZE * BitmapUCPHeaderPntr->nColors;
      BitmapRect.right = BitmapWidth = BitmapUCPHeaderPntr->nXBitmapWidth;
      BitmapBytesPerScanLine = ((BitmapWidth + 3) & 0xFFFFFFFC);
      BitmapRect.bottom = BitmapHeight = BitmapUCPHeaderPntr->nYBitmapHeight;
      BitmapRect.top = BitmapRect.left = 0;
      BitmapOffsetX = BitmapUCPHeaderPntr->nXOriginOffset;
      BitmapOffsetY = BitmapUCPHeaderPntr->nYOriginOffset;
      BitmapAlphaCount = BitmapUCPHeaderPntr->nAlpha;
      BitmapSolidColourCount = BitmapUCPHeaderPntr->nColors;
      if (BitmapUCPHeaderPntr->dwFlags & BITMAP_ALPHACHANNEL)
        BitmapDrawMode = DRAW_ALPHA;
      else if (BitmapUCPHeaderPntr->dwFlags & BITMAP_NOTRANSPARENCY)
        BitmapDrawMode = DRAW_SOLID;
      else
        BitmapDrawMode = DRAW_COLOUR_KEY;
    }
    else if (DataType == LE_DATA_DataNative)
    {
      // This is one of those runtime bitmaps of native depth,
      // created by LE_GRAFIX_ObjectCreate and friends.  No palette,
      // instead nAlpha is the overall transparency of the thing
      // and there is a flag for solid vs transparent.

      BitmapUCPHeaderPntr = (LPNEWBITMAPHEADER)
        LE_DATA_Use (SequencePntr->contentsDataID);
      if (BitmapUCPHeaderPntr == NULL)
      {
#if CE_ARTLIB_EnableDebugMode
        // Note that the screen may be locked (if working surface is the same
        // as the output surface and is using video memory), and error message
        // display will cause Windows to hang in that situation.
        sprintf (LE_ERROR_DebugMessageBuffer, "L_Rend2D.c DrawBitmapToWorkArea: "
          "Unreadable imaginary bitmap at data ID $%08X.\r\n",
          (int) SequencePntr->contentsDataID);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
        goto ErrorExit;
      }

      BitmapDepth = (UNS8) LE_GRAFIX_ScreenBitsPerPixel; // These are always native depth.
      BitmapBitsPntr = (LPBYTE) &BitmapUCPHeaderPntr->dwColorTable;
      BitmapRect.right = BitmapWidth = BitmapUCPHeaderPntr->nXBitmapWidth;
      BitmapBytesPerScanLine = ((BitmapWidth * (BitmapDepth / 8) + 3) & 0xFFFFFFFC);
      BitmapRect.bottom = BitmapHeight = BitmapUCPHeaderPntr->nYBitmapHeight;
      BitmapRect.top = BitmapRect.left = 0;
      BitmapOffsetX = BitmapUCPHeaderPntr->nXOriginOffset;
      BitmapOffsetY = BitmapUCPHeaderPntr->nYOriginOffset;
      BitmapAlphaCount = BitmapUCPHeaderPntr->nAlpha; // Really global alpha level.
      if (BitmapUCPHeaderPntr->dwFlags & BITMAP_NOTRANSPARENCY)
        BitmapDrawMode = DRAW_SOLID;
      else if (BitmapAlphaCount == ALPHA_OPAQUE100_0)
        BitmapDrawMode = DRAW_COLOUR_KEY;
      else if (BitmapAlphaCount == ALPHA_TRANSPARENT)
        BitmapDrawMode = DRAW_INVISIBLE; // So transparent that you can't see it.
      else
        BitmapDrawMode = DRAW_ALPHA;
    }
    else if (DataType == LE_DATA_DataGenericBitmap)
    {
      // A generic bitmap, really a DirectDraw surface.  Hack - just draw solid.

      BitmapGBMPntr = LE_GRAFIX_RestoreAndLockGBM (SequencePntr->contentsDataID);
      if (BitmapGBMPntr == NULL)
      {
#if CE_ARTLIB_EnableDebugMode
        // Note that the screen may be locked (if working surface is the same
        // as the output surface and is using video memory), and error message
        // display will cause Windows to hang in that situation.
        sprintf (LE_ERROR_DebugMessageBuffer, "L_Rend2D.c DrawBitmapToWorkArea: "
          "Unreadable generic bitmap at data ID $%08X.\r\n",
          (int) SequencePntr->contentsDataID);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
        goto ErrorExit;
      }

      BitmapBitsPntr = BitmapGBMPntr->bitmapBits;
      BitmapColourTablePntr = &ErrorCode; // Todo: get palette.
      BitmapDepth = BitmapGBMPntr->bitsPerPixel;
      BitmapRect.right = BitmapWidth = BitmapGBMPntr->width;
      BitmapBytesPerScanLine = (UNS16) BitmapGBMPntr->bytesToNextScanLine;
      BitmapRect.bottom = BitmapHeight = BitmapGBMPntr->height;
      BitmapRect.top = BitmapRect.left = 0;
      BitmapOffsetX = BitmapOffsetY = 0;
      BitmapAlphaCount = BitmapGBMPntr->globalAlphaLevel;
      BitmapDrawMode = DRAW_SOLID;
    }
    else // Unknown bitmap type.
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "L_Rend2D.c DrawBitmapToWorkArea: "
        "Unknown bitmap type at data ID $%08X.\r\n",
        (int) SequencePntr->contentsDataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
      goto ErrorExit;
    }
  }
  else if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
  {
    BitmapInfo.bV4Size = sizeof (BitmapInfo);
    BitmapInfoPntr = (BITMAPINFOHEADER *) &BitmapInfo;
    BitmapBitsPntr = NULL;
    VideoPntr = SequencePntr->streamSpecificData.videoStream;

    if (VideoPntr->drawDirectlyToScreen)
    {
      // Combine feeding and drawing in one operation.  This will
      // look weird if it is only drawing part of the bitmap (like
      // when it refreshes the screen just under the mouse pointer).
      // If that's a problem, turn off the mouse pointer or use the
      // intermediate bitmap facility.

      if (!LI_SEQNCR_GlueFeedAndDrawVideo (SequencePntr,
      BitmapInfoPntr, &BitmapBitsPntr,
      BoundingRectPntr->left, BoundingRectPntr->top, /* Scaling not supported */
      (LPBYTE) SlotPntr->workingBitsPntr))
        goto ErrorExit; // Something went wrong.

      if (BitmapBitsPntr == NULL)
        goto NormalExit; // It drew the picture on the screen for us, or no picture.
    }
    else // Using an intermediate bitmap, already has picture in it.
    {
#if USE_OLD_VIDEO_PLAYER
      // Old video player uses GetFrame to get the intermediate bitmap.

      if (VideoPntr->videoGetFrameFunction == NULL)
        goto ErrorExit;
      if (VideoPntr->currentVideoFrameNumber >= VideoPntr->numberOfVideoFrames)
        BitmapInfoPntr = (LPBITMAPINFOHEADER) AVIStreamGetFrame (
        VideoPntr->videoGetFrameFunction,
        VideoPntr->numberOfVideoFrames - 1);
      else // Have a valid frame number.
        BitmapInfoPntr = (LPBITMAPINFOHEADER) AVIStreamGetFrame (
        VideoPntr->videoGetFrameFunction,
        VideoPntr->currentVideoFrameNumber);
      if (BitmapInfoPntr == NULL)
        goto ErrorExit; // No image available for current frame.

      BitmapBitsPntr = ((LPBYTE) BitmapInfoPntr) + BitmapInfoPntr->biSize;
#else // Newer video players write into our own allocated intermediate bitmap.
      memcpy (&BitmapInfo, &VideoPntr->bitmapInfo, sizeof (BitmapInfo));
      BitmapBitsPntr =
        LE_GRAFIX_LockObjectDataPtr (VideoPntr->intermediateBitmapID);
      if (BitmapBitsPntr == NULL)
        goto ErrorExit;
#endif
    }

    BitmapDepth = (UNS8) BitmapInfoPntr->biBitCount;
    BitmapRect.right = BitmapWidth = (UNS16) BitmapInfoPntr->biWidth;
    BitmapBytesPerScanLine = ((BitmapWidth * (BitmapDepth / 8) + 3) & 0xFFFFFFFC);
    BitmapRect.bottom = BitmapHeight = (UNS16) abs (BitmapInfoPntr->biHeight);
    BitmapRect.top = BitmapRect.left = 0;
    BitmapOffsetX = BitmapOffsetY = 0;
    BitmapAlphaCount = SequencePntr->streamSpecificData.videoStream->alphaLevel;
    if (SequencePntr->streamSpecificData.videoStream->drawSolid)
      BitmapDrawMode = DRAW_SOLID;
    else if (BitmapAlphaCount == ALPHA_OPAQUE100_0)
      BitmapDrawMode = DRAW_COLOUR_KEY;
    else if (BitmapAlphaCount == ALPHA_TRANSPARENT)
      BitmapDrawMode = DRAW_INVISIBLE; // So transparent that you can't see it.
    else
      BitmapDrawMode = DRAW_ALPHA;
  }
  else // Something else (like a group or camera).
  {
    // Invisible except for bounding box.
    BitmapDrawMode = DRAW_INVISIBLE;
  }

  // Determine if scaling is going on.  Or maybe rotation,
  // but that's another story for another day.  Looks like
  // it is today (May 1999) since FredDS has put together
  // some texture warping code which does rotation too as
  // a side effect.  Also check for the bitmap rotated to be
  // exactly upside down (180 degrees or square ones rotated
  // by 90 degrees) and count that as scaling too so that the
  // rotation code gets called.

  Scaling = (BoundingRectPntr->right - BoundingRectPntr->left != BitmapWidth ||
    BoundingRectPntr->bottom - BoundingRectPntr->top != BitmapHeight ||
    PrivateDataPntr->boundingBox.named.topLeft.y >= PrivateDataPntr->boundingBox.named.bottomLeft.y);

  if (Scaling)
  {
    // Could be a rotation rather than a simple scale in X and Y axis.
    // See if the sides are vertical and horizontal and the top is at
    // the top, and bottom at the bottom.

    Rotating = (
      PrivateDataPntr->boundingBox.named.topLeft.y != PrivateDataPntr->boundingBox.named.topRight.y ||
      PrivateDataPntr->boundingBox.named.bottomLeft.y != PrivateDataPntr->boundingBox.named.bottomRight.y ||
      PrivateDataPntr->boundingBox.named.topLeft.x != PrivateDataPntr->boundingBox.named.bottomLeft.x ||
      PrivateDataPntr->boundingBox.named.topRight.x != PrivateDataPntr->boundingBox.named.bottomRight.x ||
      PrivateDataPntr->boundingBox.named.topLeft.y > PrivateDataPntr->boundingBox.named.bottomLeft.y ||
      PrivateDataPntr->boundingBox.named.topRight.y > PrivateDataPntr->boundingBox.named.bottomRight.y ||
      PrivateDataPntr->boundingBox.named.topLeft.x > PrivateDataPntr->boundingBox.named.topRight.x ||
      PrivateDataPntr->boundingBox.named.bottomLeft.x > PrivateDataPntr->boundingBox.named.bottomRight.x);

  }
  else
    Rotating = FALSE;

  // Turn on the box outline if requested.  This gets the
  // bitfield variable out of the structure and into a faster
  // access variable on the stack.

  DrawSelectionBox = SequencePntr->drawSelectionBox;

  // Go through all the invalid rectangles that intersect with the
  // bitmap's bounding area and redraw those parts on the working
  // surface bitmap.

  ErrorCode = GetRegionData (RedrawRegion, LI_REND_REGION_RECTANGLE_BUFFER_SPACE,
    &RegionData.header);

  if (ErrorCode == 0 || RegionData.header.rdh.iType != RDH_RECTANGLES)
    goto ErrorExit; // Can't convert region to rectangles.

  InvalidRectPntr = (LPRECT) &RegionData.header.Buffer;
  while (RegionData.header.rdh.nCount > 0)
  {
    if (Scaling)
    {
      // Set up the rotation clipping - we pretend we are drawing onto a
      // really small destination bitmap (the size of the invalid rectangle)
      // instead of the whole screen.  That way the rotation clipping will
      // cut down on the amount of work needed for small updates (like the
      // mouse pointer moving over a big rotated bitmap).

      if (Rotating)
      {
        RotateFakeDestBitmapWidth = (UNS16) (InvalidRectPntr->right - InvalidRectPntr->left);
        RotateFakeDestBitmapHeight = (UNS16) (InvalidRectPntr->bottom - InvalidRectPntr->top);
        RotateFakeDestBitmapBitsPntr = (LPBYTE) SlotPntr->workingBitsPntr;
        RotateFakeDestBitmapBitsPntr +=
          InvalidRectPntr->left * (SlotPntr->workingDepth / 8) +
          InvalidRectPntr->top * SlotPntr->workingBytesToNextScanLine;

        RotateFakeCorners = *PrivateDataPntr;
        for (i = 0; i < 4; i++)
        {
          RotateFakeCorners.boundingBox.asArray[i].x -= InvalidRectPntr->left;
          RotateFakeCorners.boundingBox.asArray[i].y -= InvalidRectPntr->top;
        }
      }
    }
    else // Not scaling or rotating, just a 1:1 draw.
    {
      // Need a clipped bitmap relative source rectangle when not scaling.
      SourceRect.left = InvalidRectPntr->left - BoundingRectPntr->left;
      SourceRect.right = InvalidRectPntr->right - BoundingRectPntr->left;
      SourceRect.top = InvalidRectPntr->top - BoundingRectPntr->top;
      SourceRect.bottom = InvalidRectPntr->bottom - BoundingRectPntr->top;
    }

    // So we now have the full size screen coordinates destination rectangle
    // in BoundingRectPntr and one of several clipped destination rectangles
    // in InvalidRectPntr.  SourceRect has the clipped source rectangle
    // in those non-scaling cases only.

    switch (BitmapDepth)
    {
    case 8: // Source bitmap 8 bits deep.
      switch (SlotPntr->workingDepth)
      {
      case 16: // Source bitmap 8 and destination 16 bits deep.
        switch (BitmapDrawMode)
        {
        case DRAW_SOLID:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling)
            LI_BLT16StretchBlt8to16 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, BoundingRectPntr,
            InvalidRectPntr, BitmapBitsPntr, BitmapWidth, &BitmapRect,
            (LPLONG) BitmapColourTablePntr);
          else
            LI_BLT16BitBlt8to16 ((BYTE *) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect,
            (LPLONG) BitmapColourTablePntr);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_8to16 (
                BitmapBitsPntr, BitmapBytesPerScanLine,
                BitmapWidth, BitmapHeight,
                (LE_BLT_AlphaPaletteEntryPointer) BitmapColourTablePntr,
                BitmapSolidColourCount, BitmapAlphaCount,
                RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
                RotateFakeDestBitmapWidth,
                RotateFakeDestBitmapHeight,
                RotateFakeCorners.boundingBox.named.topLeft.x,
                RotateFakeCorners.boundingBox.named.topLeft.y,
                RotateFakeCorners.boundingBox.named.bottomLeft.x,
                RotateFakeCorners.boundingBox.named.bottomLeft.y,
                RotateFakeCorners.boundingBox.named.bottomRight.x,
                RotateFakeCorners.boundingBox.named.bottomRight.y,
                RotateFakeCorners.boundingBox.named.topRight.x,
                RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy8To16SolidStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (LPLONG) BitmapColourTablePntr,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
          }
          else
            LI_BLT_Copy8To16SolidUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              (LPLONG) BitmapColourTablePntr,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;

        case DRAW_ALPHA:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling)
            LI_BLT16AlphaStretchBlt8to16 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, BoundingRectPntr,
            InvalidRectPntr, BitmapBitsPntr, BitmapWidth, &BitmapRect,
            (LPLONG) BitmapColourTablePntr, BitmapAlphaCount);
          else
            LI_BLT16AlphaBitBlt8to16 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect,
            (LPLONG) BitmapColourTablePntr, BitmapAlphaCount);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_8to16 (
                BitmapBitsPntr, BitmapBytesPerScanLine,
                BitmapWidth, BitmapHeight,
                (LE_BLT_AlphaPaletteEntryPointer) BitmapColourTablePntr,
                BitmapSolidColourCount, BitmapAlphaCount,
                RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
                RotateFakeDestBitmapWidth,
                RotateFakeDestBitmapHeight,
                RotateFakeCorners.boundingBox.named.topLeft.x,
                RotateFakeCorners.boundingBox.named.topLeft.y,
                RotateFakeCorners.boundingBox.named.bottomLeft.x,
                RotateFakeCorners.boundingBox.named.bottomLeft.y,
                RotateFakeCorners.boundingBox.named.bottomRight.x,
                RotateFakeCorners.boundingBox.named.bottomRight.y,
                RotateFakeCorners.boundingBox.named.topRight.x,
                RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy8To16AlphaStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (LPLONG) BitmapColourTablePntr,
                BitmapAlphaCount,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
          }
          else
            LI_BLT_Copy8To16AlphaUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              (LPLONG) BitmapColourTablePntr,
              BitmapAlphaCount,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;

        case DRAW_COLOUR_KEY:  // actually means Holes
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling)
            LI_BLT16ColorKeyStretchBlt8to16 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, BoundingRectPntr,
            InvalidRectPntr, BitmapBitsPntr, BitmapWidth, &BitmapRect,
            (LPLONG) BitmapColourTablePntr);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_8to16 (
                BitmapBitsPntr, BitmapBytesPerScanLine,
                BitmapWidth, BitmapHeight,
                (LE_BLT_AlphaPaletteEntryPointer) BitmapColourTablePntr,
                BitmapSolidColourCount, BitmapAlphaCount,
                RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
                RotateFakeDestBitmapWidth,
                RotateFakeDestBitmapHeight,
                RotateFakeCorners.boundingBox.named.topLeft.x,
                RotateFakeCorners.boundingBox.named.topLeft.y,
                RotateFakeCorners.boundingBox.named.bottomLeft.x,
                RotateFakeCorners.boundingBox.named.bottomLeft.y,
                RotateFakeCorners.boundingBox.named.bottomRight.x,
                RotateFakeCorners.boundingBox.named.bottomRight.y,
                RotateFakeCorners.boundingBox.named.topRight.x,
                RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy8To16HolesStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (LPLONG) BitmapColourTablePntr,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
          }
#endif
          else
            LI_BLT16ColorKeyBitBlt8to16 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect,
            (LPLONG) BitmapColourTablePntr);
          break;
        }
        break;


      case 24: // Source bitmap 8 and destination 24 bits deep.
        switch (BitmapDrawMode)
        {
        case DRAW_SOLID:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling)
            LI_BLT24StretchBlt8to24 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, BoundingRectPntr,
            InvalidRectPntr, BitmapBitsPntr, BitmapWidth, &BitmapRect,
            (LPLONG) BitmapColourTablePntr);
          else
            LI_BLT24BitBlt8to24 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect,
            (LPLONG) BitmapColourTablePntr);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_8to24 (
                BitmapBitsPntr, BitmapBytesPerScanLine,
                BitmapWidth, BitmapHeight,
                (LE_BLT_AlphaPaletteEntryPointer) BitmapColourTablePntr,
                BitmapSolidColourCount, BitmapAlphaCount,
                RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
                RotateFakeDestBitmapWidth,
                RotateFakeDestBitmapHeight,
                RotateFakeCorners.boundingBox.named.topLeft.x,
                RotateFakeCorners.boundingBox.named.topLeft.y,
                RotateFakeCorners.boundingBox.named.bottomLeft.x,
                RotateFakeCorners.boundingBox.named.bottomLeft.y,
                RotateFakeCorners.boundingBox.named.bottomRight.x,
                RotateFakeCorners.boundingBox.named.bottomRight.y,
                RotateFakeCorners.boundingBox.named.topRight.x,
                RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy8To24SolidStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (LPLONG) BitmapColourTablePntr,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
          }
          else
            LI_BLT_Copy8To24SolidUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              (LPLONG) BitmapColourTablePntr,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;

        case DRAW_ALPHA:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling)
            LI_BLT24AlphaStretchBlt8to24 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, BoundingRectPntr,
            InvalidRectPntr, BitmapBitsPntr, BitmapWidth, &BitmapRect,
            (LPLONG) BitmapColourTablePntr, BitmapAlphaCount);
          else
            LI_BLT24AlphaBitBlt8to24 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect,
            (LPLONG) BitmapColourTablePntr, BitmapAlphaCount);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_8to24 (
                BitmapBitsPntr, BitmapBytesPerScanLine,
                BitmapWidth, BitmapHeight,
                (LE_BLT_AlphaPaletteEntryPointer) BitmapColourTablePntr,
                BitmapSolidColourCount, BitmapAlphaCount,
                RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
                RotateFakeDestBitmapWidth,
                RotateFakeDestBitmapHeight,
                RotateFakeCorners.boundingBox.named.topLeft.x,
                RotateFakeCorners.boundingBox.named.topLeft.y,
                RotateFakeCorners.boundingBox.named.bottomLeft.x,
                RotateFakeCorners.boundingBox.named.bottomLeft.y,
                RotateFakeCorners.boundingBox.named.bottomRight.x,
                RotateFakeCorners.boundingBox.named.bottomRight.y,
                RotateFakeCorners.boundingBox.named.topRight.x,
                RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy8To24AlphaStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (LPLONG) BitmapColourTablePntr,
                BitmapAlphaCount,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
          }
          else
            LI_BLT_Copy8To24AlphaUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              (LPLONG) BitmapColourTablePntr,
              BitmapAlphaCount,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;

        case DRAW_COLOUR_KEY:  // actually means Holes
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling)
            LI_BLT24ColorKeyStretchBlt8to24 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, BoundingRectPntr,
            InvalidRectPntr, BitmapBitsPntr, BitmapWidth, &BitmapRect,
            (LPLONG) BitmapColourTablePntr);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_8to24 (
                BitmapBitsPntr, BitmapBytesPerScanLine,
                BitmapWidth, BitmapHeight,
                (LE_BLT_AlphaPaletteEntryPointer) BitmapColourTablePntr,
                BitmapSolidColourCount, BitmapAlphaCount,
                RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
                RotateFakeDestBitmapWidth,
                RotateFakeDestBitmapHeight,
                RotateFakeCorners.boundingBox.named.topLeft.x,
                RotateFakeCorners.boundingBox.named.topLeft.y,
                RotateFakeCorners.boundingBox.named.bottomLeft.x,
                RotateFakeCorners.boundingBox.named.bottomLeft.y,
                RotateFakeCorners.boundingBox.named.bottomRight.x,
                RotateFakeCorners.boundingBox.named.bottomRight.y,
                RotateFakeCorners.boundingBox.named.topRight.x,
                RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy8To24HolesStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (LPLONG) BitmapColourTablePntr,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
          }
#endif
          else
            LI_BLT24ColorKeyBitBlt8to24 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect,
            (LPLONG) BitmapColourTablePntr);
          break;
        }
        break;


      case 32: // Source bitmap 8 and destination 32 bits deep.
        switch (BitmapDrawMode)
        {
        case DRAW_SOLID:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling)
            LI_BLT32StretchBlt8to32 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, BoundingRectPntr,
            InvalidRectPntr, BitmapBitsPntr, BitmapWidth, &BitmapRect,
            (LPLONG) BitmapColourTablePntr);
          else
            LI_BLT32BitBlt8to32 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect,
            (LPLONG) BitmapColourTablePntr);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_8to32 (
                BitmapBitsPntr, BitmapBytesPerScanLine,
                BitmapWidth, BitmapHeight,
                (LE_BLT_AlphaPaletteEntryPointer) BitmapColourTablePntr,
                BitmapSolidColourCount, BitmapAlphaCount,
                RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
                RotateFakeDestBitmapWidth,
                RotateFakeDestBitmapHeight,
                RotateFakeCorners.boundingBox.named.topLeft.x,
                RotateFakeCorners.boundingBox.named.topLeft.y,
                RotateFakeCorners.boundingBox.named.bottomLeft.x,
                RotateFakeCorners.boundingBox.named.bottomLeft.y,
                RotateFakeCorners.boundingBox.named.bottomRight.x,
                RotateFakeCorners.boundingBox.named.bottomRight.y,
                RotateFakeCorners.boundingBox.named.topRight.x,
                RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy8To32SolidStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (LPLONG) BitmapColourTablePntr,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
          }
          else
            LI_BLT_Copy8To32SolidUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              (LPLONG) BitmapColourTablePntr,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;

        case DRAW_ALPHA:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling)
            LI_BLT32AlphaStretchBlt8to32 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, BoundingRectPntr,
            InvalidRectPntr, BitmapBitsPntr, BitmapWidth, &BitmapRect,
            (LPLONG) BitmapColourTablePntr, BitmapAlphaCount);
          else
            LI_BLT32AlphaBitBlt8to32 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect,
            (LPLONG) BitmapColourTablePntr, BitmapAlphaCount);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_8to32 (
                BitmapBitsPntr, BitmapBytesPerScanLine,
                BitmapWidth, BitmapHeight,
                (LE_BLT_AlphaPaletteEntryPointer) BitmapColourTablePntr,
                BitmapSolidColourCount, BitmapAlphaCount,
                RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
                RotateFakeDestBitmapWidth,
                RotateFakeDestBitmapHeight,
                RotateFakeCorners.boundingBox.named.topLeft.x,
                RotateFakeCorners.boundingBox.named.topLeft.y,
                RotateFakeCorners.boundingBox.named.bottomLeft.x,
                RotateFakeCorners.boundingBox.named.bottomLeft.y,
                RotateFakeCorners.boundingBox.named.bottomRight.x,
                RotateFakeCorners.boundingBox.named.bottomRight.y,
                RotateFakeCorners.boundingBox.named.topRight.x,
                RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy8To32AlphaStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (LPLONG) BitmapColourTablePntr,
                BitmapAlphaCount,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
          }
          else
            LI_BLT_Copy8To32AlphaUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              (LPLONG) BitmapColourTablePntr,
              BitmapAlphaCount,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;

        case DRAW_COLOUR_KEY:  // actually means Holes
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling)
            LI_BLT32ColorKeyStretchBlt8to32 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, BoundingRectPntr,
            InvalidRectPntr, BitmapBitsPntr, BitmapWidth, &BitmapRect,
            (LPLONG) BitmapColourTablePntr);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_8to32 (
                BitmapBitsPntr, BitmapBytesPerScanLine,
                BitmapWidth, BitmapHeight,
                (LE_BLT_AlphaPaletteEntryPointer) BitmapColourTablePntr,
                BitmapSolidColourCount, BitmapAlphaCount,
                RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
                RotateFakeDestBitmapWidth,
                RotateFakeDestBitmapHeight,
                RotateFakeCorners.boundingBox.named.topLeft.x,
                RotateFakeCorners.boundingBox.named.topLeft.y,
                RotateFakeCorners.boundingBox.named.bottomLeft.x,
                RotateFakeCorners.boundingBox.named.bottomLeft.y,
                RotateFakeCorners.boundingBox.named.bottomRight.x,
                RotateFakeCorners.boundingBox.named.bottomRight.y,
                RotateFakeCorners.boundingBox.named.topRight.x,
                RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy8To32HolesStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (LPLONG) BitmapColourTablePntr,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
          }
#endif
          else
            LI_BLT32ColorKeyBitBlt8to32 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect,
            (LPLONG) BitmapColourTablePntr);
          break;
        }
        break;
      }
      break;



    case 16: // Source bitmap 16 bits deep.
      switch (SlotPntr->workingDepth)
      {
      case 16: // Source bitmap 16 and destination 16 bits deep.
        switch (BitmapDrawMode)
        {
        case DRAW_SOLID:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling) // Not implemented so fill with colour.
            LI_BLT16ColorBlt ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr, SequencePntr->contentsDataID, FALSE);
          else
            LI_BLT16BitBlt16to16 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr->left, InvalidRectPntr->top,
            BitmapBitsPntr, BitmapWidth, &SourceRect);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_16to16 (
              BitmapBitsPntr, BitmapBytesPerScanLine,
              BitmapWidth, BitmapHeight,
              RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
              RotateFakeDestBitmapWidth,
              RotateFakeDestBitmapHeight,
              RotateFakeCorners.boundingBox.named.topLeft.x,
              RotateFakeCorners.boundingBox.named.topLeft.y,
              RotateFakeCorners.boundingBox.named.bottomLeft.x,
              RotateFakeCorners.boundingBox.named.bottomLeft.y,
              RotateFakeCorners.boundingBox.named.bottomRight.x,
              RotateFakeCorners.boundingBox.named.bottomRight.y,
              RotateFakeCorners.boundingBox.named.topRight.x,
              RotateFakeCorners.boundingBox.named.topRight.y);
           else
              LI_BLT_Copy16To16SolidStretchRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr);
          }
          else
            LI_BLT_Copy16To16SolidUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;

        case DRAW_ALPHA:
          if (Scaling) // Not implemented so fill with colour.
            #if CE_ARTLIB_BlitUseOldRoutines
              LI_BLT_Fill16Raw (SequencePntr->contentsDataID, FALSE,
              (LPBYTE) SlotPntr->workingBitsPntr,
              SlotPntr->workingPixelsToNextScanLine,
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr);
            #else
              LI_BLT_Copy16To16AlphaStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (INT16) BitmapAlphaCount,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
            #endif
          else
            LI_BLT16AlphaBitBlt16to16 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr->left, InvalidRectPntr->top,
            BitmapBitsPntr, BitmapWidth, &SourceRect, BitmapAlphaCount);
          break;

        case DRAW_COLOUR_KEY:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling) // Not implemented so fill with colour.
            LI_BLT16ColorBlt ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr, SequencePntr->contentsDataID, FALSE);
          else
            LI_BLT16ColorKeyPureGreenBitBlt16to16 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect);
#else
          if (Scaling)
            LI_BLT_Copy16To16SolidColourKeyStretchRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr);
          else
            LI_BLT_Copy16To16SolidColourKeyUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;
        }
        break;


      case 24: // Source bitmap 16 and destination 24 bits deep.
        // Not implemented so fill with colour.
        LI_BLT_Fill24Raw (SequencePntr->contentsDataID, FALSE,
        (LPBYTE) SlotPntr->workingBitsPntr,
        SlotPntr->workingPixelsToNextScanLine,
        (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
        InvalidRectPntr);
        break;


      case 32: // Source bitmap 16 and destination 32 bits deep.
        // Not implemented so fill with colour.
        LI_BLT_Fill32Raw (SequencePntr->contentsDataID, FALSE,
        (LPBYTE) SlotPntr->workingBitsPntr,
        SlotPntr->workingPixelsToNextScanLine,
        (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
        InvalidRectPntr);
        break;
      }
      break;



    case 24: // Source bitmap 24 bits deep.
      switch (SlotPntr->workingDepth)
      {
      case 16: // Source bitmap 24 and destination 16 bits deep.
        // Not implemented so fill with colour.
        LI_BLT_Fill16Raw (SequencePntr->contentsDataID, FALSE,
        (LPBYTE) SlotPntr->workingBitsPntr,
        SlotPntr->workingPixelsToNextScanLine,
        (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
        InvalidRectPntr);
        break;


      case 24: // Source bitmap 24 and destination 24 bits deep.
        switch (BitmapDrawMode)
        {
        case DRAW_SOLID:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling) // Not implemented so fill with colour.
            LI_BLT24ColorBlt ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr, SequencePntr->contentsDataID, FALSE);
          else
            LI_BLT24BitBlt24to24 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr->left, InvalidRectPntr->top,
            BitmapBitsPntr, BitmapWidth, &SourceRect);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_24to24 (
              BitmapBitsPntr, BitmapBytesPerScanLine,
              BitmapWidth, BitmapHeight,
              RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
              RotateFakeDestBitmapWidth,
              RotateFakeDestBitmapHeight,
              RotateFakeCorners.boundingBox.named.topLeft.x,
              RotateFakeCorners.boundingBox.named.topLeft.y,
              RotateFakeCorners.boundingBox.named.bottomLeft.x,
              RotateFakeCorners.boundingBox.named.bottomLeft.y,
              RotateFakeCorners.boundingBox.named.bottomRight.x,
              RotateFakeCorners.boundingBox.named.bottomRight.y,
              RotateFakeCorners.boundingBox.named.topRight.x,
              RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy24To24SolidStretchRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr);
          }
          else
            LI_BLT_Copy24To24SolidUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;

        case DRAW_ALPHA:
          if (Scaling) // Not implemented so fill with colour.
            #if CE_ARTLIB_BlitUseOldRoutines
              LI_BLT_Fill24Raw (SequencePntr->contentsDataID, FALSE,
              (LPBYTE) SlotPntr->workingBitsPntr,
              SlotPntr->workingPixelsToNextScanLine,
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr);
            #else
              LI_BLT_Copy24To24AlphaStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (INT16) BitmapAlphaCount,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
            #endif
          else
            LI_BLT24AlphaBitBlt24to24 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr->left, InvalidRectPntr->top,
            BitmapBitsPntr, BitmapWidth, &SourceRect, BitmapAlphaCount);
          break;

        case DRAW_COLOUR_KEY:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling) // Not implemented so fill with colour.
            LI_BLT24ColorBlt ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr, SequencePntr->contentsDataID, FALSE);
          else
            LI_BLT24ColorKeyPureGreenBitBlt24to24 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect);
#else
          if (Scaling)
            LI_BLT_Copy24To24SolidColourKeyStretchRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr);
          else
            LI_BLT_Copy24To24SolidColourKeyUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;
        }
        break;


      case 32: // Source bitmap 24 and destination 32 bits deep.
        // Not implemented so fill with colour.
        LI_BLT_Fill32Raw (SequencePntr->contentsDataID, FALSE,
        (LPBYTE) SlotPntr->workingBitsPntr,
        SlotPntr->workingPixelsToNextScanLine,
        (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
        InvalidRectPntr);
        break;
      }
      break;



    case 32: // Source bitmap 32 bits deep.
      switch (SlotPntr->workingDepth)
      {
      case 16: // Source bitmap 32 and destination 16 bits deep.
        // Not implemented so fill with colour.
        LI_BLT_Fill16Raw (SequencePntr->contentsDataID, FALSE,
        (LPBYTE) SlotPntr->workingBitsPntr,
        SlotPntr->workingPixelsToNextScanLine,
        (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
        InvalidRectPntr);
        break;


      case 24: // Source bitmap 32 and destination 24 bits deep.
        // Not implemented so fill with colour.
        LI_BLT_Fill24Raw (SequencePntr->contentsDataID, FALSE,
        (LPBYTE) SlotPntr->workingBitsPntr,
        SlotPntr->workingPixelsToNextScanLine,
        (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
        InvalidRectPntr);
        break;


      case 32: // Source bitmap 32 and destination 32 bits deep.
        switch (BitmapDrawMode)
        {
        case DRAW_SOLID:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling) // Not implemented so fill with colour.
            LI_BLT32ColorBlt ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr, SequencePntr->contentsDataID, FALSE);
          else
            LI_BLT32BitBlt32to32 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr->left, InvalidRectPntr->top,
            BitmapBitsPntr, BitmapWidth, &SourceRect);
#else
          if (Scaling)
          {
            if (Rotating)
              LE_BLTWARP_32to32 (
              BitmapBitsPntr, BitmapBytesPerScanLine,
              BitmapWidth, BitmapHeight,
              RotateFakeDestBitmapBitsPntr, SlotPntr->workingBytesToNextScanLine,
              RotateFakeDestBitmapWidth,
              RotateFakeDestBitmapHeight,
              RotateFakeCorners.boundingBox.named.topLeft.x,
              RotateFakeCorners.boundingBox.named.topLeft.y,
              RotateFakeCorners.boundingBox.named.bottomLeft.x,
              RotateFakeCorners.boundingBox.named.bottomLeft.y,
              RotateFakeCorners.boundingBox.named.bottomRight.x,
              RotateFakeCorners.boundingBox.named.bottomRight.y,
              RotateFakeCorners.boundingBox.named.topRight.x,
              RotateFakeCorners.boundingBox.named.topRight.y);
            else
              LI_BLT_Copy32To32SolidStretchRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr);
          }
          else
            LI_BLT_Copy32To32SolidUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;

        case DRAW_ALPHA:
          if (Scaling) // Not implemented so fill with colour.
            #if CE_ARTLIB_BlitUseOldRoutines
              LI_BLT_Fill32Raw (SequencePntr->contentsDataID, FALSE,
              (LPBYTE) SlotPntr->workingBitsPntr,
              SlotPntr->workingPixelsToNextScanLine,
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr);
            #else
              LI_BLT_Copy32To32AlphaStretchRaw (
                BitmapBitsPntr,
                BitmapWidth,
                BitmapHeight,
                (INT16) BitmapAlphaCount,
                &BitmapRect,
                (LPBYTE) SlotPntr->workingBitsPntr,
                (UNS16) (SlotPntr->workingPixelsToNextScanLine),
                (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
                InvalidRectPntr,
                BoundingRectPntr);
            #endif
          else
            LI_BLT32AlphaBitBlt32to32 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr->left, InvalidRectPntr->top,
            BitmapBitsPntr, BitmapWidth, &SourceRect, BitmapAlphaCount);
          break;

        case DRAW_COLOUR_KEY:
#if CE_ARTLIB_BlitUseOldRoutines
          if (Scaling) // Not implemented so fill with colour.
            LI_BLT32ColorBlt ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine,
            InvalidRectPntr, SequencePntr->contentsDataID, FALSE);
          else
            LI_BLT32ColorKeyPureGreenBitBlt32to32 ((LPBYTE) SlotPntr->workingBitsPntr,
            SlotPntr->workingPixelsToNextScanLine, InvalidRectPntr->left,
            InvalidRectPntr->top, BitmapBitsPntr, BitmapWidth, &SourceRect);
#else
          if (Scaling)
            LI_BLT_Copy32To32SolidColourKeyStretchRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr);
          else
            LI_BLT_Copy32To32SolidColourKeyUnityRaw (
              BitmapBitsPntr,
              BitmapWidth,
              BitmapHeight,
              &BitmapRect,
              (LPBYTE) SlotPntr->workingBitsPntr,
              (UNS16) (SlotPntr->workingPixelsToNextScanLine),
              (UNS16) LE_GRAFIX_VirtualScreenHeightInPixels,
              InvalidRectPntr,
              BoundingRectPntr,
              (INT16) InvalidRectPntr->left,
              (INT16) InvalidRectPntr->top);
#endif
          break;
        }
        break;
      }
      break;
    }

    // If this is a selected object, draw the box around it, but clip
    // the box too.

    if (DrawSelectionBox)
    {
      for (BoxSide = 3; BoxSide >= 0; BoxSide--)
      {
        SelectionBoxRectangle = *BoundingRectPntr;
        if (BoxSide == 0) // Top
          SelectionBoxRectangle.bottom = SelectionBoxRectangle.top + 1;
        else if (BoxSide == 1) // Right
          SelectionBoxRectangle.left = SelectionBoxRectangle.right - 1;
        else if (BoxSide == 2) // Bottom
          SelectionBoxRectangle.top = SelectionBoxRectangle.bottom - 1;
        else // Left
          SelectionBoxRectangle.right = SelectionBoxRectangle.left + 1;

        if (IntersectRect (&SelectionBoxRectangle, &SelectionBoxRectangle,
        InvalidRectPntr))
        {
          switch (SlotPntr->workingDepth)
          {
          case 16:
            LI_BLT_Fill16Raw ((DWORD) -1 /* White */, TRUE /* Native colour */,
              (BYTE *) SlotPntr->workingBitsPntr,
              SlotPntr->workingPixelsToNextScanLine,
              LE_GRAFIX_VirtualScreenHeightInPixels,
              &SelectionBoxRectangle);
            break;

          case 24:
            LI_BLT_Fill24Raw ((DWORD) -1 /* White */, TRUE /* Native colour */,
              (BYTE *) SlotPntr->workingBitsPntr,
              SlotPntr->workingPixelsToNextScanLine,
              LE_GRAFIX_VirtualScreenHeightInPixels,
              &SelectionBoxRectangle);
            break;

          case 32:
            LI_BLT_Fill32Raw ((DWORD) -1 /* White */, TRUE /* Native colour */,
              (BYTE *) SlotPntr->workingBitsPntr,
              SlotPntr->workingPixelsToNextScanLine,
              LE_GRAFIX_VirtualScreenHeightInPixels,
              &SelectionBoxRectangle);
            break;
          }
        }
      }
    }

    // On to the next rectangle.

    InvalidRectPntr++;
    RegionData.header.rdh.nCount--;
  }

ErrorExit:
NormalExit:
  if (BitmapGBMPntr != NULL)
    LE_GRAFIX_UnlockGBM (SequencePntr->contentsDataID);
}



/*****************************************************************************
 * Draw all 2D bitmaps in the sequence tree that intersect the invalid
 * area and are the responsibility of our render slot.  Only draws them
 * into the work area and only to the work area portion marked invalid.
 * Also updates 2D sounds and videos, feeding them valuable info.
 * Returns TRUE if successful, FALSE if unable to draw (try again later).
 */

static BOOL LI_REND2D_DrawAllSequencesToWorkArea (
LE_SEQNCR_RuntimeInfoPointer RootPntr, LI_REND2D_SlotDataPointer SlotPntr)
{
  LE_SOUND_AudioStreamPointer   AudioPntr;
  TYPE_Rect                     BoundingRect;
  HRESULT                       ErrorCode;
  BOOL                          InOurSlot;
  RECT                          InvalidAreaRect;
  PrivateSequenceDataPointer    PrivateDataPntr;
  LE_REND_RenderSlotSet         RenderMask;
  BOOL                          ReturnCode = FALSE;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;
  DDSURFDESC                    SurfaceDescriptor;
  HRGN                          TempRegion = NULL;

  RenderMask = SlotPntr->mySlotSetMask;
  SequencePntr = RootPntr;

  if (SlotPntr->invalidArea == NULL)
    return TRUE; // Nothing to do.

#if CE_ARTLIB_EnableMultitasking
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return FALSE; // System is closed.
  EnterCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
  if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return FALSE; // Closed while we waited.
#endif

  // Lock down the working surface.  From this point on, there is a good
  // chance that any error messages will lock up Windows, particularly
  // if the working area is the same as the screen bitmap.

  memset (&SurfaceDescriptor, 0, sizeof (SurfaceDescriptor));
  SurfaceDescriptor.dwSize = sizeof (SurfaceDescriptor);

  ErrorCode = SlotPntr->workingSurface->Lock (
    NULL /* Ask for whole surface */, &SurfaceDescriptor,
    DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_NOSYSLOCK, NULL);

  if (ErrorCode != DD_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND2D_DrawAllSequencesToWorkArea: "
    "Unable to lock working area.\r\n", LE_ERROR_DebugMsgToFile);
#endif
    goto ErrorExit;
  }

  SlotPntr->workingBitsPntr = SurfaceDescriptor.lpSurface;
  SlotPntr->workingDepth = (UNS16) SurfaceDescriptor.ddpfPixelFormat.dwRGBBitCount;
  SlotPntr->workingPixelsToNextScanLine =
    (UNS16) (SurfaceDescriptor.lPitch / (SlotPntr->workingDepth / 8));
  SlotPntr->workingBytesToNextScanLine = (UNS16) SurfaceDescriptor.lPitch;

  // Let the blitter bounds checker know what range of addresses
  // correspond to the screen, so that it recognizes the target.

  LI_BLT_BoundCheckScreenStart = (LPBYTE) SlotPntr->workingBitsPntr;
  LI_BLT_BoundCheckScreenPastEnd = LI_BLT_BoundCheckScreenStart +
    (SurfaceDescriptor.lPitch * SurfaceDescriptor.dwHeight);

  // Just create a junk region initially.

  TempRegion = CreateRectRgnIndirect (&SlotPntr->cameraRenderArea);
  if (TempRegion == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND2D_DrawAllSequencesToWorkArea: "
    "Unable to create region.\r\n", LE_ERROR_DebugMsgToFile);
#endif
    goto ErrorExit;
  }

  // Find a crude clipping rectangle for the whole invalid area, in
  // an attempt to speed up skipping past items outside the
  // redrawn space (avoid a system call).

  GetRgnBox (SlotPntr->invalidArea, &InvalidAreaRect);

  // Iterate through the sequence tree.

  while (TRUE)
  {
    InOurSlot = ((SequencePntr->renderSlotSet & RenderMask) != 0);

    if (InOurSlot)
    {
      if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND)
      {
        AudioPntr = SequencePntr->streamSpecificData.audioStream;

        if (!LE_SOUND_FeedAudioStream (AudioPntr,
        SequencePntr->endingAction == LE_SEQNCR_EndingActionLoopToBeginning))
        {
          // Error happened while feeding, kill it.
          SequencePntr->endingAction = LE_SEQNCR_EndingActionSuicide;
        }
      }

      // Process the current node.  Only draw ones which have bitmaps.
      // Ignore grouping chunks and cameras and sounds.  Well, except
      // for ones which are selected so that they can be manipulated
      // in the dope sheet (just draw their bounding box).

      if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_2DBITMAP ||
      SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO ||
      SequencePntr->drawSelectionBox)
      {
        PrivateDataPntr = (PrivateSequenceDataPointer)
          SequencePntr->renderSlotData[SlotPntr->mySlotNumber];
  #if CE_ARTLIB_EnableDebugMode
        if (PrivateDataPntr == NULL)
          LE_ERROR_DebugMsg("LI_REND2D_DrawAllSequencesToWorkArea: "
                            "Sequence is missing its private render slot data.\r\n",
                            LE_ERROR_DebugMsgToFile);
  #endif

        CalculateEnclosingScreenRect (PrivateDataPntr, &BoundingRect);

        // Do a rough rectangle intersection test (without calling
        // the OS routines) before doing more detailed (and slower)
        // tests, so that we can handle hundreds of bitmaps on the
        // screen simultaneously.

        if (InvalidAreaRect.right > BoundingRect.left &&
        InvalidAreaRect.left < BoundingRect.right &&
        InvalidAreaRect.top < BoundingRect.bottom &&
        InvalidAreaRect.bottom > BoundingRect.top &&
        RectInRegion (SlotPntr->invalidArea, &BoundingRect))
        {
          // It is on screen and at least part of it needs redrawing.
          // Find out how much actually needs to be redrawn by intersecting
          // the bounding box with the invalid area.

          SetRectRgn (TempRegion, BoundingRect.left, BoundingRect.top,
            BoundingRect.right, BoundingRect.bottom);

          CombineRgn (TempRegion, TempRegion, SlotPntr->invalidArea, RGN_AND);

          // Now draw the part that needs redrawing into the work area,
          // but not yet onto the screen.

          DrawBitmapToWorkArea (SequencePntr, SlotPntr, PrivateDataPntr,
            &BoundingRect, TempRegion);
        }
      }
    }

    // Traverse down the tree, then do siblings, then back up to the
    // next parent's sibling.  Ignore nodes not in our slot since
    // their children will definitely not be in our slot.  However,
    // still do children even if we didn't redraw their parent since
    // the parent may be a zero dimensional group with no bounding box.

    if (SequencePntr->child != NULL && InOurSlot)
      SequencePntr = SequencePntr->child;
    else
    {
      while (SequencePntr != NULL && SequencePntr->sibling == NULL)
        SequencePntr = SequencePntr->parent;
      if (SequencePntr == NULL)
        break; // All done.
      SequencePntr = SequencePntr->sibling;
    }
  }

  ReturnCode = TRUE; // Success!

ErrorExit:
  if (SlotPntr->workingBitsPntr != NULL)
  {
    ErrorCode = SlotPntr->workingSurface->Unlock (NULL/*
      SlotPntr->workingBitsPntr*/);
    SlotPntr->workingBitsPntr = NULL;
  }

  if (TempRegion != NULL)
    DeleteObject (TempRegion);

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Copy the changed portions from the working surface to the output surface,
 * if it isn't the same as the working one.  The invalid area region
 * identifies the parts that need copying.
 */

static void CopyWorkAreaToOutputArea (LI_REND2D_SlotDataPointer SlotPntr)
{
  if (SlotPntr->outputSurface == SlotPntr->workingSurface)
    return; // Nothing to do, already on output surface.

  if (SlotPntr->invalidArea == NULL)
    return; // Nothing needs updating.

  LE_GRAFIX_CopyWorkAreaToOutputArea (SlotPntr->invalidArea);
}



/*****************************************************************************
 *****************************************************************************
 ** M O S T L Y   S E Q U E N C E R   C A L L B A C K   F U N C T I O N S   **
 *****************************************************************************
 ****************************************************************************/

/*****************************************************************************
 * Called by the sequencer when it starts up a sequence in our render slot.
 * We just allocate an empty screen coordinate bounding rectangle.  If it
 * is a sound effect, we also start it at this point (it gets moved to
 * the correct panning position when the sequence position is updated).
 */

BOOL LI_REND2D_SlotDataClass::SequenceStartUp (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  UNS32                       Pitch;
  PrivateSequenceDataPointer  PrivateData;

  PrivateData = LI_REND2D_AllocPrivateData ();

  if (PrivateData == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND2D_SequenceStart: Unable to allocate "
      "private data (screen bounding box storage space).\r\n",
    LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Out of memory.
  }

#if CE_ARTLIB_EnableDebugMode
  if (SequencePntr->renderSlotData[mySlotNumber] != NULL)
    LE_ERROR_DebugMsg ("LI_REND2D_SequenceStart: Slot's private field in sequence record isn't NULL!\r\n",
    LE_ERROR_DebugMsgToFileAndScreen);
#endif

  SequencePntr->renderSlotData[mySlotNumber] = PrivateData;

  // Start the sound playing, if this is a sound sequence.
  // Panning and distance fade volume will be done when the
  // object is moved (automatically moved after it is started).

  if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND)
  {
#if CE_ARTLIB_EnableSystemSound
    // Adjust the sound initial settings.

    LE_SOUND_BufSndHandle BufSndPntr;

    if (SequencePntr->streamSpecificData.audioStream == NULL ||
    (BufSndPntr = SequencePntr->streamSpecificData.audioStream->bufSnd) == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      LE_ERROR_DebugMsg ("LI_REND2D_SequenceStart: "
      "Sound stream not allocated or maybe not initialised!\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return FALSE;
    }

    // Get the pitch setting and use it if it isn't the default of zero.

    Pitch = SequencePntr->aux.audio.pitch;
    if (Pitch != 0)
      LE_SOUND_SetPitchBufSnd (BufSndPntr, Pitch);

    // Don't bother setting volume or panning since they are
    // updated whenever the sequence moves.

#else // Sound system isn't compiled.
    return FALSE;
#endif
  } // End if sound sequence.

  return TRUE;
}



/*****************************************************************************
 * A sequence has finished and is being deleted.  Remove our private data and
 * add an invalid rectangle for the graphic that is vanishing, so that the
 * underlying area gets redrawn.  If it is a sound that is ending, stop the
 * sound from playing.  Well, not any more, the sequencer does that for us.
 */

void LI_REND2D_SlotDataClass::SequenceShutDown (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  PrivateSequenceDataPointer  PrivateData;

  PrivateData = (PrivateSequenceDataPointer)
    SequencePntr->renderSlotData[mySlotNumber];
  if (PrivateData == NULL)
    return;

  if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_2DBITMAP ||
  SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO ||
  SequencePntr->drawSelectionBox)
    InvalidateBoundingBox (PrivateData, this, TRUE /* Related Slots */);

  SequencePntr->renderSlotData[mySlotNumber] = NULL;
  LI_REND2D_FreePrivateData (PrivateData);
}



/*****************************************************************************
 * This function is called by the sequencer when a sequence moves around.
 * It should return TRUE if the sequence is still visible in the render
 * slot.  We mark the old rectangular screen area occupied by a bitmap
 * as needing a redraw and store the new screen rectangle in its private
 * data (the sequencer sets the redraw flag which will later make it draw
 * at the new position).  Of course, it is using the camera position from
 * the previous frame, so the returned on-screen flag may be slightly
 * inaccurate if the camera moves.
 */

BOOL LI_REND2D_SlotDataClass::SequenceMoved (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  TYPE_Coord2D                    CenterX;
  TYPE_Coord2D                    DistanceOffScreen;
  int                             i;
  int                             Panning;
  PrivateSequenceDataPointer      PrivateData;
  TYPE_Rect                       TempRectangle;
  int                             Volume;

#if CE_ARTLIB_EnableDebugMode
  if (SequencePntr->dimensionality != 2 && SequencePntr->dimensionality != 0)
    LE_ERROR_ErrorMsg ("LI_REND2D_SequenceMoved: Unexpected dimensionality.\r\n");
#endif

  if (SequencePntr->dimensionality != 2)
    return TRUE; // Probably a grouping sequence with zero dimensions.

  PrivateData = (PrivateSequenceDataPointer)
    SequencePntr->renderSlotData[mySlotNumber];
#if CE_ARTLIB_EnableDebugMode
  if (PrivateData == NULL)
    LE_ERROR_ErrorMsg ("LI_REND2D_SequenceMoved: Sequence is missing its private render slot data.\r\n");
#endif

  // Invalidate the old position, which may be empty if this is the first time.

  if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_2DBITMAP ||
  SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO ||
  SequencePntr->drawSelectionBox)
    InvalidateBoundingBox (PrivateData, this, TRUE /* Related Slots */);

  // Calculate the new bounding box.  But don't update the invalid areas,
  // that will get done later when things marked for redrawing get done.

  RecalculateBoundingBox (this, SequencePntr, PrivateData);

  // If this is a sound effect, move it so that its audio left/right position
  // matches its screen position.  Use the center of the bounding rectangle
  // to determine the position.

  if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND)
  {
#if CE_ARTLIB_EnableSystemSound
    // Get the base volume out of the record, 100 is full, 0 is quiet.

    Volume = SequencePntr->aux.audio.volume;
    if (Volume < 0) Volume = 0;
    if (Volume > 100) Volume = 100;

    CenterX = 0;
    for (i = 0; i < 4 ; i++)
      CenterX += PrivateData->boundingBox.asArray[i].x;
    CenterX /= 4;

    // Convert position to -100 for left, 0 for center, +100 for right.

    Panning = 200 * (CenterX - cameraRenderArea.left) /
    (cameraRenderArea.right - cameraRenderArea.left) - 100;

      // Reduce the volume by the distance off screen.

    if (Panning < -100)
    {
      Panning = -100;
      DistanceOffScreen = cameraRenderArea.left - CenterX;
    }
    else if (Panning > 100)
    {
      Panning = 100;
      DistanceOffScreen = CenterX - cameraRenderArea.right;
    }
    else // On screen, no distance fading.
      DistanceOffScreen = 0;

    if (DistanceOffScreen >= cameraOffScreenSoundQuietDistance)
      Volume = 0; // Too far away, make it quiet.
    else if (DistanceOffScreen > 0) // Nearby but off screen.  Reduce volume.
      Volume = Volume *
      (cameraOffScreenSoundQuietDistance - DistanceOffScreen) /
      cameraOffScreenSoundQuietDistance;

    LE_SOUND_SetVolumeBufSnd (
      SequencePntr->streamSpecificData.audioStream->bufSnd, (UNS8) Volume);

    LE_SOUND_SetPanBufSnd (
      SequencePntr->streamSpecificData.audioStream->bufSnd, (INT8) Panning);
#endif
  }

  // Return TRUE if any of it is on screen.

  CalculateEnclosingScreenRect (PrivateData, &TempRectangle);

  if (!IntersectRect (&TempRectangle, &TempRectangle,
  &cameraRenderArea))
    return FALSE; // Not in the camera area.

  return IntersectRect (&TempRectangle, &TempRectangle,
    &LE_GRAFIX_ScreenSourceRect); // TRUE if on visible virtual screen area.
}



/*****************************************************************************
 * Tests to see if we can handle the given sequence.  It either has to be
 * a grouping sequence (0D or 2D) or has to be a 2 dimensional bitmap,
 * camera, sound or video.
 */

BOOL LI_REND2D_SlotDataClass::CanHandleSequenceType (
LE_SEQNCR_SequenceType SequenceType, int Dimensionality)
{
  if (SequenceType == LE_CHUNK_ID_SEQ_GROUPING ||
  SequenceType == LE_CHUNK_ID_SEQ_INDIRECT)
    return ((Dimensionality == 0) || (Dimensionality == 2));

  if (SequenceType == LE_CHUNK_ID_SEQ_2DBITMAP ||
  SequenceType == LE_CHUNK_ID_SEQ_SOUND ||
  SequenceType == LE_CHUNK_ID_SEQ_VIDEO ||
  SequenceType == LE_CHUNK_ID_SEQ_CAMERA)
    return (Dimensionality == 2);

  return FALSE;
}



/*****************************************************************************
 * Start an update cycle.  Update the camera and the positions of all 2D
 * objects.  Add the bounding rectangles of all 2D bitmaps that need
 * redrawing to our invalid area, but don't draw just yet.
 *
 * First update the camera if needed.  If it changes at all then all
 * items need their bounding boxes recalculated.  If it zooms in or
 * out, or it rotates then everything needs to be redrawn (mark whole
 * screen as needing an update and recalculate the bounding boxes for
 * all 2D items).  If it just moves sideways, scroll the screen area
 * and offset all the bounding boxes and mark the new part of the
 * screen as needing a redraw.
 */

void LI_REND2D_SlotDataClass::UpdateInvalidAreas (
LE_SEQNCR_RuntimeInfoPointer RootPntr)
{
  LE_SEQNCR_RuntimeInfoPointer    CameraPntr;
  TYPE_Point2D                    CenterPoint;
  TYPE_Coord2D                    DeltaX;
  TYPE_Coord2D                    DeltaY;
  int                             i;
  LE_SEQNCR_Runtime2DInfoPointer  Info2DPntr;
  PrivateSequenceDataRecord       MyData;
  TYPE_Coord2D                    NewDeltaX;
  TYPE_Coord2D                    NewDeltaY;
  TYPE_Coord2D                    OldDeltaX;
  TYPE_Coord2D                    OldDeltaY;
  TYPE_Point2DPointer             PointPntr;
  HRGN                            ShadowedRegion;
  RECT                            TempRect;
  HRGN                            TempRegion;
  TYPE_Point2D                    Translation;

  // See if there is a labeled camera.  For that matter, any 2D object
  // can specify position and rotation, but only cameras have scaling.

  if (labelOfCameraToUse > 0 &&
  labelOfCameraToUse < CE_ARTLIB_SeqncrMaxLabels)
  {
    CameraPntr = LE_SEQNCR_LabelArray[labelOfCameraToUse];

    if (CameraPntr != NULL && CameraPntr->dimensionality == 2)
    {
      Info2DPntr = CameraPntr->dimensionSpecificData.d2;
#if CE_ARTLIB_EnableDebugMode
      if (Info2DPntr == NULL)
        LE_ERROR_ErrorMsg ("LI_REND2D_UpdateForCameraMovement: No 2D runtime info record.\r\n");
#endif

      // Use the center of its bounding box for the camera position.  But
      // first need to calculate the bounding box in world coordinates.  Yes,
      // world, not camera coordinates.

      MyData.boundingBox.named.topLeft.x =
        MyData.boundingBox.named.bottomLeft.x =
        Info2DPntr->boundingBox.left;

      MyData.boundingBox.named.topRight.x =
        MyData.boundingBox.named.bottomRight.x =
        Info2DPntr->boundingBox.right;

      MyData.boundingBox.named.topLeft.y =
        MyData.boundingBox.named.topRight.y =
        Info2DPntr->boundingBox.top;

      MyData.boundingBox.named.bottomLeft.y =
        MyData.boundingBox.named.bottomRight.y =
        Info2DPntr->boundingBox.bottom;

      CenterPoint.x = CenterPoint.y = 0;
      for (i = 0; i < 4; i++)
      {
        PointPntr = MyData.boundingBox.asArray + i;
        LE_MATRIX_2D_VectorMultiply (PointPntr,
          &Info2DPntr->sequenceToWorldTransformation.matrix2D, PointPntr);
        CenterPoint.x += PointPntr->x;
        CenterPoint.y += PointPntr->y;
      }

      // The center point is the average of the corners of the bounding box.

      CenterPoint.x /= 4;
      CenterPoint.y /= 4;

      cameraCenterWorldPosition = CenterPoint;

      // Find the angle of rotation by seeing what angle the bottom
      // side of the bounding box is at.  Make a big long simulated
      // bottom side to maintain accuracy.

      MyData.boundingBox.named.bottomRight.x = 1000000L;
      MyData.boundingBox.named.bottomLeft.x =
        MyData.boundingBox.named.bottomLeft.y =
        MyData.boundingBox.named.bottomRight.y = 0;

      PointPntr = &MyData.boundingBox.named.bottomLeft;
      LE_MATRIX_2D_VectorMultiply (PointPntr,
        &Info2DPntr->sequenceToWorldTransformation.matrix2D, PointPntr);

      PointPntr = &MyData.boundingBox.named.bottomRight;
      LE_MATRIX_2D_VectorMultiply (PointPntr,
        &Info2DPntr->sequenceToWorldTransformation.matrix2D, PointPntr);

      DeltaX = MyData.boundingBox.named.bottomRight.x -
        MyData.boundingBox.named.bottomLeft.x;

      DeltaY = MyData.boundingBox.named.bottomRight.y -
        MyData.boundingBox.named.bottomLeft.y;

      if (DeltaX != 0 || DeltaY != 0)
        cameraScreenRotation = (TYPE_Angle2D) -atan2 (DeltaY, DeltaX);
      else // Box is too small to find angle, just use straight up.
        cameraScreenRotation = 0.0F;

      // And now for the scale factor, which only gets updated from
      // the sequence if it is a real camera.

      if (CameraPntr->sequenceType == LE_CHUNK_ID_SEQ_CAMERA)
        cameraScaleFactor = CameraPntr->aux.cameraFieldOfView;
    }
  }

  // Update the camera matrix to reflect the latest values.

  RecalculateWorldToScreenMatrix (this);

  // See if the matrix is different from last time.

  if (memcmp (&worldToScreenMatrix,
  &previousWorldToScreenMatrix, sizeof (TYPE_Matrix2D)) == 0)
  {
    // Camera hasn't moved, only have to add redraw areas for the
    // things that the sequencer has determined need redrawing.

    InvalidateAllSequencesNeedingRedraw (RootPntr, this);
  }
  else // Camera moved or screen viewport changed.
  {
    // The matrix has changed, all bounding boxes are now invalid.
    // Recalculate them with the new matrix.

    RecalculateAllBoundingBoxes (RootPntr, this);

    // See if the change in matrices corresponds to just a translation.
    // If it does, we can just scroll the screen a bit and avoid having
    // to redraw it all.  Test by converting two points with the
    // before matrix and again with the after matrix.  If the distance
    // between them is the same and no rotation has happened, then we
    // just have a translation.

    MyData.boundingBox.named.bottomRight.x =
      MyData.boundingBox.named.bottomLeft.x =
      MyData.boundingBox.named.bottomLeft.y = 0;
    MyData.boundingBox.named.bottomRight.y = 1000000L;

    PointPntr = &MyData.boundingBox.named.bottomLeft;
    LE_MATRIX_2D_VectorMultiply (PointPntr,
      &previousWorldToScreenMatrix, PointPntr);

    PointPntr = &MyData.boundingBox.named.bottomRight;
    LE_MATRIX_2D_VectorMultiply (PointPntr,
      &previousWorldToScreenMatrix, PointPntr);

    OldDeltaX = MyData.boundingBox.named.bottomRight.x -
      MyData.boundingBox.named.bottomLeft.x;

    OldDeltaY = MyData.boundingBox.named.bottomRight.y -
      MyData.boundingBox.named.bottomLeft.y;

    // And now the after case.

    MyData.boundingBox.named.topRight.x =
      MyData.boundingBox.named.topLeft.x =
      MyData.boundingBox.named.topLeft.y = 0;
    MyData.boundingBox.named.topRight.y = 1000000L;

    PointPntr = &MyData.boundingBox.named.topLeft;
    LE_MATRIX_2D_VectorMultiply (PointPntr,
      &worldToScreenMatrix, PointPntr);

    PointPntr = &MyData.boundingBox.named.topRight;
    LE_MATRIX_2D_VectorMultiply (PointPntr,
      &worldToScreenMatrix, PointPntr);

    NewDeltaX = MyData.boundingBox.named.topRight.x -
      MyData.boundingBox.named.topLeft.x;

    NewDeltaY = MyData.boundingBox.named.topRight.y -
      MyData.boundingBox.named.topLeft.y;

    // Remember the matrix for next time.

    previousWorldToScreenMatrix = worldToScreenMatrix;

    if (!LE_GRAFIX_SlowScrolling &&
    OldDeltaX == NewDeltaX && OldDeltaY == NewDeltaY)
    {
      // This is just a translation.  And our video board can do scrolling.

      Translation.x = MyData.boundingBox.named.topLeft.x -
        MyData.boundingBox.named.bottomLeft.x;
      Translation.y = MyData.boundingBox.named.topLeft.y -
        MyData.boundingBox.named.bottomLeft.y;

      // Invalidate the areas occupied by things above us, so that when we move
      // the pixels around, the picture of the mouse pointer doesn't get moved
      // along with our stuff.

      ShadowedRegion = GetRelatedSlotSolidAreas (this, RootPntr);

      AddInvalidArea (ShadowedRegion, NULL);

      // Move the screen pixels and offset all bounding boxes and
      // invalidation areas to match.  Also invalidate the freshly
      // revealed area on the screen.

      ScrollScreen (this, &Translation);

      // Add the shadowed region again, this time after scrolling.
      // That way the mouse pointer will get redrawn.

      AddInvalidArea (ShadowedRegion, NULL);

      if (ShadowedRegion != NULL)
        DeleteObject (ShadowedRegion);

      // Add the updated rectangles for the things that the sequencer
      // thinks need redrawing, now that the bounding boxes are in sync
      // with the new screen position.

      InvalidateAllSequencesNeedingRedraw (RootPntr, this);
    }
    else // Scaling or rotation or something complex.
    {
      // Scaling or rotation are involved, redraw everything.
      // No need to look for things that need redrawing.

      InvalidateScreen (this, FALSE /* Related Slots */);
    }
  }

  // Tell overlapping renderers what we will be drawing.  But first
  // clip to our camera rendering area since we don't draw outside
  // of that (invalid area could have been enlarged by underlying
  // render slots adding stuff to it, and by scrolling).

  if (invalidArea != NULL)
  {
    IntersectRect (&TempRect, &cameraRenderArea,
      &LE_GRAFIX_ScreenSourceRect);

    TempRegion = CreateRectRgnIndirect (&TempRect);

    if (CombineRgn (invalidArea, invalidArea, TempRegion,
    RGN_AND) == NULLREGION)
    {
      DeleteObject (invalidArea);
      invalidArea = NULL;
    }
    else
      AddRegionToRelatedRenderSlotsInvalidArea (this, invalidArea);

    DeleteObject (TempRegion);
  }
}



/*****************************************************************************
 * Finish an update cycle.  Draw all things in the invalid area.
 */

void LI_REND2D_SlotDataClass::DrawInvalidAreas (
LE_SEQNCR_RuntimeInfoPointer RootPntr)
{
  RECT            TempRect;
  HRGN            TempRegion;

  // Make sure our invalid rectangles region only includes stuff that's
  // on the screen (overlying render slots could have added stuff that's
  // partly off our screen).

  if (invalidArea != NULL)
  {
    IntersectRect (&TempRect, &cameraRenderArea,
      &LE_GRAFIX_ScreenSourceRect);
    TempRegion = CreateRectRgnIndirect (&TempRect);
    if (TempRegion != NULL)
    {
      if (CombineRgn (invalidArea, invalidArea, TempRegion,
      RGN_AND) == NULLREGION)
      {
        DeleteObject (invalidArea);
        invalidArea = NULL;
      }
      DeleteObject (TempRegion);
    }
  }

  // If there is any invalid area, draw it.

  if (invalidArea != NULL)
  {
    // Now redraw the whole tree.  Traverse it and for each bitmap that
    // is in the invalid area (bounding box intersects invalid area),
    // redraw that invalid portion of the bitmap.  If we can't draw,
    // then leave the invalid area alone and hope that we can draw
    // it next time.

    if (LI_REND2D_DrawAllSequencesToWorkArea (RootPntr, this))
    {
      // Successfully drew on the output bitmap.  Now copy the changed
      // portions to the output bitmap, if it isn't the same as the
      // work bitmap.  But only do it if we are the last one in a
      // bunch of related render slots (the topmost render slot is
      // assumed to cover all render slots underneath, otherwise we'd
      // have to split up the drawing or do it outside the render slots).

      if (overlappingRenderSlotSet <= mySlotSetMask)
        CopyWorkAreaToOutputArea (this);

      // Get ready for the next update cycle.

      DeleteObject (invalidArea);
      invalidArea = NULL;
    }
  }
}



/*****************************************************************************
 * Returns TRUE if the given sequence is touched by the rectangle.  Touching
 * means that if the rectangle has zero width then the rectangle is really a
 * point and the sequence only has to contain that point.  Otherwise, the
 * whole sequence has to fit inside the rectangle before this returns TRUE.
 */

BOOL LI_REND2D_SlotDataClass::SequenceTouchesScreenRectangle (
  LE_SEQNCR_RuntimeInfoPointer SequencePntr,
  TYPE_RectPointer TouchingRectanglePntr)
{
  TYPE_Rect                   BoundingRectangle;
  PrivateSequenceDataPointer  PrivateDataPntr;

  PrivateDataPntr =
    (PrivateSequenceDataPointer) SequencePntr->renderSlotData[mySlotNumber];
  if (PrivateDataPntr == NULL)
    return FALSE;

  CalculateEnclosingScreenRect (PrivateDataPntr, &BoundingRectangle);

  if (TouchingRectanglePntr->left >= TouchingRectanglePntr->right)
  {
    // Point test - is the point inside the bounding rectangle?

    return (
      TouchingRectanglePntr->left >= BoundingRectangle.left &&
      TouchingRectanglePntr->right <= BoundingRectangle.right &&
      TouchingRectanglePntr->top >= BoundingRectangle.top &&
      TouchingRectanglePntr->bottom <= BoundingRectangle.bottom);
  }

  // True rectangle test, is the sequence's bounding rectangle fully
  // inside the touching rectangle?

  return (
    TouchingRectanglePntr->left <= BoundingRectangle.left &&
    TouchingRectanglePntr->right >= BoundingRectangle.right &&
    TouchingRectanglePntr->top <= BoundingRectangle.top &&
    TouchingRectanglePntr->bottom >= BoundingRectangle.bottom);
}



/*****************************************************************************
 * Fills in the given rectangle with the bounding box for the given
 * sequence.  Returns FALSE if something is wrong.
 */

BOOL LI_REND2D_SlotDataClass::GetScreenBoundingRectangle (
  LE_SEQNCR_RuntimeInfoPointer SequencePntr,
  TYPE_RectPointer ScreenRectPntr)
{
  PrivateSequenceDataPointer  PrivateDataPntr;

  if (SequencePntr == NULL || ScreenRectPntr == NULL ||
  SequencePntr->dimensionality != 2)
    return FALSE;

  PrivateDataPntr = (PrivateSequenceDataPointer)
    SequencePntr->renderSlotData[mySlotNumber];
  if (PrivateDataPntr == NULL)
    return FALSE;

  CalculateEnclosingScreenRect (PrivateDataPntr, ScreenRectPntr);

  return TRUE;
}



/*****************************************************************************
 * Attempts to convert the given mouse movement (drag) in screen
 * coordinates into a change in position in world coordinates.
 * This render slot's camera is used to determine the equivalent
 * motion projected onto the camera plane.  The SequencePntr is
 * a hint for 3D cameras to identify the distance the plane is
 * from the camera (usually this is the object the user is
 * dragging around).  If SequencePntr is NULL (it can happen),
 * assume a camera plane at a default distance from the camera.
 * The resulting delta vector will be added to the positions of
 * all selected objects (set unused higher dimensions to zero).
 * Returns FALSE if it can't figure it out.
 */

BOOL LI_REND2D_SlotDataClass::DragOnCameraPlane (
  LE_SEQNCR_RuntimeInfoPointer SequencePntr,
  TYPE_Point2DPointer DragFromVirtualScreenPointPntr,
  TYPE_Point2DPointer DragToVirtualScreenPointPntr,
  TYPE_Point3DPointer ResultingDeltaVector)
{
  TYPE_Point2D WorldPointFrom;
  TYPE_Point2D WorldPointTo;

  LE_MATRIX_2D_VectorMultiply (DragFromVirtualScreenPointPntr,
    &screenToWorldMatrix, &WorldPointFrom);

  LE_MATRIX_2D_VectorMultiply (DragToVirtualScreenPointPntr,
    &screenToWorldMatrix, &WorldPointTo);

  ResultingDeltaVector->x = (float) (WorldPointTo.x - WorldPointFrom.x);
  ResultingDeltaVector->y = (float) (WorldPointTo.y - WorldPointFrom.y);
  ResultingDeltaVector->z = 0.0F;

  return TRUE;
}



/*****************************************************************************
 * Gets the camera number.
 */

UNS8 LI_REND2D_SlotDataClass::GetCameraLabelNumber (void)
{
  return labelOfCameraToUse;
}



/*****************************************************************************
 * Return the working surface, if there is one.  NULL on failure.
 */

PDDSURFACE LI_REND2D_SlotDataClass::GetWorkingSurface (void)
{
  return workingSurface;
}



/*****************************************************************************
 * Change the size and location of the area we are drawing on the screen.
 */

BOOL LI_REND2D_SlotDataClass::SetViewport (const TYPE_Rect *ViewportRectPntr)
{
  if (ViewportRectPntr == NULL)
    return FALSE;

  cameraRenderArea = *ViewportRectPntr;
  return TRUE;
}



/*****************************************************************************
 * Change the camera settings.
 */

BOOL LI_REND2D_SlotDataClass::SetCamera (UNS8 CameraNumber,
TYPE_Point3DPointer Position, TYPE_Point3DPointer Forwards,
TYPE_Point3DPointer Up, TYPE_Angle3D FovAngle, TYPE_Coord3D NearZ,
TYPE_Coord3D FarZ)
{
  labelOfCameraToUse = CameraNumber;

  // Only use the provided coordinates if there isn't a camera object,
  // otherwise the user would have to specify reasonable values when
  // just switching objects.

  if (CameraNumber == 0)
  {
    cameraCenterWorldPosition.x = (TYPE_Coord2D) Position->x;
    cameraCenterWorldPosition.y = (TYPE_Coord2D) Position->y;
    cameraOffScreenSoundQuietDistance = (TYPE_Coord2D) Position->z;
    cameraScreenRotation = Up->x;
    cameraScaleFactor = FovAngle;
  }

  return TRUE;
}



/*****************************************************************************
 *****************************************************************************
 ** U S E R   A C C E S S   F U N C T I O N S                               **
 *****************************************************************************
 ****************************************************************************/

/*****************************************************************************
 * Moves the camera to a new position, only useful if the camera isn't
 * attached to a labeled object in the scene (then it gets its coordinates
 * from the object on every update).  You may also want to use the sequencer
 * command for moving cameras, which is synchronised with screen updates.
 */

BOOL LE_REND2D_MoveCamera (LE_REND_RenderSlot RenderSlot,
TYPE_Coord2D X, TYPE_Coord2D Y, TYPE_Scale2D Scale)
{
  LI_REND2D_SlotDataPointer SlotPntr;

  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND2D_SequenceStart: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  SlotPntr = SlotDataArray + RenderSlot;

  SlotPntr->cameraCenterWorldPosition.x = X;
  SlotPntr->cameraCenterWorldPosition.y = Y;
  SlotPntr->cameraScaleFactor = Scale;

  return TRUE;
}



/*****************************************************************************
 * Given a point in screen coordinates, return the current position of that
 * point in the world.  Will be approximate if the camera is moving.
 * Returns TRUE if successful.  FALSE if it fails (and leaves your WorldPoint
 * unchanged).
 */

BOOL LE_REND2D_ScreenToWorldCoordinates (LE_REND_RenderSlot RenderSlot,
TYPE_Point2DPointer ScreenPointPntr, TYPE_Point2DPointer WorldPointPntr)
{
  LI_REND2D_SlotDataPointer SlotPntr;

#if CE_ARTLIB_EnableDebugMode
  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots || ScreenPointPntr == NULL ||
  WorldPointPntr == NULL ||
  LI_SEQNCR_RenderSlotArray [RenderSlot] != SlotDataArray + RenderSlot)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_REND2D_ScreenToWorldCoordinates: "
      "Bad arguments for slot %d.\r\n", (int) RenderSlot);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
    return FALSE;
  }
#endif

  SlotPntr = SlotDataArray + RenderSlot;

  LE_MATRIX_2D_VectorMultiply (ScreenPointPntr,
    &SlotPntr->screenToWorldMatrix, WorldPointPntr);

  return TRUE;
}



/*****************************************************************************
 *****************************************************************************
 ** I N I T I A L I S A T I O N   &   S H U T D O W N   F U N C T I O N S   **
 *****************************************************************************
 ****************************************************************************/

/*****************************************************************************
 * Install the 2D rendering module into a particular render slot.  Returns
 * TRUE if successful.
 *
 * RenderSlot specifies the slot, from 0 to CE_ARTLIB_RendMaxRenderSlots-1,
 * higher render slots are on top of lower ones when drawn on the screen.
 *
 * CameraCenterWorldPosition is the point in the world which will show up
 * in the center of the screen.  If you change it, the world appears to
 * move around.
 *
 * CameraScreenRotation for rotating the camera, but since we can't draw
 * rotated bitmaps, it isn't too useful.
 *
 * CameraScaleFactor is used for zooming in and out.  If you leave it at
 * 1.0 then pixels in objects map exactly to pixels on the screen and no
 * stretching is done - which makes the drawing faster.
 *
 * CameraRenderArea is the area of the screen where the view from the camera
 * will be drawn.  Naturally, it should be a rectangle in screen coordinates.
 * Note that it doesn't have to be the screen, you can specify the bitmap.
 *
 * CameraOffScreenSoundQuietDistance is the distance off the screen that 2D
 * sounds are still audible.  They fade from full volume on the screen (which
 * varies from full left to full right depending on the sound's position) to
 * partial left only or right only volume until they are this many extra
 * pixels off the screen edge.  Of course, your sounds need to be 2D sounds
 * and not 0D (Seq2Cnk has a command line option for this).
 *
 * LabelOfCameraToUse identifes a particular object in the world to use as a
 * camera.  Use zero for none.  If it exists, it's position will be used to
 * set the camera position and its scaling sets the camera zoom.  That way
 * the artists can dope up camera moves.
 *
 * WorkingSurface is the bitmap which the images will be composited on.
 * Since the compositing is only done for the invalid areas of the image
 * the parts which are changing), the rest of the bitmap won't be touched.
 *
 * OutputSurface is another bitmap (usually the real screen) to which the
 * changed parts of the working surface are copied.  The copy is done only
 * by the last render slot in a group of overlapping render slots (usually
 * this is the one used by the mouse).  The output can be the same as the
 * working surface - useful if you are drawing to an off-screen buffer and
 * then flipping it onto the screen.  But if both are the same, and it is
 * visible, you will see the images being composited together (meaning that
 * it will flicker).
 *
 * OverlappingRenderSlotSet is a set of render slots which share pixels on
 * the screen.  When one render slot draws on the screen, it tells all the
 * other slots that it is drawing there and the other slots also redraw their
 * stuff in that area (in case it is under a semi-transparent object or is
 * over part of the redrawn object).  So, if slot 0 is the top half of the
 * screen, slot 1 is the bottom half (perhaps a control panel or score display)
 * and slot 2 is the whole screen (mouse pointer).  Then slot 2 shares the
 * screen with slot 0 and 1 (it overlaps with both), so slot 2's
 * OverlappingRenderSlotSet will be ((1 << 0) | (1 << 1) | (1 << 2)), which
 * is equal to 7 (yes, you include your own slot bit).  Slot 0 is only under
 * slot 2, it doesn't overlap at all with slot 1, so it has
 * ((1 << 0) | (1 << 2)) = 5.  Slot 1 is under slot 2 but doesn't touch
 * slot 0 so it has ((1 << 1) | (1 << 2)) = 6.
 */

BOOL LE_REND2D_InstallInRenderSlot (
  LE_REND_RenderSlot RenderSlot,
  TYPE_Point2D CameraCenterWorldPosition,
  TYPE_Angle2D CameraScreenRotation,
  TYPE_Scale2D CameraScaleFactor,
  TYPE_Rect CameraRenderArea,
  TYPE_Coord2D CameraOffScreenSoundQuietDistance,
  UNS8 LabelOfCameraToUse,
  PDDSURFACE WorkingSurface,
  PDDSURFACE OutputSurface,
  LE_REND_RenderSlotSet OverlappingRenderSlotSet)
{
  BOOL                      ReturnCode;
  LI_REND2D_SlotDataPointer SlotPntr;

  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND2D_InstallInRenderSlot: Slot out of range.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  if (LI_SEQNCR_RenderSlotArray [RenderSlot] != NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND2D_InstallInRenderSlot: Slot is already in use.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  if (WorkingSurface == NULL || OutputSurface == NULL ||
  LE_GRAFIX_DirectDrawObjectN == NULL || CameraScaleFactor <= 0.0)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND2D_InstallInRenderSlot: Bad parameters.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // Set up the initial values of our rendering information.  Coordinates
  // and other things can be changed later by an attached camera sequence
  // or by direct manipulation.

  SlotPntr = SlotDataArray + RenderSlot;
  SlotPntr->cameraCenterWorldPosition = CameraCenterWorldPosition;
  SlotPntr->cameraScreenRotation = CameraScreenRotation;
  SlotPntr->cameraScaleFactor = CameraScaleFactor;
  SlotPntr->cameraOffScreenSoundQuietDistance = CameraOffScreenSoundQuietDistance;
  SlotPntr->cameraRenderArea = CameraRenderArea;
  SlotPntr->workingSurface = WorkingSurface;
  WorkingSurface->AddRef ();
  SlotPntr->workingBitsPntr = NULL;
  SlotPntr->outputSurface = OutputSurface;
  OutputSurface->AddRef ();
  SlotPntr->invalidArea = NULL;
  SlotPntr->labelOfCameraToUse = LabelOfCameraToUse;
  SlotPntr->overlappingRenderSlotSet =
    (OverlappingRenderSlotSet & ~(1 << RenderSlot));

  RecalculateWorldToScreenMatrix (SlotPntr);

  // Enable the sequencer callbacks for our slot.

  ReturnCode = SlotPntr->InstallInSlot (RenderSlot);

  if (!ReturnCode)
    SlotPntr->Uninstall (); // Undo allocations / AddRefs.

  return ReturnCode;
}



/*****************************************************************************
 * Deinstall the render in the given slot, if it is one of ours.  Releases
 * allocated resources and makes the slot available for use.  Returns TRUE
 * if successful, FALSE for wrong slot etc.  Presumably no active sequences
 * are using this slot when it is called, otherwise some sequence specific
 * private data may be left allocated.
 */

BOOL LI_REND2D_SlotDataClass::Uninstall (void)
{
  BOOL  ReturnCode;

  // Disconnect from the sequencer first.  This stops sequences too,
  // which will call our own sequence shutdown callback repeatedly.

  ReturnCode = LI_REND_CommonScreenWriterRenderSlotClass::Uninstall ();

  // Remove references to the drawing surfaces and other things.

  if (workingSurface != NULL)
  {
    workingSurface->Release ();
    workingSurface = NULL;
  }

  if (outputSurface != NULL)
  {
    outputSurface->Release ();
    outputSurface = NULL;
  }

  if (invalidArea != NULL)
  {
    DeleteObject (invalidArea);
    invalidArea = NULL;
  }

  return ReturnCode;
}



/*****************************************************************************
 * Initialise the 2D rendering module.  Halts on failure.
 */

void LI_REND2D_InitSystem (void)
{
  int     i;

  // Set up the free list for our private data allocation system.

  memset (&LI_REND2D_PrivateSequenceDataArray, 0, sizeof (LI_REND2D_PrivateSequenceDataArray));

  NextFreePrivateDataRecordPntr = LI_REND2D_PrivateSequenceDataArray + 0;

  for (i = 0; i < LI_REND2D_MAX_PRIVATE_SEQUENCE_DATA - 1; i++)
  {
    *((PrivateSequenceDataPointer *) (LI_REND2D_PrivateSequenceDataArray + i)) =
    LI_REND2D_PrivateSequenceDataArray + i + 1;
  }

  // Check for slow bank switched video memory, which will make our scrolling
  // technique REALLY REALLY slow.  Well, we now do a scrolling test when the
  // game starts and can use that flag from GRAFIX.

#if 0 // Obsolete code
  memset (&HardwareCapabilities, 0, sizeof (HardwareCapabilities));
  HardwareCapabilities.dwSize = sizeof (HardwareCapabilities);
  if (DD_OK != LE_GRAFIX_DirectDrawObjectN->GetCaps (
  &HardwareCapabilities, NULL))
    LE_ERROR_ErrorMsg ("LI_REND2D_InitSystem: Unable to get DirectDraw capabilities.\r\n");
  BankSwitchedVideoBoardDetected =
    ((HardwareCapabilities.dwCaps & DDCAPS_BANKSWITCHED) != 0);
#endif // Obsolete code

#if CE_ARTLIB_EnableDebugMode
  if (LE_GRAFIX_SlowScrolling)
    LE_ERROR_DebugMsg ("LI_REND2D_InitSystem: "
    "Slow scrolling video board detected!  Scrolling work-around of "
    "drawing the whole screen will be used; it's slow but still faster "
    "than using the video hardware to move the screen.\r\n",
    LE_ERROR_DebugMsgToFileAndScreen);
#endif
}



/*****************************************************************************
 * Deallocate stuff used by the 2D rendering module and make it inactive.
 */

void LI_REND2D_RemoveSystem (void)
{
  LE_REND_RenderSlot RenderSlot;

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
    SlotDataArray[RenderSlot].Uninstall ();

  if (GlobalTempRegionForInvalidArea != NULL)
  {
    DeleteObject (GlobalTempRegionForInvalidArea);
    GlobalTempRegionForInvalidArea = NULL;
  }

  if (GlobalTempRegionForRelatedAreas != NULL)
  {
    DeleteObject (GlobalTempRegionForRelatedAreas);
    GlobalTempRegionForRelatedAreas = NULL;
  }
}

#endif /* CE_ARTLIB_EnableSystemRend2D */
