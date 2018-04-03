#if !defined(AFX_DLGOPSETTINGS_H__E4BC0F14_D445_44E6_9DD7_7CDF0F944FAF__INCLUDED_)
#define AFX_DLGOPSETTINGS_H__E4BC0F14_D445_44E6_9DD7_7CDF0F944FAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOpSettings.h : header file
//

#include "UIFPropListExPlus.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgOpSettings dialog

class CDlgOpSettings : public CDialog
{
// Construction
public:
	CDlgOpSettings(CWnd* pParent = NULL);   // standard constructor

	CDlgOpSettings(UINT nIDTemplate, CWnd* pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CDlgOpSettings)
	enum { IDD = IDD_OPSETTINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void GetValueTable(CValueTable& tab);

	void SetUIParam(CUIParam *p){
		m_param = p;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOpSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CUIFPropListExPlus m_wndPropList;
	CUIParam *m_param;
	CValueTable m_tab;

	// Generated message map functions
	//{{AFX_MSG(CDlgOpSettings)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOPSETTINGS_H__E4BC0F14_D445_44E6_9DD7_7CDF0F944FAF__INCLUDED_)
