// dlgsetupgroup.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "dlgsetupgroup.h"
#include "CollectionTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetupGroup dialog


CDlgSetupGroup::CDlgSetupGroup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetupGroup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetupGroup)
	m_strGroupName = _T("");
	//}}AFX_DATA_INIT
	m_pCurGroup = NULL;
}


void CDlgSetupGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetupGroup)
	DDX_Control(pDX, IDC_LIST_GROUP, m_cGroup);
	DDX_Text(pDX, IDC_EDIT_GROUPNAME, m_strGroupName);
	DDV_MaxChars(pDX, m_strGroupName, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetupGroup, CDialog)
	//{{AFX_MSG_MAP(CDlgSetupGroup)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_BREAK, OnButtonBreak)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, OnButtonHide)
	ON_BN_CLICKED(IDC_BUTTON_NEWGROUP, OnButtonNewgroup)
	ON_BN_CLICKED(IDC_BUTTON_SHOW, OnButtonShow)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GROUP, OnItemchangedListGroup)
	ON_MESSAGE(WM_VALIDATE, OnEndLabelEditVariableCriteria)
	ON_MESSAGE(WM_SET_ITEMS, PopulateComboList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetupGroup message handlers

BOOL CDlgSetupGroup::FillList()
{
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	int num = pDS->GetObjectGroupCount();
	for (int i=0; i<num; i++)
	{
		ObjectGroup *pGroup = pDS->GetObjectGroup(i);
		
		int curSel = m_cGroup.InsertItem(LVIF_TEXT|LVIF_STATE, i, 
			"1", 0, LVIS_SELECTED, 0, 0);
		m_cGroup.SetItemData(i, DWORD_PTR(pGroup));
		m_cGroup.SetItemText(curSel,0,pGroup->name);
		m_cGroup.SetItemText(curSel,1,pGroup->select?StrFromResID(IDS_YES):StrFromResID(IDS_NO));		

	}

	if (num > 0)
	{
		m_cGroup.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
	}	

	return TRUE;
}

BOOL CDlgSetupGroup::OnInitDialog()
{
	CDialog::OnInitDialog();
	
 	m_cGroup.SetExtendedStyle(m_cGroup.GetExtendedStyle() | LVS_EX_TWOCLICKACTIVATE | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rect;
	m_cGroup.GetClientRect(&rect);
	int wide = rect.Width()/2-1;
 	m_cGroup.InsertColumn(0,StrFromResID(IDS_GROUPNAME),LVCFMT_LEFT,wide);
 	m_cGroup.InsertColumn(1,StrFromResID(IDS_GROUP_WHOLESELECT),LVCFMT_LEFT,wide);
	m_cGroup.SetComboColumns(1);
	
	FillList();

	return TRUE;
}

void CDlgSetupGroup::Init(CDlgDoc *pDoc)
{
	m_pDoc = pDoc;
	if (!m_pDoc) return;
}

void CDlgSetupGroup::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	if (!m_pCurGroup)
	{
		GOutPut(StrFromResID(IDS_GROUP_SELECTGROUP));	
		return;
	}

	CSelection *pSelect = m_pDoc->GetSelection();
	if (!pSelect) return;
	
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	int num, validnum = 0, invalidnum = 0;
	const FTR_HANDLE *ftrs = pSelect->GetSelectedObjs(num);

	if (num == 0)
	{
		CString str;
		str.Format(StrFromResID(IDS_GROUP_SELECTNONE),m_pCurGroup->name);
		GOutPut(str);	
		return;
	}

	for (int i=0; i<num; i++)
	{
		CFeature *pFtr = (CFeature*)ftrs[i];
		if (!pFtr || pFtr->IsInObjectGroup(m_pCurGroup->id) || !pFtr->IsVisible() || pFtr->IsDeleted()) 
		{
			invalidnum++;
			continue;
		}

		pDS->DeleteObject(pFtr);

		pFtr->AddObjectGroup(m_pCurGroup->id);
		
		pDS->RestoreObject(pFtr);

		m_pCurGroup->AddObject((CFeature*)ftrs[i]);

		validnum++;

	}

	CString str;
	str.Format(StrFromResID(IDS_GROUP_ADD),num,validnum,m_pCurGroup->name,m_pCurGroup->ftrs.GetSize(),m_pCurGroup->name);
	GOutPut(str);
}

void CDlgSetupGroup::OnButtonBreak() 
{
	// TODO: Add your control notification handler code here
	if (!m_pCurGroup)
	{
		GOutPut(StrFromResID(IDS_GROUP_SELECTGROUP));	
		return;
	}
	
	CString name = m_pCurGroup->name;

	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();

	int num = m_pCurGroup->ftrs.GetSize();
	for (int i=0; i<num; i++)
	{
		CFeature *pFtr = (CFeature*)m_pCurGroup->ftrs[i];
		if (!pFtr || !pFtr->IsVisible() || pFtr->IsDeleted()) continue;
		
		pDS->DeleteObject(pFtr);

		pFtr->RemoveObjectGroup(m_pCurGroup->id);

		pDS->RestoreObject(pFtr);
	}

	m_pDoc->GetDlgDataSource()->DelObjectGroup(m_pCurGroup);
	m_pCurGroup = NULL;
	
	POSITION pos = m_cGroup.GetFirstSelectedItemPosition();
	int curSel = m_cGroup.GetNextSelectedItem(pos);
	
	m_cGroup.DeleteItem(curSel);
	
	CString str;
	str.Format(StrFromResID(IDS_GROUP_BREAK),name,num,name);
	GOutPut(str);

	if( m_cGroup.GetItemCount()>curSel )
		m_cGroup.SetItemState(curSel,LVIS_SELECTED,LVIS_SELECTED);
	else if( m_cGroup.GetItemCount()>0 )
		m_cGroup.SetItemState(m_cGroup.GetItemCount()-1,LVIS_SELECTED,LVIS_SELECTED);
}

void CDlgSetupGroup::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	if (!m_pCurGroup)
	{
		GOutPut(StrFromResID(IDS_GROUP_SELECTGROUP));	
		return;
	}
	
	CSelection *pSelect = m_pDoc->GetSelection();
	if (!pSelect) return;

	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	
	int num, validnum = 0, invalidnum = 0;
	const FTR_HANDLE *ftrs = pSelect->GetSelectedObjs(num);

	if (num == 0)
	{
		CString str;
		str.Format(StrFromResID(IDS_GROUP_SELECTDEL),m_pCurGroup->name);
		GOutPut(str);	
		return;
	}

	for (int i=0; i<num; i++)
	{
		CFeature *pFtr = (CFeature*)ftrs[i];
		if (!pFtr || !pFtr->IsInObjectGroup(m_pCurGroup->id) || !pFtr->IsVisible() || pFtr->IsDeleted())
		{
			invalidnum++;
			continue;
		}
		
		pDS->DeleteObject(pFtr);

		pFtr->RemoveObjectGroup(m_pCurGroup->id);

		pDS->RestoreObject(pFtr);

		m_pCurGroup->ClearObject((CPFeature)ftrs[i]);

		validnum++;
	}

	CString str;
	str.Format(StrFromResID(IDS_GROUP_DEL),num,validnum,m_pCurGroup->name,m_pCurGroup->ftrs.GetSize(),m_pCurGroup->name);
	GOutPut(str);
}

