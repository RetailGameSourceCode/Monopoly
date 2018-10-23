/**f***********************************************************
*
*   FILE:             LANG.C
*
*   (C) Copyright 97  Artech Digital Entertainments.
*                     All rights reserved.
*************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/************************************************************/

#include "gameinc.h"
#include "basictyp.h"
#include "lang.h"
#include <locale.h>

/************************************************************/
/* Misc                                                     */
/************************************************************/
#define CLEAN_BUFFER_SIZE  512
short   iLangId = NOTW_LANG_NOT_INITIALIZED;
static  wchar_t CleanMessageBuffer[CLEAN_BUFFER_SIZE];
static  INT32   iMaxIndexEntry;   // Highest Index Entry
LE_DATA_DataId datafile = LED_EI;  // Text data file
LE_DATA_DataId datafile2 = LED_EI; // Graphics data file
LE_DATA_DataId datafile3 = LED_EI; // Dialog data file
NUMBERFMT LANG_LocaleNumberFormat;

/************************************************************/

/************************************************************/
/* LANG_GetMaxMessageId                                     */
/************************************************************/
long LANG_GetMaxMessageId(void)
{
  return((long)iMaxIndexEntry);
  // Highest MessageId  (Not necessarily the number of msgs)
}
/************************************************************/

/************************************************************/
/* LANG_GetTextMessage                                      */
/************************************************************/
wchar_t *LANG_GetTextMessage(LANG_TextMessageNumber i)
{
  static wchar_t wszBuff[1024];
  wchar_t *wszTmp = GetLanguageString(i);

  if (wszTmp)  return wszTmp;
  if (i != 0)  wszTmp = GetLanguageString(0); // Try to lookup the default
  if (!wszTmp)  wszTmp = L" message not available.  (1:^1, 2:^2, 3:^3, A:^A, P:^P, S:^S)";

  wszBuff[0] = L'#';
  _ltow(i, &wszBuff[1], 10);
  wcscat(wszBuff, wszTmp);
  return wszBuff;
}

/************************************************************/
/* LANG_GetTextMessage2                                     */
/************************************************************/
wchar_t *LANG_GetTextMessage2(LANG_TextMessageNumber i)
{
  return(GetLanguageString(i));
}

/************************************************************/
/* LANG_GetTextMessageClean                                 */
/************************************************************/
wchar_t *LANG_GetTextMessageClean(LANG_TextMessageNumber MessageID)
{
  int     Index;
  wchar_t Letter;

  wcsncpy(CleanMessageBuffer, LANG_GetTextMessage(MessageID), CLEAN_BUFFER_SIZE-1);
  CleanMessageBuffer[CLEAN_BUFFER_SIZE-1] = 0;

  Index = wcslen(CleanMessageBuffer);
  while (Index > 0)
  {
    Index--;
    Letter = CleanMessageBuffer[Index];
    if (Letter >= 32 && Letter != L' ')  break; // Hit non-white space and not a control character.
  }
  CleanMessageBuffer[Index+1] = 0; // Remove stuff after good text.
  return CleanMessageBuffer;
}

/************************************************************/
/* LANG_InitializeSystem                                    */
/************************************************************/
int LANG_InitializeSystem(void)
{
  char tmp[64];
  static char sdecimal[64], sthousand[64];

  if (StartupExternalLanguage(-1))
    return 0;
  MDEF_CurrentLanguage = (unsigned char)iLangId;
  LANG_InitDisplayProperty(iLangId);         //init display

  LANG_LocaleNumberFormat.NumDigits = 0;    // Override for non-float
  LANG_LocaleNumberFormat.LeadingZero = 1;  // Override for non-float
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, tmp, 64);
  LANG_LocaleNumberFormat.Grouping = atoi(tmp);
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, sdecimal, 64);
  LANG_LocaleNumberFormat.lpDecimalSep = sdecimal;
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, sthousand, 64);
  LANG_LocaleNumberFormat.lpThousandSep = sthousand;
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, tmp, 64);
  LANG_LocaleNumberFormat.NegativeOrder = atoi(tmp);
  return 1; // Success
}

/************************************************************/
/* LANG_CleanAndRemoveSystem                                */
/************************************************************/
void LANG_CleanAndRemoveSystem(void)
{
  CleanupExternalLanguage();
  MDEF_CurrentLanguage = (unsigned char)iLangId;
}

