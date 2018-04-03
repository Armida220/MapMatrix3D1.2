#if !defined(AFX_DLGREORIENTATION_H__06001137_12C0_456B_B9E0_07C7522F0C6B__INCLUDED_)
#define AFX_DLGREORIENTATION_H__06001137_12C0_456B_B9E0_07C7522F0C6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgReorientation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgReorientation dialog

class CDlgReorientation : public CDialog
{
// Construction
public:
	CDlgReorientation(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgReorientation)
	enum { IDD = IDD_REORIENTATION };
	CListBox	m_wndDataFile;
	CString	m_strPrjFile1;
	CString	m_strPrjFile2;
	int m_nCvtWay;
	//}}AFX_DATA
	CString m_strDataFile;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgReorientation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void UpdateDataFileList();
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgReorientation)
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREORIENTATION_H__06001137_12C0_456B_B9E0_07C7522F0C6B__INCLUDED_)
