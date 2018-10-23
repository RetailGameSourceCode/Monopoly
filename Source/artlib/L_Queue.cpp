/*****************************************************************************
 *
 *   FILE:           L_Queue.cpp
 *   DESCRIPTION:    A multitasking safe message queue data structure.
 *
 *   (C) Copyright 1998 Artech Digital Entertainments.
 *                      All rights reserved.
 *
 * $Header: /Artlib_99/ArtLib/L_Queue.cpp 5     12/27/98 2:20p Agmsmith $
 *
 * Implementation of a queue system. Modified from the original MESS system
 * used in Monopoly.
 *
 *****************************************************************************
 * $Log: /Artlib_99/ArtLib/L_Queue.cpp $
 * 
 * 5     12/27/98 2:20p Agmsmith
 * Don't clean up uninitialised queues.
 * 
 * 4     10/29/98 6:36p Agmsmith
 * New improved memory and data system added.  Memory and data systems
 * have been separated.  Now have memory pools, and optional corruption
 * check.  The data system has data groups rather than files and
 * individual items can come from any data source. Also has a least
 * recently used data unloading system (rather than programmer set
 * priorities).  And it can all be turned off for AndrewX!
 * 
 * 3     9/22/98 2:22p Agmsmith
 * Updated for ArtLib99.
 * 
 * 2     9/15/98 1:31p Agmsmith
 * Added system define to disable it.
 ****************************************************************************/



/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"

#if CE_ARTLIB_EnableSystemQueue

/************************************************************/
/* EXPORTED GLOBALS                                         */
/************************************************************/


/****************************************************************************
 * PRIVATE GLOBALS, TYPEDEFS and MACROS                                     *
 ****************************************************************************/

#define LI_QUEUE_MAKEPLAYERS(from,to) ((UNS8) ((from << 4 ) | (to & 0xF)))



/*****************************************************************************
 * Deallocates any storage associated with the message.  Use this after you
 * have finished with a message you got from LE_QUEUE_Dequeue.
 */

void LE_QUEUE_FreeStorageAssociatedWithMessage (LE_QUEUE_MessagePointer MessagePntr)
{
  if (MessagePntr->stringA != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (MessagePntr->stringA);
    MessagePntr->stringA = NULL;
    MessagePntr->stringASize = 0;
  }
}



/*****************************************************************************
 * Attempt to add the item to the top of the queue.  Returns FALSE if the
 * queue is full (and cleans up the binary attachment if there is one).
 */

BOOL LE_QUEUE_Enqueue (LE_QUEUE_Pointer Queue, LE_QUEUE_MessagePointer Item)
{
  int NewIndex;

  if (Queue == NULL || Queue->messages == NULL)
  {
    LE_QUEUE_FreeStorageAssociatedWithMessage (Item);
    return FALSE;
  }

#if CE_ARTLIB_PlatformWin95
  /* Wait until we have access to the queue. */
  EnterCriticalSection (&Queue->accessMutex);
#endif

  if (Queue->head < 0)
  {
    /* Adding to an empty queue. */
    Queue->tail = Queue->head = 0;
#if CE_ARTLIB_PlatformWin95
    // Queue now contains a message.  Signal the message-is-ready event,
    // which will stay signaled until reset (done when queue is empty).
    SetEvent (Queue->messageEvent);
#endif
  }
  else
  {
    NewIndex = Queue->head + 1;
    if (NewIndex >= Queue->queueSize)
      NewIndex = 0;
    if (NewIndex == Queue->tail) /* Is Queue full? */
    {
      /* Deallocate attached blob. */
      LE_QUEUE_FreeStorageAssociatedWithMessage (Item);
#if CE_ARTLIB_PlatformWin95
      LeaveCriticalSection (&Queue->accessMutex);
#endif
      return FALSE;
    }
    Queue->head = NewIndex;
  }
  memcpy (Queue->messages + Queue->head, Item, sizeof (LE_QUEUE_MessageRecord));

  Item->stringA = NULL;  /* It's in the queue now, not in Item. */
  Item->stringASize = 0;

#if CE_ARTLIB_PlatformWin95
  // All done with the queue, release it.
  LeaveCriticalSection (&Queue->accessMutex);
#endif

  return TRUE;
}



/*****************************************************************************
 * Attempt to remove an item from the bottom of the queue.  Returns FALSE if
 * the queue is empty.  Otherwise fills in the Item's fields.
 */

