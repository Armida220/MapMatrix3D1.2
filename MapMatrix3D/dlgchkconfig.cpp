// dlgchkconfig.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "dlgchkconfig.h"
#include "editbasedoc.h"
#include "Markup.h "
#include "ObjectXmlIO.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChkConfig dialog


CDlgChkConfig::CDlgChkConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChkConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChkConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = NULL;
	m_pCurTaskList = NULL;
	m_pCheckScheme = NULL;
	m_strCurGroupName = _T("");
	m_strCurItemName = _T("");
	m_bNew = FALSE;
	m_pCurCmd = NULL;
}

CDlgChkConfig::~CDlgChkConfig()
{
	for (int i=m_arrChkCmd.GetSize()-1;i>=0;i--)
	{
		delete m_arrChkCmd[i];
	}
	m_arrChkCmd.RemoveAll();
	if (m_pCurCmd)
	{
		delete m_pCurCmd;
	}
}


void CDlgChkConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChkConfig)
	DDX_Control(pDX, IDC_LIST_CHK_CONFIG, m_listChkConfig);
	DDX_Control(pDX, IDC_TREE_CHK_CONFIG, m_treeChkConfig);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChkConfig, CDialog)
	//{{AFX_MSG_MAP(CDlgChkConfig)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(ID_BUTTON_DEL, OnButtonDel)
	ON_LBN_SELCHANGE(IDC_LIST_CHK_CONFIG, OnSelchangeListChkConfig)
	ON_BN_CLICKED(ID_BUTTON_SAVE, OnButtonSave)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_CHK_CONFIG, OnDblclkTreeChkConfig)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChkConfig message handlers

BOOL CDlgChkConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rect;
	GetDlgItem(IDC_POS)->GetWindowRect(rect);
	ScreenToClient(&rect);
	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rect, this, 2))
	{
		TRACE0("Failed to create Properies Grid \n");
		return -1;      // fail to create
	}
	m_pDoc = GetActiveDlgDoc();
	if(!m_pDoc)return FALSE;
	m_pCheckScheme = &GetCheckScheme();
	if(!m_pCheckScheme)return FALSE;

	//填充树形列表	
	int nRegs;
	const ChkCmdReg *pReg = m_pDoc->GetChkCmdRegs(nRegs);
	CString checkCategory,checkName;
	for (int i=0;i<nRegs;i++)
	{
		//寻找插入位置
		int nFindMode = 0;//0表示没有找到组名匹配；1表示仅仅找到组名的匹配;2表示找到完全匹配的组名和项名
		HTREEITEM hCurrent = m_treeChkConfig.GetRootItem();
		while (hCurrent != NULL)
		{
			// Get the text for the item. Notice we use TVIF_TEXT because
			// we want to retrieve only the text, but also specify TVIF_HANDLE
			// because we're getting the item by its handle.
			checkCategory = m_treeChkConfig.GetItemText(hCurrent);
			if (checkCategory==pReg[i].checkCategory)
			{
				HTREEITEM hCurrent0 = m_treeChkConfig.GetChildItem(hCurrent);
				while (hCurrent0 != NULL)
				{
					checkName = m_treeChkConfig.GetItemText(hCurrent0);
					if (checkName==pReg[i].checkName)
					{
						nFindMode = 2;
						break;
					}
					hCurrent0 = m_treeChkConfig.GetNextItem(hCurrent0, TVGN_NEXT);
				}
				if (nFindMode!=2)
				{
					nFindMode = 1;
				}			
				break;
			}			
			// Try to get the next item
			hCurrent = m_treeChkConfig.GetNextItem(hCurrent, TVGN_NEXT);			
		}
		if (nFindMode==0)
		{
			HTREEITEM hitem = m_treeChkConfig.InsertItem(pReg[i].checkCategory);
			m_treeChkConfig.SetItemData(hitem,0);
			if (hitem)
			{
				HTREEITEM hitem0 = m_treeChkConfig.InsertItem(pReg[i].checkName,hitem);
				m_treeChkConfig.SetItemData(hitem0,(DWORD_PTR)pReg[i].lpProc_Create);
			}
		}
		else if (nFindMode==1)
		{
			HTREEITEM hitem0 = m_treeChkConfig.InsertItem(pReg[i].checkName,hCurrent);
			m_treeChkConfig.SetItemData(hitem0, (DWORD_PTR)pReg[i].lpProc_Create);
		}
	}

	//填充检查任务列表
