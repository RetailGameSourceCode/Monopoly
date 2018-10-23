#include "PC3DHdr.h"

#include "tomesh.h"

/* .......... This triangle stripping code was taken from the following source...
   .......... 	-cc Oct 14,97 */

/*
 *
 *	The algorithm attempts to avoid leaving isolated triangles by
 *	choosing as the next triangle in a growing mesh that adjacent
 *	triangle with the minimum number of neighbors.  It will frequently
 *	produce a single, large mesh for a well behaved set of triangles.
 *
 *

 *	exports four routines:	
 *
 *		Meshobj *newMeshobj( ... )
 *		void in_ambegin( Meshobj * )
 *		void in_amnewtri( Meshobj * )
 *		void in_amvert( Meshobj *, long )
 *		void in_amend( Meshobj * )
 *		void freeMeshobj( Meshobj * )
 *
 *	calls back these routines:	
 *		void out_ambegin( long nverts, long ntris )
 *		void out_amend( void );
 * 		int out_amhashvert( long v )
 *		int out_amvertsame( long v1, long v2 )
 *		void out_amvertdata( long fptr )
 *		void out_ambgntmesh( void ) 
 *		void out_amendtmesh( void ) 
 *		void out_amswaptmesh( void ) 
 *		void out_amvert( long index )
 *
 *	on output the package calls:
 *	
 *	void out_ambegin( long nverts, long ntris )
 *	calls repeatedly:
 *		int out_amhashvert( long v )
 *		int out_amvertsame( long v1, long v2 )
 *	calls nverts times:
 *		void out_amvertdata( long fptr )
 *	calls these in mixed sequence:
 *		void out_ambgntmesh( void ) 
 *		  void out_amvert( long index )
 *		  void out_amswaptmesh( void )
 *		void out_amendtmesh( void ) 
 *	void out_amend()
 */


void __cdecl dprintf (char *szFormat, ...)
{
    char   szBuffer[1024];
    va_list va;

    lstrcpy (szBuffer, "");
    va_start(va, szFormat);
    wvsprintf (szBuffer+lstrlen (szBuffer), szFormat, va);
    va_end(va);
    lstrcat (szBuffer, "");
    OutputDebugString (szBuffer);
}

#define FALSE		0

#define TRUE		1

#define VERTHASHSIZE	9991

#define TRIHASHSIZE	9991

#define EDGEHASHSIZE	9991

#define INITMESH	replaceB = FALSE

#define SWAPMESH	replaceB = !replaceB

#define REPLACEVERT(v)	{vert[replaceB] = (v); SWAPMESH;}



typedef struct Vert {

    struct Vert *next,*prev;

    struct Vert *hnext;

    long id, index;

} Vert;



typedef struct Vertlist {

    Vert *first,*last;

} Vertlist;



typedef struct Edge {

    struct Edge *hnext;

    struct Tri *tri;

    long index0, index1;

} Edge;



typedef struct Tri {

    struct Tri *next,*prev;

    struct Tri *hnext;

    struct Tri *(adj[3]);

    Vert *vert[3];

    int drawvert[3];

    int adjcount;

    int used;

} Tri;



typedef struct Trilist {

    Tri *first,*last;

} Trilist;



/* local functions */

static Tri *	hashtrifind( Meshobj *, Tri * );

static Vert *	hashvertfind( Meshobj *, Vert * );

static void	beginoutputobj( Meshobj *, Vert *, Tri * );

static void	freelists( Meshobj * );

static void	hashedgebegin( Meshobj *, int );

static void	hashedgeadd( Meshobj *, Tri *, Vert *, Vert * );

static Edge *	hashedgefind( Meshobj *, Vert *, Vert * );

static void	hashedgeend( Meshobj * );

static void	hashtriadd( Meshobj *, Tri * );

static void	hashtribegin( Meshobj * );

static void	hashtriend( Meshobj * );

static int	hashvert( Meshobj *, Vert * );

static void	hashvertadd( Meshobj *, Vert* );

static void	hashvertbegin( Meshobj * );

static void	hashvertend( Meshobj * );

static void	makelists( Meshobj * );

static void	outmesh( Meshobj *, Trilist * );

static void	removeadjacencies( Meshobj *, Tri * );



static Tri *	maketri( void ) ;

static Tri *	minadj( Tri * );

static Trilist *maketrilist( void ) ;

static Vert *	makevert( void ) ;

static Vertlist *makevertlist( void );

