/*****************************************************************************
 *  FILE:         L_SOUND.CPP
 *  DESCRIPTION:  Main Direct Sound init code and decompression code and
 *                low level spooling functions.  Now with voice chat.
 *
 *  © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Sound.cpp 52    8/10/99 5:21p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Sound.cpp $
 * 
 * 52    8/10/99 5:21p Agmsmith
 * Add option to not initialise the sound recording hardware before the
 * sound playing hardware.  Sometimes need to do record before play,
 * sometimes play before record.
 *
 * 51    5/10/99 7:35p Agmsmith
 * Oops, needed a larger buffer for the compression format.
 *
 * 50    5/10/99 7:25p Agmsmith
 * Now enumerates all codecs to find one with the actual desired data
 * rate.
 *
 * 49    5/10/99 2:39p Agmsmith
 * Nah, default silence level of 0.05 is good enough and don't have to
 * worry about noisy sound cards.
 *
 * 48    5/10/99 2:38p Agmsmith
 * Tuning.  P90 SB16 has low level noise, so set default silence higher.
 *
 * 47    5/10/99 12:50p Agmsmith
 * Tuning of herald and other delays.
 *
 * 46    5/10/99 11:44a Agmsmith
 * Now with collection of chat sound before transmit starts so that it is
 * feeding about 1 second ahead of what the receiver is playing, to
 * compensate for slow game updates.
 *
 * 45    30/09/99 10:54a Agmsmith
 * More fine tuning of voice chat.
 *
 * 44    30/09/99 10:42a Agmsmith
 * Don't start transmitting right after turning on voice chat, wait for
 * some noise first.
 *
 * 43    30/09/99 10:37a Agmsmith
 * Added volume and herald delay features to voice chat.
 *
 * 42    29/09/99 7:26p Agmsmith
 * Now with herald noise, which fixes those buffering problems we were
 * having!
 *
 * 41    29/09/99 6:34p Agmsmith
 * Tuning timing.
 *
 * 40    28/09/99 12:12p Agmsmith
 * Added silence detection.  Sound resume has problems.
 *
 * 39    27/09/99 9:32p Agmsmith
 * Now initially working!  Compresses and decompresses but doesn't detect
 * silence yet.  Also found out about latency problems - use short 1
 * second buffers to avoid large time lags.
 *
 * 38    27/09/99 4:35p Agmsmith
 * Still under construction.
 *
 * 37    27/09/99 11:36a Agmsmith
 * Under construction.
 *
 * 36    26/09/99 8:50p Agmsmith
 * Reconstruction continues.
 *
 * 35    26/09/99 5:32p Agmsmith
 * Voice chat system under reconstruction - doesn't compile.
 *
 * 34    9/24/99 4:57p Lzou
 * Enabled to stop individual chat.
 *
 * 33    9/24/99 3:38p Lzou
 * Got the VoiceChat sort of working. It captures sound data, does
 * necessary audio data precompression, compresses the data to GSM6.10
 * format, and then sends out the data. The ChatReceive () receives the
 * audio data, then decompresses and plays back the sound.
 *
 * 32    16/09/99 4:37p Agmsmith
 * Export internal DirectSound globals for use with Bink video.
 *
 * 31    9/16/99 4:23p Lzou
 * Nothing important.
 *
 * 30    9/13/99 2:38p Lzou
 * Moved the bit of code responsible for audio data capturing and
 * compression from ChatOn to ChatSend. Implemented ChatSend.
 *
 * 29    9/13/99 11:14a Lzou
 * Nothing important.
 *
 * 28    9/09/99 3:19p Lzou
 * Finished the bit of code, which processes captured audio data and
 * converts the data to the specified precompressed waveformat.
 *
 * 27    9/08/99 5:35p Lzou
 * A new way used to process captured audio data and convert the data to
 * the desired precompressed wave format.
 *
 * 26    9/07/99 6:25p Lzou
 * Temporary check in.
 *
 * 25    8/27/99 12:31p Lzou
 * Have implemented audio capture functionality. The capture works pretty
 * well.
 *
 * 24    8/26/99 2:16p Lzou
 * Get the DirectSoundCaptureCreate () to work on my PIII. It is noticed
 * that on some machines the DirectSoundCaptureCreate must be called
 * before the DirectSound stuff.
 *
 * 23    7/06/99 2:20p Agmsmith
 * Pseudocode done.
 *
 * 22    6/30/99 3:51p Agmsmith
 * Found a few other spots where it could crash in single tasking mode
 * when the sound system wasn't open.
 *
 * 21    6/30/99 3:41p Agmsmith
 * User error exit when playing sound with no sound system.  Well,
 * normally it won't get called anyways.
 *
 * 20    6/29/99 15:05 Davew
 * Fixed the crashing problem when another application has hold of the
 * Sound card and DirectSound fails to initialize
 *
 * 19    6/28/99 4:14p Agmsmith
 * Yet more construction.
 *
 * 18    6/28/99 12:05p Agmsmith
 * Under construction.
 *
 * 17    6/18/99 4:59p Agmsmith
 * Voice chat under construction.
 *
 * 16    5/26/99 11:30 Davew
 * Changed all DirectSound references to use the new DirectSound types
 * defined in l_DXTypes.h
 *
 * 15    3/27/99 3:31p Agmsmith
 * Found DirectSound bug with single shot sounds reported as playing in
 * position zero when they really have finished.
 *
 * 14    3/26/99 10:27a Agmsmith
 * Enable pitch changing.
 *
 * 13    3/25/99 3:57p Agmsmith
 * Added pitch changing function.
 *
 * 12    3/19/99 12:54p Agmsmith
 * This time with the right PCMWAVEFORMAT header structure.
 *
 * 11    3/19/99 12:37p Agmsmith
 * Fixed bugs in writing of older waveformat data.
 *
 * 10    3/18/99 3:48p Agmsmith
 * Work with old style WAVEFORMAT header.
 *
 * 9     3/16/99 4:31p Agmsmith
 * Added a function for flushing cached sounds.
 *
 * 8     2/11/99 1:24p Agmsmith
 * Fixed bug where very first reported time position was at the end,
 * rather than the start of the feeding.
 *
 * 7     2/11/99 11:44a Agmsmith
 * Fixed bug with garbage returned time immediately after doing a jump
 * cut.  Still returns too large a value after the very first feeding.
 *
 * 6     12/21/98 4:47p Agmsmith
 * Don't feed when there are no free time stamps.
 *
 * 5     11/17/98 2:47p Agmsmith
 * No critical section in single tasking mode.  Also report buffer play
 * position more reasonably when the buffer has emptied out (to avoid
 * position going backwards then forwards).
 *
 * 4     10/29/98 6:36p Agmsmith
 * New improved memory and data system added.  Memory and data systems
 * have been separated.  Now have memory pools, and optional corruption
 * check.  The data system has data groups rather than files and
 * individual items can come from any data source. Also has a least
 * recently used data unloading system (rather than programmer set
 * priorities).  And it can all be turned off for AndrewX!
 *
 * 3     10/21/98 2:55p Agmsmith
 * Documentation fix - sound position is in user units, not ticks.
 *
 * 2     9/22/98 3:06p Agmsmith
 * Now compiles with ArtLib99.
 *
 * 36    8/31/98 4:11p Agmsmith
 * Looking for the stuck at end of video sound looping bug, but didn't
 * find it yet.  Instead did some small fixes which probably won't change
 * anything.
 *
 * 35    8/21/98 1:06p Agmsmith
 * Added permanent caching of sounds - so that selected sounds get
 * decompressed and stored in a DirectSound buffer for instant reuse.
 *
 * 34    8/07/98 12:12p Agmsmith
 * Added get global volume function, also spooling / nonspooling decision
 * set by user defines.
 *
 * 33    8/04/98 12:09p Agmsmith
 * Need more stamps to match bigger buffers.
 *
 * 32    8/03/98 3:48p Agmsmith
 * Boost sound buffer sizes and number of cached sounds, for slow P90
 * performance problems.
 *
 * 31    7/28/98 2:26p Agmsmith
 * Need a valid pointer for WriteCursor, else DirectX 3 on NT crashes.
 *
 * 30    7/22/98 9:46a Agmsmith
 * Even fewer sound caches.
 *
 * 29    7/18/98 3:32p Agmsmith
 * Now doesn't have a glitch with the play position not advancing just
 * after cutting to a new sound.
 *
 * 28    7/18/98 1:55p Agmsmith
 * Now you can jump around (set trigger points) in non-spooled short
 * sounds too.
 *
 * 27    7/18/98 12:04p Agmsmith
 * Add jump cut to audio streaming and fix duration calculation error.
 *
 * 26    7/18/98 11:16a Agmsmith
 * Added audio feed cut to option, for quick jumps in audio.
 *
 * 25    7/17/98 12:01p Agmsmith
 * Cut down on the number of sound cache buffers, to save memory.
 *
 * 24    7/17/98 11:54a Agmsmith
 * Comment change.
 *
 * 23    7/17/98 11:34a Agmsmith
 * Trying to fix VC 4.2 compiler bug in release mode.
 *
 * 22    7/14/98 3:22p Agmsmith
 * Fixing compiler warnings about unused variables etc.
 *
 * 21    7/12/98 3:52p Agmsmith
 * Now does looping of sounds, without a 5 second pause between loops.
 * Only remaining bug is short sounds don't trigger their ending action
 * when they loop.
 *
 * 20    7/12/98 11:40a Agmsmith
 * Now have a separate type of buffer for sounds which fit entirely in it,
 * vs spooled ones.  The position reporting should be better now too.
 *
 * 19    7/09/98 4:44p Agmsmith
 * Big sounds never got the end detected because of numerical overflow in
 * time calculation.
 *
 * 18    7/09/98 3:59p Agmsmith
 * Ah ha, was running out of time stamps.  Add an error message and more
 * stamps.
 *
 * 17    7/09/98 2:00p Agmsmith
 * Allow for looping.
 *
 * 16    7/08/98 8:24p Agmsmith
 * Fix backwards compatability set volume and pan.
 *
 * 15    7/08/98 6:44p Agmsmith
 * Added new sound system.
 ****************************************************************************/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemSound

/****************************************************************************/
/* E X P O R T E D   G L O B A L   V A R I A B L E S                        */
/*--------------------------------------------------------------------------*/

UNS32  LE_SOUND_CurrentHardwareHz;
UNS8   LE_SOUND_CurrentHardwareBits;
UNS8   LE_SOUND_CurrentHardwareChannels;
BOOL   LE_SOUND_CurrentHardware3D;
// These show the current output hardware settings, which try to match your
// specifications in CE_ARTLIB_SoundHardwareHz etc, but might not be the
// same.  If you want a sound to play with the most efficiency, try to make
// it the same as the hardware (Hz, depth are important).

PDSOUND LI_SOUND_DirectSoundObject = NULL;
  // NULL if system not initialised, otherwise it points to the
  // main (only) DirectSound object.

PDSBUFFER LI_SOUND_PrimaryDSoundBuffer = NULL;
  // This identifies the primary (hardware) buffer, which all the
  // virtual sound buffers feed into.




/****************************************************************************/
/*  P R I V A T E   D E F I N E S   &   T Y P E S                           */
/*--------------------------------------------------------------------------*/

// Maximum number of time stamps used to mark bits of data in the
// DirectSound buffer as it is playing.  So, with 0.5 second feeds
// and 7 seconds of buffer, that's 14 feeds.  Each feed uses 2 stamps,
// which makes 28.  Well, assign 3 stamps to each feeding to be safe.

#if CE_ARTLIB_SoundSpoolBufferDuration > CE_ARTLIB_SoundMaxNonSpooledSound
  #define BUFSND_MAX_STAMPS (CE_ARTLIB_SoundSpoolBufferDuration * 6 / CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ)
#else
  #define BUFSND_MAX_STAMPS (CE_ARTLIB_SoundMaxNonSpooledSound * 6 / CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ)
#endif

#define BUFSND_NO_STAMP INT32_MIN
  // Magic value used to signal that a time stamp isn't valid.



// Timestamp record for marking positions in the sound with a time stamp.

typedef struct StampStruct
{
  INT32 time;
    // The user provided time associated with a particular piece of sound,
    // or BUFSND_NO_STAMP if this entry isn't being used.

  UNS32 position;
    // Position within the sound buffer corresponding to the time stamp,
    // varies from 0 to the direct sound buffer size in bytes minus one.
} StampRecord, *StampPointer;



// BufSnd system sound buffer.  Better spooling support, and decompression,
// and compression, and memory usage than the older variety.

typedef struct BufSndStruct
{
  LE_SOUND_AttributesPointer inputFormat;
    // Describes the input sound format, including compression options.  Since
    // this is a variable length record, it gets allocated when you start a
    // sound and deallocated when the sound is stopped.

  LE_SOUND_AttributesPointer outputFormat;
    // Same thing for the output format.  Usually it will be PCM with the same
    // Hz and other settings as the input, but it can be a compressed format
    // if you are doing sound compression.  Also allocated and deallocated by
    // the sound code.

  HACMSTREAM acmStreamHandle;
    // Identifies the open stream we are using for compression/decompression,
    // NULL if we aren't compressing (straight through copy done instead).

  ACMSTREAMHEADER acmStreamHeader;
    // Contains pointers to the source and destination buffers used by the
    // Audio Compression Manager to decompress/compress sound.  We allocate
    // the buffers and keep their pointers here.  Also their sizes are stored
    // in the dwSrcUser and dwDstUser fields.  Bleeble: in the future, try to
    // get rid of these buffers, perhaps by changing the API a bit (require
    // input in blocks of compressed data) and decompress directly from user's
    // buffer into DirectSound buffer.  Also have the input and output buffers
    // be the same buffer if not compressing.  But first get this simpler
    // method working!

  FILE *outputFile;
    // If the sound is being saved in a file, this will be non-NULL.

  UNS32 amountOfDataWrittenToFile;
    // Number of bytes of data written to the output file.

  DWORD directSoundBufferSize;
    // Number of bytes in the DirectSound buffer, this will usually be the
    // same size as the destination buffer we are using for decompression,
    // but may be larger if you want to avoid gaps in the sound, or smaller
    // if you are using the output buffer to store the whole sound.

  PDSBUFFER directSoundBuffer;
    // Identifies the DirectSound buffer we are dumping data into, or
    // NULL if this is a silent sequence (such as when dumping to a file
    // or compressing voice chat).

  PDS3DBUFFER directSound3DBuffer;
    // Non-NULL if we are using 3D sound, this identifies the same sound
    // buffer as directSoundBuffer (but a different interface), and it
    // has extra 3D control functions.

  DWORD nextDirectSoundWriteOffset;
    // Offset into the DirectSound buffer where we will be writing our
    // next bit of data.  Varies from 0 to directSoundBufferSize - 1.
    // Generally we fill it as full as we can without overrunning the
    // currently playing position.

  DWORD previousDirectSoundPlayPosition;
    // This is the DirectSound play offset from the last update, so we
    // can tell how much stuff has been played since the last time,
    // which we need to know for updating the time stamps.

  StampRecord stamps [BUFSND_MAX_STAMPS];
    // The stamps are in order of increasing position, modulo the output
    // buffer size (the one used by DirectSound).  As time stamps pass by,
    // the whole array is shifted over.

  int numberOfActiveStamps;
    // Number of time stamps in the stamps array, varies from 0 to
    // BUFSND_MAX_STAMPS.

  StampRecord mostRecentlyPlayedStamp;
    // Identifies the time stamp which has been most recently passed,
    // so that you can interpolate between it and the next pending
    // stamp to find out the proportional time.  It usually is valid.

  INT32 veryFirstFeedTime;
  INT32 veryLastFeedTime;
    // These are the time stamps used for the very first and very last
    // feeding, needed by the caching system which doesn't do any feeding
    // at all and just wants to simulate the previous feeding parameters.

  INT32 previousFeedingStartTime;
    // The time stamp of the start of feeding, if the associated data
    // is still in the output buffer.  Gets set to BUFSND_NO_STAMP once
    // the first byte of data moves out of the output buffer and into
    // the sound hardware (then it will be in the stamps array).

  INT32 previousFeedingPastEndTime;
    // Time stamp for the previous feeding, used for detecting jumps
    // in the audio stream, which require resetting the decompressor
    // codec to initial defaults.

  LE_DATA_DataId hintDataID;
    // If this isn't LE_DATA_EmptyItem then it identifies the sound being
    // played.  Useful so that it can be reused if it all fits in the
    // DirectSoundBuffer and that buffer is from the cache - then we
    // don't have to load the buffer or even loop it.

  UNS8 keepPermanentlyInCache;
    // If this sound is cacheable, then when it gets into the cache,
    // it will stay there.  But first it has to get into the cache.

  INT8 cachedDirectSoundBufferIndex;
    // If this is negative then directSoundBuffer points to a uniquely
    // allocated buffer.  If it is zero or greater than it identififes
    // a cached buffer, which we use for small sounds rather than
    // allocating a whole new DirectSoundBuffer, and the buffer
    // already contains the desired sound, and is sized to exactly
    // hold the sound, so you just need to issue the play command
    // to hear it (don't need looping or spooling).

  UNS8 pausedByTheUser;
    // A flag that is TRUE when the sound has been paused by the user,
    // so that we don't accidentally try to start playing it again
    // when restoring lost direct sound buffers or switching looping
    // states.

  UNS8 soundIsNotSpooled;
    // If TRUE then this sound entirely fills the buffer and is played
    // without spooling.  Things like current position determination
    // change (become simpler).  Looping also becomes optional.  All
    // cached sounds have to be non-spooled (since they don't get
    // fed, all the sound has to be kept in the buffer).

  UNS8 userWishesToLoop;
    // If the sound fills the whole buffer (no spooling needed),
    // then looping is optional, set by the user (if the user doesn't
    // want to loop then the sound is played as a one shot sound without
    // DirectSound looping).  Spooled sounds always loop (this flag is
    // ignored), and the feeding controls whether the user hears it
    // looping or hears silence (feed silence) when it goes past the end.

} BufSndRecord, *BufSndPointer;



typedef struct CacheStruct
{
  LE_DATA_DataId dataID;
    // If you are playing the same sound over and over again, the BufSnd
    // system can find the previously used cache buffer and not have to
    // do any loading - just plays it right away.  LE_DATA_EmptyItem if
    // the buffer doesn't exist.

  PDSBUFFER DSBuffer;
    // These DirectSound buffers are saved when a small sound is played,
    // so that the small sound can be quickly repeated without allocating
    // buffers or loading data.  This buffer exists only when the dataID
    // isn't LE_DATA_EmptyItem.

  UNS32 bufferSize;
    // Number of bytes in the DSBuffer.

  INT32 firstFeedTime;
  INT32 lastFeedTime;
    // Time stamps the user provided for the start and end of the buffer,
    // which we will reuse when replaying this cached buffer (since the
    // user only feeds it the very first time, before it becomes cached).

  BOOL inUse;
    // TRUE if some BufSnd is using this one.  FALSE if available.

  BOOL permanentlyCached;
    // TRUE if this sound has been permanently cached.  That means it never
    // gets de-cached.  Useful for special sounds like the wheel of fortune
    // spinning sound, which need to be started instantaneously (no time
    // to load and decompress, and it is large so it would take a lot of
    // CPU to decompress it).

  UNS16 idleCount;
    // Counts up every time this cache entry gets passed over because
    // its dataID isn't empty yet it is unused.  Eventually it gets
    // noticed as the least recently used one and gets reused (buffer
    // deallocated and replaced by another sound).

  UNS16 reusedSameSoundCount;
    // Number of times this sound was reused, for statistical use.

  UNS16 numberOfUniqueSounds;
    // Number of different sounds that this buffer has played, for
    // statistical use.

  UNS32 totalReusedCount;
    // Every time a sound gets replaced by another one, the reused
    // count gets added to this total, so that we can find the
    // average number of times a cache entry gets reused successfully.

} CacheRecord, *CachePointer;
  // Needed because allocating and deallocating buffers for small sounds
  // just takes too much CPU time, particularly for things like eye blink
  // noises that are 0.15 seconds long.  Not to mention load time too.
  // Well, actually it isn't that bad, the problem was an old feed bug
  // that made it seem like it took a long time.  Still, I'll keep the
  // cache stuff.



/****************************************************************************/
/*  P R I V A T E   G L O B A L S                                           */
/*--------------------------------------------------------------------------*/

static short const nLogVolume[101] =
{
  -10000, -3986, -3386, -3035, -2786, -2593, -2435, -2301, -2186, -2084,
   -1993, -1910, -1835, -1766, -1701, -1642, -1586, -1533, -1484, -1437,
   -1393, -1350, -1310, -1272, -1235, -1200, -1166, -1133, -1101, -1071,
   -1042, -1013,  -986,  -959,  -933,  -908,  -884,  -860,  -837,  -815,
    -793,  -771,  -750,  -730,  -710,  -691,  -672,  -653,  -635,  -617,
    -600,  -582,  -566,  -549,  -533,  -517,  -501,  -486,  -471,  -456,
    -442,  -427,  -413,  -399,  -386,  -372,  -359,  -346,  -333,  -321,
    -308,  -296,  -284,  -272,  -260,  -249,  -237,  -226,  -215,  -204,
    -193,  -182,  -171,  -161,  -150,  -140,  -130,  -120,  -110,  -100,
     -91,   -81,   -72,   -62,   -53,   -44,   -35,   -26,   -17,    -8,
       0
};


#if CE_ARTLIB_EnableMultitasking
static CRITICAL_SECTION SoundCriticalSection;
static LPCRITICAL_SECTION volatile SoundCSPntr;
  // NULL or it points to the SoundCriticalSection.
  // Please use SoundCSPntr rather than directly using SoundCriticalSection.
  // Guards the sound code against multitasking confusion.
#endif


#if CE_ARTLIB_SoundMaxCachedDirectSounds > 0
  static CacheRecord CacheArray [CE_ARTLIB_SoundMaxCachedDirectSounds];
    // Cached pre-made DirectSound buffers for small repetative sounds,
    // they stay in memory after the sound is finished in case you need
    // the same sound again.
#endif // CE_ARTLIB_SoundMaxCachedDirectSounds > 0



/*****************************************************************************
 * Initializes the DirectSound interface for playing wave files.  Returns
 * TRUE if successful.  Safe to call multiple times.
 */

