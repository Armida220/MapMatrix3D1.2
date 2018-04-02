#ifndef IPOLYGON_BOOLEAN_H_
#define IPOLYGON_BOOLEAN_H_

#include "PolygonBoolean.h"

namespace plybn
{

struct point2d
{
	double x;
	double y;

	point2d() { x = 0; y = 0; }
	point2d(double x_, double y_) { x = x_; y = y_; }
};


class POLYGONBOOLEAN_API ILinearRing2d
{
public:
	static ILinearRing2d* Create();
	static void Release(ILinearRing2d* plr);
	virtual ~ILinearRing2d() {}
	//
	virtual void addPoint(point2d& p) = 0;
	virtual int getSize() = 0;
	virtual point2d getPoint(int index) = 0;
	virtual void clear() = 0;
	virtual ILinearRing2d* clone() = 0;
	//
	virtual int point_locate(point2d& p) = 0;
	virtual bool is_simple() = 0;
};

class POLYGONBOOLEAN_API IPolygon2d
{
public:
	static IPolygon2d* Create();
	static void Release(IPolygon2d* pp);
	virtual ~IPolygon2d() {}
	//
	virtual void setOuter(ILinearRing2d* pouter) = 0;
	virtual void addHole(ILinearRing2d* phole) = 0;
	virtual ILinearRing2d* getOuter() = 0;
	virtual int getHoleCount() = 0;
	virtual ILinearRing2d* getHole(int index) = 0;
	virtual void clear() = 0;
	virtual IPolygon2d* clone() = 0;
};


class POLYGONBOOLEAN_API IPolygon2dSet
{
public:
	static IPolygon2dSet* Create();
	static void Release(IPolygon2dSet* pps);
	virtual ~IPolygon2dSet() {}
	//
	virtual void addPolygon(IPolygon2d* pplg) = 0;
	virtual int getSize() = 0;
	virtual IPolygon2d* getPolygon(int index) = 0;
	virtual void clear() = 0;
};

class POLYGONBOOLEAN_API PolygonBoolean
{
public:
	//判断两个多边形是否有重叠的区域；
	static bool is_intersected(const IPolygon2d* pa, const IPolygon2d* pb);
	//判断多边形 pa 是否包含多边形 pb；
	static bool is_within(const IPolygon2d* pa, const IPolygon2d* pb);
	//
	static void intersection(const IPolygon2d* pa, const IPolygon2d* pb, IPolygon2dSet* pres);
	static void join(const IPolygon2d* pa, const IPolygon2d* pb, IPolygon2d* pres); 
	static void difference(const IPolygon2d* pa, const IPolygon2d* pb, IPolygon2dSet* pres);
	static void symmetric_difference(const IPolygon2d* pa, const IPolygon2d* pb, IPolygon2dSet* pres);
};


}


#endif