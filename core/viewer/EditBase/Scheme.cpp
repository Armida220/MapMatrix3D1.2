// Scheme.cpp: implementation of the CScheme class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "Scheme.h"
#include "Markup.h"
#include "Feature.h"
#include "SmartViewFunctions.h"
#include "textfile.h"
#include "ObjectXmlIO.h"
#include "GeoText.h"
#include "CADSymFile.h"
#include <vector>
#include "GeoCurve.h"
#include "GeoParallel.h"
#include "RegDef2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin


XDefine::XDefine()
{
	memset(this,0,sizeof(XDefine));
}


const XDefine& XDefine::operator=(XDefine def)
{
	_tcscpy(field,def.field);
	_tcscpy(name,def.name);
	_tcscpy(defvalue,def.defvalue);
	_tcscpy(valueRange,def.valueRange);
	valuetype = def.valuetype;
	valuelen = def.valuelen;
	isMust = def.isMust;
	nDigit = def.nDigit;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL less_definelayCode(CSchemeLayerDefine* idx1, CSchemeLayerDefine* idx2)
{
	return idx1->GetLayerCode()<idx2->GetLayerCode();
}
CScheme::CScheme()
{
	m_pRecentLD = NULL;
	m_pCADSymbolLib = NULL;
	m_pCADSymbolLib = NULL;
	m_nMaxRecentNum = 40;
	CWinApp *pApp = AfxGetApp();
	if (pApp)
	{
		m_nMaxRecentNum = pApp->GetProfileInt(REGPATH_USER, REGITEM_RECENTLAYER, 40);
	}
}

CScheme::~CScheme()
{
	Clear();	
}

void CScheme::Clear()
{
	// 每次加载要清理
	m_UserRecent.Clear();
	m_strSortedGroupName.RemoveAll();
	for(int i=0; i<m_arrPLayDefines.GetSize(); i++)
	{
		if(m_arrPLayDefines.GetAt(i) != NULL)
		{
			delete m_arrPLayDefines.GetAt(i);
		}
	}
	m_arrPLayDefines.RemoveAll();

	m_pRecentLD = NULL;
	if( m_pCADSymbolLib )
	{
		delete m_pCADSymbolLib;
		m_pCADSymbolLib = NULL;
	}
}

BOOL CScheme::WriteTo(CString &strXml)const
{
	CMarkup xmlFile;
//	xmlFile.SetDoc(strXml);
	CString data;
	if( xmlFile.AddElem(XML_SCHEME) )
	{
		xmlFile.IntoElem();

		data.Empty();
		for (int i=0; i<m_strSortedGroupName.GetSize(); i++)
		{
			data += m_strSortedGroupName.GetAt(i);
			if (i == m_strSortedGroupName.GetSize()-1)
				break;
			
			data += ",";
		}
		xmlFile.AddElem(XML_SORTEDGROUPNAME,data);

		//////////////////////////////////////////////////////////////////////////
		//把属于同一个层的放在一起
// 		CStringArray arrGroup;
// 		for(int i=0; i<m_arrPLayDefines.GetSize(); i++)
// 		{
// 			BOOL bFind = FALSE;
// 			for (int j=0; j<arrGroup.GetSize(); j++)
// 			{
// 				if (arrGroup.GetAt(j).CompareNoCase(m_arrPLayDefines.GetAt(i)->GetGroupName()) == 0)
// 				{
// 					bFind = TRUE;
// 					break;
// 				}
// 			}
// 			if (!bFind)
// 			{
// 				arrGroup.Add(m_arrPLayDefines.GetAt(i)->GetGroupName());
// 			}
// 		}

		for (i=0; i<m_strSortedGroupName.GetSize(); i++)
		{
			for (int j=0; j<m_arrPLayDefines.GetSize(); j++)
			{
				CString strGroupName = m_arrPLayDefines.GetAt(j)->GetGroupName();
				if (strGroupName.CompareNoCase(m_strSortedGroupName.GetAt(i)) == 0)
				{
					CString strDoc;
					m_arrPLayDefines.GetAt(j)->WriteTo(strDoc);
					xmlFile.AddSubDoc(strDoc);
				}				
				
			}

			if (m_strSortedGroupName.GetAt(i).CompareNoCase(StrFromResID(IDS_SPECIALGROUP)) == 0)
			{
				for (int j=0; j<m_arrSpecialGroup.GetSize(); j++)
				{
					CString strGroupName = m_arrSpecialGroup.GetAt(j)->GetGroupName();
					if (strGroupName.CompareNoCase(m_strSortedGroupName.GetAt(i)) == 0)
					{
						CString strDoc;
						m_arrSpecialGroup.GetAt(j)->WriteTo(strDoc);
						xmlFile.AddSubDoc(strDoc);
					}				
					
				}
			}
			
		}
		

		//////////////////////////////////////////////////////////////////////////
/*		for (int i=0; i<m_arrPLayDefines.GetSize(); i++)
		{
			CString strDoc;
			m_arrPLayDefines.GetAt(i)->WriteTo(strDoc);
			xmlFile.AddSubDoc(strDoc);
			
		}
*/
		if ( xmlFile.AddElem(XML_RECENT) )
		{
			xmlFile.IntoElem();
			for (int i=0; i<m_UserRecent.m_aIdx.GetSize(); i++)
			{
				IDX idx = m_UserRecent.m_aIdx.GetAt(i);
				CString data;
				data.Format(_T("%I64d %s"), idx.code, idx.FeatureName);
				xmlFile.AddElem(XMLTAG_LAYER, data);
			}
			xmlFile.OutOfElem();
		}
		
		xmlFile.OutOfElem();
	}
	
	strXml = xmlFile.GetDoc();
	return TRUE;
}

BOOL CScheme::ReadFrom(CString &strXml)
{
	Clear();
	
	CMarkup xmlFile;
	xmlFile.SetDoc(strXml);
	CString data;
	if( xmlFile.FindElem(XML_SCHEME) )
	{
		xmlFile.IntoElem();

		if ( xmlFile.FindElem(XML_SORTEDGROUPNAME) )
		{
			CString data = xmlFile.GetData();
			//去掉空格
			data.TrimLeft(_T(" "));
			data.TrimRight(_T(" "));
			//data.Replace(" ",NULL);
			
			if (!data.IsEmpty())
			{
				int  nIndex = 0;
				while (nIndex < data.GetLength())
				{
					int nfirst = nIndex;
					nIndex = data.Find(_T(','), nfirst);
					//仅一个和最一个的情况
					if (nIndex < 0)
					{
						nIndex = data.GetLength();
					}
					
					CString str = data.Mid(nfirst, nIndex-nfirst);
					m_strSortedGroupName.Add(str);
					
					nIndex++;
				}
				
			}
		}

		while( xmlFile.FindElem(XML_LAYER_DEFINE) )
		{
			CString strDoc = xmlFile.GetSubDoc();
			CSchemeLayerDefine *pLayerDef = new CSchemeLayerDefine;
			pLayerDef->ReadFrom(strDoc);
			// 过滤层名相同的层
			BOOL bExist = FALSE;
			for (int i=0; i<m_arrPLayDefines.GetSize(); i++)
			{
				CString strName = m_arrPLayDefines.GetAt(i)->GetLayerName();
				if (pLayerDef->GetLayerName().CompareNoCase(strName) == 0)
				{
					bExist = TRUE;
					break;
				}
			}

			if (!bExist)
			{
				for (int i=0; i<m_arrSpecialGroup.GetSize(); i++)
				{
					CString strName = m_arrSpecialGroup.GetAt(i)->GetLayerName();
					if (pLayerDef->GetLayerName().CompareNoCase(strName) == 0)
					{
						bExist = TRUE;
						break;
					}
				}
			}

			if (bExist) 
			{
				delete pLayerDef;
				pLayerDef = NULL;
				continue;
			}

			if (pLayerDef->GetGroupName().CompareNoCase(StrFromResID(IDS_SPECIALGROUP)) == 0)
			{
				m_arrSpecialGroup.Add(pLayerDef);
			}
			else
			{
				m_arrPLayDefines.Add(pLayerDef);
			}

		}

		// 若没有记录此项，就是加项层的先后顺序
		if (m_strSortedGroupName.GetSize() == 0)
		{
			for(int i=0; i<GetLayerDefineCount(); i++)
			{
				CSchemeLayerDefine *layer = GetLayerDefine(i);
				
				for (int j=0; j<m_strSortedGroupName.GetSize(); j++)
				{
					if (layer->GetGroupName().CompareNoCase(m_strSortedGroupName.GetAt(j)) == 0)
					{
						break;
					}
				}
				// 过滤掉老版本的Recent数据
				if (j > m_strSortedGroupName.GetSize()-1 && layer->GetGroupName().CompareNoCase(_T("Recent")) != 0)
				{
					m_strSortedGroupName.Add(layer->GetGroupName());
				}
			}
		}

		for (int i=0; i<m_strSortedGroupName.GetSize(); i++)
		{
			if (m_strSortedGroupName.GetAt(i).CompareNoCase(StrFromResID(IDS_SPECIALGROUP)) == 0)
			{
				break;
			}
		}

		if (i > m_strSortedGroupName.GetSize()-1)
		{
			m_strSortedGroupName.Add(StrFromResID(IDS_SPECIALGROUP));
		}

		if ( xmlFile.FindElem(XML_RECENT) )
		{
			xmlFile.IntoElem();
			int sum = m_nMaxRecentNum;
			while ( xmlFile.FindElem(XMLTAG_LAYER) && sum--)
			{
				__int64 code;
				TCHAR FeatureName[_MAX_FNAME];
				CString data = xmlFile.GetData();
				if( _stscanf(data,_T("%I64d %s"),&code,FeatureName)==2 )
				{
					IDX idx;
					idx.code = code;
					_tcscpy(idx.FeatureName,FeatureName);
					m_UserRecent.m_aIdx.Add(idx);
				}
				
			}
			xmlFile.OutOfElem();
		}

		xmlFile.OutOfElem();
	}
	
	return TRUE;
}

BOOL CScheme::Load(LPCTSTR strPath)
{
	Clear();

	SetPath(strPath);

	CMarkup rfile;
	rfile.Load(m_strSchemePath);
	CString strDoc = rfile.GetDoc();

	if( ReadFrom(strDoc) )
	{
		LoadCADSymols();
		return TRUE;
	}

	return FALSE;
}

BOOL CScheme::Save(LPCTSTR strPath)const
{
	CString strSchemePath = m_strSchemePath;
	if (strPath != NULL)
	{
		strSchemePath = strPath;
	}
	
	CString strDoc;
	WriteTo(strDoc);
	CMarkup xmlFile;
	xmlFile.SetDoc(strDoc);
	return xmlFile.Save(strSchemePath);

}

void CScheme::ClearCADSymbols()
{
	for( int i=m_arrSpecialGroup.GetSize()-1; i>=0; i--)
	{
		CSchemeLayerDefine *pLayDef = m_arrSpecialGroup[i];
		if( m_pRecentLD==pLayDef )
			m_pRecentLD = NULL;

		if( pLayDef->IsFromCADSymbol() )
		{
			delete pLayDef;
		}
		m_arrSpecialGroup.RemoveAt(i);
	}
}


void CScheme::CreateCADSymbols()
{	
	CCADLinFile *pLinFile = m_pCADSymbolLib->GetLin();
	CCADPatFile *pPatFile = m_pCADSymbolLib->GetPat();
	for( int i=0; i<pLinFile->GetCount(); i++)
	{
		CCADSymbol *pSym = (CCADSymbol*)GCreateSymbol(SYMTYPE_CAD);
		
		pSym->SetName((LPCTSTR)pLinFile->GetItem(i)->name);
		pSym->SetCADSymType(CCADSymbol::typeLin);
		pSym->SetCADSymLib(m_pCADSymbolLib);
		
		CSchemeLayerDefine *pLayDef = new CSchemeLayerDefine();
		pLayDef->CreateFromCADSymbol(pSym);
		
		m_arrSpecialGroup.Add(pLayDef);
	}
	
	for( i=0; i<pPatFile->GetCount(); i++)
	{
		CCADSymbol *pSym = (CCADSymbol*)GCreateSymbol(SYMTYPE_CAD);
		
		pSym->SetName((LPCTSTR)pPatFile->GetItem(i)->name);
		pSym->SetCADSymType(CCADSymbol::typeHat);
		pSym->SetCADSymLib(m_pCADSymbolLib);
		
		CSchemeLayerDefine *pLayDef = new CSchemeLayerDefine();
		pLayDef->CreateFromCADSymbol(pSym);
		
		m_arrSpecialGroup.Add(pLayDef);
	}
}


CString CScheme::GetCADSymbolPath()
{
	CString path = m_strSchemePath;
	int pos = path.ReverseFind('\\');
	if( pos>=0 )
	{
		path = path.Left(pos+1);
	}
	return path;
}

BOOL CScheme::LoadCADSymols()
{
	if( m_pCADSymbolLib )
	{
		delete m_pCADSymbolLib;
	}

	m_pCADSymbolLib = new CCADSymbolLib();
	m_pCADSymbolLib->Load(GetCADSymbolPath());

	ClearCADSymbols();
	CreateCADSymbols();

	return TRUE;
}


BOOL CScheme::ImportCADSymbols(LPCTSTR strPath)
{
	if( !m_pCADSymbolLib )
	{
		m_pCADSymbolLib = new CCADSymbolLib();
	}
	
	if( !m_pCADSymbolLib->Import(strPath) )
		return FALSE;

	ClearCADSymbols();
	CreateCADSymbols();

	return TRUE;
}


void CScheme::SaveCADSymbols()
{
	if( m_pCADSymbolLib )
	{
		m_pCADSymbolLib->Save(GetCADSymbolPath());
	}
}


void CScheme::DeleteCADSymbols()
{
	if( m_pCADSymbolLib )
	{
		delete m_pCADSymbolLib;
	}
	m_pCADSymbolLib = NULL;
	ClearCADSymbols();
	DeleteCADSymbolFile(GetCADSymbolPath());
}


//DEL BOOL  CScheme::SaveToFile(LPCTSTR strPath)
//DEL {
//DEL 	CString strSchemePath = m_strSchemePath;
//DEL 	if (strPath != NULL)
//DEL 	{
//DEL 		strSchemePath = strPath;
//DEL 	}
//DEL 
//DEL 	CString strDoc;
//DEL 	WriteTo(strDoc);
//DEL 	CMarkup xmlFile;
//DEL 	xmlFile.Save(strSchemePath);
//DEL // 	CTextFileWrite schemefile(strSchemePath, CTextFileBase::UNI16_LE);	
//DEL // 	this->WriteTo(strDoc);
//DEL // 	
//DEL // 	if (schemefile.IsOpen())
//DEL // 	{
//DEL // 		schemefile.SetCodePage(0);
//DEL // 		schemefile<<strDoc;
//DEL // 	}
//DEL // 	schemefile.Close();
//DEL 
//DEL 	return TRUE;
//DEL }

BOOL CScheme::AddLayerDefine(CSchemeLayerDefine *pItem, BOOL bSpecial)
{
	if(pItem == NULL)
		return FALSE;
	else
	{
		if (bSpecial)
		{
			m_arrSpecialGroup.Add(pItem);
		}
		else
		{
			m_arrPLayDefines.Add(pItem);
		}
		return TRUE;
	}
}

BOOL CScheme::InsertLayerDefineAt(int nIndex, CSchemeLayerDefine *pItem, BOOL bSpecial)
{
	if (nIndex<0 || pItem==NULL)
		return FALSE;
	else
	{
		if (bSpecial)
		{
			m_arrSpecialGroup.InsertAt(nIndex, pItem);
		}
		else
		{
			m_arrPLayDefines.InsertAt(nIndex, pItem);
		}
		
		return TRUE;
	}
}

BOOL CScheme::AddToRecent(__int64 code, CString strLayerName)
{
	for (int i=0; i<m_UserRecent.m_aIdx.GetSize(); i++)
	{
		if (_tcsicmp(m_UserRecent.m_aIdx.GetAt(i).FeatureName, strLayerName) == 0)
		{
			m_UserRecent.m_aIdx.RemoveAt(i);
			break;
		}
	}
	IDX idx;
	idx.code = code;
	_tcscpy(idx.FeatureName,strLayerName);
	m_UserRecent.m_aIdx.Add(idx);

	// 超过m_nMaxRecentNum删除 
	int size = m_UserRecent.m_aIdx.GetSize();
	if (size > m_nMaxRecentNum)
	{
		m_UserRecent.m_aIdx.RemoveAt(0,size-m_nMaxRecentNum);
	}
	return TRUE;
}

void CScheme::SetMaxRecentNum(int num)
{
	m_nMaxRecentNum = num;
}


int  CScheme::GetMaxRecentNum()
{
	return m_nMaxRecentNum;
}

USERIDX* CScheme::GetRecentIdx()
{
	return &m_UserRecent;
}

BOOL CScheme::DelLayerDefine(int idx, BOOL freeMemory, BOOL bSpecial)
{
	m_pRecentLD = NULL;

	if (bSpecial)
	{
		if(idx < 0 || idx > m_arrSpecialGroup.GetSize()-1)
			return FALSE;
		else
		{
			CSchemeLayerDefine* &player = m_arrSpecialGroup.ElementAt(idx);
			if (freeMemory && player)
			{
				delete player;
				player = NULL;
			}
			m_arrSpecialGroup.RemoveAt(idx);
			return TRUE;
		}
	}
	else
	{
		if(idx < 0 || idx > m_arrPLayDefines.GetSize()-1)
			return FALSE;
		else
		{
			CSchemeLayerDefine* &player = m_arrPLayDefines.ElementAt(idx);
			if (freeMemory && player)
			{
				delete player;
				player = NULL;
			}
			m_arrPLayDefines.RemoveAt(idx);
			return TRUE;
		}
	}
	
}

int CScheme::GetLayerDefineCount(BOOL bSpecial)const
{
	if (bSpecial)
	{
		return m_arrSpecialGroup.GetSize();
	}
	else
	{
		return m_arrPLayDefines.GetSize();
	}
}

CSchemeLayerDefine::CSchemeLayerDefine()
{
	m_nGeoClass = CLS_GEOPOINT;
	m_nDbGeoClass = CLS_GEOPOINT;
	m_nColor = RGB(255,255,255);
	m_nCode = 0;

	m_bFromCADSymbol = FALSE;
}

CSchemeLayerDefine::~CSchemeLayerDefine()
{
	Clear();	
}

void CSchemeLayerDefine::Clear()
{
	for(int i=0; i<m_arrPCfgs.GetSize(); i++)
	{
		if(m_arrPCfgs.GetAt(i) != NULL)
		{
			delete m_arrPCfgs.GetAt(i);
		}
	}
	m_arrPCfgs.RemoveAll();
	m_arrXDefines.RemoveAll();
	m_defValueTable.DelAll();

}

CAnnotation* CSchemeLayerDefine::GetHeightAnnotation()
{
	BOOL bHeight = FALSE;
	CAnnotation *pAnno = NULL;
	for (int i=0; i<GetSymbolCount(); i++)
	{
		CSymbol *pSymbol = GetSymbol(i);
		if (pSymbol && pSymbol->GetType()==SYMTYPE_ANNOTATION)
		{
			CAnnotation *pAnntmp = (CAnnotation*)pSymbol;
			if (pAnntmp->m_nAnnoType == CAnnotation::Height)
			{
				bHeight = TRUE;
				pAnno = pAnntmp;
				break;
			}
			
		}
		
	}
	return pAnno;
}

BOOL CSchemeLayerDefine::IsSupportGeoClass(int ncls)
{
	if (m_strSupportClsName.GetSize() < 1)
		return TRUE;

	BOOL bSupport = FALSE;
	int i;
	CString str;
	switch(ncls)
	{
	case CLS_GEOPOINT:
		str.LoadString(IDS_POINT);
		break;
	case CLS_GEODIRPOINT:
		str.LoadString(IDS_DIRPOINT);
		break;
	case CLS_GEOCURVE:
		str.LoadString(IDS_LINE);
		break;
	case CLS_GEODCURVE:
		str.LoadString(IDS_GEO_DCURVE);
		break;
	case CLS_GEOPARALLEL:
		str.LoadString(IDS_PARLINE);
		break;
	case CLS_GEOSURFACE:
		str.LoadString(IDS_SURFACE);
		break;
	case CLS_GEOTEXT:
		str.LoadString(IDS_TEXT);	
		break;
	default:
		str.Empty();
	}

	for (i=0;i<m_strSupportClsName.GetSize(); i++)
	{
		if (str.CompareNoCase(m_strSupportClsName.GetAt(i)) == 0)
		{
			bSupport = TRUE;
			break;
		}
		
	}
	return bSupport;
		
}

int CScheme::GetLayerDefineIndex(LPCTSTR layname, BOOL bSpecial)
{
	if (bSpecial)
	{
		for(int i=0; i<m_arrSpecialGroup.GetSize(); i++)
		{
			if(_tcsicmp(layname,m_arrSpecialGroup[i]->GetLayerName()) == 0)
			{
				return i;
			}
		}
	}
	else
	{
		for(int i=0; i<m_arrPLayDefines.GetSize(); i++)
		{
			if(_tcsicmp(layname,m_arrPLayDefines[i]->GetLayerName()) == 0)
			{
				return i;
			}
		}

	}

	return -1;
}

int CScheme::GetFirstLayerDefineIndex(LPCTSTR strGroupName, BOOL bSpecial)
{
	if (bSpecial)
	{
		for (int i=0; i<m_arrSpecialGroup.GetSize();i++)
		{
			if (_tcsicmp(strGroupName,m_arrSpecialGroup[i]->GetGroupName())==0)
			{
				return i;
			}
			
		}
	}
	else
	{
		for (int i=0; i<m_arrPLayDefines.GetSize();i++)
		{
			if (_tcsicmp(strGroupName,m_arrPLayDefines[i]->GetGroupName())==0)
			{
				return i;
			}
			
		}

	}
	return -1;
}

CSchemeLayerDefine* CScheme::GetLayerDefine(int idx, BOOL bSpecial)
{
	if (bSpecial)
	{
		if(idx < 0 || idx > m_arrSpecialGroup.GetSize()-1)
			return NULL;
		else
		{
			return m_arrSpecialGroup.GetAt(idx);		
		}
	}
	else
	{
		if(idx < 0 || idx > m_arrPLayDefines.GetSize()-1)
			return NULL;
		else
		{
			return m_arrPLayDefines.GetAt(idx);		
		}
	}

}

BOOL CScheme::GetLayerDefinePreviewGrBuffer(LPCTSTR layname, GrBuffer *pBuf, BOOL bSpecial)
{
	if (layname == NULL) return 0.0;
	
	__int64 code = _ttoi64(layname);
	if( code>0 )
	{
		CString name = layname;
		FindLayerIdx(TRUE,code,name);
	}

	CSchemeLayerDefine *pSchemeLayer = GetLayerDefine(layname, bSpecial);
	if (!pSchemeLayer) return FALSE;

	int nSymbolNum = pSchemeLayer->GetSymbolCount();

	GrBuffer bufs;

	CFeature *pFeature = pSchemeLayer->CreateDefaultFeature();
	if (!pFeature)  return FALSE;

	CGeometry *pGeo = pFeature->GetGeometry();

	int nGeoClass = pGeo->GetClassType();

	if (nGeoClass==CLS_GEOPOINT || nGeoClass==CLS_GEODIRPOINT)
	{
		pGeo->CreateShape(&PT_3DEX(0,0,0,penMove),1);
		pGeo->Draw(&bufs);
	}
	else if (nGeoClass == CLS_GEOPARALLEL)
	{
		// 平行线有填充时填充在基线和辅助线之间
		PT_3DEX pts[2];
		pts[0].x = 0;
		pts[0].y = 10;
		pts[0].pencode = penLine;
		pts[1].x = 20;
		pts[1].y = 10;
		pts[1].pencode = penLine;
		pGeo->CreateShape(pts,2);

		CGeoParallel *pParallele = (CGeoParallel*)pGeo;
		pParallele->SetWidth(-10);

	}
	else if(nGeoClass == CLS_GEOCURVE)
	{
		// 检查是否有颜色，图元填充，晕线填充,有则添加四个点方便填充
		BOOL bHatch = FALSE;
		// 检查是否有依比例转盘，装卸漏斗（圆形）
		BOOL bScaleTurnPlate = FALSE, bScaleFunnelHoleRound = FALSE;
		for(int i=0; i<nSymbolNum; i++)
		{
			CSymbol *pSym = pSchemeLayer->GetSymbol(i);
			int nType = pSym->GetType();
			if (nType == SYMTYPE_SCALETURNPLATETYPE)
			{
				bScaleTurnPlate = TRUE;
			}
			else if (nType == SYMTYPE_SCALEFUNNELTYPE && ((CScaleFunneltype*)pSym)->m_nFunnelType == 1)
			{
				bScaleFunnelHoleRound = TRUE;
			}

			if(nType==SYMTYPE_CELLHATCH || nType==SYMTYPE_COLORHATCH || nType==SYMTYPE_LINEHATCH || nType == SYMTYPE_DIAGONAL)
			{
				bHatch = TRUE;
				break;
			}
		}

		if (bScaleTurnPlate || bScaleFunnelHoleRound)
		{
			PT_3DEX pts[4];
			pts[0].x = 0;
			pts[0].y = 10;
			pts[0].pencode = penArc;
			pts[1].x = 10;
			pts[1].y = 20;
			pts[1].pencode = penArc;
			pts[2].x = 20;
			pts[2].y = 10;
			pts[2].pencode = penArc;
			pts[3].x = 0;
			pts[3].y = 10;
			pts[3].pencode = penArc;
			pGeo->CreateShape(pts,4);
		}
		else if (bHatch)
		{
			PT_3DEX pts[5];
			pts[0].x = 0;
			pts[0].y = 0;
			pts[0].pencode = penLine;
			pts[1].x = 40;
			pts[1].y = 0;
			pts[1].pencode = penLine;
			pts[2].x = 40;
			pts[2].y = 40;
			pts[2].pencode = penLine;
			pts[3].x = 0;
			pts[3].y = 40;
			pts[3].pencode = penLine;
 			pts[4].x = 0;
 			pts[4].y = 0;
 			pts[4].pencode = penLine;
			pGeo->CreateShape(pts,5);

		}
		else
		{
			PT_3DEX pts[2];
			pts[0].x = 0;
			pts[0].y = 0;
			pts[0].pencode = penLine;
			pts[1].x = 20;
			pts[1].y = 0;
			pts[1].pencode = penLine;
			pGeo->CreateShape(pts,2);

		}	
		
		
	}
	else if (nGeoClass == CLS_GEODCURVE)
	{
		PT_3DEX pts[5];
		pts[0].x = 0;
		pts[0].y = 10;
		pts[0].pencode = penMove;
		pts[1].x = 20;
		pts[1].y = 10;
		pts[1].pencode = penLine;
		pts[2].x = 0;
		pts[2].y = 0;
		pts[2].pencode = penMove;
		pts[3].x = 20;
		pts[3].y = 0;
		pts[3].pencode = penLine;
		pGeo->CreateShape(pts,4);
	}
	else if(nGeoClass == CLS_GEOSURFACE)
	{
		// 检查是否有依比例装卸漏斗（圆形）
		BOOL bScaleFunnelHoleRound = FALSE;
		for(int i=0; i<nSymbolNum; i++)
		{
			CSymbol *pSym = pSchemeLayer->GetSymbol(i);
			int nType = pSym->GetType();
			if (nType == SYMTYPE_SCALEFUNNELTYPE && ((CScaleFunneltype*)pSym)->m_nFunnelType == 1)
			{
				bScaleFunnelHoleRound = TRUE;
			}
		}

		if (bScaleFunnelHoleRound)
		{
			PT_3DEX pts[4];
			pts[0].x = 0;
			pts[0].y = 10;
			pts[0].pencode = penArc;
			pts[1].x = 10;
			pts[1].y = 20;
			pts[1].pencode = penArc;
			pts[2].x = 20;
			pts[2].y = 10;
			pts[2].pencode = penArc;
			pts[3].x = 0;
			pts[3].y = 10;
			pts[3].pencode = penArc;
			pGeo->CreateShape(pts,4);
		}
		else
		{
			PT_3DEX pts[5];
			pts[0].x = 0;
			pts[0].y = 0;
			pts[0].pencode = penLine;
			pts[1].x = 40;
			pts[1].y = 0;
			pts[1].pencode = penLine;
			pts[2].x = 40;
			pts[2].y = 40;
			pts[2].pencode = penLine;
			pts[3].x = 0;
			pts[3].y = 40;
			pts[3].pencode = penLine;
			pts[4].x = 0;
			pts[4].y = 0;
			pts[4].pencode = penLine;
			pGeo->CreateShape(pts,5);
		}

		

	}
	else if (nGeoClass == CLS_GEOTEXT)
	{
		pGeo->CreateShape(&PT_3DEX(0,0,0,penMove),1);
		((CGeoText*)pGeo)->SetText(_T("abc"));
	}

	for(int i=0; i<nSymbolNum; i++)
	{
		CSymbol *pSym = pSchemeLayer->GetSymbol(i);
		// 若为注记
		if (pSym->GetType() == SYMTYPE_ANNOTATION)
		{
			CValueTable tab;
			pSchemeLayer->GetBasicAttributeDefaultValues(tab);
			pSchemeLayer->GetXDefaultValues(tab);
			((CAnnotation*)pSym)->Draw(pFeature,&bufs,tab);
		}
		else
		{
			// 图元填充过密预览调整
			if (pSym->GetType() == SYMTYPE_CELLHATCH)
			{
				Envelope e = pFeature->GetGeometry()->GetEnvelope();
				CCellHatch *pCellHath = (CCellHatch*)pSym;
				if ((fabs(pCellHath->m_fdx) > 1e-4 && e.Width()/pCellHath->m_fdx > 80) && 
					(fabs(pCellHath->m_fdy) > 1e-4 && e.Height()/pCellHath->m_fdx > 80))
				{
					CCellHatch hatch;
					hatch.CopyFrom(pSym);
					hatch.m_fdx = e.Width()/80;
					hatch.m_fdy = e.Height()/80;
					hatch.Draw(pFeature,&bufs);
					continue;
				}
			}
			
			pSym->Draw(pFeature,&bufs);
		}

	}

	if (nSymbolNum == 0)
	{
		pFeature->Draw(&bufs);
	}

	delete pFeature;

	pBuf->AddBuffer(&bufs);

	return TRUE;
}

BOOL CScheme::FindLayerIdx(BOOL byCode, __int64 &code, CString& name)
{
	if( m_pRecentLD )
	{
		if( !byCode )
		{
			if( m_pRecentLD->GetLayerName().CompareNoCase(name)==0 )
			{
				code = m_pRecentLD->GetLayerCode();
				return TRUE;
			}
		}
		else
		{
			if( m_pRecentLD->GetLayerCode()==code )
			{
				name = m_pRecentLD->GetLayerName();
				return TRUE;
			}
		}
	}

	if(byCode == TRUE)
	{
		for(int i=0; i<m_arrPLayDefines.GetSize(); i++)
		{
			if(code == m_arrPLayDefines[i]->GetLayerCode())
			{
				m_pRecentLD = m_arrPLayDefines[i];
				name = m_arrPLayDefines[i]->GetLayerName();
				return TRUE;
			}
		}

	}
	else
	{
		for(int i=0; i<m_arrPLayDefines.GetSize(); i++)
		{
			if(_tcsicmp(name,m_arrPLayDefines[i]->GetLayerName()) == 0)
			{
				m_pRecentLD = m_arrPLayDefines[i];
				code = m_arrPLayDefines[i]->GetLayerCode();
				return TRUE;
			}
		}
	}
	
	return FALSE;
}



CSchemeLayerDefine *CScheme::GetLayerDefine(LPCTSTR layname, BOOL byCode, __int64 code, BOOL bSpecial)
{
	if( m_pRecentLD )
	{
		if( !byCode )
		{
			if( m_pRecentLD->GetLayerName().CompareNoCase(layname)==0 )
				return m_pRecentLD;
		}
		else
		{
			if( m_pRecentLD->GetLayerCode()==code )
				return m_pRecentLD;
		}
	}
	if (bSpecial)
	{
		for(int i=0; i<m_arrSpecialGroup.GetSize(); i++)
		{
			if(m_arrSpecialGroup.GetAt(i)->GetLayerName().CompareNoCase(layname) == 0)
			{
				m_pRecentLD = m_arrSpecialGroup.GetAt(i);
				return m_arrSpecialGroup.GetAt(i);
			}
		}
	}

	if (!byCode)
	{
		for(int i=0; i<m_arrPLayDefines.GetSize(); i++)
		{
			if(m_arrPLayDefines.GetAt(i)->GetLayerName().CompareNoCase(layname) == 0)
			{
				m_pRecentLD = m_arrPLayDefines.GetAt(i);
				return m_arrPLayDefines.GetAt(i);
			}
		}
	}
	else
	{
		for(int i=0; i<m_arrPLayDefines.GetSize(); i++)
		{
			if(m_arrPLayDefines.GetAt(i)->GetLayerCode() == code)
			{
				m_pRecentLD = m_arrPLayDefines.GetAt(i);
				return m_arrPLayDefines.GetAt(i);
			}
		}
	}
	
	return NULL;
}

BOOL CSchemeLayerDefine::GetSupportClsName(CStringArray &name)
{
	name.RemoveAll();
	name.Copy(m_strSupportClsName);
	return TRUE;
}

void CSchemeLayerDefine::SetSupportClsName(const CStringArray &name)
{
	m_strSupportClsName.RemoveAll();
	if (name.GetSize() < 1)
	{		
		m_strSupportClsName.Add(StrFromResID(IDS_POINT));
		m_strSupportClsName.Add(StrFromResID(IDS_DIRPOINT));
		m_strSupportClsName.Add(StrFromResID(IDS_LINE));
		m_strSupportClsName.Add(StrFromResID(IDS_GEO_DCURVE));
		m_strSupportClsName.Add(StrFromResID(IDS_PARLINE));
		m_strSupportClsName.Add(StrFromResID(IDS_SURFACE));
		m_strSupportClsName.Add(StrFromResID(IDS_TEXT));		
	}
	else
	{
		m_strSupportClsName.Copy(name);
	}

	
	
}

const XDefine* CSchemeLayerDefine::GetXDefines(int &size)
{
	size = m_arrXDefines.GetSize();
	if(size > 0)
		return m_arrXDefines.GetData();
	else
		return NULL;
}

BOOL CSchemeLayerDefine::AddXDefine(XDefine item)
{
	m_arrXDefines.Add(item);
	return TRUE;
}

BOOL CSchemeLayerDefine::DelXDefine(int idx)
{
	if(idx > m_arrXDefines.GetSize()-1 || idx < 0)
		return FALSE;
	else
	{
		m_arrXDefines.RemoveAt(idx);
		return TRUE;
	}
}

XDefine CSchemeLayerDefine::GetXDefine(int idx)
{
	if(idx > m_arrXDefines.GetSize()-1 || idx < 0)
		return XDefine();
	else
	{
		return m_arrXDefines.GetAt(idx);
	}

}


CFeature* CSchemeLayerDefine::CreateDefaultFeature(int cls)
{
	int nGeoClass = cls;

	if (cls == CLS_NULL)
		nGeoClass = GetGeoClass();	

	CGeometry *pGeo = (CGeometry *)CPermanent::CreatePermanentObject(nGeoClass);/*GCreateGeometry(nGeoClass)*/;
	if (!pGeo) return NULL;

	CFeature *pFt = new CFeature;
	if (!pFt)
	{
		delete pGeo;
		return NULL;
	}

	pFt->SetGeometry(pGeo);

	CValueTable tab;
	if( GetDefaultValues(tab) )
	{
		pFt->ReadFrom(tab);
	}

	if( tab.FindField(FIELDNAME_GEOCURVE_LINEWIDTH)<0 )
	{
		tab.DelAll();
		float wid = GetDefaultLineWidth();
		tab.BeginAddValueItem();
		tab.AddValue(FIELDNAME_GEOCURVE_LINEWIDTH,&CVariantEx(wid),IDS_FIELDNAME_GEOWIDTH);
		tab.EndAddValueItem();
		
		pFt->ReadFrom(tab);
	}

	return pFt;
}

BOOL CSchemeLayerDefine::SetXDefine(int idx, XDefine item)
{
	if(idx > m_arrXDefines.GetSize()-1 || idx < 0)
		return FALSE;
	else
	{
		m_arrXDefines.SetAt(idx,item);
		return TRUE;
	}

}

int CSchemeLayerDefine::FindXDefine(LPCTSTR field)
{
	for(int i=0; i<m_arrXDefines.GetSize(); i++)
	{
		if(_tcsicmp(m_arrXDefines.GetAt(i).field,field) == 0)
			return i;
	}
	return -1;
}


int CSchemeLayerDefine::FindXDefineByName(LPCTSTR name)
{
	for(int i=0; i<m_arrXDefines.GetSize(); i++)
	{
		if(_tcsicmp(m_arrXDefines.GetAt(i).name,name) == 0)
			return i;
	}
	return -1;
}

BOOL CSchemeLayerDefine::AddSymbol(CSymbol* pItem)
{
	if(pItem == NULL)
		return FALSE;
	else
	{
		m_arrPCfgs.Add(pItem);
		return TRUE;
	}
		

}

BOOL CSchemeLayerDefine::GetDefaultValues(CValueTable& tab)
{
	if (m_defValueTable.GetItemCount() == 0) return FALSE;

	tab.CopyFrom(m_defValueTable);
	return TRUE;	
}

BOOL CSchemeLayerDefine::SetDefaultValues(CValueTable& tab)
{
	m_defValueTable.CopyFrom(tab);
	return TRUE;
}

CString CSchemeLayerDefine::GetXClassName()
{
	return CString();
}

void CSchemeLayerDefine::SetXClassName(LPCTSTR name)
{
	return;
}

BOOL CSchemeLayerDefine::GetXDefaultValues(CValueTable& tab)
{
	for (int i=0; i<m_arrXDefines.GetSize(); i++)
	{
		XDefine xdef = m_arrXDefines.GetAt(i);
		_variant_t var;
		switch(xdef.valuetype)
		{
		case DP_CFT_BOOL: 
			var = (bool)_ttoi(xdef.defvalue);
			tab.AddValue(xdef.field,&(CVariantEx)var,xdef.name);
			break;
		case DP_CFT_COLOR:
			var = (long)_ttoi(xdef.defvalue);
			tab.AddValue(xdef.field,&(CVariantEx)var,xdef.name);
			break;
		case DP_CFT_SMALLINT:
			var = (short)_ttoi(xdef.defvalue);
			tab.AddValue(xdef.field,&(CVariantEx)var,xdef.name);
			break;
		case DP_CFT_VARCHAR:
			var = xdef.defvalue;
			tab.AddValue(xdef.field,&(CVariantEx)var,xdef.name);
			break;
		case DP_CFT_FLOAT:
			var = (float)_ttof(xdef.defvalue);
			tab.AddValue(xdef.field,&(CVariantEx)var,xdef.name);
			break;
		case DP_CFT_DOUBLE:
			var = (double)_ttof(xdef.defvalue);
			tab.AddValue(xdef.field,&(CVariantEx)var,xdef.name);
			break;
		case DP_CFT_INTEGER:
			var = (long)_ttoi(xdef.defvalue);
			tab.AddValue(xdef.field,&(CVariantEx)var,xdef.name);
			break;
		default:
			break;
		}

	}
	return TRUE;
	
}

float CSchemeLayerDefine::GetDefaultLineWidth()
{
	// -1为复杂线型（多种线型，线宽不一样）
	float lineWid = -1;
	int i, nSymCount = GetSymbolCount();
	for (i=0; i<nSymCount; i++)
	{
		CSymbol *pSym = GetSymbol(i);
		if (pSym)
		{
			int symType = pSym->GetType();
			float symWid = -1;
			if (symType == SYMTYPE_DASHLINETYPE)
			{
				symWid = ((CDashLinetype*)pSym)->m_fWidth;
			}
			else if (symType == SYMTYPE_SCALELINETYPE)
			{
				symWid = ((CScaleLinetype*)pSym)->m_lfWidth;
			}
			else if (symType == SYMTYPE_LINEHATCH)
			{
				symWid = ((CLineHatch*)pSym)->m_fWidth;
			}
			else if (symType == SYMTYPE_DIAGONAL)
			{
				symWid = ((CDiagonal*)pSym)->m_fWidth;
			}
			else if (symType == SYMTYPE_PARATYPE)
			{
				symWid = ((CParaLinetype*)pSym)->m_fWidth;
			}
			else if (symType == SYMTYPE_ANGBISECTORTYPE)
			{
				symWid = ((CAngBisectortype*)pSym)->m_fWidth;
			}
			else if (symType == SYMTYPE_SCALEARCTYPE)
			{
				symWid = ((CScaleArctype*)pSym)->m_fWidth;
			}
			else if (symType == SYMTYPE_SCALETURNPLATETYPE)
			{
				symWid = ((CScaleTurnplatetype*)pSym)->m_fWidth;
			}
			else if (symType == SYMTYPE_SCALECRANETYPE)
			{
				symWid = ((CScaleCranetype*)pSym)->m_fWidth;
			}
			else if (symType == SYMTYPE_SCALEFUNNELTYPE)
			{
				symWid = ((CScaleFunneltype*)pSym)->m_fWidth;
			}
			
			if (symWid >= 0)
			{
				if (lineWid < 0)
				{
					lineWid = symWid;
				}
				else if (fabs(lineWid-symWid) > 1e-4)
				{
					lineWid = -1;
					break;
				}
				
			}
		}
	}
	
	if (i >= nSymCount && lineWid < 0)
	{
		lineWid = 0;
	}
	
	return lineWid;
}

BOOL CSchemeLayerDefine::GetBasicAttributeDefaultValues(CValueTable& tab)
{
	CFeature *pFtr = NULL;
	if(1)
	{
		int nGeoClass = m_nGeoClass;

		if (nGeoClass == CLS_NULL)
			nGeoClass = GetGeoClass();	

		CGeometry *pGeo = (CGeometry *)CPermanent::CreatePermanentObject(nGeoClass);
		if (!pGeo) return FALSE;

		CFeature *pFt = new CFeature;
		if (!pFt)
		{
			delete pGeo;
			return FALSE;
		}

		pFt->SetGeometry(pGeo);

		pFtr = pFt;
	}
	
	if (pFtr)
	{
		tab.BeginAddValueItem();
		pFtr->WriteTo(tab);
		tab.EndAddValueItem();
		tab.DelField(FIELDNAME_SHAPE);
		tab.DelField(FIELDNAME_FTRID);
		tab.DelField(FIELDNAME_GEOCLASS);
		tab.DelField(FIELDNAME_FTRDISPLAYORDER);
		delete pFtr;
		return TRUE;
	}		
	return FALSE;
}

BOOL CSchemeLayerDefine::SetXDefaultValues(CValueTable& tab)
{
	for (int i=0; i<tab.GetFieldCount(); i++)
	{
		const CVariantEx *var;
		CString field, name, data;
		int type;

		tab.GetField(i,field,type,name);
		if (!tab.GetValue(0,i,var))
			continue;

		XDefine xdef;
		_tcscpy(xdef.field,field);
		_tcscpy(xdef.name,name);
		xdef.valuetype = type;		

		data.Empty();
		switch( var->GetType() ) 
		{			
		case VT_I1:
		case VT_UI1:
			data.Format(_T("%d"),(int)(BYTE)(_variant_t)*var);
			break;
		case VT_I2:
		case VT_UI2:
			data.Format(_T("%d"),(short)(_variant_t)*var);
			break;
		case VT_I4:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
			data.Format(_T("%d"),(long)(_variant_t)*var);
			break;
		case VT_BSTR:
			data.Format(_T("%s"),(LPCTSTR)(_bstr_t)(_variant_t)*var);
			break;
		case VT_BOOL:
			data.Format(_T("%d"),((bool)(_variant_t)*var)?1:0);
			break;
		case VT_R4:
			data.Format(_T("%f"),(float)(_variant_t)*var);
			break;
		case VT_R8:
			data.Format(_T("%.6f"),(double)(_variant_t)*var);
			break;
		default:;
		}
		
		if( !data.IsEmpty() )
		{
			_tcscpy(xdef.defvalue,data);
			xdef.valuelen = data.GetLength();
		}		
					
			
	}
	return TRUE;
	
}

BOOL CSchemeLayerDefine::DelSymbol(int idx)
{
	if(idx > m_arrPCfgs.GetSize()-1 || idx < 0)
		return FALSE;
	else
	{
		if(m_arrPCfgs[idx])
			delete m_arrPCfgs[idx];
		m_arrPCfgs.RemoveAt(idx);
		return TRUE;
	}

}

int CSchemeLayerDefine::GetSymbolCount()
{
	return m_arrPCfgs.GetSize();
}

CSymbol* CSchemeLayerDefine::GetSymbol(int idx)
{
	if(idx > m_arrPCfgs.GetSize()-1 || idx < 0)
		return NULL;
	else
	{
		return m_arrPCfgs.GetAt(idx);
	}
}

void CSchemeLayerDefine::GetSymbols(CArray<CSymbol*,CSymbol*> &sys)
{
	sys.RemoveAll();
	sys.Copy(m_arrPCfgs);
}

void CSchemeLayerDefine::SetSymbols(CArray<CSymbol*,CSymbol*>& sys)
{
	int nCount = sys.GetSize();
	if (nCount<1)
	{
		return;
	}
	m_arrPCfgs.RemoveAll();
	m_arrPCfgs.Copy(sys);
}
void CSchemeLayerDefine::SymbolToTop(int idx)
{
	if(idx > m_arrPCfgs.GetSize()-1 || idx <= 0)
		return;
	else
	{
		CSymbol *pSym = m_arrPCfgs[idx];
		m_arrPCfgs.RemoveAt(idx);
		m_arrPCfgs.InsertAt(0,pSym);
	}
}

void CSchemeLayerDefine::SymbolToTail(int idx)
{
	if(idx >= m_arrPCfgs.GetSize()-1 || idx < 0)
		return;
	else
	{
		CSymbol *pSym = m_arrPCfgs[idx];
		m_arrPCfgs.RemoveAt(idx);
		m_arrPCfgs.Add(pSym);
	}
}

void CSchemeLayerDefine::ReadFrom(CString &xmlString)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(xmlString);
	CString data;
	if( xmlFile.FindElem(XML_LAYER_DEFINE) )
	{
		xmlFile.IntoElem();
		
		if(xmlFile.FindElem(XMLTAG_GROUPNAME))
		{
			data = xmlFile.GetData();
			m_strGroupName = data;
		}

		if(xmlFile.FindElem(XMLTAG_CODE))
		{
			data = xmlFile.GetData();
			m_nCode = _ttoi64(data);
		}

		if( xmlFile.FindElem(XMLTAG_LAYERNAME) )
		{
			data = xmlFile.GetData();
			int nIndex = data.Find('^', 0);
			// 快捷键
			if (nIndex >= 0)
			{
				m_strLayName = data.Mid(0,nIndex);
				m_strAccel = data.Mid(nIndex+1);
			}
			else
				m_strLayName = data;
		}

		if(xmlFile.FindElem(XMLTAG_DEFAULT_GEOCLASS))
		{
			data = xmlFile.GetData();
			m_nGeoClass = _ttoi(data);	
		}

		if(xmlFile.FindElem(XMLTAG_DB_GEOCLASS))
		{
			data = xmlFile.GetData();
			m_nDbGeoClass = _ttoi(data);	
		}
		else
		{
			if (m_nGeoClass == CLS_GEOPOINT || m_nGeoClass == CLS_GEODIRPOINT || 
				m_nGeoClass == CLS_GEODEMPOINT || m_nGeoClass == CLS_GEOMULTIPOINT)
			{
				m_nDbGeoClass = CLS_GEOPOINT;
			}
			else if (m_nGeoClass == CLS_GEOCURVE || m_nGeoClass == CLS_GEODCURVE || 
				m_nGeoClass == CLS_GEOPARALLEL)
			{
				m_nDbGeoClass = CLS_GEOCURVE;
			}
			else if (m_nGeoClass == CLS_GEOSURFACE || m_nGeoClass == CLS_GEOMULTISURFACE)
			{
				m_nDbGeoClass = CLS_GEOSURFACE;
			}
			else if (m_nGeoClass == CLS_GEOTEXT)
			{
				m_nDbGeoClass = CLS_GEOTEXT;
			}
			else
			{
				m_nDbGeoClass = CLS_GEOPOINT;
			}
		}

		if(xmlFile.FindElem(XMLTAG_SUPPORT_GEOCLASS))
		{
			data = xmlFile.GetData();
			//去掉空格
			data.TrimLeft(_T(" "));
			data.TrimRight(_T(" "));
			//data.Replace(" ",NULL);

			if (!data.IsEmpty())
			{
				int  nIndex = 0;
				while (nIndex < data.GetLength())
				{
					int nfirst = nIndex;
					nIndex = data.Find(',', nfirst);
					//仅一个和最一个的情况
					if (nIndex < 0)
					{
						nIndex = data.GetLength();
					}
					
					CString str = data.Mid(nfirst, nIndex-nfirst);
					m_strSupportClsName.Add(str);
					
					nIndex++;
				}
			
			}

			//内容有效性检查
			for (int i=m_strSupportClsName.GetSize()-1; i>=0; i--)
			{
				CString str = m_strSupportClsName.GetAt(i);
				if (str.CompareNoCase(StrFromResID(IDS_POINT))!=0  
					&& str.CompareNoCase(StrFromResID(IDS_DIRPOINT))!=0
					&& str.CompareNoCase(StrFromResID(IDS_LINE))!=0
					&& str.CompareNoCase(StrFromResID(IDS_PARLINE))!=0
					&& str.CompareNoCase(StrFromResID(IDS_SURFACE))!=0
					&& str.CompareNoCase(StrFromResID(IDS_TEXT))!=0
					&& str.CompareNoCase(StrFromResID(IDS_GEO_DCURVE))!=0)
				{
					m_strSupportClsName.RemoveAt(i);
				}
			}

			if (m_strSupportClsName.GetSize() < 1)
			{
				m_strSupportClsName.Add(StrFromResID(IDS_POINT));
				m_strSupportClsName.Add(StrFromResID(IDS_DIRPOINT));
				m_strSupportClsName.Add(StrFromResID(IDS_LINE));
				m_strSupportClsName.Add(StrFromResID(IDS_PARLINE));
				m_strSupportClsName.Add(StrFromResID(IDS_SURFACE));
				m_strSupportClsName.Add(StrFromResID(IDS_TEXT));
				m_strSupportClsName.Add(StrFromResID(IDS_GEO_DCURVE));
			}

		}

		if(xmlFile.FindElem(XMLTAG_COLOR))
		{
			data = xmlFile.GetData();
			m_nColor = _ttoi(data);
			
		}

		if(xmlFile.FindElem(XMLTAG_DBLAYER))
		{
			m_strDBLayName = xmlFile.GetData();			
		}

		//FixedAttributes
		if( xmlFile.FindElem(XMLTAG_FIXEDATTRIBUTE) )
		{
			CString fixeddoc = xmlFile.GetSubDoc(); 

			CMarkup xml;
			xml.SetDoc(fixeddoc);
			xml.FindElem(NULL);
			xml.IntoElem();
			m_defValueTable.BeginAddValueItem();
			Xml_ReadValueTable(xml,m_defValueTable);
			m_defValueTable.EndAddValueItem();
			xml.OutOfElem();
		}
		
		//XAttributes
		if( xmlFile.FindElem(XMLTAG_XATTRIBUTES) )
		{
			xmlFile.IntoElem();

			while( xmlFile.FindElem(XMLTAG_XATTRIBUTE) )
			{
				xmlFile.IntoElem();

				XDefine xdef;
				memset(&xdef,0,sizeof(xdef));

				if(xmlFile.FindElem(XMLTAG_FIELD))
				{
					data = xmlFile.GetData();
					_tcscpy(xdef.field,data);
				}

				if(xmlFile.FindElem(XMLTAG_NAME))
				{
					data = xmlFile.GetData();
					_tcscpy(xdef.name,data);
				}

				if(xmlFile.FindElem(XMLTAG_VALUE_TYPE))
				{
					data = xmlFile.GetData();
					xdef.valuetype = _ttoi(data);
				}

				if(xmlFile.FindElem(XMLTAG_VALUE_LENGTH))
				{
					data = xmlFile.GetData();
					xdef.valuelen = _ttoi(data);
				}

				if(xmlFile.FindElem(XMLTAG_DEFAULT_VALUE))
				{
					data = xmlFile.GetData();
					_tcscpy(xdef.defvalue,data);
				}

				if(xmlFile.FindElem(XMLTAG_ISMUST))
				{
					data = xmlFile.GetData();
					xdef.isMust = _ttoi(data);
				}

				if(xmlFile.FindElem(XMLTAG_DIGITNUM))
				{
					data = xmlFile.GetData();
					xdef.nDigit = _ttoi(data);		
				}

				if(xmlFile.FindElem(XMLTAG_VALUERANGE))
				{
					data = xmlFile.GetData();
					_tcscpy(xdef.valueRange,data);				
				}

				if (xdef.isMust>=0)
					m_arrXDefines.Add(xdef);

				xmlFile.OutOfElem();

			}


			xmlFile.OutOfElem();
		}
		
		//Symbols
		if( xmlFile.FindElem(XMLTAG_SYMBOLS) )
		{
			xmlFile.IntoElem();
			
			while( xmlFile.FindElem(XMLTAG_SYMBOL) )
			{

				CString strDoc = xmlFile.GetSubDoc();

				xmlFile.IntoElem();
					
				CSymbol *pSy = NULL;
				int nType = -1;
				if( xmlFile.FindElem(XMLTAG_TYPE) )
				{
					data = xmlFile.GetData();
					nType = _ttoi(data);
				}

				pSy = GCreateSymbol(nType);

				if(pSy == NULL)
				{
					xmlFile.OutOfElem();
					continue;
				}
				pSy->Load(strDoc);
				m_arrPCfgs.Add(pSy);

				xmlFile.OutOfElem();
			}

			xmlFile.OutOfElem();
		}

		xmlFile.OutOfElem();
	}

}


