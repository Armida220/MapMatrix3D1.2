#if !defined(AFX_DLGXATTRIBUTES_H__473EF80A_74F2_4B51_84D2_3830336683CB__INCLUDED_)
#define AFX_DLGXATTRIBUTES_H__473EF80A_74F2_4B51_84D2_3830336683CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgxattributes.h : header file
//
#include "LayersMgrDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgXAttributes dialog

class CDlgXAttributes : public CDialog
{
// Construction
public:
	CDlgXAttributes(CWnd* pParent = NULL);   // standard constructor

	~CDlgXAttributes();

// Dialog Data
	//{{AFX_DATA(CDlgXAttributes)
	enum { IDD = IDD_XATTRIBUTES };
	CListCtrl	m_ctlList;
	//}}AFX_DATA

	enum AttributeMode{
		waySet = 0,
		wayGet = 1
	};
	void XAttributes(CMap<XAttributeItem,XAttributeItem&,int,int> &xattibutes, AttributeMode mode);

	void FillList();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgXAttributes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgXAttributes)
	virtual void OnOK();
	afx_msg void OnCancle();
	afx_msg void OnAllcheck();
	afx_msg void OnAllnocheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CMap<XAttributeItem,XAttributeItem&,int,int> m_arrXAttibutes;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGXATTRIBUTES_H__473EF80A_74F2_4B51_84D2_3830336683CB__INCLUDED_)
