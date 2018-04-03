#if !defined(AFX_DLGCONDITIONSELECT_H__C5CBE03B_4317_474E_A831_7CA7F117180B__INCLUDED_)
#define AFX_DLGCONDITIONSELECT_H__C5CBE03B_4317_474E_A831_7CA7F117180B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConditionSelect.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgConditionSelect dialog
#include "resource.h"

struct CONDSEL
{ 
	BOOL condtype[3];   // { and,(,) }
	char field[256];
//	int fidx;
	char value[256];
	int op;
};


class CDlgConditionSelect : public CDialog
{
// Construction
public:
	CDlgConditionSelect(CWnd* pParent = NULL);   // standard constructor
	
	int GetConditonCurSel();

// Dialog Data
	//{{AFX_DATA(CDlgConditionSelect)
	enum { IDD = IDD_CONDITION_SELECT };
	CListCtrl	m_lstConds;
	CComboBox	m_cOperator;
	CListBox	m_cField;
	CString	    m_strValue;
	//}}AFX_DATA

	CMFCColorButton m_Color;
	CStringArray m_arrFields;
	CArray<CONDSEL,CONDSEL> m_arrConds;
//	CString m_strExpression;

private:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConditionSelect)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	CString ConvertStrValue();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConditionSelect)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeField();
	afx_msg void OnButtonModify();
	afx_msg void OnKillfocusListConditions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonAnd();
	afx_msg void OnButtonNot();
	afx_msg void OnButtonOr();
	afx_msg void OnButtonLbracket();
	afx_msg void OnButtonRbracket();
	afx_msg void OnClickListConditions(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONDITIONSELECT_H__C5CBE03B_4317_474E_A831_7CA7F117180B__INCLUDED_)
