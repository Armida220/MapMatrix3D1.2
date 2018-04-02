#if !defined(AFX_DLGEXTPROPERTIES_H__245A5081_00C7_4E29_9B67_010008AF398F__INCLUDED_)
#define AFX_DLGEXTPROPERTIES_H__245A5081_00C7_4E29_9B67_010008AF398F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExtProperties.h : header file
//
#include "UIFPropListEx.h"
#include "feature.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgExtProperties dialog
class CDlgDoc;
class CSelChangedExchanger;


class CDlgExtProperties : public CDialog
{
// Construction
public:
	CDlgExtProperties(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgExtProperties();

// Dialog Data
	//{{AFX_DATA(CDlgExtProperties)
	enum { IDD = IDD_PROPERTIES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	BOOL Init(CDlgDoc *pDoc, CFeature *pFtr);

	CUIFPropListEx m_wndPropList;
	CDlgDoc *m_pDoc;
	CFeature *m_pFtr;
	CSelChangedExchanger * m_pSelEx;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExtProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExtProperties)
		// NOTE: the ClassWizard will add member functions here
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnPropertyChanged (WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXTPROPERTIES_H__245A5081_00C7_4E29_9B67_010008AF398F__INCLUDED_)
