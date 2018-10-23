/*****************************************************************************
 *
 * FILE:        L_Timers.cpp
 *
 * DESCRIPTION: Timing system, including cyclic functions and global timers.
 *
 * © Copyright 1998 Artech Digital Entertainments.  All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Timers.cpp 5     7/14/99 9:37a Agmsmith $
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Timers.cpp $
 * 
 * 5     7/14/99 9:37a Agmsmith
 * Don't delay in single tasking mode while timers suspended.
 *
 * 4     7/12/99 11:55a Agmsmith
 * Use atexit () to make sure timers are shut down in most situations.
 *
 * 3     11/06/98 7:14p Agmsmith
 * Trying to get the single tasking mode to work.
 ****************************************************************************/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemTimers

#define LI_TIMERS_USE_TIME_BEGIN 0
  // Turn this on to use the suspicious timeBeginPeriod Windows
  // function calls around the timer code.  It seems to make the
  // game sluggish, so avoid it?


/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

volatile UNS16      LE_Timers[CE_ARTLIB_MaxTimersArray];
UNS16               LE_TimerRestartCount[CE_ARTLIB_MaxTimersArray];
UNS8                LE_TimerSpeeds[CE_ARTLIB_MaxTimersArray];
UNS8                LE_TimerSendUIMessage[CE_ARTLIB_MaxTimersArray];
volatile TYPE_Tick  LE_TIME_TickCount;



/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

static UINT     MinTimerRes = 0;  /* Zero means Windows time system (timeBeginPeriod) not initialised. */
static MMRESULT TimerID = 0; /* Zero for Windows timer not allocated. */
static UNS8     RemainingTimerTicks[CE_ARTLIB_MaxTimersArray];  /* Counts partial ticks for LE_Timers. */
static BOOL volatile TimersSuspended; // TRUE to pause the clock.

static HANDLE   hTimerCycleTriggerEvent; // Triggers sleeping threads.
static UNS8 volatile TimerSystemIsClosingDown; // Set it to TRUE to make the threads exit.

#if CE_ARTLIB_EnableMultitasking
static HANDLE   hTimerCycleThread; // A thread that runs cyclic update things, like animation engines.
#endif



/*****************************************************************************
 * Updates LE_Timers[].  Called by timer interrupt so it has to be quick.
 */

static void LI_TIMERS_UpdateTIMERS (void)
{
  unsigned char i;

  for (i = 0; i < CE_ARTLIB_MaxTimersArray; i++)
  {
    if (LE_Timers[i] > 0)  // This user timer is still running.
    {
      if (--RemainingTimerTicks[i] == 0)  // Convert 60hz to timer speed.
      {
        RemainingTimerTicks[i] = LE_TimerSpeeds[i];
        if (--LE_Timers[i] == 0)  // One user timer tick here!
        {
          LE_Timers[i] = LE_TimerRestartCount[i];
#if CE_ARTLIB_EnableSystemUIMsg
          if (LE_TimerSendUIMessage[i])
            LE_UIMSG_SendEvent (UIMSG_TIMER_REACHED_ZERO,
            i, LE_TIME_TickCount, 0, 0, 0, NULL, 0);
#endif // CE_ARTLIB_EnableSystemUIMsg
        }
      }
    }
  }
}



/*****************************************************************************
 * Cyclic functions, called once per update cycle by either the timer's
 * cyclic update thread or by the single tasking main loop.
 */

void LI_TIMER_CallCyclicFunctions (void)
{
#if CE_ARTLIB_EnableSystemSequencer
    LI_SEQNCR_TimerTick ();
#endif

#if CE_ARTLIB_EnableSystem3DSequencer
    LI_ANIM3D_TickScene();
#endif
}



#if CE_ARTLIB_EnableMultitasking
/*****************************************************************************
 * This is run by a separate thread in multitasking mode.  Every time the
 * cycle event goes off (or less often), a bunch of cyclic functions get
 * called, one after the other.  These functions are usually for updating
 * the screen and doing other animation related things.  If they take more
 * than one tick to run, that's ok too, since they won't slow anyone else
 * down except themselves, and can get the real time from the tick counter,
 * which is separate from the cycle thread.  Returns a thread return code.
 * Exits when a global flag is set during timer module shutdown.
 */

