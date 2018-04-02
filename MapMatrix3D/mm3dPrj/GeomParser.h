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
	*  @brief parseGeoPt �����ɵ�
	*  @param geom ������
	*  @return ���ؽ����������
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoPt(CGeometry* geom);

	/*
	*  @brief parseGeoDirPt �����������
	*  @param geom ������
	*  @param fDrawScale �߶�
	*  @return ���ؽ����������
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoDirPt(CGeometry* geom,
		double fDrawScale);

	/*
	*  @brief parseGeoCurve �����ɼ�������
	*  @param geom ������
	*  @return ���ؽ����������
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoCurve(CGeometry* geom);

	/*
	*  @brief parseGeoDCurve ������˫��������
	*  @param geom ������
	*  @param dscale �߶�
	*  @return ���ؽ����������
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoDCurve(CGeometry* geom);

	/*
	*  @brief parseGeoCurve ������ƽ����
	*  @param geom ������
	*  @param dscale �߶�
	*  @return ���ؽ����������
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoParallel(CGeometry* geom);

	/*
	*  @brief parseGeoSurface ��������
	*  @param geom ������
	*  @return ���ؽ����������
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoSurface(CGeometry* geom);

	/*
	*  @brief parseMultiGeoSurface �����ɶ�����
	*  @param geom ������
	*  @return ���ؽ����������
	*/
	virtual std::vector<osgCall::osgFtr> parseMultiGeoSurface(CGeometry* geom);

	/*
	*  @brief parseGeoText �������ı�
	*  @param geom ������
	*  @return ���ؽ����������
	*/
	virtual std::vector<osgCall::osgFtr> parseGeoText(CGeometry* geom);

private:
	/*
	*  @brief createArrow ������ͷ
	*  @param direction ����
	*  @param pt ��ά��
	*  @param fDrawScale ���Ƶĳ߶�
	*  @param vecFtr ��������
	*/
	void createArrow(double direction, PT_3DEX pt, std::vector<osgCall::osgFtr> &vecFtr,
		float fDrawScale);

	/*
	*  @brief calDirection ���㷽��
	*  @param pt0 ��ά��
	*  @param pt1 ��ά��
	*/
	double calDirection(PT_3DEX pt0, PT_3DEX pt1);

	/*
	*  @brief convertLongtoRGB ���㷽��
	*  @param pt0 ��ά��
	*  @param pt1 ��ά��
	*/
	PT_3DEX convertLongtoRGB(long clr);
};

