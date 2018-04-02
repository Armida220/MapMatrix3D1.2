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
#include "DlgSetXYZ.h"
#include "..\CORE\viewer\EditBase\res\resource.h"
#include "VectorView_new.h"
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
#include "PolygonWork.h"
#include "PolygonBooleanHander.h"
#include "GeoSurfaceBooleanHandle.h"
#include "StereoView.h"
#include "MapDecorator.h"
#include "GeoPoint.h"
#include "UVSModify.h"

#include "mm3dPrj\OsgbView.h"

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
extern BOOL convertStringToStrArray(LPCTSTR str,CStringArray &arr);
extern int GetAccessType(CDataSourceEx *pDS);

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

BOOL TrimRZero(CString &str)
{
	int  nIndex = str.Find('.');
	if (nIndex >= 0)
	{
		str.TrimRight('0');
		if (str.GetLength()==nIndex+1)
		{
			str = str.Left(nIndex);
			if (str.IsEmpty())
				str = '0';
		}
	}
	
	return TRUE;
}


// line为线段 
template<class T>
bool GetIntersectPos(T *pData1, int npt, T *line, double *t1, double *t2)
{
	int i, ok = 0;
	double pos, tol1, tol2, x, y;
	
	pos = -1;
	for( i=0; i<npt-1; i++)
	{
		if( GraphAPI::GGetLineIntersectLineSeg(pData1[i].x,pData1[i].y,
			pData1[i+1].x,pData1[i+1].y,line[0].x,line[0].y,
			line[1].x,line[1].y,&x,&y,&tol1,&tol2) )
		{
			if (t1)
			{
				*t1 = i+tol1;
			}
			
			if (t2)
			{
				*t2 = tol2;
			}
			ok = 1;
		}
	}
	
	return ok;
}

static BOOL IsObjInRegion(CGeometry* pObj, PT_3D *polys, int num)
{
	CArray<PT_3DEX, PT_3DEX> pts;
	pObj->GetShape(pts);
	int nPt = pts.GetSize();
	nPt = GraphAPI::GKickoffSamePoints(pts.GetData(), nPt);

	PT_3D expt;

	if (nPt == 1)
	{
		expt.x = pts[0].x;
		expt.y = pts[0].y;
		expt.z = 0.0;
		int r = GraphAPI::GIsPtInRegion(expt, polys, num);
		if (r == 2)return TRUE;
		else if (r == -1)return FALSE;
		else return -1;
	}
	else if (nPt == 2)//只判断中点
	{
		expt.x = (pts[0].x + pts[1].x)*0.5;
		expt.y = (pts[0].y + pts[1].y)*0.5;
		expt.z = 0.0;
		int r = GraphAPI::GIsPtInRegion(expt, polys, num);
		if (r == 2)return TRUE;
		else if (r == -1)return FALSE;
		else return -1;
	}
	else
	{
		expt.x = pts[nPt / 3].x;
		expt.y = pts[nPt / 3].y;
		expt.z = 0.0;
		int r1 = GraphAPI::GIsPtInRegion(expt, polys, num);

		expt.x = pts[nPt / 3 * 2].x;
		expt.y = pts[nPt / 3 * 2].y;
		expt.z = 0.0;
		int r2 = GraphAPI::GIsPtInRegion(expt, polys, num);

		if (r1 == 2 && r2 >= 0)
		{
			return TRUE;
		}
		else if (r2 == 2 && r1 >= 0)
		{
			return TRUE;
		}
		else if (r1 == -1 && r2 <= 1)
		{
			return FALSE;
		}
		else if (r2 == -1 && r1 <= 1)
		{
			return FALSE;
		}
		else
		{
			return -1;
		}
	}

	return -1;
}


//////////////////////////////////////////////////////////////////////
// CSeparateSurfaceSymbolsCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSeparateSurfaceSymbolsCommand,CEditCommand)

CSeparateSurfaceSymbolsCommand::CSeparateSurfaceSymbolsCommand()
{
	m_nStep = -1;
	m_bKeepOld = FALSE;
	m_nSepMode = 0;
	strcat(m_strRegPath,"\\SeparateSurfaceSymbols");
}

CSeparateSurfaceSymbolsCommand::~CSeparateSurfaceSymbolsCommand()
{
}

CString CSeparateSurfaceSymbolsCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SEPERATESURFACESYMBOLS);
}

void CSeparateSurfaceSymbolsCommand::Start()
{
	if( !m_pEditor )return;

	m_bKeepOld = FALSE;
	m_nSepMode = 0;

	CEditCommand::Start();

	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num>0 )
	{
		m_nStep = 0;
	
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}		
}


BOOL CSeparateSurfaceSymbolsCommand::SeparateSymbols_new(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrObjs)
{
	//一般化打散
	if( m_nSepMode==0 )
	{
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

		if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			return FALSE;

		if( strlen(pFtr->GetGeometry()->GetSymbolName())>0 )
			return FALSE;

		return pDS->ExplodeSymbolsExceptSurface(pFtr,pLayer,arrObjs);
	}
	return FALSE;
}



void CSeparateSurfaceSymbolsCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CUndoFtrs undo(m_pEditor,Name());		
		
		int nsel;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		for( int i=nsel-1; i>=0; i--)
		{
		
			CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(handles[i]));
			
			CFtrArray arrObjs;
			if( SeparateSymbols_new(HandleToFtr(handles[i]),pLayer,arrObjs) )
			{								
				for( int j=0; j<arrObjs.GetSize(); j++)
				{
					((CFeature*)arrObjs.GetAt(j))->SetPurpose(FTR_MARK);
					m_pEditor->AddObject((CFeature*)arrObjs.GetAt(j),pLayer->GetID());
					PDOC(m_pEditor)->GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(HandleToFtr(handles[i]),(CFeature*)arrObjs.GetAt(j));
					undo.AddNewFeature(FtrToHandle((CFeature*)arrObjs.GetAt(j)));				
				}
				
				//删除原来的对象
				if (!m_bKeepOld)
				{
					m_pEditor->DeleteObject(handles[i]);
					undo.AddOldFeature(handles[i]);
				}
				
			}
		
		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	CEditCommand::PtClick(pt, flag);
}


void CSeparateSurfaceSymbolsCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}

void CSeparateSurfaceSymbolsCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("SeparateSurfaceSymbolsCommand",Name());
	
	param->BeginOptionParam(PF_KEEPOLD,StrFromResID(IDS_CMDPLANE_KEEPOLD));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bKeepOld);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bKeepOld);
	param->EndOptionParam();

}

void CSeparateSurfaceSymbolsCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_KEEPOLD,var) )
	{
		m_bKeepOld = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
}

void CSeparateSurfaceSymbolsCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	var = (bool)(m_bKeepOld);
	tab.AddValue(PF_KEEPOLD,&CVariantEx(var));
}



//////////////////////////////////////////////////////////////////////
// CCopyFtrsBetweenLayerCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCopyFtrsBetweenLayerCommand,CEditCommand)

CCopyFtrsBetweenLayerCommand::CCopyFtrsBetweenLayerCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\CopyFtrsBetweenLayer");
}

CCopyFtrsBetweenLayerCommand::~CCopyFtrsBetweenLayerCommand()
{
}

CString CCopyFtrsBetweenLayerCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COPYFTRSLAYER);
}

void CCopyFtrsBetweenLayerCommand::Start()
{
	if( !m_pEditor )return;

	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	
	CEditCommand::Start();
}

void CCopyFtrsBetweenLayerCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
		
	}
	
	if( m_nStep==1 )
	{
		CDlgDataSource *pDS = GETDS(m_pEditor);
		if (!pDS) return;
		
		CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());

		CFtrLayer *pLayer = pDS->GetFtrLayer(m_strRetLay);	

		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);

		CUndoFtrs undo(m_pEditor,Name());

		for (int i=0; i<num; i++)
		{
			CFeature *pFtr = (CFeature*)handles[i];
			if (!pFtr) continue;
			
			CFeature *pNewFtr = NULL;
			if (pLayer)
			{
				pNewFtr = pLayer->CreateDefaultFeature(pDS->GetScale(),pFtr->GetGeometry()->GetClassType());
				if (!pNewFtr) continue;

				CValueTable table;
				table.BeginAddValueItem();
				pFtr->WriteTo(table);
				table.EndAddValueItem();

				pNewFtr->ReadFrom(table);

				CGeometry *pGeo = pNewFtr->GetGeometry();
				
				float wid = 0;
				if (pScheme)
				{
					wid = pScheme->GetLayerDefineLineWidth(pLayer->GetName());					
				}
				
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					((CGeoCurveBase*)pGeo)->m_fLineWidth = wid;
				}

				pNewFtr->SetID(OUID());
				m_pEditor->AddObject(pNewFtr,pLayer->GetID());			
				
			}
			else
			{
				pNewFtr = pFtr->Clone();
				pNewFtr->SetID(OUID());

				m_pEditor->AddObject(pNewFtr,pDS->GetFtrLayerOfObject(pFtr)->GetID());
			}

			undo.AddNewFeature(FtrToHandle(pNewFtr));	
			
		}

		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		
		
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt, flag);
}


void CCopyFtrsBetweenLayerCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}

void CCopyFtrsBetweenLayerCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CopyFtrsBetweenLayer",StrFromLocalResID(IDS_CMDNAME_COPYFTRSLAYER));
	
	param->AddLayerNameParam(PF_COPYFTRS_LAYERNAME,(LPCTSTR)m_strRetLay,StrFromResID(IDS_CMDPLANE_RETLAY));
	
}

void CCopyFtrsBetweenLayerCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_COPYFTRS_LAYERNAME,var) )
	{
		m_strRetLay = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

}

void CCopyFtrsBetweenLayerCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strRetLay;
	tab.AddValue(PF_COPYFTRS_LAYERNAME,&CVariantEx(var));
}




//////////////////////////////////////////////////////////////////////
// CCopyFtrsInsideBridgeCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCopyFtrsInsideBridgeCommand,CEditCommand)

CCopyFtrsInsideBridgeCommand::CCopyFtrsInsideBridgeCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\CopyFtrsInsideBridge");
}

CCopyFtrsInsideBridgeCommand::~CCopyFtrsInsideBridgeCommand()
{
}

CString CCopyFtrsInsideBridgeCommand::Name()
{ 
	return StrFromResID(IDS_COPYOBJ_INSIDEBRIDGE);
}

void CCopyFtrsInsideBridgeCommand::Start()
{
	if( !m_pEditor )return;
	
	CEditCommand::Start();

	m_pEditor->CloseSelector();
}

void CCopyFtrsInsideBridgeCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		CDlgDataSource *pDS = GETDS(m_pEditor);
		if (!pDS) return;

		CFtrLayerArray layers;

		if(m_strRetLay.GetLength()>0)
			pDS->GetFtrLayersByNameOrCode(m_strRetLay,layers);
		else
		{
			int nLay = pDS->GetFtrLayerCount();
			for(int i=0; i<nLay; i++)
			{
				CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
				if(!pLayer)
					continue;
				if(!pLayer->IsVisible() || pLayer->IsLocked())
					continue;

				layers.Add(pLayer);

			}
		}

		CUndoFtrs undo(m_pEditor,Name());

		for(int i=0; i<layers.GetSize(); i++)
		{
			CFtrLayer *pLayer = layers[i];	

			if(!pLayer->IsVisible() || pLayer->IsLocked())
				continue;

			if(pLayer->GetEditableObjsCount()<=0)
				continue;

			int nObj = pLayer->GetObjectCount();
			for(int j=0; j<nObj; j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if(!pFtr)continue;

				if(pFtr->GetAppFlag()==1)
					continue;

				CFeature *pFtr1 = NULL, *pFtr2 = NULL;
				GetEndFtrs(pFtr,pFtr1,pFtr2);

				if(pFtr1==NULL || pFtr2==NULL)
					continue;

				CFtrLayer *pLayer1 = pDS->GetFtrLayerOfObject(pFtr1);
				CFeature *pNewFtr = pLayer1->CreateDefaultFeature(pDS->GetScale(),pFtr->GetGeometry()->GetClassType());

				CArray<PT_3DEX,PT_3DEX> arrPts;
				pFtr->GetGeometry()->GetShape(arrPts);
				pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());

				pNewFtr->SetAppFlag(1);

				m_pEditor->AddObject(pNewFtr,pLayer1->GetID());	
				undo.AddNewFeature(FtrToHandle(pNewFtr));	
			}
		}	

		for(i=0; i<undo.arrNewHandles.GetSize(); i++)
		{
			HandleToFtr(undo.arrNewHandles[i])->SetAppFlag(0);
		}

		CString strMsg;
		strMsg.Format(IDS_PROCESS_OBJ_NUM,undo.arrNewHandles.GetSize());
		
		GOutPut(strMsg);	

		undo.Commit();
		
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt, flag);
}


void CCopyFtrsInsideBridgeCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}

void CCopyFtrsInsideBridgeCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CopyFtrsInsideBridge",Name());
	
	param->AddLayerNameParam(PF_COPYFTRS_LAYERNAME,(LPCTSTR)m_strRetLay,StrFromResID(IDS_LAYER_BRIDGE));
	
}

void CCopyFtrsInsideBridgeCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_COPYFTRS_LAYERNAME,var) )
	{
		m_strRetLay = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

}

void CCopyFtrsInsideBridgeCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strRetLay;
	tab.AddValue(PF_COPYFTRS_LAYERNAME,&CVariantEx(var));
}


void CCopyFtrsInsideBridgeCommand::GetEndFtrs(CFeature *pFtr, CFeature *& pFtr1, CFeature *& pFtr2)
{
	CGeometry *pGeo = pFtr->GetGeometry();
	if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		return;

	CGeoCurveBase *pCurve = (CGeoCurveBase*)pGeo;
	if(pCurve->IsClosed())
		return;

	int npt = pCurve->GetDataPointSum();
	if( npt<2 )
		return;

	CArray<PT_3DEX,PT_3DEX> arrPts;
	pCurve->GetShape(arrPts);
	PT_3DEX pt1 = arrPts[0];
	PT_3DEX pt2 = arrPts[npt-1];

	double toler = GraphAPI::g_lfDisTolerance;

	Envelope e(pt1.x-toler,pt1.x+toler,pt1.y-toler,pt1.y+toler);
	m_pEditor->GetDataQuery()->FindObjectInRect(e,NULL);

	int num = 0;
	const CPFeature *ftrs = m_pEditor->GetDataQuery()->GetFoundHandles(num);
	if(num>1)
	{
		for(int i=0; i<num; i++)
		{
			if(pFtr==ftrs[i])
				continue;

			CGeometry *pGeo1 = ftrs[i]->GetGeometry();
			if( !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				continue;
			
			CGeoCurveBase *pCurve1 = (CGeoCurveBase*)pGeo1;
			if(pCurve1->IsClosed())
				continue;
			
			int npt1 = pCurve1->GetDataPointSum();
			if( npt1<2 )
				continue;

			CArray<PT_3DEX,PT_3DEX> arrPts1;
			pCurve1->GetShape(arrPts1);
			PT_3DEX pt3 = arrPts1[0];
			PT_3DEX pt4 = arrPts1[npt1-1];

			if(e.bPtIn(&pt3) || e.bPtIn(&pt4))
			{
				pFtr1 = ftrs[i];
				break;
			}
		}
	}

	e = Envelope(pt2.x-toler,pt2.x+toler,pt2.y-toler,pt2.y+toler);
	m_pEditor->GetDataQuery()->FindObjectInRect(e,NULL);
	
	num = 0;
	ftrs = m_pEditor->GetDataQuery()->GetFoundHandles(num);
	if(num>1)
	{
		for(int i=0; i<num; i++)
		{
			if(pFtr==ftrs[i])
				continue;
			
			CGeometry *pGeo1 = ftrs[i]->GetGeometry();
			if( !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				continue;
			
			CGeoCurveBase *pCurve1 = (CGeoCurveBase*)pGeo1;
			if(pCurve1->IsClosed())
				continue;
			
			int npt1 = pCurve1->GetDataPointSum();
			if( npt1<2 )
				continue;
			
			CArray<PT_3DEX,PT_3DEX> arrPts1;
			pCurve1->GetShape(arrPts1);
			PT_3DEX pt3 = arrPts1[0];
			PT_3DEX pt4 = arrPts1[npt1-1];
			
			if(e.bPtIn(&pt3) || e.bPtIn(&pt4))
			{
				pFtr2 = ftrs[i];
				break;
			}
		}
	}

}



//////////////////////////////////////////////////////////////////////
// CCopyFtrsInsideBridgeCommand_One Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCopyFtrsInsideBridgeCommand_One,CEditCommand)

CCopyFtrsInsideBridgeCommand_One::CCopyFtrsInsideBridgeCommand_One()
{
	m_nStep = -1;
	m_hBridgeFtr = NULL;
}

CCopyFtrsInsideBridgeCommand_One::~CCopyFtrsInsideBridgeCommand_One()
{
	
}



CString CCopyFtrsInsideBridgeCommand_One::Name()
{ 
	return StrFromResID(IDS_COPYOBJ_INSIDEBRIDGE_ONE);
}

void CCopyFtrsInsideBridgeCommand_One::Start()
{
	if( !m_pEditor )return;
	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->OpenSelector(SELMODE_MULTI);

	PromptString(StrFromResID(IDS_CMDTIP_SELECTBRIDGE));		
}


void CCopyFtrsInsideBridgeCommand_One::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( (flag&SELSTAT_POINTSEL)==0 && (flag&SELSTAT_DRAGSEL)==0 )
			return;
		int num;
		m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num!=1 )
			return;		
		
		m_hBridgeFtr = m_pEditor->GetSelection()->GetLastSelectedObj();
		if( !HandleToFtr(m_hBridgeFtr)->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
		{
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			PromptString(StrFromResID(IDS_CMDTIP_SELECTCURVE));
			return;
		}
		
		GotoState(PROCSTATE_PROCESSING);
		m_nStep = 1;
		
		//开始选择目标地物
		PromptString(StrFromResID(IDS_CMDTIP_SELECTENDOBJ));
	}
	else if( m_nStep==1 )
	{
		if( (flag&SELSTAT_MULTISEL)!=0 && (flag&SELSTAT_POINTSEL)==0 && (flag&SELSTAT_DRAGSEL_RESTART)==0 )
			return;
		
		CUndoFtrs undo(m_pEditor,Name());

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		if (!pDS) return;
	
		CFeature *pFtr = HandleToFtr(m_hBridgeFtr);
		CGeometry *pGeo = pFtr->GetGeometry();
		CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pFtrLayer) return;

		int num, processednum = 0;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num==2 )
		{
			FTR_HANDLE h2 = m_pEditor->GetSelection()->GetLastSelectedObj();
			if( m_hBridgeFtr!=h2 )
			{
				if( !HandleToFtr(h2)->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				{
					m_pEditor->GetSelection()->DeselectObj(h2);
					m_pEditor->OnSelectChanged();
					PromptString(StrFromResID(IDS_CMDTIP_SELECTCURVE));
					return;
				}

				CFtrLayer *pLayer1 = pDS->GetFtrLayerOfObject(HandleToFtr(h2));
				CFeature *pNewFtr = pLayer1->CreateDefaultFeature(pDS->GetScale(),pFtr->GetGeometry()->GetClassType());
				
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pFtr->GetGeometry()->GetShape(arrPts);
				pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
				
				m_pEditor->AddObject(pNewFtr,pLayer1->GetID());	
				undo.AddNewFeature(FtrToHandle(pNewFtr));	

				processednum++;
			}
		}

		if (undo.arrNewHandles.GetSize() > 0)
		{
			undo.Commit();
		}

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();

		Finish();
	}

	CEditCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// CRiverChangeCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CRiverChangeCommand,CEditCommand)

CRiverChangeCommand::CRiverChangeCommand()
{
	m_nStep = -1;
	m_fStartWid = 1;
	m_fEndWid = 5;
	strcat(m_strRegPath,"\\RiverChange");
}

CRiverChangeCommand::~CRiverChangeCommand()
{
}

CString CRiverChangeCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_RIVERCHANGE);
}

void CRiverChangeCommand::Start()
{
	if( !m_pEditor )return;

	m_fStartWid = 1;
	m_fEndWid = 5;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	
	m_pEditor->OpenSelector(SELMODE_SINGLE);

	GOutPut(StrFromResID(IDS_CMDTIP_SELSTARTRIVER));
}

void CRiverChangeCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag,FALSE) )return;

		m_pFtr = HandleToFtr(m_pEditor->GetSelection()->GetLastSelectedObj());
		CGeometry *pObj = NULL;
		if(m_pFtr)pObj = m_pFtr->GetGeometry();
		if( !pObj )return;
		
		if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		{
			Abort();
			return;
		}

		m_ptClick = pt;

		EditStepOne();		
	}	
	else if( m_nStep==1 )
	{
		CDlgDataSource *pDS = GETDS(m_pEditor);
		if (!pDS) return;
		
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		if (num != 1) 
		{
			Abort();
			return;
		}

		CUndoModifyProperties undo(m_pEditor,Name());

		//for (int i=0; i<num; i++)
		{
			CFeature *pFtr = m_pFtr;

			CGeometry *pGeo = pFtr->GetGeometry();

			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);
			
			int num0 = pts.GetSize();
			
			m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
			
			undo.arrHandles.Add(FtrToHandle(pFtr));
			undo.oldVT.BeginAddValueItem();
			pFtr->WriteTo(undo.oldVT);
			undo.oldVT.EndAddValueItem();
			
			const CShapeLine *pShape = pGeo->GetShape();
			double len = pShape->GetLength();

			double len1 = pShape->GetLength(&m_ptClick);
			
			BOOL bStartFirst = TRUE;
			if (len1 > len/2)
			{
				bStartFirst = FALSE;
			}

			double wid = fabs(m_fEndWid - m_fStartWid);

			double avg = wid/len;

			if (bStartFirst)
			{
				pts[0].wid = m_fStartWid;
				pts[num0-1].wid = m_fEndWid;
			}
			else
			{
				pts[0].wid = m_fEndWid;
				pts[num0-1].wid = m_fStartWid;
			}
			
			for (int j=1; j<num0-1; j++)
			{
				double len0 = pShape->GetLength(&pts[j]);

				if (bStartFirst)
				{
					if (m_fStartWid < m_fEndWid)
					{
						pts[j].wid = m_fStartWid + len0 * avg;
					}
					else
					{
						pts[j].wid = m_fStartWid - len0 * avg;
					}
				}
				else
				{
					if (m_fStartWid < m_fEndWid)
					{
						pts[j].wid = m_fEndWid - len0 * avg;
					}
					else
					{
						pts[j].wid = m_fEndWid + len0 * avg;
					}
				}

				
			}
			
			if ( !pGeo->CreateShape(pts.GetData(),pts.GetSize()) ) 
			{
				Abort();
				return;
			}
			pGeo->SetSymbolName("*");
			
			m_pEditor->RestoreObject(FtrToHandle(pFtr));
			
			undo.newVT.BeginAddValueItem();
			pFtr->WriteTo(undo.newVT);
			undo.newVT.EndAddValueItem();	
			
		}

		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		
		
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt, flag);
}


void CRiverChangeCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}

void CRiverChangeCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("RiverChange",StrFromLocalResID(IDS_CMDNAME_RIVERCHANGE));
	
	param->AddParam(PF_RIVERCHANGE_STARTWID,(double)m_fStartWid, StrFromResID(IDS_CMDPLANE_STARTWID));

	param->AddParam(PF_RIVERCHANGE_ENDWID,(double)m_fEndWid, StrFromResID(IDS_CMDPLANE_ENDWID));
	
}

void CRiverChangeCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_RIVERCHANGE_STARTWID,var) )
	{
		m_fStartWid = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_RIVERCHANGE_ENDWID,var) )
	{
		m_fEndWid = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

}

void CRiverChangeCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;

	var = (double)m_fStartWid;
	tab.AddValue(PF_RIVERCHANGE_STARTWID,&CVariantEx(var));

	var = (double)m_fEndWid;
	tab.AddValue(PF_RIVERCHANGE_ENDWID,&CVariantEx(var));
}

//////////////////////////////////////////////////////////////////////
// CRiverFillCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CRiverFillCommand,CEditCommand)

CRiverFillCommand::CRiverFillCommand()
{
	m_nStep = -1;
	m_nColor = RGB(0,0,255);
	m_pDrawProc = NULL;
	m_pGeoCurve = NULL;
	strcat(m_strRegPath,"\\RiverFill");
}

CRiverFillCommand::~CRiverFillCommand()
{
}

CString CRiverFillCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_RIVERFILL);
}

void CRiverFillCommand::Start()
{
	if( !m_pEditor )return;

	m_nColor = RGB(0,0,255);
	m_arrPts.RemoveAll();
	m_nFillType = 0;
	m_pDrawProc = NULL;
	m_pGeoCurve = NULL;

	m_pEditor->DeselectAll();
	
	CEditCommand::Start();

	if (m_nFillType == 1)
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if( !m_pDrawProc )return;
		m_pDrawProc->Init(m_pEditor);
		UpdateParams(FALSE);
		m_pDrawProc->Start();
		m_pGeoCurve = new CGeoCurve;
		if(!m_pGeoCurve) 
		{
			Abort();
			return ;
		}
		m_pGeoCurve->SetColor(m_nColor);
		m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
		m_pDrawProc->m_bClosed = TRUE;
		m_pEditor->CloseSelector();
		return;
	}
}

void CRiverFillCommand::PtReset(PT_3D &pt)
{	
	if( m_pDrawProc && !IsProcOver(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
	}
	else
		Abort();
}

CProcedure *CRiverFillCommand::GetActiveSonProc(int nMsgType)
{
	if (m_nFillType==1&&(nMsgType==msgEnd||nMsgType==msgPtMove/*||nMsgType==msgPtReset*/))
	{
		return m_pDrawProc;
	}
	return NULL;
}

void CRiverFillCommand::OnSonEnd(CProcedure *son)
{
	if( m_nFillType==1 )
	{
		if( m_nStep==1 )
		{	
			if( !m_pDrawProc->m_pGeoCurve || IsProcFinished(this) )
				return;
			
			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_nStep = 2;

			m_pDrawProc->m_pGeoCurve->GetShape(m_arrPts);
			
		}
		else 
		{
			Abort();
		}
	}
	if( m_pDrawProc )
	{
		m_pGeoCurve = (CGeoCurve *)m_pDrawProc->m_pGeoCurve->Clone();
		if(m_pDrawProc->m_pGeoCurve) delete m_pDrawProc->m_pGeoCurve;
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
		
		CEditCommand::OnSonEnd(son);
	}
}

void CRiverFillCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep == 0 || m_nStep == 1)
	{	
		if (m_nFillType == 0)
		{
			if( !CanGetSelObjs(flag) )return;
			m_nStep = 2;
		}
		else if (m_nFillType == 1)
		{
			if (m_pDrawProc)
			{
				GotoState(PROCSTATE_PROCESSING);
				m_pDrawProc->PtClick(pt,flag);
			}
		}
		
		
	}
	
	if( m_nStep==2 )
	{
		CDlgDataSource *pDS = GETDS(m_pEditor);
		if (!pDS) return;
		
		if (m_nFillType == 0)
		{
			CSelection* pSel = m_pEditor->GetSelection();
			
			int num;
			const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
			
			if(num==1)
			{
				CGeometry *pGeo1 = HandleToFtr(handles[0])->GetGeometry();
				if(!pGeo1 || !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					Abort();
					return;
				}
				CUndoModifyProperties undo(m_pEditor,Name());

				undo.arrHandles.Add(handles[0]);
				
				CValueTable tab2;
				tab2.BeginAddValueItem();
				HandleToFtr(handles[0])->WriteTo(tab2);	
				tab2.EndAddValueItem();
				tab2.DelField(FIELDNAME_SHAPE);
				
				undo.oldVT.AddItemFromTab(tab2);
				m_pEditor->DeleteObject(handles[0]);
				((CGeoCurveBase*)pGeo1)->EnableFillColor(TRUE,m_nColor);
				
				CString symName = pGeo1->GetSymbolName();
				m_pEditor->RestoreObject(handles[0]);
				
				if (tab2.SetValue(0,FIELDNAME_SYMBOLNAME,&CVariantEx((_variant_t)(const char*)symName)))
				{
					undo.newVT.AddItemFromTab(tab2);
				}
				undo.Commit();
			}
			else if (num == 2)
			{
				CGeometry *pGeo1 = HandleToFtr(handles[0])->GetGeometry(), *pGeo2 = HandleToFtr(handles[1])->GetGeometry();
				
				if ( !pGeo1 || !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				{
					Abort();
					return;
				}
				
				if ( !pGeo2 || !pGeo2->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				{
					Abort();
					return;
				}
				
				CArray<PT_3DEX,PT_3DEX> pts1, pts2;
				pGeo1->GetShape(pts1);
				pGeo2->GetShape(pts2);
				
				int num1 = pts1.GetSize(), num2 = pts2.GetSize();
				
				CArray<PT_3DEX,PT_3DEX> pts;
				
				if (!GraphAPI::GGetLineIntersectLineSeg(pts1[0].x,pts1[0].y,pts2[0].x,pts2[0].y,pts1[num1-1].x,pts1[num1-1].y,pts2[num2-1].x,pts2[num2-1].y,NULL,NULL,NULL))
				{
					pts.Append(pts1);
					for (int i=num2-1; i>=0; i--)
					{
						pts.Add(pts2[i]);
					}
				}
				else
				{
					pts.Append(pts1);
					pts.Append(pts2);
				}
				pts.Add(pts[0]);
				
				CFeature *pNewFtr = NULL;
				CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(HandleToFtr(handles[0]));
				if (!pLayer) 
				{
					Abort();
					return;
				}
				
				pNewFtr = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOSURFACE);
				if (!pNewFtr)
				{
					Abort();
					return;
				}
				
				pNewFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
				
				CString code = "%";	
				pNewFtr->GetGeometry()->SetSymbolName(code);
				pNewFtr->GetGeometry()->SetColor(m_nColor);
				
				m_pEditor->AddObject(pNewFtr,pLayer->GetID());	
				
				CUndoFtrs undo(m_pEditor,Name());
				
				undo.AddNewFeature(FtrToHandle(pNewFtr));
				undo.Commit();
			}
			else
			{
				Abort();
				return;
			}
		}
		else if (m_nFillType == 1)
		{
			CFeature *pNewFtr = NULL;
			CString name = m_strRetLay.IsEmpty()?StrFromResID(IDS_DEFLAYER_NAMEL):m_strRetLay;
			CFtrLayer *pLayer = pDS->GetFtrLayer(name);
			if (!pLayer) 
			{
				Abort();
				return;
			}
			
			pNewFtr = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOSURFACE);
			if (!pNewFtr)
			{
				Abort();
				return;
			}
			
			pNewFtr->GetGeometry()->CreateShape(m_arrPts.GetData(),m_arrPts.GetSize());
			
			CString code = "%";
			
			pNewFtr->GetGeometry()->SetSymbolName(code);
			pNewFtr->GetGeometry()->SetColor(m_nColor);
			
			m_pEditor->AddObject(pNewFtr,pLayer->GetID());	
			
			CUndoFtrs undo(m_pEditor,Name());
			
			undo.AddNewFeature(FtrToHandle(pNewFtr));
			undo.Commit();
		}
		

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		
		
		Finish();
		m_nStep = 3;
	}

	if( m_nFillType==1 )
	{
		if( m_nStep==0 || m_nStep==1 )
		{		
			m_nStep = 1;
		}
	}
	
	CEditCommand::PtClick(pt, flag);
}

void CRiverFillCommand::Abort()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
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
    m_nStep = -1;
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CRiverFillCommand::Finish()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
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
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}

void CRiverFillCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CopyFtrsBetweenLayer",StrFromLocalResID(IDS_CMDNAME_RIVERFILL));
	
	param->AddColorParam(PF_RIVERFILL_COLOR,(long)m_nColor,StrFromResID(IDS_CMDPLANE_RIVERCOLOR));

	param->BeginOptionParam(PF_RIVERFILL_TYPE,StrFromResID(IDS_CMDPLANE_RIVERFILLTYPE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SELECT),0,' ',m_nFillType==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_DRAW),1,' ',m_nFillType==1);
	param->EndOptionParam();

	if (m_nFillType == 1 || bForLoad)
	{
		param->AddLayerNameParam(PF_RIVERFILL_RETNAME,(LPCTSTR)m_strRetLay,StrFromResID(IDS_CMDPLANE_RETLAY));
	}
	
}

void CRiverFillCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_RIVERFILL_TYPE,var) )
	{					
		m_nFillType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();					
	}

	if( tab.GetValue(0,PF_RIVERFILL_RETNAME,var) )
	{					
		m_strRetLay = (const char*)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();					
	}

	if( tab.GetValue(0,PF_RIVERFILL_COLOR,var) )
	{
		m_nColor = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

}

void CRiverFillCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;

	var = (long)(m_nFillType);
	tab.AddValue(PF_RIVERFILL_TYPE,&CVariantEx(var));
	var = (const char*)m_strRetLay;
	tab.AddValue(PF_RIVERFILL_RETNAME,&CVariantEx(var));
	var = (long)m_nColor;
	tab.AddValue(PF_RIVERFILL_COLOR,&CVariantEx(var));
}
//////////////////////////////////////////////////////////////////////
// CExplodeTextCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CExplodeTextCommand,CEditCommand)

CExplodeTextCommand::CExplodeTextCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\ExplodeText");
}

CExplodeTextCommand::~CExplodeTextCommand()
{
}

CString CExplodeTextCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_EXPLODETEXT);
}

void CExplodeTextCommand::Start()
{
	if( !m_pEditor )return;

	CEditCommand::Start();

	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num>0 )
	{
		m_nStep = 0;
	
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}		
}


BOOL CExplodeTextCommand::ExplodeText(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrObjs)
{
	//一般化打散
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

	return pDS->ExplodeText(pFtr,pLayer,arrObjs);
}



void CExplodeTextCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CUndoFtrs undo(m_pEditor,Name());		
		
		int nsel;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(nsel);
		for( int i=nsel-1; i>=0; i--)
		{
		
			CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(HandleToFtr(handles[i]));
			
			CFtrArray arrObjs;
			if( ExplodeText(HandleToFtr(handles[i]),pLayer,arrObjs) )
			{								
				for( int j=0; j<arrObjs.GetSize(); j++)
				{
					((CFeature*)arrObjs.GetAt(j))->SetPurpose(FTR_MARK);
					m_pEditor->AddObject((CFeature*)arrObjs.GetAt(j),pLayer->GetID());
					PDOC(m_pEditor)->GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(HandleToFtr(handles[i]),(CFeature*)arrObjs.GetAt(j));
					undo.AddNewFeature(FtrToHandle((CFeature*)arrObjs.GetAt(j)));				
				}
				
				//删除原来的对象
				{
					m_pEditor->DeleteObject(handles[i]);
					undo.AddOldFeature(handles[i]);
				}
				
			}
		
		}
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	CEditCommand::PtClick(pt, flag);
}


void CExplodeTextCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CEditCommand::Finish();
	
}


//////////////////////////////////////////////////////////////////////
// CCreatePartDemCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCreatePartDemCommand,CDrawCommand)

CCreatePartDemCommand::CCreatePartDemCommand()
{
	m_nStep = -1;
	m_nBoundType = 0;
	m_pDrawProc = NULL;
//	m_nStep=-1;

	m_pFtr = NULL;
	m_pSel = NULL;
	strcat(m_strRegPath,"\\CreatePartDem");
}

CCreatePartDemCommand::~CCreatePartDemCommand()
{
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	if (m_pSel)
	{
		delete m_pSel;
	}
}

void CCreatePartDemCommand::Finish()
{
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	if (m_pDrawProc)
	{
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	if( m_pEditor )
	{
		if( m_pFtr )
		{			
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_ANNOTTOTEXT,FALSE);
			
			if( bAnnotToText )
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if(pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOTEXT);
				if( pLayer!=NULL && pTextLayer!=NULL && pTempl!=NULL )
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor),m_pFtr,arrAnnots);
					CUndoFtrs undo(m_pEditor,Name());
					for(int i=0; i<arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType()==SYMTYPE_ANNOTATION )
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;							
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr,tab);
							if( pAnnot->ExtractGeoText(m_pFtr,parr,tab,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()) )
							{
								for (int j=0; j<parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);									
									PDOC(m_pEditor)->AddObject(pNewFtr,pTextLayer->GetID());									
									
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
									
								}
							}
						}					
						
					}
					undo.Commit();					
				}
				if( pTempl!=NULL )delete pTempl;
			}
		}		
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}
	CDrawCommand::Finish();
}

CString CCreatePartDemCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CREATEPARTDEM);
}

void CCreatePartDemCommand::Start()
{
//	PDOC(m_pEditor).GetDlgDataSource()->GetCurFtrLayerID()
	m_nStep = 0;
	m_nBoundType = 0;
	m_pFtr = NULL;
	m_pSel = NULL;
	CDlgDataSource *pDS = GETDS(m_pEditor);
	if(!pDS) return;
	int oldID = pDS->GetCurFtrLayerID();
	CFtrLayer *pLayer = NULL;
	pLayer = pDS->GetDefaultLineFtrLayer();
	if (pLayer)
	{
		pDS->SetCurFtrLayer(pLayer->GetID());
	}
	pLayer = GETCURFTRLAYER(m_pEditor);
	if(!pLayer) return;
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOCURVE);
	if(!m_pFtr) return;	
//	((CGeoCurve*)m_pFtr->GetGeometry())->EnableClose(TRUE);
	m_pDrawProc = new CDrawCurveProcedure;
	if( !m_pDrawProc )return;
	
	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pFtr->GetGeometry();	
	
	m_pDrawProc->Start();	
	m_pDrawProc->m_bClosed = TRUE;

	CDrawCommand::Start();
	
	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
	
}

void CCreatePartDemCommand::RefreshPropertiesPanel()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}
DrawingInfo CCreatePartDemCommand::GetCurDrawingInfo()
{
	return DrawingInfo();
}

void CCreatePartDemCommand::Abort()
{
	if (m_pDrawProc)
	{
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CDrawCommand::Abort();	
}


void CCreatePartDemCommand::OnSonEnd(CProcedure *son)
{
	if( m_nBoundType==0 )
	{
		if( m_nStep==1 )
		{	
			if( !m_pDrawProc->m_pGeoCurve || IsProcFinished(this) )
				return;
			
			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_nStep = 2;
			
		}
		else 
		{
			Abort();
		}
	}
	if( m_pDrawProc )
	{
		m_pFtr->SetGeometry(m_pDrawProc->m_pGeoCurve->Clone());
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
		
		CDrawCommand::OnSonEnd(son);
	}

}


CProcedure *CCreatePartDemCommand::GetActiveSonProc(int nMsgType)
{
	if (m_nBoundType==0&&(nMsgType==msgEnd||nMsgType==msgPtMove/*||nMsgType==msgPtReset*/))
	{
		return m_pDrawProc;
	}
	return NULL;
}


void CCreatePartDemCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pFtr ||IsProcFinished(this) )return;

	if( PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(_T("DEMPoints"))==NULL )
	{
		AfxMessageBox(IDS_ERROR_NEEDDEM);
		return;
	}

	GotoState(PROCSTATE_PROCESSING);

	//画线
	if( m_nBoundType==0 )
	{
		if( m_nStep==0 || m_nStep==1 )
		{
			m_pDrawProc->PtClick(pt,flag);
			m_nStep = 1;
		}
	}

	if ( m_nStep == 2)
	{
		m_pEditor->UpdateDrag(ud_ClearDrag);
		//	m_pEditor->RefreshView();
		const CShapeLine  *pSL  = m_pFtr->GetGeometry()->GetShape();
		
		//初始化裁剪器
		CArray<PT_3DEX,PT_3DEX> pts;
		int polysnum;
		pSL->GetPts(pts);	
		polysnum = pts.GetSize();
		
		PT_3D *polys = new PT_3D[polysnum];
		
		if( !polys )
		{
			return;
		}
		
		for( int i=0; i<polysnum; i++)
		{
			COPY_3DPT((polys[i]),(pts[i]));
		}
		
		PDOC(m_pEditor)->BuildPartDem(polys,polysnum,0,0);
		
		
		if( polys )delete[] polys;
		
		Finish();
	}
	
	
	CDrawCommand::PtClick(pt,flag);
}

void CCreatePartDemCommand::PtMove(PT_3D &pt)
{	
	return;
}

void CCreatePartDemCommand::PtReset(PT_3D &pt)
{	
	if( m_pDrawProc && !IsProcOver(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
	}
	else
		Abort();	
}


void CCreatePartDemCommand::GetParams(CValueTable &tab)
{	
	CDrawCommand::GetParams(tab);
  
}


void CCreatePartDemCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CreatePartDem",StrFromResID(IDS_CMDNAME_CREATEPARTDEM)); 	
}


void CCreatePartDemCommand::SetParams(CValueTable& tab,BOOL bInit)
{ 	
	CDrawCommand::SetParams(tab,bInit);
}



//////////////////////////////////////////////////////////////////////
// CTrimDemCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTrimDemCommand,CEditCommand)

CTrimDemCommand::CTrimDemCommand()
{
	m_pDrawProc = NULL;
//	m_nStep=-1;

	m_pGeo = NULL;
//	m_pSel = NULL;
	strcat(m_strRegPath,"\\TrimDem");
}

CTrimDemCommand::~CTrimDemCommand()
{
	if (m_pGeo)
	{
		delete m_pGeo;
		m_pGeo = NULL;
	}
	if (m_pDrawProc)
	{
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
}

void CTrimDemCommand::Finish()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeo)
	{
		delete m_pGeo;
		m_pGeo = NULL;
	}
	m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

CString CTrimDemCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TRIM) + "DEM";
}

void CTrimDemCommand::Start()
{
	if( !m_pEditor )return;		
	m_nStep = 0;
//	m_bTrimOut = TRUE;
	m_nBoundType = 0;
	m_pGeo = NULL;
	m_pDrawProc = NULL;
	
	CEditCommand::Start();
	if (m_nBoundType==0)
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if( !m_pDrawProc )return;
		m_pDrawProc->Init(m_pEditor);
		UpdateParams(FALSE);
		m_pDrawProc->Start();
		m_pGeo = new CGeoCurve;
		if(!m_pGeo) 
		{
			Abort();
			return ;
		}
		m_pGeo->SetColor(RGB(255,255,255));
		m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeo;
		//m_pDrawProc->m_pGeoCurve->EnableClose(TRUE);
		m_pDrawProc->m_bClosed = TRUE;
		CCommand::Start();
		m_pEditor->CloseSelector();
		return;
	}	
	if( m_nBoundType==1 )
		m_pEditor->OpenSelector(SELMODE_MULTI);
	if (m_nBoundType==2)
	{
		m_pEditor->CloseSelector();
	}
}

void CTrimDemCommand::Abort()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
	{
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if (m_pGeo)
	{
		delete m_pGeo;
		m_pGeo = NULL;
	}
    m_nStep = -1;
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	m_pEditor->RefreshView();
	CEditCommand::Abort();	
}


void CTrimDemCommand::OnSonEnd(CProcedure *son)
{
	if( m_pDrawProc )
	{
		m_pGeo = (CGeoCurve *)m_pDrawProc->m_pGeoCurve->Clone();
		if(m_pDrawProc->m_pGeoCurve) delete m_pDrawProc->m_pGeoCurve;
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
		
		CEditCommand::OnSonEnd(son);
	}
}

CProcedure *CTrimDemCommand::GetActiveSonProc(int nMsgType)
{
	if(m_nBoundType==0 && nMsgType==msgPtClick || nMsgType==msgPtReset || nMsgType==msgPtMove )
		return NULL;
	
	return m_pDrawProc;
}


void CTrimDemCommand::PtClick(PT_3D &pt, int flag)
{
	if( PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(_T("DEMPoints"))==NULL )
	{
		AfxMessageBox(IDS_ERROR_NEEDDEM);
		return;
	}

	if (m_nBoundType==0)
	{
		if (m_pDrawProc)
		{
			GotoState(PROCSTATE_PROCESSING);
			m_pDrawProc->PtClick(pt,flag);
		}
	}
	else if( m_nBoundType==1 )
	{
		if( m_nStep==0 )
		{
			if( !CanGetSelObjs(flag,FALSE) )return;
			
			int num ;
			m_pEditor->GetSelection()->GetSelectedObjs(num);
			if( num!=1 )return;
			
			FTR_HANDLE  handle = m_pEditor->GetSelection()->GetLastSelectedObj();
			CGeometry *pObj = HandleToFtr(handle)->GetGeometry();
			if( !pObj || pObj->GetDataPointSum()<3 ||(!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))&&!pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))))return;
			
			m_pEditor->CloseSelector();			
			m_pGeo = pObj->Clone();
			m_handleBound = handle;
			
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 2;
			return;
		}
	}
	else if( m_nBoundType==2 )
	{
		if( m_nStep==0 )
		{
			m_pGeo = new CGeoCurve;
			if(m_pGeo )
			{
				PT_3D pts[4];
				PT_3DEX expts[4];
				PDOC(m_pEditor)->GetDlgDataSource()->GetBound(pts,NULL,NULL);
				
				for( int i=0; i<4; i++)
				{
					expts[i].z = 0;
					expts[i].pencode = penLine;
					COPY_2DPT(expts[i],pts[i]);
					
				}
				((CGeoCurve*)m_pGeo)->EnableClose(TRUE);
				m_pGeo->CreateShape(expts,4);
				GotoState(PROCSTATE_PROCESSING);
				m_nStep = 2;
			}
		}
	}
		//确认并执行
	if( m_nStep==2 )
	{
		if( IsProcFinished(this) )return;
		
		const CShapeLine *pBase  = m_pGeo->GetShape();
		
		//初始化裁剪器
		int polysnum = pBase->GetPtsCount();
		
		PT_3D *polys = new PT_3D[polysnum];

		
		if( !polys )
		{			
			if( polys )delete[] polys;
			return;
		}
		CArray<PT_3DEX,PT_3DEX> pts;
		if(!pBase->GetPts(pts))return;
		for( int i=0; i<polysnum; i++)
		{
			COPY_3DPT(polys[i],pts[i]);
		}

		CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			_T("All Files(*.*)|*.*||"));
		
		if( dlg.DoModal()!=IDOK )
		{		
			if( polys )delete[] polys;
			
			Abort();
			return;
		}

		PDOC(m_pEditor)->TrimDem(dlg.GetPathName(),polys,polysnum);		
	
		if( polys )delete[] polys;
		
		Finish();
	}

	//画线
	if( m_nBoundType==0 )
	{
		if( m_nStep==0 || m_nStep==1 )
		{		
			m_nStep = 1;
		}
	}

	return;
}

void CTrimDemCommand::PtMove(PT_3D &pt)
{	
	if( m_nBoundType==0 )
	{
		if( m_pDrawProc && (m_nStep==0 || m_nStep==1) )
		{
			m_pDrawProc->PtMove(pt);
		}
	}
}

void CTrimDemCommand::PtReset(PT_3D &pt)
{	
	if( m_nBoundType==0 )
	{
		if( m_nStep==1 )
		{	
			if( !m_pDrawProc->m_pGeoCurve || IsProcFinished(this) )return;

			m_pDrawProc->PtReset(pt);

			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_nStep = 2;
			
		}
		else 
		{
			Abort();
		}
	}
//	CEditCommand::PtReset(pt);	
}


void CTrimDemCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nBoundType);
	tab.AddValue(PF_TRIMBOUND,&CVariantEx(var));
	if(m_pDrawProc)var = double(m_pDrawProc->m_compress.GetLimit());
	tab.AddValue(PF_TRIMTOLER,&CVariantEx(var));  
}


void CTrimDemCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("TrimDemCommand",StrFromLocalResID(IDS_CMDNAME_TRIM));
	param->BeginOptionParam(PF_TRIMBOUND,StrFromResID(IDS_CMDPLANE_BOUND));
	param->AddOption(StrFromResID(IDS_CMDPLANE_DRAW),0,' ',m_nBoundType==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SEL),1,' ',m_nBoundType==1);
	param->AddOption(StrFromResID(IDS_CMDPLANE_WSBOUND),2,' ',m_nBoundType==2);
	param->EndOptionParam();
	if(m_nBoundType!=0||!m_pDrawProc) return;
	BOOL bHide = (m_pDrawProc->m_nCurPenCode!=penStream);
 	if(!bHide||bForLoad)param->AddParam(PF_TRIMTOLER,double(m_pDrawProc->m_compress.GetLimit()),StrFromResID(IDS_CMDPLANE_TOLER)); 	
}


void CTrimDemCommand::SetParams(CValueTable& tab,BOOL bInit)
{ 	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	if( tab.GetValue(0,PF_TRIMBOUND,var) )
	{
		m_nBoundType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();
	}
	if( tab.GetValue(0,PF_TRIMTOLER,var) )
	{
		double toler = (double)(_variant_t)*var;
		if (m_pDrawProc)
		{
			m_pDrawProc->m_compress.SetLimit(toler);
			SetSettingsModifyFlag();
		}
	}
	CEditCommand::SetParams(tab,bInit);
}



/////////////////////////////////////////////////////////////////////
// CAutoAttributeCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CAutoAttributeCommand,CEditCommand)

CAutoAttributeCommand::CAutoAttributeCommand()
{
	m_nStep = -1;
	m_arrHandles.RemoveAll();
	strcat(m_strRegPath,"\\AutoAttribute");

	m_nBoundType = 0;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;
}

CAutoAttributeCommand::~CAutoAttributeCommand()
{
	if (m_pDrawProc)
	{
		delete m_pDrawProc;
	}
	if (m_pGeoCurve)
	{
		delete m_pGeoCurve;
	}
}

CCommand* CAutoAttributeCommand::Create()
{
	return new CAutoAttributeCommand;
}

CString CAutoAttributeCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_AUTOATTRIBUTE);
}


void CAutoAttributeCommand::Back()
{
	if (m_pDrawProc)
	{
		m_pDrawProc->Back();
		if (!m_pDrawProc || IsProcOver(m_pDrawProc))
		{
			Abort();
		}
	}
}

void CAutoAttributeCommand::Start()
{
	if( !m_pEditor )return;

	m_nBoundType = 0;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;
	
	m_strFieldName = "";
	m_strStartValue = "ABC001";
	m_arrHandles.RemoveAll();

	CEditCommand::Start();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	if (m_nBoundType==0)
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if( !m_pDrawProc )return;
		m_pDrawProc->Init(m_pEditor);
		UpdateParams(FALSE);
		m_pDrawProc->Start();
		m_pGeoCurve = new CGeoCurve;
		if(!m_pGeoCurve) 
		{
			Abort();
			return ;
		}
		m_pGeoCurve->SetColor(RGB(255,255,255));
		m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
		m_pDrawProc->m_bClosed = TRUE;
		CCommand::Start();
		m_pEditor->CloseSelector();
		return;
	}	
	if( m_nBoundType==1 )
		m_pEditor->OpenSelector(SELMODE_MULTI);
	if (m_nBoundType==2)
	{
		m_pEditor->CloseSelector();
	}
}


void CAutoAttributeCommand::Abort()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
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
    m_nStep = -1;
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_ClearDrag);

	m_pEditor->RefreshView();
	CEditCommand::Abort();
}


CProcedure *CAutoAttributeCommand::GetActiveSonProc(int nMsgType)
{
	if (m_nBoundType==0&&(nMsgType==msgEnd||nMsgType==msgPtMove/*||nMsgType==msgPtReset*/))
	{
		return m_pDrawProc;
	}
	return NULL;
}

int CAutoAttributeCommand::GetState()
{
	return m_nState;
}

void CAutoAttributeCommand::PtReset(PT_3D &pt)
{	
	if( m_pDrawProc && !IsProcOver(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
	}
	else
		Abort();
}


void CAutoAttributeCommand::OnSonEnd(CProcedure *son)
{
	if( m_nBoundType==0 )
	{
		if( m_nStep==1 )
		{	
			if( !m_pDrawProc->m_pGeoCurve || IsProcFinished(this) )
				return;
			
			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_nStep = 2;
			
		}
		else 
		{
			Abort();
		}
	}
	if( m_pDrawProc )
	{
		m_pGeoCurve = (CGeoCurve *)m_pDrawProc->m_pGeoCurve->Clone();
		if(m_pDrawProc->m_pGeoCurve) delete m_pDrawProc->m_pGeoCurve;
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;

		CEditCommand::OnSonEnd(son);
	}
}

void CAutoAttributeCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("AutoAttribute"),StrFromResID(IDS_CMDNAME_AUTOATTRIBUTE));
	
	param->BeginOptionParam(PF_TRIMBOUND,StrFromResID(IDS_CMDPLANE_BOUND));
	param->AddOption(StrFromResID(IDS_CMDPLANE_DRAW),0,' ',m_nBoundType==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SEL),1,' ',m_nBoundType==1);
	param->AddOption(StrFromResID(IDS_CMDPLANE_WSBOUND),2,' ',m_nBoundType==2);
	param->EndOptionParam();
	
	param->AddParam(PF_AUTOATTRIBUTEFIELD,(LPCTSTR)m_strFieldName, StrFromResID(IDS_CMDPLANE_FIELDNAME));
		
	param->AddParam(PF_AUTOATTRIBUTESTART,(LPCTSTR)m_strStartValue, StrFromResID(IDS_CMDPLANE_STARTVALUE));

}

void CAutoAttributeCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;

	if( tab.GetValue(0,PF_TRIMBOUND,var) )
	{					
		m_nBoundType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();					
	}

	if( tab.GetValue(0,PF_AUTOATTRIBUTEFIELD,var) )
	{
		m_strFieldName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_AUTOATTRIBUTESTART,var) )
	{
		m_strStartValue = (LPCTSTR)(_bstr_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}

void CAutoAttributeCommand::GetParams(CValueTable& tab)
{
	_variant_t var;

	var = (long)(m_nBoundType);
	tab.AddValue(PF_TRIMBOUND,&CVariantEx(var));

	var = LPCTSTR(m_strFieldName);
	tab.AddValue(PF_AUTOATTRIBUTEFIELD,&CVariantEx(var));
	var = (LPCTSTR)m_strStartValue;
	tab.AddValue(PF_AUTOATTRIBUTESTART,&CVariantEx(var));
}



BOOL CAutoAttributeCommand::Convert(CFeature *pFtr, CFtrLayer *pLayer,CString value, CUndoModifyProperties& undo)
{	
	if(!pLayer|| !pFtr || pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)) )return FALSE;
	
	BOOL bRet = FALSE;
	
	CValueTable tab,oldtab;
	tab.BeginAddValueItem();
	GETXDS(m_pEditor)->GetXAttributes(pFtr,tab);
	//pFtr->WriteTo(tab);
	tab.EndAddValueItem();
	CConfigLibManager *pCon = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	if(!pCon) return FALSE;
	CScheme *pSch = pCon->GetScheme(GETDS(m_pEditor)->GetScale());
	if(!pSch) return FALSE;
	
	pSch->GetXAttributesAlias(pLayer->GetName(),tab);

	oldtab.CopyFrom(tab);
	CString field,  name;
	int type;
	_variant_t var;
	int nField = tab.GetFieldCount();

	for( int j=0; j<nField; j++)
	{		
		if(!tab.GetField(j,field, type,name))continue;
		if( m_strFieldName.CompareNoCase(field)==0 || 
			m_strFieldName.CompareNoCase(name)==0 )
		{
			break;
		}
	}
	if( j<nField )
	{		
		if( type==VT_BSTR )
		{
			CString strValue;
			strValue = value;
			var = (LPCTSTR)strValue;
			tab.SetValue(0,field,new CVariantEx(var));
// 			pNewObj->GetBind()->SetAttrValue(info.m_strName,var,DP_CFT_VARCHAR);
// 			pNewObj->UpdateData(FALSE);
			bRet = TRUE;
		}
		else if( type==VT_I2 )
		{
			var = (short)atoi(value);
			tab.SetValue(0,field,new CVariantEx(var));
// 			pNewObj->GetBind()->SetAttrValue(info.m_strName,var,DP_CFT_INTEGER);
// 			pNewObj->UpdateData(FALSE);
			bRet = TRUE;
		}
		else if (type==VT_I4)
		{
			var = (long)atoi(value);
			tab.SetValue(0,field,new CVariantEx(var));
			// 			pNewObj->GetBind()->SetAttrValue(info.m_strName,var,DP_CFT_INTEGER);
			// 			pNewObj->UpdateData(FALSE);
			bRet = TRUE;
		}
		
		if( bRet )
		{
			undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,FALSE);
			GETXDS(m_pEditor)->SetXAttributes(pFtr,tab);			
		}	
	}
	
	return bRet;
}


void CAutoAttributeCommand::Add1(CString& textID)
{
	int nLen = textID.GetLength();
	if(nLen<=0)
	{
		textID = "00001";
		return;
	}
	
	char text[1000] = {0};
	strcpy(text,textID);
	char *pos = text + nLen-1;
	while(pos>=text)
	{
		if(*pos>='0' && *pos<='9')
		{	
			pos--;
		}
		else
		{
			break;
		}
	}

	if(pos<text)
	{
		CString strFormat;
		strFormat.Format("%%0%dd",nLen);

		long value = atol(text);
		textID.Format(strFormat,value+1);
	}
	else if(pos<text + nLen-1 )
	{
		nLen = strlen(pos+1);
		
		CString strFormat;
		strFormat.Format("%%0%dd",nLen);

		long value = atol(pos+1);
		CString t;
		t.Format(strFormat,value+1);
		textID = textID.Left(pos+1-text) + t;		
	}
	else
	{
		textID = textID + "1";
	}
}


void CAutoAttributeCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nBoundType==0)
	{
		if (m_pDrawProc)
		{
			GotoState(PROCSTATE_PROCESSING);
			m_pDrawProc->PtClick(pt,flag);
		}
	}
	else if( m_nBoundType==1 )
	{
		if( m_nStep==0 )
		{
			if( !CanGetSelObjs(flag) )return;
			
			int num ;
			m_pEditor->GetSelection()->GetSelectedObjs(num);
			if( num!=1 )return;
			
			FTR_HANDLE  handle = m_pEditor->GetSelection()->GetLastSelectedObj();
			CGeometry *pObj = HandleToFtr(handle)->GetGeometry();
			if( !pObj || pObj->GetDataPointSum()<3 || (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))))return;
			
			m_pEditor->CloseSelector();			
			m_pGeoCurve = pObj->Clone();
			m_handleBound = handle;

 			m_pEditor->DeselectAll();
 			m_pEditor->OnSelectChanged();
			
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 2;
		}
	}
	
	if( m_nStep==2 )
	{ 		
 		int i,j;
		CString strStartValue = m_strStartValue;
 		if( m_pGeoCurve )
		{
			CArray<PT_3DEX,PT_3DEX> arrFtrPts;
			m_pGeoCurve->GetShape(arrFtrPts);
			int nPt = arrFtrPts.GetSize();

			if( nPt<3 )
			{
				Abort();
				return;
			}
			
			CArray<PT_3D,PT_3D> arrPts;

			arrPts.SetSize(nPt);
			PT_3D *pts = arrPts.GetData();
			
			for( j=0; j<nPt; j++)
			{
				COPY_3DPT(pts[j],arrFtrPts[j]);
			}
			
			strStartValue = m_strStartValue;
			if( strStartValue.IsEmpty() )
				strStartValue = "00001";
			
			{
				int i1, j1, nLayer = GETDS(m_pEditor)->GetFtrLayerCount(), nObj;
				
				//计数
				//并将地物按照层码分类
				CFtrLayer *pLayer = NULL;
				CFeature *pFtr1 = NULL;

				CUndoModifyProperties undo(m_pEditor,Name()); 
				for (i1=0; i1<nLayer; i1++)
				{
					pLayer = GETDS(m_pEditor)->GetFtrLayerByIndex(i1);
					if( !pLayer )continue;
					
					nObj = pLayer->GetObjectCount();
					if( nObj<=0 )continue;
					
					for( j1=0; j1<nObj; j1++)
					{
						pFtr1 = pLayer->GetObject(j1);
						if( !pFtr1 )continue;
						
						if( IsObjInRegion(pFtr1->GetGeometry(),pts,nPt) )
						{
							if( Convert(pFtr1,pLayer,strStartValue,undo) )
							{
								Add1(strStartValue);
							}
						}						
					}
				}
				undo.Commit();
			}
		}
		
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
 	}	

	//画线
	if( m_nBoundType==0 )
	{
		if( m_nStep==0 || m_nStep==1 )
		{		
			m_nStep = 1;
		}
	}

	CEditCommand::PtClick(pt,flag);
}


/////////////////////////////////////////////////////////////////////
// CModifyAttributeCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CModifyAttributeCommand,CEditCommand)

CModifyAttributeCommand::CModifyAttributeCommand()
{
	m_nStep = -1;
	m_arrHandles.RemoveAll();
	strcat(m_strRegPath,"\\ModifyAttribute");
}

CModifyAttributeCommand::~CModifyAttributeCommand()
{
}

CCommand* CModifyAttributeCommand::Create()
{
	return new CModifyAttributeCommand;
}

CString CModifyAttributeCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MODIFYATTRIBUTE);
}


void CModifyAttributeCommand::Start()
{
	if( !m_pEditor )return;
	
	m_strLayerName = "";
	m_strFieldName = "";
	m_strValue = "";
	
	m_nStep = 0;
	m_arrHandles.RemoveAll();
	m_pEditor->CloseSelector();
	CCommand::Start();
}

void CModifyAttributeCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("ModifyAttribute"),StrFromResID(IDS_CMDNAME_MODIFYATTRIBUTE));
	
	param->AddLayerNameParam(PF_MODIFYATTRIBUTELAYERCODE,(LPCTSTR)m_strLayerName,StrFromResID(IDS_CMDPLANE_LAYERCODE));

	param->AddParam(PF_MODIFYATTRIBUTEFIELD,(LPCTSTR)m_strFieldName, StrFromResID(IDS_CMDPLANE_FIELDNAME));
		
	param->AddParam(PF_MODIFYATTRIBUTEVALUE,(LPCTSTR)m_strValue, StrFromResID(IDS_CMDPLANE_MODIFYVALUE));
	
}

void CModifyAttributeCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_MODIFYATTRIBUTELAYERCODE,var) )
	{
		m_strLayerName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_MODIFYATTRIBUTEFIELD,var) )
	{
		m_strFieldName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_MODIFYATTRIBUTEVALUE,var) )
	{
		m_strValue = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}

void CModifyAttributeCommand::GetParams(CValueTable& tab)
{
	_variant_t var;
	var = LPCTSTR(m_strLayerName);
	tab.AddValue(PF_MODIFYATTRIBUTELAYERCODE,&CVariantEx(var));
	var = LPCTSTR(m_strFieldName);
	tab.AddValue(PF_MODIFYATTRIBUTEFIELD,&CVariantEx(var));
	var = LPCTSTR(m_strValue);
	tab.AddValue(PF_MODIFYATTRIBUTEVALUE,&CVariantEx(var));
}


BOOL CModifyAttributeCommand::Convert(CFeature *pFtr, CFtrLayer *pLayer,CString value, CUndoModifyProperties& undo)
{	
	if(!pLayer|| !pFtr )return FALSE;
	
	if( !m_strLayerName.IsEmpty() && !CAutoLinkContourCommand::CheckObjForLayerCode(GETDS(m_pEditor),pFtr,m_strLayerName) )
		return FALSE;
	BOOL bRet = FALSE;	
	
	CValueTable tab,xtab,oldtab,xoldtab;
	const CVariantEx* val;

	//基本属性
	tab.BeginAddValueItem();
	pFtr->WriteTo(tab);
	tab.EndAddValueItem();

	//扩展属性
	xtab.BeginAddValueItem();
	GETXDS(m_pEditor)->GetXAttributes(pFtr,xtab);
	//pFtr->WriteTo(tab);
	xtab.EndAddValueItem();
	CConfigLibManager *pCon = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	if(!pCon) return FALSE;
	CScheme *pSch = pCon->GetScheme(GETDS(m_pEditor)->GetScale());
	if(!pSch) return FALSE;
	pSch->GetXAttributesAlias(pLayer->GetName(),xtab);

	oldtab.CopyFrom(tab);
	xoldtab.CopyFrom(xtab);

	CString field, name;
	int type;
	_variant_t var;
	int nField = tab.GetFieldCount();
	for( int j=0; j<nField; j++)
	{		
		if(!tab.GetField(j,field, type,name))continue;
		if( m_strFieldName.CompareNoCase(field)==0 || 
			m_strFieldName.CompareNoCase(name)==0 )
		{
			break;
		}
	}
	if( j<nField )
	{		
		if( type==VT_BSTR )
		{
			tab.GetValue(0,field,val);
			CString strValue = (LPCTSTR)(_bstr_t)(_variant_t)*val + value;			
			var = (LPCTSTR)strValue;
			tab.SetValue(0,field,new CVariantEx(var));
			// 			pNewObj->GetBind()->SetAttrValue(info.m_strName,var,DP_CFT_VARCHAR);
			// 			pNewObj->UpdateData(FALSE);
			bRet = TRUE;
		}		
		else if (type==VT_I4||type==VT_I2)
		{
			tab.GetValue(0,field,val);
			var = (long)(_variant_t)*val+(long)atol(value);
			tab.SetValue(0,field,new CVariantEx(var));
			// 			pNewObj->GetBind()->SetAttrValue(info.m_strName,var,DP_CFT_INTEGER);
			// 			pNewObj->UpdateData(FALSE);
			bRet = TRUE;
		}
		else if (type==VT_R8||type==VT_R4)
		{
			tab.GetValue(0,field,val);
			var = (double)(_variant_t)*val+(double)atof(value);
			tab.SetValue(0,field,new CVariantEx(var));
			// 			pNewObj->GetBind()->SetAttrValue(info.m_strName,var,DP_CFT_INTEGER);
			// 			pNewObj->UpdateData(FALSE);
			bRet = TRUE;
		}
		if( bRet )
		{
			undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,FALSE);
			GETXDS(m_pEditor)->SetXAttributes(pFtr,tab);			
		}	
	}

	nField = xtab.GetFieldCount();

	for(  j=0; j<nField; j++)
	{		
		if(!xtab.GetField(j,field, type,name))continue;
		if( m_strFieldName.CompareNoCase(field)==0 || 
			m_strFieldName.CompareNoCase(name)==0 )
		{
			break;
		}
	}
	if( j<nField )
	{		
		if( type==VT_BSTR )
		{
			xtab.GetValue(0,field,val);
			CString strValue = (LPCTSTR)(_bstr_t)(_variant_t)*val + value;			
			var = (LPCTSTR)strValue;
			xtab.SetValue(0,field,new CVariantEx(var));
// 			pNewObj->GetBind()->SetAttrValue(info.m_strName,var,DP_CFT_VARCHAR);
// 			pNewObj->UpdateData(FALSE);
			bRet = TRUE;
		}		
		else if (type==VT_I4||type==VT_I2)
		{
			xtab.GetValue(0,field,val);
			var = (long)(_variant_t)*val+(long)atol(value);
			xtab.SetValue(0,field,new CVariantEx(var));
			// 			pNewObj->GetBind()->SetAttrValue(info.m_strName,var,DP_CFT_INTEGER);
			// 			pNewObj->UpdateData(FALSE);
			bRet = TRUE;
		}
		else if (type==VT_R8||type==VT_R4)
		{
			xtab.GetValue(0,field,val);
			var = (double)(_variant_t)*val+(double)atof(value);
			xtab.SetValue(0,field,new CVariantEx(var));
			// 			pNewObj->GetBind()->SetAttrValue(info.m_strName,var,DP_CFT_INTEGER);
			// 			pNewObj->UpdateData(FALSE);
			bRet = TRUE;
		}
		if( bRet )
		{
			undo.SetModifyProperties(FtrToHandle(pFtr),xoldtab,xtab,FALSE);
			GETXDS(m_pEditor)->SetXAttributes(pFtr,xtab);			
		}	
	}	
	return bRet;
}


void CModifyAttributeCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		CFeature *pFtr;
	
		//计算进度条
		int i, nsum = 0, nLayer = GETDS(m_pEditor)->GetFtrLayerCount();
		for ( i=0; i<nLayer; i++)
		{		
			CFtrLayer* pLayer = GETDS(m_pEditor)->GetFtrLayerByIndex(i);
			if (!pLayer)  continue;

			int nobj = pLayer->GetObjectCount();
		//	CGeometry *pObj = NULL;
			for (int j=0;j<nobj;j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;
				nsum++;
			}
		}
		
		GProgressStart(nsum);
		
		CUndoModifyProperties undo(m_pEditor,Name()); 
		for ( i=0; i<nLayer; i++)
		{
			CFtrLayer* pLayer = GETDS(m_pEditor)->GetFtrLayerByIndex(i);
			if (!pLayer)  continue;

			int nobj = pLayer->GetObjectCount();
		//	CGeometry *pObj = NULL;
			for (int j=0;j<nobj;j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;
				
				GProgressStep();
				
				if( Convert(pFtr,pLayer,m_strValue,undo) )
				{
				}
			}
		}

		undo.Commit();
		
		GProgressEnd();
		
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	CEditCommand::PtClick(pt,flag);
}


/////////////////////////////////////////////////////////////////////
// CModifyAttribute1Command Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CModifyAttribute1Command,CEditCommand)

CModifyAttribute1Command::CModifyAttribute1Command()
{
	m_nStep = -1;
	m_strValues = _T("0.1");
	strcat(m_strRegPath,"\\ModifyAttribute1");
}

CModifyAttribute1Command::~CModifyAttribute1Command()
{

}

CString CModifyAttribute1Command::Name()
{ 
	return StrFromResID(IDS_CMDNAME_ATTRIBUTE);
}

void CModifyAttribute1Command::Start()
{
	if( !m_pEditor )return;
	
	m_nStep = 0;
	m_pEditor->CloseSelector();
	CCommand::Start();
}

void CModifyAttribute1Command::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("ModifyAttribute1"),Name());
	
	param->AddLayerNameParam(PF_MODIFYATTRIBUTELAYERCODE,(LPCTSTR)m_strLayerName,StrFromResID(IDS_CMDPLANE_LAYERCODE));

	param->BeginOptionParam(PF_FIXTOEXT_OPTION,StrFromResID(IDS_FIXATTR));
	param->AddOption(StrFromResID(IDS_DIRPTDIR),dirPntDir,' ',m_nFieldOption==dirPntDir);
	param->AddOption(StrFromResID(IDS_FIELDNAME_GEOWIDTH),Width,' ',m_nFieldOption==Width);
	param->AddOption(StrFromResID(IDS_Z_VALUE),elevation,' ',m_nFieldOption==elevation);
	param->AddOption(StrFromResID(IDS_EXTATTR),extAttribute,' ',m_nFieldOption==extAttribute);
	param->EndOptionParam();

	if(m_nFieldOption==extAttribute)
	{
		param->AddParam(PF_MODIFYATTRIBUTEFIELD,(LPCTSTR)m_strFieldName, StrFromResID(IDS_CMDPLANE_FIELDNAME));
	}
		
	param->AddParam(PF_MODIFYATTRIBUTEVALUE,(LPCTSTR)m_strValues, StrFromResID(IDS_RANGE));
	
}

void CModifyAttribute1Command::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_MODIFYATTRIBUTELAYERCODE,var) )
	{
		m_strLayerName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_FIXTOEXT_OPTION,var) )
	{
		m_nFieldOption = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit) Abort();
	}
	if( tab.GetValue(0,PF_MODIFYATTRIBUTEFIELD,var) )
	{
		m_strFieldName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_MODIFYATTRIBUTEVALUE,var) )
	{
		m_strValues = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}

void CModifyAttribute1Command::GetParams(CValueTable& tab)
{
	_variant_t var;
	var = long(m_nFieldOption);
	tab.AddValue(PF_FIXTOEXT_OPTION,&CVariantEx(var));
	var = LPCTSTR(m_strLayerName);
	tab.AddValue(PF_MODIFYATTRIBUTELAYERCODE,&CVariantEx(var));
	var = LPCTSTR(m_strFieldName);
	tab.AddValue(PF_MODIFYATTRIBUTEFIELD,&CVariantEx(var));
	var = LPCTSTR(m_strValues);
	tab.AddValue(PF_MODIFYATTRIBUTEVALUE,&CVariantEx(var));
}


void CModifyAttribute1Command::Convert(CFeature *pFtr, int pos,double offset, CUndoModifyProperties& undo)
{	
	if(!pFtr)return;
	
	if( !m_strLayerName.IsEmpty() && !CAutoLinkContourCommand::CheckObjForLayerCode(GETDS(m_pEditor),pFtr,m_strLayerName) )
		return;	

	if(m_nFieldOption==extAttribute)//扩展属性
	{
		CValueTable oldTab, newTab;
		const CVariantEx* val;

		oldTab.BeginAddValueItem();
		GETXDS(m_pEditor)->GetXAttributes(pFtr,oldTab);
		oldTab.EndAddValueItem();

		CConfigLibManager *pCon = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
		if(pCon)
		{
			CScheme *pSch = pCon->GetScheme(GETDS(m_pEditor)->GetScale());
			if(pSch)
			{
				pSch->GetXAttributesAlias(m_strLayerName,oldTab);
			}
		}

		newTab.BeginAddValueItem();
		newTab.CopyFrom(oldTab);
		newTab.EndAddValueItem();

		CString field, name;
		int type;
		_variant_t var;
		int nField = newTab.GetFieldCount();
		for( int j=0; j<nField; j++)
		{		
			if(!newTab.GetField(j,field, type,name))continue;
			if( m_strFieldName.CompareNoCase(field)==0 || 
				m_strFieldName.CompareNoCase(name)==0 )
			{
				break;
			}
		}
		if( j>=nField ) return;
		if( type!=VT_R8 && type!=VT_R4 )
			return;
		if( !newTab.GetValue(0,field,val) )
			return;
		double value = (double)(_variant_t)*val;
		int last = int(value*pos+0.5)%10;//这个值四舍五入
		if( last>0 ) return;
		var = value+offset;
		if( newTab.SetValue(0,field,new CVariantEx(var)) )
		{
			GETXDS(m_pEditor)->SetXAttributes(pFtr,newTab);
			undo.SetModifyProperties(FtrToHandle(pFtr),oldTab,newTab,FALSE);
		}
	}
	else if(m_nFieldOption==dirPntDir)
	{
		CValueTable oldTab, newTab;
		const CVariantEx* val;

		oldTab.BeginAddValueItem();
		pFtr->WriteTo(oldTab);
		oldTab.EndAddValueItem();

		newTab.BeginAddValueItem();
		newTab.CopyFrom(oldTab);
		newTab.EndAddValueItem();

		m_strFieldName = "Angle";

		CString field, name;
		int type;
		_variant_t var;
		int nField = newTab.GetFieldCount();
		for( int j=0; j<nField; j++)
		{		
			if(!newTab.GetField(j,field, type,name))continue;
			if( m_strFieldName.CompareNoCase(field)==0 || 
				m_strFieldName.CompareNoCase(name)==0 )
			{
				break;
			}
		}
		if( j>=nField ) return;
		if( type!=VT_R8 && type!=VT_R4 )
			return;
		if( !newTab.GetValue(0,field,val) )
			return;
		double value = (double)(_variant_t)*val;
		int last = int(value*pos+0.5)%10;//这个值四舍五入
		if( last>0 ) return;
		var = value+offset;
		if( newTab.SetValue(0,field,new CVariantEx(var)) )
		{
			m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
			pFtr->ReadFrom(newTab);
			m_pEditor->RestoreObject(FtrToHandle(pFtr));
			undo.SetModifyProperties(FtrToHandle(pFtr),oldTab,newTab,TRUE);
		}
	}
	else if(m_nFieldOption==Width)
	{
		CGeometry *pObj = pFtr->GetGeometry();
		
		if( !pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			return;

		double wid = ((CGeoCurveBase*)pObj)->m_fLineWidth;
		int last = int(wid*pos+0.5)%10;//这个值四舍五入
		if( last>0 ) return;

		m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
		undo.arrHandles.Add(FtrToHandle(pFtr));
		undo.oldVT.BeginAddValueItem();
		pFtr->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();

		((CGeoCurveBase*)pObj)->m_fLineWidth = wid+offset;

		m_pEditor->RestoreObject(FtrToHandle(pFtr));
		undo.newVT.BeginAddValueItem();
		pFtr->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
	}
	else if(m_nFieldOption==elevation)
	{
		CGeometry *pObj = pFtr->GetGeometry();
		
		if( !pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
			return;

		if(pObj->GetDataPointSum()<1) return;
		PT_3DEX pt = pObj->GetDataPoint(0);
		double z = pt.z;
		int last = int(z*pos+0.5)%10;//这个值四舍五入
		if( last>0 ) return;
		pt.z = pt.z + offset;

		m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
		undo.arrHandles.Add(FtrToHandle(pFtr));
		undo.oldVT.BeginAddValueItem();
		pFtr->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();

		pObj->SetDataPoint(0, pt);
		
		m_pEditor->RestoreObject(FtrToHandle(pFtr));
		undo.newVT.BeginAddValueItem();
		pFtr->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
	}
}


void CModifyAttribute1Command::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor || m_strValues.IsEmpty()) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	CFtrLayer *pLayer = pDS->GetFtrLayer(m_strLayerName);
	if(!pLayer) return;
	int nObj = pLayer->GetObjectCount();

	CStringArray arr;
	convertStringToStrArray(m_strValues, arr);
	int num=arr.GetSize(), i;
	double *values = new double[num];
	for(i=0; i<num; i++)
	{
		values[i] = strtod(arr[i], NULL);
		if(values[i]<1e-6 || values[i]>=1)
		{
			AfxMessageBox(IDS_PARAM_ERROR);
			return;
		}
	}

	//若m_offset=0.5则pos=10，last=5 ； 若m_offset=0.04则pos=100，last=4
	int pos = 10;
	int last = (int(values[0]*pos))%10;
	while( last<1 )
	{
		pos *=10;
		last = (int(values[0]*pos))%10;
	}

	if(num>1)
	{
		srand((unsigned)time(NULL));//last=1时不需要随机
	}
	
	CUndoModifyProperties undo(m_pEditor,Name());
	GProgressStart(nObj);
	double offset = values[0];
	for ( i=0; i<nObj; i++)
	{
		GProgressStep();
		CFeature *pFtr = pLayer->GetObject(i);
		if( !pFtr )continue;

		if(num>1)
		{
			offset = values[rand()%num];
		}
		Convert(pFtr,pos,offset,undo);
	}

	delete[] values;
	GProgressEnd();
	undo.Commit();

	m_pEditor->RefreshView();
	Finish();
	CEditCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDrawSingleParallelCommand,CDrawCommand)

CDrawSingleParallelCommand::CDrawSingleParallelCommand()
{
	m_bMark = false;
	m_bTurnForChangeWid = false;
	m_nflag = 1;
	m_pFtr1 = NULL;
	m_pFtr2 = NULL;
	m_nStep = -1;
	m_lfcurWidth = 0;
	m_pFtr = NULL;
	strcat(m_strRegPath,"\\DrawSingleParallel");
}


CDrawSingleParallelCommand::~CDrawSingleParallelCommand()
{

	if(m_pFtr) delete m_pFtr;
}


CString CDrawSingleParallelCommand::Name()
{
	return StrFromLocalResID(IDS_CMDNAME_DRAWSINGLEPARALLEL);
}


void CDrawSingleParallelCommand::Start()
{
	m_bMark=false;
	m_bTurnForChangeWid=false;
	m_nflag=1;
	m_pFtr1=NULL;
	m_pFtr2=NULL;
	m_nStep=0;
	m_lfcurWidth=0;
	m_pFtr = NULL;

	m_pFtr = GETCURFTRLAYER(m_pEditor)->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOCURVE);
	if(!m_pFtr) return;

	CDlgDataSource *pDS = GETDS(m_pEditor);			

	m_pFtr1 = m_pFtr;
	if (m_pFtr1)
	{
		m_pFtr1->SetID(OUID());
	}
	m_pFtr2 = m_pFtr->Clone();
	if (m_pFtr2)
	{
		m_pFtr2->SetID(OUID());
	}

	CDrawCommand::Start();
}

void CDrawSingleParallelCommand::PtReset(PT_3D &pt)
{
	if ((m_pFtr==m_pFtr1&&m_ptsFtr1.GetSize()<2)||(m_pFtr==m_pFtr2&&m_ptsFtr2.GetSize()<2))
	{
		Abort();
		return;
	}
	if (m_pFtr==m_pFtr1)
	{
		PT_3DEX pts[2],ret[2];
		pts[0] = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-2);
		pts[1] = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
		ret[1].pencode = penLine;
		GraphAPI::GGetParallelLine(pts,2,m_lfcurWidth,ret);
		m_ptsFtr2.Add(ret[1]);
	}
	else
	{
		PT_3DEX pts[2],ret[2];
		pts[0] = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-2);
		pts[1] = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
		GraphAPI::GGetParallelLine(pts,2,m_lfcurWidth,ret);
		ret[1].pencode = penLine;
		m_ptsFtr1.Add(ret[1]);
	}
	Finish();
}

void CDrawSingleParallelCommand::PtClick(PT_3D &pt, int flag)
{
	
	if( !m_pFtr1||!m_pFtr2 || IsProcFinished(this) )return;
	
	GotoState(PROCSTATE_PROCESSING);

	if (m_nStep==0)
	{
		m_pts[0] = pt;
		m_nStep = 1;
	}
	else if (m_nStep==1)
	{
		
		m_pts[1] = pt;
		m_ptsFtr1.Add(PT_3DEX(pt,penLine));
		m_nStep = 2;
	}
	else if (m_nStep==2)
	{
		if (GraphAPI::GGetMultiply(m_pts[1],pt,m_pts[0])>0)
		{
			m_lfcurWidth = GraphAPI::GGetDisofPtToLine(m_pts[1].x,m_pts[1].y,pt.x,pt.y,m_pts[0].x,m_pts[0].y);
		}
		else
			m_lfcurWidth = -GraphAPI::GGetDisofPtToLine(m_pts[1].x,m_pts[1].y,pt.x,pt.y,m_pts[0].x,m_pts[0].y);
		
		PT_3DEX Pt1,Pt2;
		m_pts[2] = pt;
		m_ptsFtr1.Add(PT_3DEX(pt,penLine));
		PT_3D ret1[2];
		PT_3D Ptt[2];
		COPY_3DPT(Ptt[0],m_pts[1]);
		COPY_3DPT(Ptt[1],m_pts[2]);
		GraphAPI::GGetParallelLine(Ptt,2,m_lfcurWidth,ret1);
		COPY_3DPT(Pt1,ret1[0]);Pt1.pencode = penLine;
		COPY_3DPT(Pt2,ret1[1]);Pt2.pencode = penLine;
		
		m_ptsFtr2.Add(Pt1);

		m_addPt = Pt2;
		m_ptCross = ret1[1];
		
		GrBuffer cbuf;
		cbuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(),0);
		cbuf.MoveTo(&m_ptsFtr1[0]);
		cbuf.LineTo(&m_ptsFtr1[1]);
		cbuf.End();
		m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		m_nStep = 3;
	}
	
	else if (m_nStep>2)
	{
		if (m_pFtr == m_pFtr1)
		{
			m_ptsFtr1.Add(PT_3DEX(pt,penLine));
		}
		else
		{
			m_ptsFtr2.Add(PT_3DEX(pt,penLine));
		}	
		PT_3DEX Pt1,Pt2,Pt3,Pt4;	
		PT_3D ret[3];
		int npt;
		if (m_pFtr == m_pFtr1)
		{
			npt = m_ptsFtr1.GetSize();
		}
		else
		{
			npt = m_ptsFtr2.GetSize();
		}		
		if(npt>2)
		{
			if (m_bMark)
			{
				if (m_pFtr==m_pFtr1)
				{
					Pt1 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
					Pt2 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-2);
					Pt4 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-3);
					Pt3 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
					
				}
				else
				{
					Pt1 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
					Pt2 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-2);
					Pt4 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-3);
					Pt3 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
				}			

				double x1,y1;				
				GraphAPI::GGetRightAnglePoint(Pt4.x,Pt4.y,Pt2.x,Pt2.y,Pt3.x,Pt3.y,&x1,&y1);
				double tem = GraphAPI::GGetDisofPtToLine(Pt3.x,Pt3.y,x1,y1,pt.x,pt.y);
				m_lfcurWidth = m_lfcurWidth>0?tem:-tem;
				m_bMark=false;
				PT_3DEX ptex;
				ptex.x = x1;
				ptex.y = y1;
				ptex.z = Pt2.z;
				ptex.pencode = penLine;
				if (m_pFtr == m_pFtr1)
				{
					m_ptsFtr2.Add(ptex);
				}
				else
				{
					m_ptsFtr1.Add(ptex);
				}

				m_addPt = ptex;
				m_ptCross = PT_3D(x1,y1,Pt2.z);

				GrBuffer cbuf;
				PT_3D pt3d;		
				cbuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(),1);
				ptex = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-2);
				cbuf.MoveTo(&ptex);
				ptex = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);				
				cbuf.LineTo(&ptex);
				ptex = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-2);
				cbuf.MoveTo(&ptex);
				ptex = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);				
				cbuf.LineTo(&ptex);
				cbuf.End();
				m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
				m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			}
			else
			{
				if (m_pFtr == m_pFtr1)
				{
					Pt1 = m_ptsFtr1.GetAt(npt-3);
					Pt2 = m_ptsFtr1.GetAt(npt-2);
					Pt3 = m_ptsFtr1.GetAt(npt-1);
				}
				else
				{
					Pt1 = m_ptsFtr2.GetAt(npt-3);
					Pt2 = m_ptsFtr2.GetAt(npt-2);
					Pt3 = m_ptsFtr2.GetAt(npt-1);
				}				
				PT_3D Pt[3];
				COPY_3DPT(Pt[0],Pt1);
				COPY_3DPT(Pt[1],Pt2);
				COPY_3DPT(Pt[2],Pt3);
				
				GraphAPI::GGetParallelLine(Pt,3,m_lfcurWidth,ret);
				COPY_3DPT(Pt1,ret[0]);Pt1.pencode = penLine;
				COPY_3DPT(Pt2,ret[1]);Pt2.pencode = penLine;				
				if (m_pFtr == m_pFtr1)
				{
					m_ptsFtr2.Add(Pt2);
				}
				else
				{
					m_ptsFtr1.Add(Pt2);
				}

				COPY_3DPT(m_addPt,ret[2]);m_addPt.pencode = penLine;
				m_ptCross = ret[2];

				GrBuffer cbuf;
				PT_3DEX ptex;
				cbuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(),0);
				ptex = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-2);
				cbuf.MoveTo(&ptex);
				ptex = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
				cbuf.LineTo(&ptex);
				ptex = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-2);
				cbuf.MoveTo(&ptex);
				ptex = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
				cbuf.LineTo(&ptex);
				cbuf.End();
				m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
				m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
					
			}
		}

	}

	if (m_nStep >= 2 && m_pFtr1->GetGeometry()->GetDataPointSum() == 1 && PDOC(m_pEditor)->IsAutoSetAnchor())
	{
		PDOC(m_pEditor)->SetAnchorPoint(pt);
	}

	CCommand::PtClick(pt,flag);
}


void CDrawSingleParallelCommand::PtMove(PT_3D &pt)
{
	 if( !m_pFtr1 || !m_pFtr2 || IsProcFinished(this) )return;
	if(m_nStep==1)
	{
		GrBuffer vbuf1;
		vbuf1.BeginLineString(0,0);
		double len = GraphAPI::GGet2DDisOf2P(m_pts[0], pt);
		vbuf1.Dash(m_pts[0], pt, len/8, len/8);
		vbuf1.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf1);		
	}
	if(m_nStep==2)
 	{
 		double width;
 		if (GraphAPI::GGetMultiply(m_pts[1],pt,m_pts[0])>0)
			width = GraphAPI::GGetDisofPtToLine(m_pts[1].x,m_pts[1].y,pt.x,pt.y,m_pts[0].x,m_pts[0].y); 		
 		else
 			width = -GraphAPI::GGetDisofPtToLine(m_pts[1].x,m_pts[1].y,pt.x,pt.y,m_pts[0].x,m_pts[0].y); 
		PT_3DEX Pt1;
		if (m_pFtr==m_pFtr1)
		{
			Pt1 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
		}
		else
		{
			Pt1 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
		}
 		
 		PT_3D Pt[2];
 		COPY_3DPT(Pt[0],Pt1);
 		COPY_3DPT(Pt[1],pt);
 		PT_3D ret[2];
 		GraphAPI::GGetParallelLine(Pt,2,width,ret); 
		
		m_ptCross = ret[1];
		COPY_3DPT(m_addPt,ret[1]);m_addPt.pencode = penLine;

 		GrBuffer vbuf;
		vbuf.BeginLineString(0,0);
		vbuf.MoveTo(&Pt[0]);
		vbuf.LineTo(&Pt[1]);
		vbuf.MoveTo(&ret[0]);
		vbuf.LineTo(&ret[1]);
		vbuf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf); 			
	}	
	if (m_nStep>2)
	{
		PT_3DEX Pt1,Pt2,Pt3;
		if (m_bMark)
		{
			if (m_pFtr==m_pFtr1)
			{
				Pt1 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
				Pt2 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-2);
				Pt3 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);

			}
			else
			{
				Pt1 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
				Pt2 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-2);
				Pt3 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
			}
			GrBuffer vbuf;
			vbuf.BeginLineString(0,0);
			vbuf.MoveTo(&Pt1);
			vbuf.LineTo(&pt);
			double x,y,x1,y1;
					
			GraphAPI::GGetRightAnglePoint(Pt2.x,Pt2.y,Pt1.x,Pt1.y,Pt3.x,Pt3.y,&x1,&y1);				
			
			GraphAPI::GGetPerpendicular(x1,y1,Pt3.x,Pt3.y,pt.x,pt.y,&x,&y,NULL);
			Pt1.x = x;
			Pt1.y = y;
			Pt1.z = pt.z;          

			m_addPt = PT_3DEX(x1,y1,pt.z,penLine);
			m_ptCross = PT_3D(x,y,pt.z);

			vbuf.MoveTo(&Pt3);
			vbuf.LineTo(&Pt1);
			vbuf.End();	
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
		}
		else
		{			
			PT_3D ret[3];
			if (m_pFtr==m_pFtr1)
			{
				Pt1 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-2);
				Pt2 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
				Pt3 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
				
			}
			else
			{
				Pt1 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-2);
				Pt2 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
				Pt3 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
			}
			GrBuffer vbuf;
			vbuf.BeginLineString(0,0);
			vbuf.MoveTo(&Pt2);
			vbuf.LineTo(&pt);	
			
			PT_3D Pt[3];
			COPY_3DPT(Pt[0],Pt1);
			COPY_3DPT(Pt[1],Pt2);
			COPY_3DPT(Pt[2],pt);
			GraphAPI::GGetParallelLine(Pt,3,m_lfcurWidth,ret);

			m_ptCross = ret[2];
			COPY_3DPT(m_addPt,ret[1]);m_addPt.pencode = penLine;

			vbuf.MoveTo(&Pt3);
			vbuf.LineTo(&ret[1]);
			vbuf.LineTo(&ret[2]);
			vbuf.End();
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);			
		}	
	}
 	CCommand::PtMove(pt);
}


void CDrawSingleParallelCommand::Finish()
{
	UpdateParams(TRUE);	
	

	BOOL bCreated1 = FALSE, bCreated2 = FALSE;
	if (m_pFtr1->GetGeometry()->CreateShape(m_ptsFtr1.GetData(),m_ptsFtr1.GetSize()))
	{
		bCreated1 = m_pEditor->AddObject(m_pFtr1);

	}
	if (m_pFtr2->GetGeometry()->CreateShape(m_ptsFtr2.GetData(),m_ptsFtr2.GetSize()))
	{
		bCreated2 = m_pEditor->AddObject(m_pFtr2);
	}

	if (bCreated1 || bCreated2)
	{
		CUndoFtrs undo(m_pEditor,Name());
		if (bCreated1)
		{
			undo.arrNewHandles.Add(FtrToHandle(m_pFtr1));
		}
		if (bCreated2)
		{
			undo.arrNewHandles.Add(FtrToHandle(m_pFtr2));
		}
		
		undo.Commit();
	}
	if( m_pEditor )
	{
		if( m_pFtr1 )
		{			
			BOOL bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_ANNOTTOTEXT,FALSE);
			
			if( bAnnotToText )
			{
				CFtrLayer *pLayer = GETCURFTRLAYER(m_pEditor);
//				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByNameOrLayerIdx(StrFromResID(IDS_DEFLAYER_NAMET));
				CFtrLayer *pTextLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
				CFeature *pTempl = NULL;
				if(pTextLayer)
					pTempl = pTextLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOTEXT);
				if( pLayer!=NULL && pTextLayer!=NULL && pTempl!=NULL )
				{
					CPtrArray arrAnnots;
					CConfigLibManager *pCLM = gpCfgLibMan;
					pCLM->GetSymbol(GETDS(m_pEditor),m_pFtr1,arrAnnots);
					CUndoFtrs undo(m_pEditor,Name());
					for(int i=0; i<arrAnnots.GetSize(); i++)
					{
						CSymbol *pSymbol = (CSymbol*)arrAnnots.GetAt(i);
						if (pSymbol && pSymbol->GetType()==SYMTYPE_ANNOTATION )
						{
							CAnnotation *pAnnot = (CAnnotation*)pSymbol;							
							CPtrArray parr;
							CValueTable tab;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr1,tab);
							if( pAnnot->ExtractGeoText(m_pFtr1,parr,tab,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()) )
							{
								for (int j=0; j<parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr1->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);									
									PDOC(m_pEditor)->AddObject(pNewFtr,pTextLayer->GetID());									
									
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
									
								}
							}
							parr.RemoveAll();
							CValueTable tab1;
							PDOC(m_pEditor)->GetDlgDataSource()->GetAllAttribute(m_pFtr2,tab1);
							if( pAnnot->ExtractGeoText(m_pFtr2,parr,tab1,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale()) )
							{
								for (int j=0; j<parr.GetSize(); j++)
								{
									CGeoText *pText = (CGeoText*)parr[j];
									pText->SetColor(m_pFtr2->GetGeometry()->GetColor());
									CFeature *pNewFtr = pTempl->Clone();
									pNewFtr->SetGeometry(pText);									
									PDOC(m_pEditor)->AddObject(pNewFtr,pTextLayer->GetID());									
									
									undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
									
								}
							}
						}					
						
					}
					undo.Commit();					
				}
				if( pTempl!=NULL )delete pTempl;
			}
		}		
		m_pFtr = NULL;
		m_pEditor->RefreshView();
	}

	CDrawCommand::Finish();	
}


int  CDrawSingleParallelCommand::GetCurPenCode()
{
	return CDrawCommand::GetCurPenCode();
}

DrawingInfo CDrawSingleParallelCommand::GetCurDrawingInfo()
{
	if (!m_pFtr)return DrawingInfo();
	CArray<PT_3DEX,PT_3DEX> arr;
		m_pFtr->GetGeometry()->GetShape(arr);
	return DrawingInfo(m_pFtr,arr);
}


void CDrawSingleParallelCommand::Abort()
{
	if(m_pFtr) 
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	CDrawCommand::Abort();
}


int CDrawSingleParallelCommand::GetState()
{
	return CDrawCommand::GetState();
}


void CDrawSingleParallelCommand::Back()
{
	if (!m_pFtr) return;
	
	if (m_ptsFtr1.GetSize() < 1 || m_ptsFtr2.GetSize() < 1)
	{
		Abort();
		return;
	}

	m_ptsFtr1.RemoveAt(m_ptsFtr1.GetSize()-1);
	m_ptsFtr2.RemoveAt(m_ptsFtr2.GetSize()-1);

	if ( (m_pFtr == m_pFtr1 && m_ptsFtr1.GetSize() == 1) || 
		 (m_pFtr == m_pFtr2 && m_ptsFtr2.GetSize() == 1) )
	{
		m_nStep = 2;
	}
	
	CGeoCurve curve;
	GrBuffer cbuf,cbuf1,cbuf2,vbuf;
	if (curve.CreateShape(m_ptsFtr1.GetData(),m_ptsFtr1.GetSize()))
	{
		curve.Draw(&cbuf1,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
	}

	if (curve.CreateShape(m_ptsFtr2.GetData(),m_ptsFtr2.GetSize()))
	{
		curve.Draw(&cbuf2,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
	}

	cbuf.AddBuffer(&cbuf1);
	cbuf.AddBuffer(&cbuf2);

	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
//	m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
	m_pEditor->RefreshView();

}


void CDrawSingleParallelCommand::GetParams(CValueTable &tab)
{
	CDrawCommand::GetParams(tab);
	
	_variant_t var;
// 	var = (long)(m_pDrawProc->m_nCurPenCode);
// 	tab.AddValue(PF_PENCODE,&CVariantEx(var));
// 	var = (bool)(m_pDrawProc->m_bClosed);
// 	tab.AddValue(PF_CLOSED,&CVariantEx(var));
// 	var = (double)(m_pDrawProc->m_compress.GetLimit());
// 	tab.AddValue(PF_STREAMLIMIT,&CVariantEx(var));
	
}

void CDrawSingleParallelCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(PID_DRAWCURVE,StrFromLocalResID(IDS_CMDNAME_DRAWSINGLEPARALLEL));
	param->AddParam(PF_SINGLEPARALLEWIDTHCHANGE,'e',StrFromResID(IDS_CMDPLANE_ADJUSTWIDTH));
	param->AddParam(PF_SINGLEPARALLEBASECHANGE,'c',StrFromResID(IDS_CMDPLANE_CHANGESIDE));
	CDrawCommand::FillShowParams(param,bForLoad);
}


void CDrawSingleParallelCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_SINGLEPARALLEWIDTHCHANGE,var) )
	{
		char ch = (char)(BYTE)(_variant_t)*var;
		if (ch=='e')
		{
			m_bMark = TRUE;		
		}
		
	}
	if( tab.GetValue(0,PF_SINGLEPARALLEBASECHANGE,var) )
	{
		char ch = (char)(BYTE)(_variant_t)*var;
		if (ch=='c' && (m_ptsFtr1.GetSize()>=2 || m_ptsFtr2.GetSize()>=2))
		{
			if (m_pFtr==m_pFtr1)
			{
				m_ptsFtr2.Add(m_addPt);
				m_ptsFtr1.RemoveAt(m_ptsFtr1.GetSize()-1);
				m_pFtr = m_pFtr2;

			}
			else
			{
				m_ptsFtr1.Add(m_addPt);
				m_ptsFtr2.RemoveAt(m_ptsFtr2.GetSize()-1);
				m_pFtr = m_pFtr1;
				
			}

			m_lfcurWidth = -m_lfcurWidth;

			m_pEditor->UpdateDrag(ud_ClearDrag); 

			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&m_ptCross);

			GrBuffer cbuf;
			cbuf.BeginLineString(m_pFtr->GetGeometry()->GetColor(),0);
			cbuf.MoveTo(&m_ptsFtr1[0]);
			for (int i=1; i<m_ptsFtr1.GetSize(); i++)
			{					
				cbuf.LineTo(&m_ptsFtr1[i]);	
			}
			
			cbuf.MoveTo(&m_ptsFtr2[0]);
			for (i=1; i<m_ptsFtr2.GetSize(); i++)
			{					
				cbuf.LineTo(&m_ptsFtr2[i]);	
			}
			cbuf.End();
			m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
			
			if (m_nStep == 2)
			{
				PT_3DEX Pt1;
				if (m_pFtr==m_pFtr1)
				{
					Pt1 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
				}
				else
				{
					Pt1 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
				}
				
				PT_3D Pt[2];
				COPY_3DPT(Pt[0],Pt1);
				COPY_3DPT(Pt[1],m_ptCross);
				PT_3D ret[2];
				GraphAPI::GGetParallelLine(Pt,2,m_lfcurWidth,ret); 

				m_ptCross = ret[1];
				COPY_3DPT(m_addPt,ret[1]);m_addPt.pencode = penLine;
				
				GrBuffer vbuf;
				vbuf.BeginLineString(0,0);
				vbuf.MoveTo(&Pt[0]);
				vbuf.LineTo(&Pt[1]);
				vbuf.MoveTo(&ret[0]);
				vbuf.LineTo(&ret[1]);
				vbuf.End();

				m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf); 
	
			}	
			if (m_nStep>2)
			{
				PT_3DEX Pt1,Pt2,Pt3;
				PT_3D ret[3];
				if (m_pFtr==m_pFtr1)
				{
					Pt1 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-2);
					Pt2 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
					Pt3 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
				}
				else
				{
					Pt1 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-2);
					Pt2 = m_ptsFtr2.GetAt(m_ptsFtr2.GetSize()-1);
					Pt3 = m_ptsFtr1.GetAt(m_ptsFtr1.GetSize()-1);
				}
				GrBuffer vbuf;
				vbuf.BeginLineString(0,0);
				vbuf.MoveTo(&Pt2);
				vbuf.LineTo(&m_ptCross);	
				
				PT_3D Pt[3];
				COPY_3DPT(Pt[0],Pt1);
				COPY_3DPT(Pt[1],Pt2);
				COPY_3DPT(Pt[2],m_ptCross);
				GraphAPI::GGetParallelLine(Pt,3,m_lfcurWidth,ret);
				
				m_ptCross = ret[2];
				COPY_3DPT(m_addPt,ret[1]);m_addPt.pencode = penLine;
				
				vbuf.MoveTo(&Pt3);
				vbuf.LineTo(&ret[1]);
				vbuf.LineTo(&ret[2]);
				vbuf.End();
				m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);	
			}

			
		}
	}
// 	if( tab.GetValue(0,PF_STREAMLIMIT,var) )
// 	{
// 		m_pDrawProc->m_compress.SetLimit((double)(_variant_t)*var);
// 	}
// 	if( tab.GetValue(0,PF_POINT,var) )
// 	{
// 		CArray<PT_3DEX,PT_3DEX> arrPts;
// 		var->GetShape(arrPts);
// 		PT_3D point = arrPts.GetAt(0);
// 		PtClick(point,0);
// 		PtMove(point);
// 		m_pEditor->RefreshView();
// 	}
// 	
// 	if( m_pDrawProc->m_pGeoCurve )
// 		m_pDrawProc->m_pGeoCurve->EnableClose(m_pDrawProc->m_bClosed);	
//	SetSettingsModifyFlag();
	CDrawCommand::SetParams(tab,bInit);
}



BOOL CDrawSingleParallelCommand::Keyin(LPCTSTR text)
{
	// 通过长度和角度指定点
	char buf[100] = {0};

	strncpy(buf,text,100-1);
	char *stop = NULL;
	double len = strtod(buf,&stop);
	if (stop!=NULL && strlen(stop)==0)
	{		
		CArray<PT_3DEX,PT_3DEX> *pArray = NULL;
		if (m_pFtr == m_pFtr1)
		{
			pArray = &m_ptsFtr1;
		}
		else
		{
			pArray = &m_ptsFtr2;
		}	

		int num = pArray->GetSize();
		if (num > 0)
		{
			PT_3D curPt = m_pEditor->GetCoordWnd().m_ptGrd;
			PT_3DEX pt0 = pArray->GetAt(num-1), pt1;
			double dx = curPt.x-pt0.x, dy = curPt.y-pt0.y;
			double dis = sqrt(dx*dx+dy*dy);
			if(dis<GraphAPI::GetDisTolerance())
			{
				pt1.x = pt0.x + len;
				pt1.y = pt0.y;
				pt1.z = curPt.z;
			}
			else
			{
				pt1.x = pt0.x + dx/dis * len;
				pt1.y = pt0.y + dy/dis * len;
				pt1.z = curPt.z;
			}
			
			PtClick(pt1,0);
			PtMove(curPt);
			m_pEditor->RefreshView();

			return TRUE;
		}
	}

	return FALSE;
}


IMPLEMENT_DYNAMIC(CDlgEditVertexCommand,CEditVertexCommand)
CDlgEditVertexCommand::CDlgEditVertexCommand()
{
	m_pSel = NULL;
}

CDlgEditVertexCommand::~CDlgEditVertexCommand()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
}

void CDlgEditVertexCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(PID_EDITVERTEX,Name());
	
	param->BeginOptionParam(PID_EDITVERTEX,StrFromResID(IDS_CMDPLANE_GETPOINTZ));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bGetPointZ);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bGetPointZ);
	param->EndOptionParam();
	param->AddParam(PF_EDITVERTEX_DEL,char(VK_DELETE),StrFromResID(IDS_CMDPLANE_DELVERTEX));	
	param->AddParam(PF_EDITVERTEX_B,char('b'),StrFromResID(IDS_CMDPLANE_PRE_PT));
	param->AddParam(PF_EDITVERTEX_N,char('n'),StrFromResID(IDS_CMDPLANE_NEX_PT));//遍历节点的加速键
	param->AddParam(PF_EDITVERTEX_S,char('s'),StrFromResID(IDS_CMDPLANE_STA_PT));
	param->AddParam(PF_EDITVERTEX_E,char('e'),StrFromResID(IDS_CMDPLANE_END_PT));
}

void CDlgEditVertexCommand::GetParams(CValueTable& tab)
{
	CEditVertexCommand::GetParams(tab);
	
	_variant_t var;
	var = (long)(m_bGetPointZ);
	tab.AddValue(PID_EDITVERTEX,&CVariantEx(var));
}

void CDlgEditVertexCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	BOOL bChgPt = FALSE;
	if( tab.GetValue(0,PID_EDITVERTEX,var) )
	{
		m_bGetPointZ = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_EDITVERTEX_DEL,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key==VK_DELETE)
		{
			CUndoBatchAction batchUndo(m_pEditor,"EditVertexs");
			for (int i=0; i<m_arrKeyPts.GetSize(); i++)
			{
				if (!m_arrKeyPts[i].IsValid()) continue;
				
				CFeature *pFtr = m_arrFtrs[i];
				if( !pFtr )continue;
				
				CGeometry *pGeo = pFtr->GetGeometry();
				if( !pGeo )continue;

				int nPtNum = m_arrKeyPts[i].index;
				
				CDeleteVertexCommand::DeleteVertex(m_pEditor,pFtr,nPtNum,batchUndo,Name());				
			}

			batchUndo.Commit();

			m_pEditor->OnSelectChanged(TRUE);	
			m_pEditor->UpdateDrag(ud_ClearDrag);
			Finish();
			return;
		}		
	}
	if( tab.GetValue(0,PF_EDITVERTEX_B,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key=='b'||key=='B')
		{
			if (m_arrKeyPts.GetSize() != 1)
			{
				return;
			}

			PT_KEYCTRL &keyPt = m_arrKeyPts[0];
			if( (m_nStep==1||m_nStep==2) && keyPt.IsValid() )
			{
				if( keyPt.type==PT_KEYCTRL::typeKey )
				{
					CArray<PT_3DEX,PT_3DEX> arr;
					m_arrFtrs[0]->GetGeometry()->GetShape(arr);
					if( arr.GetSize()>1 )
					{
						keyPt.index = keyPt.index-1;
						if( keyPt.index<0 )keyPt.index = arr.GetSize()-1;
// 						if( m_arrFtrs[0]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
// 						{
// 							if(((CGeoCurve*)(m_arrFtrs[0]->GetGeometry()))->IsClosed() && (keyPt.index==0 || keyPt.index==arr.GetSize()-1))  
// 								m_arrPtsRepeat.SetAt(0,TRUE);
// 							else
// 								m_arrPtsRepeat.SetAt(0,FALSE);
// 						}
// 						else if( m_arrFtrs[0]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
// 						{
// 							m_arrPtsRepeat.SetAt(0,TRUE);
// 						}
						PT_3DEX t;
						t = arr.GetAt(keyPt.index);
						m_ptDragStart.x = t.x;
						m_ptDragStart.y = t.y;
						m_ptDragStart.z = t.z;
						bChgPt = TRUE;
					}
				}
				else
				{
					if( m_arrFtrs[0]->GetGeometry()->GetCtrlPointSum()>1 )
					{
						keyPt.index = keyPt.index-1;
						if( keyPt.index<0 )keyPt.index = m_arrFtrs[0]->GetGeometry()->GetCtrlPointSum()-1;
						PT_3D t;
						t = m_arrFtrs[0]->GetGeometry()->GetCtrlPoint(keyPt.index);
						m_ptDragStart.x = t.x;
						m_ptDragStart.y = t.y;
						m_ptDragStart.z = t.z;
						bChgPt = TRUE;
					}
				}
			}		
		}		
	}
	if( tab.GetValue(0,PF_EDITVERTEX_N,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key=='n'||key=='N')
		{
			if (m_arrKeyPts.GetSize() != 1)
			{
				return;
			}
			
			PT_KEYCTRL &keyPt = m_arrKeyPts[0];
			if( (m_nStep==1||m_nStep==2) && keyPt.IsValid() )
			{
				if( keyPt.type==PT_KEYCTRL::typeKey )
				{
					CArray<PT_3DEX,PT_3DEX> arr;
					m_arrFtrs[0]->GetGeometry()->GetShape(arr);
					if( arr.GetSize()>1 )
					{
						keyPt.index = keyPt.index+1;
						if( keyPt.index>=arr.GetSize() )keyPt.index = 0;
// 						if( m_arrFtrs[0]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
// 						{
// 							if(((CGeoCurve*)(m_arrFtrs[0]->GetGeometry()))->IsClosed() && (keyPt.index==0 || keyPt.index==arr.GetSize()-1)) 
// 								m_arrPtsRepeat.SetAt(0,TRUE);
// 							else
// 								m_arrPtsRepeat.SetAt(0,FALSE);
// 						}
// 						else if( m_arrFtrs[0]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
// 						{
// 							m_arrPtsRepeat.SetAt(0,FALSE);
// 						}
						PT_3DEX t;
						t = arr.GetAt(keyPt.index);
						m_ptDragStart.x = t.x;
						m_ptDragStart.y = t.y;
						m_ptDragStart.z = t.z;
						bChgPt = TRUE;
					}
				}
				else
				{
					if( m_arrFtrs[0]->GetGeometry()->GetCtrlPointSum()>1 )
					{
						keyPt.index = keyPt.index+1;
						if( keyPt.index>=m_arrFtrs[0]->GetGeometry()->GetCtrlPointSum())keyPt.index = 0;
						PT_3D t;
						t = m_arrFtrs[0]->GetGeometry()->GetCtrlPoint(keyPt.index);
						m_ptDragStart.x = t.x;
						m_ptDragStart.y = t.y;
						m_ptDragStart.z = t.z;
						bChgPt = TRUE;
					}
				}
			}
		}		
	}
	if( tab.GetValue(0,PF_EDITVERTEX_S,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key=='s'||key=='S')
		{
			if (m_arrKeyPts.GetSize() != 1)
			{
				return;
			}
			
			PT_KEYCTRL &keyPt = m_arrKeyPts[0];
			if( (m_nStep==1||m_nStep==2) && keyPt.IsValid() )
			{
				if( keyPt.type==PT_KEYCTRL::typeKey )
				{
					CArray<PT_3DEX,PT_3DEX> arr;
					m_arrFtrs[0]->GetGeometry()->GetShape(arr);
					if( arr.GetSize()>1 )
					{
						keyPt.index = 0;	
// 						if( m_arrFtrs[0]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
// 						{
// 							if(((CGeoCurve*)(m_arrFtrs[0]->GetGeometry()))->IsClosed() ) 
// 								m_arrPtsRepeat.SetAt(0,TRUE);
// 							else
// 								m_arrPtsRepeat.SetAt(0,FALSE);
// 						}
// 						else if( m_arrFtrs[0]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
// 						{							
// 							m_arrPtsRepeat.SetAt(0,FALSE);							
// 						}
						PT_3DEX t;
						t = arr.GetAt(keyPt.index);
						m_ptDragStart.x = t.x;
						m_ptDragStart.y = t.y;
						m_ptDragStart.z = t.z;
						bChgPt = TRUE;
					}
				}
				else
				{
					if( m_arrFtrs[0]->GetGeometry()->GetCtrlPointSum()>1 )
					{
						keyPt.index = 0;						
						PT_3D t;
						t = m_arrFtrs[0]->GetGeometry()->GetCtrlPoint(keyPt.index);
						m_ptDragStart.x = t.x;
						m_ptDragStart.y = t.y;
						m_ptDragStart.z = t.z;
						bChgPt = TRUE;
					}
				}
			}
		}		
	}
	if( tab.GetValue(0,PF_EDITVERTEX_E,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key=='e'||key=='E')
		{
			if (m_arrKeyPts.GetSize() != 1)
			{
				return;
			}
			
			PT_KEYCTRL &keyPt = m_arrKeyPts[0];
			if( (m_nStep==1||m_nStep==2) && keyPt.IsValid() )
			{
				if( keyPt.type==PT_KEYCTRL::typeKey )
				{
					CArray<PT_3DEX,PT_3DEX> arr;
					m_arrFtrs[0]->GetGeometry()->GetShape(arr);
					if( arr.GetSize()>1 )
					{
						keyPt.index = arr.GetSize()-1;	
// 						if( m_arrFtrs[0]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
// 						{
// 							if(((CGeoCurve*)(m_arrFtrs[0]->GetGeometry()))->IsClosed() ) 
// 								m_arrPtsRepeat.SetAt(0,TRUE);
// 							else
// 								m_arrPtsRepeat.SetAt(0,FALSE);
// 						}
// 						else if( m_arrFtrs[0]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
// 						{							
// 							m_arrPtsRepeat.SetAt(0,FALSE);							
// 						}
						PT_3DEX t;
						t = arr.GetAt(keyPt.index);
						m_ptDragStart.x = t.x;
						m_ptDragStart.y = t.y;
						m_ptDragStart.z = t.z;
						bChgPt = TRUE;
					}
				}
				else
				{
					if( m_arrFtrs[0]->GetGeometry()->GetCtrlPointSum()>1 )
					{
						keyPt.index = m_arrFtrs[0]->GetGeometry()->GetCtrlPointSum()-1;						
						PT_3D t;
						t = m_arrFtrs[0]->GetGeometry()->GetCtrlPoint(keyPt.index);
						m_ptDragStart.x = t.x;
						m_ptDragStart.y = t.y;
						m_ptDragStart.z = t.z;
						bChgPt = TRUE;
					}
				}
			}
		}		
	}
	if (bChgPt)
	{
// 		delete m_pObj;
// 		m_pObj = m_pFtr->GetGeometry()->Clone();
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&m_ptDragStart);
		m_ptDragEnd = m_ptDragStart;
		m_pEditor->UpdateDrag(ud_ClearDrag);
		PtMove(m_ptDragEnd);
	}
	CEditVertexCommand::SetParams(tab,bInit);

	if( bChgPt )
	{
		OutputVertexInfo(m_arrKeyPts[0],m_arrFtrs[0]->GetGeometry());
	}
}


DrawingInfo CDlgEditVertexCommand::GetCurDrawingInfo()
{
	if (m_arrFtrs.GetSize() < 1)return DrawingInfo();
	
	if (m_arrKeyPts.GetSize() < 1) return DrawingInfo();

	CArray<PT_3DEX,PT_3DEX> arr;
	m_arrFtrs[0]->GetGeometry()->GetShape(arr);

	DrawingInfo info(m_arrFtrs[0],arr);

	if (m_arrKeyPts[0].type == PT_KEYCTRL::typeCtrl)
	{
		info.lastPts.Add(m_arrFtrs[0]->GetGeometry()->GetCtrlPoint(m_arrKeyPts[0].index));
	}
	else
	{
		int size = arr.GetSize();

		int index = m_arrKeyPts[0].index;
		if (index == 0)
		{
			if (size > 1)
			{
				info.lastPts.Add(arr[1]);
			}			
		}
		else if (index == size-1)
		{
			if (size > 1)
			{
				info.lastPts.Add(arr[size-2]);
			}			
		}
		else 
		{
			info.lastPts.Add(arr[index-1]);
			info.lastPts.Add(arr[index]);
			info.lastPts.Add(arr[index+1]);
		}
		
	}
	
	return info;
}

void CDlgEditVertexCommand::PtClick(PT_3D &pt, int flag)
{
	CEditVertexCommand::PtClick(pt,flag);

	if (m_nStep == 1)
	{
		if (m_arrFtrs.GetSize() > 0 && m_arrKeyPts.GetSize() > 0)
		{
			m_pSel = new CSelChangedExchanger(m_arrFtrs[0],(CDlgDoc*)m_pEditor,m_arrKeyPts[0].index);
			AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
		}

		//二维窗口
		if( !m_pEditor->GetCoordWnd().m_bIsStereo )
		{
			pt.z = m_ptDragStart.z;
		}
		else if( m_bGetPointZ )
		{
			((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_ModifyHeight,(CObject*)&m_ptDragStart.z);
		}
	}

	return;
}

void CDlgEditVertexCommand::PtDblClick(PT_3D &pt, int flag)
{
	BOOL bIsText = FALSE;

	CSelection *pselection = m_pEditor->GetSelection();
	
	CPFeature pFtr = NULL;
	CGeoText *pText = NULL;
	int nsel = 0;
	pselection->GetSelectedObjs(nsel);
	if( nsel==1 )
	{
		pFtr = HandleToFtr(pselection->GetLastSelectedObj());
		
		CGeometry *pGeo = pFtr->GetGeometry();
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			pText = (CGeoText*)pGeo;
			bIsText = TRUE;
		}
	}

	if( bIsText )
	{
		CDlgEditText dlg;
		dlg.m_strEdit = pText->GetText();
		CString strOld = dlg.m_strEdit;
		
		if( dlg.DoModal()!=IDOK )
			return;
		
		CUndoModifyProperties undo(m_pEditor,StrFromResID(IDS_UNDO_EDITTEXTCONTENT));
		
		m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
		pText->SetText(dlg.m_strEdit);		
		m_pEditor->RestoreObject(FtrToHandle(pFtr));

		undo.SetModifyProp(pFtr,FIELDNAME_GEOTEXT_CONTENT,&CVariantEx((_variant_t)(LPCTSTR)strOld),
			&CVariantEx((_variant_t)(LPCTSTR)dlg.m_strEdit));
		
		undo.Commit();
		
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->OnSelectChanged(TRUE);			
		
		Finish();
		m_pEditor->CloseSelector();
		m_nStep = 3;

		return;
	}

	if( m_nStep==0 )PtClick(pt,flag);
	if( m_nStep==1 )
	{
		CDlgSetXYZ1 dlg;
		dlg.m_lfX = m_ptDragStart.x;
		dlg.m_lfY = m_ptDragStart.y;
		dlg.m_lfZ = m_ptDragStart.z;
		if( dlg.DoModal()==IDOK )
		{
			PT_3D newPt(dlg.m_lfX,dlg.m_lfY,dlg.m_lfZ);
			PtClick(newPt,flag);
		}
	}
	
	return;
}

void CDlgEditVertexCommand::OutputVertexInfo(PT_KEYCTRL pt, CGeometry *pGeo)
{
	int nxy = floor(-log10(GraphAPI::g_lfDisTolerance)+0.5);
	int nz = floor(-log10(GraphAPI::g_lfZTolerance)+0.5);

	CString str;
	if( pt.type==PT_KEYCTRL::typeKey )
	{		
		PT_3DEX t;
		t = pGeo->GetDataPoint(pt.index);
		
		CString strStyle;
		switch(t.pencode) {
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
		default:;
		}

		CString strFormat;
		strFormat.Format("%%.%df",nxy);

		CString strX, strY, strZ;
		strX.Format((LPCTSTR)strFormat,t.x);
		strY.Format((LPCTSTR)strFormat,t.y);

		strFormat.Format("%%.%df",nz);
		strZ.Format((LPCTSTR)strFormat,t.z);
		
		str.Format(IDS_CMDTIP_VERTEXINFO_EX,
			pt.index,(LPCTSTR)strX,(LPCTSTR)strY,(LPCTSTR)strZ);
		if( pGeo->GetDataPointSum()>1 )
		{
			CString str2;
			str2.Format(", %s: %s",(LPCTSTR)StrFromResID(IDS_CMDPLANE_LINETYPE),(LPCTSTR)strStyle);
			str += str2;
			
			str2.Format(", %s: %.4f",(LPCTSTR)StrFromResID(IDS_CMDTIP_NODEWID),t.wid);
			str += str2;
		}
		
		str += _T("\n");
	}
	else
	{
		PT_3D t;
		t = pGeo->GetCtrlPoint(pt.index);

		CString strFormat;
		strFormat.Format("%%.%df",nxy);
		
		CString strX, strY, strZ;
		strX.Format((LPCTSTR)strFormat,t.x);
		strY.Format((LPCTSTR)strFormat,t.y);
		
		strFormat.Format("%%.%df",nz);
		strZ.Format((LPCTSTR)strFormat,t.z);
		
		str.Format(IDS_CMDTIP_CTRLPTINFO_EX,
			(LPCTSTR)strX,(LPCTSTR)strY,(LPCTSTR)strZ);
	}
	
	PromptString(str);
}

void CDlgEditVertexCommand::PtMove(PT_3D &pt)
{
	GrBuffer buf, buf1;
	
	double matrix[16];
	Matrix44FromMove(pt.x-m_ptDragStart.x,pt.y-m_ptDragStart.y,pt.z-m_ptDragStart.z,matrix);
	
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	
	// 不在立体上显示点状地物符号
	UpdateViewType updateview = uv_AllView;

	int num = m_arrFtrs.GetSize();
	for( int i=0; i<num && num<100; i++)
	{
		CFeature *pFtr0 = m_arrFtrs.GetAt(i);
		CFeature *pFtr = pFtr0->Clone();
		if (!pFtr) continue;

		buf.DeleteAll();

		CGeometry *pGeo = pFtr->GetGeometry();
		//pGeo->Transform(matrix);
		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);

			if (pts.GetSize() > 1) 
			{
				delete pFtr;
				continue;
			}

			CPlotTextEx plot;
			TEXT_SETTINGS0  settings;
			((CGeoText*)pGeo)->GetSettings(&settings);
			
			settings.fHeight *= pDS->GetAnnotationScale();
			plot.SetSettings(&settings);	
			
			plot.SetShape(((CGeoText*)pGeo)->GetShape());
			plot.SetText(((CGeoText*)pGeo)->GetText());
			plot.GetOutLineBuf(&buf);

			buf.Transform(matrix);
		}
		else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		{
			updateview = uv_VectorView;

			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) && m_arrKeyPts[0].type==PT_KEYCTRL::typeCtrl)
			{
				//if( m_arrKeyPts[0].type==PT_KEYCTRL::typeCtrl )
				{				
					m_ptDragEnd = pt;	
										
					PT_3D t;
					t = pGeo->GetCtrlPoint(m_arrKeyPts[0].index);
					t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
					pGeo->SetCtrlPoint(m_arrKeyPts[0].index,t);
					
					CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
					CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr0);
					
					if( pLayer )
					{
						pDS->DrawFeature(pFtr,&buf,TRUE,0,pLayer->GetName());						
					}
					
				}
			}
			else
			{
				CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr0);			
				if( pLayer )
				{
					pDS->DrawFeature(pFtr,&buf,TRUE,0,pLayer->GetName());
				}

				buf.Transform(matrix);
			}
		}
		
		buf1.AddBuffer(&buf);
		
		delete pFtr;
		
	}

	
	m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf,updateview);

	CEditVertexCommand::PtMove(pt);
}

BOOL CDlgEditVertexCommand::HitTest(PT_3D pt, int state)const
{
	CSelection *pselection = m_pEditor->GetSelection();
	
	CPFeature pFtr = NULL;
	int nsel = 0;
	pselection->GetSelectedObjs(nsel);
	if( nsel==1 && state==stateDblClick )
	{
		pFtr = HandleToFtr(pselection->GetLastSelectedObj());

		CGeometry *pGeo = pFtr->GetGeometry();		
		GrBuffer buf;
		pGeo->Draw(&buf,GetSymbolDrawScale());
		Envelope e = buf.GetEnvelope();
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) && e.bPtIn(&pt) )
		{
			return TRUE;
		}
	}

	return CEditVertexCommand::HitTest(pt, state);
}

void CDlgEditVertexCommand::Abort()
{
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);

	CEditVertexCommand::Abort();
}


void CDlgEditVertexCommand::Finish()
{
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	
	CEditVertexCommand::Finish();
}


IMPLEMENT_DYNAMIC(CDlgInsertVertexCommand, CInsertVertexCommand)
CDlgInsertVertexCommand::CDlgInsertVertexCommand()
{
	
}

CDlgInsertVertexCommand::~CDlgInsertVertexCommand()
{
	
}

void CDlgInsertVertexCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(PID_EDITVERTEX,Name());
	
	param->BeginOptionParam("GetPointZ",StrFromResID(IDS_CMDPLANE_GETPOINTZ));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bGetPointZ);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bGetPointZ);
	param->EndOptionParam();
	param->AddParam(PF_INSERTVERTEX_B,char('b'),StrFromResID(IDS_CMDPLANE_PRE_PT));
	param->AddParam(PF_INSERTVERTEX_N,char('n'),StrFromResID(IDS_CMDPLANE_NEX_PT));//遍历节点的加速键
	param->AddParam(PF_INSERTVERTEX_S,char('s'),StrFromResID(IDS_CMDPLANE_STA_PT));
	param->AddParam(PF_INSERTVERTEX_E,char('e'),StrFromResID(IDS_CMDPLANE_END_PT));	
}

void CDlgInsertVertexCommand::GetParams(CValueTable& tab)
{
	CInsertVertexCommand::GetParams(tab);

	_variant_t var;
	var = (bool)(m_bGetPointZ);
	tab.AddValue("GetPointZ",&CVariantEx(var));
}

void CDlgInsertVertexCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	int num = 0;
	
	if(m_arrFtrs.GetSize()==1)
		num = m_arrPtIndex[0];

	int nPtNum = num;

	if( tab.GetValue(0,"GetPointZ",var) )
	{
		m_bGetPointZ = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_INSERTVERTEX_B,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key=='b'||key=='B')
		{
			num--;
		}
	}
	if( tab.GetValue(0,PF_INSERTVERTEX_N,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key=='n'||key=='N')
		{
			num++;
		}		
	}
	if( tab.GetValue(0,PF_INSERTVERTEX_S,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key=='s'||key=='S')
		{
			num = 0;
		}		
	}
	if( tab.GetValue(0,PF_INSERTVERTEX_E,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key=='e'||key=='E')
		{
			num = -1;	
		}		
	}

	if (m_arrFtrs.GetSize()==1)
	{
		CFeature *pFtr = m_arrFtrs[0];
		CArray<PT_3DEX,PT_3DEX> arr;
		pFtr->GetGeometry()->GetShape(arr);
		if( (m_nStep==1||m_nStep==2) && arr.GetSize()>2 )
		{
			if( num<0 )num = arr.GetSize();
			else if( num>arr.GetSize()-1 )num = 0;
			if( nPtNum!=num )
			{
				nPtNum = num;
				if( nPtNum==arr.GetSize() && pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				{
					if( ((CGeoCurve*)(pFtr->GetGeometry()))->IsClosed() )
						nPtNum--;
				}

				m_arrPtIndex[0] = nPtNum;

				PT_3D keyPt = m_arrFtrs[0]->GetGeometry()->GetDataPoint(m_arrPtIndex[0]);

				PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&keyPt);

				PtMove(keyPt);
				m_pEditor->RefreshView();
			}
		}
	}
	
	CInsertVertexCommand::SetParams(tab,bInit);
}

DrawingInfo CDlgInsertVertexCommand::GetCurDrawingInfo()
{
	if (m_arrFtrs.GetSize() <= 0)return DrawingInfo();
	
	CFeature *pFtr = m_arrFtrs[0];

	CArray<PT_3DEX,PT_3DEX> arr;
	pFtr->GetGeometry()->GetShape(arr);
	
	DrawingInfo info(pFtr,arr);
	
	int size = arr.GetSize();
	
	int index = m_arrPtIndex[0];
	if (index == 0)
	{
		if (size > 0)
		{
			info.lastPts.Add(arr[0]);
		}			
	}
	else if (index == size)
	{
		if (size > 1)
		{
			info.lastPts.Add(arr[size-1]);
		}			
	}
	else 
	{
		info.lastPts.Add(arr[index-1]);
		info.lastPts.Add(arr[index]);
	}
	
	return info;
}

void CDlgInsertVertexCommand::PtClick(PT_3D &pt, int flag)
{
	CInsertVertexCommand::PtClick(pt,flag);
	
	if (m_nStep == 1)
	{
		if(m_arrFtrs.GetSize()>0)
		{
			PT_3D keyPt = m_arrFtrs[0]->GetGeometry()->GetDataPoint(m_arrPtIndex[0]);
			//二维窗口
			if( !m_pEditor->GetCoordWnd().m_bIsStereo )
			{
				pt.z = keyPt.z;
			}
			else if( m_bGetPointZ )
			{
				((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_ModifyHeight,(CObject*)&keyPt.z);
			}
		}
	}
	
	return;
}


//////////////////////////////////////////////////////////////////////
// CSnapLayerCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSnapLayerCommand,CCommand)

CSnapLayerCommand::CSnapLayerCommand()
{
	m_bSnapHeight = FALSE;
}

CSnapLayerCommand::~CSnapLayerCommand()
{
}

CString CSnapLayerCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SNAPLAYER);
}

void CSnapLayerCommand::Start()
{
	if( m_pEditor )
	{
		int num = 0;
		m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num>0 )
		{
			CCommand::Start();
			
			PT_3D pt = m_pEditor->GetCoordWnd().m_ptGrd;
			PtClick(pt,0);
			m_nExitCode = CMPEC_STARTOLD;
			return;
		}
		
		m_pEditor->OpenSelector();
	}
	
	CCommand::Start();
}

void CSnapLayerCommand::GetParams(CValueTable &tab)
{
	CCommand::GetParams(tab);
	_variant_t var;
	var = (bool)(m_bSnapHeight);
	tab.AddValue("bSnapHeight", &CVariantEx(var));
}

void CSnapLayerCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("SnapLayerCommand", Name());
	param->AddParam("bSnapHeight", bool(m_bSnapHeight), StrFromResID(IDS_SNAPHEIGHT));
}

void CSnapLayerCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;

	if (tab.GetValue(0, "bSnapHeight", var))
	{
		m_bSnapHeight = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CCommand::SetParams(tab, bInit);
}

void CSnapLayerCommand::Abort()
{
	if( m_pEditor )
	{
		m_pEditor->CloseSelector();
	}
	
	CCommand::Abort();
	m_nExitCode = CMPEC_STARTOLD;
}

void CSnapLayerCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	GotoState(PROCSTATE_PROCESSING);
	int num ;
	const FTR_HANDLE* handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num<=0 )
	{
		Abort();
		return;	
	}
	
	if( handles[0] )
	{
		int nLayerID = m_pEditor->GetFtrLayerIDOfFtr(handles[0]);
		CGeometry *pGeo = HandleToFtr(handles[0])->GetGeometry();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged(TRUE);
		m_pEditor->RefreshView();
		Finish();
		
		GETDS(m_pEditor)->SetCurFtrLayer(nLayerID);
		PDOC(m_pEditor)->ActiveLayer();

		if (m_bSnapHeight)
		{
			if (pGeo && pGeo->GetDataPointSum() > 0)
			{
				PT_3DEX pt0 = pGeo->GetDataPoint(0);
				CView *pView = GetActiveView();
				if (pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
				{
					PT_3D pt = ((CVectorView_new*)pView)->GetCrossPoint();
					pt.z = pt0.z;
					CVectorView_new *pVV = (CVectorView_new*)pView;
					pVV->DriveToXyz(&pt, FALSE);
				}
				else if (pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
				{
					PT_3D pt = ((CStereoView*)pView)->GetCrossPoint();
					pt.z = pt0.z;
					CStereoView *pVV = (CStereoView*)pView;
					pVV->DriveToXyz(&pt, 1);
				}
			}
		}

		return;
		
	}
	CCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// CTrimLayerCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CTrimLayerCommand,CEditCommand)

CTrimLayerCommand::CTrimLayerCommand()
{
	m_bTrimOut = FALSE;
	m_nStep = -1;
	m_bNotChangeZ = TRUE;
	
	strcat(m_strRegPath,"\\TrimLayer");
}

CTrimLayerCommand::~CTrimLayerCommand()
{
}

CString CTrimLayerCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TRIMLAYER);
}


CFtrLayer *CTrimLayerCommand::GetLayer(LPCTSTR fid)
{
	CDlgDoc *pDoc = (CDlgDoc*)(m_pEditor);
	CFtrLayer *pLayer = NULL;
	if( fid!=NULL )
		pLayer = pDoc->GetDlgDataSource()->GetFtrLayer(fid);
	
	if( pLayer==NULL )
	{
		pLayer = new CFtrLayer();
		pLayer->SetName(fid);
		
		pDoc->AddFtrLayer(pLayer);
	}
	
	return pLayer;
}

void CTrimLayerCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(_T("TrimLayer"),StrFromResID(IDS_CMDNAME_TRIMLAYER));
	param->AddLayerNameParam(PF_TRIMLAYER_REF,(LPCTSTR)m_strRefLay,StrFromResID(IDS_CMDPLANE_REFLAY));
	param->AddLayerNameParam(PF_TRIMLAYER_TRI,(LPCTSTR)m_strTrimLay,StrFromResID(IDS_CMDPLANE_TRIMLAY));
	param->AddLayerNameParam(PF_TRIMLAYER_DES,(LPCTSTR)m_strRetLay,StrFromResID(IDS_PLANENAME_PLACELAYER));
	param->BeginOptionParam(PF_TRIMLAYER_KEEPZ,StrFromResID(IDS_CMDPLANE_TRIMLAYERZ));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bNotChangeZ);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bNotChangeZ);
	param->EndOptionParam();	

}

void CTrimLayerCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;	
	var = (_bstr_t)(LPCTSTR)m_strRefLay;
	tab.AddValue(PF_TRIMLAYER_REF,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strTrimLay;
	tab.AddValue(PF_TRIMLAYER_TRI,&CVariantEx(var));	
	var = (_bstr_t)(LPCTSTR)m_strRetLay;
	tab.AddValue(PF_TRIMLAYER_DES,&CVariantEx(var));
	var = (bool)m_bNotChangeZ;
	tab.AddValue(PF_TRIMLAYER_KEEPZ,&CVariantEx(var));

}


void CTrimLayerCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_TRIMLAYER_REF,var) )
	{
		m_strRefLay = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_TRIMLAYER_TRI,var) )
	{
		m_strTrimLay = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_TRIMLAYER_DES,var) )
	{
		m_strRetLay = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_TRIMLAYER_KEEPZ,var) )
	{
		m_bNotChangeZ = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}


void CTrimLayerCommand::Start()
{
	m_bTrimOut = FALSE;
	m_nStep = 0;
	m_bNotChangeZ = TRUE;
	m_pEditor->CloseSelector();
	CCommand::Start();
}

void CTrimLayerCommand::Abort()
{	
	m_nStep = -1;
	m_pEditor->CloseSelector();
	CEditCommand::Abort();
}

void CTrimLayerCommand::Finish()
{
	m_nStep = -1;
	m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
	m_pEditor->CloseSelector();
	m_pEditor->RefreshView();
	
	CCommand::Finish();
}


static BOOL CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  fid)
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
	
	return FALSE;
}

void CTrimLayerCommand::PtClick(PT_3D &pt, int flag)
{
	//确认并执行
	if( m_nStep==0 )
	{
		//获得参考层
		if( m_strRefLay.IsEmpty() )
			return;

		// 结果层名字转换
		CString strRetLayName = m_strRetLay;

		COLORREF color = RGB(255,0,0);
		
		CFeature *pObj = PDOC(m_pEditor)->GetDlgDataSource()->CreateObjByNameOrLayerIdx(strRetLayName);
		if( pObj!=NULL )
		{
			color = pObj->GetGeometry()->GetColor();
			delete pObj;
		}

		CDlgDataSource *pDS =  GETDS(m_pEditor);
		if(!pDS) return;
		//计算进度条
		int i, nsum = 0, nLayer = pDS->GetFtrLayerCount();
		for ( i=0; i<nLayer; i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			if (!pLayer)  continue;
			int nobj = pLayer->GetObjectCount();
			if(!pLayer||!pLayer->IsVisible()) continue;

			CFeature *pFtr = NULL;
			for (int j=0;j<nobj;j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;
				nsum++;
			}
		}

		GProgressStart(nsum);

		//获得参考层的闭合边界
		CPtrArray arrPolys;
		for ( i=0; i<nLayer; i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible()) continue;
			int nobj = pLayer->GetObjectCount();
			CFeature *pFtr = NULL;
			for (int j=0;j<nobj;j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;
				//跳过参考层
				if( !CheckObjForLayerCode(pDS, pFtr, m_strRefLay) )
					continue;				

				if( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || 
					pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				{
					if( pFtr->GetGeometry()->GetDataPointSum()>=2 )
					{
						CArray<PT_3D,PT_3D> *p = new CArray<PT_3D,PT_3D>;
						if( p!=NULL )
						{
							//提取基线中的点
							CArray<PT_3DEX,PT_3DEX> pts;
							CTrimCommand::GetPolyPts(pFtr->GetGeometry(), pts);		
							if(pts.GetSize()<=0)
							{
								delete p;
								continue;
							}
							int polysnum = pts.GetSize();							
							PT_3D *polys = new PT_3D[polysnum];
							
							if( polys!=NULL )
							{							
								for( int j=0; j<polysnum; j++)
								{
									COPY_3DPT((polys[j]),(pts[j]));
								}

								polysnum = GraphAPI::GKickoffSamePoints(polys,polysnum);

								p->SetSize(polysnum);
								memcpy(p->GetData(),polys,sizeof(PT_3D)*polysnum);

								arrPolys.Add(p);
							}
							else
								delete p;						
							
							if( polys )delete[] polys;
						}
					}
				}
			}
		}

		CUndoFtrs undoftr(m_pEditor,Name());
		//执行裁剪
		for ( i=0; i<nLayer; i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			if (!pLayer)  continue;
			
			int nobj = pLayer->GetObjectCount();
			for (int j=0;j<nobj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if( !pFtr)continue;

				//增长进度条
				GProgressStep();

				//跳过参考层
				if( CheckObjForLayerCode(pDS, pFtr, m_strRefLay) )
					continue;

				//跳过结果层
				if( !m_strRetLay.IsEmpty() &&
					CheckObjForLayerCode(pDS, pFtr, m_strRetLay) )
					continue;

				//只处理裁剪层
				if( !m_strTrimLay.IsEmpty() &&
					!CheckObjForLayerCode(pDS, pFtr, m_strTrimLay) )
					continue;

				CFtrLayer *pLayer0 = GetLayer(strRetLayName);				
				CPtrArray arrInside, arrOutside;

				CFeature *pFtr1 = NULL;					
				if( pLayer0 )
					pFtr1 = pLayer0->CreateDefaultFeature(m_pEditor->GetDataSource()->GetScale(),pFtr->GetGeometry()->GetClassType());
				else
					pFtr1 = pLayer->CreateDefaultFeature(m_pEditor->GetDataSource()->GetScale(),pFtr->GetGeometry()->GetClassType());
				
				if( pFtr1==NULL )continue;

				// 复制固定属性
				CValueTable tab;
				tab.BeginAddValueItem();
				pFtr->WriteTo(tab);
				tab.EndAddValueItem();
				tab.DelField(FIELDNAME_SHAPE);
				tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
				tab.DelField(FIELDNAME_FTRID);
				tab.DelField(FIELDNAME_GEOCLASS);

				if( !pLayer0 || !pLayer0->IsInherent() )
				{
					pFtr1->ReadFrom(tab);
				}
				
				CArray<PT_3DEX,PT_3DEX> pts;
				pFtr->GetGeometry()->GetShape(pts);
				pFtr1->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());

				//执行裁剪
				if( TrimObj(pFtr1->GetGeometry(), &arrPolys, &arrInside, &arrOutside) )
				{						
					int k, nSize;
					CGeometry *pObj;

					//添加裁剪区域外的地物
					for( k=0, nSize=arrOutside.GetSize(); k<nSize; k++ )
					{
						pObj = (CGeometry*)arrOutside.GetAt(k);
						CFeature *pFtr0 = pFtr->Clone();
						pFtr0->SetGeometry(pObj);
						pFtr0->ReadFrom(tab);
						pFtr0->SetID(OUID());
						pFtr0->SetToDeleted(FALSE);
						m_pEditor->AddObject(pFtr0,pLayer->GetID());//要改
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pFtr0);
						undoftr.AddNewFeature(FtrToHandle(pFtr0));
					}
					
					//添加裁剪区域内的地物
					//如果目标图层不存在，就删除区域内的地物
					if( m_strRetLay.IsEmpty() )
					{
						for( k=0, nSize=arrInside.GetSize(); k<nSize; k++ )
						{
							delete (CGeometry*)arrInside.GetAt(k);
						}
					}
					else
					{
						for( k=0, nSize=arrInside.GetSize(); k<nSize; k++ )
						{
							pObj = (CGeometry*)arrInside.GetAt(k);
							CFeature *pFtr0 = pFtr1->Clone();
							pFtr0->SetGeometry(pObj);
							pFtr0->SetID(OUID());
							pFtr0->SetToDeleted(FALSE);								
							pObj->SetColor(color);

							if (pLayer0)
							{
								m_pEditor->AddObject(pFtr0,pLayer0->GetID());
								GETXDS(m_pEditor)->CopyXAttributes(pFtr,pFtr0);
								undoftr.AddNewFeature(FtrToHandle(pFtr0));
							}							
						}
					}

					//删除当前地物
					undoftr.AddOldFeature(FtrToHandle(pFtr));
					m_pEditor->DeleteObject(FtrToHandle(pFtr));
				}

				delete pFtr1;
			}		
		}
		undoftr.Commit();
		//清除边界点数据
		int nPoly = arrPolys.GetSize();
		for( i=0; i<nPoly; i++)
		{
			delete (CArray<PT_3D,PT_3D>*)arrPolys.GetAt(i);
		}
		
		GProgressEnd();
		PromptString(StrFromResID(IDS_CMDTIP_TRIMEND));
		
		Finish();
	}
}



BOOL CTrimLayerCommand::TrimObj(CGeometry *pObj, CPtrArray *pPolys, CPtrArray *pInside, CPtrArray *pOutside)
{
	int i,j;

	BOOL bInsideValid = FALSE, bOutsideValid = FALSE;

	CPtrArray input;
	input.Add(pObj);
	for( i=0; i<pPolys->GetSize(); i++)
	{
		//初始化裁剪
		CArray<PT_3D,PT_3D> *p = (CArray<PT_3D,PT_3D>*)pPolys->GetAt(i);
		PT_3D *polys = p->GetData();
		int polysnum = p->GetSize();

		Envelope evlp;
		evlp.CreateFromPts(polys,polysnum);
		double wid = (evlp.m_xh-evlp.m_xl)/10;
		if( wid<0.1 )wid = 1;

		CGrTrim trim;
		trim.InitTrimPolygon(polys,polysnum,wid);

		//处理点或者文本
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint))||pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			PT_3D pt3d;
			PT_3DEX expt;
			expt = pObj->GetDataPoint(0);
			COPY_3DPT(pt3d,expt);
			BOOL bIn = (GraphAPI::GIsPtInRegion(pt3d,polys,polysnum)==2);
			if( bIn )
			{
				CGeometry *pNew = pObj->Clone();
				if( pNew )pInside->Add(pNew);
				bInsideValid = TRUE;
				break;
			}
		}
		//处理线
		else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		{
			CPtrArray arr, arr1, arr2;
			int nInput = input.GetSize();
			for( j=0; j<nInput; j++ )
			{
				CGeometry *pObjt = (CGeometry*)input.GetAt(j);
				
				//裁剪外部，获得区域内对象
				arr.RemoveAll();
				m_bTrimOut = TRUE;
				if( TrimCurve(pObjt,&arr,evlp,polys,polysnum,&trim) )
				{
					if (arr.GetSize() > 0)
					{
						arr1.Append(arr);
						bInsideValid = TRUE;
					}	
				}
				else
				{
					CGeometry *pNew = pObjt->Clone();
					if( pNew )arr1.Add(pNew);
					bInsideValid = TRUE;
				}

				//裁剪内部，获得区域外对象
				arr.RemoveAll();
				m_bTrimOut = FALSE;
				if( TrimCurve(pObjt,&arr,evlp,polys,polysnum,&trim) )
				{
					if (arr.GetSize() > 0)
					{
						arr2.Append(arr);
						bOutsideValid = TRUE;
					}										
				}
				else
				{
					CGeometry *pNew = pObjt->Clone();
					if( pNew )arr2.Add(pNew);
				}
			}

			//清理 input
			if( nInput==1 && input.GetAt(0)==pObj )
			{
			}
			else
			{
				for( j=0; j<nInput; j++ )
				{
					CGeometry *pObjt = (CGeometry*)input.GetAt(j);
					delete pObjt;
				}
			}
			input.RemoveAll();

			//重新填入 input
			pInside->Append(arr1);
			input.Append(arr2);
		}
		//处理面
		else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		{
			CPtrArray arr, arr1, arr2;
			int nInput = input.GetSize();
			for( j=0; j<nInput; j++ )
			{
				CGeometry *pObjt = (CGeometry*)input.GetAt(j);
				
				//裁剪外部，获得区域内对象
				arr.RemoveAll();
				m_bTrimOut = TRUE;
				if( TrimSurface(pObjt,&arr,evlp,polys,polysnum,&trim) )
				{
					if (arr.GetSize() > 0)
					{
						arr1.Append(arr);
						bInsideValid = TRUE;
					}					
				}
				else
				{
					CGeometry *pNew = pObjt->Clone();
					if( pNew )arr1.Add(pNew);
				}
				
				//裁剪内部，获得区域外对象
				arr.RemoveAll();
				m_bTrimOut = FALSE;
				if( TrimSurface(pObjt,&arr,evlp,polys,polysnum,&trim) )
				{
					if (arr.GetSize() > 0)
					{
						arr2.Append(arr);
						bOutsideValid = TRUE;
					}										
				}
				else
				{
					CGeometry *pNew = pObjt->Clone();
					if( pNew )arr2.Add(pNew);
				}
			}
			
			//清理 input
			if( nInput==1 && input.GetAt(0)==pObj )
			{
			}
			else
			{
				for( j=0; j<nInput; j++ )
				{
					CGeometry *pObjt = (CGeometry*)input.GetAt(j);
					delete pObjt;
				}
			}
			input.RemoveAll();
			
			//重新填入 input
			pInside->Append(arr1);
			input.Append(arr2);
		}
	}

	if( pInside->GetSize()<=0 && (input.GetSize()==1 && input.GetAt(0)==pObj) )
		return FALSE;

	if( (input.GetSize()==1 && input.GetAt(0)==pObj) );
	else pOutside->Append(input);

	if( !bInsideValid )
	{
		for( i=0; i<pInside->GetSize(); i++)
		{
			CGeometry *pObjt = (CGeometry*)pInside->GetAt(i);
			if( pObjt!=pObj )delete pObjt;
		}
		pInside->RemoveAll();
	}
	if( !bOutsideValid )
	{
		for( i=0; i<pOutside->GetSize(); i++)
		{
			CGeometry *pObjt = (CGeometry*)pOutside->GetAt(i);
			if( pObjt!=pObj )delete pObjt;
		}
		pOutside->RemoveAll();
	}


	return (bInsideValid||bOutsideValid);
}


BOOL CTrimLayerCommand::TrimCurve(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim)
{
	if (!pObj) return FALSE;
	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )return FALSE;

	if (pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
	{		
		CGeometry *pCur1 = NULL, *pCur2 = NULL;
		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
		{
			((CGeoParallel*)pObj)->Separate(pCur1,pCur2);
		}
		else if (pObj->IsKindOf(RUNTIME_CLASS(CGeoDCurve)))
		{
			((CGeoDCurve*)pObj)->Separate(pCur1,pCur2);
		}
		
		BOOL bRet1=FALSE, bRet2=FALSE;
		if (pCur1)
		{
			bRet1 = TrimCurve(pCur1,pArray,evlp,polys,polysnum,ptrim);
			delete pCur1;
		}
		
		if (pCur2)
		{
			bRet2 = TrimCurve(pCur2,pArray,evlp,polys,polysnum,ptrim);
			delete pCur2;
		}
		
		return (bRet1||bRet2);
	}

	CGeoCurveBase *pCurve = (CGeoCurveBase*)pObj;
	
	CArray<PT_3DEX,PT_3DEX> arrKeyPts;
	pCurve->GetShape(arrKeyPts);
	if (arrKeyPts.GetSize()<=0)
	{
		return FALSE;
	}
	if( arrKeyPts.GetSize()==1 )
	{
		PT_3DEX expt;
		expt = arrKeyPts.GetAt(0);
		
		PT_3D pt3d;
		COPY_3DPT(pt3d,expt);
		return ( (GraphAPI::GIsPtInRegion(pt3d,polys,polysnum)==2)!=m_bTrimOut);
	}
	
	const CShapeLine *pSL = pObj->GetShape();
	/*const GrUnit *pUnit = NULL;*/
	PT_3DEX  *pts = NULL;
	
	if( (evlp.m_xh-evlp.m_xl)<1e-10 || (evlp.m_yh-evlp.m_yl)<1e-10 )
		return FALSE;
	
	//完全不相交
	if( !evlp.bIntersect(&pSL->GetEnvelope()) )
	{
		return m_bTrimOut;
	}
	
	PT_3D trimLine[2];
	CArray<PT_3D,PT_3D> arrTrimPts;
	//记录所有裁剪点
	CPtrArray arr;
	if(!pSL->GetShapeLineUnit(arr)) return FALSE;
	for( int i=0;i<arr.GetSize();i++ )
	{
		CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[i];
		if( evlp.bIntersect(&(pUnit->evlp))) 
		{
			pts = pUnit->pts;
			for( int k=0; k<pUnit->nuse-1; k++,pts++)
			{
				if( !evlp.bIntersect(&pts[0],&pts[1]) )continue;
				
				ptrim->TrimLine(&pts[0],&pts[1]);
				int trimnum = ptrim->GetTrimedLinesNum();
				if( trimnum<=0 )continue;
				
				//记录裁剪点
				for( int i=0; i<trimnum; i++)
				{
					int type = ptrim->GetTrimedLine(i,trimLine,trimLine+1);
					
					if( (type&TL_MID1)!=0 || (type&TL_ENDON1)!=0 )arrTrimPts.Add(trimLine[0]);
					if( (type&TL_MID2)!=0 || (type&TL_ENDON2)!=0 )arrTrimPts.Add(trimLine[1]);
				}
			}
		}		
	}
	

	//圆弧的裁剪不稳定，只作全在内或者全在外的判断
	BOOL bHasArc = FALSE, bLinearized = FALSE;
	for( i=0; i<arrKeyPts.GetSize(); i++)
	{		 
		if( arrKeyPts[i].pencode==penArc )
		{
			bHasArc = TRUE;
			break;
		}
	}

	if( bHasArc )
	{
		if( arrTrimPts.GetSize()==0 && evlp.bEnvelopeIn(&pSL->GetEnvelope()) )
			return !m_bTrimOut;
		else
		{
			pObj = pCurve->Linearize();
			if( !pObj )return FALSE;
			bLinearized = TRUE;
			pCurve = (CGeoCurveBase*)pObj;
		}
	}
	
	//根据裁剪点生成裁剪对象
	if( arrTrimPts.GetSize()>0 )
	{
		//准备
		CGeometry *trimObj[3];
		BOOL bFirstObj = TRUE;
		
		int trimnum = arrTrimPts.GetSize();
		for( int i=0; i<trimnum; i+=2 )
		{
			if( i==trimnum-1 )
				pCurve->GetBreak(arrTrimPts[i],arrTrimPts[i],trimObj[0],
				trimObj[1],trimObj[2],FALSE);
			else
				pCurve->GetBreak(arrTrimPts[i],arrTrimPts[i+1],trimObj[0],
				trimObj[1],trimObj[2],FALSE);
			
			//依次判断各个子对象是否保留
			
			if( trimObj[0] )
			{
				if( !m_bNotChangeZ )
					SnapZ(trimObj[0],trimObj[0]->GetDataPointSum()-1,polys,polysnum);
				BOOL bObjIn = IsObjInRegion(trimObj[0],polys,polysnum);
				if (bObjIn == m_bTrimOut || bObjIn == -1)
					pArray->Add(trimObj[0]);
				else
					delete trimObj[0];
			}
			
			if( trimObj[1] )
			{
				if( !m_bNotChangeZ )
					SnapZ(trimObj[1],-1,polys,polysnum);
				BOOL bObjIn = IsObjInRegion(trimObj[1],polys,polysnum);
				if (bObjIn == m_bTrimOut || bObjIn == -1)
					pArray->Add(trimObj[1]);
				else
					delete trimObj[1];
			}
			
			if( !bFirstObj && pCurve )delete pCurve;
			bFirstObj = 0;
			pCurve = NULL;
			
			if( trimObj[2] )
			{
				//以尾对象为新目标，继续裁剪
				pCurve = (CGeoCurveBase*)trimObj[2];

				if( (!m_bNotChangeZ) && pCurve!=NULL )
					SnapZ(trimObj[2],0,polys,polysnum);
			}
			else break;
		}
		
		if( !bFirstObj && pCurve )
		{
			BOOL bObjIn = IsObjInRegion(pCurve,polys,polysnum);
			if (bObjIn == m_bTrimOut || bObjIn == -1)
				pArray->Add(pCurve);
			else
				delete pCurve;
		}
		if( bLinearized )
			delete pObj;
		return TRUE;
	}
	else//全部在边界外或者全部在边界内
	{
		BOOL bHeadIn = IsObjInRegion(pObj,polys,polysnum);
		
		if( bLinearized )
			delete pObj;

		return (bHeadIn != m_bTrimOut && bHeadIn != -1);
	}
}


void CTrimLayerCommand::SnapZ(CGeometry *pObj, int idx, PT_3D *pts, int nPt)
{
	if( pObj->GetDataPointSum()<=0 )
		return;

	if( idx<=0 )
	{
		PT_3DEX expt;
		PT_3D pt1,pt2;
		expt = pObj->GetDataPoint(0);
		double dis = 0, mindis = -1;
		for( int i=0; i<nPt; i++)
		{
			int i1 = (i+1)%nPt;
			dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
				pts[i1].x,pts[i1].y,pts[i1].z,expt.x,expt.y,expt.z,
				&pt1.x,&pt1.y,&pt1.z,false,true);
			if( mindis<0 ||mindis>dis )
			{
				mindis = dis;
				pt2 = pt1;
			}
		}
		if( mindis>=0 )
		{
			expt.z = pt2.z;
			pObj->SetDataPoint(0,expt);
		}
	}
	if( idx>0 || idx<0 )
	{
		PT_3DEX expt;
		PT_3D pt1,pt2;
		int idx2 = (idx>0?idx:(pObj->GetDataPointSum()-1));
		expt = pObj->GetDataPoint(idx2);
		double dis = 0, mindis = -1;
		for( int i=0; i<nPt; i++)
		{
			int i1 = (i+1)%nPt;
			dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
				pts[i1].x,pts[i1].y,pts[i1].z,expt.x,expt.y,expt.z,
				&pt1.x,&pt1.y,&pt1.z,false,true);
			if( mindis<0 ||mindis>dis )
			{
				mindis = dis;
				pt2 = pt1;
			}
		}
		if( mindis>=0 )
		{
			expt.z = pt2.z;
			pObj->SetDataPoint(idx2,expt);
		}
	}
	if( idx<0 && pObj->GetDataPointSum()>2 )
	{
		double len = 0, len1 = 0;
		PT_3DEX expt1,expt2,expt3,expt4;
		CArray<PT_3DEX,PT_3DEX> arr;
		pObj->GetShape(arr);
		int nPtSum = arr.GetSize();
		
		for( int i=0; i<nPtSum-1; i++)
		{
			expt1 = arr.GetAt(i);
			expt2 = arr.GetAt(i+1);
			len += sqrt((expt1.x-expt2.x)*(expt1.x-expt2.x)+(expt1.y-expt2.y)*(expt1.y-expt2.y));
		}
		
		expt3 = arr.GetAt(0);
		expt4 = arr.GetAt(nPtSum-1);
		
		double dz = expt4.z-expt3.z;
		
		for( i=1; i<nPtSum-1; i++)
		{
			expt1 = arr.GetAt(i);
			expt2 = arr.GetAt(i-1);

			len1 += sqrt((expt1.x-expt2.x)*(expt1.x-expt2.x)+(expt1.y-expt2.y)*(expt1.y-expt2.y));
			double k = 0;
			if( len>1e-4 )
				k = len1/len;
			
			expt1.z = expt3.z + k*dz;
			arr.SetAt(i,expt1);
		}
		pObj->CreateShape(arr.GetData(),arr.GetSize());
	}
}


void CTrimLayerCommand::SnapZ2(CGeometry *pObj,CArray<int,int>& flags, PT_3D *polys, int polysnum)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	pObj->GetShape(pts);
	int nPt = pts.GetSize();
	int i0, i1, i2,  start = -1;
	
	for( i0=0; i0<nPt; i0++)
	{
		if( flags[i0]==1 )break;
	}
	if( i0>=nPt )return;
	start = i0+nPt;
	
	for( ; i0<start; )
	{
		for( i1=i0+1; i1<start; i1++)
		{
			if( flags[i1%nPt]==0 )break;
		}
		for( i2=i1+1; i2<=start; i2++)
		{
			if( flags[i2%nPt]==1 )break;
		}
		
		if( i1<start && i2<=start )
		{
			SnapZ(pObj,(i1-1)%nPt,polys,polysnum);
			SnapZ(pObj,i2%nPt,polys,polysnum);

			if( ((i1-1)%nPt)==0 || (i2%nPt)==0 )
			{
				PT_3DEX expt1,expt2;
				expt1 = pts.GetAt(0);
				expt2 = pts.GetAt(nPt-1);			
				expt2.z = expt1.z;
				pts.SetAt(nPt-1,expt2);
			}
			else if( ((i1-1)%nPt)==(nPt-1) || (i2%nPt)==(nPt-1) )
			{
				PT_3DEX expt1,expt2;
				expt1 = pts.GetAt(0);
				expt2 = pts.GetAt(nPt-1);
				expt1.z = expt2.z;
				pts.SetAt(0,expt1);
			}
		}
		i0 = i2;
	}
	pObj->CreateShape(pts.GetData(),pts.GetSize());
}


void CTrimLayerCommand::InterZ(CGeometry *pObj,CArray<int,int>& flags)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	pObj->GetShape(pts);
	int nPt = pts.GetSize();
	int i0, i1, i2, i, start = -1;

	for( i0=0; i0<nPt; i0++)
	{
		if( flags[i0]==1 )break;
	}
	if( i0>=nPt )return;
	start = i0+nPt;

	for( ; i0<start; )
	{
		for( i1=i0+1; i1<start; i1++)
		{
			if( flags[i1%nPt]==0 )break;
		}
		for( i2=i1+1; i2<=start; i2++)
		{
			if( flags[i2%nPt]==1 )break;
		}

		if( i1<start && i2<=start )
		{
			double len = 0, len1 = 0;
			PT_3DEX expt1,expt2,expt3,expt4;

			for( i=i1-1; i<i2; i++)
			{
				expt1 = pts.GetAt(i%nPt);
				expt2 = pts.GetAt((i+1)%nPt);
				len += sqrt((expt1.x-expt2.x)*(expt1.x-expt2.x)+(expt1.y-expt2.y)*(expt1.y-expt2.y));
			}

			expt3 = pts.GetAt((i1-1)%nPt);
			expt4 = pts.GetAt(i2%nPt);

			double dz = expt4.z-expt3.z;

			for( i=i1; i<i2; i++)
			{
				expt1 = pts.GetAt(i%nPt);
				expt2 = pts.GetAt((i-1)%nPt);
				len1 += sqrt((expt1.x-expt2.x)*(expt1.x-expt2.x)+(expt1.y-expt2.y)*(expt1.y-expt2.y));
				double k = 0;
				if( len>1e-4 )
					k = len1/len;
				
				expt1.z = expt3.z + k*dz;
				pts.SetAt((i%nPt),expt1);
			}
		}
		i0 = i2;
	}
	pObj->CreateShape(pts.GetData(),pts.GetSize());
}


BOOL CTrimLayerCommand::TrimSurface(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim)
{
	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )return FALSE;
	if (!((CGeoCurveBase*)pObj)->IsClosed())
	{
		return TrimCurve(pObj,pArray,evlp,polys,polysnum,ptrim);
	}

	CGeoSurface *pCurve = (CGeoSurface*)pObj;
	
	if( pCurve->GetDataPointSum()==1 )
	{
		PT_3DEX expt;
		expt = pCurve->GetDataPoint(0);
		
		PT_3D pt3d;
		COPY_3DPT(pt3d,expt);
		return ( (GraphAPI::GIsPtInRegion(pt3d,polys,polysnum)==2)!=m_bTrimOut);
	}
	
	if( (evlp.m_xh-evlp.m_xl)<1e-10 || (evlp.m_yh-evlp.m_yl)<1e-10 )
		return FALSE;
	
	const CShapeLine *pSL = pObj->GetShape();
	
	//完全不相交
	if( !evlp.bIntersect(&pSL->GetEnvelope()) )
	{
		return m_bTrimOut;
	}
	
	Envelope evlpobj = pSL->GetEnvelope();
	
	//获取基线点，并确保为顺时针
	CArray<PT_3DEX,PT_3DEX> pts;
	if(!pSL->GetPts(pts)) return FALSE;
	int npt = pts.GetSize(),i,j;
	PT_3D *pt3ds = new PT_3D[npt];
	if(!pt3ds) return FALSE;	
	//裁里就用逆时针（保留相减区域），裁外就用顺时针（保留相交区域）
	if( m_bTrimOut!=(GraphAPI::GIsClockwise(pts.GetData(),npt)?TRUE:FALSE) )
	{
		for( i=0; i<npt; i++)
		{
			COPY_3DPT(pt3ds[i] ,pts[npt-1-i]);
		}
	}
	else
	{
		for( i=0; i<npt; i++)
		{
			COPY_3DPT(pt3ds[i] ,pts[i]);
		}
	}
	
	//起点是否在多边形内或外
	BOOL bHeadOut = !(GraphAPI::GIsPtInRegion(pt3ds[0],polys,polysnum)==2);
	BOOL bObjOut = !IsObjInRegion(pCurve,polys,polysnum);
	
	BOOL bIntersectNode = TRUE;
	
	//将母线节点和相交点组成一个顺序的点列
	CArray<SurfaceTrimPt,SurfaceTrimPt> arrObjPt, arrPolyPt;
	for( i=0; i<npt; i++)
	{
		SurfaceTrimPt tpt;
		tpt.pt = pt3ds[i];
		tpt.no1 = 0; tpt.no2 = 0;
		//type为0，表示是基线点，不为0，表示是交点；
		//其中，为-1，no1是作交点对应之前的对象的基线点号，no2是作交点对应之前的范围线的基线点号
		//为1，no1是作交点对应之后的对象总点列的索引号，no2是作交点对应之后的范围线总点列的索引号
		tpt.type = 0;	
		tpt.t = 0;	//t是记录的此点在线段上的位置参数
		tpt.trace = 0;
		int i0 = arrObjPt.Add(tpt);
		if( i==(npt-1) )continue;
		
		if( !evlp.bIntersect(pt3ds+i,pt3ds+i+1) )continue;
		for( j=0; j<polysnum; j++)
		{
			int j1 = (j+1)%polysnum;		
			
			SurfaceTrimPt tpt0;
			tpt0.no1 = 0; tpt0.no2 = 0;
			tpt0.type = 0;
			tpt0.t = 0; 
			tpt0.trace = 0;
			
			if( GraphAPI::GGetLineIntersectLineSeg(pt3ds[i].x,pt3ds[i].y,
				pt3ds[i+1].x,pt3ds[i+1].y,polys[j].x,polys[j].y,
				polys[j1].x,polys[j1].y,&tpt.pt.x,&tpt.pt.y,&tpt.t,&tpt0.t) )
			{
				if (tpt.t < -1e-10 || (i<npt-2?tpt.t>=1-1e-10:tpt.t>1+1e-10) || tpt0.t < -1e-10 || (j<polysnum-2?tpt0.t>=1-1e-10:tpt0.t>1+1e-10))
				{
					continue;
				}
				
				if (tpt.t > 1e-10 && tpt.t < 1-1e-10)
				{
					bIntersectNode = FALSE;
				}
				
				GraphAPI::GGetPtZOfLine(pt3ds[i].x,pt3ds[i].y,pt3ds[i].z,pt3ds[i+1].x,pt3ds[i+1].y,pt3ds[i+1].z,&tpt.pt.x,&tpt.pt.y,&tpt.pt.z);
				tpt.type = -1;
				tpt.no1 = i; tpt.no2 = j;
				
				//保持一致的顺序
				for( int k=i0+1; k<arrObjPt.GetSize(); k++)
				{
					if( arrObjPt.GetAt(k).t>tpt.t )break;
				}
				if( k>=arrObjPt.GetSize() )arrObjPt.Add(tpt);
				else arrObjPt.InsertAt(k,tpt);
				
				//////////////////////////////////////////////////////////////////////////
				tpt0.t += j;
				tpt0.pt = tpt.pt;
				tpt0.type = -1;
				tpt0.no1 = i; tpt0.no2 = j;
				
				//保持一致的顺序
				for( k=0; k<arrPolyPt.GetSize(); k++)
				{
					if( arrPolyPt.GetAt(k).t>tpt0.t )break;
				}
				if( k>=arrPolyPt.GetSize() )arrPolyPt.Add(tpt0);
				else arrPolyPt.InsertAt(k,tpt0);
			}
		}
	}

	int objPtSize = arrObjPt.GetSize();
	for (i=objPtSize-1; i>=0; i--)
	{
		for (j=i-1; j>=0 && (arrObjPt[j].type == -1 && arrObjPt[i].type == -1) && GraphAPI::GGet2DDisOf2P(arrObjPt[j].pt,arrObjPt[i].pt) < 1e-10; j--)
		{
			arrObjPt.RemoveAt(j);
			i--;
		}
	}

	if( arrObjPt.GetSize()<=npt )
	{	
		if( pt3ds )delete[] pt3ds;
		return (bObjOut==m_bTrimOut);
	}

	// 如果相交点全部在关键点处就检测地物是否全部在范围线内或者全部在范围线外
	if (bIntersectNode)
	{
		int lastRet = -3;
		int objPtSize = arrObjPt.GetSize();
		for (i=0; i<objPtSize-1; i++)
		{
			int r = GraphAPI::GIsPtInRegion(arrObjPt[i].pt,polys,polysnum);	
			PT_3D expt;
			expt.x = (arrObjPt[i].pt.x + arrObjPt[i+1].pt.x) * 0.5;
			expt.y = (arrObjPt[i].pt.y + arrObjPt[i+1].pt.y) * 0.5;
			expt.z = (arrObjPt[i].pt.z + arrObjPt[i+1].pt.z) * 0.5;
			int r1 = GraphAPI::GIsPtInRegion(expt,polys,polysnum);	
			
			if (lastRet != -3 &&  (r == -1 && lastRet == 2) || (r == 2 && lastRet == -1) || 
				(r1 == -1 && lastRet == 2) || (r1 == 2 && lastRet == -1))
			{
				break;
			}
			
			if (lastRet == -3 && (r == -1 || r == 2))
			{
				lastRet = r;
			}
			
		}
		
		if (i >= objPtSize-1)
		{
			if( pt3ds )delete[] pt3ds;
			return bObjOut==m_bTrimOut;
		}
	}
	
	for( j=0; j<polysnum; j++)
	{
		SurfaceTrimPt tpt;
		tpt.pt = polys[j];
		tpt.no1 = 0; tpt.no2 = 0;
		tpt.type = 0;
		tpt.t = j; 
		tpt.trace = 0;
		
		for(int k=0; k<arrPolyPt.GetSize(); k++)
		{
			if( arrPolyPt.GetAt(k).t>=tpt.t )break;
		}
		if( k>=arrPolyPt.GetSize() )arrPolyPt.Add(tpt);
		else arrPolyPt.InsertAt(k,tpt);
	}
	
	for (j=0; j<arrPolyPt.GetSize(); j++)
	{
		arrPolyPt[j].t -= int(arrPolyPt[j].t);
	}
	
	int polyPtSize = arrPolyPt.GetSize();
	for (i=polyPtSize-1; i>=0; i--)
	{
		for (j=i-1; j>=0 && (arrPolyPt[j].type == -1 && arrPolyPt[i].type == -1) && GraphAPI::GGet2DDisOf2P(arrPolyPt[j].pt,arrPolyPt[i].pt) < 1e-10; j--)
		{
			arrPolyPt.RemoveAt(j);
			i--;
		}
	}

	if( arrPolyPt.GetSize()<=polysnum )
	{	
		if( pt3ds )delete[] pt3ds;
		return (bObjOut==m_bTrimOut);
	}
	//将两个点列中的相交点一一对应起来
	SurfaceTrimPt *tpts1 = arrObjPt.GetData();
	SurfaceTrimPt *tpts2 = arrPolyPt.GetData();
	int ntpt1 = arrObjPt.GetSize(), ntpt2 = arrPolyPt.GetSize();
	for( i=0; i<ntpt1; i++)
	{
		if( tpts1[i].type!=-1 )continue;
		for( j=0; j<ntpt2; j++)
		{
			if( tpts2[j].type!=-1 )continue;
			if( tpts1[i].no1==tpts2[j].no1 && tpts1[i].no2==tpts2[j].no2 )
			{
				if( fabs(tpts1[i].pt.x-tpts2[j].pt.x)<1e-10 && 
					fabs(tpts1[i].pt.y-tpts2[j].pt.y)<1e-10 &&
					fabs(tpts1[i].pt.z-tpts2[j].pt.z)<1e-10 )
				{
					tpts1[i].no1 = i; tpts1[i].no2 = j;
					tpts2[j].no1 = i; tpts2[j].no2 = j;
					tpts1[i].type = 1;
					tpts2[j].type = 1;
					break;
				}
			}
		}
		//匹配失败
		if( j>=ntpt2 )
		{
		
			if( pt3ds )delete[] pt3ds;
			return FALSE;
		}
	}
	
	//创建模板对象
	CGeoSurface *pTempl = (CGeoSurface*)pObj->Clone();
	if( !pTempl )
	{
		
		if( pt3ds )delete[] pt3ds;
		return FALSE;
	}
	
	//跳过起点开始的一段，达到相交点
	for( i=0; i<ntpt1; i++)
	{
		if( tpts1[i].type==1 )break;
	}
	
	//如果从起点开始的一段不会被裁剪，那么下一段就会被裁剪，
	//为了保证从i0开始的地方总会被保留，我们就再走一段
	if( bHeadOut!=m_bTrimOut )
	{
		for( i++; i<ntpt1; i++)
		{
			if( tpts1[i].type==1 )break;
		}
	}
	
	int i0 = i%ntpt1;
	PT_3DEX expt;
	expt.pencode = penLine;
	
	do
	{
		CGeoSurface *pNew = (CGeoSurface*)pTempl->Clone();
		if( !pNew )break;

		//记录新对象中各个节点是来自原对象，还是来自范围线
		//如果来自原对象，对应标记设置为1，否则设置为0
		CArray<int,int> arrFromFlags;
		CArray<PT_3DEX,PT_3DEX> arr;
		//添加相交点，这是新对象的起点
		COPY_3DPT(expt,tpts1[i].pt);
		arr.Add(expt);
		tpts1[i].trace = 1;
		arrFromFlags.Add(1);

		int start = i, stop, bloop = 0;
LOOP:
		i = (i+1)%ntpt1;
		
		//跟踪对象上的节点，直到某个交点
		for( ; i!=i0; i = (i+1)%ntpt1 )
		{
			COPY_3DPT(expt,tpts1[i].pt);
			arr.Add(expt);
			arrFromFlags.Add(1);

			tpts1[i].trace = 1;
			if( tpts1[i].type==1 )break;
		}
		if( !bloop )stop = i;
		//跳跃到范围线上的对应的那个交点
		j = tpts1[i].no2;
		
		//跟踪范围线上的节点，直到某个交点，并包括该相交点
		j = (j+1)%ntpt2;
		for(;;)
		{
			COPY_3DPT(expt,tpts2[j].pt);
			arr.Add(expt);
			arrFromFlags.Add(0);
			if( tpts2[j].type==1 )
			{
				tpts1[tpts2[j].no1].trace = 1;
				break;
			}
			j = (j+1)%ntpt2;			
		}
		
		//如果不是跟踪的起点，说明当前新对象的边界线还没有跟踪完毕，需要继续
		if( tpts2[j].no1!=tpts1[start].no1 || tpts2[j].no2!=tpts1[start].no2 )
		{
			//跳跃到新的起点
			i = tpts2[j].no1;
			//循环
			bloop = 1;
			goto LOOP;
		}
		
		//恢复到没有跳跃前的位置
		if( bloop )i = stop;
		
		//对象的下一个相交段是被裁剪部分，跳过
		i = (i+1)%ntpt1;
		for( ; i!=i0; i = (i+1)%ntpt1 )
		{
			//跳过跟踪过的点
			if( tpts1[i].trace==1 )continue;
			if( tpts1[i].type==1 )break;
		}
		
		//去除相同的点
		{
			int ptsum = arr.GetSize();
		
			
			PT_3DEX *expts = arr.GetData();
			int pos = 0;
			for(int k=1; k<ptsum; k++)
			{
				if( fabs(expts[pos].x-expts[k].x)<1e-10 && 
					fabs(expts[pos].y-expts[k].y)<1e-10 )
				{
					arrFromFlags.SetAt(k,-1);
					continue;
				}
				
				if( k!=(pos+1) )expts[pos+1] = expts[k];
				pos++;
			}
			if( ptsum!=(pos+1) )
			{
				for( k=ptsum-1; k>=0; k--)
				{
					if( k>=(pos+1) )arr.RemoveAt(k);
					else arr.SetAt(k,expts[k]);
				}
			}
		}
		pNew->CreateShape(arr.GetData(),arr.GetSize());
		//根据需要，用来自原对象的节点z值内插来自范围线上的节点的z值
		for( int k=arrFromFlags.GetSize()-1; k>=0; k--)
		{
			if( arrFromFlags[k]<0 )arrFromFlags.RemoveAt(k);
		}
		if( m_bNotChangeZ )InterZ(pNew,arrFromFlags);
		else SnapZ2(pNew,arrFromFlags,polys,polysnum);
		
		arrFromFlags.RemoveAll();
		
		
		if( arr.GetSize()>=4 )
		{
			pArray->Add(pNew);
		}
		else
		{
			delete pNew;
		}
		
	}while(i!=i0);	

	if( pt3ds )delete[] pt3ds;
	delete pTempl;
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CInterpolateVertexsCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CInterpolateVertexsCommand,CEditCommand)

CInterpolateVertexsCommand::CInterpolateVertexsCommand()
{
	m_lfMaxLen = 10;
	strcat(m_strRegPath,"\\InterpolateVertexs");
}

CInterpolateVertexsCommand::~CInterpolateVertexsCommand()
{
}


CString CInterpolateVertexsCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_INTERPOLATEVERTEXS);
}

void CInterpolateVertexsCommand::Start()
{
	if( !m_pEditor )return;

	m_nStep = 0;

	CCommand::Start();

	if( m_pEditor )
	{
		m_pEditor->OpenSelector();
//		m_pDoc->UpdateAllViews(NULL,hc_Attach_Accubox);
	}
}


CGeometry* CInterpolateVertexsCommand::Process(CGeometry *pObj)
{	
	if( pObj && (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
	{
		int num = pObj->GetDataPointSum();
		if( num<=1 )return NULL;

		const CShapeLine  *pBuf = pObj->GetShape();
		CArray<int,int> KeyPos;
		pBuf->GetKeyPosOfBaseLines(KeyPos);		
		
		//准备
		CArray<PT_3DEX,PT_3DEX> pts;
		pBuf->GetPts(pts);
		if( pts.GetSize()<2 || KeyPos.GetSize()<=0 )return NULL;
		
// 		nodes = new GrNode[num];
// 		if( !nodes )
// 		{
// 			return NULL;
// 		}
// 
// 		pBuf->GetPts(nodes);

		CGeometry *pNewObj = pObj->Clone();
		
		//取点
//		pBuf->GetPts(nodes);
		
		PT_3DEX expt1, expt2, expt3;
		int nInsert = 0;
		CArray<PT_3DEX,PT_3DEX> pts0,newpts;
		pObj->GetShape(pts0);
		newpts.Copy(pts0);
		int nsz = pts0.GetSize();
		for( int j=0; j<nsz-1; j++,nInsert++)
		{
			expt1 = pts0.GetAt(j);
			expt2 = pts0.GetAt(j+1);			
			if( expt2.pencode==penLine || expt2.pencode==penStream )
			{
				expt3.pencode = expt2.pencode;
				double dis = sqrt((expt1.x-expt2.x)*(expt1.x-expt2.x)+(expt1.y-expt2.y)*(expt1.y-expt2.y));

				for( double d = m_lfMaxLen; d<dis; d += m_lfMaxLen )
				{
					if( d+m_lfMaxLen<dis )
					{
						expt3.x = expt1.x + (expt2.x-expt1.x)*d/dis;
						expt3.y = expt1.y + (expt2.y-expt1.y)*d/dis;
						expt3.z = expt1.z + (expt2.z-expt1.z)*d/dis;
						newpts.InsertAt(nInsert+1,expt3);
						nInsert++;
					}
					else
					{
						expt3.x = expt1.x + (expt2.x-expt1.x)*((d-m_lfMaxLen+dis)*0.5)/dis;
						expt3.y = expt1.y + (expt2.y-expt1.y)*((d-m_lfMaxLen+dis)*0.5)/dis;
						expt3.z = expt1.z + (expt2.z-expt1.z)*((d-m_lfMaxLen+dis)*0.5)/dis;
						newpts.InsertAt(nInsert+1,expt3);
						nInsert++;
					}
				}
			}
			else if( expt2.pencode==penSpline || expt2.pencode==penArc )
			{
				expt3.pencode = expt2.pencode;
				
				int nStart = KeyPos[j], nEnd = KeyPos[j+1];
				double len = 0, curlen = 0, dis;
				CArray<double,double> arrDis;
				for( int k=nStart; k<nEnd; k++)
				{
					dis = sqrt((pts[k].x-pts[k+1].x)*(pts[k].x-pts[k+1].x)+
						(pts[k].y-pts[k+1].y)*(pts[k].y-pts[k+1].y));

					arrDis.Add(dis);
					len += dis;
				}
				int nLastPos = nStart;
				for( k=nStart; k<nEnd; k++)
				{
					dis = arrDis[k-nStart];					
					curlen += dis;
					if( curlen>m_lfMaxLen )
					{
						if( (len-curlen)>m_lfMaxLen )
						{
							expt3.x = pts[k+1].x;
							expt3.y = pts[k+1].y;
							expt3.z = pts[k+1].z;
							newpts.InsertAt(nInsert+1,expt3);
							nInsert++;

							len = len-curlen;
							curlen = 0;
							nLastPos = k+1;
						}
						else
						{
							double len2 = 0;
							for( int m=nLastPos; m<nEnd; m++)
							{
								dis = arrDis[m-nStart];
								len2 += dis;

								//大约中间的位置
								if( len2>len*0.4 && len2<len*0.6 )
								{
									expt3.x = pts[m+1].x;
									expt3.y = pts[m+1].y;
									expt3.z = pts[m+1].z;
									newpts.InsertAt(nInsert+1,expt3);
									nInsert++;
									break;
								}
							}
						}
					}
				}
			}
		}
		
		//添加结果
		if( newpts.GetSize()==pObj->GetDataPointSum() )
		{
			delete pNewObj;
		}
		else
		{
			if(pNewObj->CreateShape(newpts.GetData(),newpts.GetSize()))
				return pNewObj;
		}

	}

	return NULL;

}


void CInterpolateVertexsCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_lfMaxLen>0 )
	{
		if( m_nStep==0 )
		{
			if( !CanGetSelObjs(flag) )return;
			EditStepOne();
			int num;
			const FTR_HANDLE* handles =  m_pEditor->GetSelection()->GetSelectedObjs(num);
			m_FtrHandles.RemoveAll();
			m_FtrHandles.SetSize(num);
			memcpy(m_FtrHandles.GetData(),handles,num*sizeof(FTR_HANDLE));
		}
		
		if( m_nStep==1 )
		{
			CFeature *pFtr;
			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();			
			CUndoFtrs unftr(m_pEditor,Name());
			for( int i=m_FtrHandles.GetSize()-1; i>=0; i--)
			{
				pFtr = HandleToFtr(m_FtrHandles[i]);
				
				if( pFtr && (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))) )
				{
					CGeometry *pNewObj = Process(pFtr->GetGeometry());					
					CFeature *pNewFtr = pFtr->Clone();					
					pNewFtr->SetID(OUID());
					//添加结果
					if( pNewObj&& pNewFtr)
					{
						pNewFtr->SetGeometry(pNewObj);
						m_pEditor->AddObject(pNewFtr,m_pEditor->GetFtrLayerIDOfFtr(m_FtrHandles[i]));
						unftr.AddNewFeature(FtrToHandle(pNewFtr));
						PDOC(m_pEditor)->GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(HandleToFtr(m_FtrHandles[i]),pNewFtr);
						m_pEditor->DeleteObject(m_FtrHandles[i]);
						unftr.AddOldFeature(m_FtrHandles[i]);
					}
				}
			}
			unftr.Commit();
			if( m_pEditor )m_pEditor->RefreshView();
			Finish();
			m_nStep = 2;
		}
	}
	
	CEditCommand::PtClick(pt,flag);
}

void CInterpolateVertexsCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(_T("InterpolateVertexs"),StrFromResID(IDS_CMDNAME_INTERPOLATEVERTEXS));
	param->AddParam(PF_INERPOLATEVERTEXS,m_lfMaxLen,StrFromResID(IDS_CMDPLANE_MAXLINESEG));
	
}

void CInterpolateVertexsCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_INERPOLATEVERTEXS,var) )
	{
		m_lfMaxLen = (double)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}	
	CEditCommand::SetParams(tab,bInit);
}

void CInterpolateVertexsCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;	
	var = (double)m_lfMaxLen;
	tab.AddValue(PF_INERPOLATEVERTEXS,&CVariantEx(var));

}

//////////////////////////////////////////////////////////////////////
// CMoveClosePtCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMoveClosePtCommand,CEditCommand)

CMoveClosePtCommand::CMoveClosePtCommand()
{
	strcat(m_strRegPath,"\\MoveClosePt");
	m_nMode = modeSinglePoint;
}

CMoveClosePtCommand::~CMoveClosePtCommand()
{
	
}

CString CMoveClosePtCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MOVECLOSEPT);
}

void CMoveClosePtCommand::Start()
{
	if( !m_pEditor )return;
	
	CEditCommand::Start();
	
	m_pEditor->CloseSelector();
	
}

void CMoveClosePtCommand::Abort()
{
	CEditCommand::Abort();
	
	m_pEditor->RefreshView();
}


void CMoveClosePtCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("MoveClosePtCommand", Name());
	param->BeginOptionParam("OperMode", StrFromResID(IDS_CMDPLANE_BY));
	param->AddOption(StrFromResID(IDS_CMDPLANE_SINGLEPT), 0, ' ', m_nMode == modeSinglePoint);
	param->AddOption(StrFromResID(IDS_CMDPLANE_LINESECTION), 1, ' ', m_nMode == modeLinesection);
	param->EndOptionParam();
}

void CMoveClosePtCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "OperMode", var))
	{
		m_nMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
}

void CMoveClosePtCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);

	_variant_t var;
	var = (long)(m_nMode);
	tab.AddValue("OperMode", &CVariantEx(var));
}

int CMoveClosePtCommand::MyGetFirstPointOfIntersect(PT_3DEX *pts,int num,PT_3D pt1,PT_3D pt2,PT_3D *ret)
{
	PT_3DEX pt11,pt22;
	COPY_3DPT(pt11,pt1);
	COPY_3DPT(pt22,pt2);
	int i, index = -1;
	double mint=1,t,st0/*,st1*/;
	PT_3D tem,ret1;
	for (i=0;i<num-1;i++)
	{
		if(GLineIntersectLineSeg1(pt1.x,pt1.y,pt2.x,pt2.y,pts[i].x,pts[i].y,pts[i].z,pts[i+1].x,pts[i+1].y,pts[i+1].z,&(tem.x),&(tem.y),&(tem.z),&t,&st0))
		{
			if (t<mint)
			{
				if (st0<0.5)
				{
					if(sqrt((pts[i].x-tem.x)*(pts[i].x-tem.x)+(pts[i].y-tem.y)*(pts[i].y-tem.y))<1e-10)
					{
						if (i!=0 && (GraphAPI::GGetMultiply(pt11,pt22,pts[i-1])*GraphAPI::GGetMultiply(pt11,pt22,pts[i+1])<0))
						{
							ret1=pts[i];
							mint=t;
							index = i;
							continue;
						}
						else
							continue;
					}
				}
				else
				{
					if(sqrt((pts[i+1].x-tem.x)*(pts[i+1].x-tem.x)+(pts[i+1].y-tem.y)*(pts[i+1].y-tem.y))<1e-10)
					{
						if (i+2!=num && (GraphAPI::GGetMultiply(pt11,pt22,pts[i])*GraphAPI::GGetMultiply(pt11,pt22,pts[i+2])<0))
						{						
							ret1=pts[i+1];
							mint=t;
							index = i+1;
							continue;
						}
						else
							continue;
					}					
				}
				mint=t;
				ret1=tem;
				index = i;
			}			
		}
	}
	if (mint==1)
	{
		return -1;
	}
	else
	{
		*ret=ret1;
		return index;		
	}	
	return 1;
}


int CMoveClosePtCommand::MyFindKeyPosOfObj(PT_3D pt, CGeometry *pObj)
{
	//计算插入点的位置
	const CShapeLine *pSh = pObj->GetShape();
	int pos = pSh->FindNearestKeyPt(pt);
	return pos;
// 	const int *pKeyPos = pObj->GetKeyPosOfBaseLines();
// 	
// 	if( !pKeyPos )
// 	{
// 		return -1;
// 	}
// 	
// 	int num = pObj->GetPtSum();
// 	for( int i=0; i<num; i++)
// 	{
// 		if( pos<pKeyPos[i] )break;
// 	}
// 
// 	if( i>=num )
// 		return -1;
// 
// 	PT_3DEX expt1, expt2;
// 	for( int j=i-1; j<=num+i; j++)
// 	{
// 		pObj->GetPt((j%num),&expt1);
// 		pObj->GetPt((j+1)%num,&expt2);
// 
// 		if( _FABS(expt1.x-expt2.x)>1e-4 || _FABS(expt1.y-expt2.y)>1e-4 )
// 		{
// 			break;
// 		}
// 	}
// 
// 	return j;

}


void CMoveClosePtCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{
		m_ptDragStart = pt;
		if (m_nMode == modeLinesection)
		{
			m_nStep = 1;
		}
		else if (m_nMode == modeSinglePoint)
		{
			m_nStep = 2;
		}
		GotoState(PROCSTATE_PROCESSING);
	}
	else if (m_nStep == 1)
	{
		m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
		m_ptDragEnd = pt;
		m_nStep = 2;
	}

	if (m_nStep==2)
	{
		Envelope el;

		if (m_nMode == modeLinesection)
		{
			PT_3D Pt[2];
			Pt[0] = m_ptDragStart;
			Pt[1] = m_ptDragEnd;
			el.CreateFromPts(Pt, 2);
		}
		else if (m_nMode == modeSinglePoint)
		{
			el.CreateFromPtAndRadius(m_ptDragStart, GraphAPI::g_lfDisTolerance);
		}

		el.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS, 1);
		m_pEditor->GetDataQuery()->FindObjectInRect(el, m_pEditor->GetCoordWnd().m_pSearchCS);
		int num1;
		const CPFeature *ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
		CUndoFtrs unftr(m_pEditor, Name());
		CGeometry *pObj;
		CArray<PT_3DEX, PT_3DEX> arrPts0;
		for (int i = 0; i < num1; i++)
		{
			int PtNums;
			pObj = (CGeoCurve*)(ftr[i]->GetGeometry());
			if (!pObj)continue;
			if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))continue;
			if ((PtNums = pObj->GetDataPointSum()) < 2)continue;
			if (!((CGeoCurveBase*)pObj)->IsClosed())
				continue;
			{
				const CShapeLine  *pBase = pObj->GetShape();
				pObj->GetShape(arrPts0);
				if (!pBase)continue;
				CArray<PT_3DEX, PT_3DEX> pts;
				pBase->GetPts(pts);

				PT_3D ret;

				int nPt = -1;
				if (m_nMode == modeLinesection)
				{
					int nBasePt = MyGetFirstPointOfIntersect(pts.GetData(), pts.GetSize(), m_ptDragStart, m_ptDragEnd, &ret);
					if (nBasePt >= 0)
					{
						nPt = MyFindKeyPosOfObj(pts[nBasePt], pObj);
					}
				}
				else
				{
					nPt = MyFindKeyPosOfObj(m_ptDragStart, pObj);
					ret = m_ptDragStart;
				}

				if (nPt >= 0)
				{
					PT_3DEX expt0;
					expt0 = arrPts0.GetAt(nPt);
					CGeometry *pNew = pObj->Clone();

					PtNums = arrPts0.GetSize();

					if (pNew != NULL)
					{
						CArray<PT_3DEX, PT_3DEX> arrPts;
						int k;

						if (_FABS(ret.x - expt0.x) + _FABS(ret.y - expt0.y) > 1e-6)
						{
							COPY_3DPT(expt0, ret);
							arrPts.Add(expt0);
							for (k = nPt + 1; k <= (PtNums + nPt); k++)
							{
								if (k == (PtNums - 1))continue;
								expt0 = arrPts0.GetAt(k%PtNums);
								arrPts.Add(expt0);
							}

							expt0 = arrPts0.GetAt(nPt);
							COPY_3DPT(expt0, ret);
							arrPts.Add(expt0);
						}
						else
						{
							for (k = nPt; k <= (PtNums + nPt); k++)
							{
								if (k == (PtNums - 1))continue;
								expt0 = arrPts0.GetAt(k%PtNums);
								arrPts.Add(expt0);
							}
						}

						pNew->CreateShape(arrPts.GetData(), arrPts.GetSize());
						CFeature *pFtr = ftr[i]->Clone();
						if (!pFtr)continue;
						pFtr->SetGeometry(pNew);
						pFtr->SetID(OUID());
						m_pEditor->AddObject(pFtr, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(ftr[i])));
						PDOC(m_pEditor)->GetDlgDataSource()->GetXAttributesSource()->CopyXAttributes(ftr[i], pFtr);
						unftr.AddNewFeature(FtrToHandle(pFtr));
						m_pEditor->DeleteObject(FtrToHandle(ftr[i]));
						unftr.AddOldFeature(FtrToHandle(ftr[i]));

					}
				}
			}
			pObj = NULL;
		}

		unftr.Commit();
		Finish();
	}

	CEditCommand::PtClick(pt,flag);
}



void CMoveClosePtCommand::PtMove(PT_3D &pt)
{
	if(m_nStep==1)
	{
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_ptDragStart);
		buf.LineTo(&pt);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);		
		return;
	}
}




//////////////////////////////////////////////////////////////////////
// CLineFillCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CLineFillCommand,CCommand)

CLineFillCommand::CLineFillCommand()
{
	m_nFillType = 0;
	m_fFillIntv = 0.1;
}

CLineFillCommand::~CLineFillCommand()
{
}

CString CLineFillCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_LINEFILL);
}

void CLineFillCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	var = (long)(m_nFillType);
	tab.AddValue(PF_FILLTYPE,&CVariantEx(var));
	var = (double)(m_fFillIntv);
	tab.AddValue(PF_FILLINTV,&CVariantEx(var));

}

void CLineFillCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_FILLTYPE,var) )
	{
		m_nFillType = (long)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_FILLINTV,var) )
	{
		m_fFillIntv = (double)(_variant_t)*var;
	}
	SetSettingsModifyFlag();
	CEditCommand::SetParams(tab, bInit);
}

void CLineFillCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(PID_LINEFILL,StrFromLocalResID(IDS_CMDNAME_LINEFILL));
	param->BeginOptionParam(PF_FILLTYPE,StrFromResID(IDS_CMDNAME_FILLTYPE));
	param->AddOption(StrFromResID(IDS_CMDNAME_HORIZONTAL),0,' ',m_nFillType==0);
	param->AddOption(StrFromResID(IDS_CMDNAME_VERTICAL),1,' ',m_nFillType==1);
	param->AddOption(StrFromResID(IDS_CMDNAME_HORVERTICAL),2,' ',m_nFillType==2);
	param->AddOption(StrFromResID(IDS_CMDNAME_LGRADIENT),3,' ',m_nFillType==3);
	param->AddOption(StrFromResID(IDS_CMDNAME_RGRADIENT),4,' ',m_nFillType==4);
	param->AddOption(StrFromResID(IDS_CMDNAME_LRGRADIENT),5,' ',m_nFillType==5);
	param->EndOptionParam();
	param->AddParam(PF_FILLINTV,m_fFillIntv,StrFromResID(IDS_CMDNAME_FILLINTV));
}

void CLineFillCommand::Start()
{
	if( !m_pEditor )return;
	m_nFillType = 0;
	m_fFillIntv = 0.1;
	int num;
	CDlgDoc *pDoc = (CDlgDoc*)m_pEditor;
	(CDlgDoc*)m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num>0 )
	{
		m_nStep = 0;		
		CCommand::Start();
	
		PT_3D pt;
		PtClick(pt,SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;		
		return;
	}
	
	CEditCommand::Start();
}

void CLineFillCommand::Finish()
{
	CEditCommand::Finish();
}

void CLineFillCommand::UpdateParams(BOOL bSave)
{
	CEditCommand::UpdateParams(bSave);
}

void CLineFillCommand::Abort()
{
	if( m_pEditor )
	{
		m_pEditor->CloseSelector();
	}
	
	CCommand::Abort();
	m_nExitCode = CMPEC_STARTOLD;
}

void CLineFillCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( !CanGetSelObjs(flag) )return;
	   EditStepOne();

	   GotoState(PROCSTATE_PROCESSING);

	}
	
	if( m_nStep==1 )
	{
		if (m_fFillIntv < GraphAPI::GetDisTolerance())
		{
			Abort();
			return;
		}
		CGeometry *pGeo;
		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		CUndoFtrs undo(m_pEditor,Name());

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		if (!pDS)  return;
		
//		CFtrLayer *pFtrLay = pDS->GetFtrLayer();
		CFtrLayer *pFtrLay = pDS->GetCurFtrLayer();
		if (!pFtrLay)  return;
		
		for( int i = num-1; i>=0; i--)
		{
			CFeature *pFtr = HandleToFtr(handles[i]);
			if (!pFtr) continue;
			pGeo = pFtr->GetGeometry();
			
			if( pGeo && (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))  )
			{	
				CArray<PT_3DEX,PT_3DEX> arr;
				pGeo->GetShape(arr);

				int polysnum = arr.GetSize();

				if (polysnum < 3)  continue;

				CArray<PT_3D,PT_3D> polys;
				polys.SetSize(polysnum);
				for(int j=0; j<polysnum; j++)
				{
					COPY_3DPT(polys[j],arr[j]);
				}

				GrBuffer buf;
				pFtr->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
				Envelope evlp = buf.GetEnvelope();

				double wid = evlp.Width()/10;
				//if( wid<0.1 )wid = 1;
				
				CGrTrim trim;
				trim.InitTrimPolygon(polys.GetData(),polysnum,wid);
				trim.m_bIntersectHeight = TRUE;

				buf.DeleteAll();
				buf.BeginLineString(pGeo->GetColor(),0);
		//		pFtr->Draw(&buf);

				evlp.Inflate(1e-4,1e-4,0);
				// 横，竖
				if (m_nFillType == 0 || m_nFillType == 1 || m_nFillType == 2)
				{
					// 横
					if (m_nFillType == 0 || m_nFillType == 2)
					{
						PT_3D pt0(evlp.m_xl,0,0) , pt1(evlp.m_xh,0,0);
						double y = evlp.m_yl + m_fFillIntv;
						for ( ; evlp.m_yh-y>1e-4; y+=m_fFillIntv)
						{
							pt0.y = pt1.y = y;
							
							buf.MoveTo(&pt0);
							buf.LineTo(&pt1);						
							
						}

					}

					// 竖 
					if (m_nFillType == 1 || m_nFillType == 2)
					{
						PT_3D pt0(0,evlp.m_yl,0) , pt1(0,evlp.m_yh,0);
						double x = evlp.m_xl + m_fFillIntv;
						for ( ; evlp.m_xh-x>1e-4; x+=m_fFillIntv)
						{
							pt0.x = pt1.x = x;
							
							buf.MoveTo(&pt0);
							buf.LineTo(&pt1);						
							
						}

					}

					
					
				}
				// 左斜，右斜
				else if (m_nFillType == 3 || m_nFillType == 4 || m_nFillType == 5)
				{
					// 先计算要填满矩形区域第一条线和最后条线的间距，
					PT_3D pt0(evlp.m_xl,evlp.m_yl,0), pt1(evlp.m_xh,evlp.m_yl,0), pt2(evlp.m_xh,evlp.m_yh,0);
					// y向下移动的最大距离
					double fInter = GraphAPI::GGet3DDisOf2P(pt0,pt1) + GraphAPI::GGet3DDisOf2P(pt1,pt2);

					// 每次y的移动距离
					double fSideInter = m_fFillIntv*sqrt(2);
				
					// 移动次数
					int num = fInter/(m_fFillIntv*sqrt(2));

					pt0.x = evlp.m_xl;
					pt1.x = evlp.m_xh;

					// 左斜
					if (m_nFillType == 3 || m_nFillType == 5)
					{
						// 起始点y的坐标
						double y0 = evlp.m_yh - fSideInter;
						double y1 = evlp.m_yh + (evlp.Width()-fSideInter);
						
						for (int j=0; j<num; j++)
						{							
							{
								pt0.y = y0 - fSideInter*j;
								pt1.y = y1 - fSideInter*j; 
								
								buf.MoveTo(&pt0);
								buf.LineTo(&pt1);
							}
							
						}

					}					

					// 右斜
					if (m_nFillType == 4 || m_nFillType == 5)
					{
						// 起始点y的坐标
						double y0 = evlp.m_yl + fSideInter;
						double y1 = evlp.m_yl - (evlp.Width()-fSideInter);
						
						for (int j=0; j<num; j++)
						{							
							{
								pt0.y = y0 + fSideInter*j;
								pt1.y = y1 + fSideInter*j; 
								
								buf.MoveTo(&pt0);
								buf.LineTo(&pt1);
							}
							
						}
					}

				}
/*				// 右斜
				else if (m_nFillType == 4 || m_nFillType == 5)
				{
					PT_3D pt0(evlp.m_xl,evlp.m_yl,0), pt1(evlp.m_xh,evlp.m_yl,0), pt2(evlp.m_xh,evlp.m_yh,0);
					double fInter = GraphAPI::GGet3DDisOf2P(pt0,pt1) + GraphAPI::GGet3DDisOf2P(pt1,pt2);
					double minInter, maxInter;
					minInter = maxInter = m_fFillIntv;
					while (maxInter+m_fFillIntv < fInter)
					{
						maxInter += m_fFillIntv;
					}
					
					pt0.x = evlp.m_xl;
					pt1.x = evlp.m_xh;
					
					double fInter1 = minInter, fInter2 = maxInter;
					for (; fInter1<fInter,fInter2>0; fInter1+=m_fFillIntv,fInter2-=m_fFillIntv)
					{
						pt0.y = evlp.m_yl + fInter1*sqrt(2);
						pt1.y = evlp.m_yh - fInter2*sqrt(2);
						
						
						buf.MoveTo(&pt0);
						buf.LineTo(&pt1);
					}
				}
*/
				buf.End();

				GrBuffer ret;
				trim.Trim(&buf,ret);

				// 将GrBuffer转到CFeature
				const Graph *pGr = ret.HeadGraph();
				const GrVertexList *pList;
				int num = 0;
				while( pGr )
				{
					pList = NULL;
					if( IsGrLineString(pGr) )
					{
						pList = &((GrLineString*)pGr)->ptlist;
					}
					else if( IsGrPolygon(pGr) )
					{
						pList = &((GrPolygon*)pGr)->ptlist;
					}
					
					if( pList )
					{
						if (pList->nuse == 2)
						{
							CArray<PT_3DEX,PT_3DEX> arrpts;
							arrpts.SetSize(2);
							for(int j=0; j<2; j++)
							{
								COPY_3DPT(arrpts[j],pList->pts[j]);
								arrpts[j].pencode = penLine;
							}
							
							CFeature *pFtr = pFtrLay->CreateDefaultFeature(pDS->GetScale(),CLS_GEOCURVE);
							if (!pFtr) continue;
							CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
							if (!pGeo) continue;
							pGeo->CreateShape(arrpts.GetData(),2);
							pGeo->SetColor(pGeo->GetColor());
							
							m_pEditor->AddObject(pFtr);

							undo.arrNewHandles.Add(FtrToHandle(pFtr));


						}
					}
					
					pGr = pGr->next;
				}
					
			  }			
		}
		
		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	CEditCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// CColorFillCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CColorFillCommand,CCommand)

CColorFillCommand::CColorFillCommand()
{
}

CColorFillCommand::~CColorFillCommand()
{
}

CString CColorFillCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COLORFILL);
}

void CColorFillCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		// 		m_idsOld.RemoveAll();
		// 		m_idsNew.RemoveAll();
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	
	CEditCommand::Start();
}

void CColorFillCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
		
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
				
		CUndoModifyProperties undo(PDOC(m_pEditor),"ModifyProperties");
		for (int i=0;i<num;i++)
		{
			undo.arrHandles.Add(handles[i]);
			m_pEditor->DeleteObject(handles[i]);	
		}
		
		if (num>0)
		{
			for (int i=0;i<num;i++)
			{
				CFeature *pFtr = (CFeature*)handles[i];
				if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					CGeoCurveBase *pGeo = (CGeoCurveBase*)pFtr->GetGeometry();

					CValueTable tab2;
					tab2.BeginAddValueItem();
					pFtr->WriteTo(tab2);	
					tab2.EndAddValueItem();
					tab2.DelField(FIELDNAME_SHAPE);
					
					undo.oldVT.AddItemFromTab(tab2);
					
					pGeo->EnableFillColor(TRUE,COLOUR_BYLAYER);

					CString symName = pGeo->GetSymbolName();

					if (tab2.SetValue(0,FIELDNAME_SYMBOLNAME,&CVariantEx((_variant_t)(const char*)symName)))
					{
						undo.newVT.AddItemFromTab(tab2);
					}						
				}			
			}
		}		
		
		undo.Commit();

		for (i=0;i<num;i++)
		{
			m_pEditor->RestoreObject(handles[i]);	
			//m_pEditor->UpdateObject(handles[i],FALSE);	
		}
			
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		Finish();
		m_nStep = 2;
	}
	
	CCommand::PtClick(pt, flag);
}




//////////////////////////////////////////////////////////////////////
// CDelHachureCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDelHachureCommand,CEditCommand)

CDelHachureCommand::CDelHachureCommand()
{
	m_nMode = modeSingle;
}

CDelHachureCommand::~CDelHachureCommand()
{
}

CString CDelHachureCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DELHATCURVE);
}

void CDelHachureCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	
	CEditCommand::Start();
}


void CDelHachureCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	var = (long)(m_nMode);
	tab.AddValue("Mode",&CVariantEx(var));
	
}

void CDelHachureCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Mode",var) )
	{
		m_nMode = (long)(_variant_t)*var;
	}
	SetSettingsModifyFlag();
	CEditCommand::SetParams(tab, bInit);
}

void CDelHachureCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("DelHachureCommand",Name());
	param->BeginOptionParam("Mode",StrFromResID(IDS_CMDPLANE_WAY));
	param->AddOption(StrFromResID(IDS_MODE_INNERONE),0,' ',m_nMode==0);
	param->AddOption(StrFromResID(IDS_ALL),1,' ',m_nMode==1);
	param->EndOptionParam();
}

CGeometry *CDelHachureCommand::FindSurroundObj(CGeometry *pObj)
{
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	CFtrLayer* pFtrLayer;
	CPFeature pFtr;

	CGeometry *pRet = NULL, *pGeo;
	double area_min = -1;

	for (int i=0;i<pDS->GetFtrLayerCount();i++)
	{
		pFtrLayer = pDS->GetFtrLayerByIndex(i);
		if(pFtrLayer==NULL||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible())
			continue;
		
		for (int j=0;j<pFtrLayer->GetObjectCount();j++)
		{
			pFtr = pFtrLayer->GetObject(j);
			
			if( pFtr==NULL||!pFtr->IsVisible() )
				continue;
			
			pGeo = pFtr->GetGeometry();
			if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase))||pGeo->GetDataPointSum()<=2 )
				continue;
			
			if( IsSurrounded(pObj,pGeo) )
			{
				double area = ((CGeoCurveBase*)pGeo)->GetArea();
				if( pRet==NULL || area_min>area )
				{
					pRet = pGeo;
					area_min = area;
				}
			}
		}
	}
	return pRet;
}


BOOL CDelHachureCommand::IsSurrounded(CGeometry *pObj_inside, CGeometry *pObj_outside)
{
	if( !pObj_outside->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		return FALSE;

	//1、前者外包必须落在后者外包之内；
	Envelope e1 = pObj_inside->GetEnvelope();
	Envelope e2 = pObj_outside->GetEnvelope();
	if( !e2.bEnvelopeIn(&e1) )
		return FALSE;
	
	//2、前者节点必须全部落在后者母线构成的区域内部；
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2, arrPts3;
	pObj_inside->GetShape(arrPts1);
	((CGeoCurveBase*)pObj_outside)->GetShape()->GetPts(arrPts2);

	//外扩一个小距离，增强可靠性
	arrPts3.Copy(arrPts2);
	double wid = 0.001;
	if( !GraphAPI::GIsClockwise(arrPts2.GetData(),arrPts2.GetSize()) )
	{
		wid = -wid;
	}
	GraphAPI::GGetParallelLine(arrPts2.GetData(),arrPts2.GetSize(),wid,arrPts3.GetData());

	for( int i=0; i<arrPts1.GetSize(); i++)
	{
		if( GraphAPI::GIsPtInRegion(arrPts1[i],arrPts3.GetData(),arrPts3.GetSize())<0 )
			break;
	}

	if( i<arrPts1.GetSize() )
		return FALSE;

	return TRUE;
}

void CDelHachureCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		
		if (num>0)
		{
			CUndoFtrs undo(PDOC(m_pEditor),Name());

			CFeature *pFtr = HandleToFtr(handles[0]);
			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && pGeo->GetDataPointSum()==2 )
			{
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);

				double ang = GraphAPI::GGetAngle(arrPts[0].x,arrPts[0].y,arrPts[1].x,arrPts[1].y);

				CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
				CFtrLayer* pFtrLayer;

				BOOL bOK = FALSE;

				//单个模式
				if( m_nMode==modeSingle )
				{
					//找到包含它的地物
					CGeometry *pGeo2 = FindSurroundObj(pGeo);
					if( pGeo2 )
					{
						bOK = TRUE;

						for (int i=0;i<pDS->GetFtrLayerCount();i++)
						{
							pFtrLayer = pDS->GetFtrLayerByIndex(i);
							if(pFtrLayer==NULL||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible())
								continue;
							
							for (int j=0;j<pFtrLayer->GetObjectCount();j++)
							{
								pFtr = pFtrLayer->GetObject(j);
								
								if( pFtr==NULL||!pFtr->IsVisible() )
									continue;
								
								CGeometry *pGeo3 = pFtr->GetGeometry();
								if( !pGeo3->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pGeo3->GetDataPointSum()!=2 )
									continue;
								
								pGeo3->GetShape(arrPts);
								double ang1 = GraphAPI::GGetAngle(arrPts[0].x,arrPts[0].y,arrPts[1].x,arrPts[1].y);
								
								//角度是否一致
								if( fabs(ang-ang1)<PI/180 || fabs(fabs(ang-ang1)-PI)<PI/180 || fabs(fabs(ang-ang1)-2*PI)<PI/180 )
								{		
									//是否被同一个地物包围
									if( IsSurrounded(pGeo3,pGeo2) )
									{
										undo.arrOldHandles.Add(FtrToHandle(pFtr));
										m_pEditor->DeleteObject(FtrToHandle(pFtr));	
									}
								}
							}
						}
					}
				}

				//只判断角度是否一致
				if( !bOK )
				{
					for (int i=0;i<pDS->GetFtrLayerCount();i++)
					{
						pFtrLayer = pDS->GetFtrLayerByIndex(i);
						if(pFtrLayer==NULL||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible())
							continue;
						
						for (int j=0;j<pFtrLayer->GetObjectCount();j++)
						{
							pFtr = pFtrLayer->GetObject(j);
							
							if( pFtr==NULL||!pFtr->IsVisible() )
								continue;
							
							pGeo = pFtr->GetGeometry();
							if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pGeo->GetDataPointSum()!=2 )
								continue;
							
							pGeo->GetShape(arrPts);
							double ang1 = GraphAPI::GGetAngle(arrPts[0].x,arrPts[0].y,arrPts[1].x,arrPts[1].y);
							
							if( fabs(ang-ang1)<PI/180 || fabs(fabs(ang-ang1)-PI)<PI/180 || fabs(fabs(ang-ang1)-2*PI)<PI/180 )
							{						
								undo.arrOldHandles.Add(FtrToHandle(pFtr));
								m_pEditor->DeleteObject(FtrToHandle(pFtr));	
							}
						}
					}
				}			
			}

			undo.Commit();			
		}
			
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		Finish();
		m_nStep = 2;
	}
	
	CCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// CReplaceWithPoint Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CReplaceWithPointCommand,CEditCommand)

CReplaceWithPointCommand::CReplaceWithPointCommand()
{
}

CReplaceWithPointCommand::~CReplaceWithPointCommand()
{
}

CString CReplaceWithPointCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_REPLACEWITHPOINT);
}

void CReplaceWithPointCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	
	CEditCommand::Start();
}

void CReplaceWithPointCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		
		if (num>0)
		{
			CUndoFtrs undo(PDOC(m_pEditor),"ReplaceObjWithPoint");

			CFeature *pFtr = HandleToFtr(handles[0]);
			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			{
				Envelope e = pGeo->GetEnvelope();

				CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

				CFtrLayer *pLayer = pDS->GetCurFtrLayer();
				CFeature *pFtr2 = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOPOINT);

				if( pFtr2 )
				{
					m_pEditor->DeleteObject(FtrToHandle(pFtr));	
					undo.arrOldHandles.Add(FtrToHandle(pFtr));	

					CGeometry *pGeo2 = pFtr2->GetGeometry();
					pGeo2->CreateShape(&PT_3DEX(e.CenterX(),e.CenterY(),e.CenterZ(),penLine),1);
					if( pGeo2->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
					{
						CGeoPoint *pGeoPt = (CGeoPoint*)pGeo2;
						pGeoPt->m_fKx = pGeoPt->m_fKy = e.Width()/pDS->GetSymbolDrawScale();
						pGeoPt->EnableFlated(FALSE);
						pGeoPt->EnableGrdWid(TRUE);
					}

					m_pEditor->AddObject(pFtr2,pLayer->GetID());

					undo.arrNewHandles.Add(FtrToHandle(pFtr2));	

					undo.Commit();			
				}
			}			
		}
			
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
IMPLEMENT_DYNCREATE(CSetBoundByDrawCommand,CDrawCommand)

CSetBoundByDrawCommand::CSetBoundByDrawCommand()
{
	m_pDrawProc = NULL;
	m_pFtr = NULL;
	strcat(m_strRegPath,"\\SetBoundByDraw");
}


CSetBoundByDrawCommand::~CSetBoundByDrawCommand()
{
	if( m_pDrawProc )delete m_pDrawProc;
	if(m_pFtr) delete m_pFtr;
}

CGeometry *CSetBoundByDrawCommand::GetCurDrawingObj()
{
	if(!m_pDrawProc->m_pGeoCurve) return NULL;
	return  m_pDrawProc->m_pGeoCurve;
}

CString CSetBoundByDrawCommand::Name()
{
	return StrFromLocalResID(IDS_CMDNAME_SETBOUNDBYDRAW);
}


void CSetBoundByDrawCommand::Start()
{	
	m_pDrawProc = new CDrawCurveProcedure;
	if( !m_pDrawProc )return;
	m_pDrawProc->Init(m_pEditor);
	
	m_pFtr = new CFeature;
	if(!m_pFtr) return;
	CGeoCurve *pGeo = new CGeoCurve;
	if(!pGeo) return;
	m_pFtr->SetGeometry(pGeo);
	m_pDrawProc->m_pGeoCurve = pGeo;
	UpdateParams(FALSE);
	m_pDrawProc->Start();	
	m_pDrawProc->m_bClosed = TRUE;
	CDrawCommand::Start();
}


void CSetBoundByDrawCommand::Finish()
{
	if(m_bSettingModified)
		UpdateParams(TRUE);

	if( m_pFtr!=NULL && m_pFtr->GetGeometry()!=NULL )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		m_pFtr->GetGeometry()->GetShape(arrPts);

		if( arrPts.GetSize()==4 )
		{
			PT_3D pts[4];
			double zmin=0, zmax=0;
			PDOC(m_pEditor)->GetDlgDataSource()->GetBound(pts,&zmin,&zmax);
			pts[0] = arrPts[0];
			pts[1] = arrPts[1];
			pts[2] = arrPts[2];
			pts[3] = arrPts[3];
			PDOC(m_pEditor)->SetBound(pts,zmin,zmax);
		}
	}

	if( m_pDrawProc )
	{		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}

	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	CDrawCommand::Finish();	

	m_nExitCode = CMPEC_STARTOLD;
}


int  CSetBoundByDrawCommand::GetCurPenCode()
{
	if (m_pDrawProc)
	{
		return m_pDrawProc->m_nCurPenCode;
	}
	return CDrawCommand::GetCurPenCode();
}

void CSetBoundByDrawCommand::UpdateParams(BOOL bSave)
{
	CDrawCommand::UpdateParams(bSave);
}

void CSetBoundByDrawCommand::PtClick(PT_3D &pt, int flag)
{
	CDrawCommand::PtClick(pt,flag);

	if( m_pDrawProc )
		m_pDrawProc->PtClick(pt,flag);

	if( m_pDrawProc && m_pDrawProc->m_pGeoCurve )
	{
		if( m_pDrawProc->m_pGeoCurve->GetDataPointSum()>=4 )
			Finish();
	}
}


void CSetBoundByDrawCommand::PtReset(PT_3D &pt)
{
	Abort();
}

void CSetBoundByDrawCommand::Abort()
{
	if( m_pDrawProc )
	{
		if( !IsProcOver(m_pDrawProc) )
			m_pDrawProc->Abort();
		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if(m_pFtr) 
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	CDrawCommand::Abort();
}


int CSetBoundByDrawCommand::GetState()
{
	return GetStateWithSonProcedure(m_pDrawProc);
}


void CSetBoundByDrawCommand::Back() //没有必要存在
{
	if (m_pDrawProc&&m_pDrawProc->m_pGeoCurve)
	{	
		if(m_pDrawProc->m_pGeoCurve->GetDataPointSum()<2)
		{
			Abort();
			return;
		}
		CArray<PT_3DEX,PT_3DEX> pts;
		m_pDrawProc->m_pGeoCurve->GetShape(pts);
		pts.RemoveAt(pts.GetSize()-1);
		m_pDrawProc->m_pGeoCurve->CreateShape(pts.GetData(),pts.GetSize());
		GrBuffer cbuf,vbuf;
		m_pDrawProc->m_pGeoCurve->GetShape()->GetConstGrBuffer(&cbuf);
		m_pDrawProc->m_pGeoCurve->GetShape()->GetVariantGrBuffer(&vbuf,m_pDrawProc->m_pGeoCurve->IsClosed());
		m_pEditor->UpdateDrag(ud_SetConstDrag,&cbuf);
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&vbuf);
	}
}


void CSetBoundByDrawCommand::OnSonEnd(CProcedure *son)
{
	ASSERT( m_pDrawProc==son );

	Abort();
}


CProcedure *CSetBoundByDrawCommand::GetActiveSonProc(int MsgType)
{
	if( MsgType==msgPtClick )
		return NULL;

	return m_pDrawProc;
}


void CSetBoundByDrawCommand::GetParams(CValueTable &tab)
{
}

void CSetBoundByDrawCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("SetBoundByDraw",StrFromLocalResID(IDS_CMDNAME_SETBOUNDBYDRAW));
}


void CSetBoundByDrawCommand::SetParams(CValueTable& tab,BOOL bInit)
{
}




//////////////////////////////////////////////////////////////////////
// CModifyDemPointCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CModifyDemPointCommand,CDrawCommand)

CModifyDemPointCommand::CModifyDemPointCommand()
{
	m_nStep = -1;
	m_nBoundType = 0;
	m_bUp = TRUE;
	m_lfDX = 10;
	m_lfDY = 10;
	strcat(m_strRegPath,"\\ModifyDemPoint");

	m_bAutoGo = true;
}


CModifyDemPointCommand::~CModifyDemPointCommand()
{
	
}

DrawingInfo CModifyDemPointCommand::GetCurDrawingInfo()
{
	return DrawingInfo();
}

int  CModifyDemPointCommand::GetCurPenCode()
{
	return CDrawCommand::GetCurPenCode();
}

CString CModifyDemPointCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_MODIFYDEMPOINT);
}


void CModifyDemPointCommand::Start()
{	
	m_lfDX = 10;
	m_lfDY = 10;
	m_nBoundType = 0;
	if( m_nBoundType==0 )
	{
		CArray<PT_3D,PT_3D> arrBounds;
		arrBounds.SetSize(4);
		PDOC(m_pEditor)->GetDlgDataSource()->GetBound(arrBounds.GetData(),NULL,NULL);		
		m_arrBound.RemoveAll();
		for( int i=0; i<arrBounds.GetSize(); i++)
		{
			m_arrBound.Add(PT_3DEX(arrBounds[i],penLine));
		}
	}
	CDrawCommand::Start();
	GotoState(PROCSTATE_READY);
	m_nStep = 0;
	m_bUp = TRUE;

	m_pEditor->CloseSelector();
}


void CModifyDemPointCommand::PtReset(PT_3D &pt)
{	
	Abort();
}

void CModifyDemPointCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CCommand::Finish();
}


void CModifyDemPointCommand::Abort()
{
	UpdateParams(TRUE);
	m_pEditor->UpdateDrag(ud_ClearDrag);

	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CCommand::Abort();

//	m_nExitCode = CMPEC_STARTOLD;
}


void CModifyDemPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ModifyDemPoint",StrFromResID(IDS_CMDNAME_MODIFYDEMPOINT));
	param->BeginOptionParam(PF_TRIMBOUND,StrFromResID(IDS_CMDPLANE_BOUND));
	param->AddOption(StrFromResID(IDS_CMDPLANE_WSBOUND),0,' ',m_nBoundType==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SELECT),1,' ',m_nBoundType==1);
	param->EndOptionParam();
	if( m_nBoundType==1 || bForLoad ) 
	{
		param->AddButtonParam("Select",StrFromResID(IDS_CMDPLANE_SELECT),NULL);
	}
	
	param->AddParam("AutoGo",m_bAutoGo, StrFromResID(IDS_CMDPLANE_AUTOGO));

	if (m_bAutoGo || bForLoad)
	{
		param->AddParam(PF_XSTEP,m_lfDX,StrFromResID(IDS_CMDPLANE_XSTEP));
		param->AddParam(PF_YSTEP,m_lfDY,StrFromResID(IDS_CMDPLANE_YSTEP));
	}

	param->AddParam("Turnback",'t',"Turn back");
	
}

void CModifyDemPointCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_TRIMBOUND,var) )
	{
		m_nBoundType = (long)(_variant_t)*var;	
		SetSettingsModifyFlag();

		if( m_nBoundType==0 )
		{
			CArray<PT_3D,PT_3D> arrBounds;
			arrBounds.SetSize(4);
			PDOC(m_pEditor)->GetDlgDataSource()->GetBound(arrBounds.GetData(),NULL,NULL);
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 2;
			
			m_arrBound.RemoveAll();
			for( int i=0; i<arrBounds.GetSize(); i++)
			{
				m_arrBound.Add(PT_3DEX(arrBounds[i],penLine));
			}
		}

		if(!bInit)
 			Abort();

	}

	if( tab.GetValue(0,PF_XSTEP,var) )
	{
		m_lfDX = (double)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_YSTEP,var) )
	{
		m_lfDY = (double)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"AutoGo",var) )
	{
		m_bAutoGo = (bool)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"Select",var) )
	{
		m_pEditor->OpenSelector(SELMODE_SINGLE);
	}
	if( tab.GetValue(0,"Turnback",var) )
	{
		m_bUp = !m_bUp;
	}

	var = NULL;
	if( tab.GetValue(0,"Bound", var) && m_nBoundType==1 )
	{
		if( var )
		{
			m_arrBound.RemoveAll();
			var->GetShape(m_arrBound);
		}
	}
	CDrawCommand::SetParams(tab,bInit);
}

void CModifyDemPointCommand::GetParams(CValueTable& tab)
{
	CDrawCommand::GetParams(tab);
	
	_variant_t var;	
	var = (long)m_nBoundType;
	tab.AddValue(PF_TRIMBOUND,&(CVariantEx)(long)(var));
	var = (double)m_lfDX;
	tab.AddValue(PF_XSTEP,&CVariantEx(var));
	var = (double)m_lfDY;
	tab.AddValue(PF_YSTEP,&CVariantEx(var));

	if( m_nBoundType==1 && m_arrBound.GetSize()>=4 )
	{
		CVariantEx varex;
		varex.SetAsShape(m_arrBound);
		tab.AddValue("Bound",&varex);
	}
}

void CModifyDemPointCommand::PtDblClick(PT_3D &pt, int flag)
{
	PtClick(pt,flag);
}

void CModifyDemPointCommand::PtClick(PT_3D &pt, int flag)
{
	GotoState(PROCSTATE_PROCESSING);

	if (m_nBoundType == 1)
	{
		if( m_pEditor->IsSelectorOpen() )
		{
			if( !CEditCommand::CanGetSelObjs(flag,FALSE) )
				return;

			int num;
			const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
			if( num==1 )
			{
				CFeature *pFtr = HandleToFtr(handles[0]);
				CGeometry *pGeo = pFtr->GetGeometry();

				if( pGeo->GetDataPointSum()>=4 )
				{
					m_arrBound.RemoveAll();
					pGeo->GetShape(m_arrBound);
					
					m_pEditor->CloseSelector();

					m_bSettingModified = TRUE;
					UpdateParams(TRUE);
				}
			}

			m_pEditor->GetSelection()->DeselectAll();
			m_pEditor->OnSelectChanged();
			return;
		}
	}

	if( m_arrBound.GetSize()<4 )return;

	//找到当前最近的点
	int index = -1;
	FTR_HANDLE hFtr;
	hFtr = FindNearestVertex(pt,index);
	if( hFtr==NULL || index<0 )
	{
		AfxMessageBox(IDS_ERR_NOTFOUNDDEMPOINTS);
		return;
	}

	//修改地物的坐标
	CGeometry *pGeo = HandleToFtr(hFtr)->GetGeometry();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);
	if( index>=0 && index<arrPts.GetSize() )
	{
		PDOC(m_pEditor)->DeleteObject(hFtr,FALSE);

		CUndoVertex undo(m_pEditor,Name());
		undo.bRepeat = FALSE;
		undo.handle = hFtr;
		undo.nPtType = PT_KEYCTRL::typeKey;
		undo.nPtIdx = index;
		undo.ptOld = arrPts[index];

		arrPts[index].z = pt.z;
		undo.ptNew = arrPts[index];

		undo.Commit();

		pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());

		PDOC(m_pEditor)->RestoreObject(hFtr);
		//PDOC(m_pEditor)->UpdateObject(hFtr);
		PDOC(m_pEditor)->UpdateView(0,hc_Refresh,0);
	}
	else
	{
		AfxMessageBox(IDS_ERR_NOTFOUNDDEMPOINTS);
		return;
	}

	if( !m_bAutoGo )
		return;

	//跳跃到下一个点
	double ymin, ymax;

	if( !GetBTofBound(pt.x,ymin,ymax) )
		return;

	PT_3D newPt = pt;
	newPt.y = pt.y + (m_bUp?m_lfDY:-m_lfDY);
	if( newPt.y<=ymin || newPt.y>=ymax )
	{
		newPt = pt;
		m_bUp = !m_bUp;
		newPt.x += m_lfDX;

		if( !GetBTofBound(pt.x,ymin,ymax) )
			return;

		if( m_bUp )
			newPt.y = ceil((ymin-newPt.y)/m_lfDY)*m_lfDY + newPt.y;
		else
			newPt.y = floor((ymax-newPt.y)/m_lfDY)*m_lfDY + newPt.y;
	}
	
	//找到最近的地物点，设置为当前坐标
	int index2;
	FTR_HANDLE hFtr2 = FindNearestVertex(newPt,index2);

	if( hFtr2==NULL )
		return;
	if( hFtr2==hFtr && index2==index )
		return;

	pGeo = HandleToFtr(hFtr2)->GetGeometry();
	newPt = pGeo->GetDataPoint(index2);
	newPt.z = pt.z;
	((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&newPt);
}

//计算给定的 x 坐标上的y轴平行线 与范围线相交的交点的y坐标的最大最小值
BOOL CModifyDemPointCommand::GetBTofBound(double x, double &ymin, double& ymax)
{
	PT_3D pt1, pt2, pt3;
	pt1.x = pt2.x = x;
	pt1.y = -1e+100; pt2.y = 1e+100;
	pt3.z = 0;

	if( m_arrBound.GetSize()<4 )
		return FALSE;

	CArray<PT_3D,PT_3D> arrIntersect;
	PT_3DEX *bounds = m_arrBound.GetData();
	int npt = m_arrBound.GetSize();
	for(int i=0; i<npt; i++)
	{
		int i1 = (i+1)%npt;
		if( GraphAPI::GGetLineIntersectLineSeg(bounds[i].x,bounds[i].y,bounds[i1].x,bounds[i1].y,
			pt1.x,pt1.y,pt2.x,pt2.y,&pt3.x,&pt3.y,NULL) )
		{
			arrIntersect.Add(pt3);
		}
	}
	
	if( arrIntersect.GetSize()<=1 )
		return FALSE;
	
	Envelope e;
	e = CreateEnvelopeFromPts(arrIntersect.GetData(),arrIntersect.GetSize());

	ymin = e.m_yl; ymax = e.m_yh;

	return TRUE;

}


FTR_HANDLE CModifyDemPointCommand::FindNearestVertex(PT_3D pt, int& index)
{
	Envelope e;
	e.m_xl = pt.x - m_lfDX*0.5; e.m_xh = pt.x + m_lfDX*0.5;
	e.m_yl = pt.y - m_lfDY*0.5; e.m_yh = pt.y + m_lfDY*0.5;

	CCoordSys cs;
	cs.Create44Matrix(NULL);
	int num = m_pEditor->GetDataQuery()->FindObjectInRect(e,NULL/*m_pEditor->GetCoordWnd().m_pSearchCS*/);
	if( num<=0 )return NULL;

	const CPFeature *pftrs = m_pEditor->GetDataQuery()->GetFoundHandles(num);
	double dis, min = -1;
	FTR_HANDLE hret = NULL;

	for( int i=0; i<num; i++)
	{
		CGeometry *pGeo = pftrs[i]->GetGeometry();
		if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
			continue;

		PT_KEYCTRL kpt = pGeo->FindNearestKeyCtrlPt(pt,m_lfDX,NULL,1);
		PT_3DEX expt = pGeo->GetDataPoint(kpt.index);
		dis = GraphAPI::GGet2DDisOf2P((PT_3D)expt,pt);

		if( min<0 || min>dis )
		{
			min = dis;
			hret = FtrToHandle(pftrs[i]);
			index = kpt.index;
		}
	}

	return hret;
}

/////////////////////////////////////////////////////////////////////
// CCombineCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMapManualMatchCommand,CEditCommand)

CMapManualMatchCommand::CMapManualMatchCommand()
{
	m_nStep = -1;
	m_pObjOld0 = NULL;
	m_pObjOld1 = NULL;
	m_pObjNew = NULL;
	
	m_ptNum0 = -1;
	m_ptNum1 = -1;
	m_bMatchHeight = FALSE;
	m_nJoinMode = 0;
	m_bSurfaceMode = FALSE;
	strcat(m_strRegPath,"\\MapManualMatch");
}

CMapManualMatchCommand::~CMapManualMatchCommand()
{
}

void CMapManualMatchCommand::Abort()
{
	UpdateParams(TRUE);
    m_nStep = -1;
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

CString CMapManualMatchCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MAPMANUALMATCH);
}

void CMapManualMatchCommand::Start()
{
	if( !m_pEditor )return;

	m_pObjOld0 = NULL;
	m_pObjOld1 = NULL;
	m_pObjNew = NULL;
	m_bMatchHeight = FALSE;
	m_ptNum0 = -1;
	m_ptNum1 = -1;

	m_nJoinMode = 0;
	m_bSurfaceMode = FALSE;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->OpenSelector();	
}

template<class T>
double  GGetNearstDisPToPointSet0(T pointset[],int nCount,const T &p,T &q,double *nIndex)
{
	int i;
	double pencode=DBL_MAX,td;
	T tq,cq;
	tq = pointset[0];
	for(i=0;i<nCount-1;i++)
	{	
		td = GraphAPI::GGetNearestDisOfPtToLine(pointset[i].x,pointset[i].y,pointset[i+1].x,
			pointset[i+1].y,p.x,p.y,&(tq.x),&(tq.y),false);
		if(td<pencode)
		{
			if(nIndex) 
			{
				double len = GraphAPI::GGetDisOf2P2D(pointset[i].x,pointset[i].y,pointset[i+1].x,pointset[i+1].y);
				if (len < 1e-4)
				{
					*nIndex = i;
				}
				else
				{
					*nIndex = i + GraphAPI::GGetDisOf2P2D(tq.x,tq.y,pointset[i].x,pointset[i].y)/len;
				}
			}
			pencode=td;
			cq=tq;
		}
	}
	q=cq;
	return pencode;
} 


struct IntersectPt
{
	double t;
	PT_3DEX pt;
};

/*******************************************************************************
函数名   : <GGetBreakCurve>
功能     : 获取折线pts经曲线pts1打断后的线串
参数     :  
	[IN]  pts,  num : 表征第一条直线
	[IN]  pts1 , num1: 表征打断线
           
    [OUT]	ret ,index:传出部分线及索引
*******************************************************************************/
template<class T>
static BOOL GGetBreakCurve(T *pts, int num, T *pts1, int num1, CArray<T,T> &ret, CUIntArray &index)
{
	Envelope e0, e1;
	e0.CreateFromPts(pts,num,sizeof(T));
	e1.CreateFromPts(pts1,num1,sizeof(T));

	if (!e0.bIntersect(&e1)) return FALSE;

	// 相交点排序
	CArray<IntersectPt,IntersectPt> intersectPts;

	for (int i=0; i<num-1; i++)
	{
		if (!e1.bIntersect(&pts[i],&pts[i+1])) continue;

		for (int j=0; j<num1-1; j++)
		{
			T pt;
			double t;
			if (GraphAPI::GGetLineIntersectLineSeg(pts[i].x,pts[i].y,pts[i+1].x,pts[i+1].y,pts1[j].x,pts1[j].y,pts1[j+1].x,pts1[j+1].y,&pt.x,&pt.y,&t,NULL))
			{
				if (t < 0 && t > 1) continue;
				// 排序
				int size = intersectPts.GetSize();
				for (int k=0; k<size; k++)
				{
					if (i+t < intersectPts[k].t)
					{
						GraphAPI::GGetPtZOfLine(&pts[i],&pts[i+1],&pt);
						IntersectPt pt0;
						pt0.pt = pt;
						pt0.pt.pencode = pts[i].pencode;
						pt0.t = i + t;
						intersectPts.Add(pt0);
						break;
					}
				}

				if (k >= size)
				{
					GraphAPI::GGetPtZOfLine(&pts[i],&pts[i+1],&pt);
					IntersectPt pt0;
					pt0.pt = pt;
					pt0.pt.pencode = pts[i].pencode;
					pt0.t = i + t;
					intersectPts.Add(pt0);
				}
			}
		}

		
	}

	BOOL bClosed = FALSE;
	if (num >= 3)
	{
		T pt0,pt1;
		
		pt0 = pts[0]; pt1 = pts[num-1];
		if( fabs(pt0.x-pt1.x)<1e-4 && fabs(pt0.y-pt1.y)<1e-4 && fabs(pt0.z-pt1.z)<1e-4 )
			bClosed = TRUE;
	}

	int size = intersectPts.GetSize();

	if (size <= 0) return FALSE;

	if (bClosed)
	{
		if (size == 1)
		{
			for (int k=0; k<num; k++)
			{
				ret.Add(pts[k]);
				if (int(intersectPts[0].t) == k)
				{
					ret.Add(intersectPts[0].pt);
				}
			}
			
		}
		else if (size > 1)
		{
			// 尾首相联
			index.Add(ret.GetSize());				
			
			ret.Add(intersectPts[size-1].pt);
			for (int k=ceil(intersectPts[size-1].t); k<num-1; k++)
			{
				ret.Add(pts[k]);
			}
			
			for (k=0; k<int(intersectPts[0].t); k++)
			{
				ret.Add(pts[k]);
			}
			
			ret.Add(intersectPts[0].pt);
			
			index.Add(ret.GetSize());
			
			// 依次将地物拆分
			for (int i=0; i<size-1; i++)
			{			
				index.Add(ret.GetSize());
				
				ret.Add(intersectPts[i].pt);
				for (int k=ceil(intersectPts[i].t); k<floor(intersectPts[i+1].t); k++)
				{
					ret.Add(pts[k]);
				}
				ret.Add(intersectPts[i+1].pt);

				index.Add(ret.GetSize());
				
			}
		}
	}
	else
	{
		// 首段
		index.Add(ret.GetSize());				
		
		//ret.Add(pts[0]);
		for (int k=0; k<=int(intersectPts[0].t); k++)
		{
			ret.Add(pts[k]);
		}
		ret.Add(intersectPts[0].pt);
		
		index.Add(ret.GetSize());

		// 依次将地物拆分
		for (int i=0; i<size-1; i++)
		{			
			index.Add(ret.GetSize());
			
			ret.Add(intersectPts[i].pt);
			for (int k=ceil(intersectPts[i].t); k<floor(intersectPts[i+1].t); k++)
			{
				ret.Add(pts[k]);
			}
			ret.Add(intersectPts[i+1].pt);

			index.Add(ret.GetSize());
			
		}

		// 尾段
		index.Add(ret.GetSize());				
		
		ret.Add(intersectPts[size-1].pt);
		for (k=ceil(intersectPts[size-1].t); k<num; k++)
		{
			ret.Add(pts[k]);
		}
		
		index.Add(ret.GetSize());

	}

	return TRUE;
}

/*******************************************************************************
函数名   : <GGetPartCurve>
功能     : 获取折线pts上的线，部分线由arr决定，若是闭合线，则arr为三个点，第三个点决定获取哪个部分；若为
			非闭合线，则arr为一个点，获取靠近此点的那段线
参数     :  
	[IN]  pts,  num : 表征第一条直线
	[IN]  arr , num1: 表征决策点
           
    [OUT]	ret :传出部分线
*******************************************************************************/
template<class T>
static void GGetPartCurve(T *pts, int num, T *arr, int num1, CArray<T,T> &ret, BOOL bFirst)
{
	if (!pts || num <= 1 || !arr) return;

// 	BOOL bClosed = FALSE;
// 	if (num >= 3)
// 	{
// 		T pt0,pt1;
// 		
// 		pt0 = pts[0]; pt1 = pts[num-1];
// 		if( fabs(pt0.x-pt1.x)<1e-4 && fabs(pt0.y-pt1.y)<1e-4 && fabs(pt0.z-pt1.z)<1e-4 )
// 			bClosed = TRUE;
// 	}

	int i;
	if (num1 == 1)
	{
		int index;
		T nearstPt;
		GraphAPI::GGetNearstDisPToPointSet2D(pts,num,arr[0],nearstPt,&index);

		double dis = 0, allLen = 0;
		for (i=0; i<num-1; i++)
		{			
			if (i == index)
			{
				dis = allLen + sqrt((nearstPt.x-pts[index].x)*(nearstPt.x-pts[index].x)+(nearstPt.y-pts[index].y)*(nearstPt.y-pts[index].y));
			}
			allLen += sqrt((pts[i+1].x-pts[i].x)*(pts[i+1].x-pts[i].x)+(pts[i+1].y-pts[i].y)*(pts[i+1].y-pts[i].y));
		}

		if( allLen<=0 || dis/allLen<=0.5 )
		{
			if (bFirst)
			{
				for (i=num-1; i>index; i--)
				{
					ret.Add(pts[i]);
				}
				ret.Add(nearstPt);
			}
			else
			{
				ret.Add(nearstPt);
				for (i=index+1; i<num; i++)
				{
					ret.Add(pts[i]);
				}

			}
			
		}
		else
		{
			if (bFirst)
			{
				for (i=0; i<=index; i++)
				{
					ret.Add(pts[i]);
				}
				ret.Add(nearstPt);

			}
			else
			{
				ret.Add(nearstPt);
				for (i=index; i>=0; i--)
				{
					ret.Add(pts[i]);
				}

			}	
		}

		return;
	}

	
	T nearstPts[3];
	double index[3];
	for (i=0; i<3; i++)
	{
		GGetNearstDisPToPointSet0(pts,num,arr[i],nearstPts[i],&index[i]);
	}

	double start = index[0], end = index[2];

	CArray<T,T> ret1;

	ret.Add(nearstPts[0]);

	if (start < end)
	{
		if (index[1] >= start && index[1] <= end)
		{
			for ( i= ceil(start); i<= floor(end); i++)
			{
				ret.Add(pts[i]);
			}
		}
		else
		{
			// 首段
			for ( i= floor(start); i>=0; i--)
			{
				ret.Add(pts[i]);
			}
			
			// 尾段
			for (i=num-2; i>=ceil(end); i--)
			{
				ret.Add(pts[i]);
			}
			
		}

	}
	else
	{
		if (index[1] >= end && index[1] <= start)
		{
			for ( i= floor(start); i>= ceil(end); i--)
			{
				ret.Add(pts[i]);
			}
		}
		else
		{
			// 尾段
			for (i=ceil(start); i<num-1; i++)
			{
				ret.Add(pts[i]);
			}

			// 首段
			for ( i=0; i<=floor(end); i++)
			{
				ret.Add(pts[i]);
			}
			
		}

	}

	ret.Add(nearstPts[2]);	
}

void CMapManualMatchCommand::PtClick(PT_3D &pt, int flag)
{
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;
	
	int num ;
	const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);

	if( m_nStep==0 )
	{
		if( num>0 )
		{
			if( !CanGetSelObjs(flag,FALSE) )return;
			
			CFeature *pFtr = HandleToFtr(m_pEditor->GetSelection()->GetLastSelectedObj());
			if (m_pObjOld0 != NULL && m_pObjOld0 != pFtr)
			{
				m_nStep = 1;
				goto SelAnotherFtr;
			}

			m_pObjOld0 = pFtr;

			CGeometry *pObj = m_pObjOld0->GetGeometry();
			
			if( !pObj )return;
			
			m_ptNum0++;
			m_SelPts0[m_ptNum0] = PT_3DEX(pt,penNone);
			if (m_ptNum0 == 2 || !m_bSurfaceMode)
			{
				m_nStep = 1;
			}
			else
			{
				m_pEditor->DeselectObj(FtrToHandle(m_pObjOld0),TRUE);
			}
			
			GotoState(PROCSTATE_PROCESSING);

			return;
		}
		
	}

SelAnotherFtr:

	if( m_nStep==1 )
	{
		if (num == 0)
		{
			m_nStep = 2;
 			m_pEditor->CloseSelector();
 			goto Match;
		}
		else if( num>0 )
		{		
			if( !CanGetSelObjs(flag,FALSE) )return;
			CFeature *pFtr = HandleToFtr(m_pEditor->GetSelection()->GetLastSelectedObj());

			if (m_pObjOld1 != NULL && m_pObjOld1 != pFtr)
			{
				m_nStep = 2;
				m_pEditor->CloseSelector();
				CEditCommand::PtClick(pt,flag);
				return;
			}
			
			m_pObjOld1 = pFtr;

			if( m_pObjOld1!=m_pObjOld0 )
			{
				CGeometry *pObj = m_pObjOld1->GetGeometry();
				if( !pObj )return;
				const CShapeLine *pSL = ((CGeoCurve*)pObj)->GetShape();
				
				m_ptNum1++;
				m_SelPts1[m_ptNum1] = PT_3DEX(pt,penNone);
				if (m_ptNum1 == 2 || !m_bSurfaceMode)
				{
					m_nStep = 2;
					m_pEditor->CloseSelector();
					CEditCommand::PtClick(pt,flag);
					return;
				}
				else
				{
					m_pEditor->DeselectObj(FtrToHandle(m_pObjOld1),TRUE);
				}
				
			}
		}
	}

Match:
	if( m_nStep>=2 )
	{
		int size;
		PT_3DEX t0, t;
		int first = 1;
		CArray<PT_3DEX,PT_3DEX> pts, pts0, pts1;
		CArray<PT_3DEX,PT_3DEX> arr0, arr1;

		CFtrLayer* pLayer0 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(m_pObjOld0);
		CFtrLayer* pLayer1 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(m_pObjOld1);
		if (!pLayer0||!pLayer1)
		{
			goto ReSelectFtrs;
			//return;
		}
		if(pLayer1!=pLayer0&& strcmp(pLayer1->GetName(),pLayer0->GetName())!=0 )
		{
			if( AfxMessageBox(IDS_ERR_INCONSISTENT_FID,MB_YESNO)!=IDYES )
			{
				goto ReSelectFtrs;
// 				Abort();
// 				return;
			}
		}
		
		//CArray<PT_3DEX,PT_3DEX> pts, pts0, pts1;
		//CArray<PT_3DEX,PT_3DEX> arr0, arr1;
		//int i, first = 1;
		//PT_3DEX t0, t;
		m_pObjOld0->GetGeometry()->GetShape(arr0);
		m_pObjOld1->GetGeometry()->GetShape(arr1);

// 		if (m_pObjOld0->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
// 		{
// 			arr0.Add(arr0[0]);
// 		}
// 
// 		if (m_pObjOld1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
// 		{
// 			arr1.Add(arr1[0]);
// 		}

		GGetPartCurve(arr0.GetData(),arr0.GetSize(),m_SelPts0,m_ptNum0+1,pts0,TRUE);
		GGetPartCurve(arr1.GetData(),arr1.GetSize(),m_SelPts1,m_ptNum1+1,pts1,FALSE);

		if( m_bMatchHeight )
		{
			PT_3DEX t1,t2;
			t1 = pts0[pts0.GetSize()-1];
			t2 = pts1[0];
			if( fabs(t1.z-t2.z)>=1e-4 )
			{
				goto ReSelectFtrs;
// 				Abort();
// 				return;
			}
		}

		pts.Append(pts0);
		pts.Append(pts1);
		
		size = GraphAPI::GKickoffSamePoints(pts.GetData(),pts.GetSize());
		pts.SetSize(size);

		size = pts.GetSize();
		if (size < 2)
		{
			goto ReSelectFtrs;
// 			Abort();
// 			return;
		}	

		if (m_nJoinMode == 0)
		{
			CArray<PT_3DEX,PT_3DEX> joinPts, joinRet;
			CUIntArray index;
			
			CFtrLayer *pLayer = pDS->GetFtrLayer(m_strBoundLayer);
			if (!pLayer)
			{
				AfxMessageBox(StrFromResID(IDS_PARAM_ERROR));
				return;
			}	
			
			CGeometry *pGeo = m_pObjOld0->GetGeometry();

			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && (m_ptNum0 == 2 || ((CGeoCurve*)pGeo)->IsClosed())))
			{
				pts.Add(pts[0]);
				size++;
			}

			int i = 0;
			int nObj = pLayer->GetObjectCount();
			for (i = 0; i < nObj; i++)
			{
				CFeature *pFtr = pLayer->GetObject(i);
				if (!pFtr) continue;
				CGeometry *pGeo = pFtr->GetGeometry();
				if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
					continue;
				joinPts.RemoveAll();
				pGeo->GetShape(joinPts);
				if (GGetBreakCurve(pts.GetData(), size, joinPts.GetData(), joinPts.GetSize(), joinRet, index))
					break;
			}

			if (i < nObj)
			{
				CUndoFtrs undo(m_pEditor,Name());			

				for (int i=0; i<index.GetSize(); i+=2)
				{
					int start, end;
					start = index[i];
					end = index[i+1];

					CArray<PT_3DEX,PT_3DEX> ret0;
					for (int j=start; j<end; j++)
					{
						ret0.Add(joinRet[j]);
					}

					if (ret0.GetSize() < 1)
					{
						continue;
					}

					m_pObjNew = m_pObjOld0->Clone();
					
					m_pObjNew->SetID(OUID());
					
					CGeometry *pNew = m_pObjNew->GetGeometry();	
					
					if (pNew->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || (pNew->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && (m_ptNum0 == 2 || ((CGeoCurve*)pNew)->IsClosed())) )
					{
						ret0.Add(ret0[0]);
					}

					m_pObjNew->GetGeometry()->CreateShape(ret0.GetData(),ret0.GetSize());	
					
					if (!m_pEditor->AddObject(m_pObjNew,m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(m_pObjOld0))))
					{
						delete m_pObjNew;
						Abort();
						return;
					}

					GETXDS(m_pEditor)->CopyXAttributes(m_pObjOld0,m_pObjNew);
					undo.arrNewHandles.Add(FtrToHandle(m_pObjNew));
				}

				undo.arrOldHandles.Add(FtrToHandle(m_pObjOld0));
				undo.arrOldHandles.Add(FtrToHandle(m_pObjOld1));
				
				m_pEditor->DeleteObject(FtrToHandle(m_pObjOld0));
				m_pEditor->DeleteObject(FtrToHandle(m_pObjOld1));

				undo.Commit();
			}
		}
		else
		{
			m_pObjNew = m_pObjOld0->Clone();
			
			m_pObjNew->SetID(OUID());
			
			CGeometry *pNew = m_pObjNew->GetGeometry();	
			
			if (pNew->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || (pNew->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && (m_ptNum0 == 2 || ((CGeoCurve*)pNew)->IsClosed())) )
			{
				pts.Add(pts[0]);
			}
			
			m_pObjNew->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());	
			
			if (!m_pEditor->AddObject(m_pObjNew,m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(m_pObjOld0))))
			{
				delete m_pObjNew;
				Abort();
				return;
			}
			
			CUndoFtrs undo(m_pEditor,Name());
			
			GETXDS(m_pEditor)->CopyXAttributes(m_pObjOld0,m_pObjNew);
			undo.arrNewHandles.Add(FtrToHandle(m_pObjNew));
			
			undo.arrOldHandles.Add(FtrToHandle(m_pObjOld0));
			undo.arrOldHandles.Add(FtrToHandle(m_pObjOld1));
			
			m_pEditor->DeleteObject(FtrToHandle(m_pObjOld0));
			m_pEditor->DeleteObject(FtrToHandle(m_pObjOld1));
			
			undo.Commit();

		}

ReSelectFtrs:		
		m_pEditor->DeselectAll();

 		m_pEditor->OnSelectChanged();

		m_nStep = 0;
		m_pObjOld0 = NULL;		
		m_pObjNew = NULL;		
 		m_pObjOld1 = NULL;
		m_ptNum0 = -1;
		m_ptNum1 = -1;
		m_pEditor->OpenSelector(SELMODE_SINGLE);
		/*Finish();

		m_pObjOld0 = NULL;		
		m_pObjNew = NULL;		
 		m_pObjOld1 = NULL;
		m_nStep = 3;*/		
	}
	
	CEditCommand::PtClick(pt,flag);
}

void CMapManualMatchCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (_bstr_t)(LPCSTR)(m_strBoundLayer);
	tab.AddValue("strBoundLayer",&CVariantEx(var));
	var = (long)(m_nJoinMode);
	tab.AddValue(PF_JOIN_MODE,&CVariantEx(var));
	var = (bool)(m_bMatchHeight);
	tab.AddValue(PF_COMBINEHEIMATCH,&CVariantEx(var));
	var = (bool)(m_bSurfaceMode);
	tab.AddValue(PF_COMBINESURFACEMODE,&CVariantEx(var));
}

void CMapManualMatchCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("MapManualMatchCommand",StrFromLocalResID(IDS_CMDNAME_MAPMANUALMATCH));

	param->BeginOptionParam(PF_JOIN_MODE,StrFromResID(IDS_CMDPLANE_MM_JOINMODE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_BREAK),0,' ',m_nJoinMode==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_JOIN),1,' ',m_nJoinMode==1);
	param->EndOptionParam();

	if (m_nJoinMode == 0 || bForLoad)
	{
		param->AddLayerNameParam("strBoundLayer", (LPCTSTR)m_strBoundLayer, StrFromResID(IDS_TK_LAYER), NULL, LAYERPARAMITEM_NOTEMPTY_LINEAREA);
	}

	param->AddParam(PF_COMBINEHEIMATCH,(bool)m_bMatchHeight,StrFromResID(IDS_CMDPLANE_MATCHHEIGHT));
	param->AddParam(PF_COMBINESURFACEMODE,(bool)m_bSurfaceMode,StrFromResID(IDS_CMDPLANE_MATCHSURFACEMODE));
	//param->AddParam(PF_COMBINEMODE,(bool)m_bLinkCountor,StrFromResID(IDS_CMDPLANE_LINKCONTOUR));		
}


void CMapManualMatchCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;

	if( tab.GetValue(0,"strBoundLayer",var) )
	{
		m_strBoundLayer = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_JOIN_MODE,var) )
	{
		m_nJoinMode = (long)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_COMBINEHEIMATCH,var) )
	{
		m_bMatchHeight = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,PF_COMBINESURFACEMODE,var) )
	{
		m_bSurfaceMode = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

// 	if( tab.GetValue(0,PF_COMBINEMODE,var) )
// 	{
// 		m_bLinkCountor = (bool)(_variant_t)*var;
// 		SetSettingsModifyFlag();
// 		if(!bInit)
// 			Abort();
// 	}
	CEditCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////
// CMapMatchCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMapMatchCommand,CEditCommand)

CMapMatchCommand::CMapMatchCommand()
{
	m_fJoinToler = 0.5;
	m_lfZToler = 0.5;
//	m_nScopeType = 0;
	m_nModifyMode = 0;
	m_nJoinMode = 0;
	m_bMatchColor = FALSE;
	m_bMatchLineType = FALSE;
//	m_bMatchLineWidth = FALSE;
	m_bMatchLineHei = FALSE;
	m_handleBound = 0;
	m_sLastSlectMap = m_sSelectMap;

	m_nStep = -1;
	m_bMark = FALSE;
	m_sMapName.RemoveAll();
	strcat(m_strRegPath,"\\MapMatch");
}

CMapMatchCommand::~CMapMatchCommand()
{

}

CString CMapMatchCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MAPAUTOMATCH);
}

void CMapMatchCommand::Start()
{
	if( !m_pEditor )return;

	CEditCommand::Start();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
}

void CMapMatchCommand::Abort()
{
	m_nStep = -1;
	UpdateParams(TRUE);
	m_pEditor->RefreshView();
	UpdateMark(FALSE);
	CEditCommand::Abort();
}

void CMapMatchCommand::Finish()
{
	UpdateParams(TRUE);
	m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

static bool GetNearestDisOfPtToLine(double lfX0, double lfY0, double lfX1, double lfY1,
	double lfXt, double lfYt, 
	double *plfXr, double *plfYr,double *plfTr,double *plfDis,
	double lfToler)
{
	//剔除明显不满足的
	if (lfXt+lfToler<min(lfX0,lfX1)||
		lfXt-lfToler>max(lfX0,lfX1)||
		lfYt+lfToler<min(lfY0,lfY1)||
		lfYt-lfToler>max(lfY0,lfY1))
	{
		return false;
	}
	double x,y;
	double t;
	GraphAPI::GGetPerpendicular(lfX0,lfY0,lfX1,lfY1,lfXt,lfYt,&x,&y,&t);
	
	
	double d1 = fabs(lfX0-x)+fabs(lfY0-y);
	double d2 = fabs(lfX1-x)+fabs(lfY1-y);
	double d3 = fabs(lfX0-lfX1)+fabs(lfY0-lfY1);
	if( d1>d3 || d2>d3 )return false;
	double temp = sqrt((lfXt-x)*(lfXt-x)+(lfYt-y)*(lfYt-y));
	if (temp>lfToler)return false;
	
	if(plfXr)*plfXr=x; 
	if(plfYr)*plfYr=y; 
	if(plfTr)*plfTr=t;
	if(plfDis)*plfDis=temp;
	return true;
}

void CMapMatchCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;
	GetFtrCodes(m_sMapName);
	PT_3D pts[4];
	PT_3DEX expts[4];
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	CFtrLayer* pFtrLayer=NULL;
	CFeature *pFtr=NULL;
	m_sLastSlectMap = m_sSelectMap;//备份下上次所选图幅
	convertStringToStrArray(m_sSelectMap, m_arrSelectmap);
	if (m_arrSelectmap.GetSize()<=0)
		return;//无图幅
	
	for(int i=0;i<m_arrGeoCurve.GetSize();i++)
	{
		delete m_arrGeoCurve[i];
	}
	m_arrGeoCurve.RemoveAll();
	for ( i=0;i<pDS->GetFtrLayerCount();i++)
	{
		pFtrLayer = pDS->GetFtrLayerByIndex(i);
		if(pFtrLayer==NULL||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible()||strcmp(pFtrLayer->GetName(),m_strLayNameOfTKName)!=0)
			continue;
		for (int j=0;j<pFtrLayer->GetObjectCount();j++)//同一个层中多少地物
		{
			pFtr = pFtrLayer->GetObject(j);

			if(pFtr==NULL||!pFtr->IsVisible() || !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
				continue;
			CString strcode(pFtr->GetCode());
			if (strcode.IsEmpty())
			{
				strcode = StrFromResID(IDS_LOCAL_MAPPING);
			}
			//添加选择好的关键点
 			for (int k=0;k<m_sMapName.GetSize();k++)
 			{
   				if (0==strcode.Compare((LPCTSTR)m_sMapName.GetAt(k)))//只添加选择图幅的地物
				{
					CGeoCurve *pGeoCurve = new CGeoCurve;
					if (!pGeoCurve) continue;
					arrPts.RemoveAll();
					pFtr->GetGeometry()->GetShape(arrPts);
					int nsize = arrPts.GetSize();
					if(nsize<4) 
					{
						delete pGeoCurve;
						pGeoCurve = NULL;
						continue;
					}
					for( int i=0; i<4; i++)
					{
						expts[i].z = 0;
						expts[i].pencode = penLine;
						COPY_2DPT(expts[i],arrPts[i]);
					}
					pGeoCurve->CreateShape(expts,4);//创建
						((CGeoCurve*)pGeoCurve)->EnableClose(TRUE);//闭合
					m_arrGeoCurve.Add(pGeoCurve);//添加到数组中
				}				
 			}
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 2;
		}
	}

	if( IsProcFinished(this) )return;

	//初次过滤地物
	CFtrArray arrFtrs;
	double lfJoinToler = m_fJoinToler;//pDS->GetScale()*m_fJoinToler/1000;
	for (i=0;i<pDS->GetFtrLayerCount();i++)
	{
		pFtrLayer = pDS->GetFtrLayerByIndex(i);
		if(pFtrLayer==NULL||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible()||strcmp(pFtrLayer->GetName(),m_strLayNameOfTKName)==0)
			continue;
		for (int j=0;j<pFtrLayer->GetObjectCount();j++)
		{
			pFtr = pFtrLayer->GetObject(j);
			if ( pFtr==NULL||!pFtr->IsVisible()||!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				continue;
			BOOL IsContinu =FALSE;
			CString strcode(pFtr->GetCode());
			if (strcode.IsEmpty())
			{
				strcode = StrFromResID(IDS_LOCAL_MAPPING);
			}
			for (int k = 0;k< m_arrSelectmap.GetSize();k++)
			{
				if (0 == strcode.Compare(m_arrSelectmap.GetAt(k)))//只添加选择图幅的地物
				{
					IsContinu = TRUE;
				}
			}
			if (IsContinu)
			{
				arrFtrs.Add(pFtr);//添加符合接边地物层
			}
		
		}
	}
	//进一步过滤地物,并根据接边线计算中间参数
	//遍历所有图幅的四个角点
	
	CArray<itemEdge*,itemEdge*> arrItems;
	CArray<itemNode,itemNode> arrNodes;

	CUndoBatchAction undo1(m_pEditor,Name());
	//接边处的线段插入交点，防止新悬挂的产生
//	InsertPt(arrFtrs, &undo1);
	//获取接边参数
	GetMatchItem(arrFtrs, arrItems, arrNodes);

	int nSum = arrItems.GetSize();
	GProgressStart(nSum*2);
	//配对
	MatchItem(arrItems);

	//接边(断开)
	for(i=0; i<arrItems.GetSize(); i++)
	{
		itemEdge *pItem = arrItems[i];
		itemEdge *pItem0 = pItem->pLinkItem;
		if(pItem0==NULL)
			continue;

		CFeature *pTemp = pItem->pFtr;
		CFeature *pTemp0 = NULL;
		CArray<PT_3DEX,PT_3DEX> arrTempPts;
		PT_3DEX ret;												
		CFeature *pTmp = pItem->pFtr;
		CFeature *pTmp1 = pItem0->pFtr;
		CString strcode (pTmp->GetCode());//获取标识码
		CString strcode1 (pTmp1->GetCode());
		CArray<PT_3DEX,PT_3DEX> arrTmpPts,arrTmpPts1;
		pTmp->GetGeometry()->GetShape(arrTmpPts);	
		pTmp1->GetGeometry()->GetShape(arrTmpPts1);
		if (0 == m_nModifyMode)//0 外部图和本地图幅都改动(自动修改) 取垂足中点
		{
		ret.x = (pItem->perpendicular.x+pItem0->perpendicular.x)/2;
		ret.y = (pItem->perpendicular.y+pItem0->perpendicular.y)/2;
		ret.z = (pItem->perpendicular.z+pItem0->perpendicular.z)/2;
		}
		else if (2 == m_nModifyMode)//2 修改外部图
		{
			if (!strcode1.IsEmpty()&&!strcode.IsEmpty()||(strcode1.IsEmpty()&&strcode.IsEmpty()))//都不是本地图幅 自动修改
			{
				ret.x = (pItem->perpendicular.x+pItem0->perpendicular.x)/2;
				ret.y = (pItem->perpendicular.y+pItem0->perpendicular.y)/2;
				ret.z = (pItem->perpendicular.z+pItem0->perpendicular.z)/2;
			}
			else
			{
				if (strcode.IsEmpty())//标识码为空 
				{
					ret.x = arrTmpPts[pItem->pos].x;			
					ret.y = arrTmpPts[pItem->pos].y;	
					ret.z = arrTmpPts[pItem->pos].z;	
				}
				if (strcode1.IsEmpty())//标识码为空 
				{
					ret.x = arrTmpPts1[pItem0->pos].x;			
					ret.y = arrTmpPts1[0].y;	
					ret.z = arrTmpPts1[0].z;	
				}	
			}
		}
		else if (1 == m_nModifyMode)// 1 修改本地图幅
		{
			if ((!strcode1.IsEmpty()&&!strcode.IsEmpty())||(strcode1.IsEmpty()&&strcode.IsEmpty()))//都是本地图幅 自动修改
			{
				ret.x = (pItem->perpendicular.x+pItem0->perpendicular.x)/2;
				ret.y = (pItem->perpendicular.y+pItem0->perpendicular.y)/2;
				ret.z = (pItem->perpendicular.z+pItem0->perpendicular.z)/2;
			}
			else
			{
				if (!strcode.IsEmpty())//标识码不为空 
				{
					ret.x = arrTmpPts[pItem->pos].x;			
					ret.y = arrTmpPts[pItem->pos].y;	
					ret.z = arrTmpPts[pItem->pos].z;	
				}
				if (!strcode1.IsEmpty())//标识码不为空 
				{
					ret.x = arrTmpPts1[pItem0->pos].x;			
					ret.y = arrTmpPts1[pItem0->pos].y;	
					ret.z = arrTmpPts1[pItem0->pos].z;	
				}	
			}
		}
		CreateNewObjBreak(pItem, pItem0, ret, &undo1);
	}

	SnapSurfaceNode(arrNodes, &undo1);

	undo1.Commit();
	GProgressEnd();
	for (i=0;i<arrItems.GetSize();i++)
	{
		delete arrItems[i];
	}
	for(i=0;i<m_arrGeoCurve.GetSize();i++)
	{
		delete m_arrGeoCurve[i];
	}
	arrItems.RemoveAll();
	m_arrGeoCurve.RemoveAll();
	Finish();		    
}

void CMapMatchCommand::SnapSurfaceNode(CArray<itemNode,itemNode>& arrNodes, CUndoBatchAction* pUndo)
{
	CUndoModifyProperties undo(m_pEditor,Name());

	//剔除重复
	int i, j;
	for(i=arrNodes.GetSize()-1; i>0; i--)
	{
		for(j=0; j<i; j++)
		{
			if(arrNodes[i]==arrNodes[j])
			{
				arrNodes.RemoveAt(i);
				break;
			}
		}
	}

	m_pEditor->GetDataSource()->SaveAllAppFlags();

	for(i=0; i<arrNodes.GetSize(); i++)
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		CFeature *pTemp = arrNodes[i].pSurface;
		if(pTemp->GetAppFlag()>0)
			continue;
		CGeometry *pGeo = pTemp->GetGeometry();
		pGeo->GetShape(arrPts);
		PT_3DEX pt1 = arrPts[arrNodes[i].pos1];

		CVariantEx varEx;
		varEx.SetAsShape(arrPts);
		CValueTable oldtab, tab;
		oldtab.BeginAddValueItem();
		oldtab.AddValue(FIELDNAME_SHAPE,&varEx);
		oldtab.EndAddValueItem();

		CGeometry *pGeo1 = arrNodes[i].pCurve->GetGeometry();
		PT_3DEX pt2 = pGeo1->GetDataPoint(arrNodes[i].pos2);
		if(GraphAPI::GGet2DDisOf2P(pt1, pt2) < GraphAPI::g_lfDisTolerance)
			continue;
		COPY_3DPT(arrPts[arrNodes[i].pos1], pt2);
		for(j=i+1; j<arrNodes.GetSize(); j++)
		{
			if(arrNodes[j].pSurface==pTemp)
			{
				CGeometry *pGeo2 = arrNodes[j].pCurve->GetGeometry();
				PT_3DEX pt3 = pGeo2->GetDataPoint(arrNodes[j].pos2);
				if(GraphAPI::GGet2DDisOf2P(pt1, pt3) < GraphAPI::g_lfDisTolerance)
					continue;
				COPY_3DPT(arrPts[arrNodes[j].pos1], pt3);
			}
		}

		m_pEditor->DeleteObject(FtrToHandle(pTemp));
		pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
		m_pEditor->RestoreObject(FtrToHandle(pTemp));
		
		varEx.SetAsShape(arrPts);	
		tab.BeginAddValueItem();
		tab.AddValue(FIELDNAME_SHAPE,&varEx);
		tab.EndAddValueItem();
		
		undo.SetModifyProperties(FtrToHandle(pTemp),oldtab,tab,TRUE);
		pTemp->SetAppFlag(1);
	}

	m_pEditor->GetDataSource()->RestoreAllAppFlags();
	pUndo->AddAction(&undo);
}

BOOL CMapMatchCommand::GetIntersectPt(PT_3DEX pt1, PT_3DEX pt2, PT_3DEX *ret1)
{
	if(!m_pEditor || ret1==NULL) return FALSE;
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDQ) return FALSE;

	Envelope e;
	e.m_xl = min(pt1.x, pt2.x);  e.m_xh = max(pt1.x, pt2.x);
	e.m_yl = min(pt1.y, pt2.y);  e.m_yh = max(pt1.y, pt2.y);

	pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);

	int num;
	const CPFeature *ftr = pDQ->GetFoundHandles(num);
	if(num<=0) return FALSE;

	bool bIntersect = false;
	double t = 1;

	for(int i=0; i<num; i++)
	{
		CGeometry *pGeo = ftr[i]->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
			continue;

		CArray<PT_3DEX,PT_3DEX> arrPts;

		pGeo->GetShape(arrPts);
		PT_3D ret;
		double t1, t2;

		for(int j=0; j<arrPts.GetSize()-1; j++)
		{
			if( arrPts[j].pencode == penMove) continue;

			//求相交点
			if (!GLineIntersectLineSeg1(pt1.x, pt1.y, pt2.x, pt2.y, arrPts[j].x, arrPts[j].y, arrPts[j].z,
				arrPts[j+1].x,arrPts[j+1].y,arrPts[j+1].z,&(ret.x),&(ret.y),&(ret.z),&t1,&t2))
			{
				continue;
			}

			if( fabs(t1)<1e-4 || fabs(1-t1)<1e-4 )
				continue;

			if(t1<t)
			{
				bIntersect = true;
				ret1->x = ret.x;
				ret1->y = ret.y;
				ret1->z = ret.z;
				t = t1;
			}
		}
	}
		
	return bIntersect;
}

void CMapMatchCommand::CreateNewObjBreak(itemEdge *pItem, itemEdge *pItem0,PT_3DEX& ret,CUndoBatchAction* pUndo)
{
	CUndoModifyProperties undo(m_pEditor,Name());

	CArray<PT_3DEX,PT_3DEX> arrTempPts;
	CFeature *pTemp = pItem->pFtr;
	pTemp->GetGeometry()->GetShape(arrTempPts);

	CVariantEx varEx;
	varEx.SetAsShape(arrTempPts);
	CValueTable oldtab, tab;
	oldtab.BeginAddValueItem();
	oldtab.AddValue(FIELDNAME_SHAPE,&varEx);
	oldtab.EndAddValueItem();
	
	ret.pencode = pItem->perpendicular.pencode;
	arrTempPts.SetAt(pItem->pos,ret);
	if(pItem->nClsType==CLS_GEOSURFACE && pItem->pos==0)
		arrTempPts.SetAt(arrTempPts.GetSize()-1,ret);//尾节点

	m_pEditor->DeleteObject(FtrToHandle(pTemp));
	pTemp->GetGeometry()->CreateShape(arrTempPts.GetData(),arrTempPts.GetSize());
	m_pEditor->RestoreObject(FtrToHandle(pTemp));

	varEx.SetAsShape(arrTempPts);	
	tab.BeginAddValueItem();
	tab.AddValue(FIELDNAME_SHAPE,&varEx);
	tab.EndAddValueItem();

	undo.SetModifyProperties(FtrToHandle(pTemp),oldtab,tab,TRUE);

	//第二个地物
	pTemp = pItem0->pFtr;
	pTemp->GetGeometry()->GetShape(arrTempPts);

	varEx.SetAsShape(arrTempPts);
	CValueTable oldtab1, tab1;
	oldtab1.BeginAddValueItem();
	oldtab1.AddValue(FIELDNAME_SHAPE,&varEx);
	oldtab1.EndAddValueItem();

	ret.pencode = pItem0->perpendicular.pencode;
	arrTempPts.SetAt(pItem0->pos,ret);
	if(pItem0->nClsType==CLS_GEOSURFACE && pItem0->pos==0)
		arrTempPts.SetAt(arrTempPts.GetSize()-1,ret);//尾节点

	m_pEditor->DeleteObject(FtrToHandle(pTemp));
	pTemp->GetGeometry()->CreateShape(arrTempPts.GetData(),arrTempPts.GetSize());
	m_pEditor->RestoreObject(FtrToHandle(pTemp));
	
	varEx.SetAsShape(arrTempPts);	
	tab1.BeginAddValueItem();
	tab1.AddValue(FIELDNAME_SHAPE,&varEx);
	tab1.EndAddValueItem();
	
	undo.SetModifyProperties(FtrToHandle(pTemp),oldtab1,tab1,TRUE);

	pUndo->AddAction(&undo);
}

void CMapMatchCommand::InsertPt(CFtrArray& arrFtrs,CUndoBatchAction *pUndo)
{
	int i,j,m, k;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	PT_3DEX start,end;
	double lfXr,lfYr,lfTr;
	CFeature *pFtr = NULL;
	double lfJoinToler = m_fJoinToler;

	CUndoFtrs undo(m_pEditor,Name());

	for (m =0;m<m_arrGeoCurve.GetSize();m++)
	{		
		CArray<PT_3DEX,PT_3DEX> arrEdgePts;
		m_arrGeoCurve.GetAt(m)->GetShape(arrEdgePts);//获取作业区的角上四个关键点

		for (i=0;i<arrEdgePts.GetSize()-1;i++)
		{
			start = arrEdgePts[i];
			end = arrEdgePts[i+1];
			BOOL bChanged = FALSE;
			for (int j=arrFtrs.GetSize()-1;j>=0;j--)
			{
				pFtr = arrFtrs.GetAt(j);//获取特征地物
				CGeometry *pGeo = pFtr->GetGeometry();
				if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
					continue;

				pGeo->GetShape(arrPts);//获取特征地物关键点
				if( ((CGeoCurveBase*)pGeo)->IsClosed() )
				{
					//尾点
					PT_3DEX tempPT1 = arrPts[arrPts.GetSize()-1];
					PT_3DEX tempPT2 = arrPts[arrPts.GetSize()-2];
					if(GraphAPI::GIsPtInLine(start, end, tempPT1))
					{
						if(!GraphAPI::GIsPtInLine(start, end, tempPT2))
						{
							PT_3DEX ret;
							BOOL bAddPt = GetIntersectPt(tempPT1,tempPT2, &ret);
							ret.pencode = penLine;
							if(bAddPt)
							{
								arrPts.InsertAt(arrPts.GetSize()-1, ret);
								bChanged = TRUE;
							}
						}
					}

					//中间点
					for(int k=arrPts.GetSize()-2; k>0; k--)
					{
						tempPT1 = arrPts[k];
						if(GraphAPI::GIsPtInLine(start, end, tempPT1))
						{
							int pos = -1;
							
							if(!GraphAPI::GIsPtInLine(start, end, arrPts[k-1]))
								pos = k-1;
							else
								pos = k+1;
							
							tempPT2 = arrPts.GetAt(pos);
							PT_3DEX ret;
							BOOL bAddPt = GetIntersectPt(tempPT1,tempPT2, &ret);
							ret.pencode = penLine;
							if(bAddPt && pos>=0)
							{
								arrPts.InsertAt(max(k,pos), ret);
								bChanged = TRUE;
							}
						}
					}

					//首点
					tempPT1 = arrPts[0];
					if(GraphAPI::GIsPtInLine(start, end, tempPT1))
					{
						if(!GraphAPI::GIsPtInLine(start, end, arrPts[1]))
						{
							PT_3DEX ret;
							BOOL bAddPt = GetIntersectPt(tempPT1,arrPts[1], &ret);
							ret.pencode = penLine;
							if(bAddPt)
							{
								arrPts.InsertAt(1, ret);
								bChanged = TRUE;
							}
						}
					}
				}
				else
				{
					PT_3DEX tempPT1 = arrPts.GetAt(arrPts.GetSize()-1);	//最后面的一个关键点
					PT_3DEX tempPT2 = arrPts.GetAt(arrPts.GetSize()-2);
					if (GetNearestDisOfPtToLine(start.x,start.y,end.x,end.y,tempPT1.x,tempPT1.y,&lfXr,&lfYr,&lfTr,NULL,lfJoinToler/2))
					{
						PT_3DEX ret;
						BOOL bAddPt = GetIntersectPt(tempPT1,tempPT2, &ret);	
						ret.pencode = penLine;
						if(bAddPt)
						{
							arrPts.InsertAt(arrPts.GetSize()-1, ret);
							bChanged = TRUE;
						}
					}
					
					tempPT1 = arrPts[0];	//地物的 第一个关键点
					tempPT2 = arrPts[1];
					if (GetNearestDisOfPtToLine(start.x,start.y,end.x,end.y,tempPT1.x,tempPT1.y,&lfXr,&lfYr,&lfTr,NULL,lfJoinToler/2))
					{
						PT_3DEX ret;
						BOOL bAddPt = GetIntersectPt(tempPT1,tempPT2, &ret);
						ret.pencode = penLine;
						if(bAddPt)
						{
							arrPts.InsertAt(1, ret);
							bChanged = TRUE;
						}
					}
				}

				if(bChanged)
				{
					CFeature *pNew = pFtr->Clone();
					pNew->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize());
					long oldId = PDOC(m_pEditor)->GetFtrLayerIDOfFtr(FtrToHandle(pFtr));
					m_pEditor->AddObject(pNew, oldId);
					GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNew);						
					undo.AddNewFeature(FtrToHandle(pNew));	
					m_pEditor->DeleteObject(FtrToHandle(pFtr));
					undo.AddOldFeature(FtrToHandle(pFtr));
					arrFtrs.SetAt(j, pNew);
				}
			}
		}
	}

	pUndo->AddAction(&undo);
}

void CMapMatchCommand::GetMatchItem(CFtrArray& arrFtrs, CArray<itemEdge*,itemEdge*>& arrItems, CArray<itemNode,itemNode>& arrNodes)
{
	int i,j,m, k;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	PT_3DEX start,end;
	double lfXr,lfYr,lfTr;
	CFeature *pFtr = NULL;
	double lfJoinToler = m_fJoinToler;
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();

	for (m =0;m<m_arrGeoCurve.GetSize();m++)
	{		
		CArray<PT_3DEX,PT_3DEX> arrEdgePts;
		m_arrGeoCurve.GetAt(m)->GetShape(arrEdgePts);//获取作业区的角上四个关键点
		Envelope e = m_arrGeoCurve.GetAt(m)->GetEnvelope();

		for (i=0;i<arrEdgePts.GetSize()-1;i++)
		{
			start = arrEdgePts[i];
			end = arrEdgePts[i+1];
			for (int j=arrFtrs.GetSize()-1;j>=0;j--)
			{
				pFtr = arrFtrs.GetAt(j);//获取特征地物
				CGeometry *pGeo = pFtr->GetGeometry();
				if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
					continue;

				BOOL bClosed = ((CGeoCurveBase*)pGeo)->IsClosed();
				Envelope e1 = pGeo->GetEnvelope();
				PT_3D pt(e1.CenterX(), e1.CenterY(), 0);
				if(!e.bPtIn(&pt)) continue;//不在此图幅框内

				pGeo->GetShape(arrPts);//获取特征地物关键点
				if( bClosed )
				{
					for(int k=0; k<arrPts.GetSize(); k++)
					{
						PT_3DEX tempPT = arrPts[k];
						if(!GraphAPI::GIsPtInLine(start, end, tempPT))
							continue;
						Envelope e2;
						e2.CreateFromPtAndRadius(tempPT, GraphAPI::g_lfDisTolerance);
						pDQ->FindObjectInRect(e2,NULL,FALSE,FALSE);
						int num;
						const CPFeature *ftr = pDQ->GetFoundHandles(num);
						for(int n=0; n<num; n++)
						{
							if(ftr[n]==pFtr) continue;
							CGeometry *pGeo1 = ftr[n]->GetGeometry();
							if(!pGeo1 || !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
								continue;
							if( ((CGeoCurveBase*)pGeo1)->IsClosed() )
								continue;
							PT_KEYCTRL nearestPt = pGeo1->FindNearestKeyCtrlPt(tempPT, GraphAPI::g_lfDisTolerance, NULL, 1);
							if( nearestPt.IsValid())
							{
								itemNode item;
								item.pSurface = pFtr;
								item.pos1 = k;
								item.pCurve = ftr[n];
								item.pos2 = nearestPt.index;
								arrNodes.Add(item);
							}
						}
					}
				}
				else
				{
					PT_3DEX tempPT = arrPts[0];	//地物的 第一个关键点
					//if (GetNearestDisOfPtToLine(start.x,start.y,end.x,end.y,tempPT.x,tempPT.y,&lfXr,&lfYr,&lfTr,NULL,lfJoinToler/2))
					if(GraphAPI::GIsPtInLine(start, end, tempPT))
					{
						itemEdge *pItem = new itemEdge;
						if(pItem)
						{
							pItem->bVisit = FALSE;
							pItem->pLinkItem = NULL;
							pItem->pos = 0;
							pItem->idx = i;//索引
							//pItem->perpendicular = PT_3DEX(lfXr,lfYr,tempPT.z,tempPT.pencode);//垂足
							pItem->perpendicular = tempPT;
							pItem->ptex = tempPT;
							pItem->nClsType = CLS_GEOCURVE;					
							pItem->pFtr = pFtr;
							pItem->dis = 9999;
							arrItems.Add(pItem);
						}				
					}
					tempPT = arrPts.GetAt(arrPts.GetSize()-1);	//最后面的一个关键点
					//if (GetNearestDisOfPtToLine(start.x,start.y,end.x,end.y,tempPT.x,tempPT.y,&lfXr,&lfYr,&lfTr,NULL,lfJoinToler/2))
					if(GraphAPI::GIsPtInLine(start, end, tempPT))
					{
						itemEdge *pItem = new itemEdge ;
						if (pItem)
						{
							pItem->bVisit = FALSE;
							pItem->pLinkItem = NULL;
							pItem->pos = arrPts.GetSize()-1;
							pItem->idx = i;
							//pItem->perpendicular = PT_3DEX(lfXr,lfYr,tempPT.z,tempPT.pencode);
							pItem->perpendicular = tempPT;
							pItem->ptex = tempPT;
							pItem->nClsType = CLS_GEOCURVE;
							pItem->pFtr = pFtr;
							pItem->dis = 9999;
							arrItems.Add(pItem);
						}				
					}
				}
			}
		}
	}
}

void CMapMatchCommand::MatchItem(CArray<itemEdge*,itemEdge*>& arrItems)
{
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return; 
	double lfJoinToler = m_fJoinToler;
	const double c_minToler = 1e-6;	
	int i, j;
	for (i=0;i<arrItems.GetSize();i++)
	{
		GProgressStep();
		itemEdge *pItem = arrItems[i];
		if(pItem->bVisit) continue;

		double mindis = 9999;
		itemEdge *pItem0 = NULL;
		for (j=i+1;j<arrItems.GetSize();j++)
		{
			itemEdge *pTemp = arrItems[j];
			if(pTemp->bVisit)
				continue;
			//if( pTemp->idx == pItem->idx )//同一个图幅内不接边
				//continue;
			if(pTemp->nClsType != pItem->nClsType)//几何体类型不一致，不接边
				continue;
			if( pTemp->pFtr == pItem->pFtr )//同一个地物
				continue;
			//是否属于同一层
			CFtrLayer* pLayer0 = pDS->GetFtrLayerOfObject(pTemp->pFtr);
			CFtrLayer* pLayer1 = pDS->GetFtrLayerOfObject(pItem->pFtr);
			if (strcmp(pLayer0->GetName(),pLayer1->GetName())!=0)//匹配层名
			{
				continue;
			}

			double lfTemp = GraphAPI::GGetDisOf2P2D(pItem->ptex.x,pItem->ptex.y,pTemp->ptex.x,pTemp->ptex.y);//求两点平面距离
			if (lfTemp>lfJoinToler)//大于容差范围
				continue;
			if( fabs(pItem->ptex.z-pTemp->ptex.z)>m_lfZToler )//高程差
				continue;
// 			if (lfTemp<c_minToler&&m_nJoinMode==0)//接边处断开
// 			{
// 				pItem->bVisit = TRUE;
// 				pTemp->bVisit = TRUE;
// 				continue;
// 			}
			
			if (m_bMatchColor)//匹配颜色
			{
				if( pItem->pFtr->GetGeometry()->GetColor() != pTemp->pFtr->GetGeometry()->GetColor() )
				{
					continue;
				}
			}
			if (m_bMatchLineType)//匹配线型
			{
				if (pItem->ptex.pencode!=pTemp->ptex.pencode)
				{
					continue;
				}
			}
// 			if (m_bMatchLineWidth)
// 			{
// 				if ((*(item.ppFtr))->GetGeometry()->GetColor()!=(*(pTemp.ppFtr))->GetGeometry()->GetColor())
// 				{
// 					continue;
// 				}
// 			}
			if (m_bMatchLineHei)//比较线高程值
			{
				if (fabs(pItem->ptex.z-pTemp->ptex.z)>=1e-4)
				{
					continue;
				}
			}
			
// 			if (m_arrSelectmap.GetSize()>0)//有选择的图幅
// 			{
// 				BOOL IsContiue = TRUE;	
// 				for (int i=0;i<m_arrSelectmap.GetSize();i++)
// 				{
// 					CString tstr = m_arrSelectmap.GetAt(i);
// 					if (0==strcmp(m_arrSelectmap.GetAt(i),(*(pItem->ppFtr))->GetCode())||0==strcmp(m_arrSelectmap.GetAt(i),(*(pTemp->ppFtr))->GetCode())) //Check here
// 					{
// 						IsContiue = FALSE;		
// 					}
// 				}
// 				if (IsContiue)
// 				{
// 					continue;
// 				}
// 			}
			if(lfTemp<mindis)
			{
				pItem0 = pTemp;
				mindis = lfTemp;
			}
		}
		if(pItem0)
		{
			pItem->pLinkItem = pItem0;
			pItem->dis = mindis;
			pItem0->bVisit = TRUE;
		}
	}
}

void CMapMatchCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = float(m_fJoinToler);
	tab.AddValue(PF_MATCH_TOLER,&CVariantEx(var));
	var = float(m_lfZToler);
	tab.AddValue("ZToler",&CVariantEx(var));
// 	var = (long)(m_nScopeType);
// 	tab.AddValue(PF_MAPMATCH_SCOPE,&CVariantEx(var));
	var = (long)(m_nModifyMode);
	tab.AddValue(PF_MODIFY_MODE,&CVariantEx(var));
// 	var = (long)(m_nJoinMode);
// 	tab.AddValue(PF_JOIN_MODE,&CVariantEx(var));
	var = (bool)(m_bMatchColor);
	tab.AddValue(PF_MATCH_COLOR,&CVariantEx(var));
	var = (bool)(m_bMatchLineType);
	tab.AddValue(PF_MATCH_LINETYPE,&CVariantEx(var));
// 	var = (bool)(m_bMatchLineWidth);
// 	tab.AddValue(PF_MATCH_LINEWIDTH,&CVariantEx(var));
	var = (bool)(m_bMatchLineHei);
	tab.AddValue(PF_MATCH_LINEHEI,&CVariantEx(var));
	var = (bool)(m_bMark);
	tab.AddValue(PF_MATCH_MARK,&CVariantEx(var));
	var = (_bstr_t)(LPCSTR)(m_sSelectMap);
	tab.AddValue(PF_MATCH_MAPSELECT,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strLayNameOfTKName;
	tab.AddValue(PF_LAYNAME_BOUNDNAME,&CVariantEx(var));
}

static void InsertStringintoArray(CStringArray& names, LPCTSTR name)
{
	if( strlen(name)==0 )
		return;
	
	int nsz = names.GetSize();
	CString *buf = names.GetData();
	for( int i=0; i<nsz; i++)
	{
		if( buf[i].CompareNoCase(name)==0 )
			break;
	}
	if( i<nsz )
	{
		if( i!=0 )
		{
			names.RemoveAt(i);
			names.InsertAt(0,name);
		}
	}
	else
	{
		names.InsertAt(0,name);
	}
}

void CMapMatchCommand::GetFtrCodes(CStringArray &names)
{
	names.RemoveAll();
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( !pDoc )
		return;	
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	for( int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrlayer = pDS->GetFtrLayerByIndex(i);
		if( !pFtrlayer )continue;
		if( pFtrlayer->IsEmpty() || pFtrlayer->IsDeleted() || !pFtrlayer->IsVisible() )
			continue;
		
		for( int j=0; j<pFtrlayer->GetObjectCount(); j++)
		{
			CFeature *pFtr = pFtrlayer->GetObject(j);
			if (pFtr==NULL||!pFtr->IsVisible())
				continue;
			if (0==(strcmp(pFtr->GetCode(),"")))
			{
				InsertStringintoArray(names,StrFromResID(IDS_LOCAL_MAPPING));
			}
			else
			{
				InsertStringintoArray(names,pFtr->GetCode());
			}
		}
	}
}

BOOL CMapMatchCommand::UpdateMark(BOOL bMark)
{
	if (!m_pEditor)
	{
		return FALSE;
	}
	if (!bMark)
	{
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_DelGraph);	
		return TRUE;
	}
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	CFtrLayer* pFtrLayer;
	CFeature *pFtr;
	PT_3DEX expts[4];
	GetFtrCodes(m_sMapName);
	GrBuffer m_buf;
	for (int i=0;i<pDS->GetFtrLayerCount();i++)
	{
		pFtrLayer = pDS->GetFtrLayerByIndex(i);
		if(pFtrLayer==NULL||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible()||strcmp(pFtrLayer->GetName(),m_strLayNameOfTKName)!=0)
			continue;
		for (int j=0;j<pFtrLayer->GetObjectCount();j++)//同一个层中多少地物
		{
			pFtr = pFtrLayer->GetObject(j);
			if (pFtr==NULL||!pFtr->IsVisible())
				continue;
			CString strcode(pFtr->GetCode());
			if (strcode.IsEmpty())
			{
				strcode = StrFromResID(IDS_LOCAL_MAPPING);
			}
			for (int k=0;k<m_sMapName.GetSize();k++)
			{
				if (0==strcode.Compare((LPCTSTR)m_sMapName.GetAt(k)))//只添加选择图幅的地物
				{
					arrPts.RemoveAll();
					pFtr->GetGeometry()->GetShape(arrPts);
					if (arrPts.GetSize()<4)
						continue;
					for( int i=0; i<4; i++)
					{
						expts[i].z = 0;
						expts[i].pencode = penLine;
						COPY_2DPT(expts[i],arrPts[i]);
					}
					PT_3D pt_Text;
					pt_Text.x = (expts[0].x+expts[2].x)/2;
					pt_Text.y = (expts[0].y+expts[2].y)/2;
					pt_Text.z = (expts[0].z+expts[2].z)/2;
					Envelope e;
					TextSettings settings;
					CDlgDoc *pDoc = (CDlgDoc *)(CEditor*)m_pEditor;
					settings.fHeight = pDoc->GetDlgDataSource()->GetScale()*50*0.001;
					settings.nAlignment = TAH_MID|TAV_MID;
					e.CreateFromPts(expts,4);
					PT_3DEX expt(e.CenterX(),e.CenterY(),0,penLine);
					m_buf.Text(RGB(255,0,0),&pt_Text,strcode,&settings);
					PDOC(m_pEditor)->UpdateAllViews(NULL,hc_AddGraph,(CObject*)&m_buf);		
				}
			}
		}
	}
	return TRUE;
}

void CMapMatchCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("MapMatchCommand",StrFromLocalResID(IDS_CMDNAME_MAPAUTOMATCH));
	param->AddParam(PF_MATCH_TOLER,m_fJoinToler,StrFromResID(IDS_CMDPLANE_MM_MATCHTOLER),StrFromResID(IDS_CMDPLANE_MM_TOLERDES));
	param->AddParam("ZToler",m_lfZToler,StrFromResID(IDS_TOLER_Z),StrFromResID(IDS_CMDPLANE_MM_TOLERDES));
// 	param->BeginOptionParam(PF_MAPMATCH_SCOPE,StrFromResID(IDS_CMDPLANE_MM_SCOPE));
// 	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_ACTIVEDS),0,' ',m_nScopeType==0);
// 	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_ALLDS),1,' ',m_nScopeType==1);	
// 	param->EndOptionParam();

	param->AddUsedLayerNameParam(PF_LAYNAME_BOUNDNAME,(LPCTSTR)m_strLayNameOfTKName,StrFromResID(IDS_LAYNAME_BOUNDNAME));	
	param->BeginOptionParam(PF_MODIFY_MODE,StrFromResID(IDS_CMDPLANE_MM_MODIFYMODE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_MODIFYALL),0,' ',m_nModifyMode==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_MODIFYTHIS),1,' ',m_nModifyMode==1);
	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_MODIFYOTHERS),2,' ',m_nModifyMode==2);
	param->EndOptionParam();
// 	param->BeginOptionParam(PF_JOIN_MODE,StrFromResID(IDS_CMDPLANE_MM_JOINMODE));
// 	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_BREAK),0,' ',m_nJoinMode==0);
// 	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_JOIN),1,' ',m_nJoinMode==1);
// 	param->EndOptionParam();

	param->BeginOptionParam(PF_MATCH_COLOR,StrFromResID(IDS_CMDPLANE_MM_MATCHCOLOR));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bMatchColor);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bMatchColor);
	param->EndOptionParam();
	param->BeginOptionParam(PF_MATCH_LINETYPE,StrFromResID(IDS_CMDPLANE_MM_MATCHLINETYPE));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bMatchLineType);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bMatchLineType);
	param->EndOptionParam();
// 	param->BeginOptionParam(PF_MATCH_LINEWIDTH,StrFromResID(IDS_CMDPLANE_MM_MATCHLINEWIDTH));
// 	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bMatchLineWidth);
// 	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bMatchLineWidth);
// 	param->EndOptionParam();
	param->BeginOptionParam(PF_MATCH_LINEHEI,StrFromResID(IDS_CMDPLANE_MM_MATCHLINEHEI));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bMatchLineHei);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bMatchLineHei);
	param->EndOptionParam();

	//标示图幅
	param->AddParam(PF_MATCH_MARK,bool(m_bMark),StrFromResID(IDS_MAPMATCH_MARK));
	if (!m_bMark)
	{	
		UpdateMark(FALSE);	
	}
	else
	{
		UpdateMark(TRUE);	
	}	
	param->BeginCheckListParam(PF_MATCH_MAPSELECT,StrFromResID(IDS_CMDPLANE_MM_MATCHMAPSELECT));
	GetFtrCodes(m_sMapName);//获取所有的标识码
	convertStringToStrArray(m_sSelectMap, m_arrSelectmap);
	for (int i = 0 ;i<m_sMapName.GetSize();i++)
	{
		BOOL value = FALSE;
		for (int j =0;j<m_arrSelectmap.GetSize();j++)
		{
			if (0 == m_sMapName.GetAt(i).Compare(m_arrSelectmap.GetAt(j)))
			{
				value = TRUE;
			}		
		}			
		param->AddCheckItem((LPCTSTR)m_sMapName.GetAt(i),value);
	}
	param->EndCheckListParam();
}


void CMapMatchCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_MATCH_TOLER,var) )
	{
		m_fJoinToler = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"ZToler",var) )
	{
		m_lfZToler = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
// 	if( tab.GetValue(0,PF_MAPMATCH_SCOPE,var) )
// 	{					
// 		m_nScopeType = (long)(_variant_t)*var;
// 		SetSettingsModifyFlag();		
// 	}
	if( tab.GetValue(0,PF_MODIFY_MODE,var) )
	{
		m_nModifyMode = (long)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
// 	if( tab.GetValue(0,PF_JOIN_MODE,var) )
// 	{
// 		m_nJoinMode = (long)(_variant_t)*var;	
// 		SetSettingsModifyFlag();
// 	}
	if( tab.GetValue(0,PF_MATCH_COLOR,var) )
	{
		m_bMatchColor = (bool)(_variant_t)*var;		
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_MATCH_LINETYPE,var) )
	{
		m_bMatchLineType = (bool)(_variant_t)*var;		
		SetSettingsModifyFlag();
	}
// 	if( tab.GetValue(0,PF_MATCH_LINEWIDTH,var) )
// 	{
// 		m_bMatchLineWidth = (bool)(_variant_t)*var;		
// 		SetSettingsModifyFlag();
// 	}
	if( tab.GetValue(0,PF_MATCH_LINEHEI,var) )
	{
		m_bMatchLineHei = (bool)(_variant_t)*var;		
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0,PF_MATCH_MAPSELECT,var))
	{
		m_sSelectMap  = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_MATCH_MARK,var) )
	{
		m_bMark = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0,PF_LAYNAME_BOUNDNAME,var))
	{
		m_strLayNameOfTKName  = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}



IMPLEMENT_DYNAMIC(CDlgDrawTestPointCommand,CDlgDrawPointCommand)

CDlgDrawTestPointCommand::CDlgDrawTestPointCommand()
{	
	m_pSel = NULL;
}

CDlgDrawTestPointCommand::~CDlgDrawTestPointCommand()
{
	if(m_pSel) delete m_pSel;
}


void CDlgDrawTestPointCommand::Start()
{
	if(!PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(_T("CheckPointLayer")))
	{
		CString temp;
		temp.Format(_T("%s %s"),_T("CheckPointLayer"),StrFromResID(IDS_NOTFINDLAY));
		GOutPut(temp);
		return;
	}
	CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(_T("TestPointLayer"));
	if (!pLayer)
	{
		 pLayer = PDOC(m_pEditor)->GetDlgDataSource()->CreateFtrLayer(_T("TestPointLayer"));
		 if (!pLayer)
		 {
			 return;
		 }
		 PDOC(m_pEditor)->GetDlgDataSource()->AddFtrLayer(pLayer);
	}
	
    PDOC(m_pEditor)->GetDlgDataSource()->SetCurFtrLayer(pLayer->GetID());	
	m_pFtr = pLayer->CreateDefaultFeature(GETCURSCALE(m_pEditor),CLS_GEOPOINT);
	if(!m_pFtr) return;	
	
	m_pDrawProc = new CDrawPointProcedure;
	if( !m_pDrawProc )return;
	
	m_pDrawProc->Init(m_pEditor);
	m_pDrawProc->m_pGeoPoint = (CGeoPoint*)m_pFtr->GetGeometry();	

	m_pDrawProc->Start();	
	
	CDrawCommand::Start();
	CTestVectorBar *pBar;
	AfxGetMainWnd()->SendMessage(FCCM_GETDRAWTESTPTBAR,0,(LPARAM)&pBar);
	pBar->RefreshActiveItem();	
	m_pSel = new CPermanentExchanger(m_pFtr);
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)m_pSel);
}


void CDlgDrawTestPointCommand::Finish()
{	
	if(m_pSel) 
	{
		delete m_pSel;
		m_pSel = NULL;
	}
	m_pFtr = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CDrawPointCommand::Finish();	
}


CString CDlgDrawTestPointCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DRAWTESTPT);
}

void CDlgDrawTestPointCommand::OnSonEnd(CProcedure *son)
{
	CTestVectorBar *pBar;
	AfxGetMainWnd()->SendMessage(FCCM_GETDRAWTESTPTBAR,0,(LPARAM)&pBar);
	CString strID;
	if(!pBar->GetFeatureId(strID))
	{
		GOutPut(StrFromResID(IDS_NOACTIVEPT));
		Abort();
		return;
	}
	ASSERT( m_pDrawProc==son );
	if( IsProcAborted(m_pDrawProc) || !m_pDrawProc->m_pGeoPoint )
		Abort();
	else
	{				
		if( m_pFtr )
		{
			m_pFtr->SetCode(strID);
			if( !AddObject(m_pFtr)/*m_pEditor->AddObject(m_pFtr)*/ )
			{
				// 				delete m_pFtr;
				// 				m_pFtr = NULL;
				Abort();
				return;
			}
			CUndoFtrs undo(m_pEditor,Name());
			if (pBar->IsFinishCurActiveItem())
			{
				CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer(_T("TestPointLayer"));
				int nNum = pLayer->GetObjectCount();
				CFeature *pFtr0 = NULL;
				BOOL Sucess = FALSE;
				for (int i=0;i<nNum;i++)
				{
					pFtr0 = pLayer->GetObject(i);
					if(!pFtr0||!pFtr0->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))continue;
					if (strID.CompareNoCase(pFtr0->GetCode())==0)
					{
						Sucess = TRUE;
						break;
					}
				}
				if (Sucess)
				{
					if(PDOC(m_pEditor)->DeleteObject(FtrToHandle(pFtr0)))
					{
						undo.AddOldFeature(FtrToHandle(pFtr0));
					}
				}
				else
				{
					GOutPut(StrFromResID(IDS_NEEDREFRESH));	
				}
			}		
			undo.AddNewFeature(FtrToHandle(m_pFtr));
			undo.Commit();
			PT_3DEX temp = m_pFtr->GetGeometry()->GetDataPoint(0);
			pBar->SetCurActiveItem(temp.x,temp.y,temp.z);
			Finish();
		}
		else
			Abort();
	}	
}


// CFixAttriToExtAttriCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CFixAttriToExtAttriCommand,CEditCommand)

CFixAttriToExtAttriCommand::CFixAttriToExtAttriCommand()
{
	m_nStep = -1;
	m_strLayerCodes = _T("");
	m_nFieldOption = dirPntDir;
	m_strFieldName = _T("");
	strcat(m_strRegPath,"\\FixAttriToExtAttri");

	m_bNotCoverValidValue = TRUE;
}

CFixAttriToExtAttriCommand::~CFixAttriToExtAttriCommand()
{
}

void CFixAttriToExtAttriCommand::Abort()
{
	CEditCommand::Abort();
}


CString CFixAttriToExtAttriCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_FIXATTRTOEXTATTR);
}

void CFixAttriToExtAttriCommand::Start()
{
	if( !m_pEditor )return;
	m_strLayerCodes = _T("");
	m_nFieldOption = dirPntDir;
	m_strFieldName = _T("");
	
	CEditCommand::Start();
	m_pEditor->CloseSelector();
}


extern CString VarToString(_variant_t& var);

BOOL CFixAttriToExtAttriCommand::CheckFieldValid(CValueTable& tab, LPCTSTR field)
{
	const CVariantEx *pvar = NULL;
	if(!tab.GetValue(0,field,pvar))
		return FALSE;

	_variant_t var = *pvar;
	CString text = VarToString(var);
	if( text.IsEmpty() || text.CompareNoCase("0")==0 || text.CompareNoCase("0.0000")==0)
		return FALSE;

	return TRUE;
}


void CFixAttriToExtAttriCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		if(!pDS)return;
		CConfigLibManager *pConfi = gpCfgLibMan;
		CScheme *pScheme = pConfi->GetScheme(pDS->GetScale());
		if(!pScheme)return;

		CFtrLayer *pLayer = NULL;
		CFeature *pFtr = NULL;
		CFtrLayerArray arrFtrlayer;

		int i;
		int nSum = 0;

		pDS->GetFtrLayersByNameOrCode_editable(m_strLayerCodes,arrFtrlayer);

		for (int j=0;j<arrFtrlayer.GetSize();j++)
		{
			pLayer = (CFtrLayer*)arrFtrlayer[j];
			if(!pLayer)continue;
			//由用户指定的字段名找出数据源内部的字段名
			CString field;
			CSchemeLayerDefine *pLayerDef = pScheme->GetLayerDefine(pLayer->GetName());
			if(!pLayerDef)continue;
			int idx;
			if((idx=pLayerDef->FindXDefineByName(m_strFieldName))==-1)
			{
				if(pLayerDef->FindXDefine(m_strFieldName)==-1)
					continue;
				else
					field = m_strFieldName;
			}
			else
			{
				field = pLayerDef->GetXDefine(idx).field;					
			}
			for (int k=0;k<pLayer->GetObjectCount();k++)
			{
				pFtr = pLayer->GetObject(k);
				if(!pFtr)continue;
				nSum++;
			}
		}
	
		
		CValueTable tab,oldtab;
		_variant_t var;
		GProgressStart(nSum);
		CUndoModifyProperties undo(m_pEditor,Name());

		for (j=0;j<arrFtrlayer.GetSize();j++)
		{
			pLayer = (CFtrLayer*)arrFtrlayer[j];
			if(!pLayer || pLayer->GetObjectCount()<=0)continue;
			//由用户指定的字段名找出数据源内部的字段名
			CString field;
			CSchemeLayerDefine *pLayerDef = pScheme->GetLayerDefine(pLayer->GetName());
			if(!pLayerDef)continue;
			int idx = pLayerDef->FindXDefineByName(m_strFieldName);
			if( idx==-1 )
			{
				idx = pLayerDef->FindXDefine(m_strFieldName);
				if(idx==-1)
					continue;
				else
					field = m_strFieldName;
			}
			else
			{
				field = pLayerDef->GetXDefine(idx).field;
			}
			int nDigit = pLayerDef->GetXDefine(idx).nDigit;
			if(nDigit<=0)
			{
				nDigit=6;
			}
			for (int k=0;k<pLayer->GetObjectCount();k++)
			{
				pFtr = pLayer->GetObject(k);
				if(!pFtr)continue;

				CGeometry *pGeo = pFtr->GetGeometry();

				GProgressStep();
				tab.DelAll();
				oldtab.DelAll();
				//严格来说，下面的转换是需要考虑FDB字段值类型的问题的，但FDB保存的时候，自动作了类型的自动转换，
				//所以全部按照字符串方式写入，也没有出现问题
				if(m_nFieldOption==dirPntDir)
				{
					if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)))
					{		
						double angle = ((CGeoDirPoint*)pGeo)->GetDirection();

						CString S, f;
						f.Format("%%.%dlf", nDigit);
						S.Format(f,angle);
						var = (_bstr_t)(LPCTSTR)S;

						//var = angle;								
						tab.BeginAddValueItem();
						tab.AddValue(field,&CVariantEx(var));
						tab.EndAddValueItem();
						oldtab.BeginAddValueItem();
						pDS->GetXAttributesSource()->GetXAttributes(pFtr,oldtab);
						oldtab.EndAddValueItem();
						pDS->GetXAttributesSource()->SetXAttributes(pFtr,tab);
						undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,FALSE);								
					}
				}
				else if (m_nFieldOption==parrelWidth)
				{
					if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
					{		
						double width0 = ((CGeoParallel*)pGeo)->GetWidth();

						CString S, f;
						f.Format("%%.%dlf", nDigit);
						S.Format(f,width0);
						var = (_bstr_t)(LPCTSTR)S;

						//var = width0;
						tab.BeginAddValueItem();
						tab.AddValue(field,&CVariantEx(var));
						tab.EndAddValueItem();
						oldtab.BeginAddValueItem();
						pDS->GetXAttributesSource()->GetXAttributes(pFtr,oldtab);
						oldtab.EndAddValueItem();

						if(m_bNotCoverValidValue && CheckFieldValid(oldtab,field))
							continue;

						pDS->GetXAttributesSource()->SetXAttributes(pFtr,tab);
						undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,FALSE);
					}
				}
				else if (m_nFieldOption==layerCode)
				{
					__int64 code = pLayerDef->GetLayerCode();
					CString laycode;
					laycode.Format(_T("%I64d"),code);
					var = (_bstr_t)(LPCTSTR)laycode;
					tab.BeginAddValueItem();
					tab.AddValue(field,&CVariantEx(var));
					tab.EndAddValueItem();
					oldtab.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr,oldtab);
					oldtab.EndAddValueItem();

					if(m_bNotCoverValidValue && CheckFieldValid(oldtab,field))
						continue;

					pDS->GetXAttributesSource()->SetXAttributes(pFtr,tab);
					undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,FALSE);
				}
				else if(m_nFieldOption==elevation)
				{
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeo->GetShape(arrPts);//必定至少含有一个点
					CString Z, f;
					f.Format("%%.%dlf", nDigit);
					Z.Format(f,arrPts[0].z);
					var = (_bstr_t)(LPCTSTR)Z;
					tab.BeginAddValueItem();
					tab.AddValue(field,&CVariantEx(var));
					tab.EndAddValueItem();
					oldtab.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr,oldtab);
					oldtab.EndAddValueItem();

					if(m_bNotCoverValidValue && CheckFieldValid(oldtab,field))
						continue;

					pDS->GetXAttributesSource()->SetXAttributes(pFtr,tab);
					undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,FALSE);
				}
				else if(m_nFieldOption==length && pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				{
					CString X, f;
					f.Format("%%.%dlf", nDigit);
					X.Format(f,((CGeoCurveBase*)pGeo)->GetLength());
					var = (_bstr_t)(LPCTSTR)X;
					tab.BeginAddValueItem();
					tab.AddValue(field,&CVariantEx(var));
					tab.EndAddValueItem();
					oldtab.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr,oldtab);
					oldtab.EndAddValueItem();

					if(m_bNotCoverValidValue && CheckFieldValid(oldtab,field))
						continue;

					pDS->GetXAttributesSource()->SetXAttributes(pFtr,tab);
					undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,FALSE);
				}
				else if(m_nFieldOption==area && pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				{
					CString Y, f;
					f.Format("%%.%dlf", nDigit);
					Y.Format(f,((CGeoCurveBase*)pGeo)->GetArea());
					var = (_bstr_t)(LPCTSTR)Y;
					tab.BeginAddValueItem();
					tab.AddValue(field,&CVariantEx(var));
					tab.EndAddValueItem();
					oldtab.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr,oldtab);
					oldtab.EndAddValueItem();

					if(m_bNotCoverValidValue && CheckFieldValid(oldtab,field))
						continue;

					pDS->GetXAttributesSource()->SetXAttributes(pFtr,tab);
					undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,FALSE);
				}
				else if(m_nFieldOption==text && pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
				{
					CString Y = ((CGeoText*)pGeo)->GetText();
					var = (_bstr_t)(LPCTSTR)Y;
					tab.BeginAddValueItem();
					tab.AddValue(field,&CVariantEx(var));
					tab.EndAddValueItem();
					oldtab.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr,oldtab);
					oldtab.EndAddValueItem();

					if(m_bNotCoverValidValue && CheckFieldValid(oldtab,field))
						continue;

					pDS->GetXAttributesSource()->SetXAttributes(pFtr,tab);
					undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,FALSE);
				}
				else if(m_nFieldOption==bigao/* && pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) */)
				{
					CArray<PT_3DEX,PT_3DEX> pts;
					pGeo->GetShape(pts);
					if(pts.GetSize()!=2)
						continue;
					double val = pts[0].z-pts[1].z;

					CString Y, f;
					f.Format("%%.%dlf", nDigit);
					Y.Format(f,val);
					var = (_bstr_t)(LPCTSTR)Y;

					tab.BeginAddValueItem();
					tab.AddValue(field,&CVariantEx(var));
					tab.EndAddValueItem();
					oldtab.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr,oldtab);
					oldtab.EndAddValueItem();
					
					if(m_bNotCoverValidValue && CheckFieldValid(oldtab,field))
						continue;
					
					pDS->GetXAttributesSource()->SetXAttributes(pFtr,tab);
					undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,FALSE);
				}
				else
					;
				
			}
		}
		
		GProgressEnd();
		undo.Commit();
		Finish();
	}	
	CEditCommand::PtClick(pt,flag);
}

void CFixAttriToExtAttriCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)(m_strLayerCodes);
	tab.AddValue(PF_FIXTOEXT_LAYER,&CVariantEx(var));
	var = (long)(m_nFieldOption);
	tab.AddValue(PF_FIXTOEXT_OPTION,&CVariantEx(var));	
	var = (_bstr_t)(LPCTSTR)(m_strFieldName);
	tab.AddValue(PF_FIXTOEXT_FIELD,&CVariantEx(var));
	
	var = (_bstr_t)(bool)(m_bNotCoverValidValue);
	tab.AddValue("NotCoverValidValue",&CVariantEx(var));
}

void CFixAttriToExtAttriCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(AccelStr()+"Command",Name());	
	param->AddLayerNameParamEx(PF_FIXTOEXT_LAYER,(LPCTSTR)m_strLayerCodes,StrFromResID(IDS_LAYER_NAME));
	param->BeginOptionParam(PF_FIXTOEXT_OPTION,StrFromResID(IDS_FIXATTR));
	param->AddOption(StrFromResID(IDS_DIRPTDIR),dirPntDir,' ',m_nFieldOption==dirPntDir);
	param->AddOption(StrFromResID(IDS_PARALLEL_WIDTH),parrelWidth,' ',m_nFieldOption==parrelWidth);
	param->AddOption(StrFromResID(IDS_CMDPLANE_LAYERCODE),layerCode,' ',m_nFieldOption==layerCode);
	param->AddOption(StrFromResID(IDS_Z_VALUE),elevation,' ',m_nFieldOption==elevation);
	param->AddOption(StrFromResID(IDS_SELCOND_LEN),length,' ',m_nFieldOption==length);
	param->AddOption(StrFromResID(IDS_SELCOND_AREA),area,' ',m_nFieldOption==area);
	param->AddOption(StrFromResID(IDS_CMDPLANE_TEXT),text,' ',m_nFieldOption==text);
	param->AddOption(StrFromResID(IDS_ANNO_DHEIGHT),bigao,' ',m_nFieldOption==bigao);
	param->EndOptionParam();
	param->AddParam(PF_FIXTOEXT_FIELD,(LPCTSTR)m_strFieldName,StrFromResID(IDS_PROPERTIES_EXTEND));
	param->AddParam("NotCoverValidValue",(bool)m_bNotCoverValidValue,StrFromResID(IDS_NOTCOVER_VALID_ATTR));
		
}


void CFixAttriToExtAttriCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_FIXTOEXT_LAYER,var) )
	{
		m_strLayerCodes = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_FIXTOEXT_OPTION,var) )
	{
		m_nFieldOption = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_FIXTOEXT_FIELD,var) )
	{
		m_strFieldName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"NotCoverValidValue",var) )
	{
		m_bNotCoverValidValue = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}



// CExtAttriToFixAttriCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CExtAttriToFixAttriCommand,CEditCommand)

CExtAttriToFixAttriCommand::CExtAttriToFixAttriCommand()
{
	m_bOnlyUseValidValue = TRUE;
}

CExtAttriToFixAttriCommand::~CExtAttriToFixAttriCommand()
{
}


CString CExtAttriToFixAttriCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_EXTATTRTOFIXATTR);
}



void CExtAttriToFixAttriCommand::GetParams(CValueTable &tab)
{
	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)(m_strLayerCodes);
	tab.AddValue(PF_FIXTOEXT_LAYER,&CVariantEx(var));
	var = (long)(m_nFieldOption);
	tab.AddValue(PF_FIXTOEXT_OPTION,&CVariantEx(var));	
	var = (_bstr_t)(LPCTSTR)(m_strFieldName);
	tab.AddValue(PF_FIXTOEXT_FIELD,&CVariantEx(var));
	
	var = (_bstr_t)(bool)(m_bOnlyUseValidValue);
	tab.AddValue("OnlyUseValidValue",&CVariantEx(var));
}


void CExtAttriToFixAttriCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_FIXTOEXT_LAYER,var) )
	{
		m_strLayerCodes = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_FIXTOEXT_OPTION,var) )
	{
		m_nFieldOption = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_FIXTOEXT_FIELD,var) )
	{
		m_strFieldName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"OnlyUseValidValue",var) )
	{
		m_bOnlyUseValidValue = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}


void CExtAttriToFixAttriCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID(AccelStr()+"Command",Name());	
	param->AddLayerNameParamEx(PF_FIXTOEXT_LAYER,(LPCTSTR)m_strLayerCodes,StrFromResID(IDS_LAYER_NAME));
	param->AddParam(PF_FIXTOEXT_FIELD,(LPCTSTR)m_strFieldName,StrFromResID(IDS_EXTATTR));
	param->BeginOptionParam(PF_FIXTOEXT_OPTION,StrFromResID(IDS_FIXATTR));
	param->AddOption(StrFromResID(IDS_DIRPTDIR),dirPntDir,' ',m_nFieldOption==dirPntDir);
	param->AddOption(StrFromResID(IDS_PARALLEL_WIDTH),parrelWidth,' ',m_nFieldOption==parrelWidth);
	param->AddOption(StrFromResID(IDS_FIELDNAME_LAYERID),layerCode,' ',m_nFieldOption==layerCode);
	param->AddOption(StrFromResID(IDS_Z_VALUE),elevation,' ',m_nFieldOption==elevation);
	param->EndOptionParam();
	
	param->AddParam("OnlyUseValidValue",(bool)m_bOnlyUseValidValue,StrFromResID(IDS_ONLY_USE_VALID_ATTR));
}


double VarToDouble(_variant_t& var, BOOL *pRetOK);
CString VarToString(_variant_t& var);

void CExtAttriToFixAttriCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		if(!pDS)return;
		CConfigLibManager *pConfi = gpCfgLibMan;
		CScheme *pScheme = pConfi->GetScheme(pDS->GetScale());
		if(!pScheme)return;

		CFtrLayer *pLayer = NULL;
		CFeature *pFtr = NULL;
		CFtrLayerArray arrFtrlayer;

		pDS->GetFtrLayersByNameOrCode_editable(m_strLayerCodes,arrFtrlayer);

		int i;
		int nSum = 0;

		for (int j=0;j<arrFtrlayer.GetSize();j++)
		{
			pLayer = (CFtrLayer*)arrFtrlayer[j];
			if(!pLayer)continue;
			//由用户指定的字段名找出数据源内部的字段名
			CString field;
			CSchemeLayerDefine *pLayerDef = pScheme->GetLayerDefine(pLayer->GetName());
			if(!pLayerDef)continue;
			int idx;
			if((idx=pLayerDef->FindXDefineByName(m_strFieldName))==-1)
			{
				if(pLayerDef->FindXDefine(m_strFieldName)==-1)
					continue;
				else
					field = m_strFieldName;
			}
			else
			{
				field = pLayerDef->GetXDefine(idx).field;					
			}
			for (int k=0;k<pLayer->GetObjectCount();k++)
			{
				pFtr = pLayer->GetObject(k);
				if(!pFtr)continue;
				nSum++;
			}
		}	
		
		CValueTable tab,oldtab;
		_variant_t var;
		GProgressStart(nSum);
		CUndoBatchAction undo(m_pEditor,Name());
		CUndoModifyProperties undo1(m_pEditor,Name());
		CUndoFtrs undo2(m_pEditor,Name());

		for (j=0;j<arrFtrlayer.GetSize();j++)
		{
			pLayer = (CFtrLayer*)arrFtrlayer[j];
			if(!pLayer)continue;
			//由用户指定的字段名找出数据源内部的字段名
			CString field;
			CSchemeLayerDefine *pLayerDef = pScheme->GetLayerDefine(pLayer->GetName());
			if(!pLayerDef)continue;
			int idx;
			if((idx=pLayerDef->FindXDefineByName(m_strFieldName))==-1)
			{
				if(pLayerDef->FindXDefine(m_strFieldName)==-1)
					continue;
				else
					field = m_strFieldName;
			}
			else
			{
				field = pLayerDef->GetXDefine(idx).field;					
			}
			for (int k=0;k<pLayer->GetObjectCount();k++)
			{
				pFtr = pLayer->GetObject(k);
				if(!pFtr)continue;
				GProgressStep();
				tab.DelAll();
				oldtab.DelAll();

				if(m_nFieldOption==dirPntDir)
				{
					if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
					{		
						double angle = ((CGeoPoint*)pFtr->GetGeometry())->GetDirection();
						var = angle;								
						oldtab.BeginAddValueItem();
						oldtab.AddValue(field,&CVariantEx(var));
						oldtab.EndAddValueItem();

						CValueTable tab2;
						tab2.BeginAddValueItem();
						pDS->GetXAttributesSource()->GetXAttributes(pFtr,tab2);
						tab2.EndAddValueItem();

						if(m_bOnlyUseValidValue && !CheckFieldValid(tab2,field))
							continue;

						const CVariantEx *value = NULL;
						if(tab2.GetValue(0,field,value))
						{
							BOOL bRet = FALSE;
							double angle2 = VarToDouble((_variant_t)*value,&bRet);
							if(bRet)
							{
								m_pEditor->DeleteObject(FtrToHandle(pFtr));
								
								((CGeoPoint*)pFtr->GetGeometry())->SetDirection(angle2);

								m_pEditor->RestoreObject(FtrToHandle(pFtr));
								
								var = angle2;	
								tab.BeginAddValueItem();
								tab.AddValue(field,&CVariantEx(var));
								tab.EndAddValueItem();

								undo1.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,TRUE);

							}
						}								
					}
				}
				else if (m_nFieldOption==parrelWidth)
				{
					if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
					{		
						double width0 = ((CGeoParallel*)pFtr->GetGeometry())->GetWidth();
						var = width0;
						oldtab.BeginAddValueItem();
						oldtab.AddValue(field,&CVariantEx(var));
						oldtab.EndAddValueItem();

						CValueTable tab2;
						tab2.BeginAddValueItem();
						pDS->GetXAttributesSource()->GetXAttributes(pFtr,tab2);
						tab2.EndAddValueItem();

						if(m_bOnlyUseValidValue && !CheckFieldValid(tab2,field))
							continue;

						const CVariantEx *value = NULL;
						if(tab2.GetValue(0,field,value))
						{
							BOOL bRet = FALSE;
							double width2 = VarToDouble((_variant_t)*value,&bRet);
							if(bRet)
							{
								m_pEditor->DeleteObject(FtrToHandle(pFtr));

								((CGeoParallel*)pFtr->GetGeometry())->SetWidth(width2);

								m_pEditor->RestoreObject(FtrToHandle(pFtr));

								var = width2;	
								tab.BeginAddValueItem();
								tab.AddValue(field,&CVariantEx(var));
								tab.EndAddValueItem();

								undo1.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,TRUE);
							}
						}								
					}
				}
				else if(m_nFieldOption==elevation)
				{
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pFtr->GetGeometry()->GetShape(arrPts);//必定至少含有一个点
					CString Z;
					Z.Format(_T("%.4lf"),arrPts[0].z);
					var = (_bstr_t)(LPCTSTR)Z;

					CVariantEx varEx;
					varEx.SetAsShape(arrPts);
					oldtab.BeginAddValueItem();
					oldtab.AddValue(FIELDNAME_SHAPE,&varEx);
					oldtab.EndAddValueItem();

					CValueTable tab2;
					tab2.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr,tab2);
					tab2.EndAddValueItem();

					if(m_bOnlyUseValidValue && !CheckFieldValid(tab2,field))
						continue;

					const CVariantEx *value = NULL;
					if(tab2.GetValue(0,field,value))
					{
						BOOL bRet = FALSE;
						double z2 = VarToDouble((_variant_t)*value,&bRet);
						if(bRet)
						{
							for(int k=0; k<arrPts.GetSize(); k++)
							{
								arrPts[k].z = z2;
							}

							m_pEditor->DeleteObject(FtrToHandle(pFtr));

							pFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());

							m_pEditor->RestoreObject(FtrToHandle(pFtr));

							varEx.SetAsShape(arrPts);	
							tab.BeginAddValueItem();
							tab.AddValue(FIELDNAME_SHAPE,&varEx);
							tab.EndAddValueItem();

							undo1.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,TRUE);

						}
					}
				}
				else if(m_nFieldOption==layerCode)
				{
					CValueTable tab2;
					tab2.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr,tab2);
					tab2.EndAddValueItem();

					if(m_bOnlyUseValidValue && !CheckFieldValid(tab2,field))
						continue;
					
					const CVariantEx *value = NULL;
					if(tab2.GetValue(0,field,value))
					{
						CString strcode = VarToString((_variant_t)*value);
						if(!strcode.IsEmpty())
						{
							CFtrLayer *pLayer1 = pDS->GetFtrLayer(strcode);
							if(!pLayer1 || pLayer1==pLayer) continue;
							int nCls = pFtr->GetGeometry()->GetClassType();
							CFeature *pNewFtr = pLayer1->CreateDefaultFeature(pDS->GetScale(), nCls);

							CArray<PT_3DEX,PT_3DEX> arrPts;
							pFtr->GetGeometry()->GetShape(arrPts);
							CGeometry *pGeo = pNewFtr->GetGeometry();
							pGeo->CreateShape(arrPts.GetData(), arrPts.GetSize());
							pNewFtr->SetCode(pFtr->GetCode());

							if(!m_pEditor->AddObject(pNewFtr, pLayer1->GetID()))
							{
								delete pNewFtr;
								continue;
							}
							GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNewFtr);
							undo2.AddOldFeature(FtrToHandle(pFtr));
							undo2.AddNewFeature(FtrToHandle(pNewFtr));
							m_pEditor->DeleteObject(FtrToHandle(pFtr));
						}
					}
				}							
			}
		}

		undo.AddAction(&undo1);
		undo.AddAction(&undo2);
		GProgressEnd();
		undo.Commit();
		Finish();
	}	
	CEditCommand::PtClick(pt,flag);
}




// CContourAutoLayerSettingCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CContourAutoLayerSettingCommand,CEditCommand)

CContourAutoLayerSettingCommand::CContourAutoLayerSettingCommand()
{
	m_strHandleLayer = _T("");
	m_strIdxContourCode = _T("");
	m_strStdContourCode = _T("");
	m_lfContourInterval = 1.0;
	m_nStdContourCnt = 4;
	strcat(m_strRegPath,"\\ContourAutoLayerSetting");
}

CContourAutoLayerSettingCommand::~CContourAutoLayerSettingCommand()
{
}

void CContourAutoLayerSettingCommand::Abort()
{
	CEditCommand::Abort();
}


CString CContourAutoLayerSettingCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CONTOUR_AUTOLAYERSETTING);
}

void CContourAutoLayerSettingCommand::Start()
{
	if( !m_pEditor )return;
	m_strIdxContourCode = _T("");
	m_strStdContourCode = _T("");
	m_lfContourInterval = 1.0;
	m_nStdContourCnt = 4;
	
	CEditCommand::Start();
	m_pEditor->CloseSelector();
//	m_pEditor->OpenSelector(SELMODE_NORMAL);
}


void CContourAutoLayerSettingCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor)
		return;
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if(!pDS)
		return;

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme)  return;

	if (m_lfContourInterval<0||m_nStdContourCnt<=0||m_strStdContourCode.IsEmpty()||m_strIdxContourCode.IsEmpty())
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}
	CFtrLayer *pIdxLayer = pDS->GetFtrLayer(m_strIdxContourCode);
	if (!pIdxLayer)
	{
		GOutPut(StrFromResID(IDS_IDX_CONTOUR_LAYER_ERROR));
		return;
	}
	CFtrLayer *pStdLayer = pDS->GetFtrLayer(m_strStdContourCode);
	if (!pStdLayer)
	{
		GOutPut(StrFromResID(IDS_STD_CONTOUR_LAYER_ERROR));
		return;
	}

	float fIdxLayerWidth = pScheme->GetLayerDefineLineWidth(pIdxLayer->GetName()), fStdLayerWidth = pScheme->GetLayerDefineLineWidth(pStdLayer->GetName());

	CFeature *pTemp0 = pIdxLayer->CreateDefaultFeature(pDS->GetScale());
	CFeature *pTemp1 = pStdLayer->CreateDefaultFeature(pDS->GetScale());

	CFtrLayerArray arrLayers;
	pDS->GetFtrLayersByNameOrCode_editable(m_strHandleLayer, arrLayers);
	int nSum = 0, i;
	for (i = 0; i < arrLayers.GetSize(); i++)
	{
		nSum += arrLayers[i]->GetValidObjsCount();
	}

	CUndoFtrs undo(m_pEditor,Name());
	GProgressStart(nSum);
	for (i = 0; i<arrLayers.GetSize(); i++)
	{
		CFtrLayer *pLayer = arrLayers[i];
		if(!pLayer||pLayer->IsDeleted()||pLayer->IsLocked()||!pLayer->IsVisible())
			continue;

		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr)continue;
			GProgressStep();
			if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			{
				if (CAutoLinkContourCommand::CheckObjForZ(pFtr->GetGeometry()))
				{
					PT_3DEX expt;
					expt = pFtr->GetGeometry()->GetDataPoint(0);
					
					int n = (int)floor(expt.z/m_lfContourInterval);
					int n0 = (int)ceil(expt.z/m_lfContourInterval);
					if( fabs(expt.z-n*m_lfContourInterval)>1e-4&&fabs(expt.z-n0*m_lfContourInterval)>1e-4 )
					{						
						continue;
					}
					if( fabs(expt.z-n0*m_lfContourInterval)<=1e-4&&!(fabs(expt.z-n*m_lfContourInterval)<=1e-4))
						n = n0;
					if(0==n%(m_nStdContourCnt+1)&&pLayer!=pIdxLayer)
					{						
						if( pTemp0 )
						{						
							CValueTable tab;
							tab.BeginAddValueItem();
							GETXDS(m_pEditor)->GetXAttributes(pFtr,tab );
							tab.EndAddValueItem();
							
							m_pEditor->DeleteObject(FtrToHandle(pFtr));
							undo.AddOldFeature(FtrToHandle(pFtr));
							
							CValueTable table;
							table.BeginAddValueItem();
							pFtr->WriteTo(table);
							table.EndAddValueItem();
							table.DelField(FIELDNAME_GEOCOLOR);		
							
							CFeature *pNewFtr = pTemp0->Clone();
							pNewFtr->ReadFrom(table);
							((CGeoCurve*)pNewFtr->GetGeometry())->SetLineWidth(fIdxLayerWidth);
							pNewFtr->GetGeometry()->SetColor(COLOUR_BYLAYER);
							pNewFtr->SetID(OUID());
							m_pEditor->AddObject(pNewFtr,pIdxLayer->GetID());
							undo.AddNewFeature(FtrToHandle(pNewFtr));
							GETXDS(m_pEditor)->SetXAttributes(pNewFtr,tab );		
							
						}
						continue;
					}
					else if(0!=n%(m_nStdContourCnt+1) && pLayer!=pStdLayer) 
					{
						if( pTemp1 )
						{						
							CValueTable tab;
							tab.BeginAddValueItem();
							GETXDS(m_pEditor)->GetXAttributes(pFtr,tab );
							tab.EndAddValueItem();
							
							m_pEditor->DeleteObject(FtrToHandle(pFtr));
							undo.AddOldFeature(FtrToHandle(pFtr));
							
							CValueTable table;
							table.BeginAddValueItem();
							pFtr->WriteTo(table);
							table.EndAddValueItem();							
							table.DelField(FIELDNAME_GEOCOLOR);

							CFeature *pNewFtr = pTemp1->Clone();
							pNewFtr->ReadFrom(table);
							((CGeoCurve*)pNewFtr->GetGeometry())->SetLineWidth(fStdLayerWidth);
							pNewFtr->GetGeometry()->SetColor(COLOUR_BYLAYER);
							pNewFtr->SetID(OUID());
							m_pEditor->AddObject(pNewFtr,pStdLayer->GetID());
							undo.AddNewFeature(FtrToHandle(pNewFtr));
							GETXDS(m_pEditor)->SetXAttributes(pNewFtr,tab );							
						}
						continue;
					}
				}
			}
			
		}
	}
	GProgressEnd();
	if (undo.arrNewHandles.GetSize() > 0)
	{
		undo.Commit();
		m_pEditor->OnSelectChanged();
		m_pEditor->RefreshView();		
	}
	if (pTemp0)
	{
		delete pTemp0;
	}
	if (pTemp1)
	{
		delete pTemp1;
	}
	Finish();
	CEditCommand::PtClick(pt,flag);
}

void CContourAutoLayerSettingCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strHandleLayer;
	tab.AddValue("strHandleLayer", &CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strIdxContourCode;
	tab.AddValue(PF_CAL_INDEX_CONTOUR_CODE,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strStdContourCode;
	tab.AddValue(PF_CAL_STD_CONTOUR_CODE,&CVariantEx(var));
	var = (double)(m_lfContourInterval);
	tab.AddValue(PF_CAL_CONTOUR_INTERVAL,&CVariantEx(var));
	var = (long)(m_nStdContourCnt);
	tab.AddValue(PF_CAL_STD_CONTOUR_CNT,&CVariantEx(var));	
}

void CContourAutoLayerSettingCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ContourAutoLayerSettingCommand",StrFromResID(IDS_CMDNAME_CONTOUR_AUTOLAYERSETTING));	
	param->AddLayerNameParamEx("strHandleLayer", (LPCTSTR)(m_strHandleLayer), StrFromResID(IDS_CMDPLANE_HANDLELAYER));
	param->AddLayerNameParam(PF_CAL_INDEX_CONTOUR_CODE,(LPCTSTR)(m_strIdxContourCode),StrFromResID(IDS_INDEX_CONTOUR));
	param->AddLayerNameParam(PF_CAL_STD_CONTOUR_CODE,(LPCTSTR)(m_strStdContourCode),StrFromResID(IDS_STD_CONTOUR));
	param->AddParam(PF_CAL_CONTOUR_INTERVAL,double(m_lfContourInterval),StrFromResID(IDS_CONTOUR_INTERVAL));
	param->AddParam(PF_CAL_STD_CONTOUR_CNT,long(m_nStdContourCnt),StrFromResID(IDS_CMDPLANE_STD_CONTOUR_CNT));
		
}


void CContourAutoLayerSettingCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if (tab.GetValue(0, "strHandleLayer", var))
	{
		m_strHandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CAL_INDEX_CONTOUR_CODE,var) )
	{
		m_strIdxContourCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CAL_STD_CONTOUR_CODE,var) )
	{
		m_strStdContourCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CAL_CONTOUR_INTERVAL,var) )
	{
		m_lfContourInterval = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CAL_STD_CONTOUR_CNT,var) )
	{
		m_nStdContourCnt = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}


// CContourAutoRepairCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CContourAutoRepairCommand,CEditCommand)

CContourAutoRepairCommand::CContourAutoRepairCommand()
{
	m_strIdxContourCode = _T("");
	m_strStdContourCode = _T("");
	m_lfContourInterval = 1.0;
	//m_nStdContourCnt = 4;
	strcat(m_strRegPath,"\\ContourAutoRepair");
}

CContourAutoRepairCommand::~CContourAutoRepairCommand()
{
}

void CContourAutoRepairCommand::Abort()
{
	CEditCommand::Abort();
}


CString CContourAutoRepairCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CONTOUR_AUTOREPAIR);
}

void CContourAutoRepairCommand::Start()
{
	if( !m_pEditor )return;
	m_strIdxContourCode = _T("");
	m_strStdContourCode = _T("");
	m_lfContourInterval = 1.0;
//	m_nStdContourCnt = 4;
	
	CEditCommand::Start();
	m_pEditor->CloseSelector();

	// 参数
	/*if (m_strIdxContourCode.IsEmpty() || m_strStdContourCode.IsEmpty())
	{
		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		if (!pDS) return;
		
		CConfigLibManager *pConfig = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
		CScheme *pScheme = pConfig->GetScheme(pDS->GetScale());
		if(!pScheme)return;

		if (m_strIdxContourCode.IsEmpty())
		{
			CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(StrFromResID(IDS_INDEX_CONTOUR));
			if (pScheme)
			{
				__int64 code = pSchemeLayer->GetLayerCode();
				m_strIdxContourCode.Format("%I64d",code);
			}
		}

		if (m_strStdContourCode.IsEmpty())
		{
			CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(StrFromResID(IDS_STD_CONTOUR));
			if (pScheme)
			{
				__int64 code = pSchemeLayer->GetLayerCode();
				m_strStdContourCode.Format("%I64d",code);
			}
		}
	}*/
}

void CContourAutoRepairCommand::PtClick(PT_3D &pt, int flag)
{ 
	if(!m_pEditor)
		return;
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if(!pDS)
		return;
	if (m_lfContourInterval<1e-4/*||m_nStdContourCnt<=0*/||m_strStdContourCode.IsEmpty()||m_strIdxContourCode.IsEmpty())
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}
	CFtrLayer *pIdxLayer = pDS->GetFtrLayer(m_strIdxContourCode);
	if (!pIdxLayer)
	{
		GOutPut(StrFromResID(IDS_IDX_CONTOUR_LAYER_ERROR));
		return;
	}
	CFtrLayer *pStdLayer = pDS->GetFtrLayer(m_strStdContourCode);
	if (!pStdLayer)
	{
		GOutPut(StrFromResID(IDS_STD_CONTOUR_LAYER_ERROR));
		return;
	}

	int i, j, num1, num2, num;
	num1 = pIdxLayer->GetObjectCount();
	num2 = pStdLayer->GetObjectCount();

	CArray<FTR_HANDLE,FTR_HANDLE> handles;
	for (i=0; i<num1; i++)
	{
		CFeature *pFtr = pIdxLayer->GetObject(i);
		if (pFtr)
		{
			handles.Add(FTR_HANDLE(pFtr));
		}
	}

	for (i=0; i<num2; i++)
	{
		CFeature *pFtr = pStdLayer->GetObject(i);
		if (pFtr)
		{
			handles.Add(FTR_HANDLE(pFtr));
		}
	}

	num = handles.GetSize();

	CUndoBatchAction batchUndo(m_pEditor,Name());
	GProgressStart(num);
	
	int nRepairNum = 0;
	for (i=0;i<num;i++)
	{
		GProgressStep();
		
		CFeature *pFtr = HandleToFtr(handles[i]);
		if (!pFtr) continue;
		
		CGeometry *pGeo = pFtr->GetGeometry();
		if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
			continue;
		
		if( CModifyZCommand::CheckObjForContour(pGeo) )//判断异常高程点计曲线/首曲线
			continue;

		CUndoModifyProperties undo(m_pEditor,Name());
		undo.oldVT.BeginAddValueItem();
		CPFeature(handles[i])->WriteTo(undo.oldVT);
		undo.oldVT.EndAddValueItem();
		
		// 处理平均高程在1/3等高距范围内的地物
		CArray<PT_3DEX,PT_3DEX> pts;
		CArray<PT_3DEX,PT_3DEX> oldPts;
		pGeo->GetShape(pts);
		oldPts.Copy(pts);//
		//这里需要排除差异非常大的异常点 
		double z =0,fz =0,pz =0;
		set<double> myset;
		set<double>::iterator it;
		int ptnum = pts.GetSize();

		//方案一，先获取最为多数的高程点
		for (j=0; j<ptnum; j++)
		{
			myset.insert(pts[j].z);//获取所有高程点中无重复的点
		}
		
		// 是否在误差范围内
		int maxNum =0;
		for (it = myset.begin();it!=myset.end();it++)
		{
			double tz = *it;
			int nNum =0;
			for (int i =0;i<ptnum;i++)
			{
				if (fabs(tz - oldPts.GetAt(i).z)<=GraphAPI::g_lfZTolerance)
				{
					nNum++;
				}							
			}

			if (maxNum<=nNum)
			{
				maxNum = nNum;//得到最多的高程点数量
				double v1, v2;
				v1 = fabs( tz - m_lfContourInterval*floor(tz/m_lfContourInterval) );//返回小于等于的最大整数
				v2 = fabs( tz - m_lfContourInterval*ceil(tz/m_lfContourInterval) );//返回大于等于的最大整数
				
				double fLimit = m_lfContourInterval/3;
				if( v1>v2 )
				{
					if( v2<fLimit )
						tz = m_lfContourInterval*ceil(tz/m_lfContourInterval);
					else
						tz = m_lfContourInterval*floor(tz/m_lfContourInterval);
				}	
				else
				{
					if( v1<fLimit )
						tz = m_lfContourInterval*floor(tz/m_lfContourInterval);
				}
				pz =tz;
			}
			
		}

	//方案二 直接计算平均高程点
		for (j=0; j<ptnum; j++)
		{
			fz += pts[j].z;
		}

		fz /= ptnum;

		double v1, v2;
		v1 = fabs( fz - m_lfContourInterval*floor(fz/m_lfContourInterval) );//返回小于等于的最大整数
		v2 = fabs( fz - m_lfContourInterval*ceil(fz/m_lfContourInterval) );//返回大于等于的最大整数
		double fLimit = m_lfContourInterval/3;
		if( v1>v2 )
		{
			if( v2<fLimit )
				fz = m_lfContourInterval*ceil(fz/m_lfContourInterval);
		}
		else
		{
			if( v1<fLimit )
				fz = m_lfContourInterval*floor(fz/m_lfContourInterval);
		}

		// 是否在误差范围内
		if (fabs( fz - m_lfContourInterval*floor(fz/m_lfContourInterval) ) > 1e-4)
		{
			z = pz;//方案二计算失败，方案一顶替
		}
		else
		{
			if (fabs(pz-fz)>=m_lfContourInterval)
			{
				z = pz;//方案2
			}
			else
			{
				z = fz;//方案1
			}
			
		}
		nRepairNum++;
		// 修复
		for (j=0; j<ptnum; j++)
		{
			pts[j].z = z;
		}		
		
		undo.arrHandles.Add(handles[i]);		
		
		m_pEditor->DeleteObject(handles[i],FALSE);

		pGeo->CreateShape(pts.GetData(),pts.GetSize());
		
		m_pEditor->RestoreObject(handles[i]);		
		
		undo.newVT.BeginAddValueItem();
		CPFeature(handles[i])->WriteTo(undo.newVT);
		undo.newVT.EndAddValueItem();
		
		batchUndo.AddAction(&undo);		
	}
	
	batchUndo.Commit();
	
	GProgressEnd();
	
	CString strFormat, strMsg;
	strFormat.LoadString(IDS_TIP_REPAIRCONTOUR);
	strMsg.Format(strFormat,nRepairNum);
	GOutPut(strMsg);	

	Finish();
	CEditCommand::PtClick(pt,flag);
}

void CContourAutoRepairCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)m_strIdxContourCode;
	tab.AddValue(PF_CAL_INDEX_CONTOUR_CODE,&CVariantEx(var));
	var = (_bstr_t)(LPCTSTR)m_strStdContourCode;
	tab.AddValue(PF_CAL_STD_CONTOUR_CODE,&CVariantEx(var));
	var = (double)(m_lfContourInterval);
	tab.AddValue(PF_CAL_CONTOUR_INTERVAL,&CVariantEx(var));
// 	var = (long)(m_nStdContourCnt);
// 	tab.AddValue(PF_CAL_STD_CONTOUR_CNT,&CVariantEx(var));	
}

void CContourAutoRepairCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ContourAutoRepairCommand",StrFromResID(IDS_CMDNAME_CONTOUR_AUTOREPAIR));	
	param->AddLayerNameParam(PF_CAL_INDEX_CONTOUR_CODE,(LPCTSTR)(m_strIdxContourCode),StrFromResID(IDS_INDEX_CONTOUR));
	param->AddLayerNameParam(PF_CAL_STD_CONTOUR_CODE,(LPCTSTR)(m_strStdContourCode),StrFromResID(IDS_STD_CONTOUR));
	param->AddParam(PF_CAL_CONTOUR_INTERVAL,double(m_lfContourInterval),StrFromResID(IDS_CONTOUR_INTERVAL));
//	param->AddParam(PF_CAL_STD_CONTOUR_CNT,long(m_nStdContourCnt),StrFromResID(IDS_CMDPLANE_STD_CONTOUR_CNT));
		
}


void CContourAutoRepairCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
	if( tab.GetValue(0,PF_CAL_INDEX_CONTOUR_CODE,var) )
	{
		m_strIdxContourCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CAL_STD_CONTOUR_CODE,var) )
	{
		m_strStdContourCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CAL_CONTOUR_INTERVAL,var) )
	{
		m_lfContourInterval = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
// 	if( tab.GetValue(0,PF_CAL_STD_CONTOUR_CNT,var) )
// 	{
// 		m_nStdContourCnt = (long)(_variant_t)*var;
// 		SetSettingsModifyFlag();
// 	}
	CEditCommand::SetParams(tab,bInit);
}


// CContourEvaluateCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CContourEvaluateCommand,CEditCommand)

CContourEvaluateCommand::CContourEvaluateCommand()
{
// 	m_strIdxContourCode = _T("");
// 	m_strStdContourCode = _T("");
	m_lfContourInterval = 5;
	m_fStartHeight = 0;
	m_nHeightMode = 0;
	strcat(m_strRegPath,"\\ContourEvaluate");
}

CContourEvaluateCommand::~CContourEvaluateCommand()
{
}

void CContourEvaluateCommand::Abort()
{
	CEditCommand::Abort();
}


CString CContourEvaluateCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CONTOUR_EVALUATE);
}

void CContourEvaluateCommand::Start()
{
	if( !m_pEditor )return;
// 	m_strIdxContourCode = _T("");
// 	m_strStdContourCode = _T("");
	m_lfContourInterval = 5;
	m_fStartHeight = 0;
	m_nHeightMode = 0;

	CEditCommand::Start();
	m_pEditor->CloseSelector();
}

void CContourEvaluateCommand::PtMove(PT_3D &pt)
{
	if(m_nStep == 1)
	{
		GrBuffer buf;
		buf.BeginLineString(0,0,0);
		buf.MoveTo(&m_ptLine[0]);
		buf.LineTo(&pt);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);		
	}
	
	CEditCommand::PtMove(pt);
}

void CContourEvaluateCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{
		m_ptLine[0] = pt;
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);
	}
	else if (m_nStep == 1)
	{
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		m_ptLine[1] = pt;

		// 按相交顺序排序，高程依次增加
		CArray<ObjRatio,ObjRatio> arrObjRatio;

		Envelope e1, e2;
		e1.CreateFromPts(m_ptLine,2);
		e2 = e1;
		e2.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
		m_pEditor->GetDataQuery()->FindObjectInRect(e2,m_pEditor->GetCoordWnd().m_pSearchCS);
		
		int num1;
		const CPFeature * ppftr1 = m_pEditor->GetDataQuery()->GetFoundHandles(num1);
		if( !ppftr1  )
		{	
			Abort();
			return;
		}
		CArray<CPFeature,CPFeature> ftr1;
		ftr1.SetSize(num1);
		memcpy(ftr1.GetData(),ppftr1,num1*sizeof(*ppftr1));
		
		
// 		BOOL bCheckBase = (m_strIdxContourCode.GetLength()>0);
// 		BOOL bCheckSon = (m_strStdContourCode.GetLength()>0);

		ObjRatio item;
		CGeometry *pObj;
		const CShapeLine *pSL;
		PT_3DEX *pts;
		int size1;
		for( int i=0; i<num1; i++)
		{
			pObj = ftr1[i]->GetGeometry();
			if( !pObj )continue;
			
			BOOL bBaseLine = FALSE, bSonLine = FALSE;
			//去除点状地物、文本、非等高地物、非母线特征码的地物
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) || pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )continue;
			if( /*m_bOnlyContour && */!CModifyZCommand::CheckObjForContour(pObj) )continue;
// 			if( !bCheckBase || CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr1[i],(LPCTSTR)m_strIdxContourCode) )
// 				bBaseLine = TRUE;
// 			if( !bCheckSon || CAutoLinkContourCommand::CheckObjForLayerCode(PDOC(m_pEditor)->GetDlgDataSource(),ftr1[i],(LPCTSTR)m_strStdContourCode) )
// 				bSonLine = TRUE;
// 			
// 			if (!bBaseLine && !bSonLine) continue;
			
			if( pObj->GetDataPointSum()<2 )continue;
			
			//精细检验是否与第一线段相交，记录交点
			pSL = pObj->GetShape();
			if( !pSL )continue;
			
			PT_3D ret;
			double t=-1,st0=-1;
			CPtrArray arr;
			pSL->GetShapeLineUnit(arr);
			int pos = 0;
			for( int k =0;k<arr.GetSize();k++ )
			{
				CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[k];
				if( e1.bIntersect(&pUnit->evlp) )
				{
					pts = pUnit->pts;
					for( int j=0; j<pUnit->nuse-1; j++,pts++)
					{
						if( e1.bIntersect(&pts[0],&pts[1]) )
						{
							//求相交点
							if (!GLineIntersectLineSeg1(m_ptLine[0].x,m_ptLine[0].y,m_ptLine[1].x,m_ptLine[1].y,pts[0].x,pts[0].y,pts[0].z,
								pts[1].x,pts[1].y,pts[1].z,&(ret.x),&(ret.y),&(ret.z),&t,&st0))
							{
								continue;
							}					
							else
							{
								if( fabs(pts[0].x-pts[1].x)>=1e-4 && fabs(pts[0].y-pts[1].y)<1e-4 )
									ret.z = pts[0].z;
								else
								{
									if( fabs(pts[0].x-pts[1].x)>fabs(pts[0].y-pts[1].y) )
										ret.z = pts[0].z + (ret.x-pts[0].x)*(pts[0].z-pts[1].z)/(pts[0].x-pts[1].x);
									else
										ret.z = pts[0].z + (ret.y-pts[0].y)*(pts[0].z-pts[1].z)/(pts[0].y-pts[1].y);
								}
							}
							
							item.ftr = FtrToHandle(ftr1[i]); item.lfRatio = t;
							
							//按照 t 大小排序插入
							size1 = arrObjRatio.GetSize();
							for( int m=0; m<size1 && item.lfRatio>=arrObjRatio[m].lfRatio; m++);
							if( m<size1 )arrObjRatio.InsertAt(m,item);
							else arrObjRatio.Add(item);	
							
							goto FINDOVER;
							
						}
					}
				}
				pos += pUnit->nuse;		
			}
FINDOVER:;
		}

		// 依次赋值
		CUndoFtrs undo(m_pEditor,Name());
		double fHeight = m_fStartHeight;
		double fContourInterval = (m_nHeightMode==0?m_lfContourInterval:-m_lfContourInterval);
		for(i=0; i<arrObjRatio.GetSize(); i++)
		{
			CFeature *pFtr = HandleToFtr(arrObjRatio[i].ftr);
			pObj = pFtr->GetGeometry();
			
			pObj = pObj->Clone();
			if( !pObj )continue;
			
			CArray<PT_3DEX,PT_3DEX> arr;
			pObj->GetShape(arr);
			int num0 = arr.GetSize();
			
			for( int j=0; j<num0; j++)
			{
				PT_3DEX &pt1 = arr.ElementAt(j);
				pt1.z = fHeight;
			}
			
			pObj->CreateShape(arr.GetData(),arr.GetSize());
			
			CFeature * pFtr1 = pFtr->Clone();
			if(!pFtr1) continue;

			pFtr1->SetID(OUID());
			pFtr1->SetGeometry(pObj);						
			
			if( !m_pEditor->AddObject(pFtr1,m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr))) )
			{
				delete pFtr1;
				Abort();
				return;
			}
			
			GETXDS(m_pEditor)->CopyXAttributes(pFtr,pFtr1);
			undo.arrNewHandles.Add(FtrToHandle(pFtr1));	
			
			m_pEditor->DeleteObject(FtrToHandle(pFtr));
			undo.arrOldHandles.Add(FtrToHandle(pFtr));	

			fHeight += fContourInterval;
		}
		undo.Commit();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
	}

	CEditCommand::PtClick(pt,flag);
}

void CContourEvaluateCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
// 	var = (_bstr_t)(LPCTSTR)m_strIdxContourCode;
// 	tab.AddValue(PF_CAL_INDEX_CONTOUR_CODE,&CVariantEx(var));
// 	var = (_bstr_t)(LPCTSTR)m_strStdContourCode;
// 	tab.AddValue(PF_CAL_STD_CONTOUR_CODE,&CVariantEx(var));
	var = (double)(m_lfContourInterval);
	tab.AddValue(PF_CAL_CONTOUR_INTERVAL,&CVariantEx(var));
	var = (double)(m_fStartHeight);
	tab.AddValue(PF_CONTOUREVA_STARTZ,&CVariantEx(var));
	var = (long)(m_nHeightMode);
	tab.AddValue(PF_CONTOUREVA_HEIGHTMODE,&CVariantEx(var));
}

void CContourEvaluateCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ContourEvaluate",StrFromResID(IDS_CMDNAME_CONTOUR_EVALUATE));	
	//param->AddLayerNameParam(PF_CAL_INDEX_CONTOUR_CODE,(LPCTSTR)(m_strIdxContourCode),StrFromResID(IDS_INDEX_CONTOUR));
	//param->AddLayerNameParam(PF_CAL_STD_CONTOUR_CODE,(LPCTSTR)(m_strStdContourCode),StrFromResID(IDS_STD_CONTOUR));
	param->AddParam(PF_CAL_CONTOUR_INTERVAL,double(m_lfContourInterval),StrFromResID(IDS_CONTOUR_INTERVAL));
	param->AddParam(PF_CONTOUREVA_STARTZ,double(m_fStartHeight),StrFromResID(IDS_CMDNAME_FIRSTCONTOUR));
	param->BeginOptionParam(PF_CONTOUREVA_HEIGHTMODE,StrFromResID(IDS_CMDPLANE_HEIGHTDIR));
	param->AddOption(StrFromResID(IDS_CMDPLANE_HEIGHTINCREASE),0,' ',m_nHeightMode==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_HEIGHTDECREASE),1,' ',m_nHeightMode==1);
	param->EndOptionParam();
}


void CContourEvaluateCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;
	//	bool bNeedRefresh = false;
	
// 	if( tab.GetValue(0,PF_CAL_INDEX_CONTOUR_CODE,var) )
// 	{
// 		m_strIdxContourCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
// 		SetSettingsModifyFlag();
// 	}
// 	if( tab.GetValue(0,PF_CAL_STD_CONTOUR_CODE,var) )
// 	{
// 		m_strStdContourCode = (LPCTSTR)(_bstr_t)(_variant_t)*var;
// 		SetSettingsModifyFlag();
// 	}
	if( tab.GetValue(0,PF_CAL_CONTOUR_INTERVAL,var) )
	{
		m_lfContourInterval = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CONTOUREVA_STARTZ,var) )
	{
		m_fStartHeight = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_CONTOUREVA_HEIGHTMODE,var) )
	{
		m_nHeightMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}


//////////////////////////////////////////////////////////////////////////
//设定独立符号项//
//////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CSetSpecliaSymbolCommand,CEditCommand)
CSetSpecliaSymbolCommand::CSetSpecliaSymbolCommand()
{
	
}

CSetSpecliaSymbolCommand::~CSetSpecliaSymbolCommand()
{
	
}


CString CSetSpecliaSymbolCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_SETSPECIALSYMBOL);
}


void CSetSpecliaSymbolCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	if( m_nStep==1 )
	{		
		CDlgSetSpecialSymbol dlg;
		
		if (dlg.DoModal()!=IDOK)
		{
			Abort();
			m_nExitCode = CMPEC_STARTOLD;
			return;
		}
		m_SymbolName = dlg.GetSymbolName();
		SetSpecialSymbol(m_SymbolName);
		Finish();
		m_nExitCode = CMPEC_STARTOLD;
	}
}

void CSetSpecliaSymbolCommand::Abort()
{
	CEditCommand::Abort();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}
void CSetSpecliaSymbolCommand::Start()
{
	CEditCommand::Start();
}



void CSetSpecliaSymbolCommand::SetSpecialSymbol(CString StrSymbolName)
{
	if (StrSymbolName.IsEmpty())
	{
		return;
	}
	CUndoModifyProperties undo(m_pEditor,Name());
	CSelection *pSel = 	m_pEditor->GetSelection();
	int num;
	const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
	if (num<=0)
	{
		AfxMessageBox(StrFromResID(IDS_SPECIALSYMBOL_CHECK));
		Abort();
		return;
	}
	CFeature *pFtr;
//	CGeometry *pGeo;
	
	for (int i=0;i<num;i++)
	{
		pFtr=HandleToFtr(handles[i]);
		if (!pFtr)
		{
			continue;
		}
		m_pEditor->DeleteObject(FtrToHandle(pFtr));
		CString strOldSymName = pFtr->GetGeometry()->GetSymbolName();
	
		pFtr->GetGeometry()->SetSymbolName(StrSymbolName);
		m_pEditor->RestoreObject(FtrToHandle(pFtr));
		undo.SetModifyProp(pFtr,FIELDNAME_SYMBOLNAME,
			&CVariantEx((_variant_t)(LPCTSTR)strOldSymName),
			&CVariantEx((_variant_t)(LPCTSTR)StrSymbolName));
	}	
	
	pFtr = NULL;
	
	if (undo.arrHandles.GetSize() > 0)
	{
		undo.Commit();
	}	

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CMapDecorateCommand,CEditCommand)


BOOL CMapDecorateCommand::m_bStaticValid = FALSE;
MapDecorate::CMapDecorator CMapDecorateCommand::m_MapDecorator;


CMapDecorateCommand::CMapDecorateCommand()/*:m_dlg(AfxGetMainWnd())*/
{
	m_nOperation = opNone;
	m_bSortBound = TRUE;

	m_pDlg = NULL;
}


CMapDecorateCommand::~CMapDecorateCommand()
{
	if(m_pDlg)
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}
}


CString CMapDecorateCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_MAPDECORATE);
}

void CMapDecorateCommand::Start()
{	
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	long scale = pDS->GetScale();

	m_MapDecorator.m_bNew = FALSE;		//旧版图廓整饰
	m_MapDecorator.InitData(pDS);
	m_MapDecorator.LoadParams(scale);
	m_MapDecorator.LoadDatas(NULL);

//根据比例尺，自动选择相应的对话框
	if( 500 == scale || 1000 == scale || 2000 == scale)
	{
		m_pDlg = new CMapDecorateDlg_2KO;
		if( !m_pDlg ) return;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		m_pDlg = new CMapDecorateDlg_25KO;
		if( !m_pDlg ) return;
	}
	else
	{
		GOutPut(StrFromResID(IDS_MD_TIP_OLD));
		Abort();
		m_nExitCode = CMPEC_NULL;
		return;
	}
//

	Envelope e = pDS->GetBound();
	m_MapDecorator.CalcMapNumberAndNearmap(pDS,PT_3D(e.CenterX(),e.CenterY(),0));

	m_pDlg->m_pMapDecorator = &m_MapDecorator;

	m_bSortBound = TRUE;

	CString title0,path;
	path = StrFromResID(IDS_MD_TITLE_OLD);
/*	path = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager()->GetConfigLibItemByScale(scale).GetPath();

	int pos = path.ReverseFind('\\');
	if( pos>0 )
	{
		CString path1 = path;
		path1 = path1.Left(pos);

		pos = path1.ReverseFind('\\');
		if( pos>0 )
		{
			path = path.Mid(pos+1);
		}
	}
*/

//
	if( 500 == scale || 1000 == scale || 2000 == scale)
	{
		m_pDlg->Create(IDD_MAPSHOW_DIALOG,AfxGetMainWnd());
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		m_pDlg->Create(IDD_MAPSHOW_25KO_DIALOG,AfxGetMainWnd());
	}
//

	m_pDlg->CenterWindow();
	m_pDlg->ShowWindow(SW_SHOW);

	m_pDlg->GetWindowText(title0);

	title0 = title0 + "(" + path + ")";
	m_pDlg->SetWindowText(title0);

	m_pDlg->m_pCmd = this;
}


void CMapDecorateCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	CString cmd;
	
	if( tab.GetValue(0,"Command",var) )
	{
		cmd = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		if( cmd.CompareNoCase("SelectBound")==0 )
		{
			m_pEditor->OpenSelector(SELMODE_SINGLE,SELFILTER_CURVE|SELFILTER_SURFACE);
			m_nOperation = opSelectBound;
			m_nStep = 0;

			m_pDlg->ShowWindow(SW_HIDE);
		}
		else if( cmd.CompareNoCase("SelectCorner")==0 )
		{
			m_pEditor->CloseSelector();
			m_nOperation = opSelectCorner;
			m_nStep = 0;

			m_pDlg->ShowWindow(SW_HIDE);
		}
		else if( cmd.CompareNoCase("OK")==0 )
		{			
			m_MapDecorator.Build(PDOC(m_pEditor),NULL,PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),FALSE,m_bSortBound);

			CUndoFtrs undo(m_pEditor,Name());
			undo.arrNewHandles.Copy(m_MapDecorator.m_arrNewObjHandles);
			undo.Commit();

			Finish();
			m_nExitCode = CMPEC_NULL;

			return;

		}
		else if( cmd.CompareNoCase("Cancel")==0 )
		{
			m_pDlg->ShowWindow(SW_HIDE);
			Abort();
			m_nExitCode = CMPEC_NULL;
			return;
		}
	}

	CEditCommand::SetParams(tab,bInit);
}


void CMapDecorateCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nOperation==opSelectBound )
	{
		if( m_nStep==0 )
		{	
			if( !CanGetSelObjs(flag) )return;
			EditStepOne();
		}
		if( m_nStep==1 )
		{
			CSelection* pSel = m_pEditor->GetSelection();

			FTR_HANDLE hFtr = pSel->GetLastSelectedObj();
			
			if( !hFtr )return;

			CFeature *pFtr = HandleToFtr(hFtr);
			if( pFtr )
			{
				CGeometry *pGeo = pFtr->GetGeometry();
				if( pGeo )
				{
					if( (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface))) && pGeo->GetDataPointSum()>=4 )
					{
						CArray<PT_3DEX,PT_3DEX> arrPts;
						pGeo->GetShape(arrPts);

						CArray<PT_3D,PT_3D> arrPts2;
						arrPts2.SetSize(arrPts.GetSize());
						for( int i=0; i<arrPts.GetSize(); i++)
						{
							arrPts2[i] = arrPts[i];
						}

						//自动生成的图廓，图廓节点顺序是正确的
						CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(pFtr);
						if( stricmp(pLayer->GetName(),"TK_inner_bound")==0 )
						{
							m_bSortBound = FALSE;
						}

						m_pDlg->SetBound(arrPts2.GetData());

						m_pDlg->ShowWindow(SW_SHOW);
					}
				}
			}

			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();

			//修复选择内图廓后，窗口没有发生变化的Bug
			CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
			m_MapDecorator.CalcMapBound();
			Envelope e;
			e.CreateFromPts(m_MapDecorator.m_SelectedBoundPts,4);
			m_MapDecorator.CalcMapNumberAndNearmap(pDS,PT_3D(e.CenterX(),e.CenterY(),0));
			
			m_pDlg->m_strMapNum = ((MapDecorate::CNumber*)m_MapDecorator.GetObj("Number"))->number;
			m_pDlg->UpdateData(FALSE);

			m_nStep = 2;
		}

		CCommand::PtClick(pt,flag);
	}
	else if( m_nOperation==opSelectCorner )
	{
		if( m_nStep==0 )
		{
			m_pDlg->UpdateData(TRUE);
			float dx = m_pDlg->m_fMapWid*0.01*PDOC(m_pEditor)->GetDlgDataSource()->GetScale();
			float dy = m_pDlg->m_fMapHei*0.01*PDOC(m_pEditor)->GetDlgDataSource()->GetScale();
			pt.x = floor(pt.x/dx+0.5)*dx;
			pt.y = floor(pt.y/dy+0.5)*dy;
			m_pDlg->SetBoundCorner(pt);
			m_nStep = 1;

			m_pDlg->ShowWindow(SW_SHOW);
		}
	}
}


void CMapDecorateCommand::PtReset(PT_3D &pt)
{
	m_nStep = 0;
	if( m_pDlg )
		m_pDlg->ShowWindow(SW_SHOW);
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CBatCreateRectMapBorderCommand,CEditCommand)


CBatCreateRectMapBorderCommand::CBatCreateRectMapBorderCommand()
{
	m_bExtendMap = FALSE;
	m_fHei = 50;
	m_fWid = 50;
}


CBatCreateRectMapBorderCommand::~CBatCreateRectMapBorderCommand()
{
	
}


CString CBatCreateRectMapBorderCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_BATCREATERECTMAPBORDER);
}

void CBatCreateRectMapBorderCommand::Start()
{
	m_bExtendMap = FALSE;
	m_nStep = -1;
	m_fWid = 50;
	m_fHei = 50;
	CEditCommand::Start();
	m_pEditor->OpenSelector(SELMODE_SINGLE);

}


void CBatCreateRectMapBorderCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = m_fWid;
	tab.AddValue("MapWid",&CVariantEx(var));
	var = m_fHei;
	tab.AddValue("MapHei",&CVariantEx(var));

	var = (bool)m_bExtendMap;
	tab.AddValue("ExtendMap",&CVariantEx(var));
}

void CBatCreateRectMapBorderCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	CString cmd;
	
	if( tab.GetValue(0,"MapWid",var) )
	{
		m_fWid = (float)(_variant_t)*var;
	}
	if( tab.GetValue(0,"MapHei",var) )
	{
		m_fHei = (float)(_variant_t)*var;
	}
	if( tab.GetValue(0,"ExtendMap",var) )
	{
		m_bExtendMap = (bool)(_variant_t)*var;
	}

	if( !bInit )
		SetSettingsModifyFlag();

	CEditCommand::SetParams(tab,bInit);
}


void CBatCreateRectMapBorderCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();

		FTR_HANDLE hFtr = pSel->GetLastSelectedObj();
		
		if( !hFtr )return;

		CFeature *pFtr = HandleToFtr(hFtr);
		if( pFtr )
		{
			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo )
			{
				if( pGeo->GetDataPointSum()>0 )
				{
					pGeo->GetShape(m_arrBoundLines);
				}
			}
		}

		CreateMapBorders();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();		

		m_nStep = 2;
		Finish();
	}

	CCommand::PtClick(pt,flag);
}


void CBatCreateRectMapBorderCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("BatCreateRectMapBorderCommand",StrFromResID(IDS_CMDNAME_BATCREATERECTMAPBORDER));
	param->AddParam("MapWid",m_fWid,StrFromResID(IDS_MAPBORDER_WID));
	param->AddParam("MapHei",m_fHei,StrFromResID(IDS_MAPBORDER_HEI));
	param->AddParam("ExtendMap",(bool)m_bExtendMap,StrFromResID(IDS_EXTENDMAP));
}


void CBatCreateRectMapBorderCommand::CreateMapBorder(MapDecorate::CMapDecorator *pMapDecorator, PT_3D *pts, CFtrHArray *pArr)
{
	MapDecorate::CMapDecoratorAssist a;

	a.SetDoc(PDOC(m_pEditor),PDOC(m_pEditor)->GetDlgDataSource()->GetScale());

	CFtrLayer *pLayer;
	CFeature *pFtr = a.CreateCurve("TK_inner_bound",pLayer);
	if( pFtr )
	{
		PT_3DEX pts2[5];
		for( int i=0; i<4; i++)
		{
			pts2[i] = PT_3DEX(pts[i],penLine);
		}
		pts2[4] = pts2[0];
		pFtr->GetGeometry()->CreateShape(pts2,5);

		m_pEditor->AddObject(pFtr,pLayer->GetID());

		pArr->Add(FtrToHandle(pFtr));
	}

	pFtr = a.CreateText("TK_number",pLayer);
	if( pFtr )
	{
		Envelope e;
		e = CreateEnvelopeFromPts(pts,4);
		PT_3D pt1((e.m_xl+e.m_xh)*0.5,(e.m_yl+e.m_yh)*0.5,0);

		PT_3D pt0(e.m_xl,e.m_yl,0);
		CString text = pMapDecorator->CalcRectMapNumber(pt0,PDOC(m_pEditor)->GetDlgDataSource()->GetScale());

		a.SetText((CGeoText*)pFtr->GetGeometry(),&pt1,1,text,StrFromResID(IDS_MD_HEITI),50,0,TAH_MID|TAV_MID);
		m_pEditor->AddObject(pFtr,pLayer->GetID());

		pArr->Add(FtrToHandle(pFtr));
	}
}



void CBatCreateRectMapBorderCommand::PtReset(PT_3D &pt)
{
	CEditCommand::PtReset(pt);
}


void CBatCreateRectMapBorderCommand::CreateMapBorders()
{
	if( m_fWid<=0 || m_fHei<=0 )
		return;
	
	//m_fWid 的单位是cm
	double lfWid = m_fWid*PDOC(m_pEditor)->GetDlgDataSource()->GetScale()*0.01;
	double lfHei = m_fHei*PDOC(m_pEditor)->GetDlgDataSource()->GetScale()*0.01;

	Envelope e;
	e = CreateEnvelopeFromPts(m_arrBoundLines.GetData(),m_arrBoundLines.GetSize());

	e.m_xl = floor(e.m_xl/lfWid)*lfWid; e.m_yl = floor(e.m_yl/lfHei)*lfHei;
	e.m_xh = ceil(e.m_xh/lfWid)*lfWid; e.m_yh = ceil(e.m_yh/lfHei)*lfHei;

	PT_3D mappts[4];

	CArray<PT_3D,PT_3D> arrPts;
	arrPts.SetSize(m_arrBoundLines.GetSize());
	for(int i=0; i<m_arrBoundLines.GetSize(); i++)
	{
		arrPts[i] = m_arrBoundLines[i];
	}


	CUndoFtrs undo(m_pEditor,Name());

	CFtrHArray arrHandles;

	//为了方便比较，用图幅的左下角坐标除以宽高(lfWid,lfHei)来表示图幅
	//范围所覆盖的全部图幅
	CArray<CPoint,CPoint> arrAllMaps;

	//arrExtendMaps存储需要扩展的图幅（边缘图幅），我们从这些图幅的周围8个图幅中提取所需要的外扩图幅
	CArray<CPoint,CPoint> arrExtendMaps;

	MapDecorate::CMapDecorator mapdecorator;
	mapdecorator.InitData(PDOC(m_pEditor)->GetDlgDataSource());
	mapdecorator.LoadParams(PDOC(m_pEditor)->GetDlgDataSource()->GetScale());
	mapdecorator.m_fMapWid = m_fWid;
	mapdecorator.m_fMapHei = m_fHei;

	double x, y;

	if( m_arrBoundLines.GetSize()>4 )
	{
		CGrTrim trim;
		trim.InitTrimPolygon(arrPts.GetData(),arrPts.GetSize(),max(lfWid,lfHei));

		for( y=e.m_yl; y<e.m_yh; y+=lfHei )
		{
			CPoint pt;
			for( x=e.m_xl; x<e.m_xh; x+=lfWid )
			{
				mappts[0].x = x; mappts[0].y = y;
				mappts[1].x = x+lfWid; mappts[1].y = y;
				mappts[2].x = x+lfWid; mappts[2].y = y+lfHei;
				mappts[3].x = x; mappts[3].y = y+lfHei;

				pt.x = FixPreciError(x/lfWid);
				pt.y = FixPreciError(y/lfHei);

				for( int i=0; i<4; i++)
				{
					if( trim.TrimLine(mappts+i,mappts+((i+1)%4)) && trim.GetTrimedLinesNum()>0 )
						break;
				}

				//不相交
				if(i>=4)
				{
					for( i=0; i<arrPts.GetSize(); i++)
					{
						if( GraphAPI::GIsPtInRegion(arrPts[i],mappts,4)>=0 )
							break;
					}
					
					if(i<arrPts.GetSize())
					{
						i = 0;
					}
				}

				if( i<4 )
				{
					arrHandles.RemoveAll();

					CreateMapBorder(&mapdecorator,mappts,&arrHandles);
					undo.arrNewHandles.Append(arrHandles);

					arrAllMaps.Add(pt);
					arrExtendMaps.Add(pt);
				}
			}
		}
	}
	else
	{
		for( y=e.m_yl; y<e.m_yh; y+=lfHei )
		{
			CPoint pt;
			for( x=e.m_xl; x<e.m_xh; x+=lfWid )
			{
				mappts[0].x = x; mappts[0].y = y;
				mappts[1].x = x+lfWid; mappts[1].y = y;
				mappts[2].x = x+lfWid; mappts[2].y = y+lfHei;
				mappts[3].x = x; mappts[3].y = y+lfHei;
				
				pt.x = FixPreciError(x/lfWid);
				pt.y = FixPreciError(y/lfHei);
				
				arrHandles.RemoveAll();
				
				CreateMapBorder(&mapdecorator,mappts,&arrHandles);
				undo.arrNewHandles.Append(arrHandles);
				
				arrAllMaps.Add(pt);
				arrExtendMaps.Add(pt);
			}
		}
	}

	//周围外扩一个图幅
	if( m_bExtendMap )
	{
		CArray<CPoint,CPoint> arrNewMaps;
		CPoint buf[8];
		
		for( int i=0; i<arrExtendMaps.GetSize(); i++)
		{
			CPoint pt = arrExtendMaps[i];
			buf[0] = CPoint(pt.x-1,pt.y-1);
			buf[1] = CPoint(pt.x,pt.y-1);
			buf[2] = CPoint(pt.x+1,pt.y-1);
			buf[3] = CPoint(pt.x-1,pt.y);
			buf[4] = CPoint(pt.x+1,pt.y);
			buf[5] = CPoint(pt.x-1,pt.y+1);
			buf[6] = CPoint(pt.x,pt.y+1);
			buf[7] = CPoint(pt.x+1,pt.y+1);
			
			for( int j=0; j<8; j++)
			{
				for( int k=0; k<arrAllMaps.GetSize(); k++)
				{
					if( buf[j]==arrAllMaps[k] )
						break;
				}
				
				if( k>=arrAllMaps.GetSize() )
				{
					arrNewMaps.Add(buf[j]);
					arrAllMaps.Add(buf[j]);
				}
			}
		}
		
		for( i=0; i<arrNewMaps.GetSize(); i++)
		{
			CPoint pt = arrNewMaps[i];
			
			x = pt.x*lfWid;  y = pt.y*lfHei;  
			mappts[0].x = x; mappts[0].y = y;
			mappts[1].x = x+lfWid; mappts[1].y = y;
			mappts[2].x = x+lfWid; mappts[2].y = y+lfHei;
			mappts[3].x = x; mappts[3].y = y+lfHei;
			
			arrHandles.RemoveAll();
			
			CreateMapBorder(&mapdecorator,mappts,&arrHandles);
			undo.arrNewHandles.Append(arrHandles);
		}
	}


	undo.Commit();
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CBatCreateInclinedMapBorderCommand,CEditCommand)


CBatCreateInclinedMapBorderCommand::CBatCreateInclinedMapBorderCommand()
{

}


CBatCreateInclinedMapBorderCommand::~CBatCreateInclinedMapBorderCommand()
{
	
}


CString CBatCreateInclinedMapBorderCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_BATCREATEINCLINEMAPBORDER);
}

void CBatCreateInclinedMapBorderCommand::Start()
{
	m_nStep = -1;
	m_fWid = 50;
	m_fHei = 50;
	CEditCommand::Start();
	m_pEditor->OpenSelector(SELMODE_SINGLE);

}

void CBatCreateInclinedMapBorderCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = m_fWid;
	tab.AddValue("MapWid",&CVariantEx(var));
	var = m_fHei;
	tab.AddValue("MapHei",&CVariantEx(var));
}


void CBatCreateInclinedMapBorderCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	CString cmd;
	
	if( tab.GetValue(0,"MapWid",var) )
	{
		m_fWid = (float)(_variant_t)*var;
	}
	if( tab.GetValue(0,"MapHei",var) )
	{
		m_fHei = (float)(_variant_t)*var;
	}

	if( !bInit )
		SetSettingsModifyFlag();

	CEditCommand::SetParams(tab,bInit);
}


void CBatCreateInclinedMapBorderCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	if( m_nStep==1 )
	{
		m_arrBoundLines.RemoveAll();
		CSelection* pSel = m_pEditor->GetSelection();

		FTR_HANDLE hFtr = pSel->GetLastSelectedObj();
		
		if( !hFtr )return;

		CFeature *pFtr = HandleToFtr(hFtr);
		if( pFtr )
		{
			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo )
			{
				if( (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface))) && pGeo->GetDataPointSum()>=2 )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeo->GetShape(m_arrBoundLines);
				}
			}
		}

		if( m_arrBoundLines.GetSize()<2 )
			return;

		CreateMapBorders();

		m_arrBoundLines.RemoveAll();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();		

		m_nStep = 2;
		Finish();
	}

	CCommand::PtClick(pt,flag);
}


void CBatCreateInclinedMapBorderCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("BatCreateRectMapBorderCommand",StrFromResID(IDS_CMDNAME_BATCREATEINCLINEMAPBORDER));
	param->AddParam("MapWid",m_fWid,StrFromResID(IDS_MAPBORDER_WID));
	param->AddParam("MapHei",m_fHei,StrFromResID(IDS_MAPBORDER_HEI));
}


void CBatCreateInclinedMapBorderCommand::CreateMapBorder(MapDecorate::CMapDecorator *pMapDecorator, PT_3D *pts, CFtrHArray *pArr)
{
	MapDecorate::CMapDecoratorAssist a;

	a.SetDoc(PDOC(m_pEditor),PDOC(m_pEditor)->GetDlgDataSource()->GetScale());

	CFtrLayer *pLayer;
	CFeature *pFtr = a.CreateCurve("TK_inner_bound",pLayer);
	if( pFtr )
	{
		PT_3DEX pts2[5];
		for( int i=0; i<4; i++)
		{
			pts2[i] = PT_3DEX(pts[(i+1)%4],penLine);
		}
		pts2[4] = pts2[0];
		pFtr->GetGeometry()->CreateShape(pts2,5);

		m_pEditor->AddObject(pFtr,pLayer->GetID());

		pArr->Add(FtrToHandle(pFtr));
	}

	pFtr = a.CreateText("TK_number",pLayer);
	if( pFtr )
	{
		Envelope e;
		e = CreateEnvelopeFromPts(pts,4);
		PT_3D pt((e.m_xl+e.m_xh)*0.5,(e.m_yl+e.m_yh)*0.5,0);
		CString text;

		text = pMapDecorator->CalcRectMapNumber(pt,PDOC(m_pEditor)->GetDlgDataSource()->GetScale());

		a.SetText((CGeoText*)pFtr->GetGeometry(),&pt,1,text,StrFromResID(IDS_MD_HEITI),50,0,TAH_MID|TAV_MID);
		m_pEditor->AddObject(pFtr,pLayer->GetID());

		pArr->Add(FtrToHandle(pFtr));
	}
}



void CBatCreateInclinedMapBorderCommand::PtReset(PT_3D &pt)
{
	CEditCommand::PtReset(pt);
}


void CBatCreateInclinedMapBorderCommand::GetBorderPts(int type, PT_3D pt0, PT_3D ptx, double r, PT_3D *ret_pts)
{
	double lfWid = m_fWid*PDOC(m_pEditor)->GetDlgDataSource()->GetScale()*0.01;
	double lfHei = m_fHei*PDOC(m_pEditor)->GetDlgDataSource()->GetScale()*0.01;

	//pt0 是左上角点, ptx 是右侧边中点
	if( type==0 )
	{
		double cosa = lfWid/r, sina = lfHei*0.5/r;
		double dx = ptx.x-pt0.x, dy = ptx.y-pt0.y;
		double x0 = dx*cosa - dy*sina;
		double y0 = dx*sina + dy*cosa;

		ret_pts[3].x = pt0.x + x0*lfWid/r;
		ret_pts[3].y = pt0.y + y0*lfWid/r;

		ret_pts[0] = pt0;

		dx = ret_pts[3].x-pt0.x;
		dy = ret_pts[3].y-pt0.y;
		ret_pts[1].x = pt0.x + dy*lfHei/lfWid;
		ret_pts[1].y = pt0.y - dx*lfHei/lfWid;

		ret_pts[2].x = ret_pts[1].x+dx;
		ret_pts[2].y = ret_pts[1].y+dy;
	}
	//pt0 是左下角点, ptx 是右侧边中点
	else
	{
		double cosa = lfWid/r, sina = lfHei*0.5/r;
		double dx = ptx.x-pt0.x, dy = ptx.y-pt0.y;
		double x0 = dx*cosa + dy*sina;
		double y0 = -dx*sina + dy*cosa;
		
		ret_pts[2].x = pt0.x + x0*lfWid/r;
		ret_pts[2].y = pt0.y + y0*lfWid/r;
		
		ret_pts[1] = pt0;
		
		dx = ret_pts[2].x-pt0.x;
		dy = ret_pts[2].y-pt0.y;
		ret_pts[0].x = pt0.x - dy*lfHei/lfWid;
		ret_pts[0].y = pt0.y + dx*lfHei/lfWid;
		
		ret_pts[3].x = ret_pts[0].x+dx;
		ret_pts[3].y = ret_pts[0].y+dy;		
	}
}


BOOL CBatCreateInclinedMapBorderCommand::FindPtX(PT_3D pt0, double r, double& len, PT_3D *pts, int npt, CArray<double,double>& lens, PT_3D& ret)
{
	for( int i=0; i<npt; i++)
	{
		if( len<lens[i] )
			break;
	}

	double a, b, c, delta;
	double k1, k2;
	double t1, t2;

	//使用直线的参数代入圆的方程，得到一元二次方程 r^2 = (x1+k1*t-x0)^2 + (y1+k2*t-y0)^2；以下为求解过程
	for( i=i-1; i<npt-1; i++)
	{
		k1 = pts[i+1].x-pts[i].x;
		k2 = pts[i+1].y-pts[i].y;
		a = k1*k1 + k2*k2;
		c = (pts[i].x-pt0.x)*(pts[i].x-pt0.x) + (pts[i].y-pt0.y)*(pts[i].y-pt0.y) - r*r;
		b = 2*k1*(pts[i].x-pt0.x) + 2*k2*(pts[i].y-pt0.y);

		delta = b*b-4*a*c;

		//无解
		if( delta<0 )
			continue;

		delta = sqrt(delta);

		t1 = (-b+delta)*0.5/a; 
		t2 = (-b-delta)*0.5/a; 

		int way = 0;

		//最后一个点可以放在延长线上
		if( i==(npt-2) )
		{
			if( (t1<0 ) && (t2<0 ) )
				continue;

			if( (t1>=0 ) && (t2>=0 ) )
			{
				if( t1<=t2 )
					way = 1;
			}
			else if( (t2>=0 ) )
				way = 1;
		}
		else
		{
			if( (t1<0 || t1>1.0) && (t2<0 || t2>1.0) )
				continue;
			
			if( (t1>=0 && t1<=1.0) && (t2>=0 && t2<=1.0) )
			{
				if( t1<=t2 )
					way = 1;
			}
			else if( (t2>=0 && t2<=1.0) )
				way = 1;
		}

		if( way==0 )
		{
			ret.x = k1*t1 + pts[i].x;
			ret.y = k2*t1 + pts[i].y;
			ret.z = 0;
		}
		else
		{
			ret.x = k1*t2 + pts[i].x;
			ret.y = k2*t2 + pts[i].y;
			ret.z = 0;
		}

		double len2 = lens[i] + (way==0?t1:t2)*sqrt(a);
		if( len2<len )
			continue;

		len = len2;

		return TRUE;
	}

	return FALSE;	
}


void CBatCreateInclinedMapBorderCommand::CreateMapBorders()
{
	if( m_fWid<=0 || m_fHei<=0 || m_arrBoundLines.GetSize()<2 )
		return;
	
	//m_fWid 的单位是cm
	double lfWid = m_fWid*PDOC(m_pEditor)->GetDlgDataSource()->GetScale()*0.01;
	double lfHei = m_fHei*PDOC(m_pEditor)->GetDlgDataSource()->GetScale()*0.01;

	CArray<PT_3D,PT_3D> arrPts;
	arrPts.SetSize(m_arrBoundLines.GetSize());
	for(int i=0; i<m_arrBoundLines.GetSize(); i++)
	{
		arrPts[i] = m_arrBoundLines[i];
	}

	int nBasePt = GraphAPI::GKickoffSamePoints(arrPts.GetData(),arrPts.GetSize());
	arrPts.SetSize(nBasePt);

	PT_3D *basePts = arrPts.GetData();

	CUndoFtrs undo(m_pEditor,Name());
	CFtrHArray arr;
	
	MapDecorate::CMapDecorator mapdecorator;
	mapdecorator.InitData(PDOC(m_pEditor)->GetDlgDataSource());
	mapdecorator.LoadParams(PDOC(m_pEditor)->GetDlgDataSource()->GetScale());
	mapdecorator.m_fMapWid = m_fWid;
	mapdecorator.m_fMapHei = m_fHei;

	//创建第一个矩形
	PT_3D line[2];
	GraphAPI::GGetParallelLine(basePts,2,lfHei*0.5,line);

	PT_3D mappts[4];
	mappts[0] = line[0];
	mappts[1].x = 2*basePts[0].x - line[0].x; mappts[1].y = 2*basePts[0].y - line[0].y;
	mappts[2].x = mappts[1].x + (line[1].x-line[0].x)*lfWid/GraphAPI::GGet2DDisOf2P(basePts[0],basePts[1]);
	mappts[2].y = mappts[1].y + (line[1].y-line[0].y)*lfWid/GraphAPI::GGet2DDisOf2P(basePts[0],basePts[1]);
	mappts[3].x = mappts[2].x + mappts[0].x-mappts[1].x; mappts[3].y = mappts[2].y + mappts[0].y-mappts[1].y;

	CreateMapBorder(&mapdecorator,mappts,&arr);
	undo.arrNewHandles.Append(arr);

	//计算各个线段的长度
	double len = lfWid;
	CArray<double,double> lens;
	lens.SetSize(nBasePt);
	lens[0] = 0;

	for( i=1; i<nBasePt; i++)
	{
		lens[i] = lens[i-1] + GraphAPI::GGet2DDisOf2P(basePts[i-1],basePts[i]);		
	}

	PT_3D testPts[3], tpts[4];

	//从当前点开始，计算共mappts[3]点、且右侧边中点落在 arrPts 上的矩形
	PT_3D pt0 = mappts[3], ret;
	double r = sqrt(lfWid*lfWid + lfHei*lfHei*0.25);
	
	while( 1 )
	{
		pt0 = mappts[3];

		int type = 0;
		double len0 = len;
		if( !FindPtX(pt0,r,len,basePts,nBasePt,lens,ret) )
			break;

		GetBorderPts(type,pt0,ret,r,tpts);

		testPts[0] = mappts[0];
		testPts[1] = mappts[3];
		testPts[2] = tpts[3];

		if( !GraphAPI::GIsClockwise(testPts,3) )
		{
			pt0 = mappts[2];
			type = 1;
			len = len0;
			if( !FindPtX(pt0,r,len,basePts,nBasePt,lens,ret) )
				break;
		}

		arr.RemoveAll();

		GetBorderPts(type,pt0,ret,r,mappts);
		CreateMapBorder(&mapdecorator,mappts,&arr);
		undo.arrNewHandles.Append(arr);
	}

	undo.Commit();
}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CBatCreateRoadMapBorderCommand,CEditCommand)


CBatCreateRoadMapBorderCommand::CBatCreateRoadMapBorderCommand()
{

}


CBatCreateRoadMapBorderCommand::~CBatCreateRoadMapBorderCommand()
{
	
}


CString CBatCreateRoadMapBorderCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_BATCREATEROADMAPBORDER);
}

void CBatCreateRoadMapBorderCommand::Start()
{
	m_nStep = -1;
	m_fWid = 780;
	m_fHei = 574;
	m_fMileageInterval = 700;
	m_lfMileageStart = 0;
	m_fMileageTextSize = 8;

	CEditCommand::Start();
	m_pEditor->OpenSelector(SELMODE_SINGLE);

}


void CBatCreateRoadMapBorderCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = m_fWid;
	tab.AddValue("MapWid",&CVariantEx(var));
	var = m_fHei;
	tab.AddValue("MapHei",&CVariantEx(var));
	var = m_fMileageInterval;
	tab.AddValue("MileageInterval",&CVariantEx(var));
	var = m_lfMileageStart;
	tab.AddValue("MileageStart",&CVariantEx(var));
	var = m_fMileageTextSize;
	tab.AddValue("MileageTextSize",&CVariantEx(var));
}

void CBatCreateRoadMapBorderCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	CString cmd;
	
	if( tab.GetValue(0,"MapWid",var) )
	{
		m_fWid = (float)(_variant_t)*var;
	}
	if( tab.GetValue(0,"MapHei",var) )
	{
		m_fHei = (float)(_variant_t)*var;
	}
	if( tab.GetValue(0,"MileageInterval",var) )
	{
		m_fMileageInterval = (float)(_variant_t)*var;
	}
	if( tab.GetValue(0,"MileageStart",var) )
	{
		m_lfMileageStart = (double)(_variant_t)*var;
	}
	if( tab.GetValue(0,"MileageTextSize",var) )
	{
		m_fMileageTextSize = (float)(_variant_t)*var;
	}

	if( !bInit )
		SetSettingsModifyFlag();

	CEditCommand::SetParams(tab,bInit);
}


void CBatCreateRoadMapBorderCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("BatCreateRoadMapBorderCommand",StrFromLocalResID(IDS_CMDNAME_BATCREATEROADMAPBORDER));
	param->AddParam("MapWid",m_fWid,StrFromResID(IDS_MAPBORDER_WID));
	param->AddParam("MapHei",m_fHei,StrFromResID(IDS_MAPBORDER_HEI));
	
	param->AddParam("MileageInterval",m_fMileageInterval,StrFromResID(IDS_MILEAGE_INTERVAL));
	param->AddParam("MileageStart",m_lfMileageStart,StrFromResID(IDS_MILEAGE_START));
	param->AddParam("MileageTextSize",m_fMileageTextSize,StrFromResID(IDS_MILEAGE_FONTSIZE));
	
}


void CBatCreateRoadMapBorderCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();

		FTR_HANDLE hFtr = pSel->GetLastSelectedObj();
		
		if( !hFtr )return;

		CFeature *pFtr = HandleToFtr(hFtr);
		if( pFtr )
		{
			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo )
			{
				if( (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface))) && pGeo->GetDataPointSum()>=2 )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeo->GetShape(m_arrBoundLines);
				}
			}
		}

		if( m_arrBoundLines.GetSize()<2 )
			return;

		CreateMapBorders();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();		

		m_nStep = 2;
		Finish();
	}

	CCommand::PtClick(pt,flag);
}



void CBatCreateRoadMapBorderCommand::CreateMapBorder(MapDecorate::CMapDecorator *pMapDecorator,PT_3D *pts, CFtrHArray *pArr)
{
	MapDecorate::CMapDecoratorAssist a;

	a.SetDoc(PDOC(m_pEditor),PDOC(m_pEditor)->GetDlgDataSource()->GetScale());

	CFtrLayer *pLayer;
	CFeature *pFtr = a.CreateCurve("TK_inner_bound",pLayer);
	if( pFtr )
	{
		PT_3DEX pts2[5];
		for( int i=0; i<4; i++)
		{
			pts2[i] = PT_3DEX(pts[(i+1)%4],penLine);
		}
		pts2[4] = pts2[0];
		pFtr->GetGeometry()->CreateShape(pts2,5);

		m_pEditor->AddObject(pFtr,pLayer->GetID());
		pArr->Add(FtrToHandle(pFtr));
	}

	pFtr = a.CreateText("TK_number",pLayer);
	if( pFtr )
	{
		Envelope e;
		e = CreateEnvelopeFromPts(pts,4);
		PT_3D pt((e.m_xl+e.m_xh)*0.5,(e.m_yl+e.m_yh)*0.5,0);
		CString text;
		text = pMapDecorator->CalcRectMapNumber(pt,PDOC(m_pEditor)->GetDlgDataSource()->GetScale());

		a.SetText((CGeoText*)pFtr->GetGeometry(),&pt,1,text,StrFromResID(IDS_MAPBORDER_HEI),50,0,TAH_MID|TAV_MID);
		m_pEditor->AddObject(pFtr,pLayer->GetID());

		pArr->Add(FtrToHandle(pFtr));
	}
}



void CBatCreateRoadMapBorderCommand::PtReset(PT_3D &pt)
{
	CEditCommand::PtReset(pt);
}


void CBatCreateRoadMapBorderCommand::CreateMapBorders()
{
	if( m_fWid<=0 || m_fHei<=0 || m_arrBoundLines.GetSize()<2 )
		return;
	
	//m_fWid 的单位是cm
	double lfWid = m_fWid;
	double lfHei = m_fHei;
	
	CArray<PT_3D,PT_3D> arrPts;
	arrPts.SetSize(m_arrBoundLines.GetSize());
	for(int i=0; i<m_arrBoundLines.GetSize(); i++)
	{
		arrPts[i] = m_arrBoundLines[i];
	}
	
	int nBasePt = GraphAPI::GKickoffSamePoints(arrPts.GetData(),arrPts.GetSize());
	arrPts.SetSize(nBasePt);
	
	PT_3D *basePts = arrPts.GetData();
	
	PT_3D mappts[4];
	
	//计算各个线段的长度
	double len = m_fMileageInterval;
	CArray<double,double> lens;
	lens.SetSize(nBasePt);
	lens[0] = 0;
	
	for( i=1; i<nBasePt; i++)
	{
		lens[i] = lens[i-1] + GraphAPI::GGet2DDisOf2P(basePts[i-1],basePts[i]);		
	}

	PT_3D pt0 = basePts[0];
	PT_3D pt1, dir0, dir1;
	int start = 1, nCurpt;

	dir0 = PT_3D(basePts[1].x-basePts[0].x,basePts[1].y-basePts[0].y,0);

	CUndoFtrs undo(m_pEditor,Name());
	CFtrHArray arr;

	MapDecorate::CMapDecorator mapdecorator;
	mapdecorator.InitData(PDOC(m_pEditor)->GetDlgDataSource());
	mapdecorator.LoadParams(PDOC(m_pEditor)->GetDlgDataSource()->GetScale());
	mapdecorator.m_fMapWid = m_fWid;
	mapdecorator.m_fMapHei = m_fHei;

	while( 1 )
	{
		nCurpt = GetPosPt(basePts,nBasePt,lens,len,pt1);
		GetMapBorder(pt0,pt1,mappts);
		CreateMapBorder(&mapdecorator,mappts,&arr);

		dir1 = PT_3D(basePts[nCurpt+1].x-basePts[nCurpt].x,basePts[nCurpt+1].y-basePts[nCurpt].y,0);

		if( start )
		{
			CreatePartingLine(pt0,dir0,pt1,dir1,1,0,&arr); //pt0处一条分割线+前面的里程注记
			start = 0;
		}

		if( len>=lens[nBasePt-1] )
		{
			CreatePartingLine(pt0,dir0,pt1,dir1,2,lens[nBasePt-1],&arr); //pt1处一条分割线+后面的里程注记
			break;
		}
		
		CreatePartingLine(pt0,dir0,pt1,dir1,3,len,&arr); //pt1处一条分割线+前后两个里程注记

		pt0 = pt1;
		dir0 = dir1;
		len += m_fMileageInterval;
	}

	undo.arrNewHandles.Append(arr);
	undo.Commit();
}


int CBatCreateRoadMapBorderCommand::GetPosPt(PT_3D *pts, int npt, CArray<double,double>& lens, double len, PT_3D& ret)
{
	for(int i=0; i<npt; i++)
	{
		if( len<lens[i] )
			break;
	}
	if( i>=npt )
	{
		ret = pts[npt-1];
		return i-2;
	}
	else
	{
		double k = (len-lens[i-1])/(lens[i]-lens[i-1]);
		ret.x = pts[i-1].x + k*(pts[i].x-pts[i-1].x);
		ret.y = pts[i-1].y + k*(pts[i].y-pts[i-1].y);	
		
		return i-1;
	}	
}

void CBatCreateRoadMapBorderCommand::GetMapBorder(PT_3D pt0, PT_3D pt1, PT_3D *ret_pts)
{
	double r = GraphAPI::GGet2DDisOf2P(pt0,pt1);
	double dr = (m_fWid-r)*0.5;

	PT_3D pt2, pt3;

	pt2.x = pt0.x - (pt1.x-pt0.x)*dr/r;
	pt2.y = pt0.y - (pt1.y-pt0.y)*dr/r;

	pt3.x = pt1.x - (pt0.x-pt1.x)*dr/r;
	pt3.y = pt1.y - (pt0.y-pt1.y)*dr/r;

	double dx = pt3.x-pt2.x, dy = pt3.y-pt2.y;
	ret_pts[0].x = pt2.x - dy*m_fHei*0.5/m_fWid;
	ret_pts[0].y = pt2.y + dx*m_fHei*0.5/m_fWid;

	ret_pts[1].x = 2*pt2.x - ret_pts[0].x;
	ret_pts[1].y = 2*pt2.y - ret_pts[0].y;

	ret_pts[3].x = ret_pts[0].x + dx;
	ret_pts[3].y = ret_pts[0].y + dy;

	ret_pts[2].x = ret_pts[1].x + dx;
	ret_pts[2].y = ret_pts[1].y + dy;
}


void CBatCreateRoadMapBorderCommand::CreatePartingLine(PT_3D pt0, PT_3D dir0, PT_3D pt1, PT_3D dir1, int type, double len, CFtrHArray *arr)
{
	double lineLen = 200;

	PT_3DEX expts[2];
	
	double dx = dir1.x, dy = dir1.y;
	double r = sqrt(dx*dx + dy*dy);

	MapDecorate::CMapDecoratorAssist a;
	CFtrLayer *pLayer;
	CFeature *pFtr;
	
	PT_3D pt = pt1;

	// pt0 处画直线
	if( type==1 )
	{
		pt = pt0;

		dx = dir0.x, dy = dir0.y;
		r = sqrt(dx*dx + dy*dy);
	}

	a.SetDoc(PDOC(m_pEditor),PDOC(m_pEditor)->GetDlgDataSource()->GetScale());

	pFtr = a.CreateCurve("TK_partingLine",pLayer);
	if( !pFtr )return;

	CGeoCurve *pGeo = (CGeoCurve*)pFtr->GetGeometry();
	if( pGeo )
	{
		expts[0].x = pt.x + dy*lineLen*0.5/r; 
		expts[0].y = pt.y - dx*lineLen*0.5/r; 
		expts[0].pencode = penLine;

		expts[1].x = 2*pt.x - expts[0].x; 
		expts[1].y = 2*pt.y - expts[0].y; 
		expts[1].pencode = penLine;

		pGeo->CreateShape(expts,2);

		m_pEditor->AddObject(pFtr,pLayer->GetID());
		arr->Add(FtrToHandle(pFtr));
	}

	len += m_lfMileageStart;

	//添加注记
	CString text;
	text.Format("K%d+%.3f",(int)floor(len/1000), len-floor(len/1000)*1000);

	if( type==1 || type==3 )
	{
		pFtr = a.CreateText("TK_partingLine", pLayer);
		if( !pFtr )
			return;
		
		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
		
		a.SetText(pText,&pt,1,text,StrFromResID(IDS_MD_SONGTI),m_fMileageTextSize,0,TAH_LEFT|TAV_BOTTOM);

		TEXT_SETTINGS0 sets;
		pText->GetSettings(&sets);
		sets.nPlaceType = byLineH;
		pText->SetSettings(&sets);
		
		Envelope e = a.GetTextEnvelope(pText);
		double wid = e.Width()*0.5;
		
		expts[0].x = pt.x + dy*wid/r; 
		expts[0].y = pt.y - dx*wid/r; 
		expts[0].pencode = penLine;
		
		expts[1].x = 2*pt.x - expts[0].x; 
		expts[1].y = 2*pt.y - expts[0].y; 
		expts[1].pencode = penLine;
		
		pText->CreateShape(expts,2);
		m_pEditor->AddObject(pFtr);

		arr->Add(FtrToHandle(pFtr));
	}

	if( type==2 || type==3 )
	{
		pFtr = a.CreateText("TK_partingLine", pLayer);
		if( !pFtr )
			return;
		
		CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
		
		a.SetText(pText,&pt,1,text,StrFromResID(IDS_MD_SONGTI),m_fMileageTextSize,0,TAH_LEFT|TAV_TOP);

		TEXT_SETTINGS0 sets;
		pText->GetSettings(&sets);
		sets.nPlaceType = byLineH;
		pText->SetSettings(&sets);

		Envelope e = a.GetTextEnvelope(pText);
		double wid = e.Width()*0.5;
		
		expts[0].x = pt.x + dy*wid/r; 
		expts[0].y = pt.y - dx*wid/r; 
		expts[0].pencode = penLine;
		
		expts[1].x = 2*pt.x - expts[0].x; 
		expts[1].y = 2*pt.y - expts[0].y; 
		expts[1].pencode = penLine;
		
		pText->CreateShape(expts,2);
		m_pEditor->AddObject(pFtr);

		arr->Add(FtrToHandle(pFtr));
	}
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CBatCreateTrapezoidMapBorderCommand,CEditCommand)


CBatCreateTrapezoidMapBorderCommand::CBatCreateTrapezoidMapBorderCommand()
{
	m_bExtendMap = FALSE;
}


CBatCreateTrapezoidMapBorderCommand::~CBatCreateTrapezoidMapBorderCommand()
{
	
}


CString CBatCreateTrapezoidMapBorderCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_BATCREATETRAPEZOIDMAPBORDER);
}

void CBatCreateTrapezoidMapBorderCommand::Start()
{
	m_nStep = -1;
	CEditCommand::Start();
	m_pEditor->OpenSelector(SELMODE_SINGLE);

}


void CBatCreateTrapezoidMapBorderCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);

	_variant_t var;
	
	var = (bool)m_bExtendMap;
	tab.AddValue("ExtendMap",&CVariantEx(var));
}

void CBatCreateTrapezoidMapBorderCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"ExtendMap",var) )
	{
		m_bExtendMap = (bool)(_variant_t)*var;
	}

	CEditCommand::SetParams(tab,bInit);
}


void CBatCreateTrapezoidMapBorderCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();

		FTR_HANDLE hFtr = pSel->GetLastSelectedObj();
		CFeature *pFtr = HandleToFtr(hFtr);
		if (!pFtr) return;
		CGeometry *pGeo = pFtr->GetGeometry();
		if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			return;

		pGeo->GetShape(m_arrBoundLines);
		if (m_arrBoundLines.GetSize() < 2)
			return;

		CreateMapBorders();

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();		

		m_nStep = 2;
		Finish();

		return;
	}

	CCommand::PtClick(pt,flag);
}


void CBatCreateTrapezoidMapBorderCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("BatCreateTrapezoidMapBorderCommand",StrFromLocalResID(IDS_CMDNAME_BATCREATETRAPEZOIDMAPBORDER));
	param->AddParam("ExtendMap",(bool)m_bExtendMap,StrFromResID(IDS_EXTENDMAP));
}


void CBatCreateTrapezoidMapBorderCommand::CreateMapBorder(LPCTSTR name, PT_3D *pts, CFtrHArray *pArr)
{
	MapDecorate::CMapDecoratorAssist a;

	a.SetDoc(PDOC(m_pEditor),PDOC(m_pEditor)->GetDlgDataSource()->GetScale());

	CFtrLayer *pLayer;
	CFeature *pFtr = a.CreateCurve("TK_inner_bound",pLayer);
	if( pFtr )
	{
		PT_3DEX pts2[5];
		for( int i=0; i<4; i++)
		{
			pts2[i] = PT_3DEX(pts[i],penLine);
		}
		pts2[4] = pts2[0];
		pFtr->GetGeometry()->CreateShape(pts2,5);

		m_pEditor->AddObject(pFtr,pLayer->GetID());
		pArr->Add(FtrToHandle(pFtr));
	}

	pFtr = a.CreateText("TK_number",pLayer);
	if( pFtr )
	{
		Envelope e;
		e = CreateEnvelopeFromPts(pts,4);
		PT_3D pt1((e.m_xl+e.m_xh)*0.5,(e.m_yl+e.m_yh)*0.5,0);

		a.SetText((CGeoText*)pFtr->GetGeometry(),&pt1,1,name,StrFromResID(IDS_MD_HEITI),50,0,TAH_MID|TAV_MID);
		m_pEditor->AddObject(pFtr,pLayer->GetID());
		pArr->Add(FtrToHandle(pFtr));
	}
}



void CBatCreateTrapezoidMapBorderCommand::PtReset(PT_3D &pt)
{
	CEditCommand::PtReset(pt);
}


void CBatCreateTrapezoidMapBorderCommand::Finish()
{
	CEditCommand::Finish();

	m_nExitCode = CMPEC_NULL;
}


void CBatCreateTrapezoidMapBorderCommand::Abort()
{
	CEditCommand::Abort();

	m_nExitCode = CMPEC_NULL;
}



void CBatCreateTrapezoidMapBorderCommand::CreateMapBorders()
{
	CDlgDoc *pDoc = PDOC(m_pEditor);
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();

	TMProjectionZone prj = pDS->GetProjection();
	TMDatum datum = pDS->GetDatum();

	if( strlen(datum.tmName)<=0 )
	{
		AfxMessageBox(StrFromResID(IDS_TIP_SETMAPINFO));
		return;
	}

	int nScale = pDS->GetScale();
	if( nScale<5000 )
	{
		AfxMessageBox(StrFromResID(IDS_TIP_WRONGSCALE));
		return;
	}

	CTM tm;
	tm.SetDatum(&datum);
	tm.SetZone(&prj);

	Envelope e0;
	e0 = CreateEnvelopeFromPts(m_arrBoundLines.GetData(),m_arrBoundLines.GetSize());

	PT_3D pts[4];
	tm.ConvertToGeodetic(e0.m_xl,e0.m_yl,&pts[0].y,&pts[0].x);
	tm.ConvertToGeodetic(e0.m_xh,e0.m_yl,&pts[1].y,&pts[1].x);
	tm.ConvertToGeodetic(e0.m_xh,e0.m_yh,&pts[2].y,&pts[2].x);
	tm.ConvertToGeodetic(e0.m_xl,e0.m_yh,&pts[3].y,&pts[3].x);
	
	char mapName[256];
	
	CNMapIndex mapIdxs[4];
	memset(mapIdxs,0,sizeof(mapIdxs));
		
	for( int i=0; i<4; i++)
	{
		pts[i].x *= 180/PI;
		pts[i].y *= 180/PI;

		CMapper::CNMapCoordToName(pts[i].x,pts[i].y,nScale,mapName);
		CMapper::CNMapNameToIndex(mapName,mapIdxs[i]);
	}

	int maxIndex = 0;//最大编号
	CMapper::ScaleToRange(nScale, NULL, NULL, &maxIndex);

	CMapper mapper;
	mapper.SetDatum(&datum);
	mapper.SetZone(&prj);

	CUndoFtrs undo(m_pEditor, Name());
	CFtrHArray arrHandles;

	//为了方便比较，用图幅的图幅号(i,j)来表示图幅
	//范围所覆盖的全部图幅
	CArray<CPoint, CPoint> arrAllMaps;

	//arrExtendMaps存储需要扩展的图幅（边缘图幅），我们从这些图幅的周围8个图幅中提取所需要的外扩图幅
	CArray<CPoint, CPoint> arrExtendMaps;

	CNMapIndex mapIdx = mapIdxs[0];
	double xys[8];
	PT_3D pts2[4];

	for(int m = mapIdxs[0].i100wColumn; m<=mapIdxs[2].i100wColumn; m++)
	{
		mapIdx.i100wColumn = m;
		for(int n = mapIdxs[0].i100wRow; n<=mapIdxs[2].i100wRow; n++)
		{
			mapIdx.i100wRow = n;
			int rmin = 0;
			int rmax = maxIndex;
			int cmin = 0;
			int cmax = maxIndex;

			if(m==mapIdxs[0].i100wColumn)
			{
				cmin = mapIdxs[0].iColumn;
			}

			if(m==mapIdxs[2].i100wColumn)
			{
				cmax = mapIdxs[2].iColumn;
			}

			if(n==mapIdxs[0].i100wRow)
			{
				rmax = mapIdxs[0].iRow;
			}

			if(n==mapIdxs[2].i100wRow)
			{
				rmin = mapIdxs[2].iRow;
			}

			for(i = rmin; i<=rmax; i++)
			{
				for(int j = cmin; j<=cmax; j++)
				{
					mapIdx.iRow = i;
					mapIdx.iColumn = j;

					if(!CMapper::CNMapIndexToName(mapIdx, mapName))continue;
					if(!mapper.CNMapNameToCoord2(mapName, xys))continue;

					BOOL bPtIn = FALSE;
					int num = 0;
					for(int k = 0; k < 4; k++)
					{
						pts2[k].x = xys[2 * k];
						pts2[k].y = xys[2 * k + 1];

						if(2==GraphAPI::GIsPtInRegion(PT_3DEX(pts2[k], penLine), m_arrBoundLines.GetData(), m_arrBoundLines.GetSize()))
						{
							bPtIn = TRUE;
							num++;
						}
					}

					if(!bPtIn)
					{
						for(k = m_arrBoundLines.GetSize() - 1; k >= 0; k--)
						{
							if(2==GraphAPI::GIsPtInRegion((PT_3D)m_arrBoundLines[k], pts2, 4))
							{
								bPtIn = TRUE;
							}
						}
					}

					if(bPtIn)
					{
						CreateMapBorder(mapName, pts2, &arrHandles);

						arrAllMaps.Add(CPoint(i, j));
						if(num!=4)
						{
							arrExtendMaps.Add(CPoint(i, j));
						}
					}
				}
			}
		}
	}

	undo.arrNewHandles.Append(arrHandles);

	//周围外扩一个图幅
	if( m_bExtendMap )
	{
		CArray<CPoint,CPoint> arrNewMaps;
		CPoint buf[8];
		
		for( i=0; i<arrExtendMaps.GetSize(); i++)
		{
			CPoint pt = arrExtendMaps[i];
			buf[0] = CPoint(pt.x-1,pt.y-1);
			buf[1] = CPoint(pt.x,pt.y-1);
			buf[2] = CPoint(pt.x+1,pt.y-1);
			buf[3] = CPoint(pt.x-1,pt.y);
			buf[4] = CPoint(pt.x+1,pt.y);
			buf[5] = CPoint(pt.x-1,pt.y+1);
			buf[6] = CPoint(pt.x,pt.y+1);
			buf[7] = CPoint(pt.x+1,pt.y+1);
			
			for( int j=0; j<8; j++)
			{
				for( int k=0; k<arrAllMaps.GetSize(); k++)
				{
					if( buf[j]==arrAllMaps[k] )
						break;
				}
				
				if( k>=arrAllMaps.GetSize() )
				{
					arrNewMaps.Add(buf[j]);
					arrAllMaps.Add(buf[j]);
				}
			}
		}
		
		for( i=0; i<arrNewMaps.GetSize(); i++)
		{
			CPoint pt = arrNewMaps[i];
			
			mapIdx.iRow = pt.x;
			mapIdx.iColumn = pt.y;
			
			if( !CMapper::CNMapIndexToName(mapIdx,mapName) )continue;
			if( !mapper.CNMapNameToCoord2(mapName,xys) )continue;
			
			for( int k=0; k<4; k++)
			{
				pts2[k].x = xys[2*k];
				pts2[k].y = xys[2*k+1];				
			}
			
			arrHandles.RemoveAll();
			
			CreateMapBorder(mapName,pts2,&arrHandles);
			undo.arrNewHandles.Append(arrHandles);
		}
	}

	undo.Commit();
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CMapDecorateAutoSetNearmapFromFile,CCommand)


CMapDecorateAutoSetNearmapFromFile::CMapDecorateAutoSetNearmapFromFile()
{

}


CMapDecorateAutoSetNearmapFromFile::~CMapDecorateAutoSetNearmapFromFile()
{
	
}


CString CMapDecorateAutoSetNearmapFromFile::Name()
{
	return StrFromResID(IDS_CMDNAME_AutoSetNearmapFromFile);
}



void CMapDecorateAutoSetNearmapFromFile::GetParams(CValueTable& tab)
{
	CCommand::GetParams(tab);
	_variant_t var;
	
	var = (_bstr_t)(LPCTSTR)m_strPathName;
	tab.AddValue("PathName",&CVariantEx(var));
	
	var = (_bstr_t)(LPCTSTR)m_strLayNameOfMapName;
	tab.AddValue("LayNameOfMapName",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayNameOfNearmap;
	tab.AddValue("LayNameOfNearmap",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayNameOfMapNameCorner;
	tab.AddValue("LayNameOfMapNameCorner",&CVariantEx(var));
}

void CMapDecorateAutoSetNearmapFromFile::MyFillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("MapDecorateAutoSetNearmapFromFile",StrFromLocalResID(IDS_CMDNAME_AutoSetNearmapFromFile));
	param->AddFileNameParam("PathName",m_strPathName,StrFromResID(IDS_LOADMDB_FILTER),StrFromResID(IDS_FILENAME));
	param->AddUsedLayerNameParam("LayNameOfMapName",(LPCTSTR)m_strLayNameOfMapName,StrFromResID(IDS_LAYNAME_MAPNAME));
	param->AddUsedLayerNameParam("LayNameOfNearmap",(LPCTSTR)m_strLayNameOfNearmap,StrFromResID(IDS_NEARMAP_LAYER));
	param->AddUsedLayerNameParam("LayNameOfMapNameCorner",(LPCTSTR)m_strLayNameOfMapNameCorner,StrFromResID(IDS_MAPNAME_LAYER_CORNER));
}


void CMapDecorateAutoSetNearmapFromFile::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	CString cmd;
	
	if( tab.GetValue(0,"PathName",var) )
	{
		m_strPathName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"LayNameOfMapName",var) )
	{
		m_strLayNameOfMapName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"LayNameOfNearmap",var) )
	{
		m_strLayNameOfNearmap = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"LayNameOfMapNameCorner",var) )
	{
		m_strLayNameOfMapNameCorner = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}	
	if( !bInit )
		SetSettingsModifyFlag();
	
	CCommand::SetParams(tab,bInit);
}


void CMapDecorateAutoSetNearmapFromFile::Start()
{
	m_strLayNameOfMapName = "TK_number";
	m_strLayNameOfNearmap = "TK_nearmap";
	m_strLayNameOfMapNameCorner = "TK_name1";

	CCommand::Start();	
	
	CDlgOpSettings dlg;
	CUIParam param;
	MyFillShowParams(&param,TRUE);
	dlg.SetUIParam(&param);
	
	if( dlg.DoModal()!=IDOK )
	{
		Abort();
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	CFtrArray arr, arr2;
	FindNameObjects(arr,arr2);
	
	if( arr.GetSize()!=9 )
		return;

	CValueTable tab1;
	dlg.GetValueTable(tab1);
	SetParams(tab1,FALSE);

	CDlgDataSource *pDS0 = PDOC(m_pEditor)->GetDlgDataSource();	

	PT_3D cpt = GetInnerBoundCenter();
	CString names[9];

	GetNamesFromFile(m_strPathName,cpt,names);

	CUndoModifyProperties undo(m_pEditor,Name());

	for( int i=0; i<9; i++)
	{
		CFeature *pFtr = arr[i];
		if( pFtr && !names[i].IsEmpty() )
		{
			CFtrLayer *pLayer = pDS0->GetFtrLayerOfObject(pFtr);
			if( !pLayer )
				continue;
			CGeoText *pText = (CGeoText*)pFtr->GetGeometry();

			CString strOld = pText->GetText();

			m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);

			undo.SetModifyProp(pFtr,FIELDNAME_GEOTEXT_CONTENT,&CVariantEx((_variant_t)(LPCTSTR)strOld),
				&CVariantEx((_variant_t)(LPCTSTR)names[i]));

			pText->SetText(names[i]);

			m_pEditor->RestoreObject(FtrToHandle(pFtr));
		}
	}

	for( i=0; i<arr2.GetSize(); i++)
	{
		CFeature *pFtr = arr2[i];
		if( pFtr && !names[4].IsEmpty() )
		{
			CFtrLayer *pLayer = pDS0->GetFtrLayerOfObject(pFtr);
			if( !pLayer )
				continue;
			CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
			
			CString strOld = pText->GetText();
			
			m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
			
			undo.SetModifyProp(pFtr,FIELDNAME_GEOTEXT_CONTENT,&CVariantEx((_variant_t)(LPCTSTR)strOld),
				&CVariantEx((_variant_t)(LPCTSTR)names[4]));
			
			pText->SetText(names[4]);
			
			m_pEditor->RestoreObject(FtrToHandle(pFtr));
		}
	}

	undo.Commit();

	Finish();

	m_nExitCode = CMPEC_NULL;
}



void CMapDecorateAutoSetNearmapFromFile::FindNameObjects(CFtrArray& arr, CFtrArray& arr2)
{
	//查找图名对象
	CDlgDataSource *pDS0 = PDOC(m_pEditor)->GetDlgDataSource();	
	
	int nLay = pDS0->GetFtrLayerCount();
	
	CFtrLayer *pLayer0;
	
	CFeature *pFtr0;

	arr.SetSize(9);
	memset(arr.GetData(),0,sizeof(CFeature*)*9);

	CArray<NameObjInfo,NameObjInfo> arrObjs;
	NameObjInfo item;

	PT_3D cpt;
	
	for ( int i=0;i<nLay;i++)
	{
		pLayer0 = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer0 || !pLayer0->IsVisible() )continue;
		
		if( stricmp(pLayer0->GetName(),m_strLayNameOfMapName)==0 )
		{
			int nObj = pLayer0->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr0 = pLayer0->GetObject(j);
				if (pFtr0)
				{				
					CGeometry *pGeo = pFtr0->GetGeometry();
					if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
						continue;
					
					arr[4] = pFtr0;
					break;
				}
			}
		}
	

		if( stricmp(pLayer0->GetName(),m_strLayNameOfNearmap)==0 )
		{
			int nObj = pLayer0->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr0 = pLayer0->GetObject(j);
				if (pFtr0)
				{				
					CGeometry *pGeo = pFtr0->GetGeometry();

					if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
					{
						if( pGeo->GetDataPointSum()>=4 )
						{
							pGeo->GetCenter(&PT_3D(),&cpt);
						}
					}
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
					{
						item.pFtr = pFtr0;
						item.pt0 = pGeo->GetDataPoint(0);
						arrObjs.Add(item);
					}
				}
			}
		}

		if( stricmp(pLayer0->GetName(),m_strLayNameOfMapNameCorner)==0 )
		{
			int nObj = pLayer0->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr0 = pLayer0->GetObject(j);
				if (pFtr0)
				{				
					CGeometry *pGeo = pFtr0->GetGeometry();
					if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
					{
						arr2.Add(pFtr0);
					}
				}
			}
		}
	}

	//将接图表中文字对象的节点坐标转换为接图表中心坐标系（如果图廓线不水平，这种转换就是需要的）
	PT_3D boundPts[4], pt1, pt2;
	pDS0->GetBound(boundPts,NULL,NULL);

	double ang = GraphAPI::GGetAngle(boundPts[3].x,boundPts[3].y,boundPts[2].x,boundPts[2].y);
	double ca = cos(ang), sa = sin(ang);

	NameObjInfo *pBuf = arrObjs.GetData();
	int nsize = arrObjs.GetSize();
	for( i=0; i<nsize; i++)
	{
		pt1 = pBuf[i].pt0;

		pt2.x = (pt1.x-cpt.x)*ca - (pt1.y-cpt.y)*sa;
		pt2.y = (pt1.x-cpt.x)*sa + (pt1.y-cpt.y)*ca;

		pBuf[i].pt0 = pt2;
	}

	//排序各个点

	double toler = 1e-3;
	for( i=0; i<nsize; i++)
	{
		if( pBuf[i].pt0.x<-toler && pBuf[i].pt0.y>toler )
		{
			arr[0] = pBuf[i].pFtr;
		}
		else if( fabs(pBuf[i].pt0.x)<toler && pBuf[i].pt0.y>toler )
		{
			arr[1] = pBuf[i].pFtr;
		}
		else if( pBuf[i].pt0.x>toler && pBuf[i].pt0.y>toler )
		{
			arr[2] = pBuf[i].pFtr;
		}
		else if( pBuf[i].pt0.x<-toler && fabs(pBuf[i].pt0.y)<toler )
		{
			arr[3] = pBuf[i].pFtr;
		}
		else if( pBuf[i].pt0.x>toler && fabs(pBuf[i].pt0.y)<toler )
		{
			arr[5] = pBuf[i].pFtr;
		}
		else if( pBuf[i].pt0.x<-toler && pBuf[i].pt0.y<-toler )
		{
			arr[6] = pBuf[i].pFtr;
		}
		else if( fabs(pBuf[i].pt0.x)<toler && pBuf[i].pt0.y<-toler )
		{
			arr[7] = pBuf[i].pFtr;
		}
		else if( pBuf[i].pt0.x>toler && pBuf[i].pt0.y<-toler )
		{
			arr[8] = pBuf[i].pFtr;
		}
	}
}


PT_3D CMapDecorateAutoSetNearmapFromFile::GetInnerBoundCenter()
{
	//查找图名对象
	CDlgDataSource *pDS0 = PDOC(m_pEditor)->GetDlgDataSource();	
	
	int nLay = pDS0->GetFtrLayerCount();
	
	CFtrLayer *pLayer0;
	
	CFeature *pFtr0;
	
	PT_3D cpt;
	
	for ( int i=0;i<nLay;i++)
	{
		pLayer0 = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer0 || !pLayer0->IsVisible() )continue;			
		
		if( stricmp(pLayer0->GetName(),"TK_inner_bound")==0 )
		{
			int nObj = pLayer0->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr0 = pLayer0->GetObject(j);
				if (pFtr0)
				{				
					CGeometry *pGeo = pFtr0->GetGeometry();
					
					if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
					{
						if( pGeo->GetDataPointSum()>=4 )
						{
							pGeo->GetCenter(&PT_3D(),&cpt);
							return cpt;
						}
					}
				}
			}
		}
	}

	return cpt;
}


CDlgDataSource *CMapDecorateAutoSetNearmapFromFile::OpenFDB(LPCTSTR fileName)
{
	CString strPath = fileName;
	
	if( strPath.IsEmpty() )
		return NULL;

	if(strPath.Right(4).CompareNoCase(_T(".fdb")) != 0)
		return NULL;
		
	CDlgDataSource *pDataSource = new CDlgDataSource(NULL);	
	if( !pDataSource )
	{	
		return NULL;
	}
	CSQLiteAccess *pSqlAccess = new CSQLiteAccess;
	if(!pSqlAccess) 
	{
		delete pDataSource;
		return NULL;
	}
	if( !pSqlAccess->Attach(strPath) )
	{
		delete pSqlAccess;
		delete pDataSource;
		return NULL;
	}

	pSqlAccess->ReadDataSourceInfo(pDataSource);
	
	pDataSource->SetAccessObject(pSqlAccess);
	
	pDataSource->LoadAll(NULL,FALSE);
	
	return pDataSource;
}

void CMapDecorateAutoSetNearmapFromFile::GetNamesFromFile(LPCTSTR fileName, PT_3D pt, CString names[9])
{
	CDlgDataSource *pDS0 = OpenFDB(fileName);
	if( !pDS0 )return;

	PT_3D boundPts[4];
	pDS0->GetBound(boundPts,NULL,NULL);
	
	double ang = GraphAPI::GGetAngle(boundPts[3].x,boundPts[3].y,boundPts[2].x,boundPts[2].y);

	//查找图名对象
	CBatExportMapsCommand::GetNearMapName(pDS0,pt,ang,m_strLayNameOfMapName,names);

	delete pDS0;

}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CBatExportMapsCommand::CBatExportMapsCommand()
{
	m_nNameType = typeTextInBound;
	m_nAutoNum = 1;
	m_nExitCode = CMPEC_NULL;

	m_bTrimClosedCurve = FALSE;
	m_bTrimSurface = FALSE;
	m_nNearmapContent = 1;
	m_bExportMapDecorator = FALSE;
}


CBatExportMapsCommand::~CBatExportMapsCommand()
{

}


CString CBatExportMapsCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_BatExportMapsCommand);
}

void CBatExportMapsCommand::Start()
{
	m_strLayNameOfMapName = "TK_name";
	m_strLayNameOfMapNum = "TK_number";
	m_strLayNameOfBound = "TK_inner_bound";
	CCommand::Start();

	m_nAutoNum = 1;

	CDlgDataSource *pDS0 = PDOC(m_pEditor)->GetDlgDataSource();
	if(!pDS0)return;

	if(pDS0 && UVSModify==GetAccessType(pDS0) && FALSE==CUVSModify::CanExport())
	{
		Abort();
		GOutPut(StrFromResID(IDS_NO_PERMISSION));
		m_nExitCode = CMPEC_NULL;
		return;
	}

	MapDecorate::CMapDecorator mapdecorator;
	CDlgBatExportMaps dlg;
	dlg.m_pMapDecorator = &mapdecorator;

	CUIParam param;
	MyFillShowParams(&param,TRUE);
	dlg.SetUIParam(&param);

	if( CMapDecorateCommand::m_bStaticValid )
	{
		dlg.m_pMapDecorator = &CMapDecorateCommand::m_MapDecorator;
	}

	if( dlg.DoModal()!=IDOK )
	{
		Abort();
		m_nExitCode = CMPEC_NULL;
		return;
	}

	CValueTable tab1;
	dlg.GetValueTable(tab1);
	SetParams(tab1,FALSE);

	mapdecorator.InitData(pDS0);
	mapdecorator.LoadParams(pDS0->GetScale());
	mapdecorator.LoadDatas(NULL);

	CFtrLayer *pLayer0;
	CFeature *pFtr0;

	PT_3D pts[4];

	CString strLayName = m_strLayNameOfBound;
	if( strLayName.IsEmpty() )
		strLayName = "TK_inner_bound";
	if( m_strLayNameOfMapNum.IsEmpty() )
		m_strLayNameOfMapNum = "TK_number";
	if( m_strLayNameOfMapName.IsEmpty() )
		m_strLayNameOfMapName = "TK_name";

	int nSum=0;	//图廓框的数量
	int nSum1=0;//其他地物数量
	int nLay = pDS0->GetFtrLayerCount();
	for ( int i=0; i<nLay; i++)
	{
		pLayer0 = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer0 || !pLayer0->IsVisible() )continue;
		
		if( stricmp(pLayer0->GetName(),strLayName)==0 )
		{
			nSum += pLayer0->GetObjectCount();
		}
		else
		{
			nSum1 += pLayer0->GetObjectCount();
		}
	}

	if( nSum<=0 )return;

	CString mapName;
	GProgressStart(nSum*nSum1);
	for ( i=0; i<nLay; i++)
	{
		pLayer0 = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer0 || !pLayer0->IsVisible() )continue;
		
		if( stricmp(pLayer0->GetName(),m_strLayNameOfBound)!=0 )
			continue;

		int nObj = pLayer0->GetObjectCount();		
		for (int j=0;j<nObj;j++)
		{
			pFtr0 = pLayer0->GetObject(j);
			if (pFtr0)
			{
				CGeometry *pGeo = pFtr0->GetGeometry();
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))|| pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					if( pGeo->GetDataPointSum()==5 )
					{
						CArray<PT_3DEX,PT_3DEX> arrPts;
						pGeo->GetShape(arrPts);

						for( int k=0; k<4; k++)
						{
							pts[k] = arrPts[k];
						}

						if( m_nNameType==typeTextInBound )
						{
							mapName = GetMapName(pts,m_strLayNameOfMapNum);
						}
						else
						{
							mapName.Format("%d",m_nAutoNum);
							m_nAutoNum++;
						}

						ExportMap(m_strPathName + "\\" + mapName + ".fdb",pFtr0,pts,dlg.m_pMapDecorator);
					}
				}
			}			
		}
	}
	GProgressEnd();

	Finish();
	GOutPut(StrFromResID(IDS_TIP_END));
	m_nExitCode = CMPEC_NULL;
}


CString CBatExportMapsCommand::GetMapName(PT_3D pts[4], LPCTSTR layName)
{
	CString name;

	CDlgDataSource *pDS0 = PDOC(m_pEditor)->GetDlgDataSource();	
	if(!pDS0)return name;
				
	int nLay = pDS0->GetFtrLayerCount();
	
	CFtrLayer *pLayer0;
	
	CFeature *pFtr0;
	
	for ( int i=0;i<nLay;i++)
	{
		pLayer0 = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer0 || !pLayer0->IsVisible() )continue;
		
		if( stricmp(pLayer0->GetName(),layName)!=0 )
			continue;
		
		int nObj = pLayer0->GetObjectCount();		
		for (int j=0;j<nObj;j++)
		{
			pFtr0 = pLayer0->GetObject(j);
			if (pFtr0)
			{				
				CGeometry *pGeo = pFtr0->GetGeometry();
				if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
					continue;

				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);

				if( GraphAPI::GIsPtInRegion((PT_3D)arrPts[0],pts,4)==2 )
				{
					return ((CGeoText*)pGeo)->GetText();
				}
			}
		}
	}

	return name;
}



BOOL CBatExportMapsCommand::GetNearMapName(CDlgDataSource *pDS0, PT_3D pt0, float ang, LPCTSTR layname, CString names[9])
{
	NameItem item;
	CArray<NameItem,NameItem> arrItems;

	for( int i=0; i<9; i++)
	{
		names[i].Empty();
	}
	
	{		
		int nLay = pDS0->GetFtrLayerCount();
		
		CFtrLayer *pLayer0;
		
		CFeature *pFtr0;
		
		//找到邻近的图幅，并排序
		for ( i=0;i<nLay;i++)
		{
			pLayer0 = pDS0->GetFtrLayerByIndex(i);
			if( !pLayer0 || !pLayer0->IsVisible() )continue;
			
			if( !CUIFCheckListProp::StringFindItem(layname,pLayer0->GetName()) )
				continue;
			
			int nObj = pLayer0->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr0 = pLayer0->GetObject(j);
				if (pFtr0)
				{				
					CGeometry *pGeo = pFtr0->GetGeometry();
					if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
						continue;
					
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeo->GetShape(arrPts);
					
					{
						item.pt0 = arrPts[0];
						item.pGeo = (CGeoText*)pGeo;
						strncpy(item.name,item.pGeo->GetText(),sizeof(item.name)-1);
						item.dis = GraphAPI::GGet2DDisOf2P(item.pt0,pt0);

						for( int k=0; k<arrItems.GetSize(); k++)
						{
							if( item.dis<arrItems[k].dis )
							{
								arrItems.InsertAt(k,item);
								break;
							}
						}
						if( k>=arrItems.GetSize() && arrItems.GetSize()<9 )
						{
							arrItems.Add(item);
						}
						if( arrItems.GetSize()>9 )
						{
							arrItems.RemoveAt(9,arrItems.GetSize()-9);
						}
					}
				}
			}
		}
	}

	if( arrItems.GetSize()<=0 )
		return TRUE;

	//按照倾斜角进行坐标换算，以pt0为中心
	double ca = cos(-ang), sa = sin(-ang);
	for( i=0; i<arrItems.GetSize(); i++)
	{
		item = arrItems[i];
		double x = (item.pt0.x-pt0.x)*ca - (item.pt0.y-pt0.y)*sa + pt0.x;
		double y = (item.pt0.x-pt0.x)*sa + (item.pt0.y-pt0.y)*ca + pt0.y;
		item.pt0.x = x;
		item.pt0.y = y;	
		
		arrItems[i] = item;
	}

	//比较，取出周围8个图号
	float toler = pDS0->GetScale()*0.2;
	NameItem *pItems = arrItems.GetData();
	int nsize = arrItems.GetSize();
	
	for( i=nsize-1; i>=0; i--)
	{
		//左上
		if( pItems[i].pt0.x-pt0.x<-toler && pItems[i].pt0.y-pt0.y>toler )
		{
			names[0] = pItems[i].name;
		}
		//中上
		else if( fabs(pItems[i].pt0.x-pt0.x)<toler && pItems[i].pt0.y-pt0.y>toler )
		{
			names[1] = pItems[i].name;
		}
		//右上
		else if( pItems[i].pt0.x-pt0.x>toler && pItems[i].pt0.y-pt0.y>toler )
		{
			names[2] = pItems[i].name;
		}
		else if( pItems[i].pt0.x-pt0.x<-toler && fabs(pItems[i].pt0.y-pt0.y)<toler )
		{
			names[3] = pItems[i].name;
		}
		else if( fabs(pItems[i].pt0.x-pt0.x)<toler && fabs(pItems[i].pt0.y-pt0.y)<toler )
		{
			names[4] = pItems[i].name;
		}
		else if( pItems[i].pt0.x-pt0.x>toler && fabs(pItems[i].pt0.y-pt0.y)<toler )
		{
			names[5] = pItems[i].name;
		}
		else if( pItems[i].pt0.x-pt0.x<-toler && pItems[i].pt0.y-pt0.y<-toler )
		{
			names[6] = pItems[i].name;
		}
		else if( fabs(pItems[i].pt0.x-pt0.x)<toler && pItems[i].pt0.y-pt0.y<-toler )
		{
			names[7] = pItems[i].name;
		}
		else if( pItems[i].pt0.x-pt0.x>toler && pItems[i].pt0.y-pt0.y<-toler )
		{
			names[8] = pItems[i].name;
		}
	}

	return TRUE;
}


void CBatExportMapsCommand::ExportMap(LPCTSTR fileName, CFeature *pFtr, PT_3D boundPts[4],MapDecorate::CMapDecorator *pMapDecorator)
{
	CDlgDataSource *pDS0 = PDOC(m_pEditor)->GetDlgDataSource();	
	if(!pDS0)return;
	CAttributesSource *pXDS = pDS0->GetXAttributesSource();
	if(!pXDS)return;

	CSQLiteAccess *pSqlAccess;
	CDlgDataSource *pDS1 = OpenFDB(fileName, boundPts, pSqlAccess);	

	if( !pDS1 )return;
	
	int nSum = 0;		
	int nLay = pDS0->GetFtrLayerCount();

	CFtrLayer *pLayer0;
	CValueTable tab;

	CGrTrim trim;
	trim.InitTrimPolygon(boundPts,4,50*pDS0->GetScale()*0.01);

	Envelope evlp;
	evlp.CreateFromPts(boundPts,4);

	CPtrArray arr;
	CFtrArray arr2;

	CFeature *pFtr0, *pFtr1;

	PT_3D boundPts2[5];
	memcpy(boundPts2,boundPts,sizeof(PT_3D)*4);
	boundPts2[4] = boundPts2[0];

	pSqlAccess->BatchUpdateBegin();

	tab.BeginAddValueItem();
	pXDS->GetXAttributes(pFtr,tab);
	tab.EndAddValueItem();

	arr2.Add(pFtr->Clone());
	pLayer0 = pDS0->GetFtrLayerOfObject(pFtr);
	AddFtrsToDS(arr2,pLayer0,pDS0,pDS1,tab);

	for (int i=0;i<nLay;i++)
	{
		pLayer0 = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer0 || !pLayer0->IsVisible() )continue;

		int nObj = pLayer0->GetObjectCount();		
		for (int j=0;j<nObj;j++)
		{
			GProgressStep();
			pFtr0 = pLayer0->GetObject(j);
			if ( pFtr0 && pFtr0!=pFtr )
			{
				arr.RemoveAll();
				arr2.RemoveAll();

				if( stricmp(pLayer0->GetName(),m_strLayNameOfBound)==0 && pFtr0->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
					continue;

				CValueTable xTab;
				xTab.BeginAddValueItem();
				pXDS->GetXAttributes(pFtr0, xTab);
				xTab.EndAddValueItem();

				TrimObj(pFtr0->GetGeometry(),&arr,evlp,boundPts2,5,&trim,m_bTrimClosedCurve,m_bTrimSurface);
				for( int k=0; k<arr.GetSize(); k++)
				{
					pFtr1 = pFtr0->Clone();
					if( !pFtr1 )continue;

					pFtr1->SetGeometry((CGeometry*)arr[k]);
					pFtr1->SetID(OUID());

					arr2.Add(pFtr1);
				}

				//if( strncmp(pLayer0->GetName(),"TK_",3)!=0 )
					nSum += arr2.GetSize();

				AddFtrsToDS(arr2,pLayer0,pDS0,pDS1,xTab);
			}
		}
	}

	//没有地物，就不导出
	if( nSum<=0 )
	{
		pSqlAccess->BatchUpdateEnd();
		delete pDS1;

		::DeleteFile(fileName);

		return;
	}

	if(m_bExportMapDecorator)
	{
		CString names[9];
		double ang = GraphAPI::GGetAngle(boundPts[0].x,boundPts[0].y,boundPts[1].x,boundPts[1].y);

		//使用图名填充结合表
		if( m_nNearmapContent==0 )
			GetNearMapName(pDS0,PT_3D(evlp.CenterX(),evlp.CenterY(),0),ang,m_strLayNameOfMapName,names);
		else
			GetNearMapName(pDS0,PT_3D(evlp.CenterX(),evlp.CenterY(),0),ang,m_strLayNameOfMapNum,names);

		MapDecorate::CName* pName = (MapDecorate::CName*)pMapDecorator->GetObj("Name");
		MapDecorate::CNumber* pNumber = (MapDecorate::CNumber*)pMapDecorator->GetObj("Number");
		MapDecorate::CNearMap* pNearmap = (MapDecorate::CNearMap*)pMapDecorator->GetObj("NearMap");
		
		if (pNumber)
		{
			char fileName1[256], ext1[256];
			_splitpath(fileName, NULL, NULL, fileName1, ext1);
			strncpy(pNumber->number, fileName1, sizeof(pNumber->number) - 1);

			if (strlen(pNumber->number) <= 0)
				strncpy(pNumber->number, StrFromResID(IDS_MD_MAPNUM), sizeof(pNumber->number) - 1);
		}

		if (pName)
		{
			strncpy(pName->name, GetMapName(boundPts, m_strLayNameOfMapName), sizeof(pName->name) - 1);

			if (strlen(pName->name) <= 0)
				strncpy(pName->name, StrFromResID(IDS_MD_MAPNAME), sizeof(pName->name) - 1);
		}

		if (pNearmap)
		{
			i = 0;
			strncpy(pNearmap->nearmap.name1, names[i++], sizeof(pNearmap->nearmap.name1) - 1);
			strncpy(pNearmap->nearmap.name2, names[i++], sizeof(pNearmap->nearmap.name1) - 1);
			strncpy(pNearmap->nearmap.name3, names[i++], sizeof(pNearmap->nearmap.name1) - 1);
			strncpy(pNearmap->nearmap.name4, names[i++], sizeof(pNearmap->nearmap.name1) - 1); i++;
			strncpy(pNearmap->nearmap.name5, names[i++], sizeof(pNearmap->nearmap.name1) - 1);
			strncpy(pNearmap->nearmap.name6, names[i++], sizeof(pNearmap->nearmap.name1) - 1);
			strncpy(pNearmap->nearmap.name7, names[i++], sizeof(pNearmap->nearmap.name1) - 1);
			strncpy(pNearmap->nearmap.name8, names[i++], sizeof(pNearmap->nearmap.name1) - 1);
		}
		
		pMapDecorator->Build(NULL,pDS1,pDS1->GetScale(),TRUE,FALSE);
	}

	pSqlAccess->BatchUpdateEnd();

	delete pDS1;
}


CDlgDataSource *CBatExportMapsCommand::OpenFDB(LPCTSTR fileName, PT_3D pts[4], CSQLiteAccess *& pAccess)
{
	CString strPath = fileName;

	if( strPath.IsEmpty() )
		return NULL;

	if(strPath.Right(4).CompareNoCase(_T(".fdb")) != 0)
		return NULL;

	if(::PathFileExists(strPath))
	{
		if(::DeleteFile(strPath)==FALSE)
			return NULL;
	}
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();	
	if(!pDS)return NULL;

	CDataQueryEx *pDQ = new CDataQueryEx();
	CDlgDataSource *pDataSource = new CDlgDataSource(pDQ);	
	if( !pDataSource )
	{	
		return NULL;
	}
	CSQLiteAccess *pSqlAccess = new CSQLiteAccess;
	if(!pSqlAccess) 
	{
		delete pDataSource;
		return NULL;
	}
	pSqlAccess->Attach(strPath);
	
	pDataSource->SetAccessObject(pSqlAccess);
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	CScheme *pScheme = pCfgLibManager->GetScheme(pDS->GetScale());
	DWORD scale = pCfgLibManager->GetScaleByScheme(pScheme);
	if (scale==0)
	{
		delete pDataSource;
		return NULL;
	}
	pSqlAccess->BatchUpdateBegin();
	pSqlAccess->CreateFileSys(pDS->GetScale(), pScheme);
	pSqlAccess->BatchUpdateEnd();

	pSqlAccess->BatchUpdateBegin();
	
	pDataSource->LoadAll(NULL,FALSE);	
	//写入工作区信息
//	pDataSource->SetScale(pDS->GetScale());
	
	double zmin,zmax;
	PT_3D tpts[4];
	pDS->GetBound(tpts,&zmin,&zmax);
	pDataSource->SetBound(pts,zmin,zmax);
	pDataSource->SetScale(pDS->GetScale());

//	pDataSource->ResetDisplayOrder();
//	pDataSource->SaveAllLayers();

	pSqlAccess->BatchUpdateEnd();

	pAccess = pSqlAccess;

	return pDataSource;
}


BOOL CBatExportMapsCommand::TrimObj(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim, BOOL bClosedCurveAsSurface, BOOL bSurfaceAsCurve)
{
	if (!pObj) return FALSE;	

	//将 polys 改为顺时针
	CArray<PT_3D,PT_3D> arrPts1;
	if( !GraphAPI::GIsClockwise(polys,polysnum) )
	{
		arrPts1.SetSize(polysnum);

		for( int i=0; i<polysnum; i++)
		{
			arrPts1[i] = polys[polysnum-1-i];
		}

		polys = arrPts1.GetData();
	}

	//处理点或者文本
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint))||pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
	{
		PT_3D pt3d;
		PT_3DEX expt;
		expt = pObj->GetDataPoint(0);
		COPY_3DPT(pt3d,expt);
		BOOL bIn = (GraphAPI::GIsPtInRegion(pt3d,polys,polysnum)==2);
		if( bIn )
		{
			CGeometry *pNew = pObj->Clone();
			if( pNew )pArray->Add(pNew);
			return TRUE;
		}
	}
	//处理线
	else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
	{
		if( ((CGeoCurve*)pObj)->IsClosed() && bClosedCurveAsSurface )
		{
			CGeometry *pGeo1 = (CGeometry*)CPermanent::CreatePermanentObject(CLS_GEOSURFACE);
			if( pGeo1==NULL )return FALSE;

			// 复制固定属性
			CValueTable tab;
			tab.BeginAddValueItem();
			pObj->WriteTo(tab);
			tab.EndAddValueItem();
			tab.DelField(FIELDNAME_SHAPE);
			tab.DelField(FIELDNAME_FTRID);
			tab.DelField(FIELDNAME_GEOCLASS);
			pGeo1->ReadFrom(tab);
			
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pObj->GetShape(arrPts);
			pGeo1->CreateShape(arrPts.GetData(),arrPts.GetSize());	

			if( CTrimCommand::TrimSurface(pGeo1,pArray,evlp,polys,polysnum,ptrim,TRUE) )
			{
			}
			else
			{
				pArray->Add(pObj->Clone());
			}

			delete pGeo1;
			return TRUE;

		}
		else if( CTrimCommand::TrimCurve(pObj,pArray,evlp,polys,polysnum,ptrim,TRUE) )
		{
			return TRUE;
		}
		else
		{
			pArray->Add(pObj->Clone());
		}
	}
	//处理面
	else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		if( bSurfaceAsCurve )
		{
			CGeometry *pGeo1 = (CGeometry*)CPermanent::CreatePermanentObject(CLS_GEOCURVE);
			if( pGeo1==NULL )return FALSE;
			
			// 复制固定属性
			CValueTable tab;
			tab.BeginAddValueItem();
			pObj->WriteTo(tab);
			tab.EndAddValueItem();
			tab.DelField(FIELDNAME_SHAPE);
			tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
			tab.DelField(FIELDNAME_FTRID);
			tab.DelField(FIELDNAME_GEOCLASS);
			pGeo1->ReadFrom(tab);
			
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pObj->GetShape(arrPts);
			pGeo1->CreateShape(arrPts.GetData(),arrPts.GetSize());	
			
			if( CTrimCommand::TrimCurve(pGeo1,pArray,evlp,polys,polysnum,ptrim,TRUE) )
			{
			}
			else
			{
				pArray->Add(pObj->Clone());
			}
			
			delete pGeo1;
			return TRUE;
			
		}
		else if( CTrimCommand::TrimSurface(pObj,pArray,evlp,polys,polysnum,ptrim,TRUE) )
		{
			return TRUE;
		}
		else
		{
			pArray->Add(pObj->Clone());
		}
	}

	return TRUE;
}


void CBatExportMapsCommand::AddFtrsToDS(CFtrArray& arr, CFtrLayer *pLayer0, CDlgDataSource *pDS0, CDlgDataSource *pDS1, CValueTable& xTab, CFtrArray *pRetArr)
{
	CFeature *pFtr0;
	CFtrLayer *pLayer1;
	CValueTable tab;

	int nObj = arr.GetSize();

	for (int i=0;i<nObj;i++)
	{	
		pFtr0 = (CFeature*)(arr[i]);			
		if (pFtr0)
		{
			pLayer1 = pDS1->GetFtrLayer(pLayer0->GetName());
			if (pLayer1)
			{
				;
			}
			else
			{
				pLayer1 = new CFtrLayer;
				if(!pLayer1)continue;
				tab.DelAll();
				tab.BeginAddValueItem();
				pLayer0->WriteTo(tab);
				tab.EndAddValueItem();
				pLayer1->ReadFrom(tab);
				pLayer1->SetID(0);
				pDS1->AddFtrLayer(pLayer1);
			}

			if(!pDS1->AddObject(pFtr0,pLayer1->GetID()))
			{
				delete pFtr0;
				continue;
			}

			pDS1->GetXAttributesSource()->SetXAttributes(pFtr0,xTab);

			if( pRetArr )
			{
				pRetArr->Add(pFtr0);
			}
		}			
	}
}



void CBatExportMapsCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strLayNameOfBound;
	tab.AddValue("LayNameOfBound",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayNameOfMapName;
	tab.AddValue("LayNameOfMapName",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayNameOfMapNum;
	tab.AddValue("LayNameOfMapNum",&CVariantEx(var));	

	var = (long)m_nNameType;
	tab.AddValue("NameType",&CVariantEx(var));

	var = (long)m_nNearmapContent;
	tab.AddValue("NearmapContent",&CVariantEx(var));
	
	var = (_bstr_t)(LPCTSTR)m_strPathName;
	tab.AddValue("PathName",&CVariantEx(var));

	var = (bool)(m_bTrimClosedCurve);
	tab.AddValue(PF_TRIMCLOSELINE,&CVariantEx(var));
	
	var = (bool)(m_bTrimSurface);
	tab.AddValue(PF_TRIMSURFACE,&CVariantEx(var));

	var = (bool)(m_bExportMapDecorator);
	tab.AddValue("ExportMapDecorator",&CVariantEx(var));
}

void CBatExportMapsCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	CString cmd;
	
	if( tab.GetValue(0,"LayNameOfBound",var) )
	{
		m_strLayNameOfBound = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"LayNameOfMapName",var) )
	{
		m_strLayNameOfMapName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"LayNameOfMapNum",var) )
	{
		m_strLayNameOfMapNum = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"NameType",var) )
	{
		m_nNameType = (long)(_variant_t)*var;
	}	
	if( tab.GetValue(0,"NearmapContent",var) )
	{
		m_nNearmapContent = (long)(_variant_t)*var;
	}	
	if( tab.GetValue(0,"PathName",var) )
	{
		m_strPathName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,PF_TRIMCLOSELINE,var) )
	{
		m_bTrimClosedCurve = (bool)(_variant_t)*var;		
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_TRIMSURFACE,var) )
	{
		m_bTrimSurface = (bool)(_variant_t)*var;		
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"ExportMapDecorator",var) )
	{
		m_bExportMapDecorator = (bool)(_variant_t)*var;		
		SetSettingsModifyFlag();
	}
	
	if( !bInit )
		SetSettingsModifyFlag();
	
	CEditCommand::SetParams(tab,bInit);
}


void CBatExportMapsCommand::MyFillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("BatExportMapsCommand",StrFromLocalResID(IDS_CMDNAME_BatExportMapsCommand));
	param->AddPathNameParam("PathName",m_strPathName,StrFromResID(IDS_SAVEDIR));
	param->AddUsedLayerNameParam("LayNameOfBound",(LPCTSTR)m_strLayNameOfBound,StrFromResID(IDS_MAPBODER_LAYERNAME));
	param->AddUsedLayerNameParam("LayNameOfMapName",(LPCTSTR)m_strLayNameOfMapName,StrFromResID(IDS_LAYNAME_MAPNAME));
	param->AddUsedLayerNameParam("LayNameOfMapNum",(LPCTSTR)m_strLayNameOfMapNum,StrFromResID(IDS_LAYNAME_MAPNUM));

	param->BeginOptionParam("NearmapContent",StrFromResID(IDS_NEARMAP_CONTENT));
	param->AddOption(StrFromResID(IDS_MD_MAPNAME2),0,0,TRUE);
	param->AddOption(StrFromResID(IDS_MD_MAPNUM2),1,0,TRUE);
	param->EndOptionParam();
	param->SetOptionDefault("NearmapContent",m_nNearmapContent);

	param->BeginOptionParam("NameType",StrFromResID(IDS_FILENAME_FORMAT));
	param->AddOption(StrFromResID(IDS_FILENAME_MAPNUM),0,0,TRUE);
	param->AddOption(StrFromResID(IDS_FILENAME_USEINDEX),1,0,TRUE);
	param->EndOptionParam();
	param->SetOptionDefault("NameType",m_nNameType);

	param->BeginOptionParam(PF_TRIMCLOSELINE,StrFromResID(IDS_CMDPLANE_TRIM_CLOSEDCURVE));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bTrimClosedCurve);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bTrimClosedCurve);
	param->EndOptionParam();
	
	param->BeginOptionParam(PF_TRIMSURFACE,StrFromResID(IDS_CMDPLANE_TRIM_SURFACE));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bTrimSurface);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bTrimSurface);
	param->EndOptionParam();

	param->BeginOptionParam("ExportMapDecorator",StrFromResID(IDS_CMDPLANE_EXPORTMAPDECORATOR));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bExportMapDecorator);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bExportMapDecorator);
	param->EndOptionParam();
}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CBatExportMapsCommand_dxf::CBatExportMapsCommand_dxf()
{
	m_nNameType = typeTextInBound;
	m_nAutoNum = 1;
	m_nExitCode = CMPEC_NULL;
	m_bSymbolized = FALSE;
	m_bAsLayerCode = TRUE;
	m_nFileFormat = 0;
}


CBatExportMapsCommand_dxf::~CBatExportMapsCommand_dxf()
{

}


CString CBatExportMapsCommand_dxf::Name()
{
	return StrFromResID(IDS_CMDNAME_BatExportMapsCommand_dxf);
}

void CBatExportMapsCommand_dxf::Start()
{
	m_strLayNameOfMapNum = "TK_number";
	m_strLayNameOfBound = "TK_inner_bound";

	CCommand::Start();

	m_nAutoNum = 1;

	CDlgDataSource *pDS0 = PDOC(m_pEditor)->GetDlgDataSource();
	if(!pDS0)return;

	if(pDS0 && UVSModify==GetAccessType(pDS0) && FALSE==CUVSModify::CanExport())
	{
		Abort();
		GOutPut(StrFromResID(IDS_NO_PERMISSION));
		m_nExitCode = CMPEC_NULL;
		return;
	}

	CDlgOpSettings dlg;
	CUIParam param;
	MyFillShowParams(&param,TRUE);
	dlg.SetUIParam(&param);

	while(1)
	{
		if( dlg.DoModal()!=IDOK )
		{
			Abort();
			m_nExitCode = CMPEC_NULL;
			return;
		}

		CValueTable tab1;
		dlg.GetValueTable(tab1);
		SetParams(tab1,FALSE);

		if( m_strPathName.IsEmpty() )
		{
			AfxMessageBox(StrFromResID(IDS_PARAM_ERROR));
		}
		else
		{
			break;
		}
	}

	CValueTable tab1;
	dlg.GetValueTable(tab1);
	SetParams(tab1,FALSE);

	CString strLayName = m_strLayNameOfBound;
	if( strLayName.IsEmpty() )
		strLayName = "TK_inner_bound";

	CFtrLayer *pLayer0;
	CFeature *pFtr0;

	PT_3D pts[4];

	int nSum = 0;	
	int nLay = pDS0->GetFtrLayerCount();
	for ( int i=0; i<nLay; i++)
	{
		pLayer0 = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer0 || !pLayer0->IsVisible() )continue;
		
		if( stricmp(pLayer0->GetName(),strLayName)!=0 )
			continue;

		nSum += pLayer0->GetObjectCount();
	}

	if( nSum<=0 )return;

	GProgressStart(nSum);
	for ( i=0; i<nLay; i++)
	{
		pLayer0 = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer0 || !pLayer0->IsVisible() )continue;
		
		if( stricmp(pLayer0->GetName(),strLayName)!=0 )
			continue;

		int nObj = pLayer0->GetObjectCount();		
		for (int j=0;j<nObj;j++)
		{
			GProgressStep();

			pFtr0 = pLayer0->GetObject(j);
			if (pFtr0)
			{
				CGeometry *pGeo = pFtr0->GetGeometry();
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				{
					if( pGeo->GetDataPointSum()==5 )
					{
						CArray<PT_3DEX,PT_3DEX> arrPts;
						pGeo->GetShape(arrPts);

						for( int k=0; k<4; k++)
						{
							pts[k] = arrPts[k];
						}

						ExportMap(GetMapName(pts),pFtr0,pts);
					}
				}
			}			
		}
	}
	GProgressEnd();

	Finish();
	m_nExitCode = CMPEC_NULL;
}


CString CBatExportMapsCommand_dxf::GetMapName(PT_3D pts[4])
{
	CString name;

	CString ext = ".dxf";
	if( m_nFileFormat==1 )
		ext = ".dwg";

	if( m_nNameType==typeTextInBound )
	{
		CDlgDataSource *pDS0 = PDOC(m_pEditor)->GetDlgDataSource();	
		if(!pDS0)return name;
					
		int nLay = pDS0->GetFtrLayerCount();
		
		CFtrLayer *pLayer0;
		
		CFeature *pFtr0;
		
		for ( int i=0;i<nLay;i++)
		{
			pLayer0 = pDS0->GetFtrLayerByIndex(i);
			if( !pLayer0 || !pLayer0->IsVisible() )continue;
			
			if( stricmp(pLayer0->GetName(),m_strLayNameOfMapNum)!=0 )
				continue;
			
			int nObj = pLayer0->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr0 = pLayer0->GetObject(j);
				if (pFtr0)
				{				
					CGeometry *pGeo = pFtr0->GetGeometry();
					if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
						continue;

					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeo->GetShape(arrPts);

					if( GraphAPI::GIsPtInRegion((PT_3D)arrPts[0],pts,4)==2 )
					{
						name = m_strPathName + "\\" + ((CGeoText*)pGeo)->GetText() + ext;
						return name;
					}
				}
			}
		}
	}
	else if( m_nNameType==typeAutoNumber )
	{
		name.Format("%d",m_nAutoNum);
		name = m_strPathName + "\\" + name + ext;

		m_nAutoNum++;
	}
	
	return name;
}



BOOL CBatExportMapsCommand_dxf::GetNearMapName(CDlgDataSource *pDS0, PT_3D pt0, float ang, CString names[9])
{
	NameItem item;
	CArray<NameItem,NameItem> arrItems;

	for( int i=0; i<9; i++)
	{
		names[i].Empty();
	}
	
	{		
		int nLay = pDS0->GetFtrLayerCount();
		
		CFtrLayer *pLayer0;
		
		CFeature *pFtr0;
		
		//找到邻近的图幅，并排序
		for ( i=0;i<nLay;i++)
		{
			pLayer0 = pDS0->GetFtrLayerByIndex(i);
			if( !pLayer0 || !pLayer0->IsVisible() )continue;
			
			if( stricmp(pLayer0->GetName(),m_strLayNameOfMapNum)!=0 )
				continue;
			
			int nObj = pLayer0->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr0 = pLayer0->GetObject(j);
				if (pFtr0)
				{				
					CGeometry *pGeo = pFtr0->GetGeometry();
					if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
						continue;
					
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeo->GetShape(arrPts);
					
					{
						item.pt0 = arrPts[0];
						item.pGeo = (CGeoText*)pGeo;
						strncpy(item.name,item.pGeo->GetText(),sizeof(item.name)-1);
						item.dis = GraphAPI::GGet2DDisOf2P(item.pt0,pt0);

						for( int k=0; k<arrItems.GetSize(); k++)
						{
							if( item.dis<arrItems[k].dis )
							{
								arrItems.InsertAt(k,item);
								break;
							}
						}
						if( k>=arrItems.GetSize() && arrItems.GetSize()<9 )
						{
							arrItems.Add(item);
						}
						if( arrItems.GetSize()>9 )
						{
							arrItems.RemoveAt(9,arrItems.GetSize()-9);
						}
					}
				}
			}
		}
	}

	if( arrItems.GetSize()<=0 )
		return TRUE;

	//按照倾斜角进行坐标换算，以pt0为中心
	double ca = cos(-ang), sa = sin(-ang);
	for( i=0; i<arrItems.GetSize(); i++)
	{
		item = arrItems[i];
		double x = (item.pt0.x-pt0.x)*ca - (item.pt0.y-pt0.y)*sa + pt0.x;
		double y = (item.pt0.x-pt0.x)*sa + (item.pt0.y-pt0.y)*ca + pt0.y;
		item.pt0.x = x;
		item.pt0.y = y;	
		
		arrItems[i] = item;
	}

	//比较，取出周围8个图号
	float toler = pDS0->GetScale()*0.2;
	NameItem *pItems = arrItems.GetData();
	int nsize = arrItems.GetSize();
	
	for( i=0; i<nsize; i++)
	{
		//左上
		if( pItems[i].pt0.x-pt0.x<-toler && pItems[i].pt0.y-pt0.y>toler )
		{
			names[0] = pItems[i].name;
		}
		else if( fabs(pItems[i].pt0.x-pt0.x)<toler && pItems[i].pt0.y-pt0.y>toler )
		{
			names[1] = pItems[i].name;
		}
		else if( pItems[i].pt0.x-pt0.x>toler && pItems[i].pt0.y-pt0.y>toler )
		{
			names[2] = pItems[i].name;
		}
		else if( pItems[i].pt0.x-pt0.x<-toler && fabs(pItems[i].pt0.y-pt0.y)<toler )
		{
			names[3] = pItems[i].name;
		}
		else if( fabs(pItems[i].pt0.x-pt0.x)<toler && fabs(pItems[i].pt0.y-pt0.y)<toler )
		{
			names[4] = pItems[i].name;
		}
		else if( pItems[i].pt0.x-pt0.x>toler && fabs(pItems[i].pt0.y-pt0.y)<toler )
		{
			names[5] = pItems[i].name;
		}
		else if( pItems[i].pt0.x-pt0.x<-toler && pItems[i].pt0.y-pt0.y<-toler )
		{
			names[6] = pItems[i].name;
		}
		else if( fabs(pItems[i].pt0.x-pt0.x)<toler && pItems[i].pt0.y-pt0.y<-toler )
		{
			names[7] = pItems[i].name;
		}
		else if( pItems[i].pt0.x-pt0.x>toler && pItems[i].pt0.y-pt0.y<-toler )
		{
			names[8] = pItems[i].name;
		}
	}

	return TRUE;
}

#include "DxfAccess.h"
void CBatExportMapsCommand_dxf::ExportMap(LPCTSTR fileName, CFeature *pFtr, PT_3D boundPts[4])
{
	CDlgDataSource *pDS0 = PDOC(m_pEditor)->GetDlgDataSource();	
	if(!pDS0)return;
	
	int nSum = 0;		
	int nLay = pDS0->GetFtrLayerCount();

	CFtrLayer *pLayer0;
	CValueTable tab;

	CGrTrim trim;
	trim.InitTrimPolygon(boundPts,4,50*pDS0->GetScale()*0.01);

	Envelope evlp;
	evlp.CreateFromPts(boundPts,4);

	CString strLayName = m_strLayNameOfBound;
	if( strLayName.IsEmpty() )
		strLayName = "TK_inner_bound";

	CPtrArray arrFtrLayers;
	CPtrArray arr;
	CPtrArray arr2;

	CFeature *pFtr0, *pFtr1;

	PT_3D boundPts2[5];
	memcpy(boundPts2,boundPts,sizeof(PT_3D)*4);
	boundPts2[4] = boundPts2[0];

	arr2.Add(pFtr->Clone());
	pLayer0 = pDS0->GetFtrLayerOfObject(pFtr);
	arrFtrLayers.Add(pLayer0);
//	AddFtrsToDS(arr2,pLayer0,pDS0,pDS1);

	for ( int i=0;i<nLay;i++)
	{
		pLayer0 = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer0 || !pLayer0->IsVisible() )continue;

		int nObj = pLayer0->GetObjectCount();		
		for (int j=0;j<nObj;j++)
		{
			pFtr0 = pLayer0->GetObject(j);
			if ( pFtr0 && pFtr0!=pFtr )
			{
				arr.RemoveAll();
				//arr2.RemoveAll();
				
				if( stricmp(pLayer0->GetName(),strLayName)==0 && pFtr0->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
					continue;

				TrimObj(pFtr0->GetGeometry(),&arr,evlp,boundPts2,5,&trim);
				for( int k=0; k<arr.GetSize(); k++)
				{
					pFtr1 = pFtr0->Clone();
					if( !pFtr1 )continue;

					pFtr1->SetGeometry((CGeometry*)arr[k]);
					pFtr1->SetID(OUID());

					arr2.Add(pFtr1);

					arrFtrLayers.Add(pLayer0);
				}

				//if( strncmp(pLayer0->GetName(),"TK_",3)!=0 )
					nSum += arr2.GetSize();

				//AddFtrsToDS(arr2,pLayer0,pDS0,pDS1);
			}
		}
	}

	//没有地物，就不导出
	if( nSum<=0 )
	{
		return;
	}

	CDxfWrite dxf;
	dxf.SetAsExportBaselines();
	dxf.SetDlgDataSource(pDS0);

	if( m_bSymbolized )
	{
		dxf.SetAsExportSymbols();
		dxf.m_dlgDxfExport.m_b3DLines = FALSE;
	}
	else
	{
		dxf.m_dlgDxfExport.m_bAddAnnot = TRUE;
		dxf.m_dlgDxfExport.m_b3DLines = TRUE;
	}

	if( m_bAsLayerCode )
	{
		dxf.m_dlgDxfExport.m_bUseIndex = TRUE;
	}
	else
	{
		dxf.m_dlgDxfExport.m_bUseIndex = FALSE;
	}

	//if( CString(fileName).Find("48750.0-344700")>=0 )
	if( m_nFileFormat==0 )
	{
		dxf.ObjectsToDxf(arr2,arrFtrLayers,fileName);
	}
	else
	{
		dxf.ObjectsToDwg(arr2,arrFtrLayers,fileName);
	}

	for (i=0; i<arr2.GetSize(); i++)
	{
		delete (CFeature*)arr2[i];
	}

}


BOOL CBatExportMapsCommand_dxf::TrimObj(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim)
{
	//处理点或者文本
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint))||pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
	{
		PT_3D pt3d;
		PT_3DEX expt;
		expt = pObj->GetDataPoint(0);
		COPY_3DPT(pt3d,expt);
		BOOL bIn = (GraphAPI::GIsPtInRegion(pt3d,polys,polysnum)==2);
		if( bIn )
		{
			CGeometry *pNew = pObj->Clone();
			if( pNew )pArray->Add(pNew);
			return TRUE;
		}
	}
	//处理线
	else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
	{
		if( CTrimCommand::TrimCurve(pObj,pArray,evlp,polys,polysnum,ptrim,TRUE) )
		{
			return TRUE;
		}
		else
		{
			pArray->Add(pObj->Clone());
		}
	}
	//处理面
	else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		// 面依照线的方式
		CGeometry *pNewGeo = (CGeometry *)CPermanent::CreatePermanentObject(CLS_GEOCURVE);
		if (!pNewGeo) return FALSE;
		// 复制固定属性
		CValueTable tab;
		tab.BeginAddValueItem();
		pObj->WriteTo(tab);
		tab.EndAddValueItem();
		tab.DelField(FIELDNAME_SHAPE);
		tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
		tab.DelField(FIELDNAME_FTRID);
		tab.DelField(FIELDNAME_GEOCLASS);
		pNewGeo->ReadFrom(tab);

		CArray<PT_3DEX,PT_3DEX> arrPts;
		pObj->GetShape(arrPts);
		arrPts.Add(arrPts[0]);
		pNewGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());

		if( CTrimCommand::TrimCurve(pNewGeo,pArray,evlp,polys,polysnum,ptrim,TRUE) )
		{
			return TRUE;
		}
		else
		{
			pArray->Add(pObj->Clone());
		}

		delete pNewGeo;


	}

	return TRUE;
}


void CBatExportMapsCommand_dxf::AddFtrsToDS(CFtrArray& arr, CFtrLayer *pLayer0, CDlgDataSource *pDS0, CDlgDataSource *pDS1, CFtrArray *pRetArr)
{
	CFeature *pFtr0, *pFtr1;
	CFtrLayer *pLayer1;
	CValueTable tab;

	int nObj = arr.GetSize();

	for (int i=0;i<nObj;i++)
	{	
		pFtr0 = (CFeature*)(arr[i]);			
		if (pFtr0)
		{
			pLayer1 = pDS1->GetFtrLayer(pLayer0->GetName());
			if (pLayer1)
			{
				;
			}
			else
			{
				pLayer1 = new CFtrLayer;
				if(!pLayer1)continue;
				tab.DelAll();
				tab.BeginAddValueItem();
				pLayer0->WriteTo(tab);
				tab.EndAddValueItem();
				pLayer1->ReadFrom(tab);
				pLayer1->SetID(0);
				pDS1->AddFtrLayer(pLayer1);
			}

			pFtr1 = pFtr0->Clone();
			pFtr1->SetID(OUID());
			if(!pDS1->AddObject(pFtr1,pLayer1->GetID()))
			{
				delete pFtr1;
				continue;
			}			
			tab.DelAll();
			tab.BeginAddValueItem();
			pDS0->GetXAttributesSource()->GetXAttributes(pFtr0,tab);
			tab.EndAddValueItem();
			pDS1->GetXAttributesSource()->SetXAttributes(pFtr1,tab);

			if( pRetArr )
			{
				pRetArr->Add(pFtr1);
			}
		}			
	}
}



void CBatExportMapsCommand_dxf::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (_bstr_t)(LPCTSTR)m_strLayNameOfBound;
	tab.AddValue("LayNameOfBound",&CVariantEx(var));
		
	var = (_bstr_t)(LPCTSTR)m_strLayNameOfMapNum;
	tab.AddValue("LayNameOfMapNum",&CVariantEx(var));	

	var = (long)m_nFileFormat;
	tab.AddValue("FileFormat",&CVariantEx(var));

	var = (long)m_nNameType;
	tab.AddValue("NameType",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strPathName;
	tab.AddValue("PathName",&CVariantEx(var));

	var = (_variant_t)(bool)m_bSymbolized;
	tab.AddValue("Symbolized",&CVariantEx(var));

	var = (_variant_t)(bool)m_bAsLayerCode;
	tab.AddValue("AsLayerCode",&CVariantEx(var));

}

void CBatExportMapsCommand_dxf::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	CString cmd;
	
	if( tab.GetValue(0,"LayNameOfBound",var) )
	{
		m_strLayNameOfBound = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"LayNameOfMapNum",var) )
	{
		m_strLayNameOfMapNum = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"FileFormat",var) )
	{
		m_nFileFormat = (long)(_variant_t)*var;
	}	
	if( tab.GetValue(0,"NameType",var) )
	{
		m_nNameType = (long)(_variant_t)*var;
	}	
	if( tab.GetValue(0,"PathName",var) )
	{
		m_strPathName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"Symbolized",var) )
	{
		m_bSymbolized = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,"AsLayerCode",var) )
	{
		m_bAsLayerCode = (bool)(_variant_t)*var;
	}	

	if( !bInit )
		SetSettingsModifyFlag();
	
	CEditCommand::SetParams(tab,bInit);
}


void CBatExportMapsCommand_dxf::MyFillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("BatExportMapsCommand_dxf",StrFromLocalResID(IDS_CMDNAME_BatExportMapsCommand_dxf));
	param->AddPathNameParam("PathName",m_strPathName,StrFromResID(IDS_SAVEDIR));

	param->BeginOptionParam("FileFormat",StrFromResID(IDS_FILEFORMAT));
	param->AddOption("DXF",0,0,TRUE);
	param->AddOption("DWG",1,0,TRUE);
	param->EndOptionParam();
	param->SetOptionDefault("FileFormat",m_nFileFormat);

	param->AddUsedLayerNameParam("LayNameOfBound",(LPCTSTR)m_strLayNameOfBound,StrFromResID(IDS_MAPBODER_LAYERNAME));
	param->AddUsedLayerNameParam("LayNameOfMapNum",(LPCTSTR)m_strLayNameOfMapNum,StrFromResID(IDS_LAYNAME_MAPNUM));

	param->BeginOptionParam("NameType",StrFromResID(IDS_FILENAME_FORMAT));
	param->AddOption(StrFromResID(IDS_FILENAME_MAPNUM),0,0,TRUE);
	param->AddOption(StrFromResID(IDS_FILENAME_USEINDEX),1,0,TRUE);
	param->EndOptionParam();
	param->SetOptionDefault("NameType",m_nNameType);

	param->AddParam("Symbolized",(bool)m_bSymbolized,StrFromResID(IDS_FIELDNAME_SYMBOL));
	param->AddParam("AsLayerCode",(bool)m_bAsLayerCode,StrFromResID(IDS_AS_LAYERCODE));
}

//////////////////////////////////////////////////////////////////////
// CConvertBySelectPtsCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvertBySelectPtsCommand::CConvertBySelectPtsCommand()
{
	m_nStep = -1;
	m_nIndex = 0;
}

CConvertBySelectPtsCommand::~CConvertBySelectPtsCommand()
{
}



CCommand* CConvertBySelectPtsCommand::Create()
{
	return new CConvertBySelectPtsCommand;
}

CString CConvertBySelectPtsCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_ConvertBySelectPts);
}

void CConvertBySelectPtsCommand::Start()
{
	m_nStep = 0;
	m_nIndex = 0;

	m_ftrs.RemoveAll();

	m_pEditor->OpenSelector();

	CCommand::Start();
	
	CString strMsg;
	strMsg = StrFromResID(IDS_TIP_SELECTFTRS_CONVERT);
	GOutPut(strMsg);

	return;
}


void CConvertBySelectPtsCommand::Abort()
{
	if( PDOC(m_pEditor) )
	{
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_ClearDragLine);
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Detach_Accubox);
	}
	
	m_nStep = -1;
	CCommand::Abort();
	m_nExitCode = CMPEC_STARTOLD;
}

void CConvertBySelectPtsCommand::PtClick(PT_3D &pt, int flag)
{
	//确定操作集
	if( m_nStep==0 )
	{
		if( !CEditCommand::CanGetSelObjs(flag) )
			return;

		int num = 0;
		const FTR_HANDLE *ftrs = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num<=0 )
			return;

		for( int i=0; i<num; i++)
		{
			m_ftrs.Add(HandleToFtr(ftrs[i]));
		}

		m_nStep = 1;

		GotoState(PROCSTATE_PROCESSING);

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->CloseSelector();
		m_pEditor->UpdateDrag(ud_Attach_Accubox);	
		m_pEditor->UpdateDrag(ud_ClearDrag);

		CString strMsg;
		strMsg.Format(StrFromResID(IDS_CLICK_PT1),(int)1);
		GOutPut(strMsg);
		return;
	}

	if( m_nStep>=1 )
	{
		m_pts.Add(pt);

		int npt = m_pts.GetSize();
		CString str;
		if( (npt%2)==0 )
		{
			str.Format(StrFromResID(IDS_CLICK_PT1),npt/2+1);

			GrBuffer buf;
			buf.BeginLineString(RGB(255,255,255),0);
			buf.MoveTo(&m_pts[m_pts.GetSize()-2]);
			buf.LineTo(&m_pts[m_pts.GetSize()-1]);
			buf.End();

			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetVariantDragLine,NULL);
			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_AddConstDragLine,(CObject*)&buf);
		}
		else
			str.Format(StrFromResID(IDS_CLICK_PT2),npt/2+1);

		GOutPut(str);
	}
		
	CCommand::PtClick(pt,flag);
}


void CConvertBySelectPtsCommand::PtMove(PT_3D &pt)
{
	if( m_nStep>=1 && m_pts.GetSize()>0 && (m_pts.GetSize()%2)!=0 )
	{
		PT_3D ptt;
		GrBuffer buf;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_pts[m_pts.GetSize()-1]);
		buf.LineTo(&pt);
		buf.End();
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetVariantDragLine,(CObject*)&buf);
	}
	
	CCommand::PtMove(pt);
}



void CConvertBySelectPtsCommand::PtReset(PT_3D& pt)
{
	if( m_nStep<1 )
		return;

	CArray<PT_3D,PT_3D> pts1, pts2;
	int nsz = m_pts.GetSize();
	nsz = (nsz/2) * 2;
	if( nsz<2 )
		return;

	for( int i=0; i<nsz; i+=2 )
	{
		pts1.Add(m_pts[i]);
		pts2.Add(m_pts[i+1]);
	}

	nsz = (nsz/2);
	
	double m[16];
	matrix_toIdentity(m,4);
	if( nsz<3 )
	{
		m[3] = pts2[0].x - pts1[0].x;
		m[7] = pts2[0].y - pts1[0].y;		
		m[11] = 0;
	}
	else
	{
		double m2[6];
		
		double *buf = new double[nsz*4];
		double *x1 = buf, *y1 = buf+nsz;
		double *x2 = y1+nsz, *y2 = y1+nsz+nsz;
		
		for( int i=0; i<nsz; i++)
		{
			x1[i] = pts1[i].x; y1[i] = pts1[i].y;
			x2[i] = pts2[i].x; y2[i] = pts2[i].y;
		}
		
		CalcAffineParams(x1,y1,x2,y2,nsz,m2,m2+3);
		
		m[0] = m2[0]; m[1] = m2[1]; m[3] = m2[2]; 
		m[4] = m2[3]; m[5] = m2[4]; m[7] = m2[5]; 
		
		delete[] buf;
	}

	CUndoTransform undo(m_pEditor,Name());
	memcpy(undo.matrix,m,sizeof(m));

	int nObj = m_ftrs.GetSize();

	for( i=0; i<nObj; i++)
	{
		m_pEditor->DeleteObject(FtrToHandle(m_ftrs[i]),FALSE);
		m_ftrs[i]->GetGeometry()->Transform(m);
		m_pEditor->RestoreObject(FtrToHandle(m_ftrs[i]));

		undo.arrHandles.Add(FtrToHandle(m_ftrs[i]));
	}
	undo.Commit();
	
	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_ClearDragLine);	
	Finish();

	GOutPut(StrFromResID(IDS_PROCESS_OK));
}

//////////////////////////////////////////////////////////////////////
// CDeleteShortCurveCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDeleteShortCurveCommand,CEditCommand)

CDeleteShortCurveCommand::CDeleteShortCurveCommand()
{
	m_lfLimit = 0.2;
	m_nBoundType = 0;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;
	strcat(m_strRegPath,"\\DeleteShortCurve");
}

CDeleteShortCurveCommand::~CDeleteShortCurveCommand()
{
}

CCommand* CDeleteShortCurveCommand::Create()
{
	return new CDeleteShortCurveCommand;
}

CString CDeleteShortCurveCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DELETESHORTCURVE);
}


void CDeleteShortCurveCommand::Start()
{
	if( !m_pEditor )return;
	
	m_lfLimit = 0.2;
	m_nBoundType = 0;
	m_pGeoCurve = NULL;
	m_pDrawProc = NULL;
	
	m_nStep = 0;
	CEditCommand::Start();

	// 全图
	if (m_nBoundType == 0)
	{
		m_pEditor->CloseSelector();
	}
	// 选择
	else if (m_nBoundType == 1)
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if( !m_pDrawProc )return;
		m_pDrawProc->Init(m_pEditor);
		UpdateParams(FALSE);
		m_pDrawProc->Start();
		m_pGeoCurve = new CGeoCurve;
		if(!m_pGeoCurve) 
		{
			Abort();
			return ;
		}
		m_pGeoCurve->SetColor(RGB(255,255,255));
		m_pDrawProc->m_pGeoCurve = (CGeoCurve*)m_pGeoCurve;
		m_pDrawProc->m_bClosed = TRUE;
		m_pEditor->CloseSelector();
		return;
	}

	
}

void CDeleteShortCurveCommand::Abort()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
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
    m_nStep = -1;
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->UpdateDrag(ud_ClearDrag);
	
	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CDeleteShortCurveCommand::Finish()
{
	UpdateParams(TRUE);
	if( m_pDrawProc )
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
	m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CDeleteShortCurveCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID(StrFromLocalResID(IDS_CMDNAME_DELETESHORTCURVE),StrFromLocalResID(IDS_CMDNAME_DELETESHORTCURVE));

	param->BeginOptionParam(PF_CHECKBOUND,StrFromResID(IDS_CMDPLANE_CHECKBOUND));
	param->AddOption(StrFromResID(IDS_WHOLEMAP),0,' ',m_nBoundType==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_DRAW),1,' ',m_nBoundType==1);
	param->EndOptionParam();
	
	param->AddParam(PF_LIMITED,m_lfLimit, StrFromResID(IDS_CMDPLANE_TOLER));

}

void CDeleteShortCurveCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,PF_CHECKBOUND,var) )
	{					
		m_nBoundType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();					
	}
	if( tab.GetValue(0,PF_LIMITED,var) )
	{
		m_lfLimit = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}	
	CEditCommand::SetParams(tab,bInit);
}

void CDeleteShortCurveCommand::GetParams(CValueTable& tab)
{
	_variant_t var;
	var = (long)(m_nBoundType);
	tab.AddValue(PF_CHECKBOUND,&CVariantEx(var));
	var = m_lfLimit;
	tab.AddValue(PF_LIMITED,&CVariantEx(var));
}

BOOL CDeleteShortCurveCommand::FilterFeature(LONG_PTR id)
{
	CFeature *pFtr = (CFeature*)id;
	CDlgDataSource  *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	if(pDS) 
	{
		CFtrLayer* pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) return FALSE;	
		if(pLayer->IsLocked()) return FALSE;
		if ((!pLayer->IsVisible()||!pFtr->IsVisible())) return FALSE;
		CGeometry *po = pFtr->GetGeometry();
		if( !po )return FALSE;
		if( !po->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return FALSE;

	}
	return TRUE;
}

CProcedure *CDeleteShortCurveCommand::GetActiveSonProc(int nMsgType)
{
	if (m_nBoundType==1&&(nMsgType==msgEnd||nMsgType==msgPtMove/*||nMsgType==msgPtReset*/))
	{
		return m_pDrawProc;
	}
	return NULL;
}

void CDeleteShortCurveCommand::PtReset(PT_3D &pt)
{	
	if( m_pDrawProc && !IsProcOver(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
	}
	else
		Abort();
}

void CDeleteShortCurveCommand::OnSonEnd(CProcedure *son)
{
	if( m_nBoundType==1 )
	{
		if( m_nStep==1 )
		{	
			if( !m_pDrawProc->m_pGeoCurve || IsProcFinished(this) )
				return;
			
			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_nStep = 2;
			
		}
		else 
		{
			Abort();
		}
	}
	if( m_pDrawProc )
	{
		m_pGeoCurve = (CGeoCurve *)m_pDrawProc->m_pGeoCurve->Clone();
		if(m_pDrawProc->m_pGeoCurve) delete m_pDrawProc->m_pGeoCurve;
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
		
		CEditCommand::OnSonEnd(son);
	}
}

void CDeleteShortCurveCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_nBoundType == 0)
	{
		m_nStep = 2;
	}
	else if (m_nBoundType == 1)
	{
		if (m_pDrawProc)
		{
			GotoState(PROCSTATE_PROCESSING);
			m_pDrawProc->PtClick(pt,flag);
		}
	}
   
	if( m_nStep==2 )
	{
		CArray<CFeature*,CFeature*> arrFtrs;
		int lSum = 0;

		if (m_nBoundType == 0)
		{
			CFeature *pFtr;	
			CFtrLayer *pLayer;
			
			int nLayNum = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerCount(), i , j;
			for( i=0; i<nLayNum; i++)
			{
				pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerByIndex(i);
				if( !pLayer || !pLayer->IsVisible())continue;
				
				int nObjNum = pLayer->GetObjectCount();
				for( j=0; j<nObjNum; j++)
				{
					pFtr = pLayer->GetObject(j);
					if( !pFtr )continue;
					
					if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
						continue;
					
					lSum++;

					arrFtrs.Add(pFtr);
				}
			}
		}
		else if (m_nBoundType == 1)
		{
			int i;

			const CShapeLine *pBase  = m_pGeoCurve->GetShape();
			if(!pBase)
			{
				Abort();
				return;
			}
			Envelope e = pBase->GetEnvelope();
 			CArray<PT_3DEX,PT_3DEX> arrPts;
			pBase->GetPts(arrPts);

			e.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
			m_pEditor->GetDataQuery()->FindObjectInRect(e,m_pEditor->GetCoordWnd().m_pSearchCS);
			int num;
			const CPFeature *ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);	
			
			for ( i=0; i<num; i++)
			{
				CFeature *pFtr = ftr[i];
				if (!pFtr) continue;
				CGeometry *pGeo = pFtr->GetGeometry();
				if(!pGeo) continue;
				if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
					continue;

				CArray<PT_3DEX,PT_3DEX> pts;
				pGeo->GetShape(pts);
				for(int j=0; j<pts.GetSize(); j++)
				{
					if(2==GraphAPI::GIsPtInRegion(pts[j], arrPts.GetData(), arrPts.GetSize()) )
						break;
				}
				if(j==pts.GetSize()) continue;
				
				lSum++;
				arrFtrs.Add(pFtr);
			}
		}

		if( lSum<=0 )
		{
			GOutPut(StrFromResID(IDS_CMDERR_NOOBJ));
			Abort();
			return;
		}
		
		GProgressStart(lSum);

		int sum = 0;
		CUndoFtrs undo(m_pEditor,Name());
		for (int i=arrFtrs.GetSize()-1; i>=0; i--)
		{
			CFeature *pFtr = arrFtrs[i];
			double len = pFtr->GetGeometry()->GetShape()->GetLength();
			if (fabs(len) < fabs(m_lfLimit))
			{
				undo.AddOldFeature(FtrToHandle(pFtr));
				
				m_pEditor->DeleteObject(FtrToHandle(pFtr));

				sum++;
			}
		}

		undo.Commit();

		CString str;
		str.Format(StrFromResID(IDS_CMDTIP_DELNUM),sum);

		GOutPut(str);
		
		GProgressEnd();
		
		Finish();
		m_nStep = 2;
	}

	//画线
	if( m_nBoundType==1 )
	{
		if( m_nStep==0 || m_nStep==1 )
		{		
			m_nStep = 1;
		}
	}
	
	CEditCommand::PtClick(pt, flag);
}



//////////////////////////////////////////////////////////////////////
// CDeleteOverlapLinesCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDeleteOverlapLinesCommand,CEditCommand)

CDeleteOverlapLinesCommand::CDeleteOverlapLinesCommand()
{
	m_nBoundType = 0;
	m_bCheckZ = FALSE;
	strcat(m_strRegPath,"\\DeleteOverlapLines");
}

CDeleteOverlapLinesCommand::~CDeleteOverlapLinesCommand()
{
}

CCommand* CDeleteOverlapLinesCommand::Create()
{
	return new CDeleteOverlapLinesCommand();
}

CString CDeleteOverlapLinesCommand::Name()
{ 
	return StrFromResID(IDS_DELETE_OVERLAPLINES);
}


void CDeleteOverlapLinesCommand::Start()
{
	if( !m_pEditor )return;
	
	m_nBoundType = 0;
	
	m_nStep = 0;
	CEditCommand::Start();

	if( m_nBoundType==0 )
		m_pEditor->CloseSelector();
}

void CDeleteOverlapLinesCommand::Abort()
{
	UpdateParams(TRUE);
    m_nStep = -1;
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CDeleteOverlapLinesCommand::Finish()
{
	UpdateParams(TRUE);
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CDeleteOverlapLinesCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DeleteOverlapLines",Name());

	param->BeginOptionParam("Mode",StrFromResID(IDS_CMDPLANE_WAY));
	param->AddOption(StrFromResID(IDS_WHOLEMAP),0,' ',m_nBoundType==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_SEL),1,' ',m_nBoundType==1);
	param->EndOptionParam();

	if( bForLoad || m_nBoundType==0 )
	{
		param->AddLayerNameParamEx("LayNames",(LPCTSTR)m_strLayers, StrFromResID(IDS_LAYER_NAME),NULL,LAYERPARAMITEM_LINE);
	}
	param->AddParam("bCheckZ", (bool)m_bCheckZ, StrFromResID(IDS_CMDPLANE_CHECKZ));
}

void CDeleteOverlapLinesCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Mode",var) )
	{					
		m_nBoundType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();					
	}

	if( tab.GetValue(0,"LayNames",var) )
	{
		m_strLayers = (const char*)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"bCheckZ",var) )
	{
		m_bCheckZ = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}

void CDeleteOverlapLinesCommand::GetParams(CValueTable& tab)
{
	_variant_t var;
	var = (long)(m_nBoundType);
	tab.AddValue("Mode",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayers;
	tab.AddValue("LayNames",&CVariantEx(var));

	var = (bool)m_bCheckZ;
	tab.AddValue("bCheckZ",&CVariantEx(var));
}

void CDeleteOverlapLinesCommand::PtClick(PT_3D &pt, int flag)
{
	CDlgDoc *pDoc = PDOC(m_pEditor);

	CFtrArray arrFtrs;
	if (m_nBoundType == 0)
	{
		CDlgDataSource* pDS = pDoc->GetDlgDataSource();
		int nLayerCnt = pDS->GetFtrLayerCount();
		int nSum = 0;
		
		CPFeature pFtr;
		CFtrLayer *pLayer;
		for (int i=0;i<nLayerCnt;i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if (pLayer==NULL||!pLayer->IsVisible()||pLayer->IsLocked())continue;

			if( !m_strLayers.IsEmpty() && !CheckNameForLayerCode(pDS,pLayer->GetName(),m_strLayers) )
				continue;

			int nObj = pLayer->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr = pLayer->GetObject(j);
				if (pFtr==NULL||!pFtr->IsVisible()||!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))continue;
				nSum++;
			}
		}
		CArray<OverlapLineItem,OverlapLineItem> arrItemFtrs2;
		CFtrArray arrFtrs;
		CUndoFtrs undo(m_pEditor,Name());		
		GProgressStart(nSum);
		for (i=0;i<nLayerCnt;i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if (pLayer==NULL||!pLayer->IsVisible()||pLayer->IsLocked())continue;

			if (!m_strLayers.IsEmpty() && !CheckNameForLayerCode(pDS, pLayer->GetName(), m_strLayers))
				continue;

			int nObj = pLayer->GetObjectCount();
			arrFtrs.RemoveAll();
			for (int j=0;j<nObj;j++)
			{
				pFtr = pLayer->GetObject(j);
				if (pFtr==NULL||!pFtr->IsVisible()||!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))continue;
				arrFtrs.Add(pFtr);
			}
			int nFtrs = arrFtrs.GetSize();
			
			for (int k=0;k<nFtrs;k++)
			{
				GProgressStep();
				
				arrItemFtrs2.RemoveAll();
				pDoc->FindOverlapLines(arrFtrs[k],arrItemFtrs2,TRUE, m_bCheckZ);
				pDoc->ProcessOverlapLineItems(arrItemFtrs2,undo);
			}	
		}

		//CUndoFtrs undo(m_pEditor,Name());
		
		//pDoc->ProcessOverlapLineItems(arrItemFtrs2,undo);

		CString strMsg;
		strMsg.Format(IDS_PROCESS_OBJ_NUM,undo.arrOldHandles.GetSize());
		GOutPut(strMsg);
		
		undo.Commit();

		GProgressEnd();

		Finish();
	}
	else if (m_nBoundType == 1)
	{
		if( !CEditCommand::CanGetSelObjs(flag) )
			return;
		
		int num = 0;
		const FTR_HANDLE *ftrs = m_pEditor->GetSelection()->GetSelectedObjs(num);
		if( num<=0 )
			return;

		CArray<OverlapLineItem,OverlapLineItem> arrItemFtrs2;
		
		for( int i=0; i<num; i++)
		{
			pDoc->FindOverlapLines(HandleToFtr(ftrs[i]), arrItemFtrs2, TRUE, m_bCheckZ);
		}

		CUndoFtrs undo(m_pEditor,Name());

		pDoc->ProcessOverlapLineItems(arrItemFtrs2,undo);

		CString strMsg;
		strMsg.Format(IDS_PROCESS_OBJ_NUM,undo.arrOldHandles.GetSize());
		GOutPut(strMsg);

		undo.Commit();

		Finish();
	}
   	
	CEditCommand::PtClick(pt, flag);
}



//////////////////////////////////////////////////////////////////////
// CManualRotateViewCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CManualRotateViewCommand,CEditCommand)

CManualRotateViewCommand::CManualRotateViewCommand()
{
}

CManualRotateViewCommand::~CManualRotateViewCommand()
{
}

CString CManualRotateViewCommand::Name()
{ 
	return StrFromResID(IDS_VIEW_MANUALROTATE);
}

void CManualRotateViewCommand::Start()
{
	if( !m_pEditor )return;
	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>0 )
	{
		m_nStep = 0;
		
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		PtClick(pt,0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}
	
	CEditCommand::Start();
}

void CManualRotateViewCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{	
		if( (flag&SELSTAT_POINTSEL)!=0 )
		{
			m_ptClick = pt;
		}
		if( !CanGetSelObjs(flag) )
		{
			return;
		}
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		
		if (num>0)
		{
			CFeature *pFtr = HandleToFtr(handles[0]);
			CGeometry *pGeo = pFtr->GetGeometry();
			if( (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface))) && pGeo->GetDataPointSum()>=2 )
			{
				Envelope e;
				e.CreateMaxEnvelope();

				PT_3D ret1,ret2;

				pGeo->FindNearestBaseLine(m_ptClick,e,NULL,&ret1,&ret2,NULL);

				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);

				double ang = GraphAPI::GGetAngle(ret1.x,ret1.y,ret2.x,ret2.y);

				PDOC(m_pEditor)->UpdateAllViews(NULL,hc_RotateVectView,(CObject*)&ang);
			}	
		}
			
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		Finish();
		m_nStep = 2;
	}
	
	CCommand::PtClick(pt, flag);
}




//////////////////////////////////////////////////////////////////////
// CRefFileCopyAllCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CRefFileCopyAllCommand,CEditCommand)

CRefFileCopyAllCommand::CRefFileCopyAllCommand()
{
	m_nStep = -1;
	m_nBoundType = typeWSBound;
	m_bClip = TRUE;

	m_strRefFile = StrFromResID(IDS_ALL_REFFILES);

	m_pDrawProc = NULL;
}

CRefFileCopyAllCommand::~CRefFileCopyAllCommand()
{
}

CString CRefFileCopyAllCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COPYREFFILE);
}

void CRefFileCopyAllCommand::Start()
{
	if( !m_pEditor )return;
	
	CEditCommand::Start();

	m_nStep = 0;
	m_bClip = TRUE;

	if( m_nBoundType==typeDraw )
	{
		m_pDrawProc = new CDrawCurveProcedure;
		if( !m_pDrawProc )return;
		m_pDrawProc->Init(m_pEditor);
		UpdateParams(FALSE);
		m_pDrawProc->Start();
		m_pDrawProc->m_pGeoCurve = new CGeoCurve();
		m_pDrawProc->m_pGeoCurve->SetColor(RGB(255,255,255));
		m_pDrawProc->m_bClosed = TRUE;
		CCommand::Start();
		m_pEditor->CloseSelector();
		return;
	}
}


void CRefFileCopyAllCommand::Abort()
{
	if( m_pDrawProc )
	{
		if( m_pDrawProc->m_pGeoCurve )
			delete m_pDrawProc->m_pGeoCurve;

		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	m_pEditor->UpdateDrag(ud_ClearDrag);
	CEditCommand::Abort();
}

void CRefFileCopyAllCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_bClip);
	tab.AddValue("IsClip",&CVariantEx(var));
	var = (long)(m_nBoundType);
	tab.AddValue("BoundType",&CVariantEx(var));
}


void CRefFileCopyAllCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("RefFileCopyAll",StrFromResID(IDS_CMDNAME_COPYREFFILE));
	
	param->BeginOptionParam("FileName",StrFromResID(IDS_REFFILE_NAME));
	
	CDlgDoc *pDoc = PDOC(m_pEditor);
	
	int nData = pDoc->GetDlgDataSourceCount();
	int nActiveData = pDoc->GetActiveDataSourceIdx();

	param->AddOption(StrFromResID(IDS_ALL_REFFILES),0,' ',TRUE);
	
	for( int i=0; i<nData; i++)
	{
		if( i!=nActiveData )
			param->AddOption(ExtractFileNameExt(pDoc->GetDlgDataSource(i)->GetName()),i+1,' ',FALSE);
	}
	param->EndOptionParam();

	if( m_strRefFile.CompareNoCase(StrFromResID(IDS_ALL_REFFILES))==0 || m_strRefFile.IsEmpty() )
	{
		param->SetOptionDefault("FileName",0);
	}
	else
	{
		for( i=0; i<nData; i++)
		{
			if( i!=nActiveData && m_strRefFile==ExtractFileNameExt(pDoc->GetDlgDataSource(i)->GetName()) )
			{
				param->SetOptionDefault("FileName",i+1);
				break;
			}
		}
	}
	
//	param->AddParam("IsClip",(bool)m_bClip,StrFromResID(IDS_IS_CLIP));
	
	m_bClip = TRUE;
	if( bForLoad || m_bClip )
	{
		param->BeginOptionParam("BoundType",StrFromResID(IDS_BOUNDTYPE),NULL);
		param->AddOption(StrFromResID(IDS_CMDPLANE_WSBOUND),0,' ',(m_nBoundType==typeWSBound));
		param->AddOption(StrFromResID(IDS_CMDPLANE_SEL),1,' ',(m_nBoundType==typeSelect));
		param->AddOption(StrFromResID(IDS_CMDPLANE_DRAW),2,' ',(m_nBoundType==typeDraw));
		param->EndOptionParam();
	}
	
}


void CRefFileCopyAllCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"FileName",var) )
	{
		int nIndex = (long)(_variant_t)*var;

		CDlgDoc *pDoc = PDOC(m_pEditor);

		int nData = pDoc->GetDlgDataSourceCount();
		
		if( nIndex>0 && nIndex<=nData )
			m_strRefFile = ExtractFileNameExt(pDoc->GetDlgDataSource(nIndex-1)->GetName());
		else
			m_strRefFile = StrFromResID(IDS_ALL_REFFILES);

		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"IsClip",var) )
	{
		m_bClip= (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"BoundType",var) )
	{					
		m_nBoundType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();		
		
		if( !bInit )
		{
			Abort();
			Start();
			return;
		}
	}
	
	CEditCommand::SetParams(tab,bInit);
}


CProcedure *CRefFileCopyAllCommand::GetActiveSonProc(int nMsgType)
{
	if (m_nBoundType==typeDraw&&(nMsgType==msgEnd||nMsgType==msgPtMove/*||nMsgType==msgPtReset*/))
	{
		return m_pDrawProc;
	}
	return NULL;
}



void CRefFileCopyAllCommand::PtReset(PT_3D &pt)
{	
	if( m_pDrawProc && !IsProcOver(m_pDrawProc))
	{
		m_pDrawProc->PtReset(pt);
	}
	else
		Abort();
}



void CRefFileCopyAllCommand::OnSonEnd(CProcedure *son)
{
	if( m_nBoundType==typeDraw )
	{
		if( m_nStep==1 )
		{	
			if( !m_pDrawProc->m_pGeoCurve || IsProcFinished(this) )
				return;
			
			GrBuffer buf;
			m_pDrawProc->m_pGeoCurve->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());
			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
			m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
			m_nStep = 2;
			
		}
		else 
		{
			Abort();
			return;
		}
	}
	if( m_pDrawProc )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		m_pDrawProc->m_pGeoCurve->GetShape(arrPts);
		if(m_pDrawProc->m_pGeoCurve) delete m_pDrawProc->m_pGeoCurve;
		m_pDrawProc->m_pGeoCurve = NULL;
		delete m_pDrawProc;
		m_pDrawProc = NULL;

		m_arrBoundPts.SetSize(arrPts.GetSize());
		for( int i=0; i<arrPts.GetSize(); i++)
		{
			m_arrBoundPts[i] = arrPts[i];
		}
		
		CEditCommand::OnSonEnd(son);
	}
}


void CRefFileCopyAllCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nBoundType==typeWSBound )
	{
		if( m_nStep==0 )
		{
			PT_3D bounds[4];
			PDOC(m_pEditor)->GetDlgDataSource()->GetBound(bounds,NULL,NULL);
			DoCopy(bounds,4);

			m_pEditor->UpdateView(0,hc_UpdateAllObjects,0);
			
			Finish();
			m_nStep = 2;
		}
		return;
	}
	else if ( m_nBoundType==typeDraw )
	{
		if (m_pDrawProc)
		{
			GotoState(PROCSTATE_PROCESSING);
			m_pDrawProc->PtClick(pt,flag);
			m_nStep = 1;
		}
		else if( m_arrBoundPts.GetSize()>=4 )
		{
			DoCopy(m_arrBoundPts.GetData(),m_arrBoundPts.GetSize());
			
			m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pEditor->UpdateView(0,hc_UpdateAllObjects,0);

			Finish();
			m_nStep = 2;
		}
		return;
	}

	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CSelection* pSel = m_pEditor->GetSelection();
		
		int num;
		const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
		
		if (num>0)
		{
			CFeature *pFtr = HandleToFtr(handles[0]);
			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && pGeo->GetDataPointSum()>=3 )
			{
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);	
				
				CArray<PT_3D,PT_3D> arrPts2;
				
				arrPts2.SetSize(arrPts.GetSize());
				for( int i=0; i<arrPts.GetSize(); i++)
				{
					arrPts2[i] = arrPts[i];
				}
				
				DoCopy(arrPts2.GetData(),arrPts2.GetSize());
			}	
		}
		
		m_pEditor->UpdateDrag(ud_ClearDrag);
		
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		m_pEditor->UpdateView(0,hc_UpdateAllObjects,0);
		
		Finish();
		m_nStep = 2;

		return;
	}
	
	CCommand::PtClick(pt, flag);
}


void CRefFileCopyAllCommand::DoCopy(PT_3D *boundPts, int npt)
{
	CDlgDoc *pDoc = PDOC(m_pEditor);
	
	int nData = pDoc->GetDlgDataSourceCount();
	int nActiveData = pDoc->GetActiveDataSourceIdx();

	CArray<CDlgDataSource*,CDlgDataSource*> arrDSs;
	
	CDlgDataSource *pDS0 = NULL;

	if( m_strRefFile.CompareNoCase(StrFromResID(IDS_ALL_REFFILES))==0 )
	{
		for( int i=0; i<nData; i++)
		{
			if( i!=nActiveData )
			{
				arrDSs.Add(pDoc->GetDlgDataSource(i));
			}
		}
	}
	else
	{
		for( int i=0; i<nData; i++)
		{
			if( i!=nActiveData )
			{
				if( m_strRefFile.CompareNoCase(ExtractFileNameExt(pDoc->GetDlgDataSource(i)->GetName()))==0 )
				{
					arrDSs.Add(pDoc->GetDlgDataSource(i));
					break;
				}
			}
		}
	}
		
	CFtrLayer *pLayer0;
	CValueTable tab;
	
	CPtrArray arr;
	CFtrArray arr2, arr3;
	
	CFeature *pFtr0, *pFtr1;
		
	CDlgDataSource *pDS1 = pDoc->GetDlgDataSource();
	
	CGrTrim trim;
	trim.InitTrimPolygon(boundPts,npt,50*pDS1->GetScale()*0.01);
	
	Envelope evlp;
	evlp.CreateFromPts(boundPts,npt);

	int nSum = 0;

	for( int k=0; k<arrDSs.GetSize(); k++)
	{
		pDS0 = arrDSs[k];
		if( pDS0==NULL || pDS0==pDS1 )
			continue;
		
		int nLay = pDS0->GetFtrLayerCount();
		
		for ( int i=0;i<nLay;i++)
		{
			pLayer0 = pDS0->GetFtrLayerByIndex(i);
			if( !pLayer0 || !pLayer0->IsVisible() )continue;

			nSum += pLayer0->GetValidObjsCount();
		}
	}

	CUndoFtrs undo(m_pEditor,Name());

	GProgressStart(nSum);

	for( k=0; k<arrDSs.GetSize(); k++)
	{
		pDS0 = arrDSs[k];
		if( pDS0==NULL || pDS0==pDS1 )
			continue;

		CString name = pDS0->GetName();
		char fname[256];
		char ext[256];

		_splitpath(name,NULL,NULL,fname,ext);

		name = CString(fname)+ext;

		int nLay = pDS0->GetFtrLayerCount();

		for ( int i=0;i<nLay;i++)
		{
			pLayer0 = pDS0->GetFtrLayerByIndex(i);
			if( !pLayer0 || !pLayer0->IsVisible() )continue;
			
			int nObj = pLayer0->GetObjectCount();		
			for (int j=0;j<nObj;j++)
			{
				pFtr0 = pLayer0->GetObject(j);
				if (pFtr0)
				{
					GProgressStep();

					arr.RemoveAll();
					arr2.RemoveAll();
					arr3.RemoveAll();
					
					if( m_bClip )
					{
						CBatExportMapsCommand::TrimObj(pFtr0->GetGeometry(),&arr,evlp,boundPts,npt,&trim,FALSE,TRUE);
						for( int t=0; t<arr.GetSize(); t++)
						{
							pFtr1 = pFtr0->Clone();
							if( !pFtr1 )continue;
							
							pFtr1->SetCode("");
							
							pFtr1->SetGeometry((CGeometry*)arr[t]);
							pFtr1->SetID(OUID());
							
							arr2.Add(pFtr1);
						}
					}
					else
					{
						pFtr1 = pFtr0->Clone();
						if( !pFtr1 )continue;
						
						pFtr1->SetCode("");					
						pFtr1->SetID(OUID());
						
						arr2.Add(pFtr1);
					}

					CValueTable xTab;
					xTab.BeginAddValueItem();
					GETXDS(m_pEditor)->GetXAttributes(pFtr0, xTab);
					xTab.EndAddValueItem();
					
					CBatExportMapsCommand::AddFtrsToDS(arr2,pLayer0,pDS0,pDS1,xTab,&arr3);

					for( int t=0; t<arr3.GetSize(); t++)
					{						
						undo.arrNewHandles.Add(FtrToHandle(arr3[t]));
					}
				}
			}
		}
	}

	undo.Commit();

	GProgressEnd();
}



/////////////////////////////////////////////////////////////////////
// CAlignCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CAlignCommand,CEditCommand)

CAlignCommand::CAlignCommand()
{
	m_nAlign = Left;
}

CAlignCommand::~CAlignCommand()
{
}

void CAlignCommand::Abort()
{
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	CEditCommand::Abort();
}


CString CAlignCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_ALIGN);
}

void CAlignCommand::Start()
{
	if( !m_pEditor )return;
	int num;
	CDlgDoc *pDoc = (CDlgDoc*)m_pEditor;
	(CDlgDoc*)m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( num>0 )
	{
		m_nStep = 0;		
		CCommand::Start();
	
		PT_3D pt;
		PtClick(pt,SELSTAT_NONESEL);
		m_nExitCode = CMPEC_STARTOLD;		
		return;
	}
	
	CEditCommand::Start();

	m_pEditor->OpenSelector(SELMODE_MULTI);
}


void CAlignCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( !CanGetSelObjs(flag) )return;
	   EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		ObjInfo item;
		CArray<ObjInfo,ObjInfo> arrItems;

		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		if( num<=1 )
		{
			m_nStep = 0;
			return;
		}

		for( int i=0; i<num; i++)
		{
			item.pFtr = HandleToFtr(handles[i]);
			item.pGeo = HandleToFtr(handles[i])->GetGeometry();

			if( item.pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				GrBuffer buf;
				PDOC(m_pEditor)->GetDlgDataSource()->DrawFeature(item.pFtr,&buf);
				item.e = buf.GetEnvelope();
				item.center = PT_3D(item.e.CenterX(),item.e.CenterY(),0);
			}
			else
			{
				GrBuffer buf;
				item.pGeo->Draw(&buf,PDOC(m_pEditor)->GetDlgDataSource()->GetSymbolDrawScale());

				item.e = buf.GetEnvelope();
				item.pGeo->GetCenter(NULL,&item.center);
			}

			arrItems.Add(item);			
		}

		CUndoBatchAction undo(m_pEditor,Name());

		ObjInfo item0 = arrItems[0];
		double dx, dy;
		double m[16];
		for( i=1; i<num; i++)
		{
			item = arrItems[i];
			dx = dy = 0;

			switch(m_nAlign)
			{
			case Left:
				dx = item0.e.m_xl - item.e.m_xl;
				break;
			case Right:
				dx = item0.e.m_xh - item.e.m_xh;
				break;
			case Up:
				dy = item0.e.m_yh - item.e.m_yh;
				break;
			case Down:
				dy = item0.e.m_yl - item.e.m_yl;
				break;
			case CenterH:
				dy = item0.center.y - item.center.y;
				break;
			case CenterV:
				dx = item0.center.x - item.center.x;
				break;
			}

			Matrix44FromMove(dx,dy,0,m);

			m_pEditor->DeleteObject(FtrToHandle(item.pFtr),FALSE);
			item.pGeo->Transform(m);
			m_pEditor->RestoreObject(FtrToHandle(item.pFtr));

			
			CUndoTransform undo0(m_pEditor,Name());
			
			undo0.arrHandles.Add(FtrToHandle(item.pFtr));
			memcpy(undo0.matrix,m,sizeof(m));

			undo.AddAction(&undo0);
		}

		undo.Commit();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}
	
	CEditCommand::PtClick(pt,flag);
}


void CAlignCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (long)(m_nAlign);
	tab.AddValue("Align",&CVariantEx(var));
}


void CAlignCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("AlignCommand",StrFromResID(IDS_CMDNAME_ALIGN));
	
	param->BeginOptionParam("Align",StrFromResID(IDS_ALIGN_TYPE));

	char names[6][100];
	memset(names,0,sizeof(names));

	strcpy(names[0],StrFromResID(IDS_ALIGN_LEFT));
	strcpy(names[1],StrFromResID(IDS_ALIGN_RIGHT));
	strcpy(names[2],StrFromResID(IDS_ALIGN_TOP));
	strcpy(names[3],StrFromResID(IDS_ALIGN_BOTTOM));
	strcpy(names[4],StrFromResID(IDS_ALIGN_CENTERH));
	strcpy(names[5],StrFromResID(IDS_ALIGN_CENTERV));
	
	for( int i=0; i<6; i++)
	{
		param->AddOption(names[i],i,' ',m_nAlign==i);
	}

	param->EndOptionParam();
}


void CAlignCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"Align",var) )
	{
		m_nAlign = (long)(_variant_t)*var;
		
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////////////////// CDlgEditDirPointCommand //////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(CDlgEditDirPointCommand,CEditCommand)
CDlgEditDirPointCommand::CDlgEditDirPointCommand()
{
	m_pSel = NULL;
	m_bSynchXY = TRUE;
}

CDlgEditDirPointCommand::~CDlgEditDirPointCommand()
{
	if (m_pSel)
	{
		delete m_pSel;
		m_pSel = NULL;
	}
}


CString CDlgEditDirPointCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_MODIFYPOINTSCALE);
}

void CDlgEditDirPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("EditDirPoint",Name());

	param->AddParam("SynchXY",(bool)m_bSynchXY,StrFromResID(IDS_CMDPLANE_SYNCHXYSCALE));
	
	param->AddParam(PF_EDITVERTEX_DEL,char(VK_DELETE),StrFromResID(IDS_CMDPLANE_DELVERTEX));
	
	CCommand::FillShowParams(param,bForLoad);
}

void CDlgEditDirPointCommand::GetParams(CValueTable& tab)
{	
	_variant_t var;
	var = (long)(m_bSynchXY);
	tab.AddValue("SynchXY",&CVariantEx(var));

	CCommand::GetParams(tab);
}

void CDlgEditDirPointCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	BOOL bChgPt = FALSE;
	if( tab.GetValue(0,"SynchXY",var) )
	{
		m_bSynchXY = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_EDITVERTEX_DEL,var) )
	{
		int key = (char)(BYTE)(_variant_t)*var;
		if (key==VK_DELETE)
		{
			CUndoFtrs undo(m_pEditor,Name());

			if( m_pFtr )
			{
				m_pEditor->DeleteObject(FtrToHandle(m_pFtr));
				undo.AddOldFeature(FtrToHandle(m_pFtr));
			}

			undo.Commit();

			m_pEditor->OnSelectChanged(TRUE);	
			m_pEditor->UpdateDrag(ud_ClearDrag);
			Finish();
	
		}		
	}
	CCommand::SetParams(tab,bInit);
}


void CDlgEditDirPointCommand::Start()
{
	m_nStep = 0;
	m_pEditor->OpenSelector();
//	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetCursorType,(CObject*)CURSOR_NORMAL);

	m_pEditor->UpdateDrag(ud_ClearDrag);
	m_pEditor->GetSelection()->DeselectAll();
	m_pEditor->OnSelectChanged();

	CCommand::Start();
}


PT_KEYCTRL CDlgEditDirPointCommand::FindNearestKeyCtrlPt(CGeoPoint *pGeo, PT_3D spt, double r, CCoordSys *pCS)
{
	PT_3DEX pts[4];
	pts[0] = pGeo->GetDataPoint(0);
	pts[1] = pGeo->GetCtrlPoint2(0);
	pts[2] = pGeo->GetCtrlPoint2(1);
	pts[3] = pGeo->GetCtrlPoint2(2);

	double dis, mindis = -1;
	int k = -1;
	for( int i=0; i<4; i++)
	{
		PT_3DEX pt = pts[i];
		pCS->GroundToClient(&pt,&pts[i]);

		dis = GraphAPI::GGet2DDisOf2P(spt,(PT_3D)pts[i]);

		if( mindis<0 || dis<mindis )
		{
			mindis = dis;
			k = i;
		}
	}

	if( mindis>=0 && mindis<=r )
	{
		PT_KEYCTRL ctrl;

		if( k==0 )
		{
			ctrl.index = k;
			ctrl.type = PT_KEYCTRL::typeKey;
		}
		else
		{
			ctrl.index = k-1;
			ctrl.type = PT_KEYCTRL::typeCtrl;
		}

		return ctrl;	
	}

	return PT_KEYCTRL();
}


void CDlgEditDirPointCommand::DrawCtrlPts(CGeoPoint *pGeo)
{
	GrBuffer buf;

	PT_3DEX pt0 = pGeo->GetDataPoint(0);

	PT_3DEX pts[3];
	pts[0] = pGeo->GetCtrlPoint2(0);
	pts[1] = pGeo->GetCtrlPoint2(1);
	pts[2] = pGeo->GetCtrlPoint2(2);

	CCoordWnd cw = m_pEditor->GetCoordWnd();

	double len = 5 / cw.m_pViewCS->CalcScale();
	long color = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_HILITECOLOR,RGB(255,255,255));
	buf.BeginLineString(color,0);

	for( int i=0; i<3; i++)
	{
		if( i==0 && !pGeo->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
		{
			double len2 = len * 0.5;
			buf.MoveTo(&PT_3D(pts[i].x-len2,pts[i].y-len2,pt0.z));
			buf.LineTo(&PT_3D(pts[i].x+len2,pts[i].y-len2,pt0.z));
			buf.LineTo(&PT_3D(pts[i].x+len2,pts[i].y+len2,pt0.z));
			buf.LineTo(&PT_3D(pts[i].x-len2,pts[i].y+len2,pt0.z));
			buf.LineTo(&PT_3D(pts[i].x-len2,pts[i].y-len2,pt0.z));
		}
		else
		{
			
			buf.MoveTo(&PT_3D(pts[i].x-len,pts[i].y,pt0.z));
			buf.LineTo(&PT_3D(pts[i].x,pts[i].y-len,pt0.z));
			buf.LineTo(&PT_3D(pts[i].x+len,pts[i].y,pt0.z));
			buf.LineTo(&PT_3D(pts[i].x,pts[i].y+len,pt0.z));
			buf.LineTo(&PT_3D(pts[i].x-len,pts[i].y,pt0.z));
		}
	}

	buf.End();

	m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
}


void CDlgEditDirPointCommand::PtClick(PT_3D &pt, int flag)
{
	CSelection *pselection = m_pEditor->GetSelection();

	if( m_nStep==0 )
	{
		if( !(flag==SELSTAT_DRAGSEL || flag==SELSTAT_POINTSEL) )
		{
			return;
		}

		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		if( num!=1 )
		{
			return;
		}

		CPFeature pFtr = HandleToFtr(handles[0]);
		if( !pFtr || !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{			
			return;
		}
		m_pFtr = pFtr;
		
		DrawCtrlPts((CGeoPoint*)m_pFtr->GetGeometry());
		
		GotoState(PROCSTATE_PROCESSING);
		
		m_nStep = 1;
		return;
	}

	if( m_nStep==1 )
	{
		CCoordWnd pencode = m_pEditor->GetCoordWnd();
		double r = pselection->GetSelectRadius()*pencode.GetScaleOfSearchCSToViewCS();
		PT_3D spt = pt;
		pencode.m_pSearchCS->GroundToClient(&pt,&spt);
			
		CGeometry *pGeo = m_pFtr->GetGeometry();
		if (!pGeo) 
			return;
		
		PT_KEYCTRL keyPt = FindNearestKeyCtrlPt((CGeoPoint*)pGeo,spt,r,pencode.m_pSearchCS);
		
		if (!keyPt.IsValid())
		{
			if( flag==SELSTAT_POINTSEL )
			{
				CSelection * pSel = m_pEditor->GetSelection();
				int num;
				const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
				if( num!=1 )
				{
					return;
				}
				
				CPFeature pFtr = HandleToFtr(handles[0]);
				if( !pFtr || !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
				{
					return;
				}
				m_pFtr = pFtr;
				
				DrawCtrlPts((CGeoPoint*)m_pFtr->GetGeometry());
				
				GotoState(PROCSTATE_PROCESSING);				
			}
			return;
		}

		m_ptCtrl = keyPt;
		m_nStep = 2;

		m_pEditor->CloseSelector();
	}
	else if( m_nStep==2 )
	{
		CFeature *pNewFtr = m_pFtr->Clone();

		CGeoPoint *pGeo = (CGeoPoint*)pNewFtr->GetGeometry();
		if (!pGeo) 
			return;
		
		PT_KEYCTRL keyPt = m_ptCtrl;

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(m_pFtr);

		
		
		CUndoFtrs undo(m_pEditor,Name());
		
		if( keyPt.type==PT_KEYCTRL::typeKey )
		{	
			PT_3DEX t = pGeo->GetDataPoint(0);
			
			t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;
			t.pencode = penLine;
			pGeo->SetDataPoint(keyPt.index,t);			
		}
		else if( keyPt.type==PT_KEYCTRL::typeCtrl )
		{
			PT_3D t;						
			t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
			pGeo->SetCtrlPoint2(keyPt.index,t,m_bSynchXY);						
		}
		
		m_pEditor->AddObject(pNewFtr,pLayer->GetID());
		GETXDS(m_pEditor)->CopyXAttributes(m_pFtr, pNewFtr);
		m_pEditor->DeleteObject(FtrToHandle(m_pFtr),FALSE);
		undo.AddOldFeature(FtrToHandle(m_pFtr));
		undo.AddNewFeature(FtrToHandle(pNewFtr));
		undo.Commit();
		
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->OnSelectChanged(TRUE);			

		Finish();
		m_pEditor->CloseSelector();
		m_nStep = 3;	
	}

	return;
}



void CDlgEditDirPointCommand::PtMove(PT_3D &pt)
{
	if( m_nStep==2 && m_pFtr!=NULL )
	{
		GrBuffer buf;

		CFeature *pFtr = m_pFtr;
		if( !pFtr )return;	

		m_ptDragEnd = pt;	
		
		CFeature *pObj = pFtr->Clone();
		if( !pObj )return;
		
		CGeoPoint *pGeo = (CGeoPoint*)pObj->GetGeometry();

		if( m_ptCtrl.type==PT_KEYCTRL::typeKey )
		{
			PT_3DEX t;
			t = pGeo->GetDataPoint(0);
			t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;
			pGeo->SetDataPoint(0,t);		

		}
		if( m_ptCtrl.type==PT_KEYCTRL::typeCtrl )
		{
			PT_3D t;
			t = pGeo->GetCtrlPoint2(m_ptCtrl.index);
			t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
			pGeo->SetCtrlPoint2(m_ptCtrl.index,t,m_bSynchXY);

		}

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		
		if( pLayer )
		{
			pDS->DrawFeature(pObj,&buf,TRUE,0,pLayer->GetName());
			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
			
			delete pObj;
			return;
		}
		else
		{
			delete pObj;
		}
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCopyAssistLineCommand,CEditCommand)

CCopyAssistLineCommand::CCopyAssistLineCommand()
{
	m_nStep = 0;
}

CCopyAssistLineCommand::~CCopyAssistLineCommand()
{
}

CString CCopyAssistLineCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COPY_ASSISTLINE);
}

void CCopyAssistLineCommand::Start()
{
	if( !m_pEditor )return;
	m_nStep = 0;
	
	CEditCommand::Start();

	m_pEditor->CloseSelector();
}

void CCopyAssistLineCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)m_strLayerSrc;
	tab.AddValue("LayerSrc",&CVariantEx(var));

	var = (LPCTSTR)m_strLayerDest;
	tab.AddValue("LayerDest",&CVariantEx(var));

}

void CCopyAssistLineCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CopyAssistLine",Name());
	
	param->AddLayerNameParam("LayerSrc",(LPCTSTR)m_strLayerSrc,StrFromResID(IDS_CMDPLANE_DCURVELAYER));

	param->AddLayerNameParam("LayerDest",(LPCTSTR)m_strLayerDest,StrFromResID(IDS_CMDPLANE_ASSISTLAYER));
	
}


void CCopyAssistLineCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;	
	if( tab.GetValue(0,"LayerSrc",var) )
	{
		m_strLayerSrc = (LPCTSTR)(_bstr_t)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"LayerDest",var) )
	{
		m_strLayerDest = (LPCTSTR)(_bstr_t)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}


void CCopyAssistLineCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		CDlgDoc *pDoc = PDOC(m_pEditor);
		CDlgDataSource *pDS = pDoc->GetDlgDataSource();

		int num = pDS->GetFtrLayerCount();
		CArray<CFeature*, CFeature*> arrObjs;

		CFtrLayer *pDestLayer = NULL;
		
		int i,j, lSum = 0, nObj;
		for ( i=0; i<num; i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			
			if( !pLayer||pLayer->IsLocked()||!pLayer->IsVisible() )continue;
			if( CheckNameForLayerCode(pDS,pLayer->GetName(),m_strLayerDest) )
			{
				pDestLayer = pLayer;
			}

			if( !CheckNameForLayerCode(pDS,pLayer->GetName(),m_strLayerSrc) )
				continue;
			
			nObj = pLayer->GetObjectCount();
			for( j=0; j<nObj; j++ )
			{
				CFeature *pObj = pLayer->GetObject(j);
				if( !pObj )continue;
				if( !pObj->IsVisible() )
					continue;

				if( pObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoParallel)) || pObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
				{
					arrObjs.Add(pObj);
					lSum++;
				}

			}
		}

		CUndoNewDelLayer undo1(m_pEditor,Name());
		if( pDestLayer==NULL )
		{
			pDestLayer = pDS->CreateFtrLayer(m_strLayerDest);
			if( !pDestLayer )
				return;

			pDestLayer->EnableVisible(false);
			pDoc->AddFtrLayer(pDestLayer);

			undo1.isDel = FALSE;
			undo1.arrObjs.Add(pDestLayer);
		}
		
		GProgressStart(lSum);
		
		CUndoFtrs undo2(m_pEditor,Name());
		for( i=0; i<lSum; i++)
		{
			GProgressStep();
			ProcessObj(arrObjs[i],pDoc,pDestLayer,undo2);
		}

		GProgressEnd();
		
		CUndoBatchAction undo(m_pEditor,Name());
		
		undo.AddAction(&undo1);
		undo.AddAction(&undo2);
		undo.Commit();

		Finish();
		m_nStep = 1;
	}
	
	CCommand::PtClick(pt, flag);
}

void CCopyAssistLineCommand::ProcessObj(CFeature *pFtr, CDlgDoc *pDoc, CFtrLayer *pFtrLayer, CUndoFtrs& undo)
{
	CGeometry *pGeo = pFtr->GetGeometry();
	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
	{
		CGeoParallel *p = (CGeoParallel*)pGeo;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		p->GetParallelShape(arrPts);

		CFeature *pNewFtr = pFtrLayer->CreateDefaultFeature(pDoc->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);

		pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());

		pDoc->AddObject(pNewFtr,pFtrLayer->GetID());

		undo.AddNewFeature(FtrToHandle(pNewFtr));
	}
	else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
	{
		CGeoDCurve *p = (CGeoDCurve*)pGeo;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		p->GetAssistShape(arrPts);

		CFeature *pNewFtr = pFtrLayer->CreateDefaultFeature(pDoc->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);

		pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		
		pDoc->AddObject(pNewFtr,pFtrLayer->GetID());
		
		undo.AddNewFeature(FtrToHandle(pNewFtr));
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCopyBaselineCommand,CEditCommand)

CCopyBaselineCommand::CCopyBaselineCommand()
{
	m_nStep = 0;
}

CCopyBaselineCommand::~CCopyBaselineCommand()
{
}

CString CCopyBaselineCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COPY_BASELINE);
}

void CCopyBaselineCommand::Start()
{
	if( !m_pEditor )return;
	m_nStep = 0;
	
	CEditCommand::Start();

	m_pEditor->CloseSelector();
}

void CCopyBaselineCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)m_strLayerSrc;
	tab.AddValue("LayerSrc",&CVariantEx(var));

	var = (LPCTSTR)m_strLayerDest;
	tab.AddValue("LayerDest",&CVariantEx(var));

}

void CCopyBaselineCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CopyAssistLine",Name());
	
	param->AddLayerNameParam("LayerSrc",(LPCTSTR)m_strLayerSrc,StrFromResID(IDS_SRC_LAYER));

	param->AddLayerNameParam("LayerDest",(LPCTSTR)m_strLayerDest,StrFromResID(IDS_DES_LAYER));
	
}


void CCopyBaselineCommand::SetParams(CValueTable& tab,BOOL bInit)
{	
	const CVariantEx *var;	
	if( tab.GetValue(0,"LayerSrc",var) )
	{
		m_strLayerSrc = (LPCTSTR)(_bstr_t)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"LayerDest",var) )
	{
		m_strLayerDest = (LPCTSTR)(_bstr_t)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}


void CCopyBaselineCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		CDlgDoc *pDoc = PDOC(m_pEditor);
		CDlgDataSource *pDS = pDoc->GetDlgDataSource();

		int num = pDS->GetFtrLayerCount();
		CArray<CFeature*, CFeature*> arrObjs;

		CFtrLayer *pDestLayer = NULL;
		
		int i,j, lSum = 0, nObj;
		for ( i=0; i<num; i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			
			if( !pLayer||pLayer->IsLocked()||!pLayer->IsVisible() )continue;
			if( CheckNameForLayerCode(pDS,pLayer->GetName(),m_strLayerDest) )
			{
				pDestLayer = pLayer;
			}

			if( !CheckNameForLayerCode(pDS,pLayer->GetName(),m_strLayerSrc) )
				continue;
			
			nObj = pLayer->GetObjectCount();
			for( j=0; j<nObj; j++ )
			{
				CFeature *pObj = pLayer->GetObject(j);
				if( !pObj )continue;
				if( !pObj->IsVisible() )
					continue;

				if( pObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
				{
					arrObjs.Add(pObj);
					lSum++;
				}

			}
		}

		CUndoNewDelLayer undo1(m_pEditor,Name());
		if( pDestLayer==NULL )
		{
			pDestLayer = pDS->CreateFtrLayer(m_strLayerDest);
			if( !pDestLayer )
				return;

			pDestLayer->EnableVisible(false);
			pDoc->AddFtrLayer(pDestLayer);

			undo1.isDel = FALSE;
			undo1.arrObjs.Add(pDestLayer);
		}
		
		GProgressStart(lSum);
		
		CUndoFtrs undo2(m_pEditor,Name());
		for( i=0; i<lSum; i++)
		{
			GProgressStep();
			ProcessObj(arrObjs[i],pDoc,pDestLayer,undo2);
		}

		GProgressEnd();

		CUndoBatchAction undo(m_pEditor,Name());

		undo.AddAction(&undo1);
		undo.AddAction(&undo2);
		undo.Commit();

		Finish();
		m_nStep = 1;
	}
	
	CCommand::PtClick(pt, flag);
}

void CCopyBaselineCommand::ProcessObj(CFeature *pFtr, CDlgDoc *pDoc, CFtrLayer *pFtrLayer, CUndoFtrs& undo)
{
	CGeometry *pGeo = pFtr->GetGeometry();
	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
	{
		CGeoDCurve *p = (CGeoDCurve*)pGeo;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		p->GetBaseShape(arrPts);
		
		CFeature *pNewFtr = pFtrLayer->CreateDefaultFeature(pDoc->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);

		pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		
		pDoc->AddObject(pNewFtr,pFtrLayer->GetID());
		
		undo.AddNewFeature(FtrToHandle(pNewFtr));
	}
	else
	{
		CGeometry *pGeo = pFtr->GetGeometry();
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeo->GetShape(arrPts);

		int geoClass = pGeo->GetClassType();
		if( geoClass==CLS_GEOPARALLEL )
			geoClass = CLS_GEOCURVE;
		
		CFeature *pNewFtr = pFtrLayer->CreateDefaultFeature(pDoc->GetDlgDataSource()->GetScale(),geoClass);

		pNewFtr->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());	
		
		pDoc->AddObject(pNewFtr,pFtrLayer->GetID());
		
		undo.AddNewFeature(FtrToHandle(pNewFtr));
	}


}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CMapDecorateCommand_New,CEditCommand)

BOOL CMapDecorateCommand_New::m_bStaticValid = FALSE;
MapDecorate::CMapDecorator CMapDecorateCommand_New::m_MapDecorator;

CMapDecorateCommand_New::CMapDecorateCommand_New()/*:m_dlg(AfxGetMainWnd())*/
{
	m_nOperation = opNone;
	m_bSortBound = TRUE;

	m_pDlg = NULL;
}

CMapDecorateCommand_New::~CMapDecorateCommand_New()
{
	if(m_pDlg)
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}
}


CString CMapDecorateCommand_New::Name()
{
	return StrFromResID(IDS_CMDNAME_MAPDECORATE);
}

void CMapDecorateCommand_New::Start()
{
	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
	long scale = pDS->GetScale();

	m_MapDecorator.m_bNew = TRUE;		//新版图廓整饰
	m_MapDecorator.InitData(pDS);
	m_MapDecorator.LoadParams(scale);
	m_MapDecorator.LoadDatas(NULL);

//根据比例尺，自动选择相应的对话框
	if( 5000 == scale || 10000 == scale )
	{
		m_pDlg = new CMapDecorateDlg_5KN;
		if( !m_pDlg ) return;
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		m_pDlg = new CMapDecorateDlg_25KN;
		if( !m_pDlg ) return;
	}
	else if( 500 <= scale || 2000 >= scale )
	{
		m_pDlg = new CMapDecorateDlg_2KO;
		if( !m_pDlg ) return;
	}
	else
	{
		GOutPut(StrFromResID(IDS_MD_TIP_NEW));
		Abort();
		m_nExitCode = CMPEC_NULL;
		return;
	}
//

	Envelope e = pDS->GetBound();
	m_MapDecorator.CalcMapNumberAndNearmap(pDS,PT_3D(e.CenterX(),e.CenterY(),0));

	m_pDlg->m_pMapDecorator = &m_MapDecorator;

	m_bSortBound = TRUE;

	CString title0,path;
	path = StrFromResID(IDS_MD_TITLE_NEW);
/*	path = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager()->GetConfigLibItemByScale(scale).GetPath();

	int pos = path.ReverseFind('\\');
	if( pos>0 )
	{
		CString path1 = path;
		path1 = path1.Left(pos);

		pos = path1.ReverseFind('\\');
		if( pos>0 )
		{
			path = path.Mid(pos+1);
		}
	}
*/

//
	if( 5000 == scale || 10000 == scale )
	{
		m_pDlg->Create(IDD_MAPSHOW_5KN_DIALOG,AfxGetMainWnd());
	}
	else if( 500 == scale || 1000 == scale || 2000 == scale)
	{
		m_pDlg->Create(IDD_MAPSHOW_DIALOG,AfxGetMainWnd());
	}
	else if( 25000 == scale || 50000 == scale || 100000 == scale)
	{
		m_pDlg->Create(IDD_MAPSHOW_25KN_DIALOG,AfxGetMainWnd());
	}
//
	m_pDlg->CenterWindow();
	m_pDlg->ShowWindow(SW_SHOW);

	m_pDlg->GetWindowText(title0);

	title0 = title0 + "(" + path + ")";
	m_pDlg->SetWindowText(title0);

	m_pDlg->m_pCmd = this;
}


void CMapDecorateCommand_New::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	CString cmd;
	
	if( tab.GetValue(0,"Command",var) )
	{
		cmd = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		if( cmd.CompareNoCase("SelectBound")==0 )
		{
			m_pEditor->OpenSelector(SELMODE_SINGLE,SELFILTER_CURVE|SELFILTER_SURFACE);
			m_nOperation = opSelectBound;
			m_nStep = 0;

			m_pDlg->ShowWindow(SW_HIDE);
		}
		else if( cmd.CompareNoCase("SelectCorner")==0 )
		{
			m_pEditor->CloseSelector();
			m_nOperation = opSelectCorner;
			m_nStep = 0;

			m_pDlg->ShowWindow(SW_HIDE);
		}
		else if( cmd.CompareNoCase("OK")==0 )
		{	
			m_MapDecorator.Build(PDOC(m_pEditor),NULL,PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),FALSE,m_bSortBound);
	
			CUndoFtrs undo(m_pEditor,Name());
			undo.arrNewHandles.Copy(m_MapDecorator.m_arrNewObjHandles);
			undo.Commit();

			Finish();
			m_nExitCode = CMPEC_NULL;

			return;

		}
		else if( cmd.CompareNoCase("Cancel")==0 )
		{
			m_pDlg->ShowWindow(SW_HIDE);
			Abort();
			m_nExitCode = CMPEC_NULL;
			return;
		}
	}
	
	CEditCommand::SetParams(tab,bInit);
}


void CMapDecorateCommand_New::PtClick(PT_3D &pt, int flag)
{
	if( m_nOperation==opSelectBound )
	{
		if( m_nStep==0 )
		{	
			if( !CanGetSelObjs(flag) )return;
			EditStepOne();
		}
		if( m_nStep==1 )
		{
			CSelection* pSel = m_pEditor->GetSelection();

			FTR_HANDLE hFtr = pSel->GetLastSelectedObj();
			
			if( !hFtr )return;

			CFeature *pFtr = HandleToFtr(hFtr);
			if( pFtr )
			{
				CGeometry *pGeo = pFtr->GetGeometry();
				if( pGeo )
				{
					if( (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve))||pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface))) && pGeo->GetDataPointSum()>=4 )
					{
						CArray<PT_3DEX,PT_3DEX> arrPts;
						pGeo->GetShape(arrPts);

						CArray<PT_3D,PT_3D> arrPts2;
						arrPts2.SetSize(arrPts.GetSize());
						for( int i=0; i<arrPts.GetSize(); i++)
						{
							arrPts2[i] = arrPts[i];
						}

						//自动生成的图廓，图廓节点顺序是正确的
						CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(pFtr);
						if( stricmp(pLayer->GetName(),"TK_inner_bound")==0 )
						{
							m_bSortBound = FALSE;
						}

						m_pDlg->SetBound(arrPts2.GetData());

						m_pDlg->ShowWindow(SW_SHOW);
					}
				}
			}

			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();

			//修复选择内图廓后，窗口没有发生变化的Bug
			CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
			m_MapDecorator.CalcMapBound();
			Envelope e;
			e.CreateFromPts(m_MapDecorator.m_innerBound,4);
			m_MapDecorator.CalcMapNumberAndNearmap(pDS,PT_3D(e.CenterX(),e.CenterY(),0));
			
			m_pDlg->m_strMapNum = ((MapDecorate::CNumber*)m_MapDecorator.GetObj("Number"))->number;
			m_pDlg->UpdateData(FALSE);

			m_nStep = 2;
		}

		CCommand::PtClick(pt,flag);
	}
	else if( m_nOperation==opSelectCorner )
	{
		if( m_nStep==0 )
		{
			m_pDlg->UpdateData(TRUE);
			float dx = m_pDlg->m_fMapWid*0.01*PDOC(m_pEditor)->GetDlgDataSource()->GetScale();
			float dy = m_pDlg->m_fMapHei*0.01*PDOC(m_pEditor)->GetDlgDataSource()->GetScale();
			pt.x = floor(pt.x/dx+0.5)*dx;
			pt.y = floor(pt.y/dy+0.5)*dy;
			m_pDlg->SetBoundCorner(pt);
			m_nStep = 1;

			m_pDlg->ShowWindow(SW_SHOW);

/*			CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
			m_MapDecorator.CalcMapBound();
			Envelope e;
			e.CreateFromPts(m_MapDecorator.m_innerBound,4);
			m_MapDecorator.CalcMapNumberAndNearmap(pDS,PT_3D(e.CenterX(),e.CenterY(),0));
			
			m_pDlg->m_strMapNum = ((MapDecorate::CNumber*)m_MapDecorator.GetObj("Number"))->number;
			m_pDlg->UpdateData(FALSE);*/
		}
	}
}


void CMapDecorateCommand_New::PtReset(PT_3D &pt)
{
	m_nStep = 0;
	if( m_pDlg )
		m_pDlg->ShowWindow(SW_SHOW);
}



//////////////////////////////////////////////////////////////////////
// CInterpolateHeiPointCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CInterpolateHeiPointCommand,CCommand)

CInterpolateHeiPointCommand::CInterpolateHeiPointCommand()
{
//	m_bAutoClosed = TRUE;
	m_nStep = -1;
	m_fGridSize = 10.0;
	m_fRandRange = 5.0;
	m_pDrawProc = NULL;
	m_pFtr = NULL;
	
	strcat(m_strRegPath,"\\InterpolateHeiPoint");
}

CInterpolateHeiPointCommand::~CInterpolateHeiPointCommand()
{
	if( m_pDrawProc )delete m_pDrawProc;

}

CString CInterpolateHeiPointCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_INTERPOLATE_HEIPOINTS_REGION);
}

void CInterpolateHeiPointCommand::GetParams(CValueTable &tab)
{
	CCommand::GetParams(tab);
	
	_variant_t var;
	var = (_bstr_t)(LPCTSTR)(m_strLayerHeiPoint);
	tab.AddValue("LayerHeiPoint",&CVariantEx(var));

	var = (float)m_fGridSize;
	tab.AddValue(PF_GRIDSIZE,&CVariantEx(var));

	var = (float)m_fRandRange;
	tab.AddValue("fRandRange",&CVariantEx(var));
}


void CInterpolateHeiPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("InterpolateLidar",Name());
	
	param->AddLayerNameParam("LayerHeiPoint",m_strLayerHeiPoint,StrFromResID(IDS_CHKCMD_Z_LAYERS),NULL,LAYERPARAMITEM_POINT);
	param->AddParam(PF_GRIDSIZE,m_fGridSize,StrFromResID(IDS_CMDPLANE_GRIDSIZE));	
	param->AddParam("fRandRange",m_fRandRange,StrFromResID(IDS_CMDPLANE_RAND_RANGE));
}



void CInterpolateHeiPointCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"LayerHeiPoint",var) )
	{
		m_strLayerHeiPoint = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_GRIDSIZE,var) )
	{
		m_fGridSize = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"fRandRange",var) )
	{
		m_fRandRange = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	CCommand::SetParams(tab);
}


void CInterpolateHeiPointCommand::Start()
{
	m_nStep = 0;
	m_fGridSize = 5.0;
	
	m_idsOld.RemoveAll();
	m_idsNew.RemoveAll();

	m_pDrawProc = new CDrawCurveProcedure;
	if( !m_pDrawProc )return;
	m_pDrawProc->Init(m_pEditor);
	m_pFtr = new CFeature;
	if(!m_pFtr) return;
	CGeoCurve *pGeo = new CGeoCurve;
	if(!pGeo) return;

	m_pFtr->SetGeometry(pGeo);
	m_pDrawProc->m_pGeoCurve = pGeo;
	UpdateParams(FALSE);
	m_pDrawProc->Start();
	
	m_pDrawProc->m_pGeoCurve->EnableClose(TRUE);
	
	CCommand::Start();

}

void CInterpolateHeiPointCommand::Abort()
{	
	m_nStep = -1;
	m_idsOld.RemoveAll();
	m_idsNew.RemoveAll();
	m_pEditor->CloseSelector();
	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetConstDragLine,NULL);
	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Detach_Accubox);
	m_pEditor->RefreshView();

	if( m_pDrawProc )
	{
		if( !IsProcOver(m_pDrawProc) )
			m_pDrawProc->Abort();
		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}
	if(m_pFtr) 
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}
	CCommand::Abort();

}


void CInterpolateHeiPointCommand::Finish()
{
	m_nStep = -1;
	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetConstDragLine,NULL);
	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetVariantDragLine,NULL);
	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Detach_Accubox);
	m_pEditor->CloseSelector();
	
	if (m_pFtr)
	{
		delete m_pFtr;
		m_pFtr = NULL;
	}

	if( m_pDrawProc )
	{		
		delete m_pDrawProc;
		m_pDrawProc = NULL;
	}

	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
	
	CCommand::Finish();
}

int CInterpolateHeiPointCommand::GetState()
{
	return GetStateWithSonProcedure(m_pDrawProc);
}


CProcedure *CInterpolateHeiPointCommand::GetActiveSonProc(int nMsgType)
{
	if( nMsgType==msgPtClick||nMsgType==msgPtMove||nMsgType==msgPtReset )
		return NULL;
	
	return m_pDrawProc;
}

void CInterpolateHeiPointCommand::PtClick(PT_3D &pt, int flag)
{
	//画线
	if( m_nStep==0 || m_nStep==1 )
	{
		if( !m_pFtr )return;
		m_pDrawProc->PtClick(pt,flag);
		m_nStep = 1;
	}
	
	//确认并执行
	if( m_nStep==2 )
	{
		m_pEditor->UpdateDrag(ud_SetConstDrag);

		if( !m_pFtr )return;

		CGeoCurve *m_pObj = (CGeoCurve*)m_pFtr->GetGeometry();
		if (!m_pObj) return;
		//获得边界点
		CArray<PT_3DEX,PT_3DEX> ptnodes;
		m_pObj->GetShape(ptnodes);
		int polysnum = ptnodes.GetSize();
		PT_3D *polys = new PT_3D[polysnum];
		
		if( polysnum<1 || !polys )
		{
			if( polys )delete[] polys;
			Abort();
			return;
		}
		
		for( int i=0; i<polysnum; i++)
		{
			polys[i].x = ptnodes[i].x;
			polys[i].y = ptnodes[i].y;
			polys[i].z = ptnodes[i].z;
		}

		polysnum = GraphAPI::GKickoffSame2DPoints(polys,polysnum);

		if( polysnum<3 )
		{
			if( polys )delete[] polys;
			Abort();
			return;
		}
		CDlgDoc *pDoc = PDOC(m_pEditor);
		CView * pView = pDoc->GetCurActiveView();
		CView * pOSGView = nullptr;

		if (pView->IsKindOf(RUNTIME_CLASS(COsgbView))){
			pOSGView = pView;
		}
		else if (pView->IsKindOf(RUNTIME_CLASS(CVectorView_new))){
			pOSGView = ((CVectorView_new *)pView)->getOsgbView();
		}

		if (pOSGView){
			DoOSG(pOSGView, polys, polysnum);
		}
		else
		{
			Do(polys, polysnum);
		}
		if( polys )delete[] polys;
		
		Finish();
		m_nStep = 3;
		return;

	}
	CCommand::PtClick(pt, flag);
}

extern void KickoffSameGeoPts(CArray<MYPT_3D,MYPT_3D>& arr);


void CInterpolateHeiPointCommand::Do(PT_3D *polyPts, int nPt)
{
	if(m_fGridSize<=0)
		return;

	if(m_strLayerHeiPoint.GetLength()<=0)
		return;
	
	CFtrLayer *pLayer = m_pEditor->GetDataSource()->GetFtrLayer(m_strLayerHeiPoint);
	if(pLayer==NULL)
		return;

	Envelope e = CreateEnvelopeFromPts(polyPts,nPt);

	CGrTrim trim;
	trim.InitTrimPolygon(polyPts,nPt,m_fGridSize);

	CDlgDoc *pDoc = PDOC(m_pEditor);

	//查找对象
	pDoc->GetDataQuery()->FindObjectInRect(e,NULL);
	int nObj;
	const CPFeature *ppFtr = pDoc->GetDataQuery()->GetFoundHandles(nObj);

	if( nObj<=0 )return;

	CPtrArray arrObjs;
	CPtrArray arrAllObjs;
	
	//过滤对象
	for(int i=0; i<nObj; i++)
	{
		CFeature *pFtr = ppFtr[i];
		if (!pFtr) continue;
		CGeometry *pObj = pFtr->GetGeometry();
		if( !pObj )continue;

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			continue;
		}
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
		{
			continue;
		}

		CPtrArray arrObjs;
		CBatExportMapsCommand::TrimObj(pObj, &arrObjs, e, polyPts, nPt, &trim, FALSE, TRUE);
		arrAllObjs.Append(arrObjs);
	}

	int k = 0, npt;
	nObj = arrAllObjs.GetSize();

	CArray<MYPT_3D,MYPT_3D> arrAll, arrCur;
	PT_3DEX expt;
	MYPT_3D geopt;
	MYPT_3D tri[3];

	for( int j=0; j<nObj; j++)
	{
		CGeometry *pGeo = (CGeometry*)arrAllObjs[j];
		if( !pGeo )continue;
		
		k++;
		
		if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
		{
			expt = pGeo->GetDataPoint(0);
			geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; geopt.type = 0;
			geopt.id = k;
			arrAll.Add(geopt);
		}
		else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
		{
			const CShapeLine *pShape = pGeo->GetShape();
			if (!pShape) continue;
			
			CArray<PT_3DEX,PT_3DEX> pts;
			pShape->GetKeyPts(pts);
			
			npt = pts.GetSize();
			arrCur.RemoveAll();
			for( int n=0; n<npt; n++)
			{
				expt = pts[n];
				geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z; 
				geopt.type = 1;
				geopt.id = k;
				arrCur.Add(geopt);
			}
			
			KickoffSameGeoPts(arrCur);
			if( arrCur.GetSize()>0 )arrAll.Append(arrCur);
		}
		else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		{
			const CShapeLine *pShape = pGeo->GetShape();
			if (!pShape) continue;
			
			CArray<PT_3DEX,PT_3DEX> pts;
			pShape->GetKeyPts(pts);
			
			npt = pts.GetSize();
			arrCur.RemoveAll();
			for( int n=0; n<npt; n++)
			{
				expt = pts[n];
				if (expt.pencode == penMove && n!=0)
				{
					k++;
					continue;
				}
				geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;
				geopt.type = 1;
				geopt.id = k;
				arrCur.Add(geopt);
			}
			
			KickoffSameGeoPts(arrCur);
			if( arrCur.GetSize()>0 )arrAll.Append(arrCur);
		}

		delete pGeo;
	}

	if( arrAll.GetSize()>0 )
	{
		CMYTinObj tin;
		tin.Clear();
	
		MyTriangle::createTIN(arrAll.GetData(),arrAll.GetSize(),&tin.tins,&tin.nTIN,&tin.pts,&tin.npt);

		tin.RefreshTriangleBounds();
		
		double x0 = ceil(e.m_xl/m_fGridSize)*m_fGridSize, y0 = ceil(e.m_yl/m_fGridSize)*m_fGridSize;
		double x,y,z = 0;
		MYPT_3D tri[3];

		CUndoFtrs undo(m_pEditor,Name());

		int nRow = (int)ceil(e.m_yl/m_fGridSize);

		srand((unsigned)time(NULL));
		int range = m_fRandRange*100*2;
		
		for(y=y0; y<e.m_yh; y+=m_fGridSize*0.5, nRow++)
		{
			double dx = m_fGridSize*0.5;
			if((nRow%2)==0)
				dx = 0;

			for(x=x0; x<e.m_xh; x+=m_fGridSize)
			{
				//随机位置x
				double randx = range<=0 ? (x+dx) : (x+dx+(rand()%range)/100.0-m_fRandRange);
				//随机位置y
				double randy = range<=0 ? y : (y+(rand()%range)/100.0-m_fRandRange);

				if(GraphAPI::GIsPtInRegion(PT_3D(randx,randy,0),polyPts,nPt)==2)
				{
					int index = tin.FindTriangle(PT_3D(randx,randy,0));
					if(index>=0)
					{
						tin.GetTriangle(index,tri);

						z = 0;
						tin.GetZFromTriangle(tri,randx,randy,z);
						
						CFeature *pNewFtr = pLayer->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(),CLS_GEOPOINT);
						if (!pNewFtr)
						{
							Abort();
							return;
						}
						
						if (!pNewFtr->GetGeometry()->CreateShape(&PT_3DEX(PT_3D(randx,randy,z),penLine),1))
						{			
							Abort();
							delete pNewFtr;
							return;
						}
						
						m_pEditor->AddObject(pNewFtr,pLayer->GetID());
						
						undo.AddNewFeature(FtrToHandle(pNewFtr));
					}
				}
			}
		}
		
		undo.Commit();								
	}
}


void CInterpolateHeiPointCommand::DoOSG(CView * pOsgbView, PT_3D *polyPts, int nPt)
{
	COsgbView * pOSGB = (COsgbView *)pOsgbView;

	if (!pOSGB) return;

	if (m_fGridSize <= 0)
		return;

	if (m_strLayerHeiPoint.GetLength() <= 0)
		return;

	CFtrLayer *pLayer = m_pEditor->GetDataSource()->GetFtrLayer(m_strLayerHeiPoint);
	if (pLayer == NULL)
		return;

	Envelope e = CreateEnvelopeFromPts(polyPts, nPt);

	double x0 = ceil(e.m_xl / m_fGridSize)*m_fGridSize, y0 = ceil(e.m_yl / m_fGridSize)*m_fGridSize;
	double x, y, z = 0;
	MYPT_3D tri[3];

	CUndoFtrs undo(m_pEditor, Name());

	int nRow = (int)ceil(e.m_yl / m_fGridSize);

	srand((unsigned)time(NULL));
	int range = m_fRandRange * 100 * 2;

	for (y = y0; y < e.m_yh; y += m_fGridSize*0.5, nRow++)
	{
		double dx = m_fGridSize*0.5;
		
		if ((nRow % 2) == 0 && m_fRandRange != 0)
			dx = 0;

		for (x = x0; x < e.m_xh; x += m_fGridSize)
		{
			//随机位置x
			double randx = range <= 0 ? (x + dx) : (x + dx + (rand() % range) / 100.0 - m_fRandRange);
			//随机位置y
			double randy = range <= 0 ? y : (y + (rand() % range) / 100.0 - m_fRandRange);

			if (GraphAPI::GIsPtInRegion(PT_3D(randx, randy, 0), polyPts, nPt) == 2)
			{
				
				std::vector<double> zs = pOSGB->getLinePick(randx, randy);
				
				if (zs.size() > 0)
				{			
					z = zs.back();
					if (z == 0) continue;
					CFeature *pNewFtr = pLayer->CreateDefaultFeature(GETDS(m_pEditor)->GetScale(), CLS_GEOPOINT);
					if (!pNewFtr)
					{
						Abort();
						return;
					}

					if (!pNewFtr->GetGeometry()->CreateShape(&PT_3DEX(PT_3D(randx, randy, z), penLine), 1))
					{
						Abort();
						delete pNewFtr;
						return;
					}

					m_pEditor->AddObject(pNewFtr, pLayer->GetID());

					undo.AddNewFeature(FtrToHandle(pNewFtr));
				}
			}
		}
	}

	undo.Commit();

}

void CInterpolateHeiPointCommand::PtMove(PT_3D &pt)
{
	if( m_nStep==0 || m_nStep==1 )
	{
		m_pDrawProc->m_bClosed = TRUE;
		m_pDrawProc->PtMove(pt);
	}
}

void CInterpolateHeiPointCommand::PtReset(PT_3D &pt)
{
	
	if( m_nStep==1 )
	{
		m_pDrawProc->PtReset(pt);

		if (m_pFtr)
		{
			GrBuffer cbuf;
			m_pFtr->GetGeometry()->Draw(&cbuf);
			cbuf.SetAllColor(m_pFtr->GetGeometry()->GetColor());
			m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pEditor->UpdateDrag(ud_AddConstDrag,&cbuf);
		}
		m_nStep = 2;
	}
	else
	{
		CCommand::PtReset(pt);
	}
}



//////////////////////////////////////////////////////////////////////
// CMapMatchConnectCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CMapMatchConnectCommand,CEditCommand)

CMapMatchConnectCommand::CMapMatchConnectCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\MapMatch2");
}

CMapMatchConnectCommand::~CMapMatchConnectCommand()
{

}

CString CMapMatchConnectCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MAPAUTOMATCH);
}

void CMapMatchConnectCommand::Start()
{
	if( !m_pEditor )return;

	CEditCommand::Start();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
}

void CMapMatchConnectCommand::Abort()
{
	m_nStep = -1;
	UpdateParams(TRUE);
	m_pEditor->RefreshView();
	CEditCommand::Abort();
}

void CMapMatchConnectCommand::Finish()
{
	UpdateParams(TRUE);
	m_pEditor->UpdateDrag(ud_SetConstDrag,NULL);
	m_pEditor->RefreshView();
	CEditCommand::Finish();
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

void CMapMatchConnectCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;
	CAttributesSource *pXDS = pDS->GetXAttributesSource();
	if(!pXDS) return;

	CFtrLayer *pTKLayer = pDS->GetFtrLayer(m_strLayNameOfTKName);
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
						if(dis<GraphAPI::g_lfDisTolerance)
						{
							dis = GraphAPI::GGetNearestDisOfPtToLine(segs[k].x, segs[k].y, segs[k+1].x, segs[k+1].y, pt1.x, pt1.y,NULL,NULL,FALSE);
							if(dis<GraphAPI::g_lfDisTolerance)
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
// 						if(dis<GraphAPI::g_lfDisTolerance)
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
					if(dis<GraphAPI::g_lfDisTolerance)
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
					if(dis<GraphAPI::g_lfDisTolerance)
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

	for(i=0; i<items.GetSize(); i++)
	{
		if(items[i].pFtr)
		{
			items[i].pFtr->SetAppFlag(-1);
		}
	}

	CUndoBatchAction undo(m_pEditor, Name());
	CUndoModifyProperties undo1(m_pEditor, Name());
	CUndoFtrs undo2(m_pEditor, Name());

	//线接边,appflag保存接边的item编号
	if(m_nJoinMode==0)
		GProgressStart(2*(items.GetSize()+items1.GetSize()));
	else
		GProgressStart(3*(items.GetSize()+items1.GetSize()));
	for(i=0; i<items.GetSize(); i++)
	{
		GProgressStep();
		CFeature *pFtr = items[i].pFtr;
		if(pFtr==NULL) continue;
		
		PT_3DEX p0 = items[i].pt;
		
		for (j=i+1; j<items.GetSize(); j++)
		{
			if(items[j].pFtr==NULL) continue;
			if(items[j].pLayer!=items[i].pLayer) continue;
			if( items[i].segid!=items[j].segid ) continue;
			if( 0!=items[i].position+items[j].position) continue;
			
			PT_3DEX p1 = items[j].pt;
			
			double dis = sqrt( (p0.x-p1.x)*(p0.x-p1.x)+(p0.y-p1.y)*(p0.y-p1.y) );
			if( dis>GraphAPI::g_lfDisTolerance ) continue;
			//已找到配对的地物
			CFeature *pFtr1 = items[j].pFtr;
			int flag1 = pFtr->GetAppFlag();
			int flag2 = pFtr1->GetAppFlag();
			if(flag1==-1 && flag2==-1)
			{
				pFtr->SetAppFlag(i);
				pFtr1->SetAppFlag(i);
			}
			else if( flag1==-1 && flag2>=0 )
			{
				pFtr->SetAppFlag(flag2);
			}
			else if( flag1>=0 && flag2==-1 )
			{
				pFtr1->SetAppFlag(flag1);
			}
			else if(flag1<flag2)
			{
				for(k=0; k<items.GetSize(); k++)
				{
					if(items[k].pFtr->GetAppFlag()==flag2)
					{
						items[k].pFtr->SetAppFlag(flag1);
					}
				}
			}
			else if(flag1>flag2)
			{
				for(k=0; k<items.GetSize(); k++)
				{
					if(items[k].pFtr->GetAppFlag()==flag1)
					{
						items[k].pFtr->SetAppFlag(flag2);
					}
				}
			}
			break;
		}
	}

	for(i=0; i<items.GetSize(); i++)
	{
		GProgressStep();
		CFeature *pFtr = items[i].pFtr;
		if(pFtr==NULL) continue;
		
		if(i!=pFtr->GetAppFlag()) continue;
		
		CFtrArray ftrs;
		ftrs.Add(pFtr);
		
		for (j=i+1; j<items.GetSize(); j++)
		{
			if(items[j].pFtr==NULL) continue;
			if(i==items[j].pFtr->GetAppFlag())
			{
				for(k=0; k<ftrs.GetSize(); k++)
				{
					if(ftrs[k]==items[j].pFtr)
					{
						break;
					}
				}
				if(k>=ftrs.GetSize())//不能重复添加
				{
					ftrs.Add(items[j].pFtr);
				}
			}
		}
		MakeAttributeSame(ftrs, undo1);
	}

	if(m_nJoinMode==1)
	{
		for(i=0; i<items.GetSize(); i++)
		{
			GProgressStep();
			CFeature *pFtr = items[i].pFtr;
			if(pFtr==NULL) continue;

			if(i!=pFtr->GetAppFlag()) continue;
			
			CFtrArray ftrs,newftrs;
			ftrs.Add(pFtr);
			newftrs.Add(pFtr->Clone());

			for (j=i+1; j<items.GetSize(); j++)
			{
				if(items[j].pFtr==NULL) continue;
				if(i==items[j].pFtr->GetAppFlag())
				{
					for(k=0; k<ftrs.GetSize(); k++)
					{
						if(ftrs[k]==items[j].pFtr)
						{
							break;
						}
					}
					if(k>=ftrs.GetSize())//不能重复添加
					{
						ftrs.Add(items[j].pFtr);
						newftrs.Add(items[j].pFtr->Clone());
					}
				}
			}

			if( MergeCurves(newftrs) )
			{
				CFeature *pNew = newftrs[0];
				m_pEditor->AddObject(pNew, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr)));
				pDS->GetXAttributesSource()->CopyXAttributes(pFtr, pNew);
				undo2.AddNewFeature(FtrToHandle(pNew));
				newftrs[0] = NULL;
				for(k=0; k<ftrs.GetSize(); k++)
				{
					m_pEditor->DeleteObject(FtrToHandle(ftrs[k]));
					undo2.AddOldFeature(FtrToHandle(ftrs[k]));
				}
			}
			for(k=0; k<newftrs.GetSize(); k++)
			{
				if(newftrs[k])
				{
					delete newftrs[k];
					newftrs[k] = NULL;
				}
			}
		}
	}

	for(i=0; i<items1.GetSize(); i++)
	{
		if(items1[i].pFtr)
		{
			items1[i].pFtr->SetAppFlag(-1);
		}
	}

	//面的接边
	for(i=0; i<items1.GetSize(); i++)
	{
		GProgressStep();
		CFeature *pFtr = items1[i].pFtr;
		if(pFtr==NULL) continue;
		
		PT_3DEX p0 = items1[i].pt;
		PT_3DEX p1 = items1[i].pt1;
		
		for (j=i+1; j<items1.GetSize(); j++)
		{
			if(items1[j].pFtr==NULL) continue;
			if(items1[j].pLayer!=items1[i].pLayer) continue;
			if( items1[i].segid!=items1[j].segid ) continue;
			if( 0!=items1[i].position+items1[j].position) continue;
			
			PT_3DEX p2 = items1[j].pt;
			PT_3DEX p3 = items1[j].pt1;
			
			double dis = sqrt( (p0.x-p2.x)*(p0.x-p2.x)+(p0.y-p2.y)*(p0.y-p2.y) );
			double dis1 = sqrt( (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y) );
			if( dis<GraphAPI::g_lfDisTolerance )
			{
				dis1 = sqrt( (p1.x-p3.x)*(p1.x-p3.x)+(p1.y-p3.y)*(p1.y-p3.y) );
				if(dis1>GraphAPI::g_lfDisTolerance)
				{
					continue;
				}
			}
			else if( dis1<GraphAPI::g_lfDisTolerance )
			{
				dis = sqrt( (p0.x-p3.x)*(p0.x-p3.x)+(p0.y-p3.y)*(p0.y-p3.y) );
				if(dis>GraphAPI::g_lfDisTolerance)
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
			int flag1 = pFtr->GetAppFlag();
			int flag2 = pFtr1->GetAppFlag();
			if(flag1==-1 && flag2==-1)
			{
				pFtr->SetAppFlag(i);
				pFtr1->SetAppFlag(i);
			}
			else if( flag1==-1 && flag2>=0 )
			{
				pFtr->SetAppFlag(flag2);
			}
			else if( flag1>=0 && flag2==-1 )
			{
				pFtr1->SetAppFlag(flag1);
			}
			else if(flag1<flag2)
			{
				for(k=0; k<items1.GetSize(); k++)
				{
					if(items1[k].pFtr->GetAppFlag()==flag2)
					{
						items1[k].pFtr->SetAppFlag(flag1);
					}
				}
			}
			else if(flag1>flag2)
			{
				for(k=0; k<items1.GetSize(); k++)
				{
					if(items1[k].pFtr->GetAppFlag()==flag1)
					{
						items1[k].pFtr->SetAppFlag(flag2);
					}
				}
			}
			break;
		}
	}

	for(i=0; i<items1.GetSize(); i++)
	{
		GProgressStep();
		CFeature *pFtr = items1[i].pFtr;
		if(pFtr==NULL) continue;
		
		if(i!=pFtr->GetAppFlag()) continue;
		
		CFtrArray ftrs;
		ftrs.Add(pFtr);
		
		for (j=i+1; j<items1.GetSize(); j++)
		{
			if(items1[j].pFtr==NULL) continue;
			if(i==items1[j].pFtr->GetAppFlag())
			{
				for(k=0; k<ftrs.GetSize(); k++)
				{
					if(ftrs[k]==items1[j].pFtr)
					{
						break;
					}
				}
				if(k>=ftrs.GetSize())//不能重复添加
				{
					ftrs.Add(items1[j].pFtr);
				}
			}
		}
		MakeAttributeSame(ftrs, undo1);
	}

	if(m_nJoinMode==1)
	{
		for(i=0; i<items1.GetSize(); i++)
		{
			GProgressStep();
			CFeature *pFtr = items1[i].pFtr;
			if(pFtr==NULL) continue;
			
			if(i!=pFtr->GetAppFlag()) continue;

			CFtrArray ftrs, newftrs;
			ftrs.Add(pFtr);
			newftrs.Add(pFtr->Clone());
			
			for (j=i+1; j<items1.GetSize(); j++)
			{
				if(items1[j].pFtr==NULL) continue;
				if(i==items1[j].pFtr->GetAppFlag())
				{
					for(k=0; k<ftrs.GetSize(); k++)
					{
						if(ftrs[k]==items1[j].pFtr)
						{
							break;
						}
					}
					if(k>=ftrs.GetSize())//不能重复添加
					{
						ftrs.Add(items1[j].pFtr);
						newftrs.Add(items1[j].pFtr->Clone());
					}
				}
			}
			
			if( MergeSurface(newftrs) )
			{
				CFeature *pNew = newftrs[0];
				m_pEditor->AddObject(pNew, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr)));
				pDS->GetXAttributesSource()->CopyXAttributes(pFtr, pNew);
				undo2.AddNewFeature(FtrToHandle(pNew));
				newftrs[0] = NULL;
				for(k=0; k<ftrs.GetSize(); k++)
				{
					m_pEditor->DeleteObject(FtrToHandle(ftrs[k]));
					undo2.AddOldFeature(FtrToHandle(ftrs[k]));
				}
			}
			for(k=0; k<newftrs.GetSize(); k++)
			{
				if(newftrs[k])
				{
					delete newftrs[k];
					newftrs[k] = NULL;
				}
			}
		}
	}

	if(m_nJoinMode==1)
	{
		AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,1,0);
		CChkResManger *pChkRes = &GetChkResMgr();		
		CString chkName = Name();
		CString chkReason = StrFromResID(IDS_CHECKREASON_NO_JOIN);

		for(i=0; i<items.GetSize(); i++)
		{
			if(items[i].pFtr->GetAppFlag() == -1)
			{
				pChkRes->BeginResItem(chkName);
				pChkRes->AddAssociatedFtr(items[i].pFtr);
				pChkRes->SetAssociatedPos(items[i].pt);
				pChkRes->SetReason(chkReason);
				pChkRes->EndResItem();
			}
		}

		for(i=0; i<items1.GetSize(); i++)
		{
			if(items1[i].pFtr->GetAppFlag() == -1)
			{
				pChkRes->BeginResItem(chkName);
				pChkRes->AddAssociatedFtr(items1[i].pFtr);
				PT_3D cen;
				cen.x = (items1[i].pt.x+items1[i].pt1.x)/2;
				cen.y = (items1[i].pt.y+items1[i].pt1.y)/2;
				cen.z = 0;
				pChkRes->SetAssociatedPos(cen);
				pChkRes->SetReason(chkReason);
				pChkRes->EndResItem();
			}
		}
		AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,0,0);
	}

	pDS->SetAllAppFlags(0);
	undo.AddAction(&undo1);
	undo.AddAction(&undo2);
	undo.Commit();
	GProgressEnd();
	Finish();		    
}

static BOOL CheckFieldValid(CValueTable& tab, int pos, LPCTSTR field)
{
	const CVariantEx *pvar = NULL;
	if(!tab.GetValue(pos,field,pvar))
		return FALSE;
	
	_variant_t var = *pvar;
	CString text = VarToString(var);
	if( text.IsEmpty() || text.CompareNoCase("0")==0 || text.CompareNoCase("0.0000")==0)
		return FALSE;
	
	return TRUE;
}

void CMapMatchConnectCommand::MakeAttributeSame(CFtrArray& ftrs, CUndoModifyProperties& undo)
{
	if(ftrs.GetSize()<2) return;

	CValueTable tab;
	for(int i=0; i<ftrs.GetSize(); i++)
	{
		if(ftrs[i]==NULL) continue;
		
		tab.BeginAddValueItem();
		GETXDS(m_pEditor)->GetXAttributes(ftrs[i], tab);
		tab.EndAddValueItem();
	}

	CScheme *pScheme = gpCfgLibMan->GetScheme(GETDS(m_pEditor)->GetScale());
	if(pScheme)
	{
		CFtrLayer *pLayer = GETDS(m_pEditor)->GetFtrLayerOfObject(ftrs[0]);
		CSchemeLayerDefine *pdef = pScheme->GetLayerDefine(pLayer->GetName());
		if(pdef)
		{
			int size=0;
			const XDefine *defs = pdef->GetXDefines(size);
			for(int n=0; n<size; n++)
			{
				if(defs[n].isMust<0)
				{
					tab.DelField(defs[n].field);
				}
			}
		}
	}

	CValueTable newtab;
	newtab.CopyFrom(tab);

	for(i=0; i<newtab.GetFieldCount(); i++)
	{
		CString field,name;
		int type;
		if( newtab.GetField(i, field, type, name) )
		{
			const CVariantEx *pvar = NULL;
			for(int j=0; j<ftrs.GetSize(); j++)
			{
				if( CheckFieldValid(newtab, j, field) )
					break;
			}
			if(j>=ftrs.GetSize()) j=0;
			if( !newtab.GetValue(j, field, pvar) )
				continue;

			_variant_t var = *pvar;
			for(j=0; j<ftrs.GetSize(); j++)
			{
				newtab.SetValue(j, field, &CVariantEx(var));
			}
		}
	}

	for(i=0; i<ftrs.GetSize(); i++)
	{
		CValueTable tab1, tab2;
		tab1.BeginAddValueItem();
		tab1.AddItemFromTab(tab, i);
		tab1.EndAddValueItem();
		
		tab2.BeginAddValueItem();
		tab2.AddItemFromTab(newtab, i);
		tab2.EndAddValueItem();
		GETXDS(m_pEditor)->SetXAttributes(ftrs[i], tab2);
		undo.SetModifyProperties(FtrToHandle(ftrs[i]), tab1, tab2, FALSE);
	}
}

BOOL CMapMatchConnectCommand::MergeCurves(CFtrArray& ftrs)
{
	if(ftrs.GetSize()<2)
		return FALSE;

	int i,j;
	CArray<PT_3DEX, PT_3DEX> pts1,pts2;
	for(i=0; i<ftrs.GetSize(); i++)
	{
		if(ftrs[i]==NULL) continue;
		CGeometry *pGeo1 = ftrs[i]->GetGeometry();
		if(pGeo1==NULL) continue;
		pGeo1->GetShape(pts1);
		int nPt1 = pts1.GetSize();
		if(nPt1<2) continue;

		for(j=i+1; j<ftrs.GetSize(); j++)
		{
			if(i==j) continue;
			if(ftrs[j]==NULL) continue;
			CGeometry *pGeo2 = ftrs[j]->GetGeometry();
			if(pGeo2==NULL) continue;
			pGeo2->GetShape(pts2);
			int nPt2 = pts2.GetSize();
			if(nPt2<2) continue;

			CArray<PT_3DEX, PT_3DEX> newPts;
			BOOL bConnect=FALSE;
			if(GraphAPI::GIsEqual2DPoint(&pts1[0],&pts2[0]))
			{
				bConnect = TRUE;
				for(int k=nPt1-1; k>0; k--)
				{
					newPts.Add(pts1[k]);
				}
				newPts.Append(pts2);
			}
			else if(GraphAPI::GIsEqual2DPoint(&pts1[0],&pts2[nPt2-1]))
			{
				bConnect = TRUE;
				for(int k=0; k<nPt2-1; k++)
				{
					newPts.Add(pts2[k]);
				}
				newPts.Append(pts1);
			}
			else if(GraphAPI::GIsEqual2DPoint(&pts1[nPt1-1],&pts2[0]))
			{
				bConnect = TRUE;
				for(int k=0; k<nPt1-1; k++)
				{
					newPts.Add(pts1[k]);
				}
				newPts.Append(pts2);
			}
			else if(GraphAPI::GIsEqual2DPoint(&pts1[nPt1-1],&pts2[nPt2-1]))
			{
				bConnect = TRUE;
				newPts.Copy(pts1);
				for(int k=nPt2-1; k>=0; k--)
				{
					newPts.Add(pts2[k]);
				}
			}
			
			if(bConnect)
			{
				delete ftrs[j];
				ftrs[j] = NULL;
				pGeo1->CreateShape(newPts.GetData(), newPts.GetSize());
				pts1.Copy(newPts);
				nPt1 = pts1.GetSize();
				j=i;//重新循环
			}
		}
	}
	
	if(ftrs[0]==NULL)
		return FALSE;

	return TRUE;
}

BOOL CMapMatchConnectCommand::MergeSurface(CFtrArray& ftrs)
{
	if(ftrs.GetSize()<2)
		return FALSE;

	gsbh::GeoSurfaceSetBooleanHandle gssbh;

	for(int i=0; i<ftrs.GetSize(); i++)
	{
		if(ftrs[i]==NULL) continue;
		CGeometry *pGeo = ftrs[i]->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			continue;

		gssbh.addSurface((CGeoSurface*)pGeo, ftrs[i]->GetID().ToString());
	}

	gssbh.init();
	gssbh.join();
	std::vector<gsbh::GeoSurfaceSetBooleanHandle::ResultSurface> res;
	gssbh.getResult(res);

	if(1!=res.size())
		return FALSE;

	CGeoSurface *pNewObj = res[0].pgs;
	ftrs[0]->SetGeometry(pNewObj);
	
	return TRUE;
}

void CMapMatchConnectCommand::GetParams(CValueTable &tab)
{	
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (long)(m_nJoinMode);
	tab.AddValue(PF_JOIN_MODE,&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strLayNameOfTKName;
	tab.AddValue(PF_LAYNAME_BOUNDNAME,&CVariantEx(var));
}


void CMapMatchConnectCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
 	param->SetOwnerID("MapMatchConnectCommand",StrFromLocalResID(IDS_CMDNAME_MAPAUTOMATCH));

	param->AddUsedLayerNameParam(PF_LAYNAME_BOUNDNAME,(LPCTSTR)m_strLayNameOfTKName,StrFromResID(IDS_LAYNAME_BOUNDNAME));

	param->BeginOptionParam(PF_JOIN_MODE,StrFromResID(IDS_CMDPLANE_MM_JOINMODE));
	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_BREAK),0,' ',m_nJoinMode==0);
	param->AddOption(StrFromResID(IDS_CMDPLANE_MM_JOIN),1,' ',m_nJoinMode==1);
	param->EndOptionParam();
}


void CMapMatchConnectCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;

	if( tab.GetValue(0,PF_JOIN_MODE,var) )
	{
		m_nJoinMode = (long)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0,PF_LAYNAME_BOUNDNAME,var))
	{
		m_strLayNameOfTKName  = (LPCSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab,bInit);
}

template<class runtimeCLass>
runtimeCLass * getView(CDocument * pdoc)
{
	const CRuntimeClass * pClass = RUNTIME_CLASS(runtimeCLass);
	POSITION pos = pdoc->GetFirstViewPosition();
	while (pos)
	{
		CView * p = pdoc->GetNextView(pos);
		if (p && (p->GetRuntimeClass() == pClass)) return (runtimeCLass *)p;
	}
	return nullptr;
}

IMPLEMENT_DYNAMIC(CSetWhiteModelCommand, CEditCommand)
CSetWhiteModelCommand::CSetWhiteModelCommand()
{
	m_nStep = -1;
	m_pView = nullptr;
}

CString CSetWhiteModelCommand::Name()
{
	return StrFromResID(ID_WHITE_MODEL);
}

CCommand * CSetWhiteModelCommand::Create()
{
	return new CSetWhiteModelCommand;
}

CString CSetWhiteModelCommand::AccelStr()
{
	return _T("SetWhiteModel");
}

CSetWhiteModelCommand::~CSetWhiteModelCommand()
{

}

void CSetWhiteModelCommand::Start()
{
	if (!m_pEditor)return;
	m_pView = getView<COsgbView>(PDOC(m_pEditor));
	if (!m_pView) { 
		GOutPut(StrFromResID(IDS_WHITE_MODEL_TIPS2));
		Abort();
		return;
	}
	GOutPut(StrFromResID(IDS_WHITE_MODEL_TIPS));

	m_nStep = 0;

	CEditCommand::Start();

	int num;
	m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (num > 0)
	{
		m_nStep = 1;
		return;
	}

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();

	m_pEditor->OpenSelector(SELMODE_MULTI);
}

void CSetWhiteModelCommand::PtReset(PT_3D &pt)
{
	if (!m_pEditor || !m_pView || m_nStep < 2)
	{
		m_nStep = 0;
		Abort();
		return;
	}
}



void CSetWhiteModelCommand::PtClick(PT_3D &pt, int flag)
{
	int num;
	int nsel;
	const FTR_HANDLE *handles =
		m_pEditor->GetSelection()->GetSelectedObjs(num);
	if (m_nStep == 0)
	{
		if (num <= 0) return;
		
		m_pView->whiteModel(handles, num, 0, false);
		m_pEditor->CloseSelector();
		m_nStep = 1;
	
	}
	else if (m_nStep == 1)
	{
		auto zs = m_pView->getLinePick(pt.x, pt.y);
		if (zs.empty()) return;
		double z = zs.back();
		
		m_pView->whiteModel(handles, num, z, true);
		m_pEditor->DeselectAll();
		Finish();
	}
}