//	if (1)
	{
		//从方案中查找任务
		int nGroup;
		CCheckGroup* pGroup = m_pCheckScheme->GetCheckGroup(nGroup);
		for (int i=0;i<nGroup;i++)
		{
			if(pGroup[i].m_groupName==m_strCurGroupName)
			{
				CArray<CCheckItem,CCheckItem&> &refCheckItem = pGroup[i].m_itemList;
				for (int j=0;j<refCheckItem.GetSize();j++)
				{
					if (refCheckItem[j].m_itemName==m_strCurItemName)
					{
						m_pCurTaskList = &refCheckItem[j].m_taskList;
						break;
					}
				}
				break;
			}
		}
		if(m_pCurTaskList==NULL)
			return FALSE;
		for (i=0;i<m_pCurTaskList->GetSize();i++)
		{
			CChkCmd *pCmd = m_pDoc->CreateChkCmd((*m_pCurTaskList)[i].m_checkCategory,(*m_pCurTaskList)[i].m_checkName);
			if(!pCmd)continue;	
			m_arrChkCmd.Add(pCmd);
			m_arrSaveflag.Add(1);
			CMarkup xml;
			if(xml.SetDoc((*m_pCurTaskList)[i].m_checkParams))
			{		
				xml.FindElem(XMLTAG_CHKTASKPARAM);
				xml.IntoElem();
				CValueTable tab;				
				tab.BeginAddValueItem();
				Xml_ReadValueTable(xml,tab);
				tab.EndAddValueItem();
				xml.OutOfElem();
				pCmd->SetParams(tab);				
				xml.OutOfElem();
			}
			CString strTemp = (*m_pCurTaskList)[i].m_checkCategory;
			strTemp+=_T("->");
			strTemp+=(*m_pCurTaskList)[i].m_checkName;
			int idx = m_listChkConfig.AddString(strTemp);
			m_listChkConfig.SetItemData(idx, (DWORD_PTR)pCmd);
		}
	}
	// TODO: Add extra initialization here
	//{hcw,2013.1.6,Select the first item of m_listChkConfig
	if(m_listChkConfig.GetCount()>0)
	{
		m_listChkConfig.SetCurSel(0);
		CChkCmd *pCmd = (CChkCmd *)m_listChkConfig.GetItemData(0);
		RefreshParams(pCmd);
	}
	//}
	CMFCHeaderCtrl& headCtrl = m_wndPropList.GetHeaderCtrl();
	HDITEM hdItem;
	hdItem.mask = HDI_TEXT;
	CString str = StrFromResID(IDS_CHK_PARAM_ATTRI_NAME);
	hdItem.pszText = (LPSTR)(LPCTSTR)str;
	headCtrl.SetItem(0,&hdItem);
	str = StrFromResID(IDS_CHK_PARAM_ATTRI_VALUE);
	hdItem.pszText = (LPSTR)(LPCTSTR)str;
	headCtrl.SetItem(1,&hdItem);
	GetDlgItem(ID_BUTTON_SAVE)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChkConfig::OnButtonAdd() 
{
	HTREEITEM hitem = m_treeChkConfig.GetSelectedItem();
	if (!hitem) return;
	LPPROC_CHKCMDCREATE lpProc_Create = (LPPROC_CHKCMDCREATE)m_treeChkConfig.GetItemData(hitem);
	if (lpProc_Create==0)
	{
		AfxMessageBox(StrFromResID(IDS_CHK_ERROR_NODE));
		return;
	}
	CChkCmd *pCmd = (*lpProc_Create)();
	if(!pCmd)
		return;
	
	//不允许同样的检查命令被再次添加
	for (int i=0;i<m_arrChkCmd.GetSize();i++)
	{
		if (pCmd->GetCheckCategory()==m_arrChkCmd[i]->GetCheckCategory()&&
			pCmd->GetCheckName()==m_arrChkCmd[i]->GetCheckName())
		{
			break;
		}
	}
	if (i<m_arrChkCmd.GetSize())
	{
		delete pCmd;
		return;
	}
	if(m_pCurCmd)delete m_pCurCmd;
	m_pCurCmd = (*lpProc_Create)();
	if(!m_pCurCmd)
	{
		delete pCmd;
		return;
	}
	m_arrChkCmd.Add(pCmd);
	m_arrSaveflag.Add(1);
	GetDlgItem(ID_BUTTON_SAVE)->EnableWindow(FALSE);
	CUIParam param0;
	pCmd->FillShowParams(&param0,TRUE);
	m_wndPropList.LoadParams(&param0);
	
	CUIParam param;
	m_pCurCmd->FillShowParams(&param);
	m_wndPropList.ShowParams(&param);
	CValueTable tab;
	tab.BeginAddValueItem();
	m_pCurCmd->GetParams(tab);
	tab.EndAddValueItem();
	CMarkup xml;
	xml.AddElem(XMLTAG_CHKTASKPARAM);
	xml.IntoElem();
	Xml_WriteValueTable(xml,tab);
	xml.OutOfElem();
	CCheckTask task;
	task.m_checkCategory = pCmd->GetCheckCategory();
	task.m_checkName = pCmd->GetCheckName();
	task.m_checkParams = xml.GetSubDoc();

	m_pCurTaskList->Add(task);
	
	CString strTemp = task.m_checkCategory;
	strTemp+=_T("->");
	strTemp+=task.m_checkName;
	int idx = m_listChkConfig.AddString(strTemp);
	m_listChkConfig.SetItemData(idx, (DWORD_PTR)pCmd);
	m_listChkConfig.SetCurSel(idx);
	m_pCheckScheme->Save();
}

