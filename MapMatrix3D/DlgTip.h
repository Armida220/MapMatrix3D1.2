#if !defined(AFX_DLGTIP_H__8C102E6C_AB62_4967_9EB8_F4274E9D3D87__INCLUDED_)
#define AFX_DLGTIP_H__8C102E6C_AB62_4967_9EB8_F4274E9D3D87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTip dialog

class CDlgTip : public CDialog
{
// Construction
public:
	CDlgTip(CWnd* pParent = NULL);   // standard constructor

	void SetTitle(LPCTSTR title);
	void AddTip(LPCTSTR tip);

// Dialog Data
	//{{AFX_DATA(CDlgTip)
	enum { IDD = IDD_TIP_DIALOG };
	CString	m_strTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTip)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString	m_strTip;
	CStringArray m_arrTips;

	// Generated message map functions
	//{{AFX_MSG(CDlgTip)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTIP_H__8C102E6C_AB62_4967_9EB8_F4274E9D3D87__INCLUDED_)
