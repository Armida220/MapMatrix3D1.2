#if !defined(AFX_DLGCHANGEFCODE_H__5FF73001_C569_43BB_808E_24C5AE61222D__INCLUDED_)
#define AFX_DLGCHANGEFCODE_H__5FF73001_C569_43BB_808E_24C5AE61222D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChangeFCode.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeFCode dialog

class CDlgChangeFCode : public CDialog
{
// Construction
public:
	CDlgChangeFCode(CWnd* pParent = NULL);   // standard constructor
	CString str_fcode;
	BOOL can_edit;
// Dialog Data
	//{{AFX_DATA(CDlgChangeFCode)
	enum { IDD = IDD_CHANGFCODE };
	CString	m_strSrcFCode;
	CString	m_strTarFCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChangeFCode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void set_FCode();
    BOOL OnInitDialog();
	// Generated message map functions
	//{{AFX_MSG(CDlgChangeFCode)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHANGEFCODE_H__5FF73001_C569_43BB_808E_24C5AE61222D__INCLUDED_)
