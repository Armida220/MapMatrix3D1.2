// Command.cpp: implementation of the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "CommandLidar.h"

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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GP_FAIL				0
#define GP_SUCCESS			1
#define GP_NONEXT			2

static void CutParam(char *pParams, char *pos)
{
	int len1 = strlen(pParams), len2 = strlen(pos);
	if( len2>0 )memmove(pParams,pos,len2);
	if( len1>len2 )memset(pParams+len2,0,len1-len2);
}

static int GetParamInt(char *pParams, int& value, char *& pos)
{
	char *pos1 = NULL;
	int v = strtol(pParams,&pos1,10);
	if( !pos1 || pos1==pParams )return GP_FAIL;
	value = v;
	pos = pos1;
	if( *pos1==0 )return GP_NONEXT;
	pos = pos1+1;
	return GP_SUCCESS;
}

static int GetParamDouble(char *pParams, double& value, char *& pos)
{
	char *pos1 = NULL;
	double v = strtod(pParams,&pos1);
	if( !pos1 || pos1==pParams )return GP_FAIL;
	value = v;
	pos = pos1;
	if( *pos1==0 )return GP_NONEXT;
	pos = pos1+1;
	return GP_SUCCESS;
}

static void PutParamInt(char *pParams, int value)
{
	char text[256]={0};
	sprintf(text,"%d,",value);
	strcat(pParams,text);
}

static void PutParamDouble(char *pParams, double value)
{
	char text[256]={0};
	sprintf(text,"%.6f,",value);
	strcat(pParams,text);
}


#define CMDOP_START				1
#define CMDOP_ABORT				2
#define CMDOP_FINISH			3
#define CMDOP_KEYDOWN			4
#define CMDOP_PTCLICKDOWN		5
#define CMDOP_PTCLICKUP			6
#define CMDOP_PTMOVE			7
#define CMDOP_PTRESET			8
#define CMDOP_BACK				9
#define CMDOP_UNDO				10
#define CMDOP_REDO				11

#define CMDERR_TYPEINVALID		20
#define CMDERR_TOOFEWPTS		21
#define CMDERR_TOOMANYOBJS		22



//////////////////////////////////////////////////////////////////////
// CEditLidarCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CEditLidarCommand,CCommand)

CEditLidarCommand::CEditLidarCommand()
{
	m_nStep = -1;

	m_nModifyWay = modifyNone;
	m_nKeyPt = 0;

	m_bSelectDrag = FALSE;

	strcat(m_strRegPath,"\\EditLidar");
}

CEditLidarCommand::~CEditLidarCommand()
{
}

CString CEditLidarCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_EDIT);
}

void CEditLidarCommand::Start()
{
	if( m_pEditor )
	{
		m_pEditor->CloseSelector();
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Attach_Accubox);
	}
	
	m_nStep = 0;
	m_nModifyWay = modifyNone;
	m_nKeyPt = 0;
	
	CCommand::Start();
}



void CEditLidarCommand::Abort()
{
	
	m_lidarSelection.Clear();
//	PDOC(m_pEditor)->SetCurDrawingObj(DrawingInfo());
	if( m_pEditor )
	{
		m_pEditor->CloseSelector();
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Detach_Accubox);
//		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_ClearDragLine);
		m_pEditor->UpdateDrag(ud_ClearDrag);
	}
	
	m_nStep = -1;
	m_nModifyWay = modifyNone;
	m_nKeyPt = 0;
//	m_idsOld.RemoveAll();
//	m_idsNew.RemoveAll();
	CCommand::Abort();
}


void CEditLidarCommand::Finish()
{
	m_lidarSelection.Clear();
	m_pEditor->UpdateDrag(ud_ClearDrag);
//	PDOC(m_pEditor)->SetCurDrawingObj(DrawingInfo());
//	m_pEditor->CloseSelector();
	CCommand::Finish();
}


void CEditLidarCommand::AimAllSelObjs()
{
// 	m_idsOld.RemoveAll();
// 
// 	if( (m_lidarSelection.m_nSelFlag&SELSTAT_DRAGSEL)!=0 )
// 		FindLidarObj(PDOC(m_pEditor),m_idsOld);
// 	else if( (m_lidarSelection.m_nSelFlag&SELSTAT_POINTSEL)!=0 )
// 	{
// 		m_idsOld.Add(m_lidarSelection.keyPt.id);
// 	}
}



int CEditLidarCommand::FindLidarObj(CDlgDoc *pDoc, CIDArray& ids)
{
	if( (m_lidarSelection.m_nSelFlag&SELSTAT_DRAGSEL)==0 )
		return 0;
	
	Envelope e;
	e.CreateFromPts(m_lidarSelection.boundPts.GetData(),m_lidarSelection.boundPts.GetSize());
	//转换成客户坐标
	e.TransformGrdToClt(pDoc->GetCoordWnd().m_pSearchCS,1);
	int num = pDoc->GetDataQuery()->FindObjectInRect(e,pDoc->GetCoordWnd().m_pSearchCS);
	if( num<=0 )return 0;
	
	const CPFeature *pFtr = NULL;
	pFtr = pDoc->GetDataQuery()->GetFoundHandles(num);
	for (int i=0; i<num; i++,pFtr++)
	{
		CGeometry *pObj = (*pFtr)->GetGeometry();
		if (!pObj) continue;

		if (!pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint))) continue;

		ids.Add(FtrToHandle(*pFtr));
	}
	return ids.GetSize();
}


void CEditLidarCommand::EditStepOne()
{
	AimAllSelObjs();
//	if( m_idsOld.GetSize()<=0 )return;
	
//	Run();
	m_nStep = 1;
	m_pEditor->CloseSelector();
}


BOOL CEditLidarCommand::CanGetSelObjs(int flag, BOOL bMultiSel, BOOL bOutputTip)
{
	//选择集没有发生变化，可以接收数据
	if( flag==SELSTAT_NONESEL )
		return TRUE;
	
	if( bMultiSel )
	{
		//正在多选，跳过
		if( (flag&SELSTAT_MULTISEL)!=0 )return FALSE;
		
		if( bOutputTip && (flag&SELSTAT_DRAGSEL_RESTART)==0 )
			GOutPut(StrFromResID(IDS_CMDTIP_CLICKOK));
		
		//正在重新拉框，可以接收数据
		return ( (flag&SELSTAT_DRAGSEL_RESTART)!=0 );
	}
	else
	{
		//点选完成，可以接收数据
		return ( (flag&SELSTAT_POINTSEL)!=0 );
	}
}


void CEditLidarCommand::PtReset(PT_3D &pt)
{
	m_lidarSelection.Clear();
	m_pEditor->DeselectAll();
	m_pEditor->OnSelectChanged();
	
	Abort();
	CCommand::PtReset(pt);
}

void CEditLidarCommand::PtMove(PT_3D &pt)
{
	if( m_bSelectDrag )//&& m_nStep==1 )
	{
		PT_4D t0[2];
		PT_4D t1[4];
		Envelope e;
		
		t0[0] = PT_4D(m_ptSelDragStart);
		// 		m_coordwnd.m_pViewCS->GroundToClient(t0,1);	
		// 		m_coordwnd.m_pViewCS->ClientToGround(t0,1);	
		
		t0[0] = PT_4D(m_ptSelDragStart); t0[1] = PT_4D(pt);
		m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(t0,2);	
		e.CreateFromPts(t0,2,sizeof(PT_4D));
		
		
		t1[0] = t0[0];
		t1[1].x = t0[1].x;  t1[1].y = t0[0].y; t1[1].z = t0[1].z;  t1[1].yr = t0[0].yr;
		t1[2] = t0[1];
		t1[3].x = t0[0].x;  t1[3].y = t0[1].y; t1[3].z = t0[0].z;  t1[3].yr = t0[1].yr;
		
		
		m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(t1,4);
		
		GrBuffer buf;
		buf.BeginLineString(0,0,0);
		buf.MoveTo(t1);
		buf.LineTo(t1+1);
		buf.LineTo(t1+2);
		buf.LineTo(t1+3);
		buf.LineTo(t1);
		buf.End();
		
// 		if (m_lidarSelection.m_nSelFlag == SELSTAT_DRAGSEL)
// 		{
// 			m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
// 		}
// 		else
		m_pEditor->UpdateDrag(ud_SetVariantDrag,&buf);
		
	}
}

