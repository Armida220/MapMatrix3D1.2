#if !defined(AFX_DLGEXPORTASC_H__A1A6F5A2_5956_45BB_B910_6BCF52A8D5DD__INCLUDED_)
#define AFX_DLGEXPORTASC_H__A1A6F5A2_5956_45BB_B910_6BCF52A8D5DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportASC.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportASC dialog

class CDlgExportASC : public CDialog
{
// Construction
public:
	CDlgExportASC(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportASC)
	enum { IDD = IDD_EXPORT_ASC };
		// NOTE: the ClassWizard will add data members here
	CString	m_strPath;
	CString m_strLstFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportASC)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportASC)
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

#endif // !defined(AFX_DLGEXPORTASC_H__A1A6F5A2_5956_45BB_B910_6BCF52A8D5DD__INCLUDED_)
