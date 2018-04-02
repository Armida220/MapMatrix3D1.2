#include "StdAfx.h"
#include "GeoSurfaceBooleanHandle.h"

using namespace pbh;

namespace gsbh
{
	//结果的高程以第一个面为准
   bool GeoSurfaceBooleanHandle::setSurface(CGeoSurface* sfa, CGeoSurface* sfb)
   {
	  if(!GeoSurface2PolygonSet(sfa,&gsa))
          return false;
	  if(!GeoSurface2PolygonSet(sfb,&gsb))
		  return false;
	  //
	  return true;
   }

   bool GeoSurfaceBooleanHandle::init()
   {
	  is_crossed = false;
      return true;
   }

   void GeoSurfaceBooleanHandle::intersection()
   {
	   std::vector<pbh::shared_ptr<pbh::Polygon> > pas;
	   for(int i=0; i<gsa.getSize(); ++i)
	   {
		   pas.push_back(gsa.getPolygon(i));
	   }
	   //
	   std::vector<pbh::shared_ptr<pbh::Polygon> > temp_res;
	   //
	   for(i=0; i<gsb.getSize(); ++i)
	   {
		   shared_ptr<pbh::Polygon> pb = gsb.getPolygon(i);
		   for(int j=0; j<pas.size(); ++j)
		   {
			   shared_ptr<pbh::Polygon> pa = pas[j];
			   //
			   pbh::PolygonBooleanHander pbhandle;
			   pbhandle.setPolygon(pa,pb);
			   if(pbhandle.getRelationshipType() == pbh::CROSSING)
				  is_crossed = true;
			   if(!pbhandle.init())
				   continue;
			   pbhandle.intersection();
			   pbh::PolygonSet* pres = pbhandle.getResult();
			   if(pres == NULL)
			   {
				   continue;
			   }
			   //
			   for(int k=0; k<pres->getSize(); ++k)
			   {
				   res.addPolygon(pres->getPolygon(k));
			   }
			   //pa = pres->getPolygon(0);
		   }
		   //
// 		   pas.clear();
// 		   for(int k=0; k<temp_res.size(); ++k)
// 		   {
// 			   pas.push_back(temp_res[k]);
// 		   }
// 		   //
// 		   temp_res.clear();
	   }
	   //
// 	   for(int k=0; k<pas.size(); ++k)
// 	   {
// 		   res.addPolygon(pas[k]);
// 	   }
	   //
	   pas.clear();
   }

   void GeoSurfaceBooleanHandle::join()
   {
	   std::vector<pbh::shared_ptr<pbh::Polygon> > pas;
	   for(int i=0; i<gsa.getSize(); ++i)
	   {
		   pas.push_back(gsa.getPolygon(i));
	   }
	   //
	   std::vector<pbh::shared_ptr<pbh::Polygon> > temp_res;
	   //
	   for(i=0; i<gsb.getSize(); ++i)
	   {
		   shared_ptr<pbh::Polygon> pb = gsb.getPolygon(i);
		   bool is_b_joined = false;
		   for(int j=0; j<pas.size(); ++j)
		   {
			   shared_ptr<pbh::Polygon> pa = pas[j];
			   //
			   pbh::PolygonBooleanHander pbhandle;
			   pbhandle.setPolygon(pa,pb);
			   if(pbhandle.getRelationshipType() == pbh::CROSSING)
				  is_crossed = true;
			   if(!pbhandle.init())
			   {
				   temp_res.push_back(pa);
				   continue;
			   }
			   pbhandle.join();
			   pbh::PolygonSet* pres = pbhandle.getResult();
			   if(pres == NULL)
			   {
				   temp_res.push_back(pa);
				   continue;
			   }
			   //
			   is_b_joined = true;
			   //
			   //temp_res.push_back(pres->getPolygon(0));
			   pb = pres->getPolygon(0);
		   }
		   //
		   if(!is_b_joined)
		     res.addPolygon(pb);
		   else
             temp_res.push_back(pb);
		   //
		   pas.clear();
		   for(int k=0; k<temp_res.size(); ++k)
		   {
			   pas.push_back(temp_res[k]);
		   }
		   //
		   temp_res.clear();
	   }
	   //
	   for(int k=0; k<pas.size(); ++k)
	   {
		   res.addPolygon(pas[k]);
	   }
	   //
	   pas.clear();
   }

