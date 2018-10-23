#ifndef __L_SEQNCR_H__
#define __L_SEQNCR_H__

/*****************************************************************************
 *
 *   FILE:        L_Seqncr.h
 *   DESCRIPTION: Animation timing and sequencer header file.
 *
 *   (C) Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Seqncr.h 69    5/10/99 4:24p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Seqncr.h $
 * 
 * 69    5/10/99 4:24p Agmsmith
 * Added a double size option to the Bink standalone video player - uses
 * an offscreen surface and stretch blits to make it larger.
 *
 * 68    10/05/99 12:00p Mikeh
 * Added function LE_SEQNCR_SelectionBoxToggle()
 * which allows the toggling of the bounding box
 * for a sequence. Probably of limited value in an
 * actual game but for debugging purposes...
 *
 * 67    17/09/99 12:20p Agmsmith
 * Made stand-alone Bink player available separately.
 *
 * 66    16/09/99 9:32p Agmsmith
 * Now with a stand-alone Bink video player function too.
 *
 * 65    16/09/99 5:26p Agmsmith
 * Adding Bink video player.
 *
 * 64    99/09/02 12:52p Agmsmith
 * Get a 3D bounding box for a 3D animation.
 *
 * 63    8/18/99 8:17p Agmsmith
 * Added SetCamera sequencer command.
 *
 * 62    8/16/99 5:25p Agmsmith
 * Adding set pitch and viewport sequencer commands, set camera under
 * construction.
 *
 * 61    8/09/99 5:35p Agmsmith
 * Added time to for-all-chunks function, needed for dope sheet select
 * next sequence feature.
 *
 * 60    7/28/99 2:47p Agmsmith
 * Moved possible data chunk pointers from chunky module to sequencer
 * header.
 *
 * 59    7/06/99 1:14p Agmsmith
 * Added function for finding position of a child mesh object, for
 * Monopoly 3D token location.  Should really use hit boxes but...
 *
 * 58    7/02/99 11:18a Agmsmith
 * Added drop frames option to starting a sequence.
 *
 * 57    6/07/99 3:19p Agmsmith
 * Added camera capability.
 *
 * 56    6/04/99 1:02p Agmsmith
 * Adding field of view private chunk and getting cameras to work.
 *
 * 55    5/26/99 3:48p Agmsmith
 * Updated for new camera chunk fields.
 *
 * 54    5/09/99 1:00p Agmsmith
 * Moved mesh proportion out of the tweeker chunk and into the mesh choice
 * chunk.
 *
 * 53    4/27/99 1:34p Agmsmith
 * Number of render slots is now user defined.
 *
 * 52    4/23/99 5:01p Agmsmith
 * Moved maximal data record here.
 *
 * 51    4/19/99 7:35p Agmsmith
 *
 * 50    4/19/99 12:43p Agmsmith
 * Private chunk numbering order changed.
 *
 * 49    4/19/99 11:56a Agmsmith
 * Added some 3D glue functions for start and move.
 *
 * 48    4/19/99 11:29a Agmsmith
 *
 * 47    4/18/99 4:24p Agmsmith
 * Changed offset/scale/rotate to origin/scale/rotate/offset chunk.
 *
 * 46    4/17/99 3:30p Agmsmith
 *
 * 45    4/12/99 3:06p Agmsmith
 * Need to get bounding rect for a particular sequence for hit location.
 *
 * 44    4/11/99 3:50p Agmsmith
 * Now postload converts BMPs into native screen depth bitmaps, rather
 * than having the sequencer do it.  So remove all BMP things everywhere.
 *
 * 43    3/30/99 3:34p Agmsmith
 *
 * 42    3/28/99 11:42a Agmsmith
 * Sound settings are now percentages in optional subchunks.  Also renamed
 * cancel interpolation as identity.
 *
 * 41    3/27/99 12:45p Agmsmith
 * Added suicide ending action for error handling.
 *
 * 40    3/25/99 3:45p Agmsmith
 * New chunks for sound pitch, volume, panning.
 *
 * 39    3/25/99 12:41p Agmsmith
 * Added the Cancel type of interpolation.
 *
 * 38    3/24/99 2:15p Agmsmith
 * Tweekers under construction.
 *
 * 37    3/24/99 8:56a Agmsmith
 *
 * 36    3/24/99 8:50a Agmsmith
 * Parsing of tweeker sequences added.
 *
 * 35    3/23/99 1:40p Agmsmith
 * Added some more private subchunks for position information, and
 * starting to add tweekers (more matrices involved) for mesh
 * interpolation etc.
 *
 * 34    3/21/99 3:35p Agmsmith
 * Changes to allow all sequences to have child sequences.
 *
 * 33    3/19/99 10:05a Agmsmith
 * Added tweeker sequence type.
 *
 * 32    3/18/99 10:57a Andrewx
 * Modified for HMD display
 *
 * 31    3/16/99 10:49a Agmsmith
 * Removed more compatability stubs and added wait for all sequences.
 *
 * 30    3/15/99 12:04p Agmsmith
 * Return default render slot.
 *
 * 29    3/14/99 12:14p Agmsmith
 * Added LE_SEQNCR_StartXYSlotLoadOpts.
 *
 * 28    3/13/99 3:28p Agmsmith
 * Added FindNext functions for finding sequences under the mouse cursor.
 *
 * 27    3/13/99 2:30p Agmsmith
 * Added iteration over all active sequences and a way of restricting that
 * to ones under a given mouse position.
 *
 * 26    3/13/99 1:31p Agmsmith
 * Reordered a few function prototypes.
 *
 * 25    3/09/99 4:13p Agmsmith
 * Bump up time delay between updates for stay at end sequences to the
 * max, about 4 seconds.
 *
 * 24    3/07/99 1:58p Agmsmith
 * Removed initialClock from sequence headers.
 *
 * 23    3/05/99 10:44a Agmsmith
 *
 * 22    3/04/99 4:19p Agmsmith
 * Added LE_CHUNK_ForAllSubSequences function.
 *
 * 21    2/16/99 2:19p Agmsmith
 * Detect stalled videos and force a redraw to get them feed again.
 *
 * 20    2/16/99 1:27p Agmsmith
 * Added command to forget video alternative paths.
 *
 * 19    2/15/99 4:54p Agmsmith
 * Added alternative path through a video feature.
 *
 * 18    2/14/99 5:13p Agmsmith
 * Draw a box around selected sequences.
 *
 * 17    2/09/99 1:41p Agmsmith
 * Fix drawing to an off-screen bitmap with the new video player.
 *
 * 16    2/09/99 12:55p Agmsmith
 * Added LE_SEQNCR_GetAVIFileCharacteristics.
 *
 * 15    2/02/99 1:47p Agmsmith
 * Video API changes again.
 *
 * 14    2/01/99 3:21p Agmsmith
 * Added commands to change video properties while playing.
 *
 * 13    2/01/99 2:09p Agmsmith
 * Added new video attributes.
 *
 * 12    1/31/99 5:33p Agmsmith
 * Changes for new improved video API.
 *
 * 11    1/31/99 3:29p Agmsmith
 * Rearranged video player code so that feeding was done during
 * renderering, not in the sequencer, so that it can draw directly on the
 * screen.
 *
 * 10    1/05/99 5:00p Agmsmith
 * Renderer / sequencer interface converted from a bunch of arrays of
 * pointers to functions into C++ virtual function calls and an abstract
 * base class.  Now it should be easier to add new functionality.
 ****************************************************************************/


/************************************************************/
/* SECONDARY INCLUDE FILES                                  */
/*----------------------------------------------------------*/

#if CE_ARTLIB_EnableSystemBink || CE_ARTLIB_EnableSystemBinkStandAlone
  #include "../Bink/Bink.h"
#endif


#define USE_OLD_VIDEO_PLAYER (!CE_ARTLIB_EnableSystemVideo && !CE_ARTLIB_EnableSystemBink)
  // If a newer player isn't being used, use the old AVIFile playback
  // interface from Video For Windows, which doesn't work under Win2000.



/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/

/* This pragma makes fields adjacent in memory in the following structures,
   Needed so that they all have the same layout when exchanged via external
   files. */
#pragma pack(push,1)

// This structure defines a 2D transformation that is applied to a child
// sequence's coordinates to get the equivalent coordinates in the
// parent's world.  The scaling and rotation occurs around the child's
// origin at (0,0), so put the child's origin at some useful place,
// like at the feet of a humanoid character.  Note that matrices are
// used because it is easier to combine transformations, for nested
// objects.  Later it may be changed to separate offset, scale and
// rotation values.  Or perhaps a flag and a choice between plain
// offset or matrix mode.

typedef struct LE_SEQNCR_Transform2DStruct
{
  TYPE_Matrix2D matrix2D;
} LE_SEQNCR_Transform2D, *LE_SEQNCR_Transform2DPointer;



// Same again for 3D sequences.  Could also be a 3x3 matrix containing scaling
// and rotations with a separate vector for translations.  But for now it is
// convenient to use a 4x4 matrix for everything.

typedef struct LE_SEQNCR_Transform3DStruct
{
  TYPE_Matrix3D matrix3D;
} LE_SEQNCR_Transform3D, *LE_SEQNCR_Transform3DPointer;



// The various different kinds of things that can happen when a sequence
// reaches its end.  Things like looping or stopping.

typedef enum LE_SEQNCR_EndingActionEnum
{
  LE_SEQNCR_NoEndingAction = 0,
    // No ending action is specified.  This is usually used when the
    // caller supplies an action argument and doesn't really want
    // to change the current action.

  LE_SEQNCR_EndingActionSuicide = LE_SEQNCR_NoEndingAction,
    // A runtime only action.  If the ending action field gets changed
    // to this value then the sequence will self destruct at the next
    // update.  Usually used for error handling - self destruct when
    // an error encountered while playing.

  LE_SEQNCR_EndingActionStop,
    // The sequence stops and disappears from the screen.

  LE_SEQNCR_EndingActionStayAtEnd,
    // When this sequence hits its end, it loops back to the last tick in its
    // time duration, in other words it keeps the last frame on the screen.
    // Even though it is looping back continuously (once every TimeMultiple),
    // the renderer should realise that it isn't changing shape or moving
    // and thus not waste time redrawing it.

  LE_SEQNCR_EndingActionLoopToBeginning,
    // When this sequence's clock hits the end of its duration, it will
    // cycle back to the start and continue playing from there.

  LE_SEQNCR_EndingActionMAX
}
#if !CE_ARTLIB_UseEnumDataTypes
; typedef UNS8  /* For compilers which don't like enums, use an int instead. */
#endif
LE_SEQNCR_EndingAction;



// Ways of dropping frames.

typedef enum LE_SEQNCR_DropFramesEnum
{
  LE_SEQNCR_DropDefault = 0,  // Use the setting in the sequence.
  LE_SEQNCR_DropKeepFrames,   // Play all frames.
  LE_SEQNCR_DropDropFrames,   // Drop frames to keep up with parent clock.
  LE_SEQNCR_DropMAX
}
#if !CE_ARTLIB_UseEnumDataTypes
; typedef UNS8  /* For compilers which don't like enums, use an int instead. */
#endif
LE_SEQNCR_DropFrames;



typedef struct LE_SEQNCR_WatchStruct
{
  LE_DATA_DataId  dataID;         // Use -1 for not watching.
  UNS16           priority;
  UNS8            updateCount;    // Incremented on every update.
  unsigned int    hasStarted : 1; // TRUE if Started playing.
  unsigned int    hasHitEnd : 1;  // Clock hit the end at least once.
  unsigned int    wasDeleted : 1; // TRUE if it has finished (stopped).
} LE_SEQNCR_WatchRecord, *LE_SEQNCR_WatchPointer;



typedef struct LE_SEQNCR_AutoRecolourStruct
{
  LE_DATA_DataId firstInRange;
  LE_DATA_DataId lastInRange;
    // These DataIDs define a range for automatic recolouring of TABs.
    // If a TAB (8 bit image used in an animation) is within this range
    // (ID >= first and ID <= last), it will be recoloured in memory using
    // the given mapping, before it is displayed.  If it stays in memory,
    // it will remain coloured and thus won't take much extra CPU time.
    // Set the firstInRange to -1 to turn off this range.

  LPCOLORMAP  colourMapArray;
  UNS16       colourMapSize;
    // An array of colour pair structures, each has a FROM and a TO
    // colour.  Size of the array (number of structs) is in colourMapSize.

} LE_SEQNCR_AutoRecolourRecord, *LE_SEQNCR_AutoRecolourPointer;



typedef enum LE_SEQNCR_DataDisposalOptionsEnum
{
  LE_SEQNCR_DATA_DISPOSE_DO_NOTHING = 0,
  LE_SEQNCR_DATA_DISPOSE_UNLOAD,
  LE_SEQNCR_DATA_DISPOSE_DEALLOCATE_CONTENTSDATAID,
  LE_SEQNCR_DATA_DISPOSE_MAX
}
#if !CE_ARTLIB_UseEnumDataTypes
; typedef UNS8  /* For compilers which don't like enums, use an int instead. */
#endif
LE_SEQNCR_DataDisposalOptions;
  // The various things you can do with the data items (bitmaps and such)
  // after a sequence has finished.  See LE_SEQNCR_CleanUpSequenceData
  // and related things.



