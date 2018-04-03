// UIFLayerProp.cpp: implementation of the CUIFLayerProp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "UIFLayerProp.h"
#include "DlgDataSource.h "
#include "SmartViewFunctions.h"

#include "DlgSelectLayer.h"
#include "DlgColors.h"
#include "DlgCellDefLinetypeView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define PROP_HAS_LIST	0x0001
#define PROP_HAS_BUTTON	0x0002
#define PROP_HAS_SPIN	0x0004


IMPLEMENT_DYNAMIC(CUIFLayerProp,CUIFPropEx)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUIFLayerProp::CUIFLayerProp(const CString& strName, const CString& varValue, 
LPCTSTR lpszDescr, DWORD_PTR dwData)
			  :CUIFPropEx(strName,(_variant_t)(LPCTSTR)varValue,lpszDescr,dwData,
			  NULL,NULL,NULL)
{
	m_pDS = NULL;
	m_bMutiSel = FALSE;
	m_dwFlags = PROP_HAS_BUTTON;

	m_bLocalLayers = TRUE;
	m_bNotLocalLayers = FALSE;

	m_bPoint = TRUE;
	m_bLine = TRUE;
	m_bArea = TRUE;
	m_bText = TRUE;
}

CUIFLayerProp::~CUIFLayerProp()
{

}

void CUIFLayerProp::SetLayersOption( CDlgDataSource *pDS)
{
	m_pDS = pDS;
/*
	RemoveAllOptions();
	if (pDS!=NULL)
	{	
		for (int i=0;i<pDS->GetFtrLayerCount();i++)
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);	
			if(pLayer)
				AddOption(pLayer->GetName());
		}
	}
*/
}

void CUIFLayerProp::EnableMutiSelMode(BOOL bMutiSel)
{
	m_bMutiSel = bMutiSel;
}

void CUIFLayerProp::EnableNotEmpty(BOOL bEnable)
{
	m_bNotEmpty = bEnable;
}

void CUIFLayerProp::EnableLocalLayers(BOOL bEnable)
{
	m_bLocalLayers = bEnable;
}

void CUIFLayerProp::EnableNotLocalLayers(BOOL bEnable)
{
	m_bNotLocalLayers = bEnable;
}


void CUIFLayerProp::EnablePoint(BOOL bEnable)
{
	m_bPoint = bEnable;
}


void CUIFLayerProp::EnableLine(BOOL bEnable)
{
	m_bLine = bEnable;
}


void CUIFLayerProp::EnableArea(BOOL bEnable)
{
	m_bArea = bEnable;
}


void CUIFLayerProp::EnableText(BOOL bEnable)
{
	m_bText = bEnable;
}

void CUIFLayerProp::OnClickButton(CPoint point)
{
	CDlgSelectFtrLayer dlg(NULL,m_bMutiSel?LAYER_SEL_MODE_MUTISEL:LAYER_SEL_MODE_SINGLESEL);
	dlg.m_pDS = m_pDS;	
	dlg.m_bUsed = m_bNotEmpty;
	dlg.m_bLocal = m_bLocalLayers;
	dlg.m_bNotLocal = m_bNotLocalLayers;

	dlg.m_bPoint = m_bPoint;	
	dlg.m_bLine = m_bLine;
	dlg.m_bArea = m_bArea;
	dlg.m_bText = m_bText;

	if(m_bMutiSel)
	{	
		dlg.m_strLayers = (LPCTSTR)(_bstr_t)GetValue();
	}
	if( dlg.DoModal()!=IDOK )
		return;

//	SetMultiValueState(FALSE,NULL);

	if(!m_bMutiSel)
	{
		if( dlg.m_pRetLayer!=NULL )
		{
			m_varValue = dlg.m_pRetLayer->GetName();
			
			if (m_pWndInPlace != NULL)
			{
				m_pWndInPlace->SetWindowText (dlg.m_pRetLayer->GetName());
			}
			Redraw ();
			
			if (m_pWndInPlace != NULL)
			{
				m_pWndInPlace->SetFocus ();
			}
			else
			{
				m_pWndList->SetFocus ();
			}
		}
	}
	else
	{		
		m_varValue = dlg.m_strLayers;
		if (m_pWndInPlace != NULL)
		{
			m_pWndInPlace->SetWindowText (dlg.m_strLayers);
		}
		Redraw ();
		
		if (m_pWndInPlace != NULL)
		{
			m_pWndInPlace->SetFocus ();
		}
		else
		{
			m_pWndList->SetFocus ();
		}
	}
	
}