unsigned __stdcall LI_TIMER_CycleFunction (void *ArgumentPntr)
{
  while (!TimerSystemIsClosingDown)
  {
    // Do the various cyclic things.

    LI_TIMER_CallCyclicFunctions ();

    // Sleep until the next tick wakes us up.  The wait is at
    // the end of the while so that it exits more quickly
    // (tests flag after waking up).

    WaitForSingleObject (hTimerCycleTriggerEvent, INFINITE);
  }

  return TimerSystemIsClosingDown; // Thread exit code.
}
#endif // CE_ARTLIB_EnableMultitasking



/*****************************************************************************
 * Similar to the cycle thread, this function makes the calling thread go
 * to sleep for at least the given number of ticks (may wake up late).
 * The delay aborts when the timer module shuts down.  Returns the number
 * of ticks it actually delayed.
 */

TYPE_Tick LE_TIMER_Delay (TYPE_Tick DelayTickCount)
{
  TYPE_Tick EndTime;
  TYPE_Tick StartTime;

  // It is a bug to delay in a single tasking game while suspended, the
  // whole game will stop and get stuck here since it needs to process
  // messages (for the alt-tab related application activated message) to
  // get unsuspended and it will never do that while waiting.
  // Multitasking games have a separate thread to process the messages,
  // so it is safe for the game code to delay while time is suspended.

#if !CE_ARTLIB_EnableMultitasking
  if (TimersSuspended)
  {
    #if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer,
      "LE_TIMER_Delay: Delay called while the timers are suspended, which would "
      "cause this single tasking game to hang.  Don't do it!  Have a look at "
      "LE_MAIN_ApplicationActive, which is related to suspending timers, or "
      "make this a multitasking game.\r\n");
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
    #endif

    return 0;
  }
#endif

  StartTime = LE_TIME_TickCount;
  EndTime = StartTime + DelayTickCount;

  while (!TimerSystemIsClosingDown)
  {
    if (LE_TIME_TickCount >= EndTime)
      break;

    WaitForSingleObject (hTimerCycleTriggerEvent, INFINITE);
  }

  return LE_TIME_TickCount - StartTime;
}



/*****************************************************************************
 * Called whenever a timer "TICK" occurs.  Used to process timers and
 * indirectly trigger periodic functions.  For extra accuracy this could
 * be put in a 16 bit DLL, but then it wouldn't be able to do much since
 * it would be called directly by the timer interrupt.  Leaving it as a 32 bit
 * Win95 callback lets you do more, but has timing inaccuracies because
 * the timer runs in a thread.
 */

void CALLBACK LI_TIMERS_TimerCallBack (UINT idTimer, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    if (MinTimerRes == 0 || TimersSuspended)
        return; // Hey, the system is supposed to be turned off now!

    LE_TIME_TickCount++;  // Update global elapsed time clock.

    // Do cyclic updates first (animation engine), since the user timers
    // may take varying amounts of time depending on how blocked the message
    // queue is.

    PulseEvent (hTimerCycleTriggerEvent); // Trigger another update cycle.

    LI_TIMERS_UpdateTIMERS ();  // Update user timers.
}



/*****************************************************************************
 * TRUE makes the timers stop running, FALSE turns them back on.  This is
 * useful for suspending the game while the user is looking at some other
 * program in Windows.
 */

