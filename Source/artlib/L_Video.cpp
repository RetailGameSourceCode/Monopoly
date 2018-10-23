/*************************************************************
 *
 *   FILE:           L_VIDEO.C
 *   DESCRIPTION:    Main video system source file.
 *
 *   (C) Copyright 1995/96 Artech Digital Entertainments.
 *                         All rights reserved.
 *
 *
 * $Header: /Artlib_99/ArtLib/L_Video.cpp 65    9/21/99 5:01p Lzou $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Video.cpp $
 * 
 * 65    9/21/99 5:01p Lzou
 * A temporary fix for Indeo video 5 codec double video image size feature
 * in RGB32-bit mode. In RGB32-bit mode, we play only the original image
 * size video centred at the main game window.
 * 
 * 64    8/31/99 2:45p Lzou
 * Nothing important.
 * 
 * 63    8/20/99 3:51p Lisac
 * Use same types as prototypes, so it compiles with VC6.
 * 
 * 62    7/21/99 12:50p Lzou
 * Added LE_QUEUE_Flush () in LE_VIDEO_CutVideoToFrame ().
 * 
 * 61    7/14/99 5:35p Lzou
 * Enabled to be able to play an AVI movie which has no audio stream in
 * it. Also be able to play a movie silently. Thus, the player can now
 * play only the video stream or only the audio stream.
 * 
 * 60    6/24/99 3:56p Lzou
 * Added in an option to play only the audio stream. Corrected a potential
 * bug by using pitch instead of screen width.
 * 
 * 59    26/04/99 13:28 Timp
 * Small Unicode fix
 * 
 * 58    4/22/99 15:01 Davew
 * Updated to DX v6
 * 
 * 57    4/14/99 2:56p Lzou
 * Fixed one bug when multitasking mode is enabled. During stopping a
 * video, an important data-buffering thread shut down/shut down done
 * message somehow gets lost due to message queue full. This causes the
 * StopVideo () waits forever for the data-buffering thread ShutDownDone
 * message to come.
 * 
 * 58    4/12/99 5:04p Lzou
 * 
 * 57    4/12/99 2:49p Lzou
 * 
 * 55    3/16/99 12:56p Lzou
 * Have re-implemented CutVideoToFrame quick frame jump action. For the
 * video frame jump playback mode, different approaches are used to
 * protect the main avi data structure from being accessed simultaneously
 * from other threads. Messages are used for the multitasking frame jump
 * mode while a critical section is used for the single-threaded frame
 * jump playback mode.
 * 
 * 54    3/09/99 5:52p Lzou
 * Have resolved the thread safety problem when frame jump playback mode
 * enabled.
 * 
 * 53    3/03/99 5:41p Lzou
 * Reimplemented LE_VIDEO_CutVideoToFrame () and modified
 * LE_VIDEO_SetUpVideoAlternative ().
 * 
 * 52    3/01/99 3:39p Lzou
 * For video frame jump playback mode, enabled fast alternative frame data
 * buffering.
 * 
 * 51    2/26/99 5:34p Lzou
 * Proof reading is done.
 * 
 * 50    2/25/99 2:05p Lzou
 * Fixed one bug in calculating framesToSkip when doing backward frame
 * jumping.
 * 
 * 49    2/25/99 11:53a Lzou
 * Fixed two bugs when the video frame jump playback mode is off.
 * 
 * 48    2/24/99 1:29p Lzou
 * Enabled to force alternative frame data buffering even if the end of
 * movi data chunk is marked. Also modified jump to end of the film.
 * 
 * 47    2/22/99 6:02p Lzou
 * Temporarily check in.
 * 
 * 46    2/19/99 4:47p Lzou
 * Some changes are made to the clearup functions.
 * 
 * 45    2/18/99 2:09p Lzou
 * Implemented video frame jump playback mode. Now, while playing back a
 * video clip sequencially, the user can set up an alternative frame,
 * change the video alternative frame state, forget the already set
 * alternative frame state, and cut to the video alternative frame (do the
 * pending video frame jump action immediately).
 * 
 * 44    2/12/99 5:13p Lzou
 * Have implemented video frame jump playback mode.
 * 
 * 43    2/11/99 3:19p Lzou
 * Renamed video defines and corrected the display of video avi data
 * bufferring states in other screen resolutions.
 * 
 * 42    2/10/99 5:39p Lzou
 * Have enabled video looping mode.
 * 
 * 41    2/10/99 4:00p Lzou
 * Have enabled Indeo video 5 codec feature of filling in a user specified
 * colour if the video background is transparent.
 * 
 * 40    2/09/99 2:48p Lzou
 * Small changes.
 * 
 * 39    2/09/99 1:16p Lzou
 * For simultaneously playing back multiple video clips, I have moved all
 * static variables into the main avi data structure.
 * 
 * 38    2/08/99 1:56p Lzou
 * Some small changes.
 * 
 * 37    2/05/99 3:42p Lzou
 * Small changes.
 * 
 * 36    2/05/99 11:59a Lzou
 * Have updated GetVideoState function.
 * 
 * 35    2/03/99 3:07p Lzou
 * Have modified some part of the code to let the sequencer take over.
 * 
 * 34    2/02/99 5:45p Lzou
 * Have adopted the new AVI video player API.
 * 
 * 33    2/02/99 3:40p Lzou
 * Have adopted the new video API.
 * 
 * 32    2/02/99 11:04a Lzou
 * Have removed avi video player version 2.
 * 
 * 31    1/29/99 4:46p Lzou
 * Have implemented Indeo video 5 specific alpha channel transparent.
 * 
 * 30    1/29/99 4:33p Lzou
 * Fixed one bug in FeedVideo (), when the audio stream is a few frames
 * longer than, or sometimes one frame shorter than the video stream.
 * 
 * 29    1/27/99 1:36p Lzou
 * Have implemented some of Indeo video 5 codec's specific features, such
 * as scalability, colour brightness/contrast/saturation, double image
 * size with/without alternate scanline, fill (or not fill) colour for
 * transparent video images, etc.
 * 
 * 28    1/15/99 12:06p Lzou
 * Have corrected some static variables settting/resetting. Later on, all
 * these static variables will be moved up to the main avi data structure,
 * as we want to be able to play multiple videos simultaneously.
 * 
 * 27    1/14/99 6:11p Lzou
 * Have used a define to enable switching on/off the multi-tasking mode of
 * avi data buffering (version 3 only). Some other small changes are made
 * to the code.
 * 
 * 26    1/13/99 6:39p Lzou
 * One tiny bug is fixed when jumping to the next video key frame (of the
 * same avi data buffer), instead of doing video image prerolling (version
 * 3).
 * 
 * 25    1/13/99 3:43p Lzou
 * Modifications are made to DrawToScreen () (version 3) in searching for
 * the nearest previous video kay frame or the previous decompressed video
 * frame, whichever is nearer for video image prerolling.
 * 
 * 24    1/13/99 12:18p Lzou
 * Some small changes.
 * 
 * 23    1/12/99 6:07p Lzou
 * A bug is fixed in version 3 in deciding when to feed zero data into the
 * buf sound system at the end of a film.
 * 
 * 22    1/08/99 4:04p Lzou
 * Have implement a second thread which is responsible for avi data
 * buffering.
 * 
 * 21    1/07/99 1:47p Lzou
 * Have implemented user specified multiple seconds of avi data buffering.
 * E.g. if the user wants to preload two seconds of avi data for each avi
 * data buffering, he can specify (#define CE_ARTLIB_VideoMultipleSecondsData  2)
 * and the video player does so for each avi data buffering.
 * 
 * 20    1/05/99 2:38p Lzou
 * Have finished versions 2 (audio and video data are buffered separately)
 * and 3 (for each data buffer avi raw data are preloaded at one go). Now,
 * I am doing multi-seconds data buffering for version 3 only.
 * 
 * 19    1/04/99 2:00p Lzou
 * Fixed a bug in FeedVideo () (Version 2 -- video and audio data buffered
 * separately). Code added to handle the situation in which the requested
 * video image format is not supported and ICLocate () fails. Now, if
 * ICLocate () fails, the video player gives a warning message for debug
 * build and draws black images instead of exit.
 * 
 * 18    12/23/98 6:05p Lzou
 * Have implemented a new way of avi data buffering.
 * 
 * 17    12/16/98 11:22a Lzou
 * The new avi video player, version 2, is essentially finished. It still
 * needs a bit fine tuning. According to the tests done on both PII 233
 * and P90, it is better than MS Active Movie Control, at least twice the
 * speed of Sequencer video player (on PII 233), and similar to MS Medio
 * Player. At the moment, the player can work in two modes, in which it
 * can either draw directly to the screen or output decompressed video
 * images for other application to use. 
 * 
 * 16    12/11/98 3:56p Lzou
 * Now the video player is able to judge itself whether it is good to
 * preroll some of the preceding video images or just simply jump to the
 * next key frame.
 * 
 * 15    12/09/98 3:18p Lzou
 * Use streamID instead of chunkID to filter out video/audio/other chunks.
 * 
 * 14    12/08/98 5:03p Lzou
 * A bug is fixed in LE_VIDEO_FeedVideo (). Now, the video player can play
 * very tough video clips with only 3 avi data buffers. The point is that
 * each time when feeding the video, we need to go through all preloaded
 * avi data buffers and check to see if some of them are empty.
 * 
 * 13    12/04/98 3:38p Lzou
 * Have fixed key frame and preroll problem. Now decompressing with
 * HURRYUP flag on works well for both Indeo video 5.04 and Cinepak Codec
 * by Radius.
 * 
 * 12    12/03/98 4:21p Lzou
 * Have modified the code to make the video player be able to play both
 * reblocked and non-reblocked avi files.
 * 
 * 11    12/02/98 3:43p Lzou
 * Try to make the video player be able to work on both reblocked and
 * normal (unreblocked) avi files.
 * 
 * 10    11/30/98 3:08p Lzou
 * Some small changes.
 * 
 * 9     11/27/98 1:23p Lzou
 * Have implemented LE_VIDEO_GetVideoStatus ().
 * 
 * 8     11/26/98 5:22p Lzou
 * Have enabled decompressing with preroll flag on.
 * 
 * 7     11/26/98 2:57p Lzou
 * Modified the main avi data structure to include the output bitmap
 * position/dimensions. Initialise audioFrameNumber to -1 (video start
 * setting), etc.
 * 
 * 6     11/26/98 10:27a Lzou
 * To improve the new video player's performance on P90, ten preloading
 * avi data buffers are used instead of two. And a new video user
 * interface has been adopted.
 * 
 * 5     11/19/98 1:56p Lzou
 * Temporary check in.
 * 
 * 4     11/19/98 1:51p Lzou
 * Before swapping the data buffers, check for nextChunkDataBuffer
 * audio data status.
 *************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemVideo

// Bleeble - insert new video player here.

/****************************************************************************/
/*  I N D E O  V I D E O  5  C O D E C  S P E C I F I C  F E A T U R E S    */
/*--------------------------------------------------------------------------*/
#include "vfwindeo.h"


/****************************************************************************/
/* T Y P E S   A N D   D E F I N I T I O N S                                */
/*--------------------------------------------------------------------------*/

#define MAX_CHUNK_NEST_LEVEL          5  // never change this define
// Number of sub-chunk levels in a RIFF file: Top level RIFF, LIST/movi,
// LIST/rec, ##dc frame data.  That makes 4 in the worst case, the header
// list also needs 4.

#define DSndBufNum                    CE_ARTLIB_VideoAVIBufferNumber
// Direct sound buffer size is (DSndBufNum * audioClockTicks + 1)

#define AVI_DATA_BUFFER_NUMBER        CE_ARTLIB_VideoAVIBufferNumber

#define PREROLL_LIMIT_IF_NEXT_KEY     3
// When decompressing a video non-key frame needs to preroll over a few
// previous video frames, PREROLL_LIMIT_IF_NEXT_KEY sets a limit to how
// many video frames at most to preroll if the video frame next to the
// requested video frame is a key frame.

#define SILENT_MOVIE_WITH_BUFSND      0
// 1 -- play a movie silently, still using the buf sound system to do the timing
// 0 -- play a movie silently. But the user should time the video playback.
//      That means that the user should time the update of *frameNumberPtr
//      argument for LE_VIDEO_FeedVideo ().
//      The default value is 0, in which case, the sequencer is responsible
//      for timing the update of *frameNumberPtr argument for FeedVideo ().


// data structure for avi chunk data searching, reading, and file navigation
typedef struct MyAVIFileStruct
{
  char name [MAX_PATH];
    // Name of the file.

  HMMIO file;
    // Handle to the file, closed if zero.

  MMCKINFO chunkInfo [MAX_CHUNK_NEST_LEVEL + 1];
    // For keeping track of the currently open (descended or created) chunks
    // in the file.  The overall RIFF chunk is in [1], and subchunks are in
    // increasing levels.

  short nestLevel;
    // The current nesting level, zero when no levels are open, varies
    // from 0 to MAX_CHUNK_NEST_LEVEL.

} MyAVIFileRecord, *MyAVIFilePointer;


// data structure for each avi data buffer
typedef struct tagAVIDataBufferStruct
{
  LPBYTE          aviDataBuffer;
  // AVI data buffer. The memory is allocated when processing the AVI file
  // and deallocated when the video is stopped.
  
  LPBYTE          startVideoFramePtr;
  // Points to the first video frame of the avi data buffer.
  
  LPBYTE          videoFramePtr;
  // Video frame buffer points to individual frames in videoDataBuffer.
  // It is always initialised to videoDataBuffer after data reading.
  
  LONG            startFrameNumber;
  // Frame number for the start video frame stored in videoDataBuffer.
  // The first frame of a video should be frame 0.

  LONG            endFrameNumber;
  // Frame number for the end video frame stored in videoDataBuffer.
  // This is pass end frame number, one frame bigger than the acutally
  // frame number.

  AVIINDEXENTRY  *startDataBufferIndexPtr;
  // Index chunk position referring to the start of data stored in aviDataBuffer.
  // The first frame of the data buffer must be a key video frame.
  
  AVIINDEXENTRY  *startVideoFrameIndexPtr;
  // Index position pointer to the first video frame in the data buffer.

  AVIINDEXENTRY  *videoFrameIndexPtr;
  // Index position pointer to current video frame in the data buffer.

  short           audioChunkNumber;
  // How many audio chunk data are preloaded in the data buffer.
  
  AVIINDEXENTRY  *audioDataIndexPtr;
  // Index position pointer to the next feeding audio data, keeping track
  // of audio data feeding.

  BOOL            bAudioData;
  // If TRUE, the avi data buffer has still got some audio data to feed.
  // If FALSE, the avi data buffer has no audio data left to feed.

#if CE_ARTLIB_VideoFrameJumpModeEnabled
  BOOL            bFrameJumpBuffer;
  // If TRUE, the avi data preloaded in the buffer are to be used for
  //    the next possible frame jump action.
  // If FALSE, the avi data preloaded are for normal sequencial playback.

  #if CE_ARTLIB_VideoMultiTaskingEnabled // for multi-tasking mode only
    BOOL          bVoidBuffer;
    // In multitasking mode, one data buffer may be accessed simultaneously
    // by both the video control thread and data buffering thread. When
    // doing CutVideoToFrame quick frame jump aciton, bVoidBuffer is useful
    // for voiding all data buffers and do a quick data buffering for
    // the requested frame jump aciton. In multitasking mode, using flag
    // bEmptyBuffer is dangerous, as the flag is used by the data buffering
    // thread to identify which data buffer is empty and ready for data
    // buffering.
  #endif
#endif
  
  BOOL            bEmptyBuffer;
  // TRUE indicates that the avi data buffer is empty, or may be used by
  // the avi data buffering thread; FALSE otherwise.

} MyAVIDataBufferRecord, *MyAVIDataBufferPointer;


// Main AVI data structure for the new AVI video player.
typedef struct tagDataStruct
{
  MyAVIFileRecord    aviSource;
  // A data structure which stores information for AVI file operation, such as
  // AVI data chunk searching, reading, and file navigation.

  DWORD              moviChunkDataOffset;
  // File offset of the beginning of the data portion of chunk LIST/movi. After
  // chunk id and size (8 bytes), but before the list type 'movi'.

  DWORD              currentFilePosition;
  // Keeps track of current file position, file offset from the beginning of the
  // AVI data file.

  WORD               videoStreamID;
  // Saves the video stream ID (stream number). At the moment we only use one
  // video stream. The second video stream will be implemented later on.

  WORD               audioStreamID;
  // Saves the audio stream ID (stream number).
  
  MyAVIDataBufferPointer dataBuffer[AVI_DATA_BUFFER_NUMBER];
  // Preallocated data buffers for the new AVI videoplayer. Each buffer
  // preloads one video key frame and its following delta frames as well as
  // their associated audio data. The memory is allocated when the video
  // starts and gets deallocated when the video stops.
  // The video player preloads all 10 data buffers when the video starts.
  // After the video is started, each time it preloads two data buffers.
  
  DWORD              aviDataBufferSize;
  // Amount of memory allocated for videoDataBuffer. The size is given by
  // videoStreamHeaderPtr->dwSuggestedBufferSize.
  
  LONG               audioFrameNumber;
  // Remembers the previous audio frame we rendered. It should start from
  // zero up to the length of the video.

  LONG               dataBufferStartVideoFrameNumber;
  // Remembers the start video frame number for the next avi data buffer.
  
  LONG               startVideoFrameForNextAudioFeed;
  // Saves the start video frame number for next audio data feeding (pass
  // end frame of last audio feeding). This video frame number is used to
  // find which avi data buffer will be used for next round audio data
  // feeding.
  
  LONG               audioStartPosition;
  // Start audio stamp position for each audio data feeding.

  BOOL               bEnableVideo;
  // If FALSE, play only the audio stream.

  BOOL               bSilentMovie;
  // If TRUE, the AVI file has no audio data in it.
  // Then, no audio data feeding at all. Use the frame number
  // provided by sequencer to play the movie.
  
  #if CE_ARTLIB_EnableDebugMode
    TYPE_Tick          videoStartTimeInTicks;
    // Saves the video start time in ticks. (a kind of global clock).

    LONG               videoFramesShown;
    // Counts the total video frames actually decompressed and drawn to the
    // screen, not including those skipped frames.
  #endif

  AVIINDEXENTRY     *indexChunkDataBuffer;
  // Data buffer for the AVI index chunk 'idx1'. The memory is allocated
  // when processing the AVI file and deallocated when the video is stopped.

  DWORD              indexChunkDataSize;
  // Size in bytes of the index chunk data buffer.

  #if CE_ARTLIB_VideoFrameJumpModeEnabled
    AVIINDEXENTRY  **frameIndexArray;
    // An index array for video selective jumping playback mode. The memory
    // is allocated when a video starts and gets deallocated when the video
    // is stoped.

    LONG             decisionFrameNumber;
    // The so-called decision frame number. When the decisionFrame is
    // rendered, the player will jump to play the alternative frame
    // referred to as jumpToFrameNumber, kind of immediate frame jump.

    BOOL             bTakeAlternative;
    // TRUE means to take the alternative path.
    // FALSE means to not jump to the alternative frame.

    LONG             jumpToFrameNumber;
    // the frame number to which the video player might jump in the
    // next frame jump action caused by decisionFrameNumber

    BOOL             bCutVideoToFrame;
    // Signals immediate cut video to frame jump. Throw all data buffers
    // and buffer the jump pointer frame immediately.

    #if !CE_ARTLIB_VideoMultiTaskingEnabled
      CRITICAL_SECTION  myCriticalSection;
      // Used for single threaded frame jump action only.
      // Note that different approaches are used to protect the main
      // avi data structure from being accessed simultaneously from
      // other threads. Messages are used for the multitasking frame
      // jump mode while the critical seciton is used for the single-
      // threaded frame jump playback mode.
    #endif
  #endif

  LPBITMAPINFOHEADER bmiOutputHeaderPtr;
  // Video image output format, which is actally a bitmap info header.
  // The memory is allocated when the video starts and deallocated when the
  // video is stopped.

  int  outputWidth, outputHeight;
  // Specifies output bitmap position and stretching if the output
  // bitmap width/height are different from the input specifications.

  LONG destRGBBitCount;
  // As the Indeo video 5 codec does not play double image size video
  // in RGB32-bit mode, we play only the original image size video
  // at the centre of the screen.

  BOOL               bFlipVertically;
  // Tells if the video images need to be flipped vertically.

  AVIINDEXENTRY     *nextVideoChunkIndexPtr;
  // Points to the index chunk position where we should start our next
  // data buffering.

  LE_SOUND_BufSndHandle   hBufSnd;
  // A handle to an internal data structure of the sound system.

  HIC                hIC;
  // A handle to the video image decompressor.
  
  R4_DEC_FRAME_DATA  myR4FrameState;
  // Frame data structure for Indeo video 5 codec.

  short  brightness, contrast, saturation;
  // for Indeo video 5 codec specific colour effects

  BOOL               bEndOfMoviDataChunk;
  // Indicates that the end of movi data is reached.

  BOOL               bFullScreenMode;
  // To turn on/off of full screen mode.

  BOOL               bLoopMode;
  // Indicates that the video loop mode is on.

  #if CE_ARTLIB_VideoMultiTaskingEnabled
    LE_QUEUE_Record    messagesToDataBufferingThread;
    LE_QUEUE_Record    messagesToVideoControlThread;
    // Two message queues are used to send messages to and receive messages
    // from the avi data buffering thread.
  #endif

  // Following are various AVI file/stream/format headers.

  MainAVIHeader     *mainAVIHeaderPtr;
  // General header information for the entire AVI file. The memory is allocated
  // when processing the AVI file and deallocated when the video is stopped.

  AVIStreamHeader   *videoStreamHeaderPtr;
  // Specific information for the video stream of the AVI file. It gives
  // information such as the preferred decompressor for video image data, total
  // length in frames, and at what speed (frames per second) the AVI video should
  // be played, etc. The memory is allocated when processing the AVI file
  // deallocated when the video is stopped.

  BITMAPINFO        *videoStreamFormatPtr;
  // Data format for the video stream. The size of the structure is variable.
  // It depends on the version of BITMAPINFO used in the AVI file. The memory
  // gets allocated when processing the AVI file and deallocated when the video
  // is stopped.

  AVIStreamHeader   *audioStreamHeaderPtr;
  // Audio stream information of the AVI file. It gives specific information
  // about the audio stream, such as the audio data type, preferred decompressor,
  // initial frames, suggested buffer size, and quality parameter, etc. The memory
  // is allocated when processing the AVI file and deallocated when the video is
  // stopped.

  WAVEFORMATEX      *audioStreamFormatPtr;
  // Data format for the audio stream of the AVI file. The size of the structure
  // is variable and dependent upon the actual audio data in the AVI file. The
  // memory is also allocated when processing the AVI file and deallocated when
  // the video is stopped.

}  MyDataRecord, *MyDataPointer;


// message types for video avi data buffering thread
enum BufferFillerMessageCodesEnum
{
  LI_VIDEO_MESSAGE_ShutDown,
  LI_VIDEO_MESSAGE_ShutDownDone,
  LI_VIDEO_MESSAGE_LoadBuffer,
  LI_VIDEO_MESSAGE_LoadBufferDone,
  LI_VIDEO_MESSAGE_ALTERNATIVE_FRAME_SETUP,
  LI_VIDEO_MESSAGE_CHANGE_VIDEO_ALTERNATIVE,
  LI_VIDEO_MESSAGE_FORGET_VIDEO_ALTERNATIVE,
  LI_VIDEO_MESSAGE_CUT_VIDEO_TO_FRAME,
  LI_VIDEO_MESSAGE_ErrorHappened,
};

#define CUT_VIDEO_TO_FRAME_LOAD      1 // indicates the way of data buffering


/****************************************************************************/
/* G L O B A L   V A R I A B L E S                                          */
/*--------------------------------------------------------------------------*/

union {
  DWORD   fourcc;
  char    string[5];
}  FourCCToString;
// The union data structure is used to convert a DWORD to
// a four-character string, or vice versa.


/****************************************************************************/
/*  F U N C T I O N   P R O T O T Y P E S                                   */
/*--------------------------------------------------------------------------*/

BOOL SearchForChunkFromCurrentFilePosition (
  MyAVIFilePointer  myAVIFilePtr,
  LPMMCKINFO        chunkToSearchPtr);

BOOL ReadChunkFixedDataPart (
  HMMIO   hMyAVIFile,
  DWORD   chunkDataSize,
  BYTE  **ppChunkDataBuffer,
  DWORD   suggestedBufferSize);

BOOL GetSuggestedAVIDataBufferSize (MyDataPointer aviDataPtr);

BOOL LI_VIDEO_AllocatePreloadingDataBuffers (MyDataPointer aviDataPtr);

BOOL LI_VIDEO_FeedAVIVideoBuffers (LE_VIDEO_VideoHandle hVideoHandle);

