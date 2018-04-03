#if !defined(AFX_DLGWHEELSPEED_H__7EAF2028_C26C_4229_8F44_F11BA27E5D48__INCLUDED_)
#define AFX_DLGWHEELSPEED_H__7EAF2028_C26C_4229_8F44_F11BA27E5D48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWheelSpeed.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWheelSpeed dialog

class CDlgWheelSpeed : public CDialog
{
// Construction
public:
	CDlgWheelSpeed(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWheelSpeed)
	enum { IDD = IDD_WHEELSPEED };
	float	m_fSpeed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWheelSpeed)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWheelSpeed)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWHEELSPEED_H__7EAF2028_C26C_4229_8F44_F11BA27E5D48__INCLUDED_)
