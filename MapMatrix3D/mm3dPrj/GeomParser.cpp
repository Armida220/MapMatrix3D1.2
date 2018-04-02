#include "stdafx.h"
#include "GeomParser.h"
#include <vector>
#include "ICallBack.h"
#include "GeoParallel.h"
#include "GeoCurve.h"
#include "GeoDirPoint.h"
#include "GeoText.h"
#include "qcomm.h"
#include "GeoSurface.h"

using namespace std;
using namespace osgCall;

#define PI	3.1415926535

CGeomParser::CGeomParser()
{
}


CGeomParser::~CGeomParser()
{
}

vector<osgFtr> CGeomParser::parseGeoPt(CGeometry* geom)
{
	vector<osgFtr> vecFtr;
	//获取点
	int ptNum = geom->GetDataPointSum();
	vector<Coord> pts;

	for (int j = 0; j < ptNum; j++)
	{
		PT_3DEX pt = geom->GetDataPoint(j);
		Coord xyz(pt.x, pt.y, pt.z);
		pts.push_back(xyz);
	}

	osgFtr ftr;
	ftr.vecCoord = pts;
	ftr.type = CLS_GEOPOINT;
	long clr = geom->GetColor();
	PT_3DEX rgb = convertLongtoRGB(clr);
	ftr.rgb.x = rgb.x;
	ftr.rgb.y = rgb.y;
	ftr.rgb.z = rgb.z;
	vecFtr.push_back(ftr);

	return vecFtr;
}


vector<osgFtr> CGeomParser::parseGeoDirPt(CGeometry* geom,
	double fDrawScale)
{
	vector<osgFtr> vecFtr;
	//获取线
	CGeoDirPoint* dpGeom = dynamic_cast<CGeoDirPoint*> (geom);

	if (!dpGeom)
	{
		return vecFtr;
	}

	//获取点
	int ptNum = geom->GetDataPointSum();
	vector<Coord> pts;

	for (int j = 0; j < ptNum; j++)
	{
		PT_3DEX pt = geom->GetDataPoint(j);
		double direction = dpGeom->GetDirection();
		createArrow(direction, pt, vecFtr, fDrawScale);
		long rgb = geom->GetColor();

		for (auto ftr : vecFtr)
		{
			ftr.rgb.x = GetRValue(rgb) / 255.0;
			ftr.rgb.y = GetGValue(rgb) / 255.0;
			ftr.rgb.z = GetBValue(rgb) / 255.0;
		}

	}

	return vecFtr;
}

vector<osgFtr> CGeomParser::parseGeoCurve(CGeometry* geom)
{
	vector<osgFtr> vecFtr;
	//获取线
	vector<Coord> line;
	CGeometry *linearGeom = geom->Linearize();

	int ptNum = linearGeom->GetDataPointSum();

	for (int j = 0; j < ptNum; j++)
	{
		PT_3DEX pt = linearGeom->GetDataPoint(j);
		Coord xyz(pt.x, pt.y, pt.z);
		line.push_back(xyz);
	}

	osgFtr ftr;
	ftr.vecCoord = line;
	ftr.type = CLS_GEOCURVE;

	long clr = geom->GetColor();
	PT_3DEX rgb = convertLongtoRGB(clr);
	ftr.rgb.x = rgb.x;
	ftr.rgb.y = rgb.y;
	ftr.rgb.z = rgb.z;

	CGeoCurve* curve = dynamic_cast<CGeoCurve*> (geom);

	if (ptNum >= 2)
	{
		PT_3DEX pt0 = linearGeom->GetDataPoint(0);
		PT_3DEX pt1 = linearGeom->GetDataPoint(1);
		ftr.tangent = calDirection(pt0, pt1);
	}

	vecFtr.push_back(ftr);
	return vecFtr;
}

vector<osgFtr> CGeomParser::parseGeoDCurve(CGeometry* geom)
{
	vector<osgFtr> vecFtr;
	//获取线
	CGeoDCurve* dCurve = dynamic_cast<CGeoDCurve*> (geom);

	if (!dCurve)
	{
		return vecFtr;
	}

	CGeometry* geom1, *geom2;
	bool bIsSuccesss = dCurve->Separate(geom1, geom2);

	if (bIsSuccesss && geom1 && geom2)
	{
		vector<osgFtr> vecFtr1 = parseGeoCurve(geom1);
		vector<osgFtr> vecFtr2 = parseGeoCurve(geom2);
		vecFtr.insert(vecFtr.end(), vecFtr1.begin(), vecFtr1.end());
		vecFtr.insert(vecFtr.end(), vecFtr2.begin(), vecFtr2.end());
	}

	return vecFtr;
}

