/*****************************************************************************
 * Voice Chat Subroutines by Artech Digital Entertainments, copyright (c) 1997.
 *
 * Tab stops are every 2 spaces, also save the file with spaces instead of tabs.
 *
 * $Header: /Projects - 1999/Mono 2 Hotseat/L_voice.cpp 7     99/08/27 10:27a Agmsmith $
 *
 * $Log: /Projects - 1999/Mono 2 Hotseat/L_voice.cpp $
 * 
 * 7     99/08/27 10:27a Agmsmith
 * Get Hotseat compiling again.
 * 
 * 6     8/12/99 9:04 Davew
 * Removed the memory manager stuff and reverted back to the old
 * GlobalAlloc() functions
 * 
 * 5     8/09/99 13:35 Davew
 * Changed the memory allocation stuff to use the memory manager in PC3D
 * so we can track the memory usage and leaks.
 * 
 * 4     6/11/99 10:16a Russellk
 * 
 * 3     6/11/99 9:40a Russellk
 * Changed to 6 colors, 11 tokens.
 * 
 * 2     5/26/99 11:29 Davew
 * Lots of reformatting
 * 
 * 1     1/08/99 2:06p Russellk
 * 
 * 1     1/08/99 11:36a Russellk
 * Graphics
 * 
 */

#include "gameinc.h"
#include <mmreg.h>     //    These 2 headers must be in the order seen here!
#include <msacm.h>     //    ***********************************************

#if FORHOTSEAT
// This mess is Daves fault, as it global warming.
#include <dsound.h>     /* DirectSound audio stuff */
#define RELEASEPOINTER(p)         if (p) ((p)->Release(), (p) = NULL)

// ===========================================================================
// Abstract DirectSound types from L_DXTypes.h, which we can't use since it
// has 3D stuff in it which doesn't link in the Hotseat.
// ===========================================================================

typedef struct IDirectSound             DSOUND1;
typedef struct IDirectSound             DSOUND;
typedef LPDIRECTSOUND                   PDSOUND1;
typedef LPDIRECTSOUND                   PDSOUND;

typedef struct IDirectSoundBuffer       DSBUFFER1;
typedef struct IDirectSoundBuffer       DSBUFFER;
typedef LPDIRECTSOUNDBUFFER             PDSBUFFER1;
typedef LPDIRECTSOUNDBUFFER             PDSBUFFER;

typedef struct IDirectSound3DListener   DSLISTENER1;
typedef struct IDirectSound3DListener   DSLISTENER;
typedef LPDIRECTSOUND3DLISTENER         PDS3DLISTENER1;
typedef LPDIRECTSOUND3DLISTENER         PDS3DLISTENER;

typedef struct _DS3DLISTENER            DS3DLISTENERDATA1;
typedef struct _DS3DLISTENER            DS3DLISTENERDATA;
typedef LPDS3DLISTENER                  PDS3DLISTENERDATA1;
typedef LPDS3DLISTENER                  PDS3DLISTENERDATA;

typedef struct IDirectSound3DBuffer     DS3DBUF1;
typedef struct IDirectSound3DBuffer     DS3DBUF;
typedef LPDIRECTSOUND3DBUFFER           PDS3DBUFFER1;
typedef LPDIRECTSOUND3DBUFFER           PDS3DBUFFER;

typedef struct _DS3DBUFFER              DS3DBUFFERDATA1;
typedef struct _DS3DBUFFER              DS3DBUFFERDATA;
typedef LPDS3DBUFFER                    PDS3DBUFFERDATA1;
typedef LPDS3DBUFFER                    PDS3DBUFFERDATA;

typedef struct IDirectSoundCapture      DSCAPTURE1;
typedef struct IDirectSoundCapture      DSCAPTURE;
typedef LPDIRECTSOUNDCAPTURE            PDSCAPTURE1;
typedef LPDIRECTSOUNDCAPTURE            PDSCAPTURE;

typedef struct IDirectSoundCaptureBuffer  DSCAPTUREBUFFER1;
typedef struct IDirectSoundCaptureBuffer  DSCAPTUREBUFFER;
typedef LPDIRECTSOUNDCAPTUREBUFFER        PDSCAPTUREBUFFER1;
typedef LPDIRECTSOUNDCAPTUREBUFFER        PDSCAPTUREBUFFER;

typedef struct IDirectSoundNotify       DSNOTIFY1;
typedef struct IDirectSoundNotify       DSNOTIFY;
typedef LPDIRECTSOUNDNOTIFY             PDSNOTIFY1;
typedef LPDIRECTSOUNDNOTIFY             PDSNOTIFY;
#endif


// *NOTE* Remember to include "MSACM32.LIB" in Build/Settings/Link/Object/libray modules.
// for the Audio Compression Manager

static PDSOUND g_pDSoundObject;
  /* Our main Direct Sound object, NULL if not open. */

static PDSCAPTURE g_pDSCaptureObject;
  /* The Direct Sound capture buffer factory object.  NULL if not open. */


#ifndef RULE_AllocateHandle
  typedef HGLOBAL                       RULE_CharHandle;
  #define RULE_AllocateHandle(x)        GlobalAlloc(GHND, x)
  #define RULE_FreeHandle(x)            GlobalFree(x)
  #define RULE_HandleSize(x)            GlobalSize(x)
  #define RULE_UnlockHandle(x)          GlobalUnlock(x)
  #define RULE_LockHandle(x)            GlobalLock(x)
  #define RULE_PointerToHandle(x)       GlobalHandle(x)
  #define RULE_ResizeHandle(x, NewSize) GlobalReAlloc(x, NewSize, GMEM_MOVEABLE | GMEM_ZEROINIT)
#endif


/* This file inherits the static variables g_pDSoundObject and
   g_pDSCaptureObject from the file that includes this one.
   They are presumed to be open if they are not NULL. */


/*****************************************************************************
 * EXPORTED GLOBAL VARIABLES
 */

BOOL  LE_VOICE_PLAYBACK_AVAILABLE;
BOOL  LE_VOICE_CAPTURE_AVAILABLE;
  /* These describe the sound system availability to the rest of the world. */

unsigned char LE_VOICE_NUM_COMPRESSORS;
  /* Number of sound compression techniques available, usually at least
  one since it includes the no-compression method.  Methods are numbered from
  zero to this value minus one. */

unsigned char LE_VOICE_DESIRED_COMPRESSOR;
  /* This is the compressor that will be used by the voice compression
  routines.  It defaults to no-compression. */

char *LE_VOICE_COMPRESSOR_NAMES [LE_VOICE_MAX_COMPRESSORS];
  /* An array with readable string names for the available voice
  compressors.  Some of the strings are dynamically allocated. */

long LE_VOICE_SAMPLES_PER_SEC = 11025;
  /* Desired recording rate in samples (not bytes) per second.  Cheap
  SoundBlaster clones only support 22050hz at 8 or 16 bits.  We can
  cut it in half during recording to get 11025.  This global will be
  changed to the actual effective value used when recording starts. */

short LE_VOICE_BITS_PER_SAMPLE = 8;
  /* Size of the samples.  More bits mean that it can record greater ranges in
  volume (quiet still sounds good with lots of bits).  This will be updated
  when recording starts to show the value actually used. */

short LE_VOICE_NUMBER_OF_CHANNELS = 1;
  /* Number of channels.  Either 1 for mMonophonic, 2 for stereophonic. */



// ===========================================================================
// LOCAL STATIC VARIABLES AND MACROS
// ===========================================================================

static BOOL LI_VOICE_ACM_AVAILABLE;

// Identifies the various compressors.  Zero sample rate means it doesn't
//  matter.  All have at least the wFormatTag specified.  The compression wave
//  format records are allocated at maximum size (ACM_METRIC_MAX_SIZE_FORMAT,
//  about 50 bytes) and most have the extra private data at the end that means
//  something (and gets used when you open the stream to pick between different
//  bandwidth settings at the same sampling rates).  NULL if not allocated.
static LPWAVEFORMATEX CompressorIDs [LE_VOICE_MAX_COMPRESSORS];

// The canned one we use for PCM wave format
static WAVEFORMATEX NoCompressionWaveFormatID;

// Can play this many simultaneous voice chat messages.  Any more would
//  really be useless - you wouldn't be able to understand what they are
//  saying anyways.  Even just two are hard to understand.
#define MAX_VOICE_SIMULTANEOUS_PLAY 6

typedef struct tAACONVERTDESC
{
  HACMSTREAM        has;

  HMMIO             hmmioSrc;
  HMMIO             hmmioDst;

  MMCKINFO          ckSrc;
  MMCKINFO          ckSrcRIFF;

  MMCKINFO          ckDst;
  MMCKINFO          ckDstRIFF;

  MMIOINFO          mmioInfoSrc;
  MMIOINFO          mmioInfoDst;

  // Settings for compressor input, after optional frequency halving & 
  //  mono conversion.
  LPWAVEFORMATEX    pwfxSrc;
  LPBYTE            pbSrc;
  DWORD             cbSrcLeftOvers;       // Amount in pbSrc left from last time
  DWORD             cbSrcReadSize;        // Size of pbSrc, one second of sound
  BOOL              bHalveSamplingRate;   // TRUE if we have to fake a lower sampling rate
  BOOL              bConvertStereoToMono; // TRUE if we have to fake monophonic recording

  LPWAVEFORMATEX    pwfxDst;
  LPBYTE            pbDst;
  DWORD             cbDstBufSize;
  HGLOBAL           destMemoryHandle;
  LPBYTE            destMemoryPntr;
  ACMSTREAMHEADER   ash;
  BYTE              destinationCompressor;
  // From 0 to LE_VOICE_NUM_COMPRESSORS-1.  pwfxDst derived from this.

  DWORD             dwNextReadOffset;
  DWORD             dwProgress;

} AACONVERTDESC, *PAACONVERTDESC;


// Points to a DirectSound buffer if it is in use, NULL when free.
//  Finished sounds get cleaned up when the next chat comes in or when
//  the system is shut down.
static PDSBUFFER VoiceBuffers[MAX_VOICE_SIMULTANEOUS_PLAY];


// Our single Direct Sound recording buffer, NULL if it doesn't exist.
static PDSCAPTUREBUFFER RecordingBuffer;


// "Files" for output and input compression streams
static PAACONVERTDESC paacdPlay;
static PAACONVERTDESC paacdRecord;

static DWORD SavedMaxRecordingSize;



/*****************************************************************************
 * Empty out the list of compressors, freeing dynamically allocated storage
 * used for the names etc.
 */

