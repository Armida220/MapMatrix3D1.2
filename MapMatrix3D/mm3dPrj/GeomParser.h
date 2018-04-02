#pragma once
#include <vector>
#include "Geometry.h"

namespace osgCall
{
	class ICallBack;
	class osgFtr;
}

/*
*  @brief CGeomParser
*/
class CGeomParser
{
public:
	CGeomParser();
	virtual ~CGeomParser();

	/*
	*  @brief parseGeoPt 解析成点
	*  @param geom 几何体
	*  @return 返回解析后的特征
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoPt(CGeometry* geom);

	/*
	*  @brief parseGeoDirPt 解析成有向点
	*  @param geom 几何体
	*  @param fDrawScale 尺度
	*  @return 返回解析后的特征
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoDirPt(CGeometry* geom,
		double fDrawScale);

	/*
	*  @brief parseGeoCurve 解析成几何曲线
	*  @param geom 几何体
	*  @return 返回解析后的特征
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoCurve(CGeometry* geom);

	/*
	*  @brief parseGeoDCurve 解析成双几何曲线
	*  @param geom 几何体
	*  @param dscale 尺度
	*  @return 返回解析后的特征
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoDCurve(CGeometry* geom);

	/*
	*  @brief parseGeoCurve 解析成平行线
	*  @param geom 几何体
	*  @param dscale 尺度
	*  @return 返回解析后的特征
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoParallel(CGeometry* geom);

	/*
	*  @brief parseGeoSurface 解析成面
	*  @param geom 几何体
	*  @return 返回解析后的特征
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoSurface(CGeometry* geom);

	/*
	*  @brief parseMultiGeoSurface 解析成多面面
	*  @param geom 几何体
	*  @return 返回解析后的特征
	*/
	virtual std::vector<osgCall::osgFtr> parseMultiGeoSurface(CGeometry* geom);

	/*
	*  @brief parseGeoText 解析成文本
	*  @param geom 几何体
	*  @return 返回解析后的特征
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoText(CGeometry* geom);

private:
	/*
	*  @brief createArrow 创建箭头
	*  @param direction 方向
	*  @param pt 三维点
	*  @param fDrawScale 绘制的尺度
	*  @param vecFtr 特征集合
	*/
	void createArrow(double direction, PT_3DEX pt, std::vector<osgCall::osgFtr> &vecFtr,
		float fDrawScale);

	/*
	*  @brief calDirection 计算方向
	*  @param pt0 三维点
	*  @param pt1 三维点
	*/
	double calDirection(PT_3DEX pt0, PT_3DEX pt1);

	/*
	*  @brief convertLongtoRGB 计算方向
	*  @param pt0 三维点
	*  @param pt1 三维点
	*/
	PT_3DEX convertLongtoRGB(long clr);
};