/************************************************************/
/* FigureOutLanguage                                        */
/************************************************************/
INT16 FigureOutLanguage(void)
{
  int lang;
  char szLocale[256], file[256], string[256];

  // US version has only one datafile set - 01, if its a US build, don't play around.
#if USA_VERSION
      return NOTW_LANG_ENGLISH_US;  // US English
#endif


  // Try to get language code from monlang.ini file
  UDUTILS_GetModulePath(file, 256);
  strcat(file, "monlang.ini");
  GetPrivateProfileString("INSTALL_LANG",
    "InstallLanguage", "0", string, 256, file);

  lang = atoi(string);
  if ((lang > 0) && (lang < NOTW_LANG_MAX))
    return(lang);

  // Couldn't get language code from ini file,
  // so get it from user's regional settings
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, szLocale, 256);
  if (stricmp(szLocale, "English") == 0)
  {
    GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGCOUNTRY, szLocale, 256);
    if (stricmp(szLocale, "United States") == 0)
      return NOTW_LANG_ENGLISH_US;  // US English
    else
      return NOTW_LANG_ENGLISH_UK;  // UK English
  }
  else if (stricmp(szLocale, "French") == 0)
    return NOTW_LANG_FRENCH;
  else if (stricmp(szLocale, "German") == 0)
    return NOTW_LANG_GERMAN;
  else if (stricmp(szLocale, "Spanish") == 0)
    return NOTW_LANG_SPANISH;
  else if (stricmp(szLocale, "Dutch") == 0)
    return NOTW_LANG_DUTCH;
  else if (stricmp(szLocale, "Swedish") == 0)
    return NOTW_LANG_SWEDISH;
  else if (stricmp(szLocale, "Finnish") == 0)
    return NOTW_LANG_FINNISH;
  else if (stricmp(szLocale, "Danish") == 0)
    return NOTW_LANG_DANISH;
  else if (stricmp(szLocale, "Norwegian") == 0)
    return NOTW_LANG_NORWEGIAN;
  else
    return NOTW_LANG_ENGLISH_US;
}

/************************************************************/
/* CleanupExternalLanguage                                  */
/************************************************************/
void CleanupExternalLanguage(void)
{
  if (iLangId == NOTW_LANG_NOT_INITIALIZED)
    return;
  iLangId = NOTW_LANG_NOT_INITIALIZED;
  if (datafile != LED_EI)
  {
    LE_DATA_RemoveDataGroup(DAT_LANG);
    datafile = LED_EI;
  }
  if (datafile2 != LED_EI)
  {
    LE_DATA_RemoveDataGroup(DAT_LANG2);
    datafile2 = LED_EI;
  }
  if (datafile3 != LED_EI)
  {
    LE_DATA_RemoveDataGroup(DAT_LANGDIALOG);
    datafile3 = LED_EI;
  }
}

