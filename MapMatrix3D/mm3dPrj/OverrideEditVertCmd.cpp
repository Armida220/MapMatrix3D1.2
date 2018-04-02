#include "stdafx.h"
#include "OverrideEditVertCmd.h"
#include "ExMessage.h"
#include "PlotText.h"

void COverrideEditVertCmd::subPtClick(PT_3D &pt, int flag)
{
	subParentPtClick(pt, flag);

	if (m_nStep == 1)
	{
		if (m_arrFtrs.GetSize() > 0 && m_arrKeyPts.GetSize() > 0)
		{
			m_pSel = new CSelChangedExchanger(m_arrFtrs[0], (CDlgDoc*)m_pEditor, m_arrKeyPts[0].index);
			AfxGetMainWnd()->SendMessage(FCCM_MODIFY_PROPERTY_UI, 0, (LPARAM)m_pSel);
		}

		//二维窗口
		if (!m_pEditor->GetCoordWnd().m_bIsStereo)
		{
			pt.z = m_ptDragStart.z;
		}
		else if (m_bGetPointZ)
		{
			((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL, hc_ModifyHeight, (CObject*)&m_ptDragStart.z);
		}
	}

	return;
}

void COverrideEditVertCmd::subParentPtClick(PT_3D &pt, int flag)
{
	if (m_nStep == 0)
	{
		CSelection *pselection = m_pEditor->GetSelection();
		CCoordWnd pencode = m_pEditor->GetCoordWnd();
		double r = 0.5/*pselection->GetSelectRadius()*pencode.GetScaleOfSearchCSToViewCS()*/;
		PT_3D spt = pt;

		//pencode.m_pSearchCS->GroundToClient(&pt, &spt);

		CDataQuery *pDQ = m_pEditor->GetDataQuery();

		int nsel = 0;
		pselection->GetSelectedObjs(nsel);
		if (nsel == 1)
		{
			CFeature *pFtr = HandleToFtr(pselection->GetLastSelectedObj());
			m_arrFtrs.Add(pFtr);
		}
		else if (nsel > 1)
		{
			Envelope e;
			e.CreateFromPtAndRadius(spt, r);
			m_pEditor->GetDataQuery()->FindObjectInRect(e, NULL);

			int num;
			const CPFeature * ftr = m_pEditor->GetDataQuery()->GetFoundHandles(num);

			for (int i = 0; i < num; i++)
			{
				if (pselection->IsObjInSelection(FtrToHandle(ftr[i])))
				{
					m_arrFtrs.Add(ftr[i]);
				}
			}

		}
		GotoState(PROCSTATE_PROCESSING);

		if (m_arrFtrs.GetSize() <= 0)
		{
			return;
		}

		Envelope e;
		e.CreateFromPtAndRadius(spt, r);

		for (int i = 0; i < m_arrFtrs.GetSize(); i++)
		{
			CFeature *pFtr = m_arrFtrs[i];
			if (!pFtr) continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if (!pGeo) continue;

			PT_KEYCTRL keyPt = pGeo->FindNearestKeyCtrlPt(spt, r, NULL);
			if (!keyPt.IsValid())
			{
				m_arrFtrs.RemoveAt(i);
				i--;
			}
			else
			{
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
				{
					CGeoMultiSurface *pMGeo = (CGeoMultiSurface*)pGeo;
					int nGeo = pMGeo->GetSurfaceNum();
					int nPtClosedPos = 0, j;
					for (j = 0; j < nGeo; j++)
					{
						CArray<PT_3DEX, PT_3DEX> arrSonPts;
						pMGeo->GetSurface(j, arrSonPts);

						if (keyPt.index == nPtClosedPos || keyPt.index == (nPtClosedPos + arrSonPts.GetSize() - 1))
						{
							keyPt.index = nPtClosedPos;
							m_arrPtsRepeat.Add(TRUE);
							break;
						}

						nPtClosedPos += arrSonPts.GetSize();
					}
					if (j >= nGeo)
					{
						m_arrPtsRepeat.Add(FALSE);
					}
				}
				else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					if (((CGeoCurveBase*)pGeo)->IsClosed() && (keyPt.index == 0 || keyPt.index == pGeo->GetDataPointSum() - 1))
					{
						keyPt.index = 0;
						m_arrPtsRepeat.Add(TRUE);
					}
					else
						m_arrPtsRepeat.Add(FALSE);
				}
				else
				{
					m_arrPtsRepeat.Add(FALSE);
				}

				m_arrKeyPts.Add(keyPt);
			}

		}


		if (m_arrKeyPts.GetSize() <= 0)
		{
			return;
		}

		PT_KEYCTRL keyPt = m_arrKeyPts[0];

		CGeometry *pGeo = m_arrFtrs[0]->GetGeometry();
		if (!pGeo)
		{
			return;
		}
		if (keyPt.type == PT_KEYCTRL::typeKey)
		{
			PT_3DEX t;
			t = pGeo->GetDataPoint(keyPt.index);
			m_ptDragStart.x = t.x;
			m_ptDragStart.y = t.y;
			m_ptDragStart.z = t.z;
			m_ptDragEnd = m_ptDragStart;
		}
		else
		{
			PT_3D t;
			t = pGeo->GetCtrlPoint(keyPt.index);
			m_ptDragStart.x = t.x;
			m_ptDragStart.y = t.y;
			m_ptDragStart.z = t.z;
			m_ptDragEnd = m_ptDragStart;
		}

		OutputVertexInfo(keyPt, pGeo);

		m_nStep = 1;
		m_pEditor->CloseSelector();

	}
	else if (m_nStep == 1)
	{
		m_ptDragEnd = pt;

		CUndoBatchAction batchUndo(m_pEditor, _T("EditVertexs"));
		for (int i = 0; i < m_arrFtrs.GetSize(); i++)
		{
			CFeature *pObj = m_arrFtrs[i];
			if (!pObj)return;

			CGeometry *pGeo = pObj->GetGeometry();
			if (!pGeo)return;

			m_pEditor->DeleteObject(FtrToHandle(pObj), FALSE);

			CUndoVertex undo(m_pEditor, Name());

			if (m_arrKeyPts[i].type == PT_KEYCTRL::typeKey)
			{
				PT_3DEX t;
				t = pGeo->GetDataPoint(m_arrKeyPts[i].index);

				undo.ptOld = t;

				t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;

				pGeo->SetDataPoint(m_arrKeyPts[i].index, t);

				undo.ptNew = t;
				if (m_arrPtsRepeat[i])
				{
					if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
					{
						((CGeoMultiSurface*)pGeo)->SetClosedPt(m_arrKeyPts[i].index, pt, TRUE);
					}
					else if (m_arrKeyPts[i].index)
					{
						t = pGeo->GetDataPoint(0);
						t.x = pt.x;  t.y = pt.y;  t.z = pt.z;
						pGeo->SetDataPoint(0, t);
					}
					else
					{
						int nLastPt = pGeo->GetDataPointSum() - 1;
						t = pGeo->GetDataPoint(nLastPt);
						t.x = pt.x;  t.y = pt.y;  t.z = pt.z;
						pGeo->SetDataPoint(nLastPt, t);
					}
				}
			}
			else if (m_arrKeyPts[i].type == PT_KEYCTRL::typeCtrl)
			{
				PT_3D t;
				t = pGeo->GetCtrlPoint(m_arrKeyPts[i].index);

				COPY_3DPT(undo.ptOld, t);

				t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
				pGeo->SetCtrlPoint(m_arrKeyPts[i].index, t);

				COPY_3DPT(undo.ptNew, t);

			}

			m_pEditor->RestoreObject(FtrToHandle(pObj));
			//m_pEditor->UpdateObject(FtrToHandle(m_arrFtrs[i]));	

			undo.handle = FtrToHandle(m_arrFtrs[i]);
			undo.bRepeat = m_arrPtsRepeat[i];
			undo.nPtType = m_arrKeyPts[i].type;
			undo.nPtIdx = m_arrKeyPts[i].index;
			//undo.Commit();

			batchUndo.AddAction(&undo);
		}

		batchUndo.Commit();

		m_pEditor->UpdateDrag(ud_ClearDrag);
		m_pEditor->OnSelectChanged(TRUE);

		Finish();
		m_pEditor->CloseSelector();
		m_nStep = 3;

	}
	return;
}

