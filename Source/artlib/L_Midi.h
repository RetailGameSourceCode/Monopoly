#ifndef __L_MIDI_H__
#define __L_MIDI_H__

/*************************************************************
*
*   FILE:           l_MIDI.h
*   DESCRIPTION:    MIDI system header.
*
*   (C) Copyright 1995/96 Artech Digital Entertainments.
*                         All rights reserved.
*
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/*----------------------------------------------------------*/

/************************************************************/

/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/

/************************************************************/

/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

extern BOOL LE_MIDI_bMidiFilePlaying;

/************************************************************/

/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

void LI_MIDI_RemoveSystem(void);
void LI_MIDI_InitSystem(void);
void LE_MIDI_ProcessMidiNotifies(WPARAM wParam, LPARAM lParam);

// Start Doc
// L_MIDI

DWORD LE_MIDI_PlayMidiFile(LPSTR lpszMIDIFileName, UINT Loop);
// - Starts a midi file (on the hard drive).
// - If a midi file is already playing it is stopped.
// - If the loop flag is set, the midi file will continue to loop forever until a call to CloseMidiDevice().

void LE_MIDI_CloseMidiDevice(void);
// - Closes the midi device opened by a previous call to LE_MIDI_PlayMidiFile().

// End Doc
/************************************************************/

#endif // __L_MIDI_H__
