// G:\testMm3D\FeatureOne\FeatureCollector2009\mm3dPrj\MyDefineDockPane.cpp : implementation file
//

#include "stdafx.h"
#include "MyDefineDockTabPane.h"


// CMyDefineDockTabPane

IMPLEMENT_DYNAMIC(CMyDefineDockTabPane, CTabbedPane)

CMyDefineDockTabPane::CMyDefineDockTabPane()
{

}

CMyDefineDockTabPane::~CMyDefineDockTabPane()
{
}


BEGIN_MESSAGE_MAP(CMyDefineDockTabPane, CTabbedPane)
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()

	

// CMyDefineDockTabPane message handlers
void CMyDefineDockTabPane::setNoClose()
{ 
	//m_dwControlBarStyle &= ~AFX_CBRS_CLOSE; 
	//m_dwControlBarStyle &= ~AFX_CBRS_AUTOHIDE; 
}

void CMyDefineDockTabPane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages
}


void CMyDefineDockTabPane::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default

	CDockablePane::OnNcCalcSize(bCalcValidRects, lpncsp);
}
