
/*  A Bison parser, made from parser.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	IDENTIFIER	258
#define	DEF	259
#define	USE	260
#define	PROTO	261
#define	EXTERNPROTO	262
#define	TO	263
#define	IS	264
#define	ROUTE	265
#define	SFN_NULL	266
#define	EVENTIN	267
#define	EVENTOUT	268
#define	FIELD	269
#define	EXPOSEDFIELD	270
#define	SFBOOL	271
#define	SFCOLOR	272
#define	SFFLOAT	273
#define	SFIMAGE	274
#define	SFINT32	275
#define	SFNODE	276
#define	SFROTATION	277
#define	SFSTRING	278
#define	SFTIME	279
#define	SFVEC2F	280
#define	SFVEC3F	281
#define	MFCOLOR	282
#define	MFFLOAT	283
#define	MFINT32	284
#define	MFROTATION	285
#define	MFSTRING	286
#define	MFVEC2F	287
#define	MFVEC3F	288
#define	MFNODE	289



//
// Parser for VRML 2.0 files.
// This is a minimal parser that does NOT generate an in-memory scene graph.
// 

// The original parser was developed on a Windows 95 PC with
// Borland's C++ 5.0 development tools.  This was then ported
// to a Windows 95 PC with Microsoft's MSDEV C++ 4.0 development
// tools.  The port introduced the ifdef's for
//    USING_BORLAND_CPP_5          : since this provides a "std namespace",
//    TWO_ARGUMENTS_FOR_STL_STACK  : STL is a moving target.  The stack template
//                                     class takes either one or two arguments.

#include <iostream.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "stldefs.h"
#include "vrml.h"

#include "VrmlNodeType.h"

#include "debug.h"

#define YYDEBUG 1


// Currently-being-define proto.  Prototypes may be nested, so a stack
// is needed:

CvectList<VrmlNodeType*> currentProtoStack;


CvectList< FieldRec* > currentField;

// This is used when the parser knows what kind of token it expects
// to get next-- used when parsing field values (whose types are declared
// and read by the parser) and at certain other places:
extern int expectToken;

// .......... signal that next node is to be named via DEF keyword
#define MAX_DEF_IDENTIFIER_LEN 255
char nameNextNode[MAX_DEF_IDENTIFIER_LEN+1];
vrmlNode *lastNodeExited;

// Current line number (set by lexer)
extern int currentLineNumber;

// Some helper routines defined below:
void beginProto(const char *);
void endProto();
int addField(const char *type, const char *name);
int addEventIn(const char *type, const char *name);
int addEventOut(const char *type, const char *name);
int addExposedField(const char *type, const char *name);
int add(void (VrmlNodeType::*)(const char *, int), const char *, const char *);
int fieldType(const char *type);
void useNode(const char *);
void defNode();
void enterNode(const char *);
void exitNode();
void inScript();
void enterField(const char *);
void exitField();
void expect(int type);


#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		126
#define	YYFLAG		-32768
#define	YYNTBASE	40

#define YYTRANSLATE(x) ((unsigned)(x) <= 289 ? yytranslate[x] : 66)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,    39,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    35,     2,    36,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    37,     2,    38,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     3,     6,     8,    10,    12,    14,    18,    21,
    23,    25,    26,    36,    37,    38,    47,    48,    51,    55,
    59,    60,    66,    67,    73,    74,    77,    81,    85,    89,
    93,   102,   103,   109,   110,   113,   114,   118,   120,   122,
   126,   130,   131,   137,   143,   149,   151,   153,   155,   157,
   159,   161,   163,   165,   167,   169,   171,   173,   175,   177,
   179,   181,   183,   186,   189,   192,   195,   199,   201,   202
};

static const short yyrhs[] = {    41,
     0,     0,    41,    42,     0,    43,     0,    44,     0,    56,
     0,    57,     0,     4,     3,    57,     0,     5,     3,     0,
    45,     0,    47,     0,     0,     6,     3,    46,    35,    50,
    36,    37,    41,    38,     0,     0,     0,     7,     3,    48,
    35,    54,    36,    49,    63,     0,     0,    50,    51,     0,
    12,     3,     3,     0,    13,     3,     3,     0,     0,    14,
     3,     3,    52,    63,     0,     0,    15,     3,     3,    53,
    63,     0,     0,    54,    55,     0,    12,     3,     3,     0,
    13,     3,     3,     0,    14,     3,     3,     0,    15,     3,
     3,     0,    10,     3,    39,     3,     8,     3,    39,     3,
     0,     0,     3,    58,    37,    59,    38,     0,     0,    59,
    60,     0,     0,     3,    61,    63,     0,    56,     0,    44,
     0,    12,     3,     3,     0,    13,     3,     3,     0,     0,
    14,     3,     3,    62,    63,     0,    12,     3,     3,     9,
     3,     0,    13,     3,     3,     9,     3,     0,    16,     0,
    17,     0,    27,     0,    18,     0,    28,     0,    19,     0,
    20,     0,    29,     0,    22,     0,    30,     0,    23,     0,
    31,     0,    24,     0,    25,     0,    32,     0,    26,     0,
    33,     0,    21,    43,     0,    21,    11,     0,    34,    64,
     0,     9,     3,     0,    35,    65,    36,     0,    43,     0,
     0,    65,    43,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   114,   117,   119,   122,   124,   125,   128,   130,   131,   134,
   136,   139,   141,   145,   147,   149,   151,   153,   156,   159,
   161,   163,   164,   166,   169,   171,   174,   177,   179,   181,
   185,   190,   192,   195,   197,   200,   202,   203,   204,   207,
   208,   209,   212,   213,   215,   219,   221,   222,   223,   224,
   225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
   235,   236,   238,   239,   240,   241,   244,   246,   249,   251
};

static const char * const yytname[] = {   "$","error","$undefined.","IDENTIFIER",
"DEF","USE","PROTO","EXTERNPROTO","TO","IS","ROUTE","SFN_NULL","EVENTIN","EVENTOUT",
"FIELD","EXPOSEDFIELD","SFBOOL","SFCOLOR","SFFLOAT","SFIMAGE","SFINT32","SFNODE",
"SFROTATION","SFSTRING","SFTIME","SFVEC2F","SFVEC3F","MFCOLOR","MFFLOAT","MFINT32",
"MFROTATION","MFSTRING","MFVEC2F","MFVEC3F","MFNODE","'['","']'","'{'","'}'",
"'.'","vrmlscene","declarations","declaration","nodeDeclaration","protoDeclaration",
"proto","@1","externproto","@2","@3","interfaceDeclarations","interfaceDeclaration",
"@4","@5","externInterfaceDeclarations","externInterfaceDeclaration","routeDeclaration",
"node","@6","nodeGuts","nodeGut","@7","@8","fieldValue","mfnodeValue","nodes",
""
};
#endif

static const short yyr1[] = {     0,
    40,    41,    41,    42,    42,    42,    43,    43,    43,    44,
    44,    46,    45,    48,    49,    47,    50,    50,    51,    51,
    52,    51,    53,    51,    54,    54,    55,    55,    55,    55,
    56,    58,    57,    59,    59,    61,    60,    60,    60,    60,
    60,    62,    60,    60,    60,    63,    63,    63,    63,    63,
    63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
    63,    63,    63,    63,    63,    63,    64,    64,    65,    65
};

static const short yyr2[] = {     0,
     1,     0,     2,     1,     1,     1,     1,     3,     2,     1,
     1,     0,     9,     0,     0,     8,     0,     2,     3,     3,
     0,     5,     0,     5,     0,     2,     3,     3,     3,     3,
     8,     0,     5,     0,     2,     0,     3,     1,     1,     3,
     3,     0,     5,     5,     5,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     2,     2,     2,     3,     1,     0,     2
};

static const short yydefact[] = {     2,
     1,    32,     0,     0,     0,     0,     0,     3,     4,     5,
    10,    11,     6,     7,     0,     0,     9,    12,    14,     0,
    34,     8,     0,     0,     0,     0,    17,    25,     0,    36,
     0,     0,     0,    33,    39,    38,    35,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    18,
     0,     0,     0,     0,    15,    26,     0,     0,    46,    47,
    49,    51,    52,     0,    54,    56,    58,    59,    61,    48,
    50,    53,    55,    57,    60,    62,     0,    37,    40,    41,
    42,     0,     0,     0,     0,     2,     0,     0,     0,     0,
     0,     0,    66,    64,    63,    69,    68,    65,     0,     0,
     0,    19,    20,    21,    23,     0,    27,    28,    29,    30,
    16,    31,     0,    44,    45,    43,     0,     0,    13,    67,
    70,    22,    24,     0,     0,     0
};

static const short yydefgoto[] = {   124,
     1,     8,     9,    10,    11,    23,    12,    24,    91,    38,
    50,   117,   118,    39,    56,    13,    14,    15,    26,    37,
    41,   101,    78,    98,   113
};

static const short yypact[] = {-32768,
    79,-32768,    -1,     0,     3,     4,     6,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   -23,    18,-32768,-32768,-32768,   -10,
-32768,-32768,     5,     7,    32,    -2,-32768,-32768,    33,-32768,
    40,    41,    45,-32768,-32768,-32768,-32768,    19,    66,    48,
    43,    50,    51,    54,    84,    85,    87,    88,    55,-32768,
    90,    91,    92,    93,-32768,-32768,    58,    95,-32768,-32768,
-32768,-32768,-32768,    34,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    23,-32768,    94,    96,
-32768,    97,    98,   101,   103,-32768,   104,   105,   106,   107,
    43,   108,-32768,-32768,-32768,-32768,-32768,-32768,   109,   110,
    43,-32768,-32768,-32768,-32768,    12,-32768,-32768,-32768,-32768,
-32768,-32768,    20,-32768,-32768,-32768,    43,    43,-32768,-32768,
-32768,-32768,-32768,    99,   114,-32768
};

static const short yypgoto[] = {-32768,
    29,-32768,   -64,   100,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   102,   111,-32768,-32768,-32768,
-32768,-32768,   -71,-32768,-32768
};


#define	YYLAST		128


static const short yytable[] = {    95,
    30,    16,    17,     5,     6,    18,    19,     7,    20,    31,
    32,    33,    97,    21,     2,     3,     4,     5,     6,   111,
     2,     7,     2,     3,     4,     2,     3,     4,    25,   116,
    45,    46,    47,    48,    29,    34,     2,     3,     4,    27,
    40,    28,    42,    43,    94,   122,   123,    44,   121,   119,
    57,    58,    79,    80,    49,   120,    81,    96,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
    71,    72,    73,    74,    75,    76,    77,    51,    52,    53,
    54,     2,     3,     4,     5,     6,    82,    83,     7,    84,
    85,    86,    87,    88,    89,    90,    92,    93,   125,   102,
   103,    55,    99,   104,   100,   105,   107,   108,   109,   110,
   112,   114,   115,   126,   106,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    35,    22,    36
};

static const short yycheck[] = {    64,
     3,     3,     3,     6,     7,     3,     3,    10,     3,    12,
    13,    14,    77,    37,     3,     4,     5,     6,     7,    91,
     3,    10,     3,     4,     5,     3,     4,     5,    39,   101,
    12,    13,    14,    15,     3,    38,     3,     4,     5,    35,
     8,    35,     3,     3,    11,   117,   118,     3,   113,    38,
     3,     9,     3,     3,    36,    36,     3,    35,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    12,    13,    14,
    15,     3,     4,     5,     6,     7,     3,     3,    10,     3,
     3,    37,     3,     3,     3,     3,    39,     3,     0,     3,
     3,    36,     9,     3,     9,     3,     3,     3,     3,     3,
     3,     3,     3,     0,    86,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    26,    16,    26
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */


