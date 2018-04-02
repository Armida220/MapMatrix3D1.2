#if !defined(AFX_DLGNEWLAYER_H__B5ADEA85_75DB_4A3A_91C4_BBC29AD4C942__INCLUDED_)
#define AFX_DLGNEWLAYER_H__B5ADEA85_75DB_4A3A_91C4_BBC29AD4C942__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewLayer.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgNewLayer dialog

class CDlgNewLayer : public CDialog
{
// Construction
public:
	CDlgNewLayer(CWnd* pParent = NULL);   // standard constructor
	CString GetLayerName() {  return m_strLayerName;  };
	void SetLayerName(CString tmp);
// Dialog Data
	//{{AFX_DATA(CDlgNewLayer)
	enum { IDD = IDD_NEWLAYER_DIALOG };
	CString	m_strLayerName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewLayer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewLayer)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWLAYER_H__B5ADEA85_75DB_4A3A_91C4_BBC29AD4C942__INCLUDED_)
