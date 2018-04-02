// LayersMgrDlg.h : header file
//

#if !defined(AFX_LAYERSMGRDLG_H__D2BC6C36_576F_436F_BCBE_BFEAACFBA6E7__INCLUDED_)
#define AFX_LAYERSMGRDLG_H__D2BC6C36_576F_436F_BCBE_BFEAACFBA6E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLayersMgrDlg dialog

#include <afxcoll.h>
#include "resource.h"
#include "treectrl.h"
#include "PropList.h"
#include "EditBaseDoc.h"

#define SPLITTER_WIDTH				3

#define LM_TREEID_FILE				1
#define LM_TREEID_ALLLEVELS			-1
#define LM_TREEID_ALLFEATURES		-2
#define LM_TREEID_GROUPS			-3
#define LM_TREEID_FILTERS			-4
#define LM_TREEID_FILTERITEM		-5
#define LM_TREEID_FILE_REF			-6
#define LM_TREEID_GROUPS_REF		-7	
#define LM_TREEID_ALLLEVELS_REF		-8
#define LM_TREEID_ALLFEATURES_REF	-9
#define LM_TREEID_FILTERS_REF		-10
#define LM_TREEID_FILTERITEM_REF	-11

#define XMLTAG_FILTER_FILE			_T("FilterFile")
#define XMLTAG_FILTER_ITEM			_T("FilterItem")
#define XMLTAG_FILTER_NAME			_T("Name")
#define XMLTAG_FILTER_DATA			_T("FilterData")

#define FIELDNAME_LAYERDEL			_T("Delete")

//过滤器结构体
struct FilterItem
{
	FilterItem(){}
	~FilterItem(){}
	CValueTable tab;
	CString name;
};
/*
struct LayerGroup
{
	LayerGroup() {}
	LayerGroup(const LayerGroup& group);
	LayerGroup& operator=(const LayerGroup& group);
	void AddFtrLayer(CFtrLayer *pLayer);
	void DelFtrLayer(CFtrLayer *pLayer);
	CString name;
	long color;
	BOOL bVisible;
	BOOL bSymbolized;
	CArray<CFtrLayer*,CFtrLayer*> arr;
};
*/
class CValueTableEx : public CValueTable
{
public:
	CValueTableEx& operator=(CValueTableEx &tab)
	{
		CopyFrom(tab);
		return *this;
	}
};

struct XAttributeItem
{
	XAttributeItem()
	{
		valuetype = DP_CFT_NULL;
	};
	XAttributeItem(CString f, CString n, int v):field(f),name(n),valuetype(v) {};
	XAttributeItem(const XAttributeItem& item)
	{
		field = item.field;
		name = item.name;
		value = item.value;
		valuetype = item.valuetype;
	}
	XAttributeItem& operator=(const XAttributeItem &item)
	{
		if (this == &item) return *this;
		field = item.field;
		name = item.name;
		value = item.value;
		valuetype = item.valuetype;
		return *this;
	}

	bool operator==(const XAttributeItem &x) const
	{
		return (field.CompareNoCase(x.field) == 0 && valuetype == x.valuetype && value.CompareNoCase(x.value) == 0);
	}

	bool operator!=(const XAttributeItem &x) const
	{
		return (field.CompareNoCase(x.field) != 0 || valuetype != x.valuetype || value.CompareNoCase(x.value) != 0);
	}

	//operator DWORD_PTR() { return(DWORD_PTR)this; }

//private:
	CString field;
	CString name;
	int  valuetype;
	CString value;

};

template<>
UINT AFXAPI HashKey<XAttributeItem&> (XAttributeItem &key);

struct FeatureItem
{
	FeatureItem() {}
	FeatureItem(const FeatureItem& item)
	{
		pFtrLayer = item.pFtrLayer;
		layerCode = item.layerCode;
		color = item.color;
		code = item.code;
		bVisible = item.bVisible;
		geotype = item.geotype;
		dbLayerName = item.dbLayerName;
		ftrs.Copy(item.ftrs);
		arrXAttibutes.Copy(item.arrXAttibutes);
	}
	FeatureItem& operator=(const FeatureItem& item)
	{
		pFtrLayer = item.pFtrLayer;
		layerCode = item.layerCode;
		color = item.color;
		code = item.code;
		bVisible = item.bVisible;
		geotype = item.geotype;
		dbLayerName = item.dbLayerName;
		ftrs.Copy(item.ftrs);
		arrXAttibutes.Copy(item.arrXAttibutes);
		return *this;
	}
	CArray<CFeature*,CFeature*> ftrs;
	// 基本属性
	CFtrLayer *pFtrLayer;
	__int64  layerCode;
	long	color;
	CString code;  //标识码	
	BOOL bVisible;
	int geotype;//几何体类型
	CString dbLayerName; //入库层名
	// 扩展属性
	CArray<XAttributeItem,XAttributeItem&> arrXAttibutes;
};