   void GeoSurfaceBooleanHandle::difference()
   {
		std::vector<pbh::shared_ptr<pbh::Polygon> > pas;
		for(int i=0; i<gsa.getSize(); ++i)
		{
			pas.push_back(gsa.getPolygon(i));
		}
		//
		std::vector<pbh::shared_ptr<pbh::Polygon> > temp_res;
		//
		for(i=0; i<gsb.getSize(); ++i)
		{
			shared_ptr<pbh::Polygon> pb = gsb.getPolygon(i);
			for(int j=0; j<pas.size(); ++j)
			{
				shared_ptr<pbh::Polygon> pa = pas[j];
				//
				pbh::PolygonBooleanHander pbhandle;
				pbhandle.setPolygon(pa,pb);
				if(pbhandle.getRelationshipType() == pbh::CROSSING)
				is_crossed = true;
				if(!pbhandle.init())
				continue;
				pbhandle.difference();
				pbh::PolygonSet* pres = pbhandle.getResult();
				if(pres == NULL)
				{
					continue;
				}
				for(int k=0; k<pres->getSize(); ++k)
				{
					temp_res.push_back(pres->getPolygon(k));
				}
				pa = pres->getPolygon(0);
			}
			//
			pas.clear();
			for(int k=0; k<temp_res.size(); ++k)
			{
				pas.push_back(temp_res[k]);
			}
			//
			temp_res.clear();
		}
		//
		for(int k=0; k<pas.size(); ++k)
		{
			res.addPolygon(pas[k]);
		}
		//
		pas.clear();
   }

   void GeoSurfaceBooleanHandle::symmetric_difference()
   {
       std::vector<pbh::shared_ptr<pbh::Polygon> > pas;
	   for(int i=0; i<gsa.getSize(); ++i)
	   {
		   pas.push_back(gsa.getPolygon(i));
	   }
	   //
	   std::vector<pbh::shared_ptr<pbh::Polygon> > temp_res;
	   //
	   for(i=0; i<gsb.getSize(); ++i)
	   {
		   shared_ptr<pbh::Polygon> pb = gsb.getPolygon(i);
		   for(int j=0; j<pas.size(); ++j)
		   {
			   shared_ptr<pbh::Polygon> pa = pas[j];
			   //
			   pbh::PolygonBooleanHander pbhandle;
			   pbhandle.setPolygon(pa,pb);
			   if(pbhandle.getRelationshipType() == pbh::CROSSING)
				   is_crossed = true;
			   if(!pbhandle.init())
				   continue;
			   pbhandle.symmetric_difference();
			   pbh::PolygonSet* pres = pbhandle.getResult();
			   if(pres == NULL)
			   {
				   continue;
			   }
			   //
			   for(int k=0; k<pres->getSize(); ++k)
			   {
				   temp_res.push_back(pres->getPolygon(k));
			   }
			   pa = pres->getPolygon(0);
		   }
		   //
		   pas.clear();
		   for(int k=0; k<temp_res.size(); ++k)
		   {
			   pas.push_back(temp_res[k]);
		   }
		   //
		   temp_res.clear();
	   }
	   //
	   for(int k=0; k<pas.size(); ++k)
	   {
		   res.addPolygon(pas[k]);
	   }
	   //
	   pas.clear();
   }

   CGeoSurface* GeoSurfaceBooleanHandle::getResult()
   {
       return PolygonSet2GeoSurface(&this->res);
   }

   void GeoSurfaceBooleanHandle::getResult(std::vector<CGeoSurface*>& res)
   {
      PolygonSet2GeoSurfaces(&this->res, res);
   }

