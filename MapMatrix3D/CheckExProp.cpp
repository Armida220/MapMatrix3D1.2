// CheckExProp.cpp : implementation file
//

#include "stdafx.h"
#include "CheckExProp.h"
#include "EditBaseDoc.h"
#include "NewCellTypes/GridCellCombo.h"
#include "BaseView.h"
#include "GeoText.h"
#include "UIFToolbarComboBoxButtonEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PAGE_SIZE 1000

extern CString VarToString(_variant_t& var);

static CString sortField = _T("");
static BOOL bAscending = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Field::Field()
{
	m_pScheme = NULL;
	m_rules=NULL;
	m_fields_num = 0;
}

Field::~Field()
{
	if(m_rules)
	{
		delete[] m_rules;
	}
}

bool Field::Init(int nScale)
{
	//获取配置
	CConfigLibManager *pConfig = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	m_pScheme = pConfig->GetScheme(nScale);
	if(!m_pScheme)
	{
		return false;
	}
	
	return true;
}

void Field::CleanUp()//清理，以便载入新的层信息
{
	//fields.RemoveAll();
	m_fields_num = 0;
	if(m_rules)
	{
		delete[] m_rules;
		m_rules = 0;
	}
}

extern BOOL convertStringToStrArray(LPCTSTR str,CStringArray &arr);

//获取某层的信息：扩展属性的名称和规则
int Field::getFieldInfo(LPCTSTR layername, BOOL byDbName)
{
	if (byDbName)
	{
		CArray<const XDefine*, const XDefine*> xdefs;
		CString dblayname = layername;
		m_fields_num = m_pScheme->GetDBLayerXdefines(dblayname, xdefs);
		if (m_fields_num == 0) return 0;

		m_rules = new FieldRule[m_fields_num];

		for (int i = 0; i < m_fields_num; i++)
		{
			CString str;
			str.Format(_T("%s"), xdefs[i]->valueRange);

			m_rules[i].RuleType = -1;//初始化为无规则
			m_rules[i].field_name = xdefs[i]->field;
			m_rules[i].MustSel = xdefs[i]->isMust;

			if (str.IsEmpty())
			{
				m_rules[i].RuleType = 0;
				continue;
			}

			int pos = str.Find('~');
			if (pos == -1)
			{
				if (convertStringToStrArray(str, m_rules[i].EVal))
				{
					m_rules[i].RuleType = 1;
				}
				else
				{
					m_rules[i].RuleType = 0;
				}
			}
			else if (pos == 0)
			{
				m_rules[i].RuleType = 2;
				m_rules[i].field_name = xdefs[i]->field;
				m_rules[i].minVal = -1e20;
				CString temp = str.Right(str.GetLength() - 1);
				m_rules[i].maxVal = strtod(temp, NULL);
			}
			else if (pos == str.GetLength() - 1)
			{
				m_rules[i].RuleType = 2;
				m_rules[i].field_name = xdefs[i]->field;
				m_rules[i].minVal = strtod(str, NULL);
				m_rules[i].maxVal = 1e20;
			}
			else
			{
				m_rules[i].RuleType = 2;
				m_rules[i].field_name = xdefs[i]->field;
				m_rules[i].minVal = strtod(str, NULL);
				CString temp = str.Right(str.GetLength() - pos - 1);
				m_rules[i].maxVal = strtod(temp, NULL);
			}
		}
	}
	else
	{
		CSchemeLayerDefine *pLayDef = m_pScheme->GetLayerDefine(layername);
		if (!pLayDef)
		{
			return 0;
		}
		int idx = pLayDef->FindXDefineByName(layername);
		const XDefine *xdefs = pLayDef->GetXDefines(m_fields_num);
		if (m_fields_num <= 0) return 0;

		m_rules = new FieldRule[m_fields_num];

		int i = 0;
		for (i = 0; i < m_fields_num; i++)
		{
			CString str;
			str.Format(_T("%s"), xdefs[i].valueRange);

			m_rules[i].RuleType = -1;//初始化为无规则
			m_rules[i].field_name = xdefs[i].field;
			m_rules[i].MustSel = xdefs[i].isMust;

			if (str.IsEmpty())
			{
				m_rules[i].RuleType = 0;
				continue;
			}

			int pos = str.Find('~');
			if (pos == -1)
			{
				if (convertStringToStrArray(str, m_rules[i].EVal))
				{
					m_rules[i].RuleType = 1;
				}
				else
				{
					m_rules[i].RuleType = 0;
				}
			}
			else if (pos == 0)
			{
				m_rules[i].RuleType = 2;
				m_rules[i].field_name = xdefs[i].field;
				m_rules[i].minVal = -1e20;
				CString temp = str.Right(str.GetLength() - 1);
				m_rules[i].maxVal = strtod(temp, NULL);
			}
			else if (pos == str.GetLength() - 1)
			{
				m_rules[i].RuleType = 2;
				m_rules[i].field_name = xdefs[i].field;
				m_rules[i].minVal = strtod(str, NULL);
				m_rules[i].maxVal = 1e20;
			}
			else
			{
				m_rules[i].RuleType = 2;
				m_rules[i].field_name = xdefs[i].field;
				m_rules[i].minVal = strtod(str, NULL);
				CString temp = str.Right(str.GetLength() - pos - 1);
				m_rules[i].maxVal = strtod(temp, NULL);
			}
		}
	}
	return m_fields_num;
}

int Field::GetRuleID(CString fieldname)
{
	for(int n=0; n<m_fields_num; n++)
	{
		if(fieldname == m_rules[n].field_name)
		{
			return n;
		}
	}
	return -1;
}

