// //////////////////////////////////////////////////
// MODULE: IRECT
// Purpose: Defines the class iRect
// //////////////////////////////////////////////////
#include "PC3DHdr.h"

#include "ipoint.h"
#include "irect.h"


const iRect NULL_Rectangle(0,0,0,0);

// .................... adjust rectangle to the smallest rectangle
// .................... containing both 'this' and 'to_capture'
void iRect::capture(const iPoint& to_capture)
	{
// .................... if 'this' has zero area, 'this' becomes 'to_capture'
	if ((*this).isNull())
		{
		this->setRect(to_capture.getX(),to_capture.getY(),1,1);
		return;
		}
// .................... otherwise, 'this' rectangle has non-zero area:
// .................... calculate the end coordinates of the rectangle
	int xend = x + ((int)xdim) - 1;
	int yend = y + ((int)ydim) - 1;

// .................... calculate the new rectangle points, treating the
// .................... x and y directions independently
	int newx = MIN(x,to_capture.getX());
	int newy = MIN(y,to_capture.getY());
	int newxend = MAX(xend,to_capture.getX());
	int newyend = MAX(yend,to_capture.getY());

// .................... finally, adjust 'this' rectangle to the selected values
	setRect(newx, newy, newxend - newx + 1, newyend - newy + 1);
	}

// .................... adjust rectangle to the smallest rectangle
// .................... containing both 'this' and 'to_capture'
// .................... NOTE: zero area rectangles are not captured,
// .................... regardless of their location
void iRect::capture(const iRect& to_capture)
	{
// .................... leave 'this' intact if 'to_capture' has zero area
	if (to_capture.isNull())
		return;
// .................... if 'this' has zero area, 'this' becomes 'to_capture'
	if ((*this).isNull())
		{
		*this = to_capture;
		return;
		}
// .................... otherwise, both rectangles have non-zero area:
// .................... calculate the end coordinates of the rectangles
	int xend = x + ((int)xdim) - 1;
	int yend = y + ((int)ydim) - 1;
	int capturexend = to_capture.x + ((int)to_capture.xdim) - 1;
	int captureyend = to_capture.y + ((int)to_capture.ydim) - 1;

// .................... calculate the new rectangle points, treating the
// .................... x and y directions independently
	int newx = MIN(x,to_capture.x);
	int newy = MIN(y,to_capture.y);
	int newxend = MAX(xend,capturexend);
	int newyend = MAX(yend,captureyend);

// .................... finally, adjust 'this' rectangle to the selected values
	setRect(newx, newy, newxend - newx + 1, newyend - newy + 1);
	}


// //////////////////////////////////////////////////
// FUNCTION: intersects
//		Purpose: returns 'YES'' if a rectangle
//	'intersector'intersects this rectangle.
// //////////////////////////////////////////////////

int iRect::intersects(const iRect& intersector) const
	{

	long xleft,xright,ytop,ybottom,horz_end,vert_end;

// ...................... Consider each of the two rectangle dimensions independently
// ...................... to determine whether or not there is an intersection.
// ...................... Start with the horizontal dimension, then the vertical...

// ...................... get right edge of 'this'
	horz_end = x + ((long) xdim) - 1;

// ...................... get left & right edges of intersector
	xleft = intersector.x;
	xright = xleft + ((long)intersector.getXdim()) - 1;

// ...................... test conditions for NULL (zero intersection) rectangle
	if (xright < x || xleft > horz_end)
		return(0);

// ...................... get bottom edge of 'this'
	vert_end = y + ((long)ydim) - 1;

// ...................... get top & bottom edges of intersector
	ytop = intersector.y;
	ybottom = ytop + ((long)intersector.getYdim()) - 1;

// ...................... test conditions for NULL (zero intersection) rectangle
	if (ybottom < y || ytop > vert_end)
		return(0);

	return 1;
	}


iPoint iRect::clipPt(const iPoint& to_clip) const
	{
	if (isNull())
		return *this;
	int xa = to_clip.getX();
	int xl = getX();
	int xr = getX() + getXdim() - 1;
	xa = RANGE_LIMIT(xa, xl, xr);
	int ya = to_clip.getY();
	int yl = getY();
	int yr = getY() + getYdim() - 1;
	ya = RANGE_LIMIT(ya, yl, yr);
	return iPoint(xa,ya);
	}


// //////////////////////////////////////////////////
// FUNCTION: intersect
//		Purpose: Computes a rectangle corresponding to the
//	area of intersection of 'intersector' & the current
//	implied iRect (the target) . The coordinates returned
//	are placed in 'result'. The return value is non zero if
//	there was a non-null intersection
// //////////////////////////////////////////////////

int iRect::intersect(const iRect& intersector,iRect& result) const
	{

	long xleft,xright,ytop,ybottom,horz_end,vert_end;

// ...................... Consider each of the two rectangle dimensions independently
// ...................... to determine whether or not there is an intersection.
// ...................... Start with the horizontal dimension, then the vertical...

// ...................... get right edge of 'this'
	horz_end = x + ((long) xdim) - 1;

// ...................... get left & right edges of intersector
	xleft = intersector.x;
	xright = xleft + ((long)intersector.getXdim()) - 1;

// ...................... test conditions for NULL (zero intersection) rectangle
	if (xright < x || xleft > horz_end)
		{
		result = NULL_Rectangle;
		return(0);
		}

// ...................... get bottom edge of 'this'
	vert_end = y + ((long)ydim) - 1;

// ...................... get top & bottom edges of intersector
	ytop = intersector.y;
	ybottom = ytop + ((long)intersector.getYdim()) - 1;

// ...................... test conditions for NULL (zero intersection) rectangle
	if (ybottom < y || ytop > vert_end)
		{
		result = NULL_Rectangle;
		return(0);
		}

 // ...................... restrict left coordinate to left edge of target
	if (xleft < x)
		xleft = x;
// ...................... restrict right coordinate to right edge of target
	if (xright > horz_end)
		xright = horz_end;
// ...................... restrict top coordinate to top edge of target
	if (ytop < y)
		ytop = y;
// ...................... restrict bottom coordinate to bottom edge of target
	if (ybottom > vert_end)
		ybottom = vert_end;

// ...................... now, normalize the result to the coordinates of the target
// ...................... and put the intersection rectangle in 'result'
	unsigned result_xdim = xright - xleft + 1;
	unsigned result_ydim = ybottom - ytop + 1;
	result.setRect(
		xleft,
		ytop,
		result_xdim,
		result_ydim);
	return 1;
	}


// ...................... end of file