BOOL LI_SOUND_InitSystem (void)
{
  DSCAPS        DirectSoundCapabilities;
  HRESULT       DSErrorCode;
  int           i;
  WAVEFORMATEX  MyWaveFormat;
  DSBUFFERDESC  PrimaryBufferDescription;

  if (LI_SOUND_DirectSoundObject != NULL)
    return TRUE; // Already initialised.

#if CE_ARTLIB_SoundEnableVoiceChat
  // It is noticed that on some machines the DirectSoundCaptureCreate()
  // must be called before the DirectSound stuff. This seems machine
  // dependent, not OS dependent.
  LI_SOUND_ChatInit (); // Don't care if voice chat fails.
#endif

  // Create direct sound object.  A non-null pointer means that the system
  // is at least partially initialised, so this comes first.

  DSErrorCode = DirectSoundCreate (NULL, &LI_SOUND_DirectSoundObject, NULL);
  if (DSErrorCode != DS_OK) goto ErrorExit;

  // Create sound buffer sharing mutex, actually a critical section is now
  // used since it is more efficient.  Needed to prevent two separate threads
  // from interfering while accessing global data (like the cache list).

#if CE_ARTLIB_EnableMultitasking
  SoundCSPntr = &SoundCriticalSection;
  InitializeCriticalSection (SoundCSPntr);
#endif

  // set cooperation level to "priority" - we want to set the
  // hardware playback format so that we can do 16 bit sound etc.
  DSErrorCode = LI_SOUND_DirectSoundObject->SetCooperativeLevel (
    LE_MAIN_HwndMainWindow, DSSCL_PRIORITY);
  if (DSErrorCode != DS_OK) goto ErrorExit;

  // Read capabilities so we can set the primary buffer better.
  memset (&DirectSoundCapabilities, 0, sizeof (DirectSoundCapabilities));
  DirectSoundCapabilities.dwSize = sizeof (DSCAPS);
  DSErrorCode = LI_SOUND_DirectSoundObject->GetCaps (
    &DirectSoundCapabilities);
  if (DSErrorCode != DS_OK) goto ErrorExit;

  // Make a primary buffer description.  Need volume control (for global
  // volume), and optionally 3D sound.
  memset (&PrimaryBufferDescription, 0, sizeof (DSBUFFERDESC));
  PrimaryBufferDescription.dwSize = sizeof (DSBUFFERDESC);
  PrimaryBufferDescription.dwFlags =
    DSBCAPS_PRIMARYBUFFER |
    DSBCAPS_CTRLVOLUME |
    (CE_ARTLIB_SoundEnable3DSound ? DSBCAPS_CTRL3D : 0);

  // create primary buffer
  DSErrorCode = LI_SOUND_DirectSoundObject->CreateSoundBuffer (
    &PrimaryBufferDescription, &LI_SOUND_PrimaryDSoundBuffer, NULL);
  if (DSErrorCode != DS_OK) goto ErrorExit;

  // Try changing its wave format to match what we want.
  // First see what the current settings are, then modify them.
  DSErrorCode = LI_SOUND_PrimaryDSoundBuffer->GetFormat (
    &MyWaveFormat, sizeof (MyWaveFormat), NULL);
  if (DSErrorCode != DS_OK) goto ErrorExit;

  // Override the default number of channels if the hardware supports it.
  if (CE_ARTLIB_SoundHardwareChannels <= 1 &&
  (DirectSoundCapabilities.dwFlags & DSCAPS_PRIMARYMONO))
    MyWaveFormat.nChannels = 1;
  else if (CE_ARTLIB_SoundHardwareChannels == 2 &&
  (DirectSoundCapabilities.dwFlags & DSCAPS_PRIMARYSTEREO))
    MyWaveFormat.nChannels = 2;

  // Nothing to validate the playback Hz against.  Just set it.
  MyWaveFormat.nSamplesPerSec = CE_ARTLIB_SoundHardwareHz;

  // Override the default bits by the user's request, if legal.
  if (CE_ARTLIB_SoundHardwareBits <= 8 &&
  (DirectSoundCapabilities.dwFlags & DSCAPS_PRIMARY8BIT))
    MyWaveFormat.wBitsPerSample = 8;
  else if (CE_ARTLIB_SoundHardwareBits >= 16 &&
  (DirectSoundCapabilities.dwFlags & DSCAPS_PRIMARY16BIT))
    MyWaveFormat.wBitsPerSample = 16;

  // Calculate a few derived fields.
  MyWaveFormat.nBlockAlign = (WORD)
    (MyWaveFormat.nChannels * (MyWaveFormat.wBitsPerSample / 8));
  MyWaveFormat.nAvgBytesPerSec =
    MyWaveFormat.nSamplesPerSec * MyWaveFormat.nBlockAlign;
  MyWaveFormat.cbSize = 0;

  // set format of primary buffer.  Don't care if it fails.
  DSErrorCode = LI_SOUND_PrimaryDSoundBuffer->SetFormat (
    &MyWaveFormat);

  // See how much of the format change was accepted.
  DSErrorCode = LI_SOUND_PrimaryDSoundBuffer->GetFormat (
    &MyWaveFormat, sizeof (MyWaveFormat), NULL);
  if (DSErrorCode != DS_OK) goto ErrorExit;

  // Set format again so that there isn't an emulated mixing stage
  // between the hardware and the secondary buffers.
  DSErrorCode = LI_SOUND_PrimaryDSoundBuffer->SetFormat (
    &MyWaveFormat);

  // Get the final format.
  DSErrorCode = LI_SOUND_PrimaryDSoundBuffer->GetFormat (
    &MyWaveFormat, sizeof (MyWaveFormat), NULL);
  if (DSErrorCode != DS_OK) goto ErrorExit;

  LE_SOUND_CurrentHardwareHz = MyWaveFormat.nSamplesPerSec;
  LE_SOUND_CurrentHardwareBits = (UNS8) MyWaveFormat.wBitsPerSample;
  LE_SOUND_CurrentHardwareChannels = (UNS8) MyWaveFormat.nChannels;
  LE_SOUND_CurrentHardware3D = CE_ARTLIB_SoundEnable3DSound;

  // Start the primary buffer playing, this means it will play silence
  // if there is no mixed data, keeping it running continuously to avoid
  // pops and other noises from starting and stopping?  Otherwise it would
  // stop and start as needed.

  DSErrorCode = LI_SOUND_PrimaryDSoundBuffer->Play (
    0, 0, DSBPLAY_LOOPING);
  if (DSErrorCode != DS_OK) goto ErrorExit;

  // Set up the cached buffers array.

#if CE_ARTLIB_SoundMaxCachedDirectSounds > 0
  for (i = 0; i < CE_ARTLIB_SoundMaxCachedDirectSounds; i++)
  {
    CacheArray[i].dataID = LE_DATA_EmptyItem;
    CacheArray[i].inUse = FALSE;
    CacheArray[i].permanentlyCached = FALSE;
    CacheArray[i].numberOfUniqueSounds = 0;
    CacheArray[i].totalReusedCount = 0;
  }
#endif // CE_ARTLIB_SoundMaxCachedDirectSounds > 0

#if CE_ARTLIB_SoundEnableVoiceChat
  // As mentioned above that the order matters on some machines,
  // if the first DirectSoundCaptureCreate call fails, we do it
  // here again.
  LI_SOUND_ChatInit (); // Don't care if voice chat fails.
#endif

  return TRUE;


ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LI_SOUND_InitSystem: "
    "Unable to initialise.  DirectSound error $%08X.\r\n", (int) DSErrorCode);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

  LI_SOUND_RemoveSystem ();
  return FALSE;
}



/*****************************************************************************
 * Shut down the DirectSound interface.  Presumably users of the buffered
 * sounds have shut down the individual sounds on their own before this.
 */

void LI_SOUND_RemoveSystem (void)
{
  CachePointer  CachePntr;
  int           CacheIndex;

  if (LI_SOUND_DirectSoundObject == NULL)
    return; // It is already removed.

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr != NULL)
    EnterCriticalSection (SoundCSPntr);
#endif

#if CE_ARTLIB_SoundEnableVoiceChat
  LI_SOUND_ChatRemove ();
#endif

  // Stop and deallocate all cached buffers.  Warn about ones that are
  // still in use - they shouldn't be.

#if CE_ARTLIB_SoundMaxCachedDirectSounds > 0
  for (CacheIndex = 0, CachePntr = CacheArray + 0;
  CacheIndex < CE_ARTLIB_SoundMaxCachedDirectSounds;
  CacheIndex++, CachePntr++)
  {
    if (CachePntr->DSBuffer != NULL)
    {
      CachePntr->DSBuffer->Stop ();
      CachePntr->DSBuffer->Release ();
      CachePntr->DSBuffer = NULL;
    }

    if (CachePntr->dataID != LE_DATA_EmptyItem)
    {
#if CE_ARTLIB_EnableDebugMode
      if (CachePntr->inUse)
      {
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_SOUND_RemoveSystem: "
          "Cache buffer for sound data ID $%08X is still in use.  "
          "It shouldn't be!\r\n", (int) CachePntr->dataID);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      }
#endif

      // Print some statistics on which ones are in the cache at exit.

#if CE_ARTLIB_EnableDebugMode
      // Include the sound which currently in the cache in the totals.
      CachePntr->totalReusedCount += CachePntr->reusedSameSoundCount;

      sprintf (LE_ERROR_DebugMessageBuffer, "LI_SOUND_RemoveSystem: Cache buffer #%02d "
        "has DataID $%08X, size: %d, unique sounds: %d, current reused count: %d, average reused: %f, permanent: %s.\r\n", (int) CacheIndex,
        (int) CachePntr->dataID, (int) CachePntr->bufferSize,
        (int) CachePntr->numberOfUniqueSounds,
        (int) CachePntr->reusedSameSoundCount,
        (CachePntr->numberOfUniqueSounds > 0) ? CachePntr->totalReusedCount / (double) CachePntr->numberOfUniqueSounds : 0.0,
        CachePntr->permanentlyCached ? "yes" : "no");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif

      CachePntr->dataID = LE_DATA_EmptyItem;
    }
  }
#endif // CE_ARTLIB_SoundMaxCachedDirectSounds > 0

  // Stop the primary buffer and deallocate it.
  if (LI_SOUND_PrimaryDSoundBuffer != NULL)
  {
    LI_SOUND_PrimaryDSoundBuffer->Stop ();

    // release primary buffer
    LI_SOUND_PrimaryDSoundBuffer->Release();
    LI_SOUND_PrimaryDSoundBuffer = NULL;
  }

  // Release direct sound object.
  if (LI_SOUND_DirectSoundObject != NULL)
  {
    LI_SOUND_DirectSoundObject->Release ();
    LI_SOUND_DirectSoundObject = NULL;
  }

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr != NULL)
  {
    // Make the global NULL before deleting, so that other threads
    // which will come out of EnterCriticalSection when it is
    // deleted, will see the NULL pointer and know that things
    // are shut down.

    SoundCSPntr = NULL;
    DeleteCriticalSection (&SoundCriticalSection);
  }
#endif // CE_ARTLIB_EnableMultitasking
}



/*****************************************************************************
 * Set the volume of all sounds playing to the given percentage level.
 * 100 is full on, 0 is quiet.  Works by changing the audio mixer hardware
 * Wave setting (as contrasted to MIDI, Line-In and CD Audio).  Returns TRUE
 * if successful.  Remember to set it back to the original value when
 * you exit the program, otherwise the user's wave balance will be left
 * changed without the user knowing.
 */

BOOL LE_SOUND_SetGlobalVolume (UNS8 VolumePercentage)
{
  HRESULT DSErrorCode;

  if (LI_SOUND_DirectSoundObject == NULL)
    return FALSE; // System is closed.

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  if (VolumePercentage > 100)
    VolumePercentage = 100;

  DSErrorCode = LI_SOUND_PrimaryDSoundBuffer->SetVolume (
    nLogVolume [VolumePercentage]);

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif

  if (DSErrorCode != DS_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_SetGlobalVolume: "
      "DirectSound error $%08X.\r\n", (int) DSErrorCode);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  return TRUE;
}



/*****************************************************************************
 * Returns the current sound setting.  Useful for getting the original volume
 * when the game starts (set by the Windows Wave Balance settings in the
 * mixer control panel) so you don't accidentally set the user's volume to
 * 100% and blast their ears off.
 */

UNS8 LE_SOUND_GetGlobalVolume (void)
{
  HRESULT DSErrorCode;
  UNS8    Percentage;
  LONG    VolumeDecibels;

  if (LI_SOUND_DirectSoundObject == NULL)
    return 0; // System is closed.

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return 0; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return 0; // Closed while we were waiting.
#endif

  DSErrorCode = LI_SOUND_PrimaryDSoundBuffer->GetVolume (
    &VolumeDecibels);

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif

  if (DSErrorCode != DS_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_GetGlobalVolume: "
      "DirectSound error $%08X.\r\n", (int) DSErrorCode);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return 0;
  }

  // Could do a binary search, or calculate the logarithm, but
  // this isn't called too often so a linear search should be ok.

  for (Percentage = 100; Percentage > 0; Percentage--)
    if (VolumeDecibels >= nLogVolume [Percentage])
      return Percentage;

  return Percentage;
}



/*****************************************************************************
 * Find the duration of a WAV file in ticks (CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ
 * of them happen per second).  Returns zero if it fails.  Rounds up.
 */

UNS32 LE_SOUND_GetSoundDuration (LE_DATA_DataId DataID)
{
  DWORD           ChunkName;
  DWORD           ChunkSize;
  DWORD           DataChunkLength = 0;
  UNS32           Duration;
  DWORD          *DWORDPntr;
  DWORD          *EndPntr;
  LPWAVEFORMATEX  WaveFormatPntr = NULL;
  DWORD           RIFFLength;

  // get pointer to .WAV file
  DWORDPntr = (LPDWORD) LE_DATA_Use (DataID);
  if (DWORDPntr == NULL)
    return 0; // Unable to load it.

  if (*DWORDPntr++ != MAKEFOURCC ('R', 'I', 'F', 'F'))
    return 0; // Not a RIFF file.

  RIFFLength = *DWORDPntr++;
  EndPntr = (DWORD *) (((BYTE *) DWORDPntr) + RIFFLength);

  if (*DWORDPntr++ != MAKEFOURCC ('W', 'A', 'V', 'E'))
    return 0; // Not a RIFF/WAVE type of file.

  // Hunt for the "fmt " and "data" subchunks.

  while (DWORDPntr < EndPntr)
  {
    ChunkName = *DWORDPntr++;
    ChunkSize = *DWORDPntr++;

    if (ChunkName == MAKEFOURCC ('f', 'm', 't', ' '))
      WaveFormatPntr = (LPWAVEFORMATEX) DWORDPntr;

    if (ChunkName == MAKEFOURCC ('d', 'a', 't', 'a'))
      DataChunkLength = ChunkSize;

    if (ChunkSize & 1)
      ChunkSize++; // Account for pad byte at end of odd length chunks.

    DWORDPntr = (DWORD *) (((BYTE *) DWORDPntr) + ChunkSize);
  }

  if (DataChunkLength == 0 || WaveFormatPntr == NULL)
    return 0; // Didn't find the chunks we need.

  Duration = (UNS32) ((double) DataChunkLength *
    CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ / WaveFormatPntr->nAvgBytesPerSec);
  if (Duration == 0)
    Duration = 1; // Avoid zero length duration.

  return Duration;
}



/*****************************************************************************
 * Allocate buffers and otherwise get ready to play a spooled sound.
 *
 * The input data characteristics (data rate, stereo/mono, and extra
 * compression codec specific data like MPEG's 50 bytes of extra data) are in
 * the InputAttributesPntr structure (really a WAVEFORMATEX under Windows).
 * After calling this function, you can deallocate the attributes (it makes
 * a copy internally).
 *
 * The format you want to hear the sound in is in OutputAttributesPntr -
 * usually this will be PCM (uncompressed) sound with the same Hz and other
 * settings as the input.  But this could also be used for sound compression.
 * If you specify NULL, then it will use the uncompressed equivalent of the
 * input format.  Makes an internal copy so that you can deallocate yours.
 *
 * Set OutputFileName to the name of a WAVE file if you want to save the
 * output sound.  Specify NULL if you aren't saving it.
 *
 * Set Use3DSound to TRUE if you want to position the sound in 3D space,
 * use FALSE for ordinary sound where you manually set the panning.
 *
 * Set CacheHintDataID to LE_DATA_EmptyItem unless you want this buffer to
 * be eligible for caching - keep in mind that the whole sound has to fit
 * in the DirectSound buffer for caching to work.  Then the next time you
 * play the same sound, it might use a cached DirectSound buffer to play it.
 *
 * If KeepPermanentlyInCache is TRUE then the sound will be kept permanently
 * in the cache if it is elegible for caching and there are free entries
 * in the cache.  This is useful for sounds you want to keep available,
 * but it does keep the DirectSound buffer allocated, which takes up
 * memory space.
 *
 * If UsingCachedBuffer isn't NULL then it will be set to TRUE if a cached
 * buffer was found (you shouldn't feed the sound in that case, just
 * unpause to start it).
 *
 * If SoundFitsInBuffer is TRUE then the DirectSound buffer will not run in
 * looping mode, it will play once and then stop (unless you really want it
 * to loop).  Spooling sounds should have this set to FALSE.  The current
 * play position calculation is also different for one shot sounds.  The
 * whole sound must be less than or equal to the DirectSound buffer length.
 *
 * This function allocates a pair of internal sound buffers (one for compressed
 * and the other for decompressed sound) which each can hold at most
 * InternalBufferLengthInTicks (usually 60 ticks per second - see
 * CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ) time units of sound.  Specify a value
 * here which corresponds to the amount of data you are feeding in at each
 * call LE_SOUND_FeedBufSnd, plus a few percent extra margin for decompression
 * size variance.
 *
 * DirectSoundBufferLengthInTicks specifies the size of the DirectSound buffer
 * to use, or zero if you don't want to hear the sound.  This should be the
 * worst case amount of time between calls to LE_SOUND_FeedBufSnd.  Note that
 * direct sound allocates its own memory for this buffer, in addition to the
 * internal compression buffers.  If it is smaller than the internal buffer
 * size then the values returned by LE_SOUND_GetTimeOfBufSnd will be incorrect
 * (repeatedly looping around only part of the fed range, then jumping to the
 * next fed range).
 *
 * Returns a handle (actually a pointer to internal data structures)
 * that identifies the sound.  Returns NULL if it fails.
 */

LE_SOUND_BufSndHandle LE_SOUND_StartBufSnd (
  LE_SOUND_AttributesPointer InputAttributesPntr,
  LE_SOUND_AttributesPointer OutputAttributesPntr,
  char *OutputFileName, BOOL Use3DSound,
  LE_DATA_DataId CacheHintDataID, BOOL KeepPermanentlyInCache, BOOL *UsingCachedBuffer,
  BOOL SoundFitsInBuffer,
  int InternalBufferLengthInTicks, int DirectSoundBufferLengthInTicks)
{
  MMRESULT                ACMErrorCode;
  BOOL                    BuffersNeeded;
  BufSndPointer           BufSndPntr = NULL;
  int                     BufSndRecordSize;
  BYTE                   *BytePntr;
  int                     CacheIndex;
  CachePointer            CachePntr = NULL;
  BOOL                    CompressionNeeded;
  DSBUFFERDESC            DSBufferDescription;
  HRESULT                 DSErrorCode;
  int                     i;
  int                     InputAttrSize;
  int                     InputBufferSize;
  DWORD                   MyDWORD;
  int                     OutputAttrSize;
  int                     OutputBufferSize;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return NULL; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return NULL; // Closed while we were waiting.
#endif

  // If DirectSound is not initialized, we won't be able to play the sound
  // NOTE:  This is very likely if the sound card was grabbed by some other
  //        application.  This only gets executed in single tasking mode.

  if (!LI_SOUND_DirectSoundObject)
    goto ErrorExit;

  if (InputAttributesPntr == NULL || OutputAttributesPntr == NULL)
    goto ErrorExit;

  InputAttrSize = sizeof (LE_SOUND_AttributesRecord) + InputAttributesPntr->cbSize;
  OutputAttrSize = sizeof (LE_SOUND_AttributesRecord) + OutputAttributesPntr->cbSize;

  CompressionNeeded = (InputAttrSize != OutputAttrSize ||
    memcmp (InputAttributesPntr, OutputAttributesPntr, InputAttrSize) != 0);

  BuffersNeeded = TRUE;

  // See if this sound is a small sound which we have already cached a
  // copy of in one of the cached DirectSound buffers.

  CacheIndex = -1; // Negative means no cached buffer available.

#if CE_ARTLIB_SoundMaxCachedDirectSounds > 0
  if (CacheHintDataID != LE_DATA_EmptyItem)
  {
    for (CacheIndex = 0, CachePntr = CacheArray + 0;
    CacheIndex < CE_ARTLIB_SoundMaxCachedDirectSounds;
    CacheIndex++, CachePntr++)
    {
      if (CachePntr->dataID == CacheHintDataID && !CachePntr->inUse)
        break;
    }

    if (CacheIndex < CE_ARTLIB_SoundMaxCachedDirectSounds)
    {
      // Found it!  No need to feed data or do decompression, the
      // whole sound is already in the DirectSound buffer.

      CompressionNeeded = FALSE;
      BuffersNeeded = FALSE;
      CachePntr->inUse = TRUE;
      if (KeepPermanentlyInCache) // Upgrade this cache to permanent.
        CachePntr->permanentlyCached = TRUE;
      CachePntr->reusedSameSoundCount++;
    }
    else // Can't use a cached buffer, need to make a new one.
      CacheIndex = -1;
  }
#endif // CE_ARTLIB_SoundMaxCachedDirectSounds > 0

  InputBufferSize = InputAttributesPntr->nAvgBytesPerSec *
    InternalBufferLengthInTicks / CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ;
  InputBufferSize = ((InputBufferSize + InputAttributesPntr->nBlockAlign - 1) /
    InputAttributesPntr->nBlockAlign) * InputAttributesPntr->nBlockAlign;

  OutputBufferSize = OutputAttributesPntr->nAvgBytesPerSec *
    InternalBufferLengthInTicks / CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ;
  OutputBufferSize = ((OutputBufferSize + OutputAttributesPntr->nBlockAlign - 1) /
    OutputAttributesPntr->nBlockAlign) * OutputAttributesPntr->nBlockAlign;

  if (!BuffersNeeded)
    InputBufferSize = OutputBufferSize = 0;

  BufSndRecordSize =
    sizeof (BufSndRecord) +
    InputAttrSize + OutputAttrSize +
    (CompressionNeeded ? InputBufferSize : 0) + OutputBufferSize;

  BufSndPntr = (BufSndPointer) LI_MEMORY_AllocLibraryBlock (BufSndRecordSize);
  if (BufSndPntr == NULL)
    goto ErrorExit;

  // Zero out only our header part, the rest get completely filled later.

  memset (BufSndPntr, 0, sizeof (BufSndRecord));

  BufSndPntr->cachedDirectSoundBufferIndex = (INT8) CacheIndex;
  BufSndPntr->keepPermanentlyInCache = KeepPermanentlyInCache;
  BufSndPntr->hintDataID = CacheHintDataID;

  if (CacheIndex >= 0)
    BufSndPntr->soundIsNotSpooled = TRUE; // All cached sounds are non-spooled.
  else
    BufSndPntr->soundIsNotSpooled = (UNS8) SoundFitsInBuffer;

  // The copy of the input and output attribute records follow the
  // fixed size BufSnd record in the same memory hunk.  Then the
  // input and output buffers follow that.  Neat, eh?  Everything
  // in one memory allocation.

  BytePntr = (LPBYTE) BufSndPntr;
  BytePntr += sizeof (BufSndRecord);
  BufSndPntr->inputFormat = (LPWAVEFORMATEX) BytePntr;
  BytePntr += InputAttrSize;
  memcpy (BufSndPntr->inputFormat, InputAttributesPntr, InputAttrSize);
  BufSndPntr->outputFormat = (LPWAVEFORMATEX) BytePntr;
  BytePntr += OutputAttrSize;
  memcpy (BufSndPntr->outputFormat, OutputAttributesPntr, OutputAttrSize);
  BufSndPntr->acmStreamHeader.cbStruct = sizeof (ACMSTREAMHEADER);
  BufSndPntr->acmStreamHeader.pbSrc = BytePntr; // Our input buffer.
  if (CompressionNeeded)         // Use same memory for output as input when
    BytePntr += InputBufferSize; // not compressing.
  BufSndPntr->acmStreamHeader.dwSrcUser = InputBufferSize;
  BufSndPntr->acmStreamHeader.pbDst = BytePntr; // Our output buffer.
  BytePntr += OutputBufferSize;
  BufSndPntr->acmStreamHeader.dwDstUser = OutputBufferSize;

  // Try opening the ACM compressor / decompressor, if needed.

  if (!CompressionNeeded)
  {
    // No compression needed.
    BufSndPntr->acmStreamHandle = NULL;
  }
  else // Get the audio compression manager warmed up...
  {
    ACMErrorCode = acmStreamOpen (&BufSndPntr->acmStreamHandle,
      NULL /* No specific driver */,
      BufSndPntr->inputFormat, BufSndPntr->outputFormat,
      NULL /* No filtering operation */,
      0 /* Callback for async operations */, 0 /* Callback args */,
      (DirectSoundBufferLengthInTicks > 0) ? 0 : ACM_STREAMOPENF_NONREALTIME);
    if (ACMErrorCode != 0)
      goto ErrorExit;

    // Prepare the ACM header for operations.  Basically sets up buffer sizes.

    BufSndPntr->acmStreamHeader.cbSrcLength = BufSndPntr->acmStreamHeader.dwSrcUser;
    BufSndPntr->acmStreamHeader.cbDstLength = BufSndPntr->acmStreamHeader.dwDstUser;

    ACMErrorCode = acmStreamPrepareHeader (BufSndPntr->acmStreamHandle,
      &BufSndPntr->acmStreamHeader, 0);
    if (ACMErrorCode != 0)
      goto ErrorExit;
  }

  // Inbetween decompression operations, the amount of data in the source
  // and destination buffers is given by cbSrcLength and cbDstLength.
  // Also, the destination data starts at byte offset cbDstLengthUsed
  // from the start of the destination buffer (so that we don't have
  // to shift it over in the buffer).

  BufSndPntr->acmStreamHeader.cbSrcLength = 0;
  BufSndPntr->acmStreamHeader.cbDstLength = 0;
  BufSndPntr->acmStreamHeader.cbDstLengthUsed = 0;

  // Open an output file if the sound is being saved.  Write the
  // Microsoft WAVE format header too.  Don't try to do it if
  // there aren't any buffers.

  if (OutputFileName != NULL && BuffersNeeded)
  {
    BufSndPntr->outputFile = fopen (OutputFileName, "wb");
    if (BufSndPntr->outputFile == NULL)
      goto ErrorExit;
    if (fwrite ("RIFFsizeWAVEfmt ", 16, 1, BufSndPntr->outputFile) != 1) goto ErrorExit;
    if (BufSndPntr->outputFormat->cbSize == 0 &&
    BufSndPntr->outputFormat->wFormatTag == WAVE_FORMAT_PCM)
      MyDWORD = sizeof (PCMWAVEFORMAT); // Use older simpler format if possible.
    else
      MyDWORD = OutputAttrSize;
    if (fwrite (&MyDWORD, 4, 1, BufSndPntr->outputFile) != 1) goto ErrorExit;
    if (fwrite (BufSndPntr->outputFormat, MyDWORD, 1, BufSndPntr->outputFile) != 1) goto ErrorExit;
    if (fwrite ("datasize", 8, 1, BufSndPntr->outputFile) != 1) goto ErrorExit;
  }

  // Create an output DirectSound buffer if that mode of output is
  // being used, or just reuse the cached buffer if we found one.

#if CE_ARTLIB_SoundMaxCachedDirectSounds > 0
  if (CacheIndex >= 0)
  {
    BufSndPntr->directSoundBufferSize = CachePntr->bufferSize;
    BufSndPntr->directSoundBuffer = CachePntr->DSBuffer;
  }
  else
#endif // CE_ARTLIB_SoundMaxCachedDirectSounds > 0
  {
    if (DirectSoundBufferLengthInTicks > 0) // If user wants audible sound.
    {
      // Have to create a custom DirectSound buffer.

      memset (&DSBufferDescription, 0, sizeof (DSBUFFERDESC));
      DSBufferDescription.dwSize = sizeof (DSBUFFERDESC);
      DSBufferDescription.dwFlags =
        DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
        DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
      if (Use3DSound)
        DSBufferDescription.dwFlags |= DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE;
      else // Just ordinary 0D/2D sound.
        DSBufferDescription.dwFlags |= DSBCAPS_CTRLPAN;

      BufSndPntr->directSoundBufferSize = OutputAttributesPntr->nAvgBytesPerSec *
        DirectSoundBufferLengthInTicks / CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ;
      BufSndPntr->directSoundBufferSize =
        ((BufSndPntr->directSoundBufferSize + OutputAttributesPntr->nBlockAlign - 1) /
        OutputAttributesPntr->nBlockAlign) * OutputAttributesPntr->nBlockAlign;

      DSBufferDescription.dwBufferBytes = BufSndPntr->directSoundBufferSize;
      DSBufferDescription.lpwfxFormat = BufSndPntr->outputFormat;

      DSErrorCode = LI_SOUND_DirectSoundObject->CreateSoundBuffer (
        &DSBufferDescription, &BufSndPntr->directSoundBuffer, NULL);

      if (DSErrorCode != DS_OK)
        goto ErrorExit;
    }
  }

  // Also get a 3D interface to the sound if it is in 3D.

  if (Use3DSound && BufSndPntr->directSoundBuffer != NULL)
  {
    DSErrorCode = BufSndPntr->directSoundBuffer->QueryInterface (
      IID_IDirectSound3DBuffer, (void **) &BufSndPntr->directSound3DBuffer);
    if (DSErrorCode != DS_OK)
      goto ErrorExit;
  }

  // Set up the time stamps to have no stamps.  Of course, for cached
  // buffers time stamps aren't used so this doesn't matter.

  BufSndPntr->previousFeedingStartTime = BUFSND_NO_STAMP;
  BufSndPntr->previousFeedingPastEndTime = BUFSND_NO_STAMP;
#if CE_ARTLIB_SoundMaxCachedDirectSounds > 0
  if (CacheIndex >= 0)
  {
    // Caching just uses the very first and last feed time to
    // report position to the user.

    BufSndPntr->veryFirstFeedTime = CachePntr->firstFeedTime;
    BufSndPntr->veryLastFeedTime = CachePntr->lastFeedTime;
  }
  else
#endif // CE_ARTLIB_SoundMaxCachedDirectSounds > 0
  {
    BufSndPntr->veryFirstFeedTime = BUFSND_NO_STAMP;
    BufSndPntr->veryLastFeedTime = BUFSND_NO_STAMP;
  }

  for (i = 0; i < BUFSND_MAX_STAMPS; i++)
    BufSndPntr->stamps[i].time = BUFSND_NO_STAMP;

  BufSndPntr->mostRecentlyPlayedStamp.time = BUFSND_NO_STAMP;

  if (UsingCachedBuffer != NULL)
    *UsingCachedBuffer = (CacheIndex >= 0);

  goto NormalExit;


ErrorExit:

#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_StartBufSnd: "
    "Unable to start audio streaming.\r\n");
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

  LE_SOUND_StopBufSnd (BufSndPntr);
  BufSndPntr = NULL;