//检查值是否符合规则
//返回true表示符合规则或没有规则
bool Field::checkRule(CString field, CString valstr)
{
	int fieldID=GetRuleID(field);
	if(fieldID<0) return true;
	
	//没有规则，返回true
	if(m_rules[fieldID].RuleType<0) return true;
	
	//如果必选,字符串为空则返回false
	if(m_rules[fieldID].MustSel==1)
	{
		if(valstr==CString("NULL") || valstr.GetLength()<=0)
			return false;
	}
	
	int i=0;
	double dval = 0.0;
	int size = m_rules[fieldID].EVal.GetSize();
	switch(m_rules[fieldID].RuleType)
	{
	case 1:
		for(i=0; i<size; i++)
		{
			if(valstr==m_rules[fieldID].EVal.GetAt(i))
				return true;
		}
		return false;
		break;
	case 2:
		dval = strtod(valstr, NULL);
		if(dval<m_rules[fieldID].minVal || dval>m_rules[fieldID].maxVal)
			return false;
		else
			return true;
		break;
	case 0://无限定
		return true;
		break;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CEditExProp dialog

CEditExProp::CEditExProp(CWnd* pParent /*=NULL*/)
	: CDialog(CEditExProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckExProp)
		// NOTE: the ClassWizard will add member initialization here
	m_curPage = 0;
	m_SelColumn = -1;
	m_comboRow = -1;
	m_comboCol = -1;
	m_pDoc = NULL;
	m_pDS = NULL;
	m_pXDS = NULL;
	m_pScheme = NULL;
	//}}AFX_DATA_INIT
}

CEditExProp::~CEditExProp()
{
	clear();
}

void CEditExProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckExProp)
	DDX_Control(pDX, IDC_CUSTOM1, m_gridCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditExProp, CDialog)
	//{{AFX_MSG_MAP(CEditExProp)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(ID_EXPROP_COMBO, OnSelchangeCombo)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_CUSTOM1, OnGridEndEdit)
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM1, OnGridClick)
	ON_NOTIFY(GVN_SELCHANGING, IDC_CUSTOM1, OnGridStartSelChange)
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_MESSAGE(GRID_COMBOCHANGE, OnGridComboChange)
	ON_COMMAND(ID_EXPROP_FRESH, OnRefresh)
	ON_COMMAND(ID_EXPROP_CHECK_DB, OnCheckLayerDB)
	ON_UPDATE_COMMAND_UI(ID_EXPROP_CHECK_DB, OnUpdateCheckLayerDB)
	ON_COMMAND(ID_EXPROP_EXPORT, OnExport)
	ON_COMMAND(ID_EXPROP_PREVPAGE, OnPrevPage)
	ON_UPDATE_COMMAND_UI(ID_EXPROP_PREVPAGE, OnUpdatePrevPage1)
	ON_COMMAND(ID_EXPROP_NEXTPAGE, OnNextPage)
	ON_UPDATE_COMMAND_UI(ID_EXPROP_NEXTPAGE, OnUpdateNextPage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditExProp message handlers
BOOL CEditExProp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(StrFromResID(IDS_TITLE_EDIT_EXATTR));
	
	// TODO: Add extra initialization here
	CenterWindow();
	CRect rc;
	GetClientRect(&rc);
	m_gridCtrl.MoveWindow(7, 30, rc.Width()-14,rc.Height()-54, TRUE);
	
	m_gridCtrl.SetFixedRowCount(1);//一行标题栏
	m_gridCtrl.SetFixedColumnCount(1);//一列序号栏
	m_gridCtrl.SetColumnWidth(0, 45);
	m_gridCtrl.SetItemText(0, 0, StrFromResID(IDS_TITLE_INDEX));	
	m_gridCtrl.AutoSizeRows();//自动调整行高度
	m_gridCtrl.SetBkColor(RGB(255, 255, 255));
	m_gridCtrl.SetHeaderSort();//点击列表头排序
	
	m_bShowByEDB = AfxGetApp()->GetProfileInt(REGPATH_USER,_T("EditExporpGroup"),FALSE);

	m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP |
		CBRS_GRIPPER | CBRS_HIDE_INPLACE | CBRS_TOOLTIPS | CBRS_FLYBY, CRect(0, 0, 0, 0));
	m_wndToolBar.SetPaneStyle( m_wndToolBar.GetPaneStyle() &
		~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.LoadToolBar(IDR_CHECK_EXPROP, 0, 0, TRUE);
	m_wndToolBar.SetOwner(this);
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	m_wndToolBar.SetWindowText(StrFromResID(IDS_TITLE_EDIT_EXATTR));

	CUIFToolbarComboBoxButtonEx comboButton(ID_EXPROP_COMBO,
		GetCmdMgr()->GetCmdImage(ID_EXPROP_COMBO, FALSE),
		CBS_DROPDOWNLIST, 230);

	comboButton.RemoveAllItems();
	comboButton.SetDropDownHeight(300);
	comboButton.SetFlatMode();
	m_wndToolBar.ReplaceButton(ID_EXPROP_COMBO, comboButton);
	m_wndToolBar.AdjustLayout();
	m_wndToolBar.UpdateButton(2);
	m_wndToolBar.UpdateButton(4);
	m_wndToolBar.UpdateButton(5);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditExProp::OnDestroy() 
{
	CDialog::OnDestroy();
}

BOOL CEditExProp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_DELETE)
		{
			AfxGetMainWnd()->SendMessage(WM_COMMAND, (WPARAM)ID_MODIFY_DEL, 0);
			OnRefresh();
		}
		else if(pMsg->wParam == VK_F5)
			OnRefresh();
		else if(pMsg->wParam == VK_ESCAPE)
		{
			m_gridCtrl.SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return FALSE;
		}
	}
	if(pMsg->message == WM_MOUSEWHEEL)
	{
		CPoint ptCursor;
		::GetCursorPos(&ptCursor);
		ScreenToClient (&ptCursor);

		CRect rc;
		GetClientRect(&rc);

		if(!rc.PtInRect(ptCursor))
		{
			GetActiveView()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
	}
	BOOL bRet = CDialog::PreTranslateMessage(pMsg);

	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam<VK_LEFT || pMsg->wParam>VK_DOWN)
			return bRet;
		int row = m_gridCtrl.GetFocusCell().row;
		int col = m_gridCtrl.GetFocusCell().col;
		if(col>0)
		{
			m_SelectedRows.RemoveAll();
			for(int i=1; i<m_gridCtrl.GetRowCount(); i++)
			{
				int nState = m_gridCtrl.GetItemState(i, col);
				if(nState&GVIS_SELECTED)
				{
					m_SelectedRows.Add(i);
				}
			}
			m_pDoc->DeselectAll();
			for(int k=0; k<m_SelectedRows.GetSize(); k++)
			{
				int irow = m_SelectedRows.GetAt(k);
				BaseFtr *pItem = (BaseFtr*)m_gridCtrl.GetItemData(irow, 0);
				if(pItem)
					m_pDoc->SelectObj(FTR_HANDLE(pItem->pFtr), FALSE);
			}
			m_pDoc->OnSelectChanged(TRUE);

			if(row>0)//视图定位
			{
				PT_3DEX pt0;
				CArray<PT_3DEX,PT_3DEX> arrPts;
				BaseFtr *pItem = (BaseFtr*)m_gridCtrl.GetItemData(row, 0);
				if(pItem)
				{
					pItem->pFtr->GetGeometry()->GetShape(arrPts);
					pt0 = arrPts.GetAt(0);
					m_pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt0);
				}
			}
		}
	}

	return bRet;
}

BOOL CEditExProp::LoadRuleFile()
{
	if(!m_pDoc) return FALSE;
	m_pDS = m_pDoc->GetDlgDataSource();
	if (m_pDS == NULL) return FALSE;
	m_pXDS = m_pDS->GetXAttributesSource();
	if( !m_pXDS ) return FALSE;
	UINT scale = m_pDS->GetScale();//获取比例尺
	CConfigLibManager *pConfig = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
	m_pScheme = pConfig->GetScheme(scale);

	//获取规则
	if(!m_lf.Init(scale))
		return FALSE;

	//从注册表获取规则文件路径，然后获取规则
// 	CString RuleFilePath;
// 	unsigned char buf[256] = {0};
// 	HKEY hkey;
// 	unsigned long type;
// 	unsigned long len;
// 	CString ConfigPath;
// 	CString path("Software\\Visiontek\\FeatureOne4.1\\FeatureOne\\Config\\Options\\Symbol");
// 	CString symbol("SymbolPath");
// 	if(RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
// 	{
// 		if(RegQueryValueEx(hkey, symbol, NULL, &type, buf, &len) == ERROR_SUCCESS)
//         {
//             RuleFilePath.Format("%s\\%d\\FieldRule.txt", buf, scale);
//         }
// 		RegCloseKey(hkey);
// 	}
// 	
// 	if(!lf.open((LPSTR)(LPCTSTR)RuleFilePath))
// 	{
// 		return FALSE;
// 	}
	return TRUE;
}

