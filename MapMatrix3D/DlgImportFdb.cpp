// DlgImportFdb.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgImportFdb.h"
#include "SmartViewFunctions.h"
#include "DlgChooseMP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportFdb dialog


CDlgImportFdb::CDlgImportFdb(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportFdb::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportFdb)
	m_strPath = _T("");
	m_strMapName = _T("*");
	m_bOnDB = FALSE;
	m_bCheckOnlyLocal = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgImportFdb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportFdb)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_MAPNAME, m_strMapName);
	DDX_Check(pDX, IDC_CHECK1, m_bCheckOnlyLocal);
	//}}AFX_DATA_MAP
}

BOOL CDlgImportFdb::OnInitDialog()
{
	if(m_bOnDB)
		m_bCheckOnlyLocal = TRUE;

	CDialog::OnInitDialog();
	
	SetWindowText(m_strTitle);

	if(!m_bOnDB)
	{
		CRect rc1, rc2;
		GetDlgItem(IDC_EDIT_ID_FILEPATH)->GetClientRect(&rc1);
		GetDlgItem(IDC_EDIT_MAPNAME)->GetWindowRect(&rc2);
		ScreenToClient(&rc2);
		rc2.right = rc2.left + rc1.Width();
		GetDlgItem(IDC_EDIT_MAPNAME)->MoveWindow(rc2);
		GetDlgItem(IDC_CHECK1)->ShowWindow(SW_HIDE);
	}

	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgImportFdb, CDialog)
	//{{AFX_MSG_MAP(CDlgImportFdb)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonIdBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportFdb message handlers


BOOL DlgSelectMultiFiles(HWND hOwner, LPCTSTR filter, LPCTSTR defExt, CStringArray& fileName, CString *all_name);

void CDlgImportFdb::OnButtonIdBrowse() 
{
	if(m_bOnDB)
	{
		CDlgChooseMP dlg;
		if(dlg.DoModal()!=IDOK)
			return;
		m_arrFileNames.Copy(dlg.m_arrSelectedMaps);
		for (int i = 0; i < m_arrFileNames.GetSize(); i++)
		{
			if (i>0) m_strPath += ";";
			m_strPath += m_arrFileNames[i];
		}
	}
	else
	{
		CString strFilter = m_strFilter;
		strFilter.Replace('|','\0');

		if( !DlgSelectMultiFiles(GetSafeHwnd(),
			strFilter,
			NULL,m_arrFileNames,&m_strPath) )
			return;
	}

	UpdateData(FALSE);	
}

void CDlgImportFdb::OnOK() 
{
	// TODO: Add extra validation here

	UpdateData(TRUE);
	
	CDialog::OnOK();
}
