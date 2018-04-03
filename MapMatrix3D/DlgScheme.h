#if !defined(AFX_DLGSCHEME_H__57B34998_467A_466E_AB5C_9DF7A3A043C6__INCLUDED_)
#define AFX_DLGSCHEME_H__57B34998_467A_466E_AB5C_9DF7A3A043C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgScheme.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgScheme dialog

#include "DlgSchemerAttr.h"
#include "ComboListCtrl.h"
#include "SymbolLib.h"
#include "CollectionTreeCtrl.h"
#include <map>

#include <functional>
#include <algorithm>//hcw,2012.4.5
using namespace std;
#define EPSILON 0.0001

class CDlgNewLayer;
class CDlgCellSetting;
class CDlgDashLineSetting;
class CDlgCellLineSetting;
class CDlgColorHatchSetting;
class CDlgCellHatchSetting;
class CDlgAnnotationSetting;
class CDlgScaleLineSetting;
class CDlgLinefillSetting;
class CDlgDiagonalSetting;
class CDlgParalineSetting;
class CDlgAngbisectorSetting;
class CDlgScaleArcSetting;
class CDlgScaleTurnplateSetting;
class CDlgScaleCraneSetting;
class CDlgScaleFunnelSetting;
class CDlgScaleCellLineSetting;
class CDlgProcSymLTSetting;
class CDlgScaleCellLineSetting;
class CDlgScaleChuanSongDaiSetting;
class CDlgScaleCellSetting;
class CDlgScaleOldDouyaSettings;
class CDlgScaleXiepo;
class CDlgDoc;
class CDlgScaleYouGuanSetting;
class CDlgTidalWaterSetting;
class CDlgConditionColorHatch;
class CDlgCulvertSurfaceSetting;


typedef CTypedPtrArray<CPtrList, HTREEITEM> CTreeItemList;
class CDlgScheme : public CDialog
{
// Construction
public:
	struct ModifyLayerColor 
	{
		CString name;
		COLORREF color;
	};
public:
	CDlgScheme(CWnd* pParent = NULL);   // standard constructor
	~CDlgScheme();

// Dialog Data
	//{{AFX_DATA(CDlgScheme)
	enum { IDD = IDD_SCHEME_DIALOG };
	CMFCColorButton	m_Color;
	CMFCButton	m_diagonalButton;
	CMFCButton	m_linefillButton;
	CListCtrl	m_wndListSymbol;
	CComboBox	m_defaultgeoCombo;
	CComboBox	m_dbgeoCombo;
	CComboBox	m_scaleCombo;
	CMFCButton	m_delsymbolButton;
	CMFCButton	m_colorhatchButton;
	CMFCButton	m_scalelineButton;
	CMFCButton	m_annButton;
	CMFCButton	m_cellhatchButton;
	CMFCButton	m_celllineButton;
	CMFCButton	m_dashlineButton;
	CMFCButton	m_cellButton;
	CString	m_strSupportGeoName;
	CString m_strDBLayName;
	CSliderCtrl m_wndSizeCtrl;
	//}}AFX_DATA

	float m_fPreviewSize;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScheme)
	protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

