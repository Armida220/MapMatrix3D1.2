// PropertiesViewBar.h: interface for the CPropertiesViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTIESVIEWBAR_H__E48481A9_DE34_487A_A63D_4D6E0EC8F361__INCLUDED_)
#define AFX_PROPERTIESVIEWBAR_H__E48481A9_DE34_487A_A63D_4D6E0EC8F361__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIFSimpleEx.h"
#include "UIFPropListEx.h "
#include "ValueTable.h "
#include "Feature.h "
#include "EditDef.h "

class CPropertiesViewBar;
//支持单地物或者多地物
class CPermanentExchanger :public CObject
{	
	friend CPropertiesViewBar;
	DECLARE_DYNAMIC(CPermanentExchanger)
public:
	virtual void OnModifyLayer(LPCSTR str);
	virtual void OnModifyLinetype(int code);
	virtual void OnModifyNodeWid(int index, float wid);
	virtual void OnModifyFtrWid(float wid);
	virtual void OnModifyFtrColor(COLORREF col);
	virtual void OnModifyNodeType(int index, short type);
	//单地物方式下
	CPermanentExchanger(CPermanent *pFtr, int index=0);
	//多地物方式下
	CPermanentExchanger(CArray<CPermanent*,CPermanent*>& arrPFtr, int index=0);
	virtual ~CPermanentExchanger();

	virtual CPermanentExchanger *Clone();
	virtual BOOL GetProperties(CValueTable& tab);
	virtual void OnModifyProperties(CValueTable& tab);

	virtual BOOL GetXAttributes(CValueTable& tab, BOOL bAll=FALSE);
	virtual void OnModifyXAttributes(CValueTable& tab);
	virtual int  GetObjectCount();
	virtual CPermanent* GetObject(int index);

	BOOL IsIncludedObject(CPermanent *pFtr);

	int GetCurPtPos() { return m_nPtIndex; }

	virtual void OnModifyTransparency(long transparency);
	virtual void OnModifyFillColor(BOOL bFill, COLORREF col);

protected:
	//多地物方式下，作指针copy，无须释放
	CArray<CPermanent*,CPermanent*> m_arrPFtr;

	// 要定位第一个地物的关键点
	int m_nPtIndex;

// 	单地物方式下，作深copy，需要自己释放
// 		CFeature *m_pFtr;
// 	
// 		int m_nMode;
};


class CPropertiesToolBar : public CMFCToolBar
{
public:	
	
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)	
	{		
		CMFCToolBar::OnUpdateCmdUI ((CFrameWnd*) GetOwner (), bDisableIfNoHndler);
	}
	
	virtual BOOL AllowShowOnList () const		{	return FALSE;	}
	DECLARE_MESSAGE_MAP()
};


class CPropertiesViewBar : public CDockablePane 
{
	// Construction
public:
	
	friend CUIFPropListEx;
	CPropertiesViewBar();
	

	void AdjustLayout ();
	
	void SetPermaExchanger(CPermanentExchanger *pFtrExchanger);
	CPermanentExchanger* GetPermaExchanger() { return m_pFtrExchanger; }
	
	BOOL IsHideInAutoHideMode () const {return TRUE;}
	
	// Attributes
public:
	CUIFPropListEx		m_wndPropList;
	
protected:
	
	CComboBox			m_wndObjectCombo;
	
	CPropertiesToolBar	m_wndToolBar;

	CPermanentExchanger		*m_pFtrExchanger;	

	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	// 要显示的节点索引号
	int m_nPtIndex;

	CValueTable m_Tmptab;
	
	// Attributes
protected:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesViewBar)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	afx_msg LRESULT OnPropertyChanged (WPARAM wParam, LPARAM lParam);
	void ClearPermaExchanger();
	
	virtual ~CPropertiesViewBar();

	// 更新属性到文档
	void UpdateParam(CValueTable& tab);
	// 更新属性
	void UpdateProperty(CValueTable& tab);

	virtual CDockablePane* AttachToTabWnd(CDockablePane* pTabControlBarAttachTo, AFX_DOCK_METHOD dockMethod, BOOL bSetActive = TRUE, CDockablePane** ppTabbedControlBar = NULL);
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertiesViewBar)
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg void OnSortingprop();
	
	afx_msg void OnUpdateSortingprop(CCmdUI* pCmdUI);
	
	afx_msg void OnProperies1();

	afx_msg void OnUpdateProperies1(CCmdUI* pCmdUI);
	
	afx_msg void OnProperies2();
	
	afx_msg void OnUpdateProperies2(CCmdUI* pCmdUI);
	
	afx_msg void OnExpand();
	
	afx_msg void OnUpdateExpand(CCmdUI* pCmdUI);
	
	afx_msg void OnPaint();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	/*
	** brief 颜色刷子
	*/
	CBrush m_brush;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual BOOL CanBeClosed() const { return FALSE; }

	
};

#endif // !defined(AFX_PROPERTIESVIEWBAR_H__E48481A9_DE34_487A_A63D_4D6E0EC8F361__INCLUDED_)
