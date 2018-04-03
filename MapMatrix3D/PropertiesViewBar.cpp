// PropertiesViewBar.cpp: implementation of the CPropertiesViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "PropertiesViewBar.h"
#include "SmartViewFunctions.h"
#include "Feature.h "
#include "GeoCurve.h "
#include "UIFBoolProp.h "
#include "UIFLayerProp.h "
#include "editbasedoc.h "
#include "DlgDataSource.h"
#include "UIFByLayerColorProp.h "
#include "GeoText.h "
#include "SymbolLib.h "
#include "Scheme.h "
#include "..\CORE\viewer\EditBase\res\resource.h"

#include "Functions_temp.h"

#include "UIFPropEx.h"
#include "UIParam2.h "

#include "ExMessage.h"
#include "../mm3dPrj/MyDefineDockTabPane.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CPermanentExchanger, CObject)
CPermanentExchanger::CPermanentExchanger(CPermanent *pFtr, int index)
{
	ASSERT(pFtr!=NULL);
	m_arrPFtr.Add(pFtr);
	m_nPtIndex = index;
	//m_nMode = modeSingle;	

}
//为了支持绘制命令初始阶段显示扩展属性，或许可以加一个构造函数，传入层名，通过层名，从方案中或许扩展属性（GetXAttributes）
//但有一问题，我在启动绘制命令后，对扩展属性的修改是否需要反应到方案中（应该不需要），以及如何反应到即将添加的地物中
// 或许可以临时反应到存储扩展属性的方案的内存中，在命令退出后，（除非缺省特征设置被按下（亦进入磁盘），否则内存复原）
CPermanentExchanger::CPermanentExchanger(CArray<CPermanent*,CPermanent*>& arrPFtr, int index)
{
	m_arrPFtr.Copy(arrPFtr);
	m_nPtIndex = index;
/*	m_nMode = modeMultiple;*/
}

CPermanentExchanger::~CPermanentExchanger()
{

}

CPermanentExchanger *CPermanentExchanger:: Clone()
{	
	return new CPermanentExchanger(m_arrPFtr);	
}
BOOL CPermanentExchanger::GetProperties(CValueTable& tab)
{	
	if (m_arrPFtr.GetSize()>0)
	{
		for (int i=0;i<m_arrPFtr.GetSize();i++)
		{
			CPermanent *pFtr = m_arrPFtr[i];
			tab.BeginAddValueItem();
			pFtr->WriteTo(tab);	
			tab.EndAddValueItem();
		}	
		return TRUE;
	}		
	return FALSE;
}
void CPermanentExchanger::OnModifyProperties(CValueTable& tab)
{	
	for (int i=0;i<m_arrPFtr.GetSize();i++)
	{
		m_arrPFtr[i]->ReadFrom(tab,i);
	}		
}

int  CPermanentExchanger::GetObjectCount()
{
	return m_arrPFtr.GetSize();
}

CPermanent* CPermanentExchanger::GetObject(int index)
{
	if (index >= 0 && index < m_arrPFtr.GetSize())
	{
		return m_arrPFtr[index];
	}

	return NULL;
	
}

BOOL CPermanentExchanger::GetXAttributes(CValueTable& tab, BOOL bAll)
{	
	return TRUE;
}
void CPermanentExchanger::OnModifyXAttributes(CValueTable& tab)
{

}


BOOL CPermanentExchanger::IsIncludedObject(CPermanent *pFtr)
{
	for (int i=0;i<m_arrPFtr.GetSize();i++)
	{
		if( pFtr==m_arrPFtr[i] )
			return TRUE;
	}
	return FALSE;
}

void CoverTypeToString(long& value, CString& str, BOOL bOrder)
{
	if( bOrder )
	{
		str = StrFromResID(IDS_COVERTYPE_NONE);
		if (value == COVERTYPE_NONE)
		{
			str = StrFromResID(IDS_COVERTYPE_NONE);
		}
		else if (value == COVERTYPE_RECT)
		{
			str = StrFromResID(IDS_COVERTYPE_RECT);
		}
		else if (value == COVERTYPE_CIRCLE)
		{
			str = StrFromResID(IDS_COVERTYPE_CIRCLE);
		}			
	}
	else
	{
		value = COVERTYPE_NONE;
		if (str.CompareNoCase(StrFromResID(IDS_COVERTYPE_NONE)) == 0)
		{
			value = (long)COVERTYPE_NONE;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_COVERTYPE_RECT)) == 0)
		{
			value = (long)COVERTYPE_RECT;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_COVERTYPE_CIRCLE)) == 0)
		{
			value = (long)COVERTYPE_CIRCLE;
		}
	}
}

void AlignToString(long& nAlign, CString& str, BOOL bOrder)
{
	if( bOrder )
	{
		if (nAlign==TAH_LEFT||(nAlign==(TAH_LEFT|TAV_TOP)))
		{
			str = StrFromResID(IDS_TEXTALIGNLT);
		}
		else if ((nAlign==(TAH_MID|TAV_TOP))||nAlign==TAH_MID)
		{
			str = StrFromResID(IDS_TEXTALIGNMT);
		}
		else if ((nAlign==(TAH_RIGHT|TAV_TOP))||nAlign==TAH_RIGHT)
		{
			str = StrFromResID(IDS_TEXTALIGNRT);
		}
		else if ((nAlign==(TAH_LEFT|TAV_MID))||nAlign==TAV_MID)
		{
			str = StrFromResID(IDS_TEXTALIGNLM);
		}
		else if (nAlign==(TAH_MID|TAV_MID))
		{
			str = StrFromResID(IDS_TEXTALIGNMM);
		}
		else if (nAlign==(TAH_RIGHT|TAV_MID))
		{
			str = StrFromResID(IDS_TEXTALIGNRM);
		}
		else if ((nAlign==(TAH_LEFT|TAV_BOTTOM))||nAlign==TAV_BOTTOM)
		{
			str = StrFromResID(IDS_TEXTALIGNLB);
		}
		else if (nAlign==(TAH_MID|TAV_BOTTOM))
		{
			str = StrFromResID(IDS_TEXTALIGNMB);
		}
		else if (nAlign==(TAH_RIGHT|TAV_BOTTOM))
		{
			str = StrFromResID(IDS_TEXTALIGNRB);
		}	
		else if (nAlign==(TAH_DOT))
		{
			str = StrFromResID(IDS_TEXTALIGN_DOT);
		}
	}
	else
	{
		nAlign = (long)TAH_LEFT|TAV_TOP;

		if (str.CompareNoCase(StrFromResID(IDS_TEXTALIGNLT)) == 0)
		{
			nAlign = (long)TAH_LEFT|TAV_TOP;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_TEXTALIGNMT)) == 0)
		{
			nAlign = (long)TAH_MID|TAV_TOP;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_TEXTALIGNRT)) == 0)
		{
			nAlign = (long)TAH_RIGHT|TAV_TOP;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_TEXTALIGNLM)) == 0)
		{
			nAlign = (long)TAH_LEFT|TAV_MID;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_TEXTALIGNMM)) == 0)
		{
			nAlign = (long)TAH_MID|TAV_MID;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_TEXTALIGNRM)) == 0)
		{
			nAlign = (long)TAH_RIGHT|TAV_MID;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_TEXTALIGNLB)) == 0)
		{
			nAlign = (long)TAH_LEFT|TAV_BOTTOM;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_TEXTALIGNMB)) == 0)
		{
			nAlign = (long)TAH_MID|TAV_BOTTOM;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_TEXTALIGNRB)) == 0)
		{
			nAlign = (long)TAH_RIGHT|TAV_BOTTOM;
		}		
		else if (str.CompareNoCase(StrFromResID(IDS_TEXTALIGN_DOT)) == 0)
		{
			nAlign = (long)TAH_DOT;
		}
	}
}

void AddAlignPropOptions(CUIFProp *pProp)
{
	for (int k=0;k<9;k++ )
	{
		pProp->AddOption(StrFromResID(IDS_TEXTALIGNLT+k));
	}
	pProp->AddOption(StrFromResID(IDS_TEXTALIGN_DOT));
}

void PlaceTypeToString(long& type, CString& str, BOOL bOrder)
{
	if( bOrder )
	{
		if (type==singPt)
		{
			str = StrFromResID(IDS_CMDPLANE_SINGLEPT);
		}
		else if (type==mutiPt)
		{
			str = StrFromResID(IDS_CMDPLANE_MULTIPT);
		}
		else if (type==byLineH)
		{
			str = StrFromResID(IDS_CMDPLANE_BYLINES);
		}
		else if (type==byLineV)
		{
			str = StrFromResID(IDS_CMDPLANE_BYLINES2);
		}
		else if (type==byLineGridH)
		{
			str = StrFromResID(IDS_CMDPLANE_BYLINESGRID);
		}
		else if(type==byLineGridV)
		{
			str = StrFromResID(IDS_CMDPLANE_BYLINES2GRID);
		}
	}
	else
	{
		type = (long)singPt;
		if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_SINGLEPT)) == 0)
		{
			type = (long)singPt;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_MULTIPT)) == 0)
		{
			type = (long)mutiPt;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_BYLINES)) == 0)
		{
			type = (long)byLineH;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_BYLINES2)) == 0)
		{
			type = (long)byLineV;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_BYLINESGRID)) == 0)
		{
			type = (long)byLineGridH;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_BYLINES2GRID)) == 0)
		{
			type = (long)byLineGridV;
		}
	}
}