NormalExit:

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif

  return (LE_SOUND_BufSndHandle) BufSndPntr;
}



/*****************************************************************************
 * When you have finished with the sound, call this function.  It stops
 * playing and deallocates all internal data structures associated with
 * the sound.  If caching is enabled, it may add the direct sound buffer
 * to the cache rather than deallocating it.  Returns TRUE if successful.
 */

BOOL LE_SOUND_StopBufSnd (LE_SOUND_BufSndHandle BufSndHandle)
{
  MMRESULT      ACMErrorCode;
  BufSndPointer BufSndPntr;
  int           CacheIndex;
  CachePointer  CachePntr = NULL;
  HRESULT       DSErrorCode;
  int           EmptyCacheIndex;
  UNS16         LargestIdleCount;
  DWORD         OffsetToDataSize;
  BOOL          ReturnCode = FALSE;
  DWORD         RIFFSize;
  DWORD         StatusWord;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  BufSndPntr = (BufSndPointer) BufSndHandle;

  if (BufSndPntr != NULL)
  {
    // Close the output file, if it is being used.

    if (BufSndPntr->outputFile != NULL)
    {
      // Need to update the size of the data chunk with the amount written,
      // and the global RIFF chunk with the whole size.

      OffsetToDataSize = BufSndPntr->outputFormat->cbSize +
        sizeof (LE_SOUND_AttributesRecord) + 24;
      if (BufSndPntr->outputFormat->cbSize == 0 &&
      BufSndPntr->outputFormat->wFormatTag == WAVE_FORMAT_PCM)
        OffsetToDataSize -= sizeof (LE_SOUND_AttributesRecord) - sizeof (PCMWAVEFORMAT);
      fseek (BufSndPntr->outputFile, OffsetToDataSize, SEEK_SET);
      fwrite (&BufSndPntr->amountOfDataWrittenToFile,
        sizeof (BufSndPntr->amountOfDataWrittenToFile), 1, BufSndPntr->outputFile);

      RIFFSize = OffsetToDataSize + BufSndPntr->amountOfDataWrittenToFile - 4;
      fseek (BufSndPntr->outputFile, 4, SEEK_SET);
      fwrite (&RIFFSize, sizeof (RIFFSize), 1, BufSndPntr->outputFile);

      fclose (BufSndPntr->outputFile);
      BufSndPntr->outputFile = NULL;
    }

    // Deallocate the DirectSound buffer, if needed.

    if (BufSndPntr->directSoundBuffer != NULL)
    {
      // If the sound is currently playing, stop it.

      StatusWord = 0;
      BufSndPntr->directSoundBuffer->GetStatus (&StatusWord);

      if (StatusWord & DSBSTATUS_PLAYING)
      {
        DSErrorCode = BufSndPntr->directSoundBuffer->Stop ();
      }

      // Deallocate the 3D interface.

      if (BufSndPntr->directSound3DBuffer != NULL)
      {
        DSErrorCode = BufSndPntr->directSound3DBuffer->Release ();
        BufSndPntr->directSound3DBuffer = NULL;
      }

      // Decide what sort of caching to do with this DirectSound buffer.
      // Keep it if it is a new short sound, remove the sound from the
      // cache if it is a damaged buffer, return it to the cache if it
      // was there originally.

#if CE_ARTLIB_SoundMaxCachedDirectSounds > 0
      if (StatusWord & DSBSTATUS_BUFFERLOST)
      {
        if (BufSndPntr->cachedDirectSoundBufferIndex >= 0)
        {
          // Remove damaged buffer from the cache.

          CachePntr = CacheArray + BufSndPntr->cachedDirectSoundBufferIndex;
          CachePntr->dataID = LE_DATA_EmptyItem;
          CachePntr->inUse = FALSE;
          CachePntr->permanentlyCached = FALSE;
          BufSndPntr->cachedDirectSoundBufferIndex = -1; // So it gets deallocated.
        }
      }
      else // Not a damaged buffer.
      {
        if (BufSndPntr->cachedDirectSoundBufferIndex >= 0)
        {
          // Return an already cached buffer back to the cache.

          CachePntr = CacheArray + BufSndPntr->cachedDirectSoundBufferIndex;
          CachePntr->idleCount = 0;
          CachePntr->inUse = FALSE;
        }
        else // Valid but uncached buffer, can we cache it?
        {
          if (BufSndPntr->hintDataID != LE_DATA_EmptyItem &&
          BufSndPntr->soundIsNotSpooled &&
          BufSndPntr->veryLastFeedTime != BUFSND_NO_STAMP)
          {
            // Find a cache entry for the new item.  First try empty ones,
            // or unused ones with the same DataID as ours.  Ones that are
            // permanently cached aren't available for replacement.

            EmptyCacheIndex = -1;

            for (CacheIndex = 0, CachePntr = CacheArray + 0;
            CacheIndex < CE_ARTLIB_SoundMaxCachedDirectSounds;
            CacheIndex++, CachePntr++)
            {
              if (CachePntr->inUse || CachePntr->permanentlyCached)
                continue;

              if (CachePntr->dataID == LE_DATA_EmptyItem ||
              CachePntr->dataID == BufSndPntr->hintDataID)
              {
                EmptyCacheIndex = CacheIndex;
                break;
              }
            }

            if (EmptyCacheIndex < 0)
            {
              // Didn't find it, look for the least recently used one
              // that is not in use.  Yup, we have a least recently
              // used algorithm.

              LargestIdleCount = 0;

              for (CacheIndex = 0, CachePntr = CacheArray + 0;
              CacheIndex < CE_ARTLIB_SoundMaxCachedDirectSounds;
              CacheIndex++, CachePntr++)
              {
                if (CachePntr->inUse || CachePntr->permanentlyCached)
                  continue;

                if (++(CachePntr->idleCount) > LargestIdleCount)
                {
                  LargestIdleCount = CachePntr->idleCount;
                  EmptyCacheIndex = CacheIndex;
                }
              }
            }

            if (EmptyCacheIndex >= 0)
            {
              // Yes, we can save this DirectSound buffer for later use,
              // replace any previous one.

              CachePntr = CacheArray + EmptyCacheIndex;
              CachePntr->dataID = BufSndPntr->hintDataID;
              if (CachePntr->DSBuffer != NULL)
                DSErrorCode = CachePntr->DSBuffer->Release ();
              CachePntr->DSBuffer = BufSndPntr->directSoundBuffer;
              CachePntr->bufferSize = BufSndPntr->directSoundBufferSize;
              CachePntr->firstFeedTime = BufSndPntr->veryFirstFeedTime;
              CachePntr->lastFeedTime = BufSndPntr->veryLastFeedTime;
              CachePntr->inUse = FALSE;
              CachePntr->permanentlyCached = BufSndPntr->keepPermanentlyInCache;
              CachePntr->idleCount = 0;
              CachePntr->totalReusedCount += CachePntr->reusedSameSoundCount;
              CachePntr->reusedSameSoundCount = 0;
              CachePntr->numberOfUniqueSounds++;
              BufSndPntr->cachedDirectSoundBufferIndex =
                (INT8) EmptyCacheIndex; // Don't deallocate it now.
            }
          }
        }
      }
#endif // CE_ARTLIB_SoundMaxCachedDirectSounds > 0

      // Really deallocate the sound if it isn't a cached one,
      // or if it is a damaged buffer.

      if (BufSndPntr->cachedDirectSoundBufferIndex < 0)
      {
        // Deallocate the basic interface.

        DSErrorCode = BufSndPntr->directSoundBuffer->Release ();
      }
#if CE_ARTLIB_SoundMaxCachedDirectSounds > 0
      else // Leave the DirectSound buffer in our cache, ready for next time.
      {
        // Already stopped, just reset the position to the start.

        DSErrorCode = BufSndPntr->directSoundBuffer->SetCurrentPosition (0);
      }
#endif // CE_ARTLIB_SoundMaxCachedDirectSounds > 0

      BufSndPntr->directSoundBuffer = NULL;
    }

    if (BufSndPntr->acmStreamHandle != NULL)
    {
      ACMErrorCode = acmStreamReset (BufSndPntr->acmStreamHandle, 0);

      if (BufSndPntr->acmStreamHeader.fdwStatus & ACMSTREAMHEADER_STATUSF_PREPARED)
      {
        BufSndPntr->acmStreamHeader.cbSrcLength = BufSndPntr->acmStreamHeader.dwSrcUser;
        BufSndPntr->acmStreamHeader.cbDstLength = BufSndPntr->acmStreamHeader.dwDstUser;

        ACMErrorCode = acmStreamUnprepareHeader (BufSndPntr->acmStreamHandle,
          &BufSndPntr->acmStreamHeader, 0);
      }

      ACMErrorCode = acmStreamClose (BufSndPntr->acmStreamHandle, 0);
      BufSndPntr->acmStreamHandle = NULL;
    }

    LI_MEMORY_DeallocLibraryBlock (BufSndPntr);
    BufSndPntr = NULL;
    ReturnCode = TRUE;
  }

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Utility function to add a time stamp to the time stamp array.
 */

static void AddTimeStamp (BufSndPointer BufSndPntr, INT32 Time, UNS32 Offset)
{
  StampPointer StampPntr;

  if (BufSndPntr->numberOfActiveStamps >= BUFSND_MAX_STAMPS)
  {
#if CE_ARTLIB_EnableDebugMode
    if (BufSndPntr->keepPermanentlyInCache)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_FeedBufSnd AddTimeStamp: "
       "Ran out of time stamps, but this is for a buffer you are trying to add "
       "to the cache permanently, so it is likely that it is larger than the "
       "usual and thus will run out of time stamps harmlessly.\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
    }
    else
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_FeedBufSnd AddTimeStamp: "
       "Ran out of time stamps, try making the feed size relatively larger when "
       "compared to the DirectSound buffer size.\r\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    }
#endif
    return; // Array is full.  Will forget some stamps.
  }

  StampPntr = BufSndPntr->stamps + BufSndPntr->numberOfActiveStamps;
  BufSndPntr->numberOfActiveStamps++;

  StampPntr->time = Time;
  StampPntr->position = Offset;
}



/*****************************************************************************
 * Utility function to remove expired time stamps.
 */

static void RemoveTimeStamps (BufSndPointer BufSndPntr, UNS32 NewPlayPosition)
{
  int                 FirstUnplayedStamp;
  int                 i;
  BOOL                InPlayedZone;
  UNS32               OldPlayPosition;
  StampPointer        StampPntr;
  BOOL                Wrapped;

  OldPlayPosition = BufSndPntr->previousDirectSoundPlayPosition;
  BufSndPntr->previousDirectSoundPlayPosition = NewPlayPosition;

  // Two cases - is the played zone contiguous or does it wrap
  // around the end of the buffer?

  Wrapped = (NewPlayPosition < OldPlayPosition);
  FirstUnplayedStamp = BufSndPntr->numberOfActiveStamps;

  // Find all the played stamps, stop when we hit the first unplayed one.

  for (i = 0; i < BufSndPntr->numberOfActiveStamps; i++)
  {
    StampPntr = BufSndPntr->stamps + i;

    // See if this time stamp is in the played zone.

    if (Wrapped)
      InPlayedZone = (StampPntr->position >= OldPlayPosition ||
      StampPntr->position < NewPlayPosition);
    else
      InPlayedZone = (StampPntr->position >= OldPlayPosition &&
      StampPntr->position < NewPlayPosition);

    if (!InPlayedZone)
    {
      FirstUnplayedStamp = i;
      break;
    }
  }

  // Move down the unplayed stamps to fill the hole the played ones
  // have vacated.  Remember the most recently played one for later
  // interpolation use.

  if (FirstUnplayedStamp > 0)
  {
    BufSndPntr->mostRecentlyPlayedStamp =
      BufSndPntr->stamps [FirstUnplayedStamp - 1];

    BufSndPntr->numberOfActiveStamps -= FirstUnplayedStamp;
    for (i = 0; i < BufSndPntr->numberOfActiveStamps; i++)
      BufSndPntr->stamps [i] = BufSndPntr->stamps [i + FirstUnplayedStamp];
  }
}



/*****************************************************************************
 * Gives the sound system some data to play.  You have to periodically give
 * it data otherwise it will start replaying old data.
 *
 * You can specify an input buffer with your data by setting
 * InputBufferByteSize and InputBufferPntr.  Use size zero to flush out
 * the last decompression dregs and fill the remainder of the DirectSound
 * buffer with silence; if you are compressing sound keep on using
 * zero until no more leftovers come out, use LE_SOUND_GetTimeOfBufSnd to
 * see when it has finished flushing).  InputUsed can be NULL or point to
 * a BOOL which will be set to TRUE if your input buffer was used (it gets
 * fully used) or FALSE if your input wasn't touched (because there was still
 * some decompressed data hanging around from the previous feeding - which
 * gets submitted to DirectSound in this feeding, maybe making room, so if
 * it says it can't take your data, try feeding again).  You should also stop
 * feeding for a little while when DirectSoundBufferIsFull returns TRUE since
 * there isn't any more space to put your (decompressed) audio data.
 *
 * You can also get a copy of the decompressed data (useful for voice chat)
 * by specifying OutputBufferByteSize and OutputBufferPntr.  It will return
 * the number of bytes of data put into your buffer in OutputSizeUsed.
 *
 * You also can set the start and end time that the data covers.  The end
 * time should be just past the end of the data.  When you call
 * LE_SOUND_GetTimeNowPlaying it returns the time value from StartTime
 * up to PastEndTime, proportionaly interpolated by the amount of data
 * that has actually been played out the sound card.  The start and end
 * times for a sequence of buffers don't have to be in order - this is
 * useful for keeping the video synchronised with the sound while switching
 * between different parts of the video (the sound currently playing selects
 * the frame being displayed).
 *
 * If CutToThisFeedRightNow is TRUE then it will ignore any pending sound
 * in the DirectSound buffer and write your new data so that you hear it
 * right away.  This is useful for jumping to a different position in the
 * sound without any time delay.  The alternative (FALSE) is to put your
 * sound data after the data already in the buffers, which makes for
 * seamless sound continuations.  So, if you must jump cut, try to do it
 * when the sound is quiet otherwise you will hear a pop or other noise.
 *
 * The first time you call this function after calling LE_SOUND_StartBufSnd,
 * the sound gets started (the system is in a kind of paused state until
 * it gets some data).
 */