static int	hashedge( long, long );

static int	hashtri( Tri * );

static int	ismember( Vert *, Tri * );

static int	notcommon( Tri *, Tri * );

static int	triequal( Tri *, Tri * ) ;

static void	deletetri( Trilist *, Tri * );

static void	freetri( Tri * );

static void	freetrilist( Trilist * );

static void	freevert( Vert * );

static void	freevertlist( Vertlist * );

static void	inserttri( Trilist *, Tri * );

static void	insertvert( Vertlist *, Vert * );



Meshobj *

newMeshobj ( 

    void	(*out_ambegin)( int, int ),

    void	(*out_amend)( void ),

    int		(*out_amhashvert)( long ),

    int		(*out_amvertsame)( long, long ),

    void	(*out_amvertdata)( long ),

    void	(*out_ambgntmesh)( void ),

    void	(*out_amendtmesh)( void ),

    void	(*out_amswaptmesh)( void ),

    void	(*out_amvert)( long ) )

{

    Meshobj *m;



    m			= (Meshobj *) malloc( sizeof(Meshobj) );

    m->connectcount	= 0;

    m->independentcount	= 0;

    m->ambegin		= out_ambegin;

    m->amend		= out_amend;

    m->amhashvert	= out_amhashvert;

    m->amvertsame	= out_amvertsame;

    m->amvertdata	= out_amvertdata;

    m->ambgntmesh	= out_ambgntmesh;

    m->amendtmesh	= out_amendtmesh;

    m->amswaptmesh	= out_amswaptmesh;

    m->amvert		= out_amvert;

    return m;

}



void

freeMeshobj( Meshobj *m )

{

    free( m );

}



/* 

 *	Vertex hashing

 *

 */

static void hashvertbegin( Meshobj *m )

{

    int i;



    m->verthashlist = (Vert**)malloc(VERTHASHSIZE*sizeof(Vert*));

    for(i=0; i<VERTHASHSIZE; i++) 

	m->verthashlist[i] = 0;

}



static int hashvert( Meshobj *m, Vert *vert )

{

    long val;



    val = (*m->amhashvert)(vert->id);

    return val%VERTHASHSIZE;

}



static Vert *hashvertfind( Meshobj *m, Vert *vert )

{

    int pos;

    Vert *vptr;



    pos = hashvert(m,vert);

    for( vptr = m->verthashlist[pos]; vptr; vptr = vptr->hnext )

	if((*m->amvertsame)(vert->id,vptr->id)) 

	    return vptr;

    return 0;

}



static void hashvertadd( Meshobj *m, Vert* vert )

{

    int pos;



    pos = hashvert(m,vert);

    vert->hnext = m->verthashlist[pos];

    m->verthashlist[pos] = vert;

}



static void hashvertend( Meshobj *m )

{

    free(m->verthashlist);

}



/* 

 *	Triangle hashing

 *

 */

static void hashtribegin( Meshobj *m )

{

    int i;



    m->trihashlist = (Tri**)malloc(TRIHASHSIZE*sizeof(Tri*));

    for(i=0; i<TRIHASHSIZE; i++) 

	m->trihashlist[i] = 0;

}



static int hashtri( Tri *tri )

{

    long val, l;



    l = (long)tri->vert[0];

    val = l;

    l = (long)tri->vert[1];

    val = val^l;

    l = (long)tri->vert[2];

    val = val^l;

    return val%VERTHASHSIZE;

}



static Tri *hashtrifind( Meshobj *m, Tri *tri )

{

    int pos;

    Tri *tptr;



    pos = hashtri(tri);

    for( tptr = m->trihashlist[pos]; tptr; tptr = tptr->hnext ) 

	if(triequal(tri,tptr)) 

	    return tptr;

    return 0;

}



static void hashtriadd( Meshobj *m, Tri *tri )

{

    int pos;



    pos = hashtri(tri);

    tri->hnext = m->trihashlist[pos];

    m->trihashlist[pos] = tri;

}



static void hashtriend( Meshobj *m )

{

    free(m->trihashlist);

}



/* 

 *	Edge hashing

 *

 */

static void hashedgebegin( Meshobj *m, int np )

{

    int i;



    m->edgehashlist = (Edge**)malloc(EDGEHASHSIZE*sizeof(Edge*));

    m->edgearray = m->freeedges = (Edge*)malloc(3*np*sizeof(Edge));

    for(i=0; i<EDGEHASHSIZE; i++) 

	m->edgehashlist[i] = 0;

}



