/*****************************************************************************
 * Voice Chat Subroutines by Artech Digital Entertainments, copyright (c) 1997.
 *
 * Tab stops are every 2 spaces, also save the file with spaces instead of tabs.
 *
 * $Header: /Projects - 1999/Mono 2/L_voice.h 2     5/26/99 11:29 Davew $
 *
 * $Log: /Projects - 1999/Mono 2/L_voice.h $
 * 
 * 2     5/26/99 11:29 Davew
 * Minor changes
 * 
 * 1     1/08/99 2:06p Russellk
 * 
 * 1     1/08/99 11:36a Russellk
 * Graphics
 * 
 * 2     12/17/98 4:26p Russellk
 * Obsolete comments removed for new version.  Log file facility entered
 * in Mess.c to record phase and action/error message activity.
 * 
 */

extern BOOL LE_VOICE_PLAYBACK_AVAILABLE;
extern BOOL LE_VOICE_CAPTURE_AVAILABLE;

#define LE_VOICE_MAX_COMPRESSORS 50
extern unsigned char LE_VOICE_NUM_COMPRESSORS;
extern unsigned char LE_VOICE_DESIRED_COMPRESSOR;
extern char *LE_VOICE_COMPRESSOR_NAMES [LE_VOICE_MAX_COMPRESSORS];

extern long LE_VOICE_SAMPLES_PER_SEC;
extern short LE_VOICE_BITS_PER_SAMPLE;
extern short LE_VOICE_NUMBER_OF_CHANNELS;


extern BOOL LE_VOICE_PLAY_CHAT(BYTE *WaveFileData, DWORD WaveFileSize,
                               LONG PanValue, LONG VolumeValue);

extern BOOL LE_VOICE_RECORD_START(DWORD MaxRecordingSize);

extern float LE_VOICE_RECORD_GET_POSITION();

extern BOOL LE_VOICE_RECORD_FINISH(BYTE **WaveBufferPntrPntr,
                                   DWORD *WaveBufferSizePntr);

extern void LE_VOICE_REGENERATE_COMPRESSOR_LIST();

extern void LE_VOICE_InitSystem(HWND MainWindow);

extern void LE_VOICE_RemoveSystem();
