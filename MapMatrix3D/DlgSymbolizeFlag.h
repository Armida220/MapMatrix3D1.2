#if !defined(AFX_DLGSYMBOLIZEFLAG_H__46A288D0_A56C_42AE_8CAD_D212430B0A67__INCLUDED_)
#define AFX_DLGSYMBOLIZEFLAG_H__46A288D0_A56C_42AE_8CAD_D212430B0A67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSymbolizeFlag.h : header file
//

#include "RadioListBox.h"

struct FlagItem
{
	FlagItem(){}
	FlagItem(LPCTSTR n, int d, int b){
		name = n;
		data = d;
		isbit = b;
	}
	FlagItem(const FlagItem& a){
		name = a.name;
		data = a.data;
		isbit = a.isbit;
	}
	FlagItem& operator=(const FlagItem& a){
		name = a.name;
		data = a.data;
		isbit = a.isbit;
		return *this;
	}
	CString name;
	int data;
	int isbit;
};

/////////////////////////////////////////////////////////////////////////////
// CDlgSymbolizeFlag dialog

class CDlgSymbolizeFlag : public CDialog
{
// Construction
public:
	CDlgSymbolizeFlag(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSymbolizeFlag)
	enum { IDD = IDD_SYMBOLIZEFLAG };
	CCheckListBox	m_wndList1;
	CRadioListBox	m_wndList2;
	//}}AFX_DATA

	void AutoAddFlagItems();

	void AddFlagItem(LPCTSTR name, int data, int isbit);
	
	void SetSymbolizeFlag(long flag);
	long GetSymbolizeFlag();
	CString GetDisplayText();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSymbolizeFlag)
	protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	long m_nSymbolizeFlag, m_nSymbolizeFlag_old;
	CArray<FlagItem,FlagItem> m_arrItems;

	// Generated message map functions
	//{{AFX_MSG(CDlgSymbolizeFlag)
	afx_msg void OnSelchangeList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSYMBOLIZEFLAG_H__46A288D0_A56C_42AE_8CAD_D212430B0A67__INCLUDED_)