BOOL COverrideEditVertCmd::subHitTest(PT_3D pt, int state)const
{
	CSelection *pselection = m_pEditor->GetSelection();

	CPFeature pFtr = NULL;
	int nsel = 0;
	pselection->GetSelectedObjs(nsel);
	if (nsel == 1 && state == stateDblClick)
	{
		pFtr = HandleToFtr(pselection->GetLastSelectedObj());

		CGeometry *pGeo = pFtr->GetGeometry();
		GrBuffer buf;
		pGeo->Draw(&buf, GetSymbolDrawScale());
		Envelope e = buf.GetEnvelope();
		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) && e.bPtIn(&pt))
		{
			return TRUE;
		}
	}

	return subParentHitTest(pt, state);
}

BOOL COverrideEditVertCmd::subParentHitTest(PT_3D pt, int state)const
{
	CSelection *pselection = m_pEditor->GetSelection();
	CCoordWnd cw = m_pEditor->GetCoordWnd();
	double r = 0.5;
	PT_3D spt = pt;

	CDataQuery *pDQ = m_pEditor->GetDataQuery();

	CPFeature pFtr = NULL;
	int nsel = 0;
	pselection->GetSelectedObjs(nsel);
	if (nsel == 1)pFtr = HandleToFtr(pselection->GetLastSelectedObj());
	else if (nsel > 1)
	{
		CPFeature pNearest = pDQ->FindNearestObject(spt, r, NULL);

		int nFoundNum = 0;
		const CPFeature *ftrs = pDQ->GetFoundHandles(nFoundNum);
		for (int i = 0; i < nFoundNum; i++)
		{
			if (pselection->IsObjInSelection(FTR_HANDLE(ftrs[i])))
			{
				pFtr = ftrs[i];
				break;
			}

		}

	}

	CFeature *pObj = pFtr;
	if (!pObj)return 0;

	CGeometry *pGeo = pObj->GetGeometry();
	if (!pGeo)return 0;

	Envelope e;
	e.CreateFromPtAndRadius(spt, r);

	PT_KEYCTRL retpt = pGeo->FindNearestKeyCtrlPt(spt, r, NULL);

	if (retpt.IsValid())return TRUE;
	return FALSE;
}

