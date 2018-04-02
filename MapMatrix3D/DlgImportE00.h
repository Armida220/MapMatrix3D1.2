#if !defined(AFX_DLGIMPORTE00_H__123456578_AE08_406B_9FE0_19DD983CA42A__INCLUDED_)
#define AFX_DLGIMPORTE00_H__123456578_AE08_406B_9FE0_19DD983CA42A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportShp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImportE00 dialog

class CDlgImportE00 : public CDialog
{
// Construction
public:
	CDlgImportE00(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImportE00)
	enum { IDD = IDD_IMPORT_E00 };
	CString	m_strPath;
	CString m_strLstFile;
	//}}AFX_DATA

	CStringArray m_arrFileNames;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportE00)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportE00)
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

#endif // !defined(AFX_DLGIMPORTE00_H__123456578_AE08_406B_9FE0_19DD983CA42A__INCLUDED_)