void CEditLidarCommand::PtClick(PT_3D &pt, int flag)
{
	if (m_lidarSelection.m_nSelFlag==SELSTAT_POINTSEL || m_lidarSelection.m_nSelFlag==SELSTAT_DRAGSEL)
	{
		return;
	}
	BOOL bPtInRect = FALSE;	
	PT_3D cltpt;

	if( m_lidarSelection.m_nSelFlag==SELSTAT_DRAGSEL )
	{
		Envelope rect;
		rect.CreateFromPts(m_lidarSelection.boundPts.GetData(),m_lidarSelection.boundPts.GetSize());
		if( rect.bPtIn(&pt) )
		{
			bPtInRect = TRUE;
		}
	}

	//选择器不是正在拉框选择
	if( !m_bSelectDrag && !bPtInRect )
	{
		// 搜索坐标转
		m_pEditor->GetCoordWnd().m_pSearchCS->GroundToClient(&pt,&cltpt);

		m_ptSelDragStart = pt;
		//			m_bSelectDrag = TRUE;
		
		double r = m_pEditor->GetSelection()->GetSelectRadius()*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS();
// 		PT_3D t1[4];
// 		t1[0].x = cltpt.x-r; t1[0].y = cltpt.y-r; t1[0].z = cltpt.z-r; 
// 		//	ClientToGround(&t0,t1);
// 		t1[1].x = cltpt.x+r; t1[1].y = cltpt.y-r; t1[1].z = cltpt.z+r; 
// 		//	ClientToGround(&t0,t1+1);
// 		t1[2].x = cltpt.x+r; t1[2].y = cltpt.y+r; t1[2].z = cltpt.z+r; 
// 		//	ClientToGround(&t0,t1+2);
// 		t1[3].x = cltpt.x-r; t1[3].y = cltpt.y+r; t1[3].z = cltpt.z-r; 
// 		//	ClientToGround(&t0,t1+3);	
		
	
		
		//			OBJ_GUID id = pDoc->m_selection.FindNearestObject(&pt3d,e);
		CPFeature pFtr = PDOC(m_pEditor)->GetDataQuery()->FindNearestObject(cltpt,r/*10*m_pEditor->GetCoordWnd().GetScaleOfSearchCSToViewCS()*/,m_pEditor->GetCoordWnd().m_pSearchCS);
		//相交捕捉意味着会有两个地物与捕捉点都是零距离，所以应该选择哪个，
		//由捕捉的对象来判断更合适
		// 			if( pDoc->m_snap.bOpen() && m_itemCurSnap.nSnapMode==CSnap::modeIntersect && 
		// 				m_itemCurSnap.IsValid() )
		// 				id = m_itemCurSnap.id;
		
		BOOL bValidPoint = FALSE;
		
		if ( !pFtr||!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
		{
			m_bSelectDrag = TRUE;
		}
		else
		{
			//鼠标单选: 在选中的点上绘制一个小框框
			CGeometry *pObj = pFtr->GetGeometry();
//			if( pObj!=NULL && pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
			{
				//					double r = ((e.m_xh-e.m_xl) + (e.m_yh-e.m_yl))*0.25;
				//					int nPt = PDOC(m_pEditor)->GetDataQuery()->FindNearestKeyPtInObject(&pt3d,r,pObj);
				
				PT_3D ptRet;
				PT_KEYCTRL  keypt = pObj->FindNearestKeyCtrlPt(cltpt,r,m_pEditor->GetCoordWnd().m_pSearchCS);
				if (!keypt.IsValid())
				{
					m_bSelectDrag = TRUE;
				}
// 				if (!pObj->FindNearestBasePt(cltpt,e,m_pEditor->GetCoordWnd().m_pSearchCS,&ptRet,NULL))
// 				{
// 					m_bSelectDrag = TRUE;
// 				}
				else
				{
					int nPt = keypt.index;
					//在节点上绘制一个方形
					PT_3DEX expt;
					PT_4D cltpt2;
					expt = pObj->GetDataPoint(nPt);
					
					int len = 4;
					
					//						GroundToClient(&expt,&cltpt2);
					m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(&expt,&cltpt2);
					
					PT_4D t0;
					PT_4D t1[5];
					t0.x = cltpt2.x-len; t0.y = cltpt2.y-len; t0.z = cltpt2.z-len; t0.yr = cltpt2.y-len;
					//						ClientToGround(&t0,t1);
					m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(&t0,t1);
					t0.x = cltpt2.x+len; t0.y = cltpt2.y-len; t0.z = cltpt2.z+len; t0.yr = cltpt2.y-len;
					//						ClientToGround(&t0,t1+1);
					m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(&t0,t1+1);
					t0.x = cltpt2.x+len; t0.y = cltpt2.y+len; t0.z = cltpt2.z+len; t0.yr = cltpt2.y+len;
					//						ClientToGround(&t0,t1+2);
					m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(&t0,t1+2);
					t0.x = cltpt2.x-len; t0.y = cltpt2.y+len; t0.z = cltpt2.z+len; t0.yr = cltpt2.y-len;
					//						ClientToGround(&t0,t1+3);
					m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(&t0,t1+3);
					t0.x = cltpt2.x-len; t0.y = cltpt2.y-len; t0.z = cltpt2.z-len; t0.yr = cltpt2.y-len;
					//						ClientToGround(&t0,t1+4);
					m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(&t0,t1+4);
					
					GrBuffer buf;
					//						buf.SetColor(m_clrHilite);
					buf.BeginLineString(RGB(255,255,255),0,0);
					buf.MoveTo(t1);
					buf.LineTo(t1+1);
					buf.LineTo(t1+2);
					buf.LineTo(t1+3);
					buf.LineTo(t1+4);
					buf.LineTo(t1+0);
					buf.End();
					
					m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
					m_pEditor->UpdateDrag(ud_SetVariantDrag);
					m_pEditor->RefreshView();				
					
// 					flag |= SELSTAT_POINTSEL;
					
				
					
					m_lidarSelection.keyPt.id = FtrToHandle(pFtr);
					m_lidarSelection.keyPt.nPt = nPt;
					m_lidarSelection.m_nSelFlag = SELSTAT_POINTSEL;
					
					bValidPoint = TRUE;
					
					//输出高程信息
					{						
						CString strHg;
					
						strHg += _T("\n");
						
						GOutPut(strHg);
					}
				}
				
			}
		}
// 		
// 		if( !bValidPoint && m_lidarSelection.m_nSelFlag!=SELSTAT_NONESEL )
// 		{
// 			flag = SELSTAT_DRAGSEL_RESTART;
// 		}
		
			
// 		if (!m_bSelectDrag)
// 		{
// //			Finish();
// //			m_nStep = 1;
// 			
// 		}
		return;
			

	}
	//鼠标框选
	else if(m_bSelectDrag)
	{
		// 视图坐标转
		m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(&pt,&cltpt);

		//绘制矩形框
		PT_4D t0;
		PT_4D t1[4];
		PT_4D startpt(m_ptSelDragStart);
		m_pEditor->GetCoordWnd().m_pViewCS->GroundToClient(&startpt,1);

		t1[0].x = startpt.x; t1[0].y = startpt.y; t1[0].z = t1[0].x+cltpt.z-cltpt.x; t1[0].yr = t1[0].y;
		m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(t1,1);
		t1[1].x = cltpt.x; t1[1].y = startpt.y; t1[1].z = t1[1].x+cltpt.z-cltpt.x; t1[1].yr = t1[1].y;
		m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(t1+1,1);
		t1[2].x = cltpt.x; t1[2].y = cltpt.y; t1[2].z = t1[2].x+cltpt.z-cltpt.x; t1[2].yr = t1[2].y;
		m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(t1+2,1);
		t1[3].x = startpt.x; t1[3].y = cltpt.y; t1[3].z = t1[3].x+cltpt.z-cltpt.x; t1[3].yr = t1[3].y;
		m_pEditor->GetCoordWnd().m_pViewCS->ClientToGround(t1+3,1);
		
		m_lidarSelection.boundPts.RemoveAll();
		m_lidarSelection.boundPts.Add(t1[0]);
		m_lidarSelection.boundPts.Add(t1[1]);
		m_lidarSelection.boundPts.Add(t1[2]);
		m_lidarSelection.boundPts.Add(t1[3]);
		
		GrBuffer buf;
		buf.BeginLineString(RGB(255,255,255),1);
		buf.MoveTo(t1);
		buf.LineTo(t1+1);
		buf.LineTo(t1+2);
		buf.LineTo(t1+3);
		buf.LineTo(t1+0);
		buf.End();
		
		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->UpdateDrag(ud_SetConstDrag,&buf);
		m_pEditor->RefreshView();
// 		SetConstDragLine(&buf);
// 		SetVariantDragLine(NULL);
		
		m_bSelectDrag = FALSE;
// 		flag |= SELSTAT_DRAGSEL;

		m_lidarSelection.m_nSelFlag = SELSTAT_DRAGSEL;
		
//		m_nStep = 2;
//		Finish();
		return;
	}	
	
}


//////////////////////////////////////////////////////////////////////
// CDeleteLidarCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDeleteLidarCommand,CEditLidarCommand)

CDeleteLidarCommand::CDeleteLidarCommand()
{
}

CDeleteLidarCommand::~CDeleteLidarCommand()
{
}

CCommand* CDeleteLidarCommand::Create()
{
	return new CDeleteLidarCommand;
}

CString CDeleteLidarCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_DELLIDAR);
}


void CDeleteLidarCommand::Start()
{
	CEditLidarCommand::Start();

	if( !m_pEditor )return;	

	m_nModifyWay = modifyNone;
	
}



