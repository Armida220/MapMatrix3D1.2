// FtrLayer.cpp: implementation of the CFtrLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "FtrLayer.h"
#include "GeoCurve.h "
#include "SmartViewFunctions.h"
#include "GeoPoint.h"
#include "GeoSurface.h"
#include "GeoText.h"
#include "GeoDirPoint.h "
#include "GeoParallel.h "
#include "SymbolLib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtrLayer::CFtrLayer()
{
	m_nID = 0;
	m_nColor =RGB(255,255,255);
	m_bLocked = FALSE;
	m_bVisible = TRUE;
	m_bDeleted = FALSE;
	m_bIsInherent = FALSE;
	m_nDisplayOrder = -1;
	m_bSymbolized = TRUE;
	m_bHaveExtraAttr = FALSE;
	m_bDisplayOrderModified = TRUE;
	m_bAllowQuery = TRUE;
	m_xattributeName="";
}

CFtrLayer::CFtrLayer(int id)
{
	m_nID = id;
	m_nColor =RGB(255,255,255);
	m_bLocked = FALSE;
	m_bVisible = TRUE;
	m_bDeleted = FALSE;
	m_bIsInherent = FALSE;
	m_nDisplayOrder = -1;
	m_bSymbolized = TRUE;
	m_bHaveExtraAttr = FALSE;
	m_bDisplayOrderModified = TRUE;
	m_bAllowQuery = TRUE;
	m_xattributeName="";
}

CFtrLayer::~CFtrLayer()
{

}

void CFtrLayer::CopyFrom(const CFtrLayer * rv)
{
	//m_arrObjs=rv->m_arrObjs;
	//m_arrObjsByDisplayOrder=rv->m_arrObjsByDisplayOrder;
	m_bDisplayOrderModified = rv->m_bDisplayOrderModified;
	m_nID = rv->m_nID;
	m_strID = rv->m_strID;
	m_xattributeName = rv->m_xattributeName;
	m_strName = rv->m_strName;
	m_strMapName = rv->m_strMapName;
	m_strGroupName = rv->m_strGroupName;
	m_nColor = rv->m_nColor;
	m_bLocked = rv->m_bLocked;
	m_bVisible = rv->m_bVisible;
	m_bDeleted = rv->m_bDeleted;
	m_nDisplayOrder = rv->m_nDisplayOrder;
	m_bIsInherent = rv->m_bIsInherent;
	m_bSymbolized = rv->m_bSymbolized;
	m_bHaveExtraAttr = rv->m_bHaveExtraAttr;

	//±ãÓÚ¿ØÖÆ²éÑ¯
	m_bAllowQuery = rv->m_bAllowQuery;

	// ·ûºÅ
	// m_arrSymbols=rv->m_arrSymbols;
}

int CFtrLayer::GetID()const
{
	return m_nID;
}

LPCTSTR CFtrLayer::GetstrID()
{
	return m_strID;
}

LPCTSTR CFtrLayer::GetXAttributeName()
{
	return m_xattributeName;
}

BOOL CFtrLayer::IsInherent()
{
    return m_bIsInherent;
}

BOOL CFtrLayer::IsEmpty()
{
	return GetValidObjsCount()<=0;
}

LPCTSTR CFtrLayer::GetName()const
{
	return m_strName;
}

void CFtrLayer::SetID(int nId)
{
	m_nID = nId;
}

void CFtrLayer::SetstrID(LPCTSTR strid)
{
    m_strID=strid;
}

void CFtrLayer::SetXAttributeName(LPCTSTR xname)
{
	m_xattributeName=xname;
}

void CFtrLayer::SetName(LPCTSTR name)
{
	m_strName = name;
}


LPCTSTR CFtrLayer::GetMapName()const
{
	return m_strMapName;
}

int CFtrLayer::GetClassType()const
{
	return CLS_USERLAYER;
}

void CFtrLayer::EnableLocked(BOOL bLocked)
{
	m_bLocked = bLocked;
}

void CFtrLayer::SetInherentFlag(BOOL bInherent)
{
	m_bIsInherent = bInherent;
}

BOOL CFtrLayer::IsLocked()const
{
	return m_bLocked;
}

void CFtrLayer::EnableVisible(BOOL bVisible)
{
	m_bVisible = bVisible;
}