vector<osgFtr> CGeomParser::parseGeoParallel(CGeometry* geom)
{
	vector<osgFtr> vecFtr;
	//获取线
	CGeoParallel* parallel = dynamic_cast<CGeoParallel*> (geom);

	if (!parallel)
	{
		return vecFtr;
	}

	//原来的线
	{
		vector<Coord> coord;
		int ptNum = parallel->GetDataPointSum();

		for (int j = 0; j < ptNum; j++)
		{
			PT_3DEX pt = parallel->GetDataPoint(j);
			Coord xyz(pt.x, pt.y, pt.z);
			coord.push_back(xyz);
		}

		osgFtr ftr1;
		ftr1.vecCoord = coord;
		ftr1.type = CLS_GEOCURVE;

		long clr = geom->GetColor();
		PT_3DEX rgb = convertLongtoRGB(clr);
		ftr1.rgb.x = rgb.x;
		ftr1.rgb.y = rgb.y;
		ftr1.rgb.z = rgb.z;

		if (ptNum >= 2)
		{
			PT_3DEX pt0 = parallel->GetDataPoint(0);
			PT_3DEX pt1 = parallel->GetDataPoint(1);
			double dir = calDirection(pt0, pt1);
			ftr1.tangent = dir;
		}

		vecFtr.push_back(ftr1);
	}

	{
		//另外一条线
		vector<Coord> coord;
		double width = parallel->GetWidth();
		CArray<PT_3DEX, PT_3DEX> pts;
		parallel->GetParallelShape(pts, width);

		for (int j = 0; j < pts.GetSize(); j++)
		{
			Coord xyz(pts[j].x, pts[j].y, pts[j].z);
			coord.push_back(xyz);
		}

		osgFtr ftr2;
		ftr2.vecCoord = coord;
		ftr2.type = CLS_GEOCURVE;

		long clr = geom->GetColor();
		PT_3DEX rgb = convertLongtoRGB(clr);
		ftr2.rgb.x = rgb.x;
		ftr2.rgb.y = rgb.y;
		ftr2.rgb.z = rgb.z;

		if (pts.GetSize() >= 2)
		{
			PT_3DEX pt0 = parallel->GetDataPoint(0);
			PT_3DEX pt1 = parallel->GetDataPoint(1);
			double dir = calDirection(pt0, pt1);
			ftr2.tangent = dir;
		}

		vecFtr.push_back(ftr2);
	}

	return vecFtr;
}

vector<osgFtr> CGeomParser::parseGeoSurface(CGeometry* geom)
{
	vector<osgFtr> vecFtr;
	//获取面
	CGeometry *linearGeom = geom->Linearize();
	vector<Coord> poly;
	int ptNum = linearGeom->GetDataPointSum();

	for (int j = 0; j < ptNum; j++)
	{
		PT_3DEX pt = linearGeom->GetDataPoint(j);
		Coord xyz(pt.x, pt.y, pt.z);
		poly.push_back(xyz);
	}

	osgFtr ftr;
	ftr.vecCoord = poly;
	ftr.type = CLS_GEOSURFACE;

	long clr = geom->GetColor();
	PT_3DEX rgb = convertLongtoRGB(clr);
	ftr.rgb.x = rgb.x;
	ftr.rgb.y = rgb.y;
	ftr.rgb.z = rgb.z;

	//获取面
	CGeoSurface* surface = dynamic_cast<CGeoSurface*> (geom);

	if (!surface)
	{
		return vecFtr;
	}

	if (ptNum >= 2)
	{
		PT_3DEX pt0 = surface->GetDataPoint(0);
		PT_3DEX pt1 = surface->GetDataPoint(1);
		double dir = calDirection(pt0, pt1);
		ftr.tangent = dir;
	}

	vecFtr.push_back(ftr);

	return vecFtr;
}


vector<osgFtr> CGeomParser::parseMultiGeoSurface(CGeometry* geom)
{
	vector<osgFtr> vecFtr;
	
	//获取面
	//获取面
	CGeoMultiSurface* surface = dynamic_cast<CGeoMultiSurface*> (geom);

	if (!surface)
	{
		return vecFtr;
	}

	for (int i = 0; i < surface->GetSurfaceNum(); i++)
	{
		CArray<PT_3DEX, PT_3DEX> arr;
		surface->GetSurface(i, arr);
		vector<Coord> poly;

		for (int j = 0; j < arr.GetSize(); j++)
		{
			PT_3DEX pt = arr.GetAt(j);
			Coord xyz(pt.x, pt.y, pt.z);
			poly.push_back(xyz);
		}

		osgFtr ftr;
		ftr.vecCoord = poly;
		ftr.type = CLS_GEOSURFACE;

		long clr = surface->GetColor();
		PT_3DEX rgb = convertLongtoRGB(clr);
		ftr.rgb.x = rgb.x;
		ftr.rgb.y = rgb.y;
		ftr.rgb.z = rgb.z;

		vecFtr.push_back(ftr);
		
	}

	return vecFtr;
}