void CDeleteLidarCommand::PtClick(PT_3D &pt, int flag)
{
	CEditLidarCommand::PtClick(pt, flag);
	

	if (m_lidarSelection.m_nSelFlag == SELSTAT_NONESEL)
		return;

	if (m_lidarSelection.m_nSelFlag == SELSTAT_POINTSEL)
	{
		if (m_nStep == 0)
		{
			m_nStep = 1;
		}
		
	}
	else if (m_lidarSelection.m_nSelFlag == SELSTAT_DRAGSEL)
	{
		if (m_nStep == 0)
		{
			m_nStep = 1;
			return;
		}
		else if(m_nStep == 1)
		{
			m_nStep = 2;
		}
	}

	m_pEditor->UpdateDrag(ud_ClearDrag);


	{
		if( (m_lidarSelection.m_nSelFlag&SELSTAT_POINTSEL)!=0 )
		{
			CFeature *pFtr = HandleToFtr(m_lidarSelection.keyPt.id);
			if (!pFtr) return;

			CGeometry *pObj = pFtr->GetGeometry();

			if( pObj!=NULL && pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) && 
				m_lidarSelection.keyPt.nPt>=0&&!pObj->IsKindOf(RUNTIME_CLASS(CGeoDemPoint)) )
			{
				

				if( pObj->GetDataPointSum()<=1 )
				{
					CUndoFtrs undo(m_pEditor,Name());
					undo.arrOldHandles.Add(m_lidarSelection.keyPt.id);

					PDOC(m_pEditor)->DeleteObject(m_lidarSelection.keyPt.id);
					
					undo.Commit();
					m_nModifyWay = modifyDelObj;
				}
				else
				{
					m_objID = m_lidarSelection.keyPt.id;
					m_nKeyPt = m_lidarSelection.keyPt.nPt;

					m_pEditor->DeleteObject(m_objID,FALSE);

					m_oldPt = pObj->GetDataPoint(m_nKeyPt);
//					((CGeoMultiPoint*)pObj)->DeletePt(m_nKeyPt);
					CArray<PT_3DEX,PT_3DEX> pts;
					pObj->GetShape(pts);
					pts.RemoveAt(m_nKeyPt);
					pObj->CreateShape(pts.GetData(),pts.GetSize());

					m_pEditor->RestoreObject(m_objID);
					//PDOC(m_pEditor)->UpdateObject(m_lidarSelection.keyPt.id);
					PDOC(m_pEditor)->OnSelectChanged(TRUE);

					CUndoVertex undo(m_pEditor,Name());
					undo.handle = m_objID;
					undo.nPtType = PT_KEYCTRL::typeKey;
					undo.nPtIdx = m_nKeyPt;
					undo.ptOld = m_oldPt;
					undo.Commit();

					m_nModifyWay = modifyDelPt;
				}

				PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
				
				Finish();
				return;
			}
		}
		else if( (m_lidarSelection.m_nSelFlag&SELSTAT_DRAGSEL)!=0 )
		{
			Envelope e;
			e.CreateFromPts(m_lidarSelection.boundPts.GetData(),m_lidarSelection.boundPts.GetSize());
			
			CIDArray ids;
			FindLidarObj(PDOC(m_pEditor),ids);
			int num = ids.GetSize();
			
// 			m_idsOld.RemoveAll();
// 			m_idsNew.RemoveAll();
			CUndoFtrs undo(m_pEditor,Name());
			
			for( int i=0; i<num; i++)
			{
				FTR_HANDLE id = ids[i];
				if (id == 0) continue;

				CGeometry *pObj = HandleToFtr(id)->GetGeometry();
				
				if( pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) &&!pObj->IsKindOf(RUNTIME_CLASS(CGeoDemPoint)) )
				{
					

					CGeoMultiPoint *pPoint = (CGeoMultiPoint*)pObj;
					if( pPoint->IsInEnvelope(e) )
					{
						undo.arrOldHandles.Add(id);

						PDOC(m_pEditor)->DeleteObject(id,FALSE);

					}
					else
					{
						CFeature *pFtr1 = HandleToFtr(id)->Clone();

						CGeoMultiPoint *pPoint1 = (CGeoMultiPoint*)pFtr1->GetGeometry();
						int nDel = pPoint1->DeletePart(e,TRUE);
						if( nDel<=0 )
						{
							delete pFtr1;
						}
						else if( nDel>=pPoint->GetDataPointSum() )
						{
							delete pFtr1;

							undo.arrOldHandles.Add(id);
							PDOC(m_pEditor)->DeleteObject(id,FALSE);
						}
						else
						{
							
							PDOC(m_pEditor)->AddObject(pFtr1,PDOC(m_pEditor)->GetFtrLayerIDOfFtr(id));

							GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(id),pFtr1);
							undo.arrNewHandles.Add(FtrToHandle(pFtr1));

							undo.arrOldHandles.Add(id);							
							PDOC(m_pEditor)->DeleteObject(id,FALSE);
							
						}
					}
				}
			}
			undo.Commit();

			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);

//			if( m_idsOld.GetSize()>0 || m_idsNew.GetSize()>0 )
			{
				m_nModifyWay = modifyDelObj;
				Finish();
				m_nStep = 3;
			}
// 			else
// 			{
// 				Abort();
// 			}
						
			return;
		}
	}
	
	
}


//////////////////////////////////////////////////////////////////////
// CMoveLidarCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMoveLidarCommand,CEditLidarCommand)

CMoveLidarCommand::CMoveLidarCommand()
{
	m_nStep = -1;
	m_bKeepOld = FALSE;
	m_bMouseDefine = TRUE;
	m_lfDeltaX = 0;
	m_lfDeltaY = 0;
	m_lfDeltaZ = 0;
	strcat(m_strRegPath,"\\Move");
}

CMoveLidarCommand::~CMoveLidarCommand()
{
//	m_idsNew.RemoveAll();
//	m_idsOld.RemoveAll();
	CGeometry *pObj;
	for( int i=m_ptrObjs.GetSize()-1; i>=0; i--)
	{
		pObj = (CGeometry*)m_ptrObjs[i];
		delete pObj;
	}
	m_ptrObjs.RemoveAll();
}

CString CMoveLidarCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MOVELIDAR);
}


CCommand* CMoveLidarCommand::Create()
{
	return new CMoveLidarCommand;
}

void CMoveLidarCommand::Start()
{
	if( !m_pEditor )return;
	
	m_ptrObjs.RemoveAll();
	CEditLidarCommand::Start();
}

void CMoveLidarCommand::Abort()
{
	CGeometry *pObj;
	for( int i=m_ptrObjs.GetSize()-1; i>=0; i--)
	{
		pObj = (CGeometry*)m_ptrObjs[i];
		delete pObj;
	}
	m_ptrObjs.RemoveAll();
	
	CEditLidarCommand::Abort();
}


BOOL CMoveLidarCommand::MoveObject(CGeoMultiPoint *pObj, Envelope e, double dx, double dy, double dz, CGeoMultiPoint *&pNewObj)
{
	pNewObj = (CGeoMultiPoint*)pObj->Clone();
//	pNewObj->DeleteAll();

	CArray<PT_3DEX,PT_3DEX> pts, arr;
	pObj->GetShape(arr);
	int num = arr.GetSize();
	for( int i=0; i<num; i++)
	{
		PT_3DEX expt = arr[i];
		if( e.bPtIn(&expt) )
		{
			expt.x += dx; expt.y += dy; expt.z += dz;
			arr[i] = expt;

			pts.Add(expt);
		}
	}

	if (pts.GetSize() > 0)
	{
		pObj->CreateShape(arr.GetData(),arr.GetSize());
	}

	pNewObj->CreateShape(pts.GetData(),pts.GetSize());

	if( pNewObj->GetDataPointSum()<=0 )
	{
		delete pNewObj;
		pNewObj = NULL;
		return FALSE;
	}

	return TRUE;
}

