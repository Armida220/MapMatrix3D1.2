#if !defined(AFX_DLGIMPORTSHP_H__B914F3BD_AE08_406B_9FE0_19DD983CA42A__INCLUDED_)
#define AFX_DLGIMPORTSHP_H__B914F3BD_AE08_406B_9FE0_19DD983CA42A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportShp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImportShp dialog

class CDlgImportShp : public CDialog
{
// Construction
public:
	CDlgImportShp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImportShp)
	enum { IDD = IDD_IMPORT_SHP };
	CString	m_strPath;
	CString m_strLstFile;
	CString m_strLstFile2;
	//}}AFX_DATA

	CStringArray m_arrFileNames;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportShp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportShp)
	afx_msg void OnButtonIdBrowse();
	afx_msg void OnBrowseLst();
	virtual void OnOK();
	afx_msg void OnSetfocusEditLstFile();
	afx_msg void OnKillfocusEditLstFile();

	afx_msg void OnBrowseLst2();
	afx_msg void OnSetfocusEditLstFile2();
	afx_msg void OnKillfocusEditLstFile2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTSHP_H__B914F3BD_AE08_406B_9FE0_19DD983CA42A__INCLUDED_)
