#ifndef POLYGON_BOOLEAN_H_
#define POLYGON_BOOLEAN_H_

#include "IPolygonBoolean.h"
#include <vector>
#include <list>
#include <algorithm>
#include <math.h>
#include <float.h>
#include "GeoSurface.h"
#include "SmartViewFunctions.h"

#pragma  comment(lib, "polygonboolean.lib")

namespace pbh
{
    extern double TOLERANCE;
	//
	template<class T>
	class shared_ptr
	{
       public:
		   ~shared_ptr()
		   {
			   --(*ref_count);
			   if(*ref_count == 0)
			   {
				  if(pdata != NULL)
                     delete pdata;
				  //
				  delete ref_count;
			   }
		   }

		   shared_ptr()
		   {
			   pdata = NULL;
			   ref_count = new int(1);
		   }

		   shared_ptr(const shared_ptr& p)
		   {
			   pdata = p.pdata;
			   ref_count = p.ref_count;
			   ++(*ref_count);
		   }

		   shared_ptr(T* p)
		   {
			   pdata = p;
			   ref_count = new int(1);
		   }

           shared_ptr& operator=(T* p)
		   {
			   --(*ref_count);
			   if(*ref_count == 0)
			   {
				   if(pdata != NULL)
					   delete pdata;
				   //
				   delete ref_count;
			   }
			   //
			   pdata = p;
			   ref_count = new int(1);

			   return *this;
		   }

		   shared_ptr& operator=(const shared_ptr& sp)
		   {
			   --(*ref_count);
			   if(*ref_count == 0)
			   {
				   if(pdata != NULL)
				     delete pdata;
				   //
				   delete ref_count;
			   }
			   //
			   pdata = sp.pdata;
			   ref_count = sp.ref_count;
			   ++(*ref_count);
			   //
			   return *this;
		   }

		   bool operator==(T* p)
		   {
			   if(pdata == p)
				   return true;
			   else
				   return false;
		   }

		   bool operator!=(T* p)
		   {
			   if(pdata == p)
				   return false;
			   else
				   return true;
		   }

		   T* operator->()
		   {
			   return pdata;
		   }

		   T& operator*()
		   {
			   return *pdata;
		   }

		   T* get()
		   {
			   return pdata;
		   }
	   private:
		   int *ref_count;
		   T* pdata;
	};
	//
	struct point2d
	{
		static double invalid_z;
		//
		double x;
		double y;
		double z;
		point2d()
		{
			x = 0; y = 0; z = invalid_z;
		}

		point2d(double x_, double y_, double z_ = invalid_z)
		{
            x = x_; y = y_; z = z_;
		}
		
		bool operator==(const point2d& rv)
		{
			if(fabs(x-rv.x)<=TOLERANCE && fabs(y-rv.y)<=TOLERANCE)
				return true;
			else
				return false;
		}

		bool operator!=(const point2d& rv)
		{
			if(fabs(x-rv.x)<=TOLERANCE && fabs(y-rv.y)<=TOLERANCE)
				return false;
			else
				return true;
		}
		
		point2d& operator=(const point2d& rv)
		{
			this->x = rv.x;
			this->y = rv.y;
			this->z = rv.z;
			return *this;
		}
		
		point2d& operator-(const point2d& rv)
		{
			this->x-=rv.x;
			this->y-=rv.y;
			return *this;
		}
		
		point2d& operator+(const point2d& rv)
		{
			this->x+=rv.x;
			this->y+=rv.y;
			return *this;
		}
	};


	struct box
	{
		double minx,maxx,miny,maxy;
		box()
		{
			minx = DBL_MAX;
			maxx = -DBL_MAX;
			miny = DBL_MAX;
			maxy = -DBL_MAX;
		}
	};

	class PolygonBooleanHander;
	class PolygonSet;
    class LinearRing
	{
		friend class PolygonBooleanHander;
		friend struct PointProxy;
		friend int check_ring_relationship(LinearRing* la, LinearRing* lb);
		friend int check_point_location(point2d& p, LinearRing* ring);
	public:
		void addPoint(point2d& p)
		{
			points.push_back(p);
			if(p.x<aabb.minx)  aabb.minx = p.x;
			if(p.x>aabb.maxx)  aabb.maxx = p.x;
			if(p.y<aabb.miny)  aabb.miny = p.y;
			if(p.y>aabb.maxy)  aabb.maxy = p.y;				
		}

