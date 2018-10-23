/*****************************************************************************
*
*   FILE:           L_FONTS.C
*   DESCRIPTION:    Main font system source file.
*
*   (C) Copyright 1995/96 Artech Digital Entertainments.
*                         All rights reserved.
*
* $Header: /Artlib_99/ArtLib/L_Fonts.cpp 17    99/10/26 1:47p Timp $
*****************************************************************************/

/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"

/************************************************************/

#if CE_ARTLIB_EnableSystemFonts

// ASCII or Widechar versions depending on Unicode flag
#if CE_ARTLIB_UnicodeAlphabet
  #define FONTS_ExtTextOut ExtTextOutW
  #define FONTS_GetTextExtentPoint32 GetTextExtentPoint32W
#else
  #define FONTS_ExtTextOut ExtTextOutA
  #define FONTS_GetTextExtentPoint32 GetTextExtentPoint32A
#endif

/************************************************************/
/* STATIC VARIABLES                                         */
/*----------------------------------------------------------*/

static HFONT    fontHandle = NULL;
static LOGFONT  logicalFont;
static char    *CurrentFontFile = NULL;
static char    *CurrentFontName = NULL;
static short    LI_FONTS_CurrentSize = 0;
static short    LI_FONTS_CurrentWeight = 0;
static char     LI_FONTS_FontFileName[MAX_PATH] = "";
static char     LI_FONTS_PrgPath[MAX_PATH] = "";
static char     LI_FONTS_WinFontPath[MAX_PATH] = "";
static int      LI_FONTS_LogicalScreenPixels;

static LOGFONT  logicalFontSet[LI_FONTS_MaxSetSize];
static char     CurrentFontFileSet[LI_FONTS_MaxSetSize][MAX_PATH];
static char     CurrentFontNameSet[LI_FONTS_MaxSetSize][MAX_PATH];
static short    LI_FONTS_CurrentSizeSet[LI_FONTS_MaxSetSize];
static short    LI_FONTS_CurrentWeightSet[LI_FONTS_MaxSetSize];

/************************************************************/

