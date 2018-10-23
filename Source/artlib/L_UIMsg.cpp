/*****************************************************************************
 *
 *   FILE:           L_UIMsg.c
 *   DESCRIPTION:    Message queue used for user interface events.
 *
 *   (C) Copyright 1998 Artech Digital Entertainments.
 *                      All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_UIMsg.cpp 4     26/09/99 3:18p Agmsmith $
 *
 * Wrapper around a special queue used for user interface messages.
 *
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_UIMsg.cpp $
 * 
 * 4     26/09/99 3:18p Agmsmith
 * Added queue size reporting function.
 *
 * 3     9/22/98 3:11p Agmsmith
 * Now compiles with ArtLib99.
 *
 * 2     9/15/98 1:31p Agmsmith
 * Added system define to disable it.
 ****************************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemUIMsg


/************************************************************/
/* PRIVATE DECLARATIONS                                     */
/*----------------------------------------------------------*/

static LE_QUEUE_Record EventQueue;



/*****************************************************************************
 *
 * int  LE_UIMSG_InitializeSystem( void )
 * void LE_UIMSG_RemoveSystem( void )
 *
 *  Description:
 *      Routines for initializing and removing the event queue system. This
 *      just initializes the queue.
 *
 *  Parameters:
 *    None.
 *
 *  Returns:
 *    TRUE if successful.
 *
 *****************************************************************************/

int LE_UIMSG_InitializeSystem (void)
{
  return LE_QUEUE_Initialize (&EventQueue, CE_ARTLIB_UIMsgQueueSize);
}

void LE_UIMSG_RemoveSystem (void)
{
  LE_QUEUE_Cleanup (&EventQueue);
}



/*****************************************************************************
 *
 *  Description:
 *      Place an event given by the parameters on the event queue. The parameters
 *      are similar to those that are part of a LE_QUEUE_MessageRecord structure.
 *      However, some unimportant events are thrown away if the queue is getting
 *      too full (so that you don't lose the important ones).
 *
 *  Parameters:
 *    event             - The type of event.
 *    numberA - numberE - Numerical parameters. Depends on type of event.
 *    stringA           - Pointer to start of optional string or data.
 *    binarySize        - Length of data given in stringA. If stringA is
 *                        not NULL and binarySize is 0, the data is interpreted
 *                        as a string. If binarySize > 0, the data is
 *                        interpreted as binary data.
 *
 *  Returns:
 *      TRUE if the event was placed on the event queue, FALSE if not, usually
 *      because the queue is already full.
 *
 *****************************************************************************/

int LE_UIMSG_SendEvent (LE_UIMSG_MessageType event,
  long            numberA,
  long            numberB,
  long            numberC,
  long            numberD,
  long            numberE,
  ACHAR          *stringA,
  unsigned short  binarySize)
{

  /* Is this an unimportant event?  Timer events from timers that are free
     running (not one-shot) can be safely thrown away.  Same thing for mouse
     tracking events. */

  if ((event == UIMSG_TIMER_REACHED_ZERO && LE_TimerRestartCount [numberA] != 0) ||
  (event == UIMSG_BUTTON_TRACK) || (event == UIMSG_MOUSE_MOVED))
  {
    if (LE_QUEUE_CurrentSize (&EventQueue) > (CE_ARTLIB_UIMsgQueueSize / 2))
      return FALSE;  /* Queue is more than half full.  Ignore message. */
  }

  // Place the new event on the event queue.
  return LE_QUEUE_EnqueueWithParameters (
    &EventQueue,
    (unsigned char) event,
    0, 0, /* No From or To player. */
    numberA,
    numberB,
    numberC,
    numberD,
    numberE,
    stringA,
    binarySize);
}



/*****************************************************************************
 *
 * int LE_UIMSG_ReceiveEvent( LE_QUEUE_MessagePointer message )
 *
 *  Description:
 *      Get the next event from the event queue. Return 0 if no event is
 *      available. This just calls the LE_QUEUE_Dequeue operation on the
 *      EventQueue and places the message in the supplied buffer.
 *
 *  Parameters:
 *      message     - Pointer to buffer to receive message.
 *
 *  Returns:
 *      TRUE if a message was received, FALSE if none available.
 *
 *****************************************************************************/

int LE_UIMSG_ReceiveEvent( LE_QUEUE_MessagePointer message )
{
  return( LE_QUEUE_Dequeue( &EventQueue, message ) );
}



/*****************************************************************************
 *
 * int LE_UIMSG_WaitEvent( LE_QUEUE_MessagePointer message )
 *
 *  Description:
 *      Get the next event from the event queue, doesn't return until
 *      one is available.
 *
 *  Parameters:
 *      message     - Pointer to buffer to receive message.
 *
 *****************************************************************************/

void LE_UIMSG_WaitEvent( LE_QUEUE_MessagePointer message )
{
  LE_QUEUE_WaitForMessage (&EventQueue, message);
}



/*****************************************************************************
 *
 * void LE_UIMSG_FlushEvents (void)
 *
 *  Description:
 *      Removes all events from the message queue.  Useful if something
 *      happened in the game and you don't want to see events that happened
 *      recently after it (like timeouts that got queued after the button
 *      press).
 *
 *  Parameters:
 *      None.
 *
 *****************************************************************************/

void LE_UIMSG_FlushEvents (void)
{
  LE_QUEUE_Flush (&EventQueue);
}



/*****************************************************************************
 *
 * int LE_UIMSG_PercentageFull (void)
 *
 *  Description:
 *      Returns the current queue size, 0 means empty, 100 means full and values
 *      inbetween mean a partially full queue.
 *
 *  Parameters:
 *      None.
 *
 *****************************************************************************/

int LE_UIMSG_PercentageFull (void)
{
  return (LE_QUEUE_CurrentSize (&EventQueue) * 100 / CE_ARTLIB_UIMsgQueueSize);
}
#endif // CE_ARTLIB_EnableSystemUIMsg
