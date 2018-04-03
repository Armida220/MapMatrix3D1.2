#ifndef GEOSURFACE_BOOLEAN_HANDLE_H_
#define GEOSURFACE_BOOLEAN_HANDLE_H_

#include "PolygonBooleanHander.h"
#include "GeoSurface.h"
#include "Feature.h"
#include <map>

namespace gsbh
{
   class GeoSurfaceBooleanHandle
   {
	   friend class GeoSurfaceSetBooleanHandle;
   public:
	   static bool GeoSurface2PolygonSet(CGeoSurface* psurface, pbh::PolygonSet* res);
	   static CGeoSurface*  PolygonSet2GeoSurface(pbh::PolygonSet* pps);
	   static void PolygonSet2GeoSurfaces(pbh::PolygonSet* pps, std::vector<CGeoSurface*>& res);
   public:
       bool setSurface(CGeoSurface* sfa, CGeoSurface* sfb);
	   bool init();
       void intersection();
	   void join();
	   void difference();
	   void symmetric_difference();
	   CGeoSurface* getResult();
	   void getResult(std::vector<CGeoSurface*>& res);
       //此函数在具体的布尔操作成功完成后有效；
	   bool isSurfaceCrossed() { return is_crossed; }
   private:
	   pbh::PolygonSet gsa;
	   pbh::PolygonSet gsb;
	   pbh::PolygonSet res;
	   bool is_crossed;
	   //
   };


   class GeoSurfaceSetBooleanHandle
   {
   public:
	   struct ResultSurface
	   {
          CGeoSurface* pgs;
		  CString ftr_id;
	   };
   public:
       bool addSurface(CGeoSurface* pgs, CString ftr_id);
	   bool init();
	   bool join();
	   void getResult(std::vector<ResultSurface>& res);
   private:
	   std::map<int,CString>flag_id;
	   pbh::PolygonSet org_polygons;
	   pbh::PolygonSet res_polygons;
   };

}

#endif