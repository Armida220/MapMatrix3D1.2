#if !defined(AFX_DLGSCALEARCSETTING_H__0CC72941_7473_4C6C_816B_7C4CA5AA51E9__INCLUDED_)
#define AFX_DLGSCALEARCSETTING_H__0CC72941_7473_4C6C_816B_7C4CA5AA51E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgScaleArcSetting.h : header file
//
#include "Symbol.h"
#include "UIFSimpleEx.h"
#include "SymbolLib.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgScaleArcSetting dialog

class CDlgScaleArcSetting : public CDialog
{
// Construction
public:
	CDlgScaleArcSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgScaleArcSetting)
	enum { IDD = IDD_SCALEARC_SETTING };
	CComboBox	m_cPlaceType;
	float	m_fArcLen;
	CString	m_strName;
	float	m_fWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScaleArcSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScaleArcSetting)
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeLenEdit();
	afx_msg void OnChangeLinetypenameEdit();
	afx_msg void OnSelchangePlacetypeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetMem(CScaleArctype *pScale, ConfigLibItem m_config);
	
	ConfigLibItem m_config;
	CScaleArctype *m_pScale;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALEARCSETTING_H__0CC72941_7473_4C6C_816B_7C4CA5AA51E9__INCLUDED_)