BOOL LE_QUEUE_Dequeue (LE_QUEUE_Pointer Queue, LE_QUEUE_MessagePointer Item)
{
  int NewIndex;

  if (Queue == NULL || Queue->messages == NULL)
    return FALSE;

#if CE_ARTLIB_PlatformWin95
  // Wait until we have access to the queue.
  EnterCriticalSection (&Queue->accessMutex);
#endif

  if (Queue->head < 0)
  {
#if CE_ARTLIB_PlatformWin95
    LeaveCriticalSection (&Queue->accessMutex);
#endif
    return FALSE; /* Empty queue. */
  }

  memcpy (Item, Queue->messages + Queue->tail, sizeof (LE_QUEUE_MessageRecord));

  /* Update queue pointers. */

  if (Queue->head == Queue->tail)
  {
    Queue->head = -1; /* Queue is now empty. */
#if CE_ARTLIB_PlatformWin95
    // Make other programs trying to wait for a message
    // go to sleep while they wait.
    ResetEvent (Queue->messageEvent);
#endif
  }
  else
  {
    NewIndex = Queue->tail + 1;
    if (NewIndex >= Queue->queueSize)
      NewIndex = 0;
    Queue->tail = NewIndex;
  }

#if CE_ARTLIB_PlatformWin95
  // All done with the queue, release it.
  LeaveCriticalSection (&Queue->accessMutex);
#endif

  return TRUE;
}



/******************************************************************************
 * This function will wait indefinately until a message is available on the
 * queue.  When a message becomes available, it is removed from the queue and
 * placed in the supplied buffer.  Returns FALSE if it fails because the
 * queue isn't initialised or has just been deallocated.
 */

BOOL LE_QUEUE_WaitForMessage (LE_QUEUE_Pointer Queue, LE_QUEUE_MessagePointer Item)
{
  BOOL MessageReceived = FALSE;

  if (Queue == NULL || Queue->messages == NULL)
    return FALSE;

  // Continue waiting until we get a message from the queue.

  while (!MessageReceived)
  {
#if CE_ARTLIB_PlatformWin95
    // Wait until a message may be available on the queue.
    WaitForSingleObject (Queue->messageEvent, INFINITE);

    // Make sure that the queue still exists.
    if (Queue->messages == NULL)
      break;
#endif

    // Get message from the queue. If none are available because
    // someone sneaked in between calls, keep waiting.
    MessageReceived = LE_QUEUE_Dequeue (Queue, Item);
  }

  return MessageReceived;
}



/*****************************************************************************
 * Utility function for telling the players about some action.  The
 * arguments correspond to the fields in an LE_QUEUE_MessageRecord (BinarySize
 * is size of stringA in bytes, zero to use string length instead).
 */

BOOL LE_QUEUE_EnqueueWithParameters (LE_QUEUE_Pointer Queue, unsigned char Message,
unsigned char sender, unsigned char receiver, long NumberA, long NumberB,
long NumberC, long NumberD, long NumberE, ACHAR *StringA, unsigned short BinarySize)
{
  LE_QUEUE_MessageRecord  NewAction;
  BOOL                    Success;

  memset (&NewAction, 0, sizeof (NewAction));

  NewAction.messageCode = Message;
  NewAction.players = LI_QUEUE_MAKEPLAYERS (sender, receiver);
  NewAction.numberA = NumberA;
  NewAction.numberB = NumberB;
  NewAction.numberC = NumberC;
  NewAction.numberD = NumberD;
  NewAction.numberE = NumberE;

  /* Set the string field.  Either copy binary data, or copy a string. */

  if (StringA != NULL)
  {
    if (BinarySize == 0) /* Sending a string. */
      BinarySize = (unsigned short) AStrByteSize (StringA);

    /* Allocate space for the data. */

    NewAction.stringA = (ACHAR *) LI_MEMORY_AllocLibraryBlock (BinarySize);
    if (NewAction.stringA == NULL)
      return 0;

    /* Copy the data, binary or string. */

    memcpy (NewAction.stringA, StringA, BinarySize);

    /* Remember the size of the data for later. */

    NewAction.stringASize = BinarySize;
  }

  Success = LE_QUEUE_Enqueue (Queue, &NewAction);

  return Success;
}



/*****************************************************************************
 * Removes all messages from the queue (and frees their associated storage
 * if needed).  This is useful for clearing out the queue when you have picked
 * up some message (like a button press) and want to ignore messages queued
 * behind it (like timeouts).
 */

void LE_QUEUE_Flush (LE_QUEUE_Pointer QueuePntr)
{
  LE_QUEUE_MessagePointer MessagePntr;
  int                     NewIndex;

  if (QueuePntr == NULL || QueuePntr->messages == NULL)
    return;

#if CE_ARTLIB_PlatformWin95
  // Wait until we have access to the queue.
  EnterCriticalSection (&QueuePntr->accessMutex);
#endif

  if (QueuePntr->head >= 0)
  {
    /* Remove items from the queue - just go though all things
       and free associated storage from ones that have it. */

    while (QueuePntr->head != QueuePntr->tail)
    {
      MessagePntr = QueuePntr->messages + QueuePntr->tail;

      if (MessagePntr->stringA != NULL)
        LE_QUEUE_FreeStorageAssociatedWithMessage (MessagePntr);

      NewIndex = QueuePntr->tail + 1;
      if (NewIndex >= QueuePntr->queueSize)
        NewIndex = 0;
      QueuePntr->tail = NewIndex;
    }

    /* Remove the last remaining item in the queue. */

    LE_QUEUE_FreeStorageAssociatedWithMessage (QueuePntr->messages + QueuePntr->head);

    QueuePntr->head = -1; /* Queue is now empty. */
  }

#if CE_ARTLIB_PlatformWin95
  // Make other programs trying to wait for a message
  // go to sleep while they wait because the queue is empty.
  ResetEvent (QueuePntr->messageEvent);

  // All done with the queue, release it.
  LeaveCriticalSection (&QueuePntr->accessMutex);
#endif
}



