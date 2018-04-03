#if !defined(AFX_DLGNEXTLAST_H__7870C47A_3DA2_4C86_9AA3_160930E76718__INCLUDED_)
#define AFX_DLGNEXTLAST_H__7870C47A_3DA2_4C86_9AA3_160930E76718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNextLast.h : header file
//


class CDlgDoc;

/////////////////////////////////////////////////////////////////////////////
// CDlgNextLast dialog

class CDlgNextLast : public CDialog
{
// Construction
public:
	CDlgNextLast(CWnd* pParent = NULL);   // standard constructor

	void Init(CDlgDoc *pDoc);
// Dialog Data
	//{{AFX_DATA(CDlgNextLast)
	enum { IDD = IDD_NEXT_LAST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CDlgDoc *m_pDoc;
	int m_nCurIndex;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNextLast)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
	//{{AFX_MSG(CDlgNextLast)
	afx_msg void OnButtonLast();
	afx_msg void OnButtonNext();
	//}}AFX_MSG

protected:
	DECLARE_MESSAGE_MAP()
};

class CDlgLocateFtrs : public CDialog
{
	// Construction
public:
	CDlgLocateFtrs(CWnd* pParent = NULL);   // standard constructor
	
	void Init(CDlgDoc *pDoc);
	// Dialog Data
	//{{AFX_DATA(CDlgNextLast)
	enum { IDD = IDD_LOCATE_FTRS };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	CDlgDoc *m_pDoc;
	int m_nCurIndex;
	CString m_strLayer;
	CFtrArray m_arrFtrs;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNextLast)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgNextLast)
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonLast();
	afx_msg void OnButtonNext();
	//}}AFX_MSG
	
protected:
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEXTLAST_H__7870C47A_3DA2_4C86_9AA3_160930E76718__INCLUDED_)