void CMoveLidarCommand::PtClick(PT_3D &pt, int flag)
{
	CEditLidarCommand::PtClick(pt, flag);

	if (m_lidarSelection.m_nSelFlag == SELSTAT_NONESEL)
		return;

	if (m_lidarSelection.m_nSelFlag == SELSTAT_POINTSEL)
	{
		if (m_nStep == 0)
		{
			m_nStep = 1;
			m_ptDragStart = pt;
			m_ptDragEnd = pt;
			return;
		}
		else if(m_nStep == 1||m_nStep == 2)
		{
			m_ptDragEnd = pt;
		}
	}
	else if (m_lidarSelection.m_nSelFlag==SELSTAT_DRAGSEL)
	{
		if (m_nStep == 0)
		{
			m_nStep = 1;
			return;
		}
		else if(m_nStep == 1)
		{
			m_ptDragStart = pt;
			m_ptDragEnd = pt;
			m_nStep = 2;
			return;
		}
		else if(m_nStep == 2)
		{
			m_ptDragEnd = pt;
		}
	}

	m_pEditor->UpdateDrag(ud_ClearDrag);

	int nStep = m_nStep;
	
	CGeometry *pObj;
	CGeoMultiPoint *pNew;
	
	m_ptrObjs.RemoveAll();
	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetVariantDragLine,NULL);
	PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetConstDragLine,NULL);
	PDOC(m_pEditor)->DeselectAll();
	PDOC(m_pEditor)->OnSelectChanged();
	
	double dx = m_ptDragEnd.x-m_ptDragStart.x;
	double dy = m_ptDragEnd.y-m_ptDragStart.y; 
	double dz = m_ptDragEnd.z-m_ptDragStart.z;
	
	if( !m_bMouseDefine )
	{
		dx = m_lfDeltaX; dy = m_lfDeltaY; dz = m_lfDeltaZ;
	}
	
	if( (m_lidarSelection.m_nSelFlag&SELSTAT_POINTSEL)!=0 )
	{
		
		m_objID = m_lidarSelection.keyPt.id;
		CFeature *pFtr = HandleToFtr(m_objID);
		if (!pFtr)  return;
		pObj = pFtr->GetGeometry();
		if( pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint))&&!pObj->IsKindOf(RUNTIME_CLASS(CGeoDemPoint)) )
		{
			PT_3DEX expt;
			expt = pObj->GetDataPoint(m_lidarSelection.keyPt.nPt);
			
			m_oldPt = expt;
			expt.x += dx; expt.y += dy; expt.z += dz;
			
			if( m_bMouseDefine )
				expt.z = m_ptDragEnd.z;
			
			if( m_bKeepOld )
			{
				CFeature *pFtrNew = pFtr->Clone();
				pFtrNew->SetID(OUID());
				CGeometry *pObjNew = pFtrNew->GetGeometry();
				
				pObjNew->CreateShape(&expt,1);
				
				PDOC(m_pEditor)->AddObject(pFtrNew,PDOC(m_pEditor)->GetFtrLayerIDOfFtr(m_objID));

				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objID),pFtrNew);
				
				CUndoFtrs undo(m_pEditor,Name());
				undo.arrNewHandles.Add(FtrToHandle(pFtrNew));
				undo.Commit();
				
				m_nModifyWay = modifyDelObj;
			}
			else
			{
				m_pEditor->DeleteObject(m_objID,FALSE);

				pObj->SetDataPoint(m_lidarSelection.keyPt.nPt,expt);

				m_pEditor->RestoreObject(m_objID);
				//PDOC(m_pEditor)->UpdateObject(m_objID);
				PDOC(m_pEditor)->OnSelectChanged(TRUE);
				
				m_nKeyPt = m_lidarSelection.keyPt.nPt;
				m_newPt = expt;
				
				CUndoVertex undo(m_pEditor,Name());
				undo.handle = m_objID;
				undo.nPtType = PT_KEYCTRL::typeKey;
				undo.nPtIdx = m_nKeyPt;
				undo.ptOld = m_oldPt;
				undo.ptNew = m_newPt;
				undo.Commit();
				
				m_nModifyWay = modifyMovePt;
			}
			Finish();
			m_nStep = 3;
		}
		else
		{
			Abort();
		}
		
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
		
	}
	else
	{
		Envelope e;
		e.CreateFromPts(m_lidarSelection.boundPts.GetData(),m_lidarSelection.boundPts.GetSize());
		
		CIDArray ids;
		FindLidarObj(PDOC(m_pEditor),ids);
		int num = ids.GetSize();
		CUndoFtrs undo(m_pEditor,Name());
		
		for( int i=ids.GetSize()-1; i>=0; i--)
		{
			CFeature *pFtr = HandleToFtr(ids[i]);
			if (!pFtr)  return;
			
			pObj = pFtr->GetGeometry();
			if( !pObj )continue;
			if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
				continue;
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoDemPoint)))
				continue;
			
			CFeature *pFtrNew = pFtr->Clone();
			if (!pFtrNew) continue;
			pFtrNew->SetID(OUID());
			pObj = pFtrNew->GetGeometry();
			
			if( m_bKeepOld )
			{
				if( !MoveObject((CGeoMultiPoint*)pObj,e,dx,dy,dz,pNew) )
				{
					delete pObj;
					continue;
				}
				pFtrNew->SetGeometry(pNew);
				
				PDOC(m_pEditor)->AddObject(pFtrNew,PDOC(m_pEditor)->GetFtrLayerIDOfFtr(ids[i]));

				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(ids[i]),pFtrNew);				
				undo.arrNewHandles.Add(FtrToHandle(pFtrNew));
				
			}
			else
			{
				if( !MoveObject((CGeoMultiPoint*)pObj,e,dx,dy,dz,pNew) )
				{
					delete pObj;
					continue;
				}
				
				delete pNew;
				
			
				PDOC(m_pEditor)->AddObject(pFtrNew,PDOC(m_pEditor)->GetFtrLayerIDOfFtr(ids[i]));

				GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(ids[i]),pFtrNew);
				undo.arrNewHandles.Add(FtrToHandle(pFtrNew));
				
				PDOC(m_pEditor)->DeleteObject(ids[i]);
				undo.arrOldHandles.Add(ids[i]);
				
			}
		}
		
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
		undo.Commit();
		
		{
			m_nModifyWay = modifyDelObj;
			Finish();
			m_nStep = 3;
		}
		
	}
	return;	
	
}

void CMoveLidarCommand::PtMove(PT_3D &pt)
{
	if( m_bMouseDefine && ((m_nStep==2&&m_lidarSelection.m_nSelFlag==SELSTAT_DRAGSEL) || (m_lidarSelection.m_nSelFlag==SELSTAT_POINTSEL)) )
	{
		GrBuffer buf;
		buf.BeginLineString(RGB(255,255,255),1);
		buf.MoveTo(&m_ptDragStart);
		buf.LineTo(&m_ptDragEnd);
		buf.End();
		
		PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetVariantDragLine,(CObject*)&buf);
		m_ptDragEnd = pt;
		m_nStep = 2;
	}
	
	CEditLidarCommand::PtMove(pt);
}

/*
const char * CMoveLidarCommand::GetTipString(int op)
{
	static CString str;
	str.Empty();
	
	const char *base = CCommand::GetTipString(op);
	switch(op) 
	{
	case CMDOP_START:
		{
			CString tstr;
			if( base )str = base;
			if( m_bKeepOld )str += StrFromResID(IDS_CMDTIP_KEEPOLD);
			if( m_bMouseDefine )
			{
				str += StrFromResID(IDS_CMDTIP_MOUSEDEF_MOVE);
			}
			else
			{
				tstr.Format("%s(%.4f,%.4f,%.4f) ",(LPCTSTR)StrFromResID(IDS_CMDTIP_INPUT_MOVE),m_lfDeltaX,m_lfDeltaY,m_lfDeltaZ);
				str += tstr;
			}
			
			if( m_bKeepOld )tstr.Format("[%s(o)",StrFromResID(IDS_CMDPLANE_NOTKEEPOLD));
			else tstr.Format("[%s(o)",StrFromResID(IDS_CMDPLANE_KEEPOLD));
			str += tstr;
			
			if( m_bMouseDefine )tstr.Format("/%s(m)",(LPCTSTR)StrFromResID(IDS_CMDTIP_INPUT_MOVE));
			else tstr.Format("/%s(m)",(LPCTSTR)StrFromResID(IDS_CMDTIP_MOUSEDEF_MOVE));
			str += tstr;
			
			str += _T("]\n");
		}
	case CMDOP_PTCLICKDOWN	: 
		if( IsRunning() && m_idsOld.GetSize()>0 ) 
		{
			str = StrFromResID(IDS_CMDTIP_NEWPLACE);
		}
		break;
	default: return CEditLidarCommand::GetTipString(op);
	}
	
	return str;
}
*/
void CMoveLidarCommand::GetParams(CValueTable &tab)
{
	CEditLidarCommand::GetParams(tab);
	
	_variant_t var;
	var = (bool)(m_bKeepOld);
	tab.AddValue(PF_KEEPOLD,&CVariantEx(var));
	var = (bool)(m_bMouseDefine);
	tab.AddValue(PF_MOUSEDEFINE,&CVariantEx(var));
	var = (double)m_lfDeltaX;
	tab.AddValue(PF_XOFFSET,&CVariantEx(var));
	var = (double)m_lfDeltaY;
	tab.AddValue(PF_YOFFSET,&CVariantEx(var));
	var = (double)m_lfDeltaZ;
	tab.AddValue(PF_ZOFFSET,&CVariantEx(var));
}


void CMoveLidarCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("MoveLidarCommand",StrFromLocalResID(IDS_CMDNAME_MOVELIDAR));

	param->BeginOptionParam(PF_KEEPOLD,StrFromResID(IDS_CMDPLANE_KEEPOLD));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bKeepOld);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bKeepOld);
	param->EndOptionParam();

	param->BeginOptionParam(PF_MOUSEDEFINE,StrFromResID(IDS_CMDTIP_MOUSEDEF_MOVE));
	param->AddOption(StrFromResID(IDS_YES),1,' ',m_bMouseDefine);
	param->AddOption(StrFromResID(IDS_NO),0,' ',!m_bMouseDefine);
	param->EndOptionParam();

	if (!m_bMouseDefine || bForLoad)
	{
		param->AddParam(PF_XOFFSET,m_lfDeltaX,StrFromResID(IDS_CMDPLANE_DX));
		param->AddParam(PF_YOFFSET,m_lfDeltaY,StrFromResID(IDS_CMDPLANE_DY));
		param->AddParam(PF_ZOFFSET,m_lfDeltaZ,StrFromResID(IDS_CMDPLANE_DZ));
	}
}



void CMoveLidarCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_KEEPOLD,var) )
	{
		m_bKeepOld = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_MOUSEDEFINE,var) )
	{
		m_bMouseDefine = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_XOFFSET,var) )
	{
		m_lfDeltaX = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_YOFFSET,var) )
	{
		m_lfDeltaY = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_ZOFFSET,var) )
	{
		m_lfDeltaZ = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	CEditLidarCommand::SetParams(tab);
}

