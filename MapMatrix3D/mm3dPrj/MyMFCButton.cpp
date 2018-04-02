// G:\testMm3D\FeatureOne\FeatureCollector2009\mm3dPrj\MyMFCButton.cpp : implementation file
//

#include "stdafx.h"
#include "MyMFCButton.h"


// CMyMFCButton

IMPLEMENT_DYNAMIC(CMyMFCButton, CMFCButton)

CMyMFCButton::CMyMFCButton()
{

}

CMyMFCButton::~CMyMFCButton()
{
}


BEGIN_MESSAGE_MAP(CMyMFCButton, CMFCButton)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CMyMFCButton message handlers




void CMyMFCButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CMFCButton::OnPaint() for painting messages

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(100, 100, 100)); //ÉèÖÃÎª»ÒÉ«±³¾°
}


HBRUSH CMyMFCButton::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CMFCButton::OnCtlColor(pDC, pWnd, nCtlColor);
	m_brush.CreateSolidBrush(RGB(100, 100, 100));
	// TODO:  Change any attributes of the DC here
	return m_brush;
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


int CMyMFCButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	this->SetTimer(1, 1, NULL);
	return 0;
}


void CMyMFCButton::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	Invalidate(TRUE);
	CMFCButton::OnTimer(nIDEvent);
}
