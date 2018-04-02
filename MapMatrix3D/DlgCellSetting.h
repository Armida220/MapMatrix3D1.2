#if !defined(AFX_DLGCELLSETTING_H__C4AC1E2F_9D59_45B5_96AE_B96E7A197A6A__INCLUDED_)
#define AFX_DLGCELLSETTING_H__C4AC1E2F_9D59_45B5_96AE_B96E7A197A6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCellSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCellSetting dialog

#include "Symbol.h"
#include "SymbolLib.h"
#include "UIFSimpleEx.h"

class CDlgCellSetting : public CSonDialog
{
// Construction
public:
	CDlgCellSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCellSetting)
	enum { IDD = IDD_CELL_SETTINGDLG };
	CComboBox	m_cPlaceType;
	float	m_fAngle;
	CString	m_strCellName;
	float	m_fDx;
	float	m_fDy;
	float	m_fKx;
	float	m_fKy;
	float   m_fExtendDis;
	CComboBox		m_cFillType;
	CComboBox       m_cCenLineMode;
	CComboBox       m_cCoverType;
	CComboBox       m_cDirWithFistLine;
	float	m_fWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCellSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCellSetting)
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeCellnaemEdit();
	afx_msg void OnChangeDxEdit();
	afx_msg void OnChangeKxEdit();
	afx_msg void OnChangeAngleEdit();
	afx_msg void OnChangeDyEdit();
	afx_msg void OnChangeKyEdit();
	afx_msg void OnChangeExtendDisEdit();
	afx_msg void OnChangePlacetypeEdit();
	afx_msg void OnSelchangePlacetypeCombo();
	afx_msg void OnSelchangeFilltypeCombo();
	afx_msg void OnSelchangeCenlinemodeCombo();
	afx_msg void OnSelchangeCovertypeCombo();
	afx_msg void OnSelchangeDirWithFistLineCombo();
	afx_msg void OnChangeWidthEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	void SetMem(CCell *pCell, ConfigLibItem config);

private:
	CCell *m_pCell;
	ConfigLibItem m_config;
	int m_nPlaceType;
	int m_nFillType;
	int m_nCenlineMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCELLSETTING_H__C4AC1E2F_9D59_45B5_96AE_B96E7A197A6A__INCLUDED_)