/*****************************************************************************
* Function:    LI_FONTS_InitSystem
* Description: Initialize FONTS subsystem.
*****************************************************************************/
void LI_FONTS_InitSystem(void)
{
  HDC hDC;
  char *StringPntr;

  hDC = GetDC(LE_MAIN_HwndMainWindow);
  LI_FONTS_LogicalScreenPixels = GetDeviceCaps(hDC, LOGPIXELSY);
  if (LI_FONTS_LogicalScreenPixels == 0)  // (in)Sanity check
    LI_FONTS_LogicalScreenPixels = LI_FONTS_NormalLogicalScreenPixels;
  ReleaseDC(LE_MAIN_HwndMainWindow, hDC);

  // Get the filename (and then pathname) of the executable
  if (GetModuleFileName(NULL, LI_FONTS_PrgPath, sizeof(LI_FONTS_PrgPath)) == 0)
    LE_ERROR_ErrorMsg("LI_FONTS_InitSystem: "
      "Unable to get path name of this program's executable file!\r\n");

  // Remove the program name and just leave the path name
  StringPntr = strrchr(LI_FONTS_PrgPath, '\\');
  *StringPntr = 0;

  // Get Windows font directory
  if (GetWindowsDirectory(LI_FONTS_WinFontPath, sizeof(LI_FONTS_WinFontPath)) == 0)
    LE_ERROR_ErrorMsg("LI_FONTS_InitSystem: "
      "Unable to get path name of Windows directory!\r\n");

  // Remove the trailing backslash if there is one
  StringPntr = &LI_FONTS_WinFontPath[strlen(LI_FONTS_WinFontPath) - 1];
  if (*StringPntr == '\\')
    *StringPntr = 0;
  strcat(LI_FONTS_WinFontPath, "\\fonts");

  LE_FONTS_ResetCharacteristics();
  LE_FONTS_SetFont(SYSTEM_FONT_FILE, NULL);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LI_FONTS_RemoveSystem
* Description: Cleanup and remove FONTS subsystem.
*****************************************************************************/
void LI_FONTS_RemoveSystem(void)
{
  LE_FONTS_RemoveFont();
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_GetStringHeight
* Description: Return height in pixels of the string using the current font.
*****************************************************************************/
short LE_FONTS_GetStringHeight(ACHAR *StringPntr)
{
  if (*StringPntr)
  {
    HDC  hDC;
    SIZE rsize;
    HGDIOBJ TempFont;

    hDC = GetDC(LE_MAIN_HwndMainWindow);
    TempFont = SelectObject(hDC, fontHandle);
    FONTS_GetTextExtentPoint32(hDC, StringPntr, Astrlen(StringPntr), &rsize);
    SelectObject(hDC, TempFont);
    ReleaseDC(LE_MAIN_HwndMainWindow,hDC);
    return((short)rsize.cy);
  }
  return(0);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_GetStringWidth
* Description: Return width in pixels of the string using the current font.
*****************************************************************************/
short LE_FONTS_GetStringWidth(ACHAR *StringPntr)
{
  if (*StringPntr)
  {
    HDC  hDC;
    SIZE rsize;
    HGDIOBJ TempFont;

    hDC = GetDC(LE_MAIN_HwndMainWindow);
    TempFont = SelectObject(hDC, fontHandle);
    FONTS_GetTextExtentPoint32(hDC, StringPntr, Astrlen(StringPntr), &rsize);
    SelectObject(hDC, TempFont);
    ReleaseDC(LE_MAIN_HwndMainWindow,hDC);
    return((short)rsize.cx+1);  // ie: Single "W" needs +1.
  }
  return(0);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_Print
* Description: Displays the string to the given object, which can be a native
* bitmap, a generic bitmap, a UAP or the screen (if DataID is LE_DATA_EmptyItem).
* The display location XOffset, YOffset is relative to the top left of the
* display destination.  Colour must be a 32 bit ColorRef. (possibly with alpha,
* wihch is only used for printing to UAPs -- The closest palette entry to
* Colour/Alpha is chosen)  Note that if the GBM gets unloaded, the text is lost.
*****************************************************************************/
void LE_FONTS_Print(LE_DATA_DataId DataID,
  short XOffset, short YOffset,
  DWORD Colour, ACHAR *StringPntr)
{
    #if CE_ARTLIB_EnableDebugMode
        if (DataID == LED_EI)
        {
            static BOOL first_time = TRUE;

            wsprintf(LE_ERROR_DebugMessageBuffer, "LE_FONTS_Print: Display straight to the former background (LE_DATA_EmptyItem/LED_EI) is no longer supported!");

            if (first_time) LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFileAndScreen);
            else            LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);

            first_time = FALSE;
        }
    #endif

  if (*StringPntr)
  {
    HDC            hDC;
    HDC            hMemDC;
    HBITMAP        hBitmap;
    UNS16          obj_width = 0;
    UNS16          obj_height = 0;
    LPVOID         obj_data_ptr = 0;
    LE_DATA_DataId object_type;
    long           string_width;
    long           string_height;
    HGDIOBJ        TempBmp;
    HGDIOBJ        TempFont;

    RECT           cliprect, tmprect;
    LE_GRAFIX_GenericBitmapPointer gbm;

    // Initialization and calculations
    XOffset--;
    string_width  = LE_FONTS_GetStringWidth(StringPntr);
    string_height = LE_FONTS_GetStringHeight(StringPntr);

    if (DataID != LE_DATA_EmptyItem) // Not the screen
      object_type = LE_DATA_GetLoadedDataType(DataID);
    else
      object_type = LE_DATA_DataTypeMax;

    if (object_type == LE_DATA_DataUAP) {
      obj_width  = LE_GRAFIX_TCBwidth(DataID);
      obj_height = LE_GRAFIX_TCBheight(DataID);
    } else if ((object_type == LE_DATA_DataNative) ||
               (object_type == LE_DATA_DataTypeMax)) {
      obj_width  = LE_GRAFIX_ReturnObjectWidth(DataID);
      obj_height = LE_GRAFIX_ReturnObjectHeight(DataID);
      Colour &= 0x00ffffff;  // Alpha not supported
    } else if (object_type == LE_DATA_DataGenericBitmap) {
      gbm = (LE_GRAFIX_GenericBitmapPointer)LE_DATA_GetPointer(DataID);
      obj_width  = gbm->width;
      obj_height = gbm->height;
      Colour &= 0x00ffffff;  // Alpha not supported
    }

    cliprect.right = XOffset + string_width;
    cliprect.bottom = YOffset + string_height;

    // Don't print anything if it's outside the object space.
    if ((XOffset > obj_width) || (YOffset > obj_height) ||
        (cliprect.right < 0) || (cliprect.bottom < 0))
      return;

    cliprect.left = XOffset;
    cliprect.top = YOffset;

    if (cliprect.left < 0)             cliprect.left = 0;
    if (cliprect.right > obj_width)    cliprect.right = obj_width;
    if (cliprect.top < 0)              cliprect.top = 0;
    if (cliprect.bottom > obj_height)  cliprect.bottom = obj_height;

    if (object_type == LE_DATA_DataUAP) {
      LPVOID             obj_data_ptr = 0;
      LPNEWBITMAPHEADER  TAB_Ptr;
      LPBYTE             TAB_ColorTable;
      LPBYTE             TAB_Bits;
      LPBYTE             bits;
      LPBITMAPINFOHEADER lpbih;

      // Round up the width of the string to an even multiple of 4.
      string_width = (string_width + 4) & 0xfffffffc;

      // Get some information about the TAB.
      obj_data_ptr = LE_DATA_UseDirty (DataID);
      TAB_Ptr = (LPNEWBITMAPHEADER)obj_data_ptr;
      TAB_ColorTable = (LPBYTE)&(TAB_Ptr->dwColorTable);
      TAB_Bits = TAB_ColorTable + (TAB_Ptr->nColors * sizeof(LONG) * 2);

      // Allocate some memory for the BitmapInfo structure, including space
      // for colour table in case Windows tries to access it, but we
      // only care about palette indices, so don't copy in a palette.
      lpbih = (LPBITMAPINFOHEADER)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));

      // Initialize BITMAPINFOHEADER structure
      lpbih->biSize          = sizeof(BITMAPINFOHEADER);
      lpbih->biWidth         = string_width;
      lpbih->biHeight        = -string_height;
      lpbih->biPlanes        = 1;
      lpbih->biBitCount      = 8;
      lpbih->biCompression   = BI_RGB;
      lpbih->biSizeImage     = 0;
      lpbih->biXPelsPerMeter = 0;
      lpbih->biYPelsPerMeter = 0;
      lpbih->biClrUsed       = 256;
      lpbih->biClrImportant  = 256;

      // Get handle to current display window. Create a bitmap that will be used
      // to handle the text drawing.
      hDC = GetDC(LE_MAIN_HwndMainWindow);
      hMemDC = CreateCompatibleDC(hDC);
      hBitmap = CreateDIBSection(hMemDC, (LPBITMAPINFO)lpbih,
        DIB_RGB_COLORS, (LPVOID *) &bits, NULL, 0);

      // Set the DC's bitmap, font, and fore/background text colour
      TempBmp = SelectObject(hMemDC, hBitmap);
      TempFont = SelectObject(hMemDC, fontHandle);
      SetBkColor(hMemDC, DIBINDEX(0));
      SetTextColor(hMemDC, DIBINDEX(LE_BLT_GetNearestColourAlpha(
        (LE_BLT_AlphaPaletteEntryPointer)TAB_ColorTable,
        TAB_Ptr->nColors, Colour)));

      // Display the text to the compatible bitmap
      SetRect(&tmprect, 0, 0, string_width, string_height);
      FONTS_ExtTextOut(hMemDC, 1, 0, ETO_CLIPPED|ETO_OPAQUE,
        &tmprect, StringPntr, Astrlen(StringPntr), NULL);

      cliprect.left   -= XOffset;
      cliprect.right  -= XOffset;
      cliprect.top    -= YOffset;
      cliprect.bottom -= YOffset;
      if (XOffset < 0)  XOffset = 0;
      if (YOffset < 0)  YOffset = 0;
      LI_FONTS_BltText(TAB_Bits, obj_width, XOffset, YOffset,
        bits, string_width, &cliprect);

      SelectObject(hMemDC, TempFont);
      SelectObject(hMemDC, TempBmp);
      DeleteObject(hBitmap);
      DeleteDC(hMemDC);
      ReleaseDC(LE_MAIN_HwndMainWindow, hDC);
      if (lpbih != NULL)  free(lpbih);
    } else if ((object_type == LE_DATA_DataNative) ||
               (object_type == LE_DATA_DataTypeMax)) {
      DWORD *masks;
      LPBYTE bmptr;
      PVOID bmbits;
      LPBITMAPINFO lpbi;

      bmptr = LE_GRAFIX_LockObjectDataPtr(DataID);

      // Allocate some memory for the BitmapInfo structure,
      // including space for masks for 16-bit mode
      lpbi = (LPBITMAPINFO)malloc(sizeof(BITMAPINFO) + 2 * sizeof(RGBQUAD));

      memset(lpbi, 0, sizeof(BITMAPINFO) + 2 * sizeof(RGBQUAD));
      lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      lpbi->bmiHeader.biWidth = string_width;
      lpbi->bmiHeader.biHeight = -string_height;
      lpbi->bmiHeader.biPlanes = 1;
      lpbi->bmiHeader.biBitCount = LE_GRAFIX_ScreenBitsPerPixel;
      lpbi->bmiHeader.biCompression = BI_RGB;
      // Set up the masks required for RGB 555 or 565 16-bit displays.
      if (lpbi->bmiHeader.biBitCount == 16)
      {
        lpbi->bmiHeader.biCompression = BI_BITFIELDS;
        masks = (DWORD *)lpbi->bmiColors;
        masks[0] = LE_BLT_dwRedMask;
        masks[1] = LE_BLT_dwGreenMask;
        masks[2] = LE_BLT_dwBlueMask;
      }

      hDC = GetDC(LE_MAIN_HwndMainWindow);
      hMemDC = CreateCompatibleDC(hDC);
      hBitmap = CreateDIBSection(hMemDC, lpbi, DIB_RGB_COLORS, &bmbits, NULL, 0);
      TempBmp = SelectObject(hMemDC, hBitmap);
      TempFont = SelectObject(hMemDC, fontHandle);
      SetBkColor(hMemDC, LEG_MCR(0, 255, 0));
      SetTextColor(hMemDC, Colour);
      SetRect(&tmprect, 0, 0, string_width, string_height);
      FONTS_ExtTextOut(hMemDC, 1, 0, ETO_CLIPPED|ETO_OPAQUE,
        &tmprect, StringPntr, Astrlen(StringPntr), NULL);

      cliprect.left   -= XOffset;
      cliprect.right  -= XOffset;
      cliprect.top    -= YOffset;
      cliprect.bottom -= YOffset;
      if (XOffset < 0)  XOffset = 0;
      if (YOffset < 0)  YOffset = 0;

      if (LE_GRAFIX_ScreenBitsPerPixel == 16)
        LI_BLT16ColorKeyPureGreenBitBlt16to16(
          (LPBYTE)bmptr, obj_width, XOffset, YOffset,
          (LPBYTE)bmbits, string_width, &cliprect);
      else if (LE_GRAFIX_ScreenBitsPerPixel == 24)
        LI_BLT24ColorKeyPureGreenBitBlt24to24(
          (LPBYTE)bmptr, obj_width, XOffset, YOffset,
          (LPBYTE)bmbits, string_width, &cliprect);
      else if (LE_GRAFIX_ScreenBitsPerPixel == 32)
        LI_BLT32ColorKeyPureGreenBitBlt32to32(
          (LPBYTE)bmptr, obj_width, XOffset, YOffset,
          (LPBYTE)bmbits, string_width, &cliprect);

      SelectObject(hMemDC, TempFont);
      SelectObject(hMemDC, TempBmp);
      DeleteObject(hBitmap);
      DeleteDC(hMemDC);
      ReleaseDC(LE_MAIN_HwndMainWindow, hDC);
      LE_GRAFIX_UnlockObjectDataPtr(DataID);
      if (lpbi != NULL)  free(lpbi);

#if CE_ARTLIB_EnableSystemAnim
      if (object_type == LE_DATA_DataTypeMax) {
        // If going to background register rect to update, as appropriate.
        if (LE_ANIM_BGRectAdding)
          LE_ANIM_AddToBGInvalidateRectList(&cliprect);
        else
          LE_ANIM_BGInvalidateSingleRect(&cliprect);
      }
#endif
    } else if (object_type == LE_DATA_DataGenericBitmap) {
      PDDSURFACE directDrawSurface = gbm->directDrawSurfacePntr;
      DDSURFDESC DDSurfaceDesc;
      UNS32 LikelyPitch;

      LE_GRAFIX_RestoreGBM(DataID);

      // Find out about the surface...  (Check for DirectDraw bug)
      memset (&DDSurfaceDesc, 0, sizeof (DDSurfaceDesc));
      DDSurfaceDesc.dwSize = sizeof (DDSurfaceDesc);
      DDSurfaceDesc.dwFlags = (DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT);

      if (DD_OK != directDrawSurface->GetSurfaceDesc (&DDSurfaceDesc))
      {
#if CE_ARTLIB_EnableDebugMode
        wsprintf(LE_ERROR_DebugMessageBuffer, "LE_FONTS_Print(): "
          "Couldn't get surface description for GBM DataID $%08X.\r\n", DataID);
        LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
        return;
      }

      // Look out for DirectDraw bug with calculating the pitch
      // for 24 and 32 bit images, at some sizes claiming an
      // extra 4 bytes in the pitch. (ie: < 215 pixel width)
      LikelyPitch = DDSurfaceDesc.dwWidth *
        (DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8);
      LikelyPitch = ((LikelyPitch + 3) & 0xFFFFFFFC); // Make into multiple of 4.
      if ((UNS32) DDSurfaceDesc.lPitch == LikelyPitch + 4)
      {
        // Likely the bug happened.
#if CE_ARTLIB_EnableDebugMode
        wsprintf(LE_ERROR_DebugMessageBuffer, "LE_FONTS_Print(): "
          "The GBM DataID $%08X is subject to a DirectDraw bug.\r\n"
          "The following text that was printed to it probably doesn't look right:\r\n"
          "\"%s\"\r\n\r\n",
          DataID, StringPntr);
        LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
      }

      directDrawSurface->GetDC(&hDC);
      // Set the font to the currently selected font.
      TempFont = SelectObject(hDC, fontHandle);
      SetBkMode(hDC, TRANSPARENT);
      SetTextColor(hDC, Colour);

      // Display the text to the generic bitmap
      FONTS_ExtTextOut(hDC, XOffset+1, YOffset, ETO_CLIPPED,
        &cliprect, StringPntr, Astrlen(StringPntr), NULL);

      SelectObject(hDC, TempFont);
      directDrawSurface->ReleaseDC(hDC);
    } else {
#if CE_ARTLIB_EnableDebugMode
      wsprintf(LE_ERROR_DebugMessageBuffer, "LE_FONTS_Print(): "
        "Object passed in not LE_DATA_EmptyItem, or one of type LE_DATA_DataUAP, LE_DATA_DataNative, or LE_DATA_DataGenericBitmap\r\n");
      LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
    }
  }
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_SetFont(char *FileName, char *FontName)
* Description: Changes the font to use for print operations.
* FileName can be one of the magic predefined names of
* ANSI_FIXED_FONT_FILE, ANSI_VAR_FONT_FILE, DEFAULT_GUI_FONT_FILE,
* OEM_FIXED_FONT_FILE,  SYSTEM_FONT_FILE,   SYSTEM_FIXED_FONT_FILE.
* If it is one of these values, FontName should be NULL.
*
* If FileName is not one of those values, it is the name of a font file
* (ie: "Arial.ttf").  The routine looks for it in the same directory as
* the executable file, and if it's not found there, the routine looks for
* it in the Windows fonts directory.  For these fonts, the second parameter
* is the name of the font you wish to install.
*
* To find the name for FontName, go to Control Panel->Fonts and double-click
* on the font.  If it is a TrueType font, the font name is the name after the
* "Typeface name:" string.  If it is not a TrueType font, the font name is the
* name (first line) shown in the window.  (See "Modern" and "Small Fonts")
* Note that Arial and Arial Bold, for example, have different filenames, but
* the same fontname.  If you load a bold font, you should still use the
* LE_FONTS_SetWeight function.
*
* By default the initial font available is SYSTEM_FONT_FILE.  Note that the
* "stock fonts" ([font]_FILE listed above) should be on every system but that
* does not mean they are the same font or look the same on each system.  They
* also may not scale well.  Use your own font if you want to make sure your
* display looks good and consistent.
*****************************************************************************/
BOOL LE_FONTS_SetFont(char *FileName, char *FontName)
{
  HDC hDC;
  int initial_size = 10;
  BOOL return_value = TRUE, stockflag = TRUE, loadedflag = FALSE;
  static char myfontfile[256], myfontname[256];

  if (*LI_FONTS_FontFileName != 0)
    LE_FONTS_RemoveFont();
  *LI_FONTS_FontFileName = 0;

  strncpy(myfontname, FileName, 255);
  if (strcmp(FileName, ANSI_FIXED_FONT_FILE) == 0) {
    sprintf(LI_FONTS_FontFileName, "%s\\%s", LI_FONTS_WinFontPath, FileName);
    strcpy(myfontname, ANSI_FIXED_FONT_NAME);
  } else if (strcmp(FileName, ANSI_VAR_FONT_FILE) == 0) {
    sprintf(LI_FONTS_FontFileName, "%s\\%s", LI_FONTS_WinFontPath, FileName);
    strcpy(myfontname, ANSI_VAR_FONT_NAME);
    initial_size = 8;
  } else if (strcmp(FileName, DEFAULT_GUI_FONT_FILE) == 0) {
    sprintf(LI_FONTS_FontFileName, "%s\\%s", LI_FONTS_WinFontPath, FileName);
    strcpy(myfontname, DEFAULT_GUI_FONT_NAME);
    initial_size = 8;
  } else if (strcmp(FileName, OEM_FIXED_FONT_FILE) == 0) {
    sprintf(LI_FONTS_FontFileName, "%s\\%s", LI_FONTS_WinFontPath, FileName);
    strcpy(myfontname, OEM_FIXED_FONT_NAME);
  } else if (strcmp(FileName, SYSTEM_FONT_FILE) == 0) {
    sprintf(LI_FONTS_FontFileName, "%s\\%s", LI_FONTS_WinFontPath, FileName);
    strcpy(myfontname, SYSTEM_FONT_NAME);
  } else if (strcmp(FileName, SYSTEM_FIXED_FONT_FILE) == 0) {
    sprintf(LI_FONTS_FontFileName, "%s\\%s", LI_FONTS_WinFontPath, FileName);
    strcpy(myfontname, SYSTEM_FIXED_FONT_NAME);
  } else {
    stockflag = FALSE;
  }

  if (stockflag) {
    // Stock font
    if (AddFontResource(LI_FONTS_FontFileName))  loadedflag = TRUE;
  } else {
    // Non-stock font
    sprintf(LI_FONTS_FontFileName, "%s\\%s", LI_FONTS_PrgPath, FileName);
    strncpy(myfontname, FontName, 255);
    initial_size = 20;

    if (AddFontResource(LI_FONTS_FontFileName))  loadedflag = TRUE;
    else {
      sprintf(LI_FONTS_FontFileName, "%s\\%s", LI_FONTS_WinFontPath, FileName);
      strncpy(myfontname, FontName, 255);
      if (AddFontResource(LI_FONTS_FontFileName))  loadedflag = TRUE;
    }
  }

  if (loadedflag) {
    CurrentFontFile = myfontfile;
    CurrentFontName = myfontname;
    strcpy(logicalFont.lfFaceName, myfontname);

    hDC = GetDC(LE_MAIN_HwndMainWindow);
    EnumFontFamilies(hDC, (LPCTSTR)NULL, (FONTENUMPROC)FindFontProc, (LPARAM)0);
    ReleaseDC(LE_MAIN_HwndMainWindow,hDC);

    if (fontHandle != NULL)
      DeleteObject(fontHandle);
    fontHandle = CreateFontIndirect(&logicalFont);

    LE_FONTS_ResetCharacteristics();
    LE_FONTS_SetSize(initial_size);
  } else {
    return_value = FALSE;
#if CE_ARTLIB_EnableDebugMode
    wsprintf(LE_ERROR_DebugMessageBuffer, "LE_FONTS_SetFont(): "
      "Could not add the font resource %s!\r\n\r\n", FileName);
    LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
#endif
    if (strcmp(FileName, SYSTEM_FONT_FILE) != 0)
      LE_FONTS_SetFont(SYSTEM_FONT_FILE, NULL);
  }
  return(return_value);
}
/****************************************************************************/

/*****************************************************************************
* Function:    FindFontProc
* Description: Called by EnumFontFamilies to find a font.
*****************************************************************************/
int CALLBACK FindFontProc(LOGFONT *lpelf, NEWTEXTMETRIC *lpntm, int nFontType, LPARAM lParam)
{
  if ((nFontType & DEVICE_FONTTYPE) == 0)
    if (stricmp(lpelf->lfFaceName, logicalFont.lfFaceName) == 0)
      memcpy(&logicalFont, lpelf, sizeof(LOGFONT));
  return(TRUE);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_RemoveFont
* Description: Remove the current font.
*****************************************************************************/
void LE_FONTS_RemoveFont(void)
{
  BOOL result;

  if (*LI_FONTS_FontFileName)
  {
    result = RemoveFontResource(LI_FONTS_FontFileName);
#if CE_ARTLIB_EnableDebugMode
    if (result == 0)
    {
      wsprintf(LE_ERROR_DebugMessageBuffer, "LE_FONTS_RemoveFont(): "
        "Font %s NOT successfully removed!\r\n\r\n", CurrentFontName);
      LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile);
    }
#endif
    CurrentFontFile = CurrentFontName = NULL;
  }
  if (fontHandle != NULL)
  {
    DeleteObject(fontHandle);
    fontHandle = NULL;
  }
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_ResetCharacteristics
* Description: Reset all font characteristics to the default values.
*****************************************************************************/
void LE_FONTS_ResetCharacteristics(void)
{
  logicalFont.lfEscapement    = 0;
  logicalFont.lfOrientation   = 0;
  logicalFont.lfWeight        = 400;
  logicalFont.lfItalic        = FALSE;
  logicalFont.lfUnderline     = FALSE;
  logicalFont.lfStrikeOut     = FALSE;
  logicalFont.lfOutPrecision  = OUT_TT_PRECIS;
  logicalFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  // Keep antialiasing off, since Windows may not be able
  // to choose the right colour for antialiasing pixels.
  logicalFont.lfQuality       = DRAFT_QUALITY | NONANTIALIASED_QUALITY;
  logicalFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
  if (CurrentFontName != NULL)
    strcpy(logicalFont.lfFaceName, CurrentFontName);

  if (fontHandle != NULL)
    DeleteObject(fontHandle);
  fontHandle = CreateFontIndirect(&logicalFont);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_SetSize
* Description: Changes size of the font to be the given number of pixels tall.
*              (all characters will fit in a box this many pixels tall).
*****************************************************************************/
void LE_FONTS_SetSize(int HeightInPixels)
{
  logicalFont.lfHeight = -MulDiv(HeightInPixels, LI_FONTS_LogicalScreenPixels, 72);
  // Compensate for user's selection of font size in Control Panel->Display
  // (We don't want the Windows "Large Fonts" option changing our font size)
  logicalFont.lfHeight = MulDiv(logicalFont.lfHeight, LI_FONTS_NormalLogicalScreenPixels, LI_FONTS_LogicalScreenPixels);
  logicalFont.lfWidth  = 0;

  if (fontHandle != NULL)
    DeleteObject(fontHandle);
  fontHandle = CreateFontIndirect(&logicalFont);
  LI_FONTS_CurrentSize = HeightInPixels;
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_GetSize
* Description: Return the current size of the font.
*****************************************************************************/
int LE_FONTS_GetSize(void)
{
  return(LI_FONTS_CurrentSize);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_SetWeight
* Description: Sets how bold the font is, from 0 to 1000.
*****************************************************************************/
void LE_FONTS_SetWeight(int BoldFactor)
{
  logicalFont.lfWeight = BoldFactor;
  if (fontHandle != NULL)
    DeleteObject(fontHandle);
  fontHandle = CreateFontIndirect(&logicalFont);
  LI_FONTS_CurrentWeight = BoldFactor;
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_GetWeight
* Description: Return the current weight of the font.
*****************************************************************************/
int LE_FONTS_GetWeight(void)
{
  return(LI_FONTS_CurrentWeight);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_SetPitchAndFamily
* Description: Sets the pitch and family of the font.
*              See L_Fonts.h for a list of types.
*****************************************************************************/
void LE_FONTS_SetPitchAndFamily(short pitch_flag)
{
  logicalFont.lfPitchAndFamily = DEFAULT_PITCH | pitch_flag;
  if (fontHandle != NULL)
    DeleteObject(fontHandle);
  fontHandle = CreateFontIndirect(&logicalFont);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_SetUnderline
* Description: Specify underlined or non-underlined text.
*****************************************************************************/
void LE_FONTS_SetUnderline(BOOL underline)
{
  logicalFont.lfUnderline = underline;
  if (fontHandle != NULL)
    DeleteObject(fontHandle);
  fontHandle = CreateFontIndirect(&logicalFont);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_SetItalic
* Description: Specify italic or non-italic text.
* Note that the width Windows returns for italic text seems to be
* too small.  You might have to add a space or two to your text.
*****************************************************************************/
void LE_FONTS_SetItalic(BOOL italic)
{
  logicalFont.lfItalic = italic;
  if (fontHandle != NULL)
    DeleteObject(fontHandle);
  fontHandle = CreateFontIndirect(&logicalFont);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_SetStrikeOut
* Description: Specify struck out or non-struck out text.
*****************************************************************************/
void LE_FONTS_SetStrikeOut(BOOL strikeout)
{
  logicalFont.lfStrikeOut = strikeout;
  if (fontHandle != NULL)
    DeleteObject(fontHandle);
  fontHandle = CreateFontIndirect(&logicalFont);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_GetSettings
* Description: Get a font setting previously specified with
* LE_FONTS_SetSettings (see LE_FONTS_SetSettings)
*****************************************************************************/
void LE_FONTS_GetSettings(UNS16 set)
{
  if (set >= LI_FONTS_MaxSetSize)
    return;
  memcpy(&logicalFont, &logicalFontSet[set], sizeof(LOGFONT));
  CurrentFontFile = CurrentFontFileSet[set];
  CurrentFontName = CurrentFontNameSet[set];

  LI_FONTS_CurrentSize = LI_FONTS_CurrentSizeSet[set];
  LI_FONTS_CurrentWeight = LI_FONTS_CurrentWeightSet[set];

  if (fontHandle != NULL)
    DeleteObject(fontHandle);
  fontHandle = CreateFontIndirect(&logicalFont);
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_SetSettings
* Description: Set a font setting
* Useful to keep around font settings so you don't mess up other people's
* fonts.  A reasonable convention is to designate slot 0 as the
* "scratch" slot.  So if you want to play with font settings:
*   LE_FONTS_SetSettings(0);
*   LE_FONTS_GetSettings(1);  // My magic font, previously defined
*   // font stuff that doesn't do LE_FONTS_SetSettings(0);
*   LE_FONTS_GetSettings(0);
*****************************************************************************/
void LE_FONTS_SetSettings(UNS16 set)
{
  if (set >= LI_FONTS_MaxSetSize)
    return;
  memcpy(&logicalFontSet[set], &logicalFont, sizeof(LOGFONT));
  strcpy(CurrentFontFileSet[set], CurrentFontFile);
  strcpy(CurrentFontNameSet[set], CurrentFontName);

  LI_FONTS_CurrentSizeSet[set] = LI_FONTS_CurrentSize;
  LI_FONTS_CurrentWeightSet[set] = LI_FONTS_CurrentWeight;
}
/****************************************************************************/

/*****************************************************************************
* Function:    LE_FONTS_GetLogfont
* Description: Returns the current logical font structure.
* Required by Monopoly 2 to create an HFONT to set an editbox's font to.
*****************************************************************************/
LOGFONT *LE_FONTS_GetLogfont()
{
  return(&logicalFont);
}

/*****************************************************************************
* Function:    LI_FONTS_BltText
* Description: Support function to allow blitting to an 8-bit surface.
*****************************************************************************/
void LI_FONTS_BltText(
  LPBYTE lpDestinationBits, int nXDestinationWidthInPixels,
  int nXBltStartCoordinate, int nYBltStartCoordinate,
  LPBYTE lpSourceBits,
  int nXSourceWidthInPixels, LPRECT lprSourceBltRect)
{
  LPBYTE lpDestinationBeginBlt;  // pointer to destination scanline position to begin blt
  int nXDestinationWidthInBytes; // destination bitmap width in bytes
  LPBYTE lpSourceBeginBlt;       // pointer to source scanline position to begin blt
  int nXSourceWidthInBytes;      // source bitmap width in bytes
  int nPixelsAcrossToBlt;        // blt width in pixels

  register LPBYTE lpDestinationBltPointer; // current destination blt pointer
  register LPBYTE lpSourceBltPointer;      // current source blt pointer
  register DWORD dwSourceData;             // source data
  register DWORD dwTransparentMask;        // source mask (for transparency)
  register int nLinesToBlt;                // # of scanline to blt
  register int nPixelCounter;              // pixel counter

  // calculate bitmap widths in bytes
  nXDestinationWidthInBytes = (nXDestinationWidthInPixels + 3) & 0xfffffffc;
  nXSourceWidthInBytes = (nXSourceWidthInPixels + 3) & 0xfffffffc;

  // calculate start position of blt in destination bitmap
  lpDestinationBeginBlt = lpDestinationBits +
    (nYBltStartCoordinate * nXDestinationWidthInBytes);
  lpDestinationBeginBlt += nXBltStartCoordinate;

  // Calculate start position of blt in source bitmap.
  lpSourceBeginBlt = lpSourceBits + ((lprSourceBltRect->top) * nXSourceWidthInBytes);
  lpSourceBeginBlt += lprSourceBltRect->left;

  // calculate dimensions of blt
  nLinesToBlt = lprSourceBltRect->bottom - lprSourceBltRect->top;
  nPixelsAcrossToBlt = lprSourceBltRect->right - lprSourceBltRect->left;

  // for each scanline, process nPixelsAcrossToBlt pixels
  while (nLinesToBlt > 0)
  {
    // initialize current pointers to beginning of blt line
    lpDestinationBltPointer = lpDestinationBeginBlt;
    lpSourceBltPointer = lpSourceBeginBlt;

    // initialize pixel counter
    nPixelCounter = nPixelsAcrossToBlt;
    while (nPixelCounter > 0)
    {
      // process four pixels at a time if possible
      if (nPixelCounter >= 4)
      {
        // initialize transparent mask
        dwTransparentMask = 0;

        // get four pixels of source data
        dwSourceData = *((LPLONG)lpSourceBltPointer);
        lpSourceBltPointer += 4;

        // set mask where there are zeros
        if ((dwSourceData & 0xff) == 0)       dwTransparentMask |= 0xff;
        if ((dwSourceData & 0xff00) == 0)     dwTransparentMask |= 0xff00;
        if ((dwSourceData & 0xff0000) == 0)   dwTransparentMask |= 0xff0000;
        if ((dwSourceData & 0xff000000) == 0) dwTransparentMask |= 0xff000000;

        // if there is some transparency, combine source and destination
        // data according to mask
        if (dwTransparentMask != 0)
        {
          dwSourceData &= (~dwTransparentMask);
          dwSourceData |= *((LPLONG)lpDestinationBltPointer) & dwTransparentMask;
        }
        // store result in destination bitmap
        *((LPLONG)lpDestinationBltPointer) = dwSourceData;
        lpDestinationBltPointer += 4;
        nPixelCounter -= 4;
      }
      // process two pixels at a time if possible
      else if (nPixelCounter >= 2)
      {
        // initialize transparent mask
        dwTransparentMask = 0;

        // get two pixels of source data
        dwSourceData = *((LPWORD)lpSourceBltPointer);
        lpSourceBltPointer += 2;

        // set mask where there are zeros
        if ((dwSourceData & 0xff) == 0)   dwTransparentMask |= 0xff;
        if ((dwSourceData & 0xff00) == 0) dwTransparentMask |= 0xff00;

        // if there is some transparency, combine source and destination
        // data according to mask
        if (dwTransparentMask != 0)
        {
          dwSourceData &= (~dwTransparentMask);
          dwSourceData |= *((LPWORD)lpDestinationBltPointer) & dwTransparentMask;
        }

        // store result in destination bitmap
        *((LPWORD)lpDestinationBltPointer) = (WORD)dwSourceData;
        lpDestinationBltPointer += 2;
        nPixelCounter -= 2;
      }
      // process one pixel
      else
      {
        // get source pixel
        dwSourceData = *((LPBYTE)lpSourceBltPointer);
        lpSourceBltPointer++;

        // only move pixel to destination if non-zero
        if ((dwSourceData & 0xff) != 0)
          *((LPBYTE)lpDestinationBltPointer) = (BYTE)dwSourceData;
        lpDestinationBltPointer++;
        nPixelCounter--;
      }
    }
    // finished scanline, update start scanline pointers to next scanline
    lpDestinationBeginBlt += nXDestinationWidthInBytes;
    lpSourceBeginBlt += nXSourceWidthInBytes;
    nLinesToBlt--;
  }
}
/****************************************************************************/
#endif
