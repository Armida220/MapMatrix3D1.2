// ArcgisMdbCodeTab.h: interface for the CArcgisMdbCodeTab class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARCGISMDBCODETAB_H__8ED30CF1_43C3_43FE_B623_FD93F0C35262__INCLUDED_)
#define AFX_ARCGISMDBCODETAB_H__8ED30CF1_43C3_43FE_B623_FD93F0C35262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <vector>
using namespace std;
/*******************************************************************************
管理fdb文件到Arcgis mdb文件的层码对照表（包括属性字段的对照）
码表的格式如下：
-----------------------------------------
Layer   fdb层码   arcgismdb地物表名
Field   fdb字段	  arcgismdb字段
...
...
Layer ....
-----------------------------------------
注：当fdb中某个地物存在的字段在层码对照表中找不到时，则表示对应到arcgismdb中的同一字
段名，在arcgismdb中能找到就添加，不能就放弃写字段
*******************************************************************************/
struct ArcgisCodeTabItem 
{
	string strFdbCode;
	string strArcgisMdbCode;
	vector<string> fieldPairs;

	string FindMDBField(const string& fdb);
};

class CArcgisMdbCodeTab  
{
public:
	CArcgisMdbCodeTab();
	virtual ~CArcgisMdbCodeTab();
	bool Open(const string& fileName);
	bool Close();
	bool FindItemByFdbCode(const string& fdbCode,ArcgisCodeTabItem &outItem );
	bool FindItemByArcGisCode(const string& arcgisCode,ArcgisCodeTabItem &outItem);
	vector<ArcgisCodeTabItem> m_codeTabData;
};

#endif // !defined(AFX_ARCGISMDBCODETAB_H__8ED30CF1_43C3_43FE_B623_FD93F0C35262__INCLUDED_)
