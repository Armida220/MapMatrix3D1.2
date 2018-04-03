// DlgExportCodeTabTemp.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgExportCodeTabTemp.h"
#include "SymbolLib.h "
#include "SmartViewFunctions.h "


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportCodeTabTemp dialog


CDlgExportCodeTabTemp::CDlgExportCodeTabTemp(CWnd* pParent /*=NULL*/, CConfigLibManager* pCon)
	: CDialog(CDlgExportCodeTabTemp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportCodeTable)
	m_strFilePath = _T("");
	//}}AFX_DATA_INIT
	m_pData = NULL;
	SetConfigData(pCon);
}

void CDlgExportCodeTabTemp::SetConfigData(CConfigLibManager* pCon)
{
	m_pData = pCon;
}

void CDlgExportCodeTabTemp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportCodeTabTemp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_SCALE, m_comBoxScale);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_strFilePath);
}


BEGIN_MESSAGE_MAP(CDlgExportCodeTabTemp, CDialog)
	//{{AFX_MSG_MAP(CDlgExportCodeTabTemp)
	ON_BN_CLICKED(IDC_BUTTON_BROWER, OnButtonBrower)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportCodeTabTemp message handlers

void CDlgExportCodeTabTemp::OnButtonBrower() 
{
	CString filter(StrFromResID(IDS_LOADTXT_FILTER2));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, _T(".txt"), m_strFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strFilePath = dlg.GetPathName();
	UpdateData(FALSE);	
	
}

BOOL CDlgExportCodeTabTemp::OnInitDialog() 
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
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExportCodeTabTemp::OnOK() 
{
	//得到比例尺
	int i = m_comBoxScale.GetCurSel();
	CString str;
	m_comBoxScale.GetLBText(i,str);
	DWORD scale;
	scale = atoi(LPCTSTR(str));
	CScheme *pSch = m_pData->GetScheme(scale);
	if (!pSch)return;
	FILE *stream;
	__int64 code;
	CString layName;
	CString field,name;
	int type;
	CValueTable tab;
	CSchemeLayerDefine *pLayDef;
	if((stream = _tfopen(LPCTSTR(m_strFilePath), _T("w+")))!=NULL ) 
	{
		for (int i=0;i<pSch->GetLayerDefineCount();i++)
		{
			pLayDef = pSch->GetLayerDefine(i);
			if (!pLayDef)continue;
			code = pLayDef->GetLayerCode();
			layName = pLayDef->GetLayerName();
			_ftprintf(stream,_T("Layer   %32I64d%32I64d\n"),code,code);
			tab.DelAll();
			pLayDef->GetBasicAttributeDefaultValues(tab);//固有属性		
			for (int j=0;j<tab.GetFieldCount();j++)
			{
				tab.GetField(j,field,type,name);
				_ftprintf(stream,_T("Field   %32s%32s\n"),field,field);
			}
			int size;
			const XDefine* pXdef = pLayDef->GetXDefines(size);//扩展属性
			for (j=0;j<size;j++)
			{				
				_ftprintf(stream,_T("Field   %32s%32s\n"),pXdef[j].field,pXdef[j].field);
			}
			_ftprintf(stream,_T("\n"));
		}
		fclose(stream);
	}
	else
		AfxMessageBox("The file  was not opened\n");
	CDialog::OnOK();
}
