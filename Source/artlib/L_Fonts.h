#ifndef __L_FONTS_H__
#define __L_FONTS_H__

/*************************************************************
*
*   FILE:           l_fonts.h
*   DESCRIPTION:    Font system header.
*
*   (C) Copyright 1995/96 Artech Digital Entertainments.
*                         All rights reserved.
*
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/*----------------------------------------------------------*/

#define LI_FONTS_NormalLogicalScreenPixels 96
#define LI_FONTS_MaxSetSize 10  // Slots go from 0 to LI_FONTS_MaxSetSize-1

// Definitions for "stock fonts"
#define ANSI_FIXED_FONT_FILE "coure.fon"
#define ANSI_VAR_FONT_FILE "sserife.fon"
#define DEFAULT_GUI_FONT_FILE "sserife.fon"
#define OEM_FIXED_FONT_FILE "vgaoem.fon"
#define SYSTEM_FONT_FILE "vgasys.fon"
#define SYSTEM_FIXED_FONT_FILE "vgafix.fon"

#define ANSI_FIXED_FONT_NAME "Courier"
#define ANSI_VAR_FONT_NAME "MS Sans Serif"
#define DEFAULT_GUI_FONT_NAME "MS Sans Serif"
#define OEM_FIXED_FONT_NAME "Terminal"
#define SYSTEM_FONT_NAME "System"
#define SYSTEM_FIXED_FONT_NAME "Fixedsys"

/************************************************************/

/************************************************************/
/* STRUCTURES, ENUMS, ETC.                                  */
/*----------------------------------------------------------*/

/************************************************************/

/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

/************************************************************/

/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

void LI_FONTS_InitSystem(void);
void LI_FONTS_RemoveSystem(void);

// Start Doc
// L_FONTS

short LE_FONTS_GetStringHeight(ACHAR *StringPntr);
// Returns the height in pixels of the string using the current font.

short LE_FONTS_GetStringWidth(ACHAR *StringPntr);
// Returns the width in pixels of the string using the current font.

void LE_FONTS_Print(LE_DATA_DataId DataID, short XOffset, short YOffset, DWORD Colour, ACHAR *StringPntr);
// Displays the string to the given object, which can be a native bitmap, a generic bitmap, a UAP or the screen (if DataID is LE_DATA_EmptyItem).
// The display location XOffset, YOffset is relative to the top left of the display destination.
// Colour must be a 32 bit color ref (see the LE_GRAFIX_MakeColorRef[Alpha] macro),
// it looks like 0x00bbggrr or 0xaabbggrr.
// Alpha (0-255) is only used for printing to UAPs.  (The closest palette entry to Colour/Alpha will be chosen)
// Note that if the GBM gets unloaded, the text is lost.

BOOL LE_FONTS_SetFont(char *FileName, char *FontName);
// Changes the font to use for print operations.
// FileName can be one of the magic predefined names of ANSI_FIXED_FONT_FILE, ANSI_VAR_FONT_FILE,
// DEFAULT_GUI_FONT_FILE, OEM_FIXED_FONT_FILE, SYSTEM_FONT_FILE, SYSTEM_FIXED_FONT_FILE.
// If it is one of these values, FontName should be NULL.
//
// If FileName is not one of those values, it is the name of a font file (ie: "Arial.ttf").
// The routine looks for it in the same directory as the executable file, and
// if it's not found there, the routine looks for it in the Windows fonts directory.
// For these fonts, the second parameter is the name of the font you wish to install.
//
// To find the name for FontName, go to Control Panel->Fonts and double-click on the font.
// If it is a TrueType font, the font name is the name after the "Typeface name:"
// string.  If it is not a TrueType font, the font name is the name (first line)
// shown in the window.  (See "Modern" and "Small Fonts")
// Note that Arial and Arial Bold, for example, have different filenames, but the same fontname.
// If you load a bold font, you should still use the LE_FONTS_SetWeight function.
//
// By default the initial font available is SYSTEM_FONT_FILE.  Note that the "stock fonts"
// ([font]_FILE listed above) should be on every system but that does not mean they are the
// same font or look the same on each system.  They also may not scale well.
// Use your own font if you want to make sure your display looks good and consistent.

int CALLBACK FindFontProc(LOGFONT *lpelf, NEWTEXTMETRIC *lpntm, int nFontType, LPARAM lParam);
// Called by EnumFontFamilies to find a font.

void LE_FONTS_RemoveFont(void);
// Remove the current font.

void LE_FONTS_ResetCharacteristics(void);
// Reset all font characteristics to the default values.

void LE_FONTS_SetSize(int HeightInPixels);
// Changes the size of the font to be the given number of pixels tall.
// (all characters will fit in a box this many pixels tall).

int LE_FONTS_GetSize(void);
// Return the current size of the font.

void LE_FONTS_SetWeight(int BoldFactor = 400);
// Sets how bold the font is, from 0 to 1000.

int LE_FONTS_GetWeight(void);
// Return the current weight of the font.

void LE_FONTS_SetPitchAndFamily(short pitch_flag);
// Sets the pitch and family of the font. Use one of the following:
//   - FF_DECORATIVE: Decorative fonts, old english is an example.
//   - FF_DONTCARE  : Don't care or don't know.
//   - FF_MODERN    : Fonts with constant stroke width, with or without serifs. Pica, Elite and Courier New are examples.
//   - FF_ROMAN     : Fonts with variable stroke width and with serifs. MS Serif is an example.
//   - FF_SCRIPT    : Fonts designed to look like handwriting. Script and Cursive are examples.
//   - FF_SWISS     : Fotns with variable stroke width and without serifs. MS Sans Serif is an example.

void LE_FONTS_SetUnderline(BOOL underline);
// Set to TRUE if you want the font to be displayed as underlined.

void LE_FONTS_SetItalic(BOOL italic);
// Set to TRUE if you want the font to be displayed as italics.
// Note that the width Windows returns for italic text seems to be
// too small.  You might have to add a space or two to your text.

void LE_FONTS_SetStrikeOut(BOOL strikeout);
// Set to TRUE if you want the font to be displayed as struck out.

void LE_FONTS_GetSettings(UNS16 set);
// Get a font setting previously specified with LE_FONTS_SetSettings

void LE_FONTS_SetSettings(UNS16 set);
// Set a font setting

LOGFONT *LE_FONTS_GetLogfont();
// Returns the logical font

void LI_FONTS_BltText(
  LPBYTE lpDestinationBits, int nXDestinationWidthInPixels,
  int nXBltStartCoordinate, int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels, LPRECT lprSourceBltRect);
// Support function to allow blitting to an 8-bit surface.

// End Doc
/************************************************************/

#endif // __L_FONTS_H__