void AddPlaceTypePropOptions(CUIFProp *pProp)
{
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_SINGLEPT));	
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_MULTIPT));
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_BYLINES));
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_BYLINES2));
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_BYLINESGRID));
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_BYLINES2GRID));
}


void InclinedTypeToString(long& type, CString& str, BOOL bOrder)
{
	if( bOrder )
	{
		if (type==SHRUGN)
		{
			str = StrFromResID(IDS_CMDPLANE_SHRUGN);
		}
		if (type==SHRUGL)
		{
			str = StrFromResID(IDS_CMDPLANE_SHRUGL);
		}
		else if (type==SHRUGR)
		{
			str = StrFromResID(IDS_CMDPLANE_SHRUGR);
		}
		else if (type==SHRUGU)
		{
			str = StrFromResID(IDS_CMDPLANE_SHRUGU);
		}
		else if (type==SHRUGD)
		{
			str = StrFromResID(IDS_CMDPLANE_SHRUGD);
		}
	}
	else
	{
		type = (long)SHRUGN;
		if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_SHRUGN)) == 0)
		{
			type = (long)SHRUGN;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_SHRUGL)) == 0)
		{
			type = (long)SHRUGL;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_SHRUGR)) == 0)
		{
			type = (long)SHRUGR;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_SHRUGU)) == 0)
		{
			type = (long)SHRUGU;
		}
		else if (str.CompareNoCase(StrFromResID(IDS_CMDPLANE_SHRUGD)) == 0)
		{
			type = (long)SHRUGD;
		}
	}
}


void AddInclinedTypePropOptions(CUIFProp *pProp)
{
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGN));
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGL));
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGR));
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGU));
	pProp->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGD));
}


/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

int CALLBACK EnumFontFamProc(LPENUMLOGFONT lpelf, LPNEWTEXTMETRIC lpntm, DWORD nFontType, LONG_PTR lparam);
//##ModelId=41466B7C00DA
CPropertiesViewBar::CPropertiesViewBar()
{
	m_nPtIndex = 0;
	m_pFtrExchanger =NULL;
}

//##ModelId=41466B7C010D
CPropertiesViewBar::~CPropertiesViewBar()
{
}


BEGIN_MESSAGE_MAP(CPropertiesViewBar, CDockablePane)
	//{{AFX_MSG_MAP(CPropertiesViewBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_SORTINGPROP, OnSortingprop)
	ON_UPDATE_COMMAND_UI(ID_SORTINGPROP, OnUpdateSortingprop)
	ON_COMMAND(ID_PROPERIES1, OnProperies1)
	ON_UPDATE_COMMAND_UI(ID_PROPERIES1, OnUpdateProperies1)
	ON_COMMAND(ID_PROPERIES2, OnProperies2)
	ON_UPDATE_COMMAND_UI(ID_PROPERIES2, OnUpdateProperies2)
	ON_COMMAND(ID_EXPAND, OnExpand)
	ON_UPDATE_COMMAND_UI(ID_EXPAND, OnUpdateExpand)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_CTLCOLOR()
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

//##ModelId=41466B7C00DB
void CPropertiesViewBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect (rectClient);
/*
	m_wndObjectCombo.GetWindowRect (&rectCombo);

	int cyCmb = rectCombo.Size ().cy;
	int cyTlb = 0;

	m_wndObjectCombo.SetWindowPos (NULL,
								   rectClient.left, 
								   rectClient.top,
								   rectClient.Width (),
								   200,
								   SWP_NOACTIVATE | SWP_NOZORDER);
*/
	int cyCmb = 0;
	int cyTlb = 0;

	m_wndPropList.SetWindowPos (NULL,
		rectClient.left + 1, 
		rectClient.top + cyCmb + cyTlb + 1, 
		rectClient.Width () - 2,
		rectClient.Height () - (cyCmb+cyTlb) - 2,
		SWP_NOACTIVATE | SWP_NOZORDER);

//	m_wndPropList.AdjustLayout();
}


CString PencodeToText(int pencode)
{
	CString strStyle;
	switch(pencode) 
	{
	case penNone:
		strStyle = "NONE";
		break;
	case penMove:
		strStyle = "MOVE";
		break;
	case penLine:
		strStyle = StrFromResID(IDS_CMDTIP_LINE);
		break;
	case penSpline:
		strStyle = StrFromResID(IDS_CMDTIP_CURVE);
		break;
	case penArc:
		strStyle = StrFromResID(IDS_CMDTIP_ARC);
		break;
	case penStream:
		strStyle = StrFromResID(IDS_CMDTIP_SYNCH);
		break;
	case pen3PArc:
		strStyle = StrFromResID(IDS_CMDTIP_3ARC);
		break;
	default:;
	}

	return strStyle;
}


int TextToPencode(CString& strStyle)
{
	int code = penNone;

	if (strStyle.CompareNoCase(StrFromResID(IDS_CMDTIP_LINE)) == 0)
	{
		code = penLine;
	}
	else if (strStyle.CompareNoCase(StrFromResID(IDS_CMDTIP_CURVE)) == 0)
	{
		code = penSpline;
	}
	else if (strStyle.CompareNoCase(StrFromResID(IDS_CMDTIP_ARC)) == 0)
	{
		code = penArc;
	}
	else if (strStyle.CompareNoCase(StrFromResID(IDS_CMDTIP_SYNCH)) == 0)
	{
		code = penStream;
	}
	else if (strStyle.CompareNoCase(StrFromResID(IDS_CMDTIP_3ARC)) == 0)
	{
		code = pen3PArc;
	}

	return code;
}


CString PurposeToText( int nPurpose)
{
	CString str;
	if (nPurpose == FTR_COLLECT)
	{
		str.Format(IDS_PURPOSE_COLLECT);
	}
	else if (nPurpose == FTR_MARK)
	{
		str.Format(IDS_PURPOSE_MARK);
	}
	else if (nPurpose == FTR_EDB)
	{
		str.Format(IDS_PURPOSE_EDB);
	}
	return str;
}

int TextToPurpose(CString& strText)
{
	int code = 0;
	
	if (strText.CompareNoCase(StrFromResID(IDS_PURPOSE_COLLECT)) == 0)
	{
		code = FTR_COLLECT;
	}
	else if (strText.CompareNoCase(StrFromResID(IDS_PURPOSE_MARK)) == 0)
	{
		code = FTR_MARK;
	}
	else if (strText.CompareNoCase(StrFromResID(IDS_PURPOSE_EDB)) == 0)
	{
		code = FTR_EDB;
	}
	return code;
}

CString XdefineTypeToString(int valuetype)
{
	CString strType;
	switch(valuetype)
	{
	case DP_CFT_BOOL: 
		strType.LoadString(IDS_BOOL_TYPE);
		break;
	case DP_CFT_COLOR:
		strType.LoadString(IDS_COLOR_TYPE);
		break;
	case DP_CFT_VARCHAR:
		strType.LoadString(IDS_CHAR_TYPE);
		break;
	case DP_CFT_DOUBLE:
		strType.LoadString(IDS_DOUBLE_TYPE);
		break;
	case DP_CFT_INTEGER:
		strType.LoadString(IDS_LONG_TYPE);
		break;
	case DP_CFT_DATE:
		strType.LoadString(IDS_DATE_TYPE);
		break;
	case DP_CFT_FLOAT:
		strType.LoadString(IDS_SINGLE_TYPE);
		break;
	case DP_CFT_SMALLINT:
		strType.LoadString(IDS_SMALLINT_TYPE);
		break;
	default:
		break;		
		
	}

	return strType;
}


