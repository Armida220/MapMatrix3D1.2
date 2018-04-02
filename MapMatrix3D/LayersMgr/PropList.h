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

	char			FieldName[64];		// �ֶ�����
	char			ColumnName[64];		// ������
	short			BitmapOffset;		// Ĭ��ͼƬ
	BOOL			bFrame;				// �Ƿ��б߿�
	BOOL			bReadOnly;			// �Ƿ�ֻ��
	COLORREF		DefaultColor;		// Ĭ����ɫ
	LVLTYPE			Type;				// ������
	int				ValueType;			// ֵ����
	CRect			Rect;				// ����	
	BOOL			bReadOnlyRefItem;   //������ص�ֻ��
	CStringArray	arrComboValues;	// COMBOѡ��
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
	CString								  m_ItemName;			// ������
	CRect								  m_RectName;			// ���ƾ���
	CRect								  m_Rect;				// �о���

	CLVLPropList*						  m_pWndList;			// PropertyList ����ָ��

	BOOL								  m_bInPlaceEdit;		// �༭�����Ƿ񼤻�
	BOOL								  m_bInColorSel;		// ��ɫѡ�񴰿��Ƿ񼤻�
	BOOL								  m_bInComboBox;        // ��Ͽ򴰿��Ƿ񼤻�
	CWnd*								  m_pWndInPlace;		// �༭����ָ��
	CLVLColorButton						  m_WndInColor;			// ��ɫѡ�񴰿�
	CComboBox							  m_WndComboBox;		// ��Ͽ�

	DWORD_PTR								  m_dwData;				//��������,����All Levelsʱ,Ϊ��Ӧ��CFtrLayer����
																//����Filterʱ,��Ϊ����Ķ���ֵ��ѯ���,��Ϊ_FilterResultItem��ָ��
	BOOL								  m_bIsInput;			//�Ƿ��ǹ������������ 
	
	

	CArray<_variant_t*,_variant_t*>		  m_PropValues;			// ����������
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
	CMFCHeaderCtrl	m_wndHeader;					// ����ͷ�ؼ�
	
	CScrollBar		m_wndScrollVert;				 // ���������
	CScrollBar		m_wndScrollHorz;				 // ˮƽ�������

	int				m_nBorderSize;				 	 // �ؼ��߿�ߴ�
	
	int				m_nHeaderHeight;			 	 // ͷ�ؼ��߶�

	int				m_nRowHeight;					 // ���и߶�

	int				m_nVertScrollOffset;	
	int				m_nVertScrollTotal;
	int				m_nVertScrollPage;

	int				m_nColWidth;					 // ����ƽ�����

	int				m_nHorzScrollOffset;	
	int				m_nHorzScrollTotal;
	int				m_nHorzScrollPage;

	CRect			m_rectList;						 // �б��ڿͻ�������

	BOOL			m_bFocused;						 // �ؼ��Ƿ��н���

	CMenu *         m_pPopupMenu;					 // �˵������ؼ�
	
	CArray<CLVLPropColumn,CLVLPropColumn>	m_arrColumns; //����Ϣ
	CList<CLVLPropItem*, CLVLPropItem*>		m_lstAllProps; //���е�������
	CList<CLVLPropItem*, CLVLPropItem*>		m_lstProps;	// ���˺��������
	CList<CLVLPropItem*, CLVLPropItem*>		m_lstPSels;	// ѡ�е�������
	CLVLPropItem*	m_pInEditItem;					// ��ǰ�༭��
	CLVLPropItem*	m_pFilterItem;					// ��������
	int				m_nSelColumn;					// ��ǰѡ����
	int				m_nSelHeadColumn;					// ��ǰѡ��ͷ��

	CLVLPropItem*	m_pCurLayItem;					// ��ǰ������Ӧ����


	// ��������: ���ڹ�������ͷ�ؼ��ı仯����
	CRect			m_rectTrackHeader;
	BOOL			m_bTracking;

	// ��һ�ε�������λ�ã����ڶ�ѡ
	CPoint			m_ptLastLBD;

	_variant_t		m_vValueBeforeCheck;
	BOOL			m_bMulSel;  //�Ƿ�Ϊ��ѡ

	//���ڻ�����ֵλͼ��BITMAP  -- Add by Liwq
	CBitmap			m_bmProp;
	BITMAP			m_bmPropInfo;

	// �û��Ƿ��������ɫѡ��ֵ����ʱ��ֹ��ѡ��������
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
	
	//���ع�������
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

	//����һ���������������������
	BOOL InsertInputRow();

	//����һ��ITEM
	BOOL AddItem(CLVLPropItem* pItem);

	//��PropList�ı༭��ɺ�,�����޸ĺ���
	BOOL UpdatePropItem(CLVLPropItem *pItem, CLVLPropColumn* Pc,_variant_t exData = false);

	//�õ�ͷ�ؼ�ʵ����Ҫ�Ŀ��
	int  GetHeaderCtrlActualWidth();

	//ѡ�������ֶ�
	BOOL OnSelFeaField();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UIFPROPLIST_H__F6054FED_0317_4829_B7BF_4EBBDC27DF01__INCLUDED_)
