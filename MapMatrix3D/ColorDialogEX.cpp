// ColorDialogEX.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "ColorDialogEX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorDialogEX

IMPLEMENT_DYNAMIC(CColorDialogEX, CColorDialog)

CColorDialogEX::CColorDialogEX(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd) :
	CColorDialog(clrInit, dwFlags, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(CColorDialogEX, CColorDialog)
	//{{AFX_MSG_MAP(CColorDialogEX)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CColorDialogEX::OnInitDialog() 
{
	CColorDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CColorDialog color_dlg;
	CPoint   point;			
	GetCursorPos(&point);
	CRect rect;
	GetWindowRect(&rect);

	CRect temp;
	temp.left = point.x + 10;
	temp.top = point.y + 10;
	temp.right = temp.left + rect.Width();
	temp.bottom = temp.top + rect.Height();
	MoveWindow(&temp,TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control	              // EXCEPTION: OCX Property Pages should return FALSE
}
