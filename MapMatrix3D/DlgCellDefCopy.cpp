// DlgCellDefCopy.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgCellDefCopy.h"
#include "DlgCheckList.h"

#include "UIFBoolProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCellDefCopy dialog


CDlgCellDefCopy::CDlgCellDefCopy(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCellDefCopy::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCellDefCopy)
	m_strCellDefName = _T("");
	m_strLTName = _T("");
	m_strPath1 = _T("");
	m_strPath2 = _T("");
	m_bCheckCover = TRUE;
	//}}AFX_DATA_INIT
}


void CDlgCellDefCopy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCellDefCopy)
	DDX_Text(pDX, IDC_EDIT_CELLNAME, m_strCellDefName);
	DDX_Text(pDX, IDC_EDIT_LTNAME, m_strLTName);
	DDX_Text(pDX, IDC_EDIT_PATH1, m_strPath1);
	DDX_Text(pDX, IDC_EDIT_PATH2, m_strPath2);
	DDX_Check(pDX, IDC_CHECK_COVER, m_bCheckCover);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCellDefCopy, CDialog)
	//{{AFX_MSG_MAP(CDlgCellDefCopy)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE1, OnButtonBrowse1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE2, OnButtonBrowse2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE3, OnButtonBrowse3)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE4, OnButtonBrowse4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCellDefCopy message handlers

extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
						   LPTSTR lpszPath,		// [out] 返回的文件路径
						   LPCTSTR lpszInitDir,	// [in] 初始文件路径
						   HWND hWnd,				// [in] 父系窗口
						   BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
					UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] 设置风格

void CDlgCellDefCopy::OnButtonBrowse1() 
{
	UpdateData(TRUE);
	char szpath[512];
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),szpath,m_strPath1,m_hWnd) )
	{
		return;
	}
	m_strPath1 = szpath;
	
	m_lib1.Clear();
	m_lib1.Create();
	m_lib1.SetScalePath(m_strPath1);
	if( !m_lib1.Load() )
	{
		AfxMessageBox(IDS_INVALID_SYMLIBPATH);
	}

	UpdateData(FALSE);
	
}

void CDlgCellDefCopy::OnButtonBrowse2() 
{
	UpdateData(TRUE);

	char szpath[512];
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),szpath,m_strPath1,m_hWnd) )
	{
		return;
	}
	
	m_strPath2 = szpath;

	m_lib2.Clear();
	m_lib2.Create();
	m_lib2.SetScalePath(m_strPath2);
	if( !m_lib2.Load() )
	{
		AfxMessageBox(IDS_INVALID_SYMLIBPATH);
	}
	UpdateData(FALSE);
	
}

void CDlgCellDefCopy::OnButtonBrowse3() 
{
	UpdateData(TRUE);
	{
		m_lib1.Clear();
		m_lib1.Create();
		m_lib1.SetScalePath(m_strPath1);
		m_lib1.Load();
		m_lib2.Clear();
		m_lib2.Create();
		m_lib2.SetScalePath(m_strPath2);
		m_lib2.Load();
	}

	CDlgCheckList dlg;

	if( m_lib1.bLoaded )
	{
		int nCount = m_lib1.pCellDefLib->GetCellDefCount();
		for( int i=0; i<nCount; i++)
		{
			if( m_lib1.pCellDefLib->GetCellDef(i).m_nSourceID==0 )
			{
				dlg.m_arrStrList.Add(m_lib1.pCellDefLib->GetCellDef(i).m_name);
				dlg.m_arrFlags.Add(1);
			}
		}
	}

	if( dlg.DoModal()!=IDOK )
		return;

	if( m_lib1.bLoaded )
	{
		m_strCellDefName.Empty();

		for( int i=0; i<dlg.m_arrFlags.GetSize(); i++)
		{
			if( dlg.m_arrFlags[i]==0 )
				continue;

			if( i!=0 )
				m_strCellDefName += _T(",");

			m_strCellDefName += dlg.m_arrStrList[i];
		}
	}
	
	UpdateData(FALSE);
}

