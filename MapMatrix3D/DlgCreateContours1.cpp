// DlgCreateContours1.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgCreateContours1.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCreateContours1 dialog

static CString defICode = "7101022";
static CString defCCode = "7101012";
static float defIntvl = 5.0f;
static int	 defECode = 10;
static int   defIntNum = 4;

CDlgCreateContours1::CDlgCreateContours1(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCreateContours1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCreateContours1)
	m_strCountFCode = defCCode;
	m_nExCode = defECode;	
	m_strIndexFCode = defICode;
	m_fInterval = defIntvl;
	m_nInterNum = defIntNum;
	//}}AFX_DATA_INIT
}


void CDlgCreateContours1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCreateContours1)
	DDX_Text(pDX, IDC_EDIT_DEMFILE, m_strDEMFile);
	DDX_Text(pDX, IDC_EDIT_COUNT_FCODE, m_strCountFCode);
	DDX_Text(pDX, IDC_EDIT_EXCODE, m_nExCode);
	DDX_Text(pDX, IDC_EDIT_INDEX_FCODE, m_strIndexFCode);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_fInterval);
	DDX_Text(pDX, IDC_EDIT_INTERNUM, m_nInterNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCreateContours1, CDialog)
	//{{AFX_MSG_MAP(CDlgCreateContours1)
	ON_EN_CHANGE(IDC_EDIT_INTERVAL, OnChangeEditInterval)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEDEM, OnBrowseDEMFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCreateContours1 message handlers

void CDlgCreateContours1::OnOK() 
{
	UpdateData(TRUE);

	defIntvl = m_fInterval;
	defICode = m_strIndexFCode;
	defCCode = m_strCountFCode;
	defECode = m_nExCode;
	defIntNum = m_nInterNum;
	
	CDialog::OnOK();
}

void CDlgCreateContours1::OnChangeEditInterval() 
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


void CDlgCreateContours1::OnBrowseDEMFile()
{
	CString filter(StrFromResID(IDS_LOADDEM_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strDEMFile = dlg.GetPathName();
	UpdateData(FALSE);	
}