static int hashedge( long index0, long index1 )

{

    long val;



    val = index0*index1;

    val = val&0x7fffffff;

    return val%EDGEHASHSIZE;

}



static Edge *hashedgefind( Meshobj *m, Vert *v0, Vert *v1 )

{

    int pos;

    long index0, index1;

    Edge *tptr;



    index0 = v0->index;

    index1 = v1->index;

    pos = hashedge(index0,index1);

    tptr = m->edgehashlist[pos];

    return tptr;

}



static void hashedgeadd( Meshobj *m, Tri *tri, Vert *v0, Vert *v1 )

{

    int pos;

    long index0, index1;

    Edge *edge;



    index0 = v0->index;

    index1 = v1->index;

    pos = hashedge(index0,index1);

    edge = m->freeedges++;

    edge->index0 = index0;

    edge->index1 = index1;

    edge->tri = tri;

    edge->hnext = m->edgehashlist[pos];

    m->edgehashlist[pos] = edge;

}



static void hashedgeend( Meshobj *m )

{

    free(m->edgehashlist);

    free(m->edgearray);

}



static Vertlist *makevertlist( void ) 

{

    /* allocate space for and initialize a vert list */

    Vertlist *tmp;



    if ((tmp=(Vertlist*)malloc(sizeof(Vertlist)))==0) {

	(void) fprintf(stderr,"makevertlist: out of memory.  abort.\n");

	exit(1);

    }

    tmp->first = tmp->last = 0;

    return tmp;

}



static void freevertlist( Vertlist *vl )

{

    Vert *vert, *nvert;



    for( vert = vl->first; vert; vert = nvert ) {

	nvert = vert->next;

	freevert(vert);

    }

    free(vl);

}



static Trilist *maketrilist( void ) 

{

    /* allocate space for and initialize a tri list */

    Trilist *tmp;



    if ((tmp=(Trilist*)malloc(sizeof(Trilist)))==0) {

	(void) fprintf(stderr,"maketrilist: out of memory.  abort.\n");

	exit(1);

    }

    tmp->first = tmp->last = 0;

    return tmp;

}



static void freetrilist( Trilist *tl )

{

    Tri *tri, *ntri;



    for( tri = tl->first; tri; tri = ntri ) {

	ntri = tri->next;

	freetri(tri);

    }

    free(tl);

}



static Vert *makevert( void ) 

{

    /* allocate space for and initialize a vert */

    Vert *tmp;



    if ((tmp=(Vert*)malloc(sizeof(Vert)))==0) {

	(void) fprintf(stderr,"makevert: out of memory.  abort.\n");

	exit(1);

    }

    tmp->prev = tmp->next = 0;

    tmp->index = 0;

    return tmp;

}



static void freevert( Vert *v )

{

    free(v);

}



static Tri *maketri( void ) 

{

    /* allocate space for and initialize a tri */

    Tri *tmp;



    register int i;

    if ((tmp=(Tri*)malloc(sizeof(Tri)))==0) {

	(void) fprintf(stderr,"maketri: out of memory.  abort.\n");

	exit(1);

    }

    tmp->prev = tmp->next = 0;

    for (i=0; i<3; i++) {

	tmp->adj[i] = 0;

	tmp->vert[i] = 0;

    }

    tmp->drawvert[0] = 0;

    tmp->drawvert[1] = 1;

    tmp->drawvert[2] = 2;

    tmp->adjcount = 0;

    tmp->used = FALSE;

    return tmp;

}



static void freetri( Tri *tri )

{

//    free(tri); don't do this, problems occur because code assumes data still valid

}



static void insertvert( Vertlist *list, Vert *item )

{

    /* insert the new item at the top of the list */

    if (list->first) {

	item->next = list->first;

	item->prev = 0;

	item->next->prev = item;

	list->first = item;

    } else {

	list->first = list->last = item;

	item->prev = item->next = 0;

    }

}



static void inserttri( Trilist *list, Tri *item )

{

    /* insert the new item at the top of the list */

    if (list->first) {

	item->next = list->first;

	item->prev = 0;

	item->next->prev = item;

	list->first = item;

    } else {

	list->first = list->last = item;

	item->prev = item->next = 0;

    }

}



static void deletetri( Trilist *list, Tri *item )

