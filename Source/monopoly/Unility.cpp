/*
** unility.c
**
** Utility routines useful to help with unicode conversions
*/

#include "gameinc.h"

//  #include <assert.h>
//  #include "unility.h"

#include <locale.h>
/*
** 
** functions out of windows docs of interest in dealing with unicode/myltibyte stuff
** =================================================================================
**
** Function					Description
**
** FoldString				Translates one character string to another.
** LCMapString				Maps a character string by locale.
** ToUnicode				Translates a virtual-key code into a Unicode character.
** MultiByteToWideChar			Maps a multibyte string into a wide-character string.
** WideCharToMultiByte			Maps a wide-character string into a multibyte string.
** mbstowcs				Maps a multibyte string into a wide-character string.
** wcstombs  				Maps a wide-character string into a multibyte string.
** mbtowc					Convert a multibyte character to a wide character.
** wctomb					Converts a wide character to a multibyte character.
**
**
** ANSI C also defines the conversion functions wcstombs and 
** mbstowcs, but they can only convert to and from the character 
** set supported by the standard C library. 
**
*/

SFULLWIDELCONV sDefaultWideLconv	=
	{
	/*
	** Preinitialized to correspond to English_United States values
	*/
	&sDefaultWideLconv.awcBytes[0],
	&sDefaultWideLconv.awcBytes[2],
	(char)3,
	&sDefaultWideLconv.awcBytes[4],
	&sDefaultWideLconv.awcBytes[8],
	&sDefaultWideLconv.awcBytes[10],
	&sDefaultWideLconv.awcBytes[12],
	(char)3,
	&sDefaultWideLconv.awcBytes[14],
	&sDefaultWideLconv.awcBytes[16],
	(char)2, (char)2, (char)1, (char)0, (char)1, (char)0, (char)3, (char)0,
	L'.', L'\0',
	L',', L'\0',
	L'U', L'S', L'D', L'\0',
	L'$', L'\0',
	L'.', L'\0',
	L',', L'\0',

	L' ', L'\0',
	L'-', L'\0',
	0,0,0,0
	}	;
typedef struct lconv SLOCALECONV, * PSLOCALECONV ;

static SFULLWIDELCONV sFullWideLconv ;
SFULLWIDELCONV GetWideLocaleConversionStruct( void )
	{
	PSLOCALECONV psLocaleConv ;
	/*
	** Obtain the system's conversion structure and copy it into 
	** our private structure while converting to wide character set form.
	*/
	psLocaleConv = localeconv();
	(void) LconvToWideLconv( &sFullWideLconv.sWideLconv, 
										psLocaleConv, 
										&sFullWideLconv.awcBytes[0], 
										sizeof(sFullWideLconv.awcBytes) - sizeof( wchar_t ) 
										) ;
	/*
	** Fixup anything that needs fixing. (i.e. hack away)
	*/
	return sFullWideLconv;
	}

