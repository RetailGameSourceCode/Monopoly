/*******************************************************************************
*
*   FILE:           L_PRINT.C
*   DESCRIPTION:    Routines to support printing bitmaps to paper.
*
*   (C) Copyright 1998 Artech Digital Entertainments.  All rights reserved.
*
********************************************************************************/


/************************************************************/
/* INCLUDE FILES                                            */
/*----------------------------------------------------------*/

#include "l_inc.h"
#include "float.h"

#if CE_ARTLIB_EnableSystemPrint

/************************************************************/
/* GLOBAL VARIABLES                                         */
/*----------------------------------------------------------*/

BOOL	LE_PRINT_Setting_UsePrintDialog = FALSE;
			/* A flag that when TRUE will prompt the user with the standard 
			windows print dialog to select which printer to print to and various 
			other options.  When FALSE will use the windows default printer,
			if any. */
 
BOOL	LE_PRINT_Setting_Landscape = TRUE;
			/* A flag that when TRUE will print in landscape (as opposed to
			portrait) orientation.  If the LI_PRINT_Setting_UsePrintDialog is 
			TRUE then the dialog box will be set to landscape by default, 
			however, the user may change this. */

LPSTR	LE_PRINT_Setting_PrintJobName = (LPSTR)NULL;
			/* A string which names the print job to the windows print spooler. */

float	LE_PRINT_Setting_XOffsetInInches = -1.0F;
float	LE_PRINT_Setting_YOffsetInInches = -1.0F;
float	LE_PRINT_Setting_WidthInInches = -1.0F;
float	LE_PRINT_Setting_HeightInInches = -1.0F;
			/* These values control where the print image appears on the paper.
			The units are inches.  If a width/height value is too big for the page 
			then the maximum for the page will be used.  If a width/height value 
			is less than zero then the default will be used which is 3/4 of the
			width/height of the page.  Some adjustment will be made to these values 
			to preserve the aspect ratio of the screen.  If an offset value is less 
			than zero or too big then a default will be used.  The default for XOffset 
			will centre the image horizontally.  The default for YOffset will centre 
			the image vertically.  Thus the defaults result in an image centered on 
			the page covering a large part of the paper. */


/************************************************************/
/* PRIVATE GLOBALS                                          */
/*----------------------------------------------------------*/

static	BOOL			LI_PRINT_AbortPrinting;
static	HWND			LI_PRINT_AbortDialogBoxHandle;
static	WORD			*LI_PRINT_DialogTemplateDataPointer = NULL;

/************************************************************/
/* PROTOTYPES                                               */
/*----------------------------------------------------------*/

/*******************************************************************************
*
*   Name:			LI_PRINT_ParseColorMasks
*
*   Description:	Given red, green blue mask data parses it to determine shift
*					values and number of bits for each color.
*
*   Arguments:		Red/Green/BlueMask		The color masks to be parsed.
*					Red/Green/BlueShift		Pointers to ints to receive the shift
*											values of each color mask.
*					Red/Green/BlueNumBits	Pointers to ints to receive the 
*											number of bits in each color mask.
*
*******************************************************************************/
void LI_PRINT_ParseColorMasks( DWORD RedMask, DWORD GreenMask, DWORD BlueMask,
							  int *RedShift, int *GreenShift, int *BlueShift,
							  int *NumRedBits, int *NumGreenBits, int *NumBlueBits )
{
	int			i;

	*RedShift = *GreenShift = *BlueShift = 0;
	*NumRedBits = *NumGreenBits = *NumBlueBits = 0;
	
	for ( i = 0; i < 32; i++ )
	{
		if ( RedMask & (1 << i) )
		{
			(*NumRedBits)++;
			if ( *NumRedBits == 1 )
				*RedShift = i;
		}
		if ( GreenMask & (1 << i) )
		{
			(*NumGreenBits)++;
			if ( *NumGreenBits == 1 )
				*GreenShift = i;
		}
		if ( BlueMask & (1 << i) )
		{
			(*NumBlueBits)++;
			if ( *NumBlueBits == 1 )
				*BlueShift = i;
		}
	}
}

