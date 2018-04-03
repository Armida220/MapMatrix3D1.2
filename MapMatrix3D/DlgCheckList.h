#if !defined(AFX_DLGCHECKLIST_H__E4CEE400_1D57_4A3F_AFE8_AE33382F3E39__INCLUDED_)
#define AFX_DLGCHECKLIST_H__E4CEE400_1D57_4A3F_AFE8_AE33382F3E39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCheckList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCheckList dialog

class CDlgCheckList : public CDialog
{
// Construction
public:
	CDlgCheckList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCheckList)
	enum { IDD = IDD_CHECKLIST };
	CCheckListBox	m_wndList;
	//}}AFX_DATA

	CStringArray m_arrStrList;
	CArray<int,int> m_arrFlags;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCheckList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCheckList)
	virtual void OnOK();
	afx_msg void OnSelchangeList();
	afx_msg void OnSelectAll();
	afx_msg void OnSelectNone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHECKLIST_H__E4CEE400_1D57_4A3F_AFE8_AE33382F3E39__INCLUDED_)