int LconvToWideLconv( PSWIDELCONV psWideLconv, struct lconv * psLconv, wchar_t * wszBuff, size_t stMaxBytes )
	/*
	** Converts a multibyte lconv struct (as returned from localeconv()) to a wide 
	** character version of it.
	**
	** All char * members are treated as null terminated strings.
	** They are converted to wide character strings in space taken from
	** wszBuff.
	**
	** UNCONVERTED_WCHAR ( L'#' at this writing ) is substituted for every 
	** character which fails to convert.
	**
	** If there is insufficient space to accomodate all the converted strings,
	** members which were not converted will all point to the same string
	** consisting of an UNCONVERTED_WCHAR followed by a WIDE NULL.
	** 
	** Returns the number of wide characters used out of wszBuff, including all
	**				NULL's.
	** Returns 0 if there was insufficient space to accomodate any strings at all
	**				and the conversion was aborted.
	**
	*/
	{
	wchar_t * wszDefault ;
	wchar_t * wszField ;
	int iLen ;
	stMaxBytes /= sizeof( wchar_t ); /* Convert to a count of wide characters */
	if ( stMaxBytes < 2 )
		return 0;
	wszDefault = wszBuff ;
	* wszBuff++ = L'#' ; stMaxBytes --; 
	* wszBuff++ = L'\0'; stMaxBytes --;

	psWideLconv->int_frac_digits = psLconv->int_frac_digits;
	psWideLconv->frac_digits = psLconv->frac_digits;
	psWideLconv->p_cs_precedes = psLconv->p_cs_precedes;
	psWideLconv->p_sep_by_space = psLconv->p_sep_by_space;
	psWideLconv->n_cs_precedes = psLconv->n_cs_precedes;
	psWideLconv->n_sep_by_space = psLconv->n_sep_by_space;
	psWideLconv->p_sign_posn = psLconv->p_sign_posn;
	psWideLconv->n_sign_posn = psLconv->n_sign_posn;
	/*
	** Treating these two a little differently in wide version.
	** In lconv they are indirect, here they are not.
	*/
	psWideLconv->grouping = * psLconv->grouping;
	psWideLconv->mon_grouping = * psLconv->mon_grouping;
	/*
	** 
	*/
	psWideLconv->decimal_point = wszDefault ;
	psWideLconv->thousands_sep = wszDefault ;
	psWideLconv->int_curr_symbol = wszDefault ;
	psWideLconv->currency_symbol = wszDefault ;

	psWideLconv->mon_decimal_point = wszDefault ;
	psWideLconv->mon_thousands_sep = wszDefault ;
	psWideLconv->positive_sign = wszDefault ;
	psWideLconv->negative_sign = wszDefault ;
	/*
	** Convert Each indirect field to wide character strings
	*/
#define DO_ONE_FIELD( name ) /* Local macro to save duplication */ \
	{\
	wszField = ConvertToTempWideSz( psLconv->name ) ; \
	iLen = wcslen(wszField);\
	if( iLen < (int) stMaxBytes )\
		{		\
		psWideLconv->name = wszBuff ;\
		stMaxBytes -= iLen ;\
		stMaxBytes -- ; /* One for the null */	\
		(void) wcscpy( wszBuff, wszField);\
		wszBuff += iLen ;\
		wszBuff ++ ; /* past the null too */\
		}\
	else	\
		return wszBuff - wszDefault; /* Let the rest all point to default */ \
	}/* End of DO_ONE_FIELD MACRO */
	DO_ONE_FIELD(  decimal_point ) ;
	DO_ONE_FIELD(  thousands_sep ) ;
	DO_ONE_FIELD(  int_curr_symbol ) ;
	DO_ONE_FIELD(  currency_symbol ) ;
	DO_ONE_FIELD(  mon_decimal_point ) ;
	DO_ONE_FIELD(  mon_thousands_sep ) ;
	DO_ONE_FIELD(  positive_sign ) ;
	DO_ONE_FIELD(  negative_sign ) ;
	/*
	** Hack to fix the screwed up thousands separator on french numbers
	*/
	if( * (psWideLconv->decimal_point) == L',' )
		{
		* (psWideLconv->thousands_sep) = L'.' ;
		* (psWideLconv->mon_thousands_sep) = L'.' ;
		}
	/*
	** Hack ends
	*/
	return wszBuff - wszDefault;
#undef DO_ONE_FIELD /* macro is local to this procedure */
	}


