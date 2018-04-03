// EBWorker.cpp: implementation of the CWorker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "EditBase.h"
#include "Worker.h"
#include "matrix.h"
#include "GeoPoint.h"
#include "GeoSurface.h"
#include "GeoCurve.h"
#include "Functions_temp.h"
#include "resource.h "


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWorker::CWorker()
{
	m_nLastCmdCreateID = 0;
	m_nCurCmdCreateID = 0;
	m_bDisableCmdInput = FALSE;

	m_bCmdProcessing = FALSE;
}

CWorker::~CWorker()
{
	CloseAll();
}

void CWorker::CloseAll()
{
	CancelCurrentCommand();	

	int i;
	for( i=m_arrWaitCmdObj.GetSize()-1; i>=0; i--)
	{
		((CCommand*)m_arrWaitCmdObj[i])->Abort();
		delete (CCommand*)m_arrWaitCmdObj[i];
	}
	
	m_arrWaitCmdObj.RemoveAll();
 	
	m_arrCmdReg.RemoveAll();

	CEditor::CloseAll();

}

BOOL CWorker::RegisterCommand(int id, LPPROC_CMDCREATE lpProcCreate)
{
	if( !lpProcCreate )return FALSE;
	
	for (int i=0;i<m_arrCmdReg.GetSize();i++)
	{
		if (id==m_arrCmdReg[i].id)
		{
			return FALSE;
		}
	}
	CMDREG item;
	
	item.id = id;
	item.lpProcCreate = lpProcCreate;
	
	memset(item.strAccel,0,sizeof(item.strAccel));
	CCommand *pCmd = (*lpProcCreate)();
	if( pCmd )
	{
		_tcsncpy(item.strAccel, pCmd->AccelStr(), sizeof(item.strAccel) - 1);
		_tcsncpy(item.strName, pCmd->Name(), sizeof(item.strName) - 1);
		delete pCmd;
	}	
	
	m_arrCmdReg.Add(item);
	
	return TRUE;
}


BOOL CWorker::RegisterSelectStateCommand(LPPROC_CMDCREATE lpProc)
{
	if( !lpProc )return FALSE;
	
	m_arrCreateProc.Add(lpProc);
	
	return TRUE;
}


const CMDREG *CWorker::GetCmdRegs(int &num)
{
	num = m_arrCmdReg.GetSize();
	return m_arrCmdReg.GetData();
}

CCommand* CWorker::CreateCommand(int id)
{
	CMDREG item;
	for( int i=m_arrCmdReg.GetSize()-1; i>=0; i--)
	{
		item = m_arrCmdReg[i];
		if( item.id==id )
		{
			m_nLastCmdCreateID = m_nCurCmdCreateID;
			m_nCurCmdCreateID  = id;
			return (*item.lpProcCreate)();
		}
	}
	return NULL;
}

BOOL CWorker::StartCommand(int id)
{
// 	if( m_bCmdProcessing )
// 		return NULL;

	if( id==0 )
	{
		if( m_nSelectMode==SELMODE_POLYGON )
			StartPolygonSelect();
		else if( m_nSelectMode==SELMODE_RECT )
			StartRectSelect();
		else
			StartDefaultSelect();
		return TRUE;
	}	

	CCommand *pCmd = CreateCommand(id);
	if( !pCmd )return FALSE;

	m_arrWaitCmdObj.Add(pCmd);

	pCmd->Init(this);
	OnStartCommand(pCmd);

	BOOL bOldState = m_bCmdProcessing;
	m_bCmdProcessing = TRUE;
	pCmd->Start();
	m_bCmdProcessing = bOldState;

	return TRUE;
}


void CWorker::StartDefaultSelect()
{
	if( m_bCmdProcessing )
		return;

	CancelCurrentCommand();
	OpenSelector();
	SetCursorType(CURSOR_NORMAL);
	m_nCurCmdCreateID = 0;
	UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)StrFromLocalResID(IDS_STRING_SELECT));
}