BOOL LE_SOUND_FeedBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  DWORD InputBufferByteSize, BYTE *InputBufferPntr, BOOL *InputUsed,
  DWORD OutputBufferByteSize, BYTE *OutputBufferPntr, DWORD *OutputSizeUsed,
  BOOL *DirectSoundBufferIsFull, INT32 StartTime, INT32 PastEndTime,
  BOOL CutToThisFeedRightNow)
{
  MMRESULT            ACMErrorCode;
  DWORD               AmountToWrite;
  BufSndPointer       BufSndPntr;
  PDSBUFFER           DSBuffer;
  HRESULT             DSErrorCode;
  BYTE               *DSLockPntr1;
  BYTE               *DSLockPntr2;
  DWORD               DSLockSize1;
  DWORD               DSLockSize2;
  DWORD               DSPlayCursor;
  DWORD               DSWriteCursor;
  BOOL                FlushDregs = FALSE;
  DWORD               Modulo;
  DWORD               RemainingSource;
  BOOL                ReturnCode = FALSE;
  DWORD               SourceFreeSpace;
  DWORD               StatusWord;
  DWORD               WriteSpaceAvailable;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  BufSndPntr = (BufSndPointer) BufSndHandle;
  if (BufSndPntr == NULL)
    goto ErrorExit;

  // Cached buffers already contain their data, no feeding!
  // Note that non-spooled buffers can be fed, usually all the feeding
  // is done while the buffer is in a paused state, so that it is
  // completely full before it starts playing.

  if (BufSndPntr->cachedDirectSoundBufferIndex >= 0)
    goto ErrorExit; // Cached buffers have already been fed!

  if (InputBufferByteSize > BufSndPntr->acmStreamHeader.dwSrcUser)
    goto ErrorExit; // Can only handle data that fits into our buffer.

  // Inbetween decompression operations, the amount of data in the source
  // and destination buffers is given by cbSrcLength and cbDstLength.
  // Also, the destination data starts at byte offset cbDstLengthUsed
  // from the start of the destination buffer (so that we don't have
  // to shift it over in the buffer).

  if (CutToThisFeedRightNow)
  {
    // Toss away any stuff in our buffers (DirectSound buffers will be
    // dealt with later on) so that the new data gets played right away.

    BufSndPntr->acmStreamHeader.cbSrcLength = 0;
    BufSndPntr->acmStreamHeader.cbDstLength = 0;
    BufSndPntr->acmStreamHeader.cbDstLengthUsed = 0;
  }

  // Still have some decompressed data to play?  If so, don't do any
  // decompression and leave the input untouched.  Otherwise fill up
  // the input buffer with ALL the new compressed data and decompress it.
  // Need to do all at once otherwise keeping track of the time stamps
  // gets messy.  Ignore the leftovers from the last decompression as
  // far as time stamping goes.

  if (BufSndPntr->acmStreamHeader.cbDstLength == 0)
  {
    if (BufSndPntr->acmStreamHandle == NULL)
    {
      // No compression, just copy the data.  Put the size of data
      // in the destination buffer into cbDstLengthUsed, which will
      // get moved to cbDstLength later on.

      if (InputBufferByteSize > 0)
      {
        memcpy (BufSndPntr->acmStreamHeader.pbDst, InputBufferPntr,
          InputBufferByteSize);
        BufSndPntr->acmStreamHeader.cbDstLengthUsed = InputBufferByteSize;
      }
      else // No data received.
        BufSndPntr->acmStreamHeader.cbDstLengthUsed = 0;
    }
    else // Doing decompression / compression.
    {
      SourceFreeSpace = BufSndPntr->acmStreamHeader.dwSrcUser -
        BufSndPntr->acmStreamHeader.cbSrcLength;

      if (InputBufferByteSize > SourceFreeSpace)
        goto ErrorExit; // Feeding us too much!

      if (InputBufferByteSize > 0)
      {
        memcpy (BufSndPntr->acmStreamHeader.pbSrc + BufSndPntr->acmStreamHeader.cbSrcLength,
          InputBufferPntr, InputBufferByteSize);
        BufSndPntr->acmStreamHeader.cbSrcLength += InputBufferByteSize;
      }
      else // No new data, see if the dregs can give us some more.
        FlushDregs = TRUE;

      BufSndPntr->acmStreamHeader.cbSrcLengthUsed = 0;
      BufSndPntr->acmStreamHeader.cbDstLength = BufSndPntr->acmStreamHeader.dwDstUser;
      BufSndPntr->acmStreamHeader.cbDstLengthUsed = 0;

      ACMErrorCode = acmStreamConvert (BufSndPntr->acmStreamHandle,
        &BufSndPntr->acmStreamHeader,
        ((BufSndPntr->previousFeedingPastEndTime == StartTime) ? 0 : ACM_STREAMCONVERTF_START) |
        (FlushDregs ? ACM_STREAMCONVERTF_END : ACM_STREAMCONVERTF_BLOCKALIGN));
      if (ACMErrorCode != 0)
      {
#if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_FeedBufSnd: "
       "ACM error %d (512 usually means some buffer size is too small).\r\n",
       (int) ACMErrorCode);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
        goto ErrorExit;
      }

      RemainingSource = BufSndPntr->acmStreamHeader.cbSrcLength -
        BufSndPntr->acmStreamHeader.cbSrcLengthUsed;
      if (RemainingSource > 0)
      {
        // Shove over the leftover source data (part of a compression block)
        // to the front of the buffer.

        memmove (BufSndPntr->acmStreamHeader.pbSrc,
          BufSndPntr->acmStreamHeader.pbSrc + BufSndPntr->acmStreamHeader.cbSrcLengthUsed,
          RemainingSource);
        BufSndPntr->acmStreamHeader.cbSrcLength = RemainingSource;
      }
      else // All of source was used.
      {
        BufSndPntr->acmStreamHeader.cbSrcLength = 0;
      }
    }

    // Switch back to the meanings used between compressions:
    // cbDstLength is the amount of destination buffer data,
    // cbDstLengthUsed is the offset from the start of the buffer.

    BufSndPntr->acmStreamHeader.cbDstLength =
      BufSndPntr->acmStreamHeader.cbDstLengthUsed;
    BufSndPntr->acmStreamHeader.cbDstLengthUsed = 0;

    if (InputBufferByteSize > 0)
    {
      // Update the time stamps that show what's in our buffer,
      // but only if the user provided some data.  Otherwise we
      // are just doing the dregs and the old stamps apply.

      BufSndPntr->previousFeedingStartTime = StartTime;
      BufSndPntr->previousFeedingPastEndTime = PastEndTime;

      // Note the first and last feed times, in case we later
      // want to cache the buffer.

      if (BufSndPntr->veryFirstFeedTime == BUFSND_NO_STAMP)
      {
        BufSndPntr->veryFirstFeedTime = StartTime;
        BufSndPntr->mostRecentlyPlayedStamp.time = StartTime;
      }
    }

    if (InputUsed != NULL)
      *InputUsed = TRUE;

    // If we are writing to a file, dump the data.

    if (BufSndPntr->outputFile != NULL &&
    BufSndPntr->acmStreamHeader.cbDstLength > 0)
    {
      if (fwrite (BufSndPntr->acmStreamHeader.pbDst,
      BufSndPntr->acmStreamHeader.cbDstLength, 1,
      BufSndPntr->outputFile) != 1)
        goto ErrorExit;

      BufSndPntr->amountOfDataWrittenToFile +=
        BufSndPntr->acmStreamHeader.cbDstLength;
    }

    // If outputing to a user's memory buffer, copy the data.

    if (OutputBufferByteSize > 0)
    {
      if (BufSndPntr->acmStreamHeader.cbDstLength > 0)
      {
        if (BufSndPntr->acmStreamHeader.cbDstLength > OutputBufferByteSize)
          goto ErrorExit; // Your buffer is too small!

        memcpy (OutputBufferPntr, BufSndPntr->acmStreamHeader.pbDst,
          BufSndPntr->acmStreamHeader.cbDstLength);

        if (OutputSizeUsed != NULL)
          *OutputSizeUsed = BufSndPntr->acmStreamHeader.cbDstLength;
      }
      else // No data to copy to user's buffer.
      {
        if (OutputSizeUsed != NULL)
          *OutputSizeUsed = 0;
      }
    }
  }
  else // Already have some decompressed data from last time.
  {
    if (InputUsed != NULL)
      *InputUsed = FALSE;
  }

  // Now feed the output data to the DirectSound buffer, if any.

  DSBuffer = BufSndPntr->directSoundBuffer;
  if (DSBuffer != NULL)
  {
    Modulo = BufSndPntr->directSoundBufferSize; // Convenience variable.

    // First check for a lost buffer, restore it if needed.

    DSErrorCode = DSBuffer->GetStatus (&StatusWord);
    if (DSErrorCode != DS_OK) goto ErrorExit;

    if (StatusWord & DSBSTATUS_BUFFERLOST)
    {
      BufSndPntr->nextDirectSoundWriteOffset = 0;

      // Wipe out almost all stamps, only the previousFeedingPastEndTime
      // is still meaningful (we are pretending that we played all the
      // data which was in the lost buffer).

      BufSndPntr->numberOfActiveStamps = 0;
      BufSndPntr->previousDirectSoundPlayPosition = 0;
      BufSndPntr->mostRecentlyPlayedStamp.time =
        BufSndPntr->previousFeedingPastEndTime;

      // Try to restore the buffer, only if we have something to play.

      if (BufSndPntr->acmStreamHeader.cbDstLength > 0)
      {
        DSErrorCode = DSBuffer->Restore ();
        if (DSErrorCode != DS_OK && DSErrorCode != DSERR_BUFFERLOST)
          goto ErrorExit;

        DSErrorCode = DSBuffer->GetStatus (&StatusWord);
        if (DSErrorCode != DS_OK) goto ErrorExit;
      }
    }

    // Remove expired time stamps based on the current play position.
    // Or if we are cutting to a new buffer then remove all stamps.

    if ((StatusWord & DSBSTATUS_BUFFERLOST) == 0)
    {
      DSErrorCode = DSBuffer->GetCurrentPosition (
        &DSPlayCursor, &DSWriteCursor);
      if (DSErrorCode != DS_OK)
        goto ErrorExit;

      if (CutToThisFeedRightNow)
      {
        // Remove all stamps and write over the stuff already in the
        // buffer with our new stuff, so that it gets played very soon.

        BufSndPntr->nextDirectSoundWriteOffset = DSWriteCursor;
        BufSndPntr->previousDirectSoundPlayPosition = DSPlayCursor;
        BufSndPntr->numberOfActiveStamps = 0;
        BufSndPntr->mostRecentlyPlayedStamp.time = StartTime;
      }
      else
        RemoveTimeStamps (BufSndPntr, DSPlayCursor);
    }

    // Fill the buffer with more data, if we have it.  Don't feed it
    // if there aren't any time stamps available.

    if ((StatusWord & DSBSTATUS_BUFFERLOST) == 0 &&
    BufSndPntr->acmStreamHeader.cbDstLength > 0 &&
    BufSndPntr->numberOfActiveStamps <= BUFSND_MAX_STAMPS - 2)
    {
      // Make sure that we aren't writing in the space between the play
      // cursor and the write cursor.  The write cursor marks the spot just
      // after the play cursor where DirectSound isn't yet mixing data;
      // it is not the spot last written.

      if (DSWriteCursor < DSPlayCursor) // If inaccessible position crosses end.
      {
        if (BufSndPntr->nextDirectSoundWriteOffset < DSWriteCursor ||
        BufSndPntr->nextDirectSoundWriteOffset >= DSPlayCursor)
          WriteSpaceAvailable = 0; // It's playing where we want to write.
        else
          WriteSpaceAvailable = DSPlayCursor - BufSndPntr->nextDirectSoundWriteOffset;
      }
      else // Inaccessible position isn't wrapping around the end.
      {
        if (BufSndPntr->nextDirectSoundWriteOffset < DSPlayCursor)
          WriteSpaceAvailable = DSPlayCursor - BufSndPntr->nextDirectSoundWriteOffset;
        else if (BufSndPntr->nextDirectSoundWriteOffset >= DSWriteCursor)
          WriteSpaceAvailable = Modulo + DSPlayCursor - BufSndPntr->nextDirectSoundWriteOffset;
        else
          WriteSpaceAvailable = 0; // It's playing where we want to write.
      }

      // Avoid completely filling up the buffer, which would make it
      // indistinguishable from an empty buffer due to the modularity
      // wrap-around.

      if (WriteSpaceAvailable > BufSndPntr->outputFormat->nBlockAlign)
        WriteSpaceAvailable -= BufSndPntr->outputFormat->nBlockAlign;
      else
        WriteSpaceAvailable = 0;

      AmountToWrite = BufSndPntr->acmStreamHeader.cbDstLength;
      if (AmountToWrite > WriteSpaceAvailable)
        AmountToWrite = WriteSpaceAvailable;

      if (AmountToWrite > 0)
      {
        DSErrorCode = DSBuffer->Lock (
          BufSndPntr->nextDirectSoundWriteOffset, AmountToWrite,
          (void **) &DSLockPntr1, &DSLockSize1,
          (void **) &DSLockPntr2, &DSLockSize2, 0);

        if (DSErrorCode == DSERR_BUFFERLOST)
        {
          // Darn, lost it just before trying to fill it.
          StatusWord |= DSBSTATUS_BUFFERLOST;
        }
        else if (DSErrorCode != DS_OK)
          goto ErrorExit;
        else
        {
          // First time stamp the position if this is the start of a feeding.

          if (BufSndPntr->previousFeedingStartTime != BUFSND_NO_STAMP)
          {
            AddTimeStamp (BufSndPntr,
              BufSndPntr->previousFeedingStartTime /* Time */,
              BufSndPntr->nextDirectSoundWriteOffset /* Offset */);
            BufSndPntr->previousFeedingStartTime = BUFSND_NO_STAMP;
          }

          // OK, got a lock on the buffer, copy data to it.

          if (DSLockSize1 > 0)
          {
            if (DSLockSize1 > BufSndPntr->acmStreamHeader.cbDstLength)
              DSLockSize1 = BufSndPntr->acmStreamHeader.cbDstLength;
            if (DSLockSize1 > 0)
              memcpy (DSLockPntr1,
              BufSndPntr->acmStreamHeader.pbDst + BufSndPntr->acmStreamHeader.cbDstLengthUsed,
              DSLockSize1);
            BufSndPntr->acmStreamHeader.cbDstLengthUsed += DSLockSize1;
            BufSndPntr->acmStreamHeader.cbDstLength -= DSLockSize1;
            BufSndPntr->nextDirectSoundWriteOffset += DSLockSize1;
          }
          if (DSLockSize2 > 0)
          {
            if (DSLockSize2 > BufSndPntr->acmStreamHeader.cbDstLength)
              DSLockSize2 = BufSndPntr->acmStreamHeader.cbDstLength;
            if (DSLockSize2 > 0)
              memcpy (DSLockPntr2,
              BufSndPntr->acmStreamHeader.pbDst + BufSndPntr->acmStreamHeader.cbDstLengthUsed,
              DSLockSize2);
            BufSndPntr->acmStreamHeader.cbDstLengthUsed += DSLockSize2;
            BufSndPntr->acmStreamHeader.cbDstLength -= DSLockSize2;
            BufSndPntr->nextDirectSoundWriteOffset += DSLockSize2;
          }

          DSErrorCode = DSBuffer->Unlock (
            DSLockPntr1, DSLockSize1, DSLockPntr2, DSLockSize2);
          if (DSErrorCode != DS_OK)
            goto ErrorExit;

          if (BufSndPntr->nextDirectSoundWriteOffset >= Modulo)
            BufSndPntr->nextDirectSoundWriteOffset -= Modulo;

          // If this was the last bit of output data, add a time stamp.

          if (BufSndPntr->acmStreamHeader.cbDstLength == 0)
          {
            AddTimeStamp (BufSndPntr,
              BufSndPntr->previousFeedingPastEndTime /* Time */,
              BufSndPntr->nextDirectSoundWriteOffset /* Offset */);
          }
        }
      }
    }

    // Fill the rest of the buffer with silence if there isn't any more
    // data from the user and we have run out of decompressed data.
    // This is because the buffer keeps on looping after the last bit
    // of data and doesn't stop exactly at that point.

    if ((StatusWord & DSBSTATUS_BUFFERLOST) == 0 &&
    BufSndPntr->acmStreamHeader.cbDstLength == 0 &&
    InputBufferByteSize == 0)
    {
      // OK, the user is filling this buffer with silence, which means
      // that the entire thing will be playable (no noise or garbage
      // after the end of the sound).  So it may be eligible for caching.

      BufSndPntr->veryLastFeedTime = BufSndPntr->previousFeedingPastEndTime;

      DSErrorCode = DSBuffer->GetCurrentPosition (
        &DSPlayCursor, &DSWriteCursor);
      if (DSErrorCode != DS_OK)
        goto ErrorExit;

      // Make sure that we aren't writing in the space between the play
      // cursor and the write cursor.

      if (DSWriteCursor < DSPlayCursor) // If inaccessible position crosses end.
      {
        if (BufSndPntr->nextDirectSoundWriteOffset < DSWriteCursor ||
        BufSndPntr->nextDirectSoundWriteOffset >= DSPlayCursor)
          WriteSpaceAvailable = 0; // It's playing where we want to write.
        else
          WriteSpaceAvailable = DSPlayCursor - BufSndPntr->nextDirectSoundWriteOffset;
      }
      else // Inaccessible position isn't wrapping around the end.
      {
        if (BufSndPntr->nextDirectSoundWriteOffset < DSPlayCursor)
          WriteSpaceAvailable = DSPlayCursor - BufSndPntr->nextDirectSoundWriteOffset;
        else if (BufSndPntr->nextDirectSoundWriteOffset >= DSWriteCursor)
          WriteSpaceAvailable = Modulo + DSPlayCursor - BufSndPntr->nextDirectSoundWriteOffset;
        else
          WriteSpaceAvailable = 0; // It's playing where we want to write.
      }

      // Avoid completely filling up the buffer, which would make it
      // indistinguishable from an empty buffer due to the modularity
      // wrap-around.

      if (WriteSpaceAvailable > BufSndPntr->outputFormat->nBlockAlign)
        WriteSpaceAvailable -= BufSndPntr->outputFormat->nBlockAlign;
      else
        WriteSpaceAvailable = 0;

      AmountToWrite = WriteSpaceAvailable /
        BufSndPntr->outputFormat->nBlockAlign *
        BufSndPntr->outputFormat->nBlockAlign;

      if (AmountToWrite > 0)
      {
        DSErrorCode = DSBuffer->Lock (
          BufSndPntr->nextDirectSoundWriteOffset, AmountToWrite,
          (void **) &DSLockPntr1, &DSLockSize1,
          (void **) &DSLockPntr2, &DSLockSize2, 0);

        if (DSErrorCode == DSERR_BUFFERLOST)
        {
          // Darn, lost it just before trying to fill it.
          StatusWord |= DSBSTATUS_BUFFERLOST;
        }
        else if (DSErrorCode != DS_OK)
          goto ErrorExit;
        else
        {
          // OK, got a lock on the buffer, fill it with silence.  Silence
          // value is 0 for 16 bit samples, 0x80 for 8 bit sound samples.

          if (DSLockSize1 > 0)
          {
            memset (DSLockPntr1,
              (BufSndPntr->outputFormat->wBitsPerSample == 8) ? 0x80 : 0,
              DSLockSize1);
            BufSndPntr->nextDirectSoundWriteOffset += DSLockSize1;
          }

          if (DSLockSize2 > 0)
          {
            memset (DSLockPntr2,
              (BufSndPntr->outputFormat->wBitsPerSample == 8) ? 0x80 : 0,
              DSLockSize2);
            BufSndPntr->nextDirectSoundWriteOffset += DSLockSize2;
          }

          DSErrorCode = DSBuffer->Unlock (
            DSLockPntr1, DSLockSize1, DSLockPntr2, DSLockSize2);
          if (DSErrorCode != DS_OK)
            goto ErrorExit;

          if (BufSndPntr->nextDirectSoundWriteOffset >= Modulo)
            BufSndPntr->nextDirectSoundWriteOffset -= Modulo;
        }
      }
    }
  }

  // Make sure the buffer is playing if it should be playing, or stopped
  // if the user wants to pause the sound.  Needed after restoring a buffer
  // or for the first time when playing starts.

  if (DSBuffer != NULL)
  {
    if ((StatusWord & DSBSTATUS_BUFFERLOST) == 0)
    {
      if (BufSndPntr->pausedByTheUser)
      {
        if (StatusWord & DSBSTATUS_PLAYING)
        {
          DSErrorCode = DSBuffer->Stop ();
          if (DSErrorCode != DS_OK) goto ErrorExit;
        }
      }
      else // The user wants this to be playing.
      {
        BOOL  WantToLoop;

        if (!BufSndPntr->soundIsNotSpooled)
          WantToLoop = TRUE; // Spooled sounds always loop.
        else if (BufSndPntr->userWishesToLoop)
          WantToLoop = TRUE; // User wants to loop for non-spooled.
        else
          WantToLoop = FALSE; // Single shot non-spooled.

        if ((StatusWord & DSBSTATUS_PLAYING) == 0 /* If not playing */ ||
        ((StatusWord & DSBSTATUS_LOOPING) != 0) != WantToLoop /* Wrong looping */)
        {
          // Restart playback if it stops or if it isn't
          // looping like we want it to.

          DSErrorCode = DSBuffer->Play (0, 0,
            WantToLoop ? DSBPLAY_LOOPING : 0);
          if (DSErrorCode != DS_OK) goto ErrorExit;
        }
      }
    }
  }
  else // Not playing sound through speakers, safe to discard output data.
  {
    BufSndPntr->acmStreamHeader.cbDstLength = 0;
    BufSndPntr->acmStreamHeader.cbDstLengthUsed = 0;
  }

  ReturnCode = TRUE;
  goto NormalExit;


ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_FeedBufSnd: "
    "Unable to feed audio streaming.\r\n");
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:
  if (DirectSoundBufferIsFull != NULL)
    *DirectSoundBufferIsFull = (BufSndPntr->acmStreamHeader.cbDstLength > 0);

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Find out the current playing status of a sound.  Returns the position in
 * user defined units (usually video frame number, doesn't have to be related
 * to ticks or bytes or anything - the system blindly uses values you provided
 * for StartTime and PastEndTime when you called LE_SOUND_FeedBufSnd and
 * interpolates linearly between them based on the proportion of the sound
 * played).  The proportion of the buffer's uncompressed data is used so that
 * changes in compression rate don't affect the returned values.  Returns the
 * interpolated value or -1 if it fails.
 */

INT32 LE_SOUND_GetTimeOfBufSnd (LE_SOUND_BufSndHandle BufSndHandle)
{
  BufSndPointer       BufSndPntr;
  INT32               CurrentPosition;
  INT32               DeltaPosition;
  INT32               DeltaTime;
  PDSBUFFER           DSBuffer;
  HRESULT             DSErrorCode;
  DWORD               DSPlayCursor;
  DWORD               DSWriteCursor;
  DWORD               PlayPosition;
  INT32               ReturnValue = -1;
  DWORD               StatusWord;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return -1; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return -1; // Closed while we were waiting.
#endif

  BufSndPntr = (BufSndPointer) BufSndHandle;

  if (BufSndPntr == NULL)
    goto ErrorExit;

  DSBuffer = BufSndPntr->directSoundBuffer;

  if (BufSndPntr->soundIsNotSpooled)
  {
    // This is a sound which fills the whole sound buffer and doesn't
    // use spooling.  Use the position within the sound buffer as a
    // proportion of the whole time.

    DSPlayCursor = 0;
    DSErrorCode = DSBuffer->GetCurrentPosition (
      &DSPlayCursor, &DSWriteCursor);
    if (DSErrorCode != DS_OK) goto ErrorExit;

    StatusWord = 0;
    DSErrorCode = DSBuffer->GetStatus (&StatusWord);
    if ((StatusWord & DSBSTATUS_BUFFERLOST) || DSErrorCode != DS_OK)
      goto ErrorExit;

    if (StatusWord & DSBSTATUS_PLAYING)
    {
      // Note that DirectSound 5 sometimes lies to us and when the sound
      // is over, says that the sound is still playing but has position
      // zero.  But we'll ignore that for now since it is hard to tell
      // apart from looping sounds or sounds starting up.

      PlayPosition = DSPlayCursor;
    }
    else if (DSPlayCursor != 0)
      PlayPosition = DSPlayCursor;
    else
    {
      // Stopped at location zero.  Either because we finished playing
      // or because we have yet to start playing.

      if (BufSndPntr->pausedByTheUser)
        PlayPosition = 0; // At the start of the sound.
      else // Have played and come to a stop.
        PlayPosition = BufSndPntr->directSoundBufferSize;
    }

    DeltaTime = BufSndPntr->veryLastFeedTime - BufSndPntr->veryFirstFeedTime;
    DeltaPosition = BufSndPntr->directSoundBufferSize;
    CurrentPosition = PlayPosition;

    if (CurrentPosition >= DeltaPosition) // In case of compiler math bugs.
      ReturnValue = BufSndPntr->veryLastFeedTime;
    else // Calculate the proportional position.
      ReturnValue = (INT32) (DeltaTime * (double) CurrentPosition /
      (double) DeltaPosition) + BufSndPntr->veryFirstFeedTime;
  }
  else if (BufSndPntr->previousFeedingPastEndTime == BUFSND_NO_STAMP)
    ReturnValue = -1; // Haven't started feeding the buffers yet.
  else if (BufSndPntr->directSoundBuffer != NULL)
  {
    // First update the time stamps, removing old expired ones.

    DSErrorCode = DSBuffer->GetStatus (&StatusWord);
    if (DSErrorCode != DS_OK) goto ErrorExit;

    if (StatusWord & DSBSTATUS_BUFFERLOST)
    {
      // Lost the buffer.

      BufSndPntr->numberOfActiveStamps = 0;
      BufSndPntr->previousDirectSoundPlayPosition = 0;
      BufSndPntr->mostRecentlyPlayedStamp.time =
        BufSndPntr->previousFeedingPastEndTime;
      DSPlayCursor = 0;
    }
    else // Still playing.
    {
      DSPlayCursor = 0;
      DSErrorCode = DSBuffer->GetCurrentPosition (
        &DSPlayCursor, &DSWriteCursor);
      if (DSErrorCode == DS_OK)
        RemoveTimeStamps (BufSndPntr, DSPlayCursor);
    }

    // Calculate the time represented by the current play position.

    if (BufSndPntr->numberOfActiveStamps == 0)
    {
      // Probably ran out of data (feeding isn't keeping up with the
      // sound hardware).  Or a jump cut or buffer was lost.  So, use
      // the last played time, and don't do interpolation since it
      // hasn't finished off the current partial feeding (the feeding
      // end time hasn't been fed and time stamped yet).

      ReturnValue = BufSndPntr->mostRecentlyPlayedStamp.time;

      if (BufSndPntr->mostRecentlyPlayedStamp.time == BUFSND_NO_STAMP)
        ReturnValue = BufSndPntr->previousFeedingPastEndTime;
    }
    else // Can interpolate between currently playing stamps.
    {
      DeltaTime = BufSndPntr->stamps[0].time -
        BufSndPntr->mostRecentlyPlayedStamp.time;

      DeltaPosition = (INT32) BufSndPntr->stamps[0].position -
        (INT32) BufSndPntr->mostRecentlyPlayedStamp.position;
      if (DeltaPosition < 0)
        DeltaPosition += BufSndPntr->directSoundBufferSize;

      CurrentPosition = (INT32) DSPlayCursor -
        (INT32) BufSndPntr->mostRecentlyPlayedStamp.position;
      if (CurrentPosition < 0)
        CurrentPosition += BufSndPntr->directSoundBufferSize;

      if (CurrentPosition > DeltaPosition)
        CurrentPosition = DeltaPosition; // Peg it at the end.

      if (DeltaPosition <= 0) // Avoid divide by zero when between end/start stamps.
        ReturnValue = BufSndPntr->stamps[0].time;
      else
        ReturnValue = (INT32)
        (DeltaTime * (double) CurrentPosition / (double) DeltaPosition) +
        BufSndPntr->mostRecentlyPlayedStamp.time;
    }
  }
  else // Not using DirectSound
  {
    ReturnValue = BufSndPntr->previousFeedingPastEndTime;
  }

ErrorExit:
#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif
  return ReturnValue;
}



/*****************************************************************************
 * Get the attributes of the output data, this returns a pointer to an
 * internal structure so don't tamper with it.  Returns NULL on failure.
 */

LE_SOUND_AttributesPointer LE_SOUND_GetOutputAttributesOfBufSnd (
  LE_SOUND_BufSndHandle BufSndHandle)
{
  BufSndPointer BufSndPntr;

  if (BufSndHandle == NULL)
    return NULL;

  BufSndPntr = (BufSndPointer) BufSndHandle;

  return BufSndPntr->outputFormat;
}



/*****************************************************************************
 * Pause the sound.  This corresponds to stoping the audio hardware playback
 * of the sound (DirectSound's Stop/Play).  If PauseIt is TRUE then it
 * is paused, if FALSE then it resumes from where it was paused.  Returns
 * TRUE if successful.
 */

