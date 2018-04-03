#if !defined(AFX_DLGCONDITIONSELECTPROP_H__A5CBE03B_4317_474E_A831_7CA7F117180B__INCLUDED_)
#define AFX_DLGCONDITIONSELECTPROP_H__A5CBE03B_4317_474E_A831_7CA7F117180B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConditionSelectProp.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgConditionSelectProp dialog
#include "resource.h"
#include "DlgConditionSelect.h"
#include "QueryMenu.h"

class CDlgConditionSelectProp : public CDialog
{
// Construction
public:
	CDlgConditionSelectProp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgConditionSelectProp();

	void SetFields(CStringArray& fields);

// Dialog Data
	//{{AFX_DATA(CDlgConditionSelectProp)
	enum { IDD = IDD_CONDITION_SELECT_PROP };
	CListCtrl	m_lstCtrlConds;
	CComboBox	m_wndOldConds;
	CListBox	m_cField;
	CString	    m_strValue;
	//}}AFX_DATA

	CStringArray m_arrFields;
	CArray<CONDSEL,CONDSEL> m_arrConds;

	CQueryMenu  m_cQueryMenu;
	CString m_strCondsName;

private:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgConditionSelectProp)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgConditionSelectProp)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonAddOldConditions();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONDITIONSELECTPROP_H__A5CBE03B_4317_474E_A831_7CA7F117180B__INCLUDED_)
