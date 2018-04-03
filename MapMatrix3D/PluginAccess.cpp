// PluginAccess.cpp: implementation of the CPluginAccess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SQLiteAccess.h "
#include "PluginAccess.h"
#include "AccessPlugin.h "
#include "Scheme.h "


extern CString DecodeString(const char *p);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPluginAccess::CPluginAccess()
{
	m_nCurLayID = -1;
	m_bUseSqliteEx = TRUE;
	m_pSqliteAccess = NULL;
	m_pAccessPlugin = NULL;
}

CPluginAccess::~CPluginAccess()
{
	Close();
	if (m_pAccessPlugin)
	{
		delete m_pAccessPlugin;
	}
	if (m_pSqliteAccess)
	{
		delete m_pSqliteAccess;
	}
}

BOOL CPluginAccess::Init(CAccessPlugin* pAccessPlugin,BOOL bUseSqliteEx)
{
	m_pAccessPlugin = pAccessPlugin;
	m_bUseSqliteEx = bUseSqliteEx;
	if (bUseSqliteEx)
	{
		m_pSqliteAccess = new CSQLiteAccess;
		if(!m_pSqliteAccess) return FALSE;
		m_pAccessPlugin->SetFdbAccessObj((AccessObj)m_pSqliteAccess);
	}
	return TRUE;
}

BOOL  CPluginAccess::Compress()
{
	BOOL ret = TRUE,ret2 = FALSE;
	if (m_bUseSqliteEx)
	{
		ret = m_pSqliteAccess->Compress();
	}
	ret2 = m_pAccessPlugin->Compress();
	return ret&&ret2;
}

BOOL CPluginAccess::Attach(LPCTSTR fileName)
{
	BOOL ret = TRUE,ret2 = FALSE;
	if (m_bUseSqliteEx)
	{
		ret = m_pSqliteAccess->Attach(fileName);
	}
	ret2 = m_pAccessPlugin->Attach(fileName);
	return ret&&ret2;
}

BOOL CPluginAccess::Close()
{
	BOOL ret = TRUE,ret2 = FALSE;

	ret2 = m_pAccessPlugin->Close();
// 	CppSQLite3DB  *dp = m_pSqliteAccess->GetSqliteDb();
// 	
// 	if (dp->isValid())
// 	{
// 		dp->close();
// 		}
	if (m_bUseSqliteEx)
	{
		ret = m_pSqliteAccess->Close();
	}
	
	return ret&&ret2;
}

BOOL CPluginAccess::CreateFileSys(long Scale,CScheme *ps)
{
	BOOL ret = TRUE,ret2 = FALSE;
	if (m_bUseSqliteEx)
	{
		ret = m_pSqliteAccess->CreateFileSys(Scale,ps);
		ret2 = m_pAccessPlugin->CreateFileSys(NULL,0);
	
		return ret&&ret2;
	}
	else
	{		
		int nLayerCnt = ps->GetLayerDefineCount();
		CSchemeLayerDefine *pLayerDef = NULL;	
		CArray<LayerDef,LayerDef> arrDefLay;	
		int id=5;
		for(int i=0;i<nLayerCnt;i++)
		{
			pLayerDef = ps->GetLayerDefine(i);
			
			if(!pLayerDef)continue;
			LayerDef LayDef;
			
			LayDef.m_nCode = pLayerDef->GetLayerCode();
			LayDef.m_nColor = pLayerDef->GetColor();
			LayDef.m_nGeoClass = pLayerDef->GetGeoClass();
			LayDef.m_strGroupName = pLayerDef->GetGroupName();
			LayDef.m_strLayName = pLayerDef->GetLayerName();
			pLayerDef->GetSupportClsName(LayDef.m_strSupportClsName);		
			int num;
			const XDefine* xDef = pLayerDef->GetXDefines(num);
			XDef * pxDef = new XDef[num];
			if(!pxDef) return FALSE;
			LayDef.m_pXDefines = pxDef;
			LayDef.m_nNum = num;
			for (int j=0;j<num;j++)
			{
				strcpy(pxDef->defvalue,xDef->defvalue);
				strcpy(pxDef->field,xDef->field);
				strcpy(pxDef->name,xDef->name);
				pxDef->valuelen = xDef->valuelen;
				pxDef->valuelen = xDef->valuelen;
				pxDef->valuetype = xDef->valuetype;
			}
			arrDefLay.Add(LayDef);
		}
		ret = m_pAccessPlugin->CreateFileSys(arrDefLay.GetData(),arrDefLay.GetSize());
		for (i=0;i<arrDefLay.GetSize();i++)
		{
			delete [](arrDefLay[i].m_pXDefines);
		}
		return ret;
	}
}

