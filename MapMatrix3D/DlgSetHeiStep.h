#if !defined(AFX_DLGSETHEISTEP_H__41E68D59_4AD3_4B24_B1E3_94AAE4A3D424__INCLUDED_)
#define AFX_DLGSETHEISTEP_H__41E68D59_4AD3_4B24_B1E3_94AAE4A3D424__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetHeiStep.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetHeiStep dialog

class CDlgSetHeiStep : public CDialog
{
	// Construction
public:
	CDlgSetHeiStep(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgSetHeiStep)
	enum { IDD = IDD_SET_HEISTEP };
	float	m_fHeiStep;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetHeiStep)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgSetHeiStep)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETHEISTEP_H__41E68D59_4AD3_4B24_B1E3_94AAE4A3D424__INCLUDED_)
