#if !defined(AFX_DLGIMPORTMDB_H__5421FA12_6EDF_43DE_8FEB_7220F85F6ABE__INCLUDED_)
#define AFX_DLGIMPORTMDB_H__5421FA12_6EDF_43DE_8FEB_7220F85F6ABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgImportJB.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgImportJB dialog

class CDlgImportMDB : public CDialog
{
// Construction
public:
	CDlgImportMDB(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImportMDB)
	enum { IDD = IDD_IMPORT_MDBORGDB };
	CString	m_strFilePath;
	CString	m_strLstFile;
	CString	m_strMapName;
	CString m_strKeyFields;
	//}}AFX_DATA 
	int m_nType;// 0 mdb 1 gdb -1ÆäËû
	 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImportJB)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImportJB)
	afx_msg void OnButtonIcBrowse();
	afx_msg void OnButtonIdBrowse2();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusEditIdFilepath2();
	afx_msg void OnKillfocusEditIdFilepath2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTMDB_H__5421FA12_6EDF_43DE_8FEB_7220F85F6ABE__INCLUDED_)
