// CheckSchemeViewBar.cpp: implementation of the CCheckSchemeViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CheckSchemeViewBar.h"
#include "resource.h"
#include "CheckScheme.h "
#include "EditBase.h "
#include "dlginputstr.h"
#include "DlgDataSource.h "
#include "editbasedoc.h "
#include "ExMessage.h "
#include "ATLCONV.H "


#define			TREE_ITEM_GROUP    0
#define         TREE_ITEM_ITEM     1

std::wstring CTreeCtrlWithTip::m_sstrTipinfo;

CTreeCtrlWithTip::CTreeCtrlWithTip()
{
}

CTreeCtrlWithTip::~CTreeCtrlWithTip()
{

}

BEGIN_MESSAGE_MAP(CTreeCtrlWithTip, CTreeCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
//	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify )
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

void CTreeCtrlWithTip::ClearSelection()
{
	SelectItem(NULL);
    for (HTREEITEM hItem=GetRootItem();hItem!=NULL;hItem=GetNextVisibleItem(hItem))
		if (GetItemState(hItem,TVIS_SELECTED)&TVIS_SELECTED)
			SetItemState(hItem,0,TVIS_SELECTED);
	//
	m_selitems.RemoveAll();
}

void CTreeCtrlWithTip::OnLButtonDown(UINT nFlags, CPoint point)
{	
	UINT uFlags = 0;
	HTREEITEM hTreeItem = HitTest (point,&uFlags);
	m_current_item = hTreeItem;

	//
	if (hTreeItem != NULL)
	{
        if(nFlags & MK_CONTROL)
		{
			if(GetItemData(hTreeItem)==TREE_ITEM_GROUP) return;
			if( 0 == ( GetItemState( hTreeItem,TVIS_SELECTED ) & TVIS_SELECTED ) )
			{
				m_selitems.Add(hTreeItem);
				SetItemState( hTreeItem, TVIS_SELECTED, TVIS_SELECTED );
			}
			else
			{
				for(int i=0; i<m_selitems.GetSize(); i++)
				{
					if(m_selitems[i]==hTreeItem)
					{
						m_selitems.RemoveAt(i);
						break;
					}
				}
				SetItemState( hTreeItem, 0, TVIS_SELECTED );
			}
			return;
		}
		else if(nFlags & MK_SHIFT)
		{
			if(GetItemData(hTreeItem)==TREE_ITEM_GROUP) return;
			if(m_selitems.GetSize()==0)
			{
				m_selitems.Add(hTreeItem);
				SetItemState( hTreeItem, TVIS_SELECTED,TVIS_SELECTED );
			}
			else
			{
				HTREEITEM m_hFirst = m_selitems[0];
				ClearSelection();
				m_selitems.Add(m_hFirst);
				SetItemState( m_hFirst, TVIS_SELECTED,TVIS_SELECTED );
				//
				CRect rcFrom,rcTo;
				GetItemRect( m_hFirst,&rcFrom,FALSE );
				GetItemRect( hTreeItem,&rcTo,FALSE );
				
				HTREEITEM hItemUp,hItemDown;
				if( rcFrom.top < rcTo.top )
				{
					hItemUp = m_hFirst;
					hItemDown = hTreeItem;
				}
				else
				{
					hItemUp = hTreeItem;
					hItemDown = m_hFirst;
				}
                //
				HTREEITEM hItemTemp = hItemUp;
				while( hItemTemp )
				{
					if(hItemTemp!=m_hFirst)
					{
						m_selitems.Add(hItemTemp);
					    SetItemState( hItemTemp, TVIS_SELECTED,TVIS_SELECTED );
					}
					if( hItemTemp == hItemDown )break;
					hItemTemp = GetNextVisibleItem( hItemTemp );
				}
				SelectItem(hTreeItem);
			}
			//
			return;
		}
		if( 0 == ( GetItemState( hTreeItem,TVIS_SELECTED ) & TVIS_SELECTED ) )
		{
			ClearSelection();
			SelectItem(hTreeItem);
			SetItemState( hTreeItem, TVIS_SELECTED,TVIS_SELECTED );
			m_selitems.Add(hTreeItem);
		}
	}
	else
	{
		ClearSelection();
	}

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

INT_PTR CTreeCtrlWithTip::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	RECT rect;	
	UINT nFlags;
	HTREEITEM hitem = HitTest( point, &nFlags );
	if( (nFlags & TVHT_ONITEMLABEL) &&GetItemData(hitem)==TREE_ITEM_ITEM )
	{		
		GetItemRect( hitem, &rect, TRUE );
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT_PTR)hitem;
		pTI->lpszText = LPSTR_TEXTCALLBACK;
		pTI->rect = rect;
		return 1;
	}
	
	return -1;
}

