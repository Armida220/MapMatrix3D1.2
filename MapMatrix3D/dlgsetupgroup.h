#if !defined(AFX_DLGSETUPGROUP_H__9A755108_31E9_4EE8_ABBB_B8377B1F3F83__INCLUDED_)
#define AFX_DLGSETUPGROUP_H__9A755108_31E9_4EE8_ABBB_B8377B1F3F83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgsetupgroup.h : header file
//
#include "editbasedoc.h"
#include "DlgDataSource.h"
#include "ComboListCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSetupGroup dialog

class CDlgSetupGroup : public CDialog
{
// Construction
public:
	CDlgSetupGroup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetupGroup)
	enum { IDD = IDD_GROUP };
	CComboListCtrl	m_cGroup;
	CString	m_strGroupName;
	//}}AFX_DATA

	BOOL FillList();
	void Init(CDlgDoc *pDoc);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetupGroup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetupGroup)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonBreak();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonHide();
	afx_msg void OnButtonNewgroup();
	afx_msg void OnButtonShow();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	afx_msg void OnItemchangedListGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnEndLabelEditVariableCriteria(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT PopulateComboList(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CDlgDoc *m_pDoc;
	ObjectGroup*      m_pCurGroup;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETUPGROUP_H__9A755108_31E9_4EE8_ABBB_B8377B1F3F83__INCLUDED_)