void CPropertiesViewBar::SetPermaExchanger(CPermanentExchanger *pFtrExchanger)
{
	if (pFtrExchanger==NULL||pFtrExchanger->m_arrPFtr.GetSize()==0)
	{
		ClearPermaExchanger();
		return;
	}
	m_pFtrExchanger = pFtrExchanger;
// 	if(!m_pFtrExchanger)m_pFtrExchanger = pFtrExchanger;
// 	if (m_pFtrExchanger!=NULL&&m_pFtrExchanger!=pFtrExchanger)
// 	{
// 		ClearPermaExchanger();
// 	}
//	m_pFtrExchanger = pFtrExchanger->Clone();


	CValueTable tab;
	const CVariantEx *pVar; 
	int type;
	CString field,name;
	CDlgDoc * pDoc = GetActiveDlgDoc();

	m_wndPropList.RemoveAll();
	m_nPtIndex = pFtrExchanger->GetCurPtPos();
	m_arrPts.RemoveAll();
	m_pFtrExchanger->GetProperties(tab);
	m_Tmptab.DelAll();
	m_Tmptab.BeginAddValueItem();
	int valueIndex = 0;

	//基本属性
	CUIFProp *pGroup = new CUIFProp (StrFromResID(IDS_PROPERTIES_BASE));

	//地物的层信息 
	CDlgDataSource *pDS = pDoc->GetDlgDataSourceOfFtr((CPFeature)m_pFtrExchanger->m_arrPFtr[0]);
	CFtrLayer *pLayer = NULL;
	if( pDS )
		pLayer = pDS->GetFtrLayerOfObject((CPFeature)m_pFtrExchanger->m_arrPFtr[0]);
	else
		pDS = pDoc->GetDlgDataSource();
	
	CConfigLibManager *pConfigMag = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	CScheme *pScheme = pConfigMag->GetScheme(pDS->GetScale());
	
	if (pLayer)
	{
		CString strshow;
		__int64 strLayCode;
		BOOL bFlag = FALSE;
		if (pScheme)
		{
			CString ch = pLayer->GetName();
			bFlag = pScheme->FindLayerIdx(FALSE,strLayCode,ch);
		}
// 		if(bFlag)
// 			strshow.Format("%s(%I64d)",pLayer->GetName(),strLayCode);
// 		else
			strshow.Format("%s",pLayer->GetName());
		CUIFLayerProp *pProp = new CUIFLayerProp(StrFromResID(IDS_LAYER_NAME),LPCTSTR(strshow));
		pProp->SetLayersOption(pDS);
		pProp->SetData(valueIndex++);
		pGroup->AddSubItem(pProp);		
		m_Tmptab.AddValue(FIELDNAME_LAYERNAME,&CVariantEx((_variant_t)(_bstr_t)(LPCTSTR)(strshow)));

		CString strCode;
		if(bFlag)
		{
			strCode.Format("%I64d",strLayCode);
		}
		CUIFProp *pProp1 = new CUIFProp(StrFromResID(IDS_LAYMAN_LAYERCODE), (COleVariant)(LPCTSTR)(strCode));
		if(!pProp) return;
		pProp1->Enable(TRUE);
		pProp1->SetData(valueIndex++);	
		//pProp1->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp1);	
		m_Tmptab.AddValue(FIELDNAME_LAYERCODE,&CVariantEx((_variant_t)(_bstr_t)(LPCTSTR)(strCode)));
	
// 		CUIFProp *pMapProp = new CUIFProp(StrFromResID(IDS_FIELDNAME_MAPNAME),pLayer->GetMapName(),StrFromResID(IDS_FIELDNAME_MAPNAME));
// 		pMapProp->Enable(TRUE);
// 		pMapProp->AllowEdit(FALSE);
// 					
// 		pGroup->AddSubItem(pMapProp);

		CString DBName;
		if(pScheme)
		{
			CSchemeLayerDefine *schemelayer = pScheme->GetLayerDefine(pLayer->GetName());
			if(schemelayer)
			{
				DBName = schemelayer->GetDBLayerName();
			}
		}
		CUIFProp *pDBNameProp = new CUIFProp(StrFromResID(IDS_DBLAYERNAME), (COleVariant)(LPCTSTR)(DBName));
		if(pDBNameProp)
		{
			pDBNameProp->Enable(TRUE);
			pDBNameProp->AllowEdit(FALSE);
			pGroup->AddSubItem(pDBNameProp);	
		}
	}		
	tab.DelField(FIELDNAME_FTRDELETED);
	tab.DelField(FIELDNAME_FTRID);
	tab.DelField(FIELDNAME_CLSTYPE);
	tab.DelField(FIELDNAME_FTRDISPLAYORDER);
	tab.DelField(FIELDNAME_FTRGROUPID);
//	tab.DelField(FIELDNAME_GEOTEXT_ALIGNTYPE);
//	tab.DelField(FIELDNAME_GEOTEXT_PLACETYPE);
//		tab.DelField(FIELDNAME_SYMBOLNAME);
	BOOL bShowNodeWid = TRUE;
	int GeoType = CLS_NULL;

	for(int i=0;i<tab.GetFieldCount();i++)
	{
		tab.GetField(i,field,type,name);
		tab.GetValue(0,i,pVar);
		BOOL bAdd = FALSE;

		if (strcmp((LPCTSTR)field,FIELDNAME_GEOCLASS)==0)
		{
			GeoType = (long)(_variant_t)*pVar;
			CString str = CPermanent::GetPermanentName(GeoType);
			
			if (GeoType == CLS_GEOPOINT || GeoType == CLS_GEODIRPOINT || GeoType == CLS_GEOMULTIPOINT ||
				GeoType == CLS_GEODEMPOINT || GeoType == CLS_GEOTEXT || GeoType == CLS_GEOSURFACEPOINT )
			{
				bShowNodeWid = FALSE;
			}
			
			CUIFProp *pProp = new CUIFProp (name,(LPCSTR)str, field);
			if(!pProp)continue;
			//bAdd = TRUE;
			pProp->SetData(valueIndex++);
			pProp->AllowEdit(FALSE);
			pGroup->AddSubItem(pProp);
			m_Tmptab.AddValue((LPCTSTR)field,(CVariantEx*)pVar,(LPCTSTR)name);

			if ( tab.GetValue(0,FIELDNAME_SHAPE,pVar) )
			{
				pVar->GetShape(m_arrPts);

				if (m_arrPts.GetSize() > 1)
				{
					CString strStyle;
					switch(m_arrPts[1].pencode) 
					{
					case penLine:
						strStyle = StrFromResID(IDS_CMDTIP_LINE);
						break;
					case penSpline:
						strStyle = StrFromResID(IDS_CMDTIP_CURVE);
						break;
					case penArc:
						strStyle = StrFromResID(IDS_CMDTIP_ARC);
						break;
					case penStream:
						strStyle = StrFromResID(IDS_CMDTIP_SYNCH);
						break;
					case pen3PArc:
						strStyle = StrFromResID(IDS_CMDTIP_3ARC);
						break;
					default:;
					}

					CUIFProp *pPropLine = new CUIFLongNamePropEx (StrFromResID(IDS_CMDPLANE_LINETYPE),strStyle);
					if(!pPropLine)continue;
					//bAdd = TRUE;
					pPropLine->SetData(valueIndex++);
					pPropLine->AllowEdit(FALSE);
					pPropLine->AddOption(StrFromResID(IDS_CMDTIP_LINE));
					pPropLine->AddOption(StrFromResID(IDS_CMDTIP_CURVE));
					pPropLine->AddOption(StrFromResID(IDS_CMDTIP_ARC));
					pPropLine->AddOption(StrFromResID(IDS_CMDTIP_3ARC));
					pPropLine->AddOption(StrFromResID(IDS_CMDTIP_SYNCH));
					pGroup->AddSubItem(pPropLine);
					m_Tmptab.AddValue((LPCTSTR)StrFromResID(IDS_CMDPLANE_LINETYPE),&CVariantEx((_variant_t)(_bstr_t)(LPCTSTR)(strStyle)),(LPCTSTR)StrFromResID(IDS_CMDPLANE_LINETYPE));
				}

			}

		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_SYMBOLNAME)==0)
		{
			CUIFCellLinetypeProp *pProp = new CUIFCellLinetypeProp(name,(CString)(LPCTSTR)(_bstr_t)(_variant_t)*pVar,field);
			if(!pProp)continue;

			int nSymType = 1;
			if( ((CPFeature)m_pFtrExchanger->m_arrPFtr[0])->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			{
				nSymType = 0;
			}

			pProp->SetSymbolClass(pDS->GetScale(),nSymType);

			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);

			pProp->AllowEdit(TRUE);
			pGroup->AddSubItem(pProp);
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOCOLOR)==0)
		{
			BOOL data = FALSE;
			COLORREF color;

			color = COLORREF((long)(_variant_t)*pVar);

			CUIFByLayerColorProp *pProp = new CUIFByLayerColorProp(name, color, NULL, field); 
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->SetData(valueIndex++);
			pProp->SetByLayerFlag(data);
			pProp->EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject((CPFeature)m_pFtrExchanger->m_arrPFtr[0]);
			if(pLayer)
				color = pLayer->GetColor();
			else
				color = pDS->GetCurFtrLayer()->GetColor();
			pProp->EnableAutomaticButton(_T("ByLayer"), color);	
			pGroup->AddSubItem(pProp);
			
			CFeature *pFtr = (CPFeature)m_pFtrExchanger->m_arrPFtr[0];
			CGeometry *pGeo = pFtr->GetGeometry();
			if(pGeo && pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				CGeoCurveBase *pGeoCB = (CGeoCurveBase*)pGeo;
				COLORREF color1 = pGeoCB->GetFillColor();
				BOOL bFill = pGeoCB->IsFillColor();
				if(bFill && color1==COLOUR_BYLAYER)
				{
					if(pLayer)
						color1 = pLayer->GetColor();
					else
						color1 = pDS->GetCurFtrLayer()->GetColor();
				}
				//填充颜色
				CUIFFillColorProp *pProp1  = new CUIFFillColorProp(StrFromResID(IDS_FILL_COLOR),color1);
				if(pProp1)
				{
					pProp1->SetData(-1);
					
					pProp1->SetFillFlag(bFill);
					pProp1->EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
					pProp1->EnableAutomaticButton(_T("NoFillColor"), 0);	
					pGroup->AddSubItem(pProp1);
				}
				//透明度
				long transparency = pGeoCB->GetTransparency();
				CString strTransparency;
				strTransparency.Format("%d", transparency);
				CUIFProp *pProp2 = new CUIFProp(StrFromResID(IDS_TRANSPARENCY), (COleVariant)(LPCTSTR)(strTransparency));
				if(pProp2)
				{
					pProp2->Enable(TRUE);
					pProp2->SetData(-1);
					pGroup->AddSubItem(pProp2);
				}
			}
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_COVERTYPE)==0)
		{
			long nCoverType = (long)(_variant_t)*pVar;
			CString str;
			CoverTypeToString(nCoverType,str,TRUE);

			CUIFProp *pProp = new CUIFLongNamePropEx(name,str,field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->AddOption(StrFromResID(IDS_COVERTYPE_NONE));
			pProp->AddOption(StrFromResID(IDS_COVERTYPE_RECT));
			pProp->AddOption(StrFromResID(IDS_COVERTYPE_CIRCLE));
			pProp->AllowEdit(FALSE);
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);		
			pGroup->AddSubItem(pProp);
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_FTRPURPOSE)==0)
		{
			long nPurpose = (long)(_variant_t)*pVar;
			CString str;
			if (nPurpose == FTR_COLLECT)
			{
				str.Format(IDS_PURPOSE_COLLECT);
			}
			else if (nPurpose == FTR_MARK)
			{
				str.Format(IDS_PURPOSE_MARK);
			}
			else if (nPurpose == FTR_EDB)
			{
				str.Format(IDS_PURPOSE_EDB);
			}
			
			CUIFProp *pProp = new CUIFLongNamePropEx(name,str,field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->AddOption(StrFromResID(IDS_PURPOSE_COLLECT));
			pProp->AddOption(StrFromResID(IDS_PURPOSE_MARK));
			pProp->AddOption(StrFromResID(IDS_PURPOSE_EDB));
			pProp->AllowEdit(FALSE);
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);		
			pGroup->AddSubItem(pProp);
		}
		else if( strcmp((LPCTSTR)field,FIELDNAME_SYMBOLIZEFLAG)==0 )
		{
			CUIFProp *pProp = new CUIFSymbolizeFlagProp(name,(long)(_variant_t)*pVar,field);
			if(!pProp)continue;	
			
			pProp->AllowEdit(FALSE);
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);
			bAdd = TRUE;
			pGroup->AddSubItem(pProp);

		}
		else if ( strcmp((LPCTSTR)field,FIELDNAME_GEOSURFACE_CELLANGLE)==0 )
		{
			float ang = (_variant_t)*pVar;

			CUIFProp *pProp = new CUIFProp(name,(_variant_t)(float)(180/PI*ang),field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);		
			pGroup->AddSubItem(pProp);
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_CONTENT)==0)
		{
			CUIFProp *pProp = new CUIFMultiEditProp(name,(CString)(LPCTSTR)(_bstr_t)(_variant_t)*pVar,field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);		
			pGroup->AddSubItem(pProp);		
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_FONT)==0)
		{
			CUIFFontNameProp *pProp = new CUIFFontNameProp(name,(LPCTSTR)(_bstr_t)(_variant_t)*pVar,field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->LoadFontNames();
			pProp->SetData(valueIndex++);		
			pGroup->AddSubItem(pProp);
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_CHARWIDTHS)==0)
		{
			CUIFProp *pProp = new CUIFProp(name,(_variant_t)*pVar,field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);	
			pGroup->AddSubItem(pProp);
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_CHARHEIGHT)==0)
		{
			CUIFProp *pProp = new CUIFProp(name,(_variant_t)*pVar,field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);		
			pGroup->AddSubItem(pProp);
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_CHARINTVS)==0)
		{
			CUIFProp *pProp = new CUIFProp(name,(_variant_t)*pVar,field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);	
			pGroup->AddSubItem(pProp);
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_LINEINTVS)==0)
		{
			CUIFProp *pProp = new CUIFProp(name,(_variant_t)*pVar,field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);
			pGroup->AddSubItem(pProp);
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_ALIGNTYPE)==0)
		{
			CUIFProp *pProp ;
			CString str;
			long lv = (long)(_variant_t)*pVar;
			AlignToString(lv,str,TRUE);
			pProp = new CUIFLongNamePropEx(name,str,field);
			if( pProp )
			{
				bAdd = TRUE;
				AddAlignPropOptions(pProp);

				pProp->AllowEdit(FALSE);
				pProp->SetData(valueIndex++);
				pGroup->AddSubItem (pProp);
			}
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_PLACETYPE)==0)
		{
			CUIFProp *pProp ;
			CString str;// = (const char*)(_bstr_t)(_variant_t)*pVar;

			long lv = (long)(_variant_t)*pVar;
			PlaceTypeToString(lv,str,TRUE);
			pProp = new CUIFLongNamePropEx (name,str, field);
			if( pProp )
			{
				bAdd = TRUE;
				AddPlaceTypePropOptions(pProp);

				pProp->AllowEdit(FALSE);
				pProp->SetData(valueIndex++);
				pGroup->AddSubItem (pProp);
			}				
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_INCLINE)==0)
		{		
			CUIFProp *pProp ;
			CString str;// = (const char*)(_bstr_t)(_variant_t)*pVar;
			long lv = (long)(_variant_t)*pVar;
			InclinedTypeToString(lv,str,TRUE);

			pProp = new CUIFLongNamePropEx(name,str,field);
			if( pProp )
			{
				bAdd = TRUE;
				AddInclinedTypePropOptions(pProp);
				
				pProp->AllowEdit(FALSE);
				pProp->SetData(valueIndex++);
				pGroup->AddSubItem (pProp);
				//					pProp->SetCurSelOfCombo(item.data.bValue?0:1);
			}
/*			pProp = new CUIFProp(name,(LPCTSTR)str,name);
			pProp->Enable(TRUE);
			pProp->SetData(i);			
			pGroup->AddSubItem(pProp);
*/		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_INCLINEANGLE)==0)
		{
			CUIFProp *pProp = new CUIFProp(name,(_variant_t)*pVar,field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);	
			pGroup->AddSubItem(pProp);
		}
		else if (strcmp((LPCTSTR)field,FIELDNAME_GEOTEXT_OTHERFLAG)==0)
		{
			TEXT_SETTINGS0 ts;
			ts.nOtherFlag = (long)(_variant_t)*pVar;
			CUIFProp *pProp = new CUIFBoolProp(StrFromResID(IDS_BOLD),ts.IsBold(),field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex++);	
			pGroup->AddSubItem(pProp);

			pProp = new CUIFBoolProp(StrFromResID(IDS_UNDERLINE),ts.IsUnderline(),field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->SetData(valueIndex/*++*/);	
			pGroup->AddSubItem(pProp);

		}		// 几何信息
		else if (strcmp((LPCTSTR)field,FIELDNAME_SHAPE) == 0)
		{
//			valueIndex++;

			pVar->GetShape(m_arrPts);		

			if (m_nPtIndex>=0 && m_nPtIndex < m_arrPts.GetSize())
			{	
				CUIFProp *pProp = new CUIFProp(StrFromResID(IDS_PTSNUM), (COleVariant)long(m_arrPts.GetSize()));
				pProp->AllowEdit(FALSE);
				pGroup->AddSubItem (pProp);

				PT_3DEX pt = m_arrPts.GetAt(m_nPtIndex);
				
				CUIFProp *pNewProp = NULL;

				pNewProp = new CUIFArrowProp(StrFromResID(IDS_PTINDEX), (COleVariant)(long)m_nPtIndex);
				if( pNewProp )
				{					
					pNewProp->AllowEdit(TRUE);
					pGroup->AddSubItem (pNewProp);
				} 	

				CString str;
				str.Format("%.4f",pt.x);
				pNewProp = new CUIFProp(_T("X"), (COleVariant)(str));
				if( pNewProp )
				{
					pNewProp->AllowEdit(FALSE);
					pGroup->AddSubItem (pNewProp);
				}

				str.Format("%.4f",pt.y);
				pNewProp = new CUIFProp(_T("Y"), (COleVariant)(str));
				if( pNewProp )
				{
					pNewProp->AllowEdit(FALSE);
					pGroup->AddSubItem (pNewProp);
				}

				str.Format("%.4f",pt.z);
				pNewProp = new CUIFProp(_T("Z"), (COleVariant)str);
				if( pNewProp )
				{
					pNewProp->AllowEdit(FALSE);
					pGroup->AddSubItem (pNewProp);
				}

				CString strStyle = PencodeToText(pt.pencode);

				pNewProp = new CUIFProp(StrFromResID(IDS_PTTYPE), (COleVariant)strStyle);
				if( pNewProp )
				{
					pNewProp->AllowEdit(FALSE);
					pGroup->AddSubItem (pNewProp);
				}
				
				if (bShowNodeWid)
				{
					str.Format("%.4f",pt.wid);
					
					pNewProp = new CUIFProp(StrFromResID(IDS_CMDTIP_NODEWID), (COleVariant)str);
					if( pNewProp )
					{
						pNewProp->AllowEdit(TRUE);
						pGroup->AddSubItem (pNewProp);
					}
				}	
					
				pNewProp = new CUIFNodeTypeProp(StrFromResID(IDS_CMDTIP_NODETYPE), pt.type);
				if( pNewProp )
				{
					pNewProp->AllowEdit(TRUE);
					pGroup->AddSubItem (pNewProp);
				}
					
			}
			else
			{
				PT_3DEX pt;
				CString str;
				CUIFProp *pNewProp = NULL;

				if (bShowNodeWid)
				{
					str.Format("%.4f",pt.wid);
					
					pNewProp = new CUIFProp(StrFromResID(IDS_CMDTIP_NODEWID), (COleVariant)str);
					if( pNewProp )
					{
						pNewProp->AllowEdit(TRUE);
						pGroup->AddSubItem (pNewProp);
					}
				}	
				
				pNewProp = new CUIFNodeTypeProp(StrFromResID(IDS_CMDTIP_NODETYPE), pt.type);
				if( pNewProp )
				{
					pNewProp->AllowEdit(TRUE);
					pGroup->AddSubItem (pNewProp);
				}
			}
		}
		else if( stricmp((LPCTSTR)field,FIELDNAME_FTRMODIFYTIME)==0 )
		{
			long time0 = (long)(_variant_t)*pVar;
			CTime time1(time0);
			CString strTime = time1.Format("%Y-%m-%d,%H:%M:%S");

			CUIFProp *pProp = new CUIFProp(StrFromLocalResID(IDS_FIELDNAME_MODIFYTIME),(LPCTSTR)strTime,field);
			if(!pProp)continue;
			bAdd = TRUE;
			pProp->Enable(TRUE);
			pProp->AllowEdit(FALSE);
			pProp->SetData(valueIndex++);	
			pGroup->AddSubItem(pProp);
		}
		else
		{
			CString str;
			float  fVal;
			double lfVal;
			CUIFProp *pProp;
			switch(type)
			{			
			case VT_R4:	 
				fVal = (float)(_variant_t)*pVar;
				//str.Format("%.4f",fVal);
				pProp = new CUIFProp(name,(_variant_t)fVal,field,i);
				break;
			case VT_R8:	
				lfVal = (double)(_variant_t)*pVar;
				//str.Format("%.4f",fVal);
				pProp = new CUIFProp(name,(_variant_t)lfVal,field,i);
				break;
			case VT_BSTR:
				pProp = new CUIFProp(name,(_variant_t)*pVar,field,i);
				break;
			case VT_I2:			
			case VT_I4:
				pProp = new CUIFProp(name,(long)(_variant_t)*pVar,field,i);
				break;
			case VT_BOOL:
				pProp = new CUIFBoolProp(name,(long)(_variant_t)*pVar,field);
				break;	
			default:
				break;
			}
			if (pProp)
			{
				bAdd = TRUE;
				pProp->SetData(valueIndex++);
				pProp->AllowEdit(TRUE);
				pGroup->AddSubItem(pProp);
			}			
		}
		if (bAdd)
		{
			m_Tmptab.AddValue((LPCTSTR)field,(CVariantEx*)pVar,(LPCTSTR)name);
		}
	}
	m_wndPropList.AddProperty(pGroup);

	if( pDoc )
	{
		AfxGetMainWnd()->PostMessage(FCCM_INITLAYERTOOL, WPARAM(4), LPARAM(pLayer));
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(pDoc));
	}
	
	//.............扩展属性
	if(!pLayer) return;
	tab.DelAll();
	if(m_pFtrExchanger->GetXAttributes(tab))
	{
		pGroup = new CUIFProp (StrFromResID(IDS_PROPERTIES_EXTEND));

		CSchemeLayerDefine *schemelayer = (pScheme==NULL?NULL:pScheme->GetLayerDefine(pLayer->GetName()));
		
		for(i=0;i<tab.GetFieldCount();i++)
		{
			tab.GetField(i,field,type,name);
			tab.GetValue(0,i,pVar);

			if (name.IsEmpty())
			{
				name = field;
			}

			CString strType, descr;

			if(schemelayer)
			{
				int size = 0;
				const XDefine *defs = schemelayer->GetXDefines(size);
				for(int j=0; j<size; j++)
				{
					if( stricmp(defs[j].field,field)==0 )
					{
						if(defs[j].isMust>=0)
							strType = XdefineTypeToString(defs[j].valuetype);
						break;
					}
				}
			}
			else
			{
				switch(type)
				{		
				case VT_R4:					
					strType = "Float";
					break;
				case VT_R8:	
					strType = "Double";
					break;
				case VT_BSTR:
					strType = "String";
					break;
				case VT_BOOL:
					strType = "Bool";
					break;
				case VT_I2:	
					strType = "Short";
					break;	
				case VT_I4:	
					strType = "Long";
					break;	
				default:
					break;
				}				
			}

			if(strType.IsEmpty()) continue;

			descr = field + ", " + strType;

			CString str;
			double fVal;
			CUIFProp *pProp;
			switch(type)
			{		
			case VT_R4:	    
				fVal = (float)(_variant_t)*pVar;
				str.Format("%.4f",fVal);
				pProp = new CUIFProp(name,(_variant_t)str,descr,i);
				break;
			case VT_R8:	
				fVal = (double)(_variant_t)*pVar;
				str.Format("%.4f",fVal);
				pProp = new CUIFProp(name,(_variant_t)str,descr,i);
				break;
			case VT_BSTR:
				pProp = new CUIFProp(name,(_variant_t)*pVar,descr,i);
				break;
			case VT_BOOL:
				pProp = new CUIFBoolProp(name,(long)(_variant_t)*pVar,descr);
				break;
			case VT_I2:		
				pProp = new CUIFProp(name,(long)(short)(_variant_t)*pVar,descr);
				break;	
			case VT_I4:	
				pProp = new CUIFProp(name,(long)(_variant_t)*pVar,descr);
				break;	
			default:
				break;
			}
			if (pProp)
			{
				m_Tmptab.AddValue((LPCTSTR)field,(CVariantEx*)pVar,(LPCTSTR)name);
				pProp->SetData(valueIndex++);
				pProp->AllowEdit(TRUE);
				pGroup->AddSubItem(pProp);
			}			
		}
		m_wndPropList.AddProperty(pGroup);
	}	

	// 其他属性：长度，面积，闭合状态
	CGeoCurveBase *pCurveBase = NULL;
	if( m_pFtrExchanger->m_arrPFtr.GetSize()>0 )
	{
		CGeometry *pObj = ((CPFeature)m_pFtrExchanger->m_arrPFtr[0])->GetGeometry();
		if( pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			pCurveBase = (CGeoCurveBase*)pObj;
	}
	if( pCurveBase!=NULL && m_arrPts.GetSize()>1 )
	{
		BOOL bClosed = pCurveBase->IsClosed();
		double len = pCurveBase->GetLength();
		double area = pCurveBase->GetArea();

		pGroup = new CUIFProp (StrFromResID(IDS_PROPERTIES_OTHERS));

		CUIFProp *pProp = new CUIFProp(StrFromResID(IDS_SELCOND_LEN), (COleVariant)len);
		pProp->Enable(TRUE);
		pProp->AllowEdit(FALSE);
		pProp->SetData(valueIndex++);	
		pGroup->AddSubItem(pProp);

		pProp = new CUIFProp(StrFromResID(IDS_SELCOND_AREA), (COleVariant)area);
		pProp->Enable(TRUE);
		pProp->AllowEdit(FALSE);
		pProp->SetData(valueIndex++);	
		pGroup->AddSubItem(pProp);

		pProp = new CUIFBoolProp(StrFromResID(IDS_FIELDNAME_CURVE_CLOSED),bClosed);
		pProp->Enable(FALSE);
		pProp->SetData(valueIndex++);	
		pGroup->AddSubItem(pProp);

		m_wndPropList.AddProperty(pGroup);
	}

	m_Tmptab.EndAddValueItem();

	//设置为多种值的状态
	if( pFtrExchanger->GetObjectCount()>1 )
	{
		CString multivalue = StrFromResID(IDS_MULTIVALUE_NAME);
		int n1 = m_wndPropList.GetPropertyCount();
		for( int j=0; j<n1; j++)
		{
			CUIFProp* pProp = m_wndPropList.GetProperty(j);
			if( pProp->IsGroup() )
			{
				int n2 = pProp->GetSubItemsCount();
				for( int k=0; k<n2; k++)
				{
					CUIFProp *p = pProp->GetSubItem(k);
					CString text = p->FormatProperty();
					if( text.IsEmpty() )
						text = multivalue;
					else
						text = text + _T(", ") + multivalue;
					//p->SetMultiValueState(TRUE,text);
				}
			}
			else
			{
				CString text = pProp->FormatProperty();
				if( text.IsEmpty() )
					text = multivalue;
				else
					text = text + _T(", ") + multivalue;

				//pProp->SetMultiValueState(TRUE,text);
			}
		}

		m_wndPropList.RedrawWindow();
	}

}
//##ModelId=41466B7C0118
int CPropertiesViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create combo:
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST
		| WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

