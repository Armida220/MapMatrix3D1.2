// DlgConvertByPts.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgConvertByPts.h"
#include "editbasedoc.h"
#include "Functions_temp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConvertByPts dialog


CDlgConvertByPts::CDlgConvertByPts(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConvertByPts::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConvertByPts)
	m_strPath = _T("");
	m_strPath2 = _T("");
	m_strPath3 = _T("");
	m_nRange = 0;
	m_b2D = TRUE;
	//}}AFX_DATA_INIT
}


void CDlgConvertByPts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConvertByPts)
	DDX_Text(pDX, IDC_EDIT_PATH1, m_strPath);
	DDX_Text(pDX, IDC_EDIT_PATH2, m_strPath2);
	DDX_Text(pDX, IDC_EDIT_PATH3, m_strPath3);
	DDX_Radio(pDX, IDC_RADIO_ALLMAP, m_nRange);
	DDX_Check(pDX, IDC_CHECK_2D, m_b2D);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConvertByPts, CDialog)
	//{{AFX_MSG_MAP(CDlgConvertByPts)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE2, OnButtonBrowse2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE3, OnButtonBrowse3)
	ON_BN_CLICKED(IDC_RADIO_ALLMAP, OnRadioRange)
	ON_BN_CLICKED(IDC_RADIO_SELECTION, OnRadioRange)
	ON_BN_CLICKED(IDC_RADIO_FILES, OnRadioRange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConvertByPts message handlers

extern BOOL DlgSelectMultiFiles(HWND hOwner, LPCTSTR filter, LPCTSTR defExt, CStringArray& fileName, CString *all_name);


void CDlgConvertByPts::OnButtonBrowse() 
{
	if( !DlgSelectMultiFiles(GetSafeHwnd(),
		_T("Supported files (*.fdb;*.dxf;*.dwg)\0*.fdb;*.dxf;*.dwg\0All Files (*.*)\0*.*\0\0"),
		_T("*.fdb"),m_arrFileNames,&m_strPath) )
		return;

	UpdateData(FALSE);
}

void CDlgConvertByPts::OnButtonBrowse2() 
{
	CString filter(StrFromResID(IDS_ALLFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	m_strPath2 = dlg.GetPathName();
	UpdateData(FALSE);
}

void CDlgConvertByPts::OnButtonBrowse3() 
{
	CString filter(StrFromResID(IDS_ALLFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	m_strPath3 = dlg.GetPathName();
	UpdateData(FALSE);
}

BOOL CDlgConvertByPts::OnInitDialog()
{
	CDialog::OnInitDialog();

	BeginCheck41License

	if( GetActiveDlgDoc()==NULL )
	{
		CWnd *pWnd = GetDlgItem(IDC_RADIO_ALLMAP);
		if( pWnd )
		{
			pWnd->EnableWindow(FALSE);
		}
		pWnd = GetDlgItem(IDC_RADIO_SELECTION);
		if( pWnd )
		{
			pWnd->EnableWindow(FALSE);
		}

		m_nRange = 2;
		UpdateData(FALSE);
	}

	UpdateFileEdit();

	EndCheck41License
	else
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	return TRUE;
}

void CDlgConvertByPts::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	CDialog::OnOK();
}


void CDlgConvertByPts::OnRadioRange()
{
	UpdateData(TRUE);
	UpdateFileEdit();	
}


void CDlgConvertByPts::UpdateFileEdit()
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_PATH1);
	if( pWnd )
	{
		pWnd->EnableWindow(m_nRange==2);
	}
	
	pWnd = GetDlgItem(IDC_STATIC_FILEDESC);
	if( pWnd )
	{
		pWnd->EnableWindow(m_nRange==2);
	}
}