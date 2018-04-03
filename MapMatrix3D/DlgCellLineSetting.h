#if !defined(AFX_DLGCELLLINESETTING_H__7D2EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_)
#define AFX_DLGCELLLINESETTING_H__7D2EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCellLineSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCellLineSetting dialog

#include "Symbol.h"
#include "SymbolLib.h"
#include "UIFSimpleEx.h"

class CDlgCellLineSetting : public CSonDialog
{
// Construction
public:
	CDlgCellLineSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCellLineSetting)
	enum { IDD = IDD_CELLLINE_SETTING };
	CComboBox	m_cFillType;
	CComboBox	m_cPlaceType;
	CString	m_strCellDefName;
	float	m_fAngle;
	float	m_fBaseXOffset;
	float	m_fBaseYOffset;
	float	m_fCycle;
	float	m_fkx;
	float	m_fky;
	float	m_fWidth;
	BOOL	m_bCellAlign;
	BOOL	m_bInsideBaseLine;
	
// 	float	m_fXOffset;
// 	float	m_fYOffset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCellLineSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCellLineSetting)
	afx_msg void OnChangeCelldefnameEdit();
	afx_msg void OnChangeCycleEdit();
	afx_msg void OnChangeKyEdit();
	afx_msg void OnChangeXoffsetEdit();
	afx_msg void OnChangeYoffsetEdit();
	afx_msg void OnChangeAngleEdit();
	afx_msg void OnChangeBasexoffsetEdit();
	afx_msg void OnChangeBaseyoffsetEdit();
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeKxEdit();
	afx_msg void OnSelchangeFilltypeCombo();
	afx_msg void OnSelchangePlacetypeCombo();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnCheckCellAlign();
	afx_msg void OnCheckInsideBaseLine();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetMem(CCellLinetype *pCellline, ConfigLibItem config);

	CCellLinetype *m_pCellLine;
	ConfigLibItem m_config;
	int		m_nPlacePos;
	int     m_nPlaceType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCELLLINESETTING_H__7D2EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_)