/************************************************************/
/* StartupExternalLanguage                                  */
/************************************************************/
int StartupExternalLanguage(short langid)
{
  char  szFileName[256];
  char  szFileName2[256];
  char  szFileName3[256];
  INT16 iLang;

  if (langid < 0)  iLang = FigureOutLanguage();
  else             iLang = langid;
  switch(iLang)
  {
    case NOTW_LANG_ENGLISH_US:
      strcpy(szFileName, "dat_mon\\dat_ln01.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm01.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk01.dat");
      break;
    case NOTW_LANG_ENGLISH_UK:
      strcpy(szFileName, "dat_mon\\dat_ln02.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm02.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk02.dat");
      break;
    case NOTW_LANG_FRENCH:
      strcpy(szFileName, "dat_mon\\dat_ln03.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm03.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk03.dat");
      break;
    case NOTW_LANG_GERMAN:
      strcpy(szFileName, "dat_mon\\dat_ln04.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm04.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk04.dat");
      break;
    case NOTW_LANG_SPANISH:
      strcpy(szFileName, "dat_mon\\dat_ln05.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm05.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk05.dat");
      break;
    case NOTW_LANG_DUTCH:
      strcpy(szFileName, "dat_mon\\dat_ln06.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm06.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk06.dat");
      break;
    case NOTW_LANG_SWEDISH:
      strcpy(szFileName, "dat_mon\\dat_ln07.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm07.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk07.dat");
      break;
    case NOTW_LANG_FINNISH:
      strcpy(szFileName, "dat_mon\\dat_ln08.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm08.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk08.dat");
      break;
    case NOTW_LANG_DANISH:
      strcpy(szFileName, "dat_mon\\dat_ln09.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm09.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk09.dat");
      break;
    case NOTW_LANG_NORWEGIAN:
      strcpy(szFileName, "dat_mon\\dat_ln10.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm10.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk10.dat");
      break;
    default:
      strcpy(szFileName, "dat_mon\\dat_ln01.dat");
      strcpy(szFileName2, "dat_mon\\dat_lm01.dat");
      strcpy(szFileName3, "dat_mon\\dat_lk01.dat");
      break;
  }
  iLangId = iLang;
  if (datafile != LED_EI)
  {
    LE_DATA_RemoveDataGroup(DAT_LANG);
    datafile = LED_EI;
  }
  if (datafile = LE_DATA_InitDatafile(szFileName, DAT_LANG))
  {
    LE_DATA_DataIndexEntry *table;
    unsigned long tablesize;

    table = (LE_DATA_DataIndexEntry *)LE_DATA_Use(LED_IFT(DAT_LANG, 0));
    tablesize = LE_DATA_GetCurrentSize(LED_IFT(DAT_LANG, 0)) / sizeof(LE_DATA_DataIndexEntry);
    iMaxIndexEntry = table[tablesize-1].indexValue;
  } else {
    iMaxIndexEntry = 0;
    return(1);
  }

  if (datafile2 != LED_EI)
  {// FIXME - betcha this would crash the game, or at least mess up the visuals a lot.
    LE_DATA_RemoveDataGroup(DAT_LANG2);
    datafile2 = LED_EI;
  }
  if (!(datafile2 = LE_DATA_InitDatafile(szFileName2, DAT_LANG2)))
    return(1);

  if (datafile3 != LED_EI)
  {// FIXME - betcha this would crash the game, or at least mess up the visuals a lot.
    LE_DATA_RemoveDataGroup(DAT_LANGDIALOG);
    datafile3 = LED_EI;
  }
  if (datafile3 = LE_DATA_InitDatafile(szFileName3, DAT_LANGDIALOG))
    return(0);
  return(1);
}

/************************************************************/
/* GetLanguageString                                        */
/************************************************************/
wchar_t *GetLanguageString(INT32 i)
{
  if (iLangId == NOTW_LANG_NOT_INITIALIZED)
    return(LED_EI);
  if (i < 0 || i > iMaxIndexEntry)
    return(LED_EI);
  LE_DATA_DataId item = LE_DATA_GetIndexedItemId(LED_IFT(DAT_LANG, 0), i);
  if (item != LED_EI)
    return((wchar_t *)LE_DATA_Use(item));
  return(LED_EI);
}

/**************************************************************************
* Function: LANG_MoneyToTmpIString
* Description: Takes a double, and uses Windows' regional settings to
* format it as a money value should be in the current locale.
* (ie: In France: F123 456 789,00)
* Returns a temporary string.  (One that gets overwritten with each call)
**************************************************************************/
wchar_t *LANG_MoneyToTmpIString(double value)
{
  static wchar_t string[256];
  char tmp[256], tmp1[256];

  sprintf(tmp, "%1.20f", value);
  GetCurrencyFormat(LOCALE_USER_DEFAULT, 0, tmp, NULL, tmp1, 256);
  mbstowcs(string, tmp1, 256);
  return(string);
}

/**************************************************************************
* Function: LANG_DoubleToTmpIString
* Description: Takes a double, and uses Windows' regional settings to
* format it as a number should be in the current locale.
* Returns a temporary string.  (One that gets overwritten with each call)
**************************************************************************/
wchar_t *LANG_DoubleToTmpIString(double value)
{
  static wchar_t string[256];
  char tmp[256], tmp1[256];

  sprintf(tmp, "%1.20f", value);
  GetNumberFormat(LOCALE_USER_DEFAULT, 0, tmp, NULL, tmp1, 256);
  mbstowcs(string, tmp1, 256);
  return(string);
}

/**************************************************************************
* Function: LANG_LongToTmpIString
* Description: Takes a long, and uses Windows' regional settings to format
* it as a number should be in the current locale.  (no trailing decimals)
* Returns a temporary string.  (One that gets overwritten with each call)
**************************************************************************/
wchar_t *LANG_LongToTmpIString(long value)
{
  static wchar_t string[256];
  char tmp[256], tmp1[256];

  sprintf(tmp, "%ld", value);
  GetNumberFormat(LOCALE_USER_DEFAULT, 0, tmp,
    &LANG_LocaleNumberFormat, tmp1, 256);
  mbstowcs(string, tmp1, 256);
  return(string);
}

