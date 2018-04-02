#if !defined(AFX_DLGNEWLINETYPE_H__0400543C_F6CC_453C_9200_B5D8D8846226__INCLUDED_)
#define AFX_DLGNEWLINETYPE_H__0400543C_F6CC_453C_9200_B5D8D8846226__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewLinetype.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNewLinetype dialog
#include "Symbol.h"

class CDlgNewLinetype : public CDialog
{
// Construction
public:
	CDlgNewLinetype(CWnd* pParent = NULL);   // standard constructor

	BOOL GetLineType(BaseLineType &line);
	void SetLineType(const BaseLineType &line);
	BOOL IsModified()  { return m_bModified; }

private:
	CString m_strOldName;
	CString m_strOldContent;
	BOOL m_bModified;
// Dialog Data
	//{{AFX_DATA(CDlgNewLinetype)
	enum { IDD = IDD_ADDLINETYPE_DIALOG };
	CString	m_strContent;
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewLinetype)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewLinetype)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWLINETYPE_H__0400543C_F6CC_453C_9200_B5D8D8846226__INCLUDED_)
