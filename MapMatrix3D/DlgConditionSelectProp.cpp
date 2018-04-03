// DlgConditionSelectProp.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgConditionSelectProp.h"
#include "SmartViewFunctions.h"
#include "Linearizer.h"
#include "GeoCurve.h"
#include "DlgInputName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString strOpers[];

extern CString FirstStrFromResID(UINT id);

extern void PenCodeToText(int pencode, char* text);

extern int TextToPenCode(const char*text);


/////////////////////////////////////////////////////////////////////////////
// CDlgConditionSelectProp dialog


CDlgConditionSelectProp::CDlgConditionSelectProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConditionSelectProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConditionSelectProp)
	m_strValue = _T("");
	//}}AFX_DATA_INIT
}

CDlgConditionSelectProp::~CDlgConditionSelectProp()
{
}

void CDlgConditionSelectProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConditionSelectProp)
	DDX_Control(pDX, IDC_LIST_CONDITIONS, m_lstCtrlConds);
	DDX_Control(pDX, IDC_COMBO_CONDITIONS, m_wndOldConds);
	DDX_Control(pDX, IDC_LIST_FIELD, m_cField);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_strValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConditionSelectProp, CDialog)
	//{{AFX_MSG_MAP(CDlgConditionSelectProp)
	ON_BN_CLICKED(IDC_BUTTON_ADDCONDITION, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELCONDITION, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_ADDOLD,OnButtonAddOldConditions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConditionSelectProp message handlers


void CDlgConditionSelectProp::OnButtonAdd() 
{
	UpdateData(TRUE);

	int count = m_lstCtrlConds.GetItemCount();
	m_lstCtrlConds.InsertItem (count,NULL);

	CString strField;
	int nsel = m_cField.GetCurSel();
	if( nsel>=0 )
	{
		m_cField.GetText(nsel,strField);
		m_lstCtrlConds.SetItemText(count,0,strField);
	}
	
	m_lstCtrlConds.SetItemText(count,1,m_strValue);

	//记录条件
	CONDSEL item;
	memset(&item,0,sizeof(item));
	
	strncpy(item.field,(LPCTSTR)strField,sizeof(item.field)-1);
	strncpy(item.value,(LPCTSTR)m_strValue,sizeof(item.value)-1);
		
	m_arrConds.Add(item);
}


void CDlgConditionSelectProp::OnButtonDel() 
{
	CArray<int,int> arrIdxs;

	int nItem = -1;
	POSITION pos = m_lstCtrlConds.GetFirstSelectedItemPosition();
	if (pos)
	{
		nItem = m_lstCtrlConds.GetNextSelectedItem(pos);
	}

	if	(nItem < 0)
	{
		CString strText;
		strText.LoadString(IDS_SEL_ONLYONE);
		AfxMessageBox(strText);
		return;
	}

	pos = m_lstCtrlConds.GetFirstSelectedItemPosition();
	while( pos )
	{
		nItem = m_lstCtrlConds.GetNextSelectedItem(pos);
		arrIdxs.Add(nItem);
	}

	for( int i=arrIdxs.GetSize()-1; i>=0; i--)
	{
		nItem = arrIdxs[i];
		m_lstCtrlConds.DeleteItem(nItem);
		m_arrConds.RemoveAt(nItem);
	}
}

BOOL CDlgConditionSelectProp::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_lstCtrlConds.SetExtendedStyle(m_lstCtrlConds.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	
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

	//条件列表
	int nw = 8;
	pDC = m_lstCtrlConds.GetDC();
	if( pDC )
	{
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		nw = tm.tmAveCharWidth;
		m_lstCtrlConds.ReleaseDC(pDC);
	}
	
	m_lstCtrlConds.InsertColumn(0,StrFromResID(IDS_COND_COL_FIELD),LVCFMT_LEFT,nw*8+10);
	m_lstCtrlConds.InsertColumn(1,StrFromResID(IDS_COND_COL_VALUE),LVCFMT_LEFT,nw*4+30);
	
	m_cQueryMenu.Load("Conditions.xml");

	//菜单列表
	if( m_cQueryMenu.m_arrMenus.GetSize()>0 )
	{
		for( i=0; i<m_cQueryMenu.m_arrMenus.GetSize(); i++)
		{
			CondMenu *p = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(i);
			if( p )m_wndOldConds.AddString(p->name);
		}
	}

	//初始化条件
	if( m_arrConds.GetSize()>0 )
	{
		for( i=0; i<m_arrConds.GetSize(); i++)
		{
			m_lstCtrlConds.InsertItem (i,NULL);
			
			CONDSEL cs = m_arrConds.GetAt(i);			
			
			m_lstCtrlConds.SetItemText(i,0,cs.field);			
			m_lstCtrlConds.SetItemText(i,1,cs.value);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CDlgConditionSelectProp::OnOK() 
{
	//如果存在完全相同的条件,就不加入
	int nMenu = m_cQueryMenu.m_arrMenus.GetSize();

	int i,j, k, nsz = m_arrConds.GetSize();

	if( nsz>0 )
	{
		//自动生成一个名称
		CString name, name0;
		for( i=0; i<nsz; i++)
		{
			name0 += m_arrConds[i].field;
			name0 += ",";
		}
		
		if( name0.GetLength()>=24 )
		{
			name0 = name0.Left(24) + "...";
		}

		for( i=0; i<nMenu; i++)
		{
			CondMenu *pMenu = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(i);
			if( pMenu->arrConds.GetSize()==m_arrConds.GetSize() )
			{
				for( j=0; j<nsz; j++)
				{
					for( k=0; k<nsz; k++)
					{
						if( stricmp(m_arrConds[j].field,pMenu->arrConds[k].field)==0 &&
							stricmp(m_arrConds[j].value,pMenu->arrConds[k].value)==0 )
							break;
					}
					
					if( k>=nsz )
						break;
				}
				if( j>=nsz )
					break;
			}
		}
		
		//找到了完全相同的条件，就不保存了
		if( i<nMenu )
		{
			m_strCondsName = name0;
		}
		else
		{
			//使用一个不同的文件名
			name = name0;
			for( i=1; ; i++)
			{
				for( j=0; j<nMenu; j++)
				{
					CondMenu *pMenu = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(j);
					if( name.CompareNoCase(pMenu->name)==0 )
						break;
				}
				if( j<nMenu )
				{
					name.Format("%s%d",(LPCTSTR)name0,i);
				}
				else
				{
					break;
				}
			}
			
			CondMenu *pMenu = new CondMenu();
			pMenu->arrConds.Copy(m_arrConds);
			pMenu->name = name;
			
			m_strCondsName = name;
			
			m_cQueryMenu.m_arrMenus.InsertAt(0,pMenu);
			nMenu++;
			if( nMenu>20 )
			{
				delete (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(nMenu-1);
				m_cQueryMenu.m_arrMenus.RemoveAt(nMenu-1);
			}
			
			m_cQueryMenu.Save("Conditions.xml");
		}
	}

	CDialog::OnOK();
}



BOOL CDlgConditionSelectProp::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN )
	{
		CComboBox *pCtrl = (CComboBox*)GetDlgItem(IDC_EDIT_VALUE);
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



void CDlgConditionSelectProp::OnButtonAddOldConditions()
{
	int nSel = m_wndOldConds.GetCurSel();
	if( nSel<0 )
	{
		return;
	}
	
	CondMenu *pMenu = (CondMenu*)m_cQueryMenu.m_arrMenus.GetAt(nSel);
	if( !pMenu )return;
	m_arrConds.Append(pMenu->arrConds);
	
	int nCount = m_lstCtrlConds.GetItemCount();
	
	for( int i=0; i<pMenu->arrConds.GetSize(); i++)
	{
		m_lstCtrlConds.InsertItem (nCount+i,NULL);
		
		CONDSEL cs = pMenu->arrConds.GetAt(i);			
		
		m_lstCtrlConds.SetItemText(nCount+i,0,cs.field);			
		m_lstCtrlConds.SetItemText(nCount+i,1,cs.value);
	}
}


void CDlgConditionSelectProp::SetFields(CStringArray& fields)
{
	m_arrFields.Copy(fields);
}