//此函数只在打开dialog时执行一次，用于检查和统计层数量
void CEditExProp::Check()
{
	m_pDS = m_pDoc->GetDlgDataSource();
	if (m_pDS == NULL) return;

	m_pXDS = m_pDS->GetXAttributesSource();
	if( !m_pXDS ) return;

	CConfigLibManager *pConfig = GetConfigLibManager();
	if(!pConfig) return;
	m_pScheme =  pConfig->GetScheme(m_pDS->GetScale());
	if (!m_pScheme) return;

	int i=0, j=0, k=0;	
	int nFtrLay = m_pDS->GetFtrLayerCount();
	m_layerinfos.RemoveAll();
	for (i=0;i<nFtrLay;i++)//遍历层
	{
		CFtrArray ftrs;	
		CFtrLayer* pLayer = m_pDS->GetFtrLayerByIndex(i);
		
		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for(j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			ftrs.Add( pFtr );					
		}
		
		int num = ftrs.GetSize();//地物的数量
		if(num<1)  continue;

		LAYERINFO info;
		info.layName = pLayer->GetName();
		info.num = num;
		CSchemeLayerDefine *pLayerDefine = m_pScheme->GetLayerDefine(info.layName);
		if(pLayerDefine)
		{
			CString EDBName = pLayerDefine->GetDBLayerName();
			if(EDBName.IsEmpty())
				info.EDBName = info.layName;
			else
				info.EDBName = EDBName;
		}
		else
		{
			info.EDBName = info.layName;
		}
		
		m_layerinfos.Add(info);		
	}

	m_arrLayers.RemoveAll();
    int current_sel = 0;
	if(m_layerinfos.GetSize()>0)
	{
		if(m_bShowByEDB)
		{
			CStringArray names;
			for(j=0; j<m_layerinfos.GetSize(); j++)
			{
				for(k=0; k<names.GetSize(); k++)
				{
					if(m_layerinfos[j].EDBName == names[k])
						break;
				}
				if(k==names.GetSize())
				{
					names.Add(m_layerinfos[j].EDBName);
				}
			}
			
			for(k=0; k<names.GetSize(); k++)
			{
				int num = 0;
				for(j=0; j<m_layerinfos.GetSize(); j++)
				{
					if(m_layerinfos[j].EDBName == names[k])
					{
						num += m_layerinfos[j].num;
					}	
				}
				if(names[k] == m_curLayerName)
				{
					current_sel = k;
				}
				CString str;
				str.Format("%s(%d)", names[k], num);
				m_arrLayers.Add(str);
			}
			loadLayer(names[current_sel]);
		}
		else
		{
			for(j=0; j<m_layerinfos.GetSize(); j++)
			{
				if(m_layerinfos[j].layName == m_curLayerName)
				{
					current_sel = j;
				}
				CString str;
				str.Format("%s(%d)", m_layerinfos[j].layName, m_layerinfos[j].num);
				m_arrLayers.Add(str);
			}
			loadLayer(m_layerinfos[current_sel].layName);
		}
		
		UpdateLayerCombo(current_sel);
		showLayer();
	}
}

void CEditExProp::UpdateSelection()
{
	CWnd *pFocus = GetFocus();
	if(pFocus==&m_gridCtrl)
		return;

	m_gridCtrl.SetSelectedRange(-1,-1,-1,-1);
	CSelection *pSelect = m_pDoc->GetSelection();
	if(!pSelect) return;

	int num=0;
	const FTR_HANDLE *ftrs = pSelect->GetSelectedObjs(num);
	if(num<=0) return;

	int nRows = m_gridCtrl.GetRowCount();
	for(int i=0; i<m_ftrs.GetSize(); i++)
	{
		CFeature *pFtr = m_ftrs[i]->pFtr;
		if(FtrToHandle(pFtr)==ftrs[0])
		{
			int page = i/PAGE_SIZE;
			if(page!=m_curPage)
			{
				m_curPage = page;
				showLayer();
			}
			int irow = i%PAGE_SIZE;
			m_gridCtrl.EnsureVisible(irow+1, 1);
			int nState = m_gridCtrl.GetItemState(irow+1, 1);
			m_gridCtrl.SetItemState(irow+1, 1, nState|GVIS_SELECTED);
			break;
		}
	}
	m_gridCtrl.Refresh();
}

int CEditExProp::CheckVal(CValueTable &tab, BaseFtr *pItem, BOOL OnlyCheck)
{
	int numofErrors=0;//当前层错误的属性总数量

	int RowIndex = 0;//行序号
	if(!OnlyCheck)
	{
		RowIndex = m_gridCtrl.InsertRow("");
		CString indexstr;
		indexstr.Format("%d", pItem->index);
		m_gridCtrl.SetItemText(RowIndex, 0, indexstr);
		m_gridCtrl.SetItemData(RowIndex, 0, (LPARAM)pItem);
	}

	int nCol = m_lf.m_fields_num;//扩展属性的数量
	if(nCol<1)  return 0;

	const CVariantEx *var;
	for(int i=0; i<nCol; i++)//遍历ncol个属性
	{
		CString field = m_lf.m_rules[i].field_name;
		CString valAttr;
		if( tab.GetValue(0, field, var) )
		{					
			valAttr = VarToString((_variant_t)*var);
		}
		
		if(OnlyCheck)
		{
			if(!m_lf.checkRule(field, valAttr))  
			{
				numofErrors++;
			}
		}
		else
		{
			bool rval = true;//是否符合规则
			if( valAttr=="<NULL>" )
			{
				rval = true;
			}
			else
			{
				rval = m_lf.checkRule(field, valAttr);
				if(valAttr.IsEmpty())
				{
					valAttr = "<NULL>";
				}
			}

			m_gridCtrl.SetItemText(RowIndex, i+1, valAttr);
 
			if(rval)
			{
				m_gridCtrl.SetItemFgColour(RowIndex, i + 1, 0);
			}
			else
			{
				m_gridCtrl.SetItemFgColour(RowIndex, i + 1, 255);//错误显示红色字体
				numofErrors++;
			}
		}

	}

	return numofErrors;
}

void CEditExProp::clear()
{
	for(int i=0; i<m_ftrs.GetSize(); i++)
	{
		if(m_ftrs[i]->pTab)
		{
			delete m_ftrs[i]->pTab;
			m_ftrs[i]->pTab = NULL;
		}
		delete m_ftrs[i];
		m_ftrs[i] = NULL;
	}
	m_ftrs.RemoveAll();
}

BaseFtr* CEditExProp::AddFtr(CFeature *pFtr)
{
	if(!pFtr) return NULL;

	CGeometry *pGeo = pFtr->GetGeometry();
	if (!pGeo) return NULL;

	int index = m_ftrs.GetSize()+1;

	if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
	{
		BaseFtr *pItem = new BaseFtr();
		pItem->index = index;
		pItem->pFtr = pFtr;
		pItem->pTab = new CValueTable;
		pItem->pTab->BeginAddValueItem();
		m_pXDS->GetXAttributes(pFtr, *(pItem->pTab) );
		pItem->pTab->EndAddValueItem();
		m_ftrs.Add(pItem);
		return pItem;
	}
	else if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
	{
		TextFtr *pItem = new TextFtr();
		pItem->index = index;
		pItem->pFtr = pFtr;
		pItem->pTab = new CValueTable;
		pItem->pTab->BeginAddValueItem();
		m_pXDS->GetXAttributes(pFtr, *(pItem->pTab) );
		pItem->pTab->EndAddValueItem();
		pItem->text = ((CGeoText*)pGeo)->GetText();
		m_ftrs.Add(pItem);
		return pItem;
	}
	else if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		SurfaceFtr *pItem = new SurfaceFtr();
		pItem->index = index;
		pItem->pFtr = pFtr;
		pItem->pTab = new CValueTable;
		pItem->pTab->BeginAddValueItem();
		m_pXDS->GetXAttributes(pFtr, *(pItem->pTab) );
		pItem->pTab->EndAddValueItem();
		pItem->lenth = ((CGeoCurveBase*)pGeo)->GetLength();
		pItem->area = ((CGeoCurveBase*)pGeo)->GetArea();
		m_ftrs.Add(pItem);
		return pItem;
	}
	else if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		LineFtr *pItem = new LineFtr();
		pItem->index = index;
		pItem->pFtr = pFtr;
		pItem->pTab = new CValueTable;
		pItem->pTab->BeginAddValueItem();
		m_pXDS->GetXAttributes(pFtr, *(pItem->pTab) );
		pItem->pTab->EndAddValueItem();
		pItem->lenth = ((CGeoCurveBase*)pGeo)->GetLength();
		m_ftrs.Add(pItem);
		return pItem;
	}
	return NULL;
}

