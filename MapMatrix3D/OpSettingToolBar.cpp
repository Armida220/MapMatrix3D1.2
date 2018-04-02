// OpSettingToolBar.cpp: implementation of the COpSettingToolBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "OpSettingToolBar.h"
#include "exmessage.h"
#include "EditBaseDoc.h"
#include "UIParam2.h"

#include "UIFLayerProp.h"
#include "UIFBoolProp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define WM_SETCMDPARAMS		(WM_USER+1)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpSettingToolBar::COpSettingToolBar()
{
	m_pLoadParam = NULL;
}

COpSettingToolBar::~COpSettingToolBar()
{
	if( m_pLoadParam )delete m_pLoadParam;
}

BEGIN_MESSAGE_MAP(COpSettingToolBar, CDockablePane)
	//{{AFX_MSG_MAP(COpSettingToolBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_MESSAGE(WM_SETCMDPARAMS,OnSetCmdParams)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void COpSettingToolBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient,rect;
	GetClientRect (rectClient);

	m_wndPropList.SetWindowPos (NULL,
		rectClient.left + 1, 
		rectClient.top + 1, 
		rectClient.Width () - 2,
		rectClient.Height () - 2,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

int COpSettingToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();
	
	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properies Grid \n");
		return -1;      // fail to create
	}
	
	m_wndPropList.EnableHeaderCtrl (FALSE);
	//m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook ();
	
	return 0;
}


void COpSettingToolBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout ();
}

void COpSettingToolBar::OnSortingprop() 
{
	m_wndPropList.SetAlphabeticMode ();
}

void COpSettingToolBar::OnUpdateSortingprop(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndPropList.IsAlphabeticMode ());
}

void COpSettingToolBar::OnExpand() 
{
	m_wndPropList.SetAlphabeticMode (FALSE);
}

void COpSettingToolBar::OnUpdateExpand(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (!m_wndPropList.IsAlphabeticMode ());
}

void COpSettingToolBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(&rect);

	CBrush brush(RGB(255,255,255));
	dc.FillRect(&rect,&brush);

	CRect rectList;

	m_wndPropList.GetWindowRect (rectList);
	ScreenToClient (rectList);

	rectList.InflateRect (1, 1);
	dc.Draw3dRect (rectList, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));

}

LRESULT COpSettingToolBar::OnPropertyChanged (WPARAM wParam, LPARAM lParam)
{
	if( !m_pLoadParam )return 0;

	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( !pDoc )return 0;

	CUIFProp *pProp = (CUIFProp*)lParam;

	CString name = pProp->GetName();
	CUIParam::ParamItem item;
	if( m_pLoadParam->GetParamByTitle(name,item)<0 )
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
	case CUIParam::MultiEditType:
	case CUIParam::StringType:
	case CUIParam::LayerNameType:
	case CUIParam::UsedLayerNameType:
	case CUIParam::MutiLayerNameType:
	case CUIParam::FontNameType:
	case CUIParam::CheckListType:
	case CUIParam::FileNameType:
	case CUIParam::PathNameType:
	case CUIParam::ColorType:
		var = pProp->GetValue();
		break;
	case CUIParam::OptionType:
		var = (long)item.data.pOptions->GetAt((long)((CUIFPropEx*)pProp)->GetCurSelOfCombo()).value;
		break;		
	}

	m_tabChanged.DelAll();

	m_tabChanged.BeginAddValueItem();
	m_tabChanged.AddValue(item.field,&CVariantEx(var));
	m_tabChanged.EndAddValueItem();

	//这里不能直接调用 pDoc->SetCurCmdParams(); 该函数会导致当前 pProp 销毁，从而引发后续错误
	//所以通过PostMessage延后处理
	PostMessage(WM_SETCMDPARAMS);
	
	return 1;
}


LRESULT COpSettingToolBar::OnSetCmdParams(WPARAM wParam, LPARAM lParam)
{
	if( !m_pLoadParam )return 0;

	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( !pDoc )return 0;

	pDoc->SetCurCmdParams(m_tabChanged);

	return 0;
}