BOOL Variant2XAttribute(const CVariantEx &variant, XAttributeItem &item);
CVariantEx XAttribute2Variant(const XAttributeItem &item);
BOOL ConvertXAttributeItemAndValueTab(CArray<XAttributeItem,XAttributeItem> &arrXAttibutes,CValueTable &tab,int mode);
CString XAttriColName(const XAttributeItem &item);

enum ManageType
{
	ManageLayer = 0,
	ManageLayerVisible = 1,
	ManageFtr = 2
};

class CManageBaseDlg : public CDialog
{
public:	
	CManageBaseDlg( UINT nIDTemplate, CWnd* pParentWnd = NULL );
	virtual void ModifyCheckState(CLVLPropItem **pItems, int num, CLVLPropColumn* pCol, int col, _variant_t value) {}
	virtual BOOL CanModifyCheckState() { return FALSE; }	
	virtual void ModifyAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc ,_variant_t exData) {}
	virtual DWORD_PTR GetCurSelItem() {return -1;}
	virtual void RButtonDown(UINT nFlags, CPoint point) {}
	virtual int GetManageType() = 0;
	virtual void SaveFilter(LPCTSTR name) {}
	virtual BOOL IsValidNewValue(const char* field, _variant_t value) { return TRUE; }
	virtual void UpdateStatusString(){}
protected:
	int  m_nSortColumn;
	BOOL m_bSortAscending;
};

class CLayersMgrDlg : public CManageBaseDlg
{
// Construction
public:
	CLayersMgrDlg(CWnd* pParent = NULL);	// standard constructor
	virtual void RButtonDown(UINT nFlags, CPoint point);

	void SetDoc(CDlgDoc *pDoc);

	virtual int GetManageType() { return ManageLayer; } 
	virtual DWORD_PTR GetCurSelItem() { return m_dCurSelItem; }
	virtual BOOL IsValidNewValue(const char* field, _variant_t value);
// Dialog Data
	//{{AFX_DATA(CLayersMgrDlg)
	enum { IDD = IDD_LAYERSMGR_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayersMgrDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	HICON m_hIcon;

	virtual void UpdateStatusString();