/*	
	if (!m_wndObjectCombo.Create (dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Properies Combo \n");
		return -1;      // fail to create
	}

	m_wndObjectCombo.AddString (StrFromResID(IDS_CURSEL_OBJ));
	m_wndObjectCombo.SetFont (CFont::FromHandle ((HFONT) GetStockObject (DEFAULT_GUI_FONT)));
	m_wndObjectCombo.SetCurSel (0);
*/
	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properies Grid \n");
		return -1;      // fail to create
	}

	m_wndPropList.RemoveAll();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	
	AdjustLayout ();


	return 0;
}

//##ModelId=41466B7C011B
void CPropertiesViewBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout ();
}

//##ModelId=41466B7C012A
void CPropertiesViewBar::OnSortingprop() 
{
	m_wndPropList.SetAlphabeticMode ();
}

//##ModelId=41466B7C0138
void CPropertiesViewBar::OnUpdateSortingprop(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndPropList.IsAlphabeticMode ());
}

//##ModelId=41466B7C015A
void CPropertiesViewBar::OnExpand() 
{
	m_wndPropList.SetAlphabeticMode (FALSE);
}

//##ModelId=41466B7C0167
void CPropertiesViewBar::OnUpdateExpand(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (!m_wndPropList.IsAlphabeticMode ());
}

