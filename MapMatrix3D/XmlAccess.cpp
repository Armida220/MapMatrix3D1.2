// XmlAccess.cpp: implementation of the CXmlAccess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "XmlAccess.h"
#include "FtrLayer.h"
#include "Feature.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlAccess::CXmlAccess()
{
	m_nCurFtrLayID = -1;
	m_nCurFtrGrpID = -1;
	m_nOpenMode = modeRead;
}

CXmlAccess::~CXmlAccess()
{
	Close();
}

BOOL CXmlAccess::OpenRead(LPCTSTR filename)
{
	m_strReadName = filename;
	m_nOpenMode = modeRead;
	
	return m_xmlfile.Load(filename);
}

BOOL CXmlAccess::OpenWrite(LPCTSTR filename)
{
	m_strWriteName = filename;
	m_nOpenMode = modeWrite;

	m_xmlfile.SetDoc(NULL);
	m_xmlfile.AddElem(XMLTAG_FILE);
	m_xmlfile.IntoElem();
	
	return TRUE;
}

void CXmlAccess::Close()
{
	if( m_nOpenMode==modeWrite)
	{
		CloseWrite();
	}
	else
	{
		CloseRead();
	}
}

BOOL CXmlAccess::CloseRead()
{	
	m_xmlfile.SetDoc(NULL);
	m_nCurFtrLayID = -1;
	m_nCurFtrGrpID = -1;
	m_nOpenMode = modeRead;
	return TRUE;
}

BOOL CXmlAccess::CloseWrite()
{
	BOOL bRet = FALSE;
	if( m_nOpenMode==modeWrite)
	{
		CString str = m_xmlfile.GetDoc();
		bRet = m_xmlfile.WriteTextFile(m_strWriteName,str);
	}
	
	m_xmlfile.SetDoc(NULL);
	m_nCurFtrLayID = -1;
	m_nCurFtrGrpID = -1;
	m_nOpenMode = modeRead;

	return bRet;
}


BOOL CXmlAccess::ReadDataSourceInfo(CDataSourceEx *pDS)
{
	if( !m_xmlfile.IsWellFormed() )
		return FALSE;

	m_xmlfile.ResetPos();
	if( !m_xmlfile.FindElem(XMLTAG_FILE,TRUE) )
		return FALSE;	
	m_xmlfile.IntoElem();

	if( !m_xmlfile.FindElem(XMLTAG_DATASOURCE,TRUE) )
		return FALSE;
	m_xmlfile.IntoElem();

	CValueTable tab;
	tab.BeginAddValueItem();
	
	if( !Xml_ReadValueTable(m_xmlfile,tab) )
	{
		return FALSE;
	}

	tab.EndAddValueItem();
	pDS->ReadFrom(tab);

	return TRUE;
}


void CXmlAccess::WriteDataSourceInfo(CDataSourceEx *pDS)
{
	if( !pDS )return;
	if( m_nOpenMode==modeRead )return;
	
	CValueTable tab;	
	_variant_t var;
	
	if( m_nOpenMode==modeWrite )
	{
		m_xmlfile.AddElem(XMLTAG_DATASOURCE);
		m_xmlfile.IntoElem();
		
		tab.BeginAddValueItem();
		pDS->WriteTo(tab);		
		tab.EndAddValueItem();	
		
		Xml_WriteValueTable(m_xmlfile,tab);
		
		m_xmlfile.OutOfElem();
	}
	
	return;
}



//读取层
CFtrLayer *CXmlAccess::GetFirstFtrLayer(long &idx)
{
	if( !m_xmlfile.IsWellFormed() )
		return NULL;

	m_xmlfile.ResetPos();
	if( !m_xmlfile.FindElem(XMLTAG_FILE,TRUE) )
		return NULL;

	m_xmlfile.IntoElem();
	if( !m_xmlfile.FindElem(XMLTAG_LAYER,TRUE) )
		return NULL;
	m_xmlfile.IntoElem();

	CValueTable tab;
	tab.BeginAddValueItem();

	if( !Xml_ReadValueTable(m_xmlfile,tab) )
	{
		m_xmlfile.OutOfElem();
		return NULL;
	}

	tab.EndAddValueItem();
	m_xmlfile.OutOfElem();

	CFtrLayer *pLayer = new CFtrLayer;
	if( !pLayer )return NULL;
	pLayer->ReadFrom(tab);

	if(m_nOpenMode==modeRead)
		pLayer->SetMapName(m_strReadName);
	else
		pLayer->SetMapName(m_strWriteName);

	return pLayer;
}


CFtrLayer *CXmlAccess::GetNextFtrLayer(long &idx)
{
	if( !m_xmlfile.IsWellFormed() )
		return NULL;

	if( !m_xmlfile.FindElem(XMLTAG_LAYER) )
		return NULL;
	m_xmlfile.IntoElem();
	
	CValueTable tab;
	tab.BeginAddValueItem();
	
	if( !Xml_ReadValueTable(m_xmlfile,tab) )
	{
		m_xmlfile.OutOfElem();
		return NULL;
	}
	
	tab.EndAddValueItem();
	m_xmlfile.OutOfElem();

	CFtrLayer *pLayer = new CFtrLayer;
	if( !pLayer )return NULL;

	pLayer->ReadFrom(tab);
	if(m_nOpenMode==modeRead)
		pLayer->SetMapName(m_strReadName);
	else
		pLayer->SetMapName(m_strWriteName);	
	
	return pLayer;
}



