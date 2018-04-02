#if !defined(AFX_DLGCREATECONTOURS_H__E181092D_25F8_4EB9_BDAB_4EC5D2B66C75__INCLUDED_)
#define AFX_DLGCREATECONTOURS_H__E181092D_25F8_4EB9_BDAB_4EC5D2B66C75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCreateContours.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCreateContours dialog

class CDlgCreateContours : public CDialog
{
// Construction
public:
	CDlgCreateContours(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCreateContours)
	enum { IDD = IDD_CREATE_CONTOURS };
	CString	m_strCountFCode;
	int		m_nExCode;
	CString	m_strIndexFCode;
	float	m_fInterval;
	int		m_nInterNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCreateContours)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCreateContours)
	virtual void OnOK();
	afx_msg void OnChangeEditInterval();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCREATECONTOURS_H__E181092D_25F8_4EB9_BDAB_4EC5D2B66C75__INCLUDED_)
