// UIFToolbarComboBoxButtonEx.cpp: implementation of the CUIFToolbarComboBoxButtonEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "featurecollector.h"
#include "UIFToolbarComboBoxButtonEx.h"
#include "ExMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CUIFToolbarComboBoxButtonEx, CMFCToolBarComboBoxButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFToolbarComboBoxButtonEx::CUIFToolbarComboBoxButtonEx()
{

}

CUIFToolbarComboBoxButtonEx::CUIFToolbarComboBoxButtonEx(UINT uiID, int iImage, DWORD dwStyle, int iWidth)
	:CMFCToolBarComboBoxButton(uiID, iImage, dwStyle, iWidth)
{
}

CUIFToolbarComboBoxButtonEx::~CUIFToolbarComboBoxButtonEx()
{

}


CComboBox* CUIFToolbarComboBoxButtonEx::CreateCombo(CWnd* pWndParent, const CRect& rect)
{
	CComboBox* pWndCombo = new CComboBox;
	if (!pWndCombo->Create(m_dwStyle, rect, pWndParent, m_nID))
	{
		delete pWndCombo;
		return NULL;
	}

	return pWndCombo;
}

void CUIFToolbarComboBoxButtonEx::Serialize(CArchive& ar)
{
	CMFCToolBarComboBoxButton::Serialize(ar);
}


BOOL CUIFToolbarComboBoxButtonEx::NotifyCommand(int iNotifyCode)
{
	BOOL ret = CMFCToolBarComboBoxButton::NotifyCommand(iNotifyCode);
	switch (iNotifyCode)
	{
	case CBN_SELENDOK:
	case CBN_KILLFOCUS:
	case CBN_EDITUPDATE:
		::SetFocus(AfxGetMainWnd()->m_hWnd);
		return ret;
		break;
	}

	return ret;
}

BOOL CUIFToolbarComboBoxButtonEx::OnClick(CWnd* pWnd, BOOL /*bDelay*/)
{	
	if (m_pWndCombo == NULL || m_pWndCombo->GetSafeHwnd () == NULL || !m_bHorz)
	{
		return FALSE;
	}
	
	if (m_bFlat)
	{
		if (m_pWndEdit == NULL)
		{
			m_pWndCombo->SetFocus ();
		}
		else
		{
			m_pWndEdit->SetFocus ();
		}
		
		m_pWndCombo->ShowDropDown ();		
		m_pWndCombo->SetFocus ();		
		
		if (pWnd != NULL)
		{
			pWnd->InvalidateRect (m_rectCombo);
		}
	}
	
	return TRUE;
}

///////////////////CUIFToolbarCustomizeEx/////////////////////
BEGIN_MESSAGE_MAP(CUIFToolbarCustomizeEx, CMFCToolBarsCustomizeDialog)
	//{{AFX_MSG_MAP(CUIFToolbarCustomizeEx)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CUIFToolbarCustomizeEx::CUIFToolbarCustomizeEx(CFrameWnd* pWndParentFrame, BOOL bAutoSetFromMenus)
	: CMFCToolBarsCustomizeDialog(pWndParentFrame, bAutoSetFromMenus)
{
}

void CUIFToolbarCustomizeEx::OnDestroy()
{
	CWnd *pWnd = GetParent();
	if( !pWnd )pWnd = AfxGetMainWnd();
	if( pWnd )pWnd->SendMessage(FCCM_CUSTOMDLGCLOSE);
	CMFCToolBarsCustomizeDialog::OnDestroy();
}

LPCTSTR CUIFToolbarCustomizeEx::GetCommandName (UINT uiCmd) const
{
	LPCTSTR str = CMFCToolBarsCustomizeDialog::GetCommandName(uiCmd);
	if( !str )
	{
		static CString strRes;
		if( strRes.LoadString(uiCmd) )return (LPCTSTR)strRes;
	}

	return str;
}