// TINHandler.h: interface for the TINHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TINHANDLER_H__000F8CF5_14FF_400E_BD39_1EF06334D9EB__INCLUDED_)
#define AFX_TINHANDLER_H__000F8CF5_14FF_400E_BD39_1EF06334D9EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <list>
#include <algorithm>

#include "TIN.h"

#pragma comment(lib,"Triangulation.lib")

namespace tin2d
{

extern double TOLERANCE;
/////

enum VertexType
{
	VERTEX,
	SEGMENT_SP,
	SEGMENT_EP,
	VERANDSEG_SP,
	VERANDSEG_EP
};


struct point2d
{
	double x;
	double y;
	double z;
	VertexType vt;
	int constraint_id;
	int flag;
	point2d()
	{
		x = 0; y = 0; z = 0;
		vt = VERTEX;
		flag = -1;
		constraint_id = -1;
	}

	bool operator==(const point2d& rv)
	{
		if (fabs(x - rv.x) <= TOLERANCE && fabs(y - rv.y) <= TOLERANCE)
			return true;
		else
			return false;
	}

	bool operator!=(const point2d& rv)
	{
		if (fabs(x - rv.x) <= TOLERANCE && fabs(y - rv.y) <= TOLERANCE)
			return false;
		else
			return true;
	}


	point2d& operator=(const point2d& rv)
	{
		this->x = rv.x;
		this->y = rv.y;
		this->z = rv.z;
		this->constraint_id = rv.constraint_id;
		this->vt = rv.vt;
		this->flag = rv.flag;
		//
		return *this;
	}

	point2d& operator-(const point2d& rv)
	{
		this->x -= rv.x;
		this->y -= rv.y;
		return *this;
	}

	point2d& operator+(const point2d& rv)
	{
		this->x += rv.x;
		this->y += rv.y;
		return *this;
	}
};

struct edge2d
{
	point2d sp;
	point2d ep;
	//
	int constraint_id;
	int flag;

	edge2d()
	{
		sp.vt = SEGMENT_SP;
		ep.vt = SEGMENT_EP;
		//
		constraint_id = -1;
		flag = -1;
	}
};

struct triangle2d
{
	point2d pl;
	point2d pm;
	point2d pr;
};

class TINHandler
{
public:
	TINHandler();
	~TINHandler();
	void BuildTIN(std::vector<point2d>& original_points, std::vector<edge2d>* psegments);

	BOOL insertPoint(const point2d& p);
	BOOL deletePoint(const point2d& p);

	BOOL insertSegment(const edge2d& edge);
	BOOL deleteSegment(const edge2d& edge);

	unsigned int getTriangleCount();
	triangle2d getTriangle(unsigned int index);
	unsigned int getEdgeCount();
	edge2d getEdge(int index);

	bool isConstraint(int edge_index);

	void getTriangleContainEdge(triangle2d(&res)[2], int edge_index);

	void clear();
private:
	triangulation::ITIN2D* m_ptinwork;
};


}

#endif // !defined(AFX_TINHANDLER_H__000F8CF5_14FF_400E_BD39_1EF06334D9EB__INCLUDED_)
