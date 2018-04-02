#include "stdafx.h"
#include "OverrideCutPartCmd.h"

void COverrideCutPartCmd::subPtClick(PT_3D &pt, int flag)
{
	PtClick(pt, flag);

	//¸üÐÂÊÓÍ¼
	((CDlgDoc*)m_pEditor)->UpdateAllViews(NULL, hc_UpdateAllObjects, 0);
}