typedef enum LE_SEQNCR_InterpolationTypesEnum
{
  LE_SEQNCR_INTERPOLATION_IDENTITY = 0,
    // Cancels any interpolation of positions, going back to the
    // uninterpolated value (set the tweeker transformation matrix
    // to the identity matrix).  Otherwise, if you don't use this,
    // the last tweeker's settings will remain in effect.

  LE_SEQNCR_INTERPOLATION_CONSTANT,
    // The tweeker will use the first subchunk key value for
    // the whole tweek time.  This is also used by the code as
    // the first of the tweekers with one data subchunk, so add
    // other single subchunk tweeker types after this one.

  LE_SEQNCR_INTERPOLATION_LINEAR_PAIR,
    // The tweeker interpolates linearly between the first and second
    // key value subchunks inside the tweeker chunk.  This is also
    // used by the code as the first of the tweekers with two data
    // subchunks, so add 2 or more subchunk tweeker types after this one.

  LE_SEQNCR_INTERPOLATION_MAX,
    // This is the number of implemented interpolation methods.

  LE_SEQNCR_INTERPOLATION_BSPLINE,
    // Not implemented.  But if it was, it would use a B-Spline interpolation,
    // and each tweek subchunk would contain one key value subchunk.
    // The interpolation would look at multiple tweeker subchunks (all the
    // ones with the same priority level - which identifies a group of
    // related tweekers) to do the interpolation, combining the start times
    // of the tweekers with their values to do fancier interpolation.
}
#if !CE_ARTLIB_UseEnumDataTypes
; typedef UNS8  /* For compilers which don't like enums, use an int instead. */
#endif
LE_SEQNCR_InterpolationTypes;
  // The various different ways you can interpolate the data in a tweeker.



// The callback function for use by ForAllSequences when iterating
// over all sequences which are currently active.  If the function
// returns TRUE then the iteration continues, FALSE stops it.  A
// user provided value is also passed in.  If the same sequence is
// in multiple render slots, you will be called several times with
// the same sequence.  Please be quick in this function since the
// sequencer is locked out from updating the sequences while the
// iteration is being done.

typedef BOOL (*LI_SEQNCR_IterationCallbackFunctionPointer) (
  LI_REND_AbstractRenderSlotPointer RenderSlotPntr,
  LE_SEQNCR_RuntimeInfoPointer SequencePntr,
  void *UserValue);



// The various different kinds of sequences.  Just use the same numbers
// as in the chunk headers for the public chunks.  See the chunk structures
// below for more details.

typedef LE_CHUNK_ID LE_SEQNCR_SequenceType;
/*
  LE_CHUNK_ID_SEQ_GROUPING == 1,
  LE_CHUNK_ID_SEQ_INDIRECT,
  LE_CHUNK_ID_SEQ_2DBITMAP,
  LE_CHUNK_ID_SEQ_3DMODEL,
  LE_CHUNK_ID_SEQ_SOUND,
  LE_CHUNK_ID_SEQ_VIDEO,
  LE_CHUNK_ID_SEQ_CAMERA,
  LE_CHUNK_ID_SEQ_PRELOADER,
  LE_CHUNK_ID_SEQ_3DMESH,
  LE_CHUNK_ID_SEQ_TWEEKER,
  LE_CHUNK_ID_SEQ_MAX
*/

typedef enum LE_SEQNCR_PrivateChunkIDsEnum
{
  LE_SEQNCR_CHUNK_ID_DIMENSIONALITY = LE_CHUNK_ID_NULL_IN_CONTEXT + 1,
  LE_SEQNCR_CHUNK_ID_2D_OFFSET, // Simplest to most complex data type,
  LE_SEQNCR_CHUNK_ID_2D_XFORM,  // will promote to the last one here,
  LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET, // like this one for 2D.
  LE_SEQNCR_CHUNK_ID_3D_OFFSET, // 3D also has a range of transformation
  LE_SEQNCR_CHUNK_ID_3D_XFORM,  // complexity like 2D, ending at this
  LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET, // origin... chunk.
  LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX,
  LE_SEQNCR_CHUNK_ID_3D_BOUNDING_BOX,
  LE_SEQNCR_CHUNK_ID_3D_BOUNDING_SPHERE,
  LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE,
  LE_SEQNCR_CHUNK_ID_HIT_BOX_LABEL,
  LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH,
  LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME,
  LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING,
  LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW,
  LE_SEQNCR_CHUNK_ID_MAX_PRIVATE_CHUNKS
} LE_SEQNCR_PrivateChunkIDs;
  // See chunk records below for details on each one.



/*****************************************************************************
 *****************************************************************************
 ** S E Q U E N C E   C H U N K   S T R U C T U R E S                       **
 *****************************************************************************
 ****************************************************************************/

// This is the common 12 byte header chunk that all the sequence
// type chunks use when they are in the datafile.

typedef struct LE_SEQNCR_SequenceChunkHeaderStruct
{
  int parentStartTime : 24;
    // Parent sequence clock time when this sequence starts being displayed.
    // If the parent clock goes backwards, and the parentStartTime ends up
    // in the future of the parent, then this sequence will be stopped.
    // Be careful with times, these are edge boundary values, not tick
    // numbers.  So a sequence that starts at time 0 and ends at time 4,
    // runs in real time from 0.0 seconds up to but not including 1/15
    // (0.06667) seconds.

  unsigned int priority : 8;
    // Specifies the column this cell should be displayed in in the dope
    // sheet.  From 0 to n, or from back to front.  The cells are sorted
    // so that the ones with a lower number get drawn first and thus appear
    // behind the others.  All the child sequences in a sequence get drawn
    // in a bunch, before the next parent (aunt or uncle :-) sequence is
    // drawn, so this is a subpriority when compared to the sequence
    // priorities.  In 3D this just controls the order in which the object
    // is drawn, if it is a top level sequence, subsequences within the top
    // level one are sorted by distance from the camera on the PSX and just
    // lumped together on systems that have a Z-buffer.

  int endTime : 24;
    // Sequence clock time when this thing stops, or loops.  Use 0 to
    // represent infinity (good for grouping sequences that contain
    // looping children).  The duration of the sequence is
    // endTime, or infinite if there is no end time.

  unsigned int timeMultiple : 6;
    // The basic update rate for this sequence in ticks.  For a sequence
    // running at 15 frames per second, this will be 4 (60 ticks per second
    // divided by 15 frames per second = 4 ticks per frame).  To save on
    // processing time, this sequence will only be updated this often.  So,
    // an engine running at 60fps only bothers updating a 15 fps sequence
    // every 4 frames.  Note that changing this won't affect the speed of
    // the sequence, just the update rate!  However, the times in the frames
    // and other places are still in ticks and probably are multiples of
    // TimeMultiple (but they don't have to be, which could lead to dropped
    // frames for frames that are shorter than one time multiple).  Don't
    // use zero!

  unsigned int dropFrames : 1;
    // If TRUE then drop frames to keep up with the parent sequence's clock,
    // if FALSE then play every multiple of TimeMultiple ticks (the sequence's
    // clock gets incremented by at most TimeMultiple on every update).
    // Similar to the old NOSYNC_SEQUENCE.  This is only useful in top level
    // sequences since child sequences get ignored when the parent skips
    // frames (in other words, it is best to have the children drop frames
    // otherwise they get out of sync with their parent).

  unsigned int lastUse : 1;
    // Flags this as the last use of this thing, so it can be deallocated
    // after it has finished.

  unsigned int endingAction : 3;
    // What to do when the sequence ends (ends when the sequence's clock
    // reaches or exceeds the sequence's total time).  If there is another
    // sequence chained to this one, then the chain is taken and this
    // sequence is stopped in all cases.  Otherwise either loop back to the
    // beginning (set sequence clock to zero), stay on the last tick (force
    // the clock back to the last tick), or just stop it.  Other ending
    // actions may be added as needed.  See LE_SEQNCR_EndingAction.

  unsigned int scrollingWorld : 1;
    // If this is TRUE then this sequence is part of a scrolling world and
    // its children appear and disappear as it goes on and off screen (the
    // renderers decide if it is on or off screen depending on the sequence's
    // bounding box and distance from the camera).  Every time the sequence
    // goes back on screen, this sequence's clock restarts at zero and the
    // children reappear, much like a looping sequence does it.  When it goes
    // off screen, all children are deleted and the clock goes to zero and
    // stays there.  This is most useful with the indirect grouping sequence,
    // since it can unload its indirect list of children and not take up much
    // memory space while it is off screen.

  unsigned int absoluteDataIDs : 1;
    // If TRUE then DataIDs will be interpreted as-is.  If FALSE then the
    // data file of the parent sequence will be used as the data file for
    // the data items contained in this sequence (like bitmaps in bitmap
    // sequences).

  unsigned int reserved : 27;
    // For future use and to pad it out to a multiple of 4 bytes.

} LE_SEQNCR_SequenceChunkHeaderRecord, *LE_SEQNCR_SequenceChunkHeaderPointer;



// LE_CHUNK_ID_SEQ_GROUPING is a kind of sequence that groups together other
// sub-sequences that are contained within it as sub-chunks.  Typically used
// for a 2D animated sequence with lots of images and sounds as subsequences.
// After the fixed size structure come two runs of sub-chunks.  The first run
// of private chunks (ChunkID > 128) contains optional dimensionality,
// bounding box and transformation chunks.
// The second run is made of public chunks (ChunkID < 128) that contain a
// mixture of child sequences, special effects, 2D bitmaps, 3D models and
// sounds.  All the things in the second run must be sorted by start time,
// and preferably also high (larger integer) to low priority for those with
// the same start time.

typedef struct LE_SEQNCR_SequenceGroupingChunkStruct
{
  LE_SEQNCR_SequenceChunkHeaderRecord commonHeader;
} LE_SEQNCR_SequenceGroupingChunkRecord, *LE_SEQNCR_SequenceGroupingChunkPointer;



// LE_CHUNK_ID_SEQ_INDIRECT is a grouping kind that has the sub-sequences in
// some other data item (stored as a bunch of sequence chunks).  The advantage
// is that the sub-sequences don't get loaded into memory unless this sequence
// is visible on screen.  This is useful for avoiding loading of large chunks
// of a big scrolling world.  The fixed size data is followed by a run of
// optional subchunks for just dimensionality, bounding box and transformation.

typedef struct LE_SEQNCR_SequenceIndirectChunkStruct
{
  LE_SEQNCR_SequenceChunkHeaderRecord commonHeader;
  LE_DATA_DataId                      subsequenceChunkListDataID;
} LE_SEQNCR_SequenceIndirectChunkRecord, *LE_SEQNCR_SequenceIndirectChunkPointer;



// LE_CHUNK_ID_SEQ_2DBITMAP is just a bitmap.  It can be followed by the usual
// transformation and bounding box subchunks.  Both the bounding box (which
// has the side effect of positioning the bitmap within itself) and the
// transformations can place the origin at a useful place (like at the feet of
// a cartoon character), instead of the default top left corner.

typedef struct LE_SEQNCR_Sequence2DBitmapChunkStruct
{
  LE_SEQNCR_SequenceChunkHeaderRecord commonHeader;
  LE_DATA_DataId                      bitmapDataID;
} LE_SEQNCR_Sequence2DBitmapChunkRecord, *LE_SEQNCR_Sequence2DBitmapChunkPointer;



// LE_CHUNK_ID_SEQ_3DMODEL is a 3D object which includes geometry, textures
// and joint angles.  All the data is referenced by DataID so that several
// variations of the same model (by changing the joint angles only and reusing
// the model geometry) can be specified.  It typically will have bounding cube
// and positioning transformation subchunks after the fixed size data.  File
// names 1 to 3 correspond to the alternative external file for model, texture
// and joint settings if the respective DataID is LE_DATA_EmptyItem.  It can
// also have other subchunks including one that describes the position as the
// X, Y, Z, roll, pitch, yaw, scale that were used to create the position
// matrix - useful for editing the position in the dope sheet.  If several
// transform chunks are present, only the first one gets used.

typedef struct LE_SEQNCR_Sequence3DModelChunkStruct
{
  LE_SEQNCR_SequenceChunkHeaderRecord commonHeader;
  LE_DATA_DataId                      modelDataID;
    // The 3D model geometry is a hierarchical structure of coordinate
    // systems containing triangle strip meshes, with some joint skin
    // polygons that cross between coordinate systems.  It may also contain
    // default texture maps and joint angles depending on the implementation.
    // On the Playstation the model will contain stuff that the Gray Matter
    // engine can play.
  LE_DATA_DataId                      textureMapDataID;
    // DataID of the texture map indirect index table to use
    // (see LE_DATA_DataIndexTable).  It is separate from the model in case
    // you want to recolour the same model (useful for a bike race where each
    // rider wears different colours).
  LE_DATA_DataId                      jointPositionsDataID;
    // DataID of the joint settings.  This is usually specified as an array
    // of coordinate transformation matrices and matching node IDs that
    // identify nodes within the 3D geometry.  For dynamically animated
    // models, the joint settings could be in memory as an imaginary
    // data item.
} LE_SEQNCR_Sequence3DModelChunkRecord, *LE_SEQNCR_Sequence3DModelChunkPointer;



// LE_CHUNK_ID_SEQ_3DMESH is for a MESHX, which is used by the HMD
// 3D file format renderer.  Contains polygons and textures.