BOOL CPluginAccess::IsValid()
{
	BOOL ret = TRUE,ret2 = FALSE;
	if (m_bUseSqliteEx)
	{
		ret = m_pSqliteAccess->IsValid();
	}
	ret2 = m_pAccessPlugin->IsValid();
	return ret&&ret2;
}

void CPluginAccess::ClearAttrTables()
{	
	if (m_bUseSqliteEx)
	{
		m_pSqliteAccess->ClearAttrTables();
	}
	m_pAccessPlugin->ClearAttrTables();
}

BOOL CPluginAccess::OpenRead(LPCTSTR filename)
{
	if (m_bUseSqliteEx)
	{
		return m_pSqliteAccess->OpenRead(filename);
	}
	return TRUE;
//	m_pAccessPlugin->OpenRead(filename);
}

BOOL CPluginAccess::OpenWrite(LPCTSTR filename)
{
	if (m_bUseSqliteEx)
	{
		return m_pSqliteAccess->OpenWrite(filename);
	}
	return TRUE;
}

BOOL CPluginAccess::CloseRead()
{
	if (m_bUseSqliteEx)
	{
		return m_pSqliteAccess->CloseRead();
	}
	return TRUE;
}

BOOL CPluginAccess::CloseWrite()
{	
	if (m_bUseSqliteEx)
	{
		return m_pSqliteAccess->CloseWrite();
	}
	return TRUE;
}

//读取数据源信息
BOOL CPluginAccess::ReadDataSourceInfo(CDataSourceEx *pDS)
{
	BOOL ret = TRUE,ret2 = FALSE;
	if (m_bUseSqliteEx)
	{
		ret = m_pSqliteAccess->ReadDataSourceInfo(pDS);
	}
	ret2 = m_pAccessPlugin->ReadDataSourceInfo((DataId)pDS);

	return ret&&ret2;
}

//读取层
CFtrLayer* CPluginAccess::GetFirstFtrLayer(long &idx)
{
	if (m_bUseSqliteEx)
	{
		return m_pSqliteAccess->GetFirstFtrLayer(idx);
	}
	else
		return (CFtrLayer*)m_pAccessPlugin->GetFirstFtrLayer(idx);
}

CFtrLayer* CPluginAccess::GetNextFtrLayer(long &idx)
{
	if (m_bUseSqliteEx)
	{
		return m_pSqliteAccess->GetNextFtrLayer(idx);
	}
	else
		return (CFtrLayer*)m_pAccessPlugin->GetNextFtrLayer(idx);
}

int CPluginAccess::GetFtrLayerCount()
{
	if (m_bUseSqliteEx)
	{
		return m_pSqliteAccess->GetFtrLayerCount();
	}
	else
		return m_pAccessPlugin->GetFtrLayerCount();
}

//读取地物
CFeature *CPluginAccess::GetFirstFeature(long &idx, CValueTable *exAttriTab)
{
	if (m_bUseSqliteEx)
	{
		 CFeature *pFtr = m_pSqliteAccess->GetFirstFeature(idx,exAttriTab);
		
		 if (pFtr)
		 {
			 m_nCurLayID = m_pSqliteAccess->GetCurFtrLayID();			 
		 }
		 else
		 {
			 
			 pFtr = m_pAccessPlugin->GetFirstFeature(idx,exAttriTab);
			 m_nCurLayID = m_pAccessPlugin->GetCurFtrLayID();			
		 }
		 return pFtr;
	}
	else
	{
		CFeature *pFtr = m_pAccessPlugin->GetFirstFeature(idx,exAttriTab);
		m_nCurLayID = m_pAccessPlugin->GetCurFtrLayID();
		return pFtr;
	}
}

CFeature *CPluginAccess::GetNextFeature(long &idx, CValueTable *exAttriTab)
{
	if (m_bUseSqliteEx)
	{
		CFeature *pFtr = m_pSqliteAccess->GetNextFeature(idx,exAttriTab);
		if(pFtr)
		{
			m_nCurLayID = m_pSqliteAccess->GetCurFtrLayID();
		}
		else
		{
			pFtr = m_pAccessPlugin->GetNextFeature(idx,exAttriTab);
			m_nCurLayID = m_pAccessPlugin->GetCurFtrLayID();
		}
		return pFtr;
	}
	else
	{
		CFeature *pFtr =  m_pAccessPlugin->GetNextFeature(idx,exAttriTab);
		m_nCurLayID = m_pAccessPlugin->GetCurFtrLayID();
		return pFtr;
	}
}

int CPluginAccess::GetFeatureCount()
{
	if (m_bUseSqliteEx)
	{
		int nSum = m_pSqliteAccess->GetFeatureCount();
		nSum += m_pAccessPlugin->GetFeatureCount();
		return nSum;		 
	}
	else
		return m_pAccessPlugin->GetFeatureCount();
}