void COverrideEditVertCmd::subPtMove(PT_3D &pt)
{
	GrBuffer buf, buf1;

	double matrix[16];
	Matrix44FromMove(pt.x - m_ptDragStart.x, pt.y - m_ptDragStart.y, pt.z - m_ptDragStart.z, matrix);

	CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();

	// 不在立体上显示点状地物符号
	UpdateViewType updateview = uv_AllView;

	int num = m_arrFtrs.GetSize();
	for (int i = 0; i < num && num < 100; i++)
	{
		CFeature *pFtr0 = m_arrFtrs.GetAt(i);
		CFeature *pFtr = pFtr0->Clone();
		if (!pFtr) continue;

		buf.DeleteAll();

		CGeometry *pGeo = pFtr->GetGeometry();
		//pGeo->Transform(matrix);
		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CArray<PT_3DEX, PT_3DEX> pts;
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

			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) && m_arrKeyPts[0].type == PT_KEYCTRL::typeCtrl)
			{
				//if( m_arrKeyPts[0].type==PT_KEYCTRL::typeCtrl )
				{
					m_ptDragEnd = pt;

					PT_3D t;
					t = pGeo->GetCtrlPoint(m_arrKeyPts[0].index);
					t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
					pGeo->SetCtrlPoint(m_arrKeyPts[0].index, t);

					CDlgDataSource *pDS = PDOC(m_pEditor)->GetDlgDataSource();
					CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr0);

					if (pLayer)
					{
						pDS->DrawFeature(pFtr, &buf, TRUE, 0, pLayer->GetName());
					}

				}
			}
			else
			{
				CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr0);
				if (pLayer)
				{
					pDS->DrawFeature(pFtr, &buf, TRUE, 0, pLayer->GetName());
				}

				buf.Transform(matrix);
			}
		}

		buf1.AddBuffer(&buf);

		delete pFtr;

	}

	m_pEditor->UpdateDrag(ud_SetVariantDrag, &buf, updateview);

	/*CEditVertexCommand::PtMove(pt);*/
	subParentPtMove(pt);
}