		void insertPoint(int position, point2d& p)
		{
            if(p==*(points.begin()+position-1) || p==*(points.begin()+position))
				return;
			//
			points.insert(points.begin()+position, p);
			if(p.x<aabb.minx)  aabb.minx = p.x;
			if(p.x>aabb.maxx)  aabb.maxx = p.x;
			if(p.y<aabb.miny)  aabb.miny = p.y;
			if(p.y>aabb.maxy)  aabb.maxy = p.y;
		}

		void setPoint(int index, point2d& p)
		{
			points[index].x = p.x;
			points[index].y = p.y;
			points[index].z = p.z;
			if(p.x<aabb.minx)  aabb.minx = p.x;
			if(p.x>aabb.maxx)  aabb.maxx = p.x;
			if(p.y<aabb.miny)  aabb.miny = p.y;
			if(p.y>aabb.maxy)  aabb.maxy = p.y;
		}

		int getSize()
		{
			return points.size();
		}

		point2d getPoint(int index)
		{
			return points[index];
		}

		point2d& getPointRef(int index)
		{
            return points[index];
		}

		box getBox()
		{
			return aabb;
		}
	private:
		std::vector<point2d> points;
		box aabb;
	};


	enum RINGRELATIONTYPE
	{
		DISJOINT = -2, //相离，不邻接；
		TOUCH = -1,// 相离， 邻接；
		OVERLAP = 0,// 重叠；
		CROSSING = 1,// 相交；
		A_CONTAIN_B_WITHOUT_TOUCH = 2,// la 包含 lb，不邻接;
		A_CONTAIN_B_AND_TOUCH = 3,// la 包含 lb，邻接；
		B_CONTAIN_A_WITHOUT_TOUCH = 4,// lb 包含 la，不邻接;
		B_CONTAIN_A_AND_TOUCH = 5// lb 包含 la，邻接
	};
    extern int check_ring_relationship(LinearRing* la, LinearRing* lb);
	extern int check_point_location(point2d& p, LinearRing* ring);


	class Polygon
	{
	public:
	    Polygon()
		{
           flag = -1;
		}

		void setOuter(shared_ptr<LinearRing>& pouter)
		{
			outer = pouter;
			box temp = pouter->getBox();
			if(temp.minx<aabb.minx)  aabb.minx = temp.minx;
			if(temp.maxx>aabb.maxx)  aabb.maxx = temp.maxx;
			if(temp.miny<aabb.miny)  aabb.miny = temp.miny;
			if(temp.maxy>aabb.maxy)  aabb.maxy = temp.maxy;
		}
		
		void addHole(shared_ptr<LinearRing>& phole)
		{
			holes.push_back(phole);
			box temp = phole->getBox();
			if(temp.minx<aabb.minx)  aabb.minx = temp.minx;
			if(temp.maxx>aabb.maxx)  aabb.maxx = temp.maxx;
			if(temp.miny<aabb.miny)  aabb.miny = temp.miny;
			if(temp.maxy>aabb.maxy)  aabb.maxy = temp.maxy;
		}

		void setHole(int index, shared_ptr<LinearRing>& phole)
		{
			holes[index] = phole;
			box temp = phole->getBox();
			if(temp.minx<aabb.minx)  aabb.minx = temp.minx;
			if(temp.maxx>aabb.maxx)  aabb.maxx = temp.maxx;
			if(temp.miny<aabb.miny)  aabb.miny = temp.miny;
			if(temp.maxy>aabb.maxy)  aabb.maxy = temp.maxy;
		}
		
		shared_ptr<LinearRing> getOuter()
		{
			return outer;
		}
		
		int getHoleCount()
		{
			return holes.size();
		}
		
		shared_ptr<LinearRing> getHole(int index)
		{
			return holes[index];
		}

		box getBox()
		{
			return aabb;
		}

		int flag;
	private:
		shared_ptr<LinearRing> outer;
		std::vector<shared_ptr<LinearRing> > holes;
		box aabb;
	};


    class PolygonSet
	{
	public:
		void addPolygon(shared_ptr<Polygon>& p)
		{
			polygons.push_back(p);
			box temp = p->getBox();
			if(temp.minx<aabb.minx)  aabb.minx = temp.minx;
			if(temp.maxx>aabb.maxx)  aabb.maxx = temp.maxx;
			if(temp.miny<aabb.miny)  aabb.miny = temp.miny;
			if(temp.maxy>aabb.maxy)  aabb.maxy = temp.maxy;
		}

		void deletePolygon(int index)
		{
			if(index>=0 && index < polygons.size())
			  polygons.erase(polygons.begin()+index);
		}

		shared_ptr<Polygon> pop()
		{
           shared_ptr<Polygon> temp = polygons[0];
		   polygons.erase(polygons.begin());
		   return temp;
		}

