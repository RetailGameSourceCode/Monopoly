/*****************************************************************************
 *
 * FILE:        L_Rend0D.c
 *
 * DESCRIPTION: Renders zero dimensional stuff, usually sound effects.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Rend0D.cpp 10    4/27/99 1:32p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Rend0D.cpp $
 * 
 * 10    4/27/99 1:32p Agmsmith
 * Number of render slots is now user defined.
 *
 * 9     3/28/99 12:11p Agmsmith
 * Also set pitch when a sound starts up.
 *
 * 8     3/27/99 12:49p Agmsmith
 * Don't update sequenceClock while feeding.
 *
 * 7     3/21/99 3:29p Agmsmith
 * Changes to allow all sequences to have child sequences.
 *
 * 6     3/13/99 2:29p Agmsmith
 * Added iteration over all active sequences and a way of restricting that
 * to ones under a given mouse position.
 *
 * 5     1/31/99 3:25p Agmsmith
 * Rearranged video player code so that feeding was done during
 * renderering, not in the sequencer, so that it can draw directly on the
 * screen.
 *
 * 4     1/05/99 5:00p Agmsmith
 * Renderer / sequencer interface converted from a bunch of arrays of
 * pointers to functions into C++ virtual function calls and an abstract
 * base class.  Now it should be easier to add new functionality.
 ****************************************************************************/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemRend0D


typedef class LI_REND0D_RenderSlotClass :
  public LI_REND_AbstractRenderSlotClass
{
  public:
  virtual BOOL SequenceStartUp (LE_SEQNCR_RuntimeInfoPointer SequencePntr);
  virtual void SequenceShutDown (LE_SEQNCR_RuntimeInfoPointer SequencePntr);
  virtual BOOL CanHandleSequenceType (LE_SEQNCR_SequenceType SequenceType, int Dimensionality);
  virtual void DrawInvalidAreas (LE_SEQNCR_RuntimeInfoPointer RootPntr);
} *LI_REND0D_RenderSlotPointer;



/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

static LI_REND0D_RenderSlotClass
  LI_REND0D_RenderSlotStorageSpace [CE_ARTLIB_RendMaxRenderSlots];

// Statically allocated space for our render slot data.



/*****************************************************************************
 *****************************************************************************
 ** M O S T L Y   S E Q U E N C E R   C A L L B A C K   F U N C T I O N S   **
 *****************************************************************************
 ****************************************************************************/

/*****************************************************************************
 * Called by the sequencer when it starts up a sequence in our render slot.
 * We just validate it and if it is a sound, start playing the sound.
 */

BOOL LI_REND0D_RenderSlotClass::SequenceStartUp (LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  INT8  Panning;
  UNS32 Pitch;
  UNS8  Volume;

  // Some safety checks.  Render slot valid?

  if (mySlotSetMask == 0)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND0D_RenderSlotClass::SequenceStartUp not initalised.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // We allow all grouping chunks, no matter what dimensionality.

  if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_GROUPING ||
  SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_INDIRECT)
    return TRUE; // A grouping sequence is OK.  Don't do anything.

  // Dimensionality zero?

  if (SequencePntr->dimensionality != 0)
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND0D_RenderSlotClass::SequenceStartUp: Bad dimensionality.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // Is it a sound?

  if (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND)
  {
#if CE_ARTLIB_EnableSystemSound
    // Adjust the sound panning and volume.

    LE_SOUND_BufSndHandle BufSndPntr;

    if (SequencePntr->streamSpecificData.audioStream == NULL ||
    (BufSndPntr = SequencePntr->streamSpecificData.audioStream->bufSnd) == NULL)
    {
#if CE_ARTLIB_EnableDebugMode
      LE_ERROR_DebugMsg ("LI_REND0D_RenderSlotClass::SequenceStartUp: "
      "Sound stream not allocated or maybe not initialised!\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
      return FALSE;
    }

    // Get volume out of the record, 100 is full, 0 is quiet.
    Volume = SequencePntr->aux.audio.volume;
    if (Volume > 100) Volume = 100;

    if (Volume < 100)
      LE_SOUND_SetVolumeBufSnd (BufSndPntr, Volume);

    // Get the panning out of the record, 0 is center, -100 is left, +100 is right.
    Panning = SequencePntr->aux.audio.panning;
    if (Panning < -100) Panning = -100;
    if (Panning > 100) Panning = 100;

    if (Panning != 0)
      LE_SOUND_SetPanBufSnd (BufSndPntr, Panning);

    // Get the pitch setting and use it if it isn't the default of zero.
    Pitch = SequencePntr->aux.audio.pitch;
    if (Pitch != 0)
      LE_SOUND_SetPitchBufSnd (BufSndPntr, Pitch);

#else // Sound system isn't compiled.
    return FALSE;
#endif
  }
  else // Not a sequence we know about or should be given.
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_REND0D_RenderSlotClass::SequenceStartUp: Unknown sequence type.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE; // Unknown kind of sequence.
  }

  return TRUE;
}