/*******************************************************************************
*
*   Name:			LE_PRINT_Blt16to24
*
*   Description:	blts from a 16 bit bitmap to a 24 bit bitmap.  Note the blt
*					mirrors the image from left to right.  This is for the
*					benefit of printers which seem to need this.
*
*   Arguments:		DestinationBitsPointer		- pointer to destination bitmap bits
*					DestinationWidthInPixels	- destination bitmap width
*					XBltStartCoord				- X coord, ul corner of blt RECT
*					YBltStartCoord				- Y coord, ul corner of blt RECT
*					SourceBitsPointer			- pointer to source bitmap bits
*					SourceWidthInPixels			- source bitmap width
*					SourceBltRect				- source blt RECT
*					RedMask,GreenMask,BlueMask  - The 16 bit color masks in effect
*                                                 for the source data.
*					FlipTopToBottom             - Flip the image vertically (useful
*                                                 for saving to Windows BMP files
*                                                 where, incredibly, this is the default)
*                   MirrorRightToLeft           - Flip image horizontally (useful
*                                                 for bitmaps being sent to a printer
*                                                 where, again incredibly, this is
*                                                 required)
*
*	Globals:		none
*
*   Returns:		none
*
*******************************************************************************/
void LE_PRINT_Blt16to24( LPBYTE DestinationBitsPointer,
							int DestinationWidthInPixels,
							int XBltStartCoord,
							int YBltStartCoord,
							LPBYTE SourceBitsPointer,
							int SourceWidthInPixels,
							LPRECT SourceBltRect,
							DWORD RedMask,
							DWORD GreenMask,
							DWORD BlueMask,
							BOOL FlipTopToBottom,
							BOOL MirrorRightToLeft )
{
	LPBYTE	DestinationBeginBlt;		// pointer to destination scanline position to begin blt
	int		DestinationWidthInBytes;	// destination bitmap width in bytes
	LPBYTE	SourceBeginBlt;				// pointer to source scanline position to begin blt
	int		SourceWidthInBytes;			// source bitmap width in bytes
	int		LinesToBlt;					// blt height in pixels
	LPBYTE	DestinationPointer;			// current position in destination	
	LPBYTE	SourcePointer;				// current position in source
	int		PixelCounter;
	int		PixelsAcrossToBlt;
	BYTE	Red,		Green,			Blue;
	int		RedShift,	GreenShift,		BlueShift;
	int		NumRedBits,	NumGreenBits,	NumBlueBits;

	// calculate bitmap widths in bytes
	DestinationWidthInBytes = ((DestinationWidthInPixels * 3) + 3) & 0xfffffffc;
	SourceWidthInBytes =  ((SourceWidthInPixels * 2) + 3) & 0xfffffffc;; 

	// calculate start position of blt in destination bitmap
	DestinationBeginBlt = DestinationBitsPointer +	(YBltStartCoord * DestinationWidthInBytes);
	DestinationBeginBlt += XBltStartCoord * 3;

	// calculate start position of blt in source bitmap
	SourceBeginBlt = SourceBitsPointer + (SourceBltRect->top * SourceWidthInBytes);
	SourceBeginBlt += SourceBltRect->left * 2;

	// calculate dimensions of blt
	LinesToBlt = SourceBltRect->bottom - SourceBltRect->top;
	PixelsAcrossToBlt = SourceBltRect->right - SourceBltRect->left;
	if ( FlipTopToBottom )
		SourceBeginBlt += LinesToBlt * SourceWidthInBytes;

	/* Now calculate color shift etc values so we can convert correctly. */

	LI_PRINT_ParseColorMasks( RedMask, GreenMask, BlueMask,
							&RedShift, &GreenShift, &BlueShift,
							&NumRedBits, &NumGreenBits, &NumBlueBits );

	/* Now actually copy and convert the 16 bit data into 24 bit data. */

	while ( LinesToBlt > 0 )
	{
		if ( MirrorRightToLeft )
			SourcePointer = SourceBeginBlt + (PixelsAcrossToBlt - 1) * 2;
		else
			SourcePointer = SourceBeginBlt;
		DestinationPointer = DestinationBeginBlt;

		PixelCounter = PixelsAcrossToBlt;
		while ( PixelCounter > 0 )
		{
			// Get the mask bit data for each r, g, b part.
			Red   = (BYTE)( ( *((DWORD*)SourcePointer) & RedMask)   >> RedShift );
			Green = (BYTE)( ( *((DWORD*)SourcePointer) & GreenMask) >> GreenShift );
			Blue  = (BYTE)( ( *((DWORD*)SourcePointer) & BlueMask)  >> BlueShift );
			// Re-calibrate the value over the larger 8-bit span needed for 24-bit.
			Red   = (BYTE)((short)Red   * 256 / (1 << NumRedBits));
			Green = (BYTE)((short)Green * 256 / (1 << NumGreenBits));
			Blue  = (BYTE)((short)Blue  * 256 / (1 << NumBlueBits));

			if ( MirrorRightToLeft )
				SourcePointer -= 2;
			else
				SourcePointer += 2;

			// B, G, R (not RGB) format.
			*DestinationPointer = Blue;
			DestinationPointer++;
			*DestinationPointer = Green;
			DestinationPointer++;
			*DestinationPointer = Red;
			DestinationPointer++;

			PixelCounter--;
		}
		if ( FlipTopToBottom )
			SourceBeginBlt -= SourceWidthInBytes;
		else
			SourceBeginBlt += SourceWidthInBytes;
		DestinationBeginBlt += DestinationWidthInBytes;
		LinesToBlt--;
	}

}

