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
* brief CBufParser 解析buf的类
*/
class CBufParser
{
public:
	CBufParser(std::string idStr, PT_3D layerClr, bool bIsLocal = true);

	/*
	* brief getBufData 获取buffer中的基本信息,并把渲染需要的外部的信息一并合并到osgftr中
	* param pBuf 被解析的buf
	* param vecFtr 最终解析出来的特征集合, 可以给osg直接渲染
	*/
	void getBufData(const GrBuffer *pBuf, std::vector<osgCall::osgFtr> &vecFtr, bool fillPolygon = true);

	void getBufDataToWhiteModel(const GrBuffer *pBuf, std::vector<osgCall::osgFtr> &vecFtr, double bh);
protected:
	/*
	* brief addPt 根据坐标颜色创建一个点,并添加到矢量集合中
	* param pt0 添加点的坐标
	* param color 添加点的最终颜色
	* param vecFtr 返回的矢量集合
	*/
	virtual void addPt(const PT_3D& pt0, const PT_3D& color, std::vector<osgCall::osgFtr> &vecFtr);

	/*
	* brief addPtString 根据坐标列表创建一个点串, 并添加到矢量集合中
	* param list 点串的坐标集合
	* param color 点串的最终颜色
	* param vecFtr 返回的矢量集合
	*/
	virtual void addPtString(const GrVertexList *list, const PT_3D& color, std::vector<osgCall::osgFtr> &vecFtr);

	/*
	* brief addLine 根据坐标列表创建一根线, 并添加到矢量集合中
	* param list 线的坐标集合
	* param color 线的最终颜色
	* param vecFtr 返回的矢量集合
	*/
	virtual void addLine(const GrVertexList *list, const PT_3D& color, std::vector<osgCall::osgFtr> &vecFtr);

	/*
	* brief addPolygon 根据坐标列表创建一个面, 并添加到矢量集合中
	* param list 面的坐标集合
	* param color 面的最终颜色
	* param vecFtr 返回的矢量集合
	*/
	virtual void addPolygon(const GrPolygon *cgr, const PT_3D& color, std::vector<osgCall::osgFtr> &vecFtr);

	/*
	* brief addText 增加文本, 并添加到矢量集合中
	* param list 文本的坐标集合
	* param color 文本的最终颜色
	* param textSize 文本的大小
	* param vecFtr 返回的矢量集合
	*/
	virtual void addText(const PT_3D& pt0, const std::string &text, const PT_3D& color, const TextSettings & textSize, std::vector<osgCall::osgFtr> &vecFtr);

protected:
	/*
	* brief idStr 保存id号的字符串
	*/
	std::string mIdStr;
	
	/*
	* brief mLayerClr 层颜色,bylayer时候需要渲染出来
	*/
	PT_3D mLayerClr;

	/*
	* brief mbLocal 是否显示本图
	*/
	bool mbLocal;
};

