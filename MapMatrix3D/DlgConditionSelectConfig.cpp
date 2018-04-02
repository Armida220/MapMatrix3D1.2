// DlgConditionSelect.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgConditionSelectConfig.h"
#include "SmartViewFunctions.h"
#include "Linearizer.h"
#include "GeoCurve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CString strOpers[4];

extern void PenCodeToText(int pencode, char* text);
extern int TextToPenCode(const char*text);

CString FirstStrFromResID(UINT id)
{
	CString str;
	str.LoadString(id);
	int n = str.Find('\n');
	if( n>=0 && n<str.GetLength() )
		str.Delete(n,str.GetLength()-n);
	return str;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConditionSelectConfig dialog


CDlgConditionSelectConfig::CDlgConditionSelectConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConditionSelectConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConditionSelectConfig)
	m_strValue = _T("");
	//}}AFX_DATA_INIT
	m_nCurMenu = -1;
}


void CDlgConditionSelectConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConditionSelectConfig)
	DDX_Control(pDX, IDC_COMBO_MENUITEM, m_cMenu);
	DDX_Control(pDX, IDC_LIST_CONDITIONS, m_lstConds);
	DDX_Control(pDX, IDC_COMBO_OPERATOR, m_cOperator);
	DDX_Control(pDX, IDC_LIST_FIELD, m_cField);
	DDX_CBString(pDX, IDC_COMBO_FVALUE, m_strValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConditionSelectConfig, CDialog)
	//{{AFX_MSG_MAP(CDlgConditionSelectConfig)
	ON_BN_CLICKED(IDC_BUTTON_ADDCONDITION, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnUpdateColor)
	ON_BN_CLICKED(IDC_BUTTON_DELCONDITION, OnButtonDel)
	ON_LBN_SELCHANGE(IDC_LIST_FIELD, OnSelchangeField)
	ON_BN_CLICKED(IDC_BUTTON_MODIFYCONDITION, OnButtonModify)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_CONDITIONS, OnKillfocusListConditions)
	ON_BN_CLICKED(IDC_BUTTON_AND, OnButtonAnd)
	ON_BN_CLICKED(IDC_BUTTON_NOT, OnButtonNot)
	ON_BN_CLICKED(IDC_BUTTON_OR, OnButtonOr)
	ON_BN_CLICKED(IDC_BUTTON_LBRACKET, OnButtonLbracket)
	ON_BN_CLICKED(IDC_BUTTON_RBRACKET, OnButtonRbracket)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONDITIONS, OnClickListConditions)
	ON_BN_CLICKED(IDC_BUTTON_ADD_MENUITEM, OnButtonAddMenuitem)
	ON_BN_CLICKED(IDC_BUTTON_DEL_MENUITEM, OnButtonDelMenuitem)
	ON_CBN_SELCHANGE(IDC_COMBO_MENUITEM, OnSelchangeMenuitem)
	ON_CBN_EDITCHANGE(IDC_COMBO_MENUITEM, OnEditchangeMenuitem)
	ON_CBN_DROPDOWN(IDC_COMBO_MENUITEM, OnDropdownMenuitem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConditionSelectConfig message handlers

int CDlgConditionSelectConfig::GetConditonCurSel()
{	
	int index = -1;
	POSITION pos = m_lstConds.GetFirstSelectedItemPosition();
	if (pos)
	{
		index = m_lstConds.GetNextSelectedItem(pos);
	}
	
	

	return index;

}

void CDlgConditionSelectConfig::OnButtonAdd() 
{
	UpdateData(TRUE);

	int count = m_lstConds.GetItemCount();
	m_lstConds.InsertItem (count,NULL);
	
	if (count > 0)
	{
		m_lstConds.SetItemText(count,0,_T("And"));
	}

	CString strField;
	int nsel = m_cField.GetCurSel();
	if( nsel>=0 )
	{
		m_cField.GetText(nsel,strField);
		m_lstConds.SetItemText(count,1,strField);
	}

	int nOp = m_cOperator.GetCurSel();
	m_lstConds.SetItemText(count,2,strOpers[nOp]);
	
	m_lstConds.SetItemText(count,3,m_strValue);

	//记录条件
	CONDSEL item;
	memset(&item,0,sizeof(item));

	if( strField.GetLength()<sizeof(item.field)-1 )
		strcpy(item.field,strField);
	else 
		memcpy(item.field,(LPCTSTR)strField,sizeof(item.field)-2);

//	item.fidx = m_cField.GetCurSel();
	item.op = m_cOperator.GetCurSel();

	CString str = m_strValue;//ConvertStrValue();

	if( str.GetLength()<sizeof(item.value)-1 )
		strcpy(item.value,str);
	else 
		memcpy(item.value,(LPCTSTR)str,sizeof(item.value)-2);
	

	m_arrConds.Add(item);
}

void CDlgConditionSelectConfig::OnUpdateColor()
{
	COLORREF color = m_Color.GetColor();
	m_strValue.Format("%d",color);
	UpdateData(FALSE);
}
void CDlgConditionSelectConfig::OnButtonDel() 
{	
	int nItem = -1;
	POSITION pos = m_lstConds.GetFirstSelectedItemPosition();
	if (pos)
	{
		nItem = m_lstConds.GetNextSelectedItem(pos);
	}

	if	(nItem < 0)
	{
		CString strText;
		strText.LoadString(IDS_SEL_ONLYONE);
		AfxMessageBox(strText);
		return;
	}
	else
	{
		m_lstConds.DeleteItem(nItem);
		m_arrConds.RemoveAt(nItem);
	}

	if (m_lstConds.GetItemCount()>0 && nItem==0)
	{
		m_lstConds.SetItemText(0,0,"");
	}
	
	
}

BOOL CDlgConditionSelectConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_lstConds.SetExtendedStyle(m_lstConds.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	
	//属性字段
	int mwid = 10;
	int iCount = m_arrFields.GetSize();
	for (int i=0; i<iCount; i++)
	{
		m_cField.AddString(m_arrFields[i]);

		if( mwid<m_arrFields[i].GetLength() )
			mwid = m_arrFields[i].GetLength();
	}
	m_cField.SetCurSel(0);

	CDC *pDC = m_cField.GetDC();
	if( pDC )
	{
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		m_cField.ReleaseDC(pDC);

		mwid = tm.tmAveCharWidth*mwid + 20;
		if( mwid>m_cField.GetHorizontalExtent() )
			m_cField.SetHorizontalExtent(mwid);
	}

	//运算符
	m_cOperator.AddString(StrFromResID(IDS_COND_EQUAL));
	m_cOperator.AddString(StrFromResID(IDS_COND_NOTEQUAL));
	m_cOperator.AddString(StrFromResID(IDS_COND_GREATER));
	m_cOperator.AddString(StrFromResID(IDS_COND_LESS));
	m_cOperator.SetCurSel(0);


	//条件列表
	int nw = 8;
	pDC = m_lstConds.GetDC();
	if( pDC )
	{
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		nw = tm.tmAveCharWidth;
		m_lstConds.ReleaseDC(pDC);
	}
	m_lstConds.InsertColumn(0,StrFromResID(IDS_OPERATIONS),LVCFMT_LEFT,nw*4+15);
//	m_lstConds.InsertColumn(1,StrFromResID(IDS_COND_COL_OBJ),LVCFMT_LEFT,nw*8+20);
	m_lstConds.InsertColumn(1,StrFromResID(IDS_COND_COL_FIELD),LVCFMT_LEFT,nw*8+10);
	m_lstConds.InsertColumn(2,StrFromResID(IDS_COND_COL_OP),LVCFMT_LEFT,nw*4+20);
	m_lstConds.InsertColumn(3,StrFromResID(IDS_COND_COL_VALUE),LVCFMT_LEFT,nw*4+30);
	for (i=0; i<7; i++)
	{
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_MODIFYCONDITION+i);
		if (pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
	}

	//菜单列表
	if( m_cQueryMenu.m_arrMenus.GetSize()>0 )
	{
		for( i=0; i<m_cQueryMenu.m_arrMenus.GetSize(); i++)
		{
			CondMenu *p = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(i);
			if( p )m_cMenu.AddString(p->name);
		}
		m_nCurMenu = 0;
		LoadCurMenu();
	}

	OnSelchangeField();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgConditionSelectConfig::OnOK() 
{
	

	SaveCurMenu();
	CDialog::OnOK();
}



void CDlgConditionSelectConfig::OnSelchangeField() 
{
	UpdateData(TRUE);

	CString str;
	int nsel = m_cField.GetCurSel();
	if( nsel<0 )
	{
		CWnd *pBtn = GetDlgItem(IDC_BUTTON_ADD);
		if( pBtn )pBtn->EnableWindow(FALSE);
	}
	else
	{
		CWnd *pBtn = GetDlgItem(IDC_BUTTON_ADD);
		if( pBtn )pBtn->EnableWindow(TRUE);
		
		m_cField.GetText(nsel,str);
	}

	CComboBox *pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_FVALUE);
	if( !pCtrl )return;

	if( ::IsWindow(m_Color.m_hWnd) )m_Color.ShowWindow(SW_HIDE);
	pCtrl->ShowWindow(SW_SHOW);
	pCtrl->SetWindowText(_T(""));
	pCtrl->ResetContent();
	
	if	(str.CompareNoCase(StrFromResID(IDS_FIELDNAME_GEOCLASS)) == 0)
	{
		pCtrl->AddString(StrFromResID(IDS_ALLTYPES));
		pCtrl->AddString(FirstStrFromResID(ID_ELEMENT_DOT_DOT));
		pCtrl->AddString(FirstStrFromResID(ID_ELEMENT_DOT_VECTORDOT));
		pCtrl->AddString(FirstStrFromResID(ID_ELEMENT_LINE_LINE));
		pCtrl->AddString(FirstStrFromResID(ID_ELEMENT_LINE_DLINE));
		pCtrl->AddString(FirstStrFromResID(ID_ELEMENT_LINE_PARALLEL));
		pCtrl->AddString(FirstStrFromResID(ID_ELEMENT_FACE_FACE));
		pCtrl->AddString(FirstStrFromResID(ID_ELEMENT_TEXT));
		pCtrl->SetCurSel(0);

		if (m_cOperator.GetCount() == 4)
		{
			m_cOperator.DeleteString(3);
			m_cOperator.DeleteString(2);
			m_cOperator.SetCurSel(0);
		}
	}
	else if( str.CompareNoCase(StrFromResID(IDS_FIELDNAME_LAYERNAME))==0 )
	{		
		if (m_cOperator.GetCount() == 4)
		{
			m_cOperator.DeleteString(3);
			m_cOperator.DeleteString(2);
			m_cOperator.SetCurSel(0);
		}
	}
	else if( str.CompareNoCase(StrFromResID(IDS_FIELDNAME_MAPNAME))==0 )
	{		
		if (m_cOperator.GetCount() == 4)
		{
			m_cOperator.DeleteString(3);
			m_cOperator.DeleteString(2);
			m_cOperator.SetCurSel(0);
		}
	}
	else if( str.CompareNoCase(StrFromResID(IDS_SELCOND_LINETYPE))==0 )
	{
 		pCtrl->AddString(StrFromResID(IDS_LINETYPE_LINE));
 		pCtrl->AddString(StrFromResID(IDS_LINETYPE_CURVE));
 		pCtrl->AddString(StrFromResID(IDS_LINETYPE_ARC));
 		pCtrl->AddString(StrFromResID(IDS_LINETYPE_SYNCH));
		pCtrl->SetCurSel(0);

		if (m_cOperator.GetCount() == 4)
		{
			m_cOperator.DeleteString(3);
			m_cOperator.DeleteString(2);
			m_cOperator.SetCurSel(0);
		}
	}
	else if( str.CompareNoCase(StrFromResID(IDS_SELCOND_LINEDIR))==0 )
	{
		pCtrl->AddString(StrFromResID(IDS_COND_CLOCKWISE));
		pCtrl->AddString(StrFromResID(IDS_COND_ANTICLOCKWISE));
		pCtrl->SetCurSel(0);

		if (m_cOperator.GetCount() == 4)
		{
			m_cOperator.DeleteString(3);
			m_cOperator.DeleteString(2);
			m_cOperator.SetCurSel(0);
		}
	}
	else if( str.CompareNoCase(StrFromResID(IDS_SELCOND_INTERSECTSELF))==0 ||
		str.CompareNoCase(StrFromResID(IDS_SELCOND_SUSPENDPOINT))==0 ||
		str.CompareNoCase(StrFromResID(IDS_SELCOND_SAMEPOINT))==0 ||
		str.CompareNoCase(StrFromResID(IDS_SELCOND_SAMELINE))==0 ||
		str.CompareNoCase(StrFromResID(IDS_SELCOND_LLINTERSECT))==0||
		str.CompareNoCase(StrFromResID(IDS_SELCOND_STREAMREVERSE))==0 ||
		str.CompareNoCase(StrFromResID(IDS_FIELDNAME_CURVE_CLOSED))==0)
	{
		pCtrl->AddString(StrFromResID(IDS_YES));
		pCtrl->AddString(StrFromResID(IDS_NO));
		pCtrl->SetCurSel(0);

		if (m_cOperator.GetCount() == 4)
		{
			m_cOperator.DeleteString(3);
			m_cOperator.DeleteString(2);
			m_cOperator.SetCurSel(0);
		}
	}
	else if( str.CompareNoCase(StrFromResID(IDS_COLOR))==0 )
	{
		pCtrl->ShowWindow(SW_HIDE);

		CRect rect;
		pCtrl->GetWindowRect(&rect);

		ScreenToClient(&rect);
		if( !::IsWindow(m_Color.m_hWnd) )
		{
			m_Color.Create(NULL,WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,rect,this,IDC_BUTTON_COLOR);
			m_Color.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
			m_Color.ShowWindow(SW_SHOW);
		}
		else
			m_Color.ShowWindow(SW_SHOW);

		m_strValue = "0";

		if (m_cOperator.GetCount() == 4)
		{
			m_cOperator.DeleteString(3);
			m_cOperator.DeleteString(2);
			m_cOperator.SetCurSel(0);
		}

		UpdateData(FALSE);
	}
	else
	{

		if (m_cOperator.GetCount() == 2)
		{
			m_cOperator.AddString(StrFromResID(IDS_COND_GREATER));
			m_cOperator.AddString(StrFromResID(IDS_COND_LESS));
			m_cOperator.SetCurSel(0);
		}

		m_strValue = "0";
		UpdateData(FALSE);
	}


}
CString CDlgConditionSelectConfig::ConvertStrValue()
{
	CString str, ret;
	int nsel = m_cField.GetCurSel();
	if( nsel>=0 )m_cField.GetText(nsel,str);
	
	CComboBox *pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_FVALUE);
	if( !pCtrl )return ret;
	
	ret = m_strValue;
	if	(str.CompareNoCase(StrFromResID(IDS_FIELDNAME_GEOCLASS)) == 0)
	{
		if (m_strValue.CompareNoCase(FirstStrFromResID(ID_ELEMENT_DOT_DOT)) == 0)
		{
			ret.Format("%d",CLS_GEOPOINT);
		}
		else if (m_strValue.CompareNoCase(FirstStrFromResID(ID_ELEMENT_DOT_VECTORDOT)) == 0)
		{
			ret.Format("%d",CLS_GEODIRPOINT);
		}
		else if (m_strValue.CompareNoCase(FirstStrFromResID(ID_ELEMENT_LINE_LINE)) == 0)
		{
			ret.Format("%d",CLS_GEOCURVE);	
		}
		else if (m_strValue.CompareNoCase(FirstStrFromResID(ID_ELEMENT_LINE_DLINE)) == 0)
		{
			ret.Format("%d",CLS_GEODCURVE);	
		}
		else if (m_strValue.CompareNoCase(FirstStrFromResID(ID_ELEMENT_LINE_PARALLEL)) == 0)
		{
			ret.Format("%d",CLS_GEOPARALLEL);	
		}
		else if (m_strValue.CompareNoCase(FirstStrFromResID(ID_ELEMENT_FACE_FACE)) == 0)
		{
			ret.Format("%d",CLS_GEOSURFACE);
		}
		else if (m_strValue.CompareNoCase(FirstStrFromResID(ID_ELEMENT_TEXT)) == 0)
		{
			ret.Format("%d",CLS_GEOTEXT);
		}
		
	}
	else if( str.CompareNoCase(StrFromResID(IDS_SELCOND_LINETYPE))==0 )
	{
		int pencode = TextToPenCode( m_strValue );
		if( pencode!=0 )ret.Format("%d",pencode);
	}
	else if( str.CompareNoCase(StrFromResID(IDS_SELCOND_LINEDIR))==0 )
	{
		if( m_strValue.CompareNoCase(StrFromResID(IDS_COND_CLOCKWISE))==0 )
			ret = _T("1");
		else if( m_strValue.CompareNoCase(StrFromResID(IDS_COND_ANTICLOCKWISE))==0 )
			ret = _T("0");
	}
	else if( str.CompareNoCase(StrFromResID(IDS_SELCOND_INTERSECTSELF))==0 ||
		str.CompareNoCase(StrFromResID(IDS_SELCOND_SUSPENDPOINT))==0 ||
		str.CompareNoCase(StrFromResID(IDS_SELCOND_SAMEPOINT))==0 ||
		str.CompareNoCase(StrFromResID(IDS_SELCOND_SAMELINE))==0 ||
		str.CompareNoCase(StrFromResID(IDS_SELCOND_LLINTERSECT))==0||
		str.CompareNoCase(StrFromResID(IDS_SELCOND_STREAMREVERSE))==0 ||
		str.CompareNoCase(StrFromResID(IDS_FIELDNAME_CURVE_CLOSED))==0)
	{
		if( m_strValue.CompareNoCase(StrFromResID(IDS_YES))==0 )
			ret = _T("1");
		else if( m_strValue.CompareNoCase(StrFromResID(IDS_NO))==0 )
			ret = _T("0");
	}
	else if( str.CompareNoCase(StrFromResID(IDS_COLOR))==0 )
	{
		ret.Format("%d",m_Color.GetColor());
	}

	return ret;
}

