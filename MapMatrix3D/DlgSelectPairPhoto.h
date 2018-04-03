#if !defined(AFX_DLGSELECTPAIRPHOTO_H__E1A37F80_C515_4A2D_A58D_515BAD8568A0__INCLUDED_)
#define AFX_DLGSELECTPAIRPHOTO_H__E1A37F80_C515_4A2D_A58D_515BAD8568A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectPairPhoto.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectPairPhoto dialog

class CDlgSelectPairPhoto : public CDialog
{
// Construction
public:
	CDlgSelectPairPhoto(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelectPairPhoto)
	enum { IDD = IDD_SELECTTWOIMAGE };
	CString	m_strLeftImage;
	CString	m_strRightImage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectPairPhoto)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectPairPhoto)
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonBrowse4();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTPAIRPHOTO_H__E1A37F80_C515_4A2D_A58D_515BAD8568A0__INCLUDED_)
