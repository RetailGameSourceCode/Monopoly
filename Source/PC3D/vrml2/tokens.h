#ifndef TOKENS_H
#define TOKENS_H
typedef union {
	char *string;

    /* Other types that will be needed by a true VRML implementation
     * (but are not defined by this parser due to the complexity):
     * Node *node;
     * list<Node *> *nodeList;
     */
} YYSTYPE;
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


extern YYSTYPE yylval;

#endif