BOOL LE_SOUND_PauseBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  BOOL PauseIt)
{
  BufSndPointer       BufSndPntr;
  PDSBUFFER           DSBuffer;
  HRESULT             DSErrorCode;
  BOOL                ReturnCode = FALSE;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  BufSndPntr = (BufSndPointer) BufSndHandle;

  if (BufSndPntr == NULL)
    goto ErrorExit;

  DSBuffer = BufSndPntr->directSoundBuffer;
  if (DSBuffer == NULL)
    goto ErrorExit;

  if (PauseIt != (BOOL) BufSndPntr->pausedByTheUser)
  {
    BufSndPntr->pausedByTheUser = (UNS8) PauseIt;

    if (PauseIt)
    {
      DSErrorCode = DSBuffer->Stop ();
      if (DSErrorCode != DS_OK) goto ErrorExit;
    }
    else // Hopefully there still is some stuff in the buffer to play.
    {
      DSErrorCode = DSBuffer->Play (0, 0,
        (BufSndPntr->userWishesToLoop || !BufSndPntr->soundIsNotSpooled)
        ? DSBPLAY_LOOPING : 0);
      if (DSErrorCode != DS_OK) goto ErrorExit;
    }
  }

  ReturnCode = TRUE;

ErrorExit:

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Change the looping characteristics of the sound.  This only works for
 * non-spooled sounds (ones that fit exactly into the buffer).
 */

BOOL LE_SOUND_LoopNonSpooledBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  BOOL LoopIt)
{
  BufSndPointer       BufSndPntr;
  PDSBUFFER           DSBuffer;
  HRESULT             DSErrorCode;
  BOOL                ReturnCode = FALSE;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  BufSndPntr = (BufSndPointer) BufSndHandle;

  if (BufSndPntr == NULL)
    goto ErrorExit;

  DSBuffer = BufSndPntr->directSoundBuffer;
  if (DSBuffer == NULL)
    goto ErrorExit;

  if (BufSndPntr->userWishesToLoop != LoopIt)
  {
    BufSndPntr->userWishesToLoop = (UNS8) LoopIt;

    if (!BufSndPntr->pausedByTheUser)
    {
      DSErrorCode = DSBuffer->Play (0, 0,
      (BufSndPntr->userWishesToLoop || !BufSndPntr->soundIsNotSpooled)
        ? DSBPLAY_LOOPING : 0);
      if (DSErrorCode != DS_OK) goto ErrorExit;
    }
  }

  ReturnCode = TRUE;

ErrorExit:

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Change the current playing position of a non-spooled sound.  The equivalent
 * operation in a spooled sound is to feed data for the new position with the
 * CutToThisFeedRightNow option.
 */

BOOL LE_SOUND_CutToPositionInNonSpooledBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  UNS32 NewPosition)
{
  BufSndPointer       BufSndPntr;
  PDSBUFFER           DSBuffer;
  HRESULT             DSErrorCode;
  BOOL                ReturnCode = FALSE;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  BufSndPntr = (BufSndPointer) BufSndHandle;
  if (BufSndPntr == NULL) goto ErrorExit;

  DSBuffer = BufSndPntr->directSoundBuffer;
  if (DSBuffer == NULL) goto ErrorExit;

  DSErrorCode = DSBuffer->SetCurrentPosition (NewPosition);
  if (DSErrorCode != DS_OK) goto ErrorExit;

  // If it has stopped because it reached the end, restart it playing.

  if (!BufSndPntr->pausedByTheUser)
  {
    DSErrorCode = DSBuffer->Play (0, 0,
    (BufSndPntr->userWishesToLoop || !BufSndPntr->soundIsNotSpooled)
      ? DSBPLAY_LOOPING : 0);
    if (DSErrorCode != DS_OK) goto ErrorExit;
  }

  ReturnCode = TRUE;

ErrorExit:
#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif
  return ReturnCode;
}



/*****************************************************************************
 * Set the left/right panning of the sound in stereo systems.  Full left is
 * -100, 0 is center, +100 is full right.  Returns TRUE if successful.
 * Can't be used with 3D sounds.
 */

BOOL LE_SOUND_SetPanBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  INT8 PanPercentage)
{
  BufSndPointer       BufSndPntr;
  PDSBUFFER           DSBuffer;
  HRESULT             DSErrorCode;
  LONG                PanValue;
  BOOL                ReturnCode = FALSE;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  BufSndPntr = (BufSndPointer) BufSndHandle;

  if (BufSndPntr == NULL)
    goto ErrorExit;

  DSBuffer = BufSndPntr->directSoundBuffer;
  if (DSBuffer == NULL)
    goto ErrorExit;

  if (PanPercentage < -100)
    PanPercentage = -100;
  else if (PanPercentage > 100)
    PanPercentage = 100;

  if (PanPercentage <= 0)
    PanValue = nLogVolume [PanPercentage + 100];
  else
    PanValue = -nLogVolume [100 - PanPercentage];

  DSErrorCode = DSBuffer->SetPan (PanValue);
  if (DSErrorCode != DS_OK)
    goto ErrorExit;

  ReturnCode = TRUE;

ErrorExit:
#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif
  return ReturnCode;
}



/*****************************************************************************
 * Set the volume of an individual sound.  0 is quiet, 100 is loud.
 * Returns TRUE if successful.
 */

BOOL LE_SOUND_SetVolumeBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  UNS8 VolumePercentage)
{
  BufSndPointer       BufSndPntr;
  PDSBUFFER           DSBuffer;
  HRESULT             DSErrorCode;
  BOOL                ReturnCode = FALSE;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  BufSndPntr = (BufSndPointer) BufSndHandle;

  if (BufSndPntr == NULL)
    goto ErrorExit;

  DSBuffer = BufSndPntr->directSoundBuffer;
  if (DSBuffer == NULL)
    goto ErrorExit;

  if (VolumePercentage > 100)
    VolumePercentage = 100;

  DSErrorCode = DSBuffer->SetVolume (
    nLogVolume[VolumePercentage]);

  if (DSErrorCode != DS_OK)
    goto ErrorExit;

  ReturnCode = TRUE;

ErrorExit:
#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif
  return ReturnCode;
}



/*****************************************************************************
 * Set the pitch of an individual sound.  The given frequency is in hertz,
 * samples per second.  Use zero to set the pitch back to the default for
 * the sound.  Values larger than the original recording Hz will make your
 * sound higher pitched.
 */

BOOL LE_SOUND_SetPitchBufSnd (LE_SOUND_BufSndHandle BufSndHandle, UNS32 Hertz)
{
  BufSndPointer       BufSndPntr;
  PDSBUFFER           DSBuffer;
  HRESULT             DSErrorCode;
  BOOL                ReturnCode = FALSE;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  BufSndPntr = (BufSndPointer) BufSndHandle;

  if (BufSndPntr == NULL)
    goto ErrorExit;

  DSBuffer = BufSndPntr->directSoundBuffer;
  if (DSBuffer == NULL)
    goto ErrorExit;

  if (Hertz > DSBFREQUENCY_MAX)
    Hertz = DSBFREQUENCY_MAX;
  else if (Hertz == 0)
    Hertz = DSBFREQUENCY_ORIGINAL;
  else if (Hertz < DSBFREQUENCY_MIN)
    Hertz = DSBFREQUENCY_MIN;

  DSErrorCode = DSBuffer->SetFrequency (Hertz);

  if (DSErrorCode != DS_OK)
    goto ErrorExit;

  ReturnCode = TRUE;

ErrorExit:
#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif
  return ReturnCode;
}



/*****************************************************************************
 * Set the position in 3D space of a 3D sound.  Doesn't work with non-3D
 * sounds.  Returns TRUE if successful.
 */

BOOL LE_SOUND_Set3DPositionBufSnd (LE_SOUND_BufSndHandle BufSndHandle,
  TYPE_Point3DPointer SpatialPosition)
{
#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  // Bleeble...

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif
  return FALSE;
}



/*****************************************************************************
 * Adds the given sound to the permanent cache of sounds - a fully
 * decompressed DirectSound buffer is held ready to play it at any time.
 */

BOOL LE_SOUND_CacheBufSnd (LE_DATA_DataId WaveDataID)
{
#if CE_ARTLIB_SoundMaxCachedDirectSounds > 0
  LE_SOUND_AudioStreamRecord  AudioStream;

  memset (&AudioStream, 0, sizeof (AudioStream));

  if (!LE_SOUND_OpenAudioStream (&AudioStream, NULL /* File Name */,
  WaveDataID, FALSE /* 3D Sound */, TRUE /* Start paused */,
  TRUE /* Keep permanently cached */))
    return FALSE;

  LE_SOUND_CloseAudioStream (&AudioStream);

  return TRUE;
#else // Caching not supported in this compile.
  return FALSE;
#endif
}



/*****************************************************************************
 * Removes the permanent cache option from the given sound DataID, so it
 * can have its memory reused for other sounds (but leaves it in the cache
 * in case you want to use it again).
 */

void LE_SOUND_UncacheBufSnd (LE_DATA_DataId WaveDataID)
{
#if CE_ARTLIB_SoundMaxCachedDirectSounds > 0
  CachePointer  CachePntr;
  int           CacheIndex;

  for (CacheIndex = 0, CachePntr = CacheArray + 0;
  CacheIndex < CE_ARTLIB_SoundMaxCachedDirectSounds;
  CacheIndex++, CachePntr++)
  {
    if (CachePntr->dataID == WaveDataID)
      CachePntr->permanentlyCached = FALSE;
  }
#endif
}



/*****************************************************************************
 * Remove all cached sounds.  Useful to do if you switch data files,
 * otherwise the same DataID may refer to a different sound than what's
 * in the cache.
 */

void LE_SOUND_FlushCachedSounds (void)
{
#if LE_CMAIN_SoundMaxCachedDirectSounds > 0

  CachePointer  CachePntr;
  int           i;

  for (i = 0; i < LE_CMAIN_SoundMaxCachedDirectSounds; i++)
  {
    CachePntr = CacheArray + i;
    if (CachePntr->dataID == LE_DATA_EmptyItem ||
    CachePntr->inUse || CachePntr->permanentlyCached)
      continue; // Can't flush this one.

    CachePntr->dataID = LE_DATA_EmptyItem;
    if (CachePntr->DSBuffer != NULL)
      CachePntr->DSBuffer->lpVtbl->Release (CachePntr->DSBuffer);
    CachePntr->DSBuffer = NULL;
  }
#endif // LE_CMAIN_SoundMaxCachedDirectSounds
}



/*****************************************************************************
 * Read data from the file/dataID and put it into the audio buffers, until
 * the buffers become full.  Puts in silence if the end is reached, unless
 * you do looping by using the jumpTriggerPosition option.  Returns TRUE if
 * successful, FALSE if an error happened.
 */

BOOL LE_SOUND_FeedAudioStream (LE_SOUND_AudioStreamPointer AudioPntr,
  BOOL LoopingRequested)
{
  INT32 AmountToFeed;
  UNS32 Delta;
  INT32 FeedTimeStart;
  INT32 FeedTimeEnd;
  BYTE *InputPntr;
  BOOL  InputUsed;
  BOOL  JumpDone;
  UNS32 NextCurrentPosition;
  BOOL  OutputIsFull;
  UNS32 PreJumpPosition;
  BOOL  ReturnCode = FALSE;

  BYTE  InputBuffer [45000];
  // Last so it doesn't mess up stack caching.  It also holds up to
  // half a second of data, 44100 bytes for stereo 16 bit sound.

  if (AudioPntr == NULL || AudioPntr->bufSnd == NULL)
    goto ErrorExit;

  // Validate the jump positions if it is being used,
  // has to be a multiple of the block alignment.

  if (AudioPntr->jumpCount > 0)
  {
    Delta = AudioPntr->jumpTriggerPosition - AudioPntr->startPosition;
    Delta = Delta / AudioPntr->blockAlignment * AudioPntr->blockAlignment;
    AudioPntr->jumpTriggerPosition = AudioPntr->startPosition + Delta;

    Delta = AudioPntr->jumpToPosition - AudioPntr->startPosition;
    Delta = Delta / AudioPntr->blockAlignment * AudioPntr->blockAlignment;
    AudioPntr->jumpToPosition = AudioPntr->startPosition + Delta;
  }

  if (AudioPntr->doNotFeed)
  {
    // This is a non-spooling sound, which is handled differently from
    // sounds which are fed data (all the sound is already in the
    // DirectSound buffer).

    // Update hardware looping status to match requested one.

    LE_SOUND_LoopNonSpooledBufSnd (AudioPntr->bufSnd, LoopingRequested);

    // Update the current position status.  Fake the current position,
    // so that jumps can use it to set the trigger point.  In spooled
    // sound, the current position is ahead of the playing time, since
    // it marks data going into the buffers, not coming out.  For this
    // non-spooled stuff it is the same as the playing position.

    AudioPntr->playTime = LE_SOUND_GetTimeOfBufSnd (AudioPntr->bufSnd);

    PreJumpPosition = AudioPntr->currentPosition;

    AudioPntr->currentPosition = (UNS32) ((double)
      AudioPntr->playTime / AudioPntr->duration *
      (AudioPntr->endPosition - AudioPntr->startPosition)) +
      AudioPntr->startPosition;

    // Make it a multiple of the block size.

    Delta = AudioPntr->currentPosition - AudioPntr->startPosition;
    Delta = Delta / AudioPntr->blockAlignment * AudioPntr->blockAlignment;
    AudioPntr->currentPosition = AudioPntr->startPosition + Delta;

    // Check for jumps in non-spooled sound.

    if (AudioPntr->jumpCount > 0)
    {
      if (AudioPntr->currentPosition >= AudioPntr->jumpTriggerPosition &&
      PreJumpPosition <= AudioPntr->jumpTriggerPosition)
      {
        // Ok, this jump is happening.

        LE_SOUND_CutToPositionInNonSpooledBufSnd (AudioPntr->bufSnd,
          AudioPntr->jumpToPosition - AudioPntr->startPosition);

        AudioPntr->jumpCount--; // One less jump to do.

        // Update the current time to reflect the new playing position,
        // and avoid confusion if we jump again.

        AudioPntr->playTime = LE_SOUND_GetTimeOfBufSnd (AudioPntr->bufSnd);

        AudioPntr->currentPosition = (UNS32) ((double)
          AudioPntr->playTime / AudioPntr->duration *
          (AudioPntr->endPosition - AudioPntr->startPosition)) +
          AudioPntr->startPosition;

        Delta = AudioPntr->currentPosition - AudioPntr->startPosition;
        Delta = Delta / AudioPntr->blockAlignment * AudioPntr->blockAlignment;
        AudioPntr->currentPosition = AudioPntr->startPosition + Delta;
      }
    }
  }
  else // Feedable stream, stuff in some more data.
  {
    // Do several small (spoon sized?) feedings until the buffers are full.

    while (TRUE)
    {
      // Figure out the start and end of the feed range.  First check if we
      // have hit a jump point (previous feedings leave the current position
      // exactly at the jump point).  Note that jumps are turned off by
      // specifying a zero jump counter.

      PreJumpPosition = AudioPntr->currentPosition;
      JumpDone = FALSE;

      if (AudioPntr->jumpCount > 0 &&
      AudioPntr->currentPosition == (UNS32) AudioPntr->jumpTriggerPosition)
      {
        // Time for a jump.  Check if it is a stupid jump.

        if (AudioPntr->jumpTriggerPosition == AudioPntr->jumpToPosition)
        {
  #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_FeedAudioStream: "
            "Jump trigger %d is same as target position, zero bytes to play!\r\n",
            (int) AudioPntr->jumpTriggerPosition);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
          AudioPntr->jumpCount = 1; // Disable invalid jump info.
        }

        AudioPntr->currentPosition = AudioPntr->jumpToPosition;
        AudioPntr->jumpCount--;
        JumpDone = TRUE;
      }
      else if (LoopingRequested &&
      AudioPntr->currentPosition >= AudioPntr->endPosition)
      {
        // Looping - another kind of jump - back to the beginning.
        // This has lower priority than explicit jumps, so if there
        // is a jump then it happens, not the loop.

        AudioPntr->currentPosition = AudioPntr->startPosition;
      }

      // Find the end of the feed range starting at the current position.  Stop
      // at the end of the file or at the next jump point.

      NextCurrentPosition = AudioPntr->currentPosition + AudioPntr->feedSize;
      if (NextCurrentPosition > AudioPntr->endPosition)
        NextCurrentPosition = AudioPntr->endPosition;

      if (AudioPntr->jumpCount > 0 &&
      AudioPntr->currentPosition < AudioPntr->jumpTriggerPosition &&
      NextCurrentPosition > AudioPntr->jumpTriggerPosition)
        NextCurrentPosition = AudioPntr->jumpTriggerPosition;

      AmountToFeed = NextCurrentPosition - AudioPntr->currentPosition;

      if (AmountToFeed <= 0)
      {
        // Fill the remaining part of the buffer with silence.

        ReturnCode = LE_SOUND_FeedBufSnd (AudioPntr->bufSnd,
          0, NULL, NULL, 0, NULL, NULL, NULL, 0, 0,
          JumpDone ? AudioPntr->jumpCutRequested : FALSE);

        AudioPntr->currentPosition = NextCurrentPosition;
        break; // Done feeding.
      }
      else // Have some data to feed.
      {
        // First read the data needed for feeding.

        if (AudioPntr->fileHandle != NULL)
        {
          if (AmountToFeed > sizeof (InputBuffer))
            AmountToFeed = sizeof (InputBuffer); // Normally shouldn't happen...

          if (AudioPntr->currentPosition != AudioPntr->filePosition)
          {
            if (fseek (AudioPntr->fileHandle,
            AudioPntr->currentPosition, SEEK_SET) != 0)
              goto ErrorExit;
            AudioPntr->filePosition = AudioPntr->currentPosition;
          }

          if (fread (InputBuffer, AmountToFeed, 1, AudioPntr->fileHandle) != 1)
            goto ErrorExit;

          AudioPntr->filePosition += AmountToFeed;
          InputPntr = InputBuffer + 0;
        }
        else // Reading from a DataID.
        {
          InputPntr = (LPBYTE) LE_DATA_Use (AudioPntr->waveDataID);
          if (InputPntr == NULL)
            goto ErrorExit;
          InputPntr += AudioPntr->currentPosition;
        }

        // Now feed the data to the sound system.

        NextCurrentPosition = AudioPntr->currentPosition + AmountToFeed;

        FeedTimeStart = (INT32) (AudioPntr->duration *
          (double) (AudioPntr->currentPosition - AudioPntr->startPosition) /
          (AudioPntr->endPosition - AudioPntr->startPosition));

        FeedTimeEnd = (INT32) (AudioPntr->duration *
          (double) (NextCurrentPosition - AudioPntr->startPosition) /
          (AudioPntr->endPosition - AudioPntr->startPosition));

        if (!LE_SOUND_FeedBufSnd (AudioPntr->bufSnd,
        AmountToFeed, InputPntr, &InputUsed,
        0, NULL, NULL, &OutputIsFull,
        FeedTimeStart, FeedTimeEnd,
        JumpDone ? AudioPntr->jumpCutRequested : FALSE))
          goto ErrorExit;

        if (!InputUsed)
        {
          // It didn't use our input, possibly because it was flushing out
          // some internal buffers.  Try again to be sure (so we don't
          // waste time re-reading the sound data).

          if (!LE_SOUND_FeedBufSnd (AudioPntr->bufSnd,
          AmountToFeed, InputPntr, &InputUsed,
          0, NULL, NULL, &OutputIsFull,
          FeedTimeStart, FeedTimeEnd,
          JumpDone ? AudioPntr->jumpCutRequested : FALSE))
            goto ErrorExit;
        }

        if (InputUsed)
          AudioPntr->currentPosition = NextCurrentPosition;

        if (OutputIsFull)
          break; // Stop feeding when it is full.
      }
    }

    // Update the now-playing value to show the current play time, which
    // varies from 0 to the duration.

    AudioPntr->playTime = LE_SOUND_GetTimeOfBufSnd (AudioPntr->bufSnd);
  }

  ReturnCode = TRUE;
  goto NormalExit;


ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_FeedAudioStream: "
    "Unable to feed data for some reason.\r\n");
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

NormalExit:
  return ReturnCode;
}



/*****************************************************************************
 * Close and deallocate stuff used by an audio stream.
 */

void LE_SOUND_CloseAudioStream (LE_SOUND_AudioStreamPointer AudioPntr)
{
  if (AudioPntr == NULL)
    return;

  if (AudioPntr->bufSnd != NULL)
  {
    LE_SOUND_StopBufSnd (AudioPntr->bufSnd);
    AudioPntr->bufSnd = NULL;
  }

  if (AudioPntr->fileHandle != NULL)
  {
    fclose (AudioPntr->fileHandle);
    AudioPntr->fileHandle = NULL;
  }
}



/*****************************************************************************
 * Creates an audio stream using either a DataID or a file name.  Fills in
 * the provided LE_SOUND_AudioStreamRecord (the caller can set up some
 * initial fields like the jump settings).  Also starts up the spooling by
 * preloading the initial data and putting the sound buffer into a paused
 * state.  Returns TRUE if successful.
 */

