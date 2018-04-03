#if !defined(AFX_DLGEXPORTE00_H__123654_5956_45BB_B910_6BCF52A8D5DD__INCLUDED_)
#define AFX_DLGEXPORTE00_H__123654_5956_45BB_B910_6BCF52A8D5DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLGEXPORTE00.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportE00 dialog

class CDlgExportE00 : public CDialog
{
// Construction
public:
	CDlgExportE00(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportE00)
	enum { IDD = IDD_EXPORT_E00 };
		// NOTE: the ClassWizard will add data members here
	CString	m_strPath;
	CString m_strLstFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportE00)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportE00)
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

#endif // !defined(AFX_DLGEXPORTE00_H__123654_5956_45BB_B910_6BCF52A8D5DD__INCLUDED_)
