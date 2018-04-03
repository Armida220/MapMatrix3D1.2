// DlgInputData.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgInputData.h"
#include "SilenceDDX.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInputData dialog


CDlgInputData::CDlgInputData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputData)
		// NOTE: the ClassWizard will add member initialization here
	m_fData = 0.0;
	//}}AFX_DATA_INIT
	m_bptShow = FALSE;
	m_colorBak = RGB(255,255,255);
}


void CDlgInputData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputData)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	if( pDX->m_bSaveAndValidate )
		DDX_Text_Silence(pDX, IDC_EDIT_INPUT, m_fData);
	else
	{
		float f = m_fData;
		DDX_Text_Silence(pDX, IDC_EDIT_INPUT, f);
	}
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputData, CDialog)
	//{{AFX_MSG_MAP(CDlgInputData)
		// NOTE: the ClassWizard will add message map macros here
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgInputData::OnInitDialog() 
{
	if( !CDialog::OnInitDialog() )
		return FALSE;

	m_brushBak.CreateSolidBrush(m_colorBak); 

	if (m_bptShow)
	{
		CRect rcDlg, rcFrame;
		GetWindowRect(&rcDlg);

		SetWindowPos(NULL,m_ptShow.x,m_ptShow.y,rcDlg.Width(),rcDlg.Height(),SWP_SHOWWINDOW);
	}
	
	return TRUE;
}

HBRUSH CDlgInputData::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// Call the base class implementation first! Otherwise, it may
	// undo what we are trying to accomplish here.
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// Are we painting the IDC_MYSTATIC control? We can use
	// CWnd::GetDlgCtrlID() to perform the most efficient test.
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_INPUT)
	{		
		// Set the background mode for text to transparent 
		// so background will show thru.
		pDC->SetBkMode(TRANSPARENT);
		
		// Return handle to our CBrush object.
		hbr = m_brushBak;
	}
	
	return hbr;

}
/////////////////////////////////////////////////////////////////////////////
// CDlgInputData message handlers
