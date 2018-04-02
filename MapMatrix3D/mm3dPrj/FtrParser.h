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
	* brief getFtrLyrData ���������е�������Ϣ��������
	* param pLayer ������
	* param scale ���ŵĳ߶�
	* param vecFtr ���ص�ʸ������
	*/
	virtual void getFtrLyrData(CFtrLayer* pLayer, std::vector<osgCall::osgFtr> &vecFtr, CDlgDataSource* pDS = nullptr);

	/*
	* brief getFtrLyrData ���������е�������Ϣ��������
	* param pLayer ������
	* param scale ���ŵĳ߶�
	* param vecFtr ���ص�ʸ������
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
	* @brief ���ν�����, ��ʸ�����ݽ�����mm3d�п�ʶ�������
	*/
	std::shared_ptr<CGeomParser> pGeomParser;
	COsgbView * m_pPara;
};