BOOL LI_VIDEO_SkipVideoFrames (
  MyAVIDataBufferPointer aviDataBufferPtr,
  LONG framesToAdvance,
  WORD videoStreamID);

BOOL LI_VIDEO_DrawToScreen (
  LE_VIDEO_VideoHandle hVideoHandle,
  const int dataBufferIndex,
  LONG  previousFrame,
  LONG  currentFrame,
  BYTE *outputBitmapPtr,
  LONG  offsetX,
  LONG  offsetY);

#if CE_ARTLIB_VideoMultiTaskingEnabled
  // for avi data buffering thread
  unsigned int __stdcall DataBufferingMainFunction (void *hVideoHandle);
#endif

BOOL PrepareVideoIndexForSelectiveJumpingMode (MyDataPointer aviDataPtr);
// Internally generates an array of video frame index for selective
// jumping playback mode

void ClearUpAVIDataRecord (MyDataPointer aviDataPtr);

void CloseMyAVIFile (MyAVIFilePointer aviFilePtr);


/*****************************************************************************
 *
 *  LE_VIDEO_VideoHandle LE_VIDEO_StartVideo ()
 *
 *  It opens an AVI file and processes the data, reads various AVI file/stream
 *  headers, reads the index chunk and finds the suggested buffer sizes for
 *  both the video and audio streams, allocates memory for all avi data buffers
 *  and moves the current file position to the beginning of the data portion
 *  of chunk LIST/movi (after the list type 'movi'). Before the movie really
 *  starts, it does the first round of avi data buffering.
 *
 *  It opens an appropriate video decompressor. If it is Indeo video 5 (R)
 *  interactive codec, it sets various Indeo video 5 codec features for the
 *  movie.
 *
 *  Returns LE_VIDEO_VideoHandle to be used by other functions to talk to the
 *  main avi data structure. NULL for failure.
 *
 *  NOTES for Indeo video codec version 5.10 and 5.11:
 *    Since Indeo video codec (version) 5.10 and 5.11, it is okay to play
 *    a video of its original image size in RGB32-bit mode. But the Indeo video
 *    codec double image size does not work in RGB32-bit mode.
 *    Thus, in RGB32-bit mode, we switch off the codec double video image size
 *    feature.
 *
 *****************************************************************************/

LE_VIDEO_VideoHandle LE_VIDEO_StartVideo (
  const char *aviFileName,
  LPBITMAPINFOHEADER userOutputBitmapInfoPtr,
  INT     outputWidth,
  INT     outputHeight,
  BOOL    bLoopAtEndRatherThanPause,
  BOOL    bFlipVertically,
  BOOL    bEnableVideo,
  BOOL    bEnableAudio,
  BOOL    bUse3DSound,
  BOOL    bAlternateScanLine,
  BOOL    bDoubleImageSize)
{
  MyDataPointer       aviDataPtr;          // pointer to the main avi data
                                           // structure
  BOOL                bFlag;
  MMCKINFO            tempChunkInfo;
  DWORD               streamID;            // 'vids' for video stream
                                           // 'auds' for audio stream 
  BYTE               *buffer  = NULL;      // generic data buffer
  DWORD               fixedChunkDataSize;  // size of chunk data portion
  DWORD               currentFilePosition; // keep track of current file position
  
  short               numberOfStreams;     // total number of streams (MainAVIHeader)
  short               nCounter;

  WAVEFORMATEX        audioOutputFormat;   // for BufSound system
  DWORD               audioClockTicks;     // clock ticks for the internal audio buffer
  LPBITMAPINFOHEADER  bmiInputHeaderPtr;
  LPBITMAPINFOHEADER  bmiOutputHeaderPtr;

  #if CE_ARTLIB_VideoMultiTaskingEnabled
    LE_QUEUE_MessageRecord  myMessage;
    HANDLE            hDataBufferingThread; // handle to the data buffering thread
    unsigned int      nIDThread = 0;
  #endif

  // check for input AVI file name
  if ( aviFileName == NULL || aviFileName[0] == 0 )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "aviFileName == NULL || aviFileName[0] == 0.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return NULL;  // illegal aviFileName
  }

  // Here, we allocate memory for the main avi data structure.
  aviDataPtr = (MyDataPointer) LE_MEMORY_malloc (sizeof (MyDataRecord));

  if ( aviDataPtr == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "LE_MEMORY_malloc () fails to allocate %d bytes of memory for the main "
      "AVI data structure.\n", sizeof (MyDataRecord));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return NULL;  // LE_MEMORY_malloc () fails to allocate memory for the main avi data structure.
  }

  // fill the allocated memory with zeros
  memset (aviDataPtr, 0, sizeof (MyDataRecord));

  // save the input source AVI file name
  strcpy (aviDataPtr->aviSource.name, aviFileName);

  // open the input AVI source file
  if (aviDataPtr->aviSource.name[0] != 0)
  {
    aviDataPtr->aviSource.file =
      mmioOpen (aviDataPtr->aviSource.name, NULL, MMIO_DENYWRITE | MMIO_READ);

    if (aviDataPtr->aviSource.file == NULL)
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "Unable to open file \"%s\" for reading.\n", aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit;  // mmioOpen fails to open Source.name
    }
  }
  else  // invalid input source file name
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "aviDataPtr->aviSource.name[0] == 0.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // garbage source file name
  }

  // Before doing any data processing, we must validate the source
  // file format, see if it is an AVI file.

  #if CE_ARTLIB_EnableDebugMode
//  memset (&tempChunkInfo, 0, sizeof (tempChunkInfo));
    tempChunkInfo.ckid = mmioFOURCC ('R', 'I', 'F', 'F');
    tempChunkInfo.fccType = mmioFOURCC ('A', 'V', 'I', ' ');

    if ( SearchForChunkFromCurrentFilePosition (&aviDataPtr->aviSource, &tempChunkInfo) == FALSE )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "SearchForChunkFromCurrentFilePosition () indicates that "
        "%s is not a RIFF/AVI type of file.\n", aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit;  // fails in searching for chunk RIFF/AVI
    }
  #endif

  // Start to search for various file/stream/format headers.

  // Do a forward search for chunk 'avih' for MainAVIHeader
//  memset (&tempChunkInfo, 0, sizeof (tempChunkInfo));
  tempChunkInfo.ckid = mmioFOURCC ('a', 'v', 'i', 'h');
  
  if ( SearchForChunkFromCurrentFilePosition (&aviDataPtr->aviSource, &tempChunkInfo) == FALSE )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "SearchForChunkFromCurrentFilePosition () fails in searching for "
      "chunk 'avih' in %s.\n", aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // fails in searching for chunk 'avih'
  }

  // found chunk 'avih' ?
  if ( tempChunkInfo.ckid == 0 )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "has not found chunk 'avih' before reached EOF in %s.\n",
      aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // EOF reached
  }

  // Yes, have found chunk 'avih'. Now read in MainAVIHeader

  // get chunk data size
  fixedChunkDataSize = tempChunkInfo.cksize;

  // read in chunk data part
  bFlag = ReadChunkFixedDataPart (
    aviDataPtr->aviSource.file,
    fixedChunkDataSize,
    (BYTE **) &aviDataPtr->mainAVIHeaderPtr,
    sizeof (MainAVIHeader));

  if ( bFlag == FALSE )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "ReadChunkFixedDataPart () fails to read chunk 'avih' data "
      "in %s.\n", aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // fails to read chunk 'avih' for MainAVIHeader
  }

  // Find the number of streams from MainAVIHeader.
  numberOfStreams = (short) aviDataPtr->mainAVIHeaderPtr->dwStreams;

  if ( numberOfStreams <= 0 )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "there are only %d streams in %s.\n",
      numberOfStreams, aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // found illegal number of streams
  }

  // Search for stream headers (strh) and stream data formats (strf)
  // for both the video and audio streams.

  if ( numberOfStreams > 2 )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "there are %d streams in %s.\n",
      numberOfStreams, aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    numberOfStreams = 2; // reset it to 2 -- one for video and one for audio
  }

  aviDataPtr->bSilentMovie = TRUE;  // default to video stream only (no audio stream)

  for ( nCounter = 0; nCounter < numberOfStreams; nCounter ++ )
  {
    // search for chunk 'strh' for AVIStreamHeader

//    memset (&tempChunkInfo, 0, sizeof (tempChunkInfo));
    tempChunkInfo.ckid = mmioFOURCC ('s', 't', 'r', 'h');
  
    if ( SearchForChunkFromCurrentFilePosition (&aviDataPtr->aviSource, &tempChunkInfo) == FALSE )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "SearchForChunkFromCurrentFilePosition () fails in searching for "
        "chunk 'strh' in %s.\n", aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit;  // fails in searching for chunk 'strh'
    }

    // found chunk 'strh' ?
    if ( tempChunkInfo.ckid == 0 )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "has not found chunk 'strh' before reached EOF in %s.\n",
        aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit;  // EOF reached
    }

    // Yes, have found chunk 'strh'. Now read in AVIStreamHeader

    // get chunk data size
    fixedChunkDataSize = tempChunkInfo.cksize;

    // Since we, at the moment, don't know which stream it is,
    // we read the chunk data portion into a generic data buffer.
    bFlag = ReadChunkFixedDataPart (
      aviDataPtr->aviSource.file,
      fixedChunkDataSize,
      &buffer,
      sizeof (AVIStreamHeader));

    if ( bFlag == FALSE )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "ReadChunkFixedDataPart () fails to read chunk 'strh' data "
        "in %s.\n", aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit;  // fails to read chunk 'strh' for AVIStreamHeader
    }

    // get the stream id -- 'vids' for video stream and 'auds' for audio stream
    streamID = ((AVIStreamHeader *) buffer)->fccType;

    // A video stream or an audio stream ?
    if ( streamID == mmioFOURCC ( 'v', 'i', 'd', 's' ) )
    {
      // Found a video stream.
      aviDataPtr->videoStreamHeaderPtr = (AVIStreamHeader *) buffer;
      buffer = NULL;

      // save the video stream ID
      // Note: At the moment only two streams are used
      // -- one video and one audio stream
      switch (nCounter)
      {
      case 0:
        aviDataPtr->videoStreamID = aviTWOCC ('0', '0');
        break;

      case 1:
        aviDataPtr->videoStreamID = aviTWOCC ('0', '1');
        break;

      case 2:
        aviDataPtr->videoStreamID = aviTWOCC ('0', '2');
      }
    }
    else if ( streamID == mmioFOURCC ( 'a', 'u', 'd', 's' ) )
    {
      // Found an audio stream.
      aviDataPtr->audioStreamHeaderPtr = (AVIStreamHeader *) buffer;
      buffer = NULL;

      aviDataPtr->bSilentMovie = FALSE;

      // save the audio stream ID
      switch (nCounter)
      {
      case 0:
        aviDataPtr->audioStreamID = aviTWOCC ('0', '0');
        break;

      case 1:
        aviDataPtr->audioStreamID = aviTWOCC ('0', '1');
        break;

      case 2:
        aviDataPtr->audioStreamID = aviTWOCC ('0', '2');
      }
    }
    else  // found an unknown type of stream
    {
    #if CE_ARTLIB_EnableDebugMode
      // make a string for the fccType
      memset (&FourCCToString, 0, sizeof (FourCCToString));
      FourCCToString.fourcc = streamID;

      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "found an unknown type of stream (%s) in %s.\n",
        FourCCToString.string, aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif

      // deallocate the data buffer
      if ( buffer != NULL )
      {
        LE_MEMORY_free (buffer);
        buffer = NULL;
      }
    }

    // Carry on searching for chunk 'strf' for the stream data format

//    memset (&tempChunkInfo, 0, sizeof (tempChunkInfo));
    tempChunkInfo.ckid = mmioFOURCC ('s', 't', 'r', 'f');

    if ( SearchForChunkFromCurrentFilePosition (&aviDataPtr->aviSource, &tempChunkInfo) == FALSE )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "SearchForChunkFromCurrentFilePosition () fails in searching for "
        "chunk 'strf' in %s.\n", aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit;  // fails in searching for chunk 'strf'
    }

    // found chunk 'strf' ?
    if ( tempChunkInfo.ckid == 0 )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "has not found chunk 'strf' before reached EOF in %s.\n",
        aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit;  // EOF reached
    }

    // Yes, have found chunk 'strf'. Now read in the stream data format.

    // get chunk data size
    fixedChunkDataSize = tempChunkInfo.cksize;

    // read in chunk data part for the stream data format ('strf')

    if ( streamID == mmioFOURCC ('v', 'i', 'd', 's') )
    {
      // The data format for a video stream is BITMAPINFO.
      bFlag = ReadChunkFixedDataPart (
        aviDataPtr->aviSource.file,
        fixedChunkDataSize,
        (BYTE **) &aviDataPtr->videoStreamFormatPtr,
        sizeof (BITMAPINFO));
    }
    else if (streamID == mmioFOURCC ('a', 'u', 'd', 's') )
    {
      // The data format for an audio stream might be WAVEFORMAT, PCMWAVEFORMAT,
      // or WAVEFORMATEX. So, it is safer to use WAVEFORMATEX as a suggestion.
      bFlag = ReadChunkFixedDataPart (
        aviDataPtr->aviSource.file,
        fixedChunkDataSize,
        (BYTE **) &aviDataPtr->audioStreamFormatPtr,
        sizeof (WAVEFORMATEX));
    }
    else // unknown type of stream
    {
    #if CE_ARTLIB_EnableDebugMode
      // make a string for the unknown type of stream
      memset (&FourCCToString, 0, sizeof (FourCCToString));
      FourCCToString.fourcc = streamID;
      
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "read data format for an unknown type of stream (%s) in %s.\n",
        FourCCToString.string, aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      
      // fake to read in fixedChunkDataSize bytes of data
      bFlag = ReadChunkFixedDataPart (
        aviDataPtr->aviSource.file,
        fixedChunkDataSize,
        &buffer,
        0);
      
      // deallocate the data buffer
      if ( buffer != NULL )
      {
        LE_MEMORY_free (buffer);
        buffer = NULL;
      }
    #endif
    }

    if ( bFlag == FALSE )
    {
    #if CE_ARTLIB_EnableDebugMode
      // make a string for the stream ID
      memset (&FourCCToString, 0, sizeof (FourCCToString));
      FourCCToString.fourcc = streamID;

      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "ReadChunkFixedDataPart () fails to read chunk 'strf' data "
        "for streamID %s in %s.\n",
        FourCCToString.string,
        aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit;  // fails to read chunk 'strf' for the stream data format
    }
  }  // End of searching for strh/strf for both the video and audio streams

  // Now do a forward search for chunk LIST/movi for the file position and
  // data offset of the chunk.

//  memset (&tempChunkInfo, 0, sizeof (tempChunkInfo));
  tempChunkInfo.ckid = mmioFOURCC ('L', 'I', 'S', 'T');
  tempChunkInfo.fccType = mmioFOURCC ('m', 'o', 'v', 'i');

  if ( SearchForChunkFromCurrentFilePosition (&aviDataPtr->aviSource, &tempChunkInfo) == FALSE )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "SearchForChunkFromCurrentFilePosition () fails in searching for "
      "chunk LIST/movi in %s.\n", aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // fails in searching for chunk LIST/movi
  }

  // found chunk LIST/movi ?
  if ( tempChunkInfo.ckid == 0 )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "have not found chunk LIST/movi before reached EOF in %s.\n",
      aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // EOF reached
  }

  // Yes, we have found chunk LIST/movi.

  // Get file offset of the data portion of chunk LIST/movi
  aviDataPtr->moviChunkDataOffset = tempChunkInfo.dwDataOffset;

  // find current file position -- 4 bytes after the list type 'movi'
  aviDataPtr->currentFilePosition = aviDataPtr->moviChunkDataOffset + 4;

  // To speed up the search for the video index chunk, we should skip
  // the 'movi' chunk data portion before searching for the index chunk.
  // Otherwise, we have to step through all of 'movi' data chunks to reach
  // the index chunk. So, skip the data portion of the 'movi' chunk.
  if ( mmioAscend (aviDataPtr->aviSource.file, &tempChunkInfo, 0) !=
    MMSYSERR_NOERROR )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "unable to ascend out of chunk LIST/movi in %s.\n",
      aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // unable to ascend out of chunk LIST/movi
  }

  // update current chunk nesting level for the avi data file
  aviDataPtr->aviSource.nestLevel --;

  // Now, the current file position is just after the end of chunk LIST/movi.
  // Do a forward search for AVI file index chunk 'idx1'.

//  memset (&tempChunkInfo, 0, sizeof (tempChunkInfo));
  tempChunkInfo.ckid = mmioFOURCC ('i', 'd', 'x', '1');

  if ( SearchForChunkFromCurrentFilePosition (&aviDataPtr->aviSource, &tempChunkInfo) == FALSE )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "SearchForChunkFromCurrentFilePosition () fails in searching for index "
      "chunk 'idx1' in %s.\n", aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // fails in searching for index chunk 'idx1'
  }

  // found chunk 'idx1' ?
  if ( tempChunkInfo.ckid == 0 )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "have not found chunk 'idx1' before reached EOF in %s.\n",
      aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // EOF reached
  }

  // Yes, have found the AVI file index chunk 'idx1'.

  // get current file position
  currentFilePosition = tempChunkInfo.dwDataOffset;

  // get chunk data size
  aviDataPtr->indexChunkDataSize = tempChunkInfo.cksize;

  // read index chunk data part
  bFlag = ReadChunkFixedDataPart (
    aviDataPtr->aviSource.file,
    aviDataPtr->indexChunkDataSize,
    (BYTE **) &aviDataPtr->indexChunkDataBuffer,
    0);

  if ( bFlag == FALSE )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "ReadChunkFixedDataPart () fails to read %d bytes of data from index chunk "
      "'idx1' in %s.\n", aviDataPtr->indexChunkDataSize, aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // fails to read index chunk data
  }

  // update current file position
  currentFilePosition += aviDataPtr->indexChunkDataSize;

  // Set nextVideoChunkIndexPtr to indexChunkDataBuffer.
  aviDataPtr->nextVideoChunkIndexPtr = aviDataPtr->indexChunkDataBuffer;

  // initialise the audioFrameNumber member of the main avi data structure
  aviDataPtr->audioFrameNumber = -1; // video start setting

  // initialise bEndOfMoviDataChunk to FALSE
  aviDataPtr->bEndOfMoviDataChunk = FALSE;

  // video loop mode ?
  if ( bLoopAtEndRatherThanPause == TRUE )
    aviDataPtr->bLoopMode = TRUE;

  // allocate memory for (AVI_DATA_BUFFER_NUMBER) preloading
  // avi data buffers
  bFlag = LI_VIDEO_AllocatePreloadingDataBuffers (aviDataPtr);

  if ( bFlag == FALSE )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "LI_VIDEO_AllocatePreloadingDataBuffers () fails to allocate "
      "memory for %d avi data buffers.\n", AVI_DATA_BUFFER_NUMBER);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // unable to allocate memory for avi data buffers
  }

  // double check for the generic data buffer
  if ( buffer != NULL )
  {
    LE_MEMORY_free (buffer);
    buffer = NULL;
  }

  // By now, we have got everything we need (various header structures
  // and the index chunk). Before start to playback the AVI video clip,
  // we need to move the current file position back to the beginning of
  // the movi data, 12 bytes offset of chunk LIST/movi.

  if ( mmioSeek (aviDataPtr->aviSource.file,
    (LONG) aviDataPtr->currentFilePosition - currentFilePosition, SEEK_CUR)
    != (LONG) aviDataPtr->currentFilePosition )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "unable to seek back to the beginning of data portion of chunk LIST/movi "
      "in %s.\n", aviDataPtr->aviSource.name);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // seek fails
  }

  // The current file position is now at the beginning of the movi data,
  // 12 bytes offset of chunk LIST/movi.

  // We are now preparing the video image decompressor and the sound system
  // for the movie.

  // for the video image decompressor

  // get the input bitmap data format
  bmiInputHeaderPtr = &aviDataPtr->videoStreamFormatPtr->bmiHeader;

  // specify a desired output bitmap data format
  // allocate memory for output bitmap data format
  // Note: We use BITMAPV4HEADER here, as we need to specify
  //       colour masks for the video image decompressor to
  //       work properly.
  bmiOutputHeaderPtr =
    (LPBITMAPINFOHEADER) LE_MEMORY_malloc (sizeof(BITMAPV4HEADER));

  if ( bmiOutputHeaderPtr == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "unable to allocate %d bytes for bmiOutputHeaderPtr.\n",
      sizeof (BITMAPV4HEADER));
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // LE_MEMORY_malloc () fails
  }

  // save the pointer in the main AVI data structure
  aviDataPtr->bmiOutputHeaderPtr = bmiOutputHeaderPtr;

  if ( userOutputBitmapInfoPtr != NULL )
  {
    memcpy (bmiOutputHeaderPtr, userOutputBitmapInfoPtr, sizeof(BITMAPV4HEADER));
    bmiOutputHeaderPtr->biWidth = bmiInputHeaderPtr->biWidth;
    bmiOutputHeaderPtr->biHeight = bmiInputHeaderPtr->biHeight;
  }
  else // hack one for the video image output
  {
    // For Windows 95/98, bitmap biCompression == BI_BITFIELDS is supported
    // for both 16 and 32 bit colour depths and DWORD colour masks are used.
    // For biCompression == BI_BITFIELDS, the system supports both 5-5-5 bit
    // and 5-6-5 bit colour masks. Here we set output biCompression to
    // BI_BITFIELDS. If biCompression is set to BI_RGB, then, the system
    // supports only 5-5-5 bit WORD colour masks. In 24 bit colour depth
    // BI_BITFIELDS is however not supported and BI_RGB is used.

    memset (bmiOutputHeaderPtr, 0, sizeof (BITMAPV4HEADER)); // use v4 header
    memcpy (bmiOutputHeaderPtr, bmiInputHeaderPtr, sizeof (BITMAPINFOHEADER));
    bmiOutputHeaderPtr->biSize = sizeof (BITMAPV4HEADER);    // use v4 header
    bmiOutputHeaderPtr->biPlanes = 1;
    bmiOutputHeaderPtr->biBitCount = LE_GRAFIX_ScreenBitsPerPixel; // CE_ARTLIB_InitialScreenDepth;
    if ( LE_GRAFIX_ScreenBitsPerPixel == 16 || LE_GRAFIX_ScreenBitsPerPixel == 32 )
      bmiOutputHeaderPtr->biCompression = BI_BITFIELDS;
    else
      bmiOutputHeaderPtr->biCompression = BI_RGB;

    bmiOutputHeaderPtr->biSizeImage = bmiInputHeaderPtr->biWidth *
      abs (bmiInputHeaderPtr->biHeight) * LE_GRAFIX_ScreenBitsPerPixel / 8;

    // specify colour masks (use v4 header)
    ((LPBITMAPV4HEADER) bmiOutputHeaderPtr)->bV4RedMask = LE_BLT_dwRedMask;
    ((LPBITMAPV4HEADER) bmiOutputHeaderPtr)->bV4GreenMask = LE_BLT_dwGreenMask;
    ((LPBITMAPV4HEADER) bmiOutputHeaderPtr)->bV4BlueMask = LE_BLT_dwBlueMask;
  }

  // Locate a decompression driver capable of decompressing our video data.
  // Look for the decompressor driver suggested by the video stream.
  aviDataPtr->hIC = ICLocate (
    ICTYPE_VIDEO,
    aviDataPtr->videoStreamHeaderPtr->fccHandler,  // use video stream preferred decompressor
    bmiInputHeaderPtr,                             // compressed input format
    (LPBITMAPINFOHEADER) bmiOutputHeaderPtr,       // decompressed output format
    ICMODE_FASTDECOMPRESS);

  if ( aviDataPtr->hIC == 0 )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "unable to locate the decompressor driver suggested by the video "
      "stream for video image data decompression.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif

    // Try to locate other video image decompressors which are capable of
    // decompressing our video image data.
    aviDataPtr->hIC = ICLocate (
      ICTYPE_VIDEO,
      0L,                           // video image decompressor
      bmiInputHeaderPtr,            // compressed input format
      bmiOutputHeaderPtr,           // decompressed output format
      ICMODE_FASTDECOMPRESS);

    if ( aviDataPtr->hIC == 0 )
    {
      char           tempStr[256];

      memset (tempStr, 0, 256);
      memset (&FourCCToString, 0, sizeof (FourCCToString));
      FourCCToString.fourcc = aviDataPtr->videoStreamHeaderPtr->fccHandler;
      sprintf (tempStr, "Unable to locate the required video codec (%s). "
        "Please install the video codec and play the video again.\n",
        FourCCToString.string);
      MessageBox (NULL, tempStr, NULL, MB_OK | MB_ICONERROR);
      goto ErrorExit;
    }
  }

  aviDataPtr->bEnableVideo = bEnableVideo;
  if ( !bEnableVideo && aviDataPtr->hIC )
  {
    // Since no video, close decompressor.
    ICClose (aviDataPtr->hIC);
    aviDataPtr->hIC = NULL;
  }
