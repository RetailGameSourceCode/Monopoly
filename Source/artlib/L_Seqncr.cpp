/*****************************************************************************
 *
 *   FILE:        L_Seqncr.cpp
 *
 *   DESCRIPTION: Animation timing and sequencer implementation.  This
 *                module moves around the animations using a hierarchical
 *                coordinate system (moving a parent moves the children),
 *                switches between bitmaps and otherwise handles timing
 *                related things.  After everything is ready, the bitmaps
 *                and position information are sent off to a renderer to
 *                be drawn (separate renderers for 0D, 2D and 3D).
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Seqncr.cpp 127   99-10-12 2:37p Agmsmith $
 *
 * Update Cycle Flag Propogation Notes:
 *
 * The update cycle starts with a clean slate - all the flags are in a
 * cleared state (needsRedraw (used by the render slots, only set and
 * cleared by the sequencer), needsPositionRecalc, needsReEvaluation).
 *
 * User commands are then processed.  If they add a new sequence, then
 * the needsReEvaluation flags between the new sequence and the top of the
 * sequence tree are set and the sequence's redraw flag is also set.  If a
 * sequence is moved then it gets marked as needsPositionRecalc and all its
 * parents get marked as needsReEvaluation, but redraw isn't bothered with
 * since needsPositionRecalc will force it and all its children to redraw
 * anyways (and force the children to reevaluate too).  If a sequence is
 * deleted, nothing gets marked (the delete does tell the render slots that
 * its gone, but the sequencer doesn't care - unless later we need to keep
 * parent bounding boxes up to date).
 *
 * The recursive clock update starts.  It traverses down the tree through
 * sequences with either an expired clock or needsReEvaluation flag.  Other
 * sequences just cut off the traversal since no update is needed below them.
 * As it is traversing, it pushes down the needsPositionRecalc so that all
 * children of a moved sequence get new positions.  Also, it ripples up the
 * redraw flag so all parents of something that needed redrawing also get
 * marked as needing redrawing.  As it exits from recursion it clears the
 * needsPositionRecalc and needsReEvaluation flags.  The redraw flags are
 * left for the render slots to use.
 *
 * Finally, the render slots get activated to actually draw the tree,
 * and they can use the needsRedraw to decide what to redraw, or they
 * could monitor the Start/Moved/Deleted sequence operations to update
 * their invalid areas (which is what the 2D renderer does).  So maybe
 * we don't even need those redraw flags.
 *
 * - AGMS19980419
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Seqncr.cpp $
 * 
 * 127   99-10-12 2:37p Agmsmith
 * Detect if the video card can't do stretch blits, like the Voodoo
 * series, and don't try to double size the Bink video in that case.
 *
 * 126   10/06/99 11:38a Russellk
 * Added a sequencer start check for LED_EI
 *
 * 125   5/10/99 4:59p Agmsmith
 * If Bink standalone player can't use video memory for the offscreen
 * bitmap, use system memory instead.
 *
 * 124   5/10/99 4:25p Agmsmith
 * Added a double size option to the Bink standalone video player - uses
 * an offscreen surface and stretch blits to make it larger.
 *
 * 123   10/05/99 12:00p Mikeh
 * Added function LE_SEQNCR_SelectionBoxToggle()
 * which allows the toggling of the bounding box
 * for a sequence. Probably of limited value in an
 * actual game but for debugging purposes...
 *
 * 122   21/09/99 4:35p Agmsmith
 * Don't wait for update as part of CollectCommands.
 *
 * 121   18/09/99 3:33p Agmsmith
 * Use search path when playing Bink videos.
 *
 * 120   17/09/99 12:20p Agmsmith
 * Made stand-alone Bink player available separately.
 *
 * 119   16/09/99 9:32p Agmsmith
 * Now with a stand-alone Bink video player function too.
 *
 * 118   16/09/99 8:05p Agmsmith
 * Get double size working as much as possible for Bink (only doubles
 * height and only by interlacing).
 *
 * 117   16/09/99 5:25p Agmsmith
 * Adding Bink video player.
 *
 * 116   99/09/08 10:26a Agmsmith
 * Don't call 3D functions in 2D game.
 *
 * 115   99/09/07 3:01p Agmsmith
 * Now have a custom dialog box for error messages, with custom exit
 * button choices.
 *
 * 114   99/09/03 12:17p Agmsmith
 * Now do a full traversal when finding 3D meshes for bounding box
 * retrieval, so raw HMDs will work too.
 *
 * 113   99/09/02 12:51p Agmsmith
 * Get a 3D bounding box for a 3D animation.
 *
 * 112   99/08/25 1:46p Agmsmith
 * Update CommandNames with new commands.
 *
 * 111   8/20/99 2:10p Agmsmith
 * Do a bit of math to make the camera up vector perpendicular to the
 * forwards vector.
 *
 * 110   8/20/99 1:15p Agmsmith
 * Use search paths when finding file names of videos and spooled audio
 * files.
 *
 * 109   8/20/99 11:28a Agmsmith
 * Fixing SetViewport and SetCamera commands.
 *
 * 108   8/18/99 8:17p Agmsmith
 * Added SetCamera sequencer command.
 *
 * 107   8/16/99 5:25p Agmsmith
 * Adding set pitch and viewport sequencer commands, set camera under
 * construction.
 *
 * 106   8/03/99 3:21p Agmsmith
 * Allow changes to the default volume level for games with badly recorded
 * sound files.
 *
 * 105   7/20/99 1:18p Agmsmith
 * Use structure alignment of 1 byte for chunky file simulations.
 *
 * 104   7/13/99 4:26p Agmsmith
 * Need to read the stream format (bitmap info) to find true size of the
 * AVI.
 *
 * 103   7/13/99 3:48p Agmsmith
 * Change AVIGetInfo to not use AVIFileOpen, which doesn't work under
 * Windows 2000.
 *
 * 102   7/12/99 11:25a Agmsmith
 * Removed latched keys feature in favour of UIMsg events, which have the
 * advantage of not losing keystrokes.
 *
 * 101   7/06/99 1:14p Agmsmith
 * Added function for finding position of a child mesh object, for
 * Monopoly 3D token location.  Should really use hit boxes but...
 *
 * 100   7/02/99 11:19a Agmsmith
 * Added drop frames option to starting a sequence.
 *
 * 99    6/14/99 11:35a Agmsmith
 * Forgot camera shutdown dealloc needed.
 *
 * 98    6/08/99 8:57a Agmsmith
 * Make camera default bounding box bigger.
 *
 * 97    6/07/99 3:19p Agmsmith
 * Added camera capability.
 *
 * 96    6/04/99 1:01p Agmsmith
 * Adding field of view private chunk and getting cameras to work.
 *
 * 95    5/31/99 2:36p Agmsmith
 * Fixed parameters passed to StartVideo call.
 *
 * 94    5/26/99 3:48p Agmsmith
 * Updated for new camera chunk fields.
 *
 * 93    5/09/99 12:59p Agmsmith
 * Moved mesh proportion out of the tweeker chunk and into the mesh choice
 * chunk.
 *
 * 92    5/09/99 11:18a Agmsmith
 * Force move update to get mesh change noticed by the 3D code.
 *
 * 91    4/28/99 2:32p Agmsmith
 *
 * 90    4/28/99 2:26p Agmsmith
 * Dump 3D matrices too.
 *
 * 89    4/27/99 1:34p Agmsmith
 * Number of render slots is now user defined.
 *
 * 88    4/23/99 3:52p Agmsmith
 * Avoid sending duplicate UI messages for stay at end sequences.
 *
 * 87    4/22/99 15:01 Davew
 * Updated to DX v6
 *
 * 86    4/19/99 11:56a Agmsmith
 * Added some 3D glue functions for start and move.
 *
 * 85    4/18/99 4:24p Agmsmith
 * Changed offset/scale/rotate to origin/scale/rotate/offset chunk.
 *
 * 84    4/17/99 5:07p Agmsmith
 * Allow for infinite end time when adding newly born children.
 *
 * 83    4/12/99 3:42p Agmsmith
 * Use GetLoadedDataType instead of current, unless you know what you are
 * doing.
 *
 * 82    4/12/99 3:05p Agmsmith
 * Need to get bounding rect for a particular sequence for hit location.
 *
 * 81    4/11/99 6:36p Agmsmith
 * Added render slot method for converting a mouse drag to a camera plane
 * motion in world coordinates.
 *
 * 80    4/11/99 3:49p Agmsmith
 * Now postload converts BMPs into native screen depth bitmaps, rather
 * than having the sequencer do it.  So remove all BMP things everywhere.
 *
 * 79    4/11/99 2:28p Agmsmith
 * Fix bug in LI_SEQNCR_AddNewlyBornChildren which was making stay at end
 * sequences disappear after a SetClock in the future.
 *
 * 78    4/07/99 6:39p Agmsmith
 * Forgot to find dimensionality of some special chunks.
 *
 * 77    3/31/99 5:24p Agmsmith
 * Only process mesh choice subchunks if the sequence is 3D (tweekers were
 * getting the initial settings when they shouldn't).
 *
 * 76    3/29/99 5:06p Agmsmith
 * Make it so the frame rate bar doesn't overwrite other windows.
 *
 * 75    3/28/99 12:12p Agmsmith
 * Better sound updating by tweekers - can't tweek panning and volume
 * directly on 2D or 3D sounds.
 *
 * 74    3/28/99 11:41a Agmsmith
 * Sound settings are now percentages in optional subchunks.
 *
 * 73    3/27/99 3:51p Agmsmith
 * Now correctly use the sound timing to set the sequence clock for sound
 * type sequences.
 *
 * 72    3/27/99 12:50p Agmsmith
 * Don't update sequenceClock while feeding.   Also working on having time
 * for sounds come from the sound hardware, but currently unfinished and
 * incorrect.
 *
 * 71    3/26/99 10:27a Agmsmith
 * Interpolate sound pitch, volume and panning.
 *
 * 70    3/25/99 1:42p Agmsmith
 * Added linear interpolation and cancel interpolation.
 *
 * 69    3/25/99 11:54a Agmsmith
 * Added then removed tweeking shutdown code.
 *
 * 68    3/24/99 6:58p Agmsmith
 * Tweekers initially working, but no linear interpolation yet.
 *
 * 67    3/24/99 2:15p Agmsmith
 * Tweekers under construction.
 *
 * 66    3/24/99 8:56a Agmsmith
 *
 * 65    3/24/99 8:50a Agmsmith
 * Parsing of tweeker sequences added.
 *
 * 64    3/23/99 1:41p Agmsmith
 * Added some more private subchunks for position information, and
 * starting to add tweekers (more matrices involved) for mesh
 * interpolation etc.
 *
 * 63    3/21/99 4:00p Agmsmith
 * More space in dump of sequences for nesting.
 *
 * 62    3/21/99 3:34p Agmsmith
 * Changes to allow all sequences to have child sequences.
 *
 * 61    3/19/99 1:57p Agmsmith
 * Added immediate command execution mode.
 *
 * 60    3/19/99 10:42a Agmsmith
 * Fixed up 3DMesh sequence handling.
 *
 * 59    3/18/99 10:57a Andrewx
 * Modified for HMD display
 *
 * 58    3/16/99 11:44a Agmsmith
 * A few mouse improvements.
 *
 * 57    3/16/99 10:49a Agmsmith
 * Removed more compatability stubs and added wait for all sequences.
 *
 * 56    3/15/99 4:41p Agmsmith
 * Fixed bug in LI_SEQNCR_FindNextSerialCallback.
 *
 * 55    3/15/99 12:04p Agmsmith
 * Return default render slot.
 *
 * 54    3/14/99 12:14p Agmsmith
 * Added LE_SEQNCR_StartXYSlotLoadOpts.
 *
 * 53    3/13/99 3:28p Agmsmith
 * Added FindNext functions for finding sequences under the mouse cursor.
 *
 * 52    3/13/99 2:30p Agmsmith
 * Added iteration over all active sequences and a way of restricting that
 * to ones under a given mouse position.
 *
 * 51    3/09/99 4:13p Agmsmith
 * Bump up time delay between updates for stay at end sequences to the
 * max, about 4 seconds.
 *
 * 50    3/07/99 1:58p Agmsmith
 * Removed initialClock from sequence headers.
 *
 * 49    2/19/99 9:41a Agmsmith
 * Added stay at end low update rate option for saving CPU.
 *
 * 48    2/17/99 10:52a Agmsmith
 * Oops.
 *
 * 47    2/17/99 10:46a Agmsmith
 * To avoid frame update glitches, feed the video in the sequencer for
 * videos which don't draw directly to the screen, and only draw the
 * bitmap in the renderer.  Direct to screen still feeds and draws in the
 * renderer.
 *
 * 46    2/16/99 3:35p Agmsmith
 * No warning for frame zero stuck.
 *
 * 45    2/16/99 2:19p Agmsmith
 * Detect stalled videos and force a redraw to get them feed again.
 *
 * 44    2/16/99 1:27p Agmsmith
 * Added command to forget video alternative paths.
 *
 * 43    2/16/99 12:30p Agmsmith
 * When a video alternative path jump point is hit, send a UIMsg back to
 * the game to let it know about it.  Also print debug messages.
 *
 * 42    2/15/99 4:54p Agmsmith
 * Added alternative path through a video feature.
 *
 * 41    2/14/99 5:13p Agmsmith
 * Draw a box around selected sequences.
 *
 * 40    2/13/99 5:44p Agmsmith
 * Fix up SetClock so it works with looping sequences and is accurate (now
 * delete all children and regenerate the ones which exist, and only
 * create new children if they haven't died already).
 *
 * 39    2/11/99 12:40p Agmsmith
 * New function for displaying warnings.
 *
 * 38    2/09/99 1:51p Agmsmith
 * Oops.
 *
 * 37    2/09/99 1:41p Agmsmith
 * Fix drawing to an off-screen bitmap with the new video player.
 *
 * 36    2/09/99 12:55p Agmsmith
 * Added LE_SEQNCR_GetAVIFileCharacteristics.
 *
 * 35    2/09/99 12:13p Agmsmith
 * Add video size checking and default size which understands doubling
 * tricks.
 *
 * 34    2/09/99 10:46a Lzou
 * The colour effect arguments in FeedVideo are put in order of
 * brightness, contrast, saturation.
 *
 * 33    2/05/99 12:28p Agmsmith
 * Sense of video flip is reversed in the new player, compensate for it.
 *
 * 32    2/05/99 11:50a Agmsmith
 * Update the new style video more often now.
 *
 * 31    2/05/99 11:06a Agmsmith
 * Fix render slot selection when playing videos to screen.  Also make
 * videos 2D only.
 *
 * 30    2/02/99 1:47p Agmsmith
 * Video API changes again.
 *
 * 29    2/01/99 3:21p Agmsmith
 * Added commands to change video properties while playing.
 *
 * 28    2/01/99 2:09p Agmsmith
 * Added new video attributes.
 *
 * 27    1/31/99 5:33p Agmsmith
 * Changes for new improved video API.
 *
 * 26    1/31/99 3:28p Agmsmith
 * Rearranged video player code so that feeding was done during
 * renderering, not in the sequencer, so that it can draw directly on the
 * screen.
 *
 * 25    1/22/99 12:12p Agmsmith
 * Accept any of the file name chunks as the file name for spooling audio
 * or video.
 *
 * 2     9/22/98 4:23p Agmsmith
 * Updated to work with ArtLib99.
 *
 * 186   9/10/98 1:47p Agmsmith
 * Avoid crash on shutdown in debug mode due to frame rate power bar being
 * drawn on a deallocated screen.
 *
 * 136   7/08/98 6:43p Agmsmith
 * Added new sound system.
 ****************************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"
#if CE_ARTLIB_EnableSystemSequencer


/************************************************************/
/* PRIVATE DATA TYPE DECLARATIONS                           */
/*----------------------------------------------------------*/

#define INFINITE_END_TIME (1234567890L)
  // Treat an infinite end time as a little over half a year.  But don't
  // set the end time to the largest value since the clock does get set
  // to the end time when a chain is done and it keeps on advancing a bit
  // after that.  This large value also looks nice in debug output.

#if USE_OLD_VIDEO_PLAYER
#define DELAY_BETWEEN_VIDEO_FEEDS (CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ / 4)
#define VIDEO_FEED_DURATION (CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ / 4)
#define VIDEO_READ_AHEAD_DURATION (CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ * 4 / 4) /* AVI default skew between audio and video is 0.75 seconds, just to avoid a bit of skipping without going outside the small CDROM cache size, increase it to 1 second.  */
#define VIDEO_MAX_FEED_ITERATIONS (2) /* P90 320x240 first frame takes 1 sec to show up, so need sound to cover it */
#endif


// Flags used by the sequence command messages, usually passed in NumberE,
// so it has to fit into 4 bytes.

typedef struct SeqCmdFlagsStruct
{
  unsigned int SEQFLAG_ByteSizedValue : 8;
    // A byte sized space for small numeric arguments, to avoid having to
    // allocate space for a whole attached record.

  unsigned int SEQFLAG_StopChainedAncestors : 1;
    // When this bit is set in the flags for the stop sequence command,
    // all ancestor (earlier) chained sequences are also stopped.

  unsigned int SEQFLAG_StopChainedDescendants : 1;
    // When this bit is set in the flags for the stop sequence command,
    // all descendants (later) chained sequences are also stopped.

  unsigned int SEQFLAG_InterruptPreviousSequence : 1;
    // When this command is processed, it will end the chained to sequence
    // abruptly rather than waiting for it to end.  It's kind of like
    // moving the chainee's clock to past the end of its time, and
    // ignoring looping flags.

  unsigned int SEQFLAG_SearchWholeSequenceTree : 1;
    // If FALSE then only the top level sequences (the usual user started
    // sequences) will be searched for sequences that the command applies
    // to (for example, stop sequence).  If TRUE then the whole sequence
    // tree will be searched, including active bitmaps and other things
    // inside groups (usually a waste of time).

  unsigned int SEQFLAG_SearchWholeTreeForChains : 1;
    // If FALSE then only the top level sequences (the usual user started
    // sequences) will be searched for matching chained to items.  If
    // TRUE then the whole sequence tree will be searched, including
    // active bitmaps and other things inside groups.

  unsigned int SEQFLAG_MarkedForDeletion : 1;
    // This is used at runtime to mark chained commands for deletion
    // when doing a breadth first search of all related chained commands.
    // So, initialise it to zero when you initialise the command.

  unsigned int SEQFLAG_ReplacementEndingAction : 2;
    // See LE_SEQNCR_EndingAction for enum values.  This is used rather than
    // SEQFLAG_ByteSizedValue since the ending action is also in the start
    // command, which uses the byte sized thing for something else.

  unsigned int SEQFLAG_ForceRedrawEveryUpdate : 1;
    // Makes the sequence redraw on every frame instead of doing whatever
    // its default is.

  unsigned int SEQFLAG_UseAbsoluteDataIDs : 1;
    // If TRUE then it will force absolute DataIDs for the children of
    // a new sequence, if FALSE it will use the default setting from the
    // sequence data.

  unsigned int SEQFLAG_HowToDisposeSequenceDataWhenDone : 2;
    // Copied to the runtime information record when the sequence starts,
    // controls unloading of the sequence's data when it finishes.
    // This one is really a LE_SEQNCR_DataDisposalOptions.

  unsigned int SEQFLAG_UnrefSequenceDataWhenDone : 1;
    // Copied to the runtime information record when the sequence starts,
    // controls unloading of the sequence's data when it finishes.

  unsigned int SEQFLAG_ZeroBitmapOffset : 1;
    // If TRUE then the bitmap offsets from the TAB header are ignored
    // and it is positioned with the top left corner at the origin.
    // FALSE makes it use the offsets from the bitmap.  The offsets are
    // created by the opaquer when it crops a bitmap out of the bigger
    // full size scanned image.  It's a way of avoiding storing transparent
    // parts of the big scanned image.

  unsigned int SEQFLAG_DropFramesOption : 2;
    // See LE_SEQNCR_DropFrames for the real type.  Turn on or off the
    // drop frames option, to skip frames when the parent clock goes
    // faster than the time multiple of this sequence.  Use zero
    // (LE_SEQNCR_DropDefault) for the default from the sequence data.

} SeqCmdFlagsRecord, *SeqCmdFlagsPointer;

typedef union SeqCmdFlagsUnion
{
  SeqCmdFlagsRecord asFlags;
  DWORD             asDWORD;
} SeqCmdFlagsAsLong;
  // This union is useful for storing the flags into a 4 byte number,
  // usually the NumberE in messages.



// Commands that affect the engine data structures are passed as messages
// in a command queue.  The various fields are used as:
//
// MessageCode field is set to the SEQCMD_* message number.
// NumberA is usually the DataID.
// NumberB is usually priority.
// NumberC is always the ChainToDataID (LE_DATA_EmptyItem if not chained).
// NumberD is always the ChainToPriority.
//  Note that all commands can be chained, which means they get saved up
//  in the pending chains list until the chained-to animation has finished
//  or otherwise hits its end (looping ones activate the chained-to stuff
//  at the end of every loop).
// NumberE has various flag bits, see SeqCmdFlagsRecord for details.
// StringA contains the attached binary info which can be used when there
//   isn't enough space for more arguments.  Ideally it is NULL when the
//   command's extra arguments are all zero, avoiding wasting space.
// Players is used internally to link together the chain list.

typedef enum SequencerCommandEnum
{
  SEQCMD_Start,
    // Starts a sequence (adds it to the active tree or to the list of
    // pending chains).  In the queue message, the MessageCode field is set
    // to SEQCMD_StartSequence, DataID is in NumberA, Priority in NumberB,
    // ChainToDataID in NumberC, ChainToPriority in NumberD, various flag
    // bits in NumberE, new label number in SEQFLAG_ByteSizedValue (zero to
    // leave the label unchanged).  If you have more non-zero arguments,
    // such as starting position and scaling, it is added as a binary
    // attachment to the message (see the SeqCmdStart?DAttachmentRecord).
    // You can also add a new sequence as a child of an existing one, using
    // the ParentDataID, ParentPriority and SEQFLAG_SearchWholeSequenceTree.

  SEQCMD_Stop,
    // All running sequences (subjest to the SEQFLAG_SearchWholeSequenceTree
    // flag) with the given DataID/Priority are stopped (deleted from the
    // runtime tree).  Also recursively removes all commands chained to this
    // DataID/Priority in the pending chain list (descendants in other words)
    // if SEQFLAG_StopChainedDescendants is on.  As well it can recursively
    // remove chained ancestors if SEQFLAG_StopChainedAncestors is on (only
    // start sequence commands are examined to see if they start the given
    // sequence).  If you stop both ancestors and descendants then everything
    // remotely related to the given sequence is stopped and unchained.  The
    // search for ancestors and descendants is very inefficient (of order
    // N squared where N is the pending chain list size) so try to avoid
    // stopping relatives if you know a sequence isn't chained or a chainee.
    // Useful when you have a chain of sequences and want the whole thing
    // to stop - just stop the one at the end and turn on the ancestors flag.
    // NumberA contains the DataID, NumberB has the priority, NumberE has the
    // flags, in particular SEQFLAG_StopChainedAncestors,
    // SEQFLAG_StopChainedDescendants and SEQFLAG_SearchWholeSequenceTree.

  SEQCMD_StopAll,
    // All active sequences in the given render slots are stopped (be careful
    // that you don't stop the mouse pointer!) that have a priority greater
    // than or equal to a given level.  NumberA specifies the render slot set,
    // NumberB has the limiting priority level, NumberE has flags, of which
    // SEQFLAG_StopChainedAncestors, SEQFLAG_StopChainedDescendants are used
    // as well as SEQFLAG_SearchWholeSequenceTree (but only for chain cleanup).
    // All sequences in the slot will be stopped except for the top level ones
    // with a priority level less than the given one.

  SEQCMD_Move,
    // Moves an existing sequence to a new spot in the world.  DataID is in
    // NumberA, Priority in NumberB, ChainToDataID in NumberC, ChainToPriority
    // in NumberD, various flag bits in NumberE.  Usually there is a binary
    // attachment to the message (see SeqCmdMove?DAttachmentRecord) with the
    // target position as a matrix.  The SEQFLAG_SearchWholeSequenceTree flag
    // is used when looking for the sequence to move.

  SEQCMD_ForgetChains,
    // This command makes the sequencer clear the list of pending chains.

  SEQCMD_Pause,
    // Pause or unpause the given sequence.  DataID in NumberA, priority in
    // NumberB.  SEQFLAG_ByteSizedValue flag controls whether it is a pause
    // (TRUE) or unpause (FALSE).  SEQFLAG_SearchWholeSequenceTree is also
    // used when looking for the sequence.

  SEQCMD_SetClock,
    // Changes the sequence's clock to the given time.  Convenient for setting
    // to the sequence end time: on the next update, the sequence will bump
    // into the end of time, making it do its ending action (and starting
    // chained sequences).  Useful for aborting a big intro animation and
    // activating the things that chain to it.  There is an attached
    // SeqCmdSetClockAttachmentRecord with the new time.  DataID is in
    // NumberA, Priority in NumberB, ChainToDataID in NumberC, ChainToPriority
    // in NumberD, and the usual flags in NumberE.

  SEQCMD_SetEndingAction,
    // Changes an existing sequence's ending action to the one in
    // SEQFLAG_ReplacementEndingAction, if it isn't zero.  DataID is in
    // NumberA, Priority in NumberB, ChainToDataID in NumberC, ChainToPriority
    // in NumberD, and the usual flags in NumberE.

  SEQCMD_DefaultRenderSlotSet,
    // Changes the library's default render slot used in starting sequences
    // to the set of slots in NumberA.

  SEQCMD_InvalidateRect,
    // Invalidates the given screen rectangle in the given render slots.
    // See the attached SeqCmdInvalidateRectAttachmentRecord for the actual
    // rectangle and slots.

  SEQCMD_ForceRedraw,
    // Make the given object redraw fully.  Works for sequences and bitmaps.
    // Basically just sets the re-evaluate and redraw flags for the item.
    // DataID in NumberA, Priority in NumberB, SEQFLAG_SearchWholeSequenceTree
    // used for finding matching sequences.

  SEQCMD_SetVolume,
    // Changes the volume for sound sequences.  DataID is in NumberA, Priority
    // in NumberB, ChainToDataID in NumberC, ChainToPriority in NumberD, and
    // the usual flags in NumberE.  The volume is in SEQFLAG_ByteSizedValue,
    // 0 for quiet to 100 for loudest.

  SEQCMD_SetPan,
    // Changes the panning for sound sequences.  DataID is in NumberA, Priority
    // in NumberB, ChainToDataID in NumberC, ChainToPriority in NumberD, and
    // the usual flags in NumberE.  The panning is in SEQFLAG_ByteSizedValue,
    // -100 for left, 0 for center, +100 for right.

  SEQCMD_SetPitch,
    // Changes the pitch for sound sequences.  DataID is in NumberA, Priority
    // in NumberB, ChainToDataID in NumberC, ChainToPriority in NumberD, and
    // the usual flags in NumberE.  The pitch divided by 256 is in
    // SEQFLAG_ByteSizedValue, 0 for 0 (default pitch), 255 for 65280 hz,
    // 43 for 11008 hz, etc.  That may change if more precision is needed.

  SEQCMD_SetSaturation,
    // Changes the colour saturation for video sequences.  DataID is in
    // NumberA, Priority in NumberB, ChainToDataID in NumberC, ChainToPriority
    // in NumberD, and the usual flags in NumberE.  The saturation is in
    // SEQFLAG_ByteSizedValue, varies from -100 to +100 with 0 normal.

  SEQCMD_SetBrightness,
    // Changes the brightness for video sequences.  DataID is in
    // NumberA, Priority in NumberB, ChainToDataID in NumberC, ChainToPriority
    // in NumberD, and the usual flags in NumberE.  The brightness is in
    // SEQFLAG_ByteSizedValue, varies from -100 to +100 with 0 normal.

  SEQCMD_SetContrast,
    // Changes the contrast for video sequences.  DataID is in
    // NumberA, Priority in NumberB, ChainToDataID in NumberC, ChainToPriority
    // in NumberD, and the usual flags in NumberE.  The contrast is in
    // SEQFLAG_ByteSizedValue, varies from -100 to +100 with 0 normal.

  SEQCMD_SetVideoAlternative,
    // Sets up a video alternative jump point.  DataID is in NumberA,
    // Priority in NumberB, ChainToDataID in NumberC, ChainToPriority
    // in NumberD, and the usual flags in NumberE.  The DecisionFrameNumber
    // and JumpToFrameNumber are in SeqCmdSetVideoAlternativeAttachmentRecord.

  SEQCMD_ChooseVideoAlternative,
    // Switches on and off the video alternative path.  DataID is in
    // NumberA, Priority in NumberB, ChainToDataID in NumberC, ChainToPriority
    // in NumberD, and the usual flags in NumberE.  The decision frame number
    // which identifies the switch point and the flag for using the alternative
    // are in the attached SeqCmdChooseVideoAlternativeAttachmentRecord.

  SEQCMD_ForgetVideoAlternatives,
    // Forgets all the video jump points in the given video. DataID is in
    // NumberA, Priority in NumberB, ChainToDataID in NumberC, ChainToPriority
    // in NumberD, and the usual flags in NumberE.

  SEQCMD_SetViewport,
    // Changes the viewport on a render slot (the rectangular area it draws
    // on in the screen).  See the attached SeqCmdSetViewportAttachmentRecord.

  SEQCMD_SetCamera,
    // Changes the camera for a render slot, 2D or 3D.  See the attached
    // SeqCmdSetCameraAttachmentRecord for more.

  SEQCMD_MAX // Remember to update CommandNames when you add entries!
} SequencerCommand;


// Various structures that can be attached to a command message to
// hold data that doesn't fit into the plain message record.
// If several different records can be attached, you can tell them
// apart by their size.  The zero dimensional one is also used for
// other dimensions if there is no dimension specific data (positioning
// information).  You can find the true dimensionality in the flags.

typedef struct SeqCmdStart0DAttachmentStruct
{
  INT32 initialClockOffset;
    // This gets added to the sequence's initial clock, making it
    // skip frames if positive or take longer to start if negative.
    // Useful for grouping sequences, not useful for single bitmaps,
    // which always display themselves if they exist, no matter what
    // their clock is.

  LE_DATA_DataId  parentDataID;
  UNS16           parentPriority;
    // When adding a new sequence, it can be put under an existing one
    // matching this DataID (feature disabled with LE_DATA_EmptyItem)
    // and matching the priority.  The default is to put the new
    // sequence under the root grouping sequence.

  LE_SEQNCR_UserCallbackFunctionPointer callBack;
    // NULL for no callback.

  void *userData;
    // A pointer sized field for arbitrary user data.

  LE_REND_RenderSlotSet renderSlotSet;
  // Which slots you want.  Zero for the default slot.

  UNS8 timeMultiple;
    // Use this to change the frame rate for the sequence.  Number of
    // 1/60 second ticks between updates.  Zero for the default.

} SeqCmdStart0DAttachmentRecord, *SeqCmdStart0DAttachmentPointer;


typedef struct SeqCmdStart2DAttachmentStruct
{
  SeqCmdStart0DAttachmentRecord commonArguments;

  TYPE_Matrix2D                 positionMatrix;
    // Multiplies the sequence's matrix by this.  Use the identity
    // matrix if you want the default.

} SeqCmdStart2DAttachmentRecord, *SeqCmdStart2DAttachmentPointer;


typedef struct SeqCmdStart3DAttachmentStruct
{
  SeqCmdStart0DAttachmentRecord commonArguments;

  TYPE_Matrix3D                 positionMatrix;
    // Multiplies the sequence's matrix by this.  Use the identity
    // matrix if you want the default.

} SeqCmdStart3DAttachmentRecord, *SeqCmdStart3DAttachmentPointer;


typedef struct SeqCmdMove2DAttachmentStruct
{
  TYPE_Matrix2D positionMatrix;
} SeqCmdMove2DAttachmentRecord, *SeqCmdMove2DAttachmentPointer;


typedef struct SeqCmdMove3DAttachmentStruct
{
  TYPE_Matrix3D positionMatrix;
} SeqCmdMove3DAttachmentRecord, *SeqCmdMove3DAttachmentPointer;


typedef struct SeqCmdInvalidateRectAttachmentStruct
{
  TYPE_Rect rectangle;
    // The rectangle to be invalidated, in screen coordinates.

  LE_REND_RenderSlotSet renderSlotSet;
    // Which slots you want.  Zero for the default slot.

} SeqCmdInvalidateRectAttachmentRecord, *SeqCmdInvalidateRectAttachmentPointer;


typedef struct SeqCmdSetClockAttachmentStruct
{
  INT32 newClock;
    // The time you want to set the sequence's clock to.

} SeqCmdSetClockAttachmentRecord, *SeqCmdSetClockAttachmentPointer;


typedef struct SeqCmdSetVideoAlternativeAttachmentStruct
{
  INT32 decisionFrameNumber;
    // Last common video frame number before the branches diverge.

  INT32 jumpToFrameNumber;
    // The frame number to jump to for the alternative path.

} SeqCmdSetVideoAlternativeAttachmentRecord, *SeqCmdSetVideoAlternativeAttachmentPointer;


typedef struct SeqCmdChooseVideoAlternativeAttachmentStruct
{
  INT32 decisionFrameNumber;
    // Last common video frame number before the branches diverge.

  BOOL  takeAlternative;
    // TRUE to take the alternative path, FALSE to take the normal path.

} SeqCmdChooseVideoAlternativeAttachmentRecord, *SeqCmdChooseVideoAlternativeAttachmentPointer;


typedef struct SeqCmdSetViewportAttachmentStruct
{
  TYPE_Rect viewportRect;
    // The new viewport rectangle in screen coordinates.

  LE_REND_RenderSlot renderSlotNumber;
    // Which slot you want to set the viewport for.  No default.

} SeqCmdSetViewportAttachmentRecord, *SeqCmdSetViewportAttachmentPointer;


typedef struct SeqCmdSetCameraAttachmentStruct
{
  LE_REND_RenderSlot renderSlotNumber;
    // Render slot to change the camera of.

  UNS8 cameraNumber;
    // Use a particular animated camera, or set to zero to use
    // the rest of the fields in this record for the camera.

  TYPE_Point3D position;
    // Location of camera, 2D or 3D.  Z in 2D is quiet distance.

  TYPE_Point3D forwards;
    // Forwards direction normalised vector, 3D only.

  TYPE_Point3D up;
    // Upwards direction normalised vector for 3D, rotation
    // angle for 2D in X.

  TYPE_Angle3D        fovAngle;
    // Field of view angle in radians for 3D, a simple scale factor
    // for 2D (2.0 makes pixels twice as big etc).  Well, actually
    // half the FOV angle since they measure it from the center
    // rather than across the full field of view.

  TYPE_Coord3D        nearZ;
  TYPE_Coord3D        farZ;
    // Clipping distances and Z-buffer range, 3D only.

} SeqCmdSetCameraAttachmentRecord, *SeqCmdSetCameraAttachmentPointer;



/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

LI_REND_AbstractRenderSlotPointer LI_SEQNCR_RenderSlotArray [CE_ARTLIB_RendMaxRenderSlots];
  // An array of pointers to the active render slots.  NULL for slots which
  // are closed.  The renderer takes care of allocating and deallocating
  // its structures, just the pointer gets stored here.

LE_SEQNCR_RuntimeInfoPointer LE_SEQNCR_LabelArray [CE_ARTLIB_SeqncrMaxLabels];
  // Each labeled sequence, labels from 1 to CE_ARTLIB_SeqncrMaxLabels-1, zero
  // isn't used, has a pointer to it in this array.  The pointer gets set when
  // the sequence starts and get put back to NULL when the sequence ends.
  // If you have two sequences with the same label, and they don't overlap
  // (but can butt up together in time), then it will work as expected (the
  // new sequence takes over the label when it starts).  Useful for finding
  // the current "Camera 3" sequence when it is cycling between individual
  // frame sequences as animation creates and destroys frames.  Also used
  // for hit-box identification, so try to keep the camera and hit-box
  // numbers separate.

LE_SEQNCR_WatchRecord volatile LE_SEQNCR_WatchArray [CE_ARTLIB_SeqncrMaxWatches];
  // Updated with the status of sequences you are watching.  When a sequence
  // starts, it checks to see if it is mentioned in this array.  If it is, it
  // notes down the index internally and updates that entry whenever it changes.
  // If it tries to do an update and finds a different DataID/Priority in its
  // index entry, it stops doing updates.

#if CE_ARTLIB_SeqncrMaxRecolourRanges > 0
LE_SEQNCR_AutoRecolourRecord LE_SEQNCR_AutoRecolourRangesArray [CE_ARTLIB_SeqncrMaxRecolourRanges];
  // Set by the user to specify ranges of TABs that need recolouring
  // before they get shown (and also during preloading).
#endif

CRITICAL_SECTION LI_SEQNCR_UpdateCycleCriticalSection;
  // This critical section guards the update cycle.  When the update cycle
  // is going, other threads shouldn't access any of the sequencer data
  // structures since they are busy changing.  Also, access when not
  // in the critical section should be limited to mostly read only
  // activity - use the sequencer command queue to do changes since it
  // batches things up much better than individual function calls can.

LE_SEQNCR_RuntimeInfoPointer volatile LI_SEQNCR_CommonRuntimeTreeRoot;
  // The root of the active runtime sequences tree is in here, and will
  // usually be pointing at a grouping sequence which holds all the user
  // activated sequences as children.  Set to NULL when the sequencer
  // is shut down.  Non-NULL when it is running.

#if CE_ARTLIB_EnableSystemMouse
LE_DATA_DataId LE_SEQNCR_MouseGroupingSequenceDataID = LE_DATA_EmptyItem;
UNS16 LE_SEQNCR_MouseGroupingSequencePriority;
LE_REND_RenderSlotSet MouseGroupingSequenceRenderSlotSet;
static BOOL LI_SEQNCR_MouseGroupingSequenceHasBeenStarted;
  // Identifies the grouping sequence used for the mouse.  Created at
  // runtime by the LE_SEQNCR_InitialiseMouseGroupingSequence function.
#endif // CE_ARTLIB_EnableSystemMouse



/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

static LE_SEQNCR_RuntimeInfoRecord    RuntimeCommonArray [CE_ARTLIB_SeqncrMaxActiveSequences];
static LE_SEQNCR_RuntimeInfoPointer   NextFreeCommonRuntimeElement;
static LE_SEQNCR_Runtime2DInfoRecord  Runtime2DArray [CE_ARTLIB_SeqncrMax2DSequences];
static LE_SEQNCR_Runtime2DInfoPointer NextFree2DRuntimeElement;
static LE_SEQNCR_Runtime3DInfoRecord  Runtime3DArray [CE_ARTLIB_SeqncrMax3DSequences];
static LE_SEQNCR_Runtime3DInfoPointer NextFree3DRuntimeElement;
#if CE_ARTLIB_EnableDebugMode
  static UNS32                        NumberOfFreeCommonRuntimeElements;
  static UNS32                        NumberOfFree2DRuntimeElements;
  static UNS32                        NumberOfFree3DRuntimeElements;
#endif
  // These arrays store the runtime tree of active sequences.  Unused entries in
  // the array contain a single pointer, which points to the next unused entry.
  // The head of the unused list is in the NextFree... global variable.


static LE_QUEUE_Record SequencerCommandQueue;
  // This stores many of the commands from the user program.  Well, all the
  // ones that affect the runtime tree and don't return a value.  See the
  // SequencerCommand enum for details of the messages in this queue.



typedef struct ChainedItemStruct ChainedItemRecord, *ChainedItemPointer;

struct ChainedItemStruct
{
  ChainedItemPointer next;        // For going from head to tail.
  ChainedItemPointer previous;    // For going from tail to head.
  LE_QUEUE_MessageRecord message; // The user's pending command.
};


static ChainedItemRecord ChainedItemsArray [CE_ARTLIB_SeqncrMaxChainedThings];
static ChainedItemPointer WaitingChainedCommandsHead;
static ChainedItemPointer WaitingChainedCommandsTail;
static ChainedItemPointer FreeChainedCommands;
static ChainedItemPointer ExecutableChainedCommandsHead;
static ChainedItemPointer ExecutableChainedCommandsTail;
  // Chained commands get put into the WaitingChainedCommands linked list.
  // Later, when a sequence matching the chained DataID (in NumberC) and chain
  // priority (in NumberD) ends, the pending command will get executed (moved
  // to the ExecutableChainedCommands list).  The FreeChainedCommands list
  // holds free records.  The linked lists use the next and previous fields to
  // store the pointer to the next/previous item in the list.  The lists
  // are in the order submitted (items get added to the head and taken off the
  // tail), so that you can have multiple chained commands going off in a
  // predictable order (if several commands are triggered by the chain).

static int volatile CollectCommandsNestingLevel;
  // Collect and Execute commands nest, so you can do a collect/execute without
  // worrying if the caller is in the middle of a collect or not.  Greater than
  // zero means it is still busy collecting commands, the queue isn't moving.
  // Zero or negative means that commands are processed from the queue.

static LE_REND_RenderSlotSet DefaultRenderSlotSetForStartSequence = 1;
  // Identifies the render slots that the start sequence commands with no
  // explicit value will use.

static BOOL DefaultPreloadFlagForStartSequence = FALSE;
static BOOL DefaultUseRefFlagForStartSequence = FALSE;
static LE_SEQNCR_DataDisposalOptions
 DefaultDisposalFlagForStartSequence = LE_SEQNCR_DATA_DISPOSE_DO_NOTHING;
  // These flags control the default start sequence operations for preloading,
  // using reference counts, and post-unloading of the various bitmaps and
  // sounds in a grouping sequence.

static UNS16 volatile EndOfUpdateCycleCounter;
  // Counts the number of update cycles that have gone by.  Used for
  // finding out when an animation cycle has finished in another thread.

static INT32 TopLevelClock;
  // The parent clock for the top level sequence.  This is used rather than
  // the real tick count because it avoids having the time change in the
  // middle of an update cycle.

static BOOL volatile SuspendEngine = TRUE;
  // TRUE to stop the animation engine from updating.  FALSE for normal
  // operations.

static UNS8 EngineTimeMultiplierScaledBy16 = 16;
  // Multiplies the elapsed time by this number to make the engine
  // run animations faster or slower.  Divide by 16 to get the actual
  // time multiple.  A value of 16 runs it at normal speed.
  // A hack for Jeopardy guys.

#if CE_ARTLIB_EnableSystemMouse
static INT16 LI_SEQNCR_MousePreviousX;
static INT16 LI_SEQNCR_MousePreviousY;
  // Various things that keep track of the mouse position for the callback
  // function that moves the mouse pointer sequence around.
#endif // CE_ARTLIB_EnableSystemMouse



#if CE_ARTLIB_EnableDebugMode
static const char * const SequenceTypeNames [LE_CHUNK_ID_SEQ_MAX] =
{
  "Unknown",
  "GROUPING",
  "INDIRECT",
  "2DBITMAP",
  "3DMODEL",
  "SOUND",
  "VIDEO",
  "CAMERA",
  "PRELOADER",
  "3DMESH",
  "TWEEKER"
};

static const char * const EndingActionNames [LE_SEQNCR_EndingActionMAX] =
{
  "None",
  "Stop",
  "Stay",
  "Loop"
};

static const char * const CommandNames [SEQCMD_MAX] =
{
  "Start",
  "Stop",
  "StopAll",
  "Move",
  "ForgetChains",
  "Pause",
  "SetClock",
  "SetEndingAction",
  "DefaultRenderSlotSet",
  "InvalidateRect",
  "ForceRedraw",
  "SetVolume",
  "SetPan",
  "SetPitch",
  "SetSaturation",
  "SetBrightness",
  "SetContrast",
  "SetVideoAlternative",
  "ChooseVideoAlternative",
  "ForgetVideoAlternatives",
  "SetViewport",
  "SetCamera"
};

static const char * const DisposeNames [LE_SEQNCR_DATA_DISPOSE_MAX] =
{
  "",
  " Unload",
  " DeallocSubID"
};
#endif // CE_ARTLIB_EnableDebugMode



/*****************************************************************************
 * Jump the video to a new point in time, as soon as possible (dump
 * buffered data and play it right away).  The value is in ticks, not frames.
 */

static void GlueJumpVideoStream (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
INT32 NewTime)
{
#if !USE_OLD_VIDEO_PLAYER
  UNS32                             NewFrame;
#endif

  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

  VideoPntr = SequencePntr->streamSpecificData.videoStream;

#if !USE_OLD_VIDEO_PLAYER
  NewFrame = (LONG) (NewTime / (double) SequencePntr->endTime *
    VideoPntr->numberOfVideoFrames);
  if (NewFrame > VideoPntr->numberOfVideoFrames)
    NewFrame = VideoPntr->numberOfVideoFrames;

  #if CE_ARTLIB_EnableSystemVideo
    LE_VIDEO_CutVideoToFrame (VideoPntr->videoHandle, NewFrame);
  #elif CE_ARTLIB_EnableSystemBink
    BinkGoto (VideoPntr->videoHandle,
      NewFrame + 1 /* First frame is #1 in Bink */,
      0 /* Flags, use BINKGOTOQUICK to speed up jumping to non-keyframe but with lower quality */);
  #else
    #error If not using old video player, must be using new one or Bink.
  #endif

#else // Only used in old video player.
  if (VideoPntr != NULL && VideoPntr->audioStream != NULL)
  {
    // Set the audio position back to the requested time, video
    // only streams run off the sequence clock thus don't need
    // updating.  Later, when the audio updates (feeding) the
    // video frame will update to match.

    if (SequencePntr->endTime <= 0)
      VideoPntr->currentAudioFrameNumber = 0;
    else
      VideoPntr->currentAudioFrameNumber = (UNS32)
      ((double) VideoPntr->numberOfAudioFrames * NewTime /
      SequencePntr->endTime);

    if (VideoPntr->currentAudioFrameNumber >= VideoPntr->numberOfAudioFrames)
    {
      // Don't set it to the very last frame, otherwise it won't have
      // fed the last bit of data to the sound system and the sound
      // will not report the right position, and the video won't end.

      VideoPntr->currentAudioFrameNumber = VideoPntr->numberOfAudioFrames - 1;
    }

    // Tell it to toss away the sound it had buffered ahead of time,
    // so that it starts playing our jump right away.

    VideoPntr->jumpCutAtNextFeeding = TRUE;
    VideoPntr->lastAudioFeedTime = LE_TIME_TickCount - DELAY_BETWEEN_VIDEO_FEEDS;
  }
#endif
}



/*****************************************************************************
 * Feeds the video stream data from the movie file and decompresses one
 * image.  The feeding fills sound buffers and updates the frame number
 * and also updates the sequence clock to match the frame number.
 *
 * If the video is being drawn directly to the screen then it will
 * use FitIntoRectPntr, ClipToRectPntr, ScreenBitsPntr and
 * ScreenPixelsToNextScanLine to control the drawing.  The image will be
 * stretched, shrunk and moved to fit into FitIntoRectPntr (in screen
 * coordinates).  Parts of the image sticking out past ClipToRectPntr
 * (usually set to the size of the visible screen) don't get drawn
 * (the FitIntoRectPntr can be partially or fully off the clipped area).
 * ScreenBitsPntr points to the start of screen memory,
 * ScreenPixelsToNextScanLine is the number of pixels involved.
 * The depth is assumed to be the same as the settings used when
 * the video was started.
 *
 * If it is being drawn to a temporary buffer, then this function sets
 * the user's IntermediateBitmapPntrPntr to point to our temporary
 * bitmap's info and IntermediateBitmapBitsPntrPntr to point to the
 * actual pixels.  The ScreenBitsPntr and other variables are not used.
 *
 * If an error happens, the function returns FALSE and sets the sequence
 * up to end at the next update (clock advanced to ending time and
 * ending action changed to STOP).
 *
 * Note - don't display debug messages here since the screen memory is
 * usually locked when this function is called.
 */

BOOL LI_SEQNCR_GlueFeedAndDrawVideo (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
BITMAPINFOHEADER *IntermediateBitmapInfoPntr, BYTE **IntermediateBitmapBitsPntrPntr,
int OffsetX, int OffsetY, BYTE *ScreenBitsPntr)
{
#if USE_OLD_VIDEO_PLAYER
  LPBITMAPINFOHEADER  BitmapInfoPntr;
  LONG                BytesRead;
  HRESULT             ErrorCode;
  BOOL                InputUsed;
  int                 IterationCount;
  BYTE                LocalBuffer [VIDEO_FEED_DURATION *
                      44200 /* fastest audio speed is 44.1Khz */ *
                      4 /* largest sample size is stereo 16 bit */ /
                      CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ];
  LONG                MaxFeedSizeInSamples;
  LONG                NextAudioFrame;
  LONG                NowPlayingAudioFrame;
  BOOL                OutputIsFull;
  LONG                SamplesRead;
  LONG                ThisFeedSizeInSamples;
#endif

  UNS32               CopySize;
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

  VideoPntr = SequencePntr->streamSpecificData.videoStream;

#if CE_ARTLIB_EnableSystemVideo

  // Calculate the frame number for silent films.

  if (!VideoPntr->enableAudio)
    VideoPntr->currentVideoFrameNumber =
    (UNS32) (((float) SequencePntr->sequenceClock /
    (float) CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ) *
    VideoPntr->videoFramesPerSecond);

  // If using an intermediate bitmap, draw to it instead of the screen.

  if (VideoPntr->intermediateBitmapID != LE_DATA_EmptyItem)
  {
    ScreenBitsPntr =
      LE_GRAFIX_LockObjectDataPtr (VideoPntr->intermediateBitmapID);
    OffsetX = 0;
    OffsetY = 0;
  }

  // Fill buffers, play sound and decompress an image.

  if (!LE_VIDEO_FeedVideo (VideoPntr->videoHandle,
  &VideoPntr->currentVideoFrameNumber, ScreenBitsPntr,
  OffsetX, OffsetY,
  VideoPntr->brightness, VideoPntr->contrast, VideoPntr->saturation))
    goto ErrorExit;

  // Pass the intermediate bitmap with the new image back to the user,
  // if there is an intermediate bitmap (otherwise we are just drawing
  // on the screen directly).

  if (VideoPntr->intermediateBitmapID != LE_DATA_EmptyItem)
  {
    if (IntermediateBitmapInfoPntr != NULL)
    {
      CopySize = IntermediateBitmapInfoPntr->biSize;
      if (CopySize > VideoPntr->bitmapInfo.bV4Size)
        CopySize = VideoPntr->bitmapInfo.bV4Size;
      memcpy (IntermediateBitmapInfoPntr, &VideoPntr->bitmapInfo, CopySize);
      IntermediateBitmapInfoPntr->biSize = CopySize;
    }

    if (IntermediateBitmapBitsPntrPntr != NULL)
      *IntermediateBitmapBitsPntrPntr = ScreenBitsPntr;

    LE_GRAFIX_UnlockObjectDataPtr (VideoPntr->intermediateBitmapID);
  }

  goto NormalExit;

#elif CE_ARTLIB_EnableSystemBink

  // Feed Bink, until it is ready to draw another frame.  Do nothing
  // if it is not ready yet.

  if (BinkWait (VideoPntr->videoHandle))
    goto NormalExit; // Not ready yet.

  BinkDoFrame (VideoPntr->videoHandle); // Decompress the frame.

  // If using an intermediate bitmap, draw to it instead of the screen.

  if (VideoPntr->intermediateBitmapID != LE_DATA_EmptyItem)
  {
    ScreenBitsPntr =
      LE_GRAFIX_LockObjectDataPtr (VideoPntr->intermediateBitmapID);
    OffsetX = 0;
    OffsetY = 0;
  }

  // Copy the image from the Bink internal buffer to our output buffer.

  BinkCopyToBuffer (VideoPntr->videoHandle, ScreenBitsPntr,
    VideoPntr->outputPitch, VideoPntr->bitmapInfo.bV4Height,
    OffsetX, OffsetY,
    VideoPntr->outputDepthFlags);

  // Pass the intermediate bitmap with the new image back to the user,
  // if there is an intermediate bitmap (otherwise we are just drawing
  // on the screen directly).

  if (VideoPntr->intermediateBitmapID != LE_DATA_EmptyItem)
  {
    if (IntermediateBitmapInfoPntr != NULL)
    {
      CopySize = IntermediateBitmapInfoPntr->biSize;
      if (CopySize > VideoPntr->bitmapInfo.bV4Size)
        CopySize = VideoPntr->bitmapInfo.bV4Size;
      memcpy (IntermediateBitmapInfoPntr, &VideoPntr->bitmapInfo, CopySize);
      IntermediateBitmapInfoPntr->biSize = CopySize;
    }

    if (IntermediateBitmapBitsPntrPntr != NULL)
      *IntermediateBitmapBitsPntrPntr = ScreenBitsPntr;

    LE_GRAFIX_UnlockObjectDataPtr (VideoPntr->intermediateBitmapID);
  }

  // Update the current frame number and advance to the next frame.
  // Bink automatically goes back to the first frame when it hits
  // the end, so detect that if possible.  If not looping, just
  // stay on the last frame.

  VideoPntr->currentVideoFrameNumber = VideoPntr->videoHandle->FrameNum - 1;

  if (SequencePntr->endingAction == LE_SEQNCR_EndingActionLoopToBeginning ||
  VideoPntr->videoHandle->FrameNum < VideoPntr->videoHandle->Frames)
    BinkNextFrame (VideoPntr->videoHandle);

  goto NormalExit;

#else // Old video player.

  if (VideoPntr->audioStream != NULL)
  {
    // Only feed sound every few seconds, to avoid disk seeking.

    if (LE_TIME_TickCount >=
    VideoPntr->lastAudioFeedTime + DELAY_BETWEEN_VIDEO_FEEDS)
    {
      VideoPntr->lastAudioFeedTime = LE_TIME_TickCount;

      // Feed it in half second spoonfuls, until it is full or
      // we get tired of feeding it (don't want to spend too much
      // time feeding since it slows down the rest of the game -
      // which happens when a video starts and the buffers are empty,
      // or when doing a seek in a video using jump cuts).

      MaxFeedSizeInSamples = VideoPntr->audioStreamInfo.dwRate /
        VideoPntr->audioStreamInfo.dwScale * VIDEO_FEED_DURATION /
        CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ;

      IterationCount = 0;
      while (IterationCount++ < VIDEO_MAX_FEED_ITERATIONS)
      {
        ThisFeedSizeInSamples =
          VideoPntr->numberOfAudioFrames - VideoPntr->currentAudioFrameNumber;

        if (ThisFeedSizeInSamples > MaxFeedSizeInSamples)
          ThisFeedSizeInSamples = MaxFeedSizeInSamples;

        if (ThisFeedSizeInSamples <= 0)
        {
          // Reached the end of the sound data, what to do?

          if (SequencePntr->endingAction == LE_SEQNCR_EndingActionLoopToBeginning)
          {
            // Go back and start filling from the beginning again.

            VideoPntr->currentAudioFrameNumber = 0;
            IterationCount--; // Don't count this as an iteration.
            continue; // Try filling again.
          }
          else
          {
            // Fill the remaining part of the buffer with silence,

            LE_SOUND_FeedBufSnd (VideoPntr->soundBuffer,
              0, NULL, NULL, 0, NULL, NULL, NULL, 0, 0, FALSE);
          }
          break;
        }

        ErrorCode = AVIStreamRead (VideoPntr->audioStream,
          VideoPntr->currentAudioFrameNumber,
          ThisFeedSizeInSamples,
          LocalBuffer, sizeof (LocalBuffer),
          &BytesRead, &SamplesRead);

        if (ErrorCode != 0)
          goto ErrorExit; // IO error while reading.

        NextAudioFrame = VideoPntr->currentAudioFrameNumber + SamplesRead;

        if (BytesRead > 0)
        {
          if (!LE_SOUND_FeedBufSnd (VideoPntr->soundBuffer,
          BytesRead, LocalBuffer, &InputUsed,
          0, NULL, NULL, &OutputIsFull,
          VideoPntr->currentAudioFrameNumber,
          NextAudioFrame, VideoPntr->jumpCutAtNextFeeding))
            goto ErrorExit; // Feeding problem.

          if (!InputUsed && !OutputIsFull)
          {
            // Sometimes there are dregs in the internal sound buffers
            // which stop it from using our data.  The previous call
            // flushed them out, so try again now (to avoid re-reading
            // the input).

            if (!LE_SOUND_FeedBufSnd (VideoPntr->soundBuffer,
            BytesRead, LocalBuffer, &InputUsed,
            0, NULL, NULL, &OutputIsFull,
            VideoPntr->currentAudioFrameNumber,
            NextAudioFrame, VideoPntr->jumpCutAtNextFeeding))
              goto ErrorExit; // Feeding problem.
          }

          VideoPntr->jumpCutAtNextFeeding = FALSE;

          if (InputUsed)
          {
            // Ah ha, it used our data.  Note that sometimes it has internal
            // data it needs to play before it uses our data.

            VideoPntr->currentAudioFrameNumber = NextAudioFrame;
          }

          if (OutputIsFull)
            break; // Stop feeding when it is full.
        }
        else // Usually shouldn't get here.
        {
          // Fill the remaining part of the buffer with silence.

          LE_SOUND_FeedBufSnd (VideoPntr->soundBuffer,
            0, NULL, NULL, 0, NULL, NULL, NULL, 0, 0, FALSE);
          break;
        }
      }
    }

    // Update the video frame number based on the audio play position.
    // Note that frame numbers vary from 0 to N-1.  If you try to display
    // frame N, it will fail horribly.  But internally we use N to signal
    // that the video is over.

    NowPlayingAudioFrame = LE_SOUND_GetTimeOfBufSnd (VideoPntr->soundBuffer);

    VideoPntr->currentVideoFrameNumber = (LONG) ((double) NowPlayingAudioFrame *
      VideoPntr->numberOfVideoFrames / VideoPntr->numberOfAudioFrames);
  }
  else // No sound.  The clock chooses the frame number.
  {
    VideoPntr->currentVideoFrameNumber =
      (UNS32) (((float) SequencePntr->sequenceClock /
      (float) CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ) *
      VideoPntr->videoFramesPerSecond);
  }

  if (VideoPntr->currentVideoFrameNumber > VideoPntr->numberOfVideoFrames)
    VideoPntr->currentVideoFrameNumber = VideoPntr->numberOfVideoFrames;

  // Now try to decompress an image, if there are images.

  if (VideoPntr->videoGetFrameFunction != NULL)
  {
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

#if CE_ARTLIB_EnableDebugMode // Collect some statistics.
    VideoPntr->videoFramesActuallyRendered++;  // One more frame!
    if (VideoPntr->firstVideoShownAtTime == 0)
      VideoPntr->firstVideoShownAtTime = LE_TIME_TickCount;
#endif

    // Pass the intermedate bitmap with the new image back to the user.

    if (IntermediateBitmapInfoPntr != NULL)
    {
      CopySize = IntermediateBitmapInfoPntr->biSize;
      if (CopySize > BitmapInfoPntr->biSize)
        CopySize = BitmapInfoPntr->biSize;
      memcpy (IntermediateBitmapInfoPntr, BitmapInfoPntr, CopySize);
      IntermediateBitmapInfoPntr->biSize = CopySize;
    }

    if (IntermediateBitmapBitsPntrPntr != NULL)
      *IntermediateBitmapBitsPntrPntr =
      ((LPBYTE) BitmapInfoPntr) + BitmapInfoPntr->biSize;
  }

  // Check for alternative path through the video - if we hit the
  // switch point, then maybe start feeding the alternative sound
  // on the next feeding.  Note that we show the decision frame.

  if (VideoPntr->alternativeDecisionFrame >= 0 &&
  (INT32) VideoPntr->currentVideoFrameNumber >= VideoPntr->alternativeDecisionFrame)
  {
#if CE_ARTLIB_EnableDebugMode
    char TempString [128];
    sprintf (TempString, "L_SEQNCR.cpp LI_SEQNCR_GlueFeedAndDrawVideo: "
      "Jump%s taken from frame %d (actually %d) to frame %d.\r\n",
      VideoPntr->takeAlternative ? "" : " not",
      (int) VideoPntr->alternativeDecisionFrame,
      (int) VideoPntr->currentVideoFrameNumber,
      (int) VideoPntr->alternativeJumpToFrame);
    LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);
#endif

    if (VideoPntr->takeAlternative)
    {
      GlueJumpVideoStream (SequencePntr,
        (INT32) ((VideoPntr->alternativeJumpToFrame + 0.5) /
        VideoPntr->videoFramesPerSecond *
        CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ));
    }

    LE_UIMSG_SendEvent (UIMSG_VIDEO_JUMP,
      VideoPntr->alternativeDecisionFrame,
      VideoPntr->alternativeJumpToFrame,
      VideoPntr->takeAlternative,
      0, 0, NULL, 0);

    // Alternative point used up.
    VideoPntr->alternativeDecisionFrame = -1;
  }

  goto NormalExit;
#endif // CE_ARTLIB_EnableSystemVideo etc.


NormalExit:
  return TRUE;

#if !CE_ARTLIB_EnableSystemBink // Bink never has errors.
ErrorExit: // Something went wrong, make the movie stop soon.
  SequencePntr->endingAction = LE_SEQNCR_EndingActionSuicide;
  return FALSE;
#endif
}



/*****************************************************************************
 * Returns the current frame number which should be showing, usually this is
 * based on the audio play position.  Doesn't do any feeding or image
 * decompression, just checks the time.
 */

static UNS32 GlueGetDesiredFrameNumber (LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
#if CE_ARTLIB_EnableSystemVideo
  LE_VIDEO_VideoStatusRecord        VideoStatus;
#endif

  UNS32                             DesiredFrameNumber;
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

  VideoPntr = SequencePntr->streamSpecificData.videoStream;

  if (VideoPntr->enableAudio)
  {
#if CE_ARTLIB_EnableSystemVideo
    LE_VIDEO_GetVideoStatus (VideoPntr->videoHandle, &VideoStatus);
    DesiredFrameNumber = VideoStatus.desiredFrame;
#elif CE_ARTLIB_EnableSystemBink
    DesiredFrameNumber = VideoPntr->videoHandle->FrameNum - 1;
#else // Old video player.
    DesiredFrameNumber = (LONG) (
      (double) LE_SOUND_GetTimeOfBufSnd (VideoPntr->soundBuffer) *
      VideoPntr->numberOfVideoFrames / VideoPntr->numberOfAudioFrames);
#endif
  }
  else // Just runs by the clock time.
  {
    DesiredFrameNumber =
      (UNS32) (((float) SequencePntr->sequenceClock /
      (float) CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ) *
      VideoPntr->videoFramesPerSecond);
  }

  if (DesiredFrameNumber > VideoPntr->numberOfVideoFrames)
    DesiredFrameNumber = VideoPntr->numberOfVideoFrames;
  return DesiredFrameNumber;
}



/*****************************************************************************
 * Detects stalled videos.  These happen when the audio isn't fed long enough
 * and then it keeps on returning the same frame.  The audio then doesn't
 * get fed because the frame number hasn't changed.  If that situation has
 * been happening for a while (and the sequence isn't paused), then
 * return TRUE.
 */

static BOOL GlueVideoIsStalled (LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

  if (SequencePntr == NULL)
    return FALSE;

  if (SequencePntr->paused)
    return FALSE;

  VideoPntr = SequencePntr->streamSpecificData.videoStream;
  if (VideoPntr == NULL)
    return FALSE;


  if (VideoPntr->frameNumberAtLastChange != VideoPntr->currentVideoFrameNumber)
  {
    VideoPntr->timeOfLastFrameNumberChange = LE_TIME_TickCount;
    VideoPntr->frameNumberAtLastChange = VideoPntr->currentVideoFrameNumber;
    return FALSE;
  }

  if (LE_TIME_TickCount - VideoPntr->timeOfLastFrameNumberChange <
  CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ * 2)
    return FALSE;

#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.cpp GlueVideoIsStalled: "
    "Stalled video detected!  Frame number stuck at %d for the last %d ticks.\r\n",
    (int) VideoPntr->currentVideoFrameNumber,
    (int) (LE_TIME_TickCount - VideoPntr->timeOfLastFrameNumberChange));
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
    (VideoPntr->currentVideoFrameNumber == 0)
    ? LE_ERROR_DebugMsgToFile /* Frame zero often gets stuck */
    : LE_ERROR_DebugMsgToFileAndScreen);
#endif

  // Stop kicking it for a little while.
  VideoPntr->timeOfLastFrameNumberChange = LE_TIME_TickCount;

  return TRUE;
}



/*****************************************************************************
 * Sets the volume of sound in a video.
 */

static BOOL GlueSetVideoVolume (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
UNS8 VolumePercentage)
{
#if CE_ARTLIB_EnableSystemVideo
  LE_VIDEO_VideoStatusRecord        VideoStatus;
#endif
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

  if (SequencePntr == NULL ||
  (VideoPntr = SequencePntr->streamSpecificData.videoStream) == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c GlueSetVideoVolume: "
      "NULL buffer stuff in video sound, something is wrong!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

#if CE_ARTLIB_EnableSystemVideo
  if (!LE_VIDEO_GetVideoStatus (VideoPntr->videoHandle, &VideoStatus))
    return FALSE;
  return LE_SOUND_SetVolumeBufSnd (VideoStatus.soundHandle, VolumePercentage);
#elif CE_ARTLIB_EnableSystemBink
  BinkSetVolume (VideoPntr->videoHandle, VolumePercentage * (int) 32768 / 100);
  return TRUE;
#else // Old video player.
  return LE_SOUND_SetVolumeBufSnd (VideoPntr->soundBuffer, VolumePercentage);
#endif
}



/*****************************************************************************
 * Sets the left/right panning of sound in a video.
 */

static BOOL GlueSetVideoPan (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
INT8 PanPercentage)
{
#if CE_ARTLIB_EnableSystemVideo
  LE_VIDEO_VideoStatusRecord        VideoStatus;
#endif
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

  if (SequencePntr == NULL ||
  (VideoPntr = SequencePntr->streamSpecificData.videoStream) == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c GlueSetVideoPan: "
      "NULL buffer stuff in video sound, something is wrong!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

#if CE_ARTLIB_EnableSystemVideo
  if (!LE_VIDEO_GetVideoStatus (VideoPntr->videoHandle, &VideoStatus))
    return FALSE;
  return LE_SOUND_SetPanBufSnd (VideoStatus.soundHandle, PanPercentage);
#elif CE_ARTLIB_EnableSystemBink
  BinkSetPan (VideoPntr->videoHandle, (PanPercentage + (int) 100) * 32768 / 100);
  return TRUE;
#else // Old video player.
  return LE_SOUND_SetPanBufSnd (VideoPntr->soundBuffer, PanPercentage);
#endif
}



/*****************************************************************************
 * Sets the pitch of sound in a video.
 */

static BOOL GlueSetVideoPitch (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
UNS32 NewPitch)
{
#if CE_ARTLIB_EnableSystemVideo
  LE_VIDEO_VideoStatusRecord        VideoStatus;
#endif
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

  if (SequencePntr == NULL ||
  (VideoPntr = SequencePntr->streamSpecificData.videoStream) == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c GlueSetVideoPitch: "
      "NULL buffer stuff in video sound, something is wrong!\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

#if CE_ARTLIB_EnableSystemVideo
  if (!LE_VIDEO_GetVideoStatus (VideoPntr->videoHandle, &VideoStatus))
    return FALSE;
  return LE_SOUND_SetPitchBufSnd (VideoStatus.soundHandle, NewPitch);
#elif CE_ARTLIB_EnableSystemBink
  return FALSE; // Can't change pitch in Bink.
#else // Old video player.
  return LE_SOUND_SetPitchBufSnd (VideoPntr->soundBuffer, NewPitch);
#endif
}



#if USE_OLD_VIDEO_PLAYER
/*****************************************************************************
 * Stops the streaming of video and audio, closes the audio channel and
 * deallocates the GetFrame.  This can be done as part of closing or just
 * rewinding the streams back to the start - it doesn't actually close the
 * streams.
 */

static void StopVideoStreaming (LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr)
{
#if CE_ARTLIB_EnableDebugMode
  TYPE_Tick CurrentTime;

  CurrentTime = LE_TIME_TickCount;

  if (VideoPntr->firstVideoShownAtTime != 0 &&
  CurrentTime > VideoPntr->firstVideoShownAtTime)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c StopVideoStreaming: "
      "Frame rate of %f achieved (displayed %d frames of %d from time %d to %d).\r\n",
      (double) VideoPntr->videoFramesActuallyRendered /
      (CurrentTime - VideoPntr->firstVideoShownAtTime) *
      CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ,
      (int) VideoPntr->videoFramesActuallyRendered,
      (int) VideoPntr->numberOfVideoFrames,
      (int) VideoPntr->firstVideoShownAtTime,
      (int) CurrentTime);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
  }
  else
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c StopVideoStreaming: "
      "Unable to determine frame rate, have %d frames from time %d to %d.\r\n",
      (int) VideoPntr->videoFramesActuallyRendered,
      (int) VideoPntr->firstVideoShownAtTime,
      (int) CurrentTime);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
  }
#endif

  if (VideoPntr->soundBuffer != NULL)
  {
    LE_SOUND_PauseBufSnd (VideoPntr->soundBuffer, TRUE);
  }

  if (VideoPntr->audioStreamingStarted)
  {
    AVIStreamEndStreaming (VideoPntr->audioStream);
    VideoPntr->audioStreamingStarted = FALSE;
  }

  if (VideoPntr->videoGetFrameFunction != NULL)
  {
    AVIStreamGetFrameClose (VideoPntr->videoGetFrameFunction);
    VideoPntr->videoGetFrameFunction = NULL;
  }

  if (VideoPntr->videoStreamingStarted)
  {
    AVIStreamEndStreaming (VideoPntr->videoStream);
    VideoPntr->videoStreamingStarted = FALSE;
  }
}
#endif // Old video player.



#if USE_OLD_VIDEO_PLAYER
/*****************************************************************************
 * Starts the streaming of video and audio, opens an audio channel and
 * makes a GetFrame.  Assumes that the relevant streams are already open.
 * Returns TRUE if successful.  Can also be used to restart streaming
 * for looping videos.
 */

static BOOL StartVideoStreaming (LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr)
{
  struct MyBitmapInfoStruct {
    BITMAPINFOHEADER  bmiHeader;
    DWORD             bmiColours[4];
  } MyBitmapInfo;

  LONG          BufferSize;
  HRESULT       ErrorCode;
  BOOL          ReturnCode = FALSE;
  LPBITMAPINFO  TheirBitmapInfo;

  // First start up the sound stream.

  if (VideoPntr->audioStream != NULL)
  {
    // Tell the audio stream to start loading data, over the whole file,
    // starting at the beginning.

    ErrorCode = AVIStreamBeginStreaming (VideoPntr->audioStream, 0,
      VideoPntr->numberOfAudioFrames, 1000);
    if (ErrorCode != 0) goto ErrorExit;

    VideoPntr->audioStreamingStarted = TRUE;
  }

  // Start up the video stream.

  if (VideoPntr->videoStream != NULL)
  {
    // Find out the stream bitmap format.

    BufferSize = sizeof (MyBitmapInfo);
    memset (&MyBitmapInfo, 0, sizeof (MyBitmapInfo));
    MyBitmapInfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
    ErrorCode = AVIStreamReadFormat (VideoPntr->videoStream,
      0 /* Position in stream */, &MyBitmapInfo, &BufferSize);
    if (ErrorCode != 0) goto ErrorExit;

    // Tell the video stream to get ready to stream.

#if 0 /* Well, don't do it since the GetFrame function will do it for us. */
    ErrorCode = AVIStreamBeginStreaming (VideoPntr->videoStream,
      VideoPntr->currentVideoFrameNumber,
      VideoPntr->numberOfVideoFrames,
      1000);
    if (ErrorCode != 0) goto ErrorExit;

    VideoPntr->videoStreamingStarted = TRUE;
#endif

    // Hack it up to match the format we are using for the screen.
    // Request a top down bitmap (negative height) and use the colour
    // masks to assign RGB to bits.

    MyBitmapInfo.bmiHeader.biHeight = - abs (MyBitmapInfo.bmiHeader.biHeight);
    MyBitmapInfo.bmiHeader.biBitCount = (WORD) LE_GRAFIX_ScreenBitsPerPixel;

    // Really should be BI_BIFIELDS but the codecs barf over that, so just use
    // RGB and hope it gets the bit positions right for the various modes,
    // presumably it uses the default bitfields from the video display driver.
    // Oh, and Indeo 5 doesn't work in 32 bits.  But Cinepak does.

    MyBitmapInfo.bmiHeader.biCompression = BI_RGB;

    MyBitmapInfo.bmiHeader.biSizeImage =
      ((MyBitmapInfo.bmiHeader.biWidth * (MyBitmapInfo.bmiHeader.biBitCount / 8)) + 3) & ~3;
    MyBitmapInfo.bmiHeader.biSizeImage *= abs (MyBitmapInfo.bmiHeader.biHeight);

    // Just in case someone peeks at the mask area in the future...

    MyBitmapInfo.bmiColours[0] = LE_BLT_dwRedMask;
    MyBitmapInfo.bmiColours[1] = LE_BLT_dwGreenMask;
    MyBitmapInfo.bmiColours[2] = LE_BLT_dwBlueMask;

    // Initialize GetFrame video decompression routine.  Unfortunately we
    // can't use the bitmap info we have since some PCs use their own
    // ideas of the bit masks and get it wrong (pity the BI_BIFIELDS doesn't
    // work).  Instead, request a display mode that matches the current
    // screen (but that is the Windows screen not the DirectDraw surface,
    // but it works anyways).

//    VideoPntr->videoGetFrameFunction = AVIStreamGetFrameOpen (
//      VideoPntr->videoStream, &MyBitmapInfo.bmiHeader);

    VideoPntr->videoGetFrameFunction = AVIStreamGetFrameOpen (
      VideoPntr->videoStream,
      (LPBITMAPINFOHEADER) AVIGETFRAMEF_BESTDISPLAYFMT);

    if (VideoPntr->videoGetFrameFunction == NULL)
      goto ErrorExit;

    // Try reading a later frame to see what bitmap format we actually got,
    // this also makes it load up its disk cache so that it runs smoother
    // once it is going.

    TheirBitmapInfo = (LPBITMAPINFO) AVIStreamGetFrame (VideoPntr->videoGetFrameFunction,
      (VideoPntr->numberOfVideoFrames >= 2) ? 1 : 0  /* Ask for second frame
      in the video so it refreshes image right way up when we later ask for frame 0 */);
    if (TheirBitmapInfo == NULL) goto ErrorExit;

    // This little hack (bleeble - seems to work with Cinepak & Indeo) makes
    // the system flip the video over, without our specifying a video format.
    // If it doesn't work, a call to IGetFrame::SetFormat may do the trick,
    // but that may have the bitfield problem.  We want flipped, since it
    // appears the right way up.

    if (VideoPntr->flipVertically)
    {
      // Want a positive height so that image is upside down.
      if (TheirBitmapInfo->bmiHeader.biHeight < 0)
        TheirBitmapInfo->bmiHeader.biHeight = -TheirBitmapInfo->bmiHeader.biHeight;
    }
    else // Not flipped on the screen, which means we have to flip it internally.
    {
      // Want a negative height so image is right side up.
      if (TheirBitmapInfo->bmiHeader.biHeight > 0)
        TheirBitmapInfo->bmiHeader.biHeight = -TheirBitmapInfo->bmiHeader.biHeight;
    }

    // Preload the first frame.

    TheirBitmapInfo = (LPBITMAPINFO) AVIStreamGetFrame (VideoPntr->videoGetFrameFunction,
      0 /* Ask for frame 0 so that the buffer contents will be the desired way up */);
    if (TheirBitmapInfo == NULL) goto ErrorExit;
  }

#if CE_ARTLIB_EnableDebugMode
  // Reset the statistics.
  VideoPntr->videoFramesActuallyRendered = 0;
#endif

  ReturnCode = TRUE;
  goto NormalExit;

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c StartVideoStreaming: "
    "Unable to start video streaming, error code %d.\r\n", (int) ErrorCode);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:
  return ReturnCode;
}
#endif // Old video player.



/*****************************************************************************
 * Set up an alternative path through the video.
 */

static void GlueVideoSetUpAlternativePath (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
INT32 DecisionFrame, INT32 JumpToFrame)
{
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

#if CE_ARTLIB_EnableDebugMode
  char TempString [128];
  sprintf (TempString, "L_SEQNCR.cpp GlueVideoSetUpAlternativePath: "
    "Alternative created at frame %d to jump to frame %d.\r\n",
    (int) DecisionFrame, (int) JumpToFrame);
  LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);
#endif

  VideoPntr = SequencePntr->streamSpecificData.videoStream;

#if CE_ARTLIB_EnableSystemVideo
  LE_VIDEO_SetUpVideoAlternative (VideoPntr->videoHandle,
    DecisionFrame, JumpToFrame);
#elif CE_ARTLIB_EnableSystemBink
  // Not implemented yet.
#else
  VideoPntr->alternativeDecisionFrame = DecisionFrame;
  VideoPntr->alternativeJumpToFrame = JumpToFrame;
  VideoPntr->takeAlternative = FALSE;
#endif // CE_ARTLIB_EnableSystemVideo
}



/*****************************************************************************
 * Pick from amoung alternative paths through a video.
 */

static void GlueVideoChooseAlternativePath (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
INT32 DecisionFrame, BOOL TakeAlternative)
{
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

#if CE_ARTLIB_EnableDebugMode
  char TempString [128];
  sprintf (TempString, "L_SEQNCR.cpp GlueVideoChooseAlternativePath: "
    "Alternative at frame %d jump decision changed to %s.\r\n",
    (int) DecisionFrame, TakeAlternative ? "TRUE" : "FALSE");
  LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);
#endif

  VideoPntr = SequencePntr->streamSpecificData.videoStream;

#if CE_ARTLIB_EnableSystemVideo
  LE_VIDEO_ChangeVideoAlternative (VideoPntr->videoHandle,
    DecisionFrame, TakeAlternative);
#elif CE_ARTLIB_EnableSystemBink
  // Not implemented yet.
#else
  VideoPntr->takeAlternative = TakeAlternative;
#endif // CE_ARTLIB_EnableSystemVideo
}



/*****************************************************************************
 * Forget all alternative paths.
 */

static void GlueVideoForgetAlternativePaths (LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

#if CE_ARTLIB_EnableDebugMode
  char TempString [128];
  sprintf (TempString, "L_SEQNCR.cpp GlueVideoForgetAlternativePaths: "
    "Forgetting all alternative video paths.\r\n");
  LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);
#endif

  VideoPntr = SequencePntr->streamSpecificData.videoStream;

#if CE_ARTLIB_EnableSystemVideo
  LE_VIDEO_ForgetVideoAlternatives (VideoPntr->videoHandle);
#elif CE_ARTLIB_EnableSystemBink
  // Not implemented yet.
#else
  VideoPntr->alternativeDecisionFrame = -1;
#endif // CE_ARTLIB_EnableSystemVideo
}



/*****************************************************************************
 * Deallocates the stream related stuff for a video/audio stream.  Assumes
 * that the RuntimeInfoPntr->streamSpecificData.videoStream record will be
 * deallocated by someone else after this function.
 */

static void GlueCloseVideoStream (LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

  if (SequencePntr == NULL ||
  SequencePntr->streamSpecificData.videoStream == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("L_SEQNCR.c GlueCloseVideoStream: Bad arguments.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return;
  }

  VideoPntr = SequencePntr->streamSpecificData.videoStream;

#if CE_ARTLIB_EnableSystemVideo
  if (VideoPntr->videoHandle != NULL)
  {
    LE_VIDEO_StopVideo (VideoPntr->videoHandle);
    VideoPntr->videoHandle = NULL;
  }
#elif CE_ARTLIB_EnableSystemBink
  if (VideoPntr->videoHandle != NULL)
  {
    BinkClose (VideoPntr->videoHandle);
    VideoPntr->videoHandle = NULL;
  }
#endif

#if !USE_OLD_VIDEO_PLAYER
  if (VideoPntr->intermediateBitmapID != LE_DATA_EmptyItem)
  {
    LE_DATA_FreeRuntimeDataID (VideoPntr->intermediateBitmapID);
    VideoPntr->intermediateBitmapID = LE_DATA_EmptyItem;
  }

#else // Old video player.

  StopVideoStreaming (VideoPntr);

  if (VideoPntr->soundBuffer != NULL)
  {
    LE_SOUND_StopBufSnd (VideoPntr->soundBuffer);
    VideoPntr->soundBuffer = NULL;
  }

  if (VideoPntr->audioStream != NULL)
  {
    AVIStreamRelease (VideoPntr->audioStream);
    VideoPntr->audioStream = NULL;
  }

  if (VideoPntr->videoStream != NULL)
  {
    AVIStreamRelease (VideoPntr->videoStream);
    VideoPntr->videoStream = NULL;
  }

  if (VideoPntr->fileHandle != NULL)
  {
    AVIFileRelease (VideoPntr->fileHandle);
    VideoPntr->fileHandle = NULL;
  }
#endif // !USE_OLD_VIDEO_PLAYER
}



/*****************************************************************************
 * A utility function for finding out about the characteristics of an AVI
 * file, returns TRUE if successful.  Doesn't work for AVIs which have
 * no video (use the sound system instead!).  Also works with Bink files
 * if you are running that video player.
 */

BOOL LE_SEQNCR_GetAVIFileCharacteristics (char *FileName,
float *FramesPerSecond, UNS32 *NumberOfFrames, RECT *BoundingRectangle)
{
#if CE_ARTLIB_EnableSystemBink
  HBINK             BinkFile = NULL;
#else
  LONG              AmountToRead;
  MMCKINFO          ChunkRIFFAVI;
  MMCKINFO          ChunkLISThdrl;
  MMCKINFO          ChunkLISTstrl;
  MMCKINFO          Chunkstrf;
  MMCKINFO          Chunkstrh;
  HMMIO             MovieFile = NULL;
  SMALL_RECT        SmallRect;
  BITMAPINFOHEADER  StreamFormat;
  AVIStreamHeader   StreamHeader;
#endif

  MMRESULT          ErrorCode = 1;
  BOOL              ReturnCode = FALSE;

  if (FileName == NULL || FileName[0] == 0)
    goto ErrorExit; // No file name supplied.

#if CE_ARTLIB_EnableSystemBink
  BinkFile = BinkOpen (FileName, BINKNOFILLIOBUF | BINKNOTHREADEDIO);
  if (BinkFile == NULL)
    goto ErrorExit;

  if (FramesPerSecond != NULL)
    *FramesPerSecond = BinkFile->FrameRate / (float) BinkFile->FrameRateDiv;

  if (NumberOfFrames != NULL)
    *NumberOfFrames = BinkFile->Frames - 1; // The count one extra for some reason.

  if (BoundingRectangle != NULL)
  {
    BoundingRectangle->left = 0;
    BoundingRectangle->top = 0;
    BoundingRectangle->right = BinkFile->Width;
    BoundingRectangle->bottom = BinkFile->Height;
  }

  ReturnCode = TRUE;

#else // An AVI style file.  Parse it.

  MovieFile = mmioOpen (FileName, NULL, MMIO_DENYWRITE | MMIO_READ);
  if (MovieFile == NULL)
    goto ErrorExit;

  memset (&ChunkRIFFAVI, 0, sizeof (MMCKINFO));
  ChunkRIFFAVI.ckid = formtypeAVI;
  ErrorCode = mmioDescend (MovieFile, &ChunkRIFFAVI, NULL, MMIO_FINDRIFF);
  if (ErrorCode != MMSYSERR_NOERROR)
    goto ErrorExit;

  memset (&ChunkLISThdrl, 0, sizeof (MMCKINFO));
  ChunkLISThdrl.ckid = listtypeAVIHEADER;
  ErrorCode = mmioDescend (MovieFile, &ChunkLISThdrl, &ChunkRIFFAVI, MMIO_FINDLIST);
  if (ErrorCode != MMSYSERR_NOERROR)
    goto ErrorExit;

  while (TRUE)
  {
    memset (&ChunkLISTstrl, 0, sizeof (MMCKINFO));
    ChunkLISTstrl.ckid = listtypeSTREAMHEADER;
    ErrorCode = mmioDescend (MovieFile, &ChunkLISTstrl, &ChunkLISThdrl, MMIO_FINDLIST);
    if (ErrorCode != MMSYSERR_NOERROR)
      goto ErrorExit;

    memset (&Chunkstrh, 0, sizeof (MMCKINFO));
    Chunkstrh.ckid = ckidSTREAMHEADER;
    ErrorCode = mmioDescend (MovieFile, &Chunkstrh, &ChunkLISTstrl, MMIO_FINDCHUNK);
    if (ErrorCode != MMSYSERR_NOERROR)
      goto ErrorExit;

    if (Chunkstrh.cksize <=
    sizeof (StreamHeader) - sizeof (RECT) + sizeof (SMALL_RECT))
    {
      // Using a 16 bit integer rectangle, the standard.

      if (mmioRead (MovieFile, (char *) &StreamHeader,
      sizeof (StreamHeader) - sizeof (RECT)) <
      sizeof (StreamHeader) - sizeof (RECT))
      {
        ErrorCode = 2;
        goto ErrorExit;
      }

      if (mmioRead (MovieFile, (char *) &SmallRect, sizeof (SmallRect)) <
      sizeof (SmallRect))
      {
        ErrorCode = 3;
        goto ErrorExit;
      }
      StreamHeader.rcFrame.left = SmallRect.Left;
      StreamHeader.rcFrame.right = SmallRect.Right;
      StreamHeader.rcFrame.top = SmallRect.Top;
      StreamHeader.rcFrame.bottom = SmallRect.Bottom;
    }
    else // Assume it is a 32 bit integer rectangle used by buggy code.
    {
      if (mmioRead (MovieFile, (char *) &StreamHeader, sizeof (StreamHeader)) <
      sizeof (StreamHeader))
      {
        ErrorCode = 4;
        goto ErrorExit;
      }
    }

    ErrorCode = mmioAscend (MovieFile, &Chunkstrh, 0);
    if (ErrorCode != MMSYSERR_NOERROR)
      goto ErrorExit;

    memset (&Chunkstrf, 0, sizeof (MMCKINFO));
    Chunkstrf.ckid = ckidSTREAMFORMAT;
    ErrorCode = mmioDescend (MovieFile, &Chunkstrf, &ChunkLISTstrl, MMIO_FINDCHUNK);
    if (ErrorCode != MMSYSERR_NOERROR)
      goto ErrorExit;

    AmountToRead = Chunkstrf.cksize;
    if (AmountToRead > sizeof (StreamFormat))
      AmountToRead = sizeof (StreamFormat);
    if (mmioRead (MovieFile, (char *) &StreamFormat, AmountToRead) < AmountToRead)
    {
      ErrorCode = 5;
      goto ErrorExit;
    }

    ErrorCode = mmioAscend (MovieFile, &Chunkstrf, 0);
    if (ErrorCode != MMSYSERR_NOERROR)
      goto ErrorExit;

    if (StreamHeader.fccType == streamtypeVIDEO)
    {
      // Found the video stream.  Get the true image size out
      // of the format chunk (a bitmap info header) if frame is wrong.

      if (StreamHeader.rcFrame.right - StreamHeader.rcFrame.left != StreamFormat.biWidth ||
      StreamHeader.rcFrame.bottom - StreamHeader.rcFrame.top != StreamFormat.biHeight)
      {
        StreamHeader.rcFrame.left = 0;
        StreamHeader.rcFrame.top = 0;
        StreamHeader.rcFrame.right = StreamFormat.biWidth;
        StreamHeader.rcFrame.bottom = StreamFormat.biHeight;
      }

      break; // Bust out of here.
    }

    // Didn't find it, advance to next stream list chunk.

    ErrorCode = mmioAscend (MovieFile, &ChunkLISTstrl, 0);
    if (ErrorCode != MMSYSERR_NOERROR)
      goto ErrorExit;
  }

  // If we got here, we found the video stream header chunk.

  if (FramesPerSecond != NULL)
    *FramesPerSecond = StreamHeader.dwRate /
    (float) StreamHeader.dwScale;

  if (NumberOfFrames != NULL)
    *NumberOfFrames = StreamHeader.dwLength;

  if (BoundingRectangle != NULL)
    *BoundingRectangle = StreamHeader.rcFrame;

  ReturnCode = TRUE;
#endif // CE_ARTLIB_EnableSystemBink


ErrorExit:
#if CE_ARTLIB_EnableSystemBink
  if (BinkFile != NULL)
    BinkClose (BinkFile);
#else
  if (MovieFile != NULL)
    mmioClose (MovieFile, 0);
#endif

  if (!ReturnCode)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_GetAVIFileCharacteristics: "
      "Unable to open video file named \"%s\", error code %d.\r\n",
      FileName, (int) ErrorCode);

    LE_ERROR_MsgBox (LE_ERROR_DebugMessageBuffer, "Video Warning");
  }

  return ReturnCode;
}



/*****************************************************************************
 * Creates a video/audio stream or both using either the AVIFile interface
 * to the compression libraries (not the best one, still 16 bit, but it works
 * fairly simply, but does not work under Windows 2000) or our own ICM driver
 * code (if CE_ARTLIB_EnableSystemVideo is turned on, does work under Win2000),
 * or using the Bink video compressor and player library.
 * This stream stuff is common for all render types that can do video.
 * Assumes the RuntimeInfoPntr->streamSpecificData.videoStream record has
 * already been allocated.  DesiredSizeRectPntr specifies the decompressed
 * size of the video, which is only used when drawing directly to the screen
 * (intermediate bitmap mode always uses the size of the video and relies on
 * the renderer to scale the resulting bitmap).  Returns TRUE if successful.
 */

static BOOL GlueOpenVideoStream (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
char *FileName, TYPE_RectPointer DesiredSizeRectPntr)
{
#if !USE_OLD_VIDEO_PLAYER
  int                               DesiredHeight;
  int                               DesiredWidth;
  LE_REND_RenderSlot                RenderSlot;
  LE_REND_RenderSlotSet             RenderSlotSet;
  LI_REND_AbstractRenderSlotPointer ScreenSlot;
  PDDSURFACE                        WorkingSurface;
  DDSURFDESC                        WorkingSurfaceDesc;
#else // Old video player.
  struct MyAudioFormatStruct {
    WAVEFORMATEX      waveFormat;
    BYTE              extraData [512];
  } MyAudioFormat;

  LONG                              BufferSize;
  AVIFILEINFO                       MyAVIFileInfo;
  WAVEFORMATEX                      OutputFormat;
#endif

  HRESULT                           ErrorCode = -1;
  BOOL                              ReturnCode = FALSE;
  INT32                             VideoDuration;
  LE_SEQNCR_RuntimeVideoInfoPointer VideoPntr;

  if (SequencePntr == NULL ||
  SequencePntr->streamSpecificData.videoStream == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("L_SEQNCR.c GlueOpenVideoStream: Bad arguments.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // Prepare our information record.

  VideoPntr = SequencePntr->streamSpecificData.videoStream;

#if !USE_OLD_VIDEO_PLAYER // If using new video player.

  if (VideoPntr->drawDirectlyToScreen)
  {
    // Find the render slot and thus the surface which the
    // video will be drawn on.  It isn't necessarily the
    // real screen, could be a texture surface.

    for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
    {
      if (LI_SEQNCR_RenderSlotArray [RenderSlot] == NULL)
        continue;

      RenderSlotSet = (1 << RenderSlot);
      if ((SequencePntr->renderSlotSet & RenderSlotSet) == 0)
        continue; // This sequence isn't in that slot.

      break;
    }

    if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
      goto ErrorExit; // No render slot for this video.

    ScreenSlot = LI_SEQNCR_RenderSlotArray [RenderSlot];
    WorkingSurface = ScreenSlot->GetWorkingSurface ();
    if (WorkingSurface == NULL)
      goto ErrorExit;

    // Get the surface description, and also lock it so that we
    // get the pitch too (which we use rather than the width).

    memset (&WorkingSurfaceDesc, 0, sizeof (WorkingSurfaceDesc));
    WorkingSurfaceDesc.dwSize = sizeof (WorkingSurfaceDesc);

    ErrorCode = WorkingSurface->GetSurfaceDesc (&WorkingSurfaceDesc);
    if (ErrorCode != DD_OK)
      goto ErrorExit;

    if ((WorkingSurfaceDesc.ddpfPixelFormat.dwFlags & DDPF_RGB ) == 0 ||
    WorkingSurfaceDesc.ddpfPixelFormat.dwRGBBitCount < 16)
      goto ErrorExit; // We don't support this pixel format, need true colour.

    memset (&VideoPntr->bitmapInfo, 0, sizeof (VideoPntr->bitmapInfo));
    VideoPntr->bitmapInfo.bV4Size = sizeof (VideoPntr->bitmapInfo);
    VideoPntr->bitmapInfo.bV4Planes = 1;
    VideoPntr->bitmapInfo.bV4Width = WorkingSurfaceDesc.lPitch /
      (WorkingSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8);
    VideoPntr->bitmapInfo.bV4Height = WorkingSurfaceDesc.dwHeight;
    VideoPntr->bitmapInfo.bV4BitCount =
      (WORD) WorkingSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
    if (VideoPntr->bitmapInfo.bV4BitCount == 24)
      VideoPntr->bitmapInfo.bV4V4Compression = BI_RGB;
    else
      VideoPntr->bitmapInfo.bV4V4Compression = BI_BITFIELDS;
    VideoPntr->bitmapInfo.bV4RedMask = WorkingSurfaceDesc.ddpfPixelFormat.dwRBitMask;
    VideoPntr->bitmapInfo.bV4GreenMask = WorkingSurfaceDesc.ddpfPixelFormat.dwGBitMask;
    VideoPntr->bitmapInfo.bV4BlueMask = WorkingSurfaceDesc.ddpfPixelFormat.dwBBitMask;

    #if CE_ARTLIB_EnableSystemBink
    VideoPntr->outputPitch = WorkingSurfaceDesc.lPitch;
    #endif

    DesiredWidth = DesiredSizeRectPntr->right - DesiredSizeRectPntr->left;
    DesiredHeight = DesiredSizeRectPntr->bottom - DesiredSizeRectPntr->top;
  }
  else // Drawing to an intermediate bitmap.  Make the intermediate bitmap.
  {
    // Later on we will use a GBM (generic bitmap, wrapper around a
    // DirectDraw surface) once it has enough functionality.  But for
    // now, just use a native object thingy, at screen depth.

    DesiredWidth = VideoPntr->videoRectangle.right - VideoPntr->videoRectangle.left;
    DesiredHeight = VideoPntr->videoRectangle.bottom - VideoPntr->videoRectangle.top;

    VideoPntr->intermediateBitmapID =
      LE_GRAFIX_ObjectCreate (
      (short) (DesiredWidth),
      (short) (DesiredHeight),
      LE_GRAFIX_ObjectNotTransparent);
    if (VideoPntr->intermediateBitmapID == LE_DATA_EmptyItem)
      goto ErrorExit;

    memset (&VideoPntr->bitmapInfo, 0, sizeof (VideoPntr->bitmapInfo));
    VideoPntr->bitmapInfo.bV4Size = sizeof (VideoPntr->bitmapInfo);
    VideoPntr->bitmapInfo.bV4Planes = 1;
    VideoPntr->bitmapInfo.bV4Width =
      LE_GRAFIX_AnyBitmapWidth (VideoPntr->intermediateBitmapID);
    VideoPntr->bitmapInfo.bV4Height =
      LE_GRAFIX_AnyBitmapHeight (VideoPntr->intermediateBitmapID);
    VideoPntr->bitmapInfo.bV4BitCount = LE_GRAFIX_ScreenBitsPerPixel;
    if (VideoPntr->bitmapInfo.bV4BitCount == 24)
      VideoPntr->bitmapInfo.bV4V4Compression = BI_RGB;
    else
      VideoPntr->bitmapInfo.bV4V4Compression = BI_BITFIELDS;
    VideoPntr->bitmapInfo.bV4RedMask = LE_BLT_dwRedMask;
    VideoPntr->bitmapInfo.bV4GreenMask = LE_BLT_dwGreenMask;
    VideoPntr->bitmapInfo.bV4BlueMask = LE_BLT_dwBlueMask;

    #if CE_ARTLIB_EnableSystemBink
    VideoPntr->outputPitch = ((VideoPntr->bitmapInfo.bV4Width *
      (VideoPntr->bitmapInfo.bV4BitCount / 8) + 3) & 0xFFFFFFFC);
    #endif
  }

  #if CE_ARTLIB_EnableSystemBink
    VideoPntr->outputDepthFlags = BinkDDSurfaceType (LE_GRAFIX_DDPrimarySurfaceN);
    if (VideoPntr->outputDepthFlags == -1)
    {
      ErrorCode = 123; // Unsupported screen depth.
      goto ErrorExit;
    }

    #if CE_ARTLIB_EnableSystemSound
    BinkSoundUseDirectSound (LI_SOUND_DirectSoundObject);
    #endif

    VideoPntr->videoHandle = BinkOpen (FileName,
      (VideoPntr->doubleAlternateLines) ?
      BINKSURFACEYINTERLACE /* BINKSURFACEYDOUBLE - doesn't work - yet? */ :
      0);

  #else // Our new video module.
    VideoPntr->videoHandle = LE_VIDEO_StartVideo (
      FileName,
      (BITMAPINFOHEADER *) &VideoPntr->bitmapInfo /* Output bitmap */,
      DesiredWidth /* Output width */,
      DesiredHeight /* Output height */,
      (SequencePntr->endingAction == LE_SEQNCR_EndingActionLoopToBeginning) /* Looping flag */,
      !VideoPntr->flipVertically,
      VideoPntr->enableVideo,
      VideoPntr->enableAudio,
      (SequencePntr->dimensionality == 3) /* 3D Sound */,
      FALSE /* Alternate scan line mode when doubled */,
      VideoPntr->doubleAlternateLines /* Double mode */);
  #endif

  if (VideoPntr->videoHandle == NULL)
    goto ErrorExit;

#else // Old video player.

  // The old video player doesn't support direct drawing since the
  // AVIStreamGetFrame Windows function has its own internal bitmap.

  // Open the file.

  ErrorCode = AVIFileOpen (&VideoPntr->fileHandle, FileName,
    OF_SHARE_DENY_WRITE, NULL);
  if (ErrorCode == -2147221008)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c GlueOpenVideoStream: "
      "Unable to open AVI file named \"%s\", error code %d.  This usually means that "
      "you don't have the video decompressor needed, see the multimedia control "
      "panel to add one (maybe you are missing Indeo 5.0?).\r\n",
      FileName, (int) ErrorCode);
    MessageBox (NULL, LE_ERROR_DebugMessageBuffer,
      LE_MAIN_ApplicationNameString, MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);
  }
  if (ErrorCode != 0) goto ErrorExit;

  // Get the details about the file.  Things like number of streams,
  // suggested buffer sizes, etc.

  ErrorCode = AVIFileInfo (VideoPntr->fileHandle, &MyAVIFileInfo,
    sizeof (MyAVIFileInfo));
  if (ErrorCode != 0) goto ErrorExit;

  // Try to find a video stream in the file.  May be none if
  // this is a pure audio file.

  if (VideoPntr->enableVideo)
  {
    ErrorCode = AVIFileGetStream (VideoPntr->fileHandle,
      &VideoPntr->videoStream, streamtypeVIDEO,
      0 /* Zeroeth video stream in the file */);
  }
  else // Pretend there is no video.
    ErrorCode = 1;

  if (ErrorCode == 0) // If there is video data here.
  {
    // Get video stream info.

    ErrorCode = AVIStreamInfo (VideoPntr->videoStream,
      &VideoPntr->videoStreamInfo, sizeof (VideoPntr->videoStreamInfo));
    if (ErrorCode != 0) goto ErrorExit;

    VideoPntr->videoFramesPerSecond =
      (float) VideoPntr->videoStreamInfo.dwRate /
      (float) VideoPntr->videoStreamInfo.dwScale;

    VideoPntr->numberOfVideoFrames =
      AVIStreamLength (VideoPntr->videoStream);

    VideoPntr->currentVideoFrameNumber =
      VideoPntr->videoStreamInfo.dwStart /
      VideoPntr->videoStreamInfo.dwScale;

    VideoPntr->videoRectangle =
      VideoPntr->videoStreamInfo.rcFrame;
  }
  else
    VideoPntr->enableVideo = FALSE;

  // Try to find an audio stream in the file.  May be none for silent films.
  // Also reject audio that we can't handle (but keep the video).

  if (VideoPntr->enableAudio)
  {
    ErrorCode = AVIFileGetStream (VideoPntr->fileHandle,
      &VideoPntr->audioStream, streamtypeAUDIO,
      0 /* Zeroeth audio stream in the file */);
  }
  else // Pretend there is no audio.
    ErrorCode = 1;

  if (ErrorCode == 0) // If there is audio data here.
  {
    // Get audio stream info.

    ErrorCode = AVIStreamInfo (VideoPntr->audioStream,
      &VideoPntr->audioStreamInfo, sizeof (VideoPntr->audioStreamInfo));
    if (ErrorCode != 0) goto ErrorExit;

    VideoPntr->numberOfAudioFrames =
      AVIStreamLength (VideoPntr->audioStream);

    VideoPntr->currentAudioFrameNumber =
      VideoPntr->audioStreamInfo.dwStart /
      VideoPntr->audioStreamInfo.dwScale;

    // Find out the stream audio (WAVE) format.

    BufferSize = sizeof (MyAudioFormat);
    memset (&MyAudioFormat, 0, sizeof (MyAudioFormat));
    ErrorCode = AVIStreamReadFormat (VideoPntr->audioStream,
      0 /* Position in stream */, &MyAudioFormat, &BufferSize);
    if (ErrorCode != 0) goto ErrorExit;

    // Create a sound buffer.  First make the output format -
    // essentially a decompressed version of the input format.

    OutputFormat = MyAudioFormat.waveFormat;
    OutputFormat.wFormatTag = WAVE_FORMAT_PCM;
    if (OutputFormat.nSamplesPerSec == 0)
      OutputFormat.nSamplesPerSec = LE_SOUND_CurrentHardwareHz;
    if (OutputFormat.wBitsPerSample != 8 && OutputFormat.wBitsPerSample != 16)
      OutputFormat.wBitsPerSample = LE_SOUND_CurrentHardwareBits;
    OutputFormat.nBlockAlign = (WORD)
      (OutputFormat.nChannels * (OutputFormat.wBitsPerSample / 8));
    OutputFormat.nAvgBytesPerSec =
      OutputFormat.nSamplesPerSec * OutputFormat.nBlockAlign;
    OutputFormat.cbSize = 0;

    VideoPntr->soundBuffer = LE_SOUND_StartBufSnd (&MyAudioFormat.waveFormat,
      &OutputFormat, NULL /* No dump file */, FALSE /* Not 3D */,
      LE_DATA_EmptyItem /* Don't cache it */,
      FALSE /* Don't permanently cache it */,
      NULL /* Don't care about cache status */,
      FALSE /* Doesn't fit in buffer */,
      VIDEO_FEED_DURATION + 1, VIDEO_READ_AHEAD_DURATION);

    if (VideoPntr->soundBuffer == NULL)
      goto ErrorExit;
  }
  else
    VideoPntr->enableAudio = FALSE;

  // Fake the video frame count and rate so that the audio works correctly
  // even if there is no video (the animation ends when the last frame is
  // shown).

  if (VideoPntr->audioStream != NULL && VideoPntr->videoStream == NULL)
  {
    VideoPntr->videoFramesPerSecond =
      (float) VideoPntr->audioStreamInfo.dwRate /
      (float) VideoPntr->audioStreamInfo.dwScale;
    VideoPntr->numberOfVideoFrames = VideoPntr->numberOfAudioFrames;
  }

  VideoPntr->alternativeDecisionFrame = -1; // No active alternative path.
#endif // Old player or not.

  // Common code for old and new players - update the sequence
  // timing specifics to match the video.

  if (VideoPntr->videoFramesPerSecond > 0.0F)
  {
    // Fix the sequence ending time to match the end of the video, if it is
    // longer than the video.

    VideoDuration = (LONG) (CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ *
      VideoPntr->numberOfVideoFrames / VideoPntr->videoFramesPerSecond);
    if (SequencePntr->endTime > VideoDuration)
      SequencePntr->endTime = VideoDuration;

    // Need to update the sequence every frame, but do it more often since our
    // clock runs slightly slower than the audio clock and we have code which
    // avoids redraws when the same frame is still showing.

    #if CE_ARTLIB_EnableSystemBink // Bink also needs more updates.
    SequencePntr->timeMultiple = 1;
    #else // Ordinary video players.
    SequencePntr->timeMultiple = (UNS8) (CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ /
      VideoPntr->videoFramesPerSecond / 2);
    #endif
  }

  if (SequencePntr->timeMultiple <= 0)
    SequencePntr->timeMultiple = 1;

  if (VideoPntr->enableVideo)
    SequencePntr->redrawAtEveryUpdate = TRUE;

#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c GlueOpenVideoStream: "
    "Opened video file \"%s\".\r\n", FileName);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

#if USE_OLD_VIDEO_PLAYER // Old video player needs preloading.

  // Try to start the video/audio playing.

  if (!StartVideoStreaming (VideoPntr))
    goto ErrorExit;

  // Start the sound playing and load the current image, while the file
  // is hot in the cache.

  if (!LI_SEQNCR_GlueFeedAndDrawVideo (SequencePntr, NULL, NULL, 0, 0, NULL))
    goto ErrorExit;

  if (VideoPntr->videoGetFrameFunction != NULL)
  {
    if (VideoPntr->currentVideoFrameNumber >= VideoPntr->numberOfVideoFrames)
      AVIStreamGetFrame (VideoPntr->videoGetFrameFunction,
      VideoPntr->numberOfVideoFrames - 1);
    else // Have a valid frame number.
      AVIStreamGetFrame (VideoPntr->videoGetFrameFunction,
      VideoPntr->currentVideoFrameNumber);
  }

  // Keep the sound going, in case loading the current frame took a long time.

  if (!LI_SEQNCR_GlueFeedAndDrawVideo (SequencePntr, NULL, NULL, 0, 0, NULL))
    goto ErrorExit;
#endif // Old player or not

#if CE_ARTLIB_EnableSystemBink // Bink also needs preloading.
  {
    int i;
    for (i = 0; i < 100; i++)
    {
      if (!BinkWait (VideoPntr->videoHandle))
        break; // It's ready, stop waiting.
      LE_TIMER_Delay (1);
    }
  }
#endif // Bink

  // Success!

  ReturnCode = TRUE;
  goto NormalExit;


ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c GlueOpenVideoStream: "
    "Unable to open AVI file named \"%s\", error code %d.\r\n",
    FileName, (int) ErrorCode);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

  GlueCloseVideoStream (SequencePntr);

NormalExit:
  return ReturnCode;
}



/*****************************************************************************
 * Allocates a record from ChainedItemsArray and copies the given data into
 * it.  Returns its pointer, NULL if it fails.
 */

static ChainedItemPointer AddCommandToChainQueue (LE_QUEUE_MessagePointer UserCommandPntr)
{
  ChainedItemPointer NewCommandPntr;

  if (FreeChainedCommands == NULL || UserCommandPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("L_SEQNCR.c AddCommandToChainQueue: Out of memory or bad args.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Out of memory or bad arguments.
  }

  // Remove a record from the free list.

  NewCommandPntr = FreeChainedCommands;
  FreeChainedCommands = FreeChainedCommands->next;

  // Copy the provided information.  We assume responsibility for
  // deallocating the attached dymamically allocated data.

  NewCommandPntr->message = *UserCommandPntr;
  UserCommandPntr->stringA = NULL;

  // Add the record to the head of the pending chained commands queue.

  if (WaitingChainedCommandsHead == NULL)
  {
    // Empty queue, this will be the only item in it.
    NewCommandPntr->next = NULL;
    WaitingChainedCommandsTail = NewCommandPntr;
  }
  else
  {
    NewCommandPntr->next = WaitingChainedCommandsHead;
    WaitingChainedCommandsHead->previous = NewCommandPntr;
  }
  NewCommandPntr->previous = NULL;
  WaitingChainedCommandsHead = NewCommandPntr;

  return NewCommandPntr;
}



/*****************************************************************************
 * Go through the list of pending chained commands and find ones related
 * to the given sequence and mark them for deletion.  If a command was found
 * with the current sequence as the thing it is chained to then it is a
 * descendant and will also be marked if MarkDescendants is on.  If the
 * MarkAncestors flag is on then SEQCMD_Start commands will be examined
 * and marked if they are starting this sequence, and their chainees will
 * be added recursively (depth first search for those of you who know
 * graph theory).
 */

static void MarkChainedRelatives (LE_DATA_DataId DataID, UNS16 Priority,
BOOL MarkAncestors, BOOL MarkDescendants)
{
  SeqCmdFlagsAsLong       CommandFlags;
  LE_QUEUE_MessagePointer CommandPntr;
  ChainedItemPointer      CurrentPntr;

  CurrentPntr = WaitingChainedCommandsTail;
  while (CurrentPntr != NULL)
  {
    CommandPntr = &CurrentPntr->message;
    CommandFlags.asDWORD = CommandPntr->numberE;

    if (!CommandFlags.asFlags.SEQFLAG_MarkedForDeletion)
    {
      if (MarkDescendants &&
      (LE_DATA_DataId) CommandPntr->numberC == DataID && CommandPntr->numberD == Priority)
      {
        // Found a descendant (any kind of command that is
        // chained to the given sequence).  Mark it first.

        CommandFlags.asFlags.SEQFLAG_MarkedForDeletion = TRUE;
        CommandPntr->numberE = CommandFlags.asDWORD;

        // Now recursively process its relatives (must be done after marking),
        // for start sequence commands only.

        if (CommandPntr->messageCode == SEQCMD_Start)
          MarkChainedRelatives (CommandPntr->numberA,
          (UNS16) CommandPntr->numberB, MarkAncestors, MarkDescendants);
      }

      if (MarkAncestors && CommandPntr->messageCode == SEQCMD_Start &&
      (LE_DATA_DataId) CommandPntr->numberA == DataID && CommandPntr->numberB == Priority)
      {
        // We have an ancestor - a pending command that starts our sequence.
        // Mark it and process its relatives.

        CommandFlags.asFlags.SEQFLAG_MarkedForDeletion = TRUE;
        CommandPntr->numberE = CommandFlags.asDWORD;

        // Now recursively process its ancestors and descendents.

        MarkChainedRelatives (CommandPntr->numberC,
          (UNS16) CommandPntr->numberD, MarkAncestors, MarkDescendants);
      }
    }

    // Advance to the next pending command.

    CurrentPntr = CurrentPntr->previous;
  }
}



/*****************************************************************************
 * Just mark all start sequence commands that start the given
 * DataID/Priority.
 */

static void MarkSelf (LE_DATA_DataId DataID, UNS16 Priority)
{
  SeqCmdFlagsAsLong       CommandFlags;
  LE_QUEUE_MessagePointer CommandPntr;
  ChainedItemPointer      CurrentPntr;

  CurrentPntr = WaitingChainedCommandsTail;
  while (CurrentPntr != NULL)
  {
    CommandPntr = &CurrentPntr->message;
    CommandFlags.asDWORD = CommandPntr->numberE;

    if (!CommandFlags.asFlags.SEQFLAG_MarkedForDeletion)
    {
      if (CommandPntr->messageCode == SEQCMD_Start &&
      (LE_DATA_DataId) CommandPntr->numberA == DataID && CommandPntr->numberB == Priority)
      {
        CommandFlags.asFlags.SEQFLAG_MarkedForDeletion = TRUE;
        CommandPntr->numberE = CommandFlags.asDWORD;
      }
    }

    // Advance to the next pending command.

    CurrentPntr = CurrentPntr->previous;
  }
}



/****************************************************************************
 * Allocate a LE_SEQNCR_RuntimeInfoRecord, returns NULL when out of memory.
 * The returned record is initialised to zeroes.
 */

static LE_SEQNCR_RuntimeInfoPointer LI_SEQNCR_AllocRuntimeInfo (void)
{
  LE_SEQNCR_RuntimeInfoPointer ReturnValue;

  if ((ReturnValue = NextFreeCommonRuntimeElement) == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_SEQNCR_AllocRuntimeInfo: Out of memory.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Out of memory.
  }

  // Sanity check to see if it is really free.

#if CE_ARTLIB_EnableDebugMode
  if (ReturnValue->parent != NULL)
    LE_ERROR_ErrorMsg ("LI_SEQNCR_AllocRuntimeInfo: Corrupted free nodes list.\r\n");
#endif

  // Remove the allocated record from the free list.

  NextFreeCommonRuntimeElement = ReturnValue->child;
#if CE_ARTLIB_EnableDebugMode
  NumberOfFreeCommonRuntimeElements--;
#endif
  ReturnValue->child = NULL;  // Whole record should be zero now.
  return ReturnValue;
}



/*****************************************************************************
 * Deallocate a LE_SEQNCR_RuntimeInfoRecord, returns it to free list.
 */

static void LI_SEQNCR_FreeRuntimeInfo (LE_SEQNCR_RuntimeInfoPointer FreePntr)
{
  if (FreePntr != NULL)
  {
    // Add the record to the free list.  Also make the parent pointer NULL
    // so that we can tell that it is no longer part of the tree.  While
    // we're at it, make the whole thing zero to catch other code trying
    // to use a deleted record.  Hopefully it will stay zeroed so that the
    // allocation routine doesn't have to re-zero it.

    memset (FreePntr, 0, sizeof (LE_SEQNCR_RuntimeInfoRecord));
    FreePntr->child = NextFreeCommonRuntimeElement;
    NextFreeCommonRuntimeElement = FreePntr;
#if CE_ARTLIB_EnableDebugMode
    NumberOfFreeCommonRuntimeElements++;
#endif
  }
}



/*****************************************************************************
 * Allocate a LE_SEQNCR_Runtime2DInfoRecord, returns NULL when out of memory.
 */

static LE_SEQNCR_Runtime2DInfoPointer LI_SEQNCR_AllocRuntime2DInfo (void)
{
  LE_SEQNCR_Runtime2DInfoPointer ReturnValue;

  if ((ReturnValue = NextFree2DRuntimeElement) == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_SEQNCR_AllocRuntime2DInfo: Out of memory.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Out of memory.
  }

  // Remove the allocated record from the free list.

  NextFree2DRuntimeElement = *((LE_SEQNCR_Runtime2DInfoPointer *) ReturnValue);
#if CE_ARTLIB_EnableDebugMode
  NumberOfFree2DRuntimeElements--;
#endif
  memset (ReturnValue, 0, sizeof (LE_SEQNCR_Runtime2DInfoRecord));
  return ReturnValue;
}



/*****************************************************************************
 * Deallocate a LE_SEQNCR_Runtime2DInfoRecord, returns it to free list.
 */

static void LI_SEQNCR_FreeRuntime2DInfo (LE_SEQNCR_Runtime2DInfoPointer FreePntr)
{
  if (FreePntr != NULL)
  {
    // Add the record to the free list.
    *((void **) FreePntr) = NextFree2DRuntimeElement;
    NextFree2DRuntimeElement = FreePntr;
#if CE_ARTLIB_EnableDebugMode
    NumberOfFree2DRuntimeElements++;
#endif
  }
}



/*****************************************************************************
 * Allocate a LE_SEQNCR_Runtime3DInfoRecord, returns NULL when out of memory.
 */

static LE_SEQNCR_Runtime3DInfoPointer LI_SEQNCR_AllocRuntime3DInfo (void)
{
  LE_SEQNCR_Runtime3DInfoPointer ReturnValue;

  if ((ReturnValue = NextFree3DRuntimeElement) == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_SEQNCR_AllocRuntime3DInfo: Out of memory.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return NULL; // Out of memory.
  }

  // Remove the allocated record from the free list.

  NextFree3DRuntimeElement = *((LE_SEQNCR_Runtime3DInfoPointer *) ReturnValue);
#if CE_ARTLIB_EnableDebugMode
  NumberOfFree3DRuntimeElements--;
#endif
  memset (ReturnValue, 0, sizeof (LE_SEQNCR_Runtime3DInfoRecord));
  return ReturnValue;
}



/*****************************************************************************
 * Deallocate a LE_SEQNCR_Runtime3DInfoRecord, returns it to free list.
 */

static void LI_SEQNCR_FreeRuntime3DInfo (LE_SEQNCR_Runtime3DInfoPointer FreePntr)
{
  if (FreePntr != NULL)
  {
    // Add the record to the free list.
    *((void **) FreePntr) = NextFree3DRuntimeElement;
    NextFree3DRuntimeElement = FreePntr;
#if CE_ARTLIB_EnableDebugMode
    NumberOfFree3DRuntimeElements++;
#endif
  }
}



/*****************************************************************************
 * Inserts the given runtime info record into the tree as a child of the
 * given parent node.  The priority of the new node will be used to place
 * it in the right spot in the children list of the parent.  Doesn't do
 * any allocations, just updates links.  Assumes the child isn't already
 * in the tree.
 */

static void LI_SEQNCR_InsertRuntimeChild (
LE_SEQNCR_RuntimeInfoPointer ChildPntr,
LE_SEQNCR_RuntimeInfoPointer ParentPntr)
{
  UNS16                         ChildPriority;
  LE_SEQNCR_RuntimeInfoPointer  NextSibling;
  LE_SEQNCR_RuntimeInfoPointer  PreviousSibling;

#if CE_ARTLIB_EnableDebugMode
  if (ChildPntr == NULL || ParentPntr == NULL)
    LE_ERROR_ErrorMsg ("LI_SEQNCR_InsertRuntimeChild: Called with NULL pointer.\r\n");
#endif

  // The parent link is easy to do.

  ChildPntr->parent = ParentPntr;

  // The siblings are in increasing priority order.
  // Find the spot where the new child should be inserted.

  ChildPriority = ChildPntr->priority;
  PreviousSibling = NULL;
  NextSibling = ParentPntr->child;
  while (NextSibling != NULL)
  {
    if (NextSibling->priority >= ChildPriority)
      break; // Found the spot where we should add the new guy.

    PreviousSibling = NextSibling;
    NextSibling = NextSibling->sibling;
  }

  // Link in the child.

  ChildPntr->sibling = NextSibling;
  if (PreviousSibling != NULL)
    PreviousSibling->sibling = ChildPntr;
  else // This is the first child of the parent.
    ParentPntr->child = ChildPntr;
}



/*****************************************************************************
 * Removes the given runtime info record from the tree.  Doesn't do
 * any deallocations, just updates links.  Assumes the record is
 * already in the tree and isn't the root node.
 */

static void LI_SEQNCR_RemoveRuntimeNodeFromTree (LE_SEQNCR_RuntimeInfoPointer NodePntr)
{
  LE_SEQNCR_RuntimeInfoPointer  ParentPntr;
  LE_SEQNCR_RuntimeInfoPointer  PreviousSibling;
  LE_SEQNCR_RuntimeInfoPointer  NextSibling;

#if CE_ARTLIB_EnableDebugMode
  if (NodePntr == NULL || NodePntr->parent == NULL)
    LE_ERROR_ErrorMsg ("LI_SEQNCR_RemoveRuntimeNodeFromTree: Called with NULL pointer or node not in tree.\r\n");
#endif

  ParentPntr = NodePntr->parent;
  PreviousSibling = NULL;
  NextSibling = ParentPntr->child;
  while (NextSibling != NULL)
  {
    if (NextSibling == NodePntr)
      break;  // Found the node we were looking for.

    PreviousSibling = NextSibling;
    NextSibling = NextSibling->sibling;
  }

  if (NextSibling == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_ErrorMsg ("LI_SEQNCR_RemoveRuntimeNodeFromTree: Can't find node in parent's child list.\r\n");
#endif
    return; // Didn't find the node we were looking for.
  }

  // Found the node to be deleted.  Advance the next pointer
  // to the node after the one to be deleted.  Now have
  // previous being before the deleted node and
  // next being after the deleted node.

  NextSibling = NextSibling->sibling;

  // Remove the node from the list of children that it is in.

  if (PreviousSibling != NULL)
    PreviousSibling->sibling = NextSibling;
  else // This is the first child of the parent.
    ParentPntr->child = NextSibling;
}



/*****************************************************************************
 * Look for a runtime node in the tree (starting at the given node) which has
 * a matching DataId and priority.  If SearchWholeTree is FALSE then it only
 * searches the top level nodes (the children of the root node).  Returns
 * NULL if it can't find anything.  Ignores nodes that are nested inside
 * others (since they would confusingly have the same DataID as their parent).
 */

static LE_SEQNCR_RuntimeInfoPointer FindNextSequence (
LE_DATA_DataId DataID, UNS16 Priority, BOOL SearchWholeTree,
LE_SEQNCR_RuntimeInfoPointer StartSearchHerePntr)
{
  LE_SEQNCR_RuntimeInfoPointer SequencePntr;

  SequencePntr = LI_SEQNCR_CommonRuntimeTreeRoot;
  if (SequencePntr == NULL)
    return NULL; // System isn't open.

  // Always start the search with the children of the root, you can't find
  // the root node since it is special and shouldn't be tampered with.

  if (StartSearchHerePntr != NULL)
    SequencePntr = StartSearchHerePntr;
  else
    SequencePntr = SequencePntr->child;

  if (SequencePntr == NULL)
    return NULL; // No other nodes in the tree.

  if (SearchWholeTree)
  {
    if (StartSearchHerePntr != NULL)
    {
      // Have to skip past the place where the search stopped last.

      if (SequencePntr->child != NULL)
        SequencePntr = SequencePntr->child;
      else
      {
        while (SequencePntr != NULL && SequencePntr->sibling == NULL)
          SequencePntr = SequencePntr->parent;
        if (SequencePntr == NULL)
          return NULL; // All done.
        SequencePntr = SequencePntr->sibling;
      }
    }

    while (TRUE)
    {
      // Do the current node.

      if (SequencePntr->dataID == DataID &&
      SequencePntr->priority == Priority &&
      SequencePntr->dataOffset == 0)
        return SequencePntr; // Found it.

      // Traverse down the tree, then do siblings, then back up to the
      // next parent's sibling.

      if (SequencePntr->child != NULL)
        SequencePntr = SequencePntr->child;
      else
      {
        while (SequencePntr != NULL && SequencePntr->sibling == NULL)
          SequencePntr = SequencePntr->parent;
        if (SequencePntr == NULL)
          return NULL; // All done.
        SequencePntr = SequencePntr->sibling;
      }
    }
  }
  else // Just doing children of root node.
  {
    if (StartSearchHerePntr != NULL)
    {
      // Have to skip past the place where the search stopped last.

      SequencePntr = SequencePntr->sibling;
      if (SequencePntr == NULL)
        return NULL; // All done.
    }

    while (TRUE)
    {
      // Do the current node.

      if (SequencePntr->dataID == DataID &&
      SequencePntr->priority == Priority &&
      SequencePntr->dataOffset == 0)
        return SequencePntr; // Found it.

      SequencePntr = SequencePntr->sibling;
      if (SequencePntr == NULL)
        return NULL; // All done.
    }
  }
}



/*****************************************************************************
 * Mark the children of the given sequence as needing re-evaluation.  Saves
 * time by not marking children that are already marked.  Does not mark the
 * given node.  Probably not needed since the updater automatically pushes
 * down position recalculations to all children without using the
 * needsReEvaluation flag.
 */

static void MarkDescendantsAsNeedingReEvaluation (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  LE_SEQNCR_RuntimeInfoPointer  CurrentPntr;
  BOOL                          CurrentNeededReEval;

  if (SequencePntr == NULL ||
  (CurrentPntr = SequencePntr->child) == NULL)
    return; // No sequence or no children to do.

  while (TRUE)
  {
    // Do the current node.

    CurrentNeededReEval = CurrentPntr->needsReEvaluation;
    CurrentPntr->needsReEvaluation = TRUE;

    // Traverse down the tree, then do siblings, then back up to the
    // next parent's sibling.  Stop when we get back to the original node.
    // Also skip children if this node has already been marked.

    if (CurrentPntr->child != NULL && !CurrentNeededReEval)
      CurrentPntr = CurrentPntr->child;
    else
    {
      while (CurrentPntr != SequencePntr && CurrentPntr->sibling == NULL)
        CurrentPntr = CurrentPntr->parent;
      if (CurrentPntr == SequencePntr)
        break; // All done.
      CurrentPntr = CurrentPntr->sibling;
    }
  }
}



/*****************************************************************************
 * Mark the parents of the given sequence as needing re-evaluation.  Saves
 * time by not marking ancestors that are already marked.  Does not mark the
 * given node.
 */

static void MarkAncestorsAsNeedingReEvaluation (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  LE_SEQNCR_RuntimeInfoPointer  CurrentPntr;

  if (SequencePntr == NULL)
    return; // No sequence to do.

  CurrentPntr = SequencePntr->parent;
  while (CurrentPntr != NULL && !CurrentPntr->needsReEvaluation)
  {
    CurrentPntr->needsReEvaluation = TRUE;
    CurrentPntr = CurrentPntr->parent;
  }
}



/*****************************************************************************
 * Mark this sequence as needing a position recalculaton.  Mark it and all
 * ancestors as needing a re-evaluation (descendants automatically get
 * re-evaluated in the recursive update).
 */

static void MarkAsNeedingPositionRecalc (LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  if (SequencePntr == NULL)
    return; // Just in case...

  // Setting the needsPositionRecalc flag makes the sequence (and parents and
  // children) automatically redraw.  It also forces the updater to push down
  // a position calculation to all children while it is updating (don't need
  // to explicitly set needsPositionRecalc or needsReEvaluation in children).

  SequencePntr->needsPositionRecalc = TRUE;
  SequencePntr->needsReEvaluation = TRUE;

  // Mark all parents as just needing a re-evaluation, so that this moved
  // sequence gets an update without having to wait for its clock to expire.

  MarkAncestorsAsNeedingReEvaluation (SequencePntr);
}



/*****************************************************************************
 * Creates a new sequence from the given DataId/DataOffset (assumed to
 * point to the header of a sequence chunk of some kind or be the DataID
 * of a simple data item (bitmap, wave data, etc)).  The optional
 * StartSequenceCommandPntr points to the message record for a Start Sequence
 * command and is used to modify the sequence attributes (moving it to a
 * different position, or using a different priority, and similar things).
 * All things needed are allocated here and the new sequence is added as a
 * child of the given parent.  Also updates renderers that need to know about
 * the sequence starting.  Returns NULL if it fails, else a pointer to the
 * newly started sequence runtime info.
 */

static LE_SEQNCR_RuntimeInfoPointer LI_SEQNCR_StartUpSequence (
LE_DATA_DataId DataID,
UNS32 DataOffset,
LE_QUEUE_MessagePointer StartSequenceCommandPntr,
LE_SEQNCR_RuntimeInfoPointer ParentPntr)
{
  LE_REND_RenderSlotSet                 AllowedRenderSlots;
  UNS32                                 AmountRead;
  UNS16                                 BitmapHeight;
  LPNEWBITMAPHEADER                     BitmapUCPHeaderPntr;
  UNS16                                 BitmapOffsetX;
  UNS16                                 BitmapOffsetY;
  UNS16                                 BitmapWidth;
  LE_SEQNCR_2DBoundingBoxChunkPointer   BoundingBox2DPntr;
  LE_SEQNCR_3DBoundingBoxChunkPointer   BoundingBox3DPntr;
  LE_SEQNCR_3DBoundingSphereChunkPointer BoundingSphere3DPntr;
  BOOL                                  ChildSequencesFound;
  LE_CHUNK_ID                           ChunkType;
  SeqCmdStart2DAttachmentPointer        Command2DDataPntr;
  SeqCmdStart3DAttachmentPointer        Command3DDataPntr;
  SeqCmdStart0DAttachmentPointer        CommandCommonDataPntr = NULL;
  SeqCmdFlagsAsLong                     CommandFlags;
  UNS8                                  Dimensionality;
  LE_SEQNCR_DimensionalityChunkPointer  DimensionalityChunkPntr;
  LE_CHUNK_DopeDataPointer              DopeDataPntr;
  LE_SEQNCR_DropFrames                  DropFramesOption;
  LE_SEQNCR_Sequence2DBitmapChunkRecord Fake2DBitmapSequenceData;
  LE_SEQNCR_SequenceSoundChunkRecord    FakeSoundSequenceData;
  LE_SEQNCR_Sequence3DMeshChunkRecord   Fake3DSequenceData;
  LE_SEQNCR_FieldOfViewChunkPointer     FieldOfViewChunkPntr;
  char                                  FileName1 [MAX_PATH];
  BOOL                                  FoundBoundingBoxData = FALSE;
  BOOL                                  FoundTransformationData = FALSE;
  LE_GRAFIX_GenericBitmapPointer        GBMPntr;
  int                                   i;
  LE_SEQNCR_LabelChunkPointer           LabelChunkPntr;
  LE_SEQNCR_3DMeshChoiceChunkPointer    MeshChunkPntr;
  LE_SEQNCR_2DOffsetChunkPointer        Offset2DChunkPntr;
  LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer OSRT2DChunkPntr;
  LE_SEQNCR_3DOffsetChunkPointer        Offset3DChunkPntr;
  LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer OSRT3DChunkPntr;
  LE_DATA_DataId                        OriginalUnconvertedDataID;
  TYPE_Coord3D                          RadiusNegative;
  TYPE_Coord3D                          RadiusPositive;
  LE_REND_RenderSlot                    RenderSlot;
  LI_REND_AbstractRenderSlotPointer     RenderSlotPntr;
  LE_SEQNCR_RuntimeInfoPointer          RuntimeInfoPntr = NULL;
  LE_SEQNCR_Runtime2DInfoPointer        Runtime2DInfoPntr = NULL;
  LE_SEQNCR_Runtime3DInfoPointer        Runtime3DInfoPntr = NULL;
  LE_SEQNCR_SequenceChunkHeaderRecord   SequenceHeader;
  LE_SEQNCR_SequenceType                SequenceType;
  INT32                                 SoundDuration;
  LE_SEQNCR_SoundPanningChunkPointer    SoundPanningChunkPntr;
  LE_SEQNCR_SoundPitchChunkPointer      SoundPitchChunkPntr;
  LE_SEQNCR_SoundVolumeChunkPointer     SoundVolumeChunkPntr;
  UNS32                                 SubChunksStartOffset = 0;
  LE_CHUNK_StreamID                     StreamID = -1;
  TYPE_Point3D                          Temp3DPoint;
  char                                  TempString [MAX_PATH];
  LE_DATA_DataType                      TypeOfBitmapDataID;
  LE_DATA_DataType                      TypeOfDataID;
  LE_SEQNCR_2DXformChunkPointer         XForm2DChunkPntr;
  LE_SEQNCR_3DXformChunkPointer         XForm3DChunkPntr;
  void                                 *VoidPntr;

  FileName1 [0] = 0; // Start with an empty string.

  if (DataID == LE_DATA_EmptyItem || ParentPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_SEQNCR_StartUpSequence: Called with bad arguments.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;  // Bad arguments.
  }

  if (StartSequenceCommandPntr != NULL) // Optional extra command arguments.
  {
    CommandCommonDataPntr = (SeqCmdStart0DAttachmentPointer) StartSequenceCommandPntr->stringA;
    CommandFlags.asDWORD = StartSequenceCommandPntr->numberE;
  }
  else // Use sensible defaults.
    CommandFlags.asDWORD = 0;

  Dimensionality = ParentPntr->dimensionality;

  // What were we given?  If it is a chunky file then we can look for all
  // sorts of stuff, if it is just a bitmap or 3D model we handle it more
  // simply.  Initially just set up things and find the dimensionality.

  TypeOfDataID = LE_DATA_GetLoadedDataType (DataID);

  if (TypeOfDataID == LE_DATA_DataChunky)
  {
    StreamID = LE_CHUNK_ReadFromDataID (DataID);
    if (StreamID < 0)
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
        "Unable to open stream on data ID $%08X.\r\n", (int) DataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      goto ErrorExit;  // Unable to open stream, bad DataID?
    }

    // Open the sequence header chunk at the given position in the DataID.

    if (!LE_CHUNK_SetCurrentFilePosition (StreamID, DataOffset))
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
        "Unable to seek to offset %d in stream using data ID $%08X.\r\n",
        (int) DataOffset, (int) DataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      goto ErrorExit;  // Our data offset is bad.
    }

    SequenceType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, NULL);

    if (SequenceType < LE_CHUNK_ID_SEQ_GROUPING ||
    SequenceType >= LE_CHUNK_ID_SEQ_MAX)
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
        "Got unknown chunk type %d when looking for sequence in data ID $%08X at offset %d.\r\n",
        (int) SequenceType, (int) DataID, (int) DataOffset);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      goto ErrorExit;  // Unknown sequence type or a chunk error.
    }

    // Save the position after the header, where all the nested subchunks
    // are, so that we can rewind and re-examine them.  The size of the
    // fixed size portion varies depending on chunk type.

    SubChunksStartOffset = DataOffset + sizeof (LE_CHUNK_HeaderRecord) +
      LE_CHUNK_ChunkFixedDataSizes [SequenceType];

    // Read the common sequence header, this is part of the fixed
    // size portion of all the sequence chunks.  Of course, some
    // chunk types have a bigger fixed size portion than others,
    // but worry about the extra data later.

    if (LE_CHUNK_ReadChunkData (StreamID, &SequenceHeader,
    sizeof (SequenceHeader)) != sizeof (SequenceHeader))
      goto BadChunkExit;  // Unable to read the sequence header record.

    // Try and figure out the dimensionality of this sequence.  If it is a
    // grouping sequence, and there isn't anything else to say what it is,
    // just use the dimensionality of the parent.

    switch (SequenceType)
    {
    case LE_CHUNK_ID_SEQ_VIDEO:
    case LE_CHUNK_ID_SEQ_2DBITMAP:
        Dimensionality = 2;
        break;

      case LE_CHUNK_ID_SEQ_3DMODEL:
      case LE_CHUNK_ID_SEQ_3DMESH:
        Dimensionality = 3;
        break;

      case LE_CHUNK_ID_SEQ_TWEEKER:
      case LE_CHUNK_ID_SEQ_PRELOADER:
        Dimensionality = 0; // These things have no coordinates of their own.
        break;

      default:
        // Examine the subchunks to see if there is a transformation chunk
        // that can give us a hint about the dimensionality.  Stop when
        // we hit another sequence chunk (will be start of the subsequences)
        // or get the no more chunks error (type is zero for errors).
        LE_CHUNK_SetCurrentFilePosition (StreamID, SubChunksStartOffset);

        while (TRUE)
        {
          ChunkType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, NULL);
          if (ChunkType == LE_CHUNK_ID_NULL_STANDARD)
            break; // Hit end of the enclosing parent chunk or file.

          if (ChunkType >= LE_CHUNK_ID_SEQ_GROUPING &&
          ChunkType < LE_CHUNK_ID_SEQ_MAX)
          {
            LE_CHUNK_Ascend (StreamID); // Undo descend.
            break; // Hit a subchunk, no more data describing this sequence.
          }

          if (ChunkType == LE_SEQNCR_CHUNK_ID_DIMENSIONALITY)
          {
            DimensionalityChunkPntr = (LE_SEQNCR_DimensionalityChunkPointer)
              LE_CHUNK_MapChunkData (StreamID,
              sizeof (LE_SEQNCR_DimensionalityChunkRecord), &AmountRead);

            if (AmountRead != sizeof (LE_SEQNCR_DimensionalityChunkRecord) ||
            DimensionalityChunkPntr->dimensionality == 1 ||
            DimensionalityChunkPntr->dimensionality > 3)
              goto BadChunkExit;

            Dimensionality = DimensionalityChunkPntr->dimensionality;
            LE_CHUNK_Ascend (StreamID); // Undo descend.
            break; // Yes, got a definative dimensionality.
          }

          // Only descended to get chunk type for the following ones,
          // no need to read data, so undo descend here.

          LE_CHUNK_Ascend (StreamID);

          if (ChunkType == LE_SEQNCR_CHUNK_ID_2D_XFORM ||
          ChunkType == LE_SEQNCR_CHUNK_ID_2D_OFFSET ||
          ChunkType == LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET ||
          ChunkType == LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX)
          {
            Dimensionality = 2;
            break;
          }

          if (ChunkType == LE_SEQNCR_CHUNK_ID_3D_XFORM ||
          ChunkType == LE_SEQNCR_CHUNK_ID_3D_OFFSET ||
          ChunkType == LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET ||
          ChunkType == LE_SEQNCR_CHUNK_ID_3D_BOUNDING_BOX ||
          ChunkType == LE_SEQNCR_CHUNK_ID_3D_BOUNDING_SPHERE ||
          ChunkType == LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE)
          {
            Dimensionality = 3;
            break;
          }
        } // End while.
        break;
    } // End switch on sequence type.
  } // End if chunky dataID type.
  else if (TypeOfDataID == LE_DATA_DataUAP ||
  TypeOfDataID == LE_DATA_DataNative ||
  TypeOfDataID == LE_DATA_DataGenericBitmap)
  {
    SequenceType = LE_CHUNK_ID_SEQ_2DBITMAP;
    Dimensionality = 2;
    memset (&SequenceHeader, 0, sizeof (SequenceHeader));
    SequenceHeader.timeMultiple = CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ;
    SequenceHeader.endingAction = LE_SEQNCR_EndingActionStayAtEnd;
  }
  else if (TypeOfDataID == LE_DATA_DataWave)
  {
    SequenceType = LE_CHUNK_ID_SEQ_SOUND;
    memset (&SequenceHeader, 0, sizeof (SequenceHeader));
    SequenceHeader.timeMultiple = CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ / 4;
    SequenceHeader.endingAction = LE_SEQNCR_EndingActionStop;
    // Inherit parent dimensionality.
  }
  else if (TypeOfDataID == LE_DATA_DataMESHX)
  {
    SequenceType = LE_CHUNK_ID_SEQ_3DMESH;
    memset (&SequenceHeader, 0, sizeof (SequenceHeader));
    SequenceHeader.timeMultiple = CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ;
    SequenceHeader.endingAction = LE_SEQNCR_EndingActionStayAtEnd;
    Dimensionality = 3;
  }
  else // Unknown type of DataID.
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
      "DataID $%08X has an unknown data type of %d.\r\n",
      (int) DataID, (int) TypeOfDataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Bad data type.
  }

  // Allocate the main runtime sequence information record.

  RuntimeInfoPntr = LI_SEQNCR_AllocRuntimeInfo ();
  if (RuntimeInfoPntr == NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
      "Ran out of sequence runtime info structures for DataID $%08X.\r\n",
      (int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Out of memory.
  }

  // Allocate attached runtime info records that contain position etc.

  if (Dimensionality >= 2)
  {
    if (Dimensionality == 2)
      RuntimeInfoPntr->dimensionSpecificData.d2 = Runtime2DInfoPntr =
      LI_SEQNCR_AllocRuntime2DInfo ();
    else if (Dimensionality == 3)
      RuntimeInfoPntr->dimensionSpecificData.d3 = Runtime3DInfoPntr =
      LI_SEQNCR_AllocRuntime3DInfo ();

    if (RuntimeInfoPntr->dimensionSpecificData.d0 == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
        "Ran out of sequence runtime %dD structures for DataID $%08X.\r\n",
        (int) Dimensionality, (int) DataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;  // Out of memory.
    }
  }

  // Allocate audio/video/tweeker stream runtime info if needed.
  // Note that the extra info is only used in 3D cameras.

  if (SequenceType == LE_CHUNK_ID_SEQ_VIDEO ||
  SequenceType == LE_CHUNK_ID_SEQ_SOUND ||
  SequenceType == LE_CHUNK_ID_SEQ_TWEEKER ||
  (SequenceType == LE_CHUNK_ID_SEQ_CAMERA && Dimensionality == 3))
  {
    if (SequenceType == LE_CHUNK_ID_SEQ_SOUND)
      i = sizeof (LE_SOUND_AudioStreamRecord);
    else if (SequenceType == LE_CHUNK_ID_SEQ_VIDEO)
      i = sizeof (LE_SEQNCR_RuntimeVideoInfoRecord);
    else if (SequenceType == LE_CHUNK_ID_SEQ_CAMERA)
      i = sizeof (LE_SEQNCR_Runtime3DCameraInfoRecord);
    else
      i = sizeof (LE_SEQNCR_RuntimeTweekerInfoRecord);

    RuntimeInfoPntr->streamSpecificData.voidPntr = LI_MEMORY_AllocLibraryBlock (i);

    if (RuntimeInfoPntr->streamSpecificData.voidPntr == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
        "Unable to allocate stream runtime record for sequence type %d, "
        "DataID $%08X.\r\n", (int) SequenceType, (int) DataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit; // Out of memory.
    }

    // Initialise it to default values.
    memset (RuntimeInfoPntr->streamSpecificData.voidPntr, 0, i);
  }

  // Fill in the standard fields of the runtime info, most are just copies
  // of the sequence header or other obvious information.  Ones not
  // mentioned here are initialised to zero.

  RuntimeInfoPntr->sequenceType = SequenceType;

  RuntimeInfoPntr->dimensionality = Dimensionality;

  RuntimeInfoPntr->parentStartTime = SequenceHeader.parentStartTime;

  RuntimeInfoPntr->parentTimeOfLastUpdate = INT32_MIN; // Means never updated.

  // Treat end time zero like infinity (well, 120 years).  But don't set the
  // end time to the largest value since the clock does get set to the end time
  // when a chain is done.  So put it about a million ticks away from the max.
  RuntimeInfoPntr->endTime = SequenceHeader.endTime;
  if (RuntimeInfoPntr->endTime == 0)
    RuntimeInfoPntr->endTime = INFINITE_END_TIME;

  RuntimeInfoPntr->timeMultiple = (UNS8) SequenceHeader.timeMultiple;
  if (CommandCommonDataPntr != NULL && CommandCommonDataPntr->timeMultiple != 0)
    RuntimeInfoPntr->timeMultiple = CommandCommonDataPntr->timeMultiple;

  RuntimeInfoPntr->endingAction = (LE_SEQNCR_EndingAction) SequenceHeader.endingAction;
  if (CommandFlags.asFlags.SEQFLAG_ReplacementEndingAction != LE_SEQNCR_NoEndingAction)
    RuntimeInfoPntr->endingAction = (LE_SEQNCR_EndingAction)
    CommandFlags.asFlags.SEQFLAG_ReplacementEndingAction;

  DropFramesOption = (LE_SEQNCR_DropFrames)
    CommandFlags.asFlags.SEQFLAG_DropFramesOption;
  if (DropFramesOption == LE_SEQNCR_DropKeepFrames)
    RuntimeInfoPntr->dropFrames = FALSE;
  else if (DropFramesOption == LE_SEQNCR_DropDropFrames)
    RuntimeInfoPntr->dropFrames = TRUE;
  else
    RuntimeInfoPntr->dropFrames = SequenceHeader.dropFrames;

  RuntimeInfoPntr->dataID = DataID;

  RuntimeInfoPntr->dataOffset = DataOffset;

  RuntimeInfoPntr->absoluteDataIDs = SequenceHeader.absoluteDataIDs;
  if (CommandFlags.asFlags.SEQFLAG_UseAbsoluteDataIDs)
    RuntimeInfoPntr->absoluteDataIDs = TRUE;

  RuntimeInfoPntr->disposeSequenceDataWhenDone =
    CommandFlags.asFlags.SEQFLAG_HowToDisposeSequenceDataWhenDone;

  RuntimeInfoPntr->unrefSequenceDataWhenDone =
    CommandFlags.asFlags.SEQFLAG_UnrefSequenceDataWhenDone;

  RuntimeInfoPntr->priority = (UNS16) SequenceHeader.priority;
  if (StartSequenceCommandPntr != NULL) // User gave us a priority.
    RuntimeInfoPntr->priority = (UNS16) StartSequenceCommandPntr->numberB;

  RuntimeInfoPntr->lastUse = SequenceHeader.lastUse;

  RuntimeInfoPntr->watchIndex = -1;

  RuntimeInfoPntr->scrollingWorld = SequenceHeader.scrollingWorld;

  if (CommandFlags.asFlags.SEQFLAG_ForceRedrawEveryUpdate)
    RuntimeInfoPntr->redrawAtEveryUpdate = TRUE;

  RuntimeInfoPntr->needsRedraw = TRUE;
  RuntimeInfoPntr->needsPositionRecalc = TRUE;
  RuntimeInfoPntr->needsReEvaluation = TRUE;

  // The sequence's starting clock is optionally offset by the lateness of
  // the parent, so that if we are skipping frames, we start late.  Plus the
  // user can provide an additional offset.  In fact, the sequence may stop
  // right at the first update if it started past its ending time.  That
  // should prevent it from showing up on the screen if we are skipping
  // frames (the render slot gets a sequence started and then soon after
  // a sequence shutdown, before it actually renders).  Note that top level
  // sequences ignore the skip frames property when initialising their clock
  // so that they don't grab the global clock and thus exit right away.

  if (RuntimeInfoPntr->dropFrames &&
  ParentPntr != LI_SEQNCR_CommonRuntimeTreeRoot)
  {
    RuntimeInfoPntr->sequenceClock +=
    (ParentPntr->sequenceClock - RuntimeInfoPntr->parentStartTime);
  }

  if (CommandCommonDataPntr != NULL)
  {
    // Add in the user's starting time offset.

    RuntimeInfoPntr->sequenceClock += CommandCommonDataPntr->initialClockOffset;

    // Set the user's callback function if there is one.

    RuntimeInfoPntr->callBack = CommandCommonDataPntr->callBack;
    RuntimeInfoPntr->userData = CommandCommonDataPntr->userData;
  }

  // Find all render slots which can accept this type of sequence.
  // The user can also specify a subset of the slots to put it in,
  // or if they didn't (but they did start it) then use the default.
  // Otherwise (non-user started sequence) it just inherits the
  // parent sequence's slots.

  if (StartSequenceCommandPntr != NULL)
  {
    if (CommandCommonDataPntr != NULL &&
    CommandCommonDataPntr->renderSlotSet != 0)
      AllowedRenderSlots = CommandCommonDataPntr->renderSlotSet;
    else
      AllowedRenderSlots = DefaultRenderSlotSetForStartSequence;
  }
  else // Inherit from parent.
    AllowedRenderSlots = ParentPntr->renderSlotSet;

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
  {
    if (AllowedRenderSlots & (1 << RenderSlot))
    {
      RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];

      if (RenderSlotPntr != NULL &&
      RenderSlotPntr->CanHandleSequenceType (SequenceType, Dimensionality))
      {
        RuntimeInfoPntr->renderSlotSet |= (1 << RenderSlot);
      }
    }
  }

  // The sequence is initially on screen in all its render slots.  The onScreen
  // field will get updated during rendering with the actual status.

  RuntimeInfoPntr->onScreen = RuntimeInfoPntr->renderSlotSet;

  // Set some sequence type specific defaults, which get overridden by
  // subchunks if present.  Other things default to zero

  if (SequenceType == LE_CHUNK_ID_SEQ_SOUND ||
  SequenceType == LE_CHUNK_ID_SEQ_VIDEO)
    RuntimeInfoPntr->aux.audio.volume = CE_ARTLIB_SeqncrDefaultSoundLevelPercent;
  else if (SequenceType == LE_CHUNK_ID_SEQ_CAMERA)
  {
    if (Dimensionality == 3)
    {
      RuntimeInfoPntr->aux.cameraFieldOfView = 0.7853981633974f; // pi/4.
      RuntimeInfoPntr->streamSpecificData.cameraData->nearClipPlaneDistance = 1.0F;
      RuntimeInfoPntr->streamSpecificData.cameraData->farClipPlaneDistance = 5000.0F;
    }
    else // No extra data for 2D cameras, just field of view as a scale factor.
      RuntimeInfoPntr->aux.cameraFieldOfView = 1.0F;
  }

  // More initial starting value setup from subchunks.

  if (TypeOfDataID == LE_DATA_DataChunky)
  {
    // Go through all the chunks looking for ones that provide more
    // information about the sequence.  If there are transformation chunks, we
    // use them if we don't already have the transformation.  This implies that
    // only the first transformation chunk is used, so if you have both a matrix
    // and an equivalent move/scale/rotate you can put the matrix first for
    // speed.  If none is found, the identity transformation is used.
    //
    // We also do the label chunks and dope sheet selection chunks and
    // bounding box chunks here, rather than rescanning the chunk list
    // repeatedly.
    //
    // Also, we check for subsequences here.  If any are found then the
    // childSequencesDataID and childSequencesOffset get set to show
    // that children exist (otherwise they stay at zero meaning that
    // no child subsequences exist thus avoiding wasting time scanning
    // the subchunks while the sequence is running).

    LE_CHUNK_SetCurrentFilePosition (StreamID, SubChunksStartOffset);
    ChildSequencesFound = FALSE;

    while (TRUE)
    {
      ChunkType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, NULL);
      if (ChunkType == LE_CHUNK_ID_NULL_STANDARD)
        break; // Reached end of enclosing parent chunk.

      if (!ChildSequencesFound && ChunkType >= LE_CHUNK_ID_SEQ_GROUPING &&
      ChunkType < LE_CHUNK_ID_SEQ_MAX)
      {
        RuntimeInfoPntr->childSequencesDataID = DataID;
        RuntimeInfoPntr->childSequencesOffset = SubChunksStartOffset;
        ChildSequencesFound = TRUE;
      }

      switch (ChunkType)
      {
      case LE_CHUNK_ID_SEQ_TWEEKER:
        RuntimeInfoPntr->hasTweekerChildren = TRUE;
        break;

      case LE_CHUNK_ID_FILE_NAME_1:
      case LE_CHUNK_ID_FILE_NAME_2:
      case LE_CHUNK_ID_FILE_NAME_3:
      case LE_CHUNK_ID_FILE_NAME_4:
      case LE_CHUNK_ID_FILE_NAME_5:
        // Got a file name, usually used for a video or
        // for streaming audio from a file.  Just save the
        // last file name encountered, if there are several.
        LE_CHUNK_ReadChunkData (StreamID, FileName1, sizeof (FileName1));
        FileName1 [sizeof (FileName1) - 1] = 0; // Force NUL at end of string.

        // Convert it to an absolute path name, using the search paths
        // to find it.  If it doesn't exist, just leaves the string alone.
#if CE_ARTLIB_EnableSystemDirIni
        if (LE_DIRINI_ExpandFileName (FileName1, TempString, NULL))
          strcpy (FileName1, TempString);
#endif // CE_ARTLIB_EnableSystemDirIni
        break;

      case LE_CHUNK_ID_DOPE_DATA:
        DopeDataPntr = (LE_CHUNK_DopeDataPointer)
          LE_CHUNK_MapChunkData (StreamID,
          sizeof (LE_CHUNK_DopeDataRecord), &AmountRead);
        if (AmountRead != sizeof (LE_CHUNK_DopeDataRecord))
          goto BadChunkExit;
        if (DopeDataPntr->flags & LE_CHUNK_DopeDataFlagSelected)
          RuntimeInfoPntr->drawSelectionBox = TRUE;
#if CE_ARTLIB_SeqncrUseSerialNumbers
        RuntimeInfoPntr->serialNumber = DopeDataPntr->serialNumber;
#endif
        break;

      case LE_SEQNCR_CHUNK_ID_2D_XFORM:
        if (Dimensionality == 2 && !FoundTransformationData)
        {
          XForm2DChunkPntr = (LE_SEQNCR_2DXformChunkPointer)
            LE_CHUNK_MapChunkData (StreamID,
            sizeof (LE_SEQNCR_2DXformChunkRecord), &AmountRead);
          if (AmountRead != sizeof (LE_SEQNCR_2DXformChunkRecord))
            goto BadChunkExit;

          Runtime2DInfoPntr->sequenceToParentTransformation.matrix2D =
            XForm2DChunkPntr->matrix;
          Runtime2DInfoPntr->sequenceToParentTransformNonIdentity = TRUE;

          FoundTransformationData = TRUE;
        }
        break;

      case LE_SEQNCR_CHUNK_ID_2D_OFFSET:
        if (Dimensionality == 2 && !FoundTransformationData)
        {
          Offset2DChunkPntr = (LE_SEQNCR_2DOffsetChunkPointer)
            LE_CHUNK_MapChunkData (StreamID,
            sizeof (LE_SEQNCR_2DOffsetChunkRecord), &AmountRead);
          if (AmountRead != sizeof (LE_SEQNCR_2DOffsetChunkRecord))
            goto BadChunkExit;

          LE_MATRIX_2D_SetTranslate (&Offset2DChunkPntr->offset,
            &Runtime2DInfoPntr->sequenceToParentTransformation.matrix2D);
          Runtime2DInfoPntr->sequenceToParentTransformNonIdentity = TRUE;

          FoundTransformationData = TRUE;
        }
        break;

      case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET:
        if (Dimensionality == 2 && !FoundTransformationData)
        {
          OSRT2DChunkPntr = (LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer)
            LE_CHUNK_MapChunkData (StreamID,
            sizeof (LE_SEQNCR_2DOriginScaleRotateOffsetChunkRecord), &AmountRead);
          if (AmountRead != sizeof (LE_SEQNCR_2DOriginScaleRotateOffsetChunkRecord))
            goto BadChunkExit;

          // The transformations are done in the order: scale, rotate, offset.

          LE_MATRIX_2D_SetOriginScaleRotateTranslate (
            &OSRT2DChunkPntr->origin,
            OSRT2DChunkPntr->scaleX, OSRT2DChunkPntr->scaleY,
            OSRT2DChunkPntr->rotate, &OSRT2DChunkPntr->offset,
            &Runtime2DInfoPntr->sequenceToParentTransformation.matrix2D);
          Runtime2DInfoPntr->sequenceToParentTransformNonIdentity = TRUE;

          FoundTransformationData = TRUE;
        }
        break;

      case LE_SEQNCR_CHUNK_ID_3D_XFORM:
        if (Dimensionality == 3 && !FoundTransformationData)
        {
          XForm3DChunkPntr = (LE_SEQNCR_3DXformChunkPointer)
            LE_CHUNK_MapChunkData (StreamID,
            sizeof (LE_SEQNCR_3DXformChunkRecord), &AmountRead);
          if (AmountRead != sizeof (LE_SEQNCR_3DXformChunkRecord))
            goto BadChunkExit;

          Runtime3DInfoPntr->sequenceToParentTransformation.matrix3D =
            XForm3DChunkPntr->matrix;
          Runtime3DInfoPntr->sequenceToParentTransformNonIdentity = TRUE;

          FoundTransformationData = TRUE;
        }
        break;

      case LE_SEQNCR_CHUNK_ID_3D_OFFSET:
        if (Dimensionality == 3 && !FoundTransformationData)
        {
          Offset3DChunkPntr = (LE_SEQNCR_3DOffsetChunkPointer)
            LE_CHUNK_MapChunkData (StreamID,
            sizeof (LE_SEQNCR_3DOffsetChunkRecord), &AmountRead);
          if (AmountRead != sizeof (LE_SEQNCR_3DOffsetChunkRecord))
            goto BadChunkExit;

          LE_MATRIX_3D_SetTranslate (&Offset3DChunkPntr->offset,
            &Runtime3DInfoPntr->sequenceToParentTransformation.matrix3D);
          Runtime3DInfoPntr->sequenceToParentTransformNonIdentity = TRUE;

          FoundTransformationData = TRUE;
        }
        break;

      case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET:
        if (Dimensionality == 3 && !FoundTransformationData)
        {
          OSRT3DChunkPntr = (LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer)
            LE_CHUNK_MapChunkData (StreamID,
            sizeof (LE_SEQNCR_3DOriginScaleRotateOffsetChunkRecord), &AmountRead);
          if (AmountRead != sizeof (LE_SEQNCR_3DOriginScaleRotateOffsetChunkRecord))
            goto BadChunkExit;

          // This specifies a 3D transformation by using more understandable
          // individual values for moving and rotating.  It is equivalent to
          // doing the scale, roll, pitch, yaw, and offset transformations in
          // that order.  In 3D the character is facing forwards along the
          // positive Z axis (yaw zero by definition), so you see the
          // character's back when he is in its default orientation.

          LE_MATRIX_3D_SetOriginScaleRotateTranslate (
            &OSRT3DChunkPntr->origin, OSRT3DChunkPntr->scaleX,
            OSRT3DChunkPntr->scaleY, OSRT3DChunkPntr->scaleZ,
            OSRT3DChunkPntr->yaw, OSRT3DChunkPntr->pitch,
            OSRT3DChunkPntr->roll, &OSRT3DChunkPntr->offset,
            &Runtime3DInfoPntr->sequenceToParentTransformation.matrix3D);
          Runtime3DInfoPntr->sequenceToParentTransformNonIdentity = TRUE;

          FoundTransformationData = TRUE;
        }
        break;

      case LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX:
        if (Dimensionality == 2)
        {
          BoundingBox2DPntr = (LE_SEQNCR_2DBoundingBoxChunkPointer)
            LE_CHUNK_MapChunkData (StreamID,
            sizeof (LE_SEQNCR_2DBoundingBoxChunkRecord), &AmountRead);
          if (AmountRead != sizeof (LE_SEQNCR_2DBoundingBoxChunkRecord))
            goto BadChunkExit;

          Runtime2DInfoPntr->boundingBox = BoundingBox2DPntr->boundingRect;

          FoundBoundingBoxData = TRUE;
        }
        break;

      case LE_SEQNCR_CHUNK_ID_3D_BOUNDING_BOX:
        if (Dimensionality == 3)
        {
          BoundingBox3DPntr = (LE_SEQNCR_3DBoundingBoxChunkPointer)
            LE_CHUNK_MapChunkData (StreamID,
            sizeof (LE_SEQNCR_3DBoundingBoxChunkRecord), &AmountRead);
          if (AmountRead != sizeof (LE_SEQNCR_3DBoundingBoxChunkRecord))
            goto BadChunkExit;

          for (i = 0; i < 8; i++)
            Runtime3DInfoPntr->boundingBox[i] = BoundingBox3DPntr->boundingCube[i];

          FoundBoundingBoxData = TRUE;
        }
        break;

      case LE_SEQNCR_CHUNK_ID_3D_BOUNDING_SPHERE:
        if (Dimensionality == 3)
        {
          BoundingSphere3DPntr = (LE_SEQNCR_3DBoundingSphereChunkPointer)
            LE_CHUNK_MapChunkData (StreamID,
            sizeof (LE_SEQNCR_3DBoundingSphereChunkRecord), &AmountRead);
          if (AmountRead != sizeof (LE_SEQNCR_3DBoundingSphereChunkRecord))
            goto BadChunkExit;

          // Crudely convert the bounding sphere into a cube centered
          // around the origin.

          RadiusPositive = BoundingSphere3DPntr->radius;
          RadiusNegative = -RadiusPositive;

          for (i = 0; i < 8; i++)
          {
            Temp3DPoint.x = (i & 1) ? RadiusPositive : RadiusNegative;
            Temp3DPoint.y = (i & 2) ? RadiusPositive : RadiusNegative;
            Temp3DPoint.z = (i & 4) ? RadiusPositive : RadiusNegative;
            Runtime3DInfoPntr->boundingBox[i] = Temp3DPoint;
          }

          FoundBoundingBoxData = TRUE;
        }
        break;

      case LE_SEQNCR_CHUNK_ID_HIT_BOX_LABEL:
        LabelChunkPntr = (LE_SEQNCR_LabelChunkPointer)
          LE_CHUNK_MapChunkData (StreamID,
          sizeof (LE_SEQNCR_LabelChunkRecord), &AmountRead);
        if (AmountRead != sizeof (LE_SEQNCR_LabelChunkRecord))
          goto BadChunkExit;
        RuntimeInfoPntr->labelNumber = LabelChunkPntr->labelNumber;
        break;

      case LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE:
        if (Dimensionality == 3)
        {
          MeshChunkPntr = (LE_SEQNCR_3DMeshChoiceChunkPointer)
            LE_CHUNK_MapChunkData (StreamID,
            sizeof (LE_SEQNCR_3DMeshChoiceChunkRecord), &AmountRead);
          if (AmountRead != sizeof (LE_SEQNCR_3DMeshChoiceChunkRecord))
            goto BadChunkExit;
          RuntimeInfoPntr->dimensionSpecificData.d3->meshIndexA = MeshChunkPntr->meshIndexA;
          RuntimeInfoPntr->dimensionSpecificData.d3->meshIndexB = MeshChunkPntr->meshIndexB;
          RuntimeInfoPntr->dimensionSpecificData.d3->meshProportion = MeshChunkPntr->meshProportion;
        }
        break;

      case LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH:
        SoundPitchChunkPntr = (LE_SEQNCR_SoundPitchChunkPointer)
          LE_CHUNK_MapChunkData (StreamID,
          sizeof (LE_SEQNCR_SoundPitchChunkRecord), &AmountRead);
        if (AmountRead != sizeof (LE_SEQNCR_SoundPitchChunkRecord))
          goto BadChunkExit;
        RuntimeInfoPntr->aux.audio.pitch = SoundPitchChunkPntr->soundPitch;
        break;

      case LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME:
        SoundVolumeChunkPntr = (LE_SEQNCR_SoundVolumeChunkPointer)
          LE_CHUNK_MapChunkData (StreamID,
          sizeof (LE_SEQNCR_SoundVolumeChunkRecord), &AmountRead);
        if (AmountRead != sizeof (LE_SEQNCR_SoundVolumeChunkRecord))
          goto BadChunkExit;
        RuntimeInfoPntr->aux.audio.volume = SoundVolumeChunkPntr->soundVolume;
        break;

      case LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING:
        SoundPanningChunkPntr = (LE_SEQNCR_SoundPanningChunkPointer)
          LE_CHUNK_MapChunkData (StreamID,
          sizeof (LE_SEQNCR_SoundPanningChunkRecord), &AmountRead);
        if (AmountRead != sizeof (LE_SEQNCR_SoundPanningChunkRecord))
          goto BadChunkExit;
        RuntimeInfoPntr->aux.audio.panning = SoundPanningChunkPntr->soundPanning;
        break;

      case LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW:
        FieldOfViewChunkPntr = (LE_SEQNCR_FieldOfViewChunkPointer)
          LE_CHUNK_MapChunkData (StreamID,
          sizeof (LE_SEQNCR_FieldOfViewChunkRecord), &AmountRead);
        if (AmountRead != sizeof (LE_SEQNCR_FieldOfViewChunkRecord))
          goto BadChunkExit;
        RuntimeInfoPntr->aux.cameraFieldOfView = FieldOfViewChunkPntr->fieldOfView;
        break;

      } // End switch (ChunkType)

      LE_CHUNK_Ascend (StreamID);
    } // End while more sequence description chunks to descend into for processing subchunks.
  }

  // Fill in the position information from the command record if present, now
  // that we might have read in the default position info from the sequence.

  if (Dimensionality == 2 && CommandCommonDataPntr != NULL &&
  StartSequenceCommandPntr->stringASize == sizeof (SeqCmdStart2DAttachmentRecord))
  {
    Command2DDataPntr = (SeqCmdStart2DAttachmentPointer) CommandCommonDataPntr;
    Runtime2DInfoPntr->sequenceToParentTransformation.matrix2D =
      Command2DDataPntr->positionMatrix;
    Runtime2DInfoPntr->sequenceToParentTransformNonIdentity = TRUE;
    FoundTransformationData = TRUE;
  }
  else if (Dimensionality == 3 && CommandCommonDataPntr != NULL &&
  StartSequenceCommandPntr->stringASize == sizeof (SeqCmdStart3DAttachmentRecord))
  {
    Command3DDataPntr = (SeqCmdStart3DAttachmentPointer) CommandCommonDataPntr;
    Runtime3DInfoPntr->sequenceToParentTransformation.matrix3D =
      Command3DDataPntr->positionMatrix;
    Runtime3DInfoPntr->sequenceToParentTransformNonIdentity = TRUE;
    FoundTransformationData = TRUE;
  }

  // If no transformation specified, default to the identity transformation
  // by leaving the sequenceToParentTransformNonIdentity flag set to FALSE.
  // Used to see if the FoundTransformationData was FALSE and then set the
  // matrix, but don't have to do anything now.  The actual matrix stays
  // filled with zeroes so that you can quickly see that it isn't being
  // used when debugging.

  // Process the sequence type specific part of the header.
  // If necessary, make a fake sequence header.

  if (TypeOfDataID == LE_DATA_DataChunky)
  {
    LE_CHUNK_SetCurrentFilePosition (StreamID,
      DataOffset + sizeof (LE_CHUNK_HeaderRecord)); // Back to start of header.

    VoidPntr = LE_CHUNK_MapChunkData (StreamID,
      LE_CHUNK_ChunkFixedDataSizes [SequenceType], &AmountRead);
    if (AmountRead != LE_CHUNK_ChunkFixedDataSizes [SequenceType])
      goto BadChunkExit;
  }
  else if (TypeOfDataID == LE_DATA_DataUAP ||
  TypeOfDataID == LE_DATA_DataNative ||
  TypeOfDataID == LE_DATA_DataGenericBitmap)
  {
    memset (&Fake2DBitmapSequenceData, 0, sizeof (Fake2DBitmapSequenceData));
    VoidPntr = &Fake2DBitmapSequenceData;
    Fake2DBitmapSequenceData.bitmapDataID = DataID;
  }
  else if (TypeOfDataID == LE_DATA_DataWave)
  {
    memset (&FakeSoundSequenceData, 0, sizeof (FakeSoundSequenceData));
    VoidPntr = &FakeSoundSequenceData;
    FakeSoundSequenceData.soundDataID = DataID;
  }
  else if (TypeOfDataID == LE_DATA_DataMESHX)
  {
    memset (&Fake3DSequenceData, 0, sizeof (Fake3DSequenceData));
    VoidPntr = &Fake3DSequenceData;
    Fake3DSequenceData.modelDataID = DataID;
  }
#if CE_ARTLIB_EnableDebugMode
  else
  {
    LE_ERROR_DebugMsg ("LI_SEQNCR_StartUpSequence: "
      "Code bug, not handling a data type you should be doing.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
    goto ErrorExit;
  }
#endif


  switch (SequenceType)
  {
  case LE_CHUNK_ID_SEQ_GROUPING: // These ones don't do anything special.
    break;


  case LE_CHUNK_ID_SEQ_INDIRECT:
    // Use the indirect data item as the source of child subsequences.
    if (RuntimeInfoPntr->absoluteDataIDs)
      RuntimeInfoPntr->childSequencesDataID =
      ((LE_SEQNCR_SequenceIndirectChunkPointer)VoidPntr)->
      subsequenceChunkListDataID;
    else
      RuntimeInfoPntr->childSequencesDataID = LE_DATA_IdWithFileFromParent (
      ((LE_SEQNCR_SequenceIndirectChunkPointer)VoidPntr)->
      subsequenceChunkListDataID, DataID);
    RuntimeInfoPntr->childSequencesOffset = 0;
    break;


  case LE_CHUNK_ID_SEQ_2DBITMAP:
    // Set the bounding box from the bitmap size.  Also do any other
    // bitmap pre-processing here, like converting 24 bit BMPs into
    // native screen depth.

    if (RuntimeInfoPntr->absoluteDataIDs)
      RuntimeInfoPntr->contentsDataID =
      ((LE_SEQNCR_Sequence2DBitmapChunkPointer) VoidPntr)->bitmapDataID;
    else
      RuntimeInfoPntr->contentsDataID = LE_DATA_IdWithFileFromParent (
      ((LE_SEQNCR_Sequence2DBitmapChunkPointer) VoidPntr)->bitmapDataID,
      DataID);

    TypeOfBitmapDataID =
      LE_DATA_GetLoadedDataType (RuntimeInfoPntr->contentsDataID);

    if (FoundBoundingBoxData)
      break; // Already have an explicit bounding box.

    // If this is a TAB file, there is a hidden offset in the bitmap data.

    if (TypeOfBitmapDataID == LE_DATA_DataUAP)
    {
      BitmapUCPHeaderPntr = (LPNEWBITMAPHEADER)
        LE_DATA_Use (RuntimeInfoPntr->contentsDataID);
      if (BitmapUCPHeaderPntr == NULL)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
          "Unreadable UAP bitmap at data ID $%08X, "
          "from sequence with data ID $%08X and offset %d.\r\n",
          (int) RuntimeInfoPntr->contentsDataID, (int) DataID, (int) DataOffset);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        goto ErrorExit; // Unreadable bitmap.
      }
      BitmapWidth = BitmapUCPHeaderPntr->nXBitmapWidth;
      BitmapHeight = BitmapUCPHeaderPntr->nYBitmapHeight;
      BitmapOffsetX = BitmapUCPHeaderPntr->nXOriginOffset;
      BitmapOffsetY = BitmapUCPHeaderPntr->nYOriginOffset;
    }
    else if (TypeOfBitmapDataID == LE_DATA_DataNative)
    {
      // A runtime bitmap created by LE_GRAFIX_ObjectCreate, uses current
      // screen depth and has an optional global alpha, but no palette.
      // Instead the nAlpha field is transparency level rather than number
      // of colours.  The flags also specify transparency on or off.

      BitmapUCPHeaderPntr = (LPNEWBITMAPHEADER)
        LE_DATA_Use (RuntimeInfoPntr->contentsDataID);
      if (BitmapUCPHeaderPntr == NULL)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
          "Unreadable native bitmap at data ID $%08X, "
          "from sequence with data ID $%08X and offset %d.\r\n",
          (int) RuntimeInfoPntr->contentsDataID, (int) DataID, (int) DataOffset);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        goto ErrorExit; // Unreadable bitmap.
      }
      BitmapWidth = BitmapUCPHeaderPntr->nXBitmapWidth;
      BitmapHeight = BitmapUCPHeaderPntr->nYBitmapHeight;
      BitmapOffsetX = BitmapUCPHeaderPntr->nXOriginOffset;
      BitmapOffsetY = BitmapUCPHeaderPntr->nYOriginOffset;
    }
    else if (TypeOfBitmapDataID == LE_DATA_DataGenericBitmap)
    {
      // A DirectDraw surface in a wrapper.

      GBMPntr = (LE_GRAFIX_GenericBitmapPointer)
        LE_DATA_Use (RuntimeInfoPntr->contentsDataID);
      if (GBMPntr == NULL)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
          "Unreadable generic bitmap at data ID $%08X, "
          "from sequence with data ID $%08X and offset %d.\r\n",
          (int) RuntimeInfoPntr->contentsDataID, (int) DataID, (int) DataOffset);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        goto ErrorExit; // Unreadable bitmap.
      }
      BitmapWidth = GBMPntr->width;
      BitmapHeight = GBMPntr->height;
      BitmapOffsetX = BitmapOffsetY = 0;
    }
    else // Some other kind of bitmap.
    {
      BitmapWidth = LE_GRAFIX_AnyBitmapWidth (RuntimeInfoPntr->contentsDataID);
      BitmapHeight = LE_GRAFIX_AnyBitmapHeight (RuntimeInfoPntr->contentsDataID);
      BitmapOffsetX = BitmapOffsetY = 0;
    }

    // Force the offset to zero if the user wants to ignore the bitmap's
    // internal offset.  This is the normal situation with manually
    // started bitmaps.

    if (CommandFlags.asFlags.SEQFLAG_ZeroBitmapOffset)
      BitmapOffsetX = BitmapOffsetY = 0;

    // A little sanity check.

    if (BitmapWidth == 0 || BitmapHeight == 0)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
        "Bad bitmap header at data ID $%08X, "
        "from sequence with data ID $%08X and offset %d.\r\n",
        (int) RuntimeInfoPntr->contentsDataID, (int) DataID, (int) DataOffset);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }

    // Set the bounding box.

    Runtime2DInfoPntr->boundingBox.left = BitmapOffsetX;
    Runtime2DInfoPntr->boundingBox.top = BitmapOffsetY;
    Runtime2DInfoPntr->boundingBox.right = BitmapOffsetX + BitmapWidth;
    Runtime2DInfoPntr->boundingBox.bottom = BitmapOffsetY + BitmapHeight;
    FoundBoundingBoxData = TRUE;
    break;


  case LE_CHUNK_ID_SEQ_3DMODEL:
    if (RuntimeInfoPntr->absoluteDataIDs)
    {
      RuntimeInfoPntr->contentsDataID =
        ((LE_SEQNCR_Sequence3DModelChunkPointer) VoidPntr)->modelDataID;
      Runtime3DInfoPntr->textureMapDataID =
        ((LE_SEQNCR_Sequence3DModelChunkPointer) VoidPntr)->textureMapDataID;
      Runtime3DInfoPntr->jointPositionsDataID =
        ((LE_SEQNCR_Sequence3DModelChunkPointer) VoidPntr)->jointPositionsDataID;
    }
    else
    {
      RuntimeInfoPntr->contentsDataID = LE_DATA_IdWithFileFromParent (
        ((LE_SEQNCR_Sequence3DModelChunkPointer) VoidPntr)->modelDataID, DataID);
      Runtime3DInfoPntr->textureMapDataID = LE_DATA_IdWithFileFromParent (
        ((LE_SEQNCR_Sequence3DModelChunkPointer) VoidPntr)->textureMapDataID, DataID);
      Runtime3DInfoPntr->jointPositionsDataID = LE_DATA_IdWithFileFromParent (
        ((LE_SEQNCR_Sequence3DModelChunkPointer) VoidPntr)->jointPositionsDataID, DataID);
    }
    // Bleeble Todo: Handle bounding box. - JP (Jeff Preshing)
    break;


  case LE_CHUNK_ID_SEQ_3DMESH:
    if (RuntimeInfoPntr->absoluteDataIDs)
    {
      RuntimeInfoPntr->contentsDataID =
        ((LE_SEQNCR_Sequence3DMeshChunkPointer) VoidPntr)->modelDataID;
    }
    else
    {
      RuntimeInfoPntr->contentsDataID = LE_DATA_IdWithFileFromParent (
        ((LE_SEQNCR_Sequence3DMeshChunkPointer) VoidPntr)->modelDataID,
        DataID);
    }
    // Bleeble Todo: Handle bounding box.
    break;


  case LE_CHUNK_ID_SEQ_SOUND:
    OriginalUnconvertedDataID =
      ((LE_SEQNCR_SequenceSoundChunkPointer) VoidPntr)->soundDataID;

    if (RuntimeInfoPntr->absoluteDataIDs)
      RuntimeInfoPntr->contentsDataID = OriginalUnconvertedDataID;
    else
      RuntimeInfoPntr->contentsDataID =
      LE_DATA_IdWithFileFromParent (OriginalUnconvertedDataID, DataID);

    // Initial panning and volume and pitch are in aux.audio, and will
    // get applied when the sequence starts by the render slot.
    // 2D and 3D sounds also update the volume and panning when they move.

    if (!LE_SOUND_OpenAudioStream (RuntimeInfoPntr->streamSpecificData.audioStream,
    (OriginalUnconvertedDataID != LE_DATA_EmptyItem) ? NULL : FileName1,
    RuntimeInfoPntr->contentsDataID,
    (RuntimeInfoPntr->dimensionality == 3) /* Is it a 3D sound? */,
    FALSE /* Start in pause mode */, FALSE /* Permanent cache */))
      goto ErrorExit;

    // Patch up the ending time and time multiple to fit the actual sound length.

    SoundDuration = RuntimeInfoPntr->streamSpecificData.audioStream->duration;
    RuntimeInfoPntr->endTime = SoundDuration;
    if (!RuntimeInfoPntr->streamSpecificData.audioStream->doNotFeed)
    {
      // This is a spooling sound, feed it occasionally (usually every 2 seconds).

      if (OriginalUnconvertedDataID != LE_DATA_EmptyItem)
        // Spooling from in memory, feed it in small nibbles so that the
        // game doesn't slow down from decompressing large hunks, so that
        // you can play compressed sound from memory while simultaneously
        // spooling a silent video from CDROM.
        RuntimeInfoPntr->timeMultiple = CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ / 4;
      else
        // Spooling from a file, use a longer time between feeds to avoid
        // inefficient CDROM access.  Typically this will be 2 seconds long.
        RuntimeInfoPntr->timeMultiple = CE_ARTLIB_SoundSpoolBufferDuration / 3;
    }
    // Sanity check the time multiple.
    if (RuntimeInfoPntr->timeMultiple > RuntimeInfoPntr->endTime)
      RuntimeInfoPntr->timeMultiple = (UNS8) RuntimeInfoPntr->endTime;
    if (RuntimeInfoPntr->timeMultiple < 1)
      RuntimeInfoPntr->timeMultiple = 1;
    break;


  case LE_CHUNK_ID_SEQ_VIDEO:
    if (Dimensionality != 2)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
      "Video chunk at offset %d in DataID $%08X (name \"%s\") is %d dimensional, "
      "we can only handle 2 dimensional videos.\r\n",
      (int) LE_CHUNK_GetCurrentFilePosition (StreamID), (int) DataID,
      FileName1, (int) Dimensionality);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }
    RuntimeInfoPntr->streamSpecificData.videoStream->drawSolid =
      ((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->drawSolid;

    RuntimeInfoPntr->streamSpecificData.videoStream->flipVertically =
      ((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->flipVertically;

    RuntimeInfoPntr->streamSpecificData.videoStream->alphaLevel =
      ((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->alphaLevel;

    RuntimeInfoPntr->streamSpecificData.videoStream->enableVideo =
      ((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->enableVideo;

    RuntimeInfoPntr->streamSpecificData.videoStream->enableAudio =
      ((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->enableAudio;

#if !USE_OLD_VIDEO_PLAYER // If using new video players.
    RuntimeInfoPntr->streamSpecificData.videoStream->drawDirectlyToScreen =
      ((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->drawDirectlyToScreen;
#else // Old player can't draw to the screen, it always has a temp bitmap.
    #if CE_ARTLIB_EnableDebugMode
    if (((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->drawDirectlyToScreen)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
      "Sorry, the old video player can't draw directly to the screen.  "
      "Will turn off that option for %s.\r\n", FileName1);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    }
    #endif
    RuntimeInfoPntr->streamSpecificData.videoStream->drawDirectlyToScreen = FALSE;
#endif

    RuntimeInfoPntr->streamSpecificData.videoStream->doubleAlternateLines =
      ((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->doubleAlternateLines;

    RuntimeInfoPntr->streamSpecificData.videoStream->saturation =
      ((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->saturation;

    RuntimeInfoPntr->streamSpecificData.videoStream->brightness =
      ((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->brightness;

    RuntimeInfoPntr->streamSpecificData.videoStream->contrast =
      ((LE_SEQNCR_SequenceVideoChunkPointer) VoidPntr)->contrast;

    // Open the file and see what the size of the video is, then close it.

    if (!LE_SEQNCR_GetAVIFileCharacteristics (FileName1,
    &RuntimeInfoPntr->streamSpecificData.videoStream->videoFramesPerSecond,
    &RuntimeInfoPntr->streamSpecificData.videoStream->numberOfVideoFrames,
    &RuntimeInfoPntr->streamSpecificData.videoStream->videoRectangle))
      goto ErrorExit; // Unable to open video file, or not a video.

    // Check for doubling when not drawing to screen.  Don't want it.

    if (RuntimeInfoPntr->streamSpecificData.videoStream->enableVideo &&
    RuntimeInfoPntr->streamSpecificData.videoStream->doubleAlternateLines &&
    !RuntimeInfoPntr->streamSpecificData.videoStream->drawDirectlyToScreen)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
      "Sorry, you can't use the alternate scan line option for %s "
      "since you aren't drawing directly to the screen.  Will "
      "turn the option off.\r\n",
      FileName1);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      RuntimeInfoPntr->streamSpecificData.videoStream->
        doubleAlternateLines = FALSE;
    }

    if (FoundBoundingBoxData)
    {
      if (RuntimeInfoPntr->streamSpecificData.videoStream->enableVideo &&
      RuntimeInfoPntr->streamSpecificData.videoStream->doubleAlternateLines)
      {
        // Verify that the bounding box is twice the size of the movie,
        // when trying to use doubled scan lines.

        if (Runtime2DInfoPntr->boundingBox.right -
        Runtime2DInfoPntr->boundingBox.left != 2 * RuntimeInfoPntr->
        streamSpecificData.videoStream->videoRectangle.right ||
        Runtime2DInfoPntr->boundingBox.bottom -
        Runtime2DInfoPntr->boundingBox.top != 2 * RuntimeInfoPntr->
        streamSpecificData.videoStream->videoRectangle.bottom)
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
          "Video bounding box (%d x %d) isn't double the %s movie size (%d x %d) "
          "yet you have the alternate scan lines option on, "
          "at offset %d in DataID $%08X.  Will turn the option off.\r\n",
          (int) (Runtime2DInfoPntr->boundingBox.right - Runtime2DInfoPntr->boundingBox.left),
          (int) (Runtime2DInfoPntr->boundingBox.bottom - Runtime2DInfoPntr->boundingBox.top),
          FileName1,
          (int) RuntimeInfoPntr->streamSpecificData.videoStream->videoRectangle.right,
          (int) RuntimeInfoPntr->streamSpecificData.videoStream->videoRectangle.bottom,
          (int) LE_CHUNK_GetCurrentFilePosition (StreamID), (int) DataID,
          (int) Dimensionality);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          RuntimeInfoPntr->streamSpecificData.videoStream->
            doubleAlternateLines = FALSE;
        }
      }
    }
    else // Make our own bounding box based on the movie size.
    {
      if (RuntimeInfoPntr->streamSpecificData.videoStream->enableVideo)
      {
        Runtime2DInfoPntr->boundingBox =
        RuntimeInfoPntr->streamSpecificData.videoStream->videoRectangle;

        if (RuntimeInfoPntr->streamSpecificData.videoStream->
        doubleAlternateLines)
        {
          // Screen footprint is twice as big as the video in double mode.

          Runtime2DInfoPntr->boundingBox.right *= 2;
          Runtime2DInfoPntr->boundingBox.bottom *= 2;
        }
      }
      else // Audio only stream.  Give it a very small bounding box.
      {
        Runtime2DInfoPntr->boundingBox.left = 0;
        Runtime2DInfoPntr->boundingBox.top = 0;
        Runtime2DInfoPntr->boundingBox.bottom = 1;
        Runtime2DInfoPntr->boundingBox.right = 1;
      }
      FoundBoundingBoxData = TRUE;
    }

    if (!GlueOpenVideoStream (RuntimeInfoPntr, FileName1,
    (Dimensionality == 2)
    ? &Runtime2DInfoPntr->boundingBox
    : &RuntimeInfoPntr->streamSpecificData.videoStream->videoRectangle))
      goto ErrorExit;
    break;


  case LE_CHUNK_ID_SEQ_CAMERA: // Get camera label number & scale factor.
    if (Dimensionality != 2 && Dimensionality != 3)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
      "Camera chunk at offset %d in DataID $%08X is %d dimensional, "
      "we can only handle 2 or 3 dimensional cameras.\r\n",
      (int) LE_CHUNK_GetCurrentFilePosition (StreamID), (int) DataID,
      (int) Dimensionality);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit;
    }
    RuntimeInfoPntr->labelNumber =
      ((LE_SEQNCR_SequenceCameraChunkPointer) VoidPntr)->cameraLabel;

    // Load up near and far clip planes into auxiliary camera stream data,
    // which only exists for 3D cameras.

    if (RuntimeInfoPntr->streamSpecificData.cameraData != NULL)
    {
      RuntimeInfoPntr->streamSpecificData.cameraData->nearClipPlaneDistance =
        ((LE_SEQNCR_SequenceCameraChunkPointer) VoidPntr)->nearClipPlaneDistance;
      RuntimeInfoPntr->streamSpecificData.cameraData->farClipPlaneDistance =
        ((LE_SEQNCR_SequenceCameraChunkPointer) VoidPntr)->farClipPlaneDistance;
    }

    // Make a small bounding box if there isn't one.  This is really only used
    // for showing the camera on the screen when it is selected.

    if (!FoundBoundingBoxData)
    {
      if (Dimensionality == 2)
      {
        Runtime2DInfoPntr->boundingBox.left = -5;
        Runtime2DInfoPntr->boundingBox.top = -5;
        Runtime2DInfoPntr->boundingBox.bottom = 5;
        Runtime2DInfoPntr->boundingBox.right = 5;
      }
      else
      {
        RadiusPositive = 5.0F;
        RadiusNegative = -RadiusPositive;

        for (i = 0; i < 8; i++)
        {
          Temp3DPoint.x = (i & 1) ? RadiusPositive : RadiusNegative;
          Temp3DPoint.y = (i & 2) ? RadiusPositive : RadiusNegative;
          Temp3DPoint.z = (i & 4) ? RadiusPositive : RadiusNegative;
          Runtime3DInfoPntr->boundingBox[i] = Temp3DPoint;
        }
      }
      FoundBoundingBoxData = TRUE;
    }
    break;


  case LE_CHUNK_ID_SEQ_PRELOADER:
    break;


  case LE_CHUNK_ID_SEQ_TWEEKER:
#if CE_ARTLIB_EnableDebugMode
    if (Dimensionality != 0 || TypeOfDataID != LE_DATA_DataChunky)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
      "Tweeker chunk needs to be zero dimensional and of a chunky data type.\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      goto ErrorExit;
    }
#endif

    RuntimeInfoPntr->aux.tweeker.interpolationType = (LE_SEQNCR_InterpolationTypes)
      ((LE_SEQNCR_SequenceTweekerChunkPointer) VoidPntr)->interpolationType;

    RuntimeInfoPntr->streamSpecificData.tweekData->currentProportion =
      -12345.0; // To force a recalculation when it first starts up.

    // Figure out what we are interpolating by looking for the private
    // sequence subchunks (except for dimensionality which we can't
    // interpolate).  Also store the positions where the subchunks were
    // found for later easy access.

    LE_CHUNK_SetCurrentFilePosition (StreamID,
      RuntimeInfoPntr->dataOffset + sizeof (LE_CHUNK_HeaderRecord) +
      sizeof (LE_SEQNCR_SequenceTweekerChunkRecord));

    while (RuntimeInfoPntr->streamSpecificData.tweekData->offsetToFinalDataChunk == 0)
    {
      ChunkType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, NULL);
      if (ChunkType == LE_CHUNK_ID_NULL_STANDARD)
        break; // Reached end of enclosing parent chunk.

      // Lock onto the first acceptable subchunk type we find.

      if (RuntimeInfoPntr->aux.tweeker.interpolatedChunkID == 0 &&
      ChunkType > (LE_CHUNK_ID) LE_SEQNCR_CHUNK_ID_DIMENSIONALITY &&
      ChunkType < (LE_CHUNK_ID) LE_SEQNCR_CHUNK_ID_MAX_PRIVATE_CHUNKS)
        RuntimeInfoPntr->aux.tweeker.interpolatedChunkID =
        (LE_SEQNCR_PrivateChunkIDs) ChunkType;

      // If this is the kind of subchunk being processed, record its
      // position in the DataID for later use.

      if (ChunkType == RuntimeInfoPntr->aux.tweeker.interpolatedChunkID)
      {
        if (RuntimeInfoPntr->streamSpecificData.tweekData->offsetToInitialDataChunk == 0)
          RuntimeInfoPntr->streamSpecificData.tweekData->offsetToInitialDataChunk =
          LE_CHUNK_GetCurrentFilePosition (StreamID);
        else if (RuntimeInfoPntr->streamSpecificData.tweekData->offsetToFinalDataChunk == 0)
          RuntimeInfoPntr->streamSpecificData.tweekData->offsetToFinalDataChunk =
          LE_CHUNK_GetCurrentFilePosition (StreamID);
      }

      LE_CHUNK_Ascend (StreamID);
    }
    break;

  } // End switch on sequence type for header processing.

  // Allow the start sequence command to override the label, now that
  // the default value has been read in for the camera object and others.

  if (CommandFlags.asFlags.SEQFLAG_ByteSizedValue != 0)
    RuntimeInfoPntr->labelNumber = (UNS8)
    CommandFlags.asFlags.SEQFLAG_ByteSizedValue;

  // Call the start up sequence function for all relevant render slots.
  // Note that the position and other runtime info haven't been calculated
  // yet, so that this is more of an opportunity for the render slots to
  // allocate their own internal storage.  They return FALSE if they fail.

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
  {
    if (RuntimeInfoPntr->renderSlotSet & (1 << RenderSlot))
    {
      RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];

      if (RenderSlotPntr != NULL)
      {
        if (!RenderSlotPntr->SequenceStartUp (RuntimeInfoPntr))
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
            "Render slot #%d doesn't want to start sequence at offset %d in DataID $%08X.\r\n",
            (int) RenderSlot, (int) DataOffset, (int) DataID);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
          goto ErrorExit;
        }
      }
    }
  }

  // Ok, looks like we have a winner.  Add the new sequence
  // to the runtime sequence info tree.

  LI_SEQNCR_InsertRuntimeChild (RuntimeInfoPntr, ParentPntr);

  // Add it to the labeled objects if it has a label.  If there
  // is an existing object with a label, it will take over the
  // use of that label (presumably you are more interested in
  // things which have started recently).  This also removes some
  // ambiguity with a non-overlapping sequence of items with the
  // same label, where the previous one would end at the same time
  // as the next one starts.

  if (RuntimeInfoPntr->labelNumber > 0 &&
  RuntimeInfoPntr->labelNumber < CE_ARTLIB_SeqncrMaxLabels)
    LE_SEQNCR_LabelArray [RuntimeInfoPntr->labelNumber] = RuntimeInfoPntr;

  if (RuntimeInfoPntr->labelNumber > 0)
    LE_UIMSG_SendEvent (UIMSG_SEQUENCE_STARTED,
    RuntimeInfoPntr->dataID, RuntimeInfoPntr->priority,
    RuntimeInfoPntr->labelNumber, RuntimeInfoPntr->sequenceType,
    TopLevelClock, NULL, 0);

  // Check for a sequence watch that is watching this DataID/Priority,
  // but check the data offset so that we don't accidentally end up
  // watching nested subsequences.

  if (RuntimeInfoPntr->dataOffset == 0)
  {
    for (i = 0; i < CE_ARTLIB_SeqncrMaxWatches; i++)
    {
      if (RuntimeInfoPntr->dataID == LE_SEQNCR_WatchArray[i].dataID &&
      RuntimeInfoPntr->priority == LE_SEQNCR_WatchArray[i].priority)
      {
        RuntimeInfoPntr->watchIndex = (INT8) i;
        break;
      }
    }
  }

  // Add a reference to the child sequence data if it is being used,
  // so that it doesn't get unloaded and reloaded uselessly, and is
  // faster to access (not in least recently used list).

  if (RuntimeInfoPntr->childSequencesDataID != LE_DATA_EmptyItem)
    LE_DATA_AddRef (RuntimeInfoPntr->childSequencesDataID);

  // Mark all parents as needing a re-evaluation, so that our new sequence's
  // clock will get updated right away, not when the parent's clock next expires.

  MarkAncestorsAsNeedingReEvaluation (RuntimeInfoPntr);

  // Notify the user that it has started, if requested.

  if (RuntimeInfoPntr->watchIndex >= 0)
  {
    LE_SEQNCR_WatchArray[RuntimeInfoPntr->watchIndex].hasStarted = TRUE;
    LE_SEQNCR_WatchArray[RuntimeInfoPntr->watchIndex].updateCount++;
  }

  if (RuntimeInfoPntr->callBack != NULL)
    RuntimeInfoPntr->callBack (RuntimeInfoPntr, LE_SEQNCR_CALLOP_STARTING);

  goto NormalExit;


BadChunkExit: // Have read a bad chunk.  Put up an error message and exit.
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_StartUpSequence: "
  "Error reading chunk data contents at offset %d in DataID $%08X.\r\n",
  (int) LE_CHUNK_GetCurrentFilePosition (StreamID), (int) DataID);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

ErrorExit: // Undo partial allocations.
  if (RuntimeInfoPntr != NULL)
  {
    // Remove the attached dimension specific records.
    if (RuntimeInfoPntr->dimensionality == 2)
      LI_SEQNCR_FreeRuntime2DInfo (RuntimeInfoPntr->dimensionSpecificData.d2);
    else if (RuntimeInfoPntr->dimensionality == 3)
      LI_SEQNCR_FreeRuntime3DInfo (RuntimeInfoPntr->dimensionSpecificData.d3);
    RuntimeInfoPntr->dimensionSpecificData.d0 = NULL;

    // Deallocate attached audio/video/etc stream record.
    if (RuntimeInfoPntr->streamSpecificData.voidPntr != NULL)
    {
      LI_MEMORY_DeallocLibraryBlock (RuntimeInfoPntr->streamSpecificData.voidPntr);
      RuntimeInfoPntr->streamSpecificData.voidPntr = NULL;
    }

    // Get rid of the new runtime info.
    LI_SEQNCR_FreeRuntimeInfo (RuntimeInfoPntr);
    RuntimeInfoPntr = NULL;
  }

NormalExit:
  if (StreamID >= 0)
    LE_CHUNK_CloseReadStream (StreamID);

  return RuntimeInfoPntr;
}



/*****************************************************************************
 * Deallocates all things attached to the given sequence record.  Also
 * updates renderers that need to know about things going away,  Doesn't
 * remove the record from the tree or even look at its children.
 */

static void LI_SEQNCR_ShutDownSequence (LE_SEQNCR_RuntimeInfoPointer ShutDownPntr)
{
  LE_REND_RenderSlot                RenderSlot;
  LI_REND_AbstractRenderSlotPointer RenderSlotPntr;

  if (ShutDownPntr == NULL)
    return;

  // First inform the user that it is being deleted.

  if (ShutDownPntr->watchIndex >= 0)
  {
    if (LE_SEQNCR_WatchArray[ShutDownPntr->watchIndex].dataID !=
    ShutDownPntr->dataID ||
    LE_SEQNCR_WatchArray[ShutDownPntr->watchIndex].priority !=
    ShutDownPntr->priority)
      ShutDownPntr->watchIndex = -1; // Watching someone else now.
    else
    {
      LE_SEQNCR_WatchArray[ShutDownPntr->watchIndex].wasDeleted = TRUE;
      LE_SEQNCR_WatchArray[ShutDownPntr->watchIndex].updateCount++;
    }
  }

  if (ShutDownPntr->callBack != NULL)
    ShutDownPntr->callBack (ShutDownPntr, LE_SEQNCR_CALLOP_DELETING);

  // Remove it from the labeled objects if it has a label, and the
  // label is for the sequence being removed (a later one with the
  // same label could have started up, and we don't want to unlabel
  // it by mistake).

  if (ShutDownPntr->labelNumber > 0 &&
  ShutDownPntr->labelNumber < CE_ARTLIB_SeqncrMaxLabels)
  {
    if (LE_SEQNCR_LabelArray [ShutDownPntr->labelNumber] == ShutDownPntr)
      LE_SEQNCR_LabelArray [ShutDownPntr->labelNumber] = NULL;
  }

  if (ShutDownPntr->labelNumber > 0)
    LE_UIMSG_SendEvent (UIMSG_SEQUENCE_DELETED,
    ShutDownPntr->dataID, ShutDownPntr->priority,
    ShutDownPntr->labelNumber, ShutDownPntr->sequenceType,
    TopLevelClock, NULL, 0);

  // Call the shut down sequence function for all relevant render slots.
  // For 2D ones this will mark the position of a bitmap sequence as needing
  // a redraw in the next update.  3D ones don't have to do much since they
  // redraw everything every frame.  Sound renderers can kill the sound.

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
  {
    if (ShutDownPntr->renderSlotSet & (1 << RenderSlot))
    {
      RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];

      if (RenderSlotPntr != NULL)
        RenderSlotPntr->SequenceShutDown (ShutDownPntr);
    }
  }

  // Close attached streams, the stream record gets a bit deallocated later.

  if (ShutDownPntr->streamSpecificData.voidPntr != NULL)
  {
    if (ShutDownPntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
      GlueCloseVideoStream (ShutDownPntr);
    else if (ShutDownPntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND)
      LE_SOUND_CloseAudioStream (ShutDownPntr->streamSpecificData.audioStream);
    else if (ShutDownPntr->sequenceType == LE_CHUNK_ID_SEQ_TWEEKER)
      ; // Nothing to do, and would interfere with next tweeker too.
    else if (ShutDownPntr->sequenceType == LE_CHUNK_ID_SEQ_CAMERA)
      ; // Nothing to do, extra data just contains clip plane distances.
    else // Unknown stream type.
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_ShutDownSequence: "
        "Trying to shut down an unknown stream type for sequence type %d, "
        "which is at offset %d in DataID $%08X.\r\n",
        (int) ShutDownPntr->sequenceType,
        (int) ShutDownPntr->dataOffset, (int) ShutDownPntr->dataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    }
  }

  // Unload data and optionally unreference it too, if requested.

  if (ShutDownPntr->childSequencesDataID != LE_DATA_EmptyItem)
    LE_DATA_RemoveRef (ShutDownPntr->childSequencesDataID);

  if (ShutDownPntr->unrefSequenceDataWhenDone ||
  ShutDownPntr->disposeSequenceDataWhenDone != LE_SEQNCR_DATA_DISPOSE_DO_NOTHING)
  {
    LE_SEQNCR_CleanUpSequenceData (
      (ShutDownPntr->disposeSequenceDataWhenDone ==
      LE_SEQNCR_DATA_DISPOSE_DEALLOCATE_CONTENTSDATAID) ?
      ShutDownPntr->contentsDataID : ShutDownPntr->dataID,
      ShutDownPntr->unrefSequenceDataWhenDone,
      (LE_SEQNCR_DataDisposalOptions) ShutDownPntr->disposeSequenceDataWhenDone);
  }

  // Deallocate attached runtime info records that contain position,
  // video streams, etc.

  if (ShutDownPntr->dimensionality == 2)
    LI_SEQNCR_FreeRuntime2DInfo (ShutDownPntr->dimensionSpecificData.d2);
  else if (ShutDownPntr->dimensionality == 3)
    LI_SEQNCR_FreeRuntime3DInfo (ShutDownPntr->dimensionSpecificData.d3);
  ShutDownPntr->dimensionSpecificData.d0 = NULL;

  if (ShutDownPntr->streamSpecificData.voidPntr != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (ShutDownPntr->streamSpecificData.voidPntr);
    ShutDownPntr->streamSpecificData.voidPntr = NULL;
  }
}



/*****************************************************************************
 * Shuts down and deallocates the given runtime info node and all its
 * children.  Does the children first and works up to the given node.
 */

static void DeleteSequenceAndChildren (LE_SEQNCR_RuntimeInfoPointer DeletePntr)
{
  if (DeletePntr == NULL)
    return;

  // Recursively delete all children until they are all gone.

  while (DeletePntr->child != NULL)
    DeleteSequenceAndChildren (DeletePntr->child);

  // Remove and deallocate this sequence.

  LI_SEQNCR_ShutDownSequence (DeletePntr);
  LI_SEQNCR_RemoveRuntimeNodeFromTree (DeletePntr);
  LI_SEQNCR_FreeRuntimeInfo (DeletePntr);
}



/*****************************************************************************
 * Just delete the children of the given sequence.
 */

static void DeleteJustChildren (LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  if (SequencePntr == NULL)
    return;

  while (SequencePntr->child != NULL)
    DeleteSequenceAndChildren (SequencePntr->child);
}



/*****************************************************************************
 * Goes through all sequences (or just the top level ones) and stops their
 * execution (removes them from the tree).  Ignores chained commands (doesn't
 * bother looking for them).  All ones with the matching DataID/Priority,
 * even if there several with the same DataID/Priority (but not ones nested
 * inside) get removed.  Returns number of sequences deleted.
 */

static UNS32 DeleteMatchingSequences (LE_DATA_DataId DataID,
UNS16 Priority, BOOL SearchWholeTree)
{
  UNS32                         DeleteCount;
  LE_SEQNCR_RuntimeInfoPointer  DeletePntr;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

  SequencePntr = LI_SEQNCR_CommonRuntimeTreeRoot;
  if (SequencePntr == NULL)
    return 0; // System isn't open.

  // Always start the search with the children of the root, you can't
  // find the root node since it is special and shouldn't be tampered
  // with.

  SequencePntr = SequencePntr->child;
  if (SequencePntr == NULL)
    return 0; // No other nodes in the tree.

  DeleteCount = 0;

  if (SearchWholeTree)
  {
    while (SequencePntr != NULL)
    {
      // Do the current node.

      if (SequencePntr->dataID == DataID &&
      SequencePntr->priority == Priority &&
      SequencePntr->dataOffset == 0)
      {
        DeletePntr = SequencePntr;

        // Traverse to the next sibling before deleting the node,
        // don't need to traverse down to children since they will
        // go when the node is deleted.

        while (SequencePntr != NULL && SequencePntr->sibling == NULL)
          SequencePntr = SequencePntr->parent;
        if (SequencePntr != NULL)
          SequencePntr = SequencePntr->sibling;

        DeleteSequenceAndChildren (DeletePntr);
        DeleteCount++;
      }
      else
      {
        // Traverse down the tree, then do siblings,
        // then back up to the next parent's sibling.

        if (SequencePntr->child != NULL)
          SequencePntr = SequencePntr->child;
        else
        {
          while (SequencePntr != NULL && SequencePntr->sibling == NULL)
            SequencePntr = SequencePntr->parent;
          if (SequencePntr != NULL)
            SequencePntr = SequencePntr->sibling;
        }
      }
    }
  }
  else // Just doing children of root node.
  {
    while (SequencePntr != NULL)
    {
      // Do the current node.

      if (SequencePntr->dataID == DataID &&
      SequencePntr->priority == Priority &&
      SequencePntr->dataOffset == 0)
      {
        DeletePntr = SequencePntr;
        SequencePntr = SequencePntr->sibling;
        DeleteSequenceAndChildren (DeletePntr);
        DeleteCount++;
      }
      else
        SequencePntr = SequencePntr->sibling;
    }
  }
  return DeleteCount;
}



/*****************************************************************************
 * Remove all children of the given sequence that are in the future (their
 * start time > sequence's current clock).  This is used when a sequence
 * clock goes backwards (like when looping).  Returns TRUE if any were
 * removed.
 */

static BOOL LI_SEQNCR_DeleteFutureChildren (
LE_SEQNCR_RuntimeInfoPointer RetrochronPntr)
{
  LE_SEQNCR_RuntimeInfoPointer  CurrentChildPntr;
  LE_SEQNCR_RuntimeInfoPointer  NextChildPntr;
  BOOL                          ReturnCode = FALSE;

  if (RetrochronPntr == NULL)
    return ReturnCode;

  CurrentChildPntr = RetrochronPntr->child;
  while (CurrentChildPntr != NULL)
  {
    NextChildPntr = CurrentChildPntr->sibling; // Save pointer before it is deleted.
    if (CurrentChildPntr->parentStartTime > RetrochronPntr->sequenceClock)
    {
      DeleteSequenceAndChildren (CurrentChildPntr);
      ReturnCode = TRUE;
    }
    CurrentChildPntr = NextChildPntr;
  }

  return ReturnCode;
}



/*****************************************************************************
 * Add any child sequences which started between the last update and the
 * current one.  In other words, child time > OldClock and <= current.
 * But don't add ones which start and then die before the current time.
 * Find the child sequences in a DataId, either as nested subchunks or
 * as a separate bunch of subchunks.  Does nothing when the parent is
 * an off screen scrolling world.  Returns TRUE if any added.
 */

static BOOL LI_SEQNCR_AddNewlyBornChildren (
LE_SEQNCR_RuntimeInfoPointer SequencePntr, INT32 OldClock)
{
  BOOL                                ReturnCode = FALSE;
  LE_CHUNK_StreamID                   StreamID = -1;
  LE_DATA_DataId                      SubChunksDataID;
  LE_SEQNCR_SequenceChunkHeaderRecord SubSequenceHeader;
  LE_SEQNCR_SequenceType              SubSequenceType;

  if (SequencePntr == NULL)
    return FALSE;

  if (SequencePntr->scrollingWorld && SequencePntr->onScreen == 0)
    return FALSE; // An off screen scrolling world, no children eligible.

  SubChunksDataID = SequencePntr->childSequencesDataID;

#if CE_ARTLIB_EnableDebugMode
  if (SubChunksDataID == LE_DATA_EmptyItem)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_AddNewlyBornChildren: "
      "Called with a sequence which can't have children, "
      "callers should have checked before calling.  "
      "Sequence is at offset %d in DataID $%08X.\r\n",
      (int) SequencePntr->dataOffset, (int) SequencePntr->dataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE; // No subsequences defined.
  }
#endif

  StreamID = LE_CHUNK_ReadFromDataID (SubChunksDataID);
  if (StreamID < 0)
    goto ErrorExit;

  // The nested sequences need to descend into the parent sequence
  // chunk so that it picks up the end of the parent chunk as the
  // end of the subsequences.  Indirect sequences don't since their
  // end is at the end of the whole data item.

  if (SequencePntr->childSequencesDataID == SequencePntr->dataID)
  {
    LE_CHUNK_SetCurrentFilePosition (StreamID, SequencePntr->dataOffset);
    if (LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, NULL) ==
    LE_CHUNK_ID_NULL_STANDARD)
      goto ErrorExit;
  }

  if (!LE_CHUNK_SetCurrentFilePosition (StreamID,
  SequencePntr->childSequencesOffset))
    goto ErrorExit;

  // Go through all chunks starting at the position of the next possible
  // subchunk until we find one that is too far in the future or we
  // reach the end of the subchunks.  Advance the next possible subchunk
  // position past the end of chunks that are passed over as being too
  // old.

  while (TRUE)
  {
    SubSequenceType =
      LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, NULL);
    if (SubSequenceType == LE_CHUNK_ID_NULL_STANDARD)
      break; // No more chunks.

    // Is this a known sequence chunk type?  We just ignore unknown ones.

    if (SubSequenceType >= LE_CHUNK_ID_SEQ_GROUPING &&
    SubSequenceType < LE_CHUNK_ID_SEQ_MAX)
    {
      if (LE_CHUNK_ReadChunkData (StreamID, &SubSequenceHeader,
      sizeof (SubSequenceHeader)) != sizeof (SubSequenceHeader))
        goto ErrorExit;  // Unable to read the sequence header record.

      // Is this chunk in the future?

      if (SubSequenceHeader.parentStartTime > SequencePntr->sequenceClock)
        break; // Gone far enough, leave the next subchunk position here.

      if (SubSequenceHeader.parentStartTime > OldClock &&
      (SubSequenceHeader.endingAction != LE_SEQNCR_EndingActionStop ||
      SubSequenceHeader.endTime == 0 /* Infinite end time */ ||
      SubSequenceHeader.parentStartTime +
      SubSequenceHeader.endTime > SequencePntr->sequenceClock))
      {
        // This one is in the recent past, start time is <= SequenceClock
        // and start time > OldClock.  So start it up.  Well, maybe not
        // if it also ends before the start time.

        if (LI_SEQNCR_StartUpSequence (SubChunksDataID,
        SequencePntr->childSequencesOffset, NULL, SequencePntr) != NULL)
          ReturnCode = TRUE; // Yes, a new child has been born.
      }
    }
    LE_CHUNK_Ascend (StreamID);

    // Advance the next subchunk to look at position to be just after the
    // subchunk we finished processing.  This is of course where the next
    // chunk starts, whatever it is, or is at the end of the chunks.

    SequencePntr->childSequencesOffset =
      LE_CHUNK_GetCurrentFilePosition (StreamID);
  }
  goto NormalExit;


ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LI_SEQNCR_AddNewlyBornChildren: "
    "Error while examining subchunks supposedly at offset %d "
    "in stream with DataID $%08X.\r\n",
    (int) SequencePntr->childSequencesOffset, (int) SubChunksDataID);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:
  if (StreamID >= 0)
    LE_CHUNK_CloseReadStream (StreamID);

  return ReturnCode;
}



/*****************************************************************************
 * Resets the given sequence's clock back to the given time (usually zero).
 * All children get removed and then relevant ones get added.  Children's
 * clocks recursively get set to match the given time.  Subchunk pointers
 * get set back to the beginning (so that it starts scanning for subsequences
 * starting at earlier times).  All changed sequences get marked as needing
 * a reevaluation.
 */

static void LI_SEQNCR_GoBackwardsInTime (
LE_SEQNCR_RuntimeInfoPointer SequencePntr, INT32 NewTime)
{
  LE_SOUND_AudioStreamPointer       AudioPntr;
  LE_SEQNCR_RuntimeInfoPointer      CurrentChildPntr;
  LE_SEQNCR_RuntimeInfoPointer      NextChildPntr;
  INT32                             ParentClock;

  if (SequencePntr == NULL)
    return; // Safety precaution.

  if (NewTime < 0 ||
  (NewTime >= SequencePntr->endTime &&
  SequencePntr->endingAction == LE_SEQNCR_EndingActionStop))
  {
    // Self destruct, we are past our ending time or before
    // our starting time.

    DeleteSequenceAndChildren (SequencePntr);
    return;
  }

  if (SequencePntr->parent == NULL)
    ParentClock = TopLevelClock;
  else
    ParentClock = SequencePntr->parent->sequenceClock;
  SequencePntr->parentTimeOfLastUpdate = ParentClock;

  // Set the sequence's clock to the new time, modulo its ending action.

  if (SequencePntr->endingAction == LE_SEQNCR_EndingActionLoopToBeginning)
    NewTime = labs (NewTime) % SequencePntr->endTime;
  else if (SequencePntr->endingAction == LE_SEQNCR_EndingActionStayAtEnd)
  {
    if (NewTime > SequencePntr->endTime)
      NewTime = SequencePntr->endTime;
  }

  SequencePntr->sequenceClock = NewTime;

  // First set the subchunk data offset back to the start of
  // the subchunks, if relevant.

  if (SequencePntr->childSequencesDataID != LE_DATA_EmptyItem)
  {
    if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_INDIRECT)
      SequencePntr->childSequencesOffset = 0;
    else
    {
      // Subchunks are nested inside the parent chunk.

      SequencePntr->childSequencesOffset =
        (SequencePntr->dataOffset +
        LE_CHUNK_ChunkFixedDataSizes [SequencePntr->sequenceType] +
        sizeof (LE_CHUNK_HeaderRecord));
    }
  }

  // Delete all children and add back only the ones which exist
  // at the current time (requires a scan from the distant past
  // to the current time, which can be slow).

  DeleteJustChildren (SequencePntr);

  if (SequencePntr->childSequencesDataID != LE_DATA_EmptyItem)
    LI_SEQNCR_AddNewlyBornChildren (SequencePntr, INT32_MIN);

  // If this is a video sequence, we have to rewind the
  // streams, if any (the video is keyed off the audio
  // if there is audio).

  if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
    GlueJumpVideoStream (SequencePntr, NewTime);

  // Same for pure sound sequences.  Need to restart them from
  // the given time.  Set jump point to force an immediate jump
  // from the current position to the desired time.

  if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND &&
  SequencePntr->streamSpecificData.audioStream != NULL)
  {
    AudioPntr = SequencePntr->streamSpecificData.audioStream;
    AudioPntr->jumpTriggerPosition = AudioPntr->currentPosition;
    AudioPntr->jumpToPosition =
      (INT32) ((AudioPntr->endPosition - AudioPntr->startPosition) *
      (double) NewTime / SequencePntr->endTime) + AudioPntr->startPosition;
    // Note that you shouldn't jump exactly to the end since that
    // will result in no feeding and the audio system won't
    // realise it has finished playing the sound.
    if (AudioPntr->jumpToPosition >= AudioPntr->endPosition)
      AudioPntr->jumpToPosition = AudioPntr->endPosition - 1;
    if (AudioPntr->jumpToPosition < AudioPntr->startPosition)
      AudioPntr->jumpToPosition = AudioPntr->startPosition;
    AudioPntr->jumpCount = 1;
    AudioPntr->jumpCutRequested = TRUE;
  }

  // Mark things as needing re-evaluation.  Children take care
  // of themselves later on in the recursion.

  MarkAncestorsAsNeedingReEvaluation (SequencePntr);
  SequencePntr->needsReEvaluation = TRUE;

  // Now recursively update the children's clocks to be the new time.
  // They can also turn on their re-evaluation flags if they need to,
  // and self destruct if their new clock is past their end time.

  CurrentChildPntr = SequencePntr->child;
  while (CurrentChildPntr != NULL)
  {
    NextChildPntr = CurrentChildPntr->sibling;

    LI_SEQNCR_GoBackwardsInTime (CurrentChildPntr,
      NewTime - CurrentChildPntr->parentStartTime);

    CurrentChildPntr = NextChildPntr;
  }
}



/*****************************************************************************
 * Update the position (sequence to world transformation) of a sequence.
 * Note that the needsPositionRecalc flag is updated elsewhere, not here.
 *
 * The world transform is composed of the parent's world transform with
 * the movements of the sequence added in.  The actual transformation order
 * is tweeker effects, sequence to parent, parent's world.
 */

static void UpdateSequenceToWorldTransformationAndInformRenderSlotsOfMovement (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  LE_REND_RenderSlotSet             OffMask;
  LE_REND_RenderSlotSet             OnMask;
  BOOL                              OnScreen;
  LE_REND_RenderSlot                RenderSlot;
  LI_REND_AbstractRenderSlotPointer RenderSlotPntr;
  TYPE_Matrix2D                     Temp2DMatrix;
  TYPE_Matrix3D                     Temp3DMatrix;

  // Update the sequence to world transformation.

  if (SequencePntr->parent != NULL &&
  SequencePntr->parent->dimensionality == SequencePntr->dimensionality)
  {
    if (SequencePntr->dimensionality == 2)
    {
      if (SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformNonIdentity)
      {
        if (SequencePntr->dimensionSpecificData.d2->tweekerTransformNonIdentity)
        {
          // Need to do the full multiplication, no identity matrices at all.

          LE_MATRIX_2D_MatrixMultiply (
            &SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D,
            &SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D,
            &Temp2DMatrix);

          LE_MATRIX_2D_MatrixMultiply (
            &Temp2DMatrix,
            &SequencePntr->parent->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D,
            &SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D);
        }
        else // Only sequenceToParent is non-identity and needs multiplying.
        {
          LE_MATRIX_2D_MatrixMultiply (
            &SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D,
            &SequencePntr->parent->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D,
            &SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D);
        }
      }
      else // SequenceToParent is an identity matrix.
      {
        if (SequencePntr->dimensionSpecificData.d2->tweekerTransformNonIdentity)
        {
          // Only tweeker is non-identity and needs multiplying.

          LE_MATRIX_2D_MatrixMultiply (
            &SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D,
            &SequencePntr->parent->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D,
            &SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D);
        }
        else // All matrices are identity matrices.
        {
          SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D =
          SequencePntr->parent->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D;
        }
      }
    }
    else if (SequencePntr->dimensionality == 3)
    {
      if (SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformNonIdentity)
      {
        if (SequencePntr->dimensionSpecificData.d3->tweekerTransformNonIdentity)
        {
          // Need to do the full multiplication, no identity matrices at all.

          LE_MATRIX_3D_MatrixMultiply (
            &SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D,
            &SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D,
            &Temp3DMatrix);

          LE_MATRIX_3D_MatrixMultiply (
            &Temp3DMatrix,
            &SequencePntr->parent->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D,
            &SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D);
        }
        else // Only sequenceToParent is non-identity and needs multiplying.
        {
          LE_MATRIX_3D_MatrixMultiply (
            &SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D,
            &SequencePntr->parent->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D,
            &SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D);
        }
      }
      else // SequenceToParent is an identity matrix.
      {
        if (SequencePntr->dimensionSpecificData.d3->tweekerTransformNonIdentity)
        {
          // Only tweeker is non-identity and needs multiplying.

          LE_MATRIX_3D_MatrixMultiply (
            &SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D,
            &SequencePntr->parent->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D,
            &SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D);
        }
        else // All matrices are identity matrices.
        {
          SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D =
          SequencePntr->parent->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D;
        }
      }
    }
  }
  else // No parent or one with a different dimensionality.
  {
    // Pretend that parent matrix is an identity matrix.  This results
    // in sequence to world being tweeker combined with sequence to parent.

    if (SequencePntr->dimensionality == 2)
    {
      if (SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformNonIdentity)
      {
        if (SequencePntr->dimensionSpecificData.d2->tweekerTransformNonIdentity)
        {
          // All matrices are non-identity.

          LE_MATRIX_2D_MatrixMultiply (
            &SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D,
            &SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D,
            &SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D);
        }
        else // Only sequence to parent is non-identity.
        {
          SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D =
            SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D;
        }
      }
      else // Sequence to parent is an identity matrix.
      {
        if (SequencePntr->dimensionSpecificData.d2->tweekerTransformNonIdentity)
        {
          // Only tweeker is non-identity.

          SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D =
            SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D;
        }
        else // Everything is an identity matrix.
        {
          LE_MATRIX_2D_SetIdentity (&SequencePntr->dimensionSpecificData.d2->
            sequenceToWorldTransformation.matrix2D);
        }
      }
    }
    else if (SequencePntr->dimensionality == 3)
    {
      if (SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformNonIdentity)
      {
        if (SequencePntr->dimensionSpecificData.d3->tweekerTransformNonIdentity)
        {
          // All matrices are non-identity.

          LE_MATRIX_3D_MatrixMultiply (
            &SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D,
            &SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D,
            &SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D);
        }
        else // Only sequence to parent is non-identity.
        {
          SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D =
            SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D;
        }
      }
      else // Sequence to parent is an identity matrix.
      {
        if (SequencePntr->dimensionSpecificData.d3->tweekerTransformNonIdentity)
        {
          // Only tweeker is non-identity.

          SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D =
            SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D;
        }
        else // Everything is an identity matrix.
        {
          LE_MATRIX_3D_SetIdentity (&SequencePntr->dimensionSpecificData.d3->
            sequenceToWorldTransformation.matrix3D);
        }
      }
    }
  }

  // Let the render slots know that this sequence has moved.  For 2D the
  // slot can update its invalid rectangle areas.  This also updates our
  // onScreen flags.

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
  {
    OnMask = (LE_REND_RenderSlotSet) (1 << RenderSlot);
    if (SequencePntr->renderSlotSet & OnMask)
    {
      RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];

      if (RenderSlotPntr != NULL)
      {
        OnScreen = RenderSlotPntr->SequenceMoved (SequencePntr);

        // Update the bit in the onScreen set to show if this render slot
        // thinks it is on or off screen.

        OffMask = (LE_REND_RenderSlotSet) ~OnMask;
        SequencePntr->onScreen &= OffMask;
        if (OnScreen)
          SequencePntr->onScreen |= OnMask;
      }
    }
  }
}



/*****************************************************************************
 * Moves the sequence to the position given by the matrix, if NULL then it
 * gets moved to the origin.  If the matrix is the same as the current one
 * then nothing is done, to avoid doing a full redraw of the object.
 */

static void MoveSequence (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
void *MatrixPntr, UNS16 MatrixByteSize)
{
  if (SequencePntr == NULL)
    return; // Nothing to move.

  if (SequencePntr->dimensionality == 2)
  {
    if (MatrixPntr == NULL || MatrixByteSize != sizeof (TYPE_Matrix2D))
    {
      LE_MATRIX_2D_SetIdentity (&SequencePntr->dimensionSpecificData.d2->
        sequenceToParentTransformation.matrix2D);
      SequencePntr->dimensionSpecificData.d2->
        sequenceToParentTransformNonIdentity = FALSE;
    }
    else
    {
      if (0 == memcmp (&SequencePntr->dimensionSpecificData.d2->
      sequenceToParentTransformation.matrix2D, MatrixPntr, sizeof (TYPE_Matrix2D)))
        return; // Not a new position.  Avoid redundant update.

      SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.
        matrix2D = *((TYPE_Matrix2DPointer) MatrixPntr);
      SequencePntr->dimensionSpecificData.d2->
        sequenceToParentTransformNonIdentity = TRUE;
    }
  }
  else if (SequencePntr->dimensionality == 3)
  {
    if (MatrixPntr == NULL || MatrixByteSize != sizeof (TYPE_Matrix3D))
    {
      LE_MATRIX_3D_SetIdentity (&SequencePntr->dimensionSpecificData.d3->
        sequenceToParentTransformation.matrix3D);
      SequencePntr->dimensionSpecificData.d3->
        sequenceToParentTransformNonIdentity = FALSE;
    }
    else
    {
      if (0 == memcmp (&SequencePntr->dimensionSpecificData.d3->
      sequenceToParentTransformation.matrix3D, MatrixPntr, sizeof (TYPE_Matrix3D)))
        return; // Not a new position.  Avoid redundant update.

      SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.
        matrix3D = *((TYPE_Matrix3DPointer) MatrixPntr);
      SequencePntr->dimensionSpecificData.d3->
        sequenceToParentTransformNonIdentity = TRUE;
    }
  }

  MarkAsNeedingPositionRecalc (SequencePntr);
}



/*****************************************************************************
 * Some helper functions for doing interpolation.  They return number A if
 * the proportion is 0.0 and blend towards number B as the proportion goes
 * up to 1.0.  Numbers beyond that range do extrapolation.
 */

inline int IntegerInterpolation (float Proportion, int IntA, int IntB)
{
  return (int) ((1.0F - Proportion) * (float) IntA + Proportion * (float) IntB);
}

inline float FloatInterpolation (float Proportion, float FloatA, float FloatB)
{
  return ((1.0F - Proportion) * FloatA + Proportion * FloatB);
}



/*****************************************************************************
 * Update the tweeked information in the parent sequence using this sequence's
 * current clock (already updated) to drive it.  Set the needsPositionRecalc
 * in the parent if it has changed the position or set the needsRedraw in the
 * parent if the parent sequence needs to be redrawn for some reason.  Also
 * needs to set the parent's tweekerTransformNonIdentity if it specifies a
 * tweeked position matrix in the parent's tweekerTransformation.
 */

static void DoTweeking (LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  BYTE                               *BasePntr;
  LE_SOUND_BufSndHandle               BufSndPntr;
  UNS32                               DataOffset;
  UNS8                                Dimensionality;
  LE_SEQNCR_FieldOfViewChunkPointer   FieldOfViewA;
  LE_SEQNCR_FieldOfViewChunkPointer   FieldOfViewB;
  TYPE_Matrix2DPointer                Matrix2DPntr;
  TYPE_Matrix3DPointer                Matrix3DPntr;
  LE_SEQNCR_3DMeshChoiceChunkPointer  MeshChoiceA;
  LE_SEQNCR_3DMeshChoiceChunkPointer  MeshChoiceB;
  LE_SEQNCR_2DOffsetChunkPointer      Offset2DA;
  LE_SEQNCR_2DOffsetChunkPointer      Offset2DB;
  LE_SEQNCR_3DOffsetChunkPointer      Offset3DA;
  LE_SEQNCR_3DOffsetChunkPointer      Offset3DB;
  LE_SEQNCR_RuntimeInfoPointer        ParentPntr;
  TYPE_Angle3D                        Pitch;
  TYPE_Point2D                        Point2DA;
  TYPE_Point2D                        Point2DB;
  TYPE_Point3D                        Point3DA;
  TYPE_Point3D                        Point3DB;
  TYPE_Scale3D                        Proportion;
  TYPE_Angle3D                        Roll;
  float                               ScaleX;
  float                               ScaleY;
  float                               ScaleZ;
  LE_SEQNCR_SoundPanningChunkPointer  SoundPanningA;
  LE_SEQNCR_SoundPanningChunkPointer  SoundPanningB;
  LE_SEQNCR_SoundPitchChunkPointer    SoundPitchA;
  LE_SEQNCR_SoundPitchChunkPointer    SoundPitchB;
  LE_SEQNCR_SoundVolumeChunkPointer   SoundVolumeA;
  LE_SEQNCR_SoundVolumeChunkPointer   SoundVolumeB;
  LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer OSRT2DA;
  LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer OSRT2DB;
  LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer OSRT3DA;
  LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer OSRT3DB;
  LE_SEQNCR_RuntimeTweekerInfoPointer TweekData;
  LE_SEQNCR_2DXformChunkPointer       XForm2DA;
  LE_SEQNCR_2DXformChunkPointer       XForm2DB;
  LE_SEQNCR_3DXformChunkPointer       XForm3DA;
  LE_SEQNCR_3DXformChunkPointer       XForm3DB;
  TYPE_Angle3D                        Yaw;

  // Check for bad arguments.

#if CE_ARTLIB_EnableDebugMode
  if (SequencePntr == NULL || SequencePntr->parent == NULL ||
  SequencePntr->streamSpecificData.tweekData == NULL ||
  SequencePntr->sequenceType != LE_CHUNK_ID_SEQ_TWEEKER)
  {
    LE_ERROR_DebugMsg ("L_SEQNCR.cpp DoTweeking: Bad input arguments.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
    return;
  }
#endif

  ParentPntr = SequencePntr->parent;

  // The identity type of tweeker removes the tweek modifications from the
  // parent sequence.  It just sets the parent's tweeker transformation
  // to the identity (do nothing) transformation.  Doesn't work for tweeking
  // non-transform things (like sound volume or movie brightness).

  if (SequencePntr->aux.tweeker.interpolationType ==
  LE_SEQNCR_INTERPOLATION_IDENTITY)
  {
    Dimensionality = ParentPntr->dimensionality;

    if (Dimensionality == 2)
    {
      if (ParentPntr->dimensionSpecificData.d2->tweekerTransformNonIdentity)
      {
        #if CE_ARTLIB_EnableDebugMode
        LE_MATRIX_2D_SetZero (&ParentPntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D);
        #endif
        ParentPntr->dimensionSpecificData.d2->tweekerTransformNonIdentity = FALSE;
        ParentPntr->needsPositionRecalc = TRUE;
      }
    }
    else if (Dimensionality == 3)
    {
      if (ParentPntr->dimensionSpecificData.d3->tweekerTransformNonIdentity)
      {
        #if CE_ARTLIB_EnableDebugMode
        LE_MATRIX_3D_SetZero (&ParentPntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D);
        #endif
        ParentPntr->dimensionSpecificData.d3->tweekerTransformNonIdentity = FALSE;
        ParentPntr->needsPositionRecalc = TRUE;
      }
    }

    return;
  }

  // Check for invalid sequence data, tweeker should match the
  // sequence being tweeked.

#if CE_ARTLIB_EnableDebugMode
  switch (SequencePntr->aux.tweeker.interpolatedChunkID)
  {
  case LE_SEQNCR_CHUNK_ID_2D_XFORM:
  case LE_SEQNCR_CHUNK_ID_2D_OFFSET:
  case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET:
    if (SequencePntr->parent->dimensionality != 2)
    {
      LE_ERROR_DebugMsg ("L_SEQNCR.cpp DoTweeking: "
        "Parent sequence needs to be two dimensional to match tweek data.\r\n",
        LE_ERROR_DebugMsgToFileAndScreen);
      return; // We can't interpolate this kind.
    }
    break;

  case LE_SEQNCR_CHUNK_ID_3D_XFORM:
  case LE_SEQNCR_CHUNK_ID_3D_OFFSET:
  case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET:
  case LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE:
    if (SequencePntr->parent->dimensionality != 3)
    {
      LE_ERROR_DebugMsg ("L_SEQNCR.cpp DoTweeking: "
        "Parent sequence needs to be three dimensional to match tweek data.\r\n",
        LE_ERROR_DebugMsgToFileAndScreen);
      return; // We can't interpolate this kind.
    }
    break;

  case LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH:
  case LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME:
  case LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING:
    if (ParentPntr->sequenceType != LE_CHUNK_ID_SEQ_SOUND &&
    ParentPntr->sequenceType != LE_CHUNK_ID_SEQ_VIDEO)
    {
      LE_ERROR_DebugMsg ("L_SEQNCR.cpp DoTweeking: "
        "Parent sequence must be a sound sequence or video sequence.\r\n",
        LE_ERROR_DebugMsgToFileAndScreen);
      return; // We can't interpolate this kind.
    }
    break;

  case LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW:
    if (ParentPntr->sequenceType != LE_CHUNK_ID_SEQ_CAMERA)
    {
      LE_ERROR_DebugMsg ("L_SEQNCR.cpp DoTweeking: "
        "Parent sequence must be a camera.\r\n",
        LE_ERROR_DebugMsgToFileAndScreen);
      return; // We can't interpolate this kind.
    }
    break;

  default:
    LE_ERROR_DebugMsg ("L_SEQNCR.cpp DoTweeking: Can't tweek this kind of data.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
    return; // We can't interpolate this kind.
  }
#endif

  TweekData = SequencePntr->streamSpecificData.tweekData;
  if (TweekData->offsetToInitialDataChunk == 0)
    return; // No data to tweek from.

  // If this is a sound tweeker, get the sound buffer to be tweeked.

  if (SequencePntr->aux.tweeker.interpolatedChunkID >= LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH &&
  SequencePntr->aux.tweeker.interpolatedChunkID <= LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING)
  {
    BufSndPntr = NULL;

    if (ParentPntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND)
      BufSndPntr = ParentPntr->streamSpecificData.audioStream->bufSnd;
    else if (ParentPntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
    {
#if CE_ARTLIB_EnableSystemVideo
      LE_VIDEO_VideoStatusRecord VideoStatus;

      if (LE_VIDEO_GetVideoStatus (ParentPntr->streamSpecificData.
      videoStream->videoHandle, &VideoStatus))
        BufSndPntr = VideoStatus.soundHandle;
#elif CE_ARTLIB_EnableSystemBink
      BufSndPntr = NULL; // Not implemented yet.
#else // Old video player.
      BufSndPntr = ParentPntr->streamSpecificData.videoStream->soundBuffer;
#endif
    }

    if (BufSndPntr == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      LE_ERROR_DebugMsg ("L_SEQNCR.cpp DoTweeking: Can't get sound buffer pointer!\r\n",
        LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return;
    }
  }

  // If there is no reasonable ending time or the interpolation is constant
  // or we don't have before and after data then just use the first bit of
  // data from the subchunks and don't interpolate.

  if (SequencePntr->endTime >= INFINITE_END_TIME ||
  TweekData->offsetToFinalDataChunk == 0 ||
  SequencePntr->aux.tweeker.interpolationType !=
  LE_SEQNCR_INTERPOLATION_LINEAR_PAIR)
    Proportion = 0.0F;
  else // Linear interpolation and have an ending time.
  {
    // Find the proportion of the time used and then use that to
    // interpolate between the user's initial and final data.

    Proportion = (float) SequencePntr->sequenceClock /
      (float) SequencePntr->endTime;
  }

  if (Proportion == TweekData->currentProportion)
    return; // Nothing to do right now.

  TweekData->currentProportion = Proportion;
  BasePntr = (BYTE *) LE_DATA_Use (SequencePntr->dataID);

  if (Proportion == 0.0 || Proportion == 1.0)
  {
    if (Proportion == 0.0)
      DataOffset = TweekData->offsetToInitialDataChunk;
    else
      DataOffset = TweekData->offsetToFinalDataChunk;

    switch (SequencePntr->aux.tweeker.interpolatedChunkID)
    {
    case LE_SEQNCR_CHUNK_ID_2D_XFORM:
      XForm2DA = (LE_SEQNCR_2DXformChunkPointer) (BasePntr + DataOffset);
      ParentPntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D = XForm2DA->matrix;
      ParentPntr->dimensionSpecificData.d2->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_2D_OFFSET:
      Offset2DA = (LE_SEQNCR_2DOffsetChunkPointer) (BasePntr + DataOffset);
      LE_MATRIX_2D_SetTranslate (&Offset2DA->offset,
        &ParentPntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D);
      ParentPntr->dimensionSpecificData.d2->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET:
      OSRT2DA = (LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer) (BasePntr + DataOffset);
      LE_MATRIX_2D_SetOriginScaleRotateTranslate (&OSRT2DA->origin,
        OSRT2DA->scaleX, OSRT2DA->scaleY, OSRT2DA->rotate, &OSRT2DA->offset,
        &ParentPntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D);
      ParentPntr->dimensionSpecificData.d2->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_3D_XFORM:
      XForm3DA = (LE_SEQNCR_3DXformChunkPointer) (BasePntr + DataOffset);
      ParentPntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D =
        XForm3DA->matrix;
      ParentPntr->dimensionSpecificData.d3->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_3D_OFFSET:
      Offset3DA = (LE_SEQNCR_3DOffsetChunkPointer) (BasePntr + DataOffset);
      LE_MATRIX_3D_SetTranslate (&Offset3DA->offset,
        &ParentPntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D);
      ParentPntr->dimensionSpecificData.d3->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET:
      OSRT3DA = (LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer) (BasePntr + DataOffset);
      LE_MATRIX_3D_SetOriginScaleRotateTranslate (&OSRT3DA->origin,
        OSRT3DA->scaleX, OSRT3DA->scaleY, OSRT3DA->scaleZ,
        OSRT3DA->yaw, OSRT3DA->pitch, OSRT3DA->roll,
        &OSRT3DA->offset,
        &ParentPntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D);
      ParentPntr->dimensionSpecificData.d3->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE:
      MeshChoiceA = (LE_SEQNCR_3DMeshChoiceChunkPointer) (BasePntr + DataOffset);
      ParentPntr->dimensionSpecificData.d3->meshIndexA = MeshChoiceA->meshIndexA;
      ParentPntr->dimensionSpecificData.d3->meshIndexB = MeshChoiceA->meshIndexB;
      ParentPntr->dimensionSpecificData.d3->meshProportion = MeshChoiceA->meshProportion;
      ParentPntr->needsPositionRecalc = TRUE; // The mesh update is in the move code.
      break;

    case LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH:
      SoundPitchA = (LE_SEQNCR_SoundPitchChunkPointer) (BasePntr + DataOffset);
      ParentPntr->aux.audio.pitch = SoundPitchA->soundPitch;
      LE_SOUND_SetPitchBufSnd (BufSndPntr, ParentPntr->aux.audio.pitch);
      break;

    case LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME:
      SoundVolumeA = (LE_SEQNCR_SoundVolumeChunkPointer) (BasePntr + DataOffset);
      ParentPntr->aux.audio.volume = SoundVolumeA->soundVolume;
      if (ParentPntr->dimensionality == 0 ||
      ParentPntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
        LE_SOUND_SetVolumeBufSnd (BufSndPntr, ParentPntr->aux.audio.volume);
      else // 2D & 3D calculate panning and volume by position.
        ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING:
      SoundPanningA = (LE_SEQNCR_SoundPanningChunkPointer) (BasePntr + DataOffset);
      ParentPntr->aux.audio.panning = SoundPanningA->soundPanning;
      if (ParentPntr->dimensionality == 0 ||
      ParentPntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
        LE_SOUND_SetPanBufSnd (BufSndPntr, ParentPntr->aux.audio.panning);
#if CE_ARTLIB_EnableDebugMode
      else
        LE_ERROR_DebugMsg ("L_SEQNCR.cpp DoTweeking: Can only set panning on "
        "zero dimensional sounds or videos, not this kind.  2D and 3D have "
        "panning and volume driven by the location in space.\r\n",
        LE_ERROR_DebugMsgToFileAndScreen);
#endif
      break;

    case LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW:
      FieldOfViewA = (LE_SEQNCR_FieldOfViewChunkPointer) (BasePntr + DataOffset);
      ParentPntr->aux.cameraFieldOfView = FieldOfViewA->fieldOfView;
      break;
    }
  }
  else // Doing linear interpolation between two values.
  {
    switch (SequencePntr->aux.tweeker.interpolatedChunkID)
    {
    case LE_SEQNCR_CHUNK_ID_2D_XFORM:
      XForm2DA = (LE_SEQNCR_2DXformChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      XForm2DB = (LE_SEQNCR_2DXformChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      Matrix2DPntr = &ParentPntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D;
      Matrix2DPntr->_11 = FloatInterpolation (Proportion, XForm2DA->matrix._11, XForm2DB->matrix._11);
      Matrix2DPntr->_12 = FloatInterpolation (Proportion, XForm2DA->matrix._12, XForm2DB->matrix._12);
      Matrix2DPntr->_13 = FloatInterpolation (Proportion, XForm2DA->matrix._13, XForm2DB->matrix._13);
      Matrix2DPntr->_21 = FloatInterpolation (Proportion, XForm2DA->matrix._21, XForm2DB->matrix._21);
      Matrix2DPntr->_22 = FloatInterpolation (Proportion, XForm2DA->matrix._22, XForm2DB->matrix._22);
      Matrix2DPntr->_23 = FloatInterpolation (Proportion, XForm2DA->matrix._23, XForm2DB->matrix._23);
      Matrix2DPntr->_31 = FloatInterpolation (Proportion, XForm2DA->matrix._31, XForm2DB->matrix._31);
      Matrix2DPntr->_32 = FloatInterpolation (Proportion, XForm2DA->matrix._32, XForm2DB->matrix._32);
      Matrix2DPntr->_33 = FloatInterpolation (Proportion, XForm2DA->matrix._33, XForm2DB->matrix._33);
      ParentPntr->dimensionSpecificData.d2->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_2D_OFFSET:
      Offset2DA = (LE_SEQNCR_2DOffsetChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      Offset2DB = (LE_SEQNCR_2DOffsetChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      Point2DA.x = IntegerInterpolation (Proportion, Offset2DA->offset.x, Offset2DB->offset.x);
      Point2DA.y = IntegerInterpolation (Proportion, Offset2DA->offset.y, Offset2DB->offset.y);
      LE_MATRIX_2D_SetTranslate (&Point2DA,
        &ParentPntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D);
      ParentPntr->dimensionSpecificData.d2->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_2D_ORIGIN_SCALE_ROTATE_OFFSET:
      OSRT2DA = (LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      OSRT2DB = (LE_SEQNCR_2DOriginScaleRotateOffsetChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      Point2DA.x = IntegerInterpolation (Proportion, OSRT2DA->offset.x, OSRT2DB->offset.x);
      Point2DA.y = IntegerInterpolation (Proportion, OSRT2DA->offset.y, OSRT2DB->offset.y);
      Point2DB.x = IntegerInterpolation (Proportion, OSRT2DA->origin.x, OSRT2DB->origin.x);
      Point2DB.y = IntegerInterpolation (Proportion, OSRT2DA->origin.y, OSRT2DB->origin.y);
      ScaleX = FloatInterpolation (Proportion, OSRT2DA->scaleX, OSRT2DB->scaleX);
      ScaleY = FloatInterpolation (Proportion, OSRT2DA->scaleY, OSRT2DB->scaleY);
      Yaw = FloatInterpolation (Proportion, OSRT2DA->rotate, OSRT2DB->rotate);
      LE_MATRIX_2D_SetOriginScaleRotateTranslate (
        &Point2DB, ScaleX, ScaleY, Yaw, &Point2DA,
        &ParentPntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D);
      ParentPntr->dimensionSpecificData.d2->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_3D_XFORM:
      XForm3DA = (LE_SEQNCR_3DXformChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      XForm3DB = (LE_SEQNCR_3DXformChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      Matrix3DPntr = &ParentPntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D;
      Matrix3DPntr->_11 = FloatInterpolation (Proportion, XForm3DA->matrix._11, XForm3DB->matrix._11);
      Matrix3DPntr->_12 = FloatInterpolation (Proportion, XForm3DA->matrix._12, XForm3DB->matrix._12);
      Matrix3DPntr->_13 = FloatInterpolation (Proportion, XForm3DA->matrix._13, XForm3DB->matrix._13);
      Matrix3DPntr->_14 = FloatInterpolation (Proportion, XForm3DA->matrix._14, XForm3DB->matrix._14);
      Matrix3DPntr->_21 = FloatInterpolation (Proportion, XForm3DA->matrix._21, XForm3DB->matrix._21);
      Matrix3DPntr->_22 = FloatInterpolation (Proportion, XForm3DA->matrix._22, XForm3DB->matrix._22);
      Matrix3DPntr->_23 = FloatInterpolation (Proportion, XForm3DA->matrix._23, XForm3DB->matrix._23);
      Matrix3DPntr->_24 = FloatInterpolation (Proportion, XForm3DA->matrix._24, XForm3DB->matrix._24);
      Matrix3DPntr->_31 = FloatInterpolation (Proportion, XForm3DA->matrix._31, XForm3DB->matrix._31);
      Matrix3DPntr->_32 = FloatInterpolation (Proportion, XForm3DA->matrix._32, XForm3DB->matrix._32);
      Matrix3DPntr->_33 = FloatInterpolation (Proportion, XForm3DA->matrix._33, XForm3DB->matrix._33);
      Matrix3DPntr->_34 = FloatInterpolation (Proportion, XForm3DA->matrix._34, XForm3DB->matrix._34);
      Matrix3DPntr->_41 = FloatInterpolation (Proportion, XForm3DA->matrix._41, XForm3DB->matrix._41);
      Matrix3DPntr->_42 = FloatInterpolation (Proportion, XForm3DA->matrix._42, XForm3DB->matrix._42);
      Matrix3DPntr->_43 = FloatInterpolation (Proportion, XForm3DA->matrix._43, XForm3DB->matrix._43);
      Matrix3DPntr->_44 = FloatInterpolation (Proportion, XForm3DA->matrix._44, XForm3DB->matrix._44);
      ParentPntr->dimensionSpecificData.d3->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_3D_OFFSET:
      Offset3DA = (LE_SEQNCR_3DOffsetChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      Offset3DB = (LE_SEQNCR_3DOffsetChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      Point3DA.x = FloatInterpolation (Proportion, Offset3DA->offset.x, Offset3DB->offset.x);
      Point3DA.y = FloatInterpolation (Proportion, Offset3DA->offset.y, Offset3DB->offset.y);
      Point3DA.z = FloatInterpolation (Proportion, Offset3DA->offset.z, Offset3DB->offset.z);
      LE_MATRIX_3D_SetTranslate (&Point3DA,
        &ParentPntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D);
      ParentPntr->dimensionSpecificData.d3->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_3D_ORIGIN_SCALE_ROTATE_OFFSET:
      OSRT3DA = (LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      OSRT3DB = (LE_SEQNCR_3DOriginScaleRotateOffsetChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      Point3DA.x = FloatInterpolation (Proportion, OSRT3DA->offset.x, OSRT3DB->offset.x);
      Point3DA.y = FloatInterpolation (Proportion, OSRT3DA->offset.y, OSRT3DB->offset.y);
      Point3DA.z = FloatInterpolation (Proportion, OSRT3DA->offset.z, OSRT3DB->offset.z);
      Point3DB.x = FloatInterpolation (Proportion, OSRT3DA->origin.x, OSRT3DB->origin.x);
      Point3DB.y = FloatInterpolation (Proportion, OSRT3DA->origin.y, OSRT3DB->origin.y);
      Point3DB.z = FloatInterpolation (Proportion, OSRT3DA->origin.z, OSRT3DB->origin.z);
      ScaleX = FloatInterpolation (Proportion, OSRT3DA->scaleX, OSRT3DB->scaleX);
      ScaleY = FloatInterpolation (Proportion, OSRT3DA->scaleY, OSRT3DB->scaleY);
      ScaleZ = FloatInterpolation (Proportion, OSRT3DA->scaleZ, OSRT3DB->scaleZ);
      Yaw = FloatInterpolation (Proportion, OSRT3DA->yaw, OSRT3DB->yaw);
      Pitch = FloatInterpolation (Proportion, OSRT3DA->pitch, OSRT3DB->pitch);
      Roll = FloatInterpolation (Proportion, OSRT3DA->roll, OSRT3DB->roll);
      LE_MATRIX_3D_SetOriginScaleRotateTranslate (
        &Point3DB, ScaleX, ScaleY, ScaleZ, Yaw, Pitch, Roll, &Point3DA,
        &ParentPntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D);
      ParentPntr->dimensionSpecificData.d3->tweekerTransformNonIdentity = TRUE;
      ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_3D_MESH_CHOICE:
      MeshChoiceA = (LE_SEQNCR_3DMeshChoiceChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      MeshChoiceB = (LE_SEQNCR_3DMeshChoiceChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      ParentPntr->dimensionSpecificData.d3->meshIndexA = MeshChoiceA->meshIndexA;
      ParentPntr->dimensionSpecificData.d3->meshIndexB = MeshChoiceA->meshIndexB;
      ParentPntr->dimensionSpecificData.d3->meshProportion = FloatInterpolation (Proportion,
        MeshChoiceA->meshProportion, MeshChoiceB->meshProportion);
      ParentPntr->needsPositionRecalc = TRUE; // The mesh update is in the move code.
      break;

    case LE_SEQNCR_CHUNK_ID_SET_SOUND_PITCH:
      SoundPitchA = (LE_SEQNCR_SoundPitchChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      SoundPitchB = (LE_SEQNCR_SoundPitchChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      ParentPntr->aux.audio.pitch = IntegerInterpolation (Proportion,
        SoundPitchA->soundPitch, SoundPitchB->soundPitch);
      LE_SOUND_SetPitchBufSnd (BufSndPntr, ParentPntr->aux.audio.pitch);
      break;

    case LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME:
      SoundVolumeA = (LE_SEQNCR_SoundVolumeChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      SoundVolumeB = (LE_SEQNCR_SoundVolumeChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      ParentPntr->aux.audio.volume = IntegerInterpolation (Proportion,
        SoundVolumeA->soundVolume, SoundVolumeB->soundVolume);
      if (ParentPntr->dimensionality == 0 ||
      ParentPntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
        LE_SOUND_SetVolumeBufSnd (BufSndPntr, ParentPntr->aux.audio.volume);
      else // 2D & 3D calculate panning and volume by position.
        ParentPntr->needsPositionRecalc = TRUE;
      break;

    case LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING:
      SoundPanningA = (LE_SEQNCR_SoundPanningChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      SoundPanningB = (LE_SEQNCR_SoundPanningChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      ParentPntr->aux.audio.panning = IntegerInterpolation (Proportion,
        SoundPanningA->soundPanning, SoundPanningB->soundPanning);
      if (ParentPntr->dimensionality == 0 ||
      ParentPntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
        LE_SOUND_SetPanBufSnd (BufSndPntr, ParentPntr->aux.audio.panning);
#if CE_ARTLIB_EnableDebugMode
      else
        LE_ERROR_DebugMsg ("L_SEQNCR.cpp DoTweeking: Can only set panning on "
        "zero dimensional sounds or videos, not this kind.  2D and 3D have "
        "panning and volume driven by the location in space.\r\n",
        LE_ERROR_DebugMsgToFileAndScreen);
#endif
      break;

    case LE_SEQNCR_CHUNK_ID_CAMERA_FIELD_OF_VIEW:
      FieldOfViewA = (LE_SEQNCR_FieldOfViewChunkPointer) (BasePntr + TweekData->offsetToInitialDataChunk);
      FieldOfViewB = (LE_SEQNCR_FieldOfViewChunkPointer) (BasePntr + TweekData->offsetToFinalDataChunk);
      ParentPntr->aux.cameraFieldOfView = FloatInterpolation (Proportion,
        FieldOfViewA->fieldOfView, FieldOfViewB->fieldOfView);
      break;
    }
  }
}



/*****************************************************************************
 * The given sequence has hit its end, see if any chained sequences (commands)
 * are waiting for it and move them to the list of de-chained commands which
 * will be executed soon.  Also stops the sequence that was causing the
 * chain.  Returns TRUE if chains were found, which will make the caller stop
 * processing the sequence (a good idea since it is deleted here).
 */

static BOOL LI_SEQNCR_LookForChainedCommands (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  ChainedItemPointer      CurrentPntr;
  BOOL                    FoundChainedCommand = FALSE;
  ChainedItemPointer      NextPntr;
  LE_DATA_DataId          SearchForDataID;
  UNS16                   SearchForPriority;

  if (SequencePntr == NULL)
    return FALSE;

  // To avoid picking up sub-sequences nested inside a grouping sequence, only
  // look at ones that have a start offset of zero (starting at the start
  // of the DataID).  Otherwise funny things would happen because nested
  // sequences have the same DataID as their container.

  if (SequencePntr->dataOffset != 0)
    return FALSE;

  SearchForPriority = SequencePntr->priority;
  SearchForDataID = SequencePntr->dataID;
  if (SearchForDataID == LE_DATA_EmptyItem)
    return FALSE; // The root node or something weird.

  // Search thorugh the waiting chained commands for all that match
  // the DataID and Priority of the given sequence.  Search from the
  // tail forwards, so that commands are done in the order of arrival.

  CurrentPntr = WaitingChainedCommandsTail;
  while (CurrentPntr != NULL)
  {
    NextPntr = CurrentPntr->previous; // We are going from tail to head.

    if ((LE_DATA_DataId) CurrentPntr->message.numberC == SearchForDataID &&
    CurrentPntr->message.numberD == SearchForPriority)
    {
      FoundChainedCommand = TRUE;

      // Remove the command we found from the middle of the
      // pending chained commands list.

      if (CurrentPntr->next == NULL)
        WaitingChainedCommandsTail = CurrentPntr->previous;
      else
        CurrentPntr->next->previous = CurrentPntr->previous;

      if (CurrentPntr->previous == NULL)
        WaitingChainedCommandsHead = CurrentPntr->next;
      else
        CurrentPntr->previous->next = CurrentPntr->next;

      // Add the command to the head of the list of de-chained commands
      // (things get added to the head and removed from the tail).

      if (ExecutableChainedCommandsHead != NULL)
        ExecutableChainedCommandsHead->previous = CurrentPntr;
      else
        ExecutableChainedCommandsTail = CurrentPntr;
      CurrentPntr->next = ExecutableChainedCommandsHead;
      CurrentPntr->previous = NULL;
      ExecutableChainedCommandsHead = CurrentPntr;
    }

    // Advance to the next pending chain command.

    CurrentPntr = NextPntr;
  }

  if (!FoundChainedCommand)
    return FALSE; // No chains found, nothing done.

  // Yes, this was a chained sequence.  Deallocate it.

  DeleteSequenceAndChildren (SequencePntr);

#if CE_ARTLIB_SeqncrChainStopsAllSameIDAndPri
  // Also stop all others with the same DataID/Priority, but we can only
  // do that by issuing a command to ourselves, since we are in the
  // middle of a tree traversal and deleting things would make a mess.

  if (FreeChainedCommands != NULL && FindNextSequence (SearchForDataID,
  SearchForPriority, FALSE /* Only do toplevel */, NULL) != NULL)
  {
    SeqCmdFlagsAsLong   CommandFlags;
    ChainedItemPointer  NewCommandPntr;

    // Yes, there are other sequences out there with the same
    // DataID/Priority as this one.  Stop them!

    // Allocate a command record.

    NewCommandPntr = FreeChainedCommands;
    FreeChainedCommands = FreeChainedCommands->next;

    // Set it up with a stop command.

    memset (NewCommandPntr, 0, sizeof (ChainedItemRecord));
    NewCommandPntr->message.numberA = SearchForDataID;
    NewCommandPntr->message.numberB = SearchForPriority;
    NewCommandPntr->message.numberC = LE_DATA_EmptyItem;
    CommandFlags.asDWORD = 0;
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = TRUE;
    NewCommandPntr->message.numberE = CommandFlags.asDWORD;
    NewCommandPntr->message.messageCode = SEQCMD_Stop;

    // Add it to the queue of executing commands.

    if (ExecutableChainedCommandsHead == NULL)
      ExecutableChainedCommandsTail = NewCommandPntr;
    else
      ExecutableChainedCommandsHead->previous = NewCommandPntr;
    NewCommandPntr->next = ExecutableChainedCommandsHead;
    ExecutableChainedCommandsHead = NewCommandPntr;
  }
#endif /* CE_ARTLIB_SeqncrChainStopsAllSameIDAndPri */

  return TRUE;
}



/*****************************************************************************
 * This function advances the clock for the given sequence to bring it up to
 * the current time (as defined by the parent's clock).  This includes all
 * children.  If not much time has passed since the last update (as measured
 * by the change in parent clock since the last update) then nothing is done,
 * including the children.  Besides updating clocks and checking for the end
 * of time, positions of sequences are updated if needed and the renderer
 * informed if needed.  This assumes that the parent clock and position
 * (sequence to world transformation) are up to date on entry.
 *
 * Returns TRUE if the sequence or its children changed, meaning it needs a
 * redraw.
 *
 * Upon exit all the needsRedraw flags are valid in the whole tree (propogated
 * up from children which have changed) and all the needsPositionRecalc ones
 * have been cleared.
 */

static BOOL LI_SEQNCR_UpdateSequenceClock (LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  LE_SEQNCR_RuntimeInfoPointer  CurrentChildPntr;
  INT32                         ElapsedTime;
  LE_SEQNCR_RuntimeInfoPointer  NextChildPntr;
  INT32                         OldClock;
  INT32                         ParentClock;

  if (SequencePntr == NULL)
    return FALSE; // Nothing to do to no sequence no how.

  // Get the parent sequence's clock to find the elapsed time from.
  // If this is the root node, use the real time clock.

  if (SequencePntr->parent == NULL)
    ParentClock = TopLevelClock;
  else
    ParentClock = SequencePntr->parent->sequenceClock;

  // First see if enough time has passed.  An old time stamp of INT32_MIN
  // means that the sequence is new and hasn't been updated yet.  Also
  // do an update anyways if the needsReEvaluation flag is on.

  if (SequencePntr->parentTimeOfLastUpdate == INT32_MIN)
  {
    // First update is instantaneous.  Includes updates for children
    // of a looping sequence when it loops (their clocks have already
    // been reset to the correct time).
    ElapsedTime = 0;
  }
  else
  {
    // The time is based on the change in the parent's clock.

    ElapsedTime = ParentClock - SequencePntr->parentTimeOfLastUpdate;

    // If this sequence is paused, stop the clock from advancing.
    // Paused sequences don't do much at all, unless someone requested
    // an update.  When it gets unpaused, the cumulative elapsed time
    // (from parentTimeOfLastUpdate) has to be fixed up to avoid a
    // sudden jump in its clock.

    if (SequencePntr->paused)
      ElapsedTime = 0;

    // See if it is time for another update.

    if (ElapsedTime < SequencePntr->timeMultiple &&
    !SequencePntr->needsReEvaluation)
      return FALSE; // Not enough time has passed and don't need to update.
  }

  // If we are not allowed to drop frames, then limit the advance
  // in time to just one time unit for this sequence.

  if (ElapsedTime > SequencePntr->timeMultiple && !SequencePntr->dropFrames)
    ElapsedTime = SequencePntr->timeMultiple;

  if (SequencePntr->parentTimeOfLastUpdate == INT32_MIN)
    OldClock = INT32_MIN; // No previous clock time, so use minus infinity.
  else
    OldClock = SequencePntr->sequenceClock;

  // Set the sequenceClock.  For sound sequences, read the hardware
  // clock and use that for the actual sequenceClock.  The timer based
  // clock (parent time) is only used for the initial elapsed time
  // calculations which handle the timeMultiple update rate (don't
  // want to use the sound clock for that since it can take a lot
  // of time to calculate the sound clock).  Note that it actually
  // gets the clock as of the last feeding, which happened at the
  // last frame update, so this clock is one update cycle behind
  // the actual sound you are hearing.

  if ((SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND ||
  (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO &&
  SequencePntr->streamSpecificData.videoStream->enableAudio)) &&
  OldClock != INT32_MIN)
  {
    if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
    {
      // Get the clock corresponding to the frame number.
      // Temporarily reuse the ElapsedTime variable since this
      // is a recursive function and we don't want to waste
      // stack space.

      ElapsedTime = (LONG) ((double) SequencePntr->endTime *
      SequencePntr->streamSpecificData.videoStream->currentVideoFrameNumber /
      SequencePntr->streamSpecificData.videoStream->numberOfVideoFrames);
    }
    else // Must be a sound.
    {
      // Get the clock corresponding to the hardware playback time.

      ElapsedTime = SequencePntr->streamSpecificData.audioStream->playTime;
    }

    // Fix the roundoff error in the calculations so it ends.

    if (ElapsedTime == SequencePntr->endTime - 1)
      ElapsedTime = SequencePntr->endTime;

    // Get the real elapsed time.

    ElapsedTime = ElapsedTime - OldClock;

    if (ElapsedTime < 0)
    {
      // Going backwards in time, usually due to hardware looping of sounds,
      // so wipe out all child sequences and restart from the beginning,
      // kind of like using LI_SEQNCR_GoBackwardsInTime except that
      // this isn't a command which changes things.  Note that there is
      // a DirectSound 5 bug where it reports that a cached sound (entirely
      // in the buffer) is still playing at time zero when it really has
      // finished (the sound stopped status comes on a little bit after
      // that).  So, some sounds will sometimes jump back to time zero
      // and then soon after jump to the ending time.  Will only waste
      // time with tweeked sounds.

      DeleteJustChildren (SequencePntr);
      OldClock = INT32_MIN;
      if (SequencePntr->childSequencesDataID != LE_DATA_EmptyItem)
      {
        if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_INDIRECT)
          SequencePntr->childSequencesOffset = 0;
        else
          SequencePntr->childSequencesOffset = (SequencePntr->dataOffset +
          LE_CHUNK_ChunkFixedDataSizes [SequencePntr->sequenceType] +
          sizeof (LE_CHUNK_HeaderRecord));
      }
    }
  }

  SequencePntr->sequenceClock += ElapsedTime;
  SequencePntr->parentTimeOfLastUpdate = ParentClock;

  // If this is an off screen scrolling world, the clock stays at zero, well
  // actually at -timeMultiple so that it can advance to zero and fire off
  // the child sequences that start at zero, when it wakes up.  While it
  // is hibernating off screen, it can't have any children.

  if (SequencePntr->scrollingWorld && SequencePntr->onScreen == 0)
  {
    if (OldClock != -SequencePntr->timeMultiple)
    {
      // This scrolling sequence has just gone off the screen in the
      // last update.  Remove all children and rewind the clock.

      DeleteJustChildren (SequencePntr);
      OldClock = INT32_MIN; // Restart all children since minus infinity.
      if (SequencePntr->childSequencesDataID != LE_DATA_EmptyItem)
      {
        if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_INDIRECT)
          SequencePntr->childSequencesOffset = 0;
        else
          SequencePntr->childSequencesOffset = (SequencePntr->dataOffset +
          LE_CHUNK_ChunkFixedDataSizes [SequencePntr->sequenceType] +
          sizeof (LE_CHUNK_HeaderRecord));
      }
    }
    else // Was already off screen, just keep the clock frozen.
      SequencePntr->sequenceClock = -SequencePntr->timeMultiple;
  }

  // Check for redraw at every update option.

  if (SequencePntr->redrawAtEveryUpdate)
  {
    if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
    {
      // Only request an extra redraw for video if the frame has changed,
      // otherwise we waste time redrawing the current frame.  We still
      // do the occasional update since the feeding may have stalled
      // or otherwise gone weird.

      if (GlueGetDesiredFrameNumber (SequencePntr) !=
      SequencePntr->streamSpecificData.videoStream->currentVideoFrameNumber ||
      GlueVideoIsStalled (SequencePntr))
      {
        SequencePntr->needsRedraw = TRUE;

        // Also do a feeding here (rather than in the renderer) if this is
        // drawing to an intermediate bitmap.  That way the renderer will
        // draw the same bitmap again (not the one for a different frame)
        // if it is only redrawing part of the screen (like the part under
        // a moving mouse pointer).  When drawing direct to the screen, the
        // feeding and drawing are done at the same time, in the renderer,
        // leading to some update glitches, but vastly improved performance.

        if (!SequencePntr->streamSpecificData.videoStream->drawDirectlyToScreen)
          LI_SEQNCR_GlueFeedAndDrawVideo (SequencePntr, NULL, NULL, 0, 0, NULL);
      }
    }
    else
      SequencePntr->needsRedraw = TRUE;
  }

  // See if we hit the end of the sequence.

  if (SequencePntr->sequenceClock >= SequencePntr->endTime ||
  SequencePntr->endingAction == LE_SEQNCR_EndingActionSuicide)
  {
    // Do the ending action for the sequence.  First let the user know
    // that it ended by doing callbacks and message sends.

    if (SequencePntr->watchIndex >= 0)
    {
      if (LE_SEQNCR_WatchArray[SequencePntr->watchIndex].dataID !=
      SequencePntr->dataID ||
      LE_SEQNCR_WatchArray[SequencePntr->watchIndex].priority !=
      SequencePntr->priority)
        SequencePntr->watchIndex = -1; // Watching someone else now.
      else
      {
        LE_SEQNCR_WatchArray[SequencePntr->watchIndex].hasHitEnd = TRUE;
        LE_SEQNCR_WatchArray[SequencePntr->watchIndex].updateCount++;
      }
    }

    if (SequencePntr->labelNumber != 0)
    {
      // Avoid sending duplicate end messages for the stay-at-end action,
      // which keeps bouncing the clock back to the ending time, thus
      // making it end repeatedly.

      if (SequencePntr->endingAction != LE_SEQNCR_EndingActionStayAtEnd ||
      OldClock < SequencePntr->endTime)
      {
        LE_UIMSG_SendEvent (UIMSG_SEQUENCE_REACHED_END,
          SequencePntr->dataID, SequencePntr->priority,
          SequencePntr->labelNumber, SequencePntr->endingAction,
          TopLevelClock, NULL, 0);
      }
    }

    if (SequencePntr->callBack != NULL)
      SequencePntr->callBack (SequencePntr, LE_SEQNCR_CALLOP_END_TIME_HIT);

    // Check for any pending chained commands waiting for this sequence to
    // hit the end.  If there is one, it starts and this sequence stops,
    // and maybe all others with the same DataID/Priority also die.

    if (LI_SEQNCR_LookForChainedCommands (SequencePntr))
      return TRUE; // It's deleted now, nothing more to do.

    // OK, now look at the ending actions.

    if (SequencePntr->endingAction == LE_SEQNCR_EndingActionStop ||
    SequencePntr->endingAction == LE_SEQNCR_EndingActionSuicide)
    {
      DeleteSequenceAndChildren (SequencePntr);
      return TRUE; // It's deleted now, nothing more to do.
    }

    if (SequencePntr->endingAction == LE_SEQNCR_EndingActionStayAtEnd)
    {
      SequencePntr->sequenceClock = SequencePntr->endTime; // Stay at end.
#if CE_ARTLIB_SeqncrStayAtEndLowUpdateRate
      // Cut the update rate down to 1 time per 4.25 seconds to save on CPU
      // time for essentially inactive sequences.  The downside is that
      // a pending chained command may take up to 4 seconds to be activated.
      // Note that this field is 8 bits, max value is 255.
      SequencePntr->timeMultiple = 255;
#endif
    }
    else if (SequencePntr->endingAction == LE_SEQNCR_EndingActionLoopToBeginning)
    {
      // A looping sequence, go back to time zero (which destroys the
      // child sequences and recreates them for the new time) and
      // restart everything.  But don't do it for sounds since they
      // are already looping at the hardware level, and we are reading
      // their clocks, not setting them.  Watch out for silent movies.

      if (SequencePntr->sequenceType != LE_CHUNK_ID_SEQ_SOUND &&
      (SequencePntr->sequenceType != LE_CHUNK_ID_SEQ_VIDEO ||
      !SequencePntr->streamSpecificData.videoStream->enableAudio))
        LI_SEQNCR_GoBackwardsInTime (SequencePntr, 0);
    }
  }

  // Add any new child sequences which appear because time has advanced
  // and their start time has been reached.  They can see their parent
  // with valid time but invalid position.  Only do for sequences that
  // can have children.  Starts children from just after OldClock
  // up to and including the new clock.

  if (SequencePntr->childSequencesDataID != LE_DATA_EmptyItem &&
  LI_SEQNCR_AddNewlyBornChildren (SequencePntr, OldClock))
    SequencePntr->needsRedraw = TRUE;

  // If this sequence has tweeker kind of children, do the tweeking
  // for the current time.  This has to be done before this
  // sequence's position is calculated, yet normally child sequences
  // are updated after their parent position is known.  So, just for
  // tweekers, update the time before the parent position, as well
  // tweekers don't have a position of their own (zero dimensional)
  // so that doesn't matter.  Also, this is done here in the sequencer
  // rather than the render slots since you don't want to modify a
  // sequence several times if it is in multiple slots, and tweekers
  // then don't have to be in a slot at all.  The tweeker will set
  // the parent sequence's needsPositionRecalc if it has changed
  // the position or set the parent's needsRedraw if the sequence
  // needs to be redrawn for some reason.

  if (SequencePntr->hasTweekerChildren)
  {
    CurrentChildPntr = SequencePntr->child;
    while (CurrentChildPntr != NULL)
    {
      NextChildPntr = CurrentChildPntr->sibling;

      if (CurrentChildPntr->sequenceType == LE_CHUNK_ID_SEQ_TWEEKER)
        LI_SEQNCR_UpdateSequenceClock (CurrentChildPntr);

      CurrentChildPntr = NextChildPntr;
    }
  }

  if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_TWEEKER)
    DoTweeking (SequencePntr); // Tweeks the parent sequence based on tweeker's clock.

  // Get ready to update position by calling any user provided
  // functions that change the position.  The user function will
  // set needsPositionRecalc if it changes the position.

  if (SequencePntr->callBack != NULL)
    SequencePntr->callBack (SequencePntr, LE_SEQNCR_CALLOP_POSITION_CALC);

  // Recalculate the position if needed.  Presumably the parent's position
  // is already up to date.  Also if this was due to a moving sequence, all
  // the descendants will have been marked as needsReEvaluation when it
  // was moved.  If this is a new sequence, then it doesn't have descendants.
  // Either way, we don't have to mark the descendants here.

  if (SequencePntr->needsPositionRecalc)
  {
    UpdateSequenceToWorldTransformationAndInformRenderSlotsOfMovement (SequencePntr);
    SequencePntr->needsRedraw = TRUE;
  }

  // Now have sequence to world transformation (position in other words),
  // clock and child lists all up to date.  Update the children now.
  // Note that some may self destruct when their clocks reach the end
  // of time, so be careful out there when traversing the child list.

  CurrentChildPntr = SequencePntr->child;
  while (CurrentChildPntr != NULL)
  {
    NextChildPntr = CurrentChildPntr->sibling; // Save pointer in case child is deleted.

    // If this sequence has moved, all the children have moved too
    // and need to have their positions recalculated too.  This kind
    // of passes the recalculations down the tree, without doing
    // a separate recursive update of the flags.

    if (SequencePntr->needsPositionRecalc)
    {
      CurrentChildPntr->needsPositionRecalc = TRUE;
      CurrentChildPntr->needsReEvaluation = TRUE;
    }

    // Update the child's position and other stuff.  The position recalc
    // flag goes in and the redraw flag comes out.

    if (CurrentChildPntr->sequenceType != LE_CHUNK_ID_SEQ_TWEEKER &&
    LI_SEQNCR_UpdateSequenceClock (CurrentChildPntr))
      SequencePntr->needsRedraw = TRUE;

    CurrentChildPntr = NextChildPntr;
  }

  SequencePntr->needsPositionRecalc = FALSE; // No longer needed by children.
  SequencePntr->needsReEvaluation = FALSE; // Evaluation has been done.
  return SequencePntr->needsRedraw;  // Pass it up to the parent, making it redraw.
}



/*****************************************************************************
 * This function cancels a command.  If the command isn't submitted to the
 * sequencer, perhaps because the command was chained and all chains were
 * cancelled, there could be unallocated data in the command or AddRefs on
 * data items which need to be removed.  This function cleans it up.  Note
 * that it doesn't do the LE_QUEUE_FreeStorageAssociatedWithMessage.  Also
 * note that it shouldn't touch the command queues since they are in an
 * inconsistent state at this time.
 */

static void CancelCommand (LE_QUEUE_MessagePointer CommandPntr)
{
  if (CommandPntr == NULL)
    return;

  switch (CommandPntr->messageCode)
  {
  case SEQCMD_Start:
    break; // Eventually will unlock subsequence memory locks.

  case SEQCMD_Stop:
    break; // Nothing to deallocate.

  case SEQCMD_StopAll:
    break; // Nothing to deallocate.

  case SEQCMD_Move:
    break; // Nothing to deallocate.

  case SEQCMD_ForgetChains:
    break; // Nothing to deallocate.

  case SEQCMD_Pause:
    break; // Nothing to deallocate.

  case SEQCMD_SetClock:
    break; // Nothing to deallocate.

  case SEQCMD_SetEndingAction:
    break; // Nothing to deallocate.

  case SEQCMD_DefaultRenderSlotSet:
    break; // Nothing to deallocate.

  case SEQCMD_InvalidateRect:
    break; // Nothing to deallocate.

  case SEQCMD_ForceRedraw:
    break; // Nothing to deallocate.

  case SEQCMD_SetVolume:
    break; // Nothing to deallocate.

  case SEQCMD_SetPan:
    break; // Nothing to deallocate.

  case SEQCMD_SetPitch:
    break; // Nothing to deallocate.

  case SEQCMD_SetSaturation:
    break; // Nothing to deallocate.

  case SEQCMD_SetBrightness:
    break; // Nothing to deallocate.

  case SEQCMD_SetContrast:
    break; // Nothing to deallocate.

  case SEQCMD_SetVideoAlternative:
    break; // Nothing to deallocate.

  case SEQCMD_ChooseVideoAlternative:
    break; // Nothing to deallocate.

  case SEQCMD_ForgetVideoAlternatives:
    break; // Nothing to deallocate.

  case SEQCMD_SetViewport:
    break; // Nothing to deallocate.

  case SEQCMD_SetCamera:
    break; // Nothing to deallocate.

  default:
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c CancelCommand: "
      "Unknown command #%d.\r\n", (int) CommandPntr->messageCode);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    break;
  }
}



/****************************************************************************
 * Remove all pending chain commands.
 */

static void ForgetChains (void)
{
  ChainedItemPointer      CurrentPntr;

  while (WaitingChainedCommandsHead != NULL)
  {
    // Remove the command from the waiting commands list,
    // traverse from head to tail.  Don't bother keeping
    // the queue in a consistent state.

    CurrentPntr = WaitingChainedCommandsHead;
    WaitingChainedCommandsHead = CurrentPntr->next;

    // Deallocate attached things.

    CancelCommand (&CurrentPntr->message);
    LE_QUEUE_FreeStorageAssociatedWithMessage (&CurrentPntr->message);

    // Add it to the free list.

    memset (CurrentPntr, 0, sizeof (ChainedItemRecord));
    CurrentPntr->next = FreeChainedCommands;
    FreeChainedCommands = CurrentPntr;
  }

  WaitingChainedCommandsHead = WaitingChainedCommandsTail = NULL;
}



/*****************************************************************************
 * Cancel all pending commands that are marked for deletion.  Also delete
 * any running sequences that they are waiting for (chained to).  The
 * SearchWholeTree flag controls the search for running sequences.
 */

static void DeleteSequencesAndCancelMarkedPendingCommands (BOOL SearchWholeTree)
{
  SeqCmdFlagsAsLong       CommandFlags;
  LE_QUEUE_MessagePointer CommandPntr;
  ChainedItemPointer      CurrentPntr;
  ChainedItemPointer      NextPntr;

  CurrentPntr = WaitingChainedCommandsTail;
  while (CurrentPntr != NULL)
  {
    NextPntr = CurrentPntr->previous; // Going from tail to head.

    CommandPntr = &CurrentPntr->message;
    CommandFlags.asDWORD = CommandPntr->numberE;

    if (CommandFlags.asFlags.SEQFLAG_MarkedForDeletion)
    {
      // Remove the command we found from the middle of the
      // pending chained commands list.

      if (CurrentPntr->next == NULL)
        WaitingChainedCommandsTail = CurrentPntr->previous;
      else
        CurrentPntr->next->previous = CurrentPntr->previous;

      if (CurrentPntr->previous == NULL)
        WaitingChainedCommandsHead = CurrentPntr->next;
      else
        CurrentPntr->previous->next = CurrentPntr->next;

      // Deallocate attached things.  Stop sequences it is waiting for.

      CancelCommand (CommandPntr);
      LE_QUEUE_FreeStorageAssociatedWithMessage (CommandPntr);

      if (CommandPntr->numberC != LE_DATA_EmptyItem)
      {
        DeleteMatchingSequences (CommandPntr->numberC,
          (UNS16) CommandPntr->numberD, SearchWholeTree);
      }

      // Add it to the free list.

      memset (CurrentPntr, 0, sizeof (ChainedItemRecord));
      CurrentPntr->next = FreeChainedCommands;
      FreeChainedCommands = CurrentPntr;
    }

    CurrentPntr = NextPntr;
  }
}



/*****************************************************************************
 * This function is called by the render slots when they are shutting down
 * to remove all sequences that use the given render slot.  The sequences
 * are also removed from all other slots they happen to be in.  Any
 * chains related to the sequences are forgotten too.
 */

void LI_SEQNCR_RemoveSequencesInRenderSlotSet (
LE_REND_RenderSlotSet RenderSlotSetToDelete)
{
  LE_SEQNCR_RuntimeInfoPointer NextSequencePntr;
  LE_SEQNCR_RuntimeInfoPointer SequencePntr;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return; // System is closed.

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  // Go through all top level sequences in the tree looking for ones which
  // use the given render slot.  If a sequence doesn't use the slot then we
  // can safely assume that none of its children do.

  SequencePntr = LI_SEQNCR_CommonRuntimeTreeRoot->child;
  while (SequencePntr != NULL)
  {
    NextSequencePntr = SequencePntr->sibling;

    if (SequencePntr->renderSlotSet & RenderSlotSetToDelete)
    {
      MarkChainedRelatives (
        SequencePntr->dataID, SequencePntr->priority,
        TRUE /* StopChainedAncestors */,
        TRUE /* StopChainedDescendants */);

      MarkSelf (SequencePntr->dataID, SequencePntr->priority);

      DeleteSequenceAndChildren (SequencePntr);
    }
    SequencePntr = NextSequencePntr;
  }

  DeleteSequencesAndCancelMarkedPendingCommands (
    TRUE /* SearchWholeSequenceTree */);

  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
}



/*****************************************************************************
 * Process one of the user's commands.  Can optionally chain all kinds
 * of commands to another sequence.
 */

static void ProcessUserCommand (LE_QUEUE_MessagePointer UserCommandPntr)
{
  SeqCmdSetCameraAttachmentPointer            CameraDataPntr;
  SeqCmdFlagsAsLong                           CommandFlags;
  int                                         FindCount = 0;
  SeqCmdInvalidateRectAttachmentPointer       InvalidateRectDataPntr;
  LE_SEQNCR_RuntimeInfoPointer                NextSequencePntr;
  LE_SEQNCR_RuntimeInfoPointer                ParentSequencePntr;
  LE_REND_RenderSlot                          RenderSlot;
  LI_REND_AbstractRenderSlotPointer           RenderSlotPntr;
  LE_SEQNCR_RuntimeInfoPointer                SequencePntr;
  SeqCmdStart0DAttachmentPointer              StartCommonDataPntr;
  HRGN                                        TempRegion;
  SeqCmdSetViewportAttachmentPointer          ViewPortDataPntr;

  CommandFlags.asDWORD = UserCommandPntr->numberE;

  // Print the command to the log file, for debugging.  Only prints
  // them when the scroll lock key is held down.

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_EnableSystemKeybrd
  if (LI_KEYBRD_ScrollLockPressedForSequencer)
  {
    char  TempString [256];

    sprintf (TempString, "User command in batch %d, time %d: %-20s $%08X/%-5d $%08X/%-5d %s %3d%s%s%s%s%s%s\r\n",
      (int) EndOfUpdateCycleCounter,
      (int) LE_TIME_TickCount,
      CommandNames [UserCommandPntr->messageCode],
      (int) UserCommandPntr->numberA,
      (int) UserCommandPntr->numberB,
      (int) UserCommandPntr->numberC,
      (int) UserCommandPntr->numberD,
      EndingActionNames[CommandFlags.asFlags.SEQFLAG_ReplacementEndingAction],
      (int) CommandFlags.asFlags.SEQFLAG_ByteSizedValue,
      CommandFlags.asFlags.SEQFLAG_StopChainedAncestors ? " Ancestors" : "",
      CommandFlags.asFlags.SEQFLAG_StopChainedDescendants ? " Descendants" : "",
      CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree ? " SearchTree" : "",
      CommandFlags.asFlags.SEQFLAG_ForceRedrawEveryUpdate ? " ForceRedraw" : "",
      CommandFlags.asFlags.SEQFLAG_UseAbsoluteDataIDs ? " AbsoluteIDs" : "",
      DisposeNames [CommandFlags.asFlags.SEQFLAG_HowToDisposeSequenceDataWhenDone],
      CommandFlags.asFlags.SEQFLAG_UnrefSequenceDataWhenDone ? " UnRef" : "",
      CommandFlags.asFlags.SEQFLAG_ZeroBitmapOffset ? " IgnoreTABOffset" : "");

    LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);
  }
#endif

  // Chained commands go into the waiting for chained animations list.  If the
  // optional interrupt flag is on, the chainee is forced to stop (clock set
  // to end time).  This will make it start the chained items when it is
  // updated later on in this cycle.  NumberC is always the ChainToDataID
  // (LE_DATA_EmptyItem if not chained) and NumberD is always the
  // ChainToPriority, in all kinds of commands.

  if (UserCommandPntr->numberC /* ChainDataID */ != LE_DATA_EmptyItem)
  {
    if (CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence)
    {
      // Look for the sequence that this one chains to.  Can find
      // more than one with the same DataID/Priority.

      SequencePntr = NULL;
      while (NULL != (SequencePntr = FindNextSequence (
      UserCommandPntr->numberC /* ChainDataID */,
      (UNS16) UserCommandPntr->numberD /* ChainToPriority */,
      CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains,
      SequencePntr)))
      {
        // Force chained to sequence(s) to end and stop in this update.
        SequencePntr->sequenceClock = SequencePntr->endTime;
        SequencePntr->endingAction = LE_SEQNCR_EndingActionStop;
        SequencePntr->needsReEvaluation = TRUE;
        MarkAncestorsAsNeedingReEvaluation (SequencePntr);
        FindCount++;
#if !CE_ARTLIB_SeqncrChainStopsAllSameIDAndPri
        break; // This game can assume that there is at most one chainee.
#endif
      }

      if (FindCount == 0) // If didn't find one we were interrupting.
      {
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreChainIntrErrors
        sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
          "Command chained to DataID $%08X, Priority %d with the interrupt "
          "option enabled was unable to find that sequence to interrupt it.  "
          "The faulty command will be executed right now!\r\n",
          (int) UserCommandPntr->numberC, (int) UserCommandPntr->numberD);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
          #if CE_ARTLIB_SeqncrIgnoreChainIntrErrors == 1
          LE_ERROR_DebugMsgToFile);
          #else
          LE_ERROR_DebugMsgToFileAndScreen);
          #endif
#endif
          UserCommandPntr->numberC = LE_DATA_EmptyItem;
      }
    }

    // Do we still want to have this command wait for another to finish?
    // If so, add it to the pending commands queue.

    if (UserCommandPntr->numberC != LE_DATA_EmptyItem)
    {
      if (AddCommandToChainQueue (UserCommandPntr) == 0)
        goto ErrorExit; // Failed to add the command - out of memory or something.

      // The command will come out of the pending chains list later.
      return;
    }
  }

  // The command isn't being chained, process it right now.
  // First, is there any attached data?

  // Handle each command type...

  switch (UserCommandPntr->messageCode)
  {
  case SEQCMD_Start:
    StartCommonDataPntr = (SeqCmdStart0DAttachmentPointer) UserCommandPntr->stringA;
    if (StartCommonDataPntr == NULL ||
    StartCommonDataPntr->parentDataID == LE_DATA_EmptyItem)
      ParentSequencePntr = LI_SEQNCR_CommonRuntimeTreeRoot;
    else // Has specified a parent sequence.  Try finding it.
    {
      ParentSequencePntr = FindNextSequence (
        StartCommonDataPntr->parentDataID, StartCommonDataPntr->parentPriority,
        CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree, NULL);

      if (ParentSequencePntr == NULL)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
          "Start Sequence command for (DataID $%08X, Priority %d) couldn't "
          "find the parent sequence with (DataID $%08X, Priority %d).\r\n",
          (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB,
          (int) StartCommonDataPntr->parentDataID,
          (int) StartCommonDataPntr->parentPriority);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        goto ErrorExit;
      }
    }

    // OK, now start it up!

    SequencePntr = LI_SEQNCR_StartUpSequence (UserCommandPntr->numberA, 0,
      UserCommandPntr, ParentSequencePntr);

    if (SequencePntr == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "Start Sequence command for (DataID $%08X, Priority %d) wasn't "
        "able to start it.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      goto ErrorExit; // Failed to start, hope it isn't partially started.
    }
    break;



  case SEQCMD_Stop:
    if (0 == DeleteMatchingSequences (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree))
    {
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreStopErrors
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "Stop Sequence command for (DataID $%08X, Priority %d) "
        "didn't find a running sequence to stop.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreStopErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
#endif
    }

    MarkChainedRelatives (
      UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
      CommandFlags.asFlags.SEQFLAG_StopChainedAncestors,
      CommandFlags.asFlags.SEQFLAG_StopChainedDescendants);

    // Remove chained commands that start the given sequence.
    // If remove ancestors is selected, they get included as
    // ancestors.  If remove descendants is included, they
    // don't get removed, so we have to manually remove it.
    // If no ancestors or descendent chains are being cleaned
    // out, then don't change the chains.

    if (CommandFlags.asFlags.SEQFLAG_StopChainedDescendants)
      MarkSelf (UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB);

    DeleteSequencesAndCancelMarkedPendingCommands (
      CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree);
    break;



  case SEQCMD_StopAll:
    SequencePntr = LI_SEQNCR_CommonRuntimeTreeRoot->child;
    while (SequencePntr != NULL)
    {
      NextSequencePntr = SequencePntr->sibling;

      // Look for all top level sequences that match the
      // priority and render slot specified.

      if (SequencePntr->priority >= UserCommandPntr->numberB &&
      (SequencePntr->renderSlotSet & UserCommandPntr->numberA))
      {
        MarkChainedRelatives (
          SequencePntr->dataID, SequencePntr->priority,
          CommandFlags.asFlags.SEQFLAG_StopChainedAncestors,
          CommandFlags.asFlags.SEQFLAG_StopChainedDescendants);

        if (CommandFlags.asFlags.SEQFLAG_StopChainedDescendants)
          MarkSelf (SequencePntr->dataID, SequencePntr->priority);

        DeleteSequenceAndChildren (SequencePntr);
      }
      SequencePntr = NextSequencePntr;
    }
    DeleteSequencesAndCancelMarkedPendingCommands (
      CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree);
    break;



  case SEQCMD_Move:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      MoveSequence (SequencePntr, (void *) UserCommandPntr->stringA,
        UserCommandPntr->stringASize);
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreMoveErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "Move command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreMoveErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_ForgetChains:
    ForgetChains ();
    break;



  case SEQCMD_Pause:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      if (SequencePntr->paused && !CommandFlags.asFlags.SEQFLAG_ByteSizedValue)
      {
        // Switching from paused to unpaused, so restart it
        // as if no elapsed time has passed.

        ParentSequencePntr = SequencePntr->parent;
        if (ParentSequencePntr == NULL)
          SequencePntr->parentTimeOfLastUpdate = TopLevelClock;
        else
          SequencePntr->parentTimeOfLastUpdate =
          ParentSequencePntr->sequenceClock;
      }
      SequencePntr->paused = CommandFlags.asFlags.SEQFLAG_ByteSizedValue;
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnorePauseErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "Pause command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnorePauseErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_SetClock:
    if (UserCommandPntr->stringA == NULL ||
    UserCommandPntr->stringASize != sizeof (SeqCmdSetClockAttachmentRecord))
      break; // Missing the clock data.

    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      LI_SEQNCR_GoBackwardsInTime (SequencePntr,
      ((SeqCmdSetClockAttachmentPointer) UserCommandPntr->stringA)->newClock);
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetClockErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetClock command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetClockErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_SetEndingAction:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      SequencePntr->endingAction = (LE_SEQNCR_EndingAction)
        CommandFlags.asFlags.SEQFLAG_ReplacementEndingAction;
      SequencePntr->needsReEvaluation = TRUE; // In case it ends now.
      MarkAncestorsAsNeedingReEvaluation (SequencePntr);
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetEndingActErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetEndingAction command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetEndingActErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_DefaultRenderSlotSet:
    DefaultRenderSlotSetForStartSequence =
      (LE_REND_RenderSlotSet) UserCommandPntr->numberA;
    break;



  case SEQCMD_InvalidateRect:
    InvalidateRectDataPntr = (SeqCmdInvalidateRectAttachmentPointer)
      UserCommandPntr->stringA;
    if (InvalidateRectDataPntr != NULL)
    {
      LE_GRAFIX_RectDisplayToLogical (&InvalidateRectDataPntr->rectangle);
      TempRegion = CreateRectRgnIndirect (&InvalidateRectDataPntr->rectangle);
      if (TempRegion != NULL)
      {
        for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
        {
          if (InvalidateRectDataPntr->renderSlotSet & (1 << RenderSlot))
          {
            RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];

            if (RenderSlotPntr != NULL)
              RenderSlotPntr->AddInvalidArea (TempRegion,
              LE_GRAFIX_DDPrimarySurfaceN);
          }
        }
        DeleteObject (TempRegion);
      }
    }
    break;



  case SEQCMD_ForceRedraw:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      SequencePntr->needsRedraw = TRUE;
      MarkAncestorsAsNeedingReEvaluation (SequencePntr);
      SequencePntr->needsReEvaluation = TRUE;
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreForceRedrawErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "ForceRedraw command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreForceRedrawErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_SetVolume:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND)
      {
        if (SequencePntr->streamSpecificData.audioStream == NULL ||
        SequencePntr->streamSpecificData.audioStream->bufSnd == NULL)
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
            "SetVolume found NULL buffer stuff in plain sound, something is wrong!  "
            "DataID $%08X priority %d.\r\n",
            (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        }
        else
        {
          LE_SOUND_SetVolumeBufSnd (SequencePntr->streamSpecificData.audioStream->bufSnd,
            (UNS8) CommandFlags.asFlags.SEQFLAG_ByteSizedValue);
        }
      }
      else if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
      {
        GlueSetVideoVolume (SequencePntr,
          CommandFlags.asFlags.SEQFLAG_ByteSizedValue);
      }
      else // Not a sequence with sound (though its children may have sound...).
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
          "SetVolume only works on sound sequences, not DataID $%08X priority %d "
          "which has a sequence type %d.\r\n",
          (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB,
          (int) SequencePntr->sequenceType);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      }
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetVolumeErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetVolume command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetVolumeErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_SetPan:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND)
      {
        if (SequencePntr->streamSpecificData.audioStream == NULL ||
        SequencePntr->streamSpecificData.audioStream->bufSnd == NULL)
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
            "SetPan found NULL buffer stuff in plain sound, something is wrong!  "
            "DataID $%08X priority %d.\r\n",
            (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        }
        else
        {
          LE_SOUND_SetPanBufSnd (SequencePntr->streamSpecificData.audioStream->bufSnd,
            (INT8) CommandFlags.asFlags.SEQFLAG_ByteSizedValue);
        }
      }
      else if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
      {
        GlueSetVideoPan (SequencePntr,
          (INT8) CommandFlags.asFlags.SEQFLAG_ByteSizedValue);
      }
      else // Not a sequence with sound (though its children may have sound...).
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
          "SetPan only works on sound sequences, not DataID $%08X priority %d "
          "which has a sequence type %d.\r\n",
          (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB,
          (int) SequencePntr->sequenceType);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      }
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetPanErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetPan command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetPanErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_SetPitch:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND)
      {
        if (SequencePntr->streamSpecificData.audioStream == NULL ||
        SequencePntr->streamSpecificData.audioStream->bufSnd == NULL)
        {
#if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
            "SetPitch found NULL buffer stuff in plain sound, something is wrong!  "
            "DataID $%08X priority %d.\r\n",
            (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        }
        else
        {
          LE_SOUND_SetPitchBufSnd (SequencePntr->streamSpecificData.audioStream->bufSnd,
            256 * (UNS8) CommandFlags.asFlags.SEQFLAG_ByteSizedValue);
        }
      }
      else if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO)
      {
        GlueSetVideoPitch (SequencePntr,
          256 * (UNS8) CommandFlags.asFlags.SEQFLAG_ByteSizedValue);
      }
      else // Not a sequence with sound (though its children may have sound...).
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
          "SetPitch only works on sound sequences, not DataID $%08X priority %d "
          "which has a sequence type %d.\r\n",
          (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB,
          (int) SequencePntr->sequenceType);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      }
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetPitchErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetPitch command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetPitchErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_SetSaturation:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO &&
      SequencePntr->streamSpecificData.videoStream != NULL)
      {
        SequencePntr->streamSpecificData.videoStream->saturation =
          (INT8) CommandFlags.asFlags.SEQFLAG_ByteSizedValue;
      }
      else // Not a sequence with sound (though its children may have sound...).
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
          "SetSaturation only works on video sequences, not DataID $%08X priority %d "
          "which has a sequence type %d.\r\n",
          (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB,
          (int) SequencePntr->sequenceType);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      }
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetSaturationErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetSaturation command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetSaturationErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_SetBrightness:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO &&
      SequencePntr->streamSpecificData.videoStream != NULL)
      {
        SequencePntr->streamSpecificData.videoStream->brightness =
          (INT8) CommandFlags.asFlags.SEQFLAG_ByteSizedValue;
      }
      else // Not a sequence with sound (though its children may have sound...).
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
          "SetBrightness only works on video sequences, not DataID $%08X priority %d "
          "which has a sequence type %d.\r\n",
          (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB,
          (int) SequencePntr->sequenceType);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      }
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetBrightnessErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetBrightness command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetBrightnessErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_SetContrast:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO &&
      SequencePntr->streamSpecificData.videoStream != NULL)
      {
        SequencePntr->streamSpecificData.videoStream->contrast =
          (INT8) CommandFlags.asFlags.SEQFLAG_ByteSizedValue;
      }
      else // Not a sequence with sound (though its children may have sound...).
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
          "SetContrast only works on video sequences, not DataID $%08X priority %d "
          "which has a sequence type %d.\r\n",
          (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB,
          (int) SequencePntr->sequenceType);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      }
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetContrastErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetContrast command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetContrastErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_SetVideoAlternative:
    if (UserCommandPntr->stringA == NULL || UserCommandPntr->stringASize !=
    sizeof (SeqCmdSetVideoAlternativeAttachmentRecord))
      break; // Missing the attached data.

    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      GlueVideoSetUpAlternativePath (SequencePntr,
        ((SeqCmdSetVideoAlternativeAttachmentPointer)
        UserCommandPntr->stringA)->decisionFrameNumber,
        ((SeqCmdSetVideoAlternativeAttachmentPointer)
        UserCommandPntr->stringA)->jumpToFrameNumber);
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetUpVidAltErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetVideoAlternative command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetUpVidAltErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_ChooseVideoAlternative:
    if (UserCommandPntr->stringA == NULL || UserCommandPntr->stringASize !=
    sizeof (SeqCmdChooseVideoAlternativeAttachmentRecord))
      break; // Missing the attached data.

    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      GlueVideoChooseAlternativePath (SequencePntr,
        ((SeqCmdChooseVideoAlternativeAttachmentPointer)
        UserCommandPntr->stringA)->decisionFrameNumber,
        ((SeqCmdChooseVideoAlternativeAttachmentPointer)
        UserCommandPntr->stringA)->takeAlternative);
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreChooseVidAltErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "ChooseVideoAlternative command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreChooseVidAltErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;



  case SEQCMD_ForgetVideoAlternatives:
    SequencePntr = NULL;
    while (NULL != (SequencePntr = FindNextSequence (
    UserCommandPntr->numberA, (UNS16) UserCommandPntr->numberB,
    CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree,
    SequencePntr)))
    {
      GlueVideoForgetAlternativePaths (SequencePntr);
      FindCount++;
    }

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreForgetVidAltErrors
    if (FindCount == 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "ForgetVideoAlternative command is unable to find DataID $%08X priority %d.\r\n",
        (int) UserCommandPntr->numberA, (int) UserCommandPntr->numberB);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreForgetVidAltErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;


  case SEQCMD_SetViewport:
    ViewPortDataPntr = (SeqCmdSetViewportAttachmentPointer)
      UserCommandPntr->stringA;

    if (ViewPortDataPntr != NULL &&
    (RenderSlot = ViewPortDataPntr->renderSlotNumber) < CE_ARTLIB_RendMaxRenderSlots &&
    (RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot]) != NULL &&
    RenderSlotPntr->SetViewport (&ViewPortDataPntr->viewportRect))
    {
      // Success!
    }
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetViewportErrors
    else
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetViewport command failed (bad render slot or something).\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetViewportErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;


  case SEQCMD_SetCamera:
    CameraDataPntr = (SeqCmdSetCameraAttachmentPointer)
      UserCommandPntr->stringA;

    if (CameraDataPntr != NULL &&
    (RenderSlot = CameraDataPntr->renderSlotNumber) < CE_ARTLIB_RendMaxRenderSlots &&
    (RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot]) != NULL &&
    RenderSlotPntr->SetCamera (CameraDataPntr->cameraNumber,
    &CameraDataPntr->position,
    &CameraDataPntr->forwards,
    &CameraDataPntr->up,
    CameraDataPntr->fovAngle,
    CameraDataPntr->nearZ,
    CameraDataPntr->farZ))
    {
      // Success!
    }
#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_SeqncrIgnoreSetCameraErrors
    else
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
        "SetCamera command failed (bad render slot or something).\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
        #if CE_ARTLIB_SeqncrIgnoreSetCameraErrors == 1
        LE_ERROR_DebugMsgToFile);
        #else
        LE_ERROR_DebugMsgToFileAndScreen);
        #endif
    }
#endif
    break;


  default:
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "L_SEQNCR.c ProcessUserCommand: "
      "Unknown command #%d.\r\n", (int) UserCommandPntr->messageCode);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    break;
  }
  return; // Normal exit.


ErrorExit:
  CancelCommand (UserCommandPntr);
}



/*****************************************************************************
 * Run one cycle of the sequencer rendering process.  The amount of time to
 * advance the clock is given as a parameter, so that you can do high
 * quality non-real-time off-line rendering, if you want to.  The time is in
 * is in 1/60 second units.
 */

void LE_SEQNCR_DoUpdateCycle (UNS32 TimeIncrement)
{
  ChainedItemPointer                              DeChainedPntr;
  LE_QUEUE_MessagePointer                         DeChainedCommandPntr;
  BOOL                                            MoreCommandsToProcess;
  LE_SEQNCR_RuntimeInfoPointer                    RootNode;
  LE_REND_RenderSlot                              RenderSlot;
  LI_REND_AbstractRenderSlotPointer               RenderSlotPntr;
  LE_SEQNCR_RuntimeInfoPointer                    SequencePntr;
  LE_QUEUE_MessageRecord                          UserCommand;

  // Grab a copy of the root of the tree, and use the copy.  This is because
  // the global root may turn to NULL during a system shutdown (but the data
  // is still there so we can use our root copy to finish the update cycle).

  RootNode = LI_SEQNCR_CommonRuntimeTreeRoot;
  if (RootNode == NULL)
  {
    // Counts number of cycles, even when the system is off.  Needed by
    // the shutdown code that makes sure that the system knows it has
    // been shut down (not in the middle of an update).  Note that this
    // variable is incremented at the END of a cycle, when everything
    // has been done that will be done.

    EndOfUpdateCycleCounter++;
    return; // Nothing to do.  Sequence system is inactive / shut down.
  }

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  // Sometimes the shutdown system yanks the carpet from under our feet
  // before we realise it.  Recheck the system validity now that we are
  // inside the critical section.  Abort if things are shutting down.

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
  {
    LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
    return;
  }

  // Update the parent clock for the root node.

  TopLevelClock += TimeIncrement;

  // Clear all the redraw flags.

  SequencePntr = RootNode;
  while (TRUE)
  {
    // Do the current node.

    SequencePntr->needsRedraw = FALSE;

    // Traverse down the tree, then do siblings, then back up to the
    // next parent's sibling.

    if (SequencePntr->child != NULL)
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

  // Process commands and update the sequences.  May take several passes,
  // but always do at least one pass.

#if CE_ARTLIB_EnableDebugMode
  if (LE_QUEUE_CurrentSize (&SequencerCommandQueue) >=
  CE_ARTLIB_SeqncrMaxCollectedCommands)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_DoUpdateCycle: "
      "The queue with user commands to the sequencer is full.  "
      "That's %ld entries!  What are you doing?\r\n",
      (long) CE_ARTLIB_SeqncrMaxCollectedCommands);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
#endif

  do
  {
    MoreCommandsToProcess = FALSE;

    // Process all pending user commands, if enabled (the command execution
    // can be paused to build up a batch of commands in the buffer).

    while (CollectCommandsNestingLevel <= 0 &&
    LE_QUEUE_Dequeue (&SequencerCommandQueue, &UserCommand))
    {
      ProcessUserCommand (&UserCommand);
      LE_QUEUE_FreeStorageAssociatedWithMessage (&UserCommand);
    }

    // Process dechained commands (ones released from a chain).
    // Since they were just triggered by a chain, submit them
    // without a chain setting (chain to DataID is LE_DATA_EmptyItem).
    // Do them in the same order as they were originally submitted
    // (take them off the tail of the queue).

    while (ExecutableChainedCommandsTail != NULL)
    {
      // Remove a command from the executable chained commands list tail.
      // We are traversing from tail to head.  Leave the queue in a
      // consistent state in case the command processing uses it.

      DeChainedPntr = ExecutableChainedCommandsTail;
      ExecutableChainedCommandsTail = DeChainedPntr->previous;
      if (ExecutableChainedCommandsTail != NULL)
        ExecutableChainedCommandsTail->next = NULL;
      else // Queue is now empty.
      {
#if CE_ARTLIB_EnableDebugMode
        if (ExecutableChainedCommandsHead != DeChainedPntr)
          LE_ERROR_ErrorMsg ("LE_SEQNCR_DoUpdateCycle: "
          "Executing commands queue has corrupt head!\r\n");
#endif
        ExecutableChainedCommandsHead = NULL;
      }
      DeChainedPntr->previous = NULL;
#if CE_ARTLIB_EnableDebugMode
      if (DeChainedPntr->next != NULL)
        LE_ERROR_ErrorMsg ("LE_SEQNCR_DoUpdateCycle: "
        "Executing commands queue is corrupt at tail node!\r\n");
#endif
      DeChainedCommandPntr = &DeChainedPntr->message;

      // Process the command.

      DeChainedCommandPntr->numberC = LE_DATA_EmptyItem; // Set ChainDataID.
      ProcessUserCommand (DeChainedCommandPntr);
      LE_QUEUE_FreeStorageAssociatedWithMessage (DeChainedCommandPntr);

      // Add the command to the free list.

      memset (DeChainedPntr, 0, sizeof (ChainedItemRecord));
      DeChainedPntr->next = FreeChainedCommands;
      FreeChainedCommands = DeChainedPntr;
    }

    // Update the positions of all sequences, and the births or deaths
    // of their ephemeral children.  Note that this may be called several
    // times in an update due to releasing of chained commands, but that
    // should be OK since old sequences will detect that no time has
    // passed and thus won't waste time updating, unless they need to
    // because changes have set their needsReEvaluation flag.

    LI_SEQNCR_UpdateSequenceClock (RootNode);

    // Update MoreCommandsToProcess here since the update sequence
    // may have released some pending chained commands when sequences
    // have stopped.

    if (ExecutableChainedCommandsTail != NULL)
      MoreCommandsToProcess = TRUE;

  } while (MoreCommandsToProcess);

  // Now do the rendering.  Sound and graphics and everything else.
  // First get the slots to update their invalid areas and camera
  // settings.  The slots are in order from bottom to top, so one
  // that overlaps another can rely on the order - it can get info
  // about ones on top that are still in the previous frame and it
  // can set invalid areas in ones below, which are in the current frame.
  // This is particularly useful for having the mouse with its own
  // render slot / camera running independently of the other render
  // slots except for the exchange of invalid areas.

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
  {
    RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];

    if (RenderSlotPntr != NULL)
      RenderSlotPntr->UpdateInvalidAreas (RootNode);
  }

  // Now have the slots actually draw their stuff on the screen, using
  // the invalid areas they have just updated.  Again in bottom to top
  // order for overlapping render slots.

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
  {
    RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];

    if (RenderSlotPntr != NULL)
      RenderSlotPntr->DrawInvalidAreas (RootNode);
  }

  // Dump the list of running sequences to the debug file if the user
  // hits the scroll-lock key.  Hold down shift if you want the extra
  // information in the dump.  Beeps when done.

#if CE_ARTLIB_EnableDebugMode && CE_ARTLIB_EnableSystemKeybrd
  if (LI_KEYBRD_ScrollLockLatchedForSequencer)
  {
    LI_KEYBRD_ScrollLockLatchedForSequencer = FALSE;
    LE_ERROR_DebugMsg ("Sequence dump on user request (scroll-lock key was pressed):\r\n",
      LE_ERROR_DebugMsgToFile);
    LE_SEQNCR_DumpActiveSequencesToDebugFile (LI_KEYBRD_ScrollLockShiftForSequencer);
    MessageBeep ((DWORD) -1);
  }
#endif

  // Count cycles and let others know that we have reached the end,
  // where no more processing of any kind will be done (the shutdown
  // routines can now safely yank out the global tree and delete the
  // critical section structure).

  EndOfUpdateCycleCounter++;

  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
}



/*****************************************************************************
 * This function gets called 60 times per second (or less often if we are
 * slipping behind time) by the timer module, or more often if we are
 * in single tasking mode (as fast as the main loop runs).  Since the timer
 * starts before and shuts down after the sequencer, this can be called while
 * the sequencer is inactive.
 */

void LI_SEQNCR_TimerTick (void)
{
  INT32             ElapsedTime;
  static TYPE_Tick  LastUpdateTime;
  TYPE_Tick         SnapShot;

#if CE_ARTLIB_SeqncrShowFrameRatePeriod && CE_ARTLIB_EnableDebugMode
  DDBLTFX           BlitEffects;
  TYPE_Rect         DestRect;
  TYPE_Tick         ElapsedFrameRateTime;
  UNS16             ElapsedFrameRateCount;
  HRESULT           ErrorCode;
  static TYPE_Tick  LastFrameRateDisplayTime;
  static UNS16      LastFrameRateFrameCount;
#endif

  if (SuspendEngine || LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
  {
    EndOfUpdateCycleCounter++; // So shutdown code works even when suspended.
    return; // Not currently drawing, usually someone else is using screen.
  }

  SnapShot = LE_TIME_TickCount;
  ElapsedTime = SnapShot - LastUpdateTime;
  ElapsedTime = ElapsedTime * EngineTimeMultiplierScaledBy16 / 16;
  if (ElapsedTime < 1)
    return;

  LastUpdateTime = SnapShot;

  // If more than one second has gone by since the last update, then we have
  // fallen very far behind time.  This happens when halted at a breakpoint
  // in the debugger.  Rather than skipping frames, pretend that only 1 tick
  // passed.

  if (ElapsedTime > CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ)
    ElapsedTime = 1;

  LE_SEQNCR_DoUpdateCycle (ElapsedTime);

#if CE_ARTLIB_SeqncrShowFrameRatePeriod && CE_ARTLIB_EnableDebugMode
  ElapsedFrameRateTime = SnapShot - LastFrameRateDisplayTime;
  if (ElapsedFrameRateTime >= CE_ARTLIB_SeqncrShowFrameRatePeriod)
  {
    ElapsedFrameRateCount = EndOfUpdateCycleCounter - LastFrameRateFrameCount;
    if (ElapsedFrameRateCount > ElapsedFrameRateTime)
      ElapsedFrameRateCount = (UNS16) ElapsedFrameRateTime; // Ignore overdrive.

    // Reset counters for the next measurement.

    LastFrameRateFrameCount = EndOfUpdateCycleCounter;
    LastFrameRateDisplayTime = SnapShot;

#if 0 // Don't really want to print the frame rate.
    sprintf (LE_ERROR_DebugMessageBuffer, "Frame rate is %f frames per second.\r\n",
      (double) ElapsedFrameRateCount / ElapsedFrameRateTime *
      CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ);
    OutputDebugString (LE_ERROR_DebugMessageBuffer);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

    // And now for a graphical representation of the scroll rate as a
    // vertical line down the left side of the window.  If it is all white,
    // then you are getting all the frames (none skipped), which corresponds
    // to CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ frames per second (normally 60).

    if (LE_GRAFIX_DDPrimarySurfaceN != NULL &&
    (LE_GRAFIX_ScreenDestinationRect.bottom - LE_GRAFIX_ScreenDestinationRect.top) >= 1 &&
    (LE_GRAFIX_ScreenDestinationRect.right - LE_GRAFIX_ScreenDestinationRect.left) >= 2)
    {
#if CE_ARTLIB_EnableMultitasking
      if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return; // System is closed.
      EnterCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
      if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) return; // Closed while we waited.
#endif

      ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->SetClipper (
        LI_GRAFIX_WindowVisibleRegionClipper);

      memset (&BlitEffects, 0, sizeof (BlitEffects));
      BlitEffects.dwSize = sizeof (BlitEffects);
      BlitEffects.dwFillColor = 0; // Black

      DestRect = LE_GRAFIX_ScreenDestinationRect;
      DestRect.right = DestRect.left + 2;
      DestRect.bottom = LE_GRAFIX_ScreenDestinationRect.bottom -
        (LE_GRAFIX_ScreenDestinationRect.bottom - LE_GRAFIX_ScreenDestinationRect.top) *
        ElapsedFrameRateCount / ElapsedFrameRateTime;

      ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->Blt (
        &DestRect, NULL, NULL,
        DDBLT_WAIT | DDBLT_COLORFILL, &BlitEffects);

      BlitEffects.dwFillColor = (DWORD) -1; // White.
      DestRect.top = DestRect.bottom;
      DestRect.bottom = LE_GRAFIX_ScreenDestinationRect.bottom;

      ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->Blt (
        &DestRect, NULL, NULL,
        DDBLT_WAIT | DDBLT_COLORFILL, &BlitEffects);

      ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->SetClipper (NULL);

#if CE_ARTLIB_EnableMultitasking
      LeaveCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
#endif
    }
  }
#endif
}



#if CE_ARTLIB_EnableMultitasking
/*****************************************************************************
 * Wait for a full update cycle to go by.  If one is in progress when this
 * function is called, ignore it and wait for the next to finish then return.
 */

void LE_SEQNCR_WaitForFullUpdateCycle (void)
{
  UNS16 StartingCycleCount;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return; // System isn't running or has stopped.

  // Make sure that the sequencer isn't in the middle of an update.  If it
  // is, then wait for it to finish (and update the cycle count before it
  // leaves the critical section).

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
  StartingCycleCount = EndOfUpdateCycleCounter;
  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  while (TRUE)
  {
    if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
      break; // System isn't running or has stopped.

    if (EndOfUpdateCycleCounter != StartingCycleCount)
      break; // It has done an animation update.

    LE_TIMER_Delay (1); // Sleep until next global tick.
  }
}
#endif // CE_ARTLIB_EnableMultitasking



#if CE_ARTLIB_EnableMultitasking
/*****************************************************************************
 * Wait for the given sequence to end.  Returns TRUE if it found it and it
 * has hit the end of time (happens at the end of every loop of a looping
 * sequence) or if it gets deleted while waiting for it to end.  Returns
 * FALSE if it couldn't find the sequence.
 */

BOOL LE_SEQNCR_WaitForSequenceToHitEnd (LE_DATA_DataId DataID, UNS16 Priority,
BOOL SearchWholeTree)
{
  BOOL                          HasEnded;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;
  BOOL                          WasFound = FALSE;

  LE_SEQNCR_WaitForFullUpdateCycle ();

  while (TRUE)
  {
    if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
      return FALSE; // System isn't running or has stopped.

    EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

    SequencePntr = FindNextSequence (DataID, Priority, SearchWholeTree, NULL);

    if (SequencePntr == NULL)
      HasEnded = FALSE;
    else
    {
      WasFound = TRUE;
      HasEnded = (SequencePntr->sequenceClock >= SequencePntr->endTime);
    }

    LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

    if (HasEnded)
      return TRUE;  // It exists and is at the end time.

    // TRUE if it was there and has gone away.  FALSE if it never was there.

    if (SequencePntr == NULL)
      return WasFound;

    LE_SEQNCR_WaitForFullUpdateCycle ();
  }
}
#endif // CE_ARTLIB_EnableMultitasking



/*****************************************************************************
 * Returns TRUE if a sequence has ended or is not playing.  Returns FALSE
 * if a sequence is playing.  Not to useful for looping sequences and
 * ones with infinite ending times, they never finish.
 */

BOOL LE_SEQNCR_IsSequenceFinished (LE_DATA_DataId DataID, UNS16 Priority,
BOOL SearchWholeTree)
{
  BOOL                          HasEnded;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;
  BOOL                          WasFound = FALSE;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return TRUE; // System isn't running or has stopped.

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  SequencePntr = FindNextSequence (DataID, Priority, SearchWholeTree, NULL);

  if (SequencePntr == NULL)
    HasEnded = FALSE;
  else
  {
    WasFound = TRUE;
    HasEnded = (SequencePntr->sequenceClock >= SequencePntr->endTime);
  }

  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  if (HasEnded || (SequencePntr == NULL))
    return TRUE;  // It exists and is at the end time or it does not exist.

  // It exists and is not finished.

  return FALSE;
}



/*****************************************************************************
 * Test to see if all top level sequences are finished, in the given render
 * slot set (defaults to the default slot the user uses).  Returns FALSE if
 * there is one which hasn't hit its ending time, or if there are pending
 * chained commands.  Ignores looping and other sequences which never end.
 */

BOOL LE_SEQNCR_AreAllSequencesFinished (
LE_REND_RenderSlotSet RenderSlotSet /* = 0 */)
{
  BOOL                          FoundBusySequence;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return TRUE; // System is shut down.  All finished.

  if (WaitingChainedCommandsHead != NULL)
    return FALSE; // Some pending chained commands exist.

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  SequencePntr = LI_SEQNCR_CommonRuntimeTreeRoot;
  if (SequencePntr == NULL)
    return FALSE; // System was shut down while we were waiting.

  // If the user didn't specify a render slot, use the default set,
  // which should hopefully avoid examining the mouse pointer.

  if (RenderSlotSet == 0)
    RenderSlotSet = DefaultRenderSlotSetForStartSequence;

  // Spin through all top level sequences, looking for ones which
  // haven't hit their end time.

  FoundBusySequence = FALSE;
  SequencePntr = SequencePntr->child;

  while (!FoundBusySequence && SequencePntr != NULL)
  {
    if ((SequencePntr->renderSlotSet & RenderSlotSet) &&
    SequencePntr->endTime < INFINITE_END_TIME)
    {
      // This is worth looking at, it is being rendered and it
      // is the kind that changes over time (not a static bitmap
      // or wave file, which is uninteresting to our users).

      if (SequencePntr->endingAction == LE_SEQNCR_EndingActionStop)
      {
        // Need to wait for this one to stop.
        FoundBusySequence = TRUE;
      }

      if (SequencePntr->endingAction == LE_SEQNCR_EndingActionStayAtEnd &&
      SequencePntr->sequenceClock < SequencePntr->endTime)
      {
        // Need to wait for this one to reach the end.
        FoundBusySequence = TRUE;
      }
    }
    SequencePntr = SequencePntr->sibling;
  }

  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  return !FoundBusySequence;
}



#if CE_ARTLIB_EnableMultitasking
/*****************************************************************************
 * Waits for all the sequences at the top level in the given render slot to
 * be finished.  If the timeout limit isn't zero, then it will also stop
 * waiting after that much time has passed (in debug mode it prints a
 * warning message).
 */

void LE_SEQNCR_WaitForAllSequencesFinished (
LE_REND_RenderSlotSet RenderSlotSet /* = 0 */,
TYPE_Tick TimeoutLimit /* = CE_ARTLIB_SeqncrWaitForAllFinishDelay */)
{
  TYPE_Tick StartTime;

  StartTime = LE_TIME_TickCount;

  // Make sure the user's pending commands are executed, if any.

  if (CollectCommandsNestingLevel <= 0 &&
  LE_QUEUE_CurrentSize (&SequencerCommandQueue) > 0)
    LE_SEQNCR_WaitForFullUpdateCycle ();

  while (TRUE)
  {
    if (TimeoutLimit > 0 && (LE_TIME_TickCount - StartTime) >= TimeoutLimit)
    {
#if CE_ARTLIB_EnableDebugMode
      LE_ERROR_DebugMsg ("LE_SEQNCR_WaitForAllSequencesFinished: "
        "Time limit has been reached, is something wrong?\r\n",
        LE_ERROR_DebugMsgToFileAndScreen);
#endif
      break; // Ran out of time, stop waiting.
    }

    if (LE_SEQNCR_AreAllSequencesFinished (RenderSlotSet))
      break; // They are all finished now!

    LE_SEQNCR_WaitForFullUpdateCycle ();
  }
}
#endif // CE_ARTLIB_EnableMultitasking



/*****************************************************************************
 * Call this with TRUE to make the animation engine stop updating.  All the
 * animation state stays, but it doesn't advance time or draw on the screen.
 * You can also queue up commands and do a few other things while it is
 * suspended.  This is intended for hacks which take over the screen to
 * do something else.  When it resumes, it redraws the whole screen.
 */

void LE_SEQNCR_SuspendEngine (BOOL SuspendIt)
{
  if (!SuspendIt)
    LE_SEQNCR_InvalidateScreenRect (NULL);

  SuspendEngine = SuspendIt;

  // Make sure that the engine has been suspended before returning,
  // the update cycle stuff still works while it is suspended (it
  // just does nothing on each cycle).

#if CE_ARTLIB_EnableMultitasking
  LE_SEQNCR_WaitForFullUpdateCycle ();
#endif
}



/*****************************************************************************
 * Changes the engine's frame rate multiplier, to make things go N times
 * faster than normal.
 */

void LE_SEQNCR_EngineTimeMultiplier (UNS8 TimeMultipleScaledBy16)
{
  EngineTimeMultiplierScaledBy16 = TimeMultipleScaledBy16;
}



#if CE_ARTLIB_EnableDebugMode
/*****************************************************************************
 * Prints the sequence tree and a list of chained commands to the debug file.
 */

void LE_SEQNCR_DumpActiveSequencesToDebugFile (BOOL PrintExtraData)
{
  int                           AntiLevel;
  SeqCmdFlagsAsLong             CommandFlags;
  ChainedItemPointer            CurrentChainPntr;
  int                           NestLevel;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;
  char                          TempString [512];
  int                           TypeNameIndex;
  UNS32                         NumberOf0DSequences = 0;
  UNS32                         NumberOf2DSequences = 0;
  UNS32                         NumberOf3DSequences = 0;

  SequencePntr = LI_SEQNCR_CommonRuntimeTreeRoot;
  if (SequencePntr == NULL)
  {
    LE_ERROR_DebugMsg ("LE_SEQNCR_DumpActiveSequencesToDebugFile: "
      "Sequencer is closed (uninitialised).\r\n.", LE_ERROR_DebugMsgToFile);
    return; // System is closed.
  }

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  sprintf (TempString, "LE_SEQNCR_DumpActiveSequencesToDebugFile: "
    "Sequence tree at top level clock of %d.\r\n", (int) TopLevelClock);
  LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);

  LE_ERROR_DebugMsg ("D=Dimensionality, TM=Time Multiple, PST=Parent Start Time, SC=Sequence Clock,\r\n",
    LE_ERROR_DebugMsgToFile);

  LE_ERROR_DebugMsg ("EA=Ending Action, RSS=Render Slot Set, LN=Label Number, WI=Watch Index, P=Paused\r\n",
    LE_ERROR_DebugMsgToFile);

  LE_ERROR_DebugMsg (" Sequence-Type   DataID.../Pri Offset ChildID@Offset ContentID "
    "D TM  PST SC    EA   End Time   RSS LN  WI  P\r\n", LE_ERROR_DebugMsgToFile);

  NestLevel = 0;

  while (SequencePntr != NULL)
  {
    // A little bit of statistics collecting.

    if (SequencePntr->dimensionality == 3)
      NumberOf3DSequences++;
    else if (SequencePntr->dimensionality == 2)
      NumberOf2DSequences++;
    else // Zero and other dimensions.
      NumberOf0DSequences++;

    // Print the current sequence record.
    // Print a line of standard sequence information.

    TypeNameIndex = SequencePntr->sequenceType;
    if (TypeNameIndex >= LE_CHUNK_ID_SEQ_MAX)
      TypeNameIndex = 0; // Meaning unknown.

    AntiLevel = 6 - NestLevel;
    if (AntiLevel < 1) AntiLevel = 1;

    sprintf (TempString,
      "%*s%-9s%*s "           /* Sequence type and indenting */
      "$%08X/%-5d@%-4d "      /* DataID and priority and data offset */
      "$%08X@%-4d "           /* ChildID and offset */
      "$%08X "                /* ContentsID */
      "%d "                   /* Dimensionality */
      "%-3d "                 /* TimeMultiple */
      "%-3d %-5d %-4s %-10d " /* Start time, clock, ending action, end time */
      "$%02X %-3d "           /* RenderSlotSet, label */
      "%-3d %d\r\n",          /* Watch, paused */
      NestLevel + 1, " ", SequenceTypeNames[TypeNameIndex], AntiLevel, " ",
      (int) SequencePntr->dataID, (int) SequencePntr->priority, (int) SequencePntr->dataOffset,
      (int) SequencePntr->childSequencesDataID, (int) SequencePntr->childSequencesOffset,
      (int) SequencePntr->contentsDataID,
      (int) SequencePntr->dimensionality,
      (int) SequencePntr->timeMultiple,
      (int) SequencePntr->parentStartTime,
      (int) SequencePntr->sequenceClock,
      EndingActionNames[SequencePntr->endingAction],
      (int) SequencePntr->endTime,
      (int) SequencePntr->renderSlotSet,
      (int) SequencePntr->labelNumber,
      (int) SequencePntr->watchIndex,
      (int) SequencePntr->paused);
    LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);

    // Print two dimensional data if present and requested.

    if (PrintExtraData && SequencePntr->dimensionality == 2)
    {
      LE_ERROR_DebugMsg ("                 "
        "Bounding Box |World Transformation   |Sequence to Parent     |Tweeker Transform\r\n",
        LE_ERROR_DebugMsgToFile);

      sprintf (TempString, "                 "
        "L:%-4d R:%-4d|%7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f\r\n",
        (int) SequencePntr->dimensionSpecificData.d2->boundingBox.left,
        (int) SequencePntr->dimensionSpecificData.d2->boundingBox.right,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D._11,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D._12,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D._13,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D._11,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D._12,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D._13,
        (double) SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D._11,
        (double) SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D._12,
        (double) SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D._13);
      LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);

      sprintf (TempString, "                 "
        "T:%-4d B:%-4d|%7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f\r\n",
        (int) SequencePntr->dimensionSpecificData.d2->boundingBox.top,
        (int) SequencePntr->dimensionSpecificData.d2->boundingBox.bottom,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D._21,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D._22,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D._23,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D._21,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D._22,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D._23,
        (double) SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D._21,
        (double) SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D._22,
        (double) SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D._23);
      LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);

      sprintf (TempString, "                 "
        "             |%7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f\r\n",
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D._31,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D._32,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToWorldTransformation.matrix2D._33,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D._31,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D._32,
        (double) SequencePntr->dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D._33,
        (double) SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D._31,
        (double) SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D._32,
        (double) SequencePntr->dimensionSpecificData.d2->tweekerTransformation.matrix2D._33);
      LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);
    }

    if (PrintExtraData && SequencePntr->dimensionality == 3)
    {
      LE_ERROR_DebugMsg ("                 "
        "World Transformation           |Sequence to Parent             |Tweeker Transform\r\n",
        LE_ERROR_DebugMsgToFile);

      sprintf (TempString, "                 "
        "%7.2f %7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f %7.2f\r\n",
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._11,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._12,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._13,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._14,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._11,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._12,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._13,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._14,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._11,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._12,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._13,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._14);
      LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);

      sprintf (TempString, "                 "
        "%7.2f %7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f %7.2f\r\n",
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._21,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._22,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._23,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._24,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._21,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._22,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._23,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._24,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._21,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._22,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._23,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._24);
      LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);

      sprintf (TempString, "                 "
        "%7.2f %7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f %7.2f\r\n",
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._31,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._32,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._33,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._34,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._31,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._32,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._33,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._34,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._31,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._32,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._33,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._34);
      LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);

      sprintf (TempString, "                 "
        "%7.2f %7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f %7.2f|%7.2f %7.2f %7.2f %7.2f\r\n",
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._41,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._42,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._43,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToWorldTransformation.matrix3D._44,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._41,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._42,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._43,
        (double) SequencePntr->dimensionSpecificData.d3->sequenceToParentTransformation.matrix3D._44,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._41,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._42,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._43,
        (double) SequencePntr->dimensionSpecificData.d3->tweekerTransformation.matrix3D._44);
      LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);
    }

    // Traverse down the tree, then do siblings, then back up to the
    // next parent's sibling.

    if (SequencePntr->child != NULL)
    {
      SequencePntr = SequencePntr->child;
      NestLevel++;
    }
    else
    {
      while (SequencePntr != NULL && SequencePntr->sibling == NULL)
      {
        SequencePntr = SequencePntr->parent;
        NestLevel--;
      }
      if (SequencePntr == NULL)
        break; // All done.
      SequencePntr = SequencePntr->sibling;
    }
  }

  // Print the sequence count statistics.

  sprintf (TempString, "Have %ld 0D, %ld 2D and %ld 3D sequences.  Total %ld.\r\n",
    NumberOf0DSequences, NumberOf2DSequences, NumberOf3DSequences,
    NumberOf0DSequences + NumberOf2DSequences + NumberOf3DSequences);
  LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);

  sprintf (TempString, "Used/Max runtime records for common: %ld/%ld, "
    "2D: %ld/%ld, 3D: %ld/%ld\r\n",
    CE_ARTLIB_SeqncrMaxActiveSequences - NumberOfFreeCommonRuntimeElements, (long) CE_ARTLIB_SeqncrMaxActiveSequences,
    CE_ARTLIB_SeqncrMax2DSequences - NumberOfFree2DRuntimeElements, (long) CE_ARTLIB_SeqncrMax2DSequences,
    CE_ARTLIB_SeqncrMax3DSequences - NumberOfFree3DRuntimeElements, (long) CE_ARTLIB_SeqncrMax3DSequences);
  LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);

  // Now print out the chain queue, in execution order
  // (oldest submissions first, tail to head order).

  LE_ERROR_DebugMsg (" Pending chained commands in execution order:\r\n",
    LE_ERROR_DebugMsgToFile);

  LE_ERROR_DebugMsg (" ChainToID/Pri.  Command              NumberA../B"
    "     EndAct Byte Flags...\r\n", LE_ERROR_DebugMsgToFile);

  CurrentChainPntr = WaitingChainedCommandsTail;
  while (CurrentChainPntr != NULL)
  {
    CommandFlags.asDWORD = CurrentChainPntr->message.numberE;

    sprintf (TempString, " $%08X/%-5d %-20s $%08X/%-5d %s %3d%s%s%s%s%s%s\r\n",
      (int) CurrentChainPntr->message.numberC,
      (int) CurrentChainPntr->message.numberD,
      CommandNames [CurrentChainPntr->message.messageCode],
      (int) CurrentChainPntr->message.numberA,
      (int) CurrentChainPntr->message.numberB,
      EndingActionNames[CommandFlags.asFlags.SEQFLAG_ReplacementEndingAction],
      (int) CommandFlags.asFlags.SEQFLAG_ByteSizedValue,
      CommandFlags.asFlags.SEQFLAG_StopChainedAncestors ? " Ancestors" : "",
      CommandFlags.asFlags.SEQFLAG_StopChainedDescendants ? " Descendants" : "",
      CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree ? " SearchTree" : "",
      CommandFlags.asFlags.SEQFLAG_ForceRedrawEveryUpdate ? " ForceRedraw" : "",
      CommandFlags.asFlags.SEQFLAG_UseAbsoluteDataIDs ? " AbsoluteIDs" : "",
      DisposeNames [CommandFlags.asFlags.SEQFLAG_HowToDisposeSequenceDataWhenDone],
      CommandFlags.asFlags.SEQFLAG_UnrefSequenceDataWhenDone ? " UnRef" : "",
      CommandFlags.asFlags.SEQFLAG_ZeroBitmapOffset ? " IgnoreTABOffset" : "");

    LE_ERROR_DebugMsg (TempString, LE_ERROR_DebugMsgToFile);

    CurrentChainPntr = CurrentChainPntr->previous;
  }

  LE_ERROR_DebugMsg ("\r\n", LE_ERROR_DebugMsgToFile);

  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
}
#endif /* CE_ARTLIB_EnableDebugMode */



/*****************************************************************************
 * When this function gets called, no more commands will be taken from the
 * command queue until LE_SEQNCR_ExecuteCommands is called.  The queue
 * has room for about 100 pending commands.  Returns the new nesting level,
 * zero or less means executing, positive means collecting commands.
 */

int LE_SEQNCR_CollectCommands ()
{
#if !CE_ARTLIB_SeqncrImmediateCommandExecution
  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return 0; // System is closed.

  // Processing all commands causes an extra delay in game play,
  // only need it in the ExecuteCommands, so don't use it here any more.

  #if 0
    // If we will be turning off command execution, let all the
    // commands currently in the queue finish executing first.
    if (CollectCommandsNestingLevel == 0 &&
    LE_QUEUE_CurrentSize (&SequencerCommandQueue) != 0)
      LE_SEQNCR_ProcessUserCommands ();
  #endif

  return ++CollectCommandsNestingLevel;
#else
  return 0;
#endif
}



/*****************************************************************************
 * Resume execution of user commands in the command queue.  If changing from
 * collecting to executing, it waits for all the commands to be done.  Returns
 * the new nesting level, negative or zero means commands are being processed.
 * Positive means commands are being collected in the queue.
 */

int LE_SEQNCR_ExecuteCommands (void)
{
#if !CE_ARTLIB_SeqncrImmediateCommandExecution
  int NewNestingLevel;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return 0; // System is closed.

  NewNestingLevel = --CollectCommandsNestingLevel;

  #if CE_ARTLIB_EnableDebugMode
  if (NewNestingLevel < 0)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_ExecuteCommands: "
      "ExecuteCommands called more than CollectCommands, "
      "nesting level is now %d.\r\n", NewNestingLevel);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  }
  #endif

  if (NewNestingLevel == 0 &&
  LE_QUEUE_CurrentSize (&SequencerCommandQueue) != 0)
    LE_SEQNCR_ProcessUserCommands ();

  return CollectCommandsNestingLevel;
#else
  if (LE_QUEUE_CurrentSize (&SequencerCommandQueue) != 0)
    LE_SEQNCR_ProcessUserCommands ();
  return 0;
#endif
}



/*****************************************************************************
 * Returns the amount of space currently used in the command queue, as
 * a percentage.  0 means the queue is empty and you can submit lots more
 * commands, 100 means the queue is full and is rejecting commands.
 */

int LE_SEQNCR_CommandQueuePercentageFull (void)
{
  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return 100; // System is closed.

  return 100 * LE_QUEUE_CurrentSize (&SequencerCommandQueue) /
    CE_ARTLIB_SeqncrMaxCollectedCommands;
}



/*****************************************************************************
 * Queue up a command to make the system forget all the chained commands.
 */

BOOL LE_SEQNCR_ForgetChainsTheWorks (LE_DATA_DataId ChainToDataID,
UNS16 ChainToPriority, BOOL InterruptChainee, BOOL SearchWholeTreeForChains)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_ForgetChains, 0, 0,
    0, 0, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Simpler version of forgetting chains.
 */

BOOL LE_SEQNCR_ForgetChains (void)
{
  return LE_SEQNCR_ForgetChainsTheWorks (
    LE_DATA_EmptyItem, 0, FALSE, FALSE);
}



/*****************************************************************************
 * Invalidates the given screen rectangle, making the sequencer request that
 * all render slots affected by it do their redraw.  A NULL rectangle will
 * redraw the whole screen.  Returns TRUE if your command will be done.
 */

BOOL LE_SEQNCR_InvalidateScreenTheWorks (TYPE_RectPointer InvalidRectPntr,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
BOOL SearchWholeTreeForChains, LE_REND_RenderSlotSet RenderSlotSet)
{
  SeqCmdInvalidateRectAttachmentRecord  AttachedInfo;
  SeqCmdFlagsAsLong                     CommandFlags;
  BOOL                                  ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  if (InvalidRectPntr == NULL)
    InvalidRectPntr = &LE_GRAFIX_ScreenDestinationRect; // Full screen.

  if (InvalidRectPntr->right <= InvalidRectPntr->left ||
  InvalidRectPntr->bottom <= InvalidRectPntr->top)
    return TRUE; // Empty rectangle.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;

  AttachedInfo.rectangle = *InvalidRectPntr;
  AttachedInfo.renderSlotSet = RenderSlotSet;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_InvalidateRect, 0, 0,
    0, 0, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    (ACHAR *) &AttachedInfo, sizeof (AttachedInfo));

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Invalidate the given rectangle in screen coordinates, in all render slots.
 */

BOOL LE_SEQNCR_InvalidateScreenRect (TYPE_RectPointer InvalidRectPntr)
{
  return
    LE_SEQNCR_InvalidateScreenTheWorks (InvalidRectPntr,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    (LE_REND_RenderSlotSet) -1 /* All slots */);
}



/*****************************************************************************
 * Force the redraw of the requested sequence (or several if several have
 * the same DataID/Priority).  Returns TRUE if command submitted.
 */

BOOL LE_SEQNCR_ForceRedrawTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
BOOL SearchWholeTreeForChains, BOOL SearchWholeTreeForSequence)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_ForceRedraw, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Just redraw the given sequence, assuming it is a top level one and that
 * no chaining is being done.
 */

BOOL LE_SEQNCR_ForceRedraw (LE_DATA_DataId DataID, UNS16 Priority)
{
  return LE_SEQNCR_ForceRedrawTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE, FALSE /* TRUE to search all */);
}



/*****************************************************************************
 * Utility function to add a reference to a data item for preloading.
 */

#define MAX_PREPARE_UNDO 5000

static LE_DATA_DataId *DataIDsDoneArray; // Array of MAX_PREPARE_UNDO items.
static int DataIDsDoneCount = 0;
static BOOL DoAddRefFlag;
static CRITICAL_SECTION PreLoaderCriticalSection;

static BOOL PreloadDataItem (LE_DATA_DataId DataID)
{
  if (DataID == LE_DATA_EmptyItem)
    return TRUE; // No data, nothing to do.

  // Try adding the reference.  AddRef also loads it for us.

  if (DoAddRefFlag)
  {
    if (LE_DATA_AddRef (DataID) == 0)
      return FALSE; // Failed to add a reference.

    if (DataIDsDoneCount >= MAX_PREPARE_UNDO)
      return FALSE; // Sequence is too complex to do, too many frames in it.

    // OK, successful AddRef for this thing, record it for the undo.
    DataIDsDoneArray [DataIDsDoneCount++] = DataID;
  }
  else
  {
    // Try loading it.  Data file and memory items both.

    if (!LE_DATA_Load (DataID))
      return FALSE; // Unable to load this item into memory.
  }

  // Also recolour the item if it is a TAB and is in the auto-recolour range.

#if CE_ARTLIB_SeqncrMaxRecolourRanges > 0
  if (LE_DATA_GetCurrentDataType (DataID) == LE_DATA_DataUAP)
  {
    int                           i;
    LE_SEQNCR_AutoRecolourPointer RecolourPntr;

    for (i = 0, RecolourPntr = LE_SEQNCR_AutoRecolourRangesArray + 0;
    i < CE_ARTLIB_SeqncrMaxRecolourRanges;
    i++, RecolourPntr++)
    {
      if (RecolourPntr->firstInRange == LE_DATA_EmptyItem ||
      DataID < RecolourPntr->firstInRange ||
      DataID > RecolourPntr->lastInRange ||
      RecolourPntr->colourMapArray == NULL ||
      RecolourPntr->colourMapSize <= 0)
        continue; // Nope, don't want to recolour this one.

      LE_GRAFIXBulkRecolorTAB (DataID,
        RecolourPntr->colourMapArray, RecolourPntr->colourMapSize);
    }
  }
#endif /* CE_ARTLIB_SeqncrMaxRecolourRanges > 0 */

  return TRUE;
}



/*****************************************************************************
 * Go through all the chunks in a sequence and load the data items they
 * refer to.  Optionally does an AddRef for each one.  Some items may
 * get a reference count of more than one if they occur several times.
 * Note that this is best called by the user thread, since it can take a
 * long time to load data, and that would halt the animation if it was done
 * by the animation thread.  Also, keep in mind that things with AddRef
 * stay in memory until RemoveRef is called (so you could run out of memory).
 * If it fails, it will RemoveRef everything it did and return FALSE.
 */

BOOL LE_SEQNCR_PrepareSequenceData (LE_DATA_DataId DataID, BOOL DoAddRef)
{
  LE_SEQNCR_Sequence2DBitmapChunkPointer  BitmapChunk;
  LE_DATA_DataId                          BitmapID;
  void                                   *ChunkDataPntr;
  UNS32                                   ChunkSize;
  LE_CHUNK_ID                             ChunkType;
  LE_DATA_DataId                          DataIDsDoneStorage [MAX_PREPARE_UNDO];
  LE_SEQNCR_SequenceGroupingChunkRecord   GroupChunk;
  int                                     i;
  UNS32                                   ReadAmount;
  BOOL                                    ReturnCode = FALSE;
  LE_SEQNCR_SequenceSoundChunkPointer     SoundChunk;
  LE_DATA_DataId                          SoundID;
  LE_CHUNK_StreamID                       StreamID = -1;
  LE_DATA_DataType                        TypeOfDataID;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // Critical sections not set up yet.
  EnterCriticalSection (&PreLoaderCriticalSection);

  // Copy some stuff to globals for interfacing with the PreloadDataItem
  // function, because C doesn't have nested functions like other languages.
  // Also have the critical section to prevent interference with these
  // globals.

  DataIDsDoneArray = DataIDsDoneStorage + 0;
  DataIDsDoneCount = 0;
  DoAddRefFlag = DoAddRef;

  // Add a reference to the sequence itself, for non-chunky single item
  // things (like bitmaps or sounds).

  TypeOfDataID = LE_DATA_GetCurrentDataType (DataID);

  if (TypeOfDataID != LE_DATA_DataChunky)
  {
    // Ok, this is a preloadable single item type of sequence.

    if (!PreloadDataItem (DataID))
      goto ErrorExit; // Unable to load this item.
    ReturnCode = TRUE;
    goto NormalExit;
  }

  // Start parsing the chunky sequence.

  StreamID = LE_CHUNK_ReadFromDataID (DataID);
  if (StreamID < 0)
    goto ErrorExit;

  ChunkType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, &ChunkSize);
  if (ChunkType == LE_CHUNK_ID_NULL_STANDARD)
    goto ErrorExit; // IO error or not chunky data.

  // If the first top level chunk (usually there is only one, we ignore any
  // others that follow it) is not a grouping sequence then we are done,
  // happens for video chunks and custom bitmap chunks etc.  Note that we
  // don't want to preload videos since the sequence data gets discarded
  // by game code right after the video starts.

  if (ChunkType != LE_CHUNK_ID_SEQ_GROUPING)
  {
    LE_CHUNK_Ascend (StreamID);
    ReturnCode = TRUE;
    goto NormalExit;
  }

  ChunkSize = sizeof (GroupChunk);
  ReadAmount = LE_CHUNK_ReadChunkData (StreamID, &GroupChunk, ChunkSize);
  if (ReadAmount != ChunkSize)
    goto ErrorExit;

  while (TRUE)
  {
    ChunkType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, &ChunkSize);
    if (ChunkType == LE_CHUNK_ID_NULL_STANDARD)
      break; // Hit the end of the parent group or IO error.

    if (ChunkType < LE_CHUNK_ID_SEQ_MAX)
    {
      ChunkSize = LE_CHUNK_ChunkFixedDataSizes [ChunkType];
      ChunkDataPntr = LE_CHUNK_MapChunkData (StreamID, ChunkSize, &ReadAmount);
      if (ChunkDataPntr == NULL || ReadAmount != ChunkSize)
        goto ErrorExit;
    }
    else // Private chunk type, which we will ignore.
    {
      ChunkSize = 0;
      ChunkDataPntr = NULL;
    }

    switch (ChunkType)
    {
    case LE_CHUNK_ID_SEQ_2DBITMAP:
      BitmapChunk = (LE_SEQNCR_Sequence2DBitmapChunkPointer) ChunkDataPntr;
      if (BitmapChunk->commonHeader.absoluteDataIDs)
        BitmapID = BitmapChunk->bitmapDataID;
      else
        BitmapID = LE_DATA_IdWithFileFromParent (BitmapChunk->bitmapDataID, DataID);
      if (!PreloadDataItem (BitmapID))
        goto ErrorExit; // Unable to load this item.
      break;

    case LE_CHUNK_ID_SEQ_SOUND:
      SoundChunk = (LE_SEQNCR_SequenceSoundChunkPointer) ChunkDataPntr;
      if (SoundChunk->commonHeader.absoluteDataIDs)
        SoundID = SoundChunk->soundDataID;
      else
        SoundID = LE_DATA_IdWithFileFromParent (SoundChunk->soundDataID, DataID);
      if (!PreloadDataItem (SoundID))
        goto ErrorExit; // Unable to load this item.
      break;
    }

    /* Leave this child chunk and go on to the next one. */
    if (LE_CHUNK_Ascend (StreamID) == FALSE)
      goto ErrorExit;
  }

  /* Leave the LE_CHUNK_ID_SEQ_GROUPING chunk. */

  LE_CHUNK_Ascend (StreamID);

  // Preload the grouping sequence itself, since it is useful to have.

  if (!PreloadDataItem (DataID))
    goto ErrorExit; // Unable to load this item.

  ReturnCode = TRUE;
  goto NormalExit;


ErrorExit:
  for (i = 0; i < DataIDsDoneCount; i++)
    LE_DATA_RemoveRef (DataIDsDoneArray [i]);

#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_PrepareSequenceData: "
    "Failed after adding %d references.\r\n", (int) DataIDsDoneCount);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:
  if (StreamID >= 0)
    LE_CHUNK_CloseReadStream (StreamID);
  DataIDsDoneArray = NULL;
  LeaveCriticalSection (&PreLoaderCriticalSection);
  return ReturnCode;
}



/*****************************************************************************
 * Utility function to remove a reference to a data item for postloading.
 */

static BOOL DoRemRefFlag;
static LE_SEQNCR_DataDisposalOptions DoDisposeFlag;
static CRITICAL_SECTION PostLoaderCriticalSection;

static BOOL PostUnloadDataItem (LE_DATA_DataId DataID)
{
  if (DataID == LE_DATA_EmptyItem)
    return TRUE; // Nothing to do.

  // Try removing the reference, if requested.

  if (DoRemRefFlag)
    LE_DATA_RemoveRef (DataID);

  // Unload or deallocate or change priority of the data as requested.

  if (DoDisposeFlag == LE_SEQNCR_DATA_DISPOSE_DO_NOTHING)
    return TRUE;

  if (DoDisposeFlag == LE_SEQNCR_DATA_DISPOSE_UNLOAD)
  {
    if (LE_DATA_GetRef (DataID) == 0)
      LE_DATA_Unload (DataID); // May fail but we don't care.
    return TRUE;
  }

  if (DoDisposeFlag == LE_SEQNCR_DATA_DISPOSE_DEALLOCATE_CONTENTSDATAID)
    return LE_DATA_FreeRuntimeDataID (DataID);

  return FALSE; // Unknown disposal option.
}



/*****************************************************************************
 * Go through all the data used by a sequence and do an optional RemoveRef
 * and also an optional unload for each item with a zero reference count.
 */

BOOL LE_SEQNCR_CleanUpSequenceData (LE_DATA_DataId DataID,
BOOL DoRemoveRef, LE_SEQNCR_DataDisposalOptions DisposalOptions)
{
  LE_SEQNCR_Sequence2DBitmapChunkPointer  BitmapChunk;
  LE_DATA_DataId                          BitmapID;
  void                                   *ChunkDataPntr;
  UNS32                                   ChunkSize;
  LE_CHUNK_ID                             ChunkType;
  LE_SEQNCR_SequenceGroupingChunkRecord   GroupChunk;
  UNS32                                   ReadAmount;
  BOOL                                    ReturnCode = FALSE;
  LE_SEQNCR_SequenceSoundChunkPointer     SoundChunk;
  LE_DATA_DataId                          SoundID;
  LE_CHUNK_StreamID                       StreamID = -1;
  LE_DATA_DataType                        TypeOfDataID;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // Critical sections not set up yet.
  EnterCriticalSection (&PostLoaderCriticalSection);

  // Pass on the settings to our subroutine.

  DoRemRefFlag = DoRemoveRef;
  DoDisposeFlag = DisposalOptions;

  // Not much to do for non-chunky sequences (raw bitmaps etc).

  TypeOfDataID = LE_DATA_GetCurrentDataType (DataID);

  if (TypeOfDataID != LE_DATA_DataChunky)
  {
    // Ok, this was a preloadable single item type of sequence.

    if (!PostUnloadDataItem (DataID))
      goto ErrorExit; // Rarely happens.
    ReturnCode = TRUE;
    goto NormalExit;
  }

  // Start parsing the chunky sequence.

  StreamID = LE_CHUNK_ReadFromDataID (DataID);
  if (StreamID < 0)
    goto ErrorExit;

  ChunkType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, &ChunkSize);
  if (ChunkType == LE_CHUNK_ID_NULL_STANDARD)
    goto ErrorExit; // IO error or not chunky data.

  // If the first top level chunk (usually there is only one, we ignore any
  // others that follow it) is not a grouping sequence then we are done,
  // happens for video chunks and custom bitmap chunks etc.  Note that we
  // don't want to preload videos since the sequence data gets discarded
  // by game code right after the video starts.

  if (ChunkType != LE_CHUNK_ID_SEQ_GROUPING)
  {
    LE_CHUNK_Ascend (StreamID);
    ReturnCode = TRUE;
    goto NormalExit;
  }

  ChunkSize = sizeof (GroupChunk);
  ReadAmount = LE_CHUNK_ReadChunkData (StreamID, &GroupChunk, ChunkSize);
  if (ReadAmount != ChunkSize)
    goto ErrorExit;

  while (TRUE)
  {
    ChunkType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, &ChunkSize);
    if (ChunkType == LE_CHUNK_ID_NULL_STANDARD)
      break; // Hit the end of the parent group or IO error.

    if (ChunkType < LE_CHUNK_ID_SEQ_MAX)
    {
      ChunkSize = LE_CHUNK_ChunkFixedDataSizes [ChunkType];
      ChunkDataPntr = LE_CHUNK_MapChunkData (StreamID, ChunkSize, &ReadAmount);
      if (ChunkDataPntr == NULL || ReadAmount != ChunkSize)
        goto ErrorExit;
    }
    else // Private chunk type, which we will ignore.
    {
      ChunkSize = 0;
      ChunkDataPntr = NULL;
    }

    switch (ChunkType)
    {
    case LE_CHUNK_ID_SEQ_2DBITMAP:
      BitmapChunk = (LE_SEQNCR_Sequence2DBitmapChunkPointer) ChunkDataPntr;
      if (BitmapChunk->commonHeader.absoluteDataIDs)
        BitmapID = BitmapChunk->bitmapDataID;
      else
        BitmapID = LE_DATA_IdWithFileFromParent (BitmapChunk->bitmapDataID, DataID);
      if (!PostUnloadDataItem (BitmapID))
        goto ErrorExit; // Rarely happens.
      break;


    case LE_CHUNK_ID_SEQ_SOUND:
      SoundChunk = (LE_SEQNCR_SequenceSoundChunkPointer) ChunkDataPntr;
      if (SoundChunk->commonHeader.absoluteDataIDs)
        SoundID = SoundChunk->soundDataID;
      else
        SoundID = LE_DATA_IdWithFileFromParent (SoundChunk->soundDataID, DataID);
      if (!PostUnloadDataItem (SoundID))
        goto ErrorExit; // Unable to load this item.
      break;
    }

    /* Leave the child chunk and go on to the next one. */
    if (LE_CHUNK_Ascend (StreamID) == FALSE)
      goto ErrorExit;
  }

  /* Leave the LE_CHUNK_ID_SEQ_GROUPING chunk. */

  LE_CHUNK_Ascend (StreamID);

  LE_CHUNK_CloseReadStream (StreamID);
  StreamID = -1;

  // Remove the reference for the sequence itself.

  if (!PostUnloadDataItem (DataID))
    goto ErrorExit; // Rarely happens.

  ReturnCode = TRUE; // Successful!
  goto NormalExit;


ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_CleanUpSequenceData: "
    "Failed.\r\n");
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:
  if (StreamID >= 0)
    LE_CHUNK_CloseReadStream (StreamID);
  LeaveCriticalSection (&PostLoaderCriticalSection);
  return ReturnCode;
}



/*****************************************************************************
 * This controls the default render slot used when starting a sequence with
 * the functions that don't specify a render slot.
 */

BOOL LE_SEQNCR_SetDefaultRenderSlotSetForStartSequence (
LE_REND_RenderSlotSet RenderSlotSet)
{
  BOOL              ReturnCode;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_DefaultRenderSlotSet, 0, 0,
    RenderSlotSet, 0, LE_DATA_EmptyItem, 0, 0, NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * This just returns the current setting for the default render slot.
 */

LE_REND_RenderSlotSet LE_SEQNCR_GetDefaultRenderSlotSetForStartSequence (void)
{
  return DefaultRenderSlotSetForStartSequence;
}



/*****************************************************************************
 * This controls the default loading options used when starting any sequence
 * with the functions that don't specify the options.  PreloadData TRUE means
 * do the preloading, FALSE means that bitmaps etc get loaded when they are
 * displayed for the first time (which can make the animation jerky, but
 * starts quicker).  PostDisposeData describes what to do with the data in
 * the sequence when the sequence ends (all bitmaps and sounds, and maybe
 * other types if we get around to doing them, can be unloaded/deallocated or
 * have their memory priority lowered).  If UseReferenceCounts is TRUE then
 * the reference counts will be used to control unloading (so that a bitmap in
 * use by some other sequence that added a reference count doesn't get
 * accidentally unloaded).
 *
 * This command is done right away, not put into the command queue since
 * preloading is done before submitting the StartSequence command.
 */

BOOL LE_SEQNCR_SetDefaultLoaderOptionsForStartSequence (
BOOL PreloadData, LE_SEQNCR_DataDisposalOptions PostDisposeData,
BOOL UseReferenceCounts)
{
  DefaultPreloadFlagForStartSequence = (PreloadData != 0);

  DefaultUseRefFlagForStartSequence = (UseReferenceCounts != 0);

  if (PostDisposeData < 0 || PostDisposeData >= LE_SEQNCR_DATA_DISPOSE_MAX)
    DefaultDisposalFlagForStartSequence = LE_SEQNCR_DATA_DISPOSE_DO_NOTHING;
  else
    DefaultDisposalFlagForStartSequence = PostDisposeData;

  return TRUE;
}



/*****************************************************************************
 * This user callable function requests that a sequence be started at the
 * next update, or later on if chaining is being used.  The Matrix type
 * should match the dimensionality given, and the sequence should too.
 * Use zeroes to get default values (including NULL for matrix).
 * Use LE_DATA_EmptyItem for the chain and parent sequence if you don't
 * want to chain or if you want the default parent.
 * Returns TRUE if successfully enqueued.
 */

BOOL LE_SEQNCR_StartTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
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
LE_SEQNCR_DropFrames DropFramesMethod)
{
  SeqCmdStart0DAttachmentRecord   AuxData0D;
  SeqCmdStart2DAttachmentRecord   AuxData2D;
  SeqCmdStart3DAttachmentRecord   AuxData3D;
  SeqCmdStart0DAttachmentPointer  AuxDataCommonPntr;
  UNS16                           BinarySize;
  SeqCmdFlagsAsLong               CommandFlags;
  BOOL                            ReturnCode;
  LE_DATA_DataType                TypeOfDataID;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  // Do not attempt to start a NULL sequence
  if (DataID == LED_EI)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LE_SEQNCR_StartTheWorks: Cannot start a null data ID.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

#if CE_ARTLIB_EnableDebugMode
  if (MatrixDimensionality != 0 && MatrixDimensionality != 2 && MatrixDimensionality != 3)
  {
    LE_ERROR_DebugMsg ("LE_SEQNCR_StartTheWorks: Wrong number of dimensions.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  TypeOfDataID = LE_DATA_GetLoadedDataType (DataID);

#if CE_ARTLIB_EnableDebugMode
  if (TypeOfDataID == LE_DATA_DataUnknown)
  {
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_StartTheWorks: "
      "Invalid DataID $%08X provided.\r\n", (int) DataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  // Do the preloading if needed.

  if (PreloadDataUsedBySequence || UseReferenceCountsOnDataUsedBySequence)
  {
    if (!LE_SEQNCR_PrepareSequenceData (DataID,
    UseReferenceCountsOnDataUsedBySequence))
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_StartTheWorks: "
        "Unable to preload DataID $%08X.\r\n", (int) DataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return FALSE;
    }
  }

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForParent;
  CommandFlags.asFlags.SEQFLAG_ReplacementEndingAction = EndingAction;
  CommandFlags.asFlags.SEQFLAG_ForceRedrawEveryUpdate = ForceRedrawOnEveryUpdate;
  CommandFlags.asFlags.SEQFLAG_UseAbsoluteDataIDs = UseAbsoluteDataIDs;
  CommandFlags.asFlags.SEQFLAG_ByteSizedValue = LabelNumber;
  CommandFlags.asFlags.SEQFLAG_HowToDisposeSequenceDataWhenDone = PostDisposeDataUsedBySequence;
  CommandFlags.asFlags.SEQFLAG_UnrefSequenceDataWhenDone = UseReferenceCountsOnDataUsedBySequence;
  CommandFlags.asFlags.SEQFLAG_DropFramesOption = (unsigned int) DropFramesMethod;

  // Hack to make user specified bitmaps start at the origin, this
  // flag is ignored for sequences and other non-bitmaps.

  CommandFlags.asFlags.SEQFLAG_ZeroBitmapOffset = TRUE;

  if (MatrixDimensionality == 2 && MatrixPntr != NULL)
  {
    AuxDataCommonPntr = &AuxData2D.commonArguments;
    BinarySize = sizeof (AuxData2D);
  }
  else if (MatrixDimensionality == 3 && MatrixPntr != NULL)
  {
    AuxDataCommonPntr = &AuxData3D.commonArguments;
    BinarySize = sizeof (AuxData3D);
  }
  else // Unknown number of dimensions or no matrix, pretend it is zero.
  {
    AuxDataCommonPntr = &AuxData0D;
    BinarySize = sizeof (AuxData0D);
    MatrixDimensionality = 0;
  }

  // Can we use the defaults for everything?

  if (MatrixDimensionality == 0 && InitialClockOffset == 0 &&
  CallbackFunction == NULL && CallbackData == NULL &&
  ParentDataID == LE_DATA_EmptyItem && RenderSlotSet == 0 &&
  TimeMultiple == 0)
  {
    AuxDataCommonPntr = NULL;
  }
  else // Have to attach a record with non-default values.
  {
    memset (AuxDataCommonPntr, 0, sizeof (SeqCmdStart0DAttachmentRecord));
    AuxDataCommonPntr->initialClockOffset = InitialClockOffset;
    AuxDataCommonPntr->parentDataID = ParentDataID;
    AuxDataCommonPntr->parentPriority = ParentPriority;
    AuxDataCommonPntr->callBack = CallbackFunction;
    AuxDataCommonPntr->userData = CallbackData;
    AuxDataCommonPntr->renderSlotSet = RenderSlotSet;
    AuxDataCommonPntr->timeMultiple = TimeMultiple;

    // Copy the matrix to our attached record.

    if (MatrixDimensionality == 2)
      AuxData2D.positionMatrix = *((TYPE_Matrix2DPointer) MatrixPntr);
    else if (MatrixDimensionality == 3)
      AuxData3D.positionMatrix = *((TYPE_Matrix3DPointer) MatrixPntr);
  }

  // Submit it to the command queue.  Returns TRUE if successful.

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_Start, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    (ACHAR *) AuxDataCommonPntr, BinarySize);

  if (!ReturnCode)
  {
    // Oops, that has failed.  Undo the references that were added earlier.

    if (TypeOfDataID == LE_DATA_DataChunky &&
    UseReferenceCountsOnDataUsedBySequence)
    {
      LE_SEQNCR_CleanUpSequenceData (DataID,
        UseReferenceCountsOnDataUsedBySequence,
        LE_SEQNCR_DATA_DISPOSE_DO_NOTHING);
    }
  }

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Just start the given sequence at the given priority.  Its positioning
 * and other settings are the default made in the dope sheet.  Works for
 * any dimensionality too.
 */

BOOL LE_SEQNCR_Start (LE_DATA_DataId DataID, UNS16 Priority)
{
  return LE_SEQNCR_StartTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    LE_DATA_EmptyItem, 0, FALSE,
    0, NULL, 0, 0, FALSE, 0, FALSE, LE_SEQNCR_NoEndingAction,
    0, NULL, NULL,
    DefaultPreloadFlagForStartSequence, DefaultDisposalFlagForStartSequence,
    DefaultUseRefFlagForStartSequence, LE_SEQNCR_DropDefault);
}



/*****************************************************************************
 * Start the given sequence at the given priority with the given preloading
 * and unloading options.  Its positioning and other settings are the default
 * made in the dope sheet.  Works for any dimensionality too.
 */

BOOL LE_SEQNCR_StartLoadOpts (LE_DATA_DataId DataID, UNS16 Priority,
BOOL Preload, LE_SEQNCR_DataDisposalOptions DisposalOptions,
BOOL UseReferenceCounts)
{
  return LE_SEQNCR_StartTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    LE_DATA_EmptyItem, 0, FALSE,
    0, NULL, 0, 0, FALSE, 0, FALSE, LE_SEQNCR_NoEndingAction,
    0, NULL, NULL,
    Preload, DisposalOptions, UseReferenceCounts, LE_SEQNCR_DropDefault);
}



/*****************************************************************************
 * Just start it at the given position.
 */

BOOL LE_SEQNCR_StartXY (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Coord2D X, TYPE_Coord2D Y)
{
  return
    LE_SEQNCR_StartCXYSlotDrop (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE,
    X, Y, (LE_REND_RenderSlotSet) 0, LE_SEQNCR_DropDefault);
}



/*****************************************************************************
 * Start it at the given position with a drop frames option.
 */

BOOL LE_SEQNCR_StartXYDrop (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Coord2D X, TYPE_Coord2D Y, LE_SEQNCR_DropFrames DropFramesMethod)
{
  return
    LE_SEQNCR_StartCXYSlotDrop (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE,
    X, Y, (LE_REND_RenderSlotSet) 0, DropFramesMethod);
}



/*****************************************************************************
 * Start the sequence at the given position and use a callback function.
 */

BOOL LE_SEQNCR_StartXYCallbackSlot (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Coord2D X, TYPE_Coord2D Y,
LE_SEQNCR_UserCallbackFunctionPointer CallbackFunction, void *CallbackData,
LE_REND_RenderSlotSet SlotSet)
{
  TYPE_Matrix2D MatrixA;
  TYPE_Point2D  Offset;

  // Move it over by the translation amount.

  Offset.x = X;
  Offset.y = Y;
  LE_MATRIX_2D_SetTranslate (&Offset, &MatrixA);

  return
    LE_SEQNCR_StartTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    LE_DATA_EmptyItem, 0, FALSE,
    2, &MatrixA,
    0, 0, FALSE, 0, FALSE, LE_SEQNCR_NoEndingAction,
    SlotSet, CallbackFunction, CallbackData,
    DefaultPreloadFlagForStartSequence, DefaultDisposalFlagForStartSequence,
    DefaultUseRefFlagForStartSequence, LE_SEQNCR_DropDefault);
}



/*****************************************************************************
 * Start the 2D sequence at the given X,Y position, in the slot and using the
 * specified load and disposal options.  Useful for starting a native bitmap
 * (from ObjectCreate) and having it self destruct when the sequence stops.
 */

BOOL LE_SEQNCR_StartXYSlotLoadOpts (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Coord2D X, TYPE_Coord2D Y, LE_REND_RenderSlotSet SlotSet,
BOOL Preload, LE_SEQNCR_DataDisposalOptions DisposalOptions,
BOOL UseReferenceCounts)
{
  TYPE_Matrix2D MatrixA;
  TYPE_Point2D  Offset;

  // Move it over by the translation amount.

  Offset.x = X;
  Offset.y = Y;
  LE_MATRIX_2D_SetTranslate (&Offset, &MatrixA);

  return
    LE_SEQNCR_StartTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    LE_DATA_EmptyItem, 0, FALSE,
    2, &MatrixA,
    0, 0, FALSE, 0, FALSE, LE_SEQNCR_NoEndingAction,
    SlotSet, NULL, NULL,
    Preload, DisposalOptions, UseReferenceCounts, LE_SEQNCR_DropDefault);
}



/*****************************************************************************
 * The usual 2D sequence starter upper.  Just a position and chaining info.
 */

BOOL LE_SEQNCR_StartCXY (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
TYPE_Coord2D X, TYPE_Coord2D Y)
{
  return
    LE_SEQNCR_StartCXYSlotDrop (DataID, Priority,
    ChainToDataID, ChainToPriority, InterruptChainee,
    X, Y, (LE_REND_RenderSlotSet) 0, LE_SEQNCR_DropDefault);
}



/*****************************************************************************
 * The usual 2D sequence starter upper but with a render slot set.
 */

BOOL LE_SEQNCR_StartCXYSlot (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
TYPE_Coord2D X, TYPE_Coord2D Y, LE_REND_RenderSlotSet SlotSet)
{
  return
    LE_SEQNCR_StartCXYSlotDrop (DataID, Priority,
    ChainToDataID, ChainToPriority, InterruptChainee,
    X, Y, SlotSet, LE_SEQNCR_DropDefault);
}



/*****************************************************************************
 * The usual 2D sequence starter upper but with a render slot set and drop
 * frames option.
 */

BOOL LE_SEQNCR_StartCXYSlotDrop (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
TYPE_Coord2D X, TYPE_Coord2D Y,
LE_REND_RenderSlotSet SlotSet,
LE_SEQNCR_DropFrames DropFramesMethod)
{
  TYPE_Matrix2D MatrixA;
  TYPE_Point2D  Offset;

  // Move it over by the translation amount.

  Offset.x = X;
  Offset.y = Y;
  LE_MATRIX_2D_SetTranslate (&Offset, &MatrixA);

  return
    LE_SEQNCR_StartTheWorks (DataID, Priority,
    ChainToDataID, ChainToPriority, InterruptChainee, FALSE,
    LE_DATA_EmptyItem, 0, FALSE,
    2, &MatrixA,
    0, 0, FALSE, 0, FALSE, LE_SEQNCR_NoEndingAction,
    SlotSet, NULL, NULL,
    DefaultPreloadFlagForStartSequence, DefaultDisposalFlagForStartSequence,
    DefaultUseRefFlagForStartSequence, DropFramesMethod);
}



/*****************************************************************************
 * Start the given 2D sequence at the given offset on the screen, with
 * scaling and rotation too.
 */

BOOL LE_SEQNCR_StartCXYSR (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
TYPE_Coord2D X, TYPE_Coord2D Y, TYPE_Scale2D Scale, TYPE_Angle2D Rotate)
{
  return
    LE_SEQNCR_StartCXYSRSlot (DataID, Priority,
    ChainToDataID, ChainToPriority, InterruptChainee,
    X, Y, Scale, Rotate,
    (LE_REND_RenderSlotSet) 0);
}



/*****************************************************************************
 * Start the given 2D sequence at the given offset on the screen, with
 * scaling and rotation too.
 */

BOOL LE_SEQNCR_StartCXYSRSlot (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
TYPE_Coord2D X, TYPE_Coord2D Y, TYPE_Scale2D Scale, TYPE_Angle2D Rotate,
LE_REND_RenderSlotSet SlotSet)
{
  TYPE_Matrix2D MatrixA;
  TYPE_Matrix2D MatrixB;
  TYPE_Matrix2D MatrixC;
  TYPE_Point2D  Offset;

  // Rotate the image first.

  LE_MATRIX_2D_SetRotate (Rotate, &MatrixA);

  // Then scale it up or down about its origin.

  LE_MATRIX_2D_SetScale (Scale, &MatrixB);
  LE_MATRIX_2D_MatrixMultiply (&MatrixA, &MatrixB, &MatrixC);

  // Finally move the whole mess over by the translation amount.

  Offset.x = X;
  Offset.y = Y;
  LE_MATRIX_2D_SetTranslate (&Offset, &MatrixB);
  LE_MATRIX_2D_MatrixMultiply (&MatrixC, &MatrixB, &MatrixA);

  return
    LE_SEQNCR_StartTheWorks (DataID, Priority,
    ChainToDataID, ChainToPriority, InterruptChainee, FALSE,
    LE_DATA_EmptyItem, 0, FALSE,
    2, &MatrixA,
    0, 0, FALSE, 0, FALSE, LE_SEQNCR_NoEndingAction,
    SlotSet, NULL, NULL,
    DefaultPreloadFlagForStartSequence, DefaultDisposalFlagForStartSequence,
    DefaultUseRefFlagForStartSequence, LE_SEQNCR_DropDefault);
}



/*****************************************************************************
 * Start the given 3D sequence at the given position and orientation and
 * size in the 3D world.
 */

BOOL LE_SEQNCR_StartRySTxz (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Angle3D Yaw, TYPE_Scale3D Scale, TYPE_Coord3D X, TYPE_Coord3D Z)
{
  return
    LE_SEQNCR_StartRySTxzDrop (DataID, Priority,
    Yaw, Scale, X, Z, LE_SEQNCR_DropDefault);
}



/*****************************************************************************
 * Start the given 3D sequence at the given position and orientation and
 * size in the 3D world, with a drop option too.  Minimal parameters;
 * we assume Y is zero (ground level) and it is just turning around the
 * compass heading (no pitch or roll).
 */

BOOL LE_SEQNCR_StartRySTxzDrop (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Angle3D Yaw, TYPE_Scale3D Scale, TYPE_Coord3D X, TYPE_Coord3D Z,
LE_SEQNCR_DropFrames DropFramesMethod)
{
  TYPE_Matrix3D MatrixA;
  TYPE_Point3D  Offset;
  TYPE_Point3D  Origin;

  // The origin where the rotations and scales take place around.

  memset (&Origin, 0, sizeof (Origin));

  // The final offset to move the animation to a spot in the world.

  Offset.x = X;
  Offset.y = 0.0F;
  Offset.z = Z;

  LE_MATRIX_3D_SetOriginScaleRotateTranslate (&Origin, Scale, Scale, Scale,
    Yaw, 0.0F /* Pitch */, 0.0F /* Roll */, &Offset, &MatrixA);

  return
    LE_SEQNCR_StartTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    LE_DATA_EmptyItem, 0, FALSE,
    3, &MatrixA,
    0, 0, FALSE, 0, FALSE, LE_SEQNCR_NoEndingAction,
    (LE_REND_RenderSlotSet) 0, NULL, NULL,
    DefaultPreloadFlagForStartSequence, DefaultDisposalFlagForStartSequence,
    DefaultUseRefFlagForStartSequence, DropFramesMethod);
}



/*****************************************************************************
 * Start the given 3D sequence at the given position and orientation and
 * size in the 3D world.
 */

BOOL LE_SEQNCR_StartCOxyzRyprSxyzTxyz (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
TYPE_Coord3D OriginX, TYPE_Coord3D OriginY, TYPE_Coord3D OriginZ,
TYPE_Angle3D Yaw, TYPE_Angle3D Pitch, TYPE_Angle3D Roll,
TYPE_Scale3D ScaleX, TYPE_Scale3D ScaleY, TYPE_Scale3D ScaleZ,
TYPE_Coord3D X, TYPE_Coord3D Y, TYPE_Coord3D Z)
{
  TYPE_Matrix3D MatrixA;
  TYPE_Point3D  Offset;
  TYPE_Point3D  Origin;

  // The origin where the rotations and scales take place around.

  Origin.x = OriginX;
  Origin.y = OriginY;
  Origin.z = OriginZ;

  // The final offset to move the animation to a spot in the world.

  Offset.x = X;
  Offset.y = Y;
  Offset.z = Z;

  LE_MATRIX_3D_SetOriginScaleRotateTranslate (
    &Origin, ScaleX, ScaleY, ScaleZ, Yaw, Pitch, Roll, &Offset,
    &MatrixA);

  return
    LE_SEQNCR_StartTheWorks (DataID, Priority,
    ChainToDataID, ChainToPriority, InterruptChainee, FALSE,
    LE_DATA_EmptyItem, 0, FALSE,
    3, &MatrixA,
    0, 0, FALSE, 0, FALSE, LE_SEQNCR_NoEndingAction,
    (LE_REND_RenderSlotSet) 0, NULL, NULL,
    DefaultPreloadFlagForStartSequence, DefaultDisposalFlagForStartSequence,
    DefaultUseRefFlagForStartSequence, LE_SEQNCR_DropDefault);
}



/*****************************************************************************
 * Stop the given sequence, if it is running.  If there are several with the
 * same DataID/Priority then stop them all.  This also gets rid of any chains
 * which are waiting for the DataID/Priority, even if no sequence is running,
 * and recursively all ones chained to it and so on.  If StopAncestorsToo is
 * on then the pending chain list will be searched for start commands for
 * DataID/Priority and they will be stopped and recursively their ancestors
 * too.  SearchWholeTree is TRUE if you want to look for running sequences
 * anywhere in the tree, FALSE for just the top level ones.  This can be
 * quite a slow command.
 */

BOOL LE_SEQNCR_StopTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL StopAncestorsToo, BOOL StopDescendantsToo, BOOL SearchWholeTree)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_StopChainedAncestors = StopAncestorsToo;
  CommandFlags.asFlags.SEQFLAG_StopChainedDescendants = StopDescendantsToo;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTree;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_Stop, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Stop the given sequence, if it is running.  If there are several with the
 * same DataID/Priority then stop them all.  Does not stop chained ancestors
 * or descendants - so its best to use with individual isolated sequences.
 */

BOOL LE_SEQNCR_Stop (LE_DATA_DataId DataID, UNS16 Priority)
{
  return LE_SEQNCR_StopTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE /* Ancestors */, FALSE /* Descendants */, FALSE /* Search all */);
}



/*****************************************************************************
 * Stop the given sequence, if it is running, and all chained ancestors.  This
 * is useful for stopping the last sequence in a chain and making the rest of
 * them stop too no matter how far the playing has progressed down the chain.
 */

BOOL LE_SEQNCR_StopFromChainEnd (LE_DATA_DataId DataID, UNS16 Priority)
{
  return LE_SEQNCR_StopTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    TRUE /* Ancestors */, FALSE /* Descendants */, FALSE /* Search all */);
}



/*****************************************************************************
 * Stop all sequences in the given set of render slots which are at the
 * given priority level or higher.  Chains can be affected too.  Only the
 * top level sequences are examined (children's priorities are different).
 * However, the SearchWholeTree is used to find running chained children
 * when getting rid of the chains.
 */

BOOL LE_SEQNCR_StopAllTheWorks (LE_REND_RenderSlotSet RenderSlotSet,
UNS16 Priority, LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL StopAncestorsToo, BOOL StopDescendantsToo, BOOL SearchWholeTree)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_StopChainedAncestors = StopAncestorsToo;
  CommandFlags.asFlags.SEQFLAG_StopChainedDescendants = StopDescendantsToo;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTree;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_StopAll, 0, 0,
    RenderSlotSet, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Stops all sequences in the render slots with that priority or larger.
 * Also removes all related chains.
 */

BOOL LE_SEQNCR_StopAllRP (LE_REND_RenderSlotSet RenderSlotSet, UNS16 Priority)
{
  return LE_SEQNCR_StopAllTheWorks (RenderSlotSet, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    TRUE, TRUE, TRUE);
}



/*****************************************************************************
 * Move the specified sequence to a new place in the world.  If you wish, you
 * can move it off the edge of the world too :-).  Returns TRUE if the command
 * was successfully queued up for execution.
 */

BOOL LE_SEQNCR_MoveTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL SearchWholeTreeForSequence, UNS8 MatrixDimensionality, void *MatrixPntr)
{
  SeqCmdMove2DAttachmentRecord  AuxData2D;
  SeqCmdMove3DAttachmentRecord  AuxData3D;
  void                         *AuxDataPntr = NULL;
  UNS16                         BinarySize = 0;
  SeqCmdFlagsAsLong             CommandFlags;
  BOOL                          ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

#if CE_ARTLIB_EnableDebugMode
  if (MatrixDimensionality != 2 && MatrixDimensionality != 3)
  {
    LE_ERROR_DebugMsg ("LE_SEQNCR_MoveTheWorks: Wrong number of dimensions.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
    return FALSE;
  }
#endif

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;

  if (MatrixDimensionality == 2 && MatrixPntr != NULL)
  {
    AuxDataPntr = &AuxData2D;
    AuxData2D.positionMatrix = *((TYPE_Matrix2DPointer) MatrixPntr);
    BinarySize = sizeof (AuxData2D);
  }
  else if (MatrixDimensionality == 3 && MatrixPntr != NULL)
  {
    AuxDataPntr = &AuxData3D;
    AuxData3D.positionMatrix = *((TYPE_Matrix3DPointer) MatrixPntr);
    BinarySize = sizeof (AuxData3D);
  }

  // Submit it to the command queue.  Returns TRUE if successful.

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_Move, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    (ACHAR *) AuxDataPntr, BinarySize);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Move the 2D sequence to the given coordinates, with chaining.
 */

BOOL LE_SEQNCR_MoveCXY (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
TYPE_Coord2D X, TYPE_Coord2D Y)
{
  TYPE_Matrix2D MatrixA;
  void         *MatrixPntr = NULL;
  TYPE_Point2D  Offset;

  // Move it over by the translation amount.

  if (X != 0 || Y != 0)
  {
    Offset.x = X;
    Offset.y = Y;
    LE_MATRIX_2D_SetTranslate (&Offset, &MatrixA);
    MatrixPntr = &MatrixA;
  }

  return
    LE_SEQNCR_MoveTheWorks (DataID, Priority,
    ChainToDataID, ChainToPriority, InterruptChainee, FALSE,
    FALSE /* Don't search whole tree */, 2, MatrixPntr);
}



/*****************************************************************************
 * Move the 2D sequence to the given coordinates.
 */

BOOL LE_SEQNCR_MoveXY (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Coord2D X, TYPE_Coord2D Y)
{
  return
    LE_SEQNCR_MoveCXY (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE,
    X, Y);
}



/*****************************************************************************
 * Move the 2D sequence and also do scaling and rotation.
 */

BOOL LE_SEQNCR_MoveXYSR (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Coord2D X, TYPE_Coord2D Y, TYPE_Scale2D Scale, TYPE_Angle2D Rotate)
{
  TYPE_Matrix2D MatrixA;
  TYPE_Matrix2D MatrixB;
  TYPE_Matrix2D MatrixC;
  void         *MatrixPntr = NULL;
  TYPE_Point2D  Offset;

  if (X != 0 || Y != 0 || Scale != 1.0F || Rotate != 0.0F)
  {
    // Rotate the image first.

    LE_MATRIX_2D_SetRotate (Rotate, &MatrixA);

    // Then scale it up or down about its origin.

    LE_MATRIX_2D_SetScale (Scale, &MatrixB);
    LE_MATRIX_2D_MatrixMultiply (&MatrixA, &MatrixB, &MatrixC);

    // Finally move the whole mess over by the translation amount.

    Offset.x = X;
    Offset.y = Y;
    LE_MATRIX_2D_SetTranslate (&Offset, &MatrixB);
    LE_MATRIX_2D_MatrixMultiply (&MatrixC, &MatrixB, &MatrixA);

    MatrixPntr = &MatrixA;
  }

  return
    LE_SEQNCR_MoveTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE /* Don't search whole tree */, 2, MatrixPntr);
}



/*****************************************************************************
 * Move the 3D sequence in some way.
 */

BOOL LE_SEQNCR_MoveRySTxz (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Angle3D Yaw, TYPE_Scale3D Scale, TYPE_Coord3D X, TYPE_Coord3D Z)
{
  TYPE_Matrix3D MatrixA;
  TYPE_Point3D  Offset;
  TYPE_Point3D  Origin;

  memset (&Origin, 0, sizeof (Origin));

  Offset.x = X;
  Offset.y = 0.0F;
  Offset.z = Z;

  LE_MATRIX_3D_SetOriginScaleRotateTranslate (&Origin, Scale, Scale, Scale,
    Yaw, 0.0F /* Pitch */, 0.0F /* Roll */, &Offset, &MatrixA);

  return
    LE_SEQNCR_MoveTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE /* Don't search whole tree */, 3, &MatrixA);
}



/*****************************************************************************
 * Move the 3D sequence in a more general way.
 */

BOOL LE_SEQNCR_MoveCOxyzRyprSxyzTxyz (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
TYPE_Coord3D OriginX, TYPE_Coord3D OriginY, TYPE_Coord3D OriginZ,
TYPE_Angle3D Yaw, TYPE_Angle3D Pitch, TYPE_Angle3D Roll,
TYPE_Scale3D ScaleX, TYPE_Scale3D ScaleY, TYPE_Scale3D ScaleZ,
TYPE_Coord3D X, TYPE_Coord3D Y, TYPE_Coord3D Z)
{
  TYPE_Matrix3D MatrixA;
  TYPE_Point3D  Offset;
  TYPE_Point3D  Origin;

  Origin.x = OriginX;
  Origin.y = OriginY;
  Origin.z = OriginZ;

  Offset.x = X;
  Offset.y = Y;
  Offset.z = Z;

  LE_MATRIX_3D_SetOriginScaleRotateTranslate (
    &Origin, ScaleX, ScaleY, ScaleZ, Yaw, Pitch, Roll, &Offset,
    &MatrixA);

  return
    LE_SEQNCR_MoveTheWorks (DataID, Priority,
    ChainToDataID, ChainToPriority, InterruptChainee, FALSE,
    FALSE /* Don't search whole tree */, 3, &MatrixA);
}



/*****************************************************************************
 * Pause (PauseIt is TRUE) or unpause (FALSE) the given sequence.
 */

BOOL LE_SEQNCR_PauseTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL SearchWholeTreeForSequence, BOOL PauseIt)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;
  CommandFlags.asFlags.SEQFLAG_ByteSizedValue = (PauseIt != FALSE); // Convert to 0/1.

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_Pause, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Simpler pause sequence.
 */

BOOL LE_SEQNCR_Pause (LE_DATA_DataId DataID, UNS16 Priority, BOOL PauseIt)
{
  return LE_SEQNCR_PauseTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, PauseIt);
}



/*****************************************************************************
 * Set the given sequence's clock to a new time.  Can be in the past or in
 * the future.  If in the past, sequences that would start in the future
 * are killed off.  If in the future, doesn't start sequences it skipped
 * over (otherwise you would have one really big mess on the screen, well,
 * actually it would start and then instantly stop all the intervening
 * sequences, but that would still be undesirable).
 */

BOOL LE_SEQNCR_SetClockTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL SearchWholeTreeForSequence, INT32 NewClock)
{
  SeqCmdSetClockAttachmentRecord  AttachedTime;
  SeqCmdFlagsAsLong               CommandFlags;
  BOOL                            ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;

  AttachedTime.newClock = NewClock;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetClock, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    (ACHAR *) &AttachedTime, sizeof (AttachedTime));

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * The more typical clock setting function.
 */

BOOL LE_SEQNCR_SetClock (LE_DATA_DataId DataID, UNS16 Priority, INT32 NewClock)
{
  return
    LE_SEQNCR_SetClockTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, NewClock);
}



/*****************************************************************************
 * A convenience function for forcing the clock to the end of time and thus
 * making the sequence stop normally or loop.  However, it doesn't work
 * too well for stay-at-end sequences since it jumps to the end and doesn't
 * process the frames which start up before the actual end - the ones which
 * you would otherwise see on the screen at the end.  Instead, you may want
 * to find the duration of the sequence and set the clock to almost but
 * not quite at the end so that it picks up the last frames.
 */

BOOL LE_SEQNCR_SetClockToEnd (LE_DATA_DataId DataID, UNS16 Priority)
{
  return
    LE_SEQNCR_SetClock (DataID, Priority, INFINITE_END_TIME);
}



/*****************************************************************************
 * Change the ending action of a sequence that is in progress.
 */

BOOL LE_SEQNCR_SetEndingActionTheWorks (LE_DATA_DataId DataID,
UNS16 Priority, LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL SearchWholeTreeForSequence, LE_SEQNCR_EndingAction NewEndingAction)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  if (NewEndingAction <= LE_SEQNCR_NoEndingAction ||
  NewEndingAction >= LE_SEQNCR_EndingActionMAX)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_SetEndingActionTheWorks: "
      "Ending action %d is unknown or invalid.\r\n",
      (int) NewEndingAction);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;
  CommandFlags.asFlags.SEQFLAG_ReplacementEndingAction = NewEndingAction;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetEndingAction, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Simplified version for setting the ending action.
 */

BOOL LE_SEQNCR_SetEndingAction (LE_DATA_DataId DataID, UNS16 Priority,
LE_SEQNCR_EndingAction NewEndingAction)
{
  return LE_SEQNCR_SetEndingActionTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, NewEndingAction);
}



/*****************************************************************************
 * Set the given sequence's sound volume to the new volume.  Works for
 * video sequences too.  0 is silence, 100 is loudest.
 */

BOOL LE_SEQNCR_SetVolumeTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL SearchWholeTreeForSequence, UNS8 NewVolume)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;
  CommandFlags.asFlags.SEQFLAG_ByteSizedValue = NewVolume;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetVolume, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * The more typical volume setting function.
 */

BOOL LE_SEQNCR_SetVolume (LE_DATA_DataId DataID, UNS16 Priority, UNS8 NewVolume)
{
  return
    LE_SEQNCR_SetVolumeTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, NewVolume);
}



/*****************************************************************************
 * Set the given sequence's left/right panning to the new value.  Works for
 * video sequences too.  -100 is left, 0 is middle, +100 is right.
 */

BOOL LE_SEQNCR_SetPanTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL SearchWholeTreeForSequence, INT8 NewPan)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;
  CommandFlags.asFlags.SEQFLAG_ByteSizedValue = NewPan;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetPan, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * The more typical panning setting function.
 */

BOOL LE_SEQNCR_SetPan (LE_DATA_DataId DataID, UNS16 Priority, INT8 NewPan)
{
  return
    LE_SEQNCR_SetPanTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, NewPan);
}



/*****************************************************************************
 * Set the given sound sequence's pitch to the new pitch in hertz.  Use zero
 * for the default pitch of the sound.
 */

BOOL LE_SEQNCR_SetPitchTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL SearchWholeTreeForSequence, UNS32 NewPitch)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;
  CommandFlags.asFlags.SEQFLAG_ByteSizedValue = (UNS8) (NewPitch / 256);

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetPitch, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * The more typical pitch setting function.
 */

BOOL LE_SEQNCR_SetPitch (LE_DATA_DataId DataID, UNS16 Priority, UNS32 NewPitch)
{
  return
    LE_SEQNCR_SetPitchTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, NewPitch);
}



/*****************************************************************************
 * Set the given video sequence's colour saturation to the new value.
 * -100 is black and white, 0 is normal, +100 is very colourful.
 */

BOOL LE_SEQNCR_SetSaturationTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL SearchWholeTreeForSequence, INT8 NewSaturation)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;
  CommandFlags.asFlags.SEQFLAG_ByteSizedValue = NewSaturation;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetSaturation, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * The more typical saturation setting function.
 */

BOOL LE_SEQNCR_SetSaturation (LE_DATA_DataId DataID, UNS16 Priority, INT8 NewSaturation)
{
  return
    LE_SEQNCR_SetSaturationTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, NewSaturation);
}



/*****************************************************************************
 * Set the given video sequence's brightness to the new value.
 * -100 is dark, 0 is normal, +100 is bright.
 */

BOOL LE_SEQNCR_SetBrightnessTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL SearchWholeTreeForSequence, INT8 NewBrightness)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;
  CommandFlags.asFlags.SEQFLAG_ByteSizedValue = NewBrightness;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetBrightness, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * The more typical brightness setting function.
 */

BOOL LE_SEQNCR_SetBrightness (LE_DATA_DataId DataID, UNS16 Priority, INT8 NewBrightness)
{
  return
    LE_SEQNCR_SetBrightnessTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, NewBrightness);
}



/*****************************************************************************
 * Set the given video sequence's contrast to the new value.
 * -100 is fuzzy, 0 is normal, +100 is sharp.
 */

BOOL LE_SEQNCR_SetContrastTheWorks (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
BOOL SearchWholeTreeForSequence, INT8 NewContrast)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;
  CommandFlags.asFlags.SEQFLAG_ByteSizedValue = NewContrast;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetContrast, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * The more typical contrast setting function.
 */

BOOL LE_SEQNCR_SetContrast (LE_DATA_DataId DataID, UNS16 Priority, INT8 NewContrast)
{
  return
    LE_SEQNCR_SetContrastTheWorks (DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, NewContrast);
}



/*****************************************************************************
 * Set up an alternative playback path in the given video sequence.  When
 * the player reaches the given decision frame it can play normally or it
 * can jump to a different part of the video, both with no delays.  This
 * works because the video player will buffer the frames after the decision
 * frame and also the frames after the jump frame number.  Eventually you
 * can set up multiple jump points (identified by the decision frame number),
 * but for now the player only supports one.  Once the decision frame is
 * reached, and the jump taken or not taken, the alternative gets used up
 * and is no longer active.  By default the film does not take the jump, see
 * LE_SEQNCR_ChooseVideoAlternativePath to turn on the jump.  The last frame
 * seen in common by both paths is the DecisionFrameNumber frame.  Frame
 * numbers start at zero and go up to the number of frames in the video
 * minus one.  This function doesn't work for non-video sequences, manually
 * wait for the decision time and then use SetClock, for that matter you can
 * also use SetClock with video sequences.
 */

BOOL LE_SEQNCR_SetUpVideoAlternativePathTheWorks (
  LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence,
  INT32 DecisionFrameNumber, INT32 JumpToFrameNumber)
{
  SeqCmdSetVideoAlternativeAttachmentRecord AttachedInfo;
  SeqCmdFlagsAsLong                         CommandFlags;
  BOOL                                      ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;

  AttachedInfo.decisionFrameNumber = DecisionFrameNumber;
  AttachedInfo.jumpToFrameNumber = JumpToFrameNumber;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetVideoAlternative, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    (ACHAR *) &AttachedInfo, sizeof (AttachedInfo));

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



BOOL LE_SEQNCR_SetUpVideoAlternativePath (
  LE_DATA_DataId DataID, UNS16 Priority,
  INT32 DecisionFrameNumber, INT32 JumpToFrameNumber)
{
  return LE_SEQNCR_SetUpVideoAlternativePathTheWorks (
    DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, DecisionFrameNumber, JumpToFrameNumber);
}



/*****************************************************************************
 * Turns on or off the decision to jump at the given decision frame.  You
 * have until 1 frame before the decision frame to change your choice.
 */

BOOL LE_SEQNCR_ChooseVideoAlternativePathTheWorks (
  LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence,
  INT32 DecisionFrameNumber, BOOL TakeAlternative)
{
  SeqCmdChooseVideoAlternativeAttachmentRecord  AttachedInfo;
  SeqCmdFlagsAsLong                             CommandFlags;
  BOOL                                          ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;

  AttachedInfo.decisionFrameNumber = DecisionFrameNumber;
  AttachedInfo.takeAlternative = TakeAlternative;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_ChooseVideoAlternative, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    (ACHAR *) &AttachedInfo, sizeof (AttachedInfo));

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



BOOL LE_SEQNCR_ChooseVideoAlternativePath (
  LE_DATA_DataId DataID, UNS16 Priority,
  INT32 DecisionFrameNumber, BOOL TakeAlternative)
{
  return LE_SEQNCR_ChooseVideoAlternativePathTheWorks (
    DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    FALSE, DecisionFrameNumber, TakeAlternative);
}



/*****************************************************************************
 * Deallocates, forgets, clears, and otherwise destroys all pending video
 * jump points.  Useful if you changed your mind and want to do something
 * else with the video - which will now continue playing without any
 * jumps.
 */

BOOL LE_SEQNCR_ForgetVideoAlternativePathsTheWorks (
  LE_DATA_DataId DataID, UNS16 Priority,
  LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority,
  BOOL InterruptChainee, BOOL SearchWholeTreeForChains,
  BOOL SearchWholeTreeForSequence)
{
  SeqCmdFlagsAsLong CommandFlags;
  BOOL              ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;
  CommandFlags.asFlags.SEQFLAG_SearchWholeSequenceTree = SearchWholeTreeForSequence;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_ForgetVideoAlternatives, 0, 0,
    DataID, Priority, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    NULL, 0);

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



BOOL LE_SEQNCR_ForgetVideoAlternativePaths (
  LE_DATA_DataId DataID, UNS16 Priority)
{
  return LE_SEQNCR_ForgetVideoAlternativePathsTheWorks (
    DataID, Priority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE, FALSE);
}



/*****************************************************************************
 * Changes the viewport used by a render slot - the screen area that the
 * slot draws its image into.  NULL for full screen.
 */

BOOL LE_SEQNCR_SetViewportTheWorks (TYPE_RectPointer ViewportRectPntr,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
BOOL SearchWholeTreeForChains, LE_REND_RenderSlot RenderSlotNumber)
{
  SeqCmdSetViewportAttachmentRecord     AttachedInfo;
  SeqCmdFlagsAsLong                     CommandFlags;
  BOOL                                  ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  if (ViewportRectPntr == NULL)
    ViewportRectPntr = &LE_GRAFIX_ScreenDestinationRect; // Full screen.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;

  AttachedInfo.viewportRect = *ViewportRectPntr;

  AttachedInfo.renderSlotNumber = RenderSlotNumber;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetViewport, 0, 0,
    0, 0, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    (ACHAR *) &AttachedInfo, sizeof (AttachedInfo));

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Just set the viewport for a render slot.
 */

BOOL LE_SEQNCR_SetViewport (TYPE_RectPointer ViewportRectPntr,
LE_REND_RenderSlot RenderSlotNumber)
{
  return
    LE_SEQNCR_SetViewportTheWorks (ViewportRectPntr,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    RenderSlotNumber);
}



/*****************************************************************************
 * Changes the camera settings of a render slot, works for 2D and 3D.
 *
 * If CameraNumber is zero then it uses the settings you specify, otherwise
 * it uses the animated object with that label number for the source
 * of the camera position and settings.  If the animated object doesn't
 * exist then your settings will be ignored and the camera is unchanged
 * from the previous one.  Once the labelled object starts up, it gets
 * used for the camera settings.
 *
 * In 3D the Forwards is a normalised (length == 1) vector pointing in the
 * direction you are looking towards.  Up points upwards (usually
 * perpendicular to Forwards.  HalfFovAngle is half the field of view angle in
 * radians (usually 45 degrees or 0.78539 radians for a 90 degree field of
 * view where the edge of the camera runs down the planes X=Z and Y=Z).
 * For less nauseating scenes you should match the angle that the monitor
 * covers at normal vieing distance, which is 18 degrees for a TV set, so use
 * 0.15708 radians for the half FOV argument.  The NearZ and FarZ are the
 * closest and furthest distances visible from the camera, and should tightly
 * bound the objects being viewed to increase the resolution of the Z-buffer
 * used for depth intersection calculations.
 *
 * In 2D the X and Y of Position are used to set the camera position, which
 * makes that place in the world appear in the center of the screen.  The
 * Position Z component sets the off screen sound distance (sounds become
 * quieter further from the camera and totally quite at this distance).
 * The Up vector X component is used for the camera rotation angle, to make
 * the world spin, in radians.  HalfFovAngle becomes the 2D scale factor,
 * 1.0 is normal, 2.0 makes every pixel double size (zooms in), 0.5 makes
 * every pixel half size (zoom out) and other values work like you would
 * expect (avoid 0 and negative).
 *
 * Returns TRUE if the command was successfully queued up.
 */

BOOL LE_SEQNCR_SetCameraTheWorks (UNS8 CameraNumber,
TYPE_Point3DPointer Position, TYPE_Point3DPointer Forwards,
TYPE_Point3DPointer Up, TYPE_Angle3D HalfFovAngle,
TYPE_Coord3D NearZ, TYPE_Coord3D FarZ,
LE_DATA_DataId ChainToDataID, UNS16 ChainToPriority, BOOL InterruptChainee,
BOOL SearchWholeTreeForChains, LE_REND_RenderSlot RenderSlotNumber)
{
  SeqCmdSetCameraAttachmentRecord       AttachedInfo;
  SeqCmdFlagsAsLong                     CommandFlags;
  BOOL                                  ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  CommandFlags.asDWORD = 0;
  CommandFlags.asFlags.SEQFLAG_InterruptPreviousSequence = InterruptChainee;
  CommandFlags.asFlags.SEQFLAG_SearchWholeTreeForChains = SearchWholeTreeForChains;

  AttachedInfo.renderSlotNumber = RenderSlotNumber;
  AttachedInfo.cameraNumber = CameraNumber;
  AttachedInfo.position = *Position;
  AttachedInfo.forwards = *Forwards;
  AttachedInfo.up = *Up;
  AttachedInfo.fovAngle = HalfFovAngle;
  AttachedInfo.nearZ = NearZ;
  AttachedInfo.farZ = FarZ;
  AttachedInfo.renderSlotNumber = RenderSlotNumber;

  ReturnCode = LE_QUEUE_EnqueueWithParameters (&SequencerCommandQueue,
    SEQCMD_SetCamera, 0, 0,
    0, 0, ChainToDataID, ChainToPriority, CommandFlags.asDWORD,
    (ACHAR *) &AttachedInfo, sizeof (AttachedInfo));

#if CE_ARTLIB_SeqncrImmediateCommandExecution
  LE_SEQNCR_ProcessUserCommands ();
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Use the user specified labelled animated object as the camera.
 */

BOOL LE_SEQNCR_SetCameraNumber (UNS8 CameraNumber,
LE_REND_RenderSlot RenderSlotNumber)
{
  TYPE_Point3D  Forwards;
  TYPE_Point3D  Position;
  TYPE_Point3D  Up;

  // Set some default 3D camera settings in case they accidentally specify
  // zero as the camera number.

  Forwards.x = 0.0;
  Forwards.y = 0.0;
  Forwards.z = 1.0;

  Position.x = 0.0;
  Position.y = 0.0;
  Position.z = -500.0;

  Up.x = 0.0;
  Up.y = 1.0;
  Up.z = 0.0;

  return
    LE_SEQNCR_SetCameraTheWorks (CameraNumber,
    &Position, &Forwards, &Up, 0.785398F, 1.0F, 1000.0F,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    RenderSlotNumber);
}



/*****************************************************************************
 * Just set the camera for a 3D render slot.
 */

BOOL LE_SEQNCR_SetCamera3D (TYPE_Point3DPointer Position,
TYPE_Point3DPointer Forwards, TYPE_Point3DPointer Up,
TYPE_Angle3D HalfFovAngle, TYPE_Coord3D NearZ, TYPE_Coord3D FarZ,
LE_REND_RenderSlot RenderSlotNumber)
{
  return
    LE_SEQNCR_SetCameraTheWorks (0 /* CameraNumber */,
    Position, Forwards, Up, HalfFovAngle, NearZ, FarZ,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    RenderSlotNumber);
}



/*****************************************************************************
 * An easier to understand camera setting for 3D.  The camera is at Position
 * and is looking at the given location.  Up is towards positive Y.
 */

BOOL LE_SEQNCR_SetCamera3DLookAt (
TYPE_Point3DPointer Position, TYPE_Point3DPointer LookingAt,
TYPE_Angle3D HalfFovAngle, TYPE_Coord3D NearZ, TYPE_Coord3D FarZ,
LE_REND_RenderSlot RenderSlotNumber)
{
  TYPE_Point3D  Forwards;
  float         Multiplier;
  TYPE_Point3D  ProjectedUp;
  TYPE_Point3D  Up;

  // Set the forwards vector.

  Forwards = *LookingAt - *Position;
  LE_MATRIX_3D_NormalizeVector (&Forwards, &Forwards);

  // Set the up vector.  Maybe project it onto the forwards vector and
  // remove the projected part so we get pure perpendicular part.

  Up.x = 0.0F;
  Up.y = 1.0F;
  Up.z = 0.0F;

  // Find the orthogonal projection of u onto v.
  // u = Up vector, v = Forwards vector
  // Also since ||v|| == 1.0, v.v is 1.0 too.
  //      u . v
  // w = ------- v
  //      v . v

  Multiplier = LE_MATRIX_3D_VectorDotProduct (&Up, &Forwards);
  ProjectedUp = Forwards * Multiplier;
  Up = Up - ProjectedUp;
  LE_MATRIX_3D_NormalizeVector (&Up, &Up);

  return
    LE_SEQNCR_SetCameraTheWorks (0 /* CameraNumber */,
    Position, &Forwards, &Up, HalfFovAngle, NearZ, FarZ,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    RenderSlotNumber);
}



/*****************************************************************************
 * Just set the camera for a 2D render slot.
 */

BOOL LE_SEQNCR_SetCamera2D (TYPE_Coord2D X, TYPE_Coord2D Y,
TYPE_Coord2D QuietSoundDistance, TYPE_Scale2D ScaleFactor,
TYPE_Angle2D Rotation, LE_REND_RenderSlot RenderSlotNumber)
{
  TYPE_Point3D  Forwards;
  TYPE_Point3D  Position;
  TYPE_Point3D  Up;

  Position.x = (float) X;
  Position.y = (float) Y;
  Position.z = (float) QuietSoundDistance;
  Up.x = Rotation;

  return
    LE_SEQNCR_SetCameraTheWorks (0 /* CameraNumber */,
    &Position, &Forwards, &Up, ScaleFactor /* HalfFovAngle */,
    1.0 /* NearZ */, 1000.0 /* FarZ */,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    RenderSlotNumber);
}



/*****************************************************************************
 * Change the watch monitoring for a particular index in the watch array.
 *
 * The new DataID/Priority get stored in the array (so that if that sequence
 * starts, it will use that entry).  The entry gets cleared to zeroes in all
 * cases.  You can also set the DataID to LE_DATA_EmptyItem to signify that
 * the watch is turned off.  A search is done for existing top level sequences
 * with that DataID/Priority and they get modified to update that particular
 * watch index when they change status.
 */

BOOL LE_SEQNCR_SetWatch (LE_DATA_DataId DataID, UNS16 Priority, int WatchIndex)
{
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;
  LE_SEQNCR_WatchPointer        WatchPntr;

  if (WatchIndex < 0 || WatchIndex >= CE_ARTLIB_SeqncrMaxWatches)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_SetWatch: "
      "Watch index %d is out of bounds (0 to %d).\r\n",
      (int) WatchIndex, (int) (CE_ARTLIB_SeqncrMaxWatches - 1));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Index out of bounds.
  }

  WatchPntr = (LE_SEQNCR_WatchPointer) (LE_SEQNCR_WatchArray + WatchIndex);
  memset (WatchPntr, 0, sizeof (LE_SEQNCR_WatchRecord));
  WatchPntr->dataID = DataID;
  WatchPntr->priority = Priority;


  // Now search for existing (running) top level sequences which match the
  // watch, and redirect them to use this watch index.  Also update the
  // watch entry's start flag to TRUE and increment the update count for
  // each one found.

  if (DataID != LE_DATA_EmptyItem && LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
  {
    EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

    SequencePntr = LI_SEQNCR_CommonRuntimeTreeRoot->child;

    while (SequencePntr != NULL)
    {
      if (SequencePntr->dataID == DataID &&
      SequencePntr->priority == Priority &&
      SequencePntr->dataOffset == 0)
      {
        SequencePntr->watchIndex = (INT8) WatchIndex;
        WatchPntr->hasStarted = TRUE;
        WatchPntr->updateCount++;
      }
      SequencePntr = SequencePntr->sibling;
    }

    LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
  }
  return TRUE;
}



/*****************************************************************************
 * Look through the chained commands list for a start sequence command that
 * starts the given DataID and Priority.  Return the chaining parameters
 * on that command.
 */

BOOL LE_SEQNCR_FindChainee (LE_DATA_DataId DataID, UNS16 Priority,
LE_DATA_DataId *ChaineeDataID, UNS16 *ChaineePriority)
{
  LE_QUEUE_MessagePointer CommandPntr;
  ChainedItemPointer      CurrentPntr;
  BOOL                    ReturnCode;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL ||
  ChaineeDataID == NULL || ChaineePriority == NULL)
    return FALSE; // System is closed or bad args.

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  ReturnCode = FALSE;
  CurrentPntr = WaitingChainedCommandsTail;
  while (CurrentPntr != NULL)
  {
    CommandPntr = &CurrentPntr->message;

    if (CommandPntr->messageCode == SEQCMD_Start &&
    (LE_DATA_DataId) CommandPntr->numberA == DataID &&
    CommandPntr->numberB == Priority)
    {
      *ChaineeDataID = CommandPntr->numberC;
      *ChaineePriority = (UNS16) CommandPntr->numberD;
      ReturnCode = TRUE;
      break;
    }
    CurrentPntr = CurrentPntr->previous;
  }

  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  return ReturnCode;
}


/************************************************************/
/* LE_SEQNCR_SelectionBoxToggle                             */
/************************************************************/
void LE_SEQNCR_SelectionBoxToggle (LE_DATA_DataId DataID, UNS16 Priority, BOOL EnableSelectionBox)
{
    LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

    if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
    {
        EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

        SequencePntr = FindNextSequence (DataID, Priority, TRUE, NULL);

        if (SequencePntr != NULL)
        {
            SequencePntr->drawSelectionBox      = EnableSelectionBox;
            SequencePntr->needsRedraw           = TRUE;
            SequencePntr->needsReEvaluation     = TRUE;
            MarkAncestorsAsNeedingReEvaluation (SequencePntr);
        }

        LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
    }
}
/************************************************************/


/*****************************************************************************
 * Returns a copy of the runtime sequence info, copied into user supplied
 * buffers (NULL for the info you aren't interested in).  Certain secret
 * fields are blanked out so that you don't accidentally trash stuff which
 * is changing over time (like when this sequence gets deleted).
 *
 * SearchWholeTree is TRUE if you want to search subsequences, FALSE for
 * the normal top level sequences only.
 *
 * Returns TRUE if successful, FALSE if it can't find your sequence or
 * something else goes wrong.
 */

BOOL LE_SEQNCR_GetInfo (LE_DATA_DataId DataID, UNS16 Priority, BOOL SearchWholeTree,
LE_SEQNCR_RuntimeInfoPointer      InfoGenericPntr,
LE_SEQNCR_Runtime2DInfoPointer    Info2DPntr,
LE_SEQNCR_Runtime3DInfoPointer    Info3DPntr,
LE_SEQNCR_RuntimeVideoInfoPointer InfoVideoPntr,
LE_SOUND_AudioStreamPointer       InfoAudioPntr)
{
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE;  // System not open.

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  SequencePntr = FindNextSequence (DataID, Priority, SearchWholeTree, NULL);

  if (SequencePntr != NULL)
  {
    if (InfoGenericPntr != NULL)
    {
       // Copy whole generic sequence information record.

      *InfoGenericPntr = *SequencePntr;

      // Invalidate pointers to stuff which may change at any time.

      InfoGenericPntr->child = NULL;
      InfoGenericPntr->sibling = NULL;
      InfoGenericPntr->parent = NULL;
      InfoGenericPntr->dimensionSpecificData.d0 = NULL;
      InfoGenericPntr->streamSpecificData.voidPntr = NULL;
      memset (&InfoGenericPntr->renderSlotData, 0,
        sizeof (InfoGenericPntr->renderSlotData));
    }

    if (Info2DPntr != NULL && SequencePntr->dimensionality == 2)
    {
      *Info2DPntr = *SequencePntr->dimensionSpecificData.d2;
    }

    if (Info3DPntr != NULL && SequencePntr->dimensionality == 3)
    {
      *Info3DPntr = *SequencePntr->dimensionSpecificData.d3;
    }

    if (InfoVideoPntr != NULL &&
    SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_VIDEO &&
    SequencePntr->streamSpecificData.videoStream != NULL)
    {
      *InfoVideoPntr = *SequencePntr->streamSpecificData.videoStream;
    }

    if (InfoAudioPntr != NULL &&
    SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND &&
    SequencePntr->streamSpecificData.audioStream != NULL)
    {
      *InfoAudioPntr = *SequencePntr->streamSpecificData.audioStream;
    }
  }

  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  return (SequencePntr != NULL); // TRUE if it found your sequence.
}



/*****************************************************************************
 * Look at the given sequence and its children and find the first 3D mesh
 * object and copy its sequence to world matrix into the user provided
 * matrix.  Returns FALSE if it can't find a 3D mesh subsequence.
 */

BOOL LE_SEQNCR_GetChildMeshWorldMatrix (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Matrix3DPointer UserProvidedMatrix)
{
  LE_SEQNCR_RuntimeInfoPointer  CurrentPntr;
  BOOL                          ReturnCode = FALSE;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL || UserProvidedMatrix == NULL)
    return FALSE;  // System not open or bugs somewhere.

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  SequencePntr = FindNextSequence (DataID, Priority,
    FALSE /* SearchWholeTree */, NULL);

  if (SequencePntr == NULL)
    goto ErrorExit;

  CurrentPntr = SequencePntr;

  while (TRUE)
  {
    if (CurrentPntr->sequenceType == LE_CHUNK_ID_SEQ_3DMESH)
    {
      // Found it, extract the matrix and return.

      memcpy (UserProvidedMatrix, &CurrentPntr->dimensionSpecificData.d3->
        sequenceToWorldTransformation.matrix3D, sizeof (TYPE_Matrix3D));
      ReturnCode = TRUE;
      goto NormalExit;
    }

    // Traverse through the tree until we get back to the starting point.

    if (CurrentPntr->child != NULL)
      CurrentPntr = CurrentPntr->child;
    else // No more children, try sibling and then parent.
    {
      while (CurrentPntr != SequencePntr && CurrentPntr->sibling == NULL)
        CurrentPntr = CurrentPntr->parent;
      if (CurrentPntr == SequencePntr)
        goto ErrorExit; // All done.  Failed to find 3D mesh.
      CurrentPntr = CurrentPntr->sibling;
    }
  }

ErrorExit:
NormalExit:
  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
  return ReturnCode;
}



/*****************************************************************************
 * Get the 3D bounding box of the currently playing 3D animation.  It looks
 * at the given sequence and subsequences and finds the first active 3D mesh
 * object and gets the 3D bounding box from it.  The box is then multiplied
 * by the sequence to world matrix and the resulting 8 points are put into
 * the user's buffer.  Returns FALSE if it can't find it or something else
 * goes wrong.
 */

BOOL LE_SEQNCR_GetChildMeshWorldBoundingBox (LE_DATA_DataId DataID, UNS16 Priority,
TYPE_Point3DPointer UserProvidedBuffer)
{
#if CE_ARTLIB_EnableSystemGrafix3D
  LE_SEQNCR_RuntimeInfoPointer  CurrentPntr;
  int                           i;
  TYPE_Matrix3DPointer          MatrixPntr;
  TYPE_Point3DPointer           PointPntr;
  BOOL                          ReturnCode = FALSE;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL || UserProvidedBuffer == NULL)
    return FALSE;  // System not open or bugs somewhere.

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  SequencePntr = FindNextSequence (DataID, Priority,
    FALSE /* SearchWholeTree */, NULL);

  if (SequencePntr == NULL)
    goto ErrorExit;

  CurrentPntr = SequencePntr;

  while (TRUE)
  {
    if (CurrentPntr->sequenceType == LE_CHUNK_ID_SEQ_3DMESH)
    {
      if (!LI_REND3D_GetBoundingBoxFor3DMesh (CurrentPntr, UserProvidedBuffer))
        goto ErrorExit;

      MatrixPntr = &CurrentPntr->dimensionSpecificData.d3->
        sequenceToWorldTransformation.matrix3D;

      PointPntr = UserProvidedBuffer;

      for (i = 0; i < 8; PointPntr++, i++)
        LE_MATRIX_3D_VectorMultiply (PointPntr, MatrixPntr, PointPntr);

      ReturnCode = TRUE;
      goto NormalExit;
    }

    // Traverse through the tree until we get back to the starting point.

    if (CurrentPntr->child != NULL)
      CurrentPntr = CurrentPntr->child;
    else // No more children, try sibling and then parent.
    {
      while (CurrentPntr != SequencePntr && CurrentPntr->sibling == NULL)
        CurrentPntr = CurrentPntr->parent;
      if (CurrentPntr == SequencePntr)
        goto ErrorExit; // All done.  Failed to find 3D mesh.
      CurrentPntr = CurrentPntr->sibling;
    }
  }

ErrorExit:
NormalExit:
  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
  return ReturnCode;
#else // In 2D mode.
  return FALSE;
#endif // CE_ARTLIB_EnableSystemGrafix3D
}



/*****************************************************************************
 * Returns a copy of the sequence header, copied into your user supplied
 * buffer (NULL for the info you aren't interested in).  Returns the type
 * of the sequence if successful, zero (LE_CHUNK_ID_NULL_STANDARD) if it
 * fails.  This function is useful for finding out about a sequence without
 * having to make it start playing.
 */

LE_SEQNCR_SequenceType LE_SEQNCR_GetHeaderFromDataID (
LE_DATA_DataId DataID,
LE_SEQNCR_SequenceChunkHeaderPointer InfoGenericPntr)
{
  LE_SEQNCR_SequenceChunkHeaderRecord   SequenceHeader;
  LE_SEQNCR_SequenceType                SequenceType = LE_CHUNK_ID_NULL_STANDARD;
  LE_CHUNK_StreamID                     StreamID = -1;
  LE_DATA_DataType                      TypeOfDataID;

  memset (&SequenceHeader, 0, sizeof (SequenceHeader));

  if (DataID == LE_DATA_EmptyItem)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LE_SEQNCR_GetHeaderFromDataID: Called with empty argument.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // What were we given?  If it is a chunky file then we can look for all
  // sorts of stuff, if it is just a bitmap or 3D model we handle it more
  // simply.

  TypeOfDataID = LE_DATA_GetLoadedDataType (DataID);

  if (TypeOfDataID == LE_DATA_DataChunky)
  {
    StreamID = LE_CHUNK_ReadFromDataID (DataID);
    if (StreamID < 0)
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_GetHeaderFromDataID: "
        "Unable to open stream on data ID $%08X.\r\n", (int) DataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      goto ErrorExit;  // Unable to open stream, bad DataID?
    }

    SequenceType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_NULL_STANDARD, NULL);

    if (SequenceType < LE_CHUNK_ID_SEQ_GROUPING ||
    SequenceType >= LE_CHUNK_ID_SEQ_MAX)
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_GetHeaderFromDataID: "
        "Got unknown chunk type %d when looking for sequence in data ID $%08X.\r\n",
        (int) SequenceType, (int) DataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      SequenceType = LE_CHUNK_ID_NULL_STANDARD; // Signal an error to the user.
      goto ErrorExit;  // Unknown sequence type or a chunk error.
    }

    // Read the common sequence header, this is part of the fixed
    // size portion of all the sequence chunks.  Of course, some
    // chunk types have a bigger fixed size portion than others,
    // but worry about the extra data later.

    if (LE_CHUNK_ReadChunkData (StreamID, &SequenceHeader,
    sizeof (SequenceHeader)) != sizeof (SequenceHeader))
    {
  #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_GetHeaderFromDataID: "
        "Error while reading the sequence header chunk from data ID $%08X.\r\n",
        (int) DataID);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
      SequenceType = LE_CHUNK_ID_NULL_STANDARD; // Signal an error to the user.
      goto ErrorExit;  // Unable to read the sequence header record.
    }

    // OK, got the header info and type.
  }
  else if (TypeOfDataID == LE_DATA_DataUAP ||
  TypeOfDataID == LE_DATA_DataNative ||
  TypeOfDataID == LE_DATA_DataGenericBitmap)
  {
    SequenceType = LE_CHUNK_ID_SEQ_2DBITMAP;
    SequenceHeader.timeMultiple = CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ;
    SequenceHeader.endingAction = LE_SEQNCR_EndingActionStayAtEnd;
  }
  else if (TypeOfDataID == LE_DATA_DataWave)
  {
    SequenceType = LE_CHUNK_ID_SEQ_SOUND;
    SequenceHeader.endingAction = LE_SEQNCR_EndingActionStop;
    SequenceHeader.endTime = LE_SOUND_GetSoundDuration (DataID);
  }
  else if (TypeOfDataID == LE_DATA_DataMESHX)
  {
    SequenceType = LE_CHUNK_ID_SEQ_3DMESH;
    SequenceHeader.endingAction = LE_SEQNCR_EndingActionStayAtEnd;
  }
  else // Unknown type of DataID.
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SEQNCR_GetHeaderFromDataID: "
      "DataID $%08X has an unknown data type of %d.\r\n",
      (int) DataID, (int) TypeOfDataID);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // Bad data type.
  }

ErrorExit:
  if (StreamID >= 0)
    LE_CHUNK_CloseReadStream (StreamID);

  if (SequenceType > LE_CHUNK_ID_NULL_STANDARD &&
  InfoGenericPntr != NULL)
    *InfoGenericPntr = SequenceHeader;

  return SequenceType;
}



/*****************************************************************************
 * Looks for a currently playing sequence and hunts down a bitmap or other
 * displayable thing (3D model?) which is currently being displayed.  Picks
 * the first one it finds.  Writes the screen coordinates bounding box and
 * DataID of the thing into the memory pointed to by your pointers (NULL to
 * skip it).  Returns TRUE if it found the sequence and an item, FALSE if
 * the sequence isn't running or if it isn't displaying anything.
 */

extern BOOL LE_SEQNCR_GetNowPlayingThingAndBox (
LE_DATA_DataId SequenceDataID, UNS16 SequencePriority, BOOL SearchWholeTree,
LE_DATA_DataId *ThingDataIDPntr,
TYPE_Rect *ScreenBoundingBoxPntr)
{
  LE_SEQNCR_RuntimeInfoPointer  ChildPntr;
  BOOL                          ReturnCode = FALSE;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;
  LE_REND_RenderSlot            SlotNumber;
  LE_REND_RenderSlotSet         SlotSet;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE;  // System not open.

  EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  SequencePntr = FindNextSequence (SequenceDataID, SequencePriority,
    SearchWholeTree, NULL);

  if (SequencePntr != NULL)
  {
    if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_GROUPING)
    {
      // A grouping sequence, examine all children and pick the first
      // one which is concrete (visible, has something, is solid).

      ChildPntr = SequencePntr->child;
      while (ChildPntr != NULL)
      {
        if (ChildPntr->contentsDataID != LE_DATA_EmptyItem)
          break;

        ChildPntr = ChildPntr->sibling;
      }
    }
    else // Non grouping single item sequence, what you see is what you get.
      ChildPntr = SequencePntr;

    if (ChildPntr != NULL && ChildPntr->contentsDataID != LE_DATA_EmptyItem)
    {
      ReturnCode = TRUE;

      if (ThingDataIDPntr != NULL)
        *ThingDataIDPntr = ChildPntr->contentsDataID;

      if (ScreenBoundingBoxPntr != NULL)
      {
        // Now try to find a render slot to get the screen bounding rectangle.

        SlotNumber = 0;
        SlotSet = ChildPntr->renderSlotSet;
        while ((SlotSet & 1) == 0 && SlotNumber < CE_ARTLIB_RendMaxRenderSlots)
        {
          SlotSet >>= 1;
          SlotNumber++;
        }

        // Ask the render slot for the bounding box.

        if (SlotNumber < CE_ARTLIB_RendMaxRenderSlots &&
        LI_SEQNCR_RenderSlotArray [SlotNumber] != NULL)
        {
          if (!LI_SEQNCR_RenderSlotArray[SlotNumber]->
          GetScreenBoundingRectangle (ChildPntr, ScreenBoundingBoxPntr))
          {
            // No rectangle available, set the user's one to an empty rect.

            memset (ScreenBoundingBoxPntr, 0, sizeof (TYPE_Rect));
          }
        }
      }
    }
  }

  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

  // On failure, zero the returned values, just in case the user doesn't
  // check the return code from this function.

  if (!ReturnCode)
  {
    if (ThingDataIDPntr != NULL)
      *ThingDataIDPntr = LE_DATA_EmptyItem;
    if (ScreenBoundingBoxPntr != NULL)
      memset (ScreenBoundingBoxPntr, 0, sizeof (TYPE_Rect));
  }

  return ReturnCode;
}



#if CE_ARTLIB_EnableSystemMouse
/*****************************************************************************
 * The mouse movement callback function.  This moves the given sequence
 * around to follow the mouse position.  Only works for one moveable
 * sequence, since the previous mouse position is in global variables.
 * Of course, if that one sequence is a grouping one then you can have
 * as many ones under that as you need.
 */

void LE_SEQNCR_MouseSequenceCallback (LE_SEQNCR_RuntimeInfoPointer SequencePntr,
LE_SEQNCR_CallbackOperation Opcode)
{
  TYPE_Point2D MousePoint;

  if (Opcode == LE_SEQNCR_CALLOP_POSITION_CALC)
  {
    // See if the position has changed.

    if (LI_SEQNCR_MousePreviousX != LE_MOUSE_MouseX ||
    LI_SEQNCR_MousePreviousY != LE_MOUSE_MouseY)
    {
      // Yes, move the whole mouse group.

      MousePoint.x = LI_SEQNCR_MousePreviousX = LE_MOUSE_MouseX;
      MousePoint.y = LI_SEQNCR_MousePreviousY = LE_MOUSE_MouseY;

      LE_MATRIX_2D_SetTranslate (&MousePoint, &SequencePntr->
        dimensionSpecificData.d2->sequenceToParentTransformation.matrix2D);

      SequencePntr->dimensionSpecificData.d2->
        sequenceToParentTransformNonIdentity = TRUE;

      SequencePntr->needsPositionRecalc = TRUE;
    }
  }
  else if (Opcode == LE_SEQNCR_CALLOP_STARTING)
  {
    LI_SEQNCR_MousePreviousX = 0;
    LI_SEQNCR_MousePreviousY = 0;
  }
}
#endif // CE_ARTLIB_EnableSystemMouse



#if CE_ARTLIB_EnableSystemMouse
/*****************************************************************************
 * Wipes out any previous mouse sequences (and child sequences like mouse
 * pointers) and restarts the mouse grouping sequence.
 */

static BOOL LI_SEQNCR_RestartMouseGroup (void)
{
  if (LE_SEQNCR_MouseGroupingSequenceDataID == LE_DATA_EmptyItem)
    return FALSE; // Mouse group doesn't exist.

  // Stop any existing one, if already running (or command queued to run).

  if (LI_SEQNCR_MouseGroupingSequenceHasBeenStarted)
    LE_SEQNCR_Stop (LE_SEQNCR_MouseGroupingSequenceDataID,
    LE_SEQNCR_MouseGroupingSequencePriority);

  LI_SEQNCR_MouseGroupingSequenceHasBeenStarted = TRUE;

  return LE_SEQNCR_StartTheWorks (
    LE_SEQNCR_MouseGroupingSequenceDataID, LE_SEQNCR_MouseGroupingSequencePriority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    LE_DATA_EmptyItem, 0, FALSE,
    2, NULL, 0, 0, FALSE, 0, FALSE, LE_SEQNCR_NoEndingAction,
    MouseGroupingSequenceRenderSlotSet,
    LE_SEQNCR_MouseSequenceCallback, NULL,
    DefaultPreloadFlagForStartSequence, DefaultDisposalFlagForStartSequence,
    DefaultUseRefFlagForStartSequence, LE_SEQNCR_DropDefault);
}
#endif // CE_ARTLIB_EnableSystemMouse



#if CE_ARTLIB_EnableSystemMouse
/*****************************************************************************
 * Set up the mouse grouping sequence, the actual mouse animations will be
 * added under it.  The grouping sequence will have a callback function to
 * move the sequence around to follow the mouse.  Returns TRUE if successful.
 */

BOOL LE_SEQNCR_InitialiseMouseGroupingSequence (
LE_REND_RenderSlotSet RenderSlotSetToUse, UNS16 Priority)
{
#pragma pack(push,1)
  struct GroupingStruct {
    LE_CHUNK_HeaderRecord                 groupingHeader;
    LE_SEQNCR_SequenceGroupingChunkRecord groupingContents;
    LE_CHUNK_HeaderRecord                 dimensionalityHeader;
    LE_SEQNCR_DimensionalityChunkRecord   dimensionalityContents;
  } GroupingRecord;
#pragma pack(pop)

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // System is closed.

  // Remember the priority & slot for later use.

  LE_SEQNCR_MouseGroupingSequencePriority = Priority;
  MouseGroupingSequenceRenderSlotSet = RenderSlotSetToUse;

  // Create an in-memory grouping sequence with no children.  Start with
  // a fake grouping sequence chunk with a child dimensionality chunk.

  memset (&GroupingRecord, 0, sizeof (GroupingRecord));
  GroupingRecord.groupingHeader.chunkSize = sizeof (GroupingRecord);
  GroupingRecord.groupingHeader.chunkID = LE_CHUNK_ID_SEQ_GROUPING;
  GroupingRecord.groupingContents.commonHeader.timeMultiple = 1;
  GroupingRecord.groupingContents.commonHeader.dropFrames = TRUE;
  GroupingRecord.groupingContents.commonHeader.endingAction =
    LE_SEQNCR_EndingActionLoopToBeginning;
  GroupingRecord.dimensionalityHeader.chunkSize =
    sizeof (LE_SEQNCR_DimensionalityChunkRecord) + sizeof (LE_CHUNK_HeaderRecord);
  GroupingRecord.dimensionalityHeader.chunkID = LE_SEQNCR_CHUNK_ID_DIMENSIONALITY;
  GroupingRecord.dimensionalityContents.dimensionality = 2;

  if (LE_SEQNCR_MouseGroupingSequenceDataID == LE_DATA_EmptyItem)
  {
    LE_SEQNCR_MouseGroupingSequenceDataID =
      LE_DATA_AllocMemoryDataID (sizeof (GroupingRecord), LE_DATA_DataChunky);
  }

  if (LE_SEQNCR_MouseGroupingSequenceDataID == LE_DATA_EmptyItem)
    return FALSE; // Out of memory.

  memcpy (LE_DATA_Use (LE_SEQNCR_MouseGroupingSequenceDataID),
    &GroupingRecord, sizeof (GroupingRecord));

  // Fire off our mouse grouping sequence.  It doesn't have any children
  // yet, but they will be added and removed as the mouse pointer shape
  // changes.  Also stop any previous instance of the same mouse group
  // if it exists.

  if (LI_SEQNCR_RestartMouseGroup ())
    return TRUE;

  // Failed to start it up (usually because the command queue is full),
  // get rid of the allocations.

  LE_SEQNCR_RemoveMouseGroupingSequence ();
  return FALSE;
}
#endif // CE_ARTLIB_EnableSystemMouse



#if CE_ARTLIB_EnableSystemMouse
/*****************************************************************************
 * Shut down the mouse grouping sequence.  Deallocate things etc.
 */

void LE_SEQNCR_RemoveMouseGroupingSequence (void)
{
  LE_DATA_DataId TempDataID;

  if (LE_SEQNCR_MouseGroupingSequenceDataID == LE_DATA_EmptyItem)
    return;

  // Stop other threads from trying to use the mouse runtime stuff,
  // just in case someone tries to change the mouse shape right now.

  TempDataID = LE_SEQNCR_MouseGroupingSequenceDataID;
  LE_SEQNCR_MouseGroupingSequenceDataID = LE_DATA_EmptyItem;
  LI_SEQNCR_MouseGroupingSequenceHasBeenStarted = FALSE;

  // Remove the mouse related sequences from the current play list.

  LE_SEQNCR_Stop (TempDataID, LE_SEQNCR_MouseGroupingSequencePriority);

  // So that the stop takes effect before we deallocate the sequence.
  LE_SEQNCR_ProcessUserCommands ();

  LE_DATA_FreeRuntimeDataID (TempDataID);
}
#endif // CE_ARTLIB_EnableSystemMouse



#if CE_ARTLIB_EnableSystemMouse
/*****************************************************************************
 * Use this to change the mouse shape.  It adds a new sequence to the mouse
 * grouping sequence, any previous ones are optionally removed.  Note that
 * the hotspot offsets are usually negative, to move the image around so
 * that its hot spot lies at the origin.  Use LE_DATA_EmptyItem for the
 * mouse sequence to specify no mouse.  Use zero for the offsets if you
 * want the position stored inside a sequence.  Returns TRUE if successful.
 */

BOOL LE_SEQNCR_AddMouseSubSequenceTheWorks (LE_DATA_DataId MouseSequence,
UNS16 MousePriority, INT16 HotSpotXOffset, INT16 HotSpotYOffset,
BOOL DeleteOtherMouseSequences)
{
  TYPE_Matrix2D                 OffsetMatrix;
  TYPE_Point2D                  OffsetPoint;
  BOOL                          ReturnCode = FALSE;

  if (LE_SEQNCR_MouseGroupingSequenceDataID == LE_DATA_EmptyItem ||
  LI_SEQNCR_CommonRuntimeTreeRoot == NULL)
    return FALSE; // Mouse hasn't been initialised.

  if (DeleteOtherMouseSequences)
  {
    // Remove all children of the mouse group.  These are sequences
    // that the user added.

    if (!LI_SEQNCR_RestartMouseGroup ())
      return FALSE; // Something went wrong, usually command queue full.
  }

  if (MouseSequence == LE_DATA_EmptyItem)
    return TRUE; // Nothing else to do.

  // Add the new user mouse sequence to the mouse group.
  // Offset the new shape by the hot spot offset.  Presume that the
  // sequence doesn't have its own transformation (the normal situation),
  // if it does have a transformation, use zeros for the offset..

  OffsetPoint.x = HotSpotXOffset;
  OffsetPoint.y = HotSpotYOffset;
  LE_MATRIX_2D_SetTranslate (&OffsetPoint, &OffsetMatrix);

  return LE_SEQNCR_StartTheWorks (
    MouseSequence, MousePriority,
    LE_DATA_EmptyItem, 0, FALSE, FALSE,
    LE_SEQNCR_MouseGroupingSequenceDataID, LE_SEQNCR_MouseGroupingSequencePriority, FALSE,
    2,
    (HotSpotXOffset == 0 && HotSpotYOffset == 0) ? NULL : &OffsetMatrix,
    0, 0, FALSE, 0, FALSE,
    LE_SEQNCR_EndingActionLoopToBeginning,
    MouseGroupingSequenceRenderSlotSet,
    NULL, NULL,
    DefaultPreloadFlagForStartSequence, DefaultDisposalFlagForStartSequence,
    DefaultUseRefFlagForStartSequence, LE_SEQNCR_DropDefault);
}
#endif // CE_ARTLIB_EnableSystemMouse



#if CE_ARTLIB_EnableSystemMouse
/*****************************************************************************
 * Simpler mouse sequence specifying function.  Replaces the current mouse
 * with your sequence/bitmap/etc.  If you pass in LE_DATA_EmptyItem then
 * the mouse disappears.
 */

BOOL LE_SEQNCR_AddMouseSubSequence (LE_DATA_DataId MouseSequence)
{
  return
    LE_SEQNCR_AddMouseSubSequenceTheWorks (MouseSequence,
    100 /* Priority doesn't really matter since this will be only child */,
    0, 0, TRUE /* Remove other mouse pointers */);
}
#endif // CE_ARTLIB_EnableSystemMouse



/*****************************************************************************
 * Load a chunky format sequence into memory (it must be a
 * LE_CHUNK_ID_SEQ_GROUPING type of sequence) and change all the bitmaps that
 * it displays from oldTAB to newTAB.  If adjustBoundingBox is TRUE then it
 * will also adjust the bounding box for the bitmap (position and scaling)
 * to match the size of the replacement bitmap (with same scale factor).  If
 * the addRefOnSequence flag is TRUE then it will add a reference count to
 * the sequence data so that it doesn't get unloaded from memory (if it gets
 * unloaded then all the changes made by this function get lost - it reloads
 * the original one from the data file).  Returns the number of bitmaps
 * matching oldTAB, or -1 if an error happens.  Originally written by DanF.
 */

int LE_SEQNCR_ReplaceTABInSequence (LE_DATA_DataId sequenceID,
LE_DATA_DataId oldTAB, LE_DATA_DataId newTAB,
BOOL adjustBoundingBox, BOOL addRefOnSequence)
{
  LE_SEQNCR_2DBoundingBoxChunkPointer     bboxChunk;
  LE_SEQNCR_Sequence2DBitmapChunkPointer  bitmapChunk;
  LE_SEQNCR_SequenceGroupingChunkRecord   groupChunk;
  LE_CHUNK_StreamID                       streamID;
  LE_CHUNK_ID                             chunkType;
  UNS32                                   chunkSize;
  UNS32                                   readAmount;
  int                                     bitmapsReplaced = 0;
  TYPE_Coord2D                            left, top, right, bottom;
  TYPE_Scale2D                            scaleX, scaleY;
  short                                   BitmapWidth, BitmapHeight;

  /*
  ** If the addRefOnSequence flag is specified, increment the
  ** reference counter on this object. It will be up to the user
  ** to decrement the counter before unloading the sequence.
  */
  if (addRefOnSequence)
    LE_DATA_AddRef (sequenceID);
  /*
  ** Open the chunk for reading. By virtue of the fact that we previously
  ** loaded the chunk in memory, the read should take place on the memory
  ** item.
  */
  streamID = LE_CHUNK_ReadFromDataID (sequenceID);
  if (streamID < 0)
    return FALSE;
  /*
  ** The parent chunk is always LE_CHUNK_ID_SEQ_GROUPING ( I ASSUME )
  ** We always have to descend into this chunk before examining others.
  ** It is also necessary to skip all information in that chunk
  ** before descending into the next chunk.
  **
  ** The chunk operation DESCEND will return LE_CHUNK_ID_NULL_STANDARD to
  ** signify that an error occured (can't find grouping chunk or not chunky).
  ** Other chunk operations like READ or MAP return the amount of data read.
  ** In our case, if the amount of data read is not the same as the amount
  ** requested, we treat this as an error.
  ** For all errors, we terminate the procedure immediately.
  */
  chunkType = LE_CHUNK_Descend (streamID, LE_CHUNK_ID_SEQ_GROUPING, &chunkSize);
  if (chunkType == LE_CHUNK_ID_NULL_STANDARD)
    goto ErrorExit;
  chunkSize = LE_CHUNK_ChunkFixedDataSizes [LE_CHUNK_ID_SEQ_GROUPING];
  readAmount = LE_CHUNK_ReadChunkData (streamID, &groupChunk, chunkSize);
  if (readAmount != chunkSize)
    goto ErrorExit;
  /*
  ** Repeatedly read the file finding all LE_CHUNK_ID_SEQ_2DBITMAP chunks.
  ** This is the start of a 'frame' of a sequence. When a DESCEND fails, we
  ** interpret this as the end of file and exit the loop.
  */
  while (TRUE)
  {
    chunkType = LE_CHUNK_Descend (streamID, LE_CHUNK_ID_SEQ_2DBITMAP, &chunkSize);
    if (chunkType == LE_CHUNK_ID_NULL_STANDARD)
      break;
    /*
    ** Map the 2DBITMAP structure and get the address.
    ** If the size read is not the same as the structure size or is NULL, error.
    ** If read was successful, change the bitmap if it matches the
    ** one that is to be replaced.
    */
    chunkSize = LE_CHUNK_ChunkFixedDataSizes [LE_CHUNK_ID_SEQ_2DBITMAP];
    bitmapChunk = (LE_SEQNCR_Sequence2DBitmapChunkPointer)
      LE_CHUNK_MapChunkData (streamID, chunkSize, &readAmount);
    if (!bitmapChunk || readAmount != chunkSize)
      goto ErrorExit;

    if (bitmapChunk->bitmapDataID == LED_TFI (oldTAB))
    {
      bitmapChunk->bitmapDataID = LED_TFI (newTAB); // Replace TAB number in memory.
      bitmapsReplaced++;
      /*
      ** If we changed the image for this 'frame', there may also be
      ** a LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX chunk describing the
      ** position and size of the frame.  If none, nothing to do.
      */
      chunkType = LE_CHUNK_Descend (streamID, LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX, &chunkSize);
      if (chunkType == LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX)
      {
        chunkSize = sizeof (LE_SEQNCR_2DBoundingBoxChunkRecord);
        bboxChunk = (LE_SEQNCR_2DBoundingBoxChunkPointer)
          LE_CHUNK_MapChunkData (streamID, chunkSize, &readAmount);
        if (readAmount != chunkSize)
          goto ErrorExit;
        /*
        ** If the adjustBoundingBox flag is set, modify the old bounding
        ** box to match the same scale for the new image.
        */
        if (adjustBoundingBox)
        {
          /*
          ** The bounding box determines not only the position of the bitmap,
          ** but the size as well. Therefore, we need to determine the scaling
          ** factor used based on the old image dimensions and the current
          ** bounding box dimensions. Once found, we apply these factors to
          ** the new image dimensions to get the new size for the bounding box.
          */
          left   = bboxChunk->boundingRect.left;
          top    = bboxChunk->boundingRect.top;
          right  = bboxChunk->boundingRect.right;
          bottom = bboxChunk->boundingRect.bottom;
          /*
          ** Find the scale factor for the current box and the old image.
          */
          BitmapHeight = LE_GRAFIX_AnyBitmapHeight (oldTAB);
          BitmapWidth = LE_GRAFIX_AnyBitmapWidth (oldTAB);
          scaleX = (float)( right - left ) / (float) BitmapWidth;
          scaleY = (float)( bottom - top ) / (float) BitmapHeight;
          /*
          ** Determine the dimensions of the new bounding box using the
          ** scale factors calculated above and the dimensions of the new image.
          */
          BitmapHeight = LE_GRAFIX_AnyBitmapHeight (newTAB);
          BitmapWidth = LE_GRAFIX_AnyBitmapWidth (newTAB);
          bboxChunk->boundingRect.right  = (long)( bboxChunk->boundingRect.left + BitmapWidth * scaleX );
          bboxChunk->boundingRect.bottom = (long)( bboxChunk->boundingRect.top  + BitmapHeight * scaleY );
        }
        /* Leave the LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX chunk. */
        if (LE_CHUNK_Ascend (streamID) == FALSE)
          goto ErrorExit;
      }
    }
    /* Leave the LE_CHUNK_ID_SEQ_2DBITMAP */
    if (LE_CHUNK_Ascend (streamID) == FALSE)
      goto ErrorExit;
  }
  /* Leave the LE_CHUNK_ID_SEQ_GROUPING chunk. */
  LE_CHUNK_Ascend (streamID);

  LE_CHUNK_CloseReadStream (streamID);
  return bitmapsReplaced;

ErrorExit:
  if (addRefOnSequence)
    LE_DATA_RemoveRef (sequenceID) ;
  LE_CHUNK_CloseReadStream (streamID);
  return -1;
}



/*****************************************************************************
 * Goes through the given sequence and loads and recolours all the bitmaps
 * used by it.  The recolouring is specified by an array of From/To colours.
 * Also, special colours (primary ones - pure red, green, blue, yellow,
 * cyan, magenta, black, white) get coloured faster because they are assumed
 * to be solid (not alpha).  As a side effect, all the bitmaps used by the
 * sequence are loaded into memory.  Returns TRUE if successful.
 */

BOOL LE_SEQNCR_RecolourTABsInSequence (LE_DATA_DataId DataID,
LPCOLORMAP lpColourMapArray, int nColourMapSize)
{
  LE_SEQNCR_Sequence2DBitmapChunkPointer  BitmapChunk;
  LE_DATA_DataId                          BitmapID;
  LE_SEQNCR_SequenceGroupingChunkRecord   GroupChunk;
  LE_CHUNK_StreamID                       StreamID;
  LE_CHUNK_ID                             ChunkType;
  UNS32                                   ChunkSize;
  UNS32                                   ReadAmount;

  StreamID = LE_CHUNK_ReadFromDataID (DataID);
  if (StreamID < 0)
    return FALSE;

  ChunkType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_SEQ_GROUPING, &ChunkSize);
  if (ChunkType == LE_CHUNK_ID_NULL_STANDARD)
    goto ErrorExit;

  ChunkSize = LE_CHUNK_ChunkFixedDataSizes [LE_CHUNK_ID_SEQ_GROUPING];
  ReadAmount = LE_CHUNK_ReadChunkData (StreamID, &GroupChunk, ChunkSize);
  if (ReadAmount != ChunkSize)
    goto ErrorExit;

  while (TRUE)
  {
    ChunkType = LE_CHUNK_Descend (StreamID, LE_CHUNK_ID_SEQ_2DBITMAP, &ChunkSize);
    if (ChunkType == LE_CHUNK_ID_NULL_STANDARD)
      break;

    ChunkSize = LE_CHUNK_ChunkFixedDataSizes [LE_CHUNK_ID_SEQ_2DBITMAP];
    BitmapChunk = (LE_SEQNCR_Sequence2DBitmapChunkPointer)
      LE_CHUNK_MapChunkData (StreamID, ChunkSize, &ReadAmount);
    if (!BitmapChunk || ReadAmount != ChunkSize)
      goto ErrorExit;

    if (BitmapChunk->commonHeader.absoluteDataIDs)
      BitmapID = BitmapChunk->bitmapDataID;
    else
      BitmapID = LE_DATA_IdWithFileFromParent (BitmapChunk->bitmapDataID, DataID);

    if (!LE_GRAFIXBulkRecolorTAB (BitmapID, lpColourMapArray, nColourMapSize))
      goto ErrorExit; // Recolouring failed.

    /* Leave the LE_CHUNK_ID_SEQ_2DBITMAP */
    if (LE_CHUNK_Ascend (StreamID) == FALSE)
      goto ErrorExit;
  }
  /* Leave the LE_CHUNK_ID_SEQ_GROUPING chunk. */
  LE_CHUNK_Ascend (StreamID);

  LE_CHUNK_CloseReadStream (StreamID);
  return TRUE;

ErrorExit:
  LE_CHUNK_CloseReadStream (StreamID);
  return FALSE;
}



/*****************************************************************************
 * Make a runtime chunky file that can be used for starting up a video.
 * Returns a DataID you can use (use LE_DATA_FreeRuntimeDataID when done)
 * to start a video, returns LE_DATA_EmptyItem if something went wrong.
 */

LE_DATA_DataId LE_SEQNCR_CreateVideoObject (
  char *FileName, BOOL DrawSolid, UNS16 AlphaLevel,
  TYPE_RectPointer OptionalDestinationRect,
  BOOL FlipVideoVertically, BOOL EnableVideo, BOOL EnableAudio,
  BOOL DrawDirectlyToScreen, BOOL DoubleAlternateLines,
  int Saturation, int Brightness, int Contrast)
{
#pragma pack(push,1)
  struct VideoGroupStruct {
    LE_CHUNK_HeaderRecord                 firstHeader;
    LE_SEQNCR_SequenceVideoChunkRecord    videoContents;
    LE_CHUNK_HeaderRecord                 secondHeader;
    LE_SEQNCR_2DBoundingBoxChunkRecord    boundingBox;
    LE_CHUNK_HeaderRecord                 thirdHeader;
    LE_SEQNCR_DimensionalityChunkRecord   dimensionContents;
    LE_CHUNK_HeaderRecord                 fourthHeader;
    char                                  fileName [MAX_PATH];
  } VideoGroup;
#pragma pack(pop)

  int             GroupWithoutNameLength;
  int             NameLength;
  LE_DATA_DataId  RuntimeDataID;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL || FileName == NULL)
    return LE_DATA_EmptyItem; // System is closed.

  GroupWithoutNameLength = sizeof (VideoGroup) - sizeof (VideoGroup.fileName);
  NameLength = strlen (FileName) + 1 /* For NUL at end */;
  if (NameLength > MAX_PATH)
    NameLength = MAX_PATH;

  memset (&VideoGroup, 0, sizeof (VideoGroup));
  VideoGroup.firstHeader.chunkSize = GroupWithoutNameLength + NameLength;
  VideoGroup.firstHeader.chunkID = LE_CHUNK_ID_SEQ_VIDEO;
  VideoGroup.videoContents.commonHeader.timeMultiple = 1;
  VideoGroup.videoContents.commonHeader.endingAction =
    LE_SEQNCR_EndingActionStop;
  VideoGroup.videoContents.drawSolid = (UNS8) DrawSolid;
  VideoGroup.videoContents.flipVertically = (UNS8) FlipVideoVertically;
  VideoGroup.videoContents.alphaLevel = (UNS8) AlphaLevel;
  VideoGroup.videoContents.enableVideo = (UNS8) EnableVideo;
  VideoGroup.videoContents.enableAudio = (UNS8) EnableAudio;
  VideoGroup.videoContents.drawDirectlyToScreen = (UNS8) DrawDirectlyToScreen;
  VideoGroup.videoContents.doubleAlternateLines = DoubleAlternateLines;
  VideoGroup.videoContents.saturation = Saturation;
  VideoGroup.videoContents.brightness = Brightness;
  VideoGroup.videoContents.contrast = Contrast;

  VideoGroup.secondHeader.chunkSize =
    sizeof (LE_SEQNCR_2DBoundingBoxChunkRecord) + sizeof (LE_CHUNK_HeaderRecord);
  if (OptionalDestinationRect == NULL)
    VideoGroup.secondHeader.chunkID = 255; // An unknown private chunk.
  else
  {
    VideoGroup.secondHeader.chunkID = LE_SEQNCR_CHUNK_ID_2D_BOUNDING_BOX;
    VideoGroup.boundingBox.boundingRect = *OptionalDestinationRect;
  }

  VideoGroup.thirdHeader.chunkSize =
    sizeof (LE_SEQNCR_DimensionalityChunkRecord) + sizeof (LE_CHUNK_HeaderRecord);
  VideoGroup.thirdHeader.chunkID = LE_SEQNCR_CHUNK_ID_DIMENSIONALITY;
  VideoGroup.dimensionContents.dimensionality = 2;

  VideoGroup.fourthHeader.chunkSize = sizeof (LE_CHUNK_HeaderRecord) + NameLength;
  VideoGroup.fourthHeader.chunkID = LE_CHUNK_ID_FILE_NAME_1;
  strncpy (VideoGroup.fileName, FileName, NameLength - 1);

  RuntimeDataID = LE_DATA_AllocMemoryDataID (
    GroupWithoutNameLength + NameLength, LE_DATA_DataChunky);

  if (RuntimeDataID == LE_DATA_EmptyItem) // Failed memory.
    return LE_DATA_EmptyItem;

  memcpy (LE_DATA_Use (RuntimeDataID), &VideoGroup,
    GroupWithoutNameLength + NameLength);

  return RuntimeDataID;
}



/*****************************************************************************
 * Make a runtime chunky file that can be used for starting up an external
 * audio WAVE file, using spooling.  Returns a DataID you can use (use
 * LE_DATA_FreeRuntimeDataID when done) to start the audio, returns
 * LE_DATA_EmptyItem if something went wrong.
 *
 * The filename argument is obvious, it should be in RIFF/WAVE format and can
 * be compressed (use the Windows SoundRecorder and pick Save As to do
 * compression, or use the WaveFC utility for batch compressions).  You can
 * also specify the ending action (LE_SEQNCR_EndingActionLoopToBeginning or
 * LE_SEQNCR_EndingActionStop), the panning (-100 for full left to +100 for full
 * right), and the volume (0 for quiet to 100 for loud).  Use a dimensionality
 * of 0 for sounds you want to pan, 2 for sounds that have panning automatically
 * set by there position on the screen (you can move them around like other
 * sequences), or 3 for 3D positioning.
 */

LE_DATA_DataId LE_SEQNCR_CreateAudioObject (char *FileName,
  INT8 PanPercentage, UNS8 VolumePercentage,
  LE_SEQNCR_EndingAction EndingAction, UNS8 Dimensionality)
{
#pragma pack(push,1)
  struct AudioGroupStruct {
    LE_CHUNK_HeaderRecord                 firstHeader;
    LE_SEQNCR_SequenceSoundChunkRecord    audioContents;
    LE_CHUNK_HeaderRecord                 secondHeader;
    LE_SEQNCR_DimensionalityChunkRecord   dimensionContents;
    LE_CHUNK_HeaderRecord                 thirdHeader;
    LE_SEQNCR_SoundVolumeChunkRecord      volumeContents;
    LE_CHUNK_HeaderRecord                 fourthHeader;
    LE_SEQNCR_SoundPanningChunkRecord     panningContents;
    LE_CHUNK_HeaderRecord                 fifthHeader;
    char                                  fileName [MAX_PATH];
  } AudioGroup;
#pragma pack(pop)

  int             GroupWithoutNameLength;
  int             NameLength;
  LE_DATA_DataId  RuntimeDataID;

  if (LI_SEQNCR_CommonRuntimeTreeRoot == NULL || FileName == NULL)
    return LE_DATA_EmptyItem; // System is closed.

  if (EndingAction == LE_SEQNCR_NoEndingAction ||
  EndingAction >= LE_SEQNCR_EndingActionMAX)
    EndingAction = LE_SEQNCR_EndingActionStop;

  GroupWithoutNameLength = sizeof (AudioGroup) - sizeof (AudioGroup.fileName);
  NameLength = strlen (FileName) + 1 /* For NUL at end */;
  if (NameLength > MAX_PATH)
    NameLength = MAX_PATH;

  memset (&AudioGroup, 0, sizeof (AudioGroup));
  AudioGroup.firstHeader.chunkSize = GroupWithoutNameLength + NameLength;
  AudioGroup.firstHeader.chunkID = LE_CHUNK_ID_SEQ_SOUND;
  AudioGroup.audioContents.commonHeader.timeMultiple =
    CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ; // Spool once per second.
  AudioGroup.audioContents.commonHeader.endingAction = EndingAction;
  AudioGroup.audioContents.soundDataID = LE_DATA_EmptyItem;

  AudioGroup.secondHeader.chunkSize =
    sizeof (LE_SEQNCR_DimensionalityChunkRecord) + sizeof (LE_CHUNK_HeaderRecord);
  AudioGroup.secondHeader.chunkID = LE_SEQNCR_CHUNK_ID_DIMENSIONALITY;
  AudioGroup.dimensionContents.dimensionality = Dimensionality;

  AudioGroup.thirdHeader.chunkSize =
    sizeof (LE_SEQNCR_SoundVolumeChunkRecord) + sizeof (LE_CHUNK_HeaderRecord);
  AudioGroup.thirdHeader.chunkID = LE_SEQNCR_CHUNK_ID_SET_SOUND_VOLUME;
  AudioGroup.volumeContents.soundVolume = VolumePercentage;

  AudioGroup.fourthHeader.chunkSize =
    sizeof (LE_SEQNCR_SoundPanningChunkRecord) + sizeof (LE_CHUNK_HeaderRecord);
  AudioGroup.fourthHeader.chunkID = LE_SEQNCR_CHUNK_ID_SET_SOUND_PANNING;
  AudioGroup.panningContents.soundPanning = PanPercentage;

  AudioGroup.fifthHeader.chunkSize = sizeof (LE_CHUNK_HeaderRecord) + NameLength;
  AudioGroup.fifthHeader.chunkID = LE_CHUNK_ID_FILE_NAME_1;
  strncpy (AudioGroup.fileName, FileName, NameLength - 1);

  RuntimeDataID = LE_DATA_AllocMemoryDataID (
    GroupWithoutNameLength + NameLength, LE_DATA_DataChunky);

  if (RuntimeDataID == LE_DATA_EmptyItem) // Failed memory.
    return LE_DATA_EmptyItem;

  memcpy (LE_DATA_Use (RuntimeDataID), &AudioGroup,
    GroupWithoutNameLength + NameLength);

  return RuntimeDataID;
}



#if CE_ARTLIB_EnableSystemBinkStandAlone
/*****************************************************************************
 * Stops the sequencer from running and plays the Bink video on the screen,
 * centered with the rest set to black.  Returns when done or when the
 * user clicks a mouse or hits a key.  You may be able to get a slight
 * performance improvement if you turn off the timer system while playing
 * the video.  If DoubleSize is TRUE then it doubles the size (temporarily
 * allocates a single size off screen bitmap in video memory (if possible)
 * decompresses to that and does a double size blit to the real screen).
 * Returns TRUE if it got to the end of the video successfully, FALSE
 * otherwise (error or key pressed).
 */

BOOL LE_SEQNCR_TakeOverAndPlayBinkVideo (char *FileName, BOOL DoubleSize /* = FALSE */)
{
  HBINK             BinkFile = NULL;
  DDBLTFX           BlitEffects;
  DDCAPS            DDCapabilities;
  RECT              DoubleSizeDestinationRect;
  HRESULT           ErrorCode;
  char              FilePath [MAX_PATH];
  BOOL              FirstTimeClear = TRUE;
  LPBYTE            lpSurfaceBits;
  PDDSURFACE        OffScreenSingleSizeSurface = NULL;
  UNS32             OutputDepthFlags;
  BOOL              ReturnCode = FALSE;
  DDSURFDESC        SurfaceDescription;

  if (LE_GRAFIX_DDPrimarySurfaceN == NULL)
    return FALSE;

#if CE_ARTLIB_EnableSystemDirIni
  if (!LE_DIRINI_ExpandFileName (FileName, FilePath, NULL))
    return FALSE; // Couldn't find file.
#else
  strcpy (FilePath, FileName);
#endif // CE_ARTLIB_EnableSystemDirIni

  OutputDepthFlags = BinkDDSurfaceType (LE_GRAFIX_DDPrimarySurfaceN);

  if (OutputDepthFlags == -1) // Unsupported screen depth?
    return FALSE;

  // Force the animation engine to stop while the video is playing.

#if CE_ARTLIB_EnableMultitasking
  if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
    EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#endif

  #if CE_ARTLIB_EnableSystemSound
  if (LI_SOUND_DirectSoundObject != NULL)
    BinkSoundUseDirectSound (LI_SOUND_DirectSoundObject);
  #endif

  BinkFile = BinkOpen (FilePath, 0);
  if (BinkFile == NULL)
    goto ErrorExit;

  // If running double size, create a temporary offscreen bitmap to
  // hold the single size decompress buffer, in video memory.  Check
  // and see if it can STRETCH blit from the video / system off screen
  // buffer to the screen before allocating it.  If the creation failed,
  // we continue on in single size mode.

  while (DoubleSize) // Really just an "if" using "break" to exit early.
  {
    // Get the pixel format used by the real screen.

    ZeroMemory (&SurfaceDescription, sizeof (SurfaceDescription));
    SurfaceDescription.dwSize = sizeof (SurfaceDescription);
    ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->GetSurfaceDesc (&SurfaceDescription);
    if (ErrorCode != DD_OK)
      break;
    SurfaceDescription.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_WIDTH;
    SurfaceDescription.dwHeight = BinkFile->Height;
    SurfaceDescription.dwWidth = BinkFile->Width;
    ZeroMemory (&SurfaceDescription.ddsCaps, sizeof (SurfaceDescription.ddsCaps));

    // Get the hardware capabilities.

    memset (&DDCapabilities, 0, sizeof (DDCapabilities));
    DDCapabilities.dwSize = sizeof (DDCapabilities);
    ErrorCode = LE_GRAFIX_DirectDrawObjectN->GetCaps (
      &DDCapabilities /* Driver capabilities */,
      NULL /* Emulation layer capabilities */);
    if (ErrorCode != DD_OK)
      break;

    // Try for an offscreen bitmap in video memory first, video to video blits.

    if ((DDCapabilities.dwCaps & DDCAPS_BLT) &&
    (DDCapabilities.dwCaps & DDCAPS_BLTSTRETCH) &&
    (DDCapabilities.dwFXCaps & DDFXCAPS_BLTSTRETCHX) &&
    (DDCapabilities.dwFXCaps & DDFXCAPS_BLTSTRETCHY))
    {
      SurfaceDescription.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;

      ErrorCode = LE_GRAFIX_DirectDrawObjectN->CreateSurface (
        &SurfaceDescription, &OffScreenSingleSizeSurface, NULL);
    }

    // If that didn't work, try for system memory, system to video blits.

    if (OffScreenSingleSizeSurface == NULL &&
    (DDCapabilities.dwSVBCaps & DDCAPS_BLT) &&
    (DDCapabilities.dwSVBCaps & DDCAPS_BLTSTRETCH) &&
    (DDCapabilities.dwSVBFXCaps & DDFXCAPS_BLTSTRETCHX) &&
    (DDCapabilities.dwSVBFXCaps & DDFXCAPS_BLTSTRETCHY))
    {
      SurfaceDescription.ddsCaps.dwCaps = 0;

      ErrorCode = LE_GRAFIX_DirectDrawObjectN->CreateSurface (
        &SurfaceDescription, &OffScreenSingleSizeSurface, NULL);
    }

    if (OffScreenSingleSizeSurface == NULL)
      break; // Failed, use single size video playback.

    DoubleSizeDestinationRect.left =
      (LE_GRAFIX_RealScreenWidthInPixels - 2 * (int) SurfaceDescription.dwWidth) / 2;
    if (DoubleSizeDestinationRect.left < 0)
      DoubleSizeDestinationRect.left = 0;
    DoubleSizeDestinationRect.right =
      DoubleSizeDestinationRect.left + 2 * SurfaceDescription.dwWidth;
    if (DoubleSizeDestinationRect.right > LE_GRAFIX_RealScreenWidthInPixels)
      DoubleSizeDestinationRect.right = LE_GRAFIX_RealScreenWidthInPixels;

    DoubleSizeDestinationRect.top =
      (LE_GRAFIX_RealScreenHeightInPixels - 2 * (int) SurfaceDescription.dwHeight) / 2;
    if (DoubleSizeDestinationRect.top < 0)
      DoubleSizeDestinationRect.top = 0;
    DoubleSizeDestinationRect.bottom =
      DoubleSizeDestinationRect.top + 2 * SurfaceDescription.dwHeight;
    if (DoubleSizeDestinationRect.bottom > LE_GRAFIX_RealScreenHeightInPixels)
      DoubleSizeDestinationRect.bottom = LE_GRAFIX_RealScreenHeightInPixels;

    break; // This isn't really a while loop.  Continue on to player.
  }

  // Play the video.

  while (TRUE)
  {
    if (BinkWait (BinkFile))
    {
      // Check for keyboard and mouse activity, which will cancel the video.

      #if CE_ARTLIB_EnableMultitasking // Other thread is reading keyboard for us.
        LE_QUEUE_MessageRecord  Message;

        if (LE_UIMSG_ReceiveEvent (&Message))
        {
          LE_QUEUE_FreeStorageAssociatedWithMessage (&Message);
          if (Message.messageCode == UIMSG_KEYBOARD_PRESSED ||
          Message.messageCode == UIMSG_MOUSE_LEFT_DOWN)
            goto ErrorExit; // User abort.
        }
      #else // Single tasking mode, this thread is also the window thread.
        MSG Message;

        if (PeekMessage (&Message, LE_MAIN_HwndMainWindow, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE) ||
        PeekMessage (&Message, LE_MAIN_HwndMainWindow, WM_LBUTTONDOWN, WM_LBUTTONDOWN, PM_REMOVE))
          goto ErrorExit;
      #endif
    }
    else // A frame is ready.
    {
      // Decompress the frame.

      BinkDoFrame (BinkFile);

      // Copy the frame to the screen.

      #if CE_ARTLIB_EnableMultitasking
      if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) goto ErrorExit; // System is closed.
      EnterCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
      if (LE_GRAFIX_LockedSurfaceCriticalSectionPntr == NULL) goto ErrorExit; // Closed while we waited.
      #endif

      // Clear the screen to black the very first time.

      if (FirstTimeClear)
      {
        memset (&BlitEffects, 0, sizeof (BlitEffects));
        BlitEffects.dwSize = sizeof (BlitEffects);
        BlitEffects.dwFillColor= 0;

        LE_GRAFIX_DDPrimarySurfaceN->Blt (
          NULL /* Whole screen */, NULL, NULL,
          DDBLT_WAIT | DDBLT_COLORFILL, &BlitEffects);

        FirstTimeClear = FALSE;
      }

      if (OffScreenSingleSizeSurface == NULL)
      {
        // Decompress single size directly to the screen.

        SurfaceDescription.dwSize = sizeof (SurfaceDescription);
        SurfaceDescription.dwFlags = 0;

        ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->Lock (
          NULL /* Whole surface */, &SurfaceDescription,
          DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

        if (ErrorCode == DD_OK)
        {
          lpSurfaceBits = (LPBYTE) SurfaceDescription.lpSurface;

          // Copy the image from the Bink internal buffer to our output buffer.

          BinkCopyToBuffer (BinkFile,
            SurfaceDescription.lpSurface,
            SurfaceDescription.lPitch,
            SurfaceDescription.dwHeight,
            (SurfaceDescription.dwWidth - BinkFile->Width) / 2,
            (SurfaceDescription.dwHeight - BinkFile->Height) / 2,
            OutputDepthFlags);

          LE_GRAFIX_DDPrimarySurfaceN->Unlock (NULL /* Whole surface */);
        }
        else if (ErrorCode == DDERR_SURFACELOST)
          LE_GRAFIX_DDPrimarySurfaceN->Restore ();
      }
      else // Double size display.
      {
        SurfaceDescription.dwSize = sizeof (SurfaceDescription);
        SurfaceDescription.dwFlags = 0;

        ErrorCode = OffScreenSingleSizeSurface->Lock (
          NULL /* Whole surface */, &SurfaceDescription,
          DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

        if (ErrorCode == DD_OK)
        {
          lpSurfaceBits = (LPBYTE) SurfaceDescription.lpSurface;

          // Copy the image from the Bink internal buffer to our off screen buffer.

          BinkCopyToBuffer (BinkFile,
            SurfaceDescription.lpSurface,
            SurfaceDescription.lPitch,
            SurfaceDescription.dwHeight,
            (SurfaceDescription.dwWidth - BinkFile->Width) / 2,
            (SurfaceDescription.dwHeight - BinkFile->Height) / 2,
            OutputDepthFlags);

          OffScreenSingleSizeSurface->Unlock (NULL /* Whole surface */);
        }
        else if (DDERR_SURFACELOST == ErrorCode)
          OffScreenSingleSizeSurface->Restore ();

        // Do a double size stretch blit from the off screen surface
        // to the real screen.

        ErrorCode = LE_GRAFIX_DDPrimarySurfaceN->Blt (
          &DoubleSizeDestinationRect, OffScreenSingleSizeSurface,
          NULL /* Entire source rect */, DDBLT_WAIT, NULL /* BlitEffects */);

        if (ErrorCode == DDERR_SURFACELOST)
          LE_GRAFIX_DDPrimarySurfaceN->Restore ();
      }

      #if CE_ARTLIB_EnableMultitasking
      LeaveCriticalSection (LE_GRAFIX_LockedSurfaceCriticalSectionPntr);
      #endif

      if (ErrorCode != DD_OK && ErrorCode != DDERR_SURFACELOST)
        goto ErrorExit; // Something went wrong, now safe to handle error outside critical section.

      // Update the current frame number and advance to the next frame.
      // Bink automatically goes back to the first frame when it hits
      // the end, so detect that if possible.

      if (BinkFile->FrameNum < BinkFile->Frames)
        BinkNextFrame (BinkFile);
      else
        break; // Finished playing.
    }
  }

  ReturnCode = TRUE;

ErrorExit:
#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#endif

  if (BinkFile != NULL)
    BinkClose (BinkFile);

  if (OffScreenSingleSizeSurface != NULL)
    OffScreenSingleSizeSurface->Release ();

  return ReturnCode;
}
#endif // CE_ARTLIB_EnableSystemBinkStandAlone



/*****************************************************************************
 * Iterates over all active sequences and calls the callback function for
 * some of them.  Returns the number of times it called the callback
 * function, or -1 if something went wrong.  The sequences passed to the
 * callback function have to be fully inside the rectangle (in virtual screen
 * coordinates) identified by TouchingRectanglePntr.  If the rectangle has
 * zero width and height then they only have to contain that point.  If the
 * rectangle is NULL then no rectangle test is done.  As well, the sequences
 * have to be displayable (3D models, images, sounds) unless you set the
 * IncludeGroupingSequences flag to TRUE.  You can also specify the render
 * slot set to consider if you want to restrict it even more (otherwise use
 * -1 for all slots).  The UserValue is passed as-is to the callback function
 * every time it is called, usually it is a pointer to state information
 * needed by the callback.
 */

int LI_SEQNCR_ForAllActiveSequences (
TYPE_RectPointer TouchingRectanglePntr,
BOOL IncludeGroupingSequences,
LE_REND_RenderSlotSet RenderSlotSet,
LI_SEQNCR_IterationCallbackFunctionPointer CallbackFunctionPntr,
void *UserValue)
{
  int                               CallbackCount = 0;
  LE_REND_RenderSlotSet             InOurSlotSet;
  LE_REND_RenderSlot                RenderSlot;
  LI_REND_AbstractRenderSlotPointer RenderSlotPntr;
  LE_SEQNCR_RuntimeInfoPointer      SequencePntr;

#if CE_ARTLIB_EnableMultitasking
  if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
    EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#endif
  // Test the sequencer status again, since it may have closed while
  // we were waiting to enter the critical section, and deleting the
  // critical section will free us to run again (don't exit the
  // the critical section in this case, since it got deleted).
  SequencePntr = LI_SEQNCR_CommonRuntimeTreeRoot;
  if (SequencePntr == NULL)
    return -1; // Sequencer isn't running now.

  while (TRUE)
  {
    // Process the current node.

    InOurSlotSet = (SequencePntr->renderSlotSet & RenderSlotSet);

    // Only call the callback for ones in matching slots, which also
    // contain the touching rectangle and aren't grouping ones,
    // unless the user wants grouping ones too.

    if (InOurSlotSet &&
    (IncludeGroupingSequences ||
    (SequencePntr->sequenceType != LE_CHUNK_ID_SEQ_GROUPING &&
    SequencePntr->sequenceType != LE_CHUNK_ID_SEQ_INDIRECT)))
    {
      for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
      {
        if (((1 << RenderSlot) & InOurSlotSet) == 0)
          continue; // Not in our desired set of slots.

        RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];
        if (RenderSlotPntr == NULL)
          continue; // Render slot doesn't exist.

        if (TouchingRectanglePntr != NULL &&
        !RenderSlotPntr->SequenceTouchesScreenRectangle (SequencePntr,
        TouchingRectanglePntr))
          continue; // Not inside the touching rectangle, if specified.

        if (!CallbackFunctionPntr (RenderSlotPntr, SequencePntr, UserValue))
          goto ErrorExit; // Callback requested an abort.
        CallbackCount++;
      }
    }

    // Traverse down the tree, then do siblings, then back up to the
    // next parent's sibling.  Ignore nodes not in our slot since
    // their children will definitely not be in our slot.

    if (SequencePntr->child != NULL && InOurSlotSet)
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

ErrorExit:
#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#endif

  return CallbackCount;
}



/*****************************************************************************
 * Find the next active sequence in the given render slot which is under
 * the given rectangle in screen coordinates (or contains that point if
 * the rectangle has zero width or is anywhere if the rectangle is NULL).
 * If a previous label is given then it finds the next one, otherwise it
 * finds the first one.  Returns the label of the sequence found, or 0 if
 * none, also sets the render slot where it found it, if ReturnedRenderSlot
 * is not NULL.
 */

typedef struct LI_SEQNCR_FindNextLabelStruct
{
  UNS8                                foundLabel;
  UNS8                                previousLabel;
  LI_REND_AbstractRenderSlotPointer  *returnedRenderSlot;
} LI_SEQNCR_FindNextLabelRecord, *LI_SEQNCR_FindNextLabelPointer;

BOOL LI_SEQNCR_FindNextLabelCallback (
  LI_REND_AbstractRenderSlotPointer RenderSlotPntr,
  LE_SEQNCR_RuntimeInfoPointer SequencePntr, void *UserValue)
{
  LI_SEQNCR_FindNextLabelPointer FindNextLabelInfo;

  FindNextLabelInfo = (LI_SEQNCR_FindNextLabelPointer) UserValue;

  if (FindNextLabelInfo->previousLabel != 0 &&
  FindNextLabelInfo->previousLabel == SequencePntr->labelNumber)
  {
    // Found the previous label, set things up so that the next
    // one we encounter will be the found one.

    FindNextLabelInfo->previousLabel = 0;
    return TRUE; // Continue iterating.
  }

  if (FindNextLabelInfo->previousLabel == 0)
  {
    // Found it!  Save values and stop iterating.

    FindNextLabelInfo->foundLabel = SequencePntr->labelNumber;
    if (FindNextLabelInfo->returnedRenderSlot != NULL)
      *(FindNextLabelInfo->returnedRenderSlot) = RenderSlotPntr;
    return FALSE; // Done!
  }

  return TRUE; // Continue iterating.
}

UNS8 LE_SEQNCR_FindNextLabeledSequenceUnderScreenRectangle (
  UNS8 PreviousLabel /* = 0 */,
  TYPE_RectPointer RectanglePntr /* = NULL */,
  LE_REND_RenderSlotSet RenderSlotSet /* = (LE_REND_RenderSlotSet) -1 */,
  LI_REND_AbstractRenderSlotPointer *ReturnedRenderSlot /* = NULL */,
  BOOL IncludeGroups /* = FALSE */)
{
  LI_SEQNCR_FindNextLabelRecord FindNextLabelInfo;

  FindNextLabelInfo.foundLabel = 0;
  FindNextLabelInfo.previousLabel = PreviousLabel;
  FindNextLabelInfo.returnedRenderSlot = ReturnedRenderSlot;

  LI_SEQNCR_ForAllActiveSequences (RectanglePntr, IncludeGroups,
    RenderSlotSet, LI_SEQNCR_FindNextLabelCallback, &FindNextLabelInfo);

  return FindNextLabelInfo.foundLabel;
}



#if CE_ARTLIB_SeqncrUseSerialNumbers
/*****************************************************************************
 * Find the next active sequence in the given render slot which is under
 * the given rectangle in screen coordinates (or contains that point if
 * the rectangle has zero width or is anywhere if the rectangle is NULL).
 * If a previous serial number is given then it finds the next one, otherwise
 * it finds the first one.  Returns the serial number of the sequence found,
 * or 0 if none, also sets the render slot where it found it, if
 * ReturnedRenderSlot is not NULL.
 */

typedef struct LI_SEQNCR_FindNextSerialStruct
{
  LE_CHUNK_SerialNumber               foundSerial;
  LE_CHUNK_SerialNumber               previousSerial;
  LI_REND_AbstractRenderSlotPointer  *returnedRenderSlot;
} LI_SEQNCR_FindNextSerialRecord, *LI_SEQNCR_FindNextSerialPointer;

BOOL LI_SEQNCR_FindNextSerialCallback (
LI_REND_AbstractRenderSlotPointer RenderSlotPntr,
LE_SEQNCR_RuntimeInfoPointer SequencePntr, void *UserValue)
{
  LI_SEQNCR_FindNextSerialPointer FindNextSerialInfo;

  if (SequencePntr->serialNumber == 0)
    return TRUE; // No serial number for this sequence, continue iterating.

  FindNextSerialInfo = (LI_SEQNCR_FindNextSerialPointer) UserValue;

  if (FindNextSerialInfo->previousSerial != 0 &&
  FindNextSerialInfo->previousSerial == SequencePntr->serialNumber)
  {
    // Found the previous serial number, set things up so that
    // the next one we encounter will be the found one.

    FindNextSerialInfo->previousSerial = 0;
    return TRUE; // Continue iterating.
  }

  if (FindNextSerialInfo->previousSerial == 0)
  {
    // Found it!  Save values and stop iterating.

    FindNextSerialInfo->foundSerial = SequencePntr->serialNumber;
    if (FindNextSerialInfo->returnedRenderSlot != NULL)
      *(FindNextSerialInfo->returnedRenderSlot) = RenderSlotPntr;
    return FALSE; // Done!
  }

  return TRUE; // Continue iterating.
}

LE_CHUNK_SerialNumber LE_SEQNCR_FindNextSerialNumberUnderScreenRectangle (
  LE_CHUNK_SerialNumber PreviousSerialNumber /* = 0 */,
  TYPE_RectPointer RectanglePntr /* = NULL */,
  LE_REND_RenderSlotSet RenderSlotSet /* = (LE_REND_RenderSlotSet) -1 */,
  LI_REND_AbstractRenderSlotPointer *ReturnedRenderSlot /* = NULL */,
  BOOL IncludeGroups /* = FALSE */)
{
  LI_SEQNCR_FindNextSerialRecord FindNextSerialInfo;

  FindNextSerialInfo.foundSerial = 0;
  FindNextSerialInfo.previousSerial = PreviousSerialNumber;
  FindNextSerialInfo.returnedRenderSlot = ReturnedRenderSlot;

  LI_SEQNCR_ForAllActiveSequences (RectanglePntr, IncludeGroups,
    RenderSlotSet, LI_SEQNCR_FindNextSerialCallback, &FindNextSerialInfo);

  return FindNextSerialInfo.foundSerial;
}
#endif // CE_ARTLIB_SeqncrUseSerialNumbers



/*****************************************************************************
 *****************************************************************************
 ** SYSTEM STARTUP AND SHUTDOWN FUNCTIONS                                   **
 *****************************************************************************
 ****************************************************************************/

/*****************************************************************************
 * Initialise the sequence playing module.  Halts on failure.
 */

void LI_SEQNCR_InitSystem (void)
{
  int i;
  LE_SEQNCR_RuntimeInfoPointer SequencePntr;

  if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
    return; // Is already initialised.

  // Miscellaneous inits.

  AVIFileInit ();

  LE_QUEUE_Initialize (&SequencerCommandQueue, CE_ARTLIB_SeqncrMaxCollectedCommands);
  InitializeCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
  InitializeCriticalSection (&PreLoaderCriticalSection);
  InitializeCriticalSection (&PostLoaderCriticalSection);

#if CE_ARTLIB_SeqncrMaxRecolourRanges > 0
  for (i = 0; i < CE_ARTLIB_SeqncrMaxRecolourRanges; i++)
    LE_SEQNCR_AutoRecolourRangesArray[i].firstInRange = LE_DATA_EmptyItem;
#endif /* CE_ARTLIB_SeqncrMaxRecolourRanges > 0 */

  // Initialise the free lists for the runtime tree.

  memset (&RuntimeCommonArray, 0, sizeof (RuntimeCommonArray));
  NextFreeCommonRuntimeElement = RuntimeCommonArray + 0;
  for (i = 0; i < CE_ARTLIB_SeqncrMaxActiveSequences - 1; i++)
    *((LE_SEQNCR_RuntimeInfoPointer *) (RuntimeCommonArray + i)) = RuntimeCommonArray + i + 1;
#if CE_ARTLIB_EnableDebugMode
  NumberOfFreeCommonRuntimeElements = CE_ARTLIB_SeqncrMaxActiveSequences;
#endif

  memset (&Runtime2DArray, 0, sizeof (Runtime2DArray));
  NextFree2DRuntimeElement = Runtime2DArray + 0;
  for (i = 0; i < CE_ARTLIB_SeqncrMax2DSequences - 1; i++)
    *((LE_SEQNCR_Runtime2DInfoPointer *) (Runtime2DArray + i)) = Runtime2DArray + i + 1;
#if CE_ARTLIB_EnableDebugMode
  NumberOfFree2DRuntimeElements = CE_ARTLIB_SeqncrMax2DSequences;
#endif

  memset (&Runtime3DArray, 0, sizeof (Runtime3DArray));
  NextFree3DRuntimeElement = Runtime3DArray + 0;
  for (i = 0; i < CE_ARTLIB_SeqncrMax3DSequences - 1; i++)
    *((LE_SEQNCR_Runtime3DInfoPointer *) (Runtime3DArray + i)) = Runtime3DArray + i + 1;
#if CE_ARTLIB_EnableDebugMode
  NumberOfFree3DRuntimeElements = CE_ARTLIB_SeqncrMax3DSequences;
#endif

  // Set up the command processing lists.

  WaitingChainedCommandsHead = NULL;
  WaitingChainedCommandsTail = NULL;
  ExecutableChainedCommandsHead = NULL;
  ExecutableChainedCommandsTail = NULL;
  CollectCommandsNestingLevel = 0;
  SuspendEngine = FALSE;

  memset (&ChainedItemsArray, 0, sizeof (ChainedItemsArray));
  FreeChainedCommands = ChainedItemsArray + 0;
  for (i = 0; i < CE_ARTLIB_SeqncrMaxChainedThings - 1; i++)
    ChainedItemsArray[i].next = ChainedItemsArray + (i + 1);

  // Initialise the watch system to not be watching anything.  Otherwise
  // if you have a sequence with DataID zero, it may start being watched.

  memset ((void *) &LE_SEQNCR_WatchArray, 0, sizeof (LE_SEQNCR_WatchArray));
  for (i = 0; i < CE_ARTLIB_SeqncrMaxWatches; i++)
  {
    LE_SEQNCR_WatchArray[i].dataID = LE_DATA_EmptyItem;
    LE_SEQNCR_WatchArray[i].priority = 12345;
  }

  // Initialise the special root node.  Use a separate local variable
  // so that the other threads don't try to use a partially initialised
  // sequence tree.

  SequencePntr = LI_SEQNCR_AllocRuntimeInfo ();
  SequencePntr->endTime = INFINITE_END_TIME; // Ends after half a year.
  SequencePntr->renderSlotSet = (LE_REND_RenderSlotSet) -1; // All slots.
  SequencePntr->timeMultiple = 1; // Overall engine clock update rate.
  SequencePntr->endingAction = LE_SEQNCR_EndingActionLoopToBeginning;
  SequencePntr->onScreen = (LE_REND_RenderSlotSet) -1; // Visible everywhere.
  SequencePntr->sequenceType = LE_CHUNK_ID_SEQ_GROUPING;
  SequencePntr->dropFrames = TRUE; // Set this to FALSE for non-real-time playback.
  SequencePntr->watchIndex = -1;
  LI_SEQNCR_CommonRuntimeTreeRoot = SequencePntr;
  SuspendEngine = FALSE;
}



/*****************************************************************************
 * Deallocate stuff used by the sequence playing module and make it inactive.
 */

void LI_SEQNCR_RemoveSystem (void)
{
  LE_QUEUE_MessageRecord  UserCommand;
  ChainedItemPointer      CurrentPntr;
#if CE_ARTLIB_EnableMultitasking
  UNS16                   StartingCycleCount;
#endif

  LE_TIMERS_Suspend (FALSE); // Need to unsuspend the timers first.

  if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
  {
#if CE_ARTLIB_EnableSystemMouse
    LE_SEQNCR_RemoveMouseGroupingSequence (); // Kill the mouse (deallocates space).
#endif // CE_ARTLIB_EnableSystemMouse

    // End nesting of commands, forcing the execution to start,
    // which will execute our mouse shutdown commands.

    CollectCommandsNestingLevel = 0;
    LE_SEQNCR_SuspendEngine (FALSE); // So it actually does it.

    // Deallocate all the sequences (except root).

    EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);

#if CE_ARTLIB_EnableDebugMode /* One last dump */
    LE_ERROR_DebugMsg ("Dump of sequences before LI_SEQNCR_RemoveSystem cleans up:\r\n",
      LE_ERROR_DebugMsgToFile);
    LE_SEQNCR_DumpActiveSequencesToDebugFile (FALSE);
#endif

    while (LI_SEQNCR_CommonRuntimeTreeRoot->child != NULL)
      DeleteSequenceAndChildren (LI_SEQNCR_CommonRuntimeTreeRoot->child);

    // Stop the sequencer thread activity.  There is some awkwardness here
    // for getting out of critical sections before the critical section
    // object is deallocated.

    LI_SEQNCR_CommonRuntimeTreeRoot = NULL;
    LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#if CE_ARTLIB_EnableMultitasking
    StartingCycleCount = (UNS16) (EndOfUpdateCycleCounter);
    while ((UNS16) (EndOfUpdateCycleCounter - StartingCycleCount) < 30)
      LE_TIMER_Delay (1);
#endif

    // Cancel all pending commands to free related storage.  There shouldn't
    // be too many since we tried to turn on execution.

    while (LE_QUEUE_Dequeue (&SequencerCommandQueue, &UserCommand))
    {
      CancelCommand (&UserCommand);
      LE_QUEUE_FreeStorageAssociatedWithMessage (&UserCommand);
    }

    // Delete pending chain commands too.

    while (WaitingChainedCommandsTail != NULL)
    {
      CurrentPntr = WaitingChainedCommandsTail;
      WaitingChainedCommandsTail = CurrentPntr->previous;
      CancelCommand (&CurrentPntr->message);
      LE_QUEUE_FreeStorageAssociatedWithMessage (&CurrentPntr->message);
      memset (CurrentPntr, 0, sizeof (ChainedItemRecord));
      CurrentPntr->next = FreeChainedCommands;
      FreeChainedCommands = CurrentPntr;
    }
    WaitingChainedCommandsTail = WaitingChainedCommandsHead = NULL;

    // And unexecuted commands that were recently dechained.

    while (ExecutableChainedCommandsTail != 0)
    {
      CurrentPntr = ExecutableChainedCommandsTail;
      ExecutableChainedCommandsTail = CurrentPntr->previous;
      CancelCommand (&CurrentPntr->message);
      LE_QUEUE_FreeStorageAssociatedWithMessage (&CurrentPntr->message);
      memset (CurrentPntr, 0, sizeof (ChainedItemRecord));
      CurrentPntr->next = FreeChainedCommands;
      FreeChainedCommands = CurrentPntr;
    }
    ExecutableChainedCommandsTail = ExecutableChainedCommandsHead = NULL;

    // Miscellaneous removes.

    DeleteCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
    DeleteCriticalSection (&PreLoaderCriticalSection);
    DeleteCriticalSection (&PostLoaderCriticalSection);
    LE_QUEUE_Cleanup (&SequencerCommandQueue);

    AVIFileExit ();
  }
}



/****************************************************************************/
/* S U P P O R T   F O R   R E N D E R   S L O T S                          */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
 * Base constructor for render slots.  Just initialises data to the unused
 * slot situation.  You have to install the slot to activate it.
 */

LI_REND_AbstractRenderSlotClass::LI_REND_AbstractRenderSlotClass ()
{
  mySlotNumber = CE_ARTLIB_RendMaxRenderSlots;
  mySlotSetMask = 0;
}



/*****************************************************************************
 * Default destructor for render slots.
 */

LI_REND_AbstractRenderSlotClass::~LI_REND_AbstractRenderSlotClass ()
{
}



/*****************************************************************************
 * Stubs for the virtual functions - these implementations do nothing,
 * override ones you need to use in your actual renderer class.
 */

BOOL LI_REND_AbstractRenderSlotClass::SequenceStartUp (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  return FALSE;
}

void LI_REND_AbstractRenderSlotClass::SequenceShutDown (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
}

BOOL LI_REND_AbstractRenderSlotClass::SequenceMoved (
LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  return FALSE;
}

BOOL LI_REND_AbstractRenderSlotClass::CanHandleSequenceType (
LE_SEQNCR_SequenceType SequenceType, int Dimensionality)
{
  return FALSE;
}

void LI_REND_AbstractRenderSlotClass::UpdateInvalidAreas (
LE_SEQNCR_RuntimeInfoPointer RootPntr)
{
}

void LI_REND_AbstractRenderSlotClass::DrawInvalidAreas (
LE_SEQNCR_RuntimeInfoPointer RootPntr)
{
}

void LI_REND_AbstractRenderSlotClass::AddInvalidArea (
HRGN InvalidAreaRegion, PDDSURFACE ScreenSurface)
{
}

HRGN LI_REND_AbstractRenderSlotClass::GetSolidArea (
LE_SEQNCR_RuntimeInfoPointer RootPntr, PDDSURFACE ScreenSurface)
{
  return NULL;
}

BOOL LI_REND_AbstractRenderSlotClass::SequenceTouchesScreenRectangle (
LE_SEQNCR_RuntimeInfoPointer SequencePntr,
TYPE_RectPointer TouchingRectanglePntr)
{
  return FALSE;
}

BOOL LI_REND_AbstractRenderSlotClass::GetScreenBoundingRectangle (
LE_SEQNCR_RuntimeInfoPointer SequencePntr, TYPE_RectPointer ScreenRectPntr)
{
  return FALSE;
}

BOOL LI_REND_AbstractRenderSlotClass::DragOnCameraPlane (
LE_SEQNCR_RuntimeInfoPointer SequencePntr,
TYPE_Point2DPointer DragFromVirtualScreenPointPntr,
TYPE_Point2DPointer DragToVirtualScreenPointPntr,
TYPE_Point3DPointer ResultingDeltaVector)
{
  return FALSE;
}

UNS8 LI_REND_AbstractRenderSlotClass::GetCameraLabelNumber (void)
{
  return 0;
}

PDDSURFACE LI_REND_AbstractRenderSlotClass::GetWorkingSurface (void)
{
  return NULL;
}

BOOL LI_REND_AbstractRenderSlotClass::SetViewport (const TYPE_Rect *ViewportRectPntr)
{
  return FALSE;
}

BOOL LI_REND_AbstractRenderSlotClass::SetCamera (UNS8 CameraNumber,
TYPE_Point3DPointer Position, TYPE_Point3DPointer Forwards,
TYPE_Point3DPointer Up, TYPE_Angle3D FovAngle,
TYPE_Coord3D NearZ, TYPE_Coord3D FarZ)
{
  return FALSE;
}



/*****************************************************************************
 * Install the generic rendering module into a particular render slot.
 * Returns TRUE if successful.
 */

BOOL LI_REND_AbstractRenderSlotClass::InstallInSlot (LE_REND_RenderSlot RenderSlot)
{
  if (RenderSlot >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_REND_AbstractRenderSlotClass::InstallInSlot: "
      "Slot %d out of range.\r\n", (int) RenderSlot);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  if (LI_SEQNCR_RenderSlotArray [RenderSlot] != NULL)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_REND_AbstractRenderSlotClass::InstallInSlot: "
      "Slot %d is already in use.\r\n", (int) RenderSlot);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // Enable the sequencer callbacks for our slot.  But do it while the
  // sequencer is between cycles otherwise it will have problems with
  // partial updating.

#if CE_ARTLIB_EnableMultitasking
  if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
    EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#endif

  mySlotNumber = RenderSlot;
  mySlotSetMask = (LE_REND_RenderSlotSet) (1 << RenderSlot);
  LI_SEQNCR_RenderSlotArray [RenderSlot] = this;

#if CE_ARTLIB_EnableMultitasking
  if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
    LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#endif

  return TRUE;
}



/*****************************************************************************
 * Remove the render slot from the sequencer's active slots collection.
 */

BOOL LI_REND_AbstractRenderSlotClass::Uninstall (void)
{
  BOOL  ReturnCode = FALSE;

#if CE_ARTLIB_EnableMultitasking
  if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
    EnterCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#endif

  if (mySlotSetMask == 0 && mySlotNumber == CE_ARTLIB_RendMaxRenderSlots)
    goto ErrorExit; // Already uninstalled.

  if (mySlotNumber >= CE_ARTLIB_RendMaxRenderSlots)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_REND_AbstractRenderSlotClass::Uninstall: "
      "Slot %d is invalid / uninitialised.\r\n", (int) mySlotNumber);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  if (LI_SEQNCR_RenderSlotArray [mySlotNumber] != this)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_REND_AbstractRenderSlotClass::Uninstall: "
      "Nonfatal warning - slot %d is assigned to some other renderer.\r\n",
      (int) mySlotNumber);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
  }
  else
  {
    LI_SEQNCR_RemoveSequencesInRenderSlotSet (mySlotSetMask);
    LI_SEQNCR_RenderSlotArray [mySlotNumber] = NULL;
  }

  mySlotNumber = CE_ARTLIB_RendMaxRenderSlots;
  mySlotSetMask = 0;
  ReturnCode = TRUE;

ErrorExit:
#if CE_ARTLIB_EnableMultitasking
  if (LI_SEQNCR_CommonRuntimeTreeRoot != NULL)
    LeaveCriticalSection (&LI_SEQNCR_UpdateCycleCriticalSection);
#endif
  return ReturnCode;
}



/*****************************************************************************
 * User callable function for removing a render slot.
 */

void LE_SEQNCR_UninstallRenderSlot (LE_REND_RenderSlot RenderSlot)
{
  LI_REND_AbstractRenderSlotPointer RenderSlotPntr;

  if (RenderSlot < CE_ARTLIB_RendMaxRenderSlots)
  {
    RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];

    if (RenderSlotPntr != NULL)
      RenderSlotPntr->Uninstall ();
  }
}



/*****************************************************************************
 * User callable function for getting the camera currently used by a slot,
 * returns zero if none or an error.
 */

UNS8 LE_SEQNCR_GetRenderSlotCamera (LE_REND_RenderSlot RenderSlot)
{
  LI_REND_AbstractRenderSlotPointer RenderSlotPntr;

  if (RenderSlot < CE_ARTLIB_RendMaxRenderSlots)
  {
    RenderSlotPntr = LI_SEQNCR_RenderSlotArray [RenderSlot];

    if (RenderSlotPntr != NULL)
      return RenderSlotPntr->GetCameraLabelNumber ();
  }

  return 0;
}



#endif /* CE_ARTLIB_EnableSystemSequencer */