/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

extern int yylex();
extern void yyerror(const char *);

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) || defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(YYPARSE_PARAM)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;

// .......... initialize signal that next node is to be named
nameNextNode[0] = '\0';
lastNodeExited = 0;

int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 8:
// .......... initialize signal that next node is to be named
{ assert (strlen(yyvsp[-1].string) < MAX_DEF_IDENTIFIER_LEN); strcpy( nameNextNode, yyvsp[-1].string ); defNode(); free(yyvsp[-1].string); ;
    break;}
case 9:
{ useNode( yyvsp[0].string ); free(yyvsp[0].string); ;
    break;}
case 12:
{ beginProto(yyvsp[0].string); ;
    break;}
case 13:
{ endProto();  free(yyvsp[-7].string);;
    break;}
case 14:
{ beginProto(yyvsp[0].string); ;
    break;}
case 15:
{ expect(MFSTRING); ;
    break;}
case 16:
{ endProto();  free(yyvsp[-6].string); ;
    break;}
case 19:
{ addEventIn(yyvsp[-1].string, yyvsp[0].string);
        									  free(yyvsp[-1].string); free(yyvsp[0].string); ;
    break;}
case 20:
{ addEventOut(yyvsp[-1].string, yyvsp[0].string);
        									  free(yyvsp[-1].string); free(yyvsp[0].string); ;
    break;}