//	BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );

	virtual LRESULT DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam );


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScheme)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChangeEditFcode();
	afx_msg void OnSelchangeScaleCombo();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelItem(NMHDR* pNMHDR, LRESULT* pResult);//hcw,2012.3.15
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewGroup();
	afx_msg void OnNewLayer();
	afx_msg void OnDelGroup();
	afx_msg void OnDelLayer();
	afx_msg void OnNewScheme();
	afx_msg void OnSetSchemePath();
	afx_msg void OnSaveScheme();
	afx_msg LRESULT OnSchemeModify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSymbolRepaint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAddAttButton();
	afx_msg void OnDelAttButton();
	afx_msg void OnPasteAttButton();
	afx_msg void OnCopyAttButton();
	afx_msg void OnUpAttButton();
	afx_msg void OnDownAttButton();
	afx_msg void OnCopyToOtherLayer();
	afx_msg LRESULT OnEndLabelEditVariableCriteria(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT PopulateComboList(WPARAM wParam, LPARAM lParam);
	afx_msg void OnColorStatic();
	afx_msg void OnSelchangeDefaultgeoCombo();
	afx_msg void OnSelchangeDbgeoCombo();
	afx_msg void OnCellButton();
	afx_msg void OnDashlineButton();
	afx_msg void OnCelllineButton();
	afx_msg void OnCellhatchButton();
	afx_msg void OnAnnotationButton();
	afx_msg void OnColorhatchButton();
	afx_msg void OnScalelineButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelchangeSymbolList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnDeletesymbolButton();
	afx_msg void OnClickStatic();
	afx_msg void OnMoveGroup();
	afx_msg void OnMoveToGroup();
	afx_msg void OnMoveLayer();
	afx_msg void OnMoveToLayer();
	afx_msg void OnCopySymbols();
	afx_msg void OnPasteSymbols();
	afx_msg void OnMovetoTop();
	afx_msg void OnMovetoTail();
	afx_msg void OnResetDefaultcommand();
	afx_msg void OnResetLayerPlaceLine();
	afx_msg void OnResetLayerPlaceArea();
	afx_msg void OnResetAlldefaultcommand();
	afx_msg void OnResetAllLayerDefaultValues();
	afx_msg void OnLinefillButton();
	afx_msg void OnDiagonalButton();
	afx_msg void OnSpecialSymbolsButton();
	afx_msg void OnResetAllLayerOrder();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSortIdcode(); //hcw,2012.2.20,按索引进行排序。
	afx_msg void OnClearLinetype();//hcw,2012.2.21,清理线型名。
	afx_msg void OnListSelChanged();//hcw,2012.2.27,选中搜索结果中的一项。
	afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnImportLayDefFromShp();
	afx_msg void OnImportLayDefFromMdb();
	afx_msg void OnImportLayDefFromMdbRep();
	afx_msg void OnChangeDBLayName();
	afx_msg void OnExportCodeTable_Symbols();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

public:	
	BOOL IsModify();
	BOOL UpdateCodeOrNameOfCurScale();
	// 当选择当前活动文档的比例尺时，禁用删除相关操作
	void ForBidDel();
	static int CreateNewScale(int scale);
	void SetSelectedLayerIndex(CString strNameAndCode);
	BOOL CreateCtrls();
	BOOL InitMems(ConfigLibItem &config, int nCurDocScale=0);

	BOOL FillScaleCombo();
	BOOL FillTree();
	BOOL FillAttList(int nLyaerIndex);
	BOOL FillColor();
	BOOL FillGeo();
	void FillSymbolList();
	void CreateImageList(int cx, int cy, int nsize);
	BOOL CreateImageItem(int idx);
	void DrawPreview(int nLayerIndex);
	void DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf);
	// type为0时先擦去先前画的，为1时直接画 或 擦去原来
	void DrawTemLine(CPoint pt=CPoint(0,0), int type=1);

	void DefToBuf(CellDef *pDef, GrBuffer2d *buf);

	void ClearCtrls();
	void ReleaseDlgMemory();

	void OnSupportgeonameEdit();

	void GetCurAttribute(CValueTable &tab);


	//获取不重复的层名
	CString GetCheckedLayerName(CDlgNewLayer &layer, BOOL bShowCode=TRUE);
