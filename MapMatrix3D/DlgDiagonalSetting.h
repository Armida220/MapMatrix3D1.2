#if !defined(AFX_DLGDIAGONALSETTING_H__7C2F2B66_EAF9_4DB1_8D56_446BB7A48295__INCLUDED_)
#define AFX_DLGDIAGONALSETTING_H__7C2F2B66_EAF9_4DB1_8D56_446BB7A48295__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDiagonalSetting.h : header file
//
#include "Symbol.h"
#include "SymbolLib.h"
#include "UIFSimpleEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgDiagonalSetting dialog

class CDlgDiagonalSetting : public CSonDialog
{
// Construction
public:
	CDlgDiagonalSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDiagonalSetting)
	enum { IDD = IDD_DIAGONAL_SETTINGDLG };
	CString	m_strLineName;
	float	m_fWidth;
	CComboBox	m_cDiagonalType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDiagonalSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDiagonalSetting)
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeLinetypenameEdit();
	afx_msg void OnSelchangeDiagonaltypeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetMem(CDiagonal *pDiagonal, ConfigLibItem config);
	
private:
	CDiagonal *m_pDiagonal;
	ConfigLibItem m_config;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDIAGONALSETTING_H__7C2F2B66_EAF9_4DB1_8D56_446BB7A48295__INCLUDED_)