void CSchemeLayerDefine::WriteTo(CString &xmlString)
{
	if( m_bFromCADSymbol )
		return;

	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XML_LAYER_DEFINE) )
	{
		xmlFile.IntoElem();
		
		xmlFile.AddElem(XMLTAG_GROUPNAME,m_strGroupName);

		data.Format(_T("%I64d"),m_nCode);
		xmlFile.AddElem(XMLTAG_CODE,data);
		
		data = m_strLayName;
		if (!m_strAccel.IsEmpty())
		{
			data = data + "^" + m_strAccel;
		}
		xmlFile.AddElem(XMLTAG_LAYERNAME,data);
		
		data.Format(_T("%d"),m_nGeoClass);
		xmlFile.AddElem(XMLTAG_DEFAULT_GEOCLASS,data);

		data.Format(_T("%d"),m_nDbGeoClass);
		xmlFile.AddElem(XMLTAG_DB_GEOCLASS,data);
		
		data.Empty();
		for (int i=0; i<m_strSupportClsName.GetSize(); i++)
		{
			data += m_strSupportClsName.GetAt(i);
			if (i == m_strSupportClsName.GetSize()-1)
				break;

			data += ",";
		}
		xmlFile.AddElem(XMLTAG_SUPPORT_GEOCLASS,data);

		data.Format(_T("%d"),m_nColor);		
		xmlFile.AddElem(XMLTAG_COLOR,data);

		xmlFile.AddElem(XMLTAG_DBLAYER,m_strDBLayName);

		// 固定属性
		CMarkup xml;
		xml.AddElem(XMLTAG_FIXEDATTRIBUTE);
		xml.IntoElem();
		Xml_WriteValueTable(xml,m_defValueTable);
		xml.OutOfElem();
		
		CString fixeddoc = xml.GetDoc();		
		xmlFile.AddSubDoc(fixeddoc);


		if( xmlFile.AddElem(XMLTAG_XATTRIBUTES) )
		{
			xmlFile.IntoElem();

			for(int i=0; i<m_arrXDefines.GetSize(); i++)
			{
				if( xmlFile.AddElem(XMLTAG_XATTRIBUTE) )
				{
					xmlFile.IntoElem();

					XDefine item = m_arrXDefines.GetAt(i);

					xmlFile.AddElem(XMLTAG_FIELD,item.field);

					xmlFile.AddElem(XMLTAG_NAME,item.name);
					
					data.Format(_T("%d"),item.valuetype);
					xmlFile.AddElem(XMLTAG_VALUE_TYPE,data);
					
					data.Format(_T("%d"),item.valuelen);
					xmlFile.AddElem(XMLTAG_VALUE_LENGTH,data);

					xmlFile.AddElem(XMLTAG_DEFAULT_VALUE,item.defvalue);

					data.Format(_T("%d"),item.isMust);
					xmlFile.AddElem(XMLTAG_ISMUST,data);

					data.Format(_T("%d"),item.nDigit);
					xmlFile.AddElem(XMLTAG_DIGITNUM,data);

					xmlFile.AddElem(XMLTAG_VALUERANGE,item.valueRange);

					xmlFile.OutOfElem();
				}
				
			}

			xmlFile.OutOfElem();
		}

		if( xmlFile.AddElem(XMLTAG_SYMBOLS) )
		{
			xmlFile.IntoElem();

			for(int i=0; i<m_arrPCfgs.GetSize(); i++)
			{
				CString strDoc;
				m_arrPCfgs.GetAt(i)->Save(strDoc);
				xmlFile.AddSubDoc(strDoc);
				
			}

			xmlFile.OutOfElem();
		}

		xmlFile.OutOfElem();
	}