	// Generated message map functions
	//{{AFX_MSG(CLayersMgrDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnNewFilter();
	afx_msg void OnUpdateNewLayer(CCmdUI *pCmdUI);
	afx_msg void OnRenameFilter();
	afx_msg void OnDeleteFilter();
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewLayer();
	afx_msg void OnToTop();
	afx_msg void OnToBottom();
	afx_msg void OnConvertLayer();
	afx_msg void OnNewLayerGroup();
	afx_msg void OnDelLayerGroup();
	afx_msg void OnUpdateDelLayer(CCmdUI *pCmdUI);
	afx_msg void OnDelLayer();
	afx_msg void OnAddLayerScheme();
	afx_msg void OnDelLayerScheme();
	afx_msg void OnShowTop();
	afx_msg void OnShowBottom();
	afx_msg void OnShowMove();
	afx_msg void OnShowInsert();
	afx_msg void OnSelectFtrs();
	afx_msg void OnDeSelectFtrs();
	//}}AFX_MSG
	afx_msg void OnUpdateSelectAllLayers(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDeSelectLayers(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDelLayerObj(CCmdUI *pCmdUI);
	afx_msg void OnUpdateActiveLayer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDeleteFilter(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRenameFilter(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu); 
	afx_msg void OnDelLayerObj();
	afx_msg void OnActiveLayer();
	afx_msg void OnSelectAllLayers();
	afx_msg void OnSelectAllLayersExceptCurSels();
	afx_msg void OnDeSelectLayers();
	afx_msg void OnCutSelectLayers();
	afx_msg void OnUpdateCutSelectLayers(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSelectAllLayersExceptCurSels(CCmdUI *pCmdUI);
	afx_msg void OnPasteLayer();
	afx_msg void OnUpdatePasteLayer(CCmdUI *pCmdUI);
	afx_msg void OnPasteLayerEx();
	afx_msg void OnUpdatePasteLayerEx(CCmdUI *pCmdUI);
	afx_msg void OnExit();
	afx_msg void OnResetDisplayOrder();
	afx_msg void OnResetDefalutLayerColor();
	afx_msg void OnLoadXAttributes();
	DECLARE_MESSAGE_MAP()

private:
	BOOL CreateToolbar(CMFCToolBar& toolbar, UINT ID);
	void AdjustLayout();

	//初始化各个控件
	BOOL InitCtrls();

	//填充树
	void FillTree();

	//用全部层填充列表,参数为空指针时，默认为主数据源
	void FillAllLayersList(CDlgDataSource *pDS = NULL,BOOL bForLoad = FALSE);

	//用全部层组名填充列表,参数为空指针时，默认为主数据源
	void FillAllGroups(CDlgDataSource *pDS = NULL);
	//用带过滤器的层填充列表,pDS为空指针时默认主数据源
	void FillFilterLayersList(LPCTSTR filter ,CDlgDataSource *pDS = NULL, BOOL bOnlyUsedLayers=FALSE);

	//用全部地物填充列表,参数为空指针时，默认为主数据源
	void FillAllFeatures(LPCTSTR filter, CDlgDataSource *pDS = NULL);

	//保存所有层信息
	void SaveLayers();

	//删除过滤器
	void RemoveFilter(LPCTSTR name);

	//装载和保存过滤器
	void LoadFilterFile(const char *szFileName);
	void SaveFilterFile(const char *szFileName);
	
	//表示鼠标的光标变为用作切分窗口的光标
	BOOL MouseCursorIsSplitter(CPoint pt);

	FilterItem* FindFilterItem(LPCTSTR name);

	int GetCol(CString field);
	void UpdateOrderValue();
	void UpdateColorValue();

	BOOL LoadXAttributes(CDlgDataSource *pDS);
	BOOL LoadUniqueXAttributesDefine();
public:
	//保存过滤器数据
	void SaveFilter(LPCTSTR name);

	BOOL IsFtrscolorByLayer(LONG_PTR dWord);
	BOOL IsFtrsManage();

	virtual void ModifyAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc ,_variant_t exData);
	virtual void ModifyCheckState(CLVLPropItem **pItems, int num, CLVLPropColumn* pCol, int col, _variant_t value);
	virtual BOOL CanModifyCheckState();

	// 对象管理
	void ModifyFtrsAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc);
	void OnDelFtrs();

	void ModifyFtrLayerGroupAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc);
	//过滤层管理树
	CLVLTreeCtrl	m_wndTreeCtrl;		//过滤层树
	CLVLPropList	m_wndPropListCtrl;	//过滤层列表
	DWORD_PTR			m_dCurSelItem;

//	static int CreateLayerID();
private:
	BOOL			m_bIsCuttingObj;
	CArray<CFtrLayer*,CFtrLayer*>		m_arrCutLayers;
	CMFCToolBar	m_wndToolBarFilter;       //过滤器工具条
	CImageList		m_ImageList;
	CStatic			m_wndStatus;

	BOOL			m_bMouseInSplitter;			//鼠标在Splitter中
	BOOL			m_bTrackSplitter;			//鼠标正在做拖动操作		
	
	int				m_iTreeCtrlWidth;			//当前树控件的宽度

	int				m_iMinTreeCtrlWidth;		//树控件最小宽度
	int				m_iMinListCtrlWidth;		//列表控件最小宽度

	CDlgDoc*		m_pDlgDoc;
	CArray<FilterItem*,FilterItem*> m_arrFilters;	//过滤项

	CFtrLayer *m_pMovedLayer;

	//CArray<LayerGroup,LayerGroup&> m_arrLayerGroups;

	CArray<FeatureItem,FeatureItem&> m_arrFtrItems;

	// 扩展属性界面显示
	CMap<XAttributeItem,XAttributeItem&,int,int> m_arrXAttibutes;

	// 扩展属性内存值
	CMap<CFeature*,CFeature*,CValueTableEx,CValueTableEx&> m_arrMemXAttr;

// public:
// 	static BOOL		m_bWarningAll;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERSMGRDLG_H__D2BC6C36_576F_436F_BCBE_BFEAACFBA6E7__INCLUDED_)
