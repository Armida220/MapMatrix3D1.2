// XFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "XFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CXFileDialog

IMPLEMENT_DYNAMIC(CXFileDialog, CFileDialog)

BEGIN_MESSAGE_MAP(CXFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CXFileDialog)
	//ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXFileDialog::CXFileDialog( LPCTSTR lpszTitle,
						   BOOL bOpenFileDialog, LPCTSTR lpszDefExt,
	LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName,
		dwFlags, lpszFilter, pParentWnd)
{
	m_bVistaStyle = FALSE;

	m_bCheckLoadAllMdl	   = FALSE;
	m_strTitle = lpszTitle;

	m_ofn.Flags |= OFN_ENABLETEMPLATE|OFN_ENABLESIZING;
	m_ofn.lpstrTitle = lpszTitle;
	m_ofn.hInstance = AfxGetInstanceHandle();
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_XFILEDIALOG2);
}

///////////////////////////////////////////////////////////////////////////////
// DoDataExchange
void CXFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXFileDialog)
	DDX_Check(pDX, IDC_CHECK_LOADALLMDL, m_bCheckLoadAllMdl);
	//}}AFX_DATA_MAP
}

BOOL CXFileDialog::OnFileNameOK()
{
	UpdateData(TRUE);
	return CFileDialog::OnFileNameOK();
}

///////////////////////////////////////////////////////////////////////////////
// OnInitDialog
BOOL CXFileDialog::OnInitDialog()
{
	TRACE(_T("in CXFileDialog::OnInitDialog\n"));

	CFileDialog::OnInitDialog();

	if( !m_strTitle.IsEmpty() )
		GetParent()->SetWindowText(m_strTitle);

	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}