static void ClearCompressorIDList()
{
  for (int i = 1; i < LE_VOICE_NUM_COMPRESSORS; i++)
  {
    if (LE_VOICE_COMPRESSOR_NAMES[i] != NULL)
    {
      GlobalFree (LE_VOICE_COMPRESSOR_NAMES[i]);
      LE_VOICE_COMPRESSOR_NAMES[i] = NULL;
    }

    if (CompressorIDs[i] != NULL)
    {
      GlobalFree(CompressorIDs[i]);
      CompressorIDs[i] = NULL;
    }
  }

  LE_VOICE_NUM_COMPRESSORS    = 0;
  LE_VOICE_DESIRED_COMPRESSOR = 0;
  LI_VOICE_ACM_AVAILABLE      = FALSE;
}



/*****************************************************************************
 * Utility routine for stopping the sound recording.
 */

static void LI_VOICE_FREE_RECORD()
{
  if (RecordingBuffer)
  {
    RecordingBuffer->Stop();
    RecordingBuffer->Release();
    RecordingBuffer = NULL;
  }
}



/*****************************************************************************
 * Deallocate memory just for sounds which have finished playing (StopAll is
 * FALSE), or for all sounds including recorded stuff (StopAll is TRUE).
 */

static void LI_VOICE_FREE_MEMORY(BOOL bStopAll)
{
  HRESULT             hErrorCode  = NULL;
  DWORD               dwStatus    = 0;

#if !FORHOTSEAT // Multithreaded - need mutual exclusion.
//  WaitForSingleObject (hSoundTableMutex, INFINITE);
#endif

  for (int i = 0; i < MAX_VOICE_SIMULTANEOUS_PLAY; i++)
  {
    if (VoiceBuffers[i])
    {
      dwStatus    = 0;
      hErrorCode  = VoiceBuffers[i]->GetStatus(&dwStatus);

      if (bStopAll || FAILED(hErrorCode) || !(dwStatus & DSBSTATUS_PLAYING))
      {
        // Stop the chatter and free the DirectSound buffer
        if (dwStatus & DSBSTATUS_PLAYING)
          VoiceBuffers[i]->Stop();

        VoiceBuffers[i]->Release();
        VoiceBuffers[i] = NULL;
      }
    }
  }

  if (bStopAll)
    LI_VOICE_FREE_RECORD();

#if !FORHOTSEAT
//  ReleaseMutex (hSoundTableMutex);
#endif
}



/*****************************************************************************
 * Close ACM stuff and deallocate the associated buffers and streams.
 * Optionally leave the destination MMIO open (at the start of the "file") and
 * leave its memory allocated.
 */

static void DeallocateConverterDescription(PAACONVERTDESC ConverterPntr,
                                           BOOL LeaveDestinationMMIOOpen)
{
  MMRESULT          mmr;
  LPACMSTREAMHEADER pash;

  if (ConverterPntr == NULL) return;
  pash = &ConverterPntr->ash;

  // Stop conversion and close the ACM stream

  if (NULL != ConverterPntr->has)
  {
    if (ACMSTREAMHEADER_STATUSF_PREPARED & pash->fdwStatus)
    {
      mmr = acmStreamReset (ConverterPntr->has, 0);

      pash->cbSrcLength = ConverterPntr->cbSrcReadSize;
      pash->cbDstLength = ConverterPntr->cbDstBufSize;

      mmr = acmStreamUnprepareHeader (ConverterPntr->has, pash, 0L);
    }
    acmStreamClose(ConverterPntr->has, 0L);
    ConverterPntr->has = NULL;
  }

  // Close the source data MMIO stream

  if (NULL != ConverterPntr->hmmioSrc)
  {
    mmioClose (ConverterPntr->hmmioSrc, 0);
    ConverterPntr->hmmioSrc = NULL;
  }

  // Write out the chunk sizes (by ascending as needed) and optionally close
  //  the destination MMIO stream.  If you don't close it, the stream gets
  //  left at the start of the file and the memory doesn't get deallocated.

  if (NULL != ConverterPntr->hmmioDst)
  {
    if (LeaveDestinationMMIOOpen)
    {
      mmr = 0;

      // Close off the data chunk
      if (ConverterPntr->ckDst.ckid != 0)
      {
        mmr = mmioAscend(ConverterPntr->hmmioDst, &ConverterPntr->ckDst, 0);
        ConverterPntr->ckDst.ckid = 0;
      }

      // Close the RIFF chunk
      if (mmr == 0 && ConverterPntr->ckDstRIFF.ckid != 0)
      {
        mmr = mmioAscend(ConverterPntr->hmmioDst, &ConverterPntr->ckDstRIFF, 0);
        ConverterPntr->ckDstRIFF.ckid = 0;
      }

      // Seek back to the start of the file
      if (mmr == 0)
        mmr = mmioSeek(ConverterPntr->hmmioDst, 0, SEEK_SET);
    }
    else // Close it all off.  Don't care about contents.
    {
      mmr = mmioClose(ConverterPntr->hmmioDst, 0);
      ConverterPntr->hmmioDst = NULL;
    }
  }

  // Deallocate the compression buffers we allocated

  if (ConverterPntr->pbSrc != NULL)
  {
    GlobalFree(ConverterPntr->pbSrc);
    ConverterPntr->pbSrc = NULL;
  }

  if (ConverterPntr->pbDst != NULL)
  {
    GlobalFree(ConverterPntr->pbDst);
    ConverterPntr->pbDst = NULL;
  }

  // Deallocate the wave format header storage
  if (ConverterPntr->pwfxSrc != NULL)
  {
    GlobalFree(ConverterPntr->pwfxSrc);
    ConverterPntr->pwfxSrc = NULL;
  }

  if (ConverterPntr->pwfxDst != NULL)
  {
    GlobalFree(ConverterPntr->pwfxDst);
    ConverterPntr->pwfxDst = NULL;
  }

  // Deallocate our destination memory handle only if the
  //  destination RIFF file isn't using it.

  if (NULL == ConverterPntr->hmmioDst)
  {
    if (ConverterPntr->destMemoryPntr != NULL)
    {
      RULE_UnlockHandle(ConverterPntr->destMemoryHandle);
      ConverterPntr->destMemoryPntr = NULL;
    }

    if (ConverterPntr->destMemoryHandle != NULL)
    {
      RULE_FreeHandle(ConverterPntr->destMemoryHandle);
      ConverterPntr->destMemoryHandle = NULL;
    }
  }
}



/*****************************************************************************
 * Close the play compression stuff and deallocate it completely.
 */

static void ClearPlayConverterDescription()
{
  if (paacdPlay != NULL)
  {
    DeallocateConverterDescription(paacdPlay, FALSE);
    GlobalFree(paacdPlay);
    paacdPlay = NULL;
  }
}



/*****************************************************************************
 * Close the record compression stuff and deallocate it completely.
 */

static void ClearRecordConverterDescription()
{
  if (paacdRecord != NULL)
  {
    DeallocateConverterDescription(paacdRecord, FALSE);
    GlobalFree(paacdRecord);
    paacdRecord = NULL;
  }
}



/*****************************************************************************
 * Wipe out all the Audio Compression Manager allocations.
 */

static void ACM_cleanup (void)
{
  ClearPlayConverterDescription();
  ClearRecordConverterDescription();
  ClearCompressorIDList();
}



/*****************************************************************************
 * Open up drivers and set up a compression/decompression ACM stream.
 * For input this function expects a ptr to AACONVERTDESC struct with
 * the following members initialized...
 *
 * paacd->pwfxSrc               - Source wave format.
 * paacd->destinationCompressor - Destination format, indirectly.
 *
 * paacd->pwfxDst will be allocated and filled in with the destination format
 * that was used.  As well, various buffers and streams will be set up.
 *
 * Returns TRUE for success or FALSE for
 * failure (you have to deallocate in both cases).
 */

static BOOL PrepareToConvert (PAACONVERTDESC paacd)
{
  LPACMSTREAMHEADER pash;
  MMRESULT          mmr;
  DWORD             dwSize;
  DWORD             nAvgBytesPerSec;
  DWORD             nBlockAlign;

  if (paacd == NULL)
    return FALSE;
  pash = &paacd->ash;

  // Create our destination wave format description.  We just use the format
  //  that the system suggests when given the source format and destination
  //  tag.  Unfortunately, the suggested size sometimes doesn't work (MPEG)
  //  so we have to use the maximum size instead.  Find first the largest
  //  possible wave format record size.

  mmr = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &dwSize);
  if (mmr) goto FAILURE_EXIT;

  if (dwSize < sizeof(WAVEFORMATEX))
    dwSize = sizeof(WAVEFORMATEX);

#if _DEBUG
  if (paacd->pwfxDst != NULL)
  {
    LE_ERROR_ErrorMsg("L_VOICE PrepareToConvert: destination wave format "
                      "has been already allocated by someone!");
  }
#endif

  paacd->pwfxDst = (tWAVEFORMATEX*)GlobalAlloc(GPTR, dwSize);
  if (paacd->pwfxDst == NULL)
    goto FAILURE_EXIT;

  // Use the specified wave format for compressing / decompressing to.  If
  //  values are zero, let the acmFormatSuggest pick them.  If it is fully
  //  specified (including the optional private data past the end), then
  //  just use it as is.

  memcpy(paacd->pwfxDst, CompressorIDs[paacd->destinationCompressor], dwSize);

  if (paacd->pwfxDst->nSamplesPerSec == 0)
  {
    mmr = acmFormatSuggest(NULL, paacd->pwfxSrc, paacd->pwfxDst, dwSize,
                           ACM_FORMATSUGGESTF_WFORMATTAG |
                           ACM_FORMATSUGGESTF_NCHANNELS);
    if (mmr) goto FAILURE_EXIT;
  }

  // Ok, now open the stream!
  mmr = acmStreamOpen(&paacd->has, NULL, paacd->pwfxSrc, paacd->pwfxDst,
                      NULL, 0L, 0L, ACM_STREAMOPENF_NONREALTIME);

  if (mmr) goto FAILURE_EXIT;

  // Compute source bytes to read (round down to nearest block
  //   for one second of data).  Then find the corresponding
  //   recommended destination buffer size.

  nAvgBytesPerSec       = paacd->pwfxSrc->nAvgBytesPerSec;
  nBlockAlign           = paacd->pwfxSrc->nBlockAlign;
  paacd->cbSrcReadSize  = nAvgBytesPerSec - (nAvgBytesPerSec % nBlockAlign);

#if _DEBUG
  if (paacd->pbSrc != NULL || paacd->pbDst != NULL)
  {
    LE_ERROR_ErrorMsg("L_VOICE PrepareToConvert: work buffers "
                      "has been already allocated by someone!");
  }
