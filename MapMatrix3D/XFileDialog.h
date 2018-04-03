#if !defined(AFX_XFILEDIALOG_H__5F881109_EC6F_4AA7_B4E2_808268DAD0A5__INCLUDED_)
#define AFX_XFILEDIALOG_H__5F881109_EC6F_4AA7_B4E2_808268DAD0A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXFileDialog dialog

class CXFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CXFileDialog)
		
		// Construction
public:
	CXFileDialog(LPCTSTR lpszTitle,
		BOOL bOpenFileDialog, // TRUE for Open, FALSE for Save As
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	
	// Dialog Data
	//{{AFX_DATA(CXFileDialog)
	BOOL	m_bCheckLoadAllMdl;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXFileDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Operations
public:
	// override
	//virtual INT_PTR DoModal();
	virtual BOOL OnFileNameOK();
	virtual BOOL OnInitDialog();
	
	// Implementation
protected:
	CString			m_strTitle;
	
	DECLARE_MESSAGE_MAP()
		//{{AFX_MSG(CXFileDialog)
	//}}AFX_MSG
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XFILEDIALOG_H__5F881109_EC6F_4AA7_B4E2_808268DAD0A5__INCLUDED_)
