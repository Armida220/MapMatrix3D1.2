#include "StdAfx.h"
#include "PolygonBooleanHander.h"
#include "SmartViewFunctions.h"

namespace pbh
{

	double TOLERANCE  = 1e-4;


	double point2d::invalid_z = -99999999;




	int sign_compare(double x, double toler)
	{
		if (fabs(x)<toler)
		{
			return 0;
		}
		else 
			return  x>0?1:-1;
	}

	int  GIsPtInRegion(point2d& pt0, std::vector<point2d>& pPts)
	{
		int    i,j ,  sum=0;   
		double    xmin,   ymin,   xmax,   ymax;   
		double   x;   

		int nNum = pPts.size();

		point2d* pPt = &pt0;
		
		//检验传入参数的合法性
  		if (pPt==NULL||nNum<3)
		{
			return -2;
		}	
		for(i=0;   i<nNum;   i++)   
		{
			//j=(i+1)%nNum;
			if (i==nNum-1)
			{
				j=0;
			}
			else
				j=(i+1);		
			xmin=min(pPts[i].x,pPts[j].x);   
			xmax=max(pPts[i].x,pPts[j].x);   
			ymin=min(pPts[i].y,pPts[j].y);   
			ymax=max(pPts[i].y,pPts[j].y); 
			
			//水平射线不可能与轮廓线段i->j有交点 
			if(pPt->y>ymax+TOLERANCE||pPt->y<ymin-TOLERANCE||pPt->x>xmax+TOLERANCE)   
				continue;   

			//可能经过线段i->j某个顶点
			if(fabs(pPt->y-pPts[i].y)<TOLERANCE||fabs(pPt->y-pPts[j].y)<TOLERANCE)
			{   
				//线段平行 
				if(fabs(pPts[i].y-pPts[j].y)<TOLERANCE)//此处不能用ymin，ymax  
				{
					// 检验点在线段端点上
					if (fabs(pPt->x-xmin)<TOLERANCE||fabs(pPt->x-xmax)<TOLERANCE)
					{
						return 0;
					}
					// 检验点在线段上
					else if (pPt->x>xmin&&pPt->x<xmax)
					{
						return 1;
					}
					// 其他情况直接进入下次循环
					else
						continue;
				}
				// 线段不平行
				else 
				{   
					// 经过i对应的点
					if (fabs(pPt->y-pPts[i].y)<TOLERANCE)
					{
						// 检验点与i对应的点重合
						if (fabs(pPt->x-pPts[i].x)<TOLERANCE)
						{
							return 0;
						}
						// 水平射线过i点
						else if(pPt->x<pPts[i].x)
						{
							sum+=sign_compare(pPt->y-pPts[i].y,TOLERANCE)+sign_compare(pPts[j].y-pPt->y,TOLERANCE);
							continue;
						}
						else					
							continue;
					}
					// 经过j对应的点
					else 
					{
						if (fabs(pPt->x-pPts[j].x)<TOLERANCE)
						{
							return 0;
						}
						else if(pPt->x<pPts[j].x)
						{
							sum+=sign_compare(pPt->y-pPts[i].y,TOLERANCE)+sign_compare(pPts[j].y-pPt->y,TOLERANCE);
							continue;
						}
						else					
							continue;
					}				  
				}   			   
			}
			//不过i->j端点，且可能与i->j相交
			else
			{
				//此判断应该可以不要
				if ( pPt->y>ymin&&pPt->y<ymax)
				{
					// 首先排除必然相交的情况
					if (pPt->x<xmin-TOLERANCE)
					{
						sum+=sign_compare(pPt->y-pPts[i].y,TOLERANCE)+sign_compare(pPts[j].y-pPt->y,TOLERANCE);
						continue;
					}
					// 无法直接判断的情况最后经过计算判断
					else
					{
						x=(double)(pPt->y-pPts[i].y)/(pPts[j].y-pPts[i].y)*(pPts[j].x-pPts[i].x)+pPts[i].x;
						
						//检验点在线段上
						if(fabs(x-pPt->x)<TOLERANCE)
							return (1);

						//检验点与线段相交
						if(x>pPt->x)
						{
							sum+=sign_compare(pPt->y-pPts[i].y,TOLERANCE)+sign_compare(pPts[j].y-pPt->y,TOLERANCE);
							continue;
						}
					}
				}
			}
		}  
		if(sum==0)   return(-1);   //   p在多边形外   
		else        return(2);     //   p在多边形内 
	}