{

    /* delete the item from the list */

    if (list->first == item) {

	if (list->last == item) {

	    /* this is the only item in the list */

	    list->first = list->last = 0;

	} else {

	    /* this is the first item in the list */

	    list->first = item->next;

	    list->first->prev = 0;

	}

    } else if (list->last == item) {

	/* this is the last item in the list */

	list->last = item->prev;

	list->last->next = 0;

    } else {

	item->prev->next = item->next;

	item->next->prev = item->prev;

    }

    item->prev = item->next = 0;

}



static int triequal( Tri *tri1, Tri *tri2 ) 

{

    int i, j, k;



    k = 0;

    for (i=0; i<3; i++) {

	for (j=0; j<3; j++) {

	    if (tri1->vert[i] == tri2->vert[j])

		k += 1;

	}

    }

    if (k == 3) 

	return 1;

    else 

	return 0;

}



static void makelists( Meshobj *m )

{

    int i;



    m->vertlist = makevertlist();

    m->trilist = maketrilist();

    m->newtrilist = maketrilist();

    m->donetrilist = maketrilist();

    for (i=0; i<4; i++)

	m->adjtrilist[i] = maketrilist();

}



static void freelists( Meshobj *m )

{

    int i;



    freevertlist(m->vertlist);

    freetrilist(m->trilist);

    freetrilist(m->newtrilist);

    freetrilist(m->donetrilist);

    for (i=0; i<4; i++)

	freetrilist(m->adjtrilist[i]);

}



/*

 *	actual exported routines

 *

 *

 */

void in_ambegin( Meshobj *m )

{



    makelists(m);



    m->vertcount = 0;

    hashvertbegin( m );

    m->tmpvert = makevert();

    m->npolys = 0;

    m->vertno = 0;

}





void in_amnewtri( Meshobj *m )

{

    m->vertno = 0;

    m->curtri = maketri();

    inserttri(m->trilist,m->curtri);

    m->npolys++;

}



void in_amvert( Meshobj *m, long fptr )

{

    Vert *vert;



    if(m->vertno > 2) {

	(void) fprintf(stderr,"in_amvert: can't have more than 3 verts in triangle\n");

	return;

    }

    m->curtri->vert[m->vertno] = 0;

    m->tmpvert->id = fptr;

    vert = hashvertfind(m,m->tmpvert);

    if(vert)

	m->curtri->vert[m->vertno] = vert;

    else  {

	hashvertadd(m,m->tmpvert);

/* add a new vertex to the list */

	m->tmpvert->index = m->vertcount;

	m->vertcount++;

	m->curtri->vert[m->vertno] = m->tmpvert;

	insertvert(m->vertlist,m->tmpvert);

	m->tmpvert = makevert();

    }

    m->vertno++;

}

// .......... reverses the order of a linked list of tris
void reverseTris (struct Trilist *tl)
{
Trilist *tmp = maketrilist();
Tri *tri, *ntri;

for( tri = tl->first; tri; tri = ntri ) 
	{
	ntri = tri->next;
	inserttri(tmp, tri);
	}

// .......... make the orginal list assume the temp's vars (head,tail etc.)
*tl = *tmp;

free(tmp);
}

// .......... Returns the next triangle in a strip. The triangle currently
// .......... at the end of the strip is 'tri', the previous one is 'prev'.
// .......... 'counterclockwise' indicates the order of traversal of the
// .......... current 'tri' vertices and is toggled in anticipation of the 
// .......... next call to this function (i.e. the traversal order tri returned 
// .......... will be the opposite of the current one)
static int ismember( Vert *vert, Tri *tri );

