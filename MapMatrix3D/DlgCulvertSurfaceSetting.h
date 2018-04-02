#if !defined(AFX_DLGCULVERTSURFACESETTING_H__2A6230F3_F6B5_4623_BE42_194D2DAA5E0C__INCLUDED_)
#define AFX_DLGCULVERTSURFACESETTING_H__2A6230F3_F6B5_4623_BE42_194D2DAA5E0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCulvertSurfaceSetting.h : header file
//
#include "Symbol.h"
#include "SymbolLib.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgCulvertSurfaceSetting dialog

class CDlgCulvertSurfaceSetting : public CDialog
{
// Construction
public:
	CDlgCulvertSurfaceSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCulvertSurfaceSetting)
	enum { IDD = IDD_CULVERTSURFACE };
	CString  m_LineType;
	float m_LineWidth;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCulvertSurfaceSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
public:
    
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgCulvertSurfaceSetting)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	afx_msg void OnPreviewButton();
	afx_msg void OnChangeWidthEdit();
	afx_msg void OnChangeLinetypenameEdit();
	DECLARE_MESSAGE_MAP()

public:
	void SetMem(CCulvertSurface1Symbol *pPara, ConfigLibItem config);
	void SetMem(CCulvertSurface2Symbol *pPara, ConfigLibItem config);
	CCulvertSurface1Symbol* m_pPara1;
	CCulvertSurface2Symbol* m_pPara2;
    ConfigLibItem m_config;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCULVERTSURFACESETTING_H__2A6230F3_F6B5_4623_BE42_194D2DAA5E0C__INCLUDED_)