/*
void CMoveLidarCommand::InitOpSettingBar(int type, CUIFPropListEx *pList)
{
	CUIFPropEx *pProp;
	if( type==FCOP_MOVE )
	{
		AfxCallMessage(FCCM_UPDATEROPLIST,0,0);
		return;
	}
	
	if( !pList )return;
	
	CEditLidarCommand::InitOpSettingBar(type,pList);
	AfxCallMessage(FCCM_SETOPTYPE,0,FCOP_MOVE);
	
	pProp = new CUIFPropEx (StrFromResID(IDS_CMDPLANE_KEEPOLD),(LPCTSTR)StrFromResID(IDS_YES), StrFromResID(IDS_CMDDESC_KEEPOLD),1);
	if( pProp )
	{
		pProp->AddOption(StrFromResID(IDS_YES));
		pProp->AddOption(StrFromResID(IDS_NO));
		pProp->AllowEdit(FALSE);
		pList->AddCanHideProperty(pProp);
		
		pProp->SetCurSelOfCombo(m_bKeepOld?0:1);
	}
	
	pProp = new CUIFPropEx (StrFromResID(IDS_CMDTIP_MOUSEDEF_MOVE),(LPCTSTR)StrFromResID(IDS_YES), StrFromResID(IDS_CMDDESC_MOUSEDEF_MOVE),2);
	if( pProp )
	{
		pProp->AddOption(StrFromResID(IDS_YES));
		pProp->AddOption(StrFromResID(IDS_NO));
		pProp->AllowEdit(FALSE);
		pList->AddProperty(pProp);
		
		pProp->SetCurSelOfCombo(m_bMouseDefine?0:1);
	}
	
	pProp = new CUIFPropEx (StrFromResID(IDS_CMDPLANE_DX),m_lfDeltaX, StrFromResID(IDS_CMDPLANE_DX),3);
	if( pProp )
	{
		pList->AddCanHideProperty(pProp,m_bMouseDefine);
	}
	
	pProp = new CUIFPropEx (StrFromResID(IDS_CMDPLANE_DY),m_lfDeltaY, StrFromResID(IDS_CMDPLANE_DY),4);
	if( pProp )
	{
		pList->AddCanHideProperty(pProp,m_bMouseDefine);
	}
	
	pProp = new CUIFPropEx (StrFromResID(IDS_CMDPLANE_DZ),m_lfDeltaZ, StrFromResID(IDS_CMDPLANE_DZ),5);
	if( pProp )
	{
		pList->AddCanHideProperty(pProp,m_bMouseDefine);
	}
}



void CMoveLidarCommand::OnPropertyChanged(CUIFProp *pProp)
{
	CUIFPropEx *pPropEx = (CUIFPropEx*)pProp;
	if( pProp )
	{
		switch( pProp->GetData() )
		{
		case 1:
			m_bKeepOld = pPropEx->GetCurSelOfCombo()==0?TRUE:FALSE; 
			break;
		case 2:
			{
				CUIFPropListEx *pList = NULL;
				AfxCallMessage(FCCM_GETOPPROPLIST,0,(LPARAM)&pList);
				
				if( !pList )break;
				
				BOOL old = m_bMouseDefine;
				m_bMouseDefine = pPropEx->GetCurSelOfCombo()==0; 
				if( old && !m_bMouseDefine )
				{
					int preidx = pList->FindProperty(2);
					pList->ShowPropByDataToIndex(3,preidx<0?pList->GetPropertyCount():(preidx+1),TRUE);
					pList->ShowPropByDataToIndex(4,preidx<0?pList->GetPropertyCount():(preidx+2),TRUE);
					pList->ShowPropByDataToIndex(5,preidx<0?pList->GetPropertyCount():(preidx+3),TRUE);
				}
				else if( !old && m_bMouseDefine )
				{
					pList->HidePropertyByData(3,TRUE);
					pList->HidePropertyByData(4,TRUE);
					pList->HidePropertyByData(5,TRUE);
					//如果是输入偏移改为鼠标定义偏移，那么操作应该重新开始
					if( m_nStep>0 )
					{
						SetSettingsModifyFlag();
						Abort();
					}
				}
			}
			break;
		case 3:
			m_lfDeltaX = pPropEx->GetValue();
			break;
		case 4:
			m_lfDeltaY = pPropEx->GetValue();
			break;
		case 5:
			m_lfDeltaZ = pPropEx->GetValue();
			break;
		}
	}
	SetSettingsModifyFlag();
	
	return;
}





void CMoveLidarCommand::KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar) 
	{
	case 'o':
	case 'O':
		{
			CUIFPropListEx *pList = NULL;
			
			AfxCallMessage(FCCM_GETOPPROPLIST,0,(LPARAM)&pList);
			if( !pList )return;
			
			int nCount = pList->GetPropertyCount();
			for(int i = 0 ; i < nCount; i++)
			{
				CUIFProp* pProp = pList->GetProperty(i);
				if(pProp->IsKindOf(RUNTIME_CLASS(CUIFPropEx)) && pProp->GetData()==1 )
				{
					if( m_bKeepOld )
					{
						((CUIFPropEx*)pProp)->SetCurSelOfCombo(1);
						GOutPut(StrFromResID(IDS_CMDPLANE_NOTKEEPOLD));
					}
					else
					{
						((CUIFPropEx*)pProp)->SetCurSelOfCombo(0);
						GOutPut(StrFromResID(IDS_CMDPLANE_KEEPOLD));
					}
					
					OnPropertyChanged(pProp);
					break;
				}
			}
		}
		break;
	case 'm':
	case 'M':
		{
			CUIFPropListEx *pList = NULL;
			
			AfxCallMessage(FCCM_GETOPPROPLIST,0,(LPARAM)&pList);
			if( !pList )return;
			
			int nCount = pList->GetPropertyCount();
			for(int i = 0 ; i < nCount; i++)
			{
				CUIFProp* pProp = pList->GetProperty(i);
				
				if(pProp->IsKindOf(RUNTIME_CLASS(CUIFPropEx)) && pProp->GetData()==2 )
				{
					if( m_bMouseDefine )
					{
						((CUIFPropEx*)pProp)->SetCurSelOfCombo(1);
						GOutPut(StrFromResID(IDS_CMDTIP_MOUSEDEF_MOVE));
					}
					else
					{
						((CUIFPropEx*)pProp)->SetCurSelOfCombo(0);
						GOutPut(StrFromResID(IDS_CMDTIP_INPUT_MOVE));
					}
					
					OnPropertyChanged(pProp);
					break;
				}
			}
		}
		break;
	default:;
	}
	CEditLidarCommand::KeyDown(nChar,nRepCnt,nFlags);
}
*/


//////////////////////////////////////////////////////////////////////
// CModifyZLidarCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CModifyZLidarCommand,CEditLidarCommand)

CModifyZLidarCommand::CModifyZLidarCommand()
{
	m_nStep = -1;
	m_lfResultZ = 0;
	m_nSelMode = modeNormal;
	strcat(m_strRegPath,"\\ModifyZLidar");
}

CModifyZLidarCommand::~CModifyZLidarCommand()
{
}

CCommand* CModifyZLidarCommand::Create()
{
	return new CModifyZLidarCommand;
}

CString CModifyZLidarCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_MODIFYZ)+" Lidar";
}

void CModifyZLidarCommand::Start()
{
	if( !PDOC(m_pEditor) )return;
	
	m_lfResultZ = 0;
	m_nSelMode = modeNormal;

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

	CEditLidarCommand::Start();
	if( m_nSelMode==modeRefObj )
		m_pEditor->OpenSelector(SELMODE_SINGLE);
	else
	{
		m_pEditor->CloseSelector();
	}
}

void CModifyZLidarCommand::Abort()
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
	CEditLidarCommand::Abort();
}

CProcedure *CModifyZLidarCommand::GetActiveSonProc(int nMsgType)
{
	return NULL;
}

void CModifyZLidarCommand::Finish()
{
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
	CEditLidarCommand::Finish();
}

extern int KickoffSamePoints1(PT_3D *pts, int num, double tolerance);