case 21:
{ int type = addField(yyvsp[-1].string, yyvsp[0].string);
    										  expect(type); ;
    break;}
case 22:
{ free(yyvsp[-3].string); free(yyvsp[-2].string); ;
    break;}
case 23:
{ int type = addExposedField(yyvsp[-1].string, yyvsp[0].string);
    										  expect(type); ;
    break;}
case 24:
{ free(yyvsp[-3].string); free(yyvsp[-2].string); ;
    break;}
case 27:
{ addEventIn(yyvsp[-1].string, yyvsp[0].string);
        									  free(yyvsp[-1].string); free(yyvsp[0].string); ;
    break;}
case 28:
{ addEventOut(yyvsp[-1].string, yyvsp[0].string);
        									  free(yyvsp[-1].string); free(yyvsp[0].string); ;
    break;}
case 29:
{ addField(yyvsp[-1].string, yyvsp[0].string);
        									  free(yyvsp[-1].string); free(yyvsp[0].string); ;
    break;}
case 30:
{ addExposedField(yyvsp[-1].string, yyvsp[0].string);
        									  free(yyvsp[-1].string); free(yyvsp[0].string); ;
    break;}
case 31:
{ free(yyvsp[-6].string); free(yyvsp[-4].string); free(yyvsp[-2].string); free(yyvsp[0].string); ;
    break;}
