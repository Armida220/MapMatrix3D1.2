// OpSettingToolBar.h: interface for the COpSettingToolBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPSETTINGTOOLBAR_H__DA76871F_F59B_4EC1_92FF_86E28EC39CFC__INCLUDED_)
#define AFX_OPSETTINGTOOLBAR_H__DA76871F_F59B_4EC1_92FF_86E28EC39CFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIFPropEx.h"
#include "UIFPropListEx.h"
#include "UIParam.h"
#include "ValueTable.h"

class COpSettingToolBar : public CDockablePane  
{
public:
	
	COpSettingToolBar();
	virtual ~COpSettingToolBar();

	BOOL IsHideInAutoHideMode () const {return TRUE;}
	void LoadParams(const CUIParam *param);
	void ShowParams(const CUIParam *param);
	void ClearParams();
	void OutputMsg(LPCTSTR msg);
	// Generated message map functions
protected:
	void AdjustLayout();
	
	CUIFProp *CreateProp(const CUIParam::ParamItem& item);
	void SetPropValue(CUIFProp *pProp, const CUIParam::ParamItem& item);	

	//{{AFX_MSG(COpSettingToolBar)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSortingprop();
	afx_msg void OnUpdateSortingprop(CCmdUI* pCmdUI);
	afx_msg void OnExpand();
	afx_msg void OnUpdateExpand(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
	afx_msg LRESULT OnPropertyChanged (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCmdParams(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CUIFPropListEx m_wndPropList;
	CUIParam *m_pLoadParam;

	CValueTable m_tabChanged;
};


#endif // !defined(AFX_OPSETTINGTOOLBAR_H__DA76871F_F59B_4EC1_92FF_86E28EC39CFC__INCLUDED_)
