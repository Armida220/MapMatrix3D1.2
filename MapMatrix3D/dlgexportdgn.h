#if !defined(AFX_DLGEXPORTDGN_H__CE24044C_2D4B_469B_86D1_552F9C7DC53F__INCLUDED_)
#define AFX_DLGEXPORTDGN_H__CE24044C_2D4B_469B_86D1_552F9C7DC53F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgexportdgn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportDGN dialog

class CDlgExportDGN : public CDialog
{
// Construction
public:
	CDlgExportDGN(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportDGN)
	enum { IDD = IDD_EXPORT_DGN };
		// NOTE: the ClassWizard will add data members here
	CString	m_strPath;
	CString m_strLstFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportDGN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportDGN)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnButtonIdBrowse();
	afx_msg void OnBrowseLst();
	virtual void OnOK();
	afx_msg void OnSetfocusEditLstFile();
	afx_msg void OnKillfocusEditLstFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTDGN_H__CE24044C_2D4B_469B_86D1_552F9C7DC53F__INCLUDED_)