	int check_point_location(point2d& p, LinearRing* pring)
	{
	   return GIsPtInRegion(p,pring->points);
	}

	int check_point_location(point2d& p, std::vector<point2d>& pPts)
	{
		return GIsPtInRegion(p,pPts);
	}



	//-2 相离，不邻接；
	//-1 相离， 邻接；
	//0 重叠；
	//1 相交；
	//2 la 包含 lb，不邻接;
	//3 la 包含 lb，邻接；
	//4 lb 包含 la，不邻接;
	//5 lb 包含 la，邻接；
	int check_ring_relationship(LinearRing* la, LinearRing* lb)
	{
		box la_box = la->getBox();
		box lb_box = lb->getBox();
		//
		if(lb_box.minx > la_box.maxx || lb_box.maxx < la_box.minx ||
		   lb_box.miny > lb_box.maxy || lb_box.maxy < lb_box.miny )
		   return -2;
	    //
		double la_box_area = (la_box.maxx - la_box.minx)*(la_box.maxy - la_box.miny);
		double lb_box_area = (lb_box.maxx - lb_box.minx)*(lb_box.maxy - lb_box.miny);
        std::vector<point2d> *temp1, *temp2;
		if(la_box_area>=lb_box_area)
		{
			temp1 = &(la->points);
		    temp2 = &(lb->points);
		}
		else
		{
			temp2 = &(la->points);
		    temp1 = &(lb->points);
			box temp = la_box;
			la_box = lb_box;
			lb_box = temp;
		}
        std::vector<point2d>& pts_a = *temp1;
		std::vector<point2d>& pts_b = *temp2;
		//
		bool lb_has_point_out_of_la = false;
		bool lb_has_point_in_la = false;
		bool adjoin = false;
		bool edge_adjoin = false;
		int overlap_vertex_count = 0;

		for(int i=1; i<pts_a.size(); ++i)
		{
            point2d& eas = pts_a[i-1];
			point2d& eae = pts_a[i];
		    
			double ea_minx = min(eas.x,eae.x);  double ea_maxx = max(eas.x,eae.x);		
			double ea_miny = min(eas.y,eae.y);  double ea_maxy = max(eas.y,eae.y);
			
			for(int j=1; j<pts_b.size(); ++j)
			{
               point2d& ebs = pts_b[j-1];
			   point2d& ebe = pts_b[j];

			   double eb_minx = min(ebs.x,ebe.x);  double eb_maxx = max(ebs.x,ebe.x);
			   double eb_miny = min(ebs.y,ebe.y);  double eb_maxy = max(ebs.y,ebe.y);
			   //
			   if(eb_minx > la_box.maxx+TOLERANCE || eb_maxx < la_box.minx-TOLERANCE || 
				  eb_miny > la_box.maxy+TOLERANCE || eb_maxy < la_box.miny-TOLERANCE)
			   {
				   lb_has_point_out_of_la = true;
				   continue;
			   }
			   //
			   point2d va(eae.x-eas.x, eae.y-eas.y);
			   point2d vb(ebe.x-ebs.x, ebe.y-ebs.y);
			   //
			   point2d cp;
			   double t0,t1;
			   bool is_ea_eb_crossed = false;
			   bool is_ea_eb_parall = fabs(va.x*vb.y - va.y*vb.x)<TOLERANCE?true:false;
			   //
			   if((eas==ebs || eas==ebe) && (eae==ebe || eae == ebs))
					edge_adjoin = true;
			   else if(is_ea_eb_parall && GraphAPI::GGetDisofPtToLine(eas.x,eas.y,eae.x,eae.y,ebs.x,ebs.y)<TOLERANCE)
			   {			   
				   if(eas!=ebs && eas!=ebe && GraphAPI::GIsPtInLine(ebs,ebe,eas))
					   edge_adjoin = true;
				   else if(eae!=ebs && eae!=ebe && GraphAPI::GIsPtInLine(ebs,ebe,eae))
					   edge_adjoin = true;
                   else if(ebs!=eas && ebs!=eae && GraphAPI::GIsPtInLine(eas,eae,ebs))
					   edge_adjoin = true;
				   else if(ebe!=eas && ebe!=eae && GraphAPI::GIsPtInLine(eas,eae,ebe))
					   edge_adjoin = true;
				   else if(GraphAPI::GIsPtInLine(eas,eae,ebs) && GraphAPI::GIsPtInLine(eas,eae,ebe))
					   edge_adjoin = true;
				   else if(GraphAPI::GIsPtInLine(ebs,ebe,eas) && GraphAPI::GIsPtInLine(ebs,ebe,eae))
					   edge_adjoin = true;
			   }
			   //
			   if(i>1 && 
				  (
				  (eb_minx > ea_maxx || eb_maxx < ea_minx || eb_miny > ea_maxy || eb_maxy < ea_miny) ||
                  is_ea_eb_parall ||
				  !(is_ea_eb_crossed = GraphAPI::GGetLineIntersectLineSeg(eas.x,eas.y,eae.x,eae.y,ebs.x,ebs.y,ebe.x,ebe.y,&cp.x,&cp.y,&t0,&t1))
				  ) )
			   {
                   continue;
			   }
			   else 
			   if(eb_minx > ea_maxx || eb_maxx < ea_minx || eb_miny > ea_maxy || eb_maxy < ea_miny ||
				  is_ea_eb_parall ||
				  !(is_ea_eb_crossed = GraphAPI::GGetLineIntersectLineSeg(eas.x,eas.y,eae.x,eae.y,ebs.x,ebs.y,ebe.x,ebe.y,&cp.x,&cp.y,&t0,&t1)))
			   {
				   if(j==1)
				   {
					   int ebs_location = check_point_location(ebs,pts_a);
					   if(ebs_location == -1) {
						   lb_has_point_out_of_la = true;
						   if(lb_has_point_in_la)
							   return 1;
					   } else if(ebs_location == 2) {
						   lb_has_point_in_la = true;
						   if(lb_has_point_out_of_la)
							   return 1;
					   } else if(ebs_location == 0) {
						   ++overlap_vertex_count;
						   adjoin = true;
					   }
					   else adjoin = true;
				   }
				   //
				   int ebe_location = check_point_location(ebe,pts_a);
				   if(ebe_location == -1) {
					   lb_has_point_out_of_la = true;
					   if(lb_has_point_in_la)
						   return 1;
				   } else if(ebe_location == 2) {
					   lb_has_point_in_la = true;
					   if(lb_has_point_out_of_la)
						   return 1;
				   } else if(ebe_location == 0) {
					   ++overlap_vertex_count;
					   adjoin = true;
				   }
				   else adjoin = true;
			       //
				   continue;
			   }
			   else if(is_ea_eb_crossed)
			   {
				   if(i<=1 && cp == ebs)
				   {
					   int ebe_location = check_point_location(ebe,pts_a);
					   if(ebe_location == -1) {
						   lb_has_point_out_of_la = true;
						   if(lb_has_point_in_la)
							   return 1;
					   } else if(ebe_location == 2) {
						   lb_has_point_in_la = true;
						   if(lb_has_point_out_of_la)
							   return 1;
					   }
				       else adjoin = true;
				   }
			   }
			   //
               if(cp == eas || cp == eae || cp == ebs || cp==ebe)
			   {
				   adjoin = true;
			   }
			   else
				   return 1;
			}
		}
		//
        if(la_box_area - lb_box_area > TOLERANCE)
		{
			if(adjoin && edge_adjoin)
			{
				if(lb_has_point_in_la)
					return 3;
				if(lb_has_point_out_of_la)
					return -1;
			}
			else
			{
				if(lb_has_point_in_la)
					return 2;
				if(lb_has_point_out_of_la)
					return -2;
			}
		}
		else if(la_box_area - lb_box_area < -TOLERANCE)
		{
			if(adjoin && edge_adjoin)
			{
				if(lb_has_point_in_la)
					return 5;
				if(lb_has_point_out_of_la)
					return -1;
			}
			else
			{
				if(lb_has_point_in_la)
					return 4;
				if(lb_has_point_out_of_la)
					return -2;
			}
		}
		//
		if(adjoin && !lb_has_point_in_la && !lb_has_point_out_of_la)
		{
			if(fabs(la_box_area - lb_box_area)>TOLERANCE)
				return -1;
			else
			{
                if(overlap_vertex_count==la->getSize() && overlap_vertex_count==lb->getSize())
					return 0;
				else if(la->getSize()>overlap_vertex_count)
					return 3;
				else
					return 5;
			}
		}
		//
		return -2;
	}