void CDlgChkConfig::OnButtonDel() 
{
	int idx = m_listChkConfig.GetCurSel();
	if (idx!=LB_ERR)
	{
		CChkCmd* pCmd = (CChkCmd*)m_listChkConfig.GetItemData(idx);
		if (pCmd)
		{
			for (int i=m_pCurTaskList->GetSize()-1;i>=0;i--)
			{
				if (pCmd->GetCheckCategory()==(*m_pCurTaskList)[i].m_checkCategory&&
					pCmd->GetCheckName()==(*m_pCurTaskList)[i].m_checkName)
				{
					m_pCurTaskList->RemoveAt(i);
					break;
				}
				
			}
			for (int j=0;j<m_arrChkCmd.GetSize();j++)
			{
				if (pCmd==m_arrChkCmd[j])
				{
					m_arrChkCmd.RemoveAt(j);
					m_arrSaveflag.RemoveAt(j);
				}
			}
			delete pCmd;
			if(m_pCurCmd)
				delete m_pCurCmd;
			m_pCurCmd = NULL;
			m_listChkConfig.DeleteString(idx);
			//{hcw,2013.1.7,选中下一项
			m_listChkConfig.SetCurSel(idx);
			OnSelchangeListChkConfig() ;
			//}
		}
		//{hcw,2013.1.7,添加if语句判断。
		if (m_listChkConfig.GetCount()<=0
			||m_listChkConfig.GetCurSel()<0)
		{
			m_wndPropList.RemoveAll(); 
		}
		//}
		m_pCheckScheme->Save();
// 		if(m_listChkConfig.GetCount()>0)
// 			m_listChkConfig.SetCurSel(0);
		m_wndPropList.RedrawWindow();
		GetDlgItem(ID_BUTTON_SAVE)->EnableWindow(FALSE);
	}
	
}

void CDlgChkConfig::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}