void COpSettingToolBar::LoadParams(const CUIParam *param)
{
	ClearParams();

	if( !m_pLoadParam )m_pLoadParam = new CUIParam;
	if( !m_pLoadParam )return;
	m_pLoadParam->CopyFrom(param);
	CString strTitle(param->m_strTitle);
	strTitle+=StrFromResID(IDS_OPSETTING_TITLE);
	SetWindowText(strTitle);	
	int nsz = m_pLoadParam->m_arrItems.GetSize();
	int j;
	for( j=0; j<nsz; j++)
	{
		CUIParam::ParamItem item = m_pLoadParam->m_arrItems.GetAt(j);
		if (!item.bShow || strcmp(item.field,PF_PENCODE)==0)
		{
			continue;
		}
		CUIFProp *pProp = CreateProp(item);
		if( pProp )m_wndPropList.AddCanHideProperty(pProp,TRUE);
	}
}


void COpSettingToolBar::ShowParams(const CUIParam *param)
{
	if( !param )return;
	if( !m_pLoadParam || m_pLoadParam->m_strID!=param->m_strID )
		return;

	int nsz = param->m_arrItems.GetSize(), ncount;
	int i;
	CArray<int,int> arrExistFlags;

	arrExistFlags.SetSize(nsz);
	memset(arrExistFlags.GetData(),0,sizeof(int)*nsz);

	CUIParam::ParamItem item, it;

	//隐藏参数表中不存在的属性项，并为存在的搭上标记
	while( 1 )
	{
		ncount = m_wndPropList.GetPropertyCount();
		for( i=0; i<ncount; i++)
		{
			CUIFProp *pProp = m_wndPropList.GetProperty(i);

			int idx = param->GetParamByTitle(pProp->GetName(),it);			
			if( idx>=0 )
			{
				arrExistFlags.SetAt(idx,1);
				SetPropValue((CUIFPropEx*)pProp,it);
			}
			else
			{
				m_wndPropList.HidePropertyByName(pProp->GetName());
				break;
			}
		}

		if( i>=ncount )break;
	}

	//将参数表中还没有显示的参数显示为属性项
	for( i=0; i<nsz; i++)
	{
		item = param->m_arrItems.GetAt(i);
// 		if (strcmp(item.field,PF_PENCODE)==0)
// 		{
// 			continue;
// 		}
	//	if(item.type==CUIParam::HotKeyType) continue;
		//已经存在了
		if( arrExistFlags.GetAt(i)==1 )continue;

		m_wndPropList.ShowPropByNameToIndex(item.title,i);
	}
}

/*
#define LAYERPARAMITEM_NOTEMPTY		1
#define LAYERPARAMITEM_LOCAL		2
#define LAYERPARAMITEM_NOTLOCAL		4
#define LAYERPARAMITEM_POINT		8
#define LAYERPARAMITEM_LINE			0x10
#define LAYERPARAMITEM_AREA			0x20
#define LAYERPARAMITEM_TEXT			0x40
*/

void SetLayerPropFlags(CUIFLayerProp *pProp, DWORD flag)
{
	pProp->EnableNotEmpty((flag&LAYERPARAMITEM_NOTEMPTY)!=0);
	pProp->EnableLocalLayers((flag&LAYERPARAMITEM_LOCAL)!=0);
	pProp->EnableNotLocalLayers((flag&LAYERPARAMITEM_NOTLOCAL)!=0);
	pProp->EnablePoint((flag&LAYERPARAMITEM_POINT_SINGLE)!=0);
	pProp->EnableLine((flag&LAYERPARAMITEM_LINE_SINGLE)!=0);
	pProp->EnableArea((flag&LAYERPARAMITEM_AREA_SINGLE)!=0);
	pProp->EnableText((flag&LAYERPARAMITEM_TEXT_SINGLE)!=0);	
}


