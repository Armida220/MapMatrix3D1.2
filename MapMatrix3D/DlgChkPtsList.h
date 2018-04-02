#if !defined(AFX_DLGCHKPTSLIST_H__F009235F_08DD_4FB4_9585_FE8746A8A0AF__INCLUDED_)
#define AFX_DLGCHKPTSLIST_H__F009235F_08DD_4FB4_9585_FE8746A8A0AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChkPtsList.h : header file
//Author:hcw,2013.1.14,
#include"Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgChkPtsList dialog
class CDlgChkPtsList : public CDialog
{
// Construction
public:
	CDlgChkPtsList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgChkPtsList)
	enum { IDD = IDD_CHKPTLIST };
	CListBox	m_listChkPts;
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChkPtsList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void AddtoList(CStringArray& strArr);
	CString GetSelectedStr();
//	void SetSelectedItem(int nIndex);

public:
	BOOL m_bDbClickListItem;//双击选中列表项。
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgChkPtsList)
	afx_msg void OnDblclkListChkpts();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHKPTSLIST_H__F009235F_08DD_4FB4_9585_FE8746A8A0AF__INCLUDED_)