//	xmlFile.OutOfElem();

	xmlString = xmlFile.GetDoc();

}


BOOL CSchemeLayerDefine::IsFromCADSymbol()
{
	return m_bFromCADSymbol;
}


BOOL CSchemeLayerDefine::CreateFromCADSymbol(CSymbol *pSym)
{
	Clear();

	int type = ((CCADSymbol*)pSym)->GetCADSymType();

	if( type==CCADSymbol::typeLin )
	{
		m_nGeoClass = CLS_GEOCURVE;
		m_nDbGeoClass = CLS_GEOCURVE;
	}
	else if( type==CCADSymbol::typeHat )
	{
		m_nGeoClass = CLS_GEOSURFACE;
		m_nDbGeoClass = CLS_GEOSURFACE;
	}
	else
	{
		m_nGeoClass = CLS_GEOPOINT;
		m_nDbGeoClass = CLS_GEOPOINT;
	}
	m_nColor = RGB(255,255,255);
	m_nCode = 0;
	m_bFromCADSymbol = TRUE;

	m_strGroupName = StrFromResID(IDS_SPECIALGROUP);
	m_strLayName = CString("CAD_") + pSym->GetName();
	m_strAccel.Empty();

	m_strSupportClsName.Add(StrFromResID(IDS_POINT));
	m_strSupportClsName.Add(StrFromResID(IDS_DIRPOINT));
	m_strSupportClsName.Add(StrFromResID(IDS_LINE));
	m_strSupportClsName.Add(StrFromResID(IDS_GEO_DCURVE));
	m_strSupportClsName.Add(StrFromResID(IDS_PARLINE));
	m_strSupportClsName.Add(StrFromResID(IDS_SURFACE));
	m_strSupportClsName.Add(StrFromResID(IDS_TEXT));

	m_arrPCfgs.Add(pSym);

	return TRUE;
}

