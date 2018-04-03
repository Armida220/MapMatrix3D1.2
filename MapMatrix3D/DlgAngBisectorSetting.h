#if !defined(AFX_DLGANGBISECTORSETTING_H__DFC99909_4481_490B_98C8_3E34C9A3566C__INCLUDED_)
#define AFX_DLGANGBISECTORSETTING_H__DFC99909_4481_490B_98C8_3E34C9A3566C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAngBisectorSetting.h : header file
//
#include "Symbol.h"
#include "SymbolLib.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgAngbisectorSetting dialog

class CDlgAngbisectorSetting : public CDialog
{
// Construction
public:
	CDlgAngbisectorSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgParalineSetting)
	enum { IDD = IDD_ANGBISECTOR_SETTING };
	float	m_fWidth;
	CString	m_strLineName;
	float	m_fLength;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAngbisectorSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAngbisectorSetting)
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeLenEdit();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnChangeLinetypenameEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetMem(CAngBisectortype *pAng, ConfigLibItem config);
	
	ConfigLibItem m_config;
 	CAngBisectortype *m_pAng;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGANGBISECTORSETTING_H__DFC99909_4481_490B_98C8_3E34C9A3566C__INCLUDED_)
