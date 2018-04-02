// ArcgisMdbCodeTab.cpp: implementation of the CArcgisMdbCodeTab class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ArcgisMdbCodeTab.h"
#include <fstream>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


string ArcgisCodeTabItem::FindMDBField(const string& fdb)
{
	int size = fieldPairs.size();
	for(int i=0;i<size; i+=2)
	{
		string item = fieldPairs[i];
		if(stricmp(item.c_str(),fdb.c_str())==0)
		{
			return fieldPairs[i+1];
		}
	}

	return string();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArcgisMdbCodeTab::CArcgisMdbCodeTab()
{

}

CArcgisMdbCodeTab::~CArcgisMdbCodeTab()
{
	Close();
}

bool CArcgisMdbCodeTab::Open(const string& fileName)
{
	ifstream ifs;
	ifs.open(fileName.c_str());
	if(ifs.fail())return false;
	string mark,fdbCode,mdbCode;	
	ArcgisCodeTabItem item;
	bool bBeginLayer = false;
	while(!ifs.eof())
	{	
		ifs>>mark>>fdbCode>>mdbCode;

		if (stricmp(mark.c_str(),"Layer")==0)
		{			
			bBeginLayer = true;

			item.fieldPairs.clear();
			item.strFdbCode = fdbCode;
			item.strArcgisMdbCode = mdbCode;
			m_codeTabData.push_back(item);
		}
		else if (stricmp(mark.c_str(),"Field")==0)
		{
			if(bBeginLayer)
			{
				item.fieldPairs.push_back(fdbCode);
				item.fieldPairs.push_back(mdbCode);
			}
		}
	}
    ifs.close();
	return true;
}

bool CArcgisMdbCodeTab::Close()
{
	m_codeTabData.clear();
	return true;
}

bool CArcgisMdbCodeTab::FindItemByFdbCode(const string& fdbCode,ArcgisCodeTabItem &outItem )
{
	for (int i=0;i<m_codeTabData.size();i++)
	{
		if(stricmp(m_codeTabData[i].strFdbCode.c_str(),fdbCode.c_str())==0)
		{
			outItem = m_codeTabData[i];
			return true;
		}
	}
	return false;
}
bool CArcgisMdbCodeTab::FindItemByArcGisCode(const string &arcgisCode ,ArcgisCodeTabItem &outItem )
{
	for (int i=0;i<m_codeTabData.size();i++)
	{
		if(stricmp(m_codeTabData[i].strArcgisMdbCode.c_str(),arcgisCode.c_str())==0)
		{
			outItem = m_codeTabData[i];
			return true;
		}
	}
	return false;
}