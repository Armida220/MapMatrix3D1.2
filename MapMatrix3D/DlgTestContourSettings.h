#if !defined(AFX_DLGTESTCONTOURSETTINGS_H__8E83F0C7_F7A3_4CB8_B837_DBB90B6AC7BD__INCLUDED_)
#define AFX_DLGTESTCONTOURSETTINGS_H__8E83F0C7_F7A3_4CB8_B837_DBB90B6AC7BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTestContourSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTestContourSettings dialog

class CDlgTestContourSettings : public CDialog
{
// Construction
public:
	CDlgTestContourSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTestContourSettings)
	enum { IDD = IDD_TESTCREATE_CONTOURS };
	float	m_fInterval;
	CListCtrl	m_lstCtrl;
	CString m_strLayers;
	//}}AFX_DATA

	CStringArray m_strInitList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTestContourSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTestContourSettings)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
// 	afx_msg void OnSelectall();
// 	afx_msg void OnDeselectall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTESTCONTOURSETTINGS_H__8E83F0C7_F7A3_4CB8_B837_DBB90B6AC7BD__INCLUDED_)
