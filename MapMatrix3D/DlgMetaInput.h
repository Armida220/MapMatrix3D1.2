#if !defined(AFX_DLGMETAINPUT_H__8D4C29CF_DD33_4DAA_8839_8095CDCD33B7__INCLUDED_)
#define AFX_DLGMETAINPUT_H__8D4C29CF_DD33_4DAA_8839_8095CDCD33B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMetaInput.h : header file
//

#include "ValueTable.h"
#include "UIFPropListEx.h"
#include "export\JBFormatData.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMetaInput dialog

class CDlgMetaInput : public CDialog
{
// Construction
public:
	CDlgMetaInput(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMetaInput)
	enum { IDD = IDD_METAINPUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CUIFPropListEx m_wndPropList;
	CJBMetaData *m_pData;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMetaInput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	void FillList();
	void GetValues();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMetaInput)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMetaEditfile();
	afx_msg void OnMetaRefresh();
	afx_msg void OnMetaReload();
	afx_msg void OnMetaSetProject();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMETAINPUT_H__8D4C29CF_DD33_4DAA_8839_8095CDCD33B7__INCLUDED_)