/************************************************************/
/* DEFINES                                                  */
/************************************************************/
// These numbers affects the size of memory buffer in the
// program, change them to increase or decrease memory size.
#define LANG_MAX_NUM_STORE_TXT  10      // In one line, how many different style of txt? (change in color, bold, italic...)
#define LANG_MAX_CMD            1000    // Maximum number of commands in the array
#define LANG_MAX_TXT_LEN        200     // Length of the already processed message that's ready to print (guaranteed no more than one line of text)
#define LANG_MAX_TXT_LINES      200     // Numbers of processed message

// Meta Symbols
#define LANG_META_OPEN          L'<'
#define LANG_META_CLOSE         L'>'
#define LANG_META_BOLD          L'B'
#define LANG_META_ITALIC        L'I'
#define LANG_META_NORMAL        L'N'
#define LANG_META_JLEFT         L'L'
#define LANG_META_JRGT          L'R'
#define LANG_META_JCNT          L'C'
#define LANG_META_COLOR         L'K'
#define LANG_META_TAB           L'T'
#define LANG_META_HYPER         L'H'
#define LANG_META_RETURN        L'U'
#define LANG_META_GFX           L'G'
#define LANG_META_GFXP          L'Z'
#define LANG_META_IMPERIAL      L'S'
#define LANG_META_FONTSIZE      L'P'
#define LANG_META_FONT          L'F'

/************************************************************/
/* ENUMS                                                    */
/************************************************************/

// COMMANDS ID
enum
{
  LANG_SetAlign,      // param1: justify style
  LANG_SetStyle,      // param1: bold, italic, or normal
  LANG_SetFont,       // param1: fonts id
  LANG_SetFontsSize,  // param1: fonts size

  LANG_QueueTxt,      // param1: end position, param2: justify param4: pointer to unicode string(stored text are printed before a new line)
  LANG_NewLine,       // param1: pointer to the message that follows(this pointer gets returned if the new line can't fit in the rectangle

  LANG_SetTxtLeftX,   // param1: x-pos
  LANG_SetTxtRightX,  // param1: x-pos
  LANG_SetTxtTopY,    // param1: y-pos
  LANG_SetTxtBotY,    // param1: y-pos
  LANG_SetYSpace,     // param1: pixel height of a line of text
  LANG_ShowTAB,       // param1: dataid(temporary), param2: x-pos, param3: y-pos
  LANG_TxtColor,      // param1: new color(COLOR_REF)
  LANG_HyperTxtColor, // param1: new color(COLOR_REF)
  LANG_Hyper,         // param1: link id, param2: justify, param3: pointer to wchar,
  LANG_Finish
};

/************************************************************/
/* STRUCTURES                                               */
/************************************************************/

typedef struct
{
  long    action;
  long    param1;
  long    param2;
  long    param3;
  wchar_t *param4;
} LANG_COMMAND;

typedef struct
{
  long left_x;
  long right_x;
  long top_y;
  long bot_y;
} LANG_PRINT_INFO;

// Used for queue text
typedef struct
{
  wchar_t             *msg;
  LANG_PRINT_PROPERTY property;
  BOOL                hyper_l;    //for hyper link text
  long                link_id;    //for hyper link text
  long                end_pos;
} LANG_TXT_STORE;

// Stores the already processed messages
typedef struct
{
  wchar_t msg[LANG_MAX_TXT_LINES][LANG_MAX_TXT_LEN];
} LANG_MSG_STRUCT;

typedef struct
{
  long stop;
  long width;
  long top;
  long bottom;
  long justify;
} LANG_TXT_STOP;  // give information about a TAB insertion or hypertext insertion for formating the text

typedef struct
{
  long id;
  long cur_pos;
  long justify;
} LANG_QueueTAB_Info;
//a signal for graphics insertion when text is right justifies, or center justified because
//we need to wait until the end of line reached in order to find out the actual position
//of a grafix

typedef struct
{
  char resource_name[256];
  char family_name[256];
  long id;
} FONT_STRUCT;
/************************************************************/

/************************************************************/
/* GLOBALS                                                  */
/************************************************************/
LANG_PRINT_PROPERTY    LANG_ppty;                     // Current display property for the processing
static LANG_PRINT_INFO LANG_psts;                     // Format position
static RECT            LANG_hlink[LANG_MAX_HLINK];    // Stores all hyper link rectangle
static DWORD           LANG_hlink_id[LANG_MAX_HLINK]; // Stores all hyper link id