CUIFProp *COpSettingToolBar::CreateProp(const CUIParam::ParamItem& item)
{
	CUIFProp *pProp = NULL;
	switch(item.type) 
	{
	case CUIParam::NoneType:
		break;
	case CUIParam::IntType:
		pProp = new CUIFPropEx(item.title,(long)item.data.nValue,item.desc);
		break;
	case CUIParam::BoolType:
		pProp = new CUIFBoolProp(item.title,item.data.bValue,item.desc);
		/*
		pProp = new CUIFPropEx(item.title,(LPCTSTR)StrFromResID(IDS_NO),item.desc);
		if( pProp )
		{
			pProp->AddOption(StrFromResID(IDS_YES));
			pProp->AddOption(StrFromResID(IDS_NO));
			pProp->AllowEdit(FALSE);
			pProp->SetCurSelOfCombo(item.data.bValue?0:1);
		}*/
		break;
	case CUIParam::FloatType:
		pProp = new CUIFPropEx(item.title,(float)item.data.fValue,item.desc);
		break;
	case CUIParam::DoubleType:
		pProp = new CUIFPropEx(item.title,(double)item.data.lfValue,item.desc);
		break;
	case CUIParam::StringType:
		pProp = new CUIFPropEx(item.title,(LPCTSTR)*item.data.pStrValue,item.desc);
		break;
	case CUIParam::MultiEditType:
		pProp = new CUIFMultiEditProp(item.title,(LPCTSTR)*item.data.pStrValue,item.desc);
		break;
	case CUIParam::UsedLayerNameType:
	case CUIParam::LayerNameType:
		pProp = new CUIFLayerProp(item.title,(LPCTSTR)*item.data.pStrValue,item.desc);
		{
			CDlgDoc *pDoc = GetActiveDlgDoc();
			if( pDoc )
			{
				((CUIFLayerProp*)pProp)->SetLayersOption(pDoc->GetDlgDataSource());
			}
		}
		if( item.type==CUIParam::UsedLayerNameType )
		{
			((CUIFLayerProp*)pProp)->EnableNotEmpty(TRUE);
			((CUIFLayerProp*)pProp)->EnableNotLocalLayers(TRUE);
		}
		SetLayerPropFlags((CUIFLayerProp*)pProp,item.exParam);
		break;
	case CUIParam::MutiLayerNameType:
		pProp = new CUIFLayerProp(item.title,(LPCTSTR)*item.data.pStrValue,item.desc);
		((CUIFLayerProp*)pProp)->EnableMutiSelMode(TRUE);
		{
			CDlgDoc *pDoc = GetActiveDlgDoc();
			if( pDoc )
			{
				((CUIFLayerProp*)pProp)->SetLayersOption(pDoc->GetDlgDataSource());
			}
		}
		SetLayerPropFlags((CUIFLayerProp*)pProp,item.exParam);
		break;	
	case CUIParam::FontNameType:
		pProp = new CUIFFontNameProp(item.title,(LPCTSTR)*item.data.pStrValue,item.desc);
		
		((CUIFFontNameProp*)pProp)->LoadFontNames();
		break;
	case CUIParam::ButtonType:
		pProp = new CUIFButtonProp(item.title,item.desc);
		break;
	case CUIParam::OptionType:
		if( item.data.pOptions->GetSize()>50 )
			pProp = new CUIFPropEx(item.title,(LPCTSTR)StrFromResID(IDS_NO),item.desc);
		else
			pProp = new CUIFLongNamePropEx(item.title,(LPCTSTR)StrFromResID(IDS_NO),item.desc);

		if( pProp )
		{
			int nsz = item.data.pOptions->GetSize();
			int nsel = 0;
			for( int i=0; i<nsz; i++)
			{
				pProp->AddOption(item.data.pOptions->GetAt(i).desc);
				if( item.data.pOptions->GetAt(i).flag==1 )nsel = i;
			}
			pProp->AllowEdit(FALSE);
			((CUIFPropEx*)pProp)->SetCurSelOfCombo(nsel);
		}
		break;
	case CUIParam::CheckListType:
		pProp = new CUIFCheckListProp(item.title,(LPCTSTR)StrFromResID(IDS_NO),item.desc);
		if( pProp )
		{
			CUIFCheckListProp *p = (CUIFCheckListProp*)pProp;

			CStringArray names;
			CArray<int,int> flags;
			int nsz = item.data.pOptions->GetSize();

			names.SetSize(nsz);
			flags.SetSize(nsz);

			for( int i=0; i<nsz; i++)
			{
				names[i] = item.data.pOptions->GetAt(i).desc;
				flags[i] = item.data.pOptions->GetAt(i).value;
			}
			p->SetList(names,&flags);
		}
		break;
	case CUIParam::FileNameType:
		pProp = new CUIFFilePropEx(item.title,TRUE,*item.data.FileName.pFileName,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,*item.data.FileName.pFileFilter,item.desc);
		break;
	case CUIParam::PathNameType:
		pProp = new CUIFFilePropEx(item.title,(LPCTSTR)*item.data.pStrValue,0,item.desc);
		break;
	case CUIParam::ColorType:
		pProp = new CMFCPropertyGridColorProperty(item.title, item.data.lValue, 0, item.desc);
		((CMFCPropertyGridColorProperty*)pProp)->EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
		break;
	default:;
	}
	return pProp;
}


