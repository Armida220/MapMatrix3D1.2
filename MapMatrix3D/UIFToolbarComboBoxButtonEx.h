// UIFToolbarComboBoxButtonEx.h: interface for the CUIFToolbarComboBoxButtonEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIFTOOLBARCOMBOBOXBUTTONEX_H__ACF54D4F_7157_45D5_8AD5_CE50287EB093__INCLUDED_)
#define AFX_UIFTOOLBARCOMBOBOXBUTTONEX_H__ACF54D4F_7157_45D5_8AD5_CE50287EB093__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUIFToolbarComboBoxButtonEx : public CMFCToolBarComboBoxButton
{
	friend class CUIFComboEdit;
	DECLARE_SERIAL(CUIFToolbarComboBoxButtonEx)
public:
	CUIFToolbarComboBoxButtonEx();
	CUIFToolbarComboBoxButtonEx(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0);
	virtual ~CUIFToolbarComboBoxButtonEx();
	virtual CComboBox* CreateCombo(CWnd* pWndParent, const CRect& rect);
	virtual void Serialize (CArchive& ar);
	virtual BOOL NotifyCommand (int iNotifyCode);
	virtual BOOL OnClick (CWnd* pWnd, BOOL bDelay = TRUE);
	virtual BOOL IsDroppedDown () const
	{
		return m_pWndCombo!=NULL;
	}
};


class CUIFToolbarCustomizeEx : public CMFCToolBarsCustomizeDialog
{
public:
	CUIFToolbarCustomizeEx (CFrameWnd* pWndParentFrame, BOOL bAutoSetFromMenus = FALSE);
	virtual ~CUIFToolbarCustomizeEx(){}
	virtual LPCTSTR GetCommandName (UINT uiCmd) const;
protected:
	//{{AFX_MSG(CUIFToolbarCustomizeEx)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_UIFTOOLBARCOMBOBOXBUTTONEX_H__ACF54D4F_7157_45D5_8AD5_CE50287EB093__INCLUDED_)