   bool compare_box(pbh::LinearRing* la,pbh::LinearRing* lb)
   {
	   pbh::box ba = la->getBox();
	   pbh::box bb = lb->getBox();
	   return (ba.maxx-ba.minx)*(ba.maxy-ba.miny) > (bb.maxx-bb.minx)*(bb.maxy-bb.miny);
   }
   bool GeoSurfaceBooleanHandle::GeoSurface2PolygonSet(CGeoSurface* psurface, pbh::PolygonSet* res)
   {
        CArray<PT_3DEX,PT_3DEX> pshape;
		psurface->GetShape(pshape);
		// 
		std::vector<pbh::LinearRing*> rings;
		pbh::LinearRing* plr = NULL;
		for(int i=0; i<pshape.GetSize(); ++i)
		{
			if(i==0 || (pshape[i].pencode == penMove && i<pshape.GetSize()-1))
			{
				if(plr != NULL)
				{
					if(plr->getSize()>=4)
					   rings.push_back(plr);	
					//
					plr = NULL;
				}
				//
				plr = new pbh::LinearRing();
				plr->addPoint(pbh::point2d (pshape[i].x, pshape[i].y, pshape[i].z));
			}
			else
			{
				pbh::point2d p(pshape[i].x, pshape[i].y, pshape[i].z);
				if(plr->getPointRef(plr->getSize()-1) == p)
					continue;
				else
				    plr->addPoint(p);
			}
		}
		if(plr->getSize()>=4)
		  rings.push_back(plr);
		//
		if(rings.size()==0)
			return false;
		//
		if(rings.size()>1)
		  std::stable_sort(rings.begin(), rings.end(), compare_box);
		//
        for(i=0; i<rings.size(); ++i)
		{
			if(i==0)
			{
               res->addPolygon(shared_ptr<pbh::Polygon>(new pbh::Polygon()));
			   res->getPolygon(0)->setOuter(shared_ptr<pbh::LinearRing>(rings[0]));
			   continue;
			}				
			//
			bool new_polygon = true;
			pbh::box ring_box = rings[i]->getBox();
			//
			for(int t=0; t<res->getSize(); ++t)
			{		
				pbh::Polygon* ppg = res->getPolygon(t).get();
				//
				pbh::LinearRing* pouter = ppg->getOuter().get();
				int ring_relationship = pbh::check_ring_relationship(pouter,rings[i]);
				if(ring_relationship == 1)
					return false;
				//
				if(ring_relationship == 0)
				{
					new_polygon = false;
					break;
				}
				else if(ring_relationship == -2 || ring_relationship == -1)
				{
					continue;
				}
				else 
				{
                    bool new_hole = true;
					for(int k=0; k<ppg->getHoleCount(); ++k)
					{
						pbh::LinearRing* phole = ppg->getHole(k).get();
						ring_relationship = pbh::check_ring_relationship(phole,rings[i]);
						if(ring_relationship == 1)
							return false;
						//
						if(ring_relationship == 0)
						{
							new_hole = false;
							new_polygon = false;
							break;
						}
						else if(ring_relationship == -2 || ring_relationship == -1)
						{
							continue;
						}
						else
						{
                           new_hole = false;
						}
					}			
					if(new_hole)
					{
						ppg->addHole(shared_ptr<pbh::LinearRing>(rings[i]));
						new_polygon = false;
						break;
					}
				}
			}
			//
			if(new_polygon)
			{
				res->addPolygon(shared_ptr<pbh::Polygon>(new pbh::Polygon()));
				res->getPolygon(res->getSize()-1)->setOuter(shared_ptr<pbh::LinearRing>(rings[i]));
			}
		} 
		return true;
   }