BOOL CDlgConditionSelectConfig::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN )
	{
		CComboBox *pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_FVALUE);
		if( pCtrl )
		{
			if( ::IsChild(pCtrl->GetSafeHwnd(),::GetFocus()) ||
				::GetFocus()==pCtrl->GetSafeHwnd() )
			{
				OnButtonAdd();
				return TRUE;
			}
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


void CDlgConditionSelectConfig::OnButtonAddMenuitem() 
{
	SaveCurMenu();

	CondMenu *pMenu = new CondMenu;
	if( !pMenu )return;
	m_nCurMenu = m_cQueryMenu.m_arrMenus.Add(pMenu);

	pMenu->name.Format("Menu %d",m_cQueryMenu.m_arrMenus.GetSize());
	m_cMenu.AddString(pMenu->name);
	LoadCurMenu();
	m_cMenu.SetFocus();


	CWnd *pWnd = GetDlgItem(IDC_BUTTON_DEL_MENUITEM);
	if (pWnd && !pWnd->IsWindowEnabled())
	{
		pWnd->EnableWindow(TRUE);
	}
	
}

void CDlgConditionSelectConfig::OnButtonDelMenuitem() 
{
	if( m_nCurMenu<0 )return;
	CondMenu *pMenu = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(m_nCurMenu);
	if( !pMenu )return;
	m_cMenu.DeleteString(m_nCurMenu);

	delete pMenu;
	m_cQueryMenu.m_arrMenus.RemoveAt(m_nCurMenu);
	if( m_nCurMenu>=m_cQueryMenu.m_arrMenus.GetSize() )
		m_nCurMenu--;

	LoadCurMenu();
	//	if( m_nCurMenu>=0 )
	m_cMenu.SetCurSel(m_nCurMenu);
	
	if (m_nCurMenu < 0)
	{
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_DEL_MENUITEM);
		if (pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
	}
	
}


void CDlgConditionSelectConfig::SaveCurMenu()
{
// 	m_nApply = m_cApply.GetCurSel();
// 	m_nCondType = m_cCondType.GetCurSel();
// 	
// 	for( int i=0; i<m_arrConds.GetSize(); i++)
// 	{
// 		CONDSEL item = m_arrConds.GetAt(i);
// 		item.condtype = m_nCondType;
// 		m_arrConds.SetAt(i,item);
//	}
	
	for (int i=0;i<m_lstConds.GetItemCount();i++)
	{
		//记录条件
		CONDSEL &item = m_arrConds.ElementAt(i);

		CString strText = m_lstConds.GetItemText(i,0); 
		if (strText.CompareNoCase("And")==0 || strText.IsEmpty())
		{
			//			m_strExpression += '&';
			item.condtype[0] = TRUE;
		}
		else if (strText.CompareNoCase("Or") == 0)
		{
			//			m_strExpression += '|';	
			item.condtype[0] = FALSE;
		}
		
		strText = m_lstConds.GetItemText(i,1);
		int nIndex = strText.Find('(');
		if (nIndex >= 0)
		{
			strText.Delete(nIndex);			
			//			m_strExpression += '(';
			
			item.condtype[1] = TRUE;
		}
		else
			item.condtype[1] = FALSE;

		if( strText.GetLength()<sizeof(item.field)-1 )
			strcpy(item.field,strText);
		else 
			memcpy(item.field,(LPCTSTR)strText,sizeof(item.field)-2);

		for (int j=0; j<m_arrFields.GetSize(); j++)
		{
			if (m_arrFields.GetAt(j).CompareNoCase(item.field) == 0)
			{
//				item.fidx = j;
			}
		}
		
		strText = m_lstConds.GetItemText(i,2);
		for (j=0; j<4; j++)
		{
			if (strText.CompareNoCase(strOpers[j]) == 0)
			{
				item.op = j;
			}
		}

		strText = m_lstConds.GetItemText(i,3);
		nIndex = strText.Find(')');
		if (nIndex >= 0)
		{
			strText.Delete(nIndex);			
			//			m_strExpression += ')';
			item.condtype[2] = TRUE;
		}
		else
			item.condtype[2] = FALSE;

		if( strText.GetLength()<sizeof(item.value)-1 )
			strcpy(item.value,strText);
		else 
			memcpy(item.value,(LPCTSTR)strText,sizeof(item.value)-2);
		
	}

	if( m_nCurMenu<0 )return;
	CondMenu *pMenu = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(m_nCurMenu);
	if( !pMenu )return;
	pMenu->arrConds.Copy(m_arrConds);

//	m_cQueryMenu.Save();
}

void CDlgConditionSelectConfig::LoadCurMenu()
{
	if( m_nCurMenu<0 )
	{
		m_lstConds.DeleteAllItems();
		return;
	}
	CondMenu *pMenu = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(m_nCurMenu);
	if( !pMenu )return;
	m_arrConds.Copy(pMenu->arrConds);

	m_cMenu.SelectString(-1,pMenu->name);

	m_lstConds.DeleteAllItems();
//	if( m_arrConds.GetSize()>0 )
	{
//		CONDSEL cs = m_arrConds.GetAt(0);
//		m_cCondType.SetCurSel(cs.condtype);

//		CString str;
		for( int i=0; i<m_arrConds.GetSize(); i++)
		{
			m_lstConds.InsertItem (i,NULL);

			CONDSEL cs = m_arrConds.GetAt(i);
			
			if (i > 0)
				m_lstConds.SetItemText(i,0,cs.condtype[0]?"And":"Or");
			
			
			CString str = cs.field;
			if (cs.condtype[1])
				str.Insert(0,"(");
			m_lstConds.SetItemText(i,1,str);
			
//			m_cOperator.GetLBText(cs.op,str);
			str = strOpers[cs.op];
			m_lstConds.SetItemText(i,2,str);
			
			str = cs.value;
			if (cs.condtype[2])
				str.Insert(str.GetLength(),")");
			m_lstConds.SetItemText(i,3,str);
		}
	}
}

void CDlgConditionSelectConfig::OnSelchangeMenuitem() 
{
	SaveCurMenu();

	m_arrConds.RemoveAll();

	int nsel = m_cMenu.GetCurSel();
	if( m_nCurMenu>=0 )
	{
		CondMenu *pMenu = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(m_nCurMenu);
		if( pMenu )
		{
			CString str = pMenu->name;
			
			m_cMenu.DeleteString(m_nCurMenu);
			m_cMenu.InsertString(m_nCurMenu,str);
		}
	}

	m_nCurMenu = nsel;
	LoadCurMenu();
}

void CDlgConditionSelectConfig::OnEditchangeMenuitem() 
{
	if( m_nCurMenu<0 )return;
	CondMenu *pMenu = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(m_nCurMenu);
	if( !pMenu )return;

	m_cMenu.GetWindowText(pMenu->name);
}

void CDlgConditionSelectConfig::OnDropdownMenuitem() 
{
	if( m_nCurMenu<0 )return;
	CondMenu *pMenu = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(m_nCurMenu);
	if( !pMenu )return;

	CString str = pMenu->name;

	m_cMenu.DeleteString(m_nCurMenu);
	m_cMenu.InsertString(m_nCurMenu,str);
	m_cMenu.SelectString(-1,str);
}
void CDlgConditionSelectConfig::OnButtonModify() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nItem = -1;
	POSITION pos = m_lstConds.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		AfxMessageBox("No items were selected!\n");
		return;
	}
	else
	{
		nItem = m_lstConds.GetNextSelectedItem(pos);
	}

	CString strText;
	int nsel = m_cField.GetCurSel();
	if (nsel >= 0)
	{
		m_cField.GetText(nsel,strText);
		m_lstConds.SetItemText(nItem,1,strText);
	}

	int nOp = m_cOperator.GetCurSel();
	m_lstConds.SetItemText(nItem,2,strOpers[nOp]);
	
	m_lstConds.SetItemText(nItem,3,m_strValue);

}

