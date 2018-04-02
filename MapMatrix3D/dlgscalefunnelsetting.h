#if !defined(AFX_DLGSCALEFUNNELSETTING_H__5035F505_3010_4511_9847_B823B255855E__INCLUDED_)
#define AFX_DLGSCALEFUNNELSETTING_H__5035F505_3010_4511_9847_B823B255855E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgscaleturnplatesetting.h : header file
//
#include "Symbol.h"
#include "UIFSimpleEx.h"
#include "SymbolLib.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgScaleTurnplateSetting dialog

class CDlgScaleFunnelSetting : public CDialog
{
// Construction
public:
	CDlgScaleFunnelSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgScaleFunnelSetting)
	enum { IDD = IDD_SCALEFUNNEL_SETTING };
	CString	m_strName;
	float	m_fWidth;
	CComboBox	m_cFunnelType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScaleFunnelSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScaleFunnelSetting)
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeLinetypenameEdit();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnSelchangeFunnelypeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetMem(CScaleFunneltype *pScale, ConfigLibItem m_config);
	
	ConfigLibItem m_config;
	CScaleFunneltype *m_pScale;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALECRANESETTING_H__5035F505_3010_4511_9847_B823B255855E__INCLUDED_)
