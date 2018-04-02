#if !defined(AFX_DLGDASHLINESETTING_H__12F4AF31_AF60_40E6_87E7_DEA181777F5A__INCLUDED_)
#define AFX_DLGDASHLINESETTING_H__12F4AF31_AF60_40E6_87E7_DEA181777F5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDashLineSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDashLineSetting dialog

#include "Symbol.h"
#include "UIFSimpleEx.h"
#include "SymbolLib.h"

class CDlgDashLineSetting : public CSonDialog
{
// Construction
public:
	CDlgDashLineSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDashLineSetting)
	enum { IDD = IDD_DASHLINE_SETTING };
	CMFCColorButton	m_Color;
	CComboBox	m_cUseSefcolor;
	CComboBox	m_cPlaceType;
	CComboBox	m_cIndentType;
	float	m_fBaseOffSet;
	CString	m_strLineTypeName;
	float	m_fWidth;
	float   m_fIndent;
	float   m_fXOffset;
	BOOL	m_bAdjustXOffset;
	BOOL	m_bDashAlign;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDashLineSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDashLineSetting)
	afx_msg void OnChangeLinetypenameEdit();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnChangePlacetypeEdit();
	afx_msg void OnChangeBaseoffEdit();
	afx_msg void OnPreviewButton();
	afx_msg void OnSelchangePlacetypeCombo();
	afx_msg void OnSelchangeIndenttypeCombo();
	afx_msg void OnSelchangeUseselfcolorCombo();
	afx_msg void OnColorEdit();
	afx_msg void OnChangeIndentEdit();
	afx_msg void OnChangeXOffsetEdit();
	afx_msg void OnCheckAdjustXOffset();
	afx_msg void OnCheckDashAlign();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	void SetMem(CDashLinetype *pCell, ConfigLibItem config);

	ConfigLibItem m_config;
 	CDashLinetype *m_pDashLine;
	int m_nPlaceType;
	int  m_nIndentType;

	DWORD	m_dColor;
	
	BOOL m_bUseSelfcolor;

// 	CBaseLineTypeLib *m_pBaseLib;
// 	DWORD m_dScale;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDASHLINESETTING_H__12F4AF31_AF60_40E6_87E7_DEA181777F5A__INCLUDED_)
