// FTRLayerViewBar.h: interface for the CFTRLayerViewBar class.
//
//////////////////////////////////////////////////////////////////////
#include "GeometryViewBar.h"
#include "editbasedoc.h"
#include "CheckExProp.h"
#include <map>

#if !defined(AFX_FTRLAYERVIEWBAR_H__8BE054E0_7379_4C3C_B90A_EBBEDCDAEA69__INCLUDED_)
#define AFX_FTRLAYERVIEWBAR_H__8BE054E0_7379_4C3C_B90A_EBBEDCDAEA69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFTRLayerViewBar : public CDockablePane   
{
private:
	struct RootInfo
	{
		HTREEITEM root_item;
		std::map<HTREEITEM,int> item_layerid;
	    std::map<int,HTREEITEM> layerid_item;
		std::map<CString,HTREEITEM> dblayer_item;
	};

	enum SUBTYPE
	{
		GROUP,
		DBLAYER
	};

	enum ItemType
	{
		NONE,
		ROOTFDB,
		ITEMFDB,
		ITEMDBLAYER,
		ITEMGROUP,
		ITEMLAYER,
		ROOTIMG,
		ITEMIMG
	};

	enum ItemPosition
	{
		IP_TOP,
		IP_LAST,
		IP_SORT
	};
public:
	CFTRLayerViewBar();
	virtual ~CFTRLayerViewBar();
public:
    void UpdateLayerView ();
	void Refresh(bool with_datasource = true, bool with_refimg = true);
private:
	void AdjustLayout ();

	HTREEITEM InsertRoot(LPCTSTR lpszItem, DWORD_PTR dwData = 0);
	HTREEITEM InsertSubRoot(LPCTSTR lpszItem, HTREEITEM hParent, DWORD_PTR dwData = 0, ItemPosition p = IP_SORT);
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD_PTR dwData = 0, ItemPosition p = IP_SORT);
	HTREEITEM InsertLayerItem(CString ds_name, int dblayer_type, CFtrLayer* player, HTREEITEM hParent, DWORD_PTR dwData = 0, ItemPosition p = IP_SORT);
	HTREEITEM GetDBLayerItem(CDlgDataSource* pds, CString strDBLayerName);
	HTREEITEM GetRootItemByLabel(LPCTSTR item_label)
	{
       for(HTREEITEM root = m_wndLayerView.GetRootItem(); root != NULL; root = m_wndLayerView.GetNextSiblingItem(root))
	   {
		   if(m_wndLayerView.GetItemText(root) == item_label)
			   return root;
	   }
	   //
	   return NULL;
	}
	HTREEITEM GetSubRootItemByLabel(HTREEITEM root, LPCTSTR item_label)
	{
		for(HTREEITEM sub_root = m_wndLayerView.GetChildItem(root); sub_root != NULL; sub_root = m_wndLayerView.GetNextSiblingItem(sub_root))
		{
			if(m_wndLayerView.GetItemText(sub_root) == item_label)
				return sub_root;
		}
		//
		return NULL;
	}

    ItemType GetItemType(HTREEITEM hitm)
	{
		if(hitm==NULL)
			return CFTRLayerViewBar::ItemType::NONE;
		//
		if(hitm == root_fdb)
			return CFTRLayerViewBar::ItemType::ROOTFDB;
		//
		if(hitm == root_img)
			return CFTRLayerViewBar::ItemType::ROOTIMG;
		//
		HTREEITEM parent_item = m_wndLayerView.GetParentItem(hitm);
		if(parent_item==NULL)
			return CFTRLayerViewBar::ItemType::NONE;
		//
		if(parent_item == root_fdb)
		    return CFTRLayerViewBar::ItemType::ITEMFDB;
		//
		if(parent_item == root_img)
			return CFTRLayerViewBar::ItemType::ITEMIMG;
		//
		parent_item = m_wndLayerView.GetParentItem(parent_item);
		if(parent_item==NULL)
			return CFTRLayerViewBar::ItemType::NONE;
		//
        if(parent_item == root_fdb)
		{
			if(current_subtype == CFTRLayerViewBar::SUBTYPE::DBLAYER)
				return CFTRLayerViewBar::ItemType::ITEMDBLAYER;
			else
				return CFTRLayerViewBar::ItemType::ITEMGROUP;
		}
		//
		if(parent_item == root_img)
			return CFTRLayerViewBar::ItemType::ITEMIMG;
		//
		parent_item = m_wndLayerView.GetParentItem(parent_item);
		if(parent_item==NULL)
			return CFTRLayerViewBar::ItemType::NONE;
		//
		if(parent_item == root_fdb)
			return CFTRLayerViewBar::ItemType::ITEMLAYER;
		//
        return CFTRLayerViewBar::ItemType::NONE;
	}
    
	CString GetItemLabel(CString layer_name, int dblayer_type, int ftr_count);
	void AddDatasourse(CDlgDataSource* pds, BOOL is_actived = TRUE);
	void RefreshDatasourse(CDlgDataSource* pds, BOOL is_actived = TRUE);
	void DeleteFtrLayer(CString ds_name, int layer_id);
	void DeleteLayerGroup(CString ds_name, CString group_name);
	void DeleteDatasourse(CString ds_name);

	void ChangeLayerColor(CFtrLayer* player, COLORREF color, CUndoBatchAction *pUndo);
	void SwitchVisible();

	void AddColorImage(COLORREF color);
	void AddRootImage(int type = 0);
	void AddSubRootImage();
	void clear();
private:
	CClassTreeWndLayer	m_wndLayerView; 
	HTREEITEM root_fdb;
	HTREEITEM root_img;
	CImageList images;
	int img_width, img_height;
	std::vector<CBitmap*> bmps;
	std::map<COLORREF, int> color_img;
	std::map<HTREEITEM,bool> item_check;
	std::map<HTREEITEM,int> item_image;
	std::map<CString,RootInfo*> ds_root;
	std::map<CString,HTREEITEM> refimg_item;
	std::map<HTREEITEM,CString> item_colorzone;
	CString symbol_point, symbol_line, symbol_polygon;
	int current_subtype;
private:
	void DeleteFtrLayer(RootInfo* prootinfo, int layer_id);
protected:
	//{{AFX_MSG(CFTRLayerViewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnTreeLBtnDown(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTreeLBtnDBClk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEditExProp();
	afx_msg void OnSubWithGroup();
	afx_msg void OnSubwithDBLayer();
	afx_msg void OnAddRefImageFromAny();
	afx_msg void OnAddRefImageFromWorkspace();
	afx_msg void OnRemoveRefImage();
	afx_msg void OnAddRefDatasource();
	afx_msg void OnAddRefDatasourceUVS();
	afx_msg void OnRemoveRefDatasource();
	afx_msg void OnActiveDatasource();
	afx_msg void OnSelectObjectsByLayer();
	afx_msg void OnSymbolConfig();
	afx_msg void OnResetColor();
	afx_msg void OnColorLayout();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	afx_msg void OnUpdateManual();
	//afx_msg void 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

protected:


	//virtual void DrawCaption(
	//	CDC* pDC,
	/*	CRect rectCaption){}
*/
protected:
	CBrush m_brush;
};


#endif // !defined(AFX_FTRLAYERVIEWBAR_H__8BE054E0_7379_4C3C_B90A_EBBEDCDAEA69__INCLUDED_)
