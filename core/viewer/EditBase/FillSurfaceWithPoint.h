#ifndef  FILLSURFACEWITHPOINT_H_
#define  FILLSURFACEWITHPOINT_H_

#include<vector>
#include<list>
#include<algorithm>
#include<memory>
#include "GeoSurface.h"

namespace fillregion
{

const double tolerance = 0.0001;
const int UNKNOW = 0;
const int INTERSECTION = 1;
const int INTERSECTION_VERTEX = 2;
const int ENTER_POLYGON = 1;
const int LEAVE_POLYGON = -1;
const int COVER_EDGE = 1;
const float heigh_tolerance = tolerance / 2;

struct box
{
	double minx, maxx, miny, maxy;
	box()
	{
		minx = DBL_MAX;
		maxx = -DBL_MAX;
		miny = DBL_MAX;
		maxy = -DBL_MAX;
	}
};

struct vector2d
{
	double x, y, z;
	int flag;
	//
	vector2d(double _x = 0, double _y = 0, double _z = 0)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	//
	bool operator==(const vector2d& rv)
	{
		if(fabs(x-rv.x)<=tolerance && fabs(y-rv.y)<=tolerance)
			return true;
		else
			return false;
	}
	
	bool operator!=(const vector2d& rv)
	{
		if(fabs(x-rv.x)<=tolerance && fabs(y-rv.y)<=tolerance)
			return false;
		else
			return true;
	}
};

bool inline point_sort_func(const vector2d& pa, const vector2d& pb)
{
	if (pa.x <= pb.x)
		return true;
	else
		return false;
}

struct CoordinateSequence
{
	CoordinateSequence() {}
	~CoordinateSequence() {}
	void addCoordinate(vector2d& p){
		coordinates.push_back(p);
	}
	void setCoordinate(int index, vector2d& p){
		coordinates[index] = p;
	}
	int getSize(){
		return coordinates.size();
	}
	void setSize(int size_){
		coordinates.resize(size_, vector2d());
	}
	vector2d& getCoordinate(int index){
		return coordinates[index];
	}

	std::vector<vector2d> coordinates;
};

struct CoordinateSequenceArray
{
	CoordinateSequenceArray() {}
	~CoordinateSequenceArray(){
		pcss.clear();
	}
	void addCS(CoordinateSequence* pcs){
		pcss.push_back(std::shared_ptr<CoordinateSequence>((CoordinateSequence*)pcs));
	}
	int getSize(){
		return pcss.size();
	}
	CoordinateSequence* getCS(int index){
		return pcss[index].get();
	}

	std::vector<std::shared_ptr<CoordinateSequence> >pcss;
};

/*void FillRegionWithPoint(CoordinateSequenceArray* pregions,
	vector2d ref_point, float h_interval, float v_interval, float col_offset,
	float cell_h_width, float cell_h_height,CoordinateSequence* res);*/

void FillRegionWithPointPrecise(CoordinateSequenceArray* pregions,
	vector2d ref_point, float h_interval, float v_interval, float col_offset,
	float cell_h_width, float cell_h_height,CoordinateSequence* res);

/*void FillSurfaceWithPoint(CGeometry* psf, 
	vector2d ref_point, float h_interval, float v_interval, float col_offset,
	float cell_h_width, float cell_h_height,std::vector<vector2d>& res);*/

void FillSurfaceWithPointPrecise(CGeometry* pGeo,
	vector2d ref_point, float h_interval, float v_interval, float col_offset,
	float cell_h_width, float cell_h_height,std::vector<vector2d>& res);
}


#endif





















