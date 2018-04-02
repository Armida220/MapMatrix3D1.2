#include "stdafx.h"
#include "OverridMatchVertCmd.h"

void COverrideMatchVertCmd::subPtClick(PT_3D &pt, int flag)
{
	PtClick(pt, flag);

	((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL, hc_UpdateAllObjects, 0);
}