BOOL CSchemeLayerDefine::SwapXDefine(int ndef1,int ndef2)
{
	if(ndef1<0 || ndef1>m_arrXDefines.GetSize()-1 || ndef2<0 || ndef2>m_arrXDefines.GetSize()-1)
		return FALSE;
	XDefine tDef = m_arrXDefines.GetAt(ndef1);
	m_arrXDefines.ElementAt(ndef1) = m_arrXDefines.ElementAt(ndef2);
	m_arrXDefines.ElementAt(ndef2) = tDef;
	return TRUE;

}
//hcw,2012.5.4,复制性传值。
void CSchemeLayerDefine::Copy( CSchemeLayerDefine& schemeLayerDefine )
{
	this->m_strGroupName = schemeLayerDefine.m_strGroupName;
	this->m_nCode = schemeLayerDefine.m_nCode;
	this->m_strLayName = schemeLayerDefine.m_strLayName;
	this->m_strAccel = schemeLayerDefine.m_strAccel;
	this->m_strSupportClsName.Copy(schemeLayerDefine.m_strSupportClsName);
	this->m_nGeoClass = schemeLayerDefine.m_nGeoClass;
	this->m_nDbGeoClass = schemeLayerDefine.m_nDbGeoClass;
	this->m_nColor = schemeLayerDefine.m_nColor;
	this->m_defValueTable.CopyFrom(schemeLayerDefine.m_defValueTable);
	this->m_arrXDefines.Copy(schemeLayerDefine.m_arrXDefines);
	//处理指针数组m_arrPCfgs;
	this->m_arrPCfgs.RemoveAll();
	for(int i = 0; i<schemeLayerDefine.m_arrPCfgs.GetSize(); i++)
	{
		//{hcw,2012.8.3
		CSymbol *pTmpSymbol = schemeLayerDefine.m_arrPCfgs.GetAt(i);
		if (!pTmpSymbol)
		{
			continue;
		}
		//}
		CSymbol *pSymbol = GCreateSymbol(pTmpSymbol->GetType());
		pSymbol->CopyFrom(pTmpSymbol);
		this->m_arrPCfgs.Add(pSymbol);
	
	}
	
	
}
//hcw,2012.9.10
void CSchemeLayerDefine::CopyXDefines( CSchemeLayerDefine& schemeLayerDefine, BOOL bOverrided)
{
	//schemeLayerDefine.
	int nDesXDefinesCount=0;
	this->GetXDefines(nDesXDefinesCount); 
	//判断是否要覆盖。
	if (bOverrided)
	{
		if (nDesXDefinesCount > 0)
		{
			for (int i=0; i < nDesXDefinesCount; i++)
			{
				this->DelXDefine(i);
			}
		}
	}
	
	int nSrcXDefinesCount = 0;
	schemeLayerDefine.GetXDefines(nSrcXDefinesCount); 
	for (int j = 0; j < nSrcXDefinesCount; j++)
	{
		this->AddXDefine(schemeLayerDefine.GetXDefine(j));
	}
	return ;	
}

