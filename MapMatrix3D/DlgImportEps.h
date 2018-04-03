#if !defined(AFX_DLGIMPORTEPS_H__40101CE9_3DD4_485A_9684_0CC0CD7F656C__INCLUDED_)
#define AFX_DLGIMPORTEPS_H__40101CE9_3DD4_485A_9684_0CC0CD7F656C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportEps.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImportEps dialog

class CDlgImportEps : public CDialog
{
// Construction
public:
	CDlgImportEps(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImportEps)
	enum { IDD = IDD_IMPORT_EPS };
	CString	m_strPath;
	CString	m_strLstFile;
	CString	m_strMapName;
	CString	m_strRefTmp;
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportEps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportEps)
	afx_msg void OnButtonIdBrowse();
	afx_msg void OnButtonEdBrowse2();
	afx_msg void OnKillfocusEditEdFilepath2();
	afx_msg void OnSetfocusEditEdFilepath2();
	virtual void OnOK();
	afx_msg void OnButtonEdBrowse4();
	afx_msg void OnKillfocusEditReftmpPath();
	afx_msg void OnSetfocusEditReftmpPath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTEPS_H__40101CE9_3DD4_485A_9684_0CC0CD7F656C__INCLUDED_)
