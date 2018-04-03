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
����fdb�ļ���Arcgis mdb�ļ��Ĳ�����ձ����������ֶεĶ��գ�
���ĸ�ʽ���£�
-----------------------------------------
Layer   fdb����   arcgismdb�������
Field   fdb�ֶ�	  arcgismdb�ֶ�
...
...
Layer ....
-----------------------------------------
ע����fdb��ĳ��������ڵ��ֶ��ڲ�����ձ����Ҳ���ʱ�����ʾ��Ӧ��arcgismdb�е�ͬһ��
��������arcgismdb�����ҵ�����ӣ����ܾͷ���д�ֶ�
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