void CDlgCellDefCopy::OnButtonBrowse4() 
{
	CDlgCheckList dlg;

	UpdateData(TRUE);

	{
		m_lib1.Clear();
		m_lib1.Create();
		m_lib1.SetScalePath(m_strPath1);
		m_lib1.Load();
		m_lib2.Clear();
		m_lib2.Create();
		m_lib2.SetScalePath(m_strPath2);
		m_lib2.Load();
	}
	
	if( m_lib1.bLoaded )
	{
		int nCount = m_lib1.pLinetypeLib->GetBaseLineTypeCount();
		for( int i=0; i<nCount; i++)
		{
			if( m_lib1.pLinetypeLib->GetBaseLineType(i).m_nSourceID==0 )
			{
				dlg.m_arrStrList.Add(m_lib1.pLinetypeLib->GetBaseLineType(i).m_name);
				dlg.m_arrFlags.Add(1);
			}
		}
	}
	
	if( dlg.DoModal()!=IDOK )
		return;
	
	if( m_lib1.bLoaded )
	{
		m_strLTName.Empty();
		
		for( int i=0; i<dlg.m_arrFlags.GetSize(); i++)
		{
			if( dlg.m_arrFlags[i]==0 )
				continue;
			
			if( i!=0 )
				m_strLTName += _T(",");
			
			m_strLTName += dlg.m_arrStrList[i];
		}
	}
	
	UpdateData(FALSE);
	
}


void CDlgCellDefCopy::OnOK() 
{
	UpdateData(TRUE);
	if( !m_lib1.bLoaded )
	{
		m_lib1.Clear();
		m_lib1.Create();
		m_lib1.SetScalePath(m_strPath1);
		m_lib1.Load();
	}

	if( !m_lib2.bLoaded )
	{
		m_lib2.Clear();
		m_lib2.Create();
		m_lib2.SetScalePath(m_strPath2);
		m_lib2.Load();
	}

	if( m_lib1.bLoaded && m_lib2.bLoaded )
	{
		BOOL bEmpty = m_strCellDefName.IsEmpty();
		int nCount = m_lib1.pCellDefLib->GetCellDefCount();
		for( int i=0; i<nCount; i++)
		{
			if( m_lib1.pCellDefLib->GetCellDef(i).m_nSourceID==0 )
			{
				CellDef def = m_lib1.pCellDefLib->GetCellDef(i);
				CellDef def_new;				

				CString name = def.m_name;
				if( bEmpty || CUIFCheckListProp::StringFindItem(m_strCellDefName,name) )
				{
					int index = m_lib2.pCellDefLib->GetCellDefIndex(name);
					if( index>=0 )
					{
						if( m_bCheckCover )
						{
							def_new.CopyFrom(def);
							m_lib2.pCellDefLib->SetCellDef(index,def_new);
						}
					}
					else
					{
						def_new.CopyFrom(def);
						m_lib2.pCellDefLib->AddCellDef(def_new);
					}
				}
			}
		}

		bEmpty = m_strLTName.IsEmpty();
		nCount = m_lib1.pLinetypeLib->GetBaseLineTypeCount();
		for( i=0; i<nCount; i++)
		{
			if( m_lib1.pLinetypeLib->GetBaseLineType(i).m_nSourceID==0 )
			{
				BaseLineType def = m_lib1.pLinetypeLib->GetBaseLineType(i);
				BaseLineType def_new = def;				
				
				CString name = def.m_name;
				if( bEmpty || CUIFCheckListProp::StringFindItem(m_strLTName,name) )
				{
					int index = m_lib2.pLinetypeLib->GetBaseLineTypeIndex(name);
					if( index>=0 )
					{
						if( m_bCheckCover )
						{
							m_lib2.pLinetypeLib->SetBaseLineType(index,def);
						}
					}
					else
					{
						m_lib2.pLinetypeLib->AddBaseLineType(def);
					}
				}
			}
		}

		m_lib2.SaveCellLine();

		CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
		pApp->ReloadConfig();

		AfxMessageBox(IDS_COPYOK);
	}
	else
	{
		AfxMessageBox(IDS_FAILLOAD_SYMLIB);
	}
	
	CDialog::OnOK();
}

void CDlgCellDefCopy::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CDlgCellDefCopy::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_strPath1 = GetConfigLibManager()->GetPath();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
