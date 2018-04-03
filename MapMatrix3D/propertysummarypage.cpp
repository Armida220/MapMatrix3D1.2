// propertysummarypage.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "propertysummarypage.h"
#include "DlgDataSource.h"
#include "SQLiteAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL g_bModified = FALSE;
/////////////////////////////////////////////////////////////////////////////
// CPropertySummaryPage property page

IMPLEMENT_DYNCREATE(CPropertySummaryPage, CPropertyPage)

CPropertySummaryPage::CPropertySummaryPage() : CPropertyPage(CPropertySummaryPage::IDD)
{
	//{{AFX_DATA_INIT(CPropertySummaryPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	g_bModified = FALSE;
}

CPropertySummaryPage::CPropertySummaryPage(CDlgDoc *pDoc) : CPropertyPage(CPropertySummaryPage::IDD)
{
	g_bModified = FALSE;
	m_pDoc = pDoc;
}

CPropertySummaryPage::~CPropertySummaryPage()
{
}

void CPropertySummaryPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertySummaryPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX,IDC_EDIT_TITLE,m_strTitle);
	DDX_Text(pDX,IDC_EDIT_TOPIC,m_strTopic);
	DDX_Text(pDX,IDC_EDIT_AUTHOR,m_strAuthor);
	DDX_Text(pDX,IDC_EDIT_KEYWORD,m_strKeyWord);
	DDX_Text(pDX,IDC_EDIT_REMARK,m_strRemark);
	DDX_Text(pDX,IDC_EDIT_LINK,m_strLink);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertySummaryPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertySummaryPage)
		// NOTE: the ClassWizard will add message map macros here
	ON_EN_CHANGE(IDC_EDIT_TITLE, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_TOPIC, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_AUTHOR, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_KEYWORD, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_REMARK, OnAutoSet)
	ON_EN_CHANGE(IDC_EDIT_LINK, OnAutoSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertySummaryPage message handlers
void CPropertySummaryPage::OnAutoSet()
{
	UpdateData(TRUE);
	SetModified(TRUE);
	g_bModified = TRUE;
}

BOOL CPropertySummaryPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	if (!m_pDoc) return TRUE;
	
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	
	CString name, type;
	pDS->GetDataSettings(FIELDNAME_SUMMARYTITLE,name,m_strTitle,type);
	pDS->GetDataSettings(FIELDNAME_SUMMARYTOPIC,name,m_strTopic,type);
	pDS->GetDataSettings(FIELDNAME_SUMMARYAUTHOR,name,m_strAuthor,type);
	pDS->GetDataSettings(FIELDNAME_SUMMARYKEYWORD,name,m_strKeyWord,type);
	pDS->GetDataSettings(FIELDNAME_SUMMARYREMARK,name,m_strRemark,type);
	pDS->GetDataSettings(FIELDNAME_SUMMARYLINK,name,m_strLink,type);

	UpdateData(FALSE);

	return TRUE;
}

void CPropertySummaryPage::OnOK()
{
	UpdateData(TRUE);
	
	if (!m_pDoc || !g_bModified) return;
	
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	
	CString type;
	pDS->SaveDataSettings(FIELDNAME_SUMMARYTITLE,FIELDNAME_SUMMARYTITLE,m_strTitle,type);
	pDS->SaveDataSettings(FIELDNAME_SUMMARYTOPIC,FIELDNAME_SUMMARYTOPIC,m_strTopic,type);
	pDS->SaveDataSettings(FIELDNAME_SUMMARYAUTHOR,FIELDNAME_SUMMARYAUTHOR,m_strAuthor,type);
	pDS->SaveDataSettings(FIELDNAME_SUMMARYKEYWORD,FIELDNAME_SUMMARYKEYWORD,m_strKeyWord,type);
	pDS->SaveDataSettings(FIELDNAME_SUMMARYREMARK,FIELDNAME_SUMMARYREMARK,m_strRemark,type);
	pDS->SaveDataSettings(FIELDNAME_SUMMARYLINK,FIELDNAME_SUMMARYLINK,m_strLink,type);

	g_bModified = FALSE;
}