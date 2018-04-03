#include "stdafx.h"
#include "geometry.h"
#include <algorithm>

using namespace std;

#define GEOMETRYLIBDLL  

namespace CL
{	
	//判断点在线段上
	bool IsPointOnLine(double px0, double py0, double px1, double py1, double px2, double py2)
	{
		bool flag = false;   
		double d1 = (px1 - px0) * (py2 - py0) - (px2 - px0) * (py1 - py0);
		if ((abs(d1) < EPSILON)&&((px0 - px1) * (px0 - px2) <= 0)&&((py0 - py1) * (py0 - py2) <= 0)) 
		{   
			flag = true;   
		}
		return flag;   
	}

	//判断两线段相交
	bool IsIntersect(double px1, double py1, double px2, double py2, double px3, double py3, double px4, double py4) 
	{         
		bool flag = false;   
		double d = (px2 - px1) * (py4 - py3) - (py2 - py1) * (px4 - px3);   
		if (d != 0) 
		{   
			double r = ((py1 - py3) * (px4 - px3) - (px1 - px3) * (py4 - py3))/d;   
			double s = ((py1 - py3) * (px2 - px1) - (px1 - px3) * (py2 - py1))/d;   
			if ((r >= 0) && (r <= 1) && (s >= 0) && (s <= 1)) 
			{ 
				flag = true;   
			}	
		} 		
		return flag; 
	}   

	//判断点在多边形内
	bool Point_In_Polygon_2D(double x, double y, const vector<CL::Vec2d> &POL)
	{
		bool isInside = false; 
		int count = 0;   

		double px = x;
		double py = y;
		double linePoint1x = x;   
		double linePoint1y = y;   
		double linePoint2x = -99999999.9;//0;       
		double linePoint2y = y;      

		for(int i = 0; i < POL.size()-1; i++)
		{
			double cx1 = POL[i].x;   
			double cy1 = POL[i].y;    
			double cx2 = POL[i+1].x;    
			double cy2 = POL[i+1].y;  

			if(IsPointOnLine(px, py, cx1, cy1, cx2, cy2))    
			{
				return true; 		
			}

			if (fabs(cy2 - cy1) < EPSILON)   //平行则不相交
			{
				continue;   
			}   

			if (IsPointOnLine(cx1, cy1, linePoint1x, linePoint1y, linePoint2x, linePoint2y)) 
			{   
				if (cy1 > cy2)   
				{
					count++; 
				}
			} 
			else if (IsPointOnLine(cx2, cy2, linePoint1x, linePoint1y, linePoint2x, linePoint2y))
			{ 
				if (cy2 > cy1)  
				{
					count++; 
				}
			} 
			else if (IsIntersect(cx1, cy1, cx2, cy2, linePoint1x, linePoint1y, linePoint2x, linePoint2y))   //已经排除平行的情况
			{ 
				count++;  
			}   
		}

		if (count % 2 == 1) 
		{   
			isInside = true;
		}   

		return isInside;		
	}

	bool Line_In_Polygon_2D(const vector<CL::Vec2d> &lines, const vector<CL::Vec2d> &POL)
	{
		//点是否在里面
		for (int i = 0; i < lines.size(); i++)
		{
			if (Point_In_Polygon_2D(lines.at(i).x, lines.at(i).y, POL))
			{
				return true;
			}
		}

		//线是否在里面
		for (int i = 0; i < lines.size() - 1; i++)
		{
			for (int j = 0; j < POL.size() - 1; j++)
			{
				if (IsIntersect(lines.at(i).x, lines.at(i).y, lines.at(i + 1).x, 
					lines.at(i + 1).y, POL.at(j).x, POL.at(j).y, POL.at(j + 1).x, POL.at(j + 1).y))
				{
					return true;
				}
			}
		}

		return false;
	}

	//计算两线段交点,请判线段是否相交(同时还是要判断是否平行!)
	CL::Vec2d GetLineSegmentIntersection(CL::Vec2d u1, CL::Vec2d u2, CL::Vec2d v1, CL::Vec2d v2)
	{
		CL::Vec2d ret = u1;
		double t = ((u1.x - v1.x) * (v1.y - v2.y) - (u1.y - v1.y) * (v1.x - v2.x)) / ((u1.x - u2.x) * (v1.y - v2.y) - (u1.y - u2.y) * (v1.x - v2.x));
		ret.x += (u2.x - u1.x) * t;
		ret.y += (u2.y - u1.y) * t;

		return ret;
	}

};

