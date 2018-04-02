// DlgPluginsManage.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgPluginsManage.h"
#include "DlgDataSource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define IDC_LM_PROPLIST			1001
/////////////////////////////////////////////////////////////////////////////
// CDlgPluginsManage dialog

CDlgPluginsManage::CDlgPluginsManage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPluginsManage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPluginsManage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPlugItem = (PlugItem*)((CEditBaseApp*)AfxGetApp())->GetPlugins(m_PlugSize);
}


void CDlgPluginsManage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPluginsManage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPluginsManage, CDialog)
	//{{AFX_MSG_MAP(CDlgPluginsManage)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_SIZE()
		ON_MESSAGE(WM_SEL_CHANGED,OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPluginsManage message handlers

void CDlgPluginsManage::AdjustLayout()
{
	if (GetSafeHwnd () == NULL)
		return;
	
	CRect rectClient;
	GetClientRect (rectClient);

	CWnd *pWnd = GetDlgItem(IDOK);
	CRect rectOK;
	if (pWnd)
	{
		pWnd->GetWindowRect (&rectOK);
		ScreenToClient(&rectOK);
	}
	
	m_wndPluginsList.SetWindowPos(NULL,rectClient.left,rectClient.top,rectClient.Width(),rectOK.top-5/*rectClient.Height()*/,SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDlgPluginsManage::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	AdjustLayout();	
}

BOOL CDlgPluginsManage::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT rt;
	::SetRect(&rt,0,0,0,0);
	
	if (!m_wndPluginsList.Create(WS_VISIBLE|WS_CHILD|WS_BORDER,rt,this,IDC_LM_PROPLIST))
		return FALSE;

	AdjustLayout();

	FillPluginsList();

	return TRUE;
}

void CDlgPluginsManage::FillPluginsList()
{
	m_wndPluginsList.RemoveAll();
	
	CLVLPropColumn0 col;
	strcpy(col.FieldName,FIELDNAME_PLUGINNAME);
	strcpy(col.ColumnName,StrFromResID(IDS_PLUGINNAME));
	col.ValueType = VT_BSTR;
	col.Type = CLVLPropColumn0::EDIT;
	col.bReadOnly = TRUE;
	m_wndPluginsList.AddColumn(col);
	
	strcpy(col.FieldName,FIELDNAME_PLUGINUSEState);
	strcpy(col.ColumnName,StrFromResID(IDS_PLUGINUSE));
	col.ValueType = VT_BOOL;
	col.Type = CLVLPropColumn0::CHECK;
	col.BitmapOffset = 1;
	col.bReadOnly = FALSE;
	m_wndPluginsList.AddColumn(col);

	m_wndPluginsList.InitHeaderCtrl();
	
	CHeaderCtrl& header = m_wndPluginsList.GetHeaderCtrl();
	for(int i=0; i<header.GetItemCount(); i++)
	{
		HDITEM hitem;
		hitem.mask = HDI_WIDTH;
		hitem.cxy = 100;
		header.SetItem(i,&hitem);
	}

	CLVLPropItem0 *pItem;
	for (i=0; i<m_PlugSize; i++)
	{
		pItem = new CLVLPropItem0;
		if( !pItem )continue;
		
		m_wndPluginsList.AddItem(pItem);

		pItem->SetValue((_variant_t)(const char*)((m_pPlugItem+i)->pObj->GetPlugName()),0);
		pItem->SetValue((_variant_t)(bool)((m_pPlugItem+i)->bUsed),1);
		pItem->SetData(DWORD_PTR(m_pPlugItem+i));
	}

	m_wndPluginsList.FilterPropItems();
}

void CDlgPluginsManage::OnCancel()
{	
	EndDialog(IDNO);
}

void CDlgPluginsManage::OnOK()
{
	int size = m_wndPluginsList.GetItemCount();
	for (int i=0; i<size; i++)
	{
		CLVLPropItem0 *pItem = m_wndPluginsList.GetPropItem(i);
		if (!pItem) continue;

		PlugItem *pPlugItem = (PlugItem*)pItem->GetData();
		if (pPlugItem)
		{
			bool bUsed = (bool)pItem->GetValue(1);
			if (pPlugItem->bUsed^bUsed)
			{
				pPlugItem->bUsed = bUsed;
				AfxGetApp()->WriteProfileInt(PLUGIN_SECTION,pPlugItem->pObj->GetPlugName(),bUsed);
			}
		}
	}
	CDialog::OnOK();
}

LRESULT CDlgPluginsManage::OnSelChange(WPARAM   wParam,LPARAM   lParam)
{
	return 0;
}