	plybn::IPolygon2d* PolygonBooleanHander::plybn_from_pbh_polygon(Polygon* ppg, bool& is_valid)
	{
        plybn::IPolygon2d* pC = plybn::IPolygon2d::Create();
		int i=0, j=0;
		//
        shared_ptr<LinearRing> pouter = ppg->getOuter();
		if(pouter != NULL)
		{
			plybn::ILinearRing2d* pouter_ = plybn::ILinearRing2d::Create();
			for(i=0; i<pouter->getSize()-1; ++i)
			{
				point2d& temp = pouter->getPointRef(i);
				pouter_->addPoint(plybn::point2d(temp.x, temp.y));
			}
            point2d& temp = pouter->getPointRef(i);
			if(temp != pouter->getPointRef(0)) 
				pouter_->addPoint(plybn::point2d(temp.x, temp.y));
			//
			is_valid = pouter_->is_simple();
			if(!is_valid)
				return NULL;
			
			pC->setOuter(pouter_);
		}		
		//
		for(i=0; i<ppg->getHoleCount(); ++i)
		{
			shared_ptr<LinearRing> phole = ppg->getHole(i);
			plybn::ILinearRing2d* phole_ = plybn::ILinearRing2d::Create();
			for(j=0; j<phole->getSize()-1; ++j)
			{
				point2d& temp = phole->getPointRef(j);
				phole_->addPoint(plybn::point2d(temp.x, temp.y));
			}
			point2d& temp = phole->getPointRef(j);
			if(temp != phole->getPointRef(0))
				phole_->addPoint(plybn::point2d(temp.x, temp.y));
			//
			is_valid = phole_->is_simple();
			if(!is_valid)
				return NULL;
			
			pC->addHole(phole_);
		}
		//
		return pC;
	}