void CDlgChkConfig::OnSelchangeListChkConfig() 
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_listChkConfig.GetCurSel();
	//{hcw,2013.1.6
	if (nCurSel<0)
	{
		return;
	}
	//}
	CChkCmd *pCmd = (CChkCmd *)m_listChkConfig.GetItemData(nCurSel);

	for (int i=0;i<m_arrChkCmd.GetSize();i++)
	{
		if (pCmd->GetCheckCategory()==m_arrChkCmd[i]->GetCheckCategory()&&
			pCmd->GetCheckName()==m_arrChkCmd[i]->GetCheckName())
		{
			break;
		}
	}
	
	if (i<m_arrChkCmd.GetSize())
	{		
		if(m_arrSaveflag[i]==1)
			GetDlgItem(ID_BUTTON_SAVE)->EnableWindow(FALSE);
		else 
			GetDlgItem(ID_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	else
		return ;
	if(m_pCurCmd)
		delete m_pCurCmd;
	//m_pCurCmd = (*lpProc_Create)();
	m_pCurCmd = m_pDoc->CreateChkCmd(pCmd->GetCheckCategory(),pCmd->GetCheckName());
	CValueTable tab;
	tab.BeginAddValueItem();
	pCmd->GetParams(tab);
	tab.EndAddValueItem();
	m_pCurCmd->SetParams(tab);	
	CUIParam param0;
	m_pCurCmd->FillShowParams(&param0,TRUE);
	m_wndPropList.LoadParams(&param0);
	
 	CUIParam param;
 	m_pCurCmd->FillShowParams(&param);
	m_wndPropList.ShowParams(&param);
}

void CDlgChkConfig::OnButtonSave() 
{
	int nCurSel = m_listChkConfig.GetCurSel();
	CChkCmd *pCmd = (CChkCmd *)m_listChkConfig.GetItemData(nCurSel);	
// 	CValueTable tab;
// 	tab.BeginAddValueItem();
// 	m_wndPropList.GetCmdParams(tab);
// 	tab.EndAddValueItem();
// 
// 	CValueTable oldtab;
// 	oldtab.BeginAddValueItem();
// 	pCmd->GetParams(oldtab);
// 	oldtab.EndAddValueItem();
// 
// 	pCmd->SetParams(tab);
	CString reason;
	if(!m_pCurCmd->CheckParamsValid(reason))
	{
//		pCmd->SetParams(oldtab);
		
		CValueTable tab;
		tab.BeginAddValueItem();
		pCmd->GetParams(tab);
		tab.EndAddValueItem();
		m_pCurCmd->SetParams(tab);	
		AfxMessageBox(reason);
		for (int i=0;i<m_arrChkCmd.GetSize();i++)
		{
			if (m_pCurCmd->GetCheckCategory()==m_arrChkCmd[i]->GetCheckCategory()&&
				m_pCurCmd->GetCheckName()==m_arrChkCmd[i]->GetCheckName())
			{
				break;
			}
		}
		if (i<m_arrChkCmd.GetSize())
		{
			m_arrSaveflag[i] = 1;
			GetDlgItem(ID_BUTTON_SAVE)->EnableWindow(FALSE);
			
		}
		ASSERT(pCmd==m_arrChkCmd[i]);
		CUIParam param;
		m_pCurCmd->FillShowParams(&param);
		m_wndPropList.ShowParams(&param);
		return;
	}
	CValueTable tab;
	tab.BeginAddValueItem();
	m_pCurCmd->GetParams(tab);
	tab.EndAddValueItem();
	pCmd->SetParams(tab);
	CMarkup xml;
	xml.AddElem(XMLTAG_CHKTASKPARAM);
	xml.IntoElem();
	Xml_WriteValueTable(xml,tab);
	xml.OutOfElem();
	for (int i=m_pCurTaskList->GetSize()-1;i>=0;i--)
	{
		if (pCmd->GetCheckCategory()==(*m_pCurTaskList)[i].m_checkCategory&&
			pCmd->GetCheckName()==(*m_pCurTaskList)[i].m_checkName)
		{
			(*m_pCurTaskList)[i].m_checkParams = xml.GetSubDoc();
			break;
		}		
	}
	m_pCheckScheme->Save();
	for ( i=0;i<m_arrChkCmd.GetSize();i++)
	{
		if (pCmd->GetCheckCategory()==m_arrChkCmd[i]->GetCheckCategory()&&
			pCmd->GetCheckName()==m_arrChkCmd[i]->GetCheckName())
		{
			break;
		}
	}
	
	if (i<m_arrChkCmd.GetSize())
	{
		m_arrSaveflag[i] = 1;
		GetDlgItem(ID_BUTTON_SAVE)->EnableWindow(FALSE);
	}
	else
		return ;
}


LRESULT CDlgChkConfig::OnPropertyChanged (WPARAM wParam, LPARAM lParam)
{
	if( !m_wndPropList.GetOriginalParams() )return 0;
	
	CUIFProp *pProp = (CUIFProp*)lParam;
	
	CString name = pProp->GetName();
	CUIParam::ParamItem item;
	if( m_wndPropList.GetOriginalParams()->GetParamByTitle(name,item)<0 )
		return 0;
	
	if( item.type==CUIParam::NoneType )
		return 0;
	
	_variant_t var;
	
	switch( item.type )
	{
	case CUIParam::BoolType:
		//var = (bool)(((CUIFPropEx*)pProp)->GetCurSelOfCombo()==0?true:false);
		var = pProp->GetValue();
		break;
	case CUIParam::IntType:	
	case CUIParam::FloatType:
	case CUIParam::DoubleType:
	case CUIParam::StringType:
	case CUIParam::LayerNameType:
	case CUIParam::UsedLayerNameType:
	case CUIParam::MutiLayerNameType:
	case CUIParam::CheckListType:
	case CUIParam::FileNameType:
	case CUIParam::PathNameType:
		var = pProp->GetValue();
		break;
	case CUIParam::OptionType:
		var = (long)item.data.pOptions->GetAt((long)((CUIFPropEx*)pProp)->GetCurSelOfCombo()).value;
		break;		
	}
	
	CValueTable tab;
	tab.BeginAddValueItem();
	tab.AddValue(item.field,&CVariantEx(var));
	tab.EndAddValueItem();
	int nCurSel = m_listChkConfig.GetCurSel();
	CChkCmd *pCmd = (CChkCmd *)m_listChkConfig.GetItemData(nCurSel);

	//不允许同样的检查命令被再次添加
	for (int i=0;i<m_arrChkCmd.GetSize();i++)
	{
		if (pCmd->GetCheckCategory()==m_arrChkCmd[i]->GetCheckCategory()&&
			pCmd->GetCheckName()==m_arrChkCmd[i]->GetCheckName())
		{
			break;
		}
	}
	
	if (i<m_arrChkCmd.GetSize())
	{
		m_arrSaveflag[i] = 0;
		GetDlgItem(ID_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	else
		return 0;
// 	CValueTable oldtab;
// 	oldtab.BeginAddValueItem();
// 	pCmd->GetParams(oldtab);
// 	oldtab.EndAddValueItem();
	m_pCurCmd->SetParams(tab);
	CUIParam param;
	m_pCurCmd->FillShowParams(&param);
	pProp->OnEndEdit();
	m_wndPropList.ShowParams(&param);	
	//pCmd->SetParams(oldtab);

	return 0;
}

void CDlgChkConfig::OnDblclkTreeChkConfig(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hitem = m_treeChkConfig.GetSelectedItem();
	LPPROC_CHKCMDCREATE lpProc_Create = (LPPROC_CHKCMDCREATE)m_treeChkConfig.GetItemData(hitem);
	if (lpProc_Create==0)
	{		
		return;
	}
	CChkCmd *pCmd = (lpProc_Create)();
	if(!pCmd)
		return;
	//不允许同样的检查命令被再次添加
	for (int i=0;i<m_arrChkCmd.GetSize();i++)
	{
		if (pCmd->GetCheckCategory()==m_arrChkCmd[i]->GetCheckCategory()&&
			pCmd->GetCheckName()==m_arrChkCmd[i]->GetCheckName())
		{
			break;
		}
	}
	if (i<m_arrChkCmd.GetSize())
	{
		delete pCmd;
		return;
	}
	if(m_pCurCmd)delete m_pCurCmd;
	m_pCurCmd = (*lpProc_Create)();
	if(!m_pCurCmd)
	{
		delete pCmd;
		return;
	}
	m_arrChkCmd.Add(pCmd);
	m_arrSaveflag.Add(1);
	GetDlgItem(ID_BUTTON_SAVE)->EnableWindow(FALSE);
	CUIParam param0;
	m_pCurCmd->FillShowParams(&param0,TRUE);
	m_wndPropList.LoadParams(&param0);
	
	CUIParam param;
	m_pCurCmd->FillShowParams(&param);
	m_wndPropList.ShowParams(&param);
	CValueTable tab;
	tab.BeginAddValueItem();
	m_pCurCmd->GetParams(tab);
	tab.EndAddValueItem();
	CMarkup xml;
	xml.AddElem(XMLTAG_CHKTASKPARAM);
	xml.IntoElem();
	Xml_WriteValueTable(xml,tab);
	xml.OutOfElem();
	CCheckTask task;
	task.m_checkCategory = pCmd->GetCheckCategory();
	task.m_checkName = pCmd->GetCheckName();
	task.m_checkParams = xml.GetSubDoc();
	
	m_pCurTaskList->Add(task);
	
	CString strTemp = task.m_checkCategory;
	strTemp+=_T("->");
	strTemp+=task.m_checkName;
	int idx = m_listChkConfig.AddString(strTemp);
	m_listChkConfig.SetItemData(idx, (DWORD_PTR)pCmd);
	m_listChkConfig.SetCurSel(idx);
	m_pCheckScheme->Save();
	*pResult = 0;
}
//hcw,2013.1.6,更新检查项的参数设置。
void CDlgChkConfig::RefreshParams( CChkCmd* pCmd )
{
	if (pCmd==NULL)
	{
		return;
	}
	if (m_pCurCmd)
	{
		delete m_pCurCmd;
	}
	m_pCurCmd = m_pDoc->CreateChkCmd(pCmd->GetCheckCategory(), pCmd->GetCheckName());
	CValueTable tab;
	tab.BeginAddValueItem();
	pCmd->GetParams(tab);
	tab.EndAddValueItem();
	m_pCurCmd->SetParams(tab);
	CUIParam param0;
	m_pCurCmd->FillShowParams(&param0,TRUE);
	m_wndPropList.LoadParams(&param0);

	CUIParam param;
	m_pCurCmd->FillShowParams(&param);
	m_wndPropList.ShowParams(&param);
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CDlgChkConfig_One dialog


CDlgChkConfig_One::CDlgChkConfig_One(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChkConfig_One::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChkConfig_One)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = NULL;
	m_pCurTaskList = NULL;
	m_pCheckScheme = NULL;
	m_strCurGroupName = _T("");
	m_strCurItemName = _T("");
	m_pCurCmd = NULL;
}

CDlgChkConfig_One::~CDlgChkConfig_One()
{
	for (int i=m_arrChkCmd.GetSize()-1;i>=0;i--)
	{
		delete m_arrChkCmd[i];
	}
	m_arrChkCmd.RemoveAll();
	if (m_pCurCmd)
	{
		delete m_pCurCmd;
	}
}


void CDlgChkConfig_One::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChkConfig_One)
	DDX_Control(pDX, IDC_LIST_CHK_CONFIG, m_listChkConfig);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChkConfig_One, CDialog)
	//{{AFX_MSG_MAP(CDlgChkConfig_One)
	ON_LBN_SELCHANGE(IDC_LIST_CHK_CONFIG, OnSelchangeListChkConfig)
	ON_BN_CLICKED(ID_BUTTON_SAVE, OnButtonSave)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChkConfig_One message handlers

BOOL CDlgChkConfig_One::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rect;
	GetDlgItem(IDC_POS)->GetWindowRect(rect);
	ScreenToClient(&rect);
	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rect, this, 2))
	{
		TRACE0("Failed to create Properies Grid \n");
		return -1;      // fail to create
	}
	m_pDoc = GetActiveDlgDoc();
	if(!m_pDoc)return FALSE;
	m_pCheckScheme = &GetCheckScheme();
	if(!m_pCheckScheme)return FALSE;

	//填充检查任务列表