void CDlgConditionSelectConfig::OnKillfocusListConditions(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int index = GetConditonCurSel();
	
	if (index >= 0)
	{
		*pResult = 0;
		return;
	}
	
	for (int i=0; i<7; i++)
	{
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_MODIFYCONDITION+i);
		if (pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
	}
	*pResult = 0;
}

void CDlgConditionSelectConfig::OnButtonAnd() 
{
	// TODO: Add your control notification handler code here
	int index = GetConditonCurSel();

	if (index < 0)
	{
			CString strText;
			strText.LoadString(IDS_SEL_ONLYONE);
			AfxMessageBox(strText);
			return;
	}

	if (index > 0)
	{
		m_lstConds.SetItemText(index,0,"And");
	}
	

}

void CDlgConditionSelectConfig::OnButtonNot() 
{
	// TODO: Add your control notification handler code here
	int index = GetConditonCurSel();
	
	if (index < 0)
	{
		CString strText;
		strText.LoadString(IDS_SEL_ONLYONE);
		AfxMessageBox(strText);
		return;
	}
	
	CString strOp = m_lstConds.GetItemText(index,2);
	if (strOp.CompareNoCase("=") == 0)
	{
		m_lstConds.SetItemText(index,2,"<>");
	}
	else if (strOp.CompareNoCase("<>") == 0)
	{
		m_lstConds.SetItemText(index,2,"=");
	}
	else if (strOp.CompareNoCase(">") == 0)
	{
		m_lstConds.SetItemText(index,2,"<");
	}
	else if (strOp.CompareNoCase("<") == 0)
	{
		m_lstConds.SetItemText(index,2,">");
	}
}

