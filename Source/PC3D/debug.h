#ifndef DEBUG_H
#define DEBUG_H
/*
**-----------------------------------------------------------------------------
**  File:       Debug.h
**  Purpose:    Sample Debug code
**  Notes:
**
**-----------------------------------------------------------------------------
*/

/*
**-----------------------------------------------------------------------------
**  Include files
**-----------------------------------------------------------------------------
*/

#include <signal.h>

#ifdef _DEBUG
  #if defined(_M_IX86)
    #define DbgBreak() __asm { int 3 }
  #else
    #define DbgBreak() DebugBreak()
  #endif // defined(_M_IX86)
#endif // _DEBUG


/*
**-----------------------------------------------------------------------------
**  Defines
**-----------------------------------------------------------------------------
*/

#ifdef _DEBUG
// Note:  Define DEBUG_PROMPTME if you want MessageBox Error prompting
//      This can get annoying quickly...
// #define DEBUG_PROMPTME

  // Pre and Post debug string info
  #define START_STR TEXT ("PC3D: ")
  #define END_STR   TEXT ("\r\n")
#endif // _DEBUG

// Debug Levels
#define DEBUG_ALWAYS    0L
#define DEBUG_CRITICAL  1L
#define DEBUG_ERROR     2L
#define DEBUG_MINOR     3L
#define DEBUG_WARN      4L
#define DEBUG_DETAILS   5L


// Sample Errors
#define APPERR_GENERIC        MAKE_DDHRESULT(10001)
#define APPERR_INVALIDPARAMS  MAKE_DDHRESULT(10002)
#define APPERR_NOTINITIALIZED MAKE_DDHRESULT(10003)
#define APPERR_OUTOFMEMORY    MAKE_DDHRESULT(10004)
#define APPERR_NOTFOUND       MAKE_DDHRESULT(10005)