typedef struct LE_SEQNCR_Sequence3DMeshChunkStruct
{
  LE_SEQNCR_SequenceChunkHeaderRecord commonHeader;
  LE_DATA_DataId                      modelDataID;
    //All the pertinent info hangs off the meshx pointed at by the modelDataID.
} LE_SEQNCR_Sequence3DMeshChunkRecord, *LE_SEQNCR_Sequence3DMeshChunkPointer;



// LE_CHUNK_ID_SEQ_SOUND is for a sound effect.  The optional dimensionality
// and transformation subchunks can be used to override the inherited
// dimensionality.  Zero dimension sounds are played with manually set
// panning (see the LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING chunk).  2D sounds
// have panning derived from the position on the screen (it gets camera
// settings from the render slot that is doing 2D rendering for its parent
// chunk).  3D sounds turn on 3D spatial effects, relative to some 3D render
// slot's camera position.  You can also set the pitch and volume with
// appropriate subchunks, and even tweek them as time passes.

typedef struct LE_SEQNCR_SequenceSoundChunkStruct
{
  LE_SEQNCR_SequenceChunkHeaderRecord commonHeader;

  LE_DATA_DataId                      soundDataID;
    // If the DataID is LE_DATA_EmptyItem then an external file named
    // by a LE_CHUNK_ID_FILE_NAME_5 subchunk is used instead (useful
    // for spooling long sounds off the storage media).
} LE_SEQNCR_SequenceSoundChunkRecord, *LE_SEQNCR_SequenceSoundChunkPointer;



// LE_CHUNK_ID_SEQ_VIDEO is for a video stream is playing in a rectangular
// area (set by the bounding box) in the 2D animation world.  3D is supported
// indirectly by drawing to a texture bitmap.  For speed it can be drawing
// directly to the screen.  At every TimeMultiple the next frame of video
// will be fetched (decompressed onto the output bitmap at the appropriate
// coordinates) along with the audio.  The actual video file is in an
// external file named by a LE_CHUNK_ID_FILE_NAME_5 subchunk.

typedef struct LE_SEQNCR_SequenceVideoChunkStruct
{
  LE_SEQNCR_SequenceChunkHeaderRecord commonHeader;

  UNS8 drawSolid;
    // TRUE to draw it solid, FALSE for alpha/colour key modes.
    // For the new video player, when playing Indeo 5.0x videos,
    // with transparency encoded (effectively a 1 bit alpha channel,
    // which you can make using a bunch of pictures with alpha
    // (32 bits) and compress using VidEdit with the alpha option
    // on in the Indeo specific settings), and this flag set to
    // FALSE, it will only draw the solid parts, letting the stuff
    // behind show through (the stuff behind gets drawn every frame
    // too, even in solid mode, so have nothing behind it if you
    // want extra speed).

  UNS8 flipVertically;
    // TRUE to display your video upside down (compatible with
    // old library style videos), FALSE for normal way up.

  UNS8 alphaLevel;
    // ALPHA_OPAQUE100_0 for colour key mode (green stuff shows
    // through as transparent), rest are various levels of transparency.
    // Only works if drawDirectlyToScreen is FALSE.

  UNS8 enableVideo;
    // TRUE to check for a video stream in the file, FALSE if you want
    // to ignore any video (useful for playing just the sound track of
    // an AVI).

  UNS8 enableAudio;
    // TRUE to check for an audio stream, FALSE to ignore any audio.

  UNS8 drawDirectlyToScreen;
    // If TRUE then the image is decompressed directly to the screen or
    // work screen (depending on how render slots were set up).  If FALSE
    // then it goes through an intermediate temporary bitmap, which is
    // needed for effects like alphaLevel.  The old video player only works
    // with the intermediate bitmap.  To decompress directly to the screen
    // with no bitmap copying at all, set up the render slot with the
    // screen as both the working and output surface, and turn on this
    // flag.  To blow up a small video to full screen size efficiently, set
    // up the render slot with a separate working screen, run with this
    // flag on (it gets decompressed directly to the working screen, saving
    // a copy operation), and set the LE_GRAFIX_ScreenSourceRect and
    // LE_GRAFIX_ScreenDestinationRect to magnify your video so that it
    // fills the real screen (a fast blitter stretch will be done from the
    // work screen to the real screen).  To play a video in a 3D world, set
    // up a 2D render slot with the texture surface (use a GBM generic
    // bitmap so that you can also refer to the surface using a DataID) as
    // its working and output bitmap and play the video in that slot.

  UNS8 doubleAlternateLines;
    // Artificially doubles the image size using cheap and quick pixel
    // doubling techniques rather than scaling.  Pixels are doubled in
    // width but not height, instead alternate scan lines are filled
    // with black (making the image slightly darker).  The black is
    // only drawn when the video starts, so memory bandwidth during
    // playback is halved.  You should also specify that the bounding
    // box is twice as big as usual, by scaling your sequence by 2 in
    // X and Y.

  INT8 saturation;
    // Percentage of colour saturation.  0% is normal.  -100% is black and
    // white.  +100% is psychedelicly colourful.  Currently only works
    // with videos made with Intel Indeo 5.0 compression.

  INT8 brightness;
    // Percentage brightness, -100 is dark, 0 is normal, +100 is bright.
    // Needs to have the video made with Indeo compression.

  INT8 contrast;
    // Another percentage, -100 to +100, 0 is normal.

} LE_SEQNCR_SequenceVideoChunkRecord, *LE_SEQNCR_SequenceVideoChunkPointer;



// LE_CHUNK_ID_SEQ_CAMERA specifies a view into a 2D or 3D world.
// The camera's position identifies the center of what the camera
// sees.  In 3D the camera is at location (0,0,0), looking at (0,0,1)
// with an up direction vector of (0,1,0).  In 2D it is at (0,0) by
// default and up is in the decreasing Y direction (up is at the top
// of the screen).  Of course, these are all transformed by the parent
// transformations into actual world coordinates.  However, scaling is
// ignored, and set from the LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW
// subchunk, not the parent transformation.    That's so that scaling up
// a sequence containing a camera doesn't do funny things to the camera's
// view.  It defaults to 45 degrees for 3D and 1.0 for 2D cameras if
// not specified in the LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW chunk.
// However, rotation is based on how you have rotated the camera in the
// world.  Optional subchunks and tweekers can include transformations
// to move the camera around from the default origin position and to
// change the field of view.

typedef struct LE_SEQNCR_SequenceCameraChunkStruct
{
  LE_SEQNCR_SequenceChunkHeaderRecord commonHeader;

  TYPE_Coord3D nearClipPlaneDistance;
  TYPE_Coord3D farClipPlaneDistance;
    // These define the distance range which the camera will display things
    // in.  Things furthur or closer don't get drawn.  If using a Z buffer,
    // this is also the range which the Z buffer covers, so try to make it
    // cover the range of objects visible to the camera to get finer
    // accuracy in depth intersection.  Not used in 2D.

  UNS8 cameraLabel;
    // Identifies a particular camera, so that the renderer can switch to the
    // view from camera 3, and so on.  There can be several cameras in a scene,
    // but the render slot only shows the view from one of them.  Should
    // be non-zero (zero means no label, which would be useless).
    // Note that the label is the same one used for 3D hit boxes,
    // so avoid using the same label number as something else.
} LE_SEQNCR_SequenceCameraChunkRecord, *LE_SEQNCR_SequenceCameraChunkPointer;



// LE_CHUNK_ID_SEQ_PRELOADER is used to preload a data item and hold it in
// memory for as long as the preloader sequence lasts.  Typically the
// preloader sequence ends just before the actual sequence using the
// data starts.

typedef struct LE_SEQNCR_SequencePreloaderChunkStruct
{
  LE_SEQNCR_SequenceChunkHeaderRecord commonHeader;
  LE_DATA_DataId                      preloadDataID;
    // If the DataID is LE_DATA_EmptyItem then an external file named
    // by a LE_CHUNK_ID_FILE_NAME_5 subchunk is used instead (useful
    // for preloading an external file, not too useful for spooling).
} LE_SEQNCR_SequencePreloaderChunkRecord, *LE_SEQNCR_SequencePreloaderChunkPointer;



// LE_CHUNK_ID_SEQ_TWEEKER is used for modifying the parent sequence.
// For example, a sound chunk could have tweekers which modify the
// volume and panning as it plays.  3D models can have tweekers which
// modify the mesh to do mesh interpolation over time (la raison d'etre).
// It is always a zero dimensional chunk, not having position matrices
// or bounding boxes to save on CPU time, since it just modifies its
// parent sequence and uses the parent's data.  There are subchunks
// which identify what it is tweeking, for example moving things in 3D
// can have matrix subchunks (1 or 2 of them).  It is a sequence only
// because it needs to have its own clock and because there can be
// multiple tweekers active at once (one tweeker changing the volume
// on a sound while another moves it around in 2D or 3D space).

typedef struct LE_SEQNCR_SequenceTweekerChunkStruct
{
  LE_SEQNCR_SequenceChunkHeaderRecord commonHeader;

  UNS8                                interpolationType;
    // The kind of interpolation this uses, see LE_SEQNCR_InterpolationTypes
    // for a list of possible values (constant or linear so far).  This also
    // implies the number of data subchunks which control the tweek.

} LE_SEQNCR_SequenceTweekerChunkRecord, *LE_SEQNCR_SequenceTweekerChunkPointer;




/*****************************************************************************
 *****************************************************************************
 ** P R I V A T E   S U B C H U N K   S T R U C T U R E S                   **
 *****************************************************************************
 ****************************************************************************/

// LE_SEQNCR_CHUNK_ID_DIMENSIONALITY specifies the dimensionality
// of a chunk.  Normally the system can determine the dimensionality
// by the chunk type (bitmaps are 2D) or detecting transformation
// subchunks, so this is really only useful for abstract grouping chunks.

typedef struct LE_SEQNCR_DimensionalityChunkStruct
{
  UNS8  dimensionality;
    // Number of dimensions this thing uses.  Should be 0, 2 or 3.
} LE_SEQNCR_DimensionalityChunkRecord, *LE_SEQNCR_DimensionalityChunkPointer;



// LE_SEQNCR_CHUNK_ID_2D_XFORM type chunks are used for fancy 2D
// transformations.  It is a subchunk in a 2D sequence.  Useful for
// things like rotating bitmaps, or ones that shrink or get larger.

typedef struct LE_SEQNCR_2DXformChunkStruct
{
  TYPE_Matrix2D matrix;
} LE_SEQNCR_2DXformChunkRecord, *LE_SEQNCR_2DXformChunkPointer;



// LE_SEQNCR_CHUNK_ID_2D_OFFSET is a simpler kind of 2D transformation,
// just an offset from the origin, which is normally at the top left
// corner of the bitmap (the optional bounding box sequence subchunk
// can change the origin).

typedef struct LE_SEQNCR_2DOffsetChunkStruct
{
  TYPE_Point2D  offset;
} LE_SEQNCR_2DOffsetChunkRecord, *LE_SEQNCR_2DOffsetChunkPointer;



// LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET is a fancier way of
// positioning and resizing 2D sequences.  It is equivalent to
// doing the origin translation (move by negative of the origin point),
// then scale, rotate and offset transformations in that order.
// Squash and stretch can be done by scaling by different amounts
// in X and Y.  Unfortunately rotation doesn't visibly work since
// the bitmap drawing code can't do rotations (yet).

typedef struct LE_SEQNCR_2DOriginScaleRotateOffsetChunkStruct
{
  TYPE_Point2D  offset; // Movement of the whole thing to somewhere in space.
  //  Also offset is first field for easy conversion to LE_SEQNCR_CHUNK_ID_2D_OFFSET.
  TYPE_Point2D  origin; // Location of the origin, gets subtracted from object.
  TYPE_Scale2D  scaleX; // Separate scale factors for each dimension so that
  TYPE_Scale2D  scaleY; // you can do simple squash-and-stretch animation.
  TYPE_Angle2D  rotate; // Rotates clockwise for increasing values.
} LE_SEQNCR_2DOriginScaleRotateOffsetChunkRecord,
 *LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer;



// LE_SEQNCR_CHUNK_ID_3D_XFORM is used for fancy 3D transformations.
// It is a subchunk in a 3D sequence.

typedef struct LE_SEQNCR_3DXformChunkStruct
{
  TYPE_Matrix3D matrix;
} LE_SEQNCR_3DXformChunkRecord, *LE_SEQNCR_3DXformChunkPointer;



// LE_SEQNCR_CHUNK_ID_3D_OFFSET is for simple translational movement of
// 3D models.  Doesn't happen too often, but can save a few bytes now
// and then.

typedef struct LE_SEQNCR_3DOffsetChunkStruct
{
  TYPE_Point3D  offset;
} LE_SEQNCR_3DOffsetChunkRecord, *LE_SEQNCR_3DOffsetChunkPointer;



// LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET is an alternative way of
// specifying a 3D transformation by using more understandable individual
// values for moving and rotating.  It is equivalent to subtracting the
// origin point then doing the scale, roll, pitch, yaw, and offset
// transformations in that order.

