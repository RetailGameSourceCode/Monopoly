#ifndef __L_REND_H__
#define __L_REND_H__

/*****************************************************************************
 *
 * FILE:        L_REND.h
 * DESCRIPTION: Renderer slot common data structures file,
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Rend.h 16    8/20/99 11:28a Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Rend.h $
 * 
 * 16    8/20/99 11:28a Agmsmith
 * Fixing SetViewport and SetCamera commands.
 *
 * 15    8/18/99 8:12p Agmsmith
 * Added SetCamera sequencer command.
 *
 * 14    8/16/99 5:22p Agmsmith
 * Adding viewport sequencer command.
 *
 * 13    6/07/99 3:18p Agmsmith
 * Added camera capability.
 *
 * 12    5/12/99 3:28p Agmsmith
 * Added bytes to next scan line for working surface.
 *
 * 11    4/27/99 1:32p Agmsmith
 * Number of render slots is now user defined.
 *
 * 10    4/22/99 15:00 Davew
 * Updated to DX v6
 *
 * 9     4/12/99 3:05p Agmsmith
 * Need to get bounding rect for a particular sequence for hit location.
 *
 * 8     4/11/99 6:36p Agmsmith
 * Added render slot method for converting a mouse drag to a camera plane
 * motion in world coordinates.
 *
 * 7     3/13/99 2:29p Agmsmith
 * Added iteration over all active sequences and a way of restricting that
 * to ones under a given mouse position.
 *
 * 6     1/31/99 3:25p Agmsmith
 * Rearranged video player code so that feeding was done during
 * renderering, not in the sequencer, so that it can draw directly on the
 * screen.
 *
 * 5     1/06/99 10:24a Agmsmith
 * Move abstract base class functions out of the header and into
 * L_SEQNCR.cpp, so that you don't get multiple implementations of its
 * functions (maybe - depending on C++ compiler).
 *
 * 4     1/05/99 5:00p Agmsmith
 * Renderer / sequencer interface converted from a bunch of arrays of
 * pointers to functions into C++ virtual function calls and an abstract
 * base class.  Now it should be easier to add new functionality.
 ****************************************************************************/

/************************************************************/
/* DEFINES                                                  */
/*----------------------------------------------------------*/

#define LI_REND_REGION_RECTANGLE_BUFFER_SPACE 10000
  // When retrieving or storing rectangles into a Windows region
  // structure, a temporary buffer of this many bytes is allocated
  // on the stack to hold the rectangles.



/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/

typedef UNS8 LE_REND_RenderSlot;
  // Identifies a single render slot, from 0 to CE_ARTLIB_RendMaxRenderSlots - 1.

typedef UNS8 LE_REND_RenderSlotSet;
  // Identifies a set of render slots.  There can be up to 8 render slots,
  // assigned for various different outputs from the sequencer.  One may be
  // for sound, another for 2D graphics in a stationary portion of the screen
  // and another for a scrolling 2D world.  A sequence may exist in one or
  // more of them, thus the need for this set.



// Forward references for L_SEQNCR.H stuff needed here for our callbacks.

typedef LE_CHUNK_ID LE_SEQNCR_SequenceType;

typedef struct LE_SEQNCR_RuntimeInfoStruct
  LE_SEQNCR_RuntimeInfoRecord, *LE_SEQNCR_RuntimeInfoPointer;



// This is the abstract base class which is used for all render slots.
// It's main purpose is to define the standard render slot manipulation
// functions which all the slots share as virtual functions.  This time
// we are using C++ virtual functions instead of multiple arrays of
// function pointers (making it easier to add new slot functions).

