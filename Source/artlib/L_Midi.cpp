/*************************************************************
*
*   FILE:           L_MIDI.C
*   DESCRIPTION:    Main MIDI source file.
*
*   (C) Copyright 1995/96 Artech Digital Entertainments.
*                         All rights reserved.
*
*************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"
/************************************************************/

#if CE_ARTLIB_EnableSystemMidi

/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

UINT LE_MIDI_LoopFlag = FALSE;
UINT LE_MIDI_DeviceId = 0;
/************************************************************/

// global variables...
BOOL LE_MIDI_bMidiFilePlaying; // TRUE when a midi file is playing, else FALSE

/*************************************************************
*
* void LI_MIDI_InitSystem(void);
*
*   Description:
*       MIDI system initializion procedure.
*
*************************************************************/
void LI_MIDI_InitSystem(void)
{
    LE_MIDI_bMidiFilePlaying = FALSE;
}
/************************************************************/

/*************************************************************
*
* void LI_MIDI_RemoveSystem(void);
*
*   Description:
*       MIDI system cleanup and removal procedure.
*
*************************************************************/
void LI_MIDI_RemoveSystem(void)
{
    LE_MIDI_CloseMidiDevice();
}
/************************************************************/

/*************************************************************
*
* void LE_MIDI_CloseMidiDevice(void);
*
*   Description:
*       Closes the midi device opened by a previous call to playMIDIFile().
*
*   Arguments:
*       None.
*
*   Returns:
*       None.
*
*   Notes, Bugs, Fixes, Changes, etc:
*       - 09:12:43 Tue  12-19-1995 (MMH)
*************************************************************/
void LE_MIDI_CloseMidiDevice(void)
{
    if (LE_MIDI_DeviceId != 0)
    {
        mciSendCommand(LE_MIDI_DeviceId, MCI_CLOSE, MCI_WAIT, 0);
        LE_MIDI_DeviceId = 0;
        LE_MIDI_bMidiFilePlaying = FALSE;
    }
}
/************************************************************/

/*************************************************************
*
* DWORD LE_MIDI_PlayMidiFile(LPSTR lpszMIDIFileName, UINT Loop);
*
*   Description:
*       Starts a midi file from disk playing.  If a midi file is already playing
*       it is stopped.  If the loop flag is set, the midi file will continue to
*       loop forever until a call to CloseMidiDevice().
*
*   Arguments:
*       lpszMIDIFileName    - The midi file name (with/without path).
*       Loop                - TRUE if the midi stream is to be looped.
*
*   Returns:
*       0 if succesfull, otherwise an error code.
*
*   Notes, Bugs, Fixes, Changes, etc:
*       - 09:12:43 Tue  12-19-1995 (MMH)
*************************************************************/
DWORD LE_MIDI_PlayMidiFile(LPSTR lpszMIDIFileName, UINT Loop)
{
    DWORD dwReturn = 0;
    MCI_OPEN_PARMS mciOpenParms;
    MCI_PLAY_PARMS mciPlayParms;

    LE_MIDI_LoopFlag = Loop;

    LE_MIDI_CloseMidiDevice();

    // Open the device by specifying the device name and device element.
    // MCI will attempt to choose the MIDI Mapper as the output port.
    memset(&mciOpenParms,0,sizeof(MCI_OPEN_PARMS));
//    mciOpenParms.lpstrDeviceType = "sequencer";
    mciOpenParms.lpstrElementName = lpszMIDIFileName;

    dwReturn = mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_WAIT,(DWORD)(LPVOID) &mciOpenParms);

    if (dwReturn)
    {
//    #if CE_ARTLIB_EnableDebugMode
//        wsprintf(LE_ERROR_DebugMessageBuffer,"LE_MIDI_PlayMidiFile(): Couldn't open midi device.\r\n");
//        LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer,LE_ERROR_DebugMsgToFile);
//    #endif
    }
    else
    {
        // Device opened successfully, get the device ID.
        LE_MIDI_DeviceId = mciOpenParms.wDeviceID;

        // See if the output port is the MIDI Mapper.

//        memset(&mciStatusParms,0,sizeof(MCI_STATUS_PARMS));
//        mciStatusParms.dwItem = MCI_SEQ_STATUS_PORT;
//        dwReturn = mciSendCommand(LE_MIDI_DeviceId, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT,(DWORD)(LPVOID) &mciStatusParms);
//        if (dwReturn)
//        {
//            mciSendCommand(LE_MIDI_DeviceId, MCI_CLOSE, MCI_WAIT, 0);
//        }
//        else
//        {
            // The output port is not the MIDI Mapper, ask if user wants to continue.
//            if (LOWORD(mciStatusParms.dwReturn) != LOWORD(MIDI_MAPPER))
//            {
                /// User does not want to continue. Not an error, just close the device and return.
//                mciSendCommand(LE_MIDI_DeviceId, MCI_CLOSE, MCI_WAIT, 0);
//                dwReturn = 0;
//            }

            // Begin playback. The window procedure function for the parent window
            // will be notified with an MM_MCINOTIFY message when playback is
            // complete. At this time, the window procedure closes the device.
            memset(&mciPlayParms,0,sizeof(MCI_PLAY_PARMS));
            mciPlayParms.dwCallback = (DWORD) LE_MAIN_HwndMainWindow;
            dwReturn = mciSendCommand(LE_MIDI_DeviceId, MCI_PLAY, MCI_NOTIFY, 
                (DWORD)(LPVOID) &mciPlayParms);
            if (dwReturn)
            {
                mciSendCommand(LE_MIDI_DeviceId, MCI_CLOSE, MCI_WAIT, 0);
            }
            else
                LE_MIDI_bMidiFilePlaying = TRUE;
//        }
    }

    return (dwReturn);
}
/************************************************************/