//##ModelId=41466B7C013B
void CPropertiesViewBar::OnProperies1()
{
}

//##ModelId=41466B7C0148
void CPropertiesViewBar::OnUpdateProperies1(CCmdUI* pCmdUI) 
{	
}

//##ModelId=41466B7C014B
void CPropertiesViewBar::OnProperies2() 
{
	// TODO: Add your command handler code here
	
}

//##ModelId=41466B7C0157
void CPropertiesViewBar::OnUpdateProperies2(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

//##ModelId=41466B7C0175
void CPropertiesViewBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectList;

	m_wndPropList.GetWindowRect (rectList);
	ScreenToClient (rectList);

	rectList.InflateRect (1, 1);
	dc.Draw3dRect (rectList, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));

}

void CPropertiesViewBar::ClearPermaExchanger()
{
	//delete m_pFtrExchanger;
	m_pFtrExchanger =NULL;
	m_arrPts.RemoveAll();
	m_wndPropList.RemoveAll();
	m_wndPropList.Invalidate();
	m_Tmptab.DelAll();

	CDlgDoc * pDoc = GetActiveDlgDoc();
	if (pDoc && pDoc->GetDlgDataSource())
	{
		CFtrLayer *pAcitveLayer = pDoc->GetDlgDataSource()->GetCurFtrLayer();
		AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(9), LPARAM(pAcitveLayer));		
		AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(9), LPARAM(pDoc));
	}
}

