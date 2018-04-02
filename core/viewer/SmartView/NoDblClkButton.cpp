// NoDblClkButton.cpp: implementation of the CNoDblClkButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NoDblClkButton.h"
#include <afxcontrolbars.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CNoDblClkButton, CMFCButton)

BEGIN_MESSAGE_MAP(CNoDblClkButton, CMFCButton)
	//{{AFX_MSG_MAP(CNoDblClkButton)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CNoDblClkButton::CNoDblClkButton()
{
	
}


CNoDblClkButton::~CNoDblClkButton()
{
}

void CNoDblClkButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CWnd* pParent = GetParent ();
	if (pParent != NULL)
	{
		pParent->SendMessage (	WM_COMMAND,
			MAKEWPARAM (GetDlgCtrlID (), BN_CLICKED),
			(LPARAM) m_hWnd);
	}
	CMFCButton::OnLButtonDblClk(nFlags, point);
}


void CNoDblClkButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CMFCButton::OnMouseMove(nFlags, point);
}

void CNoDblClkButton::OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState)
{
	ASSERT_VALID(pDC);

	if (m_bChecked && m_bCaptured)
	{
		pDC->Draw3dRect(rectClient, GetGlobalData()->clrBtnDkShadow, GetGlobalData()->clrBtnDkShadow);
	}
	else if (m_bChecked || m_bPushed)
	{
		pDC->Draw3dRect(rectClient, GetGlobalData()->clrBtnDkShadow, GetGlobalData()->clrBtnHilite);
	}
	else if (m_bCaptured)
	{
		pDC->Draw3dRect(rectClient, GetGlobalData()->clrBtnHilite, GetGlobalData()->clrBtnDkShadow);
	}
}

void CNoDblClkButton::OnDraw(CDC* pDC, const CRect& rect, UINT uiState)
{
	CRect rect1 = rect;
	if (m_bChecked)
	{
		rect1.top += 3;
	}
	
	CMFCButton::OnDraw(pDC, rect1, uiState);
}
