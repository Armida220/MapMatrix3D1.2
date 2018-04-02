#if !defined(AFX_DLGTIDALWATERSETTING_H__1D0EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_)
#define AFX_DLGTIDALWATERSETTING_H__1D0EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTidalWaterSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTidalWaterSetting dialog

#include "Symbol.h"
#include "SymbolLib.h"
#include "UIFSimpleEx.h"

class CDlgTidalWaterSetting : public CSonDialog
{
// Construction
public:
	CDlgTidalWaterSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTidalWaterSetting)
	enum { IDD = IDD_TIDALWATER_SETTINGDLG };
	CString	m_strCellDefName;
	float	m_fkxMax;
	float	m_fkxMin;
	float	m_fInterval;
	float	m_fWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTidalWaterSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTidalWaterSetting)
	afx_msg void OnChangeCelldefnameEdit();
	afx_msg void OnChangeKxmax();
	afx_msg void OnChangeKxmin();
	afx_msg void OnChangeInterval();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnPreviewButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetMem(CTidalWaterSymbol *pCellline, ConfigLibItem config);

	CTidalWaterSymbol *m_pSymbol;
	ConfigLibItem m_config;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTIDALWATERSETTING_H__1D2EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_)
