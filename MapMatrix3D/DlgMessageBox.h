// DlgMessageBox.h: interface for the CDlgMessageBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGMESSAGEBOX_H__F79A520D_CB3C_4884_885B_82169573C753__INCLUDED_)
#define AFX_DLGMESSAGEBOX_H__F79A520D_CB3C_4884_885B_82169573C753__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MYMB_YES					0x000100000L
#define MYMB_NO						0x000200000L
#define MYMB_YESALL					0x001000000L
#define MYMB_NOALL					0x002000000L
#define MYMB_NEVER_WARNING			0x004000000L

#define MYMB_YESNO_YESALL			(MYMB_YES|MYMB_NO|MYMB_YESALL)

#define IDYESALL					0x000000010L
#define IDC_MSGDLG_CHECK			1000

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox dialog

class CDlgMessageBox : public CDialog
{
	// Construction
public:
	CDlgMessageBox(CWnd* pParent = NULL);   // standard constructor
	static int Do(LPCTSTR lpszText, LPCTSTR lpszCaption=NULL, UINT nType=MB_OK );
	// Dialog Data
	CString m_strCaption;
	CString m_strText;
	UINT m_nType;

	CStatic m_wndText;
	CStatic m_wndIcon;
	CPtrArray m_arrBtn;
	int  m_nRet;
	static BOOL m_bCheck;
	//{{AFX_DATA(CDlgMessageBox)
	//}}AFX_DATA	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMessageBox)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgMessageBox)
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoModal();
	afx_msg void OnYes();
	afx_msg void OnNo();
	afx_msg void OnYesAll();
	afx_msg void OnClickCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_DLGMESSAGEBOX_H__F79A520D_CB3C_4884_885B_82169573C753__INCLUDED_)
