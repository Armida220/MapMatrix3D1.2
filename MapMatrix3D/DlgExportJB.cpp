// DlgExportJB.cpp : implementation file
//

#include "stdafx.h"
#include "editBase.h"
#include "DlgExportJB.h"
#include "SmartViewFunctions.h"
#include "UIFFileDialogEx.h"
#include "DlgMetaInput.h"

#include "EditbaseDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CString lstFile = _T("");
static CString strPath = _T("");
static BOOL    bInit   = FALSE;

extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
						   LPTSTR lpszPath,		// [out] 返回的文件路径
						   LPCTSTR lpszInitDir,	// [in] 初始文件路径
						   HWND hWnd,				// [in] 父系窗口
						   BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
						   UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] 设置风格


/////////////////////////////////////////////////////////////////////////////
// CDlgExportJB dialog


CDlgExportJB::CDlgExportJB(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportJB::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportJB)
	m_strFilePath = _T("");
	m_strLstFile = _T("");
	//}}AFX_DATA_INIT

	m_pDoc = NULL;
}


void CDlgExportJB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportJB)
	DDX_Text(pDX, IDC_EDIT_IC_FILEPATH, m_strFilePath);
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH2, m_strLstFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportJB, CDialog)
	//{{AFX_MSG_MAP(CDlgExportJB)
	ON_BN_CLICKED(IDC_BUTTON_IC_BROWSE, OnButtonIcBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE2, OnButtonIdBrowse2)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE3, OnButtonIdBrowse3)
	ON_EN_SETFOCUS(IDC_EDIT_ID_FILEPATH2, OnSetfocusEditIdFilepath2)
	ON_EN_KILLFOCUS(IDC_EDIT_ID_FILEPATH2, OnKillfocusEditIdFilepath2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportJB message handlers

void CDlgExportJB::OnButtonIcBrowse() 
{
	CString retPath;
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),retPath.GetBuffer(256),NULL,GetSafeHwnd(),FALSE,BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS|0x40) )
		return;
	
	UpdateData(TRUE);
	m_strFilePath = retPath;
	UpdateData(FALSE);
}

void CDlgExportJB::OnButtonIdBrowse2() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);		
}


void CDlgExportJB::OnButtonIdBrowse3()
{
	CJBMetaData data;
	CDlgMetaInput dlg;
	
	data.LoadDefines();
	data.SetValuesFromDoc(m_pDoc);
	
	CString dataPath = m_pDoc->GetPathName();
	dataPath += ".met";
	data.LoadValues(dataPath);
	
	dlg.m_pData = &data;
	
	if( dlg.DoModal()!=IDOK )
		return;
	
	data.SaveValues(dataPath);
}

void CDlgExportJB::OnOK() 
{
	UpdateData(TRUE);
	
	lstFile = m_strLstFile;
	strPath = m_strFilePath;
	
	GetValues();
	
	CDialog::OnOK();
	
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");
}

BOOL CDlgExportJB::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, CRect(), this, 2))
	{
		TRACE0("Failed to create Properies Grid \n");
		return -1;      // fail to create
	}
	
	CWnd *pWnd = GetDlgItem(IDC_STATIC_LISTPOS);
	if( pWnd!=NULL )
	{
		CRect rcWnd;
		pWnd->GetWindowRect(&rcWnd);
		ScreenToClient(&rcWnd);
		
		m_wndPropList.SetWindowPos(NULL,rcWnd.left,rcWnd.top,
			rcWnd.Width(),rcWnd.Height(),0);
	}

	m_wndPropList.EnableHeaderCtrl(TRUE,StrFromResID(IDS_NAME0),StrFromResID(IDS_VALUE));
	
	FillList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CDlgExportJB::FillList()
{
	m_wndPropList.RemoveAll();
	
	int nField = m_dat.GetFieldCount(), type;
	_variant_t var;
	const CVariantEx *pv;
	CString field,name;
	for( int i=0; i<nField; i++)
	{
		m_dat.GetField(i,field,type,name);
		if( m_dat.GetValue(0,i,pv) )
		{
			var = pv->m_variant;
		}
		COleVariant var1(var);
		CUIFProp *pProp = new CUIFProp(field, var1);
		m_wndPropList.AddProperty(pProp,FALSE);
	}
	
	m_wndPropList.RedrawWindow();
}


void CDlgExportJB::GetValues()
{
	int nField = m_dat.GetFieldCount(), type;
	_variant_t var;
	CString field,name;
	for( int i=0; i<nField; i++)
	{
		m_dat.GetField(i,field,type,name);
		
		CUIFProp *pProp = m_wndPropList.GetProperty(i);
		var = pProp->GetValue();
		m_dat.SetValue(0,field,&CVariantEx(var));
	}
	
}

void CDlgExportJB::OnSetfocusEditIdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgExportJB::OnKillfocusEditIdFilepath2() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}	
	
}