BOOL CTreeCtrlWithTip::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	//TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT_PTR nID =pNMHDR->idFrom;
	CCheckScheme& rScheme = GetCheckScheme();

	DWORD dw = GetMessagePos();
	POINTS pt = MAKEPOINTS(dw); 
	CPoint point(pt.x,pt.y);
	ScreenToClient(&point);
	UINT uFlags;
	HTREEITEM hItem = (HTREEITEM)nID;
	if( nID == (UINT_PTR)m_hWnd &&
		(( pNMHDR->code == TTN_NEEDTEXTA && pTTTA->uFlags & TTF_IDISHWND ) ||
		( pNMHDR->code == TTN_NEEDTEXTW && pTTTW->uFlags & TTF_IDISHWND ) ) )
		return FALSE;
	if ((hItem != NULL))
	{
		//				pmyTreeCtrl->Select(hItem, TVGN_CARET);
		CString strItem = GetItemText(hItem);
		HTREEITEM parent = GetParentItem(hItem);
		CString strGroup = GetItemText(parent);
		CString strTmp;
		int nNum = 0;
		CCheckGroup* pGroup = rScheme.GetCheckGroup(nNum);
		if(nNum>0)
		{		
			for (int i=0;i<nNum;i++)
			{
				if(pGroup[i].m_groupName==strGroup)
					break;
			}
			if (i<nNum)
			{
				CArray<CCheckItem,CCheckItem&> &itemList = pGroup[i].m_itemList;
				for (int j=0;j<itemList.GetSize();j++)
				{
					if(itemList[j].m_itemName==strItem)
					{
						break;
					}
				}
				if (j<itemList.GetSize())
				{
					CArray<CCheckTask,CCheckTask&> &rTask = itemList[j].m_taskList;
					if (rTask.GetSize()>0)
					{
						strTmp+=rTask[0].m_checkName;
					}
					for (int k=1;k<rTask.GetSize();k++)
					{
						strTmp+=_T(", ");
						strTmp+=rTask[k].m_checkName;						
					}
				}
			}
		}
		USES_CONVERSION;
		m_sstrTipinfo = A2W(strTmp);

		pTTTW->lpszText = (LPWSTR)m_sstrTipinfo.c_str();

		return(TRUE);
	}	
    return(FALSE);
}

int CTreeCtrlWithTip::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	EnableToolTips(TRUE);
	return 1;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCheckSchemeViewBar, CDockablePane)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_BTN_CHECKSCHEME_NEW, OnNewScheme)
	ON_BN_CLICKED(ID_BTN_CHECKSCHEME_DEL, OnDelScheme)
	ON_CBN_SELCHANGE(ID_COMBO_CHECKSCHEME_LIST, OnSelchangeScheme)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_NEWCHKGROUP,OnNewChkGroup)
	ON_COMMAND(ID_NEWCHKITEM,OnNewChkItem)
	ON_COMMAND(ID_DEL_CHK_GROUP,OnDelChkGroup)
	ON_COMMAND(ID_DEL_CHK_ITEM,OnDelChkItem)
	ON_COMMAND(ID_RENAME_CHK_GROUP,OnRenameChkGroup)
	ON_COMMAND(ID_RENAME_CHK_ITEM,OnRenameChkItem)
	ON_COMMAND(ID_CUSTOMIZE_CHKITEM,OnCustomizeChkItem)
	ON_NOTIFY(NM_RCLICK, ID_TREE_CHECKSCHEME_TREE, OnNMRClickTree)
	ON_NOTIFY(NM_DBLCLK, ID_TREE_CHECKSCHEME_TREE, OnDblClickTree)
	ON_COMMAND(ID_EXECUTE_ITEM,OnExecuteChkItem)
	ON_COMMAND(ID_EXECUTE_ITEM_SELECTION,OnExecuteChkItemSelection)
	ON_COMMAND(ID_EXECUTE_GROUP,OnExecuteChkGroup)
	ON_COMMAND(ID_CHK_IMPORT_SCRIPT,OnChkImportScript)
END_MESSAGE_MAP()
CCheckSchemeViewBar::CCheckSchemeViewBar()
{
	m_pCheckScheme = NULL;
	m_nCurScheme = -1;
	m_bActive = FALSE;
	CString path = GetConfigPath(TRUE);
	path+=_T("\\CheckScheme");
	m_strSchemeDir = path;
	if(!PathFileExists(m_strSchemeDir))
	{
		if(!::CreateDirectory(m_strSchemeDir,NULL))
			m_strSchemeDir.Empty();
	}
}

