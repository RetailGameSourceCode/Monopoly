#ifndef __L_VIDEO_H__
#define __L_VIDEO_H__

/*****************************************************************************
 *
 *   FILE:        L_Video.h
 *   DESCRIPTION: Video system header.
 *
 *   (C) Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Video.h 3     4/12/99 5:04p Lzou $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Video.h $
 * 
 * 3     4/12/99 5:04p Lzou
 * 
 * 2     3/16/99 1:12p Lzou
 * Have finished the video frame jump playback mode.
 * 
 * 1     3/15/99 12:03p Agmsmith
 * Went corrupt again.
 * 
 * 13    3/10/99 11:26a Lzou
 * Added in some comments for Indeo video (r) codec.
 * 
 * 12    2/18/99 2:50p Lzou
 * 
 * 12    2/18/99 2:10p Lzou
 * Implemented video frame jump playback mode. Now, while playing back a
 * video clip sequencially, the user can set up an alternative frame,
 * change the video alternative frame state, forget the already set
 * alternative frame state, and cut to the video alternative frame (do the
 * pending video frame jump action immediately).
 * 
 * 11    2/09/99 1:17p Lzou
 * Small changes.
 * 
 * 10    2/02/99 3:40p Lzou
 * Have adopted the new video API.
 * 
 * 9     2/02/99 1:47p Agmsmith
 * Oops.
 * 
 * 8     2/02/99 1:16p Agmsmith
 * Should be signed offsets for feed position.
 * 
 * 7     2/02/99 1:07p Agmsmith
 * Simplified feeding and starting options - can't change size during
 * playback, only at startup, and no clipping done now.
 *
 * 6     2/01/99 1:54p Agmsmith
 * Added Indeo properties to the video API.
 *
 * 5     1/31/99 4:42p Agmsmith
 * New improved video API.  No pause function, user always provides the
 * bitmap, etc.
 *
 * 4     1/29/99 4:45p Lzou
 * Have implemented Indeo video 5 specific alpha transparent.
 *
 * 3     1/27/99 1:38p Lzou
 * Have implemented some of Indeo video 5 codec's specific features, such
 * as scalability, colour brightness/contrast/saturation, double image
 * size with/without alternate scanline, fill (or not fill) colour for
 * transparent video images, etc.
 *
 * 2     1/13/99 3:51p Lzou
 * Rechecked in after a SourceSafe crash this morning.
 *
 * 8     1/13/99 12:19p Lzou
 * Some small changes.
 *
 * 7     12/23/98 6:05p Lzou
 * Have implemented a new way of avi data buffering.
 *
 * 6     12/08/98 5:03p Lzou
 * A bug is fixed in LE_VIDEO_FeedVideo (). Now, the video player can play
 * very tough video clips with only 3 avi data buffers. The point is that
 * each time when feeding the video, we need to go through all preloaded
 * avi data buffers and check to see if some of them are empty.
 *
 * 5     11/26/98 5:31p Lzou
 * Have moved all internal data structures back to L_Video.cpp from
 * L_Video.h.
 *
 * 4     11/26/98 10:27a Lzou
 * To improve the new video player's performance on P90, ten preloading
 * avi data buffers are used instead of two. And a new video user
 * interface has been adopted.
 *
 * 3     11/18/98 4:33p Lzou
 * Have moved Artech AVI video player to Artlib99.
 *
 * 2     9/22/98 12:32p Agmsmith
 * All prototypes are now C++ declarations by default.
 *
 * 1     9/14/98 12:35p Agmsmith
 * Initial version copied over from ArtLib98 and renamed from .C to .CPP.
 *
 * 2     6/10/98 6:03p Agmsmith
 * Split up video code into old and new versions, in the same file, with
 * #define controlling it.
 ****************************************************************************/


/*****************************************************************************
 * E X P O R T E D   F U N C T I O N S   F O R   T H E   N E W   P L A Y E R *
 ****************************************************************************/


typedef void *LE_VIDEO_VideoHandle;
  // Used for identifying videos using the fancier buffering scheme,
  // not the old AVIFile individual frame system.

