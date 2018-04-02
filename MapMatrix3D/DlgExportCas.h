#if !defined(AFX_DLGEXPORTCAS_H__87A359E7_546D_4C28_A7FE_635E5FE05F1B__INCLUDED_)
#define AFX_DLGEXPORTCAS_H__87A359E7_546D_4C28_A7FE_635E5FE05F1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportCas.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportCas dialog

class CDlgExportCas : public CDialog
{
// Construction
public:
	CDlgExportCas(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportCas)
	enum { IDD = IDD_EXPORT_CAS };
		// NOTE: the ClassWizard will add data members here
	CString	m_strPath;
	CString m_strLstFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportCas)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportCas)
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

#endif // !defined(AFX_DLGEXPORTCAS_H__87A359E7_546D_4C28_A7FE_635E5FE05F1B__INCLUDED_)
