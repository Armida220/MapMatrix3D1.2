#if !defined(AFX_DLGSETSPECIALSYMBOL_H__8E1DFCA4_183C_46B7_B7BE_E59733C5D0F0__INCLUDED_)
#define AFX_DLGSETSPECIALSYMBOL_H__8E1DFCA4_183C_46B7_B7BE_E59733C5D0F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetSpecialSymbol.h : header file
//
#include "CollectionTreeCtrl.h"
#include "Scheme.h"
#include "SymbolLib.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSetSpecialSymbol dialog

class CDlgSetSpecialSymbol : public CDialog
{
// Construction
public:
	BOOL FillTree();
	BOOL CreatCtrls();
	void ClearCtrls();
	CDlgSetSpecialSymbol(CWnd* pParent = NULL);   // standard constructor
	CString GetSymbolName();
	void GetCurAttribute(CValueTable &tab);
// Dialog Data
	//{{AFX_DATA(CDlgSetSpecialSymbol)
	enum { IDD = IDD_SETSPECIALSYMBOL_DIALOG };
	CString    m_StrSelect;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetSpecialSymbol)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	void DrawPreview(int nLayerIndex);
// Implementation
protected:


	int m_nCurSchemeScale;	// 当前文档比例尺
	CCollectionTreeCtrl	m_wndIdxTree;
	CString m_SpecialSymbolName;
	int m_nTreeIndex;//当前树节点索引
	ConfigLibItem m_config;
	CScheme *pScheme;
	USERIDX m_BackupUserIdx;
	USERIDX m_UserIdx;
	int m_nLayerIndex;     // 当前选中层在CScheme中的下标
	HTREEITEM m_hCurItem;
	BOOL m_bMultiSelected;
	BOOL m_bAbsentSchemePath;
	BOOL m_bSpecialLayer;
	CString m_StrLayName;//树控件当前被选中的层码

	// Generated message map functions
	//{{AFX_MSG(CDlgSetSpecialSymbol)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETSPECIALSYMBOL_H__8E1DFCA4_183C_46B7_B7BE_E59733C5D0F0__INCLUDED_)