#endif

  paacd->cbDstBufSize  = 0L;
  mmr = acmStreamSize(paacd->has, paacd->cbSrcReadSize, &paacd->cbDstBufSize,
                      ACM_STREAMSIZEF_SOURCE);
  if (mmr) goto FAILURE_EXIT;

  // Allocate the src and dst buffers for reading/converting data

  //paacd->pbSrc = (HPSTR) GlobalAlloc (GPTR, paacd->cbSrcReadSize);
  paacd->pbSrc = (BYTE*)GlobalAlloc(GPTR, paacd->cbSrcReadSize);
  if (NULL == paacd->pbSrc) goto FAILURE_EXIT;

  paacd->pbDst = (BYTE*)GlobalAlloc(GPTR, paacd->cbDstBufSize);
  if (NULL == paacd->pbDst) goto FAILURE_EXIT;

  // Tell the ACM about the buffers

  memset(pash, 0, sizeof(*pash));
  pash->cbStruct    = sizeof(*pash);
  pash->pbSrc       = paacd->pbSrc;
  pash->cbSrcLength = paacd->cbSrcReadSize;
  pash->dwSrcUser   = paacd->cbSrcReadSize;
  pash->pbDst       = paacd->pbDst;
  pash->cbDstLength = paacd->cbDstBufSize;
  pash->dwDstUser   = paacd->cbDstBufSize;

  mmr = acmStreamPrepareHeader(paacd->has, pash, 0L);
  if (mmr) goto FAILURE_EXIT;

  return TRUE;

FAILURE_EXIT: // The partial allocations get cleaned up later
  return FALSE;
}



/*****************************************************************************
 * Open the destination MMIO memory file for output and write the standard
 * chunks, leaving it positioned at the end of the data chunk.
 */

static BOOL PrepareDestRIFF(PAACONVERTDESC paacd)
{
  DWORD   dw;

  // Size of one virtual memory page is 4096 bytes under Windows 95
  #define VOICE_SAVE_ALLOC_INCREMENT  (4096 * 8)

  if (paacd->destMemoryHandle != NULL)
    return FALSE; // Something is wrong.  Already allocated?

  paacd->destMemoryHandle = RULE_AllocateHandle(VOICE_SAVE_ALLOC_INCREMENT);
  if (paacd->destMemoryHandle == NULL) return FALSE;

  paacd->destMemoryPntr = (BYTE*)RULE_LockHandle(paacd->destMemoryHandle);
  if(paacd->destMemoryPntr == NULL) return FALSE;

  paacd->mmioInfoDst.fccIOProc  = FOURCC_MEM;
  paacd->mmioInfoDst.pchBuffer  = (char*)paacd->destMemoryPntr;
  paacd->mmioInfoDst.cchBuffer  = VOICE_SAVE_ALLOC_INCREMENT; // Initial size
  paacd->mmioInfoDst.adwInfo[0] = VOICE_SAVE_ALLOC_INCREMENT; // Size increment
  paacd->hmmioDst = mmioOpen (NULL, &paacd->mmioInfoDst,
                              MMIO_READWRITE | MMIO_CREATE);
  if (paacd->hmmioDst == NULL) return FALSE;

  // Create the RIFF chunk of form type 'WAVE'

  paacd->ckDstRIFF.fccType = mmioFOURCC ('W', 'A', 'V', 'E');
  if (mmioCreateChunk(paacd->hmmioDst, &paacd->ckDstRIFF, MMIO_CREATERIFF))
    return FALSE;

  // Now create the destination fmt and data chunks _in that order_

  dw = (WAVE_FORMAT_PCM == paacd->pwfxDst->wFormatTag) ?
        sizeof(PCMWAVEFORMAT) : sizeof(WAVEFORMATEX) + paacd->pwfxDst->cbSize;

  paacd->ckDst.ckid   = mmioFOURCC('f', 'm', 't', ' ');
  paacd->ckDst.cksize = dw;

  if (mmioCreateChunk(paacd->hmmioDst, &paacd->ckDst, 0))
    return FALSE;

  if (mmioWrite(paacd->hmmioDst, (char*)paacd->pwfxDst, dw) != (LONG)dw)
    return FALSE;

  if (mmioAscend(paacd->hmmioDst, &paacd->ckDst, 0))
    return FALSE;

  // Create the data chunk and leave it positioned ready to write data
  paacd->ckDst.ckid = mmioFOURCC('d', 'a', 't', 'a');
  paacd->ckDst.cksize = 0L; // Don't know size in advance
  if (mmioCreateChunk(paacd->hmmioDst, &paacd->ckDst, 0))
    return FALSE;

  return TRUE;
}



/*****************************************************************************
 * Converts an entire file at once.
 */

static BOOL AcmConvertEntireFile(PAACONVERTDESC paacd)
{
  DWORD               BusyCount     = 0;
  DWORD               cbRead        = 0;
  DWORD               ConvertFlags  = ACM_STREAMCONVERTF_BLOCKALIGN;
  DWORD               dw            = 0;
  DWORD               LeftOvers     = 0;
  MMRESULT            mmr           = NULL;
  BOOL                ReturnCode    = FALSE;
  LPACMSTREAMHEADER   pash          = &paacd->ash;

  while (ConvertFlags != 0)
  {
    // Fill up the rest of the buffer with data from the MMIO file
    dw = mmioRead(paacd->hmmioSrc, (char*)paacd->pbSrc + LeftOvers,
                  paacd->cbSrcReadSize - LeftOvers);

    cbRead = dw + LeftOvers;
    pash->cbSrcLength = cbRead;

    pash->cbDstLengthUsed = 0L;

    // Compress it
    mmr = acmStreamConvert(paacd->has, pash, ConvertFlags);
    if (mmr) goto FAILURE_EXIT;
    while (0 == (ACMSTREAMHEADER_STATUSF_DONE & ((AACONVERTDESC volatile*)paacd)->ash.fdwStatus))
      BusyCount++; // Busy wait for it to finish, probably won't happen since we use synchronous

    dw = pash->cbDstLengthUsed;
    if (dw != 0)
    {
      if (mmioWrite(paacd->hmmioDst, (char*)paacd->pbDst, dw) != (LONG)dw)
        goto FAILURE_EXIT;
    }

    LeftOvers = (cbRead - pash->cbSrcLengthUsed);
    if (LeftOvers != 0) // Some read stuff wasn't used, shift it to the front
      memmove(paacd->pbSrc, paacd->pbSrc + pash->cbSrcLengthUsed, LeftOvers);

    if (pash->cbSrcLengthUsed == 0 && pash->cbDstLengthUsed == 0)
    {
      // Stopped processing data, start winding things up
      if (ConvertFlags == ACM_STREAMCONVERTF_BLOCKALIGN)
      {
        // Do the partial block at the end and flush internal buffers
        ConvertFlags = ACM_STREAMCONVERTF_END;
      }
      else // That's all folks
        ConvertFlags = 0;
    }
  }

  ReturnCode = TRUE;

FAILURE_EXIT:
  // Keep the output buffer only if we were successful
  DeallocateConverterDescription(paacd, ReturnCode);
  return ReturnCode;
}



/*****************************************************************************
 * Copy the data out of a sound buffer and into either the destination RIFF
 * file or the compression source buffer.  If sample halving is turned on,
 * drop every other sample.  If converting to mono, drop one of the sound
 * channels.  Returns TRUE if successful.
 */

static BOOL CopyDataFromOneSoundBuffer(PAACONVERTDESC paacd,
                                       void* SoundBufferPntr,
                                       long SoundBufferByteSize)
{
  if (SoundBufferByteSize <= 0)
    return TRUE; // Nothing to do

  BYTE*   BytePntr                = NULL;
  BOOL    ConvertToMono           = paacd->bConvertStereoToMono;
  int     FinalSampleSize         = paacd->pwfxSrc->nBlockAlign;
  BOOL    EightBitSamples         = (FinalSampleSize <= 1);
  BOOL    HalveRate               = paacd->bHalveSamplingRate;
  UINT    SkipBytes               = 0;
  int     TempOutputSize          = 0;
  int     RecordedSampleSize      = FinalSampleSize;
  BYTE    TempOutputBuffer[1024];

  if (ConvertToMono)
    RecordedSampleSize *= 2; // We are recording in stereo

  if (HalveRate)
    SkipBytes += RecordedSampleSize;
  if (ConvertToMono)
    SkipBytes += FinalSampleSize; // Skip one channel (left or right, we don't know)

  if (SkipBytes > 0)
  {
    BytePntr = (BYTE*)SoundBufferPntr;
    while (SoundBufferByteSize > 0)
    {
      TempOutputSize = 0;
      while (SoundBufferByteSize > 0 && TempOutputSize < sizeof (TempOutputBuffer))
      {
        // Fill the temporary output buffer with the used portion of the data
        if (EightBitSamples)
        {
          TempOutputBuffer [TempOutputSize++] = *BytePntr++;
          SoundBufferByteSize--; // One more byte used up
        }
        else // Reading 16 bit samples (and maybe 24 or 32 but that's unimplemented)
        {
          TempOutputBuffer [TempOutputSize++] = *BytePntr++;
          TempOutputBuffer [TempOutputSize++] = *BytePntr++;
          SoundBufferByteSize -= 2;
        }

        // Drop a sample if we are halving and also drop half of a stereo
        //  sample.  May make SoundBufferByteSize go negative.

        BytePntr += SkipBytes;
        SoundBufferByteSize -= SkipBytes;
      }

      // Write the temporary buffer to the appropriate output

      if (paacd->destinationCompressor == 0)
      {
        if (mmioWrite(paacd->hmmioDst, (char*)TempOutputBuffer,
                      TempOutputSize) != (LONG)TempOutputSize)
        {
          return FALSE;
        }
        paacd->dwProgress += TempOutputSize;
      }
      else
      {
        memcpy(paacd->pbSrc + paacd->cbSrcLeftOvers,
               TempOutputBuffer, TempOutputSize);
        paacd->cbSrcLeftOvers += TempOutputSize;
      }
    }
  }
  else // Not halving samples or dropping channels.  All data used.
  {
    if (paacd->destinationCompressor == 0)
    {
      if (mmioWrite(paacd->hmmioDst, (char*)SoundBufferPntr,
          SoundBufferByteSize) != (LONG)SoundBufferByteSize)
      {
        return FALSE;
      }
      paacd->dwProgress += SoundBufferByteSize;
    }
    else
    {
      memcpy(paacd->pbSrc + paacd->cbSrcLeftOvers,
             SoundBufferPntr, SoundBufferByteSize);
      paacd->cbSrcLeftOvers += SoundBufferByteSize;
    }
  }
  return TRUE;
}



