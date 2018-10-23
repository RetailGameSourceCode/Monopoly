#ifndef __L_TIMERS_H__
#define __L_TIMERS_H__

/*****************************************************************************
 *
 * FILE:        L_Timers.h
 *
 * DESCRIPTION: Timing system, including cyclic functions and global timers.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/LibraryCode/L_Timers.h 4     11/06/98 7:14p Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/LibraryCode/L_Timers.h $
 * 
 * 4     11/06/98 7:14p Agmsmith
 * Trying to get the single tasking mode to work.
 ****************************************************************************/

/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

extern volatile UNS16 LE_Timers[CE_ARTLIB_MaxTimersArray];
// These arrays count down by 1 every 1/10th of a second to zero. If zero
// they stop counting.  Used by code as quick simple 1 tenth of a second
// timers.  CE_ARTLIB_MaxTimersArray currently set to 4. Use by setting
// this timer to some value and then poll it occasionally seeing when it
// goes to zero.  Well, that's the default behaviour.  See the following
// variables for altering it.

extern UNS16 LE_TimerRestartCount[CE_ARTLIB_MaxTimersArray];
// When a timer hits zero, it gets immediately reset to this value.  So,
// if this value isn't zero, you will never actually see the timer at
// zero (it appears to go from 1 to this value).  To make the timer stop
// at zero, leave this zero.  Also, if the LE_Timers is zero then the timer
// is inactive, you have to kick-start it by setting it to non-zero.

extern UNS8 LE_TimerSpeeds[CE_ARTLIB_MaxTimersArray];
// This controls the countdown speed of each timer.  After this many ticks
// at 60 ticks per second (50 in PAL mode), the corresponding LE_Timers
// entry is decremented.  These are normally set to 6, so you get the
// standard 1/10 second rate in LE_Timers.  60 (better to use
// CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ than hard coding 60) makes the timer
// decrement once per second, etc.

extern UNS8 LE_TimerSendUIMessage[CE_ARTLIB_MaxTimersArray];
// When a timer reaches zero (even for that momentary situation if you are
// using LE_TIME_TimerRestartCount), it can optionally add an
// UIMSG_TIMER_REACHED_ZERO event to the message queue in UI_MSG.  Set this
// boolean to TRUE to get a message sent.

extern volatile TYPE_Tick LE_TIME_TickCount;
// This global count starts at zero when the game starts and counts up 1 tick
// every 1/60 of a second (1/50 in PAL video mode systems).  You can use it to
// keep track of elapsed time and for other things.  It wraps around after a
// bit over a century.



/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/
extern void       LI_TIMERS_RemoveSystem(void);
extern void       LI_TIMERS_InitSystem(void);
extern void       LE_TIMERS_Suspend (BOOL SuspendIt);
extern TYPE_Tick  LE_TIMER_Delay (TYPE_Tick DelayTickCount);

#if !CE_ARTLIB_EnableMultitasking
extern void LI_TIMER_CallCyclicFunctions (void);
#endif

#endif // __L_TIMERS_H__