#if !SILENT_MOVIE_WITH_BUFSND
  else if ( bEnableVideo && !bEnableAudio )
    aviDataPtr->bSilentMovie = TRUE;  // play silent movie without buf sound
#endif

  #if CE_ARTLIB_EnableDebugMode
    if ( !bEnableVideo && !bEnableAudio )
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "Both video and audio are disabled.\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      goto ErrorExit;  // nothing to be done
    }
  #endif
  
  aviDataPtr->outputWidth = bmiInputHeaderPtr->biWidth;
  aviDataPtr->outputHeight = bmiInputHeaderPtr->biHeight;
  
  if ( userOutputBitmapInfoPtr == NULL )
  {
    // In order to be able to decompress video image data directly to
    // the video board memory, we need to set output bitmap size to the
    // screen dimensions.
    // Fake a width and height for the destination bitmap.
    bmiOutputHeaderPtr->biWidth = LE_GRAFIX_VirtualScreenWidthInPixels;
    bmiOutputHeaderPtr->biHeight = LE_GRAFIX_VirtualScreenHeightInPixels;
  }
  else  // use user input bitmap width and height
  {
    bmiOutputHeaderPtr->biWidth = ((LPBITMAPINFOHEADER) userOutputBitmapInfoPtr)->biWidth;
    bmiOutputHeaderPtr->biHeight = ((LPBITMAPINFOHEADER) userOutputBitmapInfoPtr)->biHeight;
  }

  // make a query to see if the video image decompressor supports
  // the desired video image output format
  if ( aviDataPtr->hIC && ICDecompressExQuery(
    aviDataPtr->hIC,
    0,                             // DWORD dwFlags,
    bmiInputHeaderPtr,             // LPBITMAPINFOHEADER lpbiSrc,
    NULL,                          // LPVOID lpSrc,
    0,                             // int xSrc,
    0,                             // int ySrc,
    bmiInputHeaderPtr->biWidth,    // int dxSrc,
    bmiInputHeaderPtr->biHeight,   // int dySrc,
    bmiOutputHeaderPtr,            // LPBITMAPINFOHEADER lpbiDst,
    NULL,                          // LPVOID lpDst,
    0,                             // int xDst,
    0,                             // int yDst,
    aviDataPtr->outputWidth,       // int dxDst,
    aviDataPtr->outputHeight)      // int dyDst
      != ICERR_OK )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "The output video format is not supported.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit; // ICDecompressExQuery () fails
  }

  // take advantage of Indeo video 5 codec specific features

  if ( aviDataPtr->hIC &&
    (aviDataPtr->videoStreamHeaderPtr->fccHandler ==
    mmioFOURCC ('I', 'V', '5', '0') ||
    aviDataPtr->videoStreamHeaderPtr->fccHandler ==
    mmioFOURCC ('i', 'v', '5', '0')) )
  {
    R4_DEC_SEQ_DATA  myR4SeqState; // indeo video sequence data structure

		// set the Indeo(R) video sequence data
    memset (&myR4SeqState, 0, sizeof (myR4SeqState));
    myR4SeqState.dwSize = sizeof(myR4SeqState);
    myR4SeqState.dwFourCC = mmioFOURCC('I','V','5','0');
    myR4SeqState.dwVersion = SPECIFIC_INTERFACE_VERSION;
    // action taken on codec current state
		myR4SeqState.mtType = MT_DECODE_SEQ_VALUE;
    myR4SeqState.oeEnvironment = OE_32; // for 32-bit applications

    // set the codec sequence state flag
		myR4SeqState.dwFlags = DECSEQ_VALID;

    // specify one of the scalability options
    myR4SeqState.dwFlags |= DECSEQ_SCALABILITY;
    myR4SeqState.eScalability = SC_DONT_DROP_FRAMES; // reduce image quality when needed
//    myR4SeqState.eScalability = SC_ON; // switch on the scalability effect
//    myR4SeqState.eScalability = SC_OFF; // switch off the scalability effect

    // Should we double the output video image size ?
    if ( (outputWidth == bmiInputHeaderPtr->biWidth * 2 &&
      outputHeight == bmiInputHeaderPtr->biHeight * 2) ||
      bDoubleImageSize == TRUE )
    {
      // double the destination image size ?
      if ( aviDataPtr->outputWidth * 2 <= LE_GRAFIX_VirtualScreenWidthInPixels &&
        aviDataPtr->outputHeight * 2 <= LE_GRAFIX_VirtualScreenHeightInPixels )
      {
        HRESULT        errorCode;          // for direct draw
        DDPIXELFORMAT  myPixelFormat;      // for direct draw

        // make sure that the current screen colour depth is not 32-bit
        memset (&myPixelFormat, 0, sizeof (myPixelFormat));
        myPixelFormat.dwSize = sizeof (myPixelFormat);
  
        errorCode = LE_GRAFIX_DDPrimarySurfaceN->GetPixelFormat(
          &myPixelFormat /* lpDDPixelFormat */);

        // get the screen colour depth
        aviDataPtr->destRGBBitCount = myPixelFormat.dwRGBBitCount;

        if ( errorCode == DD_OK && aviDataPtr->destRGBBitCount != 32 )
        {
          // set flag for doubling output video image size
          myR4SeqState.dwFlags |= DECSEQ_ALT_LINE;
          
          // alternate destination scanline ?
          if ( bAlternateScanLine == TRUE )
            myR4SeqState.fAltLine = TRUE; // switch on alternate scanline mode
          else
            myR4SeqState.fAltLine = FALSE; // switch on solid double sized image
          
          // double destination image size
          aviDataPtr->outputWidth *= 2;
          aviDataPtr->outputHeight *= 2;
        }
      }
    }

    // specify transparency feature
    myR4SeqState.dwFlags |= DECSEQ_FILL_TRANSPARENT;
//    myR4SeqState.fFillTransparentPixels = 0; // The transparent pixels are not written.
    myR4SeqState.fFillTransparentPixels = 1; // enable transparent pixel fill

    // send ICM_SETCODECSTATE to set the requested codec state
    if ( ICSendMessage (aviDataPtr->hIC, ICM_SETCODECSTATE,
      (DWORD) &myR4SeqState, sizeof(myR4SeqState)) != ICERR_OK )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "unable to set Indeo video 5 codec sequence data state.\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
    }

    // Now, we should do something about the decoder frame state.

		// set Indeo(R) video frame data
    memset (&aviDataPtr->myR4FrameState, 0, sizeof (aviDataPtr->myR4FrameState));
    aviDataPtr->myR4FrameState.dwSize = sizeof (aviDataPtr->myR4FrameState);
    aviDataPtr->myR4FrameState.dwFourCC = mmioFOURCC('I','V','5','0');
    aviDataPtr->myR4FrameState.dwVersion = SPECIFIC_INTERFACE_VERSION;
    aviDataPtr->myR4FrameState.mtType = MT_DECODE_FRAME_VALUE;
    aviDataPtr->myR4FrameState.oeEnvironment = OE_32; // for 32 bit OS

    // set the frame data flag
    aviDataPtr->myR4FrameState.dwFlags = DECFRAME_VALID;

    // if transparent, fill in a user specified colour ?
    // check with Indeo video 5 codec sequence data state
    if ( myR4SeqState.dwFlags & DECSEQ_FILL_TRANSPARENT &&
      myR4SeqState.fFillTransparentPixels == 1 )
    {
      aviDataPtr->myR4FrameState.dwFlags |= DECFRAME_FILL_COLOR;
      // Note that dwFillColor is not truly defined as COLORREF RGB,
      // although the documentation says so.
      // The colour should be 0x00RRGGBB. It is not RGB anyway.
      aviDataPtr->myR4FrameState.dwFillColor = 0x000000ff; // fill colour blue
    }

    // set the frame flag for Indeo video 5 scalability
    aviDataPtr->myR4FrameState.dwFlags |= DECFRAME_TIME_LIMIT;
    aviDataPtr->myR4FrameState.dwTimeLimit = 0; // use default frame rate

    // send a message to set the requested frame data state
    if ( ICSendMessage(aviDataPtr->hIC, ICM_SETCODECSTATE,
      (DWORD) &aviDataPtr->myR4FrameState, sizeof(aviDataPtr->myR4FrameState))
      != ICERR_OK )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
        "unable to set Indeo video 5 Codec frame data state.\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
    }
  } // end of Indeo video 5 specific

  DWORD    dFlag;

  // use hurryup instead of preroll
  dFlag = ICDECOMPRESS_NOTKEYFRAME | ICDECOMPRESS_NULLFRAME | ICDECOMPRESS_HURRYUP;
//  dFlag = ICDECOMPRESS_NOTKEYFRAME | ICDECOMPRESS_NULLFRAME | ICDECOMPRESS_PREROLL;

  // prepare decompressor for video image decompression
  if ( aviDataPtr->hIC && ICDecompressExBegin (
    aviDataPtr->hIC,
    dFlag,                         // DWORD dwFlags,
    bmiInputHeaderPtr,             // LPBITMAPINFOHEADER lpbiSrc,
    NULL,                          // LPVOID lpSrc,
    0,                             // int xSrc,
    0,                             // int ySrc,
    bmiInputHeaderPtr->biWidth,    // int dxSrc,
    bmiInputHeaderPtr->biHeight,   // int dySrc,
    bmiOutputHeaderPtr,            // LPBITMAPINFOHEADER lpbiDst,
    NULL,                          // LPVOID lpDst,
    0,                             // int xDst,
    0,                             // int yDst,
    aviDataPtr->outputWidth,       // int dxDst,
    aviDataPtr->outputHeight)      // int dyDst
      != ICERR_OK )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StartVideo (): "
      "ICDecompressExBegin () fails.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit; // ICDecompressExBegin () fails
  }
  
  // check to see if we need to flip the video image vertically
  // Note that this must be done after ICLocate ().
  if ( aviDataPtr->hIC && bFlipVertically == TRUE )
  {
    // change sign (+/-) for the output bitmap height to make the output
    // bitmap flip vertically
    aviDataPtr->bFlipVertically = TRUE;
    aviDataPtr->bmiOutputHeaderPtr->biHeight *= -1;
  }

  // for buf sound system (audio stuff)

  if ( !aviDataPtr->bSilentMovie )
  {
    // Find clock ticks needed for the audio internal buffer
    // (audio suggested buffer size).
    audioClockTicks = 
      aviDataPtr->audioStreamHeaderPtr->dwSuggestedBufferSize *
      CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ /
      aviDataPtr->audioStreamFormatPtr->nAvgBytesPerSec;

    audioOutputFormat = *aviDataPtr->audioStreamFormatPtr;
    audioOutputFormat.wFormatTag = WAVE_FORMAT_PCM;
    if (audioOutputFormat.nSamplesPerSec == 0)
      audioOutputFormat.nSamplesPerSec = LE_SOUND_CurrentHardwareHz;
    if (audioOutputFormat.wBitsPerSample != 8 && audioOutputFormat.wBitsPerSample != 16)
      audioOutputFormat.wBitsPerSample = LE_SOUND_CurrentHardwareBits;
    audioOutputFormat.nBlockAlign = (WORD)
      (audioOutputFormat.nChannels * (audioOutputFormat.wBitsPerSample / 8));
    audioOutputFormat.nAvgBytesPerSec =
      audioOutputFormat.nSamplesPerSec * audioOutputFormat.nBlockAlign;
    audioOutputFormat.cbSize = 0;

    // start the BufSound
    aviDataPtr->hBufSnd = LE_SOUND_StartBufSnd (
      aviDataPtr->audioStreamFormatPtr,  // input audio format
      &audioOutputFormat,                // &OutputFormat.format
      NULL,                              // outfilename
      FALSE,                             // Use 3D sound
      LED_EI,                            // LE_DATA_DataId CacheHintDataID
      FALSE,                             // BOOL KeepPermanentlyInCache
      FALSE,                             // BOOL *UsingCachedBuffer
      FALSE,                             // BOOL SoundFitsInBuffer
      audioClockTicks + 1,               // Internal buffer length in ticks - largest audio chunk size plus a bit of margin.
      8 * CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ + 1); // Direct sound buffer size, 8 seconds of sound
  
    if( aviDataPtr->hBufSnd == NULL )
    {
    #if CE_ARTLIB_EnableDebugMode
      LE_ERROR_DebugMsg ("LE_VIDEO_StartVideo (): "
        "LE_SOUND_StartBufSnd () fails.\n", LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit;  // LE_SOUND_StartBufSnd () fails
    }

    #if SILENT_MOVIE_WITH_BUFSND
      // Silent ?
      if ( !bEnableAudio )
        LE_SOUND_SetVolumeBufSnd (aviDataPtr->hBufSnd, 0 /* UNS8 VolumePercentage */);
    #endif
  }

  #if CE_ARTLIB_VideoFrameJumpModeEnabled
    // create a video index array
    PrepareVideoIndexForSelectiveJumpingMode (aviDataPtr);
    aviDataPtr->decisionFrameNumber = -1;
    #if !CE_ARTLIB_VideoMultiTaskingEnabled
      // Note that different approaches are used to protect
      // the main avi data structure from being accessed
      // simultaneously from other threads.
      // Messages are used for multitasking frame jump playback mode
      // while a critical section is used for the single-threaded
      // frame jump playback mode.
      InitializeCriticalSection (&aviDataPtr->myCriticalSection);
    #endif
  #endif

  #if CE_ARTLIB_VideoMultiTaskingEnabled
    // Here we create a second execution thread for avi data buffering purpose.
    // Initialise two message queues before we use them for data buffering.

    if (!LE_QUEUE_Initialize (
      &aviDataPtr->messagesToDataBufferingThread,
      5 /* Max number of pending messages */))
      goto ErrorExit;

    if (!LE_QUEUE_Initialize (
      &aviDataPtr->messagesToVideoControlThread,
      5 /* Max number of pending messages */))
      goto ErrorExit;

    // fire up the avi data buffering thread
    hDataBufferingThread = (HANDLE) _beginthreadex (
      NULL        /* Security */,
      0           /* Stack size */,
      &DataBufferingMainFunction,
      aviDataPtr  /* Initial arguments */,
      0           /* Creation flags, CREATE_SUSPTENDED to not run right away */,
      &nIDThread  /* OS thread identifier we don't use */);

    if (hDataBufferingThread == NULL)
      goto ErrorExit; // unable to create the avi data buffering thread

    // set a thread priority for the avi data buffering thread
    SetThreadPriority (hDataBufferingThread, CE_ARTLIB_GameThreadPriority);

    // We don't need the thread handle, so close it now rather than
    // worrying about closing it later.  Closing it doesn't stop
    // the thread.
    CloseHandle (hDataBufferingThread);

    // do the first round of avi data buffering

    // send a message to the avi data buffering thread for the first round
    // of avi raw data preloading
    LE_QUEUE_EnqueueWithParameters (
      &aviDataPtr->messagesToDataBufferingThread,
      LI_VIDEO_MESSAGE_LoadBuffer, 0, 0,
      0, 0, 0, 0, 0, NULL, 0);

    // wait until the first round of avi data buffering is done
    while (TRUE)
    {
      if ( !LE_QUEUE_WaitForMessage (
        &aviDataPtr->messagesToVideoControlThread,
        &myMessage) )
        goto ErrorExit; // Something went wrong.

      LE_QUEUE_FreeStorageAssociatedWithMessage (&myMessage);
    
      if ( myMessage.messageCode == LI_VIDEO_MESSAGE_LoadBufferDone )
        break; // OK, the first round of avi data buffering is done.
    }
  #endif

  // Get the video start time in ticks. This is only for test purpose.
  #if CE_ARTLIB_EnableDebugMode
    aviDataPtr->videoStartTimeInTicks = LE_TIME_TickCount;
  #endif

  // Successful. We have done the file processing and video/audio preparation work.
  // Now return a handle to the main avi data structure.
  return (LE_VIDEO_VideoHandle) aviDataPtr;

ErrorExit:

  // Deallocate all data buffers before leaving the funciton.

  // the generic data buffer
  if ( buffer != NULL )
  {
    LE_MEMORY_free (buffer);
    buffer = NULL;
  }

  // the main avi data structure
  if ( aviDataPtr != NULL )
  {
    ClearUpAVIDataRecord (aviDataPtr);

    // also free the memory allocated for the main avi data structure
    LE_MEMORY_free (aviDataPtr);
    aviDataPtr = NULL;
  }

  return NULL;  // return a null handle

} // LE_VIDEO_StartVideo ()


/*********************************************************************************
 *
 *  BOOL GetSuggestedAVIDataBufferSize (MyDataPointer aviDataPtr)
 *
 *  Now things are different. We need the largest audio chunk size for audio
 *  data feeding, and the largest avi chunk size for avi data (buffering)
 *  instead of for video data only.
 *
 *  Returns TRUE for success and FALSE for failure.
 *
 *********************************************************************************/

BOOL GetSuggestedAVIDataBufferSize (MyDataPointer aviDataPtr)
{
  DWORD           aviBufferSize;      // suggested avi data buffer size in bytes
  DWORD           aviDataSize;        // data size of current avi data chunk
  DWORD           aviChunkOffset;     // current avi data chunk offset

  DWORD           audioBufferSize;    // suggested audio buffer size in bytes

  short           indexEntrySize;     // 16 bytes ( 4 DWORD )
  AVIINDEXENTRY  *indexPtr;           // index position pointer
  AVIINDEXENTRY  *nextKeyIndexPtr;    // next key frame index pointer
  WORD            streamID;           // LOWORD (ckid)
  WORD            videoStreamID;
  WORD            audioStreamID;
  DWORD           bytesLeft;
  DWORD           recFOURCC;          // fourcc for rec chunk entry

  short           keyFrameCounter;       // always from one key frame to the next
                                         // key frame
  AVIINDEXENTRY  *userIndexPtr;          // index position pointer
  DWORD           userBufferSize;        // user preferred avi data buffer size
  DWORD           userDataSize;          // temp data size for user buffer size
  short           videoFramesPerSecond;  // video frames per second
  int             userFramesPerBuffer;   // user specified video frames per buffer
  int             videoFrames;           // keep track of number of video frames

  // check for validity
  if ( aviDataPtr == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "GetSuggestedAVIDataBufferSize (): "
      "aviDataPtr == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE;  // invalid aviDataPtr
  }

  // initialise local variables

  indexPtr = aviDataPtr->indexChunkDataBuffer;
  bytesLeft = aviDataPtr->indexChunkDataSize;
  videoStreamID = aviDataPtr->videoStreamID;
  audioStreamID = aviDataPtr->audioStreamID;
  recFOURCC = mmioFOURCC ('r', 'e', 'c', ' ');

  aviDataSize = 0;
  aviBufferSize = 0;
  audioBufferSize = 0;
  indexEntrySize = sizeof (AVIINDEXENTRY); // 16 bytes (4 DWORD)

  // get the user specified maximum number of video frames per avi data buffer
  userIndexPtr = indexPtr;
  userBufferSize = 0;
  userDataSize = 0;
  videoFramesPerSecond = (short) (
    (double) aviDataPtr->videoStreamHeaderPtr->dwRate /
    aviDataPtr->videoStreamHeaderPtr->dwScale + 0.5 );
  userFramesPerBuffer = videoFramesPerSecond * CE_ARTLIB_VideoMultipleSecondsData;

  // get the first chunk dwChunkOffset (relative to the file offset of
  // chunk 'LIST/size/movi'
  aviChunkOffset = indexPtr->dwChunkOffset;

  // go through the avi file index chunk and look for suggested data
  //   buffer sizes.
  // audioBufferSize is the largest audio chunk size of the data file.
  // aviBufferSize is the largest avi data chunk size, usually the avi
  //   data size from one video key frame up to the second video key frame,
  //   including all audio/video chunk headers and non- audio/video chunks.
  // userBufferSize is the largest avi data size for user specified
  //   CE_ARTLIB_VideoMultipleSecondsData seconds of avi data for each buffer.

  keyFrameCounter = 0;
  videoFrames = 0;

  while ( bytesLeft > 0 )
  {
    // get stream ID
    streamID = LOWORD (indexPtr->ckid);

    if ( streamID == videoStreamID ) // found a video stream
    {
      videoFrames ++; // count video frames

      // a video key frame ?
      if ( indexPtr->dwFlags & AVIIF_KEYFRAME )  // found a video key frame
      {
        keyFrameCounter ++;

        if ( keyFrameCounter > 1 )  // found the second video key frame
        {
          // We have found all subchunks of current avi data chunk, which
          // starts from a video key frame up to the second video key frame.

          // decrement indexPtr by 1 to get to the end chunk of
          // current avi data chunk
          nextKeyIndexPtr = indexPtr; // save the next key frame index pointer
          indexPtr --;
          bytesLeft += indexEntrySize; // compensate bytesLeft

          // decrement videoFrames by 1
          videoFrames --; // keep track of video frames

          // try to avoid any potential rec chunk index entry
          while ( indexPtr->ckid == recFOURCC ) // mmioFOURCC ('r', 'e', 'c', ' ')
          {
            nextKeyIndexPtr = indexPtr;  // save the next 'rec ' index pointer
            indexPtr --;                 // decrement indexPtr by 1
            bytesLeft += indexEntrySize; // compensate bytesLeft
          }

          // get the data size of current avi data chunk
          aviDataSize = indexPtr->dwChunkOffset - aviChunkOffset +
            indexPtr->dwChunkLength + 8 /* ckid + cksize */;

          // update the suggested avi data buffer size
          if ( aviBufferSize < aviDataSize )
            aviBufferSize = aviDataSize;

          // reset keyFrameCounter to 0
          keyFrameCounter = 0;

          // reset aviChunkOffset
          aviChunkOffset = nextKeyIndexPtr->dwChunkOffset;
        }
      }
    }
    else if ( streamID == audioStreamID )  // found an audio chunk index entry
    {
      // update the audio data suggested buffer size
      if ( audioBufferSize < indexPtr->dwChunkLength )
        audioBufferSize = indexPtr->dwChunkLength;
    }

    // get userDataSize
    if ( videoFrames >= userFramesPerBuffer )
    {
      // reset the video frame counter to zero
      videoFrames = 0;

      // work out the userDataSize
      userDataSize = indexPtr->dwChunkOffset - userIndexPtr->dwChunkOffset +
        8 /* ckid + cksize */ + indexPtr->dwChunkLength;

      if ( userBufferSize < userDataSize )
        userBufferSize = userDataSize;

      // update userIndexPtr
      userIndexPtr = indexPtr + 1;
    }

    // update index position and bytes left
    indexPtr ++;
    bytesLeft -= indexEntrySize;
  }

  // dealing with the last avi data chunk
  // decrement indexPtr by 1 to get to the end chunk of current avi data chunk
  indexPtr --;

  // Is this an audio chunk ?
  if ( LOWORD (indexPtr->ckid) == audioStreamID )
  {
    // The last chunk is an audio data chunk.
    // update audio buffer size
    if ( audioBufferSize < indexPtr->dwChunkLength )
      audioBufferSize = indexPtr->dwChunkLength;
  }

  // get the data size of the last avi data chunk
  aviDataSize = indexPtr->dwChunkOffset - aviChunkOffset +
    indexPtr->dwChunkLength + 8 /* ckid + cksize */;

  // update the suggested avi data buffer size
  if ( aviBufferSize < aviDataSize )
    aviBufferSize = aviDataSize;

  // Check again for the user specified CE_ARTLIB_VideoMultipleSecondsData
  // (seconds) of avi data buffering. The trick here is that if the film
  // is shorter than the user specified CE_ARTLIB_VideoMultipleSecondsData
  // seconds, both userBufferSize and userDataSize calculated above are zero.
  if ( userBufferSize == 0 )
  {
    // The film is actually shorter than the user specified data buffering time.
    // So, we need to recalcuate the userBufferSize.
    userBufferSize = indexPtr->dwChunkOffset - userIndexPtr->dwChunkOffset +
        8 /* ckid + cksize */ + indexPtr->dwChunkLength;
  }

  // get parameters
  if ( !aviDataPtr->bSilentMovie )
    aviDataPtr->audioStreamHeaderPtr->dwSuggestedBufferSize = audioBufferSize;
  if ( aviBufferSize >= userBufferSize )
    aviDataPtr->aviDataBufferSize = aviBufferSize;
  else // use userBufferSize
    aviDataPtr->aviDataBufferSize = userBufferSize;

  // successful
  return TRUE;

}  // end of GetSuggestedAVIDataBufferSize ()


