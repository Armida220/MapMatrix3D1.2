#if !defined(AFX_DLGPARALINESETTING_H__DFC99909_4481_490B_98C8_3E34C9A3566C__INCLUDED_)
#define AFX_DLGPARALINESETTING_H__DFC99909_4481_490B_98C8_3E34C9A3566C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgParalineSetting.h : header file
//
#include "Symbol.h"
#include "SymbolLib.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgParalineSetting dialog

class CDlgParalineSetting : public CDialog
{
// Construction
public:
	CDlgParalineSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgParalineSetting)
	enum { IDD = IDD_PARALINE_SETTING };
	float	m_fIntv;
	CString	m_strLineName;
	float	m_fWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgParalineSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgParalineSetting)
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeIntvEdit();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnChangeLinetypenameEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetMem(CParaLinetype *pPara, ConfigLibItem config);
	
	ConfigLibItem m_config;
 	CParaLinetype *m_pPara;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPARALINESETTING_H__DFC99909_4481_490B_98C8_3E34C9A3566C__INCLUDED_)
