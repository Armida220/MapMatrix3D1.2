// G:\testMm3D\FeatureOne\FeatureCollector2009\mm3dPrj\MyListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MyListCtrl.h"


// CMyListCtrl

IMPLEMENT_DYNAMIC(CMyListCtrl, CListCtrl)

CMyListCtrl::CMyListCtrl()
{

}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CMyListCtrl message handlers




int CMyListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetTextColor(RGB(255, 255, 255));
	SetBkColor(RGB(50, 50, 50));
	return 0;
}



HBRUSH CMyListCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CListCtrl::OnCtlColor(pDC, pWnd, nCtlColor);
	m_brush.CreateSolidBrush(RGB(50, 50, 50));
	return m_brush;
	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CMyListCtrl::DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct)
{

}


void CMyListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnLButtonDown(nFlags, point);

	GetParent()->PostMessage(WM_NCLBUTTONDOWN,
		HTCAPTION,
		MAKELPARAM(point.x, point.y));
}