void CEditExProp::loadLayer(LPCTSTR layname)
{
	clear();
	int nSum=0;
	if(m_bShowByEDB)
	{
		for(int i=0; i<m_layerinfos.GetSize(); i++)
		{
			if(m_layerinfos[i].EDBName != layname)
				continue;
			
			CFtrLayer* pLayer = m_pDS->GetFtrLayer(m_layerinfos[i].layName);
			
			if(!pLayer||!pLayer->IsVisible())
				return;
			nSum += pLayer->GetObjectCount();
		}
	}
	else
	{
		CFtrLayer* pLayer = m_pDS->GetFtrLayer(layname);
		if(!pLayer||!pLayer->IsVisible())
			return;
		nSum = pLayer->GetObjectCount();
	}

	GProgressStart(nSum);
	if(m_bShowByEDB)
	{
		for(int i=0; i<m_layerinfos.GetSize(); i++)
		{
			if(m_layerinfos[i].EDBName != layname)
				continue;

			CFtrLayer* pLayer = m_pDS->GetFtrLayer(m_layerinfos[i].layName);
			if (!pLayer || !pLayer->IsVisible())
				continue;
			
			int nObj = pLayer->GetObjectCount();
			for(int j=0;j<nObj;j++)
			{
				GProgressStep();
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr||!pFtr->IsVisible())
					continue;
				AddFtr(pFtr);
			}
		}
	}
	else
	{
		CFtrLayer* pLayer = m_pDS->GetFtrLayer(layname);
		if(pLayer && pLayer->IsVisible())
		{
			int nObj = pLayer->GetObjectCount();
			for (int j = 0; j < nObj; j++)
			{
				GProgressStep();
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr || !pFtr->IsVisible())
					continue;
				AddFtr(pFtr);
			}
		}
	}
	GProgressEnd();
}

//显示某层，layerIndex是层的下标,同时是层下拉框的位置
void CEditExProp::showLayer()
{
	int num = m_ftrs.GetSize();//地物的数量
	if(num<1)
	{
		for(int i=m_gridCtrl.GetRowCount()-1; i>0; i--)
		{
			m_gridCtrl.DeleteRow(i);
		}
		m_gridCtrl.Refresh();
		return;
	}

	int maxPage = (num-1)/PAGE_SIZE;
	if(m_curPage>maxPage)
	{
		m_curPage = maxPage;
	}

	//上一页按钮
	m_wndToolBar.UpdateButton(4);
	//下一页按钮
	m_wndToolBar.UpdateButton(5);

	if(m_curPage<maxPage)
	{
		num = PAGE_SIZE;
	}
	else
	{
		num = num%PAGE_SIZE;
		if(num==0)
			num=PAGE_SIZE;
	}

	CString text = GetCurLayer();
	int pos = text.Find('(');
	text = text.Left(pos);

	m_lf.CleanUp();
	int size = m_lf.getFieldInfo(text, m_bShowByEDB);
	
	int type = m_ftrs[0]->type;
	if(type==TEXTFTR)
		m_gridCtrl.SetColumnCount(size+2);
	else
		m_gridCtrl.SetColumnCount(size+3);

	CRect rc;
	m_gridCtrl.GetClientRect(&rc);
	m_gridCtrl.SetRowCount(1);//相当于删除所有行（除了第一行）
	m_gridCtrl.SetItemText(0, 0, StrFromResID(IDS_TITLE_INDEX));

	//插入表头,insertcolumn函数奔溃。需要先设置列数，再设置改变单元格内容
	for(int i=1; i<size+1; i++)
	{
		m_gridCtrl.SetColumnWidth(i, 80);
		//CString field,name;
		//int type;
		//m_gridCtrl.SetItemText(0, i, arrfields[i-1]);
		m_gridCtrl.SetItemText(0, i, m_lf.m_rules[i-1].field_name);
	}

	if(type==TEXTFTR)
	{
		m_gridCtrl.SetItemText(0, size+1, StrFromResID(IDS_CMDPLANE_TEXT));
		m_gridCtrl.SetColumnWidth(size+1, 300);
	}
	else
	{
		m_gridCtrl.SetItemText(0, size+1, StrFromResID(IDS_SELCOND_LEN));
		m_gridCtrl.SetColumnWidth(size+1, 80);
		m_gridCtrl.SetItemText(0, size+2, StrFromResID(IDS_SELCOND_AREA));
		m_gridCtrl.SetColumnWidth(size+2, 80);
	}
	m_gridCtrl.ExpandLastColumn();
	m_gridCtrl.AutoSizeRows();//自动调整行高度
	m_gridCtrl.Refresh();


	GProgressStart(num);
	for(int k = 0; k < num; k++)//遍历该层的地物
	{
		GProgressStep();
		BaseFtr *pItem = m_ftrs.GetAt( k+m_curPage*PAGE_SIZE );

		CheckVal(*(pItem->pTab), pItem, FALSE);

		if(pItem->type==LINEFTR)
		{
			CString str;
			str.Format("%lf", ((LineFtr*)pItem)->lenth);
			int nState = m_gridCtrl.GetItemState(k+1, size+1);
			m_gridCtrl.SetItemText(k+1, size+1, str);
			m_gridCtrl.SetItemState(k+1, size+1, nState|GVIS_READONLY);
		}
		else if(pItem->type==SURFACEFTR)
		{
			CString str;
			str.Format("%lf", ((SurfaceFtr*)pItem)->lenth);
			int nState = m_gridCtrl.GetItemState(k+1, size+1);
			m_gridCtrl.SetItemText(k+1, size+1, str);
			m_gridCtrl.SetItemState(k+1, size+1, nState|GVIS_READONLY);

			str.Format("%lf", ((SurfaceFtr*)pItem)->area);
			nState = m_gridCtrl.GetItemState(k+1, size+1);
			m_gridCtrl.SetItemText(k+1, size+2, str);
			m_gridCtrl.SetItemState(k+1, size+2, nState|GVIS_READONLY);
		}
		else if(pItem->type==TEXTFTR)
		{
			int nState = m_gridCtrl.GetItemState(k+1, size+1);
			m_gridCtrl.SetItemText(k+1, size+1, ((TextFtr*)pItem)->text);
			m_gridCtrl.SetItemState(k+1, size+1, nState|GVIS_READONLY);
		}
	}
	m_gridCtrl.AutoSizeRows();
	GProgressEnd();
}


void CEditExProp::OnSelchangeCombo() 
{
	// TODO: Add your control notification handler code here
	CString text = GetCurLayer();
	int pos = text.Find('(');
	text = text.Left(pos);
	m_curLayerName = text;
	m_curPage = 0;
	loadLayer(text);
	showLayer();
}


