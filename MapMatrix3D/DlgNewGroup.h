#if !defined(AFX_DLGNEWGROUP_H__7ED99725_70F0_437A_8B39_A6ED195A4C3D__INCLUDED_)
#define AFX_DLGNEWGROUP_H__7ED99725_70F0_437A_8B39_A6ED195A4C3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewGroup.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgNewGroup dialog

class CDlgNewGroup : public CDialog
{
// Construction
public:
	CDlgNewGroup(CWnd* pParent = NULL);   // standard constructor
	CString GetGroupName() {  return m_strGroupName;  };
	void SetStytle(CString title, CString text){
		m_strTitle = title;
		m_strText = text;
	}
// Dialog Data
	//{{AFX_DATA(CDlgNewGroup)
	enum { IDD = IDD_NEWGROUP_DIALOG };
	CString	m_strGroupName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewGroup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString m_strTitle, m_strText;

	// Generated message map functions
	//{{AFX_MSG(CDlgNewGroup)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWGROUP_H__7ED99725_70F0_437A_8B39_A6ED195A4C3D__INCLUDED_)