BOOL CSchemeLayerDefine::DelXDefines()
{
	int nXDefinesCount=0;
	this->GetXDefines(nXDefinesCount);
	if (nXDefinesCount<=0)
	{
		return FALSE;
	}
	while((nXDefinesCount--)>=0)
	{
		this->DelXDefine(nXDefinesCount);
	}
	
	return TRUE;
}

BOOL CScheme::GetXAttributesAlias(CString LayerName, CValueTable &tab)
{
	CSchemeLayerDefine* pSLD = NULL; 
	for(int i=0;i<m_arrPLayDefines.GetSize();i++)
	{
		if(_tcsicmp(m_arrPLayDefines[i]->GetLayerName(),LayerName)==0)
		{
			pSLD = m_arrPLayDefines[i];
			break;
		}
	}
	if (pSLD)
	{
		CString field,name; 
		int num;
		const XDefine* pXDef = pSLD->GetXDefines(num);
		for(int i=0;i<num;i++)
		{
			tab.SetFieldName(pXDef[i].field,pXDef[i].name);			
		}
		return TRUE;
	}
	return FALSE;
}

void CScheme::SetRecentIdx(USERIDX& userIDX)
{

	m_UserRecent = userIDX;
	return;
}
//hcw,2012.4.6,根据组名按照层码重新排序。
void CScheme::SortLayerDefinebyCode(CString strGroupName)
{
	//m_arrPLayDefines.RemoveAll();
	if (strGroupName=="")
	{
		vector<CSchemeLayerDefine*> pLayerDefinesArray;
		for(int i=0; i<m_arrPLayDefines.GetSize(); i++)
		{
			//{hcw,2012.8.3
			if (!m_arrPLayDefines[i]) 
			{
				continue;
			}
			//}
			pLayerDefinesArray.push_back(m_arrPLayDefines[i]);
		}
		sort(pLayerDefinesArray.begin(), pLayerDefinesArray.end(),less_definelayCode);
		
		m_arrPLayDefines.RemoveAll();
		
		vector<CSchemeLayerDefine*>::iterator iter = pLayerDefinesArray.begin();
		while(iter!=pLayerDefinesArray.end())
		{
			m_arrPLayDefines.Add(*iter);
			iter++;
		}
		
	}
	else
	{
		vector<CSchemeLayerDefine*> pLayerDefinesArray;
		for (int i=0;i<m_arrPLayDefines.GetSize();i++)
		{
			//{hcw,2012.8.3
			if (!m_arrPLayDefines[i])
			{
				continue;
			}
			//}
			if(m_arrPLayDefines[i]->GetGroupName().CompareNoCase(strGroupName)==0)
			{
				pLayerDefinesArray.push_back(m_arrPLayDefines[i]);
				m_arrPLayDefines.RemoveAt(i);
				i--;
			}
		}
		sort(pLayerDefinesArray.begin(), pLayerDefinesArray.end(),less_definelayCode);
		vector<CSchemeLayerDefine*>::iterator iter = pLayerDefinesArray.begin();
		while (iter!=pLayerDefinesArray.end())
		{
			m_arrPLayDefines.Add(*iter);
			iter++;
		}
	}
	return;
}