void CWorker::StartPolygonSelect()
{
	if( m_bCmdProcessing )
		return;
	CancelCurrentCommand();
	OpenSelector(SELMODE_POLYGON);
	SetCursorType(CURSOR_DRAW);
	m_nCurCmdCreateID = 0;
	UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)StrFromLocalResID(IDS_STRING_SELECT));
}


void CWorker::StartRectSelect()
{
	if( m_bCmdProcessing )
		return;
	CancelCurrentCommand();
	OpenSelector(SELMODE_RECT);
	SetCursorType(CURSOR_NORMAL);
	m_nCurCmdCreateID = 0;
	UIRequest(UIREQ_TYPE_OUTPUTMSG, (LONG_PTR)(LPCTSTR)StrFromLocalResID(IDS_STRING_SELECT));
}


void CWorker::SetCurCmdParams(CValueTable& tab)
{
	CCommand *pCmd = GetCurrentCommand();
	if( pCmd )
	{
		pCmd->SetParams(tab);
		
		TryFinishCurProcedure();
	}
}


BOOL CWorker::SendCmdKeyin(LPCTSTR text)
{
	CCommand *pCmd = GetCurrentCommand();
	if( pCmd )
	{
		BOOL bRet = pCmd->Keyin(text);
		
		TryFinishCurProcedure();

		return bRet;
	}

	return FALSE;
}

void CWorker::TestStateCommand(PT_3D pt, int state)
{
	int num;
	m_selection.GetSelectedObjs(num);
	if( num<=0 )return;
	
	LPPROC_CMDCREATE lpProc;
	for( int i=0; i<m_arrCreateProc.GetSize(); i++)
	{
		lpProc = m_arrCreateProc.GetAt(i);
		if( lpProc )
		{
			CActiveStateCommand *pCmd = (CActiveStateCommand*)(*lpProc)();
			if( pCmd )
			{
				pCmd->Init(this);
				if( pCmd->HitTest(pt,state) )
				{
					for (int j=0;j<m_arrCmdReg.GetSize();j++)
					{
						if (lpProc == m_arrCmdReg[j].lpProcCreate)
						{
							m_nLastCmdCreateID = m_nCurCmdCreateID;
							m_nCurCmdCreateID  = m_arrCmdReg[j].id;
							break;
						}
					}					

					m_arrWaitCmdObj.Add(pCmd);
					OnStartCommand(pCmd);
					pCmd->Start();
					break;
				}
				else
					delete pCmd;				
			}
		}
	}
}


CCommand * CWorker::GetCurrentCommand()
{
	if( m_arrWaitCmdObj.GetSize()>0 )
		return (CCommand*)m_arrWaitCmdObj.GetAt(m_arrWaitCmdObj.GetSize()-1);
	return NULL;
}


CProcedure* CWorker::GetAllLevelProcedures(int nMsgType, CPtrArray* pArr)
{
	CProcedure *pProc = GetCurrentCommand();
	CProcedure *pLast = NULL;
	while( pProc )
	{
		if( pArr )pArr->Add(pProc);

		pLast = pProc;
		pProc = pProc->GetActiveSonProc(nMsgType);
	}

	return pLast;
}

