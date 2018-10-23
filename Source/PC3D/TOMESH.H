#ifndef TOMESH
#define TOMESH

// .......... This triangle stripping code was taken from the following source...
// .......... 	-cc Oct 14,97

/*
 *	tomesh - 
 *		Convert independent triangles to large triangle meshes
 *
 *			    Kurt Akeley - 24 March 1988
 *		     		Paul Haeberli - 1990	
 *			   Derrick Burns - September 1990
 *
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

#ifdef __cplusplus
	extern "C" {
#endif


typedef struct Meshobj {

    int			connectcount;

    int			independentcount;

    int			npolys;

    int			vertcount;

    int			vertno;

    struct Edge *	edgearray;

    struct Edge *	freeedges;

    struct Edge **	edgehashlist;

    struct Tri *	curtri;

    struct Tri **	trihashlist;

    struct Trilist *	adjtrilist[4];

    struct Trilist *	donetrilist;

    struct Trilist *	newtrilist;

    struct Trilist *	trilist;

    struct Vert *	tmpvert;

    struct Vert **	verthashlist;

    struct Vertlist *	vertlist;

    void		(*ambegin)( int, int );

    void		(*amend)( void );

    int			(*amhashvert)( long );

    int			(*amvertsame)( long, long );

    void		(*amvertdata)( long );

    void		(*ambgntmesh)( void );

    void		(*amendtmesh)( void );

    void		(*amswaptmesh)( void );

    void		(*amvert)( long );

} Meshobj;



extern	Meshobj * 	newMeshobj ( 

    void (*)( int, int ), void (*)( void ),

    int	 (*)( long ),     int  (*)( long, long ),

    void (*)( long ),     void (*)( void ),

    void (*)( void ),     void (*)( void ),

    void (*)( long ) );

extern	void		freeMeshobj( Meshobj * );

extern	void		in_ambegin( Meshobj * );

extern	void		in_amnewtri( Meshobj * );

extern	void		in_amvert( Meshobj *, long );

extern	void		in_amend( Meshobj * );



#endif

#ifdef __cplusplus
	}
#endif
/* end-of-file for tomesh.h */

