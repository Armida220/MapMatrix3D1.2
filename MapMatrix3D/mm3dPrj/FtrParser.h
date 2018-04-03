#pragma once
#include "FtrLayer.h"
#include <vector>
#include <memory>
#include <map>
#include <string>
namespace osgCall
{
	class osgFtr;
}
class CDlgDataSource;
class CGeomParser;
class COsgbView;

struct wmID_h
{
	std::string id;
	double h;
};

typedef std::map<std::string, wmID_h> sid_wmid;

class CFtrParser
{
public:
	CFtrParser(COsgbView * p);
	~CFtrParser();

	/*
	* brief getFtrLyrData 把特征层中的所有信息解析出来
	* param pLayer 特征层
	* param scale 符号的尺度
	* param vecFtr 返回的矢量集合
	*/
	virtual void getFtrLyrData(CFtrLayer* pLayer, std::vector<osgCall::osgFtr> &vecFtr, CDlgDataSource* pDS = nullptr);

	/*
	* brief getFtrLyrData 把特征层中的所有信息解析出来
	* param pLayer 特征层
	* param scale 符号的尺度
	* param vecFtr 返回的矢量集合
	*/
	virtual void getAllFtrInfo(CFeature* feature, std::vector<osgCall::osgFtr> &vecFtr, bool isSelect = false);

	virtual void getFtrLyrDataOnlyID(CFtrLayer* pLayer, std::vector<osgCall::osgFtr> &vecFtr);

	virtual void getAllFtrInfo(CFeature* feature, CFtrLayer *pLayer, CDlgDataSource* pDS, 
		std::vector<osgCall::osgFtr> &vecFtr, const PT_3D & rgb, bool isSelect = false);

	void getDataSourceInfo(CDlgDataSource* pDS,
		std::vector<osgCall::osgFtr> &vecFtr);

	void getFtrWhiteModel(CFeature* feature,
		const sid_wmid & featerID_WhiteModelID,
		std::vector<osgCall::osgFtr> &vecFtr);
	void getFtrWhiteModel(const FTR_HANDLE * pftr, int num,
		const sid_wmid & featerID_WhiteModelID,
		std::vector<osgCall::osgFtr> &vecFtr);
protected:


	/**
	* @brief 几何解析器, 把矢量数据解析成mm3d中可识别的数据
	*/
	std::shared_ptr<CGeomParser> pGeomParser;
	COsgbView * m_pPara;
};

