#include "stdafx.h"
#include "OverrideReplaceLineCmd.h"

void COverrideReplaceLineCmd::subPtClick(PT_3D &pt, int flag)
{
	//输入起点
	if (m_nStep == 0)
	{
		m_ptStart = pt;

		//当前选中的地物
		int num1;
		const FTR_HANDLE *handles = m_pEditor->GetSelection()->GetSelectedObjs(num1);

		//吸取绘图钢笔码
		PT_3D pt1, pt2, ptnearest;
		double r = 0.5;
		Envelope e;
		e.CreateFromPtAndRadius(pt, r);
		m_objOldID1 = FtrToHandle(m_pEditor->GetDataQuery()->FindNearestObject(pt, r, NULL));
		int num2;
		const CPFeature* ftrs = m_pEditor->GetDataQuery()->GetFoundHandles(num2);

		for (int i = 0; i<num2; i++)
		{
			for (int j = 0; j<num1; j++)
			{
				if (HandleToFtr(handles[j]) == ftrs[i])
				{
					m_objOldID1 = handles[j];
					break;
				}
			}
		}
		if (!m_objOldID1)return;

		m_pOldObj1 = HandleToFtr(m_objOldID1);
		if (!m_pOldObj1 || !(m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))))
			return;

		if (m_pOldObj1->GetGeometry()->FindNearestBasePt(pt, e, NULL, &pt2, NULL))
		{
			Envelope ee;
			ee.CreateMaxEnvelope();
			int pos;
			m_pOldObj1->GetGeometry()->GetShape()->FindNearestLine(pt, ee, NULL, NULL, NULL, NULL, &pos);

			//得到前面的关键点的绘图钢笔码以及高程
			CArray<int, int> pKeyPos;
			m_pOldObj1->GetGeometry()->GetShape()->GetKeyPosOfBaseLines(pKeyPos);
			if (pKeyPos.GetSize()>0)
			{
				int num = m_pOldObj1->GetGeometry()->GetDataPointSum() - 1;
				while (num >= 0 && pKeyPos[num]>pos)num--;

				if (num >= 0)
				{
					PT_3DEX expt;
					expt = m_pOldObj1->GetGeometry()->GetDataPoint(num);
					if (!m_pEditor->GetCoordWnd().m_bIsStereo)
					{
						if (m_bVWsnaplinestyle)//如果m_bVWsnaplinestyle是true，则吸取，否则不吸取，而根据工具栏的选择确定
							m_pDrawProc->m_nCurPenCode = expt.pencode;
						else
							m_pDrawProc->m_nCurPenCode = m_nVWPen;
					}
					else
					{
						if (m_b3Dsnaplinestyle)//如果m_bVWsnaplinestyle是true，则吸取，否则不吸取，而根据工具栏的选择确定
							m_pDrawProc->m_nCurPenCode = expt.pencode;
						else
							m_pDrawProc->m_nCurPenCode = m_n3DPen;
					}


					// 保存立体视图参数
					char params[256] = { 0 };
					PDOC(m_pEditor)->UpdateAllViewsParams(1, (LPARAM)params);

					if (sscanf(params, "%d", &m_nSaveViewParam) == 1)
						m_bUseViewParam = TRUE;

					/*CView *pView = PDOC(m_pEditor)->GetCurActiveView();
					if (pView)
					{
					pView->SendMessage(FCCM_UPDATESTEREOPARAM, 1, (LPARAM)params);
					if( sscanf(params,"%d",&m_nSaveViewParam)==1 )
					m_bUseViewParam = TRUE;
					}*/

					// 设置目标层码对应的视图参数
					CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
					if (!pDS){ Abort(); return; }
					int nLayerId = PDOC(m_pEditor)->GetFtrLayerIDOfFtr(m_objOldID1);
					CFtrLayer *pLayer = pDS->GetFtrLayer(nLayerId);
					if (pLayer)
					{
						CPlaceConfigLib *pPlaceLib = gpCfgLibMan->GetPlaceConfigLib(pDS->GetScale());
						if (pPlaceLib)
						{
							CPlaceConfig *pConfig = pPlaceLib->GetConfig(pLayer->GetName());
							if (pConfig/* && pView*/)
							{
								PDOC(m_pEditor)->UpdateAllViewsParams(0, (LPARAM)pConfig->m_strViewParams);
								//pView->SendMessage(FCCM_UPDATESTEREOPARAM, 0, (LPARAM)pConfig->m_strViewParams);
							}
						}

					}

					//立体窗口
					if (m_pEditor->GetCoordWnd().m_bIsStereo)
					{
						((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL, hc_ModifyHeight, (CObject*)&pt2.z);
					}

					m_ptStart.z = pt2.z;
					pt.z = pt2.z;
				}
			}

		}

		long color = m_pOldObj1->GetGeometry()->GetColor();
		if (color == FTRCOLOR_BYLAYER)
		{
			CFtrLayer *pLayer = PDOC(m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(m_pOldObj1);
			if (!pLayer)
			{
				Abort();
				return;
			}
			color = pLayer->GetColor();
		}
		m_pObj->SetColor(color);
		m_nStep = 1;
		GotoState(PROCSTATE_PROCESSING);
	}

	if (m_nStep >= 2)
	{
		if (m_nWaitSelect == 1)
			m_ptSel1 = pt;
		else if (m_nWaitSelect == 2)
		{
			if (m_nStep == 2)
			{
				PromptString(StrFromResID(IDS_CMDTIP_SELECTTOSAVE2));
				m_ptSel1 = pt;
				m_nStep = 3;
				return;
			}
			else m_ptSel2 = pt;
		}

		CGeometry *pObj = GetReplaceObj_needSelect();
		if (!pObj)
		{
			PromptString(StrFromResID(IDS_CMDTIP_SELECTFAIL));
			m_nStep = 2;
			return;
		}

		m_pEditor->UpdateDrag(ud_ClearDrag);

		CFeature *pFtr = HandleToFtr(m_objOldID1)->Clone();
		if (!pFtr) return;

		CUndoFtrs undo(m_pEditor, Name());

		int layid = m_pEditor->GetFtrLayerIDOfFtr(m_objOldID1);
		pFtr->SetID(OUID());
		pFtr->SetGeometry(pObj);
		if (m_pEditor->AddObject(pFtr, layid))
		{
			GETXDS(m_pEditor)->CopyXAttributes(HandleToFtr(m_objOldID1), pFtr);
			undo.AddNewFeature(FtrToHandle(pFtr));
		}
		if (m_pEditor->DeleteObject(m_objOldID1))
		{
			undo.AddOldFeature(m_objOldID1);
		}
		if (m_pEditor->DeleteObject(m_objOldID2))
		{
			undo.AddOldFeature(m_objOldID2);
		}
		undo.Commit();

		Finish();
	}

	if (m_pDrawProc)
	{
		m_pDrawProc->PtClick(pt, flag);
	}
}

