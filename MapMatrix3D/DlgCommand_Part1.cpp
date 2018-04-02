#include "stdafx.h"
#include "EditBase.h"
#include "Command.h"
#include "Editor.h"
#include "GeoCurve.h"
#include "RegDef.h"
#include "RegDef2.h"
#include "UIParam2.h"
#include "Linearizer.h"
#include "DataSource.h"
#include "Functions_temp.h "
#include "SmartViewFunctions.h"
#include "Resource.h"
#include "DlgCommand.h"
#include "MainFrm.h"
#include "ExMessage.h"
#include "PropertiesViewBar.h"
#include "EditBasedoc.h"
#include "DlgDataSource.h "
#include "ConditionSelect.h "
#include "StreamCompress.h "
#include "Container.h" 
#include "GeoText.h "
#include "GrTrim.h "
#include "Scheme.h "
#include "SymbolLib.h "
#include "PlotText.h "
#include "DlgInputData.h"
//#include "GeoSurface.h "
#include <math.h>
#include "UIFFileDialogEx.h "
#include "DlgSetXYZ.h"
#include "..\CORE\viewer\EditBase\res\resource.h"

#include "CommandLidar.h"

#include "DlgChangeFCode.h"
#include <map>
#include <list>
#include <set>
#include "baseview.h"
#include "DlgOpSettings.h"
#include "SQLiteAccess.h"
#include "Mapper.h"
#include "DlgEditText.h"

#include "EditBase.h"
#include "DlgBatExportMaps.h"

#include "UIFBoolProp.h"
#include "PlotWChar.h"
#include "DlgInputCode.h"
#include "DlgSymbolTable.h"
#include "SymbolBase.h"
#include "DlgCommand2.h"
#include "UVSModify.h"
#include "../Global/GeoBuilderContour.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define  MAXDISTANCE 99999999
#define  INTPAPI_SUCESS  0
#define  INTPAPI_INVALIDE_PARAM  1
#define  INTPAPI_ERROR  2

int CALLBACK EnumFontFamProc(LPENUMLOGFONT lpelf, LPNEWTEXTMETRIC lpntm, DWORD nFontType, LONG_PTR lparam);
extern int GetAccessType(CDataSourceEx *pDS);

IMPLEMENT_DYNAMIC(CDlgDrawPointCommand, CDrawPointCommand)

CDlgDrawPointCommand::CDlgDrawPointCommand()
{
	m_pSel = NULL;
}

CDlgDrawPointCommand::~CDlgDrawPointCommand()
{
	if (m_pSel) delete m_pSel;
}


void CDlgDrawPointCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOPOINT);
	if (!m_pFtr) return;

	m_pDrawProc = new CDrawPointProcedure;
	if (!m_pDrawProc)return;

	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_pGeoPoint = (CGeoPoint*)m_pFtr->GetGeometry();

	m_pDrawProc->Start();

	CDrawCommand::Start();

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}

void CDlgDrawPointCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}
void CDlgDrawPointCommand::Finish()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);

	if (m_pEditor)
	{
		if (m_pFtr)
		{
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);
			if (bAnnotToText)
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
				//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if (pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);
				if (pLayer != NULL && pTextLayer != NULL && pTempl != NULL)
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor), m_pFtr, arrAnnots);
					CUndoFtrs undo(m_pEditor, Name());
					for (int i = 0; i < arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType() == SYMTYPE_ANNOTATION)
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr, tab);
							if (pAnnot->ExtractGeoText(m_pFtr, parr, tab, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()))
							{
								for (int j = 0; j < parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);
									PDOC(m_pEditor)->AddObject(pNewFtr, pTextLayer->GetID());
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
								}
							}
						}
					}
					undo.Commit();
				}
				if (pTempl != NULL)delete pTempl;
			}
		}
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CDrawPointCommand::Finish();
}

CProcedure *CDlgDrawPointCommand::GetActiveSonProc(int MsgType)
{
	if (MsgType == msgPtMove)
	{
		return NULL;
	}

	return m_pDrawProc;
}

void CDlgDrawPointCommand::PtMove(PT_3D &pt)
{
	if (!m_pFtr || !m_pDrawProc || GetState() == PROCSTATE_FINISHED)return;

	GrBuffer buf;
	PT_3DEX expt(pt, penLine);
	m_pDrawProc->m_pGeoPoint->CreateShape(&expt, 1);

	CString strLayerName;
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (pLayer)
	{
		strLayerName = pLayer->GetName();
	}

	GETDS(m_pEditor)->DrawFeature(m_pFtr, &buf, TRUE, 0, strLayerName);

	m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf, uv_VectorView);

	CDrawCommand::PtMove(pt);
}

void CDlgDrawPointCommand::Abort()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CDrawPointCommand::Abort();
}

void CDlgDrawPointCommand::OnSonEnd(CProcedure *son)
{
	CDrawPointCommand::OnSonEnd(son);

}

DrawingInfo CDlgDrawPointCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	CArray<PT_3DEX, PT_3DEX> arr;
	m_pFtr->GetGeometry()->GetShape(arr);
	return DrawingInfo(m_pFtr, arr);
}


IMPLEMENT_DYNAMIC(CDlgDrawSurfacePointCommand, CDrawPointCommand)

CDlgDrawSurfacePointCommand::CDlgDrawSurfacePointCommand()
{
	m_bCreateFace = FALSE;
}

CDlgDrawSurfacePointCommand::~CDlgDrawSurfacePointCommand()
{
}

CString CDlgDrawSurfacePointCommand::Name()
{
	return StrFromResID(IDS_DRAW_SURFACEPOINT);
}


void CDlgDrawSurfacePointCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOSURFACEPOINT);
	if (!m_pFtr) return;

	m_pDrawProc = new CDrawPointProcedure;
	if (!m_pDrawProc)return;

	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_pGeoPoint = (CGeoPoint*)m_pFtr->GetGeometry();

	m_pDrawProc->Start();

	CDrawCommand::Start();

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}

void CDlgDrawSurfacePointCommand::GetParams(CValueTable &tab)
{
	CDrawCommand::GetParams(tab);

	if (!m_pDrawProc) return;

	_variant_t var;
	var = (bool)m_bCreateFace;
	tab.AddValue("CreateFace", &CVariantEx(var));
}

void CDlgDrawSurfacePointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(AccelStr(), Name());
	param->AddParam("CreateFace", (bool)m_bCreateFace, StrFromLocalResID(IDS_CREATE_FACE));
}

BOOL CDlgDrawSurfacePointCommand::AddObject(CPFeature pFtr, int layid)
{
	BOOL res = ((CDlgDoc*)m_pEditor)->AddObject(pFtr, layid);
	if (m_bCreateFace && res)
	{
		CTopoSurfaceFromPtCommand* pCommand = new CTopoSurfaceFromPtCommand();
		pCommand->Start();
		pCommand->Init(m_pEditor);
		PT_3D pt;
		PT_3DEX ptex = m_pDrawProc->m_pGeoPoint->GetDataPoint(0);
		COPY_3DPT(pt, ptex);
		pCommand->PtClick(pt, 0);
		delete pCommand;
	}
	return res;
}


void CDlgDrawSurfacePointCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "CreateFace", var))
	{
		m_bCreateFace = (bool)(_variant_t)*var;
	}

	SetSettingsModifyFlag();
	CDrawCommand::SetParams(tab, bInit);
}


IMPLEMENT_DYNAMIC(CDlgDrawCurveCommand, CDrawCurveCommand)
CDlgDrawCurveCommand::CDlgDrawCurveCommand()
{
	m_pSel = NULL;
	m_bNeedDrawSymbol = FALSE;
}

CDlgDrawCurveCommand:: ~CDlgDrawCurveCommand()
{
	if (m_pSel)delete m_pSel;
}


void CDlgDrawCurveCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOCURVE);
	if (!m_pFtr) return;

	CDlgDataSource *pDS = GETDS(m_pEditor);

	//判断是否需要在画线的时候就显示符号
	m_bNeedDrawSymbol = FALSE;
	CPtrArray arrSymbols;
	gpCfgLibMan->GetSymbol(pDS, m_pFtr, arrSymbols, pLayer->GetName());
	for (int i = 0; i < arrSymbols.GetSize(); i++)
	{
		CSymbol *pSym = (CSymbol*)arrSymbols[i];
		int type = pSym->GetType();
		if (type == SYMTYPE_SCALE_Cell || type == SYMTYPE_SCALE_DiShangYaoDong ||
			type == SYMTYPE_SCALE_ChuRuKou)
		{
			m_bNeedDrawSymbol = TRUE;
		}
		delete pSym;
	}

	m_pDrawProc = new CDrawCurveProcedure;
	if (!m_pDrawProc)return;

	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pFtr->GetGeometry();
	m_pDrawProc->m_compress.SetLimit(0.1);
	m_pDrawProc->m_compress.m_lfScale = GETCURSCALE(m_pEditor)*1e-3;
	m_pDrawProc->m_layCol = pLayer->GetColor();
	m_pDrawProc->Start();
	CDrawCommand::Start();
	if (m_pDrawProc->m_nCurPenCode == penStream)
	{
		m_pDrawProc->m_compress.BeginCompress();
		//		m_bRectify = FALSE;
	}

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}

void CDlgDrawCurveCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}
void CDlgDrawCurveCommand::Finish()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	if (m_pEditor)
	{
		if (m_pFtr)
		{
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);
			if (bAnnotToText)
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
				//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if (pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);
				if (pLayer != NULL && pTextLayer != NULL && pTempl != NULL)
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor), m_pFtr, arrAnnots);
					CUndoFtrs undo(m_pEditor, Name());
					for (int i = 0; i < arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType() == SYMTYPE_ANNOTATION)
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr, tab);
							if (pAnnot->ExtractGeoText(m_pFtr, parr, tab, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()))
							{
								for (int j = 0; j < parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);
									PDOC(m_pEditor)->AddObject(pNewFtr, pTextLayer->GetID());
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
								}
							}
						}
					}
					undo.Commit();
				}
				if (pTempl != NULL)delete pTempl;
			}
		}
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}
	CDrawCurveCommand::Finish();
}

void CDlgDrawCurveCommand::Abort()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	CDrawCurveCommand::Abort();
}

BOOL CDlgDrawCurveCommand::IsEndSnapped()
{
	if (!m_pEditor)
		return FALSE;

	if (!PDOC(m_pEditor)->m_snap.bOpen())
		return FALSE;

	SNAPITEM item = PDOC(m_pEditor)->m_snap.GetFirstSnapResult();
	if (!item.IsValid())
		return FALSE;

	if (item.nSnapMode == CSnap::modeNearPoint ||
		item.nSnapMode == CSnap::modeKeyPoint ||
		item.nSnapMode == CSnap::modeMidPoint ||
		item.nSnapMode == CSnap::modeIntersect ||
		item.nSnapMode == CSnap::modePerpPoint ||
		item.nSnapMode == CSnap::modeTangPoint ||
		item.nSnapMode == CSnap::modeEndPoint)
		return TRUE;

	return FALSE;
}

CProcedure *CDlgDrawCurveCommand::GetActiveSonProc(int nMsgType)
{
	if (nMsgType == msgPtClick || nMsgType == msgPtMove || nMsgType == msgPtReset)
		return NULL;

	return m_pDrawProc;
}

void CDlgDrawCurveCommand::PtReset(PT_3D &pt)
{
	if (!m_pDrawProc || !m_pDrawProc->m_pGeoCurve)  return;

	if (m_pDrawProc->m_nCurPenCode == penStream)
	{
		if (IsEndSnapped())
			m_pDrawProc->m_nEndSnapped |= 2;
		else
			m_pDrawProc->m_nEndSnapped &= (~2);
	}

	m_pDrawProc->PtReset(pt);
}

void CDlgDrawCurveCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pDrawProc)  return;

	ASSERT(m_pDrawProc == son);

	if (!m_pDrawProc->m_pGeoCurve)return;

	m_pDrawProc->PtClick(pt, flag);

	if (m_bNeedDrawSymbol)
		m_pEditor->UpdateDrag(ud_SetConstDrag, NULL);

	if (m_pDrawProc->m_pGeoCurve->GetDataPointSum() == 0)
	{
		m_pDrawProc->m_nEndSnapped = IsEndSnapped() ? 1 : 0;
	}
	else
	{
		if (IsEndSnapped())
			m_pDrawProc->m_nEndSnapped |= 2;
		else
			m_pDrawProc->m_nEndSnapped &= (~2);
	}

	if (m_pDrawProc->m_arrPts.GetSize() == 1/*m_pDrawProc->m_pGeoCurve->GetDataPointSum() == 1*/)
	{
		if (PDOC(m_pEditor)->IsAutoSetAnchor())
		{
			PDOC(m_pEditor)->SetAnchorPoint(m_pDrawProc->m_arrPts[0]);
		}
	}
}

void CDlgDrawCurveCommand::PtMove(PT_3D &pt)
{
	if (m_pDrawProc)
		m_pDrawProc->PtMove(pt);

	if (m_bNeedDrawSymbol && m_pDrawProc)
	{
		CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
		if (!pLayer) return;

		CDlgDataSource *pDS = GETDS(m_pEditor);
		CFeature *pFtr = m_pFtr->Clone();
		CArray<PT_3DEX, PT_3DEX> arrPts;
		arrPts.Copy(m_pDrawProc->m_arrPts);
		arrPts.Add(PT_3DEX(pt, penLine));
		pFtr->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize());

		GrBuffer buf;
		pDS->DrawFeature(pFtr, &buf, FALSE, 0, pLayer->GetName());
		delete pFtr;

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
	}
}


void CDlgDrawCurveCommand::OnSonEnd(CProcedure *son)
{
	CDrawCurveCommand::OnSonEnd(son);
}

DrawingInfo CDlgDrawCurveCommand::GetCurDrawingInfo()
{
	if (!m_pFtr || !m_pDrawProc)return DrawingInfo();
	CArray<PT_3DEX, PT_3DEX> pts;
	pts.Append(m_pDrawProc->m_arrAllPts);
	pts.Append(m_pDrawProc->m_arrPts);
	return	DrawingInfo(m_pFtr, pts);
}




IMPLEMENT_DYNAMIC(CDlgDrawCurveCommand_HuNan, CDlgDrawCurveCommand)
CDlgDrawCurveCommand_HuNan::CDlgDrawCurveCommand_HuNan()
{
	m_bRightAngleMode = FALSE;
	m_nStep = 0;
}

CDlgDrawCurveCommand_HuNan:: ~CDlgDrawCurveCommand_HuNan()
{

}


void CDlgDrawCurveCommand_HuNan::Finish()
{

	CDrawCurveCommand::Finish();
}


CString CDlgDrawCurveCommand_HuNan::Name()
{
	return StrFromResID(IDS_CMDNAME_DRAWCURVE_HUNAN);
}


BOOL CDlgDrawCurveCommand_HuNan::SnapRightAngle(PT_3D pt)
{
	CDlgDoc *pDoc = PDOC(m_pEditor);
	CCoordWnd cw = m_pEditor->GetCoordWnd();
	PT_4D cltpt;
	cw.m_pSearchCS->GroundToClient(&pt, &cltpt);
	double r = pDoc->m_snap.GetSnapRadius()*cw.GetScaleOfSearchCSToViewCS();
	Envelope e;
	e.CreateFromPtAndRadius(cltpt, r);

	CPFeature pFtr = pDoc->GetDataQuery()->FindNearestObject(cltpt, r, cw.m_pSearchCS);
	if (!pFtr) return FALSE;
	CGeometry *pGeo = pFtr->GetGeometry();
	if (!pGeo) return FALSE;

	PT_3D ptline0, ptline1;

	if (pGeo->FindNearestBaseLine(cltpt, e, cw.m_pSearchCS, &ptline0, &ptline1, NULL))
	{
		PT_3D pt2;
		PT_3D pt3 = pt;

		GraphAPI::GGetNearestDisOfPtToLine(ptline0.x, ptline0.y,
			ptline1.x, ptline1.y, pt.x, pt.y, &pt2.x, &pt2.y, false);

		pt.x = pt2.x; pt.y = pt2.y;

		//在折角的地方，究竟是前一线段还是后一线段，有歧义
		PT_KEYCTRL kc = pGeo->FindNearestKeyCtrlPt(cltpt, r, cw.m_pSearchCS, 1);
		if (kc.type == PT_KEYCTRL::typeKey && kc.index >= 0)
		{
			PT_3D pt4 = pGeo->GetDataPoint(kc.index);
			if (GraphAPI::GIsEqual2DPoint(&pt2, &pt4))
			{
				BOOL bMidPoint = (kc.index > 0 && kc.index < pGeo->GetDataPointSum() - 1);
				BOOL bClosedPoint = ((kc.index == 0 || kc.index == pGeo->GetDataPointSum() - 1) &&
					((pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)pGeo)->IsClosed() && pGeo->GetDataPointSum() >= 4) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) && pGeo->GetDataPointSum() >= 3));

				if (bMidPoint)
				{
					PT_3D pt5 = pGeo->GetDataPoint(kc.index - 1);
					PT_3D pt6 = pGeo->GetDataPoint(kc.index + 1);

					PT_3D pt7, pt8;
					pt7.x = pt4.x - (pt5.y - pt4.y); pt7.y = pt4.y + (pt5.x - pt4.x);
					pt8.x = pt4.x - (pt6.y - pt4.y); pt8.y = pt4.y + (pt6.x - pt4.x);

					double dis1 = GraphAPI::GGetNearestDisOfPtToLine(pt4.x, pt4.y,
						pt7.x, pt7.y, pt3.x, pt3.y);
					double dis2 = GraphAPI::GGetNearestDisOfPtToLine(pt4.x, pt4.y,
						pt8.x, pt8.y, pt3.x, pt3.y);

					if (dis1 < dis2)
					{
						ptline0 = pt5; ptline1 = pt4;
					}
					else
					{
						ptline0 = pt6; ptline1 = pt4;
					}

				}
				else if (bClosedPoint)
				{
					PT_3D pt5, pt6;

					if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
					{
						pt5 = pGeo->GetDataPoint(pGeo->GetDataPointSum() - 2);
						pt6 = pGeo->GetDataPoint(1);
					}
					else
					{
						pt5 = pGeo->GetDataPoint(pGeo->GetDataPointSum() - 1);
						pt6 = pGeo->GetDataPoint(1);
					}

					PT_3D pt7, pt8;
					pt7.x = pt4.x - (pt5.y - pt4.y); pt7.y = pt4.y + (pt5.x - pt4.x);
					pt8.x = pt4.x - (pt6.y - pt4.y); pt8.y = pt4.y + (pt6.x - pt4.x);

					double dis1 = GraphAPI::GGetNearestDisOfPtToLine(pt4.x, pt4.y,
						pt7.x, pt7.y, pt3.x, pt3.y);
					double dis2 = GraphAPI::GGetNearestDisOfPtToLine(pt4.x, pt4.y,
						pt8.x, pt8.y, pt3.x, pt3.y);

					if (dis1 < dis2)
					{
						ptline0 = pt5; ptline1 = pt4;
					}
					else
					{
						ptline0 = pt6; ptline1 = pt4;
					}

				}
			}
		}

		CArray<PT_3DEX, PT_3DEX>& m_arrPts = m_pDrawProc->m_arrPts;

		m_arrPts[0] = PT_3DEX(pt, penLine);

		PT_3D ptPerpend;

		double dx = ptline1.x - pt.x, dy = ptline1.y - pt.y;
		ptPerpend.x = pt.x - dy;
		ptPerpend.y = pt.y + dx;
		ptPerpend.z = pt.z;

		m_arrPts.Add(PT_3DEX(ptPerpend, penLine));

		return TRUE;
	}

	return FALSE;
}


void CDlgDrawCurveCommand_HuNan::PtClick(PT_3D &pt, int flag)
{
	if (!m_pDrawProc)  return;

	if (!m_pDrawProc->m_pGeoCurve)return;

	if (m_bRightAngleMode)
	{
		if (IsProcFinished(this))return;

		GotoState(PROCSTATE_PROCESSING);

		m_pDrawProc->m_nCurPenCode = penLine;

		if (m_nStep == 0)
		{
			m_pDrawProc->PtClick(pt, flag);
			m_nStep = 1;

			if (SnapRightAngle(pt))
			{
				m_nStep = 2;
			}
		}
		else if (m_nStep == 1)
		{
			CArray<PT_3DEX, PT_3DEX>& m_arrPts = m_pDrawProc->m_arrPts;

			PT_3DEX expt(pt, penLine);
			m_arrPts.Add(expt);
			m_nStep++;
		}
		else
		{
			CArray<PT_3DEX, PT_3DEX>& m_arrPts = m_pDrawProc->m_arrPts;

			int size = m_arrPts.GetSize();
			PT_3DEX expt0;
			expt0.z = pt.z;
			expt0.pencode = penLine;

			GraphAPI::GGetPerpendicular3D(m_arrPts[size - 2].x, m_arrPts[size - 2].y, m_arrPts[size - 2].z, m_arrPts[size - 1].x, m_arrPts[size - 1].y, m_arrPts[size - 1].z,
				pt.x, pt.y, pt.z, &(expt0.x), &(expt0.y), &(expt0.z));

			m_arrPts[size - 1] = expt0;

			PT_3DEX expt(pt, penLine);
			m_arrPts.Add(expt);
			size = m_arrPts.GetSize();
			GrBuffer abuf;
			abuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(), 0);
			abuf.MoveTo(&m_arrPts[size - 3]);
			abuf.LineTo(&m_arrPts[size - 2]);
			abuf.End();
			m_pEditor->UpdateDrag(ud_AddConstDrag, &abuf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);

			if (IsEndSnapped())
				m_pDrawProc->m_nEndSnapped |= 2;
			else
				m_pDrawProc->m_nEndSnapped &= (~2);
		}

		GotoState(PROCSTATE_PROCESSING);
		CDrawCommand::PtClick(pt, flag);
	}
	else
	{
		CDlgDrawCurveCommand::PtClick(pt, flag);
	}
}

void CDlgDrawCurveCommand_HuNan::PtMove(PT_3D &pt)
{
	if (m_bRightAngleMode)
	{
		if (!m_pDrawProc)
			return;

		if (!m_pFtr || IsProcFinished(this))return;
		if (m_nStep < 1) return;

		CArray<PT_3DEX, PT_3DEX>& m_arrPts = m_pDrawProc->m_arrPts, arrPts0;
		PT_3DEX expt(pt, penLine);

		arrPts0.Copy(m_arrPts);

		if (m_nStep == 1)
		{
			m_arrPts.Add(expt);
		}
		if (m_nStep > 1)
		{
			int size;
			if ((size = m_arrPts.GetSize()) < 2)
			{
				return;
			}
			PT_3DEX expt3;
			expt3.z = pt.z;
			expt3.pencode = penLine;

			GraphAPI::GGetPerpendicular3D(m_arrPts[size - 2].x, m_arrPts[size - 2].y, m_arrPts[size - 2].z, m_arrPts[size - 1].x, m_arrPts[size - 1].y, m_arrPts[size - 1].z,
				pt.x, pt.y, pt.z, &(expt3.x), &(expt3.y), &(expt3.z));

			m_arrPts[size - 1] = expt3;
			m_arrPts.Add(expt);
		}
		GrBuffer vbuf;
		if (m_arrPts.GetSize() >= 3)
		{
			int num = m_arrPts.GetSize();
			vbuf.BeginLineString(0, 0);

			vbuf.MoveTo(&m_arrPts[num - 3]);
			vbuf.LineTo(&m_arrPts.GetAt(num - 2));
			vbuf.LineTo(&m_arrPts.GetAt(num - 1));
			vbuf.LineTo(&pt);
			if (m_pDrawProc->m_bClosed)
				vbuf.LineTo(&m_arrPts[0]);

			vbuf.End();
			m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
		}
		else
		{
			int num = m_arrPts.GetSize();
			vbuf.BeginLineString(0, 0);
			vbuf.MoveTo(&m_arrPts[num - 2]);
			vbuf.LineTo(&m_arrPts[num - 1]);
			vbuf.End();
			m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
		}
		m_arrPts.Copy(arrPts0);
		CDrawCommand::PtMove(pt);
	}
	else
	{
		CDlgDrawCurveCommand::PtMove(pt);
	}
}



void CDlgDrawCurveCommand_HuNan::PtReset(PT_3D &pt)
{
	if (!m_pFtr || IsProcFinished(this))return;

	if (m_bRightAngleMode)
	{
		if (!m_pDrawProc)
		{
			Abort();
			return;
		}

		CArray<PT_3DEX, PT_3DEX>& m_arrPts = m_pDrawProc->m_arrPts;

		if (m_arrPts.GetSize() <= 2)
		{
			Abort();
			return;
		}

		int size = m_arrPts.GetSize();

		if (m_pDrawProc->m_bClosed && !(m_pDrawProc->m_bOpenIfSnapped && m_pDrawProc->m_nEndSnapped == 3) && (size % 2) == 0)
		{
			PT_3DEX expt3;
			expt3.z = m_arrPts[size - 1].z;
			expt3.pencode = penLine;

			GraphAPI::GGetPerpendicular3D(m_arrPts[size - 3].x, m_arrPts[size - 3].y, m_arrPts[size - 3].z, m_arrPts[size - 2].x, m_arrPts[size - 2].y, m_arrPts[size - 2].z,
				m_arrPts[size - 1].x, m_arrPts[size - 1].y, m_arrPts[size - 1].z, &(expt3.x), &(expt3.y), &(expt3.z));

			m_arrPts[size - 2] = expt3;

			PT_3DEX ptex;
			ptex.pencode = penLine;

			ptex.z = m_arrPts[size - 1].z;

			GraphAPI::GGetRightAnglePoint(m_arrPts[1].x, m_arrPts[1].y, m_arrPts[0].x, m_arrPts[0].y, m_arrPts[size - 1].x, m_arrPts[size - 1].y, &ptex.x, &ptex.y);
			m_arrPts.Add(ptex);

			m_arrPts[size - 1] = ptex;

			m_arrPts.Add(m_arrPts[0]);

		}
		else
		{
			PT_3DEX expt3;
			expt3.z = m_arrPts[size - 1].z;
			expt3.pencode = penLine;

			GraphAPI::GGetPerpendicular3D(m_arrPts[size - 3].x, m_arrPts[size - 3].y, m_arrPts[size - 3].z, m_arrPts[size - 2].x, m_arrPts[size - 2].y, m_arrPts[size - 2].z,
				m_arrPts[size - 1].x, m_arrPts[size - 1].y, m_arrPts[size - 1].z, &(expt3.x), &(expt3.y), &(expt3.z));

			m_arrPts[size - 2] = expt3;
		}
		m_pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(), m_arrPts.GetSize());
		m_pEditor->UpdateDrag(ud_ClearDrag);

		if (!AddObject(m_pFtr))
		{
			Abort();
			return;
		}

		CUndoFtrs undo(m_pEditor, Name());
		undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
		undo.Commit();
		m_pEditor->RefreshView();

		Finish();

		CDrawCommand::PtReset(pt);
	}
	else
	{
		CDlgDrawCurveCommand::PtReset(pt);
	}

}


void CDlgDrawCurveCommand_HuNan::Back()
{
	if (!m_pFtr)
	{
		return;
	}
	if (m_bRightAngleMode)
	{
		if (!m_pDrawProc)
			return;

		CArray<PT_3DEX, PT_3DEX>& m_arrPts = m_pDrawProc->m_arrPts;

		int num = m_arrPts.GetSize();
		if (num <= 1)
		{
			Abort();
			m_arrPts.RemoveAll();
			m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
			m_pEditor->RefreshView();
			return;
		}
		if (num > 3)
		{
			PT_3D pt;
			pt = m_arrPts.GetAt(num - 1);
			GrBuffer vbuf;
			vbuf.BeginLineString(0, 0);

			vbuf.MoveTo(&m_arrPts[num - 3]);
			vbuf.LineTo(&m_arrPts.GetAt(num - 2));
			vbuf.LineTo(&m_arrPts.GetAt(num - 1));
			vbuf.LineTo(&pt);
			if (m_pDrawProc->m_bClosed)
				vbuf.LineTo(&m_arrPts[0]);

			PDOC(m_pEditor)->UpdateAllViews(NULL, hc_SetCrossPos, (CObject*)&pt);
			m_arrPts.RemoveAt(num - 2, 2);
			GrBuffer cbuf;
			cbuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(), 0);
			cbuf.Lines(m_arrPts.GetData(), m_arrPts.GetSize() - 1, sizeof(PT_3DEX));
			cbuf.End();
			//	pObj->Draw(&vbuf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag, &cbuf);

			//	delete pObj;}
			m_pEditor->RefreshView();
		}
		else
		{
			m_nStep = 1;
			PT_3D pt;
			pt = m_arrPts.GetAt(num - 1);
			GrBuffer vbuf;
			vbuf.BeginLineString(0, 0);
			vbuf.MoveTo(&m_arrPts.GetAt(num - 1));
			vbuf.LineTo(&m_arrPts.GetAt(num - 2));
			vbuf.End();
			m_arrPts.RemoveAt(m_arrPts.GetSize() - 1);
			PDOC(m_pEditor)->UpdateAllViews(NULL, hc_SetCrossPos, (CObject*)&pt);
			m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag, NULL);
			m_pEditor->RefreshView();
		}
		CDrawCommand::Back();
	}
	else
	{
		CDlgDrawCurveCommand::Back();
	}
}

void CDlgDrawCurveCommand_HuNan::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var = NULL;
	if (m_bRightAngleMode)
	{
		if (tab.GetValue(0, PF_PENCODE, var))
			return;

		if (tab.GetValue(0, PF_NODEWID, var))
			return;

		if (tab.GetValue(0, PF_NODETYPE, var))
			return;

		if (tab.GetValue(0, PF_TRACKLINEACCKEY, var))
			return;

		if (tab.GetValue(0, PF_RECTIFY, var))
			return;

		if (tab.GetValue(0, PF_RECTIFYACCKEY, var))
			return;

		if (tab.GetValue(0, PF_REVERSEACCKEY, var))
			return;

		if (tab.GetValue(0, PF_TOLER, var))
			return;

		if (tab.GetValue(0, PF_AUTOCLOSETOLER, var))
			return;

		if (tab.GetValue(0, PF_POSTSMOOTH, var))
			return;

		if (tab.GetValue(0, PF_FASTCURVE, var))
			return;

		if (tab.GetValue(0, PF_POINT, var))
			return;

		// 通过长度和角度指定点
		if (tab.GetValue(0, PF_LENGTH, var))
			return;

		if (tab.GetValue(0, PF_MULPTBUILDPTACCKEY, var))
			return;

		if (tab.GetValue(0, PF_BUILDPTTOLER, var))
			return;
	}

	CDlgDrawCurveCommand::SetParams(tab, bInit);

	if (tab.GetValue(0, "RightAngle", var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 'x' || ch == 'X')
		{
			m_bRightAngleMode = !m_bRightAngleMode;
		}

		if (m_pDrawProc && m_pDrawProc->m_arrPts.GetSize() == 0)
		{
			m_nStep = 0;
		}
		else if (m_pDrawProc && m_pDrawProc->m_arrPts.GetSize() == 1)
		{
			m_nStep = 1;
		}
		else if (m_pDrawProc && m_pDrawProc->m_arrPts.GetSize() > 1)
		{
			m_nStep = 2;
		}

		if (m_bRightAngleMode && m_pDrawProc)
		{
			CArray<PT_3DEX, PT_3DEX>& m_arrPts = m_pDrawProc->m_arrPts;

			int num = m_arrPts.GetSize();
			if (num >= 3)
			{
				PT_3D pt = m_pEditor->GetCoordWnd().m_ptGrd;
				GrBuffer vbuf;
				vbuf.BeginLineString(0, 0);
				vbuf.MoveTo(&m_arrPts.GetAt(num - 2));
				vbuf.LineTo(&m_arrPts.GetAt(num - 1));
				vbuf.LineTo(&pt);
				if (m_pDrawProc->m_bClosed)
					vbuf.LineTo(&m_arrPts[0]);

				vbuf.End();

				GrBuffer cbuf;
				cbuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(), 0);
				cbuf.Lines(m_arrPts.GetData(), num - 1, sizeof(PT_3DEX));
				cbuf.End();
				m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
				m_pEditor->UpdateDrag(ud_SetConstDrag, &cbuf);

				m_pEditor->RefreshView();
			}
			else if (num == 2)
			{
				PT_3D pt = m_pEditor->GetCoordWnd().m_ptGrd;
				GrBuffer vbuf;
				vbuf.BeginLineString(0, 0);
				vbuf.MoveTo(&m_arrPts.GetAt(num - 2));
				vbuf.LineTo(&m_arrPts.GetAt(num - 1));
				vbuf.LineTo(&pt);
				vbuf.End();

				m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
				m_pEditor->UpdateDrag(ud_SetConstDrag, NULL);
				m_pEditor->RefreshView();
			}
			else
			{
				m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
				m_pEditor->UpdateDrag(ud_SetConstDrag, NULL);
				m_pEditor->RefreshView();
			}
		}
		else if (!m_bRightAngleMode && m_pDrawProc)
		{
			CArray<PT_3DEX, PT_3DEX>& m_arrPts = m_pDrawProc->m_arrPts;

			m_pDrawProc->m_pGeoCurve->CreateShape(m_arrPts.GetData(), m_arrPts.GetSize());
			GrBuffer cbuf;
			m_pDrawProc->m_pGeoCurve->GetShape()->GetConstGrBuffer(&cbuf);
			m_pEditor->UpdateDrag(ud_SetConstDrag, &cbuf);
			m_pEditor->RefreshView();
		}
	}

}


//////////////////////////////////////////////////////////////////////
// CDrawDHeightPointCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDrawDHeightPointCommand, CDrawCommand)

CDrawDHeightPointCommand::CDrawDHeightPointCommand()
{
	m_pFtr = NULL;
	m_pSel = NULL;
	strcat(m_strRegPath, "\\DHeightPoint");

}

CDrawDHeightPointCommand::~CDrawDHeightPointCommand()
{
	if (m_pFtr)
	{
		delete m_pFtr;
	}
	if (m_pSel) delete m_pSel;
}



CString CDrawDHeightPointCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_DRAWDHEIGHTPOINT);
}

void CDrawDHeightPointCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOCURVE);
	if (!m_pFtr) return;
	CDrawCommand::Start();

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);

	m_pEditor->CloseSelector();

}

void CDrawDHeightPointCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}
void CDrawDHeightPointCommand::Finish()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);

	if (m_pEditor)
	{
		if (m_pFtr)
		{
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);

			if (bAnnotToText)
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
				//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if (pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);
				if (pLayer != NULL && pTextLayer != NULL && pTempl != NULL)
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor), m_pFtr, arrAnnots);
					CUndoFtrs undo(m_pEditor, Name());
					for (int i = 0; i < arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType() == SYMTYPE_ANNOTATION)
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr, tab);
							if (pAnnot->ExtractGeoText(m_pFtr, parr, tab, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()))
							{
								for (int j = 0; j < parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);
									PDOC(m_pEditor)->AddObject(pNewFtr, pTextLayer->GetID());

									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));

								}
							}
						}

					}
					undo.Commit();
				}
				if (pTempl != NULL)delete pTempl;
			}
		}
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}

	CDrawCommand::Finish();
}

DrawingInfo CDrawDHeightPointCommand::GetCurDrawingInfo()
{
	return DrawingInfo(m_pFtr, m_arrPts);
}

int  CDrawDHeightPointCommand::GetCurPenCode()
{
	return penLine;
}

void CDrawDHeightPointCommand::Abort()
{
	m_pEditor->UpdateDrag(ud_ClearDrag);
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);

	m_pEditor->RefreshView();

	CDrawCommand::Abort();
}

void CDrawDHeightPointCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pFtr || GetState() == PROCSTATE_FINISHED)return;

	GotoState(PROCSTATE_PROCESSING);

	PT_3DEX expt(pt, penLine);
	m_arrPts.Add(expt);

	if (m_arrPts.GetSize() == 2)
	{
		if (!m_pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(), m_arrPts.GetSize())) return;

		if (!AddObject(m_pFtr))
		{
			Abort();
			return;
		}

		CUndoFtrs undo(m_pEditor, Name());
		undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
		undo.Commit();

		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pFtr = NULL;
		Finish();
		return;
	}

	if (PDOC(m_pEditor)->IsAutoSetAnchor() && m_pFtr->GetGeometry()
		&& (m_pFtr->GetGeometry()->GetDataPointSum() == 1))
	{
		PDOC(m_pEditor)->SetAnchorPoint(pt);
	}

	CDrawCommand::PtClick(pt, flag);
}

void CDrawDHeightPointCommand::PtMove(PT_3D &pt)
{
	if (!m_pFtr || GetState() == PROCSTATE_FINISHED)return;

	if (m_arrPts.GetSize() == 1)
	{
		GrBuffer buf;
		buf.BeginLineString(0, 0);
		buf.MoveTo(&m_arrPts[0]);
		buf.LineTo(&pt);
		buf.End();

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
	}
	CDrawCommand::PtMove(pt);
}

IMPLEMENT_DYNAMIC(CDlgDrawSurfaceCommand, CDrawSurfaceCommand)
void CDlgDrawSurfaceCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOSURFACE);
	if (!m_pFtr) return;

	CDlgDataSource *pDS = GETDS(m_pEditor);

	m_pDrawProc = new CDrawSurfaceProcedure;
	if (!m_pDrawProc)return;

	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_pGeoSurface = (CGeoSurface*)m_pFtr->GetGeometry();
	m_pDrawProc->Start();

	m_pDrawProc->m_compress.SetLimit(0.1);
	m_pDrawProc->m_compress.m_lfScale = GETCURSCALE(m_pEditor)*1e-3;
	m_pDrawProc->m_layCol = pLayer->GetColor();
	CDrawCommand::Start();
	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}

void CDlgDrawSurfaceCommand::Finish()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	if (m_pEditor)
	{
		if (m_pFtr)
		{
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);
			if (bAnnotToText)
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
				//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if (pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);
				if (pLayer != NULL && pTextLayer != NULL && pTempl != NULL)
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor), m_pFtr, arrAnnots);
					CUndoFtrs undo(m_pEditor, Name());
					for (int i = 0; i < arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType() == SYMTYPE_ANNOTATION)
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr, tab);
							if (pAnnot->ExtractGeoText(m_pFtr, parr, tab, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()))
							{
								for (int j = 0; j < parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);
									PDOC(m_pEditor)->AddObject(pNewFtr, pTextLayer->GetID());
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
								}
							}
						}
					}
					undo.Commit();
				}
				if (pTempl != NULL)delete pTempl;
			}
		}
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}
	CDrawSurfaceCommand::Finish();
}

CDlgDrawSurfaceCommand::~CDlgDrawSurfaceCommand()
{
	if (m_pSel)delete m_pSel;
}

void CDlgDrawSurfaceCommand::Abort()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	CDrawSurfaceCommand::Abort();
}

void CDlgDrawSurfaceCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}
CProcedure *CDlgDrawSurfaceCommand::GetActiveSonProc(int nMsgType)
{
	if (nMsgType == msgPtClick)
		return NULL;

	return m_pDrawProc;
}

void CDlgDrawSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	ASSERT(m_pDrawProc == son);

	if (m_pDrawProc)
	{
		m_pDrawProc->PtClick(pt, flag);
	}

	if (m_pDrawProc && (m_pDrawProc->m_arrPts.GetSize() == 1))
	{
		if (PDOC(m_pEditor)->IsAutoSetAnchor())
		{
			PDOC(m_pEditor)->SetAnchorPoint(m_pDrawProc->m_arrPts[0]);
		}
	}

}

void CDlgDrawSurfaceCommand::OnSonEnd(CProcedure *son)
{
	CDrawSurfaceCommand::OnSonEnd(son);
}

DrawingInfo CDlgDrawSurfaceCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	return DrawingInfo(m_pFtr, m_pDrawProc->m_arrPts);
}


IMPLEMENT_DYNAMIC(CDlgMoveCommand, CCommand)

CDlgMoveCommand::CDlgMoveCommand()
{
	m_nStep = 0;
	m_bKeepOld = FALSE;
	m_bMouseDefine = TRUE;
	m_bOnlyChangeZ = FALSE;
	m_lfDeltaX = 0;
	m_lfDeltaY = 0;
	m_lfDeltaZ = 0;
}


CDlgMoveCommand::~CDlgMoveCommand()
{
}


CString CDlgMoveCommand::Name()
{
	return StrFromLocalResID(IDS_CMDNAME_MOVE);
}


void CDlgMoveCommand::Start()
{
	m_pEditor->OpenSelector();
	m_nStep = 0;

	CEditCommand::Start();
}


void CDlgMoveCommand::Abort()
{
	m_pEditor->CloseSelector();
	m_nStep = -1;
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CEditCommand::Abort();
}

void CDlgMoveCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);

	_variant_t var;
	var = (bool)(m_bKeepOld);
	tab.AddValue(PF_KEEPOLD, &CVariantEx(var));
	var = (bool)(m_bMouseDefine);
	tab.AddValue(PF_MOUSEDEFINE, &CVariantEx(var));
	var = (double)m_lfDeltaX;
	tab.AddValue(PF_XOFFSET, &CVariantEx(var));
	var = (double)m_lfDeltaY;
	tab.AddValue(PF_YOFFSET, &CVariantEx(var));
	var = (double)m_lfDeltaZ;
	tab.AddValue(PF_ZOFFSET, &CVariantEx(var));
	var = (bool)(m_bOnlyChangeZ);
	tab.AddValue("bOnlyChangeZ", &CVariantEx(var));
}


void CDlgMoveCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("MoveCommand", StrFromLocalResID(IDS_CMDNAME_MOVE));

	param->BeginOptionParam(PF_KEEPOLD, StrFromResID(IDS_CMDPLANE_KEEPOLD));
	param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_bKeepOld);
	param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_bKeepOld);
	param->EndOptionParam();

	param->BeginOptionParam(PF_MOUSEDEFINE, StrFromResID(IDS_CMDTIP_MOUSEDEF_MOVE));
	param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_bMouseDefine);
	param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_bMouseDefine);
	param->EndOptionParam();

	if (!m_bMouseDefine || bForLoad)
	{
		param->AddParam(PF_XOFFSET, m_lfDeltaX, StrFromResID(IDS_CMDPLANE_DX));
		param->AddParam(PF_YOFFSET, m_lfDeltaY, StrFromResID(IDS_CMDPLANE_DY));
		param->AddParam(PF_ZOFFSET, m_lfDeltaZ, StrFromResID(IDS_CMDPLANE_DZ));
	}

	if(m_bMouseDefine || bForLoad)
	{
		param->AddParam("bOnlyChangeZ", (bool)(m_bOnlyChangeZ), StrFromResID(IDS_CMDPLANE_ONLY_Z));
	}
}



void CDlgMoveCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, PF_KEEPOLD, var))
	{
		m_bKeepOld = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_MOUSEDEFINE, var))
	{
		m_bMouseDefine = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_XOFFSET, var))
	{
		m_lfDeltaX = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_YOFFSET, var))
	{
		m_lfDeltaY = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_ZOFFSET, var))
	{
		m_lfDeltaZ = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if(tab.GetValue(0, "bOnlyChangeZ", var))
	{
		m_bOnlyChangeZ = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab);
}

void CDlgMoveCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		m_ptMoveStart = pt;
		int num = 0;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num > 0)
		{
			if (flag == SELSTAT_POINTSEL);
			else if (!CanGetSelObjs(flag))return;
			m_arrObjs.SetSize(num);
			memcpy(m_arrObjs.GetData(), handles, sizeof(FTR_HANDLE)*num);
			m_nStep = 1;

			m_pEditor->CloseSelector();
			GotoState(PROCSTATE_PROCESSING);
		}
	}
	else if (m_nStep == 1)
	{
		double dx = pt.x - m_ptMoveStart.x;
		double dy = pt.y - m_ptMoveStart.y;
		double dz = pt.z - m_ptMoveStart.z;

		if (!m_bMouseDefine)
		{
			dx = m_lfDeltaX; dy = m_lfDeltaY; dz = m_lfDeltaZ;
		}
		else if(m_bOnlyChangeZ)
		{
			dx = 0.0;
			dy = 0.0;
		}

		double matrix[16];
		Matrix44FromMove(dx, dy, dz, matrix);

		CArray<FTR_HANDLE, FTR_HANDLE> arrObjs;
		int num = m_arrObjs.GetSize();
		GProgressStart(num);
		for (int i = 0; i < num; i++)
		{
			GProgressStep();
			CFeature *pFtr = HandleToFtr(m_arrObjs.GetAt(i));

			if (m_bKeepOld)
			{
				pFtr = pFtr->Clone();
				pFtr->SetID(OUID());
				CGeometry *pGeo = pFtr->GetGeometry();
				pGeo->Transform(matrix);
				m_pEditor->AddObject(pFtr, m_pEditor->GetFtrLayerIDOfFtr(m_arrObjs.GetAt(i)));
				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_arrObjs.GetAt(i)), pFtr);
				arrObjs.Add(FTR_HANDLE(pFtr));
			}
			else
			{
				CGeometry *pGeo = pFtr->GetGeometry();
				m_pEditor->DeleteObject(m_arrObjs.GetAt(i), FALSE);
				pGeo->Transform(matrix);
				m_pEditor->RestoreObject(m_arrObjs.GetAt(i));
				//m_pEditor->UpdateObject(m_arrObjs.GetAt(i));				
				arrObjs.Add(m_arrObjs.GetAt(i));
			}

		}

		GProgressEnd();

		if (m_bKeepOld)
		{
			CUndoFtrs undo(m_pEditor, Name());
			undo.arrNewHandles.Copy(arrObjs);
			undo.Commit();
		}
		else
		{
			CUndoTransform undo(m_pEditor, Name());
			memcpy(undo.matrix, matrix, sizeof(matrix));
			undo.arrHandles.Copy(arrObjs);
			undo.Commit();
		}

		m_pEditor->OnSelectChanged(TRUE);
		m_pEditor->CloseSelector();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		Finish();
	}
	return;
}


void CDlgMoveCommand::PtMove(PT_3D &pt)
{
	if (m_bMouseDefine && m_nStep == 1)
	{
		GrBuffer buf;

		double dx = pt.x - m_ptMoveStart.x;
		double dy = pt.y - m_ptMoveStart.y;
		double dz = pt.z - m_ptMoveStart.z;

		if(m_bOnlyChangeZ)
		{
			dx = 0.0;
			dy = 0.0;
		}

		double matrix[16];
		Matrix44FromMove(dx, dy, dz, matrix);

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

		// 不在立体上显示点状地物符号
		UpdateViewType updateview = uv_AllView;

		int num = m_arrObjs.GetSize();
		for (int i = 0; i < num && num < 100; i++)
		{
			CFeature *pFtr = HandleToFtr(m_arrObjs.GetAt(i));//->Clone();

			CGeometry *pGeo = pFtr->GetGeometry();
			//pGeo->Transform(matrix);
			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
			{
				CPlotTextEx plot;
				TEXT_SETTINGS0  settings;
				((CGeoText*)pGeo)->GetSettings(&settings);

				settings.fHeight *= pDS->GetAnnotationScale();
				plot.SetSettings(&settings);

				plot.SetShape(((CGeoText*)pGeo)->GetShape());
				plot.SetText(((CGeoText*)pGeo)->GetText());
				plot.GetOutLineBuf(&buf);
			}
			else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
			{
				updateview = uv_VectorView;
				CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(m_arrObjs.GetAt(i)));
				if (pLayer)
				{
					pDS->DrawFeature(pFtr, &buf, TRUE, 0, pLayer->GetName());
				}
			}
			else
				pFtr->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());


			//delete pFtr;

		}

		buf.Transform(matrix);

		buf.BeginLineString(0, 0);
		buf.MoveTo(&m_ptMoveStart);
		buf.LineTo(&pt);

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf, updateview);

		if (updateview == uv_VectorView)
		{
			GrBuffer buf0;
			buf0.BeginLineString(0, 0);
			buf0.MoveTo(&m_ptMoveStart);
			buf0.LineTo(&pt);
			m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf0, uv_StereoView);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CDeleteCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDeleteCommand, CCommand)

CDeleteCommand::CDeleteCommand()
{
}

CDeleteCommand::~CDeleteCommand()
{
}

CString CDeleteCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_DEL);
}

void CDeleteCommand::Start()
{
	if (!m_pEditor)return;
	int numsel;
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);
	if (numsel > 0)
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();

		CCommand::Start();

		PT_3D pt;
		PtClick(pt, 0);
		PtClick(pt, 0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	CEditCommand::Start();
}

void CDeleteCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;
		EditStepOne();

	}

	if (m_nStep == 1)
	{
		CSelection* pSel = m_pEditor->GetSelection();

		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		CUndoFtrs undo(m_pEditor, Name());
		for (int i = num - 1; i >= 0; i--)
		{
			undo.arrOldHandles.Add(handles[i]);
		}
		for (i = num - 1; i >= 0; i--)
		{
			/*	pObj = m_pEditor->m_pDataSource->GetObjectByGUID(m_idsOld[i]);*/
			m_pEditor->DeleteObject(handles[i]);
		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		// 		for ( i=num-1; i>=0; i--)
		// 		{			
		// 		;//	((CDlgDoc*)m_pEditor)->UpdateAllViews(0,hc_DelObject,(CObject*)(HandleToFtr(handles[i])));
		// 		}


		Finish();
		m_nStep = 2;
	}

	CCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// CTextCoverCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTextCoverCommand, CCommand)

CTextCoverCommand::CTextCoverCommand()
{
	m_bSameColor = FALSE;
}

CTextCoverCommand::~CTextCoverCommand()
{
}

CString CTextCoverCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_TEXTCOVER);
}

void CTextCoverCommand::Start()
{
	if (!m_pEditor)return;

	m_bSameColor = FALSE;
	CEditCommand::Start();

	int numsel;
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);
	if (numsel > 0)
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();

		CCommand::Start();

		PT_3D pt;
		PtClick(pt, 0);
		PtClick(pt, 0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

}

void CTextCoverCommand::Abort()
{
	m_nExitCode = CMPEC_STARTOLD;
	m_pEditor->DeselectAll();
	CEditCommand::Abort();
}

void CTextCoverCommand::GetParams(CValueTable &tab)
{

	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (bool)(m_bSameColor);
	tab.AddValue(PF_TEXTCOVER_SAMECOLOR, &CVariantEx(var));
}

void CTextCoverCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("TextCoverCommand", StrFromLocalResID(IDS_CMDNAME_TEXTCOVER));
	param->AddParam(PF_TEXTCOVER_SAMECOLOR, bool(m_bSameColor), StrFromResID(IDS_CMDPLANE_SAMECOLOR));

}


void CTextCoverCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	//	bool bNeedRefresh = false;

	if (tab.GetValue(0, PF_TEXTCOVER_SAMECOLOR, var))
	{
		m_bSameColor = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}

void CTextCoverCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;
		EditStepOne();

	}

	if (m_nStep == 1)
	{
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		if (!pDS)
		{
			Abort();
			return;
		}

		CSelection* pSel = m_pEditor->GetSelection();

		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);

		CFtrArray arrSrcFtrs;
		for (int i = 0; i < num; i++)
		{
			CFeature *pFtr = (CFeature*)handles[i];
			if (pFtr && pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
			{
				arrSrcFtrs.Add(pFtr);
			}
		}

		if (arrSrcFtrs.GetSize() <= 0)
		{
			Abort();
			return;
		}

		CUndoModifyProperties undo(m_pEditor, Name());

		for (i = 0; i < arrSrcFtrs.GetSize(); i++)
		{
			undo.arrHandles.Add(FTR_HANDLE(arrSrcFtrs[i]));
			undo.oldVT.BeginAddValueItem();
			arrSrcFtrs[i]->WriteTo(undo.oldVT);
			undo.oldVT.EndAddValueItem();
		}

		// 压盖
		for (i = arrSrcFtrs.GetSize() - 1; i >= 0; i--)
		{
			CGeoText *pText = (CGeoText*)arrSrcFtrs[i]->GetGeometry();

			if (m_bSameColor)
			{
				long color = pText->GetColor();
				if (color == COLOUR_BYLAYER)
				{
					CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject((CPFeature)arrSrcFtrs[i]);
					if (pFtrLayer)
					{
						color = pFtrLayer->GetColor();
					}
				}

				GrBuffer buf;
				pText->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
				Envelope e = buf.GetEnvelope(), e1;
				e1 = e;

				PT_3D ptCen(e.CenterX(), e.CenterY(), e.CenterZ()), ptsch;
				m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&ptCen, &ptsch);
				e1.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS, 1);

				double r = sqrt(e1.Width()*e1.Width() + e1.Height()*e1.Height()) / 2;

				m_pEditor->GetDataQuery()->FindNearestObject(ptsch, r, m_pEditor->GetCoordWnd().m_pSearchCS);

				int num;
				const CPFeature * ppftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);
				if (!ppftr)
				{
					arrSrcFtrs.RemoveAt(i);
					continue;
				}

				for (int j = 0; j < num; j++)
				{
					CFeature *pFtr = ppftr[j];
					if (pFtr && pFtr != arrSrcFtrs[i])
					{
						long color0 = pFtr->GetGeometry()->GetColor();
						if (color0 == COLOUR_BYLAYER)
						{
							CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(pFtr);
							if (pFtrLayer)
							{
								color0 = pFtrLayer->GetColor();
							}
						}

						if (color == color0)
						{
							break;
						}

					}
				}

				if (j >= num)
				{
					arrSrcFtrs.RemoveAt(i);
					continue;
				}

			}

			if (pText)
			{
				pText->m_nCoverType = 1;
			}
		}

		// 置前
		if (arrSrcFtrs.GetSize() <= 0 || !pDS->DisplayTop(arrSrcFtrs.GetData(), arrSrcFtrs.GetSize()))
		{
			Abort();
			return;
		}

		for (i = 0; i < arrSrcFtrs.GetSize(); i++)
		{
			m_pEditor->DeleteObject(FTR_HANDLE(arrSrcFtrs[i]), FALSE);
			m_pEditor->RestoreObject(FTR_HANDLE(arrSrcFtrs[i]));

			undo.newVT.BeginAddValueItem();
			arrSrcFtrs[i]->WriteTo(undo.newVT);
			undo.newVT.EndAddValueItem();
		}

		undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();


		Finish();
		m_nStep = 2;
	}

	CCommand::PtClick(pt, flag);
}

//////////////////////////////////////////////////////////////////////
// CModifyCurveLengthCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CModifyCurveLengthCommand, CCommand)

CModifyCurveLengthCommand::CModifyCurveLengthCommand()
{
	m_fLength = 0;
}

CModifyCurveLengthCommand::~CModifyCurveLengthCommand()
{

}

CString CModifyCurveLengthCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CURVELENGTH);
}

void CModifyCurveLengthCommand::Start()
{
	if (!m_pEditor)return;

	m_fLength = 0;

	CEditCommand::Start();

	m_pEditor->DeselectAll();

	m_pEditor->OpenSelector(SELMODE_SINGLE);

}

void CModifyCurveLengthCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag, FALSE))return;
		EditStepOne();

		int num;
		const FTR_HANDLE* handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num == 1)
		{
			m_pSelFtr = HandleToFtr(handles[0]);
			CGeometry *pGeo = m_pSelFtr->GetGeometry();
			if (!pGeo)
			{
				Abort();
				return;
			}

			if (!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				Abort();
				return;
			}
			GOutPut(StrFromResID(IDS_CMDTIP_SELRESERVE));
		}

	}
	else if (m_nStep == 1)
	{
		int sum;
		const FTR_HANDLE* handles = m_pEditor->GetSelection()->GetSelectedObjs(sum);
		double fReserveLen = fabs(m_fLength);
		if (sum != 1 || handles[0] != FtrToHandle(m_pSelFtr) || fReserveLen < 1e-4)
		{
			Abort();
			return;
		}

		const CShapeLine *pShape = m_pSelFtr->GetGeometry()->GetShape();
		double len = pShape->GetLength();
		// 		if (fReserveLen > len)
		// 		{
		// 			Abort();
		// 			return;
		// 		}
		double len1 = pShape->GetLength(&pt);

		BOOL bProcessFirst = TRUE;
		if (len1 > len / 2)
		{
			bProcessFirst = FALSE;
		}

		CArray<PT_3DEX, PT_3DEX> pts, ret0, ret;
		pShape->GetPts(pts);
		int num = pts.GetSize();

		if (!bProcessFirst)
		{
			ret0.Copy(pts);
		}
		else
		{
			for (int i = num - 1; i >= 0; i--)
			{
				ret0.Add(pts[i]);
			}
		}
		//这里掉了个等号 忽略了相等的情况
		if (fReserveLen >= len)
		{
			ret.Copy(ret0);

			double len0 = GraphAPI::GGet2DDisOf2P(ret0[num - 2], ret0[num - 1]);
			double ratio = len0 / (len0 + fReserveLen - len);
			PT_3DEX retPt;
			retPt.x = ret0[num - 2].x + (ret0[num - 1].x - ret0[num - 2].x) / ratio;
			retPt.y = ret0[num - 2].y + (ret0[num - 1].y - ret0[num - 2].y) / ratio;
			retPt.z = ret0[num - 2].z + (ret0[num - 1].z - ret0[num - 2].z) / ratio;
			retPt.wid = ret0[num - 2].wid + ratio * (ret0[num - 1].wid - ret0[num - 2].wid);
			retPt.pencode = ret0[num - 1].pencode;

			ret.RemoveAt(num - 1);
			ret.Add(retPt);
		}
		else
		{
			double dis = 0;
			for (int i = 0; i < num - 1; i++)
			{
				dis += GraphAPI::GGet2DDisOf2P(ret0[i], ret0[i + 1]);
				ret.Add(ret0[i]);

				if (dis > fReserveLen)
				{
					break;
				}
			}

			if (i < num - 1)
			{
				double len0 = GraphAPI::GGet2DDisOf2P(ret0[i], ret0[i + 1]);
				double ratio = 1 - (dis - fReserveLen) / len0;
				PT_3DEX retPt;
				retPt.x = ret0[i].x + ratio * (ret0[i + 1].x - ret0[i].x);
				retPt.y = ret0[i].y + ratio * (ret0[i + 1].y - ret0[i].y);
				retPt.z = ret0[i].z + ratio * (ret0[i + 1].z - ret0[i].z);
				retPt.wid = ret0[i].wid + ratio * (ret0[i + 1].wid - ret0[i].wid);
				retPt.pencode = ret0[i + 1].pencode;

				ret.Add(retPt);
			}
		}

		if (bProcessFirst)
		{
			CArray<PT_3DEX, PT_3DEX> tmpRet;
			tmpRet.Copy(ret);
			ret.RemoveAll();
			for (int i = tmpRet.GetSize() - 1; i >= 0; i--)
			{
				ret.Add(tmpRet[i]);
			}
		}

		CUndoModifyProperties undo(m_pEditor, Name());

		undo.arrHandles.Add(FtrToHandle(m_pSelFtr));

		undo.oldVT.BeginAddValueItem();
		m_pSelFtr->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();

		m_pEditor->DeleteObject(FtrToHandle(m_pSelFtr), FALSE);

		if (!m_pSelFtr->GetGeometry()->CreateShape(ret.GetData(), ret.GetSize()))
		{
			Abort();
			return;
		}

		undo.newVT.BeginAddValueItem();
		m_pSelFtr->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();

		m_pEditor->RestoreObject(FtrToHandle(m_pSelFtr));

		undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		Finish();
		m_nStep = 2;
	}

	CCommand::PtClick(pt, flag);
}

void CModifyCurveLengthCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);

	_variant_t var;
	var = (double)(m_fLength);
	tab.AddValue(PF_RESERVELEN, &CVariantEx(var));

}


void CModifyCurveLengthCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ModifyCurveLength", StrFromLocalResID(IDS_CMDNAME_CURVELENGTH));

	param->AddParam(PF_RESERVELEN, (double)m_fLength, StrFromResID(IDS_CMDPLANE_RESERVELEN));

}



void CModifyCurveLengthCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, PF_RESERVELEN, var))
	{
		m_fLength = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab);
}


//////////////////////////////////////////////////////////////////////
// CMovetoLocalCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMovetoLocalCommand, CCommand)

CMovetoLocalCommand::CMovetoLocalCommand()
{
	m_bIncludeRefData = FALSE;
	m_bModifiedIncludeRef = FALSE;
	m_bOldRef = FALSE;
}

CMovetoLocalCommand::~CMovetoLocalCommand()
{
	CDlgDoc *pDoc = PDOC(m_pEditor);
	if (pDoc && m_bModifiedIncludeRef)
	{
		CDlgDataQuery *pDQ = (CDlgDataQuery*)pDoc->GetDataQuery();
		if (pDQ)
		{
			pDQ->SetFilterIncludeRefData(m_bOldRef);
		}
	}

}

CString CMovetoLocalCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_MOVETOLCALE);
}

void CMovetoLocalCommand::Start()
{
	if (!m_pEditor)return;

	m_bIncludeRefData = FALSE;
	m_bModifiedIncludeRef = FALSE;
	m_bOldRef = FALSE;

	int numsel;
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);
	if (numsel > 0)
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();

		CCommand::Start();

		PT_3D pt;
		PtClick(pt, 0);
		PtClick(pt, 0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	CEditCommand::Start();
}

CFtrLayer* CMovetoLocalCommand::GetFtrLayerOfObject(CFeature *pFtr, BOOL bIncRef)
{
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	CDataQuery *pDQ = PDOC(m_pEditor)->GetDataQuery();
	if (!pDS || !pDQ) return NULL;

	LONG_PTR info;

	if (!pDQ->GetObjectInfo(pFtr, 0, info))
		return NULL;

	if (!bIncRef && info != (LONG_PTR)pDS)
		return NULL;

	if (!pDQ->GetObjectInfo(pFtr, 1, info))
		return NULL;

	return (CFtrLayer*)(info);
}

void CMovetoLocalCommand::MovetoLocal(const FTR_HANDLE* handles, int num)
{
	CUndoBatchAction batchUndo(m_pEditor, Name());

	for (int i = 0; i < num; i++)
	{
		CFtrLayer *pFtrLayer = GetFtrLayerOfObject(HandleToFtr(handles[i]));
		// 当前数据源
		if (pFtrLayer)
		{
			CUndoModifyProperties undo(m_pEditor, Name());

			undo.arrHandles.Add(handles[i]);


			m_pEditor->DeleteObject(handles[i], FALSE);
			undo.oldVT.BeginAddValueItem();
			CPFeature(handles[i])->WriteTo(undo.oldVT);
			undo.oldVT.EndAddValueItem();

			CPFeature(handles[i])->SetCode("");

			m_pEditor->RestoreObject(handles[i]);
			//			if( !m_pEditor->UpdateObject(handles[i]) )
			//			{			
			//				continue;
			//			}


			undo.newVT.BeginAddValueItem();
			CPFeature(handles[i])->WriteTo(undo.newVT);
			undo.newVT.EndAddValueItem();

			batchUndo.AddAction(&undo);

		}
		// 参考数据源
		else
		{
			CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

			int layid = -1;

			CString strLayerName;
			CFtrLayer *pRefLayer = GetFtrLayerOfObject(HandleToFtr(handles[i]), TRUE);
			if (pRefLayer)
			{
				strLayerName = pRefLayer->GetName();
				CFtrLayer *pAddLayer = pDS->GetFtrLayer(strLayerName);
				if (pAddLayer)
				{
					layid = pAddLayer->GetID();
				}
			}

			CUndoFtrs undo(m_pEditor, Name());
			CFeature *pNewFtr = ((CPFeature)handles[i])->Clone();
			pNewFtr->SetID(OUID());
			pNewFtr->SetCode("");
			if (!m_pEditor->AddObject(pNewFtr, layid)) continue;

			undo.AddNewFeature(FTR_HANDLE(pNewFtr));

			batchUndo.AddAction(&undo);

		}


	}

	batchUndo.Commit();
}

void CMovetoLocalCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;
		EditStepOne();

	}

	if (m_nStep == 1)
	{
		CSelection* pSel = m_pEditor->GetSelection();

		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		MovetoLocal(handles, num);
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		Finish();
		m_nStep = 2;
	}

	CCommand::PtClick(pt, flag);
}

void CMovetoLocalCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);

	_variant_t var;
	var = (bool)(m_bIncludeRefData);
	tab.AddValue(PF_INCLUDE_REFDATA, &CVariantEx(var));

}


void CMovetoLocalCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("MovetoLocalCommand", StrFromLocalResID(IDS_CMDNAME_MOVETOLCALE));

	param->BeginOptionParam(PF_INCLUDE_REFDATA, StrFromResID(IDS_CMDPLANE_INCLUDEREFDATA));
	param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_bIncludeRefData);
	param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_bIncludeRefData);
	param->EndOptionParam();

}



void CMovetoLocalCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, PF_INCLUDE_REFDATA, var))
	{
		m_bIncludeRefData = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();

		CDlgDataQuery *pDQ = (CDlgDataQuery*)PDOC(m_pEditor)->GetDataQuery();
		if (pDQ)
		{
			BOOL bOld = pDQ->SetFilterIncludeRefData(m_bIncludeRefData);
			if (!m_bModifiedIncludeRef)
			{
				m_bOldRef = bOld;
				m_bModifiedIncludeRef = TRUE;
			}
		}
	}

	CEditCommand::SetParams(tab);
}




//////////////////////////////////////////////////////////////////////
// CSymbolLayoutCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSymbolLayoutCommand, CEditCommand)

CSymbolLayoutCommand::CSymbolLayoutCommand()
{
	m_pLayer = NULL;
	m_pFtrSymbol = NULL;
	m_pFtr = NULL;
}

CSymbolLayoutCommand::~CSymbolLayoutCommand()
{
	if (m_pFtr)
	{
		delete m_pFtr;
	}

	if (m_pFtrSymbol)
	{
		delete m_pFtrSymbol;
	}

}

CString CSymbolLayoutCommand::Name()
{
	return StrFromResID(IDS_SYMBOLLAYOUT);
}



void CSymbolLayoutCommand::Abort()
{
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();

	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	CDrawCommand::Abort();
}

void CSymbolLayoutCommand::Start()
{
	if (!m_pEditor)
	{
		return;
	}
	m_pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
	if (!m_pLayer)
	{
		GOutPut(StrFromResID(IDS_ERR_NOLAYER) + StrFromResID(IDS_DEFLAYER_NAMET));
		return;
	}
	m_pFtr = m_pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);
	if (!m_pFtr) return;
	m_nScale = PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale();
	CDrawCommand::Start();

	if (m_SymbolTableDlg.DoModal() != IDOK)
	{
		Abort();
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	{
		m_nTableColumnSpace.Add(m_SymbolTableDlg.m_nWidthCode*m_nScale);
		m_nTableColumnSpace.Add(m_SymbolTableDlg.m_nWidthLayerName*m_nScale);
		m_nTableColumnSpace.Add(m_SymbolTableDlg.m_nWidthSymbolMode*m_nScale);
		m_nTableColumnSpace.Add(m_SymbolTableDlg.m_nWidthSymbol*m_nScale);
		m_nTableColumnSpace.Add(m_SymbolTableDlg.m_nSymbolDiscribtion*m_nScale);
	}
	PT_3D pts[4];
	PDOC(m_pEditor)->GetDlgDataSource()->GetBound(pts, NULL, NULL);//获取四个边界点取第三个
	PT_3D pt(pts[3].x + m_SymbolTableDlg.m_nSpaceLeft, pts[3].y - m_SymbolTableDlg.m_nSpaceTop, 0);
	int nTableLine, nTableColumn;
	nTableLine = m_SymbolTableDlg.m_nHeight / m_SymbolTableDlg.m_nRowWidth;
	DrawTable(pt, nTableLine, 5, m_SymbolTableDlg.m_nTableNumber, m_SymbolTableDlg.m_nRowWidth*m_nScale, m_nTableColumnSpace);
	Finish();
	m_nExitCode = CMPEC_STARTOLD;
}

BOOL CSymbolLayoutCommand::GetPartColumSpace(CArray<double, double> &nTableSpace, int i, double &Total)// 寻找间隔点坐标
{
	if (nTableSpace.GetSize() <= 0 || i<0 || i>nTableSpace.GetSize())
	{
		return FALSE;
	}
	double Temp = 0;
	for (int j = 0; j < i; j++)
	{
		Temp = nTableSpace.GetAt(j);
		Total += Temp;
	}

	return TRUE;


}


int CSymbolLayoutCommand::FindIndexInSchemeLayerDefine(CArray<CSchemeLayerDefine*, CSchemeLayerDefine*> &arrPLayDefines, __int64 low, __int64 heigh, int &nSize)
{
	int nLayerSize = arrPLayDefines.GetSize();
	if (nLayerSize <= 0 || heigh - low < 0)
	{
		nSize = 0;
		return 0;
	}
	CSchemeLayerDefine *pTempSchemeLayerDefine = NULL, *pTempSchemeLayerDefine1 = NULL;
	int nIndexStart = 0, nIndexEnd = -1, nCount = 0;
	for (int i = 0; i < nLayerSize; i++)
	{
		pTempSchemeLayerDefine = arrPLayDefines.GetAt(i);
		if (i == nLayerSize - 1)//防止越界
		{
			pTempSchemeLayerDefine1 = arrPLayDefines.GetAt(i);
		}
		else
		{
			pTempSchemeLayerDefine1 = arrPLayDefines.GetAt(i + 1);

		}

		if (low >= pTempSchemeLayerDefine->GetLayerCode() && low <= pTempSchemeLayerDefine1->GetLayerCode())
		{
			nIndexStart = i + 1;
			if (low == pTempSchemeLayerDefine->GetLayerCode())
			{
				nIndexStart = i;
			}

		}

		if (heigh >= pTempSchemeLayerDefine->GetLayerCode() && heigh <= pTempSchemeLayerDefine1->GetLayerCode())
		{
			nIndexEnd = i;
			if (low == pTempSchemeLayerDefine1->GetLayerCode())
			{
				nIndexEnd = i + 1;
			}
		}

	}

	if (low > arrPLayDefines.GetAt(nLayerSize - 1)->GetLayerCode() || heigh < arrPLayDefines.GetAt(0)->GetLayerCode())//最小的大于最大的 最大的小于最小的
	{
		nIndexStart = -1;
		nIndexEnd = -1;
		nCount = 0;
	}

	if (low <= arrPLayDefines.GetAt(0)->GetLayerCode())
	{
		nIndexStart = 0;
	}

	if (heigh >= arrPLayDefines.GetAt(nLayerSize - 1)->GetLayerCode())
	{
		nIndexEnd = nLayerSize - 1;
	}

	nCount = nIndexEnd - nIndexStart + 1;
	nSize = nCount;

	return nIndexStart;


}


BOOL CSymbolLayoutCommand::SortSchemeLayerDefine(CArray<CSchemeLayerDefine*, CSchemeLayerDefine*> &arrPLayDefines)
{
	int nSize = arrPLayDefines.GetSize();
	if (nSize <= 0)
	{
		return FALSE;
	}
	CSchemeLayerDefine *pTmpSchLayDef = NULL;
	for (int i = 0; i < nSize - 1; i++)
	{
		for (int j = 0; j < nSize - i - 1; j++)
		{
			if (arrPLayDefines.GetAt(j)->GetLayerCode() > arrPLayDefines.GetAt(j + 1)->GetLayerCode())
			{
				pTmpSchLayDef = arrPLayDefines.GetAt(j);
				arrPLayDefines.SetAt(j, arrPLayDefines.GetAt(j + 1));
				arrPLayDefines.SetAt(j + 1, pTmpSchLayDef);
			}


		}

	}
	return TRUE;

}


void CSymbolLayoutCommand::DrawSymbolText(PT_3D &pt, CString StrTest, TEXT_SETTINGS0 sTxtSettings, CUndoFtrs &undo)
{
	m_pFtr = m_pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);

	if (!m_pFtr)
	{
		return;
	}
	((CGeoText*)m_pFtr->GetGeometry())->SetText(StrTest);
	((CGeoText*)m_pFtr->GetGeometry())->SetSettings(&sTxtSettings);

	//如果文本内容为空，则return;
	CString str = ((CGeoText*)m_pFtr->GetGeometry())->GetText();
	if (str.IsEmpty())
	{
		return;
	}

	TEXT_SETTINGS0 settings;
	((CGeoText*)m_pFtr->GetGeometry())->GetSettings(&settings);
	CArray<PT_3DEX, PT_3DEX> arr;
	CArray<PT_3DEX, PT_3DEX> arrPts;
	m_pFtr->GetGeometry()->GetShape(arr);
	PT_3DEX t;
	PT_3D retpt;
	retpt.x = pt.x;
	retpt.y = pt.y;
	retpt.z = pt.z;
	int nsize = arrPts.GetSize();
	t.x = retpt.x;
	t.y = retpt.y;
	t.z = retpt.z;
	t.pencode = penLine;
	arr.Add(t);
	m_pFtr->GetGeometry()->CreateShape(arr.GetData(), arr.GetSize());
	if (settings.nPlaceType == 0)//单点排列
	{
		if (!m_pEditor->AddObject(m_pFtr, m_pLayer->GetID()))
		{
			Abort();
			return;
		}

		//	CUndoFtrs undo(m_pEditor,Name());
		undo.arrNewHandles.Add(FtrToHandle(m_pFtr));

		//	undo.Commit();
	}

	m_pFtr = NULL;



}


void CSymbolLayoutCommand::DrawSymbolItem(int cx, int cy, const GrBuffer2d *pBuf, COLORREF col, COLORREF colBak)
{
	//创建内存设备
	CClientDC cdc(PDOC(m_pEditor)->GetCurActiveView());
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC);
	if (!hDC)return;

	HBITMAP hBmp = ::CreateCompatibleBitmap(cdc.m_hDC, cx, cy);
	if (!hBmp)
	{
		::DeleteDC(hDC);
		return;
	}

	CBitmap bm;
	bm.Attach(hBmp);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDC, hBmp);

	CBrush br(RGB(128, 128, 128));
	::FillRect(hDC, CRect(0, 0, cx, cy), (HBRUSH)br);
	//	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC,(HBRUSH)br);

	CPen pen;
	pen.CreatePen(PS_SOLID, 0, (COLORREF)0);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, (HPEN)pen);

	//计算变换系数
	CRect rect(2, 2, cx - 4, cy - 4);
	Envelope e = pBuf->GetEnvelope();

	float scalex = rect.Width() / (e.m_xh > e.m_xl ? (e.m_xh - e.m_xl) : 1e-10);
	float scaley = rect.Height() / (e.m_yh > e.m_yl ? (e.m_yh - e.m_yl) : 1e-10);
	float xoff = 0, yoff = 0;
	if (scalex > scaley)
	{
		xoff = cx / 2 - ((e.m_xh - e.m_xl) / 2 * scaley + rect.left);
		scalex = scaley;
	}
	else
	{
		yoff = cy / 2 - ((e.m_yh - e.m_yl) / 2 * scalex + rect.top);
	}

	double matrix[9] = {
		scalex, 0, -e.m_xl*scalex + rect.left + xoff + 0.5,
		0, -scalex, cy + e.m_yl*scalex - rect.top - yoff - 0.5,
		0, 0, 1
	};

	GDI_DrawGrBuffer2d(hDC, pBuf, TRUE, col, matrix, CSize(cx, cy), CRect(0, 0, cx, cy), colBak);

	::SelectObject(hDC, hOldPen);
	::SelectObject(hDC, hOldBmp);

	::DeleteDC(hDC);
	::DeleteObject(hBmp);

}

BOOL CSymbolLayoutCommand::CreateSymbolItem(CSchemeLayerDefine *pLayer, PT_3D &pt, double cx, double cy, int nLayerNum)
{
	if (!pLayer)
	{
		return FALSE;
	}
	int nSymbolNum = pLayer->GetSymbolCount();

	GrBuffer bufs;

	CGeometry *pGeo = m_pFtrSymbol->GetGeometry();
	if (!pGeo)
	{
		return FALSE;
	}
	int nGeoClass = pGeo->GetClassType();

	if (nGeoClass == CLS_GEOPOINT || nGeoClass == CLS_GEODIRPOINT)
	{
		pGeo->CreateShape(&PT_3DEX(pt.x + cx / 2, pt.y, 0, penMove, cy / 2), 1);
		//	pGeo->Draw(&bufs);
	}
	else if (nGeoClass == CLS_GEOPARALLEL)//双线
	{
		// 平行线有填充时填充在基线和辅助线之间
		PT_3DEX pts[2];

		{
			pt.x += cx / nLayerNum / 2;
			pts[0].x = pt.x - cx / nLayerNum / 4;
			pts[0].y = pt.y + cy / 4;
			pts[0].pencode = penLine;
			pts[1].x = pt.x + cx / (nLayerNum) / 4;
			pts[1].y = pt.y + cy / 4;
			pts[1].pencode = penLine;
		}

		pGeo->CreateShape(pts, 2);

		CGeoParallel *pParallele = (CGeoParallel*)pGeo;
		pParallele->SetWidth(-cy / 2);

	}
	else if (nGeoClass == CLS_GEOCURVE)//单线 含填充
	{
		// 检查是否有颜色或图元填充,有则添加四个点方便填充
		BOOL bHatch = FALSE;
		BOOL bArc = FALSE;
		for (int i = 0; i < nSymbolNum; i++)
		{
			CSymbol *pSym = pLayer->GetSymbol(i);
			int nType = pSym->GetType();
			if (nType == SYMTYPE_SCALETURNPLATETYPE || nType == SYMTYPE_SCALE_LiangChang || nType == SYMTYPE_SCALE_YouGuan)
			{
				bArc = TRUE;
			}
			else if (nType == SYMTYPE_SCALEFUNNELTYPE && (((CScaleFunneltype*)pSym)->m_nFunnelType == 1 || ((CScaleFunneltype*)pSym)->m_nFunnelType == 3))
			{
				bArc = TRUE;
			}

			if (nType == SYMTYPE_CELLHATCH || nType == SYMTYPE_COLORHATCH || nType == SYMTYPE_LINEHATCH || nType == SYMTYPE_DIAGONAL || nType == SYMTYPE_SCALE_DiShangYaoDong ||
				nType == SYMTYPE_SCALE_JianFangWu || nType == SYMTYPE_ANGBISECTORTYPE)
			{
				bHatch = TRUE;
				break;
			}
		}
		if (bArc)
		{
			pt.x += cx / 2;
			PT_3DEX pts[5];
			pts[0].x = pt.x - cy / nLayerNum / 2;
			pts[0].y = pt.y;
			pts[0].pencode = penArc;
			pts[1].x = pt.x;
			pts[1].y = pt.y + cy / nLayerNum / 2;
			pts[1].pencode = penArc;
			pts[2].x = pt.x + cy / nLayerNum / 2;
			pts[2].y = pt.y;
			pts[2].pencode = penArc;
			pts[3].x = pt.x;
			pts[3].y = pt.y - cy / nLayerNum / 2;
			pts[3].pencode = penArc;
			pts[4].x = pt.x - cy / nLayerNum / 2;
			pts[4].y = pt.y;
			pts[4].pencode = penArc;
			pGeo->CreateShape(pts, 5);
		}
		else if (bHatch)
		{
			PT_3DEX pts[5];
			pt.x += cx / 2 / nLayerNum;
			pts[0].x = pt.x - cx / (4 * nLayerNum);
			pts[0].y = pt.y + cy / 4;
			pts[0].pencode = penLine;
			pts[1].x = pt.x + cx / (4 * nLayerNum);
			pts[1].y = pt.y + cy / 4;
			pts[1].pencode = penLine;
			pts[2].x = pt.x + cx / (4 * nLayerNum);
			pts[2].y = pt.y - cy / 4;
			pts[2].pencode = penLine;
			pts[3].x = pt.x - cx / (4 * nLayerNum);
			pts[3].y = pt.y - cy / 4;
			pts[3].pencode = penLine;
			pts[4].x = pt.x - cx / (4 * nLayerNum);
			pts[4].y = pt.y + cy / 4;
			pts[4].pencode = penLine;

			pGeo->CreateShape(pts, 5);
		}
		else
		{
			PT_3DEX pts[2];
			pt.x += cx / 2 / nLayerNum;
			{
				pts[0].x = pt.x - cx / nLayerNum / 4;
				pts[0].y = pt.y;
				pts[0].pencode = penLine;
				pts[1].x = pt.x + cx / nLayerNum / 4;
				pts[1].y = pt.y;
				pts[1].pencode = penLine;
			}

			pGeo->CreateShape(pts, 2);
		}

	}
	else if (nGeoClass == CLS_GEODCURVE)
	{
		PT_3DEX pts[4];

		{
			pt.x += cx / nLayerNum / 2;
			pts[0].x = pt.x - cx / nLayerNum / 4;
			pts[0].y = pt.y + cy / 4;
			pts[0].pencode = penLine;
			pts[1].x = pt.x + cx / (nLayerNum) / 4;
			pts[1].y = pt.y + cy / 4;
			pts[1].pencode = penLine;
			pts[2].x = pt.x + cx / (nLayerNum) / 4;
			pts[2].y = pt.y - cy / 4;
			pts[2].pencode = penMove;
			pts[3].x = pt.x - cx / nLayerNum / 4;
			pts[3].y = pt.y - cy / 4;
			pts[3].pencode = penLine;
		}
		pGeo->CreateShape(pts, 4);
	}
	else if (nGeoClass == CLS_GEOSURFACE)
	{
		BOOL bArc = FALSE;
		for (int i = 0; i < nSymbolNum; i++)
		{
			CSymbol *pSym = pLayer->GetSymbol(i);
			int nType = pSym->GetType();
			if (nType == SYMTYPE_SCALEFUNNELTYPE && ((CScaleFunneltype*)pSym)->m_nFunnelType == 1)
			{
				bArc = TRUE;
			}
		}

		if (bArc)
		{
			pt.x += cx / 2;
			PT_3DEX pts[5];
			pts[0].x = pt.x - cy / nLayerNum / 2;
			pts[0].y = pt.y;
			pts[0].pencode = penArc;
			pts[1].x = pt.x;
			pts[1].y = pt.y + cy / nLayerNum / 2;
			pts[1].pencode = penArc;
			pts[2].x = pt.x + cy / nLayerNum / 2;
			pts[2].y = pt.y;
			pts[2].pencode = penArc;
			pts[3].x = pt.x;
			pts[3].y = pt.y - cy / nLayerNum / 2;
			pts[3].pencode = penArc;
			pts[4].x = pt.x - cy / nLayerNum / 2;
			pts[4].y = pt.y;
			pts[4].pencode = penArc;
			pGeo->CreateShape(pts, 5);
		}
		else
		{
			PT_3DEX pts[5];
			pt.x += cx / 2 / nLayerNum;
			{
				pts[0].x = pt.x - cx / (4 * nLayerNum);
				pts[0].y = pt.y + cy / 4;
				pts[0].pencode = penLine;
				pts[1].x = pt.x + cx / (4 * nLayerNum);
				pts[1].y = pt.y + cy / 4;
				pts[1].pencode = penLine;
				pts[2].x = pt.x + cx / (4 * nLayerNum);
				pts[2].y = pt.y - cy / 4;
				pts[2].pencode = penLine;
				pts[3].x = pt.x - cx / (4 * nLayerNum);
				pts[3].y = pt.y - cy / 4;
				pts[3].pencode = penLine;
				pts[4].x = pt.x - cx / (4 * nLayerNum);
				pts[4].y = pt.y + cy / 4;
				pts[4].pencode = penLine;

			}

			pGeo->CreateShape(pts, 5);
		}

	}
	else if (nGeoClass == CLS_GEOTEXT)
	{
		TEXT_SETTINGS0 settings;
		settings.fHeight = m_SymbolTableDlg.m_nRowWidth / 2 / PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()*m_nScale;
		pGeo->CreateShape(&PT_3DEX(pt.x + cx / 2, pt.y, 0, penMove), 1);

		{
			settings.nAlignment = TAH_MID | TAV_MID;
		}
		((CGeoText*)pGeo)->SetSettings(&settings);
		((CGeoText*)pGeo)->SetText("abc");
	}

	return TRUE;

}

extern CDocument *GetCurDocument();
void CSymbolLayoutCommand::DrawTable(PT_3D &pt, int nTableLine, int nTableColumn, int TableNumber, double nTableWidthSpace, CArray<double, double> &nTableColumnSpace)
{
	PT_3DEX expt(pt, penLine);
	PT_3DEX	pt_t(expt);
	if (nTableColumn < 2)
	{
		AfxMessageBox(StrFromResID(IDS_NOTE_DRAWTABLE));
		return;
	}
	CDlgDoc *pDoc = (CDlgDoc*)GetCurDocument();
	CDlgDataSource *pDS1 = pDoc->GetDlgDataSource();
	CFtrLayer *pLayer = pDS1->GetFtrLayer(StrFromResID(IDS_TABLE_LINES));
	if (!pLayer)
	{
		pLayer = pDS1->CreateFtrLayer(StrFromResID(IDS_TABLE_LINES));
		if (!pLayer)
		{
			return;
		}
		pDS1->AddFtrLayer(pLayer);
	}
	CFeature *pFtr = NULL;

	CUndoFtrs undo(m_pEditor, Name());

	TEXT_SETTINGS0 sTxtSettings;
	CArray<CSchemeLayerDefine*, CSchemeLayerDefine*> arrPLayDefines;
	sTxtSettings.fHeight = /*m_SymbolTableDlg.m_nRowWidth/1.5;*/m_SymbolTableDlg.m_nRowWidth / pDoc->GetDlgDataSource()->GetSymbolDrawScale()*m_nScale;//(m_SymbolTableDlg.m_nSpaceTop+m_SymbolTableDlg.m_nSpaceBottom)/2.8;
	sTxtSettings.nPlaceType = singPt;
	sTxtSettings.nAlignment = TAH_MID | TAV_MID;


	/***************获取所有定义层列表***************/
	if (!m_pEditor)
	{
		return;
	}
	CDlgDataSource *pDS = GETDS(m_pEditor);

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	arrPLayDefines.RemoveAll();
	int nSize = pScheme->GetLayerDefineCount();
	for (int i = 0; i < nSize; i++)
	{
		CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(i);
		if (pSchemeLayer)
		{
			arrPLayDefines.Add(pSchemeLayer);
		}

	}
	if (!SortSchemeLayerDefine(arrPLayDefines))//给层定义排序
	{
		return;
	}
	int nSizePlayDefin = arrPLayDefines.GetSize();
	__int64 nCodeBoundLow = m_SymbolTableDlg.m_nCodeBoundLow;
	__int64 nCodeBoundHeigh = m_SymbolTableDlg.m_nCodeBoundHigh;
	int nStart = 0;
	int nIndex = -1;
	int nTotalSize = 0;
	nStart = FindIndexInSchemeLayerDefine(arrPLayDefines, nCodeBoundLow, nCodeBoundHeigh, nTotalSize);//nStart 为编码范围中第一个在数组中出现的下标 nTotalSize 为符合编码范围的总“定义层”个数
	nIndex = nStart;
	/*****************************************/
	if (nTotalSize > nSizePlayDefin)
	{
		nTotalSize = nSizePlayDefin;
	}

	double nClolumn = ceil(((float)nTotalSize / ((float)TableNumber*(float)(nTableLine - 1))));//向上取整 总列数

	GProgressStart(nClolumn*TableNumber*nTableLine*nTableColumn);
	for (int m = 0; m < nClolumn; m++)//总共多少行表（表为个体）
	{
		if (m != 0)
		{
			expt.x = pt.x;
			expt.y = expt.y - (m_SymbolTableDlg.m_nHeight*m_nScale + m_SymbolTableDlg.m_nRowWidth*m_nScale*2.5);//表格高度+上下间隔*2.5
		}


		for (int k = 0; k < TableNumber; k++)//每行多少个表（表为个体）
		{
			double TableColumn = 0;

			GetPartColumSpace(nTableColumnSpace, 5, TableColumn);
			if (k != 0)
			{
				expt.x = expt.x + TableColumn + m_SymbolTableDlg.m_nSpace*m_nScale;
			}

			PT_3D TitlePt;

			TitlePt.x = expt.x + TableColumn / 2;
			TitlePt.y = expt.y + m_SymbolTableDlg.m_nRowWidth*m_nScale;// expt.y+(m_SymbolTableDlg.m_nSpaceTop+m_SymbolTableDlg.m_nSpaceBottom)/2; 
			TitlePt.z = 0;

			DrawSymbolText(TitlePt, StrFromResID(IDS_CLASSCIFICODETABLE), sTxtSettings, undo);//写上分类编码表 抬头
			for (int i = 0; i < nTableLine; i++)//每个表多少行
			{

				for (int j = 0; j < nTableColumn; j++)//每个表多少列
				{
					GProgressStep();
					m_arrPts.RemoveAll();
					double TableColumnSpace = 0, TableColumnSpace1 = 0;
					if (!GetPartColumSpace(nTableColumnSpace, j, TableColumnSpace)) return;
					if (!GetPartColumSpace(nTableColumnSpace, j + 1, TableColumnSpace1)) return;
					pt_t.x = expt.x + TableColumnSpace;
					pt_t.y = expt.y - i*nTableWidthSpace;
					pt_t.z = expt.z;
					m_arrPts.Add(pt_t);

					pt_t.x = expt.x + TableColumnSpace1;
					pt_t.y = expt.y - i*nTableWidthSpace;
					pt_t.z = expt.z;
					m_arrPts.Add(pt_t);

					pt_t.x = expt.x + TableColumnSpace1;
					pt_t.y = expt.y - (i + 1)*nTableWidthSpace;
					pt_t.z = expt.z;
					m_arrPts.Add(pt_t);

					pt_t.x = expt.x + TableColumnSpace;
					pt_t.y = expt.y - (i + 1)*nTableWidthSpace;
					pt_t.z = expt.z;
					m_arrPts.Add(pt_t);

					pt_t.x = expt.x + TableColumnSpace;
					pt_t.y = expt.y - i*nTableWidthSpace;
					pt_t.z = expt.z;
					m_arrPts.Add(pt_t);

					pFtr = pLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOCURVE);

					if (!pFtr->GetGeometry()->CreateShape(m_arrPts.GetData(), m_arrPts.GetSize()))
					{
						delete pFtr;
						pFtr = NULL;
						continue;
					}
					pFtr->GetGeometry()->SetColor(RGB(255, 0, 0));

					if (!m_pEditor->AddObject(pFtr, pLayer->GetID()))
					{
						delete pFtr;
						pFtr = NULL;
						Abort();
						continue;
					}

					undo.arrNewHandles.Add(FtrToHandle(pFtr));

					pFtr = NULL;

					PT_3D pt_center, pt_left;
					pt_center.x = m_arrPts.GetAt(0).x + (m_arrPts.GetAt(1).x - m_arrPts.GetAt(0).x) / 2;
					pt_center.y = m_arrPts.GetAt(0).y - fabs((m_arrPts.GetAt(0).y - m_arrPts.GetAt(3).y) / 2);
					pt_center.z = m_arrPts.GetAt(0).z;

					pt_left.x = m_arrPts.GetAt(0).x;
					pt_left.y = m_arrPts.GetAt(0).y - fabs((m_arrPts.GetAt(0).y - m_arrPts.GetAt(3).y) / 2);
					pt_left.z = m_arrPts.GetAt(0).z;
					double cx = fabs(m_arrPts.GetAt(1).x - m_arrPts.GetAt(0).x);//表格长
					double cy = fabs(m_arrPts.GetAt(0).y - m_arrPts.GetAt(3).y);//表格宽
					TEXT_SETTINGS0 TxtSettings;
					TxtSettings.fHeight = m_SymbolTableDlg.m_nRowWidth / 2 / pDoc->GetDlgDataSource()->GetSymbolDrawScale()*m_nScale;
					TxtSettings.nPlaceType = singPt;
					TxtSettings.nAlignment = TAH_MID | TAV_MID;

					if (0 == i)//写入列表抬头
					{

						switch (j)
						{
						case 0:
							DrawSymbolText(pt_center, StrFromResID(IDS_FIELDNAME_LAYERID), TxtSettings, undo);
							break;
						case 1:
							DrawSymbolText(pt_center, StrFromResID(IDS_FIELDNAME_LAYERNAME), TxtSettings, undo);
							break;
						case 2:
							DrawSymbolText(pt_center, StrFromResID(IDS_SYMBOLMODE), TxtSettings, undo);
							break;
						case 3:
							DrawSymbolText(pt_center, StrFromResID(IDS_LAYERNAME_SYM), TxtSettings, undo);
							break;
						case 4:
							DrawSymbolText(pt_center, StrFromResID(IDS_SYMBOLDISCRIBTION), TxtSettings, undo);
							break;
						}

					}
					else//开始录入数据
					{

						if (nIndex < nSizePlayDefin&&nIndex < nCodeBoundHeigh&&nIndex < nTotalSize + nStart)
						{

							CString str;
							switch (j)
							{
							case 0:
								str.Format("%I64d", arrPLayDefines.GetAt(nIndex)->GetLayerCode());
								DrawSymbolText(pt_center, str, TxtSettings, undo);
								break;
							case 1:
								str = arrPLayDefines.GetAt(nIndex)->GetLayerName();
								DrawSymbolText(pt_center, str, TxtSettings, undo);
								break;
							case 2:
								str.Format("%d", arrPLayDefines.GetAt(nIndex)->GetGeoClass());
								DrawSymbolText(pt_center, str, TxtSettings, undo);
								break;
							case 3:
								CString strSymbolLayerName = arrPLayDefines.GetAt(nIndex)->GetLayerName();
								if (strSymbolLayerName.IsEmpty())
								{
									continue;
								}
								CFtrLayer*	pLayersymbol = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(strSymbolLayerName);
								if (!pLayersymbol)
								{
									pLayersymbol = pDS->CreateFtrLayer(strSymbolLayerName);
									if (!pLayersymbol)  continue;
									GETDS(m_pEditor)->AddFtrLayer(pLayersymbol);
								}
								int nGeocls = arrPLayDefines.GetAt(nIndex)->GetGeoClass();
								if (nGeocls == -1)
								{
									continue;
								}
								m_pFtrSymbol = pLayersymbol->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(), nGeocls);
								if (CreateSymbolItem(arrPLayDefines.GetAt(nIndex), pt_left, cx, cy, 1))
								{
									if (!m_pEditor->AddObject(m_pFtrSymbol, pLayersymbol->GetID()))
									{
										Abort();
										delete m_pFtrSymbol;
										m_pFtrSymbol = NULL;
										continue;
									}
									undo.arrNewHandles.Add(FtrToHandle(m_pFtrSymbol));

									m_pFtrSymbol = NULL;
								}

								break;
							}

						}

					}

				}
				if (i != 0)
				{
					nIndex++;
				}
			}

		}
	}

	if (undo.arrNewHandles.GetSize() > 0)
	{
		undo.Commit();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
		Finish();
	}


	GProgressEnd();

}

void CSymbolLayoutCommand::Finish()
{


	CDrawCommand::Finish();
}




//////////////////////////////////////////////////////////////////////
// CMovetoLocalCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CFCodeChgAllCommand, CEditCommand)

CFCodeChgAllCommand::CFCodeChgAllCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath, "\\FCodeChgAll");
}

CFCodeChgAllCommand::~CFCodeChgAllCommand()
{
}

CString CFCodeChgAllCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CHGFCODEALL);
}

void CFCodeChgAllCommand::Start()
{
	CEditCommand::Start();

	CDlgChangeFCode dlg;
	if (!m_pEditor || dlg.DoModal() != IDOK)
	{
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	ChangeFCode(dlg.m_strSrcFCode, dlg.m_strTarFCode);

	m_nExitCode = CMPEC_STARTOLD;
}

void CFCodeChgAllCommand::ChangeFCode(const char *code1, const char *code2)
{
	CFeature *pFtr = NULL, *pTemp = NULL;
	CDlgDataSource *pDS = GETDS(m_pEditor);
	if (!pDS) return;

	CFtrLayer *pLayer1 = pDS->GetFtrLayer(code1);
	CFtrLayer *pLayer2 = pDS->GetFtrLayer(code2);
	if (!pLayer2 || !pLayer1 || pLayer1->GetObjectCount() <= 0)
	{
		CString strMsg;
		if (!pLayer1)
		{
			strMsg.Format(IDS_ERR_INVALIDLAYER, (LPCTSTR)code1);
		}
		else if (!pLayer2)
		{
			strMsg.Format(IDS_ERR_INVALIDLAYER, (LPCTSTR)code2);
		}
		PromptString(strMsg.GetBuffer(strMsg.GetLength()));
		AfxMessageBox(strMsg);//cjc 2012年11月8日 提示不存在输入层
		Abort();
		return;
	}

	if (AfxMessageBox(StrFromResID(IDS_CMDTIP_CHANGELAYERNAME), MB_OKCANCEL) == IDCANCEL)
	{
		Abort();
		return;
	}

	//获取实体对象总数
	long lSum = pLayer1->GetObjectCount();

	pTemp = pLayer2->CreateDefaultFeature(pDS->GetScale());

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());

	if (pTemp)
	{
		GProgressStart(lSum);

		CUndoFtrs undo(m_pEditor, Name());

		for (int i = 0; i < lSum; i++)
		{
			GProgressStep();

			pFtr = pLayer1->GetObject(i);
			if (pFtr)
			{
				m_pEditor->DeleteObject(FtrToHandle(pFtr));
				undo.AddOldFeature(FtrToHandle(pFtr));

				CValueTable table;
				table.BeginAddValueItem();
				pFtr->WriteTo(table);
				table.EndAddValueItem();

				CFeature *pNewFtr = pTemp->Clone();
				pNewFtr->ReadFrom(table);
				pNewFtr->SetID(OUID());

				CGeometry *pGeo = pNewFtr->GetGeometry();

				float wid = 0;
				if (pScheme && pLayer2)
				{
					wid = pScheme->GetLayerDefineLineWidth(pLayer2->GetName());
				}

				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					((CGeoCurveBase*)pNewFtr->GetGeometry())->m_fLineWidth = wid;
				}

				m_pEditor->AddObject(pNewFtr, pLayer2->GetID());
				undo.AddNewFeature(FtrToHandle(pNewFtr));
			}
		}

		if (pTemp)delete pTemp;

		GProgressEnd();

		if (undo.arrNewHandles.GetSize() > 0)
		{
			undo.Commit();
			m_pEditor->OnSelectChanged();
			m_pEditor->RefreshView();
			Finish();
		}
	}


}


//////////////////////////////////////////////////////////////////////
// CFCodeChgSelectionCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CFCodeChgSelectionCommand, CEditCommand)

CFCodeChgSelectionCommand::CFCodeChgSelectionCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath, "\\FCodeChgSelection");
	m_nExitCode = CMPEC_STARTOLD;
}

CFCodeChgSelectionCommand::~CFCodeChgSelectionCommand()
{
}

CString CFCodeChgSelectionCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CONVERLAYER_SELETION);
}


void CFCodeChgSelectionCommand::Abort()
{
	CCommand::Abort();
	m_nExitCode = CMPEC_STARTOLD;
}


void CFCodeChgSelectionCommand::Start()
{
	if (!m_pEditor)return;
	int numsel;
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);
	if (numsel > 0)
	{
		m_nStep = 0;

		CCommand::Start();

		PT_3D pt;
		PtClick(pt, 0);
		PtClick(pt, 0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	CEditCommand::Start();
}



void CFCodeChgSelectionCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;
		EditStepOne();
	}

	if (m_nStep == 1)
	{
		CSelection* pSel = m_pEditor->GetSelection();
		int num = 0;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		if (num <= 0)
			return;

		CDlgInputCode dlg;
		if (dlg.DoModal() != IDOK)
		{
			m_nStep = 0;
			pSel->DeselectAll();
			m_pEditor->OnSelectChanged();
			m_pEditor->OpenSelector();
			return;
		}

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		CFtrLayer *pFtrLayer = pDS->GetFtrLayer(dlg.m_strCode);
		if (!pFtrLayer)
		{
			CString strMsg;
			strMsg.Format(IDS_ERR_INVALIDLAYER, (LPCTSTR)dlg.m_strCode);
			AfxMessageBox(strMsg);
			return;
		}

		CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());

		CUndoFtrs undo(m_pEditor, Name());
		for (int i = num - 1; i >= 0; i--)
		{
			undo.arrOldHandles.Add(handles[i]);

			CFeature *pFtr0 = HandleToFtr(handles[i]);

			CFeature *pFtr1 = pFtrLayer->CreateDefaultFeature(pDS->GetScale(), pFtr0->GetGeometry()->GetClassType());
			pFtr1->CopyFrom(pFtr0);
			pFtr1->SetID(OUID());

			CGeometry *pGeo = pFtr1->GetGeometry();

			float wid = 0;
			if (pScheme)
			{
				wid = pScheme->GetLayerDefineLineWidth(pFtrLayer->GetName());
			}

			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			{
				((CGeoCurveBase*)pGeo)->m_fLineWidth = wid;
			}

			m_pEditor->DeleteObject(handles[i]);

			m_pEditor->AddObject(pFtr1, pFtrLayer->GetID());
			undo.AddNewFeature(FtrToHandle(pFtr1));
		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		Finish();
		m_nStep = 2;
		return;
	}

	CCommand::PtClick(pt, flag);
}
//////////////////////////////////////////////////////////////////////
// CDrawDirPointCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDrawDirPointCommand, CDrawCommand)

CDrawDirPointCommand::CDrawDirPointCommand()
{
	m_pDrawProc = NULL;
	m_pFtr = NULL;
	m_pSel = NULL;

	m_fAngleOff = 0;
}

CDrawDirPointCommand::~CDrawDirPointCommand()
{
	if (m_pDrawProc)delete m_pDrawProc;
	if (m_pFtr) delete m_pFtr;
	if (m_pSel)delete m_pSel;
}


CString CDrawDirPointCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_DRAWDIRPOINT);
}

CProcedure *CDrawDirPointCommand::GetActiveSonProc(int nMsgType)
{
	if (nMsgType == msgPtClick || nMsgType == msgPtMove || nMsgType == msgPtReset)
		return NULL;

	return m_pDrawProc;
}

void CDrawDirPointCommand::PtMove(PT_3D &pt)
{
	if (!m_pFtr || !m_pDrawProc || GetState() == PROCSTATE_FINISHED)return;

	GrBuffer buf;
	if (m_pDrawProc->m_bDrawDir)
	{
		m_pDrawProc->m_pGeoPoint->SetCtrlPoint(0, pt);
		m_pDrawProc->m_pGeoPoint->m_lfAngle += m_fAngleOff;

		GrBuffer buf0;
		m_pDrawProc->m_pGeoPoint->Draw(&buf0, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf0, uv_StereoView);
	}
	else
	{
		PT_3DEX expt(pt, penLine);
		m_pDrawProc->m_pGeoPoint->CreateShape(&expt, 1);
		m_pDrawProc->m_pGeoPoint->m_lfAngle = m_fAngleOff;
	}

	CString strLayerName;
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (pLayer)
	{
		strLayerName = pLayer->GetName();
	}

	GETDS(m_pEditor)->DrawFeature(m_pFtr, &buf, TRUE, 0, strLayerName);

	m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf, uv_VectorView);

	CDrawCommand::PtMove(pt);
}

void CDrawDirPointCommand::PtClick(PT_3D &pt, int flag)
{
	ASSERT(m_pDrawProc == son);

	if (m_pDrawProc)
	{
		m_pDrawProc->PtClick(pt, flag);
	}

	if ((m_pDrawProc->m_pGeoPoint != NULL)
		&& (m_pDrawProc->m_pGeoPoint->GetDataPointSum() == 1))
	{
		if (PDOC(m_pEditor)->IsAutoSetAnchor())
		{
			PDOC(m_pEditor)->SetAnchorPoint(m_pDrawProc->m_pGeoPoint->GetDataPoint(0));
		}
	}
}


void CDrawDirPointCommand::PtReset(PT_3D &pt)
{
	if (m_pDrawProc && m_pDrawProc->m_pGeoPoint && m_pFtr)
	{
		m_pDrawProc->m_pGeoPoint->m_lfAngle = 0;
		if (!AddObject(m_pFtr))
		{
			Abort();
			return;
		}

		CUndoFtrs undo(m_pEditor, Name());
		undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
		undo.Commit();

		Finish();
	}
	else
	{
		if (m_pDrawProc)
		{
			m_pDrawProc->PtReset(pt);
		}
	}
}

void CDrawDirPointCommand::OnSonEnd(CProcedure *son)
{
	ASSERT(m_pDrawProc == son);
	if (IsProcAborted(m_pDrawProc) || !m_pDrawProc->m_pGeoPoint)
		Abort();
	else
	{
		if (m_pFtr)
		{
			m_pDrawProc->m_pGeoPoint->m_lfAngle += m_fAngleOff;

			if (!AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/)
			{
				Abort();
				return;
			}

			CUndoFtrs undo(m_pEditor, Name());
			undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
			undo.Commit();

			Finish();
		}
		else
			Abort();
	}

}
void CDrawDirPointCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEODIRPOINT);
	if (!m_pFtr) return;

	m_pDrawProc = new CDrawDirPointProcedure;
	if (!m_pDrawProc)return;

	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_pGeoPoint = (CGeoDirPoint*)m_pFtr->GetGeometry();

	m_pDrawProc->Start();

	CDrawCommand::Start();

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}

void CDrawDirPointCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}
void CDrawDirPointCommand::Finish()
{
	if (m_pDrawProc)
	{
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}

	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	if (m_pEditor)
	{
		if (m_pFtr)
		{
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);
			if (bAnnotToText)
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
				//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if (pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);
				if (pLayer != NULL && pTextLayer != NULL && pTempl != NULL)
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor), m_pFtr, arrAnnots);
					CUndoFtrs undo(m_pEditor, Name());
					for (int i = 0; i < arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType() == SYMTYPE_ANNOTATION)
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr, tab);
							if (pAnnot->ExtractGeoText(m_pFtr, parr, tab, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()))
							{
								for (int j = 0; j < parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);
									PDOC(m_pEditor)->AddObject(pNewFtr, pTextLayer->GetID());
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
								}
							}
						}
					}
					undo.Commit();
				}
				if (pTempl != NULL)delete pTempl;
			}
		}
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CDrawCommand::Finish();
}

void CDrawDirPointCommand::Abort()
{
	if (m_pDrawProc)
	{
		if (!IsProcOver(m_pDrawProc))
			m_pDrawProc->Abort();

		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (!m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CDrawCommand::Abort();
}

DrawingInfo CDrawDirPointCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	CArray<PT_3DEX, PT_3DEX> arr;
	m_pFtr->GetGeometry()->GetShape(arr);
	return DrawingInfo(m_pFtr, arr);
}

int  CDrawDirPointCommand::GetCurPenCode()
{
	return CDrawCommand::GetCurPenCode();
}

int CDrawDirPointCommand::GetState()
{
	return GetStateWithSonProcedure(m_pDrawProc);
}


void CDrawDirPointCommand::GetParams(CValueTable &tab)
{
	CDrawCommand::GetParams(tab);

	if (!m_pDrawProc) return;

	_variant_t var;
	var = m_fAngleOff;
	tab.AddValue("AngleOff", &CVariantEx(var));
}

void CDrawDirPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(AccelStr(), Name());
	param->AddParam("AngleOff", m_fAngleOff, StrFromLocalResID(IDS_ANGLEOFF));
}


void CDrawDirPointCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "AngleOff", var))
	{
		m_fAngleOff = (float)(_variant_t)*var;
	}

	SetSettingsModifyFlag();
	CDrawCommand::SetParams(tab, bInit);
}


//////////////////////////////////////////////////////////////////////
// CDrawAutoHeightCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDrawAutoHeightPointCommand, CDrawCommand)

CDrawAutoHeightPointCommand::CDrawAutoHeightPointCommand()
{
	m_nStep = -1;
	m_pFtr = NULL;
	m_pSel = NULL;
	m_pDrawProc = NULL;
	m_pGeoCurve = NULL;
	m_bPtInArea = FALSE;
	m_nBoundType = 0;
	m_bOdd = FALSE;
	m_bDrawBound = FALSE;
	m_lfDX = 10;
	m_lfDY = 10;
	strcat(m_strRegPath, "\\DrawAutoHeightPoint");
}


CDrawAutoHeightPointCommand::~CDrawAutoHeightPointCommand()
{

}

DrawingInfo CDrawAutoHeightPointCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	CArray<PT_3DEX, PT_3DEX> arr;
	m_pFtr->GetGeometry()->GetShape(arr);
	return DrawingInfo(m_pFtr, arr);
}

int  CDrawAutoHeightPointCommand::GetCurPenCode()
{
	return CDrawCommand::GetCurPenCode();
}

CString CDrawAutoHeightPointCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_AUTOHEIGHTPOINT);
}


void CDrawAutoHeightPointCommand::Start()
{
	m_lfDX = 10;
	m_lfDY = 10;

	m_arrFtrs.RemoveAll();

	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOPOINT);
	if (!m_pFtr) return;

	EnableReadCmdParas(TRUE);

	CDrawCommand::Start();

	if (m_nBoundType == 1)
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if (!m_pDrawProc)return;
		m_pDrawProc->Init(m_pEditor);
		m_pDrawProc->Start();
		m_pGeoCurve = new CGeoCurve;
		if (!m_pGeoCurve)
		{
			Abort();
			return;
		}
		m_pGeoCurve->SetColor(RGB(255, 255, 255));
		m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
		m_pDrawProc->m_bClosed = TRUE;
	}

	if (m_nBoundType == 1 && m_arrBound.GetSize() >= 4)
	{
		m_nStep = 2;
	}
	else
	{
		m_nStep = 0;
		m_bDrawBound = TRUE;
	}

	m_bOdd = FALSE;

	m_pEditor->CloseSelector();

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}

void CDrawAutoHeightPointCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}
CProcedure *CDrawAutoHeightPointCommand::GetActiveSonProc(int nMsgType)
{
	if (m_nBoundType == 1 && (nMsgType == msgEnd || nMsgType == msgPtMove/*||nMsgType==msgPtReset*/))
	{
		return m_pDrawProc;
	}
	return NULL;
}

void CDrawAutoHeightPointCommand::PtReset(PT_3D &pt)
{
	if (m_pDrawProc && !IsProcOver(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
	}
	else
		Finish();
}

void CDrawAutoHeightPointCommand::OnSonEnd(CProcedure *son)
{
	if (m_nBoundType == 1 && m_bDrawBound)
	{
		if (m_nStep == 1)
		{
			if (!m_pDrawProc->m_pGeoCurve || IsProcFinished(this))
				return;

			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag, &buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
			m_nStep = 2;

		}
		else
		{
			Abort();
		}

		m_bDrawBound = FALSE;
	}
	if (m_pDrawProc)
	{
		const CShapeLine *pBase = m_pDrawProc->m_pGeoCurve->GetShape();
		CArray<PT_3DEX, PT_3DEX> arr;
		pBase->GetPts(arr);
		m_arrBound.SetSize(arr.GetSize());
		for (int i = 0; i < arr.GetSize(); i++)
		{
			COPY_3DPT(m_arrBound[i], arr[i]);
		}

		SetSettingsModifyFlag();

		m_eBound.CreateFromPts(m_arrBound.GetData(), m_arrBound.GetSize(), sizeof(PT_3D));

		//		m_pGeoCurve = (CGeoCurve *)m_pDrawProc->m_pGeoCurve->Clone();
		if (m_pGeoCurve)
		{
			delete m_pGeoCurve;
			m_pGeoCurve = NULL;
		}

		delete m_pDrawProc;
		m_pDrawProc = NULL;

		CDrawCommand::OnSonEnd(son);
	}
}

void CDrawAutoHeightPointCommand::Finish()
{
	if (m_pDrawProc)
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	if (m_pEditor)
	{
		BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);
		if (bAnnotToText)
		{
			CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
			CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
			CFeature *pTempl = NULL;
			if (pTextLayer)
				pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);

			if (pLayer != NULL && pTextLayer != NULL && pTempl != NULL)
			{
				CConfigLibManager *pCLM = gpCfgLibMan;

				CFeature *pFtr = NULL;
				for (int i = 0; i < m_arrFtrs.GetSize(); i++)
				{
					pFtr = (CFeature*)m_arrFtrs[i];
					if (!pFtr)  continue;

					CPtrArray arrAnnots;
					pCLM->GetSymbol(GETDS(m_pEditor), pFtr, arrAnnots);
					CUndoFtrs undo(m_pEditor, Name());
					for (int i = 0; i < arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType() == SYMTYPE_ANNOTATION)
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(pFtr, tab);
							if (pAnnot->ExtractGeoText(pFtr, parr, tab, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()))
							{
								for (int j = 0; j < parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);
									PDOC(m_pEditor)->AddObject(pNewFtr, pTextLayer->GetID());
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
								}
							}
						}
					}
					undo.Commit();

				}

				if (pTempl != NULL)delete pTempl;
				m_pEditor->RefreshView();

			}
		}

	}
	CDrawCommand::Finish();
}


void CDrawAutoHeightPointCommand::Abort()
{
	UpdateParams(TRUE);
	if (m_pDrawProc)
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
		m_pGeoCurve = NULL;
	}
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}

	m_pEditor->UpdateDrag(ud_SetConstDrag, NULL);
	m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
	m_pEditor->RefreshView();

	if (m_pEditor)
	{
		BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);
		if (bAnnotToText)
		{
			CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
			CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
			CFeature *pTempl = NULL;
			if (pTextLayer)
				pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);

			if (pLayer != NULL && pTextLayer != NULL && pTempl != NULL)
			{
				CConfigLibManager *pCLM = gpCfgLibMan;

				CFeature *pFtr = NULL;
				for (int i = 0; i < m_arrFtrs.GetSize(); i++)
				{
					pFtr = (CFeature*)m_arrFtrs[i];
					if (!pFtr)  continue;

					CPtrArray arrAnnots;
					pCLM->GetSymbol(GETDS(m_pEditor), pFtr, arrAnnots);
					CUndoFtrs undo(m_pEditor, Name());
					for (int i = 0; i < arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType() == SYMTYPE_ANNOTATION)
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(pFtr, tab);
							if (pAnnot->ExtractGeoText(pFtr, parr, tab, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()))
							{
								for (int j = 0; j < parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);
									PDOC(m_pEditor)->AddObject(pNewFtr, pTextLayer->GetID());
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
								}
							}
						}
					}
					undo.Commit();

				}

				if (pTempl != NULL)delete pTempl;
				m_pEditor->RefreshView();

			}
		}

	}

	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	CDrawCommand::Abort();

	//	m_nExitCode = CMPEC_RESTARTWITHPARAMS;
}


int CDrawAutoHeightPointCommand::GetState()
{
	return CDrawCommand::GetState();
}

void CDrawAutoHeightPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(PID_DRAWAUTOHEIGHTPOINT, StrFromResID(IDS_CMDNAME_AUTOHEIGHTPOINT));
	param->BeginOptionParam(PF_TRIMBOUND, StrFromResID(IDS_CMDPLANE_BOUND));
	param->AddOption(StrFromResID(IDS_CMDPLANE_WSBOUND), 0, ' ', m_nBoundType == 0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_DRAW), 1, ' ', m_nBoundType == 1);
	param->EndOptionParam();

	if (m_nBoundType == 1 || bForLoad)
	{
		param->AddButtonParam("Draw", StrFromResID(IDS_CMDPLANE_DRAW), NULL);
	}

	if (m_nBoundType == 1 && m_pDrawProc)
	{
		BOOL bHide = (m_pDrawProc->m_nCurPenCode != penStream);
		if (!bHide || bForLoad)param->AddParam(PF_TRIMTOLER, double(m_pDrawProc->m_compress.GetLimit()), StrFromResID(IDS_CMDPLANE_TOLER));
	}
	param->AddParam(PF_XSTEP, m_lfDX, StrFromResID(IDS_CMDPLANE_XSTEP));
	param->AddParam(PF_YSTEP, m_lfDY, StrFromResID(IDS_CMDPLANE_YSTEP));
	param->AddParam(PF_AUTOHEIGHT, 't', StrFromResID(IDS_CMDNAME_AUTOHEIGHTPOINT));

}

void CDrawAutoHeightPointCommand::SetParams(CValueTable& tab, BOOL bInit)
{

	const CVariantEx *var;
	if (tab.GetValue(0, PF_TRIMBOUND, var))
	{
		m_nBoundType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if (!bInit)
		{
			Abort();
			return;
		}

	}
	if (tab.GetValue(0, PF_XSTEP, var))
	{
		m_lfDX = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_YSTEP, var))
	{
		m_lfDY = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_AUTOHEIGHT, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 't' || ch == 'T')
		{
			if (!bInit)
			{
				PT_3D pt = m_pEditor->GetCoordWnd().m_ptGrd;

				PT_3D pt2 = pt;

				// 光标移到下一个在区域内的点 			
				do
				{
					if (pt2.y > m_eBound.m_yh || pt2.y < m_eBound.m_yl)
					{
						Abort();
						return;
					}
					if (pt2.x + m_lfDX > m_eBound.m_xh && m_lfDX > 0)
					{
						pt2.y += m_lfDY;
						if (m_bOdd)
							pt2.x = m_eBound.m_xl + m_lfDX / 2;
						else
							pt2.x = m_eBound.m_xl;

						m_bOdd = !m_bOdd;
					}
					else if (pt2.x + m_lfDX < m_eBound.m_xl && m_lfDX < 0)
					{
						pt2.y += m_lfDY;
						if (m_bOdd)
							pt2.x = m_eBound.m_xh + m_lfDX / 2;
						else
							pt2.x = m_eBound.m_xh;

						m_bOdd = !m_bOdd;
					}
					else
					{
						pt2.x += m_lfDX;
					}
				} while (GraphAPI::GIsPtInRegion(pt2, m_arrBound.GetData(), m_arrBound.GetSize()) < 0);

				// 				pt.x = m_pEditor->GetCoordWnd().m_ptGrd.x + m_lfDX;
				// 				pt.y = m_pEditor->GetCoordWnd().m_ptGrd.y + m_lfDY;
				// 				pt.z = m_pEditor->GetCoordWnd().m_ptGrd.z;
				((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL, hc_SetCrossPos, (CObject*)&pt2);
			}

		}

	}

	if (tab.GetValue(0, "Draw", var))
	{
		m_bDrawBound = TRUE;
		m_pEditor->UpdateDrag(ud_ClearDrag);
		SetSettingsModifyFlag();
	}

	var = NULL;
	if (tab.GetValue(0, "Bound", var) && m_nBoundType == 1)
	{
		if (var)
		{
			CArray<PT_3DEX, PT_3DEX> arr;
			var->GetShape(arr);

			m_arrBound.RemoveAll();
			GrBuffer buf;
			buf.BeginLineString(0, 0);

			for (int i = 0; i < arr.GetSize(); i++)
			{
				m_arrBound.Add(arr[i]);
				if (i == 0)
				{
					buf.MoveTo(&arr[i]);
				}
				else
				{
					buf.LineTo(&arr[i]);
				}
			}

			buf.End();

			m_eBound.CreateFromPts(m_arrBound.GetData(), m_arrBound.GetSize(), sizeof(PT_3D));

			m_pEditor->UpdateDrag(ud_SetConstDrag, &buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
			m_pEditor->RefreshView();

		}
	}
	CDrawCommand::SetParams(tab, bInit);
}

void CDrawAutoHeightPointCommand::GetParams(CValueTable& tab)
{
	CDrawCommand::GetParams(tab);

	_variant_t var;
	var = (long)m_nBoundType;
	tab.AddValue(PF_TRIMBOUND, &(CVariantEx)(long)(var));
	var = (double)m_lfDX;
	tab.AddValue(PF_XSTEP, &CVariantEx(var));
	var = (double)m_lfDY;
	tab.AddValue(PF_YSTEP, &CVariantEx(var));
	char ch = 't';
	var = (BYTE)(ch);
	tab.AddValue(PF_AUTOHEIGHT, &CVariantEx(var));

	if (m_nBoundType == 1 && m_arrBound.GetSize() >= 4)
	{
		CArray<PT_3DEX, PT_3DEX> arr;
		for (int i = 0; i < m_arrBound.GetSize(); i++)
		{
			arr.Add(PT_3DEX(m_arrBound[i], penLine));
		}
		CVariantEx varex;
		varex.SetAsShape(arr);
		tab.AddValue("Bound", &varex);
	}
}

void CDrawAutoHeightPointCommand::PtDblClick(PT_3D &pt, int flag)
{
	PtClick(pt, flag);
}

void CDrawAutoHeightPointCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nBoundType == 1 && m_bDrawBound)
	{
		m_nStep = 1;

		if (m_pDrawProc)
		{
			GotoState(PROCSTATE_PROCESSING);
			m_pDrawProc->PtClick(pt, flag);
		}

	}
	else if (m_nBoundType == 0)
	{
		if (m_nStep == 0)
		{
			m_arrBound.SetSize(4);
			PDOC(m_pEditor)->GetDlgDataSource()->GetBound(m_arrBound.GetData(), NULL, NULL);
			m_eBound.CreateFromPts(m_arrBound.GetData(), m_arrBound.GetSize(), sizeof(PT_3D));
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 2;
		}
	}

	if (m_nStep == 2)
	{
		if (IsProcFinished(this))return;

		GotoState(PROCSTATE_PROCESSING);

		if (!m_bPtInArea && GraphAPI::GIsPtInRegion(pt, m_arrBound.GetData(), m_arrBound.GetSize()) < 0)
		{
			Abort();
			return;
		}
		else
		{
			if (!m_pFtr) return;
			CFeature *pFtr = m_pFtr->Clone();
			CGeoPoint *pGeo = (CGeoPoint*)pFtr->GetGeometry();
			if (!pGeo) return;
			PT_3DEX t(pt, penLine);
			pGeo->SetDataPoint(0, t);
			pFtr->SetID(OUID());
			if (!AddObject(pFtr))
			{
				delete pFtr;
				pFtr = NULL;
				Abort();
				return;
			}

			m_arrFtrs.Add(FtrToHandle(pFtr));

			m_pEditor->RefreshView();
			CUndoFtrs undo(m_pEditor, Name());
			undo.arrNewHandles.Add(FtrToHandle(pFtr));
			undo.Commit();

			m_bPtInArea = TRUE;

			PT_3D pt2 = pt;

			// 光标移到下一个在区域内的点 			
			do
			{
				if (pt2.y > m_eBound.m_yh || pt2.y < m_eBound.m_yl)
				{
					Abort();
					return;
				}
				if (pt2.x + m_lfDX > m_eBound.m_xh && m_lfDX > 0)
				{
					pt2.y += m_lfDY;
					if (m_bOdd)
						pt2.x = m_eBound.m_xl + m_lfDX / 2;
					else
						pt2.x = m_eBound.m_xl;

					m_bOdd = !m_bOdd;
				}
				else if (pt2.x + m_lfDX < m_eBound.m_xl && m_lfDX < 0)
				{
					pt2.y += m_lfDY;
					if (m_bOdd)
						pt2.x = m_eBound.m_xh + m_lfDX / 2;
					else
						pt2.x = m_eBound.m_xh;

					m_bOdd = !m_bOdd;
				}
				else
				{
					pt2.x += m_lfDX;
				}
			} while (GraphAPI::GIsPtInRegion(pt2, m_arrBound.GetData(), m_arrBound.GetSize()) < 0);

			((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL, hc_SetCrossPos, (CObject*)&pt2);

		}

	}


}

/////////////////////////////////////////////////////////////////////
// CSeparateParallelCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSeparateParallelCommand, CEditCommand)

CSeparateParallelCommand::CSeparateParallelCommand()
{
	m_nStep = -1;
}

CSeparateParallelCommand::~CSeparateParallelCommand()
{
}

void CSeparateParallelCommand::Abort()
{

}

CString CSeparateParallelCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_SEPPARALLEL);
}

void CSeparateParallelCommand::Start()
{
	if (!m_pEditor)return;
	int num;
	CDlgDoc *pDoc = (CDlgDoc*)m_pEditor;
	(CDlgDoc*)m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num > 0)
	{
		m_nStep = 0;
		CCommand::Start();

		PT_3D pt;
		PtClick(pt, SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	CEditCommand::Start();
}


void CSeparateParallelCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;
		EditStepOne();
	}

	if (m_nStep == 1)
	{
		CGeometry *pObj;
		CGeometry *pObj1, *pObj2;
		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);

		CUndoFtrs undo(m_pEditor, Name());

		for (int i = num - 1; i >= 0; i--)
		{
			pObj = HandleToFtr(handles[i])->GetGeometry();

			//生成新的对象，这里需要指定新对象层码
			// 			if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
			// 			{
			// 				continue;
			// 			}

			if ((pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) && ((CGeoParallel*)pObj)->Separate(pObj1, pObj2)) || (pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) && ((CGeoDCurve*)pObj)->Separate(pObj1, pObj2)))
			{
				CFeature * pFtr1 = HandleToFtr(handles[i])->Clone();
				if (!pFtr1) return;
				pFtr1->SetID(OUID());
				if (pObj1)
				{
					pFtr1->SetGeometry(pObj1);

					if (!m_pEditor->AddObject(pFtr1, m_pEditor->GetFtrLayerIDOfFtr(handles[i])))
					{
						delete pFtr1;
						Abort();
						return;
					}
					GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]), pFtr1);
					undo.arrNewHandles.Add(FtrToHandle(pFtr1));
				}
				else
				{
					delete pFtr1;
				}

				CFeature * pFtr2 = HandleToFtr(handles[i])->Clone();
				if (!pFtr2)return;
				pFtr2->SetID(OUID());
				if (pObj2)
				{
					pFtr2->SetGeometry(pObj2);
					if (!m_pEditor->AddObject(pFtr2, m_pEditor->GetFtrLayerIDOfFtr(handles[i])))
					{
						delete pFtr2;
						Abort();
						return;
					}
					GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]), pFtr2);
					undo.arrNewHandles.Add(FtrToHandle(pFtr2));
				}
				else
				{
					delete pFtr2;
				}

				undo.arrOldHandles.Add(handles[i]);
				//删除原来的对象
				m_pEditor->DeleteObject(handles[i]);
			}

		}
		undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL, hc_Refresh, 0);
		Finish();
		m_nStep = 2;
	}

	CEditCommand::PtClick(pt, flag);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDrawDCurveCommand, CDrawCommand)

CDrawDCurveCommand::CDrawDCurveCommand()
{
	m_bToSurface = FALSE;
	m_pSel = NULL;
	m_pDrawProc = NULL;
	m_pFtr = NULL;
	strcat(m_strRegPath, "\\DrawDCurve");
}


CDrawDCurveCommand::~CDrawDCurveCommand()
{
	if (m_pSel)delete m_pSel;
	if (m_pDrawProc)delete m_pDrawProc;
	if (m_pFtr) delete m_pFtr;
}

CGeometry *CDrawDCurveCommand::GetCurDrawingObj()
{
	if (!m_pDrawProc->m_pGeoCurve) return NULL;
	return  m_pDrawProc->m_pGeoCurve;
}

CProcedure *CDrawDCurveCommand::GetActiveSonProc(int nMsgType)
{
	if (nMsgType == msgPtClick || nMsgType == msgPtReset)
		return NULL;

	return m_pDrawProc;
}

void CDrawDCurveCommand::PtReset(PT_3D &pt)
{
	if (!m_pDrawProc || !m_pDrawProc->m_pGeoCurve)  return;

	if (m_pDrawProc->m_nCurPenCode == penStream)
	{
		if (IsEndSnapped())
			m_pDrawProc->m_nEndSnapped |= 2;
		else
			m_pDrawProc->m_nEndSnapped &= (~2);
	}

	m_pDrawProc->PtReset(pt);
}

DrawingInfo CDrawDCurveCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	CArray<PT_3DEX, PT_3DEX> pts;
	pts.Append(m_pDrawProc->m_arrBasePts);
	pts.Append(m_pDrawProc->m_arrPts);
	// 解决采集基线后捕捉不到尾点的问题
	if (m_pDrawProc->m_arrPts.GetSize() == 0 && m_pDrawProc->m_arrBasePts.GetSize() > 0)
	{
		int size = m_pDrawProc->m_arrBasePts.GetSize();
		pts.Add(m_pDrawProc->m_arrBasePts[size - 1]);
	}
	CFeature tmpFtr;
	tmpFtr.CreateGeometry(CLS_GEOCURVE);

	//by mzy，解决双线绘制时，颜色和线宽无法保存为缺省的问题
	CValueTable tab;
	tab.BeginAddValueItem();
	m_pFtr->WriteTo(tab);
	tab.EndAddValueItem();

	tab.DelField(FIELDNAME_SHAPE);
	tab.DelField(FIELDNAME_FTRID);
	tab.DelField(FIELDNAME_GEOCLASS);

	tmpFtr.ReadFrom(tab);
	//
	return	DrawingInfo(&tmpFtr, pts);
}

CString CDrawDCurveCommand::Name()
{
	return StrFromLocalResID(IDS_CMDNAME_PLACEDCURVE);
}


void CDrawDCurveCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEODCURVE);
	if (!m_pFtr) return;

	CDlgDataSource *pDS = GETDS(m_pEditor);

	m_pDrawProc = new CDrawDCurveProcedure;
	if (!m_pDrawProc)return;
	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_pGeoCurve = (CGeoDCurve*)m_pFtr->GetGeometry();
	m_pDrawProc->m_layCol = pLayer->GetColor();
	m_pDrawProc->m_bSnap2D = AfxGetApp()->GetProfileInt(REGPATH_SNAP, REGITEM_IS2D, TRUE);

	m_pDrawProc->Start();

	CDrawCommand::Start();
	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}

void CDrawDCurveCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}

void CDrawDCurveCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pDrawProc)  return;

	ASSERT(m_pDrawProc == son);

	if (!m_pDrawProc->m_pGeoCurve)return;

	m_pDrawProc->PtClick(pt, flag);

	if (m_pDrawProc->m_pGeoCurve->GetDataPointSum() == 0)
	{
		m_pDrawProc->m_nEndSnapped = IsEndSnapped() ? 1 : 0;
	}
	else
	{
		if (IsEndSnapped())
			m_pDrawProc->m_nEndSnapped |= 2;
		else
			m_pDrawProc->m_nEndSnapped &= (~2);
	}

	if (m_pDrawProc->m_pGeoCurve->GetDataPointSum() == 1)
	{
		if (PDOC(m_pEditor)->IsAutoSetAnchor())
		{
			PDOC(m_pEditor)->SetAnchorPoint(m_pDrawProc->m_pGeoCurve->GetDataPoint(0));
		}
	}
}

void CDrawDCurveCommand::Finish()
{
	if (m_bSettingModified)
		UpdateParams(TRUE);
	if (m_pDrawProc)
	{
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}

	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	if (m_pEditor)
	{
		if (m_pFtr)
		{
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);
			if (bAnnotToText)
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
				//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if (pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);
				if (pLayer != NULL && pTextLayer != NULL && pTempl != NULL)
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor), m_pFtr, arrAnnots);
					CUndoFtrs undo(m_pEditor, Name());
					for (int i = 0; i < arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType() == SYMTYPE_ANNOTATION)
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr, tab);
							if (pAnnot->ExtractGeoText(m_pFtr, parr, tab, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()))
							{
								for (int j = 0; j < parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);
									PDOC(m_pEditor)->AddObject(pNewFtr, pTextLayer->GetID());
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
								}
							}
						}
					}
					undo.Commit();
				}
				if (pTempl != NULL)delete pTempl;
			}
		}
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}

	CDrawCommand::Finish();
}


int  CDrawDCurveCommand::GetCurPenCode()
{
	if (m_pDrawProc)
	{
		return m_pDrawProc->m_nCurPenCode;
	}
	return CDrawCommand::GetCurPenCode();
}

void CDrawDCurveCommand::UpdateParams(BOOL bSave)
{
	CDrawCommand::UpdateParams(bSave);
}


void CDrawDCurveCommand::Abort()
{
	if (m_pDrawProc)
	{
		if (!IsProcOver(m_pDrawProc))
			m_pDrawProc->Abort();

		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}

	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	CDrawCommand::Abort();
}


int CDrawDCurveCommand::GetState()
{
	return GetStateWithSonProcedure(m_pDrawProc);
}


void CDrawDCurveCommand::Back() //没有必要存在
{
	if (m_pDrawProc&&m_pDrawProc->m_pGeoCurve)
	{
		if (m_pDrawProc->m_pGeoCurve->GetDataPointSum() < 2)
		{
			Abort();
			return;
		}
		CArray<PT_3DEX, PT_3DEX> pts;
		m_pDrawProc->m_pGeoCurve->GetShape(pts);
		pts.RemoveAt(pts.GetSize() - 1);
		m_pDrawProc->m_pGeoCurve->CreateShape(pts.GetData(), pts.GetSize());
		GrBuffer cbuf, vbuf;
		m_pDrawProc->m_pGeoCurve->GetShape()->GetConstGrBuffer(&cbuf);
		m_pDrawProc->m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf, m_pDrawProc->m_pGeoCurve->IsClosed());
		m_pEditor->UpdateDrag(ud_SetConstDrag, &cbuf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
	}
}

BOOL CDrawDCurveCommand::IsEndSnapped()
{
	if (!m_pEditor)
		return FALSE;

	if (!PDOC(m_pEditor)->m_snap.bOpen())
		return FALSE;

	SNAPITEM item = PDOC(m_pEditor)->m_snap.GetFirstSnapResult();
	if (!item.IsValid())
		return FALSE;

	if (item.nSnapMode == CSnap::modeNearPoint ||
		item.nSnapMode == CSnap::modeKeyPoint ||
		item.nSnapMode == CSnap::modeMidPoint ||
		item.nSnapMode == CSnap::modeIntersect ||
		item.nSnapMode == CSnap::modePerpPoint ||
		item.nSnapMode == CSnap::modeTangPoint ||
		item.nSnapMode == CSnap::modeEndPoint)
		return TRUE;

	return FALSE;
}

void CDrawDCurveCommand::OnSonEnd(CProcedure *son)
{
	ASSERT(m_pDrawProc == son);

	if (IsProcAborted(m_pDrawProc) || !m_pDrawProc->m_pGeoCurve)
		Abort();
	else
	{
		if (m_pFtr)
		{
			CUndoFtrs undo(m_pEditor, Name());
			if (m_bToSurface)
			{
				CGeoDCurve *pDCurve = (CGeoDCurve*)m_pFtr->GetGeometry();
				if (!pDCurve)
				{
					Abort();
					return;
				}

				CGeometry *pObj1, *pObj2;
				if (pDCurve->Separate(pObj1, pObj2))
				{
					CArray<PT_3DEX, PT_3DEX> pts1, pts2;
					pObj1->GetShape(pts1);
					pObj2->GetShape(pts2);
					PT_3DEX p1 = pts1[0];
					PT_3DEX p2 = pts2[0];
					PT_3DEX p3 = pts1[pts1.GetSize() - 1];
					PT_3DEX p4 = pts2[pts2.GetSize() - 1];
					double x, y, t0, t1;
					bool bIntersect = GraphAPI::GGetLineIntersectLineSeg(p1.x, p1.y,
						p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, &x, &y, &t0, &t1);
					for (int j = 0; j < pts2.GetSize(); j++)
					{
						if (bIntersect)
							pts1.Add(pts2[j]);
						else
							pts1.Add(pts2[pts2.GetSize() - 1 - j]);
					}
					pts1.Add(pts1[0]);

					CFeature *pNew = m_pFtr->Clone();
					if (!pNew || !pNew->CreateGeometry(CLS_GEOSURFACE))
					{
						Abort();
						return;
					}
					pNew->GetGeometry()->CreateShape(pts1.GetData(), pts1.GetSize());
					if (!AddObject(pNew))
					{
						delete pNew;
						Abort();
						return;
					}

					delete m_pFtr;

					undo.arrNewHandles.Add(FtrToHandle(pNew));
				}
			}
			else
			{
				if (!AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/)
				{
					Abort();
					return;
				}

				undo.AddNewFeature(FtrToHandle(m_pFtr));
			}
			undo.Commit();
			Finish();
		}
		else
			Abort();
	}
}

void CDrawDCurveCommand::GetParams(CValueTable &tab)
{
	CDrawCommand::GetParams(tab);

	if (!m_pDrawProc) return;

	_variant_t var;
	var = (long)(m_pDrawProc->m_nCurPenCode);
	tab.AddValue(PF_PENCODE, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bClosed);
	tab.AddValue(PF_CLOSED, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bOpenIfSnapped);
	tab.AddValue(PF_OPENIFSNAP, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bRectify);
	tab.AddValue(PF_RECTIFY, &CVariantEx(var));

	var = (double)(m_pDrawProc->m_compress.GetLimit());
	tab.AddValue(PF_TOLER, &CVariantEx(var));

	var = (double)(m_pDrawProc->m_fAutoCloseToler);
	tab.AddValue(PF_AUTOCLOSETOLER, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bDoSmooth);
	tab.AddValue(PF_POSTSMOOTH, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bFastDisplayCurve);
	tab.AddValue(PF_FASTCURVE, &CVariantEx(var));

	var = (bool)(m_bToSurface);
	tab.AddValue(PF_TOSURFACE, &CVariantEx(var));
}

void CDrawDCurveCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DrawDCurve", Name());
	if (m_pDrawProc)
	{
		PT_3D pt;
		param->AddParam(PF_POINT, pt, StrFromLocalResID(IDS_CMDPLANE_POINT));
		param->AddParam(PF_CLOSED, (bool)(m_pDrawProc->m_bClosed), StrFromLocalResID(IDS_CMDPLANE_CLOSE));
		param->AddParam(PF_CLOSEDYACCKEY, 'c', StrFromResID(IDS_CMDPLANE_CLOSE));
		if (m_pDrawProc->m_bClosed || bForLoad)
		{
			param->BeginOptionParam(PF_OPENIFSNAP, StrFromResID(IDS_CMDPLANE_OPENIFSNAP));
			param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_pDrawProc->m_bOpenIfSnapped);
			param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_pDrawProc->m_bOpenIfSnapped);
			param->EndOptionParam();
		}

		if (m_pDrawProc->m_nCurPenCode == penLine || bForLoad)
		{
			param->BeginOptionParam(PF_RECTIFY, StrFromResID(IDS_CMDPLANE_RECTIFY));
			param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_pDrawProc->m_bRectify);
			param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_pDrawProc->m_bRectify);
			param->EndOptionParam();
		}

		param->AddParam(PF_RECTIFYACCKEY, 'r', StrFromResID(IDS_CMDPLANE_RECTIFY));
		param->AddParam(PF_REVERSEACCKEY, 't', StrFromResID(IDS_CMDPLANE_REVERSE));
		param->AddParam(PF_TRACKLINEACCKEY, 'w', StrFromResID(IDS_CMDPLANE_TRACKLINE));

		if (m_pDrawProc->m_nCurPenCode == penStream || bForLoad)
		{
			// 容差，自动闭合容差，采集后光滑
			param->AddParam(PF_TOLER, m_pDrawProc->m_compress.GetLimit(), StrFromResID(IDS_CMDPLANE_TOLERANCE));
			param->AddParam(PF_AUTOCLOSETOLER, m_pDrawProc->m_fAutoCloseToler, StrFromResID(IDS_CMDPLANE_AUTOCLOSETOLER));

			param->BeginOptionParam(PF_POSTSMOOTH, StrFromResID(IDS_CMDPLANE_POSTSMOOTH));
			param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_pDrawProc->m_bDoSmooth);
			param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_pDrawProc->m_bDoSmooth);
			param->EndOptionParam();
		}

		if (m_pDrawProc->m_nCurPenCode == penSpline || bForLoad)
		{
			param->BeginOptionParam(PF_FASTCURVE, StrFromResID(IDS_CMDPLANE_FASTCURVE));
			param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_pDrawProc->m_bFastDisplayCurve);
			param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_pDrawProc->m_bFastDisplayCurve);
			param->EndOptionParam();
			/*
						// 转换时constLine的生成
						if (!bForLoad)
						{
						CArray<PT_3DEX,PT_3DEX> arrtmp;
						arrtmp.Append(m_pDrawProc->m_arrPts);
						// 转到快速显示时
						if (m_pDrawProc->m_bFastDisplayCurve)
						{
						for (int i=0; i<arrtmp.GetSize(); i++)
						{
						PT_3DEX &t = arrtmp.ElementAt(i);
						t.pencode = penLine;
						}
						}

						CGeoCurve curve;
						curve.CreateShape(arrtmp.GetData(),arrtmp.GetSize());

						m_pEditor->UpdateDrag(ud_ClearDrag);

						GrBuffer cbuf,vbuf;
						const CShapeLine *pSL = curve.GetShape();

						if (m_pDrawProc->m_bClosed && arrtmp.GetSize()>2)
						{
						arrtmp.Add(arrtmp.GetAt(0));
						curve.CreateShape(arrtmp.GetData(),arrtmp.GetSize());
						pSL->GetConstGrBuffer(&cbuf,true);
						arrtmp.RemoveAt(arrtmp.GetSize()-1);
						curve.CreateShape(arrtmp.GetData(),arrtmp.GetSize());

						}
						else
						{
						pSL->GetConstGrBuffer(&cbuf);
						}

						long color = m_pDrawProc->m_pGeoCurve->GetColor();
						if (color == -1)
						{
						color = m_pDrawProc->m_layCol;
						}
						cbuf.SetAllColor(color);
						m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);


						}*/
		}

		param->AddLineTypeParam(PF_PENCODE, m_pDrawProc->m_nCurPenCode, StrFromLocalResID(IDS_CMDPLANE_LINETYPE));
		param->AddParam(PF_TOSURFACE, (bool)(m_bToSurface), StrFromResID(IDS_CMDPLANE_TOSURFACE));
	}
}


void CDrawDCurveCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_PENCODE, var))
	{
		m_pDrawProc->ChangePencod((long)(_variant_t)*var);
		//		m_pDrawProc->m_nCurPenCode = (long)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_NODEWID, var))
	{
		m_pDrawProc->m_fCurNodeWid = (float)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_NODETYPE, var))
	{
		m_pDrawProc->m_nCurNodeType = (short)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_CLOSED, var))
	{
		m_pDrawProc->m_bClosed = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_CLOSEDYACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 'c' || ch == 'C')
		{
			m_pDrawProc->m_bClosed = !m_pDrawProc->m_bClosed;
		}

	}
	if (tab.GetValue(0, PF_TRACKLINEACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 'w' || ch == 'W')
		{
			m_pDrawProc->TrackLine();
		}
	}
	if (tab.GetValue(0, PF_OPENIFSNAP, var))
	{
		m_pDrawProc->m_bOpenIfSnapped = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_RECTIFY, var))
	{
		m_pDrawProc->m_bRectify = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_RECTIFYACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 'r' || ch == 'R')
		{
			m_pDrawProc->m_bRectify = !m_pDrawProc->m_bRectify;
		}

	}
	if (tab.GetValue(0, PF_REVERSEACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 't' || ch == 'T')
		{
			m_pDrawProc->ReverseLine();
		}

	}
	if (tab.GetValue(0, PF_TOLER, var))
	{
		m_pDrawProc->m_compress.SetLimit((double)(_variant_t)*var);
	}
	if (tab.GetValue(0, PF_AUTOCLOSETOLER, var))
	{
		m_pDrawProc->m_fAutoCloseToler = (double)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_POSTSMOOTH, var))
	{
		m_pDrawProc->m_bDoSmooth = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_FASTCURVE, var))
	{
		m_pDrawProc->m_bFastDisplayCurve = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_POINT, var))
	{
		CArray<PT_3DEX, PT_3DEX> arrPts;
		var->GetShape(arrPts);
		PT_3D point = arrPts.GetAt(0);
		PtClick(point, 0);
		PtMove(point);
		m_pEditor->RefreshView();
	}
	if (tab.GetValue(0, PF_TOSURFACE, var))
	{
		m_bToSurface = (bool)(_variant_t)*var;
	}

	// 	if( m_pDrawProc->m_pGeoCurve )
	// 		m_pDrawProc->m_pGeoCurve->EnableClose(m_pDrawProc->m_bClosed);	
	SetSettingsModifyFlag();
	CDrawCommand::SetParams(tab, bInit);
}



BOOL CDrawDCurveCommand::Keyin(LPCTSTR text)
{
	if (!m_pDrawProc)
		return FALSE;

	// 通过长度和角度指定点
	char buf[100] = { 0 };

	strncpy(buf, text, 100 - 1);
	char *stop = NULL;
	double len = strtod(buf, &stop);
	if (stop != NULL && strlen(stop) == 0)
	{
		int num = m_pDrawProc->m_arrPts.GetSize();
		if (num > 0)
		{
			PT_3D curPt = m_pEditor->GetCoordWnd().m_ptGrd;
			PT_3DEX pt0 = m_pDrawProc->m_arrPts[num - 1], pt1;
			double dx = curPt.x - pt0.x, dy = curPt.y - pt0.y;
			double dis = sqrt(dx*dx + dy*dy);
			if (dis < GraphAPI::GetDisTolerance())
			{
				pt1.x = pt0.x + len;
				pt1.y = pt0.y;
				pt1.z = curPt.z;
			}
			else
			{
				pt1.x = pt0.x + dx / dis * len;
				pt1.y = pt0.y + dy / dis * len;
				pt1.z = curPt.z;
			}

			PtClick(pt1, 0);
			PtMove(curPt);
			m_pEditor->RefreshView();

			return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDrawParallelogramCommand, CDrawDCurveCommand)
CDrawParallelogramCommand::CDrawParallelogramCommand()
{
	strcat(m_strRegPath, "\\DrawParallelogram");
}

CDrawParallelogramCommand::~CDrawParallelogramCommand()
{

}

CString CDrawParallelogramCommand::Name()
{
	return StrFromResID(IDS_CMDTIP_PARALLELOGRAM);
}

int  CDrawParallelogramCommand::GetCurPenCode()
{
	return penLine;
}

void CDrawParallelogramCommand::Start()
{
	CDrawDCurveCommand::Start();

	if (m_pDrawProc)
	{
		m_pDrawProc->m_nMode = 1;
	}
}

void CDrawParallelogramCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DrawParallelogram", Name());
	if (m_pDrawProc)
	{
		param->AddParam(PF_TRACKLINEACCKEY, 'w', StrFromResID(IDS_CMDPLANE_TRACKLINE));
	}
}

void CDrawParallelogramCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_TRACKLINEACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 'w' || ch == 'W')
		{
			m_pDrawProc->TrackLine();
		}
	}
	SetSettingsModifyFlag();
	CDrawCommand::SetParams(tab, bInit);
}

void CDrawParallelogramCommand::GetParams(CValueTable& tab)
{

}



IMPLEMENT_DYNCREATE(CDrawParalRoadCommand, CDrawCommand)

CDrawParalRoadCommand::CDrawParalRoadCommand()
{
	m_bMark = false;
	m_bBreakup = false;
	m_bMark2 = false;
	m_nStep = -1;
	m_lfcurWidth = 0;
	m_lflastWidth = 0;
	m_pFtr = NULL;
	m_LastParalPoint.x = 0;
	m_LastParalPoint.y = 0;
	m_LastParalPoint.z = 0;
	m_DistancePoint.x = 0;
	m_DistancePoint.y = 0;
	m_DistancePoint.z = 0;
	strcat(m_strRegPath, "\\DrawParallelRoad");
}


CDrawParalRoadCommand::~CDrawParalRoadCommand()
{

	if (m_pFtr) delete m_pFtr;
}


CString CDrawParalRoadCommand::Name()
{
	return StrFromLocalResID(IDS_PARALLEL_ROAD_CHANG_WIDTH);
}


void CDrawParalRoadCommand::Start()
{
	m_bMark = false;
	m_bBreakup = false;
	m_bMark2 = false;
	m_nStep = 0;
	m_lfcurWidth = 0;
	m_lflastWidth = 0;
	m_pFtr = NULL;
	m_LastParalPoint.x = 0;
	m_LastParalPoint.y = 0;
	m_LastParalPoint.z = 0;
	m_DistancePoint.x = 0;
	m_DistancePoint.y = 0;
	m_DistancePoint.z = 0;

	m_pFtr = GETCURFTRLAYER(m_pEditor)->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOCURVE);
	if (!m_pFtr) return;

	CDlgDataSource *pDS = GETDS(m_pEditor);

	CDrawCommand::Start();
}

void CDrawParalRoadCommand::PtReset(PT_3D &pt)
{
	if ((m_ptsFtr1.GetSize() < 2) || (m_ptsFtr2.GetSize() < 2))
	{
		Abort();
		return;
	}

	PT_3DEX pts[2], ret[2];
	pts[0] = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 2);
	pts[1] = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 1);
	if (m_LastParalPoint.x == 0 && m_LastParalPoint.y == 0)
	{

		ret[1].pencode = penLine;
		GraphAPI::GGetParallelLine(pts, 2, m_lfcurWidth, ret);
		m_ptsFtr2.Add(ret[1]);
	}
	else
	{
		PT_3DEX  pt1, pt2;
		double x1, y1;
		GraphAPI::GGetRightAnglePoint(pts[0].x, pts[0].y, pts[1].x, pts[1].y, m_LastParalPoint.x, m_LastParalPoint.y, &x1, &y1);
		pt1.x = m_LastParalPoint.x;
		pt1.y = m_LastParalPoint.y;
		pt1.z = m_LastParalPoint.z;
		pt1.pencode = penLine;
		pt2.x = x1;
		pt2.y = y1;
		pt2.z = 0;
		pt2.pencode = penLine;
		//m_ptsFtr2.Add(pt1);
		m_ptsFtr2.Add(pt2);
	}
	Finish();
}

void CDrawParalRoadCommand::PtClick(PT_3D &pt, int flag)
{

	if (IsProcFinished(this))return;

	GotoState(PROCSTATE_PROCESSING);

	if (m_nStep == 0)
	{
		m_ptsFtr1.Add(PT_3DEX(pt, penLine));
		m_nStep = 1;
	}
	else if (m_nStep == 1)
	{
		m_DistancePoint = pt;
		m_nStep = 2;
	}
	else if (m_nStep == 2)
	{
		PT_3D  PT0, PT1;
		PT0.x = m_ptsFtr1[0].x;
		PT0.y = m_ptsFtr1[0].y;
		PT0.z = 0;
		//差积防止三点成一条直线
		if (GraphAPI::GGetMultiply(PT0, pt, m_DistancePoint) > 0)
		{
			m_lfcurWidth = GraphAPI::GGetDisofPtToLine(PT0.x, PT0.y, pt.x, pt.y, m_DistancePoint.x, m_DistancePoint.y);
			m_lflastWidth = m_lfcurWidth;
		}
		else
		{
			m_lfcurWidth = -GraphAPI::GGetDisofPtToLine(PT0.x, PT0.y, pt.x, pt.y, m_DistancePoint.x, m_DistancePoint.y);
			m_lflastWidth = m_lfcurWidth;
		}
		CValueTable tab;
		tab.BeginAddValueItem();
		_variant_t var;
		var = m_lfcurWidth;
		tab.AddValue(PF_WIDTH, &CVariantEx(var));
		tab.EndAddValueItem();
		SetParams(tab);
		PT_3DEX Pt1, Pt2;
		m_ptsFtr1.Add(PT_3DEX(pt, penLine));
		PT_3D ret1[2];
		PT_3D Ptt[2];
		COPY_3DPT(Ptt[0], m_ptsFtr1[0]);
		COPY_3DPT(Ptt[1], m_ptsFtr1[1]);
		GraphAPI::GGetParallelLine(Ptt, 2, m_lfcurWidth, ret1);
		COPY_3DPT(Pt1, ret1[0]); Pt1.pencode = penLine;
		COPY_3DPT(Pt2, ret1[1]); Pt2.pencode = penLine;
		m_ptsFtr2.Add(Pt1);
		GrBuffer cbuf;
		cbuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(), 0);
		cbuf.MoveTo(&m_ptsFtr1[0]);
		cbuf.LineTo(&m_ptsFtr1[1]);
		cbuf.End();
		m_pEditor->UpdateDrag(ud_AddConstDrag, &cbuf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
		m_nStep = 3;
	}

	else if (m_nStep > 2)
	{
		if ((!m_bMark) || m_bMark2)
		{
			m_ptsFtr1.Add(PT_3DEX(pt, penLine));
			m_bMark2 = false;
		}
		else
		{
			m_DistancePoint = pt;
			m_bMark2 = true;
			return;
		}

		PT_3DEX Pt1, Pt2, Pt3, Pt4;
		PT_3D ret[3];
		int npt;
		npt = m_ptsFtr1.GetSize();
		if (npt > 2)
		{
			//当宽度改变时，求平行线时，宽度用的是两个值。
			if (m_bMark)
			{
				Pt1 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 1);
				Pt2 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 2);
				Pt4 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 3);
				Pt3 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize() - 1);
				double tem = GraphAPI::GGetDisofPtToLine(Pt2.x, Pt2.y, Pt1.x, Pt1.y, m_DistancePoint.x, m_DistancePoint.y);
				m_lfcurWidth = m_lfcurWidth > 0 ? tem : -tem;

				CValueTable tab;
				tab.BeginAddValueItem();
				_variant_t var;
				var = m_lfcurWidth;
				tab.AddValue(PF_WIDTH, &CVariantEx(var));
				tab.EndAddValueItem();
				SetParams(tab);

				m_bMark = false;

				PT_3D Pts[2], Ret[2], Pts2[2], Ret2[2];
				COPY_3DPT(Pts[0], Pt2);
				COPY_3DPT(Pts[1], Pt1);
				GraphAPI::GGetParallelLine(Pts, 2, m_lfcurWidth, Ret);
				COPY_3DPT(Pts2[0], Pt4);
				COPY_3DPT(Pts2[1], Pt2);
				GraphAPI::GGetParallelLine(Pts2, 2, m_lflastWidth, Ret2);
				double vx0 = Ret[1].x - Ret[0].x;
				double vy0 = Ret[1].y - Ret[0].y;
				double vx1 = Ret2[1].x - Ret2[0].x;
				double vy1 = Ret2[1].y - Ret2[0].y;
				double x = 0, y = 0;
				if (!GraphAPI::GGetLineIntersectLine(Ret[0].x, Ret[0].y, vx0, vy0, Ret2[0].x, Ret2[0].y, vx1, vy1, &x, &y, NULL))
				{
					x = Ret[0].x; y = Ret[0].y;
				}
				PT_3DEX  IntersectPoint;
				IntersectPoint.pencode = penLine;
				IntersectPoint.x = x;
				IntersectPoint.y = y;
				m_ptsFtr2.Add(IntersectPoint);

				m_LastParalPoint.x = x;
				m_LastParalPoint.y = y;
				m_LastParalPoint.z = Pt3.z;
				m_lflastWidth = m_lfcurWidth;

				GrBuffer cbuf;
				PT_3D pt3d;
				cbuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(), 1);
				IntersectPoint = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 2);
				cbuf.MoveTo(&IntersectPoint);
				IntersectPoint = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 1);
				cbuf.LineTo(&IntersectPoint);
				//画了第一段后变长度
				//if (m_ptsFtr2.GetSize() >= 2)
				{
					IntersectPoint = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize() - 2);
					cbuf.MoveTo(&IntersectPoint);
					IntersectPoint = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize() - 1);
					cbuf.LineTo(&IntersectPoint);
				}
				cbuf.End();
				m_pEditor->UpdateDrag(ud_AddConstDrag, &cbuf);
				m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
			}
			else
			{
				PT_3DEX  PT4;
				Pt1 = m_ptsFtr1.GetAt(npt - 3);
				Pt2 = m_ptsFtr1.GetAt(npt - 2);
				Pt3 = m_ptsFtr1.GetAt(npt - 1);
				PT4 = m_ptsFtr2[m_ptsFtr2.GetSize() - 1];
				PT_3D Pt[3];
				COPY_3DPT(Pt[0], Pt1);
				COPY_3DPT(Pt[1], Pt2);
				COPY_3DPT(Pt[2], Pt3);
				//置0说明没有点击右键
				m_LastParalPoint.x = 0;
				m_LastParalPoint.y = 0;
				m_LastParalPoint.z = 0;
				if (m_lflastWidth == m_lfcurWidth)
				{
					GraphAPI::GGetParallelLine(Pt, 3, m_lfcurWidth, ret);
					COPY_3DPT(Pt2, ret[1]); Pt2.pencode = penLine;
					m_ptsFtr2.Add(Pt2);
				}
				else
				{
					//改变了宽度就存进文件
					CValueTable tab;
					tab.BeginAddValueItem();
					_variant_t var;
					var = m_lfcurWidth;
					tab.AddValue(PF_WIDTH, &CVariantEx(var));
					tab.EndAddValueItem();
					SetParams(tab);
					PT_3D Pts[2], Ret[2], Pts2[2], Ret2[2];
					COPY_3DPT(Pts[0], Pt1);
					COPY_3DPT(Pts[1], Pt2);
					GraphAPI::GGetParallelLine(Pts, 2, m_lflastWidth, Ret);
					COPY_3DPT(Pts2[0], Pt2);
					COPY_3DPT(Pts2[1], Pt3);
					GraphAPI::GGetParallelLine(Pts2, 2, m_lfcurWidth, Ret2);
					double vx0 = Ret[1].x - Ret[0].x;
					double vy0 = Ret[1].y - Ret[0].y;
					double vx1 = Ret2[1].x - Ret2[0].x;
					double vy1 = Ret2[1].y - Ret2[0].y;
					double x = 0, y = 0;
					if (!GraphAPI::GGetLineIntersectLine(Ret[0].x, Ret[0].y, vx0, vy0, Ret2[0].x, Ret2[0].y, vx1, vy1, &x, &y, NULL))
					{
						x = Ret[0].x; y = Ret[0].y;
					}
					PT_3DEX  IntersectPoint;
					IntersectPoint.pencode = penLine;
					IntersectPoint.x = x;
					IntersectPoint.y = y;
					m_ptsFtr2.Add(IntersectPoint);
					m_lflastWidth = m_lfcurWidth;
				}
				GrBuffer cbuf;
				PT_3DEX ptex;
				cbuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(), 0);
				ptex = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 2);
				cbuf.MoveTo(&ptex);
				ptex = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 1);
				cbuf.LineTo(&ptex);
				ptex = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize() - 2);
				cbuf.MoveTo(&ptex);
				ptex = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize() - 1);
				cbuf.LineTo(&ptex);
				cbuf.End();
				m_pEditor->UpdateDrag(ud_AddConstDrag, &cbuf);
				m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);

			}
		}

	}

	if (m_nStep >= 2 && m_pFtr->GetGeometry()->GetDataPointSum() == 1 && PDOC(m_pEditor)->IsAutoSetAnchor())
	{
		PDOC(m_pEditor)->SetAnchorPoint(pt);
	}

	CCommand::PtClick(pt, flag);
}


void CDrawParalRoadCommand::PtMove(PT_3D &pt)
{
	if (IsProcFinished(this))return;
	if (m_nStep == 1)
	{
		PT_3D  pt0;
		COPY_3DPT(pt0, m_ptsFtr1[0]);
		GrBuffer vbuf1;
		vbuf1.BeginLineString(0, 0);
		double len = GraphAPI::GGet2DDisOf2P(pt0, pt);
		vbuf1.Dash(pt0, pt, len / 8, len / 8);
		vbuf1.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf1);
	}
	if (m_nStep == 2)
	{
		double width;
		PT_3D  PT0;
		PT0.x = m_ptsFtr1[0].x;
		PT0.y = m_ptsFtr1[0].y;
		PT0.z = 0;
		if (GraphAPI::GGetMultiply(pt, PT0, m_DistancePoint) > 0)
			width = -GraphAPI::GGetDisofPtToLine(m_ptsFtr1[0].x, m_ptsFtr1[0].y, pt.x, pt.y, m_DistancePoint.x, m_DistancePoint.y);
		else
			width = GraphAPI::GGetDisofPtToLine(m_ptsFtr1[0].x, m_ptsFtr1[0].y, pt.x, pt.y, m_DistancePoint.x, m_DistancePoint.y);
		PT_3DEX Pt1;
		Pt1 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 1);
		PT_3D Pt[2];
		COPY_3DPT(Pt[0], Pt1);
		COPY_3DPT(Pt[1], pt);
		PT_3D ret[2];
		GraphAPI::GGetParallelLine(Pt, 2, width, ret);
		GrBuffer vbuf;
		vbuf.BeginLineString(0, 0);
		vbuf.MoveTo(&Pt[0]);
		vbuf.LineTo(&Pt[1]);
		vbuf.MoveTo(&ret[0]);
		vbuf.LineTo(&ret[1]);
		vbuf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
	}
	if (m_nStep > 2)
	{
		PT_3DEX Pt1, Pt2, Pt3;
		if (m_bMark)
		{
			if (!m_bMark2)
			{
				PT_3D  pt0;
				COPY_3DPT(pt0, m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 1));
				GrBuffer vbuf;
				vbuf.BeginLineString(0, 0);
				double len = GraphAPI::GGet2DDisOf2P(pt0, pt);
				vbuf.Dash(pt0, pt, len / 8, len / 8);
				vbuf.End();
				m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
			}
			else
			{

				Pt1 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 1);
				Pt2 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 2);
				Pt3 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize() - 1);
				double x, y, x1, y1;

				GraphAPI::GGetRightAnglePoint(Pt2.x, Pt2.y, Pt1.x, Pt1.y, Pt3.x, Pt3.y, &x1, &y1);
				double width;
				PT_3D  Pt1_1;
				Pt1_1.x = Pt1.x;
				Pt1_1.y = Pt1.y;
				Pt1_1.z = Pt1.z;
				width = GraphAPI::GGetDisofPtToLine(Pt1.x, Pt1.y, pt.x, pt.y, m_DistancePoint.x, m_DistancePoint.y);
				width = m_lfcurWidth > 0 ? width : -width;
				PT_3D Pt[2];
				COPY_3DPT(Pt[0], Pt1);
				COPY_3DPT(Pt[1], pt);
				PT_3D ret[2];
				GraphAPI::GGetParallelLine(Pt, 2, width, ret);

				double vx0 = ret[1].x - ret[0].x;
				double vy0 = ret[1].y - ret[0].y;
				double vx1 = x1 - Pt3.x;
				double vy1 = y1 - Pt3.y;
				if (!GraphAPI::GGetLineIntersectLine(ret[0].x, ret[0].y, vx0, vy0, Pt3.x, Pt3.y, vx1, vy1, &x, &y, NULL))
				{
					x = ret[0].x; y = ret[0].y;
				}
				PT_3DEX  IntersectPoint;
				IntersectPoint.pencode = penLine;
				IntersectPoint.x = x;
				IntersectPoint.y = y;

				PT_3D  endPoint;
				endPoint.x = ret[1].x;
				endPoint.y = ret[1].y;
				endPoint.z = ret[1].z;
				GrBuffer vbuf;
				vbuf.BeginLineString(0, 0);
				vbuf.MoveTo(&Pt1);
				vbuf.LineTo(&pt);
				vbuf.MoveTo(&Pt3);
				vbuf.LineTo(&IntersectPoint);
				vbuf.LineTo(&endPoint);
				vbuf.End();
				m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);

			}
		}
		else
		{
			PT_3D ret[3];
			Pt1 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 2);
			Pt2 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize() - 1);
			Pt3 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize() - 1);

			GrBuffer vbuf;
			vbuf.BeginLineString(0, 0);
			vbuf.MoveTo(&Pt2);
			vbuf.LineTo(&pt);

			if (m_lfcurWidth == m_lflastWidth)
			{
				PT_3D Pt[3];
				COPY_3DPT(Pt[0], Pt1);
				COPY_3DPT(Pt[1], Pt2);
				COPY_3DPT(Pt[2], pt);
				GraphAPI::GGetParallelLine(Pt, 3, m_lfcurWidth, ret);
				vbuf.MoveTo(&Pt3);
				vbuf.LineTo(&ret[1]);
				vbuf.LineTo(&ret[2]);
				vbuf.End();
			}
			else
			{
				PT_3D Pts1[2], Pts2[2], Ret1[2], Ret2[2];
				COPY_3DPT(Pts1[0], Pt1);
				COPY_3DPT(Pts1[1], Pt2);
				COPY_3DPT(Pts2[0], Pt2);
				COPY_3DPT(Pts2[1], pt);
				GraphAPI::GGetParallelLine(Pts1, 2, m_lflastWidth, Ret1);
				GraphAPI::GGetParallelLine(Pts2, 2, m_lfcurWidth, Ret2);
				double  vx0 = Ret1[1].x - Ret1[0].x;
				double  vy0 = Ret1[1].y - Ret1[0].y;
				double  vx1 = Ret2[1].x - Ret2[0].x;
				double  vy1 = Ret2[1].y - Ret2[0].y;
				double x = 0, y = 0;
				if (!GraphAPI::GGetLineIntersectLine(Ret1[0].x, Ret1[0].y, vx0, vy0, Ret2[0].x, Ret2[0].y, vx1, vy1, &x, &y, NULL))
				{
					x = ret[0].x; y = ret[0].y;
				}
				PT_3DEX  IntersectPoint;
				IntersectPoint.pencode = penLine;
				IntersectPoint.x = x;
				IntersectPoint.y = y;
				vbuf.MoveTo(&Pt3);
				vbuf.LineTo(&IntersectPoint);
				vbuf.LineTo(&Ret2[1]);
				vbuf.End();
			}
			m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
		}
	}
	CCommand::PtMove(pt);
}


void CDrawParalRoadCommand::Finish()
{
	UpdateParams(TRUE);

	if (m_bBreakup)
	{
		CFeature* pFtr1 = GETCURFTRLAYER(m_pEditor)->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOCURVE);
		if (!pFtr1) return;
		CFeature* pFtr2 = pFtr1->Clone();
		if (!pFtr2)
		{
			delete pFtr1;
			return;
		}

		pFtr1->SetID(OUID());
		pFtr2->SetID(OUID());

		CUndoFtrs undo(m_pEditor, Name());
		if (pFtr1->GetGeometry()->CreateShape(m_ptsFtr1.GetData(), m_ptsFtr1.GetSize()))
		{
			if (m_pEditor->AddObject(pFtr1))
			{
				undo.arrNewHandles.Add(FtrToHandle(pFtr1));
			}
			else
			{
				delete pFtr1;
			}
		}

		CArray<PT_3DEX, PT_3DEX> ptsFtr;
		ptsFtr.SetSize(m_ptsFtr2.GetSize());
		for (int i = 0; i < m_ptsFtr2.GetSize(); i++)
		{
			ptsFtr[i] = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize() - 1 - i);
		}
		if (pFtr2->GetGeometry()->CreateShape(ptsFtr.GetData(), ptsFtr.GetSize()))
		{
			if (m_pEditor->AddObject(pFtr2))
			{
				undo.arrNewHandles.Add(FtrToHandle(pFtr2));
			}
			else
			{
				delete pFtr2;
			}
		}
		undo.Commit();
	}
	else
	{
		CFeature *pFtr1 = GETCURFTRLAYER(m_pEditor)->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEODCURVE);
		m_ptsFtr2.GetAt(0).pencode = penMove;
		m_ptsFtr1.Append(m_ptsFtr2);
		if (pFtr1->GetGeometry()->CreateShape(m_ptsFtr1.GetData(), m_ptsFtr1.GetSize()))
		{
			if (!m_pEditor->AddObject(pFtr1))
			{
				delete pFtr1;
				return;
			}
			CUndoFtrs undo(m_pEditor, Name());
			undo.arrNewHandles.Add(FtrToHandle(pFtr1));
			undo.Commit();
		}
	}


	CDrawCommand::Finish();
}


int  CDrawParalRoadCommand::GetCurPenCode()
{
	return  CDrawCommand::GetCurPenCode();
}

DrawingInfo CDrawParalRoadCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	CArray<PT_3DEX, PT_3DEX> arr;
	m_pFtr->GetGeometry()->GetShape(arr);
	return DrawingInfo(m_pFtr, arr);
}


void CDrawParalRoadCommand::Abort()
{
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	CDrawCommand::Abort();
}


int CDrawParalRoadCommand::GetState()
{
	return CDrawCommand::GetState();
}


void CDrawParalRoadCommand::Back()
{
	if (!m_pFtr) return;

	if (m_ptsFtr1.GetSize() < 1 || m_ptsFtr2.GetSize() < 1)
	{
		Abort();
		return;
	}

	m_ptsFtr1.RemoveAt(m_ptsFtr1.GetSize() - 1);
	m_ptsFtr2.RemoveAt(m_ptsFtr2.GetSize() - 1);

	if ((m_ptsFtr1.GetSize() == 1) ||
		(m_ptsFtr2.GetSize() == 1))
	{
		m_nStep = 2;
	}

	CGeoCurve curve;
	GrBuffer cbuf, cbuf1, cbuf2, vbuf;
	if (curve.CreateShape(m_ptsFtr1.GetData(), m_ptsFtr1.GetSize()))
	{
		curve.Draw(&cbuf1, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
	}

	if (curve.CreateShape(m_ptsFtr2.GetData(), m_ptsFtr2.GetSize()))
	{
		curve.Draw(&cbuf2, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
	}
	cbuf.AddBuffer(&cbuf1);
	cbuf.AddBuffer(&cbuf2);
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_pEditor->UpdateDrag(ud_SetConstDrag, &cbuf);
	m_pEditor->RefreshView();

}


void CDrawParalRoadCommand::GetParams(CValueTable &tab)
{
	CDrawCommand::GetParams(tab);

	_variant_t var;
	var = (double)m_lfcurWidth;
	tab.AddValue(PF_WIDTH, &CVariantEx(var));
	var = (bool)m_bBreakup;
	tab.AddValue(PF_BREAKUP, &CVariantEx(var));
}

void CDrawParalRoadCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DrawParalRoadCommand", Name());
	param->AddParam(PF_WIDTH, m_lfcurWidth, StrFromResID(IDS_CMDPLANE_WIDTH));
	param->AddParam("WidthChange", 'e', StrFromResID(IDS_CMDPLANE_ADJUSTWIDTH));

	param->AddParam(PF_BREAKUP, (bool)m_bBreakup, StrFromResID(IDS_CMDPLANE_BREAKUP));

	param->AddParam(PF_BREAKUPACCKEY, 'b', StrFromResID(IDS_CMDPLANE_BREAKUP));
	CDrawCommand::FillShowParams(param, bForLoad);
}

void CDrawParalRoadCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "WidthChange", var))
	{
		char ch = (char)(BYTE)(_variant_t)*var;
		if (ch == 'e')
		{
			m_bMark = TRUE;
		}
	}
	if (tab.GetValue(0, PF_WIDTH, var))
	{
		m_lfcurWidth = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, PF_BREAKUP, var))
	{
		m_bBreakup = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CDrawCommand::SetParams(tab, bInit);
}



BOOL CDrawParalRoadCommand::Keyin(LPCTSTR text)
{
	// 通过长度和角度指定点
	char buf[100] = { 0 };

	strncpy(buf, text, 100 - 1);
	char *stop = NULL;
	double wid = strtod(buf, &stop);
	if (stop != NULL && strlen(stop) == 0)
	{
		CValueTable tab;
		tab.BeginAddValueItem();
		_variant_t var;
		var = wid;
		tab.AddValue(PF_WIDTH, &CVariantEx(var));
		tab.EndAddValueItem();
		SetParams(tab);
		return TRUE;
	}

	return FALSE;
}







//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDrawParalCommand, CDrawCommand)

CDrawParalCommand::CDrawParalCommand()
{
	m_bToSurface = FALSE;
	m_pDrawProc = NULL;
	m_pFtr = NULL;
	m_pSel = NULL;
	strcat(m_strRegPath, "\\Parallel");
}


CDrawParalCommand::~CDrawParalCommand()
{
	if (m_pDrawProc)delete m_pDrawProc;
	if (m_pFtr) delete m_pFtr;
	if (m_pSel) delete m_pSel;
}

CString CDrawParalCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_DRAWPARALLEL);
}


void CDrawParalCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOPARALLEL);
	if (!m_pFtr) return;

	CDlgDataSource *pDS = GETDS(m_pEditor);

	m_pDrawProc = new CDrawParallelProcedure;
	if (!m_pDrawProc)return;

	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_pGeoPara = (CGeoParallel*)m_pFtr->GetGeometry();
	m_pDrawProc->m_layCol = pLayer->GetColor();
	m_pDrawProc->m_bSnap2D = AfxGetApp()->GetProfileInt(REGPATH_SNAP, REGITEM_IS2D, TRUE);

	m_pDrawProc->Start();
	m_pDrawProc->m_compress.SetLimit(0.1);
	m_pDrawProc->m_compress.m_lfScale = GETCURSCALE(m_pEditor)*1e-3;

	CDrawCommand::Start();

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}

DrawingInfo CDrawParalCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	return DrawingInfo(m_pFtr, m_pDrawProc->m_arrPts);
}


int  CDrawParalCommand::GetCurPenCode()
{
	if (m_pDrawProc)
	{
		return m_pDrawProc->m_nCurPenCode;
	}
	return CDrawCommand::GetCurPenCode();
}

BOOL CDrawParalCommand::IsEndSnapped()
{
	if (!m_pEditor)
		return FALSE;

	if (!PDOC(m_pEditor)->m_snap.bOpen())
		return FALSE;

	SNAPITEM item = PDOC(m_pEditor)->m_snap.GetFirstSnapResult();
	if (!item.IsValid())
		return FALSE;

	if (item.nSnapMode == CSnap::modeNearPoint ||
		item.nSnapMode == CSnap::modeKeyPoint ||
		item.nSnapMode == CSnap::modeMidPoint ||
		item.nSnapMode == CSnap::modeIntersect ||
		item.nSnapMode == CSnap::modePerpPoint ||
		item.nSnapMode == CSnap::modeTangPoint ||
		item.nSnapMode == CSnap::modeEndPoint)
		return TRUE;

	return FALSE;
}

void CDrawParalCommand::Finish()
{
	if (m_bSettingModified)
		UpdateParams(TRUE);
	if (m_pDrawProc)
	{
		m_pDrawProc->m_pGeoPara = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}

	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	if (m_pEditor)
	{
		if (m_pFtr)
		{
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);
			if (bAnnotToText)
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
				//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if (pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);
				if (pLayer != NULL && pTextLayer != NULL && pTempl != NULL)
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor), m_pFtr, arrAnnots);
					CUndoFtrs undo(m_pEditor, Name());
					for (int i = 0; i < arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType() == SYMTYPE_ANNOTATION)
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr, tab);
							if (pAnnot->ExtractGeoText(m_pFtr, parr, tab, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()))
							{
								for (int j = 0; j < parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);
									PDOC(m_pEditor)->AddObject(pNewFtr, pTextLayer->GetID());
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
								}
							}
						}
					}
					undo.Commit();
				}
				if (pTempl != NULL)delete pTempl;
			}

		}
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}
	CDrawCommand::Finish();
}



void CDrawParalCommand::Abort()
{
	if (m_bSettingModified)
		UpdateParams(TRUE);
	if (m_pDrawProc)
	{
		if (!IsProcOver(m_pDrawProc))
			m_pDrawProc->Abort();

		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	CDrawCommand::Abort();
}


int CDrawParalCommand::GetState()
{
	return GetStateWithSonProcedure(m_pDrawProc);
}

void CDrawParalCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pDrawProc->m_pGeoPara)return;

	if (m_pDrawProc)
	{
		m_pDrawProc->PtClick(pt, flag);
	}

	if (m_pDrawProc->m_pGeoPara->GetDataPointSum() == 0)
	{
		m_pDrawProc->m_nEndSnapped = IsEndSnapped() ? 1 : 0;
	}
	else if (!m_pDrawProc->m_bDrawWidth)
	{
		if (IsEndSnapped())
			m_pDrawProc->m_nEndSnapped |= 2;
		else
			m_pDrawProc->m_nEndSnapped &= (~2);
	}

	if (m_pDrawProc->m_pGeoPara->GetDataPointSum() == 1)
	{
		if (PDOC(m_pEditor)->IsAutoSetAnchor())
		{
			PDOC(m_pEditor)->SetAnchorPoint(m_pDrawProc->m_pGeoPara->GetDataPoint(0));
		}
	}

	return;
}

CProcedure *CDrawParalCommand::GetActiveSonProc(int nMsgType)
{
	if (nMsgType == msgPtClick)
		return NULL;

	return m_pDrawProc;
}

CFeature* CDrawParalCommand::CreateCenterlineFeature()
{
	if (!m_pEditor || !m_pDrawProc || !m_pFtr) return NULL;

	CDlgDataSource *pDS = GETDS(m_pEditor);
	if (!pDS) return NULL;

	CGeoParallel *pGeo = (CGeoParallel*)m_pFtr->GetGeometry();

	if (!pGeo) return NULL;

	CGeoCurve *pNewGeo = pGeo->GetCenterlineCurve();
	if (!pNewGeo) return NULL;

	CFeature *pNewFtr = pDS->CreateObjByNameOrLayerIdx(m_pDrawProc->m_strRetLayer);
	if (!pNewFtr)
	{
		delete pNewGeo;
		return NULL;
	}

	pNewFtr->SetGeometry(pNewGeo);

	return pNewFtr;

}
void CDrawParalCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}
void CDrawParalCommand::OnSonEnd(CProcedure *son)
{
	ASSERT(m_pDrawProc == son);
	if (IsProcAborted(m_pDrawProc) || !m_pDrawProc->m_pGeoPara)
		Abort();
	else
	{
		if (m_pFtr)
		{
			GrBuffer buf;
			m_pDrawProc->m_pGeoPara->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_ClearDrag, &buf);

			CUndoFtrs undo(m_pEditor, Name());

			if (m_pDrawProc->m_bCenterlineMode)
			{
				CFeature *pFtr = CreateCenterlineFeature();
				if (pFtr)
				{
					int layerid = -1;
					CFtrLayer *pFtrLayer = GETDS(m_pEditor)->GetFtrLayer(m_pDrawProc->m_strRetLayer);
					if (pFtrLayer)
					{
						layerid = pFtrLayer->GetID();
					}

					if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
					{
						CScheme *pScheme = gpCfgLibMan->GetScheme(GETDS(m_pEditor)->GetScale());
						float wid = pScheme->GetLayerDefineLineWidth(m_pDrawProc->m_strRetLayer);
						((CGeoCurveBase*)pFtr->GetGeometry())->m_fLineWidth = wid;
					}

					if (m_pEditor->AddObject(pFtr, layerid))
					{
						undo.arrNewHandles.Add(FtrToHandle(pFtr));
					}

				}
			}

			if (m_pDrawProc->m_bBreakup)
			{
				CGeoParallel *pParl = (CGeoParallel*)m_pFtr->GetGeometry();
				if (!pParl)
				{
					Abort();
					return;
				}

				CGeometry *pObj1, *pObj2;
				if (pParl->Separate(pObj1, pObj2))
				{
					CFtrLayer *pAssitLayer = GETDS(m_pEditor)->GetFtrLayer(m_strAssistLayer);

					CFeature * pFtr1 = NULL;
					if (pAssitLayer)
						pFtr1 = pAssitLayer->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(), CLS_GEOCURVE);
					else
						pFtr1 = m_pFtr->Clone();

					if (!pFtr1) return;
					pFtr1->SetID(OUID());
					if (pObj1)pFtr1->SetGeometry(pObj1);

					if (!AddObject(pFtr1))
					{
						delete pFtr1;
						Abort();
						return;
					}

					CFeature * pFtr2 = m_pFtr->Clone();
					if (!pFtr2) return;
					pFtr2->SetID(OUID());
					if (pObj2)pFtr2->SetGeometry(pObj2);

					if (!AddObject(pFtr2, (pAssitLayer == NULL ? -1 : pAssitLayer->GetID())))
					{
						delete pFtr2;
						Abort();
						return;
					}

					delete m_pFtr;

					undo.arrNewHandles.Add(FtrToHandle(pFtr1));
					undo.arrNewHandles.Add(FtrToHandle(pFtr2));
				}

			}
			else if (m_bToSurface)
			{
				CGeoParallel *pParl = (CGeoParallel*)m_pFtr->GetGeometry();
				if (!pParl)
				{
					Abort();
					return;
				}

				CGeometry *pObj1, *pObj2;
				if (pParl->Separate(pObj1, pObj2))
				{
					CArray<PT_3DEX, PT_3DEX> pts1, pts2;
					pObj1->GetShape(pts1);
					pObj2->GetShape(pts2);
					for (int j = 0; j < pts2.GetSize(); j++)
					{
						pts1.Add(pts2[j]);
					}
					pts1.Add(pts1[0]);

					CFeature *pNew = m_pFtr->Clone();
					if (!pNew || !pNew->CreateGeometry(CLS_GEOSURFACE))
					{
						Abort();
						return;
					}
					pNew->GetGeometry()->CreateShape(pts1.GetData(), pts1.GetSize());
					if (!AddObject(pNew))
					{
						delete pNew;
						Abort();
						return;
					}

					delete m_pFtr;

					undo.arrNewHandles.Add(FtrToHandle(pNew));
				}
			}
			else
			{
				if (!AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/)
				{
					Abort();
					return;
				}

				undo.arrNewHandles.Add(FtrToHandle(m_pFtr));
			}

			undo.Commit();

			Finish();
		}
		else
			Abort();
	}
}

void CDrawParalCommand::GetParams(CValueTable &tab)
{
	CDrawCommand::GetParams(tab);
	if (!m_pDrawProc)return;
	_variant_t var;
	var = (long)(m_pDrawProc->m_nCurPenCode);
	tab.AddValue(PF_PENCODE, &CVariantEx(var));
	var = (bool)(m_pDrawProc->m_bClosed);
	tab.AddValue(PF_CLOSED, &CVariantEx(var));
	var = (double)(m_pDrawProc->m_lfSepcialWidth);
	tab.AddValue(PF_WIDTH, &CVariantEx(var));
	var = (bool)(m_pDrawProc->m_bByCenterLine);
	tab.AddValue(PF_PARALLBY, &CVariantEx(var));
	var = (bool)(m_pDrawProc->m_bMouseWidth);
	tab.AddValue(PF_PARALLBYMOUSE, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bOpenIfSnapped);
	tab.AddValue(PF_OPENIFSNAP, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bRectify);
	tab.AddValue(PF_RECTIFY, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bBreakup);
	tab.AddValue(PF_BREAKUP, &CVariantEx(var));

	var = (double)(m_pDrawProc->m_compress.GetLimit());
	tab.AddValue(PF_TOLER, &CVariantEx(var));

	var = (double)(m_pDrawProc->m_fAutoCloseToler);
	tab.AddValue(PF_AUTOCLOSETOLER, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bDoSmooth);
	tab.AddValue(PF_POSTSMOOTH, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bFastDisplayCurve);
	tab.AddValue(PF_FASTCURVE, &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bCenterlineMode);
	tab.AddValue(PF_PARALLCENTERLINE, &CVariantEx(var));

	var = (const char*)(m_pDrawProc->m_strRetLayer);
	tab.AddValue(PF_PARALLRETLAYER, &CVariantEx(var));

	var = (const char*)(m_strAssistLayer);
	tab.AddValue("AssistLayer", &CVariantEx(var));

	var = (bool)(m_pDrawProc->m_bDHeight);
	tab.AddValue(PF_PARALLDHEIGHT, &CVariantEx(var));

	var = (bool)(m_bToSurface);
	tab.AddValue(PF_TOSURFACE, &CVariantEx(var));

}

void CDrawParalCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(PID_DRAWPARAL, StrFromLocalResID(IDS_CMDNAME_DRAWPARALLEL));
	if (m_pDrawProc)
	{
		PT_3D pt;

		param->AddParam(PF_POINT, pt, StrFromResID(IDS_CMDPLANE_POINT));
		param->AddParam(PF_CLOSED, (bool)(m_pDrawProc->m_bClosed), StrFromResID(IDS_CMDPLANE_CLOSE));
		param->AddParam(PF_CLOSEDYACCKEY, 'c', StrFromResID(IDS_CMDPLANE_CLOSE));
		param->BeginOptionParam(PF_PARALLBY, StrFromResID(IDS_CMDPLANE_BY));
		param->AddOption(StrFromResID(IDS_CMDPLANE_BYLINE), 0, ' ', !(m_pDrawProc->m_bByCenterLine));
		param->AddOption(StrFromResID(IDS_CMDPLANE_BYCENTERLINE), 1, ' ', (m_pDrawProc->m_bByCenterLine));
		param->EndOptionParam();
		param->AddParam(PF_PARALLBYMOUSE, (bool)(m_pDrawProc->m_bMouseWidth), StrFromResID(IDS_CMDPLANE_BYMOUSE));

		if (!(m_pDrawProc->m_bMouseWidth) || bForLoad)
		{
			param->AddParam(PF_WIDTH, (m_pDrawProc->m_lfSepcialWidth), StrFromResID(IDS_CMDPLANE_WIDTH));
		}

		if (m_pDrawProc->m_bClosed || bForLoad)
		{
			param->BeginOptionParam(PF_OPENIFSNAP, StrFromResID(IDS_CMDPLANE_OPENIFSNAP));
			param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_pDrawProc->m_bOpenIfSnapped);
			param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_pDrawProc->m_bOpenIfSnapped);
			param->EndOptionParam();
		}

		if (m_pDrawProc->m_nCurPenCode == penLine || bForLoad)
		{
			param->BeginOptionParam(PF_RECTIFY, StrFromResID(IDS_CMDPLANE_RECTIFY));
			param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_pDrawProc->m_bRectify);
			param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_pDrawProc->m_bRectify);
			param->EndOptionParam();
		}

		param->AddParam(PF_RECTIFYACCKEY, 'r', StrFromResID(IDS_CMDPLANE_RECTIFY));

		param->AddParam(PF_REVERSEACCKEY, 't', StrFromResID(IDS_CMDPLANE_REVERSE));//cjc 2012年10月30日

		param->AddParam(PF_TRACKLINEACCKEY, 'w', StrFromResID(IDS_CMDPLANE_TRACKLINE));

		param->AddParam(PF_BREAKUP, (bool)m_pDrawProc->m_bBreakup, StrFromResID(IDS_CMDPLANE_BREAKUP));

		param->AddParam(PF_BREAKUPACCKEY, 'b', StrFromResID(IDS_CMDPLANE_BREAKUP));

		if (m_pDrawProc->m_nCurPenCode == penStream || bForLoad)
		{
			// 容差，自动闭合容差，采集后光滑
			param->AddParam(PF_TOLER, m_pDrawProc->m_compress.GetLimit(), StrFromResID(IDS_CMDPLANE_TOLERANCE));
			param->AddParam(PF_AUTOCLOSETOLER, m_pDrawProc->m_fAutoCloseToler, StrFromResID(IDS_CMDPLANE_AUTOCLOSETOLER));

			param->BeginOptionParam(PF_POSTSMOOTH, StrFromResID(IDS_CMDPLANE_POSTSMOOTH));
			param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_pDrawProc->m_bDoSmooth);
			param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_pDrawProc->m_bDoSmooth);
			param->EndOptionParam();
		}

		if (m_pDrawProc->m_nCurPenCode == penSpline || bForLoad)
		{
			param->BeginOptionParam(PF_FASTCURVE, StrFromResID(IDS_CMDPLANE_FASTCURVE));
			param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_pDrawProc->m_bFastDisplayCurve);
			param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_pDrawProc->m_bFastDisplayCurve);
			param->EndOptionParam();

			// 转换时constLine的生成
			if (!bForLoad)
			{
				CArray<PT_3DEX, PT_3DEX> arrtmp;
				arrtmp.Append(m_pDrawProc->m_arrPts);
				// 转到快速显示时
				if (m_pDrawProc->m_bFastDisplayCurve)
				{
					for (int i = 0; i < arrtmp.GetSize(); i++)
					{
						PT_3DEX &t = arrtmp.ElementAt(i);
						t.pencode = penLine;
					}

				}

				CGeoCurve curve;
				curve.CreateShape(arrtmp.GetData(), arrtmp.GetSize());

				m_pEditor->UpdateDrag(ud_ClearDrag);

				GrBuffer cbuf, vbuf;
				const CShapeLine *pSL = curve.GetShape();

				if (m_pDrawProc->m_bClosed && arrtmp.GetSize() > 2)
				{
					arrtmp.Add(arrtmp.GetAt(0));
					curve.CreateShape(arrtmp.GetData(), arrtmp.GetSize());
					pSL->GetConstGrBuffer(&cbuf, true);
					arrtmp.RemoveAt(arrtmp.GetSize() - 1);
					curve.CreateShape(arrtmp.GetData(), arrtmp.GetSize());

				}
				else
				{
					pSL->GetConstGrBuffer(&cbuf);
				}

				long color = m_pDrawProc->m_pGeoPara->GetColor();
				if (color == FTRCOLOR_BYLAYER)
				{
					color = m_pDrawProc->m_layCol;
				}
				cbuf.SetAllColor(color);
				m_pEditor->UpdateDrag(ud_SetConstDrag, &cbuf);


			}
		}

		param->AddLineTypeParam(PF_PENCODE, (m_pDrawProc->m_nCurPenCode), StrFromResID(IDS_CMDPLANE_LINETYPE));

		param->AddParam(PF_PARALLCENTERLINE, (bool)(m_pDrawProc->m_bCenterlineMode), StrFromResID(IDS_CMDPLANE_CENTERLINEMODE));

		if (m_pDrawProc->m_bCenterlineMode || bForLoad)
		{
			if (m_pDrawProc->m_strRetLayer.IsEmpty())
			{
				m_pDrawProc->m_strRetLayer = GETCURFTRLAYER(m_pEditor)->GetName();
			}

			param->AddLayerNameParam(PF_PARALLRETLAYER, (LPCTSTR)m_pDrawProc->m_strRetLayer, StrFromResID(IDS_CENTERLINE_LAYNAME));
		}

		param->AddParam(PF_PARALLDHEIGHT, (bool)(m_pDrawProc->m_bDHeight), StrFromResID(IDS_CMDPLANE_DHEIGHTMODE));

		if (!m_pDrawProc->m_bBreakup || bForLoad)
		{
			param->AddParam(PF_TOSURFACE, (bool)(m_bToSurface), StrFromResID(IDS_CMDPLANE_TOSURFACE));
		}

		if (m_pDrawProc->m_bBreakup || bForLoad)
		{
			// 容差，自动闭合容差，采集后光滑
			param->AddLayerNameParam("AssistLayer", (LPCTSTR)m_strAssistLayer, StrFromResID(IDS_ASSISTLINE_LAYNAME));
		}

	}
}


void CDrawParalCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	if (!m_pDrawProc) return;
	const CVariantEx *var;
	//	bool bNeedRefresh = false;

	if (tab.GetValue(0, PF_PENCODE, var))
	{
		m_pDrawProc->m_nCurPenCode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_NODEWID, var))
	{
		m_pDrawProc->m_fCurNodeWid = (float)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_NODETYPE, var))
	{
		m_pDrawProc->m_nCurNodeType = (short)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_CLOSED, var))
	{
		m_pDrawProc->m_bClosed = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_CLOSEDYACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 'c' || ch == 'C')
		{
			m_pDrawProc->m_bClosed = !m_pDrawProc->m_bClosed;
		}
	}
	if (tab.GetValue(0, PF_TRACKLINEACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 'w' || ch == 'W')
		{
			m_pDrawProc->TrackLine();
		}
	}
	if (tab.GetValue(0, PF_TOLER, var))
	{
		m_pDrawProc->m_compress.SetLimit((double)(_variant_t)*var);
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_POINT, var))
	{
		CArray<PT_3DEX, PT_3DEX> arrPts;
		var->GetShape(arrPts);
		PT_3D point = arrPts.GetAt(0);
		PtClick(point, 0);
		PtMove(point);
		m_pEditor->RefreshView();
	}
	if (tab.GetValue(0, PF_PARALLBY, var))
	{
		m_pDrawProc->m_bByCenterLine = (bool)(_variant_t)*var;
		if (m_pDrawProc->m_bByCenterLine)
		{
			m_pDrawProc->m_bDHeight = FALSE;
		}
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_PARALLBYMOUSE, var))
	{
		//		bNeedRefresh = true;
		m_pDrawProc->m_bMouseWidth = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_WIDTH, var))
	{
		m_pDrawProc->m_lfSepcialWidth = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (m_pDrawProc->m_pGeoPara)
		m_pDrawProc->m_pGeoPara->EnableClose(m_pDrawProc->m_bClosed);

	if (tab.GetValue(0, PF_OPENIFSNAP, var))
	{
		m_pDrawProc->m_bOpenIfSnapped = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_RECTIFY, var))
	{
		m_pDrawProc->m_bRectify = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_RECTIFYACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 'r' || ch == 'R')
		{
			m_pDrawProc->m_bRectify = !m_pDrawProc->m_bRectify;
		}

	}
	//cjc 2012年10月30日
	if (tab.GetValue(0, PF_REVERSEACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 't' || ch == 'T')
		{
			m_pDrawProc->ReverseLine();
		}

	}

	if (tab.GetValue(0, PF_BREAKUP, var))
	{
		m_pDrawProc->m_bBreakup = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_BREAKUPACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 'b' || ch == 'B')
		{
			m_pDrawProc->m_bBreakup = !m_pDrawProc->m_bBreakup;
		}

	}

	if (tab.GetValue(0, PF_TOLER, var))
	{
		m_pDrawProc->m_compress.SetLimit((double)(_variant_t)*var);
	}
	if (tab.GetValue(0, PF_AUTOCLOSETOLER, var))
	{
		m_pDrawProc->m_fAutoCloseToler = (double)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_POSTSMOOTH, var))
	{
		m_pDrawProc->m_bDoSmooth = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_FASTCURVE, var))
	{
		m_pDrawProc->m_bFastDisplayCurve = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_PARALLCENTERLINE, var))
	{
		m_pDrawProc->m_bCenterlineMode = (bool)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_PARALLRETLAYER, var))
	{
		m_pDrawProc->m_strRetLayer = (const char*)(_bstr_t)(_variant_t)*var;
		if (m_pDrawProc->m_strRetLayer.IsEmpty())
		{
			m_pDrawProc->m_strRetLayer = GETCURFTRLAYER(m_pEditor)->GetName();
		}
	}
	if (tab.GetValue(0, "AssistLayer", var))
	{
		m_strAssistLayer = (const char*)(_bstr_t)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_PARALLDHEIGHT, var))
	{
		if (m_pDrawProc->m_bByCenterLine)
		{
			m_pDrawProc->m_bDHeight = FALSE;
		}
		else
		{
			m_pDrawProc->m_bDHeight = (bool)(_variant_t)*var;
		}
	}
	if (tab.GetValue(0, PF_TOSURFACE, var))
	{
		m_bToSurface = (bool)(_variant_t)*var;
	}
	SetSettingsModifyFlag();

	CDrawCommand::SetParams(tab, bInit);
}



BOOL CDrawParalCommand::Keyin(LPCTSTR text)
{
	if (!m_pDrawProc)
		return FALSE;

	// 通过长度和角度指定点
	char buf[100] = { 0 };

	strncpy(buf, text, 100 - 1);
	char *stop = NULL;
	double len = strtod(buf, &stop);
	if (stop != NULL && strlen(stop) == 0)
	{
		int num = m_pDrawProc->m_arrPts.GetSize();
		if (num > 0)
		{
			PT_3D curPt = m_pEditor->GetCoordWnd().m_ptGrd;
			PT_3DEX pt0 = m_pDrawProc->m_arrPts[num - 1], pt1;
			double dx = curPt.x - pt0.x, dy = curPt.y - pt0.y;
			double dis = sqrt(dx*dx + dy*dy);
			if (dis < GraphAPI::GetDisTolerance())
			{
				pt1.x = pt0.x + len;
				pt1.y = pt0.y;
				pt1.z = curPt.z;
			}
			else
			{
				pt1.x = pt0.x + dx / dis * len;
				pt1.y = pt0.y + dy / dis * len;
				pt1.z = curPt.z;
			}

			PtClick(pt1, 0);
			PtMove(curPt);
			m_pEditor->RefreshView();

			return TRUE;
		}
	}

	return FALSE;
}



//////////////////////////////////////////////////////////////////////
// CCopyCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCopyCommand, CMoveCommand)

CCopyCommand::CCopyCommand()
{

}

CCopyCommand::~CCopyCommand()
{

}


CString CCopyCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_COPY);
}

void CCopyCommand::PtMove(PT_3D &pt)
{
	if (m_nStep == 1)
	{
		GrBuffer buf;

		double matrix[16];
		Matrix44FromMove(pt.x - m_ptMoveStart.x, pt.y - m_ptMoveStart.y, pt.z - m_ptMoveStart.z, matrix);

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

		int num = m_arrObjs.GetSize();
		for (int i = 0; i < num && num < 100; i++)
		{
			CFeature *pFtr = HandleToFtr(m_arrObjs.GetAt(i))->Clone();

			CGeometry *pGeo = pFtr->GetGeometry();
			pGeo->Transform(matrix);
			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
			{
				CPlotTextEx plot;
				TEXT_SETTINGS0  settings;
				((CGeoText*)pGeo)->GetSettings(&settings);

				settings.fHeight *= pDS->GetSymbolDrawScale();
				plot.SetSettings(&settings);

				plot.SetShape(((CGeoText*)pGeo)->GetShape());
				plot.SetText(((CGeoText*)pGeo)->GetText());
				plot.GetOutLineBuf(&buf);
			}
			else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
			{
				CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(m_arrObjs.GetAt(i)));
				if (pLayer)
				{
					pDS->DrawFeature(pFtr, &buf, TRUE, 0, pLayer->GetName());
				}
			}
			else
				pFtr->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());


			delete pFtr;

		}

		buf.BeginLineString(0, 0);
		buf.MoveTo(&m_ptMoveStart);
		buf.LineTo(&pt);

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
	}
}

VOID CCopyCommand::PtClick(PT_3D &pt, int flag)
{

	if (m_nStep == 0)
	{
		m_ptMoveStart = pt;
		int num = 0;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num > 0)
		{
			if (flag == SELSTAT_POINTSEL);
			else if (!CanGetSelObjs(flag))return;
			m_arrObjs.SetSize(num);
			memcpy(m_arrObjs.GetData(), handles, sizeof(FTR_HANDLE)*num);
			m_nStep = 1;

			m_pEditor->CloseSelector();
			GotoState(PROCSTATE_PROCESSING);
		}
	}
	else if (m_nStep == 1)
	{
		CDataQuery *pDQ = m_pEditor->GetDataQuery();
		double matrix[16];
		Matrix44FromMove(pt.x - m_ptMoveStart.x, pt.y - m_ptMoveStart.y, pt.z - m_ptMoveStart.z, matrix);

		//	CArray<CPFeature,CPFeature> arrObjs;

		CUndoFtrs undo(m_pEditor, Name());
		//	memcpy(pNew->matrix,matrix,sizeof(matrix));

		int num = m_arrObjs.GetSize();
		GProgressStart(num);
		for (int i = 0; i < num; i++)
		{
			GProgressStep();

			CPFeature pFtr = HandleToFtr(m_arrObjs.GetAt(i));
			pFtr = pFtr->Clone();
			pFtr->SetID(OUID());

			CGeometry *pGeo = pFtr->GetGeometry();
			pGeo->Transform(matrix);

			m_pEditor->AddObject(pFtr, m_pEditor->GetFtrLayerIDOfFtr(m_arrObjs.GetAt(i)));
			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_arrObjs.GetAt(i)), pFtr);
			undo.arrNewHandles.Add(FtrToHandle(pFtr));
		}

		GProgressEnd();

		//	pNew->arrHandles.Copy(arrObjs);

		undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->CloseSelector();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		//		Finish();
		// 支持连续拷贝
		m_nStep = 1;
	}
	return;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CLakeHatchCommand, CEditCommand)

CLakeHatchCommand::CLakeHatchCommand()
{
	m_nStep = 0;
}


CLakeHatchCommand::~CLakeHatchCommand()
{
}


CString CLakeHatchCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_LAKEHATCH);
}


void CLakeHatchCommand::Start()
{
	m_pEditor->OpenSelector(SELMODE_SINGLE);
	m_nStep = 0;

	CEditCommand::Start();
}


void CLakeHatchCommand::Abort()
{
	m_pEditor->CloseSelector();
	m_nStep = -1;
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CEditCommand::Abort();
}



void CLakeHatchCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		m_ptMoveStart = pt;
		int num = 0;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num == 1)
		{
			if (flag == SELSTAT_POINTSEL);
			else if (!CanGetSelObjs(flag))return;

			CGeometry *pGeo = HandleToFtr(handles[0])->GetGeometry();
			if (!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				Abort();
				return;
			}

			m_ObjFtr = HandleToFtr(handles[0]);
			m_nStep = 1;

			m_pEditor->CloseSelector();
			GotoState(PROCSTATE_PROCESSING);
		}
	}
	else if (m_nStep == 1)
	{
		CDlgDataSource *pDS = GETDS(m_pEditor);

		CArray<PT_3DEX, PT_3DEX> pts;
		const CShapeLine *pShape = m_ObjFtr->GetGeometry()->GetShape();
		pShape->GetPts(pts);

		int polysnum = pts.GetSize();

		if (polysnum < 3)
		{
			Abort();
			return;
		}

		double dis0 = GraphAPI::GGet2DDisOf2P(m_ptMoveStart, pt);

		if (fabs(dis0) < 1e-4)
		{
			Abort();
			return;
		}

		int layid = m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(m_ObjFtr));
		Envelope evlp = m_ObjFtr->GetGeometry()->GetEnvelope();

		CArray<PT_3D, PT_3D> polys;
		polys.SetSize(polysnum);
		for (int i = 0; i < polysnum; i++)
		{
			COPY_3DPT(polys[i], pts[i]);
		}

		double wid = evlp.Width() / 10;

		CGrTrim trim;
		trim.InitTrimPolygon(polys.GetData(), polysnum, wid);
		trim.m_bIntersectHeight = TRUE;

		double fInter = 2;

		CUndoFtrs undo(m_pEditor, Name());

		int sum = 2;
		while (sum--)
		{
			double dis, fStarty;
			if (sum == 1)
			{
				dis = dis0;
				fStarty = evlp.m_yl + fInter;
			}
			else if (sum == 0)
			{
				dis = dis0 / 2;
				fStarty = evlp.m_yl + fInter / 2;
			}

			CArray<PT_3DEX, PT_3DEX> arr;
			for (double y = fStarty; y < evlp.m_yh; y += fInter)
			{
				PT_3D pt1(evlp.m_xl, y, 0), pt2(evlp.m_xh, y, 0);

				trim.TrimLine(&pt1, &pt2);
				int nTrimedLinesNum = trim.GetTrimedLinesNum();
				if (nTrimedLinesNum >= 1)
				{
					trim.GetTrimedLine(0, &pt1, &pt2);

					// 取首尾两段长度为dis的线段
					double dis0 = GraphAPI::GGet2DDisOf2P(pt1, pt2);

					if (dis0 > 2 * dis)
					{
						PT_3DEX ptStart, ptEnd;

						double ratio = dis / dis0;
						ptStart = PT_3DEX(pt1, penMove);
						ptEnd.x = pt1.x + ratio * (pt2.x - pt1.x);
						ptEnd.y = pt1.y + ratio * (pt2.y - pt1.y);
						ptEnd.z = pt1.z + ratio * (pt2.z - pt1.z);
						ptEnd.pencode = penLine;
						arr.Add(ptStart);
						arr.Add(ptEnd);

						ratio = 1 - dis / dis0;
						ptStart.x = pt1.x + ratio * (pt2.x - pt1.x);
						ptStart.y = pt1.y + ratio * (pt2.y - pt1.y);
						ptStart.z = pt1.z + ratio * (pt2.z - pt1.z);
						ptStart.pencode = penMove;
						ptEnd = PT_3DEX(pt2, penLine);
						arr.Add(ptStart);
						arr.Add(ptEnd);

						if (ptStart.x < 1e-4 || ptStart.y < 1e-4)
						{
							int a = 10;
						}
					}
				}
			}

			CFeature *pNewFtr = pDS->GetFtrLayer(layid)->CreateDefaultFeature(pDS->GetScale(), CLS_GEOCURVE);
			for (int i = 0; i < arr.GetSize(); i += 2)
			{
				CFeature *pNew = pNewFtr->Clone();
				if (pNew->GetGeometry()->CreateShape(arr.GetData() + i, 2))
				{
					m_pEditor->AddObject(pNew, layid);
					undo.AddNewFeature(FtrToHandle(pNew));
				}
			}
			delete pNewFtr;
		}

		undo.Commit();

		// 将晕线编为一组
		int size = undo.arrNewHandles.GetSize();
		if (size > 1)
		{
			CString strGroupName;
			int num = 1;
			while (1)
			{
				strGroupName.Format("%s%d", Name(), num);

				int nGroupNum = pDS->GetObjectGroupCount();
				// 检查重名
				for (int i = 0; i < nGroupNum; i++)
				{
					if (strGroupName.CompareNoCase(pDS->GetObjectGroup(i)->name) == 0)
					{
						break;
					}
				}

				if (i > nGroupNum - 1)
				{
					break;
				}

				num++;
			}

			ObjectGroup *pGroup = new ObjectGroup;
			if (!pGroup)
			{
				Abort();
				return;
			}

			strncpy(pGroup->name, strGroupName, 16);
			pGroup->name[15] = '\0';

			pDS->AddObjectGroup(pGroup);

			int idGroup = pGroup->id;

			for (int i = 0; i < size; i++)
			{
				CFeature *pFtr = (CFeature*)undo.arrNewHandles[i];
				if (!pFtr || pFtr->IsInObjectGroup(idGroup))
				{
					continue;
				}

				pFtr->AddObjectGroup(idGroup);

				pGroup->AddObject(pFtr);

			}

		}

		m_pEditor->OnSelectChanged(TRUE);
		m_pEditor->CloseSelector();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		Finish();
	}
	return;
}

void CLakeHatchCommand::PtMove(PT_3D &pt)
{
	if (m_nStep == 1)
	{
		GrBuffer buf;

		buf.BeginLineString(0, 0);
		buf.MoveTo(&m_ptMoveStart);
		buf.LineTo(&pt);

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTrimContourAcrossBankCommand, CEditCommand)

CTrimContourAcrossBankCommand::CTrimContourAcrossBankCommand()
{
	m_nStep = 0;
}


CTrimContourAcrossBankCommand::~CTrimContourAcrossBankCommand()
{
}


CString CTrimContourAcrossBankCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CONTOURACROSSBANK);
}


void CTrimContourAcrossBankCommand::Start()
{
	m_pEditor->OpenSelector();
	m_nStep = 0;

	m_strLayerName = StrFromResID(IDS_INDEX_CONTOUR) + "," + StrFromResID(IDS_STD_CONTOUR) + "," + StrFromResID(IDS_INTER_CONTOUR);

	CEditCommand::Start();
}


void CTrimContourAcrossBankCommand::Abort()
{
	m_pEditor->CloseSelector();
	m_nStep = -1;
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CEditCommand::Abort();
}

bool GLineIntersectLineSeg1(double x0, double y0, double x1, double y1, double x2, double y2, double z2, double x3, double y3, double z3, double *x, double *y, double *z, double *t, double *t3);

//此函数不再使用模板 cz.2015.4.30
int GetCurveIntersectCurve(PT_3DEX *pts1, int num1, PT_3DEX *pts2, int num2, CArray<PtIntersect, PtIntersect> &arrIntersectPts)
{
	if (!pts1 || !pts2 || num1 < 2 || num2 < 2) return false;

	Envelope e1, e2;
	e1.CreateFromPts(pts1, num1, sizeof(PT_3DEX));
	e2.CreateFromPts(pts2, num2, sizeof(PT_3DEX));

	if (!e1.bIntersect(&e2)) return false;

	PT_3D ret;
	double t1, t2;
	bool bIntersect = false;
	for (int i = 0; i < num1 - 1; i++)
	{
		if ((pts1 + i + 1)->pencode == penMove) continue;
		for (int j = 0; j < num2 - 1; j++)
		{
			if ((pts2 + j + 1)->pencode == penMove) continue;
			//求相交点
			if (!GLineIntersectLineSeg1(pts1[i].x, pts1[i].y, pts1[i + 1].x, pts1[i + 1].y, pts2[j].x, pts2[j].y, pts2[j].z,
				pts2[j + 1].x, pts2[j + 1].y, pts2[j + 1].z, &(ret.x), &(ret.y), &(ret.z), &t1, &t2))
			{
				continue;
			}
			else
			{
				//首尾相连
				if ((i == 0 || i == (num1 - 2)) && (j == 0 || j == (num2 - 2)) && (fabs(t1 - 1) < 1e-10 || fabs(t1) < 1e-10) && (fabs(t2 - 1) < 1e-10 || fabs(t2) < 1e-10))
					continue;

				if (fabs(pts1[i].x - pts1[i + 1].x) >= 1e-4 && fabs(pts1[i].y - pts1[i + 1].y) < 1e-4)
					ret.z = pts1[i].z;
				else
				{
					if (fabs(pts1[i].x - pts1[i + 1].x) > fabs(pts1[i].y - pts1[i + 1].y))
						ret.z = pts1[i].z + (ret.x - pts1[i].x)*(pts1[i].z - pts1[i + 1].z) / (pts1[i].x - pts1[i + 1].x);
					else
						ret.z = pts1[i].z + (ret.y - pts1[i].y)*(pts1[i].z - pts1[i + 1].z) / (pts1[i].y - pts1[i + 1].y);
				}

				PtIntersect item;
				item.IntersectFlag = -1;
				item.pt = ret;
				item.lfRatio = i + t1;

				//按照 t 大小排序插入
				int size = arrIntersectPts.GetSize();
				for (int k = 0; k < size && item.lfRatio >= arrIntersectPts[k].lfRatio; k++);
				if (k < size)arrIntersectPts.InsertAt(k, item);
				else arrIntersectPts.Add(item);
			}
		}
	}

	return arrIntersectPts.GetSize();
}

static bool GLineIntersectLineSeg1(double x0, double y0, double x1, double y1, double x2, double y2, double z2, double x3, double y3, double z3, double *x, double *y, double *z, double *t, double *t3)
{
	double vector1x = x1 - x0, vector1y = y1 - y0;
	double vector2x = x3 - x2, vector2y = y3 - y2, vector2z = z3 - z2;
	double delta = vector1x*vector2y - vector1y*vector2x;
	if (delta<1e-6 && delta>-1e-6)return false;         //平行无交点
	double t1 = ((x2 - x0)*vector2y - (y2 - y0)*vector2x) / delta;
	double xr = x0 + t1*vector1x, yr = y0 + t1*vector1y;
	if (t1 < 0 || t1>1)
	{
		if (fabs(xr - x0) <= 1e-6 && fabs(yr - y0) <= 1e-6);
		else if (fabs(xr - x1) <= 1e-6 && fabs(yr - y1) <= 1e-6);
		else return false;
	}

	double t2 = ((x2 - x0)*vector1y - (y2 - y0)*vector1x) / delta;

	if (t2 < 0 || t2>1)
	{
		if (fabs(xr - x2) <= 1e-6 && fabs(yr - y2) <= 1e-6);
		else if (fabs(xr - x3) <= 1e-6 && fabs(yr - y3) <= 1e-6);
		else return false;
	}

	if (x)*x = x0 + t1*vector1x;
	if (y)*y = y0 + t1*vector1y;
	if (z)*z = z2 + t2*vector2z;
	if (t)*t = t1;
	if (t3)
	{
		*t3 = t2;
	}
	return true;
}


void CTrimContourAcrossBankCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ModifyContourAcrossBankCommand", Name());
	param->AddLayerNameParamEx("LayerName", (LPCTSTR)m_strLayerName, StrFromResID(IDS_CONTOUR_LAYERNAME), NULL, LAYERPARAMITEM_LINE);
}

void CTrimContourAcrossBankCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "LayerName", var))
	{
		m_strLayerName = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab, bInit);
}

void CTrimContourAcrossBankCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strLayerName;
	tab.AddValue("LayerName", &CVariantEx(var));
}

void CTrimContourAcrossBankCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;

		int num = 0;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num > 0)
		{
			for (int i = num - 1; i >= 0; i--)
			{
				m_arrObjFtrs.Add(handles[i]);
			}

			m_nStep = 1;

			m_pEditor->CloseSelector();
			GotoState(PROCSTATE_PROCESSING);
		}
	}

	if (m_nStep == 1)
	{
		CDlgDataSource *pDS = GETDS(m_pEditor);

		CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());

		CCellDefLib *pCellLib = GetCellDefLib();
		if (!pCellLib)
		{
			Abort();
			return;
		}

		int nObjSum = m_arrObjFtrs.GetSize();
		GProgressStart(nObjSum);

		CUndoFtrs undo(m_pEditor, Name());
		for (int m = 0; m < nObjSum; m++)
		{
			GProgressStep();

			// 检测是否为坡坎类地物
			CFeature *pFtr = HandleToFtr(m_arrObjFtrs[m]);
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			{
				continue;
			}

			CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(pFtr);
			if (!pFtrLayer) continue;

			CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(pFtrLayer->GetName());
			if (!pSchemeLayer) continue;

			// 坎是否在前进方向的左边
			BOOL bLeft = TRUE;
			// 坎高（图上距离）
			double fLen = -1;
			for (int j = 0; j < pSchemeLayer->GetSymbolCount(); j++)
			{
				CSymbol *pSym = pSchemeLayer->GetSymbol(j);
				if (pSym && pSym->GetType() == SYMTYPE_CELLLINETYPE)
				{
					CCellLinetype *pCellLine = (CCellLinetype*)pSym;

					if (pCellLine->m_nPlacePos != CCellLinetype::Cycle)
						continue;

					int nIndex = pCellLib->GetCellDefIndex(pCellLine->m_strCellDefName) + 1;

					CellDef cell = pCellLib->GetCellDef(nIndex - 1);

					int nsum = cell.m_pgr->GetLinePts(NULL);
					// 坎
					if (nsum >= 2)
					{
						//坎齿有可能是旋转90度的
						double fWidth = cell.m_pgr->GetEnvelope().Width()*fabs(pCellLine->m_fkx);
						double fHeight = cell.m_pgr->GetEnvelope().Height()*fabs(pCellLine->m_fky);
						if (fLen < 0 || fLen < fHeight || fLen < fWidth)
						{
							fLen = max(fHeight, fWidth);

							if((pCellLine->m_fky < 0) || fabs(pCellLine->m_fAngle-PI)<1e-6 || 
								(pCellLine->m_fBaseYOffset < 0 && fabs(pCellLine->m_fBaseYOffset) > fHeight / 2))
							{
								bLeft = FALSE;
							}
							else
							{
								bLeft = TRUE;
							}
						}

					}

				}
			}

			if (fLen <= 0) continue;

			// 等高线距离坡坎母线断开距离
			double fDis = (fLen + 0.3)*pDS->GetScale() / 1000 * pDS->GetDrawScaleByUser();

			// 寻找每个坡坎地物与之相交的等高线
			CArray<PT_3DEX, PT_3DEX> pts0;
			const CShapeLine *pShape = pGeo->GetShape();
			pShape->GetPts(pts0);

			// 穿越母线等高线	
			CArray<FtrIntersect, FtrIntersect> arrObjRatio1, arrObjRatio2;

			Envelope e1, e2;
			e1.CreateFromPts(pts0.GetData(), pts0.GetSize(), sizeof(PT_3DEX));
			e2 = e1;
			e2.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS, 1);
			m_pEditor->GetDataQuery()->FindObjectInRect(e2, m_pEditor->GetCoordWnd().m_pSearchCS);

			int num1;
			const CPFeature * ppftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
			if (!ppftr1)
			{
				Abort();
				return;
			}
			CArray<CPFeature, CPFeature> ftr1;
			ftr1.SetSize(num1);
			memcpy(ftr1.GetData(), ppftr1, num1*sizeof(*ppftr1));

			CGeometry *pObj;
			const CShapeLine *pSL;

			for (int i = 0; i < num1; i++)
			{
				if (ftr1[i] == pFtr) continue;
				pObj = ftr1[i]->GetGeometry();
				if (!pObj)continue;

				CFtrLayer *pftr1Layer = pDS->GetFtrLayerOfObject(ftr1[i]);
				if (!pftr1Layer) continue;

				// 是否为等高线地物			
				if (!CheckNameForLayerCode(pDS, pftr1Layer->GetName(), m_strLayerName))
				{
					continue;
				}

				if (pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)))continue;

				if (pObj->GetDataPointSum() < 2)continue;

				//精细检验是否与第一线段相交，记录交点
				pSL = pObj->GetShape();
				if (!pSL)continue;

				CArray<PT_3DEX, PT_3DEX> arr;
				pSL->GetPts(arr);

				FtrIntersect item;
				item.ftr = FtrToHandle(ftr1[i]);
				if (GetCurveIntersectCurve(arr.GetData(), arr.GetSize(), pts0.GetData(), pts0.GetSize(), item.pts))
				{
					arrObjRatio1.Add(item);
				}
			}

			// 穿越坡坎等高线
			CArray<PT_3DEX, PT_3DEX> pts1;
			pts1.SetSize(pts0.GetSize());
			double fWidth = fDis;
			if (!bLeft)
			{
				fWidth = -fDis;
			}

			if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))//双线斜坡，则分别用母线和辅助线去打断
			{
				int k = 0;
				for(k = 1; k<pts0.GetSize(); k++)
				{
					if(pts0[k].pencode==penMove) break;
				}
				for(; k<pts0.GetSize(); k++)
				{
					pts1.Add(pts0[k]);
				}
				pts0.SetSize(k);
			}
			else if (!GraphAPI::GGetParallelLine(pts0.GetData(), pts0.GetSize(), fWidth, pts1.GetData()))
			{
				Abort();
				return;
			}

			//用于后面裁剪时作分段取舍的判断
			CArray<PT_3DEX, PT_3DEX> pts2;
			pts2.Append(pts0);
			for (i = pts1.GetSize() - 1; i >= 0; i--)
			{
				pts2.Add(pts1[i]);
			}

			// 增加检测母线和坎线两侧的线段
			pts1.InsertAt(0, pts0[0]);
			pts1.Add(pts0[pts0.GetUpperBound()]);

			e1.CreateFromPts(pts1.GetData(), pts1.GetSize(), sizeof(PT_3DEX));
			e2 = e1;
			e2.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS, 1);
			m_pEditor->GetDataQuery()->FindObjectInRect(e2, m_pEditor->GetCoordWnd().m_pSearchCS);

			int num2;
			const CPFeature * ppftr2 = m_pEditor->GetDataQuery()->GetFoundHandles(num2);
			if (!ppftr2)
			{
				Abort();
				return;
			}
			CArray<CPFeature, CPFeature> ftr2;
			ftr2.SetSize(num2);
			memcpy(ftr2.GetData(), ppftr2, num2*sizeof(*ppftr2));

			for (i = 0; i < num2; i++)
			{
				if (ftr2[i] == pFtr) continue;
				pObj = ftr2[i]->GetGeometry();
				if (!pObj)continue;

				CFtrLayer *pftr2Layer = pDS->GetFtrLayerOfObject(ftr2[i]);
				if (!pftr2Layer) continue;

				// 是否为等高线地物
				if (!CheckNameForLayerCode(pDS, pftr2Layer->GetName(), m_strLayerName))
				{
					continue;
				}

				if (pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)))continue;

				if (pObj->GetDataPointSum() < 2)continue;

				pSL = pObj->GetShape();
				if (!pSL)continue;

				CArray<PT_3DEX, PT_3DEX> arr;
				pSL->GetPts(arr);

				FtrIntersect item;
				item.ftr = FtrToHandle(ftr2[i]);
				if (GetCurveIntersectCurve(arr.GetData(), arr.GetSize(), pts1.GetData(), pts1.GetSize(), item.pts))
				{
					arrObjRatio2.Add(item);
				}
			}

			// 对相交信息进行合并
			CArray<FtrIntersect, FtrIntersect> arrObjRatio;
			int numIntersect1 = arrObjRatio1.GetSize();
			int numIntersect2 = arrObjRatio2.GetSize();

			CArray<FTR_HANDLE, FTR_HANDLE> arrProcessedFtrs;

			for (i = 0; i < numIntersect1; i++)
			{
				for (int j = 0; j < numIntersect2; j++)
				{
					if (arrObjRatio2[j].ftr == arrObjRatio1[i].ftr)
					{
						break;
					}
				}

				if (j < numIntersect2)
				{
					arrProcessedFtrs.Add(arrObjRatio2[j].ftr);

					// 排序
					for (int n = 0; n < arrObjRatio2[j].pts.GetSize(); n++)
					{
						PtIntersect item = arrObjRatio2[j].pts[n];
						int size = arrObjRatio1[i].pts.GetSize();
						for (int k = 0; k < size && item.lfRatio >= arrObjRatio1[i].pts[k].lfRatio; k++);
						if (k < size)arrObjRatio1[i].pts.InsertAt(k, item);
						else arrObjRatio1[i].pts.Add(item);
					}
				}
			}

			for (i = 0; i < numIntersect2; i++)
			{
				int numftrs = arrProcessedFtrs.GetSize();
				for (int j = 0; j < numftrs; j++)
				{
					if (arrProcessedFtrs[j] == arrObjRatio2[i].ftr)
					{
						break;
					}
				}

				if (j < numftrs)
				{
					continue;
				}

				arrObjRatio1.Add(arrObjRatio2[i]);
			}

			// 依次处理等高线
			for (i = 0; i < arrObjRatio1.GetSize(); i++)
			{
				CFeature *pFtrRatio = HandleToFtr(arrObjRatio1[i].ftr);
				CFeature *pNewFtr0 = pFtrRatio->Clone();
				if (!pNewFtr0)
				{
					Abort();
					return;
				}

				int layidnew = m_pEditor->GetFtrLayerIDOfFtr(arrObjRatio1[i].ftr);

				undo.AddOldFeature(arrObjRatio1[i].ftr);

				m_pEditor->DeleteObject(arrObjRatio1[i].ftr);

				CArray<PT_3DEX, PT_3DEX> ptsRatio;
				pFtrRatio->GetGeometry()->GetShape()->GetPts(ptsRatio);

				int numRatio = ptsRatio.GetSize();

				CArray<PtIntersect, PtIntersect> &ptIntersect = arrObjRatio1[i].pts;
				// 依次对交点作处理，通过当前段的中点到母线和坎的距离来判断是否在穿越坎
				CArray<PT_3DEX, PT_3DEX> newPts;
				int penCode = ptsRatio[numRatio - 1].pencode;
				CUIntArray arrIndex;
				for (int j = 0; j < ptIntersect.GetSize(); j++)
				{
					if (j == 0)
					{
						int index = (int)ptIntersect[0].lfRatio;
						for (int k = 0; k <= index; k++)
						{
							newPts.Add(PT_3DEX(ptsRatio[k], penCode));
						}
						newPts.Add(PT_3DEX(ptIntersect[0].pt, penCode));

						arrIndex.Add(-1);
						arrIndex.Add(newPts.GetSize() - 1);
					}
					else
					{
						int index1, index2;
						index1 = (int)ptIntersect[j - 1].lfRatio;
						index2 = (int)ptIntersect[j].lfRatio;
						newPts.Add(PT_3DEX(ptIntersect[j - 1].pt, penCode));
						for (int k = index1 + 1; k <= index2; k++)
						{
							newPts.Add(PT_3DEX(ptsRatio[k], penCode));
						}
						newPts.Add(PT_3DEX(ptIntersect[j].pt, penCode));

						arrIndex.Add(newPts.GetSize() - 1);
					}
				}

				newPts.Add(PT_3DEX(ptIntersect[ptIntersect.GetSize() - 1].pt, penCode));
				int index = (int)ptIntersect[ptIntersect.GetSize() - 1].lfRatio;
				for (int k = index + 1; k < numRatio; k++)
				{
					newPts.Add(PT_3DEX(ptsRatio[k], penCode));
				}

				arrIndex.Add(newPts.GetSize() - 1);

				// 集中处理
				for (j = 0; j < arrIndex.GetSize() - 1; j++)
				{
					CArray<PT_3DEX, PT_3DEX> arrPts;
					for (int k = arrIndex[j] + 1; k <= arrIndex[j + 1]; k++)
					{
						arrPts.Add(newPts[k]);
					}
					BOOL bInsideBank = FALSE;
					PT_3DEX testPt;
					GraphAPI::GGetMiddlePt(arrPts.GetData(), arrPts.GetSize(), &testPt);

					if (GraphAPI::GIsPtInRegion(testPt, pts2.GetData(), pts2.GetSize()) == 2)
					{
						bInsideBank = TRUE;
					}

					CFeature *pNewFtr = pNewFtr0->Clone();
					if (!pNewFtr)
					{
						Abort();
						return;
					}

					pNewFtr->SetID(OUID());

					if (pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize()))
					{
						// 隐藏
						if (bInsideBank)
						{
							//pNewFtr->EnableVisible(FALSE);
							CString str = StrFromResID(IDS_LAYERNAME_CONTOUR);
							CFtrLayer *pContourLayer = pDS->GetFtrLayer(str);
							if (!pContourLayer)
							{
								pContourLayer = pDS->CreateFtrLayer(str);
								if (pContourLayer)
								{
									pContourLayer->EnableVisible(FALSE);
									CFtrLayer *pLayer0 = pDS->GetFtrLayerOfObject(pFtrRatio);
									if (pLayer0)
									{
										pContourLayer->SetColor(pLayer0->GetColor());
									}

									pDS->AddFtrLayer(pContourLayer);
								}
							}

							if (pContourLayer)
							{
								m_pEditor->AddObject(pNewFtr, pContourLayer->GetID());
							}
						}
						else
						{
							m_pEditor->AddObject(pNewFtr, layidnew);
						}


						undo.AddNewFeature(FtrToHandle(pNewFtr));
					}
				}

				delete pNewFtr0;
			}
		}

		undo.Commit();

		GProgressEnd();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->CloseSelector();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		Finish();
	}
	return;
}

void CTrimContourAcrossBankCommand::PtMove(PT_3D &pt)
{
	CEditCommand::PtMove(pt);
}

IMPLEMENT_DYNAMIC(CTrimContourAcrossDCurveCommand, CEditCommand)
CTrimContourAcrossDCurveCommand::CTrimContourAcrossDCurveCommand()
{
	m_nStep = 0;
	m_bKeepDelPart = TRUE;
	strcat(m_strRegPath, "\\ContourAcrossDCurve");
}
CTrimContourAcrossDCurveCommand::~CTrimContourAcrossDCurveCommand()
{

}

CString CTrimContourAcrossDCurveCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CONTOURACROSSDCURVE);
}

void CTrimContourAcrossDCurveCommand::Start()
{
	if (!m_pEditor)return;
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_nStep = 0;
	m_strLayerName = StrFromResID(IDS_INDEX_CONTOUR) + "," + StrFromResID(IDS_STD_CONTOUR) + "," + StrFromResID(IDS_INTER_CONTOUR);
	CEditCommand::Start();
	PromptString(StrFromResID(IDS_CMDTIP_SELDCURVEORTWOLINES));
	m_pEditor->OpenSelector(SELMODE_MULTI);
}

void CTrimContourAcrossDCurveCommand::Abort()
{
	m_pEditor->CloseSelector();
	m_nStep = -1;
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CEditCommand::Abort();
}

void CTrimContourAcrossDCurveCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ModifyContourAcrossDCurve", StrFromLocalResID(IDS_CMDNAME_CONTOURACROSSDCURVE));
	param->AddLayerNameParamEx(PF_MODIFCONTOURACROSSDCURVE, (LPCTSTR)m_strLayerName, StrFromResID(IDS_CONTOUR_LAYERNAME), NULL, LAYERPARAMITEM_LINE);
	param->AddParam(PF_CONTOURACROSSDCURVE_KEEPPART, (bool)m_bKeepDelPart, StrFromResID(IDS_CMDPLANE_KEEPDELPART), NULL, LAYERPARAMITEM_LINE);
	//	param->AddUsedLayerNameParam(PF_MODIFYLAYLAYERNAME,(LPCTSTR)m_strLayLayerName,StrFromResID(IDS_DEL_LAYLAYERNAME));	
}

void CTrimContourAcrossDCurveCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_MODIFCONTOURACROSSDCURVE, var))
	{
		m_strLayerName = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_CONTOURACROSSDCURVE_KEEPPART, var))
	{
		m_bKeepDelPart = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab, bInit);
}

void CTrimContourAcrossDCurveCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strLayerName;
	tab.AddValue(PF_MODIFCONTOURACROSSDCURVE, &CVariantEx(var));
	// 	var = (_bstr_t)(LPCTSTR)m_strLayLayerName;
	// 	tab.AddValue(PF_MODIFYLAYLAYERNAME,&CVariantEx(var));
	var = (bool)(m_bKeepDelPart);
	tab.AddValue(PF_CONTOURACROSSDCURVE_KEEPPART, &CVariantEx(var));
}

void CTrimContourAcrossDCurveCommand::PtClick(PT_3D &pt, int flag)
{
	CString tip;
	if (m_nStep == 0)
	{
		const FTR_HANDLE *handles = NULL;
		int nsel = 0;
		if (!CanGetSelObjs(flag) || flag != SELSTAT_DRAGSEL_RESTART)
		{
			PDOC(m_pEditor)->GetSelection()->GetSelectedObjs(nsel);
			handles = PDOC(m_pEditor)->GetSelection()->GetSelectedObjs(nsel);
			if (nsel > 2)return;
			if (nsel > 0)
				GotoState(PROCSTATE_PROCESSING);
			if (nsel == 1)
			{
				CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
				if (!pObj)return;
				int nGeotype;
				nGeotype = pObj->GetClassType();
				if (nGeotype != CLS_GEODCURVE&&nGeotype != CLS_GEOPARALLEL)
				{
					PromptString(StrFromResID(IDS_CMDTIP_SELDCURVEORTWOLINES));
					return;
				}
				PromptString(StrFromResID(IDS_CMDTIP_SURE));
			}
			else if (nsel == 2)
			{
				CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
				CGeometry *pObj1 = HandleToFtr(handles[1])->GetGeometry();
				if (!pObj || !pObj1)return;
				int nGeotype, nGeotype1;
				nGeotype = pObj->GetClassType();
				nGeotype1 = pObj->GetClassType();
				if (nGeotype != CLS_GEOCURVE&&nGeotype1 != CLS_GEOCURVE)
				{
					PromptString(StrFromResID(IDS_CMDTIP_SELDCURVEORTWOLINES));
					return;
				}
				PromptString(StrFromResID(IDS_CMDTIP_SURE));
			}
			for (int i = nsel - 1; i >= 0; i--)
			{
				m_arrObjFtrs.Add(handles[i]);
			}
			m_pEditor->CloseSelector();
		}
		m_nStep = 1;
		return;
	}
	if (m_nStep == 1)
	{
		CDlgDataSource *pDS = GETDS(m_pEditor);
		int nObjSum = m_arrObjFtrs.GetSize();
		CArray<PT_3DEX, PT_3DEX> pts0, pts1;
		if (2 == nObjSum)
		{
			CFeature *pFtr = HandleToFtr(m_arrObjFtrs[0]);
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			{
				return;
			}
			const CShapeLine *pShape = pFtr->GetGeometry()->GetShape();
			pShape->GetPts(pts0);
			pFtr = HandleToFtr(m_arrObjFtrs[1]);
			pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			{
				return;
			}
			pShape = pFtr->GetGeometry()->GetShape();
			pShape->GetPts(pts1);
		}
		else if (1 == nObjSum)
		{
			CFeature *pFtr = HandleToFtr(m_arrObjFtrs[0]);
			CGeoParallel *pGeoParallel = NULL;
			CGeoDCurve *pGeoDCurve = NULL;
			CGeometry *pGeo = pFtr->GetGeometry();
			CGeometry *pGeo1 = NULL, *pGeo2 = NULL;
			if (!pGeo && pGeo->GetClassType() != CLS_GEODCURVE&&pGeo->GetClassType() != CLS_GEOPARALLEL)
			{
				return;
			}
			if (pGeo->GetClassType() == CLS_GEOPARALLEL)
			{
				pGeoParallel = (CGeoParallel*)pFtr->GetGeometry()->Linearize();
				pGeoParallel->Separate(pGeo1, pGeo2);
				if (pGeo1)
				{
					const CShapeLine *pShape = pGeo1->GetShape();
					pShape->GetPts(pts0);
					delete pGeo1;
					pGeo1 = NULL;
				}
				if (pGeo2)
				{
					const CShapeLine *pShape = pGeo2->GetShape();
					pShape->GetPts(pts1);
					delete pGeo2;
					pGeo2 = NULL;
				}
			}
			else if (pGeo->GetClassType() == CLS_GEODCURVE)
			{
				pGeoDCurve = (CGeoDCurve*)pFtr->GetGeometry()->Linearize();
				pGeoDCurve->Separate(pGeo1, pGeo2);
				if (pGeo1)
				{
					const CShapeLine *pShape = pGeo1->GetShape();
					pShape->GetPts(pts0);
					delete pGeo1;
					pGeo1 = NULL;
				}
				if (pGeo2)
				{
					const CShapeLine *pShape = pGeo2->GetShape();
					pShape->GetPts(pts1);
					delete pGeo2;
					pGeo2 = NULL;
				}
			}
		}

		CUndoFtrs undo(m_pEditor, Name());
		CArray<FtrIntersect, FtrIntersect> arrObjRatio1, arrObjRatio2;
		Envelope e1, e2;
		e1.CreateFromPts(pts0.GetData(), pts0.GetSize(), sizeof(PT_3DEX));
		e2 = e1;
		e2.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS, 1);
		m_pEditor->GetDataQuery()->FindObjectInRect(e2, m_pEditor->GetCoordWnd().m_pSearchCS);//获取穿越坡坎的等高线地物
		int num1;
		const CPFeature * ppftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
		if (!ppftr1)
		{
			Abort();
			return;
		}
		CArray<CPFeature, CPFeature> ftr1;
		ftr1.SetSize(num1);
		memcpy(ftr1.GetData(), ppftr1, num1*sizeof(*ppftr1));
		CGeometry *pObj;
		const CShapeLine *pSL;
		for (int i = 0; i < num1; i++)
		{
			if (2 == nObjSum)
			{
				if (ftr1[i] == HandleToFtr(m_arrObjFtrs[0]) || ftr1[i] == HandleToFtr(m_arrObjFtrs[1])) continue;
			}
			else if (1 == nObjSum)
			{
				if (ftr1[i] == HandleToFtr(m_arrObjFtrs[0])) continue;
			}
			pObj = ftr1[i]->GetGeometry();
			if (!pObj)continue;
			CFtrLayer *pftr1Layer = pDS->GetFtrLayerOfObject(ftr1[i]);
			if (!pftr1Layer) continue;
			//支持层码
			if (!m_strLayerName.IsEmpty() && !CheckNameForLayerCode(pDS, pftr1Layer->GetName(), m_strLayerName))
				continue;

			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)))continue;

			if (pObj->GetDataPointSum() < 2)continue;
			pSL = pObj->GetShape();
			if (!pSL)continue;
			CArray<PT_3DEX, PT_3DEX> arr;
			pSL->GetPts(arr);
			FtrIntersect item;
			item.ftr = FtrToHandle(ftr1[i]);
			if (GetCurveIntersectCurve(arr.GetData(), arr.GetSize(), pts0.GetData(), pts0.GetSize(), item.pts))
			{
				int nPts = item.pts.GetSize();
				for (int m = 0; m < nPts; m++)
				{
					item.pts[m].IntersectFlag = 1;//设置相交点为那根线段的		
				}
				arrObjRatio1.Add(item);
			}
		}
		e1.CreateFromPts(pts1.GetData(), pts1.GetSize(), sizeof(PT_3DEX));
		e2 = e1;
		e2.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS, 1);
		m_pEditor->GetDataQuery()->FindObjectInRect(e2, m_pEditor->GetCoordWnd().m_pSearchCS);
		int num2;
		const CPFeature * ppftr2 = m_pEditor->GetDataQuery()->GetFoundHandles(num2);
		if (!ppftr2)
		{
			Abort();
			return;
		}
		CArray<CPFeature, CPFeature> ftr2;
		ftr2.SetSize(num2);
		memcpy(ftr2.GetData(), ppftr2, num2*sizeof(*ppftr2));
		for (i = 0; i < num2; i++)
		{
			if (2 == nObjSum)
			{
				if (ftr2[i] == HandleToFtr(m_arrObjFtrs[0]) || ftr2[i] == HandleToFtr(m_arrObjFtrs[1])) continue;
			}
			else if (1 == nObjSum)
			{
				if (ftr2[i] == HandleToFtr(m_arrObjFtrs[0])) continue;
			}
			pObj = ftr2[i]->GetGeometry();
			if (!pObj)continue;
			CFtrLayer *pftr2Layer = pDS->GetFtrLayerOfObject(ftr2[i]);
			if (!pftr2Layer) continue;

			if (!m_strLayerName.IsEmpty() && !CheckNameForLayerCode(pDS, pftr2Layer->GetName(), m_strLayerName))
				continue;

			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)))continue;

			if (pObj->GetDataPointSum() < 2)continue;
			pSL = pObj->GetShape();
			if (!pSL)continue;
			CArray<PT_3DEX, PT_3DEX> arr;
			pSL->GetPts(arr);
			FtrIntersect item;
			item.ftr = FtrToHandle(ftr2[i]);
			if (GetCurveIntersectCurve(arr.GetData(), arr.GetSize(), pts1.GetData(), pts1.GetSize(), item.pts))
			{
				int nPts = item.pts.GetSize();
				for (int m = 0; m < nPts; m++)
				{
					item.pts[m].IntersectFlag = 2;//设置相交点为那根线段的	
				}
				arrObjRatio2.Add(item);
			}
		}
		CArray<FtrIntersect, FtrIntersect> arrObjRatio;
		int numIntersect1 = arrObjRatio1.GetSize();
		int numIntersect2 = arrObjRatio2.GetSize();
		CArray<FTR_HANDLE, FTR_HANDLE> arrProcessedFtrs;
		for (i = 0; i < numIntersect1; i++)
		{
			for (int j = 0; j < numIntersect2; j++)
			{
				if (arrObjRatio2[j].ftr == arrObjRatio1[i].ftr)
				{
					break;
				}
			}
			if (j < numIntersect2)
			{
				arrProcessedFtrs.Add(arrObjRatio2[j].ftr);
				for (int n = 0; n < arrObjRatio2[j].pts.GetSize(); n++)
				{
					PtIntersect item = arrObjRatio2[j].pts[n];
					int size = arrObjRatio1[i].pts.GetSize();
					for (int k = 0; k < size && item.lfRatio >= arrObjRatio1[i].pts[k].lfRatio; k++);
					if (k < size)arrObjRatio1[i].pts.InsertAt(k, item);
					else arrObjRatio1[i].pts.Add(item);
				}
			}
		}
		for (i = 0; i < numIntersect2; i++)
		{
			int numftrs = arrProcessedFtrs.GetSize();
			for (int j = 0; j < numftrs; j++)
			{
				if (arrProcessedFtrs[j] == arrObjRatio2[i].ftr)
				{
					break;
				}
			}
			if (j < numftrs)
			{
				continue;
			}
			arrObjRatio1.Add(arrObjRatio2[i]);
		}
		for (i = 0; i < arrObjRatio1.GetSize(); i++)
		{
			CFeature *pFtrRatio = HandleToFtr(arrObjRatio1[i].ftr);
			if (!pFtrRatio)
			{
				continue;
			}
			CFeature *pNewFtr0 = pFtrRatio->Clone();
			if (!pNewFtr0)
			{
				Abort();
				return;
			}
			int layidnew = m_pEditor->GetFtrLayerIDOfFtr(arrObjRatio1[i].ftr);
			undo.AddOldFeature(arrObjRatio1[i].ftr);
			CArray<PT_3DEX, PT_3DEX> ptsRatio;
			if (!pFtrRatio->GetGeometry()) continue;
			pFtrRatio->GetGeometry()->GetShape()->GetPts(ptsRatio);//相交等高线地物的所有点
			int numRatio = ptsRatio.GetSize();
			m_pEditor->DeleteObject(arrObjRatio1[i].ftr);
			CArray<PtIntersect, PtIntersect> &ptIntersect = arrObjRatio1[i].pts;//交点
			CArray<PT_3DEX, PT_3DEX> newPts;
			int penCode = ptsRatio[numRatio - 1].pencode;
			CUIntArray arrIndex;
			CArray<int, int> arrIntersectFlg;
			for (int j1 = 0; j1 < ptIntersect.GetSize(); j1++)
			{
				if (j1 == 0)
				{
					int index = (int)ptIntersect[0].lfRatio;//第一个相交点索引
					for (int k = 0; k <= index; k++)
					{
						newPts.Add(PT_3DEX(ptsRatio[k], penCode));
					}
					newPts.Add(PT_3DEX(ptIntersect[0].pt, penCode));
					arrIndex.Add(-1);
					arrIndex.Add(newPts.GetSize() - 1);
					arrIntersectFlg.Add(0);
				}
				else
				{
					int index1, index2;
					int nIntersectflg1, nIntersectflg2;//相交点类型
					index1 = (int)ptIntersect[j1 - 1].lfRatio;
					index2 = (int)ptIntersect[j1].lfRatio;
					nIntersectflg1 = ptIntersect[j1 - 1].IntersectFlag;
					nIntersectflg2 = ptIntersect[j1].IntersectFlag;
					newPts.Add(PT_3DEX(ptIntersect[j1 - 1].pt, penCode));
					for (int k = index1 + 1; k <= index2; k++)
					{
						newPts.Add(PT_3DEX(ptsRatio[k], penCode));
					}
					newPts.Add(PT_3DEX(ptIntersect[j1].pt, penCode));
					arrIndex.Add(newPts.GetSize() - 1);
					if (nIntersectflg1 != nIntersectflg2)//类型不同 需要隐藏
					{
						arrIntersectFlg.Add(1);
					}
					else
					{
						arrIntersectFlg.Add(0);
					}
				}
			}
			newPts.Add(PT_3DEX(ptIntersect[ptIntersect.GetSize() - 1].pt, penCode));
			int index = (int)ptIntersect[ptIntersect.GetSize() - 1].lfRatio;//最后一个相交点的索引
			for (int k = index + 1; k < numRatio; k++)
			{
				newPts.Add(PT_3DEX(ptsRatio[k], penCode));
			}
			arrIndex.Add(newPts.GetSize() - 1);
			arrIntersectFlg.Add(0);
			for (int j = 0; j < arrIndex.GetSize() - 1; j++)
			{
				CArray<PT_3DEX, PT_3DEX> arrPts;
				for (int k = arrIndex[j] + 1; k <= arrIndex[j + 1]; k++)
				{
					arrPts.Add(newPts[k]);
				}
				BOOL bInsideBank = FALSE;
				CFeature *pNewFtr = pNewFtr0->Clone();
				if (!pNewFtr)
				{
					Abort();
					return;
				}
				pNewFtr->SetID(OUID());
				if (pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize()))
				{
					CArray<PT_3DEX, PT_3DEX> ArrTmpPts, ArrTmpPts2;
					ArrTmpPts.RemoveAll();
					ArrTmpPts2.RemoveAll();
					ArrTmpPts.Copy(pts0);
					ArrTmpPts2.Copy(pts1);
					ASSERT(ArrTmpPts.GetSize() > 0 && ArrTmpPts2.GetSize() > 0);
					int nsize0 = ArrTmpPts.GetSize();
					int nsize1 = ArrTmpPts2.GetSize();
					{
						PT_3D arr1[3], arr2[3];
						arr1[0] = ArrTmpPts[0]; arr1[1] = ArrTmpPts[nsize0 / 2]; arr1[2] = ArrTmpPts[nsize0 - 1];
						arr2[0] = ArrTmpPts2[0]; arr2[1] = ArrTmpPts2[nsize1 / 2]; arr2[2] = ArrTmpPts2[nsize1 - 1];
						int bClock1 = GraphAPI::GIsClockwise(arr1, 3);
						int bClock2 = GraphAPI::GIsClockwise(arr2, 3);
						if ((bClock1 != -1 && bClock2 != -1 && bClock1 == bClock2) || GraphAPI::GGet2DDisOf2P(ArrTmpPts[nsize0 - 1], ArrTmpPts2[0]) >= GraphAPI::GGet2DDisOf2P(ArrTmpPts[nsize0 - 1], ArrTmpPts2[nsize1 - 1]))//反向
						{
							for (int m = 0; m < nsize1 / 2; m++)
							{
								PT_3DEX t = ArrTmpPts2[m];
								ArrTmpPts2[m] = ArrTmpPts2[nsize1 - m - 1];
								ArrTmpPts2[nsize1 - m - 1] = t;
							}
						}
					}
					ArrTmpPts.Append(ArrTmpPts2);
					ArrTmpPts.Add(pts0[0]);
					if (arrIntersectFlg[j] == 1)
					{
						bInsideBank = TRUE;
					}
					else
					{
						if (LinesInPloygon(arrPts.GetData(), arrPts.GetSize(), ArrTmpPts.GetData(), ArrTmpPts.GetSize()))
						{
							bInsideBank = TRUE;
						}
					}
					if (bInsideBank)
					{
						CFtrLayer *pLayer0 = pDS->GetFtrLayerOfObject(pFtrRatio);
						if (pLayer0)
						{
							m_strLayLayerName.Format("%s_%s", pLayer0->GetName(), Name());
						}
						else
						{
							m_strLayLayerName.Format("%s", Name());
						}
						CString str = m_strLayLayerName;
						if (str.IsEmpty() || !m_bKeepDelPart)
						{
							continue;
						}
						CFtrLayer *pContourLayer = pDS->GetFtrLayer(str);
						if (!pContourLayer)
						{
							pContourLayer = pDS->CreateFtrLayer(str);
							if (pContourLayer)
							{
								CValueTable tab;
								tab.DelAll();
								tab.BeginAddValueItem();
								pLayer0->WriteTo(tab);
								tab.EndAddValueItem();
								pContourLayer->ReadFrom(tab);
								pContourLayer->SetID(0);
								pContourLayer->SetName(str);
								if (pLayer0)
								{
									pContourLayer->SetColor(RGB(128, 0, 128));
								}
								pDS->AddFtrLayer(pContourLayer);
							}
						}
						if (pContourLayer)
						{
							pNewFtr->GetGeometry()->SetColor(-1);
							GETXDS(m_pEditor)->CopyXAttributes(pNewFtr, pNewFtr0);
							m_pEditor->AddObject(pNewFtr, pContourLayer->GetID());
						}
					}
					else
					{
						m_pEditor->AddObject(pNewFtr, layidnew);
					}
					undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
				}
			}
			delete pNewFtr0;
		}

		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->CloseSelector();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		Finish();
	}
}
BOOL CTrimContourAcrossDCurveCommand::LinesInPloygon(PT_3DEX *ptsLines, int ptsnum, PT_3DEX *ptsPloygon, int ploygonnum)
{
	BOOL bret = TRUE;
	int i = 0, j = 0;
	if (ptsnum <= 0 || ploygonnum <= 0)
	{
		return bret;
	}
	for (i = 0; i < ptsnum; i++)
	{
		if (GraphAPI::GIsPtInRegion(ptsLines[i], ptsPloygon, ploygonnum) < 0)
		{
			bret = FALSE;
		}
	}
	return bret;
}

//////////////////////////////////////////////////////////////////////
// CCopyDocCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCopyDocCommand, CEditCommand)

CCopyDocCommand::CCopyDocCommand()
{
	m_nStep = -1;
	m_bCopyWithPt = FALSE;
}

CCopyDocCommand::~CCopyDocCommand()
{
	m_arrCopyFtrs.RemoveAll();
}



CString CCopyDocCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_COPYDOC);
}

void CCopyDocCommand::Start()
{
	if (!m_pEditor)return;

	m_arrCopyFtrs.RemoveAll();
	m_bCopyWithPt = FALSE;

	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num > 0)
	{
		CEditCommand::Start();
		PT_3D pt;
		PtClick(pt, 0);
	}
	else
	{
		m_pEditor->OpenSelector();
		CEditCommand::Start();
	}
}

void CCopyDocCommand::Abort()
{
	CEditCommand::Abort();
	m_nExitCode = CMPEC_STARTOLD;
}

void CCopyDocCommand::Finish()
{
	CEditCommand::Finish();
	m_nExitCode = CMPEC_STARTOLD;
}

VOID CCopyDocCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		int num;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num > 0)
		{
			if (!CanGetSelObjs(flag))
			{
				return;
			}

			m_nStep = 1;

			for (int i = num - 1; i >= 0; i--)
			{
				m_arrCopyFtrs.Add(handles[i]);
			}

			if (m_bCopyWithPt)
			{
				m_pEditor->CloseSelector();
				PromptString(StrFromResID(IDS_CMDTIP_SELPT));
			}
			else
			{
				if (m_arrCopyFtrs.GetSize() > 0)
				{
					CopyData data;
					data.pDoc = (CDlgDoc*)m_pEditor;
					data.ftrs.Copy(m_arrCopyFtrs);
					data.pt = HandleToFtr(m_arrCopyFtrs[0])->GetGeometry()->GetDataPoint(0);
					AfxGetMainWnd()->SendMessage(FCCM_COPYDATA, WPARAM(1), LPARAM(&data));
					Finish();
				}
				else
				{
					Abort();
					return;
				}

			}
		}
	}
	else if (m_nStep == 1)
	{
		m_pt = pt;

		if (m_arrCopyFtrs.GetSize() > 0)
		{
			CopyData data;
			data.pDoc = (CDlgDoc*)m_pEditor;
			data.ftrs.Copy(m_arrCopyFtrs);
			data.pt = m_pt;
			AfxGetMainWnd()->SendMessage(FCCM_COPYDATA, WPARAM(1), LPARAM(&data));

			CString strMsg;
			strMsg.Format(IDS_TIP_COPYDOC_GETOBJS, m_arrCopyFtrs.GetSize());
			GOutPut(strMsg);

			Finish();
			m_nStep = 2;

		}
		else
		{
			Abort();
			return;
		}

	}
	CEditCommand::PtClick(pt, flag);
}

void CCopyDocCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (bool)(m_bCopyWithPt);
	tab.AddValue(PF_COPYDOC_WITHPT, &CVariantEx(var));

}

void CCopyDocCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("CopyDoc", StrFromLocalResID(IDS_CMDNAME_COPYDOC));

	param->BeginOptionParam(PF_COPYDOC_WITHPT, StrFromResID(IDS_CMDPLANE_COPYWITHPT));
	param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_bCopyWithPt);
	param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_bCopyWithPt);
	param->EndOptionParam();
}


void CCopyDocCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_COPYDOC_WITHPT, var))
	{
		m_bCopyWithPt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}

//////////////////////////////////////////////////////////////////////
// CPasteDocCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPasteDocCommand, CEditCommand)

CPasteDocCommand::CPasteDocCommand()
{
	m_nStep = -1;
}

CPasteDocCommand::~CPasteDocCommand()
{
}



CString CPasteDocCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_PASTEDOC);
}

void CPasteDocCommand::Start()
{
	if (!m_pEditor)return;

	m_bPasteWithOldCoord = FALSE;

	CEditCommand::Start();

	AfxGetMainWnd()->SendMessage(FCCM_COPYDATA, WPARAM(0), LPARAM(&m_pasteData));

	m_nStep = 1;
	if (m_pasteData.pDoc == NULL || m_pasteData.ftrs.GetSize() <= 0)
	{
		m_nStep = 0;
	}

	m_pEditor->CloseSelector();

}

void CPasteDocCommand::Finish()
{
	CEditCommand::Finish();
	m_nExitCode = CMPEC_STARTOLD;
}

void CPasteDocCommand::Abort()
{
	CEditCommand::Abort();
	m_nExitCode = CMPEC_STARTOLD;
}

VOID CPasteDocCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 1)
	{
		AfxGetMainWnd()->SendMessage(FCCM_COPYDATA, WPARAM(0), LPARAM(&m_pasteData));

		if (m_pasteData.pDoc == NULL || m_pasteData.ftrs.GetSize() <= 0)
		{
			Abort();
			return;
		}

		CDataSourceEx *pDS = m_pasteData.pDoc->GetDlgDataSource();
		if(pDS && UVSModify==GetAccessType(pDS) && FALSE==CUVSModify::CanExport())
		{
			CDataSourceEx *pDS1 = PDOC(m_pEditor)->GetDlgDataSource();
			if(pDS1 && UVSModify!=GetAccessType(pDS1))
			{
				Abort();
				GOutPut(StrFromResID(IDS_NO_PERMISSION));
				return;
			}
		}
		double matrix[16];
		Matrix44FromMove(pt.x - m_pasteData.pt.x, pt.y - m_pasteData.pt.y, pt.z - m_pasteData.pt.z, matrix);

		CArray<FTR_HANDLE, FTR_HANDLE> arrObjs;
		int num = m_pasteData.ftrs.GetSize();

		CUndoFtrs undo(m_pEditor, Name());

		for (int i = 0; i < num; i++)
		{
			FTR_HANDLE ftr = m_pasteData.ftrs[i];
			CFeature *pFtr = HandleToFtr(ftr)->Clone();
			if (!pFtr) continue;

			pFtr->SetID(OUID());

			int oldlayid = m_pasteData.pDoc->GetFtrLayerIDOfFtr(ftr);
			CString strLayerName = GETDS(m_pasteData.pDoc)->GetFtrLayer(oldlayid)->GetName();
			int layid = -1;
			CFtrLayer *pFtrLayer = GETDS(m_pEditor)->GetFtrLayer(strLayerName);

			if (pFtrLayer == NULL)
			{
				long col = GETDS(m_pasteData.pDoc)->GetFtrLayer(oldlayid)->GetColor();
				pFtrLayer = GETDS(m_pEditor)->CreateFtrLayer(strLayerName);
				if (!pFtrLayer)  continue;
				pFtrLayer->SetColor(col);
				GETDS(m_pEditor)->AddFtrLayer(pFtrLayer);
			}

			layid = pFtrLayer->GetID();

			CGeometry *pGeo = pFtr->GetGeometry();
			pGeo->Transform(matrix);

			m_pEditor->AddObject(pFtr, layid);

			CValueTable tab;
			tab.BeginAddValueItem();
			GETXDS(m_pasteData.pDoc)->GetXAttributes(HandleToFtr(ftr), tab);
			tab.EndAddValueItem();

			GETXDS(m_pEditor)->SetXAttributes(pFtr, tab);

			undo.AddNewFeature(FtrToHandle(pFtr));

		}

		undo.Commit();

		m_pEditor->OnSelectChanged(TRUE);
		m_pEditor->CloseSelector();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		Finish();
		m_nStep = 2;
	}

	CEditCommand::PtClick(pt, flag);
}

void CPasteDocCommand::PtMove(PT_3D &pt)
{
	if (m_nStep == 1)
	{
		AfxGetMainWnd()->SendMessage(FCCM_COPYDATA, WPARAM(0), LPARAM(&m_pasteData));

		if (m_pasteData.pDoc == NULL || m_pasteData.ftrs.GetSize() <= 0)
		{
			Abort();
			return;
		}

		GrBuffer buf;

		double matrix[16];
		Matrix44FromMove(pt.x - m_pasteData.pt.x, pt.y - m_pasteData.pt.y, pt.z - m_pasteData.pt.z, matrix);

		CDlgDataSource *pDS = m_pasteData.pDoc->GetDlgDataSource();

		int num = m_pasteData.ftrs.GetSize();
		for (int i = 0; i < num && num < 100; i++)
		{
			CFeature *pFtr = HandleToFtr(m_pasteData.ftrs.GetAt(i))->Clone();

			CGeometry *pGeo = pFtr->GetGeometry();
			pGeo->Transform(matrix);
			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
			{
				CPlotTextEx plot;
				TEXT_SETTINGS0  settings;
				((CGeoText*)pGeo)->GetSettings(&settings);
				settings.fHeight *= PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale();

				plot.SetSettings(&settings);

				plot.SetShape(((CGeoText*)pGeo)->GetShape());
				plot.SetText(((CGeoText*)pGeo)->GetText());
				plot.GetOutLineBuf(&buf);
			}
			else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
			{
				CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(m_pasteData.ftrs.GetAt(i)));
				if (pLayer)
				{
					pDS->DrawFeature(pFtr, &buf, TRUE, 0, pLayer->GetName());
				}
			}
			else
				pFtr->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());

			delete pFtr;

		}

		//buf.BeginLineString(0,0);
		//buf.MoveTo(&m_pasteData.pt);
		//buf.LineTo(&pt);

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
	}
}


//////////////////////////////////////////////////////////////////////
// CPasteDocCoordCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPasteDocCoordCommand, CEditCommand)

CPasteDocCoordCommand::CPasteDocCoordCommand()
{
	m_nStep = -1;
}

CPasteDocCoordCommand::~CPasteDocCoordCommand()
{
}



CString CPasteDocCoordCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_PASTEDOCCOORD);
}

void CPasteDocCoordCommand::Start()
{
	if (!m_pEditor)return;

	CEditCommand::Start();

	AfxGetMainWnd()->SendMessage(FCCM_COPYDATA, WPARAM(0), LPARAM(&m_pasteData));

	if (m_pasteData.pDoc == NULL || m_pasteData.ftrs.GetSize() <= 0)
	{
		Abort();
		return;
	}

	CDataSourceEx *pDS = m_pasteData.pDoc->GetDlgDataSource();
	if(pDS && UVSModify==GetAccessType(pDS) && FALSE==CUVSModify::CanExport())
	{
		CDataSourceEx *pDS1 = PDOC(m_pEditor)->GetDlgDataSource();
		if(pDS1 && UVSModify!=GetAccessType(pDS1))
		{
			Abort();
			GOutPut(StrFromResID(IDS_NO_PERMISSION));
			return;
		}
	}
	CArray<FTR_HANDLE, FTR_HANDLE> arrObjs;
	int num = m_pasteData.ftrs.GetSize();

	CUndoFtrs undo(m_pEditor, Name());

	for (int i = 0; i < num; i++)
	{
		FTR_HANDLE ftr = m_pasteData.ftrs[i];
		CFeature *pFtr = HandleToFtr(ftr)->Clone();
		if (!pFtr) continue;

		pFtr->SetID(OUID());

		int oldlayid = m_pasteData.pDoc->GetFtrLayerIDOfFtr(ftr);
		CString strLayerName = GETDS(m_pasteData.pDoc)->GetFtrLayer(oldlayid)->GetName();
		CFtrLayer *pFtrLayer = GETDS(m_pEditor)->GetFtrLayer(strLayerName);
		if (!pFtrLayer)
		{
			pFtrLayer = new CFtrLayer;
			pFtrLayer->SetName(strLayerName);
			m_pEditor->AddFtrLayer(pFtrLayer);
		}

		m_pEditor->AddObject(pFtr, pFtrLayer->GetID());

		CValueTable tab;
		tab.BeginAddValueItem();
		GETXDS(m_pasteData.pDoc)->GetXAttributes(HandleToFtr(ftr), tab);
		tab.EndAddValueItem();

		GETXDS(m_pEditor)->SetXAttributes(pFtr, tab);

		undo.AddNewFeature(FtrToHandle(pFtr));

	}

	undo.Commit();

	m_pEditor->OnSelectChanged(TRUE);
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	Finish();
	m_nStep = 2;
}

void CPasteDocCoordCommand::Finish()
{
	CEditCommand::Finish();
	m_nExitCode = CMPEC_NULL;
}

void CPasteDocCoordCommand::Abort()
{
	CEditCommand::Abort();
	m_nExitCode = CMPEC_NULL;
}


//////////////////////////////////////////////////////////////////////
// CRotateCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CRotateCommand, CEditCommand)

CRotateCommand::CRotateCommand()
{
	m_nStep = -1;
	m_bKeepOld = FALSE;
	m_bMouseDefineAngle = TRUE;
	m_fAngle = 45;
	m_nReferType = 0;
	m_fReferAngle = 0.0;
}

CRotateCommand::~CRotateCommand()
{
	CGeometry *pObj;
	for (int i = m_ptrObjs.GetSize() - 1; i >= 0; i--)
	{
		pObj = (CGeometry*)m_ptrObjs[i];
		delete pObj;
	}
	m_ptrObjs.RemoveAll();
}



CString CRotateCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_ROTATE);
}

void CRotateCommand::Start()
{
	if (!m_pEditor)return;

	m_ptrObjs.RemoveAll();
	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num > 0)
	{
		CEditCommand::Start();
		PT_3D pt = m_pEditor->GetCoordWnd().m_ptGrd;
		PtClick(pt, SELSTAT_MULTISEL);
	}
	else
	{
		m_pEditor->OpenSelector();
		CEditCommand::Start();
	}
}

void CRotateCommand::Abort()
{
	CGeometry *pObj;
	for (int i = m_ptrObjs.GetSize() - 1; i >= 0; i--)
	{
		pObj = (CGeometry*)m_ptrObjs[i];
		delete pObj;
	}
	m_ptrObjs.RemoveAll();

	CEditCommand::Abort();
}

void CRotateCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);

	_variant_t var;
	var = (bool)(m_bKeepOld);
	tab.AddValue(PF_KEEPOLD, &CVariantEx(var));
	var = (bool)(m_bMouseDefineAngle);
	tab.AddValue(PF_MOUSEDEFINE, &CVariantEx(var));
	var = (double)m_fAngle;
	tab.AddValue(PF_ANGLE, &CVariantEx(var));
	var = (long)m_nReferType;
	tab.AddValue(PF_ROTATE_REFERTYPE, &CVariantEx(var));
	var = (double)m_fReferAngle;
	tab.AddValue(PF_ROTATE_REFERANGLE, &CVariantEx(var));

}


void CRotateCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("RotateCommand", StrFromLocalResID(IDS_CMDNAME_ROTATE));

	param->BeginOptionParam(PF_KEEPOLD, StrFromResID(IDS_CMDPLANE_KEEPOLD));
	param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_bKeepOld);
	param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_bKeepOld);
	param->EndOptionParam();

	param->BeginOptionParam(PF_MOUSEDEFINE, StrFromResID(IDS_CMDTIP_MOUSEDEF_ROTATE));
	param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_bMouseDefineAngle);
	param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_bMouseDefineAngle);
	param->EndOptionParam();

	if (!m_bMouseDefineAngle || bForLoad)
	{
		param->AddParam(PF_ANGLE, m_fAngle, StrFromResID(IDS_CMDPLANE_ROTATEANGLE));
	}

	if (m_bMouseDefineAngle || bForLoad)
	{
		param->BeginOptionParam(PF_ROTATE_REFERTYPE, StrFromResID(IDS_CMDPLANE_REFERTYPE));
		param->AddOption(StrFromResID(IDS_CMDPLANE_REFERTYPEDIRECT), 0, ' ', m_nReferType == 0);
		param->AddOption(StrFromResID(IDS_CMDPLANE_REFERTYPELINE), 1, ' ', m_nReferType == 1);
		param->EndOptionParam();


		if (m_nReferType == 0 || bForLoad)
		{
			param->AddParam(PF_ROTATE_REFERANGLE, m_fReferAngle, StrFromResID(IDS_CMDPLANE_REFERANGLE));
		}
	}


}



void CRotateCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, PF_KEEPOLD, var))
	{
		m_bKeepOld = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, PF_MOUSEDEFINE, var))
	{
		m_bMouseDefineAngle = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_ANGLE, var))
	{
		m_fAngle = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, PF_ROTATE_REFERTYPE, var))
	{
		m_nReferType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if (!bInit)
		{
			if (m_nStep > 1)
			{
				m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);

				if (m_nReferType == 0)
				{
					if (m_nStep == 2 || m_nStep == 3)
					{
						m_nStep = 4;
						PromptString(StrFromResID(IDS_CMDTIP_ROTATEDIR));
					}
				}
				else if (m_nReferType == 1)
				{
					m_nStep = 2;
					PromptString(StrFromResID(IDS_CMDTIP_ROTATEREFERDIR));
				}
			}
		}
	}

	if (tab.GetValue(0, PF_ROTATE_REFERANGLE, var))
	{
		m_fReferAngle = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab);
}

VOID CRotateCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		int num;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num > 0)
		{
			if (!CanGetSelObjs(flag))
			{
				PromptString(StrFromResID(IDS_CMDTIP_ROTATEORIGIN));
				return;
			}

			m_nStep = 1;
			m_pEditor->CloseSelector();
			GotoState(PROCSTATE_PROCESSING);

			m_ptDragStart = pt;
			m_ptDragEnd = pt;

			PromptString(StrFromResID(IDS_CMDTIP_ROTATEPT));

			CGeometry *pObj;
			for (int i = num - 1; i >= 0; i--)
			{
				pObj = HandleToFtr(handles[i])->GetGeometry();
				if (!pObj)continue;
				m_ptrObjs.Add(pObj->Clone());
			}

			//			if( (flag&GK_DRAGSEL_RESTART)!=0 || flag==GK_NONESEL )m_nStep = 2;
		}
	}
	else if (m_nStep == 1)
	{
		m_ptDragStart = pt;
		m_ptDragEnd = pt;

		if (m_bMouseDefineAngle)
		{
			if (m_nReferType == 0)
			{
				m_nStep = 4;
				PromptString(StrFromResID(IDS_CMDTIP_ROTATEDIR));
			}
			else if (m_nReferType == 1)
			{
				m_nStep = 2;
				PromptString(StrFromResID(IDS_CMDTIP_ROTATEREFERDIR));
			}

		}
		else
		{
			m_nStep = 5;
			PromptString(StrFromResID(IDS_CMDTIP_ROTATEDIR));
		}

	}
	else if (m_nStep == 2)
	{
		m_ptReferStart = m_ptReferEnd = pt;
		m_nStep = 3;
	}
	else if (m_nStep == 3)
	{
		m_ptReferEnd = pt;
		m_nStep = 4;
		m_fReferAngle0 = GraphAPI::GGetAngle(m_ptReferStart.x, m_ptReferStart.y, m_ptReferEnd.x, m_ptReferEnd.y);
		PromptString(StrFromResID(IDS_CMDTIP_ROTATEDIR));
	}

	if (m_nStep == 5)
	{
		CGeometry *pObj;
		m_ptDragEnd = pt;
		for (int i = m_ptrObjs.GetSize() - 1; i >= 0; i--)
		{
			pObj = (CGeometry*)m_ptrObjs[i];
			delete pObj;
		}
		m_ptrObjs.RemoveAll();
		m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
		m_pEditor->UpdateDrag(ud_SetConstDrag, NULL);
		int num;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);

		PT_3D ptdir;
		ptdir.x = ptdir.y = 0;
		ptdir.z = 1000.0;

		double fRotate = 0;
		if (m_bMouseDefineAngle)
		{
			double refAng = m_fReferAngle*PI / 180;
			if (m_nReferType == 1)
			{
				refAng = m_fReferAngle0;
			}

			fRotate = GraphAPI::GGetAngle(m_ptDragStart.x, m_ptDragStart.y, pt.x, pt.y) - refAng;
		}
		else
		{
			fRotate = m_fAngle / 180 * PI;
		}

		if (num == 1 && ((CGeometry*)HandleToFtr(handles[0])->GetGeometry())->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		{
			CFeature *pFtr = HandleToFtr(handles[0]);
			pFtr = pFtr->Clone();
			pFtr->SetID(OUID());
			CGeoPoint *pGeo = (CGeoPoint*)pFtr->GetGeometry();

			pGeo->SetDirection(fRotate * 180 / PI + pGeo->GetDirection());
			//((CGeoPoint*)pGeo)->SetCtrlPoint2(0,pt,FALSE);

			CUndoFtrs undo(m_pEditor, Name());

			m_pEditor->AddObject(pFtr, m_pEditor->GetFtrLayerIDOfFtr(handles[0]));
			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[0]), pFtr);

			if (!m_bKeepOld)
			{
				m_pEditor->DeleteObject(handles[0]);
				undo.arrOldHandles.Add(handles[0]);
			}

			undo.arrNewHandles.Add(FTR_HANDLE(pFtr));

			undo.Commit();
		}
		else if (num == 1 && ((CGeometry*)HandleToFtr(handles[0])->GetGeometry())->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CFeature *pFtr = HandleToFtr(handles[0]);
			pFtr = pFtr->Clone();
			pFtr->SetID(OUID());
			CGeometry *pGeo = pFtr->GetGeometry();

			TEXT_SETTINGS0 testSetting;
			((CGeoText*)pGeo)->GetSettings(&testSetting);
			testSetting.fTextAngle = fRotate * 180 / PI + testSetting.fTextAngle;
			((CGeoText*)pGeo)->SetSettings(&testSetting);

			CUndoFtrs undo(m_pEditor, Name());

			m_pEditor->AddObject(pFtr, m_pEditor->GetFtrLayerIDOfFtr(handles[0]));
			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[0]), pFtr);

			if (!m_bKeepOld)
			{
				m_pEditor->DeleteObject(handles[0]);
				undo.arrOldHandles.Add(handles[0]);
			}

			undo.arrNewHandles.Add(FTR_HANDLE(pFtr));

			undo.Commit();
		}
		else
		{
			CArray<FTR_HANDLE, FTR_HANDLE> arrObjs;

			double matrix[16];
			Matrix44FromRotate(&m_ptDragStart, &ptdir,
				fRotate, matrix);

			GProgressStart(num);

			for (i = num - 1; i >= 0; i--)
			{
				GProgressStep();

				CFeature *pFtr = HandleToFtr(handles[i]);

				if (m_bKeepOld)
				{
					pFtr = pFtr->Clone();
					pFtr->SetID(OUID());
					CGeometry *pGeo = pFtr->GetGeometry();
					pGeo->Transform(matrix);
					m_pEditor->AddObject(pFtr, m_pEditor->GetFtrLayerIDOfFtr(handles[i]));
					GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]), pFtr);
					arrObjs.Add(FTR_HANDLE(pFtr));
				}
				else
				{
					CGeometry *pGeo = pFtr->GetGeometry();
					m_pEditor->DeleteObject(handles[i], FALSE);
					pGeo->Transform(matrix);
					m_pEditor->RestoreObject(handles[i]);
					arrObjs.Add(handles[i]);
				}
			}

			GProgressEnd();

			if (m_bKeepOld)
			{
				CUndoFtrs undo(m_pEditor, Name());
				undo.arrNewHandles.Copy(arrObjs);
				undo.Commit();
			}
			else
			{
				CUndoTransform undo(m_pEditor, Name());
				memcpy(undo.matrix, matrix, sizeof(matrix));
				undo.arrHandles.Copy(arrObjs);
				undo.Commit();
			}
		}


		m_pEditor->OnSelectChanged(TRUE);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 4;
	}
	CEditCommand::PtClick(pt, flag);
}

void CRotateCommand::PtMove(PT_3D &pt)
{
	if (m_bMouseDefineAngle && m_nReferType == 1 && m_nStep == 3)
	{
		GrBuffer buf;
		buf.BeginLineString(0, 0, 0);
		buf.MoveTo(&m_ptReferStart);
		buf.LineTo(&pt);
		buf.End();

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
	}
	else if (m_nStep == 4 || m_nStep == 5)
	{
		// 不在立体上显示点状地物符号
		UpdateViewType updateview = uv_AllView;

		double refAng = 0;
		if (m_bMouseDefineAngle)
		{
			refAng = m_fReferAngle*PI / 180;
			if (m_nReferType == 1)
			{
				refAng = m_fReferAngle0;
			}
		}

		CGeometry *pObj, *pTmp;
		GrBuffer buf;
		PT_3D ptdir;
		ptdir.x = ptdir.y = 0;
		ptdir.z = 1000.0;

		int num = m_ptrObjs.GetSize();

		if (num == 1 && ((CGeometry*)m_ptrObjs[0])->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		{
			updateview = uv_VectorView;

			int numftrs;
			const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(numftrs);

			CFeature *pFtr = ((CFeature*)handles[0])->Clone();
			CGeoPoint *pTmpPoint = (CGeoPoint*)pFtr->GetGeometry();

			pTmpPoint->SetDirection((GraphAPI::GGetAngle(m_ptDragStart.x, m_ptDragStart.y, pt.x, pt.y) - refAng) * 180 / PI + pTmpPoint->GetDirection());
			//pTmpPoint->SetCtrlPoint2(0,pt,FALSE);

			CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject((CFeature*)handles[0]);

			if (pLayer)
			{
				pDS->DrawFeature(pFtr, &buf, TRUE, 0, pLayer->GetName());
			}

			delete pFtr;
		}
		else if (num == 1 && ((CGeometry*)m_ptrObjs[0])->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			updateview = uv_VectorView;

			int numftrs;
			const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(numftrs);

			CFeature *pFtr = ((CFeature*)handles[0])->Clone();
			CGeoText *pTmpText = (CGeoText*)pFtr->GetGeometry();

			TEXT_SETTINGS0 testSetting;
			pTmpText->GetSettings(&testSetting);
			testSetting.fTextAngle = (GraphAPI::GGetAngle(m_ptDragStart.x, m_ptDragStart.y, pt.x, pt.y) - refAng) * 180 / PI + testSetting.fTextAngle;
			pTmpText->SetSettings(&testSetting);

			CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject((CFeature*)handles[0]);

			if (pLayer)
			{
				pDS->DrawFeature(pFtr, &buf, TRUE, 0, pLayer->GetName());
			}

			delete pFtr;
		}
		else
		{
			for (int i = 0; i < num; i++)
			{
				pObj = (CGeometry*)m_ptrObjs[i];
				pTmp = pObj->Clone();
				double matrix[16];
				Matrix44FromRotate(&m_ptDragStart, &ptdir,
					GraphAPI::GGetAngle(m_ptDragStart.x, m_ptDragStart.y, pt.x, pt.y) - refAng, matrix);
				pTmp->Transform(matrix);
				pTmp->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
				delete pTmp;
			}
		}

		buf.BeginLineString(0, 0, 0);
		buf.MoveTo(&m_ptDragStart);
		buf.LineTo(&m_ptDragEnd);
		buf.End();

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf, updateview);

		if (updateview == uv_VectorView)
		{
			GrBuffer buf0;
			buf0.BeginLineString(0, 0, 0);
			buf0.MoveTo(&m_ptDragStart);
			buf0.LineTo(&m_ptDragEnd);
			m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf0, uv_StereoView);
		}

		m_ptDragEnd = pt;
		m_nStep = 5;
	}
	CEditCommand::PtMove(pt);
}

//////////////////////////////////////////////////////////////////////
// CModifyArrayCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CModifyArrayCommand, CEditCommand)

CModifyArrayCommand::CModifyArrayCommand()
{
	m_nStep = -1;
	m_nType = 0;
	m_nRowNum = m_nColNum = 4;
	m_fRowOff = m_fColOff = 1;
	m_fArrayAngle = 0;
	m_nCircleMode = 0;
	m_nItemNum = 4;
	m_fFillAngle = 360;
	m_fItemAngle = 90;
	m_bRotateItem = TRUE;
	m_bDefaultBasePt = TRUE;
}

CModifyArrayCommand::~CModifyArrayCommand()
{

}



CString CModifyArrayCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_ARRAY);
}

void CModifyArrayCommand::Start()
{
	if (!m_pEditor)return;

	m_nType = 0;
	m_nRowNum = m_nColNum = 4;
	m_fRowOff = m_fColOff = 1;
	m_fArrayAngle = 0;
	m_nCircleMode = 0;
	m_nItemNum = 4;
	m_fFillAngle = 360;
	m_fItemAngle = 90;
	m_bRotateItem = TRUE;
	m_bDefaultBasePt = TRUE;

	CEditCommand::Start();

	m_pEditor->DeselectAll();

	GOutPut(StrFromResID(IDS_CMDTIP_SELBASEOJB));

	m_pEditor->OpenSelector();
}

void CModifyArrayCommand::Abort()
{
	CEditCommand::Abort();
}

VOID CModifyArrayCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;

		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		if (num == 0 || !handles)
		{
			Abort();
			return;
		}

		m_arrBaseObjs.SetSize(num);
		memcpy(m_arrBaseObjs.GetData(), handles, sizeof(FTR_HANDLE)*num);

		GotoState(PROCSTATE_PROCESSING);

		if (m_nType == 0)
		{
			GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
			m_nStep = 3;
			m_pEditor->CloseSelector();
		}
		else if (m_nType == 1)
		{
			if (!m_bDefaultBasePt)
			{
				GOutPut(StrFromResID(IDS_CMDTIP_SELBASEOJBBASEPT));

				m_nStep = 1;
			}
			else
			{
				if (m_arrBaseObjs.GetSize() > 0)
				{
					CFeature *pFtr = HandleToFtr(m_arrBaseObjs[0]);
					if (!pFtr)
					{
						Abort();
						return;
					}

					CGeometry *pGeo = pFtr->GetGeometry();
					CArray<PT_3DEX, PT_3DEX> pts;
					pGeo->GetShape(pts);
					int size = pts.GetSize();

					if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					{
						// 圆弧、圆：圆心
						if (size > 1 && (pts[1].pencode == pen3PArc || pts[1].pencode == penArc))
						{
							pGeo->GetCenter(NULL, &m_BaseptOfItem);
						}
						// 第一个点
						else
						{
							m_BaseptOfItem = pts[0];
						}

						// 						PT_3DEX ret;
						// 						GraphAPI::GGetMiddlePt(pts.GetData(),pts.GetSize(),&ret);
						// 
						// 						m_BaseptOfItem = ret;
					}
					else
					{
						m_BaseptOfItem = pts[0];
					}
				}
				GOutPut(StrFromResID(IDS_CMDTIP_SELCIRCLEARRAYCENTERPT));
				m_nStep = 2;
			}

			m_pEditor->CloseSelector();
		}
	}
	else if (m_nStep == 1)
	{
		m_BaseptOfItem = pt;
		GOutPut(StrFromResID(IDS_CMDTIP_SELCIRCLEARRAYCENTERPT));
		m_nStep = 2;
	}
	else if (m_nStep == 2)
	{
		m_ptCenter = pt;
		GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
		m_nStep = 3;
		m_pEditor->CloseSelector();
	}
	else if (m_nStep == 3)
	{
		CUndoFtrs undo(m_pEditor, Name());

		// 矩形阵列
		if (m_nType == 0)
		{
			double ang = m_fArrayAngle / 180 * PI;

			double xoff0 = 0, yoff0 = 0;
			for (int i = 0; i < m_nRowNum; i++)
			{
				double xoff1 = -xoff0, yoff1 = yoff0;
				for (int j = 0; j < m_nColNum; j++)
				{
					if (i == 0 && j == 0)
					{
						continue;
					}

					if (j != 0)
					{
						xoff1 += m_fColOff*cos(ang);
						yoff1 += m_fColOff*sin(ang);
					}

					for (int k = 0; k < m_arrBaseObjs.GetSize(); k++)
					{
						CFeature *pFtr = HandleToFtr(m_arrBaseObjs[k]);
						if (pFtr)
						{
							CFeature *pNewFtr = pFtr->Clone();
							if (!pNewFtr) continue;

							pNewFtr->SetID(OUID());

							double m[16];
							Matrix44FromMove(xoff1, yoff1, 0, m);
							pNewFtr->GetGeometry()->Transform(m);

							m_pEditor->AddObject(pNewFtr, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr)));

							undo.AddNewFeature(FtrToHandle(pNewFtr));
						}
					}

				}

				xoff0 += m_fRowOff*sin(ang);
				yoff0 += m_fRowOff*cos(ang);
			}

		}
		// 环形阵列
		else if (m_nType == 1)
		{
			if (m_nItemNum == 0 || fabs(m_fItemAngle) < 1e-4 || fabs(m_fFillAngle) < 1e-4)
			{
				GOutPut(StrFromResID(IDS_CMDTIP_ARRAYCIRCLEERROR));
				return;
			}

			int nItem = -1;
			double fItemAngle = -1;
			if (m_nCircleMode == 0)
			{
				nItem = abs(m_nItemNum);
				double fFillAngle = m_fFillAngle - ((int)(m_fFillAngle / 360)) * 360;
				if (fabs(fFillAngle) < 1e-4)
				{
					fFillAngle = 360;
				}

				fItemAngle = fFillAngle / nItem;
			}
			else if (m_nCircleMode == 1)
			{
				nItem = abs(m_nItemNum);
				if (m_nItemNum*m_fItemAngle > 360)
				{
					fItemAngle = 360 / nItem;
				}
				else
				{
					fItemAngle = fabs(m_fItemAngle);
				}


			}
			else if (m_nCircleMode == 2)
			{
				fItemAngle = fabs(m_fItemAngle);
				double fFillAngle = m_fFillAngle - ((int)(m_fFillAngle / 360)) * 360;
				if (fabs(fFillAngle) < 1e-4)
				{
					fFillAngle = 360;
				}
				nItem = fFillAngle / fItemAngle;
			}

			double r = GraphAPI::GGet2DDisOf2P(m_ptCenter, m_BaseptOfItem);
			double fStartAng = GraphAPI::GGetAngle(m_ptCenter.x, m_ptCenter.y, m_BaseptOfItem.x, m_BaseptOfItem.y);

			for (int i = 1; i < nItem; i++)
			{
				double ang0 = i*fItemAngle / 180 * PI;
				double ang = ang0 + fStartAng;

				double fFillAngle = ang - ((int)(ang / (2 * PI)))*(2 * PI);

				double xoff, yoff;
				xoff = m_ptCenter.x + r * cos(ang) - m_BaseptOfItem.x;
				yoff = m_ptCenter.y + r * sin(ang) - m_BaseptOfItem.y;

				for (int k = 0; k < m_arrBaseObjs.GetSize(); k++)
				{
					CFeature *pFtr = HandleToFtr(m_arrBaseObjs[k]);
					if (pFtr)
					{
						CFeature *pNewFtr = pFtr->Clone();
						if (!pNewFtr) continue;

						pNewFtr->SetID(OUID());

						double m[16], m1[16], m2[16];

						if (m_bRotateItem)
						{
							Matrix44FromRotate(&m_BaseptOfItem, ang0, m1);
						}
						else
						{
							matrix_toIdentity(m1, 4);
						}

						Matrix44FromMove(xoff, yoff, 0, m2);

						matrix_multiply(m2, m1, 4, m);

						pNewFtr->GetGeometry()->Transform(m);

						m_pEditor->AddObject(pNewFtr, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr)));

						undo.AddNewFeature(FtrToHandle(pNewFtr));
					}
				}
			}
		}

		undo.Commit();

		GOutPut(StrFromResID(IDS_CMDTIP_ARRAYEND));

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL, hc_Refresh, 0);
		Finish();
	}

	CEditCommand::PtClick(pt, flag);
}

void CModifyArrayCommand::PtMove(PT_3D &pt)
{
	/*if( m_nStep==2 || m_nStep==3 )
	{
	CGeometry *pObj, *pTmp;
	GrBuffer buf;
	PT_3D ptdir;
	ptdir.x = ptdir.y = 0;
	ptdir.z = 1000.0;

	int num = m_ptrObjs.GetSize();
	for( int i=0; i<num; i++)
	{
	pObj = (CGeometry*)m_ptrObjs[i];
	pTmp = pObj->Clone();
	double matrix[16];
	Matrix44FromRotate(&m_ptDragStart,&ptdir,
	GraphAPI::GGetAngle(m_ptDragStart.x,m_ptDragStart.y,pt.x,pt.y),matrix);
	pTmp->Transform(matrix);
	pTmp->Draw(&buf);
	delete pTmp;
	}
	buf.BeginLineString(0,0,0);
	buf.MoveTo(&m_ptDragStart);
	buf.LineTo(&m_ptDragEnd);
	buf.End();

	m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
	m_ptDragEnd = pt;
	m_nStep = 3;
	}*/
	CEditCommand::PtMove(pt);
}

void CModifyArrayCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("Array"), StrFromLocalResID(IDS_CMDNAME_ARRAY));
	param->BeginOptionParam(PF_ARRAYTYPE, StrFromResID(IDS_CMDPLANE_ARRAYTYPE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_RECTARRAY), 0, ' ', m_nType == 0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_CIRCLEARRAY), 1, ' ', m_nType == 1);
	param->EndOptionParam();

	if (m_nType == 0 || bForLoad)
	{
		param->AddParam(PF_ARRAYROWNUM, m_nRowNum, StrFromResID(IDS_CMDPLANE_ARRAYROWNUM));
		param->AddParam(PF_ARRAYCOLNUM, m_nColNum, StrFromResID(IDS_CMDPLANE_ARRAYCOLNUM));
		param->AddParam(PF_ARRAYROWOFF, m_fRowOff, StrFromResID(IDS_CMDPLANE_ARRAYROWOFF));
		param->AddParam(PF_ARRAYCOLOFF, m_fColOff, StrFromResID(IDS_CMDPLANE_ARRAYCOLOFF));
		param->AddParam(PF_ARRAYANGLE, m_fArrayAngle, StrFromResID(IDS_CMDPLANE_ARRAYANGLE));
	}

	if (m_nType == 1 || bForLoad)
	{
		param->BeginOptionParam(PF_ARRAYCIRCLEMODE, StrFromResID(IDS_CMDPLANE_ARRAYCIRCLEMODE));
		param->AddOption(StrFromResID(IDS_CMDPLANE_ARRAYITEMSUMANDFILLANGLE), 0, ' ', m_nCircleMode == 0);
		param->AddOption(StrFromResID(IDS_CMDPLANE_ARRAYITEMSUMANDITEMANGLE), 1, ' ', m_nCircleMode == 1);
		param->AddOption(StrFromResID(IDS_CMDPLANE_ARRAYFILLANGLEANDITEMANGLE), 2, ' ', m_nCircleMode == 2);
		param->EndOptionParam();

		if (!bForLoad)
		{
			if (m_nCircleMode == 0)
			{
				param->AddParam(PF_ARRAYITEMNUM, m_nItemNum, StrFromResID(IDS_CMDPLANE_ARRAYITEMSUM));
				param->AddParam(PF_ARRAYFILLANGLE, m_fFillAngle, StrFromResID(IDS_CMDPLANE_ARRAYFILLANGLE));
			}
			else if (m_nCircleMode == 1)
			{
				param->AddParam(PF_ARRAYITEMNUM, m_nItemNum, StrFromResID(IDS_CMDPLANE_ARRAYITEMSUM));
				param->AddParam(PF_ARRAYFILLANGLE, m_fItemAngle, StrFromResID(IDS_CMDPLANE_ARRAYITEMANGLE));
			}
			else if (m_nCircleMode == 2)
			{
				param->AddParam(PF_ARRAYFILLANGLE, m_fFillAngle, StrFromResID(IDS_CMDPLANE_ARRAYFILLANGLE));
				param->AddParam(PF_ARRAYITEMANGLE, m_fItemAngle, StrFromResID(IDS_CMDPLANE_ARRAYITEMANGLE));
			}
		}
		else
		{
			param->AddParam(PF_ARRAYITEMNUM, m_nItemNum, StrFromResID(IDS_CMDPLANE_ARRAYITEMSUM));
			param->AddParam(PF_ARRAYFILLANGLE, m_fFillAngle, StrFromResID(IDS_CMDPLANE_ARRAYFILLANGLE));
			param->AddParam(PF_ARRAYITEMANGLE, m_fItemAngle, StrFromResID(IDS_CMDPLANE_ARRAYITEMANGLE));
		}

		param->AddParam(PF_ARRAYROTATEITEM, (bool)m_bRotateItem, StrFromResID(IDS_CMDPLANE_ARRAYROTATEITEM));

		param->AddParam(PF_ARRAYDEFAULTBASEPT, (bool)m_bDefaultBasePt, StrFromResID(IDS_CMDPLANE_ARRAYDEFAULTBASEPT));
	}
}

void CModifyArrayCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, PF_ARRAYTYPE, var))
	{
		m_nType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if (!bInit)
		{
			Abort();
			return;
		}
	}

	if (m_nType == 0)
	{
		if (tab.GetValue(0, PF_ARRAYROWNUM, var))
		{
			m_nRowNum = (long)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
		if (tab.GetValue(0, PF_ARRAYCOLNUM, var))
		{
			m_nColNum = (long)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
		if (tab.GetValue(0, PF_ARRAYROWOFF, var))
		{
			m_fRowOff = (double)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
		if (tab.GetValue(0, PF_ARRAYCOLOFF, var))
		{
			m_fColOff = (double)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
		if (tab.GetValue(0, PF_ARRAYANGLE, var))
		{
			m_fArrayAngle = (double)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
	}
	else if (m_nType == 1)
	{
		if (tab.GetValue(0, PF_ARRAYCIRCLEMODE, var))
		{
			m_nCircleMode = (long)(_variant_t)*var;
			SetSettingsModifyFlag();
			if (!bInit)
			{
				Abort();
				return;
			}
		}

		if (tab.GetValue(0, PF_ARRAYITEMNUM, var))
		{
			m_nItemNum = (long)(_variant_t)*var;
			SetSettingsModifyFlag();
		}

		if (tab.GetValue(0, PF_ARRAYFILLANGLE, var))
		{
			m_fFillAngle = (double)(_variant_t)*var;
			SetSettingsModifyFlag();
		}

		if (tab.GetValue(0, PF_ARRAYITEMANGLE, var))
		{
			m_fItemAngle = (double)(_variant_t)*var;
			SetSettingsModifyFlag();
		}

		if (tab.GetValue(0, PF_ARRAYROTATEITEM, var))
		{
			m_bRotateItem = (bool)(_variant_t)*var;
			SetSettingsModifyFlag();
		}

		if (tab.GetValue(0, PF_ARRAYDEFAULTBASEPT, var))
		{
			//m_bDefaultBasePt = !(bool)(_variant_t)*var;
			m_bDefaultBasePt = (bool)(_variant_t)*var;
			SetSettingsModifyFlag();
		}
	}

	CEditCommand::SetParams(tab, bInit);
}

void CModifyArrayCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nType);
	tab.AddValue(PF_ARRAYTYPE, &CVariantEx(var));

	var = (long)(m_nRowNum);
	tab.AddValue(PF_ARRAYROWNUM, &CVariantEx(var));

	var = (long)(m_nColNum);
	tab.AddValue(PF_ARRAYCOLNUM, &CVariantEx(var));

	var = (double)(m_fRowOff);
	tab.AddValue(PF_ARRAYROWOFF, &CVariantEx(var));

	var = (double)(m_fColOff);
	tab.AddValue(PF_ARRAYCOLOFF, &CVariantEx(var));

	var = (double)(m_fArrayAngle);
	tab.AddValue(PF_ARRAYANGLE, &CVariantEx(var));

	var = (long)(m_nCircleMode);
	tab.AddValue(PF_ARRAYCIRCLEMODE, &CVariantEx(var));

	var = (long)(m_nItemNum);
	tab.AddValue(PF_ARRAYITEMNUM, &CVariantEx(var));

	var = (double)(m_fFillAngle);
	tab.AddValue(PF_ARRAYFILLANGLE, &CVariantEx(var));

	var = (double)(m_fItemAngle);
	tab.AddValue(PF_ARRAYITEMANGLE, &CVariantEx(var));

	var = (bool)(m_bRotateItem);
	tab.AddValue(PF_ARRAYROTATEITEM, &CVariantEx(var));

	var = (bool)(m_bDefaultBasePt);
	tab.AddValue(PF_ARRAYDEFAULTBASEPT, &CVariantEx(var));
}



//////////////////////////////////////////////////////////////////////
// CParalleToSingleCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CParalleToSingleCommand, CEditCommand)
CParalleToSingleCommand::CParalleToSingleCommand()
{
	m_fWidLimit = 0;
}

CParalleToSingleCommand::~CParalleToSingleCommand()
{

}

CString CParalleToSingleCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_PARALLELTOSINGLE);
}

void CParalleToSingleCommand::Abort()
{
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CParalleToSingleCommand::Start()
{
	if (!m_pEditor)return;
	CEditCommand::Start();

	m_pEditor->CloseSelector();
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}


void CParalleToSingleCommand::PtClick(PT_3D &pt, int flag)
{
	Process();

	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_pEditor->RefreshView();
	Finish();
	m_nStep = 2;

	CEditCommand::PtClick(pt, flag);


}

void CParalleToSingleCommand::Process()
{
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if (!pDS) return;

	CFtrLayerArray arrLayers;
	pDS->GetFtrLayersByNameOrCode_editable(m_strLayer0, arrLayers);

	CUndoFtrs undo(m_pEditor, Name());

	int layid = -1;
	CFtrLayer *pTarLayer = NULL;
	if (!m_strLayer1.IsEmpty())
	{
		pTarLayer = pDS->GetFtrLayer(m_strLayer1);
		if (pTarLayer == NULL)
		{
			pTarLayer = new CFtrLayer();
			pTarLayer->SetName(m_strLayer1);
			m_pEditor->AddFtrLayer(pTarLayer);
		}
		layid = pTarLayer->GetID();
	}

	if (arrLayers.GetSize() <= 0 || (pTarLayer == NULL&&m_strLayer1.IsEmpty()))
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}

	int nCount = 0;

	for (int j = 0; j < arrLayers.GetSize(); j++)
	{
		CFtrLayer *pLayer = arrLayers.GetAt(j);
		int nObj = pLayer->GetObjectCount();
		for (int i = 0; i < nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if (!pFtr) continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo) continue;

			CGeometry *pNewCurve = NULL;

			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
			{
				//计算平行线线宽
				double v = ((CGeoParallel*)pGeo)->GetWidth();

				if (fabs(v) <= m_fWidLimit + GraphAPI::g_lfDisTolerance)
				{
					pNewCurve = ((CGeoParallel*)pGeo)->GetCenterlineCurve();
				}
			}
			else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
			{
				CGeoDCurve *pDCurve = ((CGeoDCurve*)pGeo);
				//计算平行线线宽
				double v = pDCurve->GetWidth();

				if (fabs(v) <= m_fWidLimit + GraphAPI::g_lfDisTolerance)
				{
					CInterpolateCommand tmp;
					tmp.InitForFunction(1);

					CArray<PT_3DEX, PT_3DEX> arrPts1, arrPts2;
					pDCurve->GetBaseShape(arrPts1);
					pDCurve->GetAssistShape(arrPts2);

					CFeature *pFtr3 = new CFeature();
					CFeature *pFtr4 = new CFeature();
					CGeoCurve *pCurve3 = new CGeoCurve();
					CGeoCurve *pCurve4 = new CGeoCurve();

					pCurve3->CreateShape(arrPts1.GetData(), arrPts1.GetSize());
					pCurve4->CreateShape(arrPts2.GetData(), arrPts2.GetSize());

					pFtr3->SetGeometry(pCurve3);
					pFtr4->SetGeometry(pCurve4);

					CPtrArray arrRets;
					tmp.InterpWholeObjByEquidis(pFtr3, pFtr4, pFtr3, &arrRets);

					if (arrRets.GetSize() == 1)
					{
						pNewCurve = ((CFeature*)arrRets[0])->GetGeometry()->Clone();
						delete (CFeature*)arrRets[0];
					}
					delete pFtr3;
					delete pFtr4;
				}
			}

			if (pNewCurve)
			{
				if (layid < 0)  pTarLayer = pLayer;
				CFeature * pFtr1 = pTarLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOCURVE);
				if (!pFtr1)
				{
					delete pNewCurve;
					continue;
				}

				pFtr1->SetID(OUID());
				pFtr1->SetGeometry(pNewCurve);

				if (!m_pEditor->AddObject(pFtr1, pTarLayer->GetID()))
				{
					delete pFtr1;
					continue;
				}

				if (layid < 0)
				{
					GETXDS(m_pEditor)->CopyXAttributes(pFtr, pFtr1);
				}
				undo.arrNewHandles.Add(FtrToHandle(pFtr1));
				undo.arrOldHandles.Add(FtrToHandle(pFtr));
				//删除原来的对象
				m_pEditor->DeleteObject(FtrToHandle(pFtr));

				nCount++;
			}
		}
	}
	undo.Commit();

	CString strMsg;
	strMsg.Format(IDS_PROCESS_OBJ_NUM, nCount);
	GOutPut(strMsg);
}


void CParalleToSingleCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ParalleToSingle", Name());
	param->AddLayerNameParamEx("Layer0", m_strLayer0, StrFromResID(IDS_CMDPLANE_HANDLELAYER));
	param->AddLayerNameParam("Layer1", m_strLayer1, StrFromResID(IDS_PLANENAME_PLACELAYER));
	param->AddParam("WidLimit", m_fWidLimit, StrFromResID(IDS_WIDTHLIMIT), StrFromResID(IDS_WIDTHLIMIT_DESC));
}


void CParalleToSingleCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "WidLimit", var))
	{
		m_fWidLimit = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "Layer0", var))
	{
		m_strLayer0 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "Layer1", var))
	{
		m_strLayer1 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);

}


void CParalleToSingleCommand::GetParams(CValueTable& tab)
{
	_variant_t var;
	var = (float)(m_fWidLimit);
	tab.AddValue("WidLimit", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayer0);
	tab.AddValue("Layer0", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayer1);
	tab.AddValue("Layer1", &CVariantEx(var));
}




//////////////////////////////////////////////////////////////////////
// CParalleToCurveCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CParalleToCurveCommand, CEditCommand)
CParalleToCurveCommand::CParalleToCurveCommand()
{
}

CParalleToCurveCommand::~CParalleToCurveCommand()
{

}

CString CParalleToCurveCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_PARALLELTOSINGLE);
}

void CParalleToCurveCommand::Abort()
{
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CParalleToCurveCommand::Start()
{
	if (!m_pEditor)return;
	CCommand::Start();

	PtClick(PT_3D(), 0);
	Finish();
	m_nExitCode = CMPEC_NULL;
}

void CParalleToCurveCommand::PtClick(PT_3D &pt, int flag)
{
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if (!pDS) return;

	CFtrLayerArray arrLayers;
	pDS->GetFtrLayersByNameOrCode_editable("", arrLayers);

	CUndoFtrs undo(m_pEditor, Name());

	if (arrLayers.GetSize() <= 0)
	{
		return;
	}

	int nCount = 0;

	for (int j = 0; j < arrLayers.GetSize(); j++)
	{
		CFtrLayer *pLayer = arrLayers.GetAt(j);
		int nObj = pLayer->GetObjectCount();
		for (int i = 0; i < nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if (!pFtr) continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo) continue;

			CGeometry *pNewCurve = NULL;

			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
			{
				//计算平行线线宽
				double w = ((CGeoParallel*)pGeo)->GetWidth();

				if (fabs(w) < GraphAPI::g_lfDisTolerance)
				{
					pNewCurve = ((CGeoParallel*)pGeo)->GetCenterlineCurve();
				}
			}

			if (pNewCurve)
			{
				CFeature * pFtr1 = pFtr->Clone();
				if (!pFtr1)
				{
					delete pNewCurve;
					continue;
				}

				pFtr1->SetID(OUID());
				pFtr1->SetGeometry(pNewCurve);

				if (!m_pEditor->AddObject(pFtr1, pLayer->GetID()))
				{
					delete pFtr1;
					continue;
				}

				GETXDS(m_pEditor)->CopyXAttributes(pFtr, pFtr1);
				undo.arrNewHandles.Add(FtrToHandle(pFtr1));
				undo.arrOldHandles.Add(FtrToHandle(pFtr));
				//删除原来的对象
				m_pEditor->DeleteObject(FtrToHandle(pFtr));

				nCount++;
			}
		}
	}
	undo.Commit();

	CString strMsg;
	strMsg.Format(IDS_PROCESS_OBJ_NUM, nCount);
	GOutPut(strMsg);
}

//////////////////////////////////////////////////////////////////////
// CSurfaceToPointCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSurfaceToPointCommand, CEditCommand)
CSurfaceToPointCommand::CSurfaceToPointCommand()
{
	m_fAreaLimit = 0;
}

CSurfaceToPointCommand::~CSurfaceToPointCommand()
{

}

CString CSurfaceToPointCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_SURFACETOPOINT);
}

void CSurfaceToPointCommand::Abort()
{
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CSurfaceToPointCommand::Start()
{
	if (!m_pEditor)return;
	CEditCommand::Start();

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
	m_pEditor->CloseSelector();
}


void CSurfaceToPointCommand::PtClick(PT_3D &pt, int flag)
{
	Process();

	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_pEditor->RefreshView();

	CEditCommand::PtClick(pt, flag);


}

void CSurfaceToPointCommand::Process()
{
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if (!pDS) return;

	CFtrLayerArray arrLayers;
	pDS->GetFtrLayersByNameOrCode_editable(m_strLayer0, arrLayers);

	CUndoFtrs undo(m_pEditor, Name());

	CFtrLayer *pTarLayer = pDS->GetFtrLayer(m_strLayer1);

	if (arrLayers.GetSize() <= 0 || (pTarLayer == NULL&&m_strLayer1.IsEmpty()))
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}

	int nCount = 0;

	for (int j = 0; j < arrLayers.GetSize(); j++)
	{
		CFtrLayer *pLayer = arrLayers.GetAt(j);
		int nObj = pLayer->GetObjectCount();
		for (int i = 0; i < nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if (!pFtr) continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo) continue;
			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				//计算面积
				double v = ((CGeoCurveBase*)pGeo)->GetArea();

				if (v <= m_fAreaLimit + GraphAPI::g_lfAreaTolerance &&
					((pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)pGeo)->IsClosed()) ||
					(pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) && ((CGeoSurface*)pGeo)->IsClosed())))
				{
					if (pTarLayer == NULL)
					{
						pTarLayer = new CFtrLayer();
						pTarLayer->SetName(m_strLayer1);
						m_pEditor->AddFtrLayer(pTarLayer);
					}

					CFeature * pFtr1 = pTarLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOPOINT);
					if (!pFtr1) return;
					pFtr1->SetID(OUID());

					PT_3DEX expt;
					expt.pencode = penLine;
					pGeo->GetCenter(NULL, &expt);

					pFtr1->GetGeometry()->SetDataPoint(0, expt);

					if (!m_pEditor->AddObject(pFtr1, pTarLayer->GetID()))
					{
						delete pFtr1;
						continue;
					}

					undo.arrNewHandles.Add(FtrToHandle(pFtr1));
					undo.arrOldHandles.Add(FtrToHandle(pFtr));
					//删除原来的对象
					m_pEditor->DeleteObject(FtrToHandle(pFtr));

					nCount++;
				}
			}
		}
	}
	undo.Commit();

	CString strMsg;
	strMsg.Format(IDS_PROCESS_OBJ_NUM, nCount);
	GOutPut(strMsg);
}


void CSurfaceToPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("SurfaceToPoint", Name());
	param->AddLayerNameParamEx("Layer0", m_strLayer0, StrFromResID(IDS_CMDPLANE_HANDLELAYER));
	param->AddLayerNameParam("Layer1", m_strLayer1, StrFromResID(IDS_PLANENAME_PLACELAYER));
	param->AddParam("Limit", m_fAreaLimit, StrFromResID(IDS_CMDPLANE_AREALIMIT), StrFromResID(IDS_CMDPLANE_AREALIMIT_DESC));
}


void CSurfaceToPointCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "Limit", var))
	{
		m_fAreaLimit = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "Layer0", var))
	{
		m_strLayer0 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "Layer1", var))
	{
		m_strLayer1 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);

}


void CSurfaceToPointCommand::GetParams(CValueTable& tab)
{
	_variant_t var;
	var = (float)(m_fAreaLimit);
	tab.AddValue("Limit", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayer0);
	tab.AddValue("Layer0", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayer1);
	tab.AddValue("Layer1", &CVariantEx(var));
}


//////////////////////////////////////////////////////////////////////
// CDeleteDHeightCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDeleteDHeightCommand, CEditCommand)
CDeleteDHeightCommand::CDeleteDHeightCommand()
{
	m_fDZLimit = 0;
}

CDeleteDHeightCommand::~CDeleteDHeightCommand()
{

}

CString CDeleteDHeightCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_DELETE_DHEIGHT);
}

void CDeleteDHeightCommand::Abort()
{
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CDeleteDHeightCommand::Start()
{
	if (!m_pEditor)return;
	CEditCommand::Start();

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
	m_pEditor->CloseSelector();
}

void CDeleteDHeightCommand::PtClick(PT_3D &pt, int flag)
{
	Process();

	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_pEditor->RefreshView();

	CEditCommand::PtClick(pt, flag);


}

void CDeleteDHeightCommand::Process()
{
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if (!pDS) return;

	CPtrArray arrLayers;
	pDS->GetFtrLayer(m_strLayer0, NULL, &arrLayers);

	CUndoFtrs undo(m_pEditor, Name());

	if (arrLayers.GetSize() <= 0)
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}

	int nCount = 0;

	for (int j = 0; j < arrLayers.GetSize(); j++)
	{
		CFtrLayer *pLayer = (CFtrLayer*)arrLayers.GetAt(j);
		int nObj = pLayer->GetObjectCount();
		for (int i = 0; i < nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if (!pFtr) continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo) continue;
			CArray<PT_3DEX, PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && arrPts.GetSize() == 2)
			{
				//计算面积

				double v = arrPts[1].z - arrPts[0].z;

				if (fabs(v) <= m_fDZLimit + GraphAPI::g_lfZTolerance)
				{
					undo.arrOldHandles.Add(FtrToHandle(pFtr));
					//删除原来的对象
					m_pEditor->DeleteObject(FtrToHandle(pFtr));

					nCount++;
				}
			}
		}
	}
	undo.Commit();

	CString strMsg;
	strMsg.Format(IDS_DELETE_OBJSUM, nCount);
	GOutPut(strMsg);
}


void CDeleteDHeightCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DeleteDHeight", Name());
	param->AddLayerNameParam("Layer0", m_strLayer0, StrFromResID(IDS_CMDPLANE_HANDLELAYER));
	param->AddParam("Limit", m_fDZLimit, StrFromResID(IDS_CMDPLANE_DZLIMIT), StrFromResID(IDS_CMDPLANE_DZLIMIT_DESC));
}


void CDeleteDHeightCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "Limit", var))
	{
		m_fDZLimit = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "Layer0", var))
	{
		m_strLayer0 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);

}


void CDeleteDHeightCommand::GetParams(CValueTable& tab)
{
	_variant_t var;
	var = (float)(m_fDZLimit);
	tab.AddValue("Limit", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayer0);
	tab.AddValue("Layer0", &CVariantEx(var));
}



//////////////////////////////////////////////////////////////////////
// CCreateWidthNoteCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCreateWidthNoteCommand, CEditCommand)
CCreateWidthNoteCommand::CCreateWidthNoteCommand()
{
	m_nDigitNum = 2;
	m_nMode = modeAllMap;
}

CCreateWidthNoteCommand::~CCreateWidthNoteCommand()
{

}

CString CCreateWidthNoteCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CREATE_WIDTHNOTE);
}

void CCreateWidthNoteCommand::Abort()
{
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CCreateWidthNoteCommand::Start()
{
	if (!m_pEditor)return;
	CEditCommand::Start();

	//居中对齐
	m_sTxtSettings.nAlignment = TAH_MID | TAV_MID;

	if (m_nMode == modeAllMap)
	{
		GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
		m_pEditor->CloseSelector();
	}
}

void CCreateWidthNoteCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nMode == modeAllMap)
	{
		Process();

		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
	}
	else
	{
		if (m_nStep == 0)
		{
			if (!CanGetSelObjs(flag))return;

			CFtrLayer *pTarLayer = m_pEditor->GetDataSource()->GetFtrLayer(m_strLayer1);

			if (pTarLayer == NULL && m_strLayer1.IsEmpty())
			{
				GOutPut(StrFromResID(IDS_PARAM_ERROR));
				return;
			}

			EditStepOne();
		}

		if (m_nStep == 1)
		{
			CSelection* pSel = m_pEditor->GetSelection();

			int num;
			const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
			CUndoFtrs undo(m_pEditor, Name());
			for (int i = num - 1; i >= 0; i--)
			{
				ProcessOne(HandleToFtr(handles[i]), &undo);
			}

			undo.Commit();

			CString strMsg;
			strMsg.Format(IDS_PROCESS_OBJ_NUM, undo.arrNewHandles.GetSize());
			GOutPut(strMsg);

			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();

			Finish();
			m_nStep = 2;
		}
	}

	CEditCommand::PtClick(pt, flag);
}


BOOL CCreateWidthNoteCommand::ProcessOne(CFeature *pFtr, CUndoFtrs *pUndo)
{
	CGeometry *pGeo = pFtr->GetGeometry();
	if (!pGeo) return FALSE;
	CArray<PT_3DEX, PT_3DEX> arrPts, arrPts1;
	pGeo->GetShape(arrPts);
	CFtrLayer *pTarLayer = m_pEditor->GetDataSource()->GetFtrLayer(m_strLayer1);

	if (pTarLayer == NULL && m_strLayer1.IsEmpty())
		return FALSE;

	CDataSourceEx *pDS = m_pEditor->GetDataSource();

	BOOL bOK = FALSE;
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) && arrPts.GetSize() > 1)
	{
		((CGeoParallel*)pGeo)->GetParallelShape(arrPts1);

		if (arrPts1.GetSize() < 1)
			return FALSE;

		//计算两端的中心点
		PT_3DEX pt0, pt1;
		pt0.x = (arrPts[0].x + arrPts1[0].x)*0.5;
		pt0.y = (arrPts[0].y + arrPts1[0].y)*0.5;
		pt0.z = (arrPts[0].z + arrPts1[0].z)*0.5;
		int idx = arrPts1.GetSize() - 1;
		pt1.x = (arrPts[idx].x + arrPts1[idx].x)*0.5;
		pt1.y = (arrPts[idx].y + arrPts1[idx].y)*0.5;
		pt1.z = (arrPts[idx].z + arrPts1[idx].z)*0.5;
		pt0.pencode = pt1.pencode = penLine;

		double v = ((CGeoParallel*)pGeo)->GetWidth();
		v = fabs(v);

		if (pTarLayer == NULL)
		{
			pTarLayer = new CFtrLayer();
			pTarLayer->SetName(m_strLayer1);
			m_pEditor->AddFtrLayer(pTarLayer);
		}

		//创建地物
		CFeature * pFtr1 = pTarLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOTEXT);
		if (!pFtr1)
			return FALSE;

		pFtr1->SetID(OUID());
		CFeature *pFtr2 = pFtr1->Clone();

		pFtr1->GetGeometry()->CreateShape(&pt0, 1);
		pFtr2->GetGeometry()->CreateShape(&pt1, 1);

		//设置内容，添加地物
		if (pFtr1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CGeoText *pGeoText = (CGeoText*)pFtr1->GetGeometry();
			pGeoText->SetSettings(&m_sTxtSettings);

			CString strText, strFormat;
			strFormat.Format("%%.%df", m_nDigitNum);
			strText.Format(strFormat, v);
			pGeoText->SetText(strText);

			pUndo->arrNewHandles.Add(FtrToHandle(pFtr1));
			m_pEditor->AddObject(pFtr1, pTarLayer->GetID());

			pGeoText = (CGeoText*)pFtr2->GetGeometry();
			pGeoText->SetSettings(&m_sTxtSettings);
			pGeoText->SetText(strText);

			pUndo->arrNewHandles.Add(FtrToHandle(pFtr2));
			m_pEditor->AddObject(pFtr2, pTarLayer->GetID());

			bOK = TRUE;
		}
		else
		{
			delete pFtr1;
			delete pFtr2;
		}
	}

	return bOK;
}

void CCreateWidthNoteCommand::Process()
{
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if (!pDS) return;

	CPtrArray arrLayers;
	pDS->GetFtrLayer(m_strLayer0, NULL, &arrLayers);
	CFtrLayer *pTarLayer = pDS->GetFtrLayer(m_strLayer1);

	CUndoFtrs undo(m_pEditor, Name());

	if (arrLayers.GetSize() <= 0 || (pTarLayer == NULL && m_strLayer1.IsEmpty()))
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}

	int nCount = 0;

	for (int j = 0; j < arrLayers.GetSize(); j++)
	{
		CFtrLayer *pLayer = (CFtrLayer*)arrLayers.GetAt(j);
		int nObj = pLayer->GetObjectCount();
		for (int i = 0; i < nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if (!pFtr) continue;

			if (ProcessOne(pFtr, &undo))
			{
				nCount++;
			}
		}
	}
	undo.Commit();

	CString strMsg;
	strMsg.Format(IDS_PROCESS_OBJ_NUM, nCount);
	GOutPut(strMsg);
}


void CCreateWidthNoteCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("CreateWidthNote", Name());

	param->BeginOptionParam("Mode", StrFromResID(IDS_CMDPLANE_BY));
	param->AddOption(StrFromResID(IDS_WHOLEMAP), 0, ' ', m_nMode == modeAllMap);
	param->AddOption(StrFromResID(IDS_SELECTOBJ), 1, ' ', m_nMode == modeSelect);
	param->EndOptionParam();

	if (bForLoad || m_nMode == modeAllMap)
	{
		param->AddLayerNameParam("Layer0", m_strLayer0, StrFromResID(IDS_CMDPLANE_HANDLELAYER));
	}

	param->AddLayerNameParam("Layer1", m_strLayer1, StrFromResID(IDS_PLANENAME_PLACELAYER));
	param->AddParam("DigitNum", m_nDigitNum, StrFromResID(IDS_CMDPLANE_DIGITNUM));

	param->AddFontNameParam(PF_FONT, m_sTxtSettings.strFontName, StrFromResID(IDS_CMDPLANE_FONT));
	param->AddParam(PF_CHARH, (double)m_sTxtSettings.fHeight, StrFromResID(IDS_CMDPLANE_CHARH));
}


void CCreateWidthNoteCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "Mode", var))
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if (!bInit)
		{
			Abort();
			return;
		}
	}
	if (tab.GetValue(0, "DigitNum", var))
	{
		m_nDigitNum = (long)(_variant_t)*var;
		if (m_nDigitNum < 1)
		{
			m_nDigitNum = 1;
			AfxMessageBox(IDS_PARAM_ERROR);
			return;
		}
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "Layer0", var))
	{
		m_strLayer0 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "Layer1", var))
	{
		m_strLayer1 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, PF_FONT, var))
	{
		CString temp = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		strncpy(m_sTxtSettings.strFontName, temp, sizeof(m_sTxtSettings.strFontName) - 1);
		TextStyle style = GetUsedTextStyles()->GetTextStyleByName(temp);
		if (style.IsValid())
		{
			m_sTxtSettings.fWidScale = style.fWidScale;
			m_sTxtSettings.nInclineType = style.nInclineType;
			m_sTxtSettings.fInclineAngle = style.fInclinedAngle;
			m_sTxtSettings.SetBold(style.bBold);
		}

		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_CHARH, var))
	{
		m_sTxtSettings.fHeight = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}


void CCreateWidthNoteCommand::GetParams(CValueTable& tab)
{
	_variant_t var;

	var = (long)(m_nMode);
	tab.AddValue("Mode", &CVariantEx(var));

	var = (long)(m_nDigitNum);
	tab.AddValue("DigitNum", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayer0);
	tab.AddValue("Layer0", &CVariantEx(var));

	var = (LPCTSTR)(m_strLayer1);
	tab.AddValue("Layer1", &CVariantEx(var));

	var = (LPCTSTR)(m_sTxtSettings.strFontName);
	tab.AddValue(PF_FONT, &CVariantEx(var));

	var = (double)(m_sTxtSettings.fHeight);
	tab.AddValue(PF_CHARH, &CVariantEx(var));
}





/////////////////////////////////////////////////////////////////////
// CReverseCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CReverseCommand, CEditCommand)

CReverseCommand::CReverseCommand()
{

}

CReverseCommand::~CReverseCommand()
{
}

void CReverseCommand::Abort()
{

}

CString CReverseCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_REVERSE);
}

void CReverseCommand::Start()
{
	if (!m_pEditor)return;
	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num > 0)
	{
		m_nStep = 0;
		CCommand::Start();

		PT_3D pt;
		PtClick(pt, SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	CEditCommand::Start();
}


void CReverseCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;
		EditStepOne();
	}

	if (m_nStep == 1)
	{
		CGeometry *pObj;
		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		CUndoFtrs undo(m_pEditor, Name());

		for (int i = num - 1; i >= 0; i--)
		{
			pObj = HandleToFtr(handles[i])->GetGeometry();
			if (!pObj)continue;

			if ((pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))))
			{
				CFeature *pFtr = HandleToFtr(handles[i])->Clone();
				if (!pFtr) continue;
				pFtr->SetID(OUID());

				pObj = pFtr->GetGeometry();
				if (!pObj) continue;

				CArray<PT_3DEX, PT_3DEX> arr, arr1;
				pObj->GetShape(arr);
				int ptsnum = arr.GetSize();

				if (ptsnum > 1 && arr[0].pencode == penMove)
				{
					arr[0].pencode = arr[1].pencode;
				}

				for (int j = 0; j < ptsnum; j++)
				{
					arr1.Add(arr[ptsnum - 1 - j]);
				}

				if (pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
				{
					for (j = 0; j < ptsnum - 1; j++)
					{
						if (arr1[j].pencode == penMove)
						{
							int pencode = arr1[j].pencode;
							arr1[j].pencode = arr1[j + 1].pencode;
							arr1[j + 1].pencode = penMove;
							break;
						}
					}
				}

				pObj->CreateShape(arr1.GetData(), arr1.GetSize());

				if (pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
				{
					double wid = ((CGeoParallel*)pObj)->GetWidth();
					((CGeoParallel*)pObj)->SetWidth(-wid);
				}

				if (!m_pEditor->AddObject(pFtr, m_pEditor->GetFtrLayerIDOfFtr(handles[i])))
				{
					delete pFtr;
					Abort();
					return;
				}

				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]), pFtr);
				undo.arrNewHandles.Add(FtrToHandle(pFtr));

				m_pEditor->DeleteObject(handles[i]);
				undo.arrOldHandles.Add(handles[i]);
			}

		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL, hc_Refresh, 0);
		Finish();
		m_nStep = 2;
	}

	CEditCommand::PtClick(pt, flag);
}

/////////////////////////////////////////////////////////////////////
// CLinearizeCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CLinearizeCommand, CEditCommand)

CLinearizeCommand::CLinearizeCommand()
{

}

CLinearizeCommand::~CLinearizeCommand()
{
}

void CLinearizeCommand::Abort()
{

}


CString CLinearizeCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_LINESERIAL);
}

void CLinearizeCommand::Start()
{
	if (!m_pEditor)return;
	int num;
	CDlgDoc *pDoc = (CDlgDoc*)m_pEditor;
	(CDlgDoc*)m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num > 0)
	{
		m_nStep = 0;
		CCommand::Start();

		PT_3D pt;
		PtClick(pt, SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	CEditCommand::Start();
}


CGeoSurface *NormalizeSurface(CEditor *pEditor, CGeoSurface *pGeo);

void CLinearizeCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;
		EditStepOne();
	}

	if (m_nStep == 1)
	{
		CGeometry *pObj, *pNewObj;
		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		CUndoFtrs undo(m_pEditor, Name());
		GProgressStart(num);
		for (int i = num - 1; i >= 0; i--)
		{
			GProgressStep();
			pObj = HandleToFtr(handles[i])->GetGeometry();
			CArray<PT_3DEX, PT_3DEX> arr;
			pObj->GetShape(arr);
			if (pObj && (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel))))
			{
				pNewObj = pObj->Linearize();
				if (!pNewObj)
				{
					continue;
				}
				CFeature * pFtr1 = HandleToFtr(handles[i])->Clone();
				if (!pFtr1) continue;

				pFtr1->SetID(OUID());
				if (pFtr1)pFtr1->SetGeometry(pNewObj);

				if (!m_pEditor->AddObject(pFtr1, m_pEditor->GetFtrLayerIDOfFtr(handles[i])))
				{
					delete pFtr1;
					Abort();
					return;
				}

				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]), pFtr1);
				undo.arrNewHandles.Add(FtrToHandle(pFtr1));

				undo.arrOldHandles.Add(handles[i]);
				m_pEditor->DeleteObject(handles[i]);
			}
		}

		GProgressEnd();

		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}

	CEditCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// CBreakCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CBreakCommand, CEditCommand)

CBreakCommand::CBreakCommand() :
m_undo(m_pEditor, Name())
{
	m_nStep = -1;
	strcat(m_strRegPath, "\\Break");
}

CBreakCommand::~CBreakCommand()
{

}



CString CBreakCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_BREAK);
}

void CBreakCommand::Start()
{
	if (!m_pEditor)return;

	m_undo.m_pEditor = m_pEditor;
	m_undo.m_strName = Name();

	m_nMode = modeDoublePoint;
	m_hOldHandle = 0;
	m_pObj = NULL;
	CEditCommand::Start();
	m_pEditor->OpenSelector(SELMODE_SINGLE);
	if (m_nMode == modeSinglePoint)
		;
	//	OutputTipString(StrFromResID(IDS_CMDTIP_CLICKBREAK));
	else if (m_nMode == modeDoublePoint)
		;
	//	OutputTipString(StrFromResID(IDS_CMDTIP_CLICKBREAK1));
	else
		m_pEditor->CloseSelector();
	//OutputTipString(StrFromResID(IDS_CMDTIP_CLICKBREAK3));
}

void CBreakCommand::PtReset(PT_3D &pt)
{
	if ((CFeature*)m_hOldHandle == NULL || IsProcOver(this))return;
	Abort();
	CEditCommand::PtReset(pt);
}

void CBreakCommand::Abort()
{
	if (m_hOldHandle != 0)
	{
		UpdateDispyParam param;
		param.handle = m_hOldHandle;
		param.type = UpdateDispyParam::typeVISIBLE;
		param.data.bVisible = true;
		PDOC(m_pEditor)->UpdateAllViews(NULL, hc_UpdateObjectDisplay, (CObject*)&param);
	}
	m_hOldHandle = 0;
	m_pObj = NULL;
	//	m_objID   = OBJ_GUID();
	m_nPtNum = -1;
	CEditCommand::Abort();

	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CBreakCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor)return;
	//	CValueTable tab;
	if (m_nStep == 0)
	{
		if (m_nMode == modeLinesection)
		{
			m_ptDragStart = pt;
			m_nStep = 1;
			GotoState(PROCSTATE_PROCESSING);
			return;
		}
		else
		{
			//得到目标地物
			if (!CanGetSelObjs(flag, FALSE))return;

			m_hOldHandle = m_pEditor->GetSelection()->GetLastSelectedObj();
			CPFeature pFtr = HandleToFtr(m_hOldHandle);
			if (!pFtr) return;
			CGeometry *pObj = pFtr->GetGeometry();

			if (!pObj)return;
			if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				return;

			m_pObj = (CGeoCurveBase*)pObj;

			//			m_pEditor->SetCurDrawingObj(DrawingInfo(m_pObj,m_pObj->GetDataPointSum()+1));

			m_pEditor->CloseSelector();

			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged(TRUE);
			m_pEditor->RefreshView();

			GotoState(PROCSTATE_PROCESSING);

			//得到打断的起点
			m_ptDragStart = pt;

			m_xTab.DelAll();
			m_xTab.BeginAddValueItem();
			GETXDS(m_pEditor)->GetXAttributes(HandleToFtr(m_hOldHandle), m_xTab);
			m_xTab.EndAddValueItem();

			UpdateDispyParam param;
			param.handle = m_hOldHandle;
			param.type = UpdateDispyParam::typeVISIBLE;
			param.data.bVisible = false;
			PDOC(m_pEditor)->UpdateAllViews(NULL, hc_UpdateObjectDisplay, (CObject*)&param);

			//m_pEditor->DeleteObject(m_hOldHandle);
			m_undo.arrOldHandles.Add(m_hOldHandle);

			GrBuffer buf;
			m_pObj->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
			//m_pEditor->UpdateDrag(NULL,hc_SetVariantDragLine,(CObject*)m_pObj->GetGrBuffer());
			m_pEditor->RefreshView();

			m_nStep = 1;
			GotoState(PROCSTATE_PROCESSING);

			//if( m_nMode==modeDoublePoint ) 
			//OutputTipString(StrFromResID(IDS_CMDTIP_CLICKBREAK2));
		}

	}
	else if (m_nStep == 1 && m_nMode == modeLinesection)//线段打断
	{
		m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
		m_ptDragEnd = pt;
		PT_3D Pt[2];
		(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_ptDragStart, Pt);//当前坐标系<-大地坐标系的转换
		(m_pEditor->GetCoordWnd()).m_pSearchCS->GroundToClient(&m_ptDragEnd, Pt + 1);

		Envelope el;
		el.CreateFromPts(Pt, 2);
		m_pEditor->GetDataQuery()->FindObjectInRect(el, (m_pEditor->GetCoordWnd()).m_pSearchCS);
		int num1;
		const	CPFeature *ftr = (m_pEditor->GetDataQuery())->GetFoundHandles(num1);//num1返回线段穿过的地物数

		//cjc 2012年11月19日 将折线打断成多段
		CGeometry *pObj, *pNewObj;

		for (int i = 0; i < num1; i++)
		{
			int PtNums;
			BOOL IsLinearize = FALSE;//是否需要线串化

			pObj = ftr[i]->GetGeometry();//得到地物对象
			pNewObj = pObj;
			if (!pObj)
			{
				continue;
			}

			else if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))continue;
			else if ((PtNums = pObj->GetDataPointSum()) < 2)continue;//获取地物上点个数
			else
			{
				CArray<PT_3DEX, PT_3DEX> arrPts;
				pNewObj->GetShape(arrPts);	//获取关键节点	
				for (int h = 0; h < arrPts.GetSize(); h++)
				{
					if (penArc == arrPts.GetAt(h).pencode || pen3PArc == arrPts.GetAt(h).pencode/* || penSpline == arrPts.GetAt(h).pencode*/)
					{
						IsLinearize = TRUE;
						break;
					}
				}

				if (IsLinearize)
				{
					pNewObj = pObj->Linearize();//圆弧、样条直接打断有可能出现错误，先线串化
					if (!pNewObj)
					{
						continue;
					}
					arrPts.RemoveAll();//删除原关键点
					pNewObj->GetShape(arrPts);	//重新获取关键节点
				}
				else
				{
					pNewObj = pObj;
				}
				PT_3DEX *ret;//返回的相交点
				ret = new PT_3DEX[arrPts.GetSize() - 1];
				CArray <PT_3DEX, PT_3DEX> retPts;//接受有效点

				int nRetPts;

				if (GraphAPI::GGetPointsOfIntersect(arrPts.GetData(), arrPts.GetSize(), PT_3DEX(m_ptDragStart, penLine), PT_3DEX(m_ptDragEnd, penLine), ret, &nRetPts))//获取相交点
				{
					for (int l = 0; l < nRetPts; l++)
					{
						retPts.Add(ret[l]);//添加有效点
					}

					CFeature *pTempl = ftr[i]->Clone();
					if (!pTempl)return;
					pTempl->SetID(OUID());
					CGeometry *objs[3] = { 0 };

					GrBuffer buf;
					pNewObj->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
					m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);

					//处理打断交点
					for (int j = 0; j < retPts.GetSize(); j++)
					{
						if (!pNewObj) break;
						CGeometry *tObj;
						((CGeoCurveBase*)pNewObj)->GetBreak(retPts.GetAt(j), retPts.GetAt(j), objs[0], objs[1], objs[2], TRUE);

						tObj = objs[2];

						if (objs[1])
						{
							delete objs[1];
							objs[1] = NULL;
						}

						CGeometry *pObj1;
						for (int k = 0; k < 3; k++)
						{
							pObj1 = objs[k];
							if (pObj1)
							{
								CFeature *pFtr = pTempl->Clone();
								pFtr->SetID(OUID());
								if (pFtr)
								{
									pFtr->SetGeometry(pObj1);
								}

								if (k == 2)
								{
									if (retPts.GetSize() != 1 && j != retPts.GetSize() - 1)//若有多个打断点，每次都跳过第二段地物不添加到数据源 
									{
										break;
									}
								}
								m_pEditor->AddObject(pFtr, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(ftr[i])));//增加一个地物 
								m_undo.arrNewHandles.Add(FtrToHandle(pFtr));
								GETXDS(m_pEditor)->CopyXAttributes(ftr[i], pFtr);//增加到数据源
							}
						}

						pNewObj = tObj;//将打断后的第二段指针给pObj 以便于再次循环打断
					}

					//删除杂余地物
					m_undo.arrOldHandles.Add(FtrToHandle(ftr[i]));
					m_pEditor->DeleteObject(FtrToHandle(ftr[i]));
					m_pEditor->UpdateDrag(ud_ClearDrag);

					//完成回收资源
					delete pTempl;
					pTempl = NULL;
					delete[]ret;
					ret = NULL;
					retPts.RemoveAll();
				}
			}
			pObj = NULL;
			pNewObj = NULL;
		}

		m_undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged(TRUE);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 3;

	}
	//得到打断的终点
	if ((m_nStep == 2 && m_nMode == modeDoublePoint) || (m_nStep == 1 && m_nMode == modeSinglePoint))
	{
		if (m_nStep == 1)m_ptDragEnd = m_ptDragStart;
		//打断地物
		CGeometry *objs[3] = { 0 };

		m_pEditor->DeleteObject(m_hOldHandle);

		CPFeature pTempl = HandleToFtr(m_hOldHandle)->Clone();
		if (!pTempl)
		{
			return;
		}
		pTempl->SetToDeleted(FALSE);

		m_pObj->GetBreak(m_ptDragStart, m_ptDragEnd, objs[0], objs[1], objs[2], TRUE);

		if (objs[1])
		{
			delete objs[1];
			objs[1] = NULL;
		}

		CGeometry *pObj;
		for (int i = 0; i < 3; i++)
		{
			pObj = objs[i];
			if (pObj)
			{
				CFeature *pFtr0 = pTempl->Clone();
				pFtr0->SetID(OUID());
				if (pFtr0)
				{
					pFtr0->SetGeometry(pObj);
				}
				m_pEditor->AddObject(pFtr0, m_pEditor->GetFtrLayerIDOfFtr(m_hOldHandle));
				m_undo.arrNewHandles.Add(FtrToHandle(pFtr0));
				GETXDS(m_pEditor)->SetXAttributes(pFtr0, m_xTab);
			}
		}
		delete pTempl;

		m_pObj = NULL;

		m_undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged(TRUE);
		m_pEditor->RefreshView();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		//完成
		Finish();
		m_nStep = 3;
	}

	CEditCommand::PtClick(pt, flag);
}

void CBreakCommand::PtMove(PT_3D &pt)
{
	if (m_nStep == 1 && m_nMode == modeLinesection)
	{
		GrBuffer buf;
		buf.BeginLineString(0, 0, 0);
		buf.MoveTo(&m_ptDragStart);
		buf.LineTo(&pt);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);


		return;

	}
	//动态显示打断状态
	if (m_nStep == 1 || m_nStep == 2)
	{
		m_nStep = 2;
		m_ptDragEnd = pt;

		CGeometry *objs[3] = { 0 };
		if (m_pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			((CGeoCurveBase*)m_pObj)->GetBreak(m_ptDragStart, m_ptDragEnd, objs[0], objs[1], objs[2], TRUE);

		if (objs[1])
		{
			delete objs[1];
			objs[1] = NULL;
		}

		CGeometry *pObj;
		GrBuffer buf;
		//获得打断后的地物的显示数据
		for (int i = 0; i < 3; i++)
		{
			pObj = objs[i];
			if (pObj)
			{
				pObj->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
				delete objs[i];
				objs[i] = NULL;
			}
		}

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
	}
	CEditCommand::PtMove(pt);
}

void CBreakCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nMode);
	tab.AddValue(PF_BREAKWAY, &CVariantEx(var));
}

void CBreakCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(PID_BREAK, StrFromLocalResID(IDS_CMDNAME_BREAK));
	param->BeginOptionParam(PF_BREAKWAY, StrFromResID(IDS_CMDPLANE_BY));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SINGLEPT), 0, ' ', m_nMode == modeSinglePoint);
	param->AddOption(StrFromResID(IDS_CMDPLANE_TWOPT), 1, ' ', m_nMode == modeDoublePoint);
	param->AddOption(StrFromResID(IDS_CMDPLANE_LINESECTION), 2, ' ', m_nMode == modeLinesection);
	param->EndOptionParam();
}


void CBreakCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	//	bool bNeedRefresh = false;

	if (tab.GetValue(0, PF_BREAKWAY, var))
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if (!bInit)
		{
			Abort();
			return;
		}
	}
	if (m_nMode == modeLinesection)
	{
		m_pEditor->CloseSelector();
	}
	else
	{
		m_pEditor->OpenSelector(SELMODE_SINGLE);
	}

	CEditCommand::SetParams(tab, bInit);
}


void CBreakCommand::Finish()
{
	CEditCommand::Finish();
}


//////////////////////////////////////////////////////////////////////
// CCopyLineCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCopyLineCommand, CEditCommand)

CCopyLineCommand::CCopyLineCommand() :
m_undo(m_pEditor, Name())
{
	m_nStep = -1;
	strcat(m_strRegPath, "\\CopyLine");

	m_bStartSnapLayerName = FALSE;
}

CCopyLineCommand::~CCopyLineCommand()
{

}



CString CCopyLineCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_COPYLINE);
}

void CCopyLineCommand::Start()
{
	if (!m_pEditor)return;

	m_undo.m_pEditor = m_pEditor;
	m_undo.m_strName = Name();

	m_hOldHandle = 0;
	m_pObj = NULL;
	CEditCommand::Start();
	m_pEditor->OpenSelector(SELMODE_SINGLE);
}

void CCopyLineCommand::PtReset(PT_3D &pt)
{
	if ((CFeature*)m_hOldHandle == NULL || IsProcOver(this))return;
	Abort();
	CEditCommand::PtReset(pt);
}

void CCopyLineCommand::Abort()
{
	// 	if (m_hOldHandle != 0)
	// 	{
	// 		m_pEditor->RestoreObject(m_hOldHandle);
	// 	}
	m_hOldHandle = 0;
	m_pObj = NULL;
	CEditCommand::Abort();

	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CCopyLineCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor)return;
	//CValueTable tab;

	if (m_bStartSnapLayerName)
	{
		CSelection * pSel = m_pEditor->GetSelection();
		int num = 0;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		if (num <= 0)
			return;

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(handles[0]));
		m_strRetLay = pLayer->GetName();
		SetSettingsModifyFlag();

		CUIParam param;
		FillShowParams(&param);
		m_pEditor->UIRequest(UIREQ_TYPE_SHOWUIPARAM, (LONG_PTR)&param);

		m_bStartSnapLayerName = FALSE;
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		return;
	}

	if (m_nStep == 0)
	{
		//得到目标地物
		if (!CanGetSelObjs(flag, FALSE))return;

		m_hOldHandle = m_pEditor->GetSelection()->GetLastSelectedObj();
		CPFeature pFtr = HandleToFtr(m_hOldHandle);
		if (!pFtr) return;
		CGeometry *pObj = (CGeoCurveBase*)pFtr->GetGeometry();

		if (!pObj)return;
		if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))return;

		m_pObj = (CGeoCurveBase*)pObj;

		m_pEditor->CloseSelector();

		GotoState(PROCSTATE_PROCESSING);

		//得到打断的起点
		m_ptDragStart = pt;
		// 		tab.BeginAddValueItem();
		// 		GETXDS(m_pEditor)->GetXAttributes(HandleToFtr(m_hOldHandle),tab);
		// 		tab.EndAddValueItem();

		//		m_pEditor->DeleteObject(m_hOldHandle);
		//		m_undo.arrOldHandles.Add(m_hOldHandle);

		// 		GrBuffer buf;
		// 		m_pObj->Draw(&buf);
		// 		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		// 		m_pEditor->RefreshView();

		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);

	}
	//得到打断的终点
	if (m_nStep == 2)
	{
		if (m_strRetLay.IsEmpty())
		{
			Abort();
			return;
		}

		if (m_nStep == 1)m_ptDragEnd = m_ptDragStart;
		//打断地物
		CGeometry *objs[3] = { 0 };

		CPFeature pTempl = HandleToFtr(m_hOldHandle)->Clone();
		if (!pTempl)
		{
			return;
		}
		pTempl->SetToDeleted(FALSE);

		m_pObj->GetBreak(m_ptDragStart, m_ptDragEnd, objs[0], objs[1], objs[2], TRUE);

		if (objs[1])
		{
			CGeometry *pObj = objs[1];
			if (pObj)
			{
				CFeature *pFtr0 = pTempl->Clone();
				pFtr0->SetID(OUID());
				if (pFtr0)
				{
					pFtr0->SetGeometry(pObj);
				}

				CFtrLayer *pFtrLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(m_strRetLay);
				if (pFtrLayer)
				{
					CFeature *pFtr2 = pFtrLayer->CreateDefaultFeature(m_pEditor->GetDataSource()->GetScale(), pObj->GetClassType());
					CGeometry *pObj2 = (pFtr2 == NULL ? NULL : pFtr2->GetGeometry());
					if (pObj2 != NULL && pObj2->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
					{
						((CGeoCurveBase*)pObj)->SetLineWidth(((CGeoCurveBase*)pObj2)->GetLineWidth());
					}
					if (pFtr2)
						delete pFtr2;
					m_pEditor->AddObject(pFtr0, pFtrLayer->GetID());
					m_undo.arrNewHandles.Add(FtrToHandle(pFtr0));
				}

			}
		}

		if (objs[0])
		{
			delete objs[0];
			objs[0] = NULL;
		}

		if (objs[2])
		{
			delete objs[2];
			objs[2] = NULL;
		}


		delete pTempl;

		m_pObj = NULL;

		m_undo.Commit();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		//完成
		Finish();
		m_nStep = 3;
	}

	CEditCommand::PtClick(pt, flag);
}

void CCopyLineCommand::PtMove(PT_3D &pt)
{

	if (m_nStep == 1 || m_nStep == 2)
	{
		m_nStep = 2;
		m_ptDragEnd = pt;

		CGeometry *objs[3] = { 0 };
		m_pObj->GetBreak(m_ptDragStart, m_ptDragEnd, objs[0], objs[1], objs[2], TRUE);

		GrBuffer buf;
		if (objs[1])
		{
			CGeometry *pObj = objs[1];
			if (pObj)pObj->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());

			buf.SetAllColor(RGB(255, 0, 0));

		}

		if (objs[0])
		{
			delete objs[0];
			objs[0] = NULL;
		}

		if (objs[2])
		{
			delete objs[2];
			objs[2] = NULL;
		}

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
	}
	CEditCommand::PtMove(pt);
}

void CCopyLineCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);

	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strRetLay;
	tab.AddValue(PF_COPYLINELAYER_DES, &CVariantEx(var));
}

void CCopyLineCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("CopyLine"), StrFromResID(IDS_CMDNAME_COPYLINE));
	param->AddLayerNameParam(PF_COPYLINELAYER_DES, (LPCTSTR)m_strRetLay, StrFromResID(IDS_CMDPLANE_RETLAY));
	param->AddButtonParam("SnapLayerName", StrFromResID(IDS_SNAPLAYERNAME));
}


void CCopyLineCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, PF_COPYLINELAYER_DES, var))
	{
		m_strRetLay = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "SnapLayerName", var))
	{
		m_bStartSnapLayerName = TRUE;
		SetSettingsModifyFlag();

		m_pEditor->OpenSelector();
	}

	CEditCommand::SetParams(tab, bInit);
}


void CCopyLineCommand::Finish()
{
	CEditCommand::Finish();
}

/////////////////////////////////////////////////////////////////////
// CRectifyCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CRectifyCommand, CEditCommand)

CRectifyCommand::CRectifyCommand()
{
	m_nStep = -1;
	m_nMode = modeAllAngles;
	m_pFtr = NULL;
	m_lfSigma = 1.0;
	m_arrHandles.RemoveAll();
	strcat(m_strRegPath, "\\Rectify");
}

CRectifyCommand::~CRectifyCommand()
{
	// 	if( m_pFtr )delete m_pFtr;
	m_pFtr = NULL;
}

void CRectifyCommand::Abort()
{
	// 	if( m_pFtr )delete m_pFtr;
	m_pFtr = NULL;

	CEditCommand::Abort();
}

CString CRectifyCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_RECTIFY);
}

void CRectifyCommand::Start()
{
	if (!m_pEditor)return;

	if (m_pFtr)m_pFtr = NULL;

	m_nMode = modeAllAngles;

	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num > 0)
	{
		if (m_nMode == modeAllAngles)
		{
			m_nStep = 0;
			// 		m_idsOld.RemoveAll();
			// 		m_idsNew.RemoveAll();

			CEditCommand::Start();

			PT_3D pt;
			PtClick(pt, 0);
			PtClick(pt, 0);
			m_nExitCode = CMPEC_STARTOLD;
			return;
			// 			PT_3D pt;
			// 			PtClick(pt,SELSTAT_NONESEL);
			// 			PtClick(pt,SELSTAT_NONESEL);
			// 			m_nExitCode = CMPEC_STARTOLD;
			// 			return;
		}

	}

	CEditCommand::Start();
	if (m_nMode == modeOneAngle)
	{
		m_pEditor->OpenSelector(SELMODE_SINGLE);
		//	OutputTipString(StrFromResID(IDS_CMDTIP_RECTIFYEDGE1));
	}
}

CGeometry *CRectifyCommand::Rectify(CGeometry* pObj, double tolerance, bool bLockStart, bool bLockEnd)
{
	if (!pObj)return NULL;
	//准备
	CArray<PT_3DEX, PT_3DEX> arr;
	pObj->GetShape(arr);
	int num = arr.GetSize();
	if (num <= 2)return NULL;

	PT_3D *pts = new PT_3D[num];
	if (!pts)return NULL;

	//取点
	PT_3DEX expt;
	for (int j = 0; j < num; j++)
	{
		COPY_3DPT(pts[j], arr[j]);
	}

	//直角化
	if (GraphAPI::GRectifyPoints(pts, num, tolerance, bLockStart, bLockEnd))
	{
		//生成新的对象
		pObj = pObj->Clone();
		if (pObj)
		{
			CArray<PT_3DEX, PT_3DEX> arr1;
			arr1.Copy(arr);

			for (j = 0; j < num; j++)
			{
				COPY_3DPT(arr1[j], pts[j]);
			}

			pObj->CreateShape(arr1.GetData(), arr1.GetSize());

			delete[] pts;
			return pObj;
		}
	}
	else
	{
		//	OutputTipString(StrFromResID(IDS_ERR_RECTIFY));
	}

	delete[] pts;
	return NULL;
}

void CRectifyCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nMode == modeOneAngle)
	{
		if (m_nStep == 0)
		{
			int num;
			m_pEditor->GetSelection()->GetSelectedObjs(num);
			if (num > 0)
			{
				//if( m_pFtr )delete m_pFtr;
				if (!CanGetSelObjs(flag, FALSE))return;

				FTR_HANDLE handle = m_pEditor->GetSelection()->GetLastSelectedObj();
				m_pFtr = HandleToFtr(handle);
				CGeometry *pObj = NULL;
				if (m_pFtr)
				{
					/*	m_pFtr = m_pFtr->Clone();*/
					pObj = m_pFtr->GetGeometry();
				}
				if (!pObj)return;
				//m_pObj = m_pObj->Clone();

				//	m_idsOld.Add(objID);

				double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();

				PT_3D pt1;
				m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt, &pt1);
				m_nPtNum = pObj->FindNearestKeyLine(pt1, r, m_pEditor->GetCoordWnd().m_pSearchCS);


				if (m_nPtNum >= 0 && m_nPtNum < pObj->GetDataPointSum())
				{
					EditStepOne();

					//		OutputTipString(StrFromResID(IDS_CMDTIP_RECTIFYEDGE2));
				}
			}
		}
		else if (m_nStep == 1)
		{
			if (!m_pFtr)return;

			double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();

			CGeometry *pObj = m_pFtr->GetGeometry();
			if (!pObj) return;
			//获取第二段关键线段
			PT_3D pt1;
			m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt, &pt1);
			int nPtNum = pObj->FindNearestKeyLine(pt1, r, m_pEditor->GetCoordWnd().m_pSearchCS);

			int sum = pObj->GetDataPointSum();
			if (nPtNum < 0 || nPtNum >= sum)return;

			BOOL bRepeatPoint = FALSE;
			if (sum >= 4)
			{
				PT_3DEX expt1, expt2;
				expt1 = pObj->GetDataPoint(0);
				expt2 = pObj->GetDataPoint(sum - 1);
				if (fabs(expt1.x - expt2.x) < 1e-4 && fabs(expt1.y - expt2.y) < 1e-4 &&
					((nPtNum == 0 && m_nPtNum == sum - 2) || (nPtNum == sum - 2 && m_nPtNum == 0)))
					bRepeatPoint = TRUE;
			}

			if (!bRepeatPoint && abs(nPtNum - m_nPtNum) != 1)return;

			m_pEditor->DeleteObject(FtrToHandle(m_pFtr), FALSE);

			CUndoModifyProperties undo(m_pEditor, Name());
			undo.arrHandles.Add(FtrToHandle(m_pFtr));
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			undo.oldVT.BeginAddValueItem();
			m_pFtr->WriteTo(undo.oldVT);
			undo.oldVT.EndAddValueItem();

			int target;
			PT_3DEX t;
			PT_3D pts[4];

			if (bRepeatPoint)
			{
				if (nPtNum == 0)
				{
					t = pObj->GetDataPoint(nPtNum + 1);
					COPY_3DPT(pts[0], t);
					t = pObj->GetDataPoint(nPtNum);
					COPY_3DPT(pts[1], t);
					t = pObj->GetDataPoint(m_nPtNum);
					COPY_3DPT(pts[2], t);
				}
				else
				{
					t = pObj->GetDataPoint(nPtNum + 1);
					COPY_3DPT(pts[0], t);
					t = pObj->GetDataPoint(nPtNum);
					COPY_3DPT(pts[1], t);
					t = pObj->GetDataPoint(m_nPtNum + 1);
					COPY_3DPT(pts[2], t);
				}

				target = 0;
			}
			else
			{
				if (nPtNum > m_nPtNum)
				{
					t = pObj->GetDataPoint(nPtNum + 1);
					COPY_3DPT(pts[0], t);
					t = pObj->GetDataPoint(nPtNum);
					COPY_3DPT(pts[1], t);
					t = pObj->GetDataPoint(m_nPtNum);
					COPY_3DPT(pts[2], t);

					target = nPtNum;
				}
				else
				{
					t = pObj->GetDataPoint(nPtNum);
					COPY_3DPT(pts[0], t);
					t = pObj->GetDataPoint(m_nPtNum);
					COPY_3DPT(pts[1], t);
					t = pObj->GetDataPoint(m_nPtNum + 1);
					COPY_3DPT(pts[2], t);

					target = m_nPtNum;
				}
			}

			GraphAPI::GGetPerpendicular(pts[0].x, pts[0].y, pts[1].x, pts[1].y, pts[2].x, pts[2].y, &pts[3].x, &pts[3].y, NULL);
			t = pObj->GetDataPoint(target);
			COPY_2DPT(t, pts[3]);
			pObj->SetDataPoint(target, t);

			if (bRepeatPoint)
			{
				pObj->SetDataPoint(sum - 1, t);
			}
			undo.newVT.BeginAddValueItem();
			m_pFtr->WriteTo(undo.newVT);
			undo.newVT.EndAddValueItem();

			m_pEditor->RestoreObject(FtrToHandle(m_pFtr));

			//m_pEditor->UpdateObject(FtrToHandle(m_pFtr));
			//	m_idsNew.Add(m_pDoc->AddObject(m_pObj,m_idsOld[0].layhdl));
			pObj = NULL;
			undo.Commit();
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged(TRUE);
			m_pEditor->RefreshView();
			Finish();
			m_nStep = 2;
		}
	}
	//整体模式
	else if (m_nMode == modeAllAngles)
	{
		if (m_nStep == 0)
		{
			if (!CanGetSelObjs(flag))return;
			int num;
			const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
			m_arrHandles.SetSize(num);
			memcpy(m_arrHandles.GetData(), handles, sizeof(FTR_HANDLE)*num);
			EditStepOne();
			//return;
		}

		if (m_nStep == 1)
		{
			CGeometry *pObj;
			int num = m_arrHandles.GetSize();

			GrBuffer buf;
			CUndoModifyProperties undo(m_pEditor, Name());
			for (int i = num - 1; i >= 0; i--)
			{
				pObj = HandleToFtr(m_arrHandles[i])->GetGeometry();
				if (pObj)
				{
					pObj = CRectifyCommand::Rectify(pObj, m_lfSigma, false, false);
					if (!pObj)
					{
						continue;
					}

					m_pEditor->DeleteObject(m_arrHandles[i], FALSE);

					undo.arrHandles.Add(m_arrHandles[i]);
					undo.oldVT.BeginAddValueItem();
					HandleToFtr(m_arrHandles[i])->WriteTo(undo.oldVT);
					undo.oldVT.EndAddValueItem();

					HandleToFtr(m_arrHandles[i])->SetGeometry(pObj);

					m_pEditor->RestoreObject(m_arrHandles[i]);

					// 					if(!m_pEditor->UpdateObject(m_arrHandles[i]))
					// 					{					
					// 						Abort();
					// 						return;
					// 					}

					undo.newVT.BeginAddValueItem();
					HandleToFtr(m_arrHandles[i])->WriteTo(undo.newVT);
					undo.newVT.EndAddValueItem();
					//	m_idsNew.Add(m_pDoc->AddObject(pObj,m_idsOld[i].layhdl));					
				}
			}
			undo.Commit();
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged(TRUE);
			m_pEditor->RefreshView();
			Finish();
			m_nStep = 2;
		}
	}

	CEditCommand::PtClick(pt, flag);
}

void CRectifyCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nMode);
	tab.AddValue(PF_RECTIFYWAY, &CVariantEx(var));
	var = (double)m_lfSigma;
	tab.AddValue(PF_RECTIFYTOL, &CVariantEx(var));

}

void CRectifyCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("RectifyCommand", StrFromLocalResID(IDS_CMDNAME_RECTIFY));
	param->BeginOptionParam(PF_RECTIFYWAY, StrFromResID(IDS_CMDPLANE_WAY));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SINGLEANG), 1, ' ', m_nMode == modeOneAngle);
	param->AddOption(StrFromResID(IDS_CMDPLANE_WHOLE), 0, ' ', m_nMode == modeAllAngles);
	param->EndOptionParam();
	if (m_nMode == modeAllAngles || bForLoad)
	{
		param->AddParam(PF_RECTIFYTOL, m_lfSigma, StrFromResID(IDS_CMDPLANE_TOLER));
	}
}


void CRectifyCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	//	bool bNeedRefresh = false;

	if (tab.GetValue(0, PF_RECTIFYWAY, var))
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_RECTIFYTOL, var))
	{
		m_lfSigma = (double)(_variant_t)*var;
		SetSettingsModifyFlag();

		WriteProfileDouble(CString(REGPATH_COMMAND) + "\\Rectify", "Sigma", m_lfSigma);
	}

	if (m_nMode == modeOneAngle)
	{
		m_pEditor->OpenSelector(SELMODE_SINGLE);
	}
	else
	{
		m_pEditor->OpenSelector();
	}
	CEditCommand::SetParams(tab, bInit);
}

/////////////////////////////////////////////////////////////////////
// CCombineCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCombineCommand, CEditCommand)

CCombineCommand::CCombineCommand()
{
	m_nStep = -1;
	m_pObjOld0 = NULL;
	m_pObjOld1 = NULL;
	m_pObjNew = NULL;

	m_ptNum0 = -1;
	m_ptNum1 = -1;
	m_bMatchHeight = FALSE;
	m_bLinkCountor = FALSE;
	m_bCombinBreak = FALSE;
	strcat(m_strRegPath, "\\Combine");
}

CCombineCommand::~CCombineCommand()
{

}

void CCombineCommand::Abort()
{
	CEditCommand::Abort();
}

CString CCombineCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_COMBINE);
}

void CCombineCommand::Start()
{
	if (!m_pEditor)return;

	m_pObjOld0 = NULL;
	m_pObjOld1 = NULL;
	m_pObjNew = NULL;
	m_bMatchHeight = FALSE;
	m_bLinkCountor = FALSE;
	m_bCombinBreak = FALSE;

	CEditCommand::Start();
	m_pEditor->OpenSelector(SELMODE_SINGLE);
	m_pEditor->DeselectAll();
}

int KickoffSame2DPoints_notPenMove(CArray<PT_3DEX, PT_3DEX>& arrPts);

void CCombineCommand::PtClick(PT_3D &pt, int flag)
{
	CGeometry *pObj;

	int num;
	const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num <= 0)
	{
		return;
	}

	if (m_nStep == 0)
	{
		if (num > 0)
		{
			if (!CanGetSelObjs(flag, FALSE))return;

			m_pObjOld0 = HandleToFtr(m_pEditor->GetSelection()->GetLastSelectedObj());
			pObj = m_pObjOld0->GetGeometry();

			if (!pObj)return;
			if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			{
				GOutPut(StrFromResID(IDS_ERROR_WRONG_OBJ));
				return;
			}
			if (((CGeoCurveBase*)pObj)->IsClosed())
			{
				GOutPut(StrFromResID(IDS_ERROR_WRONG_OBJ));
				return;
			}

			const CShapeLine *pSL = ((CGeoCurveBase*)pObj)->GetShape();

			double allLen = pSL->GetLength();
			double curLen = pSL->GetLength(&pt, TRUE);

			if (allLen <= 0 || curLen / allLen <= 0.5)m_ptNum0 = 0;
			else m_ptNum0 = pObj->GetDataPointSum() - 1;

			m_nStep = 1;
			GotoState(PROCSTATE_PROCESSING);
		}

	}
	else if (m_nStep == 1)
	{
		if (num > 0)
		{
			if (!CanGetSelObjs(flag, FALSE))return;
			m_pObjOld1 = HandleToFtr(m_pEditor->GetSelection()->GetLastSelectedObj());
			if (m_pObjOld1 != m_pObjOld0)
			{
				pObj = m_pObjOld1->GetGeometry();
				if (!pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					GOutPut(StrFromResID(IDS_ERROR_WRONG_OBJ));
					return;
				}

				if (((CGeoCurveBase*)pObj)->IsClosed())
				{
					GOutPut(StrFromResID(IDS_ERROR_WRONG_OBJ));
					return;
				}

				const CShapeLine *pSL = ((CGeoCurveBase*)pObj)->GetShape();

				double allLen = pSL->GetLength();
				double curLen = pSL->GetLength(&pt, TRUE);

				if (allLen <= 0 || curLen / allLen <= 0.5)m_ptNum1 = 0;
				else m_ptNum1 = pObj->GetDataPointSum() - 1;

				m_nStep = 2;

				if (!m_bLinkCountor)
				{
					m_pEditor->CloseSelector();
				}

				GotoState(PROCSTATE_PROCESSING);
			}
		}
	}

	if (!m_bLinkCountor && m_nStep < 3)
	{
		if (m_nStep == 2)
		{
			m_nStep = 3;
		}

		CEditCommand::PtClick(pt, flag);
		return;
	}
	if (m_nStep >= 2)
	{
		CFtrLayer* pLayer0 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(m_pObjOld0);
		CFtrLayer* pLayer1 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(m_pObjOld1);
		if (!pLayer0 || !pLayer1)
		{
			return;
		}
		if (pLayer1 != pLayer0&& strcmp(pLayer1->GetName(), pLayer0->GetName()) != 0)
		{
			if (AfxMessageBox(IDS_ERR_INCONSISTENT_FID, MB_YESNO) != IDYES)
			{
				Abort();
				return;
			}
		}

		if (m_bMatchHeight)
		{
			CGeometry *pObj0 = m_pObjOld0->GetGeometry();
			CGeometry *pObj1 = m_pObjOld1->GetGeometry();
			PT_3DEX t1, t2;
			if (m_ptNum0 == 0)
				t1 = pObj0->GetDataPoint(pObj0->GetDataPointSum() - 1);
			else
				t1 = pObj0->GetDataPoint(0);
			if (m_ptNum1 == 0)
				t2 = pObj1->GetDataPoint(0);
			else
				t2 = pObj1->GetDataPoint(pObj1->GetDataPointSum() - 1);
			if (fabs(t1.z - t2.z) >= 1e-4)
			{
				//	OutputTipString(StrFromResID(IDS_ERR_INCONSISTENT_HEIGHT));
				Abort();
				return;
			}
		}

		m_pObjNew = m_pObjOld0->Clone();

		m_pObjNew->SetID(OUID());

		CGeometry *pNew = m_pObjNew->GetGeometry();
		CArray<PT_3DEX, PT_3DEX> pts;
		PT_3DEX t0, t, pt1, pt2;
		int i, num, first = 1;
		CArray<PT_3DEX, PT_3DEX> arr;
		pNew->GetShape(arr);
		num = arr.GetSize();

		if (m_ptNum0 == 0)
		{
			for (i = num - 1; i >= 0; i--)
			{
				t = arr[i];
				pts.Add(t);
			}

			pt1 = arr[0];
		}
		else
		{
			for (i = 0; i < num; i++)
			{
				t = arr[i];
				pts.Add(t);
			}

			pt1 = arr[num - 1];
		}
		pNew = m_pObjOld1->GetGeometry();
		pNew->GetShape(arr);
		num = arr.GetSize();
		if (m_ptNum1 == 0)
		{
			for (i = 0; i < num; i++)
			{
				t = arr[i];
				pts.Add(t);
			}

			pt2 = arr[0];
		}
		else
		{
			for (i = num - 1; i >= 0; i--)
			{
				t = arr[i];
				pts.Add(t);
			}

			pt2 = arr[num - 1];
		}

		KickoffSame2DPoints_notPenMove(pts);

		CUndoFtrs undo(m_pEditor, Name());

		m_pObjNew->GetGeometry()->CreateShape(pts.GetData(), pts.GetSize());

		CFeature *pLastFtr = NULL;

		if (m_bCombinBreak)
		{
			CGeometry *pObj1 = NULL, *pObj2 = NULL, *pObj3 = NULL;
			((CGeoCurveBase*)m_pObjNew->GetGeometry())->GetBreak(pt1, pt2, pObj1, pObj2, pObj3);

			if (pObj1)
			{
				CFeature *pNewFtr = m_pObjNew->Clone();
				pNewFtr->SetGeometry(pObj1);
				if (!m_pEditor->AddObject(pNewFtr, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(m_pObjOld0))))
				{
					delete pNewFtr;
					Abort();
					return;
				}

				GETXDS(m_pEditor)->CopyXAttributes(m_pObjOld0, pNewFtr);
				undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
			}

			if (pObj2)
			{
				CFeature *pNewFtr = m_pObjNew->Clone();
				pNewFtr->SetGeometry(pObj2);
				if (!m_pEditor->AddObject(pNewFtr, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(m_pObjOld0))))
				{
					delete pNewFtr;
					Abort();
					return;
				}

				GETXDS(m_pEditor)->CopyXAttributes(m_pObjOld0, pNewFtr);
				undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
			}

			if (pObj3)
			{
				CFeature *pNewFtr = m_pObjNew->Clone();
				pNewFtr->SetGeometry(pObj3);
				if (!m_pEditor->AddObject(pNewFtr, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(m_pObjOld0))))
				{
					delete pNewFtr;
					Abort();
					return;
				}

				GETXDS(m_pEditor)->CopyXAttributes(m_pObjOld0, pNewFtr);
				undo.arrNewHandles.Add(FtrToHandle(pNewFtr));

				pLastFtr = pNewFtr;
			}

			delete m_pObjNew;
			m_pObjNew = NULL;

		}
		else
		{
			if (!m_pEditor->AddObject(m_pObjNew, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(m_pObjOld0))))
			{
				delete m_pObjNew;
				Abort();
				return;
			}

			GETXDS(m_pEditor)->CopyXAttributes(m_pObjOld0, m_pObjNew);
			undo.arrNewHandles.Add(FtrToHandle(m_pObjNew));

			pLastFtr = m_pObjNew;
		}

		undo.arrOldHandles.Add(FtrToHandle(m_pObjOld0));
		undo.arrOldHandles.Add(FtrToHandle(m_pObjOld1));

		m_pEditor->DeleteObject(FtrToHandle(m_pObjOld0));
		m_pEditor->DeleteObject(FtrToHandle(m_pObjOld1));

		undo.Commit();

		m_pEditor->DeselectAll();
		if (m_bLinkCountor)
		{
			m_pEditor->GetSelection()->SelectObj(FtrToHandle(pLastFtr));
		}
		m_pEditor->OnSelectChanged();

		if (m_bLinkCountor)
		{
			m_pObjOld0 = pLastFtr;
			m_nStep = 1;
		}
		else
		{
			m_pObjOld0 = NULL;
			Finish();
			m_nStep = 3;
		}

		m_pObjNew = NULL;
		m_pObjOld1 = NULL;


	}

	CEditCommand::PtClick(pt, flag);
}

void CCombineCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (bool)(m_bMatchHeight);
	tab.AddValue(PF_COMBINEHEIMATCH, &CVariantEx(var));

	var = (bool)(m_bLinkCountor);
	tab.AddValue(PF_COMBINEMODE, &CVariantEx(var));

	var = (bool)(m_bCombinBreak);
	tab.AddValue(PF_COMBINEBREAK, &CVariantEx(var));


}

void CCombineCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("CombineCommand", StrFromLocalResID(IDS_CMDNAME_COMBINE));
	param->AddParam(PF_COMBINEHEIMATCH, (bool)m_bMatchHeight, StrFromResID(IDS_CMDPLANE_MATCHHEIGHT));
	param->AddParam(PF_COMBINEMODE, (bool)m_bLinkCountor, StrFromResID(IDS_CMDPLANE_LINKCONTOUR));
	param->AddParam(PF_COMBINEBREAK, (bool)m_bCombinBreak, StrFromResID(IDS_CMDPLANE_COMBINBREAK));
}


void CCombineCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	//	bool bNeedRefresh = false;

	if (tab.GetValue(0, PF_COMBINEHEIMATCH, var))
	{
		m_bMatchHeight = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, PF_COMBINEMODE, var))
	{
		m_bLinkCountor = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
		if (!bInit)
			Abort();
	}

	if (tab.GetValue(0, PF_COMBINEBREAK, var))
	{
		m_bCombinBreak = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab, bInit);
}


//////////////////////////////////////////////////////////////////////
// CParallelMoveCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CParallelMoveCommand, CEditCommand)

CParallelMoveCommand::CParallelMoveCommand()
{
	m_nStep = -1;
	m_nMode = modeAllSides;
	m_pFtr = NULL;
	m_lfWidth = 0.1;
	m_bMouseWidth = FALSE;
	m_bKeepOld = FALSE;
	m_bMouseModifyZ = FALSE;
	m_lfDeltaZ = 0;
	m_bFixIntersect = FALSE;
	strcat(m_strRegPath, "\\EavesAdjust");
}

CParallelMoveCommand::~CParallelMoveCommand()
{

}



CString CParallelMoveCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_PARALLELMOVE);
}

void CParallelMoveCommand::Start()
{
	if (!m_pEditor)return;

	m_nStep = -1;
	m_nMode = modeAllSides;
	m_pFtr = NULL;
	m_lfWidth = 0.1;
	m_bMouseWidth = TRUE;
	m_bKeepOld = FALSE;
	m_bMouseModifyZ = TRUE;
	m_lfDeltaZ = 0;
	m_bFixIntersect = FALSE;
	CEditCommand::Start();
	m_pEditor->OpenSelector(SELMODE_SINGLE);
}

void CParallelMoveCommand::Abort()
{
	m_pFtr = NULL;

	CEditCommand::Abort();
}


CGeometry *CParallelMoveCommand::GetOneParaObj(CGeometry *pObj, double wid, BOOL bUsedir, BOOL bModifyZ, double dz, GrBuffer *pBuf)
{
	if (!pObj || fabs(wid) < GraphAPI::GetDisTolerance())return NULL;
	if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		return NULL;

	CGeoCurveBase *pObj2 = (CGeoCurveBase*)pObj;

	PT_3DEX t, expt;
	PT_3D line[2], line2[2];
	PT_3DEX *pts;

	CArray<PT_3DEX, PT_3DEX> arr;
	m_pFtr->GetGeometry()->GetShape(arr);

	int num = arr.GetSize();
	if (num < 2)return NULL;

	t = arr[0];
	expt = arr[num - 1];

	BOOL bClosed = FALSE;
	if (pObj2->IsClosed())
	{
		bClosed = TRUE;
	}

	//得到选中的线段
	t = arr[m_nPtNum];
	COPY_3DPT(line[0], t);
	t = arr[m_nPtNum + 1];
	COPY_3DPT(line[1], t);

	CFeature *pNewFtr = NULL;
	if (!m_strLayer.IsEmpty())
	{
		pNewFtr = PDOC(m_pEditor)->GetDlgDataSource()->CreateObjByNameOrLayerIdx(m_strLayer);
		if (pNewFtr != NULL && !pNewFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		{
			delete pNewFtr;
			pNewFtr = NULL;
		}
		else if (pNewFtr != NULL)
		{
			pNewFtr->GetGeometry()->CreateShape(arr.GetData(), arr.GetSize());
		}
	}
	if (pNewFtr != NULL)
		pObj = pNewFtr->GetGeometry();
	else
		pObj = pObj->Clone();

	if (!pObj)return NULL;

	pts = new PT_3DEX[num];
	if (!pts)
	{
		delete pObj;
		return NULL;
	}

	//取点
	for (int j = 0; j < num; j++)
	{
		pts[j] = arr[j];
	}

	//计算方向，保证宽度的正值对应外扩，负值对应内缩
	bool bClockwise = (GraphAPI::GIsClockwise(pts, num) == 1);
	if (bUsedir && !bClockwise)wid = -wid;

	delete[] pts;

	//计算选中线段的扩展线
	if (!GraphAPI::GGetParallelLine(line, 2, wid, line2))
	{
		delete pObj;
		return NULL;
	}

	GrBuffer buf;
	buf.BeginLineString(0, 0);

	//求第一个交点
	if (m_nPtNum > 0 || (bClosed&&m_nPtNum <= 0))
	{
		if (m_nPtNum <= 0)
			t = arr[num - 2];
		else
			t = arr[m_nPtNum - 1];

		buf.MoveTo(&t);

		COPY_3DPT(line[0], t);
		t = arr[m_nPtNum];
		COPY_3DPT(line[1], t);

		expt = arr[m_nPtNum];

		GraphAPI::GGetLineIntersectLine(line[0].x, line[0].y, line[1].x - line[0].x, line[1].y - line[0].y,
			line2[0].x, line2[0].y, line2[1].x - line2[0].x, line2[1].y - line2[0].y, &expt.x, &expt.y, NULL);

		if (bModifyZ)expt.z += dz;

		arr[m_nPtNum] = expt;
		if (m_nPtNum <= 0)
			arr[num - 1] = expt;

		buf.LineTo(&expt);
	}
	else
	{
		expt = arr[m_nPtNum];
		COPY_2DPT(expt, line2[0]);

		if (bModifyZ)expt.z += dz;
		arr[m_nPtNum] = expt;

		buf.MoveTo(&expt);
	}

	//求第二个交点
	if (m_nPtNum < (num - 2) || (bClosed&&m_nPtNum == (num - 2)))
	{
		PT_3DEX tmpPt = arr[m_nPtNum + 1];
		t = arr[m_nPtNum + 1];
		COPY_3DPT(line[0], t);
		if (m_nPtNum == num - 2)
			t = arr[1];
		else
			t = arr[m_nPtNum + 2];
		COPY_3DPT(line[1], t);

		expt = arr[m_nPtNum + 1];

		GraphAPI::GGetLineIntersectLine(line[0].x, line[0].y, line[1].x - line[0].x, line[1].y - line[0].y,
			line2[0].x, line2[0].y, line2[1].x - line2[0].x, line2[1].y - line2[0].y, &expt.x, &expt.y, NULL);

		if (bModifyZ)expt.z += dz;
		arr[m_nPtNum + 1] = expt;
		if (m_nPtNum == num - 2)
			arr[0] = expt;

		buf.LineTo(&expt);

		buf.LineTo(&tmpPt);

		buf.End();
	}
	else if (m_nPtNum < (num - 1))
	{
		expt = arr[m_nPtNum + 1];
		COPY_2DPT(expt, line2[1]);

		if (bModifyZ)expt.z += dz;
		arr[m_nPtNum + 1] = expt;

		buf.LineTo(&expt);
		buf.End();
	}

	pObj->CreateShape(arr.GetData(), arr.GetSize());

	if (pBuf)
	{
		pBuf->CopyFrom(&buf);
	}

	return pObj;
}

static void KickoffSamePts(PT_3D *pts, int& num, double tolerance = 1e-4)
{
	if (num <= 1)return;

	int pos = 0;
	PT_3D *pt = pts + 1;
	for (int i = 1; i < num; i++, pt++)
	{
		if (_FABS(pts[pos].x - pt->x) < tolerance &&
			_FABS(pts[pos].y - pt->y) < tolerance &&
			_FABS(pts[pos].z - pt->z) < tolerance)
			continue;

		if (i != (pos + 1))pts[pos + 1] = *pt;
		pos++;
	}

	num = (pos + 1);
}


static void CutSelfIntersect(PT_3D *pts, int& num)
{
	KickoffSamePts(pts, num);

	BOOL bClosed, bClosed0;

	bClosed0 = FALSE;
	if (pts[0].x == pts[num - 1].x && pts[0].y == pts[num - 1].y)
		bClosed0 = TRUE;

	CLinesSearch ls;
	int *delflags = new int[num];
	if (!delflags)return;

	PT_3D *line0, *line1, ptRet;
	Envelope e0, e1;
	int i, j, k, n;
	int nfind, *pidx, ndel = 0, num0 = num;

	while (1)
	{
	_RESTART:
		memset(delflags, 0, sizeof(int)*num);

		ls.Init(pts, num);

		ndel = 0, num0 = num;

		bClosed = FALSE;
		if (pts[0].x == pts[num - 1].x && pts[0].y == pts[num - 1].y)
			bClosed = TRUE;

		for (i = 0; i < num - 1; i++)
		{
			line0 = pts + i;

			e0.CreateFromPts(line0, 2);

			pidx = ls.FindNearLines(line0[0], line0[1], nfind);

			//判断当前点 i 到 num-1 点的自相交情况
			for (k = 0; k < nfind; k++)
			{
				j = pidx[k];
				if (delflags[j] == 1)continue;

				if (j <= (i + ndel) || j == (i + ndel + 1) || (bClosed && (j + 2 - num0) == (i + ndel)) ||
					(!bClosed && (j + 1 - num0) == (i + ndel)))continue;
				j = j - ndel;

				line1 = pts + j;

				e1.CreateFromPts(line1, 2);

				if (!e0.bIntersect(&e1))continue;
				if (!GraphAPI::GGetLineIntersectLineSeg(line0[0].x, line0[0].y,
					line0[1].x, line0[1].y, line1[0].x, line1[0].y,
					line1[1].x, line1[1].y, &ptRet.x, &ptRet.y, NULL))
					continue;

				GraphAPI::GGetPtZOfLine(line0[0].x, line0[0].y, line0[0].z,
					line0[1].x, line0[1].y, line0[1].z, &ptRet.x, &ptRet.y, &ptRet.z);

				//去除中间点，用交点置换下一点
				if ((j - i) <= num / 2)
				{
					memmove(pts + i + 2, pts + j + 1, sizeof(PT_3D)*(num - j - 1));
					for (n = j - i - 1; n >= 0; n--)
						delflags[pidx[k] - n] = 1;

					pts[i + 1] = ptRet;
					num -= (j - i - 1);
					ndel += (j - i - 1);

					i = i + 1;
					break;
				}
				else
				{
					memmove(pts + 1, pts + i + 1, sizeof(PT_3D)*(j - i));
					pts[0] = ptRet;
					pts[j - i + 1] = ptRet;
					num = j - i + 1;
					goto _RESTART;
				}
			}
		}

		if (num == num0)break;
	}

	delete[] delflags;

	bClosed = FALSE;
	if (pts[0].x == pts[num - 1].x && pts[0].y == pts[num - 1].y)
		bClosed = TRUE;

	if (bClosed0 && !bClosed)
	{
		pts[num] = pts[0];
		num++;
	}

	KickoffSamePts(pts, num);
}

//根据已知对象，得到生成的全平行对象
CGeometry *CParallelMoveCommand::GetAllParaObj(CGeometry *pObj, double wid, BOOL bUsedir, BOOL bModifyZ, double dz)
{
	if (!pObj)return NULL;
	PT_3D  *pts, *ptsRet;
	int num;
	CArray<PT_3DEX, PT_3DEX> arrPts;
	//准备
	pObj->GetShape(arrPts);
	num = arrPts.GetSize();
	if (num < 2)return NULL;

	PT_3DEX t, expt;

	pts = new PT_3D[num];
	if (!pts)return NULL;

	ptsRet = new PT_3D[num];
	if (!ptsRet)
	{
		delete[] pts;
		return NULL;
	}

	//取点
	for (int j = 0; j<num; j++)
	{
		expt = arrPts.GetAt(j);
		COPY_3DPT(pts[j], expt);
	}

	//计算方向，保证宽度的正值对应外扩，负值对应内缩
	bool bClockwise = (GraphAPI::GIsClockwise(pts, num) == 1);
	if (bUsedir && !bClockwise)wid = -wid;

	//计算选中线段的扩展线
	if (fabs(wid)>GraphAPI::GetDisTolerance() && GraphAPI::GGetParallelLine(pts, num, wid, ptsRet))
	{
		//去除自相交的节点
		if (m_bFixIntersect)CutSelfIntersect(ptsRet, num);

		//生成新的对象
		CFeature *pNewFtr = NULL;
		if (!m_strLayer.IsEmpty())
		{
			pNewFtr = PDOC(m_pEditor)->GetDlgDataSource()->CreateObjByNameOrLayerIdx(m_strLayer);
			if (pNewFtr != NULL && !(pNewFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pNewFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))))
			{
				delete pNewFtr;
				pNewFtr = NULL;
			}
			else if (pNewFtr != NULL)
			{
				PT_3DEX expt;
				CArray<PT_3DEX, PT_3DEX> arrpts;
				pObj->GetShape(arrpts);
				pNewFtr->GetGeometry()->CreateShape(arrpts.GetData(), arrpts.GetSize());
			}
		}
		if (pNewFtr != NULL)
			pObj = pNewFtr->GetGeometry();
		else
			pObj = pObj->Clone();
		if (pObj)
		{
			for (j = 0; j < num; j++)
			{
				expt = arrPts.GetAt(j);
				COPY_2DPT(expt, ptsRet[j]);

				if (bModifyZ)expt.z += dz;
				arrPts.SetAt(j, expt);
			}

			pObj->CreateShape(arrPts.GetData(), arrPts.GetSize());
		}
	}
	else
	{
		pObj = NULL;
	}

	delete[] pts;
	delete[] ptsRet;

	return pObj;
}

void CParallelMoveCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor)return;

	//单边模式
	if (m_nMode == modeOneSide)
	{
		if (m_nStep == 0)
		{
			int num;
			const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
			if (num > 0)
			{
				if (!CanGetSelObjs(flag, FALSE))
				{
					m_ptStart = pt;
					return;
				}

				m_pFtr = HandleToFtr(m_pEditor->GetSelection()->GetLastSelectedObj());
				CGeometry *pObj = NULL;
				if (m_pFtr)pObj = m_pFtr->GetGeometry();
				if (!pObj)return;
				//	m_idsOld.Add(objID);

				double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();

				PT_3D pt1;
				//得到最近的基线段
				m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt, &pt1);
				Envelope e;
				e.CreateFromPtAndRadius(pt1, r);
				if (!pObj->FindNearestBaseLine(pt1,
					e, m_pEditor->GetCoordWnd().m_pSearchCS, &m_ptLine0, &m_ptLine1, NULL))
					return;
				const CShapeLine *pSL = NULL;
				if (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				{
					pSL = ((CGeoCurve*)pObj)->GetShape();
				}
				else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					pSL = ((CGeoSurface*)pObj)->GetShape();
				}
				else
					return;
				m_ptLine1.x = (m_ptLine1.x + m_ptLine0.x) / 2;
				m_ptLine1.y = (m_ptLine1.y + m_ptLine0.y) / 2;
				m_ptLine1.z = (m_ptLine1.z + m_ptLine0.z) / 2;

				PT_3D pt2;
				m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&m_ptLine1, &pt2);
				e.CreateMaxEnvelope();
				pSL->FindNearestLine(pt2, e, m_pEditor->GetCoordWnd().m_pSearchCS, NULL, NULL, NULL, &m_nPtNum);
				if (m_nPtNum < 0 || (!pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) && m_nPtNum >= pObj->GetDataPointSum() - 1))
					return;

				m_ptStart = pt;
				EditStepOne();
			}
		}
		else if ((!m_bMouseWidth&&m_nStep == 1) || (m_bMouseWidth&&m_nStep >= 1))
		{
			if (!m_pFtr)return;

			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();

			double wid = m_lfWidth;
			//如果采用鼠标定义宽度，就用计算出该宽度，正值向左，负值向右
			if (m_bMouseWidth)
			{
				wid = GraphAPI::GGetNearestDisOfPtToLine(m_ptLine0.x, m_ptLine0.y, m_ptLine1.x, m_ptLine1.y, pt.x, pt.y);

				PT_3D pts[3];
				pts[0] = m_ptLine0; pts[1] = m_ptLine1; pts[2] = pt;
				if (GraphAPI::GIsClockwise(pts, 3) == 1)wid = -wid;
			}
			else
			{
				wid = fabs(m_lfWidth);
				PT_3D pts[3];
				pts[0] = m_ptLine0; pts[1] = m_ptLine1; pts[2] = pt;
				if (GraphAPI::GIsClockwise(pts, 3) == 1)wid = -wid;
			}
			CGeometry *pObj = m_pFtr->GetGeometry();
			pObj = GetOneParaObj(pObj, wid, FALSE, TRUE, m_bMouseModifyZ ? (pt.z - m_ptStart.z) : m_lfDeltaZ);

			if (pObj)
			{
				//	long layhdl = m_idsOld[0].layhdl;
				CFeature *pFtr = m_pFtr->Clone();
				if (pFtr)
				{
					pFtr->SetGeometry(pObj);
				}
				else
					return;

				CUndoFtrs undo(m_pEditor, Name());
				CValueTable tab;
				tab.BeginAddValueItem();
				GETXDS(m_pEditor)->GetXAttributes(m_pFtr, tab);
				tab.EndAddValueItem();
				// 得到新添加的地物的层的ID
				int LayerID = m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(m_pFtr));
				int newLayerID;
				if (m_strLayer.IsEmpty())
				{
					newLayerID = LayerID;
				}
				else
				{
					//					CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(m_strLayer);
					CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(m_strLayer);
					if (pLayer)
					{
						newLayerID = pLayer->GetID();
					}
					else
						newLayerID = LayerID;
				}
				// 			
				pFtr->SetID(OUID());
				m_pEditor->AddObject(pFtr, newLayerID);
				if (newLayerID == LayerID)
				{
					GETXDS(m_pEditor)->SetXAttributes(pFtr, tab);
				}

				//删除原来的对象，加入新的对象
				if (!m_bKeepOld)//这个地方修改	
				{
					m_pEditor->DeleteObject(FtrToHandle(m_pFtr));
					undo.arrOldHandles.Add(FtrToHandle(m_pFtr));
				}

				undo.arrNewHandles.Add(FtrToHandle(pFtr));
				undo.Commit();
				//	m_idsNew.Add(m_pDoc->AddObject(m_pObj,layhdl));
			}

			m_pFtr = NULL;
			m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pEditor->RefreshView();
			Finish();
			m_nStep = 2;
		}
	}
	//整体模式
	else if (m_nMode == modeAllSides)
	{
		if (m_nStep == 0)
		{
			int num;
			const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
			if (num > 0)
			{
				if (num > 1 && m_bMouseWidth)
				{
					//	OutputTipString(CMDERR_TOOMANYOBJS);
					return;
				}
				if (m_bMouseWidth)
				{
					if (!CanGetSelObjs(flag, FALSE))
					{
						m_ptStart = pt;
						EditStepOne();
						return;
					}
				}
				else if (!CanGetSelObjs(flag))
				{
					m_ptStart = pt;
					EditStepOne();
					return;
				}

				if (m_bMouseWidth)
				{
					double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
					PT_3D pt1;

					m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt, &pt1);
					Envelope e;
					e.CreateFromPtAndRadius(pt1, r);

					CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
					if (!pObj->FindNearestBaseLine(pt1,
						e, m_pEditor->GetCoordWnd().m_pSearchCS, &m_ptLine0, &m_ptLine1, NULL))
						return;
				}

				m_ptStart = pt;
				EditStepOne();
			}
		}

		if ((!m_bMouseWidth && (m_nStep == 1 || m_nStep == 2)) || (m_bMouseWidth&&m_nStep == 2))
		{

			CGeometry *pObj;
			int num;
			const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);

			//如果采用鼠标定义宽度，就用计算出该宽度
			double wid = m_lfWidth;
			if (m_bMouseWidth)
			{
				wid = GraphAPI::GGetNearestDisOfPtToLine(m_ptLine0.x, m_ptLine0.y, m_ptLine1.x, m_ptLine1.y, pt.x, pt.y);

				PT_3D pts[3];
				pts[0] = m_ptLine0; pts[1] = m_ptLine1; pts[2] = pt;
				if (GraphAPI::GIsClockwise(pts, 3) == 1)wid = -wid;
			}
			//如果使用输入的宽度，就用鼠标的坐标确定正负方向，正值向左，负值向右
			else if (num == 1)
			{
				double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
				PT_3D pt3d;
				m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt, &pt3d);
				Envelope e;
				e.CreateFromPtAndRadius(pt3d, 1000);

				CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
				if (pObj&& pObj->FindNearestBaseLine(pt3d,
					e, m_pEditor->GetCoordWnd().m_pSearchCS, &m_ptLine0, &m_ptLine1, NULL))
				{
					wid = fabs(m_lfWidth);

					PT_3D pts[3];
					pts[0] = m_ptLine0; pts[1] = m_ptLine1; pts[2] = pt;
					if (GraphAPI::GIsClockwise(pts, 3) == 1)wid = -wid;
				}
			}
			for (int i = num - 1; i >= 0; i--)
			{
				pObj = HandleToFtr(handles[i])->GetGeometry();
				if (pObj)
				{
					pObj = GetAllParaObj(pObj, wid, FALSE, TRUE, m_bMouseModifyZ ? (pt.z - m_ptStart.z) : m_lfDeltaZ);

					if (pObj)
					{
						CValueTable tab;
						tab.BeginAddValueItem();
						GETXDS(m_pEditor)->GetXAttributes(HandleToFtr(handles[i]), tab);
						tab.EndAddValueItem();

						CFeature *pFtr = HandleToFtr(handles[i])->Clone();
						if (pFtr)
						{
							pFtr->SetGeometry(pObj);
						}
						else
							return;

						CUndoFtrs undo(m_pEditor, Name());
						// 得到新添加的地物的层的ID
						int newLayerID;
						BOOL bCopyXAttributes = TRUE;
						if (m_strLayer.IsEmpty())
						{
							newLayerID = m_pEditor->GetFtrLayerIDOfFtr(handles[i]);
						}
						else
						{
							//							CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(m_strLayer);
							CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(m_strLayer);
							if (pLayer)
							{
								newLayerID = pLayer->GetID();
								bCopyXAttributes = FALSE;
							}
							else
								newLayerID = m_pEditor->GetFtrLayerIDOfFtr(handles[i]);
						}
						//删除原来的对象
						if (!m_bKeepOld)
						{
							m_pEditor->DeleteObject(handles[i]);
							undo.arrOldHandles.Add(handles[i]);
						}

						pFtr->SetID(OUID());
						m_pEditor->AddObject(pFtr, newLayerID);
						if (bCopyXAttributes)
						{
							GETXDS(m_pEditor)->SetXAttributes(pFtr, tab);
						}

						undo.arrNewHandles.Add(FtrToHandle(pFtr));
						undo.Commit();
					}
				}
			}
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pEditor->RefreshView();
			Finish();
			m_nStep = 2;
		}
	}
	CEditCommand::PtClick(pt, flag);
}

void CParallelMoveCommand::PtMove(PT_3D &pt)
{
	//	if( m_bMouseWidth )
	{
		if (m_nMode == modeOneSide && ((m_bMouseWidth && (m_nStep == 1 || m_nStep == 2)) || (!m_bMouseWidth && m_nStep == 1)))
		{
			if (!m_pFtr)return;

			double wid = m_lfWidth;
			//如果采用鼠标定义宽度，就用计算出该宽度，正值向左，负值向右
			if (m_bMouseWidth)
			{
				wid = GraphAPI::GGetNearestDisOfPtToLine(m_ptLine0.x, m_ptLine0.y, m_ptLine1.x, m_ptLine1.y, pt.x, pt.y);

				PT_3D pts[3];
				pts[0] = m_ptLine0; pts[1] = m_ptLine1; pts[2] = pt;
				if (GraphAPI::GIsClockwise(pts, 3) == 1)wid = -wid;
			}
			else
			{
				wid = fabs(m_lfWidth);
				PT_3D pts[3];
				pts[0] = m_ptLine0; pts[1] = m_ptLine1; pts[2] = pt;
				if (GraphAPI::GIsClockwise(pts, 3) == 1)wid = -wid;
			}
			CGeometry *pObj = m_pFtr->GetGeometry();
			if (!pObj)
			{
				return;
			}

			GrBuffer buf;
			pObj = GetOneParaObj(pObj, wid, FALSE, TRUE, m_bMouseModifyZ ? (pt.z - m_ptStart.z) : m_lfDeltaZ, &buf);
			if (!pObj)return;

			//pObj->Draw(&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
			//delete pObj;
			if (m_bMouseWidth)
			{
				m_nStep = 2;
			}
		}
		else if (m_nMode == modeAllSides && ((m_bMouseWidth && (m_nStep == 1 || m_nStep == 2)) || (!m_bMouseWidth && m_nStep == 1)))
		{
			int num;
			const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
			if (num != 1)return;

			double wid = m_lfWidth;
			if (m_bMouseWidth)
			{
				wid = GraphAPI::GGetNearestDisOfPtToLine(m_ptLine0.x, m_ptLine0.y, m_ptLine1.x, m_ptLine1.y, pt.x, pt.y);

				PT_3D pts[3];
				pts[0] = m_ptLine0; pts[1] = m_ptLine1; pts[2] = pt;
				if (GraphAPI::GIsClockwise(pts, 3) == 1)wid = -wid;
			}
			//如果使用输入的宽度，就用鼠标的坐标确定正负方向，正值向左，负值向右
			else if (num == 1)
			{
				double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
				PT_3D pt3d;
				m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt, &pt3d);
				Envelope e;
				e.CreateFromPtAndRadius(pt3d, 1000);

				CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
				if (pObj&& pObj->FindNearestBaseLine(pt3d,
					e, m_pEditor->GetCoordWnd().m_pSearchCS, &m_ptLine0, &m_ptLine1, NULL))
				{
					wid = fabs(m_lfWidth);

					PT_3D pts[3];
					pts[0] = m_ptLine0; pts[1] = m_ptLine1; pts[2] = pt;
					if (GraphAPI::GIsClockwise(pts, 3) == 1)wid = -wid;
				}
			}

			CGeometry *pObj = HandleToFtr(handles[0])->GetGeometry();
			pObj = GetAllParaObj(pObj, wid, FALSE, TRUE, m_bMouseModifyZ ? (pt.z - m_ptStart.z) : m_lfDeltaZ);
			if (!pObj)return;
			GrBuffer buf;
			pObj->Draw(&buf, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
			delete pObj;

			if (m_bMouseWidth)
			{
				m_nStep = 2;
			}

		}
	}

}

void CParallelMoveCommand::GetParams(CValueTable &tab)
{

	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nMode);
	tab.AddValue(PF_EAVESADJUSTWAY, &CVariantEx(var));
	var = (bool)(m_bKeepOld);
	tab.AddValue(PF_EAVESADJUSTKEEPOLD, &CVariantEx(var));
	var = (bool)(m_bMouseWidth);
	tab.AddValue(PF_EAVESADJUSTMWIDTH, &CVariantEx(var));
	var = (double)(m_lfWidth);
	tab.AddValue(PF_EAVESADJUSTWIDTH, &CVariantEx(var));
	var = (double)(m_lfDeltaZ);
	tab.AddValue(PF_EAVESADJUSTHEIGHT, &CVariantEx(var));
	var = (bool)(m_bMouseModifyZ);
	tab.AddValue(PF_EAVESADJUSTMHEIGHT, &CVariantEx(var));
	var = (bool)(m_bFixIntersect);
	tab.AddValue(PF_EAVESADJUSTFIXINTERSECT, &CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)(m_strLayer);
	tab.AddValue(PF_EAVESADJUSTNEWLAYERCODE, &CVariantEx(var));

}

void CParallelMoveCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("EavesAdjustCommand", StrFromLocalResID(IDS_CMDNAME_PARALLELMOVE));
	param->BeginOptionParam(PF_EAVESADJUSTWAY, StrFromResID(IDS_CMDPLANE_WAY));
	param->AddOption(StrFromResID(IDS_CMDPLANE_WHOLE), 0, ' ', m_nMode == modeAllSides);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SINGLESIDE), 1, ' ', m_nMode == modeOneSide);
	param->EndOptionParam();
	param->AddParam(PF_EAVESADJUSTKEEPOLD, bool(m_bKeepOld), StrFromResID(IDS_CMDPLANE_KEEPOLD));
	param->AddParam(PF_EAVESADJUSTMWIDTH, bool(m_bMouseWidth), StrFromResID(IDS_CMDPLANE_MOUSEDEF_WID));
	if (bForLoad || !m_bMouseWidth)param->AddParam(PF_EAVESADJUSTWIDTH, m_lfWidth, StrFromResID(IDS_CMDPLANE_WIDTH));
	param->AddParam(PF_EAVESADJUSTMHEIGHT, bool(m_bMouseModifyZ), StrFromResID(IDS_CMDPLANE_MOUSEDEF_DH));
	if (bForLoad || !m_bMouseModifyZ)param->AddParam(PF_EAVESADJUSTHEIGHT, m_lfDeltaZ, StrFromResID(IDS_CMDPLANE_DHEIGHT));

	if (m_nMode == modeAllSides || bForLoad)param->AddParam(PF_EAVESADJUSTFIXINTERSECT, bool(m_bFixIntersect), StrFromResID(IDS_CMDPLANE_FIXINTERSECT));
	param->AddLayerNameParam(PF_EAVESADJUSTNEWLAYERCODE, LPCTSTR(m_strLayer), StrFromResID(IDS_CMDPLANE_NEWLAYERCODE));

}


void CParallelMoveCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	//	bool bNeedRefresh = false;

	if (tab.GetValue(0, PF_EAVESADJUSTWAY, var))
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_EAVESADJUSTKEEPOLD, var))
	{
		m_bKeepOld = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_EAVESADJUSTMWIDTH, var))
	{
		m_bMouseWidth = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_EAVESADJUSTWIDTH, var))
	{
		m_lfWidth = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_EAVESADJUSTHEIGHT, var))
	{
		m_lfDeltaZ = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_EAVESADJUSTMHEIGHT, var))
	{
		m_bMouseModifyZ = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_EAVESADJUSTFIXINTERSECT, var))
	{
		m_bFixIntersect = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_EAVESADJUSTNEWLAYERCODE, var))
	{
		m_strLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab, bInit);
}

//////////////////////////////////////////////////////////////////////
// CEavesAdjustCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CEavesAdjustCommand, CParallelMoveCommand)

CEavesAdjustCommand::CEavesAdjustCommand()
{
	m_bMarkText = TRUE;
	m_bModified = FALSE;
}
CEavesAdjustCommand::~CEavesAdjustCommand()
{

}


BOOL CEavesAdjustCommand::MarkText(PT_3DEX *pts, int num, float wid)
{
	CDlgDataSource *pDS = GETDS(m_pEditor);
	if (!pDS) return FALSE;

	CString strLayerName = m_strLayerCode;
	CFtrLayer *pFtrLay = pDS->GetFtrLayer(strLayerName);

	if (pFtrLay == NULL)
	{
		pFtrLay = pDS->CreateFtrLayer(strLayerName);
		if (!pFtrLay)  return FALSE;
		pFtrLay->SetColor(RGB(255, 0, 0));
		pDS->AddFtrLayer(pFtrLay);
	}

	CUndoFtrs undo(m_pEditor, Name());

	if (m_bMarkText)
	{
		CFeature *pTextFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(), CLS_GEOTEXT);
		if (pTextFtr)
		{
			CGeoText *pText = (CGeoText*)pTextFtr->GetGeometry();
			if (pText)
			{
				pText->SetColor(RGB(255, 255, 255));

				pText->CreateShape(pts, num);

				// 				CString str;
				// 				str.Format("%.4f",wid);
				// 				TrimRZero(str);
				CString strFormat, str;
				strFormat.Format(_T("%%.%df"), m_nDigitNum);
				str.Format(strFormat, wid);

				pText->SetText(str);

				m_sTxtSettings.nAlignment = TAH_MID | TAV_MID;
				m_sTxtSettings.nPlaceType = byLineH;
				pText->SetSettings(&m_sTxtSettings);

				int layid = pFtrLay->GetID();
				m_pEditor->AddObject(pTextFtr, layid);

				undo.AddNewFeature(FtrToHandle(pTextFtr));
			}
		}
	}

	undo.Commit();

	return TRUE;
}


void CEavesAdjustCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor)return;

	if (m_nStep == 0)
	{
		int num;
		const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num > 0)
		{
			if (!CanGetSelObjs(flag, FALSE))return;

			m_pFtr = m_pSelFtr = HandleToFtr(m_pEditor->GetSelection()->GetLastSelectedObj());
			CGeometry *pObj = NULL;
			if (m_pFtr)pObj = m_pFtr->GetGeometry();
			if (!pObj)return;

			if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				return;
			}

			m_ptStart = pt;

			m_nPtNum = 0;

			EditStepOne();
		}
	}

	if (m_nStep >= 1)
	{
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		if (!m_pFtr)
		{
			Abort();
			return;
		}

		CArray<PT_3DEX, PT_3DEX> pts;
		m_pFtr->GetGeometry()->GetShape(pts);
		int num = pts.GetSize();

		// 		BOOL bSurface = FALSE;
		// 		if (m_pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		// 		{
		// 			bSurface = TRUE;
		// 			pts.Add(pts[0]);
		// 			num++;
		// 		}

		GOutPut(StrFromResID(IDS_CMDTIP_INPUTOFF));

		double wid = m_lfWidth;
		while (m_nPtNum >= 0 && m_nPtNum <= num - 2)
		{
			PT_3D midPt;

			CDlgInputData dlg;
			dlg.SetInputData(wid);
			CBaseView *pView = (CBaseView*)GetActiveView();
			if (pView)
			{
				midPt.x = (pts[m_nPtNum].x + pts[m_nPtNum + 1].x) / 2;
				midPt.y = (pts[m_nPtNum].y + pts[m_nPtNum + 1].y) / 2;
				midPt.z = (pts[m_nPtNum].z + pts[m_nPtNum + 1].z) / 2;
				//new code
				PDOC(m_pEditor)->UpdateAllViews(NULL, hc_SetCrossPos, (CObject*)&midPt);
				//new code
				PT_4D pt4d;
				pView->GroundToClient(&midPt, &pt4d);
				CPoint ptShow(pt4d.x, pt4d.y);
				ClientToScreen(pView->m_hWnd, &ptShow);
				dlg.SetShowPos(ptShow);
			}
			if (dlg.DoModal() == IDCANCEL)
			{
				m_nPtNum++;

				if (m_nPtNum > num - 2)
				{
					m_pFtr = NULL;
					m_nStep = 2;
					break;
				}
				else
				{
					continue;
				}
			}

			//new code
			m_bModified = TRUE;
			//new code

			wid = dlg.GetInputData();
			if (fabs(m_lfWidth - wid) > 1e-4)
			{
				m_lfWidth = wid;
				SetSettingsModifyFlag();
			}

			if (fabs(wid) < 1e-4)
			{
				m_nPtNum++;

				if (m_nPtNum > num - 2)
				{
					m_pFtr = NULL;
					m_nStep = 2;
					break;
				}
				else
				{
					continue;
				}
			}
			CGeometry *pObj = m_pFtr->GetGeometry();
			pObj = GetOneParaObj(pObj, -wid, TRUE, TRUE, m_bMouseModifyZ ? (pt.z - m_ptStart.z) : m_lfDeltaZ);

			if (pObj)
			{
				if (m_bMarkText)
				{
					CArray<PT_3DEX, PT_3DEX> ptsText;
					ptsText.Add(PT_3DEX(midPt, penMove));
					ptsText.Add(pts[m_nPtNum + 1]);
					MarkText(ptsText.GetData(), ptsText.GetSize(), wid);
				}

				CFeature *pFtr = m_pFtr->Clone();
				if (pFtr)
				{
					pFtr->SetGeometry(pObj);
				}
				else
				{
					break;
				}

				CUndoFtrs undo(m_pEditor, Name());

				if (!m_bKeepOld && m_pFtr == m_pSelFtr)
				{
					m_pEditor->DeleteObject(FtrToHandle(m_pFtr));
					undo.AddOldFeature(FTR_HANDLE(m_pFtr));
					/*CDlgDataSource *pDS = GETDS(m_pEditor);
					CString strLayerName = m_strLayerCode;
					CFtrLayer *pFtrLay = pDS->GetFtrLayer(strLayerName);

					if (pFtrLay == NULL)
					{
					pFtrLay = pDS->CreateFtrLayer(strLayerName);
					if (!pFtrLay)
					{
					Abort();
					return;
					}
					pFtrLay->SetColor(RGB(255,0,0));
					pDS->AddFtrLayer(pFtrLay);
					}

					CFeature *pNewFtr = m_pFtr->Clone();
					if (!pNewFtr)
					{
					Abort();
					return;
					}

					pNewFtr->SetID(OUID());
					pNewFtr->GetGeometry()->SetColor(RGB(255,0,0));

					int layid = pFtrLay->GetID();
					m_pEditor->AddObject(pNewFtr,layid);

					undo.AddNewFeature(FtrToHandle(pNewFtr));*/
				}
				else if (m_pFtr != m_pSelFtr)
				{
					m_pEditor->DeleteObject(FtrToHandle(m_pFtr));
					undo.arrOldHandles.Add(FtrToHandle(m_pFtr));
				}

				CValueTable tab;
				tab.BeginAddValueItem();
				GETXDS(m_pEditor)->GetXAttributes(m_pFtr, tab);
				tab.EndAddValueItem();

				// 得到新添加的地物的层的ID
				int newLayerID = m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(m_pFtr));

				pFtr->SetID(OUID());
				m_pEditor->AddObject(pFtr, newLayerID);
				GETXDS(m_pEditor)->SetXAttributes(pFtr, tab);

				undo.arrNewHandles.Add(FtrToHandle(pFtr));
				undo.Commit();

				m_pFtr = pFtr;
				m_nPtNum++;

				if (m_nPtNum > num - 2)
				{
					m_pFtr = NULL;
					m_nStep = 2;
				}
			}
		}

		if (m_nStep == 2)
		{
			//new code
			do
			{
				if (!(m_bModified && m_bKeepOld)) break;

				CDlgDataSource *pDS = GETDS(m_pEditor);
				if (!pDS) break;


				long layer_color = -1;
				CFtrLayer *pFtrLay = NULL;
				CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
				if (pScheme)
				{
					if (!m_ori_LayerCode.IsEmpty() && !m_bMarkText)
					{
						__int64 code = -1;
						CSchemeLayerDefine *p_app_layer = NULL;

						code = _atoi64(m_ori_LayerCode.GetBuffer(m_ori_LayerCode.GetLength())); //先尝试将参数转换成层码，若失败，则说明输入的是层名
						if (0 == code)
							p_app_layer = pScheme->GetLayerDefine(m_ori_LayerCode);
						else
							p_app_layer = pScheme->GetLayerDefine(m_ori_LayerCode, TRUE, code);

						if (p_app_layer)
							layer_color = p_app_layer->GetColor();
						pFtrLay = pDS->GetFtrLayer(m_ori_LayerCode);
					}
					else if (!m_strLayerCode.IsEmpty() && m_bMarkText)
					{
						__int64 code = -1;
						CSchemeLayerDefine *p_app_layer = NULL;

						code = _atoi64(m_strLayerCode.GetBuffer(m_strLayerCode.GetLength()));
						if (0 == code)
							p_app_layer = pScheme->GetLayerDefine(m_strLayerCode);
						else
							p_app_layer = pScheme->GetLayerDefine(m_strLayerCode, TRUE, code);

						if (p_app_layer)
							layer_color = p_app_layer->GetColor();
						pFtrLay = pDS->GetFtrLayer(m_strLayerCode);
					}
				}

				if (NULL == pFtrLay) break;

				CUndoFtrs undo(m_pEditor, "Modify layer of original object");

				int layid = pFtrLay->GetID();
				CFeature* pFtrNEW = m_pSelFtr->Clone();
				if (-1 == layer_color)
					pFtrNEW->GetGeometry()->SetColor(RGB(179, 107, 0));
				else
					pFtrNEW->GetGeometry()->SetColor(layer_color);
				m_pEditor->AddObject(pFtrNEW, layid);
				undo.AddNewFeature(FtrToHandle(pFtrNEW));

				m_pEditor->DeleteObject(FtrToHandle(m_pSelFtr));
				undo.AddOldFeature(FtrToHandle(m_pSelFtr));

				undo.Commit();
			} while (0);
			//new code
			m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pEditor->RefreshView();
			Finish();
		}


	}

	//CParallelMoveCommand::PtClick(pt,flag);
}
void CEavesAdjustCommand::PtMove(PT_3D &pt)
{
	CParallelMoveCommand::PtMove(pt);
}
void CEavesAdjustCommand::Abort()
{
	CParallelMoveCommand::Abort();
}
void CEavesAdjustCommand::Start()
{
	//CParallelMoveCommand::Start();
	if (!m_pEditor)return;

	m_bMarkText = TRUE;
	m_nDigitNum = 2;
	m_strLayerCode = StrFromResID(IDS_EAVES_LAYERNAME);
	m_ori_LayerCode = StrFromResID(IDS_EAVES_LAYERNAME);
	m_bKeepOld = TRUE;

	CEditCommand::Start();

	m_nStep = 0;
	m_nMode = modeOneSide;
	m_pFtr = m_pSelFtr = NULL;
	//m_lfWidth = 0.1;
	m_bMouseWidth = FALSE;

	m_bMouseModifyZ = FALSE;
	m_lfDeltaZ = 0;
	m_bFixIntersect = FALSE;

	m_pEditor->OpenSelector(SELMODE_SINGLE);
}
void CEavesAdjustCommand::GetParams(CValueTable& tab)
{
	CParallelMoveCommand::GetParams(tab);

	_variant_t var;
	var = (_bstr_t)(m_strLayerCode);
	tab.AddValue(PF_CONTOURCODE, &CVariantEx(var));

	var = (_bstr_t)(m_ori_LayerCode);
	tab.AddValue(CEA_CMD_ORI_CODE, &CVariantEx(var));

	var = (long)(m_nDigitNum);
	tab.AddValue(PF_CONTOURDIGITNUM, &CVariantEx(var));
	var = (LPCTSTR)(m_sTxtSettings.strFontName);
	tab.AddValue(PF_FONT, &CVariantEx(var));
	var = (double)(m_sTxtSettings.fHeight);
	tab.AddValue(PF_CHARH, &CVariantEx(var));
	var = (double)(m_sTxtSettings.fWidScale);
	tab.AddValue(PF_CHARW, &CVariantEx(var));
	var = (double)(m_sTxtSettings.fCharIntervalScale);
	tab.AddValue(PF_CHARI, &CVariantEx(var));
	var = (double)(m_sTxtSettings.fLineSpacingScale);
	tab.AddValue(PF_LINEI, &CVariantEx(var));
	var = (_variant_t)(long)(m_sTxtSettings.nInclineType);
	tab.AddValue(PF_SHRUG, &CVariantEx(var));
	var = (double)(m_sTxtSettings.fInclineAngle);
	tab.AddValue(PF_SHRUGA, &CVariantEx(var));
	var = (bool)m_bMarkText;
	tab.AddValue(PF_EAVESALTERMARKTEXT, &CVariantEx(var));
}
void CEavesAdjustCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	//CParallelMoveCommand::FillShowParams(param,bForLoad);
	param->SetOwnerID("EavesAlterCommand", StrFromResID(IDS_CMDNAME_EAVESALTER));
	param->AddParam(PF_EAVESADJUSTKEEPOLD, bool(m_bKeepOld), StrFromResID(IDS_CMDPLANE_KEEPOLD));

	if ((m_bKeepOld && !m_bMarkText) || bForLoad)
	{
		param->AddLayerNameParam(CEA_CMD_ORI_CODE, LPCTSTR(m_ori_LayerCode), StrFromResID(IDS_CMDPLANE_ORI_EAV_CODE));
	}

	param->AddParam(PF_EAVESALTERMARKTEXT, bool(m_bMarkText), StrFromResID(IDS_CMDPLANE_MARKTEXT));
	if (m_bMarkText || bForLoad)
	{
		param->AddLayerNameParam(PF_CONTOURCODE, LPCTSTR(m_strLayerCode), StrFromResID(IDS_CMDPLANE_TEXTLAYER));
		param->AddParam(PF_CONTOURDIGITNUM, int(m_nDigitNum), StrFromResID(IDS_CMDPLANE_DIGITNUM));
		param->AddFontNameParam(PF_FONT, m_sTxtSettings.strFontName, StrFromResID(IDS_CMDPLANE_FONT));

		param->AddParam(PF_CHARH, (double)m_sTxtSettings.fHeight, StrFromResID(IDS_CMDPLANE_CHARH));
		param->AddParam(PF_CHARW, (double)m_sTxtSettings.fWidScale, StrFromResID(IDS_CMDPLANE_CHARWS));
		param->AddParam(PF_CHARI, (double)m_sTxtSettings.fCharIntervalScale, StrFromResID(IDS_CMDPLANE_CHARIS));
		param->AddParam(PF_LINEI, (double)m_sTxtSettings.fLineSpacingScale, StrFromResID(IDS_CMDPLANE_LINEIS));
		param->BeginOptionParam(PF_SHRUG, StrFromResID(IDS_CMDPLANE_SHRUG));
		param->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGN), SHRUGN, ' ', m_sTxtSettings.nInclineType == SHRUGN);
		param->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGL), SHRUGL, ' ', m_sTxtSettings.nInclineType == SHRUGL);
		param->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGR), SHRUGR, ' ', m_sTxtSettings.nInclineType == SHRUGR);
		param->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGU), SHRUGU, ' ', m_sTxtSettings.nInclineType == SHRUGU);
		param->AddOption(StrFromResID(IDS_CMDPLANE_SHRUGD), SHRUGD, ' ', m_sTxtSettings.nInclineType == SHRUGD);

		param->EndOptionParam();

		param->AddParam(PF_SHRUGA, (double)m_sTxtSettings.fInclineAngle, StrFromResID(IDS_CMDPLANE_SHRUGA));
	}

}
void CEavesAdjustCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, PF_CONTOURCODE, var))
	{
		m_strLayerCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, CEA_CMD_ORI_CODE, var))
	{
		m_ori_LayerCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, PF_CONTOURDIGITNUM, var))
	{
		m_nDigitNum = (long)(_variant_t)*var;
		if (m_nDigitNum < 1)
		{
			m_nDigitNum = 1;
			AfxMessageBox(IDS_PARAM_ERROR);
			return;
		}
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_FONT, var))
	{
		CString temp = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		strncpy(m_sTxtSettings.strFontName, temp, sizeof(m_sTxtSettings.strFontName) - 1);
		TextStyle style = GetUsedTextStyles()->GetTextStyleByName(temp);
		if (style.IsValid())
		{
			m_sTxtSettings.fWidScale = style.fWidScale;
			m_sTxtSettings.nInclineType = style.nInclineType;
			m_sTxtSettings.fInclineAngle = style.fInclinedAngle;
			m_sTxtSettings.SetBold(style.bBold);
		}

		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_CHARH, var))
	{
		m_sTxtSettings.fHeight = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_CHARW, var))
	{
		m_sTxtSettings.fWidScale = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_CHARI, var))
	{
		m_sTxtSettings.fCharIntervalScale = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_LINEI, var))
	{
		m_sTxtSettings.fLineSpacingScale = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_SHRUG, var))
	{
		m_sTxtSettings.nInclineType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_SHRUGA, var))
	{
		m_sTxtSettings.fInclineAngle = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_EAVESALTERMARKTEXT, var))
	{
		m_bMarkText = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CParallelMoveCommand::SetParams(tab, bInit);
}
CString CEavesAdjustCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_EAVESALTER);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CPartMoveCommand, CEditCommand)

CPartMoveCommand::CPartMoveCommand()
{
	m_nStep = 0;
}


CPartMoveCommand::~CPartMoveCommand()
{
}


CString CPartMoveCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_PARTMOVE);
}


void CPartMoveCommand::Start()
{
	m_pEditor->OpenSelector(SELMODE_SINGLE);
	m_nStep = 0;

	CCommand::Start();
}


void CPartMoveCommand::Abort()
{
	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_nStep = -1;
	CEditCommand::Abort();
}

void CPartMoveCommand::Finish()
{
	CEditCommand::Finish();
}

void CPartMoveCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		m_ptMoveStart = pt;
		int num = 0;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if (num == 1)
		{
			CGeometry *pGeo = HandleToFtr(handles[0])->GetGeometry();

			if (!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				AfxMessageBox(StrFromResID(IDS_TIP_SUPPORTCURVE));
				m_pEditor->DeselectAll();
				return;
			}
			m_objHandle = handles[0];
			m_nStep = 1;

			m_pEditor->CloseSelector();

			GotoState(PROCSTATE_PROCESSING);
		}
	}
	else if (m_nStep == 1)
	{
		CGeometry *pGeo = HandleToFtr(m_objHandle)->GetGeometry();
		if (!pGeo)
		{
			Abort();
			return;
		}

		CArray<PT_3DEX, PT_3DEX> pts;
		pGeo->GetShape(pts);

		int num = pts.GetSize();
		if (num < 2)
		{
			Abort();
			return;
		}

		int index;
		PT_3DEX nearstPt;
		GraphAPI::GGetNearstDisPToPointSet2D(pts.GetData(), num, PT_3DEX(m_ptMoveStart, penNone), nearstPt, &index);

		if (index < 0 || index >= num - 1)
		{
			Abort();
			return;
		}

		double matrix[16];
		Matrix44FromMove(pt.x - nearstPt.x, pt.y - nearstPt.y, 0, matrix);

		double vo[4] = { 0, 0, 0, 1.0 };
		double vr[4];

		for (int i = index; i <= index + 1; i++)
		{
			vo[0] = pts[i].x, vo[1] = pts[i].y, vo[2] = pts[i].z;
			matrix_multiply_byvector(matrix, 4, 4, vo, vr);
			pts[i].x = vr[0], pts[i].y = vr[1], pts[i].z = vr[2];
		}


		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && ((CGeoCurveBase*)pGeo)->IsClosed())
		{
			if (index == num - 2)
			{
				pts[0] = pts[num - 1];
			}
			else if (index == 0)
			{
				pts[num - 1] = pts[0];
			}

			//pts.RemoveAt(num-1);
		}

		CUndoModifyProperties undo(m_pEditor, Name());

		undo.arrHandles.Add(m_objHandle);
		undo.oldVT.BeginAddValueItem();
		HandleToFtr(m_objHandle)->WriteTo(undo.oldVT);
		undo.newVT.EndAddValueItem();

		m_pEditor->DeleteObject(m_objHandle, FALSE);

		if (!pGeo->CreateShape(pts.GetData(), pts.GetSize()))
		{
			Abort();
			return;
		}

		m_pEditor->RestoreObject(m_objHandle);

		undo.newVT.BeginAddValueItem();
		HandleToFtr(m_objHandle)->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged(TRUE);
		undo.Commit();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
	}
	return;
}


void CPartMoveCommand::PtMove(PT_3D &pt)
{
	if (m_nStep == 1)
	{
		CFeature *pFtr = HandleToFtr(m_objHandle);
		if (!pFtr) return;

		CGeometry *pGeo = pFtr->GetGeometry();
		if (!pGeo) return;

		CArray<PT_3DEX, PT_3DEX> pts;
		pGeo->GetShape(pts);

		int num = pts.GetSize();
		if (num < 2)
		{
			return;
		}

		int index;
		PT_3DEX nearstPt;
		GraphAPI::GGetNearstDisPToPointSet2D(pts.GetData(), num, PT_3DEX(m_ptMoveStart, penNone), nearstPt, &index);

		if (index < 0 || index >= num - 1)
		{
			return;
		}

		double matrix[16];
		Matrix44FromMove(pt.x - nearstPt.x, pt.y - nearstPt.y, 0, matrix);

		double vo[4] = { 0, 0, 0, 1.0 };
		double vr[4];

		for (int i = index; i <= index + 1; i++)
		{
			vo[0] = pts[i].x, vo[1] = pts[i].y, vo[2] = pts[i].z;
			matrix_multiply_byvector(matrix, 4, 4, vo, vr);
			pts[i].x = vr[0], pts[i].y = vr[1], pts[i].z = vr[2];
		}

		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && ((CGeoCurveBase*)pGeo)->IsClosed())
		{
			if (index == num - 2)
			{
				pts[0] = pts[num - 1];
			}
			else if (index == 0)
			{
				pts[num - 1] = pts[0];
			}
		}

		GrBuffer buf;
		buf.BeginLineString(0, 0);

		if (index == 0)
		{
			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && ((CGeoCurveBase*)pGeo)->IsClosed())
			{
				buf.MoveTo(&pts[num - 2]);
				buf.LineTo(&pts[num - 1]);
				buf.MoveTo(&pts[index]);
				buf.LineTo(&pts[index + 1]);
				buf.LineTo(&pts[index + 2]);
			}
			else
			{
				buf.MoveTo(&pts[0]);
				buf.LineTo(&pts[1]);
				if (num >= 3)
				{
					buf.LineTo(&pts[2]);
				}
			}
		}
		else if (index == num - 2)
		{
			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && ((CGeoCurveBase*)pGeo)->IsClosed())
			{
				buf.MoveTo(&pts[index - 1]);
				buf.LineTo(&pts[index]);
				buf.LineTo(&pts[index + 1]);
				buf.MoveTo(&pts[0]);
				buf.LineTo(&pts[1]);
			}
			else
			{
				buf.MoveTo(&pts[num - 1]);
				buf.LineTo(&pts[num - 2]);
				if (num >= 3)
				{
					buf.LineTo(&pts[num - 3]);
				}
			}
		}
		else
		{
			buf.MoveTo(&pts[index - 1]);
			buf.LineTo(&pts[index]);
			buf.LineTo(&pts[index + 1]);
			buf.LineTo(&pts[index + 2]);
		}
		buf.End();


		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
	}

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CPlaceOrderCommand, CEditCommand)

CPlaceOrderCommand::CPlaceOrderCommand()
{
	m_nPlaceMode = 0;
}

CPlaceOrderCommand::~CPlaceOrderCommand()
{
}

CString CPlaceOrderCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_PLACEORDER);
}

void CPlaceOrderCommand::Start()
{
	if (!m_pEditor)return;

	m_nPlaceMode = 0;
	m_arrSrcFtrs.RemoveAll();
	m_arrRefFtrs.RemoveAll();

	CEditCommand::Start();

	if (m_nPlaceMode == 4)
	{
		m_pEditor->CloseSelector();
		m_nStep = 2;
	}
	else
	{
		m_pEditor->OpenSelector();

		int numsel;
		m_pEditor->GetSelection()->GetSelectedObjs(numsel);
		if (numsel > 0)
		{
			PT_3D pt;
			PtClick(pt, 0);
			m_nExitCode = CMPEC_STARTOLD;
			return;
		}
		else
		{
			GOutPut(StrFromResID(IDS_TIP_SELRETFTRS));
		}
	}

}

void CPlaceOrderCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (long)(m_nPlaceMode);
	tab.AddValue(PF_PLACE_MODE, &CVariantEx(var));

}

void CPlaceOrderCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("PlaceOrderCommand", StrFromLocalResID(IDS_CMDNAME_PLACEORDER));

	param->BeginOptionParam(PF_PLACE_MODE, StrFromResID(IDS_CMDPLANE_PLACEMODE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_PLACETOP), 0, ' ', m_nPlaceMode == 0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_PLACETAIL), 1, ' ', m_nPlaceMode == 1);
	param->AddOption(StrFromResID(IDS_CMDPLANE_PLACETOPREFFTRS), 2, ' ', m_nPlaceMode == 2);
	param->AddOption(StrFromResID(IDS_CMDPLANE_PLACEBOTTOMREFFTRS), 3, ' ', m_nPlaceMode == 3);
	param->AddOption(StrFromResID(IDS_CMDPLANE_PLACETOPTEXT), 4, ' ', m_nPlaceMode == 4);
	param->EndOptionParam();

}


void CPlaceOrderCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_PLACE_MODE, var))
	{
		m_nPlaceMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();

		if (!bInit)
		{
			Abort();
			return;
		}
	}

	CEditCommand::SetParams(tab, bInit);
}

void CPlaceOrderCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nPlaceMode != 4)
	{
		//前置后置
		if (m_nPlaceMode == 0 || m_nPlaceMode == 1)
		{
			if (m_nStep == 0 || m_nStep == 1)
			{
				if (!CanGetSelObjs(flag))return;

				CSelection* pSel = m_pEditor->GetSelection();
				int num;
				const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);

				if (m_nStep == 0)
				{
					m_arrSrcFtrs.SetSize(num);
					memcpy(m_arrSrcFtrs.GetData(), handles, num*sizeof(CFeature*));

					m_nStep = 2;
				}
			}
		}
		else
		{
			if (m_nStep == 0 || (m_nStep == 1 && (flag&SELSTAT_MULTISEL) != 0))
			{
				//by mzy
				if ((flag&SELSTAT_POINTSEL) != 0 || (SELSTAT_DRAGSEL&flag) != 0)
				{
					int num = -1;
					const FTR_HANDLE* handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
					if (num <= 0)
						return;

					m_arrSrcFtrs.RemoveAll();
					for (int i = 0; i < num; i++)
					{
						m_arrSrcFtrs.Add(HandleToFtr(handles[i]));
					}
					m_nStep = 1;
				}

				return;
			}

			if (m_nStep == 1)
			{
				if ((flag&SELSTAT_POINTSEL) != 0 || (SELSTAT_DRAGSEL&flag) != 0)
				{
					int num = -1;
					const FTR_HANDLE* handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
					if (num <= 0)
						return;

					m_arrRefFtrs.RemoveAll();
					for (int i = 0; i < num; i++)
					{
						m_arrRefFtrs.Add(HandleToFtr(handles[i]));
					}

					m_pEditor->CloseSelector();
					m_nStep = 2;
				}
			}
		}
	}


	if (m_nStep == 2)
	{
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		if (!pDS)
		{
			Abort();
			return;
		}

		int nPlaceMode = m_nPlaceMode;
		if (m_nPlaceMode == 4)
		{
			CFtrArray arr;
			pDS->GetAllFtrsByDisplayOrder(arr);

			for (int i = 0; i < arr.GetSize(); i++)
			{
				CFeature *pFtr = arr[i];
				if (pFtr && pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
				{
					m_arrSrcFtrs.Add(pFtr);
				}
			}

			nPlaceMode = 0;
		}

		CUndoModifyProperties undo(m_pEditor, Name());

		for (int i = 0; i < m_arrSrcFtrs.GetSize(); i++)
		{
			undo.arrHandles.Add(FTR_HANDLE(m_arrSrcFtrs[i]));
			undo.oldVT.BeginAddValueItem();
			m_arrSrcFtrs[i]->WriteTo(undo.oldVT);
			undo.oldVT.EndAddValueItem();
		}

		if (((nPlaceMode == 0 || nPlaceMode == 2) && !pDS->DisplayTop(m_arrSrcFtrs.GetData(), m_arrSrcFtrs.GetSize(), m_arrRefFtrs.GetData(), m_arrRefFtrs.GetSize())) ||
			((nPlaceMode == 1 || nPlaceMode == 3) && !pDS->DisplayBottom(m_arrSrcFtrs.GetData(), m_arrSrcFtrs.GetSize(), m_arrRefFtrs.GetData(), m_arrRefFtrs.GetSize())))
		{
			Abort();
			return;
		}

		for (i = 0; i < m_arrSrcFtrs.GetSize(); i++)
		{
			m_pEditor->DeleteObject(FTR_HANDLE(m_arrSrcFtrs[i]), FALSE);
			m_pEditor->RestoreObject(FTR_HANDLE(m_arrSrcFtrs[i]));

			undo.newVT.BeginAddValueItem();
			m_arrSrcFtrs[i]->WriteTo(undo.newVT);
			undo.newVT.EndAddValueItem();
		}

		undo.JustKeepBasicFields(CString(FIELDNAME_FTRDISPLAYORDER));

		undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		Finish();
		m_nStep = 2;
	}

	CCommand::PtClick(pt, flag);
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CPlaceOrderFrontCommand, CPlaceOrderCommand)

CString CPlaceOrderFrontCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_PLACEORDER_FRONT);
}

void CPlaceOrderFrontCommand::Start()
{
	if (!m_pEditor)return;

	m_nPlaceMode = 0;
	m_arrSrcFtrs.RemoveAll();
	m_arrRefFtrs.RemoveAll();

	CEditCommand::Start();
	m_nPlaceMode = 0;

	{
		m_pEditor->OpenSelector();

		int numsel;
		m_pEditor->GetSelection()->GetSelectedObjs(numsel);
		if (numsel > 0)
		{
			PT_3D pt;
			PtClick(pt, 0);
			m_nExitCode = CMPEC_STARTOLD;
			return;
		}
		else
		{
			GOutPut(StrFromResID(IDS_TIP_SELRETFTRS));
		}
	}
}

void CPlaceOrderFrontCommand::GetParams(CValueTable &tab)
{

}

void CPlaceOrderFrontCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(AccelStr(), Name());

}


void CPlaceOrderFrontCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	CEditCommand::SetParams(tab, bInit);
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CPlaceOrderBackCommand, CPlaceOrderCommand)

CString CPlaceOrderBackCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_PLACEORDER_BACK);
}

void CPlaceOrderBackCommand::Start()
{
	if (!m_pEditor)return;

	m_nPlaceMode = 1;
	m_arrSrcFtrs.RemoveAll();
	m_arrRefFtrs.RemoveAll();

	CEditCommand::Start();
	m_nPlaceMode = 1;

	{
		m_pEditor->OpenSelector();

		int numsel;
		m_pEditor->GetSelection()->GetSelectedObjs(numsel);
		if (numsel > 0)
		{
			PT_3D pt;
			PtClick(pt, 0);
			m_nExitCode = CMPEC_STARTOLD;
			return;
		}
		else
		{
			GOutPut(StrFromResID(IDS_TIP_SELRETFTRS));
		}
	}
}

void CPlaceOrderBackCommand::GetParams(CValueTable &tab)
{

}

void CPlaceOrderBackCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(AccelStr(), Name());

}


void CPlaceOrderBackCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	CEditCommand::SetParams(tab, bInit);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDirPointToLineCommand, CEditCommand)

CDirPointToLineCommand::CDirPointToLineCommand()
{
	m_fLength = CGeoDirPoint::GetDefaultDirLen()*GetSymbolDrawScale();
}

CDirPointToLineCommand::~CDirPointToLineCommand()
{
}

CString CDirPointToLineCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_POINTTOLINE);
}

void CDirPointToLineCommand::Start()
{
	if (!m_pEditor)return;
	int numsel;
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);
	if (numsel > 0)
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();

		CCommand::Start();

		PT_3D pt;
		PtClick(pt, 0);
		PtClick(pt, 0);

		return;
	}

	CEditCommand::Start();
}

void CDirPointToLineCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (double)(m_fLength);
	tab.AddValue(PF_PTLINE_LENGTH, &CVariantEx(var));

}

void CDirPointToLineCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DirPointToLine", StrFromLocalResID(IDS_CMDNAME_POINTTOLINE));

	param->AddParam(PF_PTLINE_LENGTH, double(m_fLength), StrFromResID(IDS_CMDPLANE_LINELENGTH));

}


void CDirPointToLineCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_PTLINE_LENGTH, var))
	{
		m_fLength = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}

void CDirPointToLineCommand::DirPointToLine(const FTR_HANDLE* handles, int num)
{
	CUndoFtrs undo(m_pEditor, Name());

	for (int i = 0; i < num; i++)
	{
		CGeometry *pGeo = HandleToFtr(handles[i])->GetGeometry();
		if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)))
		{
			continue;
		}

		CGeoDirPoint *pDir = (CGeoDirPoint*)pGeo;

		CArray<PT_3DEX, PT_3DEX> pts;
		pDir->GetShape(pts);

		if (pts.GetSize() < 1) continue;

		double ang = pDir->GetDirection()*PI / 180;
		PT_3DEX line[2];
		line[0] = line[1] = pts[0];
		line[1].x = line[0].x + fabs(m_fLength)*cos(ang);
		line[1].y = line[0].y + fabs(m_fLength)*sin(ang);
		line[1].z = line[0].z;

		CFeature *pNew = HandleToFtr(handles[i])->Clone();
		if (!pNew) continue;

		if (!pNew->CreateGeometry(CLS_GEOCURVE)) continue;

		pNew->GetGeometry()->SetColor(HandleToFtr(handles[i])->GetGeometry()->GetColor());
		pNew->GetGeometry()->CreateShape(line, 2);

		if (!m_pEditor->AddObject(pNew, m_pEditor->GetFtrLayerIDOfFtr(handles[i])))
		{
			delete pNew;
			continue;
		}

		GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handles[i]), pNew);
		undo.AddNewFeature(FTR_HANDLE(pNew));
		undo.AddOldFeature(handles[i]);
		m_pEditor->DeleteObject(handles[i]);
	}

	undo.Commit();
}

void CDirPointToLineCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;
		EditStepOne();
	}

	if (m_nStep == 1)
	{
		CSelection* pSel = m_pEditor->GetSelection();

		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		DirPointToLine(handles, num);
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}

	CCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// CInsteadLittleHouseCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CInsteadLittleHouseCommand, CEditCommand)

CInsteadLittleHouseCommand::CInsteadLittleHouseCommand()
{
	m_areaLimit = -1;
	m_longLimit = -1;
	m_shortLimit = -1;
	strcat(m_strRegPath, "\\InsteadLittleHouse");
}

CInsteadLittleHouseCommand::~CInsteadLittleHouseCommand()
{

}

CString CInsteadLittleHouseCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_INSTEADLITTLEHOUSE);
}

void CInsteadLittleHouseCommand::Start()
{
	if (!m_pEditor)return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CInsteadLittleHouseCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CInsteadLittleHouseCommand::Abort()
{
	UpdateParams(TRUE);
	m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CInsteadLittleHouseCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("Layer0", &CVariantEx(var));

	var = (LPCTSTR)(m_StoreLayer);
	tab.AddValue("Layer1", &CVariantEx(var));

	var = m_areaLimit;
	tab.AddValue("AreaLimit", &CVariantEx(var));

	var = m_longLimit;
	tab.AddValue("MaxLimit", &CVariantEx(var));

	var = m_shortLimit;
	tab.AddValue("MinLimit", &CVariantEx(var));
}

void CInsteadLittleHouseCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("InsteadLittleHouseCommand", Name());

	param->AddLayerNameParamEx("Layer0", (LPCTSTR)m_HandleLayer, StrFromResID(IDS_HOUSE_LAYNAMES), NULL, LAYERPARAMITEM_LINEAREA | LAYERPARAMITEM_NOTEMPTY);

	param->AddLayerNameParamEx("Layer1", (LPCTSTR)m_StoreLayer, StrFromResID(IDS_CONVERTTO_LAYER));

	param->AddParam("AreaLimit", (double)m_areaLimit, StrFromResID(IDS_HOUSE_AREALIMIT));
	param->AddParam("MaxLimit", (double)m_longLimit, StrFromResID(IDS_HOUSE_LONGLIMIT));
	param->AddParam("MinLimit", (double)m_shortLimit, StrFromResID(IDS_HOUSE_SHORTLIMIT));
}

void CInsteadLittleHouseCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "Layer0", var))
	{
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimLeft();
		m_HandleLayer.TrimRight();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "Layer1", var))
	{
		m_StoreLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_StoreLayer.TrimRight();
		m_StoreLayer.TrimLeft();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "AreaLimit", var))
	{
		m_areaLimit = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "MaxLimit", var))
	{
		m_longLimit = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "MinLimit", var))
	{
		m_shortLimit = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}

void CInsteadLittleHouseCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor)return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;
	if (m_HandleLayer.IsEmpty() || m_StoreLayer.IsEmpty()) return;

	CFtrLayerArray arrLayers;
	pDS->GetFtrLayersByNameOrCode(m_HandleLayer, arrLayers);
	int nSum = 0, i = 0;
	for (i = 0; i < arrLayers.GetSize(); i++)
	{
		nSum += arrLayers[i]->GetValidObjsCount();
	}

	CFtrLayer *out_layer = NULL;
	out_layer = pDS->GetFtrLayer((LPCTSTR)m_StoreLayer);
	if (!out_layer)
	{
		return;
	}
	int layerID = out_layer->GetID();
	int scale = pDS->GetScale();
	ConfigLibItem config = GetConfigLibManager()->GetConfigLibItemByScale(scale);
	CSchemeLayerDefine *pSchemeLayerDefine = NULL;
	pSchemeLayerDefine = config.pScheme->GetLayerDefine(out_layer->GetName());
	if (!pSchemeLayerDefine) return;
	int clsType = pSchemeLayerDefine->GetGeoClass();
	if (clsType != CLS_GEOCURVE && clsType != CLS_GEOPOINT && clsType != CLS_GEODIRPOINT && clsType != CLS_GEOSURFACE)
		return;

	CUndoFtrs undo(m_pEditor, Name());

	GProgressStart(nSum);
	for (i = 0; i < arrLayers.GetSize(); i++)
	{
		CFtrLayer *pLayer = arrLayers[i];
		int nObj = pLayer->GetObjectCount();
		for (int j = 0; j < nObj; j++)
		{
			GProgressStep();
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr || !pFtr->IsVisible()) continue;
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || pGeo->GetClassType() != CLS_GEOSURFACE) continue;

			CArray<PT_3DEX, PT_3DEX> retPts;
			if (BInstead(pGeo, clsType, retPts))
			{
				CFeature *pNew = out_layer->CreateDefaultFeature(scale, clsType);
				pNew->GetGeometry()->CreateShape(retPts.GetData(), retPts.GetSize());
				if (!m_pEditor->AddObject(pNew, layerID))
				{
					delete pNew;
					continue;
				}
				undo.AddOldFeature(FtrToHandle(pFtr));
				m_pEditor->DeleteObject(FtrToHandle(pFtr));
				undo.AddNewFeature(FtrToHandle(pNew));
			}
		}
	}
	GProgressEnd();
	undo.Commit();

	Finish();
	CEditCommand::PtClick(pt, flag);
}

static double GetLength(PT_3DEX* pt1, PT_3DEX* pt2)
{
	return sqrt((pt1->x - pt2->x)*(pt1->x - pt2->x) +
		(pt1->y - pt2->y)*(pt1->y - pt2->y) +
		(pt1->z - pt2->z)*(pt1->z - pt2->z));
}

BOOL CInsteadLittleHouseCommand::BInstead(CGeometry* pGeo, int clsType, CArray<PT_3DEX, PT_3DEX>& retPts)
{
	if (!pGeo) return FALSE;

	CArray<PT_3DEX, PT_3DEX> pts;
	pGeo->GetShape(pts);
	int nPt = pts.GetSize();
	if (nPt < 3) return FALSE;

	int conditions = 0;
	//面积
	double v = ((CGeoCurveBase*)pGeo)->GetArea();
	if (m_areaLimit < 0 || v < m_areaLimit)
	{
		conditions++;
	}
	//长短边
	double minlen = GetLength(&pts[0], &pts[1]);
	int minlenpos = 0;
	double maxlen = minlen;
	for (int i = 1; i < nPt - 1; i++)
	{
		double len = GetLength(&pts[i], &pts[i + 1]);
		if (len < minlen)
		{
			minlen = len;
			minlenpos = i;
		}
		else if (len >= maxlen)
		{
			maxlen = len;
		}
	}

	if (m_longLimit < 0 || maxlen < m_longLimit)
	{
		conditions++;
	}
	if (m_shortLimit < 0 || minlen < m_shortLimit)
	{
		conditions++;
	}
	if (conditions != 3) return FALSE;

	if (clsType == CLS_GEOPOINT)
	{
		PT_3D ptcen;
		pGeo->GetCenter(&pts[0], &ptcen);
		PT_3DEX pt(ptcen, penNone);
		retPts.Add(pt);
	}
	else if (clsType == CLS_GEOSURFACE)
	{
		retPts.Copy(pts);
	}
	else if (clsType == CLS_GEOCURVE || clsType == CLS_GEODIRPOINT)
	{
		PT_3DEX pt1, pt2;
		pt1.x = (pts[minlenpos].x + pts[minlenpos + 1].x) / 2;
		pt1.y = (pts[minlenpos].y + pts[minlenpos + 1].y) / 2;
		pt1.z = (pts[minlenpos].z + pts[minlenpos + 1].z) / 2;
		pt1.pencode = penLine;
		retPts.Add(pt1);
		int pos1;
		if (minlenpos < nPt / 2)
		{
			pos1 = minlenpos + (nPt - 1) / 2;
		}
		else
		{
			pos1 = minlenpos - (nPt - 1) / 2;
		}
		pt2.x = (pts[pos1].x + pts[pos1 + 1].x) / 2;
		pt2.y = (pts[pos1].y + pts[pos1 + 1].y) / 2;
		pt2.z = (pts[pos1].z + pts[pos1 + 1].z) / 2;
		pt2.pencode = penLine;
		retPts.Add(pt2);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}


///////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCulvertReverseCommand, CEditCommand)

CCulvertReverseCommand::CCulvertReverseCommand()
{
	m_linetypeReverse = FALSE;
	m_Reverse = TRUE;
}

CCulvertReverseCommand::~CCulvertReverseCommand()
{

}

void CCulvertReverseCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;
		EditStepOne();
	}
	if (m_nStep == 1)
	{
		CSelection * pSel = m_pEditor->GetSelection();
		int num = 0;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		if (num <= 0)
			return;

		if (!m_linetypeReverse && !m_Reverse)
			return;

		CUndoModifyProperties undo(m_pEditor, Name());
		GetActiveDlgDoc()->BeginBatchUpdate();
		for (int i = 0; i < num; ++i)
		{
			CPFeature pftr = HandleToFtr(handles[i]);
			if (pftr == NULL || pftr->GetGeometry() == NULL)
				continue;
			if (pftr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
			{
				CGeoParallel* pold = (CGeoParallel*)pftr->GetGeometry();
				if (m_linetypeReverse)
				{
					undo.arrHandles.Add(handles[i]);
					undo.oldVT.BeginAddValueItem();
					HandleToFtr(handles[i])->WriteTo(undo.oldVT);
					undo.oldVT.EndAddValueItem();
					//
					CArray<PT_3DEX, PT_3DEX> points;
					pold->GetShape(points);
					CArray<PT_3DEX, PT_3DEX> parallel_points;
					pold->GetParallelShape(parallel_points);
					//
					CGeoParallel* pnew = new CGeoParallel();
					CArray<PT_3DEX, PT_3DEX> new_points;
					new_points.Add(points[0]);
					new_points.Add(parallel_points[0]);
					pnew->CreateShape(new_points.GetData(), new_points.GetSize());
					pnew->SetCtrlPoint(0, points[points.GetSize() - 1]);
					pftr->SetGeometry(pnew);
					//
					m_pEditor->DeleteObject(handles[i], FALSE);
					if (!m_pEditor->RestoreObject(handles[i]))
					{
						continue;
					}

					undo.newVT.BeginAddValueItem();
					HandleToFtr(handles[i])->WriteTo(undo.newVT);
					undo.newVT.EndAddValueItem();
				}
			}
			else if (pftr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
			{
				CGeoDCurve* pold = (CGeoDCurve*)pftr->GetGeometry();
				if (m_linetypeReverse)
				{
					undo.arrHandles.Add(handles[i]);
					undo.oldVT.BeginAddValueItem();
					HandleToFtr(handles[i])->WriteTo(undo.oldVT);
					undo.oldVT.EndAddValueItem();
					//
					CArray<PT_3DEX, PT_3DEX> points;
					pold->GetBaseShape(points);
					CArray<PT_3DEX, PT_3DEX> assist_points;
					pold->GetAssistShape(assist_points);
					//
					CGeoDCurve* pnew = new CGeoDCurve();
					CArray<PT_3DEX, PT_3DEX> new_points;
					new_points.Add(points[0]);
					new_points[new_points.GetSize() - 1].pencode = penLine;
					new_points.Add(assist_points[assist_points.GetSize() - 1]);
					new_points[new_points.GetSize() - 1].pencode = penLine;
					new_points.Add(assist_points[0]);
					new_points[new_points.GetSize() - 1].pencode = penMove;
					new_points.Add(points[points.GetSize() - 1]);
					new_points[new_points.GetSize() - 1].pencode = penLine;

					pnew->CreateShape(new_points.GetData(), new_points.GetSize());
					pftr->SetGeometry(pnew);
					//
					m_pEditor->DeleteObject(handles[i], FALSE);
					if (!m_pEditor->RestoreObject(handles[i]))
					{
						continue;
					}

					undo.newVT.BeginAddValueItem();
					HandleToFtr(handles[i])->WriteTo(undo.newVT);
					undo.newVT.EndAddValueItem();
				}
			}
			else if (pftr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) && !pftr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
			{

				undo.arrHandles.Add(handles[i]);
				undo.oldVT.BeginAddValueItem();
				HandleToFtr(handles[i])->WriteTo(undo.oldVT);
				undo.oldVT.EndAddValueItem();
				//
				if (m_linetypeReverse)
				{
					CGeoSurface* pold = (CGeoSurface*)pftr->GetGeometry();
					CArray<PT_3DEX, PT_3DEX> points;
					pold->GetShape(points);
					//
					CGeoSurface* pnew = new CGeoSurface();
					CArray<PT_3DEX, PT_3DEX> new_points;
					int start_index = points.GetSize() - 1 - ceil((double)(points.GetSize() - 3) / double(4));
					for (int t = start_index; t < points.GetSize(); ++t)
					{
						new_points.Add(points[t]);
					}
					for (t = 1; t <= start_index; ++t)
					{
						new_points.Add(points[t]);
					}
					pnew->CreateShape(new_points.GetData(), new_points.GetSize());
					pftr->SetGeometry(pnew);
				}
				//
				if (m_Reverse)
				{
					CGeoSurface* pold = (CGeoSurface*)pftr->GetGeometry();
					CArray<PT_3DEX, PT_3DEX> points;
					pold->GetShape(points);
					//
					CGeoSurface* pnew = new CGeoSurface();
					CArray<PT_3DEX, PT_3DEX> new_points;
					int count_first = ceil(double((points.GetSize() - 3)) / 2.0 - 0.1);
					for (int t = count_first; t > 0; --t)
					{
						new_points.Add(points[t]);
					}
					for (t = points.GetSize() - 1; t >= count_first; --t)
					{
						new_points.Add(points[t]);
					}
					pnew->CreateShape(new_points.GetData(), new_points.GetSize());
					pftr->SetGeometry(pnew);
				}
				m_pEditor->DeleteObject(handles[i], FALSE);
				if (!m_pEditor->RestoreObject(handles[i]))
				{
					continue;
				}

				undo.newVT.BeginAddValueItem();
				HandleToFtr(handles[i])->WriteTo(undo.newVT);
				undo.newVT.EndAddValueItem();
			}
			//	
		}
		GetActiveDlgDoc()->EndBatchUpdate();
		undo.Commit();
	}

	Finish();
	CEditCommand::PtClick(pt, flag);
}

void CCulvertReverseCommand::Abort()
{
	UpdateParams(TRUE);
	m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CCulvertReverseCommand::Start()
{
	if (!m_pEditor)return;
	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num > 0)
	{
		m_nStep = 0;
		CCommand::Start();

		PT_3D pt;
		PtClick(pt, SELSTAT_NONESEL);
		return;
	}
	CEditCommand::Start();
}

void CCulvertReverseCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (bool)(m_linetypeReverse);
	tab.AddValue(PF_LINETYPEREVERSE, &CVariantEx(var));
	var = (bool)(m_Reverse);
	tab.AddValue(PF_CULVERTREVERSE, &CVariantEx(var));
}

void CCulvertReverseCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("CulvertRevertCommand", Name());

	param->BeginOptionParam(PF_CULVERTREVERSE, StrFromResID(IDS_CMDNAME_CULVERTREVERSE));
	param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_Reverse);
	param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_Reverse);
	param->EndOptionParam();

	param->BeginOptionParam(PF_LINETYPEREVERSE, StrFromResID(IDS_CULVERTREVERSE_LINETYPEREVERSE));
	param->AddOption(StrFromResID(IDS_YES), 1, ' ', m_linetypeReverse);
	param->AddOption(StrFromResID(IDS_NO), 0, ' ', !m_linetypeReverse);
	param->EndOptionParam();
}

void CCulvertReverseCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_LINETYPEREVERSE, var))
	{
		m_linetypeReverse = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, PF_CULVERTREVERSE, var))
	{
		m_Reverse = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab, bInit);
}

CString CCulvertReverseCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CULVERTREVERSE);
}

void CCulvertReverseCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	CEditCommand::Finish();
}


//////////////////////////////////////////////////////////////////////
// CSelectLayerCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSelectLayerCommand, CCommand)

CSelectLayerCommand::CSelectLayerCommand()
{
}

CSelectLayerCommand::~CSelectLayerCommand()
{
}

CString CSelectLayerCommand::Name()
{
	return StrFromResID(IDS_SELECT_LAYER);
}

void CSelectLayerCommand::Start()
{
	if (!m_pEditor)return;
	int numsel;
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);
	if (numsel == 1)
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();

		CCommand::Start();

		PT_3D pt;
		PtClick(pt, 0);
		PtClick(pt, 0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	CEditCommand::Start();
}

void CSelectLayerCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		if (!CanGetSelObjs(flag))return;
		EditStepOne();
	}

	if (m_nStep == 1)
	{
		CSelection* pSel = m_pEditor->GetSelection();

		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);

		if (num != 1)
		{
			Abort();
			return;
		}

		CDataSourceEx *pDS = m_pEditor->GetDataSource();
		if (!pDS)
		{
			Abort();
			return;
		}
		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(handles[0]));
		if (!pLayer)
		{
			Abort();
			return;
		}
		CArray<FTR_HANDLE, FTR_HANDLE> arr;
		for (int i = 0; i < pLayer->GetObjectCount(); i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if (!pFtr)  continue;
			arr.Add(FtrToHandle(pFtr));
		}

		pSel->DeselectAll();
		pSel->SelectAll(arr.GetData(), arr.GetSize());

		m_pEditor->OnSelectChanged();

		Finish();
		m_nStep = 2;
	}

	CCommand::PtClick(pt, flag);
}


IMPLEMENT_DYNAMIC(CDlgAutoSurfaceCommand, CDrawCurveCommand)

CDlgAutoSurfaceCommand::CDlgAutoSurfaceCommand()
{
	m_pSel = NULL;
	m_nHeightMode = 1;
}

CDlgAutoSurfaceCommand::~CDlgAutoSurfaceCommand()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
}

CString CDlgAutoSurfaceCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_AUTOSURFACE);
}

void CDlgAutoSurfaceCommand::Start()
{
	CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
	if (!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOCURVE);
	if (!m_pFtr) return;

	CDlgDataSource *pDS = GETDS(m_pEditor);

	//判断是否需要在画线的时候就显示符号
	m_bNeedDrawSymbol = FALSE;
	CPtrArray arrSymbols;
	gpCfgLibMan->GetSymbol(pDS, m_pFtr, arrSymbols, pLayer->GetName());
	for (int i = 0; i < arrSymbols.GetSize(); i++)
	{
		CSymbol *pSym = (CSymbol*)arrSymbols[i];
		int type = pSym->GetType();
		if (type == SYMTYPE_SCALE_Cell || type == SYMTYPE_SCALE_DiShangYaoDong ||
			type == SYMTYPE_SCALE_ChuRuKou)
		{
			m_bNeedDrawSymbol = TRUE;
		}
		delete pSym;
	}


	m_pDrawProc = new CDrawCurveProcedure;
	if (!m_pDrawProc)return;

	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pFtr->GetGeometry();
	m_pDrawProc->m_compress.SetLimit(0.1);
	m_pDrawProc->m_compress.m_lfScale = GETCURSCALE(m_pEditor)*1e-3;
	m_pDrawProc->m_layCol = pLayer->GetColor();
	m_pDrawProc->m_bClosed = FALSE;
	m_pDrawProc->Start();
	CDrawCommand::Start();
	if (m_pDrawProc->m_nCurPenCode == penStream)
	{
		m_pDrawProc->m_compress.BeginCompress();
		//		m_bRectify = FALSE;
	}

	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}

void CDlgAutoSurfaceCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
}
void CDlgAutoSurfaceCommand::Finish()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	if (m_pEditor)
	{
		if (m_pFtr)
		{
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);
			if (bAnnotToText)
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
				//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if (pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor), CLS_GEOTEXT);
				if (pLayer != NULL && pTextLayer != NULL && pTempl != NULL)
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor), m_pFtr, arrAnnots);
					CUndoFtrs undo(m_pEditor, Name());
					for (int i = 0; i < arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType() == SYMTYPE_ANNOTATION)
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr, tab);
							if (pAnnot->ExtractGeoText(m_pFtr, parr, tab, PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()))
							{
								for (int j = 0; j < parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);
									PDOC(m_pEditor)->AddObject(pNewFtr, pTextLayer->GetID());
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
								}
							}
						}
					}
					undo.Commit();
				}
				if (pTempl != NULL)delete pTempl;
			}
		}
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}
	CDrawCurveCommand::Finish();
}

void CDlgAutoSurfaceCommand::Abort()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)NULL);
	CDrawCurveCommand::Abort();
}

BOOL CDlgAutoSurfaceCommand::IsEndSnapped()
{
	if (!m_pEditor)
		return FALSE;

	if (!PDOC(m_pEditor)->m_snap.bOpen())
		return FALSE;

	SNAPITEM item = PDOC(m_pEditor)->m_snap.GetFirstSnapResult();
	if (!item.IsValid())
		return FALSE;

	if (item.nSnapMode == CSnap::modeNearPoint ||
		item.nSnapMode == CSnap::modeKeyPoint ||
		item.nSnapMode == CSnap::modeMidPoint ||
		item.nSnapMode == CSnap::modeIntersect ||
		item.nSnapMode == CSnap::modePerpPoint ||
		item.nSnapMode == CSnap::modeTangPoint ||
		item.nSnapMode == CSnap::modeEndPoint)
		return TRUE;

	return FALSE;
}

CProcedure *CDlgAutoSurfaceCommand::GetActiveSonProc(int nMsgType)
{
	if (nMsgType == msgPtClick || nMsgType == msgPtMove || nMsgType == msgPtReset)
		return NULL;

	return m_pDrawProc;
}

void CDlgAutoSurfaceCommand::PtReset(PT_3D &pt)
{
	if (!m_pDrawProc || !m_pDrawProc->m_pGeoCurve)  return;

	if (m_pDrawProc->m_nCurPenCode == penStream)
	{
		if (IsEndSnapped())
			m_pDrawProc->m_nEndSnapped |= 2;
		else
			m_pDrawProc->m_nEndSnapped &= (~2);
	}

	m_pDrawProc->PtReset(pt);
}

void CDlgAutoSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pDrawProc)  return;

	ASSERT(m_pDrawProc == son);

	if (!m_pDrawProc->m_pGeoCurve)return;

	m_pDrawProc->PtClick(pt, flag);

	if (m_bNeedDrawSymbol)
		m_pEditor->UpdateDrag(ud_SetConstDrag, NULL);

	if (m_pDrawProc->m_pGeoCurve->GetDataPointSum() == 0)
	{
		m_pDrawProc->m_nEndSnapped = IsEndSnapped() ? 1 : 0;
	}
	else
	{
		if (IsEndSnapped())
			m_pDrawProc->m_nEndSnapped |= 2;
		else
			m_pDrawProc->m_nEndSnapped &= (~2);
	}

	if (m_pDrawProc->m_arrPts.GetSize() == 1/*m_pDrawProc->m_pGeoCurve->GetDataPointSum() == 1*/)
	{
		if (PDOC(m_pEditor)->IsAutoSetAnchor())
		{
			PDOC(m_pEditor)->SetAnchorPoint(m_pDrawProc->m_arrPts[0]);
		}
	}
}

void CDlgAutoSurfaceCommand::PtMove(PT_3D &pt)
{
	if (m_pDrawProc)
		m_pDrawProc->PtMove(pt);

	if (m_bNeedDrawSymbol && m_pDrawProc)
	{
		CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
		if (!pLayer) return;

		CDlgDataSource *pDS = GETDS(m_pEditor);
		CFeature *pFtr = m_pFtr->Clone();
		CArray<PT_3DEX, PT_3DEX> arrPts;
		arrPts.Copy(m_pDrawProc->m_arrPts);
		arrPts.Add(PT_3DEX(pt, penLine));
		pFtr->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize());

		GrBuffer buf;
		pDS->DrawFeature(pFtr, &buf, FALSE, 0, pLayer->GetName());
		delete pFtr;

		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
	}
}


void CDlgAutoSurfaceCommand::OnSonEnd(CProcedure *son)
{
	CDrawCurveCommand::OnSonEnd(son);
}

DrawingInfo CDlgAutoSurfaceCommand::GetCurDrawingInfo()
{
	if (!m_pFtr || !m_pDrawProc)return DrawingInfo();
	CArray<PT_3DEX, PT_3DEX> pts;
	pts.Append(m_pDrawProc->m_arrAllPts);
	pts.Append(m_pDrawProc->m_arrPts);
	return	DrawingInfo(m_pFtr, pts);
}

void CDlgAutoSurfaceCommand::GetParams(CValueTable &tab)
{
	CDrawCommand::GetParams(tab);

	if (!m_pDrawProc) return;

	_variant_t var;
	var = (long)(m_pDrawProc->m_nCurPenCode);
	tab.AddValue(PF_PENCODE, &CVariantEx(var));

	var = (long)(m_nHeightMode);
	tab.AddValue("nHeightMode", &CVariantEx(var));
}

void CDlgAutoSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(AccelStr(), Name());
	if (m_pDrawProc)
	{
		PT_3D pt;
		param->AddParam(PF_POINT, pt, StrFromLocalResID(IDS_CMDPLANE_POINT));
		param->AddParam(PF_REVERSEACCKEY, 't', StrFromResID(IDS_CMDPLANE_REVERSE));
		param->AddParam(PF_TRACKLINEACCKEY, 'w', StrFromResID(IDS_CMDPLANE_TRACKLINE));
		param->AddLineTypeParam(PF_PENCODE, m_pDrawProc->m_nCurPenCode, StrFromLocalResID(IDS_CMDPLANE_LINETYPE));

		param->BeginOptionParam("nHeightMode", StrFromResID(IDS_CMDPLANE_DHEIGHTMODE));
		param->AddOption(StrFromResID(IDS_USE_SELF_HEIGHT), 1, ' ', 1 == m_nHeightMode);
		param->AddOption(StrFromResID(IDS_SNAP_HEIGHT), 2, ' ', 2 == m_nHeightMode);
		param->EndOptionParam();
	}
}

void CDlgAutoSurfaceCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, PF_PENCODE, var))
	{
		if (bInit)
		{
			m_pDrawProc->m_nCurPenCode = (long)(_variant_t)*var;
		}
		else
			m_pDrawProc->ChangePencod((long)(_variant_t)*var);
	}
	if (tab.GetValue(0, PF_NODEWID, var))
	{
		m_pDrawProc->m_fCurNodeWid = (float)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_NODETYPE, var))
	{
		m_pDrawProc->m_nCurNodeType = (short)(_variant_t)*var;
	}
	if (tab.GetValue(0, PF_TRACKLINEACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 'w' || ch == 'W')
		{
			m_pDrawProc->TrackLine();
		}
	}
	if (tab.GetValue(0, PF_REVERSEACCKEY, var))
	{
		char ch = (BYTE)(_variant_t)*var;
		if (ch == 't' || ch == 'T')
		{
			m_pDrawProc->ReverseLine();
		}

	}
	if (tab.GetValue(0, PF_POINT, var))
	{
		CArray<PT_3DEX, PT_3DEX> arrPts;
		var->GetShape(arrPts);
		PT_3D point = arrPts.GetAt(0);
		PtClick(point, 0);
		PtMove(point);
		m_pEditor->RefreshView();
	}
	if (tab.GetValue(0, "nHeightMode", var))
	{
		m_nHeightMode = (long)(_variant_t)*var;
	}

	SetSettingsModifyFlag();
	CDrawCommand::SetParams(tab, bInit);
}

BOOL CDlgAutoSurfaceCommand::AddObject(CPFeature pFtr, int layid)
{
	if (!m_pEditor) return FALSE;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if (!pDS || !pDQ)  return FALSE;

	CFtrLayer *pLayer = NULL;
	if (layid == -1)
	{
		pLayer = pDS->GetCurFtrLayer();
	}
	else
	{
		pDS->GetFtrLayerByIndex(layid);
	}
	if (!pLayer) return FALSE;

	CGeometry *pObj = pFtr->GetGeometry();
	Envelope e = pObj->GetEnvelope();

	e.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS, 1);
	m_pEditor->GetDataQuery()->FindObjectInRect(e, m_pEditor->GetCoordWnd().m_pSearchCS);
	int num = 0;
	const CPFeature *ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);
	if (num <= 0) return FALSE;

	int i = 0;
	//绘制的线和相交地物的外包并集
	Envelope e1;
	for (i = 0; i < num; i++)
	{
		if (ftr[i]->GetGeometry())
		{
			Envelope temp = ftr[i]->GetGeometry()->GetEnvelope();
			//e1.Union(&temp);
		}
	}

	//视图范围
	CRect rc;
	GetActiveView()->GetClientRect(&rc);
	PT_4D pt4ds[4];
	pt4ds[0].x = rc.left; pt4ds[0].y = rc.top;  pt4ds[0].z = 0;  pt4ds[0].yr = 0;
	pt4ds[1].x = rc.right; pt4ds[1].y = rc.top;  pt4ds[1].z = 0;  pt4ds[1].yr = 0;
	pt4ds[2].x = rc.right; pt4ds[2].y = rc.bottom;  pt4ds[2].z = 0;  pt4ds[2].yr = 0;
	pt4ds[3].x = rc.left; pt4ds[3].y = rc.bottom;  pt4ds[3].z = 0;  pt4ds[3].yr = 0;
	m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(pt4ds, 4);
	PT_3D pt3ds[4];
	for (i = 0; i < 4; i++)
	{
		pt3ds[i] = pt4ds[i].To3D();
	}
	Envelope e2;
	e2.CreateFromPts(pt3ds, 4);

	//e2.Union(&e1);

	Complete(e2, pLayer, pFtr);

	return FALSE;//绘制的线不添加
}

#include "DlgCommand2.h"

void CDlgAutoSurfaceCommand::Complete(Envelope &e, CFtrLayer *pLayer, CFeature *pCurveFtr)
{
	if (!pCurveFtr) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;

	CFtrArray oldftrs;//存放旧地物

	//绘制的线放入临时数据源
	CFeature *pCurveFtr1 = pCurveFtr->Clone();
	if (!pCurveFtr1)
	{
		return;
	}
	pCurveFtr1->SetAppFlag(0);
	oldftrs.Add(pCurveFtr1);

	CGeometry *pCurveGeo1 = pCurveFtr1->GetGeometry();
	if (!pCurveGeo1 || pCurveGeo1->GetDataPointSum() < 2)
		return;
	double z = pCurveFtr1->GetGeometry()->GetDataPoint(1).z;

	//外包e内的所有地物拷贝到新数据源里
	int i, j;
	for (i = 0; i < pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer)continue;
		if (pLayer->IsDeleted() || !pLayer->IsVisible())
			continue;

		if (StrFromResID(IDS_MARKLAYER_NAME).CompareNoCase(pLayer->GetName()) == 0)
		{
			continue;
		}

		int nObj = pLayer->GetObjectCount();
		for (j = 0; j < nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr || !pFtr->IsVisible() || !pFtr->GetGeometry())
				continue;

			CGeometry *pObj = pFtr->GetGeometry();
			if (!pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				continue;

			Envelope e1 = pObj->GetEnvelope();
			if (e.bIntersect(&e1))
			{
				oldftrs.Add(pFtr->Clone());
			}
		}
	}

	//调用非打断构面
	CTopoSurfaceNoBreakCommand cmd;
	cmd.Init(m_pEditor);
	CFtrArray newftrs;
	cmd.BuildSurface(oldftrs, newftrs);

	CArray<PT_3DEX, PT_3DEX> pts1;
	pCurveFtr->GetGeometry()->GetShape(pts1);

	int nObj = newftrs.GetSize();

	CUndoFtrs undo(m_pEditor, Name());
	for (i = 0; i < nObj; i++)
	{
		CFeature* pNewFtr = newftrs[i];

		//剔除与构造线不相交的面
		CArray<PT_3DEX, PT_3DEX> pts2;
		pNewFtr->GetGeometry()->GetShape(pts2);
		CArray<PtIntersect, PtIntersect> arr;
		int num2 = GetCurveIntersectCurve(pts1.GetData(), pts1.GetSize(), pts2.GetData(), pts2.GetSize(), arr);
		if (num2 <= 0) continue;

		//剔除被原面包含的面
		PT_3D opt;
		pNewFtr->GetGeometry()->GetCenter(NULL, &opt);
		BOOL bOldPart = FALSE;
		for (j = 0; j < oldftrs.GetSize(); j++)
		{
			CGeometry *pObj = oldftrs[j]->GetGeometry();
			if (!pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				continue;
			if (pObj->bPtIn(&opt))
			{
				bOldPart = TRUE;
				break;
			}
		}
		if (bOldPart) continue;

		int nCls = pNewFtr->GetGeometry()->GetClassType();
		CFeature *pFtr0 = pLayer->CreateDefaultFeature(pDS->GetScale(), nCls);
		pFtr0->SetID(OUID());
		pFtr0->SetAppFlag(0);

		CGeometry *pObj = pFtr0->GetGeometry();
		if (!pObj) continue;
		if (m_nHeightMode == 1)
		{
			for (int k = 0; k < pts2.GetSize(); k++)
			{
				pts2[k].z = z;
			}
		}
		if (!pObj->CreateShape(pts2.GetData(), pts2.GetSize()))
		{
			delete pFtr0;
			continue;
		}

		if (!m_pEditor->AddObject(pFtr0, pLayer->GetID()))
		{
			delete pFtr0;
			continue;
		}
		undo.AddNewFeature(FtrToHandle(pFtr0));
	}
	undo.Commit();

	for (i = 0; i < newftrs.GetSize(); i++)
	{
		delete newftrs[i];
	}
}


IMPLEMENT_DYNAMIC(CPartUpdateContourCommand, CEditCommand)
CPartUpdateContourCommand::CPartUpdateContourCommand()
{
	m_nStep = 0;
	strcat(m_strRegPath, "\\PartUpdateContour");
	m_pDS = NULL;
	m_min_away = 0.0;
	m_bSmooth = false;
}

CPartUpdateContourCommand::~CPartUpdateContourCommand()
{
	m_tin.Clear();
}

CString CPartUpdateContourCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_PARTUPDATECONTOUR);
}

void CPartUpdateContourCommand::Start()
{
	if (!m_pEditor)return;
	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();
	m_nStep = 0;
	//min_away = 0.0;
}

void CPartUpdateContourCommand::Abort()
{
	m_pEditor->CloseSelector();
	m_nStep = -1;
	m_bSmooth = false;
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CEditCommand::Abort();
}

void CPartUpdateContourCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("PartUpdateContour", Name());
	param->AddLayerNameParamEx("strLayerName", (LPCTSTR)m_strLayerName, StrFromResID(IDS_CONTOUR_LAYERNAME), NULL, LAYERPARAMITEM_LINE);
	param->AddParam("Min_Distance", m_min_away, StrFromResID(IDS_MIN_DISTANCE));
	param->AddParam("SmoothOrNot", (bool)(m_bSmooth), StrFromLocalResID(IDS_CMDPLANE_SMOOTH));
	param->AddParam("SmoothOrNotP", 'h', StrFromResID(IDS_CMDPLANE_SMOOTH));
}

void CPartUpdateContourCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "strLayerName", var))
	{
		m_strLayerName = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "Min_Distance", var))
	{
		m_min_away = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "SmoothOrNot", var))
	{
		m_bSmooth = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab, bInit);
}

void CPartUpdateContourCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strLayerName;
	tab.AddValue("strLayerName", &CVariantEx(var));
	var = m_min_away;
	tab.AddValue("Min_Distance", &CVariantEx(var));
	var = m_bSmooth;
	tab.AddValue("SmoothOrNot", &CVariantEx(var));
}

void CPartUpdateContourCommand::PtMove(PT_3D &pt)
{
	GrBuffer vbuf;
	int num = m_arrPts.GetSize();
	if (num == 1)
	{
		vbuf.BeginLineString(0, 0);
		vbuf.MoveTo(&m_arrPts[0]);
		PT_3DEX  Linept;
		Linept.x = m_arrPts[0].x;
		Linept.y = pt.y;
		vbuf.LineTo(&Linept);
		vbuf.LineTo(&pt);
		PT_3DEX  Linept2;
		Linept2.x = pt.x;
		Linept2.y = m_arrPts[0].y;
		vbuf.LineTo(&Linept2);
		vbuf.LineTo(&m_arrPts[0]);
		vbuf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
	}
	/*else if (m_arrPts.GetSize() >= 2)
	{
	vbuf.BeginLineString(0, 0);
	vbuf.MoveTo(&m_arrPts[0]);
	vbuf.LineTo(&pt);
	vbuf.MoveTo(&m_arrPts[num-1]);
	vbuf.LineTo(&pt);
	vbuf.End();
	m_pEditor->UpdateDrag(ud_SetVariantDrag, &vbuf);
	}*/

	CEditCommand::PtMove(pt);
}

void CPartUpdateContourCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		PT_3DEX expt(pt, penLine);
		m_arrPts.Add(expt);
		m_nStep = 1;
		return;
	}
	if (m_nStep==1)
		{
		//矩形
		PT_3DEX expt;
		expt.x = m_arrPts[0].x;
		expt.y = pt.y;
		m_arrPts.Add(expt);
		expt.x = pt.x;
		expt.y = pt.y;
		m_arrPts.Add(expt);
		expt.x = pt.x;
		expt.y = m_arrPts[0].y;
		m_arrPts.Add(expt);
			m_arrPts.Add(m_arrPts[0]);

			//更新等高线
		if (!m_pEditor)
			{
				Abort();
				return;
			}

			m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pDS = m_pEditor->GetDataSource();
			if (!m_pDS || m_strLayerName.IsEmpty())
			{
				Abort();
				return;
			}

			//m_arrPts.Add(m_arrPts[0]);//闭合

			Envelope e;
			e.CreateFromPts(m_arrPts.GetData(), m_arrPts.GetSize(), sizeof(PT_3DEX));
			e.Inflate(e.Width()/2, e.Height()/2, 0);

			m_pDS->GetFtrLayersByNameOrCode(m_strLayerName, m_arrContourLayers);

			
			if (!CreateTraiangle(e))
			{
				Abort();
				return;
			}

			//只让等高线图层可查询
			m_pDS->SaveAllQueryFlags(TRUE, FALSE);
			for (int i = 0; i < m_arrContourLayers.GetSize(); i++)
			{
				m_arrContourLayers[i]->SetAllowQuery(TRUE);
			}
			//更新等高线
			UpdateContours(e);

			m_pDS->RestoreAllQueryFlags();

			m_pEditor->RefreshView();
			Finish();
	}
	CEditCommand::PtClick(pt, flag);
}

BOOL CPartUpdateContourCommand::IsInContourLayer(CFtrLayer *pLayer)
{
	for (int i = 0; i < m_arrContourLayers.GetSize(); i++)
	{
		if (m_arrContourLayers[i] == pLayer)
			return TRUE;
	}
	return FALSE;
}

extern void KickoffSameGeoPts(CArray<MYPT_3D, MYPT_3D>& arr);
extern void KickoffSameGeo2Pts(CArray<GeoPoint, GeoPoint>& arr);
extern double GetDistanceFromPointToCurve(CArray<PT_3DEX, PT_3DEX>& pts, PT_3D& pt, PT_3D *retpt = NULL);

BOOL CPartUpdateContourCommand::CreateTraiangle(Envelope& e)
{
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if (!pDQ)
		return FALSE;

	pDQ->FindObjectInRect(e, NULL, FALSE, FALSE);

	int num = 0;
	const CPFeature *ftr = pDQ->GetFoundHandles(num);
	if (num <= 0)
		return FALSE;

	int k = 0;
	CArray<GeoPoint, GeoPoint> arrCur;
	PT_3DEX expt;
	GeoPoint geopt;
	vector<vector<PT_3DEX>> VectorPts;
	for (int ii = 0; ii < num; ii++)
	{
		CFeature *pFtr = ftr[ii];
		if (!pFtr) continue;
		CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) continue;
		BOOL bContour = IsInContourLayer(pLayer);
		CGeometry *pGeo = pFtr->GetGeometry();
		if (!pGeo)continue;
		if ((!bContour) && pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		{
			const CShapeLine *pShape = pGeo->GetShape();
			if (!pShape) continue;
			CArray<PT_3DEX, PT_3DEX> pts  ;
			pShape->GetKeyPts(pts);
			//pShape->GetPts(pts);
			int count = 0;
			for (int j = 0; j < pts.GetCount();j++)
			{
				expt = pts.GetAt(j);
				int mm = GraphAPI::GIsPtInRegion(expt, m_arrPts.GetData(), m_arrPts.GetSize());
				if (mm >= 0)
				{
					count++;
				}
				if (count > 2)
				{
					vector<PT_3DEX>  temp_vector;
					for (int j = 0; j < pts.GetCount();j++)
					{
						temp_vector.push_back(pts.GetAt(j));
					}
					VectorPts.push_back(temp_vector);
					break;
				}	
			}
		}
	}
	for (int i = 0; i < num; i++)
	{
		CFeature *pFtr = ftr[i];
		if (!pFtr) continue;
		CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) continue;
		BOOL bContour = IsInContourLayer(pLayer);
		CGeometry *pGeo = pFtr->GetGeometry();
		if (!pGeo)continue;
		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		{
			k++;
			expt = pGeo->GetDataPoint(0);
			geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;geopt.p = 0/*10002*/;
			if (!e.bPtIn(&expt))
				continue;
			m_AllPts.Add(geopt);
		}
		else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		{
			const CShapeLine *pShape = pGeo->GetShape();
			if (!pShape) continue;
			CArray<PT_3DEX, PT_3DEX> pts;
			pShape->GetKeyPts(pts);
			if (bContour)
			{
				int npt = pts.GetSize(), ret;
				for (int n = 0; n < pts.GetSize(); n++)
				{
					expt = pts[n];
					/*int mm = GraphAPI::GIsPtInRegion(expt, m_arrPts.GetData(), m_arrPts.GetSize());
					if (mm < 0)
					{
						continue;
					}*/
					if (!e.bPtIn(&expt))
						continue;
					int h = 0;
					for (; h < VectorPts.size(); h++)
					{
						vector<PT_3DEX> *pts2 = &VectorPts.at(h);
						CArray<PT_3DEX, PT_3DEX> CArraypts;
						for (int h = 0; h < pts2->size();h++)
						{
							CArraypts.Add(pts2->at(h));
						}
						double away = GetDistanceFromPointToCurve(CArraypts, expt);
						if (away <= m_min_away)
						{
							break;
						}
					}
					if (h == VectorPts.size())
					{
						k++;
						geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;
						geopt.p = 0/*(n == 0 ? 10002 : 10001)*/;
						m_AllPts.Add(geopt);
					}
				}
			}
			else
			{
				k++;
				int npt = pts.GetSize();
				arrCur.RemoveAll();
				for (int n = 0; n < npt; n++)
				{
					expt = pts[n];
					if (!e.bPtIn(&expt))
						continue;
					if (expt.pencode == penMove && n != 0)
					{
						k++;
					}
					geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;
					geopt.p = 0/*(n == 0 ? 10002 : 10001)*/;
					arrCur.Add(geopt);
				}
				KickoffSameGeo2Pts(arrCur);
				if (arrCur.GetSize()>0)m_AllPts.Append(arrCur);
			}
		}
	}
	PDOC(m_pEditor)->UpdateAllViews(NULL, hc_Refresh);
	return TRUE;
}

void CPartUpdateContourCommand::ConnectTinLine2(vector<IntersectionLine_struct> *Vector_intersection, vector<PT_3DEX>& NewPts, PT_3DEX& pt, bool flag, bool endflag, multimap<double, vector<PT_3DEX> >& Map_PointLine)
{
	int j = 0;
	for (; j < Vector_intersection->size(); j++)
	{
		IntersectionLine_struct  *intersectionLine = &Vector_intersection->at(j);
		int ii = 0;
		int count = intersectionLine->pts.size();
		bool flag2 = false;
		if (intersectionLine->bErgodic == false)
		{
			if (flag == false)
			{
				if (fabs(pt.x - intersectionLine->pts.at(0).x) < 0.0001 && fabs(pt.y - intersectionLine->pts.at(0).y) < 0.0001)
				{
					//NewPts.pop_back();
					flag2 = true;
					intersectionLine->bErgodic = true;
					int jj = 0;
					for (; jj < NewPts.size(); jj++)
					{
						if (fabs(NewPts.at(jj).x - intersectionLine->pts.at(1).x) < 0.0001 && fabs(NewPts.at(jj).y - intersectionLine->pts.at(1).y) < 0.0001)
						{
							break;
						}
					}
					if (jj == NewPts.size())
					{
						endflag = true;
						NewPts.push_back(intersectionLine->pts.at(1));
						PT_3DEX  endPt = intersectionLine->pts.at(1);
						ConnectTinLine(Vector_intersection, NewPts, endPt, false, Map_PointLine);
					}
					//有环
					else
					{
						PT_3DEX  endPt = NewPts.at(NewPts.size() - 1);
						if (jj == 0)
						{
							NewPts.push_back(intersectionLine->pts.at(1));
							return;
						}
						vector<PT_3DEX> temp_vector;
						for (int g = jj; g < NewPts.size(); g++)
						{
							temp_vector.push_back(NewPts.at(g));
						}
						temp_vector.push_back(NewPts.at(jj));
						Map_PointLine.insert(pair<double, vector<PT_3DEX>>(NewPts.at(jj).z, temp_vector));
						//递归去环
						ConnectTinLine2(Vector_intersection, NewPts, endPt, false, endflag, Map_PointLine);
					}
					break;
				}
				if (fabs(pt.x - intersectionLine->pts.at(1).x) < 0.0001 && fabs(pt.y - intersectionLine->pts.at(1).y) < 0.0001)
				{
					//NewPts.pop_back();
					flag2 = true;
					int jjj = 0;
					intersectionLine->bErgodic = true;
					for (; jjj < NewPts.size(); jjj++)
					{
						if (fabs(NewPts.at(jjj).x - intersectionLine->pts.at(0).x) < 0.0001 && fabs(NewPts.at(jjj).y - intersectionLine->pts.at(0).y) < 0.0001)
						{
							break;
						}
					}
					if (jjj == NewPts.size())
					{
						endflag = true;
						NewPts.push_back(intersectionLine->pts.at(0));
						PT_3DEX  endPt = intersectionLine->pts.at(0);
						ConnectTinLine(Vector_intersection, NewPts, endPt, false, Map_PointLine);
					}
					//有环
					else
					{
						PT_3DEX  endPt = NewPts.at(NewPts.size() - 1);
						//递归去环
						if (jjj == 0)
						{
							NewPts.push_back(intersectionLine->pts.at(0));
							return;
						}
						vector<PT_3DEX> temp_vector;
						for (int g = jjj; g < NewPts.size(); g++)
						{
							temp_vector.push_back(NewPts.at(g));
						}
						temp_vector.push_back(NewPts.at(jjj));
						Map_PointLine.insert(pair<double, vector<PT_3DEX>>(NewPts.at(jjj).z, temp_vector));
						ConnectTinLine2(Vector_intersection, NewPts, endPt, false, endflag, Map_PointLine);
					}
					break;
				}
			}
			else
			{
				if (fabs(pt.x - intersectionLine->pts.at(0).x) < 0.0001 && fabs(pt.y - intersectionLine->pts.at(0).y) < 0.0001)
				{
					//vector<PT_3DEX> ::iterator it = NewPts.begin();
					//NewPts.erase(it);
					flag2 = true;
					intersectionLine->bErgodic = true;
					int jj = 0;
					for (; jj < NewPts.size(); jj++)
					{
						if (fabs(NewPts[jj].x - intersectionLine->pts.at(1).x) < 0.0001 && fabs(NewPts[jj].y - intersectionLine->pts.at(1).y) < 0.0001)
						{
							break;
						}

					}
					if (jj == NewPts.size())
					{
						endflag = true;
						vector<PT_3DEX>::iterator  it = NewPts.begin();
						NewPts.insert(it, intersectionLine->pts.at(1));
						PT_3DEX  startPt = NewPts.at(0);
						ConnectTinLine(Vector_intersection, NewPts, startPt, true, Map_PointLine);
					}
					//有环
					else
					{
						PT_3DEX  startPt = NewPts.at(0);
						//大环，完整的等高线，但是如果是从第二点出发的完整等高线就有问题了
						if (jj == NewPts.size() - 1)
						{
							vector<PT_3DEX>::iterator  it = NewPts.begin();
							NewPts.insert(it, intersectionLine->pts.at(1));
							return;
						}
						vector<PT_3DEX> temp_vector;
						for (int g = 0; g <= jj; g++)
						{
							temp_vector.push_back(NewPts.at(g));
						}
						temp_vector.insert(temp_vector.begin(), NewPts.at(jj));
						Map_PointLine.insert(pair<double, vector<PT_3DEX>>(NewPts.at(jj).z, temp_vector));
						ConnectTinLine2(Vector_intersection, NewPts, startPt, true, endflag, Map_PointLine);
					}
					break;
				}
				if (fabs(pt.x - intersectionLine->pts.at(1).x) < 0.0001 && fabs(pt.y - intersectionLine->pts.at(1).y) < 0.0001)
				{
					flag2 = true;
					intersectionLine->bErgodic = true;
					int j2 = 0;
					for (; j2 < NewPts.size(); j2++)
					{
						if (fabs(NewPts[j2].x - intersectionLine->pts.at(0).x) < 0.0001 && fabs(NewPts[j2].y - intersectionLine->pts.at(0).y) < 0.0001)
						{
							break;
						}
					}
					if (j2 == NewPts.size())
					{
						endflag = true;
						vector<PT_3DEX>::iterator  it = NewPts.begin();
						NewPts.insert(it, intersectionLine->pts.at(0));
						PT_3DEX  startPt = NewPts.at(0);
						ConnectTinLine(Vector_intersection, NewPts, startPt, true, Map_PointLine);
					}
					//有环
					else
					{
						PT_3DEX  startPt = NewPts.at(0);
						if (j2 == NewPts.size() - 1)
						{
							vector<PT_3DEX>::iterator  it = NewPts.begin();
							NewPts.insert(it, intersectionLine->pts.at(0));
							return;
						}
						vector<PT_3DEX> temp_vector;
						temp_vector.clear();
						for (int g = 0; g <= j2; g++)
						{
							temp_vector.push_back(NewPts.at(g));
						}
						temp_vector.insert(temp_vector.begin(), NewPts.at(j2));
						Map_PointLine.insert(pair<double, vector<PT_3DEX>>(NewPts.at(j2).z, temp_vector));
						ConnectTinLine2(Vector_intersection, NewPts, startPt, true, endflag, Map_PointLine);
					}
					break;
				}
			}
		}
	}
	if (endflag == true)
	{
		endflag = false;
		m_tempVector = NewPts;
	}

	if (j == Vector_intersection->size() && flag == false)
	{
		vector<PT_3DEX> ::iterator  it = NewPts.begin() + NewPts.size() - 1;
		NewPts.erase(it);
		if (NewPts.size() == 0)
		{
			NewPts = m_tempVector;
			return;
		}
		PT_3DEX  endPt = NewPts.at(NewPts.size() - 1);
		//递归去环
		ConnectTinLine2(Vector_intersection, NewPts, endPt, false, endflag, Map_PointLine);
		//ConnectTinLine(Vector_intersection, NewPts, endPt, false, Map_PointLine);
	}
	if (j == Vector_intersection->size() && flag == true)
	{
		vector<PT_3DEX> ::iterator  it = NewPts.begin();
		NewPts.erase(it);
		if (NewPts.size() == 0)
		{
			NewPts = m_tempVector;
			return;
		}
		PT_3DEX  endPt = NewPts.at(0);
		//递归去环
		ConnectTinLine2(Vector_intersection, NewPts, endPt, true, endflag, Map_PointLine);
	}
}






void CPartUpdateContourCommand::ConnectTinLine(vector<IntersectionLine_struct> *Vector_intersection, vector<PT_3DEX>& NewPts, PT_3DEX& pt, bool flag, multimap<double, vector<PT_3DEX> >& Map_PointLine)
{
	int j = 0;
	for (; j < Vector_intersection->size(); j++)
	{
		IntersectionLine_struct  *intersectionLine = &Vector_intersection->at(j);
		int ii = 0;
		int count = intersectionLine->pts.size();
		bool flag2 = false;
		if (intersectionLine->bErgodic == false)
		{
			if (flag == false)
			{
				if (fabs(pt.x - intersectionLine->pts.at(0).x) <= 0.0001 && fabs(pt.y - intersectionLine->pts.at(0).y) <= 0.0001)
				{
					flag2 = true;
					intersectionLine->bErgodic = true;
					int jj = 0;
					for (; jj < NewPts.size(); jj++)
					{
						if (fabs(NewPts.at(jj).x - intersectionLine->pts.at(1).x) < 0.0001 && fabs(NewPts.at(jj).y - intersectionLine->pts.at(1).y) < 0.0001)
						{
							break;
						}
					}
					if (jj == NewPts.size())
					{
						NewPts.push_back(intersectionLine->pts.at(1));
						PT_3DEX  endPt = intersectionLine->pts.at(1);
						ConnectTinLine(Vector_intersection, NewPts, endPt, false, Map_PointLine);
					}
					//有环
					else
					{
						vector<PT_3DEX>  New_vector;
						PT_3DEX  endPt = NewPts.at(NewPts.size() - 1);
						//jj=0 说明有可能是个大环，也就是一条完整的等高线
						if (jj == 0)
						{
							NewPts.push_back(intersectionLine->pts.at(1));
							//Map_PointLine.insert(pair < double, vector<PT_3DEX>>(NewPts.at(jj).z, NewPts));
							return;
						}
						for (int g = jj; g < NewPts.size(); g++)
						{
							New_vector.push_back(NewPts.at(g));
						}
						New_vector.push_back(NewPts.at(jj));
						Map_PointLine.insert(pair < double, vector<PT_3DEX>>(NewPts.at(jj).z, New_vector));
						//递归去环
						ConnectTinLine2(Vector_intersection, NewPts, endPt, false, true, Map_PointLine);
					}
					break;
				}
				if (fabs(pt.x - intersectionLine->pts.at(1).x) <= 0.0001 && fabs(pt.y - intersectionLine->pts.at(1).y) <= 0.0001)
				{
					flag2 = true;
					intersectionLine->bErgodic = true;
					int jj = 0;
					for (; jj < NewPts.size(); jj++)
					{
						if (fabs(NewPts.at(jj).x - intersectionLine->pts.at(0).x) <= 0.0001 && fabs(NewPts.at(jj).y - intersectionLine->pts.at(0).y) <= 0.0001)
						{
							break;
						}
					}
					if (jj == NewPts.size())
					{
						NewPts.push_back(intersectionLine->pts.at(0));
						PT_3DEX  endPt = intersectionLine->pts.at(0);
						ConnectTinLine(Vector_intersection, NewPts, endPt, false, Map_PointLine);
					}
					//有环
					else
					{
						PT_3DEX  endPt = NewPts.at(NewPts.size() - 1);
						//jj=0 说明有可能是个大环，也就是一条完整的等高线
						if (jj == 0)
						{
							NewPts.push_back(intersectionLine->pts.at(0));
							return;
						}
						vector<PT_3DEX>  New_vector;
						for (int g = jj; g < NewPts.size(); g++)
						{
							New_vector.push_back(NewPts.at(g));
						}
						New_vector.push_back(NewPts.at(jj));
						Map_PointLine.insert(pair < double, vector<PT_3DEX>>(NewPts.at(jj).z, New_vector));
						//递归去环
						ConnectTinLine2(Vector_intersection, NewPts, endPt, false, true, Map_PointLine);
					}
					
					break;
				}
			}
			else
			{
				if (fabs(pt.x - intersectionLine->pts.at(0).x) < 0.0001 && fabs(pt.y - intersectionLine->pts.at(0).y) < 0.0001)
				{
					flag2 = true;
					intersectionLine->bErgodic = true;
					int jj = 0;
					for (; jj < NewPts.size(); jj++)
					{
						if (fabs(NewPts.at(jj).x - intersectionLine->pts.at(1).x) < 0.0001 && fabs(NewPts.at(jj).y - intersectionLine->pts.at(1).y) < 0.0001)
						{
							break;
						}
					}
					if (jj == NewPts.size())
					{
						vector<PT_3DEX>::iterator  it = NewPts.begin();
						NewPts.insert(it, intersectionLine->pts.at(1));
						PT_3DEX  startPt = NewPts.at(0);
						ConnectTinLine(Vector_intersection, NewPts, startPt, true, Map_PointLine);
					}
					//有环
					else
					{
						PT_3DEX  startPt = NewPts.at(0);
						//jj=NewPts.size() - 1  说明有可能是个大环，也就是一条完整的等高线
						if (jj == NewPts.size() - 1)
						{
							vector<PT_3DEX>::iterator  it = NewPts.begin();
							NewPts.insert(it, intersectionLine->pts.at(1));
							return;
						}
						vector<PT_3DEX>  New_vector;
						for (int g = 0; g <= jj ; g++)
						{
							New_vector.push_back(NewPts.at(g));
						}
						New_vector.insert(New_vector.begin(),NewPts.at(jj));
						Map_PointLine.insert(pair < double, vector<PT_3DEX>>(NewPts.at(jj).z, New_vector));
						ConnectTinLine2(Vector_intersection, NewPts, startPt, true, true, Map_PointLine);
					}
					break;
				}
				if (fabs(pt.x - intersectionLine->pts.at(1).x) < 0.0001 && fabs(pt.y - intersectionLine->pts.at(1).y) < 0.0001)
				{
					flag2 = true;
					int j2 = 0;
					intersectionLine->bErgodic = true;
					for (; j2 < NewPts.size(); j2++)
					{
						if (fabs(NewPts.at(j2).x - intersectionLine->pts.at(0).x) < 0.0001 && fabs(NewPts.at(j2).y - intersectionLine->pts.at(0).y) < 0.0001)
						{
							break;
						}
					}
					if (j2 == NewPts.size())
					{
						vector<PT_3DEX>::iterator  it = NewPts.begin();
						NewPts.insert(it, intersectionLine->pts.at(0));
						PT_3DEX  startPt = NewPts.at(0);
						ConnectTinLine(Vector_intersection, NewPts, startPt, true, Map_PointLine);
					}
					//有环
					else
					{
						PT_3DEX  startPt = NewPts.at(0);
						if (j2 == NewPts.size() - 1)
						{
							vector<PT_3DEX>::iterator  it = NewPts.begin();
							NewPts.insert(it, intersectionLine->pts.at(0));
							return;
						}

						vector<PT_3DEX>  New_vector;
						for (int g = 0; g <= j2; g++)
						{
							New_vector.push_back(NewPts.at(g));
						}
						New_vector.insert(New_vector.begin(), NewPts.at(j2));
						Map_PointLine.insert(pair < double, vector<PT_3DEX>>(NewPts.at(j2).z, New_vector));
						ConnectTinLine2(Vector_intersection, NewPts, startPt, true, true, Map_PointLine);
					}
					break;
				}
			}
		}
	}
	/*if (j == Vector_intersection->size() && flag == false)
	{


	}*/
}

extern void SmoothLines(PT_3DEX *pts, int num, double toler, CArray<PT_3DEX, PT_3DEX>& arrPts);

void CPartUpdateContourCommand::UpdateContours(Envelope &e)
{
	//CUndoBatchAction batchUndo(m_pEditor, Name());
	int nsel = 0;
	((CDlgDoc*)m_pEditor)->GetDataQuery()->FindObjectInRect(e, NULL, FALSE, FALSE);
	const CPFeature *ContourFtrbuf = ((CDlgDoc*)m_pEditor)->GetDataQuery()->GetFoundHandles(nsel);
	CGeoBuilderContour build;
	GeoLine *pLine;
	build.m_lfDemInterval = FLT_MAX;
	bool bRet = build.Init((GeoPoint*)m_AllPts.GetData(), m_AllPts.GetSize(), 1);
	int nContours = build.GetContourNumber();
	int i = 0;
	multimap<double, vector<PT_3DEX>>  map_contour, NewMap_contour, TempMap_contour;
	GProgressStart(nContours);
	for( i=0; i<nContours; i++)
	{
		GProgressStep();
		vector<PT_3DEX> expt;
		expt.clear();
		pLine = NULL;
		if( !build.GetLine(i,pLine) || pLine==NULL )continue;
		if( pLine->iPtNum<=1 )continue;
		int j;
		for( j=0; j<pLine->iPtNum; j++)
		{
			PT_3DEX pt;
			pt.x = pLine->gp[j].x; pt.y = pLine->gp[j].y; pt.z = pLine->gp[j].z;
			pt.pencode = penLine;
			expt.push_back(pt);
		}
		map_contour.insert(pair<double,vector<PT_3DEX>>(expt.at(0).z,expt));
     }
	GProgressEnd();
#if 1
	//排除掉不与矩形相交的等高线
	multimap<double, vector<PT_3DEX>>::iterator  it_map = map_contour.begin();
	for (; it_map != map_contour.end(); /*it_map++*/)
	{
		vector<PT_3DEX>  vectorPt = it_map->second;
		vector<int> vector_int;
		for (int g = 0; g < vectorPt.size();g++)
		{
			int judge = GraphAPI::GIsPtInRegion(vectorPt.at(g), m_arrPts.GetData(), m_arrPts.GetSize());
			if (judge >= 0)
			{
				vector_int.push_back(judge);
				if (vector_int.size() >= 2)
				{
					break;
				}
			}
		}
		if (vector_int.size() < 2)
		{
			it_map = map_contour.erase(it_map);
		}
		else
		{
			it_map++;
		}
	}
	//找到在矩形区域内的等高线
	multimap<double, vector<PT_3DEX>>::iterator  it_mapContour = map_contour.begin();
	for (; it_mapContour != map_contour.end(); it_mapContour++)
	{
		vector<PT_3DEX>  *vectorPt = &it_mapContour->second;
		vector<int> vector_int;
		for (int g = 0; g < vectorPt->size();g++)
		{
			int judge = GraphAPI::GIsPtInRegion(vectorPt->at(g), m_arrPts.GetData(), m_arrPts.GetSize());
			if (  g == vectorPt->size() - 1 )
			{
				if (judge < 0)
				{
					vectorPt->at(g).wid = 1.0;
				}
				break;
			}
#if 1
			int judge2 = GraphAPI::GIsPtInRegion(vectorPt->at(g + 1), m_arrPts.GetData(), m_arrPts.GetSize());
			//外部
			if (judge < 0 && judge2 < 0 && g != vectorPt->size() - 2)
			{
				vectorPt->at(g).wid = 1.0;
				continue;
			}
			else if (judge < 0 && judge2 < 0 && g == vectorPt->size() - 2)
			{
				vectorPt->at(g).wid = 1.0;
				vectorPt->at(g + 1).wid = 1.0;
				break;
			}
			else if (judge < 0 && (judge2 == 0 || judge2 == 1))
			{
				vectorPt->at(g).wid = 1.0;
				continue;
			}
			else if (judge > 0 && judge2 < 0 /*&& g == vectorPt->size() - 2*/)
			{
				g++;
				if (g == vectorPt->size() - 1)
				{
					/*int judge3 = GraphAPI::GIsPtInRegion(vectorPt->at(g), m_arrPts.GetData(), m_arrPts.GetSize());
					if (judge3 < 0)
					{
						vectorPt->at(g).wid = 1.0;
					}*/
					break;
				}
				continue;
			}
			else if ((judge == 0 || judge == 1) && judge2 < 0)
			{
				vectorPt->at(g + 1).wid = 1.0;
				continue;
			}


			//入边界
			//if (judge < 0 && judge2 == 2)
			//{
			//	continue;
			//}
			////内部
			//if (judge >= 0 && judge2 >= 0)
			//{
			//	continue;
			//}
			////出边界
			//if (judge == 2 && judge2 < 0)
			//{
			//	continue;
			//}
#endif
		}
	}
 	it_mapContour = map_contour.begin();
	for (; it_mapContour != map_contour.end(); it_mapContour++)
	{
		vector<PT_3DEX> *pts = &it_mapContour->second;
		bool flag = false;
		vector<PT_3DEX>  new_Contour;
		new_Contour.clear();
		for (int j = 0; j < pts->size();j++)
		{
			if (pts->at(j).wid == 0)
			{
				new_Contour.push_back(pts->at(j));
				if (j == pts->size() - 1 && new_Contour.size() > 1)
				{
					TempMap_contour.insert(pair<double, vector<PT_3DEX>>(new_Contour.at(0).z, new_Contour));
					break;
				}
			}
			else
			{
				if (new_Contour.size() > 1)
				{
					TempMap_contour.insert(pair<double, vector<PT_3DEX>>(new_Contour.at(0).z, new_Contour));
					new_Contour.clear();
				}
				new_Contour.clear();
            }
		}
    }
	it_mapContour = TempMap_contour.begin();
	for (; it_mapContour != TempMap_contour.end(); it_mapContour++)
	{
		
		multimap<double, vector<PT_3DEX>>::iterator  it_mapContourSec = it_mapContour;
		it_mapContourSec++;
		double first_z = it_mapContour->first;
		vector<PT_3DEX> *firstPts = &it_mapContour->second;
		it_mapContour++;
		if (it_mapContour == TempMap_contour.end())
		{
			NewMap_contour.insert(pair<double, vector<PT_3DEX>>(first_z, *firstPts));
			break;
		}
		else
		{
			it_mapContour--;
		}
		for (; it_mapContourSec != TempMap_contour.end(); it_mapContourSec++)
		{
			double second_z = it_mapContourSec->first;
			if (fabs(second_z - first_z) > 0.0001)
			{
				multimap<double, vector<PT_3DEX>> ::iterator  it_mapContour = NewMap_contour.begin();
				int count = 0;
				for (; it_mapContour != NewMap_contour.end(); it_mapContour++)
				{
					double  z = it_mapContour->first;
					vector<PT_3DEX>  pts = it_mapContour->second;

					/*if (pts.size() == 1)
					{
					continue;
					}*/

					if (fabs(z - first_z) <= 0.0001/* && pts.size() == firstPts->size()*/)
					{
						if (fabs(pts.at(0).x - firstPts->at(0).x) <= 0.0001 && fabs(pts.at(0).y - firstPts->at(0).y) <= 0.0001)
						{
							break;
						}
						if (fabs(pts.at(pts.size() - 1).x - firstPts->at(firstPts->size() - 1).x) <= 0.0001 && fabs(pts.at(pts.size() - 1).y - firstPts->at(firstPts->size() - 1).y) <= 0.0001)
						{
							break;
						}
					}
					count++;
				}
				if (count == NewMap_contour.size())
				{
					NewMap_contour.insert(pair<double, vector<PT_3DEX>>(first_z, *firstPts));
				}
				break;
			}
			else
			{
				vector<PT_3DEX> *secondPts = &it_mapContourSec->second;

				/*if (secondPts->size() == 1)
				{
				continue;
				}*/

				PT_3DEX  firstPt     = firstPts->at(0);
				PT_3DEX  firstPtEnd  = firstPts->at(firstPts->size() - 1 );
				PT_3DEX  secondPt    = secondPts->at(0);
				PT_3DEX  secondPtEnd = secondPts->at(secondPts->size() - 1);
				//头尾相连
				if (fabs(firstPt.x - secondPtEnd.x) <= 0.001 && fabs(firstPt.y - secondPtEnd.y) <= 0.001)
				{
					vector<PT_3DEX>  NewPoints;

					for (int j = 0; j < secondPts->size(); j++)
					{
						NewPoints.push_back(secondPts->at(j));
					}
					for (int j = 1; j < firstPts->size(); j++)
					{
						NewPoints.push_back(firstPts->at(j));
					}
					NewMap_contour.insert(pair<double, vector<PT_3DEX>>(first_z, NewPoints));
					it_mapContourSec = TempMap_contour.erase(it_mapContourSec);
					it_mapContourSec--;
					
				}
				//尾头相连
				else if (fabs(firstPtEnd.x - secondPt.x) <= 0.001 && fabs(firstPtEnd.y - secondPt.y) <= 0.001)
				{
					vector<PT_3DEX>  NewPoints;
					for (int j = 0; j < firstPts->size(); j++)
					{
						NewPoints.push_back(firstPts->at(j));
					}
					for (int j = 1; j < secondPts->size() ; j++)
					{
						NewPoints.push_back(secondPts->at(j));
					}
					NewMap_contour.insert(pair<double, vector<PT_3DEX>>(first_z, NewPoints));
					it_mapContourSec = TempMap_contour.erase(it_mapContourSec);
					it_mapContourSec--;
				}
				else
				{
					//multimap<double, vector<PT_3DEX>> ::iterator  it_mapContour = NewMap_contour.begin();
					//int count = 0;
					//for (; it_mapContour != NewMap_contour.end(); it_mapContour++)
					//{
					//	double  z = it_mapContour->first;
					//	vector<PT_3DEX>  pts = it_mapContour->second;
					//	if (fabs(z - first_z) <= 0.0001 /*&& pts.size() == firstPts->size()*/)
					//	{
					//		if (fabs(pts.at(0).x - firstPts->at(0).x) < 0.0001 && fabs(pts.at(0).y - firstPts->at(0).y) < 0.0001)
					//		{
					//			break;
					//		}
					//	}
					//	count++;
					//}
					//if (count == NewMap_contour.size())
					//{
					//	NewMap_contour.insert(pair<double, vector<PT_3DEX>>(first_z, *firstPts));
					//}	
				}
				//不存在头头或是尾尾相连
            }
        }
	}
	CUndoFtrs  undoFtr(m_pEditor, Name());
	//更新等高线
	for (int i = 0; i < nsel; i++)
	{
		CFeature *ftr = ContourFtrbuf[i];
		CGeometry *geoCurve = ftr->GetGeometry();
		if (!geoCurve) continue;
		const CShapeLine* shapeLine = geoCurve->GetShape();
		if (!shapeLine) continue;
		CArray<PT_3DEX, PT_3DEX> CurPts, CopyCurPts;
		CArray<PT_3DEX, PT_3DEX> NewPts;
		NewPts.RemoveAll();
		CurPts.RemoveAll();
		shapeLine->GetPts(CurPts);
		CopyCurPts.RemoveAll();
		CopyCurPts.Append(CurPts);
		vector<int> vectJudge;
		vectJudge.clear();
		for (int n = 0; n < CurPts.GetSize(); n++)
		{
			int mm = GraphAPI::GIsPtInRegion(CurPts[n], m_arrPts.GetData(), m_arrPts.GetSize());
			if (mm >= 0)
			{
				vectJudge.push_back(mm);
			}
		}
		if (vectJudge.size() <= 1)
		{
			continue;
		}
		int layerid = ftr->GetLayerID();
		long color = 255;
		double contour_z = CurPts.GetAt(0).z;
		int first = 0, count_in = 0;
		bool flag = true;
		int mm = GraphAPI::GIsPtInRegion(CurPts[0], m_arrPts.GetData(), m_arrPts.GetSize());
		if (mm == 2)
		{

			CurPts.RemoveAt(0);
			for (int j = 0; j < CurPts.GetCount(); j++)
			{
				mm = GraphAPI::GIsPtInRegion(CurPts[j], m_arrPts.GetData(), m_arrPts.GetSize());
				if (mm == 2 /*&& flag_in == false*/)
				{
					//只有一个点在外面的情况剔除
					if (j > 0 && CurPts.GetAt(j - 1).wid == 1 && flag == true)
					{
						//CurPts.GetAt(j - 1).wid = 0;
						CurPts.RemoveAt(j);
						CurPts.RemoveAt(j - 1);
						j--;
						j--;
						first = 0;
						count_in = 0;
						flag = false;
						continue;
					}
					if (count_in == 1)
					{
						CurPts.GetAt(j - 1).wid = 1;
						//标识两个连续的点，他们没有断开
						if (j - 1 >= 1 && CurPts.GetAt(j - 2).wid == 1)
						{
							CurPts.GetAt(j - 2).type = ptSpecial;
						}
						count_in = 0;
						flag = false;
					}
					CurPts.RemoveAt(j);
					j--;
					first = 0;
				}
				else if (first == 0)
				{
					CurPts.GetAt(j).wid = 1;
					first = 1;;
					count_in = 1;
					flag = true;
				}
			}
		}
		else /*if (mm == -1)*/
		{
			first = 0;
			count_in = 0;
			for (int j = 1; j < CurPts.GetCount(); j++)
			{
				mm = GraphAPI::GIsPtInRegion(CurPts[j], m_arrPts.GetData(), m_arrPts.GetSize());
				if (mm == 2 /*&& flag_in == false*/)
				{
					//只有一个点在外面的情况剔除
					if (j > 0 && CurPts.GetAt(j - 1).wid == 1 && flag == true)
					{
						//CurPts.GetAt(j - 1).wid = 0;
						CurPts.RemoveAt(j);
						CurPts.RemoveAt(j - 1);
						j--;
						j--;
						first = 1;
						count_in = 1;
						flag = false;
						continue;
					}

					if (count_in == 0)
					{
						CurPts.GetAt(j - 1).wid = 1;
						if (j - 1 >= 1 && CurPts.GetAt(j - 2).wid == 1)
						{
							CurPts.GetAt(j - 2).type = ptSpecial;
						}
						count_in = 1;
						flag = false;
					}
					CurPts.RemoveAt(j);
					j--;
					first = 1;
				}
				else if (first == 1)
				{
					CurPts.GetAt(j).wid = 1;
					first = 0;;
					count_in = 0;
					flag = true;
				}
			}
		}
		//不让成环
		/*if (CurPts.GetCount() >= 3)
		{
			if (fabs(CurPts.GetAt(0).x - CurPts.GetAt(CurPts.GetCount() - 1).x) <= 0.0001 &&
				fabs(CurPts.GetAt(0).y - CurPts.GetAt(CurPts.GetCount() - 1).y) <= 0.0001)
			{
				CurPts.RemoveAt(CurPts.GetCount() - 1);
			}
	    }*/
		
		/*if (fabs(CurPts.GetAt(0).x - CurPts.GetAt(CurPts.GetCount() - 1 ).x) < 0.0001 &&
			fabs(CurPts.GetAt(0).y - CurPts.GetAt(CurPts.GetCount() - 1).y) < 0.0001)
		{
			CurPts.RemoveAt(CurPts.GetCount() - 1);
		}*/
			/*if (CurPts.GetSize() < 2  )
			{
			CurPts.RemoveAll();
			}*/
		bool   bflag = false;
		multimap<double, vector<PT_3DEX>>::iterator  it_contour = NewMap_contour.begin();
		for (; it_contour != NewMap_contour.end();)
		{
			NewPts.RemoveAll();
 			double  z = it_contour->first;
 			if (fabs(z - contour_z) <= 0.0001)
			{
				bflag = true;
				vector<PT_3DEX> temp_vector = it_contour->second;
				for (int f = 0; f < temp_vector.size(); f++)
				{
					NewPts.Add(temp_vector.at(f));
				}
				if (NewPts.GetSize() < 2)
				{

					it_contour++;
					NewPts.RemoveAll();
					continue;
				}
				if (fabs(temp_vector.at(0).x - temp_vector.at(temp_vector.size() - 1).x) <= 0.0001 &&
					fabs(temp_vector.at(0).y - temp_vector.at(temp_vector.size() - 1).y) <= 0.0001 && temp_vector.size() > 1)
				{
					CFeature *pFtrHoop = new CFeature();
					pFtrHoop->SetLayerID(layerid);
					CGeoCurve* GeoCurveHoop = new CGeoCurve();
					GeoCurveHoop->CreateShape(NewPts.GetData(), NewPts.GetCount());
					GeoCurveHoop->SetColor(color);
					pFtrHoop->SetGeometry(GeoCurveHoop);
					m_pEditor->AddObject(pFtrHoop, layerid);
					undoFtr.AddNewFeature(FtrToHandle(pFtrHoop));
					//batchUndo.AddAction(&undoFtr);
					it_contour++;
					if (it_contour == NewMap_contour.end())
					{
						it_contour--;
						NewMap_contour.erase(it_contour);
						NewPts.RemoveAll();
						break;
					}
					continue;
				}
#if 1
				vector<PT_3DEX>  vectorPts;
				vectorPts.clear();
				
				if (CurPts.GetCount() == 0 /*&& fabs(contour_z - NewPts.GetAt(0).z) <= 0.0001*/)
				{
					for (int j = 0; j < NewPts.GetCount(); j++)
					{
						CurPts.Add(NewPts[j]);
					}
					//break;
				}
				else if (CurPts.GetCount() > 0)
				{
					int DISTANCE = 10;
					bool  flag = false;
					PT_3DEX  firstPt = NewPts.GetAt(0);
					PT_3DEX  endPt = NewPts.GetAt(NewPts.GetCount() - 1);
					//找到具体哪个位置
					//multimap<double, int>map_distance;
					//for (int g = 0; g < CurPts.GetCount(); g++)
					//{
					//	if (CurPts.GetAt(g).wid == 1)
					//	{
					//		//开头插
					//		if (g == 0 && CurPts.GetAt(g + 1).wid == 0)
					//		{
					//			double Startaway = sqrt(pow(CurPts.GetAt(0).x - firstPt.x, 2) + pow(CurPts.GetAt(0).y - firstPt.y, 2));
					//			double Endaway = sqrt(pow(CurPts.GetAt(0).x - endPt.x, 2)   + pow(CurPts.GetAt(0).y - endPt.y, 2));
					//			if (Startaway <= Endaway)
					//			{
					//				map_distance.insert(pair<double, int>(Startaway, g));
					//			}
					//		}
					//		else if (g == CurPts.GetCount() - 1)
					//		{
					//			double Startaway = sqrt(pow(CurPts.GetAt(g).x - firstPt.x, 2) + pow(CurPts.GetAt(g).y - firstPt.y, 2));
					//			double Endaway = sqrt(pow(CurPts.GetAt(g).x - endPt.x, 2) + pow(CurPts.GetAt(g).y - endPt.y, 2));
					//			if (Startaway <= Endaway)
					//			{
					//				map_distance.insert(pair<double, int>(Startaway, g));
					//			}
					//		}
					//	}
					//}

					for (int g = 0; g < CurPts.GetCount();g++)
					{
						if (CurPts.GetAt(g).wid == 1)
						{
							PT_3DEX  pt = CurPts.GetAt(g);
							double away  =  sqrt(pow(firstPt.x - pt.x, 2) + pow(firstPt.y - pt.y, 2));
							double away2 =  sqrt(pow(endPt.x - pt.x, 2)   + pow(endPt.y - pt.y, 2));
							//结尾
							if (g == CurPts.GetCount() - 1  )
							{
								
								if (away < DISTANCE && away <= away2 )
								{
									CurPts.GetAt(g).wid = 0;
									for (int f = 0; f < NewPts.GetCount();f++)
									{
										CurPts.Add(NewPts.GetAt(f));
									}
									//终点以后还有等高线
									CurPts.GetAt(CurPts.GetCount() - 1).wid = 1;
									flag = true;
									break;
								}
								else if (away2 < DISTANCE && away2 <= away )
								{
									CurPts.GetAt(g).wid = 0;
									for (int f = NewPts.GetCount() - 1 ; f >= 0; f--)
									{
										CurPts.Add(NewPts.GetAt(f));
									}
									//终点以后还有等高线
									CurPts.GetAt(CurPts.GetCount() - 1).wid = 1;
									flag = true;
									break;
								}
						     }
							 //开始处
							else if ((g == 0 && CurPts.GetAt(1).wid == 0) /*|| (g == 0 && CurPts.GetCount() == 2) */ )
							{
								//
								
								double otherFirstAway = 100;
								double otherEndAway = 100;
								//防止离起点太近的但不应该连接的新等高线
								int s = 1;
								multimap<double, int> map_FirstAway,map_EndAway;
								while (s < CurPts.GetSize())
								{
									if (CurPts.GetAt(s).wid == 1)
									{
										otherFirstAway = sqrt(pow(CurPts.GetAt(s).x - firstPt.x, 2) + pow(CurPts.GetAt(s).y - firstPt.y, 2));
										otherEndAway   = sqrt(pow(CurPts.GetAt(s).x - endPt.x, 2)   + pow(CurPts.GetAt(s).y - endPt.y, 2));
										map_FirstAway.insert(pair<double, int>(otherFirstAway, s));
								        map_EndAway.insert(pair<double, int>(otherEndAway, s));	
									}
									s++;
								}
								multimap<double, int>::iterator it_Firstmap = map_FirstAway.begin();
								double min_Firstaway = it_Firstmap->first;
								multimap<double, int>::iterator it_Endmap = map_EndAway.begin();
								double min_Endaway = it_Endmap->first;
								if (away < DISTANCE  && away <= away2   && away < min_Firstaway /*&& away < min_Endaway*/)
								{
									CurPts.GetAt(0).wid == 0;
									for (int f = 0; f < NewPts.GetCount(); f++)
									{
										CurPts.InsertAt(0, NewPts.GetAt(f));
									}
									flag = true;
									break;
								}
								else if (away2 < DISTANCE && away2 < away && away2 < min_Endaway /*&& away2 < min_Firstaway*/)
								{
									CurPts.GetAt(0).wid == 0;
									for (int f = NewPts.GetCount() - 1; f >= 0; f--)
									{
										CurPts.InsertAt(0, NewPts.GetAt(f));
									}
									flag = true;
									break;
								}
							}
							else if ((g == 0 && CurPts.GetAt(1).wid == 1 && CurPts.GetAt(0).type == ptSpecial))
							{
								double FirstPt_distance = sqrt(pow(CurPts.GetAt(1).x - firstPt.x, 2) + pow(CurPts.GetAt(1).y - firstPt.y, 2));
								double EndPt_distance =   sqrt(pow(CurPts.GetAt(1).x   - endPt.x, 2) + pow(CurPts.GetAt(1).y   - endPt.y, 2));
								//同向或异向
								if ((away > FirstPt_distance  && away < away2 && FirstPt_distance < EndPt_distance) && 
									(away > EndPt_distance    && away < away2 && EndPt_distance < FirstPt_distance) &&
									(away2 > EndPt_distance   && away2 < away && EndPt_distance < FirstPt_distance) &&
									(away2 > FirstPt_distance && away2 < away && FirstPt_distance < EndPt_distance))
								{
									continue;
								}
								//同向
								else if (away <= FirstPt_distance  && away < away2 && FirstPt_distance < EndPt_distance)
								{
									CurPts.GetAt(0).wid == 0;
									for (int f = 0; f < NewPts.GetCount(); f++)
									{
										CurPts.InsertAt(0, NewPts.GetAt(f));
									}
									flag = true;
									break;
								}
								else if (away <= EndPt_distance  && away < away2 && EndPt_distance < FirstPt_distance)
								{
									CurPts.GetAt(0).wid == 0;
									for (int f = 0; f < NewPts.GetCount(); f++)
									{
										CurPts.InsertAt(0, NewPts.GetAt(f));
									}
									flag = true;
									break;
								}
								else if (away2 < EndPt_distance  && away2 < away && EndPt_distance < FirstPt_distance)
								{
									CurPts.GetAt(0).wid == 0;
									for (int f = NewPts.GetCount() - 1; f >= 0; f--)
									{
										CurPts.InsertAt(0, NewPts.GetAt(f));
									}
									flag = true;
									break;
								}
								else if (away2 < FirstPt_distance && away2 < away && FirstPt_distance < EndPt_distance)
								{
									CurPts.GetAt(0).wid == 0;
									for (int f = NewPts.GetCount() - 1; f >= 0; f--)
									{
										CurPts.InsertAt(0, NewPts.GetAt(f));
									}
									flag = true;
									break;
								}
							}
							//中间
							else
							{
								PT_3DEX		Endpt = CurPts.GetAt(g + 1);
								if (Endpt.wid == 0.0)
								{
									continue;
								}
								double Endaway  = sqrt(pow(firstPt.x - Endpt.x, 2) + pow(firstPt.y - Endpt.y, 2));
								double Endaway2 = sqrt(pow(endPt.x -   Endpt.x, 2) + pow(endPt.y   - Endpt.y, 2));
								if (away < DISTANCE && Endaway2 < DISTANCE &&  away <= Endaway && Endaway2 <= away2)
								{
									int pos = g + 1;
									CurPts.GetAt(g).wid == 0;
									CurPts.GetAt(g + 1 ).wid == 0;
									for (int h = 0; h < NewPts.GetCount();h++)
									{

										CurPts.InsertAt(pos++, NewPts.GetAt(h));
									}
									flag = true;
									break;
								}
								else if (away2 < DISTANCE && Endaway <DISTANCE && away2 < Endaway2 && Endaway < away)
								{
									int pos = g + 1;
									CurPts.GetAt(g).wid == 0;
									CurPts.GetAt(g + 1).wid == 0;
									for (int h = 0; h < NewPts.GetCount(); h++)
									{

										CurPts.InsertAt(pos, NewPts.GetAt(h));
									}
									flag = true;
									break;
								}
							}
						}
					}

					if (flag == true)
					{
						for (int d = 0; d < CurPts.GetSize() - 1; d++)
						{
							if (fabs(CurPts.GetAt(d).x - CurPts.GetAt(d + 1).x) <= 0.0001 && fabs(CurPts.GetAt(d).y - CurPts.GetAt(d + 1).y) <= 0.0001)
							{
								CurPts.RemoveAt(d);
								d--;
							}
						}
					}

				}
#endif
				//break;
			}
			it_contour++;
		}
		if (bflag == false)
		{
		  continue;
		}
		if (CurPts.GetCount() == 0)
		{
			m_pEditor->DeleteObject(FtrToHandle(ftr), FALSE);
			undoFtr.AddOldFeature(FtrToHandle(ftr));
			//batchUndo.AddAction(&undoFtr);
			continue;
		}
		m_pEditor->DeleteObject(FtrToHandle(ftr), FALSE);
		undoFtr.AddOldFeature(FtrToHandle(ftr));
		CFeature *pFtr = new CFeature();
		pFtr->SetID(OUID());
		pFtr->SetLayerID(layerid);
		CGeoCurve* GeoCurve = new CGeoCurve();
		CArray<PT_3DEX, PT_3DEX> SmoothPts;
		for (int h = 0; h < CurPts.GetCount(); h++)
		{
			CurPts.GetAt(h).wid = 0.0;
		}
		PT_3DEX firstPT = CurPts.GetAt(0);
		PT_3DEX endPT = CurPts.GetAt(CurPts.GetCount() - 1);
		if (fabs(firstPT.x - endPT.x) <= 0.01 && fabs(firstPT.y - endPT.y) <= 0.01)
		{
			CurPts.Add(firstPT);
		}
		if (m_bSmooth == true)
		{
			SmoothLines(CurPts.GetData(), CurPts.GetCount(), 0.2000, SmoothPts);
			GeoCurve->CreateShape(SmoothPts.GetData(), SmoothPts.GetCount());
		}
		else
		{
			GeoCurve->CreateShape(CurPts.GetData(), CurPts.GetCount());
		}
		
		
		//GeoCurve->CreateShape(CurPts.GetData(), CurPts.GetCount());
		GeoCurve->SetColor(color);
		pFtr->SetGeometry(GeoCurve);
		m_pEditor->AddObject(pFtr, layerid);
		undoFtr.AddNewFeature(FtrToHandle(pFtr));
		//batchUndo.AddAction(&undoFtr);
	}
#endif
	//CUndoFtrs  undoFtr(m_pEditor, Name());
#if 0
	for (int i = 0; i < nsel; i++)
	{
		CFeature *ftr = ContourFtrbuf[i];
		CGeometry *geoCurve = ftr->GetGeometry();
		if (!geoCurve) continue;
		const CShapeLine* shapeLine = geoCurve->GetShape();
		if (!shapeLine) continue;
		CArray<PT_3DEX, PT_3DEX> pts;
		CArray<PT_3DEX, PT_3DEX> NewPts;
		shapeLine->GetPts(pts);
		vector<int> vectJudge;
		vectJudge.clear();
		//找到在区域内的等高线
		for (int n = 0; n < pts.GetSize(); n++)
		{
			int mm = GraphAPI::GIsPtInRegion(pts[n], m_arrPts.GetData(), m_arrPts.GetSize());
			if (mm >= 0)
			{
				vectJudge.push_back(mm);
			}
		}
		if (vectJudge.size() <= 1)
		{
			continue;
		}
		double  contour_Z = pts.GetAt(0).z;
		/*if (fabs(60 - contour_Z) > 0.0001 && fabs(54 - contour_Z) > 0.0001)
		{
			continue;
		}
*/
		int layerid = ftr->GetLayerID();
		multimap<double, vector<PT_3DEX> > ::iterator  muit = NewMap_contour.begin();
		//以生成的等高线为主     

		for (; muit != NewMap_contour.end(); muit++)
		{
			double z = muit->first;
			vector<PT_3DEX>  New_pts = muit->second;
			if (fabs(z - contour_Z) <= 0.0001)
			{
				NewPts.RemoveAll();
				for (int g = 0; g < New_pts.size(); g++)
				{
					NewPts.Add(New_pts.at(g));
				}
				CFeature *newFtr = new CFeature();
				CGeoCurve *curve = new CGeoCurve();
				curve->CreateShape(NewPts.GetData(), NewPts.GetCount());
				curve->SetColor(125);
				newFtr->SetGeometry(curve);
				m_pEditor->AddObject(newFtr, layerid);
				undoFtr.AddNewFeature(FtrToHandle(newFtr));
				//batchUndo.AddAction(&undoFtr);
			}
		}
	}
#endif
	undoFtr.Commit();
	PDOC(m_pEditor)->UpdateAllViews(NULL, hc_Refresh);
}