LRESULT CPropertiesViewBar::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CUIFProp* pProp = (CUIFProp*)lParam;
	if(!pProp)return 0;
	
	CUIFProp *pProp0 = pProp;
	BOOL IsExAttr = FALSE;
	while(pProp0->GetParent())
	{
		pProp0 = pProp0->GetParent();
	}
	if(strcmp(pProp0->GetName(),StrFromResID(IDS_PROPERTIES_BASE))==0)
		IsExAttr = FALSE;
	else
		IsExAttr = TRUE;

	if (m_wndPropList.GetPropertyCount()>0)
	{	
		if(strcmp(pProp->GetName(),(LPCSTR)StrFromResID(IDS_LAYER_NAME))==0)
		{
// 			if(AfxMessageBox(StrFromResID(IDS_CMDTIP_CHANGELAYERNAME),MB_OKCANCEL)==IDCANCEL)
// 			{
// 				const CVariantEx* value = NULL;
// 				if(m_Tmptab.GetValue(0,FIELDNAME_LAYERNAME,value))
// 				{					
// 					pProp->SetValue((_variant_t)*value);
// 		 			return ;
// 				}
//  			}
			m_pFtrExchanger->OnModifyLayer((LPCSTR)(_bstr_t)pProp->GetValue());
			// 更新层码
			CUIFProp *pProp0 = m_wndPropList.FindProperty(StrFromResID(IDS_LAYMAN_LAYERCODE));
			if (pProp0)
			{
				CDlgDoc * pDoc = GetActiveDlgDoc();
				if (!pDoc) return 0;
				CConfigLibManager *pConfigMag = gpCfgLibMan;
				CScheme *pScheme = pConfigMag->GetScheme(pDoc->GetDlgDataSource()->GetScale());
				
				BOOL bFlag = FALSE;
				if (pScheme)
				{
					__int64 code;
					_bstr_t str = (_bstr_t)pProp->GetValue();
					CString ch = (const char*)str;
					bFlag = pScheme->FindLayerIdx(FALSE,code,ch);
					CString strCode;
					if(bFlag)
					{
						strCode.Format("%I64d",code);
					}
					pProp0->SetValue((_variant_t)(LPCTSTR)(strCode));
				}
				
			}
			return 0;
		}
		else if(strcmp(pProp->GetName(),(LPCSTR)StrFromResID(IDS_LAYMAN_LAYERCODE))==0)
		{
// 			if(AfxMessageBox(StrFromResID(IDS_CMDTIP_CHANGELAYERNAME),MB_OKCANCEL)==IDCANCEL)
// 			{
// 				const CVariantEx* value = NULL;
// 				if(m_Tmptab.GetValue(0,FIELDNAME_LAYERCODE,value))
// 				{					
// 					pProp->SetValue((_variant_t)*value);
// 					return 0;
// 				}
// 			}
			m_pFtrExchanger->OnModifyLayer((LPCSTR)(_bstr_t)pProp->GetValue());
			// 更新层名
			CUIFProp *pProp0 = m_wndPropList.FindProperty(StrFromResID(IDS_LAYER_NAME));
			if (pProp0)
			{
				CDlgDoc * pDoc = GetActiveDlgDoc();
				if (!pDoc) return 0;
				CConfigLibManager *pConfigMag = gpCfgLibMan;
				CScheme *pScheme = pConfigMag->GetScheme(pDoc->GetDlgDataSource()->GetScale());
				
				BOOL bFlag = FALSE;
				if (pScheme)
				{
					CString strcode = (const char*)(_bstr_t)(pProp->GetValue());
					__int64 code = _atoi64(strcode);
					TCHAR ch0[216]={0};
					CString ch1 = ch0;
					bFlag = pScheme->FindLayerIdx(TRUE,code,ch1);
					pProp0->SetValue((_variant_t)(ch1));		
				}
			}
			return 0;
		}
		else if(strcmp(pProp->GetName(),(LPCSTR)StrFromResID(IDS_PTINDEX)) == 0)
		{
			COleVariant oval = pProp->GetValue();
			oval.ChangeType(VT_I4);
			m_nPtIndex = oval.intVal;
			if (m_nPtIndex < 0)
			{
				m_nPtIndex = m_arrPts.GetSize()-1;
				pProp->SetValue((_variant_t)(long)(m_nPtIndex));				
			}
			else if (m_nPtIndex >= m_arrPts.GetSize())
			{
				m_nPtIndex = 0;
				pProp->SetValue((_variant_t)(long)(m_nPtIndex));
				
			}

			PT_3DEX pt = m_arrPts.GetAt(m_nPtIndex);

			CDlgDoc * pDoc = GetActiveDlgDoc();
			if (pDoc)
			{
				pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt);
			}			

			{
				pProp = m_wndPropList.FindProperty(_T("X"));
				if (pProp)
				{
					CString str;
					str.Format("%.4f",pt.x);
					pProp->SetValue(_variant_t(str));
				}

				pProp = m_wndPropList.FindProperty(_T("Y"));
				if (pProp)
				{
					CString str;
					str.Format("%.4f",pt.y);
					pProp->SetValue(_variant_t(str));
				}

				pProp = m_wndPropList.FindProperty(_T("Z"));
				if (pProp)
				{
					CString str;
					str.Format("%.4f",pt.z);
					pProp->SetValue(_variant_t(str));
				}

				pProp = m_wndPropList.FindProperty(StrFromResID(IDS_PTTYPE));
				if (pProp)
				{
					CString strStyle = PencodeToText(pt.pencode);

					pProp->SetValue((_variant_t)(const char*)(strStyle));
				}

				pProp = m_wndPropList.FindProperty(StrFromResID(IDS_CMDTIP_NODEWID));
				if (pProp)
				{
					CString str;
					str.Format("%.4f",pt.wid);
					pProp->SetValue(_variant_t(str));
				}

				pProp = m_wndPropList.FindProperty(StrFromResID(IDS_CMDTIP_NODETYPE));
				if (pProp)
				{
					pProp->SetValue(_variant_t(pt.type));
				}
			}
//			SetPermaExchanger(m_pFtrExchanger);
			return 0;
		}
		else if (strcmp(pProp->GetName(),(LPCSTR)StrFromResID(IDS_CMDPLANE_LINETYPE)) == 0)
		{
			CString strStyle = (const char*)(_bstr_t)pProp->GetValue();
			int code = TextToPencode(strStyle);

			m_pFtrExchanger->OnModifyLinetype(code);

			return 0;
		
		}
		else if (strcmp(pProp->GetName(),(LPCSTR)StrFromResID(IDS_CMDTIP_NODEWID)) == 0)
		{
			float wid = atof((const char*)(_bstr_t)pProp->GetValue());
			if (m_nPtIndex>=0 && m_nPtIndex < m_arrPts.GetSize())
			{
				m_pFtrExchanger->OnModifyNodeWid(m_nPtIndex,wid);
			}
			else
			{
				_variant_t var = wid;
				CValueTable tab;
				tab.BeginAddValueItem();
				tab.AddValue(PF_NODEWID,&CVariantEx(var));
				tab.EndAddValueItem();
				
				UpdateParam(tab);
			}

			return 0;
		}
		else if (strcmp(pProp->GetName(),(LPCSTR)StrFromResID(IDS_CMDTIP_NODETYPE)) == 0)
		{
			short type = atoi((const char*)(_bstr_t)pProp->GetValue());
			if (m_nPtIndex>=0 && m_nPtIndex < m_arrPts.GetSize())
			{
				m_pFtrExchanger->OnModifyNodeType(m_nPtIndex,type);
			}
			else
			{
				_variant_t var = type;
				CValueTable tab;
				tab.BeginAddValueItem();
				tab.AddValue(PF_NODETYPE,&CVariantEx(var));
				tab.EndAddValueItem();
				
				UpdateParam(tab);
			}

			return 0;
		}
		else if(strcmp(pProp->GetName(),(LPCSTR)StrFromResID(IDS_FILL_COLOR))==0)
		{
			CUIFFillColorProp *pColorProp = (CUIFFillColorProp*)pProp;
			if(!pColorProp) return 0;
			COLORREF color = pColorProp->GetColor();
			BOOL bFill = pColorProp->GetFillFlag();

			m_pFtrExchanger->OnModifyFillColor(bFill, color);
			return 0;
		}
		else if(strcmp(pProp->GetName(),(LPCSTR)StrFromResID(IDS_TRANSPARENCY))==0)
		{
			CString str = (const char*)(_bstr_t)pProp->GetValue();
			int transparency = atoi(str);
			if(transparency<0) transparency = 0;
			if(transparency>100) transparency = 100;

			m_pFtrExchanger->OnModifyTransparency(transparency);
			return 0;
		}
		
		_variant_t var = pProp->GetValue();	
		
		if (StrFromResID(IDS_FIELDNAME_TEXT_INCLINE).CompareNoCase(pProp->GetName()) == 0)
		{
			CString str = (const char*)(_bstr_t)pProp->GetValue();
			long type = SHRUGN;

			InclinedTypeToString(type,str,FALSE);
			
			var = type;
		}
		else if (StrFromResID(IDS_FIELDNAME_TEXT_ALIGNTYPE).CompareNoCase(pProp->GetName()) == 0)
		{
			CString str = (const char*)(_bstr_t)pProp->GetValue();
			long nAlign = (long)TAH_LEFT|TAV_TOP;

			AlignToString(nAlign,str,FALSE);
			
			var = nAlign;
		}
		else if (StrFromResID(IDS_FIELDNAME_TEXT_PLACETYPE).CompareNoCase(pProp->GetName()) == 0)
		{
			CString str = (const char*)(_bstr_t)pProp->GetValue();
			long type = singPt;
			PlaceTypeToString(type,str,FALSE);

			var = type;
		}
		else if (StrFromResID(IDS_FIELDNAME_GEOCOVERTYPE).CompareNoCase(pProp->GetName()) == 0)
		{
			CString str = (const char*)(_bstr_t)pProp->GetValue();
			long nCoverType = COVERTYPE_NONE;
			CoverTypeToString(nCoverType,str,FALSE);

			var = nCoverType;
		}
		else if (StrFromResID(IDS_FIELDNAME_FTRPURPOSE).CompareNoCase(pProp->GetName()) == 0)
		{
			CString str = (const char*)(_bstr_t)pProp->GetValue();
			var = (long)TextToPurpose(str);
		}
		else if (StrFromResID(IDS_FIELDNAME_GEOCELLANGLE).CompareNoCase(pProp->GetName()) == 0)
		{
			COleVariant oval = pProp->GetValue();
			oval.ChangeType(VT_R4);
			float ang = oval.fltVal;
			var = PI/180*ang;
		}
		
		DWORD_PTR dw = pProp->GetData();
		
		CString field,name;
		int type;			
		m_Tmptab.GetField(dw,field,type,name);

		CValueTable tab2;

		BOOL bModifyValuesOK = FALSE;

		//修改字体样式时，需要同时修改字宽倍率和倾斜角
		if (StrFromResID(IDS_FIELDNAME_TEXT_FONT).CompareNoCase(pProp->GetName()) == 0)
		{
			int old_flag = 0;
			const CVariantEx* value = NULL;
			if( m_Tmptab.GetValue(0,FIELDNAME_GEOTEXT_OTHERFLAG,value) )
			{					
				old_flag = (long)(_variant_t)*value;
			}

			TEXT_SETTINGS0 ts;
			ts.nOtherFlag = old_flag;

			CString font = (LPCTSTR)(_bstr_t)var;
			CUsedTextStyles *pStyles = GetUsedTextStyles();
			TextStyle style = pStyles->GetTextStyleByName(font);
			if( style.IsValid() )
			{
				_variant_t var2,var3,var4,var5;
				var2 = (double)style.fWidScale;
				var3 = (long)style.nInclineType;
				var4 = (double)style.fInclinedAngle;

				ts.SetBold(style.bBold);

				var5 = (long)ts.nOtherFlag;

				CString s2 = StrFromResID(IDS_FIELDNAME_TEXT_CHARWIDTHS);
				CString s3 = StrFromResID(IDS_FIELDNAME_TEXT_INCLINE);
				CString s4 = StrFromResID(IDS_FIELDNAME_TEXT_INCLINEANGLE);
				CString s5 = StrFromResID(IDS_FIELDNAME_TEXT_OTHERFLAG);

				for( int i=0; i<m_pFtrExchanger->GetObjectCount(); i++)
				{
					tab2.BeginAddValueItem();
					tab2.AddValue(field,&CVariantEx(var),name);
					tab2.AddValue(FIELDNAME_GEOTEXT_CHARWIDTHS,&CVariantEx(var2),s2);
					tab2.AddValue(FIELDNAME_GEOTEXT_INCLINE,&CVariantEx(var3),s3);
					tab2.AddValue(FIELDNAME_GEOTEXT_INCLINEANGLE,&CVariantEx(var4),s4);
					tab2.AddValue(FIELDNAME_GEOTEXT_OTHERFLAG,&CVariantEx(var5),s5);
					tab2.EndAddValueItem();
				}

				bModifyValuesOK = TRUE;
			}

		}
		else if( StrFromResID(IDS_BOLD).CompareNoCase(pProp->GetName())==0 || StrFromResID(IDS_UNDERLINE).CompareNoCase(pProp->GetName())==0 )
		{
			int old_value = 0;
			const CVariantEx* value = NULL;
			if( m_Tmptab.GetValue(0,FIELDNAME_GEOTEXT_OTHERFLAG,value) )
			{					
				old_value = (long)(_variant_t)*value;

				TEXT_SETTINGS0 ts;
				ts.nOtherFlag = old_value;

				if( StrFromResID(IDS_BOLD).CompareNoCase(pProp->GetName())==0 )
					ts.SetBold((bool)var);
				else
					ts.SetUnderline((bool)var);

				var = (_variant_t)(long)ts.nOtherFlag;

				m_Tmptab.SetValue(0,FIELDNAME_GEOTEXT_OTHERFLAG,&CVariantEx(var));

				for( int i=0; i<m_pFtrExchanger->GetObjectCount(); i++)
				{
					tab2.BeginAddValueItem();
					tab2.AddValue(FIELDNAME_GEOTEXT_OTHERFLAG,&CVariantEx(var),name);
					tab2.EndAddValueItem();
				}

				bModifyValuesOK = TRUE;
			}
		}
		
		if( !bModifyValuesOK )
		{
			for( int i=0; i<m_pFtrExchanger->GetObjectCount(); i++)
			{
				tab2.BeginAddValueItem();
				tab2.AddValue(field,&CVariantEx(var),name);
				tab2.EndAddValueItem();
			}
		}

		if(!IsExAttr)
			m_pFtrExchanger->OnModifyProperties(tab2);	
		else
			m_pFtrExchanger->OnModifyXAttributes(tab2);

		if( bModifyValuesOK )
		{
			UpdateProperty(tab2);
		}

		CDlgDoc *pDoc = GetActiveDlgDoc();
		if( pDoc )
		{
			CFtrLayer *pLayer = pDoc->GetDlgDataSource()->GetFtrLayerOfObject((CPFeature)m_pFtrExchanger->m_arrPFtr[0]);
			AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(4), LPARAM(pLayer));
			AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(pDoc));
		}		
	}
	return 0;
}

