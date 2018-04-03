#if !defined(AFX_DLGSCALEOLDDOUYASETTINGS_H__26A1570D_2746_4339_9051_78E13E566980__INCLUDED_)
#define AFX_DLGSCALEOLDDOUYASETTINGS_H__26A1570D_2746_4339_9051_78E13E566980__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgScaleOldDouyaSettings.h : header file
//

#include "Symbol.h"
#include "SymbolLib.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleOldDouyaSettings dialog

class CDlgScaleOldDouyaSettings : public CDialog
{
// Construction
public:
	CDlgScaleOldDouyaSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgScaleOldDouyaSettings)
	enum { IDD = IDD_SCALE_OLDDOUYA };
	float	m_fInterval;
	float	m_fMaxToothLen;
	float	m_fWidth;
	float	m_fCycle;
	//}}AFX_DATA

	CScaleOldDouya *m_pSym;
	ConfigLibItem m_config;

	void SetMem(CScaleOldDouya* pSym, ConfigLibItem config);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScaleOldDouyaSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScaleOldDouyaSettings)
	afx_msg void OnChangeEditCycle();
	afx_msg void OnChangeEditInterval();
	afx_msg void OnChangeEditMaxtoothlen();
	afx_msg void OnChangeWidthEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALEOLDDOUYASETTINGS_H__26A1570D_2746_4339_9051_78E13E566980__INCLUDED_)
