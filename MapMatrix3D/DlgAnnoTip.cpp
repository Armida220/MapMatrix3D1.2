// DlgAnnoTip.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "editbasedoc.h"
#include "DlgAnnoTip.h"
#include "DlgCommand.h"
#include "dlginputstr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  TREE_ITEM_GROUP 0
#define  TREE_ITEM_ITEM  1


BOOL CStringScoreMap::GetScore(CString item, float& scroe)
{
	for(int i=0; i<arr.GetSize(); i++)
	{
		if(item==arr[i].item)
		{
			if(arr[i].scroe<0)
				return FALSE;
			scroe = arr[i].scroe;
			return TRUE;
		}
	}
	return FALSE;
}

void CStringScoreMap::SetOrAddScore(CString item, float scroe)
{
	for(int i=0; i<arr.GetSize(); i++)
	{
		if(item==arr[i].item)
		{
			//arr[i].scroe = scroe;
			return;
		}
	}

	SCORE temp;
	temp.item = item;
	temp.scroe = scroe;
	arr.Add(temp);
}

void CStringScoreMap::DeleteScore(CString item)
{
	for(int i=0; i<arr.GetSize(); i++)
	{
		if(item==arr[i].item)
		{
			arr.RemoveAt(i);
			return;
		}
	}
}

void CStringScoreMap::DeleteAll()
{
	arr.RemoveAll();
}



/////////////////////////////////////////////////////////////////////////////
// CDlgAnnoTip dialog


CDlgAnnoTip::CDlgAnnoTip(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAnnoTip::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAnnoTip)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgAnnoTip::~CDlgAnnoTip()
{

}

void CDlgAnnoTip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAnnoTip)
	DDX_Control(pDX, IDC_TREE_TIPSTRS, m_treeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAnnoTip, CDialog)
	//{{AFX_MSG_MAP(CDlgAnnoTip)
//	ON_WM_CLOSE()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_TIPSTRS, OnNMRClickTree)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_TIPSTRS, OnDblClickTree)
//	ON_WM_CONTEXTMENU()
//	ON_COMMAND(ID_NEWCHKGROUP,OnNewGroup)
//	ON_COMMAND(ID_NEWCHKITEM,OnNewItem)
//	ON_COMMAND(ID_DEL_CHK_GROUP,OnDelGroup)
//	ON_COMMAND(ID_DEL_CHK_ITEM,OnDelItem)
//	ON_COMMAND(ID_RENAME_CHK_GROUP,OnRenameGroup)
//	ON_COMMAND(ID_RENAME_CHK_ITEM,OnRenameItem)
	ON_COMMAND(ID_EXECUTE_ITEM,OnExecuteItem)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAnnoTip message handlers

BOOL CDlgAnnoTip::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_filepath = GetConfigPath(TRUE);
	m_filepath += "\\Annotips.txt";
	DWORD dwStyles=GetWindowLong(m_treeCtrl.m_hWnd,GWL_STYLE);//获取树控制原风格
	dwStyles |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_treeCtrl.m_hWnd,GWL_STYLE,dwStyles);//设置风格
	m_treeCtrl.ModifyStyle(LVS_SORTASCENDING|LVS_SORTDESCENDING,0);

	Load();
	HTREEITEM hItem = m_treeCtrl.GetRootItem();
	m_treeCtrl.Expand(hItem,TVE_EXPAND);
	//ExpandAll();
	m_treeCtrl.SetItemHeight(20);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAnnoTip::Load()
{
	FILE *fp = fopen(m_filepath, "r");
	if(!fp) return;

	m_map.DeleteAll();
	char line[256];
	HTREEITEM hItem,hSubItem;
	while(!feof(fp))
	{
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);

		int len = strlen(line);
		if( len<=1 )continue;
		if(line[len-1]=='\n')
		{
			line[len-1] = '\0';//去掉末尾的换行符
		}

		if(line[0]!='\t')//组
		{
			hItem = m_treeCtrl.InsertItem(line,0,1,TVI_ROOT);
			m_treeCtrl.SetItemData(hItem, TREE_ITEM_GROUP);
		}
		else//项
		{
			int pos = 1;
			char *str = line;
			while (*(str + pos))
			{
				if (';'==*(str+pos)) break;
				pos++;
			}

			char str1[256];
			memset(str1, 0, sizeof(line));
			strncpy(str1, line + 1, pos - 1);

			float score = 0.0f;
			if (strlen(line + pos +1)>0)
				score = atof(line + pos+1);

			m_map.SetOrAddScore(str1, score);
			hSubItem = m_treeCtrl.InsertItem(str1, 0, 1, hItem);
			m_treeCtrl.SetItemData(hSubItem, TREE_ITEM_ITEM);
		}
	}
	fclose(fp);
}

