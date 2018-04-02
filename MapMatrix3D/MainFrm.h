// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__7B8B2F69_9316_478B_8052_8BEAD5C457C6__INCLUDED_)
#define AFX_MAINFRM_H__7B8B2F69_9316_478B_8052_8BEAD5C457C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OpSettingToolBar.h"
#include "OutputViewBar.h"
#include "PropertiesViewBar.h"
#include "UIFStatusBarEx.h"
#include "VDVirtualDriver.h"
#include "ProjectViewBar.h "
#include "CollectionViewBar.h "
#include "DlgSearchResult.h"
#include "QueryMenu.h"
#include "AccuDlg.h "
#include "DlgDetectResult.h"
#include "TestVectorBar.h "
#include "CheckSchemeViewBar.h "
#include "CheckResultViewBar.h "
#include "multicombo.h"
#include "DlgNextLast.h"
#include "FTRLayerViewBar.h"
#include "CheckExProp.h"
#include <memory>
//#include "../mm3dPrj/DuiTreeView.h"
#include "../mm3dPrj/DuiToolbar.h"
#include "../mm3dPrj/ShowImgDlg.h"
//#include "../mm3dPrj/DuiTitlePane.h"


class CBlankDlg;

typedef struct CopyData
{
	CopyData() {
		pDoc = NULL;
	}
	~CopyData() {}
	CopyData& operator=(CopyData &data){
		pDoc = data.pDoc;
		ftrs.Copy(data.ftrs);
		pt = data.pt;
		return *this;
	}
	void Clear(){
		pDoc = NULL;
		ftrs.RemoveAll();
	}
	CDlgDoc *pDoc;
	CArray<FTR_HANDLE,FTR_HANDLE> ftrs;
	// 基点
	PT_3D pt;
}CopyData;

extern CMFCToolBarComboBoxButton *GetCommandButton(UINT CmdID);

typedef struct tag_DockingControlBar
{
	CDockablePane *pBar;
	int nType;
	UINT nID;
}DockingControlBar;

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

	CMFCStatusBar& GetStatusBar() { return m_wndStatusBar; }
	CPermanentExchanger* GetPermanentExchangerOfPropertiesView() { return m_wndPropertiesBar.GetPermaExchanger(); }
	void DockPlugPane();

	/**
	* @brief getPrjViewBar 获得工程视图工具栏
	*/
	inline CProjectViewBar* getPrjViewBar()
	{
		return &m_wndProjectView;
	}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	virtual BOOL OnShowPopupMenu (CMFCPopupMenu* pMenuPopup);
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
           
	inline CDuiToolbar* getToolbarPane()
	{
		return &m_duiToolBar;
	}

protected:  // control bar embedded members
	CMFCMenuBar		m_wndMenuBar;
	CUIFStatusBarEx		m_wndStatusBar;
	CMFCToolBar			m_wndToolBar;
	CMFCToolBar			m_wndToolbarView;
	CMFCToolBar			m_wndToolbarLidar;
	CMFCToolBar			m_wndToolBarTestContour;
	CMFCToolBar			m_wndToolbarEdit;
	CMFCToolBar			m_wndToolbarDraw;
	CMFCToolBar			m_wndToolbarEditContours;
	CMFCToolBar			m_wndToolbarEditVect;
	CMFCToolBar			m_wndToolbarEditAdvanced;
	CMFCToolBar			m_wndToolbarStereoMagnify;
	CMFCDropDownToolBar m_wndToolbarDrawPoint;
	CMFCDropDownToolBar m_wndToolbarDrawLine;
	CMFCDropDownToolBar m_wndToolbarDrawCircle;
	CMFCDropDownToolBar m_wndToolbarDrawSurface;
	CMFCToolBar		m_wndToolbarLayer;
	CMFCToolBar		m_wndToolbarFtr;
	CMFCToolBar		m_wndToolbarVisibilitySwitch;
	CMFCToolBar      m_wndToolbarTopo;

	///*
	//* brief 重做title栏
	//*/
	//CDuiTitlePane m_duiTitle;

	/*
	* brief 重新制作工具栏
	*/
	CDuiToolbar m_duiToolBar;

	/*
	* brief 重新制作侧边栏
	*/
	//CDuiTreeView m_duiTreeView;
	
	CPtrArray m_ptrListViews;

	CopyData   m_copyData;