void CPropertiesViewBar::UpdateParam(CValueTable& tab)
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( pDoc )
	{
		pDoc->SetCurCmdParams(tab);
	}
}

void CPropertiesViewBar::UpdateProperty(CValueTable& tab)
{
	for (int i=0; i<tab.GetFieldCount(); i++)
	{
		CString field,name;
		int type;
		tab.GetField(i,field,type,name);

		const CVariantEx* value;
		tab.GetValue(0,field,value);

		CUIFProp *pProp = m_wndPropList.FindProperty(name);

		if( field.CompareNoCase(FIELDNAME_GEOTEXT_OTHERFLAG) == 0)
		{
			TEXT_SETTINGS0 ts;
			ts.nOtherFlag = (long)(_variant_t)*value;
			pProp = m_wndPropList.FindProperty(StrFromResID(IDS_BOLD));
			if (pProp)
			{
				pProp->SetValue((long)ts.IsBold());
			}
			pProp = m_wndPropList.FindProperty(StrFromResID(IDS_UNDERLINE));
			if (pProp)
			{
				pProp->SetValue((long)ts.IsUnderline());
			}
		}
		else if (field.CompareNoCase(FIELDNAME_GEOCOLOR) == 0 && pProp )
		{
			CUIFByLayerColorProp *pColorProp = (CUIFByLayerColorProp*)pProp;
			if (pColorProp)
			{
				pColorProp->SetColor((long)(_variant_t)(*value));
			}
		}
		else
		{
			if( pProp )pProp->SetValue(_variant_t(*value));
		}
	}

	if (m_pFtrExchanger != NULL)
	{
		m_pFtrExchanger->OnModifyProperties(tab);
	}
	
}