//当单元格改变后
void CEditExProp::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	//1.获取行列
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	int row = pItem->iRow;
	int col = pItem->iColumn;
	if(row==0) return;
	if(col==0 || col==m_gridCtrl.GetColumnCount()-1)  return;
	CString val = m_gridCtrl.GetItemText(row, col);
	if (val == "<NULL>")
	{
		val = "";
	}
	_variant_t value = val;
	CString fieldname = m_gridCtrl.GetItemText(0, col);
	
	//2.检查正确性，不正确给予提示，但任然接受结果
	bool rval = m_lf.checkRule(fieldname, val);
	if(val.GetLength()<1)
	{
		val="<NULL>";
		m_gridCtrl.SetItemText(row, col, val);
	}
	CUndoModifyProperties undo(m_pDoc,"ModifyProperties");
	//3.更改属性到文件里
	if(m_SelColumn>0)
	{
		for(int i=0; i<m_SelectedRows.GetSize(); i++)
		{
			int irow = m_SelectedRows.GetAt(i);//行数
			m_gridCtrl.SetItemText(irow, col, val);
			if(rval) { m_gridCtrl.SetItemFgColour(irow, col, 0); }
			else     { m_gridCtrl.SetItemFgColour(irow, col, 255); }

			BaseFtr *pItem = (BaseFtr*)m_gridCtrl.GetItemData(irow, 0);
			if(!pItem) continue;

			CValueTable oldTab, newTab;
			oldTab.CopyFrom(*(pItem->pTab));
			
			if(pItem->pTab->SetValue(0, fieldname,&CVariantEx(value)))
			{
				m_pXDS->SetXAttributes(pItem->pFtr, *(pItem->pTab));

				newTab.CopyFrom(*(pItem->pTab));
				undo.SetModifyProperties(FTR_HANDLE(pItem->pFtr), oldTab, newTab, FALSE);
				m_pDoc->DeleteObject(FtrToHandle(pItem->pFtr), FALSE);
				m_pDoc->RestoreObject(FtrToHandle(pItem->pFtr));
			}
		}
		undo.Commit();
		SetSelected(col, FALSE);
		m_SelColumn = -1;
		//刷新表格显示
		m_gridCtrl.Refresh();
		m_pDS->SetModifiedFlag(true);
	}

	//当前单元格设定为非聚焦
	m_gridCtrl.SetFocusCell(-1, -1);
}

void CEditExProp::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	int row = pItem->iRow;
	int col = pItem->iColumn;

	if(m_comboRow>0)
	{
		m_gridCtrl.SetCellType(m_comboRow,m_comboCol, RUNTIME_CLASS(CGridCell));
		m_comboRow = -1;
		m_comboCol = -1;
		m_gridCtrl.Refresh();
	}

	//排序
	if(row==0)
	{
		CString field = m_gridCtrl.GetItemText(0, col);
		SortItems(field);
	}

	if(row==0 && col==0)  return;

	if(col==m_SelColumn)
	{
		BOOL Isin = FALSE;//此时点击的单元格是否在选中的之中
		for(int j=0; j<m_SelectedRows.GetSize(); j++)
		{
			if(m_SelectedRows.GetAt(j)==row)
			{
				Isin = TRUE;
			}
		}
		if(Isin)  SetSelected(col, TRUE);
	}
	else
	{
		m_SelColumn = col;
	}

	//保存选中的行号
	m_SelectedRows.RemoveAll();
	for(int i=1; i<m_gridCtrl.GetRowCount(); i++)
	{
		int state =  m_gridCtrl.GetItemState(i, col==0?1:col);
		if(state&GVIS_SELECTED)
			m_SelectedRows.Add(i);
	}

	//如果点击行或单元格，则选中地物
	m_pDoc->DeselectAll();
	for(int k=0; k<m_SelectedRows.GetSize(); k++)
	{
		int irow = m_SelectedRows.GetAt(k);
		BaseFtr *pItem = (BaseFtr*)m_gridCtrl.GetItemData(irow, 0);
		if(pItem)
			m_pDoc->SelectObj(FTR_HANDLE(pItem->pFtr), FALSE);
	}
	m_pDoc->OnSelectChanged(TRUE);

	if(row>0)//视图定位
	{
		PT_3DEX pt0;
		CArray<PT_3DEX,PT_3DEX> arrPts;
		BaseFtr *pItem = (BaseFtr*)m_gridCtrl.GetItemData(row, 0);
		if(pItem)
		{
			pItem->pFtr->GetGeometry()->GetShape(arrPts);
			pt0 = arrPts.GetAt(0);
			m_pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt0);
		}
	}

	if(row>0 && col>0)//如果是枚举值，设置下拉框
	{
		CString fieldname = m_gridCtrl.GetItemText(0, col);
		int ruleID = m_lf.GetRuleID(fieldname);
		if(ruleID<0) return;
		if(m_lf.m_rules[ruleID].RuleType==1)//枚举值
		{
			AddComboToCell(row, col);
			m_comboRow = row;
			m_comboCol = col;
			m_gridCtrl.Refresh();
		}
	}
}

// 指定单元格设置下拉框
void CEditExProp::AddComboToCell(int row, int col)
{
	if (!m_gridCtrl.SetCellType(row,col, RUNTIME_CLASS(CGridCellCombo)))
        return;
	m_gridCtrl.AutoSizeRow(row);
	
	CString str = m_gridCtrl.GetItemText(row, col);
    m_gridCtrl.SetItemText(row, col, str);

    CGridCellCombo *pCell = (CGridCellCombo*)m_gridCtrl.GetCell(row,col);
	CString fieldname = m_gridCtrl.GetItemText(0, col);
	int ruleID = m_lf.GetRuleID(fieldname);
    pCell->SetOptions(m_lf.m_rules[ruleID].EVal);
    pCell->SetStyle(CBS_DROPDOWN); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE
}

void CEditExProp::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_gridCtrl.MoveWindow(7, 30, cx-14, cy-54, TRUE);
	m_gridCtrl.SetColumnWidth(m_gridCtrl.GetColumnCount()-1, 1);
	m_gridCtrl.Refresh();
	m_gridCtrl.ExpandLastColumn();
}

LRESULT CEditExProp::OnGridComboChange(WPARAM wParam, LPARAM lParam)
{
	int rid =  m_lf.GetRuleID(m_gridCtrl.GetItemText(0, m_comboCol));
	CString val = m_lf.m_rules[rid].EVal.GetAt((int)lParam);
	_variant_t value = val;
	CString fieldname = m_gridCtrl.GetItemText(0, m_comboCol);
	bool rval = m_lf.checkRule(fieldname, val);
	CUndoModifyProperties undo(m_pDoc,"ModifyProperties");
	for(int i=0; i<m_SelectedRows.GetSize(); i++)
	{
		int irow = m_SelectedRows.GetAt(i);//行号
		m_gridCtrl.SetItemText(irow, m_comboCol, val);
		
		if(rval) { m_gridCtrl.SetItemFgColour(irow, m_comboCol, 0); }
		else     { m_gridCtrl.SetItemFgColour(irow, m_comboCol, 255); }

		BaseFtr *pItem = (BaseFtr*)m_gridCtrl.GetItemData(irow, 0);
		if(!pItem) continue;
		CValueTable oldTab, newTab;
		oldTab.CopyFrom(*(pItem->pTab));

		if(pItem->pTab->SetValue(0, fieldname,&CVariantEx(value)))
		{
			m_pXDS->SetXAttributes(pItem->pFtr, *(pItem->pTab));

			newTab.CopyFrom(*(pItem->pTab));
			undo.SetModifyProperties(FTR_HANDLE(pItem->pFtr), oldTab, newTab, FALSE);
			m_pDoc->DeleteObject(FtrToHandle(pItem->pFtr), FALSE);
			m_pDoc->RestoreObject(FtrToHandle(pItem->pFtr));
		}
	}
	undo.Commit();

	SetSelected(m_comboCol, FALSE);//取消选中状态

	m_pDS->SetModifiedFlag(true);

	//还原为普通单元格
	m_gridCtrl.SetCellType(m_comboRow,m_comboCol, RUNTIME_CLASS(CGridCell));
	m_gridCtrl.Refresh();
	m_gridCtrl.SetFocusCell(-1, -1);
	return 0;
}