   CGeoSurface* GeoSurfaceBooleanHandle::PolygonSet2GeoSurface(pbh::PolygonSet* pps)
   {
	   if(pps->getSize() == 0)
		   return NULL;
	   //
	    CGeoSurface* pgeo_surface = NULL;
        CArray<PT_3DEX,PT_3DEX> arr3;
	    bool is_multi_surface = false;
		if(pps->getSize() > 1)
			is_multi_surface = true;

		for(int t=0; t<pps->getSize(); ++t)
		{
			shared_ptr<pbh::Polygon> pres = pps->getPolygon(t);
			
			shared_ptr<pbh::LinearRing> pouter = pres->getOuter();
			if(pouter != NULL)
			{
				for(int i=0; i<pouter->getSize(); ++i)
				{
					PT_3DEX p;
					pbh::point2d temp = pouter->getPoint(i);
					p.x = temp.x;
					p.y = temp.y;
					p.z = temp.z;
					if(t>0 && i==0)
						p.pencode = penMove;
					else
						p.pencode = penLine;
					
					arr3.Add(p);		
				}
			}
			
			for(int i=0; i<pres->getHoleCount(); ++i)
			{
				is_multi_surface = true;
				//
				shared_ptr<pbh::LinearRing> phole = pres->getHole(i);
				if(phole == NULL)
					continue;
				//
				for(int j=0; j<phole->getSize(); ++j)
				{
					PT_3DEX p;
					pbh::point2d temp = phole->getPoint(j);
					p.x = temp.x;
					p.y = temp.y;
					p.z = temp.z;
					if(j==0)
						p.pencode = penMove;
					else
						p.pencode = penLine;
					
					arr3.Add(p);
				}
			}
		}

		if(arr3.GetSize()<4)
			return NULL;
		//
		
		if(is_multi_surface)
			pgeo_surface = new CGeoMultiSurface();
		else
		{
			pgeo_surface = new CGeoSurface();
			int nPt = arr3.GetSize();
			if(!GraphAPI::GIsClockwise(arr3.GetData(), nPt))
			{
				for(int i=0; i<nPt/2; i++)
				{
					PT_3DEX temp = arr3[i];
					arr3[i] = arr3[nPt-1-i];
					arr3[nPt-1-i] = temp;
				}
			}
		}
		//
		pgeo_surface->CreateShape(arr3.GetData(),arr3.GetSize());

		if(is_multi_surface)
		{
			((CGeoMultiSurface*)pgeo_surface)->NormalizeDirection();
		}
		//
		return pgeo_surface;
   }


void GeoSurfaceBooleanHandle::PolygonSet2GeoSurfaces(pbh::PolygonSet* pps, std::vector<CGeoSurface*>& res)
{
    if(pps->getSize() == 0)
		return;
	//
	for(int t=0; t<pps->getSize(); ++t)
	{
		shared_ptr<pbh::Polygon> pres = pps->getPolygon(t);
		//
		shared_ptr<pbh::LinearRing> pouter = pres->getOuter();
		//
		CArray<PT_3DEX,PT_3DEX> arr3;
		bool is_multi_surface = false;
        //
		if(pouter != NULL)
		{
			for(int i=0; i<pouter->getSize(); ++i)
			{
				PT_3DEX p;
				pbh::point2d temp = pouter->getPoint(i);
				p.x = temp.x;
				p.y = temp.y;
				p.z = temp.z;
				p.pencode = penLine;
				
				arr3.Add(p);		
			}
		}
		
		for(int i=0; i<pres->getHoleCount(); ++i)
		{
			is_multi_surface = true;
			//
			shared_ptr<pbh::LinearRing> phole = pres->getHole(i);
			if(phole == NULL)
				continue;
			//
			for(int j=0; j<phole->getSize(); ++j)
			{
				PT_3DEX p;
				pbh::point2d temp = phole->getPoint(j);
				p.x = temp.x;
				p.y = temp.y;
				p.z = temp.z;
				if(j==0)
					p.pencode = penMove;
				else
					p.pencode = penLine;
				
				arr3.Add(p);
			}
		}
		//
        if(arr3.GetSize()<4)
			continue;
		//
		CGeoSurface* pgeo_surface = NULL;
		if(is_multi_surface)
			pgeo_surface = new CGeoMultiSurface();
		else
		{
			pgeo_surface = new CGeoSurface();
			int nPt = arr3.GetSize();
			if(!GraphAPI::GIsClockwise(arr3.GetData(), nPt))
			{
				for(int i=0; i<nPt/2; i++)
				{
					PT_3DEX temp = arr3[i];
					arr3[i] = arr3[nPt-1-i];
					arr3[nPt-1-i] = temp;
				}
			}
		}
		//
		pgeo_surface->CreateShape(arr3.GetData(),arr3.GetSize());
		
		if(is_multi_surface)
		{
			((CGeoMultiSurface*)pgeo_surface)->NormalizeDirection();
		}
		//
		res.push_back(pgeo_surface);
	}
}


bool GeoSurfaceSetBooleanHandle::addSurface(CGeoSurface* pgs, CString ftr_id)
{
   pbh::PolygonSet temp;
   if(GeoSurfaceBooleanHandle::GeoSurface2PolygonSet(pgs,&temp))
   {
	   int flag = flag_id.size();
       flag_id[flag] = ftr_id;
	   for(int i=0; i<temp.getSize(); ++i)
	   {
		   temp.getPolygon(i)->flag = flag;
		   org_polygons.addPolygon(temp.getPolygon(i));	   
	   }
	   return true;
   }
   return false;
}

bool GeoSurfaceSetBooleanHandle::init()
{
   return true;
}

bool GeoSurfaceSetBooleanHandle::join()
{
   if(org_polygons.getSize()==0)
	   return true;
   else if(org_polygons.getSize()==1)
   {
	   res_polygons.addPolygon(org_polygons.getPolygon(0));
	   return true;
   }
   //
   pbh::PolygonSet temp_polygons_a;
   pbh::PolygonSet temp_polygons_b;

   for(int i=0; i<org_polygons.getSize(); ++i)
   {
	   temp_polygons_a.addPolygon(org_polygons.getPolygon(i));
   }

   pbh::PolygonSet* temp_polygons = &temp_polygons_a;
   pbh::PolygonSet* buf_polygons = &temp_polygons_b;

L: bool is_first_joined = false;
   pbh::shared_ptr<pbh::Polygon> pa = temp_polygons->getPolygon(0);
   bool is_pa_valid = true;
   for(i=1; i<temp_polygons->getSize(); ++i)
   {
	   pbh::shared_ptr<pbh::Polygon> pb = temp_polygons->getPolygon(i);
	   //
	   if(!is_pa_valid)
	   {
           buf_polygons->addPolygon(pb);
		   continue;
	   }
	   pbh::PolygonBooleanHander pbhandle;
	   pbhandle.setPolygon(pa,pb);
	   bool is_pb_valid;
	   if(!pbhandle.init(is_pa_valid,is_pb_valid))
	   {
		   if(!is_pa_valid)
		   {
               res_polygons.addPolygon(pa);
			   buf_polygons->addPolygon(pb);
		   }
		   else if(!is_pb_valid)
		   {
			   res_polygons.addPolygon(pb);
		   }
		   continue;
	   }
	   pbhandle.join();
	   pbh::PolygonSet* pres = pbhandle.getResult();
	   if(pres == NULL)
	   {
          buf_polygons->addPolygon(pb);
		  continue;
	   }
	   else
	   {
		   is_first_joined = true;
           int flag = pa->flag;
           pa = pres->getPolygon(0);
		   pa->flag = flag;
	   }
   }
   
   if(is_pa_valid && is_first_joined)
   {
	   buf_polygons->addPolygon(pa);
   }
   else if(is_pa_valid)
   {
	   res_polygons.addPolygon(pa);
   }

   if(buf_polygons->getSize()>=2)
   {
	   PolygonSet* temp = temp_polygons;
	   temp_polygons = buf_polygons;
	   buf_polygons = temp;
	   buf_polygons->clear();
	   
	   goto L;
   }
   else if(buf_polygons->getSize()==1)
   {
	   res_polygons.addPolygon(buf_polygons->getPolygon(0));
	   return true;
   }
    
   return true;
}

void GeoSurfaceSetBooleanHandle::getResult(std::vector<ResultSurface>& res)
{
	for(int i=0; i<res_polygons.getSize(); ++i)
	{
        CGeoSurface* pgs = NULL;
		pbh::Polygon* temp = res_polygons.getPolygon(i).get();
		if(temp->getOuter()->getSize()==0)
			continue;
		//
		if(temp->getHoleCount()>0)
		{
			pgs = new CGeoMultiSurface();
		}
		else
		{
			pgs = new CGeoSurface();
		}
		//
		CArray<PT_3DEX,PT_3DEX> arr3;
        shared_ptr<pbh::LinearRing> pouter = temp->getOuter();
		if(pouter != NULL)
		{
			for(int i=0; i<pouter->getSize(); ++i)
			{
				PT_3DEX p;
				pbh::point2d temp = pouter->getPoint(i);
				p.x = temp.x;
				p.y = temp.y;
				p.z = temp.z;
				if(i==0)
					p.pencode = penMove;
				else
					p.pencode = penLine;
				
				arr3.Add(p);		
			}
		}
		
		for(int i=0; i<temp->getHoleCount(); ++i)
		{
			shared_ptr<pbh::LinearRing> phole = temp->getHole(i);
			if(phole == NULL)
				continue;
			//
			for(int j=0; j<phole->getSize(); ++j)
			{
				PT_3DEX p;
				pbh::point2d temp = phole->getPoint(j);
				p.x = temp.x;
				p.y = temp.y;
				p.z = temp.z;
				if(j==0)
					p.pencode = penMove;
				else
					p.pencode = penLine;
				
				arr3.Add(p);
			}
		}
		pgs->CreateShape(arr3.GetData(), arr3.GetSize());
		//
		ResultSurface rs;
		rs.ftr_id = flag_id[temp->flag];
		rs.pgs = pgs;
		//
		res.push_back(rs);
	}
}


}






