		int getSize()
		{
			return polygons.size();
		}

		shared_ptr<Polygon> getPolygon(int index)
		{
			return polygons[index];
		}

		box getBox()
		{
			return aabb;
		}

		void clear()
		{
			polygons.clear();
			aabb = box();
		}
	private:
		std::vector<shared_ptr<Polygon> > polygons;
		box aabb;
	};


	struct PointProxy
	{
		LinearRing* plr;
		int index;  
		
		bool operator==(const PointProxy& rv)
		{
			if(fabs(plr->points[index].x-rv.plr->points[rv.index].x)<=TOLERANCE && 
				fabs(plr->points[index].y-rv.plr->points[rv.index].y)<=TOLERANCE)
				return true;
			else
				return false;
		}
	};

	struct EdgeProxy
	{
		PointProxy sp;
		PointProxy ep;
	};


	class PolygonBooleanHander
	{
	private:		
		static bool compare_point(const PointProxy& pf,const PointProxy& ps)
		{
			if(pf.plr->points[pf.index].x - ps.plr->points[ps.index].x < -TOLERANCE)
				return true;
			else if(pf.plr->points[pf.index].x - ps.plr->points[ps.index].x > TOLERANCE)
				return false;
			else
			{
				if(pf.plr->points[pf.index].y - ps.plr->points[ps.index].y < -TOLERANCE)
					return true;
				else if(pf.plr->points[pf.index].y - ps.plr->points[ps.index].y > TOLERANCE)
					return false;
				else 
					return pf.plr->points[pf.index].z > ps.plr->points[ps.index].z;
			}
		}
	public:
        PolygonBooleanHander()
		{
			  pA = NULL;
			  pB = NULL;
			  pRes = plybn::IPolygon2dSet::Create();
		}

		~PolygonBooleanHander()
		{
			if(pA != NULL)
			{
				plybn::IPolygon2d::Release(pA);
				pA = NULL;
			}
			if(pB != NULL)
			{
				plybn::IPolygon2d::Release(pB);
				pB = NULL;
			}
			if(pRes != NULL)
			{
				plybn::IPolygon2dSet::Release(pRes);
				pRes = NULL;
			}
		}

		void setPolygon(shared_ptr<Polygon>& pa_, shared_ptr<Polygon>& pb_)
		{
			pa = pa_;
			pb = pb_;
			//
			compare_vertex();
			
			check_vertex_on_edge(pa.get(),pb.get());
		    check_vertex_on_edge(pb.get(),pa.get());
		    //
			outer_relationship = check_ring_relationship(pa->getOuter().get(),pb->getOuter().get());
			if(outer_relationship==2)
			{
				int hole_relationship;
				for(int i=0; i<pa->getHoleCount(); ++i)
				{
					hole_relationship = check_ring_relationship(pa->getHole(i).get(),pb->getOuter().get());
					if(hole_relationship==2)
					{
						outer_relationship=-2;
						break;
					}
				}
			}
			else if(outer_relationship==4)
			{
				int hole_relationship;
                for(int i=0; i<pb->getHoleCount(); ++i)
				{
					hole_relationship = check_ring_relationship(pb->getHole(i).get(),pa->getOuter().get());
					if(hole_relationship==2)
					{
						outer_relationship=-2;
						break;
					}				
				}
			}

			if(pA != NULL)
			{
				plybn::IPolygon2d::Release(pA);
				pA = NULL;
			}
			if(pB != NULL)
			{
				plybn::IPolygon2d::Release(pB);
				pB = NULL;
			}
		}
		
		RINGRELATIONTYPE getRelationshipType()
		{
			return RINGRELATIONTYPE(outer_relationship);
		}

        bool init(bool& is_pa_valid, bool& is_pb_valid);
		bool init();

		void clear_result()
		{
			pRes->clear();
			res.clear();
		}

        void intersection()
		{
			clear_result();
			if(outer_relationship==-1 || outer_relationship==-2)
			{
               return ;
			}
            if(outer_relationship == A_CONTAIN_B_AND_TOUCH || outer_relationship == A_CONTAIN_B_WITHOUT_TOUCH)
			{
				if(plybn::PolygonBoolean::is_within(pA,pB))
				{
					pRes->addPolygon(pB->clone());
				    return ;
				}
				
			}
			else if(outer_relationship == B_CONTAIN_A_AND_TOUCH || outer_relationship == B_CONTAIN_A_WITHOUT_TOUCH)
			{
                if(plybn::PolygonBoolean::is_within(pB,pA))
				{
					pRes->addPolygon(pA->clone());
			   	    return ;
				}	
			}
			//
			plybn::PolygonBoolean::intersection(pA,pB,pRes);
		}

