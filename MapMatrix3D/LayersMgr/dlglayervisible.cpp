// dlglayervisible.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "dlglayervisible.h"
#include "DlgDataSource.h"
#include "UndoAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define IDC_LM_PROPLIST			1001
/////////////////////////////////////////////////////////////////////////////
// CDlgLayerVisible dialog


CDlgLayerVisible::CDlgLayerVisible(CWnd* pParent /*=NULL*/)
	: CManageBaseDlg(CDlgLayerVisible::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLayerVisible)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDlgDoc = GetActiveDlgDoc();
}


void CDlgLayerVisible::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLayerVisible)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLayerVisible, CDialog)
	//{{AFX_MSG_MAP(CDlgLayerVisible)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLayerVisible message handlers

void CDlgLayerVisible::AdjustLayout()
{
	if (GetSafeHwnd () == NULL)
		return;
	
	CRect rectClient;
	GetClientRect (rectClient);
	
	m_wndPropListCtrl.SetWindowPos(NULL,rectClient.left,rectClient.top,rectClient.Width(),rectClient.Height(),SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDlgLayerVisible::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	AdjustLayout();	
}

BOOL CDlgLayerVisible::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT rt;
	::SetRect(&rt,0,0,0,0);
	
	if (!m_wndPropListCtrl.Create(WS_VISIBLE|WS_CHILD|WS_BORDER,rt,this,IDC_LM_PROPLIST))
		return FALSE;

	AdjustLayout();

	FillAllLayersList();

	m_pDlgDoc->BeginBatchUpdate();

	return TRUE;
}

void CDlgLayerVisible::FillAllLayersList(CDlgDataSource *pDS)
{
	m_wndPropListCtrl.RemoveAll();
	
	CLVLPropColumn col;
	strcpy(col.FieldName,FIELDNAME_LAYERNAME);
	strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_LAYERNAME));
	col.ValueType = VT_BSTR;
	col.Type = CLVLPropColumn::EDIT;
	col.bReadOnly = TRUE;
	m_wndPropListCtrl.AddColumn(col);
	
	strcpy(col.FieldName,FIELDNAME_FTRVISIBLE);
	strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_FTRVISIBLE));
	col.ValueType = VT_BOOL;
	col.Type = CLVLPropColumn::CHECK;
	col.BitmapOffset = 1;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);

	m_wndPropListCtrl.InitHeaderCtrl();
	
	CHeaderCtrl& header = m_wndPropListCtrl.GetHeaderCtrl();
	for(int i=0; i<header.GetItemCount(); i++)
	{
		HDITEM hitem;
		hitem.mask = HDI_WIDTH;
		hitem.cxy = 120;
		header.SetItem(i,&hitem);
	}

	CDlgDataSource *pDS0 = NULL;
	if(!pDS)
		pDS0 = m_pDlgDoc->GetDlgDataSource();
	else
		pDS0 = pDS;
	int nLayer = pDS0->GetFtrLayerCount();
	CLVLPropItem *pItem;
	const CVariantEx* value;
	int nCol = m_wndPropListCtrl.GetColumnCount();
	long code;
	for( i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer )continue;
		if(pLayer->IsEmpty() || !pLayer->IsVisible()) continue;	
		
		pItem = new CLVLPropItem;
		if( !pItem )continue;
		
		m_wndPropListCtrl.AddItem(pItem);
		if (pDS0->GetCurFtrLayer()==pLayer)
		{
			m_wndPropListCtrl.SetCurLayerPropItem(pItem);
		}		

		BOOL bViewValue = TRUE;
		m_pBaseView->m_mapLayerVisible.Lookup(pLayer,bViewValue);
		bViewValue &= pLayer->IsVisible();

		pItem->SetValue((_variant_t)(const char*)(pLayer->GetName()),0);
		pItem->SetValue((_variant_t)(bool)(bViewValue),1);
		pItem->SetData((DWORD_PTR)pLayer);
	}

	m_wndPropListCtrl.FilterPropItems();
}

void CDlgLayerVisible::ModifyAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc, _variant_t exData)
{
	CFtrLayer* pLayer = NULL;

	pLayer = (CFtrLayer*)pItem->GetData();
    if (stricmp(Pc->FieldName,FIELDNAME_LAYERNAME)==0)
    {	
		if (pLayer->IsInherent())
		{
			return;
		}
	}

	_variant_t value = pItem->GetValue(Pc->FieldName);
	CLVLPropItem* pPropitem;
	
	_variant_t data;	
	
	pLayer = (CFtrLayer*)pItem->GetData();
	if (pLayer)
	{					
		if (stricmp(Pc->FieldName,FIELDNAME_LAYVISIBLE)==0)
		{
			data = (bool)pLayer->IsVisible();			
		}
		else
			return;

		m_pBaseView->m_mapLayerVisible.SetAt(pLayer,(int)(long)value);

		m_pDlgDoc->ModifyLayer(pLayer,Pc->FieldName,value,TRUE,FALSE);

	}	

	m_pDlgDoc->UpdateAllViews(NULL,hc_Refresh);
}

void CDlgLayerVisible::OnCancel()
{	
	m_pDlgDoc->EndBatchUpdate();
	EndDialog(IDNO);
}

void CDlgLayerVisible::ModifyCheckState(CLVLPropItem **pItems, int num, CLVLPropColumn* pCol, int col, _variant_t value)
{
	if (pItems == NULL || num <= 0 || pCol == NULL) return;
	
	for (int i=0; i<num; i++)
	{
		CLVLPropItem *pItem = pItems[i];
		if (!pItem) continue;
		
		if (pCol->bReadOnlyRefItem && pItem->IsReadOnly())
			continue;
		
		pItem->SetValue(value,col);
		
		{
			_variant_t data;	
			
			CFtrLayer *pLayer = (CFtrLayer*)pItem->GetData();
			if (pLayer)
			{					
				if (stricmp(pCol->FieldName,FIELDNAME_LAYVISIBLE)==0)
				{
					data = (bool)pLayer->IsVisible();		
					
				}
				else
					continue;

				m_pBaseView->m_mapLayerVisible.SetAt(pLayer,(long)value);

				m_pDlgDoc->ModifyLayer(pLayer,pCol->FieldName,value,TRUE,FALSE);

				
			}
			
		}
		
	}
	
	m_pDlgDoc->UpdateAllViews(NULL,hc_Refresh);
}