typedef struct LE_SEQNCR_3DOriginScaleRotateOffsetChunkStruct
{
  TYPE_Point3D  offset; // Movement of the whole thing to somewhere in space.
  //  Also offset is first field for easy conversion to LE_SEQNCR_CHUNK_ID_3D_OFFSET.
  TYPE_Point3D  origin; // Location of the origin, gets subtracted from object.
  TYPE_Angle3D  roll;   // Sides of a boat going up and down.
  TYPE_Angle3D  pitch;  // Bow and stern of the boat going up and down.
  TYPE_Angle3D  yaw;    // Compass heading that the boat is pointing towards.
  TYPE_Scale3D  scaleX; // Separate scale factors for each dimension so that
  TYPE_Scale3D  scaleY; // you can do simple squash-and-stretch animation.
  TYPE_Scale3D  scaleZ;
} LE_SEQNCR_3DOriginScaleRotateOffsetChunkRecord,
 *LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer;



// LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX describes a rectangle that will contain
// the bitmap, stretched if needed to make it fit the rectangle.  The default
// bounding box for a 2D bitmap is the same size as the rectangle and has
// (0,0) at the top left corner.  By using a different rectangle, you can move
// the origin to somewhere more useful, like the center of the bitmap or at
// the feet of a cartoon character.  The origin is important since that is
// where rotation goes around and where scaling centers.

typedef struct LE_SEQNCR_2DBoundingBoxChunkStruct
{
  TYPE_Rect boundingRect;
} LE_SEQNCR_2DBoundingBoxChunkRecord, *LE_SEQNCR_2DBoundingBoxChunkPointer;



// LE_SEQNCR_CHUNK_ID_3D_BOUNDING_BOX describes a cube that will contain
// the 3D model or child objects.

typedef struct LE_SEQNCR_3DBoundingBoxChunkStruct
{
  TYPE_Point3D boundingCube[8];
    // The vertices are given in hypercube order.  0 for left, 1 for right,
    // 0 for bottom, 2 for top, 0 for near face and 4 for far one (since a
    // left handed coordinate system assumed (+Z goes into the screen)).
    // Index  Left/Right Bottom/Top  Near/Far
    // ======================================
    //   ZYX
    // 0=000  left       bottom       near
    // 1=001  right      bottom       near
    // 2=010  left       top          near
    // 3=011  right      top          near
    // 4=100  left       bottom       far
    // 5=101  right      bottom       far
    // 6=110  left       top          far
    // 7=111  right      top          far
    // Of course, the cube can be rotated, scaled and otherwise mangled so
    // that the corners are no longer at the top etc.  But it will be an
    // accurate bounding volume, unlike a sphere which can't handle being
    // squished in different axis or min/max axis values which don't like
    // being rotated.
} LE_SEQNCR_3DBoundingBoxChunkRecord, *LE_SEQNCR_3DBoundingBoxChunkPointer;



// LE_SEQNCR_CHUNK_ID_3D_BOUNDING_SPHERE gives you a different bounding shape.
// Rather than using a cube, you can use a sphere to give a bounding shape,
// but it will be less accurate than a box (encompasses more area in many
// situations), however it is faster to compute, though it won't work
// right if you have squashed it in one dimension more than in others
// (it will be an oval in that case and won't be represented correctly
// by the radius number).

typedef struct LE_SEQNCR_3DBoundingSphereChunkStruct
{
  TYPE_Coord3D radius;
} LE_SEQNCR_3DBoundingSphereChunkRecord, *LE_SEQNCR_3DBoundingSphereChunkPointer;



// LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE picks a particular mesh (same bunch
// of polygons but with different vertex coordinates) from an HMD model
// (which can have several meshes).  For example, you could animate a walking
// dog by having the meshes for different walk poses and moving between
// them.  There is also an interpolation feature which creates a tempoary
// intermediate mesh  by doing a weighted average of the positions of each
// vertex using the proportion to make it closer to one mesh or the other.

typedef struct LE_SEQNCR_3DMeshChoiceChunkStruct
{
  INT16         meshIndexA;
  INT16         meshIndexB;
    // Identifies which meshes to use from the file, indices start at 0.

  TYPE_Scale3D  meshProportion;
    // If meshProportion is 0.0 then meshIndexA will be used to select
    // the mesh to use.  If meshProportion is 1.0 then meshIndexB is used.
    // If it is inbetween then an interpolated mesh will be used (the meshes
    // better have the same number of points attached to the same polygons!).
    // You can also go negative or more than 1.0 for strange effects.  For
    // non-interpolated meshes just leave meshProportion at 0.0 and use
    // meshIndexA to choose the mesh.
} LE_SEQNCR_3DMeshChoiceChunkRecord, *LE_SEQNCR_3DMeshChoiceChunkPointer;



// LE_SEQNCR_CHUNK_ID_HIT_BOX_LABEL labels a sequence as being watched by
// the renderer for special handling.  Typically, the renderer will make
// a note of the sequence being under the mouse cursor or not.  Also, the
// library code can quickly find a labeled sequence by using the
// LE_SEQNCR_LabelArray global variable.  Note that the cameras also
// have a built-in label.

typedef struct LE_SEQNCR_LabelChunkStruct
{
  UNS8 labelNumber;
} LE_SEQNCR_LabelChunkRecord, *LE_SEQNCR_LabelChunkPointer;



// LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH changes the sound pitch, usually
// used in tweekers.  The value is in hertz, use zero to play the sound
// at its normal speed.

typedef struct LE_SEQNCR_SoundPitchChunkStruct
{
  UNS16 soundPitch;
} LE_SEQNCR_SoundPitchChunkRecord, *LE_SEQNCR_SoundPitchChunkPointer;



// LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME changes the sound volume.  The
// volume is expressed as a percentage, from 0 (quiet) to 100 (normal).

typedef struct LE_SEQNCR_SoundVolumeChunkStruct
{
  UNS8 soundVolume;
} LE_SEQNCR_SoundVolumeChunkRecord, *LE_SEQNCR_SoundVolumeChunkPointer;



// LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING changes the left to right
// panning in zero dimensional (monophonic) sounds.  The value is
// a kind of percentage, with -100 being full left, 0 center and
// +100 full right.

typedef struct LE_SEQNCR_SoundPanningChunkStruct
{
  INT8 soundPanning;
} LE_SEQNCR_SoundPanningChunkRecord, *LE_SEQNCR_SoundPanningChunkPointer;



// LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW changes the camera's field of
// view angle, making it act like a zoom lens or fisheye lens.

typedef struct LE_SEQNCR_FieldOfViewChunkStruct
{
  TYPE_Angle3D fieldOfView;
    // For 3D it is the field of view angle in radians.  This gets used
    // to scale the world coordinates into screen pixels, so a large angle
    // gives you a fish eye lens and a small one gives you a telephoto
    // lens effect.  In 2D it is a scale factor which zooms in or out
    // (larger zooms in, smaller zooms out).  In 2D a value of 1.0 gives
    // you a 1 to 1 pixel ratio, each pixel in bitmaps drawn to the screen
    // shows up as a screen pixel (assuming that the bitmap itself isn't
    // stretched).
} LE_SEQNCR_FieldOfViewChunkRecord, *LE_SEQNCR_FieldOfViewChunkPointer;

/* End of structure packing for external file interchangability. */
#pragma pack(pop)



/*****************************************************************************
 *****************************************************************************
 ** R U N T I M E   T R E E   D A T A   S T R U C T U R E S                 **
 *****************************************************************************
 ****************************************************************************/

// First some forward declarations so that we can use the
// various pointer types in the record definitions.

typedef struct LE_SEQNCR_RuntimeInfoStruct
  LE_SEQNCR_RuntimeInfoRecord, *LE_SEQNCR_RuntimeInfoPointer;

typedef struct LE_SEQNCR_Runtime2DInfoStruct
  LE_SEQNCR_Runtime2DInfoRecord, *LE_SEQNCR_Runtime2DInfoPointer;

typedef struct LE_SEQNCR_Runtime3DInfoStruct
  LE_SEQNCR_Runtime3DInfoRecord, *LE_SEQNCR_Runtime3DInfoPointer;

typedef struct LE_SEQNCR_RuntimeVideoInfoStruct
  LE_SEQNCR_RuntimeVideoInfoRecord, *LE_SEQNCR_RuntimeVideoInfoPointer;

typedef struct LE_SEQNCR_RuntimeTweekerInfoStruct
  LE_SEQNCR_RuntimeTweekerInfoRecord, *LE_SEQNCR_RuntimeTweekerInfoPointer;

typedef struct LE_SEQNCR_Runtime3DCameraInfoStruct
  LE_SEQNCR_Runtime3DCameraInfoRecord, *LE_SEQNCR_Runtime3DCameraInfoPointer;


typedef enum LE_SEQNCR_CallbackOperationEnum
{
  LE_SEQNCR_CALLOP_STARTING,
    // If you specified a callback function when you started a sequence,
    // it will get called when the sequence starts (after it has been
    // fully initialised though not updated).

  LE_SEQNCR_CALLOP_POSITION_CALC,
    // When you get this operation code, the sequencer wants you to update
    // the position of your sequence (sequence to parent transformation),
    // and set the needsPositionRecalc flag and needsRedraw flag if you
    // move the sequence (the sequencer takes care of the children and
    // ancestors redraw and position calc flags automatically).  At this
    // time the sequence's clock has been updated, expired children have been
    // removed and new children have been added.  However, the sequence to
    // world transformation hasn't been updated (it needs your position
    // change first).

  LE_SEQNCR_CALLOP_END_TIME_HIT,
    // This just informs you that the sequence hit the end of its time.
    // It isn't safe to change anything in the sequence tree since this
    // sequence is being updated.  But you can queue up new commands.

  LE_SEQNCR_CALLOP_DELETING,
    // This is the last callback you will get from this sequence.  It is
    // about to be deallocated.  You get this callback for all sequences,
    // including ones that are being manually stopped, etc, not just the
    // ones which hit the end of time.

  LE_SEQNCR_CALLOP_MAX
}
#if !CE_ARTLIB_UseEnumDataTypes
; typedef UNS8  /* For compilers which don't like enums, use an int instead. */
#endif
LE_SEQNCR_CallbackOperation;
  // Various different things that the callback function can be called on to do.


typedef void (*LE_SEQNCR_UserCallbackFunctionPointer)
(LE_SEQNCR_RuntimeInfoPointer RuntimeInfoPntr, LE_SEQNCR_CallbackOperation Opcode);
  // The user of the sequencer can provide a callback function that will
  // be called when updating each sequence (just before children are
  // updated).  The runtime info has everything you need (Sequence DataID,
  // priority, clock, position) and you can change the position and
  // visibility.
  // This is mostly useful for sequences that have physics or other strange
  // properties.  In 2D worlds this could be a function that moves a sequence
  // with many sub-sequences around to simulate a scrolling parallax plane.
  // Keep in mind that this is called by the animation thread, not the game
  // thread, so watch out (it is best to avoid making any library calls and
  // only update the position information).



// This record is used for all sequences and is used to build an
// in-memory tree structure of all active sequences.

struct LE_SEQNCR_RuntimeInfoStruct
{ // Note that field order is optimised for word alignment packing,
  // not comprehension.  Sorry about that.

  LE_SEQNCR_RuntimeInfoPointer child;
  LE_SEQNCR_RuntimeInfoPointer sibling;
  LE_SEQNCR_RuntimeInfoPointer parent;
    // These link together the run-time records into a tree structure.
    // Also, the siblings are arranged in order of increasing priority
    // numbers (ones in back come before ones that go in front).
    // Also, the first one (the child pointer) is used to link together
    // the free nodes list, and the parent is set to NULL when a node
    // is free (or it is the magic root node); nodes that have a non-NULL
    // parent are definitely in the tree.

  INT32 sequenceClock;
    // The local sequence clock time in ticks, can be negative if this
    // sequence is delaying before it starts up.  This is a boundary
    // value not a tick number, so all things <= sequenceClock are in the
    // past while things > sequenceClock are in the future, note the equals.
    // The particular frames to be displayed are controlled directly by this
    // time value.  It is incremented as time passes, in steps of TimeMultiple
    // ticks, though it may not increment as fast as the global clock if it
    // is not skipping frames when it falls behind.  When the sequence loops
    // back to an earlier frame, this gets updated to set the correct sequence
    // relative time.  For example, looping back to the beginning means that
    // the sequencer would set this to zero.  If you loop back to the last
    // frame, this would be set to the sequence time corresponding to the last
    // frame.

  INT32 endTime;
    // The time when this sequence ends.  Depending on the ending action,
    // it may loop back or stop or do something else.  If it holds the
    // last frame then sequenceClock stays equal to endTime.  In local
    // sequence clock time units.

  INT32 offScreenStopTime;
    // When the sequence clock reaches this time, the sequence will be
    // stopped.  Zero means this feature is disabled.  This gets set to
    // the current sequence clock plus a delay factor (specified elsewhere)
    // when the sequence goes from on-screen to off-screen.  It is ignored
    // while the sequence is on-screen.  It is useful for keeping sequences
    // running that may be off-screen for a short while - like that fire
    // hydrant that starts spouting water when it appears.  If you leave the
    // screen and come back, it will restart its spouting, but with this timer
    // it can be still spouting when you come back.  Also, the sound for the
    // off-screen thing will keep on going while it is off screen.

  INT32 parentTimeOfLastUpdate;
    // The parent's clock time when this sequence was last updated.  This is
    // useful for telling exactly how much time has elapsed since the last
    // update so that you can advance the sequence clock appropriately (but
    // maybe limited to the time multiple if not skipping frames).  An
    // INT32_MIN value means that this sequence has never been updated before.
    // All child sequences are ignored until their parent updates.