void CDlgConditionSelectConfig::OnButtonOr() 
{
	// TODO: Add your control notification handler code here
	int index = GetConditonCurSel();

	if (index < 0)
	{
		CString strText;
		strText.LoadString(IDS_SEL_ONLYONE);
		AfxMessageBox(strText);
		return;
	}
	if (index > 0)
	{
		m_lstConds.SetItemText(index,0,"Or");
	}
}

void CDlgConditionSelectConfig::OnButtonLbracket() 
{
	// TODO: Add your control notification handler code here
	int index = GetConditonCurSel();
	
	if (index < 0)
	{
		CString strText;
		strText.LoadString(IDS_SEL_ONLYONE);
		AfxMessageBox(strText);
		return;
	}
	
	CString str = m_lstConds.GetItemText(index,1);
	if (str.Find('(') == 0)
	{
		str.Delete(0);
	}
	else
	{
		str.Insert(0,'(');
	}

	m_lstConds.SetItemText(index,1,str);
	
}

void CDlgConditionSelectConfig::OnButtonRbracket() 
{
	// TODO: Add your control notification handler code here
	int index = GetConditonCurSel();
	
	if (index < 0)
	{
		CString strText;
		strText.LoadString(IDS_SEL_ONLYONE);
		AfxMessageBox(strText);
		return;
	}	
	
	CString str = m_lstConds.GetItemText(index,3);
	int nLen = str.GetLength();
	if (str.ReverseFind(')') == nLen-1)
	{
		str.Delete(nLen-1);
	}
	else
	{
		str.Insert(nLen,')');
	}
	
	m_lstConds.SetItemText(index,3,str);
}

