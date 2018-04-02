// GeometryViewBar.h: interface for the CGeometryViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMETRYVIEWBAR_H__EBA9C0A9_8CAA_4C42_BE37_EA6EB647DC9F__INCLUDED_)
#define AFX_GEOMETRYVIEWBAR_H__EBA9C0A9_8CAA_4C42_BE37_EA6EB647DC9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClassTreeWnd.h"


//##ModelId=41466B7F009B
class CClassToolBar : public CMFCToolBar
{
	//##ModelId=41466B7F009D
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)	
	{		
		CMFCToolBar::OnUpdateCmdUI ((CFrameWnd*) GetOwner (), bDisableIfNoHndler);
	}
	
	//##ModelId=41466B7F00AC
	virtual BOOL AllowShowOnList () const		{	return FALSE;	}

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	virtual void DoPaint(CDC* pDCPaint);

protected:
	CBrush m_brush;

	
};

//##ModelId=41466B7F00CA
class CGeometryViewBar : public CDockablePane
{
public:
	//##ModelId=41466B7F00CC
	CGeometryViewBar();
	//##ModelId=41466B7F00CD
	virtual ~CGeometryViewBar();
// 	
// 	//##ModelId=41466B7F00DA
// 	void AdjustLayout ();
// 	
// protected:
// 	//##ModelId=41466B7F00DC
// 	CClassToolBar			m_wndToolBar;
// 	//##ModelId=41466B7F00EA
// 	CClassTreeWndGeometry	m_wndGeometryView;
// 	//##ModelId=41466B7F00EF
// 	CImageList				m_GeometryViewImages;
// 	
// 	//##ModelId=41466B7F00F8
// 	UINT					m_nCurrSort;
// 	
// 	HTREEITEM InsertRoot(LPCTSTR lpszItem,DWORD dwData=0);
// 	HTREEITEM InsertSubRoot(LPCTSTR lpszItem,HTREEITEM hParent,DWORD dwData=0,HTREEITEM hInsertAfter=TVI_LAST);
// 	HTREEITEM InsertItem(LPCTSTR lpszItem,HTREEITEM hParent,DWORD dwData=0);
// 	
// 	void SetItemImage(HTREEITEM item, DWORD flag);
// 	
// public:
// 	void FillGeometryView (CGeoData *pData);
// 	
// private:
// 	CGeoData *m_pData;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeometryViewBar)
public:
	//##ModelId=41466B7F0109
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CGeometryViewBar)
// 	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
// 	afx_msg void OnSize(UINT nType, int cx, int cy);
// 	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
// 	afx_msg void OnPaint();
// 	afx_msg void OnLayerEdit(UINT id);
// 	afx_msg void OnUpdateLayerEdit(CCmdUI* pCmdUI);
// 	afx_msg void OnCreateAnnot();
// 	afx_msg LRESULT OnChangeActiveTab (WPARAM, LPARAM);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};



#endif // !defined(AFX_GEOMETRYVIEWBAR_H__EBA9C0A9_8CAA_4C42_BE37_EA6EB647DC9F__INCLUDED_)
