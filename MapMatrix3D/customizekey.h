#if !defined(AFX_CUSTOMIZEKEY_H__D1A66FB0_29AD_41EC_834B_5C17173A3CC7__INCLUDED_)
#define AFX_CUSTOMIZEKEY_H__D1A66FB0_29AD_41EC_834B_5C17173A3CC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomizeKey.h : header file
//

#include "GridCtrl_src/GridCtrl.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomizeKey dialog

struct CustomizeKey
{
	CString Name;
	CString Accel;
	CString Cskey;//自定义加速键
};

typedef CArray<CustomizeKey, CustomizeKey> CustomizeKeyArray;

class CCustomizeKey : public CDialog
{
// Construction
public:
	CCustomizeKey(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomizeKey)
	enum { IDD = IDD_CUSTOMIZE_KEY };
	CGridCtrl m_gridCtrl;
	//}}AFX_DATA

public:
	CString m_keyfilepath;
	CString m_CurCommandName;
	CString m_CurCommandAccel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomizeKey)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadCustomizeKey();
	void SaveCustomizeKey();
	// Generated message map functions
	//{{AFX_MSG(CCustomizeKey)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMIZEKEY_H__D1A66FB0_29AD_41EC_834B_5C17173A3CC7__INCLUDED_)
