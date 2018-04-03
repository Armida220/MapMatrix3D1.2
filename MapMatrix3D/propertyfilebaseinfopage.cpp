// propertyfilebaseinfopage.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "propertyfilebaseinfopage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyFileBaseInfoPage property page

IMPLEMENT_DYNCREATE(CPropertyFileBaseInfoPage, CPropertyPage)

CPropertyFileBaseInfoPage::CPropertyFileBaseInfoPage() : CPropertyPage(CPropertyFileBaseInfoPage::IDD)
{
	//{{AFX_DATA_INIT(CPropertyFileBaseInfoPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyFileBaseInfoPage::CPropertyFileBaseInfoPage(CDlgDoc *pDoc) : CPropertyPage(CPropertyFileBaseInfoPage::IDD)
{
	m_pDoc = pDoc;
}

CPropertyFileBaseInfoPage::~CPropertyFileBaseInfoPage()
{
}

void CPropertyFileBaseInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyFileBaseInfoPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CPropertyFileBaseInfoPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	if (!m_pDoc) return TRUE;

	CString filepath = m_pDoc->GetPathName();

	// FileName
	CString filename;
	int index = filepath.ReverseFind('\\');
	if (index >= 0)
	{
		filename = filepath.Right(filepath.GetLength()-index-1);
	}
	
	SetDlgItemText(IDC_STATIC_FILENAME,filename);

	// FileType
	SetDlgItemText(IDC_STATIC_FILETYPE,StrFromResID(IDR_MAINFRAME));

	// FileType
	SetDlgItemText(IDC_STATIC_FILELOCATION,filepath);

	WIN32_FILE_ATTRIBUTE_DATA attr;
	memset(&attr,0,sizeof(attr));
	GetFileAttributesEx(filepath,GetFileExInfoStandard,&attr);

	// FileSize
	char filesize[128];
	sprintf(filesize,"%d KB (%d,%d %s)",(int)ceil((double)attr.nFileSizeLow/1024),(int)attr.nFileSizeLow/1000,(int)(attr.nFileSizeLow-attr.nFileSizeLow/1000*1000),(const char*)(LPCTSTR)StrFromResID(IDS_BYTE));
	SetDlgItemText(IDC_STATIC_FILESIZE,filesize);

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
		SetDlgItemText(IDC_STATIC_CREATETIME,createtime);
	}
	else
	{
		CTime t1(st);
		CString createtime = t1.Format("%B %d, %Y, %H:%M:%S");
		SetDlgItemText(IDC_STATIC_CREATETIME,createtime);
	}
	

	// MODIFYTIME
    FileTimeToLocalFileTime ( &attr.ftLastWriteTime, &ftLocal );
    FileTimeToSystemTime ( &ftLocal, &st );

	if( LANG_CHINESE == id )
	{
		char modifytime[128];
		sprintf(modifytime,"%d%s%d%s%d%s, %d:%d:%d",(int)st.wYear,(const char*)(LPCTSTR)StrFromResID(IDS_YEAR),(int)st.wMonth,
			(const char*)(LPCTSTR)StrFromResID(IDS_MONTH),(int)st.wDay,(const char*)(LPCTSTR)StrFromResID(IDS_DAY),(int)st.wHour,(int)st.wMinute,(int)st.wSecond);
		SetDlgItemText(IDC_STATIC_MODIFYTIME,modifytime);
	}
	else
	{
		CTime t2(st);
		CString modifytime = t2.Format("%B %d, %Y, %H:%M:%S");
		SetDlgItemText(IDC_STATIC_MODIFYTIME,modifytime);
	}

	// ACCESSTIME
    FileTimeToLocalFileTime ( &attr.ftLastAccessTime, &ftLocal );
    FileTimeToSystemTime ( &ftLocal, &st );

	if( LANG_CHINESE == id )
	{
		char accesstime[128];
		sprintf(accesstime,"%d%s%d%s%d%s, %d:%d:%d",(int)st.wYear,(const char*)(LPCTSTR)StrFromResID(IDS_YEAR),(int)st.wMonth,
			(const char*)(LPCTSTR)StrFromResID(IDS_MONTH),(int)st.wDay,(const char*)(LPCTSTR)StrFromResID(IDS_DAY),(int)st.wHour,(int)st.wMinute,(int)st.wSecond);
		SetDlgItemText(IDC_STATIC_ACCESSTIME,accesstime);
	}
	else
	{
		CTime t3(st);
		CString accesstime = t3.Format("%B %d, %Y, %H:%M:%S");
		SetDlgItemText(IDC_STATIC_ACCESSTIME,accesstime);
	}

	// file save checked
	SendDlgItemMessage(IDC_CHECK_FILESAVE,BM_SETCHECK,(WPARAM)1,0);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CPropertyFileBaseInfoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertyFileBaseInfoPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyFileBaseInfoPage message handlers
