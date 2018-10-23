/*************************************************************
** unility.h
**
** Utility routines helpful with unicode/multibyte conversions
*/
#if ! defined( _UNILITY_H )
#include <stdio.h>
#include <locale.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#define _UNILITY_H

#define UNCONVERTED_CHAR  '#'
#define UNCONVERTED_WCHAR L'#'
#define MAX_WIDE_PRINT 2048      /* Increase this to suit worst case */  
#define WORST_CASE_MB  2
#define WORST_CASE_MB_SZ (MAX_WIDE_PRINT * WORST_CASE_MB)
#define CV_SZ_MAX   WORST_CASE_MB_SZ
/* 
** Wide character version of the standard lconv struct 
** (see standard runtime library localeconv() ) 
*/
typedef struct  {
	wchar_t *decimal_point;
	wchar_t *thousands_sep;
	char grouping;	 /* Treating this differently than the original lconv */
	wchar_t *int_curr_symbol;
	wchar_t *currency_symbol;
	wchar_t *mon_decimal_point;
	wchar_t *mon_thousands_sep;
	char mon_grouping; /* Treating this differently than the original lconv */
	wchar_t *positive_sign;
	wchar_t *negative_sign;
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	char p_sep_by_space;
	char n_cs_precedes;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	}
SWIDELCONV, * PSWIDELCONV;

typedef struct {
		SWIDELCONV sWideLconv ;
		wchar_t awcBytes[40]; /* space for strings pointed at by sWideLconv members */
	}
SFULLWIDELCONV, * PSFULLWIDELCONV;


#if defined( __cplusplus )
extern "C" {
#endif
SFULLWIDELCONV GetWideLocaleConversionStruct( void );
	/*
	** Returns a numeric/monetary conversion structure
	** initialized to suit the requested locale.
	** If the locale is not recognized, a default is used instead
	** This routine also contains hacks to circumvent bugs in
	** the supporting locale library calls (if and when required).
	*/

int LconvToWideLconv( PSWIDELCONV psWideLconv, struct lconv * psLconv, wchar_t * wszBuff, size_t stMaxBytes );
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


size_t WideSzToMultiByteSz( char * sz, wchar_t * wsz, size_t stMaxBytes );
	/*
	** Converts a wide character string to a multibyte character string
	** in the current LOCALE.
	**
	** The stMaxBytes argument is the size in BYTES of the target buffer
	** allow room for an additional terminating null character.
	**
	** UNCONVERTED_WCHAR ( L'#' at this writing ) will be substituted for every 
	** unconvertible character.
	**
	** Returns the number of bytes written into the target buffer,
	** not including the terminating null.
	*/

size_t MultiByteSzToWideSz( wchar_t * wsz, char * sz, size_t stMaxBytes );
	/*
	** Converts a multibyte character string in the current locale to a 
	** wide character string.
	**
	** The stMaxBytes argument is the size in BYTES of the target buffer
	** allow room for an additional terminating null wide character (i.e. 2 bytes).
	**
	** UNCONVERTED_WCHAR ( L'#' at this writing ) will be substituted for every 
	** unconvertible character.
	**
	** Returns the number of wide characters written into the target buffer,
	** not including the terminating null wide character.
	*/

/*
** BEWARE:
** The effect of the following two functions is TEMPORARY.
** CALLING ONE TWICE WILL OVERWRITE THE FIRST RESULT WITH THE SECOND.
** e.g. WidePrintf( L"%s %s\n", ConvertTempFromWideChar( sz1 ), ConvertTempFromWideChar( sz2 ));
**      will print sz2 twice.
*/
wchar_t * ConvertToTempWideSz( char * sz );
	/*
	** Converts a multibyte charater string to a unicode wide character string.
	** The conversion is LOCALE dependant.
	** 
	** UNCONVERTED_WCHAR ( L'#' at this writing ) will be substituted for every 
	** unconvertible character.
	**
	** Returns a pointer to a static buffer containing the converted string.
	**			BEWARE: subsequent calls will overwrite the buffer.
	**
	** If the passed argument is a NULL pointer, a NULL pointer is returned.
	** The converted string will be truncated to fit in MAX_WIDE_PRINT charaters.
	** Unconvertible source characters will result in an empty string.
	*/

char * ConvertTempFromWideSz( wchar_t * wsz );
	/*
	** Converts a unicode wide character string to a multibyte charater string.
	** The conversion is LOCALE dependant.
	** 
	** UNCONVERTED_WCHAR ( L'#' at this writing ) will be substituted for every 
	** unconvertible character.
	**
	** Returns a pointer to a static buffer containing the converted string.
	**			BEWARE: subsequent calls will overwrite the buffer.
	**
	** If the passed argument is a NULL pointer, a NULL pointer is returned.
	**
	** The converted string will be truncated to fit in MAX_WIDE_PRINT charaters.
	** Unconvertible source characters will result .
	*/

char * ConvertTempFromWideChar( wchar_t * wsz );
	/*
	**	Converts a unicode wide character to a multibyte charater string.
	** The conversion is LOCALE dependant.
	**
	** UNCONVERTED_WCHAR ( L'#' at this writing ) will be substituted for every 
	** unconvertible character.
	**
	** Returns a pointer to a static buffer containing the converted multi byte
	** character followed by a  null.
	**
	** If the passed argument is a NULL pointer, a NULL pointer is returned.
	**
	** Failure to convert the character will cause xxx to be output.
	*/

wchar_t * ConvertToTempWideChar( char * pc, int iMaxBytes );
	/*
	**	Converts a multibyte charater string to a unicode wide character.
	** The conversion is LOCALE dependant.
	**
	** UNCONVERTED_WCHAR ( L'#' at this writing ) will be substituted for every 
	** unconvertible character.
	**
	** Returns a pointer to a static buffer containing the converted wide character 
	** followed by a wide character null.
	**
	** If the passed argument is a NULL pointer, a NULL pointer is returned.
	** iMaxBytes denotes the maximum size in bytes of the multibyte character 
	** (usually MB_CUR_MAX).
	** Invalid or unconvertible source characters will result a wide character NULL.
	*/

int WideSprintf( char * szBuffer, wchar_t * wszFormat, ...);
	/*
	** Emulates sprintf, but accepts unicode format, strings, and characters and puts out
	** multibyte characters.
	**
	** UNCONVERTED_WCHAR ( L'#' at this writing ) will be substituted for every 
	** unconvertible character.
	**
	** returns the number of bytes written. 
	** (as opposed to the number of wide characters, or multibyte characters)
	**
	** Substitutes '#' for wide characters it cannot map to the current locale.
	**
	** Caveat:
	**		Internal buffer limit of MAX_WIDE_PRINT wide characters.
	**		It is assumed the formatted WIDECHARATER string will not exceed it.
	**		szBuffer must be large enough to accomodate formatted multibyte string
	**		resulting from the conversion. 
	*/

int WideFprintf( FILE * fp, wchar_t * wszFormat, ...);
	/*
	** Emulates fprintf, but accepts unicode format, strings, and characters and puts out
	** multibyte characters.
	**
	** UNCONVERTED_WCHAR ( L'#' at this writing ) will be substituted for every 
	** unconvertible character.
	**
	** Returns the number of bytes in the output string if file io is successful. 
	** If not, returns EOF.
	**
	** Substitutes '#' for wide characters it cannot map to the current locale.
	**
	** Caveat:
	**		Internal buffer limit of MAX_WIDE_PRINT wide characters.
	**		It is assumed the formatted WIDECHARATER string will not exceed it.
	*/


#if defined( __cplusplus )
	}
#endif
#endif /* _UNILITY_H */
