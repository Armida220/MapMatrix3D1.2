// DlgSelectPairPhoto.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgSelectPairPhoto.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectPairPhoto dialog


CDlgSelectPairPhoto::CDlgSelectPairPhoto(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectPairPhoto::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectPairPhoto)
	m_strLeftImage = _T("");
	m_strRightImage = _T("");
	//}}AFX_DATA_INIT
}


void CDlgSelectPairPhoto::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectPairPhoto)
	DDX_Text(pDX, IDC_EDIT_LEFTIMAGE, m_strLeftImage);
	DDX_Text(pDX, IDC_EDIT_RIGHTIMAGE, m_strRightImage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectPairPhoto, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectPairPhoto)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE4, OnButtonBrowse4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectPairPhoto message handlers

void CDlgSelectPairPhoto::OnButtonBrowse() 
{
	CString filter(StrFromResID(IDS_LOADTIF_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	
	m_strLeftImage = dlg.GetPathName();
	
	UpdateData(FALSE);
	
}

void CDlgSelectPairPhoto::OnButtonBrowse4() 
{
	CString filter(StrFromResID(IDS_LOADTIF_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	
	m_strRightImage = dlg.GetPathName();
	
	UpdateData(FALSE);
}

void CDlgSelectPairPhoto::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	if (m_strLeftImage.IsEmpty()||m_strRightImage.IsEmpty()||!PathFileExists(m_strLeftImage)||!PathFileExists(m_strRightImage))
	{
		AfxMessageBox(StrFromResID(IDS_FILE_NOTEMPTY_OR_PATHERROR));
		return;
	}
	CDialog::OnOK();
}