//	if (1)
	{
		//从方案中查找任务
		int nGroup;
		CCheckGroup* pGroup = m_pCheckScheme->GetCheckGroup(nGroup);
		for (int i=0;i<nGroup;i++)
		{
			if(pGroup[i].m_groupName==m_strCurGroupName)
			{
				CArray<CCheckItem,CCheckItem&> &refCheckItem = pGroup[i].m_itemList;
				for (int j=0;j<refCheckItem.GetSize();j++)
				{
					if (refCheckItem[j].m_itemName==m_strCurItemName)
					{
						m_pCurTaskList = &refCheckItem[j].m_taskList;
						break;
					}
				}
				break;
			}
		}
		if(m_pCurTaskList==NULL)
			return FALSE;
		for (i=0;i<m_pCurTaskList->GetSize();i++)
		{
			CChkCmd *pCmd = m_pDoc->CreateChkCmd((*m_pCurTaskList)[i].m_checkCategory,(*m_pCurTaskList)[i].m_checkName);
			if(!pCmd)continue;	
			m_arrChkCmd.Add(pCmd);
			m_arrSaveflag.Add(1);
			CMarkup xml;
			if(xml.SetDoc((*m_pCurTaskList)[i].m_checkParams))
			{		
				xml.FindElem(XMLTAG_CHKTASKPARAM);
				xml.IntoElem();
				CValueTable tab;				
				tab.BeginAddValueItem();
				Xml_ReadValueTable(xml,tab);
				tab.EndAddValueItem();
				xml.OutOfElem();
				pCmd->SetParams(tab);				
				xml.OutOfElem();
			}
			CString strTemp = (*m_pCurTaskList)[i].m_checkCategory;
			strTemp+=_T("->");
			strTemp+=(*m_pCurTaskList)[i].m_checkName;
			int idx = m_listChkConfig.AddString(strTemp);
			m_listChkConfig.SetItemData(idx, (DWORD_PTR)pCmd);
		}
	}
	// TODO: Add extra initialization here
	//{hcw,2013.1.6,Select the first item of m_listChkConfig
	if(m_listChkConfig.GetCount()>0)
	{
		m_listChkConfig.SetCurSel(0);
		CChkCmd *pCmd = (CChkCmd *)m_listChkConfig.GetItemData(0);
		RefreshParams(pCmd);
	}
	//}
	CMFCHeaderCtrl& headCtrl = m_wndPropList.GetHeaderCtrl();
	HDITEM hdItem;
	hdItem.mask = HDI_TEXT;
	hdItem.pszText = (LPSTR)(LPCTSTR)StrFromResID(IDS_CHK_PARAM_ATTRI_NAME);
	headCtrl.SetItem(0,&hdItem);
	hdItem.pszText = (LPSTR)(LPCTSTR)StrFromResID(IDS_CHK_PARAM_ATTRI_VALUE);
	headCtrl.SetItem(1,&hdItem);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgChkConfig_One::OnOK() 
{
	m_wndPropList.EndEditItem();

	OnButtonSave();
	
	CDialog::OnOK();
}