BOOL LE_SOUND_OpenAudioStream (LE_SOUND_AudioStreamPointer AudioPntr,
char *FileName, LE_DATA_DataId WaveDataID, BOOL Use3DSound,
BOOL StartInPauseMode, BOOL KeepPermanentlyCached)
{
  struct MyAudioFormatStruct {
    WAVEFORMATEX      waveFormat;
    BYTE              extraData [512];
  };

  DWORD                       ChunkName;
  DWORD                       ChunkSize;
  DWORD                       CurrentOffset;
  DWORD                       DataChunkLength = 0;
  DWORD                      *DWORDPntr = NULL;
  INT32                       FeedDuration;
  FILE                       *InFile = NULL;
  DWORD                       JunkDWORD;
  struct MyAudioFormatStruct  MyAudioFormat;
  BOOL                        NotSpooled;
  INT32                       OutputDuration;
  WAVEFORMATEX                OutputFormat;
  BOOL                        ReturnCode = FALSE;
  DWORD                       RIFFLength;

  if (AudioPntr == NULL ||
  (FileName == NULL && WaveDataID == LE_DATA_EmptyItem))
  {
#if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LE_SOUND_OpenAudioStream: NULL arguments.\r\n",
      LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  // Open the file if one is specified, or load the DataID.

  CurrentOffset = 0;
  memset (&MyAudioFormat, 0, sizeof (MyAudioFormat));
  AudioPntr->fileHandle = NULL;

  if (FileName != NULL && FileName[0] != 0)
  {
    AudioPntr->fileHandle = InFile = fopen (FileName, "rb");
    if (InFile == NULL) goto ErrorExit;

    if (fread (&ChunkName, 4, 1, InFile) != 1)
      goto ErrorExit;

    KeepPermanentlyCached = FALSE; // Can't cache data from files.
  }
  else // Using a DataID.
  {
    AudioPntr->waveDataID = WaveDataID;
    DWORDPntr = (LPDWORD) LE_DATA_Use (WaveDataID);
    if (DWORDPntr == NULL)
      goto ErrorExit;
    ChunkName = *DWORDPntr++;
  }
  CurrentOffset += 4;

  // Start parsing the RIFF file header.

  if (ChunkName != MAKEFOURCC ('R', 'I', 'F', 'F'))
    goto ErrorExit; // Not a RIFF file.

  // Get the size of the whole RIFF.

  if (InFile)
    {if (fread (&ChunkSize, 4, 1, InFile) != 1) goto ErrorExit;}
  else
    ChunkSize = *DWORDPntr++;
  CurrentOffset += 4;
  RIFFLength = ChunkSize + CurrentOffset;

  // Check for the WAVE subtype.

  if (InFile)
    {if (fread (&ChunkName, 4, 1, InFile) != 1) goto ErrorExit;}
  else
    ChunkName = *DWORDPntr++;
  CurrentOffset += 4;

  if (ChunkName != MAKEFOURCC ('W', 'A', 'V', 'E'))
    goto ErrorExit;

  // Hunt for the "fmt " and "data" subchunks.

  while (CurrentOffset < RIFFLength)
  {
    if (InFile)
      {if (fread (&ChunkName, 4, 1, InFile) != 1) goto ErrorExit;}
    else
      ChunkName = *DWORDPntr++;
    CurrentOffset += 4;

    if (InFile)
      {if (fread (&ChunkSize, 4, 1, InFile) != 1) goto ErrorExit;}
    else
      ChunkSize = *DWORDPntr++;
    CurrentOffset += 4;

    if (ChunkName == MAKEFOURCC ('f', 'm', 't', ' '))
    {
      // Copy ChunkSize bytes of wave format data to our in-memory copy.

      if (ChunkSize > sizeof (MyAudioFormat))
        goto ErrorExit; // This wave format is too big for us to handle.

      if (InFile)
        {if (fread (&MyAudioFormat, ChunkSize, 1, InFile) != 1) goto ErrorExit;}
      else
      {
        memcpy (&MyAudioFormat, DWORDPntr, ChunkSize);
        DWORDPntr = (DWORD *) (((BYTE *) DWORDPntr) + ChunkSize);
      }
      CurrentOffset += ChunkSize;

      if (ChunkSize & 1) // Skip the pad byte if needed.
      {
        if (InFile)
          {if (fread (&JunkDWORD, 1, 1, InFile) != 1) goto ErrorExit;}
        else
          DWORDPntr = (DWORD *) (((BYTE *) DWORDPntr) + 1);
        CurrentOffset++;
      }
    }
    else if (ChunkName == MAKEFOURCC ('d', 'a', 't', 'a'))
    {
      DataChunkLength = ChunkSize;
      break; // Stop at the data!
    }
    else // Unknown kind of chunk.  Skip it.
    {
      if (ChunkSize & 1)
        ChunkSize++; // Pad length to even.

      // Skip over the contents.

      if (InFile)
        {if (fseek (InFile, ChunkSize, SEEK_CUR)) goto ErrorExit;}
      else
        DWORDPntr = (DWORD *) (((BYTE *) DWORDPntr) + ChunkSize);
      CurrentOffset += ChunkSize;
    }
  }

  if (DataChunkLength == 0 || DataChunkLength + CurrentOffset > RIFFLength)
    goto ErrorExit; // Didn't find the chunks we need or corrupt data.

  // Set the byte positions.

  AudioPntr->startPosition = CurrentOffset;
  AudioPntr->endPosition = CurrentOffset + DataChunkLength;
  AudioPntr->filePosition = CurrentOffset;
  AudioPntr->currentPosition = CurrentOffset;

  // Find the duration.

  if (MyAudioFormat.waveFormat.nAvgBytesPerSec == 0)
    goto ErrorExit; // Need a meaningful value here.

  AudioPntr->duration =
    (INT32) ((double) DataChunkLength *
    (double) CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ /
    (double) MyAudioFormat.waveFormat.nAvgBytesPerSec);

  if (AudioPntr->duration <= 0)
    AudioPntr->duration = 1; // Avoid zero length duration.

  // Calculate the feed size and duration.  Typically want to feed it in
  // half second long chunks of data, about 11K bytes per feeding if
  // the sound is at 22kHz uncompressed.  Several feedings get done,
  // usually a bunch of them every second.  Theoretically the feeds should
  // be done with the block size of the input data, but that would have
  // too much overhead.

  if (AudioPntr->duration > CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ / 2)
    FeedDuration = CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ / 2;
  else
    FeedDuration = AudioPntr->duration;

  AudioPntr->feedSize = FeedDuration *
    MyAudioFormat.waveFormat.nAvgBytesPerSec /
    CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ;

  AudioPntr->feedSize = AudioPntr->feedSize /
    MyAudioFormat.waveFormat.nBlockAlign * MyAudioFormat.waveFormat.nBlockAlign;
  if (AudioPntr->feedSize <= 0)
    AudioPntr->feedSize = MyAudioFormat.waveFormat.nBlockAlign;

  AudioPntr->blockAlignment = MyAudioFormat.waveFormat.nBlockAlign;

  // Make the output format - essentially a decompressed version
  // of the input format.

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

  // Find the size of the DirectSound buffer that actually plays the sound.
  // It holds the entire sound for non-spooled sounds, and is allocated for at
  // most a few seconds of sound (the longest typical delay between animation
  // updates) for spooled sound, or the sound length if it is shorter.

  if (KeepPermanentlyCached)
    NotSpooled = TRUE; // Explicit cached sounds fully loaded into memory, no matter how big.
  else
    NotSpooled = (AudioPntr->duration <= CE_ARTLIB_SoundMaxNonSpooledSound);
  OutputDuration = AudioPntr->duration;

  if (!NotSpooled)
  {
    if (OutputDuration > CE_ARTLIB_SoundSpoolBufferDuration)
      OutputDuration = CE_ARTLIB_SoundSpoolBufferDuration;
  }

  // Open the sound buffer.  Specify slightly more for the buffer sizes
  // since decompressed data expands to slightly more than the time that
  // the average bytes per second says it should.  Also allow it to be
  // cached if the whole sound will fit in the output and is a DataID sound.

  AudioPntr->bufSnd = LE_SOUND_StartBufSnd (
    &MyAudioFormat.waveFormat, &OutputFormat,
    NULL /* File name for audio dump */, Use3DSound,
    (InFile == NULL && NotSpooled) ? WaveDataID : LE_DATA_EmptyItem,
    KeepPermanentlyCached,
    &AudioPntr->usingCachedDataID, NotSpooled,
    FeedDuration + 1, OutputDuration + 1);

  if (AudioPntr->bufSnd == NULL)
    goto ErrorExit;

  // Pause the sound before we start filling it, so that we can get
  // a head start on the spooling or prevent it from playing if it
  // is a non-spooled sound until it has the whole sound.

  LE_SOUND_PauseBufSnd (AudioPntr->bufSnd, TRUE);

  // Fill the buffer with initial data and if it is less than the
  // DirectSound buffer size, pads it with silence to the end.

  if (!AudioPntr->usingCachedDataID)
  {
    if (!LE_SOUND_FeedAudioStream (AudioPntr, FALSE))
      goto ErrorExit;
  }

  // Now that the buffer is loaded (including the silence at the end, which
  // would overwrite the beginning if we did any more feeding), we can turn
  // off feeding for those buffers that don't need it.

  AudioPntr->doNotFeed = NotSpooled;

  // Start it playing!

  if (!StartInPauseMode)
    LE_SOUND_PauseBufSnd (AudioPntr->bufSnd, FALSE);

  // Success!

  ReturnCode = TRUE;
  goto NormalExit;


ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  if (FileName == NULL)
    FileName = "<None>";
  sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_OpenAudioStream: "
    "Unable to open WAVE file named \"%s\" or DataID $%08X.\r\n",
    FileName, (int) WaveDataID);
  LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif

  LE_SOUND_CloseAudioStream (AudioPntr);

NormalExit:
  return ReturnCode;
}



#if CE_ARTLIB_SoundEnableVoiceChat
/*****************************************************************************
 * Voice Chat Design
 *
 * Voice chat works by continuously recording sound (in a small buffer set
 * to looping mode).  The game periodically calls a function which takes
 * the sound accumulated in the buffer and compresses it.  The game then
 * takes the compressed data and sends it over the network to the server
 * which then sends it to all the other players (the server better have a
 * good network connection).  The clients receive the data and call another
 * function which decompresses it and plays it (again using a looping buffer,
 * so if data is missed, it replays old data, and can replay silence).
 *
 * The identity of the players is taken care of by the game, which includes
 * the from and to player numbers in extraneous message data; the chat
 * data doesn't identify players, it only contains sound.  This also helps
 * with security - the game takes care of making sure a player is who he
 * says he is.
 *
 * The sound is transfered in RIFF style data chunks (not fully RIFF), so
 * that future features can be easily added.  Inside the data block, the
 * chunks are in increasing time order (so process them in that order).
 * Various command chunks are also present (start, stop, audible
 * positioning etc) besides the sound.
 *
 * The BufSnd system is used for compressing the sounds, as well as
 * decompressing and playing received sounds.
 *
 * Special compression is done for silence.  If the chat code doesn't detect
 * any sound for a short time (a few seconds), it just stops compressing
 * and sending data.  The receiver just replays the old data it has
 * (it is running in looping mode) which results in the user hearing
 * silence.  When some fresh sound is detected, it is sent with a special
 * marker telling the receiver to start playing it immediately (don't wait
 * for the current loop to finish).
 *
 * Data packet format:
 * - Overall size of the packet is provided by the messaging system, we
 *   just give it a buffer and a buffer length, it gives us the same
 *   sort of thing, so we don't need to encode the total length into
 *   the data, so no all-enclosing RIFF chunk.  Maximum 64K bytes.
 * - Format is just a list of chunks, each with a type and size header
 *   (same format as RIFF chunk headers - size field is size of the data
 *   and does not include the 8 byte header).  Keep on processing the chunks
 *   until the end of the buffer is reached.  Padding to even multiples
 *   is not used (unlike RIFF).
 * - "CHAT" chunk: starts a chat, contains a wave format header subchunk
 *   "fmt " describing the compressed data format (see WAVEFORMATEX) and a
 *   "dims" chunk containing a DWORD with the number of dimensions and a
 *   "volm" with the volume of the sound as a DWORD, 0 to 100
 *   percent loudness.  The default is PCM 11khz mono sound for the format
 *   and 0 dimensions and 100% loudness (overridable by the .INI file).
 * - "STOP" chunk: stops a chat, the receiver should deallocate the sound
 *   buffer used to play the received data.  No contents.
 * - "DATN" chunk: contains plain sound sample data, always a multiple
 *   of the nBlockAlign value from the wave format header.
 * - "DAT1" chunk: similar to "data" but this one comes after a period of
 *   silence (non-transmission) so that the receiver should flush buffers
 *   and start playing it immediately (use CutToThisFeedRightNow feature).
 * - "POSN" chunk: sets the position of the sound.  The chunk contains
 *   from 1 to 3 floating point numbers (depending on dimensionality),
 *   each a single precision 4 byte value.  They correspond to the X, Y
 *   and Z position of the sound.
 ****************************************************************************/


/*****************************************************************************
 * LE_SOUND_ChatSendBufferCallbackPntr is a pointer to the user provided
 * callback function which is called whenever there is data to send to
 * other computers.  It should send the buffer, copying out the contents
 * if needed since the buffer will be reused by the sound code after the
 * callback returns.  The callback function should take care of all the
 * addressing needed, so that the receiver can figure out who it came
 * from.  If the ImportantDataDoNotDiscard is FALSE then the callback
 * is allowed to discard the data, usually it is discarded when the
 * outgoing message queue is half full (bandwidth running out).  Buffers
 * with the flag set to TRUE should be sent because they contain important
 * information, such as start and stop chat commands.
 */

// See L_Sound.h for the actual typedef, which looks like this:
// typedef BOOL (* LE_SOUND_ChatSendBufferCallbackPntr) (void *Buffer,
//   UNS16 BufferSize, BOOL ImportantDataDoNotDiscard);



/*****************************************************************************
 * Settings that control the voice chat.  Some are constants, others are
 * read in / writen to the game's .INI file in the [Voice Chat] section.
 */

#define LI_SOUND_ChatMaxSilentTicks   (6 * CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ)
// If the user is quiet for this long, stop transmitting.  Should
// be longer than the receive DirectSound buffer so that the last n seconds
// of silence will fill it and remain there as it loops while idle until
// some non-silent data arrives.

float LI_SOUND_ChatSilenceThreshold = 0.05F;
// The average volume level which separates silence from noise. 0.0 means
// everything is noise, 1.0 means all is silence.  Well, maybe average of
// squared volume level.  Whatever, the user just provides 0 to 1.

WAVEFORMATEX LI_SOUND_ChatCaptureBufferFormat;
// Describes the current settings of the buffer used to record the sound,
// note that we convert the data from these settings to the ones for
// precompressed sound.  Ideally these settings will be the same as the
// PreCompress ones, but the audio hardware usually won't do that.

WAVEFORMATEX LI_SOUND_ChatPreCompressFormat =
{
  WAVE_FORMAT_PCM,  // wFormatTag
  1,                // nChannels
  11025,            // nSamplesPerSec
  11025,            // nAvgBytesPerSec
  1,                // nBlockAlign
  8,                // wBitsPerSample
  0                 // cbSize
};
// Sound format for the stage before compression.  We will convert from
// the capture format to this one.  It is always 11025Hz 8 bit mono.

union PostCompressUnion {
  WAVEFORMATEX      waveFormatEx;
  GSM610WAVEFORMAT  waveFormatGSM;
  MPEG1WAVEFORMAT   waveFormatMPEG;
  char              filler [512]; // For other compressions with extra data.
} LI_SOUND_ChatPostCompressFormat;
// Describes the final compression format.  Includes a few extra bytes past
// the usual header for codec specific data.  Generated by using the
// acmFormatSuggest function with LI_SOUND_ChatCodec and LI_SOUND_ChatDataRate.

int LI_SOUND_ChatCodec = WAVE_FORMAT_GSM610;
// Format number which identifies the compression type to use, from the .INI
// file.  The .INI file also has a human readable version of this just for
// user readability.  Use WAVE_FORMAT_PCM for no compression.

int LI_SOUND_ChatDataRate = 2048;
// Number of bytes per second we want the codec to output, some of them have
// a selection of rates (MPEG-3 in particular).  Stored in the .INI file.



/*****************************************************************************
 * Global variables for voice chat.
 */

static LPDIRECTSOUNDCAPTURE                   LI_SOUND_DirectSoundCaptureObject;
static LPDIRECTSOUNDCAPTUREBUFFER             LI_SOUND_DirectSoundCaptureBuffer;
static DWORD                                  LI_SOUND_CaptureBufferSize;
static DWORD                                  LI_SOUND_LastCaptureReadCursor;
static LE_SOUND_BufSndHandle                  LI_SOUND_BufSndForCompression;
static LE_SOUND_ChatSendBufferCallbackPointer LI_SOUND_ChatSendBufferCallbackPntr;
static DWORD                                  LI_SOUND_ChatFeedSerialNumber;
static TYPE_Tick                              LI_SOUND_TimeOfFirstNoise;
static TYPE_Tick                              LI_SOUND_TimeOfLastNoise;
static char                                   LI_SOUND_VoiceChatINISection [] = "Voice Chat";
static BOOL                                   LI_SOUND_ChatEnabledByINI;
static BOOL                                   LI_SOUND_ChatInitialiseRecordBeforePlay = TRUE;
static int                                    LI_SOUND_ChatVolume = 100;

static int                                    LI_SOUND_ChatHeraldSoundType;
static int                                    LI_SOUND_ChatHeraldDelay = 20;
  // The herald feature adds a beep or other noise to the user's sound
  // when talking is detected.  This has the side effect of reducing the
  // problem of looping when the receiver doesn't get data by sending extra
  // data (the beep) to make the feeding be further ahead of the playing.

static int                                    LI_SOUND_ChatPostSilenceDelay = 2 * CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ;
static BYTE                                   LI_SOUND_ChatPostSilenceStorageBuffer [11025];
static DWORD                                  LI_SOUND_ChatPostSilenceStorageSize;
static enum PostSilenceStateEnum {CHAT_SILENCE, CHAT_COLLECTING, CHAT_STREAMING} LI_SOUND_ChatPostSilenceState;
  // The post-silence feature collects a reasonable chunk of sound before
  // the system starts streaming so that it has a head start at filling
  // buffers.  First it starts of in the CHAT_SILENCE state, periodically
  // checking the recording buffer to see if there is any signal (each
  // check involves examining a small piece of sound, ideally 1/4 second
  // or so to see if there is noise, so that we start exactly at the
  // start of the sound).  Once noise is detected, it switches into the
  // CHAT_COLLECTING state where it collects around 1 second of sound
  // and compresses it and stores the compressed data in the global
  // LI_SOUND_ChatPostSilenceStorageBuffer.  The actual amount collected
  // is set by LI_SOUND_ChatPostSilenceDelay (60 represents 1 second).
  // After the desired amount is collected, it gets sent off in one large
  // packet to the receiver and the state moves to CHAT_STREAMING.
  // While streaming, the system sends small blocks of chat data (around
  // 1/4 second big) until LI_SOUND_ChatMaxSilentTicks ticks of silence
  // have been detected and sent (thus the receiver will loop on silence
  // if it doesn't get any fresh data).  Then it moves back to the
  // CHAT_SILENCE state.


struct ReceiveDataStruct
{
  UNS32 playerID;
    // This identifies the player whose chat data is being received.
    // When new data comes in, we look for the player to find the
    // sound buffer to use for playing his data.

  LE_SOUND_BufSndHandle hBufSnd;
    // NULL if this entry isn't in use, otherwise the sound buffer
    // being used to decompress and play the voice chat.

  DWORD lastFeedSerialNumber;
    // time stamp for audio data feeding

  DWORD maxFeedingSize;
    // The internal BufSnd buffer can only hold this much data,
    // usually we set it to 1 second's worth,  so break up
    // incoming data into pieces at most this large.

} LI_SOUND_ReceiveChatArray [CE_ARTLIB_SoundMaxChatsReceivable];
  // An array of structures, one for each chat being actively received.



/*****************************************************************************
 * Go through all format tags that the driver supplies and look for
 * one which matches our desired coded and data rate.
 */

BOOL CALLBACK AcmFormatTagEnumCallback (HACMDRIVERID hadid,
LPACMFORMATTAGDETAILS paftd, DWORD dwInstance, DWORD fdwSupport)
{
  DWORD               FormatSubtypeIndex;
  HACMDRIVER          had;
  MMRESULT            mmr;
  int                 NewDataRateDelta;
  int                 OldDataRateDelta;
  ACMFORMATDETAILS    SubtypeDetails;
  WAVEFORMATEX        wfxSrc;
  struct              MyAudioFormatStruct
  {
    WAVEFORMATEX        waveFormat;
    BYTE                extraData [512];
  }                   wfxDst;

  if ((fdwSupport & ACMDRIVERDETAILS_SUPPORTF_CODEC) == 0)
    return TRUE; // Not a Codec type of tag, ignore it

  if (paftd == NULL)
    return TRUE; // Can't use one with no details.

  // See if this is the kind of codec we want.

  if (paftd->dwFormatTag != (WORD) LI_SOUND_ChatCodec)
    return TRUE; // Wrong compression type.

  // Open the driver so we can query it.

  mmr = acmDriverOpen(&had, hadid, 0L);
  if (mmr) return TRUE;

  // Will this compressor work with the input PCM format?

  wfxSrc = LI_SOUND_ChatPreCompressFormat;
  memset (&wfxDst.waveFormat, 0, sizeof (WAVEFORMATEX));
  wfxDst.waveFormat.nSamplesPerSec = wfxSrc.nSamplesPerSec;
  wfxDst.waveFormat.nChannels = wfxSrc.nChannels;

  // Ask the driver what he would do with the specified input format

  mmr = acmFormatSuggest(had, &wfxSrc, &wfxDst.waveFormat, sizeof (wfxDst),
    ACM_FORMATSUGGESTF_NCHANNELS | ACM_FORMATSUGGESTF_NSAMPLESPERSEC);
  if (mmr || wfxDst.waveFormat.wFormatTag != LI_SOUND_ChatCodec)
  {
    // Something wrong - he won't respond to the query or uses a different format.
    acmDriverClose(had, 0);
    return TRUE;
  }

  /* Go through all the subtypes of the format and find the one which
     matches our input sample rate and also has the lowest data rate.
     Don't care about bits per sample since that reflects the compressed
     bits per sample which will be smaller than the uncompressed one. */

  for (FormatSubtypeIndex = 0; FormatSubtypeIndex < paftd->cStandardFormats;
  FormatSubtypeIndex++)
  {
    memset(&wfxDst, 0, sizeof (wfxDst));
    memset(&SubtypeDetails, 0, sizeof (SubtypeDetails));
    SubtypeDetails.cbStruct       = sizeof (SubtypeDetails);
    SubtypeDetails.dwFormatIndex  = FormatSubtypeIndex;
    SubtypeDetails.dwFormatTag    = LI_SOUND_ChatCodec;
    SubtypeDetails.pwfx           = &wfxDst.waveFormat;
    SubtypeDetails.cbwfx          = sizeof (wfxDst);

    mmr = acmFormatDetails (had, &SubtypeDetails, ACM_FORMATDETAILSF_INDEX);

    if (mmr == 0 &&
    wfxDst.waveFormat.nChannels == LI_SOUND_ChatPreCompressFormat.nChannels &&
    wfxDst.waveFormat.nSamplesPerSec == LI_SOUND_ChatPreCompressFormat.nSamplesPerSec)
    {
      NewDataRateDelta =
        wfxDst.waveFormat.nAvgBytesPerSec - LI_SOUND_ChatDataRate;
      NewDataRateDelta = abs (NewDataRateDelta);

      OldDataRateDelta =
        LI_SOUND_ChatPostCompressFormat.waveFormatEx.nAvgBytesPerSec -
        LI_SOUND_ChatDataRate;
      OldDataRateDelta = abs (OldDataRateDelta);

      if (NewDataRateDelta < OldDataRateDelta &&
      sizeof (WAVEFORMATEX) + wfxDst.waveFormat.cbSize < sizeof (LI_SOUND_ChatPostCompressFormat))
      {
        // Found a better data rate than the last compression format.

        memcpy (&LI_SOUND_ChatPostCompressFormat.waveFormatEx,
          &wfxDst.waveFormat, sizeof (WAVEFORMATEX) + wfxDst.waveFormat.cbSize);
      }
    }
  }

  acmDriverClose(had, 0);

  return TRUE;
}



/*****************************************************************************
 * Callback to examine a single compression codec.
 */

BOOL CALLBACK AcmDriverEnumCallback (HACMDRIVERID hadid,
DWORD dwInstance, DWORD fdwSupport)
{
  HACMDRIVER          DriverHandle;
  MMRESULT            mmr;
  ACMFORMATTAGDETAILS TagDetails;

  if ((fdwSupport & ACMDRIVERDETAILS_SUPPORTF_CODEC) == 0)
    return TRUE; // This isn't a Codec, so ignore it

  DriverHandle = NULL;
  mmr = acmDriverOpen(&DriverHandle, hadid, 0);
  if (mmr)
    return FALSE;

  memset(&TagDetails, 0, sizeof(TagDetails));
  TagDetails.cbStruct = sizeof(TagDetails);

  mmr = acmFormatTagEnum (DriverHandle, &TagDetails,
    AcmFormatTagEnumCallback, 0, 0);

  acmDriverClose(DriverHandle, 0);

  if (mmr != MMSYSERR_NOERROR)
    return FALSE; /* Something went wrong. */

  return (TRUE);
}



/*****************************************************************************
 * Find the post-compression wave format closest to the given data rate.
 * Has to enumerate all ACM (audio compression manager) drivers then
 * enumerate formats and pick through them to find the best one.  Saves
 * the result in LI_SOUND_ChatPostCompressFormat.  Returns TRUE if
 * successful.
 */

static BOOL LI_SOUND_FindPostCompressAudioFormat (void)
{
  LI_SOUND_ChatPostCompressFormat.waveFormatEx = LI_SOUND_ChatPreCompressFormat;

  acmDriverEnum(AcmDriverEnumCallback, 0, 0);

  return
    (LI_SOUND_ChatPostCompressFormat.waveFormatEx.wFormatTag ==
    LI_SOUND_ChatCodec);
}



/*****************************************************************************
 * Initialise things for voice chat.  Gets ready to listen to incoming
 * chats, but doesn't start recording chat.
 */

