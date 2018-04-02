#if !defined(AFX_DLGSCALE_H__F1EEE627_19C0_4CFC_BE7E_741A936A841B__INCLUDED_)
#define AFX_DLGSCALE_H__F1EEE627_19C0_4CFC_BE7E_741A936A841B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgScale.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgScale dialog

class CDlgScale : public CDialog
{
// Construction
public:
	CDlgScale(CWnd* pParent = NULL);   // standard constructor
	UINT GetScale() {  return m_nScale;  };
// Dialog Data
	//{{AFX_DATA(CDlgScale)
	enum { IDD = IDD_SCALE_DIALOG };
	UINT	m_nScale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScale)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScale)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALE_H__F1EEE627_19C0_4CFC_BE7E_741A936A841B__INCLUDED_)