BOOL CFtrLayer::IsVisible()const
{
	return m_bVisible;
}

BOOL CFtrLayer::IsDeleted()const
{
	return m_bDeleted;
}


BOOL CFtrLayer::IsAllowQuery()const
{
	return m_bAllowQuery;
}

void CFtrLayer::SetDeleted(BOOL bDeleted)
{
	m_bDeleted = bDeleted;
}

BOOL CFtrLayer::GetSymbols(CPtrArray &arr)
{
	if (m_arrSymbols.GetSize() <= 0)
	{
		return FALSE;
	}
	arr.RemoveAll();
	arr.Copy(m_arrSymbols);
	return TRUE;
}

void CFtrLayer::SetSymbols(const CPtrArray &arr)
{
	m_arrSymbols.RemoveAll();
	if (arr.GetSize() > 0)
	{
		m_arrSymbols.Copy(arr);
	}
	
}

CFeature* CFtrLayer::CreateDefaultFeature(int nScale,int clsid )
{
	CSchemeLayerDefine *pSchemeItem = NULL;
	CConfigLibManager *pCfg = GetConfigLibManager();
	if(pCfg)
	{
		CScheme *pScheme = pCfg->GetScheme(nScale);
		if (pScheme)
			pSchemeItem = pScheme->GetLayerDefine(m_strName);
	}
	if (!pSchemeItem)
	{
		if(clsid!=CLS_NULL)
		{
			CFeature *pFtr = new CFeature;
			if (!pFtr)	return NULL;

			CGeometry *pGeo = (CGeometry *)CPermanent::CreatePermanentObject(clsid);

			/*switch(clsid)
			{
			case CLS_GEOPOINT:
				pGeo = new CGeoPoint;
				break;
			case CLS_GEOCURVE:
				pGeo = new CGeoCurve;
				break;
			case CLS_GEODCURVE:
				pGeo = new CGeoDCurve;
				break;
			case CLS_GEOSURFACE:
				pGeo = new CGeoSurface;
				break;
			case CLS_GEOTEXT:
				pGeo = new CGeoText;
				break;
			case CLS_GEODIRPOINT:
				pGeo = new CGeoDirPoint;
				break;
			case CLS_GEOPARALLEL:
				pGeo = new CGeoParallel;
				break;
			case CLS_GEOMULTISURFACE:
				pGeo = new CGeoMultiSurface;
				break;
			case CLS_GEOMULTIPOINT:
				pGeo = new CGeoMultiPoint;
				break;
			case CLS_GEODEMPOINT:
				pGeo = new CGeoDemPoint;
				break;
			default:
				break;
			}	*/		  
			if (!pGeo) return NULL;
			pFtr->SetGeometry(pGeo);
			return pFtr;			
		}
		if (_tcscmp(LPCTSTR(m_strName),LPCTSTR(StrFromResID(IDS_DEFLAYER_NAMEP)))==0)
		{
			CFeature *pFtr = new CFeature;
			if (!pFtr)	return NULL;
			CGeoPoint *pGeo = new CGeoPoint;
			if (!pGeo) return NULL;
			pFtr->SetGeometry(pGeo);
			return pFtr;
		}
		else if (_tcscmp(LPCTSTR(m_strName),LPCTSTR(StrFromResID(IDS_DEFLAYER_NAMEL)))==0)
		{
			CFeature *pFtr = new CFeature;
			if (!pFtr)	return NULL;
			CGeoCurve *pGeo = new CGeoCurve;
			if (!pGeo) return NULL;
			pFtr->SetGeometry(pGeo);
			return pFtr;
		}
		else if (_tcscmp(LPCTSTR(m_strName),LPCTSTR(StrFromResID(IDS_DEFLAYER_NAMEP)))==0)
		{
			CFeature *pFtr = new CFeature;
			if (!pFtr)	return NULL;
			CGeoSurface *pGeo = new CGeoSurface;
			if (!pGeo) return NULL;
			pFtr->SetGeometry(pGeo);
			return pFtr;
		}
		else if (_tcscmp(LPCTSTR(m_strName),LPCTSTR(StrFromResID(IDS_DEFLAYER_NAMET)))==0)
		{
			CFeature *pFtr = new CFeature;
			if (!pFtr)	return NULL;
			CGeoText *pGeo = new CGeoText;
			if (!pGeo) return NULL;
			pFtr->SetGeometry(pGeo);
			return pFtr;
		}
		else
		{
			CFeature *pFtr = new CFeature;
			if (!pFtr)	return NULL;
			CGeoCurve *pGeo = new CGeoCurve;
			if (!pGeo) return NULL;
			pFtr->SetGeometry(pGeo);
			return pFtr;
		}
		//return NULL;
	}
	return pSchemeItem->CreateDefaultFeature(clsid);	
}