/*****************************************************************************
 * Get the number of messages currently in the message queue.  This includes
 * both outgoing and incoming messages.
 */

int LE_QUEUE_CurrentSize (LE_QUEUE_Pointer Queue)
{
  int QueueSize;

  if (Queue == NULL || Queue->messages == NULL)
    return 0;

#if CE_ARTLIB_PlatformWin95
  EnterCriticalSection (&Queue->accessMutex);
#endif

  if (Queue->head < 0)
    QueueSize = 0; /* Empty queue. */
  else
  {
    QueueSize = Queue->head - Queue->tail + 1;
    if (Queue->head < Queue->tail)
      QueueSize += Queue->queueSize;
  }

#if CE_ARTLIB_PlatformWin95
  LeaveCriticalSection (&Queue->accessMutex);
#endif

  return QueueSize;
}



/*****************************************************************************
 * LE_QUEUE_Initialize - allocate space and set up mutual exclusion stuff.
 * Returns TRUE if successful, FALSE if it fails.  Presumably QueuePntr
 * points to a global variable in some other module.
 */

BOOL LE_QUEUE_Initialize (LE_QUEUE_Pointer QueuePntr, int SizeInMessages)
{
  /* Initialise all the message queue to empty. */

  memset (QueuePntr, 0, sizeof (LE_QUEUE_Record));
  QueuePntr->head = -1;

  // Allocate space for the messages.
  if (SizeInMessages < 1)
    goto ErrorExit; // Need to have at least one message in the queue.

  QueuePntr->messages = (LE_QUEUE_MessagePointer) LI_MEMORY_AllocLibraryBlock (SizeInMessages *
    sizeof (LE_QUEUE_MessageRecord));
  if (QueuePntr->messages == NULL)
    goto ErrorExit;

  QueuePntr->queueSize = SizeInMessages;

#if CE_ARTLIB_PlatformWin95
  // Create the event object used for processes waiting for a message.
  QueuePntr->messageEvent = CreateEvent (NULL /* Security */,
    TRUE /* Manual reset */, FALSE /* TRUE for initial state is signaled */,
    NULL /* Name */);
  if (QueuePntr->messageEvent == NULL)
    goto ErrorExit;

  // Initialise the critical section mutex.
  InitializeCriticalSection (&QueuePntr->accessMutex);
#endif

  return TRUE; // Success!

ErrorExit:
#if CE_ARTLIB_EnableDebugMode
  LE_ERROR_DebugMsg ("LE_QUEUE_Initialize: "
    "Unable to allocate enough of something for your queue.\r\n",
    LE_ERROR_DebugMsgToFileAndScreen);
#endif

#if CE_ARTLIB_PlatformWin95
  if (QueuePntr->messageEvent != NULL)
  {
    CloseHandle (QueuePntr->messageEvent);
    QueuePntr->messageEvent = NULL;
    DeleteCriticalSection (&QueuePntr->accessMutex);
  }
#endif

  if (QueuePntr->messages != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (QueuePntr->messages);
    QueuePntr->messages = NULL;
  }

  return FALSE;
}



/*****************************************************************************
 * LE_QUEUE_Cleanup - deallocate things allocated to make the queue.
 */

void LE_QUEUE_Cleanup (LE_QUEUE_Pointer QueuePntr)
{
  LE_QUEUE_MessageRecord  DeadMessage;
  HANDLE                  TempMessageEvent;

  if (QueuePntr == NULL || QueuePntr->messages == NULL)
    return; // Uninitialised.

#if CE_ARTLIB_PlatformWin95
  // First, wait until we have access to the queue.
  EnterCriticalSection (&QueuePntr->accessMutex);
#endif

  /* Remove all messages from the queues, and deallocate associated memory. */

  while (LE_QUEUE_Dequeue (QueuePntr, &DeadMessage))
    LE_QUEUE_FreeStorageAssociatedWithMessage (&DeadMessage);

  if (QueuePntr->messages != NULL)
  {
    LI_MEMORY_DeallocLibraryBlock (QueuePntr->messages);
    QueuePntr->messages = NULL;
  }

#if CE_ARTLIB_PlatformWin95
  if (QueuePntr->messageEvent != NULL)
  {
    // Destroy the mutex and the event handles.  Presumably any waiting
    // threads will wake up at this point.
    TempMessageEvent = QueuePntr->messageEvent;
    QueuePntr->messageEvent = NULL;
    CloseHandle (TempMessageEvent);
    DeleteCriticalSection (&QueuePntr->accessMutex);
  }
#endif
}

#endif // CE_ARTLIB_EnableSystemQueue