int CPluginAccess::GetCurFtrLayID()
{
	return m_nCurLayID;	
}

int CPluginAccess::GetCurFtrGrpID()
{
	return 0;
}

//读取扩展属性信息
BOOL CPluginAccess::ReadXAttribute(CFeature *pFtr, CValueTable& tab)
{
	if (m_bUseSqliteEx)
	{
		if(!m_pSqliteAccess->ReadXAttribute(pFtr,tab))
		{
			return m_pAccessPlugin->ReadXAttribute(pFtr,tab);
		}
		else
			return TRUE;		 
	}
	else
	{
		return m_pAccessPlugin->ReadXAttribute(pFtr,tab);
	}
}

//更新数据源信息
void CPluginAccess::WriteDataSourceInfo(CDataSourceEx *pDS)
{
	if (m_bUseSqliteEx)
	{
		m_pSqliteAccess->WriteDataSourceInfo(pDS);			 
	}
	else
	{
		m_pAccessPlugin->WriteDataSourceInfo((DataId)pDS);
	}
}

//更新层
BOOL CPluginAccess::SaveFtrLayer(CFtrLayer* pLayer)
{
	if (m_bUseSqliteEx)
	{
		return m_pSqliteAccess->SaveFtrLayer(pLayer);			 
	}
	else
	{
		return m_pAccessPlugin->SaveFtrLayer(LayId(pLayer));
	}
}

BOOL CPluginAccess::DelFtrLayer(CFtrLayer* pLayer)
{
	if (m_bUseSqliteEx)
	{
		return m_pSqliteAccess->DelFtrLayer(pLayer);			 
	}
	else
	{
		return m_pAccessPlugin->DelFtrLayer(LayId(pLayer));
	}
}

//更新地物
BOOL CPluginAccess::SetCurFtrLayID(int id)
{
	m_nCurLayID = id;
	if (m_bUseSqliteEx)
	{
		m_pSqliteAccess->SetCurFtrLayID(id);		 
	}	
	return m_pAccessPlugin->SetCurFtrLayID(id);
	
}

BOOL CPluginAccess::SetCurFtrGrpID(int id)
{
	return TRUE;
}

BOOL CPluginAccess::SaveFeature(CFeature *pFt, CValueTable *exAttriTab)
{
	if (m_bUseSqliteEx)
	{
		if(!m_pSqliteAccess->SaveFeature(pFt,exAttriTab))
			return m_pAccessPlugin->SaveFeature(pFt,exAttriTab);
		else
			return TRUE;
	}
	else
	{
		return m_pAccessPlugin->SaveFeature(pFt,exAttriTab);
	}
}

BOOL CPluginAccess::DelFeature(CFeature *pFt)
{
	if (m_bUseSqliteEx)
	{
		if(!m_pSqliteAccess->DelFeature(pFt))
			return m_pAccessPlugin->DelFeature(pFt);
		else
			return TRUE;
	}
	else
	{
		return m_pAccessPlugin->DelFeature(pFt);
	}
}

BOOL CPluginAccess::DelXAttributes(CFeature *pFtr)
{
	if (m_bUseSqliteEx)
	{
		if(!m_pSqliteAccess->DelXAttributes(pFtr))
			return m_pAccessPlugin->DelXAttributes(pFtr);
		else
			return TRUE;
	}
	else
	{
		return m_pAccessPlugin->DelXAttributes(pFtr);
	}
}

BOOL CPluginAccess::RestoreXAttributes(CFeature *pFtr)
{
	if (m_bUseSqliteEx)
	{
		if(!m_pSqliteAccess->RestoreXAttributes(pFtr))
			return m_pAccessPlugin->RestoreXAttributes(pFtr);
		else
			return TRUE;
	}
	else
	{
		return m_pAccessPlugin->RestoreXAttributes(pFtr);
	}
}

//更新扩展属性信息
BOOL CPluginAccess::SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx)
{
	if (m_bUseSqliteEx)
	{
		if(!m_pSqliteAccess->RestoreXAttributes(pFtr))
			return m_pAccessPlugin->RestoreXAttributes(pFtr);
		else
			return TRUE;
	}
	else
	{
		return m_pAccessPlugin->RestoreXAttributes(pFtr);
	}
}

void CPluginAccess::BatchUpdateBegin()
{
	if (m_bUseSqliteEx)
	{
		m_pSqliteAccess->BatchUpdateBegin();		
	}
	else
	{
		m_pAccessPlugin->BatchUpdateBegin();
	}
}

void CPluginAccess::BatchUpdateEnd()
{
	if (m_bUseSqliteEx)
	{
		m_pSqliteAccess->BatchUpdateEnd();		
	}
	else
	{
		m_pAccessPlugin->BatchUpdateEnd();
	}
}