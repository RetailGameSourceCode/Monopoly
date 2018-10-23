/******************************************************************************
 * Debugging routines and things.  Use "#define DEBUGART 1" to turn on
 * debugging code.
 *
 * $Header: /Projects - 1999/Mono 2 Hotseat/Debugart.cpp 1     1/08/99 2:06p Russellk $
 *
 * $Log: /Projects - 1999/Mono 2 Hotseat/Debugart.cpp $
 * 
 * 1     1/08/99 2:06p Russellk
 * 
 * 1     1/08/99 11:39a Russellk
 * 
 * 1     1/08/99 11:36a Russellk
 * Graphics
 * 
 * 2     12/17/98 4:26p Russellk
 * Obsolete comments removed for new version.  Log file facility entered
 * in Mess.c to record phase and action/error message activity.
 * 
*/

// RK - Warning: code written for hotseat but required for all - rewrite it as multi-purpose.
#include "gameinc.h"

#include <String.h>

#if FORMACINTOSH
  #include "Resource.h"
#endif



#if FORMACINTOSH
/******************************************************************************
 * Set up the parameterized message strings.
 */

static void SetUpMessageParameters (char *BoxTitle, char *Message)
{
   Str255   PascalMessage0;
   Str255   PascalMessage1;

   if (BoxTitle == NULL)
      CToPascalString ("Error:", PascalMessage0);
   else
      CToPascalString (BoxTitle, PascalMessage0);

   if (Message == NULL)
      PascalMessage1 [0] = 0;
   else
      CToPascalString (Message, PascalMessage1);

   ParamText (PascalMessage0, PascalMessage1, "\p", "\p");
}
#endif



/******************************************************************************
 * Just displays the message in a titled box and returns.  Either
 * part of the message can be NULL if you don't want to display it
 * (the title defaults to "Error" in that case).  Halts other
 * processing while the box is up.
 */

void DisplayMessage (char *BoxTitle, char *Message)
{
#if FORWINDOWS
   MessageBox (NULL, Message, BoxTitle,
   MB_ICONINFORMATION | MB_OK | MB_SYSTEMMODAL
   #if FORWIN95
   | MB_SETFOREGROUND
   #endif
   );
#else
   SetUpMessageParameters (BoxTitle, Message);
   SetArrowCursor ();
   Alert (rErrorMessageAlert, NULL);
#endif
}



/******************************************************************************
 * Displays the message like DisplayMessage but with a Retry button and
 * a Cancel button.  Returns TRUE if retry was picked, FALSE for cancel.
 * Allows other applications to run and message processing to continue.
 */

BOOL RetryMessage (char *BoxTitle, char *Message)
{
#if FORWINDOWS
   return IDRETRY == MessageBox (NULL, Message, BoxTitle,
   MB_ICONEXCLAMATION | MB_RETRYCANCEL | MB_TASKMODAL
   #if FORWIN95
   | MB_SETFOREGROUND
   #endif
   );
#else
   SetUpMessageParameters (BoxTitle, Message);
   SetArrowCursor ();
   return bRCARetryButton == Alert (rRetryCancelAlert, NULL);
#endif
}



/******************************************************************************
 * Displays the message like DisplayMessage but with an OK button and
 * a Cancel button.  Returns TRUE if OK was picked, FALSE for cancel.
 * Allows other applications to run and message processing to continue.
 */

BOOL OKCancelMessage (char *BoxTitle, char *Message)
{
#if FORWINDOWS
   return IDOK == MessageBox (NULL, Message, BoxTitle,
   MB_ICONQUESTION | MB_OKCANCEL | MB_TASKMODAL
   #if FORWIN95
   | MB_SETFOREGROUND
   #endif
   );
#else
   SetUpMessageParameters (BoxTitle, Message);
   SetArrowCursor ();
   return bRCAOKButton == Alert (rOKCancelAlert, NULL);
#endif
}


/******************************************************************************
 * Displays the message like DisplayMessage but with an Yes button and
 * a No button.  Returns TRUE if Yes was picked, FALSE for No.  If DefaultYes
 * is TRUE then the Yes button is the default, otherwise the No button is.
 * Allows other applications to run and message processing to continue.
 */

BOOL YesNoMessage (char *BoxTitle, char *Message, BOOL DefaultYes)
{
#if FORWINDOWS
   return IDYES == MessageBox (NULL, Message, BoxTitle,
   MB_ICONQUESTION | MB_YESNO | MB_TASKMODAL |
   #if FORWIN95
   MB_SETFOREGROUND |
   #endif
   (DefaultYes ? MB_DEFBUTTON1 : MB_DEFBUTTON2));
#else
   SetUpMessageParameters (BoxTitle, Message);
   SetArrowCursor ();
   return bRCAYesButton == Alert (rYesNoAlert, NULL);
#endif
}

/******************************************************************************
 * Displays the given message and exits from the program abruptly.
 */

void ErrorExit (char *ErrorMessage)
{
   DisplayMessage ("Something went wrong!  Error Exit Message:", ErrorMessage);

#if FORWINDOWS
   exit (20);
#else
   ExitToShell ();
#endif
}