static size_t SlowWsToMbs( char * sz, wchar_t * wsz, size_t stLength, size_t stMaxBytes )
	/*
	** Slow version of Wide character to Multi Byte string conversion.
	**
	** Attempts to convert stLength wide characters.
	** Converts each unconvertible character in the source as id it were UNCONVERTED_WCHAR.
	** Truncates at stMaxBytes output string size (in bytes).
	** Returns the number of bytes in the output string.
	** Leaves it to the caller to insert the trailing null after this call.
	** Although it uses the spot reserved for the trailing null (after stMaxBytes) 
	** for an overflow byte.
	*/
	{
	char * tsz ;
	int iBytesUsed ;
	assert( wsz && sz ); /* No NULL pointers allowed */
	for( tsz = sz ; stLength  ; wsz++, stLength--, tsz+= iBytesUsed   )
		{
		if( ! stMaxBytes )
			break; /* Truncate */
		iBytesUsed = wctomb( tsz, * wsz );
		if ( iBytesUsed > (int)stMaxBytes )
			{
			/*
			** We overran sz, but only into the spot for the nul character.
			** (assuming MB_CUR_MAX <= 2 )
			** So never mind and Truncate before the overflow byte.
			*/
			break;
			}
		if ( iBytesUsed < 1 )
			{
			/*
			** substitute the Marker character.
			*/
			iBytesUsed = wctomb( tsz, UNCONVERTED_WCHAR ) ;
			}
		if ( iBytesUsed < 1 )
			{
			/*
			** the marker is unconvertible
			*/
			iBytesUsed = 0; /* skip it instead */
			}
		stMaxBytes -= iBytesUsed ; /* decrement truncation count down */
		}
	return tsz - sz ;
	}

static size_t SlowMbsToWs( wchar_t * wsz, char * sz, size_t stLength, size_t stMaxBytes )
	{
	size_t stIndex, stOut ;
	int iBytesUsed ;
	/*
	** Slow version of Multi Byte to Wide character string conversion.
	** Inserts UNCONVERTED_WCHAR for each unconvertible character encountered
	** in the source string.
	*/
	assert( wsz && sz ); /* No NULL pointers allowed */
	for( stIndex = 0, stOut = 0 ; stIndex < stLength ; wsz++, stOut ++  )
		{
		if( stMaxBytes < sizeof( wchar_t ) )
			break; /* Truncate */
		stMaxBytes -= sizeof( wchar_t ) ; /* decrement truncation count down */
		iBytesUsed = mbtowc( wsz, (sz+stIndex), MB_CUR_MAX );
		if ( iBytesUsed < 0)
			{
			* wsz = UNCONVERTED_WCHAR ;/* Mark as uncoverted char */
			stIndex++ ;
			}
		else if ( iBytesUsed == 0 )
			{
			/*
			** What can't possibly happen, did.
			*/
			break;
			}
		else 
			{
			stIndex += iBytesUsed ;
			}
		}
	return stOut ;
	}

size_t WideSzToMultiByteSz( char * sz, wchar_t * wsz, size_t stMaxBytes )
	{
	size_t stBytes;			/* Number of bytes in result */
	size_t stWideChars ;		/* Number of characters in source */
	assert( wsz && sz ); /* NO null pointers allowed */
	stBytes = wcstombs( sz, wsz, stMaxBytes ); /* Do'em fast */
	if ( stBytes == (size_t) (-1) )
		{
		/*
		** Unhappy with one of them
		*/
		stWideChars = wcslen( wsz );
		stBytes = SlowWsToMbs( sz, wsz, stWideChars, stMaxBytes ); /* Do'em slow */
		}
	* (sz + stBytes) = (char) 0; /* Do trailing Null */
	return stBytes;
	}

size_t MultiByteSzToWideSz( wchar_t * wsz, char * sz, size_t stMaxBytes )
	{
	size_t stMultiBytes; /* Number of multibyte characters */
	size_t stBytes ;		/* Number of bytes */
	assert( wsz && sz ); /* NO null pointers allowed */
	stBytes = strlen( sz );
	if( stBytes )
		{
		stMultiBytes = mbstowcs( (wchar_t *) 0, sz, stBytes ); /* Count'em fast */
		if ( stMultiBytes == (size_t) (-1) || stMultiBytes > (stMaxBytes/sizeof(wchar_t)) )
			{
			stMultiBytes = SlowMbsToWs( wsz, sz, stBytes, stMaxBytes ); /* Do'em slow */
			}
		else
			{
			(void) mbstowcs( wsz, sz, stBytes ); /* Do'em fast */
			}
		wsz += stMultiBytes ;
		}
	else 
		stMultiBytes = 0 ;
	* wsz = (wchar_t) 0; /* Do trailing Null */
	return stMultiBytes ;
	}

