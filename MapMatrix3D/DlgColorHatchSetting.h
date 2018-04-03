#if !defined(AFX_DLGCOLORHATCHSETTING_H__BA010E2D_F9DE_4368_9D1A_98A11D3E4B17__INCLUDED_)
#define AFX_DLGCOLORHATCHSETTING_H__BA010E2D_F9DE_4368_9D1A_98A11D3E4B17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgColorHatchSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgColorHatchSetting dialog
#include "Symbol.h"
#include "UIFSimpleEx.h"

class CDlgColorHatchSetting : public CSonDialog
{
// Construction
public:
	CDlgColorHatchSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgColorHatchSetting)
	enum { IDD = IDD_COLORHATCH_SETTING };
	CMFCColorButton	m_Color;
	CComboBox	m_cUseSefcolor;
	float	m_fTransparence;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColorHatchSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgColorHatchSetting)
	afx_msg void OnChangeTransparenceEdit();
	afx_msg void OnColorEdit();
	afx_msg void OnSelchangeUseselfcolorCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:

	void SetMem(CColorHatch *pColorHatch);

	DWORD	m_dColor;

	BOOL m_bUseSelfcolor;

	CColorHatch *m_pColorHatch;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLORHATCHSETTING_H__BA010E2D_F9DE_4368_9D1A_98A11D3E4B17__INCLUDED_)
