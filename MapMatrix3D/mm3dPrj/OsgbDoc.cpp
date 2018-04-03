#include "stdafx.h"
#include "OsgbDoc.h"
#include "OsgbView.h"
#include "OsgbVecView.h"
#include "DlgSelectShift.h"
#include "qcomm.h"
#include "DlgCommand.h"
#include "osgdef.h"
#include "DlgWorkSpaceBound.h"
#include "DlgScheme.h"
#include "PluginAccess.h"
#include "ExMessage.h"
#include "PlotText.h"
#include "DlgCommand2.h"
#include "OverridMatchVertCmd.h"
#include "OverridTrimContourCmd.h"
#include "OverrideEditVertCmd.h"
#include "OverrideInsertVertCmd.h"
#include "OverrideReplaceLineCmd.h"
#include "OverrideCutPartCmd.h"


//面的母线是否通过的区域
static BOOL IsSurfaceBaselineThroughRect(CFeature *pFtr, BOOL bSurfaceInsideSelect, PT_3D ptsch, double r, CCoordSys *pCS)
{
	//非面无需检查
	CGeometry *pGeo = pFtr->GetGeometry();
	if (!pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		return TRUE;

	CGeoSurface *pSur = (CGeoSurface*)pGeo;
	if (pSur->IsFillColor() || bSurfaceInsideSelect)
	{
		Envelope e;
		e.CreateFromPtAndRadius(ptsch, r);

		PT_3D ret;
		double dis = -1;
		if (!pSur->FindNearestBasePt(ptsch, e, pCS, &ret, &dis))
			return FALSE;
	}

	return TRUE;
}


COsgbDoc::COsgbDoc()
{

}

COsgbDoc::~COsgbDoc()
{
}


BEGIN_MESSAGE_MAP(COsgbDoc, CDlgDoc)
	ON_COMMAND(ID_OPEN_OSGB, &COsgbDoc::OnOpenOsgb)
	ON_COMMAND(ID_REMOVE_OSGB, &COsgbDoc::OnRemoveOsgb)
END_MESSAGE_MAP()


void COsgbDoc::OnOpenOsgb()
{
	// TODO: Add your command handler code here
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		CView* pOsgView = GetNextView(pos);
		if (pOsgView && pOsgView->IsKindOf(RUNTIME_CLASS(COsgbView)))
		{
			pOsgView->SendMessage(0, 0, 0);
			return;
		}
	}

}

void COsgbDoc::OnRemoveOsgb()
{
	// TODO: Add your command handler code here
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		CView* pOsgView = GetNextView(pos);
		if (pOsgView && pOsgView->IsKindOf(RUNTIME_CLASS(COsgbView)))
		{
			pOsgView->SendMessage(WM_RM_ALL_SCENE, 0, 0);
			return;
		}
	}

}

