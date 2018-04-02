#if !defined(AFX_PROPLIST_H__F6054FED_0317_4829_B7BF_4EBBDC27DF01__INCLUDED_)
#define AFX_PROPLIST_H__F6054FED_0317_4829_B7BF_4EBBDC27DF01__INCLUDED_

#if _MSC_VER > 1000
#pragma once

#include "LVLColorButton.h"
#include "resource.h"

#endif // _MSC_VER > 1000
// PropList.h : header file
//

#define WM_UPDATE_FILTER		(WM_USER+1)

// #define  NOCHECK				0
// #define  CHECK					1
// #define  ALL					2		



#define SELNULL			-2

class CLVLPropItem;

struct CLVLPropColumn
{
	enum LVLTYPE
	{
		NONE	= 0,
		CHECK	= 1,
		EDIT	= 2,
		COLOR	= 3,
		COMBO   = 4
		//STYLE,
		//WEIGHT,
	};
	CLVLPropColumn::CLVLPropColumn()
	{
		bFrame			= TRUE;
		BitmapOffset	= 0;
		Type			= EDIT;
		ValueType		= VT_BSTR;
		DefaultColor	= RGB(255,255,255);
		bReadOnly		= FALSE;
		memset(FieldName,0,sizeof(FieldName));
		memset(ColumnName,0,sizeof(ColumnName));
		Rect.SetRectEmpty();
		arrComboValues.RemoveAll();
		bReadOnlyRefItem = FALSE;
	}

	CLVLPropColumn(const CLVLPropColumn &col)
	{
		*this = col;
	}

	CLVLPropColumn& operator=(const CLVLPropColumn &col)
	{
		bFrame			= col.bFrame;
		BitmapOffset	= col.BitmapOffset;
		Type			= col.Type;
		ValueType		= col.ValueType;
		DefaultColor	= col.DefaultColor;
		bReadOnly		= col.bReadOnly;
		memcpy(FieldName,col.FieldName,sizeof(FieldName));
		memcpy(ColumnName,col.ColumnName,sizeof(ColumnName));
		Rect = col.Rect;
		arrComboValues.Copy(col.arrComboValues);
		bReadOnlyRefItem = col.bReadOnlyRefItem;
		return *this;
	}

	char			FieldName[64];		// 字段名称
	char			ColumnName[64];		// 列名称
	short			BitmapOffset;		// 默认图片
	BOOL			bFrame;				// 是否有边框
	BOOL			bReadOnly;			// 是否只读
	COLORREF		DefaultColor;		// 默认颜色
	LVLTYPE			Type;				// 列类型
	int				ValueType;			// 值类型
	CRect			Rect;				// 矩形	
	BOOL			bReadOnlyRefItem;   //与行相关的只读
	CStringArray	arrComboValues;	// COMBO选项
};


/////////////////////////////////////////////////////////////////////////////
// CLVLPropItem object

class  CLVLPropItem : public CObject
{
	DECLARE_DYNAMIC(CLVLPropItem)

	friend class CLVLPropList;
	
public:
	CLVLPropItem();

	virtual ~CLVLPropItem();

	void ShowBitmap(CDC * pDC,CRect rect, int offset);

	BOOL HitTest (CPoint point);
	void Redraw ();
	void SetReadOnly(BOOL bReadOnly){m_bReadOnlyRefCol = bReadOnly;}
	BOOL IsReadOnly()  { return m_bReadOnlyRefCol;}
	void SetColReadOnly(int col, BOOL bReadOnly);
	BOOL IsColReadOnly(int col);
protected:	
	void Init ();
	void SetFlags ();
	void SetOwnerList (CLVLPropList* pWndList);
	void Repos (int x, int& y);
	void RemoveAllValues();

public:
	virtual void OnDrawValue (CDC* pDC);

	virtual BOOL OnUpdateValue (CLVLPropColumn * PC);
	virtual BOOL OnEdit (CLVLPropColumn* Pc);
	virtual BOOL OnColor(CLVLPropColumn* Pc);
	virtual BOOL OnCombo(CLVLPropColumn* Pc);
	virtual BOOL OnEndEdit ();	
	virtual CWnd* CreateInPlaceEdit (CRect rectEdit);
	virtual CLVLColorButton* CreateInColorEdit (CRect rectColor);

	virtual BOOL OnRBClick (CPoint point , BOOL bSelChanged);
	virtual BOOL OnLBClick (CPoint point);
	virtual BOOL OnDblClick (CPoint point);

	virtual BOOL OnSetCursor () const;
	virtual BOOL PushChar (UINT nChar);
	BOOL IsSelected () const;

protected:
	virtual HBRUSH OnCtlColor(CDC* pDC, UINT nCtlColor);
	virtual void   OnDestroyWindow ();

	virtual BOOL OnKillFocus (CWnd* /*pNewWnd*/)
	{
		return TRUE;
	}

	virtual BOOL IsProcessFirstClick () const
	{
		return TRUE;
	}

public:
	void SetColumnCount(int num);

	void SetValue (const _variant_t& varValue,int index);

	_variant_t GetValue (LPCTSTR field)const;

