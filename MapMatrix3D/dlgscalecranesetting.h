#if !defined(AFX_DLGSCALECRANESETTING_H__5035F505_3010_4511_9847_B823B255855E__INCLUDED_)
#define AFX_DLGSCALECRANESETTING_H__5035F505_3010_4511_9847_B823B255855E__INCLUDED_

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

class CDlgScaleCraneSetting : public CDialog
{
// Construction
public:
	CDlgScaleCraneSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgScaleCraneSetting)
	enum { IDD = IDD_SCALECRANE_SETTING };
	CString	m_strName;
	float	m_fWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScaleCraneSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScaleCraneSetting)
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeLinetypenameEdit();
	afx_msg void OnChangeWidthEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetMem(CScaleCranetype *pScale, ConfigLibItem m_config);
	
	ConfigLibItem m_config;
	CScaleCranetype *m_pScale;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALECRANESETTING_H__5035F505_3010_4511_9847_B823B255855E__INCLUDED_)
