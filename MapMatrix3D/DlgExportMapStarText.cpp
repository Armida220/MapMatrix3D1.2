// DlgExportMapStarText.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgExportMapStarText.h"
#include "SmartViewFunctions.h"
#include "UIFFileDialogEx.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportMapStarText dialog


CDlgExportMapStarText::CDlgExportMapStarText(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportMapStarText::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportMapStarText)
	m_strDir = _T("");
	m_strLstFile = _T("");
	m_fContourInterval = 5.0f;
	//}}AFX_DATA_INIT

	char path[_MAX_PATH]={0};
	GetModuleFileName(NULL,path,sizeof(path)-1);
	char *pos = strrchr(path,'\\');
	if( pos )
	{
		*pos = '\0';
		pos = strrchr(path,'\\');
		if( pos )
			sprintf(pos,"\\Config\\MapStarLayerCode.lst");
	}
	if( _access(path,0)!=-1 )
		m_strLstFile = path;
}


void CDlgExportMapStarText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportMapStarText)
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH, m_strDir);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strLstFile);
	DDX_Text(pDX, IDC_EDIT_CONTOURINTERVAL, m_fContourInterval);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportMapStarText, CDialog)
	//{{AFX_MSG_MAP(CDlgExportMapStarText)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE, OnButtonBrowse1)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnButtonBrowse2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportMapStarText message handlers

extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
						   LPTSTR lpszPath,		// [out] 返回的文件路径
						   LPCTSTR lpszInitDir,	// [in] 初始文件路径
						   HWND hWnd,				// [in] 父系窗口
						   BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
						   UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] 设置风格


void CDlgExportMapStarText::OnButtonBrowse1() 
{
	UpdateData(TRUE);
	char retPath[_MAX_PATH]={0};
	
	if( !BrowseFolderEx(StrFromResID(IDS_SYMLIB_DIR),retPath,m_strDir,m_hWnd,FALSE,BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS|0x40) )return;
		
	int str_len = strlen(retPath);
	if( str_len<=0 )return;
	if( retPath[str_len-1]=='\\')
	{
		retPath[str_len-1] = 0;
	}
	
	m_strDir = retPath;	
	UpdateData(FALSE);	
}

void CDlgExportMapStarText::OnButtonBrowse2() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);		
}
