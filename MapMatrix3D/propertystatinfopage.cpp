// propertystatinfopage.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "propertystatinfopage.h"
#include "DlgDataSource.h"
#include "SQLiteAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyStatInfoPage property page

IMPLEMENT_DYNCREATE(CPropertyStatInfoPage, CPropertyPage)

CPropertyStatInfoPage::CPropertyStatInfoPage() : CPropertyPage(CPropertyStatInfoPage::IDD)
{
	//{{AFX_DATA_INIT(CPropertyStatInfoPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyStatInfoPage::CPropertyStatInfoPage(CDlgDoc *pDoc) : CPropertyPage(CPropertyStatInfoPage::IDD)
{
	m_pDoc = pDoc;
}

CPropertyStatInfoPage::~CPropertyStatInfoPage()
{
}

void CPropertyStatInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyStatInfoPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyStatInfoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertyStatInfoPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyStatInfoPage message handlers
BOOL CPropertyStatInfoPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	if (!m_pDoc) return TRUE;
	
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();

	CString filepath = m_pDoc->GetPathName();
	
	WIN32_FILE_ATTRIBUTE_DATA attr;
	memset(&attr,0,sizeof(attr));
	GetFileAttributesEx(filepath,GetFileExInfoStandard,&attr);
	
	SYSTEMTIME st;
	FILETIME   ftLocal;	

	LCID lcID = GetThreadLocale();
	DWORD dwID = LANGIDFROMLCID( lcID );
	DWORD id = PRIMARYLANGID( dwID );
	
	// CREATETIME
    FileTimeToLocalFileTime ( &attr.ftCreationTime, &ftLocal );
    FileTimeToSystemTime ( &ftLocal, &st );
	
	if( LANG_CHINESE == id )
	{
		char createtime[128];
		sprintf(createtime,"%d%s%d%s%d%s, %d:%d:%d",(int)st.wYear,(const char*)(LPCTSTR)StrFromResID(IDS_YEAR),(int)st.wMonth,
			(const char*)(LPCTSTR)StrFromResID(IDS_MONTH),(int)st.wDay,(const char*)(LPCTSTR)StrFromResID(IDS_DAY),(int)st.wHour,(int)st.wMinute,(int)st.wSecond);
		SetDlgItemText(IDC_STATIC_STATCREATETIME,createtime);
	}
	else
	{
		CTime t1(st);
		CString createtime = t1.Format("%B %d, %Y, %H:%M:%S");
		SetDlgItemText(IDC_STATIC_STATCREATETIME,createtime);
	}
	
	// MODIFYTIME
    FileTimeToLocalFileTime ( &attr.ftLastWriteTime, &ftLocal );
    FileTimeToSystemTime ( &ftLocal, &st );
	
	if( LANG_CHINESE == id )
	{
		char modifytime[128];
		sprintf(modifytime,"%d%s%d%s%d%s, %d:%d:%d",(int)st.wYear,(const char*)(LPCTSTR)StrFromResID(IDS_YEAR),(int)st.wMonth,
			(const char*)(LPCTSTR)StrFromResID(IDS_MONTH),(int)st.wDay,(const char*)(LPCTSTR)StrFromResID(IDS_DAY),(int)st.wHour,(int)st.wMinute,(int)st.wSecond);
		SetDlgItemText(IDC_STATIC_STATMODIFYTIME,modifytime);
	}
	else
	{
		CTime t2(st);
		CString modifytime = t2.Format("%B %d, %Y, %H:%M:%S");
		SetDlgItemText(IDC_STATIC_STATMODIFYTIME,modifytime);
	}
	
	// Editor
	CString name, strEditor, type;
	pDS->GetDataSettings(FIELDNAME_STATEDITOR,name,strEditor,type);
	SetDlgItemText(IDC_STATIC_EDITOR,strEditor);

	// EditNum
	CString strEditNum;
	pDS->GetDataSettings(FIELDNAME_STATEDITNUM,name,strEditNum,type);
	SetDlgItemText(IDC_STATIC_EDITSUM,strEditNum);

	// EditTime
	CString strEditTime;
	pDS->GetDataSettings(FIELDNAME_STATEDITTIME,name,strEditTime,type);
	CString str;
	if (strEditTime.IsEmpty())
	{
		str.Format("0 %s",StrFromResID(IDS_MINUTE));
	}
	else
	{
		str.Format("%s %s",strEditTime,StrFromResID(IDS_MINUTE));
	}
	SetDlgItemText(IDC_STATIC_EDITTIME,str);

	return TRUE;
}