/*
**-----------------------------------------------------------------------------
**  Macros
**-----------------------------------------------------------------------------
*/
#ifdef _DEBUG
  #define RECORD_3DSTATS

  #define DPF           dprintf
  #define DUMPCALLSTACK DumpStack

  #ifndef TRACE
  #define TRACE         PrintDebugString
  #endif // TRACE

  #ifndef ASSERT // this is to allow MFC's ASSERT if using MFC
  #define ASSERT(exp)                                                       \
    if (!(exp))                                                             \
    {                                                                       \
      switch(DEBUGAssert(#exp, __FILE__, (DWORD)__LINE__))                  \
      {                                                                     \
        case IDABORT:   raise(SIGABRT); _exit(3);                           \
        case IDRETRY:   DbgBreak();                                         \
        case IDIGNORE:  break;                                              \
        default:        abort();                                            \
      }                                                                     \
    } 
  #endif  // ASSERT

  #ifndef VERIFY
  #define VERIFY(exp)   ASSERT(exp)
  #endif // VERIFY

  #define REPORTERR(x)  ReportDDError((x), TEXT(__FILE__), (DWORD)__LINE__ );

  #define FATALERR(x)   ReportDDError((x), TEXT(__FILE__), (DWORD)__LINE__ ); \
                        OnPause(TRUE); \
                        DestroyWindow(g_hMainWindow)

  #define MSG(msg, category) \
    DPF(DEBUG_ERROR, TEXT("%s: %s\n"), TEXT(category), TEXT(msg))

  // .......... define to output message box stuff to file
  //  #define MSG(msg,category) MSGF(0,msg)

  #define MSGF(d,str) {\
    extern char debugtemp[100];\
    extern int debugStep;\
    extern FILE *Qfile;\
    Qfile = fopen("c:\\debug.txt","a"); \
    fputs( __FILE__, Qfile);  \
    fputs(" line#: ", Qfile); fputs(itoa( __LINE__,debugtemp,10), Qfile); \
    fputs(" step: ", Qfile); fputs(itoa( debugStep++,debugtemp,10), Qfile); \
    fputs(" debugval: ", Qfile); fputs(itoa(d, debugtemp,10), Qfile); \
    fputs(" comment: ", Qfile);  fputs(str, Qfile); \
    fputs(" \n", Qfile); fclose(Qfile); }


#else // _DEBUG

  #define REPORTERR(x)
  #define DPF           1 ? (void)0 : (void)
  #define DUMPCALLSTACK()

  #ifndef ASSERT
  #define ASSERT(exp)
  #endif // ASSERT

  #ifndef VERIFY
  #define VERIFY(exp)   exp
  #endif // VERIFY

  #ifndef TRACE
  #define TRACE         1 ? (void)0 : (void)
  #endif // TRACE

  #define FATALERR(x)   OnPause(TRUE); DestroyWindow(g_hMainWindow)

  #define MSG(msg,category)
  #define MSGF(d,str) 


#endif // _DEBUG


#define MSGBOX(msg, category)  \
  ::MessageBox(NULL, msg, category, MB_OK | MB_SYSTEMMODAL);  \
  MSG(msg,category)


// helpers to handle low level API errors  
//( BE SURE TO CHECK RETURN CODE FROM ALL D3D CALLS! )

#define REPORT_ON_ERROR(hResult)    if (FAILED(hResult)) REPORTERR(hResult)

#define QUIT_ON_ERROR(hResult)                                              \
  if (FAILED(hResult)) { REPORTERR(hResult); return; }

#define RETURN_ON_ERROR(hResult, retval)                                    \
  if (FAILED(hResult)) { REPORTERR(hResult); return retval; }

#define GOTO_ON_ERROR(hResult, label)                                       \
  if (FAILED(hResult)) { REPORTERR(hResult); goto label; }

#define RETURN_FALSE_ON_ERROR(hResult)        RETURN_ON_ERROR(hResult, FALSE)
#define CLEANUP_AND_RETURN_ON_ERROR(hResult)  GOTO_ON_ERROR(hResult, CLEANUP)
#define ATTEMPT(exp)  hResult = (exp); CLEANUP_AND_RETURN_ON_ERROR(hResult);


// Useful clean up macros

#define INL                       inline

#define TOLERANCE                 0.000001f
#define WITHINTOLERANCE(value)    (TOLERANCE < fabs(value))

#include "MemUtl.h"

// ===========================================================================
// The TBD ("to be done") macro below can be used after the #pragma 
// preprocessor directive to cause a message to be displayed in MSVC's output
// window when the source file using the macro is compiled. The message is
// formatted in exactly the same way as a compiler error or warning message 
// and can thus be double clicked on to cause the source file containing the
// macro usage to be opened and scrolled to the line where the usage is
// located. 
// Sample usage :
//
//    void SomeFunction()
//    {
//      ...
//      #pragma TBD("Extract info required for caption from INI file")
//      CString strCaption("Temporary Title");
//      ...
//    }
// ===========================================================================

#define ConvertToString(arg)  #arg
#define STRINGIT(arg)         ConvertToString(arg)
#define bldmsg(strMsg)        message(__FILE__ "(" STRINGIT(__LINE__) ") : " strMsg)
#define TBD(description)      bldmsg("TBD - " description)


/*
**-----------------------------------------------------------------------------
**  Global Variables
**-----------------------------------------------------------------------------
*/

// Debug Variables
#ifdef _DEBUG
  extern DWORD g_dwDebugLevel;
#endif

extern BOOL  g_fDebug;



/*
**-----------------------------------------------------------------------------
**  Function Prototypes
**-----------------------------------------------------------------------------
*/

// Debug Routines
#ifdef _DEBUG
  void __cdecl PrintDebugString(LPCSTR szFormat, ...);
  void __cdecl dprintf(DWORD dwDebugLevel, LPCTSTR szFormat, ...);
  void __stdcall DumpStack();
  int  _cdecl DEBUGAssert(LPSTR szExp, LPSTR szFile, DWORD dwLine);
#endif //_DEBUG

void _cdecl ReportDDError(HRESULT hResult, LPCTSTR szFile, DWORD dwLine);

#endif // End DEBUG_H
// ===========================================================================


