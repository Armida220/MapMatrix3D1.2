// ChkCmd.cpp: implementation of the CChkCmd class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Editbase.h"
#include "ChkCmd.h"
#include "GeoText.h "
#include "ConditionSelect.h "
#include "Selection.h "
#include "ExMessage.h "
#include "editbasedoc.h"
#include "CheckResultViewBar.h "
#include "Scheme.h "
//#include "..\CPolygonLib\polygon.h "
#include "SymbolLib.h "
#include "DlgCommand.h"
#include "Functions_temp.h"
#include "RegDef2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define REG_CHK_CMD(p)		pDoc->RegisterChkCmd(p::Create)	
void gRegisterChkCmd(CDlgDoc *pDoc)
{
//	REG_CHK_CMD(CTestChkCmd);

	REG_CHK_CMD(COverlapChkCmd);
	REG_CHK_CMD(CPartOverlapLineWithTolChkCmd);
	REG_CHK_CMD(CMinObtuseAngleChkCmd);

	REG_CHK_CMD(CSelfIntersectionChkCmd);
	REG_CHK_CMD(CSurfaceintersectChkCmd);
	REG_CHK_CMD(CGeoCurveClosedChkCmd);

	REG_CHK_CMD(CPLChkCmd);
	REG_CHK_CMD(CLLChkCmd);
//	REG_CHK_CMD(CElevPointChkCmd);
	REG_CHK_CMD(CRiverZChkCmd);
	REG_CHK_CMD(CClockDirChkCmd);
	REG_CHK_CMD(CLengthChkCmd);
	REG_CHK_CMD(CAreaChkCmd);
	REG_CHK_CMD(CZChkCmd);
	REG_CHK_CMD(CIntersectCurveChkCmd);

	BeginCheck41License

	REG_CHK_CMD(CGeoTypeConsistencyChkCmd);			//hcw,2013.1.24,层与对象的一致性检查
	REG_CHK_CMD(CAnnoandSurfaceConsistencyChkCmd);	//hcw,2013.1.25,注记与面或封闭线的一致性检查
    REG_CHK_CMD(CPtsandSurfaceConsistencyChkCmd);	//hcw,2013.1.30,点与面或封闭线的一致性检查
	REG_CHK_CMD(CZandAnnoConsistencyChkCmd);		//hcw,2013.1.31,高程点注记的一致性检查

	REG_CHK_CMD(CAppendantLoseChkCmd);
	REG_CHK_CMD(CPLSuspendChkCmd);
	REG_CHK_CMD(CLLSuspendChkCmd);
	REG_CHK_CMD(CUnreasonableBLChkCmd);
	REG_CHK_CMD(CZandAnnoConsistencyChkCmd);		//hcw,2013.1.31,高程点注记的一致性检查

	REG_CHK_CMD(CFtrSpaceLogicChkCmd);
	REG_CHK_CMD(CIsolatedObjChkCmd);
	REG_CHK_CMD(CContourValidChkCmd);
	REG_CHK_CMD(CPPChkCmd);
	REG_CHK_CMD(CSurfaceincludeChkCmd);

	BeginCheck42License
	REG_CHK_CMD(CCurveAcrossSurfaceChkCmd);
	REG_CHK_CMD(CContourRiverChkCmd);
	REG_CHK_CMD(CCurveAcrossHouseChkCmd);
	REG_CHK_CMD(CContourRiverIntersectChkCmd);
	REG_CHK_CMD(CNoIntersectInsideTunnelCmd);
	REG_CHK_CMD(CHeiPointOnHouseCmd);

	REG_CHK_CMD(CElevPtNumChkCmd);
	REG_CHK_CMD(CLayerChkCmd);
	REG_CHK_CMD(CSlpoeChkCmd);
	REG_CHK_CMD(CMapMatchChkCmd);
	REG_CHK_CMD(CCurveIntersectCurveChkCmd);
	REG_CHK_CMD(CSurfaceGapChkCmd);
	REG_CHK_CMD(CCurveSnapChkCmd);
	REG_CHK_CMD(CCulvertLineTypeChkCmd);
	REG_CHK_CMD(CHouseRectangularChkCmd);
	if (0x0804 == GetThreadLocale())//仅中文支持
	{
		REG_CHK_CMD(CHeiPointAcrossCurveChkCmd);
	}
	REG_CHK_CMD(CNarrowSurfaceChkCmd);
	EndCheck42License

	EndCheck41License	
}

BOOL convertStringToStrArray(LPCTSTR str,CStringArray &arr)
{
	if(!str)
		return FALSE;
	CString temp(str);
	int startPos = 0,endPos = 0;
	while(endPos>=0)
	{
		endPos = temp.Find(_T(","),startPos);
		if(endPos>startPos)
			arr.Add(temp.Mid(startPos,endPos-startPos));
		else
			arr.Add(temp.Mid(startPos));
		startPos = endPos+1;
	}
	return TRUE;
}

void GetFtrLayers(CDlgDataSource *pDS, LPCTSTR names, CPtrArray& arrPLayers)
{
	if (!pDS)
	{
		return;
	}

	if( !names||strlen(names) )
	{
		int nLayers = pDS->GetFtrLayerCount();
		for( int i=0; i<nLayers; i++)
		{
			arrPLayers.Add(pDS->GetFtrLayerByIndex(i));
		}
		return;
	}

	CString text = names;
	text.Remove(' ');
	int startpos = 0, findpos = 0;
	while( findpos>=0 )
	{
		findpos = text.Find(',',startpos);
		
		CString sub;
		if( findpos>startpos )
			sub = text.Mid(startpos,(findpos-startpos));
		else
			sub = text.Mid(startpos);
		
		pDS->GetFtrLayer(names,NULL,&arrPLayers);
		
		startpos = findpos+1;
	}
	
	return;
}
BOOL IsPtsinRegion(CArray<PT_3DEX,PT_3DEX>& ptsArr1,CArray<PT_3DEX,PT_3DEX>& ptsArr2) //hcw,2012.1.29,判断ptsArr1中的点是否被包含在ptsArr2形成的封闭区域中。
{
	if (ptsArr1.GetSize()<=0||ptsArr2.GetSize()<=0)
	{
		return FALSE;
	}
    for (int i=0; i<ptsArr1.GetSize(); i++)
    {
		if (GraphAPI::GIsPtInRegion(ptsArr1[i], ptsArr2.GetData(),ptsArr2.GetSize())<=1)
		{
			return FALSE;
		}
    }
	return TRUE;
}
BOOL IsPtsinRegion(PT_3DEX pt, CArray<PT_3DEX,PT_3DEX>& ptsArr)//hcw,2012.1.29,重载
{
	if (ptsArr.GetSize()<=0)
	{
		return FALSE;
	}
	if (GraphAPI::GIsPtInRegion(pt,ptsArr.GetData(),ptsArr.GetSize())==2)
	{
		return TRUE;
	}
	return FALSE;
}


#define MARK_SELECTION		if( bForSelection )MarkSelectionFtrs(1)

#define CLEAR_MARK_SELECTION    if( bForSelection )MarkSelectionFtrs(0)

#define SKIP_NOT_SELECTION		if( bForSelection && pFtr->GetAppFlagBit(23)==0 )continue

#define SKIP_NOT_SELECTION0(x)		if( bForSelection && (x)->GetAppFlagBit(23)==0 )continue

extern BOOL CheckNameForLayerCode(CDlgDataSource *pDS, LPCTSTR name, LPCTSTR pstrCode);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChkCmd::CChkCmd()
{
	m_pDoc = NULL;
}

CChkCmd::~CChkCmd()
{

}


void CChkCmd::MarkSelectionFtrs(int flag)
{
	if( !m_pDoc )return;
	CSelection *pSel = m_pDoc->GetSelection();

	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if( flag )
		pDS->SaveAllAppFlags();

	int num = 0;
	const FTR_HANDLE *buf = pSel->GetSelectedObjs(num);
	if( num<=0 )
		return;

	for( int i=0; i<num; i++)
	{
		CFeature *pFtr = HandleToFtr(buf[i]);
		if( pFtr )
		{
			pFtr->SetAppFlagBit(23,flag);
		}
	}

	if( flag==0 )
		pDS->RestoreAllAppFlags();
}

//////////////////////////////////////////////////////////////////////////
CTestChkCmd::CTestChkCmd()
{
	m_nInt = 0;
	m_lfDouble = 1.0;
	m_strString = _T("");
	m_bBool = FALSE;
	m_nOption = Option1;
}

CTestChkCmd::~CTestChkCmd()
{

}

CString CTestChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CTestChkCmd::GetCheckName()const
{
	return _T("TestCheckCmd");
}

void CTestChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (long)m_nInt;
	tab.AddValue(CHK_INT,&CVariantEx(var));
	var = (double)m_lfDouble;
	tab.AddValue(CHK_DBL,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strString;
	tab.AddValue(CHK_STR,&CVariantEx(var));
	var = (bool)m_bBool;
	tab.AddValue(CHK_BOOL,&CVariantEx(var));
	var = (long)m_nOption;
	tab.AddValue(CHK_OPTION,&CVariantEx(var));
}

void CTestChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_INT,var) )
	{
		m_nInt = (long)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_DBL,var) )
	{
		m_lfDouble= (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_STR,var) )
	{
		m_strString = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_BOOL,var) )
	{
		m_bBool = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_OPTION,var) )
	{
		m_nOption = (long)(_variant_t)*var;
	}	
}

void CTestChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CTestChkCmd"),_T("CTestChkCmd"));
	if(!m_bBool&&!bForLoad);
	else
		param->AddParam(CHK_INT,m_nInt,_T("int"));
	param->AddParam(CHK_DBL,m_lfDouble,_T("float"));
	param->AddParam(CHK_STR,m_strString,_T("string"));

	if(!bForLoad&&m_nOption==2);	
	else
		param->AddParam(CHK_BOOL,(bool)m_bBool,_T("BOOL type"));
	
	param->BeginOptionParam(CHK_OPTION,_T("option"));
	param->AddOption(_T("option1"),Option1,' ',m_nOption==Option1);
	param->AddOption(_T("option2"),Option2,' ',m_nOption==Option2);
	param->AddOption(_T("option3"),Option3,' ',m_nOption==Option3);
	param->EndOptionParam();
// 	param->AddLayerNameParam(CMDPLANE_CONTOURCODE,(LPCTSTR)m_strBaseFCode,StrFromResID(IDS_CMDPLANE_CONTOURCODE));
// 	param->AddLayerNameParam(CMDPLANE_CHECKCODE,(LPCTSTR)m_strCheckFCode, StrFromResID(IDS_CMDPLANE_CHECKCODE));
// 	
}

void CTestChkCmd::Execute(BOOL bForSelection)
{
	AfxMessageBox(_T("hellow"));
// 	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
// 	pDS->GetO
	CSelection *sel = m_pDoc->GetSelection();
	int nNum;
	const FTR_HANDLE *handles = sel->GetSelectedObjs(nNum);
	if(nNum<=0)return;
	CCPResultList result;
	result.attrNameList.Add(StrFromResID(IDS_CMDNAME_CHECKPP));

			result.StartRecord();
			result.SetCurFtr(HandleToFtr(handles[0]));
			result.SetCurAttr(0);
		result.AddPointResult(0,(LPCTSTR)StrFromResID(IDS_PLERR_ZOUTSIDE));
		
			result.FinishRecord();
			
	
	AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,2,(LPARAM)&result);
}

void CTestChkCmd::GetAllReasons(CStringArray& reasons)
{

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtrSpaceLogicChkCmd::CFtrSpaceLogicChkCmd()
{
	m_bCheckLineForLess2Pt = TRUE;
	m_bCheckSurfaceLess3Pt = TRUE;
	m_bCheckRepeatedPt = TRUE;
	m_lfTolerForRepeatedPt = 0.01;
	m_bTurnRoundLine = TRUE;
}

CFtrSpaceLogicChkCmd::~CFtrSpaceLogicChkCmd()
{

}

CString CFtrSpaceLogicChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CFtrSpaceLogicChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_SPACELOGRELATIONCHK);
}

void CFtrSpaceLogicChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (LPCTSTR)m_strLays;
	tab.AddValue("LayNames",&CVariantEx(var));
	var = (bool)m_bCheckLineForLess2Pt;
	tab.AddValue(CHK_LINE_2PT,&CVariantEx(var));
	var = (bool)m_bCheckSurfaceLess3Pt;
	tab.AddValue(CHK_SURFACE_3PT,&CVariantEx(var));
	var = (bool)m_bCheckRepeatedPt;
	tab.AddValue(CHK_REPEATEDPT,&CVariantEx(var));
	var = (double)m_lfTolerForRepeatedPt;
	tab.AddValue(CHK_TOLER_REPEATEDPT,&CVariantEx(var));
	var = (bool)m_bTurnRoundLine;
	tab.AddValue(CHK_TURNROUNDLINE,&CVariantEx(var));

}

void CFtrSpaceLogicChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"LayNames",var) )
	{
		m_strLays = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_LINE_2PT,var) )
	{
		m_bCheckLineForLess2Pt = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_SURFACE_3PT,var) )
	{
		m_bCheckSurfaceLess3Pt= (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_REPEATEDPT,var) )
	{
		m_bCheckRepeatedPt= (bool)(_variant_t)*var;
	}

	if( tab.GetValue(0,CHK_TOLER_REPEATEDPT,var) )
	{
		m_lfTolerForRepeatedPt = (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_TURNROUNDLINE,var) )
	{
		m_bTurnRoundLine = (bool)(_variant_t)*var;
	}
	
}

void CFtrSpaceLogicChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CFtrSpaceLogicChkCmd"),_T("CFtrSpaceLogicChkCmd"));
	//param->AddParam("LayNames",(LPCTSTR)m_strLays,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS));
	param->AddLayerNameParamEx("LayNames",(LPCTSTR)m_strLays,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY);

	param->AddParam(CHK_LINE_2PT,(bool)m_bCheckLineForLess2Pt,StrFromResID(IDS_CHKCMD_DES_LESSTWOLINE));
	param->AddParam(CHK_SURFACE_3PT,(bool)m_bCheckSurfaceLess3Pt,StrFromResID(IDS_CHKCMD_DES_LESSTHREEAREA));
	param->AddParam(CHK_REPEATEDPT,(bool)m_bCheckRepeatedPt,StrFromResID(IDS_CHKCMD_DES_REPEATEDPT));
	if(bForLoad||m_bCheckRepeatedPt)param->AddParam(CHK_TOLER_REPEATEDPT,m_lfTolerForRepeatedPt,StrFromResID(IDS_CHKCMD_DES_REPEATEDPT_TOL));
	param->AddParam(CHK_TURNROUNDLINE,(bool)m_bTurnRoundLine,StrFromResID(IDS_CHKCMD_DES_TURNROUNDLINE));
}

namespace
{
	bool ptIsOnLine(PT_3DEX start,PT_3DEX end,PT_3DEX testpt)
	{
		//start-end为重合点的情况排除
		double dx = end.x-start.x;
		double dy = end.y-start.y;
		if (fabs(dx)<1e-4&&
			fabs(dy)<1e-4)
		{
			return false;
		}
		//排除点在线段的正向延长线上的情况
		double dx0 = testpt.x-end.x;
		double dy0 = testpt.y-end.y;
		if(-dx*dx0>0||
			-dy*dy0>0)
		{
			if (fabs(dx*dy0-dy*dx0)<1e-4)
			{
				return true;
			}
		}
		return false;
	}
}
void CFtrSpaceLogicChkCmd::Execute(BOOL bForSelection)
{
 	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
/*	int nFtrLay = pDS->GetFtrLayerCount();
	int nSum = 0;
	for (int i=0;i<nFtrLay;i++)
	{
		CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;

		if( !CheckNameForLayerCode(pDS,pLayer->GetName(),m_strLays) )
			continue;

		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			nSum++;
		}
	}

	MARK_SELECTION;

	GProgressStart(nSum);
	for (i=0;i<nFtrLay;i++)
	{
		CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();*/
	int i =0 , j =0;
	CFtrArray ftrs;
	
	if( m_strLays.IsEmpty() )
	{
		int nFtrLay = pDS->GetFtrLayerCount();		
		for (i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for(j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				ftrs.Add( pFtr );					
			}
		}
		
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLays,arr))
			return ;	
		for(int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				continue;
			int nTar = arrLayers.GetSize();	
			for (i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;					
					ftrs.Add( pFtr );
				}
			}
		}
		//
		
	}
	int nSum = ftrs.GetSize();
	MARK_SELECTION;
	GProgressStart(nSum);

		for (j=0;j<nSum;j++)
		{
		//	CFeature *pFtr = pLayer->GetObject(j);
		//	if (!pFtr||!pFtr->IsVisible())
		//		continue;
			CFeature* pFtr = ftrs.GetAt(j);
			SKIP_NOT_SELECTION;

			GProgressStep();
			if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
				continue;
			else if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			{
				//少于2点的线
				CGeometry *pObj = pFtr->GetGeometry();
				if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				{
					if( m_bCheckLineForLess2Pt&&pObj->GetDataPointSum()<2)
					{					
						CString str = StrFromResID(IDS_CHK_LESSTWOPT_LINE);
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetReason(str);
						PT_3DEX ptex = pObj->GetDataPoint(0);
						m_pChkResMgr->SetAssociatedPos(PT_3D(ptex.x,ptex.y,0));
						m_pChkResMgr->EndResItem();
						continue;
					}
				}
				else
				{
					//少于3点的面				
					if(m_bCheckSurfaceLess3Pt&&pObj->GetDataPointSum()<3)
					{					
						CString str = StrFromResID(IDS_CHK_LESSTHREEPT_AREA);					
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetReason(str);
						PT_3DEX ptex = pObj->GetDataPoint(0);
						m_pChkResMgr->SetAssociatedPos(PT_3D(ptex.x,ptex.y,0));
						m_pChkResMgr->EndResItem();
						continue;
					}
				}
				
				//相邻重叠点
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pObj->GetShape(arrPts);
				if(m_bCheckRepeatedPt)
				{					
					CArray<PT_3D,PT_3D> arrPos;
					CArray<int,int> arrCnt;
					BOOL bRepeat = FALSE;
					int nCurCnt;
					int nNum = arrPts.GetSize();
					PT_3DEX *pts = arrPts.GetData(); 					
					{
						int k, pos = 0;
						PT_3DEX *pt = pts+1;					
						for( k=1; k<nNum; k++,pt++)
						{
							if( fabs(pts[pos].x-pt->x)<m_lfTolerForRepeatedPt && 
								fabs(pts[pos].y-pt->y)<m_lfTolerForRepeatedPt )
							{	
								if(bRepeat)
									nCurCnt++;
								else
								{
									bRepeat = TRUE;
									arrPos.Add(PT_3D(pt->x,pt->y,0));
									nCurCnt = 1;
								}
								continue;
							}
							else
							{
								if(bRepeat)
								{
									bRepeat = FALSE;
									arrCnt.Add(nCurCnt);
								}
							}
							pos = k;
						}
						if (bRepeat)
						{
							arrCnt.Add(nCurCnt);
						}

						for (int l=0;l<arrPos.GetSize();++l)
						{
							m_pChkResMgr->BeginResItem();
							m_pChkResMgr->AddAssociatedFtr(pFtr);
							m_pChkResMgr->SetAssociatedPos(arrPos[l]);
							m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_REPEATED_PT));
							m_pChkResMgr->EndResItem();
						}
					}
				}
				//回头线:有点串（p1,p1,...,pi,...pn),若pi点与pi-2点重合（i>=2),则称点串在pi处回头
				if (m_bTurnRoundLine)
				{
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pObj->GetShape(arrPts);
					int nNum = arrPts.GetSize();
					PT_3DEX *pts = arrPts.GetData(); 					
					{
						int k, pos = 0;
						PT_3DEX *pt = pts+2;					
						for( k=2; k<nNum; k++,pt++)
						{							
							if( ptIsOnLine(pts[pos],pts[pos+1],pt[0]))
							{
								m_pChkResMgr->BeginResItem();
								m_pChkResMgr->AddAssociatedFtr(pFtr);
								m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_TURNROUND_LINE));
								PT_3D pt0;
								pt0.x = ((*(pt-1)).x+pt[0].x)/2;
								pt0.y = ((*(pt-1)).y+pt[0].y)/2;
								pt0.z = 0;
								m_pChkResMgr->SetAssociatedPos(pt0);
								m_pChkResMgr->EndResItem();
							}
							pos++;						
						}					
					}
				}			
				
			}
		}
	//}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}


void CFtrSpaceLogicChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_LESSTWOPT_LINE));
	reasons.Add(StrFromResID(IDS_CHK_LESSTHREEPT_AREA));
	reasons.Add(StrFromResID(IDS_CHK_REPEATED_PT));
	reasons.Add(StrFromResID(IDS_CHK_TURNROUND_LINE));
}

//////////////////////////////////////////////////////////////////////////
COverlapChkCmd::COverlapChkCmd()
{
	m_bSameLayerCode = TRUE;//是否同层码
	m_geotypelimit = 1;
	m_nMode = 1;
}

COverlapChkCmd::~COverlapChkCmd()
{

}

CString COverlapChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString COverlapChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_OVERLAP_OBJ);
}

void COverlapChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (long)(m_geotypelimit);
	tab.AddValue(_T("geo_limit"),&CVariantEx(var));
	var = (long)(m_nMode);
	tab.AddValue(_T("chk_mode"),&CVariantEx(var));
	var = (LPCTSTR)m_strLays;
	tab.AddValue("LayNames",&CVariantEx(var));
	var = (bool)m_bSameLayerCode;
	tab.AddValue(CHK_OVERLAP_SAMELAYCODE,&CVariantEx(var));
}

void COverlapChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"LayNames",var) )
	{
		m_strLays = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_OVERLAP_SAMELAYCODE,var) )
	{
		m_bSameLayerCode = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,_T("geo_limit"),var) )
	{		
		m_geotypelimit = (long)(_variant_t)*var;
	}
	if( tab.GetValue(0,_T("chk_mode"),var) )
	{		
		m_nMode = (long)(_variant_t)*var;
	}
}

void COverlapChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("COverlapChkCmd"),_T("COverlapChkCmd"));

	param->AddLayerNameParamEx("LayNames",(LPCTSTR)m_strLays,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY);
	param->AddParam(CHK_OVERLAP_SAMELAYCODE,(bool)m_bSameLayerCode,StrFromResID(IDS_CHKCMD_SAME_LAYERCODE));

	param->BeginOptionParam(_T("geo_limit"),StrFromResID(IDS_GEOTYPE_LIMIT));
	param->AddOption(StrFromResID(IDS_GEO_NOLIMIT),0,' ',m_geotypelimit==0);
	param->AddOption(StrFromResID(IDS_GEO_CURVE),1,' ',m_geotypelimit==1);
	param->AddOption(StrFromResID(IDS_GEO_SURFACE),2,' ',m_geotypelimit==2);
	param->EndOptionParam();

	param->BeginOptionParam(_T("chk_mode"),StrFromResID(IDS_CHK_MODE));
	param->AddOption(StrFromResID(IDS_CHK_MODE_SAMENODE),0,' ',m_nMode==0);
	param->AddOption(StrFromResID(IDS_CHK_MODE_SAMESHAPE),1,' ',m_nMode==1);
	param->EndOptionParam();
}


void COverlapChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	CDataQuery* pDq = m_pDoc->GetDataQuery();
	if(!pDS)return;
	int nFtrLay = pDS->GetFtrLayerCount();
	int nSum = 0;
	CFtrArray ftrs, ftrs1;
	for (int i=0;i<nFtrLay;i++)
	{
		CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;

		if( !m_strLays.IsEmpty() && !CheckNameForLayerCode(pDS,pLayer->GetName(),m_strLays) )
			continue;

		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo) continue;

			if(m_geotypelimit==1 && !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				continue;
			if(m_geotypelimit==2 && !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				continue;
			
			nSum++;
			ftrs.Add(pFtr);
		}
	}

	MARK_SELECTION;

	GProgressStart(nSum);

	for (i=0;i<nSum;i++)
	{
		GProgressStep();

		CFeature *pFtr = ftrs[i];

		ftrs1.RemoveAll();
		for( int j=i+1; j<nSum; j++)
		{
			CFeature *pFtr1 = ftrs[j];
			if( !pFtr1 )continue;

			if( CDelAllSameCommand::IsWholeOverlapedObj(pDS,pFtr,pFtr1,m_bSameLayerCode,m_nMode) )
			{
				ftrs1.Add(pFtr1);
				ftrs[j] = NULL;
			}
		}

		if( ftrs1.GetSize()>0 )
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);

			for( j=0; j<ftrs1.GetSize(); j++)
			{
				m_pChkResMgr->AddAssociatedFtr(ftrs1[j]);
			}

			CArray<PT_3DEX,PT_3DEX> arrPts;
			pFtr->GetGeometry()->GetShape(arrPts);

			m_pChkResMgr->SetAssociatedPos(arrPts[0]);

			CString temp;
			temp.Format(StrFromResID(IDS_CHKCMD_HOWMANY_OVERLAP),ftrs1.GetSize()+1);
			m_pChkResMgr->SetReason(temp);

			m_pChkResMgr->EndResItem();
		}
	}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}

void COverlapChkCmd::GetAllReasons(CStringArray& reasons)
{
	CString temp;
	temp.Format(StrFromResID(IDS_CHKCMD_HOWMANY_OVERLAP),(int)0);
	reasons.Add(temp);
}

//////////////////////////////////////////////////////////////////////////
CSelfIntersectionChkCmd::CSelfIntersectionChkCmd()
{
	m_strLayerCodes = _T("");
	m_geotypelimit = 0;
}

CSelfIntersectionChkCmd::~CSelfIntersectionChkCmd()
{

}

CString CSelfIntersectionChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CSelfIntersectionChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_SELFINTERSECTION);
}

void CSelfIntersectionChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;

	var = (long)(m_geotypelimit);
	tab.AddValue(_T("type_limit"),&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_SELF_INTERSECT_CODES,&CVariantEx(var));	
}

void CSelfIntersectionChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;

	if( tab.GetValue(0,_T("type_limit"),var) )
	{		
		m_geotypelimit = (long)(_variant_t)*var;
	}

	if( tab.GetValue(0,CHK_SELF_INTERSECT_CODES,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
}

void CSelfIntersectionChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CSelfIntersectionChkCmd"),_T("CSelfIntersectionChkCmd"));

	param->AddLayerNameParamEx(CHK_SELF_INTERSECT_CODES,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);

	param->BeginOptionParam(_T("type_limit"),StrFromResID(IDS_GEOTYPE_LIMIT));
	param->AddOption(StrFromResID(IDS_GEO_NOLIMIT),0,' ',m_geotypelimit==0);
	param->AddOption(StrFromResID(IDS_GEO_CURVE),1,' ',m_geotypelimit==1);
	param->AddOption(StrFromResID(IDS_GEO_SURFACE),2,' ',m_geotypelimit==2);
	param->EndOptionParam();
}


static bool SelIntersect(CArray<PT_3DEX,PT_3DEX> &arrPts0,CArray<PT_3D,PT_3D> &arrPts)
{
	bool res = false;

	CArray<PT_3DEX,PT_3DEX> arrPts1;
	arrPts1.Copy(arrPts0);

	//清除重复节点
	int npt = GraphAPI::GKickoffSame2DPoints(arrPts1.GetData(),arrPts1.GetSize());
	arrPts1.SetSize(npt);

	if(npt<=2)
		return false;

	double lfX,lfY;
	PT_3DEX *pos = arrPts1.GetData();

	if(npt==3)
	{
		if( GraphAPI::GIsPtInLine(pos[0],pos[1],pos[2]) || GraphAPI::GIsPtInLine(pos[2],pos[1],pos[0]) )
			return true;
		else
			return false;		
	}

	for( int i=0; i<npt-3; i++)
	{
		double x0 = pos[i].x;
		double y0 = pos[i].y;
		double x1 = pos[i+1].x;
		double y1 = pos[i+1].y;
		if(pos[i+1].pencode==penMove) continue;
		double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;
		
		if( x0<x1 ){ xmin1 = x0; xmax1 = x1; }
		else { xmin1 = x1; xmax1 = x0; }
		if( y0<y1 ){ ymin1 = y0; ymax1 = y1; }
		else { ymin1 = y1; ymax1 = y0; }		
		
		for (int j=i+2;j<npt-1;j++)
		{
			if(pos[j+1].pencode==penMove) continue;
			if( pos[j].x<pos[j+1].x ){ xmin2 = pos[j].x; xmax2 = pos[j+1].x; }
			else { xmin2 = pos[j+1].x; xmax2 = pos[j].x; }
			if( pos[j].y<pos[j+1].y ){ ymin2 = pos[j].y; ymax2 = pos[j+1].y; }
			else { ymin2 = pos[j+1].y; ymax2 = pos[j].y; }
			if( xmax1<xmin2 || xmax2<xmin1 || 
				ymax1<ymin2 || ymax2<ymin1 )
				continue;
			if (GraphAPI::GGetLineIntersectLineSeg(x0,y0,x1,y1,pos[j].x,pos[j].y,pos[j+1].x,pos[j+1].y,&lfX,&lfY,NULL,NULL))
			{
				//排除闭合情况
				if(i==0 || pos[i].pencode==penMove)
				{
					for(int k=i+1; k<npt-2; k++)
					{
						if(pos[k+2].pencode==penMove) break;
					}
					if(j==k && GraphAPI::GIsEqualPoint2D(x0,y0,pos[j+1].x,pos[j+1].y))
						continue;
				}

				arrPts.Add(PT_3D(lfX,lfY,0));
				res =  true;
			}
		}
	}
	return res;
}

static void delSamePt(CArray<PT_3D,PT_3D> &arrPts)
{
	for (int i=arrPts.GetSize()-1;i>0;i--)
	{
		if (GraphAPI::GIsEqual2DPoint(&arrPts[i],&arrPts[i-1]))
		{
			arrPts.RemoveAt(i);
		}			
	}
}

namespace
{
	//hcw,2013.1.30,返回字符串str在字符串数组strArr中的索引nIdx;
	int FindValueItem(CUIntArray& arr, UINT value)
	{
		int nIdx = -1;
		for (int i=0; i<arr.GetSize(); i++)
		{
		   if (value==arr.GetAt(i))
		   {
			   nIdx = i;
			   break;
		   }
		}
		return nIdx;
	}
	/************************************************************************************
	hcw.2013.1.30
	参数:
		[IN]pDS:数据源
		[IN]clsNameArr:选中的地物类型
		[OUT]container:符合条件的地物对象
		[IN]strFiterLayerCodes:选中的层码
		[IN]bCurveClosed:线类型(CGeoCurveBase)地物是否封闭。
	*************************************************************************************/
	void LoadDataSource(CDlgDataSource *pDS,CUIntArray& clsArr,vector<CFeature*>& container,CString strFiterLayerCodes, BOOL bCurveClosed=FALSE)
	{
		if (strFiterLayerCodes.IsEmpty())
		{
			
			int nFtrLay = pDS->GetFtrLayerCount();
			for (int i=0;i<nFtrLay;i++)
			{
				CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;

					CGeometry *pGeo = pFtr->GetGeometry();

					//{hcw,2013.1.29. test for uniform
				    if (FindValueItem(clsArr,pGeo->GetClassType())>=0)
				    {
						//CString str = prt->m_lpszClassName; //for monitor;
						if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
						{
							if(((CGeoCurveBase*)pGeo)->IsClosed())
							{
								if (bCurveClosed)
									container.push_back(pFtr);
								else
									continue;
							}
							else
							{
								if (!bCurveClosed)
									container.push_back(pFtr);
								else
									continue;
							}
						}
						else
							container.push_back(pFtr);
				    }
					//}
				}
			}    
			
		}
		else
		{
			CStringArray arr;
			if (!convertStringToStrArray(strFiterLayerCodes,arr))
				return;
			for (int a=0;a<arr.GetSize();a++)
			{
				CPtrArray arrLayers;
				if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
					return;
				int nTar = arrLayers.GetSize();	
				for (int i=0;i<nTar;i++)
				{
					CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
					if(!pLayer||!pLayer->IsVisible())
						continue;
					int nObj = pLayer->GetObjectCount();
					for (int j=0;j<nObj;j++)
					{
						CFeature *pFtr = pLayer->GetObject(j);
						if (!pFtr||!pFtr->IsVisible())
							continue;
						
						CGeometry *pGeo = pFtr->GetGeometry();
						
						//{hcw,2013.1.29. test for uniform
						if (FindValueItem(clsArr,pGeo->GetClassType())>=0)
						{
							//CString str = prt->m_lpszClassName; //for monitor;
							if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
							{
								if(((CGeoCurveBase*)pGeo)->IsClosed())
								{
									if (bCurveClosed)
										container.push_back(pFtr);
									else
										continue;
								}
								else
								{
									if (!bCurveClosed)
										container.push_back(pFtr);
									else
										continue;
								}
							}
							else
								container.push_back(pFtr);
						}
						//}
					}
				}
			}
			
		}
		return;
	}
}

void CSelfIntersectionChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	vector<CFeature*> container;
	//把选定层中的面地物选出来
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;

				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				{
					if(m_geotypelimit==0 || m_geotypelimit==1)//限定线或者不限定
						container.push_back(pFtr);
				}
				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					if(m_geotypelimit==0 || m_geotypelimit==2)//限定面或者不限定
						container.push_back(pFtr);
				}					
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					
					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
					{
						if(m_geotypelimit==0 || m_geotypelimit==1)//限定线或者不限定
							container.push_back(pFtr);
					}
					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					{
						if(m_geotypelimit==0 || m_geotypelimit==2)//限定面或者不限定
							container.push_back(pFtr);
					}
				}
			}
		}			
	}
	
	int nCount = container.size();//hcw,2013.1.23,for monitor
	MARK_SELECTION;

	GProgressStart(container.size());
	vector<CFeature*>::const_iterator it;
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		CArray<PT_3D,PT_3D> arrPts;
		CFeature *pFtr = (*it);

		SKIP_NOT_SELECTION;

		CGeometry *pGeo = pFtr->GetGeometry();
		if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			continue;
		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape()->GetPts(pts);

		if(SelIntersect(pts,arrPts))
		{
			delSamePt(arrPts);
			for (int i=0;i<arrPts.GetSize();i++)
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(arrPts[i]);
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_SELFINTERSECTION));
				m_pChkResMgr->EndResItem();
			}		
		}		
	}	
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}



void CSelfIntersectionChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_CMD_SELFINTERSECTION));
}

//////////////////////////////////////////////////////////////////////////
CIntersectCurveChkCmd::CIntersectCurveChkCmd()
{
	m_strLayerCodes = _T("");
}

CIntersectCurveChkCmd::~CIntersectCurveChkCmd()
{

}

CString CIntersectCurveChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CIntersectCurveChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_INTERSECCURVE);
}

void CIntersectCurveChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_INTERSECTCURVE_CODES,&CVariantEx(var));

}

void CIntersectCurveChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;

	if( tab.GetValue(0,CHK_INTERSECTCURVE_CODES,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}	
}

void CIntersectCurveChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CIntersectCurveChkCmd"),_T("CIntersectCurveChkCmd"));

	param->AddLayerNameParamEx(CHK_INTERSECTCURVE_CODES,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);

// 	param->AddLayerNameParam(CMDPLANE_CONTOURCODE,(LPCTSTR)m_strBaseFCode,StrFromResID(IDS_CMDPLANE_CONTOURCODE));
// 	param->AddLayerNameParam(CMDPLANE_CHECKCODE,(LPCTSTR)m_strCheckFCode, StrFromResID(IDS_CMDPLANE_CHECKCODE));
// 	
}

extern int GetCurveIntersectCurve(PT_3DEX *pts1, int num1, PT_3DEX *pts2, int num2, CArray<PtIntersect,PtIntersect> &arrIntersectPts);
void CIntersectCurveChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	vector<CFeature*> container;
	CStringArray arr;
	if (!m_strLayerCodes.IsEmpty())
	{
		convertStringToStrArray(m_strLayerCodes,arr);
	}
	//把选定层中的线和面地物选出来
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				
				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				{				
					container.push_back(pFtr);
				}						
			}
		}
	}
	else
	{
		
		int nCount = arr.GetSize();//hcw,2013.1.23,for test
		if (arr.GetSize() <= 0)
		{
			return;
		}

		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					
					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
					{					
						container.push_back(pFtr);
					}
				}
			}
		}			
	}

	MARK_SELECTION;

	CDataQuery *pDQ = m_pDoc->GetDataQuery();

	GProgressStart(container.size());
	vector<CFeature*>::const_iterator it;
	CArray<CFeature*,CFeature*> arrHaveChecked;
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		
		CFeature *pFtr = (*it);

		SKIP_NOT_SELECTION;

		pFtr->SetAppFlagBit(0,1);

		CArray<PT_3DEX,PT_3DEX> pts;
		const CShapeLine *pShape = pFtr->GetGeometry()->GetShape();
		pShape->GetPts(pts);

		int npt = GraphAPI::GKickoffSame2DPoints(pts.GetData(),pts.GetSize());
		pts.SetSize(npt);

		PT_3DEX *pts_buf = pts.GetData();

		for( int k=1; k<npt; k++)
		{
			Envelope e1;
			e1.CreateFromPts(pts_buf+k-1,2,sizeof(PT_3DEX));
			m_pDoc->GetDataQuery()->FindObjectInRect(e1,NULL,NULL,NULL);

			int num = 0;
			const CPFeature * ppftr = m_pDoc->GetDataQuery()->GetFoundHandles(num);
			if (num<=1 || !ppftr) continue;	

			for (int i=num-1; i>=0; i--)
			{
				CFeature *pFoundFtr = ppftr[i];
				if( pFoundFtr->GetAppFlagBit(0) )
					continue;

				if (!pFoundFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					continue;
				}

				CFtrLayer *pftrLayer = pDS->GetFtrLayerOfObject(pFoundFtr);
				if (!pftrLayer) continue;
				

				//增加对层码的识别
				if(!m_strLayerCodes.IsEmpty())
				{
					__int64 id = -1;
					CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
					if (pScheme)
					{
						CSchemeLayerDefine *p_app_layer = pScheme->GetLayerDefine( pftrLayer->GetName() );
						if( p_app_layer )
							id = p_app_layer->GetLayerCode();
					}
					CString strID;
					strID.Format("%I64d", id);
					//
					
					for (int j=0; j<arr.GetSize(); j++)
					{
						if (arr[j].CompareNoCase(pftrLayer->GetName()) == 0 || arr[j].CompareNoCase(strID) == 0 )	//modified
						{
							break;
						}
					}
					
					if (j >= arr.GetSize())
					{
						continue;
					}
				}

				CArray<PT_3DEX,PT_3DEX> pts0;
				const CShapeLine *pShape0 = pFoundFtr->GetGeometry()->GetShape();
				pShape0->GetPts(pts0);

				int npt2 = GraphAPI::GKickoffSame2DPoints(pts0.GetData(),pts0.GetSize());
				pts0.SetSize(npt2);

				CArray<PtIntersect,PtIntersect> arrIntersectPts;
				GetCurveIntersectCurve(pts_buf+k-1,2,pts0.GetData(),pts0.GetSize(),arrIntersectPts);

				int j=0;
				if( k==1 || k==(npt-1) )
				{
					PT_3D targetPt;
					if( k==1 )
						targetPt = pts_buf[0];
					else
						targetPt = pts_buf[npt-1];

					//排除首尾相连的情况
					for( j=arrIntersectPts.GetSize()-1; j>=0; j--)
					{
						if( GraphAPI::GGet2DDisOf2P(arrIntersectPts[j].pt,targetPt)<GraphAPI::g_lfDisTolerance )
						{
							arrIntersectPts.RemoveAt(j);
						}
					}
				}

				CArray<PT_3D,PT_3D> arrPts;
				for (j=0; j<arrIntersectPts.GetSize(); j++)
				{
					arrPts.Add(arrIntersectPts[j].pt);
				}
				delSamePt(arrPts);
				for ( j=0;j<arrPts.GetSize();j++)
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->SetAssociatedPos(arrPts[j]);
					m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_INTERSECCURVE));
					m_pChkResMgr->EndResItem();
				}
			}
		}
		
		arrHaveChecked.Add(pFtr);
	}	

	for (it= container.begin();it<container.end();it++)
	{
		(*it)->SetAppFlagBit(0,0);
	}

	GProgressEnd();

	CLEAR_MARK_SELECTION;

}


void CIntersectCurveChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_CMD_INTERSECCURVE));
}

//////////////////////////////////////////////////////////////////////////
CMinObtuseAngleChkCmd::CMinObtuseAngleChkCmd()
{
	m_lfMinAngle = 15.0;
}

CMinObtuseAngleChkCmd::~CMinObtuseAngleChkCmd()
{

}

CString CMinObtuseAngleChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CMinObtuseAngleChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_MIN_OBTUSE_ANGLE);
}

void CMinObtuseAngleChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (double)m_lfMinAngle;
	tab.AddValue(CHK_MIN_ANGLE,&CVariantEx(var));
	var = (LPCTSTR)m_strLayerCodes;
	tab.AddValue("chk_Codes",&CVariantEx(var));
}

void CMinObtuseAngleChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_MIN_ANGLE,var) )
	{
		m_lfMinAngle= (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,"chk_Codes",var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}		
}

void CMinObtuseAngleChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CMinObtuseAngleChkCmd"),_T("CMinObtuseAngleChkCmd"));
	
	param->AddParam(CHK_MIN_ANGLE,m_lfMinAngle,StrFromResID(IDS_CHKCMD_DES_MIN_ANGLE));
	param->AddLayerNameParamEx("chk_Codes",m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
	
}

void CMinObtuseAngleChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	CDataQuery* pDq = m_pDoc->GetDataQuery();
	if(!pDS)return;
	int nFtrLay = pDS->GetFtrLayerCount();
	int nSum = 0;

	CStringArray arr;
	if (!m_strLayerCodes.IsEmpty())
	{
		convertStringToStrArray(m_strLayerCodes,arr);
	}

	CArray<CFtrLayer*,CFtrLayer*> arrLayers0;

	//把选定层中的线和面地物选出来
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();

			nSum += nObj;

			arrLayers0.Add(pLayer);
		}
	}
	else
	{		
		if (arr.GetSize() <= 0)
		{
			return;
		}
		
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;

			for( int i=0; i<arrLayers.GetSize(); i++)
			{
				CFtrLayer *pLayer = (CFtrLayer*)arrLayers[i];
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				
				nSum += nObj;
				arrLayers0.Add(pLayer);
			}
		}
	}

	nFtrLay = arrLayers0.GetSize();

	MARK_SELECTION;

	GProgressStart(nSum);
	const double c_minAngle = m_lfMinAngle*PI/180.0;
	for (int i=0;i<nFtrLay;i++)
	{
		CFtrLayer* pLayer = arrLayers0[i];
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			GProgressStep();
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;

			SKIP_NOT_SELECTION;

			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				continue;

			CArray<PT_3DEX,PT_3DEX> arrPts;
			const CShapeLine *pShp = ((CGeoCurveBase*)pFtr->GetGeometry())->GetShape();
			pShp->GetPts(arrPts);

			int nSz = GraphAPI::GKickoffSame2DPoints(arrPts.GetData(),arrPts.GetSize());
			arrPts.SetSize(nSz);
			if(nSz<3) continue;
			PT_3DEX* pts = arrPts.GetData();
			for (int k=0;k<nSz-2;++k)
			{
				if(pts[k+1].pencode==penMove || pts[k+2].pencode==penMove)
					continue;
				double ang = GraphAPI::GGetIncludedAngle(pts[k+1],pts[k],pts[k+1],pts[k+2]);
				if(ang<c_minAngle)
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					CString reason;
					reason.Format("%s : %.2f", StrFromResID(IDS_CHK_ANGLE_TOO_SMALL), ang*180/PI);
					m_pChkResMgr->SetAssociatedPos(PT_3D(pts[k+1].x,pts[k+1].y,0));
					m_pChkResMgr->SetReason(reason);
					m_pChkResMgr->EndResItem();
				}
			}
			
			//处理闭合点
			if( ((CGeoCurveBase*)pGeo)->IsClosed() )
			{
				PT_3DEX pt0;
				PT_3DEX pt1;
				PT_3DEX pt2;
				for(k=0; k<nSz-1; k++)
				{
					if(k==0 || pts[k].pencode==penMove)
					{
						pt1 = pts[k];
						pt2 = pts[k+1];
					}

					if(k+2>=nSz || pts[k+2].pencode==penMove)
					{
						pt0 = pts[k];
						double ang = GraphAPI::GGetIncludedAngle(pt1,pt0,pt1,pt2);
						if(ang<c_minAngle)
						{
							m_pChkResMgr->BeginResItem();
							m_pChkResMgr->AddAssociatedFtr(pFtr);
							CString reason;
							reason.Format("%s : %.2f", StrFromResID(IDS_CHK_ANGLE_TOO_SMALL), ang*180/PI);
							m_pChkResMgr->SetAssociatedPos(pt1);
							m_pChkResMgr->SetReason(reason);
							m_pChkResMgr->EndResItem();
						}
					}
				}
			}
		}
	}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}

void CMinObtuseAngleChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_ANGLE_TOO_SMALL));
}



//////////////////////////////////////////////////////////////////////////
CIsolatedObjChkCmd::CIsolatedObjChkCmd()
{
	m_strTargetCode = _T("");
	m_strAdjCode = _T("");
	m_lfSearchRadius = 10.0;
}

CIsolatedObjChkCmd::~CIsolatedObjChkCmd()
{

}

CString CIsolatedObjChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CIsolatedObjChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_ISOLATED_OBJ);
}

void CIsolatedObjChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;	
	var = (_bstr_t)(LPCTSTR)m_strTargetCode;
	tab.AddValue(CHK_TARGET_OBJ_CODE,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strAdjCode;
	tab.AddValue(CHK_ADJACENT_OBJ_CODE,&CVariantEx(var));
	var = (double)m_lfSearchRadius;
	tab.AddValue(CHK_ISOLATED_RADIUS,&CVariantEx(var));
}

void CIsolatedObjChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;	
	if( tab.GetValue(0,CHK_TARGET_OBJ_CODE,var) )
	{
		m_strTargetCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_ADJACENT_OBJ_CODE,var) )
	{
		m_strAdjCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_ISOLATED_RADIUS,var) )
	{
		m_lfSearchRadius= (double)(_variant_t)*var;
	}	
}

void CIsolatedObjChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CIsolatedObjChkCmd"),_T("CIsolatedObjChkCmd"));
	param->AddLayerNameParam(CHK_TARGET_OBJ_CODE,m_strTargetCode,StrFromResID(IDS_CHKCMD_DES_TARGETCODE),NULL,LAYERPARAMITEM_NOTEMPTY);
	param->AddLayerNameParam(CHK_ADJACENT_OBJ_CODE,m_strAdjCode,StrFromResID(IDS_CHK_CMD_DES_ADJCODE),NULL,LAYERPARAMITEM_NOTEMPTY);
	param->AddParam(CHK_ISOLATED_RADIUS,m_lfSearchRadius,StrFromResID(IDS_CHK_CMD_DES_SEARCH_RADIUS));
}

double CIsolatedObjChkCmd::GetDisOfPt2Ftr(PT_3DEX pt, CFeature* pFtr)
{
	if(!pFtr) return -1;	
	CGeometry* pGeo = pFtr->GetGeometry();
	if(!pGeo) return -1;
	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
	{
		CArray<PT_3DEX, PT_3DEX> arrPts;
		pGeo->GetShape( arrPts );
		if( arrPts.GetSize() <= 0 ) return -1;
		
		PT_3DEX ept = arrPts.GetAt(0);
		return GraphAPI::GGet2DDisOf2P(pt, ept);
		
	}

	const CShapeLine* pSL = pGeo->GetShape();
	if(!pSL) return -1;
	CArray<PT_3DEX, PT_3DEX> pts;
	pSL->GetPts( pts );
	int num = pts.GetSize();
	if(num <= 0) return -1;

	if( 1 == num )
	{
		PT_3DEX expt = pts.GetAt(0);
		return GraphAPI::GGet2DDisOf2P(pt, expt);
	}

	PT_3DEX ptRet;
	int index = -1;
	double dis = GraphAPI::GGetNearstDisPToPointSet2D(pts.GetData(), num, pt, ptRet, &index);
	return dis;
}

void CIsolatedObjChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	CDataQuery* pDq = m_pDoc->GetDataQuery();
	if(!pDS)return;
	CPtrArray arrTargetLayers,arrAdjLayers;
	if(!pDS->GetFtrLayer(m_strTargetCode,NULL,&arrTargetLayers))
		return;
	CString strAdjName = pDS->GetFtrLayerNameByNameOrLayerIdx(m_strAdjCode);
	int nTar = arrTargetLayers.GetSize();
	int nSum = 0;
	for (int i=0;i<nTar;i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrTargetLayers.GetAt(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			nSum++;
		}
	}

	MARK_SELECTION;

	GProgressStart(nSum);
	for (i=0;i<nTar;i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrTargetLayers.GetAt(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;

			GProgressStep();

			CLEAR_MARK_SELECTION;

			BOOL bFlag = FALSE;
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pFtr->GetGeometry()->GetShape(arrPts);
			int nPts = arrPts.GetSize();
			if (nPts==1)
			{
				if(pDq->FindNearestObject(arrPts[0],m_lfSearchRadius,NULL))
				{
					int nNum;
					const CPFeature *ftr = pDq->GetFoundHandles(nNum);
					for (int m=0;m<nNum;++m)
					{
						if(ftr[m]==pFtr)
							continue;
						//by mzy
						double real_dis = GetDisOfPt2Ftr(arrPts[0], ftr[m]);
						if( real_dis == -1 || (real_dis-m_lfSearchRadius) > GraphAPI::g_lfDisTolerance )
							continue;
						//
						CString strCode = pDS->GetFtrLayerOfObject(ftr[m])->GetName();
						if (strCode.CompareNoCase(strAdjName)==0)
						{
							bFlag = TRUE;
// 							m_pChkResMgr->BeginResItem();
// 							m_pChkResMgr->AddAssociatedFtr(pFtr);
// 							m_pChkResMgr->SetAssociatedPos(PT_3D(arrPts[0].x,arrPts[0].y,0));
// 							m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_ISOLATED_OBJ));
// 							m_pChkResMgr->EndResItem();
 							break;
						}
					}
				}
			}
			else
			{
			
				//首点
				if(pDq->FindNearestObject(arrPts[0],m_lfSearchRadius,NULL))
				{
					int nNum;
					const CPFeature *ftr = pDq->GetFoundHandles(nNum);
					for (int m=0;m<nNum;++m)
					{
						if(ftr[m]==pFtr)
							continue;
						CString strCode = pDS->GetFtrLayerOfObject(ftr[m])->GetName();
						if (strCode.CompareNoCase(strAdjName)==0)
						{
							bFlag = TRUE;
// 							m_pChkResMgr->BeginResItem();
// 							m_pChkResMgr->AddAssociatedFtr(pFtr);
// 							m_pChkResMgr->SetAssociatedPos(PT_3D(arrPts[0].x,arrPts[0].y,0));
// 							m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_ISOLATED_OBJ));
// 							m_pChkResMgr->EndResItem();
// 							flag = TRUE;
							break;
						}
					}
				}
				//尾点
				if(bFlag==FALSE&&pDq->FindNearestObject(arrPts[nPts-1],m_lfSearchRadius,NULL))
				{
					int nNum;
					const CPFeature *ftr = pDq->GetFoundHandles(nNum);
					for (int m=0;m<nNum;++m)
					{
						if(ftr[m]==pFtr)
							continue;
						CString strCode = pDS->GetFtrLayerOfObject(ftr[m])->GetName();
						if (strCode.CompareNoCase(strAdjName)==0)
						{
							bFlag = TRUE;
// 							m_pChkResMgr->BeginResItem();
// 							m_pChkResMgr->AddAssociatedFtr(pFtr);
// 							m_pChkResMgr->SetAssociatedPos(PT_3D(arrPts[0].x,arrPts[0].y,0));
// 							m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_ISOLATED_OBJ));
// 							m_pChkResMgr->EndResItem();
// 							flag = TRUE;
							break;
						}
					}
				}
				//线串
				if (bFlag==FALSE)
				{
					CArray<PT_3D,PT_3D> arrPtsReverse;
					arrPtsReverse.SetSize(nPts);
					for (int m=0;m<nPts;m++)
					{
						COPY_3DPT(arrPtsReverse[m],arrPts[m]);
					}
					CArray<PT_3D,PT_3D> res;
					res.SetSize(nPts+nPts);
					GraphAPI::GGetParallelLine(arrPtsReverse.GetData(),nPts,m_lfSearchRadius,res.GetData());
					for ( m=nPts-1;m>=0;m--)
					{
						COPY_3DPT(arrPtsReverse[nPts-1-m],arrPts[m]);
					}
					GraphAPI::GGetParallelLine(arrPtsReverse.GetData(),nPts,m_lfSearchRadius,res.GetData()+nPts);
					if(pDq->FindObjectInPolygon(res.GetData(),nPts+nPts,NULL)>0)
					{
						int nNum;
						const CPFeature *ftr = pDq->GetFoundHandles(nNum);
						for (int m=0;m<nNum;++m)
						{
							if(ftr[m]==pFtr)
								continue;
							CString strCode = pDS->GetFtrLayerOfObject(ftr[m])->GetName();
							if (strCode.CompareNoCase(strAdjName)==0)
							{
								bFlag = TRUE;
// 								m_pChkResMgr->BeginResItem();
// 								m_pChkResMgr->AddAssociatedFtr(pFtr);
// 								m_pChkResMgr->SetAssociatedPos(PT_3D(arrPts[0].x,arrPts[0].y,0));
// 								m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_ISOLATED_OBJ));
// 								m_pChkResMgr->EndResItem();
								break;
							}
						}
					}
			
				}				

			}
			if(bFlag==FALSE)
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(PT_3D(arrPts[0].x,arrPts[0].y,0));
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_ISOLATED_OBJ));
				m_pChkResMgr->EndResItem();
			}
		}
	}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}


void CIsolatedObjChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_CMD_ISOLATED_OBJ));
}



//////////////////////////////////////////////////////////////////////////
CSurfaceintersectChkCmd::CSurfaceintersectChkCmd()
{
	m_strLayerCodes = _T("");
}

CSurfaceintersectChkCmd::~CSurfaceintersectChkCmd()
{

}

CString CSurfaceintersectChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CSurfaceintersectChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_SURFACE_INTERSECT);
}

void CSurfaceintersectChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;	
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_SURFACE_INTERSEC_LAYCODES,&CVariantEx(var));
}

void CSurfaceintersectChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;	
	if( tab.GetValue(0,CHK_SURFACE_INTERSEC_LAYCODES,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}

}

void CSurfaceintersectChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CSurfaceintersectChkCmd"),_T("CSurfaceintersectChkCmd"));
	param->AddLayerNameParamEx(CHK_SURFACE_INTERSEC_LAYCODES,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_AREA);
}



namespace
{
	struct item
	{
		Envelope e;
		CFeature *pFtr;
	};
}

static bool GLineIntersectLineSeg1(double x0,double y0, double x1, double y1,double x2,double y2,double x3,double y3, double *x, double *y, double*t)
{
	double vector1x = x1-x0, vector1y = y1-y0;
	double vector2x = x3-x2, vector2y = y3-y2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if( delta<1e-5 && delta>-1e-5 )return false;
	double t1 = ( (x2-x0)*vector2y-(y2-y0)*vector2x )/delta;
	double xr = x0 + t1*vector1x, yr = y0 + t1*vector1y;
	double t2 = ( (x2-x0)*vector1y-(y2-y0)*vector1x )/delta;
	if(t1<-1e-5 || t1>1+1e-5)
		return false;
	if(t2<1e-5 || t2>1+1e-5)
		return false;

	if(x) *x = xr;
	if(y) *y = yr;
	if(t) *t = t1;
	return true;
}

// 闭合线串和线段相交
int GetCurveIntersectSeg(CGeometry *pGeo, PT_3DEX pt1, PT_3DEX pt2, CArray<PtIntersect,PtIntersect> &arrIntersectPts)
{
	if(!pGeo) return 0;
	PT_3DEX pts2[] = {pt1,pt2};
	CArray<PT_3DEX,PT_3DEX> pts, newpts;
	pGeo->GetShape()->GetPts(pts);
	PT_3DEX *pts1 = pts.GetData();
	int nPt = pts.GetSize();
	if (nPt<2) return 0;
	
	Envelope e1, e2;
	e1.CreateFromPts(pts1,nPt,sizeof(PT_3DEX));
	e2.CreateFromPts(pts2,2,sizeof(PT_3DEX));
	
	if (!e1.bIntersect(&e2)) return 0;
	
	PT_3D ret;
	double t;
	bool bIntersect = false;
	CArray<PtIntersect,PtIntersect> arr;
	PtIntersect item;

	//插入线段首点
	item.IntersectFlag = 0;
	COPY_3DPT(item.pt, pts2[0]);
	item.lfRatio = 0;
	arr.Add(item);

	//插入线段交点（交点包含了首尾点）
	for (int i=0; i<nPt-1; i++)
	{
		if( (pts1+i+1)->pencode == penMove) continue;
		//求相交点
		if (!GLineIntersectLineSeg1(pts2[0].x, pts2[0].y, pts2[1].x, pts2[1].y, 
			       pts1[i].x,pts1[i].y,pts1[i+1].x,pts1[i+1].y,&(ret.x),&(ret.y),&t))
		{
			continue;
		}					
		else
		{
			item.IntersectFlag = 1;
			item.pt = ret;
			item.lfRatio = t;
			
			//按照 t 大小排序插入
			int size = arr.GetSize();
			for( int k=1; k<size && item.lfRatio>arr[k].lfRatio; k++);
			if( k<size )arr.InsertAt(k,item);
			else arr.Add(item);
		}
	}
	//插入线段尾点
	item.IntersectFlag = 0;
	COPY_3DPT(item.pt, pts2[1]);
	item.lfRatio = 1;
	arr.Add(item);

	if(arr.GetSize() <=2)//无交点,则直接返回
	{
		return 0;
	}

	for(int j=0; j<arr.GetSize()-1; j++)
	{
		if(GraphAPI::GIsEqual2DPoint(&arr[j].pt, &arr[j+1].pt))
			continue;

		PT_3D ptcen;
		ptcen.x = (arr[j].pt.x+arr[j+1].pt.x)/2;
		ptcen.y = (arr[j].pt.y+arr[j+1].pt.y)/2;
		ptcen.z = 0;

		if(pGeo->bPtIn(&ptcen)==2)
		{
			if(arr[j].IntersectFlag) arrIntersectPts.Add(arr[j]);
			if(arr[j+1].IntersectFlag) arrIntersectPts.Add(arr[j+1]);
		}
	}
	
	return arrIntersectPts.GetSize();
}
//不计算交点为端点的情况
int GetIntersectPts(CFeature *pFtr1, CFeature *pFtr2, CArray<PtIntersect,PtIntersect> &arrIntersectPts)
{
	if(!pFtr1 || !pFtr2) return 0;

	CGeometry *pGeo1 = pFtr1->GetGeometry();
	CGeometry *pGeo2 = pFtr2->GetGeometry();
	if(!pGeo1 || !pGeo2)  return 0;

	CArray<PT_3DEX,PT_3DEX> pts2;
	pGeo2->GetShape()->GetPts(pts2);
	int npt2 = GraphAPI::GKickoffSame2DPoints(pts2.GetData(),pts2.GetSize());
	pts2.SetSize(npt2);

	for (int i=0; i<npt2-1; i++)
	{
		if(pts2[i+1].pencode==penMove) continue;
		GetCurveIntersectSeg(pGeo1, pts2[i], pts2[i+1],arrIntersectPts);
	}
	
	return arrIntersectPts.GetSize();
}

void CSurfaceintersectChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	vector<item> container;
	//把选定层中的面地物选出来
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				
				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					item data;
					data.pFtr = pFtr;
					data.e = pFtr->GetGeometry()->GetEnvelope();
					container.push_back(data);
				}
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					
					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					{
						item data;
						data.pFtr = pFtr;
						data.e = pFtr->GetGeometry()->GetEnvelope();
						container.push_back(data);
					}
				}
			}
		}			
	}

	MARK_SELECTION;

	GProgressStart(container.size());
	vector<item>::const_iterator it,it0;
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		CFeature *pFtr = (*it).pFtr;

		SKIP_NOT_SELECTION;

		Envelope e = (*it).e;

		for (it0=it+1;it0<container.end();it0++)
		{
			CFeature *pFtr0 = (*it0).pFtr;
			if(!e.bIntersect(&((*it0).e))) continue;

			CArray<PtIntersect,PtIntersect> arrIntersectPts;
			GetIntersectPts(pFtr,pFtr0,arrIntersectPts);

			if(arrIntersectPts.GetSize()>0)
			{
				CArray<PT_3D,PT_3D> arrPts;
				int j=0;
				for (j=0; j<arrIntersectPts.GetSize(); j++)
				{
					arrPts.Add(arrIntersectPts[j].pt);
				}
				delSamePt(arrPts);
				for ( j=0;j<arrPts.GetSize();j++)
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->AddAssociatedFtr(pFtr0);
					m_pChkResMgr->SetAssociatedPos(arrPts[j]);
					m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_AREAPT_INC_AREA));
					m_pChkResMgr->EndResItem();
				}
			}
		}
	}
	GProgressEnd();

	for (it= container.begin();it<container.end();it++)
	{
		(*it).pFtr->SetAppFlagBit(0,0);
	}
	
	CLEAR_MARK_SELECTION;
}

void CSurfaceintersectChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_AREAPT_INC_AREA));
}


//////////////////////////////////////////////////////////////////////////
CSurfaceincludeChkCmd::CSurfaceincludeChkCmd()
{
	m_strLayerCodes = _T("");
}

CSurfaceincludeChkCmd::~CSurfaceincludeChkCmd()
{

}

CString CSurfaceincludeChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CSurfaceincludeChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_SURFACE_INC);
}

void CSurfaceincludeChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;	
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_SURFACE_INCLUDE_LAYCODES,&CVariantEx(var));
}

void CSurfaceincludeChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;	
	if( tab.GetValue(0,CHK_SURFACE_INCLUDE_LAYCODES,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}

}

void CSurfaceincludeChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CSurfaceincludeChkCmd"),_T("CSurfaceincludeChkCmd"));
	param->AddLayerNameParamEx(CHK_SURFACE_INCLUDE_LAYCODES,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_AREA);
}

static bool GLineIntersectLineSeg2(double x0,double y0, double x1, double y1,double x2,double y2,double x3,double y3, double *x, double *y, double *rt1,double *rt2)
{
	double vector1x = x1-x0, vector1y = y1-y0;
	double vector2x = x3-x2, vector2y = y3-y2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if( delta<1e-5 && delta>-1e-5 )return false;
	double t1 = ( (x2-x0)*vector2y-(y2-y0)*vector2x )/delta;
	double xr = x0 + t1*vector1x, yr = y0 + t1*vector1y;
	double t2 = ( (x2-x0)*vector1y-(y2-y0)*vector1x )/delta;
	if(t1<0 || t1>1)
		return false;
	if(t2<0 || t2>1)
		return false;
	
	if(x) *x = xr;
	if(y) *y = yr;
	if(rt1) *rt1 = t1;
	if(rt2) *rt2 = t2;
	return true;
}
//两条线段的交点个数，端点处的交点不算
int GetCurveIntersectCurve1(PT_3DEX *pts1, int num1, PT_3DEX *pts2, int num2)
{
	if (!pts1 || !pts2 || num1 < 2 || num2 < 2) return false;
	
	Envelope e1, e2;
	e1.CreateFromPts(pts1,num1,sizeof(PT_3DEX));
	e2.CreateFromPts(pts2,num2,sizeof(PT_3DEX));
	
	if (!e1.bIntersect(&e2)) return 0;
	
	PT_3D ret;
	double t1, t2;
	bool bIntersect = false;
	int nSum=0;
	for (int i=0; i<num1-1; i++)
	{
		if( (pts1+i+1)->pencode == penMove) continue;
		for( int j=0; j<num2-1; j++ )
		{
			if( (pts2+j+1)->pencode == penMove) continue;
			//求相交点
			if (GLineIntersectLineSeg2(pts1[i].x, pts1[i].y, pts1[i+1].x, pts1[i+1].y, pts2[j].x, pts2[j].y,
				pts2[j+1].x, pts2[j+1].y, &(ret.x), &(ret.y), &t1, &t2))
			{				
				if( fabs(t1)<1e-6 || fabs(t1-1)<1e-6 || fabs(t2)<1e-6 || fabs(t2-1)<1e-6 )
				{
					continue;
				}
				nSum++;
			}
		}
	}
	
	return nSum;
}

//判断面包含
bool IsSurfaceInclude(CFeature* pFtr0, CFeature* pFtr1)
{
	if(!pFtr0 || !pFtr1)
		return false;
	
	CGeometry* pObj0 = pFtr0->GetGeometry();
	CGeometry* pObj1 = pFtr1->GetGeometry();
	if(!pObj0 || !pObj1)
		return false;

	if( !pObj0->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || !pObj1->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		return false;
	}

	Envelope e1 = pObj0->GetEnvelope();
	Envelope e2 = pObj1->GetEnvelope();

	if(!e1.bEnvelopeIn(&e2))
	{
		return false;
	}

	CArray<PT_3DEX, PT_3DEX> arrPts0, arrPts1;
	pObj0->GetShape()->GetPts(arrPts0);
	pObj1->GetShape()->GetPts(arrPts1);

	//1如果边线存在交点则不包含
	//CArray<PtIntersect,PtIntersect> arrIntersectPts;
	//int sum = GetCurveIntersectCurve1(arrPts0.GetData(), arrPts0.GetSize(), arrPts1.GetData(), arrPts1.GetSize());
	//if(sum>0) return false;
	//2检查pFtr0的点是否有落在pFtr1内部
	for(int i=0; i<arrPts0.GetSize(); i++)
	{
		PT_3D pt0(arrPts0[i].x, arrPts0[i].y, arrPts0[i].z);
		if(2==pObj1->bPtIn(&pt0))
		{
			return false;
		}
	}

	//3检查pFtr1的点是否有落在pFtr0内部
	for(i=0; i<arrPts1.GetSize(); i++)
	{
		PT_3D pt1(arrPts1[i].x, arrPts1[i].y, arrPts1[i].z);
		if(pObj0->bPtIn(&pt1)<1)
		{
			return false;
		}
	}

//	//2再判断pFtr1每条边的中点是否在pFtr0的内部或线上
// 	for(int i=0; i<arrPts1.GetSize()-1; i++)
// 	{
// 		if(arrPts1[i+1].pencode==penMove) continue;
// 		PT_3DEX ptcen;
// 		ptcen.x = (arrPts1[i].x+arrPts1[i+1].x)/2;
// 		ptcen.y = (arrPts1[i].y+arrPts1[i+1].y)/2;
// 		ptcen.z = (arrPts1[i].z+arrPts1[i+1].z)/2;
// 
// 		PT_3DEX *pts = arrPts0.GetData();
// 		int size = arrPts0.GetSize();
// 
// 		BOOL bPtInLine = FALSE;
// 		int i=0, nSum=0, start=0;
// 		for( i=0; i<=size; i++)
// 		{
// 			if( i==size || arrPts0[i].pencode==penMove )
// 			{
// 				int ret = GraphAPI::GIsPtInRegion(ptcen, pts+start, i-start);
// 				if(ret==0 || ret==1)
// 				{
// 					bPtInLine = TRUE;
// 					break;
// 				}
// 				else if(ret==2)
// 				{
// 					nSum++;
// 				}
// 				start = i;
// 			}	
// 		}
// 
// 		if(bPtInLine) continue;
// 
// 		if( (nSum%2)==0 )
// 		{
// 			return false;
// 		}
// 	}
	//4判断中心点是否在内部
	PT_3D pt0 ,pt1;
	COPY_3DPT(pt0, arrPts1[0]);
	pObj1->GetCenter(&pt0, &pt1);
	if(!pObj0->bPtIn(&pt1))
	{
		return false;
	}

	return true;
}

void CSurfaceincludeChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	vector<item> container;
	//把选定层中的面地物选出来
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				
				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					item data;
					data.pFtr = pFtr;
					data.e = pFtr->GetGeometry()->GetEnvelope();
					container.push_back(data);
				}
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					
					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					{
						item data;
						data.pFtr = pFtr;
						data.e = pFtr->GetGeometry()->GetEnvelope();
						container.push_back(data);
 					}
				}
			}
		}			
	}

	MARK_SELECTION;

	GProgressStart(container.size());
	vector<item>::const_iterator it,it0;
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		CFeature *pFtr = (*it).pFtr;

		SKIP_NOT_SELECTION;

		Envelope e = (*it).e;
		for (it0=it+1;it0<container.end();it0++)
		{
			if(it==it0)
				continue;
			if(!e.bIntersect(&((*it0).e)))
				continue;
			CFeature *pFtr1 = (*it0).pFtr;
			if(IsSurfaceInclude(pFtr, pFtr1))
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->AddAssociatedFtr(pFtr1);
				PT_3DEX center;
				center = pFtr1->GetGeometry()->GetDataPoint(0);
				center.z = 0;
				m_pChkResMgr->SetAssociatedPos(center);
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_INCLUDE_AREA));
				m_pChkResMgr->EndResItem();
			}
			else if(IsSurfaceInclude(pFtr1, pFtr))
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr1);
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				PT_3DEX center;
				center = pFtr->GetGeometry()->GetDataPoint(0);
				center.z = 0;
				m_pChkResMgr->SetAssociatedPos(center);
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_INCLUDE_AREA));
				m_pChkResMgr->EndResItem();
			}
		}
	}	
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}


void CSurfaceincludeChkCmd::GetAllReasons(CStringArray& reasons)
{
	CString temp;
	temp.Format(_T("%s:%d"),StrFromResID(IDS_CHK_INCLUDE_AREA),(int)0);
	reasons.Add(StrFromResID(IDS_CHK_INCLUDE_AREA));
}


//////////////////////////////////////////////////////////////////////////
CPartOverlapLineWithTolChkCmd::CPartOverlapLineWithTolChkCmd()
{
	m_lfToler = 0.0001;
	m_geotypelimit = 1;
}

CPartOverlapLineWithTolChkCmd::~CPartOverlapLineWithTolChkCmd()
{

}

CString CPartOverlapLineWithTolChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CPartOverlapLineWithTolChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_PART_OVERLAP_WITH_TOL);
}

void CPartOverlapLineWithTolChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strLayerCodes1;
	tab.AddValue("LayNames1",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayerCodes2;
	tab.AddValue("LayNames2",&CVariantEx(var));

	var = (double)m_lfToler;
	tab.AddValue(CHK_PARTOVERLAP_TOLER,&CVariantEx(var));

	var = (long)m_geotypelimit;
	tab.AddValue("geo_limit",&CVariantEx(var));
}

void CPartOverlapLineWithTolChkCmd::SetParams(CValueTable& tab)
{
 	const CVariantEx *var;
	if( tab.GetValue(0,"LayNames1",var) )
	{
		m_strLayerCodes1 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"LayNames2",var) )
	{
		m_strLayerCodes2 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_PARTOVERLAP_TOLER,var) )
	{
		m_lfToler= (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,"geo_limit",var) )
	{
		m_geotypelimit = (long)(_variant_t)*var;
	}
}

void CPartOverlapLineWithTolChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CPartOverlapLineWithTolChkCmd"),_T("CPartOverlapLineWithTolChkCmd"));
	param->AddLayerNameParamEx("LayNames1",(LPCTSTR)m_strLayerCodes1,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS)+_T("1"),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
	param->AddLayerNameParamEx("LayNames2",(LPCTSTR)m_strLayerCodes2,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS)+_T("2"),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
	param->AddParam(CHK_PARTOVERLAP_TOLER,m_lfToler,StrFromResID(IDS_CHK_CMD_PART_OVERLAP_TOL));

	param->BeginOptionParam(_T("geo_limit"),StrFromResID(IDS_GEOTYPE_LIMIT));
	param->AddOption(StrFromResID(IDS_GEO_NOLIMIT),0,' ',m_geotypelimit==0);
	param->AddOption(StrFromResID(IDS_GEO_CURVE),1,' ',m_geotypelimit==1);
	param->EndOptionParam();
}

static bool GetOverlapSection(double xS1,double yS1,double xE1,double yE1,double xS2,double yS2,double xE2,double yE2,double toler,
							  double *xs0,double *ys0,double *xe1,double *ye1/*,double *x2,double *y2,double *x3,double *y3*/)

{
	//将线段1的两点投影到线段2上，垂距超限差那么就不重叠
	double x0,y0,x1,y1,t0,t1;
	GraphAPI::GGetPerpendicular(xS2,yS2,xE2,yE2,xS1,yS1,&x0,&y0,&t0);
	if(fabs(xS1-x0)<toler&&fabs(yS1-y0)<toler)
		;
	else
		return false;
	GraphAPI::GGetPerpendicular(xS2,yS2,xE2,yE2,xE1,yE1,&x1,&y1,&t1);
	if(fabs(xE1-x1)<toler&&fabs(yE1-y1)<toler)
		;
	else
		return false;

	//不重叠
	if( (t0<=0 && t1<=0) || (t0>=1 && t1>=1) )
		return false;	
	

	//两线段长度的和
	double dis1 = sqrt((xS2-xE2)*(xS2-xE2) + (yS2-yE2)*(yS2-yE2)) + sqrt((x0-x1)*(x0-x1) + (y0-y1)*(y0-y1)); 

	//计算线段叠加后的总长
	double x2=xS2, y2=yS2, x3=xE2, y3=yE2;
	if( t0<0 )
	{
		if( t0<t1 )
		{
			x2 = x0;
			y2 = y0;
		}
		else
		{
			x2 = x1;
			y2 = y1;
		}
	}
	if( t1>1 )
	{
		if( t1<t0 )
		{
			x3 = x0;
			y3 = y0;
		}
		else
		{
			x3 = x1;
			y3 = y1;
		}
	}

	double dis2 = sqrt((x2-x3)*(x2-x3) + (y2-y3)*(y2-y3));

	//计算重叠部分长度
	double dis3 = dis1-dis2;

	//是否超过限差
	if( dis3<GraphAPI::g_lfDisTolerance )
		return false;

	if( t0<t1 )
	{
		if( t0<0 )
		{
			if(xs0) *xs0 = xS2;  if(ys0) *ys0 = yS2;
		}
		else
		{
			if(xs0) *xs0 = xS1;  if(ys0) *ys0 = yS1;
		}

		if( t1>1 )
		{
			if(xe1) *xe1 = xE2;  if(ye1) *ye1 = yE2;
		}
		else
		{
			if(xe1) *xe1 = xE1;  if(ye1) *ye1 = yE1;
		}
	}
	else
	{
		if( t1<0 )
		{
			if(xs0) *xs0 = xS2;  if(ys0) *ys0 = yS2;
		}
		else
		{
			if(xs0) *xs0 = xE1;  if(ys0) *ys0 = yE1;
		}

		if( t0>1 )
		{
			if(xe1) *xe1 = xE2;  if(ye1) *ye1 = yE2;
		}
		else
		{
			if(xe1) *xe1 = xS1;  if(ye1) *ye1 = yS1;
		}
	}

	return true;
}

void CPartOverlapLineWithTolChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource* pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	int nLayerCnt = pDS->GetFtrLayerCount();
	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = NULL,*pFtr0 = NULL;

	vector<CFeature*> container, container2;
	for (int i=0;i<nLayerCnt;i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if (pLayer==NULL||!pLayer->IsVisible()||pLayer->IsLocked())continue;

		if( m_strLayerCodes1.IsEmpty() || CheckNameForLayerCode(pDS,pLayer->GetName(),m_strLayerCodes1) )
		{
			int nObj = pLayer->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr = pLayer->GetObject(j);
				if (pFtr==NULL||!pFtr->IsVisible() )continue;

				CGeometry *pGeo = pFtr->GetGeometry();
				if(!pGeo) continue;
				if(m_geotypelimit==1 && !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				{
					continue;
				}
				
				pFtr->SetAppFlag(0);
				container.push_back(pFtr);
			}
		}
		if( m_strLayerCodes2.IsEmpty() || CheckNameForLayerCode(pDS,pLayer->GetName(),m_strLayerCodes2) )
		{
			int nObj = pLayer->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr = pLayer->GetObject(j);
				if (pFtr==NULL||!pFtr->IsVisible() )continue;
				
				CGeometry *pGeo = pFtr->GetGeometry();
				if(!pGeo) continue;
				if(m_geotypelimit==1 && !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				{
					continue;
				}
				
				pFtr->SetAppFlag(0);
				container2.push_back(pFtr);
			}
		}
	}
	CArray<CFeature*,CFeature*> arrFtrs;
	CArray<PT_3DEX,PT_3DEX> arrPts0,arrPts1;
	PT_3DEX pt0,pt1,pt2,pt3;

	CDataQuery *pDQ = m_pDoc->GetDataQuery();

	MARK_SELECTION;

	GProgressStart(container.size());

	double retX0,retY0,retX1,retY1;

	vector<CFeature*>::const_iterator it,it2;
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		pFtr = *it/*arrFtrs[k]*/;
		pFtr->SetAppFlag(1);
 
		SKIP_NOT_SELECTION;

		pFtr->GetGeometry()->GetShape(arrPts0);
		if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		{
			if( arrPts0.GetSize()<2 )
				continue;

			for (int l=0;l<arrPts0.GetSize()-1;l++)
			{
				Envelope e;
				PT_3D pt3ds[2];
				pt3ds[0] = arrPts0[l];
				pt3ds[1] = arrPts0[l+1];
				e.CreateFromPts(pt3ds, 2);

				for (it2= container2.begin();it2<container2.end();it2++)
				{
					pFtr0 = *it2;
					if( pFtr0==pFtr )continue;
					if(pFtr0->GetAppFlag()) continue;

					CGeometry *pGeo = pFtr0->GetGeometry();
					if(!pGeo) continue;
					if(m_geotypelimit==1 && !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
					{
						continue;
					}
					Envelope e2 = pGeo->GetEnvelope();
					if(!e.bIntersect(&e2)) continue;
					
					pGeo->GetShape(arrPts1);
					for (int n=0;n<arrPts1.GetSize()-1;n++)
					{
						pt2 = arrPts1[n];
						pt3 = arrPts1[n+1];								
						if(GetOverlapSection(pt3ds[0].x,pt3ds[0].y,pt3ds[1].x,pt3ds[1].y,pt2.x,pt2.y,pt3.x,pt3.y,m_lfToler,&retX0,&retY0,&retX1,&retY1))
						{
							m_pChkResMgr->BeginResItem();
							m_pChkResMgr->AddAssociatedFtr(pFtr);
							m_pChkResMgr->AddAssociatedFtr(pFtr0);
							m_pChkResMgr->SetAssociatedPos(PT_3D((retX0+retX1)/2,(retY0+retY1)/2,0));
							m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_PART_OVERLAP_WITH_TOL));
							m_pChkResMgr->EndResItem();
						}
					}
				}
			}
		}
	}

	GProgressEnd();

	for (it= container.begin();it<container.end();it++)
	{
		(*it)->SetAppFlag(0);
	}

	CLEAR_MARK_SELECTION;
}


void CPartOverlapLineWithTolChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_CMD_PART_OVERLAP_WITH_TOL));
	reasons.Add(StrFromResID(IDS_CHK_PART_OVERLAP));
}


//////////////////////////////////////////////////////////////////////////
CGeoCurveClosedChkCmd::CGeoCurveClosedChkCmd()
{
	m_strLayerCodes = _T("");
}

CGeoCurveClosedChkCmd::~CGeoCurveClosedChkCmd()
{

}

CString CGeoCurveClosedChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CGeoCurveClosedChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_CURVE_CLOSED);
}

void CGeoCurveClosedChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;	
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_GEOCURVE_CLOSED_LAYCODES,&CVariantEx(var));
	var = (bool)m_bChkClosed;
	tab.AddValue(CHK_GEOCURVE_CLOSED_CHKCLOSE,&CVariantEx(var));
}

void CGeoCurveClosedChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;	
	if( tab.GetValue(0,CHK_GEOCURVE_CLOSED_LAYCODES,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_GEOCURVE_CLOSED_CHKCLOSE,var) )
	{
		m_bChkClosed = (bool)(_variant_t)*var;
	}

}

void CGeoCurveClosedChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CGeoCurveClosedChkCmd"),_T("CGeoCurveClosedChkCmd"));
	param->AddLayerNameParamEx(CHK_GEOCURVE_CLOSED_LAYCODES,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddParam(CHK_GEOCURVE_CLOSED_CHKCLOSE,(bool)m_bChkClosed,StrFromResID(IDS_CHKCMD_DES_CHKCLOSED));
}

void CGeoCurveClosedChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	vector<CFeature*> container;
	//把选定层中的面地物选出来
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				
				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{				
					container.push_back(pFtr);
				}						
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					
					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
					{						
						container.push_back(pFtr);
					}
				}
			}
		}			
	}

	MARK_SELECTION;

	GProgressStart(container.size());
	vector<CFeature*>::const_iterator it;
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		CFeature *pFtr = (*it);

		SKIP_NOT_SELECTION;

		if(m_bChkClosed)
		{
			if(((CGeoCurveBase*)pFtr->GetGeometry())->IsClosed())
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CLOSED));
				m_pChkResMgr->EndResItem();
			}
		}
		else
		{
			if(!((CGeoCurveBase*)pFtr->GetGeometry())->IsClosed())
			{
				
  				m_pChkResMgr->BeginResItem();
 				m_pChkResMgr->AddAssociatedFtr(pFtr);
  				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
                m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_NOT_CLOSED));
  				m_pChkResMgr->EndResItem();
			}
		}	
	
	}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
	
}


void CGeoCurveClosedChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_CLOSED));
	reasons.Add(StrFromResID(IDS_CHK_NOT_CLOSED));
}



//////////////////////////////////////////////////////////////////////////
CContourValidChkCmd::CContourValidChkCmd()
{
	m_strIdxContourCode = _T("");
	m_strStdContourCode = _T("");
	m_lfContourInterval = 1.0;
	m_nStdContourCnt = 4;
	m_bCheckLoseContour = true;
}

CContourValidChkCmd::~CContourValidChkCmd()
{

}

CString CContourValidChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_CONTOUR);
}

CString CContourValidChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_CONTOUR_VALID);
}

void CContourValidChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strIdxContourCode;
	tab.AddValue(CHK_INDEX_CONTOUR_CODE,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strStdContourCode;
	tab.AddValue(CHK_STD_CONTOUR_CODE,&CVariantEx(var));
	var = (double)m_lfContourInterval;
	tab.AddValue(CHK_CONTOUR_INTERVAL,&CVariantEx(var));
	var = (long)m_nStdContourCnt;
	tab.AddValue(CHK_STD_CONTOUR_CNT,&CVariantEx(var));
	var = m_bCheckLoseContour;
	tab.AddValue("bCheckLoseContour", &CVariantEx(var));
}

void CContourValidChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_INDEX_CONTOUR_CODE,var) )
	{
		m_strIdxContourCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_STD_CONTOUR_CODE,var) )
	{
		m_strStdContourCode= (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_CONTOUR_INTERVAL,var) )
	{
		m_lfContourInterval = (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_STD_CONTOUR_CNT,var) )
	{
		m_nStdContourCnt = (long)(_variant_t)*var;
	}	
	if(tab.GetValue(0, "bCheckLoseContour", var))
	{
		m_bCheckLoseContour = (bool)(_variant_t)*var;
	}
}

void CContourValidChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CContourValidChkCmd"),_T("CContourValidChkCmd"));

	param->AddLayerNameParam(CHK_INDEX_CONTOUR_CODE,(LPCTSTR)m_strIdxContourCode,StrFromResID(IDS_INDEX_CONTOUR),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParam(CHK_STD_CONTOUR_CODE,(LPCTSTR)m_strStdContourCode,StrFromResID(IDS_STD_CONTOUR),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddParam(CHK_CONTOUR_INTERVAL,m_lfContourInterval,StrFromResID(IDS_CONTOUR_INTERVAL));
	param->AddParam(CHK_STD_CONTOUR_CNT,m_nStdContourCnt,StrFromResID(IDS_CMDPLANE_STD_CONTOUR_CNT));
	param->AddParam("bCheckLoseContour", m_bCheckLoseContour, StrFromResID(IDS_CHK_LOSECONTOUR));
}


bool CContourValidChkCmd::CheckObjForZ( CGeometry *pObj)
{
	if (!pObj) return false;
	
	CArray<PT_3DEX,PT_3DEX> pts;
	pObj->GetShape(pts);
	
	int size = pts.GetSize();
	if (size < 2) return false;
	
	double z = pts[0].z;
	for(int i=1;i<size;i++)
	{
		PT_3DEX ptex = pts[i];
		if (fabs(z-ptex.z)>1e-4)
		{
			return false;
		}
	}
	return true;
}

bool CContourValidChkCmd::CheckObjForZAndMarkPt( CGeometry *pObj,CArray<PT_3DEX,PT_3DEX> &arrPts)
{
	if (!pObj) return false;
	
	CArray<PT_3DEX,PT_3DEX> pts;
	pObj->GetShape(pts);
	
	int size = pts.GetSize();
	if (size < 2) return false;
	if (size<5)
	{
		double z = pts[0].z;
		for(int i=1;i<size;i++)
		{
			PT_3DEX ptex = pts[i];
			if (fabs(z-ptex.z)>GraphAPI::g_lfZTolerance)
			{
				arrPts.Add(pts[0]);
				return false;
			}
		}
		return true;
	}
	else
	{
		//先检查前5个点
		CArray<double ,double> Z;
		int cnt[5] = {0};
		double zRes;
		PT_3DEX ptex;
		for (int i=0;i<5;i++)
		{
			ptex = pts[i];
			for (int j=0;j<Z.GetSize();j++)
			{
				if (fabs(ptex.z-Z[j])<=GraphAPI::g_lfZTolerance)
				{
					cnt[j]++;
					if (cnt[j]>=3)
					{
						zRes = ptex.z;
						goto Ok;
					}
					break;
				}
			}
			if (j<Z.GetSize())
			{
				continue;
			}
			Z.Add(ptex.z);
			cnt[Z.GetSize()-1]=1;
		}
Ok:
		if (i<5)//成功
		{
			for(int m=0;m<size;m++)
			{
				PT_3DEX ptex = pts[m];
				if (fabs(zRes-ptex.z)>=GraphAPI::g_lfZTolerance)
				{
					arrPts.Add(ptex);				
				}
			}
			if (arrPts.GetSize()>0)
			{
				return false;
			}
			return true;
			
		}
		else
		{
			arrPts.Add(pts[0]);
			return false;
		}
	}
}

bool CContourValidChkCmd::CheckObjBetweenTwoObjs(CFeature *pFtr1, CFeature *pFtr2, PT_3DEX *ret, double *len)
{
	CGeometry *pObj1 = pFtr1->GetGeometry();
	CGeometry *pObj2 = pFtr2->GetGeometry();

	CArray<PT_3DEX,PT_3DEX> pts1, pts2;
	const CShapeLine *pShape1 = pObj1->GetShape();
	const CShapeLine *pShape2 = pObj2->GetShape();

	pShape1->GetPts(pts1);
	pShape2->GetPts(pts2);

	PT_3DEX line0[2];
	
	GraphAPI::GGetMiddlePt(pts1.GetData(),pts1.GetSize(),&line0[0]);
	GraphAPI::GGetMiddlePt(pts2.GetData(),pts2.GetSize(),&line0[1]);

	if (ret)
	{
		GraphAPI::GGetMiddlePt(line0,2,ret);
	}

	if (len)
	{
		*len = GraphAPI::GGet2DDisOf2P(line0[0],line0[1]);
	}

	PT_3D line[2];
	
	m_pDoc->GetCoordWnd().m_pSearchCS->GroundToClient(&line0[0],&line[0]);
	m_pDoc->GetCoordWnd().m_pSearchCS->GroundToClient(&line0[1],&line[1]);

	m_pDoc->GetDataQuery()->FindIntersectObject(line,2,Envelope(),m_pDoc->GetCoordWnd().m_pSearchCS,FALSE);

	int nObj ;
	const CPFeature *ftr = m_pDoc->GetDataQuery()->GetFoundHandles(nObj);
	
	int num = nObj;
	for (int i=0; i<nObj; i++)
	{
		if (ftr[i] == pFtr1 || ftr[i] == pFtr2 || !ftr[i]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
		{
			num--;
			continue;
		}

		CFtrLayer *pFtrLayer = m_pDoc->GetDlgDataSource()->GetFtrLayerOfObject(ftr[i]);

		CString strLayer = pFtrLayer->GetName();
		if (strLayer.CompareNoCase(StrFromResID(IDS_INDEX_CONTOUR)) != 0 && strLayer.CompareNoCase(StrFromResID(IDS_STD_CONTOUR)) != 0)
		{
			num--;
			continue;
		}

		PT_3DEX pt = ftr[i]->GetGeometry()->GetDataPoint(0);

		if ( (pts1[0].z > pts2[0].z && (pt.z > pts1[0].z || pt.z < pts2[0].z)) || 
			(pts1[0].z < pts2[0].z && (pt.z > pts2[0].z || pt.z < pts1[0].z)) )
		{
			num--;
			continue;
		}

		
	}
	
	return (num==1);
}



bool CContourValidChkCmd::CheckObjBetweenTwoObjs(CFeature *pFtr1, CFeature *pFtr2, PT_3DEX pt1, PT_3DEX pt2)
{
	PT_3DEX line0[2];
	memcpy( &line0[0], &pt1, sizeof(PT_3DEX) );
	memcpy( &line0[1], &pt2, sizeof(PT_3DEX) );

	PT_3D line[2];	
	m_pDoc->GetCoordWnd().m_pSearchCS->GroundToClient(&line0[0],&line[0]);
	m_pDoc->GetCoordWnd().m_pSearchCS->GroundToClient(&line0[1],&line[1]);

	m_pDoc->GetDataQuery()->FindIntersectObject(line,2,Envelope(),m_pDoc->GetCoordWnd().m_pSearchCS,FALSE);

	int nObj ;
	const CPFeature *ftr = m_pDoc->GetDataQuery()->GetFoundHandles(nObj);
	
	int num = nObj;
	for (int i=0; i<nObj; i++)
	{
		if (ftr[i] == pFtr1 || ftr[i] == pFtr2 || !ftr[i]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
		{
			num--;
			continue;
		}

		CFtrLayer *pFtrLayer = m_pDoc->GetDlgDataSource()->GetFtrLayerOfObject(ftr[i]);

		CString strLayer = pFtrLayer->GetName();
		if (strLayer.CompareNoCase(StrFromResID(IDS_INDEX_CONTOUR)) != 0 && strLayer.CompareNoCase(StrFromResID(IDS_STD_CONTOUR)) != 0)
		{
			num--;
			continue;
		}

		PT_3DEX pt = ftr[i]->GetGeometry()->GetDataPoint(0);

	//	if ( (pts1[0].z > pts2[0].z && (pt.z > pts1[0].z || pt.z < pts2[0].z)) || 
	//		(pts1[0].z < pts2[0].z && (pt.z > pts2[0].z || pt.z < pts1[0].z)) )
		if ( (pt1.z > pt2.z && (pt.z > pt1.z || pt.z < pt2.z)) || 
			(pt1.z < pt2.z && (pt.z > pt2.z || pt.z < pt1.z)) )
		{
			num--;
			continue;
		}

		
	}
	
	return (num==1);
}

double CContourValidChkCmd::GetDisOf2Obj(CFeature* line1, CFeature* line2, PT_3DEX& pt1, PT_3DEX& pt2)
{
	//分别计算两条线的中点到别一条线的最短距离
	//返回较小的那个，并取出对应的两点坐标

	CGeometry *pObj1 = line1->GetGeometry();
	CGeometry *pObj2 = line2->GetGeometry();

	CArray<PT_3DEX,PT_3DEX> pts1, pts2;
	const CShapeLine *pShape1 = pObj1->GetShape();
	const CShapeLine *pShape2 = pObj2->GetShape();

	pShape1->GetPts(pts1);
	pShape2->GetPts(pts2);

	PT_3DEX ptMid1, ptMid2;
	GraphAPI::GGetMiddlePt(pts1.GetData(),pts1.GetSize(),&ptMid1);
	GraphAPI::GGetMiddlePt(pts2.GetData(),pts2.GetSize(),&ptMid2);

	double len1 = 0, len2 = 0;
	int retIdx = -1;
	PT_3DEX retPt;
	len1 = GraphAPI::GGetNearstDisPToPointSet2D( pts2.GetData(), pts2.GetSize(), ptMid1, retPt, &retIdx);//line1的中点到line2的距离
	retPt.z = pts2[0].z;

	memcpy( &pt1, &ptMid1, sizeof(PT_3DEX) );
	memcpy( &pt2, &retPt, sizeof(PT_3DEX) );

	len2 = GraphAPI::GGetNearstDisPToPointSet2D( pts1.GetData(), pts1.GetSize(), ptMid2, retPt, &retIdx);
	retPt.z = pts1[0].z;

	if(len2 < len1)
	{
		memcpy( &pt1, &retPt, sizeof(PT_3DEX) );
		memcpy( &pt2, &ptMid2, sizeof(PT_3DEX) );
		return len2;
	}

	return len1;
}

//比较两个三角形是否有公共边
static bool comparetri(MYPT_3D *tri1, MYPT_3D *tri2)
{
	int i, j, nSum = 0;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (GraphAPI::GIsEqual2DPoint(&tri1[i], &tri2[j]))
			{
				nSum++;
			}
			if (nSum >= 2)
			{
				return true;
			}
		}
	}
	return false;
}

extern void KickoffSameGeoPts(CArray<MYPT_3D, MYPT_3D>& arr);
extern double GetDistanceFromPointToCurve(CArray<PT_3DEX, PT_3DEX>& pts, PT_3D& pt, PT_3D *retpt = NULL);

void CContourValidChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if (!pDS)return;
	if (m_strIdxContourCode.IsEmpty() || m_strStdContourCode.IsEmpty() || m_lfContourInterval <= 0 || m_nStdContourCnt <= 0)
		return;
	//vector<CFeature*> container;

	int nSum = 0;
	CPtrArray arrLayersIdx, arrLayersStd;
	if (!pDS->GetFtrLayer(m_strIdxContourCode, NULL, &arrLayersIdx))
		return;
	int nTarIdx = arrLayersIdx.GetSize();

	for (int i = 0; i < nTarIdx; i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrLayersIdx.GetAt(i);
		if (!pLayer || !pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (int j = 0; j < nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr || !pFtr->IsVisible())
				continue;
			nSum++;
		}
	}

	if (!pDS->GetFtrLayer(m_strStdContourCode, NULL, &arrLayersStd))
		return;
	int nTarStd = arrLayersStd.GetSize();
	for (i = 0; i < nTarStd; i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrLayersStd.GetAt(i);
		if (!pLayer || !pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (int j = 0; j < nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr || !pFtr->IsVisible())
				continue;
			nSum++;
		}
	}

	CFtrArray arrFtrs;

	MARK_SELECTION;

	for (i = 0; i < nTarIdx; i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrLayersIdx.GetAt(i);
		if (!pLayer || !pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (int j = 0; j < nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr || !pFtr->IsVisible())
				continue;

			SKIP_NOT_SELECTION;

			if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				CArray<PT_3DEX, PT_3DEX> arrPts;
				if (!CheckObjForZAndMarkPt(pFtr->GetGeometry(), arrPts))
				{
					for (int k = 0; k < arrPts.GetSize(); k++)
					{
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetAssociatedPos(arrPts[k]);
						m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_IDXLINE_Z_NOT_SAME));
						m_pChkResMgr->EndResItem();
					}
				}
				else
				{
					PT_3DEX expt;
					expt = pFtr->GetGeometry()->GetDataPoint(0);

					int n = (int)floor(expt.z / m_lfContourInterval);
					int n0 = (int)ceil(expt.z / m_lfContourInterval);
					if (fabs(expt.z - n*m_lfContourInterval) > 1e-4&&fabs(expt.z - n0*m_lfContourInterval) > 1e-4)
					{
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
						m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_IDX_Z_NOT_VALID));
						m_pChkResMgr->EndResItem();
						continue;
					}
					if (fabs(expt.z - n0*m_lfContourInterval) <= 1e-4&&!(fabs(expt.z - n*m_lfContourInterval) <= 1e-4))
						n = n0;
					if (0 != n % (m_nStdContourCnt + 1))
					{
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
						m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_IDX_Z_NOT_VALID));
						m_pChkResMgr->EndResItem();
					}
					else
					{
						arrFtrs.Add(pFtr);
					}
				}
			}
		}
	}

	for (i = 0; i < nTarStd; i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrLayersStd.GetAt(i);
		if (!pLayer || !pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (int j = 0; j < nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr || !pFtr->IsVisible())
				continue;

			SKIP_NOT_SELECTION;

			if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				CArray<PT_3DEX, PT_3DEX> arrPts;
				if (!CheckObjForZAndMarkPt(pFtr->GetGeometry(), arrPts))
				{
					for (int k = 0; k < arrPts.GetSize(); k++)
					{
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetAssociatedPos(arrPts[k]);
						m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_STDLINE_Z_NOT_SAME));
						m_pChkResMgr->EndResItem();
					}
				}
				else
				{
					PT_3DEX expt;
					expt = pFtr->GetGeometry()->GetDataPoint(0);

					int n = (int)floor(expt.z / m_lfContourInterval);
					int n0 = (int)ceil(expt.z / m_lfContourInterval);
					if(fabs(expt.z - n*m_lfContourInterval) > 1e-4&&fabs(expt.z - n0*m_lfContourInterval) > 1e-4)
					{
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
						m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_STDLINE_Z_NOT_VALID));
						m_pChkResMgr->EndResItem();
						continue;
					}

					if(fabs(expt.z - n0*m_lfContourInterval) <= 1e-4)
						n = n0;

					if (0 == (n % (m_nStdContourCnt + 1)))
					{
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
						m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_STDLINE_Z_NOT_VALID));
						m_pChkResMgr->EndResItem();
					}
					else
					{
						arrFtrs.Add(pFtr);
					}
				}
			}
		}
	}

	if(m_bCheckLoseContour)
	{
		CArray<MYPT_3D, MYPT_3D> arrAll, arrCur;
		MYPT_3D geopt;

		int k = 0;
		for(i = 0; i < arrFtrs.GetSize(); i++)
		{
			CGeometry *pGeo = arrFtrs[i]->GetGeometry();
			if(!pGeo) continue;

			CArray<PT_3DEX, PT_3DEX> pts;
			const CShapeLine *pShp = pGeo->GetShape();
			if(!pShp) continue;
			pShp->GetPts(pts);
			arrCur.RemoveAll();
			int nPt = pts.GetSize();
			for(int n = 0; n < nPt; n++)
			{
				PT_3DEX expt = pts[n];
				geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;
				geopt.type = 1;
				geopt.id = k;
				arrCur.Add(geopt);
			}

			if(arrCur.GetSize()>0)
			{
				arrAll.Append(arrCur);
				k++;
			}
		}

		CMYTinObj tin;
		MyTriangle::createTIN(arrAll.GetData(), arrAll.GetSize(), &tin.tins, &tin.nTIN, &tin.pts, &tin.npt);

		CDataQueryEx *pDQ = m_pDoc->GetDataQuery();

		MYPT_3D tri[3];
		GProgressStart(tin.nTIN * 3);
		int *flags = new int[tin.nTIN];
		for(i = 0; i<tin.nTIN; i++)
		{
			flags[i] = -1;//初始值；三角形正常
		}
		for(i = 0; i<tin.nTIN; i++)
		{
			GProgressStep();
			if(!tin.GetTriangle(i, tri))
			{
				continue;
			}

			//三角形有可能三点共线，需要排出掉
			double t = GraphAPI::GGetMultiply(tri[0], tri[1], tri[2]);
			if(fabs(t) < 1e-4) continue;

			if(fabs(tri[0].z - tri[1].z) > m_lfContourInterval + GraphAPI::g_lfDisTolerance ||
				fabs(tri[0].z - tri[2].z)>m_lfContourInterval + GraphAPI::g_lfDisTolerance ||
				fabs(tri[1].z - tri[2].z) > m_lfContourInterval + GraphAPI::g_lfDisTolerance)
			{
				flags[i] = -2;//三角形不正常
			}
		}

		//合并三角形;      向后查找，合并不完全
		for(i = 0; i < tin.nTIN; i++)
		{
			GProgressStep();
			if(flags[i] == -1) continue;
			if(!tin.GetTriangle(i, tri))
			{
				continue;
			}

			MYPT_3D tri1[3];
			for(int j = i + 1; j < tin.nTIN; j++)
			{
				if(flags[j] != -2) continue;
				if(!tin.GetTriangle(j, tri1))
				{
					continue;
				}

				MYPT_3D tri2[3];
				if(comparetri(tri, tri1))//如果有公关边
				{
					int pos = flags[i]>=0 ? flags[i] : i;
					flags[pos]--;
					flags[j] = pos;
				}
			}
		}

		for(i = 0; i < tin.nTIN; i++)
		{
			GProgressStep();

			if(flags[i] >-6) continue;//大于等于5个三角形一组，才报错

			if(!tin.GetTriangle(i, tri))
			{
				continue;
			}

			PT_3D ptg;
			ptg.x = (tri[0].x + tri[1].x + tri[2].x) / 3;
			ptg.y = (tri[0].y + tri[1].y + tri[2].y) / 3;
			ptg.z = (tri[0].z + tri[1].z + tri[2].z) / 3;

			// 加入检查结果表
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->SetAssociatedPos(ptg);
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_LOSECONTOUR));
			m_pChkResMgr->EndResItem();
		}

		if(flags) delete[] flags;

		tin.Clear();

		GProgressEnd();
	}

	CLEAR_MARK_SELECTION;
}

void CContourValidChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_IDXLINE_Z_NOT_SAME));
	reasons.Add(StrFromResID(IDS_CHK_IDX_Z_NOT_VALID));
	reasons.Add(StrFromResID(IDS_CHK_STDLINE_Z_NOT_SAME));
	reasons.Add(StrFromResID(IDS_CHK_STDLINE_Z_NOT_VALID));
	reasons.Add(StrFromResID(IDS_CHK_LOSECONTOUR));
}


//////////////////////////////////////////////////////////////////////////

CStringArray  CPLChkCmd::m_strcodeArray;
CPLChkCmd::CPLChkCmd()
{
	m_strContourCode = _T("");
	m_strTestPtCode = _T("");
	m_lfContourInterval = 5.0;
	m_lfMinHeightDiff = 0.0;
}

CPLChkCmd::~CPLChkCmd()
{

}

CString CPLChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_CONTOUR);
}

CString CPLChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_PL_ERR);
}

void CPLChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strContourCode;
	tab.AddValue(CHK_PL_CONTOUR_CODE,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strTestPtCode;
	tab.AddValue(CHK_PL_TESTPT_CODE,&CVariantEx(var));
	var = (double)m_lfContourInterval;
	tab.AddValue(CHK_PL_CONTOUR_INTERVAL,&CVariantEx(var));
	var = (double)m_lfMinHeightDiff;
	tab.AddValue(CHK_PL_MIN_HEI_DIFF,&CVariantEx(var));
}

void CPLChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_PL_CONTOUR_CODE,var) )
	{
		m_strContourCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_PL_TESTPT_CODE,var) )
	{
		m_strTestPtCode= (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_PL_CONTOUR_INTERVAL,var) )
	{
		m_lfContourInterval = (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_PL_MIN_HEI_DIFF,var) )
	{
		m_lfMinHeightDiff = (double)(_variant_t)*var;
	}	
}

void CPLChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CPLChkCmd"),_T("CPLChkCmd"));

	param->AddLayerNameParamEx(CHK_PL_CONTOUR_CODE,(LPCTSTR)m_strContourCode,StrFromResID(IDS_CHKCMD_DES_CONTOUR),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParamEx(CHK_PL_TESTPT_CODE,(LPCTSTR)m_strTestPtCode,StrFromResID(IDS_CHKCMD_DES_TESTPT_CODE),NULL,LAYERPARAMITEM_POINT);
	param->AddParam(CHK_PL_CONTOUR_INTERVAL,m_lfContourInterval,StrFromResID(IDS_CONTOUR_INTERVAL));
	param->AddParam(CHK_PL_MIN_HEI_DIFF,m_lfMinHeightDiff,StrFromResID(IDS_CHKCMD_DES_MIN_HEI));
}

BOOL CPLChkCmd::CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  Layercode)
{
	if (!pDS||!pFtr||strlen(Layercode)<=0)
	{
		return FALSE;
	}
	CFtrLayer *player = pDS->GetFtrLayerOfObject(pFtr);
	if(!player) return FALSE;
	const char *name = player->GetName();
	CString ret(Layercode);

	__int64 code = _atoi64(Layercode);
	if(code>0)
	{
		CConfigLibManager *pConfi = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
		CScheme *pScheme = pConfi->GetScheme(pDS->GetScale());
		if(!pScheme)return FALSE;
		pScheme->FindLayerIdx(TRUE,code,ret);
	}
	
	if( stricmp(ret,name)==0 )return TRUE;
	
	return FALSE;
	
}
#include "GrTrim.h "
void CPLChkCmd::CheckObj(CFeature *pFtr)
{
	PT_3DEX expt;
	PT_3D pt3d[8];
	Envelope e;
	CArray<FTR_HANDLE,FTR_HANDLE> arrIds1, arrIds2;
	
	CGeometry *pObj = pFtr->GetGeometry();
	expt = pObj->GetDataPoint(0);
	
	CTree8Search * pSearch = (CTree8Search *)m_pDoc->GetDataQuery()->GetActiveSearcher();
	double r;
	int  nObj = ((CDlgDataQuery*)(m_pDoc->GetDataQuery()))->FindNearestObjectByObjNum(expt,5,NULL,FALSE,&r,NULL);
	if( nObj<2 )
		return;

	Envelope e0 = pSearch->GetRegion();
	double dx,dy;
	const CPFeature *ftr = m_pDoc->GetDataQuery()->GetFoundHandles(nObj);
	nObj = nObj<=5?nObj:5;
	
	dx = r; dy = r;
	
	pt3d[0].x = expt.x-dx; pt3d[0].y = expt.y-dy; pt3d[0].z = expt.z;
	pt3d[1].x = expt.x+dx; pt3d[1].y = expt.y+dy; pt3d[1].z = expt.z;
	pt3d[2].x = expt.x-dx; pt3d[2].y = expt.y+dy; pt3d[2].z = expt.z;
	pt3d[3].x = expt.x+dx; pt3d[3].y = expt.y-dy; pt3d[3].z = expt.z;
	
	pt3d[4].x = expt.x-dx; pt3d[4].y = expt.y; pt3d[4].z = expt.z;
	pt3d[5].x = expt.x+dx; pt3d[5].y = expt.y; pt3d[5].z = expt.z;
	pt3d[6].x = expt.x; pt3d[6].y = expt.y+dy; pt3d[6].z = expt.z;
	pt3d[7].x = expt.x; pt3d[7].y = expt.y-dy; pt3d[7].z = expt.z;
	
	//找到八个方向(四条线段)下获得地物的数目最多的一个方向
	int nfind1 = 0;
	for( int n=0; n<4; n++)
	{
		arrIds2.RemoveAll();
		int nfind2;
		{
			CFeature *pFtr0 = NULL;

			CShapeLine::ShapeLineUnit *shapeUnit = NULL;
			Envelope evlp,evlp2;
			PT_3D pt0,  pt1;
			PT_3D pts3d[2];
			pt0 = pts3d[0] = pt3d[n*2], pt1 = pts3d[1] = pt3d[n*2+1];
			evlp.CreateFromPts(pts3d,2,sizeof(PT_3D));
			for(int i=0;i<nObj;i++)
			{
				pFtr0 = ftr[i];
				if( !pFtr0 )continue;
				
				const CShapeLine *shape = pFtr0->GetGeometry()->GetShape();
			    if( !evlp.bIntersect(&shape->GetEnvelope()) )continue;

				shapeUnit = shape->HeadUnit();
				while( shapeUnit )
				{
					if (evlp.bIntersect(&shapeUnit->evlp))
					{				
						
						{
							PT_3DEX *pts = shapeUnit->pts;
							for( int k=0; k<shapeUnit->nuse-1; k++,pts++)
							{							
								COPY_3DPT(pts3d[0],pts[0]);
								COPY_3DPT(pts3d[1],pts[1]);
								evlp2.CreateFromPts(pts3d,2,sizeof(PT_3D));
								if( !evlp.bIntersect(&evlp2) )continue;
								
								if( (fabs(pt0.x-pts[0].x)<1e-4&&fabs(pt0.y-pts[0].y)<1e-4)||
									(fabs(pt1.x-pts[0].x)<1e-4&&fabs(pt1.y-pts[0].y)<1e-4)||
									(fabs(pt0.x-pts[1].x)<1e-4&&fabs(pt0.y-pts[1].y)<1e-4)||
									(fabs(pt1.x-pts[1].x)<1e-4&&fabs(pt1.y-pts[1].y)<1e-4) )
									continue;
								
								if( GraphAPI::GGetLineIntersectLineSeg(pt0.x,pt0.y,pt1.x,pt1.y,
									pts[0].x,pts[0].y,pts[1].x,pts[1].y,NULL,NULL,NULL) )break;
							}
							
							if (k < shapeUnit->nuse-1)
							{
								arrIds2.Add(FtrToHandle(pFtr0));
								break;
							}
						}
					}
					
					shapeUnit = shapeUnit->next;
				}
			}
		}
		nfind2 = arrIds2.GetSize();		
		
		if( nfind2<2 )continue;
		
		if( n==0 )
		{
			nfind1 = nfind2;
			arrIds1.Copy(arrIds2);
		}
		else if( nfind1<nfind2 )
		{
			nfind1 = nfind2;
			pt3d[0] = pt3d[n*2]; pt3d[1] = pt3d[n*2+1];
			arrIds1.Copy(arrIds2);
		}
	}
	
	Envelope evlp;
	evlp.CreateFromPts(pt3d,2);
	
	CArray<double,double> arrPos, arrZ;
	PT_3D ret;
	double t;
	for( int i=0; i<nfind1; i++)
	{
		CGeometry *po = HandleToFtr(arrIds1[i])->GetGeometry();
		if( !po )continue;
	
		const CShapeLine *pShape = ((CGeoCurve*)po)->GetShape();

		if (!pShape)   continue;

		if (evlp.bIntersect(&pShape->GetEnvelope()))
		{
			CArray<PT_3DEX,PT_3DEX> arrpts;
			pShape->GetPts(arrpts);
			PT_3DEX *pts = arrpts.GetData();
			for( int j=0; j<arrpts.GetSize()-1; j++,pts++)
			{
				if( CGrTrim::Intersect(pt3d,pt3d+1,&pts[0],&pts[1],&ret,&t) )
				{
					int nsz = arrPos.GetSize();
					for( int k=0; k<nsz; k++)
					{
						if( t<arrPos[k] )break;
					}
					if( k>=nsz )
					{
						arrPos.Add(t);
						arrZ.Add(pts[1].z);
					}
					else
					{
						arrPos.InsertAt(k,t);
						arrZ.InsertAt(k,pts[1].z);
					}
				}
			}
		}
		
	}
	
	int nsz = arrPos.GetSize();
	for( i=0; i<nsz; i++)
	{
		if( arrPos[i]>0.5 )break;
	}
	
	//等高线之间
	if( i>0 && i<nsz && fabs(arrZ[i-1]-arrZ[i])>1e-4 )
	{
		m_pChkResMgr->BeginResItem();
		m_pChkResMgr->AddAssociatedFtr(pFtr);
		m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
		if( expt.z>=max(arrZ[i-1],arrZ[i]) || expt.z<=min(arrZ[i-1],arrZ[i]) )
		{			
			m_pChkResMgr->SetReason((LPCTSTR)StrFromResID(IDS_PLERR_ZOUTSIDE));
		}
		else if( fabs(expt.z-arrZ[i-1])<=m_lfMinHeightDiff || fabs(expt.z-arrZ[i])<=m_lfMinHeightDiff )
		{
			m_pChkResMgr->SetReason((LPCTSTR)StrFromResID(IDS_PLERR_LESSDZ));
		}
		else if( m_lfContourInterval>0 && (fabs(expt.z-arrZ[i-1])>=m_lfContourInterval||fabs(expt.z-arrZ[i])>=m_lfContourInterval) )
		{
			m_pChkResMgr->SetReason((LPCTSTR)StrFromResID(IDS_PLERR_GREATERDZ));
		}
		m_pChkResMgr->EndResItem();
	}
	//山谷或者山顶
	else
	{
		if( i>=2 )
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
			if( (expt.z>=arrZ[i-2]&&expt.z<=arrZ[i-1])||(expt.z>=arrZ[i-1]&&expt.z<=arrZ[i-2]) )
			{
				m_pChkResMgr->SetReason((LPCTSTR)StrFromResID(IDS_PLERR_ZOUTSIDE));
			}
			else if( m_lfContourInterval>0 && fabs(expt.z-arrZ[i-1])>=m_lfContourInterval )
			{
				m_pChkResMgr->SetReason((LPCTSTR)StrFromResID(IDS_PLERR_GREATERDZ));
			}
			else if( fabs(expt.z-arrZ[i-1])<=m_lfMinHeightDiff )
			{
				m_pChkResMgr->SetReason((LPCTSTR)StrFromResID(IDS_PLERR_LESSDZ));
			}
			m_pChkResMgr->EndResItem();
		}
		else if( i<nsz-1 )
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
			if( (expt.z>=arrZ[i]&&expt.z<=arrZ[i+1])||(expt.z>=arrZ[i+1]&&expt.z<=arrZ[i]) )
			{
				m_pChkResMgr->SetReason((LPCTSTR)StrFromResID(IDS_PLERR_ZOUTSIDE));
			}
			else if( m_lfContourInterval>0 && fabs(expt.z-arrZ[i])>=m_lfContourInterval )
			{
				m_pChkResMgr->SetReason((LPCTSTR)StrFromResID(IDS_PLERR_GREATERDZ));
			}
			else if( fabs(expt.z-arrZ[i])<=m_lfMinHeightDiff )
			{
				m_pChkResMgr->SetReason((LPCTSTR)StrFromResID(IDS_PLERR_LESSDZ));
			}
			m_pChkResMgr->EndResItem();
		}
	}
}


void CPLChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_PLERR_ZOUTSIDE));
	reasons.Add(StrFromResID(IDS_PLERR_GREATERDZ));
	reasons.Add(StrFromResID(IDS_PLERR_LESSDZ));
}

BOOL CPLChkCmd::FilterFeature(LONG_PTR id)
{
	CFeature *pFtr = (CFeature*)id;
	CDlgDataSource  *pDS = m_pDoc->GetDlgDataSource();
	if(pDS) 
	{
		CFtrLayer* pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) return FALSE;	
		if(pLayer->IsLocked()) return FALSE;
		if ((!pLayer->IsVisible()||!pFtr->IsVisible())) return FALSE;
		CGeometry *po = pFtr->GetGeometry();
		if( !po )return FALSE;
		if( !po->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return FALSE;

		if (m_strcodeArray.GetSize()==0);
		else
		{
			for (int j=0;j<m_strcodeArray.GetSize();j++)
			{
				if (CheckObjForLayerCode(m_pDoc->GetDlgDataSource(),pFtr,m_strcodeArray[j]))
				{
					break;
				}
			}
			if(j>=m_strcodeArray.GetSize()) return FALSE;
		}

		if( !CContourValidChkCmd::CheckObjForZ(po) )return FALSE;
		
	}
	return TRUE;
}

void CPLChkCmd::Execute(BOOL bForSelection)
{
	BOOL bCheckCode = (m_strTestPtCode.GetLength()>0);
	if( !bCheckCode )
	{
		GOutPut(StrFromResID(IDS_CMDERR_CHECKCODE));
		return;
	}
	if(m_strContourCode.IsEmpty())
	{
		m_strcodeArray.RemoveAll();
	}
	else
	{
		m_strcodeArray.RemoveAll();
		if(!convertStringToStrArray(m_strContourCode,m_strcodeArray))
			return;
	}
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	CPtrArray arrLayers;
	CStringArray codeArray2;
	convertStringToStrArray(m_strTestPtCode,codeArray2);

	for( int i=0; i<codeArray2.GetSize(); i++)
	{
		CPtrArray arrLayers2;
		pDS->GetFtrLayer(codeArray2[i],NULL,&arrLayers2);

		arrLayers.Append(arrLayers2);
	}

	vector<CFeature*> container;
	int nTar = arrLayers.GetSize();	
	for (i=0;i<nTar;i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
				continue;
			container.push_back(pFtr);
		}
	}
	if( container.size()<=0 )
	{
		GOutPut(StrFromResID(IDS_CMDERR_NOOBJ));
		return;
	}

	MARK_SELECTION;

	CDlgDataQuery* pQuery = (CDlgDataQuery*)m_pDoc->GetDataQuery();
	CDlgDataQuery::FilterFeatureItem oldItem;
	CDlgDataQuery::FilterFeatureItem newItem;
	newItem.m_pFilter = (CSearcherFilter *)this;
	newItem.m_pFilterFunc = (PFilterFunc)&CPLChkCmd::FilterFeature;
	oldItem = pQuery->SetFilterFeature(newItem);
	GProgressStart(container.size());	
	vector<CFeature*>::iterator it;
	for( it=container.begin(); it!=container.end(); ++it)
	{	
		CFeature *pFtr = *it;
		
		SKIP_NOT_SELECTION;

		CheckObj(pFtr);		
		GProgressStep();
		
	}
	pQuery->SetFilterFeature(oldItem);
	
	GProgressEnd();	

	CLEAR_MARK_SELECTION;
}



//////////////////////////////////////////////////////////////////////////

CStringArray CLLChkCmd::m_strCodeArray;
CLLChkCmd::CLLChkCmd()
{
	m_strContourCode = _T("");
	m_strTestLineCode = _T("");
}

CLLChkCmd::~CLLChkCmd()
{

}

CString CLLChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_CONTOUR);
}

CString CLLChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_LL_ERR);
}

void CLLChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strContourCode;
	tab.AddValue(CHK_LL_CONTOUR_CODE,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strTestLineCode;
	tab.AddValue(CHK_LL_TESTLINE_CODE,&CVariantEx(var));
}

void CLLChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_LL_CONTOUR_CODE,var) )
	{
		m_strContourCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_LL_TESTLINE_CODE,var) )
	{
		m_strTestLineCode= (const char*)(_bstr_t)(_variant_t)*var;
	}
}

void CLLChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CLLChkCmd"),_T("CLLChkCmd"));

	param->AddLayerNameParamEx(CHK_LL_CONTOUR_CODE,(LPCTSTR)m_strContourCode,StrFromResID(IDS_CHKCMD_DES_CONTOUR),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParamEx(CHK_LL_TESTLINE_CODE,(LPCTSTR)m_strTestLineCode,StrFromResID(IDS_CHKCMD_DES_TESTLINE_CODE),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
}

BOOL CLLChkCmd::CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  Layercode)
{
	if (!pDS||!pFtr||strlen(Layercode)<=0)
	{
		return FALSE;
	}
	CFtrLayer *player = pDS->GetFtrLayerOfObject(pFtr);
	if(!player) return FALSE;
	const char *name = player->GetName();
	CString ret(Layercode);
	
	__int64 code = _atoi64(Layercode);
	if(code>0)
	{
		CConfigLibManager *pConfi = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
		CScheme *pScheme = pConfi->GetScheme(pDS->GetScale());
		if(!pScheme)return FALSE;
		pScheme->FindLayerIdx(TRUE,code,ret);
	}
	
	if( stricmp(ret,name)==0 )return TRUE;
	
	return FALSE;
	
}

namespace
{
	struct OBJ_ITEM
	{
		FTR_HANDLE id;
		int pos;
		PT_3D ret;
		double t;
	};
}

void CLLChkCmd::GetIntersectObjs(PT_3D pt0, PT_3D pt1, CArray<FTR_HANDLE, FTR_HANDLE>& arr)
{
	PT_3D ptLine[2]; 
	ptLine[0] = pt0; ptLine[1] = pt1;
	//粗略查找第一条线段的相交地物
	Envelope e1;
	e1.CreateFromPts(ptLine,2);
	m_pDoc->GetDataQuery()->FindObjectInRect(e1,NULL);
	
	int num1;
	const CPFeature *ids1 = m_pDoc->GetDataQuery()->GetFoundHandles(num1);
	
	
	//获得有效的对象与两线段的相交信息，并排序
	CArray<OBJ_ITEM,OBJ_ITEM> arr1;
	OBJ_ITEM item;
	CGeometry *pObj;
	for( int i=0; i<num1; i++)
	{
		CFeature *pFtr = ids1[i];
		if (!pFtr)  continue;
		pObj = pFtr->GetGeometry();
		if( !pObj )continue;
		
		//去除点状地物、文本、非等高地物、非母线特征码的地物
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;
		if( !CContourValidChkCmd::CheckObjForZ(pObj) )continue;
		if (m_strCodeArray.GetSize()==0);
		else
		{
			for (int j=0;j<m_strCodeArray.GetSize();j++)
			{
				if (CheckObjForLayerCode(m_pDoc->GetDlgDataSource(),pFtr,m_strCodeArray[j]))
				{
					break;
				}
			}
			if(j>=m_strCodeArray.GetSize())continue;
		}
	//	if( bCheckBase && !CPLChkCmd::CheckObjForLayerCode(m_pDoc->GetDlgDataSource(),pFtr,m_strContourCode) )continue;
		if( CAutoLinkContourCommand::CheckObjForLayerCode(m_pDoc->GetDlgDataSource(),pFtr,m_strTestLineCode) )continue;
		if( pObj->GetDataPointSum()<2 )continue;

		const CShapeLine *pShape = NULL;
		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
		{
			pShape = ((CGeoCurve*)pObj)->GetShape();
		}
		else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		{
			pShape = ((CGeoSurface*)pObj)->GetShape();
		}
		
		if (!pShape)  continue;
		
		int pos = 0;
		if (e1.bIntersect(&pShape->GetEnvelope()))
		{
			CArray<PT_3DEX,PT_3DEX> arrpts;
			pShape->GetPts(arrpts);
			PT_3DEX *pts = arrpts.GetData();
			for( int j=0; j<arrpts.GetSize()-1; j++,pts++)
			{
				if( e1.bIntersect(pts,&pts[1]) )
				{
					//求相交点
					PT_3D ret;
					double t=-1;
					if( !CGrTrim::Intersect(ptLine,ptLine+1,pts,&pts[1],&ret,&t) )
						continue;
					
					ret.z = pts->z;
					
					item.id = FTR_HANDLE(ids1[i]); item.pos= pos+j; item.ret= ret; item.t = t;
					
					//按照 t 大小排序插入
					int size1 = arr1.GetSize();
					for( int k=0; k<size1 && item.t>=arr1[k].t; k++);
					if( k<size1 )arr1.InsertAt(k,item);
					else arr1.Add(item);
					
					goto FINDOVER_1;
				}
			}
		}
FINDOVER_1:;
	}
	
	int size1 = arr1.GetSize();
	for( i=0; i<size1; i++)
	{
		FTR_HANDLE id = arr1[i].id;
		arr.Add(id);
	}
}

void CLLChkCmd::CheckObj(CFeature *pFtr)
{
	if (!pFtr) return;
	CGeometry *pObj = pFtr->GetGeometry();
	if (!pObj) return;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pObj->GetShape(arrPts);
	int i,j,ptnum = arrPts.GetSize();
	if( ptnum<2 )return;
	
	PT_3D ptLine[2],pt;
	PT_3DEX expt1, expt2, expt3, expt4, expt5;
	CArray<FTR_HANDLE,FTR_HANDLE> arrIds;
	FTR_HANDLE id1=0, id2=0;
	CGeometry *pObj1, *pObj2;
	int checkpos = -1; 
	for( i=0; i<=ptnum; i++) 
	{
		if( i==0 )
		{
			expt2 = arrPts.GetAt(i);
			continue;
		}
		else if( i==ptnum )
		{
			continue;
		}
		else
		{
			expt1 = expt2;
			expt2 = arrPts.GetAt(i);
		}
		
		COPY_3DPT(ptLine[0],expt1);
		COPY_3DPT(ptLine[1],expt2);
		
		arrIds.RemoveAll();
		GetIntersectObjs(ptLine[0],ptLine[1],arrIds);
		if( arrIds.GetSize()>0 )
		{
			id1 = id2;
			id2 = arrIds[0];
			
			if( id1 && id2 )
			{
				pObj1 = HandleToFtr(id1)->GetGeometry();
				pObj2 = HandleToFtr(id2)->GetGeometry();
				if( pObj1 && pObj2 && pObj1!=pObj2 )
				{
					expt3 = pObj1->GetDataPoint(0);
					expt4 = pObj2->GetDataPoint(0);
					
					if( checkpos>=0 )
					{
						//从上次检查结束的位置开始检查
						for( j=checkpos; j<i; j++)
						{
							expt5 = arrPts.GetAt(j);
							if( expt5.z<(expt3.z<expt4.z?expt3.z:expt4.z) || 
								expt5.z>(expt3.z>expt4.z?expt3.z:expt4.z) )
							{
								m_pChkResMgr->BeginResItem();
								m_pChkResMgr->AddAssociatedFtr(pFtr);
								m_pChkResMgr->SetAssociatedPos(expt5);
								m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_LL_ERR));
								m_pChkResMgr->EndResItem();
							}
						}
					}
				}
			}
			
			checkpos = i;
			id2 = arrIds[arrIds.GetSize()-1];
		}
	}
}



void CLLChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_LL_ERR));
}


void CLLChkCmd::Execute(BOOL bForSelection)
{
	BOOL bCheckCode = (m_strTestLineCode.GetLength()>0);
	if( !bCheckCode )
	{
		GOutPut(StrFromResID(IDS_CMDERR_CHECKCODE));
		return;
	}
	if(m_strContourCode.IsEmpty())
	{
		m_strCodeArray.RemoveAll();
	}
	else
	{
		m_strCodeArray.RemoveAll();
		if(!convertStringToStrArray(m_strContourCode,m_strCodeArray))
			return;
	}

	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	CPtrArray arrLayers;
	if(!pDS->GetFtrLayer(m_strTestLineCode,NULL,&arrLayers))
		return;
	vector<CFeature*> container;
	int nTar = arrLayers.GetSize();	
	for (int i=0;i<nTar;i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				continue;
			}
			container.push_back(pFtr);
		}
	}
	if( container.size()<=0 )
	{
		GOutPut(StrFromResID(IDS_CMDERR_NOOBJ));
		return;
	}

	MARK_SELECTION;

	GProgressStart(container.size());	
	vector<CFeature*>::iterator it;
	for( it=container.begin(); it!=container.end(); ++it)
	{	
		CFeature *pFtr = *it;	

		SKIP_NOT_SELECTION;

		CheckObj(pFtr);		
		GProgressStep();
		
	}	
	GProgressEnd();	

	CLEAR_MARK_SELECTION;
}



//////////////////////////////////////////////////////////////////////////

CPPChkCmd::CPPChkCmd()
{
	m_lfDistance = 0;
	m_strTestPtCode = _T("");
}

CPPChkCmd::~CPPChkCmd()
{

}

CString CPPChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_CONTOUR);
}

CString CPPChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_PP_ERR);
}

void CPPChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strTestPtCode;
	tab.AddValue("testpt_code",&CVariantEx(var));
	var = m_lfDistance;
	tab.AddValue("distance",&CVariantEx(var));
}

void CPPChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"testpt_code",var) )
	{
		m_strTestPtCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"distance",var) )
	{
		m_lfDistance = (_variant_t)*var;
	}
}

void CPPChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CPPChkCmd"),_T("CPPChkCmd"));

	param->AddLayerNameParamEx("testpt_code",(LPCTSTR)m_strTestPtCode,StrFromResID(IDS_CHKCMD_DES_TESTPT_CODE),NULL,LAYERPARAMITEM_NOTEMPTY_POINT);
	param->AddParam("distance",m_lfDistance,StrFromResID(IDS_CHK_PP_DISTANCE));
}

BOOL CPPChkCmd::CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  fid)
{
	if( !fid )return FALSE;
	CString text = fid;
	text.Remove(' ');
	int startpos = 0, findpos = 0;
	while( findpos>=0 )
	{
		findpos = text.Find(',',startpos);
		
		CString sub;
		if( findpos>startpos )
			sub = text.Mid(startpos,(findpos-startpos));
		else
			sub = text.Mid(startpos);
		
		if( CAutoLinkContourCommand::CheckObjForLayerCode(pDS,pFtr,sub) )
		{
			return TRUE;
		}
		
		startpos = findpos+1;
	}
	return TRUE;
}


void CPPChkCmd::CheckObj(CFeature *pFtr)
{
	PT_3DEX expt;
	Envelope e;
	CArray<FTR_HANDLE,FTR_HANDLE> arrIds1, arrIds2;
	
	CGeometry *pObj = pFtr->GetGeometry();
	expt = pObj->GetDataPoint(0);

	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme)  return;
	
	CFtrLayer *pLayer1 = pDS->GetFtrLayerOfObject(pFtr);

	CSchemeLayerDefine *pSLayer = pScheme->GetLayerDefine(pLayer1->GetName());
	
	if (!pSLayer) return;
	
	CAnnotation *pAnno = pSLayer->GetHeightAnnotation();
	if( !pAnno )return;

	//double r = m_lfDistance*pDS->GetScale()*0.001;
	double r = m_lfDistance;

	e.CreateFromPtAndRadius(expt,r);

	CDataQuery *pDQ = m_pDoc->GetDataQuery();
	int num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);

	const CPFeature* buf = pDQ->GetFoundHandles(num);

	CValueTable tab;
	CPtrArray geo1, geo2;
	pAnno->ExtractGeoText(pFtr,geo1,tab,pDS->GetSymbolDrawScale());
	if( geo1.GetSize()<=0 )
		return;

	if( geo1.GetSize()==1 )
	{
		m_pChkResMgr->BeginResItem();
		int bAddSelf = FALSE;

		for( int i=0; i<num; i++)
		{	
			if( pFtr==buf[i] )
				continue;

			CFtrLayer *pLayer2 = pDS->GetFtrLayerOfObject(buf[i]);
			if( stricmp(pLayer1->GetName(),pLayer2->GetName())!=0 )
				continue;

			PT_3DEX expt1 = buf[i]->GetGeometry()->GetDataPoint(0);
			if( GraphAPI::GGet2DDisOf2P(expt,expt1)>r )
				continue;
			
			pAnno->ExtractGeoText(buf[i],geo2,tab,pDS->GetSymbolDrawScale());
			if( geo2.GetSize()<=0 )
				continue;
			
			if( geo2.GetSize()==1 )
			{
				CGeoText *p1 = (CGeoText*)geo1[0];
				CGeoText *p2 = (CGeoText*)geo2[0];

				if( p1->GetText().CompareNoCase(p2->GetText())==0 )
				{
					if( !bAddSelf )m_pChkResMgr->AddAssociatedFtr(pFtr);
					bAddSelf = TRUE;
					m_pChkResMgr->AddAssociatedFtr(buf[i]);
					m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_PP_ERR));
					
				}
			}

			for( int j=0; j<geo2.GetSize(); j++)
			{
				delete geo2[j];
			}

			geo2.RemoveAll();
		}
		m_pChkResMgr->SetAssociatedPos(expt);		
		m_pChkResMgr->EndResItem();
	}

	for( int j=0; j<geo1.GetSize(); j++)
	{
		delete geo1[j];
	}
}



void CPPChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_CMD_PP_ERR));
}


void CPPChkCmd::Execute(BOOL bForSelection)
{
	BOOL bCheckCode = (m_strTestPtCode.GetLength()>0);

	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	/*
	CPtrArray arrLayers;
//	GetFtrLayers(pDS,m_strTestPtCode,arrLayers);
	if(!pDS->GetFtrLayer(m_strTestPtCode, NULL, &arrLayers))
		return;

	vector<CFeature*> container;
	int nTar = arrLayers.GetSize();	
	for (int i=0;i<nTar;i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
			{
				continue;
			}
			container.push_back(pFtr);
		}
	}
	*/

	vector<CFeature*> container;
	CStringArray arr;
	if(!convertStringToStrArray(m_strTestPtCode,arr))
		return ;
	for(int a=0;a<arr.GetSize();a++)
	{
		CPtrArray arrLayers;
		if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
			continue;
		int nTar = arrLayers.GetSize();	
		for (int i=0;i<nTar;i++)
		{
			CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
				{
					continue;
				}
				container.push_back(pFtr);
			}
		}
	}

	if( container.size()<=0 )
	{
		GOutPut(StrFromResID(IDS_CMDERR_NOOBJ));
		return;
	}

	MARK_SELECTION;

	CDlgDataQuery* pQuery = (CDlgDataQuery*)m_pDoc->GetDataQuery();
	CDlgDataQuery::FilterFeatureItem oldItem;
	CDlgDataQuery::FilterFeatureItem newItem;
	newItem.m_pFilter = (CSearcherFilter *)this;
	newItem.m_pFilterFunc = (PFilterFunc)&CPPChkCmd::FilterFeature;
	oldItem = pQuery->SetFilterFeature(newItem);

	m_pChkResMgr->Clear();

	GProgressStart(container.size());	
	vector<CFeature*>::iterator it;
	for( it=container.begin(); it!=container.end(); ++it)
	{	
		CFeature *pFtr = *it;	

		SKIP_NOT_SELECTION;

		CheckObj(pFtr);		
		GProgressStep();
		
	}	
	GProgressEnd();	

	pQuery->SetFilterFeature(oldItem);

	CLEAR_MARK_SELECTION;
}



BOOL CPPChkCmd::FilterFeature(LONG_PTR id)
{
	CFeature *pFtr = (CFeature*)id;
	CDlgDataSource  *pDS = m_pDoc->GetDlgDataSource();
	if(pDS) 
	{
		CFtrLayer* pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) return FALSE;	
		if(pLayer->IsLocked()) return FALSE;
		if ((!pLayer->IsVisible()||!pFtr->IsVisible())) return FALSE;
		CGeometry *po = pFtr->GetGeometry();
		if( !po )return FALSE;
		if( !po->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )return FALSE;		
		
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
CRiverZChkCmd::CRiverZChkCmd()
{
	m_strLayerCodes = _T("");
	m_bRiverZIncValid = FALSE;
	m_bChkMonotony = FALSE;
	m_lfTolerZ = GraphAPI::g_lfZTolerance;
}

CRiverZChkCmd::~CRiverZChkCmd()
{

}

CString CRiverZChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CRiverZChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_RIVERZ_VALID);
}

void CRiverZChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;	
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_RIVERZ_VALID_CODE,&CVariantEx(var));
	var = (bool)m_bRiverZIncValid;
	tab.AddValue(CHK_RIVERZ_DESC,&CVariantEx(var));
	var = (bool)m_bChkMonotony;
	tab.AddValue("ChkMonotony",&CVariantEx(var));
	var = (double)m_lfTolerZ;
	tab.AddValue("TolerZ",&CVariantEx(var));
}

void CRiverZChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;	
	if( tab.GetValue(0,CHK_RIVERZ_VALID_CODE,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_RIVERZ_DESC,var) )
	{
		m_bRiverZIncValid = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,"ChkMonotony",var) )
	{
		m_bChkMonotony = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,"TolerZ",var) )
	{
		m_lfTolerZ = (double)(_variant_t)*var;
	}
}

void CRiverZChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CRiverZChkCmd"),_T("CRiverZChkCmd"));
	param->AddLayerNameParamEx(CHK_RIVERZ_VALID_CODE,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
	param->AddParam("ChkMonotony",(bool)m_bChkMonotony,StrFromResID(IDS_CHKCMD_RIVERZ_MONOYONY));
	if(m_bChkMonotony || bForLoad)
	{
		param->AddParam("TolerZ",m_lfTolerZ,StrFromResID(IDS_CMDPLANE_MAXOFFSET));
	}
	param->BeginOptionParam(CHK_RIVERZ_DESC,StrFromResID(IDS_CHKCMD_DES_RIVERZ));
	param->AddOption(StrFromResID(IDS_CHK_Z_INC),1,' ',m_bRiverZIncValid?TRUE:FALSE);
	param->AddOption(StrFromResID(IDS_CHK_Z_DEC),0,' ',m_bRiverZIncValid?FALSE:TRUE);
	param->EndOptionParam();
}

void CRiverZChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	vector<CFeature*> container;
	//把选定层中的面地物选出来
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				
				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				{				
					container.push_back(pFtr);
				}						
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					
					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
					{						
						container.push_back(pFtr);
					}
				}
			}
		}			
	}

	MARK_SELECTION;

	vector<CFeature*>::const_iterator it;
	GProgressStart(container.size());
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		CFeature *pFtr = (*it);

		SKIP_NOT_SELECTION;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		pFtr->GetGeometry()->GetShape(arrPts);
		if (m_bChkMonotony)
		{
			int fag = 0;//0平衡，1增，-1减
			for (int i=0;i<arrPts.GetSize()-1;i++)
			{
				//if ((arrPts[i].z<arrPts[i+1].z&&fag==-1)||(arrPts[i].z>arrPts[i+1].z&&fag==1))
				if( ((arrPts[i+1].z-arrPts[i].z)>m_lfTolerZ &&fag==-1) ||
					((arrPts[i].z-arrPts[i+1].z)>m_lfTolerZ &&fag==1) )
				{
					break;
				}
				else
				{
					//if (arrPts[i].z<arrPts[i+1].z)
					if ((arrPts[i+1].z-arrPts[i].z)>m_lfTolerZ)
					{
						fag = 1;
					}
					//else if (arrPts[i].z>arrPts[i+1].z)
					else if ((arrPts[i].z-arrPts[i+1].z)>m_lfTolerZ)
					{
						fag = -1;
					}
				}
			
			}
			if (i<arrPts.GetSize()-1)
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_Z_MONO_NOT_SAME));
				m_pChkResMgr->EndResItem();
				continue;
			}
		}

		int nPt = arrPts.GetSize();
		if(nPt<2) continue;
		if( m_bRiverZIncValid && (arrPts[0].z-arrPts[nPt-1].z)>GraphAPI::g_lfZTolerance )
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_Z_DEC));
			m_pChkResMgr->EndResItem();
		}
		if( !m_bRiverZIncValid && (arrPts[nPt-1].z-arrPts[0].z)>GraphAPI::g_lfZTolerance )
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_Z_INC));
			m_pChkResMgr->EndResItem();
		}
	}
	GProgressEnd();
	
	CLEAR_MARK_SELECTION;
}


void CRiverZChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_Z_MONO_NOT_SAME));
	reasons.Add(StrFromResID(IDS_CHK_Z_INC));
}


//////////////////////////////////////////////////////////////////////////
CClockDirChkCmd::CClockDirChkCmd()
{
	m_strLayerCodes = _T("");
	m_bClockwise = TRUE;
}

CClockDirChkCmd::~CClockDirChkCmd()
{

}

CString CClockDirChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CClockDirChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_CLOCKWISE);
}

void CClockDirChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;	
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_CLOCK_DIR_CODE,&CVariantEx(var));
	var = (bool)m_bClockwise;
	tab.AddValue(CHK_CLOCKWISE,&CVariantEx(var));
}

void CClockDirChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;	
	if( tab.GetValue(0,CHK_CLOCK_DIR_CODE,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_CLOCKWISE,var) )
	{
		m_bClockwise = (bool)(_variant_t)*var;
	}

}

void CClockDirChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CClockDirChkCmd"),_T("CClockDirChkCmd"));
	param->AddLayerNameParamEx(CHK_CLOCK_DIR_CODE,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
//	param->AddParam(CHK_CLOCKWISE,(bool)m_bClockwise,StrFromResID(IDS_CHK_CMD_CLOCKWISE));
	param->BeginOptionParam(CHK_CLOCKWISE,StrFromResID(IDS_CHK_CMD_DES_CLOCKDIR));
	param->AddOption(StrFromResID(IDS_CHK_DES_CLOCKWISE),TRUE,' ',m_bClockwise);
	param->AddOption(StrFromResID(IDS_CHK_CMD_DES_ANTICLOCKWISE),FALSE,' ',!m_bClockwise);
	param->EndOptionParam();
}

void CClockDirChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	vector<CFeature*> container;
	//把选定层中的面地物选出来
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				
				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{				
					container.push_back(pFtr);
				}						
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					
					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					{						
						container.push_back(pFtr);
					}
				}
			}
		}			
	}

	MARK_SELECTION;

	GProgressStart(container.size());
	vector<CFeature*>::const_iterator it;
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		CFeature *pFtr = (*it);

		SKIP_NOT_SELECTION;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		pFtr->GetGeometry()->GetShape(arrPts);
		int nPts = GraphAPI::GKickoffSame2DPoints(arrPts.GetData(),arrPts.GetSize());
		arrPts.SetSize(nPts);
		if(nPts<3)continue;
		if (m_bClockwise)
		{
			if(1==GraphAPI::GIsClockwise(arrPts.GetData(),nPts))
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_DES_CLOCKWISE));
				m_pChkResMgr->EndResItem();
			}			
		}
		else
		{
			if(0==GraphAPI::GIsClockwise(arrPts.GetData(),nPts))
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_DES_ANTICLOCKWISE));
				m_pChkResMgr->EndResItem();
			}
		}
	}	
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}



void CClockDirChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_DES_CLOCKWISE));
	reasons.Add(StrFromResID(IDS_CHK_CMD_DES_ANTICLOCKWISE));
}


//////////////////////////////////////////////////////////////////////////
CLengthChkCmd::CLengthChkCmd()
{
	m_strLayerCodes = _T("");
	m_lfMinlen = 0.0;
	m_lfMaxLen = 0.0;
}

CLengthChkCmd::~CLengthChkCmd()
{

}

CString CLengthChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_GEOMETRIC_ATTRI);
}

CString CLengthChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_LEN);
}

BOOL CLengthChkCmd::CheckParamsValid(CString &reason)
{
// 	double lfMinlen = m_lfMinlen>0.0?m_lfMinlen:0.0;
// 	double lfMaxlen = m_lfMaxLen>=0.0?m_lfMaxLen:DBL_MAX;
	if(m_lfMinlen > m_lfMaxLen || m_lfMinlen < 0 || m_lfMaxLen < 0)
	{
		reason = StrFromResID(IDS_PARAM_ERROR);
		return FALSE;
	}
	return TRUE;
}

void CLengthChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;		
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_LEN_CODE,&CVariantEx(var));
	var = (double)m_lfMinlen;
	tab.AddValue(CHK_LEN_MIN,&CVariantEx(var));
	var = (double)m_lfMaxLen;
	tab.AddValue(CHK_LEN_MAX,&CVariantEx(var));
}

void CLengthChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_LEN_CODE,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_LEN_MIN,var) )
	{
		m_lfMinlen= (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_LEN_MAX,var) )
	{
		m_lfMaxLen= (double)(_variant_t)*var;
	}
}

void CLengthChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CLengthChkCmd"),_T("CLengthChkCmd"));
	
	param->AddLayerNameParamEx(CHK_LEN_CODE,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
	param->AddParam(CHK_LEN_MAX,m_lfMaxLen,StrFromResID(IDS_CHKCMD_DES_MAXLEN));
	param->AddParam(CHK_LEN_MIN,m_lfMinlen,StrFromResID(IDS_CHKCMD_DES_MINLEN));

	
// 	param->AddLayerNameParam(CMDPLANE_CONTOURCODE,(LPCTSTR)m_strBaseFCode,StrFromResID(IDS_CMDPLANE_CONTOURCODE));
// 	param->AddLayerNameParam(CMDPLANE_CHECKCODE,(LPCTSTR)m_strCheckFCode, StrFromResID(IDS_CMDPLANE_CHECKCODE));
// 	
}

void CLengthChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	double lfMinlen = m_lfMinlen>0.0?m_lfMinlen:0.0;
	double lfMaxlen = m_lfMaxLen>=0.0?m_lfMaxLen:DBL_MAX;
	if(lfMinlen>=lfMaxlen)
		return;
	vector<CFeature*> container;
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				
				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{				
					container.push_back(pFtr);
				}						
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					
					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					{						
						container.push_back(pFtr);
					}
				}
			}
		}			
	}

	MARK_SELECTION;

	vector<CFeature*>::const_iterator it;
	GProgressStart(container.size());
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		CFeature *pFtr = (*it);

		SKIP_NOT_SELECTION;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		const CShapeLine *shape = pFtr->GetGeometry()->GetShape();
		shape->GetPts(arrPts);
// 		if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
// 		{
// 			arrPts.Add(arrPts[0]);
// 		}
		double len = GraphAPI::GGetAllLen2D(arrPts.GetData(),arrPts.GetSize());
		if(len<lfMaxlen&&len>lfMinlen)
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_LEN_IN_RANGE));
			m_pChkResMgr->EndResItem();
		}	
	}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}


void CLengthChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_LEN_IN_RANGE));
}


//////////////////////////////////////////////////////////////////////////
CAreaChkCmd::CAreaChkCmd()
{
	m_strLayerCodes = _T("");
	m_lfMinArea = 0.0;
	m_lfMaxArea = 0.0;
}

CAreaChkCmd::~CAreaChkCmd()
{

}

CString CAreaChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_GEOMETRIC_ATTRI);
}

CString CAreaChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_AREA);
}

BOOL CAreaChkCmd::CheckParamsValid(CString &reason)
{
// 	double lfMinArea = m_lfMinArea>0.0?m_lfMinArea:0.0;
// 	double lfMaxArea = m_lfMaxArea>=0.0?m_lfMaxArea:DBL_MAX;
	if(m_lfMinArea > m_lfMaxArea || m_lfMaxArea < 0 || m_lfMinArea < 0)
	{
		reason = StrFromResID(IDS_PARAM_ERROR);
		return FALSE;
	}
	return TRUE;
}

void CAreaChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;		
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_AREA_CODE,&CVariantEx(var));
	var = (double)m_lfMinArea;
	tab.AddValue(CHK_AREA_MIN,&CVariantEx(var));
	var = (double)m_lfMaxArea;
	tab.AddValue(CHK_AREA_MAX,&CVariantEx(var));
}

void CAreaChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_AREA_CODE,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_AREA_MIN,var) )
	{
		m_lfMinArea= (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_AREA_MAX,var) )
	{
		m_lfMaxArea= (double)(_variant_t)*var;
	}
}

void CAreaChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CAreaChkCmd"),_T("CAreaChkCmd"));
	
	param->AddLayerNameParamEx(CHK_AREA_CODE,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
	param->AddParam(CHK_AREA_MAX,m_lfMaxArea,StrFromResID(IDS_CHKCMD_DES_MAXAREA));
	param->AddParam(CHK_AREA_MIN,m_lfMinArea,StrFromResID(IDS_CHKCMD_DES_MINAREA));
	
	
// 	param->AddLayerNameParam(CMDPLANE_CONTOURCODE,(LPCTSTR)m_strBaseFCode,StrFromResID(IDS_CMDPLANE_CONTOURCODE));
// 	param->AddLayerNameParam(CMDPLANE_CHECKCODE,(LPCTSTR)m_strCheckFCode, StrFromResID(IDS_CMDPLANE_CHECKCODE));
// 	
}

void CAreaChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	double lfMinArea = m_lfMinArea>0.0?m_lfMinArea:0.0;
	double lfMaxArea = m_lfMaxArea>=0.0?m_lfMaxArea:DBL_MAX;
	if(lfMinArea>=lfMaxArea)
		return;
	vector<CFeature*> container;
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				
				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{				
					container.push_back(pFtr);
				}						
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					
					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					{						
						container.push_back(pFtr);
					}
				}
			}
		}			
	}

	MARK_SELECTION;

	vector<CFeature*>::const_iterator it;
	GProgressStart(container.size());
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		CFeature *pFtr = (*it);

		SKIP_NOT_SELECTION;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		const CShapeLine *shape = pFtr->GetGeometry()->GetShape();
		shape->GetPts(arrPts);		
		double area = GraphAPI::GGetPolygonArea(arrPts.GetData(),arrPts.GetSize());
		if(area<lfMaxArea&&area>lfMinArea)
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_AREA_IN_RANGE));
			m_pChkResMgr->EndResItem();
		}	
	}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}


void CAreaChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_AREA_IN_RANGE));
}


//////////////////////////////////////////////////////////////////////////
CZChkCmd::CZChkCmd()
{
	m_strLayerCodes = _T("");
	m_lfMinZ = 0.0;
	m_lfMaxZ = 0.0;
	m_bOnlyContour = TRUE;
}

CZChkCmd::~CZChkCmd()
{

}

CString CZChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_GEOMETRIC_ATTRI);
}

CString CZChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_Z);
}


BOOL CZChkCmd::CheckParamsValid(CString &reason)
{
// 	double lfMinZ = m_lfMinZ>0.0?m_lfMinZ:0.0;
// 	double lfMaxZ = m_lfMaxZ>=0.0?m_lfMaxZ:DBL_MAX;
	if(m_lfMinZ > m_lfMaxZ)
	{
		reason = StrFromResID(IDS_PARAM_ERROR);
		return FALSE;
	}
	return TRUE;
}

void CZChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;		
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_Z_CODE,&CVariantEx(var));
	var = (double)m_lfMinZ;
	tab.AddValue(CHK_Z_MIN,&CVariantEx(var));
	var = (double)m_lfMaxZ;
	tab.AddValue(CHK_Z_MAX,&CVariantEx(var));
	var = (bool)m_bOnlyContour;
	tab.AddValue("OnlyContour",&CVariantEx(var));
}

void CZChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_Z_CODE,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_Z_MIN,var) )
	{
		m_lfMinZ = (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_Z_MAX,var) )
	{
		m_lfMaxZ = (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,"OnlyContour",var) )
	{
		m_bOnlyContour = (bool)(_variant_t)*var;
	}
}

void CZChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CZChkCmd"),_T("CZChkCmd"));
	
	param->AddLayerNameParamEx(CHK_Z_CODE,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY);
	param->AddParam(CHK_Z_MAX,m_lfMaxZ,StrFromResID(IDS_CHKCMD_DES_MAXZ));
	param->AddParam(CHK_Z_MIN,m_lfMinZ,StrFromResID(IDS_CHKCMD_DES_MINZ));

	param->BeginOptionParam("OnlyContour",StrFromResID(IDS_CMDPLANE_ONLYCONTOUR));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bOnlyContour);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bOnlyContour);
	param->EndOptionParam();
	
// 	param->AddLayerNameParam(CMDPLANE_CONTOURCODE,(LPCTSTR)m_strBaseFCode,StrFromResID(IDS_CMDPLANE_CONTOURCODE));
// 	param->AddLayerNameParam(CMDPLANE_CHECKCODE,(LPCTSTR)m_strCheckFCode, StrFromResID(IDS_CMDPLANE_CHECKCODE));
// 	
}

void CZChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
// 	double lfMinZ = m_lfMinZ>0.0?m_lfMinZ:0.0;
// 	double lfMaxZ = m_lfMaxZ>0.0?m_lfMaxZ:DBL_MAX;
	if(m_lfMinZ>=m_lfMaxZ)
		return;
	vector<CFeature*> container;
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				
			//	if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{				
					container.push_back(pFtr);
				}						
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					
				//	if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					{						
						container.push_back(pFtr);
					}
				}
			}
		}			
	}

	MARK_SELECTION;

	GProgressStart(container.size());
	vector<CFeature*>::const_iterator it;
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		CFeature *pFtr = (*it);

		SKIP_NOT_SELECTION;

		if(m_bOnlyContour)
		{
			Envelope e;
/*			if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
			{
				e.m_zh = e.m_zl = pFtr->GetGeometry()->GetDataPoint(0).z;
			}
			else
				continue;*/
			double z = 0;
			if( !GetZValue(pFtr, &z) ) continue;
			e.m_zh = e.m_zl = z;

			if(e.m_zh<m_lfMinZ||e.m_zl>m_lfMaxZ)
			{
				;
			}
			else
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_Z_IN_RANGE));
				m_pChkResMgr->EndResItem();
			}
		}
		else
		{
			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo) continue;
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			for(int i=0; i<arrPts.GetSize(); i++)
			{
				if(arrPts[i].z<m_lfMinZ || arrPts[i].z>m_lfMaxZ)
				{
					;
				}
				else
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->SetAssociatedPos(arrPts[i]);
					m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_Z_IN_RANGE));
					m_pChkResMgr->EndResItem();
					//break;
				}
			}
		}
	}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}


void CZChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_Z_IN_RANGE));
}

BOOL CZChkCmd::GetZValue(CFeature* pFtr, double* pZValue)
{
	if(!pFtr || !pZValue) return FALSE;
	CArray<PT_3DEX,PT_3DEX> pts;
	CGeometry* pGeo = pFtr->GetGeometry();
	if(!pGeo) return FALSE;
	pGeo->GetShape( pts );
	int npt = pts.GetSize();
	if( npt <= 0 ) return FALSE;

	if( 1 == npt )
	{
		PT_3DEX pt = pts.GetAt(0);
		*pZValue = pt.z;
		return TRUE;
	}

	PT_3DEX pt0 = pts.GetAt(0);
	for(int i = 1; i < npt; i++)
	{
		PT_3DEX pt1 = pts.GetAt(i);
		if( fabs(pt1.z-pt0.z) >= GraphAPI::g_lfZTolerance )
		{
			return FALSE;
		}
	}
	
	*pZValue = pt0.z;
	return TRUE;
}

//hcw,2013.1.24,层与对象的一致性检查
CGeoTypeConsistencyChkCmd::CGeoTypeConsistencyChkCmd()
{
	m_strLayerCodes = _T("");
}
CGeoTypeConsistencyChkCmd::~CGeoTypeConsistencyChkCmd()
{
	
}
CString CGeoTypeConsistencyChkCmd::GetCheckCategory() const
{
    return StrFromResID(IDS_CHK_CATEGORY_BASIC);
}

CString CGeoTypeConsistencyChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_CMD_GEOTYPE_CONSISTENCY);
}

void CGeoTypeConsistencyChkCmd::GetParams( CValueTable& tab ) const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_GEOTYPE_CONSISTENCY_CODES,&CVariantEx(var));
	return;
}

void CGeoTypeConsistencyChkCmd::SetParams( CValueTable& tab )
{
	const CVariantEx *var;
	if (tab.GetValue(0, CHK_GEOTYPE_CONSISTENCY_CODES, var))
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
}

void CGeoTypeConsistencyChkCmd::FillShowParams( CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(_T("CGeoTypeConsistencyChkCmd"),_T("CGeoTypeConsistencyChkCmd"));
	param->AddLayerNameParamEx(CHK_GEOTYPE_CONSISTENCY_CODES,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY);
}

void CGeoTypeConsistencyChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if (!pDS) return;
	vector<CFeature*> container;
	//将参与层中的地物选出来,若参与层为空,则将所有层中的地物都挑出来。
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;			
				container.push_back(pFtr);					
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;					
						container.push_back(pFtr);
				}
			}
		}			
	}
	
	MARK_SELECTION;
	
	GProgressStart(container.size());
	vector<CFeature*>::const_iterator it;
	for (it= container.begin();it<container.end();it++)
	{
		GProgressStep();
		//CFeature *pFtr = (*it).pFtr;
		CFeature *pFtr = (*it);
		SKIP_NOT_SELECTION;
		//pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint))
		CString strObjType = "";
 		if (!CheckGeoTypeConsistency(pFtr,strObjType))
 		{
			//记录到检查结果
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
			CString strReason = StrFromResID(IDS_CHK_CMD_GEOTYPE_NOT_CONSISTENT);
			int nPos= strReason.Find("不一致");
			strReason.Insert(nPos,strObjType);
			m_pChkResMgr->SetReason(strReason);
  			m_pChkResMgr->EndResItem();
		}
		
		
	}
	GProgressEnd();
	
	CLEAR_MARK_SELECTION;
	return;
}

void CGeoTypeConsistencyChkCmd::GetAllReasons( CStringArray& reasons )
{	
	reasons.Add(StrFromResID(IDS_CHK_CMD_GEOTYPE_NOT_CONSISTENT));
}

BOOL CGeoTypeConsistencyChkCmd::CheckGeoTypeConsistency( CFeature* pFtr, CString& strObjType)
{
	if (!pFtr)
	{
		return TRUE;
	}
	
	//获得层名和层码
	DocId doc =fdb_getCurDoc();
	FtrId ftrID = fdb_FtrIDFromFtr(pFtr);
	LayId lay =	fdb_getLayerOfFtr(doc,ftrID); 
	int nLayCode = fdb_getCodeOfLayer(doc,lay);
	CString strLayerName = fdb_getNameOfLayer(lay);
	long nScale = fdb_getScale(doc);
	long nSuitedScale = gpCfgLibMan->GetSuitedScale(nScale);
	ConfigLibItem configLibItem = gpCfgLibMan->GetConfigLibItemByScale(nSuitedScale);
	CSchemeLayerDefine *pSchemeLayerDefine=NULL;
	pSchemeLayerDefine = configLibItem.pScheme->GetLayerDefine("",TRUE,nLayCode);
	if (!pSchemeLayerDefine)
	{
		return TRUE;
	}
	int nGeoType = configLibItem.pScheme->GetLayerDefine("",TRUE,nLayCode)->GetGeoClass();
	
	//获取当前对象的属性
	if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
	{
		//pFtr->GetGeometry()->GetLa
		if (nGeoType==CLS_GEOPOINT||nGeoType==CLS_GEODIRPOINT
			||nGeoType==CLS_GEOMULTIPOINT||nGeoType==CLS_GEODEMPOINT)
		{
		     return TRUE;
		}
		strObjType = _T("(点)");
	}
	else if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		if (nGeoType==CLS_GEOSURFACE||nGeoType==CLS_GEOMULTISURFACE)
		{
			return TRUE;
		}
		strObjType = _T("(面)");
	}
	else if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
	{
		if(nGeoType==CLS_GEOCURVE||nGeoType==CLS_GEODCURVE||nGeoType==CLS_GEOPARALLEL)
		{
			return TRUE;
		}
		strObjType = _T("(线)");
	}
	else if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
	{
		if (nGeoType==CLS_GEOTEXT)
		{
			return TRUE;
		}
		strObjType = _T("(文本)");
	}
	return FALSE;
}


//hcw,2013.1.25,注记与面或封闭线的一致性检查
CAnnoandSurfaceConsistencyChkCmd::CAnnoandSurfaceConsistencyChkCmd()
{
	m_strAnnoLayerCodes = "";
	m_strAnnoText = "";
	m_strSurfaceorClosedCurveLayerCodes = "";
}

CAnnoandSurfaceConsistencyChkCmd::~CAnnoandSurfaceConsistencyChkCmd()
{
	
}

CString CAnnoandSurfaceConsistencyChkCmd::GetCheckCategory() const
{
	return StrFromResID(IDS_CHK_CATEGORY_BASIC);
}

CString CAnnoandSurfaceConsistencyChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_CMD_ANNO_SURFACE_CONSISTENCY);
}

void CAnnoandSurfaceConsistencyChkCmd::GetParams( CValueTable& tab ) const
{
	_variant_t var;
	var = (LPCTSTR)(_bstr_t)m_strAnnoLayerCodes;  //hcw,2013.1.25,LPCTSTR←→_bstr_t
	tab.AddValue(CHK_ANNO_LAYCODES,&CVariantEx(var));
	var = (LPCTSTR)(_bstr_t)m_strAnnoText;
	tab.AddValue(CHK_ANNO_TEXT,&CVariantEx(var));
	var = (LPCTSTR)(_bstr_t)m_strSurfaceorClosedCurveLayerCodes;
	tab.AddValue(CHK_SURFACE_CLOSEDCURVE_LAYCODES, &CVariantEx(var));
	
	return;
}

void CAnnoandSurfaceConsistencyChkCmd::SetParams( CValueTable& tab )
{
	const CVariantEx *var;
	if (tab.GetValue(0, CHK_ANNO_LAYCODES, var))
	{
		m_strAnnoLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0, CHK_ANNO_TEXT, var))
	{
		m_strAnnoText = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0, CHK_SURFACE_CLOSEDCURVE_LAYCODES, var))
	{
		m_strSurfaceorClosedCurveLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	return;
}

void CAnnoandSurfaceConsistencyChkCmd::FillShowParams( CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(_T("CAnnoandSurfaceConsistencyChkCmd"),_T("CAnnoandSurfaceConsistencyChkCmd"));
	param->AddLayerNameParamEx(CHK_ANNO_LAYCODES, m_strAnnoLayerCodes, StrFromResID(IDS_CHKCMD_ANNO_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_TEXT);
	param->AddParam(CHK_ANNO_TEXT, (LPCTSTR)m_strAnnoText, StrFromResID(IDS_CHKCMD_ANNO_TEXT));
	param->AddLayerNameParamEx(CHK_SURFACE_CLOSEDCURVE_LAYCODES,m_strSurfaceorClosedCurveLayerCodes,StrFromResID(IDS_CHKCMD_SURFACE_CLOSEDCURVE_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
}

void CAnnoandSurfaceConsistencyChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if (!pDS) return;
	
	//判断注记层和面层是否为空
	if (m_strSurfaceorClosedCurveLayerCodes.IsEmpty()||m_strAnnoLayerCodes.IsEmpty())
	{
		return;
	}
	vector<CFeature*> container;
	vector<CFeature*> textContainer;
	
	//将选中层中的面或封闭线地物都选出来。
	CUIntArray clsArr;
	clsArr.Add(CLS_GEOSURFACE);
	clsArr.Add(CLS_GEOCURVE);
    LoadDataSource(pDS,clsArr,container,m_strSurfaceorClosedCurveLayerCodes,TRUE);

	//将选中层的文本对象都挑选出来。
	clsArr.RemoveAll();
	clsArr.Add(CLS_GEOTEXT);
	LoadDataSource(pDS,clsArr,textContainer,m_strAnnoLayerCodes);

	MARK_SELECTION;
	GProgressStart(container.size());
	vector<CFeature*>::const_iterator it;
	
	UINT nGeoType = -1;
	for (it = container.begin(); it < container.end(); it++)
	{
		GProgressStep();
		CFeature *pFtr = (*it);
		SKIP_NOT_SELECTION;
		if(!CheckAnnoandSurfaceConsistency(pFtr,textContainer,nGeoType))
		{
			//记录到检查结果
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			
			m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
			CString strReason;
			if (nGeoType==CLS_GEOSURFACE)
			{
				strReason = StrFromResID(IDS_CHK_CMD_ANNO_SURFACE_NOT_CONSISTENT);
			}
			else
			{
				strReason = StrFromResID(IDS_CHK_CMD_ANNO_CLOSEDCURVE_NOT_CONSISTENT);
			}
			m_pChkResMgr->SetReason(strReason);
			m_pChkResMgr->EndResItem();
		}
			
	}
	GProgressEnd();
}

void CAnnoandSurfaceConsistencyChkCmd::GetAllReasons( CStringArray& reasons )
{
	reasons.Add(StrFromResID(IDS_CHK_CMD_ANNO_SURFACE_NOT_CONSISTENT));
	reasons.Add(StrFromResID(IDS_CHK_CMD_ANNO_CLOSEDCURVE_NOT_CONSISTENT));
}


BOOL CAnnoandSurfaceConsistencyChkCmd::CheckAnnoandSurfaceConsistency( CFeature* pFtr, vector<CFeature*>& textContainer, UINT& nGeoType )
{	
	CGeometry *pGeo = pFtr->GetGeometry();
	CArray<PT_3DEX,PT_3DEX> curvePtsArr;
	pGeo->GetShape(curvePtsArr);
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
	{
		return TRUE;
	}

	nGeoType = pGeo->GetClassType();
	((CGeoCurveBase*)pGeo)->GetShape()->GetPts(curvePtsArr);

	curvePtsArr.SetSize(GraphAPI::GKickoffSame2DPoints(curvePtsArr.GetData(),curvePtsArr.GetSize()));

	if(curvePtsArr.GetSize()<=3)
	{
		return TRUE;
	}
	if (textContainer.size()<=0)
	{
		return FALSE;
	}
	CArray<PT_3DEX, PT_3DEX> textPtsArr;
	vector<CFeature*>::const_iterator iter;
	int nInside = 0;
	for (iter = textContainer.begin(); iter!=textContainer.end(); iter++)
    {
		//找到匹配的文本
		CFeature *pTextFtr = (*iter);
		if (pTextFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CGeoText *pText = (CGeoText*)pTextFtr->GetGeometry();
			pText->GetShape(textPtsArr);
			PT_3DEX textPt = textPtsArr[0];
			if (GraphAPI::GIsPtInRegion(textPt,curvePtsArr.GetData(),curvePtsArr.GetSize())==2)
			{
				if(m_strAnnoText.GetLength()>0)
				{
					if (pText->GetText().CompareNoCase(m_strAnnoText)==0 )
					{
						nInside++;
					}
				}
			}				
		}
    }
	return (nInside==1);
}

void CAnnoandSurfaceConsistencyChkCmd::RemoveElem( CFeature* pFtr, vector<CFeature*>& container )
{
	vector<CFeature*>::iterator it;
	for (it=container.begin(); it!=container.end();it++)
	{
		if (pFtr==(*it))
		{
			container.erase(it);
		}
	}
	return; 
}

void CAnnoandSurfaceConsistencyChkCmd::GetMatchedAnnoLayInClosedCurve( CGeoCurveBase* pClosedCurveFtr, vector<CFeature*>& container, vector<CFeature*>& subContainer )
{
	if (pClosedCurveFtr)
	{
		return;
	}
	if (!pClosedCurveFtr->IsClosed())
	{
		return;
	}
	
	CArray<PT_3DEX,PT_3DEX> ptsClosedCurvePtsArr;
	CArray<PT_3DEX,PT_3DEX> ptsTextArr;
	pClosedCurveFtr->GetShape(ptsClosedCurvePtsArr);
	//在container中的找包含在pClosedCurveFtr中的文本对象
	vector<CFeature*>::iterator iter;
    for (iter=container.begin(); iter!=container.end(); iter++)
    {
		CFeature* pFtr = (*iter);
		if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			continue;
		}
		CGeoText* pTextFtr = (CGeoText*)pFtr;
		pTextFtr->GetShape(ptsTextArr);
		GraphAPI::GKickoffSame2DPoints(ptsTextArr.GetData(), ptsTextArr.GetSize());
		if (IsPtsinRegion(ptsTextArr,ptsClosedCurvePtsArr))
		{
			subContainer.push_back(pFtr);
		}

    }
	return;
}






//hcw,2013.1.30,点与面或封闭线的一致性
CPtsandSurfaceConsistencyChkCmd::CPtsandSurfaceConsistencyChkCmd()
{
	m_strSurfaceorClosedCurveLayerCodes = _T("");
}

CPtsandSurfaceConsistencyChkCmd::~CPtsandSurfaceConsistencyChkCmd()
{
	
}

CString CPtsandSurfaceConsistencyChkCmd::GetCheckCategory() const
{
	return StrFromResID(IDS_CHK_CATEGORY_BASIC);	
}

CString CPtsandSurfaceConsistencyChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_CMD_PTS_SURFACE_CONSISTENCY);
}

void CPtsandSurfaceConsistencyChkCmd::GetParams( CValueTable& tab ) const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strSurfaceorClosedCurveLayerCodes;
	tab.AddValue(CHK_SURFACE_CLOSEDCURVE_LAYCODES,&CVariantEx(var));
}

void CPtsandSurfaceConsistencyChkCmd::SetParams( CValueTable& tab )
{
	const CVariantEx *var;
	if (tab.GetValue(0, CHK_SURFACE_CLOSEDCURVE_LAYCODES, var))
	{
		m_strSurfaceorClosedCurveLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
}

void CPtsandSurfaceConsistencyChkCmd::FillShowParams( CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(_T("CPtsandSurfaceConsistencyChkCmd"),_T("CPtsandSurfaceConsistencyChkCmd"));
	param->AddLayerNameParamEx(CHK_SURFACE_CLOSEDCURVE_LAYCODES,m_strSurfaceorClosedCurveLayerCodes,StrFromResID(IDS_CHKCMD_SURFACE_CLOSEDCURVE_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_AREA);
}

void CPtsandSurfaceConsistencyChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	CDlgDataSource* pDS = m_pDoc->GetDlgDataSource();
	if (!pDS)
	{
		return;
	}
    vector<CFeature*> container;
	vector<CFeature*> ptsContainer;
	
	//提取选中层中的面或封闭线对象。
	CUIntArray clsArr;
	clsArr.Add(CLS_GEOSURFACE);
	clsArr.Add(CLS_GEOCURVE);
    LoadDataSource(pDS,clsArr,container,m_strSurfaceorClosedCurveLayerCodes,TRUE);

	//提取选中层中的点对象
	clsArr.RemoveAll();
	clsArr.Add(CLS_GEOSURFACEPOINT);
	LoadDataSource(pDS,clsArr,ptsContainer,m_strSurfaceorClosedCurveLayerCodes);

	MARK_SELECTION;
	GProgressStart(container.size());
	vector<CFeature*>::const_iterator it;
	UINT nGeoType = -1;
	for (it = container.begin(); it < container.end(); it++)
	{
		GProgressStep();
		CFeature *pFtr = (*it);
		if(!pFtr)continue;

		SKIP_NOT_SELECTION;
		if (!CheckPtsandSurfaceConsistency(pFtr,ptsContainer,nGeoType))
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
			CString strReason;
			if (nGeoType==CLS_GEOSURFACE)
			{
				strReason = StrFromResID(IDS_CHK_CMD_PTS_SURFACE_NOT_CONSISTENT);
			}
			else
			{
				strReason = StrFromResID(IDS_CHK_CMD_PTS_CLOSEDCURVE_NOT_CONSISTENT);
			}
			m_pChkResMgr->SetReason(strReason);
			m_pChkResMgr->EndResItem();
		}
	}
}

void CPtsandSurfaceConsistencyChkCmd::GetAllReasons( CStringArray& reasons )
{
	reasons.Add(StrFromResID(IDS_CHK_CMD_PTS_SURFACE_NOT_CONSISTENT));
	reasons.Add(StrFromResID(IDS_CHK_CMD_PTS_CLOSEDCURVE_NOT_CONSISTENT));
}

BOOL CPtsandSurfaceConsistencyChkCmd::CheckPtsandSurfaceConsistency( CFeature* pFtr, vector<CFeature*>& ptsContainer, UINT& nGeoType )
{
	nGeoType = pFtr->GetGeometry()->GetClassType();
	CArray<PT_3DEX,PT_3DEX> curvePtsArr;
	pFtr->GetGeometry()->GetShape(curvePtsArr);
    
	int npt = GraphAPI::GKickoffSame2DPoints(curvePtsArr.GetData(),curvePtsArr.GetSize());
	if(npt<=3)
	{
		return TRUE;
	}
	if (ptsContainer.size()<=0)
	{
		return FALSE;
	}
	CArray<PT_3DEX, PT_3DEX> ptsArr;
	vector<CFeature*>::const_iterator iter;
	int nInside = 0;
	for (iter = ptsContainer.begin(); iter!=ptsContainer.end(); iter++)
    {
		//找到匹配的面域点
		CFeature *pPtFtr = (*iter);
		PT_3DEX ptPos = pPtFtr->GetGeometry()->GetDataPoint(0);
		if (pPtFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)))
		{			
			if (GraphAPI::GIsPtInRegion(ptPos,curvePtsArr.GetData(),npt)==2)
			{
				nInside++;
			}			
		}
    }
	return (nInside==1);
}

//hcw,2013.1.31,高程注记一致性检查
CZandAnnoConsistencyChkCmd::CZandAnnoConsistencyChkCmd()
{
	m_strZLayerCodes = _T("");
	m_strAnnoLayerCodes = _T("");
	m_lfSearchRadius = 2.0;
	m_lfMinZ = 0.0;
	m_lfMaxZ = 0.0;
}

CZandAnnoConsistencyChkCmd::~CZandAnnoConsistencyChkCmd()
{
	
}

CString CZandAnnoConsistencyChkCmd::GetCheckCategory() const
{
	return StrFromResID(IDS_CHK_CATEGORY_BASIC);
}

CString CZandAnnoConsistencyChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_CMD_Z_ANNO_CONSISTENCY);
}

void CZandAnnoConsistencyChkCmd::GetParams( CValueTable& tab ) const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strZLayerCodes;
	tab.AddValue(CHK_Z_CODE,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strAnnoLayerCodes;
	tab.AddValue(CHK_ANNO_LAYCODES,&CVariantEx(var));
	var = double(m_lfSearchRadius);
	tab.AddValue(CHK_Z_SEARCH_RADIUS,&CVariantEx(var));
	var = double(m_lfMaxZ);
	tab.AddValue(CHK_Z_MAX,&CVariantEx(var));
	var = double(m_lfMinZ);
	tab.AddValue(CHK_Z_MIN,&CVariantEx(var));
	return;	
}
void CZandAnnoConsistencyChkCmd::SetParams( CValueTable& tab )
{
	const CVariantEx *var;
	if (tab.GetValue(0,CHK_Z_CODE,var))
	{
		m_strZLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0,CHK_ANNO_LAYCODES,var))
	{
		m_strAnnoLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0,CHK_Z_SEARCH_RADIUS,var))
	{
		m_lfSearchRadius = (double)(_variant_t)*var;
	}
	if (tab.GetValue(0,CHK_Z_MAX,var))
	{
		m_lfMaxZ = (double)(_variant_t)*var;
	}
	if (tab.GetValue(0,CHK_Z_MIN,var))
	{
		m_lfMinZ = (double)(_variant_t)*var;
	}
	return;
}
void CZandAnnoConsistencyChkCmd::FillShowParams( CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CZandAnnoConsistencyChkCmd","CZandAnnoConsistencyChkCmd");
	param->AddLayerNameParamEx(CHK_Z_CODE,m_strZLayerCodes, StrFromResID(IDS_CHKCMD_Z_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_POINT);
	param->AddLayerNameParamEx(CHK_ANNO_LAYCODES,m_strAnnoLayerCodes, StrFromResID(IDS_CHKCMD_ANNO_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_TEXT);
	param->AddParam(CHK_Z_SEARCH_RADIUS,m_lfSearchRadius, StrFromResID(IDS_CHKCMD_Z_SEARCH_RADIUS));
	param->AddParam(CHK_Z_MAX,m_lfMaxZ, StrFromResID(IDS_CHKCMD_DES_MAXZ));
	param->AddParam(CHK_Z_MIN,m_lfMinZ, StrFromResID(IDS_CHKCMD_DES_MINZ));
}

void CZandAnnoConsistencyChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	CDlgDataSource* pDS = m_pDoc->GetDlgDataSource();

	if (!pDS)
	{
		return;
	}

	if (m_strZLayerCodes.IsEmpty()
		||m_strAnnoLayerCodes.IsEmpty()
		||m_lfSearchRadius==0.0f
		||(m_lfMinZ>m_lfMaxZ&&m_lfMaxZ!=0.0f))
	{
		return;
	}

	int nScale= pDS->GetScale();
	double lfSearchRadius = nScale*m_lfSearchRadius/1000.0;

	vector<CFeature*> container;
	vector<CFeature*> zTextContainer;
	
	//提取选中高程点层地物
	CUIntArray clsArr;
	clsArr.Add(CLS_GEOPOINT);
	if (m_lfMinZ==0.0&&m_lfMaxZ==0.0)
	{
		LoadDataSource(pDS,clsArr,container,m_strZLayerCodes);  
	}
    else
	{
		LoadDataSourceinZRange(pDS,clsArr,container,m_strZLayerCodes,m_lfMinZ,m_lfMaxZ);
	}
		
	
	//提取选中注记层地物
	clsArr.RemoveAll();
	clsArr.Add(CLS_GEOTEXT);
	LoadDataSource(pDS,clsArr,zTextContainer,m_strAnnoLayerCodes);

	MARK_SELECTION;
	vector<CFeature*>::const_iterator it;
	for (it = container.begin(); it<container.end(); it++)
	{
		GProgressStep();
		CFeature* pFtr = (*it);
		SKIP_NOT_SELECTION;
		if (!CheckZandAnnoConsistency(pFtr,zTextContainer,lfSearchRadius))
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_Z_ANNO_NOT_CONSISTENT));
			m_pChkResMgr->EndResItem();
		}
	}




}

void CZandAnnoConsistencyChkCmd::GetAllReasons( CStringArray& reasons )
{
	reasons.Add(StrFromResID(IDS_CHK_CMD_Z_ANNO_NOT_CONSISTENT));
}

BOOL CZandAnnoConsistencyChkCmd::CheckZandAnnoConsistency( CFeature* pFtr, vector<CFeature*>& zTextContainer, double lfSearchRadius )
{
	if (!pFtr||lfSearchRadius==0.0f)
	{
		return TRUE;
	}
	
	//是否为点地物
	CArray<PT_3DEX,PT_3DEX> zPtsArr;
    if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
    {
		CGeoPoint* pPt = (CGeoPoint*)pFtr->GetGeometry();
		pPt->GetShape(zPtsArr);
    }
	else 
		return TRUE;
	GraphAPI::GKickoffSame2DPoints(zPtsArr.GetData(),zPtsArr.GetSize());
	if (zPtsArr.GetSize()<1)
	{
		return TRUE;
	}
	PT_3DEX zPt = zPtsArr.GetAt(0);
	if (zTextContainer.size()<1)
	{
		return FALSE;
	}
	
	vector<CFeature*>::const_iterator iter;
	CArray<PT_3DEX,PT_3DEX> zTextPtsArr;

	for (iter=zTextContainer.begin(); iter!=zTextContainer.end(); iter++)
	{
		CFeature* pFtr = (*iter);
		if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CGeoText *pTextFtr = (CGeoText*)((*iter)->GetGeometry());
			pTextFtr->GetShape(zTextPtsArr);

			GraphAPI::GKickoffSame2DPoints(zTextPtsArr.GetData(),zTextPtsArr.GetSize());
			if (zTextPtsArr.GetSize()<1)
			{
				continue;
			}
			PT_3DEX zTextPt = zTextPtsArr[0];
			//判断点zTextPt与点zPt距离;
			double lfDis = GraphAPI::GGet2DDisOf2P(zTextPt,zPt) ;//hcw,for monitor;
			if(GraphAPI::GGet2DDisOf2P(zTextPt,zPt)<=lfSearchRadius)
			{
				//判断高程点的值和注记类型是否一致
				CString strText = pTextFtr->GetText();
				int nLenText = strText.GetLength();
				int nPrecision = nLenText-strText.ReverseFind('.')-1;
				CString strPrecision;
				strPrecision.Format("%d",nPrecision);
				double lfZValue = zPt.z;
				CString strZValue;
				sprintf(strZValue.GetBuffer(nLenText),"%."+strPrecision+"f",lfZValue);
				if (strZValue==strText)
				{
					return TRUE;
				}
				else
					continue;
				strZValue.ReleaseBuffer();
			}
			else
				continue;
			
		}
		else
			continue;
	}
	return FALSE;
}

BOOL CZandAnnoConsistencyChkCmd::CheckParamsValid( CString &reason )
{
	if(m_lfMinZ > m_lfMaxZ&&m_lfMaxZ!=0)
	{
		reason = StrFromResID(IDS_PARAM_ERROR);
		return FALSE;
	}
	return TRUE;
}

void CZandAnnoConsistencyChkCmd::LoadDataSourceinZRange( CDlgDataSource* pDS, CUIntArray& clsArr, vector<CFeature*>& zPtsContainer, CString strZLayerCodes, double lfMinZ, double lfMaxZ )
{
	if (!pDS)
	{
		return;
	}
	
	vector<CFeature*> container;
	LoadDataSource(pDS,clsArr,container,strZLayerCodes);
	CArray<PT_3DEX,PT_3DEX> ptsArr;
	vector<CFeature*>::const_iterator it;
	for(it=container.begin(); it<container.end(); it++)
	{
		CFeature *pFtr = (*it);
		PT_3DEX pt;
		pFtr->GetGeometry()->GetShape(ptsArr);
		pt = ptsArr.GetAt(0);
		if ((pt.z>=lfMinZ&&pt.z<=lfMaxZ)
			||(pt.z>=lfMinZ&&lfMaxZ==0.0))
		{
			zPtsContainer.push_back(pFtr);
		}
		
	}
	return;
}






//新加功能实现部分---2013.09.18
//1.交叉位置附属物缺失检查
CAppendantLoseChkCmd::CAppendantLoseChkCmd()
{
	sch_radius = 50;
}

CAppendantLoseChkCmd::~CAppendantLoseChkCmd()
{
}

CString CAppendantLoseChkCmd::GetCheckCategory() const
{
	return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CAppendantLoseChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_CMD_APPENDANT_LOSE);
}

BOOL CAppendantLoseChkCmd::CheckParamsValid(CString &reason)
{
	return TRUE;
}

void CAppendantLoseChkCmd::GetParams(CValueTable& tab) const
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_cross_layer;
	tab.AddValue(CHK_CROSS_CODE, &CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_crossed_layer;
	tab.AddValue(CHK_CROSSED_CODE, &CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_appendant_layer;
	tab.AddValue(CHK_APPENDANT_CODE, &CVariantEx(var));	
}

void CAppendantLoseChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0, CHK_CROSS_CODE, var) )
	{
		m_cross_layer = (const char*)(_bstr_t)(_variant_t)*var;
	}

	if( tab.GetValue(0, CHK_CROSSED_CODE, var) )
	{
		m_crossed_layer = (const char*)(_bstr_t)(_variant_t)*var;
	}

	if( tab.GetValue(0, CHK_APPENDANT_CODE, var) )
	{
		m_appendant_layer = (const char*)(_bstr_t)(_variant_t)*var;
	}
}

void CAppendantLoseChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CAppendantLoseChkCmd"),_T("CAppendantLoseChkCmd"));

	param->AddLayerNameParam(CHK_CROSS_CODE, m_cross_layer, StrFromResID(IDS_CHK_CROSS_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA );
	param->AddLayerNameParam(CHK_CROSSED_CODE, m_crossed_layer, StrFromResID(IDS_CHK_CROSSED_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA );
	param->AddLayerNameParam(CHK_APPENDANT_CODE, m_appendant_layer, StrFromResID(IDS_CHK_APPENDANT_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY );
}

void CAppendantLoseChkCmd::Execute(BOOL bForSelection)
{
	
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS) return;

	if( m_cross_layer.IsEmpty() || m_crossed_layer.IsEmpty() || m_appendant_layer.IsEmpty() )
		return;

	CPtrArray arrCRS, arrCRSED;
	CFtrArray ftrsCRS, ftrsCRSED;
	int i = 0, j = 0, k = 0;

	//交叉层地物
	if( !pDS->GetFtrLayer(m_cross_layer, NULL, &arrCRS) )
		return;
	int nTarCRS = arrCRS.GetSize();
	for(i = 0; i < nTarCRS; i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrCRS.GetAt(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (j=0; j<nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				continue;
			ftrsCRS.Add( pFtr );
		}
	}

	//被交叉层地物
	if( !pDS->GetFtrLayer(m_crossed_layer, NULL, &arrCRSED) )
		return;
	int nTarCRSED = arrCRSED.GetSize();
	for(i = 0; i < nTarCRSED; i++)
	{
		CFtrLayer* pLayer = (CFtrLayer*)arrCRSED.GetAt(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for (j=0; j<nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				continue;
			ftrsCRSED.Add( pFtr );
		}
	}

	int nCRS = ftrsCRS.GetSize();
	int nCRSED = ftrsCRSED.GetSize();
	MARK_SELECTION;
	GProgressStart(nCRS);

	for(i = 0; i < nCRS; i++)
	{
		CFeature* pFtr = ftrsCRS.GetAt(i);
		SKIP_NOT_SELECTION;

		//层名相同时，实际上两数组中的对象是相同的（包括排列顺序），为了避免重复，做如下处理
		if( 0 == m_cross_layer.CompareNoCase( m_crossed_layer ) )
			j = i + 1;
		else
			j = 0;

		for( ; j < nCRSED; j++)
		{			
			CFeature* pFtr2 = ftrsCRSED.GetAt(j);
			CArray<PT_3DEX,PT_3DEX> arrPts;
			if( FALSE == GetAllIntersectPts(pFtr, pFtr2, arrPts) )	//计算出两地物的所有交点
				continue;
			int npt = arrPts.GetSize();
			for(k = 0; k < npt; k++)
			{
				PT_3DEX pt = arrPts.GetAt(k);
				if( 0 == CheckPt(pt) )
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr( pFtr );
					m_pChkResMgr->SetAssociatedPos( pt );
					m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_REASON_APPENDANT_LOSE));
					m_pChkResMgr->EndResItem();
				}
			}

		}
		GProgressStep();
	}

	GProgressEnd();
	CLEAR_MARK_SELECTION;
}

void CAppendantLoseChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add( StrFromResID(IDS_CHK_REASON_APPENDANT_LOSE) );
}

BOOL CAppendantLoseChkCmd::CheckPt(PT_3DEX pt)
{//将搜索到的地物的层名，与指定的层名对比
	CDlgDataSource* pDS = m_pDoc->GetDlgDataSource();
	CDataQuery* pDQ = m_pDoc->GetDataQuery();
	struct SearchNearest sr;
	sr.pt = pt;
	sr.r = sch_radius;

	(pDQ->m_arrFoundHandles).RemoveAll();
	int num = pDQ->FindObject( &sr );
	if( num <= 0 )
		return FALSE;

	for(int i = 0; i < num; i++)
	{
		CFeature* pFtr = (pDQ->m_arrFoundHandles).GetAt(i);
		CFtrLayer* pLayer = pDS->GetFtrLayerOfObject( pFtr );
		CString strLayerName = pLayer->GetName();
		//2013.10.30，增加附属层对层码的识别
		__int64 id = -1;
		CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
		if (pScheme)
		{
			CSchemeLayerDefine *p_app_layer = pScheme->GetLayerDefine( strLayerName );
			if( p_app_layer )
				id = p_app_layer->GetLayerCode();
		}
		CString strID;
		strID.Format("%I64d", id);
		//
		if( 0 == strLayerName.CompareNoCase(m_appendant_layer) || 0 == strID.CompareNoCase(m_appendant_layer) )
			return TRUE;
	}
	return FALSE;
}

BOOL CAppendantLoseChkCmd::GetAllIntersectPts(CFeature* pFtr1, CFeature* pFtr2, CArray<PT_3DEX,PT_3DEX>& arrPts)
{//线线相交、线面相交、面面相交全部简化成线线相交
	arrPts.RemoveAll();
	CArray<PT_3DEX,PT_3DEX> ptsA1, ptsA2, ptsB1, ptsB2, ptsRet;
	ExtractPts(pFtr1, ptsA1, ptsA2);
	ExtractPts(pFtr2, ptsB1, ptsB2);

	ptsRet.RemoveAll();
	GetPtsOfIntersect(&ptsA1, &ptsB1, ptsRet);
	if( ptsRet.GetSize() != 0)
		arrPts.Append( ptsRet );

	ptsRet.RemoveAll();
	GetPtsOfIntersect(&ptsA1, &ptsB2, ptsRet);
	if( ptsRet.GetSize() != 0)
		arrPts.Append( ptsRet );

	ptsRet.RemoveAll();
	GetPtsOfIntersect(&ptsA2, &ptsB1, ptsRet);
	if( ptsRet.GetSize() != 0)
		arrPts.Append( ptsRet );

	ptsRet.RemoveAll();
	GetPtsOfIntersect(&ptsA2, &ptsB2, ptsRet);
	if( ptsRet.GetSize() != 0)
		arrPts.Append( ptsRet );

	if( arrPts.GetSize() != 0)
		return TRUE;
	return FALSE;
}


void CAppendantLoseChkCmd::ExtractPts(CFeature* pFtr, CArray<PT_3DEX,PT_3DEX>& pts1, CArray<PT_3DEX,PT_3DEX>& pts2)
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	const CShapeLine* pSL = pFtr->GetGeometry()->GetShape();
	pSL->GetPts( arrPts );
	if( arrPts.GetSize() <= 0)
		return;
	pts1.RemoveAll();
	pts2.RemoveAll();

	if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
	{//线
		if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
		{//双线
			int n = arrPts.GetSize();
			int i = 0;
			while( i < n )
			{
				PT_3DEX pt = arrPts.GetAt( i );
				if( penMove == pt.pencode )
					break;
				pts1.Add( pt );
				i++;
			}

			while( i < n )
			{
				pts2.Add( arrPts.GetAt( i ) );
				i++;
			}
		}
		else if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
		{//平行线
			int n = arrPts.GetSize();
			int i;
			for(i = 0; i < n; i++)
				pts1.Add( arrPts.GetAt(i) );

			double width = ((CGeoParallel*)pFtr->GetGeometry())->GetWidth();
			PT_3DEX* ptsRet = new PT_3DEX[n];
			GraphAPI::GGetParallelLine(pts1.GetData(), n, width, ptsRet);

			for(i = 0; i < n; i++)
			{
				pts2.Add( ptsRet[i] );
			}

			delete [] ptsRet;
		}
		else
		{//普通线
			pts1.Append( arrPts );
		}
	}
	else if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{//面
		if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) )
		{//多面
			int n = arrPts.GetSize();
			int i = 0;
			while( i < n )
			{
				PT_3DEX pt = arrPts.GetAt( i );
				if( penMove == pt.pencode )
					break;
				pts1.Add( pt );
				i++;
			}

			while( i < n )
			{
				pts2.Add( arrPts.GetAt( i ) );
				i++;
			}
		}
		else
		{//普通面
			pts1.Append( arrPts );
		}
	}
	else
	{//其他
		pts1.Append( arrPts );
	}
}

void CAppendantLoseChkCmd::GetPtsOfIntersect( CArray<PT_3DEX,PT_3DEX>* ptsLine1, CArray<PT_3DEX,PT_3DEX>* ptsLine2, CArray<PT_3DEX,PT_3DEX>& ptsOut)
{
	int n1 = ptsLine1->GetSize();
	int n2 = ptsLine2->GetSize();
	if( n1 < 2 || n2 < 2 )
		return;
	PT_3DEX* pts = new PT_3DEX[ n2 ];
	if( !pts )
		return;

	for(int i = 0; i < n1 -1; i++)
	{
		PT_3DEX pt1 = ptsLine1->GetAt(i);
		PT_3DEX pt2 = ptsLine1->GetAt(i+1);
		int retnum = 0;
		GraphAPI::GGetPointsOfIntersect(ptsLine2->GetData(), n2, pt1, pt2, pts, &retnum);

		for(int j = 0; j < retnum; j++)
		{
			PT_3DEX pt = pts[j];
			ptsOut.Add( pt );
		}
	}

	delete [] pts;
}

//2.点线悬挂检查
CPLSuspendChkCmd::CPLSuspendChkCmd()
{
	m_bAllow_pseudo_intersect = TRUE;
}

CPLSuspendChkCmd::~CPLSuspendChkCmd()
{
}

CString CPLSuspendChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CPLSuspendChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_PL_SUSPEND);
}

BOOL CPLSuspendChkCmd::CheckParamsValid(CString &reason)
{
	return TRUE;
}

void CPLSuspendChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;		
	var = (_bstr_t)(LPCTSTR)m_point_layer;
	tab.AddValue(CHK_PLS_POINT_CODE, &CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_line_layer;
	tab.AddValue(CHK_PLS_LINE_CODE, &CVariantEx(var));

	var = (bool)m_bAllow_pseudo_intersect;
	tab.AddValue(CHK_PLS_PSEUDO_SECT, &CVariantEx(var));
}

void CPLSuspendChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_PLS_POINT_CODE,var) )
	{
		m_point_layer = (const char*)(_bstr_t)(_variant_t)*var;
	}

	if( tab.GetValue(0,CHK_PLS_LINE_CODE,var) )
	{
		m_line_layer = (const char*)(_bstr_t)(_variant_t)*var;
	}

	if( tab.GetValue(0,CHK_PLS_PSEUDO_SECT,var) )
	{
		m_bAllow_pseudo_intersect = (bool)(_variant_t)*var;
	}
}

void CPLSuspendChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CPLSuspendChkCmd"),_T("CPLSuspendChkCmd"));	
	param->AddLayerNameParamEx(CHK_PLS_POINT_CODE, m_point_layer, StrFromResID(IDS_CHKCMD_PLS_POINT_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY_POINT);
	param->AddLayerNameParamEx(CHK_PLS_LINE_CODE, m_line_layer, StrFromResID(IDS_CHKCMD_PLS_LINE_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddParam(CHK_PLS_PSEUDO_SECT, (bool)m_bAllow_pseudo_intersect, StrFromResID(IDS_CHKCMD_PLS_PSEUDO_INTERSECT));
}

void CPLSuspendChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS) return;

	int i = 0, j = 0;

	//1.找出所有符合要求的点状地物
	CFtrArray ptObjs;
	if( m_point_layer.IsEmpty() )
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for(j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;			
				if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
					continue;
				ptObjs.Add( pFtr );					
			}
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_point_layer,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
						continue;
					ptObjs.Add( pFtr );
				}
			}
		}
		//
	}

	//2.设置线状地物层的查询标志
	CFtrLayerArray layers_line;
	pDS->GetFtrLayersByNameOrCode(m_line_layer, layers_line);
	if( layers_line.GetSize()<=0 )
	{
		pDS->SaveAllQueryFlags(TRUE,TRUE);
	}
	else
	{
		//pDS->SaveAllQueryFlags(TRUE,FALSE);
		pDS->SaveAllQueryFlags(TRUE,TRUE);
		for(i=0; i<layers_line.GetSize(); i++)
		{
			layers_line[i]->SetAllowQuery(TRUE);
		}
	}

	//3.处理
	int num = ptObjs.GetSize();
	if( num <= 0 )
		return;
	MARK_SELECTION;
	GProgressStart(num);
	for(i = 0; i < num; i++)
	{
		GProgressStep();
		CFeature* pFtr = ptObjs.GetAt(i);
		SKIP_NOT_SELECTION;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pFtr->GetGeometry()->GetShape(arrPts);
		if( arrPts.GetSize() <=0 )
			continue;
		PT_3DEX pt = arrPts.GetAt(0);

		if( !CheckPoint(pt, pFtr) )
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr( pFtr );
			m_pChkResMgr->SetAssociatedPos( pt );
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_REASON_POINT_PSEUDO));
			m_pChkResMgr->EndResItem();
		}
	}
	GProgressEnd();
	CLEAR_MARK_SELECTION;
	pDS->RestoreAllQueryFlags();
}

void CPLSuspendChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add( StrFromResID(IDS_CHK_REASON_POINT_PSEUDO) );
}

int CPLSuspendChkCmd::IsNodeOfLine(PT_3DEX pt, CFeature* pFtr)
{
	const CShapeLine* pSL = pFtr->GetGeometry()->GetShape();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pSL->GetPts( arrPts );
	int n = arrPts.GetSize();

	for(int i = 0; i < n; i++)
	{
		PT_3DEX pt2 = arrPts.GetAt(i);
		if( fabs(pt.x - pt2.x) < 1e-4 && fabs(pt.y - pt2.y) < 1e-4 && fabs(pt.z - pt2.z) < 1e-4 )
			return 1;
	}
	return 0;
}

BOOL CPLSuspendChkCmd::CheckPoint(PT_3DEX pt, CFeature* pFtr)
{
	CDlgDataSource* pDS = m_pDoc->GetDlgDataSource();
	CDataQuery* pDQ = m_pDoc->GetDataQuery();

	double r = GraphAPI::GetDisTolerance();
	Envelope e;
	e.CreateFromPtAndRadius(pt, r);

	int num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);
	if( num <= 0 )
		return FALSE;

	const CPFeature *ftrs = pDQ->GetFoundHandles(num);
	for(int i = 0; i < num; i++)
	{
		CFeature* pFound = ftrs[i];
		if( pFound == pFtr )
			continue;

		if( !pFound->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			continue;

		if( !m_line_layer.IsEmpty() )
		{//将搜索到的地物的层名或层码与参数中的值对比
			CFtrLayer* pLayer = pDS->GetFtrLayerOfObject( pFound );
			CString strLayerName = pLayer->GetName();
			__int64 id = -1;
			CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
			if (pScheme)
			{
				CSchemeLayerDefine *p_app_layer = pScheme->GetLayerDefine( strLayerName );
				id = p_app_layer->GetLayerCode();
			}
			CString strID;
			strID.Format("%I64d", id);


			CStringArray arr;
			convertStringToStrArray(m_line_layer,arr);

			BOOL match = FALSE;
			for(int j = 0; j < arr.GetSize(); j++)
			{
				CString str = arr.GetAt(j);
				if( 0 == str.CompareNoCase(strID) || 0 == str.CompareNoCase(strLayerName) )
				{
					match = TRUE;
					break;
				}
			}
			if( FALSE == match )
				return FALSE;
			
		}

		//是否容忍虚相交
		if( m_bAllow_pseudo_intersect )
			return TRUE;
		else
		{
			if( IsNodeOfLine(pt, pFound) )
				return TRUE;
		}

	}

	return FALSE;
}



//3.线线悬挂检查
CLLSuspendChkCmd::CLLSuspendChkCmd()
{
	m_bAllow_pseudo_intersect = TRUE;
}

CLLSuspendChkCmd::~CLLSuspendChkCmd()
{
}

CString CLLSuspendChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CLLSuspendChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_LL_SUSPEND);
}

BOOL CLLSuspendChkCmd::CheckParamsValid(CString &reason)
{
	return TRUE;
}

void CLLSuspendChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;		
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_LLS_LAYER_CODE, &CVariantEx(var));

	var = (bool)m_bAllow_pseudo_intersect;
	//tab.AddValue(CHK_PLS_PSEUDO_SECT, &CVariantEx(var));
	tab.AddValue(CHK_LLS_PSEUDO_SECT, &CVariantEx(var));
}

void CLLSuspendChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_LLS_LAYER_CODE,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}

	if( tab.GetValue(0,CHK_LLS_PSEUDO_SECT,var) )
	{
		m_bAllow_pseudo_intersect = (bool)(_variant_t)*var;
	}
}

void CLLSuspendChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CLLSuspendChkCmd"),_T("CLLSuspendChkCmd"));	
	param->AddLayerNameParamEx(CHK_LLS_LAYER_CODE,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddParam(CHK_LLS_PSEUDO_SECT, (bool)m_bAllow_pseudo_intersect, StrFromResID(IDS_CHKCMD_PLS_PSEUDO_INTERSECT),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
}

void CLLSuspendChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)
		return;

	int i =0 , j =0;

	//1.获取所有符合要求的线状地物
	CFtrArray ftrs;
	if( m_strLayerCodes.IsEmpty() )
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for(j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;			
				if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
					continue;
				if( ((CGeoCurveBase*)pFtr->GetGeometry())->IsClosed() )
					continue;

				ftrs.Add( pFtr );					
			}
		}

	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;	
		for(int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;				
					if ( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
						continue;
					if( ((CGeoCurveBase*)pFtr->GetGeometry())->IsClosed() )
						continue;
					
					ftrs.Add( pFtr );
				}
			}
		}
		//
		
	}

	

	//2.处理
	int num = ftrs.GetSize();
	MARK_SELECTION;
	GProgressStart(num);

	for(i = 0; i < num; i++)
	{
		GProgressStep();
		CFeature* pFtr = ftrs.GetAt( i );
		SKIP_NOT_SELECTION;

		CGeometry *pGeo = pFtr->GetGeometry();
		CGeoCurveBase *pGeoCurve = (CGeoCurveBase*)pGeo;
		CArray<PT_3DEX,PT_3DEX> arrPts; 
		pGeoCurve->GetShape(arrPts);		

		int npt= arrPts.GetSize();
		if( npt<2 )
			continue;

		PT_3DEX ptS = arrPts.GetAt(0);			//首点
		PT_3DEX ptE = arrPts.GetAt(npt-1);		//尾点

		if( !CheckPoint(ptS, pFtr) )
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr( pFtr );
			m_pChkResMgr->SetAssociatedPos( ptS );
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_REASON_LINE_PSEUDO));
			m_pChkResMgr->EndResItem();

			continue;	//如果首点已悬挂，为了节省时间，不再判断尾点
		}

		if( !CheckPoint(ptE, pFtr) )
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr( pFtr );
			m_pChkResMgr->SetAssociatedPos( ptE );
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_REASON_LINE_PSEUDO));
			m_pChkResMgr->EndResItem();
		}
	}

	GProgressEnd();
	CLEAR_MARK_SELECTION;
}

void CLLSuspendChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add( StrFromResID(IDS_CHK_REASON_LINE_PSEUDO) );
}

int CLLSuspendChkCmd::IsNodeOfLine(PT_3DEX pt, CFeature* pFtr)
{
	const CShapeLine* pSL = pFtr->GetGeometry()->GetShape();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pSL->GetPts( arrPts );
	int n = arrPts.GetSize();

	for(int i = 0; i < n; i++)
	{
		PT_3DEX pt2 = arrPts.GetAt(i);
		if( fabs(pt.x - pt2.x) < 1e-4 && fabs(pt.y - pt2.y) < 1e-4 && fabs(pt.z - pt2.z) < 1e-4 )
			return 1;
	}
	return 0;
}

BOOL CLLSuspendChkCmd::CheckPoint(PT_3DEX pt, CFeature* pFtr)
{
	CDlgDataSource* pDS = m_pDoc->GetDlgDataSource();
	CDataQuery* pDQ = m_pDoc->GetDataQuery();

	double r = GraphAPI::GetDisTolerance();
	Envelope e;
	e.CreateFromPtAndRadius(pt, r);

	int num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);
	if( num <= 0 )
		return FALSE;

	const CPFeature *ftrs = pDQ->GetFoundHandles(num);
	for(int i = 0; i < num; i++)
	{
		CFeature* pFound = ftrs[i];
		if( pFound == pFtr )
			continue;

		if( !pFound->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			continue;

		//是否容忍虚相交
		if( m_bAllow_pseudo_intersect )
			return TRUE;
		else
		{
			if( IsNodeOfLine(pt, pFound) )
				return TRUE;
		}

	}

	return FALSE;
}

//4.不合理断线检查
CUnreasonableBLChkCmd::CUnreasonableBLChkCmd()
{
	m_lfInterval = 0.0;
}

CUnreasonableBLChkCmd::~CUnreasonableBLChkCmd()
{
}

CString CUnreasonableBLChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CUnreasonableBLChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_UNREASONABLE_BL);
}

BOOL CUnreasonableBLChkCmd::CheckParamsValid(CString &reason)
{
// 	if( m_lfInterval <= GraphAPI::GetDisTolerance() )
// 	{
// 		reason = StrFromResID(IDS_PARAM_ERROR);
// 		return FALSE;
// 	}
	return TRUE;
}

void CUnreasonableBLChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;	
	
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue(CHK_URBL_LAYER_CODE, &CVariantEx(var));

	var = (double)m_lfInterval;
	tab.AddValue(CHK_URBL_INTERVAL, &CVariantEx(var));
}

void CUnreasonableBLChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_URBL_LAYER_CODE,var) )
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}

	if( tab.GetValue(0,CHK_URBL_INTERVAL,var) )
	{
		m_lfInterval = (double)(_variant_t)*var;
	}
}

void CUnreasonableBLChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CUnreasonableBLChkCmd"),_T("CUnreasonableBLChkCmd"));
	param->AddLayerNameParamEx(CHK_URBL_LAYER_CODE,m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddParam(CHK_URBL_INTERVAL, m_lfInterval, StrFromResID(IDS_CHKCMD_URBL_BL_INTERVAL));
}

void CUnreasonableBLChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
//	CArray<CFtrArray*, CFtrArray*> ftrs;

	if( m_strLayerCodes.IsEmpty() )
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		for(int i=0;i<nFtrLay;i++)
		{
			CFtrArray ftrsPerLayer;	//层名相同的地物，放到同一个数组中

			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for(int j = 0; j < nObj; j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				if ( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
					continue;
				if( ((CGeoCurveBase*)pFtr->GetGeometry())->IsClosed() )
					continue;

				ftrsPerLayer.Add( pFtr );
			}

			//ftrs.Add( &ftrsPerLayer );
			//读一层，检查一层
			int num = ftrsPerLayer.GetSize();
			for(int k = 0; k < num; k++)
			{
				CFeature* pFtr = ftrsPerLayer.GetAt(k);
				SKIP_NOT_SELECTION;
				CheckFeature(pFtr, &ftrsPerLayer, k);
			}
			//---
		}
	}
	else
	{
		CStringArray arr;
		if( !convertStringToStrArray(m_strLayerCodes, arr) )
				return;
		for (int a=0;a<arr.GetSize();a++)
		{
			CFtrArray ftrsPerLayer;

			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;

			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				int nObj = pLayer->GetObjectCount();
				for (int j=0;j<nObj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr||!pFtr->IsVisible())
						continue;
					if ( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
						continue;
					if( ((CGeoCurveBase*)pFtr->GetGeometry())->IsClosed() )
						continue;
					
					ftrsPerLayer.Add( pFtr );
				}
			}

			//ftrs.Add( &ftrsPerLayer );
			int num = ftrsPerLayer.GetSize();
			for(int k = 0; k < num; k++)
			{
				CFeature* pFtr = ftrsPerLayer.GetAt(k);
				SKIP_NOT_SELECTION;
				CheckFeature(pFtr, &ftrsPerLayer, k);
			}
			//----
		}
	}

	/*
	int n = ftrs.GetSize();
	MARK_SELECTION;
	GProgressStart(n);
	for(int i = 0; i < n; i++)
	{
		CFtrArray* pArrFtr = ftrs.GetAt(i);
		int nFtrs = pArrFtr->GetSize();
		for(int j = 0; j < nFtrs; j++)
		{
			CFeature* pFtr = pArrFtr->GetAt(j);
			SKIP_NOT_SELECTION;
			CheckFeature(pFtr, pArrFtr, j);		
		}
		GProgressStep();
	}
	GProgressEnd();
	CLEAR_MARK_SELECTION;
	*/
}

void CUnreasonableBLChkCmd::GetAllReasons(CStringArray& reasons)
{	
	reasons.Add(StrFromResID(IDS_CHK_REASON_URBL_BREAKLINE));
}

void CUnreasonableBLChkCmd::CheckFeature(CFeature* pFtr, CFtrArray* pFtrArr, int index)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	pFtr->GetGeometry()->GetShape( pts );
	int npt = pts.GetSize();
	if( npt < 2 )
		return;
	PT_3DEX ptS = pts.GetAt(0);		//首点
	PT_3DEX ptE = pts.GetAt(npt-1);	//尾点

	CDataQueryEx *pDQ = m_pDoc->GetDataQuery();

	int num = pFtrArr->GetSize();
	for(int i = index+1; i < num; i++)
	{
		CFeature* pFtr2 = pFtrArr->GetAt(i);
		CArray<PT_3DEX,PT_3DEX> pts2;
		pFtr2->GetGeometry()->GetShape( pts2 );
		//int npt2 = pts.GetSize();
		int npt2 = pts2.GetSize();
		if( npt2 < 2 ) continue;
		PT_3DEX ptS2 = pts2.GetAt(0);
		//PT_3DEX ptE2 = pts2.GetAt(npt-1);
		PT_3DEX ptE2 = pts2.GetAt(npt2-1);

		double tol = GraphAPI::GetDisTolerance();
		double disS1 = GraphAPI::GGet2DDisOf2P(ptS, ptS2);
		double disS2 = GraphAPI::GGet2DDisOf2P(ptS, ptE2);
		if( disS1 <= m_lfInterval || disS2 <= m_lfInterval )
		{
			Envelope e;
			e.CreateFromPtAndRadius(ptS, m_lfInterval);
			if( pDQ->FindObjectInRect(e,NULL,FALSE,FALSE)!=2 )
			{
				continue;
			}
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(ptS);
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_REASON_URBL_BREAKLINE));
			m_pChkResMgr->EndResItem();
		}

		double disE1 = GraphAPI::GGet2DDisOf2P(ptE, ptS2);
		double disE2 = GraphAPI::GGet2DDisOf2P(ptE, ptE2);
		if( disE1 <= m_lfInterval || disE2 <= m_lfInterval )
		{
			Envelope e;
			e.CreateFromPtAndRadius(ptE, m_lfInterval);
			if( pDQ->FindObjectInRect(e,NULL,FALSE,FALSE)!=2 )
			{
				continue;
			}
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(ptE);
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_REASON_URBL_BREAKLINE));
			m_pChkResMgr->EndResItem();
		}
	}

}



//隧道或渡槽（双线）内无实交点
CNoIntersectInsideTunnelCmd::CNoIntersectInsideTunnelCmd()
{
	
}

CNoIntersectInsideTunnelCmd::~CNoIntersectInsideTunnelCmd()
{
}

CString CNoIntersectInsideTunnelCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CNoIntersectInsideTunnelCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_NOINTERSECT_INSIDETUNNEL);
}

BOOL CNoIntersectInsideTunnelCmd::CheckParamsValid(CString &reason)
{
	if( m_strLayerTunnel.GetLength()<=0 )
	{
		reason = StrFromResID(IDS_PARAM_ERROR);
		return FALSE;
	}
	return TRUE;
}

void CNoIntersectInsideTunnelCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;	
	
	var = (_bstr_t)(LPCTSTR)m_strLayerTunnel;
	tab.AddValue("LayerTunnel", &CVariantEx(var));
}

void CNoIntersectInsideTunnelCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"LayerTunnel",var) )
	{
		m_strLayerTunnel = (const char*)(_bstr_t)(_variant_t)*var;
	}
}

void CNoIntersectInsideTunnelCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("NoIntersectInsideTunnel"),_T("NoIntersectInsideTunnel"));
	param->AddLayerNameParamEx("LayerTunnel",m_strLayerTunnel,StrFromResID(IDS_CHK_TUNNELLAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
}

void CNoIntersectInsideTunnelCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;

	if( m_strLayerTunnel.GetLength()<=0 )
	{
		return;
	}
	else
	{
		CFtrLayerArray arrLayers;
		pDS->GetFtrLayersByNameOrCode(m_strLayerTunnel,arrLayers);
		CStringArray arr;
		
		CFtrArray ftrs;

		int nTar = arrLayers.GetSize();	
		for (int i=0;i<nTar;i++)
		{
			CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
			if(!pLayer||!pLayer->IsVisible()||pLayer->IsLocked())
				continue;
			
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;

				if ( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
					continue;
				
				ftrs.Add( pFtr );
			}
		}

		MARK_SELECTION;
		int num = ftrs.GetSize();
		for(int k = 0; k < num; k++)
		{
			CFeature* pFtr = ftrs.GetAt(k);
			SKIP_NOT_SELECTION;
			CheckFeature(pFtr);
		}

		CLEAR_MARK_SELECTION;
	}
}

void CNoIntersectInsideTunnelCmd::GetAllReasons(CStringArray& reasons)
{	
	reasons.Add(StrFromResID(IDS_CHK_REASON_INTERSECT_INSIDETUNNEL));
}

void CNoIntersectInsideTunnelCmd::CheckFeature(CFeature* pFtr)
{
	CGeoParallel *pGeo = (CGeoParallel*)(pFtr->GetGeometry());
	CArray<PT_3DEX,PT_3DEX> pts, pts2;
	pGeo->GetShape( pts );
	pGeo->GetParallelShape(pts2);

	Envelope e = pGeo->GetEnvelope();

	CDataQueryEx *pDQ = m_pDoc->GetDataQuery();

	int npt = pts.GetSize();
	if( npt < 2 )
		return;
	
	CArray<PT_3DEX,PT_3DEX> rangePts0,rangePts;
	rangePts0.Append(pts);
	
	for(int i=pts2.GetSize()-1; i>=0; i--)
	{
		rangePts0.Add(pts2[i]);
	}
	
	rangePts0.Add(pts[0]);
	rangePts.Copy(rangePts0);

	GraphAPI::GGetParallelLine(rangePts0.GetData(),rangePts0.GetSize(),1e-4,rangePts.GetData());

	pDQ->FindObjectInRect(e,NULL);
	
	int num = 0;
	const CPFeature *ftrs = pDQ->GetFoundHandles(num);
	if(num<=2)
		return;

	CArray<PT_3D,PT_3D> arrIntersects;

	for(i=0; i<num; i++)
	{
		if(pFtr==ftrs[i])
			continue;
		
		CGeometry *pGeo1 = ftrs[i]->GetGeometry();
		if( !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			continue;

		for(int j=i+1; j<num; j++)
		{
			if(pFtr==ftrs[j])
				continue;
			
			CGeometry *pGeo2 = ftrs[j]->GetGeometry();
			if( !pGeo2->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				continue;

			GetSolidIntersect((CGeoCurveBase*)pGeo1,(CGeoCurveBase*)pGeo2,arrIntersects);

			if(arrIntersects.GetSize()<0)
				continue;

			for(int k=0; k<arrIntersects.GetSize(); k++)
			{
				if(GraphAPI::GIsPtInRegion(PT_3DEX(arrIntersects[k],penLine),rangePts.GetData(),rangePts.GetSize())==2)
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->SetAssociatedPos(arrIntersects[k]);
					m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_REASON_INTERSECT_INSIDETUNNEL));
					m_pChkResMgr->EndResItem();
					return;
				}
			}
		}
	}
}


void CNoIntersectInsideTunnelCmd::GetSolidIntersect(CGeoCurveBase* pGeo1, CGeoCurveBase* pGeo2, CArray<PT_3D,PT_3D>& arrPts)
{
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;

	pGeo1->GetShape(arrPts1);
	pGeo2->GetShape(arrPts2);

	for(int i=0; i<arrPts1.GetSize(); i++)
	{
		for(int j=0; j<arrPts2.GetSize(); j++)
		{
			if( fabs(arrPts1[i].x-arrPts2[j].x)<GraphAPI::g_lfDisTolerance && fabs(arrPts1[i].y-arrPts2[j].y)<GraphAPI::g_lfDisTolerance )
			{
				arrPts.Add(arrPts1[i]);
			}
		}
	}
}



//高程点落水、进房屋的检查
CHeiPointOnHouseCmd::CHeiPointOnHouseCmd()
{
}

CHeiPointOnHouseCmd::~CHeiPointOnHouseCmd()
{
}

CString CHeiPointOnHouseCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CHeiPointOnHouseCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_HEIPOINT_ONHOUSE);
}

BOOL CHeiPointOnHouseCmd::CheckParamsValid(CString &reason)
{
	if( m_strLayerHeiPoint.GetLength()<=0 || m_strLayerHouse.GetLength()<=0 )
	{
		reason = StrFromResID(IDS_PARAM_ERROR);
		return FALSE;
	}
	return TRUE;
}

void CHeiPointOnHouseCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;	
	
	var = (_bstr_t)(LPCTSTR)m_strLayerHeiPoint;
	tab.AddValue("LayerHeiPoint", &CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayerHouse;
	tab.AddValue("LayerHouse", &CVariantEx(var));
	
}

void CHeiPointOnHouseCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"LayerHeiPoint",var) )
	{
		m_strLayerHeiPoint = (const char*)(_bstr_t)(_variant_t)*var;
	}

	if( tab.GetValue(0,"LayerHouse",var) )
	{
		m_strLayerHouse = (const char*)(_bstr_t)(_variant_t)*var;
	}
}

void CHeiPointOnHouseCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("HeiPointOnHouse"),_T("HeiPointOnHouse"));
	param->AddLayerNameParamEx("LayerHeiPoint",m_strLayerHeiPoint,StrFromResID(IDS_CHKCMD_Z_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_POINT);
	param->AddLayerNameParamEx("LayerHouse", m_strLayerHouse, StrFromResID(IDS_CHK_HOUSELAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
}

void CHeiPointOnHouseCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;
	
	if( m_strLayerHouse.GetLength()<=0 || m_strLayerHeiPoint.GetLength()<=0 )
	{
		return;
	}
	else
	{
		CFtrLayerArray arrLayers;
		pDS->GetFtrLayersByNameOrCode(m_strLayerHouse,arrLayers);
		CStringArray arr;
		
		CFtrArray ftrs;
		
		int nTar = arrLayers.GetSize();	
		for (int i=0;i<nTar;i++)
		{
			CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
			if(!pLayer||!pLayer->IsVisible()||pLayer->IsLocked())
				continue;
			
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				
				if ( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
					continue;
				
				ftrs.Add( pFtr );
			}
		}
		
		MARK_SELECTION;
		int num = ftrs.GetSize();
		for(int k = 0; k < num; k++)
		{
			CFeature* pFtr = ftrs.GetAt(k);
			SKIP_NOT_SELECTION;
			CheckFeature(pFtr);
		}
		
		CLEAR_MARK_SELECTION;
	}
}

void CHeiPointOnHouseCmd::GetAllReasons(CStringArray& reasons)
{	
	reasons.Add(StrFromResID(IDS_CHK_REASON_HEIPOINT_ONHOUSE));
}

void CHeiPointOnHouseCmd::CheckFeature(CFeature* pFtr)
{
	CGeoCurveBase *pGeo = (CGeoCurveBase*)(pFtr->GetGeometry());
	CArray<PT_3DEX,PT_3DEX> pts;
	pGeo->GetShape( pts );
	
	Envelope e = pGeo->GetEnvelope();
	
	CDataQueryEx *pDQ = m_pDoc->GetDataQuery();
	
	int npt = pts.GetSize();
	if( npt < 2 )
		return;
	
	CArray<PT_3DEX,PT_3DEX> rangePts0,rangePts;
	rangePts0.Append(pts);
	
	rangePts.Copy(rangePts0);
	
	GraphAPI::GGetParallelLine(rangePts0.GetData(),rangePts0.GetSize(),1e-4,rangePts.GetData());
	
	pDQ->FindObjectInRect(e,NULL);
	
	int num = 0;
	const CPFeature *ftrs = pDQ->GetFoundHandles(num);
	if(num<=1)
		return;
	
	CArray<PT_3D,PT_3D> arrIntersects;
	
	for(int i=0; i<num; i++)
	{
		if(pFtr==ftrs[i])
			continue;
		
		CGeometry *pGeo1 = ftrs[i]->GetGeometry();
		if( !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			continue;
		
		if( CAutoLinkContourCommand::CheckObjForLayerCode(m_pDoc->GetDlgDataSource(),ftrs[i],(LPCTSTR)m_strLayerHeiPoint) )
		{
			if(GraphAPI::GIsPtInRegion(pGeo1->GetDataPoint(0),rangePts.GetData(),rangePts.GetSize())==2)
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pGeo1->GetDataPoint(0));
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_REASON_HEIPOINT_ONHOUSE));
				m_pChkResMgr->EndResItem();
				return;
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
CCurveAcrossSurfaceChkCmd::CCurveAcrossSurfaceChkCmd()
{
	m_strCurveCode = _T("");
	m_strSurfaceCode = _T("");
}

CCurveAcrossSurfaceChkCmd::~CCurveAcrossSurfaceChkCmd()
{

}

CString CCurveAcrossSurfaceChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CCurveAcrossSurfaceChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_DES_CURVEARCOSSSURFACE);
}

void CCurveAcrossSurfaceChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strCurveCode;
	tab.AddValue(CHK_CURVE_CODE,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strSurfaceCode;
	tab.AddValue(CHK_SURFACE_CODE,&CVariantEx(var));
}

void CCurveAcrossSurfaceChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_CURVE_CODE,var) )
	{
		m_strCurveCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_SURFACE_CODE,var) )
	{
		m_strSurfaceCode= (const char*)(_bstr_t)(_variant_t)*var;
	}	
}

void CCurveAcrossSurfaceChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CCurveAcrossSurfaceChkCmd"),_T("CCurveAcrossSurfaceChkCmd"));

	param->AddLayerNameParamEx(CHK_CURVE_CODE,(LPCTSTR)m_strCurveCode,StrFromResID(IDS_CMDPLANE_LINELAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParamEx(CHK_SURFACE_CODE,(LPCTSTR)m_strSurfaceCode,StrFromResID(IDS_CMDPLANE_SURFACELAYER),NULL, LAYERPARAMITEM_NOTEMPTY_AREA);
}

void CCurveAcrossSurfaceChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS) return;

	if( m_strSurfaceCode.IsEmpty())
		return;
	
	CFtrLayerArray Surfacelayers, CurveLayers;
	pDS->GetFtrLayersByNameOrCode_editable(m_strSurfaceCode,Surfacelayers);
	pDS->GetFtrLayersByNameOrCode_editable(m_strCurveCode, CurveLayers);

	int i=0, nSum=0;
	//只让线图层可以查询
	pDS->SaveAllQueryFlags(TRUE,FALSE);
	for( i=0; i<CurveLayers.GetSize(); i++)
	{
		CurveLayers[i]->SetAllowQuery(TRUE);
	}
	for( i=0; i<Surfacelayers.GetSize(); i++)
	{
		Surfacelayers[i]->SetAllowQuery(FALSE);
		nSum += Surfacelayers[i]->GetObjectCount();
	}

	CDataQueryEx *pDQ = m_pDoc->GetDataQuery();
	if(!pDQ) return;

	MARK_SELECTION;
	GProgressStart(nSum);
	for(i=0; i<Surfacelayers.GetSize(); i++)
	{
		CFtrLayer* pLayer = Surfacelayers[i];
		if(!pLayer) continue;

		int nObj = pLayer->GetObjectCount();
		for(int j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature* pFtr = pLayer->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;

			SKIP_NOT_SELECTION;

			CGeometry *pObj = pFtr->GetGeometry();
			if(!pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				continue;
			if( !((CGeoCurveBase*)pObj)->IsClosed() )
				continue;

			Envelope e = pObj->GetEnvelope();

			pDQ->FindObjectInRect(e, NULL, FALSE, FALSE);
			int nNum;
			const CPFeature *ftrs = pDQ->GetFoundHandles(nNum);
			for (int k=0; k<nNum; k++)
			{
				if(pFtr==ftrs[k]) continue;
				CGeometry *pGeo = ftrs[k]->GetGeometry();
				if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
					continue;

				CArray<PtIntersect,PtIntersect> arr;
				GetIntersectPts(pFtr, ftrs[k], arr);
				for(int n=0; n<arr.GetSize(); n++)
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->AddAssociatedFtr(ftrs[k]);
					m_pChkResMgr->SetAssociatedPos(arr[n].pt);
					m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_DES_CURVEARCOSSSURFACE));
					m_pChkResMgr->EndResItem();
				}
			}
		}
	}
	GProgressEnd();

	pDS->RestoreAllQueryFlags();

	CLEAR_MARK_SELECTION;
}

void CCurveAcrossSurfaceChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_DES_CURVEARCOSSSURFACE));
}

//////////////////////////////////////////////////////////////////////////
CContourRiverChkCmd::CContourRiverChkCmd()
{
	m_strCurveCode = _T("");
	m_strSurfaceCode = _T("");
}

CContourRiverChkCmd::~CContourRiverChkCmd()
{
	
}

CString CContourRiverChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_CONTOUR);
}

CString CContourRiverChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_DES_CONTOUR_RIVER);
}

void CContourRiverChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CContourRiverChkCmd"),_T("CContourRiverChkCmd"));
	
	param->AddLayerNameParamEx(CHK_CURVE_CODE,(LPCTSTR)m_strCurveCode,StrFromResID(IDS_CONTOUR_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParamEx(CHK_SURFACE_CODE,(LPCTSTR)m_strSurfaceCode,StrFromResID(IDS_LINK_RIVERLAYERS),NULL, LAYERPARAMITEM_NOTEMPTY_AREA);
}

//////////////////////////////////////////////////////////////////////////
CCurveAcrossHouseChkCmd::CCurveAcrossHouseChkCmd()
{
	m_strCurveCode = _T("");
	m_strSurfaceCode = _T("");
}

CCurveAcrossHouseChkCmd::~CCurveAcrossHouseChkCmd()
{
	
}

CString CCurveAcrossHouseChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CCurveAcrossHouseChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_DES_CURVEARCOSSHOUSE);
}

void CCurveAcrossHouseChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CCurveAcrossHouseChkCmd"),_T("CCurveAcrossHouseChkCmd"));
	
	param->AddLayerNameParamEx(CHK_CURVE_CODE,(LPCTSTR)m_strCurveCode,StrFromResID(IDS_CMDPLANE_LINELAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParamEx(CHK_SURFACE_CODE,(LPCTSTR)m_strSurfaceCode,StrFromResID(IDS_HOUSE_LAYNAMES),NULL, LAYERPARAMITEM_NOTEMPTY_AREA);
}

extern bool IsPtInCurve(PT_3DEX pt, CArray<PT_3DEX,PT_3DEX>& arrPts);

void CCurveAcrossHouseChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS) return;
	
	if( m_strSurfaceCode.IsEmpty())
		return;
	
	CFtrLayerArray Surfacelayers, CurveLayers;
	pDS->GetFtrLayersByNameOrCode_editable(m_strSurfaceCode,Surfacelayers);
	pDS->GetFtrLayersByNameOrCode_editable(m_strCurveCode, CurveLayers);
	
	int i=0, nSum=0;
	//只让线图层可以查询
	pDS->SaveAllQueryFlags(TRUE,FALSE);
	for( i=0; i<CurveLayers.GetSize(); i++)
	{
		CurveLayers[i]->SetAllowQuery(TRUE);
	}
	for( i=0; i<Surfacelayers.GetSize(); i++)
	{
		nSum += Surfacelayers[i]->GetObjectCount();
	}
	
	CDataQueryEx *pDQ = m_pDoc->GetDataQuery();
	if(!pDQ) return;
	
	MARK_SELECTION;
	GProgressStart(nSum);
	for(i=0; i<Surfacelayers.GetSize(); i++)
	{
		CFtrLayer* pLayer = Surfacelayers[i];
		if(!pLayer) continue;
		
		int nObj = pLayer->GetObjectCount();
		for(int j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature* pFtr = pLayer->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;
			
			SKIP_NOT_SELECTION;

			CFeature *pFtr1 = pFtr->Clone();
			if(!pFtr) continue;
			
			CGeometry *pObj = pFtr1->GetGeometry();
			if(!pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				continue;

			CArray<PT_3DEX,PT_3DEX> pts;
			pObj->GetShape(pts);

			if( !((CGeoCurveBase*)pObj)->IsClosed() )
			{
				//((CGeoCurveBase*)pObj)->EnableClose(TRUE);
			}
			
			Envelope e = pObj->GetEnvelope();
			
			pDQ->FindObjectInRect(e, NULL, FALSE, FALSE);
			int nNum;
			const CPFeature *ftrs = pDQ->GetFoundHandles(nNum);
			for (int k=0; k<nNum; k++)
			{
				if(pFtr==ftrs[k]) continue;
				CGeometry *pGeo = ftrs[k]->GetGeometry();
				if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
					continue;
				
				CArray<PtIntersect,PtIntersect> arr;
				GetIntersectPts(pFtr1, ftrs[k], arr);
				for(int n=0; n<arr.GetSize(); n++)
				{
					PT_3DEX pt1(arr[n].pt,penLine);
					if(!IsPtInCurve(pt1, pts))
						continue;
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->AddAssociatedFtr(ftrs[k]);
					m_pChkResMgr->SetAssociatedPos(arr[n].pt);
					m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_DES_CURVEARCOSSSURFACE));
					m_pChkResMgr->EndResItem();
				}
			}
			delete pFtr1;
		}
	}
	GProgressEnd();
	
	pDS->RestoreAllQueryFlags();
	
	CLEAR_MARK_SELECTION;
}

//////////////////////////////////////////////////////////////////////////
CContourRiverIntersectChkCmd::CContourRiverIntersectChkCmd()
{
	m_strContourCode = _T("");
	m_strRiverCode = _T("");
}

CContourRiverIntersectChkCmd::~CContourRiverIntersectChkCmd()
{

}

CString CContourRiverIntersectChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_CONTOUR);
}

CString CContourRiverIntersectChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_DES_CONTOUR_RIVER_INTERSECT);
}

void CContourRiverIntersectChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strContourCode;
	tab.AddValue(CHK_CONTOUR_CODE,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strRiverCode;
	tab.AddValue(CHK_RIVER_CODE,&CVariantEx(var));
}

void CContourRiverIntersectChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,CHK_CONTOUR_CODE,var) )
	{
		m_strContourCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,CHK_RIVER_CODE,var) )
	{
		m_strRiverCode= (const char*)(_bstr_t)(_variant_t)*var;
	}	
}

void CContourRiverIntersectChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CContourRiverIntersect"),_T("CContourRiverIntersect"));

	param->AddLayerNameParamEx(CHK_CONTOUR_CODE,(LPCTSTR)m_strContourCode,StrFromResID(IDS_CONTOUR_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
	param->AddLayerNameParamEx(CHK_RIVER_CODE,(LPCTSTR)m_strRiverCode,StrFromResID(IDS_LINK_RIVERLAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
}

void CContourRiverIntersectChkCmd::Execute(BOOL bForSelection)
{
	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS) return;

	if( m_strRiverCode.IsEmpty() || m_strContourCode.IsEmpty())
		return;
	
	CFtrLayerArray Riverlayers, ContourLayers;
	pDS->GetFtrLayersByNameOrCode(m_strRiverCode,Riverlayers);
	pDS->GetFtrLayersByNameOrCode(m_strContourCode, ContourLayers);

	int i=0;
	//只让目标图层可以查询
	pDS->SaveAllQueryFlags(TRUE,FALSE);
	for( i=0; i<Riverlayers.GetSize(); i++)
	{
		Riverlayers[i]->SetAllowQuery(TRUE);
	}

	int nSum = 0;
	for(i=0; i<ContourLayers.GetSize(); i++)
	{
		nSum += ContourLayers[i]->GetObjectCount();
	}

	CDataQueryEx *pDQ = m_pDoc->GetDataQuery();

	MARK_SELECTION;
	GProgressStart(nSum);
	for(i=0; i<ContourLayers.GetSize(); i++)
	{
		CFtrLayer* pLayer = ContourLayers[i];
		if(!pLayer) continue;

		int nObj = pLayer->GetObjectCount();
		for(int j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature* pFtr = pLayer->GetObject(j);
			if(!pFtr) continue;

			SKIP_NOT_SELECTION;
			CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
			const CShapeLine *pShape = pFtr->GetGeometry()->GetShape();
			pShape->GetPts(arrPts1);
			
			int npt1 = GraphAPI::GKickoffSame2DPoints(arrPts1.GetData(),arrPts1.GetSize());
			arrPts1.SetSize(npt1);

			Envelope e;
			e.CreateFromPts(arrPts1.GetData(), npt1, sizeof(PT_3DEX));

			pDQ->FindObjectInRect(e, NULL, FALSE, FALSE);

			int nNum;
			const CPFeature *ftrs = pDQ->GetFoundHandles(nNum);
			for (int k=0; k<nNum; k++)
			{
				arrPts2.RemoveAll();
				const CShapeLine *pShape = ftrs[k]->GetGeometry()->GetShape();
				pShape->GetPts(arrPts2);
				int npt2 = GraphAPI::GKickoffSame2DPoints(arrPts2.GetData(),arrPts2.GetSize());
				arrPts2.SetSize(npt2);

				CArray<PT_3DEX, PT_3DEX> retPts;
				if(IsCurveIntersect(arrPts1, arrPts2, retPts))
				{
					if(retPts.GetSize()>1)
					{
						for(int n=0; n<retPts.GetSize(); n++)
						{
							m_pChkResMgr->BeginResItem();
							m_pChkResMgr->SetAssociatedPos(retPts[n]);
							m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_DES_CONTOUR_RIVER_ERROR));
							m_pChkResMgr->EndResItem();
						}
					}
				}
			}
		}
	}
	GProgressEnd();

	pDS->RestoreAllQueryFlags();

	CLEAR_MARK_SELECTION;
}

void CContourRiverIntersectChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_CHK_DES_CONTOUR_RIVER_ERROR));
}

BOOL CContourRiverIntersectChkCmd::IsCurveIntersect( CArray<PT_3DEX,PT_3DEX>& pts1, CArray<PT_3DEX,PT_3DEX>& pts2, CArray<PT_3DEX,PT_3DEX>& retpts)
{
	int n1 = pts1.GetSize();
	int n2 = pts2.GetSize();
	if( n1 < 2 || n2 < 2 )
		return FALSE;
	
	PT_3DEX* pts = new PT_3DEX[ n2 ];
	if( !pts )
		return FALSE;

	for(int i = 0; i < n1 -1; i++)
	{
		PT_3DEX pt1 = pts1.GetAt(i);
		PT_3DEX pt2 = pts1.GetAt(i+1);
		int retnum = 0;
		GraphAPI::GGetPointsOfIntersect(pts2.GetData(), n2, pt1, pt2, pts, &retnum);

		for(int j = 0; j < retnum; j++)
		{
			PT_3DEX pt = pts[j];
			retpts.Add( pt );
		}
	}
	
	delete [] pts;

	return (retpts.GetSize()>0);
}


//////////////////////////////////////////////////////////////////////////
CElevPtNumChkCmd::CElevPtNumChkCmd()
{
	m_lfDistance = 10;
	m_nLimitPt = 1;
}

CElevPtNumChkCmd::~CElevPtNumChkCmd()
{

}

CString CElevPtNumChkCmd::GetCheckCategory()const
{
	return StrFromResID(IDS_CHK_CATEGORY_CONTOUR);
}

CString CElevPtNumChkCmd::GetCheckName()const
{
	return StrFromResID(IDS_CHK_CMD_ELEVPTNUM_ERR);
}

void CElevPtNumChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strTKCode;
	tab.AddValue("TKcode",&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strElevPtCode;
	tab.AddValue("ElevPtCode",&CVariantEx(var));
	var = m_lfDistance;
	tab.AddValue("distance",&CVariantEx(var));
	var = (long)m_nLimitPt;
	tab.AddValue("NumLimit",&CVariantEx(var));
}

void CElevPtNumChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"TKcode",var) )
	{
		m_strTKCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"ElevPtCode",var) )
	{
		m_strElevPtCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"distance",var) )
	{
		m_lfDistance = (_variant_t)*var;
	}
	if( tab.GetValue(0,"NumLimit",var) )
	{
		m_nLimitPt = (long)(_variant_t)*var;
	}
}

void CElevPtNumChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CElevPtNumChkCmd"),_T("CElevPtNumChkCmd"));

	param->AddLayerNameParam("TKcode",(LPCTSTR)m_strTKCode,StrFromResID(IDS_TK_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParamEx("ElevPtCode",(LPCTSTR)m_strElevPtCode,StrFromResID(IDS_ELEVPT_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY_POINT);
	param->AddParam("distance",m_lfDistance,StrFromResID(IDS_Grid_DISTANCE));
	param->AddParam("NumLimit",m_nLimitPt,StrFromResID(IDS_ELEVPTNUM_LIMIT));
}

void CElevPtNumChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_ELEVPTNUM_MORE));
	reasons.Add(StrFromResID(IDS_ELEVPTNUM_LESS));
}

void CElevPtNumChkCmd::Execute(BOOL bForSelection)
{
	BOOL bCheckCode = (m_strElevPtCode.GetLength()>0);

	CDlgDataSource * pDS = m_pDoc->GetDlgDataSource();
	if(!pDS)return;

	double dis = m_pDoc->GetDlgDataSource()->GetScale()*0.01*m_lfDistance;
	if(dis<0) return;

	vector<CFeature*> container;
	CStringArray arr;
	if(!convertStringToStrArray(m_strElevPtCode,arr))
		return ;
	for(int a=0;a<arr.GetSize();a++)
	{
		CPtrArray arrLayers;
		if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
			continue;
		int nTar = arrLayers.GetSize();	
		for (int i=0;i<nTar;i++)
		{
			CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
				{
					continue;
				}
				container.push_back(pFtr);
			}
		}
	}

	//读取图廓
	vector<Envelope> evlps;
	Envelope evlp;
	if(m_strTKCode.IsEmpty())
	{
		PT_3D pt3ds[4];
		memset(pt3ds,0,sizeof(pt3ds));
		pDS->GetBound(pt3ds,NULL,NULL);
		evlp.CreateFromPts(pt3ds,4);
		evlps.push_back(evlp);
	}
	else
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer(m_strTKCode);
		if(!pLayer || !pLayer->IsVisible())
		{
			PT_3D pt3ds[4];
			memset(pt3ds,0,sizeof(pt3ds));
			pDS->GetBound(pt3ds,NULL,NULL);
			evlp.CreateFromPts(pt3ds,4);
			evlps.push_back(evlp);
		}
		else
		{
			int nObj = pLayer->GetObjectCount();
			for(int i=0; i<nObj; i++)
			{
				CFeature *pFtr = pLayer->GetObject(i);
				if(!pFtr) continue;
				
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pFtr->GetGeometry()->GetShape(arrPts);
				int npt = arrPts.GetSize();
				if(npt < 5) continue;
				evlp.CreateFromPts(arrPts.GetData(), npt, sizeof(PT_3DEX));
				evlps.push_back(evlp);
			}
		}
	}

	//显示格网
	vector<Envelope>::iterator itevlp = evlps.begin();
	evlp = *itevlp;
	for(++itevlp; itevlp!=evlps.end(); ++itevlp)
	{
		Envelope e = *itevlp;
		evlp.Union(&e);
	}
	WriteProfileDouble(REGPATH_OVERLAYGRID,"Width",m_lfDistance);
	WriteProfileDouble(REGPATH_OVERLAYGRID,"Height",m_lfDistance);
	WriteProfileDouble(REGPATH_OVERLAYGRID,"MinX",evlp.m_xl);
	WriteProfileDouble(REGPATH_OVERLAYGRID,"MinY",evlp.m_yl);
	WriteProfileDouble(REGPATH_OVERLAYGRID,"XRange",evlp.m_xh-evlp.m_xl);
	WriteProfileDouble(REGPATH_OVERLAYGRID,"YRange",evlp.m_yh-evlp.m_yl);
	m_pDoc->m_bOverlayGrid = TRUE;
	m_pDoc->UpdateAllViews(NULL,hc_UpdateGrid,NULL);


	MARK_SELECTION;

	m_pChkResMgr->Clear();

	double ox = evlp.m_xl;
	double oy = evlp.m_yl;
	int row = (int)ceil( (evlp.m_xh-evlp.m_xl)/dis );
	int col = (int)ceil( (evlp.m_yh-evlp.m_yl)/dis );

	int *ptSum = new int[row*col];//每个格子里点的数量
	memset(ptSum, 0, row*col*sizeof(int));

	GProgressStart(container.size());	
	vector<CFeature*>::iterator it;
	for( it=container.begin(); it!=container.end(); ++it)
	{
		GProgressStep();
		CFeature *pFtr = *it;
		SKIP_NOT_SELECTION;

		PT_3DEX pt = pFtr->GetGeometry()->GetDataPoint(0);

		int r = (int)floor( (pt.x-ox)/dis );
		if(r<0 || r>=row) continue;
		int c = (int)floor( (pt.y-oy)/dis );
		if(c<0 || c>=col) continue;

		(*(ptSum+r*col+c))++;
	}	
	GProgressEnd();

	for(int i=0; i<row; i++)
	{
		for(int j=0; j<col; j++)
		{
			//检测是否没有和任一个图廓相交
			Envelope eGrid;
			eGrid.m_xl = ox + dis*i;  eGrid.m_xh = ox + dis*i +dis;
			eGrid.m_yl = oy + dis*j;  eGrid.m_yh = oy + dis*j +dis;
			eGrid.m_zl = 0;  eGrid.m_zh = 0;
			BOOL bNoIntersect = TRUE;
			for(itevlp=evlps.begin(); itevlp!=evlps.end(); ++itevlp)
			{
				Envelope e = *itevlp;
				e.Intersect(&eGrid);
				double v = (e.m_xh-e.m_xl)*(e.m_yh-e.m_yl);
				if(e.m_xh-e.m_xl>dis/5 && e.m_yh-e.m_yl>dis/5 && v>dis*dis/10)
				{
					bNoIntersect = FALSE;
				}
			}

			if(bNoIntersect) continue;

// 			if(*(ptSum+i*col+j) > m_nLimitPt)
// 			{
// 				PT_3D pt;
// 				pt.x = ox + dis*i + dis/2;
// 				pt.y = oy + dis*j + dis/2;
// 				m_pChkResMgr->BeginResItem();
// 				m_pChkResMgr->SetReason(StrFromResID(IDS_ELEVPTNUM_MORE));
// 				m_pChkResMgr->SetAssociatedPos(pt);
// 				m_pChkResMgr->EndResItem();
// 			}
			if(*(ptSum+i*col+j) < m_nLimitPt)
			{
				PT_3D pt;
				pt.x = ox + dis*i + dis/2;
				pt.y = oy + dis*j + dis/2;
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->SetReason(StrFromResID(IDS_ELEVPTNUM_LESS));
				m_pChkResMgr->SetAssociatedPos(pt);
				m_pChkResMgr->EndResItem();
			}
		}
	}

	delete ptSum;

	CLEAR_MARK_SELECTION;
}


////////////////////////////////////////////////////////////
CLayerChkCmd::CLayerChkCmd()
{
	m_strLayerCodes = _T("");
	m_strField = _T("CODE");
}
CLayerChkCmd::~CLayerChkCmd()
{
	
}
CString CLayerChkCmd::GetCheckCategory() const
{
    return StrFromResID(IDS_CHK_CATEGORY_BASIC);
}

CString CLayerChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_CODE_CORRECTNESS);
}

void CLayerChkCmd::GetParams( CValueTable& tab ) const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strLayerCodes;
	tab.AddValue("Layers",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strField;
	tab.AddValue("field",&CVariantEx(var));
}

void CLayerChkCmd::SetParams( CValueTable& tab )
{
	const CVariantEx *var;
	if (tab.GetValue(0, "Layers", var))
	{
		m_strLayerCodes = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0, "field", var))
	{
		m_strField = (const char*)(_bstr_t)(_variant_t)*var;
	}
}

void CLayerChkCmd::FillShowParams( CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(_T("CLayerChkCmd"),_T("CLayerChkCmd"));
	param->AddLayerNameParamEx("Layers",m_strLayerCodes,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS));
	param->AddParam("field",m_strField,StrFromResID(IDS_CMDPLANE_FIELDNAME));
}

void CLayerChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	if(m_strField.IsEmpty()) return;
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if (!pDS) return;
	CAttributesSource* pXDS = pDS->GetXAttributesSource();
	if(!pXDS) return;
	CConfigLibManager *pConfigMag = gpCfgLibMan;
	if(!pConfigMag) return;
	CScheme *pScheme = pConfigMag->GetScheme(pDS->GetScale());
	if(!pScheme) return;

	int nSum = 0;
	CFtrLayerArray arrLayer;
	//将参与层中的地物选出来,若参与层为空,则将所有层中的地物都挑出来。
	if (m_strLayerCodes.IsEmpty())
	{
		int nFtrLay = pDS->GetFtrLayerCount();
		
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			arrLayer.Add(pLayer);
			nSum += pLayer->GetObjectCount();
		}
	}
	else
	{
		CStringArray arr;
		if(!convertStringToStrArray(m_strLayerCodes,arr))
			return ;
		for (int a=0;a<arr.GetSize();a++)
		{
			CPtrArray arrLayers;
			if(!pDS->GetFtrLayer(arr.GetAt(a),NULL,&arrLayers))
				return;
			int nTar = arrLayers.GetSize();	
			for (int i=0;i<nTar;i++)
			{
				CFtrLayer* pLayer = (CFtrLayer*)arrLayers.GetAt(i);
				if(!pLayer||!pLayer->IsVisible())
					continue;
				arrLayer.Add(pLayer);
				nSum += pLayer->GetObjectCount();
			}
		}			
	}
	
	MARK_SELECTION;
	
	GProgressStart(nSum);
	for(int i=0; i<arrLayer.GetSize(); i++)
	{
		CFtrLayer *pLayer = arrLayer[i];
		__int64 code;
		CString ch = pLayer->GetName();
		if( !pScheme->FindLayerIdx(FALSE,code,ch) )
		{
			continue;
		}
		CString strcode1;
		strcode1.Format("%I64d", code);

		int nObj = pLayer->GetObjectCount();
		for(int j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;

			SKIP_NOT_SELECTION;

			CValueTable tab;
			tab.BeginAddValueItem();
			pXDS->GetXAttributes(pFtr,tab);
			tab.EndAddValueItem();
			
			const CVariantEx *var;
			CString strcode2;
			if( tab.GetValue(0, m_strField, var) )
			{					
				strcode2 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
				if(strcode1==strcode2) continue;
				//记录到检查结果
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
				CString strReason;
				strReason.Format(IDS_CHK_LAYER_ERROR, m_strField);
				m_pChkResMgr->SetReason(strReason);
				m_pChkResMgr->EndResItem();
			}
		}
	}

	GProgressEnd();
	
	CLEAR_MARK_SELECTION;
}

void CLayerChkCmd::GetAllReasons( CStringArray& reasons )
{	
	reasons.Add(StrFromResID(IDS_CHK_LAYER_ERROR));
}

////////////////////////////////////////////////////////////
CSlpoeChkCmd::CSlpoeChkCmd()
{
	m_strlayer1 = _T("");
	m_strlayer2 = _T("");
}
CSlpoeChkCmd::~CSlpoeChkCmd()
{
	
}
CString CSlpoeChkCmd::GetCheckCategory() const
{
    return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CSlpoeChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_CMD_XIEPO);
}

void CSlpoeChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strlayer1;
	tab.AddValue("layer1",&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strlayer2;
	tab.AddValue("layer2",&CVariantEx(var));
}

void CSlpoeChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"layer1",var) )
	{
		m_strlayer1 = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"layer2",var) )
	{
		m_strlayer2 = (const char*)(_bstr_t)(_variant_t)*var;
	}
}

void CSlpoeChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CSlpoeChkCmd"),_T("CSlpoeChkCmd"));
	
	param->AddLayerNameParamEx("layer1",(LPCTSTR)m_strlayer1,StrFromResID(IDS_XIEPO_TOP),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddLayerNameParamEx("layer2",(LPCTSTR)m_strlayer2,StrFromResID(IDS_XIEPO_BOTTOM),NULL,LAYERPARAMITEM_NOTEMPTY_LINE);
}

void CSlpoeChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if (!pDS) return;
	if(m_strlayer1.IsEmpty() || m_strlayer2.IsEmpty())
		return;

	CFtrLayerArray arr, arr2;
	pDS->GetFtrLayersByNameOrCode_editable(m_strlayer1, arr);
	pDS->GetFtrLayersByNameOrCode_editable(m_strlayer2, arr2);

	int i, j, k, m, pos;
	for (i=0; i<arr.GetSize(); i++)
	{
		int nObj = arr[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = arr[i]->GetObject(j);
			if(pFtr && pFtr->IsVisible())
				pFtr->SetAppFlag(0);
		}
	}
	for (i=0; i<arr2.GetSize(); i++)
	{
		int nObj = arr2[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = arr2[i]->GetObject(j);
			if(pFtr && pFtr->IsVisible())
				pFtr->SetAppFlag(0);
		}
	}

	MARK_SELECTION;

	for (i=0; i<arr.GetSize(); i++)
	{
		CString LayerName = arr[i]->GetName();
		pos = LayerName.ReverseFind('_');
		if(pos<0) continue;
		LayerName = LayerName.Left(pos);

		int nObj = arr[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = arr[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;

			SKIP_NOT_SELECTION;

			for (k=0; k<arr2.GetSize(); k++)
			{
				CString LayerName2 = arr2[k]->GetName();
				pos = LayerName2.ReverseFind('_');
				if(pos<0) continue;
				LayerName2 = LayerName2.Left(pos);
				if(LayerName!=LayerName2) continue;

				int nObj2 = arr2[k]->GetObjectCount();
				for(m=0; m<nObj2; m++)
				{
					CFeature *pFtr2 = arr2[k]->GetObject(m);
					if(!pFtr2 || !pFtr2->IsVisible())
						continue;
					if(pFtr2->GetAppFlag()==2) continue;

					if(CheckLoopAndDir(pFtr, pFtr2))
					{
						pFtr->SetAppFlag(2);
						pFtr2->SetAppFlag(2);
					}
				}
			}
		}
	}

	for (i=0; i<arr.GetSize(); i++)
	{
		int nObj = arr[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = arr[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;
			if(pFtr->GetAppFlag()==0)
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_NOT_CLOSED));
				m_pChkResMgr->EndResItem();
			}
			else if(pFtr->GetAppFlag()==1)
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				int ptSum = pFtr->GetGeometry()->GetDataPointSum();
				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(ptSum-1));//末端点
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_NOT_CLOSED));
				m_pChkResMgr->EndResItem();
				pFtr->SetAppFlag(0);
			}
			else
			{
				pFtr->SetAppFlag(0);
			}
		}
	}
	for (i=0; i<arr2.GetSize(); i++)
	{
		int nObj = arr2[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = arr2[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;
			if(pFtr->GetAppFlag()==0)
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(0));
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_NOT_CLOSED));
				m_pChkResMgr->EndResItem();
			}
			else if(pFtr->GetAppFlag()==1)
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				int ptSum = pFtr->GetGeometry()->GetDataPointSum();
				m_pChkResMgr->SetAssociatedPos(pFtr->GetGeometry()->GetDataPoint(ptSum-1));//末端点
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_NOT_CLOSED));
				m_pChkResMgr->EndResItem();
				pFtr->SetAppFlag(0);
			}
			else
			{
				pFtr->SetAppFlag(0);
			}
		}
	}

	CLEAR_MARK_SELECTION;
}

//构成回路返回TRUE
BOOL CSlpoeChkCmd::CheckLoopAndDir(CFeature *pFtr, CFeature *pFtr2)
{
	if(!pFtr || !pFtr2) return FALSE;

	CGeometry *pObj = pFtr->GetGeometry();
	if(!pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
		return FALSE;
	CGeometry *pObj2 = pFtr2->GetGeometry();
	if(!pObj2 || !pObj2->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
		return FALSE;

	Envelope e = pObj->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	if(!e.bIntersect(&e2)) return FALSE;

	CArray<PT_3DEX,PT_3DEX> pts, pts2;
	pObj->GetShape(pts);
	if(pts.GetSize()<2) return FALSE;
	pObj2->GetShape(pts2);
	if(pts2.GetSize()<2) return FALSE;

	PT_3DEX pt1 = pts[0];
	PT_3DEX pt2 = pts[pts.GetSize()-1];
	PT_3DEX pt3 = pts2[0];
	PT_3DEX pt4 = pts2[pts2.GetSize()-1];

	BOOL bret1 = GraphAPI::GIsEqual2DPoint(&pt1, &pt3);
	BOOL bret2 = GraphAPI::GIsEqual2DPoint(&pt2, &pt4);

	if( bret1 && bret2 )
	{
		//闭合但是方向不正确
		m_pChkResMgr->BeginResItem();
		m_pChkResMgr->AddAssociatedFtr(pFtr);
		m_pChkResMgr->AddAssociatedFtr(pFtr2);
		m_pChkResMgr->SetAssociatedPos(pt1);
		m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_XIEPO_DIR_ERR));
		m_pChkResMgr->EndResItem();
		return TRUE;
	}

	BOOL bret3 = GraphAPI::GIsEqual2DPoint(&pt1, &pt4);
	BOOL bret4 = GraphAPI::GIsEqual2DPoint(&pt2, &pt3);

	if(bret3 && bret4)
	{
		CArray<PT_3DEX,PT_3DEX> allPts;
		allPts.Copy(pts);
		pts2.RemoveAt(0);
		allPts.Append(pts2);
		int ret = GraphAPI::GIsClockwise(allPts.GetData(), allPts.GetSize());
		if(1==ret)
		{
			return TRUE;
		}
		else if(0==ret)
		{
			//闭合但是方向不正确
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->AddAssociatedFtr(pFtr2);
			m_pChkResMgr->SetAssociatedPos(pt1);
			m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_DES_ANTICLOCKWISE));
			m_pChkResMgr->EndResItem();
			return TRUE;
		}
	}

	if(bret1||bret3)
	{
		pFtr->SetAppFlag(1);//pFtr的首端点闭合
	}

	if(bret1||bret4)
	{
		pFtr2->SetAppFlag(1);//pFtr2的首端点闭合
	}
	
	return FALSE;
}

void CSlpoeChkCmd::GetAllReasons( CStringArray& reasons )
{	
	reasons.Add(StrFromResID(IDS_CHK_XIEPO_DIR_ERR));
	reasons.Add(StrFromResID(IDS_CHK_CMD_DES_ANTICLOCKWISE));
	reasons.Add(StrFromResID(IDS_CHK_NOT_CLOSED));
}

////////////////////////////////////////////////////////////
CMapMatchChkCmd::CMapMatchChkCmd()
{
	m_strTKLayerCode = _T("");
	m_lfToler = 1e-4;
	m_bMatchColor = FALSE;
	m_bMatchAttr = FALSE;
}
CMapMatchChkCmd::~CMapMatchChkCmd()
{
	
}
CString CMapMatchChkCmd::GetCheckCategory() const
{
    return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CMapMatchChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_CMD_MAPMATCH);
}

void CMapMatchChkCmd::GetParams( CValueTable& tab ) const
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strTKLayerCode;
	tab.AddValue("TKLayer",&CVariantEx(var));
	var = m_lfToler;
	tab.AddValue("Toler",&CVariantEx(var));
	var = (bool)(m_bMatchColor);
	tab.AddValue("MatchColor",&CVariantEx(var));
	var = (bool)(m_bMatchAttr);
	tab.AddValue("MatchAttr",&CVariantEx(var));
}

void CMapMatchChkCmd::SetParams( CValueTable& tab )
{
	const CVariantEx *var;
	if (tab.GetValue(0, "TKLayer", var))
	{
		m_strTKLayerCode = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0, "Toler", var))
	{
		m_lfToler = (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,"MatchColor",var) )
	{
		m_bMatchColor = (bool)(_variant_t)*var;		
	}
	if( tab.GetValue(0,"MatchAttr",var) )
	{
		m_bMatchAttr = (bool)(_variant_t)*var;
	}
}

void CMapMatchChkCmd::FillShowParams( CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(_T("CMapMatchChkCmd"),_T("CMapMatchChkCmd"));
	param->AddLayerNameParam("TKLayer",m_strTKLayerCode,StrFromResID(IDS_TK_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
	param->AddParam("Toler",m_lfToler,StrFromResID(IDS_CMDPLANE_TOLER));

	param->BeginOptionParam("MatchColor",StrFromResID(IDS_CMDPLANE_MM_MATCHCOLOR));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bMatchColor);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bMatchColor);
	param->EndOptionParam();

	param->BeginOptionParam("MatchAttr",StrFromResID(IDS_CMDPLANE_MATCHATTR));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bMatchAttr);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bMatchAttr);
	param->EndOptionParam();
}

//获取图廓框的公共线段
static BOOL FindSameSeg(CArray<PT_3DEX,PT_3DEX>& pts1, CArray<PT_3DEX,PT_3DEX>& pts2, PT_3DEX *pt1, PT_3DEX *pt2)
{
	for(int i=0; i<pts1.GetSize()-1; i++)
	{
		for(int j=0; j<pts2.GetSize()-1; j++)
		{
			if( GraphAPI::GIsEqual2DPoint(&pts1[i], &pts2[j]) && GraphAPI::GIsEqual2DPoint(&pts1[i+1], &pts2[j+1]))
			{
				*pt1 = pts1[i];
				*pt2 = pts1[i+1];
				return TRUE;
			}
			if( GraphAPI::GIsEqual2DPoint(&pts1[i], &pts2[j+1]) && GraphAPI::GIsEqual2DPoint(&pts1[i+1], &pts2[j]))
			{
				*pt1 = pts1[i];
				*pt2 = pts1[i+1];
				return TRUE;
			}
		}
	}

	return FALSE;
}

extern CString VarToString(_variant_t& var);
static BOOL bTabSame(CValueTable& tab1, CValueTable& tab2)
{
	int n1 = tab1.GetFieldCount();
	int n2 = tab2.GetFieldCount();
	if(n1!=n2) return FALSE;

	const CVariantEx *var1, *var2;
	for(int i=0; i<n1; i++)
	{
		tab1.GetValue(0, i, var1);
		tab2.GetValue(0, i, var2);
		CString val1 = VarToString((_variant_t)*var1);
		CString val2 = VarToString((_variant_t)*var2);
		if(val1==val2) continue;

		return FALSE;
	}
	return TRUE;
}

void CMapMatchChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	if(!m_pDoc) return;
	int nDS = m_pDoc->GetDlgDataSourceCount();
	if(nDS==1)
		ExecuteSingleDS(bForSelection);
	else
		ExecuteMutiDS(bForSelection);
}

void CMapMatchChkCmd::GetAllReasons( CStringArray& reasons )
{
	reasons.Add(StrFromResID(IDS_CHK_ERR_LAYERDIFF));
	reasons.Add(StrFromResID(IDS_CHK_ERR_ATTRDIFF));
	reasons.Add(StrFromResID(IDS_CHK_ERR_COLORDIFF));
	reasons.Add(StrFromResID(IDS_CHK_ERR_NOLINK));
}

//返回矩形外包和线段(水平或垂直)的位置关系
static int GetPositionEnvlopeSeg(Envelope& e, PT_3DEX& pt1, PT_3DEX& pt2)
{
	if( fabs(pt1.x-pt2.x) > fabs(pt1.y-pt2.y) )//水平
	{
		if(e.CenterY()>pt1.y)
		{
			return 2;
		}
		else
		{
			return -2;
		}
	}
	else //垂直
	{
		if(e.CenterX()>pt1.x)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	return 0;
}

void CMapMatchChkCmd::ExecuteSingleDS( BOOL bForSelection/*=FALSE*/ )
{
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if (!pDS) return;
	CAttributesSource *pXDS = pDS->GetXAttributesSource();
	if(!pXDS) return;

	CFtrLayer *pTKLayer = pDS->GetFtrLayer(m_strTKLayerCode);
	if(!pTKLayer) return;

	int i, j, k;
	//取出图廓层的公共线段
	CArray<PT_3DEX,PT_3DEX> arrTKPts, segs;
	{
		int nObj = pTKLayer->GetObjectCount();
		if(nObj==1) return;
		for(i=0; i<nObj; i++)
		{
			CFeature *pFtr = pTKLayer->GetObject(i);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			CArray<PT_3DEX,PT_3DEX> pts;
			pFtr->GetGeometry()->GetShape(pts);
			for(j=i+1; j<nObj; j++)
			{
				CFeature *pFtr1 = pTKLayer->GetObject(j);
				if (!pFtr1||!pFtr1->IsVisible())
					continue;
				CArray<PT_3DEX,PT_3DEX> pts1;
				pFtr1->GetGeometry()->GetShape(pts1);

				PT_3DEX ret1, ret2;
				if(FindSameSeg(pts, pts1, &ret1, &ret2))
				{
					segs.Add(ret1);
					segs.Add(ret2);
				}
			}
		}
	}

	CArray<OBJ_ITEM1,OBJ_ITEM1> items;
	CArray<OBJ_ITEM2,OBJ_ITEM2> items1;

	int nFtrLay = pDS->GetFtrLayerCount();
	for (i=0;i<nFtrLay;i++)
	{
		CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;

		if(pTKLayer == pLayer) continue;

		int nObj = pLayer->GetObjectCount();
		for (j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				continue;

			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);
			int nPt = pts.GetSize();
			if(nPt<2) continue;

			Envelope e = pGeo->GetEnvelope();

			double dis;
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			{
				for(int m=0; m<nPt-1; m++)
				{
					PT_3DEX pt1 = pts[m+1];
					if(pt1.pencode==penMove) break;
					for(k=0; k<segs.GetSize(); k+=2)
					{
						dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pts[m].x, pts[m].y,NULL,NULL,FALSE);
						if(dis<m_lfToler)
						{
							dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pt1.x, pt1.y,NULL,NULL,FALSE);
							if(dis<m_lfToler)
							{
								OBJ_ITEM2 item;
								item.pLayer = pLayer;
								item.pFtr = pFtr;
								item.pt = pts[m];
								item.pt1 = pt1;
								item.segid = k;
								item.position = GetPositionEnvlopeSeg(e, segs[k],segs[k+1]);
								items1.Add(item);
								break;
							}
						}
					}
				}
			}
			else if( ((CGeoCurveBase*)pGeo)->IsClosed() )
			{
// 				for(int m=0; m<nPt-1; m++)
// 				{
// 					for(k=0; k<segs.GetSize(); k+=2)
// 					{
// 						dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pts[m].x, pts[m].y,NULL,NULL,FALSE);
// 						if(dis<m_lfToler)
// 						{
// 							OBJ_ITEM1 item;
// 							item.pLayer = pLayer;
// 							item.pFtr = pFtr;
// 							item.pt = pts[m];
// 							item.segid = k;
// 							item.position = GetPositionEnvlopeSeg(e, segs[k],segs[k+1]);
// 							items.Add(item);
// 							break;
// 						}
// 					}
// 				}
			}
			else
			{
				//线段起点
				for(k=0; k<segs.GetSize(); k+=2)
				{
					dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pts[0].x, pts[0].y,NULL,NULL,FALSE);
					if(dis<m_lfToler)
					{
						OBJ_ITEM1 item;
						item.pLayer = pLayer;
						item.pFtr = pFtr;
						item.pt = pts[0];
						item.segid = k;
						item.position = GetPositionEnvlopeSeg(e, segs[k],segs[k+1]);
						items.Add(item);
						break;
					}
				}
				//线段终点
				for(k=0; k<segs.GetSize(); k+=2)
				{
					dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pts[nPt-1].x, pts[nPt-1].y,NULL,NULL,FALSE);
					if(dis<m_lfToler)
					{
						OBJ_ITEM1 item;
						item.pLayer = pLayer;
						item.pFtr = pFtr;
						item.pt = pts[nPt-1];
						item.segid = k;
						item.position = GetPositionEnvlopeSeg(e, segs[k],segs[k+1]);
						items.Add(item);
						break;
					}
				}
			}
		}
	}
	
	MARK_SELECTION;

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	
	/*线的接边检查*/
	GProgressStart(items.GetSize()+items1.GetSize());
	for(i=0; i<items.GetSize(); i++)
	{
		GProgressStep();
		CFeature *pFtr = items[i].pFtr;
		if(pFtr==NULL) continue;
		SKIP_NOT_SELECTION;

		PT_3DEX p0 = items[i].pt;

		for (j=i+1; j<items.GetSize(); j++)
		{
			if(items[j].pFtr==NULL) continue;
			if( items[i].segid!=items[j].segid ) continue;
			if( 0!=items[i].position+items[j].position) continue;

			PT_3DEX p1 = items[j].pt;

			double dis = sqrt( (p0.x-p1.x)*(p0.x-p1.x)+(p0.y-p1.y)*(p0.y-p1.y) );
			if( dis>m_lfToler ) continue;
			//已找到配对的地物
			CFeature *pFtr1 = items[j].pFtr;
			items[i].pFtr = NULL;
			items[j].pFtr = NULL;
			//图层是否一致
			if( items[i].pLayer!=items[j].pLayer )
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->AddAssociatedFtr(pFtr1);
				m_pChkResMgr->SetAssociatedPos(items[i].pt);
				CString strReason = StrFromResID(IDS_LINE);
				strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_LAYERDIFF);
				m_pChkResMgr->SetReason(strReason);
				m_pChkResMgr->EndResItem();
				break;
			}

			//比较颜色是否一致
			if(m_bMatchColor)
			{
				COLORREF color1 = pFtr->GetGeometry()->GetColor();
				if(color1==-1) color1 = items[i].pLayer->GetColor();
				COLORREF color2 = pFtr1->GetGeometry()->GetColor();
				if(color2==-1) color2 = items[j].pLayer->GetColor();
				if(color1!=color2)
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->AddAssociatedFtr(pFtr1);
					m_pChkResMgr->SetAssociatedPos(items[i].pt);
					CString strReason = StrFromResID(IDS_LINE);
					strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_COLORDIFF);
					m_pChkResMgr->SetReason(strReason);
					m_pChkResMgr->EndResItem();
					break;
				}
			}
			//比较扩展属性是否一致
			if(m_bMatchAttr)
			{
				CValueTable tab1, tab2;
				tab1.BeginAddValueItem();
				pXDS->GetXAttributes(pFtr, tab1);
				tab1.EndAddValueItem();
				tab2.BeginAddValueItem();
				pXDS->GetXAttributes(pFtr1, tab2);
				tab2.EndAddValueItem();
				if(pScheme)
				{
					CSchemeLayerDefine *pdef = pScheme->GetLayerDefine(items[i].pLayer->GetName());
					if(pdef)
					{
						int size=0;
						const XDefine *defs = pdef->GetXDefines(size);
						for(int n=0; n<size; n++)
						{
							if(defs[n].isMust<0)
							{
								tab1.DelField(defs[n].field);
								tab2.DelField(defs[n].field);
							}
						}
					}
				}
				if(!bTabSame(tab1, tab2))
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->AddAssociatedFtr(pFtr1);
					m_pChkResMgr->SetAssociatedPos(items[i].pt);
					CString strReason = StrFromResID(IDS_LINE);
					strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_ATTRDIFF);
					m_pChkResMgr->SetReason(strReason);
					m_pChkResMgr->EndResItem();
				}
			}
		}
	}

	for(i=0; i<items.GetSize(); i++)
	{
		if( items[i].pFtr )
		{
			//记录到检查结果
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(items[i].pFtr);
			m_pChkResMgr->SetAssociatedPos(items[i].pt);
			CString strReason = StrFromResID(IDS_LINE);
				strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_NOLINK);
			m_pChkResMgr->SetReason(strReason);
			m_pChkResMgr->EndResItem();
		}
	}

	/*面的接边检查*/
	for(i=0; i<items1.GetSize(); i++)
	{
		GProgressStep();
		CFeature *pFtr = items1[i].pFtr;
		if(pFtr==NULL) continue;
		SKIP_NOT_SELECTION;
		
		PT_3DEX p0 = items1[i].pt;
		PT_3DEX p1 = items1[i].pt1;
		
		for (j=i+1; j<items1.GetSize(); j++)
		{
			if(items1[j].pFtr==NULL) continue;
			if( items1[i].segid!=items1[j].segid ) continue;
			if( 0!=items1[i].position+items1[j].position) continue;
			
			PT_3DEX p2 = items1[j].pt;
			PT_3DEX p3 = items1[j].pt1;
			
			double dis = sqrt( (p0.x-p2.x)*(p0.x-p2.x)+(p0.y-p2.y)*(p0.y-p2.y) );
			double dis1 = sqrt( (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y) );
			if( dis<m_lfToler )
			{
				dis1 = sqrt( (p1.x-p3.x)*(p1.x-p3.x)+(p1.y-p3.y)*(p1.y-p3.y) );
				if(dis1>m_lfToler)
				{
					continue;
				}
			}
			else if( dis1<m_lfToler )
			{
				dis = sqrt( (p0.x-p3.x)*(p0.x-p3.x)+(p0.y-p3.y)*(p0.y-p3.y) );
				if(dis>m_lfToler)
				{
					continue;
				}
			}
			else
			{
				continue;
			}
			//已找到配对的地物
			CFeature *pFtr1 = items1[j].pFtr;
			items1[i].pFtr = NULL;
			items1[j].pFtr = NULL;
			PT_3D cen( (items1[i].pt.x+items1[i].pt1.x)/2, (items1[i].pt.y+items1[i].pt1.y)/2, 0);
			//图层是否一致
			if( items1[i].pLayer!=items1[j].pLayer )
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->AddAssociatedFtr(pFtr1);
				m_pChkResMgr->SetAssociatedPos(cen);
				CString strReason = StrFromResID(IDS_SURFACE);
				strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_LAYERDIFF);
				m_pChkResMgr->SetReason(strReason);
				m_pChkResMgr->EndResItem();
				break;
			}
			
			//比较颜色是否一致
			if(m_bMatchColor)
			{
				COLORREF color1 = pFtr->GetGeometry()->GetColor();
				if(color1==-1) color1 = items1[i].pLayer->GetColor();
				COLORREF color2 = pFtr1->GetGeometry()->GetColor();
				if(color2==-1) color2 = items1[j].pLayer->GetColor();
				if(color1!=color2)
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->AddAssociatedFtr(pFtr1);
					m_pChkResMgr->SetAssociatedPos(cen);
					CString strReason = StrFromResID(IDS_SURFACE);
					strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_COLORDIFF);
					m_pChkResMgr->SetReason(strReason);
					m_pChkResMgr->EndResItem();
					break;
				}
			}
			//比较扩展属性是否一致
			if(m_bMatchAttr)
			{
				CValueTable tab1, tab2;
				tab1.BeginAddValueItem();
				pXDS->GetXAttributes(pFtr, tab1);
				tab1.EndAddValueItem();
				tab2.BeginAddValueItem();
				pXDS->GetXAttributes(pFtr1, tab2);
				tab2.EndAddValueItem();
				if(pScheme)
				{
					CSchemeLayerDefine *pdef = pScheme->GetLayerDefine(items1[i].pLayer->GetName());
					if(pdef)
					{
						int size=0;
						const XDefine *defs = pdef->GetXDefines(size);
						for(int n=0; n<size; n++)
						{
							if(defs[n].isMust<0)
							{
								tab1.DelField(defs[n].field);
								tab2.DelField(defs[n].field);
							}
						}
					}
				}
				if(!bTabSame(tab1, tab2))
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->AddAssociatedFtr(pFtr1);
					m_pChkResMgr->SetAssociatedPos(cen);
					CString strReason = StrFromResID(IDS_SURFACE);
					strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_ATTRDIFF);
					m_pChkResMgr->SetReason(strReason);
					m_pChkResMgr->EndResItem();	
				}
			}
		}
	}
	
	for(i=0; i<items1.GetSize(); i++)
	{
		if( items1[i].pFtr )
		{
			//记录到检查结果
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(items1[i].pFtr);
			PT_3D cen( (items1[i].pt.x+items1[i].pt1.x)/2, (items1[i].pt.y+items1[i].pt1.y)/2, 0);
			m_pChkResMgr->SetAssociatedPos(cen);
			CString strReason = StrFromResID(IDS_SURFACE);
			strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_NOLINK);
			m_pChkResMgr->SetReason(strReason);
			m_pChkResMgr->EndResItem();
		}
	}

	GProgressEnd();
	
	CLEAR_MARK_SELECTION;
}

void CMapMatchChkCmd::ExecuteMutiDS( BOOL bForSelection/*=FALSE*/ )
{
	int idx = m_pDoc->GetActiveDataSourceIdx();
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource(idx);
	int nDS = m_pDoc->GetDlgDataSourceCount();
	if (!pDS) return;

	CFtrLayer *pTKLayer = pDS->GetFtrLayer(m_strTKLayerCode);
	if(!pTKLayer) return;

	int i, j, k;
	//取出图廓层的公共线段
	CArray<PT_3DEX,PT_3DEX> arrTKPts, segs;
	{
		int nObj = pTKLayer->GetObjectCount();

		//和其他参考数据源比较
		for(i=0; i<nObj; i++)
		{
			CFeature *pFtr = pTKLayer->GetObject(i);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			CArray<PT_3DEX,PT_3DEX> pts;
			pFtr->GetGeometry()->GetShape(pts);
			for(j=0; j<nDS; j++)
			{
				CDlgDataSource * pDS1 = m_pDoc->GetDlgDataSource(j);
				if(pDS==pDS1) continue;
				CFtrLayer *pTKLayer1 = pDS1->GetFtrLayer(m_strTKLayerCode);
				if(!pTKLayer1) return;
				int nObj1 = pTKLayer1->GetObjectCount();
				for(k=0; k<nObj1; k++)
				{
					CFeature *pFtr1 = pTKLayer1->GetObject(j);
					if (!pFtr1||!pFtr1->IsVisible())
						continue;
					CArray<PT_3DEX,PT_3DEX> pts1;
					pFtr1->GetGeometry()->GetShape(pts1);
					
					PT_3DEX ret1, ret2;
					if(FindSameSeg(pts, pts1, &ret1, &ret2))
					{
						segs.Add(ret1);
						segs.Add(ret2);
					}
				}
			}
		}

		//当前数据源进行比较
		if(nObj==1) return;
		for(i=0; i<nObj; i++)
		{
			CFeature *pFtr = pTKLayer->GetObject(i);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			CArray<PT_3DEX,PT_3DEX> pts;
			pFtr->GetGeometry()->GetShape(pts);
			for(j=i+1; j<nObj; j++)
			{
				CFeature *pFtr1 = pTKLayer->GetObject(j);
				if (!pFtr1||!pFtr1->IsVisible())
					continue;
				CArray<PT_3DEX,PT_3DEX> pts1;
				pFtr1->GetGeometry()->GetShape(pts1);

				PT_3DEX ret1, ret2;
				if(FindSameSeg(pts, pts1, &ret1, &ret2))
				{
					segs.Add(ret1);
					segs.Add(ret2);
				}
			}
		}
	}

	CArray<OBJ_ITEM1,OBJ_ITEM1> items;
	CArray<OBJ_ITEM2,OBJ_ITEM2> items1;

	for(int n=0;n<nDS; n++)
	{
		CDlgDataSource *pDS1 = m_pDoc->GetDlgDataSource(n);
		if(!pDS1) continue;
		int nFtrLay = pDS1->GetFtrLayerCount();
		for (i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = pDS1->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;

			if(pTKLayer == pLayer) continue;

			int nObj = pLayer->GetObjectCount();
			for (j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;

				CGeometry *pGeo = pFtr->GetGeometry();
				if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
					continue;

				CArray<PT_3DEX,PT_3DEX> pts;
				pGeo->GetShape(pts);
				int nPt = pts.GetSize();
				if(nPt<2) continue;

				Envelope e = pGeo->GetEnvelope();

				double dis;
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				{
					for(int m=0; m<nPt-1; m++)
					{
						PT_3DEX pt1 = pts[m+1];
						if(pt1.pencode==penMove) break;
						for(k=0; k<segs.GetSize(); k+=2)
						{
							dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pts[m].x, pts[m].y,NULL,NULL,FALSE);
							if(dis<m_lfToler)
							{
								dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pt1.x, pt1.y,NULL,NULL,FALSE);
								if(dis<m_lfToler)
								{
									OBJ_ITEM2 item;
									item.pLayer = pLayer;
									item.pFtr = pFtr;
									item.pt = pts[m];
									item.pt1 = pt1;
									item.segid = k;
									item.pds = pDS1;
									items1.Add(item);
									break;
								}
							}
						}
					}
				}
				else if( ((CGeoCurveBase*)pGeo)->IsClosed() )
				{
// 					for(int m=0; m<nPt-1; m++)
// 					{
// 						for(k=0; k<segs.GetSize(); k+=2)
// 						{
// 							dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pts[m].x, pts[m].y,NULL,NULL,FALSE);
// 							if(dis<m_lfToler)
// 							{
// 								OBJ_ITEM1 item;
// 								item.pLayer = pLayer;
// 								item.pFtr = pFtr;
// 								item.pt = pts[m];
// 								item.segid = k;
// 								item.pds = pDS1;
// 								items.Add(item);
// 								break;
// 							}
// 						}
// 					}
				}
				else
				{
					//线段起点
					for(k=0; k<segs.GetSize(); k+=2)
					{
						dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pts[0].x, pts[0].y,NULL,NULL,FALSE);
						if(dis<m_lfToler)
						{
							OBJ_ITEM1 item;
							item.pLayer = pLayer;
							item.pFtr = pFtr;
							item.pt = pts[0];
							item.segid = k;
							item.pds = pDS1;
							items.Add(item);
							break;
						}
					}
					//线段终点
					for(k=0; k<segs.GetSize(); k+=2)
					{
						dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pts[nPt-1].x, pts[nPt-1].y,NULL,NULL,FALSE);
						if(dis<m_lfToler)
						{
							OBJ_ITEM1 item;
							item.pLayer = pLayer;
							item.pFtr = pFtr;
							item.pt = pts[nPt-1];
							item.segid = k;
							item.pds = pDS1;
							items.Add(item);
							break;
						}
					}
				}
			}
		}
	}
	
	MARK_SELECTION;

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	
	GProgressStart(items.GetSize()+items1.GetSize());
	/*线的接边检查*/
	for(i=0; i<items.GetSize(); i++)
	{
		GProgressStep();
		CFeature *pFtr = items[i].pFtr;
		if(pFtr==NULL) continue;
		if(items[i].pds!=pDS) continue;
		SKIP_NOT_SELECTION;

		PT_3DEX p0 = items[i].pt;

		for (j=i+1; j<items.GetSize(); j++)
		{
			if(items[j].pFtr==NULL) continue;
			if( items[i].segid!=items[j].segid ) continue;
			if( items[i].pds==items[j].pds ) continue;

			PT_3DEX p1 = items[j].pt;

			double dis = sqrt( (p0.x-p1.x)*(p0.x-p1.x)+(p0.y-p1.y)*(p0.y-p1.y) );
			if( dis>m_lfToler ) continue;
			//已找到配对的地物
			CFeature *pFtr1 = items[j].pFtr;
			items[i].pFtr = NULL;
			items[j].pFtr = NULL;
			//图层是否一致
			if( 0!=strcmp(items[i].pLayer->GetName(),items[j].pLayer->GetName()) )
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(items[i].pt);
				CString strReason = StrFromResID(IDS_LINE);
				strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_LAYERDIFF);
				m_pChkResMgr->SetReason(strReason);
				m_pChkResMgr->EndResItem();
				break;
			}
			
			//比较颜色是否一致
			if(m_bMatchColor)
			{
				COLORREF color1 = pFtr->GetGeometry()->GetColor();
				if(color1==-1) color1 = items[i].pLayer->GetColor();
				COLORREF color2 = pFtr1->GetGeometry()->GetColor();
				if(color2==-1) color2 = items[j].pLayer->GetColor();
				if(color1!=color2)
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->SetAssociatedPos(items[i].pt);
					CString strReason = StrFromResID(IDS_LINE);
					strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_COLORDIFF);
					m_pChkResMgr->SetReason(strReason);
					m_pChkResMgr->EndResItem();
					break;
				}
			}
			//比较扩展属性是否一致
			if(m_bMatchAttr)
			{
				CValueTable tab1, tab2;
				tab1.BeginAddValueItem();
				items[i].pds->GetXAttributesSource()->GetXAttributes(pFtr, tab1);
				tab1.EndAddValueItem();
				tab2.BeginAddValueItem();
				items[j].pds->GetXAttributesSource()->GetXAttributes(pFtr1, tab2);
				tab2.EndAddValueItem();
				if(pScheme)
				{
					CSchemeLayerDefine *pdef = pScheme->GetLayerDefine(items[i].pLayer->GetName());
					if(pdef)
					{
						int size=0;
						const XDefine *defs = pdef->GetXDefines(size);
						for(int n=0; n<size; n++)
						{
							if(defs[n].isMust<0)
							{
								tab1.DelField(defs[n].field);
								tab2.DelField(defs[n].field);
							}
						}
					}
				}
				if(!bTabSame(tab1, tab2))
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->SetAssociatedPos(items[i].pt);
					CString strReason = StrFromResID(IDS_LINE);
					strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_ATTRDIFF);
					m_pChkResMgr->SetReason(strReason);
					m_pChkResMgr->EndResItem();
				}
			}
		}
	}

	for(i=0; i<items.GetSize(); i++)
	{
		if( items[i].pFtr && items[i].pds==pDS)
		{
			//记录到检查结果
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(items[i].pFtr);
			m_pChkResMgr->SetAssociatedPos(items[i].pt);
			CString strReason = StrFromResID(IDS_LINE);
			strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_NOLINK);
			m_pChkResMgr->SetReason(strReason);
			m_pChkResMgr->EndResItem();
		}
	}
	
	/*面的接边检查*/
	for(i=0; i<items1.GetSize(); i++)
	{
		GProgressStep();
		CFeature *pFtr = items1[i].pFtr;
		if(pFtr==NULL) continue;
		if(items1[i].pds!=pDS) continue;
		SKIP_NOT_SELECTION;
		
		PT_3DEX p0 = items1[i].pt;
		PT_3DEX p1 = items1[i].pt1;
		
		for (j=i+1; j<items1.GetSize(); j++)
		{
			if(items1[j].pFtr==NULL) continue;
			if( items1[i].segid!=items1[j].segid ) continue;
			if( items1[i].pds==items1[j].pds ) continue;
			
			PT_3DEX p2 = items1[j].pt;
			PT_3DEX p3 = items1[j].pt1;
			
			double dis = sqrt( (p0.x-p2.x)*(p0.x-p2.x)+(p0.y-p2.y)*(p0.y-p2.y) );
			double dis1 = sqrt( (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y) );
			if( dis<m_lfToler )
			{
				dis1 = sqrt( (p1.x-p3.x)*(p1.x-p3.x)+(p1.y-p3.y)*(p1.y-p3.y) );
				if(dis1>m_lfToler)
				{
					continue;
				}
			}
			else if( dis1<m_lfToler )
			{
				dis = sqrt( (p0.x-p3.x)*(p0.x-p3.x)+(p0.y-p3.y)*(p0.y-p3.y) );
				if(dis>m_lfToler)
				{
					continue;
				}
			}
			else
			{
				continue;
			}
			//已找到配对的地物
			CFeature *pFtr1 = items1[j].pFtr;
			items1[i].pFtr = NULL;
			items1[j].pFtr = NULL;
			//图层是否一致
			if( 0!=strcmp(items1[i].pLayer->GetName(),items1[j].pLayer->GetName()) )
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(items1[i].pt);
				CString strReason = StrFromResID(IDS_SURFACE);
				strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_LAYERDIFF);
				m_pChkResMgr->SetReason(strReason);
				m_pChkResMgr->EndResItem();
				break;
			}
			
			//比较颜色是否一致
			if(m_bMatchColor)
			{
				COLORREF color1 = pFtr->GetGeometry()->GetColor();
				if(color1==-1) color1 = items1[i].pLayer->GetColor();
				COLORREF color2 = pFtr1->GetGeometry()->GetColor();
				if(color2==-1) color2 = items1[j].pLayer->GetColor();
				if(color1!=color2)
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->SetAssociatedPos(items1[i].pt);
					CString strReason = StrFromResID(IDS_SURFACE);
					strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_COLORDIFF);
					m_pChkResMgr->SetReason(strReason);
					m_pChkResMgr->EndResItem();
					break;
				}
			}
			//比较扩展属性是否一致
			if(m_bMatchAttr)
			{
				CValueTable tab1, tab2;
				tab1.BeginAddValueItem();
				items1[i].pds->GetXAttributesSource()->GetXAttributes(pFtr, tab1);
				tab1.EndAddValueItem();
				tab2.BeginAddValueItem();
				items1[j].pds->GetXAttributesSource()->GetXAttributes(pFtr1, tab2);
				tab2.EndAddValueItem();
				if(pScheme)
				{
					CSchemeLayerDefine *pdef = pScheme->GetLayerDefine(items1[i].pLayer->GetName());
					if(pdef)
					{
						int size=0;
						const XDefine *defs = pdef->GetXDefines(size);
						for(int n=0; n<size; n++)
						{
							if(defs[n].isMust<0)
							{
								tab1.DelField(defs[n].field);
								tab2.DelField(defs[n].field);
							}
						}
					}
				}
				if(!bTabSame(tab1, tab2))
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->SetAssociatedPos(items1[i].pt);
					CString strReason = StrFromResID(IDS_SURFACE);
					strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_ATTRDIFF);
					m_pChkResMgr->SetReason(strReason);
					m_pChkResMgr->EndResItem();
				}
			}
		}
	}
	
	for(i=0; i<items1.GetSize(); i++)
	{
		if( items1[i].pFtr && items1[i].pds==pDS)
		{
			//记录到检查结果
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(items1[i].pFtr);
			m_pChkResMgr->SetAssociatedPos(items1[i].pt);
			CString strReason = StrFromResID(IDS_SURFACE);
			strReason = strReason + ":" +StrFromResID(IDS_CHK_ERR_NOLINK);
			m_pChkResMgr->SetReason(strReason);
			m_pChkResMgr->EndResItem();
		}
	}

	GProgressEnd();
	CLEAR_MARK_SELECTION;
}

////////////////////////////////////////////////////////////
CCurveIntersectCurveChkCmd::CCurveIntersectCurveChkCmd()
{
	m_lineLayer1 = _T("");
	m_lineLayer2 = _T("");
}
CCurveIntersectCurveChkCmd::~CCurveIntersectCurveChkCmd()
{
	
}
CString CCurveIntersectCurveChkCmd::GetCheckCategory() const
{
    return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CCurveIntersectCurveChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_CMD_CURVEINTERSEC);
}

void CCurveIntersectCurveChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;
	var = (LPCTSTR)(m_lineLayer1);
	tab.AddValue("LineLayer1",&CVariantEx(var));

	var = (LPCTSTR)(m_lineLayer2);
	tab.AddValue("LineLayer2",&CVariantEx(var));
}

void CCurveIntersectCurveChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"LineLayer1",var) )
	{					
		m_lineLayer1 = (LPCTSTR)(_bstr_t)(_variant_t)*var;		
	}

	if( tab.GetValue(0,"LineLayer2",var) )
	{					
		m_lineLayer2 = (LPCTSTR)(_bstr_t)(_variant_t)*var;		
	}
}

void CCurveIntersectCurveChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CCurveIntersectCurveChkCmd"),_T("CCurveIntersectCurveChkCmd"));
	
	param->AddLayerNameParamEx("LineLayer1",(LPCTSTR)m_lineLayer1,StrFromResID(IDS_CMDPLANE_LINELAYER1));
	param->AddLayerNameParamEx("LineLayer2",(LPCTSTR)m_lineLayer2,StrFromResID(IDS_CMDPLANE_LINELAYER2));
}

static bool GLineIntersectLineSeg1(double x0,double y0, double x1, double y1,double x2,double y2,double z2,double x3,double y3, double z3,double *x, double *y,double *z, double *t,double *t3)
{
	double vector1x = x1-x0, vector1y = y1-y0;
	double vector2x = x3-x2, vector2y = y3-y2,vector2z=z3-z2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if( delta<1e-6 && delta>-1e-6 )return false;         //平行无交点
	double t1 = ( (x2-x0)*vector2y-(y2-y0)*vector2x )/delta;
	double xr = x0 + t1*vector1x, yr = y0 + t1*vector1y;
	if( t1<0 || t1>1 )
	{
		if( fabs(xr-x0)<=1e-6 && fabs(yr-y0)<=1e-6 );
		else if( fabs(xr-x1)<=1e-6 && fabs(yr-y1)<=1e-6 );
		else return false;
	}
	
	double t2 = ( (x2-x0)*vector1y-(y2-y0)*vector1x )/delta;
	
	if( t2<0 || t2>1 )
	{
		if( fabs(xr-x2)<=1e-6 && fabs(yr-y2)<=1e-6 );
		else if( fabs(xr-x3)<=1e-6 && fabs(yr-y3)<=1e-6 );
		else return false;
	}
	
	if(x)*x = x0 + t1*vector1x;
	if(y)*y = y0 + t1*vector1y;
	if(z)*z = z2 + t2*vector2z;
	if(t)*t = t1;
	if (t3)
	{
		*t3=t2;
	}
	return true;
}

//计算两曲线的交点，不包含首尾端点为交点
int GetCurveIntersectCurve2(PT_3DEX *pts1, int num1, PT_3DEX *pts2, int num2, CArray<PtIntersect,PtIntersect> &arrIntersectPts)
{
	if (!pts1 || !pts2 || num1 < 2 || num2 < 2) return false;
	
	Envelope e1, e2;
	e1.CreateFromPts(pts1,num1,sizeof(PT_3DEX));
	e2.CreateFromPts(pts2,num2,sizeof(PT_3DEX));
	
	if (!e1.bIntersect(&e2)) return false;
	
	PT_3D ret;
	double t1, t2;
	bool bIntersect = false;
	for (int i=0; i<num1-1; i++)
	{
		if( (pts1+i+1)->pencode == penMove) continue;
		for( int j=0; j<num2-1; j++ )
		{
			if( (pts2+j+1)->pencode == penMove) continue;
			//求相交点
			if (!GLineIntersectLineSeg1(pts1[i].x,pts1[i].y,pts1[i+1].x,pts1[i+1].y,pts2[j].x,pts2[j].y,pts2[j].z,
				pts2[j+1].x,pts2[j+1].y,pts2[j+1].z,&(ret.x),&(ret.y),&(ret.z),&t1,&t2))
			{
				continue;
			}					
			else
			{
				//首尾端点为交点
				if( (i==0&&fabs(t1)<1e-4) || (i==(num1-2)&&fabs(t1-1)<1e-4) ||
					(j==0&&fabs(t2)<1e-4) || (j==(num2-2)&&fabs(t2-1)<1e-4) )
				{
					continue;
				}
				
				PtIntersect item;
				item.IntersectFlag = -1;
				item.pt = ret;
				item.lfRatio = i + t1;
				
				//按照 t 大小排序插入
				int size = arrIntersectPts.GetSize();
				for( int k=0; k<size && item.lfRatio>=arrIntersectPts[k].lfRatio; k++);
				if( k<size )arrIntersectPts.InsertAt(k,item);
				else arrIntersectPts.Add(item);	
			}
		}
	}
	
	return arrIntersectPts.GetSize();
}

void CCurveIntersectCurveChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if (!pDS) return;
	if(m_lineLayer1.IsEmpty() || m_lineLayer1.IsEmpty())
		return;

	CFtrLayerArray arrLayer1, arrLayer2;
	CFtrArray arr1, arr2;
	pDS->GetFtrLayersByNameOrCode_editable(m_lineLayer1, arrLayer1);
	pDS->GetFtrLayersByNameOrCode_editable(m_lineLayer2, arrLayer2);
	int i,j,k;
	for(i=0; i<arrLayer1.GetSize(); i++)
	{
		int nObj = arrLayer1[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = arrLayer1[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;
			CGeometry *pGeo = pFtr->GetGeometry();
			if(pGeo && pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				arr1.Add(pFtr);
			}
		}
	}
	for(i=0; i<arrLayer2.GetSize(); i++)
	{
		int nObj = arrLayer2[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = arrLayer2[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;
			CGeometry *pGeo = pFtr->GetGeometry();
			if(pGeo && pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				arr2.Add(pFtr);
			}
		}
	}
	
	MARK_SELECTION;
	
	GProgressStart(arr1.GetSize());
	for (i=0; i<arr1.GetSize(); i++)
	{
		GProgressStep();
		
		CFeature *pFtr = arr1[i];
		SKIP_NOT_SELECTION;

		CGeometry *pGeo = pFtr->GetGeometry();

		CArray<PT_3DEX,PT_3DEX> pts;
		const CShapeLine *pShape = pGeo->GetShape();
		pShape->GetPts(pts);
		Envelope e = pGeo->GetEnvelope();

		int npt = GraphAPI::GKickoffSame2DPoints(pts.GetData(),pts.GetSize());
		pts.SetSize(npt);

		for(j=0; j<arr2.GetSize(); j++)
		{
			CFeature *pFtr1 = arr2[j];
			if(pFtr==pFtr1) continue;
			Envelope e1 = pFtr1->GetGeometry()->GetEnvelope();
			if(!e.bIntersect(&e1)) continue;

			CArray<PT_3DEX,PT_3DEX> pts1;
			const CShapeLine *pShape0 = pFtr1->GetGeometry()->GetShape();
			pShape0->GetPts(pts1);
			
			int npt1 = GraphAPI::GKickoffSame2DPoints(pts1.GetData(),pts1.GetSize());
			pts1.SetSize(npt1);
			
			CArray<PtIntersect,PtIntersect> arrIntersectPts;
			GetCurveIntersectCurve2(pts.GetData(),npt,pts1.GetData(),npt1,arrIntersectPts);
			
			CArray<PT_3D,PT_3D> arrPts;
			for (k=0; k<arrIntersectPts.GetSize(); k++)
			{
				arrPts.Add(arrIntersectPts[k].pt);
			}
			delSamePt(arrPts);
			for ( k=0;k<arrPts.GetSize();k++)
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->AddAssociatedFtr(pFtr1);
				m_pChkResMgr->SetAssociatedPos(arrPts[k]);
				m_pChkResMgr->SetReason(StrFromResID(IDS_CHK_CMD_CURVEINTERSEC));
				m_pChkResMgr->EndResItem();
			}
		}
	}

	CLEAR_MARK_SELECTION;
}

void CCurveIntersectCurveChkCmd::GetAllReasons( CStringArray& reasons )
{
	reasons.Add(StrFromResID(IDS_CHK_CMD_CURVEINTERSEC));
}


////////////////////////////////////////////////////////////
CSurfaceGapChkCmd::CSurfaceGapChkCmd()
{
	m_lfArea = 50;
}
CSurfaceGapChkCmd::~CSurfaceGapChkCmd()
{
	
}
CString CSurfaceGapChkCmd::GetCheckCategory() const
{
    return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CSurfaceGapChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CMDNAME_SURFACEGAP);
}

void CSurfaceGapChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;

	var = m_lfArea;
	tab.AddValue("lfArea",&CVariantEx(var));
}

void CSurfaceGapChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "lfArea", var))
	{
		m_lfArea = (double)(_variant_t)*var;
	}
}

void CSurfaceGapChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CSurfaceGapChkCmd"),_T("CSurfaceGapChkCmd"));

	param->AddParam("lfArea",m_lfArea,StrFromResID(IDS_HOUSE_AREALIMIT));
}

#include "DlgCommand2.h"
void CSurfaceGapChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	if (!m_pDoc) return;
	CCheckSurfaceGapCommand cmd;
	cmd.Init(m_pDoc);
	cmd.m_lfArea = m_lfArea;
	cmd.m_bAddCheckResult = FALSE;
	PT_3D pt;
	cmd.PtClick(pt, 0);
}

void CSurfaceGapChkCmd::GetAllReasons( CStringArray& reasons )
{	
	reasons.Add(StrFromResID(IDS_REASON_SURFACEGAP));
}

////////////////////////////////////////////////////////////
CCurveSnapChkCmd::CCurveSnapChkCmd()
{
	m_lfRange = 0.02;
}
CCurveSnapChkCmd::~CCurveSnapChkCmd()
{
	
}
CString CCurveSnapChkCmd::GetCheckCategory() const
{
    return StrFromResID(IDS_CHK_CATEGORY_SPATIAL_RELATION);
}

CString CCurveSnapChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CMDNAME_CURVESNAP);
}

void CCurveSnapChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strLayers;
	tab.AddValue("HandleLayers",&CVariantEx(var));
	var = m_lfRange;
	tab.AddValue("lfRange",&CVariantEx(var));
}

void CCurveSnapChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "HandleLayers", var))
	{
		m_strLayers = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0, "lfRange", var))
	{
		m_lfRange = (double)(_variant_t)*var;
	}
}

void CCurveSnapChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CMapMatchChkCmd"),_T("CMapMatchChkCmd"));
	param->AddLayerNameParamEx("HandleLayers",m_strLayers,StrFromResID(IDS_CHKCMD_DES_INCLUDED_LAYERS),NULL,LAYERPARAMITEM_NOTEMPTY_LINEAREA);
	param->AddParam("lfRange",m_lfRange,StrFromResID(IDS_CMDPLANE_TOLER));
}

//非正常重叠
static bool GetOverlapSection1(double xS1,double yS1,double xE1,double yE1,double xS2,double yS2,double xE2,double yE2,double toler,double toler1, 
							  double *xs0,double *ys0,double *xe1,double *ye1/*,double *x2,double *y2,double *x3,double *y3*/)

{
	//将线段1的两点投影到线段2上，垂距超限差那么就不重叠
	double x0,y0,x1,y1,t0,t1;
	GraphAPI::GGetPerpendicular(xS2,yS2,xE2,yE2,xS1,yS1,&x0,&y0,&t0);
	if( fabs(xS1-x0)>toler1 || fabs(yS1-y0)>toler1 )
		return false;//距离太大

	GraphAPI::GGetPerpendicular(xS2,yS2,xE2,yE2,xE1,yE1,&x1,&y1,&t1);
	if(fabs(xS1-x0)<toler&&fabs(yS1-y0)<toler&&fabs(xE1-x1)<toler&&fabs(yE1-y1)<toler)
		return false;//重叠
	else if( fabs(xE1-x1)>toler1 || fabs(yE1-y1)>toler1 )
		return false;//距离太大

	//不重叠
	if( (t0<=0 && t1<=0) || (t0>=1 && t1>=1) )
		return false;

	if( t0<t1 )
	{
		if( t0<0 )
		{
			if(xs0) *xs0 = xS2;  if(ys0) *ys0 = yS2;
		}
		else
		{
			if(xs0) *xs0 = xS1;  if(ys0) *ys0 = yS1;
		}
		
		if( t1>1 )
		{
			if(xe1) *xe1 = xE2;  if(ye1) *ye1 = yE2;
		}
		else
		{
			if(xe1) *xe1 = xE1;  if(ye1) *ye1 = yE1;
		}
	}
	else
	{
		if( t1<0 )
		{
			if(xs0) *xs0 = xS2;  if(ys0) *ys0 = yS2;
		}
		else
		{
			if(xs0) *xs0 = xE1;  if(ys0) *ys0 = yE1;
		}
		
		if( t0>1 )
		{
			if(xe1) *xe1 = xE2;  if(ye1) *ye1 = yE2;
		}
		else
		{
			if(xe1) *xe1 = xS1;  if(ye1) *ye1 = yS1;
		}
	}

	//计算重叠部分长度
	double dis = sqrt((*xs0-*xe1)*(*xs0-*xe1) + (*ys0-*ye1)*(*ys0-*ye1));
	
	//是否超过限差
	if( dis<GraphAPI::g_lfDisTolerance )
		return false;
	
	return true;
}

extern int GetCurveIntersectCurve(PT_3DEX *pts1, int num1, PT_3DEX *pts2, int num2, CArray<PtIntersect,PtIntersect> &arrIntersectPts);
extern CGeometry* Compress(CGeometry *pObj, double limit);

void CCurveSnapChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	if (!m_pDoc) return;
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if(!pDS) return;
	
	CFtrLayerArray arr;
	pDS->GetFtrLayersByNameOrCode_editable(m_strLayers, arr);

	CFtrArray ftrs;
	int i,j, k;
	for(i=0; i<arr.GetSize(); i++)
	{
		int nObj = arr[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = arr[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible()) continue;

			CGeometry* pGeo = pFtr->GetGeometry();
			if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				continue;

			ftrs.Add(pFtr);
		}
	}

	MARK_SELECTION;

	int nSum = ftrs.GetSize();
	GProgressStart(nSum);
	CArray<PT_3DEX, PT_3DEX> pts1, pts2;
	CGeometry *pObj1=NULL, *pObj2=NULL;
	for(i=0; i<nSum; i++)
	{
		GProgressStep();
		CFeature *pFtr = ftrs[i];
		SKIP_NOT_SELECTION;
		Envelope e1 = pFtr->GetGeometry()->GetEnvelope();
		pObj1 = pFtr->GetGeometry()->Linearize();
		if(!pObj1) continue;
		pObj2 = Compress(pObj1, m_lfRange/10);
		if(pObj2)
		{
			delete pObj1;
			pObj1 = NULL;
		}
		else
		{
			pObj2 = pObj1;
		}
		pObj2->GetShape(pts1);
		delete pObj2;
		pObj2=NULL;

		for(j=i+1; j<nSum; j++)
		{
			CFeature *pFtr2 = ftrs[j];
			Envelope e2 = pFtr2->GetGeometry()->GetEnvelope();
			if(!e1.bIntersect(&e2)) continue;

			pObj1 = pFtr2->GetGeometry()->Linearize();
			if(!pObj1) continue;
			pObj2 = Compress(pObj1, m_lfRange/10);
			if(pObj2)
			{
				delete pObj1;
				pObj1 = NULL;
			}
			else
			{
				pObj2 = pObj1;
			}
			pObj2->GetShape(pts2);
			delete pObj2;
			pObj2=NULL;

			//至少有两个交点
			//CArray<PtIntersect,PtIntersect> arr;
			//GetCurveIntersectCurve(pts1.GetData(), pts1.GetSize(), pts2.GetData(), pts2.GetSize(), arr);
			//if(arr.GetSize()<2) continue;

			CArray<PT_3D,PT_3D> ret;
			CheckSnap(pts1, pts2, ret);
			CheckSnap(pts2, pts1, ret);
			for(k=0; k<ret.GetSize(); k++)
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->AddAssociatedFtr(pFtr2);
				m_pChkResMgr->SetAssociatedPos(ret[k]);
				m_pChkResMgr->SetReason(StrFromResID(IDS_REASON_CURVESNAP));
				m_pChkResMgr->EndResItem();
			}
		}
	}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}

void CCurveSnapChkCmd::GetAllReasons( CStringArray& reasons )
{	
	reasons.Add(StrFromResID(IDS_REASON_CURVESNAP));
}

void CCurveSnapChkCmd::CheckSnap(CArray<PT_3DEX, PT_3DEX>& pts1, CArray<PT_3DEX, PT_3DEX>& pts2, CArray<PT_3D,PT_3D>& ret)
{
	int nPt1 = pts1.GetSize();
	int nPt2 = pts2.GetSize();
	if(nPt1<2 || nPt2<2) return;

	double toler = GraphAPI::g_lfDisTolerance;

	for(int i=0; i<nPt1-1; i++)
	{
		PT_3D pt0 = pts1[i];
		PT_3D pt1 = pts1[i+1];

		for(int j=0; j<nPt2-1; j++)
		{
			PT_3D pt2 = pts2[j];
			PT_3D pt3 = pts2[j+1];
			double retX0,retY0,retX1,retY1;
			if(GetOverlapSection1(pt0.x,pt0.y,pt1.x,pt1.y,pt2.x,pt2.y,pt3.x,pt3.y,toler,m_lfRange,&retX0,&retY0,&retX1,&retY1))
			{
				PT_3D temp((retX0+retX1)/2, (retY0+retY1)/2, 0);
				ret.Add(temp);
			}
		}
	}
}


////////////////////////////////////////////////////////////
CCulvertLineTypeChkCmd::CCulvertLineTypeChkCmd()
{
	m_nLineType = 1;
}
CCulvertLineTypeChkCmd::~CCulvertLineTypeChkCmd()
{
	
}
CString CCulvertLineTypeChkCmd::GetCheckCategory() const
{
    return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CCulvertLineTypeChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_CULVERT_LINETYPE);
}

void CCulvertLineTypeChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strCulvertLayer;
	tab.AddValue("CulvertLayer",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayers;
	tab.AddValue("HandleLayers",&CVariantEx(var));

	var = (long)m_nLineType;
	tab.AddValue("nLineType",&CVariantEx(var));
}

void CCulvertLineTypeChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;

	if (tab.GetValue(0, "CulvertLayer", var))
	{
		m_strCulvertLayer = (const char*)(_bstr_t)(_variant_t)*var;
	}

	if (tab.GetValue(0, "HandleLayers", var))
	{
		m_strLayers = (const char*)(_bstr_t)(_variant_t)*var;
	}

	if (tab.GetValue(0, "nLineType", var))
	{
		m_nLineType = (long)(_variant_t)*var;
	}
}

void CCulvertLineTypeChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CulvertLineTypeChkCmd"),_T("CulvertLineTypeChkCmd"));
	param->AddLayerNameParam("CulvertLayer",m_strCulvertLayer,StrFromResID(IDS_CULVERT_SURFACE),NULL,LAYERPARAMITEM_NOTEMPTY_AREA);
	param->AddLayerNameParamEx("HandleLayers",m_strLayers,StrFromResID(IDS_RIVERSURFACE_LAYER),NULL,LAYERPARAMITEM_NOTEMPTY_AREA);

	CString strLinetypeName1;
	CString strLinetypeName2;
	if(!m_strCulvertLayer.IsEmpty() ||bForLoad)
	{
		CConfigLibManager *pCL = GetConfigLibManager();
		CDlgDoc *pDoc = GetActiveDlgDoc();
		if(pCL && pDoc)
		{
			CScheme *pScheme = pCL->GetScheme(pDoc->GetDlgDataSource()->GetScale());
			if(pScheme)
			{
				CSchemeLayerDefine *pDef = pScheme->GetLayerDefine(m_strCulvertLayer);
				if(pDef)
				{
					int nSymbol = pDef->GetSymbolCount();
					for(int i=0; i<nSymbol; i++)
					{
						CSymbol *pSymbol = pDef->GetSymbol(i);
						if(!pSymbol) continue;

						if(pSymbol->GetType() == SYMTYPE_CULVERTSURFACE1)
						{
							strLinetypeName1 = ((CCulvertSurface1Symbol*)pSymbol)->m_strLinetypeName;
						}
						else if(pSymbol->GetType() == SYMTYPE_CULVERTSURFACE2)
						{
							strLinetypeName2 = ((CCulvertSurface2Symbol*)pSymbol)->m_strLinetypeName;
						}
					}
				}
			}
		}
	}
	param->BeginOptionParam("nLineType",StrFromResID(IDS_CMDPLANE_CULVERT));
	param->AddOption(strLinetypeName1,0,' ',m_nLineType==0);
	param->AddOption(strLinetypeName2,1,' ',m_nLineType==1);
	param->EndOptionParam();
}

void CCulvertLineTypeChkCmd::Execute( BOOL bForSelection/*=FALSE*/ )
{
	if (!m_pDoc || m_strCulvertLayer.IsEmpty() || m_strLayers.IsEmpty())
		return;
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if(!pDS) return;
	
	CFtrLayerArray arr;
	pDS->GetFtrLayersByNameOrCode_editable(m_strLayers, arr);
	pDS->SaveAllQueryFlags(TRUE,FALSE);

	int i;
	for(i=0; i<arr.GetSize(); i++)
	{
		arr[i]->SetAllowQuery(TRUE);
	}

	CFtrLayer *pLayer = pDS->GetFtrLayer(m_strCulvertLayer);
	if(!pLayer) return;

	MARK_SELECTION;

	int nObj = pLayer->GetObjectCount();
	GProgressStart(nObj);
	for(i=0; i<nObj; i++)
	{
		GProgressStep();
		CFeature *pFtr = pLayer->GetObject(i);
		if(!pFtr || !pFtr->IsVisible())
			continue;
		SKIP_NOT_SELECTION;
		CGeometry *pGeo = pFtr->GetGeometry();
		if(!pGeo || pGeo->GetClassType()!=CLS_GEOSURFACE)
			continue;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeo->GetShape(arrPts);
		int nPt = arrPts.GetSize();
		if(nPt<4) continue;

		if(1!=GraphAPI::GIsClockwise(arrPts.GetData(), arrPts.GetSize()))
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(arrPts[0]);
			m_pChkResMgr->SetReason(StrFromResID(IDS_CULVERT_REASON1));
			m_pChkResMgr->EndResItem();
			continue;
		}

		PT_3D ptcen;
		if(m_nLineType==0)
		{
			ptcen.x = (arrPts[0].x+arrPts[1].x)/2;
			ptcen.y = (arrPts[0].y+arrPts[1].y)/2;
			ptcen.z = 0;
		}
		else
		{
			ptcen.x = (arrPts[nPt-1].x+arrPts[nPt-2].x)/2;
			ptcen.y = (arrPts[nPt-1].y+arrPts[nPt-2].y)/2;
			ptcen.z = 0;
		}

		Envelope e;
		e.CreateFromPtAndRadius(ptcen, GraphAPI::g_lfDisTolerance);
		int num = m_pDoc->GetDataQuery()->FindObjectInRect(e,NULL,FALSE,FALSE);
		if(num<=0)
		{
			m_pChkResMgr->BeginResItem();
			m_pChkResMgr->AddAssociatedFtr(pFtr);
			m_pChkResMgr->SetAssociatedPos(ptcen);
			m_pChkResMgr->SetReason(StrFromResID(IDS_CULVERT_REASON));
			m_pChkResMgr->EndResItem();
		}
	}
	GProgressEnd();

	pDS->RestoreAllQueryFlags();
	CLEAR_MARK_SELECTION;
}

void CCulvertLineTypeChkCmd::GetAllReasons( CStringArray& reasons )
{	
	reasons.Add(StrFromResID(IDS_CULVERT_REASON));
}

////////////////////////////////////////////////////////////
CHouseRectangularChkCmd::CHouseRectangularChkCmd()
{
	m_lfToler1 = 1.0;
	//m_lfToler2 = 5.0;
	m_lfAreaToler = 100.0;
}
CHouseRectangularChkCmd::~CHouseRectangularChkCmd()
{

}
CString CHouseRectangularChkCmd::GetCheckCategory() const
{
	return StrFromResID(IDS_CHK_CATEGORY_PARTICAL_APP);
}

CString CHouseRectangularChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_HOUSE_RECTANGULAR);
}

void CHouseRectangularChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strHouseLayers;
	tab.AddValue("HouseLayers", &CVariantEx(var));

	var = m_lfToler1;
	tab.AddValue("lfToler1", &CVariantEx(var));

	//var = m_lfToler2;
	//tab.AddValue("lfToler2", &CVariantEx(var));

	var = m_lfAreaToler;
	tab.AddValue("lfAreaToler", &CVariantEx(var));
}

void CHouseRectangularChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;

	if (tab.GetValue(0, "HouseLayers", var))
	{
		m_strHouseLayers = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0, "lfToler1", var))
	{
		m_lfToler1 = (double)(_variant_t)*var;
	}
	/*if (tab.GetValue(0, "lfToler2", var))
	{
		m_lfToler2 = (double)(_variant_t)*var;
	}*/
	if (tab.GetValue(0, "lfAreaToler", var))
	{
		m_lfAreaToler = (double)(_variant_t)*var;
	}
}

void CHouseRectangularChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("HouseRectangularChkCmd"), _T("CulvertLineTypeChkCmd"));
	param->AddLayerNameParamEx("HouseLayers", m_strHouseLayers, StrFromResID(IDS_HOUSE_LAYNAMES), NULL, LAYERPARAMITEM_NOTEMPTY_AREA);
	param->AddParam("lfToler1", m_lfToler1, StrFromResID(IDS_RECTANGULAR_TOLER));
	//param->AddParam("lfToler2", m_lfToler2, StrFromResID(IDS_CHK_TOLER));
	param->AddParam("lfAreaToler", m_lfAreaToler, StrFromResID(IDS_HOUSE_AREALIMIT) + "(>)");
}

void CHouseRectangularChkCmd::Execute(BOOL bForSelection/*=FALSE*/)
{
	if (!m_pDoc || m_strHouseLayers.IsEmpty())
		return;
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if (!pDS) return;

	CFtrLayerArray arr;
	pDS->GetFtrLayersByNameOrCode_editable(m_strHouseLayers, arr);

	int i, j, nSum = 0;
	for (i = 0; i < arr.GetSize(); i++)
	{
		nSum += arr[i]->GetObjectCount();
	}

	double lftoler1 = PI*m_lfToler1 / 180;
	//double lftoler2 = PI*m_lfToler2 / 180;

	MARK_SELECTION;

	GProgressStart(nSum);
	for (i = 0; i < arr.GetSize(); i++)
	{
		for (j = 0; j < arr[i]->GetObjectCount(); j++)
		{
			GProgressStep();
			CFeature *pFtr = arr[i]->GetObject(j);
			if (!pFtr || !pFtr->IsVisible())
				continue;

			SKIP_NOT_SELECTION;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				continue;

			if (((CGeoCurveBase*)pGeo)->GetArea() < m_lfAreaToler)
				continue;

			CArray<PT_3DEX, PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			if (!((CGeoCurveBase*)pGeo)->IsClosed())
			{
				arrPts.Add(arrPts[0]);
			}
			int nSz = GraphAPI::GKickoffSame2DPoints(arrPts.GetData(), arrPts.GetSize());
			arrPts.SetSize(nSz);
			if (nSz < 3) continue;
			PT_3DEX* pts = arrPts.GetData();

			for (int k = 1; k<nSz - 1; ++k)
			{
				if (pts[k].pencode == penMove || pts[k + 1].pencode == penMove)
					continue;
				double ang = GraphAPI::GGetIncludedAngle(pts[k], pts[k - 1], pts[k], pts[k + 1]);
				ang = fabs(ang - PI / 2);
				if (ang>lftoler1/* && ang < lftoler2*/)
				{
					m_pChkResMgr->BeginResItem();
					m_pChkResMgr->AddAssociatedFtr(pFtr);
					m_pChkResMgr->SetAssociatedPos(pts[k]);
					m_pChkResMgr->SetReason(StrFromResID(IDS_REASON_NOT_RECTANGULAR));
					m_pChkResMgr->EndResItem();
				}
			}


			//处理闭合点
			PT_3DEX pt0;
			PT_3DEX pt1;
			PT_3DEX pt2;
			for (k = 0; k<nSz - 1; k++)
			{
				if (k == 0 || pts[k].pencode == penMove)
				{
					pt1 = pts[k];
					pt2 = pts[k + 1];
				}

				if (k + 2 >= nSz || pts[k + 2].pencode == penMove)
				{
					pt0 = pts[k];
					double ang = GraphAPI::GGetIncludedAngle(pt1, pt0, pt1, pt2);
					ang = fabs(ang - PI / 2);
					if (ang>lftoler1/* && ang < lftoler2*/)
					{
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetAssociatedPos(pt1);
						m_pChkResMgr->SetReason(StrFromResID(IDS_REASON_NOT_RECTANGULAR));
						m_pChkResMgr->EndResItem();
					}
				}
			}
		}
	}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}

void CHouseRectangularChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add(StrFromResID(IDS_REASON_NOT_RECTANGULAR));
}

////////////////////////////////////////////////////////////
CHeiPointAcrossCurveChkCmd::CHeiPointAcrossCurveChkCmd()
{
	m_lfToler1 = 300.0;
	m_lfToler2 = 400.0;
}
CHeiPointAcrossCurveChkCmd::~CHeiPointAcrossCurveChkCmd()
{

}
CString CHeiPointAcrossCurveChkCmd::GetCheckCategory() const
{
	return StrFromResID(IDS_CHK_CATEGORY_CONTOUR);
}

CString CHeiPointAcrossCurveChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_HEIPT_ACROSSRIVER);
}

void CHeiPointAcrossCurveChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strHeiLayer;
	tab.AddValue("strHeiLayer", &CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayers;
	tab.AddValue("strLayers", &CVariantEx(var));

	var = m_lfToler1;
	tab.AddValue("lfToler1", &CVariantEx(var));

	var = m_lfToler2;
	tab.AddValue("lfToler2", &CVariantEx(var));
}

void CHeiPointAcrossCurveChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;

	if (tab.GetValue(0, "strHeiLayer", var))
	{
		m_strHeiLayer = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0, "strLayers", var))
	{
		m_strLayers = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0, "lfToler1", var))
	{
		m_lfToler1 = (double)(_variant_t)*var;
	}
	if (tab.GetValue(0, "lfToler2", var))
	{
		m_lfToler2 = (double)(_variant_t)*var;
	}
}

void CHeiPointAcrossCurveChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("HouseRectangularChkCmd"), _T("CulvertLineTypeChkCmd"));
	param->AddLayerNameParam("strHeiLayer", m_strHeiLayer, StrFromResID(IDS_CMDPLANE_POINTZLAYER), NULL, LAYERPARAMITEM_NOTEMPTY_POINT);
	param->AddLayerNameParamEx("strLayers", m_strLayers, StrFromResID(IDS_LINK_RIVERLAYERS), NULL, LAYERPARAMITEM_NOTEMPTY_LINE);
	param->AddParam("lfToler1", m_lfToler1, StrFromResID(IDS_CMDPLANE_CHECKBOUND) + "(>)");
	param->AddParam("lfToler2", m_lfToler2, StrFromResID(IDS_CMDPLANE_CHECKBOUND) + "(<)");
}

static double GetDisofPtatCurve(PT_3DEX& pt, CArray<PT_3DEX, PT_3DEX>& pts)
{
	int nPt = pts.GetSize();
	double dis = -1;
	int i = 0;
	for (i = 0; i < nPt - 1; i++)
	{
		if (GraphAPI::GIsPtInLine(pts[i], pts[i + 1], pt))
		{
			dis += GraphAPI::GGet2DDisOf2P(pts[i], pt);
			break;
		}
		else
		{
			dis += GraphAPI::GGet2DDisOf2P(pts[i], pts[i + 1]);
		}
	}

	if (i >= nPt - 1) return -1;

	return dis;
}

struct DisItem
{
	PT_3DEX pt;
	double dis;
};

int compare_DisItem(const void *p1, const void *p2)
{
	DisItem *a = (DisItem*)p1;
	DisItem *b = (DisItem*)p2;
	return (a->dis) > (b->dis) ? 1 : -1;
}

void CHeiPointAcrossCurveChkCmd::Execute(BOOL bForSelection/*=FALSE*/)
{
	if (!m_pDoc || m_strHeiLayer.IsEmpty() || m_strLayers.IsEmpty())
		return;
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if (!pDS) return;
	CDataQueryEx *pDQ = m_pDoc->GetDataQuery();
	if (!pDQ) return;

	CFtrLayer *pHeiLayer = pDS->GetFtrLayer(m_strHeiLayer);
	if (!pHeiLayer) return;
	//只让高程点图层可查询
	pDS->SaveAllQueryFlags(TRUE, FALSE);
	pHeiLayer->SetAllowQuery(TRUE);

	CFtrLayerArray arr;
	pDS->GetFtrLayersByNameOrCode_editable(m_strLayers, arr);

	int i, j, k, nSum = 0;
	for (i = 0; i < arr.GetSize(); i++)
	{
		nSum += arr[i]->GetObjectCount();
	}

	MARK_SELECTION;

	GProgressStart(nSum);
	for (i = 0; i < arr.GetSize(); i++)
	{
		for (j = 0; j < arr[i]->GetObjectCount(); j++)
		{
			GProgressStep();
			CFeature *pFtr = arr[i]->GetObject(j);
			if (!pFtr || !pFtr->IsVisible())
				continue;

			SKIP_NOT_SELECTION;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				continue;

			CArray<PT_3DEX, PT_3DEX> pts;
			const CShapeLine * pShape = pGeo->GetShape();
			if (!pShape) continue;
			pShape->GetPts(pts);

			Envelope e = pGeo->GetEnvelope();
			pDQ->FindObjectInRect(e, NULL, FALSE, FALSE);

			int num = 0;
			const CPFeature * ftr = pDQ->GetFoundHandles(num);
			if (num > 0)
			{
				DisItem *dis = new DisItem[num];
				for (k = 0; k < num; k++)
				{
					dis[k].dis = -1;
					CGeometry *pGeo1 = ftr[k]->GetGeometry();
					if (!pGeo1 || !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
						continue;
					PT_3DEX ptex = pGeo1->GetDataPoint(0);
					dis[k].pt = ptex;
					dis[k].dis = GetDisofPtatCurve(ptex, pts);
				}

				qsort(dis, num, sizeof(DisItem), compare_DisItem);

				double lastdis = -1;
				for (k = 0; k < num; k++)
				{
					if (dis[k].dis < 0) continue;

					if (lastdis < 0)//河流首端点到第一个高程点的距离必须小于m_lfToler2
					{
						if (dis[k].dis > m_lfToler2)
						{
							m_pChkResMgr->BeginResItem();
							m_pChkResMgr->AddAssociatedFtr(pFtr);
							m_pChkResMgr->SetAssociatedPos(dis[k].pt);
							CString reason;
							reason.Format("dis(%.2lf)>%.2lf", dis[k].dis, m_lfToler2);
							m_pChkResMgr->EndResItem();
							break;
						}
						else
						{
							lastdis = dis[k].dis;
						}
						continue;
					}

					if (dis[k].dis < lastdis + m_lfToler1)
					{
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetAssociatedPos(dis[k].pt);
						CString reason;
						reason.Format("dis(%.2lf)<%.2lf", dis[k].dis - lastdis, m_lfToler1);
						m_pChkResMgr->SetReason(reason);
						m_pChkResMgr->EndResItem();
						break;
					}
					else if (dis[k].dis > lastdis + m_lfToler2)
					{
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetAssociatedPos(dis[k].pt);
						CString reason;
						reason.Format("dis(%.2lf)>%.2lf", dis[k].dis - lastdis, m_lfToler2);
						m_pChkResMgr->SetReason(reason);
						m_pChkResMgr->EndResItem();
						break;
					}

					lastdis = dis[k].dis;
				}

				//河流尾端点到最后一个高程点的距离必须小于m_lfToler2
				if (k == num)
				{
					double allLen = ((CGeoCurveBase*)pGeo)->GetLength();
					if (allLen - lastdis > m_lfToler2)
					{
						m_pChkResMgr->BeginResItem();
						m_pChkResMgr->AddAssociatedFtr(pFtr);
						m_pChkResMgr->SetAssociatedPos(pts[pts.GetSize() - 1]);
						CString reason;
						reason.Format("dis(%.2lf)>%.2lf", allLen - lastdis, m_lfToler2);
						m_pChkResMgr->SetReason(reason);
						m_pChkResMgr->EndResItem();
					}
				}

				delete[] dis;
			}
			else
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pts[0]);
				m_pChkResMgr->SetReason("No Point");
				m_pChkResMgr->EndResItem();
			}
		}
	}
	GProgressEnd();

	pDS->RestoreAllQueryFlags();

	CLEAR_MARK_SELECTION;
}

void CHeiPointAcrossCurveChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add("dis");
}


////////////////////////////////////////////////////////////
CNarrowSurfaceChkCmd::CNarrowSurfaceChkCmd()
{
	m_lfToler = 5;
}
CNarrowSurfaceChkCmd::~CNarrowSurfaceChkCmd()
{

}
CString CNarrowSurfaceChkCmd::GetCheckCategory() const
{
	return StrFromResID(IDS_CHK_CATEGORY_CONTOUR);
}

CString CNarrowSurfaceChkCmd::GetCheckName() const
{
	return StrFromResID(IDS_CHK_NARROWSURFACE);
}

void CNarrowSurfaceChkCmd::GetParams(CValueTable& tab)const
{
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strLayers;
	tab.AddValue("strLayers", &CVariantEx(var));
	var = m_lfToler;
	tab.AddValue("lfToler", &CVariantEx(var));
}

void CNarrowSurfaceChkCmd::SetParams(CValueTable& tab)
{
	const CVariantEx *var;

	if (tab.GetValue(0, "strLayers", var))
	{
		m_strLayers = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0, "lfToler", var))
	{
		m_lfToler = (double)(_variant_t)*var;
	}
}

void CNarrowSurfaceChkCmd::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("HouseRectangularChkCmd"), _T("CulvertLineTypeChkCmd"));
	param->AddLayerNameParamEx("strLayers", m_strLayers, StrFromResID(IDS_CMDPLANE_HANDLELAYER), NULL, LAYERPARAMITEM_NOTEMPTY_AREA);
	param->AddParam("lfToler2", m_lfToler, StrFromResID(IDS_CMDPLANE_TOLER));
}

//线串和线段相交
static int GetCurveIntersectSeg1(CGeometry *pGeo, PT_2D pt1, PT_2D pt2, CArray<PtIntersect, PtIntersect> &arr, BOOL sortByX)
{
	if (!pGeo) return 0;
	CArray<PT_3DEX, PT_3DEX> pts;
	pGeo->GetShape()->GetPts(pts);
	int nPt = pts.GetSize();
	if (nPt < 5) return 0;

	PT_3D ret;
	double t;
	for (int i = 0; i < nPt - 1; i++)
	{
		if (pts[i+1].pencode == penMove) continue;
		//求相交点
		if (!GLineIntersectLineSeg1(pt1.x, pt1.y, pt2.x, pt2.y,
			pts[i].x, pts[i].y, pts[i + 1].x, pts[i + 1].y, &(ret.x), &(ret.y), &t))
		{
			continue;
		}
		else
		{
			PtIntersect item;
			item.IntersectFlag = i;
			item.pt = ret;
			item.lfRatio = t;

			if (sortByX)
			{
				int size = arr.GetSize();
				for (int k = 1; k<size && item.pt.x>arr[k].pt.x; k++);
				if (k < size)arr.InsertAt(k, item);
				else arr.Add(item);
			}
			else
			{
				int size = arr.GetSize();
				for (int k = 1; k<size && item.pt.y>arr[k].pt.y; k++);
				if (k < size)arr.InsertAt(k, item);
				else arr.Add(item);
			}
		}
	}

	return arr.GetSize();
}

static BOOL IsNarrowSurface(CGeometry *pGeo, double toler, PT_3DEX& ret)
{
	CArray<PT_3DEX, PT_3DEX> pts;
	const CShapeLine * pShape = pGeo->GetShape();
	if (!pShape) return FALSE;
	pShape->GetPts(pts);
	int nPt = pts.GetSize();
	if (nPt < 6) return FALSE;

	Envelope e = pGeo->GetEnvelope();

	int GRIDSIZE = 5;
	double dertax = e.Width() / GRIDSIZE;
	double dertay = e.Height() / GRIDSIZE;
	int i, j;
	//横向扫描
	for (i = 1; i < GRIDSIZE; i++)
	{
		PT_2D ptA(e.m_xl + dertax*i, e.m_yh);
		PT_2D ptB(e.m_xl + dertax*i, e.m_yl);

		CArray<PtIntersect, PtIntersect> arrIntersectPts;
		int size = GetCurveIntersectSeg1(pGeo, ptA, ptB, arrIntersectPts,TRUE);
		if (size > 1)
		{
			for (j = 0; j < size-1; j += 2)
			{
				double dis = GraphAPI::GGet2DDisOf2P(arrIntersectPts[j].pt, arrIntersectPts[j + 1].pt);
				int dertapos = abs(arrIntersectPts[j].IntersectFlag - arrIntersectPts[j + 1].IntersectFlag);
				if (dis < toler && dertapos>2 && nPt-dertapos>2)
				{
					ret.x = arrIntersectPts[j].pt.x;
					ret.y = arrIntersectPts[j].pt.y;
					return TRUE;
				}
			}
		}
	}

	//纵向扫描
	for (i = 1; i < GRIDSIZE; i++)
	{
		PT_2D ptA(e.m_xl, e.m_yl+dertay*i);
		PT_2D ptB(e.m_xh, e.m_yl+dertay*i);

		CArray<PtIntersect, PtIntersect> arrIntersectPts;
		int size = GetCurveIntersectSeg1(pGeo, ptA, ptB, arrIntersectPts, FALSE);
		if (size > 1)
		{
			for (j = 0; j < size-1; j += 2)
			{
				double dis = GraphAPI::GGet2DDisOf2P(arrIntersectPts[j].pt, arrIntersectPts[j + 1].pt);
				int dertapos = abs(arrIntersectPts[j].IntersectFlag - arrIntersectPts[j + 1].IntersectFlag);
				if (dis < toler && dertapos>2 && nPt-dertapos>2)
				{
					ret.x = arrIntersectPts[j].pt.x;
					ret.y = arrIntersectPts[j].pt.y;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void CNarrowSurfaceChkCmd::Execute(BOOL bForSelection/*=FALSE*/)
{
	if (!m_pDoc) return;
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if (!pDS) return;
	CDataQueryEx *pDQ = m_pDoc->GetDataQuery();
	if (!pDQ) return;

	CFtrLayerArray arrLayers;
	pDS->GetFtrLayersByNameOrCode_editable(m_strLayers, arrLayers);

	int i, j, nSum = 0;
	for (i = 0; i < arrLayers.GetSize(); i++)
	{
		nSum += arrLayers[i]->GetObjectCount();
	}

	MARK_SELECTION;

	GProgressStart(nSum);
	for (i = 0; i < arrLayers.GetSize(); i++)
	{
		int nObi = arrLayers[i]->GetObjectCount();
		for (j = 0; j < nObi; j++)
		{
			GProgressStep();
			CFeature *pFtr = arrLayers[i]->GetObject(j);
			if (!pFtr || !pFtr->IsVisible())
				continue;

			SKIP_NOT_SELECTION;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				continue;
			
			PT_3DEX pt;
			if (IsNarrowSurface(pGeo,m_lfToler,pt))
			{
				m_pChkResMgr->BeginResItem();
				m_pChkResMgr->AddAssociatedFtr(pFtr);
				m_pChkResMgr->SetAssociatedPos(pt);
				m_pChkResMgr->SetReason("狭长面");
				m_pChkResMgr->EndResItem();
			}
		}
	}
	GProgressEnd();

	CLEAR_MARK_SELECTION;
}

void CNarrowSurfaceChkCmd::GetAllReasons(CStringArray& reasons)
{
	reasons.Add("狭长面");
}