		void join()
		{
			clear_result();
			if(outer_relationship==-2)
			{
				return;
			}
            //
			if(outer_relationship == A_CONTAIN_B_AND_TOUCH || outer_relationship == A_CONTAIN_B_WITHOUT_TOUCH)
			{
				if(plybn::PolygonBoolean::is_within(pA,pB))
				{
					pRes->addPolygon(pA->clone());
					return ;
				}	
			}
			else if(outer_relationship == B_CONTAIN_A_AND_TOUCH || outer_relationship == B_CONTAIN_A_WITHOUT_TOUCH)
			{
                if(plybn::PolygonBoolean::is_within(pB,pA))
				{
					pRes->addPolygon(pB->clone());
					return ;
				}	
			}
			//
			plybn::IPolygon2d* temp = plybn::IPolygon2d::Create();
			plybn::PolygonBoolean::join(pA,pB,temp);
			if(temp->getOuter() == NULL)
				return;
			pRes->addPolygon(temp);	
		}

		void difference()
		{
			clear_result();
			if (outer_relationship == -2)
			{
				bool is_valid;
				pRes->addPolygon(pA == NULL ? plybn_from_pbh_polygon(pa.get(), is_valid) : pA->clone());
				return;
			}
			//
			if (outer_relationship == B_CONTAIN_A_AND_TOUCH || outer_relationship == B_CONTAIN_A_WITHOUT_TOUCH)
			{
				if (plybn::PolygonBoolean::is_within(pB, pA))
				{
					return;
				}
			}
			//
			plybn::PolygonBoolean::difference(pA, pB, pRes);
		}

		void symmetric_difference()
		{
			clear_result();
			if(outer_relationship == -2)
			{
				return ;
			}	
			plybn::PolygonBoolean::symmetric_difference(pA,pB,pRes);
		}

		bool is_intersected()
		{
			if(outer_relationship == -2)
			{
				return false;
			}	
            return plybn::PolygonBoolean::is_intersected(pA,pB);
		}

		PolygonSet* getResult();
	private:
		bool isPointOnEdge(PointProxy& p_, EdgeProxy& e_, double* z)
		{
			point2d p = p_.plr->points[p_.index];
			point2d es = e_.sp.plr->points[e_.sp.index];
			point2d ee = e_.ep.plr->points[e_.ep.index];

			if(p.x<min(es.x,ee.x) || p.x>max(es.x,ee.x) ||
			   p.y<min(es.y,ee.y) || p.y>max(es.y,ee.y) )
			   return false;

			if(GraphAPI::GIsPtInLine(es,ee,p))
			{
				if(z!=NULL)
				{
					point2d va(p.x - es.x, p.y - es.y);
					point2d vb(p.x - ee.x, p.y - ee.y);
			        point2d ve(ee.x - es.x, ee.y - es.y);
					//
					double rate = sqrtf((va.x*va.x + va.y*va.y)/(ve.x*ve.x + ve.y*ve.y));
					*z = es.z*rate+(1-rate)*ee.z;
				}	
				return true;
			}
			//
			return false;
		}

		bool isPointOnEdge(point2d& p, point2d& es, point2d& ee, double* z)
		{
			if(p.x<min(es.x,ee.x)-TOLERANCE || p.x>max(es.x,ee.x)+TOLERANCE ||
				p.y<min(es.y,ee.y)-TOLERANCE || p.y>max(es.y,ee.y)+TOLERANCE )
				return false;
			
			if(GraphAPI::GIsPtInLine(es,ee,p))
			{
				if(z!=NULL)
				{
					point2d va(p.x - es.x, p.y - es.y);
					point2d vb(p.x - ee.x, p.y - ee.y);
			        point2d ve(ee.x - es.x, ee.y - es.y);
					//
					double rate = sqrtf((va.x*va.x + va.y*va.y)/(ve.x*ve.x + ve.y*ve.y));
					*z = es.z*rate+(1-rate)*ee.z;
				}	
				return true;
			}
			//
			return false;		
		}

		void compare_vertex();

		void check_vertex_on_edge(Polygon* pv, Polygon* pe);

		void recoverZ();
	private:
		plybn::IPolygon2d* plybn_from_pbh_polygon(Polygon* ppg, bool& is_valid);
	private:
		shared_ptr<Polygon> pa;
		shared_ptr<Polygon> pb;
		PolygonSet res;

		plybn::IPolygon2d* pA;
		plybn::IPolygon2d* pB;
		plybn::IPolygon2dSet* pRes;	
		int outer_relationship;
	};



}


#endif