void CModifyZLidarCommand::PtClick(PT_3D &pt, int flag)
{
	if(m_nSelMode==modeNormal)
	{
		CEditLidarCommand::PtClick(pt, flag);
		
		if (m_lidarSelection.m_nSelFlag == SELSTAT_NONESEL)
			return;
		
		if (m_lidarSelection.m_nSelFlag==SELSTAT_POINTSEL || m_lidarSelection.m_nSelFlag==SELSTAT_DRAGSEL)
		{
			if (m_nStep == 0)
			{
				m_nStep = 1;
				return;
			}
		}
		
		if( m_nStep==1 )
		{
			CGeometry *pObj;
			PDOC(m_pEditor)->DeselectAll();
			PDOC(m_pEditor)->OnSelectChanged();
			if( (m_lidarSelection.m_nSelFlag&SELSTAT_POINTSEL)!=0 )
			{			
				m_objID = m_lidarSelection.keyPt.id;
				CFeature *pFtr = HandleToFtr(m_objID);
				if (!pFtr) return;
				pObj = pFtr->GetGeometry();
				if( pObj && pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
				{
					PT_3DEX expt;
					expt = pObj->GetDataPoint(m_lidarSelection.keyPt.nPt);
					
					m_oldPt = expt;
					expt.z = m_lfResultZ;
					
					PDOC(m_pEditor)->DeleteObject(m_objID,FALSE);
					pObj->SetDataPoint(m_lidarSelection.keyPt.nPt,expt);
					PDOC(m_pEditor)->RestoreObject(m_objID);
					
					m_nKeyPt = m_lidarSelection.keyPt.nPt;
					m_newPt = expt;
					
					CUndoVertex undo(m_pEditor,Name());
					undo.handle = m_objID;
					undo.nPtType = PT_KEYCTRL::typeKey;
					undo.nPtIdx = m_nKeyPt;
					undo.ptOld = m_oldPt;
					undo.ptNew = m_newPt;
					undo.Commit();
					
					m_nModifyWay = modifyMovePt;
					
					Finish();
					m_nStep = 3;
				}
				else
				{
					Abort();
				}
				
				PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
				return;
			}
			else
			{
				Envelope e;
				e.CreateFromPts(m_lidarSelection.boundPts.GetData(),m_lidarSelection.boundPts.GetSize());
				
				CIDArray ids;
				FindLidarObj(PDOC(m_pEditor),ids);
				
				CUndoFtrs undo(m_pEditor,Name());
				GrBuffer buf;
				PT_3DEX pt0,pt1;
				for( int i=ids.GetSize()-1; i>=0; i--)
				{
					CFeature *pFtr = HandleToFtr(ids[i]);
					if (!pFtr) continue;
					pObj = pFtr->GetGeometry();
					if( !pObj )continue;
					if( pObj->GetDataPointSum()<=0 || !pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
						continue;
					
					pt0 = pObj->GetDataPoint(0);
					int num = pObj->GetDataPointSum(), nModify = 0;
					
					CFeature *pFtrNew = pFtr->Clone();
					if( !pFtrNew )continue;
					
					CGeometry *pObjNew = pFtrNew->GetGeometry();
					if( !pObjNew )continue;
					
					CArray<PT_3DEX,PT_3DEX> arrNew;
					pObjNew->GetShape(arrNew);
					
					for( int j=0; j<num; j++)
					{
						pt1 = arrNew[j];
						
						if( e.bPtIn(&pt1) )
						{
							pt1.z = m_lfResultZ;
							arrNew[j] = pt1;
							
							nModify++;
						}
					}
					
					if( nModify>0 )
					{
						pObjNew->CreateShape(arrNew.GetData(),arrNew.GetSize());				
						
						PDOC(m_pEditor)->AddObject(pFtrNew,PDOC(m_pEditor)->GetFtrLayerIDOfFtr(ids[i]));
						
						GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(ids[i]),pFtrNew);
						undo.arrNewHandles.Add(FtrToHandle(pFtrNew));

						undo.arrOldHandles.Add(ids[i]);							
						PDOC(m_pEditor)->DeleteObject(ids[i]);
						
					}
					else
					{
						delete pFtrNew;
					}
				}
				
				PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
				undo.Commit();
				m_nModifyWay = modifyDelObj;
				Finish();
				m_nStep = 2;	
				
			}
			return;
		}
	}
	else if(m_nSelMode==modePolygon)
	{
		if (m_nStep==0)
		{
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 1;
		}
		if (m_nStep==1)
		{
			m_pDrawProc->PtClick(pt,flag);
		}
		if (m_nStep==2)
		{
			//确认处理
			m_pEditor->UpdateDrag(ud_SetConstDrag);
			
			if( !m_pFtr )return;			
			
			CGeoCurve *m_pObj = (CGeoCurve*)m_pFtr->GetGeometry();
			if (!m_pObj) return;			
		
			int polysnum = m_lidarSelection.boundPts.GetSize();				
			PT_3D *polys = m_lidarSelection.boundPts.GetData();
			if( polysnum<3 )
			{
				m_lidarSelection.boundPts.RemoveAll();
				Abort();
				return;
			}
			Envelope e;
			e.CreateFromPts(polys,polysnum);
// 			//转换成客户坐标
// 			e.TransformGrdToClt(pDoc->GetCoordWnd().m_pSearchCS,1);
			int num = PDOC(m_pEditor)->GetDataQuery()->FindObjectInRect(e,NULL);
			if( num<=0 )return ;
			
			const CPFeature *ftr = NULL;
			ftr = PDOC(m_pEditor)->GetDataQuery()->GetFoundHandles(num);		

			CUndoFtrs undo(m_pEditor,Name());
			CGeometry *pObj = NULL;
			GrBuffer buf;
			PT_3D pt1;
			for( int i=0; i<num; i++)
			{
				CFeature *pFtr = ftr[i];
				if (!pFtr) continue;
				pObj = pFtr->GetGeometry();
				if( !pObj )continue;
				if( pObj->GetDataPointSum()<=0 || !pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
					continue;
				
		//		pt0 = pObj->GetDataPoint(0);
				int ptNum = pObj->GetDataPointSum(), nModify = 0;
				
				CFeature *pFtrNew = pFtr->Clone();
				if( !pFtrNew )continue;
				pFtrNew->SetID(OUID());
				CGeometry *pObjNew = pFtrNew->GetGeometry();
				if( !pObjNew )continue;
				
				CArray<PT_3DEX,PT_3DEX> arrNew;
				pObjNew->GetShape(arrNew);
				
				for( int j=0; j<ptNum; j++)
				{
					COPY_3DPT(pt1 , arrNew[j]);
					
					if( GraphAPI::GIsPtInRegion(pt1,polys,polysnum)==2)
					{
						arrNew[j].z = m_lfResultZ;						
						nModify++;
					}
				}
				
				if( nModify>0 )
				{					
					if(pObjNew->CreateShape(arrNew.GetData(),arrNew.GetSize())&&PDOC(m_pEditor)->AddObject(pFtrNew,PDOC(m_pEditor)->GetFtrLayerIDOfFtr(FtrToHandle(pFtr))))
					{
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pFtrNew);
						undo.arrNewHandles.Add(FtrToHandle(pFtrNew));
						if(PDOC(m_pEditor)->DeleteObject(FtrToHandle(pFtr)))
							undo.arrOldHandles.Add(FtrToHandle(pFtr));	
					}	
				}
				else
				{
					delete pFtrNew;
				}
			}
			
			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
			undo.Commit();
			m_nModifyWay = modifyDelObj;
			Finish();
		}	
	}
	else
	{
		if( m_nStep==0 )
		{
			if( !CanGetSelObjs(flag) )return;
			
			int num ;
			m_pEditor->GetSelection()->GetSelectedObjs(num);
			if( num!=1 )return;
			
			FTR_HANDLE  handle = m_pEditor->GetSelection()->GetLastSelectedObj();
			CGeometry *pObj = HandleToFtr(handle)->GetGeometry();
			if( !pObj || pObj->GetDataPointSum()<3 )return;
			CArray<PT_3DEX,PT_3DEX> arrPts;
			PT_3D pt3d;
			pObj->GetShape(arrPts);
			m_lidarSelection.boundPts.RemoveAll();
			for (int i=0;i<arrPts.GetSize();i++)
			{
				COPY_3DPT(pt3d,arrPts[i]);
				m_lidarSelection.boundPts.Add(pt3d);
			}
			int nNum = KickoffSamePoints1(m_lidarSelection.boundPts.GetData(),m_lidarSelection.boundPts.GetSize(),1e-4);
			m_lidarSelection.boundPts.SetSize(nNum);
			m_lidarSelection.boundPts.FreeExtra();

			m_pEditor->CloseSelector();				
			
			GotoState(PROCSTATE_PROCESSING);
			m_nStep = 1;		
		}
		if (m_nStep==1)
		{
			int polysnum = m_lidarSelection.boundPts.GetSize();				
			PT_3D *polys = m_lidarSelection.boundPts.GetData();
			if( polysnum<3 )
			{
				m_lidarSelection.boundPts.RemoveAll();
				Abort();
				return;
			}
			Envelope e;
			e.CreateFromPts(polys,polysnum);
			// 			//转换成客户坐标
			// 			e.TransformGrdToClt(pDoc->GetCoordWnd().m_pSearchCS,1);
			int num = PDOC(m_pEditor)->GetDataQuery()->FindObjectInRect(e,NULL);
			if( num<=0 )return ;
			
			const CPFeature *ftr = NULL;
			ftr = PDOC(m_pEditor)->GetDataQuery()->GetFoundHandles(num);		
			
			CUndoFtrs undo(m_pEditor,Name());
			CGeometry *pObj = NULL;
			GrBuffer buf;
			PT_3D pt1;
			for( int i=0; i<num; i++)
			{
				CFeature *pFtr = ftr[i];
				if (!pFtr) continue;
				pObj = pFtr->GetGeometry();
				if( !pObj )continue;
				if( pObj->GetDataPointSum()<=0 || !pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
					continue;
				
				//		pt0 = pObj->GetDataPoint(0);
				int ptNum = pObj->GetDataPointSum(), nModify = 0;
				
				CFeature *pFtrNew = pFtr->Clone();
				if( !pFtrNew )continue;
				pFtrNew->SetID(OUID());
				CGeometry *pObjNew = pFtrNew->GetGeometry();
				if( !pObjNew )continue;
				
				CArray<PT_3DEX,PT_3DEX> arrNew;
				pObjNew->GetShape(arrNew);
				
				for( int j=0; j<ptNum; j++)
				{
					COPY_3DPT(pt1 , arrNew[j]);
					
					if( GraphAPI::GIsPtInRegion(pt1,polys,polysnum)==2)
					{
						arrNew[j].z = m_lfResultZ;						
						nModify++;
					}
				}
				
				if( nModify>0 )
				{					
					if(pObjNew->CreateShape(arrNew.GetData(),arrNew.GetSize())&&PDOC(m_pEditor)->AddObject(pFtrNew,PDOC(m_pEditor)->GetFtrLayerIDOfFtr(FtrToHandle(pFtr))))
					{
						GETXDS(m_pEditor)->CopyXAttributes(pFtr,pFtrNew);
						undo.arrNewHandles.Add(FtrToHandle(pFtrNew));
						if(PDOC(m_pEditor)->DeleteObject(FtrToHandle(pFtr)))
							undo.arrOldHandles.Add(FtrToHandle(pFtr));	
					}	
				}
				else
				{
					delete pFtrNew;
				}
			}
			
			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
			undo.Commit();
			m_nModifyWay = modifyDelObj;
			m_pEditor->DeselectAll();
			Finish();
		}
	}
}

void CModifyZLidarCommand::PtMove(PT_3D &pt)
{
	if(m_nSelMode==modePolygon)
	{
		if (m_nStep==1)
		{
			m_pDrawProc->PtMove(pt);
		}
		CCommand::PtMove(pt);
		return;
	}
	CEditLidarCommand::PtMove(pt);
}

void CModifyZLidarCommand::PtReset(PT_3D &pt)
{
	if(m_nSelMode==modePolygon)
	{
		if( m_nStep==1 )
		{
			m_pDrawProc->PtReset(pt);
			CArray<PT_3DEX,PT_3DEX> arrPts;
			PT_3D pt3d;
			m_pDrawProc->m_pGeoCurve->GetShape(arrPts);
			m_lidarSelection.boundPts.RemoveAll();
			for (int i=0;i<arrPts.GetSize();i++)
			{
				COPY_3DPT(pt3d,arrPts[i]);
				m_lidarSelection.boundPts.Add(pt3d);
			}
			int nNum = KickoffSamePoints1(m_lidarSelection.boundPts.GetData(),m_lidarSelection.boundPts.GetSize(),1e-4);
			m_lidarSelection.boundPts.SetSize(nNum);
			m_lidarSelection.boundPts.FreeExtra();
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
		return;
	}
	else
		CEditLidarCommand::PtReset(pt);
}

void CModifyZLidarCommand::GetParams(CValueTable &tab)
{
	CCommand::GetParams(tab);
	
	_variant_t var;
	var = (double)m_lfResultZ;
	tab.AddValue(PF_ZLIDAR,&CVariantEx(var));
	var = (long)m_nSelMode;
	tab.AddValue(PF_ZLIDARSELMODE,&CVariantEx(var));
}


void CModifyZLidarCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("ModifyZLidar",StrFromLocalResID(IDS_CMDNAME_MOVELIDAR));
	
	param->AddParam(PF_ZLIDAR,m_lfResultZ,StrFromResID(IDS_CMDDESC_Z));

	param->BeginOptionParam(PF_ZLIDARSELMODE,StrFromResID(IDS_CMDPLANE_SELMODE));
	param->AddOption(StrFromResID(IDS_NORMALSEL),0,' ',m_nSelMode==modeNormal);
	param->AddOption(StrFromResID(IDS_POLYSEL),1,' ',m_nSelMode==modePolygon);
	param->AddOption(StrFromResID(IDS_REFOBJECT),2,' ',m_nSelMode==modeRefObj);
	param->EndOptionParam();
	
}



void CModifyZLidarCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_ZLIDAR,var) )
	{
		m_lfResultZ = (double)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if (tab.GetValue(0,PF_ZLIDARSELMODE,var))
	{
		m_nSelMode = (long)(_variant_t)*var;
		SetSettingsModifyFlag();
		if(!bInit)
			Abort();
	}
	
	CCommand::SetParams(tab);
}

