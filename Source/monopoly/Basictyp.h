/*************************************************************
**	basictyp.h
**
**  Declaration of basic types to avoid use of ambiguous native data types
**  which result in porting headaches like long, short, int, unsigned etc...
*/  
#if ! defined( BASICTYP_H )
	#define BASICTYP_H 
#include <limits.h>    
/*
** Names are a little weird to minimize odds of clashing with name space hogs
** like windoze
*/
typedef unsigned char IBOOL; /* integer enough for 0 and 1 */
#define ITRUE 1
#define IFALSE 0
/*
** The types
*/
typedef signed char   ICH;    /* char that behaves like a signed     */
typedef unsigned char UCH;    /* char that behaves like an unsigned  */ 
typedef char	CHR;           /* CHR: behaviour mimics standard C "char" */ 

//Original SWM types, redefed with new library.
//typedef ICH UNS8; /* 8 bit number */ 
//typedef UCH INT8;

typedef unsigned short UNS16; /* 16 bit number */
typedef signed short   INT16; 

typedef unsigned long  UNS32; /* 32 bit number */
//typedef signed long  INT32; 

/*
** Case Names for use by type portability conversion routines.
*/
#define IBOOL_TYPE_ID 1
#define ICH_TYPE_ID	 2
#define UCH_TYPE_ID	 3
#define CHR_TYPE_ID	 4
#define UNS8_TYPE_ID	 5
#define INT8_TYPE_ID	 6
#define UNS16_TYPE_ID 7
#define INT16_TYPE_ID 8
#define UNS32_TYPE_ID 9
#define INT32_TYPE_ID 10
/*
** High and low values
*/
#define ICH_MAX  SCHAR_MAX
#define ICH_MIN  SCHAR_MIN
#define UCH_MAX  UCHAR_MAX

#define INT8_MAX SCHAR_MAX
#define INT8_MIN SCHAR_MIN
#define UNS8_MAX UCHAR_MAX

#define INT16_MAX SHRT_MAX
#define INT16_MIN SHRT_MIN
#define UNS16_MAX USHRT_MAX

#define INT32_MAX LONG_MAX
#define INT32_MIN LONG_MIN
#define UNS32_MAX ULONG_MAX
/*
** Use this for passing data structure references back to caller 
** when the caller may not include the structure/typedef declaration
** E.G. when a routine must include a windows header, but doesn't 
** want to pollute client procedure's name space.
**
** A cast to PHIDDEN and back must be identical to the original variable.
*/
typedef void * PHIDDEN ; 
/*
** Generate compiler errors if data types are improperly declared for
** the compiler version
*/
#if ! (UCH_MAX == 255 ) 
	#error "Improperly declared UCH"
#endif 
#if ! (UNS8_MAX == 255) 
	#error "Improperly declared UNS8"
#endif 
#if ! (UNS16_MAX == 0xffff ) 
	#error "Improperly declared UNS16"
#endif 
#if ! (UNS32_MAX == 0xffffffff ) 
	#error "Improperly declared UNS16"
#endif 
#endif /* BASICTYP_H */