COLORREF CFtrLayer::GetColor()const
{
	return m_nColor;
}

void CFtrLayer::SetColor(COLORREF color)
{
	m_nColor = color;
}

BOOL CFtrLayer::FindObj(FTR_HANDLE handle, int& insert_idx)
{
	int findidx = -1, bnew = 0;
	if( m_arrObjs.GetSize()<=0 )
	{ 
		findidx = 0;
		bnew = 1;
	}
	else
	{
		int i0 = 0, i1 = m_arrObjs.GetSize()-1, i2;
		while (findidx == -1) 
		{
			if( handle<=FTR_HANDLE(m_arrObjs.GetAt(i0)) )
			{
				bnew = (handle==FTR_HANDLE(m_arrObjs.GetAt(i0))?0:1);
				findidx = i0;
				break;
			}
			else if( handle>=FTR_HANDLE(m_arrObjs.GetAt(i1)) )
			{
				bnew = (handle==FTR_HANDLE(m_arrObjs.GetAt(i1))?0:1);
				findidx = (handle==FTR_HANDLE(m_arrObjs.GetAt(i1))?i1:(i1+1));
				break;
			}
			
			i2 = (i0+i1)/2;
			if( i2==i0 )
			{
				bnew = (handle==FTR_HANDLE(m_arrObjs.GetAt(i0))?0:1);
				findidx = (handle==FTR_HANDLE(m_arrObjs.GetAt(i0))?i0:(i0+1));
				break;
			}
			
			if( handle<FTR_HANDLE(m_arrObjs.GetAt(i2)) )
				i1 = i2;
			else if( handle>FTR_HANDLE(m_arrObjs.GetAt(i2)) )
				i0 = i2;
			else
			{
				findidx = i2; bnew = 0;
				break;
			}
		}
	}
	
	insert_idx = findidx;
	return (bnew==0);
}

BOOL CFtrLayer::AddObject(CFeature *pFt)
{
	m_arrObjs.Add(pFt);
	pFt->SetLayerID(m_nID);

	m_bDisplayOrderModified = TRUE;
	
	return TRUE;
}

BOOL CFtrLayer::DisplayTop(CFeature *pFtr)
{
	if (!pFtr)  return FALSE;
	
	double order = pFtr->GetDisplayOrder();
	
	double max = GetFtrsMaxMinDisplayOrder(TRUE);
	if (order >= max)
		return FALSE;
	
	pFtr->SetDisplayOrder(++max);

	m_bDisplayOrderModified = TRUE;

	return TRUE;
}	

BOOL CFtrLayer::DisplayBottom(CFeature *pFtr)
{
	if (!pFtr)  return FALSE;
	
	double order = pFtr->GetDisplayOrder();

	double min = GetFtrsMaxMinDisplayOrder(FALSE);
	if (order <= min)
		return FALSE;

	pFtr->SetDisplayOrder(--min);

	m_bDisplayOrderModified = TRUE;

	return TRUE;

}

double CFtrLayer::GetFtrsMaxMinDisplayOrder(BOOL bMax)
{
	int count = GetObjectCount();

	if (bMax)
	{
		double nMax = -1;
		
		for (int i=0; i<count; i++)
		{
			CFeature *pFtr = GetObject(i,FILTERMODE_NULL);
			if (!pFtr) continue;
			
			double order = pFtr->GetDisplayOrder();
			if (order > nMax)
				nMax = order;
		}

		return nMax;
	}
	else
	{
		double nMin = -1;
		
		for (int i=0; i<count; i++)
		{
			CFeature *pFtr = GetObject(i,FILTERMODE_NULL);
			if (!pFtr) continue;
			
			double order = pFtr->GetDisplayOrder();
			if (order < nMin)
				nMin = order;
		}
		
		return nMin;
	}	
	
}