	bool PolygonBooleanHander::init()
	{
		if(outer_relationship == -2)
		{
			return true;
		}
		//
		bool is_valid;
		pA = plybn_from_pbh_polygon(pa.get(), is_valid);
		if(!is_valid)
			return false;
		pB = plybn_from_pbh_polygon(pb.get(), is_valid);
		if(!is_valid)
			return false;
		//
		return true;
	}

	bool PolygonBooleanHander::init(bool& is_pa_valid, bool& is_pb_valid)
	{
		if(outer_relationship == -2)
		{
			return true;
		}
		//
		pA = plybn_from_pbh_polygon(pa.get(), is_pa_valid);
		pB = plybn_from_pbh_polygon(pb.get(), is_pb_valid);
		//
		return (is_pa_valid && is_pb_valid);
	}


	void PolygonBooleanHander::compare_vertex()
	{
		std::vector<PointProxy> points;
		
		int i=0; int j=0;
		
		
		Polygon* ppg = pa.get();
		int flag = 1;
		
L:      LinearRing* pouter = ppg->getOuter().get();
		if(pouter != NULL)
		{
			for(i=0; i<pouter->getSize()-1; ++i)
			{
				PointProxy temp;
				temp.plr = pouter;
				temp.index = i;
				points.push_back(temp);
			}
		}
		//
		for(i=0; i<ppg->getHoleCount(); ++i)
		{
			LinearRing* phole = ppg->getHole(i).get();
            for(j=0; j<phole->getSize()-1; ++j)
			{
				PointProxy temp;
				temp.plr = phole;
				temp.index = j;
				points.push_back(temp);
			}
		}
		
		if(flag ==1)
		{
			++flag;
			ppg = pb.get();
			goto L;
		}
		
		////
		std::stable_sort(points.begin(), points.end(), PolygonBooleanHander::compare_point);
        for(i=1; i<points.size(); ++i)
		{
			if(points[i] == points[i-1])
			{
				points[i].plr->setPoint(points[i].index, points[i-1].plr->points[points[i-1].index]);
			}
		}
		//
		ppg = pa.get();
		flag = 1;
		
L1:     pouter = ppg->getOuter().get();
		if(pouter != NULL)
		{
			pouter->setPoint(pouter->getSize()-1,pouter->getPointRef(0));
		}
		//
		for(i=0; i<ppg->getHoleCount(); ++i)
		{
			LinearRing* phole = ppg->getHole(i).get();
            phole->setPoint(phole->getSize()-1,phole->getPoint(0));
		}
		
		if(flag ==1)
		{
			++flag;
			ppg = pb.get();
			goto L1;
		}
	}

