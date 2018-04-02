// CoordWnd.cpp: implementation of the CCoordWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CoordWnd.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CCoordWnd::CalcScaleView()
{
	if( m_bScaleViewValid || !m_pViewCS )return;

	m_lfScaleView = m_pViewCS->CalcScale();
}

void CCoordWnd::CalcScaleSearch()
{
	if( m_bScaleSearchValid || !m_pSearchCS )return;
	
	m_lfScaleSearch = m_pSearchCS->CalcScale();
}

double CCoordWnd::GetScaleOfSearchCSToViewCS()
{
	CalcScaleSearch();
	CalcScaleView();
	return m_lfScaleSearch/m_lfScaleView;
}


MyNameSpaceEnd