#ifndef __L_SOUND_H__
#define __L_SOUND_H__

/*****************************************************************************
 * FILE:         L_SOUND.H
 * DESCRIPTION:  Sound system header - public functions and types.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Sound.h 11    26/09/99 5:33p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Sound.h $
 * 
 * 11    26/09/99 5:33p Agmsmith
 * Voice chat system under reconstruction - doesn't compile.
 *
 * 10    16/09/99 4:37p Agmsmith
 * Export internal DirectSound globals for use with Bink video.
 *
 * 9     9/13/99 2:39p Lzou
 * Moved the bit of code responsible for audio data capturing and
 * compression from ChatOn to ChatSend. Implemented ChatSend.
 *
 * 8     7/06/99 2:20p Agmsmith
 * Pseudocode done.
 *
 * 7     6/18/99 4:59p Agmsmith
 * Voice chat under construction.
 *
 * 6     6/18/99 4:25p Agmsmith
 * Added initial streaming voice chat functions.
 *
 * 5     3/25/99 3:57p Agmsmith
 * Added pitch changing function.
 *
 * 4     3/16/99 4:31p Agmsmith
 * Added a function for flushing cached sounds.
 *
 * 3     9/22/98 3:06p Agmsmith
 * Now compiles with ArtLib99.
 *
 * 2     9/22/98 12:32p Agmsmith
 * All prototypes are now C++ declarations by default.
 *
 * 1     9/14/98 12:35p Agmsmith
 * Initial version copied over from ArtLib98 and renamed from .C to .CPP.
 *
 * 17    8/21/98 1:06p Agmsmith
 * Added permanent caching of sounds - so that selected sounds get
 * decompressed and stored in a DirectSound buffer for instant reuse.
 *
 * 16    8/07/98 12:13p Agmsmith
 * Added get global volume function.
 *
 * 15    8/07/98 10:38a Agmsmith
 * Get old global volume function.
 *
 * 14    7/18/98 1:55p Agmsmith
 * Now you can jump around (set trigger points) in non-spooled short
 * sounds too.
 *
 * 13    7/18/98 12:04p Agmsmith
 * Add jump cut to audio streaming.
 *
 * 12    7/18/98 11:19a Agmsmith
 * Added jumps to higher level AudioStream stuff.
 *
 * 11    7/18/98 11:16a Agmsmith
 * Added audio feed cut to option, for quick jumps in audio.
 *
 * 10    7/13/98 11:18a Danf
 * Added prototypes for all old LE_SOUND type calls.
 *
 * 9     7/12/98 3:52p Agmsmith
 * Now does looping of sounds, without a 5 second pause between loops.
 * Only remaining bug is short sounds don't trigger their ending action
 * when they loop.
 *
 * 8     7/12/98 11:40a Agmsmith
 * Now have a separate type of buffer for sounds which fit entirely in it,
 * vs spooled ones.  The position reporting should be better now too.
 *
 * 7     7/08/98 6:44p Agmsmith
 * Added new sound system.
 ****************************************************************************/

/****************************************************************************/
/* D E F I N E S  &  T Y P E S                                              */
/*--------------------------------------------------------------------------*/

// Define a copy of WAVEFORMATEX if it doesn't exist (so you don't
// have to include the whole Windows header file).

#ifndef _INC_MMSYSTEM
typedef struct tWAVEFORMATEX
{
    WORD        wFormatTag;         /* format type */
    WORD        nChannels;          /* number of channels (i.e. mono, stereo...) */
    DWORD       nSamplesPerSec;     /* sample rate */
    DWORD       nAvgBytesPerSec;    /* for buffer estimation */
    WORD        nBlockAlign;        /* block size of data */
    WORD        wBitsPerSample;     /* number of bits per sample of mono data */
    WORD        cbSize;             /* the count in bytes of the size of */
    /* Extra information follows after cbSize, containing cbSize bytes of data */
} WAVEFORMATEX, *PWAVEFORMATEX, NEAR *NPWAVEFORMATEX, FAR *LPWAVEFORMATEX;
#endif



// Sound attribute information.  Just use WAVEFORMATEX on PC.
#if CE_ARTLIB_PlatformWin95
  typedef WAVEFORMATEX LE_SOUND_AttributesRecord, *LE_SOUND_AttributesPointer;
#endif


typedef void *LE_SOUND_BufSndHandle;
  // Used for identifying sounds using the newer BufSnd system of playback.
  // Actually a pointer to an internal data structure.


typedef struct LE_SOUND_AudioStreamStruct
{
  // User changeable fields.
  UNS32 jumpTriggerPosition;// When this position is reached, the sound
  UNS32 jumpToPosition;     // will jump to this new position.
  UNS32 jumpCount;          // Number of times to do the jump.  Zero for off.
  BOOL  jumpCutRequested;   // Set this to TRUE to flush out pending sound when doing a jump.

  // Library use fields - you can read them but don't change them.
  LE_SOUND_BufSndHandle bufSnd; // Spooled sound handle.
  BOOL usingCachedDataID;   // TRUE if playing a cached sound.
  BOOL doNotFeed;           // TRUE if the whole sound fits in the buffer.
  FILE *fileHandle;         // Used if reading from a file, else NULL.
  LE_DATA_DataId waveDataID;// DataID if reading from the data system.
  UNS32 startPosition;      // Byte offset into the file for audio data.
  UNS32 endPosition;        // Byte offset past end of audio data.
  UNS32 filePosition;       // Current seek position for files, to avoid seeks.
  UNS32 currentPosition;    // Currently loading this part of the data.
  UNS32 blockAlignment;     // Audio comes in blocks of this many bytes.
  INT32 duration;           // Number of ticks the whole thing lasts for.
  UNS32 feedSize;           // Maximum amount of data per feeding.
  INT32 playTime;           // Time stamp (0 to duration) of sound you hear now.
} LE_SOUND_AudioStreamRecord, *LE_SOUND_AudioStreamPointer;