/*******************************************************************************
*
*   Name:			LI_PRINT_CalculatePrintCoords
*
*   Description:	Checks the print positioning globals and translates these
*					into pixel coords for the position of the image on the page.
*
*   Arguments:		PrinterDC				The printer's device context.
*					ImageWidthInPixels,
*					ImageHeightInPixels		Dimensions of the source bitmap.
*									
*					X, Y					Pointers to ints where the calculated
*											x/y offsets will be stored.  The offsets
*											are in printer pixel units.
*					Width, Height			Pointers to ints where the final width 
*											and height will be stored.  These values
*											are also in printer pixel units.
*
*   Returns:		Assigns coord values to the parameters passed in.
*
*******************************************************************************/
void LI_PRINT_CalculatePrintCoords( HDC PrinterDC, 
								   int	ImageWidthInPixels, int ImageHeightInPixels,
								   int *X, int *Y, int *Width, int *Height )
{
	int		PageWidthInPixels,		PageHeightInPixels,
			PagePixelsPerInchWidth,	PagePixelsPerInchHeight;
	float	PageWidthInInches,		PageHeightInInches;
	float	PageAspectRatio,		ImageAspectRatio;

	PageWidthInPixels       = GetDeviceCaps( PrinterDC, HORZRES );
	PageHeightInPixels      = GetDeviceCaps( PrinterDC, VERTRES );
	PagePixelsPerInchWidth  = GetDeviceCaps( PrinterDC, LOGPIXELSX );
	PagePixelsPerInchHeight = GetDeviceCaps( PrinterDC, LOGPIXELSY );
	PageWidthInInches       = (float)PageWidthInPixels / PagePixelsPerInchWidth;
	PageHeightInInches      = (float)PageHeightInPixels / PagePixelsPerInchHeight;
	PageAspectRatio			= (float)PageWidthInPixels / PageHeightInPixels;
	ImageAspectRatio		= (float)ImageWidthInPixels / ImageHeightInPixels;

	if ( PageWidthInPixels > PageHeightInPixels )
	{
		/* Landscape mode. */

		if ( ImageAspectRatio > PageAspectRatio )
		{
			/* Make sure the image will fit horizontally and adjust the height for 
			aspect ratio. */

			if ( LE_PRINT_Setting_WidthInInches > PageWidthInInches )
			{
				// Width is too large use maximum.
				*Width = (int)(PageWidthInInches * PagePixelsPerInchWidth);
			}
			else if ( LE_PRINT_Setting_WidthInInches < FLT_EPSILON )	// negative?
			{
				// Default is three quarter page.
				*Width = (int)(PageWidthInInches * PagePixelsPerInchWidth * 3 / 4);
			}
			else
			{
				// Width will be fine.  Convert to pixels.
				*Width = (int)(LE_PRINT_Setting_WidthInInches * PagePixelsPerInchWidth);
			}
			// Maintain aspect ratio.
			*Height = *Width * ImageHeightInPixels / ImageWidthInPixels;
		}
		else
		{
			if ( LE_PRINT_Setting_HeightInInches > PageHeightInInches )
			{
				// Height is too large use maximum.
				*Height = (int)(PageHeightInInches * PagePixelsPerInchHeight);
			}
			else if ( LE_PRINT_Setting_HeightInInches < FLT_EPSILON )	// negative?
			{
				// Default is three quarter page.
				*Height = (int)(PageHeightInInches * PagePixelsPerInchHeight * 3 / 4);
			}
			else
			{
				// Height will be fine.  Convert to pixels.
				*Height = (int)(LE_PRINT_Setting_HeightInInches * PagePixelsPerInchHeight);
			}
			// Maintain aspect ratio.
			*Width = *Height * ImageWidthInPixels / ImageHeightInPixels;
		}
	}
	else
	{
		/* Portrait mode. */

		if ( ImageAspectRatio < PageAspectRatio )
		{
			/* Make sure the image will fit vertically and adjust
			the width for aspect ratio. */

			if ( LE_PRINT_Setting_HeightInInches > PageHeightInInches )
			{
				// Height is too large use maximum.
				*Height = (int)(PageHeightInInches * PagePixelsPerInchHeight);
			}
			else if ( LE_PRINT_Setting_HeightInInches < FLT_EPSILON )	// negative?
			{
				// Default is three quarter page.
				*Height = (int)(PageHeightInInches * PagePixelsPerInchHeight * 3 / 4);
			}
			else
			{
				// Height will be fine.  Convert to pixels.
				*Height = (int)(LE_PRINT_Setting_HeightInInches * PagePixelsPerInchHeight);
			}
			// Maintain aspect ratio.
			*Width = *Height * ImageWidthInPixels / ImageHeightInPixels;
		}
		else
		{
			if ( LE_PRINT_Setting_WidthInInches > PageWidthInInches )
			{
				// Width is too large use maximum.
				*Width = (int)(PageWidthInInches * PagePixelsPerInchWidth);
			}
			else if ( LE_PRINT_Setting_WidthInInches < FLT_EPSILON )	// negative?
			{
				// Default is three quarter page.
				*Width = (int)(PageWidthInInches * PagePixelsPerInchWidth * 3 / 4);
			}
			else
			{
				// Width will be fine.  Convert to pixels.
				*Width = (int)(LE_PRINT_Setting_WidthInInches * PagePixelsPerInchWidth);
			}
			// Maintain aspect ratio.
			*Height = *Width * ImageHeightInPixels / ImageWidthInPixels;
		}
	}

	/* Adjust X,Y coords, if necessary. */

	if ( (LE_PRINT_Setting_XOffsetInInches < FLT_EPSILON)	// negative?
		 || (LE_PRINT_Setting_XOffsetInInches > PageWidthInInches) )
	{
		// Default is centered.
		*X = (PageWidthInPixels - *Width) / 2;
	}
	else
	{
		if ( LE_PRINT_Setting_XOffsetInInches * PagePixelsPerInchWidth
			  > PageWidthInPixels )
		{
			// Adjust the x offset back so the image doesn't overrun the
			// right side of the page.
			*X = PageWidthInPixels - *Width;
		}
		else
		{
			// The offset will be fine.  Just convert it to pixels.
			*X = (int)(LE_PRINT_Setting_XOffsetInInches * PagePixelsPerInchWidth);
		}
	}

	if ( (LE_PRINT_Setting_YOffsetInInches < FLT_EPSILON)	// negative?
		 || (LE_PRINT_Setting_YOffsetInInches > PageHeightInInches) )
	{
		// Default is centered.
		*Y = (PageHeightInPixels - *Height) / 2;
	}
	else
	{
		if ( LE_PRINT_Setting_YOffsetInInches * PagePixelsPerInchHeight
			  > PageHeightInPixels )
		{
			// Adjust the y offset up so the image doesn't overrun the
			// bottom of the page.
			*Y = PageHeightInPixels - *Height;
		}
		else
		{
			// The offset will be fine.  Just convert it to pixels.
			*Y = (int)(LE_PRINT_Setting_YOffsetInInches * PagePixelsPerInchHeight);
		}
	}
}

/*******************************************************************************
*
*   Name:			LI_PRINT_AbortDialog
*
*   Description:	Process specific messages from the abort dialog.  A WIN32
*					API callback function.
*
*   Arguments:		hDlg			A handle to the abort dialog window.
*					message			A windows message id.
*					wParam			Message parameter.
*					lParam			Message paramater.									
*
*   Returns:		TRUE if the message was handled, FALSE otherwise.
*
*******************************************************************************/