typedef struct LE_VIDEO_VideoStatusStruct
{
  INT32 currentFrame;
    // Current frame number now playing (last drawn actually), -1 if it
    // hasn't started yet, equal to or larger than the number of frames
    // if the movie has reached the end and paused there.  The first frame
    // is frame number zero.

  INT32 desiredFrame;
    // The frame which should be showing, according to the audio
    // playback position at this instant in time.  For silent
    // movies it is the same as the current frame (the caller does
    // the timing for silent movies).

  INT32 numberOfFrames;
    // Total number of frames in the movie.  Should be at least 1
    // otherwise video code will divide by zero etc.

#if CE_ARTLIB_EnableDebugMode
  TYPE_Tick firstShownAtTime;  // Global clock when the video started.
  UNS32     framesDrawn; // Total number of frames drawn, not including skipped ones.
    // These are used for frame rate evaluation.
#endif

  LE_SOUND_BufSndHandle soundHandle;
    // So that you can fiddle with the sound, usually for changing
    // its position in space if the video is moving across the screen.
    // NULL for silent films.

} LE_VIDEO_VideoStatusRecord, *LE_VIDEO_VideoStatusPointer;



extern LE_VIDEO_VideoHandle LE_VIDEO_StartVideo (
  const char *VideoFileName,
  LPBITMAPINFOHEADER UserOutputBitmapInfoPntr,
  INT  DesiredWidth = 0,              // Indeo video 5 (r) codec only
  INT  DesiredHeight = 0,             // Indeo video 5 (r) codec only
  BOOL LoopAtEndRatherThanPause = FALSE,
  BOOL FlipVertically = FALSE,
  BOOL EnableVideo = TRUE,
  BOOL EnableAudio = TRUE,
  BOOL Use3DSound = FALSE,
  BOOL DoubleAlternateLines = FALSE,  // Indeo video 5 (r) codec only
  BOOL DoubleImageSize = FALSE);      // Indeo video 5 (r) codec only
  // Starts up a video from the given file.  UserOutputBitmapInfoPtr
  // describes the user provided bitmap the video will be drawn into,
  // which can be a different depth than the screen (for 3D textures).
  // If the width and height are zero then the movie's width and
  // height will be used, otherwise the movie will be stretched to
  // the given size by the decompressor (if supported).  Sorry, it
  // doesn't seem to support changing the size during playback.
  // Enabling audio and video is used for playing only one part of
  // the video, for example only playing the audio of Alex Trebek
  // reading a Jeopardy! question.
  // If DoubleAlternateLines is TRUE then alternate scan lines in
  // the video will be drawn in black and the rest stretched to
  // double size - only works for Indeo compressed movies, and only
  // if you specify the width and height as exactly twice the
  // movie's width and height.
  // If you have a movie encoded with transparency (Indeo again,
  // with a 1 bit alpha channel image per frame) then it gets
  // drawn as transparent, the caller should clear the bitmap to
  // the background picture before decompressing each frame.
  // Returns a handle to the video, or NULL if it fails.
  //
  // The best way of specifying double image size (alternate scan line)
  // is using DoubleImageSize (DoubleAlternateLines) and leave
  // DesiredWidth and DesiredHeight to be zero.

extern BOOL LI_VIDEO_VisualInspectDataBufferStatus (
  LE_VIDEO_VideoHandle hVideoHandle, int bufferIndex);
  // This is for debugging/testing purpose only, showing how many avi data
  // buffers are used, how much amount of avi data are preloaded, and how they
  // are fed. This is useful when playing back video clips on a slow machine.
  // Four colours are used to show the video player's buffering states.
  //    yellowish green  -- current (playing) avi data buffer
  //    black            -- empty avi data buffer
  //    blue             -- freshly loaded avi data (both audio and video data)
  //    red              -- buffer with valid video data (audio data are fed)

extern BOOL LE_VIDEO_StopVideo (LE_VIDEO_VideoHandle VideoHandle);
  // Make the video stop playing, closes files, deallocates buffers etc.
  // Returns TRUE if successful, returns FALSE if successful but something
  // went wrong.

