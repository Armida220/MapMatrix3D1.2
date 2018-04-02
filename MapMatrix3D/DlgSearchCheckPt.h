// EditplusListBoxDlg.h : header file
//

#if !defined(AFX_EDITPLUSLISTBOXDLG_H__EAE9DAC5_860B_4BDB_B8EF_C539B586223A__INCLUDED_)
#define AFX_EDITPLUSLISTBOXDLG_H__EAE9DAC5_860B_4BDB_B8EF_C539B586223A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Author:hcw,2013.1.14
//#include "Resource.h"
#include "resource.h"
#include "DlgChkPtsList.h"

extern HWND g_hwnd;
#define WM_MYDBLCLK 1024+100
/////////////////////////////////////////////////////////////////////////////
// CEditplusListBoxDlg dialog

class CSearchCheckPtDlg : public CDialog
{
	DECLARE_DYNAMIC(CSearchCheckPtDlg);
// Construction
public:
	CSearchCheckPtDlg(CWnd* pParent = NULL);	// standard constructor
	~CSearchCheckPtDlg();
	LRESULT MyDbClkMsg(WPARAM wParam,LPARAM lParam);
// Dialog Data
	//{{AFX_DATA(CEditplusListBoxDlg)
	enum { IDD = IDD_SEARCHCHKPT_DIALOG };
	CEdit	m_editCtrlKeyText;
	//}}AFX_DATA
	void GetStrArray(CString str, CStringArray& strArraySrc, CStringArray& strArrayResult); //获取搜索结果
	void AddStrArraytoList(CStringArray& strArrayResult, CListBox& listBoxCtrl); 
	void AddStrArraytoList(CStringArray& strArrayResult, CDlgChkPtsList* pDlgChkPtsList); //重载
	void InitDlgMems(CStringArray& strArray);
	void UpDateEditCtrlKeyTxt();
	CString GetEditTxt();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditplusListBoxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg); 
	//}}AFX_VIRTUAL

// Implementation
public:
	CStringArray m_strArray;
	CListBox m_wndListSearchResult;
	BOOL m_bPoppedoutListSearchResult;
	BOOL m_bHasSelectedItem;
	CString m_strCurTxtforSearch;
	CDlgChkPtsList *m_pDlgChkPtsList;
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEditplusListBoxDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeEditKeytext();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void RepositionDlgPtsList();//重新定位下拉框。
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITPLUSLISTBOXDLG_H__EAE9DAC5_860B_4BDB_B8EF_C539B586223A__INCLUDED_)