/*****************************************************************************
 *
 *  BOOL LI_VIDEO_AllocatePreloadingDataBuffers (MyDataPointer aviDataPtr)
 *
 *  Allocates memory for preloading avi data buffers for both video and
 *  audio streams. The allocated memory should be big enough to contain
 *  at least one (or multiple) key frame to key frame avi data chunk(s),
 *  which starts from a video key frame and includes all the following
 *  delta frames and their associated audio (chunk) data.
 *
 *  The memory allocated here will be deallocated when the video is stopped
 *  by LE_VIDEO_StopVideo ().
 *
 *  Returns TRUE for success and FALSE for failure.
 *
 *****************************************************************************/

BOOL LI_VIDEO_AllocatePreloadingDataBuffers (MyDataPointer aviDataPtr)
{
  MyAVIDataBufferPointer tempDataBuffer; // for avi data buffers

  // check for aviDataPtr
  if ( aviDataPtr == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_AllocatePreloadingDataBuffers (): "
      "aviDataPtr == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE;  // garbage aviDataPtr
  }

  // Find suggested avi data buffer size for both video and audio streams.
  // Note: the suggested avi data buffer size is based upon key frame to key 
  //   frame avi data chunk size. The allocated memory should be big enough
  //   to contain at least one (or multiple) key frame to key frame avi data
  //   chunk(s), which starts from a video key frame and includes all the
  //   following delta frames and their associated audio (chunk) data.
  if ( GetSuggestedAVIDataBufferSize (aviDataPtr) == FALSE )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_AllocatePreloadingDataBuffers (): "
      "GetSuggestedAVIDataBufferSize () fails to get the suggested buffer size.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE;  // GetSuggestedAVIDataBufferSize () fails
  }

  // allocate memory for each of (AVI_DATA_BUFFER_NUMBER)
  // preloading avi data buffers

  for ( int i = 0; i < AVI_DATA_BUFFER_NUMBER; i++ )
  {
    // check for validity of avi data buffers
    if ( aviDataPtr->dataBuffer[i] != NULL )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_AllocatePreloadingDataBuffers (): "
        "aviDataPtr->dataBuffer[%d] != NULL.\n", i);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit; // Memory of the data buffer is already allocated.
    }

    // allocate memory for the avi data buffer

    // for aviDataPtr->dataBuffer[i]
    tempDataBuffer = aviDataPtr->dataBuffer[i] =
      (MyAVIDataBufferPointer) LE_MEMORY_malloc (sizeof(MyAVIDataBufferRecord));

    if ( tempDataBuffer == NULL )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_AllocatePreloadingDataBuffers (): "
        "fails to allocate %d bytes of memory for aviDataPtr->dataBuffer[%d].\n",
        sizeof (MyAVIDataBufferRecord), i);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit; // unable to allocate memory for aviDataPtr->dataBuffer[i]
    }
    
    // for aviDataBuffer
    tempDataBuffer->aviDataBuffer = (BYTE *) LE_MEMORY_malloc (aviDataPtr->aviDataBufferSize);
    if ( tempDataBuffer->aviDataBuffer == NULL )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer,
        "LI_VIDEO_AllocatePreloadingDataBuffers (): fails to allocate %d bytes "
        "of memory for aviDataPtr->dataBuffer[%d]->aviDataBuffer.\n",
        aviDataPtr->aviDataBufferSize, i);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      goto ErrorExit; // unable to allocate memory for aviDataBuffer
    }

    // initialise various members of the newly allocated avi data buffer
    tempDataBuffer->startFrameNumber = tempDataBuffer->endFrameNumber = -1;
    tempDataBuffer->bAudioData = FALSE;
    tempDataBuffer->bEmptyBuffer = TRUE;
    #if CE_ARTLIB_VideoFrameJumpModeEnabled
      // the data buffer not committed (for alternative frame)
      tempDataBuffer->bFrameJumpBuffer = FALSE;
      #if CE_ARTLIB_VideoMultiTaskingEnabled
        tempDataBuffer->bVoidBuffer = FALSE;
      #endif
    #endif
  }

  // successful
  return TRUE;

ErrorExit:

  // deallocate all allocated data buffers before error exit
  for ( i = 0; i < AVI_DATA_BUFFER_NUMBER; i++ )
  {
    if ( aviDataPtr->dataBuffer[i] != NULL )
    {
      // get a pointer to the data buffer
      tempDataBuffer = aviDataPtr->dataBuffer[i];

      // free the avi data buffer
      if ( tempDataBuffer->aviDataBuffer != NULL )
      {
        LE_MEMORY_free (tempDataBuffer->aviDataBuffer);
        tempDataBuffer->aviDataBuffer = NULL;
      }

      // free the avi data buffer itself
      LE_MEMORY_free (aviDataPtr->dataBuffer[i]);
      aviDataPtr->dataBuffer[i] = NULL;
    }
  }

  // Now, it is ok to exit
  return FALSE;

}  // LI_VIDEO_AllocatePreloadingDataBuffers ()


/*****************************************************************************
 *
 *  unsigned int __stdcall DataBufferingMainFunction (void *hVideoHandle)
 *
 *  This function serves as a start address of the avi data buffering
 *  execution thread, which communicates with the main video control
 *  thread through two message queues, messagesToDataBufferingThread
 *  and messagesToVideoControlThread.
 *
 *  Return:
 *    When it returns, the avi data buffering thread terminates and
 *    automatically frees all of the system resources allocated to
 *    the thread.
 *
 *****************************************************************************/

#if CE_ARTLIB_VideoMultiTaskingEnabled
unsigned int __stdcall DataBufferingMainFunction (void *hVideoHandle)
{
  LE_QUEUE_MessageRecord  myMessage;
  FILE                   *videoFile = NULL;
  MyDataPointer           aviDataPtr;
  #if CE_ARTLIB_VideoFrameJumpModeEnabled
    MyAVIDataBufferPointer  tempDataBuffer;  // for an avi data buffer
  #endif

  aviDataPtr = (MyDataPointer) hVideoHandle;  // Caller provides this pointer

  // Now we process requests from the main thread.  Goes to sleep when there
  // is no work to do.

  while (TRUE)
  {
    if (!LE_QUEUE_WaitForMessage (&aviDataPtr->messagesToDataBufferingThread,
      &myMessage))
    {
      LE_QUEUE_EnqueueWithParameters (
        &aviDataPtr->messagesToVideoControlThread,
        LI_VIDEO_MESSAGE_ErrorHappened, 0, 0, 0, 0, 0, 0, 0,
        A_T("DataBufferingThread: LE_QUEUE_WaitForMessage () fails"), 0);
      goto ErrorExit; // Something went wrong.
    }

    switch (myMessage.messageCode)
    {
      // load some avi data
    case LI_VIDEO_MESSAGE_LoadBuffer:
      {
        #if CE_ARTLIB_VideoFrameJumpModeEnabled
          if ( myMessage.numberA == CUT_VIDEO_TO_FRAME_LOAD )
          {
            // load one buffer immediately
            // reset some members of the main avi data structure
            aviDataPtr->bCutVideoToFrame = TRUE;
            aviDataPtr->nextVideoChunkIndexPtr = (AVIINDEXENTRY *) myMessage.numberB;
            aviDataPtr->dataBufferStartVideoFrameNumber = myMessage.numberC;

            // mark all voided data buffers empty
            for ( int i = 0; i < AVI_DATA_BUFFER_NUMBER; i++ )
            {
              // get an avi data buffer
              tempDataBuffer = aviDataPtr->dataBuffer[i];
              if ( tempDataBuffer->bVoidBuffer == TRUE )
              {
                tempDataBuffer->bEmptyBuffer = TRUE;
                tempDataBuffer->bFrameJumpBuffer = FALSE;
                tempDataBuffer->bVoidBuffer = FALSE;
              }
            }
          }
        #endif

        // load some avi data into avi data buffers
        if ( LI_VIDEO_FeedAVIVideoBuffers (
          (LE_VIDEO_VideoHandle) hVideoHandle) == FALSE )
        {
          #if CE_ARTLIB_EnableDebugMode
            LE_ERROR_DebugMsg ("DataBufferingMainFunction (): "
              "LI_VIDEO_FeedAVIVideoBuffers () fails.\n", LE_ERROR_DebugMsgToFileAndScreen);
          #endif

          LE_QUEUE_EnqueueWithParameters (
            &aviDataPtr->messagesToVideoControlThread,
            LI_VIDEO_MESSAGE_ErrorHappened, 0, 0, 0, 0, 0, 0, 0,
            A_T("DataBufferingThread: LI_VIDEO_FeedAVIVideoBuffers () fails"), 0);

          // free any possible storage associated with the message
          LE_QUEUE_FreeStorageAssociatedWithMessage (&myMessage);

          goto ErrorExit; // LI_VIDEO_FeedAVIVideoBuffers () fails
        }
      }
      break;

      // terminate the avi data buffering thread
    case LI_VIDEO_MESSAGE_ShutDown:

      #if CE_ARTLIB_EnableDebugMode
        LE_ERROR_DebugMsg ("DataBufferingMainFunction (): "
          "message LI_VIDEO_MESSAGE_ShutDown is processed.\n",
          LE_ERROR_DebugMsgToFile);
      #endif

      // To make sure that this ShutDownDone message won't get
      // lost due to queue full, we flush the message queue
      // before sending out this message.
      LE_QUEUE_Flush (&aviDataPtr->messagesToVideoControlThread);

      LE_QUEUE_EnqueueWithParameters (
        &aviDataPtr->messagesToVideoControlThread,
        LI_VIDEO_MESSAGE_ShutDownDone, 0, 0, 0, 0, 0, 0, 0, NULL, 0);
      
      // free any possible storage associated with the message
      LE_QUEUE_FreeStorageAssociatedWithMessage (&myMessage);
      goto NormalExit;
    }

    // free any possible storage associated with the message
    LE_QUEUE_FreeStorageAssociatedWithMessage (&myMessage);
  }

ErrorExit:

  return -500; // error return

NormalExit:

  return 500;  // ends the thread

} // DataBufferingMainFunction () -- start address of the avi data buffering thread
#endif


/*****************************************************************************
 *
 *  BOOL LI_VIDEO_FeedAVIVideoBuffers (LE_VIDEO_VideoHandle hVideoHandle)
 *
 *  Loads avi data into the video player's preallocated data buffers, with
 *  each avi data buffer preloading roughly the user specified multiple
 *  seconds (CE_ARTLIB_VideoMultipleSecondsData) worth of avi data.
 *
 *  For each avi data buffer, the first video frame must be a video key frame.
 *
 *  Returns TRUE for success, FALSE for failure.
 *
 *****************************************************************************/

BOOL LI_VIDEO_FeedAVIVideoBuffers (LE_VIDEO_VideoHandle hVideoHandle)
{
  MyDataPointer     aviDataPtr;           // pointer to the main avi data structure
  BOOL              bEndOfMoviDataChunk;  // flag for the end of chunk "LIST/movi"
  
  AVIINDEXENTRY    *indexBufferPtr;       // index buffer position
  AVIINDEXENTRY    *nextKeyFrameIndexPtr; // for the next key frame
  AVIINDEXENTRY    *tempNextKeyFrameIndexPtr; // for the next key frame
  DWORD             currentFilePosition;  // current file position

  MyAVIDataBufferPointer tempDataBuffer;  // for avi data buffers

  DWORD             aviDataBufferSize;    // avi data buffer size in bytes
  DWORD             videoFrameNumber;     // number of video frames to buffer
  DWORD             tempVideoFrameNumber; // number of video frames to buffer

  short             audioChunkNumber;     // audio chunks to buffer
  short             tempAudioChunkNumber; // audio chunks to buffer

  WORD              streamID;             // LOWORD (ckid)
  WORD              videoStreamID;        // stream ID for the video stream
  WORD              audioStreamID;        // stream ID for the audio stream
  DWORD             recFOURCC;            // mmioFOURCC ('r', 'e', 'c', ' ')
  DWORD             bytesToRead;          // bytes of avi data to read
  DWORD             tempBytesToRead;      // bytes of avi data to read
  DWORD             dataOffsetInBytes;    // offset from current file position
                                          // to where we start to read data
  short             keyFrameCounter;      // keep track of key frames
  short             dataBufferLimit;      // number of data buffers to buffer
  #if CE_ARTLIB_VideoFrameJumpModeEnabled
    short           fastBufferOnceOnly;   // for fast frame jump only
  #endif
  int               i;                    // an int
  
  // check for validity
  if ( hVideoHandle == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LI_VIDEO_FeedAVIVideoBuffers  (): hVideoHandle == NULL.\n",
      LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE; // found invalid LE_VIDEO_VideoHandle
  }

  // get a pointer to the main avi data structure
  aviDataPtr = (MyDataPointer) hVideoHandle;

  #if CE_ARTLIB_VideoFrameJumpModeEnabled && !CE_ARTLIB_VideoMultiTaskingEnabled
    // single threaded and frame jump playback mode
    EnterCriticalSection (&aviDataPtr->myCriticalSection);
  #endif

  // End of movi chunk data (chunk LIST/movi) reached ?
  if ( aviDataPtr->bEndOfMoviDataChunk == TRUE && aviDataPtr->bLoopMode == FALSE )
  {
#if CE_ARTLIB_VideoFrameJumpModeEnabled
    if ( aviDataPtr->bCutVideoToFrame == TRUE )
      aviDataPtr->bEndOfMoviDataChunk = FALSE; // erase end of movie mark
    else
#endif
      goto NormalExit;  // reached the end of 'movi' data chunk
  }

  // get avi data buffer size
  aviDataBufferSize = aviDataPtr->aviDataBufferSize;

  // get stream IDs for both the video and audio streams
  videoStreamID = aviDataPtr->videoStreamID;
  audioStreamID = aviDataPtr->audioStreamID;
  recFOURCC = mmioFOURCC ('r', 'e', 'c', ' ');

#if CE_ARTLIB_VideoFrameJumpModeEnabled && !CE_ARTLIB_VideoMultiTaskingEnabled
  // for single threaded mode only
  if ( aviDataPtr->bCutVideoToFrame == TRUE ) // immediate cut video to frame jump
  {
    // throw away all data buffers
    for ( i = 0; i < AVI_DATA_BUFFER_NUMBER; i++ )
    {
      // get an avi data buffer
      tempDataBuffer = aviDataPtr->dataBuffer[i];
      tempDataBuffer->bFrameJumpBuffer = FALSE;   // default data buffer
      tempDataBuffer->bEmptyBuffer = TRUE;
    }
  }
#endif

  // Go through all those avi data buffers. If found an empty data buffer,
  // feed some avi data into the data buffer.
  
  bEndOfMoviDataChunk = FALSE; // default to FALSE
  dataBufferLimit = 0;

  for ( i = 0; i < AVI_DATA_BUFFER_NUMBER; i++ )
  {
    // get an avi data buffer
    tempDataBuffer = aviDataPtr->dataBuffer[i];

    // Is this data buffer marked empty ?
    if ( tempDataBuffer->bEmptyBuffer == FALSE )  // The data buffer is not empty.
      continue; // skip this avi data buffer

    #if CE_ARTLIB_VideoFrameJumpModeEnabled
      if ( aviDataPtr->bCutVideoToFrame == TRUE )
      {
        // do one fast data buffering
        fastBufferOnceOnly = 0;
        // a committed frame jump buffer
        tempDataBuffer->bFrameJumpBuffer = TRUE;
      }
    #endif

    // get next video chunk index pointer
    indexBufferPtr = aviDataPtr->nextVideoChunkIndexPtr;

    // initialise some local variables
    bytesToRead = 0;
    videoFrameNumber = 0;
    audioChunkNumber = 0;

    // get current file position
    currentFilePosition = aviDataPtr->currentFilePosition;

    // find the file offset of current chunk
    dataOffsetInBytes =
      aviDataPtr->moviChunkDataOffset + indexBufferPtr->dwChunkOffset;

    // find how many bytes we need to seek from current file position to
    // the place where we start to read avi data
    dataOffsetInBytes -= currentFilePosition;
    
    if ( dataOffsetInBytes != 0 )  // we need to seek dataOffsetInBytes bytes
    {
      if ( mmioSeek (aviDataPtr->aviSource.file, (LONG) dataOffsetInBytes, SEEK_CUR) == -1 )
      {
      #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_FeedAVIVideoBuffers (): "
          "unable to seek %d bytes in %s.\n",
          dataOffsetInBytes, aviDataPtr->aviSource.name);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
      #endif
        goto ErrorExit;  // unable to mmioSeek
      }

      // update currentFilePosition
      currentFilePosition += dataOffsetInBytes;
    }

    // save the avi data buffer start index position
    tempDataBuffer->startDataBufferIndexPtr = indexBufferPtr;

    // save audioDataIndexPtr for the avi data buffer
    tempDataBuffer->audioDataIndexPtr = indexBufferPtr;

    // Try to feed some avi data into the specified avi data buffer.

    while (1) // for multiple avi data chunk buffering
    {
      keyFrameCounter = 0;       // keep track of key frame number
      tempVideoFrameNumber = 0;  // count video frames up to next key frame
      tempAudioChunkNumber = 0;  // count audio chunks up to next video key frame

      while (2)  // looking for the next video key frame
      {
        // End of chunk LIST/movi reached ?
        if ( (BYTE *) indexBufferPtr >= (BYTE *) aviDataPtr->indexChunkDataBuffer +
          aviDataPtr->indexChunkDataSize )
        {
          // signal that the end of 'movi' data is reached.
          bEndOfMoviDataChunk = TRUE;
          break;  // end of 'movi' data is reached
        }

        // get stream ID
        streamID = LOWORD (indexBufferPtr->ckid);

        // A video frame or an audio chunk ?
        if ( streamID == videoStreamID ) // found a video stream
        {
          // a video key frame ?
          if ( indexBufferPtr->dwFlags & AVIIF_KEYFRAME ) // found a key frame
          {
            keyFrameCounter ++;
        
            // check for key frame status
            if ( keyFrameCounter > 1 )  // found the next key frame
            {
              // save the index pointer for the next key frame
              tempNextKeyFrameIndexPtr = indexBufferPtr;

              // decrement indexBufferPtr by 1
              indexBufferPtr --;

              // avoid any potential rec chunk, mmioFOURCC ('r', 'e', 'c', ' ')
              while ( indexBufferPtr->ckid == recFOURCC )
              {
                // update tempNextKeyFrameIndexPtr
                tempNextKeyFrameIndexPtr = indexBufferPtr;
                indexBufferPtr --;
              }
              break; // Have found the next video key frame.
            }
          }

          // update video frame number
          tempVideoFrameNumber ++;
        }
        else if ( streamID == audioStreamID )  // an audio stream
          tempAudioChunkNumber ++; // count audio chunk numbers

        // increment indexBufferPtr by 1
        indexBufferPtr ++;
      } // end of while (2) statement

      // check for the end of movi chunk
      if ( bEndOfMoviDataChunk == TRUE )
        indexBufferPtr --; // decrement by 1 to get back to the end data chunk

      // find out how many bytes of avi data to read
      tempBytesToRead = aviDataPtr->moviChunkDataOffset +
        indexBufferPtr->dwChunkOffset +
        8 /* ckid + cksize */ +
        indexBufferPtr->dwChunkLength;
      tempBytesToRead -= currentFilePosition;

      if ( tempBytesToRead <= 0 )
      {
      #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_FeedAVIVideoBuffers  (): "
          "no data to read (%d bytes).\n", tempBytesToRead);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      #endif
        goto ErrorExit;  // no data to read
      }

      // Do we have enough space to read in the data ?
      if ( tempBytesToRead <= aviDataPtr->aviDataBufferSize )
      {
        #if CE_ARTLIB_VideoFrameJumpModeEnabled
          if ( tempDataBuffer->bFrameJumpBuffer == TRUE )
          {
            fastBufferOnceOnly ++;
            if ( fastBufferOnceOnly > 1 &&
              tempBytesToRead >= aviDataPtr->aviDataBufferSize / 3 )
              break; // enough data for cut-video-to-frame jump
          }
        #endif

        // Yes, we have got enough buffer space to read in the data.
        // update various variables
        bytesToRead = tempBytesToRead;  // get total amount of bytes to read
        indexBufferPtr = nextKeyFrameIndexPtr = tempNextKeyFrameIndexPtr;
        videoFrameNumber += tempVideoFrameNumber;
        audioChunkNumber += tempAudioChunkNumber;

        if ( bytesToRead == aviDataPtr->aviDataBufferSize )
          break;  // We have got enough avi data to read.
      }
      else  // no space to read in tempBytesToRead
        break; // The avi data buffer is full.

      // check for the end of movi chunk
      if ( bEndOfMoviDataChunk == TRUE )
        break; // end of the film reached

    } // end of while (1)

    // read avi data into aviDataBuffer
    if ( mmioRead (aviDataPtr->aviSource.file,
      (char *) tempDataBuffer->aviDataBuffer,
      bytesToRead)
      != (LONG) bytesToRead )
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_FeedAVIVideoBuffers (): "
        "unable to read %d bytes of avi data into aviDataBuffer in %s.\n",
        bytesToRead, aviDataPtr->aviSource.name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
    #endif
      goto ErrorExit; // mmioRead () fails
    }

    // update currentFilePosition
    currentFilePosition += bytesToRead;

    // update changes
    aviDataPtr->currentFilePosition = currentFilePosition;

    // update data buffer startFrameNumber and endFrameNumber
    tempDataBuffer->startFrameNumber = aviDataPtr->dataBufferStartVideoFrameNumber;
    tempDataBuffer->endFrameNumber = tempDataBuffer->startFrameNumber +
      videoFrameNumber;

    // for buffered audio data
    tempDataBuffer->audioChunkNumber = audioChunkNumber;
    if ( !aviDataPtr->bSilentMovie )
    {
      if ( audioChunkNumber > 0 )
        tempDataBuffer->bAudioData = TRUE;
      else
        tempDataBuffer->bAudioData = FALSE;
    }
    else
      tempDataBuffer->bAudioData = FALSE;

    // find the first video frame of the data buffer
    tempDataBuffer->videoFrameIndexPtr = tempDataBuffer->startDataBufferIndexPtr;
    while (TRUE)
    {
      if ( LOWORD (tempDataBuffer->videoFrameIndexPtr->ckid) == videoStreamID )
        break; // found the first video frame
      else
        tempDataBuffer->videoFrameIndexPtr ++;
    }

    // get the startVideoFramePtr and videoFramePtr
    tempDataBuffer->startVideoFramePtr = tempDataBuffer->videoFramePtr =
      tempDataBuffer->aviDataBuffer +
      tempDataBuffer->videoFrameIndexPtr->dwChunkOffset -
      tempDataBuffer->startDataBufferIndexPtr->dwChunkOffset +
      8 /* ckid + cksize */;
    tempDataBuffer->startVideoFrameIndexPtr = tempDataBuffer->videoFrameIndexPtr;
    
    // update the dataBufferStartVideoFrameNumber
    aviDataPtr->dataBufferStartVideoFrameNumber = tempDataBuffer->endFrameNumber;
    
    // mark the data buffer ready for use
    tempDataBuffer->bEmptyBuffer = FALSE;
    
    if ( bEndOfMoviDataChunk == FALSE )
      aviDataPtr->nextVideoChunkIndexPtr = nextKeyFrameIndexPtr;
    else // end of movi data chunk reached
    {
      // The end of movi data chunk is hit by the normal sequencial playback
      // mode data buffering. So,
      // rewind to the beginning of the index chunk buffer
      aviDataPtr->nextVideoChunkIndexPtr = aviDataPtr->indexChunkDataBuffer;
      break;
    }

    #if CE_ARTLIB_VideoFrameJumpModeEnabled
      if ( tempDataBuffer->bFrameJumpBuffer == TRUE )
      {
        if ( aviDataPtr->bCutVideoToFrame == TRUE )
          aviDataPtr->bCutVideoToFrame = FALSE;

        // for CutVideoToFrame jump action, we do one half
        // data buffer quick loading
        break;
      }
    #endif

    // limit number of data buffers to preload the avi data
    // This may be important when playing back a big video on
    // a slow machine.
    #if CE_ARTLIB_VideoFrameJumpModeEnabled
      if ( ++ dataBufferLimit > 0 ) // for frame jump mode, we need speed
      break; // We have already done two data buffers.
    #endif
//    #else
//      if ( ++ dataBufferLimit > 2 ) // for normal sequential mode, we need quality.
//    #endif
//      break; // We have already done two data buffers.
    
  } // end of for statement

  #if CE_ARTLIB_VideoMultiTaskingEnabled
    // send a message to the main video control thread to indicate that
    // this round of avi data buffering is done
    LE_QUEUE_EnqueueWithParameters (
      &aviDataPtr->messagesToVideoControlThread,
      LI_VIDEO_MESSAGE_LoadBufferDone,
      0, 0, 0, 0, 0, 0, 0, NULL, 0);
  #endif

  // Have we reached the end of movi data chunk ?
  if ( bEndOfMoviDataChunk == TRUE && aviDataPtr->bLoopMode == FALSE )
    aviDataPtr->bEndOfMoviDataChunk = TRUE;