BOOL LI_SOUND_ChatInit (void)
{
  HRESULT ErrorCode;
  char    FormattedNumber [32];
  char    TempString [256];

  if (LI_SOUND_DirectSoundCaptureObject != NULL)
    return TRUE; // The DS capture object is already created.

  LE_SOUND_ChatOff ();
  LE_SOUND_ChatCloseAll ();

  // Load up default settings from the .INI file.

  LI_SOUND_ChatEnabledByINI = GetPrivateProfileInt (LI_SOUND_VoiceChatINISection,
    "Enable", LI_SOUND_ChatEnabledByINI, LE_DIRINI_INI_PathName);

  sprintf (FormattedNumber, "%f", (double) LI_SOUND_ChatSilenceThreshold);
  GetPrivateProfileString (LI_SOUND_VoiceChatINISection,
    "Silence Threshold", FormattedNumber, TempString, sizeof (TempString),
    LE_DIRINI_INI_PathName);
  sscanf (TempString, "%f", &LI_SOUND_ChatSilenceThreshold);
  if (LI_SOUND_ChatSilenceThreshold <= 0.0)
    LI_SOUND_ChatSilenceThreshold = 0.0;
  else if (LI_SOUND_ChatSilenceThreshold >= 1.0)
    LI_SOUND_ChatSilenceThreshold = 1.0;

  LI_SOUND_ChatCodec = GetPrivateProfileInt (LI_SOUND_VoiceChatINISection,
    "Codec Number", WAVE_FORMAT_GSM610, LE_DIRINI_INI_PathName);

  LI_SOUND_ChatDataRate = GetPrivateProfileInt (LI_SOUND_VoiceChatINISection,
    "Data Rate", 2048, LE_DIRINI_INI_PathName);

  LI_SOUND_ChatHeraldSoundType = GetPrivateProfileInt (LI_SOUND_VoiceChatINISection,
    "Herald Sound Type", LI_SOUND_ChatHeraldSoundType, LE_DIRINI_INI_PathName);

  LI_SOUND_ChatHeraldDelay = GetPrivateProfileInt (LI_SOUND_VoiceChatINISection,
    "Herald Delay", LI_SOUND_ChatHeraldDelay, LE_DIRINI_INI_PathName);
  if (LI_SOUND_ChatHeraldDelay < 0)
    LI_SOUND_ChatHeraldDelay = 0;
  if (LI_SOUND_ChatHeraldDelay > CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ * 2)
    LI_SOUND_ChatHeraldDelay = CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ * 2;

  LI_SOUND_ChatPostSilenceDelay = GetPrivateProfileInt (LI_SOUND_VoiceChatINISection,
    "Post-Silence Delay", LI_SOUND_ChatPostSilenceDelay, LE_DIRINI_INI_PathName);
  if (LI_SOUND_ChatPostSilenceDelay < 0)
    LI_SOUND_ChatPostSilenceDelay = 0;
  if (LI_SOUND_ChatPostSilenceDelay > CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ * 4)
    LI_SOUND_ChatPostSilenceDelay = CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ * 4;

  LI_SOUND_ChatVolume = GetPrivateProfileInt (LI_SOUND_VoiceChatINISection,
    "Volume", LI_SOUND_ChatVolume, LE_DIRINI_INI_PathName);
  if (LI_SOUND_ChatVolume < 0)
    LI_SOUND_ChatVolume = 0;
  if (LI_SOUND_ChatVolume > 100)
    LI_SOUND_ChatVolume = 100;

  LI_SOUND_ChatInitialiseRecordBeforePlay = GetPrivateProfileInt (LI_SOUND_VoiceChatINISection,
    "Initialise Record before Play", LI_SOUND_ChatInitialiseRecordBeforePlay, LE_DIRINI_INI_PathName);

  if (!LI_SOUND_ChatEnabledByINI)
    return FALSE; // Not doing voice chat today.  Exit before DirectSound capture init.

  if (LI_SOUND_DirectSoundObject == NULL /* If this is before play init */ &&
  !LI_SOUND_ChatInitialiseRecordBeforePlay)
    return FALSE; // Don't want to initialise before playback.

  if (!LI_SOUND_FindPostCompressAudioFormat ())
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_ChatOn (): "
      "LI_SOUND_FindPostCompressAudioFormat () fails to suggest a destination waveformat "
      "for our audio data compression.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;  // Unable to suggest a destination waveformat.
  }

  LI_SOUND_ChatCodec = LI_SOUND_ChatPostCompressFormat.waveFormatEx.wFormatTag;
  LI_SOUND_ChatDataRate = LI_SOUND_ChatPostCompressFormat.waveFormatEx.nAvgBytesPerSec;

  // Try to create the DirectSound capture object, representing
  // the audio board doing the capturing.

  ErrorCode = DirectSoundCaptureCreate (NULL, &LI_SOUND_DirectSoundCaptureObject, NULL);
  if ( ErrorCode != DS_OK )
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_SOUND_ChatInit (): "
      "unable to create a DirectSoundCaptureObject.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    return FALSE;
  }

  return TRUE;
}



/*****************************************************************************
 * Shuts down voice chat, safe to call multiple times.
 */

void LI_SOUND_ChatRemove (void)
{
  char TempString [256];

  LE_SOUND_ChatOff ();
  LE_SOUND_ChatCloseAll ();

  if (LI_SOUND_DirectSoundCaptureObject != NULL)
  {
    LI_SOUND_DirectSoundCaptureObject->Release ();
    LI_SOUND_DirectSoundCaptureObject = NULL;
  }

  // Write back the default settings we used.

  sprintf (TempString, "%d", (int) LI_SOUND_ChatEnabledByINI);
  WritePrivateProfileString (LI_SOUND_VoiceChatINISection,
    "Enable", TempString, LE_DIRINI_INI_PathName);

  // Rest of settings aren't needed if disabled.

  if (!LI_SOUND_ChatEnabledByINI)
    return;

  sprintf (TempString, "%f", (double) LI_SOUND_ChatSilenceThreshold);
  WritePrivateProfileString (LI_SOUND_VoiceChatINISection,
    "Silence Threshold", TempString, LE_DIRINI_INI_PathName);

  sprintf (TempString, "%d", (int) LI_SOUND_ChatCodec);
  WritePrivateProfileString (LI_SOUND_VoiceChatINISection,
    "Codec Number", TempString, LE_DIRINI_INI_PathName);

  sprintf (TempString, "%d", (int) LI_SOUND_ChatDataRate);
  WritePrivateProfileString (LI_SOUND_VoiceChatINISection,
    "Data Rate", TempString, LE_DIRINI_INI_PathName);

  sprintf (TempString, "%d", (int) LI_SOUND_ChatHeraldSoundType);
  WritePrivateProfileString (LI_SOUND_VoiceChatINISection,
    "Herald Sound Type", TempString, LE_DIRINI_INI_PathName);

  sprintf (TempString, "%d", (int) LI_SOUND_ChatHeraldDelay);
  WritePrivateProfileString (LI_SOUND_VoiceChatINISection,
    "Herald Delay", TempString, LE_DIRINI_INI_PathName);

  sprintf (TempString, "%d", (int) LI_SOUND_ChatPostSilenceDelay);
  WritePrivateProfileString (LI_SOUND_VoiceChatINISection,
    "Post-Silence Delay", TempString, LE_DIRINI_INI_PathName);

  sprintf (TempString, "%d", (int) LI_SOUND_ChatVolume);
  WritePrivateProfileString (LI_SOUND_VoiceChatINISection,
    "Volume", TempString, LE_DIRINI_INI_PathName);

  sprintf (TempString, "%d", (int) LI_SOUND_ChatInitialiseRecordBeforePlay);
  WritePrivateProfileString (LI_SOUND_VoiceChatINISection,
    "Initialise Record before Play", TempString, LE_DIRINI_INI_PathName);
}



/*****************************************************************************
 *
 *  Start chatting.  Things are allocated and the sound hardware starts
 *  recording the microphone.  Please call ChatSend periodically to empty
 *  out the recorded data.  This is usually called when the user hits the
 *  "start talking" button.  Or you can call it once and leave chat on all
 *  the time.
 *
 *  This includes selecting the audio compressor Codec and other aspects
 *  of the wave formats to use for capture and transmission.  Normally
 *  the settings are held in the game's .INI file (see LE_DIRINI_INI_PathName)
 *  but if there are none, defaults will be used (GSM/11khz) and they will
 *  be written to the file.
 *
 *  The PositionDimensionality specifies where you want the sound to appear
 *  when listened to:
 *    0D means center it.  Note that we always transmit monophonic sounds.
 *    1D means that you can specify the X position, -1.0 for full left,
 *      0.0 for centre, +1.0 for full right, and values inbetween.
 *    2D means specify a 2D world coordinate, the camera position at the
 *      receiver will control the volume and panning.
 *    3D means specify a 3D world coordinate, similar to 2D.
 *
 *  Returns TRUE if successful.  If a previous chat is on, it is turned off.
 *  Usually returns FALSE if the user doesn't have the GSM audio codec
 *  installed.
 *
 *****************************************************************************/

BOOL LE_SOUND_ChatOn (LE_SOUND_ChatSendBufferCallbackPointer CallbackPntr,
int PositionDimensionality)
{
  int             BitsPerSample;
  int             Channels;
  DWORD           DataSize;
  DSCBUFFERDESC   dscbDesc;
  DWORD           dwCapturePos;
  HRESULT         ErrorCode;
  int             Hertz;
  BOOL            ReturnCode = FALSE;
  char            MessageBuffer [sizeof (LI_SOUND_ChatPostCompressFormat) + 256];
  char           *MessagePntr;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  if (LI_SOUND_DirectSoundObject == NULL)
    goto ErrorExit; // Sound system isn't available.

  if (!LI_SOUND_DirectSoundCaptureObject)
    goto ErrorExit;  // The DirectSoundCapture system is not available.

  // Only one chat is active at a time, since we only have one microphone
  // to listen to, and probably only one sound card too.

  LE_SOUND_ChatOff ();

  // Initialise the DirectSoundCapture buffer.  It is set up to record at
  // the same settings as the sound card is using to play sounds (most
  // cards can only record and play at the same speed).  It generally will
  // record CE_ARTLIB_SoundMaxNonSpooledSound time units of sound, and be
  // set up to run in looping mode.  Use the values describing the primary
  // sound playing buffer for initialising the recording buffer, see the
  // LE_SOUND_CurrentHardwareHz and LE_SOUND_CurrentHardwareBits variables.
  // Note that some sound cards only record stereo, so first try mono
  // and then try stereo if it doesn't work.

  for (Hertz = 11025; Hertz <= 44100; Hertz += Hertz)
  {
    for (BitsPerSample = 8; BitsPerSample <= 16; BitsPerSample += 8)
    {
      for (Channels = 1; Channels <= 2; Channels++)
      {
        memset (&LI_SOUND_ChatCaptureBufferFormat, 0, sizeof (LI_SOUND_ChatCaptureBufferFormat));
        LI_SOUND_ChatCaptureBufferFormat.wFormatTag = WAVE_FORMAT_PCM;
        LI_SOUND_ChatCaptureBufferFormat.nChannels = Channels;
        LI_SOUND_ChatCaptureBufferFormat.nSamplesPerSec = Hertz;
        LI_SOUND_ChatCaptureBufferFormat.wBitsPerSample = BitsPerSample;
        LI_SOUND_ChatCaptureBufferFormat.nBlockAlign =
          LI_SOUND_ChatCaptureBufferFormat.nChannels *
          LI_SOUND_ChatCaptureBufferFormat.wBitsPerSample / 8;
        LI_SOUND_ChatCaptureBufferFormat.nAvgBytesPerSec =
          LI_SOUND_ChatCaptureBufferFormat.nBlockAlign *
          LI_SOUND_ChatCaptureBufferFormat.nSamplesPerSec;

        memset (&dscbDesc, 0, sizeof (dscbDesc));
        dscbDesc.dwSize = sizeof (dscbDesc);
        dscbDesc.dwFlags = DSCBCAPS_WAVEMAPPED;

        // Capture at most 4 seconds in advance.  Should be long enough
        // to span delays in the game calling our update function.

        LI_SOUND_CaptureBufferSize =
          LI_SOUND_ChatCaptureBufferFormat.nAvgBytesPerSec * 4;

        LI_SOUND_CaptureBufferSize =
          LI_SOUND_CaptureBufferSize /
          LI_SOUND_ChatCaptureBufferFormat.nBlockAlign *
          LI_SOUND_ChatCaptureBufferFormat.nBlockAlign;

        dscbDesc.dwBufferBytes = LI_SOUND_CaptureBufferSize;
        dscbDesc.lpwfxFormat = &LI_SOUND_ChatCaptureBufferFormat;

        // create a DS capture buffer
        ErrorCode = LI_SOUND_DirectSoundCaptureObject->CreateCaptureBuffer (&dscbDesc,
          &LI_SOUND_DirectSoundCaptureBuffer, NULL);
        if ( ErrorCode == DS_OK )
          break; // The LI_SOUND_DirectSoundCaptureBuffer is successfully created.
      }
      if ( ErrorCode == DS_OK )
        break; // Pass out the result.
    }
    if ( ErrorCode == DS_OK )
      break; // Pass out the result.
  }

  if ( ErrorCode != DS_OK || LI_SOUND_DirectSoundCaptureBuffer == NULL )
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_ChatOn (): "
      "LI_SOUND_DirectSoundCaptureObject->CreateCaptureBuffer "
      "failed.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;  // Unable to create a DS capture buffer
  }

  // Create BufSnd for compressing.

  LI_SOUND_BufSndForCompression = LE_SOUND_StartBufSnd (
    &LI_SOUND_ChatPreCompressFormat, // input audio format
    &LI_SOUND_ChatPostCompressFormat.waveFormatEx, // outputFormat.format
    NULL,                           // outfilename
    FALSE,                          // Use 3D sound
    LED_EI,                         // LE_DATA_DataId CacheHintDataID
    FALSE,                          // BOOL KeepPermanentlyInCache
    NULL,                           // BOOL *UsingCachedBuffer
    FALSE,                          // BOOL SoundFitsInBuffer
    CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ / 3, // Internal buffer length in ticks, feed at most 1/3 second of data at a time.
    0 // Direct sound buffer size
    );

  if (!LI_SOUND_BufSndForCompression)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_ChatOn (): "
      "unable to start the bufsnd system.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;  // unable to start the bufsnd system
  }

  // Start recording.

  ErrorCode = LI_SOUND_DirectSoundCaptureBuffer->Start (DSCBSTART_LOOPING);
  if (ErrorCode != DS_OK)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_ChatOn (): "
      "unable to start the DirectSoundCaptureBuffer.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;  // Unable to start the DS capture buffer.
  }

  // Send a message to the other end telling them that the chat has
  // started.  Include the audio settings needed to decode the data.

  LI_SOUND_ChatSendBufferCallbackPntr = CallbackPntr;
  MessagePntr = MessageBuffer;

  // CHAT chunk header.

  * (LPDWORD) MessagePntr = MAKEFOURCC ('C', 'H', 'A', 'T');
  MessagePntr += 4;
  * (LPDWORD) MessagePntr =
    3 /* Number of subheaders: fmt, dims, volm */ * 8 /* 4 byte ID + 4 byte size */ +
    sizeof (WAVEFORMATEX) + LI_SOUND_ChatPostCompressFormat.waveFormatEx.cbSize +
    sizeof (DWORD) /* For dims argument */ +
    sizeof (DWORD) /* For volm argument */;
  MessagePntr += 4;

  // Chunk 'fmt ' with the compressed audio format.

  * (LPDWORD) MessagePntr = MAKEFOURCC ('f', 'm', 't', ' ');
  MessagePntr += 4;
  DataSize = sizeof (WAVEFORMATEX) + LI_SOUND_ChatPostCompressFormat.waveFormatEx.cbSize;
  * (LPDWORD) MessagePntr = DataSize;
  MessagePntr += 4;
  memcpy (MessagePntr, &LI_SOUND_ChatPostCompressFormat.waveFormatEx, DataSize);
  MessagePntr += DataSize;

  // Chunk 'dims' with dimensionality.

  * (LPDWORD) MessagePntr = MAKEFOURCC ('d', 'i', 'm', 's');
  MessagePntr += 4;
  DataSize = sizeof (DWORD);
  * (LPDWORD) MessagePntr = DataSize;
  MessagePntr += 4;
  * (LPDWORD) MessagePntr = PositionDimensionality;
  MessagePntr += DataSize;

  // Chunk 'volm' with volume level.

  * (LPDWORD) MessagePntr = MAKEFOURCC ('v', 'o', 'l', 'm');
  MessagePntr += 4;
  DataSize = sizeof (DWORD);
  * (LPDWORD) MessagePntr = DataSize;
  MessagePntr += 4;
  * (LPDWORD) MessagePntr = LI_SOUND_ChatVolume;
  MessagePntr += DataSize;

  // Send the startup message across the network.

  if (!LI_SOUND_ChatSendBufferCallbackPntr (MessageBuffer,
  MessagePntr - MessageBuffer, TRUE /* Important */))
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_ChatOn (): "
      "unable to send the initial CHAT message.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit;
  }

  // Start up in the listening to silence state, as soon as there is some
  // noise it will move to the collecting state and then when it has collected
  // enough, it will start streaming.

  LI_SOUND_ChatPostSilenceState = CHAT_SILENCE;

  // Set the timer to the distant past so that it doesn't start transmitting
  // sound as soon as the chat is turned on.  There has to be some noise
  // first.  Otherwise, in network games, everybody could start transmitting
  // volumous data when the game starts up, swamping important messages.

  LI_SOUND_TimeOfLastNoise = 0;
  LI_SOUND_TimeOfFirstNoise = 0;

  // Flush out the recording buffer in case it has leftovers or starts
  // in the middle.

  if (DS_OK != LI_SOUND_DirectSoundCaptureBuffer->
  GetCurrentPosition (&dwCapturePos, &LI_SOUND_LastCaptureReadCursor))
    LI_SOUND_LastCaptureReadCursor = 0; // Couldn't get current position.

  ReturnCode = TRUE; // Successful.
  goto NormalExit;


ErrorExit: // Deallocate everything we allocated.

  if (LI_SOUND_DirectSoundCaptureBuffer)
  {
    LI_SOUND_DirectSoundCaptureBuffer->Stop ();
    LI_SOUND_DirectSoundCaptureBuffer->Release ();
    LI_SOUND_DirectSoundCaptureBuffer = NULL;
  }

  if (LI_SOUND_BufSndForCompression)
  {
    LE_SOUND_StopBufSnd (LI_SOUND_BufSndForCompression);
    LI_SOUND_BufSndForCompression = NULL;
  }

  LI_SOUND_ChatSendBufferCallbackPntr = NULL;

NormalExit:

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif

  return ReturnCode;
}



/*****************************************************************************
 * Stop chatting.  Usually called when the user turns off voice chat
 * transmission; reception isn't affected by this.  Just deallocates all
 * recording resources and sends a stop packet.
 */

BOOL LE_SOUND_ChatOff (void)
{
  char    MessageBuffer [256];
  char   *MessagePntr;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  // Send a STOP command and stop using the callback function.

  MessagePntr = MessageBuffer;
  * (LPDWORD) MessagePntr = MAKEFOURCC ('S', 'T', 'O', 'P');
  MessagePntr += 4;
  * (LPDWORD) MessagePntr = 0; // No data.
  MessagePntr += 4;

  if (LI_SOUND_ChatSendBufferCallbackPntr != NULL)
  {
    LI_SOUND_ChatSendBufferCallbackPntr (MessageBuffer,
      MessagePntr - MessageBuffer, TRUE /* Important */);

    LI_SOUND_ChatSendBufferCallbackPntr = NULL;
  }

  // Deallocate the sound capture buffer and BufSnd used for compression.

  if (LI_SOUND_DirectSoundCaptureBuffer)
  {
    LI_SOUND_DirectSoundCaptureBuffer->Stop ();
    LI_SOUND_DirectSoundCaptureBuffer->Release ();
    LI_SOUND_DirectSoundCaptureBuffer = NULL;
  }

  if (LI_SOUND_BufSndForCompression)
  {
    LE_SOUND_StopBufSnd (LI_SOUND_BufSndForCompression);
    LI_SOUND_BufSndForCompression = NULL;
  }

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif

  return TRUE; // Successful.
}



/*****************************************************************************
 * Compress the accumulated sound data and send it.  The caller can provide
 * a position for games with moving sounds.  Returns FALSE if an error
 * happened.  TRUE if successful (includes doing nothing).
 */

