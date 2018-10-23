#ifndef COMMON_H
#define COMMON_H
/*
**-----------------------------------------------------------------------------
**  File:       Common.h
**  Purpose:    Common definitions and include files
**
**-----------------------------------------------------------------------------
*/


/*
**-----------------------------------------------------------------------------
**  Include files
**-----------------------------------------------------------------------------
*/
#ifndef STRICT
	#define STRICT
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>    /* Microsoft Windows standard stuff */
#include <windowsx.h>   /* Yet more Windows stuff */
#include <commctrl.h>   /* Windows common controls */
#include <direct.h>     /* Directory handling */
#include <mmsystem.h>   /* Multimedia Windows stuff - like MIDI */
#include <vfw.h>        /* Video for Windows */
#include <tchar.h>
#include <math.h>
#include <memory.h>   // Needed for memcpy and relatives.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <time.h>

// Note:  Must Define D3D_OVERLOADS to get C++ version of D3DMATRIX
#define D3D_OVERLOADS 
#include <ddraw.h>
#include <dsound.h>     /* DirectSound audio stuff */
#include <dplay.h>      /* DirectPlay networking stuff */
#include <dinput.h>
#include <d3d.h>



/*
**-----------------------------------------------------------------------------
**  Defines
**-----------------------------------------------------------------------------
*/
#ifndef FAR
    #define FAR
#endif

#ifndef NEAR
    #define NEAR
#endif

#ifndef CONST
    #define CONST const
#endif

#ifndef MAX_PATH
    #define MAX_PATH 260
#endif

#ifndef MAX_STRING
    #define MAX_STRING 260
#endif

#ifndef NULL
    #ifdef __cplusplus
        #define NULL 0
    #else
        #define NULL ((void *)0)
    #endif
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef TRUE
    #define TRUE 1
#endif

/*
**-----------------------------------------------------------------------------
**  Typedefs
**-----------------------------------------------------------------------------
*/
typedef unsigned char Boolean;


    
/*
**-----------------------------------------------------------------------------
**  Macros
**-----------------------------------------------------------------------------
*/
#ifndef max
    #define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
    #define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef max3
    #define max3(a,b,c)         (((a) > (b)) ? (((a) > (c)) ? (a) : (c)) : (((b) > (c)) ? (b) : (c)))
#endif

#ifndef min3
    #define min3(a,b,c)         (((a) < (b)) ? (((a) < (c)) ? (a) : (c)) : (((b) < (c)) ? (b) : (c)))
#endif

#ifndef clamp
    #define clamp(L,V,U)        (((L) > (V)) ? (L) : (((U) < (V)) ? (U) : (V)))
#endif

template<class X>
inline X absolute(X of_val) {return (of_val >= 0)? of_val : -of_val;}

#define limitRange(of, lower, upper)\
	((of <= lower)? lower : ((of >= upper)? upper : of))

#define RANGE_LIMIT(of,lower,upper) limitRange(of,lower,upper)
#define ABSOLUTE_VAL(x) absolute(x)
#define MIN(x,y) ((x <= y)? x : y)
#define MAX(x,y) ((x <= y)? y : x)


// .................... define truncated values of:
// .................... 	pi 	(180 degrees),
// .................... 	pi/2 	(90 degrees),
// .................... 	3pi/2 	(270 degrees),
// .................... 	2pi 	(360 degrees)
#ifndef PIDOUBLE
	#define PIDOUBLE                3.14159265358979323846
#endif
#define PIFLOAT                ((float) PIDOUBLE)
#define PI_OVER_TWO 		(1.570796326795)
#define THREE_PI_OVER_TWO 	(4.712388980385)
#define TWO_PI			(6.28318530718)

#ifndef MAXINT
	#define MAXINT 	0X7FFFFFFF
#endif


// .................... macro to specify overloaded memory management
// .................... in any class declaration
#define OVERLOAD_MEMORY\
	void *operator new(size_t size);\
	void operator delete(void *p);\
	static int n;\
	static int d;

// .................... macro to provide basic overloaded memory management
// .................... for debugging purposes
#define OVERLOAD_MEMORY_CODE(baseclass)\
	int baseclass::n = 0;\
	int baseclass::d = 0;\
	void *baseclass::operator new(size_t size)\
		{\
		n++;\
		void *new_memory = ::operator new(size);\
		if ( new_memory )\
			return new_memory;\
		return 0;\
		}\
	void baseclass::operator delete(void *p)\
		{\
		d++;\
		delete (void *) p;\
		}

// .................... macro to conveniently allow derived class access to
// .................... protected base class operators new and delete
#define OVERLOAD_ACCESS(baseclass)\
	void *operator new(size_t size)\
		{return baseclass::operator new(size);}\
	void operator delete(void *p)\
		{baseclass::operator delete(p);}


/*
**-----------------------------------------------------------------------------
**  End of File
**-----------------------------------------------------------------------------
*/
#endif // COMMON_H


