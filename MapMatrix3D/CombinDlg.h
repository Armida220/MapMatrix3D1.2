#if !defined(AFX_COMBINDLG_H__B6D878C6_05C1_4251_8780_1665A999B6F1__INCLUDED_)
#define AFX_COMBINDLG_H__B6D878C6_05C1_4251_8780_1665A999B6F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CombinDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCombinDlg dialog

class CCombinDlg : public CDialog
{
// Construction
public:
	CCombinDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCombinDlg)
	enum { IDD = IDD_COMBIN_DIALOG };
	CString	m_strPath1;
	CString	m_strPath2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCombinDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCombinDlg)
	afx_msg void OnBrowseButton1();
	afx_msg void OnBrowseButton2();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBINDLG_H__B6D878C6_05C1_4251_8780_1665A999B6F1__INCLUDED_)
