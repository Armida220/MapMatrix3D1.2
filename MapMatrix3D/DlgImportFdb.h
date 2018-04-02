#if !defined(AFX_DLGIMPORTFDB_H__7F06FE7E_C24F_4556_B2D9_A6EACDCF8329__INCLUDED_)
#define AFX_DLGIMPORTFDB_H__7F06FE7E_C24F_4556_B2D9_A6EACDCF8329__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportFdb.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImportFdb dialog

class CDlgImportFdb : public CDialog
{
// Construction
public:
	CDlgImportFdb(CWnd* pParent = NULL);   // standard constructor

	CString GetPathName() { return m_strPath; };
	CString GetMapName()  { return m_strMapName; };
	void SetFilter(CString filter) { m_strFilter = filter; };
	void SetTitle(CString title) { m_strTitle = title; };
	
// Dialog Data
	//{{AFX_DATA(CDlgImportFdb)
	enum { IDD = IDD_IMPORT_FDB };
	CString	m_strPath;
	CString	m_strMapName;
	//}}AFX_DATA

	CStringArray m_arrFileNames;
	BOOL m_bOnDB;
	BOOL m_bCheckOnlyLocal;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportFdb)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	CString m_strFilter;
	CString m_strTitle;

	// Generated message map functions
	//{{AFX_MSG(CDlgImportFdb)
	afx_msg void OnButtonIdBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTFDB_H__7F06FE7E_C24F_4556_B2D9_A6EACDCF8329__INCLUDED_)
