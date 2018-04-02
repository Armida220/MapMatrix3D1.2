#include "stdafx.h"
#include "OverrideInsertVertCmd.h"


BOOL COverrideInsertVertCmd::subHitTest(PT_3D pt, int state) const
{
	CPFeature pFtr = NULL;

	CSelection *pselection = m_pEditor->GetSelection();
	CCoordWnd pencode = m_pEditor->GetCoordWnd();
	double r = 0.5;
	PT_3D spt = pt;

	CDataQuery *pDQ = m_pEditor->GetDataQuery();

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

	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		return 0;

	//是否在有效范围内
	Envelope e;
	e.CreateFromPtAndRadius(spt, r);

	PT_3D nearest;
	if (!pGeo->FindNearestBasePt(spt, e, NULL, &nearest, NULL))
	{
		return FALSE;
	}

	PT_KEYCTRL kc = pGeo->FindNearestKeyCtrlPt(spt, max(e.Width(), e.Height()), pencode.m_pSearchCS);
	if (kc.type == PT_KEYCTRL::typeKey && (kc.index == 0 || kc.index == (pGeo->GetDataPointSum() - 1)))
	{
		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)pGeo)->IsClosed())
			return FALSE;
		if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			return FALSE;
	}

	return TRUE;
}