void CDlgSetupGroup::OnButtonNewgroup() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_strGroupName.IsEmpty())
	{
		CString str;
		str.Format(IDS_CANNT_EMPTY);

		MessageBox(str);
		return;
	}
	
	// 检查是否重名
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	int num = pDS->GetObjectGroupCount();
	for (int i=0; i<num; i++)
	{
		if (m_strGroupName.CompareNoCase(pDS->GetObjectGroup(i)->name) == 0)
		{
			break;
		}
	}
	
	if (i < num)
	{
		CString msg;
		msg.LoadString(IDS_GROUPNAME_EXIST);
		MessageBox(msg,NULL,MB_OK|MB_ICONASTERISK);
		return;
	}

	ObjectGroup *pGroup = new ObjectGroup;
	if (!pGroup) return;
	
	strncpy(pGroup->name,m_strGroupName,16);
	pGroup->name[15] = '\0';
	
	//将所有项状态设置为没有选中,将增加的一项设置为选中状态
	int item_num = m_cGroup.GetItemCount();		
	int curSel = m_cGroup.InsertItem(LVIF_TEXT|LVIF_STATE, item_num, 
		"1", 0, LVIS_SELECTED, 0, 0);

	m_cGroup.SetItemData(curSel, DWORD_PTR(pGroup));
	m_cGroup.SetItemText(curSel,0,(LPCTSTR)pGroup->name);
	m_cGroup.SetItemText(curSel,1,pGroup->select?StrFromResID(IDS_YES):StrFromResID(IDS_NO));		
		
	m_pDoc->GetDlgDataSource()->AddObjectGroup(pGroup);
	m_pCurGroup = pGroup;

	CSelection *pSelect = m_pDoc->GetSelection();
	
	int sel_num, validnum = 0, invalidnum = 0;
	const FTR_HANDLE *ftrs = pSelect->GetSelectedObjs(sel_num);
	
	if (sel_num>0)
	{	
		for (int i=0; i<sel_num; i++)
		{
			CFeature *pFtr = (CFeature*)ftrs[i];
			if (!pFtr || pFtr->IsInObjectGroup(m_pCurGroup->id) || !pFtr->IsVisible() || pFtr->IsDeleted()) 
			{
				invalidnum++;
				continue;
			}
			
			pDS->DeleteObject(pFtr);
			
			pFtr->AddObjectGroup(m_pCurGroup->id);
			
			pDS->RestoreObject(pFtr);
			
			m_pCurGroup->AddObject((CFeature*)ftrs[i]);
			
			validnum++;			
		}
		
		CString str;
		str.Format(StrFromResID(IDS_GROUP_ADD),sel_num,validnum,m_pCurGroup->name,m_pCurGroup->ftrs.GetSize(),m_pCurGroup->name);
		GOutPut(str);
	}
	else
	{		
		CString str;
		str.Format(StrFromResID(IDS_GROUP_NEW),pGroup->name);
		GOutPut(str);
	}

	m_cGroup.SetItemState(curSel,LVIS_SELECTED,LVIS_SELECTED);
}

