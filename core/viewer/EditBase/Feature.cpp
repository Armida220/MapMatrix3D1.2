// EBFeature.cpp: implementation of the CFeature class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "EditBase.h"
#include "resource.h"
#include "Feature.h"
#include "Symbol.h"
#include "GeoPoint.h"
#include "GeoCurve.h"
#include "GeoSurface.h"
#include "GeoText.h"
#include "GeoDirPoint.h"
#include "GeoParallel.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin


IMPLEMENT_DYNAMIC(CFeature,CPermanent)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
CGeometry*  GCreateGeometry(int nGeoClass)
{
	switch(nGeoClass)
	{
	case CLS_GEOPOINT:
		{
			CGeoPoint *pPoint = new CGeoPoint;
			return pPoint;
		}
	case CLS_GEOCURVE:
		{
			CGeoCurve *pCur = new CGeoCurve;
			return pCur;
		}
	case CLS_GEOSURFACE:
		{
			CGeoSurface *pSur = new CGeoSurface;
			return pSur;
		}
	case CLS_GEOTEXT:
		{
			CGeoText *pText = new CGeoText;
			return pText;
		}
	case CLS_GEODIRPOINT:
		{
			CGeoDirPoint *pDirPoint = new CGeoDirPoint;
			return pDirPoint;
		}
	case CLS_GEOPARALLEL:
		{
			CGeoParallel *pParallel = new CGeoParallel;
			return pParallel;
		}
	case CLS_GEODEMPOINT:
		{
			CGeoDemPoint *pDemPoint = new CGeoDemPoint;
			return pDemPoint;
		}
	case CLS_GEOMULTIPOINT:
		{
			CGeoMultiPoint *pMultiPoint = new CGeoMultiPoint;
			return pMultiPoint;
		}
	case CLS_GEOMULTISURFACE:
		{
			CGeoMultiSurface *pPMultiSurface = new CGeoMultiSurface;
			return pPMultiSurface;
		}
	default:
		return NULL;
	}
	
}
*/
CFeature::CFeature()
{
	m_pGeometry = NULL;
	m_bDeleted = FALSE;
	m_bVisible = TRUE;
	m_nPurpose = FTR_COLLECT;
	m_nAppFlag = 0;
	m_fDisplayOrder = -1;
	m_nModifyTime = 0;
	memset(m_strCode,0x00,16);
}

CFeature::~CFeature()
{
	if( m_pGeometry )
	{
		delete m_pGeometry;
		m_pGeometry = NULL;
	}
}

BOOL CFeature::CreateGeometry(int nGeoClass)
{
// 	CGeometry *pGeo = GetGeometry();
// 	if (pGeo)
// 	{
// 		SetGeometry(NULL);
// 	}

	CGeometry *pGeo = (CGeometry*)CPermanent::CreatePermanentObject(nGeoClass);//GCreateGeometry(nGeoClass);

	if (pGeo)
	{
		SetGeometry(pGeo);
		return TRUE;
	}
	return FALSE;

}

CFeature* CFeature::Clone()const
{
	CFeature *pFt = new CFeature;
	if( !pFt )return NULL;
	
	pFt->CopyFrom(this);
	return pFt;
}


BOOL CFeature::CopyFrom(const CFeature *pObj)
{
	if( m_pGeometry )m_pGeometry->CopyFrom(pObj->GetGeometry());
	else
	{
		CGeometry *pGeo = (pObj->m_pGeometry!=NULL?pObj->m_pGeometry->Clone():NULL);
		SetGeometry(pGeo);
	}
	m_id = pObj->m_id;
	m_bDeleted = pObj->m_bDeleted;
	m_bVisible = pObj->m_bVisible;
	m_nUserID = pObj->m_nUserID;
	m_nPurpose = pObj->m_nPurpose;
	m_layerid = pObj->m_layerid;
	m_fDisplayOrder = pObj->m_fDisplayOrder;
	m_nModifyTime = pObj->m_nModifyTime;
	strncpy(m_strCode,pObj->m_strCode,16);
	return TRUE;	
}


CGeometry * CFeature::GetGeometry()const
{
	return m_pGeometry;
}

CGeometry * CFeature::SetGeometry(CGeometry *pGeo)
{
	if (m_pGeometry == pGeo) return m_pGeometry;

	if (m_pGeometry)
	{
		delete m_pGeometry;
		m_pGeometry = NULL;
	}
	m_pGeometry = pGeo;
	return m_pGeometry;
}

int  CFeature::GetClassType()const
{
	return CLS_DLGFEATURE;
}

void CFeature::SetCode(const char* code)
{
	if (!code) return;

	strncpy(m_strCode,code,15); 
	m_strCode[15] = '\0';
}

BOOL CFeature::AddObjectGroup(UINT id)
{
	m_arrGroupID.Add(id);
	return TRUE;
}