static long            LANG_num_hlink;
static LANG_MSG_STRUCT LANG_mst;                      // Where all text goes before they get displayed
static BOOL            LANG_ProcessError=FALSE;
static LANG_TXT_STOP   LANG_tstop[100];               // Indicate a graphics insertion, so we can format text around them
static long            LANG_num_tstop=0;
static long            LANG_pitch_adjust=0;           // Give control of fonts adjustment

//current display
static long LANG_cur_font      = -1;
static long LANG_cur_font_size = -1;
static long LANG_cur_style     = -1;

FONT_STRUCT LANG_AllFonts[30]={
  {"F_EN_01.TTF", "F_en_01", 1001}, // English US
  {"F_EN_02.TTF", "F_en_02", 1002},
  {"F_EN_03.TTF", "F_en_03", 1003},
  {"F_EN_01.TTF", "F_en_01", 1001}, // English UK
  {"F_EN_02.TTF", "F_en_02", 1002},
  {"F_EN_03.TTF", "F_en_03", 1003},
  {"F_EN_01.TTF", "F_en_01", 1001}, // French
  {"F_EN_02.TTF", "F_en_02", 1002},
  {"F_EN_03.TTF", "F_en_03", 1003},
  {"F_EN_01.TTF", "F_en_01", 1001}, // German
  {"F_EN_02.TTF", "F_en_02", 1002},
  {"F_EN_03.TTF", "F_en_03", 1003},
  {"F_EN_01.TTF", "F_en_01", 1001}, // Spanish
  {"F_EN_02.TTF", "F_en_02", 1002},
  {"F_EN_03.TTF", "F_en_03", 1003},
  {"F_EN_01.TTF", "F_en_01", 1001}, // Dutch
  {"F_EN_02.TTF", "F_en_02", 1002},
  {"F_EN_03.TTF", "F_en_03", 1003},
  {"F_EN_01.TTF", "F_en_01", 1001}, // Swedish
  {"F_EN_02.TTF", "F_en_02", 1002},
  {"F_EN_03.TTF", "F_en_03", 1003},
  {"F_EN_01.TTF", "F_en_01", 1001}, // Finnish
  {"F_EN_02.TTF", "F_en_02", 1002},
  {"F_EN_03.TTF", "F_en_03", 1003},
  {"F_EN_01.TTF", "F_en_01", 1001}, // Danish
  {"F_EN_02.TTF", "F_en_02", 1002},
  {"F_EN_03.TTF", "F_en_03", 1003},
  {"F_EN_01.TTF", "F_en_01", 1001}, // Norwegian
  {"F_EN_02.TTF", "F_en_02", 1002},
  {"F_EN_03.TTF", "F_en_03", 1003}
//  {"F_EN_03.TTF", "Haettenshweiler", 1003},   // German
//  {"F_EN_03.TTF", "Haettenshweiler", 1003},
//  {"F_EN_03.TTF", "Haettenshweiler", 1003}
};

FONT_STRUCT *LANG_Fonts;
void LANG_SetPrintEnvironment(LANG_PRINT_PROPERTY *prty);

/************************************************************/
/* LANG_OnLanguageSet                                       */
/*                                                          */
/* Set fonts pointer according current language             */
/************************************************************/
void LANG_OnLanguageSet(short id)
{
  short index;
  switch(id)
  {
  case NOTW_LANG_ENGLISH_US:
    index = 0;
    break;
  case NOTW_LANG_ENGLISH_UK:
    index = 3;
    break;
  case NOTW_LANG_FRENCH:
    index = 6;
    break;
  case NOTW_LANG_GERMAN:
    index = 9;
    break;
  case NOTW_LANG_SPANISH:
    index = 12;
    break;
  case NOTW_LANG_DUTCH:
    index = 15;
    break;
  case NOTW_LANG_SWEDISH:
    index = 18;
    break;
  case NOTW_LANG_FINNISH:
    index = 21;
    break;
  case NOTW_LANG_DANISH:
    index = 24;
    break;
  case NOTW_LANG_NORWEGIAN:
    index = 27;
    break;
  default:
    index = 0;
    break;
  }
  LANG_Fonts=&LANG_AllFonts[index];
}
/************************************************************/

/************************************************************/
/* LANG_SetCurPrintProperty                                 */
/*                                                          */
/* Set display property and make the actual fonts print     */
/* property consistent with display property that used for  */
/* processing                                               */
/************************************************************/
void LANG_SetCurPrintProperty(long font, long font_size, long style, long align)
{
  if (font!=-1)
  {
    LANG_ppty.font=font;
    LANG_ppty.font_size=-1;
  }
  if (font_size!=-1)
    LANG_ppty.font_size=font_size;
  if (style!=-1) LANG_ppty.style=style;
  if (align!=-1) LANG_ppty.align=align;
  LANG_SetPrintEnvironment(&LANG_ppty);
}