case 32:
{ enterNode(yyvsp[0].string); ;
    break;}
case 33:
{ exitNode(); free(yyvsp[-4].string);;
    break;}
case 36:
{ enterField(yyvsp[0].string); ;
    break;}
case 37:
{ exitField(); free(yyvsp[-2].string); ;
    break;}
case 40:
{ inScript(); free(yyvsp[-1].string); free(yyvsp[0].string); ;
    break;}
case 41:
{ inScript(); free(yyvsp[-1].string); free(yyvsp[0].string); ;
    break;}
case 42:
{ inScript(); 
    										  int type = fieldType(yyvsp[-1].string);
    										  expect(type); ;
    break;}
case 43:
{ free(yyvsp[-3].string); free(yyvsp[-2].string); ;
    break;}
case 44:
{ inScript(); free(yyvsp[-3].string); free(yyvsp[-2].string); free(yyvsp[0].string); ;
    break;}
case 45:
{ inScript(); free(yyvsp[-3].string); free(yyvsp[-2].string); free(yyvsp[0].string); ;
    break;}
case 66:
{ free(yyvsp[0].string); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */

  
  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}


void
yyerror(const char *msg)
{
    char report[128];
    strcpy(report, "VRML parse error near line ");
    itoa(currentLineNumber,report+strlen(report),10);

    MSG(msg,report);
    expect(0);
}

static void
beginProto(const char *protoName)
{
	// Any protos in the implementation are in a local namespace:
    VrmlNodeType::pushNameSpace();

	VrmlNodeType *t = new VrmlNodeType(protoName);
    currentProtoStack.push(t);
}

static void
endProto()
{
	// Make any protos defined in implementation unavailable:
	VrmlNodeType::popNameSpace();

    // Add this proto definition:
	if (currentProtoStack.isEmpty()) {
    	cerr << "Error: Empty PROTO stack!\n";
    }
    else {
    	VrmlNodeType *t = currentProtoStack.getTop();
    	currentProtoStack.pop();
        VrmlNodeType::addToNameSpace(t);
    }
}

int
addField(const char *type, const char *name)
{
	return add(&VrmlNodeType::addField, type, name);
}

int
addEventIn(const char *type, const char *name)
{
	return add(&VrmlNodeType::addEventIn, type, name);
}
int
addEventOut(const char *type, const char *name)
{
	return add(&VrmlNodeType::addEventOut, type, name);
}
int
addExposedField(const char *type, const char *name)
{
	return add(&VrmlNodeType::addExposedField, type, name);
}

int
add(void (VrmlNodeType::*func)(const char *, int), 
	const char *typeString, const char *name)
{
	int type = fieldType(typeString);

    if (type == 0) {
    	cerr << "Error: invalid field type: " << type << "\n";
    }

    // Need to add support for Script nodes:
	// if (inScript) ... ???

	if (currentProtoStack.isEmpty()) {
    	cerr << "Error: declaration outside of prototype\n";
        return 0;
    }
    VrmlNodeType *t = currentProtoStack.getTop();
    (t->*func)(name, type);

return type;
}

