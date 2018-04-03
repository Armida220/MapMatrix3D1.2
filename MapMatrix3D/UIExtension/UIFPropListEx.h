// UIFPropListEx.h: interface for the CUIFPropListEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIFPROPLISTEX_H__36D0E164_3937_4C3D_AF4A_D55B7F80F904__INCLUDED_)
#define AFX_UIFPROPLISTEX_H__36D0E164_3937_4C3D_AF4A_D55B7F80F904__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxpropertygridctrl.h>

#include "UIFPropEx.h"

class CUIFPropListEx : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CUIFPropListEx)
public:
	CUIFPropListEx();
	virtual ~CUIFPropListEx();

	BOOL HidePropertyByData(DWORD_PTR data, BOOL bRedraw = FALSE);
	BOOL ShowPropByDataToIndex(DWORD_PTR data, int idx, BOOL bRedraw = FALSE);
	BOOL HidePropertyByName(LPCTSTR name, BOOL bRedraw=FALSE);
	BOOL ShowPropByNameToIndex(LPCTSTR name, int idx, BOOL bRedraw=FALSE);

	void RemoveAllHideProps();
	void AddCanHideProperty(CUIFProp* pProp, BOOL bHide=FALSE);
	void InsertProperty(CUIFProp* pProp,int idx, BOOL bRedraw=TRUE);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int	 FindProperty(DWORD_PTR data);
	CUIFProp* FindProperty(LPCTSTR name);
	// 回溯查找每一个prop
	CUIFProp* FindProperty(LPCTSTR name, CUIFProp *prop);

	void SaveCurValue();

	virtual void UpdateColor (COLORREF color);
protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

protected:
	CList<CUIFProp*, CUIFProp*>	m_lstHideProps;

	/*
	** brief 颜色刷子
	*/
	CBrush m_brush;

	
};

#endif // !defined(AFX_UIFPROPLISTEX_H__36D0E164_3937_4C3D_AF4A_D55B7F80F904__INCLUDED_)
