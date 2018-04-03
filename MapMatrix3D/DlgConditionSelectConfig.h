#if !defined(AFX_DLGCONDITIONSELECTCONFIG_H__C5CBE03B_4317_474E_A831_7CA7F117180B__INCLUDED_)
#define AFX_DLGCONDITIONSELECTCONFIG_H__C5CBE03B_4317_474E_A831_7CA7F117180B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConditionSelectConfig.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgConditionSelectConfig dialog
#include "resource.h"
#include "DlgConditionSelect.h"
#include "QueryMenu.h"


class CDlgConditionSelectConfig : public CDialog
{
// Construction
public:
	CDlgConditionSelectConfig(CWnd* pParent = NULL);   // standard constructor
	int GetConditonCurSel();

// Dialog Data
	//{{AFX_DATA(CDlgConditionSelectConfig)
	enum { IDD = IDD_CONDITION_SELECT_CONFIG };
	CComboBox	m_cMenu;
	CListCtrl	m_lstConds;
	CComboBox	m_cOperator;
	CListBox	m_cField;
	CString	m_strValue;
	//}}AFX_DATA

	CMFCColorButton m_Color;
	CStringArray m_arrFields;
	CArray<CONDSEL,CONDSEL> m_arrConds;

	CQueryMenu  m_cQueryMenu;
	int m_nCurMenu;
//	CString m_strExpression;

private:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConditionSelectConfig)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	CString ConvertStrValue();
	void SaveCurMenu();
	void LoadCurMenu();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConditionSelectConfig)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeField();
	afx_msg void OnButtonModify();
	afx_msg void OnKillfocusListConditions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonAddMenuitem();
	afx_msg void OnButtonDelMenuitem();
	afx_msg void OnSelchangeMenuitem();
	afx_msg void OnEditchangeMenuitem();
	afx_msg void OnButtonAnd();
	afx_msg void OnButtonNot();
	afx_msg void OnButtonOr();
	afx_msg void OnButtonLbracket();
	afx_msg void OnButtonRbracket();
	afx_msg void OnClickListConditions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateColor();
	afx_msg void OnDropdownMenuitem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONDITIONSELECTCONFIG_H__C5CBE03B_4317_474E_A831_7CA7F117180B__INCLUDED_)