  INT32 parentStartTime;
    // Copied from the LE_SEQNCR_SequenceChunkHeaderRecord.

  LE_DATA_DataId dataID;
    // The DataID that this sequence came from.  The philosophy is to open
    // the DataID and copy out all relevant information into this runtime
    // record and then close it.  That way the DataID doesn't have to be
    // used any more for small (single bitmap) sequences.  For sequences
    // with lists of children, or inline bitmaps, the DataID does get used
    // later on.  Of course, the render slot may want to have a look at
    // the sequence too, or you may just want to debug it.  Besides, this
    // is needed to identify a sequence (DataID + Priority identify a
    // running sequence to the user).

  UNS32 dataOffset;
    // Byte offset into the dataID where the sequence header starts.  Note
    // that since a sequence can contain subsequences, the runtime header
    // (this structure) for the children will have the same DataID as the
    // parent but the offset will be different.

  LE_DATA_DataId  childSequencesDataID;
  UNS32           childSequencesOffset;
    // For sequences that have subsequences (used to just be grouping, now
    // that we have tweekers, all sequences can have subsequences), these
    // point to the place where the next subsequence starts (though there
    // may be some intervening non-sequence chunks).  The sequence data
    // is in the data item identified by childSequencesDataID, or if the
    // childSequencesDataID is equal to LE_DATA_EmptyItem then there is
    // no child sequence data at all.  So, most things inside a bigger
    // sequence will have the same DataID number for their dataID and
    // childSequencesDataID fields, the exception being indirect sequences
    // which have children stashed elsewhere and only their initial values
    // (like starting location) come from subchunks in the original dataID.
    // The position moves forwards as time advances, so that it always
    // points near the next sequence to be started (the sequence start
    // time will be in the future).  If there are no more subsequences,
    // it points just past the end of the data.  You can tell where the
    // end of the subchunks is by looking for the end of the parent
    // sequence chunk.  If it is an indirect data item then the subchunks
    // end at the end of the indirect data item.  Also, while the sequence
    // is in use, an AddRef is outstanding for the childSequencesDataID.

  LE_DATA_DataId contentsDataID;
    // The DataID of the thing being shown by the sequence, the particular
    // thing depends on the kind of sequence (a bitmap for 2D, a wave file
    // for sounds, a 3D model for 3D graphics).  The ID is often in a data
    // file but sometimes this data item is created when the sequence
    // starts and destroyed when the sequence ends.  There is also a data
    // disposal option for automatically deallocating this data item when
    // the sequence ends (see LE_SEQNCR_DataDisposalOptions).

  union auxiliaryDataUnion
  {
    struct  audioStruct
    {
      UNS8  volume;   // From 0 to 100 percent.
      INT8  panning;  // From -100 (left) to +100 (right), only in 0D sound.
      UNS16 pitch;    // Number of Hertz to play back the sound at, 0 for default.
    } audio; // Also for sound part of video.

    struct tweekerStruct
    {
      LE_SEQNCR_InterpolationTypes  interpolationType;
      LE_SEQNCR_PrivateChunkIDs     interpolatedChunkID; // Zero if uninitialised.
    } tweeker;

    TYPE_Angle3D cameraFieldOfView; // Also have near and far clip in 3D, in stream.
  } aux;
    // Specific kinds of sequences use this for their own purposes.
    // Camera sequences put the camera scale factor in here, simple
    // sounds store pan and volume, etc.

  LE_SEQNCR_UserCallbackFunctionPointer callBack;
    // This is a user provided function that is called just before the
    // sequence is updated.  See the function typedef for details.
    // NULL for no callback.

  void *userData;
    // A pointer sized value (32 bits) that the callback function can use for
    // any purpose.  The engine doesn't use it.  Perhaps you may use it to
    // flag certain sequences?  It gets initialised to zero when the sequence
    // starts, unless you specified an initial value.

#if CE_ARTLIB_SeqncrUseSerialNumbers
  LE_CHUNK_SerialNumber serialNumber;
    // The serial number from the LE_CHUNK_ID_DOPE_DATA subchunk,
    // if any, or zero if no serial number is available.
#endif

  union DimensionSpecificStruct {
    void                           *d0;
    LE_SEQNCR_Runtime2DInfoPointer  d2;
    LE_SEQNCR_Runtime3DInfoPointer  d3;
  } dimensionSpecificData;
    // A pointer into an array storing dimension specific data for this
    // sequence.  In 2D it is a bounding box, a 2D coordinate transformation
    // and some other settings, see LE_SEQNCR_Runtime2DInfoRecord for details.
    // 3D is similar, except that this is a pointer into a different array.
    // There is no extra info for 0D sequences, or when this field is NULL.
    // The Dimensionality field specifies which kind of record goes here.

  union StreamSpecificStruct {
    void                                 *voidPntr;
    LE_SOUND_AudioStreamPointer           audioStream;
    LE_SEQNCR_RuntimeVideoInfoPointer     videoStream;
    LE_SEQNCR_RuntimeTweekerInfoPointer   tweekData;
    LE_SEQNCR_Runtime3DCameraInfoPointer  cameraData;
  } streamSpecificData;
    // A pointer to a dynamically allocated record describing the video
    // or other stream type of thing (like sound AVIs) being played by
    // this sequence.  NULL if no stream open.  Allocated using the
    // library memory pool.

  void *renderSlotData [CE_ARTLIB_RendMaxRenderSlots];
    // Each render slot can store some private data here, perhaps just a
    // 32 bit value, or maybe a pointer to some private allocated stuff,
    // such as the previous screen bounding box of a 2D bitmap.

  LE_REND_RenderSlotSet renderSlotSet;
    // Which render slots this sequence appears in.  Can be more than one
    // (typically a 2D sequence will be in the slot that draws 2D animation
    // on the screen and the audio output slot).  Almost useless to have none.

  UNS8 timeMultiple;
    // Copied from LE_SEQNCR_SequenceChunkHeaderRecord.  Controls how often
    // the sequencer updates this sequence.  Stay at end sequences optionally
    // can get this set to 255 to cut down on useless updates.

  LE_SEQNCR_EndingAction endingAction;
    // Copied from LE_SEQNCR_SequenceChunkHeaderRecord.

  UNS8 dimensionality;
    // Number of spatial dimensions this sequence exists in.  Can be 0 for
    // grouping sequences, 2 for 2D animation, 3 for 3D animation.  This also
    // specifies the type of extra dimension specific runtime info; 2 means
    // use a LE_SEQNCR_Runtime2DInfoRecord for the dimensionSpecificData.

  LE_REND_RenderSlotSet onScreen;
    // This is updated for sequences that have a bounding box to show which
    // renderers are currently displaying the sequence on screen.  Ones with
    // no bounding box just leave the value here alone (initially set to on
    // screen).  Sequences with data items (bitmap or 3D model) that haven't
    // been loaded yet (delayed load feature) should be updated as if they
    // are visible, so that they don't get incorrectly stopped.  Sequences
    // with their parent seqeuence off-screen are destroyed (stopped) to
    // save memory and computation (but the parent stays alive, unless its
    // parent is off-screen and so on).

  UNS16 priority;
    // The front to back priority for 2D sequences (0 is at the back and gets
    // drawn first, larger numbers are drawn in front).  In 3D only the top
    // half bits of the top level sequences (not child ones) are used for
    // sorting out drawing batch groups, the lower ones just distinguish
    // between different sequences (sorting is relevant on systems that
    // don't have a Z-buffer, like the Sony Playstation).

  UNS8 labelNumber;
    // Labels this sequence for special handling.  A label of zero means
    // that this isn't labeled.  All cameras have a label (so you can pick
    // one for viewing the scene from).  There is also a chunk that specifies
    // a label for use with hit boxes.  Also see the LE_SEQNCR_LabelArray.

  INT8 watchIndex;
    // Which watch index is being used to monitor this sequence.
    // Set to -1 if not being watched.  It should also check the
    // watch array to see if the DataID and priority still match
    // whenever it does an update.

  LE_SEQNCR_SequenceType sequenceType;
    // What kind of sequence this is.  Copied out of the sequence chunk header.

  unsigned int dropFrames : 1;
    // Copied from LE_SEQNCR_SequenceChunkHeaderRecord, though the user can
    // override it when starting a sequence.

  unsigned int lastUse : 1;
    // Copied from LE_SEQNCR_SequenceChunkHeaderRecord.

  unsigned int scrollingWorld : 1;
    // Copied from LE_SEQNCR_SequenceChunkHeaderRecord.

  unsigned int paused : 1;
    // TRUE if this sequence is paused.  The sequenceClock doesn't increment.

  unsigned int redrawAtEveryUpdate : 1;
    // If this is TRUE then the sequence will be redrawn at every update even
    // if it hasn't changed.  This is usually used for video sequences and
    // special 2D bitmap items that refer to an external DirectDraw surface
    // that is changing frequently.  Mostly useless in 3D things since they
    // update every frame anyways, except perhaps animated textures.

  unsigned int needsRedraw : 1;
    // If TRUE then this sequence's contents will be redrawn and this flag
    // will be cleared after it is done by all render slots.  If a 2D image
    // hasn't changed even though time is passing, the engine will leave
    // this FALSE.  However, if the parent sequence moves around, or this
    // one changes, or a child changes then it gets set to TRUE.  In other
    // words, child changes propogate up to all ancestors (but leave siblings
    // and cousins unchanged).  Parent changes propogate down to all children.

  unsigned int needsPositionRecalc : 1;
    // If TRUE then the coordinates for this sequence will be recalculated,
    // to be precise the sequenceToWorldTransformation is regenerated.
    // When a sequence moves, this gets set in it and all its children.
    // Later on the marked nodes get their sequence to world transformation
    // recalculated (and the flag gets cleared at that time).  This also
    // implies that the sequence to world transformation is invalid when
    // this flag is TRUE.

  unsigned int needsReEvaluation : 1;
    // This flag gets set to TRUE when a sequence or its children change.
    // For example, a child moves around.  This makes it set the
    // needsReEvaluation flag in all its parents and ancestors up to the
    // root.  Then the next update (or partial update if busy processing
    // unchained commands) will go through the parents to update the
    // children, though it normally would have stopped at the already
    // updated parents.

  unsigned int hasTweekerChildren : 1;
    // TRUE if this sequence has tweekers as direct children, which
    // means that the tweekers have to have a special update before
    // the parent sequence position is calculated (normally children
    // get done after the parent is done).

  unsigned int absoluteDataIDs : 1;
    // If TRUE then DataIDs will be interpreted as-is.  If FALSE then the
    // data file of the parent sequence will be used as the data file for
    // the data items contained in this sequence (like bitmaps in bitmap
    // sequences).

  unsigned int unrefSequenceDataWhenDone : 1;
    // When a grouping sequence ends, it can optionally remove references
    // to all data items in the sequence and also unload the data items
    // (as the disposeSequenceDataWhenDone directs).  These are the items
    // listed in the sequence's chunks, not just the ones currently in
    // the runtime tree.  Non-grouping sequences (raw bitmaps etc) just
    // unload / unref their single DataID.

  unsigned int disposeSequenceDataWhenDone : 2;
    // What to do with the data used by the sequence when it is done.
    // This is really a LE_SEQNCR_DataDisposalOptions value, which can
    // tell the system to unload the sequence's associated data (bitmaps
    // and such things mentioned in the group's chunk list) or change
    // them to low memory priority, or do nothing.  Normally this option
    // is only specified on grouping sequences since they control the
    // life and death of their subsiduary bitmaps and sounds.  However,
    // it may also be useful for stand-alone bitmaps and sounds.

  unsigned int drawSelectionBox : 1;
    // If this is TRUE then the sequence will be drawn with a rectangle
    // around it to show that it is selected.  Only works for sequences
    // which get drawn.  The initial value is obtained from the
    // LE_CHUNK_ID_DOPE_DATA subchunk, if present (otherwise off).
};



// The extra info for 2D sequences that doesn't fit in the more generic
// LE_SEQNCR_RuntimeInfoRecord.  There is a separate array of these
// records, which are allocated as needed.

struct LE_SEQNCR_Runtime2DInfoStruct
{
  TYPE_Rect boundingBox;
    // The whole sequence and its children fit in this box, uses the sequence's
    // coordinate system so you will have to transform it to get screen
    // coordinates.  This is dynamically recalculated for grouping sequences
    // as children are added and removed and move around.  Useful for
    // determining if the sequence is on or off screen.  If none is explicitly
    // provided, it is the size of the bitmap and has (0,0) at the top left
    // corner.  The bitmap will be stretched and rotated to fit in this box,
    // after the box has been transformed to screen coordinates.

  // LE_DATA_DataId bitmapOrSoundDataID - moved to contentsDataID.
    // The DataID to use for the bitmap.  This normally takes the data file
    // from the sequence's file and the index from the relevant chunks.
    // It also means that you can change the bitmap at runtime.  For 2D
    // sounds this contains the Sound's ID instead.

  BOOL tweekerTransformNonIdentity;
  BOOL sequenceToParentTransformNonIdentity;
    // To avoid wasting time doing matrix multiplications, these flags
    // have to be turned on to include the related matrix in the
    // calculations of the sequence's position.

  LE_SEQNCR_Transform2D tweekerTransformation;
    // Movement, scaling and rotation which is applied by tweekers to modify
    // the sequence.  Applied before the sequenceToParentTransformation so
    // that it is relative to initial position and user program generated
    // movements of the overall sequence.

