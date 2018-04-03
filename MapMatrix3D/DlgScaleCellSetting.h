#if !defined(AFX_DLGSCALECELLSETTING_H__1D2EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_)
#define AFX_DLGSCALECELLSETTING_H__1D2EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgScaleCellSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleCellSetting dialog

#include "Symbol.h"
#include "SymbolLib.h"
#include "UIFSimpleEx.h"

class CDlgScaleCellSetting : public CSonDialog
{
// Construction
public:
	CDlgScaleCellSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgScaleCellSetting)
	enum { IDD = IDD_SCALECELL_SETTINGDLG };
	CString	m_strCellDefName;
	float	m_fX1;
	float	m_fX2;
	float	m_fY1;
	float	m_fWidth;
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScaleCellSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScaleCellSetting)
	afx_msg void OnChangeCelldefnameEdit();
	afx_msg void OnPreviewButton();	
	afx_msg void OnChangeX1Edit();
	afx_msg void OnChangeX2Edit();
	afx_msg void OnChangeY1Edit();
	afx_msg void OnChangeWidthEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetMem(CScaleCell *pSym, ConfigLibItem config);

	CScaleCell *m_pSym;
	ConfigLibItem m_config;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALECELLSETTING_H__1D2EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_)
