#if !defined(AFX_DLGLINEFILLSETTING_H__6390DAB6_D31C_4278_BFBE_547DF055FFFE__INCLUDED_)
#define AFX_DLGLINEFILLSETTING_H__6390DAB6_D31C_4278_BFBE_547DF055FFFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLinefillSetting.h : header file
//
#include "Symbol.h"
#include "SymbolLib.h"
#include "UIFSimpleEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgLinefillSetting dialog

class CDlgLinefillSetting : public CSonDialog
{
// Construction
public:
	CDlgLinefillSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgLinefillSetting)
	enum { IDD = IDD_LINEFILL_SETTINGDLG };
	float	m_fAngle;
	float	m_fIntv;
	CString	m_strLineName;
	float	m_fWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLinefillSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLinefillSetting)
	afx_msg void OnChangeLinetypenameEdit();
	afx_msg void OnChangeIntvEdit();
	afx_msg void OnChangeAngleEdit();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnPreviewButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetMem(CLineHatch *pLineHatch, ConfigLibItem config);
	
private:
	CLineHatch *m_pLineHatch;
	ConfigLibItem m_config;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLINEFILLSETTING_H__6390DAB6_D31C_4278_BFBE_547DF055FFFE__INCLUDED_)