void CEditExProp::OnGridStartSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	int row = pItem->iRow;
	int col = pItem->iColumn;

	if(m_ftrs.GetSize()<=0)
		return;

	if(m_ftrs.GetSize()>PAGE_SIZE)
	{
		m_gridCtrl.SetCompareFunction(NULL);//不排序
		return;
	}

	CString field = m_gridCtrl.GetItemText(0, col);
	int pos = m_ftrs[0]->pTab->FindField(field);
	
	if(pos>=0)
	{
		int type = VT_NULL;
		CString name, f;
		m_ftrs[0]->pTab->GetField(pos, f, type, name);
		if (type == VT_BSTR)
		{
			m_gridCtrl.SetCompareFunction(CGridCtrl::pfnCellTextCompare);//按文本排序	
		}
		else
		{
			m_gridCtrl.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);//按数字排序
		}
	}
	else if (0==field.Compare(StrFromResID(IDS_CMDPLANE_TEXT)))
	{
		m_gridCtrl.SetCompareFunction(CGridCtrl::pfnCellTextCompare);//按文本排序	
	}
	else
	{
		m_gridCtrl.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);//按数字排序
	}
}

void CEditExProp::OnPrevPage()
{
	m_curPage--;
	showLayer();
}

void CEditExProp::OnNextPage()
{
	m_curPage++;
	showLayer();
}

void CEditExProp::OnCheckLayerDB()
{
	m_bShowByEDB = !m_bShowByEDB;
	AfxGetApp()->WriteProfileInt(REGPATH_USER,_T("EditExporpGroup"), m_bShowByEDB);
	Check();
}

void CEditExProp::OnExport()
{
	CString path;
	CFileDialog dlg(FALSE,NULL,path,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("*.csv|*.csv|*.*|*.*|"),NULL);
	
	if( dlg.DoModal()!=IDOK )return;

	path = dlg.GetPathName();
	CString suffix = path.Right(4);
	if(0 != suffix.CompareNoCase(".csv"))
	{
		path+=".csv";
	}

	int num = m_ftrs.GetSize();//地物的数量
	if(num<1)  return;

	CString text = GetCurLayer();
	int pos = text.Find('(');
	text = text.Left(pos);

	CStringArray arrfields;
	
	if(m_pScheme && !m_bShowByEDB)
	{
		CSchemeLayerDefine *pLayDef = m_pScheme->GetLayerDefine(text);
		if(pLayDef)
		{
			int nDef;
			const XDefine *xdefs = pLayDef->GetXDefines(nDef);
			for(int n=0; n<nDef; n++)
			{
				arrfields.Add(xdefs[n].field);
			}
		}
	}
	else if (m_bShowByEDB)
	{
		CArray<const XDefine*, const XDefine*> xdefs;
		int nXDefCount = m_pScheme->GetDBLayerXdefines(text, xdefs);
		for (int n = 0; n < nXDefCount; n++)
		{
			arrfields.Add(xdefs[n]->field);
		}
	}

	FILE *fp = fopen(path, "w");
	if(!fp) return;

	int size = arrfields.GetSize();
	fprintf(fp, "%s,", StrFromResID(IDS_TITLE_INDEX));
	for(int i=0; i<size; i++)
	{
		//CString field,name;
		//int type;
		fprintf(fp, "%s,", arrfields[i]);
	}
	
	int type = m_ftrs[0]->type;
	if (type == TEXTFTR)
		fprintf(fp, "%s\n", StrFromResID(IDS_CMDPLANE_TEXT));
	else
		fprintf(fp, "%s,%s\n", StrFromResID(IDS_SELCOND_LEN), StrFromResID(IDS_SELCOND_AREA));
	
	GProgressStart(num);
	for(int k = 0; k < num; k++)//遍历改层的地物
	{
		GProgressStep();
		BaseFtr *pItem = m_ftrs[k];
		fprintf(fp, "%d,", pItem->index);

		for(int i=0; i<arrfields.GetSize(); i++)
		{
			CString field = arrfields[i];
			CString valAttr;
			const CVariantEx *var;
			if(pItem->pTab->GetValue(0, field, var))
			{
				valAttr = VarToString((_variant_t)*var);
			}
			fprintf(fp, "%s,", valAttr);
		}

		if(pItem->type==LINEFTR)
		{
			fprintf(fp, "%lf\n", ((LineFtr*)pItem)->lenth);
		}
		else if(pItem->type==SURFACEFTR)
		{
			fprintf(fp, "%lf,", ((SurfaceFtr*)pItem)->lenth);
			fprintf(fp, "%lf\n", ((SurfaceFtr*)pItem)->area);
		}
		else if(pItem->type==TEXTFTR)
		{
			fprintf(fp, "%s\n", ((TextFtr*)pItem)->text);
		}
		else
		{
			fprintf(fp, "\n");
		}
	}
	GProgressEnd();

	fclose(fp);
	fp=0;
	AfxMessageBox(IDS_IMPORTOK);
}

void CEditExProp::OnRefresh()
{
	CString text = GetCurLayer();
	int pos = text.Find('(');
	text = text.Left(pos);
	m_gridCtrl.SetRowCount(1);
	m_gridCtrl.SetColumnCount(1);
	Check();
	m_gridCtrl.Refresh();
}

template<class T>
int compare(T v1, T v2)
{
	if(v1>v2)
	{
		return 1;
	}
	else if(v1<v2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

static int compare_func(const void *p1, const void *p2)
{
	BaseFtr *t1 = *((BaseFtr**)p1);
	BaseFtr *t2 = *((BaseFtr**)p2);

	if (sortField == StrFromResID(IDS_TITLE_INDEX))
	{
		int index1 = t1->index;
		int index2 = t2->index;
		return compare(index1, index2);
	}
	else if (sortField == StrFromResID(IDS_SELCOND_LEN))
	{
		double len1 = -1;
		double len2 = -1;
		if (t1->type == LINEFTR)
			len1 = ((LineFtr*)t1)->lenth;
		else if (t1->type == SURFACEFTR)
			len1 = ((SurfaceFtr*)t1)->lenth;

		if (t2->type == LINEFTR)
			len2 = ((LineFtr*)t2)->lenth;
		else if (t2->type == SURFACEFTR)
			len2 = ((SurfaceFtr*)t2)->lenth;

		return compare(len1, len2);
	}
	else if (sortField == StrFromResID(IDS_SELCOND_AREA))
	{
		double v1 = -1;
		double v2 = -1;
		if (t1->type == SURFACEFTR)
			v1 = ((SurfaceFtr*)t1)->area;
		if (t2->type == SURFACEFTR)
			v2 = ((SurfaceFtr*)t2)->area;

		return compare(v1, v2);
	}
	else if (sortField == StrFromResID(IDS_CMDPLANE_TEXT))
	{
		CString s1, s2;
		if (t1->type == TEXTFTR)
			s1 = ((TextFtr*)t1)->text;
		if (t2->type == TEXTFTR)
			s2 = ((TextFtr*)t2)->text;

		return compare(s1, s2);
	}
	else
	{
		int pos = t1->pTab->FindField(sortField);
		if (pos < 0) return 0;

		int type = VT_NULL;
		CString name, fd;
		t1->pTab->GetField(pos, fd, type, name);

		const CVariantEx *var;
		if (!t1->pTab->GetValue(0, fd, var))
			return -1;
		CString str1 = VarToString((_variant_t)*var);
		if (!t2->pTab->GetValue(0, fd, var))
			return 1;
		CString str2 = VarToString((_variant_t)*var);

		if (type == VT_BSTR)
		{
			return compare(str1, str2);
		}
		else if (type == VT_R4 || type == VT_R8)
		{
			double v1 = strtod(str1, NULL);
			double v2 = strtod(str2, NULL);
			return compare(v1, v2);
		}
		else
		{
			int v1 = atoi(str1);
			int v2 = atoi(str2);
			return compare(v1, v2);
		}
	}

	return 0;
}

void CEditExProp::SortItems(CString field)
{
	if(m_ftrs.GetSize()<=PAGE_SIZE)
	{
		return;//只有1页，则使用控件的排序
	}

	sortField = field;
	qsort(m_ftrs.GetData(),m_ftrs.GetSize(),sizeof(BaseFtr*),compare_func);
	sortField = _T("");

	if(bAscending)
	{
		int size = m_ftrs.GetSize();
		for(int i=0; i<size/2; i++)
		{
			BaseFtr *ptemp = m_ftrs[i];
			m_ftrs[i] = m_ftrs[size-1-i];
			m_ftrs[size-1-i] = ptemp;
		}
	}

	bAscending = !bAscending;

	m_curPage = 0;
	showLayer();
}

void CEditExProp::OnUpdateCheckLayerDB(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowByEDB);
}

void CEditExProp::OnUpdatePrevPage1(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_curPage>0);
}

