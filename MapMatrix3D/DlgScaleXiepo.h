#if !defined(AFX_DLGSCALEXIEPO_H__DCFA0B98_0435_44F7_A649_FA6BE3D71A50__INCLUDED_)
#define AFX_DLGSCALEXIEPO_H__DCFA0B98_0435_44F7_A649_FA6BE3D71A50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgScaleXiepo.h : header file
//

#include "Symbol.h"
#include "SymbolLib.h"
#include "Editbase.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgScaleXiepo dialog

class CDlgScaleXiepo : public CDialog
{
// Construction
public:
	CDlgScaleXiepo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgScaleXiepo)
	enum { IDD = IDD_SCALE_XIEPO };
	BOOL	m_bFasten;
	float	m_fInterval;
	float	m_fPointSize;
	float	m_fPointInterval;
	float	m_fToothLen;
	float	m_fWidth;
	float	m_fBaseYOffset;
	BOOL	m_bAverageDraw;
	//}}AFX_DATA

	CScaleXiepo *m_pSym;
	ConfigLibItem m_config;

	void SetMem(CScaleXiepo *pSym, ConfigLibItem item);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScaleXiepo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScaleXiepo)
	afx_msg void OnCheckFasten();
	afx_msg void OnChangeEditInterval();
	afx_msg void OnChangeEditPointsize();
	afx_msg void OnChangeEditPointinterval();
	afx_msg void OnChangeEditToothlen();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnCheckAverageDraw();
	afx_msg void OnChangeBaseYOffset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALEXIEPO_H__DCFA0B98_0435_44F7_A649_FA6BE3D71A50__INCLUDED_)
