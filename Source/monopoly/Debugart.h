/******************************************************************************
 * Debugging routines and things.  Use "#define DEBUGART 1" to turn on
 * debugging code.
 *
 * $Header: /Projects - 1999/Mono 2 Hotseat/Debugart.h 1     1/08/99 2:06p Russellk $
 *
 * $Log: /Projects - 1999/Mono 2 Hotseat/Debugart.h $
 * 
 * 1     1/08/99 2:06p Russellk
 * 
 * 1     1/08/99 11:39a Russellk
 * 
 * 1     1/08/99 11:34a Russellk
 * Graphics
 * 
 * 2     12/17/98 4:26p Russellk
 * Obsolete comments removed for new version.  Log file facility entered
 * in Mess.c to record phase and action/error message activity.
 * 
 */

extern void DisplayMessage (char *BoxTitle, char *Message);
   /* Just displays the error message in a titled box and returns.  Either
   part of the message can be NULL if you don't want to display it.
   Halts other processing while the box is up. */

extern BOOL RetryMessage (char *BoxTitle, char *Message);
   /* Displays the message like DisplayMessage but with a Retry button and
   a Cancel button.  Returns TRUE if retry was picked, FALSE for cancel.
   Allows other applications to run and message processing to continue. */

extern BOOL OKCancelMessage (char *BoxTitle, char *Message);
   /* Displays the message like DisplayMessage but with an OK button and
   a Cancel button.  Returns TRUE if OK was picked, FALSE for cancel.
   Allows other applications to run and message processing to continue. */

extern BOOL YesNoMessage (char *BoxTitle, char *Message, BOOL DefaultYes);
   /* Displays the message like DisplayMessage but with an Yes button and
   a No button.  Returns TRUE if Yes was picked, FALSE for No.  If DefaultYes
   is TRUE then the Yes button is the default, otherwise the No button is.
   Allows other applications to run and message processing to continue. */

extern void ErrorExit (char *ErrorMessage);
   /* Displays the given message and exits from the program abruptly. */