CScheme &CScheme::operator=(const CScheme& scheme) //hcw,2012.5.17,重载"="操作 
{

	this->m_arrPLayDefines.RemoveAll();
	this->m_arrSpecialGroup.RemoveAll();
	
	for (int i=0; i<scheme.m_arrPLayDefines.GetSize();i++)
	{
		CSchemeLayerDefine *pTmpSchemeLayerDefine = scheme.m_arrPLayDefines.GetAt(i);
		if (!pTmpSchemeLayerDefine)
		{
			continue;
		}
		//}
		CSchemeLayerDefine *pSchemeLayerDefine = (CSchemeLayerDefine *)new CSchemeLayerDefine();
		pSchemeLayerDefine->Copy(*pTmpSchemeLayerDefine);
		this->m_arrPLayDefines.Add(pSchemeLayerDefine);
	}
	for(int j=0; j<scheme.m_arrSpecialGroup.GetSize();j++)
	{
		//{hcw,2012.8.3
		CSchemeLayerDefine *pTmpSpecialLayerDefine = scheme.m_arrPLayDefines.GetAt(j);
		if (!pTmpSpecialLayerDefine)
		{
			continue;
		}
		//}
		CSchemeLayerDefine *pSpecialLayerDefine = (CSchemeLayerDefine *)new CSchemeLayerDefine();
		pSpecialLayerDefine->Copy(*pTmpSpecialLayerDefine);
		this->m_arrSpecialGroup.Add(pSpecialLayerDefine);
	}
	this->m_strSchemePath = scheme.m_strSchemePath;
	this->m_UserRecent = scheme.m_UserRecent;

	m_pRecentLD = NULL;
	
	this->m_strSortedGroupName.Copy(scheme.m_strSortedGroupName);
	return *this;
}