wchar_t * ConvertToTempWideSz( char * sz )
	{
	static wchar_t wszTempWide[MAX_WIDE_PRINT+1];
	if( ! sz )
		return (wchar_t *) 0; /* Null for Null */
	MultiByteSzToWideSz( wszTempWide, sz, sizeof(wszTempWide) - sizeof(wchar_t) );
	return wszTempWide ;
	}

wchar_t * ConvertToTempWideChar( char * pc, int iMaxBytes )
	{
	static wchar_t wszTempWide[] = {(wchar_t)NULL,(wchar_t)NULL }; /* Trailing Null too */
	int i ;
	if ( ! pc )
		return (wchar_t *) 0 ; /* Null for Null */
	if ( (i = mbtowc( wszTempWide, pc, iMaxBytes )) < 0)
		{
		wszTempWide[0] = UNCONVERTED_WCHAR;/* Invalid .. return null character*/
		}
	else if ( i == 0 )
		wszTempWide[0] = (wchar_t) 0;	
	else
		; /* it's in there */
	return wszTempWide;
	}

char * ConvertTempFromWideSz( wchar_t * wsz )
	{
	static char szTempMb[WORST_CASE_MB_SZ+1];
	WideSzToMultiByteSz( szTempMb, wsz, WORST_CASE_MB_SZ );
	return szTempMb ;
	}

char * ConvertTempFromWideChar( wchar_t * wsz )
	{
	int i;
	static char szTempMb[3]; /* One extra to append trailing nul */
	if( ! wsz )
		return (char *)0; /* Null for Null */
	i = wctomb( szTempMb, * wsz ) ;
	if ( i < 1 )
		{
		i = wctomb( szTempMb, UNCONVERTED_WCHAR );
		}
	if ( i < 1 ) 
		{
		szTempMb[0] = (char) 0;
		i = 1 ;
		}
	szTempMb[i] = (char) 0; /* A trailing null, in case it is used like a string */
	return szTempMb;
	}


int WideSprintf( char * szBuffer, wchar_t * wszFormat, ...)
	{
	/*
	** Emulates sprintf, but accepts unicode format, strings, and characters and puts out
	** multibyte characters.
	**
	** returns the number of bytes written. 
	** (as opposed to the number of wide characters, or multibyte characters)
	** negative means error
	**
	** Substitutes '#' for wide characters it cannot map to the current locale.
	**
	** Internal buffer limit of MAX_WIDE_PRINT wide characters.
	** szBuffer must be large enough to accomodate formatted multibyte string
	** resulting from the conversion. 
	*/
	static wchar_t wszBuffer[MAX_WIDE_PRINT+1];
	size_t stCount;
	va_list args;
	va_start( args, wszFormat );
	vswprintf( wszBuffer, wszFormat, args);
	va_end(args);
	stCount = WideSzToMultiByteSz(szBuffer, wszBuffer, WORST_CASE_MB_SZ );
	return (int) stCount ;
	}

int WideFprintf( FILE * fp, wchar_t * wszFormat, ...)
	{
	static wchar_t wszBuffer[MAX_WIDE_PRINT+1];
	static char		szBuffer[WORST_CASE_MB_SZ+1];
	size_t stCount ;
	int iRet ;
	va_list args;
	va_start( args, wszFormat );
	vswprintf( wszBuffer, wszFormat, args);
	va_end(args);
	stCount = WideSzToMultiByteSz(szBuffer, wszBuffer, WORST_CASE_MB_SZ );
	iRet = fputs( szBuffer, fp );
	if( iRet < 0 )
		return iRet;
	else
		return (int) stCount ;
	}