  LE_SEQNCR_Transform2D sequenceToParentTransformation;
    // Movement, scaling and rotation which will be used to transform all of
    // the items in the sequence from sequence to parent coordinates.  This
    // is set by the sequence positioning information when the sequence
    // starts up.  You can change it if you want to move the sequence under
    // program control.

  LE_SEQNCR_Transform2D sequenceToWorldTransformation;
    // Movement, scaling and rotation which will be used to transform all of
    // the items in the sequence to world coordinates (one step before the
    // render slot camera offset transforms it to screen coordinates).  This
    // is computed by applying all the parent sequence transformations.
    // The computation is done after the UpdateCallbackFunctions have been
    // called, so it contains the transformation from the previous frame
    // during the UpdateCallbackFunctions operation.  If needsPositionRecalc
    // is TRUE then it will be recalculated.

  TYPE_Coord2D offScreenStopDistance;
    // Distance in screen coordinates that an off-screen sequence can go off
    // screen (actually distance from all the active cameras that are
    // displaying it) before being stopped.  For example, in the pony game,
    // things should be removed from the scrolling world when they are more
    // than half a screen width off the edge.  This is a distance equal to
    // the screen width away from the screen center.  This feature is
    // disabled when the distance is zero.
};



// The extra info for 3D sequences that doesn't fit in the more generic
// LE_SEQNCR_RuntimeInfoRecord.  There is a separate array of these
// records, which are allocated as needed.

struct LE_SEQNCR_Runtime3DInfoStruct
{
  TYPE_Point3D boundingBox [8];
    // The whole sequence and its children fit in this box, uses the sequence's
    // coordinate system so you will have to transform it to get screen
    // coordinates.  This is dynamically recalculated for grouping sequences
    // as children are added and removed and move around.  Useful for
    // determining if the sequence is on or off screen.

  // LE_DATA_DataId modelOrSoundDataID - moved to contentsDataID;
    // The DataID to use for the 3D model.  This normally takes the data file
    // from the sequence's file and the index from the relevant chunks.
    // It also means that you can change the model at runtime.  For 3D
    // sounds this contains the Sound's ID instead.

  LE_DATA_DataId textureMapDataID;
    // This identifies the texture mapping data to use for colouring
    // the model.  LE_DATA_EmptyItem means use the default mapping
    // which is in the model data.

  LE_DATA_DataId jointPositionsDataID;
    // Joint angle and positioning data to use.  LE_DATA_EmptyItem
    // means use the default positions which are in the model data.

  BOOL tweekerTransformNonIdentity;
  BOOL sequenceToParentTransformNonIdentity;
    // To avoid wasting time doing matrix multiplications, these flags
    // have to be turned on to include the related matrix in the
    // calculations of the sequence's position.

  INT16 meshIndexA;
  INT16 meshIndexB;
  TYPE_Scale3D meshProportion;
    // Which mesh to use if there are several.  If meshProportion is
    // 0.0 then meshIndexA will be used to select the mesh to use,
    // for HMD data which has multiple meshes in the same file, the
    // first (default) mesh is number zero, negative mesh numbers
    // may mean something in the future.  If meshProportion is 1.0
    // then meshIndexB is used.  If it is inbetween then an
    // interpolated mesh will be used (the meshes better have the
    // same number of points and polygons!).  You can also go negative or
    // more than 1.0 for strange effects.  For non-interpolated meshes
    // just leave meshProportion at 0.0 and use meshIndexA to choose
    // the mesh.

  LE_SEQNCR_Transform3D tweekerTransformation;
    // Movement, scaling and rotation which is applied by tweekers to modify
    // the sequence.  Applied before the sequenceToParentTransformation so
    // that it is relative to initial position and user program generated
    // movements of the overall sequence.

  LE_SEQNCR_Transform3D sequenceToParentTransformation;
    // Movement, scaling and rotation which will be used to transform all of
    // the items in the sequence from sequence to parent coordinates.  This
    // is set by the sequence positioning information when the sequence
    // starts up.  You can change it if you want to move the sequence under
    // program control.

  LE_SEQNCR_Transform3D sequenceToWorldTransformation;
    // Movement, scaling and rotation which will be used to transform all of
    // the items in the sequence to screen coordinates.  This is computed by
    // applying all the parent sequence transformations.  The computation is
    // done after the UpdateCallbackFunctions have been called, so it contains
    // the transformation from the previous frame during the
    // UpdateCallbackFunctions operation.  If needsPositionRecalc
    // is TRUE then it will be recalculated.
};



// The extra information used by a video stream.  There are three variations,
// depending on if you are using the  old Artech video system that uses
// AVIFile to access AVI format video data (doesn't work under Win2000) or
// the newer one which uses the ICM (Image Compression Manager) to decompress
// directly to the screen, with alpha transparency (not just green tests) and
// better buffering, or the 3rd party Bink video player ($4000 license per
// game, but quality better than Indeo).

struct LE_SEQNCR_RuntimeVideoInfoStruct {
#if CE_ARTLIB_EnableSystemVideo || CE_ARTLIB_EnableSystemBink
  LE_DATA_DataId        intermediateBitmapID;
  BITMAPV4HEADER        bitmapInfo;
#endif
#if CE_ARTLIB_EnableSystemVideo
  LE_VIDEO_VideoHandle  videoHandle;
#endif
#if CE_ARTLIB_EnableSystemBink
  HBINK                 videoHandle;
  UNS32                 outputPitch;
  UNS32                 outputDepthFlags;
#endif
#if USE_OLD_VIDEO_PLAYER
  PAVISTREAM            audioStream;
  AVISTREAMINFO         audioStreamInfo;
  BOOL                  audioStreamingStarted;
  UNS32                 currentAudioFrameNumber;
  PAVIFILE              fileHandle;
  BOOL                  jumpCutAtNextFeeding; // TRUE to flush audio buffers so it jumps to the time in currentAudioFrameNumber.
  INT32                 alternativeDecisionFrame; // Negative if no decision frame.
  INT32                 alternativeJumpToFrame; // Frame to jump to if alternative taken.
  BOOL                  takeAlternative; // TRUE if the alternative should be taken.
  UNS32                 lastAudioFeedTime;
  UNS32                 numberOfAudioFrames;
  LE_SOUND_BufSndHandle soundBuffer;
  PGETFRAME             videoGetFrameFunction;
  PAVISTREAM            videoStream;
  AVISTREAMINFO         videoStreamInfo;
  BOOL                  videoStreamingStarted;
  #if CE_ARTLIB_EnableDebugMode
  UNS32                 firstVideoShownAtTime; // For statistic collection, see videoFramesActuallyRendered.
  UNS32                 videoFramesActuallyRendered; // Number of frames drawn on the screen.
  #endif
#endif // CE_ARTLIB_EnableSystemVideo

  // Common to old and new versions of the player code:

  UNS16 alphaLevel;
    // ALPHA_OPAQUE100_0 for colourkey mode, rest are transparent.

  UNS32 currentVideoFrameNumber;
    // Frame number from latest rendering.  Used for sequence timing (rather
    // than the clock) and also used to check if the frame has changed since
    // the rendering.

  UNS32 timeOfLastFrameNumberChange;
  UNS32 frameNumberAtLastChange;
    // The global clock and frame number at the time when it last was
    // seen to change.  Used for detecting when the video has stalled
    // (needs audio feeding to resume but not being fed since the
    // frame number isn't changing since there is no change in audio).

  BOOL doubleAlternateLines;
    // TRUE to use the double scan lines interleaved with black trick.

  BOOL drawDirectlyToScreen;
    // TRUE to avoid temporary intermediate bitmaps.  The old player
    // doesn't support this.  See LE_SEQNCR_SequenceVideoChunkRecord
    // for more details.

  BOOL drawSolid;
    // TRUE to draw it solid, FALSE for alpha/colour key modes or
    // transparency mode if using the new player and Indeo encoding.
    // See LE_SEQNCR_SequenceVideoChunkRecord for more details.

  BOOL enableAudio;
    // TRUE to check for an audio stream.  Updated to reflect what's playing.

  BOOL enableVideo;
    // TRUE to check for a video stream.  Also updated to reflect
    // playing video or not playing it (if you ask for video and
    // the movie doesn't have video, this gets set to FALSE).

  BOOL flipVertically;
    // TRUE to display your video upside-down.

  UNS32 numberOfVideoFrames;
    // Total number of frames in the movie.

  INT8 saturation;
  INT8 brightness;
  INT8 contrast;
    // Percentage values from -100 to +100, 0 is normal.

  float videoFramesPerSecond;
    // As specified in the AVI file.  Expect not quite right
    // values like 14.94 instead of 15.0 (due to NTSC TV not
    // being exactly 60hz).

  RECT videoRectangle;
    // Size of the video image, left and top are usually zero.
};



// The extra information used by tweekers.

struct LE_SEQNCR_RuntimeTweekerInfoStruct
{
  TYPE_Scale3D currentProportion;
    // Proportion between 0.0 (when the tweeker starts) and 1.0 (corresponding
    // to ending time) to interpolate between the values for the data to be
    // tweeked.  Initialised to a large negative number so that the first
    // update gets done, then updates are only done when the proportion
    // related to the current time is different than this value.

  UNS32 offsetToInitialDataChunk;
  UNS32 offsetToFinalDataChunk;
    // Offset within the LE_SEQNCR_RuntimeInfoRecord.dataID to where
    // the subchunks with the data to be interpolated are.  These are
    // usually sequence private chunks like LE_SEQNCR_CHUNK_ID_2D_OFFSET.
    // Zero if no data available.  Otherwise points to the start of
    // the fixed part data (just past the 4 byte subchunk header).
};



// The extra information used by 3D cameras (also see aux.cameraFieldOfView).
// Note that 2D cameras don't have one of these records allocated.

struct LE_SEQNCR_Runtime3DCameraInfoStruct
{
  TYPE_Coord3D nearClipPlaneDistance;
  TYPE_Coord3D farClipPlaneDistance;
    // Copied from the LE_CHUNK_ID_SEQ_CAMERA chunk, see comments there.
};



/************************************************************/
/* GLOBAL VARIABLES - see L_Seqncr.c for details            */
/*----------------------------------------------------------*/

extern LI_REND_AbstractRenderSlotPointer
  LI_SEQNCR_RenderSlotArray [CE_ARTLIB_RendMaxRenderSlots];
  // An array of pointers to the active render slots.  NULL for slots which
  // are closed.  The renderer takes care of allocating and deallocating
  // its structures, just the pointer gets stored here.

extern LE_SEQNCR_RuntimeInfoPointer LE_SEQNCR_LabelArray [CE_ARTLIB_SeqncrMaxLabels];

extern LE_SEQNCR_WatchRecord volatile LE_SEQNCR_WatchArray [CE_ARTLIB_SeqncrMaxWatches];
  // Updated with the status of sequences you are watching.  When a sequence
  // starts, it checks to see if it is mentioned in this array.  If it is, it
  // notes down the index internally and updates that entry whenever it changes.
  // If it tries to do an update and finds a different DataID/Priority in its
  // index entry, it stops doing updates.

#if CE_ARTLIB_EnableSystemMouse
extern LE_DATA_DataId LE_SEQNCR_MouseGroupingSequenceDataID;
extern UNS16 LE_SEQNCR_MouseGroupingSequencePriority;
  // Identifies the grouping sequence used for the mouse.  The DataID is
  // -1 (LE_DATA_EmptyItem) if the grouping sequence runtime DataID
  // hasn't been created yet by the LE_SEQNCR_InitialiseMouseGroupingSequence
  // function.
#endif // CE_ARTLIB_EnableSystemMouse

#if CE_ARTLIB_SeqncrMaxRecolourRanges > 0
extern LE_SEQNCR_AutoRecolourRecord LE_SEQNCR_AutoRecolourRangesArray [CE_ARTLIB_SeqncrMaxRecolourRanges];
  // This array of colour maps and DataID ranges for each map is used for
  // recolouring TAB style bitmaps before they are displayed, if they
  // need to be recoloured.  See the Struct for more details.
#endif

extern CRITICAL_SECTION LI_SEQNCR_UpdateCycleCriticalSection;
extern LE_SEQNCR_RuntimeInfoPointer volatile LI_SEQNCR_CommonRuntimeTreeRoot;
  // For use by render slots and other things that need to know if the sequencer
  // is running and if it is in the middle of an update.  If the sequencer is
  // not running then LI_SEQNCR_CommonRuntimeTreeRoot will be NULL and the
  // critical section will be uninitialised.  See L_Seqncr.c for details.



/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

// Internal system stuff (LI_*), user programs shouldn't call these.
// LE_ are exported library routines that you are free to call.

extern void LI_SEQNCR_InitSystem (void);
extern void LI_SEQNCR_RemoveSystem (void);
extern void LI_SEQNCR_TimerTick (void);
extern void LE_SEQNCR_DoUpdateCycle (UNS32 TimeIncrement);
extern void LI_SEQNCR_RemoveSequencesInRenderSlotSet (
  LE_REND_RenderSlotSet RenderSlotSetToDelete);
extern BOOL LI_SEQNCR_GlueFeedAndDrawVideo (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
  BITMAPINFOHEADER *IntermediateBitmapInfoPntr, BYTE **IntermediateBitmapBitsPntrPntr,
  int OffsetX, int OffsetY, BYTE *ScreenBitsPntr);