static Tri *getNextInStrip( Tri **addedToStrip, int added, Tri *prev, Tri *tri, int *clockwise )
{
int startprev,nextprev,finalprev;
int prevCW = (*clockwise == -1) ? 1 : -1;
int i,j;
Vert *startvert;
Vert *middlevert;
Tri *nexttri;

// .......... get the starting vertex of 'prev'
startprev = notcommon(prev,tri);

// .......... get the next vertex of 'prev', taking into account the traversal order
nextprev = startprev+prevCW;
if (nextprev < 0)
	nextprev += 3;
else if (nextprev > 2)
	nextprev -= 3;

// .......... get the final vertex of 'prev', taking into account the traversal order
finalprev = nextprev;
finalprev = nextprev+prevCW;
if (finalprev < 0)
	finalprev += 3;
else if (finalprev > 2)
	finalprev -= 3;

// .......... for the current 'tri', the starting vert 
// .......... is the middle (next) vert of the previous tri, 
// .......... and the middle vert is the final vert of the previous.
startvert = prev->vert[ nextprev ];
middlevert = prev->vert[ finalprev ];

// .......... The next tri must have 'middlevert' as a vertex and
// .......... cannot have 'startvert', so just look for such a tri 
// .......... adjacent to the current 'tri'

// .......... by default there is no next triangle
nexttri = NULL;

for (i=0; i < 3; i++)
	{
	if (tri->adj[ i ])
		{
		if ( ismember( middlevert, tri->adj[ i ] ) &&
			 !ismember( startvert, tri->adj[ i ] ) )
			{
			// one more check to see if it's already in the strip
			if (addedToStrip)
				{
				j = -1;
				while(++j < added)
					{
					if (tri->adj[ i ] == addedToStrip[ j ])
						 break;	
					}
				if (j != added)
					continue; // sorry. this tri is already in the strip
				}

			nexttri = tri->adj[ i ];
			break;
			}
		}				 
	}

// .......... finally, toggle the traversal order flag
*clockwise = (*clockwise == -1) ? 1 : -1;

return nexttri;
}

#define MAX_TRIS_IN_STRIP (1024*16)
Tri *addedToStrip[MAX_TRIS_IN_STRIP];

void in_amend( Meshobj *m )

