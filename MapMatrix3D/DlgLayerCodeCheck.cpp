// DlgLayerCodeCheck.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgLayerCodeCheck.h"
#include "DlgDataSource.h"
#include "SymbolLib.h"
#include "DlgSelectLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLayerCodeCheck dialog


CDlgLayerCodeCheck::CDlgLayerCodeCheck(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLayerCodeCheck::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLayerCodeCheck)
	m_bCheckAlllayers = FALSE;
	//}}AFX_DATA_INIT

	m_pDoc = NULL;
	m_bSortAscending = TRUE;
	m_nCurColumn = -1;
}


void CDlgLayerCodeCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLayerCodeCheck)
	DDX_Control(pDX, IDC_LIST_LAYERS, m_wndLayers);
	DDX_Check(pDX, IDC_CHECK_ALLLAYERS, m_bCheckAlllayers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLayerCodeCheck, CDialog)
	//{{AFX_MSG_MAP(CDlgLayerCodeCheck)
	ON_BN_CLICKED(IDC_CHECK_ALLLAYERS, OnCheckAlllayers)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LAYERS, OnClickListLayers)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_LAYERS, OnColumnclickListLayers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLayerCodeCheck message handlers


void CDlgLayerCodeCheck::FillLayers()
{
	if( !m_pDoc )return;

	CDlgDataSource *pds = m_pDoc->GetDlgDataSource();
	int nlayer = pds->GetFtrLayerCount();

	CScheme *psch = gpCfgLibMan->GetScheme(pds->GetScale());
	if( !psch )return;

	m_wndLayers.DeleteAllItems();
	int nItem = 0;
	for( int i=0; i<nlayer; i++)
	{
		CFtrLayer *pLayer = pds->GetFtrLayerByIndex(i);
		if (!pLayer || pLayer->IsEmpty()) continue;

		CString name = pLayer->GetName();
		__int64 code = 0;

		if( !m_bCheckAlllayers )
		{
			if( psch->FindLayerIdx(FALSE,code,name) )
				continue;
		}

		m_wndLayers.InsertItem(nItem,"");
		m_wndLayers.SetItemText(nItem,0,CString(name));

		//m_wndLayers.SetItemText(nItem,1,pLayer->GetMapName());
		m_wndLayers.SetItemText(nItem,1,CString());

		m_wndLayers.SetItemData(nItem,(DWORD_PTR)pLayer);
	}
}

void CDlgLayerCodeCheck::OnCheckAlllayers() 
{
	UpdateData(TRUE);

	FillLayers();
}

void CDlgLayerCodeCheck::OnOK() 
{
	m_pDoc->BeginBatchUpdate();
	
	for( int i=0; i<m_wndLayers.GetItemCount(); i++)
	{
		CString text1, text2;
		CFtrLayer *pLayer = (CFtrLayer*)m_wndLayers.GetItemData(i);
		if( !pLayer )continue;

		text2 = m_wndLayers.GetItemText(i,1);
		CFtrLayer *pLayer2 = m_pDoc->GetDlgDataSource()->GetLocalFtrLayer(text2);

		if( !pLayer2)continue;

		if( pLayer!=pLayer2 )
			m_pDoc->CutObjsToLayer(pLayer,pLayer2,TRUE);
	}

	m_pDoc->EndBatchUpdate();
	
	CDialog::OnOK();
}

BOOL CDlgLayerCodeCheck::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_wndLayers.ModifyStyleEx(0,LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	m_wndLayers.SetExtendedStyle(m_wndLayers.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_wndLayers.InsertColumn(0,StrFromResID(IDS_LAYER_NOCONFIG));	
	//m_wndLayers.InsertColumn(1,StrFromResID(IDS_FIELDNAME_MAPNAME));
	m_wndLayers.InsertColumn(1,StrFromResID(IDS_CONVERTTO_LAYER));

	m_wndLayers.SetColumnWidth(0,160);
	m_wndLayers.SetColumnWidth(1,160);
	//m_wndLayers.SetColumnWidth(2,160);

	FillLayers();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgLayerCodeCheck::OnClickListLayers(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	POSITION pos = m_wndLayers.GetFirstSelectedItemPosition();
	if( pos==NULL )return;

	int nItem = m_wndLayers.GetNextSelectedItem(pos);

	CDlgSelectLayer dlg;
	dlg.m_pDoc = m_pDoc;
	dlg.m_strLayer = m_wndLayers.GetItemText(nItem,1);

	if( dlg.DoModal()!=IDOK )
		return;

	if( !dlg.m_strLayer.IsEmpty() )
		m_wndLayers.SetItemText(nItem,1,dlg.m_strLayer);
}


struct LAYERLIST_COMPARE
{
	CListCtrl *pCtrl;
	int index;
	BOOL bAscending;
};


static int FindItemByData(CListCtrl *pCtrl, DWORD_PTR data)
{
	for( int i=0; i<pCtrl->GetItemCount(); i++)
	{
		if( pCtrl->GetItemData(i)==data )
			return i;
	}
	return -1;
}

static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, 
						 LPARAM lParamSort)
{
	LAYERLIST_COMPARE *pData = (LAYERLIST_COMPARE*)lParamSort;

	CString    strItem1 = pData->pCtrl->GetItemText(FindItemByData(pData->pCtrl,lParam1), pData->index);
	CString    strItem2 = pData->pCtrl->GetItemText(FindItemByData(pData->pCtrl,lParam2), pData->index);

	int nret = strItem1.CompareNoCase(strItem2);
	if( pData->bAscending )return nret;
	else return -nret;
}

void CDlgLayerCodeCheck::OnColumnclickListLayers(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	if( m_nCurColumn==pNMListView->iSubItem )
		m_bSortAscending = !m_bSortAscending;

	m_nCurColumn = pNMListView->iSubItem;

	LAYERLIST_COMPARE data;
	data.pCtrl = &m_wndLayers;
	data.index = pNMListView->iSubItem;
	data.bAscending = m_bSortAscending;
	
	m_wndLayers.SortItems(CompareFunc,(LPARAM)&data);
	
	*pResult = 0;
}