// Some global engine control and debug functions.

extern void LE_SEQNCR_SuspendEngine (BOOL SuspendIt);
extern void LE_SEQNCR_EngineTimeMultiplier (UNS8 TimeMultipleScaledBy16);

#if CE_ARTLIB_EnableDebugMode
  extern void LE_SEQNCR_DumpActiveSequencesToDebugFile (BOOL PrintExtraData);
#endif


// The various wait functions.  These only work in multitasking
// mode.  In single tasking mode, you can call LE_SEQNCR_DoUpdateCycle
// with an elapsed time of zero to execute your commands immediately,
// otherwise you have to return to the main loop before the sequencer
// does an update.  Or just use LE_SEQNCR_ProcessUserCommands.

#if CE_ARTLIB_EnableMultitasking
  extern void LE_SEQNCR_WaitForFullUpdateCycle (void);

  extern BOOL LE_SEQNCR_WaitForSequenceToHitEnd (LE_DATA_DataId DataID,
    UNS16 Priority, BOOL SearchWholeTree);

  extern void LE_SEQNCR_WaitForAllSequencesFinished (
    LE_REND_RenderSlotSet RenderSlotSet = 0,
    TYPE_Tick TimeoutLimit = CE_ARTLIB_SeqncrWaitForAllFinishDelay);
#endif // CE_ARTLIB_EnableMultitasking


// Various command batching commands.  Used for collecting other commands
// over a long time and then doing them all in one update cycle.  The
// Collect and Execute commands nest, so you can do a collect/execute without
// worrying if the caller is in the middle of a collect or not.  Note that
// you can only submit MAX_MESSAGES_IN_QUEUE messages (about 100) before
// it runs out of space.  But you can nest as much as you want (uses an
// "int" counter).

extern int LE_SEQNCR_CollectCommands (void);

extern int LE_SEQNCR_ExecuteCommands (void);

extern int LE_SEQNCR_CommandQueuePercentageFull (void);

#if CE_ARTLIB_EnableMultitasking
#define LE_SEQNCR_ProcessUserCommands() LE_SEQNCR_WaitForFullUpdateCycle ()
#else
#define LE_SEQNCR_ProcessUserCommands() LE_SEQNCR_DoUpdateCycle (0)
#endif


// This one makes the library forget all pending chains.  Oddly enough
// you can chain this command to other commands.

extern BOOL LE_SEQNCR_ForgetChainsTheWorks (
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains);

extern BOOL LE_SEQNCR_ForgetChains (void);


// Various invalidate screen commands.

extern BOOL LE_SEQNCR_InvalidateScreenTheWorks (TYPE_RectPointer InvalidRectPntr,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  BOOL SearchWholeTreeForChains, LE_REND_RenderSlotSet RenderSlotSet);

extern BOOL LE_SEQNCR_InvalidateScreenRect (TYPE_RectPointer InvalidRectPntr);


// The user accessible render slot support functions.

extern void LE_SEQNCR_UninstallRenderSlot (LE_REND_RenderSlot RenderSlot);

extern UNS8 LE_SEQNCR_GetRenderSlotCamera (LE_REND_RenderSlot RenderSlot);


// An invalidate object command.  Forces redraw of sequences and bitmaps,
// most useful for redisplaying runtime objects you have modified.

extern BOOL LE_SEQNCR_ForceRedrawTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  BOOL SearchWholeTreeForChains, BOOL SearchWholeTreeForSequence);

extern BOOL LE_SEQNCR_ForceRedraw (LE_DATA_DataId DataID, UNS16 Priority);


// A few data preloading and unloading commands.

extern BOOL LE_SEQNCR_PrepareSequenceData (
  LE_DATA_DataId DataID, BOOL DoAddRef);

extern BOOL LE_SEQNCR_CleanUpSequenceData (LE_DATA_DataId DataID,
  BOOL DoRemoveRef, LE_SEQNCR_DataDisposalOptions DisposalOptions);


// Various start sequence commands.  Options are "C" for chain arguments
// (use LE_DATA_EmptyItem if you don't want to chain), "X" and "Y" and "Z"
// for position arguments, "R" for rotate, "S" for scale.

extern BOOL LE_SEQNCR_SetDefaultRenderSlotSetForStartSequence (
  LE_REND_RenderSlotSet RenderSlotSet);

extern LE_REND_RenderSlotSet LE_SEQNCR_GetDefaultRenderSlotSetForStartSequence (void);

extern BOOL LE_SEQNCR_SetDefaultLoaderOptionsForStartSequence (
  BOOL PreloadData, LE_SEQNCR_DataDisposalOptions PostDisposeData,
  BOOL UseReferenceCounts);

extern BOOL LE_SEQNCR_StartTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  LE_DATA_DataId ParentDataID, UNS16 ParentPriority, BOOL SearchWholeTreeForParent,
  UNS8 MatrixDimensionality, void *MatrixPntr,
  INT32 InitialClockOffset, UNS8 LabelNumber, BOOL ForceRedrawOnEveryUpdate,
  UNS8 TimeMultiple, BOOL UseAbsoluteDataIDs,
  LE_SEQNCR_EndingAction EndingAction,
  LE_REND_RenderSlotSet RenderSlotSet,
  LE_SEQNCR_UserCallbackFunctionPointer CallbackFunction, void *CallbackData,
  BOOL PreloadDataUsedBySequence,
  LE_SEQNCR_DataDisposalOptions PostDisposeDataUsedBySequence,
  BOOL UseReferenceCountsOnDataUsedBySequence,
  LE_SEQNCR_DropFrames DropFramesMethod);

extern BOOL LE_SEQNCR_Start (LE_DATA_DataId DataID, UNS16 Priority);

extern BOOL LE_SEQNCR_StartLoadOpts (LE_DATA_DataId DataID, UNS16 Priority,
  BOOL Preload, LE_SEQNCR_DataDisposalOptions DisposalOptions,
  BOOL UseReferenceCounts);

extern BOOL LE_SEQNCR_StartXY (LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Coord2D X, TYPE_Coord2D Y);

extern BOOL LE_SEQNCR_StartXYDrop (LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Coord2D X, TYPE_Coord2D Y, LE_SEQNCR_DropFrames DropFramesMethod);

extern BOOL LE_SEQNCR_StartXYCallbackSlot (LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Coord2D X, TYPE_Coord2D Y,
  LE_SEQNCR_UserCallbackFunctionPointer CallbackFunction, void *CallbackData,
  LE_REND_RenderSlotSet SlotSet);

extern BOOL LE_SEQNCR_StartXYSlotLoadOpts (LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Coord2D X, TYPE_Coord2D Y, LE_REND_RenderSlotSet SlotSet,
  BOOL Preload, LE_SEQNCR_DataDisposalOptions DisposalOptions,
  BOOL UseReferenceCounts);

extern BOOL LE_SEQNCR_StartCXY (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  TYPE_Coord2D X, TYPE_Coord2D Y);

extern BOOL LE_SEQNCR_StartCXYSlot (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  TYPE_Coord2D X, TYPE_Coord2D Y, LE_REND_RenderSlotSet SlotSet);

extern BOOL LE_SEQNCR_StartCXYSlotDrop (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  TYPE_Coord2D X, TYPE_Coord2D Y, LE_REND_RenderSlotSet SlotSet,
  LE_SEQNCR_DropFrames DropFramesMethod);

extern BOOL LE_SEQNCR_StartCXYSR (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  TYPE_Coord2D X, TYPE_Coord2D Y, TYPE_Scale2D Scale, TYPE_Angle2D Rotate);

extern BOOL LE_SEQNCR_StartCXYSRSlot (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  TYPE_Coord2D X, TYPE_Coord2D Y, TYPE_Scale2D Scale, TYPE_Angle2D Rotate,
  LE_REND_RenderSlotSet SlotSet);

extern BOOL LE_SEQNCR_StartRySTxz (LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Angle3D Yaw, TYPE_Scale3D Scale, TYPE_Coord3D X, TYPE_Coord3D Z);

extern BOOL LE_SEQNCR_StartRySTxzDrop (LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Angle3D Yaw, TYPE_Scale3D Scale, TYPE_Coord3D X, TYPE_Coord3D Z,
  LE_SEQNCR_DropFrames DropFramesMethod);

extern BOOL LE_SEQNCR_StartCOxyzRyprSxyzTxyz (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  TYPE_Coord3D OriginX, TYPE_Coord3D OriginY, TYPE_Coord3D OriginZ,
  TYPE_Angle3D Yaw, TYPE_Angle3D Pitch, TYPE_Angle3D Roll,
  TYPE_Scale3D ScaleX, TYPE_Scale3D ScaleY, TYPE_Scale3D ScaleZ,
  TYPE_Coord3D X, TYPE_Coord3D Y, TYPE_Coord3D Z);


// Various stop sequence commands.  Options are "R" for RenderSlotSet and
// "P" for priority.  Remember that the background is a sequence too,
// so use the priority to avoid a totally empty screen!

extern BOOL LE_SEQNCR_StopTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL StopAncestorsToo, BOOL StopDescendantsToo, BOOL SearchWholeTree);

extern BOOL LE_SEQNCR_Stop (LE_DATA_DataId DataID, UNS16 Priority);

extern BOOL LE_SEQNCR_StopFromChainEnd (LE_DATA_DataId DataID, UNS16 Priority);

extern BOOL LE_SEQNCR_StopAllTheWorks (LE_REND_RenderSlotSet RenderSlotSet,
  UNS16 Priority, LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL StopAncestorsToo, BOOL StopDescendantsToo, BOOL SearchWholeTree);

extern BOOL LE_SEQNCR_StopAllRP (LE_REND_RenderSlotSet RenderSlotSet,
  UNS16 Priority);


// Various commands for moving around an existing sequence.

extern BOOL LE_SEQNCR_MoveTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence, UNS8 MatrixDimensionality, void *MatrixPntr);

extern BOOL LE_SEQNCR_MoveXY (LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Coord2D X, TYPE_Coord2D Y);

extern BOOL LE_SEQNCR_MoveCXY (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  TYPE_Coord2D X, TYPE_Coord2D Y);

extern BOOL LE_SEQNCR_MoveXYSR (LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Coord2D X, TYPE_Coord2D Y, TYPE_Scale2D Scale, TYPE_Angle2D Rotate);

extern BOOL LE_SEQNCR_MoveRySTxz (LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Angle3D Yaw, TYPE_Scale3D Scale, TYPE_Coord3D X, TYPE_Coord3D Z);

extern BOOL LE_SEQNCR_MoveCOxyzRyprSxyzTxyz (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  TYPE_Coord3D OriginX, TYPE_Coord3D OriginY, TYPE_Coord3D OriginZ,
  TYPE_Angle3D Yaw, TYPE_Angle3D Pitch, TYPE_Angle3D Roll,
  TYPE_Scale3D ScaleX, TYPE_Scale3D ScaleY, TYPE_Scale3D ScaleZ,
  TYPE_Coord3D X, TYPE_Coord3D Y, TYPE_Coord3D Z);


// An assortment of commands for pausing and unpausing a sequence.

extern BOOL LE_SEQNCR_PauseTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence, BOOL PauseIt);

extern BOOL LE_SEQNCR_Pause (LE_DATA_DataId DataID, UNS16 Priority,
  BOOL PauseIt);


// Some commands for altering a sequence's current clock.

extern BOOL LE_SEQNCR_SetClockTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence, INT32 NewClock);

extern BOOL LE_SEQNCR_SetClock (LE_DATA_DataId DataID, UNS16 Priority,
  INT32 NewClock);

extern BOOL LE_SEQNCR_SetClockToEnd (LE_DATA_DataId DataID, UNS16 Priority);


// A bunch of commands for setting a sequence's ending action.

extern BOOL LE_SEQNCR_SetEndingActionTheWorks (LE_DATA_DataId DataID,
  UNS16 Priority, LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence, LE_SEQNCR_EndingAction NewEndingAction);

extern BOOL LE_SEQNCR_SetEndingAction (LE_DATA_DataId DataID, UNS16 Priority,
  LE_SEQNCR_EndingAction NewEndingAction);


// Some commands for changing sound settings.  They only work for pure sounds
// and for videos.

extern BOOL LE_SEQNCR_SetVolumeTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence, UNS8 NewVolume);

extern BOOL LE_SEQNCR_SetVolume (LE_DATA_DataId DataID, UNS16 Priority,
  UNS8 NewVolume);

extern BOOL LE_SEQNCR_SetPanTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence, INT8 NewPan);

extern BOOL LE_SEQNCR_SetPan (LE_DATA_DataId DataID, UNS16 Priority,
  INT8 NewPan);


extern BOOL LE_SEQNCR_SetPitchTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence, UNS32 NewPitch);

extern BOOL LE_SEQNCR_SetPitch (LE_DATA_DataId DataID, UNS16 Priority,
  UNS32 NewPitch);


// Commands for changing video settings while the video is playing.

extern BOOL LE_SEQNCR_SetSaturationTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence, INT8 NewSaturation);

extern BOOL LE_SEQNCR_SetSaturation (LE_DATA_DataId DataID, UNS16 Priority,
  INT8 NewSaturation);

extern BOOL LE_SEQNCR_SetBrightnessTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence, INT8 NewBrightness);

extern BOOL LE_SEQNCR_SetBrightness (LE_DATA_DataId DataID, UNS16 Priority,
  INT8 NewBrightness);