public:	
	CAccuDlg				m_wndAccu;
	CTestVectorBar      m_wndTestDlgBar;
	CCollectionViewBar	m_wndCollectionView;
	CPtrArray			m_arrPlugsToolBar;
	CArray<DockingControlBar,DockingControlBar> m_arrPluginDockingControlBars;
	CProjectViewBar     m_wndProjectView;
	COpSettingToolBar m_wndOpSettingBar;
	COutputViewBar m_wndOutputBar;
	CPropertiesViewBar m_wndPropertiesBar;
	CVDVirtualDriver m_deviceDriver;
	CFTRLayerViewBar m_wndFtrLayerView;

	CShowImgBar m_showImgBar;

	CDlgDetectResult m_dlgDetectResult;
	CQueryMenu	m_cQueryMenu;

	CDlgCellDefLinetypeView *m_pCellDefDlg;
	CCheckExProp *m_CheckExPropDlg;
	CEditExProp *m_EditExPropDlg;
	CCheckSchemeViewBar   m_wndCheckSchemeViewBar;
	CCheckResultViewBar   m_wndCheckResultBar;
	
	CDlgNextLast	m_dlgSelectLocate;
	CDlgLocateFtrs  m_dlfFtrsLocate;

	CConfigLibManager *m_pMainSrcCfgLibManforMerge;//hcw,2012.7.25,方案合并中，源方案下对不同比例尺方案的管理。
	CConfigLibManager *m_pMainDesCfgLibManforMerge;//hcw,2012.7.25,方案合并中，目标方案下对不同比例尺方案的管理。
	int m_DesSchemeScaleforMerge; //hcw,2012.6.21,方案合并中,目标方案当前比例尺。
	int m_SrcSchemeScaleforMerge; //hcw,2012.6.21,方案合并中,源方案当前比例尺。
	CConfigLibManager *m_pMainSrcCfgLibManforCopy; //hcw,2012.9.20,复制属性中,源中各比例尺方案。
	CConfigLibManager *m_pMainDesCfgLibManforCopy; //hcw,2012.9.20,复制属性中,目标中各比例尺方案。
	int m_DesSchemeScaleforCopy; //hcw,2012.9.20,复制属性中当前源比例尺。
	int m_SrcSchemeScaleforCopy; //hcw,2012.9.20,复制属性中当前目标比例尺。
	BOOL m_bLoadedSrcTreeforMerge; //hcw,2012.9.20,合并方案中的目标方案是否被初始化。
	BOOL m_bLoadedSrcTreeforCopy; //hcw,2012.9.20,复制属性中目标方案是否被初始化。
	BOOL m_bPauseDevice;

	void AdjustPosUserToolBar();
	void LoadPluginDockingControlBars();
private:
	void SendMessageToViews(UINT message, WPARAM wParam, LPARAM lParam);
	void AddMenuCMDToDevice(UINT menuId);
	void ReleaseCfgLibMan();//hcw,2012.8.31
	BOOL CreateToolBars();
	BOOL CreateDockingWindows();
// Generated message map functions

protected:
	/*
	** brief hideToolBars 隐藏工具栏
	*/
	void hideToolBars();

protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg	void OnViewOpSetting();
	afx_msg void OnUpdateViewOpSetting(CCmdUI* pCmdUI);
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnToolInputequipment();
	afx_msg void OnDevicePause();
	afx_msg void OnUpdateDevicePause(CCmdUI* pCmdUI);
	afx_msg void OnAddSensitive();
	afx_msg void OnSubSensitive();
	afx_msg void OnAddSensitiveZ();
	afx_msg void OnSubSensitiveZ();
	afx_msg void OnToolsOptions();
	afx_msg void OnCustomizeKey();
	afx_msg void OnToolsPluginManage();
	afx_msg void OnConfigConvert();
	afx_msg void OnChangeSchemeOfFDB();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateOption(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg void OnViewProject();
	afx_msg void OnUpdateViewProject(CCmdUI* pCmdUI);
	afx_msg void OnViewFtrLayer();
	afx_msg void OnUpdateViewFtrLayer(CCmdUI* pCmdUI);
	afx_msg void OnViewCollection();
	afx_msg void OnViewResult();
	afx_msg void OnQueryResultLast();
	afx_msg void OnQueryResultNext();
	afx_msg LRESULT OnGetQueryMenu(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCollectionView(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnInitFtrRecent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitLayerTool(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitFtrTool(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitFtrLayerView(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateViewToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewToolbar();
	afx_msg void OnFeatureCode();
	afx_msg void OnToolHeightstep();
	afx_msg void OnWindowSwitch();
	afx_msg void OnToolSchememanage();
	afx_msg void OnViewXYZ();
	afx_msg void OnViewDrawToolbar();
	afx_msg void OnUpdateViewDrawToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewViewToolbar();
	afx_msg void OnUpdateViewViewToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewEditToolbar();
	afx_msg void OnUpdateViewEditToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewEditvectToolbar();
	afx_msg void OnUpdateViewEditvectToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewEditAdvancedToolbar();
	afx_msg void OnUpdateViewEditAdvancedToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewTestcontour();
	afx_msg void OnUpdateViewTestcontour(CCmdUI* pCmdUI);
	afx_msg void OnViewEditcontoursToolbar();
	afx_msg void OnUpdateViewEditcontoursToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewLidar();
	afx_msg void OnUpdateViewLidar(CCmdUI* pCmdUI);
	afx_msg void OnViewVisibilitySwitch();
	afx_msg void OnUpdateViewVisibilitySwitch(CCmdUI* pCmdUI);
	afx_msg void OnViewToolbarTopo();
	afx_msg void OnUpdateViewToolbarTopo(CCmdUI* pCmdUI);
	afx_msg void OnViewLayerToolbar();
	afx_msg void OnUpdateViewLayerToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewFeatureToolbar();
	afx_msg void OnUpdateViewFeatureToolbar(CCmdUI* pCmdUI);
	afx_msg void OnCellDefView();
	afx_msg void OnCellDefEdit();
	afx_msg void OnCellDefCopy();
	afx_msg void OnWindowCloseAll();
	afx_msg void OnCombinScheme();
	afx_msg void OnDestroy();
	afx_msg void OnOptionQt();
	afx_msg void OnDataCheck();
	afx_msg void OnUpdateDataCheck(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewResult(CCmdUI* pCmdUI);
	afx_msg void OnConfigMerge();//hcw,2012.3.14,方案合并
	afx_msg void OnConfigCopyproperty();//hcw,2012.8.31,复制方案中层的相关属性。
	afx_msg void OnViewStereoMagnify();
	afx_msg void OnUpdateViewStereoMagnify(CCmdUI* pCmdUI);
	afx_msg void OnCommandIBoxFunc(UINT nID);
	//}}AFX_MSG
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	afx_msg LRESULT OnToolbarContextMenu(WPARAM,LPARAM);
	afx_msg LRESULT OnUpdateIndicator(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnModifyPropertyWnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdatePropertyWnd(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateSensitive(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCorrd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSnapFtr(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSnapStatus(CCmdUI* pCmdUI);
	afx_msg LRESULT OnLoadProject(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUnInstallProject(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshVecWin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshCurrentStereoWin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshCurrentStereoMS(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshVecflag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSwitchStereoMS(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetToler(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetProject(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXYZ(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChangeActiveTab (WPARAM wp, LPARAM);
	afx_msg LRESULT OnKeyIn (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTranslateKeyInMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrintStr(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnUpdateAccuDlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowAccudlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceLFOOT(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceMFOOT(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceRFOOT(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceMOVE(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceKEY(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateViewList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomDlgClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddRecentCmd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadRecentCmd(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWindowManager();
	afx_msg LRESULT OnUpdateSymScale(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReCreateAllFtrs(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnViewTestDlgBar();
	afx_msg void OnUpdateViewTestDlgBar(CCmdUI* pCmdUI);
	afx_msg LRESULT OnGetDrawTestPtBar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshChkSchemeViewBar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowCheckResults(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCopyDocData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCloseDoc(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg LRESULT OnSetFocusColletion(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUVSOperatecountChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCheckExProp();
	afx_msg void OnEditExProp();
	afx_msg void OnUpdateExportMenu(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	
	afx_msg void OnViewMm3dtools();
	afx_msg void OnUpdateViewMm3dtools(CCmdUI *pCmdUI);
	afx_msg void OnIdrOsgmultifilelinkcreator();
	afx_msg void OnIdrMm3dConvertSmart3d();
	afx_msg void OnIdrDsmToOsgb();
	afx_msg void OnLinkfetchcoord();
	afx_msg void OnExtractaerial();

public:
	afx_msg void OnOsgbviewImg();
	afx_msg void OnUpdateOsgbviewImg(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__7B8B2F69_9316_478B_8052_8BEAD5C457C6__INCLUDED_)
