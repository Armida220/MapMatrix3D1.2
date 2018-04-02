// CombinDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "CombinDlg.h"
#include "SymbolLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCombinDlg dialog


CCombinDlg::CCombinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCombinDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCombinDlg)
	m_strPath1 = _T("");
	m_strPath2 = _T("");
	//}}AFX_DATA_INIT
}


void CCombinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCombinDlg)
	DDX_Text(pDX, IDC_EDIT_PATH1, m_strPath1);
	DDX_Text(pDX, IDC_EDIT_PATH2, m_strPath2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCombinDlg, CDialog)
	//{{AFX_MSG_MAP(CCombinDlg)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON1, OnBrowseButton1)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON2, OnBrowseButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCombinDlg message handlers
extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
						   LPTSTR lpszPath,		// [out] 返回的文件路径
						   LPCTSTR lpszInitDir,	// [in] 初始文件路径
						   HWND hWnd,				// [in] 父系窗口
						   BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
						   UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] 设置风格


void CCombinDlg::OnBrowseButton1() 
{
	// TODO: Add your control notification handler code here
	char szPath[MAX_PATH];
	CString strInitDir = GetConfigPath();
	int len = strInitDir.GetLength();
	if( len>0 && strInitDir[len-1]=='\\' )
		strInitDir.SetAt(len-1,0);
	
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_CATALOG),szPath,strInitDir,GetSafeHwnd(),
		FALSE,BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | 0x0040/*BIF_NEWDIALOGSTYLE*/) )
		return;
	
	m_strPath1 = szPath;
	
	UpdateData(FALSE);
}

void CCombinDlg::OnBrowseButton2() 
{
	// TODO: Add your control notification handler code here
	char szPath[MAX_PATH];
	CString strInitDir = GetConfigPath();
	int len = strInitDir.GetLength();
	if( len>0 && strInitDir[len-1]=='\\' )
		strInitDir.SetAt(len-1,0);
	
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_CATALOG),szPath,strInitDir,GetSafeHwnd(),
		FALSE,BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | 0x0040/*BIF_NEWDIALOGSTYLE*/) )
		return;
	
	m_strPath2 = szPath;
	
	UpdateData(FALSE);
}

void CCombinDlg::OnOK() 
{
	// TODO: Add extra validation here
	ConfigLibItem item1, item2, item3;
	item1.SetScalePath(m_strPath1);
// 	item2.SetScalePath(m_strPath1+"\\500(彭)");
// 	item3.SetScalePath(m_strPath1+"\\500(h)");
	if (!item1.Load()/* || !item2.Load() || !item3.Load()*/)  return;

	CScheme *pScheme1, *pScheme2, *pScheme3;

	pScheme1 = item1.pScheme;
	pScheme2 = item2.pScheme;
	pScheme3 = item3.pScheme;

	for (int i=pScheme1->GetLayerDefineCount()-1; i>=0; i--)
	{
		CSchemeLayerDefine *pLayer = pScheme1->GetLayerDefine(i);
		if (!pLayer) continue;

		if (pLayer->GetGeoClass() == CLS_GEOSURFACE)
		{
			pLayer->SetGeoClass(CLS_GEOCURVE);
		}

// 		CString strGroupName = pLayer->GetGroupName();
// 
// 		if (strGroupName.CompareNoCase("辅助特征") == 0 || strGroupName.CompareNoCase("1测量控制") == 0 
// 			|| strGroupName.CompareNoCase("2居民地") == 0 || strGroupName.CompareNoCase("3工矿和公共") == 0)
// 		{
// 			continue;
// 		}
// 
// 		pScheme1->DelLayerDefine(i);

	}

// 	for (i=0; i<pScheme2->GetLayerDefineCount(); i++)
// 	{
// 		CSchemeLayerDefine *pLayer = pScheme2->GetLayerDefine(i);
// 		if (!pLayer) continue;
// 
// 		CString strGroupName = pLayer->GetGroupName();
// 
// 		if (strGroupName.CompareNoCase("4道路和附属") == 0 || strGroupName.CompareNoCase("5管线墙栅栏") == 0 
// 			|| strGroupName.CompareNoCase("6水系和附属") == 0)
// 		{
// 			pScheme1->AddLayerDefine(pLayer);
// 		}
// 	}
// 
// 	for (i=0; i<pScheme3->GetLayerDefineCount(); i++)
// 	{
// 		CSchemeLayerDefine *pLayer = pScheme3->GetLayerDefine(i);
// 		if (!pLayer) continue;
// 		
// 		CString strGroupName = pLayer->GetGroupName();
// 		
// 		if (strGroupName.CompareNoCase("7境界") == 0 || strGroupName.CompareNoCase("8地貌和土质") == 0 
// 			|| strGroupName.CompareNoCase("9植被") == 0)
// 		{
// 			pScheme1->AddLayerDefine(pLayer);
// 		}
// 	}


// 	for (int i=0; i<pScheme2->GetLayerDefineCount(); i++)
// 	{
// 		CSchemeLayerDefine *pLayer = pScheme2->GetLayerDefine(i);
// 		if (!pLayer) continue;
// 		
// 		long code = pLayer->GetLayerCode();
// 		const char *p = NULL;
// 		if (code > 9999999 || (code > 100 && code < 9999999 && !pScheme1->FindLayerIdx(TRUE,code,p)))
// 		{
// 			pScheme1->AddLayerDefine(pLayer);
// 
// 			for (int j=0; j<pScheme1->m_strSortedGroupName.GetSize(); j++)
// 			{
// 				if (pScheme1->m_strSortedGroupName.GetAt(j).CompareNoCase(pLayer->GetGroupName()) == 0)
// 				{
// 					break;
// 				}
// 			}
// 
// 			if (j > pScheme1->m_strSortedGroupName.GetSize()-1)
// 			{
// 				pScheme1->m_strSortedGroupName.Add(pLayer->GetGroupName());
// 			}
// 
// 
// 		}
// 	}

// 	for (i=pScheme1->GetLayerDefineCount()-1; i>=0; i--)
// 	{
// 		CSchemeLayerDefine *pLayer = pScheme1->GetLayerDefine(i);
// 		if (!pLayer) continue;
// 
// 		long code = pLayer->GetLayerCode();
// 		if (code > 100 && code < 9999999 && pLayer->GetGroupName().CompareNoCase("4道路和附属") == 0)
// 		{
// 			pScheme1->DelLayerDefine(i);
// 		}
// 	}

	pScheme1->Save();

	MessageBox(StrFromResID(IDS_COMBINE_OK));

	CDialog::OnOK();
}