HBRUSH CPropertiesViewBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDockablePane::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	m_brush.CreateSolidBrush(RGB(50, 50, 50));
	// TODO:  Return a different brush if the default is not desired
	return m_brush;
	return hbr;
}


BOOL CPropertiesViewBar::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
	cs.style &= ~(LONG)WS_CAPTION;
	cs.style &= ~(LONG)WS_BORDER;

	//ModifyStyleEx(WS_EX_CLIENTEDGE, NULL, SWP_DRAWFRAME);
	//ModifyStyle(WS_CAPTION, 0);
	//ModifyStyleEx(WS_EX_WINDOWEDGE, 0);
	//ModifyStyle(WS_SIZEBOX, 0);
	//ModifyStyle(WS_THICKFRAME, 0);
	return CDockablePane::PreCreateWindow(cs);
}


void CPropertiesViewBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default

	/*CDockablePane::OnNcCalcSize(bCalcValidRects, lpncsp);*/
}


CDockablePane* CPropertiesViewBar::AttachToTabWnd(CDockablePane* pTabControlBarAttachTo,
	AFX_DOCK_METHOD dockMethod, BOOL bSetActive, CDockablePane** ppTabbedControlBar)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pTabControlBarAttachTo);

	if (ppTabbedControlBar != NULL)
	{
		*ppTabbedControlBar = NULL;
	}

	if (!pTabControlBarAttachTo->CanBeAttached() || !CanBeAttached())
	{
		return NULL; // invalid attempt to attach non-attachable control bar
	}

	// check whether pTabBar is derived from CTabbedPane. If so, we
	// can attach this bar to it immediately. Otherwise, we need to create a
	// new tabbed control bar and replace pTabControlBarAttachTo with it.
	CBaseTabbedPane* pTabbedBarAttachTo = DYNAMIC_DOWNCAST(CBaseTabbedPane, pTabControlBarAttachTo);

	BOOL bBarAttachToIsFloating = (pTabControlBarAttachTo->GetParentMiniFrame() != NULL);

	CWnd* pOldParent = GetParent();
	CRect rectWndTab; rectWndTab.SetRectEmpty();
	if (pTabbedBarAttachTo == NULL)
	{
		CWnd* pTabParent = pTabControlBarAttachTo->GetParent();
		if (DYNAMIC_DOWNCAST(CMFCBaseTabCtrl, pTabParent) != NULL)
		{
			pTabParent = pTabParent->GetParent();
		}

		pTabbedBarAttachTo = DYNAMIC_DOWNCAST(CBaseTabbedPane, pTabParent);

		if (pTabbedBarAttachTo == NULL)
		{
			//pTabControlBarAttachTo->StoreRecentDockSiteInfo();

			pTabControlBarAttachTo->GetWindowRect(rectWndTab);
			pTabControlBarAttachTo->GetParent()->ScreenToClient(&rectWndTab);

			{

				ASSERT_VALID(this);
				CRect rectTabBar;
				GetWindowRect(&rectTabBar);
				ASSERT_VALID(GetParent());
				GetParent()->ScreenToClient(&rectTabBar);

				CMyDefineDockTabPane* pTabbedBar = (CMyDefineDockTabPane*)m_pTabbedControlBarRTC->CreateObject();
				ASSERT_VALID(pTabbedBar);

				pTabbedBar->SetAutoDestroy(TRUE);
				bool s = pTabbedBar->ModifyStyle(TRUE, AFX_CBRS_CLOSE);

				if (!pTabbedBar->Create(_T(""), GetParent(), rectTabBar, TRUE, (UINT)-1, WS_CHILD | WS_VISIBLE| CBRS_LEFT | CBRS_HIDE_INPLACE 
					| WS_CAPTION | AFX_CBRS_RESIZE | AFX_CBRS_REGULAR_TABS))
				{
					TRACE0("Failed to create tabbed control bar\n");
					return NULL;
				}

				// override recent floating/docking info

				pTabbedBar->m_recentDockInfo.m_recentMiniFrameInfo.m_rectDockedRect = m_recentDockInfo.m_recentMiniFrameInfo.m_rectDockedRect;
				pTabbedBar->m_recentDockInfo.m_recentSliderInfo.m_rectDockedRect = m_recentDockInfo.m_recentSliderInfo.m_rectDockedRect;
				pTabbedBar->m_recentDockInfo.m_rectRecentFloatingRect = m_recentDockInfo.m_rectRecentFloatingRect;

				pTabbedBar->setNoClose();
				pTabbedBarAttachTo = pTabbedBar;

			}

			ASSERT_VALID(pTabbedBarAttachTo);

			pTabControlBarAttachTo->InsertPane(pTabbedBarAttachTo, pTabControlBarAttachTo);

			if (!pTabControlBarAttachTo->ReplacePane(pTabbedBarAttachTo, dockMethod))
			{
				if (!bBarAttachToIsFloating)
				{
					RemovePaneFromDockManager(pTabbedBarAttachTo);
				}
				ASSERT(FALSE);
				TRACE0("Failed to replace resizable control bar by tabbed control bar. \n");
				delete pTabbedBarAttachTo;
				return NULL;
			}

			pTabbedBarAttachTo->EnableDocking(pTabControlBarAttachTo->GetEnabledAlignment());
			pTabbedBarAttachTo->SetPaneAlignment(pTabControlBarAttachTo->GetCurrentAlignment());

			pTabControlBarAttachTo->UndockPane(TRUE);
			pTabbedBarAttachTo->AddTab(pTabControlBarAttachTo, TRUE, bSetActive);
			pTabControlBarAttachTo->EnableGripper(FALSE);
		}
	}

	if (ppTabbedControlBar != NULL)
	{
		*ppTabbedControlBar = pTabbedBarAttachTo;
	}

	EnableGripper(FALSE);

	// send before dock notification without guarantee that the bar will
	// be attached to another dock bar
	OnBeforeDock((CBasePane**)&pTabbedBarAttachTo, NULL, dockMethod);
	// reassign the parentship to the tab bar
	OnBeforeChangeParent(pTabbedBarAttachTo, TRUE);

	// remove from miniframe
	RemoveFromMiniframe(pTabbedBarAttachTo, dockMethod);

	// AddTab returns TRUE only if this pointer is not tabbed control bar
	//(tabbed control bar is destroyed by AddTab and its tab windows are copied
	// to pTabbedBarAttachTo tabbed window)
	BOOL bResult = pTabbedBarAttachTo->AddTab(this, TRUE, bSetActive);
	if (bResult)
	{
		OnAfterChangeParent(pOldParent);
		OnAfterDock(pTabbedBarAttachTo, NULL, dockMethod);
	}

	if (!rectWndTab.IsRectEmpty())
	{
		pTabbedBarAttachTo->SetWindowPos(NULL, rectWndTab.left, rectWndTab.top, rectWndTab.Width(), rectWndTab.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

		if (bResult)
		{
			AdjustDockingLayout();
		}
	}

	pTabbedBarAttachTo->RecalcLayout();

	return /*bResult ?*/ this/* : pTabbedBarAttachTo*/;
}


void CPermanentExchanger::OnModifyLayer(LPCSTR str)
{

}

void CPermanentExchanger::OnModifyLinetype(int code)
{

}

void CPermanentExchanger::OnModifyNodeWid(int index, float wid)
{

}

void CPermanentExchanger::OnModifyFtrWid(float wid)
{

}

void CPermanentExchanger::OnModifyFtrColor(COLORREF col)
{

}

void CPermanentExchanger::OnModifyNodeType(int index, short type)
{

}

void CPermanentExchanger::OnModifyTransparency(long transparency)
{

}

void CPermanentExchanger::OnModifyFillColor(BOOL bFill, COLORREF col)
{

}


BEGIN_MESSAGE_MAP(CPropertiesToolBar, CMFCToolBar)
END_MESSAGE_MAP()

