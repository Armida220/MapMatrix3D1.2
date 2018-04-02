// DlgImportCodeTable.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgImportCodeTable.h"
#include "SymbolLib.h "
#include "UIFFileDialogEx.h "
#include "SmartViewFunctions.h "
#include "DlgScheme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImportCodeTable dialog


CDlgImportCodeTable::CDlgImportCodeTable(CWnd* pParent /*=NULL*/, CConfigLibManager* pCon)
	: CDialog(CDlgImportCodeTable::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImportCodeTable)
		// NOTE: the ClassWizard will add member initialization here
	m_strFilePath = _T("");
	//}}AFX_DATA_INIT
	m_pData = NULL;
	SetConfigData(pCon);
}


void CDlgImportCodeTable::SetConfigData(CConfigLibManager* pCon)
{
	m_pData = pCon;
}

void CDlgImportCodeTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportCodeTable)
	DDX_Control(pDX, IDC_CODEMODE_COMBO, m_comBoxMode);
	DDX_Control(pDX, IDC_COMBO_SCALE, m_comBoxScale);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_strFilePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImportCodeTable, CDialog)
	//{{AFX_MSG_MAP(CDlgImportCodeTable)
		// NOTE: the ClassWizard will add message map macros here
		ON_BN_CLICKED(IDC_BUTTON_BROWER, OnButtonBrower)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImportCodeTable message handlers
void CDlgImportCodeTable::OnButtonBrower() 
{
	CString filter(StrFromResID(IDS_LOADTXT_FILTER2));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, _T(".txt"), m_strFilePath, OFN_HIDEREADONLY,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strFilePath = dlg.GetPathName();
	UpdateData(FALSE);	
}

BOOL CDlgImportCodeTable::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (!m_pData)return FALSE;
	int  nNum = m_pData->GetConfigLibCount();
	ConfigLibItem item;
	for (int i=0;i<nNum;i++)
	{
		item = m_pData->GetConfigLibItem(i);
		char buffer[20];
		_itoa(item.GetScale(), buffer, 10 );
		m_comBoxScale.AddString(buffer);
	}

	m_comBoxMode.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgImportCodeTable::OnOK() 
{
	//得到比例尺
	int i = m_comBoxScale.GetCurSel();
	if (i < 0) return;
	CString str;
	m_comBoxScale.GetLBText(i,str);
	DWORD scale;
	scale = atoi(LPCTSTR(str));
	CScheme *pSch = m_pData->GetScheme(scale);
	if (!pSch)return;

	int mode = m_comBoxMode.GetCurSel();
	
	FILE *stream;
	long code;
	char layName[64];
	CSchemeLayerDefine *pLayDef;
	if((stream = _tfopen(LPCTSTR(m_strFilePath), _T("r")))!=NULL ) 
	{
		int res = -1;

		// eg: 8110 首曲线
		while ((res=_ftscanf( stream, "%d %s", &code, layName )) == 2)
		{
			//层名
			if (mode == 0)
			{
				pLayDef = pSch->GetLayerDefine(layName);
				if (pLayDef)
				{
					pLayDef->SetLayerCode(code);
				}
			}
			else
			{
				pLayDef = pSch->GetLayerDefine(NULL,TRUE,code);
				if (pLayDef)
				{
					pLayDef->SetLayerName(layName);
				}
				
			}
		}
		
		// eg: 首曲线 8110
		if (res != EOF)
		{
			while ((res=_ftscanf(stream, "%s %d", layName, &code)) == 2)
			{
				//层名
				if (mode == 0)
				{
					pLayDef = pSch->GetLayerDefine(layName);
					if (pLayDef)
					{
						pLayDef->SetLayerCode(code);
					}
				}
				else
				{
					pLayDef = pSch->GetLayerDefine(NULL,TRUE,code);
					if (pLayDef)
					{
						pLayDef->SetLayerName(layName);
					}
					
				}
			}
		}

		fclose(stream);

		// 格式错误
		if (res != EOF)
		{
			AfxMessageBox(IDS_TIP_FILEFORMAT_ERROR);
		}
		else
		{
			pSch->Save();
			CDlgScheme *pScheme = (CDlgScheme*)GetParent();
			if (pScheme && scale==pScheme->m_config.GetScale())
			{
				pScheme->UpdateCodeOrNameOfCurScale();
			}
		}

		
	}
	else
		AfxMessageBox("The file  was not opened\n");
	CDialog::OnOK();
}