	_variant_t GetValue(int ncol)const
	{
		return *m_PropValues.GetAt(ncol);
	}

	CRect GetRect () const
	{
		return m_Rect;
	}

	BOOL IsInPlaceEditing () const
	{
		return m_bInPlaceEdit;
	}

	void SetData (DWORD_PTR dwData)
	{
		m_dwData = dwData;
	}

	DWORD_PTR GetData() const
	{
		return m_dwData;
	}
	
protected:
	CString								  m_ItemName;			// 行名称
	CRect								  m_RectName;			// 名称矩形
	CRect								  m_Rect;				// 行矩形

	CLVLPropList*						  m_pWndList;			// PropertyList 窗口指针

	BOOL								  m_bInPlaceEdit;		// 编辑窗口是否激活
	BOOL								  m_bInColorSel;		// 颜色选择窗口是否激活
	BOOL								  m_bInComboBox;        // 组合框窗口是否激活
	CWnd*								  m_pWndInPlace;		// 编辑窗口指针
	CLVLColorButton						  m_WndInColor;			// 颜色选择窗口
	CComboBox							  m_WndComboBox;		// 组合框

	DWORD_PTR								  m_dwData;				//附加数据,当是All Levels时,为对应的CFtrLayer对象
																//当是Filter时,就为具体的独立值查询结果,即为_FilterResultItem的指针
	BOOL								  m_bIsInput;			//是否是过滤条件输入框 
	
	

	CArray<_variant_t*,_variant_t*>		  m_PropValues;			// 列属性数组
	CArray<BOOL,BOOL>                     m_arrValuesReadOnly;

public:
	void SetInput(BOOL bInput){m_bIsInput = bInput;}

private:
	BOOL                                  m_bReadOnlyRefCol;   //
};


/////////////////////////////////////////////////////////////////////////////
// CLVLPropList window

#define UIFROPLIST_ID_INPLACE 3

#define CUIFPropList CLVLPropList 

class  CLVLPropList : public CWnd
{
	DECLARE_DYNAMIC(CLVLPropList)

	friend class CLVLPropItem;

public:
	CLVLPropList();

public:
	void SetCurLayerPropItem(CLVLPropItem* pPropItem)
	{
		 m_pCurLayItem = pPropItem;
	}
	CLVLPropItem* GetCurLayerPropItem()
	{
		return m_pCurLayItem;
	}
	CRect GetListRect () const
	{
		return m_rectList; 
	}

	int GetHeaderHeight () const
	{
		return m_nHeaderHeight;
	}

	int	GetRowHeight () const
	{
		return m_nRowHeight;
	}

	virtual CHeaderCtrl& GetHeaderCtrl ()
	{
		return m_wndHeader;
	}

	BOOL IsParentLayerVisible();

protected:
	CMFCHeaderCtrl	m_wndHeader;					// 属性头控件
	
	CScrollBar		m_wndScrollVert;				 // 纵向滚动条
	CScrollBar		m_wndScrollHorz;				 // 水平向滚动条

	int				m_nBorderSize;				 	 // 控件边框尺寸
	
	int				m_nHeaderHeight;			 	 // 头控件高度

	int				m_nRowHeight;					 // 单行高度

	int				m_nVertScrollOffset;	
	int				m_nVertScrollTotal;
	int				m_nVertScrollPage;

	int				m_nColWidth;					 // 单列平均宽度

	int				m_nHorzScrollOffset;	
	int				m_nHorzScrollTotal;
	int				m_nHorzScrollPage;

	CRect			m_rectList;						 // 列表窗口客户区矩形

	BOOL			m_bFocused;						 // 控件是否有焦点

	CMenu *         m_pPopupMenu;					 // 菜单弹出控件
	
	CArray<CLVLPropColumn,CLVLPropColumn>	m_arrColumns; //列信息
	CList<CLVLPropItem*, CLVLPropItem*>		m_lstAllProps; //所有的行数据
	CList<CLVLPropItem*, CLVLPropItem*>		m_lstProps;	// 过滤后的行数据
	CList<CLVLPropItem*, CLVLPropItem*>		m_lstPSels;	// 选中的行数据
	CLVLPropItem*	m_pInEditItem;					// 当前编辑项
	CLVLPropItem*	m_pFilterItem;					// 过滤器项
	int				m_nSelColumn;					// 当前选择列
	int				m_nSelHeadColumn;					// 当前选择头列

	CLVLPropItem*	m_pCurLayItem;					// 当前层所对应的项


	// 跟踪属性: 用于滚动条或头控件的变化跟踪
	CRect			m_rectTrackHeader;
	BOOL			m_bTracking;

	// 上一次的鼠标左键位置，用于多选
	CPoint			m_ptLastLBD;

	_variant_t		m_vValueBeforeCheck;
	BOOL			m_bMulSel;  //是否为多选

	//用于画属性值位图的BITMAP  -- Add by Liwq
	CBitmap			m_bmProp;
	BITMAP			m_bmPropInfo;

	// 用户是否忽略了颜色选择值，此时禁止多选（补丁）
	BOOL            m_bIgnoreColSel;

public:	
	virtual void AdjustLayout ();
	virtual void OnDrawList (CDC* pDC);