void COpSettingToolBar::SetPropValue(CUIFProp *pProp, const CUIParam::ParamItem& item)
{
	switch(item.type) 
	{
	case CUIParam::NoneType:
		break;
	case CUIParam::IntType:
		if( item.data.nValue!=(long)(_variant_t)pProp->GetValue() )
			pProp->SetValue((long)item.data.nValue);
		break;
	case CUIParam::BoolType:
		//if( item.data.bValue!=((long)pProp->GetCurSelOfCombo()==0) )
		//	pProp->SetCurSelOfCombo(item.data.bValue?0:1);
		pProp->SetValue(item.data.bValue ? VARIANT_TRUE : VARIANT_FALSE);
		break;
	case CUIParam::ColorType:
		//if( item.data.bValue!=((long)pProp->GetCurSelOfCombo()==0) )
		//	pProp->SetCurSelOfCombo(item.data.bValue?0:1);
		pProp->SetValue((long)item.data.lValue);
		break;
	case CUIParam::FloatType:
		if (item.data.fValue != (long)(_variant_t)pProp->GetValue())
			pProp->SetValue((float)item.data.fValue);
		break;
	case CUIParam::DoubleType:
		if (item.data.lfValue != (double)(_variant_t)pProp->GetValue())
			pProp->SetValue((double)item.data.lfValue);
		break;
	case CUIParam::MultiEditType:
	case CUIParam::StringType:
	case CUIParam::PathNameType:
	case CUIParam::LayerNameType:
	case CUIParam::UsedLayerNameType:
	case CUIParam::MutiLayerNameType:
	case CUIParam::FontNameType:
		if( item.data.pStrValue->CompareNoCase((LPCTSTR)(_bstr_t)pProp->GetValue())!=0 )
			pProp->SetValue((LPCTSTR)*item.data.pStrValue);
		break;
	case CUIParam::FileNameType:
		if( item.data.FileName.pFileName->CompareNoCase((LPCTSTR)(_bstr_t)pProp->GetValue())!=0 )
			pProp->SetValue((LPCTSTR)*item.data.FileName.pFileName);
		break;
	case CUIParam::OptionType:
		{
			int nsz = item.data.pOptions->GetSize();
			int nsel = 0;
			for( int i=0; i<nsz; i++)
			{
				if( item.data.pOptions->GetAt(i).flag==1 )nsel = i;
			}
			if( nsel!=((CUIFPropEx*)pProp)->GetCurSelOfCombo() )
				((CUIFPropEx*)pProp)->SetCurSelOfCombo(nsel);
		}
		break;
	case CUIParam::CheckListType:
		{
			CUIFCheckListProp *p = DYNAMIC_DOWNCAST(CUIFCheckListProp,pProp);

			if( p )
			{
				CArray<int,int> flags;
				int nsz = item.data.pOptions->GetSize();
				
				flags.SetSize(nsz);
				
				for( int i=0; i<nsz; i++)
				{
					flags[i] = item.data.pOptions->GetAt(i).value;
				}
				p->SetValue(flags);
			}
		}
		break;
	default:;
	}
	return;
}

void COpSettingToolBar::ClearParams()
{
	SetWindowText(StrFromResID(IDS_OPSETTING_TITLE));
	if( m_pLoadParam )delete m_pLoadParam;
	m_pLoadParam = NULL;

	m_wndPropList.SetCurSel(NULL);
	m_wndPropList.RemoveAll();
	m_wndPropList.RemoveAllHideProps();

	m_wndPropList.Invalidate(TRUE);
	m_wndPropList.UpdateWindow();
}


void COpSettingToolBar::OutputMsg(LPCTSTR msg)
{
}