void CDlgConditionSelectConfig::OnClickListConditions(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here

	if (phdn->iItem < 0)
	{
		for (int i=0; i<7; i++)
		{
			CWnd *pWnd = GetDlgItem(IDC_BUTTON_MODIFYCONDITION+i);
			if (pWnd)
			{
				pWnd->EnableWindow(FALSE);
			}
		}
		*pResult = 0;
		return;
	}
	
	for (int i=0; i<7; i++)
	{
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_MODIFYCONDITION+i);
		if (pWnd)
		{
			pWnd->EnableWindow(TRUE);
		}
	}
	
	
	int nItem = phdn->iItem;
	
	CString strText;
	strText = m_lstConds.GetItemText(nItem,1);
	m_cField.SelectString(-1,strText);
//	m_cField.EnableWindow(FALSE);
	OnSelchangeField();
	
	
	strText = m_lstConds.GetItemText(nItem,2);
	for (i=0; i<4; i++)
	{
		if (strText.CollateNoCase(strOpers[i]) == 0)
		{
			m_cOperator.SetCurSel(i);
			break;
		}
	}
//	m_cOperator.SelectString(-1,strText);
	
	strText = m_lstConds.GetItemText(nItem,3);
	m_strValue = strText;
	m_Color.SetColor(atoi(m_strValue));
	
	UpdateData(FALSE);
	
	
	
	*pResult = 0;
}

