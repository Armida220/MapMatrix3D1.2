#include "stdafx.h"
#include "OsgRealtimeVecLayer.h"


COsgRealtimeVecLayer::COsgRealtimeVecLayer()
{
}


COsgRealtimeVecLayer::~COsgRealtimeVecLayer()
{
}



GrBuffer* COsgRealtimeVecLayer::drawSymbol(CFeature *pFtr)
{
	CCoordSys *pCS = m_pCS;

	double gscale0 = pCS->CalcScale();
	double gscale = gscale0*m_pMainDS->GetScale() / 1000.0;
	int nDrawSymbolizedFlag = CalcSymbolizedFlag(gscale);

	CSymbolFlag sf1(CSymbolFlag::Tolerance, 1 / gscale0);
	CSymbolFlag sf2(CSymbolFlag::BreakCell, FALSE);

	GrBuffer *pBuf = new GrBuffer();
	if (m_pDrawMan)
	{
		CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
		if (pDS)
		{
			(m_pDrawMan->*m_pDrawFun)(pDS, pDS->GetFtrLayerOfObject(pFtr), pFtr, nDrawSymbolizedFlag, pBuf);
		}
	}

	return pBuf;
}