BOOL LI_PRINT_AbortDialog( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch (message)
   {
      case WM_INITDIALOG:
         EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE, MF_GRAYED);
         ShowWindow( hDlg, SW_SHOWNORMAL );
         return TRUE ;

      case WM_COMMAND: /* Any button activity will abort. */
         LI_PRINT_AbortPrinting = TRUE;
         DestroyWindow (hDlg);
         return TRUE;

      case WM_DESTROY:
         EnableWindow( LE_MAIN_HwndMainWindow, TRUE );
         SetFocus( LE_MAIN_HwndMainWindow );
         LI_PRINT_AbortDialogBoxHandle = NULL;
         return FALSE; /* Let normal processing continue. */
   }
   return FALSE ;
}

/*****************************************************************************
*
*   Name:			LI_PRINT_CopyAnsiToWideChar
*
*   Description:	Takes second parameter as Ansi string, copies it to first 
*					parameter as wide character (16-bits / char) string, and 
*					returns integer number of wide characters (words) in string 
*					(including the trailing wide char NULL).  Needed for 
*					dialog templates done in memory.
*
*   Arguments:		lpWCStr			A pointer to where the new wide character 
*									string can be created.
*					lpAnsiIn		A pointer to the ansi character string
*									which is to be converted.
*
*   Returns:		Returns the number of wide characters including NULL.
*
******************************************************************************/

int	LI_PRINT_CopyAnsiToWideChar( LPWORD lpWCStr, LPSTR lpAnsiIn ) 
{ 
  int   cchAnsi = lstrlen(lpAnsiIn);
  int   WideCharCount;
 
  WideCharCount = MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, lpAnsiIn, cchAnsi, lpWCStr, cchAnsi);
  lpWCStr[WideCharCount] = 0; // Add NUL for end of string.
  return WideCharCount + 1; 
} 


 
/*****************************************************************************
*
*   Name:			LI_PRINT_DWORDAlignPointer
*
*   Description:	Given a pointer, returns closest pointer that is aligned
*					on a DWORD (4 byte) boundary.  Needed for dialog templates
*					done in memory.
*
*   Arguments:		lpIn	Input pointer.
*
*   Returns:		Returns the dword aligned pointer.
*
******************************************************************************/

LPWORD LI_PRINT_DWORDAlignPointer( LPWORD lpIn ) 
{ 
	ULONG ul; 

	ul = (ULONG) lpIn; 
	ul +=3; 
	ul >>=2; 
	ul <<=2; 
	return (LPWORD) ul; 
} 

/*******************************************************************************
*
*   Name:			LI_PRINT_AbortProc
*
*   Description:	The event loop handler for the abort dialog.  A WIN32 API
*					callback function.
*
*   Arguments:		PrinterDC	The device context of the printer to which the
*								cancel dialog applies.
*					ErrorCode	A flag indicating whether an error has occurred
*								with the printer.
*
*   Returns:		TRUE if the message was handled, FALSE otherwise.
*
*******************************************************************************/

BOOL CALLBACK LI_PRINT_AbortProc( HDC PrinterDC, int ErrorCode )
{
	MSG	msg;

	while ( !ErrorCode 
			&& !LI_PRINT_AbortPrinting
			&& PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE) )
	{ 
		/* Process any messages for the Cancel dialog box. */ 

		if ( !IsDialogMessage(LI_PRINT_AbortDialogBoxHandle,(LPMSG)&msg) )
		{ 
			TranslateMessage( (LPMSG)&msg ); 
			DispatchMessage( (LPMSG)&msg ); 
		} 
	} 

	if (ErrorCode)
	  LI_PRINT_AbortPrinting = TRUE;

	return !LI_PRINT_AbortPrinting ;
}


/*****************************************************************************
*
*   Name:			LI_PRINT_CreateAbortDialog
*
*   Description:	Creates a dialog box that will be displayed while printing
*					is in progress.  The dialog will give the user the opportunity
*					to abort the printing.  The dialog box is created from a 
*					template in memory and does NOT use any resources from a
*					.rc file.
*
*   Arguments:		PrinterDC		The handle to the printer's device context.
*
*   Returns:		Returns a handle to the dialog box window.
*
******************************************************************************/

