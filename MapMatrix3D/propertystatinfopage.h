#if !defined(AFX_PROPERTYSTATINFOPAGE_H__13237A89_05AB_4BB6_AA3A_881F3D5BA327__INCLUDED_)
#define AFX_PROPERTYSTATINFOPAGE_H__13237A89_05AB_4BB6_AA3A_881F3D5BA327__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// propertystatinfopage.h : header file
//
#include "editbasedoc.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyStatInfoPage dialog

class CPropertyStatInfoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropertyStatInfoPage)

// Construction
public:
	CPropertyStatInfoPage();
	CPropertyStatInfoPage(CDlgDoc *pDoc);
	~CPropertyStatInfoPage();

// Dialog Data
	//{{AFX_DATA(CPropertyStatInfoPage)
	enum { IDD = IDD_STATINFO_PROPPAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyStatInfoPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyStatInfoPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CDlgDoc *m_pDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYSTATINFOPAGE_H__13237A89_05AB_4BB6_AA3A_881F3D5BA327__INCLUDED_)