typedef class LI_REND_AbstractRenderSlotClass
{
  public:
  LI_REND_AbstractRenderSlotClass ();
  virtual ~LI_REND_AbstractRenderSlotClass ();
    // Constructor and destructor as usual.

  virtual BOOL SequenceStartUp (LE_SEQNCR_RuntimeInfoPointer SequencePntr);
    // This renderslot callback is called by the sequencer when it creates
    // a sequence.  Note that the position, parent links and other runtime
    // info haven't been calculated yet, so that this is more of an
    // opportunity for the render slot to allocate its own internal storage
    // (see renderSlotData).  Return FALSE if you don't want the sequence
    // to start, TRUE if you were successful with your internal allocations.
    // The startup function will be followed eventually by a call to the
    // Sequence Moved function to set its initial position (so treat a newly
    // started sequence as not being visible until it has moved into position).

  virtual void SequenceShutDown (LE_SEQNCR_RuntimeInfoPointer SequencePntr);
    // This function is called by the sequencer when it is terminating a
    // sequence.  For a 2D bitmap type of sequence this will mark the position
    // of a bitmap sequence as needing a redraw in the next update.  3D slots
    // don't have to do much since they redraw everything every frame.  Sound
    // renderers can kill the sound.  After this call the sequence runtime info
    // will be deallocated, so don't use any cached pointers to it etc.

  virtual BOOL SequenceMoved (LE_SEQNCR_RuntimeInfoPointer SequencePntr);
    // This function is called by the sequencer when a sequence moves around.
    // It should return TRUE if the sequence is still visible in the render
    // slot (or is near to being visible - half a screen width away for
    // a 2D sequence is a good compromise for scrolling worlds).  For a 2D
    // renderer, it would mark the new and old rectangular screen areas
    // occupied by a bitmap as needing a redraw and store the
    // new screen rectangle in its private data.

  virtual BOOL CanHandleSequenceType (LE_SEQNCR_SequenceType SequenceType,
    int Dimensionality);
    // This function is called by the sequencer when it wants to know if a
    // render slot can handle the given type of sequence.  Used when it
    // has a new sequence and wants to find out which slots can render it.
    // Returns TRUE if it can render the given sequence type and dimensionality.
    // You should also return TRUE for grouping sequences which could possibly
    // contain sequences of the type you understand, since subsequences use a
    // subset of the render slot set of their parent.

  virtual void UpdateInvalidAreas (LE_SEQNCR_RuntimeInfoPointer RootPntr);
    // This function is called by the sequencer when it has finished updating
    // all the sequence positions (sequence to world matrix) and has added
    // and removed sequences as time and commands dictate.  The renderer
    // should first update its camera settings.  Then go through the given
    // render tree and update the slot's invalid area for all the sequences
    // it needs to (ones assigned to its slot and of concrete type - don't
    // draw grouping sequences).  You can assume that overlapping render
    // slots are called in increasing render slot order, so the ones with
    // lower numbers are underneath.

  virtual void DrawInvalidAreas (LE_SEQNCR_RuntimeInfoPointer RootPntr);
    // This function is called by the sequencer after all the invalid areas
    // have been updated in all the slots.  The renderer should go through
    // the given render tree and draw the things that intersect the invalid
    // area.  Or for 3D, you probably end up drawing everything.  Again
    // it is called in increasing render slot number so that overlapping
    // render slots have the ones with larger slot numbers as being on top.

  virtual void AddInvalidArea (HRGN InvalidAreaRegion, PDDSURFACE ScreenSurface);
    // This function is called by the sequencer or another render slot when
    // it wants to add some invalid area (in screen coordinates) to a renderer.
    // This is useful for overlapping render slots, such as the one drawing
    // the mouse over the whole screen - when the mouse moves, the underlying
    // render slots need to be told to redraw the affected areas.  The
    // ScreenSurface is passed in so that the render slot can decide if it
    // is drawing to the same screen as the caller is drawing to (if it is
    // different then it ignores the invalid areas, NULL to always update
    // the area).

  virtual HRGN GetSolidArea (LE_SEQNCR_RuntimeInfoPointer RootPntr,
    PDDSURFACE ScreenSurface);
    // This function is called by the sequencer or another render slot when
    // it wants to find out what parts of the screen are occupied (solid
    // or semi-transparent).  This is used during scrolling when the
    // scrolling surface wants to clean out mouse pointer images from an
    // overlapping mouse render slot.  The returned region is in screen
    // coordinates, the caller will deallocate it.  The ScreenSurface is
    // passed in so that the render slot can decide if it is drawing to
    // the same screen as the caller is drawing to (if it is different
    // then it returns NULL, if the surface is NULL then it always
    // returns the area).

  virtual BOOL SequenceTouchesScreenRectangle (
    LE_SEQNCR_RuntimeInfoPointer SequencePntr,
    TYPE_RectPointer TouchingRectanglePntr);
    // Returns TRUE if the given sequence is touched by the rectangle
    // (in virtual screen coordinates).  Touching means that if the
    // rectangle has zero width then the rectangle is really a point
    // and the sequence's screen footprint only has to contain that point.
    // Otherwise, the whole sequence's screen footprint has to fit inside
    // the rectangle before this function will return TRUE.

  virtual BOOL GetScreenBoundingRectangle (
    LE_SEQNCR_RuntimeInfoPointer SequencePntr,
    TYPE_RectPointer ScreenRectPntr);
    // Fills in the given rectangle with the bounding box for the given
    // sequence.  Returns FALSE if something is wrong.

  virtual BOOL DragOnCameraPlane (
    LE_SEQNCR_RuntimeInfoPointer SequencePntr,
    TYPE_Point2DPointer DragFromVirtualScreenPointPntr,
    TYPE_Point2DPointer DragToVirtualScreenPointPntr,
    TYPE_Point3DPointer ResultingDeltaVector);
    // Attempts to convert the given mouse movement (drag) in screen
    // coordinates into a change in position in world coordinates.
    // This render slot's camera is used to determine the equivalent
    // motion projected onto the camera plane.  The SequencePntr is
    // a hint for 3D cameras to identify the distance the plane is
    // from the camera (usually this is the object the user is
    // dragging around).  If SequencePntr is NULL (it can happen),
    // assume a camera plane at a default distance from the camera.
    // The resulting delta vector will be added to the positions of
    // all selected objects (set unused higher dimensions to zero).
    // Returns FALSE if it can't figure it out.

  virtual UNS8 GetCameraLabelNumber (void);
    // A number of zero means don't use a sequence as the camera, just
    // stay with the last specified settings.  Otherwise it is the label
    // of the animated object currently being used as a camera.

  virtual PDDSURFACE GetWorkingSurface (void);
    // Returns the working surface for render slots which draw on bitmaps,
    // returns NULL for ones which don't support drawing.  Used by the
    // video player for getting attributes (need surface properties).

  virtual BOOL SetViewport (const TYPE_Rect *ViewportRectPntr);
    // Changes the screen area occupied by the render slot, useful
    // for switching from wide screen to thumbnail views in 3D, also
    // works in 2D.  Does nothing in 0D.

  virtual BOOL SetCamera (UNS8 CameraNumber, TYPE_Point3DPointer Position,
    TYPE_Point3DPointer Forwards, TYPE_Point3DPointer Up,
    TYPE_Angle3D FovAngle, TYPE_Coord3D NearZ, TYPE_Coord3D FarZ);
    // Set the camera of a 2D or 3D render slot.  See the
    // LE_SEQNCR_SetCameraTheWorks function for a description of
    // all the arguments.

  BOOL InstallInSlot (LE_REND_RenderSlot RenderSlot);
  virtual BOOL Uninstall (void);
    // Installs and removes this render slot from the sequencer's
    // global array of render slots.  Uninstall can also do other
    // related activities (like releasing bitmaps used).  Install
    // isn't virtual since there are render specific Install
    // functions.

  UNS8 mySlotNumber;
    // The slot number of this render slot.  Saves on having to pass in
    // both the pointer and the index to various functions, or having
    // to do an address inverse calculation.  Set to
    // CE_ARTLIB_RendMaxRenderSlots when the slot is inactive.

  LE_REND_RenderSlotSet mySlotSetMask;
    // Equal to (1 << mySlotNumber), useful for masking off our slot bit
    // in checking which slot a sequence belongs to.  Set to zero when the
    // slot is inactive.

} *LI_REND_AbstractRenderSlotPointer;