void CDlgChkConfig_One::OnSelchangeListChkConfig() 
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_listChkConfig.GetCurSel();
	//{hcw,2013.1.6
	if (nCurSel<0)
	{
		return;
	}
	//}
	CChkCmd *pCmd = (CChkCmd *)m_listChkConfig.GetItemData(nCurSel);

	for (int i=0;i<m_arrChkCmd.GetSize();i++)
	{
		if (pCmd->GetCheckCategory()==m_arrChkCmd[i]->GetCheckCategory()&&
			pCmd->GetCheckName()==m_arrChkCmd[i]->GetCheckName())
		{
			break;
		}
	}
	
	if (i<m_arrChkCmd.GetSize())
	{
	}
	else
		return ;
	if(m_pCurCmd)
		delete m_pCurCmd;
	//m_pCurCmd = (*lpProc_Create)();
	m_pCurCmd = m_pDoc->CreateChkCmd(pCmd->GetCheckCategory(),pCmd->GetCheckName());
	CValueTable tab;
	tab.BeginAddValueItem();
	pCmd->GetParams(tab);
	tab.EndAddValueItem();
	m_pCurCmd->SetParams(tab);	
	CUIParam param0;
	m_pCurCmd->FillShowParams(&param0,TRUE);
	m_wndPropList.LoadParams(&param0);
	
 	CUIParam param;
 	m_pCurCmd->FillShowParams(&param);
	m_wndPropList.ShowParams(&param);
}