//读取地物
CFeature *CXmlAccess::GetFirstFeature(long &idx, CValueTable *exAttriTab)
{
	if( !m_xmlfile.IsWellFormed() )
		return NULL;

	m_xmlfile.ResetPos();
	if( !m_xmlfile.FindElem(XMLTAG_FILE,TRUE) )
		return NULL;
	
	m_xmlfile.IntoElem();
	if( !m_xmlfile.FindElem(XMLTAG_FEATURE,TRUE) )
		return NULL;
	m_xmlfile.IntoElem();
	
	CValueTable tab;
	tab.BeginAddValueItem();
	
	if( !Xml_ReadValueTable(m_xmlfile,tab) )
	{
		m_xmlfile.OutOfElem();
		return NULL;
	}
	
	tab.EndAddValueItem();
	m_xmlfile.OutOfElem();

	const CVariantEx *p;
	tab.GetValue(0,XMLTAG_LAYERID,p);	
	m_nCurFtrLayID = (long)(_variant_t)*p;
	tab.GetValue(0,XMLTAG_GROUPID,p);
	m_nCurFtrGrpID = (long)(_variant_t)*p;

	return (CFeature*)CreateObject(tab);
}


CFeature *CXmlAccess::GetNextFeature(long &idx, CValueTable *exAttriTab)
{
	if( !m_xmlfile.IsWellFormed() )
		return NULL;
	
	if( !m_xmlfile.FindElem(XMLTAG_FEATURE) )
		return NULL;
	m_xmlfile.IntoElem();
	
	CValueTable tab;
	tab.BeginAddValueItem();
	
	if( !Xml_ReadValueTable(m_xmlfile,tab) )
	{
		m_xmlfile.OutOfElem();
		return NULL;
	}
	
	tab.EndAddValueItem();
	m_xmlfile.OutOfElem();

	const CVariantEx *p;
	tab.GetValue(0,XMLTAG_LAYERID,p);	
	m_nCurFtrLayID = (long)(_variant_t)*p;
	tab.GetValue(0,XMLTAG_GROUPID,p);
	m_nCurFtrGrpID = (long)(_variant_t)*p;

	return (CFeature*)CreateObject(tab);
}


int CXmlAccess::GetCurFtrLayID()
{
	return m_nCurFtrLayID;
}


int CXmlAccess::GetCurFtrGrpID()
{
	return m_nCurFtrGrpID;
}

BOOL CXmlAccess::ReadXAttribute(CFeature *pFtr, CValueTable& tab)
{
	return TRUE;
}


BOOL CXmlAccess::DelXAttributes(CFeature *pFtr)
{
	return TRUE;
}

BOOL CXmlAccess::RestoreXAttributes(CFeature *pFtr)
{
	return TRUE;
}

//更新层
BOOL CXmlAccess::SaveFtrLayer(CFtrLayer *pLayer)
{
	if( !pLayer )return FALSE;
	if( m_nOpenMode==modeRead )return FALSE;

	if( m_nOpenMode==modeWrite )
	{
		// add the new layer
		m_xmlfile.AddElem(XMLTAG_LAYER);
		m_xmlfile.IntoElem();

		CValueTable tab;
		tab.BeginAddValueItem();
		pLayer->WriteTo(tab);
		tab.EndAddValueItem();
		Xml_WriteValueTable(m_xmlfile,tab);

		m_xmlfile.OutOfElem();
	}

	return TRUE;
}



//更新地物
BOOL CXmlAccess::SetCurFtrLayID(int id)
{
	m_nCurFtrLayID = id;
	return TRUE;
}


BOOL CXmlAccess::SetCurFtrGrpID(int id)
{
	m_nCurFtrGrpID = id;
	return TRUE;
}


BOOL CXmlAccess::SaveFeature(CFeature *pFt, CValueTable *exAttriTab)
{
	if( !pFt )return FALSE;
	if( m_nOpenMode==modeRead )return FALSE;

	CValueTable tab;	
	_variant_t var;
	
	if( m_nOpenMode==modeWrite )
	{
		m_xmlfile.AddElem(XMLTAG_FEATURE);
		m_xmlfile.IntoElem();

		tab.BeginAddValueItem();
		
		var = (long)m_nCurFtrLayID;
		tab.AddValue(XMLTAG_LAYERID,&CVariantEx(var));
		var = (long)m_nCurFtrGrpID;
		tab.AddValue(XMLTAG_GROUPID,&CVariantEx(var));		
		pFt->WriteTo(tab);
		
		tab.EndAddValueItem();		
		Xml_WriteValueTable(m_xmlfile,tab);

		m_xmlfile.OutOfElem();
	}

	return TRUE;
}

BOOL CXmlAccess::SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx)
{
	return TRUE;
}