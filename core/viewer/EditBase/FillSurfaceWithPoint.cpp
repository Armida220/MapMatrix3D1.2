#include "stdafx.h"
#include <float.h>
#include "SmartViewBaseType.h"
#include "FillSurfaceWithPoint.h"

namespace fillregion
{

struct FillCell
{
	int flag = 0;
};

struct FillRow
{
	vector2d start_point;
	double bottom_y = 0;
	double top_y = 0;
	std::vector<FillCell> cells;
	std::vector<vector2d> points;
	//
	void fillCells(float h_interval, double end_x)
	{
		cells.resize((end_x - start_point.x) / h_interval + 0.1, FillCell());
	}
};

void  FillRegionWithPointPrecise(CoordinateSequenceArray* pregions,
	vector2d ref_point, float h_interval, float v_interval, float col_offset,
	float cell_h_width, float cell_h_height, CoordinateSequence* res)
{
	if (pregions == NULL)
		return;
	CoordinateSequenceArray* prings = (CoordinateSequenceArray*)pregions;
	//calculate AABB;
	box aabb;
	for (int i = 0; i<pregions->getSize(); ++i)
	{
		CoordinateSequence* pring = (CoordinateSequence*)prings->getCS(i);
		for (int j = 0; j<pring->coordinates.size(); ++j)
		{
			if (pring->coordinates[j].x < aabb.minx)  aabb.minx = pring->coordinates[j].x;
			if (pring->coordinates[j].x > aabb.maxx)  aabb.maxx = pring->coordinates[j].x;
			if (pring->coordinates[j].y < aabb.miny)  aabb.miny = pring->coordinates[j].y;
			if (pring->coordinates[j].y > aabb.maxy)  aabb.maxy = pring->coordinates[j].y;
		}
	}
	//
	bool cell_constraint_x = cell_h_width>tolerance ? true : false;
	bool cell_constraint_y = cell_h_height>tolerance ? true : false;
	//calculate fill boundary;
	double critical_top = ceil((aabb.maxy + 1 - ref_point.y) / v_interval)*v_interval + ref_point.y;
	double critical_bottom = floor((aabb.miny - 1 - ref_point.y) / v_interval)*v_interval + ref_point.y;
	double critical_left = (floor((aabb.minx - ref_point.x) / h_interval) - 1)*h_interval + ref_point.x;
	//
	int max_res_size = 0;
	//init all filling rows;
	int nsize = (critical_top - critical_bottom) / v_interval + 0.1;
	if (nsize <= 0 || nsize > 10000)
		return;
	std::vector<FillRow> fill_rows(nsize, FillRow());
	for (int i = 0; i < fill_rows.size(); ++i)
	{
		fill_rows[i].start_point.y = critical_bottom + i*v_interval;
		double temp_interval = i*col_offset;
		fill_rows[i].start_point.x = (critical_left + temp_interval) - floor(temp_interval / h_interval)*h_interval;
		fill_rows[i].bottom_y = fill_rows[i].start_point.y - cell_h_height;
		fill_rows[i].top_y = fill_rows[i].start_point.y + cell_h_height;
		//
		fill_rows[i].fillCells(h_interval, aabb.maxx + h_interval);
		max_res_size += fill_rows[i].cells.size();
	}
	std::vector<vector2d> temp_res(max_res_size, vector2d());
	int real_res_size = 0;
	//
	for (int k = 0; k < prings->pcss.size(); ++k)
	{
		CoordinateSequence* pring = prings->pcss[k].get();
		//
		int ring_size = pring->getSize();
		for (int t = 1; t < ring_size; ++t)
		{
			vector2d rp = pring->coordinates[(t - 2 + ring_size) % ring_size];
			vector2d sp = pring->coordinates[t - 1];
			vector2d ep = pring->coordinates[t];
			//
			if (sp.y > ep.y)
			{
				vector2d temp = sp;
				sp = ep;
				ep = temp;
				//
				rp = pring->coordinates[(t + 1 + ring_size) % ring_size];
			}
			//
			int bottom_row_index = floor((sp.y - critical_bottom) / v_interval);
			bottom_row_index = bottom_row_index < 0 ? 0 : bottom_row_index;
			int top_row_index = ceil((ep.y - critical_bottom) / v_interval);
			top_row_index = top_row_index >= fill_rows.size() ? fill_rows.size() - 1 : top_row_index;
			//
			for (int row_index = bottom_row_index; row_index <= top_row_index; ++row_index)
			{
				//find covered cells;
				{
					double sx = DBL_MAX;
					double ex = DBL_MAX;
					//
					if (sp.y >= fill_rows[row_index].top_y)
						continue;
					else if (sp.y < fill_rows[row_index].bottom_y && ep.y > fill_rows[row_index].bottom_y)
					{
						sx = sp.x + ((ep.x - sp.x) / (ep.y - sp.y))*(fill_rows[row_index].bottom_y - sp.y);
					}
					else if (sp.y >= fill_rows[row_index].bottom_y&& sp.y <= fill_rows[row_index].top_y)
					{
						sx = sp.x;
					}
					//
					if (ep.y <= fill_rows[row_index].bottom_y)
						continue;
					else if (sp.y < fill_rows[row_index].top_y && ep.y > fill_rows[row_index].top_y)
					{
						ex = sp.x + ((ep.x - sp.x) / (ep.y - sp.y))*(fill_rows[row_index].top_y - sp.y);
					}
					else if (ep.y >= fill_rows[row_index].bottom_y && ep.y <= fill_rows[row_index].top_y)
					{
						ex = ep.x;
					}
					//
					if (sx != DBL_MAX || ex != DBL_MAX)
					{
						if (sx > ex)
						{
							auto temp = sx;
							sx = ex;
							ex = temp;
						}
						//
						int start_cell_index = INT_MAX;
						int end_cell_index = INT_MAX;
						//
						{
							float index = (sx - fill_rows[row_index].start_point.x) / h_interval;
							int pre_index = floor(index);
							int after_index = ceil(index);
							if (fill_rows[row_index].start_point.x + h_interval*pre_index + cell_h_width > sx)
								start_cell_index = pre_index;
							else
								start_cell_index = after_index;
						};
						{
							float index = (ex - fill_rows[row_index].start_point.x) / h_interval;
							int pre_index = floor(index);
							int after_index = ceil(index);
							if (fill_rows[row_index].start_point.x + h_interval*after_index - cell_h_width < ex)
								end_cell_index = after_index;
							else
								end_cell_index = pre_index;
						};
						//
						if (end_cell_index >= start_cell_index) {
							for (int temp_index = start_cell_index; temp_index <= end_cell_index; ++temp_index)
							{
								if (temp_index >= 0 && temp_index <fill_rows[row_index].cells.size())
									fill_rows[row_index].cells[temp_index].flag = COVER_EDGE;
							}
						}
					}
				}
				//find intersection points;
				{
					if (sp.y - fill_rows[row_index].start_point.y > heigh_tolerance ||
						fill_rows[row_index].start_point.y - ep.y > heigh_tolerance)
						continue;
					//
					if (sp.y + heigh_tolerance < fill_rows[row_index].start_point.y && ep.y - heigh_tolerance > fill_rows[row_index].start_point.y)
					{
						//calculate the intersection point from ray and segment; 
						vector2d sp_ep(ep.x - sp.x, ep.y - sp.y);
						vector2d intersect_p;
						intersect_p.y = fill_rows[row_index].start_point.y;
						intersect_p.x = sp.x + (sp_ep.x / sp_ep.y)*(intersect_p.y - sp.y);
						intersect_p.flag = INTERSECTION;
						fill_rows[row_index].points.push_back(intersect_p);
					}
					else if (fabs(sp.y - fill_rows[row_index].start_point.y) <= heigh_tolerance)
					{
						if (fabs(rp.y - fill_rows[row_index].start_point.y) <= heigh_tolerance)
							rp.y = fill_rows[row_index].start_point.y;
						if (fabs(ep.y - fill_rows[row_index].start_point.y) <= heigh_tolerance)
							ep.y = fill_rows[row_index].start_point.y;
						bool is_through = (rp.y - fill_rows[row_index].start_point.y)*(ep.y - fill_rows[row_index].start_point.y) < 0;
						//
						vector2d p(sp.x, fill_rows[row_index].start_point.y, sp.z);
						p.flag = is_through ? INTERSECTION : INTERSECTION_VERTEX;
						fill_rows[row_index].points.push_back(p);
					}
				}
				//
			}
		}
	}
	//sort all insertect points;
	for (int i = 0; i < fill_rows.size(); ++i)
	{
		std::stable_sort(fill_rows[i].points.begin(), fill_rows[i].points.end(), point_sort_func);
	}
	//find filling points;
	for (int i = 0; i < fill_rows.size(); ++i)
	{
		//find legal region;
		std::vector < std::pair<double, double> > legal_region;
		while (fill_rows[i].points.size() > 0 && fill_rows[i].points.begin()->flag != INTERSECTION)
		{
			fill_rows[i].points.erase(fill_rows[i].points.begin());
		}
		//
		int last_state = ENTER_POLYGON;
		if (fill_rows[i].points.size() >= 2)
			legal_region.push_back(std::pair<double, double>(fill_rows[i].points[0].x, fill_rows[i].points[1].x));
		for (int m = 2; m < fill_rows[i].points.size(); ++m)
		{
			vector2d& ca = fill_rows[i].points[m - 1];
			vector2d& cb = fill_rows[i].points[m];
			//
			if (last_state == ENTER_POLYGON)
			{
				if (ca.flag == INTERSECTION)
				{
					last_state = LEAVE_POLYGON;
					continue;
				}
				if (ca.flag == INTERSECTION_VERTEX)
				{
					legal_region.push_back(std::pair<double, double>(ca.x, cb.x));
					last_state = ENTER_POLYGON;
					continue;
				}
			}
			else if (last_state == LEAVE_POLYGON)
			{
				if (ca.flag == INTERSECTION)
				{
					legal_region.push_back(std::pair<double, double>(ca.x, cb.x));
					last_state = ENTER_POLYGON;
					continue;
				}
				if (ca.flag == INTERSECTION_VERTEX)
				{
					last_state = LEAVE_POLYGON;
					continue;
				}
			}
		}
		//
		//generate filling points;
		//for (int t = 0; t < legal_fill_region.size(); ++t)
		for (auto itr = legal_region.begin(); itr != legal_region.end(); ++itr)
		{
			if (itr->first == itr->second)
				continue;
			//
			int st = ceil((itr->first - fill_rows[i].start_point.x) / h_interval);
			double sx = fill_rows[i].start_point.x + st*h_interval;
			while (sx < itr->second)
			{
				if (fill_rows[i].cells[st].flag == COVER_EDGE)
				{
					++st;
					sx += h_interval;
					continue;
				}
				//
				temp_res[real_res_size].x = sx;
				temp_res[real_res_size].y = fill_rows[i].start_point.y;
				temp_res[real_res_size].z = 0;
				++real_res_size;
				//
				++st;
				sx += h_interval;
			}
		}
	}
	//
	((CoordinateSequence*)res)->coordinates.insert(((CoordinateSequence*)res)->coordinates.begin(), temp_res.begin(), temp_res.begin() + real_res_size);
}

//////////////////////////////////////////
/*void FillSurfaceWithPoint(CGeometry* psf,
	vector2d ref_point, float h_interval, float v_interval, float col_offset,
	float cell_h_width, float cell_h_height,std::vector<vector2d>& res)
{
	CoordinateSequenceArray* rings = new CoordinateSequenceArray();
    //separate rings;
	CArray<PT_3DEX, PT_3DEX> shape;
	psf->GetShape(shape);
	CoordinateSequence* plr = NULL;
    for(int i=0; i<shape.GetSize(); ++i)
	{
		if(i==0 || (shape[i].pencode == penMove && i<shape.GetSize()-1))
		{
			if(plr != NULL)
			{
				if(plr->getSize()>=4)
					rings->addCS(plr);
				//
				plr = NULL;
			}
			//
			plr = new CoordinateSequence;
			plr->addCoordinate(vector2d(shape[i].x, shape[i].y, shape[i].z));
		}
		else
		{
			vector2d p(shape[i].x, shape[i].y, shape[i].z);
			vector2d& last = plr->getCoordinate(plr->getSize()-1);
			if(fabs(p.x - last.x)<tolerance && fabs(p.y - last.y)<tolerance)
				continue;
			else
				plr->addCoordinate(p);
		}
	}
	if(plr->getSize()>=4)
		rings->addCS(plr);
	//
	CoordinateSequence* temp_res = new CoordinateSequence;
	FillRegionWithPoint(rings,vector2d(ref_point.x,ref_point.y),h_interval,v_interval,col_offset,cell_h_width,cell_h_height,temp_res);
	delete rings;
	res.resize(temp_res->getSize(), vector2d());
	for(int t=0; t<temp_res->getSize(); ++t)
	{
		vector2d& temp = temp_res->getCoordinate(t);
		res[t].x = temp.x;
		res[t].y = temp.y;
		res[t].z = temp.z;
	}
	delete temp_res;
}
*/

void FillSurfaceWithPointPrecise(CGeometry* pGeo,
	vector2d ref_point, float h_interval, float v_interval, float col_offset,
	float cell_h_width, float cell_h_height, std::vector<vector2d>& res)
{
	CoordinateSequenceArray* rings = new CoordinateSequenceArray();
    //separate rings;
	CArray<PT_3DEX, PT_3DEX> shape;
	pGeo->GetShape(shape);
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !((CGeoCurveBase*)pGeo)->IsClosed())
	{
		shape.Add(shape[0]);
	}
	CoordinateSequence* plr = NULL;
    for(int i=0; i<shape.GetSize(); ++i)
	{
		if(i==0 || (shape[i].pencode == penMove && i<shape.GetSize()-1))
		{
			if(plr != NULL)
			{
				if(plr->getSize()>=4)
					rings->addCS(plr);
				//
				plr = NULL;
			}
			//
			plr = new CoordinateSequence();
			plr->addCoordinate(vector2d(shape[i].x, shape[i].y, shape[i].z));
		}
		else
		{
			vector2d p(shape[i].x, shape[i].y, shape[i].z);
			vector2d& last = plr->getCoordinate(plr->getSize()-1);
			if(fabs(p.x - last.x)<tolerance && fabs(p.y - last.y)<tolerance)
				continue;
			else
				plr->addCoordinate(p);
		}
	}
	if(plr->getSize()>=4)
		rings->addCS(plr);
	//
	CoordinateSequence* temp_res = new CoordinateSequence();
	FillRegionWithPointPrecise(rings,vector2d(ref_point.x,ref_point.y),h_interval,v_interval,col_offset,cell_h_width,cell_h_height,temp_res);
	delete rings;
	res.resize(temp_res->getSize(), vector2d());
	for(int t=0; t<temp_res->getSize(); ++t)
	{
		vector2d& temp = temp_res->getCoordinate(t);
		res[t].x = temp.x;
		res[t].y = temp.y;
		res[t].z = temp.z;
	}
	delete temp_res;
}


}