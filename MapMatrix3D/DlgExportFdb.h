#if !defined(AFX_DLGEXPORTFDB_H__BCEC8F06_DA99_4EC9_BC9B_4BF480903438__INCLUDED_)
#define AFX_DLGEXPORTFDB_H__BCEC8F06_DA99_4EC9_BC9B_4BF480903438__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportFdb.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportFdb dialog

class CDlgExportFdb : public CDialog
{
// Construction
public:
	CDlgExportFdb(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportFdb)
	enum { IDD = IDD_EXPORT_FDB };
	CString	m_strFilePath;
	int		m_nOption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportFdb)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportFdb)
	afx_msg void OnButtonFdbBrowser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTFDB_H__BCEC8F06_DA99_4EC9_BC9B_4BF480903438__INCLUDED_)