void COverrideEditVertCmd::subParentPtMove(PT_3D &pt)
{
	if (m_nStep == 1)
	{
		GrBuffer buf;

		m_ptDragEnd = pt;

		for (int i = 0; i < m_arrFtrs.GetSize(); i++)
		{
			CFeature *pFtr = m_arrFtrs[i];
			if (!pFtr)return;

			CFeature *pObj = pFtr->Clone();
			if (!pObj)return;

			CGeometry *pGeo = pObj->GetGeometry();
			if (!pGeo)return;

			if (m_arrKeyPts[i].type == PT_KEYCTRL::typeKey)
			{
				CArray<PT_3DEX, PT_3DEX> arrPts;
				pGeo->GetShape(arrPts);

				PT_3DEX t;
				t = arrPts[m_arrKeyPts[i].index];
				t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
				arrPts[m_arrKeyPts[i].index] = t;

				if (m_arrPtsRepeat[i])
				{
					if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
					{
						((CGeoMultiSurface*)pGeo)->SetClosedPt(m_arrKeyPts[i].index, pt, TRUE);
					}
					else if (m_arrKeyPts[i].index)
					{
						t = arrPts[0];
						t.x = pt.x;  t.y = pt.y;  t.z = pt.z;
						arrPts[0] = t;

						pGeo->CreateShape(arrPts.GetData(), arrPts.GetSize());
					}
					else
					{
						int nLastPt = arrPts.GetSize() - 1;
						t = arrPts[nLastPt];
						t.x = pt.x;  t.y = pt.y;  t.z = pt.z;
						arrPts[nLastPt] = t;

						pGeo->CreateShape(arrPts.GetData(), arrPts.GetSize());
					}
				}
				else
				{
					pGeo->CreateShape(arrPts.GetData(), arrPts.GetSize());
				}

				BOOL close = FALSE;
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					close = ((CGeoCurveBase*)pGeo)->IsClosed();
				}
				const CShapeLine *pSL = pGeo->GetShape();
				if (pSL)pSL->GetVariantGrBuffer(&buf, close, m_arrKeyPts[i].index, true);


			}
			else if (m_arrKeyPts[i].type == PT_KEYCTRL::typeCtrl)
			{
				PT_3D t;
				t = pGeo->GetCtrlPoint(m_arrKeyPts[i].index);
				t.x = m_ptDragEnd.x;  t.y = m_ptDragEnd.y;  t.z = m_ptDragEnd.z;
				pGeo->SetCtrlPoint(m_arrKeyPts[i].index, t);

				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
				{
					CArray<PT_3DEX, PT_3DEX> pts;
					((CGeoParallel*)pGeo)->GetParallelShape(pts);
					int num = pts.GetSize();

					buf.BeginLineString(pGeo->GetColor(), 0, 0);
					for (int i = num - 1; i >= 0; i--)
					{
						if (i == (num - 1))buf.MoveTo(&pts[i]);
						else buf.LineTo(&pts[i]);
					}
					buf.End();
				}
				// 				else
				// 				{
				// 					pGeo->Draw(&buf);
				// 				}

			}

			delete pObj;
		}

		m_pEditor->UpdateDrag(ud_AddVariantDrag, &buf);
	}
}