int
fieldType(const char *type)
{
    if (strcmp(type, "SFBool") == 0) return SFBOOL;
    if (strcmp(type, "SFColor") == 0) return SFCOLOR;
    if (strcmp(type, "SFFloat") == 0) return SFFLOAT;
    if (strcmp(type, "SFImage") == 0) return SFIMAGE;
    if (strcmp(type, "SFInt32") == 0) return SFINT32;
    if (strcmp(type, "SFNode") == 0) return SFNODE;
    if (strcmp(type, "SFRotation") == 0) return SFROTATION;
    if (strcmp(type, "SFString") == 0) return SFSTRING;
    if (strcmp(type, "SFTime") == 0) return SFTIME;
    if (strcmp(type, "SFVec2f") == 0) return SFVEC2F;
    if (strcmp(type, "SFVec3f") == 0) return SFVEC3F;
    if (strcmp(type, "MFColor") == 0) return MFCOLOR;
    if (strcmp(type, "MFFloat") == 0) return MFFLOAT;
    if (strcmp(type, "MFInt32") == 0) return MFINT32;
    if (strcmp(type, "MFNode") == 0) return MFNODE;
    if (strcmp(type, "MFRotation") == 0) return MFROTATION;
    if (strcmp(type, "MFString") == 0) return MFSTRING;
    if (strcmp(type, "MFVec2f") == 0) return MFVEC2F;
    if (strcmp(type, "MFVec3f") == 0) return MFVEC3F;

    cerr << "Illegal field type: " << type << "\n";

    return 0;
}

vrmlNode *parserRoot;
char rootfieldname[]="children";

void
enterNode(const char *nodeType)
{
	const VrmlNodeType *t = VrmlNodeType::find(nodeType);
    if (t == NULL) {
    	char tmp[1000];
        sprintf(tmp, "Unknown node type '%s'", nodeType);
    	yyerror(tmp);
    }

    FieldRec *fr = new FieldRec;
    fr->nodeType = t;
    fr->fieldName = NULL;

// .......... get the parent node
    vrmlNode *parentNode = parserRoot;  // the root node by default
    char *parentFieldName = rootfieldname;

    if (currentField.count())
	   {
		FieldRec *frParent = currentField.getTop();
		parentNode = frParent->node;
		parentFieldName = (char *) frParent->fieldName;
	   }

// .......... and create the new node
    fr->node = vrmlNode::create(nodeType, parentNode, parentFieldName, 0);

// .......... set a root node pointer to indicate the scene's root frame if one is not provided
	if (!parentNode)
		parserRoot = fr->node;

    currentField.push(fr);
}

void
exitNode()
{
	FieldRec *fr = currentField.getTop();
    assert(fr != NULL);
	currentField.pop();

	lastNodeExited = fr->node;
    delete fr;
}

void
inScript()
{
	FieldRec *fr = currentField.getTop();
    if (fr->nodeType == NULL ||
    	strcmp(fr->nodeType->getName(), "Script") != 0) {
        yyerror("interface declaration outside of Script or prototype");
    }
}



FieldRec *currentlyParsing=0;

void
enterField(const char *fieldName)
{
	FieldRec *fr = currentField.getTop();
    assert(fr != NULL);

    currentlyParsing = fr;
    fr->fieldName = fieldName;
    if (fr->nodeType != NULL) {
    	// enterField is called when parsing eventIn and eventOut IS
        // declarations, in which case we don't need to do anything special--
        // the IS IDENTIFIER will be returned from the lexer normally.
    	if (fr->nodeType->hasEventIn(fieldName) ||
        	fr->nodeType->hasEventOut(fieldName))
            return;
    
        int type = fr->nodeType->hasField(fieldName);
        if (type != 0) {
        	// Let the lexer know what field type to expect:
        	expect(type);

        }
        else {
        	cerr << "Error: Node's of type " << fr->nodeType->getName() <<
            	" do not have fields/eventIn/eventOut named " <<
                fieldName << "\n";
            // expect(ANY_FIELD);
        }
    }
    // else expect(ANY_FIELD);
}

void
exitField()
{
	FieldRec *fr = currentField.getTop();
    assert(fr != NULL);

	fr->fieldName = NULL;
}

void
expect(int type)
{
	expectToken = type;
}

void defNode()
	{
	if ( lastNodeExited && nameNextNode[0] )
		{
		lastNodeExited->setName( nameNextNode );
		nameNextNode[0]='\0';
		lastNodeExited = 0;
		}
	}

void useNode(const char *name)
	{
	vrmlNode *namedNode =  vrmlNode::findNode( (char *) name );
	assert( namedNode ); // named node must be already DEF'd

	assert ( currentlyParsing ); // must be parsing a valid field

	const char *fname = currentlyParsing->fieldName;
	vrmlNode *n = currentlyParsing->node;
	assert(fname);
	assert(n);

// .......... since namedNode is non-NULL, 'create' will create a reference to the existing
// .......... node rather than allocate an entirely new node.
	vrmlNode::create(0, n, fname, namedNode);
	}