CCheckSchemeViewBar::~CCheckSchemeViewBar()
{

}

void CCheckSchemeViewBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if(!m_bActive||m_strSchemeDir.IsEmpty())
		return;	
	if(pWnd==&m_treeCtrl)
	{	
		CMenu menu;
		menu.LoadMenu(IDR_MENU_CHKSCHEME);
		menu.GetSubMenu(0)->RemoveMenu(ID_EXECUTE_GROUP,MF_BYCOMMAND);
		HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
		if(hitem==NULL)
		{
			if (m_nCurScheme==-1)
			{
				return;
			}
			menu.GetSubMenu(0)->EnableMenuItem(ID_EXECUTE_ITEM,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			menu.GetSubMenu(0)->EnableMenuItem(ID_NEWCHKITEM,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			menu.GetSubMenu(0)->EnableMenuItem(ID_DEL_CHK_GROUP,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			menu.GetSubMenu(0)->EnableMenuItem(ID_DEL_CHK_ITEM,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			menu.GetSubMenu(0)->EnableMenuItem(ID_CUSTOMIZE_CHKITEM,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			menu.GetSubMenu(0)->EnableMenuItem(ID_RENAME_CHK_GROUP,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			menu.GetSubMenu(0)->EnableMenuItem(ID_RENAME_CHK_ITEM,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else if(m_treeCtrl.GetItemData(hitem)==TREE_ITEM_GROUP)
		{
			menu.GetSubMenu(0)->EnableMenuItem(ID_EXECUTE_ITEM,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			menu.GetSubMenu(0)->EnableMenuItem(ID_DEL_CHK_ITEM,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			menu.GetSubMenu(0)->EnableMenuItem(ID_CUSTOMIZE_CHKITEM,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			menu.GetSubMenu(0)->EnableMenuItem(ID_RENAME_CHK_ITEM,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else if (m_treeCtrl.GetItemData(hitem)==TREE_ITEM_ITEM)
		{
			menu.GetSubMenu(0)->EnableMenuItem(ID_DEL_CHK_GROUP,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			menu.GetSubMenu(0)->EnableMenuItem(ID_RENAME_CHK_GROUP,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else
		{
			return;
		}
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,point.x,point.y,this);	
		return;
	}
	CDockablePane::OnContextMenu( pWnd,  point);	
}

void CCheckSchemeViewBar::RefreshViewBar(int mode)
{
	if (mode==0)
	{
		if (m_bActive==FALSE)
		{
			RefreshSchemeView();
			m_bActive = TRUE;
		}
		
	}
	else
	{
		if (m_bActive==TRUE)
		{
			ClearData();
			m_bActive = FALSE;
		}
	}
		
}

BOOL CCheckSchemeViewBar::RefreshSchemeView()
{	
	if (!m_pCheckScheme)
		m_pCheckScheme = &GetCheckScheme();

	if (bInit==FALSE)
	{
		//列出方案文件
		CString path = m_strSchemeDir;
		if (!path.IsEmpty())
		{
			path+=_T("\\*.chk");
			CFileFind finder;
			BOOL bWorking = finder.FindFile(path);
			while (bWorking)
			{
				bWorking = finder.FindNextFile();
				m_arrStrSchemes.Add(finder.GetFilePath());
			}
		}
		
		for (int i=0;i<m_arrStrSchemes.GetSize();i++)
		{
			CString text = ::PathFindFileName(m_arrStrSchemes[i]);
			int index = text.ReverseFind('.');
			if (index != -1)
			{
				text = text.Left(index);
			}
			m_comBobox.AddString(text);
		}
		if (m_comBobox.GetCount()>0)
		{
			m_nCurScheme = 0;
			m_comBobox.SetCurSel(m_nCurScheme);
		}
		
		bInit = TRUE;
	}
	if (m_arrStrSchemes.GetSize()>0)
	{	
		m_treeCtrl.DeleteAllItems();

		if (oldScheme!=m_arrStrSchemes[m_nCurScheme])
		{	
			oldScheme = m_arrStrSchemes[m_nCurScheme];
			if(!m_pCheckScheme->Load(m_arrStrSchemes[m_nCurScheme]))
				return FALSE;
		}
		{
			int nNum;
			CCheckGroup* pGroup = m_pCheckScheme->GetCheckGroup(nNum);
			HTREEITEM hitem = NULL,hitem0 = NULL;
			for (int i=0;i<nNum;i++)
			{
				hitem = m_treeCtrl.InsertItem(pGroup[i].m_groupName,0,0);
				m_treeCtrl.SetItemData(hitem,TREE_ITEM_GROUP);
				int itemSize = pGroup[i].m_itemList.GetSize();
				for (int j=0;j<itemSize;j++)
				{
					hitem0 = m_treeCtrl.InsertItem(pGroup[i].m_itemList[j].m_itemName,1,1,hitem);
					CCheckItem &refItem = pGroup[i].m_itemList[j];
					CString strTemp;
					for (int k=0;k<refItem.m_taskList.GetSize()-1;k++)
					{
						strTemp+=refItem.m_taskList[k].m_checkName;
						strTemp+=_T("\n");
					}
// 					strTemp+=refItem.m_taskList[k].m_checkName;
// 					m_toolTips.AddTool(&m_treeCtrl, strTemp, &r, id)
					m_treeCtrl.SetItemData(hitem0,TREE_ITEM_ITEM);
				}				
			}
		}

		HTREEITEM hItem = m_treeCtrl.GetRootItem();
		while (NULL != hItem)
		{
			m_treeCtrl.Expand(hItem, TVE_EXPAND);
			hItem = m_treeCtrl.GetNextItem(hItem, TVGN_NEXT);
		}
	}
	else
	{
		m_treeCtrl.DeleteAllItems();
		m_pCheckScheme->Load(NULL);
	}
	return TRUE; 
}


void CCheckSchemeViewBar::ClearData()
{
	m_comBobox.ResetContent();
	m_treeCtrl.DeleteAllItems();
	m_nCurScheme = -1;
	m_arrStrSchemes.RemoveAll();
	
    bInit = FALSE;
	
    oldScheme = _T("");
	
}

int CCheckSchemeViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;


	
	CRect dumy;
	dumy.SetRectEmpty();
	
	m_comBobox.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,dumy, this, ID_COMBO_CHECKSCHEME_LIST);
	
	
	m_treeCtrl.Create(WS_VISIBLE | WS_TABSTOP | WS_CHILD | WS_BORDER
		| TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES|TVS_SHOWSELALWAYS
		| TVS_DISABLEDRAGDROP, dumy, this, ID_TREE_CHECKSCHEME_TREE);
	m_ImageList.Create(IDB_BITMAP_CHK, 16, 0, RGB(255, 0, 255));
	
	m_treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);
	
	
	m_NewScheme.Create(StrFromResID(IDS_NEW_CHK_SCHEME),WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,dumy,this,ID_BTN_CHECKSCHEME_NEW);
	m_DelScheme.Create(StrFromResID(IDS_DEL_CHK_SCHEME),WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,dumy,this,ID_BTN_CHECKSCHEME_DEL);
	m_NewScheme.SetFont(&GetGlobalData()->fontRegular);
	m_DelScheme.SetFont(&GetGlobalData()->fontRegular);
// 	m_comBobox.EnableWindow(FALSE);
// 	m_treeCtrl.EnableWindow(FALSE);
// 	m_NewScheme.EnableWindow(FALSE);
// 	m_DelScheme.EnableWindow(FALSE);
	AdjustLayout();
	return 0;	
}

void CCheckSchemeViewBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout ();
}

BOOL CCheckSchemeViewBar::OnEraseBkgnd( CDC* pDC )
{
	CRect rectClient;
	GetClientRect (rectClient);	
	
	::FillRect(pDC->GetSafeHdc(),rectClient,GetSysColorBrush(COLOR_3DFACE));
	return TRUE;

}

void CCheckSchemeViewBar::OnNewScheme()
{
	if(!m_bActive||m_strSchemeDir.IsEmpty())
		return;
	CDlgInputStr dlg;
	dlg.SetTitle(StrFromResID(IDS_INPUT_CHK_SCH_NAME));
	if(dlg.DoModal()==IDOK&&!dlg.m_strInput.IsEmpty())
	{
		int i = 0;
		for (;i<m_arrStrSchemes.GetSize();i++)
		{
			if(m_arrStrSchemes[i]==dlg.m_strInput)
			{				
				break;
			}
		}
		if (i<m_arrStrSchemes.GetSize())
		{
			AfxMessageBox(StrFromResID(IDS_SAME_CHK_SCHNAME));
		}
		else
		{
			CString path = m_strSchemeDir;
			path+=_T("\\");
			path+=dlg.m_strInput;
			path+=_T(".chk");
			if (!m_pCheckScheme)
				m_pCheckScheme = &GetCheckScheme();
			//m_pCheckScheme->Load(path);
			
			m_arrStrSchemes.Add(path);
			CString text = ::PathFindFileName(path);
			int index = text.ReverseFind('.');
			if (index != -1)
			{
				text = text.Left(index);
			}
			m_comBobox.AddString(text);
			m_comBobox.SetCurSel(m_comBobox.GetCount()-1);
			m_nCurScheme = m_arrStrSchemes.GetSize()-1;
			RefreshSchemeView();
			m_pCheckScheme->Save();
		}
	}

}

void CCheckSchemeViewBar::OnDelScheme()
{
	if(!m_bActive||m_strSchemeDir.IsEmpty())
		return;
	if(m_comBobox.GetCount()>0)
	{


		if(IDOK == MessageBox(StrFromResID(IDS_CHECKSCHEME_DEL),"警告",MB_OKCANCEL|MB_ICONWARNING))
		{
		::DeleteFile(m_arrStrSchemes[m_nCurScheme]);//删除当前选中方案配置文件，不可恢复
		m_arrStrSchemes.RemoveAt(m_nCurScheme);
		m_comBobox.DeleteString(m_comBobox.GetCurSel());
		m_treeCtrl.DeleteAllItems();
		if (m_comBobox.GetCount()>0)
		{
			m_nCurScheme = 0;
			m_comBobox.SetCurSel(m_nCurScheme);
			RefreshSchemeView();
		}
		else
		{
			m_comBobox.SetCurSel(-1);
			m_nCurScheme = -1;
		}

		}
		
	}
}

void CCheckSchemeViewBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}
	
	CRect rectClient;
	GetClientRect (rectClient);

	int dx = 4,dy = 2;

	CRect rectText;
	rectText.left = rectClient.left;
	rectText.top = rectClient.top+dy;
	rectText.right = 0;
	rectText.bottom = rectText.top+20;
	m_staticText.SetWindowPos (NULL,
		rectText.left, 
		rectText.top, 
		rectText.Width(),
		rectText.Height(),
		SWP_NOACTIVATE | SWP_NOZORDER);
// 	CClientDC dc(this);
// 	CFont *pOld = dc.SelectObject(&m_font);
// 	dc.SetBkMode(TRANSPARENT);
// 	dc.DrawText(_T("schemelist"),rectText,DT_CENTER);
// 	dc.SelectObject(pOld);
// 	m_comBobox.SetItemHeight(-1,4);
// 	int fdfd = m_comBobox.GetItemHeight(-1);
	
	m_comBobox.SetWindowPos (NULL,
		rectText.right/*+dx*/, 
		rectText.top, 
		rectClient.right-rectText.right/*-dx*/,
		200,
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_treeCtrl.SetWindowPos (NULL,
		rectClient.left, 
		rectText.bottom+dy, 
		rectClient.Width(),
		rectClient.bottom-3*dy-20-rectText.bottom,
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_NewScheme.SetWindowPos(NULL,
		rectClient.left+dx+dx, 
		rectClient.bottom -dy-20, 
		rectClient.Width ()*1/3,
		20,
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_DelScheme.SetWindowPos(NULL,
		rectClient.right-dx-dx-rectClient.Width ()/3, 
		rectClient.bottom -dy-20, 
		rectClient.Width ()*1/3,
		20,
		SWP_NOACTIVATE | SWP_NOZORDER);
//	m_staticText.Invalidate(TRUE);		
}

void CCheckSchemeViewBar::OnSelchangeScheme()
{
	int nIndex = m_comBobox.GetCurSel();
	if (nIndex!=CB_ERR )
	{
		CString strSchemeName;
		m_comBobox.GetLBText(nIndex,strSchemeName);
		int i=0;
		for (;i<m_arrStrSchemes.GetSize();i++)
		{
			if ((strSchemeName+".chk")==CString(PathFindFileName(m_arrStrSchemes[i])))
			{
				m_nCurScheme = i;
				break;
			}
		}
		if (i<m_arrStrSchemes.GetSize())
		{
			RefreshSchemeView();
		}
	}
}

void CCheckSchemeViewBar::OnNewChkGroup()
{
	if(!m_bActive||m_strSchemeDir.IsEmpty())
		return;
	int nIndex = m_comBobox.GetCurSel();
	if(nIndex==LB_ERR)
		return;
	CDlgInputStr dlg;
	dlg.SetTitle(StrFromResID(IDS_INPUT_CHK_GROUPNAME));
	if(dlg.DoModal()==IDOK&&!dlg.m_strInput.IsEmpty())
	{
//		if(m_pCheckScheme&&m_pCheckScheme->IsValid())
		{
			int nNum = 0;
			CCheckGroup* pGroup = m_pCheckScheme->GetCheckGroup(nNum);
			if(nNum>0)
			{
				for (int i=0;i<nNum;i++)
				{
					if(pGroup[i].m_groupName==dlg.m_strInput)
						break;
				}
				if (i<nNum)
				{
					AfxMessageBox(StrFromResID(IDS_CHK_GROUPNAME_REPEAT));
					return;
				}			
			}
			
			CCheckGroup group;
			group.m_groupName = dlg.m_strInput;
			m_pCheckScheme->AddCheckGroup(group);
			HTREEITEM hitem = m_treeCtrl.InsertItem(group.m_groupName);
			m_treeCtrl.SetItemData(hitem,TREE_ITEM_GROUP);
			m_treeCtrl.SetItemImage(hitem,0,0);
			m_pCheckScheme->Save();
		}
		
	}
	
}
#include "dlgchkconfig.h "
void CCheckSchemeViewBar::OnNewChkItem()
{
	if(!m_bActive||m_strSchemeDir.IsEmpty())
		return;	
	if (m_treeCtrl.GetSelectedItem()==NULL)
		return;
	CDlgInputStr dlg;
	dlg.SetTitle(StrFromResID(IDS_INPUT_CHK_ITEMNAME));
	if(dlg.DoModal()==IDOK&&!dlg.m_strInput.IsEmpty())
	{
//		if(m_pCheckScheme)
		{
			int nNum = 0;
			CCheckGroup* pGroup = m_pCheckScheme->GetCheckGroup(nNum);
			if(nNum>0)
			{
				HTREEITEM hSel = m_treeCtrl.GetSelectedItem();
				HTREEITEM hGroup = NULL;
				DWORD_PTR data = m_treeCtrl.GetItemData(hSel);
				if (data == TREE_ITEM_ITEM)
				{
					hGroup = m_treeCtrl.GetParentItem(hSel);
				}
				else if (data == TREE_ITEM_GROUP)
				{
					hGroup = hSel;
				}
						
				CString strGroup = m_treeCtrl.GetItemText(hGroup);
				for (int i=0;i<nNum;i++)
				{
					if(pGroup[i].m_groupName==strGroup)
						break;
				}
				if (i<nNum)
				{
					CArray<CCheckItem,CCheckItem&> &itemList = pGroup[i].m_itemList;
					for (int j=0;j<itemList.GetSize();j++)
					{
						if(itemList[j].m_itemName==dlg.m_strInput)
						{
							break;
						}
					}
					if (j<itemList.GetSize())
					{
						AfxMessageBox(StrFromResID(IDS_CHK_ITEMNAME_REPEAT));
						return;
					}
					CCheckItem item;
					item.m_itemName = dlg.m_strInput;
					itemList.Add(item);

					HTREEITEM hitem0 = m_treeCtrl.InsertItem(item.m_itemName, 1, 1, hGroup, hSel == hGroup ? TVI_FIRST : hSel);
					m_treeCtrl.SetItemData(hitem0,TREE_ITEM_ITEM);
					m_pCheckScheme->Save();
	//				m_treeCtrl.Select(hitem0, TVGN_CARET);
	//				m_treeCtrl.SelectDropTarget(hitem0);
					m_treeCtrl.SelectItem(hitem0);
					CDlgChkConfig dlg;
					dlg.SetCurGroupAndItem(strGroup,item.m_itemName,TRUE);
					if(dlg.DoModal()==IDOK)
					{
						;
					}
//					SendMessage(WM_COMMAND,ID_CUSTOMIZE_CHKITEM);
				}
				else
				{
					throw 0;
				}
			}
			
		}
		
	}
}

void CCheckSchemeViewBar::OnDelChkGroup()
{
	if(!m_bActive||m_strSchemeDir.IsEmpty())
		return;
	HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
	if(hitem==NULL)
		return;
	if(m_treeCtrl.GetItemData(hitem)==TREE_ITEM_GROUP)
	{
		if(IDOK != AfxMessageBox(StrFromResID(IDS_DEL_CONFIRM),MB_OKCANCEL))
			return;

		CString str = m_treeCtrl.GetItemText(hitem);
		m_pCheckScheme->DelCheckGroup(str);
		m_treeCtrl.DeleteItem(hitem);
		m_pCheckScheme->Save();
	}
				
}

void CCheckSchemeViewBar::OnDelChkItem()
{
	if(!m_bActive||m_strSchemeDir.IsEmpty())
		return;
	HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
	if(hitem==NULL)
		return;
	if(m_treeCtrl.GetItemData(hitem)==TREE_ITEM_ITEM)
	{
		if(IDOK != AfxMessageBox(StrFromResID(IDS_DEL_CONFIRM),MB_OKCANCEL))
			return;

		CString strItem = m_treeCtrl.GetItemText(hitem);
		HTREEITEM parent = m_treeCtrl.GetParentItem(hitem);
		CString strGroup = m_treeCtrl.GetItemText(parent);
		
		int nNum = 0;
		CCheckGroup* pGroup = m_pCheckScheme->GetCheckGroup(nNum);
		if(nNum>0)
		{		
			for (int i=0;i<nNum;i++)
			{
				if(pGroup[i].m_groupName==strGroup)
					break;
			}
			if (i<nNum)
			{
				CArray<CCheckItem,CCheckItem&> &itemList = pGroup[i].m_itemList;
				for (int j=0;j<itemList.GetSize();j++)
				{
					if(itemList[j].m_itemName==strItem)
					{
						break;
					}
				}
				if (j<itemList.GetSize())
				{
					itemList.RemoveAt(j);
				}
			}
		}		
		m_treeCtrl.DeleteItem(hitem);
		m_pCheckScheme->Save();
	}
}

void CCheckSchemeViewBar::OnRenameChkGroup()
{
	if(!m_bActive||m_strSchemeDir.IsEmpty())
		return;
	HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
	if(hitem==NULL)
		return;
	if(m_treeCtrl.GetItemData(hitem)==TREE_ITEM_GROUP)
	{
		CString group = m_treeCtrl.GetItemText(hitem);
		CDlgInputStr dlg;
		dlg.m_strInput = group;
		dlg.SetTitle(StrFromResID(IDS_RENAME_CHK_GROUPNAME));
		if(dlg.DoModal()==IDOK&&!dlg.m_strInput.IsEmpty())
		{
			
			{
				int nNum = 0;
				CCheckGroup* pGroup = m_pCheckScheme->GetCheckGroup(nNum);
				if(nNum>0)
				{
					for (int i=0;i<nNum;i++)
					{
						if(pGroup[i].m_groupName==group)
							break;
					}
					if (i<nNum)
					{
						pGroup[i].m_groupName = dlg.m_strInput;
					}
				}
			}
		}		
		m_treeCtrl.SetItemText(hitem,dlg.m_strInput);
		m_pCheckScheme->Save();
	}
}

void CCheckSchemeViewBar::OnRenameChkItem()
{
	if(!m_bActive||m_strSchemeDir.IsEmpty())
		return;
	HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
	if(hitem==NULL)
		return;
	if(m_treeCtrl.GetItemData(hitem)==TREE_ITEM_ITEM)
	{
		CString strItem = m_treeCtrl.GetItemText(hitem);
		CDlgInputStr dlg;
		dlg.m_strInput = strItem;
		dlg.SetTitle(StrFromResID(IDS_RENAME_CHK_ITEMNAME));
		if(dlg.DoModal()==IDOK&&!dlg.m_strInput.IsEmpty())
		{
			
			HTREEITEM parent = m_treeCtrl.GetParentItem(hitem);
			CString strGroup = m_treeCtrl.GetItemText(parent);
			
			int nNum = 0;
			CCheckGroup* pGroup = m_pCheckScheme->GetCheckGroup(nNum);
			if(nNum>0)
			{		
				for (int i=0;i<nNum;i++)
				{
					if(pGroup[i].m_groupName==strGroup)
						break;
				}
				if (i<nNum)
				{
					CArray<CCheckItem,CCheckItem&> &itemList = pGroup[i].m_itemList;
					for (int j=0;j<itemList.GetSize();j++)
					{
						if(itemList[j].m_itemName==strItem)
						{
							break;
						}
					}
					if (j<itemList.GetSize())
					{
						itemList[j].m_itemName = dlg.m_strInput;
					}
				}
			}		
			m_treeCtrl.SetItemText(hitem,dlg.m_strInput);
			m_pCheckScheme->Save();
		}
		
	}
}

void CCheckSchemeViewBar::OnCustomizeChkItem()
{
	HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
	if (!hitem)return;
	if(m_treeCtrl.GetItemData(hitem)!=TREE_ITEM_ITEM)
		return;

	CString itemName =  m_treeCtrl.GetItemText(hitem);
	CString groupName = m_treeCtrl.GetItemText(m_treeCtrl.GetParentItem(hitem));

	CDlgChkConfig dlg;
	dlg.SetCurGroupAndItem(groupName,itemName,FALSE);
	if(dlg.DoModal()==IDOK)
	{
		CDlgDoc* pDoc = GetActiveDlgDoc();
		if( pDoc )
			pDoc->ExecuteCheck(groupName,itemName,FALSE);
	}
}

void CCheckSchemeViewBar::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM   hItem   =   m_treeCtrl.GetDropHilightItem();
	HTREEITEM  hSelect = m_treeCtrl.GetSelectedItem();
	if(hItem==NULL) hItem = hSelect;
	int state = m_treeCtrl.GetItemState(hItem, TVIS_SELECTED);
	if( 0 == (state&TVIS_SELECTED) )
	{
		m_treeCtrl.ClearSelection();
		m_treeCtrl.SelectItem(hItem); 
		m_treeCtrl.SelectDropTarget(hItem);
	}
	*pResult = 0;
}



void CCheckSchemeViewBar::OnDblClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM  hSelect = m_treeCtrl.GetSelectedItem();
	if(hSelect==NULL)
		return;
	
	*pResult = 0;
	OnCustomizeChkItem();
}

void CCheckSchemeViewBar::OnExecuteChkGroup()
{
	HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
	if (!hitem)return;
	if(m_treeCtrl.GetItemData(hitem)!=TREE_ITEM_GROUP)
		return;
	CString groupName = m_treeCtrl.GetItemText(hitem);
	CDlgDoc* pDoc = GetActiveDlgDoc();
	pDoc->ExecuteCheck(groupName,NULL,FALSE);	
//	AfxGetMainWnd()->SendMessage(FCCM_EXECUTECHKCMD,(WPARAM)(LPCTSTR)(groupName),0);
}

void CCheckSchemeViewBar::OnChkImportScript()
{
	CFileDialog dlg(TRUE,_T("*.chk"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("chk files(*.chk)|*.chk||"));
	if(dlg.DoModal()==IDOK)
	{	
		if(!m_pCheckScheme->Import(dlg.GetPathName()))
			GOutPut(StrFromResID(IDS_CHK_IMPORT_FAIL));	
		else
		{
			m_pCheckScheme->Save();
			RefreshSchemeView();
		}
	}
}

void CCheckSchemeViewBar::OnExecuteChkItem()
{
	HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
	if (!hitem)return;
	if(m_treeCtrl.GetItemData(hitem)!=TREE_ITEM_ITEM)
		return;

	CString itemName =  m_treeCtrl.GetItemText(hitem);
	CString groupName = m_treeCtrl.GetItemText(m_treeCtrl.GetParentItem(hitem));

	int nSel = m_treeCtrl.GetSelectCount();
	if(nSel>1)
	{
		CDlgDoc* pDoc = GetActiveDlgDoc();
		if( pDoc )
			pDoc->ExecuteCheck(groupName,itemName,FALSE);
	}
	else
	{
		CDlgChkConfig_One dlg;
		dlg.SetCurGroupAndItem(groupName,itemName);
		if(dlg.DoModal()==IDOK)
		{
			CDlgDoc* pDoc = GetActiveDlgDoc();
			if( pDoc )
				pDoc->ExecuteCheck(groupName,itemName,FALSE);
		}
	}
}



void CCheckSchemeViewBar::OnExecuteChkItemSelection()
{
	HTREEITEM hitem = m_treeCtrl.GetSelectedItem();
	if (!hitem)return;
	if(m_treeCtrl.GetItemData(hitem)!=TREE_ITEM_ITEM)
		return;

	CString itemName =  m_treeCtrl.GetItemText(hitem);
	CString groupName = m_treeCtrl.GetItemText(m_treeCtrl.GetParentItem(hitem));


	int nSel = m_treeCtrl.GetSelectCount();
	if(nSel>1)
	{
		CDlgDoc* pDoc = GetActiveDlgDoc();
		if( pDoc )
			pDoc->ExecuteCheck(groupName,itemName,FALSE);
	}
	else
	{
		CDlgChkConfig_One dlg;
		dlg.SetCurGroupAndItem(groupName,itemName);
		if(dlg.DoModal()==IDOK)
		{
			CDlgDoc* pDoc = GetActiveDlgDoc();
			if( pDoc )
				pDoc->ExecuteCheck(groupName,itemName,TRUE);
		}
	}
}

BOOL CCheckSchemeViewBar::IsItemSelected(LPCTSTR pGroup, LPCTSTR pItem)
{
	HTREEITEM hItem = m_treeCtrl.GetRootItem();
	while(hItem != NULL)
	{
		CString group = m_treeCtrl.GetItemText(hItem);
		if(group==pGroup) break;
		hItem = m_treeCtrl.GetNextSiblingItem(hItem);
	}

	HTREEITEM hChild = m_treeCtrl.GetChildItem(hItem);
	while(hChild != NULL)
	{
		CString item = m_treeCtrl.GetItemText(hChild);
		if(item==pItem) break;
		hChild = m_treeCtrl.GetNextSiblingItem(hChild);
	}

	if(hChild)
	{
		return ( m_treeCtrl.GetItemState(hChild,TVIS_SELECTED)&TVIS_SELECTED );
	}

	return FALSE;
}


BOOL CCheckSchemeViewBar::bInit = FALSE;

CString CCheckSchemeViewBar::oldScheme = _T("");