/**************************************************
 * VRML 2.0, Draft 2 Parser
 * Copyright (C) 1996 Silicon Graphics, Inc.
 *
 * Author(s)    : Gavin Bell
 *                Daniel Woods (first port)
 **************************************************
 */

#include <iostream.h>
#include <stdio.h>
#include "stldefs.h"
#include "VrmlNodeType.h"

extern int yyparse();
extern void yyResetLineNumber();
extern int yydebug;
extern int yy_flex_debug;
extern FILE *yyin;

main(int argc, char **argv)
{
    //
    // Read in PROTO declarations for all of the standard nodes:
    //
	yydebug = 0;
    yy_flex_debug = 0;
    FILE *standardNodes = fopen("standardNodes.wrl", "r");
    if (standardNodes == NULL) {
    	cerr << "Error, couldn't open standardNodes.wrl file";
        exit(1);
    }
    yyin = standardNodes;
    yyparse();
    yyin = NULL;
    yyResetLineNumber();
    fclose(standardNodes);
    
	int flags = 0;
	if (argc > 1) {
    	flags = atoi(argv[1]);
    }
    yydebug = flags & 1;
    yy_flex_debug = flags & 2;

    char filename[100];
    cerr << "Enter test file name: ";
    cin >> filename;
    yyin = fopen(filename, "r");

    if (yyin == NULL) {
    	cerr << "Error, could not open file\n";
        exit(1);
    }
    // For this little test application, pushing and popping the node
    // namespace isn't really necessary.  But each VRML .wrl file is
    // a separate namespace for PROTOs (except for things predefined
    // in the spec), and pushing/popping the namespace when reading each
    // file is a good habit to get into:
    VrmlNodeType::pushNameSpace();
    yyparse();
    VrmlNodeType::popNameSpace();

    return(0);
}