NormalExit:
  #if CE_ARTLIB_VideoFrameJumpModeEnabled && !CE_ARTLIB_VideoMultiTaskingEnabled
    LeaveCriticalSection (&aviDataPtr->myCriticalSection);
  #endif
  // successful
  return TRUE;

ErrorExit:
  #if CE_ARTLIB_VideoFrameJumpModeEnabled && !CE_ARTLIB_VideoMultiTaskingEnabled
    LeaveCriticalSection (&aviDataPtr->myCriticalSection);
  #endif
  return FALSE;

}  // LI_VIDEO_FeedAVIVideoBuffers ()


/*****************************************************************************
 *
 *  BOOL LE_VIDEO_FeedVideo (
 *    LE_VIDEO_VideoHandle hVideoHandle,
 *    UNS32    *frameNumberPtr,     // current (decompressed) frame number
 *    LPBYTE    outputBitmapPtr,    // user specified frame buffer
 *    INT32     offsetX,            // x offset to the upper left corner
 *    INT32     offsetY,            // y offset to the upper left cornet
 *    short     brightness,         // -255 to 255
 *    short     contrast,           // -255 to 255
 *    short     saturation,         // -255 to 255
 *    LPBOOL    bEndVideo)          // TRUE when a video clip is finished.
 *
 *  It arranges avi data preloading, feeds audio data into the buf sound
 *  system, fetches a video frame which corresponds to the current
 *  sound and gets the video frame properly decompressed.
 *
 *  When *bEndVideo is TRUE, the movie is finished.
 *
 *  Returns TRUE for success, FALSE for failure.
 *
 *****************************************************************************/

BOOL LE_VIDEO_FeedVideo (
  LE_VIDEO_VideoHandle hVideoHandle,
  UNS32    *frameNumberPtr,
  LPBYTE    outputBitmapPtr,
  INT32     offsetX,
  INT32     offsetY,
  short     brightness,
  short     contrast,
  short     saturation,
  LPBOOL    bEndVideo)
{
  MyDataPointer          aviDataPtr;        // pointer to main avi data structure
  MyAVIDataBufferPointer tempDataBuffer;    // for avi data buffers
  #if CE_ARTLIB_VideoMultiTaskingEnabled
    LE_QUEUE_MessageRecord myMessage;       // message data structure
  #endif

  LPBYTE             audioDataBufferPtr;    // pointer to audio data buffer
  AVIINDEXENTRY     *audioDataIndexPtr;     // audio index position
  LONG               audioEndPosition;      // pass end audio frame position
  BOOL               InputUsed = FALSE;
  BOOL               DirectSoundBufferIsFull = FALSE;
  DWORD              audioDataSize;         // audio data chunk size
  DWORD              audioFramesPerChunk;   // audio data in terms of frames
  BOOL               bFlag;

  LONG               audioCurrentFrame;     // for video image update

  if ( hVideoHandle == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    LE_ERROR_DebugMsg ("LE_VIDEO_FeedVideo (): hVideoHandle == NULL.\n",
      LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE; // found an invalid video handle
  }

  // get a pointer to the main avi data structure
  aviDataPtr = (MyDataPointer) hVideoHandle;

  // for Indeo video 5 codec frame specific features only
  if ( aviDataPtr->videoStreamHeaderPtr->fccHandler == mmioFOURCC ('I', 'V', '5', '0') ||
    aviDataPtr->videoStreamHeaderPtr->fccHandler == mmioFOURCC ('i', 'v', '5', '0') )
  {
    // new colour effects (brightness, contrast, and saturation) ?
    if ( aviDataPtr->hIC && 
      (brightness != aviDataPtr->brightness ||
      contrast != aviDataPtr->contrast ||
      saturation != aviDataPtr->saturation) )
    {
      if ( brightness != aviDataPtr->brightness )
      {
        aviDataPtr->brightness = brightness;
        aviDataPtr->myR4FrameState.dwFlags |= DECFRAME_BRIGHTNESS;
        aviDataPtr->myR4FrameState.lBrightness = brightness;
      }
    
      if ( contrast != aviDataPtr->contrast )
      {
        aviDataPtr->contrast = contrast;
        aviDataPtr->myR4FrameState.dwFlags |= DECFRAME_CONTRAST;
        aviDataPtr->myR4FrameState.lContrast = contrast;
      }
    
      if ( saturation != aviDataPtr->saturation )
      {
        aviDataPtr->saturation = saturation;
        aviDataPtr->myR4FrameState.dwFlags |= DECFRAME_SATURATION;
        aviDataPtr->myR4FrameState.lSaturation = saturation;
      }

      // send a message to set the requested frame data state
      if ( ICSendMessage(aviDataPtr->hIC, ICM_SETCODECSTATE,
        (DWORD) &aviDataPtr->myR4FrameState, sizeof(aviDataPtr->myR4FrameState))
        != ICERR_OK )
      {
        #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_FeedVideo (): "
            "unable to set Indeo video 5 Codec frame data state.\n");
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
        #endif
      }
    }
  } // end of Indeo video 5 codec specific features

  #if CE_ARTLIB_VideoMultiTaskingEnabled
    // Before doing anything, we should check for messages from the avi data
    // buffering thread
    while (TRUE)
    {
      // check for message from the avi data buffering thread
      if (!LE_QUEUE_Dequeue (&aviDataPtr->messagesToVideoControlThread,
        &myMessage))
        break;  // The message queue is empty.

      // Have received a message
      switch (myMessage.messageCode)
      {
      #if CE_ARTLIB_VideoFrameJumpModeEnabled
        case LI_VIDEO_MESSAGE_CUT_VIDEO_TO_FRAME:
          {
            // void all data buffers immediately, except
            // for committed frame jump data buffer
            for ( int k = 0; k < AVI_DATA_BUFFER_NUMBER; k++ )
            {
              tempDataBuffer = aviDataPtr->dataBuffer[k];
              tempDataBuffer->bVoidBuffer = TRUE;
            }

            // Since this is CutVideoToFrame jump action, we need to
            // reset audioFrameNumber in order to get any subsequent
            // video alternatives to work correctly.
            aviDataPtr->audioFrameNumber = - 1;

            // clear the message queue before sending the message
            LE_QUEUE_Flush (&aviDataPtr->messagesToDataBufferingThread);

            // send a message to the avi data buffering thread
            // to load data for cut video to frame
            LE_QUEUE_EnqueueWithParameters (
              &aviDataPtr->messagesToDataBufferingThread,
              LI_VIDEO_MESSAGE_LoadBuffer, 0, 0,
              CUT_VIDEO_TO_FRAME_LOAD,
              myMessage.numberA /* nextVideoChunkIndexPtr */,
              myMessage.numberB /* jumpToFrameNumber */,
              0, 0, NULL, 0);
          }
          break;

          // set up an alternative frame for the next
          // possible frame jump action
        case LI_VIDEO_MESSAGE_ALTERNATIVE_FRAME_SETUP:
          {
            // reset some members of the main avi data structure
            aviDataPtr->bTakeAlternative = FALSE;
            aviDataPtr->jumpToFrameNumber = myMessage.numberA;
            aviDataPtr->decisionFrameNumber = myMessage.numberB;
          }
          break;

        case LI_VIDEO_MESSAGE_FORGET_VIDEO_ALTERNATIVE:
          aviDataPtr->decisionFrameNumber = -1;
          break;

        case LI_VIDEO_MESSAGE_CHANGE_VIDEO_ALTERNATIVE:
          aviDataPtr->bTakeAlternative = myMessage.numberA;
          break;

      #endif

      case LI_VIDEO_MESSAGE_ErrorHappened:

        #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_FeedVideo (): "
            "%s.\n", myMessage.stringA);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
        #endif

        // free any possible message associated storage
        LE_QUEUE_FreeStorageAssociatedWithMessage (&myMessage);      
        return FALSE; // Something went wrong.

      case LI_VIDEO_MESSAGE_ShutDownDone:

        #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_FeedVideo (): "
            "message from the avi data buffering thread is -- "
            "LI_VIDEO_MESSAGE_ShutDownDone.\n");
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
        #endif

        // free any possible message associated storage
        LE_QUEUE_FreeStorageAssociatedWithMessage (&myMessage);      
        return FALSE; // Something went wrong.
      }

      // free any possible message associated storage
      LE_QUEUE_FreeStorageAssociatedWithMessage (&myMessage);      
    }
  #else
    // Here we do normal single-threaded avi data buffering.
    // preload some more avi data into data buffers
    if ( LI_VIDEO_FeedAVIVideoBuffers (hVideoHandle) == FALSE )
    {
    #if CE_ARTLIB_EnableDebugMode
      LE_ERROR_DebugMsg ("LE_VIDEO_FeedVideo (): "
        "LI_VIDEO_FeedAVIVideoBuffers () fails.\n", LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      return FALSE; // LI_VIDEO_FeedAVIVideoBuffers () fails
    }
  #endif

  #if CE_ARTLIB_VideoFrameJumpModeEnabled

    // deal with automatic frame jump caused by alternative frame
    // setup and decisionFrameNumber
    // Have we passed the decision frame ?
    if (aviDataPtr->decisionFrameNumber >= 0 &&
      aviDataPtr->audioFrameNumber >= aviDataPtr->decisionFrameNumber )
    {
      if ( aviDataPtr->bTakeAlternative == TRUE )
      {
        #if !CE_ARTLIB_VideoMultiTaskingEnabled
          // for thread safety
          EnterCriticalSection (&aviDataPtr->myCriticalSection); // for thread safety
        #endif
        
        // cut video to aviDataPtr->jumpToFrameNumber
        LE_VIDEO_CutVideoToFrame (hVideoHandle,
          aviDataPtr->jumpToFrameNumber);

        #if !CE_ARTLIB_VideoMultiTaskingEnabled
          // Here we do normal single-threaded avi data buffering.
          if ( LI_VIDEO_FeedAVIVideoBuffers (hVideoHandle) == FALSE )
          {
          #if CE_ARTLIB_EnableDebugMode
            LE_ERROR_DebugMsg ("LE_VIDEO_FeedVideo (): "
              "LI_VIDEO_FeedAVIVideoBuffers () fails.\n",
              LE_ERROR_DebugMsgToFileAndScreen);
          #endif
            return FALSE; // LI_VIDEO_FeedAVIVideoBuffers () fails
          }
        #endif

        #if !CE_ARTLIB_VideoMultiTaskingEnabled
          // It is now safe to leave the critical section
          LeaveCriticalSection (&aviDataPtr->myCriticalSection);
        #endif
      }

      // send a message to the Event Queue notifying
      // that the decision frame has been played
      LE_UIMSG_SendEvent (UIMSG_VIDEO_JUMP,
        aviDataPtr->decisionFrameNumber,
        aviDataPtr->jumpToFrameNumber,
        aviDataPtr->bTakeAlternative,
        0, 0, NULL, 0);

      // The alternative aciton is taken.
      aviDataPtr->decisionFrameNumber = -1;
    }
  #endif

  // a silent movie ?
  if ( !aviDataPtr->bSilentMovie )
  {
  // Go through all preloaded avi data buffers and feed audio data into
  // the sound system until either there are no more preloaded audio data
  // available to feed or the internal sound buffer is full.

  for ( int i = 0; i < AVI_DATA_BUFFER_NUMBER; i++ )
  {
    BOOL   bCutToFrameAudioFeeding = FALSE; // default to FALSE

    // get a pointer to an avi data buffer
    tempDataBuffer = aviDataPtr->dataBuffer[i];

    #if CE_ARTLIB_VideoFrameJumpModeEnabled && CE_ARTLIB_VideoMultiTaskingEnabled
      // Is it a voided data buffer ?
      if ( tempDataBuffer->bVoidBuffer == TRUE )
        continue; // skip the voided data buffer
    #endif
    
    // interpret the data buffer status
    if ( tempDataBuffer->bEmptyBuffer == TRUE )
      continue; // skip this data buffer

    #if CE_ARTLIB_VideoFrameJumpModeEnabled && CE_ARTLIB_VideoMultiTaskingEnabled
      if ( tempDataBuffer->bFrameJumpBuffer == TRUE ) // a frame jump data buffer
      {
        // Reset the audioStartPosition for audio data feeding.
        aviDataPtr->audioStartPosition = tempDataBuffer->startFrameNumber;
        // reset the startVideoFrameForNextAudioFeed
        aviDataPtr->startVideoFrameForNextAudioFeed = tempDataBuffer->startFrameNumber;
      }
    #endif

    // Check to see if this avi data buffer is the right data buffer
    // to feed the sound system.
    if ( tempDataBuffer->startFrameNumber !=
      aviDataPtr->startVideoFrameForNextAudioFeed )
      continue; // This buffer is not next to the previous fed buffer

    // Yes, we have found the data buffer to feed the sound system.
    if ( tempDataBuffer->bAudioData == FALSE ) // No audio data available.
    {
      if ( tempDataBuffer->startFrameNumber == aviDataPtr->startVideoFrameForNextAudioFeed )
        aviDataPtr->startVideoFrameForNextAudioFeed = tempDataBuffer->endFrameNumber; // audioEndPosition;
      continue; // carry on searching for the next data buffer for audio feeding
    }
    
    #if CE_ARTLIB_VideoFrameJumpModeEnabled
      if ( tempDataBuffer->bFrameJumpBuffer == TRUE ) // a frame jump data buffer
      {
        tempDataBuffer->bFrameJumpBuffer = FALSE;
        // set up the audio cut to frame feeding flag
        bCutToFrameAudioFeeding = TRUE;
        aviDataPtr->audioFrameNumber = tempDataBuffer->startFrameNumber - 1;
      }
    #endif
    
    // Yes, this data buffer has got some audio data available to feed.    
    // use video frame number to mark the audio start and end positions
    // -- audio data feeding time stamps

    // try to feed all audio chunks in the avi data buffer
    while (1)
    {
      // get a pointer to the avi data buffer
      audioDataBufferPtr = tempDataBuffer->aviDataBuffer;

      // get a pointer to the audio index position
      audioDataIndexPtr = tempDataBuffer->audioDataIndexPtr;

      // find next audio chunk index entry
      while (2)
      {
        if ( LOWORD (audioDataIndexPtr->ckid) == aviDataPtr->audioStreamID )
          break; // found one audio data chunk
        else
          // skip all non-audio chunks
          audioDataIndexPtr ++;
      }

      // found an audio data chunk, get the audio data
      // note: we should include 8 bytes offset for ckid and cksize
      audioDataBufferPtr += audioDataIndexPtr->dwChunkOffset -
        tempDataBuffer->startDataBufferIndexPtr->dwChunkOffset + 8;

      // get the audio chunk data size
      audioDataSize = audioDataIndexPtr->dwChunkLength;

      // figure out how many audio frames the audio data correspond
      audioFramesPerChunk = (LONG) ((double) audioDataSize *
        aviDataPtr->videoStreamHeaderPtr->dwRate /
        aviDataPtr->videoStreamHeaderPtr->dwScale /
        aviDataPtr->audioStreamFormatPtr->nAvgBytesPerSec + 0.5);

      // work out the start/end time stamp of this audio data feeding
      audioEndPosition = aviDataPtr->audioStartPosition + audioFramesPerChunk; // pass end position

      // Feed the audio data to the sound system.
      bFlag = LE_SOUND_FeedBufSnd (
        aviDataPtr->hBufSnd,
        audioDataSize,               // InputBufferByteSize
        audioDataBufferPtr,          // InputBufferPtr
        &InputUsed,                  // BOOL *InputUsed
        0,                           // DWORD OutputBufferByteSize
        NULL,                        // BYTE *OutputBufferPtr
        NULL,                        // DWORD *OutputSizeUsed
        &DirectSoundBufferIsFull,    // BOOL  *DirectSoundBufferIsFull
        aviDataPtr->audioStartPosition, // StartTime
        audioEndPosition,            // PastEndTime
        bCutToFrameAudioFeeding);    // BOOL CutToThisFeedRightNow
    
      if ( bFlag == FALSE )
      {
      #if CE_ARTLIB_EnableDebugMode
        LE_ERROR_DebugMsg ("LE_VIDEO_FeedVideo (): "
          "LE_SOUND_FeedBufSnd () fails.\n", LE_ERROR_DebugMsgToFileAndScreen);
      #endif
        return FALSE;  // LE_SOUND_FeedBufSnd () fails
      }

      if (!InputUsed)
      {
        // The internal sound buffer is full and the audio data
        // are not fed. So, we should save the audio data index
        // pointer for the next round of audio data feeding.
        tempDataBuffer->audioDataIndexPtr = audioDataIndexPtr;
        break; // The buf sound internal buffer is full.
      }

      #if CE_ARTLIB_VideoFrameJumpModeEnabled
        if ( bCutToFrameAudioFeeding == TRUE )
          // set back to (default) normal audio data feeding mode
          bCutToFrameAudioFeeding = FALSE;
      #endif

      // update audioStartPosition
      aviDataPtr->audioStartPosition = audioEndPosition;

      // save the audio index pointer for the next audio chunk feeding
      // note: It should be the index entry next to current entry
      tempDataBuffer->audioDataIndexPtr = audioDataIndexPtr + 1;

      if ( -- tempDataBuffer->audioChunkNumber <= 0 )
        break;
    } // end of while (1)

    // check for audio data feeding successfulness for each avi data buffer
    if (InputUsed)
    {
      // The audio data feeding is successful.
      // Now update the audio feeding start frame number.
      aviDataPtr->startVideoFrameForNextAudioFeed = tempDataBuffer->endFrameNumber; // audioEndPosition;

      // mark the end of audio data feeding status
      tempDataBuffer->bAudioData = FALSE;
    }
    else
      break; // The buf sound internal buffer is full.

  } // end of for statement (feeding audio data into the sound system)

  // Have we reached the end of movi chunk data ?
  if ( aviDataPtr->bEndOfMoviDataChunk == TRUE && aviDataPtr->bLoopMode == FALSE )
  {
    BOOL   bAudioDataFinished;

    // Yes, we have finished avi data buffering.
    // Any audio data left unfed ?

    bAudioDataFinished = TRUE;

    for ( i = 0; i < AVI_DATA_BUFFER_NUMBER; i++ )
    {
      // get a pointer to an avi data buffer
      tempDataBuffer = aviDataPtr->dataBuffer[i];

#if CE_ARTLIB_VideoFrameJumpModeEnabled
      if ( tempDataBuffer->bFrameJumpBuffer == FALSE )
#endif
      {
        if ( tempDataBuffer->bAudioData == TRUE )
          bAudioDataFinished = FALSE; // We still have some unfed audio data.
      }
    }

    if ( bAudioDataFinished == TRUE )
    {
      // From now on, each time we should feed some zeros into the sound
      // buffer. As we still have got some valid audio data in the sound
      // buffer, fill the remaining part of the sound buffer with silence
      // until the sound data finally gets out the speakers.
      
      audioEndPosition = aviDataPtr->audioStartPosition; // pass end frame
      aviDataPtr->videoStreamHeaderPtr->dwLength = (DWORD) audioEndPosition;

      for ( int k = 0; k < 2; k++ )
      {
        LE_SOUND_FeedBufSnd (
          aviDataPtr->hBufSnd,         // BufSnd handle
          0,                           // InputBufferByteSize
          NULL,                        // InputBufferPtr
          &InputUsed,                  // BOOL *InputUsed
          0,                           // DWORD OutputBufferByteSize
          NULL,                        // BYTE *OutputBufferPtr
          NULL,                        // DWORD *OutputSizeUsed
          &DirectSoundBufferIsFull,    // BOOL  *DirectSoundBufferIsFull
          aviDataPtr->audioStartPosition, // StartTime
          audioEndPosition,            // PastEndTime
          FALSE);                      // BOOL CutToThisFeedRightNow

        if ( InputUsed == TRUE )
          break;
      }
    }
  } // end of movi chunk data ?
  
  // We have done this round of avi data preloading and have fed some
  // more audio data into the buf sound system.
  // It is the time to fetch a video image which matches the current
  // audio frame and to update the screen.
  
  // get buf sound system time (audio frame number)
  audioCurrentFrame = LE_SOUND_GetTimeOfBufSnd (aviDataPtr->hBufSnd);
  }
  else // for silent movie only
  {
    if (frameNumberPtr)
    {
      audioCurrentFrame = *frameNumberPtr;

      #if CE_ARTLIB_VideoFrameJumpModeEnabled
        for ( int i = 0; i < AVI_DATA_BUFFER_NUMBER; i++ )
        {
          if ( aviDataPtr->dataBuffer[i]->bFrameJumpBuffer == TRUE ) // a frame jump data buffer
          {
            aviDataPtr->dataBuffer[i]->bFrameJumpBuffer = FALSE;
            aviDataPtr->audioFrameNumber = aviDataPtr->dataBuffer[i]->startFrameNumber - 1;
          }
        }
      #endif
    }
    else
    {
      #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_FeedVideo (): "
          "This is a silent movie. So the sequencer needs to provide a frame "
          "number to play.");
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      #endif
      return FALSE;
    }
  }

  if ( audioCurrentFrame != aviDataPtr->audioFrameNumber )
  {
    BOOL    bPassEndFrame = FALSE;  // for audio pass end position
    BOOL    bFrameRendered = FALSE; // indicates that one frame has been rendered
    int     framesToSkip;           // number of frames to skip

    // go through all preloaded avi data buffers and look for the video
    // frame (image) which matches the current audio frame

    for ( int i = 0; i < AVI_DATA_BUFFER_NUMBER; i++ )
    {
      tempDataBuffer = aviDataPtr->dataBuffer[i];

      #if CE_ARTLIB_VideoFrameJumpModeEnabled && CE_ARTLIB_VideoMultiTaskingEnabled
        // Is it a voided data buffer ?
        if ( tempDataBuffer->bVoidBuffer == TRUE )
          continue; // skip the voided data buffer
      #endif

      // An empty data buffer ?
      if ( tempDataBuffer->bEmptyBuffer == TRUE )
      {
        #if CE_ARTLIB_VideoMultiTaskingEnabled
          // send a message to the avi data buffering thread to load
          // some avi data
          LE_QUEUE_EnqueueWithParameters (
            &aviDataPtr->messagesToDataBufferingThread,
            LI_VIDEO_MESSAGE_LoadBuffer, 0, 0,
            0, 0, 0, 0, 0, NULL, 0);
        #endif
        continue; // skip the empty data buffer
      }

      if ( audioCurrentFrame >= tempDataBuffer->endFrameNumber )
      {
        // Should we skip the whole buffer ?
        // Is it at the pass end position ?
        if ( audioCurrentFrame == tempDataBuffer->endFrameNumber )
          bPassEndFrame = TRUE; // set the audio pass end flag

        if ( tempDataBuffer->bAudioData == FALSE )
        {
          // Yes, we need to skip this buffer.
          // mark the data buffer not ready for use
          tempDataBuffer->bEmptyBuffer = TRUE;

          #if CE_ARTLIB_VideoMultiTaskingEnabled
            if ( aviDataPtr->bEndOfMoviDataChunk == FALSE ||
              aviDataPtr->bLoopMode == TRUE )
            {
              // send a message to the avi data buffering thread to load
              // some avi data
              LE_QUEUE_EnqueueWithParameters (
                &aviDataPtr->messagesToDataBufferingThread,
                LI_VIDEO_MESSAGE_LoadBuffer, 0, 0,
                0, 0, 0, 0, 0, NULL, 0);

              continue; // carry on searching for the requested video frame
            }
          #endif
        }

        // We have still got some audio data left to feed. So, keep
        // the data buffer until the audio data are totally fed.
        continue; // carry on searching for the requested video frame
      }
      else if ( (audioCurrentFrame >= tempDataBuffer->startFrameNumber &&
        audioCurrentFrame <= tempDataBuffer->endFrameNumber - 1) )
      {
        // Found the video frame (data buffer). Reset the audio pass end flag.
        if ( bPassEndFrame == TRUE )
          bPassEndFrame = FALSE;

        // Figure out how many frames we need to skip to get to the video frame
        // which matches the current audio frame.
        
        // Is the previous frame in this data buffer ?
        if ( aviDataPtr->audioFrameNumber >= tempDataBuffer->startFrameNumber &&
          aviDataPtr->audioFrameNumber < tempDataBuffer->endFrameNumber )
        {
          // The previous frame is found in this data buffer.
          if ( audioCurrentFrame > aviDataPtr->audioFrameNumber )
            framesToSkip = audioCurrentFrame - aviDataPtr->audioFrameNumber;
          else
            // This could happen when doing CutVideoToFrame jump action.
            framesToSkip = audioCurrentFrame - tempDataBuffer->startFrameNumber;
        }
        else  // The previous frame is not found in this data buffer.
        {
          framesToSkip = audioCurrentFrame - tempDataBuffer->startFrameNumber;
        }

        // skip to the video frame which matches the audioCurrentFrame
        if ( LI_VIDEO_SkipVideoFrames (
          tempDataBuffer, framesToSkip, aviDataPtr->videoStreamID) == FALSE )
        {
        #if CE_ARTLIB_EnableDebugMode
          sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_FeedVideo (): "
            "LI_VIDEO_SkipVideoFrames () fails to skip %d frames to get "
            "to the video frame which matches audioCurrentFrame.\n",
            framesToSkip);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer,
            LE_ERROR_DebugMsgToFileAndScreen);
        #endif
          return FALSE; // LI_VIDEO_SkipVideoFrames () fails
        }

        // lzou

        // Is Indeo video 5 codec ?
        if ( aviDataPtr->videoStreamHeaderPtr->fccHandler == mmioFOURCC ('I', 'V', '5', '0') ||
          aviDataPtr->videoStreamHeaderPtr->fccHandler == mmioFOURCC ('i', 'v', '5', '0') )
        {
          if ( aviDataPtr->destRGBBitCount == 32 && (offsetX == 0 && offsetY == 0) )
          {
            // By default, we want to position the video at
            // the centre of the screen.
            offsetX = (LE_GRAFIX_VirtualScreenWidthInPixels - aviDataPtr->outputWidth) / 2;
            offsetY = (LE_GRAFIX_VirtualScreenHeightInPixels - aviDataPtr->outputHeight) / 2;
          }
        }

        // render the video image to the screen
        LI_VIDEO_DrawToScreen (
          hVideoHandle,
          i /* buffer index */,
          aviDataPtr->audioFrameNumber,
          audioCurrentFrame,
          outputBitmapPtr,
          offsetX, offsetY);
        
        // The video frame has been rendered.
        bFrameRendered = TRUE;

        // update frameNumberPtr, the latest rendered video frame number
        if ( !aviDataPtr->bSilentMovie && frameNumberPtr != NULL )
          *frameNumberPtr = aviDataPtr->audioFrameNumber;

        // update videoFramesShown
        #if CE_ARTLIB_EnableDebugMode
          aviDataPtr->videoFramesShown ++;
        #endif

        // After having found and rendered the requested image, we should
        // carry on checking the rest of avi data buffers to see if some of
        // them are finsihed and get them marked. This is very important.
        continue; // carry on searching for finished avi data buffers
      }
    }  // end of for statement

    // Have we found the requested video frame ?
    if ( bFrameRendered == FALSE )
    {
      if ( bPassEndFrame == TRUE )
      {
        // No, the requested video frame is not found in the preloaded
        // avi data buffers. We are in trouble now.
        #if CE_ARTLIB_EnableDebugMode
          if ( audioCurrentFrame < (LONG) aviDataPtr->videoStreamHeaderPtr->dwLength )
          {
            sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_FeedVideo (): "
              "the requested video frame (%d) is not found (bPassEndFrame == TRUE), "
              "the previous frame is %d.\n",
              audioCurrentFrame, aviDataPtr->audioFrameNumber);
            LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
          }
        #endif
      }
    }

    // check for the end of video
    if ( audioCurrentFrame >= (LONG) aviDataPtr->videoStreamHeaderPtr->dwLength )
    {
      // We have finished the video clip.
      if ( aviDataPtr->bLoopMode == FALSE )  // nonlooping mode
      {
        // update frameNumberPtr, the latest rendered video frame number
        // Note that as some other applications (such as sequencer) waits
        //      for the very last video frame to finish, here we set
        //      *frameNumberPtr to the last frame of the movie to signal
        //      the end of movie.
        if ( frameNumberPtr != NULL )
          *frameNumberPtr = aviDataPtr->videoStreamHeaderPtr->dwLength;

        // reset flag bEndOfMoviDataChunk
        aviDataPtr->bEndOfMoviDataChunk = FALSE;

        // reset some variables to their default start values
        aviDataPtr->startVideoFrameForNextAudioFeed = 0;
        aviDataPtr->audioStartPosition = 0;
        
        // reset audioFrameNumber member to -1 (video start setting)
        aviDataPtr->audioFrameNumber = -1;
        
        // signal the end of film
        if ( bEndVideo != NULL )
          *bEndVideo = TRUE;
      }
      else  // loop mode on
      {
        if ( bEndVideo != NULL )
          *bEndVideo = FALSE;
      }
    }
    else
    {
      if ( bEndVideo != NULL )
        *bEndVideo = FALSE;
    }
  }

  // successful
  return TRUE;

} // LE_VIDEO_FeedVideo ()


