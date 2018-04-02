#if !defined(AFX_DLGCREATECONTOURS1_H__E181092D_25F8_4EB9_BDAB_4EC5D2B66C75__INCLUDED_)
#define AFX_DLGCREATECONTOURS1_H__E181092D_25F8_4EB9_BDAB_4EC5D2B66C75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCreateContours1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCreateContours1 dialog

class CDlgCreateContours1 : public CDialog
{
// Construction
public:
	CDlgCreateContours1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCreateContours1)
	enum { IDD = IDD_CREATE_CONTOURS1 };
	CString	m_strCountFCode;
	int		m_nExCode;
	CString	m_strIndexFCode;
	float	m_fInterval;
	int		m_nInterNum;
	CString m_strDEMFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCreateContours1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCreateContours1)
	virtual void OnOK();
	afx_msg void OnChangeEditInterval();
	afx_msg void OnBrowseDEMFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCREATECONTOURS_H__E181092D_25F8_4EB9_BDAB_4EC5D2B66C75__INCLUDED_)
