// DBAccess.cpp: implementation of the CDBAccess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "DBAccess.h"
#include "FtrLayer.h"
#include "DlgDataSource.h"
#include <io.h>
#include <float.h>
#include "Feature.h"
#include "GeoPoint.h"
#include "GeoSurface.h"
#include "GeoText.h"
#include "GeoDirPoint.h "
#include "GeoParallel.h "
#include "..\CORE\viewer\EditBase\res\resource.h"
#include "SmartViewFunctions.h"
#include "PlotWChar.h "

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void DpVar2Var(CDpDBVariant& var1, _variant_t& var2)
{
	switch( var1.m_dwType ) 
	{
	case DPDBVT_BOOL:
		var2 = (bool)var1.m_boolVal;
		break;
	case DPDBVT_UCHAR:
		var2 = (short)var1.m_chVal;
		break;
	case DPDBVT_SHORT:
		var2 = (short)var1.m_iVal;
		break;
	case DPDBVT_LONG:
		var2 = (long)var1.m_lVal;
		break;
	case DPDBVT_SINGLE:
		var2 = (float)var1.m_fltVal;
		break;
	case DPDBVT_DOUBLE:
		var2 = (double)var1.m_dblVal;
		break;
	case DPDBVT_STRING:
		var2 = (LPCTSTR)var1.m_pString;
		break;
	default:;
	}
}

