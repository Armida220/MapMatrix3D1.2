// TINHandler.cpp: implementation of the TINHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TINHandler.h"
#include <algorithm>
#include <list>
#include "smartviewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


namespace tin2d
{

double TOLERANCE = GraphAPI::g_lfDisTolerance;

static bool compare_point(const point2d& pf, const point2d& ps)
{
	if (pf.x - ps.x < -TOLERANCE)
		return true;
	else if (pf.x - ps.x > TOLERANCE)
		return false;
	else
		return pf.vt < ps.vt;
	//	return pf.x < ps.x;
}

static bool compare_edge(const edge2d& ef, const edge2d& es)
{
	if (ef.sp.x - es.sp.x < -TOLERANCE)
		return true;
	else if (ef.sp.x - es.sp.x > TOLERANCE)
		return false;
	else if (ef.sp.y - es.sp.y < -TOLERANCE)
		return true;
	else if (ef.sp.y - es.sp.y > TOLERANCE)
		return false;
	else if (ef.ep.x - es.ep.x < -TOLERANCE)
		return true;
	else if (ef.ep.x - es.ep.x > TOLERANCE)
		return false;
	else
		return ef.ep.y < es.ep.y;
}

TINHandler::TINHandler()
{
	m_ptinwork = triangulation::ITIN2D::NEWTIN2DHandler();
	m_ptinwork->init();
	m_ptinwork->setTolerance(TOLERANCE);
}

TINHandler::~TINHandler()
{

}

void TINHandler::BuildTIN(std::vector<point2d>& original_points, std::vector<edge2d>* psegments)
{
	//将约束线段点加入到点列一起排序；
	if (psegments != NULL)
	{
		for (int i = 0; i < psegments->size(); ++i)
		{
			psegments->at(i).sp.flag = i;
			original_points.push_back(psegments->at(i).sp);
			//
			psegments->at(i).ep.flag = i;
			original_points.push_back(psegments->at(i).ep);
		}
	}
	int ori_count = original_points.size();
	//x 轴优先， 对点排序；
	std::sort(original_points.begin(), original_points.end(), compare_point);
	ori_count = original_points.size();
	//去除重复点；
	std::vector<point2d> good_points;
	int vertex_count = 0;
	for (int i = 0; i < original_points.size(); ++i)
	{
		point2d& the_p = original_points.at(i);
		//
		if (the_p.vt == VERTEX)
		{
			++vertex_count;
			if (good_points.size()>0 && good_points[good_points.size() - 1] == the_p)
				continue;
			//
			good_points.push_back(the_p);
		}
		else if (the_p.vt == SEGMENT_SP || the_p.vt == SEGMENT_EP)
		{
			if (good_points.size() == 0)
				continue;
			//
			point2d& pre_p = original_points.at(i - 1);
			if (pre_p.vt != VERTEX && pre_p.vt != VERANDSEG_SP && pre_p.vt != VERANDSEG_EP)
				continue;
			//
			if (the_p == pre_p)
			{
				the_p.x = pre_p.x; the_p.y = pre_p.y; the_p.z = pre_p.z;
				//
				if (the_p.vt == SEGMENT_SP)
				{
					psegments->at(the_p.flag).sp = the_p;
					the_p.vt = VERANDSEG_SP;
				}
				else if (the_p.vt == SEGMENT_EP)
				{
					psegments->at(the_p.flag).ep = the_p;
					the_p.vt = VERANDSEG_EP;
				}
			}
		}
		else
		{
			continue;
		}
	}
	original_points.clear();
	//
	int point_count = good_points.size();
	triangulation::point_3d* points = new triangulation::point_3d[point_count];
	for (i = 0; i < point_count; ++i)
	{
		points[i].x = good_points[i].x;
		points[i].y = good_points[i].y;
		points[i].z = good_points[i].z;
		//
		points[i].constraint_id = -1;
		points[i].flag = -1;
	}
	good_points.clear();
	//构网；
	m_ptinwork->insertPoints(points, point_count);
	//
	delete[] points;
	points = NULL;
	//
	if (psegments != NULL)
	{
		//保证边的起点在左边；
		for (i = 0; i < psegments->size(); ++i)
		{
			edge2d& edge = psegments->at(i);
			if (edge.sp.x - edge.ep.x < -TOLERANCE) {
				continue;
			}
			else if (edge.sp.x - edge.ep.x > TOLERANCE) {
				point2d temp = edge.sp;
				edge.sp = edge.ep; edge.sp.vt = SEGMENT_SP;
				edge.ep = temp; edge.ep.vt = SEGMENT_EP;
			}
			else if (edge.sp.y - edge.ep.y <= -TOLERANCE) {
				continue;
			}
			else {
				point2d temp = edge.sp;
				edge.sp = edge.ep; edge.sp.vt = SEGMENT_SP;
				edge.ep = temp; edge.ep.vt = SEGMENT_EP;
			}
		}
		//对边排序，x 轴优先排， x 轴从左到右， y 轴从下到上；
		std::sort(psegments->begin(), psegments->end(), compare_edge);
		//去除重复边并添加约束；
		std::vector<edge2d> good_edges;
		edge2d* last_segment = NULL;
		for (i = 0; i < psegments->size(); ++i)
		{
			edge2d* segment = &psegments->at(i);
			if (segment->ep == segment->sp)
				continue;
			//
			if (i == 0)
			{
				triangulation::point_3d pa, pb;
				pa.x = segment->sp.x; pa.y = segment->sp.y; pa.z = segment->sp.z; pa.constraint_id = segment->constraint_id;
				pb.x = segment->ep.x; pb.y = segment->ep.y; pb.z = segment->ep.z; pb.constraint_id = segment->constraint_id;
				//
				m_ptinwork->insertSegment(pa, pb);
				//
				last_segment = segment;
			}
			else
			{
				if ((last_segment->sp == psegments->at(i).sp &&
					last_segment->ep == psegments->at(i).ep))
				{
					last_segment = segment;
					continue;
				}
				//
				triangulation::point_3d pa, pb;
				pa.x = segment->sp.x; pa.y = segment->sp.y; pa.z = segment->sp.z; pa.constraint_id = segment->constraint_id;
				pb.x = segment->ep.x; pb.y = segment->ep.y; pb.z = segment->ep.z; pb.constraint_id = segment->constraint_id;
				//
				m_ptinwork->insertSegment(pa, pb);
				//
				last_segment = segment;
			}
		}
	}
	//
	m_ptinwork->refreshTriangleInfo();
}

BOOL TINHandler::insertPoint(const point2d& p)
{
	triangulation::point_3d point;
	point.x = p.x; point.y = p.y; point.z = p.z;
	if (m_ptinwork->insertPoint(point))
	{
		m_ptinwork->refreshTriangleInfo();
		return TRUE;
	}
	//
	return FALSE;
}

BOOL TINHandler::deletePoint(const point2d& p)
{
	triangulation::point_3d point;
	point.x = p.x; point.y = p.y; point.z = p.z;
	if (m_ptinwork->deletePoint(point))
	{
		m_ptinwork->refreshTriangleInfo();
		return TRUE;
	}
	//
	return FALSE;
}

BOOL TINHandler::insertSegment(const edge2d& edge)
{
	triangulation::point_3d pa, pb;
	pa.x = edge.sp.x; pa.y = edge.sp.y; pa.z = edge.sp.z;
	pb.x = edge.ep.x; pb.y = edge.ep.y; pb.z = edge.ep.z;
	if (m_ptinwork->insertSegment(pa, pb))
	{
		m_ptinwork->refreshTriangleInfo();
		return TRUE;
	}
	//
	return FALSE;
}

BOOL TINHandler::deleteSegment(const edge2d& edge)
{
	triangulation::point_3d pa, pb;
	pa.x = edge.sp.x; pa.y = edge.sp.y; pa.z = edge.sp.z;
	pb.x = edge.ep.x; pb.y = edge.ep.y; pb.z = edge.ep.z;
	if (m_ptinwork->deleteSegment(pa, pb))
	{
		m_ptinwork->refreshTriangleInfo();
		return TRUE;
	}
	//
	return FALSE;
}

unsigned int TINHandler::getTriangleCount()
{
	return m_ptinwork->getTriangleCount();
}

triangle2d TINHandler::getTriangle(unsigned int index)
{
	if (index < 0 || index >= m_ptinwork->getTriangleCount())
		return triangle2d();
	//
	triangle2d temp;
	triangulation::triangle_3d ta = m_ptinwork->getTriangle(index);
	temp.pl.x = ta.pa.x;
	temp.pl.y = ta.pa.y;
	temp.pl.z = ta.pa.z;
	temp.pl.constraint_id = ta.pa.constraint_id;

	temp.pm.x = ta.pb.x;
	temp.pm.y = ta.pb.y;
	temp.pm.z = ta.pb.z;
	temp.pm.constraint_id = ta.pb.constraint_id;

	temp.pr.x = ta.pc.x;
	temp.pr.y = ta.pc.y;
	temp.pr.z = ta.pc.z;
	temp.pr.constraint_id = ta.pc.constraint_id;

	return temp;
}

unsigned int TINHandler::getEdgeCount()
{
	return m_ptinwork->getEdgeCount();
}

edge2d TINHandler::getEdge(int index)
{
	if (index < 0 || index >= m_ptinwork->getEdgeCount())
		return edge2d();
	//
	triangulation::edge_3d temp = m_ptinwork->getEdge(index);
	edge2d edge;
	edge.sp.x = temp.sp.x;
	edge.sp.y = temp.sp.y;
	edge.sp.z = temp.sp.z;


	edge.ep.x = temp.ep.x;
	edge.ep.y = temp.ep.y;
	edge.ep.z = temp.ep.z;

	return edge;
}

bool TINHandler::isConstraint(int edge_index)
{
	return m_ptinwork->isConstraint(edge_index);
}

void TINHandler::getTriangleContainEdge(triangle2d(&res)[2], int edge_index)
{
	if (m_ptinwork->getTriangleCount() <= 0 || edge_index < 0 || edge_index >= m_ptinwork->getEdgeCount())
		return;
	//
	triangulation::triangle_3d temp[2];
	m_ptinwork->getTriangleContainEdge(temp, edge_index);
	//
	for (int i = 0; i < 2; ++i)
	{
		res[i].pl.x = temp[i].pa.x;
		res[i].pl.y = temp[i].pa.y;
		res[i].pl.z = temp[i].pa.z;
		res[i].pl.constraint_id = temp[i].pa.constraint_id;

		res[i].pr.x = temp[i].pb.x;
		res[i].pr.y = temp[i].pb.y;
		res[i].pr.z = temp[i].pb.z;
		res[i].pr.constraint_id = temp[i].pb.constraint_id;

		res[i].pm.x = temp[i].pc.x;
		res[i].pm.y = temp[i].pc.y;
		res[i].pm.z = temp[i].pc.z;
		res[i].pm.constraint_id = temp[i].pc.constraint_id;
	}
}

void TINHandler::clear()
{
	m_ptinwork->clear();
}


}