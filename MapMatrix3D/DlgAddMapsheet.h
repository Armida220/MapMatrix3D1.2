#if !defined(AFX_DIALOGADDMAPSHEET_H__0E644088_6CB0_431F_866A_D08AC27F5A7D__INCLUDED_)
#define AFX_DIALOGADDMAPSHEET_H__0E644088_6CB0_431F_866A_D08AC27F5A7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddMapsheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAddMapsheet dialog

class CDlgAddMapsheet : public CDialog
{
// Construction
public:
	CDlgAddMapsheet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAddMapsheet)
	enum { IDD = IDD_ADDMAPSHEET };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CString m_mapsheet_name;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddMapsheet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAddMapsheet)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGADDMAPSHEET_H__0E644088_6CB0_431F_866A_D08AC27F5A7D__INCLUDED_)
