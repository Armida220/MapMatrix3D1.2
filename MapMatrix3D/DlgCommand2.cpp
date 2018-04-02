// DlgCommand2.cpp: implementation of the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "DlgCommand2.h"

#include "EditbaseDoc.h"
//#include "TDocDef.h"
#include "GrBuffer.h"
#include "Geometry.h"
#include "GeoCurve.h"
#include "GeoParallel.h"
#include "GeoSurface.h"
#include "GeoText.h"
#include "Linearizer.h"
#include "SmartViewFunctions.h"
#include "MainFrm.h"
#include "UIFPropEx.h"
#include "UIFPropListEx.h"
#include "CommonCallStation.h"
//#include "UIDataExchange.h"
#include "exmessage.h"
#include "RegDef.h"
#include "GrTrim.h"
#include "display.h"
//#include "EqualIn.h"
#include "resource.h"
#include "DlgSetXYZ.h"
#include "ConditionSelect.h"
#include "container.h"
#include "editbasedoc.h"
#include "DlgCommand.h"
#include "BaseView.h"
#include "CoordWnd.h"
#include "CoordSys.h"
#include "Functions_temp.h"

#include "PolygonWork.h"
#include "PolygonBooleanHander.h"
#include "GeoSurfaceBooleanHandle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GP_FAIL				0
#define GP_SUCCESS			1
#define GP_NONEXT			2



static CFtrLayer *GetLayer(CDlgDoc *pDoc, LPCTSTR fid)
{
	CFtrLayer *pLayer = NULL;
	if( fid!=NULL )
		pLayer = pDoc->GetDlgDataSource()->GetFtrLayer(fid);
	
	if( pLayer==NULL )
	{
		pLayer = new CFtrLayer();
		pLayer->SetName(fid);
		
		if( pDoc )
			pDoc->AddFtrLayer(pLayer);
	}
	
	return pLayer;
}

extern BOOL convertStringToStrArray(LPCTSTR str,CStringArray &arr);

static bool GetOverlapSection_ret_tt(double xS1, double yS1, double xE1, double yE1, double xS2, double yS2, double xE2, double yE2, double toler,
	double *xs0, double *ys0, double *rt1, double *rt2)
{
	//将线段1的两点投影到线段2上，垂距超限差那么就不重叠
	double x0, y0, x1, y1, t0, t1;
	GraphAPI::GGetPerpendicular(xS2, yS2, xE2, yE2, xS1, yS1, &x0, &y0, &t0);
	if (fabs(xS1 - x0) < toler&&fabs(yS1 - y0) < toler)
		;
	else
		return false;
	GraphAPI::GGetPerpendicular(xS2, yS2, xE2, yE2, xE1, yE1, &x1, &y1, &t1);
	if (fabs(xE1 - x1) < toler&&fabs(yE1 - y1) < toler)
		;
	else
		return false;

	//不重叠
	if ((t0 <= 0 && t1 <= 0) || (t0 >= 1 && t1 >= 1))
		return false;

	//两线段长度的和
	double dis1 = sqrt((xS2 - xE2)*(xS2 - xE2) + (yS2 - yE2)*(yS2 - yE2)) + sqrt((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1));

	//计算线段叠加后的总长
	double x2 = xS2, y2 = yS2, x3 = xE2, y3 = yE2;
	if (t0 < 0)
	{
		if (t0 < t1)
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
	if (t1 > 1)
	{
		if (t1 < t0)
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

	double dis2 = sqrt((x2 - x3)*(x2 - x3) + (y2 - y3)*(y2 - y3));

	//计算重叠部分长度
	double dis3 = dis1 - dis2;

	//是否超过限差
	if (dis3 < GraphAPI::g_lfDisTolerance)
		return false;

	if (xs0) *xs0 = xS1;
	if (ys0) *ys0 = yS1;
	if (rt1) *rt1 = t0;
	if (rt2) *rt2 = t1;

	return true;
}

//清除重复点，跳过penMove
int KickoffSame2DPoints_notPenMove(CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	if(arrPts.GetSize()<=0)
		return 0;

	CArray<PT_3DEX,PT_3DEX> pts;
	int npt = arrPts.GetSize();
	for(int i=0; i<npt; i++)
	{
		if(i==0)
		{
			pts.Add(arrPts[0]);
		}
		else
		{
			if( GraphAPI::GIsEqual2DPoint(&pts[pts.GetSize()-1],&arrPts[i]) )
			{
				if(arrPts[i].pencode==penMove)
				{
					if(pts[pts.GetSize()-1].pencode==penMove)
						continue;
					else
					{
						pts.Add(arrPts[i]);
					}
				}
			}
			else
			{
				pts.Add(arrPts[i]);
			}
		}
	}
	arrPts.Copy(pts);
	return arrPts.GetSize();
}

bool GLineIntersectLineSegInLimit(double x0,double y0, double x1, double y1,double x2,double y2,double x3,double y3, double *x, double *y, double *ret_t1, double *ret_t2, double toler);

//判断多边形是否自相交
BOOL IsPolygonIntersectSelf(CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	pts.Copy(arrPts);
	int npt = KickoffSame2DPoints_notPenMove(pts);
	pts.SetSize(npt);

	if(npt<=2)
		return FALSE;

	BOOL bClosed = FALSE;
	if( GraphAPI::GIsEqual2DPoint(&pts[0],&pts[npt-1]) )
	{
		bClosed = TRUE;
	}

	PT_3D pt1,pt2,line[4],ret;

	npt = pts.GetSize();
	PT_3DEX *buf = pts.GetData();
	for( int i=0; i<npt-1; i++)
	{
		if( buf[i+1].pencode==penMove )
			continue;

		for( int j=i+2; j<npt-1; j++)
		{
			if( buf[j+1].pencode==penMove )
				continue;

			if( bClosed && i==0 && j==(npt-2) )
				continue;

			COPY_3DPT(line[0],buf[i]);
			COPY_3DPT(line[1],buf[i+1]);
			
			COPY_3DPT(line[2],buf[j]);
			COPY_3DPT(line[3],buf[j+1]);

			//为了优化，先判断一下
			double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;
			
			if( line[0].x<line[1].x ){ xmin1 = line[0].x; xmax1 = line[1].x; }
			else { xmin1 = line[1].x; xmax1 = line[0].x; }
			if( line[0].y<line[1].y ){ ymin1 = line[0].y; ymax1 = line[1].y; }
			else { ymin1 = line[1].y; ymax1 = line[0].y; }

			if( line[2].x<line[3].x ){ xmin2 = line[2].x; xmax2 = line[3].x; }
			else { xmin2 = line[3].x; xmax2 = line[2].x; }
			if( line[2].y<line[3].y ){ ymin2 = line[2].y; ymax2 = line[3].y; }
			else { ymin2 = line[3].y; ymax2 = line[2].y; }

			if( xmax1<xmin2-1e-4 || xmax2<xmin1-1e-4 || 
				ymax1<ymin2-1e-4 || ymax2<ymin1-1e-4 )
				continue;

			if( (line[0].x-line[1].x)*(line[0].x-line[1].x) + (line[0].y-line[1].y)*(line[0].y-line[1].y)<1e-8 )
				continue;

			if( (line[2].x-line[3].x)*(line[2].x-line[3].x) + (line[2].y-line[3].y)*(line[2].y-line[3].y)<1e-8 )
				continue;

			double xt,yt;
			double t0,t1;
			if(GLineIntersectLineSegInLimit(line[0].x,line[0].y,line[1].x,line[1].y,line[2].x,line[2].y,line[3].x,line[3].y,&xt,&yt,&t0,&t1,GraphAPI::g_lfDisTolerance))
			{	
				return TRUE;
			}
			//重叠
			else if( GetOverlapSection_ret_tt(line[0].x,line[0].y,line[1].x,line[1].y,line[2].x,line[2].y,line[3].x,line[3].y,GraphAPI::g_lfDisTolerance,&xt,&yt,&t0,&t1) )
			{
				return TRUE;				
			}
		}
	}

	return FALSE;
}


//获取面（包括复杂面）对象的一个内部点
//算法思路：基于这样一个判定：面的任意一个有长度的边，必然能和另外一个顶点构成一个内部三角形。该判定是从三角构网的思路衍生而来的。
BOOL FindSurfaceInnerPoint(CGeoSurface *pSurface, PT_3D *ret)
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pSurface->GetShape()->GetPts(arrPts);	

	KickoffSame2DPoints_notPenMove(arrPts);
	if( arrPts.GetSize()<=3)
		return FALSE;

	int npt = arrPts.GetSize();
	PT_3DEX *pts = arrPts.GetData();
	for( int i=2; i<npt; i++)
	{
		PT_3D cpt;
		cpt.x = (pts[0].x + pts[1].x + pts[i].x)/3;
		cpt.y = (pts[0].y + pts[1].y + pts[i].y)/3;

		if(pSurface->bPtIn(&cpt))
		{
			*ret = cpt;
			return TRUE;
		}
	}

	return FALSE;
}


//规范化面对象：如果面存在自相交，就用它的边界重新构面，得到边界不相交的复杂面（仍然可以有重叠顶点）
CGeoSurface *NormalizeSurface(CEditor *pEditor, CGeoSurface *pGeo)
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape()->GetPts(arrPts);

	KickoffSame2DPoints_notPenMove(arrPts);
	if( arrPts.GetSize()<=2)
		return NULL;
	
	//如果不存在自相交，就不处理
	if(!IsPolygonIntersectSelf(arrPts))
	{
		return (CGeoSurface*)pGeo->Clone();
	}

	//用它的边界构面
	//1\切成多个对象
	int npt = arrPts.GetSize();
	PT_3DEX *pts = arrPts.GetData();

	CArray<PT_3DEX,PT_3DEX> pts2;
	CFtrArray ftrs, out_ftrs;
	
	for( int i=0; i<npt; i++)
	{
		if( i!=0 && (pts[i].pencode==penMove || i==(npt-1) ) )
		{
			if(pts[i].pencode!=penMove)
			{
				pts2.Add(pts[i]);
			}

			if(pts2.GetSize()>=4)
			{
				CFeature *pFtr = new CFeature();
				CGeoSurface *pSurface = new CGeoSurface();
				pSurface->CreateShape(pts2.GetData(),pts2.GetSize());
				pFtr->SetGeometry(pSurface);
				pFtr->SetAppFlag(0);
				ftrs.Add(pFtr);
			}
			
			pts2.RemoveAll();
			pts2.Add(pts[i]);
		}
		else
		{
			pts2.Add(pts[i]);
		}		
	}

	if(ftrs.GetSize()<=0)
		return NULL;

	//2\重新构面
	CTopoSurfaceNoBreakCommand cmd;
	cmd.Init(pEditor);
	cmd.m_bCreateMultiSurface = FALSE;
	cmd.BuildSurface(ftrs,out_ftrs,FALSE);

	//对结果进行排查：当结果面中某个内部点落在原对象内部时，该面保留，否则丢弃（因为它是空洞）
	CArray<PT_3DEX,PT_3DEX> arrNewPts, arr;
	int objCount = 0;
	
	for(i=0; i<out_ftrs.GetSize(); i++)
	{
		CGeoSurface *pSurface = (CGeoSurface*)(out_ftrs[i]->GetGeometry());
		//1\获取复杂面的一个内部点
		//PT_3D ret;
		//FindSurfaceInnerPoint(pSurface,&ret);	
	
		//2\判断它是否落在原始对象内部
		//if( pGeo->bPtIn(&ret) )
		{			
			arr.RemoveAll();
			pSurface->GetShape(arr);
			if(arrNewPts.GetSize()>0)
				arr[0].pencode = penMove;
			arrNewPts.Append(arr);
			objCount++;
		}			
		delete out_ftrs[i];
	}

	CGeoSurface *pNewObj = NULL;

	if(objCount>1)
		pNewObj = new CGeoMultiSurface();
	else
		pNewObj = new CGeoSurface();

	pNewObj->CopyFrom(pGeo);
	pNewObj->CreateShape(arrNewPts.GetData(),arrNewPts.GetSize());

	return pNewObj;
}



//////////////////////////////////////////////////////////////////////
// CCreateCoordNoteLineCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCreateCoordNoteLineCommand,CDrawCommand)


CArray<PT_3DEX,PT_3DEX> CCreateCoordNoteLineCommand::m_arrBound1;
CArray<PT_3DEX,PT_3DEX> CCreateCoordNoteLineCommand::m_arrBound2;

CCreateCoordNoteLineCommand::CCreateCoordNoteLineCommand()
{
	m_bSelectBound = FALSE;
	m_fInterval = 10;
	m_nDirection = 0;
	m_nPosition = 0;

	strcat(m_strRegPath,"\\CreateCoordNoteLine");
}


CCreateCoordNoteLineCommand::~CCreateCoordNoteLineCommand()
{
	
}

DrawingInfo CCreateCoordNoteLineCommand::GetCurDrawingInfo()
{
	return DrawingInfo();
}

int  CCreateCoordNoteLineCommand::GetCurPenCode()
{
	return CDrawCommand::GetCurPenCode();
}

CString CCreateCoordNoteLineCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_CreateCoordNoteLine);
}


void CCreateCoordNoteLineCommand::Start()
{
	CDrawCommand::Start();
	GotoState(PROCSTATE_READY);

	m_pEditor->CloseSelector();
}


void CCreateCoordNoteLineCommand::PtReset(PT_3D &pt)
{	
	Abort();
}

void CCreateCoordNoteLineCommand::Finish()
{
	AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
	CCommand::Finish();
}


void CCreateCoordNoteLineCommand::Abort()
{
	UpdateParams(TRUE);
	m_pEditor->UpdateDrag(ud_ClearDrag);

	CCommand::Abort();

	m_nExitCode = CMPEC_RESTARTWITHPARAMS;
}


void CCreateCoordNoteLineCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CreateCoordNoteLine",StrFromResID(IDS_CMDNAME_CreateCoordNoteLine));

	param->AddButtonParam("Select",StrFromResID(IDS_SELECT_BOUND),NULL);

	param->AddParam("Interval",m_fInterval,StrFromResID(IDS_GRID_WIDTH));

	param->BeginOptionParam("Direction",StrFromResID(IDS_DIRECTION));
	param->AddOption(StrFromResID(IDS_HORIZONTAL),0,' ',m_nDirection==0);
	param->AddOption(StrFromResID(IDS_VERTICAL),1,' ',m_nDirection==1);
	param->EndOptionParam();

	param->BeginOptionParam("Position",StrFromResID(IDS_POS));
	param->AddOption(StrFromResID(IDS_LEFTORTOP),0,' ',m_nPosition==0);
	param->AddOption(StrFromResID(IDS_RIGHTORBOTTOM),1,' ',m_nPosition==1);
	param->EndOptionParam();
}

void CCreateCoordNoteLineCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;

	if( tab.GetValue(0,"Interval",var) )
	{
		m_fInterval = (float)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"Direction",var) )
	{
		m_nDirection = (long)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"Position",var) )
	{
		m_nPosition = (long)(_variant_t)*var;	
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"Select",var) )
	{
		m_pEditor->OpenSelector(SELMODE_MULTI);
		m_bSelectBound = TRUE;

		GotoState(PROCSTATE_PROCESSING);
	}

	CDrawCommand::SetParams(tab,bInit);
}

void CCreateCoordNoteLineCommand::GetParams(CValueTable& tab)
{
	CDrawCommand::GetParams(tab);
	
	_variant_t var;	
	var = (float)m_fInterval;
	tab.AddValue("Interval",&CVariantEx(var));
	var = (long)m_nDirection;
	tab.AddValue("Direction",&CVariantEx(var));
	var = (double)m_nPosition;
	tab.AddValue("Position",&CVariantEx(var));
}

void CCreateCoordNoteLineCommand::PtDblClick(PT_3D &pt, int flag)
{
	PtClick(pt,flag);
}

void CCreateCoordNoteLineCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_bSelectBound)
	{
		if( m_pEditor->IsSelectorOpen() )
		{
			if( !CEditCommand::CanGetSelObjs(flag,TRUE) )
				return;

			int num;
			const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
			if( num==2 )
			{
				CFeature *pFtr1 = HandleToFtr(handles[0]);
				CGeometry *pGeo1 = pFtr1->GetGeometry();

				CFeature *pFtr2 = HandleToFtr(handles[1]);
				CGeometry *pGeo2 = pFtr2->GetGeometry();

				if( pGeo1->GetDataPointSum()>=4 && pGeo2->GetDataPointSum()>=4 )
				{
					pGeo1->GetShape(m_arrBound1);
					pGeo2->GetShape(m_arrBound2);

					//是否相互包含
					int nRet = IsContainEachother(m_arrBound1,m_arrBound2);
					if( nRet==0 )
					{
						GOutPut(StrFromResID(IDS_INVALID_MAPBORDER));
						Abort();
						return;
					}

					if( nRet==2 )
					{	
						CArray<PT_3DEX,PT_3DEX> arrPts;
						arrPts.Copy(m_arrBound1);
						m_arrBound1.Copy(m_arrBound2);
						m_arrBound2.Copy(arrPts);
					}

					m_bSelectBound = FALSE;
					
					m_pEditor->CloseSelector();

				}
			}

			m_pEditor->GetSelection()->DeselectAll();
			m_pEditor->OnSelectChanged();
		}
	}
	else
	{
		PT_3D pts[2];
		if( CreateCoordNoteLine(pt,pts) )
		{
			CFtrLayer *pLayer = GetLayer(PDOC(m_pEditor),"TK_grid");
			CFeature *pFtr = pLayer->CreateDefaultFeature(PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);
			CGeometry *pGeo = pFtr->GetGeometry();
			
			PT_3DEX expts[2];
			expts[0] = PT_3DEX(pts[0],penLine);
			expts[1] = PT_3DEX(pts[1],penLine);

			pGeo->CreateShape(expts,2);

			m_pEditor->AddObject(pFtr,pLayer->GetID());

			CUndoFtrs undo(m_pEditor,Name());
			undo.arrNewHandles.Add(FtrToHandle(pFtr));

			undo.Commit();

			m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pEditor->RefreshView();
		}
	}

	CDrawCommand::PtClick(pt,flag);

}


int CCreateCoordNoteLineCommand::IsContainEachother(CArray<PT_3DEX,PT_3DEX>& arr1, CArray<PT_3DEX,PT_3DEX>& arr2)
{
	int npt1 = arr1.GetSize();
	PT_3DEX *pts1 = arr1.GetData();

	int npt2 = arr2.GetSize();
	PT_3DEX *pts2 = arr2.GetData();

	Envelope e1, e2;
	e1 = CreateEnvelopeFromPts(pts1,npt1);
	e2 = CreateEnvelopeFromPts(pts2,npt2);

	if( e1.bEnvelopeIn(&e2) )
	{
		for( int i=0; i<npt2; i++)
		{
			if( GraphAPI::GIsPtInRegion(pts2[i],pts1,npt1)!=2 )
				break;
		}

		if( i>=npt2 )
			return 1;
	}
	else if( e2.bEnvelopeIn(&e1) )
	{
		for( int i=0; i<npt1; i++)
		{
			if( GraphAPI::GIsPtInRegion(pts1[i],pts2,npt2)!=2 )
				break;
		}
		
		if( i>=npt1 )
			return 2;
	}


	return 0;
}


void CCreateCoordNoteLineCommand::PtMove(PT_3D& pt)
{
	if( !m_bSelectBound )
	{
		PT_3D pts[2];
		if( CreateCoordNoteLine(pt,pts) )
		{
			GrBuffer buf;
			buf.BeginLineString(0,0);
			buf.MoveTo(pts);
			buf.LineTo(pts+1);
			buf.End();

			m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
			m_pEditor->RefreshView();
		}
	}
}


static long NearInt(double a)
{
	if( a<0 )
		return floor(a-0.5);
	else
		return floor(a+0.5);
}

BOOL CCreateCoordNoteLineCommand::CreateCoordNoteLine(PT_3D curPt, PT_3D pts[2])
{
	int npt1 = m_arrBound1.GetSize();
	PT_3DEX *pts1 = m_arrBound1.GetData();
	
	int npt2 = m_arrBound2.GetSize();
	PT_3DEX *pts2 = m_arrBound2.GetData();

	Envelope e1, e2;
	e1 = CreateEnvelopeFromPts(pts1,npt1);
	e2 = CreateEnvelopeFromPts(pts2,npt2);

	if( e1.Width()<e2.Width() )
		e1 = e2;

	float interval = m_fInterval*PDOC(m_pEditor)->GetDlgDataSource()->GetScale()*0.01;
	if( interval<1e-6 )return FALSE;

	curPt.x = NearInt(curPt.x/interval)*interval;
	curPt.y = NearInt(curPt.y/interval)*interval;

	PT_3D line[2];

	line[0] = curPt;
	line[1] = curPt;

	if( m_nDirection==0 )
	{
		if( m_nPosition==0 )
		{
			line[0].x = curPt.x - e1.Width()*1.5;
			line[1].x = curPt.x + e1.Width()*1.5;			
		}
		else
		{
			line[1].x = curPt.x - e1.Width()*1.5;
			line[0].x = curPt.x + e1.Width()*1.5;			
		}
	}
	else
	{
		if( m_nPosition==0 )
		{
			line[1].y = curPt.y - e1.Height()*1.5;
			line[0].y = curPt.y + e1.Height()*1.5;			
		}
		else
		{
			line[0].y = curPt.y - e1.Height()*1.5;
			line[1].y = curPt.y + e1.Height()*1.5;			
		}
	}

	//计算交点，并按照 t 值排序，取前一段线段
	IntersectPoint item;
	CArray<IntersectPoint,IntersectPoint> arrItems;
	for( int i=0; i<npt1-1; i++)
	{
		if( GraphAPI::GGetLineIntersectLineSeg(line[0].x,line[0].y,line[1].x,line[1].y,
			pts1[i].x,pts1[i].y,pts1[i+1].x,pts1[i+1].y,&item.pt.x,&item.pt.y,&item.t) )
		{
			for( int j=0; j<arrItems.GetSize(); j++)
			{
				if( item.t<arrItems[j].t )
					break;
			}
			if( j>=arrItems.GetSize() )
				arrItems.Add(item);
			else
				arrItems.InsertAt(j,item);
		}
	}

	int nItem = arrItems.GetSize();

	if( (nItem%2)!=0 )
		return FALSE;

	for( i=0; i<npt2-1; i++)
	{
		if( GraphAPI::GGetLineIntersectLineSeg(line[0].x,line[0].y,line[1].x,line[1].y,
			pts2[i].x,pts2[i].y,pts2[i+1].x,pts2[i+1].y,&item.pt.x,&item.pt.y,&item.t) )
		{
			for( int j=0; j<arrItems.GetSize(); j++)
			{
				if( item.t<arrItems[j].t )
					break;
			}
			if( j>=arrItems.GetSize() )
				arrItems.Add(item);
			else
				arrItems.InsertAt(j,item);
		}
	}

	if( (arrItems.GetSize()%2)!=0 )
		return FALSE;

	if( arrItems.GetSize()>nItem && nItem>0 )
	{
		pts[0] = arrItems[0].pt;
		pts[1] = arrItems[1].pt;
		return TRUE;
	}

	return FALSE;
}



//////////////////////////////////////////////////////////////////////
// CCreateCoordNoteCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCreateCoordNoteCommand,CCommand)

CCreateCoordNoteCommand::CCreateCoordNoteCommand()
{
	m_nAlign = 0;
	m_fDX = 2;
	m_fDY = 0;
	m_fFontSize = 3;
	m_strFontName = TEXT_SETTINGS0().strFontName;

	m_fWidthScale = 1.0f;
	m_fCoordUnit = 1000;
	m_nDecimal = 1;
	m_bUseNEMark = FALSE;
}

CCreateCoordNoteCommand::~CCreateCoordNoteCommand()
{
}

CString CCreateCoordNoteCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CreateCoordNote);
}

void CCreateCoordNoteCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;
	var = (long)m_nAlign;
	tab.AddValue("Align",&CVariantEx(var));
	var = m_fDX;
	tab.AddValue("DX",&CVariantEx(var));
	var = m_fDY;
	tab.AddValue("DY",&CVariantEx(var));

	var = (_bstr_t)(LPCTSTR)m_strFontName;
	tab.AddValue("FontName",&CVariantEx(var));

	var = m_fFontSize;
	tab.AddValue("FontSize",&CVariantEx(var));

	var = m_fWidthScale;
	tab.AddValue("WidthScale",&CVariantEx(var));

	var = (long)m_bUseNEMark;
	tab.AddValue("UseNEMark",&CVariantEx(var));

	var = m_fCoordUnit;
	tab.AddValue("CoordUnit",&CVariantEx(var));

	var = (long)m_nDecimal;
	tab.AddValue("Decimal",&CVariantEx(var));
}

void CCreateCoordNoteCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Align",var) )
	{
		m_nAlign = (long)(_variant_t)*var;
	}
	if( tab.GetValue(0,"UseNEMark",var) )
	{
		m_bUseNEMark = (bool)(_variant_t)*var;
	}
	if( tab.GetValue(0,"DX",var) )
	{
		m_fDX = (_variant_t)*var;
	}
	if( tab.GetValue(0,"DY",var) )
	{
		m_fDY = (_variant_t)*var;
	}
	if( tab.GetValue(0,"FontName",var) )
	{
		m_strFontName = (LPCTSTR)(_bstr_t)(_variant_t)*var;
	}
	if( tab.GetValue(0,"FontSize",var) )
	{
		m_fFontSize = (_variant_t)*var;
	}
	if( tab.GetValue(0,"WidthScale",var) )
	{
		m_fWidthScale = (_variant_t)*var;
	}
	if( tab.GetValue(0,"CoordUnit",var) )
	{
		m_fCoordUnit = (_variant_t)*var;
	}
	if( tab.GetValue(0,"Demical",var) )
	{
		m_nDecimal = (long)(_variant_t)*var;
		if (m_nDecimal < 1)
		{
			m_nDecimal = 1;
			AfxMessageBox(IDS_PARAM_ERROR);
			return;
		}
	}
	SetSettingsModifyFlag();

	CEditCommand::SetParams(tab, bInit);
}


void CCreateCoordNoteCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("CreateCoordNote",StrFromResID(IDS_CMDNAME_CreateCoordNote));
	param->BeginOptionParam("Align",StrFromResID(IDS_CMDPLANE_ALIGNTYPE));
	param->AddOption(StrFromResID(IDS_LEFTALIGN),0,' ',m_nAlign==0);
	param->AddOption(StrFromResID(IDS_CENTERALIGN),1,' ',m_nAlign==1);
	param->AddOption(StrFromResID(IDS_RIGHTALIGN),2,' ',m_nAlign==2);
	param->EndOptionParam();

	param->AddParam("DX",m_fDX,StrFromResID(IDS_GRID_DX));
	param->AddParam("DY",m_fDY,StrFromResID(IDS_GRID_DY));

	param->AddFontNameParam("FontName",m_strFontName,StrFromResID(IDS_CMDPLANE_FONT));
	param->AddParam("FontSize",m_fFontSize,StrFromResID(IDS_CMDPLANE_CHARH));
	param->AddParam("WidthScale",m_fWidthScale,StrFromResID(IDS_CMDPLANE_CHARWS));
	param->AddParam("CoordUnit",m_fCoordUnit,StrFromResID(IDS_COORDUNIT));
	param->AddParam("Demical", (long)m_nDecimal, StrFromResID(IDS_CMDPLANE_DIGITNUM));
	param->AddParam("UseNEMark",(bool)m_bUseNEMark,StrFromResID(IDS_ADD_NEMARK));
}

void CCreateCoordNoteCommand::Start()
{
	if( !m_pEditor )return;
	
	CEditCommand::Start();
}

void CCreateCoordNoteCommand::Finish()
{
	CEditCommand::Finish();
}

void CCreateCoordNoteCommand::UpdateParams(BOOL bSave)
{
	CEditCommand::UpdateParams(bSave);
}

void CCreateCoordNoteCommand::Abort()
{
	if( m_pEditor )
	{
		m_pEditor->CloseSelector();
	}
	
	CCommand::Abort();
}

void CCreateCoordNoteCommand::PtClick(PT_3D &pt, int flag)
{
	if( m_nStep==0 )
	{
		if( !CanGetSelObjs(flag) )return;
	   EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		CGeometry *pGeo;
		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		CUndoFtrs undo(m_pEditor,Name());

		if( num!=1 )
		{
			Abort();
			return;
		}

		CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
		if (!pDS)  return;			

		CFeature *pFtr = HandleToFtr(handles[0]);
		pGeo = pFtr->GetGeometry();
		
		if( pGeo && pGeo->GetDataPointSum()==2 && pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
		{	
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);

			//交换位置，使 arrPts[0] 位于左上方
			if( arrPts[0].x-arrPts[0].y>arrPts[1].x-arrPts[1].y )
			{
				PT_3DEX expt = arrPts[0];
				arrPts[0] = arrPts[1];
				arrPts[1] = expt;
			}

			BOOL bHorizontal = TRUE;
			CString text;

			if( m_nDecimal<=0 )
			{
				if( fabs(arrPts[0].x-arrPts[1].x)<1e-4 )
				{
					if( m_bUseNEMark )
						text.Format("E%d", (int)floor(arrPts[0].x/m_fCoordUnit+0.5));
					else
						text.Format("%d", (int)floor(arrPts[0].x/m_fCoordUnit+0.5));
					bHorizontal = FALSE;
				}
				else
				{
					if( m_bUseNEMark )
						text.Format("N%d", (int)floor(arrPts[0].y/m_fCoordUnit+0.5));
					else
						text.Format("%d", (int)floor(arrPts[0].y/m_fCoordUnit+0.5));
				}
			}
			else
			{
				CString filter;
				filter.Format("%%.%df",m_nDecimal);

				if( fabs(arrPts[0].x-arrPts[1].x)<1e-4 )
				{
					if( m_bUseNEMark )
						text.Format("E"+filter, (double)(arrPts[0].x/m_fCoordUnit));
					else
						text.Format(filter, (double)(arrPts[0].x/m_fCoordUnit));
					bHorizontal = FALSE;
				}
				else
				{
					if( m_bUseNEMark )
						text.Format("N"+filter, (double)(arrPts[0].y/m_fCoordUnit));
					else
						text.Format(filter, (double)(arrPts[0].y/m_fCoordUnit));
				}
			}


			CFtrLayer *pFtrLayer = GetLayer(PDOC(m_pEditor),"TK_grid");
			CFeature *pFtr = pFtrLayer->CreateDefaultFeature(PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOTEXT);
			CGeometry *pGeo = pFtr->GetGeometry();

			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
			{
				CGeoText *pText = (CGeoText*)pGeo;
				TEXT_SETTINGS0 sets;
				pText->GetSettings(&sets);

				float unit = PDOC(m_pEditor)->GetDlgDataSource()->GetScale()*0.001;

				sets.fHeight = m_fFontSize;
				sets.fWidScale = m_fWidthScale;

				double ang = GraphAPI::GGetAngle(arrPts[0].x,arrPts[0].y,arrPts[1].x,arrPts[1].y);			
				
				sets.fTextAngle = ang*180/PI;
				strncpy(sets.strFontName,m_strFontName,sizeof(sets.strFontName)-1);

				PT_3DEX expt;
				float k = 1;

				if( m_nAlign==0 )
				{
					sets.nAlignment = TAH_LEFT|TAV_BOTTOM;
					pText->SetSettings(&sets);

					expt = arrPts[0];
					k = 1.0f;
				}
				else if( m_nAlign==1 )
				{
					sets.nAlignment = TAH_MID|TAV_BOTTOM;
					pText->SetSettings(&sets);

					expt = PT_3DEX((arrPts[0].x+arrPts[1].x)*0.5,(arrPts[0].y+arrPts[1].y)*0.5,(arrPts[0].z+arrPts[1].z)*0.5, penLine);
					
					k = 0.0f;
				}
				else if( m_nAlign==2 )
				{
					sets.nAlignment = TAH_RIGHT|TAV_BOTTOM;
					pText->SetSettings(&sets);

					expt = arrPts[1];
					k = -1.0f;
				}
				
				if( bHorizontal )
				{
					expt.x += k*m_fDX*unit;
					expt.y += m_fDY*unit;
				}
				else
				{
					expt.y -= k*m_fDX*unit;
					expt.x += m_fDY*unit;
				}

				pText->CreateShape(&expt,1);

				pText->SetText(text);

				m_pEditor->AddObject(pFtr,pFtrLayer->GetID());

				undo.arrNewHandles.Add(FtrToHandle(pFtr));
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
// CSetTextAngleFromLineCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSetTextAngleFromLineCommand,CCommand)

CSetTextAngleFromLineCommand::CSetTextAngleFromLineCommand()
{
	m_hTextObj = NULL;
}

CSetTextAngleFromLineCommand::~CSetTextAngleFromLineCommand()
{
}

CString CSetTextAngleFromLineCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SetTextAngleFromLine);
}


void CSetTextAngleFromLineCommand::Start()
{
	if( !m_pEditor )return;
	
	CEditCommand::Start();

	m_pEditor->OpenSelector(SELMODE_SINGLE);
}


void CSetTextAngleFromLineCommand::Abort()
{
	if( m_pEditor )
	{
		m_pEditor->CloseSelector();
	}
	
	CCommand::Abort();
}

void CSetTextAngleFromLineCommand::PtClick(PT_3D &pt, int flag)
{
	if( flag!=SELMODE_SINGLE )
		return;

	if( m_nStep==0 )
	{
		CGeometry *pGeo;
		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);

		if( num==1 )
		{
			CFeature *pFtr = HandleToFtr(handles[0]);
			pGeo = pFtr->GetGeometry();

			if( pGeo && pGeo->IsKindOf(RUNTIME_CLASS(CGeoText))  )
			{
				m_hTextObj = handles[0];
				m_nStep = 1;
			}
		}
	}
	else if( m_nStep==1 )
	{
		CGeometry *pGeo;
		CSelection * pSel = m_pEditor->GetSelection();
		int num;
		const FTR_HANDLE * handles = pSel->GetSelectedObjs(num);
		
		if( num==1 )
		{
			CFeature *pFtr = HandleToFtr(handles[0]);
			pGeo = pFtr->GetGeometry();
			
			if( pGeo && (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))  )
			{
				Envelope e;
				e.CreateMaxEnvelope();

				PT_3DEX ret1, ret2;
				if( pGeo->FindNearestBaseLine(pt, e, NULL, &ret1, &ret2,NULL) )
				{
					CFeature *pFtr0 = HandleToFtr(m_hTextObj);

					CGeometry *pGeo0 = pFtr0->GetGeometry();	
					
					CGeoText *pText = (CGeoText*)pGeo0;

					PT_3D pt0 = pText->GetDataPoint(0);

					PT_3D tpt0, tpt1;

					GraphAPI::GGetNearestDisOfPtToLine(ret1.x,ret1.y,ret2.x,ret2.y,pt0.x,pt0.y,&tpt0.x,&tpt0.y);
					GraphAPI::GGetNearestDisOfPtToLine(ret1.x,ret1.y,ret2.x,ret2.y,pt.x,pt.y,&tpt1.x,&tpt1.y);

					double ang = GraphAPI::GGetAngle(tpt0.x,tpt0.y,tpt1.x,tpt1.y);

					ang = ang*180/PI;

					m_pEditor->DeleteObject(m_hTextObj,FALSE);

					TEXT_SETTINGS0 sets;
					pText->GetSettings(&sets);

					CUndoModifyProperties undo(m_pEditor,Name());
					
					undo.SetModifyProp(pFtr0,FIELDNAME_GEOTEXT_TEXTANGLE,&CVariantEx((_variant_t)sets.fTextAngle),
						&CVariantEx((_variant_t)ang));

					sets.fTextAngle = ang;
					pText->SetSettings(&sets);

					m_pEditor->RestoreObject(m_hTextObj);

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
	CEditCommand::PtClick(pt,flag);
}



//////////////////////////////////////////////////////////////////////
// CExtendOrTrimCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CExtendOrTrimCommand,CEditCommand)

CExtendOrTrimCommand::CExtendOrTrimCommand()
{
	m_nStep = -1;
	m_lfToler = 0;
	m_bAddPt = FALSE;
	m_bSnap3D = FALSE;
	strcat(m_strRegPath,"\\ExtendOrTrim");
}

CExtendOrTrimCommand::~CExtendOrTrimCommand()
{

}



CString CExtendOrTrimCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_EXTENDORTRIM);
}

void CExtendOrTrimCommand::Start()
{
 	if( !m_pEditor )return;
	
	m_bSnap3D = FALSE;

	CEditCommand::Start();

	m_pEditor->OpenSelector(SELMODE_NORMAL);
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_TIP_SELREFFTRS));
}
void CExtendOrTrimCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CExtendOrTrimCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CExtendOrTrimCommand::GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, FTR_HANDLE id, CArray<double,double> &ret1, CArray<double,double> &ret2, CArray<double,double> &ret_z)
{
	PT_3D pt3d0, pt3d1, tpt;
	
	COPY_3DPT(pt3d0,pt0);
	COPY_3DPT(pt3d1,pt1);
	
	CDlgDataSource *pDS = GETDS(m_pEditor);

	{
		CFeature *pFtr = HandleToFtr(id);
		if (!pFtr) return;

		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) return;
		
		CFtrArray arrObjs;

		arrObjs.Add(pFtr);
		
		for (int j=0; j<arrObjs.GetSize(); j++)
		{
			CFeature *pFtr0 = arrObjs[j];
			if (!pFtr0) continue;

			CGeometry *po = pFtr0->GetGeometry();
			if( !po )continue;
			
			const CShapeLine  *pb = po->GetShape();
			
			if (pb == NULL) continue;
			
			int npt2 = pb->GetPtsCount();
			if( npt2<=1 )continue;
			CArray<PT_3DEX,PT_3DEX> pts;
			pb->GetPts(pts);	
			
			for( int k=0; k<npt2-1; k++)
			{
				//求交
				double vector1x = pt3d1.x-pt3d0.x, vector1y = pt3d1.y-pt3d0.y;
				double vector2x = pts[k+1].x-pts[k].x, vector2y = pts[k+1].y-pts[k].y;
				
				double delta = vector1x*vector2y-vector1y*vector2x;
				if( delta<1e-10 && delta>-1e-10 )continue;
				
				double t1 = ( (pts[k].x-pt3d0.x)*vector2y-(pts[k].y-pt3d0.y)*vector2x )/delta;
				if( t1<0.0 )continue;
				
				double t2 = ( (pts[k].x-pt3d0.x)*vector1y-(pts[k].y-pt3d0.y)*vector1x )/delta;
				if( t2<0.0 || t2>1.0 )continue;
				
				double z = pts[k].z + t2 * (pts[k+1].z-pts[k].z);

				tpt.x = pt3d0.x + t1*vector1x;  tpt.y = pt3d0.y + t1*vector1y;
				
				//由小到大排序
				for( int m=0; m<ret1.GetSize(); m++)
				{
					if( t1<ret1[m] )break;
				}
				
				if( m<ret1.GetSize() )
				{
					ret1.InsertAt(m,t1);
					ret_z.InsertAt(m,z);
					ret2.InsertAt(m,k+t2);
				}
				else
				{
					ret1.Add(t1);
					ret_z.Add(z);
					ret2.Add(k+t2);
				}
			}

			if (pFtr0 != pFtr)
			{
				delete pFtr0;
			}
		}
		
	}
}

void CExtendOrTrimCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	var = (double)(m_lfToler);
	tab.AddValue("Toler",&CVariantEx(var));

	var = (bool)(m_bSnap3D);
	tab.AddValue("Snap3D",&CVariantEx(var));
	
	var = (bool)(m_bAddPt);
	tab.AddValue("AddPt",&CVariantEx(var));
}

void CExtendOrTrimCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ExtendOrTrimCommand",Name());

	param->AddParam("Toler",(double)m_lfToler,StrFromResID(IDS_CMDPLANE_TOLER));

	param->AddParam("Snap3D",(bool)m_bSnap3D,StrFromResID(IDS_CMDPLANE_3DSNAP));

	param->AddParam("AddPt",(bool)m_bAddPt,StrFromResID(IDS_CMDPLANE_ADDPT));
}

void CExtendOrTrimCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Toler",var) )
	{					
		m_lfToler = (double)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"AddPt",var) )
	{
		m_bAddPt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"Snap3D",var) )
	{					
		m_bSnap3D = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	CEditCommand::SetParams(tab,bInit);
}


void CExtendOrTrimCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
	if( m_nStep==0 )
	{
		if( (flag&SELSTAT_MULTISEL)!=0 )
		{			
			int num = 0;
			const FTR_HANDLE *ftrs = m_pEditor->GetSelection()->GetSelectedObjs(num);	
			if( num>0 )GOutPut(StrFromResID(IDS_CMDTIP_CLICKOK2));
			return;
		}
		else if( (flag&SELSTAT_POINTSEL)!=0 );
		else if( !CanGetSelObjs(flag) )
		{
			int num = 0;
			const FTR_HANDLE *ftrs = m_pEditor->GetSelection()->GetSelectedObjs(num);	
			if( num>0 )GOutPut(StrFromResID(IDS_CMDTIP_CLICKOK2));
			return;
		}

		int num = 0;
		const FTR_HANDLE *ftrs = m_pEditor->GetSelection()->GetSelectedObjs(num);		
		if( num<=0 )
			return;

		for( int i=0; i<num; i++)
		{
			CFeature *pFtr = HandleToFtr(ftrs[i]);
			m_arrRefFtrs.Add(pFtr);
		}

		m_nStep = 1;

		GOutPut(StrFromResID(IDS_TIP_SELEXTENDORTRIMFTR));

		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();

		m_pEditor->CloseSelector();
		m_pEditor->OpenSelector();

		GotoState(PROCSTATE_PROCESSING);
	}
	else if (m_nStep == 1)
	{
		if( (flag&SELSTAT_POINTSEL)!=0 )
		{
			FTR_HANDLE handle = m_pEditor->GetSelection()->GetLastSelectedObj();
			if( handle==0 )
				return;

			CFeature *pFtr1 = HandleToFtr(handle);

			for( int i=0; i<m_arrRefFtrs.GetSize(); i++)
			{
				if( pFtr1==m_arrRefFtrs[i] )
					break;
			}

			if( i<m_arrRefFtrs.GetSize() )
			{
				GOutPut(StrFromResID(IDS_ERROR_OBJECT));
				return;
			}

			CGeometry *pObj = pFtr1->GetGeometry();
			if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pObj->GetDataPointSum()<2 )
			{
				GOutPut(StrFromResID(IDS_ERROR_OBJECT));
				return;
			}

			if( ((CGeoCurve*)pObj)->IsClosed() )
			{			
				GOutPut(StrFromResID(IDS_ERROR_OBJECT));
				return;
			}

			m_ptClick1 = pt;

			double r = m_lfToler;

			double mindis = -1, dis;
			int k = -1;
			for( i=0; i<m_arrRefFtrs.GetSize(); i++)
			{
				if( ProcessOne(pFtr1,m_arrRefFtrs[i],m_ptClick1,dis,TRUE,NULL) )
				{
					if( mindis<0 || mindis>dis )
					{
						mindis = dis;
						k = i;
					}
				}
			}

			if( k<0 || mindis>r )
			{
				GOutPut(StrFromResID(IDS_ERROR_OUTTOLERANCE));
			}
			else
			{
				CUndoBatchAction undo(m_pEditor,Name());
				ProcessOne(pFtr1,m_arrRefFtrs[k],m_ptClick1,dis,FALSE,&undo);
				undo.Commit();
			}

			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
		}
	}


	CEditCommand::PtClick(pt,flag);
}

BOOL CExtendOrTrimCommand::ProcessOne(CFeature *pFtr, CFeature *pFtrRef, PT_3D ptClick, double& dis, BOOL bTest, CUndoBatchAction *pUndo)
{
	//找到点中的地物	
	double r = m_lfToler;
	CGeometry *pObj = pFtr->GetGeometry();
	
	//---计算当前点位于哪一个端
	double alllen = pObj->GetShape()->GetLength();
	double curlen = pObj->GetShape()->GetLength(&ptClick);

	CArray<PT_3DEX,PT_3DEX> arrPts1;
	pObj->GetShape(arrPts1);
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( curlen<=alllen/2 )
	{
		nIndex = 0;
		expt0 = arrPts1[1];
		expt1 = arrPts1[0];
	}
	else
	{
		nIndex = arrPts1.GetSize()-1;
		expt0 = arrPts1[nIndex-1];
		expt1 = arrPts1[nIndex];
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;
	GetIntersectPoints(expt0,expt1,FtrToHandle(pFtrRef),arrRets,arrRets2,arrRets_z);
	
	if( arrRets.GetSize()<=0 )
	{		
		return FALSE;
	}

	//找到最接近1的
	double mint = 0;
	int k = -1;
	for( int i=0; i<arrRets.GetSize(); i++)
	{
		if( i==0 || fabs(mint-1)>fabs(arrRets[i]-1) )
		{
			mint = arrRets[i];
			k = i;
		}
	}		
	
	//延长端点到相交点
	double t = arrRets[k];
	PT_3DEX expt = expt1;
	expt.x = expt0.x + t*(expt1.x-expt0.x);
	expt.y = expt0.y + t*(expt1.y-expt0.y);

	if( m_bSnap3D )
	{
		expt.z = arrRets_z[0];
	}

	dis = GraphAPI::GGet2DDisOf2P(expt,expt1);

	if( dis<GraphAPI::g_lfDisTolerance || dis>r )
	{
		return FALSE;
	}

	if( bTest )
		return TRUE;

	m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);	
	
	CUndoVertex undo(m_pEditor,Name());

	undo.handle = FtrToHandle(pFtr);
	undo.ptOld = expt1;
	undo.ptNew = expt;
	undo.nPtType = PT_KEYCTRL::typeKey;
	undo.nPtIdx = nIndex;
	
	pFtr->GetGeometry()->SetDataPoint(nIndex,expt);		
	m_pEditor->RestoreObject(FtrToHandle(pFtr));

	pUndo->AddAction(&undo);

	if( m_bAddPt )
	{
		CGeometry *pGeo = pFtrRef->GetGeometry();
		const CShapeLine  *pb = pGeo->GetShape();
		int index = pb->FindNearestKeyPt(expt);

		m_pEditor->DeleteObject(FtrToHandle(pFtrRef), FALSE);

		CUndoVertex undo1(m_pEditor, Name());

		undo1.handle = FtrToHandle(pFtrRef);
		undo1.bRepeat = FALSE;
		undo1.nPtType = PT_KEYCTRL::typeKey;
		undo1.nPtIdx = index + 1;
		undo1.ptNew = expt;
		undo1.ptNew.z = arrRets_z[k];
		undo1.ptOld.pencode = penNone;

		pUndo->AddAction(&undo1);

		CArray<PT_3DEX, PT_3DEX> arrPts;
		pFtrRef->GetGeometry()->GetShape(arrPts);
		arrPts.InsertAt(undo1.nPtIdx, undo1.ptNew);

		pFtrRef->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize());
		m_pEditor->RestoreObject(FtrToHandle(pFtrRef));
	}
	return TRUE;	
}


//////////////////////////////////////////////////////////////////////
// CExtendOrTrimAllCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CExtendOrTrimAllCommand,CEditCommand)

CExtendOrTrimAllCommand::CExtendOrTrimAllCommand()
{
	m_nStep = -1;
	m_lfToler = 0;
	m_bAddPt = FALSE;
	m_bSnap3D = FALSE;
	strcat(m_strRegPath,"\\ExtendOrTrimAll");
}

CExtendOrTrimAllCommand::~CExtendOrTrimAllCommand()
{
	
}



CString CExtendOrTrimAllCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_EXTENDORTRIM_ALL);
}



void CExtendOrTrimAllCommand::Start()
{
 	if( !m_pEditor )return;
	
	m_bSnap3D = FALSE;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}
void CExtendOrTrimAllCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CExtendOrTrimAllCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CExtendOrTrimAllCommand::GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, FTR_HANDLE id, CArray<double,double> &ret1, CArray<double,double> &ret2, CArray<double,double> &ret_z)
{
	PT_3D pt3d0, pt3d1, tpt;
	
	COPY_3DPT(pt3d0,pt0);
	COPY_3DPT(pt3d1,pt1);
	
	CDlgDataSource *pDS = GETDS(m_pEditor);

	{
		CFeature *pFtr = HandleToFtr(id);
		if (!pFtr) return;

		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) return;
		
		CFtrArray arrObjs;

		arrObjs.Add(pFtr);
		
		for (int j=0; j<arrObjs.GetSize(); j++)
		{
			CFeature *pFtr0 = arrObjs[j];
			if (!pFtr0) continue;

			CGeometry *po = pFtr0->GetGeometry();
			if( !po )continue;
			
			const CShapeLine  *pb = po->GetShape();
			
			if (pb == NULL) continue;
			
			int npt2 = pb->GetPtsCount();
			if( npt2<=1 )continue;
			CArray<PT_3DEX,PT_3DEX> pts;
			pb->GetPts(pts);	
			
			for( int k=0; k<npt2-1; k++)
			{
				//求交
				double vector1x = pt3d1.x-pt3d0.x, vector1y = pt3d1.y-pt3d0.y;
				double vector2x = pts[k+1].x-pts[k].x, vector2y = pts[k+1].y-pts[k].y;
				
				double delta = vector1x*vector2y-vector1y*vector2x;
				if( delta<1e-10 && delta>-1e-10 )continue;
				
				double t1 = ( (pts[k].x-pt3d0.x)*vector2y-(pts[k].y-pt3d0.y)*vector2x )/delta;
				if( t1<0.0 )continue;
				
				double t2 = ( (pts[k].x-pt3d0.x)*vector1y-(pts[k].y-pt3d0.y)*vector1x )/delta;
				if( t2<0.0 || t2>1.0 )continue;
				
				double z = pts[k].z + t2 * (pts[k+1].z-pts[k].z);

				tpt.x = pt3d0.x + t1*vector1x;  tpt.y = pt3d0.y + t1*vector1y;
				
				//由小到大排序
				for( int m=0; m<ret1.GetSize(); m++)
				{
					if( t1<ret1[m] )break;
				}
				
				if( m<ret1.GetSize() )
				{
					ret1.InsertAt(m,t1);
					ret_z.InsertAt(m,z);
					ret2.InsertAt(m,k+t2);
				}
				else
				{
					ret1.Add(t1);
					ret_z.Add(z);
					ret2.Add(k+t2);
				}
			}

			if (pFtr0 != pFtr)
			{
				delete pFtr0;
			}
		}
		
	}
}

void CExtendOrTrimAllCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_strLayer0);
	tab.AddValue("Layer0",&CVariantEx(var));

	var = (LPCTSTR)(m_strLayer1);
	tab.AddValue("Layer1",&CVariantEx(var));

	var = (double)(m_lfToler);
	tab.AddValue("Toler",&CVariantEx(var));

	var = (bool)(m_bSnap3D);
	tab.AddValue("Snap3D",&CVariantEx(var));
	
	var = (bool)(m_bAddPt);
	tab.AddValue("AddPt",&CVariantEx(var));
}

void CExtendOrTrimAllCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("ExtendOrTrimCommand",Name());

	param->AddLayerNameParamEx("Layer0",(LPCTSTR)m_strLayer0,StrFromResID(IDS_CMDPLANE_PROCESS_LAY),NULL,LAYERPARAMITEM_LINEAREA);
	param->AddLayerNameParamEx("Layer1",(LPCTSTR)m_strLayer1,StrFromResID(IDS_CMDPLANE_REFLAY),NULL,LAYERPARAMITEM_LINEAREA);

	param->AddParam("Toler",(double)m_lfToler,StrFromResID(IDS_CMDPLANE_TOLER));

	param->AddParam("Snap3D",(bool)m_bSnap3D,StrFromResID(IDS_CMDPLANE_3DSNAP));

	param->AddParam("AddPt",(bool)m_bAddPt,StrFromResID(IDS_CMDPLANE_ADDPT));
}

void CExtendOrTrimAllCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Layer0",var) )
	{					
		m_strLayer0 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"Layer1",var) )
	{					
		m_strLayer1 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"Toler",var) )
	{
		m_lfToler = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"AddPt",var) )
	{
		m_bAddPt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"Snap3D",var) )
	{					
		m_bSnap3D = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	CEditCommand::SetParams(tab,bInit);
}


void CExtendOrTrimAllCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;

	CDataSourceEx *pDS = m_pEditor->GetDataSource();

	CFtrLayerArray layers0, layers1;
	pDS->GetFtrLayersByNameOrCode(m_strLayer0,layers0);
	if( layers0.GetSize()<=0 )return;

	pDS->GetFtrLayersByNameOrCode(m_strLayer1,layers1);

	if( m_strLayer1.IsEmpty() )
	{
		pDS->SaveAllQueryFlags(TRUE,TRUE);
	}
	else if( layers1.GetSize()<=0 )
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}
	else
	{
		pDS->SaveAllQueryFlags(TRUE,FALSE);
		for( int i=0; i<layers1.GetSize(); i++)
		{
			layers1[i]->SetAllowQuery(TRUE);
		}
	}

	int nSum = 0;
	for( int i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}

	GProgressStart(nSum);

	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();

	double r = m_lfToler;

	CUndoBatchAction undo(m_pEditor,Name());

	int nCount = 0;

	for( int k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];

		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			GProgressStep();

			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && !((CGeoCurveBase*)pGeo)->IsClosed() )
			{
				CGeoCurveBase *pGeoCurve = (CGeoCurveBase*)pGeo;
				CArray<PT_3DEX,PT_3DEX> arrPts; 
				pGeoCurve->GetShape(arrPts);

				if( arrPts.GetSize()<2 )
					continue;

				Envelope e;
				e.CreateFromPtAndRadius(arrPts[0],r);
				int num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;

				double min_dis = -1;
				const CPFeature *ftrs = pDQ->GetFoundHandles(num);

				for( int j=0; j<num; j++)
				{
					if( pFtr==ftrs[j] )
						continue;

					double dis = CalcExtendDistance(pFtr,ftrs[j],0);
					if( dis>GraphAPI::g_lfDisTolerance && (min_dis<0 || min_dis>dis) )
					{
						k = j;
						min_dis = dis;
					}				
				}

				BOOL bProcessed = FALSE;

				if( k>=0 && min_dis<r )
				{
					ProcessOneFeature(pFtr,ftrs[k],0,&undo);
					bProcessed = TRUE;
				}

				int index = arrPts.GetSize()-1;
				e.CreateFromPtAndRadius(arrPts[index],r);
				num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
				
				min_dis = -1;
				k = -1;
				ftrs = pDQ->GetFoundHandles(num);
				
				for( j=0; j<num; j++)
				{				
					if( pFtr==ftrs[j] )
						continue;

					double dis = CalcExtendDistance(pFtr,ftrs[j],index);
					if( dis>GraphAPI::g_lfDisTolerance && (min_dis<0 || min_dis>dis) )
					{
						k = j;
						min_dis = dis;
					}				
				}
				
				if( k>=0 && min_dis<r )
				{
					ProcessOneFeature(pFtr,ftrs[k],index,&undo);
					bProcessed = TRUE;
				}

				if( bProcessed )
					nCount++;
			}
		}
	}

	GProgressEnd();

	pDS->RestoreAllQueryFlags();

	CString strMsg;
	strMsg.Format(IDS_PROCESS_OBJ_NUM,nCount);
	GOutPut(strMsg);

	undo.Commit();

	Finish();

	CEditCommand::PtClick(pt,flag);

}


double CExtendOrTrimAllCommand::CalcExtendDistance(CPFeature f1, CPFeature f2, int nPtIndex)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return -1;
	
	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj )return -1;
	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )return -1;
	if( ((CGeoCurveBase*)pObj)->IsClosed() )return -1;
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = pObj->GetDataPoint(1);
		expt1 = pObj->GetDataPoint(0);
	}
	else
	{
		nIndex = pObj->GetDataPointSum()-1;
		expt0 = pObj->GetDataPoint(nIndex-1);
		expt1 = pObj->GetDataPoint(nIndex);
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;
	GetIntersectPoints(expt0,expt1,FtrToHandle(f2),arrRets,arrRets2,arrRets_z);
	
	if( arrRets.GetSize()<=0 )return -1;
	
	//找到最接近1的
	double mint = 0;
	int k = -1;
	for( int i=0; i<arrRets.GetSize(); i++)
	{
		if( i==0 || fabs(mint-1)>fabs(arrRets[i]-1) )
		{
			mint = arrRets[i];
			k = i;
		}
	}		
	
	//延长端点到相交点
	double t = arrRets[k];
	PT_3DEX expt = expt1;
	expt.x = expt0.x + t*(expt1.x-expt0.x);
	expt.y = expt0.y + t*(expt1.y-expt0.y);
	
	if( m_bSnap3D )
	{
		expt.z = arrRets_z[0];
	}
	
	return GraphAPI::GGet2DDisOf2P(expt,expt1);
}

void CExtendOrTrimAllCommand::ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex, CUndoBatchAction *pUndo)
{
	//找到点中的地物	
	double r = m_lfToler;

	CPFeature pFtr = f1;		
	if( !pFtr )return;
	
	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj )return;
	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )return;
	if( ((CGeoCurveBase*)pObj)->IsClosed() )return;

	CArray<PT_3DEX,PT_3DEX> arrPts0;
	pObj->GetShape(arrPts0);
		
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = arrPts0[1];
		expt1 = arrPts0[0];
	}
	else
	{
		nIndex = arrPts0.GetSize()-1;
		expt0 = arrPts0[nIndex-1];
		expt1 = arrPts0[nIndex];
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;
	GetIntersectPoints(expt0,expt1,FtrToHandle(f2),arrRets,arrRets2,arrRets_z);
	
	if( arrRets.GetSize()<=0 )return;

	//找到最接近1的
	double mint = 0;
	int k = -1;
	for( int i=0; i<arrRets.GetSize(); i++)
	{
		if( i==0 || fabs(mint-1)>fabs(arrRets[i]-1) )
		{
			mint = arrRets[i];
			k = i;
		}
	}		
	
	//延长端点到相交点
	double t = arrRets[k];
	PT_3DEX expt = expt1;
	expt.x = expt0.x + t*(expt1.x-expt0.x);
	expt.y = expt0.y + t*(expt1.y-expt0.y);

	if( m_bSnap3D )
	{
		expt.z = arrRets_z[0];
	}

	m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
		
	CUndoVertex undo(m_pEditor,Name());

	undo.handle = FtrToHandle(f1);
	undo.ptOld = expt1;
	undo.ptNew = expt;
	undo.nPtType = PT_KEYCTRL::typeKey;
	undo.nPtIdx = nIndex;
	
	pFtr->GetGeometry()->SetDataPoint(nIndex,expt);		
	m_pEditor->RestoreObject(FtrToHandle(pFtr));

	if( m_bAddPt )
	{
		m_pEditor->DeleteObject(FtrToHandle(f2),FALSE);

		CUndoVertex undo1(m_pEditor,Name());

		undo1.handle = FtrToHandle(f2);
		undo1.bRepeat = FALSE;
		undo1.nPtType = PT_KEYCTRL::typeKey;
		undo1.nPtIdx = (int)arrRets2[k]+1;
		undo1.ptNew = expt;
		undo1.ptNew.z = arrRets_z[k];
		undo1.ptOld.pencode = penNone;

		pUndo->AddAction(&undo);
		pUndo->AddAction(&undo1);

		CArray<PT_3DEX,PT_3DEX> arrPts;
		f2->GetGeometry()->GetShape(arrPts);
		arrPts.InsertAt(undo1.nPtIdx,undo1.ptNew);

		f2->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		m_pEditor->RestoreObject(FtrToHandle(f2));
	}
	else
	{
		pUndo->AddAction(&undo);
	}				
}



//////////////////////////////////////////////////////////////////////
// CLLSuspendProcess1Command Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CLLSuspendProcess1Command,CEditCommand)

CLLSuspendProcess1Command::CLLSuspendProcess1Command()
{
	m_nStep = -1;
	m_lfToler = 0;
	m_bAddPt = FALSE;
	m_bSnap3D = FALSE;
	strcat(m_strRegPath,"\\LLSuspendProcess1");
}

CLLSuspendProcess1Command::~CLLSuspendProcess1Command()
{
	
}



CString CLLSuspendProcess1Command::Name()
{ 
	return StrFromResID(IDS_CMDNAME_LLSUSPENDPROCESS1);
}



void CLLSuspendProcess1Command::Start()
{
 	if( !m_pEditor )return;
	
	m_bSnap3D = FALSE;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}
void CLLSuspendProcess1Command::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CLLSuspendProcess1Command::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CLLSuspendProcess1Command::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_strLayer0);
	tab.AddValue("Layer0",&CVariantEx(var));

	var = (LPCTSTR)(m_strLayer1);
	tab.AddValue("Layer1",&CVariantEx(var));

	var = (double)(m_lfToler);
	tab.AddValue("Toler",&CVariantEx(var));

	var = (bool)(m_bSnap3D);
	tab.AddValue("Snap3D",&CVariantEx(var));
	
	var = (bool)(m_bAddPt);
	tab.AddValue("AddPt",&CVariantEx(var));
}

void CLLSuspendProcess1Command::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("SuspendProcess1Command",Name());

	param->AddLayerNameParamEx("Layer0",(LPCTSTR)m_strLayer0,StrFromResID(IDS_CMDPLANE_PROCESS_LAY),NULL,LAYERPARAMITEM_LINE);
	param->AddLayerNameParamEx("Layer1",(LPCTSTR)m_strLayer1,StrFromResID(IDS_CMDPLANE_REFLAY),NULL,LAYERPARAMITEM_LINE);

	param->AddParam("Toler",(double)m_lfToler,StrFromResID(IDS_CMDPLANE_TOLER));

	param->AddParam("Snap3D",(bool)m_bSnap3D,StrFromResID(IDS_CMDPLANE_3DSNAP));

	param->AddParam("AddPt",(bool)m_bAddPt,StrFromResID(IDS_CMDPLANE_ADDPT));
}

void CLLSuspendProcess1Command::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Layer0",var) )
	{					
		m_strLayer0 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"Layer1",var) )
	{					
		m_strLayer1 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"Toler",var) )
	{
		m_lfToler = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"AddPt",var) )
	{
		m_bAddPt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"Snap3D",var) )
	{					
		m_bSnap3D = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	CEditCommand::SetParams(tab,bInit);
}


void CLLSuspendProcess1Command::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;

	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	
	CFtrLayerArray layers0, layers1;
	pDS->GetFtrLayersByNameOrCode(m_strLayer0,layers0);
	if( layers0.GetSize()<=0 )return;
	
	pDS->GetFtrLayersByNameOrCode(m_strLayer1,layers1);
		
	if( m_strLayer1.IsEmpty() )
	{
		pDS->SaveAllQueryFlags(TRUE,TRUE);
	}
	else if( layers1.GetSize()<=0 )
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}
	else
	{
		pDS->SaveAllQueryFlags(TRUE,FALSE);
		for( int i=0; i<layers1.GetSize(); i++)
		{
			layers1[i]->SetAllowQuery(TRUE);
		}
	}
	
	int nSum = 0;
	for( int i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
	
	GProgressStart(nSum);

	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();

	double r = m_lfToler;

	CUndoBatchAction undo(m_pEditor,Name());

	for( int k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			GProgressStep();

			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && !((CGeoCurveBase*)pGeo)->IsClosed() )
			{
				CGeoCurveBase *pGeoCurve = (CGeoCurveBase*)pGeo;
				CArray<PT_3DEX,PT_3DEX> arrPts; 
				pGeoCurve->GetShape(arrPts);

				if( arrPts.GetSize()<2 )
					continue;

				Envelope e;
				e.CreateFromPtAndRadius(arrPts[0],r);
				int num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;

				double min_dis = -1;
				const CPFeature *ftrs = pDQ->GetFoundHandles(num);

				for( int j=0; j<num; j++)
				{
					if( pFtr==ftrs[j] )
						continue;

					double dis = CalcSnapDistance(pFtr,ftrs[j],0);
					if( min_dis<0 || min_dis>dis )
					{
						k = j;
						min_dis = dis;
					}				
				}

				if( k>=0 )
					ProcessOneFeature(pFtr,ftrs[k],0,&undo);

				int index = arrPts.GetSize()-1;
				e.CreateFromPtAndRadius(arrPts[index],r);
				num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
				
				min_dis = -1;
				k = -1;
				ftrs = pDQ->GetFoundHandles(num);
				
				for( j=0; j<num; j++)
				{				
					if( pFtr==ftrs[j] )
						continue;

					double dis = CalcSnapDistance(pFtr,ftrs[j],index);
					if( min_dis<0 || min_dis>dis )
					{
						k = j;
						min_dis = dis;
					}				
				}
				
				if( k>=0 )
					ProcessOneFeature(pFtr,ftrs[k],index,&undo);
			}
		}
	}

	GProgressEnd();

	pDS->RestoreAllQueryFlags();

	undo.Commit();

	Finish();

	CEditCommand::PtClick(pt,flag);

}


double CLLSuspendProcess1Command::CalcSnapDistance(CPFeature f1, CPFeature f2, int nPtIndex)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return -1;
	
	CGeometry *pObj = pFtr->GetGeometry();

	CArray<PT_3DEX,PT_3DEX> arrPts0;
	pObj->GetShape(arrPts0);
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = arrPts0[1];
		expt1 = arrPts0[0];
	}
	else
	{
		nIndex = arrPts0.GetSize()-1;
		expt0 = arrPts0[nIndex-1];
		expt1 = arrPts0[nIndex];
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;

	CGeometry *pObj2 = f2->GetGeometry();
	PT_KEYCTRL nearestPt = pObj2->FindNearestKeyCtrlPt(expt1,r,NULL,1);

	PT_3DEX newPt;
	int nIndex2 = -1;
	if( nearestPt.IsValid() )
	{
		newPt = pObj2->GetDataPoint(nearestPt.index);
	}
	else
	{
		Envelope e;
		e.CreateFromPtAndRadius(expt1,r);
		if( !pObj2->FindNearestBasePt(expt1,e,NULL,&newPt,NULL) )
			return -1;
	}
	
	return GraphAPI::GGet2DDisOf2P(expt1,newPt);
}

void CLLSuspendProcess1Command::ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex, CUndoBatchAction *pUndo)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return;
	
	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj )return;
	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )return;
	if( ((CGeoCurveBase*)pObj)->IsClosed() )return;
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = pObj->GetDataPoint(1);
		expt1 = pObj->GetDataPoint(0);
	}
	else
	{
		nIndex = pObj->GetDataPointSum()-1;
		expt0 = pObj->GetDataPoint(nIndex-1);
		expt1 = pObj->GetDataPoint(nIndex);
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;

	CGeometry *pObj2 = f2->GetGeometry();
	PT_KEYCTRL nearestPt = pObj2->FindNearestKeyCtrlPt(expt1,r,NULL,1);

	BOOL bSnapVertex = FALSE;
	PT_3DEX newPt = expt1;
	int nIndex2 = -1;
	if( nearestPt.IsValid() )
	{
		newPt = pObj2->GetDataPoint(nearestPt.index);
		nIndex2 = nearestPt.index;
		bSnapVertex = TRUE;
	}
	else
	{
		Envelope e;
		e.CreateFromPtAndRadius(expt1,r);
		if( !pObj2->FindNearestBasePt(expt1,e,NULL,&newPt,NULL) )
			return;

		const CShapeLine *pShp = pObj2->GetShape();
		if( !pShp )
			return;

		nIndex2 = pShp->FindNearestKeyPt(newPt);
	}
	
	m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
		
	CUndoVertex undo(m_pEditor,Name());

	undo.handle = FtrToHandle(f1);
	undo.ptOld = expt1;
	undo.ptNew = newPt;
	undo.nPtType = PT_KEYCTRL::typeKey;
	undo.nPtIdx = nIndex;

	if( !m_bSnap3D )
	{
		undo.ptNew.z = expt1.z;
	}
	
	pFtr->GetGeometry()->SetDataPoint(nPtIndex,undo.ptNew);		
	m_pEditor->RestoreObject(FtrToHandle(pFtr));

	if( m_bAddPt && !bSnapVertex )
	{
		m_pEditor->DeleteObject(FtrToHandle(f2),FALSE);

		CUndoVertex undo1(m_pEditor,Name());

		undo1.handle = FtrToHandle(f2);
		undo1.bRepeat = FALSE;
		undo1.nPtType = PT_KEYCTRL::typeKey;
		undo1.nPtIdx = nIndex2+1;
		undo1.ptNew = newPt;
		undo1.ptOld.pencode = penNone;

		pUndo->AddAction(&undo);
		pUndo->AddAction(&undo1);

		CArray<PT_3DEX,PT_3DEX> arrPts;
		f2->GetGeometry()->GetShape(arrPts);
		arrPts.InsertAt(undo1.nPtIdx,undo1.ptNew);

		f2->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		m_pEditor->RestoreObject(FtrToHandle(f2));
	}
	else
	{
		pUndo->AddAction(&undo);
	}				
}



//////////////////////////////////////////////////////////////////////
// CPLSuspendProcessCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPLSuspendProcessCommand,CEditCommand)

CPLSuspendProcessCommand::CPLSuspendProcessCommand()
{
	m_nStep = -1;
	m_lfToler = 0;
	m_bAddPt = TRUE;
	m_bSnap3D = FALSE;
	m_nModifyType = typeMovePt;
	strcat(m_strRegPath,"\\PLSuspendProcess");
}

CPLSuspendProcessCommand::~CPLSuspendProcessCommand()
{
	
}



CString CPLSuspendProcessCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_PLSUSPENDPROCESS);
}



void CPLSuspendProcessCommand::Start()
{
 	if( !m_pEditor )return;
	
	m_bSnap3D = FALSE;
	m_nModifyType = typeMovePt;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}
void CPLSuspendProcessCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CPLSuspendProcessCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}


void CPLSuspendProcessCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_strLayer0);
	tab.AddValue("Layer0",&CVariantEx(var));

	var = (LPCTSTR)(m_strLayer1);
	tab.AddValue("Layer1",&CVariantEx(var));
	
	var = (double)(m_lfToler);
	tab.AddValue("Toler",&CVariantEx(var));

	var = (long)(m_nModifyType);
	tab.AddValue("ModifyType",&CVariantEx(var));

	var = (bool)(m_bSnap3D);
	tab.AddValue("Snap3D",&CVariantEx(var));
	
}

void CPLSuspendProcessCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("PLSuspendProcessCommand",Name());

	param->AddLayerNameParamEx("Layer0",(LPCTSTR)m_strLayer0,StrFromResID(IDS_CHKCMD_PTS_LAYERS),NULL,LAYERPARAMITEM_POINT);
	param->AddLayerNameParamEx("Layer1",(LPCTSTR)m_strLayer1,StrFromResID(IDS_PLANENAME_LAYERNAME),NULL,LAYERPARAMITEM_LINE);

	param->AddParam("Toler",(double)m_lfToler,StrFromResID(IDS_CMDPLANE_TOLER));

	param->BeginOptionParam("ModifyType",StrFromResID(IDS_PLANENAME_MODIFYTYPE),StrFromResID(IDS_PLANENAME_MODIFYTYPE));
	param->AddOption(StrFromResID(IDS_PLANENAME_MOVEPT),(int)typeMovePt,0,m_nModifyType==typeMovePt);
	param->AddOption(StrFromResID(IDS_PLANENAME_MOVELINE),(int)typeMoveLine,0,m_nModifyType==typeMoveLine);
	param->EndOptionParam();

	param->AddParam("Snap3D",(bool)m_bSnap3D,StrFromResID(IDS_CMDPLANE_3DSNAP));

}

void CPLSuspendProcessCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Layer0",var) )
	{					
		m_strLayer0 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"Layer1",var) )
	{					
		m_strLayer1 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"Toler",var) )
	{
		m_lfToler = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	if( tab.GetValue(0,"ModifyType",var) )
	{
		m_nModifyType = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"Snap3D",var) )
	{					
		m_bSnap3D = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	CEditCommand::SetParams(tab,bInit);
}


void CPLSuspendProcessCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;

	CDataSourceEx *pDS = m_pEditor->GetDataSource();


	CFtrLayerArray layers0, layers1;
	pDS->GetFtrLayersByNameOrCode(m_strLayer0,layers0);
	if( layers0.GetSize()<=0 )return;

	pDS->GetFtrLayersByNameOrCode(m_strLayer1,layers1);

	if( m_strLayer1.IsEmpty() )
	{
		pDS->SaveAllQueryFlags(TRUE,TRUE);
	}
	else if( layers1.GetSize()<=0 )
	{
		GOutPut(StrFromResID(IDS_PARAM_ERROR));
		return;
	}
	else
	{
		pDS->SaveAllQueryFlags(TRUE,FALSE);
		for( int i=0; i<layers1.GetSize(); i++)
		{
			layers1[i]->SetAllowQuery(TRUE);
		}
	}
	
	int nSum = 0;
	for( int i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
	
	GProgressStart(nSum);

	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();

	double r = m_lfToler;

	CUndoBatchAction undo(m_pEditor,Name());

	for( int k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];	

		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			GProgressStep();

			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
			{
				CArray<PT_3DEX,PT_3DEX> arrPts; 
				pGeo->GetShape(arrPts);

				if( arrPts.GetSize()<1 )
					continue;

				Envelope e;
				e.CreateFromPtAndRadius(arrPts[0],r);
				int num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;

				double min_dis = -1;
				const CPFeature *ftrs = pDQ->GetFoundHandles(num);

				for( int j=0; j<num; j++)
				{
					if( pFtr==ftrs[j] )
						continue;

					double dis = CalcSnapDistance(pFtr,ftrs[j],0);
					if( min_dis<0 || min_dis>dis )
					{
						k = j;
						min_dis = dis;
					}				
				}

				if( k>=0 )
					ProcessOneFeature(pFtr,ftrs[k],0,&undo);
			}
		}
	}

	GProgressEnd();

	pDS->RestoreAllQueryFlags();

	undo.Commit();

	Finish();

	CEditCommand::PtClick(pt,flag);

}


double CPLSuspendProcessCommand::CalcSnapDistance(CPFeature f1, CPFeature f2, int nPtIndex)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return -1;
	
	CGeometry *pObj = pFtr->GetGeometry();

	CArray<PT_3DEX,PT_3DEX> arrPts0;
	pObj->GetShape(arrPts0);
	
	PT_3DEX expt1 = arrPts0[0];

	CGeometry *pObj2 = f2->GetGeometry();
	PT_KEYCTRL nearestPt = pObj2->FindNearestKeyCtrlPt(expt1,r,NULL,1);

	PT_3DEX newPt;
	int nIndex2 = -1;
	if( nearestPt.IsValid() )
	{
		newPt = pObj2->GetDataPoint(nearestPt.index);
	}
	else
	{
		Envelope e;
		e.CreateFromPtAndRadius(expt1,r);
		if( !pObj2->FindNearestBasePt(expt1,e,NULL,&newPt,NULL) )
			return -1;
	}
	
	return GraphAPI::GGet2DDisOf2P(expt1,newPt);
}

void CPLSuspendProcessCommand::ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex, CUndoBatchAction *pUndo)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return;
	
	CGeometry *pObj = pFtr->GetGeometry();
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = pObj->GetDataPoint(1);
		expt1 = pObj->GetDataPoint(0);
	}
	else
	{
		nIndex = pObj->GetDataPointSum()-1;
		expt0 = pObj->GetDataPoint(nIndex-1);
		expt1 = pObj->GetDataPoint(nIndex);
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;

	CGeometry *pObj2 = f2->GetGeometry();
	PT_KEYCTRL nearestPt = pObj2->FindNearestKeyCtrlPt(expt1,r,NULL,1);

	BOOL bSnapVertex = FALSE;
	PT_3DEX newPt = expt1;
	int nIndex2 = -1;
	if( nearestPt.IsValid() )
	{
		newPt = pObj2->GetDataPoint(nearestPt.index);
		nIndex2 = nearestPt.index;
		bSnapVertex = TRUE;
	}
	else
	{
		Envelope e;
		e.CreateFromPtAndRadius(expt1,r);
		if( !pObj2->FindNearestBasePt(expt1,e,NULL,&newPt,NULL) )
			return;

		const CShapeLine *pShp = pObj2->GetShape();
		if( !pShp )
			return;

		nIndex2 = pShp->FindNearestKeyPt(newPt);
	}

	//点向线移动
	if( m_nModifyType==typeMovePt )
	{
		m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
		
		CUndoVertex undo(m_pEditor,Name());
		
		undo.handle = FtrToHandle(f1);
		undo.ptOld = expt1;
		undo.ptNew = newPt;
		undo.nPtType = PT_KEYCTRL::typeKey;
		undo.nPtIdx = nIndex;
		
		if( !m_bSnap3D )
		{
			undo.ptNew.z = expt1.z;
		}
		
		pFtr->GetGeometry()->SetDataPoint(nPtIndex,undo.ptNew);		
		m_pEditor->RestoreObject(FtrToHandle(pFtr));
		
		// m_bAddPt 总是为 TRUE
		if( m_bAddPt && !bSnapVertex )
		{
			m_pEditor->DeleteObject(FtrToHandle(f2),FALSE);
			
			CUndoVertex undo1(m_pEditor,Name());
			
			undo1.handle = FtrToHandle(f2);
			undo1.bRepeat = FALSE;
			undo1.nPtType = PT_KEYCTRL::typeKey;
			undo1.nPtIdx = nIndex2+1;
			undo1.ptNew = newPt;
			undo1.ptOld.pencode = penNone;
			
			pUndo->AddAction(&undo);
			pUndo->AddAction(&undo1);
			
			CArray<PT_3DEX,PT_3DEX> arrPts;
			f2->GetGeometry()->GetShape(arrPts);
			arrPts.InsertAt(undo1.nPtIdx,undo1.ptNew);
			
			f2->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
			m_pEditor->RestoreObject(FtrToHandle(f2));
		}
		else
		{
			pUndo->AddAction(&undo);
		}	
	}
	else if( m_nModifyType==typeMoveLine )
	{
		if( bSnapVertex )
		{
			//线上节点向点对象移动
			m_pEditor->DeleteObject(FtrToHandle(f2),FALSE);
			
			CUndoVertex undo(m_pEditor,Name());
			
			undo.handle = FtrToHandle(f2);
			undo.ptOld = pObj2->GetDataPoint(nIndex2);
			undo.ptNew = expt1;
			undo.nPtType = PT_KEYCTRL::typeKey;
			undo.nPtIdx = nIndex2;
			
			if( !m_bSnap3D )
			{
				undo.ptNew.z = newPt.z;
			}
			
			f2->GetGeometry()->SetDataPoint(nIndex2,undo.ptNew);		
			m_pEditor->RestoreObject(FtrToHandle(f2));

			pUndo->AddAction(&undo);
		}
		else
		{
			//线上插入新点，点的坐标等于点对象的坐标
			m_pEditor->DeleteObject(FtrToHandle(f2),FALSE);
			
			CUndoVertex undo1(m_pEditor,Name());
			
			undo1.handle = FtrToHandle(f2);
			undo1.bRepeat = FALSE;
			undo1.nPtType = PT_KEYCTRL::typeKey;
			undo1.nPtIdx = nIndex2+1;
			undo1.ptNew = expt1;
			undo1.ptOld.pencode = penNone;

			if( !m_bSnap3D )
			{
				undo1.ptNew.z = newPt.z;
			}
			
			pUndo->AddAction(&undo1);
			
			CArray<PT_3DEX,PT_3DEX> arrPts;
			f2->GetGeometry()->GetShape(arrPts);
			arrPts.InsertAt(undo1.nPtIdx,undo1.ptNew);
			
			f2->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
			m_pEditor->RestoreObject(FtrToHandle(f2));
		}
	}
			
}


//////////////////////////////////////////////////////////////////////
// CTopoPreTreatCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTopoPreTreatCommand,CEditCommand)

CTopoPreTreatCommand::CTopoPreTreatCommand()
{
	m_nStep = -1;
	m_lfToler = 0;
	m_bAddPt = FALSE;
	m_bSnap3D = FALSE;
	strcat(m_strRegPath,"\\TopoPreTreat");
}

CTopoPreTreatCommand::~CTopoPreTreatCommand()
{
	
}



CString CTopoPreTreatCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TOPO_PRETREAT);
}



void CTopoPreTreatCommand::Start()
{
 	if( !m_pEditor )return;
	
	m_bSnap3D = FALSE;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}
void CTopoPreTreatCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CTopoPreTreatCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CTopoPreTreatCommand::GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, FTR_HANDLE id, CArray<double,double> &ret1, CArray<double,double> &ret2, CArray<double,double> &ret_z)
{
	PT_3D pt3d0, pt3d1, tpt;
	
	COPY_3DPT(pt3d0,pt0);
	COPY_3DPT(pt3d1,pt1);
	
	CDlgDataSource *pDS = GETDS(m_pEditor);

	{
		CFeature *pFtr = HandleToFtr(id);
		if (!pFtr) return;

		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) return;
		
		CFtrArray arrObjs;

		arrObjs.Add(pFtr);
		
		for (int j=0; j<arrObjs.GetSize(); j++)
		{
			CFeature *pFtr0 = arrObjs[j];
			if (!pFtr0) continue;

			CGeometry *po = pFtr0->GetGeometry();
			if( !po )continue;
			
			const CShapeLine  *pb = po->GetShape();
			
			if (pb == NULL) continue;
			
			int npt2 = pb->GetPtsCount();
			if( npt2<=1 )continue;
			CArray<PT_3DEX,PT_3DEX> pts;
			pb->GetPts(pts);	
			
			for( int k=0; k<npt2-1; k++)
			{
				//求交
				double vector1x = pt3d1.x-pt3d0.x, vector1y = pt3d1.y-pt3d0.y;
				double vector2x = pts[k+1].x-pts[k].x, vector2y = pts[k+1].y-pts[k].y;
				
				double delta = vector1x*vector2y-vector1y*vector2x;
				if( delta<1e-10 && delta>-1e-10 )continue;
				
				double t1 = ( (pts[k].x-pt3d0.x)*vector2y-(pts[k].y-pt3d0.y)*vector2x )/delta;
				if( t1<0.0 )continue;
				
				double t2 = ( (pts[k].x-pt3d0.x)*vector1y-(pts[k].y-pt3d0.y)*vector1x )/delta;
				if( t2<0.0 || t2>1.0 )continue;
				
				double z = pts[k].z + t2 * (pts[k+1].z-pts[k].z);

				tpt.x = pt3d0.x + t1*vector1x;  tpt.y = pt3d0.y + t1*vector1y;
				if( fabs(tpt.x-pt3d1.x)<GraphAPI::g_lfDisTolerance && fabs(tpt.y-pt3d1.y)<GraphAPI::g_lfDisTolerance )
					continue;
				
				//由小到大排序
				for( int m=0; m<ret1.GetSize(); m++)
				{
					if( t1<ret1[m] )break;
				}
				
				if( m<ret1.GetSize() )
				{
					ret1.InsertAt(m,t1);
					ret_z.InsertAt(m,z);
					ret2.InsertAt(m,k+t2);
				}
				else
				{
					ret1.Add(t1);
					ret_z.Add(z);
					ret2.Add(k+t2);
				}
			}

			if (pFtr0 != pFtr)
			{
				delete pFtr0;
			}
		}
		
	}
}

void CTopoPreTreatCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_strLayer0);
	tab.AddValue("Layer0",&CVariantEx(var));

	var = (double)(m_lfToler);
	tab.AddValue("Toler",&CVariantEx(var));
/*
	var = (bool)(m_bSnap3D);
	tab.AddValue("Snap3D",&CVariantEx(var));
	
	var = (bool)(m_bAddPt);
	tab.AddValue("AddPt",&CVariantEx(var));
*/
}

void CTopoPreTreatCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("SuspendProcess1Command",Name());

	param->AddLayerNameParamEx("Layer0",(LPCTSTR)m_strLayer0,StrFromResID(IDS_CMDPLANE_PROCESS_LAY));

	param->AddParam("Toler",(double)m_lfToler,StrFromResID(IDS_CMDPLANE_TOLER));

//	param->AddParam("Snap3D",(bool)m_bSnap3D,StrFromResID(IDS_CMDPLANE_3DSNAP));

//	param->AddParam("AddPt",(bool)m_bAddPt,StrFromResID(IDS_CMDPLANE_ADDPT));
}

void CTopoPreTreatCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Layer0",var) )
	{					
		m_strLayer0 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"Toler",var) )
	{
		m_lfToler = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
/*
	if( tab.GetValue(0,"AddPt",var) )
	{
		m_bAddPt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"Snap3D",var) )
	{					
		m_bSnap3D = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}
*/
	CEditCommand::SetParams(tab,bInit);
}


void CTopoPreTreatCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;

	int i;

	//只让目标图层可以查询
	CDataSourceEx *pDS = m_pEditor->GetDataSource();

	CFtrLayerArray layers0, layers1;
	pDS->GetFtrLayersByNameOrCode_editable(m_strLayer0,layers0);
	int nSum = 0;

	pDS->SaveAllQueryFlags(TRUE,FALSE);

	//不包含查询标记层
	for( i=0; i<layers0.GetSize(); i++)
	{
		if(StrFromResID(IDS_MARKLAYER_NAME) == layers0[i]->GetName())
		{
			layers0.RemoveAt(i);
			break;
		}
	}
	
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
		
		layers0[i]->SetAllowQuery(TRUE);
	}

	CUndoBatchAction undo(m_pEditor,Name());

	GProgressStart(9*nSum);

	//线串化, Progress: 1*nSum
	Linearize(layers0,&undo);
	
	//节点咬合, Progress: 1*nSum
	SnapVertexes(layers0,&undo);

	//消除悬挂点, Progress: 1*nSum
	ProcessSuspend(layers0,&undo);

	//创建面域点, Progress: 0
	CreateSurfacePoints(layers0,&undo);	

	//打断自相交线, Progress: 2*nSum
	CutLines_self(layers0,&undo);

	//打断相交线, Progress: 2*nSum
	CutLines(layers0,&undo);

	//剔除公共边, Progress: 1*nSum
	DeleteCommonLine(layers0,&undo);

	GProgressEnd();

	pDS->RestoreAllQueryFlags();

	undo.Commit();

	Finish();

	CEditCommand::PtClick(pt,flag);

}


BOOL CTopoPreTreatCommand::Linearize(CFtrLayerArray& layers0, CUndoBatchAction* pUndo)
{
	int i, j, k;

	double r = m_lfToler;
	
	//1、线串化 + 闭合点精确化处理 + 双线打散、复杂面打散
	for( i=0; i<layers0.GetSize(); i++)
	{
		int nObj = layers0[i]->GetObjectCount();
		for( j=0; j<nObj; j++)
		{
			CFeature *pFtr = layers0[i]->GetObject(j);
			if( !pFtr )continue;

			GProgressStep();
			
			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			{
				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);
				
				//本身是折线，就不线串化
				for(k=0; k<arrPts.GetSize(); k++)
				{
					int pencode = arrPts[k].pencode;
					if( pencode==penArc || pencode==pen3PArc || pencode==penSpline )
						break;
				}
				
				if( k<arrPts.GetSize() )
				{								
					CGeometry *pGeo2 = pGeo->Linearize();
					if( pGeo2 )
					{
						CArray<PT_3DEX,PT_3DEX> arrPts2;
						pGeo2->GetShape(arrPts2);

						m_pEditor->DeleteObject(FtrToHandle(pFtr));
						pGeo->CreateShape(arrPts2.GetData(),arrPts2.GetSize());
						
						m_pEditor->RestoreObject(FtrToHandle(pFtr));
						delete pGeo2;

						CUndoShape undo(m_pEditor,Name());
						undo.arrPtsOld.Copy(arrPts);
						undo.arrPtsNew.Copy(arrPts2);

						undo.handle = FtrToHandle(pFtr);
						pUndo->AddAction(&undo);
					}
				}

				pGeo->GetShape(arrPts);

				int npt = arrPts.GetSize();
				double dis = GraphAPI::GGet2DDisOf2P(arrPts[0],arrPts[npt-1]);
				if( dis>0 && dis<r )
				{
					CUndoVertex undo(m_pEditor,Name());
					undo.bRepeat = FALSE;
					undo.handle = FtrToHandle(pFtr);
					undo.nPtType = PT_KEYCTRL::typeKey;
					undo.ptOld = arrPts[npt-1];

					arrPts[npt-1].x = arrPts[0].x;
					arrPts[npt-1].y = arrPts[0].y;

					m_pEditor->DeleteObject(FtrToHandle(pFtr));

					pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());

					m_pEditor->RestoreObject(FtrToHandle(pFtr));

					undo.ptNew = arrPts[npt-1];
					undo.nPtIdx = npt-1;					
					undo.handle = FtrToHandle(pFtr);

					pUndo->AddAction(&undo);
				}

				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) ||
					pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) )
				{
					CGeoArray geos;

					if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
					{
						CGeometry *pGeo1=NULL, *pGeo2=NULL;
						((CGeoDCurve*)pGeo)->Separate(pGeo1,pGeo2);
						if( pGeo1 && pGeo2 )
						{
							geos.Add(pGeo1);
							geos.Add(pGeo2);
						}
						else
						{
							if( pGeo1 )delete pGeo1;
							if( pGeo2 )delete pGeo2;
						}
					}
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
					{
						CGeometry *pGeo1=NULL, *pGeo2=NULL;
						((CGeoParallel*)pGeo)->Separate(pGeo1,pGeo2);
						if( pGeo1 && pGeo2 )
						{
							geos.Add(pGeo1);
							geos.Add(pGeo2);
						}
						else
						{
							if( pGeo1 )delete pGeo1;
							if( pGeo2 )delete pGeo2;
						}
					}
					else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) )
					{
						CGeoMultiSurface *pMGeo = (CGeoMultiSurface*)pGeo;
						int nSurface = pMGeo->GetSurfaceNum();
						for( int m=0; m<nSurface; m++)
						{
							CArray<PT_3DEX,PT_3DEX> arrPts2;
							pMGeo->GetSurface(m,arrPts2);
							if( arrPts2.GetSize()<4 )
								continue;
							arrPts2[0].pencode = penLine;

							CGeoSurface *pNewObj = new CGeoSurface();
							pNewObj->CopyFrom(pMGeo);
							if( pNewObj->CreateShape(arrPts2.GetData(),arrPts2.GetSize()) )
							{
								geos.Add(pNewObj);
							}
							else
							{
								delete pNewObj;
							}
						}
					}

					CUndoFtrs undo(m_pEditor,Name());
					for( int m=0; m<geos.GetSize(); m++)
					{
						CFeature *pNewFtr = pFtr->Clone();
						pNewFtr->SetGeometry(geos[m]);

						m_pEditor->AddObject(pNewFtr,layers0[i]->GetID());
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNewFtr);
						undo.AddNewFeature(FtrToHandle(pNewFtr));
					}

					if( geos.GetSize()>0 )
					{
						m_pEditor->DeleteObject(FtrToHandle(pFtr));
						undo.AddOldFeature(FtrToHandle(pFtr));
						pUndo->AddAction(&undo);
					}
				}
			}
		}
	}
	return TRUE;
}

//逻辑与线线悬挂处理中的功能类似
BOOL CTopoPreTreatCommand::ProcessSuspend(CFtrLayerArray& layers0, CUndoBatchAction* pUndo)
{	
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
	
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	
	double r = m_lfToler;
		
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			GProgressStep();
			
			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && !((CGeoCurveBase*)pGeo)->IsClosed() )
			{
				CGeoCurveBase *pGeoCurve = (CGeoCurveBase*)pGeo;
				CArray<PT_3DEX,PT_3DEX> arrPts; 
				pGeoCurve->GetShape(arrPts);
				
				if( arrPts.GetSize()<2 )
					continue;
				
				Envelope e;
				e.CreateFromPtAndRadius(arrPts[0],r);
				int num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
				
				double min_dis = -1;
				const CPFeature *ftrs = pDQ->GetFoundHandles(num);
				
				for( j=0; j<num; j++)
				{
					if( pFtr==ftrs[j] )
						continue;
					
					double dis = CalcSnapDistance(pFtr,ftrs[j],0);
					if( min_dis<0 || min_dis>dis )
					{
						k = j;
						min_dis = dis;
					}				
				}
				
				if( k>=0 )
					ProcessOneFeature(pFtr,ftrs[k],0,pUndo);
				
				int index = arrPts.GetSize()-1;
				e.CreateFromPtAndRadius(arrPts[index],r);
				num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
				
				min_dis = -1;
				k = -1;
				ftrs = pDQ->GetFoundHandles(num);
				
				for( j=0; j<num; j++)
				{				
					if( pFtr==ftrs[j] )
						continue;
					
					double dis = CalcSnapDistance(pFtr,ftrs[j],index);
					if( min_dis<0 || min_dis>dis )
					{
						k = j;
						min_dis = dis;
					}				
				}
				
				if( k>=0 )
					ProcessOneFeature(pFtr,ftrs[k],index,pUndo);
			}
		}
	}
	
	return TRUE;
}


static  int compare_intersect_item(const void *p1, const void *p2)
{
	CTopoPreTreatCommand::IntersectItem *t1 = (CTopoPreTreatCommand::IntersectItem*)p1;
	CTopoPreTreatCommand::IntersectItem *t2 = (CTopoPreTreatCommand::IntersectItem*)p2;

	if( t1->pFtr<t2->pFtr )
		return -1;
	else if( t1->pFtr>t2->pFtr )
		return 1;

	if( t1->ptIdx<t2->ptIdx )
		return -1;
	else if( t1->ptIdx>t2->ptIdx )
		return 1;

	if( t1->t<t2->t )
		return -1;
	else if( t1->t>t2->t )
		return 1;

	return 0;
}


static  int compare_snap_item(const void *p1, const void *p2)
{
	CTopoPreTreatCommand::SnapItem *t1 = (CTopoPreTreatCommand::SnapItem*)p1;
	CTopoPreTreatCommand::SnapItem *t2 = (CTopoPreTreatCommand::SnapItem*)p2;
	
	if( t1->pFtr1<t2->pFtr1 )
		return -1;
	else if( t1->pFtr1>t2->pFtr1 )
		return 1;	
	
	if( t1->ptIdx1<t2->ptIdx1 )
		return -1;
	else if( t1->ptIdx1>t2->ptIdx1 )
		return 1;	
	
	return 0;
}



BOOL CTopoPreTreatCommand::CutLines_self(CFtrLayerArray& layers0, CUndoBatchAction* pUndo)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}

	//计算所有的交点，并将交点信息放入数组
	
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			j++;
		}
	}

	ftrsArray.SetSize(j);

	IntersectItemArray arr;

	for( i=0; i<j; i++)
	{
		GProgressStep();
		GetIntersects_self(ftrsArray[i],arr);
	}

	if( arr.GetSize()<=1 )
		return TRUE;

	//按照地物和交点位置的顺序对交点排序
	qsort(arr.GetData(),arr.GetSize(),sizeof(IntersectItem),compare_intersect_item);

	IntersectItem *data = arr.GetData();
	nSum = arr.GetSize();

	//将重复的清除，
	for( i=1; i<arr.GetSize(); i++)
	{
		if( data[i].pFtr==data[i-1].pFtr && fabs(data[i].t-data[i-1].t)<1e-8 && data[i].ptIdx==data[i-1].ptIdx )
			data[i] = IntersectItem();
	}

	//刚好为连续顶点的，就取其头尾，使得它们能成为连续的一段
	int clear_start = 0, clear_stop = 0;
	for( i=1; i<arr.GetSize(); i++)
	{
		if( data[i].pFtr==NULL )
			continue;

		if( data[i].pFtr==data[i-1].pFtr && fabs(data[i].t)<1e-8 && fabs(data[i-1].t)<1e-8 && data[i].ptIdx==(data[i].ptIdx+1) )
			clear_stop++;
		else
		{
			if( clear_stop-clear_start>1 )
			{
				for( k=clear_start+1; k<clear_stop; k++)
				{
					data[k] = IntersectItem();
				}
			}

			clear_stop = clear_start = i;
		}
	}

	//将空交点删除
	for( i=arr.GetSize()-1; i>=0; i--)
	{
		if( arr[i].pFtr==NULL )
			arr.RemoveAt(i);
	}

	//对每个地物依次处理：用该地物的N个交点将地物打断成N+1份
	data = arr.GetData();
	nSum = arr.GetSize();

	CUndoFtrs undo(m_pEditor,Name());

	int startItem = 0, stopItem = -1;
	for( i=0; i<nSum; i++)
	{
		if( i!=(nSum-1) && data[i].pFtr==data[i+1].pFtr )
			continue;

		stopItem = i+1;

		CFeature *pFtr = data[startItem].pFtr;
		CGeometry *pGeo = pFtr->GetGeometry();
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);

		//处理start~stop之间的交点
		int ptIndex0 = 0, ptIndex1 = 0;
		for( j=startItem; j<stopItem; j++)
		{
			ptIndex1 = data[j].ptIdx;
			ptIndex1 = pShp->GetKeyPos(ptIndex1);

			//在 ptIndex1+1 处插入顶点，取 ptIndex0 ~ ptIndex1 之间的点构成新地物
			
			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);
			pts2.InsertAt(ptIndex1+1,PT_3DEX(data[j].pt,penLine));
			
			if( j>startItem )
			{
				pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			}

			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+2-ptIndex0);

			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pEditor->AddObject(pNewFtr,pDS->GetFtrLayerOfObject(pFtr)->GetID());
					GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNewFtr);
					undo.AddNewFeature(FtrToHandle(pNewFtr));
				}
				else
				{
					delete pNewFtr;
				}
			}

			ptIndex0 = ptIndex1;
		}

		//最后一段
		if( stopItem>startItem )
		{
			ptIndex1 = pts.GetSize()-1;

			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);			
			pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			
			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+1-ptIndex0);
			
			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pEditor->AddObject(pNewFtr,pDS->GetFtrLayerOfObject(pFtr)->GetID());
					GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNewFtr);
					undo.AddNewFeature(FtrToHandle(pNewFtr));
				}
				else
				{
					delete pNewFtr;
				}
			}
		}

		m_pEditor->DeleteObject(FtrToHandle(pFtr));
		undo.AddOldFeature(FtrToHandle(pFtr));

		startItem = stopItem;
	}

	pUndo->AddAction(&undo);

	return TRUE;
}


BOOL CTopoPreTreatCommand::CutLines(CFtrLayerArray& layers0, CUndoBatchAction* pUndo)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}

	//计算所有的交点，并将交点信息放入数组
	
	CDataSourceEx *pDS = m_pEditor->GetDataSource();

	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}

	ftrsArray.SetSize(j);

	IntersectItemArray arr;

	for( i=0; i<j; i++)
	{
		GProgressStep();
		
		Envelope e = ftrsArray[i]->GetGeometry()->GetEnvelope();
		e.Inflate(r,r,0);
		int num = pDQ->FindObjectInRect_SimpleAndSym(e,NULL,FALSE,FALSE);
		
		const CPFeature *ftrs = pDQ->GetFoundHandles(num);
		
		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()<=i )continue;
			GetIntersects(ftrsArray[i],ftrs[k],arr);
		}
/*
		for( k=i+1; k<j; k++)
		{
			GetIntersects(ftrsArray[i],ftrsArray[k],arr);
		}
*/
	}

	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}

	if( arr.GetSize()<=1 )
		return TRUE;

	//按照地物和交点位置的顺序对交点排序
	qsort(arr.GetData(),arr.GetSize(),sizeof(IntersectItem),compare_intersect_item);
	
	//将重复的清除，
	for( i=arr.GetSize()-1; i>0; i--)
	{
		if( arr[i].pFtr==arr[i-1].pFtr && fabs(arr[i].t-arr[i-1].t)<1e-8 && arr[i].ptIdx==arr[i-1].ptIdx  )
			arr.RemoveAt(i);
	}
	
	IntersectItem *data = arr.GetData();
	nSum = arr.GetSize();
/*	
	//刚好为连续顶点的，就取其头尾，使得它们能成为连续的一段
	int clear_start = 0, clear_stop = 0;
	for( i=1; i<nSum; i++)
	{		
		if( data[i].pFtr==data[i-1].pFtr && fabs(data[i].t)<1e-8 && fabs(data[i-1].t)<1e-8 && data[i].ptIdx==(data[i-1].ptIdx+1) )
		{
			clear_stop++;

			if( i!=(nSum-1) )
				continue;
		}
		
		if( clear_stop-clear_start>1 )
		{
			for( k=clear_start+1; k<clear_stop; k++)
			{
				data[k] = IntersectItem();
			}
		}
		
		clear_stop = clear_start = i;
	}
*/	
	//将空交点删除
	for( i=nSum-1; i>=0; i--)
	{
		if( arr[i].pFtr==NULL )
			arr.RemoveAt(i);
	}

	//对每个地物依次处理：用该地物的N个交点将地物打断成N+1份
	data = arr.GetData();
	nSum = arr.GetSize();

	CUndoFtrs undo(m_pEditor,Name());

	int startItem = 0, stopItem = -1;
	for( i=0; i<nSum; i++)
	{
		GProgressStep();

		if( i!=(nSum-1) && data[i].pFtr==data[i+1].pFtr )
			continue;

		stopItem = i+1;

		CFeature *pFtr = data[startItem].pFtr;
		CGeometry *pGeo = pFtr->GetGeometry();
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);

		//处理start~stop之间的交点
		int ptIndex0 = 0, ptIndex1 = 0;
		for( j=startItem; j<stopItem; j++)
		{
			ptIndex1 = data[j].ptIdx;
			ptIndex1 = pShp->GetKeyPos(ptIndex1);

			//在 ptIndex1+1 处插入顶点，取 ptIndex0 ~ ptIndex1 之间的点构成新地物
			
			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);
			pts2.InsertAt(ptIndex1+1,PT_3DEX(data[j].pt,penLine));
			
			if( j>startItem )
			{
				pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			}

			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+2-ptIndex0);

			if( num>=2 )
			{
				PT_3DEX pt0, pt1;
				pt0 = pts2[ptIndex0];
				pt1 = pts2[ptIndex0+num];
				if(GraphAPI::GIsEqual3DPoint(&pt0, &pt1))
				{
					CFeature *pNewFtr = pFtr->Clone();
					if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
					{
						m_pEditor->AddObject(pNewFtr,pDS->GetFtrLayerOfObject(pFtr)->GetID());
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNewFtr);
						undo.AddNewFeature(FtrToHandle(pNewFtr));
					}
					else
					{
						delete pNewFtr;
					}
				}
				else//面被打断，则变成线
				{
					CGeoCurve *pCurve = new CGeoCurve;
					pCurve->SetColor(pGeo->GetColor());
					if(pCurve->CreateShape(pts2.GetData()+ptIndex0,num))
					{
						if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
							pCurve->SetSymbolName("*");
						CFeature *pNewFtr = pFtr->Clone();
						pNewFtr->SetGeometry(pCurve);
						m_pEditor->AddObject(pNewFtr,pDS->GetFtrLayerOfObject(pFtr)->GetID());
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNewFtr);
						undo.AddNewFeature(FtrToHandle(pNewFtr));
					}
				}	
			}

			ptIndex0 = ptIndex1;
		}

		//最后一段
		if( stopItem>startItem )
		{
			ptIndex1 = pts.GetSize()-1;

			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);			
			pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			
			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+1-ptIndex0);
			
			if( num>=2 )
			{
				PT_3DEX pt0, pt1;
				pt0 = pts2[ptIndex0];
				pt1 = pts2[ptIndex0+num-1];
				if(GraphAPI::GIsEqual3DPoint(&pt0, &pt1))
				{
					CFeature *pNewFtr = pFtr->Clone();
					if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
					{
						m_pEditor->AddObject(pNewFtr,pDS->GetFtrLayerOfObject(pFtr)->GetID());	
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNewFtr);
						undo.AddNewFeature(FtrToHandle(pNewFtr));
					}
					else
					{
						delete pNewFtr;
					}
				}
				else//面被打断，则变成线
				{
					CGeoCurve *pCurve = new CGeoCurve;
					pCurve->SetColor(pGeo->GetColor());
					if(pCurve->CreateShape(pts2.GetData()+ptIndex0,num))
					{
						if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
							pCurve->SetSymbolName("*");
						CFeature *pNewFtr = pFtr->Clone();
						pNewFtr->SetGeometry(pCurve);
						m_pEditor->AddObject(pNewFtr,pDS->GetFtrLayerOfObject(pFtr)->GetID());
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNewFtr);
						undo.AddNewFeature(FtrToHandle(pNewFtr));
					}
				}
			}
		}

		m_pEditor->DeleteObject(FtrToHandle(pFtr));
		undo.AddOldFeature(FtrToHandle(pFtr));

		startItem = stopItem;
	}

	pUndo->AddAction(&undo);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CDealSelfIntersectionCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDealSelfIntersectionCommand, CEditCommand)

CDealSelfIntersectionCommand::CDealSelfIntersectionCommand()
{
	m_lfToler = GraphAPI::g_lfDisTolerance;
}

CDealSelfIntersectionCommand:: ~CDealSelfIntersectionCommand()
{

}

CCommand*  CDealSelfIntersectionCommand::Create()
{
	return new  CDealSelfIntersectionCommand();
}

void CDealSelfIntersectionCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("Layer0", &CVariantEx(var));

	var = m_lfToler;
	tab.AddValue("Toler", &CVariantEx(var));
}

void CDealSelfIntersectionCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("DealSelfIntersectionCommand", Name());

	param->AddLayerNameParamEx("Layer0", (LPCTSTR)m_HandleLayer, StrFromResID(IDS_CMDPLANE_HANDLELAYER));

	param->AddParam("Toler", (double)m_lfToler, StrFromResID(IDS_CMDPLANE_TOLER));
}

void CDealSelfIntersectionCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
   if (tab.GetValue(0, "Layer0", var))
	{
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimLeft();
		m_HandleLayer.TrimRight();
		SetSettingsModifyFlag();
	}

	if (tab.GetValue(0, "Toler", var))
	{
		m_lfToler = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

    CEditCommand::SetParams(tab, bInit);
}

void CDealSelfIntersectionCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

CString CDealSelfIntersectionCommand::Name()
{
	return StrFromResID(IDS_DEAL_SELF_INTERSECTION);
}

void CDealSelfIntersectionCommand::Start()
{
	if (!m_pEditor)return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CDealSelfIntersectionCommand::Abort()
{
	UpdateParams(TRUE);
	m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CDealSelfIntersectionCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor)return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if (!pDS) return;

	CFtrLayerArray layers;
	pDS->GetFtrLayersByNameOrCode_editable(m_HandleLayer, layers);

	int i = 0, nSum = 0;
	for (i = 0; i<layers.GetSize(); i++)
	{
		nSum += layers[i]->GetObjectCount();
	}

	GProgressStart(nSum);

	CUndoFtrs undo(m_pEditor, Name());
	
	for (i = 0; i < layers.GetSize(); i++)
	{
		int nObj = layers[i]->GetObjectCount();
		for (int j = 0; j < nObj; j++)
		{
			GProgressStep();
			CFeature *pFtr = layers[i]->GetObject(j);
			if (!pFtr)continue;
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				continue;
			CGeometry *pNew = ((CGeoCurveBase*)pGeo)->Linearize();
			if (!pNew) continue;
			//打断自相交线
			CGeoArray arrGeos;
			if (!CutLines_self(pNew, arrGeos))
			{
				delete pNew;
				continue;
			}
			//剔除公共边
			DeleteCommonLine(arrGeos);

			if (arrGeos.GetSize()<=0)
				continue;

			for (int k = 0; k < arrGeos.GetSize(); k++)
			{
				CFeature *pNew = pFtr->Clone();
				pNew->SetGeometry(arrGeos[k]);
				if (!pDS->AddObject(pNew, layers[i]->GetID()))
				{
					delete pNew;
					continue;
				}
				undo.AddNewFeature(FtrToHandle(pNew));
			}

			pDS->DeleteObject(pFtr);
			undo.AddOldFeature(FtrToHandle(pFtr));
		}
	}
	undo.Commit();
	GProgressEnd();
}

void CDealSelfIntersectionCommand::GetIntersects_self(CGeometry *pGeo1, IntersectItemArray &arr)
{
	if (!pGeo1) return;

	const CShapeLine *pShp1 = pGeo1->GetShape();
	if (!pShp1) return;

	CArray<PT_3DEX, PT_3DEX> pts;
	pShp1->GetPts(pts);
	KickoffSame2DPoints_notPenMove(pts);

	PT_3D pt1, pt2, line[4], ret;

	double toler = m_lfToler;

	int npt = pts.GetSize();
	PT_3DEX *buf = pts.GetData();
	for (int i = 0; i < npt - 2; i++)
	{
		if (buf[i + 1].pencode == penMove)
			continue;

		COPY_3DPT(line[0], buf[i]);
		COPY_3DPT(line[1], buf[i+1]);

		for (int j = i + 1; j < npt - 1; j++)
		{
			if (buf[j + 1].pencode == penMove)
				continue;

			COPY_3DPT(line[2], buf[j]);
			COPY_3DPT(line[3], buf[j + 1]);

			//为了优化，先判断一下
			double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;

			if (line[0].x < line[1].x){ xmin1 = line[0].x; xmax1 = line[1].x; }
			else { xmin1 = line[1].x; xmax1 = line[0].x; }
			if (line[0].y < line[1].y){ ymin1 = line[0].y; ymax1 = line[1].y; }
			else { ymin1 = line[1].y; ymax1 = line[0].y; }

			if (line[2].x < line[3].x){ xmin2 = line[2].x; xmax2 = line[3].x; }
			else { xmin2 = line[3].x; xmax2 = line[2].x; }
			if (line[2].y < line[3].y){ ymin2 = line[2].y; ymax2 = line[3].y; }
			else { ymin2 = line[3].y; ymax2 = line[2].y; }

			if (xmax1 < xmin2 - 1e-6 || xmax2 < xmin1 - 1e-6 ||
				ymax1 < ymin2 - 1e-6 || ymax2 < ymin1 - 1e-6)
				continue;

			if ((line[0].x - line[1].x)*(line[0].x - line[1].x) + (line[0].y - line[1].y)*(line[0].y - line[1].y) < 1e-8)
				continue;

			if ((line[2].x - line[3].x)*(line[2].x - line[3].x) + (line[2].y - line[3].y)*(line[2].y - line[3].y) < 1e-8)
				continue;

			double xt, yt;
			double t0, t1;
			if (j>i+1 && GraphAPI::GGetLineIntersectLineSeg(line[0].x, line[0].y, line[1].x, line[1].y, line[2].x, line[2].y, line[3].x, line[3].y, &xt, &yt, &t0, &t1))
			{
				IntersectItem item0, item1;
				item0.pGeo = pGeo1;
				item0.pt.x = xt;
				item0.pt.y = yt;
				item0.pt.z = line[0].z + t0 * (line[1].z - line[0].z);
				item0.t = t0;
				item0.ptIdx = i;

				if (fabs(1 - t0) < 1e-6 && (line[1].x - xt)*(line[1].x - xt) + (line[1].y - yt)*(line[1].y - yt) < 1e-8)
				{
					item0.ptIdx++;
					item0.t = 0;
				}

				item1.pGeo = pGeo1;
				item1.pt.x = xt;
				item1.pt.y = yt;
				item1.pt.z = line[2].z + t1 * (line[3].z - line[2].z);
				item1.t = t1;
				item1.ptIdx = j;

				if (fabs(1 - t1) < 1e-6 && (line[3].x - xt)*(line[3].x - xt) + (line[3].y - yt)*(line[3].y - yt) < 1e-8)
				{
					item1.ptIdx++;
					item1.t = 0;
				}

				arr.Add(item0);
				arr.Add(item1);
			}
			//重叠
			else if (GetOverlapSection_ret_tt(line[0].x, line[0].y, line[1].x, line[1].y, line[2].x, line[2].y, line[3].x, line[3].y, toler, &xt, &yt, &t0, &t1))
			{
				IntersectItem item0, item1;
				item0.pGeo = pGeo1;
				item0.pt.x = xt;
				item0.pt.y = yt;
				item0.pt.z = line[0].z + t0 * (line[1].z - line[0].z);
				item0.t = t0;
				item0.ptIdx = i;

				if (fabs(1 - t0) < 1e-6)
				{
					item0.ptIdx++;
					item0.t = 0;
				}

				item1.pGeo = pGeo1;
				item1.pt.x = xt;
				item1.pt.y = yt;
				item1.pt.z = line[2].z + t1 * (line[3].z - line[2].z);
				item1.t = t1;
				item1.ptIdx = j;

				if (fabs(1 - t1) < 1e-6)
				{
					item1.ptIdx++;
					item1.t = 0;
				}

				arr.Add(item0);
				arr.Add(item1);
			}
		}
	}
}

BOOL CDealSelfIntersectionCommand::CutLines_self(CGeometry *pGeo, CGeoArray& arrGeos)
{
	int i, j, k, nSum = 0;

	//计算所有的交点，并将交点信息放入数组
	double r = m_lfToler;

	IntersectItemArray arr;

	GetIntersects_self(pGeo, arr);

	if (arr.GetSize() <= 1)
		return false;

	//按照地物和交点位置的顺序对交点排序
	qsort(arr.GetData(), arr.GetSize(), sizeof(IntersectItem), compare_intersect_item);

	IntersectItem *data = arr.GetData();
	nSum = arr.GetSize();

	//将重复的清除，
	for (i = 1; i<arr.GetSize(); i++)
	{
		if (data[i].pGeo == data[i - 1].pGeo && fabs(data[i].t - data[i - 1].t)<1e-8 && data[i].ptIdx == data[i - 1].ptIdx)
			data[i] = IntersectItem();
	}

	//刚好为连续顶点的，就取其头尾，使得它们能成为连续的一段
	int clear_start = 0, clear_stop = 0;
	for (i = 1; i<arr.GetSize(); i++)
	{
		if (data[i].pGeo == NULL)
			continue;

		if (data[i].pGeo == data[i - 1].pGeo && fabs(data[i].t)<1e-8 && fabs(data[i - 1].t)<1e-8 && data[i].ptIdx == (data[i - 1].ptIdx + 1))
			clear_stop++;
		else
		{
			if (clear_stop - clear_start>1)
			{
				for (k = clear_start + 1; k<clear_stop; k++)
				{
					data[k] = IntersectItem();
				}
			}

			clear_stop = clear_start = i;
		}
	}

	//将空交点删除
	for (i = arr.GetSize() - 1; i >= 0; i--)
	{
		if (arr[i].pGeo == NULL)
			arr.RemoveAt(i);
	}

	//对每个地物依次处理：用该地物的N个交点将地物打断成N+1份
	data = arr.GetData();
	nSum = arr.GetSize();

	int startItem = 0, stopItem = -1;
	for (i = 0; i<nSum; i++)
	{
		if (i != (nSum - 1) && data[i].pGeo == data[i + 1].pGeo)
			continue;

		stopItem = i + 1;
		CGeometry *pGeo = data[startItem].pGeo;
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX, PT_3DEX> pts;
		pGeo->GetShape(pts);

		//处理start~stop之间的交点
		int ptIndex0 = 0, ptIndex1 = 0;
		for (j = startItem; j<stopItem; j++)
		{
			ptIndex1 = data[j].ptIdx;
			ptIndex1 = pShp->GetKeyPos (ptIndex1);

			//在 ptIndex1+1 处插入顶点，取 ptIndex0 ~ ptIndex1 之间的点构成新地物

			CArray<PT_3DEX, PT_3DEX> pts2;
			pts2.Copy(pts);
			pts2.InsertAt(ptIndex1 + 1, PT_3DEX(data[j].pt, penLine));

			if (j>startItem)
			{
				pts2[ptIndex0] = PT_3DEX(data[j - 1].pt, penLine);
			}

			int num = GraphAPI::GKickoffSamePoints(pts2.GetData() + ptIndex0, ptIndex1 + 2 - ptIndex0);

			if (num >= 2)
			{
				CGeometry *pNewGeo = pGeo->Clone();
				if (pNewGeo->CreateShape(pts2.GetData() + ptIndex0, num))
				{
					arrGeos.Add(pNewGeo);
				}
				else
				{
					delete pNewGeo;
				}
			}

			ptIndex0 = ptIndex1;
		}

		//最后一段
		if (stopItem>startItem)
		{
			ptIndex1 = pts.GetSize() - 1;

			CArray<PT_3DEX, PT_3DEX> pts2;
			pts2.Copy(pts);
			pts2[ptIndex0] = PT_3DEX(data[j - 1].pt, penLine);

			int num = GraphAPI::GKickoffSamePoints(pts2.GetData() + ptIndex0, ptIndex1 + 1 - ptIndex0);

			if (num >= 2)
			{
				CGeometry *pNewGeo = pGeo->Clone();
				if (pNewGeo->CreateShape(pts2.GetData() + ptIndex0, num))
				{
					arrGeos.Add(pNewGeo);
				}
				else
				{
					delete pNewGeo;
				}
			}
		}

		startItem = stopItem;
	}

	return TRUE;
}


BOOL CDealSelfIntersectionCommand::DeleteCommonLine(CGeoArray& arrGeos)
{
	//比较地物是否完全重叠，是的话，就删除其中一个
	for (int i = arrGeos.GetSize()-1; i >0; i--)
	{
		for (int j = 0; j < i; j++)
		{
			if (IsOverlapped(arrGeos[i], arrGeos[j]))
			{
				arrGeos.RemoveAt(i);
				break;
			}
		}
	}

	return TRUE;
}

BOOL CDealSelfIntersectionCommand::IsOverlapped(CGeometry *pGeo1, CGeometry *pGeo2)
{
	if (!pGeo1 || !pGeo2)
		return FALSE;

	if (!pGeo1->GetEnvelope().bIntersect(&pGeo2->GetEnvelope()))
		return FALSE;

	CArray<PT_3DEX, PT_3DEX> arrPts1, arrPts2;
	pGeo1->GetShape(arrPts1);
	pGeo2->GetShape(arrPts2);

	double toler = m_lfToler;
	toler = toler*toler;

	toler = 1e-8;

	if (arrPts1.GetSize() != arrPts2.GetSize())
		return FALSE;

	int npt = arrPts1.GetSize();

	for (int i = 0; i < npt; i++)
	{
		PT_3DEX pt1 = arrPts1[i];
		PT_3DEX pt2 = arrPts2[i];
		if ((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y) >= toler)
		{
			break;
		}
	}

	if (i < npt)
	{
		for (i = 0; i < npt; i++)
		{
			PT_3DEX pt1 = arrPts1[i];
			PT_3DEX pt2 = arrPts2[npt - 1 - i];
			if ((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y) >= toler)
			{
				break;
			}
		}

		if (i < npt)
			return FALSE;
	}

	return TRUE;
}

static void _FixT(double& t, double dis)
{
	double toler = 1e-4/dis;
	if(fabs(t)<toler)t = 0.0;
	if(fabs(1-t)<toler)t = 1.0;	
}

static void _Swap(double& a, double& b)
{
	double t = a;
	a = b;
	b = t;
}

static bool GGetLineIntersectLineSeg_withOverlap(double lfX0,double lfY0, double lfX1, double lfY1,double lfX2,double lfY2,double lfX3,double lfY3, 
												 double *plfX, double *plfY,double *plfT0, double *plfT1,
												 bool &twoIntersect,
												 double *plfX2, double *plfY2,double *plfT02, double *plfT12)
{
	twoIntersect = false;

	double vector1x = lfX1-lfX0, vector1y = lfY1-lfY0;
	double vector2x = lfX3-lfX2, vector2y = lfY3-lfY2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if ((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y))<=1e-15)
	{
		return false;
	}
	double unitdelta = delta/((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y)));
	if( unitdelta<GraphAPI::g_lfPraTolerance && unitdelta>-GraphAPI::g_lfPraTolerance )
	{	
		//虽然平行，但并不共线，而是有间距
		if(GraphAPI::GGetDisofPtToLine(lfX0,lfY0,lfX1,lfY1,lfX2,lfY2)>GraphAPI::g_lfDisTolerance )
			return false;
	
		double vector3x = lfX2-lfX0, vector3y = lfY2-lfY0;
		//共线的情况
		double av1x = fabs(vector1x), av1y = fabs(vector1y);
		double av2x = fabs(vector2x), av2y = fabs(vector2y);
		double av3x = fabs(vector3x), av3y = fabs(vector3y);

		//两对点都是重叠点
		if( av1x<GraphAPI::g_lfDisTolerance && av1y<GraphAPI::g_lfDisTolerance && av2x<GraphAPI::g_lfDisTolerance && av2y<GraphAPI::g_lfDisTolerance )
		{
			if( av3x<GraphAPI::g_lfDisTolerance && av3y<GraphAPI::g_lfDisTolerance )
			{
				if(plfX)*plfX = lfX0;
				if(plfY)*plfY = lfY0;
				if(plfT0)*plfT0 = 0;
				if(plfT1)*plfT1 = 0;

				return true;
			}
			
			return false;

		}
		//第一对点都是重叠点
		else if( av1x<GraphAPI::g_lfDisTolerance && av1y<GraphAPI::g_lfDisTolerance )
		{
			double t1, t2;
			if( av2x>av2y )
				t2 = -vector3x/vector2x;
			else
				t2 = -vector3y/vector2y;

			if( t2<0 || t2>1 )
				return false;

			t1 = 0;

			if(plfX)*plfX = lfX0;
			if(plfY)*plfY = lfY0;
			if(plfT0)*plfT0 = t1;
			if(plfT1)*plfT1 = t2;

			return true;
		}
		//第二对点都是重叠点
		else if( av2x<GraphAPI::g_lfDisTolerance && av2y<GraphAPI::g_lfDisTolerance )
		{
			double t1, t2;
			if( av1x>av1y )
				t1 = vector3x/vector1x;
			else
				t1 = vector3y/vector1y;
			
			if( t1<0 || t1>1 )
				return false;
			
			t2 = 0;
			
			if(plfX)*plfX = lfX2;
			if(plfY)*plfY = lfY2;

			if(plfT0)*plfT0 = t1;
			if(plfT1)*plfT1 = t2;

			return true;
		}
		//一般情况
		else
		{
			twoIntersect = true;

			//将两线段的方向调整，保持一致
			bool swapflag = false;
			if( av2x>av2y )
			{
				if( vector1x*vector2x<0 )
					swapflag = true;
			}
			else
			{
				if( vector1y*vector2y<0 )
					swapflag = true;
			}

			if(swapflag)
			{
				_Swap(lfX2,lfX3);
				_Swap(lfY2,lfY3);
				vector2x = -vector2x;
				vector2y = -vector2y;				
			}
			
			//先计算线段的端点在另一线段上的比例点ta,tb
			double ta1, tb1, ta2, tb2;
			if( av2x>av2y )
			{
				ta1 = (lfX0-lfX2)/vector2x;
				tb1 = (lfX1-lfX2)/vector2x;
			}
			else
			{
				ta1 = (lfY0-lfY2)/vector2y;
				tb1 = (lfY1-lfY2)/vector2y;
			}

			if( av1x>av1y )
			{
				ta2 = (lfX2-lfX0)/vector1x;
				tb2 = (lfX3-lfX0)/vector1x;
			}
			else
			{
				ta2 = (lfY2-lfY0)/vector1y;
				tb2 = (lfY3-lfY0)/vector1y;
			}

			_FixT(ta1,max(av2x,av2y));
			_FixT(tb1,max(av2x,av2y));
			_FixT(ta2,max(av1x,av1y));
			_FixT(tb2,max(av1x,av1y));

			//由于两个线段的方向一致，必定有ta1<tb1,ta2<tb2
			//根据ta,tb计算两个交点的坐标和参数，由于两线段共线，所以两个交点实际上是靠内的两个端点
			//共6种可能性
			if( (ta1<0 && tb1<0) || (ta1>1 && tb1>1) )
			{
				//线段共线不相交
				return false;
			}
			else if(ta1<=0 && tb1>=0 && tb1<=1 )
			{
				if(plfX)*plfX = lfX2;
				if(plfY)*plfY = lfY2;
				if(plfT0)*plfT0 = ta2;
				if(plfT1)*plfT1 = 0;

				if(plfX2)*plfX2 = lfX1;
				if(plfY2)*plfY2 = lfY1;
				if(plfT02)*plfT02 = 1;
				if(plfT12)*plfT12 = tb1;
			}
			else if(ta1<=0 && tb1>=1 )
			{
				if(plfX)*plfX = lfX2;
				if(plfY)*plfY = lfY2;
				if(plfT0)*plfT0 = ta2;
				if(plfT1)*plfT1 = 0;

				if(plfX2)*plfX2 = lfX3;
				if(plfY2)*plfY2 = lfY3;
				if(plfT02)*plfT02 = tb2;
				if(plfT12)*plfT12 = 1;
			}
			else if(ta1>=0 && ta1<=1 && tb1>=0 && tb1<=1 )
			{
				if(plfX)*plfX = lfX0;
				if(plfY)*plfY = lfY0;
				if(plfT0)*plfT0 = 0;
				if(plfT1)*plfT1 = ta1;

				if(plfX2)*plfX2 = lfX1;
				if(plfY2)*plfY2 = lfY1;
				if(plfT02)*plfT02 = 1;
				if(plfT12)*plfT12 = tb1;
			}
			else if(ta1>=0 && ta1<=1 && tb1>=1 )
			{
				if(plfX)*plfX = lfX0;
				if(plfY)*plfY = lfY0;
				if(plfT0)*plfT0 = 0;
				if(plfT1)*plfT1 = ta1;

				if(plfX2)*plfX2 = lfX3;
				if(plfY2)*plfY2 = lfY3;
				if(plfT02)*plfT02 = tb2;
				if(plfT12)*plfT12 = 1;
			}

			//前后端点衔接的情况，就认为只有一个交点
			if( (ta1<0 && tb1==0) || (ta1==1 && tb1>1) )
			{
				twoIntersect = false;
			}

			if(swapflag)
			{
				if(plfT1)*plfT1 = 1.0-*plfT1;
				if(plfT12)*plfT12 = 1.0-*plfT12;			
			}

			return true;
		}
	}

	double t1 = ( (lfX2-lfX0)*vector2y-(lfY2-lfY0)*vector2x )/delta;
	double t2 = ( (lfX2-lfX0)*vector1y-(lfY2-lfY0)*vector1x )/delta;
	double xr = lfX0 + t1*vector1x, yr = lfY0 + t1*vector1y;

	bool bSnapEndPoint = true;
	if( fabs(xr-lfX0)<=GraphAPI::g_lfDisTolerance && fabs(yr-lfY0)<=GraphAPI::g_lfDisTolerance )
	{
		t1 = 0;
		xr = lfX0; yr = lfY0;
	}
	else if( fabs(xr-lfX1)<=GraphAPI::g_lfDisTolerance && fabs(yr-lfY1)<=GraphAPI::g_lfDisTolerance )
	{
		t1 = 1;
		xr = lfX1; yr = lfY1;
	}
	else bSnapEndPoint = false;

	if( fabs(xr-lfX2)<=GraphAPI::g_lfDisTolerance && fabs(yr-lfY2)<=GraphAPI::g_lfDisTolerance )
	{
		t2 = 0;
		xr = lfX2; yr = lfY2;
	}
	else if( fabs(xr-lfX3)<=GraphAPI::g_lfDisTolerance && fabs(yr-lfY3)<=GraphAPI::g_lfDisTolerance )
	{
		t2 = 1;
		xr = lfX3; yr = lfY3;
	}
	else bSnapEndPoint = false;

	if(!bSnapEndPoint)
	{
		if( t1<0 || t1>1 || t2<0 || t2>1 )
		{
			return false;
		}
	}

	if(plfX)*plfX = xr;
	if(plfY)*plfY = yr;

	if(plfT0)*plfT0 = t1;
	if(plfT1)*plfT1 = t2;

	return true;
}

void CTopoPreTreatCommand::GetIntersects(CFeature* f1, CFeature* f2, IntersectItemArray &arr)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();

	const CShapeLine *pShp1 = pGeo1->GetShape();
	const CShapeLine *pShp2 = pGeo2->GetShape();

	if( !pShp1 || !pShp2 )
		return;

	PT_3D pt1,pt2,line[2],ret;
	double mindis = -1;
	
	int nIdx1=-1,nIdx2=-1;
	int nBaseIdx1 = 0, nBaseIdx2 = 0;
	const CShapeLine::ShapeLineUnit *pList1 = pShp1->HeadUnit();
	while (pList1!=NULL)
	{
		nIdx1++;

		Envelope e = pList1->evlp;
		e.m_xl -= 1e-6; e.m_xh += 1e-6;
		e.m_yl -= 1e-6; e.m_yh += 1e-6;

		nBaseIdx2 = 0;
		const CShapeLine::ShapeLineUnit *pList2 = pShp2->HeadUnit();
		nIdx2 = -1;
		while(pList2!=NULL)
		{
			nIdx2++;
			if(e.bIntersect(&(pList2->evlp)))
			{
				for (int i=0;i<pList1->nuse-1;i++)
				{
					if( pList1->pts[i+1].pencode==penMove )
						continue;

					COPY_3DPT(line[0],pList1->pts[i]);
					COPY_3DPT(line[1],pList1->pts[i+1]);
					
					Envelope e0 = pList2->evlp;
					e0.m_xl -= 1e-4; e0.m_xh += 1e-4;
					e0.m_yl -= 1e-4; e0.m_yh += 1e-4;
					if(!e0.bIntersect(line,line+1))continue;

					for (int j=0;j<pList2->nuse-1;j++)
					{
						if( pList2->pts[j+1].pencode==penMove )
							continue;

						//为了优化，先判断一下
						double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;
						
						if( line[0].x<line[1].x ){ xmin1 = line[0].x; xmax1 = line[1].x; }
						else { xmin1 = line[1].x; xmax1 = line[0].x; }
						if( line[0].y<line[1].y ){ ymin1 = line[0].y; ymax1 = line[1].y; }
						else { ymin1 = line[1].y; ymax1 = line[0].y; }
						
						if( pList2->pts[j].x<pList2->pts[j+1].x ){ xmin2 = pList2->pts[j].x; xmax2 = pList2->pts[j+1].x; }
						else { xmin2 = pList2->pts[j+1].x; xmax2 = pList2->pts[j].x; }
						if( pList2->pts[j].y<pList2->pts[j+1].y ){ ymin2 = pList2->pts[j].y; ymax2 = pList2->pts[j+1].y; }
						else { ymin2 = pList2->pts[j+1].y; ymax2 = pList2->pts[j].y; }
						
						if( xmax1<xmin2-1e-4 || xmax2<xmin1-1e-4 || 
							ymax1<ymin2-1e-4 || ymax2<ymin1-1e-4 )
							continue;

						double xt,yt,xt2,yt2;
						double t0=0,t1=0,t2=0,t3=0;
						bool twoIntersect = false;
						if(GGetLineIntersectLineSeg_withOverlap(line[0].x,line[0].y,line[1].x,line[1].y,pList2->pts[j].x,pList2->pts[j].y,pList2->pts[j+1].x,pList2->pts[j+1].y,
							&xt,&yt,&t0,&t1,
							twoIntersect,
							&xt2,&yt2,&t2,&t3))
						{	
							if(t0<-0.01 || t1<-0.01 || t2<-0.01||t3<-0.01)
							{
								//不应该得到这样的结果
								GGetLineIntersectLineSeg_withOverlap(line[0].x,line[0].y,line[1].x,line[1].y,pList2->pts[j].x,pList2->pts[j].y,pList2->pts[j+1].x,pList2->pts[j+1].y,
									&xt,&yt,&t0,&t1,
									twoIntersect,
									&xt2,&yt2,&t2,&t3);
								int a=1;	
							}
							IntersectItem item0,item1;
							item0.pFtr = f1;
							item0.pt.x = xt;
							item0.pt.y = yt;
							item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
							item0.t = t0;
							item0.ptIdx = nBaseIdx1 + i;

							if( (line[0].x-xt)*(line[0].x-xt)+(line[0].y-yt)*(line[0].y-yt)<1e-8 )
							{
								item0.t = 0;
							}

							if( (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
							{
								item0.ptIdx++;
								item0.t = 0;
							}

							item1.pFtr = f2;
							item1.pt.x = xt;
							item1.pt.y = yt;
							item1.pt.z = pList2->pts[j].z + t1 * (pList2->pts[j+1].z-pList2->pts[j].z);
							item1.t = t1;
							item1.ptIdx = nBaseIdx2 + j;

							if( (pList2->pts[j].x-xt)*(pList2->pts[j].x-xt)+(pList2->pts[j].y-yt)*(pList2->pts[j].y-yt)<1e-8 )
							{
								item1.t = 0;
							}

							if( (pList2->pts[j+1].x-xt)*(pList2->pts[j+1].x-xt)+(pList2->pts[j+1].y-yt)*(pList2->pts[j+1].y-yt)<1e-8 )
							{
								item1.ptIdx++;
								item1.t = 0;
							}

							arr.Add(item0);
							arr.Add(item1);

							if(twoIntersect)
							{
								item0 = IntersectItem();
								item1 = IntersectItem();
								xt = xt2; yt = yt2;
								t0 = t2; t1 = t3;
								
								item0.pFtr = f1;
								item0.pt.x = xt;
								item0.pt.y = yt;
								item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
								item0.t = t0;
								item0.ptIdx = nBaseIdx1 + i;

								if( (line[0].x-xt)*(line[0].x-xt)+(line[0].y-yt)*(line[0].y-yt)<1e-8 )
								{
									item0.t = 0;
								}

								if( (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
								{
									item0.ptIdx++;
									item0.t = 0;
								}

								item1.pFtr = f2;
								item1.pt.x = xt;
								item1.pt.y = yt;
								item1.pt.z = pList2->pts[j].z + t1 * (pList2->pts[j+1].z-pList2->pts[j].z);
								item1.t = t1;
								item1.ptIdx = nBaseIdx2 + j;

								if( (pList2->pts[j].x-xt)*(pList2->pts[j].x-xt)+(pList2->pts[j].y-yt)*(pList2->pts[j].y-yt)<1e-8 )
								{
									item1.t = 0;
								}

								if( (pList2->pts[j+1].x-xt)*(pList2->pts[j+1].x-xt)+(pList2->pts[j+1].y-yt)*(pList2->pts[j+1].y-yt)<1e-8 )
								{
									item1.ptIdx++;
									item1.t = 0;
								}

								arr.Add(item0);
								arr.Add(item1);
							}
						}
					}
				}				
			}
			nBaseIdx2 += pList2->nuse;
			pList2 = pList2->next;
		}		

		nBaseIdx1 += pList1->nuse;
		pList1 = pList1->next;
	}	
}

void CTopoPreTreatCommand::GetIntersects_self(CFeature* f1, IntersectItemArray &arr)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	
	const CShapeLine *pShp1 = pGeo1->GetShape();
	
	if( !pShp1 )
		return;

	BOOL bClosed = FALSE;
	if( pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
	{
		if( ((CGeoCurveBase*)pGeo1)->IsClosed() )
		{
			bClosed = TRUE;
		}
	}

	CArray<PT_3DEX,PT_3DEX> pts;
	pShp1->GetPts(pts);

	KickoffSame2DPoints_notPenMove(pts);

	PT_3D pt1,pt2,line[4],ret;

	int npt = pts.GetSize();
	PT_3DEX *buf = pts.GetData();
	for( int i=0; i<npt-1; i++)
	{
		if( buf[i+1].pencode==penMove )
			continue;

		for( int j=i+2; j<npt-1; j++)
		{
			if( buf[j+1].pencode==penMove )
				continue;

			if( bClosed && i==0 && j==(npt-2) )
				continue;

			COPY_3DPT(line[0],buf[i]);
			COPY_3DPT(line[1],buf[i+1]);
			
			COPY_3DPT(line[2],buf[j]);
			COPY_3DPT(line[3],buf[j+1]);

			//为了优化，先判断一下
			double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;
			
			if( line[0].x<line[1].x ){ xmin1 = line[0].x; xmax1 = line[1].x; }
			else { xmin1 = line[1].x; xmax1 = line[0].x; }
			if( line[0].y<line[1].y ){ ymin1 = line[0].y; ymax1 = line[1].y; }
			else { ymin1 = line[1].y; ymax1 = line[0].y; }

			if( line[2].x<line[3].x ){ xmin2 = line[2].x; xmax2 = line[3].x; }
			else { xmin2 = line[3].x; xmax2 = line[2].x; }
			if( line[2].y<line[3].y ){ ymin2 = line[2].y; ymax2 = line[3].y; }
			else { ymin2 = line[3].y; ymax2 = line[2].y; }

			if( xmax1<xmin2-1e-6 || xmax2<xmin1-1e-6 || 
				ymax1<ymin2-1e-6 || ymax2<ymin1-1e-6 )
				continue;

			if( (line[0].x-line[1].x)*(line[0].x-line[1].x) + (line[0].y-line[1].y)*(line[0].y-line[1].y)<1e-8 )
				continue;

			if( (line[2].x-line[3].x)*(line[2].x-line[3].x) + (line[2].y-line[3].y)*(line[2].y-line[3].y)<1e-8 )
				continue;

			double xt,yt;
			double t0,t1;
			if(GraphAPI::GGetLineIntersectLineSeg(line[0].x,line[0].y,line[1].x,line[1].y,line[2].x,line[2].y,line[3].x,line[3].y,&xt,&yt,&t0,&t1))
			{	
				IntersectItem item0,item1;
				item0.pFtr = f1;
				item0.pt.x = xt;
				item0.pt.y = yt;
				item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
				item0.t = t0;
				item0.ptIdx = i;

				if( fabs(1-t0)<1e-6 && (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
				{
					item0.ptIdx++;
					item0.t = 0;
				}
				
				item1.pFtr = f1;
				item1.pt.x = xt;
				item1.pt.y = yt;
				item1.pt.z = line[2].z + t1 * (line[3].z-line[2].z);
				item1.t = t1;
				item1.ptIdx = j;
				
				if( fabs(1-t1)<1e-6 && (line[3].x-xt)*(line[3].x-xt)+(line[3].y-yt)*(line[3].y-yt)<1e-8 )
				{
					item1.ptIdx++;
					item1.t = 0;
				}
				
				arr.Add(item0);
				arr.Add(item1);
			}
			//重叠
			else if( GetOverlapSection_ret_tt(line[0].x,line[0].y,line[1].x,line[1].y,line[2].x,line[2].y,line[3].x,line[3].y,GraphAPI::g_lfDisTolerance,&xt,&yt,&t0,&t1) )
			{
				IntersectItem item0,item1;
				item0.pFtr = f1;
				item0.pt.x = xt;
				item0.pt.y = yt;
				item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
				item0.t = t0;
				item0.ptIdx = i;

				if( fabs(1-t0)<1e-6 && (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
				{
					item0.ptIdx++;
					item0.t = 0;
				}
				
				item1.pFtr = f1;
				item1.pt.x = xt;
				item1.pt.y = yt;
				item1.pt.z = line[2].z + t1 * (line[3].z-line[2].z);
				item1.t = t1;
				item1.ptIdx = j;
				
				if( fabs(1-t1)<1e-6 && (line[3].x-xt)*(line[3].x-xt)+(line[3].y-yt)*(line[3].y-yt)<1e-8 )
				{
					item1.ptIdx++;
					item1.t = 0;
				}
				
				arr.Add(item0);
				arr.Add(item1);				
			}
		}
	}	
}


void CTopoPreTreatCommand::GetSnapVertexes(CFeature* f1, CFeature* f2, Envelope evlp, SnapItemArray &arr)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();

	if( !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) || !pGeo2->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		return;
	
	const CShapeLine *pShp1 = pGeo1->GetShape();
	const CShapeLine *pShp2 = pGeo2->GetShape();

	if( !pShp1 || !pShp2 )
		return;

	double toler = m_lfToler;

	Envelope e1 = pGeo1->GetEnvelope();
	e1.Inflate(toler,toler,0);

	BOOL bUseEvlp = (!evlp.IsEmpty());
	double toler2 = toler * toler;
	PT_3DEX pt1,pt2;

	if( !e1.bIntersect(&pGeo2->GetEnvelope()) )
		return;

	if( f1==f2 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeo1->GetShape(arrPts);

		int npt = arrPts.GetSize();
		for( int i=0; i<npt; i++)
		{
			pt1 = arrPts[i];
			if( bUseEvlp && !evlp.bPtIn(&pt1) )
				continue;

			for( int j=i+2; j<npt; j++)
			{
				pt2 = arrPts[j];
				if( bUseEvlp && !evlp.bPtIn(&pt2) )
					continue;

				double dis = (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y);
				
				if( dis<toler2 && dis>0 )
				{
					SnapItem item0;
					item0.pFtr1 = f1;
					item0.pFtr2 = f2;
					item0.pt1 = pt1;
					item0.pt2 = pt2;

					CArray<int,int> arrIndex;
					pShp1->GetKeyPosOfBaseLines(arrIndex);

					item0.ptIdx1 = arrIndex[i];
					item0.ptIdx2 = arrIndex[j];
					/*
					arr.Add(item0);

					item0.pFtr1 = f1;
					item0.pFtr2 = f2;
					item0.pt1 = pt2;
					item0.pt2 = pt1;
					item0.ptIdx1 = arrIndex[j];
					item0.ptIdx2 = arrIndex[i];
					*/
					arr.Add(item0);
				}
			}
		}
		return;
	}
		
	
	PT_3D line[2],ret;
	double mindis = -1;
	
	int nIdx1=-1,nIdx2=-1;
	int nBaseIdx1 = 0, nBaseIdx2 = 0;
	const CShapeLine::ShapeLineUnit *pList1 = pShp1->HeadUnit();
	while (pList1!=NULL)
	{
		nIdx1++;
		const CShapeLine::ShapeLineUnit *pList2 = pShp2->HeadUnit();
		nIdx2 = -1;
		nBaseIdx2 = 0;

		Envelope e = pList1->evlp;
		e.m_xl -= toler; e.m_xh += toler;
		e.m_yl -= toler; e.m_yh += toler;

		if( bUseEvlp && !e.bIntersect(&evlp) )
		{
			nBaseIdx1 += pList1->nuse;
			pList1 = pList1->next;	
			continue;
		}

		while(pList2!=NULL)
		{
			nIdx1++;

			if( e.bIntersect(&(pList2->evlp)) )
			{
				for (int i=0;i<pList1->nuse;i++)
				{
					pt1 = pList1->pts[i];
					if( pt1.pencode==penNone )
						continue;
					
					for (int j=0;j<pList2->nuse;j++)
					{						
						pt2 = pList2->pts[j];

						if( pt2.pencode==penNone )
							continue;

						double dis = (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y);

						if( dis<toler2 && dis>0 )
						{	
							if( !bUseEvlp || (evlp.bPtIn(&pt1) && evlp.bPtIn(&pt2)) )
							{
								SnapItem item0;
								item0.pFtr1 = f1;
								item0.pFtr2 = f2;
								item0.pt1 = pt1;
								item0.pt2 = pt2;
								item0.ptIdx1 = nBaseIdx1 + i;
								item0.ptIdx2 = nBaseIdx2 + j;
								
								arr.Add(item0);
							}
						}
					}
				}				
			}

			nBaseIdx2 += pList2->nuse;
			pList2 = pList2->next;
		}	
		
		nBaseIdx1 += pList1->nuse;
		pList1 = pList1->next;		
	}	
}


BOOL CTopoPreTreatCommand::SnapVertexes(CFtrLayerArray& layers0, CUndoBatchAction* pUndo)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
	
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}

	ftrsArray.SetSize(j);

	BOOL bMsgbox = FALSE;
	
	//计算所有的匹配点（咬合点），并将匹配点信息放入数组
	SnapItemArray arr;

	for( i=0; i<j; i++)
	{
		GProgressStep();

		Envelope e = ftrsArray[i]->GetGeometry()->GetEnvelope();
		e.Inflate(r,r,0);

		int m, num = 0;
		pDQ->FindObjectInRect_SimpleAndSym(e, NULL, FALSE, FALSE);

		const CPFeature *ftrs = pDQ->GetFoundHandles(num);
		
		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()<=i )continue;

			SnapItemArray arr1; 
			
			GetSnapVertexes(ftrsArray[i],ftrs[k],Envelope(),arr1);

			if( arr1.GetSize()>100 )
			{
				continue;//不处理大于100的
				if( !bMsgbox )
				{
					bMsgbox = TRUE;
					if( AfxMessageBox(IDS_ERROR_MUCHTIME,MB_YESNO)!=IDYES )
					{
						goto EXIT;
					}
				}
			}
/*
			//去掉串联的部分（A->B,B->C），避免一个地物的某个节点被多次修改
			for( m=arr.GetSize()-1; m>=0; m--)
			{
				SnapItem item0 = arr[m];
				for( int n=arr1.GetSize()-1; n>=0; n--)
				{
					SnapItem item1 = arr1[n];
					if( (item0.pFtr2==item1.pFtr1 && item0.ptIdx2==item1.ptIdx1) ||
						(item0.pFtr2==item1.pFtr2 && item0.ptIdx2==item1.ptIdx2) )
					{
						arr1.RemoveAt(n);
					}
				}				
			}

			//去掉两个节点同时连接某个节点的情况（A->C,B->C）
			for( m=arr1.GetSize()-1; m>=0; m--)
			{
				SnapItem item0 = arr1[m];
				for( int n=m-1; n>=0; n--)
				{
					SnapItem item1 = arr1[n];
					if( item0.pFtr2==item1.pFtr2 && item0.ptIdx2==item1.ptIdx2 )
					{
						arr1.RemoveAt(m);
						break;
					}
				}				
			}
*/
			arr.Append(arr1);
		}
	}

EXIT:
	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}

	if( arr.GetSize()<=0 )
		return TRUE;

	//按照地物和咬合点位置的顺序对交点排序
	qsort(arr.GetData(),arr.GetSize(),sizeof(SnapItem),compare_snap_item);

	//对每个地物依次处理：将该地物的N个重叠点调整位置
	SnapItem *data = arr.GetData();
	nSum = arr.GetSize();

	int startItem = 0, stopItem = -1;
	for( i=0; i<nSum; i++)
	{
		if( i!=(nSum-1) && data[i].pFtr1==data[i+1].pFtr1 && data[i].ptIdx1==data[i+1].ptIdx1 )
			continue;

		stopItem = i+1;

		CGeometry *pGeo = data[startItem].pFtr1->GetGeometry();
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX,PT_3DEX> pts;
		pShp->GetPts(pts);

		//处理start~stop之间的重叠点
		//计算平均值，并计算各个重叠点所对应的地物的顶点序号，便于下一步修改
		double ax = data[startItem].pt1.x, ay = data[startItem].pt1.y;
		CArray<int,int> arrIndex;

		for( j=startItem; j<stopItem; j++)
		{
			CGeometry *pGeo2 = data[j].pFtr2->GetGeometry();
			const CShapeLine *pShp2 = pGeo2->GetShape();

			arrIndex.Add(pShp2->GetKeyPos(data[j].ptIdx2));

			ax += data[j].pt2.x;
			ay += data[j].pt2.y;
		}

		ax /= (stopItem-startItem+1);
		ay /= (stopItem-startItem+1);

		arrIndex.Add(pShp->GetKeyPos(data[startItem].ptIdx1));

		//修改地物
		for( j=startItem; j<stopItem+1; j++)
		{
			int ptIndex = arrIndex[j-startItem];
			CFeature* pFtr = ((j==stopItem)?data[startItem].pFtr1:data[j].pFtr2);

			CUndoVertex undo(m_pEditor,Name());			
			m_pEditor->DeleteObject(FtrToHandle(pFtr));

			CGeometry *pGeo2 = pFtr->GetGeometry();
			PT_3DEX expt = pGeo2->GetDataPoint(ptIndex);
			undo.ptOld = expt;
			expt.x = ax;
			expt.y = ay;
			pGeo2->SetDataPoint(ptIndex,expt);

			undo.ptNew = expt;
			undo.bRepeat = FALSE;
			undo.nPtType = PT_KEYCTRL::typeKey;
			undo.nPtIdx = ptIndex;
			undo.handle = FtrToHandle(pFtr);

			m_pEditor->RestoreObject(FtrToHandle(pFtr));

			pUndo->AddAction(&undo);
		}

		startItem = stopItem;
	}

	return TRUE;
}


BOOL CTopoPreTreatCommand::IsOverlapped(CPFeature f1, CPFeature f2)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();

	if( !pGeo1->GetEnvelope().bIntersect(&pGeo2->GetEnvelope()) )
		return FALSE;
		
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	pGeo1->GetShape(arrPts1);
	pGeo2->GetShape(arrPts2);

	double toler = m_lfToler;
	toler = toler*toler;

	toler = 1e-8;

	if( arrPts1.GetSize()!=arrPts2.GetSize() )
		return FALSE;

	int npt = arrPts1.GetSize();

	for( int i=0; i<npt; i++)
	{
		PT_3DEX pt1 = arrPts1[i];
		PT_3DEX pt2 = arrPts2[i];
		if( (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y)>=toler )
		{
			break;
		}
	}

	if( i<npt )
	{
		for( i=0; i<npt; i++)
		{
			PT_3DEX pt1 = arrPts1[i];
			PT_3DEX pt2 = arrPts2[npt-1-i];
			if( (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y)>=toler )
			{
				break;
			}
		}

		if( i<npt )
			return FALSE;
	}

	return TRUE;
}


BOOL CTopoPreTreatCommand::DeleteCommonLine(CFtrLayerArray& layers0, CUndoBatchAction* pUndo)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
	
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}

	ftrsArray.SetSize(j);

	//比较地物是否完全重叠，是的话，就删除其中一个
	SnapItemArray arr;

	CUndoFtrs undo(m_pEditor,Name());

	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();

	for( i=j-1; i>=0; i--)
	{
		GProgressStep();

		CArray<PT_3DEX,PT_3DEX> arrPts;
		ftrsArray[i]->GetGeometry()->GetShape(arrPts);

		Envelope e;
		e.CreateFromPtAndRadius(arrPts[0],GraphAPI::g_lfDisTolerance);
		int num = pDQ->FindObjectInRect_SimpleAndSym(e,NULL,FALSE,FALSE);
		
		const CPFeature *ftrs = pDQ->GetFoundHandles(num);

		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()>=i )continue;

			if( IsOverlapped(ftrsArray[i],ftrs[k]) )
			{
				m_pEditor->DeleteObject(FtrToHandle(ftrsArray[i]));
				undo.AddOldFeature(FtrToHandle(ftrsArray[i]));
				break;
			}
		}
	}

	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}

	if(undo.arrOldHandles.GetSize()>0)
		pUndo->AddAction(&undo);

	return TRUE;
}


double CTopoPreTreatCommand::CalcSnapDistance(CPFeature f1, CPFeature f2, int nPtIndex)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return -1;
	
	CGeometry *pObj = pFtr->GetGeometry();

	CArray<PT_3DEX,PT_3DEX> arrPts0;
	pObj->GetShape(arrPts0);
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = arrPts0[1];
		expt1 = arrPts0[0];
	}
	else
	{
		nIndex = arrPts0.GetSize()-1;
		expt0 = arrPts0[nIndex-1];
		expt1 = arrPts0[nIndex];
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;

	CGeometry *pObj2 = f2->GetGeometry();
	PT_KEYCTRL nearestPt = pObj2->FindNearestKeyCtrlPt(expt1,r,NULL,1);

	PT_3DEX newPt;
	if( nearestPt.IsValid() )
	{
		newPt = pObj2->GetDataPoint(nearestPt.index);
	}
	else
	{
		Envelope e;
		e.CreateFromPtAndRadius(expt1,r);
		if( !pObj2->FindNearestBasePt(expt1,e,NULL,&newPt,NULL) )
			return -1;
	}
	
	return GraphAPI::GGet2DDisOf2P(expt1,newPt);
}

void CTopoPreTreatCommand::ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex, CUndoBatchAction* pUndo)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return;
	
	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj )return;
	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )return;
	if( ((CGeoCurveBase*)pObj)->IsClosed() )return;

	CArray<PT_3DEX,PT_3DEX> arrPts0;
	pObj->GetShape(arrPts0);
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = arrPts0[1];
		expt1 = arrPts0[0];
	}
	else
	{
		nIndex = arrPts0.GetSize()-1;
		expt0 = arrPts0[nIndex-1];
		expt1 = arrPts0[nIndex];
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;

	CGeometry *pObj2 = f2->GetGeometry();
	PT_KEYCTRL nearestPt = pObj2->FindNearestKeyCtrlPt(expt1,r,NULL,1);

	BOOL bSnapVertex = FALSE;
	PT_3DEX newPt = expt1;
	int nIndex2 = -1;
	if( nearestPt.IsValid() )
	{
		newPt = pObj2->GetDataPoint(nearestPt.index);
		nIndex2 = nearestPt.index;
		bSnapVertex = TRUE;
	}
	else
	{
		Envelope e;
		e.CreateFromPtAndRadius(expt1,r);
		if( !pObj2->FindNearestBasePt(expt1,e,NULL,&newPt,NULL) )
			return;

		const CShapeLine *pShp = pObj2->GetShape();
		if( !pShp )
			return;

		nIndex2 = pShp->FindNearestKeyPt(newPt);
	}
	
	m_pEditor->DeleteObject(FtrToHandle(pFtr),FALSE);
		
	CUndoVertex undo(m_pEditor,Name());

	undo.handle = FtrToHandle(f1);
	undo.ptOld = expt1;
	undo.ptNew = newPt;
	undo.nPtType = PT_KEYCTRL::typeKey;
	undo.nPtIdx = nIndex;

	if( !m_bSnap3D )
	{
		undo.ptNew.z = expt1.z;
	}
	
	pFtr->GetGeometry()->SetDataPoint(nPtIndex,undo.ptNew);		
	m_pEditor->RestoreObject(FtrToHandle(pFtr));

	pUndo->AddAction(&undo);

	if( m_bAddPt && !bSnapVertex )
	{
		m_pEditor->DeleteObject(FtrToHandle(f2),FALSE);

		CUndoVertex undo1(m_pEditor,Name());

		undo1.handle = FtrToHandle(f2);
		undo1.bRepeat = FALSE;
		undo1.nPtType = PT_KEYCTRL::typeKey;
		undo1.nPtIdx = nIndex2+1;
		undo1.ptNew = newPt;
		undo1.ptOld.pencode = penNone;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		f2->GetGeometry()->GetShape(arrPts);
		arrPts.InsertAt(undo1.nPtIdx,undo1.ptNew);

		f2->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		m_pEditor->RestoreObject(FtrToHandle(f2));
	}
}

extern int GetCurveIntersectCurve(PT_3DEX *pts1, int num1, PT_3DEX *pts2, int num2, CArray<PtIntersect,PtIntersect> &arrIntersectPts);
//获取面域点，ftrs是和该地物相交的所有闭合地物
static BOOL GetSurfacePoint(PT_3D &retpt, CFtrArray& ftrs, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	BOOL bOK = FALSE;
	PT_3DEX line[2];//对角线
	int pos = 0;
	int nSize = arrPts.GetSize();
	if(nSize<4)
	{
		return FALSE;
	}
	else if(nSize==4)
	{
		line[0] = arrPts[0];
		line[1].x = (arrPts[1].x+arrPts[2].x)/2;
		line[1].y = (arrPts[1].y+arrPts[2].y)/2;
		line[1].z = (arrPts[1].z+arrPts[2].z)/2;
	}
	else if(nSize>4)
	{
		line[0] = arrPts[0];
		line[1] = arrPts[nSize/2];
	}

	int i,j;

LOOP1:
	CArray<PtIntersect,PtIntersect> arr;
	for(i=0; i<ftrs.GetSize(); i++)
	{
		CArray<PT_3DEX,PT_3DEX> pts;
		ftrs[i]->GetGeometry()->GetShape()->GetPts(pts);
		GetCurveIntersectCurve(line, 2, pts.GetData(), pts.GetSize(), arr);
	}

	double t=0;

	for(i=0; i<=arr.GetSize(); i++)
	{
		double temp, len1=0.0, len2;//temp：两个交点间线段的终点位置， len2：两个交点间线段的比例，len1：所有len2的最大值
		if(i==arr.GetSize())
		{
			if(i==0)
			{
				temp = 0.5;
				len2 = 1;
			}
			else
			{
				temp = (1+arr[i-1].lfRatio)/2;
				len2 = 1-arr[i-1].lfRatio;
			}
		}
		else
		{
			temp = (arr[i].lfRatio+t)/2;
			len2 = arr[i].lfRatio-t;
		}

		PT_3DEX pt1;
		pt1.x = line[0].x + (line[1].x-line[0].x)*temp;
		pt1.y = line[0].y + (line[1].y-line[0].y)*temp;
		pt1.z = line[0].z + (line[1].z-line[0].z)*temp;

		if(2==GraphAPI::GIsPtInRegion(pt1, arrPts.GetData(), nSize))
		{
			bOK = TRUE;
		}

		if(bOK)
		{
			for(j=0; j<ftrs.GetSize(); j++)
			{
				CArray<PT_3DEX,PT_3DEX> pts;
				ftrs[j]->GetGeometry()->GetShape()->GetPts(pts);
				if(GraphAPI::GIsPtInRegion(pt1, pts.GetData(), pts.GetSize()) >=1)
				{
					bOK = FALSE;
					break;
				}
			}
		}

		if(bOK && len2>len1)
		{
			COPY_3DPT(retpt, pt1);
			len1 = len2;
		}
		
		if(i<arr.GetSize())
			t = arr[i].lfRatio;
	}

	if(bOK)
	{
		return bOK;
	}
	
	//bOK == FALSE
	if(nSize>4)
	{
		pos++;
		if(pos>(nSize/2) )
			return FALSE;
		line[0] = arrPts[pos];
		line[1] = arrPts[nSize/2+pos];
		goto LOOP1;
	}

	
	return TRUE;
}

void CTopoPreTreatCommand::CreateSurfacePoints(CFtrLayerArray& layers0, CUndoBatchAction* pUndo)
{
	int i, j, k;

	CDataSourceEx *pDS = m_pEditor->GetDataSource();

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if( !pScheme )
		return;

	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();

	CUndoFtrs undo(m_pEditor,Name());
	
	//创建面域点
	for( i=0; i<layers0.GetSize(); i++)
	{
		int nObj = layers0[i]->GetObjectCount();

		CSchemeLayerDefine *pDefLayer = pScheme->GetLayerDefine( layers0[i]->GetName() );
		if( !pDefLayer )
			continue;

		if( pDefLayer->GetDbGeoClass()!=CLS_GEOSURFACE )
			continue;

		for( j=0; j<nObj; j++)
		{
			CFeature *pFtr = layers0[i]->GetObject(j);
			if( !pFtr )continue;

			GProgressStep();
			
			CGeometry *pGeo = pFtr->GetGeometry();
			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && ((CGeoCurveBase*)pGeo)->IsClosed() && pGeo->GetDataPointSum()>=4 )
			{
				//该对象内部没有面域点才创建
				Envelope e = pGeo->GetEnvelope();
				pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);

				BOOL bFindOK = FALSE;

				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);

				int num1 = 0;
				const CPFeature *ftrs = m_pEditor->GetDataQuery()->GetFoundHandles(num1);	
				for(k=0; k<num1; k++)
				{
					if(pFtr==ftrs[k]) continue;
					if(ftrs[k]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)) )
					{
						PT_3DEX expt = ftrs[k]->GetGeometry()->GetDataPoint(0);
						if( GraphAPI::GIsPtInRegion(expt,arrPts.GetData(),arrPts.GetSize())==2 )
						{
							bFindOK = TRUE;
							//如果该面域点在内部的某个闭合地物内，则bFindOK = FALSE
							for(int n=0; n<num1; n++)
							{
								CGeometry* pObj1 = ftrs[n]->GetGeometry();
								if( pObj1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && ((CGeoCurveBase*)pObj1)->IsClosed() )
								{
									CArray<PT_3DEX,PT_3DEX> arrPts1;
									pObj1->GetShape(arrPts1);
									if( GraphAPI::GIsPtInRegion(expt,arrPts1.GetData(),arrPts1.GetSize())==2 )
									{
										bFindOK = FALSE;
										break;
									}
								}
							}
						}

						if(bFindOK)
						{
							break;
						}
					}
				}

				if(bFindOK)
					continue;

				CFtrArray arrftrs;

				for(k=0; k<num1; k++)
				{
					if(pFtr==ftrs[k]) continue;
					CGeometry* pObj = ftrs[k]->GetGeometry();
					if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && ((CGeoCurveBase*)pObj)->IsClosed() )
					{
						arrftrs.Add(ftrs[k]);
					}
				}
				
				PT_3D cpt;
				pGeo->GetCenter(NULL,&cpt);
				PT_3DEX pt1(cpt, penNone);
				BOOL bOK = FALSE;
				if(2==GraphAPI::GIsPtInRegion(pt1, arrPts.GetData(), arrPts.GetSize()))
				{
					bOK = TRUE;
				}
				if(bOK)
				{
					for(k=0; k<arrftrs.GetSize(); k++)
					{
						CArray<PT_3DEX,PT_3DEX> pts;
						arrftrs[k]->GetGeometry()->GetShape()->GetPts(pts);
						if(GraphAPI::GIsPtInRegion(pt1, pts.GetData(), pts.GetSize()) >=1)
						{
							bOK = FALSE;
							break;
						}
					}
				}
				if(!bOK)//如果获取的点不满足要求，则重新获取
				{
					if(!GetSurfacePoint(cpt, arrftrs, arrPts))
						continue;
				}

				CFeature *pNewFtr = layers0[i]->CreateDefaultFeature(pDS->GetScale(),CLS_GEOSURFACEPOINT);
				if(pNewFtr)				
				{
					pNewFtr->GetGeometry()->CreateShape(&PT_3DEX(cpt,penLine),1);

					m_pEditor->AddObject(pNewFtr,layers0[i]->GetID());
					
					undo.AddNewFeature(FtrToHandle(pNewFtr));

					GETXDS(m_pEditor)->CopyXAttributes(pFtr, pNewFtr);
					
				}
			}
		}
	}

	if(undo.arrNewHandles.GetSize()>0)
		pUndo->AddAction(&undo);
}



// CMatchVertexsCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMatchVertexsCommand,CTopoPreTreatCommand)

CMatchVertexsCommand::CMatchVertexsCommand()
{
	m_lfToler = 0.2;
	strcat(m_strRegPath,"\\MatchVertexs");
}

CMatchVertexsCommand::~CMatchVertexsCommand()
{
	
}

CString CMatchVertexsCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MATCHVERTEXS);
}

void CMatchVertexsCommand::Start()
{
	if( !m_pEditor )return;
	m_lfToler = 0.2;
	CEditCommand::Start();
	
	m_pEditor->CloseSelector();
	
}

void CMatchVertexsCommand::Abort()
{
	CEditCommand::Abort();
	
	m_pEditor->RefreshView();
}

void CMatchVertexsCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;

	var = (LPCTSTR)m_strLayer0;
	tab.AddValue("Layers",&CVariantEx(var));
	
	var = (_variant_t)m_lfToler;
	tab.AddValue("TolerXY",&CVariantEx(var));
}

void CMatchVertexsCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("MatchVertexsCommand",StrFromLocalResID(IDS_CMDNAME_MATCHVERTEXS));

	param->AddLayerNameParamEx("Layers",(LPCTSTR)m_strLayer0,StrFromResID(IDS_CMDPLANE_PROCESS_LAY));
	
	param->AddParam("TolerXY",m_lfToler,StrFromResID(IDS_TOLER_XY));

	CEditCommand::FillShowParams(param,bForLoad);
}

void CMatchVertexsCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"Layers",var) )
	{
		m_strLayer0 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"TolerXY",var) )
	{
		m_lfToler = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab);
}

void CMatchVertexsCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{
		m_ptDragStart = pt;
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);
		return;
	}
	else if (m_nStep==1)
	{
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		m_ptDragEnd = pt;
        PT_3D Pt[2];
		Pt[0] = m_ptDragStart;
		Pt[1] = m_ptDragEnd;
		
		CDataSourceEx *pDS = m_pEditor->GetDataSource();
		CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
		
		double r = m_lfToler;

		CFtrLayerArray layers0;
		pDS->GetFtrLayersByNameOrCode_editable(m_strLayer0,layers0);

		pDS->SaveAllQueryFlags(TRUE,FALSE);
		int i;
		for( i=0; i<layers0.GetSize(); i++)
		{
			layers0[i]->SetAllowQuery(TRUE);
		}
		
		Envelope el,e;
		el.CreateFromPts(Pt,2);
		e = el;
		e.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
		m_pEditor->GetDataQuery()->FindObjectInRect(e,m_pEditor->GetCoordWnd().m_pSearchCS);

		int j,k,num1;
		const CPFeature *ftrs = m_pEditor->GetDataQuery()->GetFoundHandles(num1);		
		CUndoBatchAction undo0(m_pEditor,Name());

		CArray<VertexMatchItem,VertexMatchItem> arrItems, arrItems_all;

		BOOL bCheckLayName = (!m_strLayer0.IsEmpty());

		GProgressStart(num1);

		//计算所有的匹配点（咬合点），并将匹配点信息放入数组
		SnapItemArray arr; 

		BOOL bMsgbox = FALSE;
		
		//计算端点匹配状态
		for (i=0;i<num1;i++)
		{
			GProgressStep();
			
			SnapItemArray arr1; 
			for( k=0; k<num1; k++)
			{
				GetSnapVertexes(ftrs[i],ftrs[k],el,arr1);
			}

			if( arr1.GetSize()>100 )
			{
				if( !bMsgbox )
				{
					bMsgbox = TRUE;
					if( AfxMessageBox(IDS_ERROR_MUCHTIME,MB_YESNO)!=IDYES )
					{
						arr.RemoveAll();
						break;
					}
				}
			}

			//去掉串联的部分（A->B,B->C），避免一个地物的某个节点被多次修改
// 			for( int m=arr.GetSize()-1; m>=0; m--)
// 			{
// 				SnapItem item0 = arr[m];
// 				for( int n=arr1.GetSize()-1; n>=0; n--)
// 				{
// 					SnapItem item1 = arr1[n];
// 					if( (item0.pFtr2==item1.pFtr1 && item0.ptIdx2==item1.ptIdx1) ||
// 						(item0.pFtr2==item1.pFtr2 && item0.ptIdx2==item1.ptIdx2) )
// 					{
// 						arr1.RemoveAt(n);
// 					}
// 				}				
// 			}

			//去掉两个节点同时连接某个节点的情况（A->C,B->C）
// 			for( m=arr1.GetSize()-1; m>=0; m--)
// 			{
// 				SnapItem item0 = arr1[m];
// 				for( int n=m-1; n>=0; n--)
// 				{
// 					SnapItem item1 = arr1[n];
// 					if( item0.pFtr2==item1.pFtr2 && item0.ptIdx2==item1.ptIdx2 )
// 					{
// 						arr1.RemoveAt(m);
// 						break;
// 					}
// 				}				
// 			}

			arr.Append(arr1);
		}

		GProgressEnd();

		pDS->RestoreAllQueryFlags();
		
		if( arr.GetSize()<=0 )
		{
			Abort();
			return;
		}
		
		//按照地物和咬合点位置的顺序对交点排序
		qsort(arr.GetData(),arr.GetSize(),sizeof(SnapItem),compare_snap_item);
		
		//对每个地物依次处理：将该地物的N个重叠点调整位置
		SnapItem *data = arr.GetData();
		int nSum = arr.GetSize();
		
		int startItem = 0, stopItem = -1;
		for( i=0; i<nSum; i++)
		{
			if( i!=(nSum-1) && data[i].pFtr1==data[i+1].pFtr1 && data[i].ptIdx1==data[i+1].ptIdx1 )
				continue;
			
			stopItem = i+1;
			
			CGeometry *pGeo = data[startItem].pFtr1->GetGeometry();
			const CShapeLine *pShp = pGeo->GetShape();
			
			CArray<PT_3DEX,PT_3DEX> pts;
			pShp->GetPts(pts);
			
			//处理start~stop之间的重叠点
			//计算平均值，并计算各个重叠点所对应的地物的顶点序号，便于下一步修改
			double ax = data[startItem].pt1.x, ay = data[startItem].pt1.y;
			CArray<int,int> arrIndex;
			
			for( j=startItem; j<stopItem; j++)
			{
				CGeometry *pGeo2 = data[j].pFtr2->GetGeometry();
				const CShapeLine *pShp2 = pGeo2->GetShape();
				
				arrIndex.Add(pShp2->GetKeyPos(data[j].ptIdx2));
				
				ax += data[j].pt2.x;
				ay += data[j].pt2.y;
			}
			
			ax /= (stopItem-startItem+1);
			ay /= (stopItem-startItem+1);
			
			arrIndex.Add(pShp->GetKeyPos(data[startItem].ptIdx1));
			
			//修改地物
			for( j=startItem; j<stopItem+1; j++)
			{
				int ptIndex = arrIndex[j-startItem];
				CFeature* pFtr = ((j==stopItem)?data[startItem].pFtr1:data[j].pFtr2);
				
				CUndoVertex undo(m_pEditor,Name());			
				m_pEditor->DeleteObject(FtrToHandle(pFtr));
				
				CGeometry *pGeo2 = pFtr->GetGeometry();
				PT_3DEX expt = pGeo2->GetDataPoint(ptIndex);
				undo.ptOld = expt;
				expt.x = ax;
				expt.y = ay;
				pGeo2->SetDataPoint(ptIndex,expt);
				
				undo.ptNew = expt;
				undo.bRepeat = FALSE;
				undo.nPtType = PT_KEYCTRL::typeKey;
				undo.nPtIdx = ptIndex;
				undo.handle = FtrToHandle(pFtr);
				
				m_pEditor->RestoreObject(FtrToHandle(pFtr));
				
				undo0.AddAction(&undo);
			}
			
			startItem = stopItem;
		}

		undo0.Commit();

		m_pEditor->RefreshView();
		Finish();
	}

	CEditCommand::PtClick(pt,flag);
}



void CMatchVertexsCommand::PtMove(PT_3D &pt)
{
	if(m_nStep==1)
	{
		GrBuffer buf;
		PT_3D temp = m_ptDragStart;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_ptDragStart);
		temp.x = pt.x;
		buf.LineTo(&temp);		
		buf.LineTo(&pt);
		temp.x = m_ptDragStart.x;
		temp.y = pt.y;
		buf.LineTo(&temp);
		buf.LineTo(&m_ptDragStart);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);		
		return;
	}
}



// CMatchAllVertexsCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMatchAllVertexsCommand,CTopoPreTreatCommand)

CMatchAllVertexsCommand::CMatchAllVertexsCommand()
{
	m_lfToler = 0.2;
	strcat(m_strRegPath,"\\MatchAllVertexs");
}

CMatchAllVertexsCommand::~CMatchAllVertexsCommand()
{
	
}

CString CMatchAllVertexsCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MATCHALLVERTEXS);
}

void CMatchAllVertexsCommand::Start()
{
	if( !m_pEditor )return;
	m_lfToler = 0.2;
	CEditCommand::Start();
	
	m_pEditor->CloseSelector();
	GOutPut(StrFromResID(IDS_CMDTIP_CLICKTOGO));
}

void CMatchAllVertexsCommand::Abort()
{
	CEditCommand::Abort();
	
	m_pEditor->RefreshView();
}

void CMatchAllVertexsCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	
	_variant_t var;

	var = (LPCTSTR)m_strLayer0;
	tab.AddValue("Layers",&CVariantEx(var));
	
	var = (_variant_t)m_lfToler;
	tab.AddValue("TolerXY",&CVariantEx(var));
}

void CMatchAllVertexsCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("MatchAllVertexsCommand",Name());

	param->AddLayerNameParamEx("Layers",(LPCTSTR)m_strLayer0,StrFromResID(IDS_CMDPLANE_PROCESS_LAY));
	
	param->AddParam("TolerXY",m_lfToler,StrFromResID(IDS_TOLER_XY));

	CEditCommand::FillShowParams(param,bForLoad);
}

void CMatchAllVertexsCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"Layers",var) )
	{
		m_strLayer0 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,"TolerXY",var) )
	{
		m_lfToler = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	CEditCommand::SetParams(tab);
}

void CMatchAllVertexsCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	
	//只让目标图层可以查询
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	
	CFtrLayerArray layers0;
	pDS->GetFtrLayersByNameOrCode_editable(m_strLayer0,layers0);

	int nSum = 0, i;
	pDS->SaveAllQueryFlags(TRUE,FALSE);
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
		layers0[i]->SetAllowQuery(TRUE);
	}
		
	CUndoBatchAction undo(m_pEditor,Name());
	
	GProgressStart(nSum);
	
	//节点咬合, Progress: 1*nSum
	SnapVertexes2(layers0,&undo);
	SnapVertexes(layers0,&undo);
	
	GProgressEnd();
	
	pDS->RestoreAllQueryFlags();
	
	undo.Commit();
	
	Finish();
	
	CEditCommand::PtClick(pt,flag);
}


void CMatchAllVertexsCommand::SnapVertexes2(CFtrLayerArray& layers0, CUndoBatchAction* pUndo)
{
	int i, j, k;
	
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();		

	CFtrArray ftrsArray;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray.Add(pFtr);
		}
	}

	int nCount = ftrsArray.GetSize();
	
	//计算所有的匹配点（咬合点），并将匹配点信息放入数组
	SnapItemArray arr;

	GProgressStart(nCount);

	for( i=0; i<nCount; i++)
	{
		GProgressStep();

		SnapVertexes2_One(ftrsArray[i],m_lfToler,GraphAPI::g_lfZTolerance,TRUE,FALSE,pUndo);
	}

	GProgressEnd();
}


void CMatchAllVertexsCommand::AddVectexUndo(CUndoBatchAction* pUndo, CFeature *pFtr, int ptidx, PT_3DEX ptOld, PT_3DEX ptNew, BOOL bRepeat)
{
	CUndoVertex undo(m_pEditor,Name());

	undo.handle = FtrToHandle(pFtr);
	undo.bRepeat = bRepeat;
	undo.ptOld = ptOld;
	undo.ptNew = ptNew;
	undo.nPtIdx = ptidx;
	undo.nPtType = PT_KEYCTRL::typeKey;
	pUndo->AddAction(&undo);
}

BOOL CMatchAllVertexsCommand::SnapVertexes2_One(CFeature *pFtr, double lfSusToler, double lfTolerZ, BOOL bCrossLay, BOOL bModifyZ, CUndoBatchAction* pUndo)
{
	CDlgDoc *pDoc = PDOC(m_pEditor);
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	CDataQuery* pDQ = pDoc->GetDataQuery();
	CCoordSys *pCS = pDoc->GetCoordWnd().m_pSearchCS;

	CArray<PT_3DEX,PT_3DEX> arrPts, arrPtsKey, arrPtsKey0;
	PT_3DEX targetPt,tempPt;
	PT_3D line[2];

	if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		return FALSE;

	CString strLayName;
	if(!bCrossLay)strLayName = pDS->GetFtrLayerOfObject(pFtr)->GetName();
	((CGeoCurveBase*)pFtr->GetGeometry())->GetShape()->GetPts(arrPts);
	pFtr->GetGeometry()->GetShape(arrPtsKey);
	
	arrPtsKey0.Copy(arrPtsKey);

	int npt = arrPts.GetSize();

	if( arrPts.GetSize()<2 )
		return FALSE;

	CArray<CPFeature,CPFeature> arrFoundHandles;

	double c_minToler = GraphAPI::g_lfDisTolerance;
	c_minToler = 1e-6;
	
	int cur_idx = 0;

	//处理地物闭合的情况
	if( GraphAPI::GGet2DDisOf2P(arrPts[0],arrPts[npt-1])<lfSusToler )
	{
		//正确闭合，无需处理
		if( GraphAPI::GGet3DDisOf2P(arrPts[0],arrPts[npt-1])<c_minToler )
			return FALSE;

		int nKeyPt = arrPtsKey.GetSize();

		PT_3DEX ptOld = arrPtsKey[nKeyPt-1];

		arrPtsKey[nKeyPt-1].x = arrPtsKey[0].x;
		arrPtsKey[nKeyPt-1].y = arrPtsKey[0].y;

		if( fabs(arrPtsKey[0].z-arrPtsKey[nKeyPt-1].z)<lfTolerZ )
		{
			arrPtsKey[nKeyPt-1].z = arrPtsKey[0].z;
		}

		AddVectexUndo(pUndo, pFtr, nKeyPt-1, ptOld, arrPtsKey[nKeyPt-1], FALSE);

		m_pEditor->DeleteObject(FtrToHandle(pFtr));

		pFtr->GetGeometry()->CreateShape(arrPtsKey.GetData(),arrPtsKey.GetSize());

		m_pEditor->RestoreObject(FtrToHandle(pFtr));

		return TRUE;
	}

	BOOL bModified = FALSE;

	for(int i=0; i<2; i++)
	{
		//首点
		if( i==0 )
		{
			targetPt = arrPts[0];
			double dis = GraphAPI::GGet2DDisOf2P(targetPt,arrPts[1]);
			double k =  (dis<1e-6?0:(lfSusToler/dis));
			line[0].x = targetPt.x+(arrPts[1].x-targetPt.x)*k;
			line[0].y = targetPt.y+(arrPts[1].y-targetPt.y)*k;
			line[1].x = targetPt.x+targetPt.x-line[0].x;
			line[1].y = targetPt.y+targetPt.y-line[0].y;
			cur_idx = 0;
		}
		//尾点
		else
		{
			targetPt = arrPts[npt-1];
			double dis = GraphAPI::GGet2DDisOf2P(targetPt,arrPts[npt-2]);
			double k =  (dis<1e-6?0:(lfSusToler/dis));
			line[0].x = targetPt.x+(arrPts[npt-2].x-targetPt.x)*k;
			line[0].y = targetPt.y+(arrPts[npt-2].y-targetPt.y)*k;
			line[1].x = targetPt.x+targetPt.x-line[0].x;
			line[1].y = targetPt.y+targetPt.y-line[0].y;
			cur_idx = arrPtsKey.GetSize()-1;
		}

		Envelope e;
		e.CreateFromPtAndRadius(targetPt,lfSusToler);

		BOOL bProcess = FALSE;
		if( pDQ->FindObjectInRect(e,NULL,FALSE,FALSE)>1 )
		{
			int nNum = 0;
			const CPFeature *ftr = pDQ->GetFoundHandles(nNum);
			arrFoundHandles.SetSize(nNum);
			memcpy(arrFoundHandles.GetData(),ftr,nNum*sizeof(CPFeature));
			ftr = arrFoundHandles.GetData();
			if (nNum<=0)
				continue;

			//判断是不是悬点：第一步，是否有顶点重合
			int j=0;

			//是悬挂点，首先尝试按照延伸或者缩进来处理
			PT_3DEX ret;
			ret.pencode = penLine;
			double mint = -1, mindis = -1;
			CFeature *pFtr0 = NULL;
			int idx = -1;
			CArray<PT_3DEX,PT_3DEX> arrPts1;

			for (j=0;j<nNum;j++)
			{
				if(ftr[j]==pFtr)
					continue;

				if (!ftr[j]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					continue;
				}
				if(!bCrossLay&&strLayName.CompareNoCase(pDS->GetFtrLayerOfObject(ftr[j])->GetName())!=0)
					continue;
				CArray<PT_3DEX,PT_3DEX> arrPts0;
				((CGeoCurveBase*)ftr[j]->GetGeometry())->GetShape()->GetPts(arrPts0);
				int nSz = arrPts0.GetSize();
				int start = 0, end = nSz-1;

				for (int k=start;k<end;k++)
				{
					double xt,yt;
					double t0;
					if(GraphAPI::GGetLineIntersectLineSeg(line[0].x,line[0].y,line[1].x,line[1].y,arrPts0[k].x,arrPts0[k].y,arrPts0[k+1].x,arrPts0[k+1].y,&xt,&yt,&t0))
					{
						if (idx<0||fabs(t0-0.5)<fabs(mint-0.5))
						{
							idx = k;
							mint = t0;
							pFtr0 = ftr[j];								
							ret.x = xt;
							ret.y = yt;
							double tem = arrPts0[k+1].x-arrPts0[k].x;
							if(fabs(tem)<c_minToler)
								ret.z = arrPts0[k].z+(arrPts0[k+1].z-arrPts0[k].z)*(yt-arrPts0[k].y)/(arrPts0[k+1].y-arrPts0[k].y);
							else
								ret.z = arrPts0[k].z+(arrPts0[k+1].z-arrPts0[k].z)*(xt-arrPts0[k].x)/tem;
						}
					}					
				}
			}
			//可以按照延伸或者缩进来处理
			if( mint>=0.0 && mint<=1.0 && pFtr0!=NULL )
			{
				goto modify_obj;
			}

			//尝试按照垂直距离处理
			ret = PT_3DEX();
			ret.pencode = penLine;
			pFtr0 = NULL;
			idx = -1;

			for (j=0;j<nNum;j++)
			{
				if(ftr[j]==pFtr)
					continue;

				if (!ftr[j]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					continue;
				}
				if(!bCrossLay&&strLayName.CompareNoCase(pDS->GetFtrLayerOfObject(ftr[j])->GetName())!=0)
					continue;

				Envelope sch_evlp;
				sch_evlp.CreateFromPtAndRadius(targetPt,lfSusToler);

				CGeoCurveBase *pGeo = (CGeoCurveBase*)ftr[j]->GetGeometry();
				
				if( pGeo->FindNearestBasePt(targetPt,sch_evlp,NULL,&ret,NULL) )
				{
					if( fabs(targetPt.z-ret.z)>=lfTolerZ )
						ret.z = targetPt.z;

					idx = 1;
					pFtr0 = ftr[j];
				}
			}

			//可以按照垂直距离处理			
			if( idx>=0 )
			{
				int state = 0;
				idx = CLinearizer::FindKeyPosOfBaseLine(ret,((CGeoCurveBase*)pFtr0->GetGeometry())->GetShape(),NULL,&state);				
			}

modify_obj:	

			if(pFtr0)pFtr0->GetGeometry()->GetShape(arrPts1);
			if( idx>=0 && idx<arrPts1.GetSize() )
			{
				if ( idx<arrPts1.GetSize()-1 && GraphAPI::GGet2DDisOf2P(arrPts1[idx+1],ret)<lfSusToler && GraphAPI::GGet2DDisOf2P(arrPts1[idx+1],ret)<GraphAPI::GGet2DDisOf2P(arrPts1[idx],ret))
				{
					tempPt = arrPts1[idx+1];
					tempPt.pencode = arrPts[cur_idx].pencode;								
					if(!bModifyZ)
					{
						tempPt.z = targetPt.z;
					}
					arrPtsKey.SetAt(cur_idx,tempPt);
				}
				else if (GraphAPI::GGet2DDisOf2P(arrPts1[idx],ret)<lfSusToler)
				{
					tempPt = arrPts1[idx];
					tempPt.pencode = arrPtsKey[cur_idx].pencode;								
					if(!bModifyZ)
					{
						tempPt.z = targetPt.z;
					}
					arrPtsKey.SetAt(cur_idx,tempPt);
				}				
				else
				{
					tempPt = ret;
					tempPt.pencode = arrPtsKey[cur_idx].pencode;								
					if(!bModifyZ)
					{
						tempPt.z = targetPt.z;
					}
					arrPtsKey.SetAt(cur_idx,tempPt);
					arrPts1.InsertAt(idx+1,ret);
				}

				bModified = TRUE;

				AddVectexUndo(pUndo,pFtr,cur_idx,arrPtsKey0[cur_idx],arrPtsKey[cur_idx],FALSE);
			}			
		}
	}
	
	if(bModified)
	{
		m_pEditor->DeleteObject(FtrToHandle(pFtr));

		pFtr->GetGeometry()->CreateShape(arrPtsKey.GetData(),arrPtsKey.GetSize());

		m_pEditor->RestoreObject(FtrToHandle(pFtr));		
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////
// CTopoBuildSurfaceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CTopoBuildSurfaceCommand/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTopoBuildSurfaceCommand,CEditCommand)

CTopoBuildSurfaceCommand::CTopoBuildSurfaceCommand()
{
   
   strcat(m_strRegPath,"\\TopoBuildSurface");
}

CTopoBuildSurfaceCommand::~CTopoBuildSurfaceCommand()
{
	
}

CString CTopoBuildSurfaceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TOPO_BUILD);
}

void CTopoBuildSurfaceCommand::Start()
{
	if( !m_pEditor )return;
	
	m_DeleteExistFace=TRUE;
	m_lfToler=GraphAPI::g_lfDisTolerance;
	m_pr=180/PI;
	m_bFillAtt = TRUE;
	m_bBuildWithSurfacePt = FALSE;
	m_StoreLayer = _T("30");

	ColorList[0]=RGB(150,150,200);
	ColorList[1]=RGB(210,170,170);
	ColorList[2]=RGB(100,100,190);
	ColorList[3]=RGB(100,210,190);
	ColorList[4]=RGB(110,180,170);
	ColorList[5]=RGB(24,238,156);
	ColorList[6]=RGB(237,140,242);
	ColorList[7]=RGB(73,176,226);
	ColorList[8]=RGB(201,254,152);
	ColorList[9]=RGB(207,95,129);
    //
	CEditCommand::Start();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->CloseSelector();
	
	m_nStep = 0;
	
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CTopoBuildSurfaceCommand::Finish()
{
	UpdateParams(TRUE);
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CTopoBuildSurfaceCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CTopoBuildSurfaceCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("Layer0",&CVariantEx(var));
	
	var = (LPCTSTR)(m_StoreLayer);
	tab.AddValue("Layer1",&CVariantEx(var));
	
	var = (bool)(m_DeleteExistFace);
	tab.AddValue("Snap3D",&CVariantEx(var));

	var = (bool)(m_bFillAtt);
	tab.AddValue("FillAtt",&CVariantEx(var));

	var = (bool)(m_bDelSurfacePt);
	tab.AddValue("DelSurfacePt",&CVariantEx(var));

	var = (bool)(m_bBuildWithSurfacePt);
	tab.AddValue("BuildWithSurfacePt",&CVariantEx(var));
}

void CTopoBuildSurfaceCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("TopoBuildSurfaceCommand",Name());
	
	param->AddLayerNameParamEx("Layer0",(LPCTSTR)m_HandleLayer,StrFromResID(IDS_CMDPLANE_HANDLELAYER));

	param->AddLayerNameParamEx("Layer1",(LPCTSTR)m_StoreLayer,StrFromResID(IDS_CMDPLANE_STORELAYER),NULL,LAYERPARAMITEM_AREA);
	
	param->AddParam("DeleteExistFace",(bool)m_DeleteExistFace,StrFromResID(IDS_CMDPLANE_DELEXISTFACE));
	param->AddParam("FillAtt",(bool)m_bFillAtt,StrFromResID(IDS_CMDNAME_FILLSURFACE_FROMPT));
	if(m_bFillAtt)
	{
		param->AddParam("DelSurfacePt",(bool)m_bDelSurfacePt,StrFromResID(IDS_CMDNAME_DEL_SURFACEPT));
	}
	param->AddParam("BuildWithSurfacePt",(bool)m_bBuildWithSurfacePt,StrFromResID(IDS_CMDNAME_BUILD_WITHPT));
}

void CTopoBuildSurfaceCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Layer0",var) )
	{					
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimLeft();
		m_HandleLayer.TrimRight();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,"Layer1",var) )
	{					
		m_StoreLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_StoreLayer.TrimRight();
		m_StoreLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}

 	if( tab.GetValue(0,"DeleteExistFace",var) )
 	{
 	    m_DeleteExistFace = (bool)(_variant_t)*var;
 		SetSettingsModifyFlag();
 	}

	if( tab.GetValue(0,"FillAtt",var) )
	{
		m_bFillAtt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
 	}

	if( tab.GetValue(0,"DelSurfacePt",var) )
	{
		m_bDelSurfacePt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	if( tab.GetValue(0,"BuildWithSurfacePt",var) )
	{
		m_bBuildWithSurfacePt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
 	}

	CEditCommand::SetParams(tab,bInit);
}


void CTopoBuildSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;
	if(m_StoreLayer.IsEmpty())
	{
		return;
	}

	CFtrLayer *out_layer=0;
	CDataSourceEx  *pData = m_pEditor->GetDataSource();
	out_layer=pData->GetFtrLayer((LPCTSTR)m_StoreLayer);
	if(out_layer>0)
	{
		m_Layer_ID=out_layer->GetID();
		if(m_DeleteExistFace)
		{
			int i=0;
			while(i<out_layer->GetObjectCount())
			{
				CFeature *ftr=out_layer->GetObject(i);
				if(ftr!=0 && ftr->GetPurpose()==FTR_EDB)
				{
					m_pEditor->DeleteObject(FtrToHandle(ftr));
				}
				else
				{
					++i;
				}
			}
		}
	}
	else
	{
		out_layer=new CFtrLayer();
		out_layer->SetName((LPCTSTR)m_StoreLayer);
		m_pEditor->AddFtrLayer(out_layer);
		m_Layer_ID=out_layer->GetID();
	}
	//
	build_surface();

	if(m_bFillAtt)
	{
		CFillSurfaceATTFromPtCommand *pFillAttCommand;
		pFillAttCommand = new CFillSurfaceATTFromPtCommand();
		pFillAttCommand->m_HandleLayer = _T("");
		pFillAttCommand->Init(m_pEditor);
		pFillAttCommand->m_bDelSurfacePt = m_bDelSurfacePt;
		PT_3D pt;
		pFillAttCommand->PtClick(pt,0);
		delete pFillAttCommand;
	}

	AfxGetMainWnd()->SendMessage(WM_COMMAND, (WPARAM)ID_FILL_RAND_COLOR, 0);

	Finish();
	CEditCommand::PtClick(pt,flag);
}

ULONG32 CTopoBuildSurfaceCommand::get_geo_data()
{
	CDataSourceEx  *pData = m_pEditor->GetDataSource();  
	CString text =m_HandleLayer;
	text.Remove(' ');
	geo_list.RemoveAll();
	ftr_list.RemoveAll();
	ULONG32 geo_count=0;
	//
	if (text.IsEmpty())
	{
		CFeature *pFtr=NULL;	
		CFtrLayer *pLayer=NULL;   
		int nLayNum = pData->GetFtrLayerCount(),i,j;
		for( i=0; i<nLayNum; i++)
		{
			pLayer = pData->GetFtrLayerByIndex(i);
			if( !pLayer )continue;

			if( StrFromResID(IDS_MARKLAYER_NAME).CompareNoCase(pLayer->GetName())==0 )
			{
				continue;
			}
			
			int nObjNum = pLayer->GetObjectCount();
			for( j=0; j<nObjNum; j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !pFtr )continue;
				if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
				//
				   geo_list.Add(pFtr->GetGeometry());
				   ftr_list.Add(pFtr);
				   ++geo_count;
				}
			}
		}
	}
	else
	{
		CFtrLayer *pLayer = NULL;
		CFeature *pFtr = NULL;
		int startpos = 0, findpos = 0;
		while( findpos>=0 )
		{
			findpos = text.Find(_T(','),startpos);
			
			CString sub;
			if( findpos>startpos )
				sub = text.Mid(startpos,(findpos-startpos));
			else
				sub = text.Mid(startpos);
			CPtrArray p;
			pData->GetFtrLayer(sub,NULL,&p);		
			for (int i=0;i<p.GetSize();i++)
			{
				pLayer = (CFtrLayer *)p[i];
				int nObjNum = pLayer->GetObjectCount();
				for( int j=0; j<nObjNum; j++)
				{					
					pFtr = pLayer->GetObject(j);
					if( !pFtr )continue;	
					if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
					{
						//
						geo_list.Add(pFtr->GetGeometry());
						ftr_list.Add(pFtr);
						++geo_count;
					}			
				}
			}			
			startpos = findpos+1;
		}	
	}
	return geo_count;
}

void CTopoBuildSurfaceCommand::create_arc_list()
{
	arc_list.RemoveAll();
	int i,j;
	//生成弧列表

	int num1 = geo_list.GetSize();
	int num2 = ftr_list.GetSize();
	
	for(i=0;i<num1;++i)
	{
	    CShapeLine *shape=const_cast<CShapeLine *>(geo_list[i]->GetShape());
		//
		TArcItem temp;
		if(i<num2)
		{
			temp.FeatureFlag = ftr_list[i]->GetAppFlag();
			ftr_list[i]->SetAppFlag(i);
		}
		temp.geo_index=i;
		temp.point_count = shape->GetPtsCount();
		//
		temp.sp.data=shape->GetPt(0);
		temp.sp.connects=new TConnectItem();
		temp.sp.connects->index=i;
		temp.sp.connects->degree=361;
		temp.sp.connects->StoE=FALSE;
		temp.sp.next_arc = NULL;
		//
		temp.s_degree=get_degree(temp.sp.data,shape->GetPt(1));
		//
		temp.ep.data=shape->GetPt(temp.point_count-1);
		temp.ep.connects=new TConnectItem();
		temp.ep.connects->index=i;
		temp.ep.connects->degree=361;
		temp.ep.connects->StoE=TRUE;
		temp.ep.next_arc = NULL;
		//
		PT_3DEX temp_ep=shape->GetPt(temp.point_count-2);
		temp.e_degree=get_degree(temp.ep.data,temp_ep);
		//
		if(temp.sp==temp.ep)
		{
			temp.state=CLOSED;
		}
		else
		{
			temp.state=UNVISITED;
		}
		//
		temp.face_index_list=new CArray<int,int>;
		arc_list.Add(temp);
	}

	//构建弧的连接关系;
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	GProgressStart(arc_list.GetSize());	
	for(i=0;i<arc_list.GetSize();++i)
	{
		GProgressStep();
		//起点
		Envelope e;
		e.CreateFromPtAndRadius(arc_list[i].sp.data, m_lfToler);
		int num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
		const CPFeature *ftrs = pDQ->GetFoundHandles(num);
		for( j=0; j<num; j++)
		{
			int pos = ftrs[j]->GetAppFlag();//取出下标
			if( pos == i )
				continue;
			
			if(arc_list[i].sp==arc_list[pos].sp)
			{
               TConnectItem *p=new TConnectItem();
               p->index=pos;
			   //计算与此弧的角度(逆时针方向)；
               p->degree=arc_list[i].s_degree-arc_list[pos].s_degree;
			   if(fabs(p->degree)<0.0001) continue;
			   if(p->degree<0)
			   {
				   p->degree+=360;
			   }
			   p->StoE = TRUE;
			   //
			   arc_list[i].sp.add_con_item(p);
			}
			else if(arc_list[i].sp==arc_list[pos].ep)
			{
				TConnectItem *p=new TConnectItem();
				p->index=pos;
				//计算与此弧的角度(逆时针方向)；
                p->degree=arc_list[i].s_degree-arc_list[pos].e_degree;
				if(fabs(p->degree)<0.0001) continue;
				if(p->degree<0)
				{
					p->degree+=360;
				}
				p->StoE = FALSE;
				//
			    arc_list[i].sp.add_con_item(p);
			}				
		}
	
		//终点
		e.CreateFromPtAndRadius(arc_list[i].ep.data, m_lfToler);
		num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
		ftrs = pDQ->GetFoundHandles(num);
		for( j=0; j<num; j++)
		{
			int pos = ftrs[j]->GetAppFlag();//取出下标
			if( pos == i )
				continue;

			if(arc_list[i].ep==arc_list[pos].sp)
			{
				TConnectItem *p=new TConnectItem();
				p->index=pos;
			    //计算与此弧的角度(逆时针方向)；
                p->degree=arc_list[i].e_degree-arc_list[pos].s_degree;
				if(fabs(p->degree)<0.0001) continue;
				if(p->degree<0)
				{
					p->degree+=360;
				}
				p->StoE = TRUE;
				//
				arc_list[i].ep.add_con_item(p);
			}
			else if(arc_list[i].ep==arc_list[pos].ep)
			{
				TConnectItem *p=new TConnectItem();
				p->index=pos;
				//计算与此弧的角度(逆时针方向)；
                p->degree=arc_list[i].e_degree-arc_list[pos].e_degree;
				if(fabs(p->degree)<0.0001) continue;
				if(p->degree<0)
				{
					p->degree+=360;
				}
				p->StoE = FALSE;
				//
				arc_list[i].ep.add_con_item(p);
			}
		}
	}

	//还原feature的AppFlag
	for(i=0; i<num1 && i<num2; ++i)
	{
		ftr_list[i]->SetAppFlag(arc_list[i].FeatureFlag);
	}
	GProgressEnd();

	//标记断路的弧
IV:	int nIvalid=0;//每次循环找出的无效弧的个数
	for(i=0; i<arc_list.GetSize(); i++)
	{
		if(arc_list[i].state == CLOSED) continue;
		if(arc_list[i].state == INVALID) continue;
		int j=0;
		TConnectItem *pc = NULL;
		//ep
		pc = arc_list[i].ep.connects->next;
		BOOL IsConnectsValid = TRUE;//sp或者ep的所有连接弧是否都是断开的
		while(pc)
		{
			if(arc_list[pc->index].state==UNVISITED)
			{
				IsConnectsValid = FALSE;
				break;
			}
			pc = pc->next;
		}
		if(IsConnectsValid)
		{
			arc_list[i].state=INVALID;
			nIvalid++;
			continue;
		}
		//sp
		pc = arc_list[i].sp.connects->next;
		IsConnectsValid = TRUE;//sp的所有连接弧是否都是断开的
		while(pc)
		{
			if(arc_list[pc->index].state==UNVISITED)
			{
				IsConnectsValid = FALSE;
				break;
			}
			pc = pc->next;
		}
		if(IsConnectsValid)
		{
			arc_list[i].state=INVALID;
			nIvalid++;
			continue;
		}
	}
	if(nIvalid>0)   goto IV;//循环直到没有断开的弧，断开指的是不能和其他弧构成闭合

	//标记追踪过程中被往返走两次的弧， 这样的弧不参与构面
	for(i=0; i<arc_list.GetSize(); i++)
	{
		if(arc_list[i].state == CLOSED) continue;
		if(arc_list[i].state == INVALID) continue;
		TConnectItem *pc = NULL;
		BOOL bInvalid = FALSE;
		//ep
		TEndPoint *face_ep = &arc_list[i].ep;
		while(1)
		{
			pc = face_ep->connects->next;
			while(pc)
			{
				if(arc_list[pc->index].state==UNVISITED)
				{
					break;
				}
				pc = pc->next;
			}
			if(!pc) break;
			if(pc->index == i)
			{
				if(!pc->StoE)
				{
					arc_list[i].state=INVALID;
					bInvalid = TRUE;
				}
				break;
			}
			if(pc->StoE)
				face_ep = &arc_list[pc->index].ep;
			else
				face_ep = &arc_list[pc->index].sp;
		}
		if(bInvalid) continue;
		//sp
		TEndPoint *face_sp = &arc_list[i].sp;
		while(1)
		{
			pc = face_sp->connects->next;
			while(pc)
			{
				if(arc_list[pc->index].state==UNVISITED)
				{
					break;
				}
				pc = pc->next;
			}
			if(!pc) break;
			if(pc->index == i)
			{
				if(pc->StoE)
				{
					arc_list[i].state=INVALID;
					bInvalid = TRUE;
				}
				break;
			}
			if(pc->StoE)
				face_sp = &arc_list[pc->index].ep;
			else
				face_sp = &arc_list[pc->index].sp;
		}
		if(bInvalid) continue;
	}
}

void CTopoBuildSurfaceCommand::find_closed_arc()
{
   for (int i=0;i<arc_list.GetSize();++i)
   {
	   if(arc_list[i].state==CLOSED)
	   {
		   CShapeLine *shape=const_cast<CShapeLine *>(geo_list[arc_list[i].geo_index]->GetShape());
		   //
		   TFaceItem temp;
		   int j=0;
		   //
		   temp.color_index=-1;
		   temp.arc_item_list=new CArray<TConnectItemBase,TConnectItemBase>();
           TConnectItem *p=0;
		   //
		   TConnectItemBase item;
		   item.index=i;
		   //item.r_direction=arc_list[i].r_direct;
	       temp.arc_item_list->Add(item);
		   temp.point_count=arc_list[i].point_count;
		   temp.datas=new PT_3DEX[temp.point_count];
		   for(j=0;j<temp.point_count;++j)
		   {
              temp.datas[j]=shape->GetPt(j);
			  temp.datas[j].pencode = penLine;
		   }
		   arc_list[i].face_index_list->Add(face_list.GetSize());
		   int geo_index = arc_list[i].geo_index;
		   temp.layer_id  = m_pEditor->GetDataSource()->GetFtrLayerOfObject(ftr_list[geo_index])->GetID();
		   face_list.Add(temp);
	   }
   }
}

void CTopoBuildSurfaceCommand::create_face(int start_index, BOOL IsEP)
{
    TFaceItem temp;
	int i=0,j=0;
	//
	temp.color_index=-1;
	temp.arc_item_list=new CArray<TConnectItemBase,TConnectItemBase>();
	TConnectItem *p=0;
    //
	TConnectItemBase item;
	item.index=start_index;
	item.StoE=IsEP;
	temp.arc_item_list->Add(item);
	//
	arc_list[start_index].face_index_list->Add(face_list.GetSize());
	if(IsEP)
	{
		p=arc_list[start_index].ep.next_arc;
	}
	else
	{
		p=arc_list[start_index].sp.next_arc;
	}
	//
	TConnectItem *p1 = p;
	while(p!=0)
	{
       item.index=p->index;
	   item.StoE=p->StoE;
	   temp.arc_item_list->Add(item);
	   arc_list[p->index].face_index_list->Add(face_list.GetSize());
	   if(p->StoE)
	   {
		   arc_list[p->index].SEVisited = TRUE;//标记该弧s->e已经走过
		   p=arc_list[p->index].ep.next_arc;
	   }
	   else
	   {
		   arc_list[p->index].ESVisited = TRUE;//标记该弧e->s已经走过
           p=arc_list[p->index].sp.next_arc;
	   }

	   if(p && p->StoE && arc_list[p->index].SEVisited)
		   return;
	   if(p && !p->StoE && arc_list[p->index].ESVisited)
		   return;

	   if(p == p1)
	   {
		   return;//死循环
	   }
	}
	//
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
    CArray<PT_3DEX,PT_3DEX> temp_datas;
    //添加起始弧数据；
	int geo_index = arc_list[temp.arc_item_list->GetAt(0).index].geo_index;
	int temp_layer_id  = pDS->GetFtrLayerOfObject(ftr_list[geo_index])->GetID();
	int id;
	if(temp.arc_item_list->GetAt(0).StoE)
	{
		CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
		CArray<PT_3DEX,PT_3DEX> pts2;
		shape->GetPts(pts2);
		for(j=0;j<arc_list[temp.arc_item_list->GetAt(0).index].point_count;++j)
		{
			if(j<pts2.GetSize())
				temp_datas.Add(pts2[j]);
		}
	}
	else
	{
		CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
		CArray<PT_3DEX,PT_3DEX> pts2;
		shape->GetPts(pts2);
		for(j=arc_list[temp.arc_item_list->GetAt(0).index].point_count-1;j>=0;--j)
		{
			if(j<pts2.GetSize())
				temp_datas.Add(pts2[j]);
		}
	}
	//添加中间弧数据；
	for(i=1;i<temp.arc_item_list->GetSize()-1;++i)
	{
		geo_index=arc_list[temp.arc_item_list->GetAt(i).index].geo_index;
		id  = pDS->GetFtrLayerOfObject(ftr_list[geo_index])->GetID();
		if(id!=temp_layer_id)
		{
			temp_layer_id = 0;
		}

		if(temp.arc_item_list->GetAt(i).StoE)
		{
			CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
			CArray<PT_3DEX,PT_3DEX> pts2;
			shape->GetPts(pts2);
			for(j=1;j<arc_list[temp.arc_item_list->GetAt(i).index].point_count;++j)
			{
				if(j<pts2.GetSize())
					temp_datas.Add(pts2[j]);
			}
		}
		else
		{
			CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
			CArray<PT_3DEX,PT_3DEX> pts2;
			shape->GetPts(pts2);
			for(j=arc_list[temp.arc_item_list->GetAt(i).index].point_count-2;j>=0;--j)
			{
				if(j<pts2.GetSize())
					temp_datas.Add(pts2[j]);
			}
		}
	}
	//添加尾弧数据；
	geo_index=arc_list[temp.arc_item_list->GetAt(i).index].geo_index;
	id  = pDS->GetFtrLayerOfObject(ftr_list[geo_index])->GetID();
	if(id!=temp_layer_id)
	{
		temp_layer_id = 0;
	}
    if(temp.arc_item_list->GetAt(i).StoE)
	{
		CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
		CArray<PT_3DEX,PT_3DEX> pts2;
		shape->GetPts(pts2);
		for(j=1;j<arc_list[temp.arc_item_list->GetAt(i).index].point_count-1;++j)
		{
			if(j<pts2.GetSize())
				temp_datas.Add(pts2[j]);
		}
	}
	else
	{
		CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
		CArray<PT_3DEX,PT_3DEX> pts2;
		shape->GetPts(pts2);
		for(j=arc_list[temp.arc_item_list->GetAt(i).index].point_count-2;j>=1;--j)
		{
			if(j<pts2.GetSize())
				temp_datas.Add(pts2[j]);
		}
	}
	//闭合面；
	PT_3DEX end_point=temp_datas[0];
	temp_datas.Add(end_point);
	//
	temp.point_count=temp_datas.GetSize();
	temp.datas=new PT_3DEX[temp_datas.GetSize()];
	for (i=0;i<temp_datas.GetSize();++i)
	{
		temp.datas[i]=temp_datas[i];
	}
	temp.layer_id = temp_layer_id;
	//
	//剔除顺时针的面
	if(!GraphAPI::GIsClockwise(temp_datas.GetData(), temp_datas.GetSize()))
	{
		face_list.Add(temp);	
	}
	temp_datas.RemoveAll();
}

//此算法并不能保证所有相邻的面颜色都不一样；
void CTopoBuildSurfaceCommand::set_color()
{
	int current_color=0;
	for(int i=0;i<face_list.GetSize();++i)
	{
		if(face_list[i].color_index==-1)
		{
			face_list[i].color_index=current_color;
			current_color=(current_color+1)%10;
		}
		for(int j=0;j<face_list[i].arc_item_list->GetSize();++j)
		{
            for(int k=0;k<arc_list[(face_list[i].arc_item_list->GetAt(j)).index].face_index_list->GetSize();++k)
			{
				int t=arc_list[(face_list[i].arc_item_list->GetAt(j)).index].face_index_list->GetAt(k);
				if(t==i)
				{
					continue;
				}
				else
				{
					if(face_list[t].color_index==-1 || face_list[t].color_index==face_list[i].color_index)
					{
						face_list[t].color_index=(face_list[i].color_index+1)%10;
					}
				}
			}
		}
	}
}

void CTopoBuildSurfaceCommand::save_data()
{
	struct TfaceSort
	{
		CFeature *feature;
		double area;
		TfaceSort *next; 
	};
	TfaceSort *sort_head=new TfaceSort;
	sort_head->feature=0;
	sort_head->area=DBL_MAX;
	sort_head->next=0;
	//
	int transparency = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,"Transparency", 50);
	TfaceSort *p=0;
	for(int i=0;i<face_list.GetSize();++i)
	{
		CFeature *featur=new CFeature();
		CGeoSurface *geometry=new CGeoSurface();	
		geometry->CreateShape(face_list[i].datas,face_list[i].point_count);	
		geometry->EnableFillColor(TRUE,COLORREF(ColorList[face_list[i].color_index]));
		geometry->SetTransparency(transparency);
	 	featur->SetGeometry(geometry);
		featur->SetID(OUID());
		featur->SetPurpose(FTR_EDB);
		featur->SetAppFlag(face_list[i].layer_id);
		double area=geometry->GetArea();
		//
		p=sort_head;
        while(p->next!=0 && p->next->area>=area)
		{
           p=p->next;
		}
        TfaceSort *s=new TfaceSort;
		s->area=area;
		s->feature=featur;
		s->next=p->next;
        p->next=s;
	}

	//创建复杂面
	CFtrArray ftrs1, ftrs2;
	p=sort_head;
    while(p!=0 )
	{
		if(p->feature)ftrs1.Add(p->feature);

		TfaceSort *old=p;
		p=p->next;
		delete old;
	}

	CreateSurfaces(ftrs1, ftrs2);

	//
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if( !pScheme )  return;
	CUndoFtrs undo(m_pEditor,Name());
	GProgressStart(ftrs2.GetSize());
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	for(i=0; i<ftrs2.GetSize();i++)
	{
		GProgressStep();

		//如果和已存在的地物重合，则新建的面不添加
		Envelope e;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		ftrs2[i]->GetGeometry()->GetShape(arrPts);
		e.CreateFromPts(arrPts.GetData(), arrPts.GetSize(), sizeof(PT_3DEX));
		/*int num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
		const CPFeature *ftrs = pDQ->GetFoundHandles(num);
		BOOL bOverLapped = FALSE;
		for(k=0; k<num; k++)
		{
			CGeometry *pObj = ftrs[k]->GetGeometry();
			if(pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && ((CGeoCurveBase*)pObj)->IsClosed() )
			{
				if( IsOverlapped(ftrs2[i], ftrs[k]) )
				{
					bOverLapped = TRUE;
					ToNewSurface(FtrToHandle(ftrs[k]), undo);
					break;
				}
			}
		}

		if(bOverLapped)
		{
			delete ftrs2[i];
			continue;
		}*/

		int num = pDQ->FindObjectInRect(e,NULL,TRUE,FALSE), k;
		const CPFeature *ftrs = pDQ->GetFoundHandles(num);
		for(k=0; k<num; k++)
		{
			CGeometry *pObj = ftrs[k]->GetGeometry();
			if(pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)))
			{
				PT_3DEX pt0 = pObj->GetDataPoint(0);
				PT_3D pt3d;
				COPY_3DPT(pt3d, pt0);
				if(ftrs2[i]->GetGeometry()->bPtIn(&pt3d))
				{
					break;
				}
			}
		}

		if(m_bBuildWithSurfacePt && k==num)//无面域点时不构面
		{
			delete ftrs2[i];
			continue;
		}

		int layerid = 0;
		if(k==num)//无面域点
		{
			if(ftrs2[i]->GetAppFlag()>0)//构成ftrs2[i]的弧属于同一层
			{
				CFtrLayer *pLayer = pDS->GetFtrLayer(ftrs2[i]->GetAppFlag());
				if(pLayer)
				{
					CSchemeLayerDefine *pDefLayer = pScheme->GetLayerDefine( pLayer->GetName() );
					if( pDefLayer )
					{
						if(pDefLayer->GetDbGeoClass() == CLS_GEOSURFACE)//入库类型为面
						{
							layerid = ftrs2[i]->GetAppFlag();
							ftrs2[i]->GetGeometry()->SetColor(pDefLayer->GetColor());
							ftrs2[i]->GetGeometry()->SetSymbolName("");
						}
					}
				}
			}
		}

		ftrs2[i]->SetAppFlag(0);
		ftrs2[i]->SetPurpose(FTR_EDB);
		if(layerid==0)
		{
			layerid = m_Layer_ID;
		}
		m_pEditor->AddObject(ftrs2[i],layerid);
		undo.AddNewFeature(FtrToHandle(ftrs2[i]));
	}
	GProgressEnd();
	undo.Commit();
}

//如果重合的是面，则更改属性
//如果是闭合线，则转面
void CTopoBuildSurfaceCommand::ToNewSurface(FTR_HANDLE handle, CUndoFtrs& undo)
{
	CGeometry *pGeo = HandleToFtr(handle)->GetGeometry();
	if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
	{
		return;
	}

	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		CFeature *pNew = HandleToFtr(handle)->Clone();
		if (!pNew) return;
		pNew->SetPurpose(FTR_EDB);
		CArray<PT_3DEX,PT_3DEX> arrPts;
		HandleToFtr(handle)->GetGeometry()->GetShape(arrPts);
		CGeoSurface* pSurface = new CGeoSurface();
		pSurface->CreateShape(arrPts.GetData(), arrPts.GetSize());
		pNew->SetGeometry(pSurface);
		
		if (!m_pEditor->AddObject(pNew,m_pEditor->GetFtrLayerIDOfFtr(handle)))
		{
			delete pNew;
			return;
		}
		GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handle),pNew);
		undo.AddNewFeature(FTR_HANDLE(pNew));
		undo.AddOldFeature(handle);		
		m_pEditor->DeleteObject(handle);
	}
	else
	{
		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);
		
		if (pts.GetSize() < 3) return;
		
		BOOL bClosed = ((CGeoCurveBase*)pGeo)->IsClosed();
		if (!bClosed)
		{
			return;
		}
		
		CValueTable tab;
		tab.BeginAddValueItem();
		HandleToFtr(handle)->WriteTo(tab);
		tab.EndAddValueItem();
		
		tab.DelField(FIELDNAME_SHAPE);
		tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
		tab.DelField(FIELDNAME_FTRDELETED);
		tab.DelField(FIELDNAME_FTRID);
		tab.DelField(FIELDNAME_GEOCLASS);
		tab.DelField(FIELDNAME_FTRDISPLAYORDER);
		tab.DelField(FIELDNAME_FTRGROUPID);

		CFeature *pNew = HandleToFtr(handle)->Clone();
		if (!pNew) return;
		
		if (!pNew->CreateGeometry(CLS_GEOSURFACE)) return;
		pNew->ReadFrom(tab);
		pNew->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
		pNew->SetPurpose(FTR_EDB);
		CGeoSurface* pSurface = (CGeoSurface*)pNew->GetGeometry();
		if (!m_pEditor->AddObject(pNew,m_pEditor->GetFtrLayerIDOfFtr(handle)))
		{
			delete pNew;
			return;
		}
		
		GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handle),pNew);
		undo.AddNewFeature(FTR_HANDLE(pNew));
		undo.AddOldFeature(handle);		
		m_pEditor->DeleteObject(handle);
	}
}

void CTopoBuildSurfaceCommand::FindSameEdgePart(int *buf, int num, int& start, int& end)
{
	int i, i1;
	
	//找到任一个非公共点
	for( i=0; i<num; i++)
	{
		if( buf[i]==0 )break;
	}
	//无公共点
	if( i>=num )
	{
		start = end = 0;
		return;
	}
	//找到下一个公共点，此点可以认为是非公共部分的末点
	for( i1=i+1; i1<num+i; i1++)
	{
		if( buf[(i1%num)]!=0 )break;
	}
	end = (i1%num);
	
	//找到下一个非公共点，此点的前一点可以认为是非公共部分的起点
	for( ; i1<=num+i; i1++)
	{
		if( buf[((i1+1)%num)]==0 )break;
	}
	start = (i1%num);
}



//获得对象中在start和end之间的点，如果start>end，那么，获得的点是包括对象的首尾点的
void CTopoBuildSurfaceCommand::GetObjectPart(CGeometry* pObj, int start, int end, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);
	int nPtSum = arr.GetSize();
	PT_3DEX expt;
	
	if( end<start )end += nPtSum;
	
	for( int i=start; i<=end; i++)
	{
		expt = arr[i%nPtSum];
		arrPts.Add(expt);
	}
}

CGeoSurface * CTopoBuildSurfaceCommand::MergeSurface(CGeoSurface * pObj1, CGeoSurface * pObj2)
{
	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	e3.Intersect(&e2);
	if(e3.m_xl>e3.m_xh || e3.m_yl>e3.m_yh )
		return NULL;
	
	if( pObj1==NULL || pObj2==NULL )
		return NULL;
	
	if( !pObj1->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || !pObj2->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		return NULL;
	
	CGeoSurface *pObj1_new = NULL;
	CGeoSurface *pObj2_new = NULL;
	
	if(pObj1_new==NULL)
		pObj1_new = pObj1;
	
	if(pObj2_new==NULL)
		pObj2_new = pObj2;
	
	gsbh::GeoSurfaceBooleanHandle gsbhandle;
	if(!gsbhandle.setSurface(pObj2_new,pObj1_new))
	{
		if(pObj1_new!=pObj1)delete pObj1_new;
		if(pObj2_new!=pObj2)delete pObj2_new;
		return FALSE;
	}
	//
	if(!gsbhandle.init())
	{
		if(pObj1_new!=pObj1)delete pObj1_new;
		if(pObj2_new!=pObj2)delete pObj2_new;
		return FALSE;
	}
	//
	gsbhandle.join();
	
	CGeoSurface* pNewObj = gsbhandle.getResult();
	
	if(pObj1_new!=pObj1)delete pObj1_new;
	if(pObj2_new!=pObj2)delete pObj2_new;
	
	if(pNewObj == NULL)
		return NULL;
	//
	CGeoSurface* ptemp = (CGeoSurface*)pObj1->Clone();
	CArray<PT_3DEX, PT_3DEX> temp_shape;
	pNewObj->GetShape(temp_shape);
	ptemp->CreateShape(temp_shape.GetData(),temp_shape.GetSize());
	delete pNewObj;
	
	return ptemp;
/*	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	pObj1 = (CGeoSurface*)pObj1->Clone();
	pObj2 = (CGeoSurface*)pObj2->Clone();

	CArray<PT_3DEX,PT_3DEX> arr1, arr2;
	pObj1->GetShape(arr1);
	pObj2->GetShape(arr2);
	
	int npt1 = GraphAPI::GKickoffSame2DPoints(arr1.GetData(),arr1.GetSize());
	arr1.SetSize(npt1);
	int npt2 = GraphAPI::GKickoffSame2DPoints(arr2.GetData(),arr2.GetSize());
	arr2.SetSize(npt2);
	pObj1->CreateShape(arr1.GetData(),arr1.GetSize());
	pObj2->CreateShape(arr2.GetData(),arr2.GetSize());

	int nPtSum1 = arr1.GetSize(), nPtSum2 = arr2.GetSize(), count = 1;
	int nSamePtPos;
	CArray<int,int> pos1, pos2;
	pos1.SetSize(nPtSum1);
	pos2.SetSize(nPtSum2);

	//找到点重叠的部分，并设置标志位
	int *buf1 = pos1.GetData(), *buf2 = pos2.GetData();

	memset(buf1,0,sizeof(int)*nPtSum1);
	memset(buf2,0,sizeof(int)*nPtSum2);

	if (pObj1->IsClosed())
	{
		nPtSum1--;
	}

	if (pObj2->IsClosed())
	{
		nPtSum2--;
	}
	
	PT_3DEX expt;
	double tolerance = GraphAPI::g_lfDisTolerance;
	
	for( int i=0; i<nPtSum1; i++)
	{
		expt = arr1[i];

		if (i != 0 && GraphAPI::GIsEqual2DPoint(&expt,&arr1[i-1]) )
		{
			if (buf1[i-1] != 0)
			{					
				buf1[i] = buf1[i-1];
				buf1[i-1] = 0;
			}
			
			continue;
		}

		if(!e2.bPtIn(&expt))
			continue;

		PT_3D p0,p1;
		COPY_3DPT(p0,expt);
		//m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&p0,&p1);
		PT_KEYCTRL pos;
		pos = pObj2->FindNearestKeyCtrlPt(p0,tolerance,NULL,1);
		if (pos.IsValid())
		{				
			for (int j=pos.index; j<nPtSum2-1; j++)
			{
				if (!GraphAPI::GIsEqual2DPoint(&arr2[j],&arr2[j+1]))
				{
					break;
				}
			}

			nSamePtPos = j;
			buf1[i] = count;
			buf2[nSamePtPos] = count;
			count++;
		}
	}

	if(count<=1)
	{
		delete pObj1;
		delete pObj2;
		return NULL;		
	}

	int start1, end1, start2, end2;

	//获得非公共边的部分，但是包括了公共边的起点和末点
	FindSameEdgePart(buf1,nPtSum1,start1,end1);
	FindSameEdgePart(buf2,nPtSum2,start2,end2);

	//有问题的公共边
	if( (start1==end1) || (start2==end2) ||
		(buf1[start1]!=buf2[start2] && buf1[start1]!=buf2[end2]) ||
		(buf1[end1]!=buf2[start2] && buf1[end1]!=buf2[end2]) )
	{
		delete pObj1;
		delete pObj2;
		return NULL;
	}

	//建立新对象
	CGeoSurface *pNewObj = (CGeoSurface*)pObj1->Clone();	

	//获取重复边
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;

	GetObjectPart(pObj1,start1,end1,arrPts1);
	GetObjectPart(pObj2,start2,end2,arrPts2);

	//调整顺序
	if( buf1[end1]!=buf2[start2] )
	{
		int size = arrPts2.GetSize();
		for( i=0; i<size/2; i++)
		{
			expt = arrPts2.GetAt(i);
			arrPts2.SetAt(i,arrPts2.GetAt(size-1-i));
			arrPts2.SetAt(size-1-i,expt);
		}
	}

	delete pObj1;
	delete pObj2;

	//添加节点

	CArray<PT_3DEX,PT_3DEX> arr;

	arr.Append(arrPts1);
	arrPts2.RemoveAt(0);

	arr.Append(arrPts2);

	int npt3 = GraphAPI::GKickoffSame2DPoints(arr.GetData(),arr.GetSize());
	arr.SetSize(npt3);

	pNewObj->CreateShape(arr.GetData(),arr.GetSize());	

	return pNewObj;*/
}


void CTopoBuildSurfaceCommand::CreateSurfaces(CFtrArray& ftrs1, CFtrArray& ftrs2)
{	
	for(int i=0; i<ftrs1.GetSize(); i++)
	{
		CFeature *pFtr1 = ftrs1[i];
		if(!pFtr1)continue;
		
		CGeoSurface *pGeo1 = (CGeoSurface*)pFtr1->GetGeometry();
		CFtrArray ftrs;
		
		for( int j=i+1; j<ftrs1.GetSize(); j++)
		{
			CFeature *pFtr2 = ftrs1[j];
			if(!pFtr2)continue;
			CGeoSurface *pGeo2 = (CGeoSurface*)pFtr2->GetGeometry();

			if(IsInside(pGeo2,pGeo1))
			{
				//pGeo2 不能同时落入另一个面中
				for(int k=0; k<ftrs.GetSize(); k++)
				{
					CFeature *pFtr3 = ftrs[k];
					CGeoSurface *pGeo3 = (CGeoSurface*)pFtr3->GetGeometry();					
					if(IsInside(pGeo2,pGeo3))
					{
						break;
					}
				}

				if(k>=ftrs.GetSize())
				{
					ftrs.Add(pFtr2);					
				}
			}
		}

		if(ftrs.GetSize()==0)
		{
			ftrs2.Add(pFtr1);
			ftrs1[i] = NULL;
		}
		else
		{
			CFeature *pNewFtr = pFtr1->Clone();
			CGeoMultiSurface *pNewGeo = new CGeoMultiSurface();	
			pNewGeo->CopyFrom(pFtr1->GetGeometry());

			CGeoArray geos;
			geos.SetSize(ftrs.GetSize());
			for(int k=0; k<ftrs.GetSize(); k++)
			{
				CFeature *pFtr3 = ftrs[k];
				CGeoSurface *pGeo3 = (CGeoSurface*)pFtr3->GetGeometry();
				geos[k] = pGeo3->Clone();
			}

			//对子面作合并
			for(k=0; k<geos.GetSize(); k++)
			{
				CGeoSurface *pGeo3 = (CGeoSurface*)geos[k];
				if(!pGeo3)continue;

				for(int m=k+1; m<geos.GetSize(); m++)
				{
					CGeoSurface *pGeo4 = (CGeoSurface*)geos[m];
					if(!pGeo4)continue;

					CGeoSurface *pGeo5 = MergeSurface(pGeo3,pGeo4);
					if(pGeo5)
					{
						delete pGeo3;
						delete pGeo4;
						
						geos[k] = pGeo5;
						pGeo3 = pGeo5;
						geos[m] = NULL;

						//pGeo3已经改变，重新开始遍历
						m = k;
					}			
				}
			}

			CArray<PT_3DEX,PT_3DEX> arrPts0;
			pNewGeo->GetShape(arrPts0);
			
			for(k=0; k<geos.GetSize(); k++)
			{
				CGeoSurface *pGeo3 = (CGeoSurface*)geos[k];
				if(!pGeo3)continue;

				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo3->GetShape(arrPts);
				if(arrPts.GetSize()>0 )
					arrPts[0].pencode = penMove;

				arrPts0.Append(arrPts);				
				delete pGeo3;
			}

			pNewGeo->CreateShape(arrPts0.GetData(),arrPts0.GetSize());

			delete pFtr1;

			pNewFtr->SetGeometry(pNewGeo);

			ftrs2.Add(pNewFtr);
		}
	}	
}


BOOL CTopoBuildSurfaceCommand::IsInside(CGeometry *pObj_inside, CGeometry *pObj_outside)
{
	//1、前者外包必须落在后者外包之内；
	Envelope e1 = pObj_inside->GetEnvelope();
	Envelope e2 = pObj_outside->GetEnvelope();
	if( !e2.bEnvelopeIn(&e1) )
		return FALSE;
	
	//2、前者节点必须全部落在后者母线构成的区域内部；
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2, arrPts3;
	pObj_inside->GetShape(arrPts1);
	pObj_outside->GetShape(arrPts2);

	if( GraphAPI::GIsPtInRegion(arrPts1[0],arrPts2.GetData(),arrPts2.GetSize())!=2 )
		return FALSE;
	
	return TRUE;
}

BOOL CTopoBuildSurfaceCommand::IsOverlapped(CPFeature f1, CPFeature f2)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();
	
	if( !pGeo1->GetEnvelope().bIntersect(&pGeo2->GetEnvelope()) )
		return FALSE;
	
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	pGeo1->GetShape(arrPts1);
	pGeo2->GetShape(arrPts2);
	
	double toler = m_lfToler;
	toler = toler*toler;
	
	toler = 1e-8;
	
	if( arrPts1.GetSize()!=arrPts2.GetSize() )
		return FALSE;
	
	int npt = arrPts1.GetSize();
	
	for( int i=0; i<npt; i++)
	{
		PT_3DEX pt1 = arrPts1[i];
		PT_3DEX pt2 = arrPts2[i];
		if( (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y)>=toler )
		{
			break;
		}
	}
	
	if( i<npt )
	{
		for( i=0; i<npt; i++)
		{
			PT_3DEX pt1 = arrPts1[i];
			PT_3DEX pt2 = arrPts2[npt-1-i];
			if( (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y)>=toler )
			{
				break;
			}
		}
		
		if( i<npt )
			return FALSE;
	}
	
	return TRUE;
}

void CTopoBuildSurfaceCommand::face_trace()
{
	int i=0, j=0;
	GProgressStart(arc_list.GetSize());
	for(i=0; i<arc_list.GetSize(); i++)
	{
		GProgressStep();
		if(arc_list[i].state != UNVISITED)
		{
			continue;//跳过自闭合、断开的弧
		}

		if(arc_list[i].SEVisited && arc_list[i].ESVisited)
		{
			arc_list[i].state = HANDLED;
			continue;
		}

		//s->e最小角遍历
		if(!arc_list[i].SEVisited)
		{
			arc_list[i].SEVisited = TRUE;
			//定义一个指向该面尾点的指针；
			TEndPoint *face_ep = &arc_list[i].ep;

			TConnectItem *pc = NULL;
L0:			pc = face_ep->connects->next;
			while(pc)
			{
				if(arc_list[pc->index].state == UNVISITED)
				{
					break;//找到第一条可用的弧
				}
				pc = pc->next;
			}

			if(pc && pc->visited<i)
			{
				face_ep->next_arc = pc;
				pc->visited = i;
				if(pc->StoE && !arc_list[pc->index].SEVisited)
				{
					if(arc_list[i].sp == arc_list[pc->index].ep)//闭合
					{
						create_face(i, TRUE);
					}
					else//非闭合，查找后续弧
					{
						face_ep = &arc_list[pc->index].ep;
						goto L0;
					}
				}
				else if(!pc->StoE && !arc_list[pc->index].ESVisited)
				{
					if(arc_list[i].sp == arc_list[pc->index].sp)//闭合
					{
						create_face(i, TRUE);
					}
					else//非闭合，查找后续弧
					{
						face_ep = &arc_list[pc->index].sp;
						goto L0;
					}
				}
			}
		}

		//e->s最小角遍历
		if(!arc_list[i].ESVisited)
		{
			arc_list[i].ESVisited = TRUE;
			//定义一个指向该面尾点的指针；
			TEndPoint *face_ep = &arc_list[i].sp;
			
			TConnectItem *pc = NULL;
L1:			pc = face_ep->connects->next;
			while(pc)
			{
				if(arc_list[pc->index].state == UNVISITED)
				{
					break;//找到第一条可用的弧
				}
				pc = pc->next;
			}

			if(pc && pc->visited<i )
			{
				face_ep->next_arc = pc;
				pc->visited = i;
				if(pc->StoE && !arc_list[pc->index].SEVisited)
				{
					if(arc_list[i].ep == arc_list[pc->index].ep)//闭合
					{
						create_face(i, FALSE);
					}
					else//非闭合，查找后续弧
					{
						face_ep = &arc_list[pc->index].ep;
						goto L1;
					}
				}
				else if(!pc->StoE && !arc_list[pc->index].ESVisited)
				{
					if(arc_list[i].ep == arc_list[pc->index].sp)//闭合
					{
						create_face(i, FALSE);
					}
					else//非闭合，查找后续弧
					{
						face_ep = &arc_list[pc->index].sp;
						goto L1;
					}
				}
			}
		}

		arc_list[i].state=HANDLED;
	}
	GProgressEnd();
}

void CTopoBuildSurfaceCommand::clear()
{
	geo_list.RemoveAll();
	in_layer_list.RemoveAll();
	//
	for (int i=0;i<arc_list.GetSize();++i)
	{
       TConnectItem *p0=arc_list[i].sp.connects;
	   TConnectItem *p1=p0->next;
	   delete p0;
	   while(p1!=0)
	   {
		  p0=p1;
		  p1=p1->next;
		  delete p0;
	   }
	   //
	   p0=arc_list[i].ep.connects;
	   p1=p0->next;
	   delete p0;
	   while(p1!=0)
	   {
		   p0=p1;
		   p1=p1->next;
		   delete p0;
	   }
	   //
	   arc_list[i].face_index_list->RemoveAll();
	}
	arc_list.RemoveAll();
	//
	for(int j=0;j<face_list.GetSize();++j)
	{
		face_list[j].arc_item_list->RemoveAll();
		//delete [] face_list[i].datas;
	}
	face_list.RemoveAll();
}

BOOL CTopoBuildSurfaceCommand::build_surface()
{
    if(get_geo_data()==0)
	{
		return TRUE;
	}
	create_arc_list();
	find_closed_arc();
	face_trace();
	set_color();
	save_data();
	clear();
	Finish();
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CTopoSurfaceNoBreakCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTopoSurfaceNoBreakCommand,CEditCommand)

CTopoSurfaceNoBreakCommand::CTopoSurfaceNoBreakCommand()
{
	m_pTempDS = NULL;
	m_pDQ = NULL;
	m_nStep = -1;
	m_bAddPt = FALSE;
	m_bSnap3D = FALSE;
	strcat(m_strRegPath,"\\TopoSurfaceNoBreak");

	m_DeleteExistFace=TRUE;
	m_lfToler=GraphAPI::g_lfDisTolerance;
	m_pr=180/PI;
	m_bFillAtt = TRUE;
	m_bBuildWithSurfacePt = FALSE;
	
	ColorList[0]=RGB(150,150,200);
	ColorList[1]=RGB(210,170,170);
	ColorList[2]=RGB(100,100,190);
	ColorList[3]=RGB(100,210,190);
	ColorList[4]=RGB(110,180,170);
	ColorList[5]=RGB(24,238,156);
	ColorList[6]=RGB(237,140,242);
	ColorList[7]=RGB(73,176,226);
	ColorList[8]=RGB(201,254,152);
	ColorList[9]=RGB(207,95,129);
	//
	PRECISION=GraphAPI::g_lfDisTolerance;
	m_bProgress = TRUE;
	m_bCreateMultiSurface = TRUE;

	m_StoreLayer = _T("30");
	m_bDelSurfacePt = FALSE;
}

CTopoSurfaceNoBreakCommand::~CTopoSurfaceNoBreakCommand()
{
	if(m_pTempDS)
	{
		delete m_pTempDS;
		m_pTempDS = NULL;
	}
	if(m_pDQ)
	{
		delete m_pDQ;
		m_pDQ = NULL;
	}
}

CString CTopoSurfaceNoBreakCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TOPO_NOBREAK);
}

void CTopoSurfaceNoBreakCommand::Start()
{
	if( !m_pEditor )return;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CTopoSurfaceNoBreakCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CTopoSurfaceNoBreakCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CTopoSurfaceNoBreakCommand::GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, FTR_HANDLE id, CArray<double,double> &ret1, CArray<double,double> &ret2, CArray<double,double> &ret_z)
{
	PT_3D pt3d0, pt3d1, tpt;
	
	COPY_3DPT(pt3d0,pt0);
	COPY_3DPT(pt3d1,pt1);
	
	CDlgDataSource *pDS = GETDS(m_pEditor);

	{
		CFeature *pFtr = HandleToFtr(id);
		if (!pFtr) return;

		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) return;
		
		CFtrArray arrObjs;

		arrObjs.Add(pFtr);
		
		for (int j=0; j<arrObjs.GetSize(); j++)
		{
			CFeature *pFtr0 = arrObjs[j];
			if (!pFtr0) continue;

			CGeometry *po = pFtr0->GetGeometry();
			if( !po )continue;
			
			const CShapeLine  *pb = po->GetShape();
			
			if (pb == NULL) continue;
			
			int npt2 = pb->GetPtsCount();
			if( npt2<=1 )continue;
			CArray<PT_3DEX,PT_3DEX> pts;
			pb->GetPts(pts);	
			
			for( int k=0; k<npt2-1; k++)
			{
				//求交
				double vector1x = pt3d1.x-pt3d0.x, vector1y = pt3d1.y-pt3d0.y;
				double vector2x = pts[k+1].x-pts[k].x, vector2y = pts[k+1].y-pts[k].y;
				
				double delta = vector1x*vector2y-vector1y*vector2x;
				if( delta<1e-10 && delta>-1e-10 )continue;
				
				double t1 = ( (pts[k].x-pt3d0.x)*vector2y-(pts[k].y-pt3d0.y)*vector2x )/delta;
				if( t1<0.0 )continue;
				
				double t2 = ( (pts[k].x-pt3d0.x)*vector1y-(pts[k].y-pt3d0.y)*vector1x )/delta;
				if( t2<0.0 || t2>1.0 )continue;
				
				double z = pts[k].z + t2 * (pts[k+1].z-pts[k].z);

				tpt.x = pt3d0.x + t1*vector1x;  tpt.y = pt3d0.y + t1*vector1y;
				if( fabs(tpt.x-pt3d1.x)<GraphAPI::g_lfDisTolerance && fabs(tpt.y-pt3d1.y)<GraphAPI::g_lfDisTolerance )
					continue;
				
				//由小到大排序
				for( int m=0; m<ret1.GetSize(); m++)
				{
					if( t1<ret1[m] )break;
				}
				
				if( m<ret1.GetSize() )
				{
					ret1.InsertAt(m,t1);
					ret_z.InsertAt(m,z);
					ret2.InsertAt(m,k+t2);
				}
				else
				{
					ret1.Add(t1);
					ret_z.Add(z);
					ret2.Add(k+t2);
				}
			}

			if (pFtr0 != pFtr)
			{
				delete pFtr0;
			}
		}
		
	}
}

void CTopoSurfaceNoBreakCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("Layer0",&CVariantEx(var));
	
	var = (LPCTSTR)(m_StoreLayer);
	tab.AddValue("Layer1",&CVariantEx(var));
	
	var = m_lfToler;
	tab.AddValue("Toler",&CVariantEx(var));

	var = (bool)(m_bDelSurfacePt);
	tab.AddValue("DelSurfacePt",&CVariantEx(var));

	var = (bool)(m_bBuildWithSurfacePt);
	tab.AddValue("BuildWithSurfacePt",&CVariantEx(var));

	var = (LPCTSTR)(m_ReplaceLayer);
	tab.AddValue("Layer2",&CVariantEx(var));
}

void CTopoSurfaceNoBreakCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	//param->SetOwnerID("TopoSurfaceNoBreakCommand",Name());
	param->SetOwnerID("TopoSurfaceNoBreakCommand",Name());
	
	param->AddLayerNameParamEx("Layer0",(LPCTSTR)m_HandleLayer,StrFromResID(IDS_CMDPLANE_HANDLELAYER));

	param->AddLayerNameParam("Layer1",(LPCTSTR)m_StoreLayer,StrFromResID(IDS_CMDPLANE_STORELAYER),NULL,LAYERPARAMITEM_AREA);
	
	param->AddParam("Toler",(double)m_lfToler,StrFromResID(IDS_CMDPLANE_TOLER));

	if(m_bFillAtt)
	{
		param->AddParam("DelSurfacePt",(bool)m_bDelSurfacePt,StrFromResID(IDS_CMDNAME_DEL_SURFACEPT));
	}

	param->AddParam("BuildWithSurfacePt",(bool)m_bBuildWithSurfacePt,StrFromResID(IDS_CMDNAME_BUILD_WITHPT));
	param->AddLayerNameParamEx("Layer2",(LPCTSTR)m_ReplaceLayer,StrFromResID(IDS_SURFACE_NO_TOPO));
}

void CTopoSurfaceNoBreakCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Layer0",var) )
	{					
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimLeft();
		m_HandleLayer.TrimRight();
		SetSettingsModifyFlag();		
	}
	
	if( tab.GetValue(0,"Layer1",var) )
	{					
		m_StoreLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_StoreLayer.TrimRight();
		m_StoreLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}

 	if( tab.GetValue(0,"Toler",var) )
 	{
 	    m_lfToler = (double)(_variant_t)*var;
 		SetSettingsModifyFlag();
 	}

	if( tab.GetValue(0,"DelSurfacePt",var) )
	{
		m_bDelSurfacePt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	if( tab.GetValue(0,"BuildWithSurfacePt",var) )
	{
		m_bBuildWithSurfacePt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
 	}

	if( tab.GetValue(0,"Layer2",var) )
	{					
		m_ReplaceLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_ReplaceLayer.TrimRight();
		m_ReplaceLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}

	CEditCommand::SetParams(tab,bInit);
}

//inArr在此函数执行完成后内部删除，其appflag保存层id
//outArr需要外部删除，其appflag保存层id
void CTopoSurfaceNoBreakCommand::BuildSurface(CFtrArray& inArr, CFtrArray& outArr, BOOL bProgress)
{
	if(!m_pEditor) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	m_bProgress = bProgress;
	//新建数据源
	m_pDQ = new CDataQueryEx();
	if(!m_pDQ) return;
	m_pTempDS = new CDataSourceEx(m_pDQ);
	if(!m_pTempDS) return;
	m_pTempDS->SetScale(pDS->GetScale());
	PT_3D pts[4];
	double zmin,zmax;
	pDS->GetBound(pts,&zmin,&zmax);
	m_pTempDS->SetBound(pts,zmin,zmax);
	
	//临时数据源中加入目标层
	CFtrLayer *pLayer = m_pTempDS->CreateFtrLayer("TopoSufaceLayer");
	if(!pLayer) return;
	m_pTempDS->AddFtrLayer(pLayer);
	m_Layer_ID = pLayer->GetID();
	CFtrLayerArray arr;
	arr.Add(pLayer);
	
	//外包e内的所有地物拷贝到新数据源里
	int i;
	for( i=0; i<inArr.GetSize(); i++)
	{
		CFeature *pNew = inArr[i];

		int layid = pNew->GetAppFlag();
		CFtrLayer *pLayer1 = pDS->GetFtrLayer(layid);
		CFtrLayer *pLayer2 = pLayer;
		if(pLayer1)
		{
			CFtrLayer *pLayer3 = m_pTempDS->GetFtrLayer(pLayer1->GetName());
			if(!pLayer3)
			{
				pLayer3 = new CFtrLayer();
				pLayer3->SetID(layid);
				pLayer3->SetName(pLayer1->GetName());
				m_pTempDS->AddFtrLayer(pLayer3);
				arr.Add(pLayer3);
			}
			pLayer2=pLayer3;
		}
		pNew->SetAppFlag(0);

		if( !m_pTempDS->AddObject(pNew, pLayer2->GetID()) )
		{
			delete pNew;
		}
	}
	m_pTempDS->RestoreAllQueryFlags();

	if(m_bProgress) GProgressStart(9*inArr.GetSize());
	//线串化, Progress: 1*nSum
	Linearize(arr);	
	//节点咬合, Progress: 1*nSum
	SnapVertexes(arr);
	//消除悬挂点, Progress: 1*nSum
	ProcessSuspend(arr);
	//打断自相交线, Progress: 2*nSum
	CutLines_self(arr);
	//打断相交线, Progress: 2*nSum
	CutLines(arr);
	//剔除公共边, Progress: 1*nSum
	DeleteCommonLine(arr);
	if(m_bProgress) GProgressEnd();
	m_pTempDS->RestoreAllQueryFlags();
	
	//构面
	build_surface();

	for(int j=0; j<arr.GetSize(); j++)
	{
		int nObj = arr[j]->GetObjectCount();
		for(i=0; i<nObj; i++)
		{
			CFeature* pFtr = arr[j]->GetObject(i);
			if(pFtr && pFtr->GetAppFlag()>0 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if(pNewFtr)
				{
					if(arr[j]==pLayer)
						pNewFtr->SetAppFlag(0);
					else
						pNewFtr->SetAppFlag(arr[j]->GetID());
					outArr.Add(pNewFtr);
				}
			}
		}
	}
}

int CTopoSurfaceNoBreakCommand::CopyDataSource(CFtrLayerArray& layers0, CFtrLayerArray& layers1)
{
	int i=0;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return 0;

	//新建CDataQueryEx
	m_pDQ = new CDataQueryEx();
	m_pTempDS = new CDlgDataSource(m_pDQ);

	//写入工作区信息
	m_pTempDS->SetScale(pDS->GetScale());
	PT_3D pts[4];
	double zmin,zmax;
	pDS->GetBound(pts,&zmin,&zmax);
	m_pTempDS->SetBound(pts,zmin,zmax);	

	CValueTable tab;
	CFeature *pFtr = NULL,*pFtr0 = NULL;
	//拷贝需要处理的图层
	for(i=0; i<layers0.GetSize(); i++)
	{
		CFtrLayer *pLayer=NULL;
		pLayer = m_pTempDS->GetFtrLayer(layers0[i]->GetName());
		if(!pLayer)
		{
			pLayer = new CFtrLayer;
			if (!pLayer) continue;
			tab.DelAll();
			tab.BeginAddValueItem();
			layers0[i]->WriteTo(tab);
			tab.EndAddValueItem();
			pLayer->ReadFrom(tab);
			pLayer->SetID(layers0[i]->GetID());
			pLayer->SetName(layers0[i]->GetName());
			m_pTempDS->AddFtrLayer(pLayer);
		}

		int nObj = layers0[i]->GetObjectCount();		
		for (int j=0;j<nObj;j++)
		{
			pFtr = layers0[i]->GetObject(j);
			if (pFtr)
			{
				pFtr0 = pFtr->Clone();
				pFtr0->SetID(OUID());
				pFtr0->SetAppFlag(0);
				if (!m_pTempDS->AddObject(pFtr0, pLayer->GetID()))
				{
					delete pFtr0;
					continue;
				}
			}
		}
	}
	m_pTempDS->RestoreAllQueryFlags();

	//统计要处理的地物数量
	int nSum=0;
	for( i=0; i<m_pTempDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = m_pTempDS->GetFtrLayerByIndex(i);
		if( !pLayer )continue;
		if( pLayer->IsDeleted() || !pLayer->IsVisible() || pLayer->IsLocked() )
			continue;

		layers1.Add(pLayer);
		nSum += pLayer->GetValidObjsCount();
	}

	//临时数据源中加入目标层
	CFtrLayer *out_layer = m_pTempDS->GetFtrLayer((LPCTSTR)m_StoreLayer);
	if(out_layer == NULL)
	{
		out_layer=new CFtrLayer();
		out_layer->SetName((LPCTSTR)m_StoreLayer);
		m_pTempDS->AddFtrLayer(out_layer);
	}
	out_layer->SetID(m_Layer_ID);
	
	return nSum;
}

extern bool IsSurfaceInclude(CFeature* pFtr0, CFeature* pFtr1);

void CTopoSurfaceNoBreakCommand::SurfaceToCurve(CFtrLayerArray& layers0)
{
	CStringArray m_ReplaceLayerstrs;
	convertStringToStrArray(m_ReplaceLayer, m_ReplaceLayerstrs);
	CUndoFtrs undo(m_pEditor,"SurfaceToCurve");
	int nLayer = layers0.GetSize();
	for (int i=0; i<nLayer;i++)
	{
		CFtrLayer *pLayer = layers0[i];
		int nObj = pLayer->GetObjectCount();
		for(int j=0; j<nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr) continue;
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				continue;
			}

			CArray<PT_3DEX,PT_3DEX> pts, pts1;
			pGeo->GetShape(pts);
			int nsize = pts.GetSize();
			for(int k=0; k<=nsize; k++)
			{
				if(k==nsize || (k!=0 && pts[k].pencode==penMove) )
				{
					CValueTable tab;
					tab.BeginAddValueItem();
					pFtr->WriteTo(tab);
					tab.EndAddValueItem();
					
					tab.DelField(FIELDNAME_SHAPE);
					tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
					tab.DelField(FIELDNAME_FTRDELETED);
					tab.DelField(FIELDNAME_FTRID);
					tab.DelField(FIELDNAME_GEOCLASS);
					tab.DelField(FIELDNAME_FTRDISPLAYORDER);
					tab.DelField(FIELDNAME_FTRGROUPID);
					tab.DelField(FIELDNAME_SYMBOLNAME);
				
					CFeature *pNew = pFtr->Clone();
					if (!pNew) continue;

					if (!pNew->CreateGeometry(CLS_GEOCURVE)) continue;

					pNew->ReadFrom(tab);

					pts1[0].pencode = penLine;
					pNew->GetGeometry()->CreateShape(pts1.GetData(),pts1.GetSize());
					
					if (!m_pEditor->AddObject(pNew,pLayer->GetID()))
					{
						delete pNew;
						continue;
					}

					for(int m=0; m<m_ReplaceLayerstrs.GetSize(); m++)
					{
						if (m_ReplaceLayerstrs[m].CompareNoCase(pLayer->GetName()) == 0)
						{
							pNew->SetAppFlag(-2);//标记不重构的面
							break;
						}
					}
					
					GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNew);
					undo.AddNewFeature(FTR_HANDLE(pNew));
					pts1.RemoveAll();
				}

				if(k==nsize) break;
				
				pts1.Add(pts[k]);
			}
			undo.AddOldFeature(FtrToHandle(pFtr));			
			m_pEditor->DeleteObject(FtrToHandle(pFtr));
		}
	}
	undo.Commit();
}

void CTopoSurfaceNoBreakCommand::ReplaceLittleSurface(CUndoFtrs& undo)
{
	if(!m_pEditor) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	CFtrLayer *pLayer = pDS->GetFtrLayer(m_StoreLayer);
	if(!pLayer) return;
	
	pDS->SaveAllQueryFlags(TRUE, FALSE);
	pLayer->SetAllowQuery(TRUE);
	
	CFtrLayerArray arr;
	pDS->GetFtrLayersByNameOrCode_editable(m_ReplaceLayer,arr);
	int i, j, k;
	for(i=0; i<arr.GetSize(); i++)
	{
		arr[i]->SetAllowQuery(TRUE);
		int nObj = arr[i]->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = arr[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;

			if(pFtr->GetAppFlag()!=-2)
				continue;
			pFtr->SetAppFlag(0);
			
			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				continue;
			if( !((CGeoCurveBase*)pGeo)->IsClosed() )
				continue;
			
			CArray<PT_3DEX,PT_3DEX> pts, newPts;
			pGeo->GetShape(pts);
			
			CGeoSurface *pSurface = new CGeoSurface;
			if(!pSurface->CreateShape(pts.GetData(), pts.GetSize()))
			{
				delete pSurface;
				continue;
			}
			CFeature *pFtr1 = pFtr->Clone();
			pSurface->EnableFillColor(FALSE,0);
			pFtr1->SetGeometry(pSurface);
			Envelope e = pSurface->GetEnvelope();
			int num = m_pEditor->GetDataQuery()->FindObjectInRect(e,NULL,FALSE,FALSE);
			const CPFeature *ftrs = m_pEditor->GetDataQuery()->GetFoundHandles(num);

			for(int k=0; k<num; k++)
			{
				if(ftrs[k]==pFtr1) continue;
				CGeometry *pObj = ftrs[k]->GetGeometry();
				if( !pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				{
					continue;
				}
				if(IsSurfaceInclude(pFtr1, ftrs[k]))
				{
					m_pEditor->DeleteObject(FtrToHandle(ftrs[k]), FALSE);
					undo.AddOldFeature(FtrToHandle(ftrs[k]));
				}
			}
			
			pFtr1->SetPurpose(FTR_EDB);
			pFtr1->SetAppFlag(0);
			m_pEditor->AddObject(pFtr1, arr[i]->GetID());
			undo.AddNewFeature(FtrToHandle(pFtr1));
			pDS->GetXAttributesSource()->CopyXAttributes(pFtr, pFtr1);
		}
		arr[i]->SetAllowQuery(FALSE);
	}
	pDS->RestoreAllQueryFlags();
}

void CTopoSurfaceNoBreakCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;
	if(m_StoreLayer.IsEmpty()) return;
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDQ) return;

	if(m_DeleteExistFace)
	{
		CTopoDelSurfaceCommand cmd;
		cmd.Init(m_pEditor);
		cmd.PtClick(pt,flag);
	}

	CFtrLayer *out_layer=NULL;
	out_layer=pDS->GetFtrLayer((LPCTSTR)m_StoreLayer);
	if(out_layer==NULL)
	{
		out_layer=new CFtrLayer();
		out_layer->SetName((LPCTSTR)m_StoreLayer);
		m_pEditor->AddFtrLayer(out_layer);
	}
	m_Layer_ID=out_layer->GetID();

	CFtrLayerArray layers0;//主数据源的层
	if( m_HandleLayer.IsEmpty() )
	{
		for(int i=0; i<pDS->GetFtrLayerCount(); i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			if( !pLayer )continue;
			if( pLayer->IsDeleted() || !pLayer->IsVisible() || pLayer->IsLocked() )
				continue;
			
			if(m_StoreLayer == pLayer->GetName() )
				continue;
			
			if( StrFromResID(IDS_MARKLAYER_NAME).CompareNoCase(pLayer->GetName())==0 )
			{
				continue;
			}
			
			if(pLayer->GetObjectCount()>0)
				layers0.Add(pLayer);
		}
	}
	else
	{
		pDS->GetFtrLayersByNameOrCode(m_HandleLayer,layers0);
	}

	//面转为线
	SurfaceToCurve(layers0);

	//复制数据源
	CFtrLayerArray layers1;//保存临时数据源的层
	int nSum = CopyDataSource(layers0, layers1);
	if(nSum==0) return;

	//拓扑预处理
	GProgressStart(9*nSum);
	//线串化, Progress: 1*nSum
	Linearize(layers1);	
	//节点咬合, Progress: 1*nSum
	SnapVertexes(layers1);
	//消除悬挂点, Progress: 1*nSum
	ProcessSuspend(layers1);
	//打断自相交线, Progress: 2*nSum
	CutLines_self(layers1);
	//打断相交线, Progress: 2*nSum
	CutLines(layers1);
	//剔除公共边, Progress: 1*nSum
	DeleteCommonLine(layers1);
	GProgressEnd();
	m_pTempDS->RestoreAllQueryFlags();

	//构面
	build_surface();

	//取出结果存入主数据源
	CUndoFtrs undo(m_pEditor,Name());
	int nlayer = m_pTempDS->GetFtrLayerCount(), i;
	nSum = 0;
	for(i=0; i<nlayer; i++)
	{
		CFtrLayer *pLayer = m_pTempDS->GetFtrLayerByIndex(i);
		if(!pLayer) continue;
		nSum += pLayer->GetObjectCount();
	}
	GProgressStart(nSum);
	for(i=0; i<nlayer; i++)
	{
		CFtrLayer *pLayer = m_pTempDS->GetFtrLayerByIndex(i);
		if(!pLayer) continue;

		CFeature *pFtrTempl = out_layer->CreateDefaultFeature(((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOSURFACE);
		if(!pFtrTempl)
			return;

		int nObj = pLayer->GetObjectCount();
		for(int j=0; j<nObj; j++)
		{
			GProgressStep();
			CFeature* pFtr = pLayer->GetObject(j);
			if(!pFtr || pFtr->GetAppFlag()<=0 )
				continue;

			//如果被已存在的面包含(包括完全重叠)，则新建的面不添加
// 			Envelope e;
// 			CArray<PT_3DEX,PT_3DEX> arrPts;
// 			pFtr->GetGeometry()->GetShape(arrPts);
// 			e.CreateFromPts(arrPts.GetData(), arrPts.GetSize(), sizeof(PT_3DEX));
// 			int num = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
// 			const CPFeature *ftrs = pDQ->GetFoundHandles(num);
// 			BOOL bIncluded = FALSE;
// 			for(k=0; k<num; k++)
// 			{
// 				CGeometry *pObj = ftrs[k]->GetGeometry();
// 				if( !pObj || !pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
// 				{
// 					continue;
// 				}
// 				if(IsSurfaceInclude(ftrs[k], pFtr))
// 				{
// 					bIncluded = TRUE;
// 					break;
// 				}
// 			}
// 			if(bIncluded)
// 			{
// 				continue;
// 			}

			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo || pGeo->GetDataPointSum()<4)
			{
				continue;
			}

			CFeature *pFtr0 = pFtrTempl->Clone();
			pFtr0->SetID(OUID());
			pFtr0->SetAppFlag(0);
			pFtr0->SetGeometry(pGeo->Clone());
			pFtr0->SetPurpose(FTR_EDB);
			if( !m_pEditor->AddObject(pFtr0,pFtr->GetAppFlag()) )
			{
				delete pFtr0;
				continue;
			}
			undo.AddNewFeature(FtrToHandle(pFtr0));
		}
		
		delete pFtrTempl;
	}
	GProgressEnd();
	undo.Commit();

	if(!m_ReplaceLayer.IsEmpty())
	{
		ReplaceLittleSurface(undo);//采集面不重构
	}

	if(m_bFillAtt)
	{
		CFillSurfaceATTFromPtCommand *pFillAttCommand;
		pFillAttCommand = new CFillSurfaceATTFromPtCommand();
		pFillAttCommand->m_HandleLayer = m_StoreLayer;
		pFillAttCommand->Init(m_pEditor);
		pFillAttCommand->m_bDelSurfacePt = m_bDelSurfacePt;
		PT_3D pt;
		pFillAttCommand->PtClick(pt,0);
		delete pFillAttCommand;
	}

	AfxGetMainWnd()->SendMessage(WM_COMMAND, (WPARAM)ID_FILL_RAND_COLOR, 0);
	
	Finish();
	CEditCommand::PtClick(pt,flag);
}


BOOL CTopoSurfaceNoBreakCommand::Linearize(CFtrLayerArray& layers0)
{
	int i, j, k;

	double r = m_lfToler;
	
	//1、线串化 + 闭合点精确化处理 + 双线打散、复杂面打散、面转线
	for( i=0; i<layers0.GetSize(); i++)
	{
		int nObj = layers0[i]->GetObjectCount();
		for( j=0; j<nObj; j++)
		{
			CFeature *pFtr = layers0[i]->GetObject(j);
			if( !pFtr )continue;

			if(m_bProgress) GProgressStep();
			
			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			{
				continue;
			}

			CArray<PT_3DEX,PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			
			//本身是折线，就不线串化
			for(k=0; k<arrPts.GetSize(); k++)
			{
				int pencode = arrPts[k].pencode;
				if( pencode==penArc || pencode==pen3PArc || pencode==penSpline || pencode==penStream)
					break;
			}
			
			if( k<arrPts.GetSize() )
			{								
				CGeometry *pGeo2 = pGeo->Linearize();
				if( pGeo2 )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts2;
					pGeo2->GetShape(arrPts2);

					m_pTempDS->DeleteObject(pFtr);
					pGeo->CreateShape(arrPts2.GetData(),arrPts2.GetSize());
					
					m_pTempDS->RestoreObject(pFtr);
					delete pGeo2;
				}
			}

			pGeo->GetShape(arrPts);

			int npt = arrPts.GetSize();
			if(npt<2) continue;
			double dis = GraphAPI::GGet2DDisOf2P(arrPts[0],arrPts[npt-1]);
			if( dis>0 && dis<r )
			{
				arrPts[npt-1].x = arrPts[0].x;
				arrPts[npt-1].y = arrPts[0].y;

				m_pTempDS->DeleteObject(pFtr);

				pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());

				m_pTempDS->RestoreObject(pFtr);
			}

			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) ||
				pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			{
				CGeoArray geos;

				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
				{
					CGeometry *pGeo1=NULL, *pGeo2=NULL;
					((CGeoDCurve*)pGeo)->Separate(pGeo1,pGeo2);
					if( pGeo1 && pGeo2 )
					{
						geos.Add(pGeo1);
						geos.Add(pGeo2);
					}
					else
					{
						if( pGeo1 )delete pGeo1;
						if( pGeo2 )delete pGeo2;
					}
				}
				else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
				{
					CGeometry *pGeo1=NULL, *pGeo2=NULL;
					((CGeoParallel*)pGeo)->Separate(pGeo1,pGeo2);
					if( pGeo1 && pGeo2 )
					{
						geos.Add(pGeo1);
						geos.Add(pGeo2);
					}
					else
					{
						if( pGeo1 )delete pGeo1;
						if( pGeo2 )delete pGeo2;
					}
				}
				else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) )
				{
					CGeoMultiSurface *pMGeo = (CGeoMultiSurface*)pGeo;
					int nSurface = pMGeo->GetSurfaceNum();
					for( int m=0; m<nSurface; m++)
					{
						CArray<PT_3DEX,PT_3DEX> arrPts2;
						pMGeo->GetSurface(m,arrPts2);
						if( arrPts2.GetSize()<4 )
							continue;
						arrPts2[0].pencode = penLine;

						CGeoCurve *pNewObj = new CGeoCurve();
						pNewObj->CopyFrom(pMGeo);
						if( pNewObj->CreateShape(arrPts2.GetData(),arrPts2.GetSize()) )
						{
							geos.Add(pNewObj);
						}
						else
						{
							delete pNewObj;
						}
					}
				}
				else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts2;
					pGeo->GetShape(arrPts2);
					if( arrPts2.GetSize()<4 )
						continue;
					arrPts2[0].pencode = penLine;

					CGeoCurve *pNewObj = new CGeoCurve();
					pNewObj->CopyFrom(pGeo);
					if( pNewObj->CreateShape(arrPts2.GetData(),arrPts2.GetSize()) )
					{
						geos.Add(pNewObj);
					}
					else
					{
						delete pNewObj;
					}
				}

				for( int m=0; m<geos.GetSize(); m++)
				{
					CFeature *pNewFtr = pFtr->Clone();
					pNewFtr->SetGeometry(geos[m]);

					m_pTempDS->AddObject(pNewFtr,layers0[i]->GetID());

				}

				if( geos.GetSize()>0 )
				{
					m_pTempDS->DeleteObject(pFtr);
				}
			}
		}
	}
	return TRUE;
}

//逻辑与线线悬挂处理中的功能类似
BOOL CTopoSurfaceNoBreakCommand::ProcessSuspend(CFtrLayerArray& layers0)
{	
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
	
	double r = m_lfToler;

	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			if(m_bProgress) GProgressStep();
			
			CGeometry *pGeo = pFtr->GetGeometry();
			if(pGeo && pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && !((CGeoCurveBase*)pGeo)->IsClosed() )
			{
				CGeoCurveBase *pGeoCurve = (CGeoCurveBase*)pGeo;
				CArray<PT_3DEX,PT_3DEX> arrPts; 
				pGeoCurve->GetShape(arrPts);
				
				if( arrPts.GetSize()<2 )
					continue;
				
				Envelope e;
				e.CreateFromPtAndRadius(arrPts[0],r);
				int num = m_pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
				
				double min_dis = -1;
				const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);
				
				for( j=0; j<num; j++)
				{
					if( pFtr==ftrs[j] )
						continue;
					
					double dis = CalcSnapDistance(pFtr,ftrs[j],0);
					if( min_dis<0 || min_dis>dis )
					{
						k = j;
						min_dis = dis;
					}				
				}
				
				if( k>=0 )
					ProcessOneFeature(pFtr,ftrs[k],0);
				
				int index = arrPts.GetSize()-1;
				e.CreateFromPtAndRadius(arrPts[index],r);
				num = m_pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
				
				min_dis = -1;
				k = -1;
				ftrs = m_pDQ->GetFoundHandles(num);
				
				for( j=0; j<num; j++)
				{				
					if( pFtr==ftrs[j] )
						continue;
					
					double dis = CalcSnapDistance(pFtr,ftrs[j],index);
					if( min_dis<0 || min_dis>dis )
					{
						k = j;
						min_dis = dis;
					}				
				}
				
				if( k>=0 )
					ProcessOneFeature(pFtr,ftrs[k],index);
			}
		}
	}
	
	return TRUE;
}

BOOL CTopoSurfaceNoBreakCommand::CutLines_self(CFtrLayerArray& layers0)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}

	//计算所有的交点，并将交点信息放入数组
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			j++;
		}
	}

	ftrsArray.SetSize(j);

	IntersectItemArray arr;

	for( i=0; i<j; i++)
	{
		if(m_bProgress) GProgressStep();
		GetIntersects_self(ftrsArray[i],arr);
	}

	if( arr.GetSize()<=1 )
		return TRUE;

	//按照地物和交点位置的顺序对交点排序
	qsort(arr.GetData(),arr.GetSize(),sizeof(IntersectItem),compare_intersect_item);

	IntersectItem *data = arr.GetData();
	nSum = arr.GetSize();

	//将重复的清除，
	for( i=1; i<arr.GetSize(); i++)
	{
		if( data[i].pFtr==data[i-1].pFtr && fabs(data[i].t-data[i-1].t)<1e-8 && data[i].ptIdx==data[i-1].ptIdx )
			data[i] = IntersectItem();
	}

	//刚好为连续顶点的，就取其头尾，使得它们能成为连续的一段
	int clear_start = 0, clear_stop = 0;
	for( i=1; i<arr.GetSize(); i++)
	{
		if( data[i].pFtr==NULL )
			continue;

		if( data[i].pFtr==data[i-1].pFtr && fabs(data[i].t)<1e-8 && fabs(data[i-1].t)<1e-8 && data[i].ptIdx==(data[i].ptIdx+1) )
			clear_stop++;
		else
		{
			if( clear_stop-clear_start>1 )
			{
				for( k=clear_start+1; k<clear_stop; k++)
				{
					data[k] = IntersectItem();
				}
			}

			clear_stop = clear_start = i;
		}
	}

	//将空交点删除
	for( i=arr.GetSize()-1; i>=0; i--)
	{
		if( arr[i].pFtr==NULL )
			arr.RemoveAt(i);
	}

	//对每个地物依次处理：用该地物的N个交点将地物打断成N+1份
	data = arr.GetData();
	nSum = arr.GetSize();

	int startItem = 0, stopItem = -1;
	for( i=0; i<nSum; i++)
	{
		if( i!=(nSum-1) && data[i].pFtr==data[i+1].pFtr )
			continue;

		stopItem = i+1;

		CFeature *pFtr = data[startItem].pFtr;
		CGeometry *pGeo = pFtr->GetGeometry();
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);

		//处理start~stop之间的交点
		int ptIndex0 = 0, ptIndex1 = 0;
		for( j=startItem; j<stopItem; j++)
		{
			ptIndex1 = data[j].ptIdx;
			ptIndex1 = pShp->GetKeyPos(ptIndex1);

			//在 ptIndex1+1 处插入顶点，取 ptIndex0 ~ ptIndex1 之间的点构成新地物
			
			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);
			pts2.InsertAt(ptIndex1+1,PT_3DEX(data[j].pt,penLine));
			
			if( j>startItem )
			{
				pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			}

			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+2-ptIndex0);

			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pTempDS->AddObject(pNewFtr,m_pTempDS->GetFtrLayerOfObject(pFtr)->GetID());
				}
				else
				{
					delete pNewFtr;
				}
			}

			ptIndex0 = ptIndex1;
		}

		//最后一段
		if( stopItem>startItem )
		{
			ptIndex1 = pts.GetSize()-1;

			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);			
			pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			
			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+1-ptIndex0);
			
			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pTempDS->AddObject(pNewFtr,m_pTempDS->GetFtrLayerOfObject(pFtr)->GetID());					
				}
				else
				{
					delete pNewFtr;
				}
			}
		}

		m_pTempDS->DeleteObject(pFtr);

		startItem = stopItem;
	}

	return TRUE;
}


BOOL CTopoSurfaceNoBreakCommand::CutLines(CFtrLayerArray& layers0)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}

	//计算所有的交点，并将交点信息放入数组		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}

	ftrsArray.SetSize(j);

	IntersectItemArray arr;

	for( i=0; i<j; i++)
	{
		if(m_bProgress) GProgressStep();
		
		Envelope e = ftrsArray[i]->GetGeometry()->GetEnvelope();
		e.Inflate(r,r,0);
		int num = m_pDQ->FindObjectInRect_SimpleAndSym(e,NULL,FALSE,FALSE);
		
		const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);
		
		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()<=i )continue;

			GetIntersects(ftrsArray[i],ftrs[k],arr);
		}
/*
		for( k=i+1; k<j; k++)
		{
			GetIntersects(ftrsArray[i],ftrsArray[k],arr);
		}
*/
	}

	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}

	if( arr.GetSize()<=1 )
		return TRUE;

	//按照地物和交点位置的顺序对交点排序
	qsort(arr.GetData(),arr.GetSize(),sizeof(IntersectItem),compare_intersect_item);
	
	//将重复的清除，
	for( i=arr.GetSize()-1; i>0; i--)
	{
		if( arr[i].pFtr==arr[i-1].pFtr && fabs(arr[i].t-arr[i-1].t)<1e-8 && arr[i].ptIdx==arr[i-1].ptIdx  )
			arr.RemoveAt(i);
	}
	
	IntersectItem *data = arr.GetData();
	nSum = arr.GetSize();
/*	
	//刚好为连续顶点的，就取其头尾，使得它们能成为连续的一段
	int clear_start = 0, clear_stop = 0;
	for( i=1; i<nSum; i++)
	{		
		if( data[i].pFtr==data[i-1].pFtr && fabs(data[i].t)<1e-8 && fabs(data[i-1].t)<1e-8 && data[i].ptIdx==(data[i-1].ptIdx+1) )
		{
			clear_stop++;

			if( i!=(nSum-1) )
				continue;
		}
		
		if( clear_stop-clear_start>1 )
		{
			for( k=clear_start+1; k<clear_stop; k++)
			{
				data[k] = IntersectItem();
			}
		}
		
		clear_stop = clear_start = i;
	}
*/	
	//将空交点删除
	for( i=nSum-1; i>=0; i--)
	{
		if( arr[i].pFtr==NULL )
			arr.RemoveAt(i);
	}

	//对每个地物依次处理：用该地物的N个交点将地物打断成N+1份
	data = arr.GetData();
	nSum = arr.GetSize();

	int startItem = 0, stopItem = -1;
	for( i=0; i<nSum; i++)
	{
		if(m_bProgress) GProgressStep();

		if( i!=(nSum-1) && data[i].pFtr==data[i+1].pFtr )
			continue;

		stopItem = i+1;

		CFeature *pFtr = data[startItem].pFtr;
		CGeometry *pGeo = pFtr->GetGeometry();
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);

		//处理start~stop之间的交点
		int ptIndex0 = 0, ptIndex1 = 0;
		for( j=startItem; j<stopItem; j++)
		{
			ptIndex1 = data[j].ptIdx;
			ptIndex1 = pShp->GetKeyPos(ptIndex1);

			//在 ptIndex1+1 处插入顶点，取 ptIndex0 ~ ptIndex1 之间的点构成新地物
			
			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);
			pts2.InsertAt(ptIndex1+1,PT_3DEX(data[j].pt,penLine));
			
			if( j>startItem )
			{
				pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			}

			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+2-ptIndex0);

			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pTempDS->AddObject(pNewFtr,m_pTempDS->GetFtrLayerOfObject(pFtr)->GetID());
				}
				else
				{
					delete pNewFtr;
				}
			}

			ptIndex0 = ptIndex1;
		}

		//最后一段
		if( stopItem>startItem )
		{
			ptIndex1 = pts.GetSize()-1;

			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);			
			pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			
			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+1-ptIndex0);
			
			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pTempDS->AddObject(pNewFtr,m_pTempDS->GetFtrLayerOfObject(pFtr)->GetID());
				}
				else
				{
					delete pNewFtr;
				}
			}
		}

		m_pTempDS->DeleteObject(pFtr);

		startItem = stopItem;
	}

	return TRUE;
}

void CTopoSurfaceNoBreakCommand::GetIntersects(CFeature* f1, CFeature* f2, IntersectItemArray &arr)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();

	const CShapeLine *pShp1 = pGeo1->GetShape();
	const CShapeLine *pShp2 = pGeo2->GetShape();

	if( !pShp1 || !pShp2 )
		return;

	PT_3D pt1,pt2,line[2],ret;
	double mindis = -1;
	
	int nIdx1=-1,nIdx2=-1;
	int nBaseIdx1 = 0, nBaseIdx2 = 0;
	const CShapeLine::ShapeLineUnit *pList1 = pShp1->HeadUnit();
	while (pList1!=NULL)
	{
		nIdx1++;

		Envelope e = pList1->evlp;
		e.m_xl -= 1e-6; e.m_xh += 1e-6;
		e.m_yl -= 1e-6; e.m_yh += 1e-6;

		nBaseIdx2 = 0;
		const CShapeLine::ShapeLineUnit *pList2 = pShp2->HeadUnit();
		nIdx2 = -1;
		while(pList2!=NULL)
		{
			nIdx2++;
			if(e.bIntersect(&(pList2->evlp)))
			{
				for (int i=0;i<pList1->nuse-1;i++)
				{
					if( pList1->pts[i+1].pencode==penMove )
						continue;

					COPY_3DPT(line[0],pList1->pts[i]);
					COPY_3DPT(line[1],pList1->pts[i+1]);
					
					Envelope e0 = pList2->evlp;
					e0.m_xl -= 1e-4; e0.m_xh += 1e-4;
					e0.m_yl -= 1e-4; e0.m_yh += 1e-4;
					if(!e0.bIntersect(line,line+1))continue;

					for (int j=0;j<pList2->nuse-1;j++)
					{
						if( pList2->pts[j+1].pencode==penMove )
							continue;

						//为了优化，先判断一下
						double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;
						
						if( line[0].x<line[1].x ){ xmin1 = line[0].x; xmax1 = line[1].x; }
						else { xmin1 = line[1].x; xmax1 = line[0].x; }
						if( line[0].y<line[1].y ){ ymin1 = line[0].y; ymax1 = line[1].y; }
						else { ymin1 = line[1].y; ymax1 = line[0].y; }
						
						if( pList2->pts[j].x<pList2->pts[j+1].x ){ xmin2 = pList2->pts[j].x; xmax2 = pList2->pts[j+1].x; }
						else { xmin2 = pList2->pts[j+1].x; xmax2 = pList2->pts[j].x; }
						if( pList2->pts[j].y<pList2->pts[j+1].y ){ ymin2 = pList2->pts[j].y; ymax2 = pList2->pts[j+1].y; }
						else { ymin2 = pList2->pts[j+1].y; ymax2 = pList2->pts[j].y; }
						
						if( xmax1<xmin2-1e-4 || xmax2<xmin1-1e-4 || 
							ymax1<ymin2-1e-4 || ymax2<ymin1-1e-4 )
							continue;

						double xt,yt,xt2,yt2;
						double t0=0,t1=0,t2=0,t3=0;
						bool twoIntersect = false;
						if(GGetLineIntersectLineSeg_withOverlap(line[0].x,line[0].y,line[1].x,line[1].y,pList2->pts[j].x,pList2->pts[j].y,pList2->pts[j+1].x,pList2->pts[j+1].y,
							&xt,&yt,&t0,&t1,
							twoIntersect,
							&xt2,&yt2,&t2,&t3))
						{	
							if(t0<-0.01 || t1<-0.01 || t2<-0.01||t3<-0.01)
							{
								//不应该得到这样的结果
								GGetLineIntersectLineSeg_withOverlap(line[0].x,line[0].y,line[1].x,line[1].y,pList2->pts[j].x,pList2->pts[j].y,pList2->pts[j+1].x,pList2->pts[j+1].y,
									&xt,&yt,&t0,&t1,
									twoIntersect,
									&xt2,&yt2,&t2,&t3);
								int a=1;	
							}
							IntersectItem item0,item1;
							item0.pFtr = f1;
							item0.pt.x = xt;
							item0.pt.y = yt;
							item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
							item0.t = t0;
							item0.ptIdx = nBaseIdx1 + i;

							if( (line[0].x-xt)*(line[0].x-xt)+(line[0].y-yt)*(line[0].y-yt)<1e-8 )
							{
								item0.t = 0;
							}

							if( (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
							{
								item0.ptIdx++;
								item0.t = 0;
							}

							item1.pFtr = f2;
							item1.pt.x = xt;
							item1.pt.y = yt;
							item1.pt.z = pList2->pts[j].z + t1 * (pList2->pts[j+1].z-pList2->pts[j].z);
							item1.t = t1;
							item1.ptIdx = nBaseIdx2 + j;

							if( (pList2->pts[j].x-xt)*(pList2->pts[j].x-xt)+(pList2->pts[j].y-yt)*(pList2->pts[j].y-yt)<1e-8 )
							{
								item1.t = 0;
							}

							if( (pList2->pts[j+1].x-xt)*(pList2->pts[j+1].x-xt)+(pList2->pts[j+1].y-yt)*(pList2->pts[j+1].y-yt)<1e-8 )
							{
								item1.ptIdx++;
								item1.t = 0;
							}

							arr.Add(item0);
							arr.Add(item1);

							if(twoIntersect)
							{
								item0 = IntersectItem();
								item1 = IntersectItem();
								xt = xt2; yt = yt2;
								t0 = t2; t1 = t3;
								
								item0.pFtr = f1;
								item0.pt.x = xt;
								item0.pt.y = yt;
								item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
								item0.t = t0;
								item0.ptIdx = nBaseIdx1 + i;

								if( (line[0].x-xt)*(line[0].x-xt)+(line[0].y-yt)*(line[0].y-yt)<1e-8 )
								{
									item0.t = 0;
								}

								if( (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
								{
									item0.ptIdx++;
									item0.t = 0;
								}

								item1.pFtr = f2;
								item1.pt.x = xt;
								item1.pt.y = yt;
								item1.pt.z = pList2->pts[j].z + t1 * (pList2->pts[j+1].z-pList2->pts[j].z);
								item1.t = t1;
								item1.ptIdx = nBaseIdx2 + j;

								if( (pList2->pts[j].x-xt)*(pList2->pts[j].x-xt)+(pList2->pts[j].y-yt)*(pList2->pts[j].y-yt)<1e-8 )
								{
									item1.t = 0;
								}

								if( (pList2->pts[j+1].x-xt)*(pList2->pts[j+1].x-xt)+(pList2->pts[j+1].y-yt)*(pList2->pts[j+1].y-yt)<1e-8 )
								{
									item1.ptIdx++;
									item1.t = 0;
								}

								arr.Add(item0);
								arr.Add(item1);
							}
						}
					}
				}				
			}
			nBaseIdx2 += pList2->nuse;
			pList2 = pList2->next;
		}		

		nBaseIdx1 += pList1->nuse;
		pList1 = pList1->next;
	}
}



void CTopoSurfaceNoBreakCommand::GetIntersects_self(CFeature* f1, IntersectItemArray &arr)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	
	const CShapeLine *pShp1 = pGeo1->GetShape();
	
	if( !pShp1 )
		return;

	BOOL bClosed = FALSE;
	if( pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
	{
		if( ((CGeoCurveBase*)pGeo1)->IsClosed() )
		{
			bClosed = TRUE;
		}
	}

	CArray<PT_3DEX,PT_3DEX> pts;
	pShp1->GetPts(pts);
	KickoffSame2DPoints_notPenMove(pts);

	PT_3D pt1,pt2,line[4],ret;

	int npt = pts.GetSize();
	PT_3DEX *buf = pts.GetData();
	for( int i=0; i<npt-1; i++)
	{
		if( buf[i+1].pencode==penMove )
			continue;

		for( int j=i+2; j<npt-1; j++)
		{
			if( buf[j+1].pencode==penMove )
				continue;

			if( bClosed && i==0 && j==(npt-2) )
				continue;

			COPY_3DPT(line[0],buf[i]);
			COPY_3DPT(line[1],buf[i+1]);
			
			COPY_3DPT(line[2],buf[j]);
			COPY_3DPT(line[3],buf[j+1]);

			//为了优化，先判断一下
			double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;
			
			if( line[0].x<line[1].x ){ xmin1 = line[0].x; xmax1 = line[1].x; }
			else { xmin1 = line[1].x; xmax1 = line[0].x; }
			if( line[0].y<line[1].y ){ ymin1 = line[0].y; ymax1 = line[1].y; }
			else { ymin1 = line[1].y; ymax1 = line[0].y; }

			if( line[2].x<line[3].x ){ xmin2 = line[2].x; xmax2 = line[3].x; }
			else { xmin2 = line[3].x; xmax2 = line[2].x; }
			if( line[2].y<line[3].y ){ ymin2 = line[2].y; ymax2 = line[3].y; }
			else { ymin2 = line[3].y; ymax2 = line[2].y; }

			if( xmax1<xmin2-1e-6 || xmax2<xmin1-1e-6 || 
				ymax1<ymin2-1e-6 || ymax2<ymin1-1e-6 )
				continue;

			if( (line[0].x-line[1].x)*(line[0].x-line[1].x) + (line[0].y-line[1].y)*(line[0].y-line[1].y)<1e-8 )
				continue;

			if( (line[2].x-line[3].x)*(line[2].x-line[3].x) + (line[2].y-line[3].y)*(line[2].y-line[3].y)<1e-8 )
				continue;

			double xt,yt;
			double t0,t1;
			if(GraphAPI::GGetLineIntersectLineSeg(line[0].x,line[0].y,line[1].x,line[1].y,line[2].x,line[2].y,line[3].x,line[3].y,&xt,&yt,&t0,&t1))
			{	
				IntersectItem item0,item1;
				item0.pFtr = f1;
				item0.pt.x = xt;
				item0.pt.y = yt;
				item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
				item0.t = t0;
				item0.ptIdx = i;

				if( fabs(1-t0)<1e-6 && (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
				{
					item0.ptIdx++;
					item0.t = 0;
				}
				
				item1.pFtr = f1;
				item1.pt.x = xt;
				item1.pt.y = yt;
				item1.pt.z = line[2].z + t1 * (line[3].z-line[2].z);
				item1.t = t1;
				item1.ptIdx = j;
				
				if( fabs(1-t1)<1e-6 && (line[3].x-xt)*(line[3].x-xt)+(line[3].y-yt)*(line[3].y-yt)<1e-8 )
				{
					item1.ptIdx++;
					item1.t = 0;
				}
				
				arr.Add(item0);
				arr.Add(item1);
			}
			//重叠
			else if( GetOverlapSection_ret_tt(line[0].x,line[0].y,line[1].x,line[1].y,line[2].x,line[2].y,line[3].x,line[3].y,GraphAPI::g_lfDisTolerance,&xt,&yt,&t0,&t1) )
			{
				IntersectItem item0,item1;
				item0.pFtr = f1;
				item0.pt.x = xt;
				item0.pt.y = yt;
				item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
				item0.t = t0;
				item0.ptIdx = i;

				if( fabs(1-t0)<1e-6 && (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
				{
					item0.ptIdx++;
					item0.t = 0;
				}
				
				item1.pFtr = f1;
				item1.pt.x = xt;
				item1.pt.y = yt;
				item1.pt.z = line[2].z + t1 * (line[3].z-line[2].z);
				item1.t = t1;
				item1.ptIdx = j;
				
				if( fabs(1-t1)<1e-6 && (line[3].x-xt)*(line[3].x-xt)+(line[3].y-yt)*(line[3].y-yt)<1e-8 )
				{
					item1.ptIdx++;
					item1.t = 0;
				}
				
				arr.Add(item0);
				arr.Add(item1);				
			}
		}
	}	
}


void CTopoSurfaceNoBreakCommand::GetSnapVertexes(CFeature* f1, CFeature* f2, Envelope evlp, SnapItemArray &arr)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();

	if( !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) || !pGeo2->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		return;
	
	const CShapeLine *pShp1 = pGeo1->GetShape();
	const CShapeLine *pShp2 = pGeo2->GetShape();

	if( !pShp1 || !pShp2 )
		return;

	double toler = m_lfToler;

	Envelope e1 = pGeo1->GetEnvelope();
	e1.Inflate(toler,toler,0);

	BOOL bUseEvlp = (!evlp.IsEmpty());
	double toler2 = toler * toler;
	PT_3DEX pt1,pt2;

	if( !e1.bIntersect(&pGeo2->GetEnvelope()) )
		return;

	if( f1==f2 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeo1->GetShape(arrPts);

		int npt = arrPts.GetSize();
		for( int i=0; i<npt; i++)
		{
			pt1 = arrPts[i];
			if( bUseEvlp && !evlp.bPtIn(&pt1) )
				continue;

			for( int j=i+2; j<npt; j++)
			{
				pt2 = arrPts[j];
				if( bUseEvlp && !evlp.bPtIn(&pt2) )
					continue;

				double dis = (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y);
				
				if( dis<toler2 && dis>1e-8 )
				{
					SnapItem item0;
					item0.pFtr1 = f1;
					item0.pFtr2 = f2;
					item0.pt1 = pt1;
					item0.pt2 = pt2;

					CArray<int,int> arrIndex;
					pShp1->GetKeyPosOfBaseLines(arrIndex);

					item0.ptIdx1 = arrIndex[i];
					item0.ptIdx2 = arrIndex[j];
					/*
					arr.Add(item0);

					item0.pFtr1 = f1;
					item0.pFtr2 = f2;
					item0.pt1 = pt2;
					item0.pt2 = pt1;
					item0.ptIdx1 = arrIndex[j];
					item0.ptIdx2 = arrIndex[i];
					*/
					arr.Add(item0);
				}
			}
		}
		return;
	}
		
	
	PT_3D line[2],ret;
	double mindis = -1;
	
	int nIdx1=-1,nIdx2=-1;
	int nBaseIdx1 = 0, nBaseIdx2 = 0;
	const CShapeLine::ShapeLineUnit *pList1 = pShp1->HeadUnit();
	while (pList1!=NULL)
	{
		nIdx1++;
		const CShapeLine::ShapeLineUnit *pList2 = pShp2->HeadUnit();
		nIdx2 = -1;
		nBaseIdx2 = 0;

		Envelope e = pList1->evlp;
		e.m_xl -= toler; e.m_xh += toler;
		e.m_yl -= toler; e.m_yh += toler;

		if( bUseEvlp && !e.bIntersect(&evlp) )
		{
			nBaseIdx1 += pList1->nuse;
			pList1 = pList1->next;	
			continue;
		}

		while(pList2!=NULL)
		{
			nIdx1++;

			if( e.bIntersect(&(pList2->evlp)) )
			{
				for (int i=0;i<pList1->nuse;i++)
				{
					pt1 = pList1->pts[i];
					if( pt1.pencode==penNone )
						continue;
					
					for (int j=0;j<pList2->nuse;j++)
					{						
						pt2 = pList2->pts[j];

						if( pt2.pencode==penNone )
							continue;

						double dis = (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y);

						if( dis<toler2 && dis>1e-8 )
						{	
							if( !bUseEvlp || (evlp.bPtIn(&pt1) && evlp.bPtIn(&pt2)) )
							{
								SnapItem item0;
								item0.pFtr1 = f1;
								item0.pFtr2 = f2;
								item0.pt1 = pt1;
								item0.pt2 = pt2;
								item0.ptIdx1 = nBaseIdx1 + i;
								item0.ptIdx2 = nBaseIdx2 + j;
								
								arr.Add(item0);
							}
						}
					}
				}				
			}

			nBaseIdx2 += pList2->nuse;
			pList2 = pList2->next;
		}	
		
		nBaseIdx1 += pList1->nuse;
		pList1 = pList1->next;		
	}	
}


BOOL CTopoSurfaceNoBreakCommand::SnapVertexes(CFtrLayerArray& layers0)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}

	ftrsArray.SetSize(j);

	BOOL bMsgbox = FALSE;
	
	//计算所有的匹配点（咬合点），并将匹配点信息放入数组
	SnapItemArray arr;

	for( i=0; i<j; i++)
	{
		if(m_bProgress) GProgressStep();

		Envelope e = ftrsArray[i]->GetGeometry()->GetEnvelope();
		e.Inflate(r,r,0);

		int m, num = m_pDQ->FindObjectInRect_SimpleAndSym(e,NULL,FALSE,FALSE);
		
		const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);
		
		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()<=i )continue;

			SnapItemArray arr1; 
			
			GetSnapVertexes(ftrsArray[i],ftrs[k],Envelope(),arr1);

			if( arr1.GetSize()>100 )
			{
				if( !bMsgbox )
				{
					bMsgbox = TRUE;
					if( AfxMessageBox(IDS_ERROR_MUCHTIME,MB_YESNO)!=IDYES )
					{
						goto EXIT;
					}
				}
			}

/*			//去掉串联的部分（A->B,B->C），避免一个地物的某个节点被多次修改
			for( m=arr.GetSize()-1; m>=0; m--)
			{
				SnapItem item0 = arr[m];
				for( int n=arr1.GetSize()-1; n>=0; n--)
				{
					SnapItem item1 = arr1[n];
					if( (item0.pFtr2==item1.pFtr1 && item0.ptIdx2==item1.ptIdx1) ||
						(item0.pFtr2==item1.pFtr2 && item0.ptIdx2==item1.ptIdx2) )
					{
						arr1.RemoveAt(n);
					}
				}				
			}

			//去掉两个节点同时连接某个节点的情况（A->C,B->C）
			for( m=arr1.GetSize()-1; m>=0; m--)
			{
				SnapItem item0 = arr1[m];
				for( int n=m-1; n>=0; n--)
				{
					SnapItem item1 = arr1[n];
					if( item0.pFtr2==item1.pFtr2 && item0.ptIdx2==item1.ptIdx2 )
					{
						arr1.RemoveAt(m);
						break;
					}
				}				
			}
*/
			arr.Append(arr1);
		}
	}

EXIT:
	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}

	if( arr.GetSize()<=0 )
		return TRUE;

	//按照地物和咬合点位置的顺序对交点排序
	qsort(arr.GetData(),arr.GetSize(),sizeof(SnapItem),compare_snap_item);

	//对每个地物依次处理：将该地物的N个重叠点调整位置
	SnapItem *data = arr.GetData();
	nSum = arr.GetSize();

	int startItem = 0, stopItem = -1;
	for( i=0; i<nSum; i++)
	{
		if( i!=(nSum-1) && data[i].pFtr1==data[i+1].pFtr1 && data[i].ptIdx1==data[i+1].ptIdx1 )
			continue;

		stopItem = i+1;

		CGeometry *pGeo = data[startItem].pFtr1->GetGeometry();
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX,PT_3DEX> pts;
		pShp->GetPts(pts);

		//处理start~stop之间的重叠点
		//计算平均值，并计算各个重叠点所对应的地物的顶点序号，便于下一步修改
		double ax = data[startItem].pt1.x, ay = data[startItem].pt1.y;
		CArray<int,int> arrIndex;

		for( j=startItem; j<stopItem; j++)
		{
			CGeometry *pGeo2 = data[j].pFtr2->GetGeometry();
			const CShapeLine *pShp2 = pGeo2->GetShape();

			arrIndex.Add(pShp2->GetKeyPos(data[j].ptIdx2));

			ax += data[j].pt2.x;
			ay += data[j].pt2.y;
		}

		ax /= (stopItem-startItem+1);
		ay /= (stopItem-startItem+1);

		arrIndex.Add(pShp->GetKeyPos(data[startItem].ptIdx1));

		//修改地物
		for( j=startItem; j<stopItem+1; j++)
		{
			int ptIndex = arrIndex[j-startItem];
			CFeature* pFtr = ((j==stopItem)?data[startItem].pFtr1:data[j].pFtr2);
		
			m_pTempDS->DeleteObject(pFtr);

			CGeometry *pGeo2 = pFtr->GetGeometry();
			PT_3DEX expt = pGeo2->GetDataPoint(ptIndex);
			expt.x = ax;
			expt.y = ay;
			pGeo2->SetDataPoint(ptIndex,expt);

			m_pTempDS->RestoreObject(pFtr);
		}

		startItem = stopItem;
	}

	return TRUE;
}


BOOL CTopoSurfaceNoBreakCommand::IsOverlapped(CPFeature f1, CPFeature f2)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();

	if( !pGeo1->GetEnvelope().bIntersect(&pGeo2->GetEnvelope()) )
		return FALSE;
		
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	pGeo1->GetShape(arrPts1);
	pGeo2->GetShape(arrPts2);

	double toler = m_lfToler;
	toler = toler*toler;

	toler = 1e-8;

	if( arrPts1.GetSize()!=arrPts2.GetSize() )
		return FALSE;

	int npt = arrPts1.GetSize();

	for( int i=0; i<npt; i++)
	{
		PT_3DEX pt1 = arrPts1[i];
		PT_3DEX pt2 = arrPts2[i];
		if( (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y)>=toler )
		{
			break;
		}
	}

	if( i<npt )
	{
		for( i=0; i<npt; i++)
		{
			PT_3DEX pt1 = arrPts1[i];
			PT_3DEX pt2 = arrPts2[npt-1-i];
			if( (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y)>=toler )
			{
				break;
			}
		}

		if( i<npt )
			return FALSE;
	}

	return TRUE;
}


BOOL CTopoSurfaceNoBreakCommand::DeleteCommonLine(CFtrLayerArray& layers0)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}

	ftrsArray.SetSize(j);

	//比较地物是否完全重叠，是的话，就删除其中一个
	SnapItemArray arr;

	layid_list.SetSize(j);
	CScheme *pScheme = GetConfigLibManager()->GetScheme(m_pTempDS->GetScale());
	for(i=0; i<j; i++)
	{
		layid_list[i].pFtr = ftrsArray[i];
		layid_list[i].layids = new CArray<int,int>;
		CFtrLayer *pLay = m_pTempDS->GetFtrLayerOfObject(ftrsArray[i]);
		CSchemeLayerDefine *pDef = NULL;
		if(pScheme && pLay)
		{
			pDef = pScheme->GetLayerDefine(pLay->GetName());
		}

		if(pDef && pLay)
		{
			layid_list[i].layids->Add(pLay->GetID());
		}
		else
		{
			layid_list[i].layids->Add(0);
		}
	}

	for( i=j-1; i>=0; i--)
	{
		if(m_bProgress) GProgressStep();

		CArray<PT_3DEX,PT_3DEX> arrPts;
		ftrsArray[i]->GetGeometry()->GetShape(arrPts);
		if(arrPts.GetSize()<1) continue;

		Envelope e;
		e.CreateFromPtAndRadius(arrPts[0],GraphAPI::g_lfDisTolerance);
		int num = m_pDQ->FindObjectInRect_SimpleAndSym(e,NULL,FALSE,FALSE);
		
		const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);

		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()>=i )continue;

			if( IsOverlapped(ftrsArray[i],ftrs[k]) )
			{
				m_pTempDS->DeleteObject(ftrsArray[i]);
				for(int m=0; m<i; m++)
				{
					if(ftrs[k]==layid_list[m].pFtr)
					{
						layid_list[m].copy_item(layid_list[i]);
						layid_list[i].layids->RemoveAll();
						break;
					}
				}
				break;
			}
		}
	}

	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}

	return TRUE;
}


double CTopoSurfaceNoBreakCommand::CalcSnapDistance(CPFeature f1, CPFeature f2, int nPtIndex)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return -1;
	
	CGeometry *pObj = pFtr->GetGeometry();

	CArray<PT_3DEX,PT_3DEX> arrPts0;
	pObj->GetShape(arrPts0);
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = arrPts0[1];
		expt1 = arrPts0[0];
	}
	else
	{
		nIndex = arrPts0.GetSize()-1;
		expt0 = arrPts0[nIndex-1];
		expt1 = arrPts0[nIndex];
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;

	CGeometry *pObj2 = f2->GetGeometry();
	PT_KEYCTRL nearestPt = pObj2->FindNearestKeyCtrlPt(expt1,r,NULL,1);

	PT_3DEX newPt;
	if( nearestPt.IsValid() )
	{
		newPt = pObj2->GetDataPoint(nearestPt.index);
	}
	else
	{
		Envelope e;
		e.CreateFromPtAndRadius(expt1,r);
		if( !pObj2->FindNearestBasePt(expt1,e,NULL,&newPt,NULL) )
			return -1;
	}
	
	return GraphAPI::GGet2DDisOf2P(expt1,newPt);
}

void CTopoSurfaceNoBreakCommand::ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return;
	
	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj )return;
	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )return;
	if( ((CGeoCurveBase*)pObj)->IsClosed() )return;

	CArray<PT_3DEX,PT_3DEX> arrPts0;
	pObj->GetShape(arrPts0);
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = arrPts0[1];
		expt1 = arrPts0[0];
	}
	else
	{
		nIndex = arrPts0.GetSize()-1;
		expt0 = arrPts0[nIndex-1];
		expt1 = arrPts0[nIndex];
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;

	CGeometry *pObj2 = f2->GetGeometry();
	PT_KEYCTRL nearestPt = pObj2->FindNearestKeyCtrlPt(expt1,r,NULL,1);

	BOOL bSnapVertex = FALSE;
	PT_3DEX newPt = expt1;
	int nIndex2 = -1;
	if( nearestPt.IsValid() )
	{
		newPt = pObj2->GetDataPoint(nearestPt.index);
		nIndex2 = nearestPt.index;
		bSnapVertex = TRUE;
	}
	else
	{
		Envelope e;
		e.CreateFromPtAndRadius(expt1,r);
		if( !pObj2->FindNearestBasePt(expt1,e,NULL,&newPt,NULL) )
			return;

		const CShapeLine *pShp = pObj2->GetShape();
		if( !pShp )
			return;

		nIndex2 = pShp->FindNearestKeyPt(newPt);
	}
	
	m_pTempDS->DeleteObject(pFtr);
	
	PT_3DEX pt1 = newPt;
	if( !m_bSnap3D )
	{
		pt1.z = expt1.z;
	}
	pFtr->GetGeometry()->SetDataPoint(nPtIndex,pt1);		
	m_pTempDS->RestoreObject(pFtr);

	if( m_bAddPt && !bSnapVertex )
	{
		m_pTempDS->DeleteObject(f2);

		CArray<PT_3DEX,PT_3DEX> arrPts;
		f2->GetGeometry()->GetShape(arrPts);
		arrPts.InsertAt(nIndex2+1,newPt);

		f2->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		m_pTempDS->RestoreObject(f2);
	}
}

ULONG32 CTopoSurfaceNoBreakCommand::get_geo_data()
{  
	CString text =m_HandleLayer;
	text.Remove(' ');
	geo_list.RemoveAll();
	ftr_list.RemoveAll();
	ULONG32 geo_count=0;
	//
	if (text.IsEmpty())
	{
		CFeature *pFtr=NULL;	
		CFtrLayer *pLayer=NULL;   
		int nLayNum = m_pTempDS->GetFtrLayerCount(),i,j;
		for( i=0; i<nLayNum; i++)
		{
			pLayer = m_pTempDS->GetFtrLayerByIndex(i);
			if( !pLayer )continue;
			
			int nObjNum = pLayer->GetObjectCount();
			for( j=0; j<nObjNum; j++)
			{
				pFtr = pLayer->GetObject(j);
				if( !m_pTempDS->IsFeatureValid(pFtr) )
					continue;
				if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
				//
				   geo_list.Add(pFtr->GetGeometry());
				   ftr_list.Add(pFtr);
				   ++geo_count;
				}
			}
		}
	}
	else
	{
		CFtrLayer *pLayer = NULL;
		CFeature *pFtr = NULL;
		int startpos = 0, findpos = 0;
		while( findpos>=0 )
		{
			findpos = text.Find(_T(','),startpos);
			
			CString sub;
			if( findpos>startpos )
				sub = text.Mid(startpos,(findpos-startpos));
			else
				sub = text.Mid(startpos);
			CPtrArray p;
			m_pTempDS->GetFtrLayer(sub,NULL,&p);		
			for (int i=0;i<p.GetSize();i++)
			{
				pLayer = (CFtrLayer *)p[i];
				int nObjNum = pLayer->GetObjectCount();
				for( int j=0; j<nObjNum; j++)
				{					
					pFtr = pLayer->GetObject(j);
					if( !m_pTempDS->IsFeatureValid(pFtr) )
						continue;	
					if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
					{
						//
						geo_list.Add(pFtr->GetGeometry());
						ftr_list.Add(pFtr);
						++geo_count;
					}			
				}
			}			
			startpos = findpos+1;
		}	
	}
	return geo_count;
}


void CTopoSurfaceNoBreakCommand::create_arc_list()
{
	arc_list.RemoveAll();
	int i,j;
	//生成弧列表；
	int num1 = geo_list.GetSize();
	int num2 = ftr_list.GetSize();
	
	for(i=0;i<num1;++i)
	{
	    CShapeLine *shape=const_cast<CShapeLine *>(geo_list[i]->GetShape());
		//
		TArcItem temp;
		if(i<num2)
		{
			temp.FeatureFlag = ftr_list[i]->GetAppFlag();
			ftr_list[i]->SetAppFlag(i);
		}
		temp.geo_index=i;
		temp.point_count = shape->GetPtsCount();
		//
		temp.sp.data=shape->GetPt(0);
		temp.sp.connects=new TConnectItem();
		temp.sp.connects->index=i;
		temp.sp.connects->degree=361;
		temp.sp.next_arc=NULL;
		//
		temp.s_degree=get_degree(temp.sp.data,shape->GetPt(1));
		//
		temp.ep.data=shape->GetPt(temp.point_count-1);
		temp.ep.connects=new TConnectItem();
		temp.ep.connects->index=i;
		temp.ep.connects->degree=361;
		temp.ep.next_arc=NULL;
		//
		PT_3DEX temp_ep=shape->GetPt(temp.point_count-2);
		temp.e_degree=get_degree(temp.ep.data,temp_ep);
		//
		if(temp.sp==temp.ep)
		{
			temp.state=CLOSED;
		}
		else
		{
			temp.state=UNVISITED;
		}
		//
		temp.face_index_list=new CArray<int,int>;
		arc_list.Add(temp);
	}
	//构建弧的连接关系;
	if(m_bProgress) GProgressStart(arc_list.GetSize());
	for(i=0;i<arc_list.GetSize();++i)
	{
		if(m_bProgress) GProgressStep();
		//起点
		Envelope e;
		e.CreateFromPtAndRadius(arc_list[i].sp.data, PRECISION);
		int num = m_pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
		const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);
		for( j=0; j<num; j++)
		{
			int pos = ftrs[j]->GetAppFlag();//取出下标
			if( pos == i )
				continue;
			
			if(arc_list[i].sp==arc_list[pos].sp)
			{
               TConnectItem *p=new TConnectItem();
               p->index=pos;
			   //计算与此弧的角度(逆时针方向)；
               p->degree=arc_list[i].s_degree-arc_list[pos].s_degree;
			   if(fabs(p->degree)<0.0001) continue;
			   if(p->degree<0)
			   {
				   p->degree+=360;
			   }
			   p->StoE = TRUE;
			   //
			   arc_list[i].sp.add_con_item(p);
			}
			else if(arc_list[i].sp==arc_list[pos].ep)
			{
				TConnectItem *p=new TConnectItem();
				p->index=pos;
				//计算与此弧的角度(逆时针方向)；
                p->degree=arc_list[i].s_degree-arc_list[pos].e_degree;
				if(fabs(p->degree)<0.0001) continue;
				if(p->degree<0)
				{
					p->degree+=360;
				}
				p->StoE = FALSE;
				//
			    arc_list[i].sp.add_con_item(p);
			}				
		}
	
		//终点
		e.CreateFromPtAndRadius(arc_list[i].ep.data, PRECISION);
		num = m_pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
		ftrs = m_pDQ->GetFoundHandles(num);
		for( j=0; j<num; j++)
		{
			int pos = ftrs[j]->GetAppFlag();//取出下标
			if( pos == i )
				continue;

			if(arc_list[i].ep==arc_list[pos].sp)
			{
				TConnectItem *p=new TConnectItem();
				p->index=pos;
			    //计算与此弧的角度(逆时针方向)；
                p->degree=arc_list[i].e_degree-arc_list[pos].s_degree;
				if(fabs(p->degree)<0.0001) continue;
				if(p->degree<0)
				{
					p->degree+=360;
				}
				p->StoE = TRUE;
				//
				arc_list[i].ep.add_con_item(p);
			}
			else if(arc_list[i].ep==arc_list[pos].ep)
			{
				TConnectItem *p=new TConnectItem();
				p->index=pos;
				//计算与此弧的角度(逆时针方向)；
                p->degree=arc_list[i].e_degree-arc_list[pos].e_degree;
				if(fabs(p->degree)<0.0001) continue;
				if(p->degree<0)
				{
					p->degree+=360;
				}
				p->StoE = FALSE;
				//
				arc_list[i].ep.add_con_item(p);
			}
		}
	}

	//还原feature的AppFlag
	for(i=0; i<num1 && i<num2; ++i)
	{
		ftr_list[i]->SetAppFlag(arc_list[i].FeatureFlag);
	}
	if(m_bProgress) GProgressEnd();

	//标记断路的弧
IV:	int nIvalid=0;//每次循环找出的无效弧的个数
	for(i=0; i<arc_list.GetSize(); i++)
	{
		if(arc_list[i].state == CLOSED) continue;
		if(arc_list[i].state == INVALID) continue;

		TConnectItem *pc = NULL;
		//ep
		pc = arc_list[i].ep.connects->next;
		BOOL IsConnectsValid = TRUE;//sp或者ep的所有连接弧是否都是断开的
		while(pc)
		{
			if(arc_list[pc->index].state==UNVISITED)
			{
				IsConnectsValid = FALSE;
				break;
			}
			pc = pc->next;
		}
		if(IsConnectsValid)
		{
			arc_list[i].state=INVALID;
			nIvalid++;
			continue;
		}
		//sp
		pc = arc_list[i].sp.connects->next;
		IsConnectsValid = TRUE;//sp的所有连接弧是否都是断开的
		while(pc)
		{
			if(arc_list[pc->index].state==UNVISITED)
			{
				IsConnectsValid = FALSE;
				break;
			}
			pc = pc->next;
		}
		if(IsConnectsValid)
		{
			arc_list[i].state=INVALID;
			nIvalid++;
			continue;
		}
	}
	if(nIvalid>0)   goto IV;//循环直到没有断开的弧，断开指的是不能和其他弧构成闭合

	//标记追踪过程中被往返走两次的弧， 这样的弧不参与构面
	CArray<int,int> arrIndex;//记录走过的弧段序号
	for(i=0; i<arc_list.GetSize(); i++)
	{
		if(arc_list[i].state == CLOSED) continue;
		if(arc_list[i].state == INVALID) continue;
		TConnectItem *pc = NULL;
		BOOL bInvalid = FALSE;
		//ep
		TEndPoint *face_ep = &arc_list[i].ep;
		arrIndex.RemoveAll();
		while(1)
		{
			pc = face_ep->connects->next;
			while(pc)
			{
				if(arc_list[pc->index].state==UNVISITED)
				{
					break;
				}
				pc = pc->next;
			}
			if(!pc) break;
			if(pc->index == i)
			{
				if(!pc->StoE)
				{
					arc_list[i].state=INVALID;
					bInvalid = TRUE;
				}
				break;
			}
			arrIndex.Add(pc->index);
			if(pc->StoE)
			{
				if(arc_list[pc->index].SEVisited)
				{
					break;
				}
				arc_list[pc->index].SEVisited=TRUE;
				face_ep = &arc_list[pc->index].ep;
			}
			else
			{
				if(arc_list[pc->index].ESVisited)
				{
					break;
				}
				arc_list[pc->index].ESVisited=TRUE;
				face_ep = &arc_list[pc->index].sp;
			}
		}
		for(j=0; j<arrIndex.GetSize(); j++)
		{
			int index = arrIndex[j];
			arc_list[index].SEVisited = FALSE;
			arc_list[index].ESVisited = FALSE;
		}
		if(bInvalid) continue;
		//sp
		TEndPoint *face_sp = &arc_list[i].sp;
		arrIndex.RemoveAll();
		while(1)
		{
			pc = face_sp->connects->next;
			while(pc)
			{
				if(arc_list[pc->index].state==UNVISITED)
				{
					break;
				}
				pc = pc->next;
			}
			if(!pc) break;
			if(pc->index == i)
			{
				if(pc->StoE)
				{
					arc_list[i].state=INVALID;
				}
				break;
			}
			arrIndex.Add(pc->index);
			if(pc->StoE)
			{
				if(arc_list[pc->index].SEVisited)
				{
					break;
				}
				arc_list[pc->index].SEVisited=TRUE;
				face_ep = &arc_list[pc->index].ep;
			}
			else
			{
				if(arc_list[pc->index].ESVisited)
				{
					break;
				}
				arc_list[pc->index].ESVisited=TRUE;
				face_ep = &arc_list[pc->index].sp;
			}
		}
		for(j=0; j<arrIndex.GetSize(); j++)
		{
			int index = arrIndex[j];
			arc_list[index].SEVisited = FALSE;
			arc_list[index].ESVisited = FALSE;
		}
	}
}

void CTopoSurfaceNoBreakCommand::find_closed_arc()
{
   for (int i=0;i<arc_list.GetSize();++i)
   {
	   if(arc_list[i].state==CLOSED)
	   {
		   CShapeLine *shape=const_cast<CShapeLine *>(geo_list[arc_list[i].geo_index]->GetShape());
		   //
		   TFaceItem temp;
		   int j=0;
		   //
		   temp.color_index=-1;
		   temp.arc_item_list=new CArray<TConnectItemBase,TConnectItemBase>();
           TConnectItem *p=0;
		   //
		   TConnectItemBase item;
		   item.index=i;
	       temp.arc_item_list->Add(item);
		   temp.point_count=arc_list[i].point_count;
		   temp.datas=new PT_3DEX[temp.point_count];
		   for(j=0;j<temp.point_count;++j)
		   {
              temp.datas[j]=shape->GetPt(j);
			  temp.datas[j].pencode = penLine;
		   }
		   arc_list[i].face_index_list->Add(face_list.GetSize());
		   int geo_index = arc_list[i].geo_index;
		   temp.layer_id  = m_pTempDS->GetFtrLayerOfObject(ftr_list[geo_index])->GetID();
		   face_list.Add(temp);
	   }
   }
}

static double CaleZ(double z1, double z2)
{
	if(fabs(z1)<GraphAPI::g_lfZTolerance)
	{
		return z2;
	}
	else
	{
		return z1;
	}
}

void CTopoSurfaceNoBreakCommand::create_face(int start_index, BOOL IsEP)
{
    TFaceItem temp;
	int i=0,j=0;
	//
	temp.color_index=-1;
	temp.arc_item_list=new CArray<TConnectItemBase,TConnectItemBase>();
	TConnectItem *p=0;
    //
	TConnectItemBase item;
	item.index=start_index;
	item.StoE=IsEP;
	temp.arc_item_list->Add(item);
	//
	arc_list[start_index].face_index_list->Add(face_list.GetSize());
	if(IsEP)
	{
		p=arc_list[start_index].ep.next_arc;
	}
	else
	{
		p=arc_list[start_index].sp.next_arc;
	}
	//
	TConnectItem *p1 = p;

	while(p!=0)
	{
       item.index=p->index;
	   item.StoE=p->StoE;
	   temp.arc_item_list->Add(item);
	   arc_list[p->index].face_index_list->Add(face_list.GetSize());
	   if(p->StoE)
	   {
		   arc_list[p->index].SEVisited = TRUE;//标记该弧s->e已经走过
		   p=arc_list[p->index].ep.next_arc;
	   }
	   else
	   {
		   arc_list[p->index].ESVisited = TRUE;//标记该弧e->s已经走过
           p=arc_list[p->index].sp.next_arc;
	   }
	   
	   if(p && p->StoE && arc_list[p->index].SEVisited)
		   return;
	   if(p && !p->StoE && arc_list[p->index].ESVisited)
		   return;

	   if(p == p1)
	   {
		   return;//死循环
	   }
	}
	//
    CArray<PT_3DEX,PT_3DEX> temp_datas;
    //添加起始弧数据；
	int geo_index = arc_list[temp.arc_item_list->GetAt(0).index].geo_index;

	CountItem ids;
	int m=0, n=0;
	for(m=0; m<layid_list.GetSize(); m++)
	{
		if(ftr_list[geo_index]==layid_list[m].pFtr)
		{
			for(n=0; n<layid_list[m].layids->GetSize(); n++)
			{
				int id = layid_list[m].layids->GetAt(n);
				ids.addValue(id);
			}
		}
	}

	if(temp.arc_item_list->GetAt(0).StoE)
	{
		CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
		CArray<PT_3DEX,PT_3DEX> pts2;
		shape->GetPts(pts2);
		for(j=0;j<arc_list[temp.arc_item_list->GetAt(0).index].point_count;++j)
		{
			if(j<pts2.GetSize())
				temp_datas.Add(pts2[j]);
		}
	}
	else
	{
		CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
		CArray<PT_3DEX,PT_3DEX> pts2;
		shape->GetPts(pts2);
		for(j=arc_list[temp.arc_item_list->GetAt(0).index].point_count-1;j>=0;--j)
		{
			if(j<pts2.GetSize())
				temp_datas.Add(pts2[j]);
		}
	}
	//添加中间弧数据；
	for(i=1;i<temp.arc_item_list->GetSize()-1;++i)
	{
		//计算前一个点的高程
		int nPt_temp = temp_datas.GetSize();

		geo_index=arc_list[temp.arc_item_list->GetAt(i).index].geo_index;

		for(m=0; m<layid_list.GetSize(); m++)
		{
			if(ftr_list[geo_index]==layid_list[m].pFtr)
			{
				for(n=0; n<layid_list[m].layids->GetSize(); n++)
				{
					int id = layid_list[m].layids->GetAt(n);
					ids.addValue(id);
				}
			}
		}

		if(temp.arc_item_list->GetAt(i).StoE)
		{
			CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
			CArray<PT_3DEX,PT_3DEX> pts2;
			shape->GetPts(pts2);
			temp_datas[nPt_temp-1].z = CaleZ(temp_datas[nPt_temp-1].z, pts2[0].z);
			for(j=1;j<arc_list[temp.arc_item_list->GetAt(i).index].point_count;++j)
			{
				if(j<pts2.GetSize())
					temp_datas.Add(pts2[j]);
			}
		}
		else
		{
			CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
			CArray<PT_3DEX,PT_3DEX> pts2;
			shape->GetPts(pts2);
			temp_datas[nPt_temp-1].z = CaleZ(temp_datas[nPt_temp-1].z, pts2[pts2.GetSize()-1].z);
			for(j=arc_list[temp.arc_item_list->GetAt(i).index].point_count-2;j>=0;--j)
			{
				if(j<pts2.GetSize())
					temp_datas.Add(pts2[j]);
			}
		}
	}
	//添加尾弧数据；
	geo_index=arc_list[temp.arc_item_list->GetAt(i).index].geo_index;

	for(m=0; m<layid_list.GetSize(); m++)
	{
		if(ftr_list[geo_index]==layid_list[m].pFtr)
		{
			for(n=0; n<layid_list[m].layids->GetSize(); n++)
			{
				int id = layid_list[m].layids->GetAt(n);
				ids.addValue(id);
			}
		}
	}
    if(temp.arc_item_list->GetAt(i).StoE)
	{
		CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
		CArray<PT_3DEX,PT_3DEX> pts2;
		shape->GetPts(pts2);
		int nPt_temp = temp_datas.GetSize();
		temp_datas[nPt_temp-1].z = CaleZ(temp_datas[nPt_temp-1].z, pts2[0].z);
		for(j=1;j<arc_list[temp.arc_item_list->GetAt(i).index].point_count;++j)
		{
			if(j<pts2.GetSize())
				temp_datas.Add(pts2[j]);
		}
	}
	else
	{
		CShapeLine *shape=const_cast<CShapeLine *>(geo_list[geo_index]->GetShape());
		CArray<PT_3DEX,PT_3DEX> pts2;
		shape->GetPts(pts2);
		int nPt_temp = temp_datas.GetSize();
		temp_datas[nPt_temp-1].z = CaleZ(temp_datas[nPt_temp-1].z, pts2[pts2.GetSize()-1].z);
		for(j=arc_list[temp.arc_item_list->GetAt(i).index].point_count-2;j>=0;--j)
		{
			if(j<pts2.GetSize())
				temp_datas.Add(pts2[j]);
		}
	}
	//首尾点高程
	int nPt = temp_datas.GetSize();
	if(nPt<3) return;
	if( GraphAPI::GIsEqual2DPoint(&temp_datas[0], &temp_datas[nPt-1]) )
	{
		if( fabs(temp_datas[0].z)<1e-4 )
		{
			temp_datas[0].z = temp_datas[nPt-1].z;
		}
		else if( fabs(temp_datas[nPt-1].z)<1e-4 )
		{
			temp_datas[nPt-1].z = temp_datas[0].z;
		}
	}

	//剔除顺时针的面
	if(0!=GraphAPI::GIsClockwise(temp_datas.GetData(), nPt))
		return;
	//
	temp.point_count=nPt;
	temp.datas=new PT_3DEX[nPt];
	for (i=0;i<nPt;++i)//逆时针变顺时针
	{
		temp.datas[i]=temp_datas[nPt-1-i];
	}
	
	temp.layer_id = ids.getValue(temp.arc_item_list->GetSize());
	//
	face_list.Add(temp);	
	temp_datas.RemoveAll();
}

//此算法并不能保证所有相邻的面颜色都不一样；
void CTopoSurfaceNoBreakCommand::set_color()
{
	int current_color=0;
	for(int i=0;i<face_list.GetSize();++i)
	{
		if(face_list[i].color_index==-1)
		{
			face_list[i].color_index=current_color;
			current_color=(current_color+1)%10;
		}
		for(int j=0;j<face_list[i].arc_item_list->GetSize();++j)
		{
            for(int k=0;k<arc_list[(face_list[i].arc_item_list->GetAt(j)).index].face_index_list->GetSize();++k)
			{
				int t=arc_list[(face_list[i].arc_item_list->GetAt(j)).index].face_index_list->GetAt(k);
				if(t==i)
				{
					continue;
				}
				else
				{
					if(face_list[t].color_index==-1 || face_list[t].color_index==face_list[i].color_index)
					{
						face_list[t].color_index=(face_list[i].color_index+1)%10;
					}
				}
			}
		}
	}
}

void CTopoSurfaceNoBreakCommand::save_data()
{
	struct TfaceSort
	{
		CFeature *feature;
		double area;
		TfaceSort *next; 
	};
	TfaceSort *sort_head=new TfaceSort;
	sort_head->feature=0;
	sort_head->area=DBL_MAX;
	sort_head->next=0;
	//
	int transparency = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,"Transparency", 50);
	TfaceSort *p=0;
	for(int i=0;i<face_list.GetSize();++i)
	{
		CFeature *featur=new CFeature();
		CGeoSurface *geometry=new CGeoSurface();	
		geometry->CreateShape(face_list[i].datas,face_list[i].point_count);	
		geometry->EnableFillColor(TRUE,COLORREF(ColorList[face_list[i].color_index]));
		geometry->SetTransparency(transparency);
	 	featur->SetGeometry(geometry);
		featur->SetID(OUID());
		featur->SetAppFlag(face_list[i].layer_id);
		featur->SetPurpose(FTR_EDB);
		double area=geometry->GetArea();
		//
		p=sort_head;
        while(p->next!=0 && p->next->area>=area)
		{
           p=p->next;
		}
        TfaceSort *s=new TfaceSort;
		s->area=area;
		s->feature=featur;
		s->next=p->next;
        p->next=s;
	}

	//创建复杂面
	CFtrArray ftrs1, ftrs2;
	p=sort_head;
    while(p!=0 )
	{
		if(p->feature)ftrs1.Add(p->feature);

		TfaceSort *old=p;
		p=p->next;
		delete old;
	}

	if(m_bCreateMultiSurface)
	{
		CreateSurfaces(ftrs1, ftrs2);
	}
	else
	{
		ftrs2.Copy(ftrs1);
	}

	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if( !pScheme )  return;
	for(i=0; i<ftrs2.GetSize();i++)
	{
		Envelope e;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		ftrs2[i]->GetGeometry()->GetShape(arrPts);
		e.CreateFromPts(arrPts.GetData(), arrPts.GetSize(), sizeof(PT_3DEX));

		int num = m_pDQ->FindObjectInRect(e,NULL,TRUE,FALSE), k;
		const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);
		for(k=0; k<num; k++)
		{
			CGeometry *pObj = ftrs[k]->GetGeometry();
			if(pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)))
			{
				PT_3DEX pt0 = pObj->GetDataPoint(0);
				PT_3D pt3d;
				COPY_3DPT(pt3d, pt0);
				if(ftrs2[i]->GetGeometry()->bPtIn(&pt3d))
				{
					break;
				}
			}
		}

		int layerid = 0;
		if(k==num)//无面域点
		{
			if(ftrs2[i]->GetAppFlag()>0)//构成ftrs2[i]的弧属于同一层
			{
				CFtrLayer *pLayer = pDS->GetFtrLayer(ftrs2[i]->GetAppFlag());
				if(pLayer)
				{
					CSchemeLayerDefine *pDefLayer = pScheme->GetLayerDefine( pLayer->GetName() );
					if( pDefLayer )
					{
						if(pDefLayer->GetDbGeoClass() == CLS_GEOSURFACE)//入库类型为面
						{
							layerid = ftrs2[i]->GetAppFlag();
							ftrs2[i]->GetGeometry()->SetColor(pDefLayer->GetColor());
							ftrs2[i]->GetGeometry()->SetSymbolName("");
						}
					}
				}
			}
			if (m_bBuildWithSurfacePt && layerid == 0)//无面域点时不构面
			{
				delete ftrs2[i];
				continue;
			}
		}

		if(layerid==0)
		{
			layerid = m_Layer_ID;
		}
		ftrs2[i]->SetAppFlag(layerid);
		ftrs2[i]->SetPurpose(FTR_EDB);
		if(!m_pTempDS->AddObject(ftrs2[i],layerid))
		{
			delete ftrs2[i];
		}
	}
}

//如果重合的是面，则更改属性
//如果是闭合线，则转面
CPFeature CTopoSurfaceNoBreakCommand::ToNewSurface(FTR_HANDLE handle, CUndoFtrs& undo)
{
	CGeometry *pGeo = HandleToFtr(handle)->GetGeometry();
	if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
	{
		return HandleToFtr(handle);
	}
	
	CFeature *pNew = NULL;
	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		pNew = HandleToFtr(handle)->Clone();
		if (!pNew) return NULL;
		pNew->SetPurpose(FTR_EDB);
		CArray<PT_3DEX,PT_3DEX> arrPts;
		HandleToFtr(handle)->GetGeometry()->GetShape(arrPts);
		CGeoSurface* pSurface = new CGeoSurface();
		pSurface->CreateShape(arrPts.GetData(), arrPts.GetSize());
		pNew->SetGeometry(pSurface);
		
		if (!m_pEditor->AddObject(pNew,m_pEditor->GetFtrLayerIDOfFtr(handle)))
		{
			delete pNew;
			return NULL;
		}
		GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handle),pNew);
		undo.AddNewFeature(FTR_HANDLE(pNew));
		undo.AddOldFeature(handle);		
		m_pEditor->DeleteObject(handle);
	}
	else
	{
		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);
		
		if (pts.GetSize() < 3) return NULL;
		
		BOOL bClosed = ((CGeoCurveBase*)pGeo)->IsClosed();
		if (!bClosed)
		{
			return NULL;
		}
		
		CValueTable tab;
		tab.BeginAddValueItem();
		HandleToFtr(handle)->WriteTo(tab);
		tab.EndAddValueItem();
		
		tab.DelField(FIELDNAME_SHAPE);
		tab.DelField(FIELDNAME_GEOCURVE_CLOSED);
		tab.DelField(FIELDNAME_FTRDELETED);
		tab.DelField(FIELDNAME_FTRID);
		tab.DelField(FIELDNAME_GEOCLASS);
		tab.DelField(FIELDNAME_FTRDISPLAYORDER);
		tab.DelField(FIELDNAME_FTRGROUPID);
		
		pNew = HandleToFtr(handle)->Clone();
		
		if (!pNew || !pNew->CreateGeometry(CLS_GEOSURFACE))
			return NULL;
		pNew->ReadFrom(tab);
		pNew->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
		pNew->SetPurpose(FTR_EDB);
		CGeoSurface* pSurface = (CGeoSurface*)pNew->GetGeometry();
		if (!m_pEditor->AddObject(pNew,m_pEditor->GetFtrLayerIDOfFtr(handle)))
		{
			delete pNew;
			return NULL;
		}
		
		GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(handle),pNew);
		undo.AddNewFeature(FTR_HANDLE(pNew));
		undo.AddOldFeature(handle);		
		m_pEditor->DeleteObject(handle);
	}
	return pNew;
}

void CTopoSurfaceNoBreakCommand::FindSameEdgePart(int *buf, int num, int& start, int& end)
{
	int i, i1;
	
	//找到任一个非公共点
	for( i=0; i<num; i++)
	{
		if( buf[i]==0 )break;
	}
	//无公共点
	if( i>=num )
	{
		start = end = 0;
		return;
	}
	//找到下一个公共点，此点可以认为是非公共部分的末点
	for( i1=i+1; i1<num+i; i1++)
	{
		if( buf[(i1%num)]!=0 )break;
	}
	end = (i1%num);
	
	//找到下一个非公共点，此点的前一点可以认为是非公共部分的起点
	for( ; i1<=num+i; i1++)
	{
		if( buf[((i1+1)%num)]==0 )break;
	}
	start = (i1%num);
}



//获得对象中在start和end之间的点，如果start>end，那么，获得的点是包括对象的首尾点的
void CTopoSurfaceNoBreakCommand::GetObjectPart(CGeometry* pObj, int start, int end, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);
	int nPtSum = arr.GetSize();
	PT_3DEX expt;
	
	if( end<start )end += nPtSum;
	
	for( int i=start; i<=end; i++)
	{
		expt = arr[i%nPtSum];
		arrPts.Add(expt);
	}
}

CGeoSurface * CTopoSurfaceNoBreakCommand::MergeSurface(CGeoSurface * pObj1, CGeoSurface * pObj2)
{
	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;
	
	e3.Intersect(&e2);
	if(e3.m_xl>e3.m_xh || e3.m_yl>e3.m_yh )
		return NULL;
	
	if( pObj1==NULL || pObj2==NULL )
		return NULL;
	
	if( !pObj1->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || !pObj2->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		return NULL;
	
	CGeoSurface *pObj1_new = NULL;
	CGeoSurface *pObj2_new = NULL;
	
	if(pObj1_new==NULL)
		pObj1_new = pObj1;
	
	if(pObj2_new==NULL)
		pObj2_new = pObj2;
	
	gsbh::GeoSurfaceBooleanHandle gsbhandle;
	if(!gsbhandle.setSurface(pObj2_new,pObj1_new))
	{
		if(pObj1_new!=pObj1)delete pObj1_new;
		if(pObj2_new!=pObj2)delete pObj2_new;
		return FALSE;
	}
	//
	if(!gsbhandle.init())
	{
		if(pObj1_new!=pObj1)delete pObj1_new;
		if(pObj2_new!=pObj2)delete pObj2_new;
		return FALSE;
	}
	//
	gsbhandle.join();
	
	CGeoSurface* pNewObj = gsbhandle.getResult();
	
	if(pObj1_new!=pObj1)delete pObj1_new;
	if(pObj2_new!=pObj2)delete pObj2_new;
	
	if(pNewObj == NULL)
		return NULL;
	//
	CGeoSurface* ptemp = (CGeoSurface*)pObj1->Clone();
	CArray<PT_3DEX, PT_3DEX> temp_shape;
	pNewObj->GetShape(temp_shape);
	ptemp->CreateShape(temp_shape.GetData(),temp_shape.GetSize());
	delete pNewObj;
	
	return ptemp;
/*	Envelope e1 = pObj1->GetEnvelope();
	Envelope e2 = pObj2->GetEnvelope();
	Envelope e3 = e1;

	pObj1 = (CGeoSurface*)pObj1->Clone();
	pObj2 = (CGeoSurface*)pObj2->Clone();

	CArray<PT_3DEX,PT_3DEX> arr1, arr2;
	pObj1->GetShape(arr1);
	pObj2->GetShape(arr2); 
	
	int npt1 = GraphAPI::GKickoffSame2DPoints(arr1.GetData(),arr1.GetSize());
	arr1.SetSize(npt1);
	int npt2 = GraphAPI::GKickoffSame2DPoints(arr2.GetData(),arr2.GetSize());
	arr2.SetSize(npt2);
	pObj1->CreateShape(arr1.GetData(),arr1.GetSize());
	pObj2->CreateShape(arr2.GetData(),arr2.GetSize());

	int nPtSum1 = arr1.GetSize(), nPtSum2 = arr2.GetSize(), count = 1;
	int nSamePtPos;
	CArray<int,int> pos1, pos2;
	pos1.SetSize(nPtSum1);
	pos2.SetSize(nPtSum2);

	//找到点重叠的部分，并设置标志位
	int *buf1 = pos1.GetData(), *buf2 = pos2.GetData();

	memset(buf1,0,sizeof(int)*nPtSum1);
	memset(buf2,0,sizeof(int)*nPtSum2);

	if (pObj1->IsClosed())
	{
		nPtSum1--;
	}

	if (pObj2->IsClosed())
	{
		nPtSum2--;
	}
	
	PT_3DEX expt;
	double tolerance = GraphAPI::g_lfDisTolerance;
	
	for( int i=0; i<nPtSum1; i++)
	{
		expt = arr1[i];

		if (i != 0 && GraphAPI::GIsEqual2DPoint(&expt,&arr1[i-1]) )
		{
			if (buf1[i-1] != 0)
			{					
				buf1[i] = buf1[i-1];
				buf1[i-1] = 0;
			}
			
			continue;
		}

		if(!e2.bPtIn(&expt))
			continue;

		PT_3D p0,p1;
		COPY_3DPT(p0,expt);
		//m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&p0,&p1);
		PT_KEYCTRL pos;
		pos = pObj2->FindNearestKeyCtrlPt(p0,tolerance,NULL,1);
		if (pos.IsValid())
		{				
			for (int j=pos.index; j<nPtSum2-1; j++)
			{
				if (!GraphAPI::GIsEqual2DPoint(&arr2[j],&arr2[j+1]))
				{
					break;
				}
			}

			nSamePtPos = j;
			buf1[i] = count;
			buf2[nSamePtPos] = count;
			count++;
		}
	}

	int start1, end1, start2, end2;

	//获得非公共边的部分，但是包括了公共边的起点和末点
	FindSameEdgePart(buf1,nPtSum1,start1,end1);
	FindSameEdgePart(buf2,nPtSum2,start2,end2);

	//有问题的公共边
	if( (start1==end1) || (start2==end2) ||
		(buf1[start1]!=buf2[start2] && buf1[start1]!=buf2[end2]) ||
		(buf1[end1]!=buf2[start2] && buf1[end1]!=buf2[end2]) )
	{
		delete pObj1;
		delete pObj2;
		return NULL;
	}

	//建立新对象
	CGeoSurface *pNewObj = (CGeoSurface*)pObj1->Clone();	

	//获取重复边
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;

	GetObjectPart(pObj1,start1,end1,arrPts1);
	GetObjectPart(pObj2,start2,end2,arrPts2);

	//调整顺序
	if( buf1[end1]!=buf2[start2] )
	{
		int size = arrPts2.GetSize();
		for( i=0; i<size/2; i++)
		{
			expt = arrPts2.GetAt(i);
			arrPts2.SetAt(i,arrPts2.GetAt(size-1-i));
			arrPts2.SetAt(size-1-i,expt);
		}
	}

	delete pObj1;
	delete pObj2;

	//添加节点

	CArray<PT_3DEX,PT_3DEX> arr;

	arr.Append(arrPts1);
	arrPts2.RemoveAt(0);

	arr.Append(arrPts2);

	int npt3 = GraphAPI::GKickoffSame2DPoints(arr.GetData(),arr.GetSize());
	arr.SetSize(npt3);

	pNewObj->CreateShape(arr.GetData(),arr.GetSize());	

	return pNewObj;*/
}


void CTopoSurfaceNoBreakCommand::CreateSurfaces(CFtrArray& ftrs1, CFtrArray& ftrs2)
{	
	for(int i=0; i<ftrs1.GetSize(); i++)
	{
		CFeature *pFtr1 = ftrs1[i];
		if(!pFtr1)continue;
		
		CGeoSurface *pGeo1 = (CGeoSurface*)pFtr1->GetGeometry();
		CFtrArray ftrs;
		
		for( int j=i+1; j<ftrs1.GetSize(); j++)
		{
			CFeature *pFtr2 = ftrs1[j];
			if(!pFtr2)continue;
			CGeoSurface *pGeo2 = (CGeoSurface*)pFtr2->GetGeometry();

			if(IsInside(pGeo2,pGeo1))
			{
				//pGeo2 不能同时落入另一个面中
				for(int k=0; k<ftrs.GetSize(); k++)
				{
					CFeature *pFtr3 = ftrs[k];
					CGeoSurface *pGeo3 = (CGeoSurface*)pFtr3->GetGeometry();					
					if(IsInside(pGeo2,pGeo3))
					{
						break;
					}
				}

				if(k>=ftrs.GetSize())
				{
					ftrs.Add(pFtr2);					
				}
			}
		}

		if(ftrs.GetSize()==0)
		{
			ftrs2.Add(pFtr1);
			ftrs1[i] = NULL;
		}
		else
		{
			CFeature *pNewFtr = pFtr1->Clone();
			CGeoMultiSurface *pNewGeo = new CGeoMultiSurface();	
			pNewGeo->CopyFrom(pFtr1->GetGeometry());

			CGeoArray geos;
			geos.SetSize(ftrs.GetSize());
			for(int k=0; k<ftrs.GetSize(); k++)
			{
				CFeature *pFtr3 = ftrs[k];
				CGeoSurface *pGeo3 = (CGeoSurface*)pFtr3->GetGeometry();
				geos[k] = pGeo3->Clone();
			}

			//对子面作合并
			for(k=0; k<geos.GetSize(); k++)
			{
				CGeoSurface *pGeo3 = (CGeoSurface*)geos[k];
				if(!pGeo3)continue;

				for(int m=k+1; m<geos.GetSize(); m++)
				{
					CGeoSurface *pGeo4 = (CGeoSurface*)geos[m];
					if(!pGeo4)continue;

					CGeoSurface *pGeo5 = MergeSurface(pGeo3,pGeo4);
					if(pGeo5)
					{
						delete pGeo3;
						delete pGeo4;
						
						geos[k] = pGeo5;
						pGeo3 = pGeo5;
						geos[m] = NULL;

						//pGeo3已经改变，重新开始遍历
						m = k;
					}			
				}
			}
			

			CArray<PT_3DEX,PT_3DEX> arrPts0;
			pNewGeo->GetShape(arrPts0);
			
			for(k=0; k<geos.GetSize(); k++)
			{
				CGeoSurface *pGeo3 = (CGeoSurface*)geos[k];
				if(!pGeo3)continue;

				CArray<PT_3DEX,PT_3DEX> arrPts;
				pGeo3->GetShape(arrPts);
				if(arrPts.GetSize()>0 )
					arrPts[0].pencode = penMove;

				arrPts0.Append(arrPts);				
				delete pGeo3;
			}

			pNewGeo->CreateShape(arrPts0.GetData(),arrPts0.GetSize());

			delete pFtr1;

			pNewFtr->SetGeometry(pNewGeo);

			ftrs2.Add(pNewFtr);
		}
	}	
}


BOOL CTopoSurfaceNoBreakCommand::IsInside(CGeometry *pObj_inside, CGeometry *pObj_outside)
{
	//1、前者外包必须落在后者外包之内；
	Envelope e1 = pObj_inside->GetEnvelope();
	Envelope e2 = pObj_outside->GetEnvelope();
	if( !e2.bEnvelopeIn(&e1) )
		return FALSE;
	
	//2、前者节点必须全部落在后者母线构成的区域内部；
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2, arrPts3;
	pObj_inside->GetShape(arrPts1);
	pObj_outside->GetShape(arrPts2);
	if(arrPts1.GetSize()<=0)
		return FALSE;

	if( GraphAPI::GIsPtInRegion(arrPts1[0],arrPts2.GetData(),arrPts2.GetSize())!=2 )
		return FALSE;
	
	return TRUE;
}

void CTopoSurfaceNoBreakCommand::face_trace()
{
	int i=0, j=0;
	for(i=0; i<arc_list.GetSize(); i++)
	{
		if(arc_list[i].state != UNVISITED)
		{
			continue;//跳过自闭合、断开的弧
		}

		if(arc_list[i].SEVisited && arc_list[i].ESVisited)
		{
			arc_list[i].state = HANDLED;
			continue;
		}

		//s->e最小角遍历
		if(!arc_list[i].SEVisited)
		{
			arc_list[i].SEVisited = TRUE;
			//定义一个指向该面尾点的指针；
			TEndPoint *face_ep = &arc_list[i].ep;

			TConnectItem *pc = NULL;
L0:			pc = face_ep->connects->next;
			while(pc)
			{
				if(arc_list[pc->index].state == UNVISITED)
				{
					break;//找到第一条可用的弧
				}
				pc = pc->next;
			}

			if(pc && pc->visited<i)
			{
				face_ep->next_arc = pc;
				pc->visited = i;
				if(pc->StoE && !arc_list[pc->index].SEVisited)
				{
					if(arc_list[i].sp == arc_list[pc->index].ep)//闭合
					{
						create_face(i, TRUE);
					}
					else//非闭合，查找后续弧
					{
						face_ep = &arc_list[pc->index].ep;
						goto L0;
					}
				}
				else if(!pc->StoE && !arc_list[pc->index].ESVisited)
				{
					if(arc_list[i].sp == arc_list[pc->index].sp)//闭合
					{
						create_face(i, TRUE);
					}
					else//非闭合，查找后续弧
					{
						face_ep = &arc_list[pc->index].sp;
						goto L0;
					}
				}
			}
		}

		//e->s最小角遍历
		if(!arc_list[i].ESVisited)
		{
			arc_list[i].ESVisited = TRUE;
			//定义一个指向该面尾点的指针；
			TEndPoint *face_ep = &arc_list[i].sp;
			
			TConnectItem *pc = NULL;
L1:			pc = face_ep->connects->next;
			while(pc)
			{
				if(arc_list[pc->index].state == UNVISITED)
				{
					break;//找到第一条可用的弧
				}
				pc = pc->next;
			}

			if(pc && pc->visited<i )
			{
				face_ep->next_arc = pc;
				pc->visited = i;
				if(pc->StoE && !arc_list[pc->index].SEVisited)
				{
					if(arc_list[i].ep == arc_list[pc->index].ep)//闭合
					{
						create_face(i, FALSE);
					}
					else//非闭合，查找后续弧
					{
						face_ep = &arc_list[pc->index].ep;
						goto L1;
					}
				}
				else if(!pc->StoE && !arc_list[pc->index].ESVisited)
				{
					if(arc_list[i].ep == arc_list[pc->index].sp)//闭合
					{
						create_face(i, FALSE);
					}
					else//非闭合，查找后续弧
					{
						face_ep = &arc_list[pc->index].sp;
						goto L1;
					}
				}
			}
		}

		arc_list[i].state=HANDLED;
	}
}

void CTopoSurfaceNoBreakCommand::clear()
{
	geo_list.RemoveAll();
	in_layer_list.RemoveAll();
	//
	for (int i=0;i<arc_list.GetSize();++i)
	{
       TConnectItem *p0=arc_list[i].sp.connects;
	   TConnectItem *p1=p0->next;
	   delete p0;
	   while(p1!=0)
	   {
		  p0=p1;
		  p1=p1->next;
		  delete p0;
	   }
	   //
	   p0=arc_list[i].ep.connects;
	   p1=p0->next;
	   delete p0;
	   while(p1!=0)
	   {
		   p0=p1;
		   p1=p1->next;
		   delete p0;
	   }
	   //
	   arc_list[i].face_index_list->RemoveAll();
	}
	arc_list.RemoveAll();
	//
	for(int j=0;j<face_list.GetSize();++j)
	{
		face_list[j].arc_item_list->RemoveAll();
		//delete [] face_list[i].datas;
	}
	face_list.RemoveAll();

	for(i=0; i<layid_list.GetSize(); i++)
	{
		layid_list[i].pFtr=NULL;
		layid_list[i].layids->RemoveAll();
	}
	layid_list.RemoveAll();
}

BOOL CTopoSurfaceNoBreakCommand::build_surface()
{
    if(get_geo_data()==0)
	{
		return TRUE;
	}
	create_arc_list();
	find_closed_arc();
	face_trace();
	//set_color();
	save_data();
	clear();
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CTopoSurfaceFromPtCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CTopoSurfaceFromPtCommand Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTopoSurfaceFromPtCommand,CEditCommand)

CTopoSurfaceFromPtCommand::CTopoSurfaceFromPtCommand()
{
	m_StoreLayer = _T("30");
	strcat(m_strRegPath,"\\TopoSurfaceFromPt");
}

CTopoSurfaceFromPtCommand::~CTopoSurfaceFromPtCommand()
{
}

CString CTopoSurfaceFromPtCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TOPO_FROMPT);
}

void CTopoSurfaceFromPtCommand::Start()
{
	if( !m_pEditor )return;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
	GOutPut(StrFromResID(IDS_INSURE_VISIBLE));
}

void CTopoSurfaceFromPtCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CTopoSurfaceFromPtCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CTopoSurfaceFromPtCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_StoreLayer);
	tab.AddValue("Layer1",&CVariantEx(var));
}

void CTopoSurfaceFromPtCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("TopoSurfaceFromPtCommand",Name());

	param->AddLayerNameParam("Layer1",(LPCTSTR)m_StoreLayer,StrFromResID(IDS_CMDPLANE_STORELAYER),NULL,LAYERPARAMITEM_AREA);
}

void CTopoSurfaceFromPtCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"Layer1",var) )
	{					
		m_StoreLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_StoreLayer.TrimRight();
		m_StoreLayer.TrimLeft();
		SetSettingsModifyFlag();		
	}

	CEditCommand::SetParams(tab,bInit);
}

//判断一组地物的图层和属性是否一致
static int IsFtrSameLayerName(CFtrArray& arrFtr)
{
	int nObj = arrFtr.GetSize();
	if(nObj==1)
	{
		return 1;
	}
	else if(nObj>1)
	{
		CDlgDataSource *pDS = GetActiveDlgDoc()->GetDlgDataSource();
		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(arrFtr[0]);
		
		int i=0;
		for(i=1; i<nObj; i++)
		{
			CFtrLayer *pLayer1 = pDS->GetFtrLayerOfObject(arrFtr[i]);
			if(stricmp(pLayer1->GetName(),pLayer->GetName())!=0)
			{
				return 0;
			}
		}	
		
		return 1;
	}
	else
	{
		return 0;
	}
}

void CTopoSurfaceFromPtCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if(!pDS || !pDQ) return;
	if(m_StoreLayer.IsEmpty()) return;
	
	CFtrLayer *out_layer = pDS->GetOrCreateFtrLayer((LPCTSTR)m_StoreLayer);
	if(out_layer==NULL) return;

	//拷贝屏幕范围内的图层
	CRect rc;
	GetActiveView()->GetClientRect(&rc);
	PT_4D pt4ds[4];
	pt4ds[0].x = rc.left; pt4ds[0].y = rc.top;  pt4ds[0].z = rc.left;  pt4ds[0].yr = rc.top;
	pt4ds[1].x = rc.right; pt4ds[1].y = rc.top;  pt4ds[1].z = rc.right;  pt4ds[1].yr = rc.top;
	pt4ds[2].x = rc.right; pt4ds[2].y = rc.bottom;  pt4ds[2].z = rc.right;  pt4ds[2].yr = rc.bottom;
	pt4ds[3].x = rc.left; pt4ds[3].y = rc.bottom;  pt4ds[3].z = rc.left;  pt4ds[3].yr = rc.bottom;
	m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(pt4ds,4);
	PT_3D pt3ds[4];
	int i=0;
	for(i=0; i<4; i++)
	{
		pt3ds[i] = pt4ds[i].To3D();
	}
	
	Envelope e0;
	e0.CreateFromPts(pt3ds, 4);
	int num = pDQ->FindObjectInRect(e0,NULL,FALSE,FALSE), k = -1;
	const CPFeature *ftrs = pDQ->GetFoundHandles(num);
	CFeature *pFtr = NULL;
	CFtrArray oldftrs;
	for(i=0; i<num; i++)
	{
		CFeature *pNew = ftrs[i]->Clone();
		pNew->SetAppFlag(m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(ftrs[i])));
		oldftrs.Add(pNew);
	}

	//调用非打断构面
	CTopoSurfaceNoBreakCommand cmd;
	cmd.Init(m_pEditor);
	CFtrArray newftrs;//存放旧地物
	cmd.BuildSurface(oldftrs, newftrs);
	
	//找到包含单点的面
	CUndoFtrs undo(m_pEditor,Name());
	int nObj = newftrs.GetSize();
	for(i=0; i<nObj; i++)
	{
		CFeature* pFtr = newftrs[i];
		CGeometry *pGeo = pFtr->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) ) continue;
		if(2!=pGeo->bPtIn(&pt)) continue;

		CFtrLayer *pStoreLayer = out_layer;
		if(pFtr->GetAppFlag()>0)
		{
			pStoreLayer = pDS->GetFtrLayer(pFtr->GetAppFlag());
		}

		int nCls = pGeo->GetClassType();
		CFeature *pNew = pStoreLayer->CreateDefaultFeature(((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetScale(),nCls);
		if(!pNew) return;
		pNew->SetID(OUID());
		pNew->SetGeometry(pGeo->Clone());
		pNew->SetPurpose(FTR_EDB);

		BOOL bOK = FALSE;
		if(1)//通过面域点刷属性
		{
			Envelope e = pGeo->GetEnvelope();
			if( pDQ->FindObjectInRect(e,NULL,TRUE,FALSE) > 0 )
			{
				int num, pos=-1;
				const CPFeature *ftrs = pDQ->GetFoundHandles(num);
				CFtrArray SPArr;
				for(int k=0; k<num; k++)
				{
					if(ftrs[k]->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)))
					{
						PT_3D pt0;
						COPY_3DPT(pt0, ftrs[k]->GetGeometry()->GetDataPoint(0));
						if(pGeo->bPtIn(&pt0))
						{
							SPArr.Add(ftrs[k]);
						}
					}
				}
				
				if(1==IsFtrSameLayerName(SPArr))
				{
					CFtrLayer *pLayer1 = pDS->GetFtrLayerOfObject(SPArr[0]);
					CGeometry* pNewObj = pNew->GetGeometry();
					((CGeoSurface*)pNewObj)->EnableFillColor(FALSE,0);
					pNewObj->SetColor(FTRCOLOR_BYLAYER);
					m_pEditor->AddObject(pNew, pLayer1->GetID());
					GETXDS(m_pEditor)->CopyXAttributes(SPArr[0], pNew);
					bOK = TRUE;
				}
			}
		}
		if(!bOK)
		{
			if(!m_pEditor->AddObject(pNew,pStoreLayer->GetID()))
			{
				delete pNew;
			}
			else
			{
				bOK = TRUE;
			}
		}
		if(bOK)
		{
			undo.AddNewFeature(FtrToHandle(pNew));
		}
		break;
	}
	undo.Commit();

	for(i=0; i<nObj; i++)
	{
		delete newftrs[i];
	}
	
	Finish();
	CEditCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// CTopoDelSurfaceCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CTopoDelSurfaceCommand/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTopoDelSurfaceCommand,CEditCommand)

CTopoDelSurfaceCommand::CTopoDelSurfaceCommand()
{
	strcat(m_strRegPath,"\\TopoDelSurface");
}

CTopoDelSurfaceCommand::~CTopoDelSurfaceCommand()
{
	
}

CString CTopoDelSurfaceCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TOPO_DEL);
}

void CTopoDelSurfaceCommand::Start()
{
	if( !m_pEditor )return;
	
	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->CloseSelector();
	
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CTopoDelSurfaceCommand::Finish()
{
 	UpdateParams(TRUE);
 	
 	m_pEditor->DeselectAll();
 	m_pEditor->OnSelectChanged();
 	m_pEditor->RefreshView();
 	CEditCommand::Finish();
}

void CTopoDelSurfaceCommand::Abort()
{	
	UpdateParams(TRUE);
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

BOOL CTopoDelSurfaceCommand::delete_surface()
{
	m_pEditor->SelectAll();

	CSelection* pSel = m_pEditor->GetSelection();
	
	int num;
	const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
	CUndoFtrs undo(m_pEditor,Name());
	for (int i=num-1; i>=0; i--)
	{
		if(HandleToFtr(handles[i])->GetPurpose()==FTR_EDB)
		{
            undo.arrOldHandles.Add(handles[i]);
		}
	}		
	for( i=num-1; i>=0; i--)
	{
		if(HandleToFtr(handles[i])->GetPurpose()==FTR_EDB)
		{
		  m_pEditor->DeleteObject(handles[i]);
		}
	}
	undo.Commit();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	return TRUE;
}

void CTopoDelSurfaceCommand::PtClick(PT_3D &pt, int flag)
{
  delete_surface();
  Finish();
  CEditCommand::PtClick(pt,flag);
}



//////////////////////////////////////////////////////////////////////
// CTopoReplaceLinesCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CTopoReplaceLinesCommand/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTopoReplaceLinesCommand,CEditCommand)

CTopoReplaceLinesCommand::CTopoReplaceLinesCommand()
{
	strcat(m_strRegPath,"\\TopoReplaceLines");
}

CTopoReplaceLinesCommand::~CTopoReplaceLinesCommand()
{
	
}

CString CTopoReplaceLinesCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TOPO_REPLACELINES);
}

void CTopoReplaceLinesCommand::Start()
{
	if( !m_pEditor )return;
	
	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->CloseSelector();
	
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CTopoReplaceLinesCommand::Finish()
{
 	UpdateParams(TRUE);
 	
 	m_pEditor->DeselectAll();
 	m_pEditor->OnSelectChanged();
 	m_pEditor->RefreshView();
 	CEditCommand::Finish();
}

void CTopoReplaceLinesCommand::Abort()
{	
	UpdateParams(TRUE);
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

BOOL CTopoReplaceLinesCommand::replace_lines()
{
	m_pEditor->SelectAll();

	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS)
		return FALSE;

	CFtrArray ftrs1;
	
	int nSum = 0, i, j;
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

			if(pFtr->GetPurpose()==FTR_EDB && pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{	
				nSum++;
				ftrs1.Add( pFtr );			
			}	
		}
	}

	CUndoFtrs undo(m_pEditor,Name());
	for(int m=0; m<nSum; m++)
	{
		CFeature *ftr1 = ftrs1[m];

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

				if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)pFtr->GetGeometry())->IsClosed())
				{
					CFeature *ftr2 = pFtr;
					if(CDelAllSameCommand::IsWholeOverlapedObj(pDS,ftr1,ftr2,FALSE))
					{
						CFeature *pNewFtr = ftr1->Clone();

						m_pEditor->DeleteObject(FtrToHandle(ftr1));
						m_pEditor->DeleteObject(FtrToHandle(ftr2));

						undo.arrOldHandles.Add(FtrToHandle(ftr1));
						undo.arrOldHandles.Add(FtrToHandle(ftr2));
						
						m_pEditor->AddObject(pNewFtr,pLayer->GetID());

						undo.arrNewHandles.Add(FtrToHandle(pNewFtr));
					}		
				}		
			}
		}
	}

	undo.Commit();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	return TRUE;
}

void CTopoReplaceLinesCommand::PtClick(PT_3D &pt, int flag)
{
  replace_lines();
  Finish();
  CEditCommand::PtClick(pt,flag);
}


//获得对象中在start和end之间的点
static void GetObjectPart(CArray<PT_3DEX,PT_3DEX>& arr, int start, int end, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	PT_3DEX expt;
	
	for( int i=start; i<=end; i++)
	{
		expt = arr[i];
		arrPts.Add(expt);
	}
}


//////////////////////////////////////////////////////////////////////
// CCreateFigureCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CCreateFigureCommand/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCreateFigureCommand,CDrawCommand)

CCreateFigureCommand::CCreateFigureCommand()
{
	strcat(m_strRegPath,"\\CreateFigure");
}

CCreateFigureCommand::~CCreateFigureCommand()
{
	
}

CString CCreateFigureCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CREATEFIGURE);
}

void CCreateFigureCommand::Start()
{
	if( !m_pEditor )return;
	
	CDrawCommand::Start();
	
	GOutPut(StrFromResID(IDS_CMDTIP_SET_FIGURE_ORIGIN));
}



void CCreateFigureCommand::PtClick(PT_3D &pt, int flag)
{
	CDrawCommand::PtClick(pt,flag);

	PDOC(m_pEditor)->CreateFigure(-pt.x,-pt.y);
	Finish();
	AfxMessageBox(IDS_SAVE_SUCCEED);
}


//////////////////////////////////////////////////////////////////////
// CCopyXAttrOfPointCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCopyXAttrOfPointCommand,CEditCommand)

CCopyXAttrOfPointCommand::CCopyXAttrOfPointCommand()
{
}

CCopyXAttrOfPointCommand::~CCopyXAttrOfPointCommand()
{
}

CString CCopyXAttrOfPointCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_COPYXARRT_P2S);
}

void CCopyXAttrOfPointCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->CloseSelector();
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}
void CCopyXAttrOfPointCommand::Finish()
{
	UpdateParams(TRUE);
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CCopyXAttrOfPointCommand::Abort()
{
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}


void CCopyXAttrOfPointCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_strLayer);
	tab.AddValue("Layer",&CVariantEx(var));

	var = (LPCTSTR)(m_strLayer2);
	tab.AddValue("Layer2",&CVariantEx(var));
}

void CCopyXAttrOfPointCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("CopyXAttrOfPoint",Name());
	
	param->AddLayerNameParamEx("Layer",(LPCTSTR)m_strLayer,StrFromResID(IDS_SURFACE_LAYER),NULL,LAYERPARAMITEM_AREA);
	param->AddLayerNameParam("Layer2", (LPCTSTR)m_strLayer2, StrFromResID(IDS_POINT_LAYER), NULL, LAYERPARAMITEM_POINT|LAYERPARAMITEM_TEXT);
}

void CCopyXAttrOfPointCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"Layer",var) )
	{					
		m_strLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"Layer2",var) )
	{					
		m_strLayer2 = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

BOOL CCopyXAttrOfPointCommand::CopyXAttr(CAttributesSource* pXDS, CFeature* pSrc, CFeature* pDes)
{
	if( !pXDS || !pSrc || !pDes ) return FALSE;

	CValueTable srcTab;
	srcTab.BeginAddValueItem();
	pXDS->GetXAttributes(pSrc,srcTab);
	srcTab.EndAddValueItem();
	int n1 = srcTab.GetFieldCount();
	if( n1 <= 0 ) return FALSE;
	
	CValueTable desTab;
	desTab.BeginAddValueItem();
	pXDS->GetXAttributes(pDes,desTab);
	desTab.EndAddValueItem();
	int n2 = desTab.GetFieldCount();
	if( n2 <= 0 ) return FALSE;


	BOOL bModified = FALSE;
	for(int i = 0; i < n1; i++)
	{
		CString field;
		int type;
		CString name;
		if( !srcTab.GetField(i, field, type, name) ) continue;
		for(int j = 0; j < n2; j++)
		{
			CString field2;
			int type2;
			CString name2;
			desTab.GetField(j, field2, type2, name2);
			if( field.CompareNoCase(field2) == 0 )
			{
				const CVariantEx* var;
				if( !srcTab.GetValue(0, field, var) ) return FALSE;
				if(	!desTab.SetValue(0, field, var) ) return FALSE;
				bModified = TRUE;
			}
		}

	}
	if( !bModified ) return FALSE;

	if( !pXDS->SetXAttributes(pDes,desTab) ) return FALSE;
	return TRUE;
}


BOOL IsPtInGeoSurface(PT_3D pt, CGeometry *pObj)
{
	if(pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
	{
		int nSon = ((CGeoMultiSurface*)pObj)->GetSurfaceNum();
		int nInside = 0;
		for(int i=0; i<nSon; i++)
		{
			CArray<PT_3DEX,PT_3DEX> pts;
			((CGeoMultiSurface*)pObj)->GetSurface(i,pts);

			CGeoSurface son;
			son.CreateShape(pts.GetData(),pts.GetSize());

			pts.RemoveAll();
			son.GetShape()->GetPts(pts);

			if(GraphAPI::GIsPtInRegion(PT_3DEX(pt,penLine),pts.GetData(),pts.GetSize())==2)
			{
				nInside++;
			}
		}

		if((nInside%2)==0)
			return FALSE;
		else
			return TRUE;
	}
	else if(pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		CArray<PT_3DEX,PT_3DEX> pts;
		((CGeoSurface*)pObj)->GetShape()->GetPts(pts);

		if(GraphAPI::GIsPtInRegion(PT_3DEX(pt,penLine),pts.GetData(),pts.GetSize())==2)
		{
			return TRUE;
		}		
	}
	return FALSE;
}


void CCopyXAttrOfPointCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	if( !pDS || !pDQ ) return;
	CAttributesSource *pXDS = pDS->GetXAttributesSource();
	if( !pXDS ) return;

	pDS->SaveAllQueryFlags(TRUE,FALSE);
	//1.挑出所有面
	int i =0 , j =0;
	CFtrArray ftrs;			//面对象集合
	CStringArray arr;
	if(!convertStringToStrArray(m_strLayer,arr))
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
			for(j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;	
				if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					continue;
				ftrs.Add( pFtr );
			}
		}
	}

	//2.获取点的层码
	CFtrLayer *pPointLayer = pDS->GetFtrLayer(m_strLayer2);
	if(!pPointLayer) return;
	pPointLayer->SetAllowQuery(TRUE);
	
	//3.在这个面的外包矩形内找点
	int num_handled = 0;
	int num = ftrs.GetSize();
	CUndoModifyProperties undo(m_pEditor, Name());
	GProgressStart(num);

	for(i = 0; i < num; i++)
	{
		GProgressStep();
		CFeature* pFtr = ftrs.GetAt( i );
		CFtrLayer* pLayer1 = pDS->GetFtrLayerOfObject(pFtr);

		CGeometry *pGeo = pFtr->GetGeometry();
		if(!pGeo) continue;
		
		Envelope e = pGeo->GetEnvelope();

		int n = pDQ->FindObjectInRect(e,NULL,FALSE,FALSE);
		//int n = pDQ->FindObjectInPolygon(arrPts.GetData(), arrPts.GetSize(),NULL,FALSE);

		const CPFeature *pftrs = pDQ->GetFoundHandles(n);
		for(j = 0; j < n; j++)
		{
			CFeature* pFound = pftrs[j];
			CGeometry *pFoundGeo = pFound->GetGeometry();
			if(!pFoundGeo) continue;

			//找点状态地物
			if(!pFoundGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) && !pFoundGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
				continue;

			//和参数中的点同层
			CFtrLayer* pLayer2 = pDS->GetFtrLayerOfObject(pFound);
			if(!pLayer2) continue;
			if(pPointLayer->GetID() != pLayer2->GetID())
				continue;

			//必须在面内部
			if( pFound->GetGeometry()->GetDataPointSum()<=0 ) continue;
			PT_3DEX pt = pFound->GetGeometry()->GetDataPoint(0);
			BOOL ret = IsPtInGeoSurface(pt,pGeo);
			if(!ret)continue;

			//将扩展属性复制到面上
			CValueTable xTab1, xTab2, xTab0;
			xTab1.BeginAddValueItem();
			pXDS->GetXAttributes(pFtr,xTab1);
			xTab1.EndAddValueItem();

			xTab2.BeginAddValueItem();
			pXDS->GetXAttributes(pFound,xTab2);
			xTab2.EndAddValueItem();
			
			xTab0.CopyFrom(xTab1);

			BOOL bModified = FALSE;
			for(int k=0; k<xTab2.GetFieldCount(); k++)
			{
				CString field, name;
				int ntype;
				xTab2.GetField(k,field,ntype,name);

				const CVariantEx *value = NULL;
				xTab2.GetValue(0,k,value);

				if(xTab1.SetValue(0,field,(CVariantEx*)value))
				{
					bModified = TRUE;
				}
			}

			if(bModified)
			{
				m_pEditor->DeleteObject( FtrToHandle(pFtr));

				m_pEditor->RestoreObject( FtrToHandle(pFtr));
				
				pXDS->SetXAttributes(pFtr,xTab1);

				undo.SetModifyProperties(FtrToHandle(pFtr),xTab0,xTab1,FALSE);
				num_handled++;
			}			
		}
	}

	pDS->RestoreAllQueryFlags();

	CString strMsg;
	strMsg.Format(IDS_CMDTIP_OBJS_HANDLED,num_handled);
	GOutPut(strMsg);

	GProgressEnd();
	undo.Commit();
	Finish();
	CEditCommand::PtClick(pt,flag);
}



//////////////////////////////////////////////////////////////////////
// CTransformGB2CCCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTransformGB2CCCommand,CEditCommand)

CTransformGB2CCCommand::CTransformGB2CCCommand()
{
}

CTransformGB2CCCommand::~CTransformGB2CCCommand()
{
}

CString CTransformGB2CCCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TRANSFORM_GBTOCC);
}

void CTransformGB2CCCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->CloseSelector();
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}
void CTransformGB2CCCommand::Finish()
{
	UpdateParams(TRUE);
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CTransformGB2CCCommand::Abort()
{
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}


void CTransformGB2CCCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_strLayer);
	tab.AddValue("Layer",&CVariantEx(var));
}

void CTransformGB2CCCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("TransformGB2CC",Name());
	
	param->AddLayerNameParamEx("Layer",(LPCTSTR)m_strLayer,StrFromResID(IDS_CMDPLANE_HANDLELAYER));
}

void CTransformGB2CCCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"Layer",var) )
	{					
		m_strLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CTransformGB2CCCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if( !pDS ) return;
	CAttributesSource *pXDS = pDS->GetXAttributesSource();
	if( !pXDS ) return;

	//1. 将对照表读到一个缓冲区中
	CString path;
	path = GetConfigPath(TRUE)+_T("\\GBtoCC.txt");
	std::ifstream fin(path);
	if(!fin)
	{
		GOutPut(StrFromResID(IDS_CMDTIP_REF_FILE_ERROR));
		return;
	}

	std::map<std::string,std::string> buf;
	std::string strRead;
	while( getline(fin,strRead) )
	{
		int pos = strRead.find_first_of(' ');
		if( pos != std::string::npos )
		{
			int len = strRead.length();
			std::string gbCode = strRead.substr(0,pos);
			std::string ccCode = strRead.substr(pos+1,len-pos-1);
			buf.insert( std::pair<string,string>(gbCode,ccCode) );
		}
	};

	fin.close();

	//2. 挑出所有地物
	int i =0 , j =0;
	CFtrArray ftrs;
	
	if( m_strLayer.IsEmpty() )
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
		if(!convertStringToStrArray(m_strLayer,arr))
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
		
	}

	//3. 匹配修改
	int num_handled = 0;
	int num = ftrs.GetSize();
	CUndoFtrs undo(m_pEditor, Name());
	GProgressStart(num);
	
	for(i = 0; i < num; i++)
	{
		GProgressStep();
		CFeature* pFtr = ftrs.GetAt( i );
		
		CValueTable xTab;
		xTab.BeginAddValueItem();
		pXDS->GetXAttributes(pFtr,xTab);
		xTab.EndAddValueItem();
		
		if( UpdateXAttr(xTab, buf) )
		{
			CFeature* pNew = pFtr->Clone();
			if( m_pEditor->AddObject( pNew, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr))) )
				undo.AddNewFeature(FtrToHandle( pNew ));
			pXDS->SetXAttributes(pNew,xTab);
			
			if( m_pEditor->DeleteObject( FtrToHandle(pFtr)) )
				undo.AddOldFeature(FtrToHandle( pFtr ));

			num_handled++;
		}
	}

	CString strMsg;
	strMsg.Format(IDS_CMDTIP_OBJS_HANDLED,num_handled);
	GOutPut(strMsg);

	GProgressEnd();
	undo.Commit();
	Finish();
	CEditCommand::PtClick(pt,flag);
}

BOOL CTransformGB2CCCommand::UpdateXAttr(CValueTable& tab,std::map<string,string>& refTable)
{
	if( tab.GetFieldCount() < 2 ) return FALSE;

	const CVariantEx *var;
	CString gbAttr;
	CString ccAttr;
	
	if( tab.GetValue(0,"GB",var) )
	{					
		gbAttr = (LPCTSTR)(_bstr_t)(_variant_t)*var;		
	}

	if( tab.GetValue(0,"CC",var) )
	{					
		ccAttr = (LPCTSTR)(_bstr_t)(_variant_t)*var;		
	}

	std::string gbCode( gbAttr.GetBuffer(gbAttr.GetLength()) );
	std::string ccCode( ccAttr.GetBuffer(ccAttr.GetLength()) );

	std::map<string,string>::iterator it;
	it = refTable.find(gbCode);
	if( it == refTable.end() ) return FALSE;
	if( (it->second).compare(ccCode) == 0) return FALSE;	//与参照表中的值相等，不需要修改
	ccCode = it->second;

	_variant_t value = (LPCTSTR)(ccCode.c_str());
	if( !tab.SetValue(0,"CC",&CVariantEx(value)) ) return FALSE;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CTransformCC2GBCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTransformCC2GBCommand,CEditCommand)

CTransformCC2GBCommand::CTransformCC2GBCommand()
{
}

CTransformCC2GBCommand::~CTransformCC2GBCommand()
{
}

CString CTransformCC2GBCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_TRANSFORM_CCTOGB);
}

void CTransformCC2GBCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	m_pEditor->CloseSelector();
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}
void CTransformCC2GBCommand::Finish()
{
	UpdateParams(TRUE);
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CTransformCC2GBCommand::Abort()
{
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}


void CTransformCC2GBCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_strLayer);
	tab.AddValue("Layer",&CVariantEx(var));
}

void CTransformCC2GBCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("TransformCC2GB",Name());
	
	param->AddLayerNameParamEx("Layer",(LPCTSTR)m_strLayer,StrFromResID(IDS_CMDPLANE_HANDLELAYER));
}

void CTransformCC2GBCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"Layer",var) )
	{					
		m_strLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CTransformCC2GBCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if( !pDS ) return;
	CAttributesSource *pXDS = pDS->GetXAttributesSource();
	if( !pXDS ) return;

	//1. 将对照表读到一个缓冲区中
	CString path;
	path = GetConfigPath(TRUE)+_T("\\GBtoCC.txt");
	std::ifstream fin(path);
	if(!fin)
	{
		GOutPut(StrFromResID(IDS_CMDTIP_REF_FILE_ERROR));
		return;
	}

	std::map<std::string,std::string> buf;
	std::string strRead;
	while( getline(fin,strRead) )
	{
		int pos = strRead.find_first_of(' ');
		if( pos != std::string::npos )
		{
			int len = strRead.length();
			std::string gbCode = strRead.substr(0,pos);
			std::string ccCode = strRead.substr(pos+1,len-pos-1);
			buf.insert( std::pair<string,string>(ccCode,gbCode) );
		}
	};

	fin.close();

	//2. 挑出所有地物
	int i =0 , j =0;
	CFtrArray ftrs;
	
	if( m_strLayer.IsEmpty() )
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
		if(!convertStringToStrArray(m_strLayer,arr))
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
		
	}

	//3. 匹配修改
	int num_handled = 0;
	int num = ftrs.GetSize();
	CUndoFtrs undo(m_pEditor, Name());
	GProgressStart(num);
	
	for(i = 0; i < num; i++)
	{
		GProgressStep();
		CFeature* pFtr = ftrs.GetAt( i );
		
		CValueTable xTab;
		xTab.BeginAddValueItem();
		pXDS->GetXAttributes(pFtr,xTab);
		xTab.EndAddValueItem();
		
		if( UpdateXAttr(xTab, buf) )
		{
			CFeature* pNew = pFtr->Clone();
			if( m_pEditor->AddObject( pNew, m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(pFtr))) )
				undo.AddNewFeature(FtrToHandle( pNew ));
			pXDS->SetXAttributes(pNew,xTab);
			
			if( m_pEditor->DeleteObject( FtrToHandle(pFtr)) )
				undo.AddOldFeature(FtrToHandle( pFtr ));

			num_handled++;
		}
	}

	CString strMsg;
	strMsg.Format(IDS_CMDTIP_OBJS_HANDLED,num_handled);
	GOutPut(strMsg);

	GProgressEnd();
	undo.Commit();
	Finish();
	CEditCommand::PtClick(pt,flag);
}

BOOL CTransformCC2GBCommand::UpdateXAttr(CValueTable& tab,std::map<string,string>& refTable)
{
	if( tab.GetFieldCount() < 2 ) return FALSE;

	const CVariantEx *var;
	CString gbAttr;
	CString ccAttr;
	
	if( tab.GetValue(0,"GB",var) )
	{					
		gbAttr = (LPCTSTR)(_bstr_t)(_variant_t)*var;		
	}

	if( tab.GetValue(0,"CC",var) )
	{					
		ccAttr = (LPCTSTR)(_bstr_t)(_variant_t)*var;		
	}

	std::string gbCode( gbAttr.GetBuffer(gbAttr.GetLength()) );
	std::string ccCode( ccAttr.GetBuffer(ccAttr.GetLength()) );

	std::map<string,string>::iterator it;
	it = refTable.find(ccCode);//查找CC对应的到下表
	if( it == refTable.end() ) return FALSE;
	if( (it->second).compare(gbCode) == 0) return FALSE;	//与参照表中的值相等，不需要修改
	gbCode = it->second;

	_variant_t value = (LPCTSTR)(gbCode.c_str());
	if( !tab.SetValue(0,"GB",&CVariantEx(value)) ) return FALSE;

	return TRUE;
}



//////////////////////////////////////////////////////////////////////
// CModifyCutLinesCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CModifyCutLinesCommand,CCommand)

CModifyCutLinesCommand::CModifyCutLinesCommand()
{

}

CModifyCutLinesCommand::~CModifyCutLinesCommand()
{

}

CString CModifyCutLinesCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_CUTLINES);
}

void CModifyCutLinesCommand::Start()
{
	if( !m_pEditor )return;

	CEditCommand::Start();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();
	m_nStep = 0;
	
	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
	
}

void CModifyCutLinesCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;

	CFtrLayerArray arrLayers;
	pDS->GetFtrLayersByNameOrCode_editable(m_handleLayers,arrLayers);

	int nSum = 0, i=0;
	pDS->SaveAllQueryFlags(TRUE, FALSE);
	for( i=0; i<arrLayers.GetSize(); i++)
	{
		nSum += arrLayers[i]->GetValidObjsCount();
		
		arrLayers[i]->SetAllowQuery(TRUE);
	}	
	
	CUndoBatchAction undo(m_pEditor,Name());
	
	GProgressStart(5*nSum);
	
	//线串化, Progress: 1*nSum
	Linearize(arrLayers,&undo);
	
	//打断自相交线, Progress: 2*nSum
	CutLines_self(arrLayers,&undo);
	
	//打断相交线, Progress: 2*nSum
	CutLines1(arrLayers,&undo);

	GProgressEnd();

	pDS->RestoreAllQueryFlags();
	
	undo.Commit();
	
	Finish();

	CCommand::PtClick(pt, flag);
}

void CModifyCutLinesCommand::GetParams(CValueTable &tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_handleLayers);
	tab.AddValue("handleLayers",&CVariantEx(var));
}


void CModifyCutLinesCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ModifyCutLines", Name());
	
	param->AddLayerNameParamEx("handleLayers",(LPCTSTR)m_handleLayers,StrFromResID(IDS_CMDPLANE_HANDLELAYER));
}



void CModifyCutLinesCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"handleLayers",var) )
	{
		m_handleLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_handleLayers.TrimLeft();
		m_handleLayers.TrimRight();
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab);
}

BOOL CModifyCutLinesCommand::CutLines1(CFtrLayerArray& layers0, CUndoBatchAction* pUndo)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}

	//计算所有的交点，并将交点信息放入数组
	
	CDataSourceEx *pDS = m_pEditor->GetDataSource();

	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}

	ftrsArray.SetSize(j);

	IntersectItemArray arr;

	for( i=0; i<j; i++)
	{
		GProgressStep();
		
		Envelope e = ftrsArray[i]->GetGeometry()->GetEnvelope();
		e.Inflate(r,r,0);
		int num = pDQ->FindObjectInRect_SimpleAndSym(e,NULL,FALSE,FALSE);
		
		const CPFeature *ftrs = pDQ->GetFoundHandles(num);
		
		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()<=i )continue;

			//不跨图层
			//int id1 = pDS->GetFtrLayerOfObject(ftrsArray[i])->GetID();
			//int id2 = pDS->GetFtrLayerOfObject(ftrs[k])->GetID();
			//if(id1 != id2) continue;

			GetIntersects(ftrsArray[i],ftrs[k],arr);
		}
/*
		for( k=i+1; k<j; k++)
		{
			GetIntersects(ftrsArray[i],ftrsArray[k],arr);
		}
*/
	}

	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}

	if( arr.GetSize()<=1 )
		return TRUE;

	//按照地物和交点位置的顺序对交点排序
	qsort(arr.GetData(),arr.GetSize(),sizeof(IntersectItem),compare_intersect_item);
	
	//将重复的清除，
	for( i=arr.GetSize()-1; i>0; i--)
	{
		if( arr[i].pFtr==arr[i-1].pFtr && fabs(arr[i].t-arr[i-1].t)<1e-8 && arr[i].ptIdx==arr[i-1].ptIdx  )
			arr.RemoveAt(i);
	}
	
	IntersectItem *data = arr.GetData();
	nSum = arr.GetSize();
/*	
	//刚好为连续顶点的，就取其头尾，使得它们能成为连续的一段
	int clear_start = 0, clear_stop = 0;
	for( i=1; i<nSum; i++)
	{		
		if( data[i].pFtr==data[i-1].pFtr && fabs(data[i].t)<1e-8 && fabs(data[i-1].t)<1e-8 && data[i].ptIdx==(data[i-1].ptIdx+1) )
		{
			clear_stop++;

			if( i!=(nSum-1) )
				continue;
		}
		
		if( clear_stop-clear_start>1 )
		{
			for( k=clear_start+1; k<clear_stop; k++)
			{
				data[k] = IntersectItem();
			}
		}
		
		clear_stop = clear_start = i;
	}
*/	
	//将空交点删除
	for( i=nSum-1; i>=0; i--)
	{
		if( arr[i].pFtr==NULL )
			arr.RemoveAt(i);
	}

	//对每个地物依次处理：用该地物的N个交点将地物打断成N+1份
	data = arr.GetData();
	nSum = arr.GetSize();

	CUndoFtrs undo(m_pEditor,Name());

	int startItem = 0, stopItem = -1;
	for( i=0; i<nSum; i++)
	{
		GProgressStep();

		if( i!=(nSum-1) && data[i].pFtr==data[i+1].pFtr )
			continue;

		stopItem = i+1;

		CFeature *pFtr = data[startItem].pFtr;
		CGeometry *pGeo = pFtr->GetGeometry();
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);

		//处理start~stop之间的交点
		int ptIndex0 = 0, ptIndex1 = 0;
		for( j=startItem; j<stopItem; j++)
		{
			ptIndex1 = data[j].ptIdx;
			ptIndex1 = pShp->GetKeyPos(ptIndex1);

			//在 ptIndex1+1 处插入顶点，取 ptIndex0 ~ ptIndex1 之间的点构成新地物
			
			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);
			pts2.InsertAt(ptIndex1+1,PT_3DEX(data[j].pt,penLine));
			
			if( j>startItem )
			{
				pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			}

			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+2-ptIndex0);

			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pEditor->AddObject(pNewFtr,pDS->GetFtrLayerOfObject(pFtr)->GetID());
					GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNewFtr);
					undo.AddNewFeature(FtrToHandle(pNewFtr));
				}
				else
				{
					delete pNewFtr;
				}
			}

			ptIndex0 = ptIndex1;
		}

		//最后一段
		if( stopItem>startItem )
		{
			ptIndex1 = pts.GetSize()-1;

			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);			
			pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			
			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+1-ptIndex0);
			
			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pEditor->AddObject(pNewFtr,pDS->GetFtrLayerOfObject(pFtr)->GetID());
					GETXDS(m_pEditor)->CopyXAttributes(pFtr,pNewFtr);
					undo.AddNewFeature(FtrToHandle(pNewFtr));
				}
				else
				{
					delete pNewFtr;
				}
			}
		}

		m_pEditor->DeleteObject(FtrToHandle(pFtr));
		undo.AddOldFeature(FtrToHandle(pFtr));

		startItem = stopItem;
	}

	pUndo->AddAction(&undo);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// CFillSurfaceATTFromPtCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CFillSurfaceATTFromPtCommand,CEditCommand)

CFillSurfaceATTFromPtCommand::CFillSurfaceATTFromPtCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\FillSurfaceATTFromPt");
	m_pDS = NULL;
}

CFillSurfaceATTFromPtCommand::~CFillSurfaceATTFromPtCommand()
{
}

CString CFillSurfaceATTFromPtCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_FILLSURFACE_FROMPT);
}

void CFillSurfaceATTFromPtCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->CloseSelector();

	m_nStep = 0;

	GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
}

void CFillSurfaceATTFromPtCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CFillSurfaceATTFromPtCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CFillSurfaceATTFromPtCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_HandleLayer);
	tab.AddValue("Layer0",&CVariantEx(var));

	var = (bool)(m_bDelSurfacePt);
	tab.AddValue("DelSurfacePt",&CVariantEx(var));
}

void CFillSurfaceATTFromPtCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("FillSurfaceATTFromPtCommand",Name());
	
	param->AddLayerNameParam("Layer0",(LPCTSTR)m_HandleLayer,StrFromResID(IDS_CMDPLANE_SURFACELAYER));

	param->AddParam("DelSurfacePt",(bool)m_bDelSurfacePt,StrFromResID(IDS_CMDNAME_DEL_SURFACEPT));
}

void CFillSurfaceATTFromPtCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Layer0",var) )
	{					
		m_HandleLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_HandleLayer.TrimLeft();
		m_HandleLayer.TrimRight();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"DelSurfacePt",var) )
	{
		m_bDelSurfacePt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}

void CFillSurfaceATTFromPtCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	m_pDS = m_pEditor->GetDataSource();
	if(!m_pDS) return;

	CFtrArray ftrs;
	if(m_HandleLayer.IsEmpty())
	{
		int nFtrLay = m_pDS->GetFtrLayerCount();
		for (int i=0;i<nFtrLay;i++)
		{
			CFtrLayer* pLayer = m_pDS->GetFtrLayerByIndex(i);
			if(!pLayer||!pLayer->IsVisible())
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j=0;j<nObj;j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					ftrs.Add(pFtr);
			}
		}
	}
	else
	{
		CFtrLayer *pLayer=m_pDS->GetFtrLayer((LPCTSTR)m_HandleLayer);
		int nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				ftrs.Add(pFtr);
		}
	}

	//读取所有点地物
	int nLayer = m_pDS->GetFtrLayerCount();
	int i=0, j=0;
	CFtrArray arrPointFtr;
	for(i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = m_pDS->GetFtrLayerByIndex(i);
		
		if (!pLayer ||!pLayer->IsVisible())
		{
			continue;
		}
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;
			if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)))
			{
				arrPointFtr.Add(pFtr);
			}
		}
	}

	CUndoFtrs undo(m_pEditor,Name());

	int nSum = ftrs.GetSize();
	GProgressStart(nSum);
	CFtrArray arrRetFtrs0;
	CArray<PT_3D,PT_3D> arrRetPts0;
	CFtrArray arrRetFtrs2;
	CArray<PT_3D,PT_3D> arrRetPts2;
	CFtrArray arrRetFtrs3;
	CArray<PT_3D,PT_3D> arrRetPts3;
	for(i=0; i<nSum; i++)
	{
		GProgressStep();
		CFeature * pFtr = ftrs[i];

		//FillAttribute函数内部根据m_bDelSurfacePt标志直接删除了面域点
		int ret = FillAttribute(arrPointFtr, pFtr, &undo);
		if(ret==0)
		{
			arrRetFtrs0.Add(pFtr);
			arrRetPts0.Add(pFtr->GetGeometry()->GetDataPoint(0));
		}
		else if(ret==1)
		{
			undo.AddOldFeature(FtrToHandle(pFtr));
			m_pDS->DeleteObject(pFtr);
		}
		else if(ret==2)
		{
			arrRetFtrs2.Add(pFtr);
			arrRetPts2.Add(pFtr->GetGeometry()->GetDataPoint(0));
		}
		else if(ret==3)
		{
			arrRetFtrs3.Add(pFtr);
			arrRetPts3.Add(pFtr->GetGeometry()->GetDataPoint(0));
		}
	}

	GProgressEnd();

	undo.Commit();

	if(arrRetFtrs0.GetSize()>0 || arrRetFtrs2.GetSize()>0 || arrRetFtrs3.GetSize()>0)
	{
		//添加到检查列表
		AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,1,0);
		
		CChkResManger *pChkRes = &GetChkResMgr();
		
		CString chkName = StrFromResID(IDS_CMDNAME_FILLSURFACE_FROMPT);
		CString chkReason0 = StrFromResID(IDS_NO_POINT_IN);
		CString chkReason2 = StrFromResID(IDS_DIFFRENT_POINT_IN);
		CString chkReason3 = StrFromResID(IDS_SURFACEPOINT_ATT_DIFFRENT);
		
		int j=0;
		for (j=0;j<arrRetFtrs0.GetSize();j++)
		{
			pChkRes->BeginResItem(chkName);
			pChkRes->AddAssociatedFtr(arrRetFtrs0[j]);
			pChkRes->SetAssociatedPos(arrRetPts0[j]);
			pChkRes->SetReason(chkReason0);
			pChkRes->EndResItem();
		}
		for (j=0;j<arrRetFtrs2.GetSize();j++)
		{
			pChkRes->BeginResItem(chkName);
			pChkRes->AddAssociatedFtr(arrRetFtrs2[j]);
			pChkRes->SetAssociatedPos(arrRetPts2[j]);
			pChkRes->SetReason(chkReason2);
			pChkRes->EndResItem();
		}
		for (j=0;j<arrRetFtrs3.GetSize();j++)
		{
			pChkRes->BeginResItem(chkName);
			pChkRes->AddAssociatedFtr(arrRetFtrs3[j]);
			pChkRes->SetAssociatedPos(arrRetPts3[j]);
			pChkRes->SetReason(chkReason3);
			pChkRes->EndResItem();
		}
		
		AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,0,0);
	}
	
	Finish();
	CEditCommand::PtClick(pt,flag);
}

int CFillSurfaceATTFromPtCommand::FillAttribute(CFtrArray& arrPtFtr, CFeature *pFtr, CUndoFtrs *undo)
{
	CGeometry *pObj = pFtr->GetGeometry();
	if(!pObj)  return 0;

	Envelope e = pObj->GetEnvelope();

	CArray<int,int> arrIndexs;
	CFtrArray arr;

	for(int i=0; i<arrPtFtr.GetSize(); i++)
	{
		PT_3DEX pt = arrPtFtr[i]->GetGeometry()->GetDataPoint(0);

		if(!e.bPtIn(&pt))
			continue;

		if(pObj->bPtIn(&pt))
		{
			arr.Add(arrPtFtr[i]);
			arrIndexs.Add(i);
		}
	}

	int ret = IsFtrSameLayerName(arr);
	if(ret==1)
	{
		CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(arr[0]);

		CFeature *pNewFtr = pFtr->Clone();
		CGeometry* pNewObj = pNewFtr->GetGeometry();
		((CGeoSurface*)pNewObj)->EnableFillColor(FALSE,0);
		pNewObj->SetColor(FTRCOLOR_BYLAYER);
		pNewFtr->SetPurpose(FTR_EDB);
		m_pEditor->AddObject(pNewFtr, pLayer->GetID());
		GETXDS(m_pEditor)->CopyXAttributes(arr[0], pNewFtr);
		undo->AddNewFeature(FtrToHandle(pNewFtr));
		m_pEditor->DeleteObject(FtrToHandle(pFtr));
		undo->AddOldFeature(FtrToHandle(pFtr));

		if(m_bDelSurfacePt)
		{
			for(int j=arr.GetSize()-1; j>=0; j--)
			{
				undo->AddOldFeature(FtrToHandle(arr[j]));
				m_pDS->DeleteObject(arr[j]);
				
				arrPtFtr.RemoveAt(arrIndexs[j]);
			}
		}

		return 1;//成功
	}
	else if(ret==2)
	{
		return 3;//面内部点的层一致，但扩展属性不一致
	}

	if(arr.GetSize()==0)
	{
		return 0;//没有面域点
	}
	else if(arr.GetSize()>1)
	{
		return 2;//内部的面域点属于不同层
	}
	return -1;
}


//////////////////////////////////////////////////////////////////////
// CSurNoOverPartCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSurNoOverPartCommand,CEditCommand)

CSurNoOverPartCommand::CSurNoOverPartCommand()
{
	m_pDS = NULL;
	m_pTempDS = NULL;
	m_pDQ = NULL;
	m_bSnap3D = FALSE;
	m_bAddPt = FALSE;
	m_lfToler = GraphAPI::g_lfDisTolerance;
	m_nStep = -1;
	strcat(m_strRegPath,"\\SurNoOverPartToArea");
}

CSurNoOverPartCommand::~CSurNoOverPartCommand()
{
}

CString CSurNoOverPartCommand::Name()
{ 
	return StrFromResID(IDS_CMDPLANE_SURNOOVERPART);
}

void CSurNoOverPartCommand::Start()
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
		
		return;
	}
	
	CEditCommand::Start();
}

void CSurNoOverPartCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CSurNoOverPartCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CSurNoOverPartCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_targetLayer);
	tab.AddValue("Layer0",&CVariantEx(var));
}

void CSurNoOverPartCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("SurNoOverPartCommand",Name());
	
	param->AddLayerNameParam("Layer0",(LPCTSTR)m_targetLayer,StrFromResID(IDS_CMDPLANE_BOUNDALYER));
}

void CSurNoOverPartCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Layer0",var) )
	{					
		m_targetLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_targetLayer.TrimLeft();
		m_targetLayer.TrimRight();
		SetSettingsModifyFlag();		
	}

	CEditCommand::SetParams(tab,bInit);
}

void CSurNoOverPartCommand::PtClick(PT_3D &pt, int flag)
{	
	if( m_nStep==0 )
	{	
		if( !CanGetSelObjs(flag) )return;
		EditStepOne();
	}
	
	if( m_nStep==1 )
	{
		if(!m_pEditor || m_targetLayer.IsEmpty())
		{
			Abort();
			return;
		}
		m_pDS = m_pEditor->GetDataSource();
		if(!m_pDS)
		{
			Abort();
			return;
		}

		GetNoOverPart();
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->RefreshView();
		Finish();
		m_nStep = 2;
	}	
	Finish();
	CEditCommand::PtClick(pt,flag);
}


void CSurNoOverPartCommand::GetNoOverPart()
{
	if(!m_pEditor) return;
	CSelection* pSel = m_pEditor->GetSelection();
	int num0, i, j;
	const FTR_HANDLE* handles = pSel->GetSelectedObjs(num0);
	if(num0<=0)	return;

	Envelope e;
	for(i=0; i<num0; i++)
	{
		CGeometry *pGeo = HandleToFtr(handles[i])->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			continue;
		Envelope e1 = pGeo->GetEnvelope();
		e.Union(&e1);
	}

	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	pDQ->FindObjectInRect(e, NULL, FALSE, FALSE);

	int num;
	const CPFeature *ftrs = pDQ->GetFoundHandles(num);

	CFtrArray arrftrs;
	for(i=0; i<num; i++)
	{
		CGeometry *pGeo = ftrs[i]->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			continue;
		for(j=0; j<num0; j++)
		{
			if(HandleToFtr(handles[j]) == ftrs[i])
			{
				break;
			}
		}
		if(j<num0)
		{
			CFeature *pTemp = ftrs[i]->Clone();
			pTemp->SetCode("selectedftr");
			arrftrs.Add(pTemp);
		}
		else
		{
			CFeature *pTemp = ftrs[i]->Clone();
			pTemp->SetCode("");
			arrftrs.Add(pTemp);
		}
	}

	//拓扑预处理
	CFtrLayerArray arr;
	int nSum = CopyDataSource(arrftrs, arr);//将pFtr和ftrs放入新数据源
	if(nSum==0) return;

	for(i=0; i<arrftrs.GetSize(); i++)
	{
		delete arrftrs[i];
	}
	
	GProgressStart(9*nSum);
	//线串化, Progress: 1*nSum
	Linearize(arr);	
	//节点咬合, Progress: 1*nSum
	SnapVertexes(arr);
	//消除悬挂点, Progress: 1*nSum
	ProcessSuspend(arr);
	//打断自相交线, Progress: 2*nSum
	CutLines_self(arr);
	//打断相交线, Progress: 2*nSum
	CutLines(arr);
	//剔除公共边, Progress: 1*nSum
	DeleteCommonLine(arr);
	GProgressEnd();
	m_pTempDS->RestoreAllQueryFlags();

	CUndoFtrs undo(m_pEditor,Name());
	CFtrLayer *out_layer=NULL;
	out_layer=m_pDS->GetFtrLayer((LPCTSTR)m_targetLayer);
	if(out_layer==NULL)
	{
		out_layer=new CFtrLayer();
		out_layer->SetName((LPCTSTR)m_targetLayer);
		m_pEditor->AddFtrLayer(out_layer);
	}
	CFeature *pFtrTempl = out_layer->CreateDefaultFeature(((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);
	if(!pFtrTempl) return;
	int nLay = m_pTempDS->GetFtrLayerCount();
	for(i=0; i<nLay; i++)
	{
		CFtrLayer* pLayer = m_pTempDS->GetFtrLayerByIndex(i);
		if(!pLayer) continue;
		
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(pFtr && 0==strcmp("selectedftr", pFtr->GetCode()))
			{
				CFeature *pFtr0 = pFtrTempl->Clone();
				pFtr0->SetID(OUID());
				CArray<PT_3DEX, PT_3DEX> arrPts;
				pFtr->GetGeometry()->GetShape(arrPts);
				pFtr0->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
				if(!m_pEditor->AddObject(pFtr0,out_layer->GetID()))
				{
					delete pFtr0;
					continue;
				}
				undo.AddNewFeature(FtrToHandle(pFtr0));
			}
		}
	}
	delete pFtrTempl;
	undo.Commit();
}

void CSurNoOverPartCommand::GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, FTR_HANDLE id, CArray<double,double> &ret1, CArray<double,double> &ret2, CArray<double,double> &ret_z)
{
	PT_3D pt3d0, pt3d1, tpt;
	
	COPY_3DPT(pt3d0,pt0);
	COPY_3DPT(pt3d1,pt1);
	
	CDlgDataSource *pDS = GETDS(m_pEditor);

	{
		CFeature *pFtr = HandleToFtr(id);
		if (!pFtr) return;

		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
		if (!pLayer) return;
		
		CFtrArray arrObjs;

		arrObjs.Add(pFtr);
		
		for (int j=0; j<arrObjs.GetSize(); j++)
		{
			CFeature *pFtr0 = arrObjs[j];
			if (!pFtr0) continue;

			CGeometry *po = pFtr0->GetGeometry();
			if( !po )continue;
			
			const CShapeLine  *pb = po->GetShape();
			
			if (pb == NULL) continue;
			
			int npt2 = pb->GetPtsCount();
			if( npt2<=1 )continue;
			CArray<PT_3DEX,PT_3DEX> pts;
			pb->GetPts(pts);	
			
			for( int k=0; k<npt2-1; k++)
			{
				//求交
				double vector1x = pt3d1.x-pt3d0.x, vector1y = pt3d1.y-pt3d0.y;
				double vector2x = pts[k+1].x-pts[k].x, vector2y = pts[k+1].y-pts[k].y;
				
				double delta = vector1x*vector2y-vector1y*vector2x;
				if( delta<1e-10 && delta>-1e-10 )continue;
				
				double t1 = ( (pts[k].x-pt3d0.x)*vector2y-(pts[k].y-pt3d0.y)*vector2x )/delta;
				if( t1<0.0 )continue;
				
				double t2 = ( (pts[k].x-pt3d0.x)*vector1y-(pts[k].y-pt3d0.y)*vector1x )/delta;
				if( t2<0.0 || t2>1.0 )continue;
				
				double z = pts[k].z + t2 * (pts[k+1].z-pts[k].z);

				tpt.x = pt3d0.x + t1*vector1x;  tpt.y = pt3d0.y + t1*vector1y;
				if( fabs(tpt.x-pt3d1.x)<GraphAPI::g_lfDisTolerance && fabs(tpt.y-pt3d1.y)<GraphAPI::g_lfDisTolerance )
					continue;
				
				//由小到大排序
				for( int m=0; m<ret1.GetSize(); m++)
				{
					if( t1<ret1[m] )break;
				}
				
				if( m<ret1.GetSize() )
				{
					ret1.InsertAt(m,t1);
					ret_z.InsertAt(m,z);
					ret2.InsertAt(m,k+t2);
				}
				else
				{
					ret1.Add(t1);
					ret_z.Add(z);
					ret2.Add(k+t2);
				}
			}

			if (pFtr0 != pFtr)
			{
				delete pFtr0;
			}
		}
		
	}
}

int CSurNoOverPartCommand::CopyDataSource(CFtrArray& ftrs, CFtrLayerArray& arr)
{
	int i=0;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return 0;

	//新建CDataQueryEx
	m_pDQ = new CDataQueryEx();
	m_pTempDS = new CDlgDataSource(m_pDQ);

	//写入工作区信息
	m_pTempDS->SetScale(pDS->GetScale());
	PT_3D pts[4];
	double zmin,zmax;
	pDS->GetBound(pts,&zmin,&zmax);
	m_pTempDS->SetBound(pts,zmin,zmax);	

	CFtrLayer *pLayer = new CFtrLayer();
	pLayer->SetName("layer");
	m_pTempDS->AddFtrLayer(pLayer);
	arr.Add(pLayer);

	//拷贝地物
	for(i=0; i<ftrs.GetSize(); i++)
	{
		CFeature *pFtr = ftrs[i];
		if(!pFtr) continue;

		CFeature *pFtr1 = pFtr->Clone();
		pFtr1->SetID(OUID());
		if(!m_pTempDS->AddObject(pFtr1,pLayer->GetID()))
		{
			delete pFtr1;
			continue;
		}
	}
	m_pTempDS->RestoreAllQueryFlags();

	return pLayer->GetObjectCount();
}

BOOL CSurNoOverPartCommand::Linearize(CFtrLayerArray& layers0)
{
	int i, j, k;

	double r = m_lfToler;
	
	//1、线串化 + 闭合点精确化处理 + 双线打散、复杂面打散、面转线
	for( i=0; i<layers0.GetSize(); i++)
	{
		int nObj = layers0[i]->GetObjectCount();
		for( j=0; j<nObj; j++)
		{
			CFeature *pFtr = layers0[i]->GetObject(j);
			if( !pFtr )continue;

			GProgressStep();
			
			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
			{
				continue;
			}

			CArray<PT_3DEX,PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			
			//本身是折线，就不线串化
			for(k=0; k<arrPts.GetSize(); k++)
			{
				int pencode = arrPts[k].pencode;
				if( pencode==penArc || pencode==pen3PArc || pencode==penSpline || pencode==penStream)
					break;
			}
			
			if( k<arrPts.GetSize() )
			{								
				CGeometry *pGeo2 = pGeo->Linearize();
				if( pGeo2 )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts2;
					pGeo2->GetShape(arrPts2);

					m_pTempDS->DeleteObject(pFtr);
					pGeo->CreateShape(arrPts2.GetData(),arrPts2.GetSize());
					
					m_pTempDS->RestoreObject(pFtr);
					delete pGeo2;
				}
			}

			pGeo->GetShape(arrPts);

			int npt = arrPts.GetSize();
			if(npt<2) continue;
			double dis = GraphAPI::GGet2DDisOf2P(arrPts[0],arrPts[npt-1]);
			if( dis>0 && dis<r )
			{
				arrPts[npt-1].x = arrPts[0].x;
				arrPts[npt-1].y = arrPts[0].y;

				m_pTempDS->DeleteObject(pFtr);

				pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());

				m_pTempDS->RestoreObject(pFtr);
			}

			if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) || pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) ||
				pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
			{
				CGeoArray geos;

				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) )
				{
					CGeometry *pGeo1=NULL, *pGeo2=NULL;
					((CGeoDCurve*)pGeo)->Separate(pGeo1,pGeo2);
					if( pGeo1 && pGeo2 )
					{
						geos.Add(pGeo1);
						geos.Add(pGeo2);
					}
					else
					{
						if( pGeo1 )delete pGeo1;
						if( pGeo2 )delete pGeo2;
					}
				}
				else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
				{
					CGeometry *pGeo1=NULL, *pGeo2=NULL;
					((CGeoParallel*)pGeo)->Separate(pGeo1,pGeo2);
					if( pGeo1 && pGeo2 )
					{
						geos.Add(pGeo1);
						geos.Add(pGeo2);
					}
					else
					{
						if( pGeo1 )delete pGeo1;
						if( pGeo2 )delete pGeo2;
					}
				}
				else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)) )
				{
					CGeoMultiSurface *pMGeo = (CGeoMultiSurface*)pGeo;
					int nSurface = pMGeo->GetSurfaceNum();
					for( int m=0; m<nSurface; m++)
					{
						CArray<PT_3DEX,PT_3DEX> arrPts2;
						pMGeo->GetSurface(m,arrPts2);
						if( arrPts2.GetSize()<4 )
							continue;
						arrPts2[0].pencode = penLine;

						CGeoCurve *pNewObj = new CGeoCurve();
						pNewObj->CopyFrom(pMGeo);
						if( pNewObj->CreateShape(arrPts2.GetData(),arrPts2.GetSize()) )
						{
							geos.Add(pNewObj);
						}
						else
						{
							delete pNewObj;
						}
					}
				}
				else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts2;
					pGeo->GetShape(arrPts2);
					if( arrPts2.GetSize()<4 )
						continue;
					arrPts2[0].pencode = penLine;

					CGeoCurve *pNewObj = new CGeoCurve();
					pNewObj->CopyFrom(pGeo);
					if( pNewObj->CreateShape(arrPts2.GetData(),arrPts2.GetSize()) )
					{
						geos.Add(pNewObj);
					}
					else
					{
						delete pNewObj;
					}
				}

				for( int m=0; m<geos.GetSize(); m++)
				{
					CFeature *pNewFtr = pFtr->Clone();
					pNewFtr->SetGeometry(geos[m]);

					m_pTempDS->AddObject(pNewFtr,layers0[i]->GetID());

				}

				if( geos.GetSize()>0 )
				{
					m_pTempDS->DeleteObject(pFtr);
				}
			}
		}
	}
	return TRUE;
}

//逻辑与线线悬挂处理中的功能类似
BOOL CSurNoOverPartCommand::ProcessSuspend(CFtrLayerArray& layers0)
{	
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
	
	double r = m_lfToler;
		
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			GProgressStep();
			
			CGeometry *pGeo = pFtr->GetGeometry();
			if(pGeo && pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && !((CGeoCurveBase*)pGeo)->IsClosed() )
			{
				CGeoCurveBase *pGeoCurve = (CGeoCurveBase*)pGeo;
				CArray<PT_3DEX,PT_3DEX> arrPts; 
				pGeoCurve->GetShape(arrPts);
				
				if( arrPts.GetSize()<2 )
					continue;
				
				Envelope e;
				e.CreateFromPtAndRadius(arrPts[0],r);
				int num = m_pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
				
				double min_dis = -1;
				const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);
				
				for( j=0; j<num; j++)
				{
					if( pFtr==ftrs[j] )
						continue;
					
					double dis = CalcSnapDistance(pFtr,ftrs[j],0);
					if( min_dis<0 || min_dis>dis )
					{
						k = j;
						min_dis = dis;
					}				
				}
				
				if( k>=0 )
					ProcessOneFeature(pFtr,ftrs[k],0);
				
				int index = arrPts.GetSize()-1;
				e.CreateFromPtAndRadius(arrPts[index],r);
				num = m_pDQ->FindObjectInRect(e,NULL,FALSE,FALSE), k = -1;
				
				min_dis = -1;
				k = -1;
				ftrs = m_pDQ->GetFoundHandles(num);
				
				for( j=0; j<num; j++)
				{				
					if( pFtr==ftrs[j] )
						continue;
					
					double dis = CalcSnapDistance(pFtr,ftrs[j],index);
					if( min_dis<0 || min_dis>dis )
					{
						k = j;
						min_dis = dis;
					}				
				}
				
				if( k>=0 )
					ProcessOneFeature(pFtr,ftrs[k],index);
			}
		}
	}
	
	return TRUE;
}

BOOL CSurNoOverPartCommand::CutLines_self(CFtrLayerArray& layers0)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}

	//计算所有的交点，并将交点信息放入数组
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			j++;
		}
	}

	ftrsArray.SetSize(j);

	IntersectItemArray arr;

	for( i=0; i<j; i++)
	{
		GProgressStep();
		GetIntersects_self(ftrsArray[i],arr);
	}

	if( arr.GetSize()<=1 )
		return TRUE;

	//按照地物和交点位置的顺序对交点排序
	qsort(arr.GetData(),arr.GetSize(),sizeof(IntersectItem),compare_intersect_item);

	IntersectItem *data = arr.GetData();
	nSum = arr.GetSize();

	//将重复的清除，
	for( i=1; i<arr.GetSize(); i++)
	{
		if( data[i].pFtr==data[i-1].pFtr && fabs(data[i].t-data[i-1].t)<1e-8 && data[i].ptIdx==data[i-1].ptIdx )
			data[i] = IntersectItem();
	}

	//刚好为连续顶点的，就取其头尾，使得它们能成为连续的一段
	int clear_start = 0, clear_stop = 0;
	for( i=1; i<arr.GetSize(); i++)
	{
		if( data[i].pFtr==NULL )
			continue;

		if( data[i].pFtr==data[i-1].pFtr && fabs(data[i].t)<1e-8 && fabs(data[i-1].t)<1e-8 && data[i].ptIdx==(data[i].ptIdx+1) )
			clear_stop++;
		else
		{
			if( clear_stop-clear_start>1 )
			{
				for( k=clear_start+1; k<clear_stop; k++)
				{
					data[k] = IntersectItem();
				}
			}

			clear_stop = clear_start = i;
		}
	}

	//将空交点删除
	for( i=arr.GetSize()-1; i>=0; i--)
	{
		if( arr[i].pFtr==NULL )
			arr.RemoveAt(i);
	}

	//对每个地物依次处理：用该地物的N个交点将地物打断成N+1份
	data = arr.GetData();
	nSum = arr.GetSize();

	int startItem = 0, stopItem = -1;
	for( i=0; i<nSum; i++)
	{
		if( i!=(nSum-1) && data[i].pFtr==data[i+1].pFtr )
			continue;

		stopItem = i+1;

		CFeature *pFtr = data[startItem].pFtr;
		CGeometry *pGeo = pFtr->GetGeometry();
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);

		//处理start~stop之间的交点
		int ptIndex0 = 0, ptIndex1 = 0;
		for( j=startItem; j<stopItem; j++)
		{
			ptIndex1 = data[j].ptIdx;
			ptIndex1 = pShp->GetKeyPos(ptIndex1);

			//在 ptIndex1+1 处插入顶点，取 ptIndex0 ~ ptIndex1 之间的点构成新地物
			
			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);
			pts2.InsertAt(ptIndex1+1,PT_3DEX(data[j].pt,penLine));
			
			if( j>startItem )
			{
				pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			}

			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+2-ptIndex0);

			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pTempDS->AddObject(pNewFtr,m_pTempDS->GetFtrLayerOfObject(pFtr)->GetID());
				}
				else
				{
					delete pNewFtr;
				}
			}

			ptIndex0 = ptIndex1;
		}

		//最后一段
		if( stopItem>startItem )
		{
			ptIndex1 = pts.GetSize()-1;

			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);			
			pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			
			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+1-ptIndex0);
			
			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pTempDS->AddObject(pNewFtr,m_pTempDS->GetFtrLayerOfObject(pFtr)->GetID());					
				}
				else
				{
					delete pNewFtr;
				}
			}
		}

		m_pTempDS->DeleteObject(pFtr);

		startItem = stopItem;
	}

	return TRUE;
}


BOOL CSurNoOverPartCommand::CutLines(CFtrLayerArray& layers0)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}

	//计算所有的交点，并将交点信息放入数组		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}

	ftrsArray.SetSize(j);

	IntersectItemArray arr;

	for( i=0; i<j; i++)
	{
		GProgressStep();
		
		Envelope e = ftrsArray[i]->GetGeometry()->GetEnvelope();
		e.Inflate(r,r,0);
		int num = m_pDQ->FindObjectInRect_SimpleAndSym(e,NULL,FALSE,FALSE);
		
		const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);
		
		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()<=i )continue;

			GetIntersects(ftrsArray[i],ftrs[k],arr);
		}
/*
		for( k=i+1; k<j; k++)
		{
			GetIntersects(ftrsArray[i],ftrsArray[k],arr);
		}
*/
	}

	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}

	if( arr.GetSize()<=1 )
		return TRUE;

	//按照地物和交点位置的顺序对交点排序
	qsort(arr.GetData(),arr.GetSize(),sizeof(IntersectItem),compare_intersect_item);
	
	//将重复的清除，
	for( i=arr.GetSize()-1; i>0; i--)
	{
		if( arr[i].pFtr==arr[i-1].pFtr && fabs(arr[i].t-arr[i-1].t)<1e-8 && arr[i].ptIdx==arr[i-1].ptIdx  )
			arr.RemoveAt(i);
	}
	
	IntersectItem *data = arr.GetData();
	nSum = arr.GetSize();
/*	
	//刚好为连续顶点的，就取其头尾，使得它们能成为连续的一段
	int clear_start = 0, clear_stop = 0;
	for( i=1; i<nSum; i++)
	{		
		if( data[i].pFtr==data[i-1].pFtr && fabs(data[i].t)<1e-8 && fabs(data[i-1].t)<1e-8 && data[i].ptIdx==(data[i-1].ptIdx+1) )
		{
			clear_stop++;

			if( i!=(nSum-1) )
				continue;
		}
		
		if( clear_stop-clear_start>1 )
		{
			for( k=clear_start+1; k<clear_stop; k++)
			{
				data[k] = IntersectItem();
			}
		}
		
		clear_stop = clear_start = i;
	}
*/	
	//将空交点删除
	for( i=nSum-1; i>=0; i--)
	{
		if( arr[i].pFtr==NULL )
			arr.RemoveAt(i);
	}

	//对每个地物依次处理：用该地物的N个交点将地物打断成N+1份
	data = arr.GetData();
	nSum = arr.GetSize();

	int startItem = 0, stopItem = -1;
	for( i=0; i<nSum; i++)
	{
		GProgressStep();

		if( i!=(nSum-1) && data[i].pFtr==data[i+1].pFtr )
			continue;

		stopItem = i+1;

		CFeature *pFtr = data[startItem].pFtr;
		CGeometry *pGeo = pFtr->GetGeometry();
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);

		//处理start~stop之间的交点
		int ptIndex0 = 0, ptIndex1 = 0;
		for( j=startItem; j<stopItem; j++)
		{
			ptIndex1 = data[j].ptIdx;
			ptIndex1 = pShp->GetKeyPos(ptIndex1);

			//在 ptIndex1+1 处插入顶点，取 ptIndex0 ~ ptIndex1 之间的点构成新地物
			
			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);
			pts2.InsertAt(ptIndex1+1,PT_3DEX(data[j].pt,penLine));
			
			if( j>startItem )
			{
				pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			}

			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+2-ptIndex0);

			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pTempDS->AddObject(pNewFtr,m_pTempDS->GetFtrLayerOfObject(pFtr)->GetID());
				}
				else
				{
					delete pNewFtr;
				}
			}

			ptIndex0 = ptIndex1;
		}

		//最后一段
		if( stopItem>startItem )
		{
			ptIndex1 = pts.GetSize()-1;

			CArray<PT_3DEX,PT_3DEX> pts2;
			pts2.Copy(pts);			
			pts2[ptIndex0] = PT_3DEX(data[j-1].pt,penLine);
			
			int num = GraphAPI::GKickoffSamePoints(pts2.GetData()+ptIndex0,ptIndex1+1-ptIndex0);
			
			if( num>=2 )
			{
				CFeature *pNewFtr = pFtr->Clone();
				if( pNewFtr->GetGeometry()->CreateShape(pts2.GetData()+ptIndex0,num) )
				{
					m_pTempDS->AddObject(pNewFtr,m_pTempDS->GetFtrLayerOfObject(pFtr)->GetID());
				}
				else
				{
					delete pNewFtr;
				}
			}
		}

		m_pTempDS->DeleteObject(pFtr);

		startItem = stopItem;
	}

	return TRUE;
}

void CSurNoOverPartCommand::GetIntersects(CFeature* f1, CFeature* f2, IntersectItemArray &arr)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();

	const CShapeLine *pShp1 = pGeo1->GetShape();
	const CShapeLine *pShp2 = pGeo2->GetShape();

	if( !pShp1 || !pShp2 )
		return;

	PT_3D pt1,pt2,line[2],ret;
	double mindis = -1;
	
	int nIdx1=-1,nIdx2=-1;
	int nBaseIdx1 = 0, nBaseIdx2 = 0;
	const CShapeLine::ShapeLineUnit *pList1 = pShp1->HeadUnit();
	while (pList1!=NULL)
	{
		nIdx1++;

		Envelope e = pList1->evlp;
		e.m_xl -= 1e-6; e.m_xh += 1e-6;
		e.m_yl -= 1e-6; e.m_yh += 1e-6;

		nBaseIdx2 = 0;

		const CShapeLine::ShapeLineUnit *pList2 = pShp2->HeadUnit();
		nIdx2 = -1;
		while(pList2!=NULL)
		{
			nIdx2++;
			if(e.bIntersect(&(pList2->evlp)))
			{
				for (int i=0;i<pList1->nuse-1;i++)
				{
					if( pList1->pts[i+1].pencode==penMove )
						continue;

					COPY_3DPT(line[0],pList1->pts[i]);
					COPY_3DPT(line[1],pList1->pts[i+1]);
					
					Envelope e0 = pList2->evlp;
					e0.m_xl -= 1e-4; e0.m_xh += 1e-4;
					e0.m_yl -= 1e-4; e0.m_yh += 1e-4;
					if(!e0.bIntersect(line,line+1))continue;

					for (int j=0;j<pList2->nuse-1;j++)
					{
						if( pList2->pts[j+1].pencode==penMove )
							continue;

						//为了优化，先判断一下
						double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;
						
						if( line[0].x<line[1].x ){ xmin1 = line[0].x; xmax1 = line[1].x; }
						else { xmin1 = line[1].x; xmax1 = line[0].x; }
						if( line[0].y<line[1].y ){ ymin1 = line[0].y; ymax1 = line[1].y; }
						else { ymin1 = line[1].y; ymax1 = line[0].y; }
						
						if( pList2->pts[j].x<pList2->pts[j+1].x ){ xmin2 = pList2->pts[j].x; xmax2 = pList2->pts[j+1].x; }
						else { xmin2 = pList2->pts[j+1].x; xmax2 = pList2->pts[j].x; }
						if( pList2->pts[j].y<pList2->pts[j+1].y ){ ymin2 = pList2->pts[j].y; ymax2 = pList2->pts[j+1].y; }
						else { ymin2 = pList2->pts[j+1].y; ymax2 = pList2->pts[j].y; }
						
						if( xmax1<xmin2-1e-4 || xmax2<xmin1-1e-4 || 
							ymax1<ymin2-1e-4 || ymax2<ymin1-1e-4 )
							continue;

						double xt,yt,xt2,yt2;
						double t0=0,t1=0,t2=0,t3=0;
						bool twoIntersect = false;
						if(GGetLineIntersectLineSeg_withOverlap(line[0].x,line[0].y,line[1].x,line[1].y,pList2->pts[j].x,pList2->pts[j].y,pList2->pts[j+1].x,pList2->pts[j+1].y,
							&xt,&yt,&t0,&t1,
							twoIntersect,
							&xt2,&yt2,&t2,&t3))
						{	
							if(t0<-0.01 || t1<-0.01 || t2<-0.01||t3<-0.01)
							{
								//不应该得到这样的结果
								GGetLineIntersectLineSeg_withOverlap(line[0].x,line[0].y,line[1].x,line[1].y,pList2->pts[j].x,pList2->pts[j].y,pList2->pts[j+1].x,pList2->pts[j+1].y,
									&xt,&yt,&t0,&t1,
									twoIntersect,
									&xt2,&yt2,&t2,&t3);
								int a=1;	
							}
							IntersectItem item0,item1;
							item0.pFtr = f1;
							item0.pt.x = xt;
							item0.pt.y = yt;
							item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
							item0.t = t0;
							item0.ptIdx = nBaseIdx1 + i;

							if( (line[0].x-xt)*(line[0].x-xt)+(line[0].y-yt)*(line[0].y-yt)<1e-8 )
							{
								item0.t = 0;
							}

							if( (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
							{
								item0.ptIdx++;
								item0.t = 0;
							}

							item1.pFtr = f2;
							item1.pt.x = xt;
							item1.pt.y = yt;
							item1.pt.z = pList2->pts[j].z + t1 * (pList2->pts[j+1].z-pList2->pts[j].z);
							item1.t = t1;
							item1.ptIdx = nBaseIdx2 + j;

							if( (pList2->pts[j].x-xt)*(pList2->pts[j].x-xt)+(pList2->pts[j].y-yt)*(pList2->pts[j].y-yt)<1e-8 )
							{
								item1.t = 0;
							}

							if( (pList2->pts[j+1].x-xt)*(pList2->pts[j+1].x-xt)+(pList2->pts[j+1].y-yt)*(pList2->pts[j+1].y-yt)<1e-8 )
							{
								item1.ptIdx++;
								item1.t = 0;
							}

							arr.Add(item0);
							arr.Add(item1);

							if(twoIntersect)
							{
								item0 = IntersectItem();
								item1 = IntersectItem();
								xt = xt2; yt = yt2;
								t0 = t2; t1 = t3;
								
								item0.pFtr = f1;
								item0.pt.x = xt;
								item0.pt.y = yt;
								item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
								item0.t = t0;
								item0.ptIdx = nBaseIdx1 + i;

								if( (line[0].x-xt)*(line[0].x-xt)+(line[0].y-yt)*(line[0].y-yt)<1e-8 )
								{
									item0.t = 0;
								}

								if( (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
								{
									item0.ptIdx++;
									item0.t = 0;
								}

								item1.pFtr = f2;
								item1.pt.x = xt;
								item1.pt.y = yt;
								item1.pt.z = pList2->pts[j].z + t1 * (pList2->pts[j+1].z-pList2->pts[j].z);
								item1.t = t1;
								item1.ptIdx = nBaseIdx2 + j;

								if( (pList2->pts[j].x-xt)*(pList2->pts[j].x-xt)+(pList2->pts[j].y-yt)*(pList2->pts[j].y-yt)<1e-8 )
								{
									item1.t = 0;
								}

								if( (pList2->pts[j+1].x-xt)*(pList2->pts[j+1].x-xt)+(pList2->pts[j+1].y-yt)*(pList2->pts[j+1].y-yt)<1e-8 )
								{
									item1.ptIdx++;
									item1.t = 0;
								}

								arr.Add(item0);
								arr.Add(item1);
							}
						}
					}
				}				
			}
			nBaseIdx2 += pList2->nuse;
			pList2 = pList2->next;
		}		

		nBaseIdx1 += pList1->nuse;
		pList1 = pList1->next;
	}
}



void CSurNoOverPartCommand::GetIntersects_self(CFeature* f1, IntersectItemArray &arr)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	
	const CShapeLine *pShp1 = pGeo1->GetShape();
	
	if( !pShp1 )
		return;

	BOOL bClosed = FALSE;
	if( pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
	{
		if( ((CGeoCurveBase*)pGeo1)->IsClosed() )
		{
			bClosed = TRUE;
		}
	}

	CArray<PT_3DEX,PT_3DEX> pts;
	pShp1->GetPts(pts);

	KickoffSame2DPoints_notPenMove(pts);

	PT_3D pt1,pt2,line[4],ret;

	int npt = pts.GetSize();
	PT_3DEX *buf = pts.GetData();
	for( int i=0; i<npt-1; i++)
	{
		if( buf[i+1].pencode==penMove )
			continue;

		for( int j=i+2; j<npt-1; j++)
		{
			if( buf[j+1].pencode==penMove )
				continue;

			if( bClosed && i==0 && j==(npt-2) )
				continue;

			COPY_3DPT(line[0],buf[i]);
			COPY_3DPT(line[1],buf[i+1]);
			
			COPY_3DPT(line[2],buf[j]);
			COPY_3DPT(line[3],buf[j+1]);

			//为了优化，先判断一下
			double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;
			
			if( line[0].x<line[1].x ){ xmin1 = line[0].x; xmax1 = line[1].x; }
			else { xmin1 = line[1].x; xmax1 = line[0].x; }
			if( line[0].y<line[1].y ){ ymin1 = line[0].y; ymax1 = line[1].y; }
			else { ymin1 = line[1].y; ymax1 = line[0].y; }

			if( line[2].x<line[3].x ){ xmin2 = line[2].x; xmax2 = line[3].x; }
			else { xmin2 = line[3].x; xmax2 = line[2].x; }
			if( line[2].y<line[3].y ){ ymin2 = line[2].y; ymax2 = line[3].y; }
			else { ymin2 = line[3].y; ymax2 = line[2].y; }

			if( xmax1<xmin2-1e-6 || xmax2<xmin1-1e-6 || 
				ymax1<ymin2-1e-6 || ymax2<ymin1-1e-6 )
				continue;

			if( (line[0].x-line[1].x)*(line[0].x-line[1].x) + (line[0].y-line[1].y)*(line[0].y-line[1].y)<1e-8 )
				continue;

			if( (line[2].x-line[3].x)*(line[2].x-line[3].x) + (line[2].y-line[3].y)*(line[2].y-line[3].y)<1e-8 )
				continue;

			double xt,yt;
			double t0,t1;
			if(GraphAPI::GGetLineIntersectLineSeg(line[0].x,line[0].y,line[1].x,line[1].y,line[2].x,line[2].y,line[3].x,line[3].y,&xt,&yt,&t0,&t1))
			{	
				IntersectItem item0,item1;
				item0.pFtr = f1;
				item0.pt.x = xt;
				item0.pt.y = yt;
				item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
				item0.t = t0;
				item0.ptIdx = i;

				if( fabs(1-t0)<1e-6 && (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
				{
					item0.ptIdx++;
					item0.t = 0;
				}
				
				item1.pFtr = f1;
				item1.pt.x = xt;
				item1.pt.y = yt;
				item1.pt.z = line[2].z + t1 * (line[3].z-line[2].z);
				item1.t = t1;
				item1.ptIdx = j;
				
				if( fabs(1-t1)<1e-6 && (line[3].x-xt)*(line[3].x-xt)+(line[3].y-yt)*(line[3].y-yt)<1e-8 )
				{
					item1.ptIdx++;
					item1.t = 0;
				}
				
				arr.Add(item0);
				arr.Add(item1);
			}
			//重叠
			else if( GetOverlapSection_ret_tt(line[0].x,line[0].y,line[1].x,line[1].y,line[2].x,line[2].y,line[3].x,line[3].y,GraphAPI::g_lfDisTolerance,&xt,&yt,&t0,&t1) )
			{
				IntersectItem item0,item1;
				item0.pFtr = f1;
				item0.pt.x = xt;
				item0.pt.y = yt;
				item0.pt.z = line[0].z + t0 * (line[1].z-line[0].z);
				item0.t = t0;
				item0.ptIdx = i;

				if( fabs(1-t0)<1e-6 && (line[1].x-xt)*(line[1].x-xt)+(line[1].y-yt)*(line[1].y-yt)<1e-8 )
				{
					item0.ptIdx++;
					item0.t = 0;
				}
				
				item1.pFtr = f1;
				item1.pt.x = xt;
				item1.pt.y = yt;
				item1.pt.z = line[2].z + t1 * (line[3].z-line[2].z);
				item1.t = t1;
				item1.ptIdx = j;
				
				if( fabs(1-t1)<1e-6 && (line[3].x-xt)*(line[3].x-xt)+(line[3].y-yt)*(line[3].y-yt)<1e-8 )
				{
					item1.ptIdx++;
					item1.t = 0;
				}
				
				arr.Add(item0);
				arr.Add(item1);				
			}
		}
	}	
}


void CSurNoOverPartCommand::GetSnapVertexes(CFeature* f1, CFeature* f2, Envelope evlp, SnapItemArray &arr)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();

	if( !pGeo1->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) || !pGeo2->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		return;
	
	const CShapeLine *pShp1 = pGeo1->GetShape();
	const CShapeLine *pShp2 = pGeo2->GetShape();

	if( !pShp1 || !pShp2 )
		return;

	double toler = m_lfToler;

	Envelope e1 = pGeo1->GetEnvelope();
	e1.Inflate(toler,toler,0);

	BOOL bUseEvlp = (!evlp.IsEmpty());
	double toler2 = toler * toler;
	PT_3DEX pt1,pt2;

	if( !e1.bIntersect(&pGeo2->GetEnvelope()) )
		return;

	if( f1==f2 )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeo1->GetShape(arrPts);

		int npt = arrPts.GetSize();
		for( int i=0; i<npt; i++)
		{
			pt1 = arrPts[i];
			if( bUseEvlp && !evlp.bPtIn(&pt1) )
				continue;

			for( int j=i+2; j<npt; j++)
			{
				pt2 = arrPts[j];
				if( bUseEvlp && !evlp.bPtIn(&pt2) )
					continue;

				double dis = (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y);
				
				if( dis<toler2 && dis>1e-8 )
				{
					SnapItem item0;
					item0.pFtr1 = f1;
					item0.pFtr2 = f2;
					item0.pt1 = pt1;
					item0.pt2 = pt2;

					CArray<int,int> arrIndex;
					pShp1->GetKeyPosOfBaseLines(arrIndex);

					item0.ptIdx1 = arrIndex[i];
					item0.ptIdx2 = arrIndex[j];
					/*
					arr.Add(item0);

					item0.pFtr1 = f1;
					item0.pFtr2 = f2;
					item0.pt1 = pt2;
					item0.pt2 = pt1;
					item0.ptIdx1 = arrIndex[j];
					item0.ptIdx2 = arrIndex[i];
					*/
					arr.Add(item0);
				}
			}
		}
		return;
	}
		
	
	PT_3D line[2],ret;
	double mindis = -1;
	
	int nIdx1=-1,nIdx2=-1;
	int nBaseIdx1 = 0, nBaseIdx2 = 0;
	const CShapeLine::ShapeLineUnit *pList1 = pShp1->HeadUnit();
	while (pList1!=NULL)
	{
		nIdx1++;
		const CShapeLine::ShapeLineUnit *pList2 = pShp2->HeadUnit();
		nIdx2 = -1;
		nBaseIdx2 = 0;

		Envelope e = pList1->evlp;
		e.m_xl -= toler; e.m_xh += toler;
		e.m_yl -= toler; e.m_yh += toler;

		if( bUseEvlp && !e.bIntersect(&evlp) )
		{
			nBaseIdx1 += pList1->nuse;
			pList1 = pList1->next;	
			continue;
		}

		while(pList2!=NULL)
		{
			nIdx1++;

			if( e.bIntersect(&(pList2->evlp)) )
			{
				for (int i=0;i<pList1->nuse;i++)
				{
					pt1 = pList1->pts[i];
					if( pt1.pencode==penNone )
						continue;
					
					for (int j=0;j<pList2->nuse;j++)
					{						
						pt2 = pList2->pts[j];

						if( pt2.pencode==penNone )
							continue;

						double dis = (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y);

						if( dis<toler2 && dis>1e-8 )
						{	
							if( !bUseEvlp || (evlp.bPtIn(&pt1) && evlp.bPtIn(&pt2)) )
							{
								SnapItem item0;
								item0.pFtr1 = f1;
								item0.pFtr2 = f2;
								item0.pt1 = pt1;
								item0.pt2 = pt2;
								item0.ptIdx1 = nBaseIdx1 + i;
								item0.ptIdx2 = nBaseIdx2 + j;
								
								arr.Add(item0);
							}
						}
					}
				}				
			}

			nBaseIdx2 += pList2->nuse;
			pList2 = pList2->next;
		}	
		
		nBaseIdx1 += pList1->nuse;
		pList1 = pList1->next;		
	}	
}


BOOL CSurNoOverPartCommand::SnapVertexes(CFtrLayerArray& layers0)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}

	ftrsArray.SetSize(j);
	
	//计算所有的匹配点（咬合点），并将匹配点信息放入数组
	SnapItemArray arr;

	for( i=0; i<j; i++)
	{
		GProgressStep();

		Envelope e = ftrsArray[i]->GetGeometry()->GetEnvelope();
		e.Inflate(r,r,0);

		int m, num = m_pDQ->FindObjectInRect_SimpleAndSym(e,NULL,FALSE,FALSE);
		
		const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);
		
		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()<=i )continue;

			SnapItemArray arr1; 
			
			GetSnapVertexes(ftrsArray[i],ftrs[k],Envelope(),arr1);

/*			//去掉串联的部分（A->B,B->C），避免一个地物的某个节点被多次修改
			for( m=arr.GetSize()-1; m>=0; m--)
			{
				SnapItem item0 = arr[m];
				for( int n=arr1.GetSize()-1; n>=0; n--)
				{
					SnapItem item1 = arr1[n];
					if( (item0.pFtr2==item1.pFtr1 && item0.ptIdx2==item1.ptIdx1) ||
						(item0.pFtr2==item1.pFtr2 && item0.ptIdx2==item1.ptIdx2) )
					{
						arr1.RemoveAt(n);
					}
				}				
			}

			//去掉两个节点同时连接某个节点的情况（A->C,B->C）
			for( m=arr1.GetSize()-1; m>=0; m--)
			{
				SnapItem item0 = arr1[m];
				for( int n=m-1; n>=0; n--)
				{
					SnapItem item1 = arr1[n];
					if( item0.pFtr2==item1.pFtr2 && item0.ptIdx2==item1.ptIdx2 )
					{
						arr1.RemoveAt(m);
						break;
					}
				}				
			}
*/
			arr.Append(arr1);
		}
	}

	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}

	if( arr.GetSize()<=0 )
		return TRUE;

	//按照地物和咬合点位置的顺序对交点排序
	qsort(arr.GetData(),arr.GetSize(),sizeof(SnapItem),compare_snap_item);

	//对每个地物依次处理：将该地物的N个重叠点调整位置
	SnapItem *data = arr.GetData();
	nSum = arr.GetSize();

	int startItem = 0, stopItem = -1;
	for( i=0; i<nSum; i++)
	{
		if( i!=(nSum-1) && data[i].pFtr1==data[i+1].pFtr1 && data[i].ptIdx1==data[i+1].ptIdx1 )
			continue;

		stopItem = i+1;

		CGeometry *pGeo = data[startItem].pFtr1->GetGeometry();
		const CShapeLine *pShp = pGeo->GetShape();

		CArray<PT_3DEX,PT_3DEX> pts;
		pShp->GetPts(pts);

		//处理start~stop之间的重叠点
		//计算平均值，并计算各个重叠点所对应的地物的顶点序号，便于下一步修改
		double ax = data[startItem].pt1.x, ay = data[startItem].pt1.y;
		CArray<int,int> arrIndex;

		for( j=startItem; j<stopItem; j++)
		{
			CGeometry *pGeo2 = data[j].pFtr2->GetGeometry();
			const CShapeLine *pShp2 = pGeo2->GetShape();

			arrIndex.Add(pShp2->GetKeyPos(data[j].ptIdx2));

			ax += data[j].pt2.x;
			ay += data[j].pt2.y;
		}

		ax /= (stopItem-startItem+1);
		ay /= (stopItem-startItem+1);

		arrIndex.Add(pShp->GetKeyPos(data[startItem].ptIdx1));

		//修改地物
		for( j=startItem; j<stopItem+1; j++)
		{
			int ptIndex = arrIndex[j-startItem];
			CFeature* pFtr = ((j==stopItem)?data[startItem].pFtr1:data[j].pFtr2);
		
			m_pTempDS->DeleteObject(pFtr);

			CGeometry *pGeo2 = pFtr->GetGeometry();
			PT_3DEX expt = pGeo2->GetDataPoint(ptIndex);
			expt.x = ax;
			expt.y = ay;
			pGeo2->SetDataPoint(ptIndex,expt);

			m_pTempDS->RestoreObject(pFtr);
		}

		startItem = stopItem;
	}

	return TRUE;
}


BOOL CSurNoOverPartCommand::IsOverlapped(CPFeature f1, CPFeature f2)
{
	CGeometry *pGeo1 = f1->GetGeometry();
	CGeometry *pGeo2 = f2->GetGeometry();

	if( !pGeo1->GetEnvelope().bIntersect(&pGeo2->GetEnvelope()) )
		return FALSE;
		
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	pGeo1->GetShape(arrPts1);
	pGeo2->GetShape(arrPts2);

	double toler = m_lfToler;
	toler = toler*toler;

	toler = 1e-8;

	if( arrPts1.GetSize()!=arrPts2.GetSize() )
		return FALSE;

	int npt = arrPts1.GetSize();

	for( int i=0; i<npt; i++)
	{
		PT_3DEX pt1 = arrPts1[i];
		PT_3DEX pt2 = arrPts2[i];
		if( (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y)>=toler )
		{
			break;
		}
	}

	if( i<npt )
	{
		for( i=0; i<npt; i++)
		{
			PT_3DEX pt1 = arrPts1[i];
			PT_3DEX pt2 = arrPts2[npt-1-i];
			if( (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y)>=toler )
			{
				break;
			}
		}

		if( i<npt )
			return FALSE;
	}

	return TRUE;
}


BOOL CSurNoOverPartCommand::DeleteCommonLine(CFtrLayerArray& layers0)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;

			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}

	ftrsArray.SetSize(j);

	//比较地物是否完全重叠，是的话，就删除其中一个
	SnapItemArray arr;

	for( i=j-1; i>=0; i--)
	{
		GProgressStep();

		if(ftrsArray[i]->IsDeleted()) continue;

		CArray<PT_3DEX,PT_3DEX> arrPts;
		ftrsArray[i]->GetGeometry()->GetShape(arrPts);
		if(arrPts.GetSize()<1) continue;

		Envelope e;
		e.CreateFromPtAndRadius(arrPts[0],GraphAPI::g_lfDisTolerance);
		int num = m_pDQ->FindObjectInRect_SimpleAndSym(e,NULL,FALSE,FALSE);
		
		const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);

		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()>=i )continue;

			if( IsOverlapped(ftrsArray[i],ftrs[k]) )
			{
				m_pTempDS->DeleteObject(ftrsArray[i]);
				m_pTempDS->DeleteObject(ftrs[k]);//同时删除公共部分
				break;
			}
		}
	}

	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}

	return TRUE;
}


double CSurNoOverPartCommand::CalcSnapDistance(CPFeature f1, CPFeature f2, int nPtIndex)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return -1;
	
	CGeometry *pObj = pFtr->GetGeometry();

	CArray<PT_3DEX,PT_3DEX> arrPts0;
	pObj->GetShape(arrPts0);
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = arrPts0[1];
		expt1 = arrPts0[0];
	}
	else
	{
		nIndex = arrPts0.GetSize()-1;
		expt0 = arrPts0[nIndex-1];
		expt1 = arrPts0[nIndex];
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;

	CGeometry *pObj2 = f2->GetGeometry();
	PT_KEYCTRL nearestPt = pObj2->FindNearestKeyCtrlPt(expt1,r,NULL,1);

	PT_3DEX newPt;
	if( nearestPt.IsValid() )
	{
		newPt = pObj2->GetDataPoint(nearestPt.index);
	}
	else
	{
		Envelope e;
		e.CreateFromPtAndRadius(expt1,r);
		if( !pObj2->FindNearestBasePt(expt1,e,NULL,&newPt,NULL) )
			return -1;
	}
	
	return GraphAPI::GGet2DDisOf2P(expt1,newPt);
}

void CSurNoOverPartCommand::ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex)
{
	//找到点中的地物	
	double r = m_lfToler;
	
	CPFeature pFtr = f1;		
	if( !pFtr )return;
	
	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj )return;
	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )return;
	if( ((CGeoCurveBase*)pObj)->IsClosed() )return;

	CArray<PT_3DEX,PT_3DEX> arrPts0;
	pObj->GetShape(arrPts0);
	
	int nIndex = -1;
	PT_3DEX expt0, expt1;
	if( nPtIndex==0 )
	{
		nIndex = 0;
		expt0 = arrPts0[1];
		expt1 = arrPts0[0];
	}
	else
	{
		nIndex = arrPts0.GetSize()-1;
		expt0 = arrPts0[nIndex-1];
		expt1 = arrPts0[nIndex];
	}
	
	//计算端点延长后，相交点的位置
	CArray<double,double> arrRets, arrRets2, arrRets_z;

	CGeometry *pObj2 = f2->GetGeometry();
	PT_KEYCTRL nearestPt = pObj2->FindNearestKeyCtrlPt(expt1,r,NULL,1);

	BOOL bSnapVertex = FALSE;
	PT_3DEX newPt = expt1;
	int nIndex2 = -1;
	if( nearestPt.IsValid() )
	{
		newPt = pObj2->GetDataPoint(nearestPt.index);
		nIndex2 = nearestPt.index;
		bSnapVertex = TRUE;
	}
	else
	{
		Envelope e;
		e.CreateFromPtAndRadius(expt1,r);
		if( !pObj2->FindNearestBasePt(expt1,e,NULL,&newPt,NULL) )
			return;

		const CShapeLine *pShp = pObj2->GetShape();
		if( !pShp )
			return;

		nIndex2 = pShp->FindNearestKeyPt(newPt);
	}
	
	m_pTempDS->DeleteObject(pFtr);
	
	PT_3DEX pt1 = newPt;
	if( !m_bSnap3D )
	{
		pt1.z = expt1.z;
	}
	pFtr->GetGeometry()->SetDataPoint(nPtIndex,pt1);		
	m_pTempDS->RestoreObject(pFtr);

	if( m_bAddPt && !bSnapVertex )
	{
		m_pTempDS->DeleteObject(f2);

		CArray<PT_3DEX,PT_3DEX> arrPts;
		f2->GetGeometry()->GetShape(arrPts);
		arrPts.InsertAt(nIndex2+1,newPt);

		f2->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		m_pTempDS->RestoreObject(f2);
	}
}


//////////////////////////////////////////////////////////////////////
// CRiverAddDirectCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CRiverAddDirectCommand,CEditCommand)

CRiverAddDirectCommand::CRiverAddDirectCommand()
{
	m_nOperation = 0;
	m_deta = 15;
	m_offset = 0.1;
	m_nStep = -1;
	strcat(m_strRegPath,"\\RiverAddDirect");
	m_pDS = NULL;
	m_pFtrTempl = NULL;
}

CRiverAddDirectCommand::~CRiverAddDirectCommand()
{
	if(m_pFtrTempl)
		delete m_pFtrTempl;
}

CString CRiverAddDirectCommand::Name()
{
	return StrFromResID(IDS_RIVER_ADD_DIRECT);
}

void CRiverAddDirectCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_nStep = 0;
}

void CRiverAddDirectCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CRiverAddDirectCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CRiverAddDirectCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (long)(m_nOperation);
	tab.AddValue("Operation",&CVariantEx(var));
	
	var = (LPCTSTR)(m_RiverLayer);
	tab.AddValue("RiverLayer",&CVariantEx(var));

	var = (LPCTSTR)(m_dirpointLayer);
	tab.AddValue("DirPointLayer",&CVariantEx(var));

	var = m_deta;
	tab.AddValue("deta",&CVariantEx(var));

	var = m_offset;
	tab.AddValue("offset",&CVariantEx(var));
}

void CRiverAddDirectCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("RiverAddDirectCommand",Name());

	param->AddLayerNameParam("RiverLayer",(LPCTSTR)m_RiverLayer,StrFromResID(IDS_LINK_RIVERLAYERS));
	
	param->BeginOptionParam("Operation",StrFromResID(IDS_CMDPLANE_MM_OPERATIONMODE));
	param->AddOption(StrFromResID(IDS_WHOLEMAP),  0,' ', m_nOperation==0);
	param->AddOption(StrFromResID(IDS_SELECTOBJ), 1,' ', m_nOperation==1);
	param->EndOptionParam();

	if(m_nOperation==0)
	{
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->CloseSelector();
		GOutPut(StrFromResID(IDS_CMDTIP_PLACELEFTKEY));
	}
	else
	{
		m_pEditor->DeselectAll();
		m_pEditor->OnSelectChanged();
		m_pEditor->OpenSelector();
	}
	param->AddLayerNameParam("DirPointLayer",(LPCTSTR)m_dirpointLayer,StrFromResID(IDS_CMDPLANE_DIRPOINTLAYER));

	param->AddParam("deta", m_deta, StrFromResID(IDS_DETA));
	param->AddParam("offset", m_offset, StrFromResID(IDS_OFFSET));
}

void CRiverAddDirectCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Operation",var) )
	{					
		m_nOperation = (long)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"RiverLayer",var) )
	{					
		m_RiverLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_RiverLayer.TrimLeft();
		m_RiverLayer.TrimRight();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"DirPointLayer",var) )
	{					
		m_dirpointLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_dirpointLayer.TrimLeft();
		m_dirpointLayer.TrimRight();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"DirPointLayer",var) )
	{					
		m_dirpointLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_dirpointLayer.TrimLeft();
		m_dirpointLayer.TrimRight();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"deta",var) )
	{					
		m_deta = (float)(_variant_t)*var;
		if (m_deta < 1)
		{
			m_deta = 15;
			AfxMessageBox(IDS_PARAM_ERROR);
			return;
		}
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,"offset",var) )
	{					
		m_offset = (float)(_variant_t)*var;
		SetSettingsModifyFlag();		
	}

	CEditCommand::SetParams(tab,bInit);
}

BOOL CRiverAddDirectCommand::prepare()
{
	if( !m_pEditor ) return FALSE;
	m_pDS = m_pEditor->GetDataSource();
	if(!m_pDS) return FALSE;
	if(m_dirpointLayer.IsEmpty()) return FALSE;
	CFtrLayer *out_layer=NULL;
	out_layer=m_pDS->GetFtrLayer((LPCTSTR)m_dirpointLayer);
	if(!out_layer)
	{
		out_layer=new CFtrLayer();
		out_layer->SetName((LPCTSTR)m_dirpointLayer);
		m_pEditor->AddFtrLayer(out_layer);
	}
	if(!out_layer) return FALSE;
	m_layid = out_layer->GetID();
	m_pFtrTempl = out_layer->CreateDefaultFeature(((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEODIRPOINT);
	if(!m_pFtrTempl) return FALSE;
	
	//厘米转图层长度
	m_d = m_deta*m_pDS->GetScale()/100;
	m_off = m_offset*m_pDS->GetScale()/100;
	return TRUE;
}

void CRiverAddDirectCommand::PtClick(PT_3D &pt, int flag)
{
	if(m_nOperation==0)
	{
		if(!prepare()) return;
		if(m_RiverLayer.IsEmpty())  return;
		CFtrLayerArray layers;
		m_pDS->GetFtrLayersByNameOrCode(m_RiverLayer,layers);
		CUndoFtrs undo(m_pEditor,Name());
		for(int i=0; i<layers.GetSize(); i++)
		{
			int nObj = layers[i]->GetObjectCount();
			for(int j=0; j<nObj; j++)
			{
				CFeature *pFtr = layers[i]->GetObject(j);
				if(!pFtr) continue;

				AddDirPoint(pFtr, undo);
			}
		}
		undo.Commit();
	}
	else if(m_nOperation==1)
	{
		if(m_nStep==0)
		{
			if( !CanGetSelObjs(flag) )
				return;
			EditStepOne();
		}
		if( m_nStep==1 )
		{
			if(!prepare()) return;
			CSelection* pSel = m_pEditor->GetSelection();
			
			CUndoFtrs undo(m_pEditor,Name());
			int num;
			const FTR_HANDLE* handles = pSel->GetSelectedObjs(num);
			for(int i=0; i<num; i++)
			{
				AddDirPoint(HandleToFtr(handles[i]), undo);
			}

			m_pEditor->DeselectAll();
			m_pEditor->OnSelectChanged();
			m_pEditor->UpdateDrag(ud_ClearDrag);
			m_pEditor->RefreshView();
			m_nStep = 2;
			undo.Commit();
		}
	}
	
	Finish();
	CEditCommand::PtClick(pt,flag);
}

void CRiverAddDirectCommand::AddDirPoint(CFeature *pFtr, CUndoFtrs& undo)
{
	const CShapeLine *pShape = pFtr->GetGeometry()->GetShape();
	if(!pShape) return;
	double len1, len2;
	CArray<PT_3DEX, PT_3DEX> arrPts;
	pShape->GetPts(arrPts);

	int i=1;
	double m_curPosion = m_d/2;
LOOP:
	for(; i<arrPts.GetSize(); i++)
	{
		PT_3D test_pt;
		COPY_3DPT(test_pt, arrPts[i]);
		len2 = pShape->GetLength(&test_pt);

		if(len2>m_curPosion)
		{
			COPY_3DPT(test_pt, arrPts[i-1]);
			len1 = pShape->GetLength(&test_pt);
			break;
		}
	}

	if(i>=arrPts.GetSize()) return;

	PT_3DEX pt1;//河流线上的基点
	double part = (m_curPosion-len1)/(len2-len1);
	double dx = (arrPts[i].x-arrPts[i-1].x)*part;
	double dy = (arrPts[i].y-arrPts[i-1].y)*part;
	double dz = (arrPts[i].z-arrPts[i-1].z)*part;
	pt1.x = arrPts[i-1].x+dx;
	pt1.y = arrPts[i-1].y+dy;
	pt1.z = arrPts[i-1].z+dz;

	PT_3DEX pt2;//得到有向点的第一个点
	part = (m_off)/(len2-len1);
	dx = (arrPts[i].x-arrPts[i-1].x)*part;
	dy = (arrPts[i].y-arrPts[i-1].y)*part;
	pt2.x = pt1.x+dy;
	pt2.y = pt1.y-dx;
	pt2.z = pt1.z;

	PT_3DEX pt3;//方向
	pt3.x = pt2.x+dx;
	pt3.y = pt2.y+dy;
	pt3.z = pt1.z;

	CFeature * pNewFtr = m_pFtrTempl->Clone();
	if (pNewFtr)
	{
		CArray<PT_3DEX,PT_3DEX> pts;
		pts.Add(pt2);
		pts.Add(pt3);
		if (pNewFtr->CreateGeometry(CLS_GEODIRPOINT))
		{
			pNewFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
			pNewFtr->SetID(OUID());
			pNewFtr->SetPurpose(FTR_EDB);
			if(!m_pEditor->AddObject(pNewFtr,m_layid))
			{
				delete pNewFtr;
			}
			else
			{
				undo.AddNewFeature(FtrToHandle(pNewFtr));
			}
		}
	}

	m_curPosion += m_d;
	goto LOOP;
}


//////////////////////////////////////////////////////////////////////
// CCheckSurfaceGapCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CCheckSurfaceGapCommand Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCheckSurfaceGapCommand,CEditCommand)

CCheckSurfaceGapCommand::CCheckSurfaceGapCommand()
{
	m_nStep = 0;
	m_nOperation = 0;
	m_flags = NULL;
	m_lfArea = 50;
	m_bAddCheckResult = TRUE;
	strcat(m_strRegPath,"\\CheckSurfaceGap");
}

CCheckSurfaceGapCommand::~CCheckSurfaceGapCommand()
{
}

CString CCheckSurfaceGapCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_SURFACEGAP);
}

void CCheckSurfaceGapCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();

	m_nStep = 0;
}

void CCheckSurfaceGapCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CCommand::Finish();
}

void CCheckSurfaceGapCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CCheckSurfaceGapCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (long)(m_nOperation);
	tab.AddValue("Operation",&CVariantEx(var));

	var = m_lfArea;
	tab.AddValue("lfArea",&CVariantEx(var));
}

void CCheckSurfaceGapCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("CheckSurfaceGapCommand",Name());
	
	param->BeginOptionParam("Operation", StrFromResID(IDS_CMDPLANE_MM_OPERATIONMODE));
	param->AddOption(StrFromResID(IDS_SURFACEGAP_OP1),0,' ',m_nOperation==0);
	param->AddOption(StrFromResID(IDS_SURFACEGAP_OP2),1,' ',m_nOperation==1);
	param->AddOption(StrFromResID(IDS_SURFACEGAP_OP3),2,' ',m_nOperation==2);
	param->EndOptionParam();

	param->AddParam("lfArea",m_lfArea,StrFromResID(IDS_HOUSE_AREALIMIT));
}

void CCheckSurfaceGapCommand::SetParams(CValueTable& tab,BOOL bInit)
{
 	const CVariantEx *var;
	if( tab.GetValue(0,"Operation",var) )
	{
		m_nOperation = (long)(_variant_t)*var;	
		SetSettingsModifyFlag();
		if(!bInit)
		{
			Abort();
		}
	}
	if (tab.GetValue(0, "lfArea", var))
	{
		m_lfArea = (double)(_variant_t)*var;
	}

	SetSettingsModifyFlag();
	CEditCommand::SetParams(tab,bInit);
}

extern void KickoffSameGeoPts(CArray<MYPT_3D,MYPT_3D>& arr);

//按边线裁剪
static void TrimCurve(CArray<PT_3DEX,PT_3DEX>& pts, CArray<PT_3DEX,PT_3DEX>& bound, CArray<MYPT_3D,MYPT_3D>& arrAll)
{
	CArray<PtIntersect, PtIntersect> arr;
	GetCurveIntersectCurve(pts.GetData(), pts.GetSize(), bound.GetData(), bound.GetSize(), arr);
	static int segid=1;//线段id
	int i=0, j=0;
	//插入交点
	CArray<PT_3DEX,PT_3DEX> pts1;
	for(i=0; i<pts.GetSize(); i++)
	{
		if(j>=arr.GetSize() || i<arr[j].lfRatio)
		{
			pts1.Add(pts[i]);
		}
		else
		{
			PT_3DEX pt(arr[j].pt, penLine);
			pts1.Add(pt);
			pts1.Add(pts[i]);
			j++;
		}
	}

	//在边界内的线放入到arrall里
	CArray<MYPT_3D,MYPT_3D> arrCur;
	PT_3DEX expt;
	MYPT_3D geopt;
	int nPt = pts1.GetSize();
	for(i=0; i<nPt; i++)
	{
		expt = pts1[i];
		int val = GraphAPI::GIsPtInRegion(expt, bound.GetData(), bound.GetSize());
		if(val<1)//外部
		{
			continue;
		}
		else if(val==2)//内部
		{
			geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;
			geopt.type = 1;
			geopt.id = segid;
			arrCur.Add(geopt);
		}
		else if(val==1)//边界
		{
			if(arrCur.GetSize()==0)//进入边界
			{
				geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;
				geopt.type = 1;
				geopt.id = segid;
				arrCur.Add(geopt);
			}
			else//离开边界
			{
				geopt.x = expt.x; geopt.y = expt.y; geopt.z = expt.z;
				geopt.type = 1;
				geopt.id = segid;
				arrCur.Add(geopt);
				arrAll.Append(arrCur);
				arrCur.RemoveAll();
				segid++;
			}
		}
	}
	if(arrCur.GetSize()>1)
	{
		arrAll.Append(arrCur);
		segid++;
	}
}

void CCheckSurfaceGapCommand::PtClick(PT_3D &pt, int flag)
{
	if( !m_pEditor )return;
	if( m_nStep==0 )
	{
		m_pt0 = pt;
		m_nStep = 1;
		if(m_nOperation==2)
		{
			GotoState(PROCSTATE_PROCESSING);
			return;
		}
	}
	if (m_nStep==1)
	{
		m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		m_pt1 = pt;

		CDataSourceEx *pDS = m_pEditor->GetDataSource();
		if(!pDS) return;

		PT_3D bound[4];
		Envelope e;
		if(m_nOperation==1)
		{
			double zmax,zmin;
			pDS->GetBound(bound, &zmin, &zmax);
			e.CreateFromPts(bound, 4);
		}
		else if(m_nOperation==2)
		{
			bound[0].x = m_pt0.x; bound[0].y = m_pt0.y; bound[0].z = m_pt0.z;
			bound[1].x = m_pt1.x; bound[1].y = m_pt0.y; bound[1].z = m_pt0.z;
			bound[2].x = m_pt1.x; bound[2].y = m_pt1.y; bound[2].z = m_pt1.z;
			bound[3].x = m_pt0.x; bound[3].y = m_pt1.y; bound[3].z = m_pt0.z;
			e.CreateFromPts(bound, 4);
		}

		vector<item> container;
		int nlayer = pDS->GetFtrLayerCount(), i, j;
		for(i=0; i<nlayer; i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			if(!pLayer) continue;
			
			int nObj = pLayer->GetObjectCount();
			for(int j=0; j<nObj; j++)
			{
				CFeature* pFtr = pLayer->GetObject(j);
				if(!pFtr || !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
					continue;
				CGeometry *pGeo = pFtr->GetGeometry();
				if(pGeo)
				{
					Envelope e1 = pGeo->GetEnvelope();
					if(m_nOperation==0 || e1.bIntersect(&e))
					{
						item data;
						data.pFtr = pFtr;
						data.e = e1;
						container.push_back(data);
					}
				}
			}
		}

		//创建三角网
		CGeometry *pGeo;
		CArray<MYPT_3D,MYPT_3D> arrAll, arrCur;
		MYPT_3D geopt;

		//添加边框
		if(m_nOperation==1 || m_nOperation==2)
		{
			for(i=0; i<4; i++)
			{
				geopt.x = bound[i].x; geopt.y = bound[i].y; geopt.z = bound[i].z;
				geopt.type = 1; geopt.id = 0;
				arrCur.Add(geopt);
			}

			geopt.x = bound[0].x; geopt.y = bound[0].y; geopt.z = bound[0].z;
			geopt.type = 1; geopt.id = 0;
			arrCur.Add(geopt);

			arrAll.Append(arrCur);
		}

		int nobj = container.size();
		CArray<PT_3DEX,PT_3DEX> bound1;
		if(m_nOperation==1 || m_nOperation==2)
		{
			for(i=0; i<4; i++)
			{
				PT_3DEX temp(bound[i], penLine);
				bound1.Add(temp);
			}
			bound1.Add(bound1[0]);
		}
		int k=0;
		vector<item>::const_iterator it;
		for (it= container.begin();it<container.end();it++)
		{					
			CFeature *pFtr = (*it).pFtr;
			pGeo = pFtr->GetGeometry();
			
			CArray<PT_3DEX,PT_3DEX> pts, pts1;
			pGeo->GetShape()->GetPts(pts);
			int nPt = pts.GetSize();
			if(m_nOperation==0)
			{
				arrCur.RemoveAll();
				for( int n=0; n<nPt; n++)
				{
					PT_3DEX expt = pts[n];
					if (expt.pencode == penMove && n != 0)
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
				if( arrCur.GetSize()>0 )
				{
					arrAll.Append(arrCur);
					k++;
				}
			}
			else
			{
				for(j=0; j<nPt;)
				{
					PT_3DEX temp = pts[j];
					pts1.Add(temp);
					j++;
					if(j>=nPt || pts[j].pencode==penMove)
					{
						TrimCurve(pts1, bound1, arrAll);//根据bound裁剪， 并把线段存入arrAll
						pts1.RemoveAll();
					}
				}
			}
		}
		
		if( arrAll.GetSize()>0 )
		{
			m_tin.Clear();
			MyTriangle::createTIN(arrAll.GetData(),arrAll.GetSize(),&m_tin.tins,&m_tin.nTIN,&m_tin.pts,&m_tin.npt);
		}
//#define TESTAAA
#ifdef TESTAAA
CFtrLayer *pLayer = pDS->GetFtrLayer("30");
CFeature *ptempFtr = pLayer->CreateDefaultFeature(pDS->GetScale(), CLS_GEOSURFACE);
#endif
		//判断三角形的重心是否在某个面内部
		MYPT_3D tri[3];
		GProgressStart(m_tin.nTIN*2);
		m_flags = new int[m_tin.nTIN];
		memset(m_flags, -1, m_tin.nTIN*sizeof(int));
		for(i=0; i<m_tin.nTIN; i++)
		{
			GProgressStep();
			if( !m_tin.GetTriangle(i,tri) )
			{
				continue;
			}

			//三角形有可能三点共线，需要排出掉
			double t = GraphAPI::GGetMultiply(tri[0],tri[1],tri[2]);
			if( fabs(t) < 1e-4) continue;

			PT_3D ptg;//重心
			ptg.x = (tri[0].x + tri[1].x + tri[2].x)/3;
			ptg.y = (tri[0].y + tri[1].y + tri[2].y)/3;
			ptg.z = 0.0;
#ifdef TESTAAA
			PT_3DEX pts[4];
			pts[0].x=tri[0].x; pts[0].y=tri[0].y; pts[0].z=tri[0].z; pts[0].pencode=penLine;
			pts[1].x=tri[1].x; pts[1].y=tri[1].y; pts[1].z=tri[1].z; pts[1].pencode=penLine;
			pts[2].x=tri[2].x; pts[2].y=tri[2].y; pts[2].z=tri[2].z; pts[2].pencode=penLine;
			pts[3] = pts[0];

			CGeoSurface* pSurface = new CGeoSurface();
			pSurface->CreateShape(pts, 4);
#endif

			BOOL bPtIn = FALSE;
			for (it= container.begin();it<container.end();it++)
			{
				Envelope e = (*it).e;
				if(e.bPtIn(&ptg) )
				{
					CFeature* pFtr = (*it).pFtr;
					if(pFtr->GetGeometry()->bPtIn(&ptg))
					{
						bPtIn = TRUE;
						break;
					}
				}
			}

			if(!bPtIn)
			{
				m_flags[i] = i;
#ifdef TESTAAA
				CFeature *pNew = ptempFtr->Clone();
				pNew->SetGeometry(pSurface);
				pNew->SetPurpose(FTR_MARK);
				m_pEditor->AddObject(pNew, pLayer->GetID());
#endif
			}
		}

		//共边的三角形合成一组， 每一组取出一个点添加到检查列表
		MergeTins();

		GProgressEnd();
#ifdef TESTAAA
		delete ptempFtr;
#endif
	}

	if(m_flags)
	{
		delete[] m_flags;m_flags=NULL;
	}
	m_tin.Clear();
	Finish();
	CEditCommand::PtClick(pt,flag);
}

void CCheckSurfaceGapCommand::PtMove(PT_3D &pt)
{
	if(m_nStep==1 && m_nOperation==2)
	{
		GrBuffer buf;
		PT_3D temp = m_pt0;
		buf.BeginLineString(0,0);
		buf.MoveTo(&m_pt0);
		temp.x = pt.x;
		buf.LineTo(&temp);		
		buf.LineTo(&pt);
		temp.x = m_pt0.x;
		temp.y = pt.y;
		buf.LineTo(&temp);
		buf.LineTo(&m_pt0);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
	}
}

//比较两个三角形是否有公共边
static bool comparetri(MYPT_3D *tri1, MYPT_3D *tri2)
{
	int i, j, nSum=0;
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			if(GraphAPI::GIsEqual2DPoint(&tri1[i], &tri2[j]))
			{
				nSum++;
			}
			if(nSum>=2)
			{
				return true;
			}
		}
	}
	return false;
}

void CCheckSurfaceGapCommand::MergeTins()
{
	CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
	int i=0, j=0, k=0;
	MYPT_3D tri1[3];
	MYPT_3D tri2[3];

	for(i=0; i<m_tin.nTIN; i++)
	{
		GProgressStep();
		if(m_flags[i]<0) continue;
		if( !m_tin.GetTriangle(i,tri1) )
			continue;

		for(j=i+1; j<m_tin.nTIN; j++)
		{
			if(m_flags[j]<0) continue;
			if( !m_tin.GetTriangle(j,tri2) )
				continue;

			if( comparetri(tri1, tri2) )//如果有公关边，当做一组
			{
				int maxflag = max(m_flags[i], m_flags[j]);
				int minflag = min(m_flags[i], m_flags[j]);
				for(k=0; k<m_tin.nTIN; k++)
				{
					if(m_flags[k]==maxflag)
					{
						m_flags[k] = minflag;
					}
				}
			}
		}
	}

	MYPT_3D tri[3];
	int temp = -1;
	CArray<PT_3D,PT_3D> arrRetPts;
	CFtrArray arrRetFtrs;
	for(i=0; i<m_tin.nTIN; i++)
	{
		if(m_flags[i]<=temp) continue;
		temp=m_flags[i];

		double areaSum = 0;
		double maxArea = 0;//找出最大的tin,中心取做检查标记点
		int maxPos = 0;
		for(j=i; j<m_tin.nTIN; j++)
		{
			if(m_flags[j]==temp)
			{
				if( !m_tin.GetTriangle(j,tri) )
					continue;

				double area = GraphAPI::GGetTriangleArea(
					tri[0].x, tri[0].y, tri[1].x, tri[1].y, tri[2].x, tri[2].y);

				if(area>maxArea)
				{
					maxArea = area;
					maxPos = j;
				}
				areaSum += area;
			}
		}

		if( !m_tin.GetTriangle(maxPos,tri) )
			continue;

		PT_3D ptg;//重心
		ptg.x = (tri[0].x + tri[1].x + tri[2].x)/3;
		ptg.y = (tri[0].y + tri[1].y + tri[2].y)/3;
		ptg.z = 0.0;
		
		if(areaSum<m_lfArea)
			arrRetPts.Add(ptg);
// 		//关联一个附件的地物
// 		pDQ->FindNearestObjectByObjNum(ptg, 1, NULL);
// 		int nObj;
// 		const CPFeature *ftr = pDQ->GetFoundHandles(nObj);
// 		if(nObj>=1)
// 		{
// 			arrRetPts.Add(ptg);
// 			arrRetFtrs.Add(ftr[0]);
// 		}
	}

	if(m_bAddCheckResult)
		AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,1,0);
	
	CChkResManger *pChkRes = &GetChkResMgr();		
	
	CString chkName = StrFromResID(IDS_CMDNAME_SURFACEGAP);
	CString chkReason = StrFromResID(IDS_REASON_SURFACEGAP);
	
	for (j=0;j<arrRetPts.GetSize();j++)
	{
		pChkRes->BeginResItem(chkName);
//		pChkRes->AddAssociatedFtr(arrRetFtrs[j]);
		pChkRes->SetAssociatedPos(arrRetPts[j]);
		pChkRes->SetReason(chkReason);
		pChkRes->EndResItem();
	}
	
	if(m_bAddCheckResult)
		AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,0,0);
	GOutPut(StrFromResID(IDS_TIP_END));
}


//////////////////////////////////////////////////////////////////////
// CFillSurfaceATTFromPtSelCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CFillSurfaceATTFromPtSelCommand,CEditCommand)

CFillSurfaceATTFromPtSelCommand::CFillSurfaceATTFromPtSelCommand()
{
	m_nStep = -1;
	strcat(m_strRegPath,"\\FillSurfaceATTFromPtSel");
	m_pDS = NULL;
	m_bDelSurfacePt = FALSE;
}

CFillSurfaceATTFromPtSelCommand::~CFillSurfaceATTFromPtSelCommand()
{
}

CString CFillSurfaceATTFromPtSelCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_FILLSURFACE_FROMPT);
}

void CFillSurfaceATTFromPtSelCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();

	m_pEditor->OpenSelector();

	m_nStep = 0;
}

void CFillSurfaceATTFromPtSelCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (bool)(m_bDelSurfacePt);
	tab.AddValue("DelSurfacePt",&CVariantEx(var));
}

void CFillSurfaceATTFromPtSelCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("FillSurfaceATTFromPtSelCommand",Name());
	
	param->AddParam("DelSurfacePt",(bool)m_bDelSurfacePt,StrFromResID(IDS_CMDNAME_DEL_SURFACEPT));
}

void CFillSurfaceATTFromPtSelCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,"DelSurfacePt",var) )
	{
		m_bDelSurfacePt = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	CEditCommand::SetParams(tab,bInit);
}

void CFillSurfaceATTFromPtSelCommand::PtClick(PT_3D &pt, int flag)
{
	if( !CEditCommand::CanGetSelObjs(flag,TRUE) )
		return;
	if( !m_pEditor )return;
	m_pDS = m_pEditor->GetDataSource();
	if(!m_pDS) return;
	
	
	int num;
	const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num);

	//读取所有点地物
	int nLayer = m_pDS->GetFtrLayerCount();
	int i=0, j=0;
	CFtrArray arrPointFtr;
	for(i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = m_pDS->GetFtrLayerByIndex(i);
		
		if (!pLayer ||!pLayer->IsVisible())
		{
			continue;
		}
		int nObj = pLayer->GetObjectCount();
		for(j=0; j<nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;
			if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)))
			{
				arrPointFtr.Add(pFtr);
			}
		}
	}

	CUndoFtrs undo(m_pEditor,Name());

	GProgressStart(num);
	CFtrArray arrRetFtrs0;
	CArray<PT_3D,PT_3D> arrRetPts0;
	CFtrArray arrRetFtrs2;
	CArray<PT_3D,PT_3D> arrRetPts2;
	CFtrArray arrRetFtrs3;
	CArray<PT_3D,PT_3D> arrRetPts3;
	for(i=0; i<num; i++)
	{
		GProgressStep();
		CFeature * pFtr = HandleToFtr(handles[i]);

		CGeometry *pGeo = pFtr->GetGeometry();
		if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			continue;

		//FillAttribute函数内部根据m_bDelSurfacePt标志直接删除了面域点
		int ret = FillAttribute(arrPointFtr, pFtr, &undo);
		if(ret==0)
		{
			arrRetFtrs0.Add(pFtr);
			arrRetPts0.Add(pGeo->GetDataPoint(0));
		}
		else if(ret==1)
		{
			undo.AddOldFeature(FtrToHandle(pFtr));
			m_pDS->DeleteObject(pFtr);
		}
		else if(ret==2)
		{
			arrRetFtrs2.Add(pFtr);
			arrRetPts2.Add(pGeo->GetDataPoint(0));
		}
		else if(ret==3)
		{
			arrRetFtrs3.Add(pFtr);
			arrRetPts3.Add(pGeo->GetDataPoint(0));
		}
	}

	GProgressEnd();

	undo.Commit();

	if(arrRetFtrs0.GetSize()>0 || arrRetFtrs2.GetSize()>0 || arrRetFtrs3.GetSize()>0)
	{
		//添加到检查列表
		AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,1,0);
		
		CChkResManger *pChkRes = &GetChkResMgr();
		
		CString chkName = StrFromResID(IDS_CMDNAME_FILLSURFACE_FROMPT);
		CString chkReason0 = StrFromResID(IDS_NO_POINT_IN);
		CString chkReason2 = StrFromResID(IDS_DIFFRENT_POINT_IN);
		CString chkReason3 = StrFromResID(IDS_SURFACEPOINT_ATT_DIFFRENT);
		
		int j=0;
		for (j=0;j<arrRetFtrs0.GetSize();j++)
		{
			pChkRes->BeginResItem(chkName);
			pChkRes->AddAssociatedFtr(arrRetFtrs0[j]);
			pChkRes->SetAssociatedPos(arrRetPts0[j]);
			pChkRes->SetReason(chkReason0);
			pChkRes->EndResItem();
		}
		for (j=0;j<arrRetFtrs2.GetSize();j++)
		{
			pChkRes->BeginResItem(chkName);
			pChkRes->AddAssociatedFtr(arrRetFtrs2[j]);
			pChkRes->SetAssociatedPos(arrRetPts2[j]);
			pChkRes->SetReason(chkReason2);
			pChkRes->EndResItem();
		}
		for (j=0;j<arrRetFtrs3.GetSize();j++)
		{
			pChkRes->BeginResItem(chkName);
			pChkRes->AddAssociatedFtr(arrRetFtrs3[j]);
			pChkRes->SetAssociatedPos(arrRetPts3[j]);
			pChkRes->SetReason(chkReason3);
			pChkRes->EndResItem();
		}
		
		AfxGetMainWnd()->SendMessage(FCCM_CHECKRESULT,0,0);
	}
	
	Finish();
	CEditCommand::PtClick(pt,flag);
}


//////////////////////////////////////////////////////////////////////
// CSurOverPartCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSurOverPartCommand,CEditCommand)

CSurOverPartCommand::CSurOverPartCommand()
{
	m_pDS = NULL;
	m_pTempDS = NULL;
	m_pDQ = NULL;
	m_bSnap3D = FALSE;
	m_bAddPt = FALSE;
	m_lfToler = GraphAPI::g_lfDisTolerance;
	m_nStep = -1;
	strcat(m_strRegPath,"\\SurOverPart");
}

CSurOverPartCommand::~CSurOverPartCommand()
{
}

CString CSurOverPartCommand::Name()
{ 
	return StrFromResID(IDS_CMDPLANE_SUROVERPART);
}

void CSurOverPartCommand::Start()
{
	if( !m_pEditor )return;

	CEditCommand::Start();

	m_pEditor->OpenSelector(SELMODE_MULTI);

	int numsel;	
	m_pEditor->GetSelection()->GetSelectedObjs(numsel);	
	if(numsel>1 )
	{
		CCommand::Start();
		
		PT_3D pt;
		PtClick(pt,0);
		m_nExitCode = CMPEC_STARTOLD;
		return;
	}

	GOutPut(StrFromResID(IDS_SELECT_TWOSURFACES));
}

void CSurOverPartCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CSurOverPartCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CSurOverPartCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_targetLayer);
	tab.AddValue("Layer0",&CVariantEx(var));
}

void CSurOverPartCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("SurOverPartCommand",Name());
	
	param->AddLayerNameParam("Layer0",(LPCTSTR)m_targetLayer,StrFromResID(IDS_CMDPLANE_BOUNDALYER));
}

void CSurOverPartCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	if( tab.GetValue(0,"Layer0",var) )
	{					
		m_targetLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_targetLayer.TrimLeft();
		m_targetLayer.TrimRight();
		SetSettingsModifyFlag();		
	}

	CEditCommand::SetParams(tab,bInit);
}

void CSurOverPartCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor || m_targetLayer.IsEmpty())
	{
		Abort();
		return;
	}
	m_pDS = m_pEditor->GetDataSource();
	if(!m_pDS)
	{
		Abort();
		return;
	}

	int num ;
	const FTR_HANDLE * handles = m_pEditor->GetSelection()->GetSelectedObjs(num);
	if( m_nStep==0 )
	{
		if( num>=2 )
		{	
			if( !CanGetSelObjs(flag,FALSE) )return;
			CGeometry *pObj1 = HandleToFtr(handles[0])->GetGeometry();
			CGeometry *pObj2 = HandleToFtr(handles[1])->GetGeometry();
			
			if( pObj1==NULL || pObj2==NULL )
				return;
			
			if( !pObj1->IsKindOf(RUNTIME_CLASS(CGeoSurface)) || !pObj2->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				return;
			
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 1;
			m_pEditor->CloseSelector();
			return;
		}
		
	}

	if( m_nStep==1 && num>=2 )
	{
		GetOverPart(HandleToFtr(handles[0]), HandleToFtr(handles[1]));

		Finish();
		m_nStep = 3;
	}	

	CEditCommand::PtClick(pt,flag);
}

//点是否在线串上
bool IsPtInCurve(PT_3DEX pt, CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	for(int i=0; i<arrPts.GetSize()-1; i++)
	{
		if(arrPts[i+1].pencode==penMove)
			continue;
		if(GraphAPI::GIsPtInLine(arrPts[i], arrPts[i+1], pt))
		{
			return true;
		}
	}
	return false;
}

void CSurOverPartCommand::GetCommonLine(CFtrLayerArray& layers0, CGeoArray& geos)
{
	int i, j, k, nSum = 0;
	for( i=0; i<layers0.GetSize(); i++)
	{
		nSum += layers0[i]->GetValidObjsCount();
	}
		
	double r = m_lfToler;

	CFtrArray ftrsArray;
	ftrsArray.SetSize(nSum);

	j = 0;
	
	for( k=0; k<layers0.GetSize(); k++)
	{
		CFtrLayer *pLayer = layers0[k];
		int nObj = pLayer->GetObjectCount();
		for( i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i);
			if( !pFtr )continue;
			
			ftrsArray[j] = pFtr;
			pFtr->SetAppFlag(j);
			j++;
		}
	}
	
	ftrsArray.SetSize(j);
	
	//比较地物是否完全重叠，是的话，就删除其中一个
	SnapItemArray arr;
	
	for( i=j-1; i>=0; i--)
	{
		GProgressStep();
		
		if(ftrsArray[i]->IsDeleted()) continue;
		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		ftrsArray[i]->GetGeometry()->GetShape(arrPts);
		if(arrPts.GetSize()<1) continue;
		
		Envelope e;
		e.CreateFromPtAndRadius(arrPts[0],GraphAPI::g_lfDisTolerance);
		int num = m_pDQ->FindObjectInRect_SimpleAndSym(e,NULL,FALSE,FALSE);
		
		const CPFeature *ftrs = m_pDQ->GetFoundHandles(num);
		
		for( k=0; k<num; k++)
		{
			if( ftrs[k]->GetAppFlag()>=i )continue;
			
			if( IsOverlapped(ftrsArray[i],ftrs[k]) )
			{
				m_pTempDS->DeleteObject(ftrsArray[i]);
				geos.Add(ftrsArray[i]->GetGeometry()->Clone());
				break;
			}
		}
	}
	
	for( i=0; i<j; i++)
	{
		ftrsArray[i]->SetAppFlag(0);
	}
}

void CSurOverPartCommand::GetOverPart(CFeature* pFtr1,CFeature* pFtr2)
{
	if(!pFtr1 || !pFtr2)
		return;
	CArray<PT_3DEX, PT_3DEX> pts1,pts2;
	pFtr1->GetGeometry()->GetShape(pts1);
	pFtr2->GetGeometry()->GetShape(pts2);

	CFtrLayer *out_layer=m_pDS->GetOrCreateFtrLayer((LPCTSTR)m_targetLayer);
	if(out_layer==NULL)
	{
		return;
	}

	//拓扑预处理
	CFtrLayerArray arr;
	CFtrArray ftrs;
	ftrs.Add(pFtr1);
	ftrs.Add(pFtr2);
	CopyDataSource(ftrs, arr);//将pFtr和ftrs放入新数据源
	Linearize(arr);	
	SnapVertexes(arr);
	ProcessSuspend(arr);
	CutLines_self(arr);
	CutLines(arr);

	CGeoArray geos;
	GetCommonLine(arr, geos);
	//连接的线串合并
	MergeCurves(geos);

	CUndoFtrs undo(m_pEditor,Name());	
	CFeature *pFtrTempl = out_layer->CreateDefaultFeature(((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOCURVE);
	if(!pFtrTempl)
		return;
	
	for (int j=0; j<geos.GetSize(); j++)
	{
		if(geos[j]==NULL) continue;
		CFeature *pNew = pFtrTempl->Clone();
		pNew->SetID(OUID());
		CArray<PT_3DEX, PT_3DEX> arrPts;
		geos[j]->GetShape(arrPts);
		delete geos[j];
		pNew->GetGeometry()->CreateShape(arrPts.GetData(),arrPts.GetSize());
		if(!m_pEditor->AddObject(pNew,out_layer->GetID()))
		{
			delete pNew;
			continue;
		}
		undo.AddNewFeature(FtrToHandle(pNew));
	}
	
	delete pFtrTempl;
	undo.Commit();
}

void CSurOverPartCommand::MergeCurves(CGeoArray &arr)
{
	int i,j;
	CArray<PT_3DEX, PT_3DEX> pts1,pts2;
	for(i=0; i<arr.GetSize(); i++)
	{
		if(arr[i]==NULL) continue;
		arr[i]->GetShape(pts1);
		int nPt1 = pts1.GetSize();
		if(nPt1<2) continue;

		for(j=0; j<arr.GetSize(); j++)
		{
			if(i==j || arr[j]==NULL)
				continue;
			arr[j]->GetShape(pts2);
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
				arr[i]->CreateShape(newPts.GetData(), newPts.GetSize());
				pts1.Copy(newPts);
				nPt1 = pts1.GetSize();
				delete arr[j];
				arr[j] = NULL;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CRiverZChangeCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CRiverZChangeCommand,CEditCommand)

CRiverZChangeCommand::CRiverZChangeCommand()
{
	m_bZInc = TRUE;
	m_nStep = -1;
	strcat(m_strRegPath,"\\RiverZChange");
}

CRiverZChangeCommand::~CRiverZChangeCommand()
{

}

CString CRiverZChangeCommand::Name()
{
	return StrFromResID(IDS_RIVER_Z_CHANGE);
}

void CRiverZChangeCommand::Start()
{
 	if( !m_pEditor )return;

	CEditCommand::Start();
	
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();

	m_nStep = 0;
}

void CRiverZChangeCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CRiverZChangeCommand::Abort()
{	
	UpdateParams(TRUE);
    m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CRiverZChangeCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;
	
	var = (LPCTSTR)(m_RiverLayer);
	tab.AddValue("RiverLayer",&CVariantEx(var));

	var = (bool)(m_bZInc);
	tab.AddValue(CHK_RIVERZ_DESC,&CVariantEx(var));
}

void CRiverZChangeCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("RiverZChange",Name());

	param->AddLayerNameParamEx("RiverLayer",(LPCTSTR)m_RiverLayer,StrFromResID(IDS_LINK_RIVERLAYERS));

	param->BeginOptionParam(CHK_RIVERZ_DESC,StrFromResID(IDS_CHKCMD_DES_RIVERZ));
	param->AddOption(StrFromResID(IDS_CHK_Z_INC),1,' ',m_bZInc?TRUE:FALSE);
	param->AddOption(StrFromResID(IDS_CHK_Z_DEC),0,' ',m_bZInc?FALSE:TRUE);
	param->EndOptionParam();
}

void CRiverZChangeCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;

	if( tab.GetValue(0,"RiverLayer",var) )
	{					
		m_RiverLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_RiverLayer.TrimLeft();
		m_RiverLayer.TrimRight();
		SetSettingsModifyFlag();		
	}

	if( tab.GetValue(0,CHK_RIVERZ_DESC,var) )
	{
		m_bZInc = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab,bInit);
}

void CRiverZChangeCommand::PtClick(PT_3D &pt, int flag)
{
	if(!m_pEditor || m_RiverLayer.IsEmpty()) return;
	CDataSourceEx *pDS = m_pEditor->GetDataSource();
	if(!pDS) return;

	CFtrLayerArray arr;
	pDS->GetFtrLayersByNameOrCode_editable(m_RiverLayer,arr);
	CUndoModifyProperties undo(m_pEditor,Name());
	for(int i=0; i<arr.GetSize(); i++)
	{
		int nObj = arr[i]->GetObjectCount();
		for(int j=0; j<nObj; j++)
		{
			CFeature *pFtr = arr[i]->GetObject(j);
			if(!pFtr || !pFtr->IsVisible())
				continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				continue;

			CArray<PT_3DEX,PT_3DEX> pts, newPts;
			pGeo->GetShape(pts);

			CVariantEx varEx;
			varEx.SetAsShape(pts);
			CValueTable oldtab, tab;
			oldtab.BeginAddValueItem();
			oldtab.AddValue(FIELDNAME_SHAPE,&varEx);
			oldtab.EndAddValueItem();

			if(ZChange(pts, newPts) )
			{
				m_pEditor->DeleteObject(FtrToHandle(pFtr));
				pFtr->GetGeometry()->CreateShape(newPts.GetData(),newPts.GetSize());
				m_pEditor->RestoreObject(FtrToHandle(pFtr));

				varEx.SetAsShape(newPts);	
				tab.BeginAddValueItem();
				tab.AddValue(FIELDNAME_SHAPE,&varEx);
				tab.EndAddValueItem();
				
				undo.SetModifyProperties(FtrToHandle(pFtr),oldtab,tab,TRUE);
			}
		}
	}
	undo.Commit();
	
	Finish();
	CEditCommand::PtClick(pt,flag);
}

BOOL CRiverZChangeCommand::ZChange(CArray<PT_3DEX,PT_3DEX>& pts, CArray<PT_3DEX,PT_3DEX>& newPts)
{
	int nPt = pts.GetSize();
	if(nPt<2) return FALSE;

	BOOL bRet = FALSE;

	int i, j, k;
	if( (m_bZInc&&pts[0].z>pts[nPt-1].z) || (!m_bZInc&&pts[0].z<pts[nPt-1].z) )
	{
		for(i=0; i<nPt/2; i++)
		{
			PT_3DEX temp = pts[i];
			pts[i] = pts[nPt-1-i];
			pts[nPt-1-i] = temp;
		}
		bRet = TRUE;
	}

	if(m_bZInc)
	{
		newPts.Add(pts[0]);
		for(i=1; i<nPt; i+=j)
		{
			j=1;

			//若i点的高程值小于前一个点 或者大于末尾点的高程，则认为不合理
			if(pts[i].z<pts[i-1].z || pts[i].z>pts[nPt-1].z)
			{
				for(j=1 ;i+j<nPt-1; j++)
				{
					if(pts[i+j].z>pts[i-1].z && pts[i+j].z<pts[nPt-1].z)
						break;
				}

				double len = GraphAPI::GGetAllLen2D(&pts[i-1], j+2);
				double t = pts[i+j].z - pts[i-1].z;
				for(k=0; k<j; k++)
				{
					double len1 = GraphAPI::GGetAllLen2D(&pts[i-1], k+2);
					pts[i+k].z = pts[i-1].z + t*len1/(len);
				}

				bRet = TRUE;
			}

			for(k=0; k<j; k++)
			{
				newPts.Add(pts[i+k]);
			}
		}
	}
	else
	{
		newPts.Add(pts[0]);
		for(i=1; i<nPt; i+=j)
		{
			j=1;
			
			//若i点的高程值大于前一个点 或者小于末尾点的高程，则认为不合理
			if(pts[i].z>pts[i-1].z || pts[i].z<pts[nPt-1].z)
			{
				for(j=1 ;i+j<nPt-1; j++)
				{
					if(pts[i+j].z<pts[i-1].z && pts[i+j].z>pts[nPt-1].z)
						break;
				}
				
				double len = GraphAPI::GGetAllLen2D(&pts[i-1], j+2);
				double t = pts[i+j].z - pts[i-1].z;
				for(k=0; k<j; k++)
				{
					double len1 = GraphAPI::GGetAllLen2D(&pts[i-1], k+2);
					pts[i+k].z = pts[i-1].z + t*len1/(len);
				}
				
				bRet = TRUE;
			}
			
			for(k=0; k<j; k++)
			{
				newPts.Add(pts[i+k]);
			}
		}
	}
	return bRet;
}


//////////////////////////////////////////////////////////////////////
// CSurfaceToBoundCommand Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSurfaceToBoundCommand, CEditCommand)

CSurfaceToBoundCommand::CSurfaceToBoundCommand()
{
	m_pDS = NULL;
	m_pTempDS = NULL;
	m_pDQ = NULL;
	m_bSnap3D = FALSE;
	m_bAddPt = FALSE;
	m_lfToler = GraphAPI::g_lfDisTolerance;
	m_nStep = -1;
	strcat(m_strRegPath, "\\SurOverPart");
}

CSurfaceToBoundCommand::~CSurfaceToBoundCommand()
{
}

CString CSurfaceToBoundCommand::Name()
{
	return StrFromResID(IDS_CMDPLANE_SURFACETOAREA);
}

void CSurfaceToBoundCommand::Start()
{
	if (!m_pEditor)return;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();

	m_nStep = 0;
}

void CSurfaceToBoundCommand::Finish()
{
	UpdateParams(TRUE);

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->RefreshView();
	CEditCommand::Finish();
}

void CSurfaceToBoundCommand::Abort()
{
	UpdateParams(TRUE);
	m_nStep = -1;
	CEditCommand::Abort();
	m_pEditor->DeselectAll();
	m_pEditor->UpdateDrag(ud_ClearDrag);
}

void CSurfaceToBoundCommand::GetParams(CValueTable& tab)
{
	CEditCommand::GetParams(tab);
	_variant_t var;

	var = (LPCTSTR)(m_handleLayers);
	tab.AddValue("handleLayers", &CVariantEx(var));

	var = (LPCTSTR)(m_targetLayer);
	tab.AddValue("Layer0", &CVariantEx(var));

	var = (LPCTSTR)(m_excludelayers);
	tab.AddValue("excludelayers", &CVariantEx(var));
}

void CSurfaceToBoundCommand::FillShowParams(CUIParam* param, BOOL bForLoad)
{
	param->SetOwnerID("SurOverPartCommand", Name());

	param->AddLayerNameParamEx("handleLayers", (LPCTSTR)m_handleLayers, StrFromResID(IDS_CMDPLANE_HANDLELAYER));
	param->AddLayerNameParam("Layer0", (LPCTSTR)m_targetLayer, StrFromResID(IDS_CMDPLANE_BOUNDALYER));
	param->AddLayerNameParamEx("excludelayers", (LPCTSTR)m_excludelayers, StrFromResID(IDS_COMMON_LINE_NO_AREA));
}

void CSurfaceToBoundCommand::SetParams(CValueTable& tab, BOOL bInit)
{
	const CVariantEx *var;
	if (tab.GetValue(0, "handleLayers", var))
	{
		m_handleLayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_handleLayers.TrimLeft();
		m_handleLayers.TrimRight();
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "Layer0", var))
	{
		m_targetLayer = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_targetLayer.TrimLeft();
		m_targetLayer.TrimRight();
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0, "excludelayers", var))
	{
		m_excludelayers = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		m_excludelayers.TrimLeft();
		m_excludelayers.TrimRight();
		SetSettingsModifyFlag();
	}

	CEditCommand::SetParams(tab, bInit);
}

void CSurfaceToBoundCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor || m_targetLayer.IsEmpty() || m_handleLayers.IsEmpty())
	{
		return;
	}
	m_pDS = m_pEditor->GetDataSource();
	if (!m_pDS)
	{
		return;
	}

	CFtrLayerArray arr1, arr2;
	m_pDS->GetFtrLayersByNameOrCode_editable(m_handleLayers, arr1);
	if (!m_excludelayers.IsEmpty())
	{
		m_pDS->GetFtrLayersByNameOrCode_editable(m_excludelayers, arr2);
	}

	CFtrLayer *out_layer = NULL;
	out_layer = m_pDS->GetFtrLayer((LPCTSTR)m_targetLayer);
	if (out_layer == NULL)
	{
		out_layer = new CFtrLayer();
		out_layer->SetName((LPCTSTR)m_targetLayer);
		m_pEditor->AddFtrLayer(out_layer);
	}
	else
	{
		arr2.Add(out_layer);//地类界图层也要排除
	}

	CFtrArray arrftrs;
	int i, j;
	for (i = 0; i < arr1.GetSize(); i++)
	{
		CFtrLayer *pLayer = arr1[i];
		int nObj = pLayer->GetObjectCount();
		for (j = 0; j < nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr) continue;
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				continue;
			CFeature *pTemp = pFtr->Clone();
			pTemp->SetCode("selectedftr");
			arrftrs.Add(pTemp);
		}
	}
	for (i = 0; i < arr2.GetSize(); i++)
	{
		CFtrLayer *pLayer = arr2[i];
		int nObj = pLayer->GetObjectCount();
		for (j = 0; j < nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr) continue;
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				continue;
			CFeature *pTemp = pFtr->Clone();
			pTemp->SetCode("");
			arrftrs.Add(pTemp);
		}
	}

	//拓扑预处理
	CFtrLayerArray arr;
	int nSum = CopyDataSource(arrftrs, arr);//将pFtr和ftrs放入新数据源
	if (nSum == 0) return;

	for (i = 0; i < arrftrs.GetSize(); i++)
	{
		delete arrftrs[i];
	}

	GProgressStart(9 * nSum);
	//线串化, Progress: 1*nSum
	Linearize(arr);
	//节点咬合, Progress: 1*nSum
	SnapVertexes(arr);
	//消除悬挂点, Progress: 1*nSum
	ProcessSuspend(arr);
	//打断自相交线, Progress: 2*nSum
	CutLines_self(arr);
	//打断相交线, Progress: 2*nSum
	CutLines(arr);
	//剔除公共边, Progress: 1*nSum
	DeleteCommonLine(arr);
	GProgressEnd();
	m_pTempDS->RestoreAllQueryFlags();

	CUndoFtrs undo(m_pEditor, Name());
	CFeature *pFtrTempl = out_layer->CreateDefaultFeature(((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetScale(), CLS_GEOCURVE);
	if (!pFtrTempl) return;
	int nLay = m_pTempDS->GetFtrLayerCount();
	for (i = 0; i < nLay; i++)
	{
		CFtrLayer* pLayer = m_pTempDS->GetFtrLayerByIndex(i);
		if (!pLayer) continue;

		int nObj = pLayer->GetObjectCount();
		for (j = 0; j < nObj; j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (pFtr && 0 == strcmp("selectedftr", pFtr->GetCode()))
			{
				CFeature *pFtr0 = pFtrTempl->Clone();
				pFtr0->SetID(OUID());
				CArray<PT_3DEX, PT_3DEX> arrPts;
				pFtr->GetGeometry()->GetShape(arrPts);
				pFtr0->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize());
				if (!m_pEditor->AddObject(pFtr0, out_layer->GetID()))
				{
					delete pFtr0;
					continue;
				}
				undo.AddNewFeature(FtrToHandle(pFtr0));
			}
		}
	}
	delete pFtrTempl;
	undo.Commit();

	CEditCommand::PtClick(pt, flag);
}


//////////////////////////////////////////////////////////////////////
// CTopoSurfaceRegionCommand Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CTopoSurfaceRegionCommand, CEditCommand)

CTopoSurfaceRegionCommand::CTopoSurfaceRegionCommand()
{

}

CTopoSurfaceRegionCommand::~CTopoSurfaceRegionCommand()
{

}

CString CTopoSurfaceRegionCommand::Name()
{
	return StrFromResID(IDS_CMDNAME_REGION_TOPO);
}


void CTopoSurfaceRegionCommand::Start()
{
	if (!m_pEditor)return;

	m_nStep = 0;

	CEditCommand::Start();

	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	m_pEditor->CloseSelector();

	GOutPut(StrFromResID(IDS_INSURE_VISIBLE));
}

void CTopoSurfaceRegionCommand::Abort()
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

void CTopoSurfaceRegionCommand::PtClick(PT_3D &pt, int flag)
{
	if (!m_pEditor)return;

	if (m_nStep == 0)
	{
		m_ptDragStart = pt;
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);
		return;
	}
	else if (m_nStep == 1)
	{
		m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
		m_ptDragEnd = pt;
		PT_3D pts[2];
		pts[0] = m_ptDragStart;
		pts[1] = m_ptDragEnd;

		CDataSourceEx *pDS = m_pEditor->GetDataSource();
		CDataQueryEx *pDQ = m_pEditor->GetDataQuery();
		if (!pDS || !pDQ)
		{
			Abort();
			return;
		}

		//拷贝屏幕范围内的图层
		CRect rc;
		GetActiveView()->GetClientRect(&rc);
		PT_4D pt4ds[4];
		pt4ds[0].x = rc.left; pt4ds[0].y = rc.top;  pt4ds[0].z = rc.left;  pt4ds[0].yr = rc.top;
		pt4ds[1].x = rc.right; pt4ds[1].y = rc.top;  pt4ds[1].z = rc.right;  pt4ds[1].yr = rc.top;
		pt4ds[2].x = rc.right; pt4ds[2].y = rc.bottom;  pt4ds[2].z = rc.right;  pt4ds[2].yr = rc.bottom;
		pt4ds[3].x = rc.left; pt4ds[3].y = rc.bottom;  pt4ds[3].z = rc.left;  pt4ds[3].yr = rc.bottom;
		m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(pt4ds, 4);
		PT_3D pt3ds[4];
		int i, j;
		for (i = 0; i < 4; i++)
		{
			pt3ds[i] = pt4ds[i].To3D();
		}

		Envelope e0;
		e0.CreateFromPts(pt3ds, 4);
		int num = pDQ->FindObjectInRect(e0, NULL, FALSE, FALSE);
		const CPFeature *ftrs = pDQ->GetFoundHandles(num);
		CFeature *pFtr = NULL;
		CFtrArray oldftrs;
		for (i = 0; i < num; i++)
		{
			CFeature *pNew = ftrs[i]->Clone();
			pNew->SetAppFlag(m_pEditor->GetFtrLayerIDOfFtr(FtrToHandle(ftrs[i])));
			oldftrs.Add(pNew);
		}

		//调用非打断构面
		CTopoSurfaceNoBreakCommand cmd;
		cmd.Init(m_pEditor);
		CFtrArray newftrs;//存放旧地物
		cmd.BuildSurface(oldftrs, newftrs, TRUE);


		//找到所有在矩形框内且不在某个面内部的面域点
		Envelope ee0;
		ee0.CreateFromPts(pts, 2);
		pDQ->FindObjectInRect(ee0, NULL, FALSE, FALSE);
		num = 0;
		const CPFeature *ftrs0 = pDQ->GetFoundHandles(num);
		CFtrArray surfacepts;
		CGeoArray surfaces;
		for (i = 0; i < num; i++)
		{
			CGeometry *pGeo = ftrs0[i]->GetGeometry();
			if (pGeo && pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				surfaces.Add(pGeo);
			}
		}
		int numsurfaces = surfaces.GetSize();
		for (i = 0; i < num; i++)
		{
			CGeometry *pGeo0 = ftrs0[i]->GetGeometry();
			if (!pGeo0 || !pGeo0->IsKindOf(RUNTIME_CLASS(CGeoSurfacePoint)))
				continue;

			PT_3DEX pt = pGeo0->GetDataPoint(0);

			for (j = 0; j < numsurfaces; j++)
			{
				if (surfaces[j]->bPtIn(&pt))
				{
					break;
				}
			}
			if (j >= numsurfaces)
			{
				surfacepts.Add(ftrs0[i]);
			}
		}

		//找到包含单点的面
		CUndoFtrs undo(m_pEditor, Name());
		int nObj = newftrs.GetSize();
		for (i = 0; i < nObj; i++)
		{
			CFeature* pFtr = newftrs[i];
			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				continue;

			int nPtInSum = 0;
			CFeature *pFtr_old1 = NULL;
			for (j = 0; j < surfacepts.GetSize(); j++)
			{
				PT_3DEX pt_0 = surfacepts[j]->GetGeometry()->GetDataPoint(0);
				if (pGeo->bPtIn(&pt_0))
				{
					pFtr_old1 = surfacepts[j];
					nPtInSum++;
				}
			}

			if (nPtInSum != 1) continue;

			CFtrLayer *pOutLayer = pDS->GetFtrLayerOfObject(pFtr_old1);
			if (pOutLayer == NULL) continue;

			int nCls = pGeo->GetClassType();
			CFeature *pNew = pOutLayer->CreateDefaultFeature(((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetScale(), nCls);
			if (!pNew) continue;

			CArray<PT_3DEX, PT_3DEX> arrPts;
			pGeo->GetShape(arrPts);
			if (!pNew->GetGeometry()->CreateShape(arrPts.GetData(), arrPts.GetSize()))
			{
				delete pNew;
				continue;
			}
			pNew->SetID(OUID());
			pNew->SetPurpose(FTR_EDB);

			if (!m_pEditor->AddObject(pNew, pOutLayer->GetID()))
			{
				delete pNew;
				continue;
			}
			GETXDS(m_pEditor)->CopyXAttributes(pFtr_old1, pNew);

			undo.AddNewFeature(FtrToHandle(pNew));
		}

		undo.Commit();

		for (i = 0; i < nObj; i++)
		{
			delete newftrs[i];
		}

		m_pEditor->RefreshView();
		Finish();
	}

	CEditCommand::PtClick(pt, flag);
}

void CTopoSurfaceRegionCommand::PtMove(PT_3D &pt)
{
	if (m_nStep == 1)
	{
		GrBuffer buf;
		PT_3D temp = m_ptDragStart;
		buf.BeginLineString(0, 0);
		buf.MoveTo(&m_ptDragStart);
		temp.x = pt.x;
		buf.LineTo(&temp);
		buf.LineTo(&pt);
		temp.x = m_ptDragStart.x;
		temp.y = pt.y;
		buf.LineTo(&temp);
		buf.LineTo(&m_ptDragStart);
		buf.End();
		m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf);
		return;
	}
}