	int  AddColumn(CLVLPropColumn PropColumn)
	{
		return m_arrColumns.Add(PropColumn);
	}
	void  InsertColumn(int index, CLVLPropColumn PropColumn)
	{
		m_arrColumns.InsertAt(index, PropColumn);
	}
	void DeleteColumn(int index)
	{
		m_arrColumns.RemoveAt(index);
	}

	CLVLPropColumn GetColumn(int iIndex)const
	{
		return m_arrColumns.GetAt(iIndex);
	}
	void SetColumn(int idx, CLVLPropColumn col)
	{
		m_arrColumns.SetAt(idx,col);
	}

	int GetColumnIndexByName(LPCTSTR name) const
	{
		int nsz = m_arrColumns.GetSize();
		for( int i=0; i<nsz; i++)
		{
			if( stricmp(m_arrColumns.GetAt(i).ColumnName,name)==0 )
				return i;
		}
		return -1;
	}

	int GetColumnIndexByField(LPCTSTR field) const
	{
		int nsz = m_arrColumns.GetSize();
		for( int i=0; i<nsz; i++)
		{
			if( stricmp(m_arrColumns.GetAt(i).FieldName,field)==0 )
				return i;
		}
		return -1;
	}
	
	int  GetColumnCount() const
	{
		return m_arrColumns.GetSize();
	}

	int GetSelColumnIndex()const
	{
		return m_nSelColumn;
	}

	int GetItemCount () const
	{
		return m_lstProps.GetCount ();
	}

	CLVLPropItem *GetPropItem(int idx) const
	{
		return m_lstProps.GetAt(m_lstProps.FindIndex(idx));
	}

	int GetSelectedCount() const
	{
		return m_lstPSels.GetCount();
	}

	BOOL IsInSelection(const CLVLPropItem *pItem) const
	{
		return m_lstPSels.Find((CLVLPropItem*)pItem)!=NULL;
	}

	CLVLPropItem* GetSelectedItem (int idx) const
	{
		return m_lstPSels.GetAt(m_lstPSels.FindIndex(idx));
	}

	void SetFilterItem(CLVLPropItem *pItem);
	
	//返回过滤器项
	CLVLPropItem *GetFilterItem()const
	{
		return m_pFilterItem;
	}

	void FilterPropItems(BOOL bRedraw=TRUE);

	void SelectItem (const CLVLPropItem* pProp, BOOL bOnlyOne = TRUE, BOOL bRedraw = TRUE);
	void DeselectItem(const CLVLPropItem* pProp, BOOL bRedraw = TRUE);
	void SelectAll(BOOL bRedraw = TRUE);
	void DeselectAll(BOOL bRedraw = TRUE);
	
	void RemovePropItem(CLVLPropItem *pItem);
	
	void SetSelColumn( int idx);
	
	void RemoveAll();

	int  GetTotalItems () const;
	int  GetTotalColWid() const;
	void ReposProperties ();

	void InitHeaderCtrl();

	void ReSortItems(int ncol);

	void SortItems(int ncol, BOOL bAscend);

protected:
	virtual void Init ();
	CLVLPropItem* HitTest (CPoint pt) const;

	int HitColumn(CPoint pt) const;
	
	

	void EnsureVisible (CLVLPropItem* pProp);

	virtual void OnItemChanged (CLVLPropItem* pProp) const;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLVLPropList)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	virtual void OnChangeSelection (CLVLPropItem* /*pNewSel*/, CLVLPropItem* /*pOldSel*/) {}

	virtual BOOL EditItem  (CLVLPropItem* pProp,CLVLPropColumn* Pc);
//	virtual void OnRBClick (CPoint point);
	virtual BOOL EndEditItem (BOOL bUpdateData = TRUE);

	virtual int  OnDrawProperty (CDC* pDC, CLVLPropItem* pProp) const;

// Implementation
public:
	virtual ~CLVLPropList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLVLPropList)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelAll();
	afx_msg void OnUnSelAll();
	//}}AFX_MSG
	afx_msg void OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderEndTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderClick (NMHDR* pNMHDR , LRESULT* pResult);
	afx_msg void OnColorChange();
	afx_msg void OnComboChange();
	DECLARE_MESSAGE_MAP()

	//------------------
	// Internal helpres:
	//------------------
	void TrackHeader (int nOffset,NMHEADER* pHeader = NULL);

	void SetScrollSizes ();
public:

	//插入一个用于输入过滤条件的行
	BOOL InsertInputRow();

	//插入一个ITEM
	BOOL AddItem(CLVLPropItem* pItem);

	//在PropList的编辑完成后,触发修改函数
	BOOL UpdatePropItem(CLVLPropItem *pItem, CLVLPropColumn* Pc,_variant_t exData = false);

	//得到头控件实际需要的宽度
	int  GetHeaderCtrlActualWidth();

	//选择属性字段
	BOOL OnSelFeaField();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UIFPROPLIST_H__F6054FED_0317_4829_B7BF_4EBBDC27DF01__INCLUDED_)