BOOL CUIFLayerProp::OnUpdateValue()
{
	return CUIFProp::OnUpdateValue();

	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT_VALID (m_pWndList);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));
	
	CString strText;
	m_pWndInPlace->GetWindowText (strText);
	
	BOOL bRes = FALSE;
	CString strOld;
	strOld = FormatProperty ();
	BOOL bIsChanged = strOld != strText;
	if (bIsChanged && m_lstOptions.Find(strText)==NULL)
	{
		return TRUE;
	}

	switch (m_varValue.vt)
	{
	case VT_BSTR:
		m_varValue = (LPCTSTR) strText;
		bRes = TRUE;
		break;
		
    case VT_I2:
		m_varValue = (short) _ttoi (strText);
		bRes = TRUE;
		break;
		
	case VT_I4:
		m_varValue = _ttol (strText);
		bRes = TRUE;
		break;
		
    case VT_R4:
		{
			float fVal;
			_stscanf (strText, _T("%f"), &fVal);
			m_varValue = fVal;
			bRes = TRUE;
		}
		break;
		
    case VT_R8:
		{
			double dblVal;
			_stscanf(strText, _T("%lf"), &dblVal);
			m_varValue = dblVal;
			bRes = TRUE;
		}
		break;
		
	case VT_DATE:
		break;
		
    case VT_BOOL:
		{
			m_varValue = (long)(strText == "Yes");
			bRes = TRUE;
		}
		break;
		
	default:
		break;
	}
	
	if (bRes && bIsChanged &&m_lstOptions.Find(strText)!=NULL )
	{
		m_pWndList->OnPropertyChanged (this);
	}
	return bRes;

}



IMPLEMENT_DYNAMIC(CUIFMultiColorProp,CUIFPropEx)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUIFMultiColorProp::CUIFMultiColorProp(const CString& strName, const CString& varValue, 
LPCTSTR lpszDescr, DWORD_PTR dwData)
									   :CUIFPropEx(strName,(_variant_t)(LPCTSTR)varValue,lpszDescr,dwData,
									   NULL,NULL,NULL)
{
	m_dwFlags = PROP_HAS_BUTTON;
}

CUIFMultiColorProp::~CUIFMultiColorProp()
{
	
}


void CUIFMultiColorProp::OnClickButton(CPoint point)
{
	//判断当前编辑框的输入光标位置是否在在结尾处，如果是，选中的颜色就追加到后面；否则，颜色采用替换的方式
	BOOL bFocusEnd = FALSE;
	if( m_pWndInPlace && m_pWndInPlace->IsKindOf(RUNTIME_CLASS(CEdit)) )
	{
		CEdit *pEdit = (CEdit*)m_pWndInPlace;
		DWORD pos = (pEdit->GetSel()&0xffff);
		int nLine = pEdit->LineLength(0);
		if( nLine>0 && pos==nLine )
			bFocusEnd = TRUE;
	}

	CDlgColors dlg;

	dlg.m_Colors.Copy(m_colors);
	
	if( dlg.DoModal()!=IDOK )
		return;

//	SetMultiValueState(FALSE,NULL);
	
	CString text = (LPCTSTR)(_bstr_t)m_varValue;
	CString clr;
	clr.Format("%06X",dlg.m_RetColor);

	if( text.Find(clr)<0 )
	{		
		if( bFocusEnd )
		{
			if( !text.IsEmpty() )
			{
				text += ",";
			}
		}
		else
		{
			text.Empty();
		}
		
		text += clr;
	}
	
	m_varValue = (LPCTSTR)text;
	
	Redraw ();
	
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText(text);
		m_pWndInPlace->SetFocus ();

		//保持光标的位置不变
		if( bFocusEnd )
		{
			CEdit *pEdit = (CEdit*)m_pWndInPlace;
			int nLine = pEdit->LineLength(0);
			pEdit->SetSel(nLine,nLine,TRUE);
		}
		else if( m_pWndInPlace->IsKindOf(RUNTIME_CLASS(CEdit)) )
		{
			CEdit *pEdit = (CEdit*)m_pWndInPlace;
			pEdit->SetSel(0,0,TRUE);
		}
	}
	else
	{
		m_pWndList->SetFocus ();
	}
}



IMPLEMENT_DYNAMIC(CUIFCellLinetypeProp,CUIFPropEx)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUIFCellLinetypeProp::CUIFCellLinetypeProp(const CString& strName, const CString& varValue, 
LPCTSTR lpszDescr, DWORD_PTR dwData)
			  :CUIFPropEx(strName,(_variant_t)(LPCTSTR)varValue,lpszDescr,dwData,
			  NULL,NULL,NULL)
{
	m_nScale = 0;
	m_nSymType = 0;

	m_dwFlags = PROP_HAS_BUTTON;

}

CUIFCellLinetypeProp::~CUIFCellLinetypeProp()
{

}

void CUIFCellLinetypeProp::SetSymbolClass(int nScale, int nSymType)
{
	m_nScale = nScale;
	m_nSymType = nSymType;
}


void CUIFCellLinetypeProp::OnClickButton(CPoint point)
{
	CDlgCellDefLinetypeView dlg;

	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(m_nScale);
	dlg.SetConfig(config);

	dlg.SetShowMode(FALSE,TRUE,0,(LPCTSTR)(_bstr_t)m_varValue);
	dlg.SetAsSelectMode(m_nSymType);

	if( dlg.DoModal()!=IDOK )
		return;

//	SetMultiValueState(FALSE,NULL);

	CString strValue = CString("@") + dlg.GetName();

	m_varValue = (LPCTSTR)strValue;
	
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText (strValue);
	}
	Redraw ();
	
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus ();
	}
	else
	{
		m_pWndList->SetFocus ();
	}	
}