void CDlgAnnoTip::ExpandAll()
{
	HTREEITEM hItem = m_treeCtrl.GetRootItem();
	while(hItem != NULL)
	{
		m_treeCtrl.Expand(hItem,TVE_EXPAND);
		hItem = m_treeCtrl.GetNextSiblingItem(hItem);
	}
}

/*void CDlgAnnoTip::Save()
{
	FILE *fp = fopen(m_filepath, "w");
	if(!fp) return;

	HTREEITEM hItem = m_treeCtrl.GetRootItem();
	while(hItem != NULL)
	{
		CString group = m_treeCtrl.GetItemText(hItem);
		fprintf(fp, "%s\n", group);
		HTREEITEM hChild = m_treeCtrl.GetChildItem(hItem);
		while(hChild != NULL)
		{
			CString item = m_treeCtrl.GetItemText(hChild);
			float score;
			if( m_map.GetScore(item, score) )
				fprintf(fp, "\t%s,%.2f\n", item, score);
			else
				fprintf(fp, "\t%s,0.00\n", item);
			hChild = m_treeCtrl.GetNextSiblingItem(hChild);
		}
		hItem = m_treeCtrl.GetNextSiblingItem(hItem);
	}

	fclose(fp);
}*/

BOOL CDlgAnnoTip::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		if(pMsg->wParam == VK_RETURN && pMsg->hwnd==this->m_hWnd)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// void CDlgAnnoTip::OnClose()
// {
// 	Save();
// 	EndDialog(IDOK);
// }

void CDlgAnnoTip::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM   hSelect   =   m_treeCtrl.GetSelectedItem();
	HTREEITEM   hLight   =   m_treeCtrl.GetDropHilightItem();
	if(hLight != NULL  &&  hSelect != hLight)
		hSelect = hLight;
	if(hSelect == NULL)
		return;
	m_treeCtrl.SelectItem(hSelect); 
	*pResult = 0;
}

void CDlgAnnoTip::OnDblClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	OnExecuteItem();
}

void CDlgAnnoTip::OnExecuteItem()
{
	HTREEITEM  hSel = m_treeCtrl.GetSelectedItem();
	if(hSel==NULL || m_treeCtrl.GetItemData(hSel)!=TREE_ITEM_ITEM)
		return;
	
	CString text = m_treeCtrl.GetItemText(hSel);
	text.TrimLeft();
	text.TrimRight();
	AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_ELEMENT_TEXT);
	
	CDlgDoc *pDoc = GetActiveDlgDoc();
	CFtrLayer *pLayer = pDoc->GetDlgDataSource()->GetOrCreateFtrLayer(StrFromResID(IDS_ANNOTIP_LAYER));
	if( pLayer ) pDoc->GetDlgDataSource()->SetCurFtrLayer(pLayer->GetID());
	CCommand* pCurCmd = pDoc->GetCurrentCommand();

	//删除采集查图注记的配置，以免命令不能复用
	CPlaceConfigLib *pPlaceLib = gpCfgLibMan->GetPlaceConfigLib(pDoc->GetDlgDataSource()->GetScale());
	if (pPlaceLib)
	{
		if(pPlaceLib->DeleteConfig(StrFromResID(IDS_ANNOTIP_LAYER)))
			pPlaceLib->Save();
	}

	if(pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawTextCommand)))
	{
		CValueTable tab;
		tab.BeginAddValueItem();
		_variant_t var;
		var = (_bstr_t)(LPCTSTR)text;
		tab.AddValue(PF_TEXT,&CVariantEx(var));
		tab.EndAddValueItem();
		pCurCmd->SetParams(tab, TRUE);
	}	
}

void CDlgAnnoTip::OnButton1() 
{
	// TODO: Add your control notification handler code here
	ShellExecute(this->GetSafeHwnd(),"open","notepad.exe",m_filepath,NULL,SW_NORMAL);
}

void CDlgAnnoTip::OnButton2() 
{
	// TODO: Add your control notification handler code here
	m_treeCtrl.DeleteAllItems();
	Load();
	HTREEITEM hItem = m_treeCtrl.GetRootItem();
	m_treeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgAnnoTip::OnButton3() 
{
	// TODO: Add your control notification handler code here
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(!pDoc) return;
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if(!pDS) return;
	CFtrLayer *pLayer = pDS->GetFtrLayer(StrFromResID(IDS_ANNOTIP_LAYER));

	int nObj = 0;
	float sum=0;
	if(pLayer)
	{
		nObj = pLayer->GetValidObjsCount();
		for(int i=0; i<nObj; i++)
		{
			CFeature *pFtr = pLayer->GetObject(i, FILTERMODE_DELETED);
			if(!pFtr) continue;
			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
				continue;

			CString item = ((CGeoText*)pGeo)->GetText();
			float score;
			if( m_map.GetScore(item, score) )
			{
				sum += score;
			}
		}
	}

	CString str;
	str.Format(IDS_ANNOTIP_OUTSTR, nObj, sum);
	GOutPut(str);
}