void CDlgSetupGroup::OnButtonHide() 
{
	// TODO: Add your control notification handler code here
	if (!m_pCurGroup)
	{
		GOutPut(StrFromResID(IDS_GROUP_SELECTGROUP));	
		return;
	}
	
	CSelection *pSelect = m_pDoc->GetSelection();
	if (!pSelect) return;
	
	int num = m_pCurGroup->ftrs.GetSize();
	for (int k=0; k<num; k++)
	{
		CFeature *pFtr = m_pCurGroup->ftrs[k];
		if (!pFtr || !pFtr->IsVisible() || pFtr->IsDeleted()) continue;
		
		pSelect->DeselectObj(FTR_HANDLE(pFtr));
		
	}	
	
	if (num >= 1)
	{
		m_pDoc->OnSelectChanged();
	}

	CString str;
	str.Format(StrFromResID(IDS_GROUP_HIDE),num);
	GOutPut(str);
}

void CDlgSetupGroup::OnButtonShow() 
{
	// TODO: Add your control notification handler code here
	if (!m_pCurGroup)
	{
		GOutPut(StrFromResID(IDS_GROUP_SELECTGROUP));	
		return;
	}

	CSelection *pSelect = m_pDoc->GetSelection();
	if (!pSelect) return;
	
	pSelect->DeselectAll();

	Envelope e;

	int num = m_pCurGroup->ftrs.GetSize();
	for (int k=0; k<num; k++)
	{
		CFeature *pFtr = m_pCurGroup->ftrs[k];
		if (!pFtr || !pFtr->IsVisible() || pFtr->IsDeleted()) continue;

		e.Union(&pFtr->GetGeometry()->GetEnvelope());
		
		pSelect->SelectObj(FTR_HANDLE(pFtr));
		
	}	
	
	if (num >= 1)
	{
		m_pDoc->OnSelectChanged();
		m_pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&PT_3D(e.CenterX(),e.CenterY(),e.CenterZ()));
	}
	
	if (num <= 1)
	{
		CString str;
		str.Format(StrFromResID(IDS_GROUP_SHOW),num);
		GOutPut(str);
	}
	
}

void CDlgSetupGroup::OnOK() 
{
	ShowWindow(SW_HIDE);
}