// This data is associated with each active render slot that draws
// to a bitmap / screen.  Zero dimensional ones don't use this.

typedef class LI_REND_CommonScreenWriterRenderSlotClass :
  public LI_REND_AbstractRenderSlotClass
{
  public:

  TYPE_Rect cameraRenderArea;
    // This rectangle defines the portion of the screen (well, could be any
    // bitmap but it is usually the screen) that the renderer will draw
    // into, in absolute screen coordinates.  Usually will be the full
    // screen, but you can make it smaller for split screen and other
    // effects (like a non-scrolling control panel underneath in a scrolling
    // world).

  PDDSURFACE workingSurface;
    // The images will be composited together on this surface before being
    // drawn to the output surface.  It is the same size as the output
    // surface (and should contain the cameraRenderArea rectangle).  The
    // contents of this surface don't need to be preserved between cycles,
    // it is just for temporary compositing use.

  UNS16 workingPixelsToNextScanLine;
    // Width of the working surface in pixels plus a padding factor
    // (unused pixels at the end to make the scan lines DWORD aligned).
    // It is often called pitch or stride, meaning the number of pixels
    // to skip to get to the next scan line.  Only valid when
    // workingBitsPntr isn't NULL.  You should really use
    // workingBytesToNextScanLine if possible since padding can be a
    // fraction of the number of pixels (particularly in 24 bit mode).

  UNS16 workingBytesToNextScanLine;
    // Number of bytes per scan line, includes the padding bytes at the
    // end of the scan line.  Only valid when workingBitsPntr isn't NULL.

  UNS16 workingDepth;
    // Depth of the working surface in bits per pixel, usually 16, 24 or
    // 32.  Valid only when workingBitsPntr isn't NULL.

  void *workingBitsPntr;
    // Points at the working bitmap's bits, NULL when the working surface
    // isn't locked down.  Use workingStride and workingDepth with this
    // pointer.

  PDDSURFACE outputSurface;
    // After compositing, the whole image is drawn to this surface, filling
    // the rectangular area in cameraRenderArea, and using its origin offset.
    // Sometimes it will scroll this area before updating, if the world
    // is scrolling, to avoid redrawing everything.  The renderer assumes
    // that this surface isn't modified between updates so that it doesn't
    // have to redraw everything.  You can have the same surface for both
    // the working area and output surface, if you don't mind flickering
    // updates (you can see it draw); useful for off-screen rendering.

  HRGN invalidArea;
    // A Windows style region (bunch of rectangles) that identifies
    // the area of the screen that needs to be redrawn.  NULL if none.
    // Uses screen coordinates, and also gets clipped to the camera area.

  UNS8 labelOfCameraToUse;
    // Non-zero to use a labeled camera to update the settings in this
    // record on every update.  Zero means don't bother looking
    // and just use the current settings in this record.  Of course, if
    // the labeled sequence doesn't exist, just reuse the current settings
    // until it appears.

  LE_REND_RenderSlotSet overlappingRenderSlotSet;
    // These identify the render slots that this one overlaps with,
    // do not include this render slot in the set.
    // Usually the one on top is a mouse render slot that covers the
    // whole screen.  The ones underneath can be any 2D (or 3D)
    // render slot, for example a control panel area and a scrolling
    // world area.  These slots have to update each other's invalid
    // areas when things change, and use these sets to identify the
    // render slots over or under them.  Because the render slots are
    // done in increasing order (slot zero is done first, then 1 and
    // so on), lower numbered slots are on the bottom of the overlapping
    // areas.

} *LI_REND_CommonScreenWriterRenderSlotPointer;

#endif // __L_REND_H__