/*****************************************************************************
 *
 *  BOOL LI_VIDEO_SkipVideoFrames (
 *    MyAVIDataBufferPointer aviDataBufferPtr,
 *    LONG framesToAdvance,
 *    WORD videoStreamID)
 *
 *  Advances (skips) framesToAdvance video frames, used for dropping frames.
 *
 *  Returns TRUE for success and FALSE for failure.
 *
 *****************************************************************************/

BOOL LI_VIDEO_SkipVideoFrames (
  MyAVIDataBufferPointer  aviDataBufferPtr,
  LONG  framesToAdvance,
  WORD  videoStreamID)
{
  MyAVIDataBufferPointer   tempDataBuffer;
  AVIINDEXENTRY           *indexPtr;
  LONG                     tempVideoFrames;

  if ( aviDataBufferPtr == NULL || framesToAdvance < 0 )
  {
#if CE_ARTLIB_EnableDebugMode
    if ( aviDataBufferPtr == NULL )
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_SkipVideoFrames (): "
        "invalid data buffer pointer: aviDataBufferPtr == NULL.\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    }
    
    if (framesToAdvance < 0)
    {
      sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_SkipVideoFrames (): "
        "framesToAdvance (%d) < 0.\n", framesToAdvance);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    }
#endif
    return FALSE;  // found invalid parameters
  }

  if ( framesToAdvance == 0 )
    return TRUE; // no frame to skip

  // initialise local variables
  tempVideoFrames = framesToAdvance;
  tempDataBuffer = aviDataBufferPtr;
  indexPtr = tempDataBuffer->videoFrameIndexPtr;

  // Move to the requested video frame.
  while ( tempVideoFrames > 0 )
  {
    // move to next index entry
    indexPtr ++;
    
    // A video frame (stream) ?
    if ( LOWORD (indexPtr->ckid) == videoStreamID ) // found a video frame
      tempVideoFrames --;
  }

  // get a pointer to the requested video frame data
  tempDataBuffer->videoFramePtr =
    tempDataBuffer->aviDataBuffer +
    indexPtr->dwChunkOffset -
    tempDataBuffer->startDataBufferIndexPtr->dwChunkOffset +
    8 /* ckid + cksize */;

  // Save the new frame index pointer
  tempDataBuffer->videoFrameIndexPtr = indexPtr;

  return TRUE;

} // LI_VIDEO_SkipVideoFrames ()


/*****************************************************************************
 *
 *  BOOL LI_VIDEO_DrawToScreen (
 *    LE_VIDEO_VideoHandle hVideoHandle,
 *    const int dataBufferIndex,
 *    LONG  previousFrame,
 *    LONG  currentFrame,
 *    BYTE *outputBitmapPtr)
 *    LONG  offsetX,
 *    LONG  offsetY)
 *
 *  Decompresses the requested video frame image into a user provided output
 *  buffer (outputBitmapPtr). If outputBitmapPtr is NULL, it then internally
 *  locks the screen for a pointer to the video memory and decompresses the
 *  requested video image directly to the video memory. The user can also
 *  provide the function with a pointer to the video memory, the effect would
 *  be the same.
 *
 *  One of the advantages of using video memory directly is that the function
 *  can draw all prerolled intermediate images to the screen, which may result
 *  in a much higher frame rate (say 3 to 4 time higher).
 *
 *  Returns TRUE for success and FALSE for failure.
 *
 *  NOTES for Indeo video codec version 5.10 and 5.11:
 *    Since Indeo video codec (version) 5.10 and 5.11, it is okay to play
 *    a video of its original image size in RGB32-bit mode. But the Indeo video
 *    codec double image size does not work in RGB32-bit mode.
 *    Thus, in RGB32-bit mode, we switch off the codec double video image size
 *    feature and draw the video at the centre of the window.
 *
 *****************************************************************************/

BOOL LI_VIDEO_DrawToScreen (
  LE_VIDEO_VideoHandle hVideoHandle,
  const int dataBufferIndex,
  LONG  previousFrame,
  LONG  currentFrame,
  BYTE *outputBitmapPtr,
  LONG  offsetX,
  LONG  offsetY)
{
  MyDataPointer        aviDataPtr;         // pointer to the main avi data structure
  MyAVIDataBufferPointer tempDataBuffer;   // pointer to one avi data buffer

  DWORD                videoImageDataSize;
  DWORD                decompressFlag;
  LONG                 audioFrameNumber;   // the frame just rendered
  int                  currentDataBufferIndex;

  LPBYTE               outputBufferPtr;    // video image output buffer pointer
  LPBITMAPINFOHEADER   bmiInputHeaderPtr;  // input bitmap format
  LPBITMAPINFOHEADER   bmiOutputHeaderPtr; // output bitmap format

  DWORD                ICErrorCode;        // for video image decompression

  HRESULT              errorCode;          // for direct draw
  DDSURFDESC           surfaceDescriptor;  // for direct draw

  // check for the validity of hVideoHandle
  if ( hVideoHandle == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_DrawToScreen (): "
      "hVideoHandle == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE;  // found invalid hVideoHandle
  }

  // get a pointer to the main avi data structure
  aviDataPtr = (MyDataPointer) hVideoHandle;

  // get a pointer to the requested avi data buffer
  tempDataBuffer = aviDataPtr->dataBuffer[dataBufferIndex];

  // get pointers to the input/output bitamp info headers
  bmiInputHeaderPtr = &aviDataPtr->videoStreamFormatPtr->bmiHeader;
  bmiOutputHeaderPtr = aviDataPtr->bmiOutputHeaderPtr;

  // get current avi data buffer index
  currentDataBufferIndex = dataBufferIndex;

  // initialise audioFrameNumber
  audioFrameNumber = currentFrame;

  if ( aviDataPtr->bEnableVideo )
  {
    if ( outputBitmapPtr != NULL ) // use user supplied output data buffer
    {
      outputBufferPtr = outputBitmapPtr;
      errorCode = DD_OK;

      // find out if the video image is vertically flipped
      if ( aviDataPtr->bFlipVertically == TRUE )
      {
        LPBITMAPINFOHEADER    bmiHeaderPtr;
      
        bmiHeaderPtr = aviDataPtr->bmiOutputHeaderPtr;
        if ( offsetY > abs (bmiHeaderPtr->biHeight) - aviDataPtr->outputHeight )
          offsetY = bmiHeaderPtr->biHeight - abs (aviDataPtr->outputHeight);
        else
          // recalculate the image offsetY to the upper left corner
          offsetY = abs (bmiHeaderPtr->biHeight) - offsetY - aviDataPtr->outputHeight;
      }
    }
    else // use the screen pointer
    {
      // Lock down the working surface.  From this point on, there is a good
      // chance that any error messages will lock up Windows, particularly
      // if the working area is the same as the screen bitmap.
      DWORD  dwDestWidth;
    
      memset (&surfaceDescriptor, 0, sizeof (surfaceDescriptor));
      surfaceDescriptor.dwSize = sizeof (surfaceDescriptor);
  
      errorCode = LE_GRAFIX_DDPrimarySurfaceN->Lock (
        NULL /* Ask for whole surface */,
        &surfaceDescriptor,
        DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
        NULL);
    
      if (errorCode == DD_OK)
      {
        outputBufferPtr = (BYTE *) surfaceDescriptor.lpSurface;
        dwDestWidth = surfaceDescriptor.lPitch /
          (surfaceDescriptor.ddpfPixelFormat.dwRGBBitCount / 8);

        // If we lock the work screen ourselves, we need to re-check the real screen
        // dimensions.
//    if ( surfaceDescriptor.dwWidth != (DWORD) bmiOutputHeaderPtr->biWidth ||
        if ( dwDestWidth != (DWORD) bmiOutputHeaderPtr->biWidth ||
          surfaceDescriptor.dwHeight != (DWORD) bmiOutputHeaderPtr->biHeight )
        {
          aviDataPtr->bmiOutputHeaderPtr->biWidth = (LONG) dwDestWidth;
          aviDataPtr->bmiOutputHeaderPtr->biHeight =
            ((aviDataPtr->bmiOutputHeaderPtr->biHeight > 0) ? (LONG) surfaceDescriptor.dwHeight :
            - (LONG) surfaceDescriptor.dwHeight);
        }

        // find out if the video image is vertically flipped
        if ( aviDataPtr->bFlipVertically == TRUE )
        {
          if ( offsetY > (LONG) surfaceDescriptor.dwHeight - aviDataPtr->outputHeight )
            offsetY = (LONG) surfaceDescriptor.dwHeight - aviDataPtr->outputHeight;
          else
            // recalculate the image offsetY to the upper left corner
            offsetY = (LONG) surfaceDescriptor.dwHeight - offsetY - aviDataPtr->outputHeight;
        }
      }
      else
      {
        #if CE_ARTLIB_EnableDebugMode
          char  str[256];
        
          memset (str, 0, 256);
          strcpy (str,"LI_VIDEO_DrawToScreen (): unable to lock the display surface.\n");
          OutputDebugString (str);
        #endif
      }
    }
  }
  else
    errorCode = DD_FALSE;

  if (errorCode == DD_OK)
  {
    // get the video frame (chunk) data size
    videoImageDataSize = tempDataBuffer->videoFrameIndexPtr->dwChunkLength;

    if ( videoImageDataSize > 0 )
    {
      // If current frame is not a key frame, then, we need to check to see
      // whether we need to preroll some of the preceding video images, or
      // jump to the next key frame of the same buffer, or just simply jump
      // to the first frame of the next avi data buffer.
      if ( !(tempDataBuffer->videoFrameIndexPtr->dwFlags & AVIIF_KEYFRAME) )
      {
        // Do we need to preroll some of the preceding video images ?
        if ( currentFrame != previousFrame + 1 ) // Yes, we do need to preroll.
        {
          BOOL              bPreroll;        // need to do prerolling ?
          BOOL              bPassEndFlag;    // currentFrame the end video frame ?
          BOOL              bNextKeyFrame;   // next frame is a key frame ?
          WORD              videoStreamID;
          BYTE             *buffer;
          AVIINDEXENTRY    *indexPtr;
          BYTE             *frameBuffer;     // point to the nearest key frame or previousFrame
          AVIINDEXENTRY    *frameIndexPtr;   // point to the nearest key frame or previousFrame
          short             framesToPreroll; // number of images needs to be prerolled

          // set frameBuffer and frameIndexPtr to
          // the first video frame of current data buffer
          // (a video key frame by default)
          frameBuffer = tempDataBuffer->startVideoFramePtr;
          frameIndexPtr = tempDataBuffer->startVideoFrameIndexPtr;

          // set buffer and indexPtr to the requested video frame
          buffer = tempDataBuffer->videoFramePtr;
          indexPtr = tempDataBuffer->videoFrameIndexPtr;

          // get the video stream ID
          videoStreamID = aviDataPtr->videoStreamID;

          #if CE_ARTLIB_EnableDebugMode && 0
            // a test
            if ( !(frameIndexPtr->dwFlags & AVIIF_KEYFRAME) )
            {
              FourCCToString.fourcc = frameIndexPtr->ckid;
              sprintf (LE_ERROR_DebugMessageBuffer, "*** "
                "The first frame -- ckid(%s)/dwFlag(0x%08x), "
                "frames : p(%d)/c(%d)/s(%d)/e(%d).\n",
                FourCCToString.string,
                frameIndexPtr->dwFlags,
                previousFrame,
                currentFrame,
                tempDataBuffer->startFrameNumber,
                tempDataBuffer->endFrameNumber);
              LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
            }
          #endif

          // initialise local variables
          bPreroll = TRUE;
          framesToPreroll = 0;

          // get to the nearest previous video stream
          while ( -- indexPtr >= tempDataBuffer->startVideoFrameIndexPtr )
          {
            if ( LOWORD (indexPtr->ckid) == videoStreamID ) // a video stream
              break; // found a video frame
          }

          // Go backward searching for a video key frame or a previous
          // decompressed video frame, whichever is closer, and determine
          // that how many video frames need to be prerolled before we can
          // decompress the requested video frame properly.

          while ( indexPtr >= tempDataBuffer->startVideoFrameIndexPtr )
          {
            // count frames to preroll
            framesToPreroll ++;

            // Is this frame a key frame ?
            if ( indexPtr->dwFlags & AVIIF_KEYFRAME ) // found a video key frame
            {
              // save the indexPtr
              frameIndexPtr = indexPtr;
              break; // Have found a video key frame.
            }
            // Have we reached the previous decompressed video frame ?
            // (actually, previousFrame + 1 needed)
            else if ( currentFrame - framesToPreroll == previousFrame + 1 )
            {
              // Yes, we have reached the previous decompressed video frame.
              // save the indexPtr
              frameIndexPtr = indexPtr;
              break; // Have reached the previous decompressed video frame.
            }

            // go backward looking for a video stream
            while ( -- indexPtr >= tempDataBuffer->startVideoFrameIndexPtr )
            {
              if ( LOWORD (indexPtr->ckid) == videoStreamID ) // a video stream
                break; // found a video frame
            }
          }  // end of for statement for preroll

          // get the video frame data from which we are going to preroll
          frameBuffer = tempDataBuffer->aviDataBuffer +
            frameIndexPtr->dwChunkOffset -
            tempDataBuffer->startDataBufferIndexPtr->dwChunkOffset +
            8 /* ckid + cksize */;

          // We also need to know if the next video frame is a key frame.
          // Thus, we can decide whether we should do the framesToPreroll
          // frames of prerolling, or just simply jump to the next video
          // key frame.

          // set buffer and indexPtr to the requested video frame
          buffer = tempDataBuffer->videoFramePtr;        // point to the current frame
          indexPtr = tempDataBuffer->videoFrameIndexPtr; // point to the current frame

          // set the pass end flag to FALSE
          bPassEndFlag = FALSE;

          if ( currentFrame == tempDataBuffer->endFrameNumber - 1 )
          {
            // currentFrame is the end video frame of current data buffer
            bPassEndFlag = TRUE;  // no more next video frame
            bNextKeyFrame = TRUE; // Next video frame is the first video frame of
                                  // next data buffer (a key frame by default).
          }
          else // We do have next video frame in the data buffer.
          {
            // get to the next video frame
            while (TRUE)
            {
              // increment the indexPtr by 1
              indexPtr ++;
              if ( LOWORD (indexPtr->ckid) == videoStreamID ) // a video stream
                break; // found a video frame
            }

            // A video key frame ?
            if ( indexPtr->dwFlags & AVIIF_KEYFRAME )
            {
              bNextKeyFrame = TRUE;  // Next video frame is a key frame.
              
              // get the video key frame data
              buffer = tempDataBuffer->aviDataBuffer + indexPtr->dwChunkOffset -
                tempDataBuffer->startDataBufferIndexPtr->dwChunkOffset +
                8 /* ckid + cksize */;
            }
            else
              bNextKeyFrame = FALSE; // Next video frame is not a key frame.
          }

          // Should we do video image prerolling or just simply jump to the
          // next key frame ?
          if ( bNextKeyFrame == TRUE &&
            framesToPreroll > PREROLL_LIMIT_IF_NEXT_KEY )
          {
            // We'd better jump to the next key frame. So,
            bPreroll = FALSE;

            // test for pass end frame position
            if ( bPassEndFlag == TRUE ) // Next video frame is in the next avi data buffer.
            {
              MyAVIDataBufferPointer   dataBuffer; // pointer to a temporary avi data buffer
              LONG                     passEndFrame;

              // We should jump to the first video frame of next
              // avi data buffer, if we can find it.

              // Get the pass end frame number.
              passEndFrame = tempDataBuffer->endFrameNumber;

              // See if we can find the next avi data buffer.
              // If we can not find it, we should still do video image prerolling.
              bPreroll = TRUE; // test to see if we can find the next data buffer

              // look for the next avi data buffer
              for ( int i = 0 ; i < AVI_DATA_BUFFER_NUMBER; i ++ )
              {
                dataBuffer = aviDataPtr->dataBuffer[i];

                #if CE_ARTLIB_VideoFrameJumpModeEnabled && CE_ARTLIB_VideoMultiTaskingEnabled
                  // Is it a voided data buffer ?
                  if ( dataBuffer->bVoidBuffer == TRUE )
                    continue; // skip the voided data buffer
                #endif

                if ( dataBuffer->bEmptyBuffer == FALSE &&
                  passEndFrame == dataBuffer->startFrameNumber )
                {
                  // We have found the next avi data buffer.

                  // check the audio data status for current data buffer
                  if ( tempDataBuffer->bAudioData == FALSE )
                  {
                    // There is no unfed audio data left.
                    // So, mark the current data buffer empty.
                    tempDataBuffer->bEmptyBuffer = TRUE;
                  }

                  #if CE_ARTLIB_EnableDebugMode && 0
                    FourCCToString.fourcc = tempDataBuffer->videoFrameIndexPtr->ckid;
                    sprintf (LE_ERROR_DebugMessageBuffer, "*** "
                      "current frame -- ckid(%s)/dwFlag(0x%08x), "
                      "frames : p(%d)/c(%d)/s(%d)/e(%d), audioFrame (%d).\n",
                      FourCCToString.string,
                      tempDataBuffer->videoFrameIndexPtr->dwFlags,
                      previousFrame,
                      currentFrame,
                      tempDataBuffer->startFrameNumber,
                      tempDataBuffer->endFrameNumber,
                      audioFrameNumber);
                    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
                  #endif

                  // Note that the tempDataBuffer will be released
                  // from being used in FeedVideo () rather than here.

                  // Save the newly found data buffer.
                  tempDataBuffer = dataBuffer;
                  currentDataBufferIndex = i;
                  bPreroll = FALSE; // We are not going to do any prerolling.

                  // save the new video image frame number
                  audioFrameNumber = tempDataBuffer->startFrameNumber;

                  #if CE_ARTLIB_EnableDebugMode && 0
                    MessageBeep (-1); // found the next avi data buffer
                    LE_ERROR_DebugMsg ("We jump to the first key frame of the "
                      "next data buffer.\n", LE_ERROR_DebugMsgToFile);

                    FourCCToString.fourcc = tempDataBuffer->videoFrameIndexPtr->ckid;
                    sprintf (LE_ERROR_DebugMessageBuffer, "*** "
                      "The next key frame -- ckid(%s)/dwFlag(0x%08x), "
                      "frames : p(%d)/c(%d)/s(%d)/e(%d), audioFrame (%d).\n",
                      FourCCToString.string,
                      tempDataBuffer->videoFrameIndexPtr->dwFlags,
                      previousFrame,
                      currentFrame,
                      tempDataBuffer->startFrameNumber,
                      tempDataBuffer->endFrameNumber,
                      audioFrameNumber);
                    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
                  #endif
                  break; // found the next avi data buffer
                }
              }  // end of for statement
            }
            // We are simply jumping to the next key frame of the same data buffer
            // (the next video frame), instead of doing video image prerolling.
            else
            {
              // reset the current videoFramePtr and videoFrameIndexPtr
              tempDataBuffer->videoFramePtr = buffer;
              tempDataBuffer->videoFrameIndexPtr = indexPtr;

              // save the new video image frame number
              audioFrameNumber = currentFrame + 1; // next frame to the current frame

              #if CE_ARTLIB_EnableDebugMode && 0
                LE_ERROR_DebugMsg ("We jump to the next key frame of the same "
                  "data buffer.\n", LE_ERROR_DebugMsgToFile);

                FourCCToString.fourcc = tempDataBuffer->videoFrameIndexPtr->ckid;
                sprintf (LE_ERROR_DebugMessageBuffer, "*** "
                  "The next key frame -- ckid(%s)/dwFlag(0x%08x), "
                  "frames : p(%d)/c(%d)/s(%d)/e(%d), audioFrame (%d).\n",
                  FourCCToString.string,
                  tempDataBuffer->videoFrameIndexPtr->dwFlags,
                  previousFrame,
                  currentFrame,
                  tempDataBuffer->startFrameNumber,
                  tempDataBuffer->endFrameNumber,
                  audioFrameNumber);
                LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
              #endif
            } // jump to next key of the same data buffer
          } // end of jumping to the next key frame

          if ( bPreroll == TRUE ) // do video image prerolling
          {
            short prerollCount = 0;
    
            // We should start video image preroll here.
            // We are now prerolling from the nearest key frame or
            // the previousFrame decompressed video frame up to the
            // frame which is one frame earlier than the current video
            // frame.
            while ( frameIndexPtr < tempDataBuffer->videoFrameIndexPtr )
            {
              prerollCount ++;

              #if CE_ARTLIB_EnableDebugMode && 0
                FourCCToString.fourcc = frameIndexPtr->ckid;
                sprintf (LE_ERROR_DebugMessageBuffer, "-- "
                  "Prerolled %d : "
                  "ckid(%s)/dwFlag(0x%08x), "
                  "frames : p(%d)/c(%d)/s(%d)/e(%d).\n",
                  prerollCount,
                  FourCCToString.string,
                  frameIndexPtr->dwFlags,
                  previousFrame,
                  currentFrame,
                  tempDataBuffer->startFrameNumber,
                  tempDataBuffer->endFrameNumber);
                LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
              #endif

              // set the flag to preroll a video image
              if ( frameIndexPtr->dwFlags & AVIIF_KEYFRAME )
                decompressFlag = ICDECOMPRESS_HURRYUP;
              else
                decompressFlag = ICDECOMPRESS_HURRYUP | ICDECOMPRESS_NOTKEYFRAME;

              ICErrorCode = ICDecompressEx (
                aviDataPtr->hIC,                  // handle to the video image decompressor
                decompressFlag,                   // decompress flag
                bmiInputHeaderPtr,                // LPBITMAPINFOHEADER lpbiSrc,
                frameBuffer,                      // LPVOID lpSrc,
                0,                                // int xSrc,
                0,                                // int ySrc,
                bmiInputHeaderPtr->biWidth,       // int dxSrc,
                bmiInputHeaderPtr->biHeight,      // int dySrc,
                bmiOutputHeaderPtr,               // LPBITMAPINFOHEADER lpbiDst,
                (LPVOID) outputBufferPtr,         // LPVOID lpDst,
                offsetX,                          // int xDst,
                offsetY,                          // int yDst,
                aviDataPtr->outputWidth,          // int dxDst,
                aviDataPtr->outputHeight);        // int dyDst

              if ( ICErrorCode != ICERR_OK )
              {
              #if CE_ARTLIB_EnableDebugMode
                sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_DrawToScreen (): "
                  "ICDecompressEx () (2) fails to preroll a video image.\n");
                LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
              #endif
              }
        
              // get to the index position of the next video frame
              while (TRUE)
              {
                frameIndexPtr ++;
                if ( LOWORD (frameIndexPtr->ckid) == videoStreamID )
                  break; // found an index entry for the next video frame
              }

              // move frameBuffer to the next video frame
              frameBuffer = tempDataBuffer->aviDataBuffer +
                frameIndexPtr->dwChunkOffset -
                tempDataBuffer->startDataBufferIndexPtr->dwChunkOffset +
                8 /* ckid + cksize */;
            } // end of while for prerolling
          } // end of video image prerolling
        }
      } // end of not a key frame

      // decompress/render the requested video image

      // set the decompression flag for the video frame
      if ( tempDataBuffer->videoFrameIndexPtr->dwFlags & AVIIF_KEYFRAME )
        decompressFlag = 0;
      else
        decompressFlag = ICDECOMPRESS_NOTKEYFRAME;

      // We are now able to decompress the current video frame correctly.
      ICErrorCode = ICDecompressEx (
        aviDataPtr->hIC,                  // handle to the video image decompressor
        decompressFlag,                   // decompress flag
        bmiInputHeaderPtr,                // LPBITMAPINFOHEADER lpbiSrc,
        tempDataBuffer->videoFramePtr,    // LPVOID lpSrc,
        0,                                // int xSrc,
        0,                                // int ySrc,
        bmiInputHeaderPtr->biWidth,       // int dxSrc,
        bmiInputHeaderPtr->biHeight,      // int dySrc,
        bmiOutputHeaderPtr,               // LPBITMAPINFOHEADER lpbiDst,
        (LPVOID) outputBufferPtr,         // LPVOID lpDst,
        offsetX,                          // int xDst,
        offsetY,                          // int yDst,
        aviDataPtr->outputWidth,          // int dxDst,
        aviDataPtr->outputHeight);        // int dyDst

      if ( ICErrorCode != ICERR_OK )
      {
      #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LI_VIDEO_DrawToScreen (): "
          "ICDecompressEx () (1) fails.\n");
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
      #endif
        goto ErrorExit;  // ICDecompressEx () fails
      }
    }
    else // a zero size video image. We will try to handle this case later on.
      decompressFlag = ICDECOMPRESS_NULLFRAME;

    // unlock the surface
    LE_GRAFIX_DDPrimarySurfaceN->Unlock(NULL/*outputBufferPtr*/);
  }

  // update the player's audioFrameNumber
  aviDataPtr->audioFrameNumber = audioFrameNumber;
  
  #if CE_ARTLIB_VideoViewDataBufferState && CE_ARTLIB_EnableDebugMode
    // Draw at the bottom of the screen all preloading avi data buffer status.
    LI_VIDEO_VisualInspectDataBufferStatus (
      hVideoHandle,
      currentDataBufferIndex /* buffer index */);
  #endif

  return TRUE;