void CDlgSetupGroup::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void CDlgSetupGroup::PostNcDestroy()
{
	CDialog::PostNcDestroy();
}

void CDlgSetupGroup::OnItemchangedListGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	if (pNMListView->uChanged == LVIF_STATE)
	{
		if (pNMListView->uNewState&LVIS_SELECTED)
		{
			m_pCurGroup = (ObjectGroup*)m_cGroup.GetItemData(pNMListView->iItem);
			
			if (!m_pCurGroup) return;			
			
			CString str;
			str.Format(StrFromResID(IDS_GROUP_SELECT),m_pCurGroup->name);
			GOutPut(str);

			if (m_pCurGroup)
			{
				m_strGroupName = m_pCurGroup->name;
				UpdateData(FALSE);
			}
			else
			{
				m_strGroupName.Empty();
				UpdateData(FALSE);
			}

			OnButtonShow();
		}
	}
	

	*pResult = 0;
}

LRESULT CDlgSetupGroup::OnEndLabelEditVariableCriteria(WPARAM wParam, LPARAM lParam)
{
	if(m_cGroup.m_bEdit == FALSE)
		return -1;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;

	//保存修改的属性值
	int curSel = pDispInfo->item.iItem;

	BOOL bModified = FALSE;

	CString msg;
	msg.LoadString(IDS_GROUPNAME_EXIST);	

	ObjectGroup *pLastGroup = (ObjectGroup*)m_cGroup.GetItemData(curSel);
	if (!pLastGroup) return -1;

	switch(pDispInfo->item.iSubItem)
	{
	case 0:
		{
			if (stricmp(pDispInfo->item.pszText,pLastGroup->name) == 0)
			{
				return -1;
			}

			CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
			int num = pDS->GetObjectGroupCount();
			for (int i=0; i<num; i++)
			{
				ObjectGroup *pGroup = pDS->GetObjectGroup(i);
				if (pLastGroup != pGroup && stricmp(pDispInfo->item.pszText,pGroup->name) == 0)
				{
					break;
				}
			}	
			
			if (i < num)
			{
				MessageBox(msg,NULL,MB_OK|MB_ICONASTERISK);
				return -1;
			}
			else
			{
				strncpy(pLastGroup->name,pDispInfo->item.pszText,16);
				bModified = TRUE;
			}

			break;	
		}
	case 1:
		{
			if (stricmp(pDispInfo->item.pszText,pLastGroup->select?StrFromResID(IDS_YES):StrFromResID(IDS_NO)) == 0)
			{
				return -1;
			}

			if(stricmp(pDispInfo->item.pszText,StrFromResID(IDS_YES)) == 0)
				pLastGroup->select = true;
			else if(stricmp(pDispInfo->item.pszText,StrFromResID(IDS_NO)) ==0)
				pLastGroup->select = false;

			bModified = TRUE;

			break;
		}
	default:
		break;
	}

	if (bModified)
	{
		m_pDoc->GetDlgDataSource()->UpdateObjectGroup(pLastGroup);
	}

	return 1;
		

}

LRESULT CDlgSetupGroup::PopulateComboList(WPARAM wParam, LPARAM lParam)
{
	// Get the Combobox window pointer
	CComboBox* pInPlaceCombo = static_cast<CComboBox*> (GetFocus());
	
	// Get the inplace combbox top left
	CRect focWindowRect;
	
	pInPlaceCombo->GetWindowRect(&focWindowRect);
	
	CPoint obInPlaceComboTopLeft(focWindowRect.TopLeft()); 
	
	// Get the active list
	// Get the control window rect
	// If the inplace combobox top left is in the rect then
	// The control is the active control
	CRect rSymbolRect;
	m_cGroup.GetWindowRect(&rSymbolRect);
	int iColIndex = (int )wParam;
	CStringList* pComboList = reinterpret_cast<CStringList*>(lParam);
	pComboList->RemoveAll(); 
	if (rSymbolRect.PtInRect(obInPlaceComboTopLeft)) 
	{				
		if(iColIndex == 1)
		{
			pComboList->AddTail(StrFromResID(IDS_YES));
			pComboList->AddTail(StrFromResID(IDS_NO));	
		}	
	}
	
	return true;
	
}