/*****************************************************************************
 * A sequence has finished and is being deleted.
 */

void LI_REND0D_RenderSlotClass::SequenceShutDown (LE_SEQNCR_RuntimeInfoPointer SequencePntr)
{
  if (SequencePntr->sequenceType != LE_CHUNK_ID_SEQ_SOUND)
    return; // Nothing to do for groups and unknowns.
}



/*****************************************************************************
 * Tests to see if we can handle the given sequence.  It either has to be
 * a grouping sequence or has to be a 0D sound.
 */

BOOL LI_REND0D_RenderSlotClass::CanHandleSequenceType (LE_SEQNCR_SequenceType SequenceType,
int Dimensionality)
{
  if (SequenceType == LE_CHUNK_ID_SEQ_GROUPING ||
  SequenceType == LE_CHUNK_ID_SEQ_INDIRECT)
    return TRUE;

  if (Dimensionality != 0 || SequenceType != LE_CHUNK_ID_SEQ_SOUND)
    return FALSE;

  // A zero dimensional sound.

  return TRUE;
}



/*****************************************************************************
 * Not much to draw in zero dimensions, but we can feed the sound for
 * spooling big sounds.
 */

void LI_REND0D_RenderSlotClass::DrawInvalidAreas (
LE_SEQNCR_RuntimeInfoPointer RootPntr)
{
  LE_SOUND_AudioStreamPointer   AudioPntr;
  BOOL                          InOurSlot;
  LE_REND_RenderSlotSet         RenderMask;
  LE_SEQNCR_RuntimeInfoPointer  SequencePntr;

  RenderMask = mySlotSetMask;
  SequencePntr = RootPntr;
  while (TRUE)
  {
    // Process the current node.

    InOurSlot = ((SequencePntr->renderSlotSet & RenderMask) != 0);

    if (InOurSlot &&
    (SequencePntr->sequenceType == LE_CHUNK_ID_SEQ_SOUND))
    {
      AudioPntr = SequencePntr->streamSpecificData.audioStream;

      if (!LE_SOUND_FeedAudioStream (AudioPntr,
      SequencePntr->endingAction == LE_SEQNCR_EndingActionLoopToBeginning))
      {
        // Error happened while feeding, kill it.
        SequencePntr->endingAction = LE_SEQNCR_EndingActionSuicide;
      }
    }

    // Traverse down the tree, then do siblings, then back up to the
    // next parent's sibling.  Ignore nodes not in our slot since
    // their children will definitely not be in our slot.

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
 *****************************************************************************
 ** I N I T I A L I S A T I O N   &   S H U T D O W N   F U N C T I O N S   **
 *****************************************************************************
 ****************************************************************************/

/*****************************************************************************
 * Install the 0D rendering module into a particular render slot.  Returns
 * TRUE if successful.
 */

BOOL LE_REND0D_InstallInRenderSlot (
  LE_REND_RenderSlot RenderSlot)
{
  return LI_REND0D_RenderSlotStorageSpace[RenderSlot].
    InstallInSlot (RenderSlot);
}



/*****************************************************************************
 * Initialise the 0D rendering module.  Halts on failure.
 */

void LI_REND0D_InitSystem (void)
{
}



/*****************************************************************************
 * Deallocate stuff used by the 0D rendering module and make it inactive.
 */

void LI_REND0D_RemoveSystem (void)
{
  LE_REND_RenderSlot RenderSlot;

  for (RenderSlot = 0; RenderSlot < CE_ARTLIB_RendMaxRenderSlots; RenderSlot++)
    LI_REND0D_RenderSlotStorageSpace[RenderSlot].Uninstall ();
}

#endif /* CE_ARTLIB_EnableSystemRend0D */
