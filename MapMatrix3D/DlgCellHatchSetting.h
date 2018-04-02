#if !defined(AFX_DLGCELLHACTHSETTING_H__E2586253_57E6_401E_894C_64DFFF6006D6__INCLUDED_)
#define AFX_DLGCELLHACTHSETTING_H__E2586253_57E6_401E_894C_64DFFF6006D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCellHacthSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCellHatchSetting dialog
#include "Symbol.h"
#include "SymbolLib.h"
#include "UIFSimpleEx.h"

class CDlgCellHatchSetting : public CSonDialog
{
// Construction
public:
	CDlgCellHatchSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCellHatchSetting)
	enum { IDD = IDD_CELLHATCH_SETTING };
	float	m_fAngle;
	CString	m_strCellDefName;
	float	m_fddx;
	float	m_fddy;
	float	m_fdx;
	float	m_fdy;
	float	m_fkx;
	float	m_fky;
	float	m_fxoff;
	float	m_fyoff;
	float	m_fWidth;
	BOOL	m_bAccurateCutCell;
	BOOL	m_bRandomAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCellHatchSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCellHatchSetting)
	afx_msg void OnChangeAngleEdit();
	afx_msg void OnChangeCelldefnameEdit();
	afx_msg void OnChangeDdxEdit();
	afx_msg void OnChangeDdyEdit();
	afx_msg void OnChangeDxEdit();
	afx_msg void OnChangeDyEdit();
	afx_msg void OnChangeKxEdit();
	afx_msg void OnChangeKyEdit();
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeXoffEdit();
	afx_msg void OnChangeYoffEdit();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnAccurateCutCell();
	afx_msg void OnRandomAngle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	void SetMem(CCellHatch *pCellHatch, ConfigLibItem config);
	
	CCellHatch *m_pCellHatch;
	ConfigLibItem m_config;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCELLHACTHSETTING_H__E2586253_57E6_401E_894C_64DFFF6006D6__INCLUDED_)