ErrorExit:

  // unlock the surface before leaving
  LE_GRAFIX_DDPrimarySurfaceN->Unlock(NULL/*outputBufferPtr*/);

  // update the player's audioFrameNumber
  aviDataPtr->audioFrameNumber = audioFrameNumber;

  return FALSE;

} // LI_VIDEO_DrawToScreen ()


/******************************************************************************
 *
 *  BOOL LI_VIDEO_VisualInspectDataBufferStatus (
 *    LE_VIDEO_VideoHandle hVideoHandle,
 *    int                  bufferIndex)
 *
 *  Draws at the bottom of the screen all preloading avi data buffer status.
 *  It shows how many frames preloaded in each avi data buffer and which buffer
 *  is the current data buffer and which buffer(s) are already finished (empty).
 *
 *  This function is only for debug purpose.
 *
 ******************************************************************************/

BOOL LI_VIDEO_VisualInspectDataBufferStatus (
  LE_VIDEO_VideoHandle hVideoHandle,
  int                  bufferIndex)
{
  MyDataPointer              aviDataPtr;     // pointer to the main avi data struture
  MyAVIDataBufferPointer     tempDataBuffer; // pointer to one preloading avi data buffer
  COLORREF                   debugBoxColour; // colour used to show data buffer status
  RECT                       boxRect;        // a box rect for one avi data buffer
  COLORREF                   bgColour;       // background colour
  RECT                       bgRect;         // background rect
  int                        boxSpacing;     // space between two neighbouring boxes
  int                        frameHeight;    // frame height for boxRect
  int                        boxWidth;       // box width
  int                        barHeight;      // total height for boxRect (one buffer)

  HRESULT                    errorCode;
  LPBYTE                     screenBytesPtr;
  DDSURFDESC                 surfaceDescriptor;
  DWORD                      dwMyDestWidth;

  if ( hVideoHandle == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "VisualInspectDataBufferStatus (): "
      "hVideoHandle == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
  #endif
    return FALSE;  // found invalid hVideoHandle
  }

  // get a pointer to our main avi data structure
  aviDataPtr = (MyDataPointer) hVideoHandle;

  // Each box is 20 pixels wide. Every 5 pixels for each frame in the data buffer.
  boxWidth = 20;

  if ( LE_GRAFIX_VirtualScreenWidthInPixels < 2 * boxWidth * AVI_DATA_BUFFER_NUMBER )
    boxWidth /= 2;

  boxSpacing = LE_GRAFIX_VirtualScreenWidthInPixels  - boxWidth * AVI_DATA_BUFFER_NUMBER;
  boxSpacing /= AVI_DATA_BUFFER_NUMBER + 1;

  barHeight = LE_GRAFIX_VirtualScreenHeightInPixels * 5 / 6;
  frameHeight = 2;  // 2 pixels for each frame in an avi data buffer

  // make the background colour black
  bgColour = RGB (0, 0, 0);
  
  // Lock down the working surface.  From this point on, there is a good
  // chance that any error messages will lock up Windows, particularly
  // if the working area is the same as the screen bitmap.

  memset (&surfaceDescriptor, 0, sizeof (surfaceDescriptor));
  surfaceDescriptor.dwSize = sizeof (surfaceDescriptor);
  
  errorCode = LE_GRAFIX_DDPrimarySurfaceN->Lock (
    NULL /* Ask for whole surface */,
    &surfaceDescriptor,
    DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
    NULL);

  if (errorCode == DD_OK)
  {
    screenBytesPtr = (BYTE *) surfaceDescriptor.lpSurface;
    dwMyDestWidth = surfaceDescriptor.lPitch /
        (surfaceDescriptor.ddpfPixelFormat.dwRGBBitCount / 8);

    // draw one coloured box for each avi data buffer
    boxRect.bottom = LE_GRAFIX_VirtualScreenHeightInPixels - 1; // 1 pixels from the bottom of screen

    for ( int i = 0; i < AVI_DATA_BUFFER_NUMBER; i ++ )
    {
      // get a pointer to an avi data buffer
      tempDataBuffer = aviDataPtr->dataBuffer[i];

      // define a box for the avi data buffer
      if ( i == 0 )
        boxRect.left = boxSpacing;
      else
        boxRect.left = boxRect.right + boxSpacing;
      boxRect.top = boxRect.bottom - frameHeight *
        (tempDataBuffer->endFrameNumber - tempDataBuffer->startFrameNumber);
      boxRect.right = boxRect.left + boxWidth;

      // get background rect
      bgRect.left = boxRect.left;
      bgRect.top = barHeight;
      bgRect.right = boxRect.right;
      bgRect.bottom = boxRect.top;

      // get a colour for the preloading avi data buffer status
      // red   -- the data buffer has valid data in it
      // green -- current avi data buffer
      // blue  -- newly preloaded data buffer with unfed audio data
      if ( i == bufferIndex )
        debugBoxColour = RGB (200, 255, 0);   // current data buffer
      else if ( tempDataBuffer->bEmptyBuffer == FALSE &&
        tempDataBuffer->bAudioData == FALSE )
      {
        debugBoxColour = RGB (255, 0, 0);  // red -- the buffer has valid data
      }
      else if ( tempDataBuffer->bEmptyBuffer == FALSE &&
        tempDataBuffer->bAudioData == TRUE )
      {
        debugBoxColour = RGB (0, 0, 255);  // blue -- audio data are not fed yet
      }
      else if ( tempDataBuffer->bEmptyBuffer == TRUE )
        debugBoxColour = RGB (0, 0, 0);    // black -- the data buffer is empty
      else
        debugBoxColour = RGB (255, 255, 255);  // in a wrong status

      if ( LE_GRAFIX_ScreenBitsPerPixel == 16 )
      {
        LI_BLT_Fill16Raw (bgColour, FALSE, screenBytesPtr,
          (UNS16) dwMyDestWidth,
          (UNS16) surfaceDescriptor.dwHeight, &bgRect);
        LI_BLT_Fill16Raw (debugBoxColour, FALSE, screenBytesPtr,
          (UNS16) dwMyDestWidth,
          (UNS16) surfaceDescriptor.dwHeight, &boxRect);
      }
      else if ( LE_GRAFIX_ScreenBitsPerPixel == 24 )
      {
        LI_BLT_Fill24Raw (bgColour, FALSE, screenBytesPtr,
          (UNS16) dwMyDestWidth,
          (UNS16) surfaceDescriptor.dwHeight, &bgRect);
        LI_BLT_Fill24Raw (debugBoxColour, FALSE, screenBytesPtr,
          (UNS16) dwMyDestWidth,
          (UNS16) surfaceDescriptor.dwHeight, &boxRect);
      }
      else if ( LE_GRAFIX_ScreenBitsPerPixel == 32 )
      {
        LI_BLT_Fill32Raw (bgColour, FALSE, screenBytesPtr,
          (UNS16) dwMyDestWidth,
          (UNS16) surfaceDescriptor.dwHeight, &bgRect);
        LI_BLT_Fill32Raw (debugBoxColour, FALSE, screenBytesPtr,
          (UNS16) dwMyDestWidth,
          (UNS16) surfaceDescriptor.dwHeight, &boxRect);
      }
    } // end of for statement

    // unlock the surface
    LE_GRAFIX_DDPrimarySurfaceN->Unlock(NULL/*screenBytesPtr*/);
  }
  else
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "VisualInspectDataBufferStatus (): "
      "Unable to use the screen.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
  #endif
    return FALSE;
  }

  return TRUE;
} // LI_VIDEO_VisualInspectDataBufferStatus ()


/*****************************************************************************
 *
 *  BOOL LE_VIDEO_StopVideo (LE_VIDEO_VideoHandle VideoHandle)
 *
 *  Stops the video and deallocates all allocated resources associated with
 *  the main avi data structure.
 *
 *  Returns TRUE for success and FALSE for failure.
 *
 *****************************************************************************/

BOOL LE_VIDEO_StopVideo (LE_VIDEO_VideoHandle hVideoHandle)
{
  MyDataPointer             aviDataPtr;  // pointer to the main avi data structure
  #if CE_ARTLIB_VideoMultiTaskingEnabled
    LE_QUEUE_MessageRecord  myMessage;
  #endif

  // check for the validity of the video handle
  if ( hVideoHandle == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_StopVideo (): "
      "hVideoHandle == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE;  // found an invalid video handle
  }

  // get a pointer to the main avi data structure
  aviDataPtr = (MyDataPointer) hVideoHandle;

  // We have finished playing back the video clip.

#if CE_ARTLIB_VideoMultiTaskingEnabled
  // send a message to the avi data buffering thread to
  // shut down the second video thread
  // To make sure that this shutdown message won't get lost
  // due to queue full, flush the message queue before sending
  // out this message.
  LE_QUEUE_Flush (&aviDataPtr->messagesToDataBufferingThread);

  LE_QUEUE_EnqueueWithParameters (
    &aviDataPtr->messagesToDataBufferingThread,
    LI_VIDEO_MESSAGE_ShutDown, 0, 0,
    0, 0, 0, 0, 0, NULL, 0);

  // wait until the avi data buffering thread is properly shut down
  while (TRUE)
  {
    if ( !LE_QUEUE_WaitForMessage (
      &aviDataPtr->messagesToVideoControlThread,
      &myMessage) )
      break; // Something went wrong.
    
    LE_QUEUE_FreeStorageAssociatedWithMessage (&myMessage);
    
    if ( myMessage.messageCode == LI_VIDEO_MESSAGE_ShutDownDone )
      break; // OK, the avi data buffering thread is properly shut down.
  }
#endif
 
  // deallocate all allocated resources associated with
  // the main avi data structure -- MyAVIDataRecord
  ClearUpAVIDataRecord (aviDataPtr);

  // Now, it is ok to close the avi video file
  CloseMyAVIFile (&aviDataPtr->aviSource);

  // Free the memory allocated for the main data structure.
  LE_MEMORY_free (aviDataPtr);
  aviDataPtr = NULL;

  return TRUE;  // successful

} // LE_VIDEO_StopVideo ()


/*****************************************************************************
 *
 *  BOOL SearchForChunkFromCurrentFilePosition (
 *    MyAVIFilePointer myAVIFilePtr,
 *    LPMMCKINFO chunkToSearchPtr)
 *
 *  Searches for the requested chunk from the current file position in data
 *  file. Before doing any chunk search, the user should make it sure that
 *  the current file position is at a place ready for descend. That is, the
 *  file position should be at a place for mmioDescend to function properly.
 *
 *  If successful, chunkToSearchPtr is filled with the found chunk information,
 *  and the file position is at the beginning of data portion of the found chunk.
 *  If the found chunk is a 'RIFF' or 'LIST' chunk, then, the file position
 *  is just after the chunk form/list type (fccType).
 *
 *  Return:  TRUE for success and FALSE for failure.
 *
 *  Notes: If the requested chunk is not found and EOF is reached,
 *    it returns TRUE with *chunkToSearchPtr filled with zeros.
 *
 *****************************************************************************/

BOOL SearchForChunkFromCurrentFilePosition (
  MyAVIFilePointer myAVIFilePtr,
  LPMMCKINFO chunkToSearchPtr)
{
  MMRESULT         mmr;
  short            nestLevel;
  LPMMCKINFO       tempChunkInfoPtr;
  LPMMCKINFO       parentChunkInfoPtr;
  DWORD            chunkID;

  // validate the data file pointer
  if ( myAVIFilePtr == NULL || myAVIFilePtr->file == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer,
      "SearchForChunkFromCurrentFilePosition (): "
      "invalid data file pointer.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE;  // garbage data file pointer
  }

  if ( chunkToSearchPtr == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer,
      "SearchForChunkFromCurrentFilePosition (): chunkToSearchPtr == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE;  // no specified chunk to search for
  }

  // start to search for the requested chunk from the current file position

  nestLevel = myAVIFilePtr->nestLevel; // get current chunk nesting level

  while (1)
  {
    // descend into next available chunk/subchunk
    tempChunkInfoPtr = &myAVIFilePtr->chunkInfo[nestLevel+1]; // point to a subchunk
    memset (tempChunkInfoPtr, 0, sizeof (MMCKINFO));

    if ( nestLevel == 0 )  // start to descend into the first chunk RIFF/size/AVI
    {
      mmr = mmioDescend (myAVIFilePtr->file, tempChunkInfoPtr, NULL, 0);
    }
    else if ( nestLevel > 0 && nestLevel < MAX_CHUNK_NEST_LEVEL )
    {
      parentChunkInfoPtr = &myAVIFilePtr->chunkInfo[nestLevel];
      mmr = mmioDescend (myAVIFilePtr->file, tempChunkInfoPtr, parentChunkInfoPtr, 0);
    }
    else
    {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer,
        "SearchForChunkFromCurrentFilePosition (): invalid nestLevel "
        "(%d) in %s.\n", nestLevel, myAVIFilePtr->name);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
      return FALSE;  // invaid nest level
    }

    if ( mmr != MMSYSERR_NOERROR )
    {
      // End of chunk or EOF ?
      if ( nestLevel == 1 )  // EOF reached
      {
        // The requested chunk is not found. Set *chunkToSearchPtr to zeros.
        memset (chunkToSearchPtr, 0, sizeof (MMCKINFO));
        return TRUE;  // requested chunk not found
      }

      // end of chunk reached

      // ascend out of the chunk
      tempChunkInfoPtr = &myAVIFilePtr->chunkInfo[nestLevel];
      mmr = mmioAscend (myAVIFilePtr->file, tempChunkInfoPtr, 0);

      if ( mmr != MMSYSERR_NOERROR )
      {
      #if CE_ARTLIB_EnableDebugMode
        // make a string for the chunk ID
        memset (&FourCCToString, 0, sizeof (FourCCToString));
        FourCCToString.fourcc = tempChunkInfoPtr->ckid;

        sprintf (LE_ERROR_DebugMessageBuffer,
          "SearchForChunkFromCurrentFilePosition (): ascend fails "
          "in %s. Unable to ascend out of ckid(%s)/nestLevel(%d)\n",
          myAVIFilePtr->name, FourCCToString.string, nestLevel);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
      #endif
        return FALSE;  // ascend fails
      }

      // update current chunk nesting level
      myAVIFilePtr->nestLevel = -- nestLevel;
      continue;
    }

    // descend succeeds, update current chunk nesting level
    myAVIFilePtr->nestLevel = ++ nestLevel;

    // get the chunk id
    chunkID = tempChunkInfoPtr->ckid;

    // check with chunkToSearchPtr->ckid
    if ( chunkID == chunkToSearchPtr->ckid )
    {
      // 'RIFF' or 'LIST' chunk ?
      if ( chunkID == mmioFOURCC ('R', 'I', 'F', 'F') ||
        chunkID == mmioFOURCC ('L', 'I', 'S', 'T') )
      {
        // check for fccType
        if ( tempChunkInfoPtr->fccType == chunkToSearchPtr->fccType )
        {
          *chunkToSearchPtr = *tempChunkInfoPtr;
          break;  // have found the requested chunk
        }
      }
      else // just a data chunk
      {
        *chunkToSearchPtr = *tempChunkInfoPtr;
        break;  // have found the requested chunk
      }
    }

    // If neither a RIFF chunk nor a LIST chunk, skip the chunk data part.
    if ( chunkID != mmioFOURCC ('R', 'I', 'F', 'F') &&
      chunkID != mmioFOURCC ('L', 'I', 'S', 'T') )
    {
      // ascend out of the chunk
      mmr = mmioAscend (myAVIFilePtr->file, tempChunkInfoPtr, 0);

      if ( mmr != MMSYSERR_NOERROR )
      {
      #if CE_ARTLIB_EnableDebugMode
        // make a string for the chunk ID
        memset (&FourCCToString, 0, sizeof (FourCCToString));
        FourCCToString.fourcc = tempChunkInfoPtr->ckid;

        sprintf (LE_ERROR_DebugMessageBuffer,
          "SearchForChunkFromCurrentFilePosition (): ascend fails "
          "in %s. Unable to ascend out of ckid(%s)/nestLevel(%d)\n",
          myAVIFilePtr->name, FourCCToString.string, nestLevel);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
      #endif
        return FALSE;  // ascend fails
      }

      // update current chunk nesting level
      myAVIFilePtr->nestLevel = -- nestLevel;
    }
  }  // end of while (1) statement

  // successful
  return TRUE;

}  // end of SearchForChunkFromCurrentFilePosition ()


/****************************************************************************
 *
 *  BOOL ReadChunkFixedDataPart (HMMIO hMyAVIFile, DWORD chunkDataSize,
 *    BYTE **ppChunkDataBuffer, DWORD suggestedBufferSize)
 *
 *  It allocates a data buffer of size chunkDataSize or suggestedBufferSize
 *  whichever is bigger. The suggestedBufferSize is only for data format
 *  backward compatibility. Normally, suggestedBufferSize is greater than or
 *  equal to chunkDataSize. If suggestedBufferSize is 0, it is ignored.
 *  The chunkDataSize will be the data buffer size.
 *
 *  After having allocated a data buffer, the function reads chunkDataSize
 *  bytes of data from hMyAVIFile into the data buffer, pointed to by
 *  *ppChunkDataBuffer. After having finished with the data, the buffer
 *  should be deallocated manually.
 *
 *  Return:
 *    TRUE for success and FALSE for failure.
 *
 ****************************************************************************/

BOOL ReadChunkFixedDataPart (
  HMMIO hMyAVIFile,
  DWORD chunkDataSize,
  BYTE **ppChunkDataBuffer,
  DWORD suggestedBufferSize)
{
  BYTE     *buffer  = NULL;

  // check for validaty

  if ( chunkDataSize <= 0 )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "ReadChunkFixedDataPart (): "
      "chunkDataSize == %d.\n", chunkDataSize);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE; // invalid chunkDataSize
  }

  if ( ppChunkDataBuffer == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "ReadChunkFixedDataPart (): "
      "ppChunkDataBuffer == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE; // garbage data buffer pointer
  }

  // find an appropriate size of memory to allocate
  if ( suggestedBufferSize < chunkDataSize )
    suggestedBufferSize = chunkDataSize;

  // allocate memory for data reading
  if ( (buffer = (BYTE *) LE_MEMORY_malloc (suggestedBufferSize)) == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "ReadChunkFixedDataPart (): "
      "LE_MEMORY_malloc () fails to allocate %d bytes of memory.\n",
      suggestedBufferSize);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE; // LE_MEMORY_malloc () fails
  }

  // fill in the data buffer with zeroes
  ZeroMemory (buffer, suggestedBufferSize);

  // read fixedChunkDataSize bytes of data into the data buffer
  if ( mmioRead (hMyAVIFile, (char *) buffer, (LONG) chunkDataSize) !=
    (LONG) chunkDataSize )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "ReadChunkFixedDataPart (): "
      "unable to read in %d bytes.\n", chunkDataSize);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    goto ErrorExit;  // unable to read chunk fixed size data part
  }

  // make the data accessible from outside the function
  *ppChunkDataBuffer = buffer;

  return TRUE;

ErrorExit:

  if ( buffer != NULL )
  {
    LE_MEMORY_free (buffer);
    buffer = NULL;
  }

  *ppChunkDataBuffer = NULL;

  return FALSE;
}  // ReadChunkFixedDataPart ()


/*****************************************************************************
 *
 *  BOOL LE_VIDEO_GetVideoStatus (LE_VIDEO_VideoHandle VideoHandle,
 *    LE_VIDEO_VideoStatusPointer StatusPtr)
 *
 *****************************************************************************/