void COverrideReplaceLineCmd::subPtMove(PT_3D &pt)
{
	if (m_pDrawProc)
	{
		m_pDrawProc->PtMove(pt);
	}
	if (m_nWaitSelect == 1 || m_nWaitSelect == 2)
	{
		return;
	}
	if (IsProcFinished(this))return;

	{
		//判断目标地物是否在可以连接的范围内
		BOOL bCanLink = TRUE;
		double r = 0.5;

		//m_pEditor->UpdateDrag(ud_SetVariantDrag,NULL);
		//查找

		CFeature *pOldObj = NULL;
		Envelope e;
		e.CreateFromPtAndRadius(pt, r);
		pOldObj = m_pEditor->GetDataQuery()->FindNearestObject(pt, r, NULL);
		if (pOldObj)
		{
			if ((pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))))
			{
				//层码必须一致
				CFtrLayer *pLayer1 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(m_pOldObj1);
				CFtrLayer *pLayer2 = ((CDlgDoc*)m_pEditor)->GetDlgDataSource()->GetFtrLayerOfObject(pOldObj);

				if (pLayer1 && pLayer2&&pLayer1 != pLayer2&&
					strcmp(pLayer1->GetName(), pLayer2->GetName()) != 0)
				{
					pOldObj = NULL;
				}
				//如果是面，就不支持同时修测两个地物
				else if ((m_pOldObj1&&m_pOldObj1->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))) ||
					(pOldObj&&pOldObj->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface))))
				{
					if (FtrToHandle(pOldObj) != m_objOldID1)pOldObj = NULL;
				}
			}
			else pOldObj = NULL;
		}
		else pOldObj = NULL;

		/*if( !pOldObj )pFtr = NULL;*/

		//不是处于选点状态
		if (m_nWaitSelect == 0 && m_bCanTwoObjs)
		{
			//高程必须对应
			if (m_bMatchHeight && pOldObj)
			{
				if (m_pOldObj1 && CModifyZCommand::CheckObjForContour(m_pOldObj1->GetGeometry()))
				{
					PT_3DEX expt1, expt2;
					expt1 = m_pOldObj1->GetGeometry()->GetDataPoint(0);
					expt2 = pOldObj->GetGeometry()->GetDataPoint(0);
					if (!CModifyZCommand::CheckObjForContour(pOldObj->GetGeometry()) ||
						fabs(expt1.z - expt2.z) >= 1e-4)
					{
						pOldObj = NULL;
					}
				}
			}

			if (!pOldObj)
			{
				int aaaa = 10;
			}
			m_objOldID2 = FtrToHandle(pOldObj);
			m_pOldObj2 = pOldObj;
		}
		//必须是已经被接受处理的两个地物
		else
		{
			if ((m_objOldID1&&FtrToHandle(pOldObj) != m_objOldID1) && (m_objOldID2&&FtrToHandle(pOldObj) != m_objOldID2))
				pOldObj = NULL;
		}

		PT_3D pt0;
		if (!pOldObj || !pOldObj->GetGeometry()->FindNearestBasePt(pt, e, NULL, &pt0, NULL))
			bCanLink = FALSE;

		if (!m_bCanTwoObjs && m_objOldID1 && FtrToHandle(pOldObj) != m_objOldID1)
			bCanLink = FALSE;

		GrBuffer vbuf;
		if (bCanLink && m_bShowMark)
		{
			//更新显示
			GrBuffer buf;
			buf.BeginLineString(0, 0);
			subDrawPointTip(pOldObj->GetGeometry(), pt0, &buf);
			buf.End();
			vbuf.CopyFrom(&buf);
		}

		if (m_pDrawProc&&m_pDrawProc->m_arrPts.GetSize() <= 0)
		{
			m_pEditor->UpdateDrag(ud_SetVariantDrag, NULL);
		}

		m_pEditor->UpdateDrag(ud_AddVariantDrag, &vbuf);
	}

}

void COverrideReplaceLineCmd::subDrawPointTip(CGeometry *pObj, PT_3D pt, GrBuffer *buf)
{
	PT_3D pt3ds[4];
	pt3ds[0] = PT_3D(pt.x - 1, pt.y, pt.z);
	pt3ds[1] = PT_3D(pt.x + 1, pt.y, pt.z);
	pt3ds[2] = PT_3D(pt.x, pt.y - 1, pt.z);
	pt3ds[3] = PT_3D(pt.x, pt.y + 1, pt.z);

	buf->MoveTo(pt3ds);
	buf->LineTo(pt3ds + 1);
	buf->MoveTo(pt3ds + 2);
	buf->LineTo(pt3ds + 3);
}
