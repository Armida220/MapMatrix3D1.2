#ifndef VECTOR2D_H_
#define VECTOR2D_H_

namespace geom
{

#if  defined(PRECISION_FLOAT)
#define real float
#elif  defined(PRECISION_INT)
#define real int
#else 
#define real double
#endif 

class vector2d
{
public:
	vector2d(real _x, real _y)
	{
		x = _x; y = _y;
	}

    vector2d normalize()
	{
	   double t = sqrt(x*x+y*y);
       return vector2d(x/t,y/t);
	}

	vector2d operator+(const vector2d& rv)
	{
		return vector2d(x+rv.x,y+rv.y);
	}

	vector2d operator-(const vector2d& rv)
	{
		return vector2d(x-rv.x,y-rv.y);
	}

	real operator*(const vector2d& rv)
	{
		return x*rv.x+y*rv.y;
	}

	real cross(const vector2d& rv)
	{
		return x*rv.y - y*rv.x;
	}

	real length()
	{
		return sqrt(x*x+y*y);
	}
private:
	real x,y;
};


}
#endif