BOOL CModifyZLidarCommand::CheckObjForContour(CGeometry *pObj)
{
	if( !pObj )return FALSE;
	
	PT_3DEX pt0,pt1;
	int num = pObj->GetDataPointSum();
	pt0 = pObj->GetDataPoint(0);
	for( int j=1; j<num; j++)
	{
		pt1 = pObj->GetDataPoint(j);
		if( fabs(pt1.z-pt0.z)>=1e-4 )break;
	}
	
	if( j<num )return FALSE;
	return TRUE;
}



//////////////////////////////////////////////////////////////////////
// CInterpolateLidarCommand Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CInterpolateLidarCommand,CCommand)

CInterpolateLidarCommand::CInterpolateLidarCommand()
{
//	m_bAutoClosed = TRUE;
	m_nStep = -1;
	m_nMode = modeAddPt;
	m_fGridSize = 5.0;
	m_pDrawProc = NULL;
	m_pFtr = NULL;
	
	strcat(m_strRegPath,"\\Interpolate");
}

CInterpolateLidarCommand::~CInterpolateLidarCommand()
{
	if( m_pDrawProc )delete m_pDrawProc;
/*	if(m_pFtr) delete m_pFtr;*/
}

CString CInterpolateLidarCommand::Name()
{ 
	return StrFromResID(IDS_CMDNAME_INTERPOLATELIDAR);
}

void CInterpolateLidarCommand::GetParams(CValueTable &tab)
{
	CCommand::GetParams(tab);
	
	_variant_t var;
	var = (bool)(m_nMode);
	tab.AddValue(PF_MODE,&CVariantEx(var));
	var = (float)m_fGridSize;
	tab.AddValue(PF_GRIDSIZE,&CVariantEx(var));
}


void CInterpolateLidarCommand::FillShowParams(CUIParam* param, BOOL bForLoad )
{
	param->SetOwnerID("InterpolateLidar",StrFromLocalResID(IDS_CMDNAME_INTERPOLATELIDAR));
	
	param->BeginOptionParam(PF_MODE,StrFromResID(IDS_CMDPLANE_WAY));
	param->AddOption(StrFromResID(IDS_CMDPLANE_ADDPT),0,' ',!m_nMode);
	param->AddOption(StrFromResID(IDS_CMDPLANE_MODIFYPT),1,' ',m_nMode);
	param->EndOptionParam();

	if (!m_nMode)
	{
		param->AddParam(PF_GRIDSIZE,m_fGridSize,StrFromResID(IDS_CMDPLANE_GRIDSIZE));
	}
	
	
}



void CInterpolateLidarCommand::SetParams(CValueTable& tab,BOOL bInit)
{
	const CVariantEx *var;
	
	if( tab.GetValue(0,PF_MODE,var) )
	{
		m_nMode = (bool)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	if( tab.GetValue(0,PF_GRIDSIZE,var) )
	{
		m_fGridSize = (float)(_variant_t)*var;
		SetSettingsModifyFlag();
	}
	
	CCommand::SetParams(tab);
}


void CInterpolateLidarCommand::Start()
{
	m_nStep = 0;
	m_nMode = modeAddPt;
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

void CInterpolateLidarCommand::Abort()
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


void CInterpolateLidarCommand::Finish()
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

int CInterpolateLidarCommand::GetState()
{
	return GetStateWithSonProcedure(m_pDrawProc);
}

int KickoffSamePoints1(PT_3D *pts, int num, double tolerance)
{
	int i, pos = 0;
	PT_3D *pt = pts+1;
	if( num<=1 )return num;
	for( i=1; i<num; i++,pt++)
	{
		if( fabs(pts[pos].x-pt->x)<tolerance && 
			fabs(pts[pos].y-pt->y)<tolerance  )
			continue;
		if( i!=(pos+1) )pts[pos+1] = *pt;
		pos++;
	}
	return (pos+1);
}


CProcedure *CInterpolateLidarCommand::GetActiveSonProc(int nMsgType)
{
	if( nMsgType==msgPtClick||nMsgType==msgPtMove||nMsgType==msgPtReset )
		return NULL;
	
	return m_pDrawProc;
}

void CInterpolateLidarCommand::PtClick(PT_3D &pt, int flag)
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

		polysnum = KickoffSamePoints1(polys,polysnum,1e-4);

		if( polysnum<3 )
		{
			if( polys )delete[] polys;
			Abort();
			return;
		}

		//构建 TIN
		CMYTinObj tin;
		CArray<MYPT_3D,MYPT_3D> arrAll;
		MYPT_3D geopt;
		MYPT_3D tri[4];
		for( i=0; i<polysnum; i++)
		{
			geopt.x = polys[i].x; geopt.y = polys[i].y; geopt.z = polys[i].z; 
			geopt.type = 1;
			geopt.id = 1;
			arrAll.Add(geopt);
		}

		MyTriangle::createTIN(arrAll.GetData(),arrAll.GetSize(),&tin.tins,&tin.nTIN,&tin.pts,&tin.npt);

		//生成格网点
		if( tin.nTIN>0 )
		{
			//1. 获得外包
			Envelope e;
			e.CreateFromPts(polys,polysnum);
			
			double w = e.m_xh-e.m_xl, h = e.m_yh-e.m_yl;

			// 加点模式
			if( m_nMode==modeAddPt )
			{
				//2. 计算格网遍历区域范围
				int nx = 0, ny = 0;

				if( m_fGridSize>1e-4 )nx = ceil(w/m_fGridSize);
				if( m_fGridSize>1e-4 )ny = ceil(h/m_fGridSize);

				if( nx*ny>1000000 ){ nx = 0; ny = 0; }

				if( nx==0 || ny==0 )
				{
					if( polys )delete[] polys;
					Abort();
					return;
				}

				//3. 遍历所有格网点
				PT_3DEX expt;
				expt.pencode = penLine;

				PT_3D tri3ds[3];
				double tx, ty, tz;

				CArray<CFeature*,CFeature*> arrObjs;
				CFeature *pFtr = NULL;

				CFtrLayer *pFtrLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayer((LPCTSTR)"Interpolate");

				for( tx = m_fGridSize*floor(e.m_xl/m_fGridSize); tx<e.m_xh; tx += m_fGridSize )
				{
					// 排除区域外的点
					if( tx<=e.m_xl || tx>=e.m_xh )continue;

					for( ty = m_fGridSize*floor(e.m_yl/m_fGridSize); ty<e.m_yh; ty += m_fGridSize )
					{
						// 排除区域外的点
						if( ty<=e.m_yl || ty>=e.m_yh )continue;

						expt.x = tx; expt.y = ty;
						if( GraphAPI::GIsPtInRegion((PT_3D)expt,polys,polysnum)<0 )continue;

						// 检索它落在哪个三角形中
						for( int j=0; j<tin.nTIN; j++)
						{
							if( !tin.GetTriangle( j,tri ) )
								continue;

							COPY_3DPT(tri3ds[0], tri[0]);
							COPY_3DPT(tri3ds[1], tri[1]);
							COPY_3DPT(tri3ds[2], tri[2]);

							if( GraphAPI::GIsPtInRegion((PT_3D)expt,tri3ds,3)<0 )continue;

							//计算格网点的高程
							if( tin.GetZFromTriangle( tri,tx,ty,tz ) )
							{
								expt.z = tz;

								if (!pFtrLayer)
								{
									pFtrLayer = PDOC(m_pEditor)->GetDlgDataSource()->CreateFtrLayer((LPCTSTR)"Interpolate");
									if (!pFtrLayer)
									{
										if( polys )delete[] polys;
										Abort();
										return;
										
									}
									PDOC(m_pEditor)->AddFtrLayer(pFtrLayer);
								}

								//生成多点对象
								if( pFtr==NULL || pFtr->GetGeometry()->GetDataPointSum()>1000 )
								{
									
									//创建 Geometry 对象
									pFtr = pFtrLayer->CreateDefaultFeature(PDOC(m_pEditor)->GetDlgDataSource()->GetScale(),CLS_GEOMULTIPOINT);
									if( !pFtr )continue;

									pFtr->GetGeometry()->SetColor(RGB(255,255,255));

									arrObjs.Add(pFtr);
								}
								
								CArray<PT_3DEX,PT_3DEX> pts;
								pFtr->GetGeometry()->GetShape(pts);
								pts.Add(expt);
								pFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());

								
							}
						}
					}
				}

				CUndoFtrs undo(m_pEditor,Name());
				if( arrObjs.GetSize()>0 )
				{
					for( i=0; i<arrObjs.GetSize(); i++)
					{

						PDOC(m_pEditor)->AddObject(arrObjs.GetAt(i),pFtrLayer->GetID());

						undo.arrNewHandles.Add(FtrToHandle(arrObjs.GetAt(i)));
						
					}
				}
				undo.Commit();
			}
			//改点模式
			else
			{
				//2. 得到所有相交对象
				Envelope rect(e.m_xl,e.m_xh,e.m_yl,e.m_yh,e.m_zl,e.m_zh);
				rect.TransformGrdToClt(m_pEditor->GetCoordWnd().m_pSearchCS,1);
				int num = PDOC(m_pEditor)->GetDataQuery()->FindObjectInRect(rect,m_pEditor->GetCoordWnd().m_pSearchCS);
				if( num<=0 )
				{
					if( polys )delete[] polys;
					Abort();
					return;
				}
				
				CArray<FTR_HANDLE,FTR_HANDLE> ids;
				const CPFeature *ppFtr = NULL;
				ppFtr = PDOC(m_pEditor)->GetDataQuery()->GetFoundHandles(num);
				for (int i=0; i<num; i++,ppFtr++)
				{
					ids.Add(FtrToHandle(*ppFtr));
				}

				//3. 遍历这些对象
				PT_3DEX expt;
				expt.pencode = penLine;

				PT_3D tri3ds[3];
				double tx, ty, tz;

				CArray<CFeature*,CFeature*> arrObjs;
				CFeature *pFtr = NULL;
				CUndoFtrs undo(m_pEditor,Name());

				for( int j=0; j<num; j++ )
				{
					pFtr = HandleToFtr(ids[j]);
					if( pFtr!=NULL && 
						( pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) ||
						pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)) ) )
					{
						pFtr = pFtr->Clone();
						if( !pFtr )continue;

						pFtr->SetID(OUID());
						
						CArray<PT_3DEX,PT_3DEX> arr;
						pFtr->GetGeometry()->GetShape(arr);

						int npt = arr.GetSize(), nModify = 0;
						for( int k=0; k<npt; k++)
						{
							expt = arr[k];

							// 排除区域外的点
							if( !e.bPtIn(&expt) )continue;
							if( GraphAPI::GIsPtInRegion((PT_3D)expt,polys,polysnum)<0 )continue;

							// 检索它落在哪个三角形中
							for( int n=0; n<tin.nTIN; n++)
							{
								if( !tin.GetTriangle( n,tri ) )
									continue;
								
								COPY_3DPT(tri3ds[0], tri[0]);
								COPY_3DPT(tri3ds[1], tri[1]);
								COPY_3DPT(tri3ds[2], tri[2]);
								
								if( GraphAPI::GIsPtInRegion((PT_3D)expt,tri3ds,3)<0 )continue;
								
								//计算格网点的高程
								if( tin.GetZFromTriangle( tri,expt.x,expt.y,expt.z ) )
								{
									arr[k] = expt;
									nModify++;
								}
							}
						}

						if( nModify>0 )
						{
							pFtr->GetGeometry()->CreateShape(arr.GetData(),arr.GetSize());
							
							PDOC(m_pEditor)->AddObject(pFtr, PDOC(m_pEditor)->GetFtrLayerIDOfFtr(ids[j]));

							GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(ids[j]),pFtr);
							undo.arrNewHandles.Add(FtrToHandle(pFtr));

							undo.arrOldHandles.Add(ids[j]);							
							PDOC(m_pEditor)->DeleteObject(ids[j]);
						}
					}
				}
				undo.Commit();
			}

			if( polys )delete[] polys;
			
			Finish();
			return;
		}
		
		if( polys )delete[] polys;
		
		Abort();
		return;

	}
	CCommand::PtClick(pt, flag);
}