//	CString GetTemplate();
	BOOL isInRect(UINT uID, CPoint pt); //hcw,2012.2.13,点是否在资源区域内。
	BOOL CompareLineTypeCntxt(BaseLineType LType1, BaseLineType LType2);//hcw,2012.3.2,比较两个线型内容是否完全相同。
	BOOL hasValidZeroinLineTypeName(CString lineTypeName);//hcw,2012.2.22
	BOOL IsValidDefine(CString lineTypeName);//hcw,2012.2.22
	BOOL hasMatchedSep(CString lineTypeName, CString strSep);//hcw,2012.2.22
	BOOL hasLineType(CArray<CSymbol*, CSymbol*>& symbolsArray);//hcw,2012.2.26,判断层中的符号是否有线型。

	void ClearInvalidZeroinLineTypeName(CString& LineTypeName); //hcw, 2012.2.22, 清理无效的零
	void ClearSingleCtxt(CString& lineTypeCtxt);//hcw,2012.2.22.
	CString RetrieveSeqSeps(CString lineTypeName);//hcw,2012.2.22获取连接符序列,
												//eg:0.00X0.01*0.2 ->X*
	CString CombineSepsandCtxts(CString seqSeps, CStringArray& ctxtArray);//hcw,2012.2.22
	void substitudeLineNameLayer(map<CString, CString> lineTypeNameMap, //hcw,2012.2.26
										 CArray<CSymbol*,CSymbol*>& symbolsArray);//连接连接符和内容
	void ModifiedLineTypeinLayerDef(map<CString, CString> lineTypeNameMap, ConfigLibItem& config);//hcw,2012.2.26,对照修改各LayDefine中的想关线型名
	void SetTreeNodeToTop(CString strLayerNameorID);//hcw,2012.2.27
	BOOL HasInvalidZeroinLineTypeCnTxt(float* fCnTxtArray, int iLen);//hcw,2012.3.1,判断线型内容中多余的“0”
	void ClearInvalidZeroinLineTypeCnTxt(float* fCnTxtArray, int& iLen);//hcw,2012.3.1,清理线型内容中多余的“0”
	//BOOL hasSameLineTypeName(map<CString, CString> lineTypeNameMap, CString LineTypeName)//hcw,2012.3.1,是否有相同的线型名。
	//BOOL hasSameLineTypeCntxt(CArray<BaseLineType>&lineTypeArray, CString LineTypeName)//hcw,2012.3.1,内容是否相同。
	BOOL hasLineTypeConflict(map<CString,BaseLineType>& lineTypeMap); //hcw,2012.3.2,判断冲突，并返回冲突的结果集(同名线型)。
	map<CString, CString> ClearLineTypeConflict(map<CString,BaseLineType> lineTypeMap);//hcw,2012.3.2,处理清理后仍然同名的线型。
	void SetLineTypeNameMap(map<CString, CString>& LineTypeNameMap, map<CString,CString> lineTypeNameCflctMap);//hcw,2012.3.2,将参数2更新入参数1；
	void SortLayerByGroup(CString strGroupName);//hcw,2012.4.5,根据组名来读出树节点。
	CDlgDoc				*m_pDoc;													
	ConfigLibItem m_config;
	CArray<ConfigLibItem, ConfigLibItem> m_configsbyScale;//hcw,2012.2.24,
														//记录不同比例尺下的config
	int m_nCurSchemeScale;//hcw,2012.7.18,当前方案比例尺。
