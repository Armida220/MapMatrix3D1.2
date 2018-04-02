// propertyuserdefinepage.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "propertyuserdefinepage.h"
#include "DlgDataSource.h"
#include "SQLiteAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL g_bModified = FALSE;
/////////////////////////////////////////////////////////////////////////////
// CPropertyUserDefinePage property page

IMPLEMENT_DYNCREATE(CPropertyUserDefinePage, CPropertyPage)

CPropertyUserDefinePage::CPropertyUserDefinePage() : CPropertyPage(CPropertyUserDefinePage::IDD)
{
	//{{AFX_DATA_INIT(CPropertyUserDefinePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	g_bModified = FALSE;
}

CPropertyUserDefinePage::CPropertyUserDefinePage(CDlgDoc *pDoc) : CPropertyPage(CPropertyUserDefinePage::IDD)
{
	g_bModified = FALSE;
	m_pDoc = pDoc;
}

CPropertyUserDefinePage::~CPropertyUserDefinePage()
{
}

void CPropertyUserDefinePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyUserDefinePage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX,IDC_EDIT_NAME1,m_strName1);
	DDX_Text(pDX,IDC_EDIT_VALUE1,m_strValue1);
	DDX_Text(pDX,IDC_EDIT_NAME2,m_strName2);
	DDX_Text(pDX,IDC_EDIT_VALUE2,m_strValue2);
	DDX_Text(pDX,IDC_EDIT_NAME3,m_strName3);
	DDX_Text(pDX,IDC_EDIT_VALUE3,m_strValue3);
	DDX_Text(pDX,IDC_EDIT_NAME4,m_strName4);
	DDX_Text(pDX,IDC_EDIT_VALUE4,m_strValue4);
	DDX_Text(pDX,IDC_EDIT_NAME5,m_strName5);
	DDX_Text(pDX,IDC_EDIT_VALUE5,m_strValue5);
	DDX_Text(pDX,IDC_EDIT_NAME6,m_strName6);
	DDX_Text(pDX,IDC_EDIT_VALUE6,m_strValue6);
	DDX_Text(pDX,IDC_EDIT_NAME7,m_strName7);
	DDX_Text(pDX,IDC_EDIT_VALUE7,m_strValue7);
	DDX_Text(pDX,IDC_EDIT_NAME8,m_strName8);
	DDX_Text(pDX,IDC_EDIT_VALUE8,m_strValue8);
	DDX_Text(pDX,IDC_EDIT_NAME9,m_strName9);
	DDX_Text(pDX,IDC_EDIT_VALUE9,m_strValue9);
	DDX_Text(pDX,IDC_EDIT_NAME10,m_strName10);
	DDX_Text(pDX,IDC_EDIT_VALUE10,m_strValue10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyUserDefinePage, CPropertyPage)
//{{AFX_MSG_MAP(CPropertyUserDefinePage)
// NOTE: the ClassWizard will add message map macros here
	ON_EN_CHANGE(IDC_EDIT_NAME1, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_VALUE1, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_NAME2, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_VALUE2, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_NAME3, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_VALUE3, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_NAME4, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_VALUE4, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_NAME5, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_VALUE5, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_NAME6, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_VALUE6, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_NAME7, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_VALUE7, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_NAME8, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_VALUE8, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_NAME9, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_VALUE9, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_NAME10, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_VALUE10, OnAutoSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyUserDefinePage message handlers
void CPropertyUserDefinePage::OnAutoSet()
{
	UpdateData(TRUE);
	SetModified(TRUE);
	g_bModified = TRUE;
}

BOOL CPropertyUserDefinePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	if (!m_pDoc) return TRUE;
	
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	
	CString type;
	pDS->GetDataSettings(FIELDNAME_USERDEFINEFIELD1,m_strName1,m_strValue1,type);
	pDS->GetDataSettings(FIELDNAME_USERDEFINEFIELD2,m_strName2,m_strValue2,type);
	pDS->GetDataSettings(FIELDNAME_USERDEFINEFIELD3,m_strName3,m_strValue3,type);
	pDS->GetDataSettings(FIELDNAME_USERDEFINEFIELD4,m_strName4,m_strValue4,type);
	pDS->GetDataSettings(FIELDNAME_USERDEFINEFIELD5,m_strName5,m_strValue5,type);
	pDS->GetDataSettings(FIELDNAME_USERDEFINEFIELD6,m_strName6,m_strValue6,type);
	pDS->GetDataSettings(FIELDNAME_USERDEFINEFIELD7,m_strName7,m_strValue7,type);
	pDS->GetDataSettings(FIELDNAME_USERDEFINEFIELD8,m_strName8,m_strValue8,type);
	pDS->GetDataSettings(FIELDNAME_USERDEFINEFIELD9,m_strName9,m_strValue9,type);
	pDS->GetDataSettings(FIELDNAME_USERDEFINEFIELD10,m_strName10,m_strValue10,type);
	
	UpdateData(FALSE);
	return TRUE;
}

void CPropertyUserDefinePage::OnOK()
{
	UpdateData(TRUE);
	
	if (!m_pDoc || !g_bModified) return;
	
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	
	CString type;
	pDS->SaveDataSettings(FIELDNAME_USERDEFINEFIELD1,m_strName1,m_strValue1,type);
	pDS->SaveDataSettings(FIELDNAME_USERDEFINEFIELD2,m_strName2,m_strValue2,type);
	pDS->SaveDataSettings(FIELDNAME_USERDEFINEFIELD3,m_strName3,m_strValue3,type);
	pDS->SaveDataSettings(FIELDNAME_USERDEFINEFIELD4,m_strName4,m_strValue4,type);
	pDS->SaveDataSettings(FIELDNAME_USERDEFINEFIELD5,m_strName5,m_strValue5,type);
	pDS->SaveDataSettings(FIELDNAME_USERDEFINEFIELD6,m_strName6,m_strValue6,type);
	pDS->SaveDataSettings(FIELDNAME_USERDEFINEFIELD7,m_strName7,m_strValue7,type);
	pDS->SaveDataSettings(FIELDNAME_USERDEFINEFIELD8,m_strName8,m_strValue8,type);
	pDS->SaveDataSettings(FIELDNAME_USERDEFINEFIELD9,m_strName9,m_strValue9,type);
	pDS->SaveDataSettings(FIELDNAME_USERDEFINEFIELD10,m_strName10,m_strValue10,type);

	g_bModified = FALSE;
}