HWND LI_PRINT_CreateAbortDialog( HDC PrinterDC )
{
	HWND			AbortDialog = NULL;
	WORD			*p;
	int				CharCount;
	int				NumDlgItems = 2;
	DWORD			Style;

	// Only allocate our in memory resource data once.  The first time we need it.
	if ( !LI_PRINT_DialogTemplateDataPointer )
	{
		// Allocate more than we need to allow for dword padding.
		LI_PRINT_DialogTemplateDataPointer = (WORD *)LI_MEMORY_AllocLibraryBlock( 500 );
		if ( !LI_PRINT_DialogTemplateDataPointer )
			return NULL;

		/* Settings concerning dialog box itself. */

		p = LI_PRINT_DialogTemplateDataPointer;
		Style = WS_CAPTION | WS_VISIBLE | WS_POPUP;
		*p++ = LOWORD( Style );
		*p++ = HIWORD( Style );
		*p++ = 0;			// LOWORD dwExtendedStyle
		*p++ = 0;			// HIWORD dwExtendedStyle
		*p++ = NumDlgItems;
		*p++ = 33;			// x
		*p++ = 32;			// y
		*p++ = 160;			// w
		*p++ = 70;			// h (in dialog units)
		*p++ = 0;			// Menu
		*p++ = 0;			// Class
		// Dialog box title
		CharCount = LI_PRINT_CopyAnsiToWideChar( p, TEXT(LE_PRINT_Setting_PrintJobName) ); 
		p += CharCount;
		p = LI_PRINT_DWORDAlignPointer( p );

		/* First dialog box item is some static text. */

		Style = SS_CENTER | WS_CHILD | WS_VISIBLE;
		*p++ = LOWORD( Style );
		*p++ = HIWORD( Style );
		*p++ = 0;			// LOWORD dwExtendedStyle
		*p++ = 0;			// HIWORD dwExtendedStyle
		*p++ = 0;			// x
		*p++ = 10;			// y
		*p++ = 160;			// w
		*p++ = 8;			// h (in dialog units)
		*p++ = 0;			// IDD_TEXT
		// Fill in class i.d. by name.
		CharCount = LI_PRINT_CopyAnsiToWideChar( p, TEXT("static") ); 
		p += CharCount; 
		// The static text.
		CharCount = LI_PRINT_CopyAnsiToWideChar( p, TEXT("Now Printing.") ); 
		p += CharCount; 
		*p++ = 0;  // advance pointer over nExtraStuff WORD 

		p = LI_PRINT_DWORDAlignPointer( p ); 

		/* Second dialog box item is a cancel button. */

		Style = BS_DEFPUSHBUTTON | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
		*p++ = LOWORD( Style );
		*p++ = HIWORD( Style );
		*p++ = 0;			// LOWORD dwExtendedStyle
		*p++ = 0;			// HIWORD dwExtendedStyle
		*p++ = 60;			// x
		*p++ = 45;			// y
		*p++ = 45;			// w
		*p++ = 15;			// h (in dialog units)
		*p++ = IDCANCEL;
		// Fill in class i.d. by name.
		CharCount = LI_PRINT_CopyAnsiToWideChar( p, TEXT("button") ); 
		p += CharCount; 
		// The button text.
		CharCount = LI_PRINT_CopyAnsiToWideChar( p, TEXT("Cancel") ); 
		p += CharCount; 
		*p++ = 0;  // advance pointer over nExtraStuff WORD 
	}

	/* Set the flag used by the AbortPrintJob dialog procedure. */ 

	LI_PRINT_AbortPrinting = FALSE; 
 
	/* Create the cancel dialog. */ 
 
	AbortDialog = CreateDialogIndirect( LE_MAIN_Hinst,
										(LPDLGTEMPLATE)LI_PRINT_DialogTemplateDataPointer,
										LE_MAIN_HwndMainWindow,
										(DLGPROC)LI_PRINT_AbortDialog );

	/* Register the application's AbortProc function with GDI. */ 

	SetAbortProc( PrinterDC, LI_PRINT_AbortProc ); 

	return AbortDialog;
}


