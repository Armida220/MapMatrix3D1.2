#if !defined(AFX_DLGSCALETURNPLATESETTING_H__5035F505_3010_4511_9847_B823B255855E__INCLUDED_)
#define AFX_DLGSCALETURNPLATESETTING_H__5035F505_3010_4511_9847_B823B255855E__INCLUDED_

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

class CDlgScaleTurnplateSetting : public CDialog
{
// Construction
public:
	CDlgScaleTurnplateSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgScaleTurnplateSetting)
	enum { IDD = IDD_SCALETURNPLATE_SETTING };
	float	m_fRoadWid;
	CString	m_strName;
	float	m_fWidth;
	BOOL	m_bSingleLine;
	float	m_fStickupLen;
	float	m_fSideLineWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScaleTurnplateSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScaleTurnplateSetting)
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeLenEdit();
	afx_msg void OnChangeLinetypenameEdit();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnCheckSingleLine();
	afx_msg void OnChangeStickupLen();
	afx_msg void OnChangeSideLineEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void SetMem(CScaleTurnplatetype *pScale, ConfigLibItem m_config);
	
	ConfigLibItem m_config;
	CScaleTurnplatetype *m_pScale;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALETURNPLATESETTING_H__5035F505_3010_4511_9847_B823B255855E__INCLUDED_)
