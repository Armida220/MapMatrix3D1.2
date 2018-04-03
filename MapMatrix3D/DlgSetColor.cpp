// CDlgSetColor.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSetColor.h"
#include "EditBaseDoc.h"
#include "RegDef2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetColor dialog


CDlgSetColor::CDlgSetColor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetColor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetColor)
	m_pDS = NULL;
	m_pXDS = NULL;
	m_pDoc = NULL;
	//}}AFX_DATA_INIT
}


void CDlgSetColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetColor)
	DDX_Control(pDX, IDC_BUTTON3, m_button3);
	DDX_Control(pDX, IDC_COMBO1, m_combo);
	DDX_Control(pDX, IDC_BUTTON2, m_button2);
	DDX_Control(pDX, IDC_BUTTON1, m_button1);
	DDX_Control(pDX, IDC_CUSTOM1, m_gridCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetColor, CDialog)
	//{{AFX_MSG_MAP(CDlgSetColor)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_CUSTOM1, OnGridClick)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetColor message handlers
BOOL CDlgSetColor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CenterWindow();
	CRect rc;
	GetClientRect(&rc);
	m_gridCtrl.MoveWindow(7, 47, rc.Width()-14,rc.Height()-100, TRUE);
	m_button3.MoveWindow((rc.Width()-300)/4, rc.Height()-45, 100, 30);
	m_button1.MoveWindow((rc.Width()-100)/2, rc.Height()-45, 100, 30);
	m_button2.MoveWindow(rc.Width()*3/4-25, rc.Height()-45, 100, 30);
	
	m_gridCtrl.SetRowCount(1);
	m_gridCtrl.SetColumnCount(2);
	m_gridCtrl.SetColumnWidth(0, rc.Width()/2-6);
	m_gridCtrl.ExpandLastColumn();
	m_gridCtrl.SetItemText(0, 0, "Field");
	m_gridCtrl.SetItemText(0, 1, StrFromResID(IDS_COLOR));
	m_gridCtrl.AutoSizeRows();//自动调整行高度
	m_gridCtrl.SetBkColor(RGB(255, 255, 255));
	SetGridReadOnly(0, 0);
	SetGridReadOnly(0, 1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgSetColor::Init(CDlgDoc* pDoc)
{
	m_pDoc = pDoc;
	if(!m_pDoc) return;
	m_pDS = m_pDoc->GetDlgDataSource();
	if (m_pDS == NULL) return;

	m_pXDS = m_pDS->GetXAttributesSource();
	if( !m_pXDS ) return;

	int nSum=0;	
	int nFtrLay = m_pDS->GetFtrLayerCount();
	int i, j, k, n;

	for (i=0;i<nFtrLay;i++)//遍历层
	{
		CFtrLayer* pLayer = m_pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
		nSum += pLayer->GetObjectCount();
	}

	GProgressStart(nSum);
	for (i=0;i<nFtrLay;i++)//遍历层
	{
		CFtrLayer* pLayer = m_pDS->GetFtrLayerByIndex(i);
		if(!pLayer||!pLayer->IsVisible())
			continue;
			
		for(j=0;j<pLayer->GetObjectCount();j++)
		{
			GProgressStep();
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr||!pFtr->IsVisible())
				continue;

			CGeometry *pGeo = pFtr->GetGeometry();
			if(!pGeo || !pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				continue;

			m_ftrs.Add( pFtr );
			m_defaultColors.Add( pLayer->GetColor() );

			//统计面中使用的扩展属性
			CValueTable xTab;
			xTab.BeginAddValueItem();
			m_pXDS->GetXAttributes(pFtr,xTab);
			xTab.EndAddValueItem();
			for(k=0; k<xTab.GetFieldCount(); k++)
			{
				CString field, name;
				int type;
				xTab.GetField(k, field, type, name);

				for(n=0; n<m_arrFields.GetSize(); n++)
				{
					if(field == m_arrFields.GetAt(n))
						break;
				}
				if(n>=m_arrFields.GetSize())
					m_arrFields.Add(field);
			}
		}
	}
	GProgressEnd();

	if(m_arrFields.GetSize()<=0) return;

	for(i=0; i<m_arrFields.GetSize(); i++)
	{
		m_combo.AddString(m_arrFields.GetAt(i));
	}
	ShowField(0);
}

void CDlgSetColor::ShowField(int index)
{
	m_combo.SetCurSel(index);
	CString field = m_arrFields.GetAt(index);//当前显示的属性
	m_gridCtrl.SetItemText(0, 0, field);
	const CVariantEx *var;
	CString valAttr;
	m_gridCtrl.SetRowCount(1);
	m_arrVals.RemoveAll();
	m_arrColors.RemoveAll();

	GProgressStart(m_ftrs.GetSize());
	for(int i=0; i<m_ftrs.GetSize(); i++)
	{
		GProgressStep();
		CFeature* pFtr = m_ftrs.GetAt( i );
		CValueTable xTab;
		xTab.BeginAddValueItem();
		m_pXDS->GetXAttributes(pFtr,xTab);
		xTab.EndAddValueItem();
		if(xTab.GetValue( 0, field, var))
		{
			valAttr = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		}

		bool valExits = false;
		for(int j=0; j<m_arrVals.GetSize(); j++)
		{
			if(valAttr == m_arrVals.GetAt(j))
			{
				valExits = true;
				break;
			}
		}
		
		if(!valExits)
		{
			m_arrVals.Add(valAttr);
			COLORREF color;
			if(((CGeoCurveBase*)(pFtr->GetGeometry()))->IsFillColor())
			{
				//读取填充色
				color = ((CGeoCurveBase*)(pFtr->GetGeometry()))->GetFillColor();
			}
			else
			{
				color = m_defaultColors.GetAt(i);//读取默认地物颜色
			}
			
			m_arrColors.Add(color);
		}
	}
	GProgressEnd();

	int height = m_gridCtrl.GetRowHeight(0);
	for(int k=0; k<m_arrVals.GetSize(); k++)
	{
		CString valStr = m_arrVals.GetAt(k);
		int row = m_gridCtrl.InsertRow(valStr);
		m_gridCtrl.SetItemBkColour(row, 1, m_arrColors.GetAt(k));
		m_gridCtrl.SetRowHeight(row, height);
		SetGridReadOnly(row, 0);
		SetGridReadOnly(row, 1);
	}
	m_gridCtrl.Refresh();
}

//保存颜色
void CDlgSetColor::OnButton1() 
{
	// TODO: Add your control notification handler code here
	const CVariantEx *var;
	CString field = m_gridCtrl.GetItemText(0, 0);//获取当前扩展属性
	CUndoModifyProperties undo(m_pDoc,"ModifyProperties");

	int transparency = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,"Transparency", 50);

	GProgressStart(m_ftrs.GetSize());
	for(int i=0; i<m_ftrs.GetSize(); i++)
	{
		GProgressStep();
		CFeature* pFtr = m_ftrs.GetAt( i );
		CValueTable xTab;//扩展属性，属性
		xTab.BeginAddValueItem();
		m_pXDS->GetXAttributes(pFtr,xTab);
		xTab.EndAddValueItem();
		CString valAttr;
		if(xTab.GetValue( 0, field, var))
		{
			valAttr = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		}

		CGeoSurface *pGeo = (CGeoSurface*)pFtr->GetGeometry();
		for(int j=0; j<m_arrVals.GetSize(); j++)
		{
			if(valAttr == m_arrVals.GetAt(j))
			{
				undo.arrHandles.Add(FtrToHandle(pFtr));
				CValueTable tab;
				tab.BeginAddValueItem();
				pFtr->WriteTo(tab);
				tab.EndAddValueItem();
				tab.DelField(FIELDNAME_SHAPE);
				tab.DelField(FIELDNAME_GEOCOLOR);
				undo.oldVT.AddItemFromTab(tab);

				pGeo->EnableFillColor(TRUE, m_arrColors.GetAt(j));
				pGeo->SetTransparency(transparency);

				CString symName = pGeo->GetSymbolName();
				tab.SetValue(0,FIELDNAME_SYMBOLNAME,&CVariantEx((_variant_t)(const char*)symName));
				undo.newVT.AddItemFromTab(tab);

				break;
			}
		}
	}
	undo.Commit();
	GProgressEnd();
	m_pDoc->UpdateAllViews(NULL,hc_UpdateAllObjects);
	m_pDoc->DeselectAll();
	m_pDoc->OnSelectChanged();
	EndDialog(TRUE);
}

