// G:\testMm3D\FeatureOne\FeatureCollector2009\mm3dPrj\MyCodeEdit.cpp : implementation file
//

#include "stdafx.h"
#include "MyCodeEdit.h"


// CMyCodeEdit

IMPLEMENT_DYNAMIC(CMyCodeEdit, CEdit)

CMyCodeEdit::CMyCodeEdit()
{

}

CMyCodeEdit::~CMyCodeEdit()
{
}


BEGIN_MESSAGE_MAP(CMyCodeEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CMyCodeEdit message handlers




HBRUSH CMyCodeEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  Change any attributes of the DC here
	pDC->SetBkColor(RGB(50, 50, 50));
	m_brush.CreateSolidBrush(RGB(50, 50, 50));
	pDC->SetTextColor(RGB(255, 255, 255));           // ×ÖÌåÉ«²Ê   
	// TODO:  Return a non-NULL brush if the parent's handler should not be called
	return m_brush;
}