/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

extern UNS32  LE_SOUND_CurrentHardwareHz;
extern UNS8   LE_SOUND_CurrentHardwareBits;
extern UNS8   LE_SOUND_CurrentHardwareChannels;
extern BOOL   LE_SOUND_CurrentHardware3D;

extern PDSOUND LI_SOUND_DirectSoundObject;
extern PDSBUFFER LI_SOUND_PrimaryDSoundBuffer;



/****************************************************************************/
/* F U N C T I O N   P R O T O T Y P E S                                    */
/*--------------------------------------------------------------------------*/

// The usual library init stuff.

extern BOOL LI_SOUND_InitSystem (void);
extern void LI_SOUND_RemoveSystem (void);


// Miscellaneous functions.

extern BOOL LE_SOUND_SetGlobalVolume (UNS8 VolumePercentage);
extern UNS8 LE_SOUND_GetGlobalVolume (void);
extern UNS32 LE_SOUND_GetSoundDuration (LE_DATA_DataId DataID);


// Buffered spooled decompressed sound functions.

extern LE_SOUND_BufSndHandle LE_SOUND_StartBufSnd (
  LE_SOUND_AttributesPointer InputAttributesPntr,
  LE_SOUND_AttributesPointer OutputAttributesPntr,
  char *OutputFileName, BOOL Use3DSound,
  LE_DATA_DataId CacheHintDataID, BOOL KeepPermanentlyInCache,
  BOOL *UsingCachedBuffer, BOOL SoundFitsInBuffer,
  int InternalBufferLengthInTicks, int DirectSoundBufferLengthInTicks);

extern BOOL LE_SOUND_StopBufSnd (LE_SOUND_BufSndHandle BufSndHandle);

extern BOOL LE_SOUND_FeedBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  DWORD InputBufferByteSize, BYTE *InputBufferPntr, BOOL *InputUsed,
  DWORD OutputBufferByteSize, BYTE *OutputBufferPntr, DWORD *OutputSizeUsed,
  BOOL *DirectSoundBufferIsFull, INT32 StartTime, INT32 PastEndTime,
  BOOL CutToThisFeedRightNow);

extern INT32 LE_SOUND_GetTimeOfBufSnd (LE_SOUND_BufSndHandle BufSndHandle);

extern LE_SOUND_AttributesPointer LE_SOUND_GetOutputAttributesOfBufSnd (
  LE_SOUND_BufSndHandle BufSndHandle);

extern BOOL LE_SOUND_PauseBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  BOOL PauseIt);

extern BOOL LE_SOUND_LoopNonSpooledBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  BOOL LoopIt);

extern BOOL LE_SOUND_CutToPositionInNonSpooledBufSnd (
  LE_SOUND_BufSndHandle BufSndHandle, UNS32 NewPosition);

extern BOOL LE_SOUND_SetPanBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  INT8 PanPercentage);

extern BOOL LE_SOUND_SetVolumeBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  UNS8 VolumePercentage);

extern BOOL LE_SOUND_SetPitchBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  UNS32 Hertz);

extern BOOL LE_SOUND_Set3DPositionBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  TYPE_Point3DPointer SpatialPosition);

extern BOOL LE_SOUND_CacheBufSnd (LE_DATA_DataId WaveDataID);

extern void LE_SOUND_UncacheBufSnd (LE_DATA_DataId WaveDataID);

extern void LE_SOUND_FlushCachedSounds (void);


// Higher level file parsing and streaming functions.

extern BOOL LE_SOUND_OpenAudioStream (LE_SOUND_AudioStreamPointer AudioPntr,
  char *FileName, LE_DATA_DataId WaveDataID,
  BOOL Use3DSound, BOOL StartInPauseMode, BOOL KeepPermanentlyCached);

extern BOOL LE_SOUND_FeedAudioStream (LE_SOUND_AudioStreamPointer AudioPntr,
  BOOL LoopingRequested);

extern void LE_SOUND_CloseAudioStream (LE_SOUND_AudioStreamPointer AudioPntr);


// Higher level voice chat support.

#if CE_ARTLIB_SoundEnableVoiceChat

typedef BOOL (* LE_SOUND_ChatSendBufferCallbackPointer)
  (void *Buffer, UNS16 BufferSize, BOOL ImportantDataDoNotDiscard);

extern BOOL LI_SOUND_ChatInit (void);
extern void LI_SOUND_ChatRemove (void);

extern BOOL LE_SOUND_ChatOn (
  LE_SOUND_ChatSendBufferCallbackPointer CallbackPntr,
  int PositionDimensionality);

extern BOOL LE_SOUND_ChatOff (void);

extern BOOL LE_SOUND_ChatSend (float X, float Y, float Z);

extern BOOL LE_SOUND_ChatReceive (
  void *Buffer, UNS16 BufferSize, UNS32 PlayerID);

extern BOOL LE_SOUND_ChatCloseAll (void);

#endif // CE_ARTLIB_SoundEnableVoiceChat

/****************************************************************************/

#endif // __L_SOUND_H__
