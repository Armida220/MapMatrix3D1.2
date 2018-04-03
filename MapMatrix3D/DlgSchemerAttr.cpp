// DlgSchemerAttr.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgSchemerAttr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSchemerAttr dialog


CDlgSchemerAttr::CDlgSchemerAttr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSchemerAttr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSchemerAttr)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSchemerAttr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSchemerAttr)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSchemerAttr, CDialog)
	//{{AFX_MSG_MAP(CDlgSchemerAttr)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSchemerAttr message handlers

BOOL CDlgSchemerAttr::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return false;
	}
	
	
	return CDialog::PreTranslateMessage(pMsg);
}