/*************************************************************
*
* void LE_MIDI_ProcessMidiNotifies(WPARAM wParam);
*
*   Description:
*       This routine should be called by the midi "window procedure" when a
*       notify message is received.
*
*   Arguments:
*       wParam          - Standard windows message wParam.
*
*   Returns:
*       None.
*
*   Notes, Bugs, Fixes, Changes, etc:
*       - 09:12:43 Tue  12-19-1995 (MMH)
*************************************************************/
void LE_MIDI_ProcessMidiNotifies(WPARAM wParam, LPARAM lParam)
{
    MCI_STATUS_PARMS    mciStatus;
    DWORD               dwReturn = 0;

    if (LOWORD(lParam) == LE_MIDI_DeviceId)
    {
        switch(wParam)
        {
            case MCI_NOTIFY_SUCCESSFUL:
                if (LE_MIDI_LoopFlag && (LE_MIDI_DeviceId != 0))
                {
                    MCI_PLAY_PARMS mciPlayParms;
                    MCI_SEEK_PARMS mciSeekParms;

                    memset(&mciSeekParms,0,sizeof(MCI_SEEK_PARMS));
                    if (mciSendCommand(LE_MIDI_DeviceId, MCI_SEEK, MCI_SEEK_TO_START | MCI_WAIT, (DWORD)(LPVOID) &mciSeekParms))
                    {
                        mciSendCommand(LE_MIDI_DeviceId, MCI_CLOSE, MCI_WAIT, 0);
                        LE_MIDI_DeviceId = 0;
                        return;
                    }
                    memset(&mciPlayParms,0,sizeof(MCI_PLAY_PARMS));
                    mciPlayParms.dwCallback = (DWORD) LE_MAIN_HwndMainWindow;
                    if (mciSendCommand(LE_MIDI_DeviceId, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms))
                    {
                        mciSendCommand(LE_MIDI_DeviceId, MCI_CLOSE, MCI_WAIT, 0);
                        LE_MIDI_DeviceId = 0;
                        return;
                    }
                }
                else // doesn't loop, check status flags...
                {
                    // to receive a midi stopped message...
                    memset(&mciStatus, 0, sizeof(MCI_STATUS_PARMS));
                    mciStatus.dwCallback = (DWORD) LE_MAIN_HwndMainWindow;
                    mciStatus.dwItem     = MCI_STATUS_MODE;
                    dwReturn = mciSendCommand(
                        LE_MIDI_DeviceId, 
                        MCI_STATUS, 
                        MCI_STATUS_ITEM, 
                        (DWORD)(LPSTR)&mciStatus);
                    if (dwReturn != 0)
                        mciSendCommand(LE_MIDI_DeviceId, MCI_CLOSE, MCI_WAIT, 0);

                    if (mciStatus.dwReturn == MCI_MODE_STOP)
                    {
                        LE_MIDI_CloseMidiDevice();
                    }
                }
                break;

            default:
                break;
        }
    }

}
/************************************************************/

#endif // if CE_ARTLIB_EnableSystemMidi