/*****************************************************************************
 * Compress the waiting amount of recorded data.  If Dregs is TRUE then it
 * will compress less than the full block size (includes zero), and
 * flush buffers.  Returns TRUE if successful, FALSE for errors.
 */

static BOOL GetAndCompressRecordedData(BOOL Dregs)
{
  if (RecordingBuffer == NULL || paacdRecord == NULL)
    return FALSE;

  DWORD             BusyCount             = 0;
  DWORD             CapturePosition       = 0;
  HRESULT           hErrorCode            = NULL;
  MMRESULT          mmr                   = NULL;
  int               MultiplicationFactor  = 1;
  LPACMSTREAMHEADER pash                  = NULL;
  DWORD             ReadableLength        = 0;
  DWORD             ReadPosition          = 0;
  BYTE*             SoundBufferPntr1      = NULL;
  BYTE*             SoundBufferPntr2      = NULL;
  DWORD             SoundBufferSize1      = 0;
  DWORD             SoundBufferSize2      = 0;
  DWORD             SpaceNeeded           = 0;
  BOOL              SndBufLocked          = FALSE;
  DSCBCAPS          BufferCaps;

  // See how much sound is available from the recorder.  Note: the
  //  CapturePosition isn't working in DirectX 5.0 (size is half the
  //  correct value).

  hErrorCode = RecordingBuffer->GetCurrentPosition(&CapturePosition, &ReadPosition);
  if (FAILED(hErrorCode)) goto FAILURE_EXIT;

  // How big is the buffer?
  memset(&BufferCaps, 0, sizeof (BufferCaps));
  BufferCaps.dwSize = sizeof (BufferCaps);
  hErrorCode = RecordingBuffer->GetCaps(&BufferCaps);
  if (FAILED(hErrorCode) || BufferCaps.dwBufferBytes == 0) 
    goto FAILURE_EXIT;

  // Calculate how many readable bytes there are behind the capture cursor
  if (ReadPosition < paacdRecord->dwNextReadOffset)
  {
    // The ReadPosition cursor has progressed past dwNextReadOffset and
    // re-cycled back to the top of the buffer, & moved on down from there.
    ReadableLength = ReadPosition + BufferCaps.dwBufferBytes - 
                     paacdRecord->dwNextReadOffset;
  }
  else
  {
    // The ReadPosition cursor has progressed past dwNextWriteOffset...
    ReadableLength = ReadPosition - paacdRecord->dwNextReadOffset;
  }

  // Find out how much data we have buffered up in all places.
  //   Measure with units the same as the source compression buffer.
  if (paacdRecord->bConvertStereoToMono)
    MultiplicationFactor *= 2;

  if (paacdRecord->bHalveSamplingRate)
    MultiplicationFactor *= 2;

  SpaceNeeded = ReadableLength + MultiplicationFactor * paacdRecord->cbSrcLeftOvers;
  SpaceNeeded /= MultiplicationFactor;

  // We will be taking chunks of 'paacdRecord->cbSrcReadSize' amount
  //  of data at a time from the DS record buffer (unless doing the dregs,
  //  when we take anything).

  if (!Dregs && (SpaceNeeded < paacdRecord->cbSrcReadSize))
    goto NORMAL_EXIT;  // Got to wait for more readable data to accumulate.

  // If there is more data than we can fit in the buffer, then limit it.
  // If the sampling rate is halved, have twice as much space as the
  // size values for the source buffer show (minus an extra sample if
  // we had a previously odd pair).

  if (SpaceNeeded > paacdRecord->cbSrcReadSize)
  {
    ReadableLength = paacdRecord->cbSrcReadSize - paacdRecord->cbSrcLeftOvers;
    ReadableLength *= MultiplicationFactor;
  }

  if (ReadableLength > 0)
  {
    // Suck the data out of the recording buffer
    hErrorCode = RecordingBuffer->Lock(paacdRecord->dwNextReadOffset,
                                       ReadableLength, 
                                       (void**)&SoundBufferPntr1,
                                       &SoundBufferSize1,
                                       (void**)&SoundBufferPntr2,
                                       &SoundBufferSize2, 0);

    if (FAILED(hErrorCode)) goto FAILURE_EXIT;

    SndBufLocked = TRUE;

    // If we aren't compressing the data, write it to the output file
    // else buffer it for the compressor...  Also cut it in half if
    // we are halving the sampling rate.

    if (!CopyDataFromOneSoundBuffer(paacdRecord, SoundBufferPntr1, SoundBufferSize1))
      goto FAILURE_EXIT;

    if (!CopyDataFromOneSoundBuffer(paacdRecord, SoundBufferPntr2, SoundBufferSize2))
      goto FAILURE_EXIT;

    // Unlock the record buffer to set it free...
    hErrorCode = RecordingBuffer->Unlock(SoundBufferPntr1, SoundBufferSize1,
                                         SoundBufferPntr2, SoundBufferSize2);
    SndBufLocked = FALSE;

    // Record the next read offset for later...
    paacdRecord->dwNextReadOffset += ReadableLength;

    if (paacdRecord->dwNextReadOffset >= BufferCaps.dwBufferBytes)
      paacdRecord->dwNextReadOffset -= BufferCaps.dwBufferBytes;
  }

  if (paacdRecord->destinationCompressor != 0)
  {
    // Have to compress the data we collected (may be zero length)
    pash                  = &paacdRecord->ash;
    pash->cbSrcLength     = paacdRecord->cbSrcLeftOvers;
    pash->cbDstLengthUsed = 0L;

    mmr = acmStreamConvert(paacdRecord->has, pash, (ReadableLength == 0 && Dregs) ?
                           ACM_STREAMCONVERTF_END : ACM_STREAMCONVERTF_BLOCKALIGN);
    if (mmr) goto FAILURE_EXIT;

    // Busy wait for it to finish, probably won't happen since we use synchronous
    while (0 == (ACMSTREAMHEADER_STATUSF_DONE & 
           ((AACONVERTDESC volatile*)paacdRecord)->ash.fdwStatus))
    {
      BusyCount++;
    }

    paacdRecord->cbSrcLeftOvers = paacdRecord->cbSrcLeftOvers - 
                                  pash->cbSrcLengthUsed;

    if (paacdRecord->cbSrcLeftOvers != 0)
    {
      // Some read stuff wasn't used, shift it to the front
      memmove(paacdRecord->pbSrc, paacdRecord->pbSrc + pash->cbSrcLengthUsed,
              paacdRecord->cbSrcLeftOvers);
    }

    if (pash->cbDstLengthUsed > 0)
    {
      if (mmioWrite(paacdRecord->hmmioDst, (char*)paacdRecord->pbDst, 
                    pash->cbDstLengthUsed) != (LONG)pash->cbDstLengthUsed)
      {
        goto FAILURE_EXIT;
      }
    }
    paacdRecord->dwProgress += pash->cbDstLengthUsed;
  }
  
  goto NORMAL_EXIT;


FAILURE_EXIT:
  if (SndBufLocked)
  {
    // Unlock the record buffer to set it free...
    hErrorCode = RecordingBuffer->Unlock(SoundBufferPntr1, SoundBufferSize1,
                                         SoundBufferPntr2, SoundBufferSize2);
  }
  return FALSE;

NORMAL_EXIT:
  return TRUE;
}



/*****************************************************************************
 * Start playing the given sound as a voice chat sound.  The data will
 * be copied into a DirectSound buffer, so you can deallocate it right
 * after calling this function.  Assumes the data pointed to is in
 * Microsoft's WAV file format (RIFF chunky file format with WAVE subtype),
 * preferably in mono (so panning makes sense).  For voice, 11025hz 8 bit mono
 * is good enough and doesn't take too much network bandwidth.
 *
 * The PanValue specifies where the sound appears, use DSBPAN_LEFT for far
 * left and DSB_RIGHT for far right and values inbetween for inbetween,
 * DSBPAN_CENTER for center.
 *
 * The VolumeValue controls how quiet the sound is.  DSBVOLUME_MAX is no
 * attenuation, DSBVOLUME_MIN (a large negative number) is silence.
 *
 * Returns TRUE if successfully started playing the sound, FALSE if something
 * went wrong (bad data format, no more chat buffers, etc).
 */

