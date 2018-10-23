// //////////////////////////////////////////////////
// MODULE: IPOINT
// Purpose: Defines the class iPoint
// //////////////////////////////////////////////////
#include "PC3DHdr.h"

#include "ipoint.h"



// ...................... points to the point (1,0)
iPoint iPoint::UNIT_X(1,0);
// ...................... points to the point (0,1)
iPoint iPoint::UNIT_Y(0,1);
// ...................... points to the point (1,1)
iPoint iPoint::UNIT_XY(1,1);
// ...................... general purpose point for private use
iPoint iPoint::GENERAL;



unsigned iPoint::Distance(iPoint &p1,iPoint &p2)
	{
// ...................... get the x & y distances
	int x_len = p2.x - p1.x;
	int y_len = p2.y - p1.y;

// ...................... get absolute values of distances
	unsigned xabs_len =  ABSOLUTE_VAL(x_len);
	unsigned yabs_len =  ABSOLUTE_VAL(y_len);

// ...................... use Pythagorean Theorem to calculate distance
	return (unsigned)( sqrt( (double)
		(((unsigned long) xabs_len) * xabs_len) +
		(((unsigned long) yabs_len) * yabs_len) ) );
	}


// ...................... end of file