void CDlgChkConfig_One::OnButtonSave() 
{
	int nCurSel = m_listChkConfig.GetCurSel();
	CChkCmd *pCmd = (CChkCmd *)m_listChkConfig.GetItemData(nCurSel);	
// 	CValueTable tab;
// 	tab.BeginAddValueItem();
// 	m_wndPropList.GetCmdParams(tab);
// 	tab.EndAddValueItem();
// 
// 	CValueTable oldtab;
// 	oldtab.BeginAddValueItem();
// 	pCmd->GetParams(oldtab);
// 	oldtab.EndAddValueItem();
// 
// 	pCmd->SetParams(tab);
	CString reason;
	if(!m_pCurCmd->CheckParamsValid(reason))
	{
//		pCmd->SetParams(oldtab);
		
		CValueTable tab;
		tab.BeginAddValueItem();
		pCmd->GetParams(tab);
		tab.EndAddValueItem();
		m_pCurCmd->SetParams(tab);	
		AfxMessageBox(reason);
		for (int i=0;i<m_arrChkCmd.GetSize();i++)
		{
			if (m_pCurCmd->GetCheckCategory()==m_arrChkCmd[i]->GetCheckCategory()&&
				m_pCurCmd->GetCheckName()==m_arrChkCmd[i]->GetCheckName())
			{
				break;
			}
		}
		if (i<m_arrChkCmd.GetSize())
		{
			m_arrSaveflag[i] = 1;			
		}
		ASSERT(pCmd==m_arrChkCmd[i]);
		CUIParam param;
		m_pCurCmd->FillShowParams(&param);
		m_wndPropList.ShowParams(&param);
		return;
	}
	CValueTable tab;
	tab.BeginAddValueItem();
	m_pCurCmd->GetParams(tab);
	tab.EndAddValueItem();
	pCmd->SetParams(tab);
	CMarkup xml;
	xml.AddElem(XMLTAG_CHKTASKPARAM);
	xml.IntoElem();
	Xml_WriteValueTable(xml,tab);
	xml.OutOfElem();
	for (int i=m_pCurTaskList->GetSize()-1;i>=0;i--)
	{
		if (pCmd->GetCheckCategory()==(*m_pCurTaskList)[i].m_checkCategory&&
			pCmd->GetCheckName()==(*m_pCurTaskList)[i].m_checkName)
		{
			(*m_pCurTaskList)[i].m_checkParams = xml.GetSubDoc();
			break;
		}		
	}
	m_pCheckScheme->Save();
	for ( i=0;i<m_arrChkCmd.GetSize();i++)
	{
		if (pCmd->GetCheckCategory()==m_arrChkCmd[i]->GetCheckCategory()&&
			pCmd->GetCheckName()==m_arrChkCmd[i]->GetCheckName())
		{
			break;
		}
	}
	
	if (i<m_arrChkCmd.GetSize())
	{
		m_arrSaveflag[i] = 1;
	}
	else
		return ;
}


