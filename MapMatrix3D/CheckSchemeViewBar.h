// CheckSchemeViewBar.h: interface for the CCheckSchemeViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKSCHEMEVIEWBAR_H__E3048D57_521B_42D3_8671_24816BD973E1__INCLUDED_)
#define AFX_CHECKSCHEMEVIEWBAR_H__E3048D57_521B_42D3_8671_24816BD973E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
class CTreeCtrlWithTip:public CTreeCtrl
{
public:
	CTreeCtrlWithTip();
	~CTreeCtrlWithTip();
	void ClearSelection();
	int GetSelectCount(){return m_selitems.GetSize();}
protected:
	INT_PTR OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	DECLARE_MESSAGE_MAP()
	static	std::wstring m_sstrTipinfo;
	CArray<HTREEITEM,HTREEITEM> m_selitems;
	HTREEITEM m_current_item;
};
class CCheckScheme;
class CCheckSchemeViewBar : public CDockablePane  
{
public:
	CCheckSchemeViewBar();
	~CCheckSchemeViewBar();
	void RefreshViewBar(int mode);

	BOOL RefreshSchemeView();
	void ClearData();
	BOOL IsItemSelected(LPCTSTR pGroup, LPCTSTR pItem);
private:
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)	
	{		
		CDockablePane::OnUpdateCmdUI ( (CFrameWnd*)GetOwner(), FALSE);
	}
	static BOOL bInit;	
	static CString oldScheme;

	CStatic   m_staticText;
	CComboBox m_comBobox;
	CTreeCtrlWithTip  m_treeCtrl;

	CImageList	m_ImageList;
	CButton m_NewScheme;
	CButton m_DelScheme;
//	CFont m_font;
	CCheckScheme *m_pCheckScheme;
	CStringArray  m_arrStrSchemes;//方案名
	int m_nCurScheme;		//当前选中列表项索引
	CString m_strSchemeDir;	//方案文件路径

	BOOL m_bActive;
protected:

	void AdjustLayout();
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNewScheme();
	afx_msg void OnDelScheme();
	afx_msg void OnSelchangeScheme();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnNewChkGroup();
	afx_msg void OnNewChkItem();
	afx_msg void OnDelChkGroup();
	afx_msg void OnDelChkItem();
	afx_msg void OnRenameChkGroup();
	afx_msg void OnRenameChkItem();	
	afx_msg void OnCustomizeChkItem();
	afx_msg void OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnExecuteChkItem();
	afx_msg void OnExecuteChkItemSelection();
	afx_msg void OnExecuteChkGroup();
	afx_msg void OnChkImportScript();
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CHECKSCHEMEVIEWBAR_H__E3048D57_521B_42D3_8671_24816BD973E1__INCLUDED_)