void CEditExProp::OnUpdateNextPage1(CCmdUI* pCmdUI)
{
	int maxPage = (m_ftrs.GetSize() - 1) / PAGE_SIZE;
	pCmdUI->Enable(m_curPage<maxPage);
}

void CEditExProp::UpdateLayerCombo(int sel)
{
	CMFCToolBarButton *pBtn = m_wndToolBar.GetButton(0);
	CMFCToolBarComboBoxButton *pCombo = DYNAMIC_DOWNCAST(CMFCToolBarComboBoxButton, pBtn);
	if (pCombo)
	{
		pCombo->RemoveAllItems();
		for (int i = 0; i < m_arrLayers.GetSize(); i++)
		{
			pCombo->AddItem(m_arrLayers[i]);
		}

		if (sel>=0 && sel< m_arrLayers.GetSize())
		{
			pCombo->SelectItem(sel, TRUE);
		}
	}
}

LPCTSTR CEditExProp::GetCurLayer()
{
	CMFCToolBarButton *pBtn = m_wndToolBar.GetButton(0);
	CMFCToolBarComboBoxButton *pCombo = DYNAMIC_DOWNCAST(CMFCToolBarComboBoxButton, pBtn);
	if (pCombo)
	{
		return pCombo->GetItem();
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CCheckExProp dialog

CCheckExProp::CCheckExProp(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CCheckExProp)
	// NOTE: the ClassWizard will add member initialization here
	m_curPage = 0;
	m_SelColumn = -1;
	m_comboRow = -1;
	m_comboCol = -1;
	m_pDoc = NULL;
	m_pDS = NULL;
	m_pXDS = NULL;
	//}}AFX_DATA_INIT
}

CCheckExProp::~CCheckExProp()
{
	clear();
}

BOOL CCheckExProp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(StrFromResID(IDS_TITLE_CHECK_EXATTR));
	
	// TODO: Add extra initialization here
	CenterWindow();
	CRect rc;
	GetClientRect(&rc);
	m_gridCtrl.MoveWindow(7, 30, rc.Width()-14,rc.Height()-54, TRUE);
	
	m_gridCtrl.SetFixedRowCount(1);//一行标题栏
	m_gridCtrl.SetFixedColumnCount(1);//一列序号栏
	m_gridCtrl.SetColumnWidth(0, 45);
	m_gridCtrl.SetItemText(0, 0, StrFromResID(IDS_TITLE_INDEX));	
	m_gridCtrl.AutoSizeRows();//自动调整行高度
	m_gridCtrl.SetBkColor(RGB(255, 255, 255));
	m_gridCtrl.SetHeaderSort();//点击列表头排序

	m_bShowByEDB = FALSE;

	m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP |
		CBRS_GRIPPER | CBRS_HIDE_INPLACE | CBRS_TOOLTIPS | CBRS_FLYBY, CRect(0, 0, 200, 20));
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() &
		~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.LoadToolBar(IDR_CHECK_EXPROP, 0, 0, TRUE);
	m_wndToolBar.SetOwner(this);
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	m_wndToolBar.SetWindowText(StrFromResID(IDS_TITLE_CHECK_EXATTR));

	CUIFToolbarComboBoxButtonEx comboButton(ID_EXPROP_COMBO,
		GetCmdMgr()->GetCmdImage(ID_EXPROP_COMBO, FALSE),
		CBS_DROPDOWNLIST, 230);

	comboButton.RemoveAllItems();
	comboButton.SetDropDownHeight(300);
	comboButton.SetFlatMode();
	m_wndToolBar.ReplaceButton(ID_EXPROP_COMBO, comboButton);
	m_wndToolBar.RemoveButton(2);
	m_wndToolBar.AdjustLayout();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//此函数只在打开dialog时执行一次，用于检查
void CCheckExProp::Check()
{
	m_pDS = m_pDoc->GetDlgDataSource();
	if (m_pDS == NULL) return;
	
	m_pXDS = m_pDS->GetXAttributesSource();
	if( !m_pXDS ) return;
	
	CConfigLibManager *pConfig = GetConfigLibManager();
	if(!pConfig) return;
	m_pScheme =  pConfig->GetScheme(m_pDS->GetScale());

	//挑出所有地物
	int i =0 , j =0;	
	int nFtrLay = m_pDS->GetFtrLayerCount();
	int nSum=0;
	for(i=0; i<nFtrLay; i++)
	{
		CFtrLayer* pLayer = m_pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())  continue;
		nSum += pLayer->GetObjectCount();
	}

	GProgressStart(nSum);

	CArray<int,int> ERRLayer;//有错误的层的序号
	m_arrLayers.RemoveAll();
	int current_sel = -1;

	for (i=0;i<nFtrLay;i++)//遍历层
	{
		CFtrArray ftrs;	
		CFtrLayer* pLayer = m_pDS->GetFtrLayerByIndex(i);

		if(!pLayer||!pLayer->IsVisible())
			continue;
		int nObj = pLayer->GetObjectCount();
		for(j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;
			ftrs.Add( pFtr );					
		}
		//读取并显示扩展属性
		int num = ftrs.GetSize();//地物的数量
		if(num<1)  continue;

		m_lf.CleanUp();
		if(m_lf.getFieldInfo(pLayer->GetName(),FALSE) < 1)
			continue;//若该层没有规则,则跳过

		int numOfErrors = 0;//计数：层里错误的地物数

		for(int k = 0; k < num; k++)//遍历某一个层的地物
		{
			GProgressStep();
			CFeature* pFtr = ftrs.GetAt( k );
			CValueTable xTab;
			xTab.BeginAddValueItem();
			m_pXDS->GetXAttributes(pFtr,xTab);
			xTab.EndAddValueItem();
			if(CheckVal(xTab, NULL, TRUE)>0)//只检查地物是否有错
			{
				numOfErrors++;
			}
		}

		if(numOfErrors>0)
		{
			ERRLayer.Add(i);
			CString str;
			str.Format("(%d)", numOfErrors);
			m_arrLayers.Add(pLayer->GetName() + str);
			if(m_curLayerName==pLayer->GetName())
			{
				current_sel = ERRLayer.GetSize()-1;
			}
		}
	}

	GProgressEnd();

	if(ERRLayer.GetSize()>0)
	{
		if(current_sel<0)
		{
			current_sel=0;
			CFtrLayer* pLayer = m_pDS->GetFtrLayerByIndex(ERRLayer[0]);
			if(pLayer)
			{
				m_curLayerName = pLayer->GetName();
			}
		}
		UpdateLayerCombo(current_sel);
		loadLayer(m_curLayerName);
		showLayer();
	}
	else
	{
		AfxMessageBox(IDS_NOANYERROR);
		EndDialog(TRUE);
	}
}

