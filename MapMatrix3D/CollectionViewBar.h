// CollectionViewBar.h: interface for the CCollectionViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLECTIONVIEWBAR_H__B42256E2_743F_4472_B69C_695CA6E57004__INCLUDED_)
#define AFX_COLLECTIONVIEWBAR_H__B42256E2_743F_4472_B69C_695CA6E57004__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClassTreeWnd.h"
#include "GeometryViewBar.h"
#include "SymbolLib.h"
#include "CollectionTreeCtrl.h"
#include "../mm3dPrj/MyCodeEdit.h"
#include "../mm3dPrj/MyMFCButton.h"
#include "../mm3dPrj/MyListBox.h"

void GDI_DrawGrBuffer2d(HDC hdc, const GrBuffer2d *pBuf, BOOL bUseClr, COLORREF clr, double m[9], CSize szDC, CRect rcView, COLORREF backCol=RGB(255,255,255));



class CCollectionViewBar : public CDockablePane  
{
public:
	void DockRecent();
	void PopupforEditCode();
	//##ModelId=41466B7F00CC
	CCollectionViewBar();
	//##ModelId=41466B7F00CD
	virtual ~CCollectionViewBar();
	
	//##ModelId=41466B7F00DA
	void AdjustLayout ();
	void FillCollectionView (int flag, ConfigLibItem *pData);
	CArray<HTREEITEM,HTREEITEM&> m_aRootItem;
	CClassTreeWndCollection	m_wndCollectionView;

	void OnSetFocusEdit();
	
//	CDlgDataSource *m_pData;

//	ConfigLibItem *m_pData;

	ConfigLibItem m_config;
	USERIDX m_UserIdx;	
protected:
	//##ModelId=41466B7F00DC
	CClassToolBar			m_wndToolBar;
	//##ModelId=41466B7F00EF
	CImageList				m_CollectionViewImages;
	
	//##ModelId=41466B7F00F8
	UINT					m_nCurrSort;
	
	HTREEITEM InsertRoot(LPCTSTR lpszItem, DWORD_PTR dwData = 0);
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD_PTR dwData = 0);
	
	void SetItemImage(HTREEITEM item, DWORD_PTR flag);
	
private:
	void CreateImageList(int cx, int cy, int nsize);
	BOOL CreateImageItem(int idx);
	void DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf);
	BOOL SetLayerByUseridx(int idx);
	BOOL IsValid();
	
	CArray<int,int> m_arrIdxCreateFlag;
	int m_nIdxToCreateImage, m_nImageWid, m_nImageHei;
	
	BOOL  m_bClosedBeforePopup, m_bAutoHideBeforePopup;
	BOOL  m_bChgFromSelect, m_bCanSelectDefault;
	CMyCodeEdit m_wndCodeEdit;
	CMyMFCButton m_wndLayerPreview;

//	USERIDX m_UserIdx;
	USERIDX m_UserRencent;

	CString m_strCurNameAndCode;

	CMyListBox	m_wndSearchRsltList;
	BOOL m_bUpDown;
	BOOL m_bReturn;
	BOOL m_bCommonUse;//常用
	CStringArray m_arrCommonLayers;//常用图层
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCollectionViewBar)
public:
	//##ModelId=41466B7F0109
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CCollectionViewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnChangeEditFcode();
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLayerEdit(UINT id);
	afx_msg void OnUpdateLayerEdit(CCmdUI* pCmdUI);
	afx_msg void OnLayerEdit2(UINT id);
	afx_msg void OnUpdateLayerEdit2(CCmdUI* pCmdUI);
	afx_msg void OnCreateAnnot();
	afx_msg void OnSymbolConfig();
	afx_msg void OnPreview();
	afx_msg void OnListSelChanged();
	afx_msg void OnNcPaint();
	afx_msg void OnModifyLayer();
	afx_msg void OnModifyLayer1();
	afx_msg void OnSwitchCommon();
	afx_msg void OnAddToCommon();
	afx_msg void OnDeleteFromCommon();
	//}}AFX_MSG
	//##ModelId=41466B7F0187
	//##ModelId=41466B7F0196
	afx_msg void OnSort (UINT id);
	//##ModelId=41466B7F0199
	afx_msg void OnUpdateSort (CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

protected:
	/*
	** brief 颜色刷子
	*/
	CBrush m_brush;
};

#endif // !defined(AFX_COLLECTIONVIEWBAR_H__B42256E2_743F_4472_B69C_695CA6E57004__INCLUDED_)
