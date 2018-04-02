#if !defined(AFX_DLGEXPORTMAPSTARTEXT_H__4A00C308_7B92_4BFB_BEE5_730A5568C063__INCLUDED_)
#define AFX_DLGEXPORTMAPSTARTEXT_H__4A00C308_7B92_4BFB_BEE5_730A5568C063__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportMapStarText.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExportMapStarText dialog

class CDlgExportMapStarText : public CDialog
{
// Construction
public:
	CDlgExportMapStarText(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExportMapStarText)
	enum { IDD = IDD_EXPORT_MAPSTARTEXT };
	CString	m_strDir;
	CString	m_strLstFile;
	float	m_fContourInterval;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportMapStarText)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExportMapStarText)
	afx_msg void OnButtonBrowse1();
	afx_msg void OnButtonBrowse2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTMAPSTARTEXT_H__4A00C308_7B92_4BFB_BEE5_730A5568C063__INCLUDED_)