{

    Vert *vert0,*vert1;

    register Tri *tri;

    register Tri *tmptri;

    register Tri *nexttri;

    register Edge *edge;

    int i, j, k;

    int degeneratecount;

    int equivalentcount;

    int count;

    int adjcount[4];

    int adjnumber;

// .......... stuff to support real tri strips -cc
Tri *originalTri;
Tri *nextTri,*tri2nd,*prevTri;
int trisInDir[3];
int sortedDirByTris[3];
int maxTris=0;
int maxDir=0;
int sort;
int dir;
int clockwise;
int added;


    freevert(m->tmpvert);

    if(m->vertno != 3) {

	(void) fprintf(stderr,"in_amend: incomplete triangle\n");

	exit(1);

    }

    hashvertend(m);

    dprintf("%d triangles read\n",m->npolys);

    dprintf("%d vertexes created\n",m->vertcount);



    beginoutputobj(m,m->vertlist->first,m->trilist->first);



/*** eliminate degenerate triangles ***/

    dprintf("eliminating degenerate triangles\n");

    degeneratecount = 0;

    for (tri=m->trilist->first; tri;) {

	if ((tri->vert[0] == tri->vert[1]) ||

	    (tri->vert[0] == tri->vert[2]) ||

	    (tri->vert[1] == tri->vert[2])) {

	    degeneratecount += 1;

	    tmptri = tri->next;

	    deletetri(m->trilist,tri);

	    freetri(tri);

	    tri = tmptri;

	} else 

	    tri = tri->next;

    }

    dprintf("%d degenerate triangles eliminated\n",degeneratecount);



/*** eliminate equivalent triangles ***/

    dprintf("eliminating equivalent triangles\n");

    count = 0;

    equivalentcount = 0;



    hashtribegin(m);

    for (tri=m->trilist->first; tri;) {

	count += 1;

	if(hashtrifind(m,tri)) {

	    equivalentcount += 1;

	    nexttri = tri->next;

	    deletetri(m->trilist,tri);

	    freetri(tri);

	    tri = nexttri;

	} else {

	    hashtriadd(m,tri);

	    tri = tri->next;

	}

    }

    hashtriend(m);

    dprintf("%d equivalent triangles eliminated\n",equivalentcount);



/*** compute triangle adjacencies ***/

    dprintf("computing adjacent triangles\n");

    hashedgebegin(m,m->npolys);

    dprintf("adding to hash table . . ");

    for (tri=m->trilist->first; tri; tri=tri->next) {

	for(i=0; i<3; i++) {

	    vert0 = tri->vert[(i+0)%3];

	    vert1 = tri->vert[(i+1)%3];

	    hashedgeadd(m,tri,vert0,vert1);

	}

    }

    dprintf("done\n");

    count = 0;

    for (tri=m->trilist->first; tri; tri=tri->next) {

	count += 1;

	for (i=0; i<3; i++) {

	    if (tri->adj[i])

		continue;

	    vert0 = tri->vert[i];

	    vert1 = tri->vert[(i+1)%3];

	    for (edge=hashedgefind(m,vert0,vert1); edge; edge=edge->hnext) {

		nexttri = edge->tri;

		if(nexttri == tri) 

		    continue;

		for (j=0; j<3; j++) {

		    if (vert0 == nexttri->vert[j]) {

			for (k=0; k<3; k++) {

			    if (k==j)

				continue;

			    if (vert1 == nexttri->vert[k]) {

				switch (j+k) {

				    case 1:

					adjnumber = 0;

					break;

				    case 2:

					adjnumber = 2;

					break;

				    case 3:

					adjnumber = 1;

					break;

				    default:

					(void) fprintf(stderr,

					    "ERROR: bad adjnumber\n");

					break;

				}

				if (tri->adj[i]||nexttri->adj[adjnumber]) {

				} else {	

				    tri->adj[i] = nexttri;

				    nexttri->adj[adjnumber] = tri;

				}

			    }

			}

		    }

		}

	    }

	}

    }

    hashedgeend(m);

    dprintf(" done\n");



/*** test adjacency pointers for consistency ***/

    for (tri=m->trilist->first; tri; tri=tri->next) {

	for (i=0; i<3; i++) {

	    if (nexttri = tri->adj[i]) {

		for (j=0,k=0; j<3; j++) {

		    if (tri == nexttri->adj[j])

			k += 1;

		}

		if (k != 1) {

		    (void) fprintf(stderr," ERROR: %x to %x k = %d\n",tri,nexttri,k);

		}

	    }

	}

    }



/*** compute adjacency statistics ***/

    for (i=0; i<4; i++)

	adjcount[i] = 0;

    for (tri=m->trilist->first; tri;) {

	for (i=0,count=0; i<3; i++) {

	    if (tri->adj[i])

		count += 1;

	}

	tri->adjcount = count;

	adjcount[count] += 1;

	nexttri = tri->next;

	deletetri(m->trilist,tri);

	inserttri(m->adjtrilist[count],tri);

	tri = nexttri;

    }

    dprintf("adjacencies: 0:%d, 1:%d, 2:%d, 3:%d\n",

	adjcount[0],adjcount[1],adjcount[2],adjcount[3]);



/*** search for connected triangles and output ***/

    while (1) {

    /*** output singular triangles, if any ***/

	while (tri = m->adjtrilist[0]->first) {

	    deletetri(m->adjtrilist[0],tri);

	    inserttri(m->newtrilist,tri);

	    tri->used = TRUE;

	    outmesh(m,m->newtrilist);

	}

    /*** choose a seed triangle with the minimum number of adjacencies ***/

	if (tri = m->adjtrilist[1]->first)

	    deletetri(m->adjtrilist[1],tri);

	else if (tri = m->adjtrilist[2]->first)

	    deletetri(m->adjtrilist[2],tri);

	else if (tri = m->adjtrilist[3]->first)

	    deletetri(m->adjtrilist[3],tri);

	else

	    break;

	inserttri(m->newtrilist,tri);

	removeadjacencies(m,tri);

// .......... START OF CODE TO CHECK FOR PURE STRIPS, NOT GL "PSEUDO-STRIPS" -cc

// .......... See which one of the three possible directions will result
// .......... in the largest triangle strip. New triangle strip code -cc 
originalTri=tri;

maxTris=0;
maxDir=0;
trisInDir[0] = trisInDir[1] = trisInDir[2] = 0;

for (dir = 0; dir < 3; dir++)
{
tri = originalTri;
tri2nd = tri->adj[ dir ];

if (!tri2nd)
	continue; // no more tris in this direction

trisInDir[ dir ] = 1; // at least one tri in this direction
prevTri = tri;
tri = tri2nd;

// .......... get the vertex that starts the 2nd tri, assuming
// .......... that the first tri is counterclockwise

// .......... having chosen the first two triangles ,the rest of the strip
// .......... is already defined...just count up the tris in this strip.
// .......... The strip must end if there is no adjacency at the open edge,
// .......... or if it crosses a triangle already 'added' to the strip.

// .......... number of tris added to this test strip
addedToStrip[0] = prevTri;
addedToStrip[1] = tri;
added = 2;

// .......... original tri is assumed clockwise, 
// .......... so this tri is counterclockwise
clockwise = -1;
while ( (nextTri = getNextInStrip(addedToStrip, added, prevTri, tri, &clockwise)) )
	{
    assert (added < MAX_TRIS_IN_STRIP); // increase the array size
	addedToStrip[added] = nextTri;
	added++;
	trisInDir[ dir ]++; // one more tri in this direction
	prevTri = tri;
	tri = nextTri;
	}
}

// .......... sort the directions by # of tris (only care about the first two directions)
for (sort = 0; sort < 2; sort++)
	{
	if ( trisInDir[ sort ] > maxTris )
		{
		maxDir = sort;
		maxTris = trisInDir[ sort ];
		}
	}

sortedDirByTris[0] = maxDir;
if (maxDir == 0)
	sortedDirByTris[1] = (trisInDir[1] >= trisInDir[2]) ? 1 : 2;
else if (maxDir == 1)
	sortedDirByTris[1] = (trisInDir[0] >= trisInDir[2]) ? 0 : 2;
else 
	sortedDirByTris[1] = (trisInDir[0] >= trisInDir[1]) ? 0 : 1;

// .......... we now have the optimal direction, so init the tri strip 
// .......... by extending in one direction (the strip with the max # of tris)

prevTri = originalTri;
tri = prevTri->adj[ sortedDirByTris[0] ];
clockwise = -1; // original tri is assumed clockwise, so this tri is counterclockwise

while ( tri )
	{
   deletetri(m->adjtrilist[tri->adjcount],tri);
   inserttri(m->newtrilist,tri);
   removeadjacencies(m,tri);

	nextTri = getNextInStrip(0,0, prevTri, tri, &clockwise);
	prevTri = tri;
	tri = nextTri;
	}

// .......... 	The outmesh function simply starts at the head of the linked list 
// .......... and starts with the vertex in the first tri not common to the 
// .......... second triangle, etc. however, the tris were inserted at the head of the 
// .......... list, so the one that was added first is now at the tail of the list,
// .......... So, if the # tris in the list is odd the winding order of the strip
// .......... will be reversed. To ensure correct order, we just reverse the list 
// .......... before giving it  to 'outmesh' so the tri inserted first will be
// .......... output first. -cc
reverseTris (m->newtrilist);

// .......... NOTE: we could optimize further by extending the strip in one of the two remaining 
// .......... directions. I'm not sure how much of a benefit this would be, so it might
// .......... be worth  complicating the code further (i.e. figuring out which of the two
// .......... directions would result in a proper concatenated strip, and whether or not it
// .......... would actually join with the original or the 3rd).

// .......... END OF CODE TO CHECK FOR PURE STRIPS, NOT GL "PSEUDO-STRIPS" -cc


    /*** output the resulting mesh ***/

	outmesh(m,m->newtrilist);

    }

    if (m->connectcount)

	dprintf("%d triangle mesh%s output\n",

	    m->connectcount,m->connectcount==1?"":"es");

    if (m->independentcount)

	dprintf("%d independent triangle%s output\n",

	    m->independentcount,m->independentcount==1?"":"s");

    (*m->amend)();

    freelists(m);

}