/*******************************************************************************
*
*   Name:			LI_PRINT_PrintPreparedBitmap
*
*   Description:	The internal function which handles the task of printing 
*					a bitmap to paper.  The bitmap must already be in one of 
*					the printer friendly formats.  Rejects bitmaps which do 
*					not conform.
*
*   Arguments:		lpBitmapInfo	A pointer to a BITMAPINFO struct which describes
*									in gritty detail the nature of the bitmap to be
*									printed.  
*					lpBitmapBits	A pointer to the actual bits of the bitmap.
*									The size in bytes will be determined from the
*									BITMAPINFO biSizeImage member.
*
*   Returns:		TRUE			on success
*					FALSE			on failure
*
*******************************************************************************/
BOOL LI_PRINT_PrintPreparedBitmap( LPBITMAPINFO lpBitmapInfo, LPVOID lpBitmapBits )
{
	PRINTDLG	PrintDialogParameters;
	LPDEVMODE	DevModePointer;
	LPDEVNAMES	DevNamesPointer;
	LPTSTR		DeviceName;
	DOCINFO		StartDocNames;
	HDC			PrinterDC = NULL;
	BOOL		PrintError = FALSE;
	int			PageX,		PageY,		PageWidth,		PageHeight;
	int			Width,		Height,		ScanLinesCopied;
    BOOL        NeedToHideCursor = FALSE;

#if CE_ARTLIB_EnableDebugMode
	LE_ERROR_DebugMessageBuffer[0] = 0;
#endif

#if CE_ARTLIB_ReleaseModeMessageBoxDebugging
    MessageBox(0, "LI_PRINT_PrintPreparedBitmap() Enter.", "Debugging", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif

	/* Do some basic error checking. */

	if ( lpBitmapInfo == NULL )
		goto PrintFailure;
	if ( lpBitmapBits == NULL )
		goto PrintFailure;
	// Only 24-bit bitmaps are acceptable.
	if ( lpBitmapInfo->bmiHeader.biBitCount != 24 )
		goto PrintFailure;
	Width = lpBitmapInfo->bmiHeader.biWidth;
	Height = lpBitmapInfo->bmiHeader.biHeight;
	if ( (Width < 0) || (Height < 0) )
		goto PrintFailure;

	/* Get some DEVMODE info about the default printer. */

	memset( &PrintDialogParameters, 0, sizeof(PrintDialogParameters) );
	PrintDialogParameters.lStructSize = sizeof(PrintDialogParameters);
	PrintDialogParameters.Flags = PD_RETURNDEFAULT;	// Just uses default printer (no dialog)
	if ( PrintDlg(&PrintDialogParameters) == 0 )
    {
        if (!NeedToHideCursor)
        {
            ShowCursor (TRUE);
            NeedToHideCursor = TRUE;
        }
        MessageBox (LE_MAIN_HwndMainWindow,
            "Sorry, but you need to install a printer to be able to print!",
            LE_MAIN_ApplicationNameString,
            MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
		goto PrintFailure; /* Failed somehow. */
    }

	/* Now that our DevMode struct is filled out for us, set the paper orientation,
	if possible. */

#if CE_ARTLIB_ReleaseModeMessageBoxDebugging
    MessageBox(0, "LI_PRINT_PrintPreparedBitmap() Before GlobalLock.", "Debugging", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif
	DevModePointer = (LPDEVMODE)GlobalLock( PrintDialogParameters.hDevMode );
	if ( !DevModePointer )
		goto PrintFailure;

	if ( DevModePointer->dmFields & DM_ORIENTATION )
	{
		if ( LE_PRINT_Setting_Landscape )
			DevModePointer->dmOrientation = DMORIENT_LANDSCAPE;
		else
			DevModePointer->dmOrientation = DMORIENT_PORTRAIT;
	}

	/* Now get a printer DC so we can do some real printing.  This is done
	differently depending on whether we are putting up a dialog or not. */

	if ( LE_PRINT_Setting_UsePrintDialog )
	{
		GlobalUnlock( PrintDialogParameters.hDevMode );

		PrintDialogParameters.Flags = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
		if ( PrintDlg(&PrintDialogParameters) == 0 )
			goto PrintFailure; /* Failed somehow. */

		PrinterDC = PrintDialogParameters.hDC;
	}
	else
	{
		/* Don't use a dailog box. */

		DevNamesPointer = (LPDEVNAMES)GlobalLock(PrintDialogParameters.hDevNames);
		if ( !DevNamesPointer )
		{
			GlobalUnlock( PrintDialogParameters.hDevMode );
			goto PrintFailure;
		}
		DeviceName = (LPTSTR)DevNamesPointer + DevNamesPointer->wDeviceOffset;

		PrinterDC = CreateDC( NULL, DeviceName, NULL, DevModePointer );

		GlobalUnlock( PrintDialogParameters.hDevNames );
		GlobalUnlock( PrintDialogParameters.hDevMode );
	}

	if ( PrinterDC == NULL )
		goto PrintFailure;

	/* Examine the raster capabilities of the device identified by PrinterDC to
	   verify that it supports the BitBlt function. */ 

#if CE_ARTLIB_ReleaseModeMessageBoxDebugging
    MessageBox(0, "LI_PRINT_PrintPreparedBitmap() Before GetDeviceCaps.", "Debugging", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif
	if ( !(GetDeviceCaps(PrinterDC, RASTERCAPS) & RC_BITBLT) )
	{ 
#if CE_ARTLIB_EnableDebugMode
		wsprintf( LE_ERROR_DebugMessageBuffer, "Printer cannot display bitmaps." ); 
#endif
		PrintError = TRUE;
		goto PrintFailure;
	}

	/* Display the modeless Cancel dialog box. */ 

    ShowCursor (TRUE);
    NeedToHideCursor = TRUE;
	LI_PRINT_AbortDialogBoxHandle = LI_PRINT_CreateAbortDialog( PrinterDC );

	/* Calculate our destination coords. */

	LI_PRINT_CalculatePrintCoords( PrinterDC, Width, Height, &PageX, &PageY, &PageWidth, &PageHeight );

	/* Start the print job, StartDoc returns a job number. */

	memset (&StartDocNames, 0, sizeof (StartDocNames));
	StartDocNames.cbSize = sizeof (StartDocNames);
	StartDocNames.lpszDocName = LE_PRINT_Setting_PrintJobName;
	if ( StartDoc( PrinterDC, &StartDocNames ) <= 0 )
	{
		PrintError = TRUE;
		goto PrintFailure;
	}

	if ( StartPage(PrinterDC) <= 0 )
	{
		PrintError = TRUE;
		goto PrintFailure;
	}

	SetStretchBltMode( PrinterDC, HALFTONE );
	SetMapMode( PrinterDC, MM_TEXT ); // Use pixel units.

#if CE_ARTLIB_EnableDebugMode
	wsprintf( LE_ERROR_DebugMessageBuffer, "X,Y = %d,%d    width = %d   height = %d.\r\n", PageX, PageY, PageWidth, PageHeight );
	LE_ERROR_DebugMsg( LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile );
#endif

#if CE_ARTLIB_ReleaseModeMessageBoxDebugging
    MessageBox(0, "LI_PRINT_PrintPreparedBitmap() Before StretchDIBits.", "Debugging", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif
	ScanLinesCopied = StretchDIBits( PrinterDC,
									PageX, PageY,
									PageWidth, PageHeight,
									0, 0,
									Width, Height,
									lpBitmapBits,
									lpBitmapInfo,
									DIB_RGB_COLORS, SRCCOPY);

#if CE_ARTLIB_ReleaseModeMessageBoxDebugging
    MessageBox(0, "LI_PRINT_PrintPreparedBitmap() After StretchDIBits.", "Debugging", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif
	if ( LI_PRINT_AbortPrinting )
	{
#if CE_ARTLIB_EnableDebugMode
		wsprintf( LE_ERROR_DebugMessageBuffer, "Printing cancelled." );
#endif
		PrintError = TRUE;
		goto PrintFailure;
	}
	else if ( ScanLinesCopied != Height )
	{
#if CE_ARTLIB_EnableDebugMode
		wsprintf( LE_ERROR_DebugMessageBuffer, "Error code $%lX while blitting.", (long)GetLastError() );
#endif
		PrintError = TRUE;
		goto PrintFailure;
	}

	if ( EndPage( PrinterDC ) <= 0 )
	{
		PrintError = TRUE;
		goto PrintFailure;
	}

	if ( EndDoc( PrinterDC ) <= 0 )
	{
		PrintError = TRUE;
		goto PrintFailure;	// actually is the next line, but will it always be?...
	}

PrintFailure:

	if ( PrinterDC )
	{
		if ( LI_PRINT_AbortDialogBoxHandle )
		{
			DestroyWindow( LI_PRINT_AbortDialogBoxHandle );
		}

#if CE_ARTLIB_EnableDebugMode
		if ( PrintError )
			if ( LE_ERROR_DebugMessageBuffer[0] != 0)
				LE_ERROR_DebugMsg( LE_ERROR_DebugMessageBuffer, LE_ERROR_DebugMsgToFile );
#endif
		DeleteDC( PrinterDC );
	}
	GdiFlush();
    if (NeedToHideCursor)
        ShowCursor (FALSE);
	ShowWindow( LE_MAIN_HwndMainWindow, SW_RESTORE );

#if CE_ARTLIB_ReleaseModeMessageBoxDebugging
    MessageBox(0, "LI_PRINT_PrintPreparedBitmap() Exit.", "Debugging", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif
	return !PrintError;
}


/*******************************************************************************
*
*   Name:			LE_PRINT_PrintImage
*
*   Description:	Prints bit data as a bitmap on a printer.
*
*   Arguments:		SourceBitsPointer	A pointer to the actual bits of the bitmap.
*					Width, Height		The width and height of the bitmap represented
*										by the bitmap bits.
*					BitsPerPixel		8, 16, 24, or 32.
*					PalettePointer		A pointer to palette data for indexed 8 bit mode.
*					RedMask
*					GreenMask
*					BlueMask			Mask definitions for interpretting colors in
*										pixel data where applicable.
*
*   Returns:		TRUE				on success
*					FALSE				on failure
*
*******************************************************************************/
BOOL LE_PRINT_PrintImage( LPBYTE SourceBitsPointer,
						int Width,
						int Height,
						int	BitsPerPixel,
						LPBYTE PalettePointer,
						DWORD RedMask,
						DWORD GreenMask,
						DWORD BlueMask )
{
	BOOL		Printable = TRUE;
	BOOL		PrintSuccess;
	BITMAPINFO	BitmapInfo;
	RECT		SourceRect;
	DWORD		BytesIn24BitBitmap,
				BytesIn24BitWidth;
	LPBYTE		Source,	Destination,	// Used for converting data.
				BitsPointer = NULL,			// Used in final call to print function.
				NewBitsPointer = NULL;	// Helps us remember to de-alloc anything
										// that might have gotten alloced.
	BYTE		Red,		Green,			Blue;
	int			i,	j;
	int			RedShift,	GreenShift,		BlueShift,
				NumRedBits,	NumGreenBits,	NumBlueBits;

#if CE_ARTLIB_ReleaseModeMessageBoxDebugging
    MessageBox(0, "LE_PRINT_PrintImage() Enter.", "Debugging", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif
	memset( &BitmapInfo, 0, sizeof(BITMAPINFO) );
	BitmapInfo.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth    = Width;
	BitmapInfo.bmiHeader.biHeight   = Height; 
	BitmapInfo.bmiHeader.biPlanes   = 1;
	BitmapInfo.bmiHeader.biBitCount = BitsPerPixel;

	switch ( BitsPerPixel )
	{
	case 8:
		Printable = FALSE;		/* For now. */
		break;

	case 16:

		/* First allocate enough room for 24 bit data. */

		BytesIn24BitWidth = ((Width * (24 / 8)) + 3) & 0xfffffffc;
		BytesIn24BitBitmap = Height * BytesIn24BitWidth;

#if CE_ARTLIB_ReleaseModeMessageBoxDebugging
		MessageBox(0, "LE_PRINT_PrintImage() Enter.", "Debugging", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif
		NewBitsPointer = (LPBYTE)LI_MEMORY_AllocLibraryBlock( BytesIn24BitBitmap );
#if CE_ARTLIB_ReleaseModeMessageBoxDebugging
		{
			char Buffer[100];

			wsprintf( Buffer, "LE_PRINT_PrintImage() NewBitsPointer = 0x%x.", NewBitsPointer ); 
			MessageBox(0, Buffer, "Debugging", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
		}
#endif
		if ( !NewBitsPointer )
		{
			Printable = FALSE;
			break;
		}

		SourceRect.top    = 0;
		SourceRect.left   = 0;
		SourceRect.right  = Width;
		SourceRect.bottom = Height;

		LE_PRINT_Blt16to24( NewBitsPointer, Width, 0, 0,
							SourceBitsPointer, Width, &SourceRect, 
							LE_BLT_dwRedMask, LE_BLT_dwGreenMask,	LE_BLT_dwBlueMask, FALSE, TRUE );

		BitmapInfo.bmiHeader.biCompression = BI_RGB;
		BitmapInfo.bmiHeader.biSizeImage   = BytesIn24BitBitmap;
		BitmapInfo.bmiHeader.biBitCount    = 24;	// We've now converted from 16 to 24.
		BitsPointer = NewBitsPointer;

		break;

	case 24:

		/* Do we already have our desired 24 bit format? */

		if ( (RedMask != 0x00ff0000) || (GreenMask != 0x0000ff00) || (BlueMask != 0x000000ff) )
		{
			/* Nope.  To be thorough assume nothing.  Convert the 24 bit data into
			proper r,g,b order for the printer's sake. */

			/* First allocate enough room for 24 bit data. */

			BytesIn24BitWidth = ((Width * (24 / 8)) + 3) & 0xfffffffc;
			BytesIn24BitBitmap = Height * BytesIn24BitWidth;
			NewBitsPointer = (LPBYTE)LI_MEMORY_AllocLibraryBlock( BytesIn24BitBitmap );
			if ( !NewBitsPointer )
			{
				Printable = FALSE;
				break;
			}

			/* Calculate the color masks. */

			LI_PRINT_ParseColorMasks( RedMask, GreenMask, BlueMask,
									&RedShift, &GreenShift, &BlueShift,
									&NumRedBits, &NumGreenBits, &NumBlueBits );

			/* Now actually copy and convert the data. */

			Source = SourceBitsPointer;
			Destination = NewBitsPointer;
			for ( i = 0; i < Height; i++ )
			{
				for ( j = 0; j < Width; j++ )
				{
					// Get the mask bit data for each r, g, b part.
					Red   = (BYTE)( ( *((DWORD*)Source) & RedMask)   >> RedShift );
					Green = (BYTE)( ( *((DWORD*)Source) & GreenMask) >> GreenShift );
					Blue  = (BYTE)( ( *((DWORD*)Source) & BlueMask)  >> BlueShift );
					// Re-calibrate if for some wild reason it is not 8-bits per color.
					if ( NumRedBits != 8 )
						Red = (BYTE)((short)Red * 256 / (1 << NumRedBits));
					if ( NumGreenBits != 8 )
						Green = (BYTE)((short)Green * 256 / (1 << NumGreenBits));
					if ( NumBlueBits != 8 )
						Blue = (BYTE)((short)Blue * 256 / (1 << NumBlueBits));

					Source++;
					Source++;
					Source++;

					*Destination = Blue;
					Destination++;
					*Destination = Green;
					Destination++;
					*Destination = Red;
					Destination++;
				}
				// Account for padding scanlines to DWORD alignment.
				if ( Width*3 % 4 )
				{
					for ( j = 0; j <  4 - (Width*3 % 4); j++ )
					{
						Source++;
						*Destination = 0;
						Destination++;
					}
				}
			}
			BitsPointer = NewBitsPointer;
		}
		else
		{
			// The nice case.  Use data as is.
			BitsPointer = SourceBitsPointer;
		}
		BytesIn24BitBitmap = ((Width * (24 / 8)) + 3) & 0xfffffffc;
		BytesIn24BitBitmap *= Height;
		BitmapInfo.bmiHeader.biCompression = BI_RGB;
		BitmapInfo.bmiHeader.biSizeImage   = BytesIn24BitBitmap;
		break;

	case 32:

		/* Convert 32 bit data down to 24 bit. */

		/* First allocate enough room for 24 bit data. */

		BytesIn24BitWidth = ((Width * (24 / 8)) + 3) & 0xfffffffc;
		BytesIn24BitBitmap = Height * BytesIn24BitWidth;
		NewBitsPointer = (LPBYTE)LI_MEMORY_AllocLibraryBlock( BytesIn24BitBitmap );
		if ( !NewBitsPointer )
		{
			Printable = FALSE;
			break;
		}

		/* Calculate the color masks. */

		LI_PRINT_ParseColorMasks( RedMask, GreenMask, BlueMask,
								&RedShift, &GreenShift, &BlueShift,
								&NumRedBits, &NumGreenBits, &NumBlueBits );

		/* Now actually copy and convert the data. */

		Source = SourceBitsPointer;
		Destination = NewBitsPointer;
		for ( i = 0; i < Height; i++ )
		{
			// Flip vertically AND horizontally for the printer's sake.
			Destination = (LPBYTE)(NewBitsPointer + (Height - 1 - i)*BytesIn24BitWidth);
			for ( j = 0; j < Width; j++ )
			{
				// Get the mask bit data for each r, g, b part.
				Red   = (BYTE)( ( *((DWORD*)Source) & RedMask)   >> RedShift );
				Green = (BYTE)( ( *((DWORD*)Source) & GreenMask) >> GreenShift );
				Blue  = (BYTE)( ( *((DWORD*)Source) & BlueMask)  >> BlueShift );
				// Re-calibrate if for some wild reason it is not 8-bits per color.
				if ( NumRedBits != 8 )
					Red = (BYTE)((short)Red * 256 / (1 << NumRedBits));
				if ( NumGreenBits != 8 )
					Green = (BYTE)((short)Green * 256 / (1 << NumGreenBits));
				if ( NumBlueBits != 8 )
					Blue = (BYTE)((short)Blue * 256 / (1 << NumBlueBits));

				Source += 4;

				*Destination = Blue;
				Destination++;
				*Destination = Green;
				Destination++;
				*Destination = Red;
				Destination++;
			}
			// Account for padding scanlines to DWORD alignment.
			if ( Width*3 % 4 )
			{
				for ( j = 0; j <  4 - (Width*3 % 4); j++ )
				{
					*Destination = 0;
					Destination++;
				}
			}
		}
		BytesIn24BitBitmap = ((Width * (24 / 8)) + 3) & 0xfffffffc;
		BytesIn24BitBitmap *= Height;
		BitmapInfo.bmiHeader.biCompression = BI_RGB;
		BitmapInfo.bmiHeader.biSizeImage   = BytesIn24BitBitmap;
		BitmapInfo.bmiHeader.biBitCount    = 24;	// We've now converted from 32 to 24.
		BitsPointer = NewBitsPointer;
		break;
	}

	PrintSuccess = FALSE;
	if ( Printable )
		PrintSuccess = LI_PRINT_PrintPreparedBitmap( &BitmapInfo, BitsPointer );

	if ( NewBitsPointer )
		LI_MEMORY_DeallocLibraryBlock( (LPVOID)NewBitsPointer );	// Free memory if it was allocated.

#if CE_ARTLIB_ReleaseModeMessageBoxDebugging
    MessageBox(0, "LE_PRINT_PrintImage() Exit.", "Debugging", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);
#endif
	return PrintSuccess;
}


/*******************************************************************************
*
*   Name:			LI_PRINT_InitSystem
*
*   Description:	Gives print jobs the default name equal to the application
*					name.
*
*   Arguments:		None.
*
*   Returns:		None.
*
*******************************************************************************/
void LI_PRINT_InitSystem( void )
{
	LE_PRINT_Setting_PrintJobName = LE_MAIN_ApplicationNameString;
}


/*******************************************************************************
*
*   Name:			LI_PRINT_RemoveSystem
*
*   Description:	Cleans up resources (if any) used by the print system.
*
*   Arguments:		None.
*
*   Returns:		None.
*
*******************************************************************************/
void LI_PRINT_RemoveSystem( void )
{
	if ( LI_PRINT_DialogTemplateDataPointer )
		LI_MEMORY_DeallocLibraryBlock( (LPVOID)LI_PRINT_DialogTemplateDataPointer );
}


#endif // CE_ARTLIB_EnableSystemPrint