BOOL LE_VOICE_PLAY_CHAT(BYTE* WaveFileData, DWORD WaveFileSize,
                        LONG PanValue, LONG VolumeValue)
{
  LONG                AmountRead;
  PDSBUFFER           DSBuffer = NULL;
  HRESULT             ErrorCode;
  LPWAVEFORMATEX      lpWaveFormatEx;
  MMCKINFO            RIFFChunk;
  MMIOINFO            MMIOInfoRec;
  MMRESULT            MMErrorCode;
  BOOL                ReturnCode;
  HMMIO               RIFFFile = NULL;
  BOOL                SoundBufferIsLocked = FALSE;
  BYTE*               SoundBufferPntr1;
  BYTE*               SoundBufferPntr2;
  DWORD               SoundBufferSize1;
  DWORD               SoundBufferSize2;
  MMCKINFO            SubChunk;
  int                 VoiceBufferIndex;
  WAVEFORMATEX        WaveFormatEx;
  DSBUFFERDESC        DSBufferDesc;

  if (g_pDSoundObject == NULL)
    return FALSE; // DirectSound isn't open

  if (WaveFileData == NULL || WaveFileSize == 0)
    return FALSE; // Need some data to play

  // Do some garbage collection, so there might be some free buffers
  LI_VOICE_FREE_MEMORY(FALSE);

  VoiceBufferIndex = -1;
  for (int i = 0; i < MAX_VOICE_SIMULTANEOUS_PLAY; i++)
  {
    if (VoiceBuffers [i] == NULL)
    {
      VoiceBufferIndex = i;
      break;
    }
  }
  if (VoiceBufferIndex == -1)
    return FALSE; // No free chat slots

#if !FORHOTSEAT // Multithreaded - need mutual exclusion past here
//  WaitForSingleObject (hSoundTableMutex, INFINITE);
#endif

  // Decompress the data, if it was compressed.  Open a MMIO stream on the
  //  buffer and look for the "fmt " chunk which describes the compression
  //  format.

  memset(&MMIOInfoRec, 0, sizeof (MMIOInfoRec));
  MMIOInfoRec.fccIOProc = FOURCC_MEM;
  MMIOInfoRec.pchBuffer = (char*) WaveFileData;
  MMIOInfoRec.cchBuffer = WaveFileSize;

  RIFFFile = mmioOpen (NULL, &MMIOInfoRec, MMIO_READ);
  if (RIFFFile == NULL) goto FAILURE_EXIT;

  memset (&RIFFChunk, 0, sizeof (RIFFChunk));
  RIFFChunk.fccType = MAKEFOURCC('W', 'A', 'V', 'E');
  MMErrorCode       = mmioDescend(RIFFFile, &RIFFChunk, NULL, MMIO_FINDRIFF);

  if (MMErrorCode != MMSYSERR_NOERROR)
    goto FAILURE_EXIT;

  // Look for the "fmt " chunk that describes the wave format
  memset(&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = MAKEFOURCC('f', 'm', 't', ' ');
  MMErrorCode   = mmioDescend(RIFFFile, &SubChunk, &RIFFChunk, MMIO_FINDCHUNK);

  if (MMErrorCode != MMSYSERR_NOERROR)
    goto FAILURE_EXIT;
  
  lpWaveFormatEx = (tWAVEFORMATEX *) (WaveFileData + SubChunk.dwDataOffset);

  // Is it the uncompressed format?
  if (lpWaveFormatEx->wFormatTag == WAVE_FORMAT_PCM)
  {
    // Uncompressed, leave the RIFF file pointing to the start of the file
    mmioAscend (RIFFFile, &SubChunk, 0);
    mmioAscend (RIFFFile, &RIFFChunk, 0);
    MMErrorCode = mmioSeek (RIFFFile, 0, SEEK_SET);
    if (MMErrorCode != MMSYSERR_NOERROR)
      goto FAILURE_EXIT;
  }
  else // Got some compressed data that needs decompressing
  {
    // Allocate an AACONVERTDESC for the conversion...
    paacdPlay = (PAACONVERTDESC)GlobalAlloc(GPTR, sizeof (AACONVERTDESC));
    if (paacdPlay == NULL)
      goto FAILURE_EXIT;

    // Allocate space for the input WAVEFORMATEX struct.
    paacdPlay->pwfxSrc = (LPWAVEFORMATEX)GlobalAlloc(GPTR, 
                                                     sizeof(WAVEFORMATEX) + 
                                                     lpWaveFormatEx->cbSize);

    if (paacdPlay->pwfxSrc == NULL) goto FAILURE_EXIT;

    memcpy(paacdPlay->pwfxSrc, lpWaveFormatEx,
           sizeof(WAVEFORMATEX) + lpWaveFormatEx->cbSize);

    // Request output in PCM format
    paacdPlay->destinationCompressor = 0; // No compression's magic index

    // Set up the RIFF file to point at the start of the data chunk contents

    mmioAscend (RIFFFile, &SubChunk, 0);
    memset (&SubChunk, 0, sizeof (SubChunk));
    SubChunk.ckid = MAKEFOURCC ('d', 'a', 't', 'a');
    MMErrorCode = mmioDescend (RIFFFile, &SubChunk, &RIFFChunk, MMIO_FINDCHUNK);
    if (MMErrorCode != MMSYSERR_NOERROR)
      goto FAILURE_EXIT;

    // Hand over control of the RIFF input file to the stream system.

    paacdPlay->hmmioSrc = RIFFFile;
    RIFFFile = NULL;
    paacdPlay->ckSrcRIFF = RIFFChunk;
    paacdPlay->ckSrc = SubChunk;

    if (!PrepareToConvert (paacdPlay)) goto FAILURE_EXIT;
    if (!PrepareDestRIFF (paacdPlay)) goto FAILURE_EXIT;
    if (!AcmConvertEntireFile (paacdPlay)) goto FAILURE_EXIT;

    // Take control of the output RIFF file, but leave the memory
    //  deallocation to the stream stuff later.  It will be positioned
    //  at the start of the file.

    RIFFFile = paacdPlay->hmmioDst;
    paacdPlay->hmmioDst = NULL;
  }

  // Parse the sound data, find wave header for playback parameters.
  //  Look for the RIFF chunk with subtype WAVE.

  memset(&RIFFChunk, 0, sizeof (RIFFChunk));
  RIFFChunk.fccType = MAKEFOURCC ('W', 'A', 'V', 'E');
  MMErrorCode = mmioDescend (RIFFFile, &RIFFChunk, NULL, MMIO_FINDRIFF);
  if (MMErrorCode != MMSYSERR_NOERROR)
    goto FAILURE_EXIT;

  // Look for the "fmt " chunk that describes the wave format

  memset(&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = MAKEFOURCC ('f', 'm', 't', ' ');
  MMErrorCode = mmioDescend (RIFFFile, &SubChunk, &RIFFChunk, MMIO_FINDCHUNK);
  if (MMErrorCode != MMSYSERR_NOERROR)
    goto FAILURE_EXIT;

  memset(&WaveFormatEx, 0, sizeof (WaveFormatEx));
  AmountRead = mmioRead(RIFFFile, (char *)&WaveFormatEx, sizeof(WaveFormatEx));
  if (AmountRead < sizeof (WAVEFORMAT))
    goto FAILURE_EXIT;  // Need at least the basic header part

  MMErrorCode = mmioAscend(RIFFFile, &SubChunk, 0);
  if (MMErrorCode != MMSYSERR_NOERROR)
    goto FAILURE_EXIT;

  if (WaveFormatEx.wFormatTag != WAVE_FORMAT_PCM)
    goto FAILURE_EXIT;  // We don't understand this format

  if (WaveFormatEx.nChannels == 0 || WaveFormatEx.nSamplesPerSec == 0 ||
      WaveFormatEx.nBlockAlign == 0 || WaveFormatEx.wBitsPerSample == 0)
  {
    goto FAILURE_EXIT;  // Something is wrong.  Unplayable settings.
  }

  // Find the start and size of the "data" chunk, which has
  //  the actual sound data.  Should be somewhere after the
  //  "fmt " chunk.

  memset(&SubChunk, 0, sizeof (SubChunk));
  SubChunk.ckid = MAKEFOURCC('d', 'a', 't', 'a');
  MMErrorCode = mmioDescend(RIFFFile, &SubChunk, &RIFFChunk, MMIO_FINDCHUNK);

  if (MMErrorCode != MMSYSERR_NOERROR)
    goto FAILURE_EXIT;

  if (SubChunk.cksize < DSBSIZE_MIN || SubChunk.cksize > DSBSIZE_MAX)
    goto FAILURE_EXIT;

  // Allocate a DirectSound buffer big enough to hold all the data.  Also
  //  make it sticky so that chat messages continue to sound while this
  //  application is in the background (but stop when another DirectSound
  //  program is in the foreground).

  memset(&DSBufferDesc, 0, sizeof(DSBufferDesc));
  DSBufferDesc.dwSize   = sizeof (DSBufferDesc);
  DSBufferDesc.dwFlags  = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME |
                          DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STICKYFOCUS;
  DSBufferDesc.dwBufferBytes  = SubChunk.cksize;
  DSBufferDesc.lpwfxFormat    = &WaveFormatEx;

  ErrorCode = g_pDSoundObject->CreateSoundBuffer(&DSBufferDesc, 
                                                 VoiceBuffers + 
                                                 VoiceBufferIndex, NULL);
  DSBuffer = VoiceBuffers [VoiceBufferIndex];
  if (FAILED (ErrorCode) || DSBuffer == NULL)
    goto FAILURE_EXIT;

  // Copy the data from the WAVE file to the DirectSound buffer

  ErrorCode = DSBuffer->Lock(0, SubChunk.cksize,
                            (void**)&SoundBufferPntr1, &SoundBufferSize1,
                            (void**)&SoundBufferPntr2, &SoundBufferSize2, 0);

  if (ErrorCode == DSERR_BUFFERLOST)
  {
    DSBuffer->Restore();

    ErrorCode = DSBuffer->Lock(0, SubChunk.cksize,
                              (void**)&SoundBufferPntr1, &SoundBufferSize1,
                              (void**)&SoundBufferPntr2, &SoundBufferSize2, 0);
  }

  if (FAILED (ErrorCode))
    goto FAILURE_EXIT;

  SoundBufferIsLocked = TRUE;

  if (SoundBufferSize1 > 0)
  {
    AmountRead = mmioRead(RIFFFile, (char*)SoundBufferPntr1, SoundBufferSize1);
    if (AmountRead != (LONG)SoundBufferSize1)
      goto FAILURE_EXIT;
  }

  if (SoundBufferSize2 > 0)
  {
    AmountRead = mmioRead(RIFFFile, (char*)SoundBufferPntr2, SoundBufferSize2);
    if (AmountRead != (LONG) SoundBufferSize2)
      goto FAILURE_EXIT;
  }

  DSBuffer->Unlock(SoundBufferPntr1, SoundBufferSize1,
                   SoundBufferPntr2, SoundBufferSize2);

  SoundBufferIsLocked = FALSE;

  // Set the volume and panning

  ErrorCode = DSBuffer->SetVolume(VolumeValue);
  if (FAILED(ErrorCode))
    goto FAILURE_EXIT;

  ErrorCode = DSBuffer->SetPan(PanValue);
  if (FAILED(ErrorCode))
    goto FAILURE_EXIT;

  // Start the sound playing

  ErrorCode = DSBuffer->Play(0, 0, 0);
  if (FAILED(ErrorCode))
    goto FAILURE_EXIT;

  ReturnCode = TRUE;
  goto NORMAL_EXIT;


FAILURE_EXIT:
  ReturnCode = FALSE;

  if (SoundBufferIsLocked)
  {
    DSBuffer->Unlock(SoundBufferPntr1, SoundBufferSize1,
                     SoundBufferPntr2, SoundBufferSize2);
  }

  if (DSBuffer != NULL)
  {
    DSBuffer->Release();
    VoiceBuffers[VoiceBufferIndex] = NULL;
  }

NORMAL_EXIT:
  if (RIFFFile != NULL)
  {
    mmioClose(RIFFFile, 0);
    RIFFFile = NULL;
  }

  ClearPlayConverterDescription();

#if !FORHOTSEAT
//  ReleaseMutex (hSoundTableMutex);
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Start the recording of sound into a hidden sound buffer.  Returns TRUE if
 * successful.  Use LE_VOICE_RECORD_GET_POSITION to see how the recording is
 * going and LE_VOICE_RECORD_FINISH to retrieve the recorded sound data.
 *
 * The recording will use the settings in LE_VOICE_DESIRED_COMPRESSOR,
 * LE_VOICE_SAMPLES_PER_SEC and LE_VOICE_BITS_PER_SAMPLE as a guide.  It
 * will keep on running until you stop it with LE_VOICE_RECORD_FINISH
 * (LE_VOICE_RECORD_GET_POSITION will say it has done 100% when it has
 * recorded MaxRecordingSize bytes of compressed data, but that doesn't
 * actually stop it).
 */

typedef struct CaptureSettingsStruct
{
  DWORD format;
  long  hz;
  short bits;
  short channels;
} CaptureSettingsRecord, *CaptureSettingsPointer;

#define MAX_STD_CAPTURE_SETTINGS 12

static CaptureSettingsRecord StandardCaptureSettings[MAX_STD_CAPTURE_SETTINGS] =
{
  {WAVE_FORMAT_1M08, 11025, 8,  1},
  {WAVE_FORMAT_1M16, 11025, 16, 1},
  {WAVE_FORMAT_2M08, 22050, 8,  1},
  {WAVE_FORMAT_2M16, 22050, 16, 1},
  {WAVE_FORMAT_4M08, 44100, 8,  1},
  {WAVE_FORMAT_4M16, 44100, 16, 1},
  // Some sound cards only do stereo.  But we don't have time to implement
  // the stereo to mono conversion just now, so allow stereo recording.
  {WAVE_FORMAT_1M08, 11025, 8,  2},
  {WAVE_FORMAT_1M16, 11025, 16, 2},
  {WAVE_FORMAT_2M08, 22050, 8,  2},
  {WAVE_FORMAT_2M16, 22050, 16, 2},
  {WAVE_FORMAT_4M08, 44100, 8,  2},
  {WAVE_FORMAT_4M16, 44100, 16, 2}
};


// Quicksort comparison function
int CompareStandardCapture(const void *E1, const void *E2)
{
  CaptureSettingsPointer  Element1;
  CaptureSettingsPointer  Element2;
  long                    Error1;
  long                    Error2;

  Element1 = (CaptureSettingsStruct *) E1;
  Element2 = (CaptureSettingsStruct *) E2;

  Error1 = labs(Element1->hz - LE_VOICE_SAMPLES_PER_SEC) + 2000 * 
           labs(Element1->bits - LE_VOICE_BITS_PER_SAMPLE) + 50000 * 
           labs(Element1->channels - LE_VOICE_NUMBER_OF_CHANNELS);

  Error2 = labs(Element2->hz - LE_VOICE_SAMPLES_PER_SEC) + 2000 *
           labs(Element2->bits - LE_VOICE_BITS_PER_SAMPLE) + 50000 *
           labs(Element2->channels - LE_VOICE_NUMBER_OF_CHANNELS);

  return Error1 - Error2;
}


BOOL LE_VOICE_RECORD_START(DWORD MaxRecordingSize)
{
  DWORD         ActualRecordingSize;
  DSCBUFFERDESC CapBufDescription;
  HRESULT       ErrorCode;
  WAVEFORMATEX  WaveFormatEx;

  // Check that the sound system is available

  if (g_pDSCaptureObject == NULL || MaxRecordingSize < 1)
    return FALSE;

  if (RecordingBuffer != NULL)
    LI_VOICE_FREE_RECORD ();

  SavedMaxRecordingSize = MaxRecordingSize;

  // Sort the standard sound settings so that the ones closest to the
  //  requested LE_VOICE_SAMPLES_PER_SEC and LE_VOICE_BITS_PER_SAMPLE
  //  come first.

  qsort(StandardCaptureSettings, MAX_STD_CAPTURE_SETTINGS,
        sizeof(CaptureSettingsRecord), CompareStandardCapture);

  // Try to create a sound recording buffer, first with the
  //  given settings and then try standard settings in order
  //  of "closest" to the given settings, until we find one
  //  that works.

  for (int i = -1; i < MAX_STD_CAPTURE_SETTINGS; i++)
  {
    memset(&WaveFormatEx, 0, sizeof (WaveFormatEx));
    WaveFormatEx.wFormatTag       = WAVE_FORMAT_PCM; // Standard uncompressed format
    WaveFormatEx.nChannels        = (i < 0) ? LE_VOICE_NUMBER_OF_CHANNELS :
                                    StandardCaptureSettings[i].channels;
    WaveFormatEx.nSamplesPerSec   = (i < 0) ? LE_VOICE_SAMPLES_PER_SEC : 
                                    StandardCaptureSettings[i].hz;
    WaveFormatEx.wBitsPerSample   = (i < 0) ? LE_VOICE_BITS_PER_SAMPLE : 
                                    StandardCaptureSettings[i].bits;
    WaveFormatEx.nBlockAlign      = WaveFormatEx.nChannels * 
                                    WaveFormatEx.wBitsPerSample / 8;
    WaveFormatEx.nAvgBytesPerSec  = WaveFormatEx.nSamplesPerSec * 
                                    WaveFormatEx.nBlockAlign;

    // Set the DS capture buffer size to hold 10 seconds of sound.
    ActualRecordingSize = WaveFormatEx.nAvgBytesPerSec * 10;

    if (ActualRecordingSize <= 0)
      continue; // Can't use this setting, try another.

    memset(&CapBufDescription, 0, sizeof (CapBufDescription));
    CapBufDescription.dwSize        = sizeof (CapBufDescription);
    CapBufDescription.dwFlags       = DSCBCAPS_WAVEMAPPED;

    // Allow conversions to desired format.  Doesn't seem to do anything.
    CapBufDescription.dwBufferBytes = ActualRecordingSize;
    CapBufDescription.lpwfxFormat   = &WaveFormatEx;

    ErrorCode = g_pDSCaptureObject->CreateCaptureBuffer(&CapBufDescription,
                                                        &RecordingBuffer,
                                                        NULL);

    if (FAILED(ErrorCode))
      continue; // Ok, this one isn't supported or runs out of memory. Try next one.

    // Found a capture setting that works
    break;
  }

  if (i >= MAX_STD_CAPTURE_SETTINGS || RecordingBuffer == NULL)
    return FALSE; // Couldn't find one that worked

  // Allocate an AACONVERTDESC for the conversion and/or mmio file IO
  paacdRecord = (PAACONVERTDESC)GlobalAlloc(GPTR, sizeof(AACONVERTDESC));
  if (paacdRecord == NULL) goto FAILURE_EXIT;

  // Update the global variables showing what we are actually recording with,
  //  or what we are pretending to record with
  if (WaveFormatEx.nSamplesPerSec / 2 == (DWORD)LE_VOICE_SAMPLES_PER_SEC)
  {
    // Fake it, record at 22050 and convert down to 11025 before using it
    LE_VOICE_SAMPLES_PER_SEC = WaveFormatEx.nSamplesPerSec / 2;
    paacdRecord->bHalveSamplingRate = TRUE;
  }
  else
  {
    paacdRecord->bHalveSamplingRate = FALSE;
    LE_VOICE_SAMPLES_PER_SEC        = WaveFormatEx.nSamplesPerSec;
  }

  LE_VOICE_BITS_PER_SAMPLE = WaveFormatEx.wBitsPerSample;

  if (WaveFormatEx.nChannels == 2)
  {
    // Converting stereo to mono for cards that only do stereo
    paacdRecord->bConvertStereoToMono = TRUE;
    LE_VOICE_NUMBER_OF_CHANNELS       = 1;
  }
  else // Recording in mono or more than stereo
  {
    paacdRecord->bConvertStereoToMono = FALSE;
    LE_VOICE_NUMBER_OF_CHANNELS       = WaveFormatEx.nChannels;
  }

  paacdRecord->pwfxSrc = (LPWAVEFORMATEX)GlobalAlloc(GPTR, sizeof(WAVEFORMATEX));
  if (paacdRecord->pwfxSrc == NULL) goto FAILURE_EXIT;

  // Save the source wave format settings, these are at the fake recording
  //  speed, after mono conversion, and correspond to the wave data we give
  //  to the compressor

  // Copy the contents, not the pointer
  *paacdRecord->pwfxSrc                 = WaveFormatEx; 
  paacdRecord->pwfxSrc->nSamplesPerSec  = LE_VOICE_SAMPLES_PER_SEC;
  paacdRecord->pwfxSrc->nChannels       = LE_VOICE_NUMBER_OF_CHANNELS;
  paacdRecord->pwfxSrc->nBlockAlign     = paacdRecord->pwfxSrc->nChannels *
                                          paacdRecord->pwfxSrc->wBitsPerSample / 8;
  paacdRecord->pwfxSrc->nAvgBytesPerSec = paacdRecord->pwfxSrc->nSamplesPerSec *
                                          paacdRecord->pwfxSrc->nBlockAlign;

  // If compression is desired, get setup for it...
  if (LE_VOICE_DESIRED_COMPRESSOR > 0)
  {
    if (LE_VOICE_DESIRED_COMPRESSOR >= LE_VOICE_NUM_COMPRESSORS) goto FAILURE_EXIT;
    if (LI_VOICE_ACM_AVAILABLE == FALSE) goto FAILURE_EXIT;

    paacdRecord->destinationCompressor = LE_VOICE_DESIRED_COMPRESSOR;

    // Open the ACM compression stream...
    if (!PrepareToConvert(paacdRecord)) goto FAILURE_EXIT;
  }
  else // No compression...
  {
    paacdRecord->destinationCompressor = 0;

    // Will copy blocks equal to 1 second's worth of the recorded sound to output file.
    paacdRecord->cbSrcReadSize = paacdRecord->pwfxSrc->nAvgBytesPerSec;

    // Need copy of the WAVEFORMATEX for the output file...
    paacdRecord->pwfxDst = (LPWAVEFORMATEX)GlobalAlloc(GPTR, sizeof(WAVEFORMATEX));
    if (paacdRecord->pwfxDst == NULL) goto FAILURE_EXIT;
    *(paacdRecord->pwfxDst) = *(paacdRecord->pwfxSrc);
  }

  // Open a memory based RIFF file.  The multimedia routines will
  // reallocate the memory as space is needed.
  if (!PrepareDestRIFF(paacdRecord)) goto FAILURE_EXIT;

  // Start recording
  ErrorCode = RecordingBuffer->Start(DSCBSTART_LOOPING);
  if (FAILED (ErrorCode))
    goto FAILURE_EXIT;

  return TRUE; // LE_VOICE_RECORD_GET_POSITION will now take care of recording


FAILURE_EXIT:
  LI_VOICE_FREE_RECORD();
  ClearRecordConverterDescription();
  return FALSE;
}



/*****************************************************************************
 * See what is happening with the recording.  Returns a number between
 * 0.0 (nothing) and 1.0 (all done) to show how much has been proportionally
 * recorded (proportional to MaxRecordingSize).
 *
 * Writes blocks of sound data to the output file, compressing them first
 * if required.
 *
 * Returns 1.1 if something is wrong (giving more than 100% to make the caller
 * hopefully stop recording).
 */

float LE_VOICE_RECORD_GET_POSITION(void)
{
  DWORD     dwStatusFlags = 0;

  if (RecordingBuffer == NULL || paacdRecord == NULL)
    return 1.1F;

  HRESULT hErrorCode = RecordingBuffer->GetStatus(&dwStatusFlags);
  if (FAILED(hErrorCode)) return 1.1F;

  // See if it has stopped capturing.  That means it is finished.
  if ((dwStatusFlags & DSCBSTATUS_CAPTURING) == 0) return 1.0F;

  // Compress the recorded data and store it

  if (!GetAndCompressRecordedData(FALSE))
    return 1.1F; // Bad error happened

  return((float)paacdRecord->dwProgress / (float)SavedMaxRecordingSize);
}



/*****************************************************************************
 * Finish up the recording.  Stops it if it is in progress.  Pass in NULL
 * pointers to cancel recording.  Returns TRUE if successful and allocates
 * a buffer for you.
 */

BOOL LE_VOICE_RECORD_FINISH (BYTE** WaveBufferPntrPntr, DWORD* WaveBufferSizePntr)
{
  DWORD*  DWordPntr   = NULL;
  DWORD   OutputSize  = 0;
  BOOL    LastData    = FALSE;

  if (WaveBufferPntrPntr == NULL || WaveBufferSizePntr == NULL ||
      RecordingBuffer == NULL)
  {
    goto FAILURE_EXIT;
  }
  // Make sure it has stopped recording
  RecordingBuffer->Stop();

  // Drain the last of the data from the DS record buffer

  OutputSize = paacdRecord->dwProgress - 1;
  while (OutputSize != paacdRecord->dwProgress)
  {
    OutputSize = paacdRecord->dwProgress;
    if (!GetAndCompressRecordedData(TRUE /* Do the dregs */))
      goto FAILURE_EXIT;
  }

  // Wrap up the mmio...

  DeallocateConverterDescription(paacdRecord, TRUE);

  mmioClose(paacdRecord->hmmioDst, 0);
  paacdRecord->hmmioDst = NULL;

  // Get a fresh pointer to the memory block, in case it got moved around
  // while the RIFF writing code was reallocating it.

  RULE_UnlockHandle(paacdRecord->destMemoryHandle);
  paacdRecord->destMemoryPntr = (BYTE*)RULE_LockHandle(paacdRecord->destMemoryHandle);
  if (paacdRecord->destMemoryPntr == NULL) goto FAILURE_EXIT;

  // Find out the size of the RIFF data in the buffer.

  DWordPntr = (DWORD*)paacdRecord->destMemoryPntr;
  if (*DWordPntr != MAKEFOURCC ('R', 'I', 'F', 'F'))
    goto FAILURE_EXIT; // This isn't a RIFF format buffer!

  DWordPntr++; // Point at the size of the RIFF chunk contents.
  OutputSize = *DWordPntr + 8; // Add 8 for RIFF header size at the front

  /* Give control of the memory to the caller. */

  *WaveBufferSizePntr           = OutputSize;
  *WaveBufferPntrPntr           = paacdRecord->destMemoryPntr;
  paacdRecord->destMemoryHandle = NULL;
  paacdRecord->destMemoryPntr   = NULL;
  goto NORMAL_EXIT;

FAILURE_EXIT:
  if (WaveBufferSizePntr != NULL)
    *WaveBufferSizePntr = 0;

  if (WaveBufferPntrPntr != NULL)
    *WaveBufferPntrPntr = NULL;

NORMAL_EXIT:
  ClearRecordConverterDescription();
  LI_VOICE_FREE_RECORD();
  return TRUE;
}



/*****************************************************************************
 * Go through all format tags that the driver supplies and add new ones to
 * our list of compressors.
 */

BOOL CALLBACK AcmFormatTagEnumCallback(HACMDRIVERID           hadid,
                                       LPACMFORMATTAGDETAILS  paftd,
                                       DWORD                  dwInstance,
                                       DWORD                  fdwSupport)
{
  ACMFORMATDETAILS    afd;
  DWORD               BestDataRate;
  int                 BestDataRateIndex;
  DWORD               dwDstFmtSize;
  DWORD               FormatSubtypeIndex;
  HACMDRIVER          had;
  MMRESULT            mmr;
  int                 StringLength;
  ACMFORMATDETAILS    SubtypeDetails;
  WAVEFORMATEX        wfxSrc;
  LPWAVEFORMATEX      pwfxDst;

  if (LE_VOICE_NUM_COMPRESSORS >= LE_VOICE_MAX_COMPRESSORS)
    return FALSE; // Stop it!

  if ((fdwSupport & ACMDRIVERDETAILS_SUPPORTF_CODEC) == 0)
    return TRUE; // Not a Codec type of tag, ignore it

  if (paftd == NULL || paftd->szFormatTag == NULL)
    return TRUE; // Can't use one with no name

  // See if this is a new kind of tag

  for (int i = 0; i < LE_VOICE_NUM_COMPRESSORS; i++)
  {
    if (paftd->dwFormatTag == CompressorIDs[i]->wFormatTag)
      return TRUE; // Already have this one
  }

  // Will this compressor work with the input PCM format?
  memset(&wfxSrc, 0, sizeof(wfxSrc));
  // Standard uncompressed format
  wfxSrc.wFormatTag       = WAVE_FORMAT_PCM; 
  wfxSrc.nChannels        = LE_VOICE_NUMBER_OF_CHANNELS;
  wfxSrc.nSamplesPerSec   = LE_VOICE_SAMPLES_PER_SEC;
  wfxSrc.wBitsPerSample   = LE_VOICE_BITS_PER_SAMPLE;
  wfxSrc.nBlockAlign      = wfxSrc.nChannels * wfxSrc.wBitsPerSample / 8;
  wfxSrc.nAvgBytesPerSec  = wfxSrc.nSamplesPerSec * wfxSrc.nBlockAlign;
  wfxSrc.cbSize           = 0;

  // Initialize all unused members of the ACMFORMATDETAILS structure to zero.
  memset(&afd, 0, sizeof(afd));

  // To get short & long name of this compressor, do this...
  //  mmr = acmDriverDetails(hadid, &add, 0);
  // If he doesn't want to give his name, we don't want anything to do with him!
  // if(mmr)return TRUE;
  // The name can now be found in struct 'add'.

  // Open the driver so we can query it.
  mmr = acmDriverOpen(&had, hadid, 0L);
  
  // If we can't open the driver, it is useless!
  if (mmr) return TRUE;

  // Fill in the required members of the ACMFORMATDETAILS
  //  structure for the ACM_FORMATDETAILSF_FORMAT query.
  afd.cbStruct    = sizeof(afd);
  afd.dwFormatTag = wfxSrc.wFormatTag;
  afd.pwfx        = &wfxSrc;
  afd.cbwfx       = sizeof (wfxSrc);

  // Ask the driver if he is able to use the supplied format...
  mmr = acmFormatDetails(had, &afd, ACM_FORMATDETAILSF_FORMAT);
  // If the driver can't use the input format, it is useless to us right now!
  if (mmr || !afd.fdwSupport)
  {
    acmDriverClose(had, 0);
    return TRUE;
  }

  dwDstFmtSize = 0;
  acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, (LPVOID)&dwDstFmtSize); // never fails...

  // Allocate a maybe temporary WAVEFORMATEX to place info about output formats...
  pwfxDst = (LPWAVEFORMATEX)GlobalAlloc(GPTR, dwDstFmtSize);
  if(pwfxDst == NULL)
  {
    acmDriverClose(had, 0);
    return FALSE;
  }

  // Init the output waveformatex structure...
  pwfxDst->nSamplesPerSec = wfxSrc.nSamplesPerSec;
  pwfxDst->nChannels      = wfxSrc.nChannels;

  // Ask the driver what he would do with the specified input format
  mmr = acmFormatSuggest(had, &wfxSrc, pwfxDst, dwDstFmtSize,
                         ACM_FORMATSUGGESTF_NCHANNELS | 
                         ACM_FORMATSUGGESTF_NSAMPLESPERSEC);
  if (mmr || pwfxDst->wFormatTag != paftd->dwFormatTag)
  {
    // Something wrong - he won't respond to the query or uses a different format.
    acmDriverClose(had, 0);
    GlobalFree(pwfxDst);
    return TRUE;
  }

  /* Go through all the subtypes of the format and find the one which
     matches our input sample rate and also has the lowest data rate.
     Don't care about bits per sample since that reflects the compressed
     bits per sample which will be smaller than the uncompressed one. */

  BestDataRate      = (DWORD)-1;
  BestDataRateIndex = -1;

  for (FormatSubtypeIndex = 0; FormatSubtypeIndex < paftd->cStandardFormats;
       FormatSubtypeIndex++)
  {
    memset(pwfxDst, 0, dwDstFmtSize);
    memset(&SubtypeDetails, 0, sizeof (SubtypeDetails));
    SubtypeDetails.cbStruct       = sizeof (SubtypeDetails);
    SubtypeDetails.dwFormatIndex  = FormatSubtypeIndex;
    SubtypeDetails.dwFormatTag    = paftd->dwFormatTag;
    SubtypeDetails.pwfx           = pwfxDst;
    SubtypeDetails.cbwfx          = dwDstFmtSize;

    mmr = acmFormatDetails(had, &SubtypeDetails, ACM_FORMATDETAILSF_INDEX);

    if (mmr == 0 && pwfxDst->nChannels == wfxSrc.nChannels &&
    pwfxDst->nSamplesPerSec == (DWORD)LE_VOICE_SAMPLES_PER_SEC)
    {
      if (pwfxDst->nAvgBytesPerSec < BestDataRate)
      {
        BestDataRate      = pwfxDst->nAvgBytesPerSec;
        BestDataRateIndex = FormatSubtypeIndex;
      }
    }
  }

  /* Grab the particular settings of the best data rate, includes the cbSize
     bytes of extra data after the plain WAVEFORMATEX record. */

  if (BestDataRateIndex >= 0)
  {
    memset(pwfxDst, 0, dwDstFmtSize);
    memset(&SubtypeDetails, 0, sizeof (SubtypeDetails));
    SubtypeDetails.cbStruct       = sizeof (SubtypeDetails);
    SubtypeDetails.dwFormatIndex  = BestDataRateIndex;
    SubtypeDetails.dwFormatTag    = paftd->dwFormatTag;
    SubtypeDetails.pwfx           = pwfxDst;
    SubtypeDetails.cbwfx          = dwDstFmtSize;
    acmFormatDetails(had, &SubtypeDetails, ACM_FORMATDETAILSF_INDEX);
  }

  acmDriverClose(had, 0);

  if (BestDataRateIndex < 0) // No useable settings
  {
    GlobalFree(pwfxDst);
    return TRUE;
  }

  // Got the settings, add it to our collection

  LE_VOICE_NUM_COMPRESSORS++; // Here so cleanup will deallocate partials

  CompressorIDs[LE_VOICE_NUM_COMPRESSORS-1] = pwfxDst;

  StringLength = strlen(paftd->szFormatTag);
  LE_VOICE_COMPRESSOR_NAMES[LE_VOICE_NUM_COMPRESSORS - 1] = 
    (char*)GlobalAlloc(GPTR, StringLength + 1);

  if (LE_VOICE_COMPRESSOR_NAMES [LE_VOICE_NUM_COMPRESSORS - 1] == NULL)
    return FALSE; /* Out of memory, stop. */

  strcpy(LE_VOICE_COMPRESSOR_NAMES [LE_VOICE_NUM_COMPRESSORS - 1],
         paftd->szFormatTag);

  return TRUE;
}



/*****************************************************************************
 * Examine a single compression codec.  Add its tags to our list of
 * compression techniques.
 */

BOOL CALLBACK AcmDriverEnumCallback(HACMDRIVERID hadid, DWORD dwInstance,
                                    DWORD fdwSupport)
{
  HACMDRIVER          DriverHandle;
  MMRESULT            mmr;
  ACMFORMATTAGDETAILS TagDetails;

  if (LE_VOICE_NUM_COMPRESSORS >= LE_VOICE_MAX_COMPRESSORS)
    return FALSE; // Time to stop

  if ((fdwSupport & ACMDRIVERDETAILS_SUPPORTF_CODEC) == 0)
    return TRUE; // This isn't a Codec, so ignore it

  DriverHandle = NULL;
  mmr = acmDriverOpen(&DriverHandle, hadid, 0);
  if (mmr)
    return FALSE;

  memset(&TagDetails, 0, sizeof(TagDetails));
  TagDetails.cbStruct = sizeof(TagDetails);

  mmr = acmFormatTagEnum(DriverHandle, &TagDetails,
                         AcmFormatTagEnumCallback, 0, 0);

  acmDriverClose(DriverHandle, 0);

  if (mmr != MMSYSERR_NOERROR)
    return FALSE; /* Something went wrong. */

  return (TRUE);
}



/*****************************************************************************
 * Using the current recording settings in LE_VOICE_SAMPLES_PER_SEC and
 * LE_VOICE_BITS_PER_SAMPLE, set the list of compressors to
 * contain all the Codecs that work with those settings.
 */

void LE_VOICE_REGENERATE_COMPRESSOR_LIST()
{
  char  PreviousCompressorName[100];

  if (LE_VOICE_DESIRED_COMPRESSOR < LE_VOICE_NUM_COMPRESSORS &&
      LE_VOICE_COMPRESSOR_NAMES[LE_VOICE_DESIRED_COMPRESSOR] != NULL)
  {
    strncpy(PreviousCompressorName,
            LE_VOICE_COMPRESSOR_NAMES[LE_VOICE_DESIRED_COMPRESSOR],
            sizeof(PreviousCompressorName));
  }
  else
    strcpy(PreviousCompressorName, "None");

  ClearCompressorIDList();

  /* Initialise the no-compression setting.  This is really only used for
     decompressing files.  Zero settings mean the acmFormatSuggest will
     pick appropriate values. */

  LE_VOICE_NUM_COMPRESSORS      = 1;
  LE_VOICE_COMPRESSOR_NAMES[0]  = "No Compression";
  CompressorIDs[0]              = &NoCompressionWaveFormatID;
  CompressorIDs[0]->wFormatTag  = WAVE_FORMAT_PCM;
  CompressorIDs[0]->nChannels   = LE_VOICE_NUMBER_OF_CHANNELS;

  // Enumerate the available compression wave format tags.
  LI_VOICE_ACM_AVAILABLE = TRUE;
  acmDriverEnum(AcmDriverEnumCallback, 0, 0);

  // Restore the current compressor if possible
  for (int i = 0; i < LE_VOICE_NUM_COMPRESSORS; i++)
  {
    if (0 == strcmp(LE_VOICE_COMPRESSOR_NAMES [i], PreviousCompressorName))
    {
      LE_VOICE_DESIRED_COMPRESSOR = i;
      break;
    }
  }
}



/*****************************************************************************
 * Add the capture devices to the list of available ones.  This is a callback
 * function that the DirectSoundCaptureEnumerate function calls.
 */

#define MAX_CAPTURE_DEVICES 3
static BYTE NumberOfCaptureDevices;
static GUID CaptureDeviceGUIDs[MAX_CAPTURE_DEVICES];

BOOL CALLBACK LI_VOICE_EnumDSCaptureProc(LPGUID lpguidDevice, LPCSTR lpszDesc,
                                         LPCSTR lpszDrvName, LPVOID lpContext)
{
  if (NumberOfCaptureDevices < MAX_CAPTURE_DEVICES && lpguidDevice != NULL)
  {
    CaptureDeviceGUIDs[NumberOfCaptureDevices] = *lpguidDevice;
    NumberOfCaptureDevices++;
  }

  return(NumberOfCaptureDevices < MAX_CAPTURE_DEVICES);
}



/*****************************************************************************
 * Initialise the voice chat system.  Essentially just opens the DirectSound
 * object and capture object.  Always succeeds, you just have a game with no
 * sound if the sound system isn't available.  Check the
 * LE_VOICE_CAPTURE_AVAILABLE and LE_VOICE_PLAYBACK_AVAILABLE flags to see if
 * sound is available.
 */

void LE_VOICE_InitSystem(HWND MainWindow)
{
  typedef HRESULT (WINAPI * PFN_DSCCREATE) (LPGUID lpGUID, LPDIRECTSOUNDCAPTURE *lplpDSC,LPUNKNOWN pUnkOuter);
  typedef BOOL (WINAPI * PFN_DSCENUMERATE) (LPDSENUMCALLBACK lpDSEnumCallback,LPVOID lpContext);

  HRESULT           ErrorCode;
  HINSTANCE         hDSoundLib = NULL;
  PFN_DSCCREATE     pfn_DSCCreate;
  PFN_DSCENUMERATE  pfn_DSCEnumerate;

  LI_VOICE_FREE_MEMORY (TRUE);
  ACM_cleanup(); // In case Init gets called several times

  /* Now create the optional sound capture object.  This needs to be done
     before the main DirectSound object otherwise it reports that the audio
     capture device is in use (unless you have SoundBlaster hardware or hit
     the OK button in the DirectX control panel before running this program).
     Stupid, eh?  Or maybe it can't tell that we are the same process as the
     one which opened DirectSound. */

  if (g_pDSCaptureObject == NULL)
  {
    // See if the DirectSoundCaptureCreate function is available.  We can't
    // have it directly in the code otherwise the program won't run with
    // DirectX3, which doesn't have it.

    hDSoundLib = LoadLibrary("DSOUND.DLL");
    if (hDSoundLib == NULL)
      goto FAILURE_EXIT;

    // Try creating the default sound capture device

    pfn_DSCCreate = (PFN_DSCCREATE)GetProcAddress(hDSoundLib, "DirectSoundCaptureCreate");
    if (pfn_DSCCreate == NULL)
      goto FAILURE_EXIT;

    ErrorCode = (*pfn_DSCCreate)(NULL, &g_pDSCaptureObject, NULL);
    if (FAILED (ErrorCode))
      g_pDSCaptureObject = NULL; /* Just in case. */

    // If the default device doesn't work, find out which devices
    //  are available and try creating each one until one works

    if (g_pDSCaptureObject == NULL)
    {
      pfn_DSCEnumerate = (PFN_DSCENUMERATE)GetProcAddress(hDSoundLib, "DirectSoundCaptureEnumerateA");
      if (pfn_DSCEnumerate == NULL)
        goto FAILURE_EXIT;

      NumberOfCaptureDevices = 0;
      ErrorCode = (*pfn_DSCEnumerate)(LI_VOICE_EnumDSCaptureProc, NULL);
      if (FAILED (ErrorCode))
        goto FAILURE_EXIT;

      for (int i = 0; i < NumberOfCaptureDevices; i++)
      {
        // Try to create a direct sound capture thingy on this device
        ErrorCode = (*pfn_DSCCreate)(CaptureDeviceGUIDs + i, 
                                     &g_pDSCaptureObject, NULL);
        if (FAILED(ErrorCode))
          g_pDSCaptureObject = NULL; // Just in case.
        else // Did it!  Success.
          break;
      }
    }
  }

  // Initialise the voice compression libraries
  LE_VOICE_REGENERATE_COMPRESSOR_LIST();

#if FORHOTSEAT
  /* Create the optional main Direct Sound COM object, if our including
  program has failed to do it (no, don't do that, the parent init stuff
  also does other things that our partial DirectSound init would miss).
  Also use the COM system to create it so that we don't need the
  DirectSound DLL to run (so can run with DirectX 3.0 (no sound) or 5.0). */

  if (g_pDSoundObject == NULL)
  {
    ErrorCode = CoCreateInstance(CLSID_DirectSound, NULL, CLSCTX_INPROC_SERVER,
                                 IID_IDirectSound, (void**)&g_pDSoundObject);

    if (FAILED (ErrorCode) || g_pDSoundObject == NULL)
      goto FAILURE_EXIT;

    // Start it up
    ErrorCode = g_pDSoundObject->Initialize(NULL /* Use default device. */);

    if (FAILED (ErrorCode))
      goto FAILURE_EXIT;

    // Set cooperation level.  Needed or nothing works.
    ErrorCode = g_pDSoundObject->SetCooperativeLevel(MainWindow, DSSCL_NORMAL);

    if (FAILED (ErrorCode))
      goto FAILURE_EXIT;
  }
#endif

  goto NORMAL_EXIT;


FAILURE_EXIT:
  LI_VOICE_ACM_AVAILABLE = FALSE;

NORMAL_EXIT:
  if (hDSoundLib != NULL)
    FreeLibrary (hDSoundLib);

  LE_VOICE_PLAYBACK_AVAILABLE = (g_pDSoundObject != NULL);
  LE_VOICE_CAPTURE_AVAILABLE  = (g_pDSCaptureObject != NULL);
}



/*****************************************************************************
 * Clean up and deallocate things for the voice chat system.  Safe to call
 * multiple times.
 */

void LE_VOICE_RemoveSystem (void)
{
  ACM_cleanup();
  LI_VOICE_FREE_MEMORY(TRUE);

  RELEASEPOINTER(g_pDSCaptureObject);
  LE_VOICE_PLAYBACK_AVAILABLE = FALSE;

  RELEASEPOINTER(g_pDSoundObject);
  LE_VOICE_CAPTURE_AVAILABLE = FALSE;
}
