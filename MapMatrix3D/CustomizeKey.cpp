// CustomizeKey.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "CustomizeKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomizeKey dialog

CCustomizeKey::CCustomizeKey(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomizeKey::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomizeKey)
		// NOTE: the ClassWizard will add member initialization here
		m_CurCommandName = _T("");
		m_CurCommandAccel = _T("");
	//}}AFX_DATA_INIT
}


void CCustomizeKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomizeKey)
	DDX_Control(pDX, IDC_CUSTOM1, m_gridCtrl);
	//}}AFX_DATA_MAP
}

BOOL CCustomizeKey::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return FALSE;
	return CDialog::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CCustomizeKey, CDialog)
	//{{AFX_MSG_MAP(CCustomizeKey)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomizeKey message handlers
BOOL CCustomizeKey::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_gridCtrl.SetFixedRowCount(1);//一行标题栏
	m_gridCtrl.SetFixedColumnCount(2);
	m_gridCtrl.SetColumnWidth(0, 0);
	m_gridCtrl.SetBkColor(RGB(255, 255, 255));
	m_gridCtrl.SetColumnCount(3);

	CRect rc;
	m_gridCtrl.GetClientRect(&rc);
	m_gridCtrl.SetColumnWidth(0, rc.Width()/3);

	m_gridCtrl.SetItemText(0, 0, StrFromResID(IDS_OUTPUT_CMDPROMPT));
	m_gridCtrl.SetItemText(0, 1, StrFromResID(IDS_TITLE_ACCEL));
	m_gridCtrl.SetItemText(0, 2, StrFromResID(IDS_TITLE_CUSTOMIZE_ACCEL));

	m_keyfilepath = GetConfigPath(TRUE) + "\\CustomizeKey.txt";
	LoadCustomizeKey();
	m_gridCtrl.AutoSizeColumns();
	m_gridCtrl.ExpandLastColumn();
	m_gridCtrl.SortItems(1, TRUE);

	//选中当前命令
	if(!m_CurCommandName.IsEmpty())
	{
		for (int i=1; i<m_gridCtrl.GetRowCount(); i++)
		{
			if( m_CurCommandName == m_gridCtrl.GetItemText(i, 0) )
			{
				int nState = m_gridCtrl.GetItemState(i, 2);
				m_gridCtrl.SetItemState(i, 2, nState|GVIS_SELECTED);
				m_gridCtrl.SetFocusCell(i, 2);
				m_gridCtrl.EnsureVisible(i, 0);
				break;
			}
		}
		if(i>=m_gridCtrl.GetRowCount())
		{
			int row = m_gridCtrl.InsertRow("");
			m_gridCtrl.SetItemText(row, 0, m_CurCommandName);
			m_gridCtrl.SetItemText(row, 1, m_CurCommandAccel);
			int nState = m_gridCtrl.GetItemState(row, 2);
			m_gridCtrl.SetItemState(row, 2, nState|GVIS_SELECTED);
			m_gridCtrl.SetFocusCell(row, 2);
			m_gridCtrl.EnsureVisible(row, 0);
			SaveCustomizeKey();
		}
	}
	m_gridCtrl.SetModified(FALSE);
	m_gridCtrl.AutoSizeRows();//自动调整行高度
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomizeKey::LoadCustomizeKey()
{
	FILE *fp = fopen(m_keyfilepath, "rt");
	if(!fp) return;
	
	char line[1024];
	char szName[512];
	char szAccel[256];
	char szCskey[255];
	int id=0;
	while(!feof(fp))
	{
		memset(line,0,sizeof(line));
		memset(szName,0,sizeof(szName));
		memset(szAccel,0,sizeof(szAccel));
		memset(szCskey,0,sizeof(szCskey));
		fgets(line,sizeof(line)-1,fp);
		if(strlen(line)<=0)  continue;

		char *temp = strchr(line, '>');
		if(temp==NULL)
			continue;

		int length = temp-line;
		strncpy(szName, line+1, length-1);
		
		if (sscanf(temp+1,"%s%s", szAccel, szCskey)>0)
		{
			int row = m_gridCtrl.InsertRow("");
			m_gridCtrl.SetItemText(row, 0, szName);
			m_gridCtrl.SetItemText(row, 1, szAccel);
			m_gridCtrl.SetItemText(row, 2, szCskey);
		}
	}
	fclose(fp);
	fp=NULL;
	m_gridCtrl.SetModified(FALSE);
	m_gridCtrl.AutoSizeRows();
}

void CCustomizeKey::SaveCustomizeKey()
{
	FILE *fp = fopen(m_keyfilepath, "wt");
	if(!fp) return;

	for (int i=1; i<m_gridCtrl.GetRowCount(); i++)
	{
		CString Name = m_gridCtrl.GetItemText(i, 0);
		CString Accel = m_gridCtrl.GetItemText(i, 1);
		CString Cskey = m_gridCtrl.GetItemText(i, 2);

		Name = _T("<") + Name + _T(">");
		fprintf(fp, "%-60s %-40s %s\n", Name, Accel, Cskey);
	}
	
	fclose(fp);
	fp=NULL;
}

void CCustomizeKey::OnOK() 
{
	// TODO: Add extra validation here
	SaveCustomizeKey();
	CDialog::OnOK();
}

void CCustomizeKey::OnCancel() 
{
	// TODO: Add extra cleanup here
	CString szText,szCaption;
	bool bModified = m_gridCtrl.GetModified();
	int nResult;
	if (bModified)
	{
		szText.LoadString(IDS_SAVETIP);
		nResult = MessageBox(szText,szCaption,MB_YESNOCANCEL|MB_ICONASTERISK);
	}
	else
	{
		szText.LoadString(IDS_CONFIRM_CLOSE);
		nResult = MessageBox(szText,szCaption,MB_OKCANCEL|MB_ICONASTERISK);
	}
	
	if (nResult==IDCANCEL)
    {
		return;
    }
	else if(nResult==IDYES)
	{
		SaveCustomizeKey();
		CDialog::OnOK();
	}
	else
	{
		CDialog::OnCancel();
	}
}

void CCustomizeKey::OnButton1() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(this->GetSafeHwnd(),"open","notepad.exe",m_keyfilepath,NULL,SW_NORMAL);
}

void CCustomizeKey::OnButton2() 
{
	// TODO: Add your control notification handler code here
	for (int i=m_gridCtrl.GetRowCount()-1; i>=1; i--)
	{
		m_gridCtrl.DeleteRow(i);
	}
	LoadCustomizeKey();
	m_gridCtrl.Refresh();
}