void COsgbDoc::subLButtonDown(PT_3D &pt, int flag)
{
	BOOL bOld = m_pDataQuery->SetSymFixed(TRUE);

	CView* pView = GetActiveView();

	CWorker::LButtonDown(pt, flag);

	m_pDataQuery->SetSymFixed(bOld);

	if (!(m_bSelectorOpen&&m_bSelectDrag) && CanSetAnchor())
	{
		SetAnchorPoint(pt);
	}

	if (m_selection.m_arrCanSelObjs.GetSize() > 1)
	{
		if (m_pDlgSelectShift == NULL)
		{
			m_pDlgSelectShift = new CDlgSelectShift();
			m_pDlgSelectShift->Create(CDlgSelectShift::IDD, NULL);
		}

		if (m_pDlgSelectShift != NULL)
		{
			if (!m_pDlgSelectShift->IsWindowVisible())
			{
				CRect rcClient;
				GetWindowRect(GetActiveView()->GetSafeHwnd(), rcClient);

				CRect rcDlg;
				m_pDlgSelectShift->GetWindowRect(&rcDlg);

				rcDlg.OffsetRect(rcClient.left - rcDlg.left, rcClient.top - rcDlg.top);
				m_pDlgSelectShift->MoveWindow(&rcDlg);

				m_pDlgSelectShift->m_pDoc = this;
				m_pDlgSelectShift->m_arrFtrs.RemoveAll();
				for (int i = 0; i < m_selection.m_arrCanSelObjs.GetSize(); i++)
				{
					m_pDlgSelectShift->m_arrFtrs.Add(HandleToFtr(m_selection.m_arrCanSelObjs[i]));
				}
				m_pDlgSelectShift->UpdateList();
				m_pDlgSelectShift->ShowWindow(SW_SHOW);

				CView *pView = GetActiveView();
				if (pView)pView->SetFocus();

			}
			else
				m_pDlgSelectShift->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if (m_pDlgSelectShift != NULL)
		{
			m_pDlgSelectShift->ShowWindow(SW_HIDE);
		}
	}

}

void COsgbDoc::subMouseMove(PT_3D &pt, int flag)
{
	CView* pView = GetActiveView();

	subParentMouseMv(pt, flag);
}

void COsgbDoc::subParentMouseMv(PT_3D &pt, int flag)
{
	if (m_bDisableCmdInput || m_bCmdProcessing)return;

	//执行选择器
	if (m_bSelectorOpen && m_bSelectDrag)
	{
		if (m_nSelectMode == SELMODE_POLYGON)
		{
			if (m_arrSelDragPts.GetSize() > 0)
			{
				GrBuffer buf;
				buf.BeginLineString(0, 0);
				buf.MoveTo(&m_arrSelDragPts.GetAt(m_arrSelDragPts.GetSize() - 1));
				buf.LineTo(&pt);
				if (m_arrSelDragPts.GetSize() >= 2)
					buf.LineTo(&m_arrSelDragPts.GetAt(0));
				buf.End();
				UpdateDrag(ud_SetVariantDrag, &buf);
			}
		}
		else
		{
			PT_4D t0[2];
			PT_4D t1[4];
			Envelope e;

			t0[0] = PT_4D(m_ptSelDragStart); t0[1] = PT_4D(pt);
			m_coordwnd.m_pViewCS->GroundToClient(t0, 2);
			e.CreateFromPts(t0, 2, sizeof(PT_4D));


			t1[0] = t0[0];
			t1[1].x = t0[1].x;  t1[1].y = t0[0].y; t1[1].z = t0[1].z;  t1[1].yr = t0[0].yr;
			t1[2] = t0[1];
			t1[3].x = t0[0].x;  t1[3].y = t0[1].y; t1[3].z = t0[0].z;  t1[3].yr = t0[1].yr;


			m_coordwnd.m_pViewCS->ClientToGround(t1, 4);

			GrBuffer buf;
			buf.BeginLineString(0, 0, 0);
			buf.MoveTo(t1);
			buf.LineTo(t1 + 1);
			buf.LineTo(t1 + 2);
			buf.LineTo(t1 + 3);
			buf.LineTo(t1);
			buf.End();

			UpdateDrag(ud_SetVariantDrag, &buf);
		}
	}
	else
	{
		CProcedure *pCurProc = GetAllLevelProcedures(CProcedure::msgPtMove, NULL);
		if (pCurProc)
		{
			m_bCmdProcessing = TRUE;

			if (pCurProc->IsKindOf(RUNTIME_CLASS(CReplaceLinesCommand)))
			{
				COverrideReplaceLineCmd* replaceCmd = DYNAMIC_DOWNCAST(COverrideReplaceLineCmd, pCurProc);

				if (replaceCmd)
				{
					replaceCmd->subPtMove(pt);
				}
			}
			/*else if (pCurProc->IsKindOf(RUNTIME_CLASS(CDlgEditVertexCommand)))
			{
				COverrideEditVertCmd* editCmd = DYNAMIC_DOWNCAST(COverrideEditVertCmd, pCurProc);

				if (editCmd)
				{
					editCmd->subPtMove(pt);
				}
			}*/
			else
			{
				pCurProc->PtMove(pt);
			}

			m_bCmdProcessing = FALSE;

			TryFinishCurProcedure();
		}
	}
}

void COsgbDoc::TestStateCommand(PT_3D pt, int state)
{
	int num;
	m_selection.GetSelectedObjs(num);
	if (num <= 0)return;

	LPPROC_CMDCREATE lpProc;
	for (int i = 0; i < m_arrCreateProc.GetSize(); i++)
	{
		lpProc = m_arrCreateProc.GetAt(i);
		if (lpProc)
		{
			COverrideInsertVertCmd* insertCmd = DYNAMIC_DOWNCAST(COverrideInsertVertCmd, (*lpProc)());
			COverrideEditVertCmd* editCmd = DYNAMIC_DOWNCAST(COverrideEditVertCmd, (*lpProc)());

			if (insertCmd)
			{
				insertCmd->Init(this);

				if (insertCmd->subHitTest(pt, state))
				{
					for (int j = 0; j < m_arrCmdReg.GetSize(); j++)
					{
						if (lpProc == m_arrCmdReg[j].lpProcCreate)
						{
							m_nLastCmdCreateID = m_nCurCmdCreateID;
							m_nCurCmdCreateID = m_arrCmdReg[j].id;
							break;
						}
					}

					m_arrWaitCmdObj.Add(insertCmd);
					OnStartCommand(insertCmd);
					insertCmd->Start();
					break;
				}
				else
					delete insertCmd;
			}
			else if (editCmd)
			{
				editCmd->Init(this);

				if (editCmd->subHitTest(pt, state))
				{
					for (int j = 0; j < m_arrCmdReg.GetSize(); j++)
					{
						if (lpProc == m_arrCmdReg[j].lpProcCreate)
						{
							m_nLastCmdCreateID = m_nCurCmdCreateID;
							m_nCurCmdCreateID = m_arrCmdReg[j].id;
							break;
						}
					}

					m_arrWaitCmdObj.Add(editCmd);
					OnStartCommand(editCmd);
					editCmd->Start();
					break;
				}
				else
					delete editCmd;
			}
		}
	}
}
