#ifndef __L_QUEUE_H__
#define __L_QUEUE_H__

/*****************************************************************************
 *
 *   FILE:           L_Queue.h
 *   DESCRIPTION:    A multitasking safe message queue data structure.
 *
 *   (C) Copyright 1998 Artech Digital Entertainments.
 *                      All rights reserved.
 *
 * $Header: /Artlib_99/LibraryCode/L_Queue.h 3     9/22/98 2:22p Agmsmith $
 *
 * Implementation of a queue system. Modified from the original MESS system
 * used in Monopoly.
 *
 ****************************************************************************/

/************************************************************/
/* EXPORTED GLOBALS                                         */
/************************************************************/


/************************************************************/
/* EXPORTED DATA STRUCTURES                                 */
/************************************************************/

typedef struct LE_QUEUE_MessageStruct
{
  INT32 numberA;
  INT32 numberB;
  INT32 numberC;
  INT32 numberD;
  INT32 numberE;
    /* Various numeric arguments, usage depends on the message.
    Five is enough for most commands, as determined by Star Wars
    Monopoly rules engine messages. */

  ACHAR *stringA;
    /* A string in local character set characters.  May also be used for binary
    objects.  NULL when not present.  Size of the object is in stringASize. */

  UNS16 stringASize;
    /* Number of bytes that stringA points to.  0 if none.  Includes NUL
    character at the end of strings. */

  UNS8 messageCode;
    /* The message code for this message.  The meaning of the code is defined
    by the user and no meaning is attached to this value by the queue system. */

  UNS8 players;
    /* Extra value used to store the player number of the sender of the message
    and the receiver.  From in high 4 bits, To in lower 4 bits.  Useful for
    multiplayer games over a network. */

} LE_QUEUE_MessageRecord, *LE_QUEUE_MessagePointer;
  /* Definition of a queue message. These are the actual items stored on
    the queue.  */


typedef struct LE_QUEUE_QueueStruct
{
  LE_QUEUE_MessagePointer messages;
    /* The actual message contents are stored in this dynamically
    allocated array (created when the queue is initialised with
    queueSize entries in it). */

  int queueSize;
    /* Number of elements in the messages array.  Set by the user
    when the queue is initialised. */

  int head;
    /* Points to top of queue (new items added after it), -1 if queue empty. */

  int tail;
    /* Points to item at bottom of queue (items removed).  Undefined when empty. */

#if CE_ARTLIB_PlatformWin95
  CRITICAL_SECTION accessMutex;
    /* For controlling access to the queue. Any operations that require
    access to the queue and its messages must first gain control of this
    critical section.  This synchronization is handled internally, so
    please don't touch this variable in user code. */

  HANDLE messageEvent;
    /* Handle to event object that remains signalled as long as there are
    messages available on the queue. When the queue becomes empty, the event
    is reset.  This is used in the QUEUE_WaitForMessage() routine.
    Synchronization is handled internally so the user must not use this
    variable. */
#endif

} LE_QUEUE_Record, *LE_QUEUE_Pointer;
  /* Definition of a queue.  Users can define their own queues by declaring
  a variable of type LE_QUEUE_Record and calling LE_QUEUE_Initialize. */



/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/

extern BOOL LE_QUEUE_Dequeue (LE_QUEUE_Pointer QueuePntr, LE_QUEUE_MessagePointer MessagePntr);
  /* Get the next message from a specified queue and place it in the buffer
  supplied.  A return value of 0 means no message was obtained from the queue,
  usually because the queue is empty. */

extern BOOL LE_QUEUE_WaitForMessage (LE_QUEUE_Pointer QueuePntr, LE_QUEUE_MessagePointer MessagePntr);
  /* Wait until a message becomes available on the given queue. If the queue is
  not empty, the next message will be removed from the queue and placed in the
  supplied buffer.  If the queue is empty, the routine will block the calling
  thread indefinately (no CPU time used) until a message is placed on the queue,
  at which time it will be removed and placed in the buffer.  Returns TRUE if
  successful, FALSE if the queue was deallocated while waiting. */

extern void LE_QUEUE_FreeStorageAssociatedWithMessage (LE_QUEUE_MessagePointer MessagePntr);
  /* Free any memory that has been allocated for a particular message (that
  stringA thing).  This routine should be called after a message has been
  dequeued and is no longer needed (you have finished processing it). */

extern BOOL LE_QUEUE_Enqueue (LE_QUEUE_Pointer QueuePntr, LE_QUEUE_MessagePointer MessagePntr);
  /* Place a new queue message on the specified queue.  The message record is
  defined by the user and is passed as a pointer.  It gets copied to internal
  queue storage, and the queue takes over responsibility for stringA's
  deallocation.  A FALSE return value means the message could not be placed on
  the queue, probably because the queue is full. */

extern BOOL LE_QUEUE_EnqueueWithParameters (LE_QUEUE_Pointer QueuePntr,
  unsigned char MessageCode,
  unsigned char FromPlayer, unsigned char ToPlayer,
  long NumberA, long NumberB, long NumberC, long NumberD, long NumberE,
  ACHAR *StringA, unsigned short BinarySize
);
  /* Place a new queue message on the specified queue.  The message is given
  in terms of the individual components contained in the message.  A return
  value of FALSE means the message was not placed on the queue, probably
  because it was full. */

extern void LE_QUEUE_Flush (LE_QUEUE_Pointer QueuePntr);
  /* Removes all messages from the queue (and frees their associated storage
  if needed).  This is useful for clearing out the queue when you have picked
  up some message (like a button press) and want to ignore messages queued
  behind it (like timeouts). */

extern int LE_QUEUE_CurrentSize (LE_QUEUE_Pointer QueuePntr);
  /* Return the current size of the queue as the number of messages stored. */

extern void LE_QUEUE_Cleanup (LE_QUEUE_Pointer QueuePntr);
  /* Routine to clean up a queue when it is no longer needed.  Removes all
  messages currently on the queue and frees the memory for each message
  and deallocates the space used for the queue array. */

extern BOOL LE_QUEUE_Initialize (LE_QUEUE_Pointer QueuePntr, int SizeInMessages);
  /* Routine to initialize a queue (you provide the storage for the
  LE_QUEUE_Record (usually as a global variable)). Call this routine once
  when the queue is first defined.  FALSE returned on failure.
  SizeInMessages specifies the maximum number of messages that the queue
  can hold (an internal array will be allocated with this size). */

/************************************************************/

#endif // __L_QUEUE_H__
