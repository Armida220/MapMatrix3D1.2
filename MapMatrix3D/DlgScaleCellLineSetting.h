#if !defined(AFX_DLGSCALECELLLINESETTING_H__1D2EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_)
#define AFX_DLGSCALECELLLINESETTING_H__1D2EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgScaleCellLineSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleCellLineSetting dialog

#include "Symbol.h"
#include "SymbolLib.h"
#include "UIFSimpleEx.h"

class CDlgScaleCellLineSetting : public CSonDialog
{
// Construction
public:
	CDlgScaleCellLineSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgScaleCellLineSetting)
	enum { IDD = IDD_SCALE_CELLLINE_SETTING };
	CString	m_strCellDefName;
	float	m_fBaseXOffset;
	float	m_fBaseYOffset;
	float	m_fCycle;
	float	m_fkx;
	float	m_fWidth;
	float   m_fAssistYOffset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScaleCellLineSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScaleCellLineSetting)
	afx_msg void OnChangeCelldefnameEdit();
	afx_msg void OnChangeCycleEdit();
	afx_msg void OnChangeKyEdit();
	afx_msg void OnChangeAngleEdit();
	afx_msg void OnChangeBasexoffsetEdit();
	afx_msg void OnChangeBaseyoffsetEdit();
	afx_msg void OnChangeAssistyoffsetEdit();
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeKxEdit();
	afx_msg void OnChangeWidthEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetMem(CScaleCellLinetype *pCellline, ConfigLibItem config);

	CScaleCellLinetype *m_pCellLine;
	ConfigLibItem m_config;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALECELLLINESETTING_H__1D2EAA35_E0D7_49DB_B6DB_8F7F334F7CA6__INCLUDED_)
