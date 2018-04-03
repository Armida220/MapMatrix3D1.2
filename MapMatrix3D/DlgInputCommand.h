#if !defined(AFX_DLGINPUTCOMMAND_H__22C0823A_E28C_489C_89A1_555939956A63__INCLUDED_)
#define AFX_DLGINPUTCOMMAND_H__22C0823A_E28C_489C_89A1_555939956A63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlginputcommand.h : header file
//

#define WM_COMMANDSELCHANGE		(WM_USER + 500)
#define WM_COMMANDSELRETURN		(WM_USER + 501)
/////////////////////////////////////////////////////////////////////////////
// CDlgInputCommand dialog

class CDlgInputCommand : public CDialog
{
// Construction
public:
	CDlgInputCommand(CWnd* pParent = NULL);   // standard constructor

	void SetProcessWnd(CWnd *pWnd) {
		m_pProcessEventWnd = pWnd;
	}

	int AddString(LPCTSTR lpszItem);
	void ResetContent();
	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect);
	DWORD_PTR GetItemData(int nIndex) const;
	int SetItemData(int nIndex, DWORD_PTR dwItemData);

	void AdjustSize();
// Dialog Data
	//{{AFX_DATA(CDlgInputCommand)
	enum { IDD = IDD_INPUTCOMMAND };
	CListBox	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputCommand)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg BOOL OnNcActivate( BOOL bActive );	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// Generated message map functions
	//{{AFX_MSG(CDlgInputCommand)
	afx_msg void OnSelchangeListCommand();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CWnd *m_pProcessEventWnd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPUTCOMMAND_H__22C0823A_E28C_489C_89A1_555939956A63__INCLUDED_)