LRESULT CDlgChkConfig_One::OnPropertyChanged (WPARAM wParam, LPARAM lParam)
{
	if( !m_wndPropList.GetOriginalParams() )return 0;
	
	CUIFProp *pProp = (CUIFProp*)lParam;
	
	CString name = pProp->GetName();
	CUIParam::ParamItem item;
	if( m_wndPropList.GetOriginalParams()->GetParamByTitle(name,item)<0 )
		return 0;
	
	if( item.type==CUIParam::NoneType )
		return 0;
	
	_variant_t var;
	
	switch( item.type )
	{
	case CUIParam::BoolType:
		//var = (bool)(((CUIFPropEx*)pProp)->GetCurSelOfCombo()==0?true:false);
		var = pProp->GetValue();
		break;
	case CUIParam::IntType:	
	case CUIParam::FloatType:
	case CUIParam::DoubleType:
	case CUIParam::StringType:
	case CUIParam::LayerNameType:
	case CUIParam::UsedLayerNameType:
	case CUIParam::MutiLayerNameType:
	case CUIParam::CheckListType:
	case CUIParam::FileNameType:
	case CUIParam::PathNameType:
		var = pProp->GetValue();
		break;
	case CUIParam::OptionType:
		var = (long)item.data.pOptions->GetAt((long)((CUIFPropEx*)pProp)->GetCurSelOfCombo()).value;
		break;		
	}
	
	CValueTable tab;
	tab.BeginAddValueItem();
	tab.AddValue(item.field,&CVariantEx(var));
	tab.EndAddValueItem();
	int nCurSel = m_listChkConfig.GetCurSel();
	CChkCmd *pCmd = (CChkCmd *)m_listChkConfig.GetItemData(nCurSel);

	//不允许同样的检查命令被再次添加
	for (int i=0;i<m_arrChkCmd.GetSize();i++)
	{
		if (pCmd->GetCheckCategory()==m_arrChkCmd[i]->GetCheckCategory()&&
			pCmd->GetCheckName()==m_arrChkCmd[i]->GetCheckName())
		{
			break;
		}
	}
	
	if (i<m_arrChkCmd.GetSize())
	{
		m_arrSaveflag[i] = 0;
	}
	else
		return 0;
// 	CValueTable oldtab;
// 	oldtab.BeginAddValueItem();
// 	pCmd->GetParams(oldtab);
// 	oldtab.EndAddValueItem();
	m_pCurCmd->SetParams(tab);
	CUIParam param;
	m_pCurCmd->FillShowParams(&param);
	pProp->OnEndEdit();
	m_wndPropList.ShowParams(&param);	
	//pCmd->SetParams(oldtab);

	return 0;
}

//hcw,2013.1.6,更新检查项的参数设置。
void CDlgChkConfig_One::RefreshParams( CChkCmd* pCmd )
{
	if (pCmd==NULL)
	{
		return;
	}
	if (m_pCurCmd)
	{
		delete m_pCurCmd;
	}
	m_pCurCmd = m_pDoc->CreateChkCmd(pCmd->GetCheckCategory(), pCmd->GetCheckName());
	CValueTable tab;
	tab.BeginAddValueItem();
	pCmd->GetParams(tab);
	tab.EndAddValueItem();
	m_pCurCmd->SetParams(tab);
	CUIParam param0;
	m_pCurCmd->FillShowParams(&param0,TRUE);
	m_wndPropList.LoadParams(&param0);

	CUIParam param;
	m_pCurCmd->FillShowParams(&param);
	m_wndPropList.ShowParams(&param);
	return;
}