void LE_TIMERS_Suspend (BOOL SuspendIt)
{
  MMRESULT retval;
  MMRESULT TempID;

  TimersSuspended = (SuspendIt != 0);

  if (TimersSuspended)
  {
    if (TimerID != 0)
    {
      // Stop calling our timer callback function, please!
      // Also do a quick variable swap in case of multiple
      // calls from other threads to suspend timers (could use
      // a critical section but it isn't likely to happen).

      TempID = TimerID;
      TimerID = 0;
      retval = timeKillEvent (TempID);

  #if CE_ARTLIB_EnableDebugMode
      if (retval != TIMERR_NOERROR)
        LE_ERROR_DebugMsg ("LE_TIMERS_Suspend: timeKillEvent failed",
        LE_ERROR_DebugMsgToFileAndScreen);
  #endif
    }
  }
  else // Should be running now, if system is open.
  {
    if (MinTimerRes > 0 /* System open? */ && TimerID == 0)
    {
      TempID = timeSetEvent (
        1000 / CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ, /* Delay in milliseconds */
        MinTimerRes, /* Timer resolution */
        LI_TIMERS_TimerCallBack, /* Callback function */
        0, /* User value - unused */
        TIME_PERIODIC /* Free running timer */);
      if (TempID == 0)
        LE_ERROR_ErrorMsg ("LE_TIMERS_Suspend: Unable to allocate a timer.");

      if (TimerID == 0)
        TimerID = TempID;
      else // Someone else beat us to unsuspending the timer.
        retval = timeKillEvent (TempID);
    }
  }
}



/*****************************************************************************
 * Special atexit callback function to shut down the timer when this program
 * exits due to an error of some sort.
 */

void __cdecl LI_TIMERS_CleanUp (void)
{
  LI_TIMERS_RemoveSystem ();
}



/*************************************************************
*
* void LI_TIMERS_InitSystem(void);
*
*   Description:
*       TIMERS system initializion procedure.
*
*************************************************************/
void LI_TIMERS_InitSystem(void)
{
  unsigned int  CycleThreadID = 0;
  int           i;
  TIMECAPS      TimeCaps;

  if (MinTimerRes > 0)
    return; /* Timer is already initialised. */

  // If the program ends due to an error message display, close the
  // timer stuff.  This also works for partially opened timer stuff.
  // The exit function is safe to call multiple times, so registering
  // it multiple times won't cause any problems other than wasted
  // memory space.  Note that leaving timers partially running after
  // a crash can cause Windows slowdown and other odd problems.

  atexit (LI_TIMERS_CleanUp);

  /* Set the default user timers speeds to 10hz. */

  for (i = 0; i < CE_ARTLIB_MaxTimersArray; i++)
  {
    LE_Timers[i] = 0;
    LE_TimerSpeeds[i] = CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ / 10;
    LE_TimerRestartCount[i] = 0;
    LE_TimerSendUIMessage[i] = FALSE;
    RemainingTimerTicks[i] = 1; // So timers time out right away.
  }

  // Allocate an event to let the timing cycle thread (and threads
  // using the delay function) run separately from the timer interrupt,
  // so that they can run slower than the interrupt (missing ticks)
  // and yet the essential time values are accurate.  There's also
  // that Windows 95 weirdness where multimedia timer calls stack
  // up in the kernel and eventually slow the system down to a crash
  // if too many pile up.

  hTimerCycleTriggerEvent = CreateEvent (NULL /* Security */,
    TRUE /* Manual Reset */, FALSE /* Initial State */, NULL /* Name */);
  if (hTimerCycleTriggerEvent == NULL)
  {
    LE_ERROR_ErrorMsg ("LI_TIMERS_InitSystem: Unable to allocate cycle event.\r\n");
    return; // Normally it won't return, but...
  }

  /* Start getting the Windows multimedia timer working. */

  if (timeGetDevCaps(&TimeCaps, sizeof(TIMECAPS)) != TIMERR_NOERROR)
  {
    LE_ERROR_ErrorMsg("LI_TIMERS_InitSystem: timeGetDevCaps() failed.");
  }

  // see if our desired resolution is outside the timer's range.  We want
  // a resolution less than the timing rate.

  MinTimerRes = (1000 /* Millisecond units */ / CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ) / 4;

  if (TimeCaps.wPeriodMin > MinTimerRes)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer,
      "LI_TIMERS_InitSystem: Desired timer resolution too small: Min = %dms Desired = %dms",
      TimeCaps.wPeriodMin, MinTimerRes);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    MinTimerRes = TimeCaps.wPeriodMin;
  }
  else if (TimeCaps.wPeriodMax < MinTimerRes)
  {
#if CE_ARTLIB_EnableDebugMode
    sprintf (LE_ERROR_DebugMessageBuffer,
      "LI_TIMERS_InitSystem: Desired timer resolution too large: Max = %dms Desired = %dms",
      TimeCaps.wPeriodMax, MinTimerRes);
    LE_ERROR_DebugMsg (LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
#endif
    MinTimerRes = TimeCaps.wPeriodMax;
  }
  if (MinTimerRes == 0)
    MinTimerRes = 1; /* We use zero to mark the system as closed. */

#if LI_TIMERS_USE_TIME_BEGIN
  if (timeBeginPeriod (MinTimerRes) != TIMERR_NOERROR)
  {
    MinTimerRes = 0;  /* Well, the system is closed. */
    LE_ERROR_ErrorMsg ("LI_TIMERS_InitSystem: Unable to init timer period.");
  }
#endif // LI_TIMERS_USE_TIME_BEGIN

  TimerID = timeSetEvent (
    1000 / CE_ARTLIB_TIME_BASIC_CLOCK_RATE_HZ, /* Delay in milliseconds */
    MinTimerRes, /* Timer resolution */
    LI_TIMERS_TimerCallBack, /* Callback function */
    0, /* User value - unused */
    TIME_PERIODIC /* Free running timer */);

  if (TimerID == 0)
    LE_ERROR_ErrorMsg("LI_TIMERS_InitSystem: Unable to allocate a timer.");

  TimersSuspended = FALSE;
  TimerSystemIsClosingDown = FALSE;

  // Now start up the cycle thread, which runs the animation engines and
  // other cyclic things.

#if CE_ARTLIB_EnableMultitasking
  hTimerCycleThread = (HANDLE) _beginthreadex (NULL /* Security */,
    0 /* Stack size, zero means parent size, can grow */,
    LI_TIMER_CycleFunction /* Thread function */,
    NULL /* Arguments */, 0 /* Flags */, &CycleThreadID);
  if (hTimerCycleThread == NULL)
    LE_ERROR_ErrorMsg ("LI_TIMERS_InitSystem: Unable to create cycle thread.\r\n");

  SetThreadPriority (hTimerCycleThread, CE_ARTLIB_GraficsThreadPriority);
#endif // CE_ARTLIB_EnableMultitasking
}
/************************************************************/