void CInterpolateLidarCommand::PtMove(PT_3D &pt)
{
	if( m_nStep==0 || m_nStep==1 )
	{
		m_pDrawProc->m_bClosed = TRUE;
		m_pDrawProc->PtMove(pt);
	}
}

void CInterpolateLidarCommand::PtReset(PT_3D &pt)
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
//	Abort();

//	if( m_nStep==1 )
//	{
//		if( !m_pFtr )return;
// 		if( m_bDrag )
// 		{
// 			if(  && m_nCurPenCode!=penStream && m_pObj->GetPtSum()>=4 )
// 				m_pObj->DeletePt(m_pObj->GetPtSum()-2);
// 			else
// 				m_pObj->DeletePt(m_pObj->GetPtSum()-1);
// 		}
		
//		PutStreamPts();
		
// 		if( m_nCurPenCode==penStream && m_pObj->GetPtSum()>=3 )
// 		{
// 			PT_3DEX t;
// 			m_pObj->GetPt(0,&t);
// 			m_pObj->AddPt(&t);
// 		}
		
// 		if( m_pFtr->GetGeometry()->GetDataPointSum()==3 )
// 		{
// 			CArray<PT_3DEX,PT_3DEX> pts;
// 			m_pFtr->GetGeometry()->GetShape(pts);
// 			pts.RemoveAt(2);
// 			m_pFtr->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize());
// //			((CGeoMultiPoint*)m_pFtr->GetGeometry())->DeletePt(2);
// 		}
// 		
// 		if( m_pFtr->GetGeometry()->GetDataPointSum()<=3 )
// 			Abort();
// 		else
		
//		{
//			PDOC(m_pEditor)->SetCurDrawingObj(DrawingInfo());
//			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Detach_Accubox);
			
//			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetConstDragLine,NULL);
//			PDOC(m_pEditor)->UpdateAllViews(NULL,hc_SetVariantDragLine,NULL);
//			m_nStep = 2;
//		}
		
//		CCommand::PtReset(pt);
//	}
// 	else 
// 	{
// 		Abort();
// 	}
}
/*

void CInterpolateLidarCommand::KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( IsFinished() )return;
	if( nChar>='A' && nChar<='Z' )
		nChar = nChar-'A'+'a';
	
	switch( nChar )
	{
	case 'c':
		{
			CUIFPropListEx *pList = NULL;
			
			AfxCallMessage(FCCM_GETOPPROPLIST,0,(LPARAM)&pList);
			if( !pList )return;
			
			int nCount = pList->GetPropertyCount();
			for(int i = 0 ; i < nCount; i++)
			{
				CUIFProp* pProp = pList->GetProperty(i);
				if(pProp->IsKindOf(RUNTIME_CLASS(CUIFPropEx)) && pProp->GetData()==1 )
				{
					if( m_nMode==modeAddPt )
					{
						((CUIFPropEx*)pProp)->SetCurSelOfCombo(1);
					}
					else
					{
						((CUIFPropEx*)pProp)->SetCurSelOfCombo(0);
					}
					
					OnPropertyChanged(pProp);
					return;
				}
			}
		}
		return;
	default:;
	}
	
	if( m_pObj )
		CDrawLinesCommand::KeyDown(nChar,nRepCnt,nFlags);
	else
		CDrawCommand::KeyDown(nChar,nRepCnt,nFlags);
}

void CInterpolateLidarCommand::Back()
{
	if( m_nStep==0||m_nStep==1 );
//		CDrawLinesCommand::Back();
}


void CInterpolateLidarCommand::UnDo()
{
	//del the new
	for( int i=m_idsNew.GetSize()-1; i>=0; i--)
	{
		PDOC(m_pEditor)->DeleteObject(m_idsNew[i]);
	}
	
	//add the old
	for( i=m_idsOld.GetSize()-1; i>=0; i--)
	{
		PDOC(m_pEditor)->RestoreObject(m_idsOld[i]);
	}
	
	if( PDOC(m_pEditor) )PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
	
	CCommand::UnDo();
}

void CInterpolateLidarCommand::ReDo()
{
	//del the old
	for( int i=m_idsOld.GetSize()-1; i>=0; i--)
	{
		PDOC(m_pEditor)->DeleteObject(m_idsOld[i]);
	}
	
	//add the new
	for( i=m_idsNew.GetSize()-1; i>=0; i--)
	{
		PDOC(m_pEditor)->RestoreObject(m_idsNew[i]);
	}
	
	if( PDOC(m_pEditor) )PDOC(m_pEditor)->UpdateAllViews(NULL,hc_Refresh,0);
	
	CCommand::UnDo();
}


void CInterpolateLidarCommand::UpdateSettings(BOOL bSave)
{
	if( bSave )
	{
		AfxGetApp()->WriteProfileInt(m_strRegPath,"Mode", m_nMode);
		WriteProfileDouble(m_strRegPath,"GridSize",m_fGridSize);
	}
	else
	{
		m_nMode = AfxGetApp()->GetProfileInt(m_strRegPath,"Mode",modeAddPt);
		m_fGridSize = GetProfileInt(m_strRegPath,"GridSize",5.0);
	}
	
	CDrawLinesCommand::UpdateSettings(bSave);
}
*/