void Var2DpVar(_variant_t& var1, CDpDBVariant& var2)
{
	switch( var1.vt ) 
	{
	case VT_UI1:
		var2 = (long)var1.bVal;
		break;
	case VT_UI2:
		var2 = (long)var1.uiVal;
		break;
	case VT_UI4:
		var2 = (long)var1.ulVal;
		break;
	case VT_UINT:
		var2 = (long)var1.uintVal;
		break;
	case VT_INT:
		var2 = (long)var1.intVal;
		break;
	case VT_I1:
		var2 = (long)var1.cVal;
		break;
	case VT_I2:
		var2 = (long)var1.iVal;
		break;
	case VT_I4:
		var2 = (long)var1.lVal;
		break;
	case VT_R4:
		var2 = (float)var1.fltVal;
		break;
	case VT_R8:
		var2 = (float)var1.dblVal;
		break;
	case VT_BSTR:
		var2 = (LPCTSTR)(_bstr_t)var1;
		break;
	case VT_BOOL:
		var2 = (bool)var1;
		break;
	default:;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDBAccess::CDBAccess()
{
	m_pDpWks = NULL;
	m_pDpDatabase = NULL;

	m_pLayerMgr = NULL;
	m_pGroupMgr = NULL;

	m_pLayerRecords = NULL;
	m_pGroupRecords = NULL;

	m_pFtrCls = NULL;
	m_nCurLayer = -1;
	m_nCurFtr = 0;

	m_nCurLayId = 0;
	m_nCurGrpId = 0;

	m_nLayIdIdx = -1;
	m_nGrpIdIdx = -1;

	m_pDlgDataSource = NULL;
}

CDBAccess::~CDBAccess()
{
	Close();
}

void CDBAccess::SetRefDataSource(CDataSourceEx *pDS)
{
	m_pDlgDataSource = pDS;
}

BOOL CDBAccess::ReadDataSourceInfo(CDataSourceEx *pDS)
{	
	m_pDlgDataSource = pDS;

	if( !m_pDpWks )return FALSE;

	// 获取图幅范围
	CDpStringArray arrUnit;
	m_pDpWks->GetUnitsID(arrUnit);
	
	if( arrUnit.GetCount()<=0 )return FALSE;
	
	CDpUnit *pu = NULL;
	m_pDpWks->GetUnit(arrUnit.GetItem(0),&pu);
	
	if( !pu )return FALSE;
	
	CDpPoint *ps = pu->m_ptQuadrangle;

	CArray<PT_3DEX,PT_3DEX> arrPts;
	arrPts.SetSize(4);

	PT_3DEX *pts = arrPts.GetData();
	pts[0].x = ps[0].m_dx; pts[0].y = ps[0].m_dy; pts[0].z = 0;
	pts[1].x = ps[1].m_dx; pts[1].y = ps[1].m_dy; pts[1].z = 0; 
	pts[2].x = ps[2].m_dx; pts[2].y = ps[2].m_dy; pts[2].z = 0; 
	pts[3].x = ps[3].m_dx; pts[3].y = ps[3].m_dy; pts[3].z = 0;	

	//获取比例尺
	DWORD nScale = (DWORD)m_pDpWks->GetWksScale();	

	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();
	
	var.SetAsShape(arrPts);
	tab.AddValue(FIELDNAME_BOUND,&var);

	var = (_variant_t)(long)nScale;
	tab.AddValue(FIELDNAME_SCALE,&var);

	tab.EndAddValueItem();

	pDS->ReadFrom(tab);

	return TRUE;
}


void CDBAccess::WriteDataSourceInfo(CDataSourceEx *pDS)
{
	if( !m_pDpWks )return;
	CDpStringArray arrUnit;
	m_pDpWks->GetUnitsID(arrUnit);
	
	if( arrUnit.GetCount()<=0 )return;
	
	CDpUnit *pu = NULL;
	m_pDpWks->GetUnit(arrUnit.GetItem(0),&pu);
	
	if( !pu )return;
	PT_3D pts[4];
	double zMin,zMax;
	pDS->GetBound(pts,&zMin,&zMax);
	CDpPoint pts0[4];
	memset(pts0,0,sizeof(pts0));
	pts0[0].m_dx = pts[0].x;
	pts0[0].m_dy = pts[0].y;
	pts0[1].m_dx = pts[1].x;
	pts0[1].m_dy = pts[1].y;
	pts0[2].m_dx = pts[2].x;
	pts0[2].m_dy = pts[2].y;
	pts0[3].m_dx = pts[3].x;
	pts0[3].m_dy = pts[3].y;
	pu->SetUnitVailRegion(pts0);

	m_pDpWks->SetWksScale(pDS->GetScale());
}



CFtrLayer* CDBAccess::FtrLayerFromDpObject(CDpUserDefinedGroupMgr *pMgr, CDpUserDefinedGroupObject *pObj)
{
	CFtrLayer *pLayer = new CFtrLayer;
	if( !pLayer )return NULL;

	int nOIDIdx = pMgr->GetAttrFieldIndex("ID");
	int nUIDIdx = pMgr->GetAttrFieldIndex("GRID_ID");

	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();

	var = (_variant_t)(long)pObj->GetID();
	tab.AddValue("ID",&var);

	var = (_variant_t)(LPCTSTR)pObj->GetUnitID();
	tab.AddValue("GRID_ID",&var);

	CDpCustomFieldInfo info;
	CDpDBVariant dpvar;

	int nField = pMgr->GetAttrFieldCount();
	for( int i=0, j=0; i<nField; i++)
	{
		if( i==nOIDIdx || i==nUIDIdx )continue;
		pMgr->GetAttrFieldInfo(i, &info);

		pObj->GetAttrValue(j++, dpvar);

		_variant_t t;
		DpVar2Var(dpvar,t);
		var = t;
		
		tab.AddValue(info.m_strName,&var);
	}

	tab.EndAddValueItem();
	pLayer->ReadFrom(tab);

	return pLayer;
}


CDpUserDefinedGroupObject* CDBAccess::FtrLayerToDpObject(CDpUserDefinedGroupMgr *pMgr, CFtrLayer *pLayer)
{
	int nField = pMgr->GetAttrFieldCount();
	CDpUserDefinedGroupObject *pObj = new CDpUserDefinedGroupObject(0,0);
	if( !pObj )return NULL;
	
	pObj->SetID(pLayer->GetID());
	pObj->SetUnitID(pLayer->GetMapName());

	pObj->m_pAttrFieldValueArray = new CDpDBVariant[nField-2];
	if( pObj->m_pAttrFieldValueArray )
	{
		CValueTable tab;
		tab.BeginAddValueItem();
		pLayer->WriteTo(tab);
		tab.EndAddValueItem();

		pObj->m_iAttrFieldCount = nField-2;

		int nIdIdx1 = pMgr->GetAttrFieldIndex("ID"), j=0;
		int nIdIdx2 = pMgr->GetAttrFieldIndex("GRID_ID");
		
		const CVariantEx *var;
		CDpCustomFieldInfo info;
		CDpDBVariant dpvar;
		for( int i=0; i<nField; i++)
		{
			if( i==nIdIdx1 || i==nIdIdx2 )continue;

			pMgr->GetAttrFieldInfo(i, &info);
			
			if( tab.GetValue(0, info.m_strName, var) )
			{
				_variant_t t = (_variant_t)*var;
				Var2DpVar(t,dpvar);
				pObj->SetAttrValue(j++, dpvar);
			}
		}
	}
	
	return pObj;
}


//读取层
CFtrLayer *CDBAccess::GetFirstFtrLayer(long &idx)
{	
	if( m_pFtrCls )delete m_pFtrCls;
		m_pFtrCls = NULL;
	BOOL bFind = FALSE;
	for (int i=0; i<m_listDpLayers.GetSize(); i++)
	{	
		CDpFeatureLayer *pLayer = (CDpFeatureLayer*)m_listDpLayers.GetAt(i);
		if (pLayer)
		{	
			CDpStringArray SelectCol;
			SelectCol.AddString("FID");				
			if(pLayer->QueryFeatureClass(&SelectCol,"DISTINCT",NULL,&m_pFtrCls))
			{
				if(m_pFtrCls->NextRecord())
				{
					m_nCurIndexLayer = i;
					bFind = TRUE;
					break;
				}
			}
			if (m_pFtrCls)delete m_pFtrCls;			
		}
	}
	if (bFind)
	{
		CFtrLayer *pLayer = new CFtrLayer;
		if( !pLayer )
		{
			if (m_pFtrCls)delete m_pFtrCls;
			idx = 0;
			return NULL;
		}
		CDpDBVariant var;
		m_pFtrCls->GetFieldValue("FID",var);	
		pLayer->SetName(var.m_pString);
		idx = 1;
		return pLayer;
	}
	idx = 0;
	return NULL;
}


CFtrLayer *CDBAccess::GetNextFtrLayer(long &idx)
{
	if (m_pFtrCls==NULL)
	{
		idx = 0;
		return NULL;
	}
	if (m_pFtrCls->NextRecord())
	{
		CFtrLayer *pLayer = new CFtrLayer;
		if( !pLayer )
		{
			if (m_pFtrCls)delete m_pFtrCls;
			idx = 0;
			return NULL;
		}
		CDpDBVariant var;
		m_pFtrCls->GetFieldValue("FID",var);	
		pLayer->SetName(var.m_pString);
		idx++;
		return pLayer;
	}
	else
	{
		if (m_pFtrCls)delete m_pFtrCls;
		m_pFtrCls = NULL;
		BOOL bFind = FALSE;
		for (int i=m_nCurIndexLayer+1; i<m_listDpLayers.GetSize(); i++)
		{	
			CDpFeatureLayer *pLayer = (CDpFeatureLayer*)m_listDpLayers.GetAt(i);
			if (pLayer)
			{	
				CDpStringArray SelectCol;
				SelectCol.AddString("FID");				
				if(pLayer->QueryFeatureClass(&SelectCol,"DISTINCT",NULL,&m_pFtrCls))
				{
					if(m_pFtrCls->NextRecord())
					{
						m_nCurIndexLayer = i;
						bFind = TRUE;
						break;
					}
				}
				if (m_pFtrCls)delete m_pFtrCls;			
			}
		}
		if (bFind)
		{
			CFtrLayer *pLayer = new CFtrLayer;
			if( !pLayer )
			{
				if (m_pFtrCls)delete m_pFtrCls;
				idx = 0;
				return NULL;
			}
			CDpDBVariant var;
			m_pFtrCls->GetFieldValue("FID",var);	
			pLayer->SetName(var.m_pString);
			idx++;
			return pLayer;
		}
	}
	idx = 0;
	return NULL;
}


BOOL CDBAccess::Attach(LPCTSTR filename)
{
	if( m_pDpDatabase )
		Close();
	
	m_pDpDatabase = new CDpDatabase(_T("00001"));
	if (m_pDpDatabase->ConnectAccess(_T("Admin"),_T("Admin"),filename)==FALSE)
		return FALSE;
	
	m_strReadName = m_strWriteName = filename;
	CDpStringArray	ida;
	CDpStringArray	name;
	CDpStringArray  units;
	
	m_pDpDatabase->GetWksIDAndName(ida,name);
	if (ida.GetCount()<=0) 
		return FALSE;
	
	m_pDpDatabase->GetWksByID(ida.GetItem(0),&m_pDpWks);
	if (m_pDpWks==NULL)
		return FALSE;

	CDpLayer*	pDpLayer		= NULL;
	
	m_pDpWks->GetLayersIDAndName(ida,name);
	
	UINT LayerSum = ida.GetCount();
	
	for (UINT i=0;i<LayerSum;i++)
	{
		pDpLayer = NULL;

		if (!m_pDpWks->GetLayer(ida.GetItem(i),&pDpLayer) || !pDpLayer)
			continue;
		
		DP_LAYER_TYPE LayerType = pDpLayer->GetLayerType();
		if (LayerType!=GEO_Raster&&LayerType!=GEO_NULL)
		{
			m_listDpLayers.Add(pDpLayer);
		}
		else
		{
			delete pDpLayer;
		}
	}

	int nUnit = m_pDpWks->GetUnitCount();
	if( nUnit<=0 )
		return FALSE;

	m_pDpWks->GetUnitsID(units);

	m_pLayerMgr = new CDpUserDefinedGroupMgr();
	if( m_pLayerMgr )
	{
		m_pLayerMgr->Attach(m_pDpWks,units,0);
	}
	
	m_pGroupMgr = new CDpUserDefinedGroupMgr();
	if( m_pGroupMgr )
	{
		m_pGroupMgr->Attach(m_pDpWks,units,1);
	}

	return TRUE;
}


BOOL CDBAccess::Close()
{
	if( m_pLayerRecords )
		delete m_pLayerRecords;
	m_pLayerRecords = NULL;
	
	if( m_pGroupRecords )
		delete m_pGroupRecords;
	m_pGroupRecords = NULL;
	
	if( m_pLayerMgr )
		delete m_pLayerMgr;
	m_pLayerMgr = NULL;
	
	if( m_pGroupMgr )
		delete m_pGroupMgr;
	m_pGroupMgr = NULL;
	
	if( m_pFtrCls )
		delete m_pFtrCls;
	m_pFtrCls = NULL;

	for( int i=m_listDpLayers.GetSize()-1; i>=0; i--)
	{
		CDpFeatureLayer* pLayer = (CDpFeatureLayer*)m_listDpLayers.GetAt(i);
		delete pLayer;
	}
	
	m_listDpLayers.RemoveAll();
	
	if( m_pDpWks )
		delete m_pDpWks;
	m_pDpWks = NULL;

	if( m_pDpDatabase )
		delete m_pDpDatabase;
	m_pDpDatabase = NULL;

	return TRUE;
}

BOOL CDBAccess::OpenRead(LPCTSTR filename)
{
	m_strReadName = filename;
	return Attach(filename);
}

char gstrFileTempl[] = "F:\\Work\\Dynarea\\GeoBuilder_WA\\Source\\src_cur\\FeatureCollector2007\\TestData\\WaTemplate.mdb";

BOOL CDBAccess::OpenWrite(LPCTSTR filename)
{
	m_strWriteName = filename;
	if( _access(filename,0)!=0 )
	{
		if( !::CopyFile(gstrFileTempl,filename,FALSE) )
			return FALSE;
	}
	return Attach(filename);
}


BOOL CDBAccess::CloseRead()
{
	return Close();
}


BOOL CDBAccess::CloseWrite()
{
	return Close();
}

int CodeFromDpCode(int cd)
{
	switch(cd)
	{
	case 10: cd = penMove; break;
	case 20: cd = penLine; break;
	case 30: cd = penSpline; break;
	case 40: cd = penStream; break;
	case 50: 
	case 60: cd = penArc; break;
	case 70: cd = pen3PArc; break;
	default: cd = penLine; break;
	}
	return cd;
}

CFeature* CDBAccess::FeatureFromDpFeature(CDpFeatureLayer *pLayer, CDpFeature *pDpFtr)
{
	int nField = pLayer->GetAttrFieldCount();
	int nClassID = CLS_NULL;
	BOOL bDeleted = FALSE;
	CValueTable tab;
	CVariantEx var;
	tab.BeginAddValueItem();

// 	var = (_variant_t)(LPCTSTR)GUIDToString(pDpFtr->GetOID());
// 	tab.AddValue(FIELDNAME_FTRID,&var);

	CDpCustomFieldInfo info;
	CDpDBVariant dpvar;
	float charWidR,charInterR,LineInterR,charHei;
	CString strFont;
	for( int i=0; i<nField; i++)
	{		
		pLayer->GetAttrFieldInfo(i, &info);

		pDpFtr->GetAttrValue(i, dpvar);
		
		
		_variant_t t;
		DpVar2Var(dpvar,t);
		var = t;

		if (stricmp(info.m_strName,_T("DELETED")) == 0)
		{
			bDeleted = (bool)t;
			// 删除标记为真则不添加该地物
			if (bDeleted)
			{
				return NULL;
			}
		}
		else if (stricmp(info.m_strName,_T("FID")) == 0)
		{
			// 设置当前层
			CString strLayerName = (const char*)(_bstr_t)t;
			
			if (m_pDlgDataSource)
			{
				CFtrLayer *pFtrLayer = m_pDlgDataSource->GetFtrLayer(strLayerName/*,m_strMapName*/);
				if (!pFtrLayer)
				{	
					
					pFtrLayer = m_pDlgDataSource->CreateFtrLayer(strLayerName);
					if (!pFtrLayer)  return NULL;
// 					if (!m_strMapName.IsEmpty())
// 					{
// 						CFtrLayer *pAtt = m_pDlgDataSource->GetLocalFtrLayer(strLayerName);
// 						if (pAtt)
// 						{
// 							CValueTable tab;
// 							tab.BeginAddValueItem();
// 							pAtt->WriteTo(tab);
// 							tab.EndAddValueItem();
// 							pFtrLayer->ReadFrom(tab);
// 							pFtrLayer->SetID(0);
// 							pFtrLayer->SetInherentFlag(FALSE);
// 						}
// 					}
// 					
// 					pFtrLayer->SetMapName(m_strMapName);
					m_pDlgDataSource->AddFtrLayer(pFtrLayer);					
					
				}
				m_nCurLayId = pFtrLayer->GetID();
/*				else if (nClassID != GEO_NULL)
				{
					if( nClassID==CLS_GEOPOINT || nClassID==CLS_GEODIRPOINT || nClassID==CLS_GEOMULTIPOINT || nClassID==DPCLS_GEODEMPOINT)
						strLayerName = StrFromResID(IDS_DEFLAYER_NAMEP);
					else if( nClassID==CLS_GEOCURVE || nClassID==CLS_GEOPARALLEL )
						strLayerName = StrFromResID(IDS_DEFLAYER_NAMEL);
					else if( nClassID==CLS_GEOSURFACE || nClassID==CLS_GEOMULTISURFACE)
						strLayerName = StrFromResID(IDS_DEFLAYER_NAMES);
					else if( nClassID==CLS_GEOTEXT )
						strLayerName = StrFromResID(IDS_DEFLAYER_NAMET);

					pFtrLayer = m_pDlgDataSource->GetFtrLayer(strLayerName);
					if (pFtrLayer)
					{
						m_nCurLayId = pFtrLayer->GetID();
					}
				}
*/			}
		}
		else if (stricmp(info.m_strName,_T("COLOR")) == 0)
		{
			tab.AddValue(FIELDNAME_GEOCOLOR,&var,IDS_FIELDNAME_COLOR);
		}
		else if (stricmp(info.m_strName,_T("CLASSID")) == 0)
		{
			int n = (long)t;
			
			switch(n)
			{				
			case DPCLS_GEOPOINT:
				{
					nClassID = CLS_GEOPOINT;
					break;
				}
			case DPCLS_GEODIRPOINT:
				{
					nClassID = CLS_GEODIRPOINT;
					break;
				}
			case DPCLS_GEOCURVE:
				{
					nClassID = CLS_GEOCURVE;
					break;
				}
			case DPCLS_PARALLELCURVE:	
				{
					nClassID = CLS_GEOPARALLEL;
					break;
				}
			case DPCLS_GEOMULTIPOINT:
				{
					nClassID = CLS_GEOMULTIPOINT;
					break;
				}
			case DPCLS_GEODEMPOINT:	
				{
					nClassID = CLS_GEODEMPOINT;
					break;
				}
			case DPCLS_GEOSURFACE:	
				{
					nClassID = CLS_GEOSURFACE;
					break;
				}
			case DPCLS_GEOMULTISURFACE:	
				{
					nClassID = CLS_GEOMULTISURFACE;
					break;
				}
			case DPCLS_GEOTEXT:	
				{
					nClassID = CLS_GEOTEXT;
					break;
				}
			}
			
			tab.AddValue(FIELDNAME_GEOCLASS,&var,IDS_FIELDNAME_GEOCLASS);
		}
		else if (stricmp(info.m_strName,_T("WIDTH")) == 0)
		{				
			tab.AddValue(FIELDNAME_GEOCURVE_WIDTH,&var,IDS_FIELDNAME_GEOCURVE_WIDTH);
		}
		else if (stricmp(info.m_strName,_T("DIRECTION_ANGLE")) == 0)
		{
			tab.AddValue(FIELDNAME_GEOPOINT_ANGLE,&var,IDS_FIELDNAME_DIRPOINT_ANGLE);
		}
		else if (stricmp(info.m_strName,_T("T_ALIGN_TYPE")) == 0)
		{
			tab.AddValue(FIELDNAME_GEOTEXT_ALIGNTYPE,&var,IDS_FIELDNAME_TEXT_ALIGNTYPE);
		}
		else if (stricmp(info.m_strName,_T("T_SHRUG_ANG")) == 0)
		{
			tab.AddValue(FIELDNAME_GEOTEXT_INCLINEANGLE,&var,IDS_FIELDNAME_TEXT_INCLINEANGLE);
		}
		else if (stricmp(info.m_strName,_T("T_SHRUG_TYPE")) == 0)
		{
			tab.AddValue(FIELDNAME_GEOTEXT_INCLINE,&var,IDS_FIELDNAME_TEXT_INCLINE);
		}
		else if (stricmp(info.m_strName,_T("T_LINE_INTERV")) == 0)
		{
			LineInterR = (double)(_variant_t)var;
	//		tab.AddValue(FIELDNAME_GEOTEXT_LINEINTVS,&var,IDS_FIELDNAME_TEXT_LINEINTVS);
		}
		else if (stricmp(info.m_strName,_T("T_CHAR_INTERV")) == 0)
		{
			charInterR = (double)(_variant_t)var;
//			tab.AddValue(FIELDNAME_GEOTEXT_CHARINTVS,&var,IDS_FIELDNAME_TEXT_CHARINTVS);
		}
		else if (stricmp(info.m_strName,_T("T_LAYOUT_TYPE")) == 0)
		{
			tab.AddValue(FIELDNAME_GEOTEXT_PLACETYPE,&var,IDS_FIELDNAME_TEXT_PLACETYPE);
		}
		else if (stricmp(info.m_strName,_T("T_HEIGHT")) == 0)
		{
			charHei = (double)(_variant_t)var;
			tab.AddValue(FIELDNAME_GEOTEXT_CHARHEIGHT,&var,IDS_FIELDNAME_TEXT_CHARHEIGHT);
		}
		else if (stricmp(info.m_strName,_T("T_WIDTH")) == 0)
		{
			charWidR = (double)(_variant_t)var;
//			tab.AddValue(FIELDNAME_GEOTEXT_CHARWIDTHS,&var,IDS_FIELDNAME_TEXT_CHARWIDTHS);
		}
		else if (stricmp(info.m_strName,_T("T_FONT")) == 0)
		{
			strFont = (LPCTSTR)(_bstr_t)(_variant_t)var;
			tab.AddValue(FIELDNAME_GEOTEXT_FONT,&var,IDS_FIELDNAME_TEXT_FONT);
		}
		else if (stricmp(info.m_strName,_T("T_STR")) == 0)
		{
			tab.AddValue(FIELDNAME_GEOTEXT_CONTENT,&var,IDS_FIELDNAME_TEXT_CONTENT);
		}

		tab.AddValue(info.m_strName,&var);
	}
	if (nClassID==CLS_GEOTEXT)
	{
		HFONT hFont = ::CreateFont(
			charHei,				   // nHeight
			0,                   // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			0,                 // nWeight
			FALSE,           // bItalic
			FALSE,        // bUnderline
			FALSE,        // cStrikeOut
			DEFAULT_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH,  // nPitchAndFamily
			strFont);
		HDC hdc = g_Memdc.GetHdc();
		HFONT oldFont = (HFONT)::SelectObject(hdc,hFont);
		TEXTMETRIC txtMet;
		::GetTextMetrics(hdc,&txtMet);
		::SelectObject(hdc,oldFont);
		::DeleteObject(hFont);
		LineInterR = LineInterR/(txtMet.tmExternalLeading+txtMet.tmHeight);
		charInterR = charInterR/(txtMet.tmAveCharWidth);
		charWidR = charWidR/(txtMet.tmAveCharWidth);		
		var =(_variant_t)charInterR;
		tab.AddValue(FIELDNAME_GEOTEXT_CHARINTVS,&var,IDS_FIELDNAME_TEXT_CHARINTVS);
		var =(_variant_t)LineInterR;
		tab.AddValue(FIELDNAME_GEOTEXT_LINEINTVS,&var,IDS_FIELDNAME_TEXT_LINEINTVS);
		var =(_variant_t)charWidR;
		tab.AddValue(FIELDNAME_GEOTEXT_CHARWIDTHS,&var,IDS_FIELDNAME_TEXT_CHARWIDTHS);
	}
	const CDpShape *pShp = NULL;
	if( pDpFtr->GetShape(&pShp) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		CDpPoint *pts = NULL;
		int nCount = 0;
		PT_3DEX expt;

		pShp->GetPoints(&pts,&nCount);
		arrPts.SetSize(nCount);
		for( int i=0; i<nCount; i++)
		{
			expt.x = pts[i].m_dx;
			expt.y = pts[i].m_dy;
			expt.z = pts[i].m_dz;			
			expt.pencode = CodeFromDpCode(pts[i].m_iExtensionCode);
			arrPts.SetAt(i, expt);
		}

		if (nCount == 2 && arrPts[0].pencode == 0 && arrPts[1].pencode == 0)
		{
			int a = 10;
		}

		var.SetAsShape(arrPts);
		tab.AddValue(FIELDNAME_SHAPE,&var);
	}

	tab.EndAddValueItem();

	CGeometry *pGeo = (CGeometry*)CPermanent::CreatePermanentObject(nClassID);//GCreateGeometry(nClassID);
	if (pGeo == NULL) return NULL;
	CFeature *pFtr = new CFeature;
	if( !pFtr )return NULL;
	pFtr->SetGeometry(pGeo);
	pFtr->ReadFrom(tab);

	pFtr->SetCode(m_strMapName);

	// 将老版本中线结点个数为小于1和面结点个数小于3的地物过滤掉
	CGeometry *pTest = (CGeometry*)CPermanent::CreatePermanentObject(nClassID);//GCreateGeometry(nClassID);
	if (!pTest->ReadFrom(tab))
	{
		delete pFtr;
		pFtr = NULL;
	}
	if (pTest)
	{
		delete pTest;
	}
	
	return pFtr;
}


CDpShape *DpShapeFromPts(PT_3DEX *pts, int npt, DP_LAYER_TYPE type)
{
	CDpShapeData*	pData = new CDpShapeData;
	
	pData->iNumParts	= 1;
	pData->iNumPoints	= npt;
	pData->pParts		= new int[1];
	pData->pPoints	= new CDpPoint[npt];
	pData->pParts[0]	= 0;
	
	for (int i=0; i<npt; i++)
	{
		PT_3DEX pt = pts[i];
		pData->pPoints[i].m_dx = pt.x;
		pData->pPoints[i].m_dy = pt.y;
		pData->pPoints[i].m_dz = pt.z;
		pData->pPoints[i].m_iExtensionCode = pt.pencode;
	}
	
	Envelope enve;
	enve.CreateFromPts(pts,npt,sizeof(PT_3DEX));
	pData->dBound[0]	= enve.m_xl;
	pData->dBound[1]	= enve.m_yl;
	pData->dBound[2]	= enve.m_xh;
	pData->dBound[3]	= enve.m_yh;
	
	CDpShape*	pShape = new CDpShape(type);
	pShape->SetData(pData);

	return pShape;
}


//此处要改，麻烦
CDpFeature* CDBAccess::FeatureToDpFeature(CDpFeatureLayer *pLayer, CFeature *pFtr)
{
	int nField = pLayer->GetAttrFieldCount();
	CDpFeature *pDpFtr = new CDpFeature(nField);
	if( !pDpFtr )return NULL;

	pDpFtr->SetOID(pFtr->GetID().ToGUID());

	CValueTable tab;
	tab.BeginAddValueItem();
	pFtr->WriteTo(tab);
	tab.EndAddValueItem();

	const CVariantEx* var;
	CDpCustomFieldInfo info;
	CDpDBVariant dpvar;
	for( int i=0; i<nField; i++)
	{
		pLayer->GetAttrFieldInfo(i, &info);
		
		if( tab.GetValue(0, info.m_strName, var) )
		{
			_variant_t t = (_variant_t)*var;
			Var2DpVar(t,dpvar);
			pDpFtr->SetAttrValue(i, dpvar);
		}
	}

	int nIdx = pLayer->GetAttrFieldIndex("UDLAY_ID");
	dpvar = (long)m_nCurLayId;
	pDpFtr->SetAttrValue(nIdx,dpvar);

	nIdx = pLayer->GetAttrFieldIndex("GROUP_ID");
	dpvar = (long)m_nCurGrpId;
	pDpFtr->SetAttrValue(nIdx,dpvar);

	if( tab.GetValue(0, FIELDNAME_SHAPE, var) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		if( var->GetShape(arrPts) )
		{
			CDpShape *pShp = DpShapeFromPts(arrPts.GetData(),arrPts.GetSize(),pLayer->GetLayerType());
			if( pShp )
			{
				pDpFtr->SetShapeByRef(pShp);
			}
		}
	}

	return pDpFtr;
}

int CDBAccess::GetFtrLayerCount()
{	
	int nsum = 0;
	
	for (int i=0; i<m_listDpLayers.GetSize(); i++)
	{	
		CDpFeatureLayer *pLayer = (CDpFeatureLayer*)m_listDpLayers.GetAt(i);
		if (pLayer)
		{
			CDpStringArray SelectCol;
			SelectCol.AddString("FID");
			CDpFeatureClass *pFeatureCls = NULL;
			if(pLayer->QueryFeatureClass(&SelectCol,"DISTINCT",NULL,&pFeatureCls))
			{
				while(pFeatureCls->NextRecord())	++nsum;
			}
			if (pFeatureCls)delete pFeatureCls;			
		}
	}
	return nsum;
}

int CDBAccess::GetFeatureCount()
{
	int nsum = 0;
	for (int i=0; i<m_listDpLayers.GetSize(); i++)
	{	
		CDpFeatureLayer *pLayer = (CDpFeatureLayer*)m_listDpLayers.GetAt(i);
		if (pLayer)
		{
			nsum += pLayer->GetRecordCount();
		}
	}
	return nsum;
}

//读取地物
CFeature *CDBAccess::GetFirstFeature(long &idx, CValueTable *exAttriTab)
{
	int nLayer = m_listDpLayers.GetSize();
	if( nLayer<=0 )
		return NULL;

	m_nCurLayer = 0;
	m_nCurFtr = 0;

	while (1)
	{
		CDpFeatureLayer *pLayer = (CDpFeatureLayer*)m_listDpLayers.GetAt(m_nCurLayer);
		
		if( m_pFtrCls )delete m_pFtrCls;
		m_pFtrCls = NULL;
		
		char szWhere[256] = {0};
		pLayer->OpenFeatureClass(NULL,szWhere,NULL,&m_pFtrCls);
		
		if (m_pFtrCls)
		{
			m_nLayIdIdx = m_pFtrCls->GetAttrFieldIndex("UDLAY_ID");
			m_nGrpIdIdx = m_pFtrCls->GetAttrFieldIndex("GROUP_ID");
			
			while (m_pFtrCls->Next())
			{
				CDpFeature *pDpFeature = NULL;
				m_pFtrCls->GetFeature(&pDpFeature);
				
				if( pDpFeature )
				{
					// 根据数据库类标示动态创建对象
					CFeature *pFtr = FeatureFromDpFeature(pLayer,pDpFeature);
					if (!pFtr) continue;

					idx = 1;
					m_nCurFtr++;
					
					CDpDBVariant var;
					if( pDpFeature->GetAttrValue(m_nLayIdIdx,var) )
					{
						m_nCurLayId = (int)var;
					}
					if( pDpFeature->GetAttrValue(m_nGrpIdIdx,var) )
					{
						m_nCurGrpId = (int)var;
					}
					
					delete pDpFeature;
					
					return pFtr;
				}
			}
		}

		m_nCurLayer++;

		if (m_nCurLayer >= m_listDpLayers.GetSize())
		{
			break;
		}

	}

	

	return NULL;
}


CFeature *CDBAccess::GetNextFeature(long &idx, CValueTable *exAttriTab)
{
	int nLayer = m_listDpLayers.GetSize();
	if( nLayer<=0 )
		return NULL;

	if( m_nCurLayer<0 || m_nCurLayer>=nLayer )
		return NULL;
	
	if( !m_pFtrCls )
		return NULL;

	while (1)
	{
		if( !m_pFtrCls->Next() )
		{
			m_nCurLayer++;
			delete m_pFtrCls;
			m_pFtrCls = NULL;
			m_nCurFtr = 0;
			
			if( m_nCurLayer<0 || m_nCurLayer>=nLayer )
				return NULL;
		}
		else
		{
			m_nCurFtr++;
		}
		
		CDpFeatureLayer *pLayer = (CDpFeatureLayer*)m_listDpLayers.GetAt(m_nCurLayer);
		
		if( !m_pFtrCls )
		{
			char szWhere[256] = {0};
			pLayer->OpenFeatureClass(NULL,szWhere,NULL,&m_pFtrCls);
			
			if( !m_pFtrCls )
				return NULL;
			else if( !m_pFtrCls->Next() )
				return GetNextFeature(idx);
			else
			{
				m_nLayIdIdx = m_pFtrCls->GetAttrFieldIndex("UDLAY_ID");
				m_nGrpIdIdx = m_pFtrCls->GetAttrFieldIndex("GROUP_ID");
			}
		}
		
		{
			CDpFeature *pDpFeature = NULL;
			m_pFtrCls->GetFeature(&pDpFeature);
			
			if( pDpFeature )
			{
				// 根据数据库类标示动态创建对象
				CFeature *pFtr = FeatureFromDpFeature(pLayer,pDpFeature);
				if (!pFtr)  continue;
				
				idx++;
				
				CDpDBVariant var;
				if( pDpFeature->GetAttrValue(m_nLayIdIdx,var) )
				{
					m_nCurLayId = (int)var;
				}
				if( pDpFeature->GetAttrValue(m_nGrpIdIdx,var) )
				{
					m_nCurGrpId = (int)var;
				}
				
				delete pDpFeature;
				
				return pFtr;
			}
		}
	}

	
	
	return NULL;
}


int CDBAccess::GetCurFtrLayID()
{
	return m_nCurLayId;
}


int CDBAccess::GetCurFtrGrpID()
{
	return m_nCurGrpId;
}

BOOL CDBAccess::ReadXAttribute(CFeature *pFtr, CValueTable& tab)
{
	return TRUE;
}

//更新层
BOOL CDBAccess::SaveFtrLayer(CFtrLayer *pLayer)
{
/*	if( !m_pLayerMgr || !pLayer )
		return FALSE;

	CDpUserDefinedGroupObject *pObj = FtrLayerToDpObject(m_pLayerMgr,pLayer);
	if( !pObj )
		return FALSE;

	CDpUserDefinedGroupObject *pTmp = m_pLayerMgr->GetGroup(pObj->GetID());
	if( pTmp )
	{
		delete pTmp;
		m_pLayerMgr->UpdateGroup(pObj);
	}
	else
	{
		int nId = 0;
		m_pLayerMgr->AddGroup(pObj,&nId);
		pLayer->SetID(nId);
	}

	delete pObj;
*/
	return TRUE;
}

BOOL CDBAccess::DelXAttributes(CFeature *pFtr)
{
	return TRUE;
}

BOOL CDBAccess::RestoreXAttributes(CFeature *pFtr)
{
	return TRUE;
}

//更新地物
BOOL CDBAccess::SetCurFtrLayID(int id)
{
	m_nCurLayId = id;
	return TRUE;
}


BOOL CDBAccess::SetCurFtrGrpID(int id)
{
	m_nCurGrpId = id;
	return TRUE;
}


CDpFeatureLayer* CDBAccess::FindOwnerDpLayer(CFeature *pFtr)
{
	CGeometry *pGeo = pFtr->GetGeometry();
	if( !pGeo )
		return NULL;

	int ncls = pGeo->GetClassType();
	char strLayName[256] = {0}, strLayName2[256];

	switch( ncls ) 
	{
	case CLS_GEOPOINT:
		strcpy(strLayName,"Layer_LControlPoint");
		break;
	case CLS_GEODIRPOINT:
		strcpy(strLayName,"Layer_LPoint");
		break;
	case CLS_GEOPARALLEL:
	case CLS_GEOCURVE:
		strcpy(strLayName,"Layer_LLine");
		break;
	case CLS_GEOSURFACE:
		strcpy(strLayName,"Layer_LSurface");
		break;
	case CLS_GEOTEXT:
		strcpy(strLayName,"Layer_LText");
		break;
	default:;
	}

	if( strlen(strLayName)<=0 )
		return NULL;

	for( int i=m_listDpLayers.GetSize()-1; i>=0; i--)
	{
		CDpFeatureLayer *pLayer = (CDpFeatureLayer*)m_listDpLayers.GetAt(i);
		if( pLayer && pLayer->GetLayerName(strLayName2,sizeof(strLayName2)) )
		{
			if( strcmp(strLayName,strLayName2)==0 )
				return pLayer;
		}
	}

	return NULL;
}

CFtrLayer *CDBAccess::GetFtrLayerByID(long id)
{
	CFtrLayer *pLayer = NULL;
	for (int i=0;i<m_pDlgDataSource->GetFtrLayerCount();i++)
	{
		pLayer = m_pDlgDataSource->GetFtrLayerByIndex(i);
		if(!pLayer)continue;
		if (pLayer->GetID()==id)
		{
			return pLayer;
		}
	}
	return NULL;
}

BOOL CDBAccess::SaveFeature(CFeature *pFt, CValueTable *exAttriTab)
{
	CDpFeatureLayer *pLayer = FindOwnerDpLayer(pFt);
	if( !pLayer )return FALSE;

	CDpFeature *pDpFtr = FeatureToDpFeature(pLayer, pFt);
	if( !pDpFtr )return FALSE;

	if( pLayer->FindFeature(pFt->GetID().ToGUID()) )
	{
		pLayer->UpdateFeature(pDpFtr);
	}
	else
	{
		pLayer->AddFeature(pDpFtr);
		pFt->SetID(pDpFtr->GetOID());
	}

	delete pDpFtr;

	return TRUE;
}


BOOL CDBAccess::DelFtrLayer(CFtrLayer *pLayer)
{
	if( !m_pLayerMgr )
		return FALSE;

	return m_pLayerMgr->DelGroup(pLayer->GetID());
}


BOOL CDBAccess::DelFeature(CFeature *pFt)
{
	CDpFeatureLayer *pLayer = FindOwnerDpLayer(pFt);
	if( !pLayer )return FALSE;
	
	CDpFeature *pDpFtr = FeatureToDpFeature(pLayer, pFt);
	if( !pDpFtr )return FALSE;
	
	BOOL bRet = pLayer->DeleteFeature(pDpFtr);

	delete pDpFtr;

	return bRet;
}


BOOL CDBAccess::SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx)
{
	return TRUE;
}
