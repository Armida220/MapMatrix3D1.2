// DlgExtProperties.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgExtProperties.h"
#include "DlgDataSource.h"
#include "editbasedoc.h"
#include "UIFBoolProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL convertStringToStrArray(LPCTSTR str, CStringArray &arr);

/////////////////////////////////////////////////////////////////////////////
// CDlgExtProperties dialog


CDlgExtProperties::CDlgExtProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExtProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExtProperties)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgExtProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExtProperties)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExtProperties, CDialog)
	//{{AFX_MSG_MAP(CDlgExtProperties)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED,OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExtProperties message handlers
void CDlgExtProperties::OnOK()
{	
	m_wndPropList.SaveCurValue();

	CDialog::OnOK();
}


CDlgExtProperties::~CDlgExtProperties()
{
	if( m_pSelEx )delete m_pSelEx;
}

BOOL CDlgExtProperties::Init(CDlgDoc *pDoc, CFeature *pFtr)
{
	if (pDoc == NULL || pFtr == NULL) return FALSE;
	m_pDoc = pDoc;
	m_pFtr = pFtr;
	m_pSelEx = new CSelChangedExchanger(m_pFtr,m_pDoc);
	return m_pSelEx!=NULL?TRUE:FALSE;
}

BOOL CDlgExtProperties::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, CRect(), this, 2))
	{
		TRACE0("Failed to create Properies Grid \n");
		return -1;      // fail to create
	}
	
	CWnd *pWnd = GetDlgItem(IDC_STATIC_LISTPOS);
	if( pWnd!=NULL )
	{
		CRect rcWnd;
		pWnd->GetWindowRect(&rcWnd);
		ScreenToClient(&rcWnd);
		
		m_wndPropList.SetWindowPos(NULL,rcWnd.left,rcWnd.top,
			rcWnd.Width(),rcWnd.Height(),0);
	}
	
	//扩展属性
	if (!m_pSelEx) return FALSE;

	CValueTable tab;
	if(!m_pSelEx->GetXAttributes(tab)) return FALSE;
	CUIFProp *pGroup = new CUIFProp (StrFromResID(IDS_PROPERTIES_EXTEND));
	if (!pGroup)  return FALSE;

	int nXDefCount = 0;
	const XDefine *defs = nullptr;

	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	if(pDS)
	{
		CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
		CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(m_pFtr);
		CSchemeLayerDefine *pDef = pScheme->GetLayerDefine(pLayer?pLayer->GetName():NULL);
		if(pDef)
		{
			defs = pDef->GetXDefines(nXDefCount);
		}
	}
    
	const CVariantEx *pVar; 
	int type;
	CString field,name;
	for(int i=0;i<tab.GetFieldCount();i++)
	{
		tab.GetField(i,field,type,name);
		tab.GetValue(0,i,pVar);
		CUIFProp *pProp;

		if (defs)
		{
			int j = 0;
			for (j = 0; j < nXDefCount; j++)
			{
				if (field == defs[j].field)
					break;
			}
			if (j < nXDefCount)
			{
				if (defs[j].isMust == 1)
				{
					name += "*";
				}
				//枚举属性
				CString strRange = defs[j].valueRange;
				if (!strRange.IsEmpty() && strRange.Find('~')<0)
				{
					CStringArray arr;
					convertStringToStrArray(defs[j].valueRange, arr);
					pProp = new CUIFProp(name, (_variant_t)*pVar, name, i);
					for (int k = 0; k < arr.GetSize(); k++)
					{
						pProp->AddOption(arr[k]);
					}
					pProp->AllowEdit(FALSE);
					pGroup->AddSubItem(pProp);
					break;
				}
			}
		}

		switch(type)
		{
		case VT_I2:			
		case VT_I4:			
		case VT_R4:	    
		case VT_R8:		   
		case VT_BSTR:
			pProp = new CUIFProp(name,(_variant_t)*pVar,name,i);
			break;
		case VT_BOOL:
			pProp = new CUIFBoolProp(name,(long)(_variant_t)*pVar,name);
			break;	
		default:
			break;
		}
		if (pProp)
		{
			pProp->AllowEdit(TRUE);
			pGroup->AddSubItem(pProp);
		}
	}	
	m_wndPropList.AddProperty(pGroup);

	if (tab.GetFieldCount() <= 0)
	{
		CDialog::OnCancel();
		return FALSE;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgExtProperties::OnPropertyChanged (WPARAM wParam, LPARAM lParam)
{
	CUIFProp* pProp = (CUIFProp*)lParam;
	if(!pProp)return 0;
	
	_variant_t var = pProp->GetValue();		
	CValueTable tab;
	m_pSelEx->GetXAttributes(tab);

	DWORD_PTR dw = pProp->GetData();
	CString field,name;
	int type;	
	
	tab.GetField(dw,field,type,name);

	for (int i=0;i<tab.GetItemCount();i++)
	{
		tab.SetValue(i,(LPCSTR)field,new CVariantEx(var));			
	}

	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	CAttributesSource  *pDXS = pDS->GetXAttributesSource();
	if(!pDXS) return 0;

	for (i=0;i<m_pSelEx->GetObjectCount();i++)
	{		
		CFeature *pFtr = CPFeature(m_pSelEx->GetObject(i));
		if (!pFtr) continue;

		CFtrLayer *pFtrLayer = pDS->GetFtrLayerOfObject(pFtr);
		if( pFtrLayer==NULL )
			continue;
		
		pDXS->SetXAttributes(pFtr,tab,i);
	}

	//m_pSelEx->OnModifyXAttributes(tab);
	return 0;
}