int CompFuncSortFtrInLayer(const void *e1, const void *e2)
{
	double order1 = (*(CFeature**)e1)->GetDisplayOrder();
	double order2 = (*(CFeature**)e2)->GetDisplayOrder();
	if( order1<order2 )
	{
		return -1;
	}
	else if( order1==order2 )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void CFtrLayer::GetAllFtrsByDisplayOrder(CFtrArray &arr)
{
	arr.Copy(m_arrObjsByDisplayOrder);
	
	if( m_bDisplayOrderModified )
	{
		arr.Copy(m_arrObjs);

		CPFeature *data = arr.GetData();
		int nCount = arr.GetSize();

		qsort(data,nCount,sizeof(CPFeature),CompFuncSortFtrInLayer);

		/*
		int k, nCount = arr.GetSize();
		for (int i=0; i<nCount; i++)
		{
			k = i;
			for( int j=i+1; j<nCount; j++)
			{
				if( arr[k]->GetDisplayOrder()>arr[j]->GetDisplayOrder() )
				{
					k = j;
				}
			}
			
			if( k!=i )
			{
				CFeature *pFtr = arr[k];
				arr[k] = arr[i];
				arr[i] = pFtr;
			}
		}*/

		m_arrObjsByDisplayOrder.Copy(arr);
		m_bDisplayOrderModified = FALSE;
	}
}

void CFtrLayer::ResetFtrsDisplayOrder()
{
	CFtrArray arr;
	arr.Copy(m_arrObjs);
	
	int k, nCount = arr.GetSize();
	for (int i=0; i<nCount; i++)
	{
		k = i;
		for( int j=i+1; j<nCount; j++)
		{
			if( arr[k]->GetDisplayOrder()>arr[j]->GetDisplayOrder() )
			{
				k = j;
			}
		}
		
		if( k!=i )
		{
			CFeature *pFtr = arr[k];
			arr[k] = arr[i];
			arr[i] = pFtr;
		}
	}

	int minOrder = 0;
	for (i=nCount-1; i>=0; i--,minOrder++)
	{
		arr[i]->SetDisplayOrder(minOrder);
	}

	m_bDisplayOrderModified = TRUE;
}

BOOL CFtrLayer::DeleteObject(CFeature *pFt)
{
// 	int i;
// 	if (FindObj(FTR_HANDLE(pFt),i))
// 	{		
// 		m_arrObjs.RemoveAt(i);
// 		return TRUE;		
// 	}
// 	return FALSE;
	int nsz = m_arrObjs.GetSize();
	CFeature **pp = m_arrObjs.GetData();
	for( int i=0; i<nsz; i++)
	{
		if( pp[i]==pFt )
		{
			m_arrObjs.RemoveAt(i);
			m_bDisplayOrderModified = TRUE;
			return TRUE;
		}
	}
 	return FALSE;
}

int CFtrLayer::GetObjectCount()
{
	return m_arrObjs.GetSize();
}

CFeature* CFtrLayer::GetObject(int i, int filterMode)
{
	if( i>=0 && i<m_arrObjs.GetSize() )
	{
		CFeature* pFt = m_arrObjs.GetAt(i);
		if(pFt!=NULL)
		{
			if((filterMode&FILTERMODE_DELETED)&&pFt->IsDeleted())
				return NULL;
			if ((filterMode&FILTERMODE_INVISIBLE)&&!pFt->IsVisible())
				return NULL;
			return pFt;
	}
		else
			return NULL;

	}
	return NULL;
}

BOOL CFtrLayer::WriteTo(CValueTable& tab)const
{
	CPermanent::WriteTo(tab);

	_variant_t var;
	var = (long)m_nID;
	tab.AddValue(FIELDNAME_LAYERID,&CVariantEx(var),IDS_FIELDNAME_LAYERID);

	var = (LPCTSTR)m_strName;
	tab.AddValue(FIELDNAME_LAYERNAME,&CVariantEx(var),IDS_FIELDNAME_LAYERNAME);

// 	var = (LPCTSTR)m_strMapName;
// 	tab.AddValue(FIELDNAME_LAYMAPNAME,&CVariantEx(var),IDS_FIELDNAME_MAPNAME);

	var = (LPCTSTR)m_strGroupName;
	tab.AddValue(FIELDNAME_LAYGROUPNAME,&CVariantEx(var),IDS_FIELDNAME_GROUPNAME);

	var = (long)m_nColor;
	tab.AddValue(FIELDNAME_LAYCOLOR,&CVariantEx(var),IDS_FIELDNAME_DEFCOLOR);

	var = (bool)m_bLocked;
	tab.AddValue(FIELDNAME_LAYLOCKED,&CVariantEx(var),IDS_FIELDNAME_LOCKED);
	
	var = (bool)m_bVisible;
	tab.AddValue(FIELDNAME_LAYVISIBLE,&CVariantEx(var),IDS_FIELDNAME_VISIBLE);
	
	var = (bool)m_bIsInherent;
	tab.AddValue(FIELDNAME_LAYINHERENT,&CVariantEx(var),IDS_FIELDNAME_INHERENT);

	var = (_variant_t)(long)m_nDisplayOrder;
	tab.AddValue(FIELDNAME_LAYERDISPLAYORDER,&CVariantEx(var),IDS_FIELDNAME_DISPLAYORDER);

	var = (bool)m_bSymbolized;
	tab.AddValue(FIELDNAME_LAYERSYMBOLIZED,&CVariantEx(var),IDS_FIELDNAME_SYMBOL);

	
	return TRUE;
}


BOOL CFtrLayer::ReadFrom(CValueTable& tab,int idx)
{
	CPermanent::ReadFrom(tab,idx);

	const CVariantEx *p;
	if( tab.GetValue(idx,FIELDNAME_LAYERID,p) )
	{
		m_nID = (long)(_variant_t)*p;
	}

	if( tab.GetValue(idx,FIELDNAME_LAYERNAME,p) )
	{
		m_strName = (LPCTSTR)(_bstr_t)(_variant_t)*p;
	}

// 	if( tab.GetValue(idx,FIELDNAME_LAYMAPNAME,p) )
// 	{
// 		m_strMapName = (LPCTSTR)(_bstr_t)(_variant_t)*p;
// 	}

	if( tab.GetValue(idx,FIELDNAME_LAYGROUPNAME,p) )
	{
		m_strGroupName = (LPCTSTR)(_bstr_t)(_variant_t)*p;
	}

	if( tab.GetValue(idx,FIELDNAME_LAYLOCKED,p) )
	{
		m_bLocked = (bool)(_variant_t)*p;
	}

	if( tab.GetValue(idx,FIELDNAME_LAYVISIBLE,p) )
	{
		m_bVisible = (bool)(_variant_t)*p;
	}
	if ( tab.GetValue(idx,FIELDNAME_LAYCOLOR,p) )
	{
		m_nColor = (long)(_variant_t)*p;
	}
	if( tab.GetValue(idx,FIELDNAME_LAYINHERENT,p) )
	{
		m_bIsInherent = (bool)(_variant_t)*p;
	}
	if( tab.GetValue(idx,FIELDNAME_LAYERDISPLAYORDER,p) )
	{
		m_nDisplayOrder = (long)(_variant_t)*p;
	}
	if( tab.GetValue(idx,FIELDNAME_LAYERSYMBOLIZED,p) )
	{
		m_bSymbolized = (bool)(_variant_t)*p;
	}

	
	return TRUE;
}

void CFtrLayer::SetMapName(LPCTSTR mapName)
{
	m_strMapName = mapName;
}


void CFtrLayer::SetAllowQuery(BOOL bQuery)
{
	m_bAllowQuery = bQuery;
}


int CFtrLayer::GetValidObjsCount()
{
	int count = 0 ;
	for( int i=0 ; i<m_arrObjs.GetSize();i++ )
	{
		CFeature* pFt = m_arrObjs.GetAt(i);		
		if( pFt!=NULL && !pFt->IsDeleted() )
			count++;
	}	
	return count;
}

int CFtrLayer::GetEditableObjsCount()
{
	int count = 0 ;
	for( int i=0 ; i<m_arrObjs.GetSize();i++ )
	{
		CFeature* pFt = m_arrObjs.GetAt(i);		
		if( pFt!=NULL && !pFt->IsDeleted()&&pFt->IsVisible() )
			count++;
	}	
	return count;
}


MyNameSpaceEnd