extern BOOL LE_SEQNCR_SetContrastTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence, INT8 NewContrast);

extern BOOL LE_SEQNCR_SetContrast (LE_DATA_DataId DataID, UNS16 Priority,
  INT8 NewContrast);


// Commands for jumping around in the video (also see SetClock).

extern BOOL LE_SEQNCR_SetUpVideoAlternativePathTheWorks (
  LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence,
  INT32 DecisionFrameNumber, INT32 JumpToFrameNumber);

extern BOOL LE_SEQNCR_SetUpVideoAlternativePath (
  LE_DATA_DataId DataID, UNS16 Priority,
  INT32 DecisionFrameNumber, INT32 JumpToFrameNumber);

extern BOOL LE_SEQNCR_ChooseVideoAlternativePathTheWorks (
  LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence,
  INT32 DecisionFrameNumber, BOOL TakeAlternative);

extern BOOL LE_SEQNCR_ChooseVideoAlternativePath (
  LE_DATA_DataId DataID, UNS16 Priority,
  INT32 DecisionFrameNumber, BOOL TakeAlternative);

extern BOOL LE_SEQNCR_ForgetVideoAlternativePathsTheWorks (
  LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence);

extern BOOL LE_SEQNCR_ForgetVideoAlternativePaths (
  LE_DATA_DataId DataID, UNS16 Priority);


// Commands for fiddling with viewports (portion of screen used for output)
// and camera settings.

extern BOOL LE_SEQNCR_SetViewportTheWorks (TYPE_RectPointer ViewportRectPntr,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  BOOL SearchWholeTreeForChains, LE_REND_RenderSlot RenderSlotNumber);

extern BOOL LE_SEQNCR_SetViewport (TYPE_RectPointer ViewportRectPntr,
  LE_REND_RenderSlot RenderSlotNumber);

extern BOOL LE_SEQNCR_SetCameraTheWorks (UNS8 CameraNumber,
  TYPE_Point3DPointer Position, TYPE_Point3DPointer Forwards,
  TYPE_Point3DPointer Up, TYPE_Angle3D HalfFovAngle,
  TYPE_Coord3D NearZ, TYPE_Coord3D FarZ,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
  BOOL SearchWholeTreeForChains, LE_REND_RenderSlot RenderSlotNumber);

extern BOOL LE_SEQNCR_SetCameraNumber (UNS8 CameraNumber,
  LE_REND_RenderSlot RenderSlotNumber);

extern BOOL LE_SEQNCR_SetCamera3D (TYPE_Point3DPointer Position,
  TYPE_Point3DPointer Forwards, TYPE_Point3DPointer Up,
  TYPE_Angle3D HalfFovAngle, TYPE_Coord3D NearZ, TYPE_Coord3D FarZ,
  LE_REND_RenderSlot RenderSlotNumber);

extern BOOL LE_SEQNCR_SetCamera3DLookAt (
  TYPE_Point3DPointer Position, TYPE_Point3DPointer LookingAt,
  TYPE_Angle3D HalfFovAngle, TYPE_Coord3D NearZ, TYPE_Coord3D FarZ,
  LE_REND_RenderSlot RenderSlotNumber);

extern BOOL LE_SEQNCR_SetCamera2D (TYPE_Coord2D X, TYPE_Coord2D Y,
  TYPE_Coord2D QuietSoundDistance, TYPE_Scale2D ScaleFactor,
  TYPE_Angle2D Rotation, LE_REND_RenderSlot RenderSlotNumber);


// Some query functions.  These execute right away, they don't use the
// command queue so collect/execute doesn't affect them.

extern BOOL LE_SEQNCR_SetWatch (LE_DATA_DataId DataID, UNS16 Priority,
  int WatchIndex);

extern BOOL LE_SEQNCR_FindChainee (LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId *ChaineeDataID, UNS16 *ChaineePriority);

extern BOOL LE_SEQNCR_IsSequenceFinished (LE_DATA_DataId DataID,
  UNS16 Priority, BOOL SearchWholeTree);

extern BOOL LE_SEQNCR_AreAllSequencesFinished (
  LE_REND_RenderSlotSet RenderSlotSet = 0);

extern BOOL LE_SEQNCR_GetInfo (LE_DATA_DataId DataID, UNS16 Priority,
  BOOL SearchWholeTree,
  LE_SEQNCR_RuntimeInfoPointer      InfoGenericPntr,
  LE_SEQNCR_Runtime2DInfoPointer    Info2DPntr,
  LE_SEQNCR_Runtime3DInfoPointer    Info3DPntr,
  LE_SEQNCR_RuntimeVideoInfoPointer InfoVideoPntr,
  LE_SOUND_AudioStreamPointer       InfoAudioPntr);

extern BOOL LE_SEQNCR_GetChildMeshWorldMatrix (
  LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Matrix3DPointer UserProvidedMatrix);

extern BOOL LE_SEQNCR_GetChildMeshWorldBoundingBox (
  LE_DATA_DataId DataID, UNS16 Priority,
  TYPE_Point3DPointer UserProvidedBuffer);

extern LE_SEQNCR_SequenceType LE_SEQNCR_GetHeaderFromDataID (
  LE_DATA_DataId DataID,
  LE_SEQNCR_SequenceChunkHeaderPointer InfoGenericPntr);

extern BOOL LE_SEQNCR_GetNowPlayingThingAndBox (
  LE_DATA_DataId SequenceDataID, UNS16 SequencePriority, BOOL SearchWholeTree,
  LE_DATA_DataId *ThingDataIDPntr,
  TYPE_Rect *ScreenBoundingBoxPntr);

extern BOOL LE_SEQNCR_GetAVIFileCharacteristics (char *FileName,
  float *FramesPerSecond, UNS32 *NumberOfFrames, RECT *BoundingRectangle);


// Some mouse click interpretation functions.

extern UNS8 LE_SEQNCR_FindNextLabeledSequenceUnderScreenRectangle (
  UNS8 PreviousLabel = 0,
  TYPE_RectPointer RectanglePntr = NULL,
  LE_REND_RenderSlotSet RenderSlotSet = (LE_REND_RenderSlotSet) -1,
  LI_REND_AbstractRenderSlotPointer *ReturnedRenderSlot = NULL,
  BOOL IncludeGroups = FALSE);

#if CE_ARTLIB_SeqncrUseSerialNumbers
extern LE_CHUNK_SerialNumber LE_SEQNCR_FindNextSerialNumberUnderScreenRectangle (
  LE_CHUNK_SerialNumber PreviousSerialNumber = 0,
  TYPE_RectPointer RectanglePntr = NULL,
  LE_REND_RenderSlotSet RenderSlotSet = (LE_REND_RenderSlotSet) -1,
  LI_REND_AbstractRenderSlotPointer *ReturnedRenderSlot = NULL,
  BOOL IncludeGroups = FALSE);
#endif


// Commands for setting up the mouse grouping sequence.  Also
// see the LE_SEQNCR_MouseGroupingSequenceDataID and the
// LE_SEQNCR_MouseGroupingSequencePriority global variables.

#if CE_ARTLIB_EnableSystemMouse
extern BOOL LE_SEQNCR_InitialiseMouseGroupingSequence (
  LE_REND_RenderSlotSet RenderSlotSetToUse, UNS16 Priority);

extern void LE_SEQNCR_RemoveMouseGroupingSequence (void);

extern BOOL LE_SEQNCR_AddMouseSubSequenceTheWorks (LE_DATA_DataId MouseSequence,
  UNS16 MousePriority, INT16 HotSpotXOffset, INT16 HotSpotYOffset,
  BOOL DeleteOtherMouseSequences);

extern BOOL LE_SEQNCR_AddMouseSubSequence (LE_DATA_DataId MouseSequence);
#endif // CE_ARTLIB_EnableSystemMouse


// Commands for hacking up a sequence in memory.

extern int LE_SEQNCR_ReplaceTABInSequence (LE_DATA_DataId sequenceID,
  LE_DATA_DataId oldTAB, LE_DATA_DataId newTAB,
  BOOL adjustBoundingBox, BOOL addRefOnSequence);

extern BOOL LE_SEQNCR_RecolourTABsInSequence (LE_DATA_DataId DataID,
  LPCOLORMAP lpColourMapArray, int nColourMapSize);


// Spooled Video and Audio handling commands.

extern LE_DATA_DataId LE_SEQNCR_CreateVideoObject (
  char *FileName,
  BOOL DrawSolid = TRUE,
  UNS16 AlphaLevel = ALPHA_OPAQUE100_0,
  TYPE_RectPointer OptionalDestinationRect = NULL,
  BOOL FlipVideoVertically = FALSE,
  BOOL EnableVideo = TRUE,
  BOOL EnableAudio = TRUE,
  BOOL DrawDirectlyToScreen = TRUE,
  BOOL DoubleAlternateLines = FALSE,
  int Saturation = 0,
  int Brightness = 0,
  int Contrast = 0);

extern LE_DATA_DataId LE_SEQNCR_CreateAudioObject (char *FileName,
  INT8 PanPercentage, UNS8 VolumePercentage,
  LE_SEQNCR_EndingAction EndingAction, UNS8 Dimensionality);


#if CE_ARTLIB_EnableSystemBinkStandAlone
extern BOOL LE_SEQNCR_TakeOverAndPlayBinkVideo (char *FileName, BOOL DoubleSize = FALSE);
#endif


// Some internal stuff, mostly for the dope sheet to use.
// The first few are really chunky routines, but we need the sequencer
// headers so they have to go in here.  See L_Chunk.cpp for details.

#if CE_ARTLIB_EnableChunkManipulation

typedef BOOL (*LE_CHUNK_ForAllCallbackFunctionPointer)
  (LE_CHUNK_StreamID StreamID,
  LE_CHUNK_ID ChunkID,
  LE_SEQNCR_SequenceChunkHeaderPointer SeqHeaderPntr,
  LE_CHUNK_DopeDataPointer DopeDataPntr,
  void *UserValue, INT32 AbsoluteStartTime);

extern BOOL LE_CHUNK_ForAllSubSequences (LE_DATA_DataId DataID,
  LE_CHUNK_SerialNumber SubSequenceSerialNumber, BOOL Recursive,
  UNS32 FlagsToMatch, BOOL RequireAllFlags,
  LE_CHUNK_ForAllCallbackFunctionPointer UserFunctionPntr, void *UserValue,
  INT32 BaseStartTime);

extern BOOL LE_CHUNK_ModifySequenceData (LE_DATA_DataId DataID,
  LE_CHUNK_SerialNumber SequenceSerialNumber,
  LE_SEQNCR_InterpolationTypes InterpolationType,
  INT32 TimeToModify,
  BOOL RelativeNotAbsolute,
  LE_CHUNK_ModifiableTypesOfSequenceData ThingToModify,
  float X, float Y, float Z);

#endif // CE_ARTLIB_EnableChunkManipulation



// Pointers to assorted kinds of data chunks.  Pointers rather than
// a union of records since subrecords (Direct3D matrices in particular)
// get constructors called and that doesn't work with unions.

typedef union LE_SEQNCR_PossibleDataChunkPointersUnion
{
  void                                           *voidPntr;
  LE_SEQNCR_DimensionalityChunkPointer            dim;
  LE_SEQNCR_2DXformChunkPointer                   xForm2D;
  LE_SEQNCR_2DOffsetChunkPointer                  offset2D;
  LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer osrt2D;
  LE_SEQNCR_3DXformChunkPointer                   xForm3D;
  LE_SEQNCR_3DOffsetChunkPointer                  offset3D;
  LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer osrt3D;
  LE_SEQNCR_2DBoundingBoxChunkPointer             box2D;
  LE_SEQNCR_3DBoundingBoxChunkPointer             box3D;
  LE_SEQNCR_3DBoundingSphereChunkPointer          sphere;
  LE_SEQNCR_3DMeshChoiceChunkPointer              meshChoice;
  LE_SEQNCR_LabelChunkPointer                     label;
  LE_SEQNCR_SoundPitchChunkPointer                soundPitch;
  LE_SEQNCR_SoundVolumeChunkPointer               volume;
  LE_SEQNCR_SoundPanningChunkPointer              panning;
  LE_SEQNCR_FieldOfViewChunkPointer               fov;
} LE_SEQNCR_PossibleDataChunkPointers;


// A record for holding the data for making a new data chunk,
// which can contain the largest kind of data possible.

#pragma pack(push,1)
typedef struct LI_SEQNCR_MaximalDataChunkStruct
{
  LE_CHUNK_HeaderRecord header;
  BYTE                  contents [sizeof (LE_SEQNCR_3DBoundingBoxChunkRecord)];
} LI_SEQNCR_MaximalDataChunkRecord, *LI_SEQNCR_MaximalDataChunkPointer;
#pragma pack(pop)



extern int LI_SEQNCR_ForAllActiveSequences (
  TYPE_RectPointer TouchingRectanglePntr,
  BOOL IncludeGroupingSequences,
  LE_REND_RenderSlotSet RenderSlotSet,
  LI_SEQNCR_IterationCallbackFunctionPointer CallbackFunctionPntr,
  void *UserValue);


void LE_SEQNCR_SelectionBoxToggle (LE_DATA_DataId DataID, UNS16 Priority, BOOL EnableSelectionBox);

#endif // __L_SEQNCR_H__