void CWorker::TryFinishCurProcedure()
{
	if( m_bCmdProcessing )
		return;
	
	CValueTable tab;
	CCommand *pCurCommand = GetCurrentCommand();
	if (pCurCommand)
	{
		tab.BeginAddValueItem();
		pCurCommand->GetParams(tab);
		tab.EndAddValueItem();
	}
	//遍历所有子过程，如果子过程已结束，就通知父过程
	CPtrArray arr;
	GetAllLevelProcedures(CProcedure::msgEnd,&arr);

	for( int i=arr.GetSize()-1; i>=0; i--)
	{
		CProcedure *pCurPro = (CProcedure*)arr.GetAt(i);
		if( IsProcOver(pCurPro) && i>0 )
		{
			CProcedure *pParentPro = (CProcedure*)arr.GetAt(i-1);
			if( !IsProcOver(pParentPro) )
			{
				m_bCmdProcessing = TRUE;
				pParentPro->OnSonEnd(pCurPro);
				m_bCmdProcessing = FALSE;
			}
		}
		else
		{
			break;
		}
	}

	int exitcode = CMPEC_NULL;
	while(1) 
	{
		CCommand *pCurCmd = GetCurrentCommand();
		if( !pCurCmd )break;
		if( !IsProcOver(pCurCmd) )
			return;
		
		exitcode = pCurCmd->GetExitCode();

		OnEndCommand(pCurCmd);
		m_arrWaitCmdObj.RemoveAt(m_arrWaitCmdObj.GetSize()-1);
		delete pCurCmd;
	}
	
	if( m_arrWaitCmdObj.GetSize()>0 )
	{
	}
	// 重启上一个命令
	else
	{
		if( exitcode==CMPEC_RESTART )
		{
			StartCommand(m_nCurCmdCreateID);
		}
		else if( exitcode==CMPEC_STARTOLD )
		{
			if( m_nLastCmdCreateID==m_nCurCmdCreateID )
				StartDefaultSelect();
			else
				StartCommand(m_nLastCmdCreateID);
		}
		else if ( exitcode==CMPEC_RESTARTWITHPARAMS )
		{
			if (StartCommand(m_nCurCmdCreateID))
			{
				CCommand *pCmd = GetCurrentCommand();
				if (pCmd)
				{
					pCmd->SetParams(tab,TRUE);
				}
			}
		}
		else
		{
			if( m_nSelectMode==SELMODE_RECT )
				StartRectSelect();
			else if( m_nSelectMode==SELMODE_POLYGON )
				StartPolygonSelect();
			else
				StartDefaultSelect();
		}
	}
	
	return;
}


int CWorker::GetCurrentCommandId()
{
	return m_nCurCmdCreateID;
}

void CWorker::CancelCurrentCommand()
{
	if( m_bCmdProcessing )
		return;
	
	CCommand *pCurCmd;
	while( m_arrWaitCmdObj.GetSize()>0 &&
 		(pCurCmd=(CCommand*)m_arrWaitCmdObj.GetAt(0)) )
	{
		pCurCmd->Abort();
		OnEndCommand(pCurCmd);
		m_arrWaitCmdObj.RemoveAt(0);
		delete pCurCmd;
	}

	if( m_bSelectorOpen && m_nSelectMode==SELMODE_POLYGON )
	{
		m_arrSelDragPts.RemoveAll();
	}
	
	return;
}



void CWorker::OnStartCommand(CCommand* pCmd)
{
}


void CWorker::OnEndCommand(CCommand* pCmd)
{
}

void CWorker::LButtonDblClk(PT_3D pt, int flag)
{
	if( m_bCmdProcessing )  return;

	if( !(m_bSelectorOpen&&m_bSelectDrag) )
	{
		CCommand *pCurCmd = GetCurrentCommand();
		if( !pCurCmd )
		{
			m_bCmdProcessing = TRUE;
			TestStateCommand(pt,CActiveStateCommand::stateDblClick);
			m_bCmdProcessing = FALSE;
		}
		//遍历所有子过程，看看是否子过程也需要响应用户操作
		CProcedure *pCurProc = GetAllLevelProcedures(CProcedure::msgPtDblClick,NULL);
		if( pCurProc )
		{
			pCurProc->PtDblClick(pt,0);
			TryFinishCurProcedure();
		}
	}
}

BOOL CWorker::IsDefaultSelect()
{
	if (m_bSelectorOpen && GetCurrentCommand()==NULL)
	{
		return TRUE;
	}
	return FALSE;
}