float CScheme::GetLayerDefineLineWidth(LPCTSTR layname)
{
	if (layname == NULL) return 0.0;

	__int64 code = _ttoi64(layname);
	if( code>0 )
	{
		CString name = layname;
		FindLayerIdx(TRUE, code, name);
	}

	CSchemeLayerDefine *pLayer = GetLayerDefine(layname);
	if (!pLayer) return 0.0;

	return pLayer->GetDefaultLineWidth();	
}

int CScheme::GetDBLayerXdefines(CString dblayname, CArray<const XDefine*, const XDefine*>& xdefs)
{
	if (dblayname.IsEmpty())
		return 0;

	int nLay = GetLayerDefineCount();
	for (int i = 0; i < nLay; i++)
	{
		CSchemeLayerDefine *pLayDef = GetLayerDefine(i);
		if (!pLayDef) continue;
		if (dblayname.CompareNoCase(pLayDef->GetDBLayerName()) != 0)
			continue;

		int nXDefCount = 0;
		const XDefine *defs = pLayDef->GetXDefines(nXDefCount);
		for (int n = 0; n < nXDefCount; n++)
		{
			CString field(defs[n].field);
			BOOL bHas = FALSE;
			for (int k = 0; k < xdefs.GetSize(); k++)
			{
				if (field == xdefs[k]->field)
				{
					bHas = TRUE;
					break;
				}
			}
			if (!bHas)
			{
				xdefs.Add(&defs[n]);
			}
		}
	}

	return xdefs.GetSize();
}

MyNameSpaceEnd