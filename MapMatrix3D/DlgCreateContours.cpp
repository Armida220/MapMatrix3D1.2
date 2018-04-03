// DlgCreateContours.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgCreateContours.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCreateContours dialog

static CString defICode = "7101022";
static CString defCCode = "7101012";
static float defIntvl = 5.0f;
static int	 defECode = 10;
static int   defIntNum = 4;

#define CREATECONTOURS_SETTINGS  "CreateContours_Settings"

CDlgCreateContours::CDlgCreateContours(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCreateContours::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCreateContours)
	m_strCountFCode = defCCode;
	m_nExCode = defECode;	
	m_strIndexFCode = defICode;
	m_fInterval = defIntvl;
	m_nInterNum = defIntNum;
	//}}AFX_DATA_INIT

	CWinApp *pApp = AfxGetApp();
	m_strCountFCode = pApp->GetProfileString(CREATECONTOURS_SETTINGS,"CountFCode",m_strCountFCode);
	m_strIndexFCode = pApp->GetProfileString(CREATECONTOURS_SETTINGS,"IndexFCode",m_strIndexFCode);
}


void CDlgCreateContours::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCreateContours)
	DDX_Text(pDX, IDC_EDIT_COUNT_FCODE, m_strCountFCode);
	DDX_Text(pDX, IDC_EDIT_EXCODE, m_nExCode);
	DDX_Text(pDX, IDC_EDIT_INDEX_FCODE, m_strIndexFCode);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_fInterval);
	DDX_Text(pDX, IDC_EDIT_INTERNUM, m_nInterNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCreateContours, CDialog)
	//{{AFX_MSG_MAP(CDlgCreateContours)
	ON_EN_CHANGE(IDC_EDIT_INTERVAL, OnChangeEditInterval)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCreateContours message handlers

void CDlgCreateContours::OnOK() 
{
	UpdateData(TRUE);

	defIntvl = m_fInterval;
	defICode = m_strIndexFCode;
	defCCode = m_strCountFCode;
	defECode = m_nExCode;
	defIntNum = m_nInterNum;

	CWinApp *pApp = AfxGetApp();
	pApp->WriteProfileString(CREATECONTOURS_SETTINGS,"CountFCode",m_strCountFCode);
	pApp->WriteProfileString(CREATECONTOURS_SETTINGS,"IndexFCode",m_strIndexFCode);
	
	CDialog::OnOK();
}

void CDlgCreateContours::OnChangeEditInterval() 
{
	UpdateData(TRUE);

	if( m_fInterval==2.5 )
	{
		m_nInterNum = 3;
		CWnd *pWnd = GetDlgItem(IDC_EDIT_INTERNUM);
		if( pWnd )
		{
			CString strText = _T("3");
			pWnd->SetWindowText(strText);
		}
	}
	else
	{
		m_nInterNum = 4;
		CWnd *pWnd = GetDlgItem(IDC_EDIT_INTERNUM);
		if( pWnd )
		{
			CString strText = _T("4");
			pWnd->SetWindowText(strText);
		}
	}
}
