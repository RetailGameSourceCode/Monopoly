// //////////////////////////////////////////////////
// MODULE: IRECT.H
// Purpose: header file for module IRECT.CPP
// //////////////////////////////////////////////////
#ifndef IRECT_H
#define IRECT_H


class iRect : public iPoint {

public:
	int 	xdim, ydim;

		iRect(int x_init, int y_init, int xdim_init, int ydim_init)
			: iPoint(x_init, y_init),
			xdim(xdim_init),
			ydim(ydim_init)
			{}
		iRect() : iPoint(), xdim(0), ydim(0) {};
		iRect(const iPoint& startPt, const iPoint& endPt)
			: iPoint(startPt),
			xdim(endPt.getX() - x + 1),
			ydim(endPt.getY() - y + 1)
			{}
		iRect(const iPoint& startPt, int xdim_init, int ydim_init)
			: iPoint(startPt),
			xdim(xdim_init),
			ydim(ydim_init)
			{}

	void 	setRect(int x_init, int y_init, int xdim_init, int ydim_init)
			{moveTo(x_init, y_init);
			xdim = xdim_init;
			ydim = ydim_init;}

	int 	getXdim() const {return xdim;}
	int 	getYdim() const {return ydim;}
	void  	setXdim(int xdim_init) {xdim =  xdim_init;}
	void  	setYdim(int ydim_init) {ydim =  ydim_init;}

	int isNull() const {return xdim == 0 || ydim == 0;}
	int isNotNull() const {return xdim != 0 && ydim != 0;}
	long 	getArea() const {return ((long)xdim) * ((long)ydim);}

	void 	expandBy(int byX, int byY) {xdim += byX; ydim += byY;}
	iPoint 	getEndPoint() const { return iPoint(x + xdim - 1, y + ydim - 1);}
	iPoint 	getCenterPoint() const { return iPoint(x + (xdim >> 1), y + (ydim >> 1));}

	int encloses(const iPoint& point) const
			{int xp = point.getX();	int yp = point.getY();
			return  (x <= xp && xp < (x + ((int)xdim)))
				&& (y <= yp && yp < (y + ((int)ydim)));}
	int	clipLine(iPoint& p1, iPoint& p2) const;
	iPoint 	clipPt(const iPoint& to_clip) const;
	void 	capture(const iRect& to_capture);
	void 	capture(const iPoint& to_capture);
	int intersects(const iRect& intersector) const;
	int intersect(const iRect& intersector, iRect& result) const;

	iRect	operator && (const iRect& intersector) const
			{iRect temp;
			intersect(intersector,temp);
			return temp;}

	int operator ==(const iRect& toEqual) const
			{return x == toEqual.x && y == toEqual.y &&
				xdim == toEqual.xdim && ydim == toEqual.ydim;}
};/* class iRect */

extern const iRect NULL_Rectangle;

#endif
// ...................... end of file
