// DlgConnectDB.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgConnectDB.h"
#include "UVSModify.h"
#include "MainFrm.h"
#include "RegDef2.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL bOpendUVS = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CDlgOpenWP dialog

CDlgOpenWP::CDlgOpenWP(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOpenWP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOpenWP)
	// NOTE: the ClassWizard will add member initialization here
	char logPath[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, logPath, _MAX_FNAME);
	char *pos;
	if((pos = strrchr(logPath, '\\')))*pos = '\0';
	if((pos = strrchr(logPath, '\\')))*pos = '\0';
	strcat(logPath, "\\config\\login.bin");

	char strServer[128] = { 0 };
	int nType;
	char strUser[128] = { 0 };
	char strPass[128] = { 0 };
	int nRemPass = 0;
	FILE *fp = fopen(logPath, "rb");
	if(fp)
	{
		fread(strServer, 1, 128, fp);
		fread(&nType, 4, 1, fp);
		fread(strUser, 1, 128, fp);
		fread(strPass, 1, 128, fp);
		fread(&nRemPass, 4, 1, fp);
		fclose(fp);
	}
	m_username = strUser;
	m_password = strPass;
	//}}AFX_DATA_INIT
}


void CDlgOpenWP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOpenWP)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_EDIT_DBUSER, m_username);
	DDX_Text(pDX, IDC_EDIT_DBPASSWORD, m_password);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOpenWP, CDialog)
	//{{AFX_MSG_MAP(CDlgOpenWP)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgOpenWP::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOpenWP::OnOK()
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileString("uvs", "username", m_username);
	AfxGetApp()->WriteProfileString("uvs", "password", m_password);

	CDialog::OnOK();
}

void CDlgOpenWP::OnCancel()
{
	// TODO: Add extra cleanup here

	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConnectDB dialog

CDlgConnectDB::CDlgConnectDB(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConnectDB::IDD, pParent),puvs(NULL)
{
	//{{AFX_DATA_INIT(CDlgConnectDB)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgConnectDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConnectDB)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_EDIT_CURRENT_WP, m_current_wp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConnectDB, CDialog)
	//{{AFX_MSG_MAP(CDlgConnectDB)
	ON_LBN_DBLCLK(IDC_LIST_WORKSPACE, OnDblclkWorkspace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConnectDB message handlers

void CDlgConnectDB::OnDblclkWorkspace() 
{
	// TODO: Add your control notification handler code here
	int index = ((CListBox *)GetDlgItem(IDC_LIST_WORKSPACE))->GetCurSel();
	CString workspace_name;
    ((CListBox *)GetDlgItem(IDC_LIST_WORKSPACE))->GetText(index,workspace_name);
	GetDlgItem(IDC_EDIT_CURRENT_WP)->SetWindowText(workspace_name);
}

BOOL CDlgConnectDB::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!ConnectDB())
	{
		CDialog::OnCancel();
		return FALSE;
	}

	CStringArray wp_list;
	CUVSModify::EnumerateWorkspace(wp_list);
	for(int i = 0; i<wp_list.GetSize(); ++i)
	{
		((CListBox *)GetDlgItem(IDC_LIST_WORKSPACE))->AddString(wp_list[i]);
	}

	CString current_workspace = CUVSModify::GetCurrentWorkspace();
	GetDlgItem(IDC_EDIT_CURRENT_WP)->SetWindowText(current_workspace);

	if (bOpendUVS)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConnectDB::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	if (!OpenWorkSpace())
	{
		return;
	}

	bOpendUVS = TRUE;

	CDialog::OnOK();
}

void CDlgConnectDB::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CDlgConnectDB::ConnectDB()
{
	if (!CheckLicense(86))
	{
		GOutPut(StrFromResID(IDS_ERR_INVALIDLICENSE));
		return FALSE;
	}

	CString inipath = GetConfigPath(TRUE);
	inipath += "\\NetWork.ini";

	char uvs_ip[64] = { 0 };
	char uvs_port[16] = { 0 };
	GetPrivateProfileString("uvs", "uvs_ip", "", uvs_ip, 64, inipath);
	GetPrivateProfileString("uvs", "uvs_port", "8888", uvs_port, 16, inipath);

	int port = atoi(uvs_port);

	if (CUVSModify::ConnectUVSServer(uvs_ip, port))
	{
		return TRUE;
	}

	COptionsDlg dlgOptions(StrFromResID(IDS_OPTIONS), NULL, 9);
	if (dlgOptions.DoModal() != IDOK)
	{
		return FALSE;
	}

	GetPrivateProfileString("uvs", "uvs_ip", "", uvs_ip, 64, inipath);
	GetPrivateProfileString("uvs", "uvs_port", "", uvs_port, 16, inipath);
	port = atoi(uvs_port);
	if (CUVSModify::ConnectUVSServer(uvs_ip, port))
	{
		return TRUE;
	}

	AfxMessageBox(IDS_CONN_UVS_FAILE);
	return FALSE;
}

BOOL CDlgConnectDB::OpenWorkSpace()
{
	CString username = AfxGetApp()->GetProfileString("uvs", "username", "");
	CString password = AfxGetApp()->GetProfileString("uvs", "password", "");

	if (!CUVSModify::OpenDB(m_current_wp, username, password, identity_id, identity_id2))
	{
		CDlgOpenWP dlg;
		if(dlg.DoModal()!= IDOK)
			return FALSE;
		if (!CUVSModify::OpenDB(m_current_wp, dlg.m_username, dlg.m_password, identity_id, identity_id2))
		{
			//AfxMessageBox(IDS_OPEN_UVSWP_FAILE);
			return FALSE;
		}
	}

	//WritePrivateProfileString("uvs", "uvs_id", identity_id, m_uvscfgpath);
	//WritePrivateProfileString("uvs", "uvs_id2", identity_id2, m_uvscfgpath);

	//设置延时保存
	BOOL bUseCache = AfxGetApp()->GetProfileInt(REGPATH_SAVE, _T("UseCache"), TRUE);
	CUVSModify::UpdateImmediately(!bUseCache);

	//设置当前符号库
	CString scale = CUVSModify::GetWorkspaceScale();
	int pos = scale.Find(':');
	if (pos > 0)
	{
		CString cfgPath = scale.Left(pos);
		CString curPath = AfxGetApp()->GetProfileString(REGPATH_SYMBOL, REGITEM_SYMPATH, cfgPath);
		curPath = curPath.Right(pos);
		if (0!=cfgPath.CompareNoCase(curPath))
		{
			CString msg = "Loading " + cfgPath;
			GOutPut(msg);
			cfgPath = GetConfigPath(TRUE) + "\\" + cfgPath;
			AfxGetApp()->WriteProfileString(REGPATH_SYMBOL, REGITEM_SYMPATH, cfgPath);
			theApp.ReloadConfig();
		}
	}

	return TRUE;
}