int CCheckExProp::CheckVal(CValueTable &tab, BaseFtr *pItem, BOOL OnlyCheck)
{
	int numofErrors = CEditExProp::CheckVal(tab, pItem, OnlyCheck);

	if(numofErrors<1)
	{
		m_gridCtrl.DeleteRow(m_gridCtrl.GetRowCount()-1);//删除最后插入的行
		m_gridCtrl.SetFixedRowCount(1);//一行标题栏
	}

	return numofErrors;
}

void CCheckExProp::loadLayer(LPCTSTR layname)
{
	CFtrLayer* pLayer = m_pDS->GetFtrLayer(layname);
	if(!pLayer||!pLayer->IsVisible())
		return;

	clear();

	m_lf.CleanUp();
	m_lf.getFieldInfo(layname,FALSE);

	int nObj = pLayer->GetObjectCount();
	for(int j=0;j<nObj;j++)
	{
		CFeature *pFtr = pLayer->GetObject(j);
		if (!pFtr||!pFtr->IsVisible())
			continue;

		CValueTable tab;
		tab.BeginAddValueItem();
		m_pXDS->GetXAttributes(pFtr,tab);
		tab.EndAddValueItem();

		if( CheckVal(tab, 0, TRUE)>0 )
		{
			AddFtr(pFtr);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgOutputLayerCheck dialog


CDlgOutputLayerCheck::CDlgOutputLayerCheck(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOutputLayerCheck::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOutputLayerCheck)
		// NOTE: the ClassWizard will add member initialization here
	m_pDoc = NULL;
	m_pDS = NULL;
	m_pXDS = NULL;
	//}}AFX_DATA_INIT
}


void CDlgOutputLayerCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckExProp)
	DDX_Control(pDX, IDC_COMBO_FIELDNAME, m_wndComboFields);
	DDX_Text(pDX, IDC_EDIT_LAYER, m_LayerName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOutputLayerCheck, CDialog)
	//{{AFX_MSG_MAP(CEditExProp)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT, OnButtonOutput)
	ON_BN_CLICKED(IDC_BUTTON_BROWER, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditExProp message handlers
BOOL CDlgOutputLayerCheck::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CenterWindow();
	m_LayerName = AfxGetApp()->GetProfileString("Config\\Options\\Save","CheckLayer", "");
	m_Field = AfxGetApp()->GetProfileString("Config\\Options\\Save","CheckField", "");
	SetDlgItemText(IDC_EDIT_LAYER, m_LayerName);

	if(!m_pDoc) return FALSE;
	m_pDS = m_pDoc->GetDlgDataSource();
	if (!m_pDS) return FALSE;
	InitLayer(m_LayerName);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOutputLayerCheck::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CDlgOutputLayerCheck::InitLayer(CString name)
{
	if(name.IsEmpty()) return;
	//获取配置
	CConfigLibManager *pConfig = GetConfigLibManager();
	CScheme *pScheme = pConfig->GetScheme(m_pDoc->GetDlgDataSource()->GetScale());
	if(!pScheme) return;
	
	CSchemeLayerDefine *pDef = pScheme->GetLayerDefine(name);
	if(!pDef) return;
	
	int size;
	const XDefine *defines = pDef->GetXDefines(size);
	m_wndComboFields.ResetContent();
	for( int i=0; i<size; i++)
	{
		m_wndComboFields.AddString(defines[i].field);
	}
	
	if( m_wndComboFields.SelectString(0, m_Field)<0 )
	{
		m_wndComboFields.SetCurSel(0);
	}
	UpdateData(FALSE);
}

#include "DlgSelectLayer.h"

void CDlgOutputLayerCheck::OnButtonBrowse()
{
	CDlgSelectFtrLayer dlg;
	dlg.m_pDS = m_pDS;	
	dlg.m_bUsed = TRUE;
	dlg.m_bLocal = TRUE;
	dlg.m_bNotLocal = FALSE;
	
	if( dlg.DoModal()!=IDOK )
		return;
	
	m_LayerName = dlg.m_SingleLayer;

	InitLayer(m_LayerName);
	UpdateData(FALSE);
}

void CDlgOutputLayerCheck::OnButtonOutput() 
{
	// TODO: Add your control notification handler code here
	CString layer, field;
	GetDlgItemText(IDC_EDIT_LAYER, layer);
	GetDlgItemText(IDC_COMBO_FIELDNAME, field);
	if(layer.GetLength()<1)
	{
		AfxMessageBox(IDS_LAYERNOEMPTY);
		return;
	}
	if(field.GetLength()<1)
	{
		AfxMessageBox(IDS_FIELDNOEMPTY);
		return;
	}

	m_pXDS = m_pDS->GetXAttributesSource();
	if( !m_pXDS ) return;
	CFtrArray ftrs;	
	CFtrLayer* pLayer = m_pDS->GetLocalFtrLayer(layer);
	if(!pLayer||!pLayer->IsVisible())
	{
		AfxMessageBox(IDS_IMPORTERR1);
		return;
	}

	if(pLayer->GetValidObjsCount()==0)
	{
		AfxMessageBox(IDS_IMPORTERR2);
		return;
	}
// 	{
// 		CFeature *pFtr = pLayer->GetObject(0);
// 		CValueTable tab;
// 		tab.BeginAddValueItem();
// 		m_pXDS->GetXAttributes(pFtr,tab);
// 		tab.EndAddValueItem();	
// 
// 		if( tab.FindField(field)<0 )
// 		{
// 			AfxMessageBox(IDS_IMPORTERR3);
// 			return;
// 		}
// 	}

	CString path;
	path = m_pDoc->GetPathName();
	path = path.Left(path.GetLength()-4);
	path = path + "-" + layer + "-" + field + ".csv";

	CFileDialog dlg(FALSE,NULL,path,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("*.csv|*.csv|*.*|*.*|"),NULL);
	
	if( dlg.DoModal()!=IDOK )return;

	path = dlg.GetPathName();
	CString suffix = path.Right(4);
	if(0 != suffix.CompareNoCase(".csv"))
	{
		path+=".csv";
	}

	AfxGetApp()->WriteProfileString("Config\\Options\\Save","CheckLayer",layer);
	AfxGetApp()->WriteProfileString("Config\\Options\\Save","CheckField",field);

	FILE *fp = fopen(path, "w");
	if(!fp) return;
	
	int count=1;
	int nObj = pLayer->GetObjectCount();
	for(int i=0; i<nObj; i++)
	{
		CFeature *pFtr = pLayer->GetObject(i);
		if (!pFtr||!pFtr->IsVisible()) continue;

		CValueTable tab;
		tab.BeginAddValueItem();
		m_pXDS->GetXAttributes(pFtr,tab);
		tab.EndAddValueItem();	

		const CVariantEx *var;
		if( !tab.GetValue(0, field, var) ) continue;				
		CString ValStr = VarToString((_variant_t)*var);

		CArray<PT_3DEX,PT_3DEX> arrPts;
		pFtr->GetGeometry()->GetShape(arrPts);
		if(arrPts.GetSize()>0)
		{
			fprintf(fp, "%d,\"(%lf,%lf)%s\"\n", count, arrPts[0].x, arrPts[0].y, (LPSTR)(LPCTSTR)ValStr);
			count++;
		}
	}
	fclose(fp);
	fp=0;
	AfxMessageBox(IDS_IMPORTOK);
	EndDialog(TRUE);
}

