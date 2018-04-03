// UIFPropEx.h: interface for the CUIFPropEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIFPROPEX_H__87E8EE4C_39B6_41EB_9177_AFB371674121__INCLUDED_)
#define AFX_UIFPROPEX_H__87E8EE4C_39B6_41EB_9177_AFB371674121__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxpropertygridctrl.h>

#define CUIFProp CMFCPropertyGridProperty

class CUIFPropEx : public CUIFProp
{
	DECLARE_DYNAMIC(CUIFPropEx)
public:
	// Group constructor
	CUIFPropEx(const CString& strGroupName, DWORD_PTR dwData = 0);
	virtual ~CUIFPropEx();

	// Simple property
	CUIFPropEx(const CString& strName, const _variant_t& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);
	int GetCurSelOfCombo();
	void SetCurSelOfCombo(int index);

	virtual BOOL OnUpdateValue ();

	virtual void OnDrawName (CDC* pDC, CRect rect);
	virtual void OnClickName (CPoint point);

	void EnableCheck(BOOL bEnable);
	void SetChecked(BOOL bChecked);
	BOOL GetChecked();

	void SetOwnerList_Public(CMFCPropertyGridCtrl* pWndList);

protected:
	BOOL m_bEnableCheck;
	BOOL m_bChecked;
};

#endif // !defined(AFX_UIFPROPEX_H__87E8EE4C_39B6_41EB_9177_AFB371674121__INCLUDED_)