vector<osgFtr> CGeomParser::parseGeoText(CGeometry* geom)
{
	vector<osgFtr> vecFtr;
	//获取线
	CGeoText* text = dynamic_cast<CGeoText*> (geom);

	if (!text)
	{
		return vecFtr;
	}

	{
		//另外一条线
		vector<Coord> coord;
		osgFtr ftr;
		ftr.type = CLS_GEOTEXT;
		ftr.str = text->GetText();

		long clr = geom->GetColor();
		PT_3DEX rgb = convertLongtoRGB(clr);
		ftr.rgb.x = rgb.x;
		ftr.rgb.y = rgb.y;
		ftr.rgb.z = rgb.z;
	
		PT_3DEX pt = text->GetDataPoint(0);
		Coord xyz(pt.x, pt.y, pt.z);
		coord.push_back(xyz);
		ftr.vecCoord = coord;

		//设置字体大小
		TEXT_SETTINGS0 setting;
		text->GetSettings(&setting);
		double height = setting.fHeight + 1;
		ftr.textSize = height;
		vecFtr.push_back(ftr);
	}

	return vecFtr;
}


void CGeomParser::createArrow(double direction, PT_3DEX pt, vector<osgFtr> &vecFtr, float fDrawScale)
{
	PT_3DEX pt0;
	double ang = direction * PI / 180;

	double offlen = 1 * fDrawScale;

	{
		//第一个点
		vector<Coord> coord;
		Coord xyz(pt.x, pt.y, pt.z);
		coord.push_back(xyz);

		double cosa = cos(ang), sina = sin(ang);
		//第二个点
		PT_3DEX pt1;
		pt1.x = pt.x + cosa * offlen;
		pt1.y = pt.y + sina * offlen;
		pt1.z = pt.z;

		Coord xyz1(pt1.x, pt1.y, pt1.z);
		coord.push_back(xyz1);

		osgFtr ftr;
		ftr.vecCoord = coord;
		ftr.type = CLS_GEOCURVE;
		vecFtr.push_back(ftr);

		pt0 = pt1;
	}

	ang -= PI;
	
	{
		offlen = 0.2 * fDrawScale;
		double cosa = cos(ang - PI / 6);
		double sina = sin(ang - PI / 6);
		PT_3DEX pt1;
		pt1.x = pt0.x + cosa * offlen;
		pt1.y = pt0.y + sina * offlen;
		pt1.z = pt0.z;

		//第一个点
		vector<Coord> coord;
		Coord xyz(pt1.x, pt1.y, pt1.z);
		coord.push_back(xyz);

		//第二个点
		Coord xyz1(pt0.x, pt0.y, pt0.z);
		coord.push_back(xyz1);

		osgFtr ftr;
		ftr.vecCoord = coord;
		ftr.type = CLS_GEOCURVE;
		vecFtr.push_back(ftr);
	}
	
	{
		offlen = 0.2 * fDrawScale;
		double cosa = cos(ang + PI / 6);
		double sina = sin(ang + PI / 6);
		PT_3DEX pt1;
		pt1.x = pt0.x + cosa * offlen;
		pt1.y = pt0.y + sina * offlen;
		pt1.z = pt0.z;

		//第一个点
		vector<Coord> coord;
		Coord xyz(pt1.x, pt1.y, pt1.z);
		coord.push_back(xyz);

		//第二个点
		Coord xyz1(pt0.x, pt0.y, pt0.z);
		coord.push_back(xyz1);

		osgFtr ftr;
		ftr.vecCoord = coord;
		ftr.type = CLS_GEOCURVE;
		vecFtr.push_back(ftr);
	}
	
}

double CGeomParser::calDirection(PT_3DEX pt0, PT_3DEX pt1)
{
	double deno = pt1.y - pt0.y;
	double sp = pt1.x - pt0.x;
	double angle = deno / sp;

	return atan(angle);
}

PT_3DEX CGeomParser::convertLongtoRGB(long clr)
{
	PT_3DEX rgb;
	rgb.x = GetRValue(clr) / 255.0;
	rgb.y = GetGValue(clr) / 255.0;
	rgb.z = GetBValue(clr) / 255.0;
	
	return rgb;
}