static int ismember( Vert *vert, Tri *tri )

{

    /*** return TRUE if vert is one of the vertexes in tri, otherwise FALSE ***/

    register int i;



    for (i=0; i<3; i++)

	if (vert == tri->vert[i])

	    return TRUE;

    return FALSE;

}



static int notcommon( Tri *tri, Tri *tri2 )

{

    /*** returns the index of the vertex in tri that is not in tri2 ***/

    int i;



    for (i=0; i<3; i++)

	if (!ismember(tri->vert[i],tri2))

	    return i;

    return -1;

}


static void outmesh( Meshobj *m, Trilist *tris )

{

    Tri *tri;

    int i;

    Vert *vert[2];

    Vert *nextvert;

    int replaceB;



    /*** output trilist - transfer to donelist ***/

    tri = tris->first;

    if (tri == tris->last) {

	/*** there is only one triangle in the mesh - use polygon command ***/

	m->independentcount += 1;

	(*m->ambgntmesh)();

	(*m->amvert)(tri->vert[0]->index);

	(*m->amvert)(tri->vert[1]->index);

	(*m->amvert)(tri->vert[2]->index);

	(*m->amendtmesh)();

	deletetri(tris,tri);

	inserttri(m->donetrilist,tri);

    } else {

	/*** a real mesh output is required ***/

	m->connectcount += 1;

	/*** start output with vertex that is not in the second triangle ***/

	i = notcommon(tri,tri->next);

	INITMESH;

	(*m->ambgntmesh)();

	(*m->amvert)(tri->vert[i]->index);

	REPLACEVERT(tri->vert[i]);

	(*m->amvert)(tri->vert[(i+1)%3]->index);

	REPLACEVERT(tri->vert[(i+1)%3]);

	(*m->amvert)(tri->vert[(i+2)%3]->index);

	REPLACEVERT(tri->vert[(i+2)%3]);

	/*** compute vertex of second triangle that is not in the first ***/

	i = notcommon(tri->next,tri);

	nextvert = (tri->next)->vert[i];

	/*** transfer triangle to done list ***/

	deletetri(tris,tri);

	inserttri(m->donetrilist,tri);

	for( tri = tris->first; tri->next; tri=tris->first ) {
         
	    /*** check for errors ***/

	    if ((!ismember(vert[0],tri)) || (!ismember(vert[1],tri)) ||

		(!ismember(nextvert,tri))) {

		(void) fprintf(stderr,"ERROR in mesh generation\n");

	    }

	    if ((vert[0] == vert[1]) || (vert[0] == nextvert)) {

		(void) fprintf(stderr,"ERROR in mesh generation\n");

	    }

	    /*** decide whether to swap or not ***/

	    if (ismember(vert[replaceB],tri->next)) {

		(*m->amswaptmesh)();

		SWAPMESH;

	    }

	    /*** output the next vertex ***/

	    (*m->amvert)(nextvert->index);

	    REPLACEVERT(nextvert);

	    /*** determine the next output vertex ***/

	    i = notcommon(tri->next,tri);

	    nextvert = (tri->next)->vert[i];   

	    /*** transfer tri to the done list ***/

	    deletetri(tris,tri);

	    inserttri(m->donetrilist,tri);

	}

	/*** output the last vertex ***/

	(*m->amvert)(nextvert->index);

	REPLACEVERT(nextvert);

	deletetri(tris,tri);

	inserttri(m->donetrilist,tri);

	(*m->amendtmesh)();

    }

}