BOOL CFeature::RemoveObjectGroup(UINT id)
{
	int size = m_arrGroupID.GetSize();
	for (int i=size-1; i>=0; i--)
	{
		if (m_arrGroupID[i] == id)
		{
			m_arrGroupID.RemoveAt(i);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFeature::IsInObjectGroup(UINT id)const
{
	int size = m_arrGroupID.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_arrGroupID[i] == id)
		{
			break;
		}
	}
	
	if (i < size) return TRUE;

	return FALSE;
}

int CFeature::GetAppFlagBit(int nBit)const
{
	if( nBit<0 || nBit>=24 )
		return 0;

	int flag = (1<<nBit);
	if( (m_nAppFlag&flag)!=0 )
		return 1;

	return 0;
}


void CFeature::SetAppFlagBit(int nBit, int flag)
{
	if( nBit<0 || nBit>=24 )
		return;

	int value = (1<<nBit);
	if( flag!=0 )
		m_nAppFlag |= value;
	else
		m_nAppFlag &= (~value);
}

BOOL CFeature::ReadFrom(CValueTable &tab,int idx)
{
	CPermanent::ReadFrom(tab,idx);
	
	const CVariantEx *var;
	if( tab.GetValue(idx,FIELDNAME_FTRID,var) )
	{
		CString str = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_id.FromString(str);
	}

	if( tab.GetValue(idx,FIELDNAME_FTRCODE,var) )
	{
		CString str = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		str = str.Left(15);
		strcpy(m_strCode,str);
	}

	if( tab.GetValue(idx,FIELDNAME_FTRVISIBLE,var) )
	{
		m_bVisible  = (bool)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_FTRPURPOSE,var) )
	{
		m_nPurpose  = (long)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_FTRDISPLAYORDER,var) )
	{
		m_fDisplayOrder  = (double)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_FTRGROUPID,var) )
	{
		int nLen;
		const BYTE *pBuf = var->GetBlob(nLen);
		if (pBuf && nLen > 0)
		{
			UINT *p = (UINT*)pBuf;
			for (int i=0; i<nLen; i+=sizeof(UINT))
			{
				m_arrGroupID.Add(UINT(*p));
				p = p + i;
			}
		}
	}
	
	if( tab.GetValue(idx,FIELDNAME_FTRMODIFYTIME,var) )
	{
		m_nModifyTime = (long)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOCLASS,var) )
	{
		long nClsType = (long)(_variant_t)*var;
		if( m_pGeometry && m_pGeometry->GetClassType()==nClsType );
		else
		{
			CGeometry *pObj = (CGeometry*)CPermanent::CreatePermanentObject(nClsType);
			if( pObj )
			{
				if( m_pGeometry )delete m_pGeometry;
				m_pGeometry = pObj;
			}
		}
	}

	if( m_pGeometry )
	{
		m_pGeometry->ReadFrom(tab,idx);
	}

	return TRUE;
}

BOOL CFeature::WriteTo(CValueTable &tab)const
{
	CPermanent::WriteTo(tab);
	
	CVariantEx var;

	var = (_variant_t)(LPCTSTR)(m_id.ToString());
	tab.AddValue(FIELDNAME_FTRID,&var,IDS_FIELDNAME_ID);

	var = (_variant_t)(LPCTSTR)(m_strCode);
	tab.AddValue(FIELDNAME_FTRCODE,&var,IDS_FIELDNAME_FTRCODE);

	var = (_variant_t)(bool)(m_bVisible);
	tab.AddValue(FIELDNAME_FTRVISIBLE,&var,IDS_FIELDNAME_FTRVISIBLE);

	var = (_variant_t)(long)(m_nPurpose);
	tab.AddValue(FIELDNAME_FTRPURPOSE,&var,IDS_FIELDNAME_FTRPURPOSE);

	var = (_variant_t)(double)(m_fDisplayOrder);
	tab.AddValue(FIELDNAME_FTRDISPLAYORDER,&var,IDS_FIELDNAME_DISPLAYORDER);

	var.SetAsBlob((BYTE*)m_arrGroupID.GetData(),m_arrGroupID.GetSize()*sizeof(UINT));
	tab.AddValue(FIELDNAME_FTRGROUPID,&var,IDS_FIELDNAME_GROUPID);
	
	var = (_variant_t)(long)(m_nModifyTime);
	tab.AddValue(FIELDNAME_FTRMODIFYTIME,&var,IDS_FIELDNAME_MODIFYTIME);
	
	
	if( m_pGeometry )
	{
		m_pGeometry->WriteTo(tab);
	}
	
	return TRUE;
}


void CFeature::Draw(GrBuffer *pBuf, float fDrawScale)const
{
	if( !m_pGeometry )return;
	m_pGeometry->Draw(pBuf,fDrawScale);
	return;
}


long CFeature::GetModifyTime()const
{
	return m_nModifyTime;
}


void CFeature::SetCurrentModifyTime()
{
	time_t t;
	time(&t);

	m_nModifyTime = t;
}

MyNameSpaceEnd