	void PolygonBooleanHander::check_vertex_on_edge(Polygon* pv, Polygon* pe)
	{
		std::vector<point2d> points;
		box pe_box = pe->getBox();
		if(pv->getOuter() != NULL)
		{
			LinearRing* pouter = pv->getOuter().get();
			for(int i=0; i<pouter->getSize(); ++i)
			{
				point2d& p = pouter->getPointRef(i);
				if(p.x < pe_box.minx - TOLERANCE || p.x>pe_box.maxx + TOLERANCE ||
					p.y<pe_box.miny - TOLERANCE || p.y>pe_box.maxy + TOLERANCE )
					continue;
				else
				{
					points.push_back(p);
				}
			}
		}
		for(int i=0; i<pv->getHoleCount(); ++i)
		{
			LinearRing* phole = pv->getHole(i).get();
			for(int t=0; t<phole->getSize(); ++t)
			{
				point2d& p = phole->getPointRef(t);
				if(p.x < pe_box.minx - TOLERANCE || p.x > pe_box.maxx + TOLERANCE ||
					p.y < pe_box.miny - TOLERANCE || p.y > pe_box.maxy + TOLERANCE )
					continue;
				else
				{
					points.push_back(p);
				}
				
			}
		}
		//
		if(points.size() == 0)
			return;
		//
		std::vector<LinearRing*> rings;
		if(pe->getOuter() != NULL)
		{
			rings.push_back(pe->getOuter().get());
		}
		for(i=0; i<pe->getHoleCount(); ++i)
		{
			rings.push_back(pe->getHole(i).get());
		}
		/////
        for(i=0; i<rings.size(); ++i)
		{
			LinearRing* plr = rings[i];
			box ring_box = plr->getBox();
			//
			for(int j=0; j<points.size(); ++j)
			{
				point2d& p = points[j];
				//
				if(p.x < ring_box.minx - TOLERANCE || p.x > ring_box.maxx + TOLERANCE ||
					p.y < ring_box.miny - TOLERANCE || p.y > ring_box.maxy + TOLERANCE )
					continue;
				else
				{
					for(int t=1; t<plr->getSize(); ++t)
					{ 
						if(p !=plr->points[t-1] && p !=plr->points[t] &&  isPointOnEdge(p,plr->points[t-1],plr->points[t],NULL))
						{
							plr->insertPoint(t,p);
							break;
						}
					}
				}
			}
		}	
	}