//面的母线是否通过的区域
static BOOL IsSurfaceBaselineThroughRect(CFeature *pFtr, BOOL bSurfaceInsideSelect, PT_3D ptsch, double r, CCoordSys *pCS)
{
	//非面无需检查
	CGeometry *pGeo = pFtr->GetGeometry();
	if( !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
		return TRUE;
	
	CGeoSurface *pSur = (CGeoSurface*)pGeo;
	if(pSur->IsFillColor() || bSurfaceInsideSelect)
	{
		Envelope e;
		e.CreateFromPtAndRadius(ptsch,r);

		PT_3D ret;
		double dis = -1;
		if( !pSur->FindNearestBasePt(ptsch,e,pCS,&ret,&dis) )
			return FALSE;
	}

	return TRUE;
}

void CWorker::LButtonDown(PT_3D &pt, int flag)
{
	if( m_bDisableCmdInput || m_bCmdProcessing)return;

	GetDataQuery()->GetActiveSearcher()->SetHitSurfaceInside(TRUE);

	int rflag = SELSTAT_NONESEL;

	m_selection.m_arrCanSelObjs.RemoveAll();
	if( m_bSelectorOpen )
	{
		int cursel_num = 0;
		const FTR_HANDLE *culselFtrs = m_selection.GetSelectedObjs(cursel_num);

		//m_selection.GetSelectedObjs(cursel_num);

		//多边形方式选取
		if( !m_bSelectDrag && m_nSelectMode==SELMODE_POLYGON )
		{
			m_arrSelDragPts.Add(pt);
			m_bSelectDrag = TRUE;
		}
		else
		//选择器不是正在拉框选择
		if( !m_bSelectDrag )
		{
			m_ptSelDragStart = pt;
			m_bSelectDrag = TRUE;

			PT_4D ptsch = pt;
			m_coordwnd.m_pSearchCS->GroundToClient(&ptsch,1);

			CPFeature pFtr = NULL;
			
			if(m_nSelectMode!=SELMODE_RECT)
			{						
				pFtr = GetDataQuery()->FindNearestObject(ptsch,m_selection.GetSelectRadius()*m_coordwnd.GetScaleOfSearchCSToViewCS(),
					m_coordwnd.m_pSearchCS,TRUE);

				double r = m_selection.GetSelectRadius()*m_coordwnd.GetScaleOfSearchCSToViewCS();

				int nFoundNum = 0;
				const CPFeature *ftrs = GetDataQuery()->GetFoundHandles(nFoundNum);
				for( int i=0; i<nFoundNum; i++)
				{
					if( !FilterSelect(FtrToHandle(ftrs[i])) )continue;
					m_selection.m_arrCanSelObjs.Add(FtrToHandle(ftrs[i]));			

					if (cursel_num > 1 && cursel_num < 100)
					{
						for (int j=0; j<cursel_num; j++)
						{
							if (culselFtrs[j] == FtrToHandle(ftrs[i]))
							{
								//由于面内部可选中面对象，这会导致两个面部分重叠时，已选中的面总是有优先选中权，这种优先权此时是不合理的。
								if( IsSurfaceBaselineThroughRect(ftrs[i],GetDataSource()->IsSurfaceInsideSelect(),ptsch,r,m_coordwnd.m_pSearchCS ) )
									pFtr = ftrs[i];
								break;
							}
						}
					}
					else
					{
						if (cursel_num == 1 && culselFtrs[0] == FtrToHandle(ftrs[i]))
						{
							if( IsSurfaceBaselineThroughRect(ftrs[i],GetDataSource()->IsSurfaceInsideSelect(),ptsch,r,m_coordwnd.m_pSearchCS ) )
								pFtr = ftrs[i];
						}
					}
				}

				if( !FilterSelect(FtrToHandle(pFtr)) )
				{
					if( m_selection.m_arrCanSelObjs.GetSize()>0 )
					{
						pFtr = HandleToFtr(m_selection.m_arrCanSelObjs[0]);
					}
				}
			}

			if( pFtr!=NULL )
			{			
				//鼠标单选
				if( !m_selection.IsObjInSelection(FtrToHandle(pFtr)) )
				{
					//增加一个选择(条件：多选模式、普通模式+按住Ctrl)
					if( m_nSelectMode==SELMODE_MULTI ||
						( m_nSelectMode==SELMODE_NORMAL && ((flag&MK_CONTROL)||(flag&MK_SHIFT)) ) )
					{
						m_selection.SelectObj(FtrToHandle(pFtr));
						rflag |= SELSTAT_MULTISEL;

					}
					//去除已有选择
					else
					{
						m_selection.DeselectAll();
						m_selection.SelectObj(FtrToHandle(pFtr));
					}
					rflag |= SELSTAT_POINTSEL;
				}
				else
				{
					//去除当前这个选择(条件：多选模式、普通模式+按住Ctrl)
					if( m_nSelectMode==SELMODE_MULTI ||
						( m_nSelectMode==SELMODE_NORMAL && ((flag&MK_CONTROL)||(flag&MK_SHIFT)) ) )
					{
						rflag |= SELSTAT_MULTISEL;
						m_selection.DeselectObj(FtrToHandle(pFtr));
					}
					//重选当前这个选择
					else
					{
						m_selection.DeselectObj(FtrToHandle(pFtr));
						m_selection.SelectObj(FtrToHandle(pFtr));						
					}
				}
				
				//纠正捕捉可能导致单选的二义性问题
				if( (rflag&SELSTAT_POINTSEL)!=0 && (fabs(m_coordwnd.m_ptGrd.x-m_coordwnd.m_ptCursor.x)>1e-6 || fabs(m_coordwnd.m_ptGrd.y-m_coordwnd.m_ptCursor.y)>1e-6) )
				{
					int selnum = 0;
					int num = 0;
					m_selection.GetSelectedObjs(num);	
					if( num==1 && m_selection.m_arrCanSelObjs.GetSize()>1 && m_selection.m_arrCanSelObjs.GetSize()<10 )
					{
						PT_3D pt2 = m_coordwnd.m_ptCursor;
						PT_3D pt3;
						m_coordwnd.m_pSearchCS->GroundToClient(&pt2,&pt3);
						
						FTR_HANDLE hFtr1 = m_selection.GetNearestInCanSelObjs(pt3,m_coordwnd.m_pSearchCS);
						m_selection.DeselectAll();
						m_selection.SelectObj(hFtr1);
					}
				}

				OnSelectChanged();
				m_bSelectDrag = FALSE;				
			}
			else
			{
				if( cursel_num>0 && !((flag&MK_CONTROL)||(flag&MK_SHIFT)) )
				{
					rflag = SELSTAT_DRAGSEL_RESTART;
				}
				if (m_nSelectMode!=SELMODE_SINGLE)
				{
					if (cursel_num==0)
					{
						rflag = SELSTAT_DRAGSEL_START;
					}

					//多选模式、普通模式+按住Ctrl
					if( ((flag&MK_CONTROL)||(flag&MK_SHIFT)) )
					{
						rflag |= SELSTAT_MULTISEL;						
					}
				}
			}
		}
		else if( m_bSelectDrag && m_nSelectMode==SELMODE_POLYGON )
		{
			m_arrSelDragPts.Add(pt);
			if( m_arrSelDragPts.GetSize()>1 )
			{
				GrBuffer buf;
				buf.BeginLineString(RGB(0,0,255),0);
				buf.MoveTo(&m_arrSelDragPts.GetAt(m_arrSelDragPts.GetSize()-2));
				buf.LineTo(&m_arrSelDragPts.GetAt(m_arrSelDragPts.GetSize()-1));
				buf.End();
				UpdateDrag(ud_AddConstDrag,&buf);
			}
		}
		//鼠标框选
		else
		{
			PT_4D t0[2];
			PT_4D t1[4];
			Envelope e;

			t0[0] = PT_4D(m_ptSelDragStart); t0[1] = PT_4D(pt);
			m_coordwnd.m_pViewCS->GroundToClient(t0,2);				
			
			t1[0] = t0[0];
			t1[1].x = t0[1].x;  t1[1].y = t0[0].y; t1[1].z = t0[1].z;  t1[1].yr = t0[0].yr;
			t1[2] = t0[1];
			t1[3].x = t0[0].x;  t1[3].y = t0[1].y; t1[3].z = t0[0].z;  t1[3].yr = t0[1].yr;			
			
			//是否按照全部包含的要求查找
			BOOL bEntirelyInc = (t1[2].x>=t1[0].x);

			m_coordwnd.m_pViewCS->ClientToGround(t1,4);

			m_coordwnd.m_pSearchCS->GroundToClient(t1,4);
			e.CreateFromPts(t1,4,sizeof(PT_4D));

			GetDataQuery()->FindObjectInRect(e,m_coordwnd.m_pSearchCS,bEntirelyInc);

			if( m_nSelectMode==SELMODE_MULTI ||
				( m_nSelectMode==SELMODE_NORMAL && ((flag&MK_CONTROL)||(flag&MK_SHIFT)) ) )
				rflag |= SELSTAT_MULTISEL;
			else
				m_selection.DeselectAll();

			int nsel = 0;
			const CPFeature *buf = GetDataQuery()->GetFoundHandles(nsel);
			for( int i=0; i<nsel; i++)
			{
				if( FilterSelect(FtrToHandle(buf[i])))
				{
					if( (flag&MK_SHIFT) )
					{
						if( !m_selection.IsObjInSelection(FtrToHandle(buf[i])) )
							m_selection.SelectObj(FtrToHandle(buf[i]));
						else
							m_selection.DeselectObj(FtrToHandle(buf[i]));
					}
					else
					{
						m_selection.SelectObj(FtrToHandle(buf[i]));
					}
				}
			}			

 			UpdateDrag(ud_SetConstDrag,NULL);
 			UpdateDrag(ud_SetVariantDrag,NULL);
			OnSelectChanged();

			m_bSelectDrag = FALSE;
			rflag |= SELSTAT_DRAGSEL;
		}

		OnSelectState(pt,rflag);
	}

	GetDataQuery()->GetActiveSearcher()->SetHitSurfaceInside(FALSE);

	//if( !(m_bSelectorOpen&&m_bSelectDrag&&rflag==SELSTAT_DRAGSEL_RESTART) )
	{
		CCommand *pCurCmd = GetCurrentCommand();
		if( !pCurCmd  && !( m_bSelectorOpen && m_nSelectMode==SELMODE_POLYGON ) && !( m_bSelectorOpen && m_nSelectMode==SELMODE_RECT ) )
		{
			m_bCmdProcessing = TRUE;
			if( rflag==SELSTAT_NONESEL || rflag==SELSTAT_DRAGSEL_RESTART )
				TestStateCommand(pt,CActiveStateCommand::stateClick);
			m_bCmdProcessing = FALSE;
		}

		CProcedure *pCurProc = GetAllLevelProcedures(CProcedure::msgPtClick,NULL);
		if( pCurProc )
		{
			m_bCmdProcessing = TRUE;
			pCurProc->PtClick(pt,rflag);
			m_bCmdProcessing = FALSE;

			TryFinishCurProcedure();
		}
	}

}





void CWorker::RButtonDown(PT_3D pt, int flag)
{
	if( m_bDisableCmdInput || m_bCmdProcessing)return;

	GetDataQuery()->GetActiveSearcher()->SetHitSurfaceInside(TRUE);

	if( m_bSelectorOpen )
	{
		if( m_bSelectDrag )
		{
			if( m_nSelectMode==SELMODE_POLYGON )
			{
				int size = m_arrSelDragPts.GetSize();
				if( size>=3 )
				{
					CArray<PT_3D,PT_3D> pts;
					pts.SetSize(size);

					for (int i=0; i<pts.GetSize(); i++)
					{
						m_coordwnd.m_pSearchCS->GroundToClient(&m_arrSelDragPts[i],&pts[i]);
					}
					

// 					Envelope e;
// 					e.CreateFromPts(m_arrSelDragPts.GetData(),m_arrSelDragPts.GetSize());
					m_selection.DeselectAll();
//					e.TransformGrdToClt(m_coordwnd.m_pSearchCS,1);
					GetDataQuery()->FindObjectInPolygon(pts.GetData(),pts.GetSize(),m_coordwnd.m_pSearchCS);
					int nsel = 0;
					const CPFeature *buf = GetDataQuery()->GetFoundHandles(nsel);
					for( i=0; i<nsel; i++)
					{
						if( FilterSelect(FtrToHandle(buf[i]))/* && 
							IsObjInRegion(buf[i]->GetGeometry(),m_arrSelDragPts.GetData(),m_arrSelDragPts.GetSize()) */)
							m_selection.SelectObj(FtrToHandle(buf[i]));
					}			
				}
				m_arrSelDragPts.RemoveAll();
				UpdateDrag(ud_SetConstDrag,NULL);
				UpdateDrag(ud_SetVariantDrag,NULL);
				OnSelectChanged();
				RefreshView();
				m_bSelectDrag = FALSE;				
				
				GetDataQuery()->GetActiveSearcher()->SetHitSurfaceInside(FALSE);

				return;
			}
			else
			{
				UpdateDrag(ud_ClearDrag);
				m_bSelectDrag = FALSE;
				
				GetDataQuery()->GetActiveSearcher()->SetHitSurfaceInside(FALSE);
				return;
			}
		}
	}

	GetDataQuery()->GetActiveSearcher()->SetHitSurfaceInside(FALSE);


	CProcedure *pCurProc = GetAllLevelProcedures(CProcedure::msgPtReset,NULL);
	if( pCurProc )
	{
		m_bCmdProcessing = TRUE;
		pCurProc->PtReset(pt);
		m_bCmdProcessing = FALSE;
		
		TryFinishCurProcedure();
	}
}


void CWorker::MouseMove(PT_3D &pt, int flag)
{
	if( m_bDisableCmdInput || m_bCmdProcessing)return;

	//执行选择器
	if( m_bSelectorOpen && m_bSelectDrag )
	{
		if( m_nSelectMode==SELMODE_POLYGON )
		{
			if( m_arrSelDragPts.GetSize()>0 )
			{
				GrBuffer buf;
				buf.BeginLineString(0,0);
				buf.MoveTo(&m_arrSelDragPts.GetAt(m_arrSelDragPts.GetSize()-1));
				buf.LineTo(&pt);
				if( m_arrSelDragPts.GetSize()>=2 )
					buf.LineTo(&m_arrSelDragPts.GetAt(0));
				buf.End();
				UpdateDrag(ud_SetVariantDrag,&buf);
			}
		}
		else
		{
			PT_4D t0[2];
			PT_4D t1[4];
			Envelope e;

			t0[0] = PT_4D(m_ptSelDragStart); t0[1] = PT_4D(pt);
			m_coordwnd.m_pViewCS->GroundToClient(t0,2);	
			e.CreateFromPts(t0,2,sizeof(PT_4D));
			
		
			t1[0] = t0[0];
			t1[1].x = t0[1].x;  t1[1].y = t0[0].y; t1[1].z = t0[1].z;  t1[1].yr = t0[0].yr;
			t1[2] = t0[1];
			t1[3].x = t0[0].x;  t1[3].y = t0[1].y; t1[3].z = t0[0].z;  t1[3].yr = t0[1].yr;

			
			m_coordwnd.m_pViewCS->ClientToGround(t1,4);
					
			GrBuffer buf;
			buf.BeginLineString(0,0,0);
			buf.MoveTo(t1);
			buf.LineTo(t1+1);
			buf.LineTo(t1+2);
			buf.LineTo(t1+3);
			buf.LineTo(t1);
			buf.End();

			UpdateDrag(ud_SetVariantDrag,&buf);
		}
	}
	else
	{		
		CProcedure *pCurProc = GetAllLevelProcedures(CProcedure::msgPtMove,NULL);
		if( pCurProc )
		{
			m_bCmdProcessing = TRUE;
			pCurProc->PtMove(pt);
			m_bCmdProcessing = FALSE;

			TryFinishCurProcedure();
		}
	}
}


void CWorker::KeyDown(UINT nChar, int flag)
{
	if( m_bDisableCmdInput )return;
}


BOOL CWorker::UIRequest(long reqtype, LONG_PTR param)
{
	if( reqtype==UIREQ_TYPE_STARTWAIT )
	{
		m_bDisableCmdInput = TRUE;
		SetCursorType(CURSOR_DISABLE);
	}
	else if( reqtype==UIREQ_TYPE_ENDWAIT )
	{
		m_bDisableCmdInput = FALSE;
		SetCursorType(CURSOR_DRAW);
	}

	return TRUE;
}

void CWorker::OnSelectState(PT_3D pt, int flag)
{
}

MyNameSpaceEnd