/************************************************************/
/* LANG_Default                                             */
/************************************************************/
void LANG_Default(void)
{
  LANG_SetCurPrintProperty(2, 12, LANG_Normal, LANG_Left);
  LANG_SetPrintEnvironment(&LANG_ppty);
}

/************************************************************/
/* LANG_InitDisplayProperty                                 */
/*                                                          */
/* Called when the program start or language has changed    */
/************************************************************/
void LANG_InitDisplayProperty(short langid)
{
  LANG_cur_font=-1;           // manually force the fonts to be installed.
  LANG_OnLanguageSet(langid);
  LANG_Default();
}

/************************************************************/
/* LANG_CopyPrintProperty                                   */
/************************************************************/
void LANG_CopyPrintProperty(LANG_PRINT_PROPERTY *dest, LANG_PRINT_PROPERTY *src)
{
  memcpy(dest, src, sizeof(LANG_PRINT_PROPERTY));
}

/************************************************************/
/* LANG_SetPrintEnvironment                                 */
/*                                                          */
/* Only function that make change to actual fonts print     */
/* property in the library.                                 */
/************************************************************/
void LANG_SetPrintEnvironment(LANG_PRINT_PROPERTY *prty)
{
  long new_font;
  long tmp_size;

  if (prty->font!=LANG_cur_font && prty->font>=0)
  {
    new_font=prty->font;
//    LE_FONTS_RemoveFont();

//    if (new_font>-1)
//      LE_FONTS_SetFont(LANG_Fonts[new_font].resource_name, LANG_Fonts[new_font].family_name);
      //(short)LANG_Fonts[new_font].id); // Old library used this

    LANG_cur_font=new_font;
    LANG_cur_font_size=-1;      //reset font size
  }

  //if (prty->font_size!=LANG_cur_font_size)
  {
    tmp_size=prty->font_size+LANG_pitch_adjust;
    if (tmp_size<4) tmp_size=4;         //minimum pitch size
    if (tmp_size>72) tmp_size=72;       //maximum pitch size

    if (prty->font_size>=0)
      LE_FONTS_SetSize((short)tmp_size);
    else
      LE_FONTS_ResetCharacteristics();

    LANG_cur_font_size=prty->font_size;
  }

  //if (LANG_cur_style!=prty->style)
  {
    if (prty->style==LANG_Bold)
    {
      LE_FONTS_SetItalic(FALSE);
      LE_FONTS_SetWeight(700);
    }
    else if (prty->style==LANG_Italic)
    {
      LE_FONTS_SetWeight(400);
      LE_FONTS_SetItalic(TRUE);
    }
    else
    {
      LE_FONTS_SetWeight(400);
      LE_FONTS_SetItalic(FALSE);
    }
    LANG_cur_style=prty->style;
  }
}

/************************************************************/
/* LANG_CpCmd                                               */
/*                                                          */
/* Compose a Command                                        */
/************************************************************/
void LANG_CpCmd(LANG_COMMAND *cmd, long action, long param1, long param2, long param3, wchar_t *param4)
{
  cmd->action=action;
  cmd->param1=param1;
  cmd->param2=param2;
  cmd->param3=param3;
  cmd->param4=param4;
}

/************************************************************/
BOOL LANG_Meta(wchar_t *str)
{
  if (str[0]==LANG_META_OPEN && str[2]==LANG_META_CLOSE) return TRUE;
  return FALSE;
}

/************************************************************/
short LANG_GetStringHeight(wchar_t *wsz)
{
  short return_value = 0;
  if (wsz != NULL)
    if (*wsz)
      return_value =  LE_FONTS_GetStringHeight(wsz);
  return (return_value);
}

/************************************************************/
short LANG_GetStringWidth(wchar_t *wsz)
{
  short return_value = 0;
  if (wsz != NULL)
    if (*wsz)
      return_value = LE_FONTS_GetStringWidth(wsz);
  return (return_value);
}

/************************************************************/
void LANG_Print(LE_DATA_DataId iObjectHandle, short iX, short iY, DWORD iPrintColour, wchar_t *wszString)
{
  LE_FONTS_Print(iObjectHandle,iX, iY,iPrintColour,wszString);
}
