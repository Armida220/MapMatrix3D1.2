#if !defined(AFX_TREECTRLEX_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_)
#define AFX_TREECTRLEX_H__0517A828_D18F_11D3_A727_009027900694__INCLUDED_



#define TVGN_EX_ALL			0x000F

/////////////////////////////////////////////////////////////////////////////
// 
#ifndef TVIS_FOCUSED
#define TVIS_FOCUSED	1
#else
#if TVIS_FOCUSED != 1
#error TVIS_FOCUSED was assumed to be 1
#endif
#endif

typedef CTypedPtrArray<CPtrList, HTREEITEM> CTreeItemList;

class CTreeCtrlEx : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlEx)

// Construction
public:

	CTreeCtrlEx() : m_bSelectPending(FALSE), m_hClickedItem(NULL), m_hFirstSelectedItem(NULL), m_bSelectionComplete(TRUE), m_bEditLabelPending(FALSE),m_bLButtonDown(FALSE),m_bRealMulti(FALSE) {}
	BOOL Create(DWORD dwStyle, DWORD dwExStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	
	HTREEITEM InsertRoot(LPCTSTR lpszItem,int type,DWORD_PTR dwData=0,HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertSubItem(LPCTSTR lpszItem, HTREEITEM hParent, int type, DWORD_PTR dwData = 0, HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM FindItemByData(DWORD_PTR dwData, HTREEITEM hParent = NULL);
// Attributes

public:
	UINT GetSelectedCount() const;
	void GetSelectedList(CTreeItemList& list) const;
	
	HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode);
	HTREEITEM GetFirstSelectedItem() const;
	HTREEITEM GetNextSelectedItem(HTREEITEM hItem)const; 
	HTREEITEM GetPrevSelectedItem(HTREEITEM hItem);
	HTREEITEM ItemFromData(DWORD_PTR dwData, HTREEITEM hStartAtItem = NULL) const;

	BOOL SelectItemEx(HTREEITEM hItem, BOOL bSelect=TRUE);

	BOOL SelectItems(HTREEITEM hFromItem, HTREEITEM hToItem);
	void ClearSelection(BOOL bMultiOnly=FALSE);
	
	BOOL m_bLButtonDown;
	BOOL m_bRealMulti;//hcw,2012.3.16,实时是否为多选。
protected:
	virtual int ImageIndexOfType(int type);
	void SelectMultiple( HTREEITEM hClickedItem, UINT nFlags, CPoint point );

private:
	BOOL		m_bSelectPending;
	CPoint		m_ptClick;
	HTREEITEM	m_hClickedItem;
	HTREEITEM	m_hFirstSelectedItem;
	BOOL		m_bSelectionComplete;
	BOOL		m_bEditLabelPending;
	UINT		m_idTimer;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeCtrlEx() {}

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeCtrlEx)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


HTREEITEM GetTreeItemFromData(CTreeCtrl& treeCtrl, DWORD_PTR dwData, HTREEITEM hStartAtItem=NULL);

#endif