BOOL LE_SOUND_ChatSend (float X, float Y, float Z)
{
  LPBYTE  lpAudioPtr1;
  DWORD   dwAudioBytes1;
  LPBYTE  lpAudioPtr2;
  DWORD   dwAudioBytes2;
  DWORD   dwCapturePos;
  DWORD   dwReadPos;
  DWORD   dwNumBytes;
  DWORD   dwNumSamples;
  DWORD   dwCaptureDataMinimumBytes;
  DWORD   dwCaptureDataMaximumBytes;

  DWORD   AdditionalCompressedAmount;
  DWORD   AmountFed;
  DWORD   CaptureAmountUsed;
  DWORD   CompressedAmount;
  BYTE    CompressedDataBuffer [10008]; // Assume 5:1 compression ratio, plus 8 byte header.
  HRESULT ErrorCode;
  BOOL    InputUsed;
  BYTE   *MessagePntr;
  BOOL    ReturnCode = FALSE;
  BYTE    WorkBuffer [50000];

  if (LI_SOUND_DirectSoundCaptureBuffer == NULL)
    return FALSE; // Need to initialize and start chat first.

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  // See if enough data has accumulated, need at least
  // 1/4 second's worth of sound, about 3K of data otherwise
  // the overhead of starting a compress cycle is too much.
  // Also, if we do sound level detection on a larger buffer then
  // there is more silence and we may miss the start of the
  // conversation (if the buffer contains "hello" and 3 seconds
  // of silence then it will mostly show up as silence).

  dwCaptureDataMinimumBytes =
    LI_SOUND_ChatCaptureBufferFormat.nAvgBytesPerSec / 4;

  dwCaptureDataMaximumBytes = sizeof (WorkBuffer);

  ErrorCode = LI_SOUND_DirectSoundCaptureBuffer->
    GetCurrentPosition (&dwCapturePos, &dwReadPos);
  if (ErrorCode != DS_OK)
  {
    #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_ChatSend (): "
      "the GetCurrentPosition method failed.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
    goto ErrorExit; // unable to get the current read position
  }

  dwNumBytes = dwReadPos - LI_SOUND_LastCaptureReadCursor;
  if (dwReadPos < LI_SOUND_LastCaptureReadCursor)
    dwNumBytes += LI_SOUND_CaptureBufferSize; // Wrapped around the buffer end.

  if (dwNumBytes < dwCaptureDataMinimumBytes)
  {
    ReturnCode = TRUE;
    goto ErrorExit;  // still no enough data captured, try next time.
  }

  // Copy the sound data from the capture buffer into WorkBuffer,
  // at most 50K since we don't want to overdo it and make the
  // calling game too slow.  Since this is called about 4 times
  // a second, the worst case is 176K of data for 1 second of
  // sound (44khz, stereo, 16 bit) allows for a missed update
  // once in a while.

  if (dwNumBytes > dwCaptureDataMaximumBytes)
    dwNumBytes = dwCaptureDataMaximumBytes;

  // Always read a multiple of the sample block size so we don't
  // have to worry about partial bits of data.

  dwNumSamples = dwNumBytes / LI_SOUND_ChatCaptureBufferFormat.nBlockAlign;
  dwNumBytes = dwNumSamples * LI_SOUND_ChatCaptureBufferFormat.nBlockAlign;

  // lock the capture buffer and read off the data
  ErrorCode = LI_SOUND_DirectSoundCaptureBuffer->Lock (
    LI_SOUND_LastCaptureReadCursor,
    dwNumBytes,
    (LPVOID *) &lpAudioPtr1,
    &dwAudioBytes1,
    (LPVOID *) &lpAudioPtr2,
    &dwAudioBytes2,
    0 /* dwFlags */);

  if ( ErrorCode != DS_OK )
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_ChatSend (): "
      "unable to lock down the DS capture buffer.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    goto ErrorExit; // unable to lock the DS capture buffer
  }

  if (lpAudioPtr1 != NULL)
  {
    memcpy (WorkBuffer, lpAudioPtr1, dwAudioBytes1);

    if (lpAudioPtr2 != NULL)
      memcpy (WorkBuffer + dwAudioBytes1, lpAudioPtr2, dwAudioBytes2);
  }

  LI_SOUND_DirectSoundCaptureBuffer->Unlock (lpAudioPtr1, dwAudioBytes1,
    lpAudioPtr2, dwAudioBytes2);

  // Update the read position.

  LI_SOUND_LastCaptureReadCursor += dwNumBytes;
  if (LI_SOUND_LastCaptureReadCursor >= LI_SOUND_CaptureBufferSize)
    LI_SOUND_LastCaptureReadCursor -= LI_SOUND_CaptureBufferSize;

  // Convert the sound from our capture format to the precompress
  // format (which is 11025hz, 8 bit, mono).  First convert stereo
  // into mono.

  if (LI_SOUND_ChatCaptureBufferFormat.nChannels >= 2)
  {
    if (LI_SOUND_ChatCaptureBufferFormat.wBitsPerSample >= 16)
    {
      WORD *DestPntr;
      DWORD i;
      WORD *SourcePntr;

      DestPntr = (LPWORD) WorkBuffer;
      SourcePntr = (LPWORD) WorkBuffer;

      for (i = 0; i < dwNumSamples; i++)
      {
        *DestPntr++ = *SourcePntr++;
        SourcePntr++; // Skip the other channel.
      }
    }
    else // Captured in 8 bit, convert stereo to mono.
    {
      BYTE *DestPntr;
      DWORD i;
      BYTE *SourcePntr;

      DestPntr = WorkBuffer;
      SourcePntr = WorkBuffer;

      for (i = 0; i < dwNumSamples; i++)
      {
        *DestPntr++ = *SourcePntr++;
        SourcePntr++; // Skip the other channel.
      }
    }
  }

  // Convert from 16 bit to 8 bit samples.

  if (LI_SOUND_ChatCaptureBufferFormat.wBitsPerSample >= 16)
  {
    BYTE         *DestPntr;
    DWORD         i;
    signed short *SourcePntr;

    DestPntr = WorkBuffer;
    SourcePntr = (signed short *) WorkBuffer;

    for (i = 0; i < dwNumSamples; i++)
    {
      *DestPntr++ = (BYTE) (((*SourcePntr++) / 256) + 128);
    }
  }

  // Drop the samples per second from whatever to 11025.

  if (LI_SOUND_ChatCaptureBufferFormat.nSamplesPerSec >= 22050)
  {
    BYTE *DestPntr;
    int   Divisor;
    BYTE *EndPntr;
    BYTE *SourcePntr;

    Divisor = LI_SOUND_ChatCaptureBufferFormat.nSamplesPerSec / 11025;
    Divisor--; // Now number of samples to skip, at least 1.

    DestPntr = WorkBuffer;
    SourcePntr = WorkBuffer;
    EndPntr = SourcePntr + dwNumSamples;
    dwNumSamples = 0;

    while (SourcePntr < EndPntr)
    {
      *DestPntr++ = *SourcePntr++;
      SourcePntr += Divisor;
      dwNumSamples++;
    }
  }

  dwNumBytes = dwNumSamples; // Since each sample is now 1 byte long.

  // Check for silence.  Subsample the data to find the average sound
  // level, use the sum of absolute values and then divide by the number
  // of subsamples done and normalise to 0.0 to 1.0.  If it is above a
  // threshold then it is noisy, otherwise it is silent.  Note that 16
  // bit samples are signed values and 8 bit ones are offset-128 values
  // (subtract 0x80 in a signed byte variable to get the signed value
  // from -128 to +127).

  {
    float       Average;
    int         Count;
    BYTE       *EndPntr;
    signed char Sample;
    BYTE       *SourcePntr;
    int         Total;

    SourcePntr = WorkBuffer;
    EndPntr = SourcePntr + dwNumSamples;
    Total = 0;
    Count = 0;

    while (SourcePntr < EndPntr)
    {
      Sample = *SourcePntr;
      SourcePntr += 16; // Subsample to save computational time.
      Sample += -128; // Convert from zero == 128 to a signed value from -128 to +127.
      if (Sample < 0)
        Total -= Sample;
      else
        Total += Sample;
      Count++;
    }

    if (Count > 0)
    {
      Average = (float) Total;
      Average /= Count;
      Average /= 128.0F;
    }
    else
      Average = 0.0F;

    if (Average > LI_SOUND_ChatSilenceThreshold)
      LI_SOUND_TimeOfLastNoise = LE_TIME_TickCount;
    else if (LE_TIME_TickCount - LI_SOUND_TimeOfLastNoise > LI_SOUND_ChatMaxSilentTicks)
    {
      // Throwaway excess silence, don't bother compressing or sending.
      // Note that silence before the max silence time is up will
      // get transmitted, so that the other end gets silence filling
      // up its looping playback buffer.

      LI_SOUND_ChatPostSilenceState = CHAT_SILENCE;
      ReturnCode = TRUE;
      goto ErrorExit;
    }
  }

  // If this is the end of the silence and the start of a new line of
  // chatter, start collecting the compressed sound until enough time
  // has gone by.

  if (LI_SOUND_ChatPostSilenceState == CHAT_SILENCE)
  {
    DWORD HeraldSamples;

    HeraldSamples =
      LI_SOUND_ChatHeraldDelay * 11025 / CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ;

    LI_SOUND_ChatPostSilenceState = CHAT_COLLECTING;
    LI_SOUND_ChatPostSilenceStorageSize = 8; // Leave 8 bytes for chunk header.
    LI_SOUND_TimeOfFirstNoise = LI_SOUND_TimeOfLastNoise;

    // It was silent and is now noisy, add a beep to the start of the
    // work buffer, much like the NASA Apollo voice link effects.
    // This also masks a playback startup error where it echos the
    // first half second of sound (you need to send a bit more data than
    // the elapsed time otherwise future feedings will fail and you get
    // the echo as the playback buffer loops around before it can feed
    // again).  Note that we now work around the playback startup problem
    // by collecting more sound before transmitting the first data.

    if (LI_SOUND_ChatHeraldDelay > 0 &&
    sizeof (WorkBuffer) - dwNumBytes > HeraldSamples)
    {
      BYTE *DestPntr;
      int   i;

      memmove (WorkBuffer + HeraldSamples, WorkBuffer, dwNumBytes);
      dwNumSamples += HeraldSamples;
      dwNumBytes = dwNumSamples;
      DestPntr = WorkBuffer;

      if (LI_SOUND_ChatHeraldSoundType == 0)
      {
        // A three tone beep.
        for (i = HeraldSamples / 3; i > 0; i--)
        {
          if ((i & 31) < 16)
            *DestPntr++ = 128 + 20;
          else
            *DestPntr++ = 128 - 20;
        }
        for (i = HeraldSamples / 3; i > 0; i--)
        {
          if ((i & 15) < 8)
            *DestPntr++ = 128 + 20;
          else
            *DestPntr++ = 128 - 20;
        }
        for (i = HeraldSamples / 3; i > 0; i--)
        {
          if ((i & 31) < 16)
            *DestPntr++ = 128 + 20;
          else
            *DestPntr++ = 128 - 20;
        }
      }
      else if (LI_SOUND_ChatHeraldSoundType == 1)
      {
        // A siren like rising tone.
        for (i = HeraldSamples; i > 0; i--)
        {
          BYTE j;

          j = (BYTE) (i + i * i / 8192);
          if ((j & 31) < 16)
            *DestPntr++ = 128 + 20;
          else
            *DestPntr++ = 128 - 20;
        }
      }
      else
      {
        // Just silence.
        for (i = HeraldSamples; i > 0; i--)
          *DestPntr++ = 128;
      }
    }
  }

  // Compress the sound (feed it to our BufSnd), send the resulting data
  // to the network.  Repeat until all the data is fed, this can result
  // in several transmitted packets.  Remember to send it as a DAT1 chunk
  // if it is the first one after we stopped sending data due to silence
  // detection.

  CaptureAmountUsed = 0;
  CompressedAmount = 8; // Skip past future chunky header.

  while (CaptureAmountUsed < dwNumBytes)
  {
    AdditionalCompressedAmount = 0;
    InputUsed = FALSE;

    // Feed at most 1/3 second of data since that's the internal BufSnd buffer
    // size, which means multiple feedings if the recording system has
    // captured more than one second of data.  However, feed slightly less
    // since the compressor might want to hold over fractions of its block
    // size (a few hundred bytes) from the previous feeding.  Also, if not
    // doing compression, only feed as much as our output buffer can hold.

    AmountFed = dwNumBytes - CaptureAmountUsed;
    if (AmountFed > 3000)
      AmountFed = 3000;

    if (!LE_SOUND_FeedBufSnd (
    LI_SOUND_BufSndForCompression,      // bufsnd handle
    AmountFed,                          // InputBufferByteSize
    WorkBuffer + CaptureAmountUsed,     // InputBufferPtr
    &InputUsed,                         // BOOL *InputUsed
    sizeof (CompressedDataBuffer) - CompressedAmount, // DWORD OutputBufferByteSize
    CompressedDataBuffer + CompressedAmount, // BYTE *OutputBufferPtr
    &AdditionalCompressedAmount,        // DWORD *OutputSizeUsed
    NULL,                               // BOOL  *DirectSoundBufferIsFull
    LI_SOUND_ChatFeedSerialNumber,      // StartTime
    LI_SOUND_ChatFeedSerialNumber + 1,  // PastEndTime
    FALSE                               // BOOL CutToThisFeedRightNow
    ))
    {
      #if CE_ARTLIB_EnableDebugMode
      LE_ERROR_DebugMsg ("LE_SOUND_ChatSend (): "
        "LE_SOUND_FeedBufSnd () fails.\n", LE_ERROR_DebugMsgToFile);
      #endif
      goto ErrorExit;  // LE_SOUND_FeedBufSnd () fails
    }

    if (InputUsed)
    {
      CaptureAmountUsed += AmountFed;
      LI_SOUND_ChatFeedSerialNumber++;
    }

    CompressedAmount += AdditionalCompressedAmount;

    if (CompressedAmount > 8 /* Need to have some data in buffer */ &&
    (CompressedAmount >= sizeof (CompressedDataBuffer) / 2 ||
    CaptureAmountUsed >= dwNumBytes /* Last feeding */))
    {
      // Have compressed a reasonable amount of data, or this is the
      // last bit of the batch, either transmit it to make room for more
      // compressed data or store it for later use.

      if (LI_SOUND_ChatPostSilenceState == CHAT_COLLECTING)
      {
        // Collect the compressed data until enough time has passed
        // or the storage buffer is full.

        if (LE_TIME_TickCount - LI_SOUND_TimeOfFirstNoise >=
        (DWORD) LI_SOUND_ChatPostSilenceDelay ||
        CompressedAmount > sizeof (LI_SOUND_ChatPostSilenceStorageBuffer) -
        LI_SOUND_ChatPostSilenceStorageSize)
        {
          // Send the collected data.

          if (LI_SOUND_ChatPostSilenceStorageSize > 8)
          {
            MessagePntr = LI_SOUND_ChatPostSilenceStorageBuffer;
            * (LPDWORD) MessagePntr = MAKEFOURCC ('D', 'A', 'T', '1');
            MessagePntr += 4;
            * (LPDWORD) MessagePntr = LI_SOUND_ChatPostSilenceStorageSize - 8;
            MessagePntr += 4;

            if (LI_SOUND_ChatSendBufferCallbackPntr != NULL)
            {
              LI_SOUND_ChatSendBufferCallbackPntr (LI_SOUND_ChatPostSilenceStorageBuffer,
                (UNS16) LI_SOUND_ChatPostSilenceStorageSize, FALSE /* Important */);
            }
          }

          // Send the extra compressed data too.

          MessagePntr = CompressedDataBuffer;
          if (LI_SOUND_ChatPostSilenceStorageSize > 8)
            * (LPDWORD) MessagePntr = MAKEFOURCC ('D', 'A', 'T', 'N');
          else // Wasn't any compressed data, this is actually the first block.
            * (LPDWORD) MessagePntr = MAKEFOURCC ('D', 'A', 'T', '1');
          MessagePntr += 4;

          * (LPDWORD) MessagePntr = CompressedAmount - 8;
          MessagePntr += 4;

          if (LI_SOUND_ChatSendBufferCallbackPntr != NULL)
          {
            LI_SOUND_ChatSendBufferCallbackPntr (CompressedDataBuffer,
              (UNS16) CompressedAmount, FALSE /* Important */);
          }

          // Switch to streaming state.

          LI_SOUND_ChatPostSilenceState = CHAT_STREAMING;
        }
        else // Add the compressed data to the storage buffer.
        {
          memcpy (LI_SOUND_ChatPostSilenceStorageBuffer +
            LI_SOUND_ChatPostSilenceStorageSize,
            CompressedDataBuffer + 8,
            CompressedAmount - 8);
          LI_SOUND_ChatPostSilenceStorageSize += CompressedAmount - 8;
        }
      }
      else // LI_SOUND_ChatPostSilenceState == CHAT_STREAMING
      {
        // Transmit a data packet with the sound.

        MessagePntr = CompressedDataBuffer;
        * (LPDWORD) MessagePntr = MAKEFOURCC ('D', 'A', 'T', 'N');
        MessagePntr += 4;

        * (LPDWORD) MessagePntr = CompressedAmount - 8;
        MessagePntr += 4;

        if (LI_SOUND_ChatSendBufferCallbackPntr != NULL)
        {
          LI_SOUND_ChatSendBufferCallbackPntr (CompressedDataBuffer,
            (UNS16) CompressedAmount, FALSE /* Important */);
        }
      }

      CompressedAmount = 8; // Start fresh, leave room for chunk headers.
    }
  }

  ReturnCode = TRUE;

ErrorExit:

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif
  return ReturnCode;
}



/*****************************************************************************
 * Decompress and play sound data and other chat control packets.  The Player
 * is a 32 bit value which identifies one of the active players (usually
 * an IP address or DirectPlay player number).  Returns FALSE if it
 * can't process the message (max conversations reached etc).
 */

BOOL LE_SOUND_ChatReceive (void *Buffer, UNS16 BufferSize, UNS32 PlayerID)
{
  DWORD         AmountFed;
  DWORD         AmountToFeed;
  LPBYTE        BufferEndPntr;
  DWORD         ChunkID;
  DWORD         ChunkSize;
  BOOL          CutBufferNeeded;
  BOOL          CutToFrameAudioFeeding;
  BOOL          DirectSoundBufferIsFull;
  int           EmptyBufferIndex;
  BOOL          InputUsed;
  LPBYTE        MessagePntr;
  WAVEFORMATEX  OutputFormat;
  int           ReceiveBufferIndex;
  struct ReceiveDataStruct *ReceiveChatPntr;
  LPBYTE        SubChunkEndPntr;
  DWORD         SubChunkID;
  DWORD         SubChunkSize;
  LPBYTE        SubMessagePntr;

  // Check for invalid arguments.

  if ( Buffer == NULL || BufferSize <= 0 )
  {
    if ( Buffer == NULL )
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_ChatReceive (): "
        "Buffer == NULL\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    }

    if ( BufferSize <= 0 )
    {
#if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_ChatReceive (): "
        "BufferSize (%d)\n", BufferSize);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    }
    return FALSE;  // invalid input argument(s)
  }

  // Look through the LI_SOUND_ReceiveChatArray for the player.  If
  // it isn't found, look for an empty array element to use (but don't
  // allocate the bufSnd just yet).

  EmptyBufferIndex = -1;

  for (ReceiveBufferIndex = 0;
  ReceiveBufferIndex < CE_ARTLIB_SoundMaxChatsReceivable;
  ReceiveBufferIndex++)
  {
    if (LI_SOUND_ReceiveChatArray [ReceiveBufferIndex].hBufSnd == NULL)
    {
      if (EmptyBufferIndex < 0)
        EmptyBufferIndex = ReceiveBufferIndex;
      continue;
    }

    if (LI_SOUND_ReceiveChatArray [ReceiveBufferIndex].playerID == PlayerID)
      break; // Found it!
  }

  if (ReceiveBufferIndex >= CE_ARTLIB_SoundMaxChatsReceivable)
  {
    ReceiveBufferIndex = -1; // Mark it as not found.
    ReceiveChatPntr = NULL;
  }
  else
    ReceiveChatPntr = LI_SOUND_ReceiveChatArray + ReceiveBufferIndex;

  // Parse the chunky data in the buffer.

  MessagePntr = (LPBYTE) Buffer;
  BufferEndPntr = MessagePntr + BufferSize;

  while (MessagePntr < BufferEndPntr)
  {
    ChunkID = *(LPLONG)MessagePntr;
    MessagePntr += 4;
    ChunkSize = *(LPLONG)MessagePntr;
    MessagePntr += 4;

    switch (ChunkID)
    {
    case MAKEFOURCC ('C', 'H', 'A', 'T'):  // chunk to start a chat

      if (ReceiveBufferIndex >= 0)
      {
        // Stop an existing chat, must have missed the STOP command.

        LE_SOUND_StopBufSnd (LI_SOUND_ReceiveChatArray[ReceiveBufferIndex].hBufSnd);
        LI_SOUND_ReceiveChatArray[ReceiveBufferIndex].hBufSnd = NULL;
        EmptyBufferIndex = ReceiveBufferIndex; // Can reuse this buffer.
        ReceiveBufferIndex = -1;
        ReceiveChatPntr = NULL;
      }

      if (EmptyBufferIndex < 0)
        break; // No more buffers.  Ignore this chunk.

      // Process the subchunks in the chat info.

      SubMessagePntr = MessagePntr;
      SubChunkEndPntr = MessagePntr + ChunkSize;
      if (SubChunkEndPntr > BufferEndPntr)
        return FALSE; // Corrupt data.

      while (SubMessagePntr < SubChunkEndPntr)
      {
        SubChunkID = *(LPLONG)SubMessagePntr;
        SubMessagePntr += 4;
        SubChunkSize = *(LPLONG)SubMessagePntr;
        SubMessagePntr += 4;

        switch (SubChunkID)
        {
        case MAKEFOURCC ('f', 'm', 't', ' '):

          // Found a audio data format chunk, allocate the BufSnd using the
          // given format and activate free receive buffer for this new chat.

          memcpy (&OutputFormat, SubMessagePntr, sizeof (OutputFormat));
          OutputFormat.wFormatTag = WAVE_FORMAT_PCM;  // uncompressed wave format
          OutputFormat.cbSize = 0;
          if (OutputFormat.wBitsPerSample != 16)
            OutputFormat.wBitsPerSample = 8;
          OutputFormat.nBlockAlign =
            OutputFormat.nChannels *
            (OutputFormat.wBitsPerSample / 8);
          OutputFormat.nAvgBytesPerSec =
            OutputFormat.nSamplesPerSec *
            OutputFormat.nBlockAlign;

          LI_SOUND_ReceiveChatArray[EmptyBufferIndex].hBufSnd =
            LE_SOUND_StartBufSnd (
            (LPWAVEFORMATEX) SubMessagePntr, // input audio format
            &OutputFormat,  // outputFormat.format
            NULL,           // outfilename
            FALSE,          // Use 3D sound
            LED_EI,         // LE_DATA_DataId CacheHintDataID
            FALSE,          // BOOL KeepPermanentlyInCache
            FALSE,          // BOOL *UsingCachedBuffer
            FALSE,          // BOOL SoundFitsInBuffer
            CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ / 3, // Internal buffer length in ticks enough for 1/3 second.
            CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ * 4); // Direct sound buffer size short to avoid latency backlog.

          if (LI_SOUND_ReceiveChatArray[EmptyBufferIndex].hBufSnd == NULL)
          {
#if CE_ARTLIB_EnableDebugMode
            sprintf (LE_ERROR_DebugMessageBuffer, "LE_SOUND_ChatReceive (): "
              "unable to start the bufsnd system for player (%d or 0x%0x).\n", PlayerID);
            LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
            return FALSE;  // unable to start the bufsnd system
          }

          // Sucessfully allocated a receive buffer.

          ReceiveBufferIndex = EmptyBufferIndex;
          ReceiveChatPntr = LI_SOUND_ReceiveChatArray + ReceiveBufferIndex;
          EmptyBufferIndex = -1;

          ReceiveChatPntr->playerID = PlayerID;
          ReceiveChatPntr->lastFeedSerialNumber = 0;

          // Set maximum feeding size to 1/4 second of data, rounded down to
          // the block size.  Since our BufSnd has a 1/3 second internal buffer,
          // this gives it a bit of free space for leftovers from previous feeds.

          ReceiveChatPntr->maxFeedingSize =
            ((LPWAVEFORMATEX) SubMessagePntr)->nAvgBytesPerSec / 4;
          if (((LPWAVEFORMATEX) SubMessagePntr)->nBlockAlign > 0)
          {
            ReceiveChatPntr->maxFeedingSize /=
              ((LPWAVEFORMATEX) SubMessagePntr)->nBlockAlign;
            ReceiveChatPntr->maxFeedingSize *=
              ((LPWAVEFORMATEX) SubMessagePntr)->nBlockAlign;
          }
          break;


        case MAKEFOURCC ('d', 'i', 'm', 's'):
          // Dimensionality of the chatter's position in space.
          break;


        case MAKEFOURCC ('v', 'o', 'l', 'm'):
          // Volume to play back the sound at.
          if (ReceiveChatPntr != NULL)
          {
            LE_SOUND_SetVolumeBufSnd (ReceiveChatPntr->hBufSnd,
              (UNS8) * (LPDWORD) SubMessagePntr);
          }
          break;
        }

        SubMessagePntr += SubChunkSize;
      }
      break;


    case MAKEFOURCC ('D', 'A', 'T', 'N'):  // found a data chunk
    case MAKEFOURCC ('D', 'A', 'T', '1'):  // data chunk after a period of silence
      if (ReceiveChatPntr == NULL)
        break; // Sound data from an unknown player, ignore it.

      // start to decompress the audio data and play back the sound

      CutToFrameAudioFeeding = (ChunkID == MAKEFOURCC ('D', 'A', 'T', '1'));
      CutBufferNeeded = CutToFrameAudioFeeding;
      AmountFed = 0;

      if (CutToFrameAudioFeeding)
        LE_SOUND_PauseBufSnd (ReceiveChatPntr->hBufSnd, TRUE);

      while (AmountFed < ChunkSize)
      {
        AmountToFeed = ChunkSize - AmountFed;
        if (AmountToFeed > ReceiveChatPntr->maxFeedingSize)
          AmountToFeed = ReceiveChatPntr->maxFeedingSize;

        InputUsed = FALSE;
        DirectSoundBufferIsFull = FALSE;

        if (!LE_SOUND_FeedBufSnd (
        ReceiveChatPntr->hBufSnd,    // bufsnd handle
        AmountToFeed,                // InputBufferByteSize
        MessagePntr + AmountFed,     // InputBufferPtr
        &InputUsed,                  // BOOL *InputUsed
        NULL,                        // DWORD OutputBufferByteSize
        NULL,                        // BYTE *OutputBufferPtr
        NULL,                        // DWORD *OutputSizeUsed
        &DirectSoundBufferIsFull,    // BOOL  *DirectSoundBufferIsFull
        ReceiveChatPntr->lastFeedSerialNumber, // StartTime
        ReceiveChatPntr->lastFeedSerialNumber + 1, // PastEndTime
        CutBufferNeeded))     // BOOL CutToThisFeedRightNow
        {
          #if CE_ARTLIB_EnableDebugMode
          LE_ERROR_DebugMsg ("LE_SOUND_ChatReceive (): "
            "LE_SOUND_FeedBufSnd () fails.\n", LE_ERROR_DebugMsgToFile);
          #endif
          return FALSE;  // LE_SOUND_FeedBufSnd () fails
        }

        if (InputUsed)
        {
          AmountFed += AmountToFeed;
          CutBufferNeeded = FALSE;
          ReceiveChatPntr->lastFeedSerialNumber++;
        }

        if (DirectSoundBufferIsFull)
          break; // Forget the rest of the data.
      }
      if (CutToFrameAudioFeeding)
        LE_SOUND_PauseBufSnd (ReceiveChatPntr->hBufSnd, FALSE);
      break;


    case MAKEFOURCC ('P', 'O', 'S', 'N'):
      // Position in space of the player, adjust panning or 3D effects.
      break;

    case MAKEFOURCC ('S', 'T', 'O', 'P'):
      if (ReceiveChatPntr != NULL)
      {
        LE_SOUND_StopBufSnd (ReceiveChatPntr->hBufSnd);
        ReceiveChatPntr->hBufSnd = NULL;

        EmptyBufferIndex = ReceiveBufferIndex; // Now have an empty buffer.
        ReceiveBufferIndex = -1;
        ReceiveChatPntr = NULL;
      }
      break;

    } // End switch ChunkID.
    MessagePntr += ChunkSize;
  } // End while (MessagePntr < BufferEndPntr)

  return TRUE;
}



/*****************************************************************************
 * Kill all received chat buffers.
 */

BOOL LE_SOUND_ChatCloseAll (void)
{
  int i;

#if CE_ARTLIB_EnableMultitasking
  if (SoundCSPntr == NULL) return FALSE; // Sound system isn't open.
  EnterCriticalSection (SoundCSPntr);
  if (SoundCSPntr == NULL) return FALSE; // Closed while we were waiting.
#endif

  // Close all sound buffers in LI_SOUND_ReceiveChatArray and mark all
  // array elements as available.

  for (i = 0; i < CE_ARTLIB_SoundMaxChatsReceivable; i++)
  {
    if (LI_SOUND_ReceiveChatArray[i].hBufSnd != NULL)
      LE_SOUND_StopBufSnd (LI_SOUND_ReceiveChatArray[i].hBufSnd);
    LI_SOUND_ReceiveChatArray[i].hBufSnd = NULL;
  }

#if CE_ARTLIB_EnableMultitasking
  LeaveCriticalSection (SoundCSPntr);
#endif

  return TRUE;
}
#endif // CE_ARTLIB_SoundEnableVoiceChat

#endif // if CE_ARTLIB_EnableSystemSound