BOOL LE_VIDEO_GetVideoStatus (LE_VIDEO_VideoHandle hVideoHandle,
  LE_VIDEO_VideoStatusPointer statusPtr)
{
  MyDataPointer   aviDataPtr;  // a pointer to the main avi data structure

  if ( hVideoHandle == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_GetVideoStatus (): "
      "hVideoHandle == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE;  // found invalid video handle
  }

  if ( statusPtr == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_GetVideoStatus (): "
      "statusPtr == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    return FALSE;  // found invalid address
  }

  // get a pointer to the main avi data structure
  aviDataPtr = (MyDataPointer) hVideoHandle;

  statusPtr->currentFrame = aviDataPtr->audioFrameNumber;
  statusPtr->desiredFrame = LE_SOUND_GetTimeOfBufSnd (aviDataPtr->hBufSnd);
  statusPtr->numberOfFrames = aviDataPtr->videoStreamHeaderPtr->dwLength;
  #if CE_ARTLIB_EnableDebugMode
    statusPtr->firstShownAtTime = aviDataPtr->videoStartTimeInTicks;
    // Global clock when the video started.
    statusPtr->framesDrawn = aviDataPtr->videoFramesShown;
    // Total number of frames drawn, not including skipped ones.
    // These are used for frame rate evaluation.
  #endif

  statusPtr->soundHandle = aviDataPtr->hBufSnd;
  // So that you can fiddle with the sound, usually for changing
  // its position in space if the video is moving across the screen.

  return TRUE;

} // LE_VIDEO_GetVideoStatus ()


/*****************************************************************************
 *
 *  void ClearUpAVIDataRecord (MyDataPointer aviDataPtr)
 *
 *  Clears up all allocated memory associated with MyAVIDataRecord data
 *  structure.
 *
 *****************************************************************************/

void ClearUpAVIDataRecord (MyDataPointer aviDataPtr)
{
  // deallocate all allocated resources associated with MyAVIDataRecord,
  // the main avi data structure

  // MainAVIHeader
  if ( aviDataPtr->mainAVIHeaderPtr != NULL )
  {
    LE_MEMORY_free (aviDataPtr->mainAVIHeaderPtr);
    aviDataPtr->mainAVIHeaderPtr = NULL;
  }

  // AVIStreamHeader for the video stream
  if ( aviDataPtr->videoStreamHeaderPtr != NULL )
  {
    LE_MEMORY_free (aviDataPtr->videoStreamHeaderPtr);
    aviDataPtr->videoStreamHeaderPtr = NULL;
  }

  // AVIStreamHeader for the audio stream
  if ( aviDataPtr->audioStreamHeaderPtr != NULL )
  {
    LE_MEMORY_free (aviDataPtr->audioStreamHeaderPtr);
    aviDataPtr->audioStreamHeaderPtr = NULL;
  }

  // Data format for the video stream
  if ( aviDataPtr->videoStreamFormatPtr != NULL )
  {
    LE_MEMORY_free (aviDataPtr->videoStreamFormatPtr);
    aviDataPtr->videoStreamFormatPtr = NULL;
  }

  // Data format for the audio stream
  if ( aviDataPtr->audioStreamFormatPtr != NULL )
  {
    LE_MEMORY_free (aviDataPtr->audioStreamFormatPtr);
    aviDataPtr->audioStreamFormatPtr = NULL;
  }

  // data buffer for the AVI index chunk 'idx1'
  if ( aviDataPtr->indexChunkDataBuffer != NULL )
  {
    LE_MEMORY_free (aviDataPtr->indexChunkDataBuffer);
    aviDataPtr->indexChunkDataBuffer = NULL;
  }

  // video output data format -- bmiOutputHeaderPtr
  if ( aviDataPtr->bmiOutputHeaderPtr != NULL )
  {
    LE_MEMORY_free (aviDataPtr->bmiOutputHeaderPtr);
    aviDataPtr->bmiOutputHeaderPtr = NULL;
  }

  // all avi data buffers
  MyAVIDataBufferPointer tempDataBuffer; // for avi data buffers

  for ( int i = 0; i < AVI_DATA_BUFFER_NUMBER; i++ )
  {
    tempDataBuffer = aviDataPtr->dataBuffer[i];

    if ( tempDataBuffer != NULL )
    {
      // aviDataBuffer
      if ( tempDataBuffer->aviDataBuffer != NULL )
      {
        LE_MEMORY_free (tempDataBuffer->aviDataBuffer);
        tempDataBuffer->aviDataBuffer = NULL;
      }

      // free the data buffer itself
      LE_MEMORY_free (aviDataPtr->dataBuffer[i]);
      aviDataPtr->dataBuffer[i] = NULL;
    }
  }

  // free resources allocated for video image decompression
  if (aviDataPtr->hIC)
  {
    // end decompressing video frames
    ICDecompressExEnd (aviDataPtr->hIC);

    // close decompressor
    ICClose (aviDataPtr->hIC);
    aviDataPtr->hIC = NULL;
  }

  // Stop the BufSound system and free all internal buffers.
  if (aviDataPtr->hBufSnd)
  {
    LE_SOUND_StopBufSnd (aviDataPtr->hBufSnd);
    aviDataPtr->hBufSnd = NULL;
  }

  #if CE_ARTLIB_VideoMultiTaskingEnabled
    // remove the two message queues
    LE_QUEUE_Cleanup (&aviDataPtr->messagesToDataBufferingThread);
    LE_QUEUE_Cleanup (&aviDataPtr->messagesToVideoControlThread);
  #endif

  #if CE_ARTLIB_VideoFrameJumpModeEnabled
    // memory allocated for frameIndexArray
    if ( aviDataPtr->frameIndexArray != NULL )
    {
      LE_MEMORY_free (aviDataPtr->frameIndexArray);
      aviDataPtr->frameIndexArray = NULL;
    }

    #if !CE_ARTLIB_VideoMultiTaskingEnabled
      // We need to release the resources allocated for
      // our critical section
      DeleteCriticalSection (&aviDataPtr->myCriticalSection);
    #endif
  #endif
  
  // successful
  return;
}  // ClearUpAVIDataRecord ()


/*****************************************************************************
 *
 *  void CloseMyAVIFile (MyAVIFilePointer aviFilePtr)
 *
 *  Ascends out of all chunks and closes the file.
 *
 *****************************************************************************/

void CloseMyAVIFile (MyAVIFilePointer aviFilePtr)
{
  // If the riff file is open, ascend out of all chunk/subchunks and close the file

  if ( aviFilePtr->file != NULL )
  {
    if ( aviFilePtr->nestLevel > 0 )
      aviFilePtr->nestLevel = 0;

    // close the AVI file
    mmioClose (aviFilePtr->file, 0);
    aviFilePtr->file = NULL;
  }
}  // end of CloseMyAVIFile 


/*****************************************************************************
 *
 *  BOOL PrepareVideoIndexForSelectiveJumpingMode (MyDataPointer aviDataPtr)
 *
 *  Internally generates an array of video frame index for selective
 *  jumping playback mode.
 *
 *  Return: TRUE for success and FALSE for failure.
 *
 *****************************************************************************/

BOOL PrepareVideoIndexForSelectiveJumpingMode (MyDataPointer aviDataPtr)
{
#if CE_ARTLIB_VideoFrameJumpModeEnabled

  AVIINDEXENTRY    **frameIndexArray;
  AVIINDEXENTRY     *indexPtr;
  WORD               videoStreamID;
  DWORD              totalVideoFrames;

  // check for the aviDataPtr
  if ( aviDataPtr == NULL )
  {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer,
        "PrepareVideoIndexForSelectiveJumping (): aviDataPtr == NULL.\n");
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
    return FALSE; // invalid aviDataPtr
  }

  // allocate memory for the video frameIndexArray
  totalVideoFrames = aviDataPtr->videoStreamHeaderPtr->dwLength;
  frameIndexArray = (AVIINDEXENTRY **) LE_MEMORY_malloc (
    totalVideoFrames * sizeof (AVIINDEXENTRY));

  if ( frameIndexArray == NULL )
  {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer,
        "PrepareVideoIndexForSelectiveJumping (): unable to allocate %d bytes of"
        "memory for frameIndexArray.\n",
        totalVideoFrames * sizeof(AVIINDEXENTRY));
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif
    return FALSE; // fails to allocate memory for the movie frameIndexArray
  }

  // save frameIndexArray in our avi data structure
  aviDataPtr->frameIndexArray = frameIndexArray;

  // initialise local variables
  indexPtr = aviDataPtr->indexChunkDataBuffer;
  videoStreamID = aviDataPtr->videoStreamID;

  // process the movie index chunk and generate a new index array
  // for video frames only (for video jump frame playback mode)
  for ( int i = 0; i < (LONG) totalVideoFrames; i ++ )
  {
    // get to the next video frame index entry
    while ( LOWORD (indexPtr->ckid) != videoStreamID )
      indexPtr ++;

    // save the video frame index in frameIndexArray
    aviDataPtr->frameIndexArray[i] = indexPtr;

    // increment indexPtr by one
    indexPtr ++;
  }

#endif

  // successful
  return TRUE;

} // PrepareVideoIndexForSelectiveJumping ()


/*******************************************************************************
 *
 * BOOL LE_VIDEO_SetUpVideoAlternative (LE_VIDEO_VideoHandle hVideoHandle,
 *   LONG decisionFrameNumber, LONG jumpToFrameNumber)
 *
 * Sets up an alternative frame for the next possible frame jump action.
 *
 * It returns TRUE for success and FALSE for failure.
 *
 *******************************************************************************/

BOOL LE_VIDEO_SetUpVideoAlternative (LE_VIDEO_VideoHandle hVideoHandle,
  INT32 decisionFrameNumber, INT32 jumpToFrameNumber)
{
#if CE_ARTLIB_VideoFrameJumpModeEnabled
  
  AVIINDEXENTRY  **frameIndexArray; // an array of video frame index entry
  MyDataPointer    aviDataPtr;      // pointer to the main avi data structure
  LONG             tempFrame;
  
  // check for validity
  if ( hVideoHandle == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_SetUpVideoAlternative (): "
      "hVideoHandle == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
  #endif
    return FALSE; // found an invalid video handle
  }

  // get a pointer to the main avi data structure
  aviDataPtr = (MyDataPointer) hVideoHandle;

  // get the frameIndexArray
  frameIndexArray = aviDataPtr->frameIndexArray;

  // get to the requested video frame
  if ( jumpToFrameNumber < 0 ||
    jumpToFrameNumber > (LONG) aviDataPtr->videoStreamHeaderPtr->dwLength )
  {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_SetUpVideoAlternative (): "
        "found invalid jumpToFrameNumber (%d).\n", jumpToFrameNumber);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
    #endif
    
    if ( jumpToFrameNumber < 0 )
      tempFrame = 0;
    else
      tempFrame = (LONG) aviDataPtr->videoStreamHeaderPtr->dwLength - 1;
  }
  else
    // get the requested video frame number
    tempFrame = jumpToFrameNumber;

  // If the requested video frame is not a key frame, get its nearest
  // previous key frame.
  while ( (frameIndexArray[tempFrame]->dwFlags & AVIIF_KEYFRAME) == FALSE )
    tempFrame --;

  #if CE_ARTLIB_EnableDebugMode && 0
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_SetUpVideoAlternative (): "
      "The requested frame to jump is frame %d. "
      "the player jumps to frame %d.\n", jumpToFrameNumber, tempFrame);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
  #endif

  // setup the video alternative frame jump option
  #if CE_ARTLIB_VideoMultiTaskingEnabled
    // send a message to the video control thread to
    // set up an alternative frame point
    LE_QUEUE_EnqueueWithParameters (
      &aviDataPtr->messagesToVideoControlThread,
      LI_VIDEO_MESSAGE_ALTERNATIVE_FRAME_SETUP, 0, 0,
      tempFrame /* jumpToFrameNumber */,
      decisionFrameNumber, 0, 0, 0, NULL, 0);
  #else
    // set up an alternative frame for the next possible frame jump action
    // for thread safety
    EnterCriticalSection (&aviDataPtr->myCriticalSection);

    // reset some members of the main avi data structure
    aviDataPtr->bTakeAlternative = FALSE;
    aviDataPtr->jumpToFrameNumber = tempFrame;
    aviDataPtr->decisionFrameNumber = decisionFrameNumber;

    // It is safe to leave the critical section
    LeaveCriticalSection (&aviDataPtr->myCriticalSection);
  #endif

#endif

  // successful
  return TRUE;

} // LE_VIDEO_SetUpVideoAlternative ()


/*****************************************************************************
 *
 * BOOL LE_VIDEO_ChangeVideoAlternative (LE_VIDEO_VideoHandle hVideoHandle,
 *   LONG decisionFrameNumber, BOOL bTakeJump)
 *
 * The function checks to see if there is any alternative frame state setup.
 * If yes, the action to be taken depends on bTakeJump. If TRUE, no action is
 * to be taken, if FALSE, unset the alternative frame state and reclaim back
 * the data buffer which contains the specified alternative frame for normal
 * sequencial playback mode.
 *
 * If there is no alternative frame already set up, and if bTakeJump is TRUE,
 * set up the previously specified alternative frame number and get the needed
 * avi data buffered for the next possible frame jump action. If no previously
 * specified alternative frame number, then set up frame 0, the first frame
 * of the movie.
 *
 * Returns TRUE for success and FALSE for failure.
 *
 *****************************************************************************/

BOOL LE_VIDEO_ChangeVideoAlternative (LE_VIDEO_VideoHandle hVideoHandle,
  INT32 decisionFrameNumber, BOOL bTakeJump)
{
#if CE_ARTLIB_VideoFrameJumpModeEnabled

  MyDataPointer          aviDataPtr;     // point to the main avi data structure

  // check for validity
  if ( hVideoHandle == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_ChangeVideoAlternative (): "
      "hVideoHandle == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
  #endif
    return FALSE; // found an invalid video handle
  }

  // get a pointer to the main avi data structure
  aviDataPtr = (MyDataPointer) hVideoHandle;

  #if CE_ARTLIB_VideoMultiTaskingEnabled
    // send a message to the video control thread to
    // change the alternative jump setting
    LE_QUEUE_EnqueueWithParameters (
      &aviDataPtr->messagesToVideoControlThread,
      LI_VIDEO_MESSAGE_CHANGE_VIDEO_ALTERNATIVE, 0, 0,
      bTakeJump, 0, 0, 0, 0, NULL, 0);
  #else
    // set up an alternative frame for the next possible frame jump action
    // for thread safety
    EnterCriticalSection (&aviDataPtr->myCriticalSection);

    // switch on video alternative frame jump option
    aviDataPtr->bTakeAlternative = bTakeJump;

    // It is safe to leave the critical section
    LeaveCriticalSection (&aviDataPtr->myCriticalSection);
  #endif
#endif

  return TRUE;

} // LE_VIDEO_ChangeVideoAlternative ()


/******************************************************************************
 *
 * BOOL LE_VIDEO_ForgetVideoAlternatives (VideoPntr->videoHandle)
 *
 * If there is an alternative frame already set up, then, unset it --
 * reclaim back the data buffer for normal sequencial playback mode.
 *
 * Returns TRUE for success and FALSE for failure.
 *
 ******************************************************************************/

BOOL LE_VIDEO_ForgetVideoAlternatives (LE_VIDEO_VideoHandle hVideoHandle)
{
#if CE_ARTLIB_VideoFrameJumpModeEnabled

  MyDataPointer   aviDataPtr;     // point to the main avi data structure

  if ( hVideoHandle == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_ForgetVideoAlternatives (): "
      "hVideoHandle == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
  #endif
    return FALSE; // found an invalid video handle
  }

  // get a pointer to the main avi data structure
  aviDataPtr = (MyDataPointer) hVideoHandle;

  #if CE_ARTLIB_VideoMultiTaskingEnabled
    // send a message to the video control thread to
    // forget the alternative frame setting
    LE_QUEUE_EnqueueWithParameters (
      &aviDataPtr->messagesToVideoControlThread,
      LI_VIDEO_MESSAGE_FORGET_VIDEO_ALTERNATIVE, 0, 0,
      0, 0, 0, 0, 0, NULL, 0);
  #else
    // set up an alternative frame for the next possible frame jump action
    // for thread safety
    EnterCriticalSection (&aviDataPtr->myCriticalSection);

    // switch off video alternative frame jump option
    aviDataPtr->decisionFrameNumber = -1;

    // It is safe to leave the critical section
    LeaveCriticalSection (&aviDataPtr->myCriticalSection);
  #endif
#endif

  // successful
  return TRUE;

} // LE_VIDEO_ForgetVideoAlternatives ()


/******************************************************************************
 *
 * BOOL LE_VIDEO_CutVideoToFrame (LE_VIDEO_VideoHandle hVideoHandle,
 *   LONG newFrameNumber)
 *
 * This function is different from LE_VIDEO_SetUpVideoAlternative ().
 * LE_VIDEO_SetUpVideoAlternative () sets up an alternative frame and
 * waits for the decision frame to be reached, and the frame jump action
 * may or may not be taken depending upon if the decision frame is
 * reached or not.
 *
 * LE_VIDEO_CutVideoToFrame () cuts the movie immediately to the specified
 * frame number and plays sequencially after the frame jump.
 * 
 * Both single threaded mode and multi-tasking mode are supported.
 *
 * Returns TRUE for success and FALSE for failure or no data buffer
 *    available.
 * 
 ******************************************************************************/

BOOL LE_VIDEO_CutVideoToFrame (LE_VIDEO_VideoHandle hVideoHandle,
  INT32 newFrameNumber)
{
#if CE_ARTLIB_VideoFrameJumpModeEnabled

  MyDataPointer          aviDataPtr;      // point to the main avi data structure
  AVIINDEXENTRY        **frameIndexArray; // an array of video frame index entry
  LONG                   tempFrame;

  // check for validity
  if ( hVideoHandle == NULL )
  {
  #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_CutVideoToFrame (): "
      "hVideoHandle == NULL.\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
  #endif
    return FALSE; // found an invalid video handle
  }

  // get a pointer to the main avi data structure
  aviDataPtr = (MyDataPointer) hVideoHandle;

  // get the frameIndexArray
  frameIndexArray = aviDataPtr->frameIndexArray;

  // get to the requested video frame
  if ( newFrameNumber < 0 ||
    newFrameNumber > (LONG) aviDataPtr->videoStreamHeaderPtr->dwLength )
  {
    #if CE_ARTLIB_EnableDebugMode
      sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_CutVideoToFrame (): "
        "found invalid newFrameNumber (%d).\n", newFrameNumber);
      LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
    #endif
    
    if ( newFrameNumber < 0 )
      tempFrame = 0;
    else
      tempFrame = (LONG) aviDataPtr->videoStreamHeaderPtr->dwLength - 1;
  }
  else
    // get the requested video frame number
    tempFrame = newFrameNumber;

  // If the requested video frame is not a key frame, get its nearest
  // previous key frame.
  while ( (frameIndexArray[tempFrame]->dwFlags & AVIIF_KEYFRAME) == FALSE )
    tempFrame --;

  #if CE_ARTLIB_EnableDebugMode && 0
    sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_CutVideoToFrame (): "
      "The requested frame to jump is frame %d. "
      "the player jumps to frame %d.\n", newFrameNumber, tempFrame);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
  #endif

  #if CE_ARTLIB_VideoMultiTaskingEnabled
    // lzou
    // Flush the message queue before sending an important message.
    // Otherwise, this message might get lost due to queue full.
    LE_QUEUE_Flush (&aviDataPtr->messagesToVideoControlThread);

    // send a message to the video control thread to
    // throw away all data buffers immediately
    LE_QUEUE_EnqueueWithParameters (
      &aviDataPtr->messagesToVideoControlThread,
      LI_VIDEO_MESSAGE_CUT_VIDEO_TO_FRAME, 0, 0,
      (LONG) frameIndexArray[tempFrame] /* nextVideoChunkIndexPtr */,
      tempFrame /* jumpToFrameNumber */,
      0, 0, 0, NULL, 0);
  #else // for single threaded mode only
    EnterCriticalSection (&aviDataPtr->myCriticalSection); // for thread safety

    // reset some members of the main avi data structure
    aviDataPtr->nextVideoChunkIndexPtr = frameIndexArray[tempFrame];
    aviDataPtr->dataBufferStartVideoFrameNumber =
      aviDataPtr->audioStartPosition =
      aviDataPtr->startVideoFrameForNextAudioFeed = tempFrame;
    aviDataPtr->bCutVideoToFrame = TRUE;  // immediate cut video to frame jump

    // It is safe to leave the critical section
    LeaveCriticalSection (&aviDataPtr->myCriticalSection);
  #endif

#endif

  return TRUE;

} // LE_VIDEO_CutVideoToFrame ()


/*****************************************************************************
 *
 *  BOOL LE_VIDEO_AVIVideoPlay (const char *aviFileName)
 *
 *  This is a quick test function. It takes up full CPU power.
 *  A better way of playing an avi video is to use timer to control
 *  the avi data feeding, instead of continuously feeding avi data.
 *
 *****************************************************************************/

BOOL LE_VIDEO_AVIVideoPlay (const char *aviFileName)
{
  LE_VIDEO_VideoHandle    hVideoHandle;
  BOOL                    bEndVideo = FALSE;
  BOOL                    bStillPlayingVideo;
  BOOL                    bGotFileName;
  char                    fileName [_MAX_PATH];
  int                     return_value = TRUE;

  bGotFileName = FALSE;

  if ( aviFileName == NULL )
  {
    OPENFILENAME            OpenInfo;

    memset (&OpenInfo, 0, sizeof (OpenInfo));
    OpenInfo.lStructSize = sizeof (OpenInfo);
    OpenInfo.hwndOwner = LE_MAIN_HwndMainWindow;
    OpenInfo.lpstrFilter = "Video Files we Know\0*.avi\0";
    OpenInfo.lpstrFile = fileName;
    OpenInfo.nMaxFile = sizeof (fileName);
    fileName [0] = 0;
    OpenInfo.lpstrTitle = "Artech Video Player - Pick a Video";
    OpenInfo.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES;

    ShowCursor (TRUE);
    bGotFileName = GetOpenFileName (&OpenInfo);
    ShowWindow (LE_MAIN_HwndMainWindow, SW_RESTORE);
    ShowCursor (FALSE);
  }
  else
  {
    if ( aviFileName[0] != 0 )
    {
      strcpy (fileName, aviFileName);
      bGotFileName = TRUE;
    }
    else
    {
      #if CE_ARTLIB_EnableDebugMode
        sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_AVIVideoPlay (): "
          "found invalid avi file name (%s).\n", aviFileName);
        LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
      #endif
      bGotFileName = FALSE;
    }
  }
  
  if (bGotFileName)
  {
    // start the video
    hVideoHandle = LE_VIDEO_StartVideo (
      fileName,
      NULL,       // LPBITMAPINFOHEADER UserOutputBitmapInfoPntr = NULL,
      0, 0,       // Output width and height for scaling
      FALSE,      // BOOL LoopAtEndRatherThanPause
      TRUE,       // BOOL FlipVertically = FALSE,
      TRUE,       // BOOL EnableVideo = TRUE,
      TRUE,       // BOOL EnableAudio = TRUE,
      FALSE,      // BOOL Use3DSound = FALSE,
      FALSE,      // bAlternateScanLine for Indeo video 5
      TRUE);      // bDoubleImageSize for Indeo video 5
  }
  else
    return FALSE;

  // feed avi data to the video player
  
  bStillPlayingVideo = FALSE;

  if ( hVideoHandle != NULL )
  {
    // keep feeding avi data until the end of movie
    
    bStillPlayingVideo = TRUE;

    while (!bEndVideo)
    {
      if (LE_VIDEO_FeedVideo (
        hVideoHandle,
        NULL,
        NULL,
        0, 0,
        0,    // brightness
        0,    // contrast
        0,    // saturation
        &bEndVideo) == FALSE)
      {
        bStillPlayingVideo = FALSE;
        return_value = FALSE;
        break;
      }
    
      if (bEndVideo)
      {
        #if CE_ARTLIB_EnableDebugMode
          LE_VIDEO_VideoStatusRecord videoStatus;

          bStillPlayingVideo = FALSE;
          // get video status
          LE_VIDEO_GetVideoStatus (hVideoHandle, &videoStatus);

          sprintf (LE_ERROR_DebugMessageBuffer, "LE_VIDEO_GetVideoStatus (): "
            "videoStatus: startTicks/endTicks (%d/%d)\n"
            "framesShown/totalFrames (%d/%d).\n"
            "fps is %f, currentFrame (%d)\n",
            videoStatus.firstShownAtTime, LE_TIME_TickCount,
            videoStatus.framesDrawn, videoStatus.numberOfFrames,
            (double) videoStatus.framesDrawn * 60 / (LE_TIME_TickCount - videoStatus.firstShownAtTime),
            videoStatus.currentFrame);
          LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
        #endif
      }
    } // end of FeedVideo loop
  }

  // remove the video system
  if (!bStillPlayingVideo)
  {
    if (hVideoHandle)
    {
      LE_VIDEO_StopVideo (hVideoHandle);
      hVideoHandle = NULL;
    }
  }

  return return_value;
}

#endif // if CE_ARTLIB_EnableSystemVideo
