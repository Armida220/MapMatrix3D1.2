#if !defined(AFX_DLGEXPORTEPS_H__874578BB_B810_45F0_BAB0_CDD1CAF0682D__INCLUDED_)
#define AFX_DLGEXPORTEPS_H__874578BB_B810_45F0_BAB0_CDD1CAF0682D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportEps.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportEps dialog

class CDlgExportEps : public CDialog
{
// Construction
public:
	CDlgExportEps(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportEps)
	enum { IDD = IDD_EXPORT_EPS };
	CString	m_strFilePath;
	CString	m_strCodeList;
	CString	m_strTemplateFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportEps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportEps)
	afx_msg void OnButtonIdBrowse();
	afx_msg void OnButtonEdBrowse2();
	virtual void OnOK();
	afx_msg void OnSetfocusEditEdFilepath2();
	afx_msg void OnKillfocusEditEdFilepath2();
	afx_msg void OnButtonBrowse3();
	afx_msg void OnSetfocusEditEdFilepath3();
	afx_msg void OnKillfocusEditEdFilepath3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTEPS_H__874578BB_B810_45F0_BAB0_CDD1CAF0682D__INCLUDED_)
