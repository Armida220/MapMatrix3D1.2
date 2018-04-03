#if !defined(AFX_DLGCHKCONFIG_H__143BB7E2_FDBB_4F32_BFB1_A8405892C75D__INCLUDED_)
#define AFX_DLGCHKCONFIG_H__143BB7E2_FDBB_4F32_BFB1_A8405892C75D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgchkconfig.h : header file
//
#include "UIFPropListExPlus.h  "
#include "CheckScheme.h "

/////////////////////////////////////////////////////////////////////////////
// CDlgChkConfig dialog
class CDlgDoc;
class CChkCmd;
class CDlgChkConfig :  public CDialog
{
// Construction
public:
	CDlgChkConfig(CWnd* pParent = NULL);   // standard constructor
	~CDlgChkConfig();
// Dialog Data
	//{{AFX_DATA(CDlgChkConfig)
	enum { IDD = IDD_CHECK };
	CListBox	m_listChkConfig;
	CTreeCtrl	m_treeChkConfig;
	//}}AFX_DATA
	void RefreshParams(CChkCmd* pCmd);//hcw,2013.1.6,更新检查项的参数设置。
	void SetCurGroupAndItem(LPCTSTR GroupName,LPCTSTR ItemName,BOOL bNew = TRUE){
		m_strCurGroupName = GroupName;
		m_strCurItemName = ItemName;
		m_bNew = bNew;
	}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChkConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
	CUIFPropListExPlus		m_wndPropList;
	CDlgDoc *     m_pDoc;
	CCheckScheme *m_pCheckScheme;
	CArray<CCheckTask,CCheckTask&> *m_pCurTaskList;
	CString m_strCurGroupName;
	CString m_strCurItemName;
	CArray<CChkCmd*,CChkCmd*> m_arrChkCmd;//参数定制的检查命令，与任务列表一一对应
	CChkCmd *m_pCurCmd;
	CUIntArray  m_arrSaveflag;//存储0（还没保存） 1（已经保存）
	BOOL  m_bNew;
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgChkConfig)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	virtual void OnOK();
	afx_msg void OnSelchangeListChkConfig();
	afx_msg void OnButtonSave();
	afx_msg void OnDblclkTreeChkConfig(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	//这个函数仅仅是为了达到让用户可以控制隐藏Prop的目的
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


//执行检查项时弹出该对话框
class CDlgChkConfig_One :  public CDialog
{
// Construction
public:
	CDlgChkConfig_One(CWnd* pParent = NULL);   // standard constructor
	~CDlgChkConfig_One();
// Dialog Data
	//{{AFX_DATA(CDlgChkConfig)
	enum { IDD = IDD_CHECK_ONE };
	CListBox	m_listChkConfig;
	//}}AFX_DATA
	void RefreshParams(CChkCmd* pCmd);//hcw,2013.1.6,更新检查项的参数设置。
	void SetCurGroupAndItem(LPCTSTR GroupName,LPCTSTR ItemName){
		m_strCurGroupName = GroupName;
		m_strCurItemName = ItemName;
	}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChkConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
	CUIFPropListExPlus		m_wndPropList;
	CDlgDoc *     m_pDoc;
	CCheckScheme *m_pCheckScheme;
	CArray<CCheckTask,CCheckTask&> *m_pCurTaskList;
	CString m_strCurGroupName;
	CString m_strCurItemName;
	CArray<CChkCmd*,CChkCmd*> m_arrChkCmd;//参数定制的检查命令，与任务列表一一对应
	CChkCmd *m_pCurCmd;
	CUIntArray  m_arrSaveflag;//存储0（还没保存） 1（已经保存）
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgChkConfig)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeListChkConfig();
	afx_msg void OnButtonSave();
	//}}AFX_MSG
	//这个函数仅仅是为了达到让用户可以控制隐藏Prop的目的
	afx_msg LRESULT OnPropertyChanged (WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHKCONFIG_H__143BB7E2_FDBB_4F32_BFB1_A8405892C75D__INCLUDED_)
