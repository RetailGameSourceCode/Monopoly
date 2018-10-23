// //////////////////////////////////////////////////
// MODULE: IPOINT.H
// Purpose: header file for module IPOINT.CPP
// //////////////////////////////////////////////////
#ifndef IPOINT_H
#define IPOINT_H
// .................................. CLASS: <iPoint>
// .................................. Defines a coordinate pair -> intended
// .................................. to be cartesian, can be interpreted as desired
class iPoint {

public :

	int x;
	int y;

// ...................... constructor
	iPoint(int x_init, int y_init)
		: x(x_init), y(y_init)
		{}

	iPoint() : x(0), y(0) {}

	int getX() const
		{return x;}

	int getY() const
		{return y;}

	void setX(int new_x)
		{x = new_x;}

	void setY(int new_y)
		{y = new_y;}

	void moveBy(int dx,int dy)
		{
		x += dx;
		y += dy;
		}

	void moveTo(int x_init,int y_init)
		{
// ...................... move the point's coordinates
		x = x_init;
		y = y_init;
		}

// ...................... add the coordinates of one point to 'this' point
	void operator += (const iPoint& to_add)
		{
// ...................... move the point's coordinates
		x += to_add.x;
		y += to_add.y;
		}

// ...................... subtract the coordinates of one point from 'this' point
	void operator -= (const iPoint& to_subtract)
		{
// ...................... move the point's coordinates
		x -= to_subtract.x;
		y -= to_subtract.y;
		}

// ...................... scale the point
	void operator *= (int scale_factor)
		{
		x *= scale_factor;
		y *= scale_factor;
		}

// ...................... de-scale the point
	void operator /= (int descale_factor)
		{
		x /= descale_factor;
		y /= descale_factor;
		}

// ...................... return a point reference to the vector sum of two points
	iPoint operator +(const iPoint& to_add) const {return iPoint(x + to_add.x, y + to_add.y);}

// ...................... determine if a point is equal to 'this'
	int operator == (const iPoint& to_equal) const
		{return (x == to_equal.x && y == to_equal.y);}

// ...................... return a point reference to the vector subtraction of two points
	iPoint operator -(const iPoint& to_sub) const {return iPoint(x - to_sub.x, y - to_sub.y);}
	iPoint operator -() const {return iPoint(-x,-y);}

// ...................... test to see if point is at the origin
	int isOrigin(void) const
		{return (x == 0 && y == 0);}

	static unsigned Distance(iPoint &p1,iPoint &p2);

	static iPoint UNIT_X;
	static iPoint UNIT_Y;
	static iPoint UNIT_XY;

private:
	static iPoint GENERAL;
	};/* end class iPoint */


#endif
// ...................... end of file