protected:	
	void SaveCellLinebyScale();//hcw,2012.2.24,保存各种比例尺下的线型名
	CEdit				m_wndCodeEdit;
	CListBox			m_wndSearchRsltList; //hcw,2012.2.27,保存模糊搜索的结果
	CCollectionTreeCtrl	m_wndIdxTree;
	CString m_SelLayNameandId;//hcw,2012.2.27,搜索框中被选择中的层名和层码
	CString m_StrLayName;//hcw,2012.4.9,树控件当前被选中的层码
	BOOL m_bUpDown; //hcw,2012.2.27,是否按下 ↑↓。
	BOOL m_bReturn; //hcw,2012.2.27,是否按下 Enter 键。
	BOOL m_bLButtnUp;//hcw,2012.2.27,是否按下左键
	BOOL m_bShift;//hcw,2012.3.9,是否按下shift
	BOOL m_bCtrl; //hcw,2012.3.9,是否按下Ctrl
	BOOL m_bFirstinSearchEdit; //hcw,2012.3.6,第一次进入编辑框
	BOOL m_bMultiSelected; //hcw,2012.3.15,是否为多选
	BOOL m_bInitialSelected;//hcw,2012.3.26,初始化时选中树节点。
	BOOL m_bSchemeDlgInitialized;//hcw,2012.3.29
	BOOL m_bFirstDrawPreview;//hcw,2012.3.29
	BOOL m_bEditLabel;//hcw,2012,4.9,编辑树节点。
	BOOL m_bAbsentSchemePath;//hcw,2012.7.26,方案路径是否存在。
	int m_nSelinSearchRsltList;//hcw,2012.4.9.
	CString m_CurStrCode;//hcw,2012.4.8
	UINT m_nCountTreeSelected;//hcw,2012.2.27
	CTreeItemList m_treeNodeList;//hcw,2012.2.27
	// 辅助修改层码和层名
	int m_nGroup,m_nIdx;
	CString m_oldGroupName,m_oldLayerName;

	CComboListCtrl		m_wndAttList;
	
	int m_nLayerIndex;     // 当前选中层在CScheme中的下标
	BOOL m_bSpecialLayer;  // 符号配置层
	
	//备份
	CString m_BackupCurSchemeXML;

	USERIDX m_BackupUserIdx;
	USERIDX m_UserIdx;

	BOOL m_bChgFromSelect;
	BOOL m_bCanSelectDefault;

	HTREEITEM m_hCurItem;

	BOOL m_bModified;
	BOOL m_bRepaintCollectionView;
	
	//复制
	vector<ItemInfo> copyItemList;
	
	//SymbolList
	CImageList m_listImages;
	CArray<int,int> m_arrIdxCreateFlag;
	BOOL m_bAddSymbol;
	int m_nSelSymbol;//hcw,2012-2-13,被选中的符号
	BOOL m_binSymbolListRegion;//hcw,2012-2-14,判断是否在符号选择区。
	BOOL m_bSymbolSelected ;//hcw,2012-2-14,判断是否选中了符号
	int m_nIdxToCreateImage, m_nImageWid, m_nImageHei;

	COLORREF m_nLayerColor;

	//参数修改对话框
	CDlgCellSetting		     *m_pCellDlg;
	CDlgDashLineSetting		 *m_pDashLineDlg;
	CDlgCellLineSetting		 *m_pCellLineDlg;
	CDlgColorHatchSetting    *m_pColorHatchDlg;
	CDlgCellHatchSetting	 *m_pCellHatchDlg;
	CDlgAnnotationSetting	 *m_pAnnoDlg;
	CDlgScaleLineSetting	 *m_pScaleDlg;
	CDlgLinefillSetting      *m_pLinefillDlg;
	CDlgDiagonalSetting      *m_pDiagonalDlg;
	CDlgParalineSetting      *m_pParalineDlg;
	CDlgAngbisectorSetting   *m_pAngBisectorDlg;
	CDlgScaleArcSetting      *m_pScaleArcDlg;
	CDlgScaleTurnplateSetting *m_pScaleTurnplateSetting;
	CDlgScaleCraneSetting *m_pScaleCraneSetting;
	CDlgScaleFunnelSetting *m_pScaleFunnelSetting;
	CDlgScaleChuanSongDaiSetting *m_pScaleChuanSongDaiSetting;
	CDlgProcSymLTSetting	*m_pProcSymLTSetting;
	CDlgScaleCellLineSetting *m_pScaleCellLTDlg;
	CDlgScaleCellSetting	*m_pScaleCellDlg;
	CDlgScaleOldDouyaSettings *m_pScaleOldDouyaDlg;
	CDlgScaleXiepo *m_pScaleXiepoDlg;
	CDlgScaleYouGuanSetting		*m_pScaleYouGuanDlg;
	CDlgTidalWaterSetting *m_pTidalWaterDlg;
	CDlgConditionColorHatch *m_pCondColorHatchDlg;
	CDlgCulvertSurfaceSetting* m_pCulvertSurfaceDlg;

	CToolTipCtrl m_ToolTip;

	// 当前文档比例尺
	int m_nCurDocScale;

	// 点击预览区域的次数:0,1,2
	int m_nClickNum;
	CPoint m_ptStart;
	CPoint m_ptCur;
	// 变换矩阵的逆矩阵
	double m_transformMatrix[9];

	// 要移动的组的下标
	int m_nMovedGroup;
	// 要移动层的下标
	int m_nMovedLayer;
	HTREEITEM m_hMovedItem;

	// 空白处点击
	BOOL m_bClickNewGroup;

	// 被复制符号的层名
	CString m_strcopyLayerName;

	HICON m_hIcon;

	CArray<ModifyLayerColor,ModifyLayerColor> m_arrModifyLayerColor;

private:
	CString m_strOldPath,m_strNewPath;//新路径与原路径 新路径无效时替换原路径

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCHEME_H__57B34998_467A_466E_AB5C_9DF7A3A043C6__INCLUDED_)
