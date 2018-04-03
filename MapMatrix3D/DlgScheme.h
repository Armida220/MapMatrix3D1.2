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
	afx_msg void OnSortIdcode(); //hcw,2012.2.20,��������������
	afx_msg void OnClearLinetype();//hcw,2012.2.21,������������
	afx_msg void OnListSelChanged();//hcw,2012.2.27,ѡ����������е�һ�
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
	// ��ѡ��ǰ��ĵ��ı�����ʱ������ɾ����ز���
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
	// typeΪ0ʱ�Ȳ�ȥ��ǰ���ģ�Ϊ1ʱֱ�ӻ� �� ��ȥԭ��
	void DrawTemLine(CPoint pt=CPoint(0,0), int type=1);

	void DefToBuf(CellDef *pDef, GrBuffer2d *buf);

	void ClearCtrls();
	void ReleaseDlgMemory();

	void OnSupportgeonameEdit();

	void GetCurAttribute(CValueTable &tab);


	//��ȡ���ظ��Ĳ���
	CString GetCheckedLayerName(CDlgNewLayer &layer, BOOL bShowCode=TRUE);
//	CString GetTemplate();
	BOOL isInRect(UINT uID, CPoint pt); //hcw,2012.2.13,���Ƿ�����Դ�����ڡ�
	BOOL CompareLineTypeCntxt(BaseLineType LType1, BaseLineType LType2);//hcw,2012.3.2,�Ƚ��������������Ƿ���ȫ��ͬ��
	BOOL hasValidZeroinLineTypeName(CString lineTypeName);//hcw,2012.2.22
	BOOL IsValidDefine(CString lineTypeName);//hcw,2012.2.22
	BOOL hasMatchedSep(CString lineTypeName, CString strSep);//hcw,2012.2.22
	BOOL hasLineType(CArray<CSymbol*, CSymbol*>& symbolsArray);//hcw,2012.2.26,�жϲ��еķ����Ƿ������͡�

	void ClearInvalidZeroinLineTypeName(CString& LineTypeName); //hcw, 2012.2.22, ������Ч����
	void ClearSingleCtxt(CString& lineTypeCtxt);//hcw,2012.2.22.
	CString RetrieveSeqSeps(CString lineTypeName);//hcw,2012.2.22��ȡ���ӷ�����,
												//eg:0.00X0.01*0.2 ->X*
	CString CombineSepsandCtxts(CString seqSeps, CStringArray& ctxtArray);//hcw,2012.2.22
	void substitudeLineNameLayer(map<CString, CString> lineTypeNameMap, //hcw,2012.2.26
										 CArray<CSymbol*,CSymbol*>& symbolsArray);//�������ӷ�������
	void ModifiedLineTypeinLayerDef(map<CString, CString> lineTypeNameMap, ConfigLibItem& config);//hcw,2012.2.26,�����޸ĸ�LayDefine�е����������
	void SetTreeNodeToTop(CString strLayerNameorID);//hcw,2012.2.27
	BOOL HasInvalidZeroinLineTypeCnTxt(float* fCnTxtArray, int iLen);//hcw,2012.3.1,�ж����������ж���ġ�0��
	void ClearInvalidZeroinLineTypeCnTxt(float* fCnTxtArray, int& iLen);//hcw,2012.3.1,�������������ж���ġ�0��
	//BOOL hasSameLineTypeName(map<CString, CString> lineTypeNameMap, CString LineTypeName)//hcw,2012.3.1,�Ƿ�����ͬ����������
	//BOOL hasSameLineTypeCntxt(CArray<BaseLineType>&lineTypeArray, CString LineTypeName)//hcw,2012.3.1,�����Ƿ���ͬ��
	BOOL hasLineTypeConflict(map<CString,BaseLineType>& lineTypeMap); //hcw,2012.3.2,�жϳ�ͻ�������س�ͻ�Ľ����(ͬ������)��
	map<CString, CString> ClearLineTypeConflict(map<CString,BaseLineType> lineTypeMap);//hcw,2012.3.2,�����������Ȼͬ�������͡�
	void SetLineTypeNameMap(map<CString, CString>& LineTypeNameMap, map<CString,CString> lineTypeNameCflctMap);//hcw,2012.3.2,������2���������1��
	void SortLayerByGroup(CString strGroupName);//hcw,2012.4.5,�����������������ڵ㡣
	CDlgDoc				*m_pDoc;													
	ConfigLibItem m_config;
	CArray<ConfigLibItem, ConfigLibItem> m_configsbyScale;//hcw,2012.2.24,
														//��¼��ͬ�������µ�config
	int m_nCurSchemeScale;//hcw,2012.7.18,��ǰ���������ߡ�
