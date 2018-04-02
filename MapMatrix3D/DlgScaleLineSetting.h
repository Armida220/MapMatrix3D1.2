#if !defined(AFX_DLGSCALLLINESETTING_H__2117FB70_BDCA_473E_A5CD_25845AE8ED50__INCLUDED_)
#define AFX_DLGSCALLLINESETTING_H__2117FB70_BDCA_473E_A5CD_25845AE8ED50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgScallLineSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleLineSetting dialog

#include "Symbol.h"
#include "UIFSimpleEx.h"
#include "SymbolLib.h"

class CDlgScaleLineSetting : public CSonDialog
{
// Construction
public:
	CDlgScaleLineSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgScaleLineSetting)
	enum { IDD = IDD_SCALELINE_SETTING };
// 	CComboBox	m_cYoffsetType1;
// 	CComboBox	m_cYoffsetType0;
	CComboBox	m_cPlaceType;
	CComboBox   m_cAverageDraw;
	CString	m_strBaseLinetypeName;
	double	m_lfCycle;
	double	m_lfWidth;
	double	m_lfXOffset0;
	double	m_lfXOffset1;
	double	m_lfYOffset0;
	double	m_lfYOffset1;
	double	m_lfYMod0;
	double	m_lfYMod1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScaleLineSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScaleLineSetting)
	afx_msg void OnChangeBaselinetypenameEdit();
	afx_msg void OnChangeCycleEdit();
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnChangeXoffset0Edit();
	afx_msg void OnChangeXoffset1Edit();
	afx_msg void OnChangeYoffset0Edit();
	afx_msg void OnChangeYoffset1Edit();
	afx_msg void OnSelchangePlacetypeCombo();
	afx_msg void OnSelchangeAveragedrawCombo();
// 	afx_msg void OnSelchangeXoffsettype0Combo();
// 	afx_msg void OnSelchangeYoffsettype0Combo();
// 	afx_msg void OnSelchangeXoffsettype1Combo();
// 	afx_msg void OnSelchangeYoffsettype1Combo();
	afx_msg void OnChangeYmod0Edit();
	afx_msg void OnChangeYmod1Edit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	void SetMem(CScaleLinetype *pScale, ConfigLibItem m_config);
	
	ConfigLibItem m_config;
	CScaleLinetype *m_pScale;
	int		m_nPlaceType;
	BOOL    m_bAverageDraw;
// 	CBaseLineTypeLib *m_pBaseLib;
// 	DWORD m_dScale;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALLLINESETTING_H__2117FB70_BDCA_473E_A5CD_25845AE8ED50__INCLUDED_)