static void removeadjacencies( Meshobj *m, Tri *tri )

{

    register int i,j;

    Tri *adjtri;



    tri->used = TRUE;

    for (i=0; i<3; i++) {

	adjtri = tri->adj[i];

	if (adjtri) {

	    deletetri(m->adjtrilist[adjtri->adjcount],adjtri);

	    adjtri->adjcount -= 1;

	    for (j=0; j<3; j++) {

		if (tri == adjtri->adj[j]) {

		    adjtri->adj[j] = 0;

		    freetri(adjtri);

		    break;

		}

	    }

	    inserttri(m->adjtrilist[adjtri->adjcount],adjtri);

	}

    }

}



static Tri *minadj( Tri *tri )

{

    int min0,min1;



    switch (tri->adjcount) {

	case 0:

	    return 0;

	case 1:

	    if (tri->adj[0])

		return tri->adj[0];

	    else if (tri->adj[1])

		return tri->adj[1];

	    else

		return tri->adj[2];

	case 2:

	    if (tri->adj[0]) {

		min0 = 0;

		if (tri->adj[1])

		    min1 = 1;

		else

		    min1 = 2;

	    } else {

		min0 = 1;

		min1 = 2;

	    }

	    if ((tri->adj[min0])->adjcount <= (tri->adj[min1])->adjcount)

		return tri->adj[min0];

	    else

		return tri->adj[min1];

	case 3:

	default:

	    if ((tri->adj[0])->adjcount <= (tri->adj[1])->adjcount)

		min0 = 0;

	    else

		min0 = 1;

	    min1 = 2;

	    if ((tri->adj[min0])->adjcount <= (tri->adj[min1])->adjcount)

		return tri->adj[min0];

	    else

		return tri->adj[min1];

    }

}



static void beginoutputobj( Meshobj *m, Vert *verts, Tri *tris ) 

{

    Vert *vert;

    Tri *tri;

    int nverts, ntris;

    int i;



/* count verticies and triangles */

    nverts = 0;

    for( vert = verts; vert; vert = vert->next )

	nverts++;



    ntris = 0;

    for( tri = tris; tri; tri = tri->next )

	ntris++;



    dprintf("makeoutput: vertexes in %d out %d\n",m->npolys*3,nverts);

    dprintf("makeoutput: tris     in %d out %d\n",m->npolys,ntris);

    (*m->ambegin)(nverts,ntris);



    for( i=0, vert = verts; vert; vert = vert->next, i++ ) {

	vert->index = i;

	(*m->amvertdata)(vert->id);

    }

}