protected:	
	void SaveCellLinebyScale();//hcw,2012.2.24,������ֱ������µ�������
	CEdit				m_wndCodeEdit;
	CListBox			m_wndSearchRsltList; //hcw,2012.2.27,����ģ�������Ľ��
	CCollectionTreeCtrl	m_wndIdxTree;
	CString m_SelLayNameandId;//hcw,2012.2.27,�������б�ѡ���еĲ����Ͳ���
	CString m_StrLayName;//hcw,2012.4.9,���ؼ���ǰ��ѡ�еĲ���
	BOOL m_bUpDown; //hcw,2012.2.27,�Ƿ��� ������
	BOOL m_bReturn; //hcw,2012.2.27,�Ƿ��� Enter ����
	BOOL m_bLButtnUp;//hcw,2012.2.27,�Ƿ������
	BOOL m_bShift;//hcw,2012.3.9,�Ƿ���shift
	BOOL m_bCtrl; //hcw,2012.3.9,�Ƿ���Ctrl
	BOOL m_bFirstinSearchEdit; //hcw,2012.3.6,��һ�ν���༭��
	BOOL m_bMultiSelected; //hcw,2012.3.15,�Ƿ�Ϊ��ѡ
	BOOL m_bInitialSelected;//hcw,2012.3.26,��ʼ��ʱѡ�����ڵ㡣
	BOOL m_bSchemeDlgInitialized;//hcw,2012.3.29
	BOOL m_bFirstDrawPreview;//hcw,2012.3.29
	BOOL m_bEditLabel;//hcw,2012,4.9,�༭���ڵ㡣
	BOOL m_bAbsentSchemePath;//hcw,2012.7.26,����·���Ƿ���ڡ�
	int m_nSelinSearchRsltList;//hcw,2012.4.9.
	CString m_CurStrCode;//hcw,2012.4.8
	UINT m_nCountTreeSelected;//hcw,2012.2.27
	CTreeItemList m_treeNodeList;//hcw,2012.2.27
	// �����޸Ĳ���Ͳ���
	int m_nGroup,m_nIdx;
	CString m_oldGroupName,m_oldLayerName;

	CComboListCtrl		m_wndAttList;
	
	int m_nLayerIndex;     // ��ǰѡ�в���CScheme�е��±�
	BOOL m_bSpecialLayer;  // �������ò�
	
	//����
	CString m_BackupCurSchemeXML;

	USERIDX m_BackupUserIdx;
	USERIDX m_UserIdx;

	BOOL m_bChgFromSelect;
	BOOL m_bCanSelectDefault;

	HTREEITEM m_hCurItem;

	BOOL m_bModified;
	BOOL m_bRepaintCollectionView;
	
	//����
	vector<ItemInfo> copyItemList;
	
	//SymbolList
	CImageList m_listImages;
	CArray<int,int> m_arrIdxCreateFlag;
	BOOL m_bAddSymbol;
	int m_nSelSymbol;//hcw,2012-2-13,��ѡ�еķ���
	BOOL m_binSymbolListRegion;//hcw,2012-2-14,�ж��Ƿ��ڷ���ѡ������
	BOOL m_bSymbolSelected ;//hcw,2012-2-14,�ж��Ƿ�ѡ���˷���
	int m_nIdxToCreateImage, m_nImageWid, m_nImageHei;

	COLORREF m_nLayerColor;

	//�����޸ĶԻ���
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

	// ��ǰ�ĵ�������
	int m_nCurDocScale;

	// ���Ԥ������Ĵ���:0,1,2
	int m_nClickNum;
	CPoint m_ptStart;
	CPoint m_ptCur;
	// �任����������
	double m_transformMatrix[9];

	// Ҫ�ƶ�������±�
	int m_nMovedGroup;
	// Ҫ�ƶ�����±�
	int m_nMovedLayer;
	HTREEITEM m_hMovedItem;

	// �հ״����
	BOOL m_bClickNewGroup;

	// �����Ʒ��ŵĲ���
	CString m_strcopyLayerName;

	HICON m_hIcon;

	CArray<ModifyLayerColor,ModifyLayerColor> m_arrModifyLayerColor;

private:
	CString m_strOldPath,m_strNewPath;//��·����ԭ·�� ��·����Чʱ�滻ԭ·��

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCHEME_H__57B34998_467A_466E_AB5C_9DF7A3A043C6__INCLUDED_)
