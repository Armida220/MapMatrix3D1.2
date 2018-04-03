#pragma once
#include <string>
#include <vector>


namespace EditBase
{
	class GrBuffer;
	struct GrVertexList;
	struct GrPolygon;
}


namespace osgCall
{
	class osgFtr;
}

/*
* brief CBufParser ����buf����
*/
class CBufParser
{
public:
	CBufParser(std::string idStr, PT_3D layerClr, bool bIsLocal = true);

	/*
	* brief getBufData ��ȡbuffer�еĻ�����Ϣ,������Ⱦ��Ҫ���ⲿ����Ϣһ���ϲ���osgftr��
	* param pBuf ��������buf
	* param vecFtr ���ս�����������������, ���Ը�osgֱ����Ⱦ
	*/
	void getBufData(const GrBuffer *pBuf, std::vector<osgCall::osgFtr> &vecFtr, bool fillPolygon = true);

	void getBufDataToWhiteModel(const GrBuffer *pBuf, std::vector<osgCall::osgFtr> &vecFtr, double bh);
protected:
	/*
	* brief addPt ����������ɫ����һ����,����ӵ�ʸ��������
	* param pt0 ��ӵ������
	* param color ��ӵ��������ɫ
	* param vecFtr ���ص�ʸ������
	*/
	virtual void addPt(const PT_3D& pt0, const PT_3D& color, std::vector<osgCall::osgFtr> &vecFtr);

	/*
	* brief addPtString ���������б���һ���㴮, ����ӵ�ʸ��������
	* param list �㴮�����꼯��
	* param color �㴮��������ɫ
	* param vecFtr ���ص�ʸ������
	*/
	virtual void addPtString(const GrVertexList *list, const PT_3D& color, std::vector<osgCall::osgFtr> &vecFtr);

	/*
	* brief addLine ���������б���һ����, ����ӵ�ʸ��������
	* param list �ߵ����꼯��
	* param color �ߵ�������ɫ
	* param vecFtr ���ص�ʸ������
	*/
	virtual void addLine(const GrVertexList *list, const PT_3D& color, std::vector<osgCall::osgFtr> &vecFtr);

	/*
	* brief addPolygon ���������б���һ����, ����ӵ�ʸ��������
	* param list ������꼯��
	* param color ���������ɫ
	* param vecFtr ���ص�ʸ������
	*/
	virtual void addPolygon(const GrPolygon *cgr, const PT_3D& color, std::vector<osgCall::osgFtr> &vecFtr);

	/*
	* brief addText �����ı�, ����ӵ�ʸ��������
	* param list �ı������꼯��
	* param color �ı���������ɫ
	* param textSize �ı��Ĵ�С
	* param vecFtr ���ص�ʸ������
	*/
	virtual void addText(const PT_3D& pt0, const std::string &text, const PT_3D& color, const TextSettings & textSize, std::vector<osgCall::osgFtr> &vecFtr);

protected:
	/*
	* brief idStr ����id�ŵ��ַ���
	*/
	std::string mIdStr;
	
	/*
	* brief mLayerClr ����ɫ,bylayerʱ����Ҫ��Ⱦ����
	*/
	PT_3D mLayerClr;

	/*
	* brief mbLocal �Ƿ���ʾ��ͼ
	*/
	bool mbLocal;
};

