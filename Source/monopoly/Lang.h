#ifndef __LANG_H__
#define __LANG_H__
#include <stdlib.h>
/*************************************************************
*
*   FILE:             LANG.H
*
*   (C) Copyright 97 Artech Digital Entertainments.
*                    All rights reserved.
*
*   Monopoly Game Language Header.
*************************************************************/

extern short iLangId;

/************************************************************/
/* DEFINES                                                  */
/************************************************************/
// Renaming constants so I can conform to a more traditional naming convention.
#if defined( STANDALONE )
    #define NOTW_LANG_ENGLISH_US        1 // NOTW gets rid of conflict with windows names dragged in by inc.h
    #define NOTW_LANG_ENGLISH_UK        2
    #define NOTW_LANG_FRENCH            3
    #define NOTW_LANG_GERMAN            4
    #define NOTW_LANG_SPANISH           5
    #define NOTW_LANG_DUTCH             6
    #define NOTW_LANG_SWEDISH           7
    #define NOTW_LANG_FINNISH           8
    #define NOTW_LANG_DANISH            9
    #define NOTW_LANG_NORWEGIAN         10
    #define NOTW_LANG_MAX               11
    #define NOTW_LANG_NOT_INITIALIZED   0
    unsigned char   MDEF_CurrentLanguage = NOTW_LANG_NOT_INITIALIZED ; // So we don't need to link mdef.c
    #define MDEF_Monopoly_StarWars
#else
    #define NOTW_LANG_ENGLISH_US        MDEF_English_US         // 1
    #define NOTW_LANG_ENGLISH_UK        MDEF_English_UK         // 2
    #define NOTW_LANG_FRENCH            MDEF_French             // 3
    #define NOTW_LANG_GERMAN            MDEF_German             // 4
    #define NOTW_LANG_SPANISH           MDEF_Spanish            // 5
    #define NOTW_LANG_DUTCH             MDEF_Dutch              // 6
    #define NOTW_LANG_SWEDISH           MDEF_Swedish            // 7
    #define NOTW_LANG_FINNISH           MDEF_Finnish            // 8
    #define NOTW_LANG_DANISH            MDEF_Danish             // 9
    #define NOTW_LANG_NORWEGIAN         MDEF_Norwegian          // 10
    #define NOTW_LANG_MAX               MDEF_Max                // 11
    #define NOTW_LANG_NOT_INITIALIZED   MDEF_Unsupported        // 0
#endif

typedef unsigned long LANG_TextMessageNumber;
  /* The various text messages use this type to identify the messages.  The
  numbers don't have to be contiguous, so you can add more messages in the
  middle without having to renumber all your data files. */

/*
** Constant Message numbers now in a seperate include file
** reserved exclusively for message ids. (langids.h)
*/
//#include "LangIds.h" /* include this separately now */

/************************************************************/
/* PROTOTYPES                                               */
/************************************************************/
#if defined( __cplusplus )
  extern "C" {
#endif
wchar_t *LANG_GetTextMessage( LANG_TextMessageNumber MessageID);
/*
** Returns a pointer to the message string associated with MessageID
**    If for any reason it cannot, returns the default message string.
*/

wchar_t *LANG_GetTextMessage2( LANG_TextMessageNumber MessageID );
/*
** Returns a pointer to the message string associated with MessageID
**    If for any reason it cannot, returns NULL
**
*/

wchar_t *LANG_GetTextMessageClean ( LANG_TextMessageNumber MessageID);
/*
** Returns a pointer to the message string associated with MessageID
**    If for any reason it cannot, returns the default message string.
**    The string is modified to not contain line feeds and trailing
**    spaces and copied to a temporary buffer, which you get when
**    you call this function (so only up to 1024 byte / 512 wchar
**    strings available).
*/

long LANG_GetMaxMessageId( void );
/*
** Returns the highest message id
*/


void LANG_CleanAndRemoveSystem(void);
/*
- Completely cleans up and de-installs everything used
and setup from LANG_InitializeSystem.
- Note: This function call must be able to handle repeated
calls to itself appropriately.
*/

int LANG_InitializeSystem(void);
/*
// Language system initialize and cleanup routines. These
// are automatically used in the global initialization
// and cleanup functions found in INIT.C. These both
// must handle repeated calls to themselves.
*/
/*
- Determines the correct language text base to use according
to the current language settings.
- If this language matches one of our supported languages
or doesn't quite match but is similar then continue.
- Initialize the appropriate text base.
- If no errors set the variable MDEF_CurrentLanguage to the
appropriate supported language define.
- Note: This function call must be able to handle repeated
calls to itself appropriately.
returns 1 success, 0 failure.
*/

wchar_t *LANG_clmv(short american_dollar_value);
/*
// Convert to Local Monitary Value.
// Automatically converts from an american_dollar_value
// to the correct monitary value for the current language
// and game version. Returns a string containing monitary
// value and monitary currancy sign included as appropriate.
// Should be able to shove the string straight into the
// appropriate language print function.
*/

#if !FORHOTSEAT
//change this if you have more hyperlink in one message
#define LANG_MAX_HLINK 100

// Properties, information while displaying
typedef struct
{
    long        lang_id;
    long        align;
    long        style;
    long        font_size;
    long        y_space;
    long        color[2];
    long        font;
}LANG_PRINT_PROPERTY;

// Parameter definition for  some commands
enum
{
    LANG_Left       =0,
    LANG_Right      =1,
    LANG_Center     =2,
    LANG_Normal,
    LANG_Bold,
    LANG_Italic
};

void LANG_InitDisplayProperty(short langid);

void LANG_SetCurPrintProperty(long font, long font_size, long style, long align);

INT16 FigureOutLanguage(void);
void CleanupExternalLanguage(void);
int StartupExternalLanguage(short langid);
wchar_t *GetLanguageString(INT32 i);
wchar_t *LANG_MoneyToTmpIString(double value);
wchar_t *LANG_DoubleToTmpIString(double value);
wchar_t *LANG_LongToTmpIString(long value);
void LANG_OnLanguageSet(short id);
void LANG_Default(void);
void LANG_CopyPrintProperty(LANG_PRINT_PROPERTY *dest, LANG_PRINT_PROPERTY *src);
void LANG_SetPrintEnvironment(LANG_PRINT_PROPERTY *prty);
//void LANG_CpCmd(LANG_COMMAND *cmd, long action, long param1, long param2, long param3, wchar_t *param4);
BOOL LANG_Meta(wchar_t *str);

short LANG_GetStringHeight(wchar_t *wszString);
// Duplicates LE_FONTS_GetStringHeight using unicode string
// Returns the height in pixels of the string using the current font.

short LANG_GetStringWidth(wchar_t *wszString);
// Duplicates LE_FONTS_GetStringWidth using unicode string
// Returns the width in pixels of the string using the current font.

void LANG_Print(LE_DATA_DataId object_handle, short iX, short iY, DWORD iPrintColour, wchar_t *wszString);
// Duplicates LE_FONTS_Print using unicode string.

#endif /* !FORHOTSEAT */

#if defined( __cplusplus )
}
#endif
#endif /* __LANG_H__ */
