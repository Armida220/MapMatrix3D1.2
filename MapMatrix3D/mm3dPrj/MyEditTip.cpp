// G:\testMm3D\FeatureOne\FeatureCollector2009\mm3dPrj\MyEditTip.cpp : implementation file
//

#include "stdafx.h"
#include "MyEditTip.h"


// CMyEditTip

IMPLEMENT_DYNAMIC(CMyEditTip, CEdit)

CMyEditTip::CMyEditTip()
{

}

CMyEditTip::~CMyEditTip()
{
}


BEGIN_MESSAGE_MAP(CMyEditTip, CEdit)
//	ON_WM_CTLCOLOR()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CMyEditTip message handlers



HBRUSH CMyEditTip::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  Change any attributes of the DC here
	pDC->SetBkColor(RGB(50, 50, 50));
	m_brush.CreateSolidBrush(RGB(50, 50, 50));
	pDC->SetTextColor(RGB(255, 255, 255));           // ×ÖÌåÉ«²Ê 
	return m_brush;
	// TODO:  Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}
