#include "stdafx.h"
#include "OverridTrimContourCmd.h"

void COverridTrimContourCmd::subPtClick(PT_3D &pt, int flag)
{
	PtClick(pt, flag);

	((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL, hc_UpdateAllObjects, 0);
}