	void PolygonBooleanHander::recoverZ()
	{
		std::vector<PointProxy> points;
		std::vector<PointProxy> cross_points;

		int i=0, j=0;

		Polygon* ppg = pa.get();
		int flag = 1;
		
L:      LinearRing* pouter = ppg->getOuter().get();
		if(pouter != NULL)
		{
			for(i=0; i<pouter->getSize(); ++i)
			{
				PointProxy temp;
				temp.plr = pouter;
				temp.index = i;
				points.push_back(temp);
			}
		}
		//
		for(i=0; i<ppg->getHoleCount(); ++i)
		{
			LinearRing* phole = ppg->getHole(i).get();
            for(j=0; j<phole->getSize(); ++j)
			{
				PointProxy temp;
				temp.plr = phole;
				temp.index = j;
				points.push_back(temp);
			}
		}
		
		if(flag > 0)
		{
			 if(flag == 1)
			 {
				 ++flag;
				 ppg = pb.get();
				 goto L;
			 }
			 else if(flag - 2 < res.getSize())
			 {  
				ppg = res.getPolygon(flag - 2).get();
				++flag;
				goto L;
			 }
 		}

		std::stable_sort(points.begin(), points.end(), PolygonBooleanHander::compare_point);
		//
		i=0;j=0;
		for(i=1; i<points.size(); ++i)
		{
			if(points[i].plr->points[points[i].index].z == point2d::invalid_z)
			{
				if(points[i-1].plr->points[points[i-1].index].z == point2d::invalid_z)
					cross_points.push_back(points[i]);
				else if(points[i] == points[i-1])
                    points[i].plr->points[points[i].index].z = points[i-1].plr->points[points[i-1].index].z;
				else
                    cross_points.push_back(points[i]);
			}
		}
		//

		std::vector<EdgeProxy> org_edges;
		std::vector<LinearRing*> org_lrs;
		//
		ppg = pa.get();
		flag = 1;

L1:		if(ppg->getOuter()!=NULL)
		org_lrs.push_back(ppg->getOuter().get());
		for(i=0; i<ppg->getHoleCount(); ++i)
		{
			org_lrs.push_back(ppg->getHole(i).get());
		}

		if(flag == 1)
		{
			++flag;
			ppg = pb.get();
			goto L1;
		}

        ///
		for(int t=0; t<org_lrs.size(); ++t)
		{
            LinearRing* plr = org_lrs[t];
			for(int j=1; j<plr->getSize(); ++j)
			{
				//point2d p1 = plr->getPoint(j-1);
				//point2d p2 = plr->getPoint(j);
				
				PointProxy temp_s;
				temp_s.plr = plr;
				temp_s.index = j-1;
				
				PointProxy temp_e;
				temp_e.plr = plr;
				temp_e.index = j;					
				
				
				EdgeProxy temp;
				temp.sp = temp_s;
				temp.ep = temp_e;
				org_edges.push_back(temp);
			}
		}

		//
		for(t=0; t<cross_points.size(); ++t)
		{
            for(i=0; i<org_edges.size(); ++i)
			{
               double z;
			   if(isPointOnEdge(cross_points[t],org_edges[i],&z))
			   {
				   cross_points[t].plr->points[cross_points[t].index].z = z;
				   break;
			   }
			}
		}
	}

	PolygonSet* PolygonBooleanHander::getResult()
	{
		if(pRes->getSize() == 0)
			return NULL;
		//
		int i=0, j=0;
		
		for(i=0; i<pRes->getSize(); ++i)
		{
			plybn::IPolygon2d* temp = pRes->getPolygon(i);
			shared_ptr<pbh::Polygon> pres = shared_ptr<pbh::Polygon>(new Polygon());	
			if(temp->getOuter() != NULL)
			{
				plybn::ILinearRing2d* temp_outer = temp->getOuter();
				shared_ptr<LinearRing> pouter = shared_ptr<LinearRing>(new LinearRing());
				
				for(j=0; j<temp_outer->getSize(); ++j)
				{
					plybn::point2d p = temp_outer->getPoint(j);
					pouter->addPoint(point2d(p.x,p.y));
				}
				//
                plybn::point2d p = temp_outer->getPoint(0);
				pouter->addPoint(point2d(p.x,p.y));
                //
				pres->setOuter(pouter);
			}
			//
			for(j=0; j<temp->getHoleCount(); ++j)
			{
				plybn::ILinearRing2d* temp_hole = temp->getHole(j);
				shared_ptr<LinearRing> phole = shared_ptr<LinearRing>(new LinearRing());
				
				for(int t=0; t<temp_hole->getSize(); ++t)
				{
					plybn::point2d p = temp_hole->getPoint(t);
					phole->addPoint(point2d(p.x,p.y));
				}
				//
				plybn::point2d p = temp_hole->getPoint(0);
				phole->addPoint(point2d(p.x,p.y));
				//		
				pres->addHole(phole);
			}
			//
			
			res.addPolygon(pres);
		}
		//
		recoverZ();
		//
		return &res;        
	}

}