//取消
void CDlgSetColor::OnButton2() 
{
	// TODO: Add your control notification handler code here
	EndDialog(TRUE);
}

void CDlgSetColor::OnSelchangeCombo1() 
{
	// TODO: Add your control notification handler code here
	int sel = m_combo.GetCurSel();
	ShowField(sel);
}

void CDlgSetColor::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	int row = pItem->iRow;
	int col = pItem->iColumn;
	if(row <= 0) return;

	if(col==1)
	{
		long color;
		CColorDialog dlg(RGB(255, 255, 0), CC_FULLOPEN);
		if(dlg.DoModal() == IDOK)
		{
			color = dlg.GetColor();
			m_gridCtrl.SetItemBkColour(row, 1, color);
			m_arrColors.SetAt(row-1, color);
		}
	}

	m_pDoc->DeselectAll();//撤销所有选中	
	CString field = m_gridCtrl.GetItemText(0, 0);
	CString val = m_gridCtrl.GetItemText(row, 0);
	const CVariantEx *var;
	CString valAttr;
	for(int i=0; i<m_ftrs.GetSize(); i++)
	{
		CFeature* pFtr = m_ftrs.GetAt( i );
		CValueTable xTab;
		xTab.BeginAddValueItem();
		m_pXDS->GetXAttributes(pFtr,xTab);
		xTab.EndAddValueItem();
		if(xTab.GetValue( 0, field, var))
		{
			valAttr = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		}
		if(valAttr == val)
		{
			m_pDoc->SelectObj(FTR_HANDLE(m_ftrs.GetAt(i)), FALSE);
		}
	}
	m_pDoc->OnSelectChanged();
}

void CDlgSetColor::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_gridCtrl.MoveWindow(7, 47, cx-14, cy-100, TRUE);
	m_gridCtrl.SetColumnWidth(0, cx/2-6);
	m_gridCtrl.SetColumnWidth(1, 1);
	m_gridCtrl.Refresh();
	m_gridCtrl.ExpandLastColumn();
	m_button3.MoveWindow((cx-300)/4, cy-45, 100, 30);
	m_button1.MoveWindow((cx-100)/2, cy-45, 100, 30);
	m_button2.MoveWindow(cx*3/4-25, cy-45, 100, 30);
}

//随机颜色
void CDlgSetColor::OnButton3() 
{
	// TODO: Add your control notification handler code here
	m_arrColors.RemoveAll();
	for(int i=0; i<m_arrVals.GetSize(); i++)
	{
		int color, same=0;
		do{
			color = RGB(rand()%256, rand()%256, rand()%256);
			for(int j=0; j<m_arrColors.GetSize(); j++)
			{
				if(color == m_arrColors.GetAt(j))
				{
					same++;
					break;
				}
			}
		}while(color==0 || same>0);
		m_arrColors.Add(color);
		m_gridCtrl.SetItemBkColour(i+1, 1, color);
	}
	m_gridCtrl.Refresh();
}