extern BOOL LE_VIDEO_FeedVideo (
  LE_VIDEO_VideoHandle VideoHandle,
  UNS32 *FrameNumberPntr,
  LPBYTE OutputBitmapPixelsPntr,
  INT32  OffsetX = 0,
  INT32  OffsetY = 0,
  short  Brightness = 0,       // Indeo video 5 (r) codec only (-255, 255)
  short  Contrast = 0,         // Indeo video 5 (r) codec only (-255, 255)
  short  Saturation = 0,       // Indeo video 5 (r) codec only (-255, 255)
  BOOL  *bEndVideo = NULL);    // TRUE when the end of movie is detected.
  // Reads the file, submits data for sound playback, and decompresses
  // the current video frame, and draws it on the given bitmap.
  //
  // The frame to display is determined by the sound playback position
  // (for noisy films) or by the user provided frame number in
  // *FrameNumberPntr (for silent films).  The new current frame is
  // written into *FrameNumberPntr before the function returns.  If the
  // frame number is the same as the last one drawn then gets drawn
  // again.
  //
  // The attributes of the destination bitmap (width, height) were
  // specified when the video was started.  Only the position can be
  // updated every frame.  It is drawn in the given spot, but you should
  // take care that the image does not go over the destination bitmap
  // boundaries since clipping isn't done.
  //
  // OutputBitmapPixelsPntr points to the start of the pixel memory,
  // depth and bytes per scan line were specified when the video was
  // started.  Since it may be a locked video surface pointer, don't
  // put up any dialog boxes or other Windows things since that will
  // fail.
  //
  // Note that if you want to pause the film, just pause the sound for
  // noisy films or keep on feeding in the same frame number for
  // silent films.
  //
  // Returns TRUE if successful, FALSE on errors.  Look at the frame
  // number to see if it hit the end of the film (it returns TRUE when
  // stopped at end and keeps on displaying the last frame).

extern BOOL LE_VIDEO_GetVideoStatus (LE_VIDEO_VideoHandle VideoHandle,
  LE_VIDEO_VideoStatusPointer StatusPtr);
  // Fills your buffer with information about the video.  Also interrogates
  // the audio hardware for the current sound position.

  // ==================================================================//

  // Following are utility functions used in frame jump actions when
  // the video frame jump playback mode is enabled.
  // For the video player to work correctly in doing video frame jumps,
  // all avi files need to be reblocked using reblockavi.exe.

extern BOOL LE_VIDEO_CutVideoToFrame (LE_VIDEO_VideoHandle VideoHandle,
  INT32 NewFrameNumber);
  // Immediately jumps to the given frame.  Right now - pending buffered
  // sound is skipped etc.

extern BOOL LE_VIDEO_SetUpVideoAlternative (LE_VIDEO_VideoHandle VideoHandle,
  INT32 DecisionFrameNumber, INT32 JumpToFrameNumber);
  // Sets up a possible jump point at frame number DecisionFrameNumber, where
  // the movie can either continue on to frame DecisionFrameNumber+1 or jump
  // to JumpToFrameNumber after finishing playing DecisionFrameNumber.  By
  // default it will continue on.  Internally, the player will buffer frames
  // for both alternatives and sound for the default alternative so that it
  // doesn't interrupt smooth playback.  If it does jump, the sound cuts to
  // the JumpToFrameNumber after playing the decision frame (the feeding may
  // have to busy wait to get the exact timing).  If it misses the jump (due
  // to not calling the feed routine often enough), you may get some extra
  // frames and sound from the main path before the jump kicks in - if several
  // jumps are covered by the feeding then it uses the latest one and ignores
  // the earlier ones.  Use -1 for the JumpToFrameNumber to cancel the jump;
  // the alternative stays in effect even after being played, in case the film
  // loops back.

extern BOOL LE_VIDEO_ChangeVideoAlternative (LE_VIDEO_VideoHandle VideoHandle,
  INT32 DecisionFrameNumber, BOOL TakeJump);
  // Switches between taking the alternative jump or continuing straight on.
  // Returns TRUE if successful, FALSE if the jump hasn't been set up.

extern BOOL LE_VIDEO_ForgetVideoAlternatives (LE_VIDEO_VideoHandle hVideoHandle);
  // Forgets the already setup alternative frame for the next possible frame
  // jump action.

extern BOOL LE_VIDEO_AVIVideoPlay (const char *aviFileName);
  // This is a quick test function. It takes up full CPU power.
  // A better way of playing an avi video is to use timer to control
  // the avi data feeding, instead of continuously feeding avi data.

#endif // __L_VIDEO_H__