/*************************************************************
*
* void LI_TIMERS_RemoveSystem(void);
*
*   Description:
*       TIMERS system cleanup and removal procedure.
*
*************************************************************/
void LI_TIMERS_RemoveSystem(void)
{
  MMRESULT retval;

  if (TimerID != 0)
  {
    retval = timeKillEvent (TimerID); // get rid of timer event, makes it stop calling our function.

#if CE_ARTLIB_EnableDebugMode
    if (retval != TIMERR_NOERROR)
      LE_ERROR_DebugMsg("LI_TIMERS_RemoveSystem: timeKillEvent failed", LE_ERROR_DebugMsgToFile);
#endif
    TimerID = 0;
  }

  if (MinTimerRes > 0)
  {
#if LI_TIMERS_USE_TIME_BEGIN
    retval = timeEndPeriod (MinTimerRes); // Shut down the timer system.
    #if CE_ARTLIB_EnableDebugMode
    if (retval != TIMERR_NOERROR)
      LE_ERROR_DebugMsg("LI_TIMERS_RemoveSystem: timeEndPeriod failed", LE_ERROR_DebugMsgToFile);
    #endif
#endif // LI_TIMERS_USE_TIME_BEGIN
    MinTimerRes = 0; // Mark Windows timer as being closed.
  }

  // Suggest that the waiting threads stop waiting.

  TimerSystemIsClosingDown = TRUE;

  // Make threads waiting in the delay function exit now.

  if (hTimerCycleTriggerEvent != NULL)
    SetEvent (hTimerCycleTriggerEvent);

  if (hTimerCycleTriggerEvent != NULL)
  {
    CloseHandle (hTimerCycleTriggerEvent);
    hTimerCycleTriggerEvent = NULL;
  }

#if CE_ARTLIB_EnableMultitasking
  if (hTimerCycleThread != NULL)
  {
    CloseHandle (hTimerCycleThread); // Doesn't actually stop it.
    hTimerCycleThread = NULL;
  }
#endif // CE_ARTLIB_EnableMultitasking
}
/************************************************************/

#endif // if CE_ARTLIB_EnableSystemTimers
