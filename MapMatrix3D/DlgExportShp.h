#if !defined(AFX_DLGEXPORTSHP_H__B914F3BD_AE08_406B_9FE0_19DD983CA42A__INCLUDED_)
#define AFX_DLGEXPORTSHP_H__B914F3BD_AE08_406B_9FE0_19DD983CA42A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportShp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportShp dialog

class CDlgExportShp : public CDialog
{
// Construction
public:
	CDlgExportShp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportShp)
	enum { IDD = IDD_EXPORT_SHP };
	CString	m_strPath;
	CString m_strLstFile;
	CString m_strLstFile2;
	BOOL m_bBaseAtt;
	BOOL m_bExtAtt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportShp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportShp)
	afx_msg void OnButtonIdBrowse();
	afx_msg void OnBrowseLst();
	afx_msg void OnBrowseLst2();
	virtual void OnOK();
	afx_msg void OnSetfocusEditLstFile();
	afx_msg void OnKillfocusEditLstFile();
	afx_msg void OnSetfocusEditLstFile2();
	afx_msg void OnKillfocusEditLstFile2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTSHP_H__B914F3BD_AE08_406B_9FE0_19DD983CA42A__INCLUDED_)
