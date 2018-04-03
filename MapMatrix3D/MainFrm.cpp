// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "EditBase.h"
#include "SmartViewFunctions.h"
#include "MainFrm.h"
#include "ExMessage.h"
#include "OptionsDlg.h"
#include "UIFToolbarComboBoxButtonEx.h"
#include "VDDevice.h"
#include "BaseView.h "
#include "RegDef.h"
#include "RegDef2.h"
#include "CollectionTreeCtrl.h"
#include "DlgSetHeiStep.h "
#include "DlgScheme.h"
#include "StereoView.h "
#include "RecentCmd.h "
#include "DlgCellDefLinetypeView.h"
#include "UndoBar.h"
#include "CommonCallStation.h"
#include "UserCustomToolbar.h "
#include "CombinDlg.h"
#include "DlgPluginsManage.h"
#include "DlgCellDefCopy.h"
#include "SchemeMerge.h"//hcw,2012.4.10
#include "SchemePropertyCopy.h" //hcw,2012.8.31
#include "windowsx.h"//cjc 2012.10.26
#include "DlgSchemeConvert.h"
#include "VectorView_new.h"
#include "UVSModify.h"
#include "../mm3dPrj/MyDefineDockTabPane.h"


#ifdef TRIAL_VERSION
#include "WinlicenseSDK.h"
#endif

#define TIMERID_SAVE			1
#define TIMERID_TRIALMSG		2

#define MAX_PRECISION			1000
#define MIN_PRECISION			0.001

#define DOCKINGCONTROLBARID_USERCUSTOM     63000

UINT gDockingControlBarId = DOCKINGCONTROLBARID_USERCUSTOM;
extern int GetAccessType(CDataSourceEx *pDS);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMFCToolBarComboBoxButton *GetCommandButton(UINT CmdID)
{
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (CmdID, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition (); posCombo != NULL;)
		{
			CMFCToolBarComboBoxButton* pCombo =
				DYNAMIC_DOWNCAST(CMFCToolBarComboBoxButton, listButtons.GetNext(posCombo));
			
			if (pCombo != NULL )
			{
				return pCombo;
			}
		}
	}
	return NULL;
}


static BOOL gFindUserIdx(USERIDX *pIdx, BOOL bFindStr, __int64 &code, const char*& name)
{
	int num = pIdx->m_aIdx.GetSize();
	if( bFindStr )
	{
		for( int i=0; i<num; i++)
		{
			static IDX t;
			t = pIdx->m_aIdx.GetAt(i);
			if( t.code>0 && t.code==code )
			{
				name = t.FeatureName;
				return TRUE;
			}
		}
	}
	else
	{
		for( int i=0; i<num; i++)
		{
			IDX t = pIdx->m_aIdx.GetAt(i);
			if( t.code>0 && stricmp(name,t.FeatureName)==0 )
			{
				code = t.code;
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)
const int  iMaxUserToolbars		= 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId	= uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_COMMAND(ID_VIEW_INPUTSETTING, OnViewOpSetting)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INPUTSETTING, OnUpdateViewOpSetting)
	ON_COMMAND(ID_VIEW_OUTPUT, OnViewOutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, OnUpdateViewOutput)
	ON_WM_CLOSE()
	ON_COMMAND(ID_TOOL_INPUTEQUIPMENT, OnToolInputequipment)
	ON_COMMAND(ID_DEVICE_PAUSE, OnDevicePause)
	ON_UPDATE_COMMAND_UI(ID_DEVICE_PAUSE, OnUpdateDevicePause)
	ON_COMMAND(ID_DEVICE_ADD_SENSITIVE,OnAddSensitive)
	ON_COMMAND(ID_DEVICE_SUB_SENSITIVE,OnSubSensitive)
	ON_COMMAND(ID_DEVICE_ADD_SENSITIVE_Z,OnAddSensitiveZ)
	ON_COMMAND(ID_DEVICE_SUB_SENSITIVE_Z,OnSubSensitiveZ)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(ID_CUSTOMIZE_KEY, OnCustomizeKey)
	ON_COMMAND(ID_TOOL_PLUGINMANAGE,OnToolsPluginManage)
	ON_COMMAND(ID_CONFIG_CONVERT, OnConfigConvert)
	ON_MESSAGE(FCCM_PROGRESS,OnProgress)
	ON_MESSAGE(FCCM_OPTIONUPDATE,OnUpdateOption)
	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, OnUpdateViewProperties)
	ON_COMMAND(ID_VIEW_PROJECT, OnViewProject)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT, OnUpdateViewProject)
	ON_COMMAND(ID_VIEW_LAYER, OnViewFtrLayer)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LAYER, OnUpdateViewFtrLayer)
	ON_COMMAND(ID_VIEW_COLLECTION, OnViewCollection)
	ON_COMMAND(ID_VIEW_RESULT, OnViewResult)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESULT, OnUpdateViewResult)
	ON_COMMAND(ID_QUERYRESULT_LAST,OnQueryResultLast)
	ON_COMMAND(ID_QUERYRESULT_NEXT,OnQueryResultNext)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_COMMAND(IDR_FEATURE_CODE,OnFeatureCode)
	ON_COMMAND(ID_TOOL_HEIGHTSTEP, OnToolHeightstep)
	ON_COMMAND(ID_WINDOW_SWITCH, OnWindowSwitch)
	ON_COMMAND(ID_TOOL_SCHEME, OnToolSchememanage)
	ON_COMMAND(ID_VIEW_ACCUDLG, OnViewXYZ)
	ON_COMMAND(ID_VIEW_DRAW_TOOLBAR, OnViewDrawToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DRAW_TOOLBAR, OnUpdateViewDrawToolbar)
	ON_COMMAND(ID_VIEW_VIEW_TOOLBAR, OnViewViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VIEW_TOOLBAR, OnUpdateViewViewToolbar)
	ON_COMMAND(ID_VIEW_EDIT_TOOLBAR, OnViewEditToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDIT_TOOLBAR, OnUpdateViewEditToolbar)
	ON_COMMAND(ID_VIEW_EDITVECT_TOOLBAR, OnViewEditvectToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITVECT_TOOLBAR, OnUpdateViewEditvectToolbar)
	ON_COMMAND(ID_VIEW_EDITADVANCED_TOOLBAR, OnViewEditAdvancedToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITADVANCED_TOOLBAR, OnUpdateViewEditAdvancedToolbar)
	ON_COMMAND(ID_VIEW_TESTCONTOUR, OnViewTestcontour)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TESTCONTOUR, OnUpdateViewTestcontour)
	ON_COMMAND(ID_VIEW_LAYER_TOOLBAR, OnViewLayerToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LAYER_TOOLBAR, OnUpdateViewLayerToolbar)
	ON_COMMAND(ID_VIEW_FEATURE_TOOLBAR, OnViewFeatureToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FEATURE_TOOLBAR, OnUpdateViewFeatureToolbar)
	ON_COMMAND(ID_VIEW_STEREO_MAGNIFY, OnViewStereoMagnify)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STEREO_MAGNIFY, OnUpdateViewStereoMagnify)
	ON_COMMAND(ID_VIEW_EDITCONTOURS_TOOLBAR, OnViewEditcontoursToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITCONTOURS_TOOLBAR, OnUpdateViewEditcontoursToolbar)
	ON_COMMAND(ID_VIEW_LIDAR, OnViewLidar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIDAR, OnUpdateViewLidar)
	ON_COMMAND(ID_VIEW_VISIBILITY_SWITCH, OnViewVisibilitySwitch)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VISIBILITY_SWITCH, OnUpdateViewVisibilitySwitch)
	ON_COMMAND(ID_VIEW_TOPO, OnViewToolbarTopo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOPO, OnUpdateViewToolbarTopo)
	ON_COMMAND(ID_VIEW_STEREOMAGNIFY, OnViewStereoMagnify)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STEREOMAGNIFY, OnUpdateViewStereoMagnify)
	ON_COMMAND(ID_CELLDEF_VIEW, OnCellDefView)
	ON_COMMAND(ID_CELLDEF_EDIT, OnCellDefEdit)
	ON_COMMAND(ID_CELLDEF_COPY, OnCellDefCopy)
	ON_COMMAND(ID_WINDOW_CLOSE_ALL, OnWindowCloseAll)
	ON_COMMAND(ID_COMBIN_SCHEME,OnCombinScheme)
	ON_COMMAND(ID_TOOL_DATA_CHANGESCHEME,OnChangeSchemeOfFDB)
	ON_WM_DESTROY()
	ON_COMMAND(ID_OPTION_QT, OnOptionQt)
	ON_COMMAND(ID_CONFIG_MERGE, OnConfigMerge)//hcw,2012.3.14,方案合并
	ON_COMMAND(ID_INDICATOR_SENSITIVE, OnToolInputequipment)
	ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, OnToolbarReset)
	ON_REGISTERED_MESSAGE(AFX_WM_TOOLBARMENU, OnToolbarContextMenu)
	ON_COMMAND(ID_DATACHECK, OnDataCheck)
	ON_UPDATE_COMMAND_UI(ID_DATACHECK, OnUpdateDataCheck)
	ON_COMMAND(ID_CONFIG_COPYPROPERTY, OnConfigCopyproperty)
	ON_COMMAND_RANGE(ID_IBOX_FUNC_CLOSE, ID_IBOX_FUNC_ZOOMOUT_S, OnCommandIBoxFunc)
	//}}AFX_MSG_MAP
	ON_MESSAGE(FCCM_MODIFY_PROPERTY_UI, OnModifyPropertyWnd)
	ON_MESSAGE(FCCM_UPDATE_PROPERTY_UI, OnUpdatePropertyWnd)
	ON_MESSAGE(FCCM_UPDATEINDICATOR,OnUpdateIndicator)
	ON_MESSAGE(FCCM_SHOW_XYZ , OnXYZ)
	ON_MESSAGE(FCCM_LOADPROJECT,OnLoadProject)
	ON_MESSAGE(FCCM_UNINSTALLPROJ,OnUnInstallProject)
	ON_MESSAGE(FCCM_REFRESHVECWIN,OnRefreshVecWin)
	ON_MESSAGE(FCCM_REFRESHCURRENTSTEREOWIN,OnRefreshCurrentStereoWin)
	ON_MESSAGE(FCCM_REFRESHCURRENTSTEREOMS,OnRefreshCurrentStereoMS)
	ON_MESSAGE(FCCM_REFRESHVECFLAG,OnRefreshVecflag)
	ON_MESSAGE(FCCM_SWITCHSTEREOMS,OnSwitchStereoMS)
	ON_MESSAGE(FCCM_GETTOLER,OnGetToler)
	ON_MESSAGE(FCCM_GETPROJECT,OnGetProject)
	ON_MESSAGE(FCCM_GETQUERYMENU,OnGetQueryMenu)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SENSITIVE,OnUpdateSensitive)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COORD,OnUpdateCorrd)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SNAP_FTR,OnUpdateSnapFtr)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SNAP, OnUpdateSnapStatus)
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, OnChangeActiveTab)
	ON_REGISTERED_MESSAGE(AFX_WM_TOOLBARMENU, OnToolbarContextMenu)
	ON_MESSAGE(FCCM_KEYIN,OnKeyIn)
	ON_MESSAGE(FCCM_TRANSLATEMSG,OnTranslateKeyInMsg)
	ON_MESSAGE(FCCM_COLLECTIONVIEW,OnCollectionView)
	ON_MESSAGE(FCCM_INITFTRCLASS,OnInitFtrRecent)
	ON_MESSAGE(FCCM_INITLAYERTOOL,OnInitLayerTool)
	ON_MESSAGE(FCCM_INITLAYVIEWBAR,OnInitFtrLayerView)
	ON_MESSAGE(FCCM_INITFTRTOOL,OnInitFtrTool)
	ON_MESSAGE(FCCM_PRINTSTR,OnPrintStr)
	ON_MESSAGE(FCCM_UPDATE_ACCUDLG,OnUpdateAccuDlg)
	ON_MESSAGE(FCCM_SHOW_ACCUDLG,OnShowAccudlg)	
	ON_MESSAGE(WM_DEVICE_LFOOT,OnDeviceLFOOT)
	ON_MESSAGE(WM_DEVICE_RFOOT,OnDeviceRFOOT)
	ON_MESSAGE(WM_DEVICE_MFOOT,OnDeviceMFOOT)
	ON_MESSAGE(WM_DEVICE_MOVE,OnDeviceMOVE)
	ON_MESSAGE(WM_DEVICE_KEY,OnDeviceKEY)
	ON_MESSAGE(FCCM_UPDATEVIEWLIST,OnUpdateViewList)
	ON_MESSAGE(FCCM_CUSTOMDLGCLOSE,OnCustomDlgClose)
	ON_MESSAGE(FCCM_ADD_RECENTCMD,OnAddRecentCmd)
	ON_MESSAGE(FCCM_LOAD_RECENTCMD,OnLoadRecentCmd)
	ON_MESSAGE(FCCM_SETSYMDRAWSCALE,OnUpdateSymScale)
	ON_MESSAGE(FCCM_DOCRECREATEFTRS,OnReCreateAllFtrs)
	ON_COMMAND(ID_TESTDLG_BAR,OnViewTestDlgBar)
	ON_UPDATE_COMMAND_UI(ID_TESTDLG_BAR, OnUpdateViewTestDlgBar)
	ON_MESSAGE(FCCM_GETDRAWTESTPTBAR,OnGetDrawTestPtBar)
	ON_MESSAGE(FCCM_REFRESHCHKSCHBAR,OnRefreshChkSchemeViewBar)
	ON_MESSAGE(FCCM_CHECKRESULT,OnShowCheckResults)
	ON_MESSAGE(FCCM_COPYDATA,OnCopyDocData)
	ON_MESSAGE(FCCM_CLOSEDOC,OnCloseDoc)
	ON_WM_COPYDATA()
	ON_MESSAGE(FCCM_SETFOCUSCOL,OnSetFocusColletion)
	ON_MESSAGE(FCCM_UVS_OPERATECOUNT_CHANGE, OnUVSOperatecountChange)
	ON_COMMAND(ID_CHECK_EXTERNAL_PRO, OnCheckExProp)
	ON_COMMAND(ID_EDIT_EXTERNAL_PRO, OnEditExProp)
	//
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ASDXF, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_FILE_OUTPUT_CTRLPTS, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ASXML, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_WORK_OUTPUT_MAPSTARTEXT, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_WORK_OUTPUT_VVT, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_WORK_OUTPUT_SHP, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_WORK_OUTPUT_EPS, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_WORK_OUTPUT_E00, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTJB, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTASC, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_FDB, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_ARCGISMDB, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ASCAS, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_BATCH_EXPORTMAPS, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_BATCH_EXPORTMAPS_DXF, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_MAPDECORATE_CREATE_FIGURE, OnUpdateExportMenu)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateExportMenu)
	//
//	ON_WM_NCCALCSIZE()
	ON_WM_NCCALCSIZE()
//ON_COMMAND(ID_WINDOW_TILE_VERT, &CMainFrame::OnWindowTileVert)
//ON_WM_PAINT()
//ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCLBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCHITTEST()
	ON_WM_SYSCOMMAND()
	
	ON_COMMAND(ID_VIEW_MM3DTOOLS, &CMainFrame::OnViewMm3dtools)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MM3DTOOLS, &CMainFrame::OnUpdateViewMm3dtools)
	ON_COMMAND(IDR_OSGMULTIFILELINKCREATOR, &CMainFrame::OnIdrOsgmultifilelinkcreator)
	ON_COMMAND(IDR_MM3D_CONVERT_SMART3D, &CMainFrame::OnIdrMm3dConvertSmart3d)
	ON_COMMAND(IDR_DSM_TO_OSGB, &CMainFrame::OnIdrDsmToOsgb)
	ON_COMMAND(ID_LINKFETCHCOORD, &CMainFrame::OnLinkfetchcoord)
	ON_COMMAND(ID_EXTRACTAERIAL, &CMainFrame::OnExtractaerial)
	ON_COMMAND(ID_OSGBVIEW_IMGDLG, &CMainFrame::OnOsgbviewImg)
	ON_UPDATE_COMMAND_UI(ID_OSGBVIEW_IMGDLG, &CMainFrame::OnUpdateOsgbviewImg)
	
	END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
		//IDS_CARET_POS,
 	ID_INDICATOR_SENSITIVE,
 	ID_INDICATOR_COORD,
	ID_INDICATOR_SNAP,
	ID_INDICATOR_SNAP_FTR,
	IDS_CARET_POS,
	ID_INDICATOR_GRID,
	ID_INDICATOR_OPERATECOUNT
	/*	ID_INDICATOR_LOCKZ,
		ID_INDICATOR_SYMBOL,
		ID_INDICATOR_LAYER, 
		ID_INDICATOR_SYMINDEX, 
		ID_INDICATOR_SELECTSTATUS,
		ID_INDICATOR_DBSTATUS*/
};

static void ValidatePrecision(float& p)
{
	if( p>MAX_PRECISION )p = MAX_PRECISION;
	else if( p<-MAX_PRECISION )p = -MAX_PRECISION;
	else if( p>=0 && p<MIN_PRECISION )p = MIN_PRECISION;
	else if( p<0 && p>-MIN_PRECISION )p = -MIN_PRECISION;
}


static long gTrialTime0 = 1;

CDlgDoc *GetActiveDlgDoc();

void StopWork()
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( pDoc!=NULL )
	{
		pDoc->StopWork();
	}
	else
	{
		AfxGetMainWnd()->DestroyWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	CMFCPopupMenu::SetForceShadow (TRUE);
	m_bPauseDevice = FALSE;
//	m_bFullScreen	= false;
	m_pCellDefDlg = NULL;
	m_DesSchemeScaleforMerge = 0; //hcw,2012.6.21
	m_SrcSchemeScaleforMerge = 0; //hcw,2012.6.21
	m_pMainDesCfgLibManforMerge = NULL;//hcw,2012.7
	m_pMainSrcCfgLibManforMerge = NULL;//hcw,2012.7
	
	//{2012.9.20,Append;
	m_DesSchemeScaleforCopy = 0; 
	m_SrcSchemeScaleforCopy = 0; 
	m_pMainDesCfgLibManforCopy = NULL; 
	m_pMainSrcCfgLibManforCopy = NULL; 
	
	m_bLoadedSrcTreeforCopy = FALSE; 
	m_bLoadedSrcTreeforMerge = FALSE; 

	m_CheckExPropDlg = NULL;
	m_EditExPropDlg = NULL;
	//}
}

CMainFrame::~CMainFrame()
{
	if (m_pCellDefDlg)
	{
		delete m_pCellDefDlg;
		m_pCellDefDlg = NULL;
	}
	if (m_CheckExPropDlg)
	{
		delete m_CheckExPropDlg;
		m_CheckExPropDlg = NULL;
	}
	if (m_EditExPropDlg)
	{
		delete m_EditExPropDlg;
		m_EditExPropDlg = NULL;
	}
	ReleaseCfgLibMan();
//	CUVSModify::ReleaseUVSServer(TRUE);
}

LRESULT CMainFrame::OnUVSOperatecountChange( WPARAM wParam, LPARAM lParam )
{
	CString  str;
	str.Format("%d", wParam);	
    m_wndStatusBar.SetPaneText(7,str);
	return 1;
}

static void AddMenuToDevice(CMenu *pMenu, char *label, CVDVirtualDriver *pd)
{
	if( !pMenu )return;
	
	int num = pMenu->GetMenuItemCount();
	for( int i=0; i<num; i++)
	{
		int id = pMenu->GetMenuItemID(i);
		if( id>0 )
		{
			COMMAND cmd;
			cmd.nMsgID = WM_DEVICE_KEY;
			cmd.wParam = id;
			cmd.lParam = 0;
			
			memset(cmd.strDescr,0,sizeof(cmd.strDescr));
			if( label && strlen(label)>0 )
				strcpy(cmd.strDescr,label);
			
			char curstr[256] = {0};
			pMenu->GetMenuString(i,curstr,sizeof(cmd.strDescr)-1,MF_BYPOSITION);
			strcat(cmd.strDescr,curstr);
			
			pd->AddCommand(&cmd,1);
		}
		else if( id==-1 )
		{
			char curstr[256] = {0};
			int  len = strlen(label);
			pMenu->GetMenuString(i,curstr,sizeof(curstr)-1,MF_BYPOSITION);
			strcat(label,curstr);
			strcat(label,"->");
			
			AddMenuToDevice(pMenu->GetSubMenu(i),label,pd);
			memset(label+len,0,strlen(label)-len);
		}
	}
}


void CMainFrame::AddMenuCMDToDevice(UINT menuId)
{
	char label[1024] = {0};
	CMenu menu;
	menu.LoadMenu(menuId);
	AddMenuToDevice(&menu,label,&m_deviceDriver);
}

void CMainFrame::LoadPluginDockingControlBars()
{
	const CArray<PlugItem,PlugItem> &arrItem = ((CEditBaseApp*)AfxGetApp())->m_arrPlugObjs;
	for (int i=0;i<arrItem.GetSize();i++)
	{
		if (!arrItem[i].bUsed) continue;
		
		CDockablePane *pDockingControlBars = arrItem[i].pObj->GetDockingControlBar();
		if (pDockingControlBars == NULL) continue;
		
		CString title = arrItem[i].pObj->GetDockingControlBarTitle();
		int type = arrItem[i].pObj->GetDockingControlBarType();
		
		if (!pDockingControlBars->Create (title, this, CRect (0, 0, 200, 200),
			TRUE, 
			gDockingControlBarId++,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
		{
			continue;
		}
		
		DockingControlBar bar;
		bar.pBar = pDockingControlBars;
		bar.nType = type;
		bar.nID = gDockingControlBarId-1;
		m_arrPluginDockingControlBars.Add(bar);
		
	}
}


BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	CString strRecv = (LPCTSTR)(pCopyDataStruct->lpData);

//	AfxMessageBox(strRecv);
	if (IsIconic())
	{
		AfxGetMainWnd()->ShowWindow(SW_SHOWNORMAL);
	}
	
	if (strRecv.IsEmpty())
	{
		return FALSE;
	}

	int pos = strRecv.Find('*');
	
	if (pos<0)
	{
		AfxGetMainWnd()->SendMessage(FCCM_LOADPROJECT, FCPV_DLGFILE, (LPARAM)(LPCTSTR)strRecv);
	}
	else
	{
		CString strParam[2];
		strParam[0] = strRecv.Left(pos);
		strParam[1] = strRecv.Mid(pos + 1);
		AfxGetMainWnd()->SendMessage(FCCM_LOADPROJECT, FCPV_PROJECT, (LPARAM)(LPCTSTR)strParam[0]);
		AfxGetMainWnd()->SendMessage(FCCM_LOADPROJECT, FCPV_DLGFILE, (LPARAM)(LPCTSTR)strParam[1]);
	}
	
	return TRUE;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = FALSE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = FALSE; // 在选项卡区域的右边缘启用文档菜单
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	//菜单栏
	if (!m_wndMenuBar.Create (this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	// 允许用户定义的工具栏操作: 
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);
	CMFCToolBarComboBoxButton::SetFlatMode();

	//创建状态栏
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_wndStatusBar.SetPaneStyle(0, SBPS_STRETCH);
	m_wndStatusBar.SetProgressStepUpdateTimes(100);

	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);

	// 创建工具条
	if (!CreateToolBars())
	{
		TRACE0("未能创建工具条\n");
		return -1;
	}

	hideToolBars();

	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 加载菜单项图像(不在任何标准工具栏上): 
	//CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_MAN_WINDOWS, TRUE);
	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE), ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	// 将文档名和应用程序名称在窗口标题栏上的顺序进行交换。这
	// 将改进任务栏的可用性，因为显示的文档名带有缩略图。
	ModifyStyle(0, FWS_PREFIXTITLE);

	// 查询结果对话框
	m_dlgDetectResult.Create(IDD_OVERLAPPOINT_RESULT);

	// 创建Accu对话框
	m_wndAccu.Create(IDD_ACCU_DLG3,this);
	m_wndAccu.ShowWindow(SW_HIDE);

	// 外接设备管理器
	m_deviceDriver.Init();
	AddMenuCMDToDevice(IDR_EDITBATYPE);
	AddMenuCMDToDevice(IDR_POPUP_SNAPSTATUS);
	AddMenuCMDToDevice(IDR_POPUP_STEREO);
	AddMenuCMDToDevice(IDR_POPUP_FUNC);
	m_deviceDriver.AddWnd(GetSafeHwnd());
	m_deviceDriver.Start();
	
	CString strSensitive;
	for(int i=0; i< m_deviceDriver.GetDeviceCount(); i++)
	{
		CVDDevice* pDevice = m_deviceDriver.GetDevice(i);
		if( !pDevice )continue;
		
		const SOFTWARE_CONFIG* pConfg = pDevice->GetSoftConfig();
		if( pConfg && pConfg->state==DEVICE_STATE_ENABLE && pConfg->state==DEVICE_SUPPORTMOVE )
		{
			strSensitive.Format(  "X:%0.3f Y:%0.3f Z:%0.3f",
				pConfg->speed.x,
				pConfg->speed.y,
				pConfg->speed.z);
			break;
		}
	}
	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENSITIVE);
	m_wndStatusBar.SetPaneText(nIndex,strSensitive);
	CDC* pDC = m_wndStatusBar.GetDC();
	CSize ftwd = pDC->GetOutputTextExtent(strSensitive); 
	m_wndStatusBar.ReleaseDC(pDC);
	int width = 150;
	m_wndStatusBar.SetPaneWidth(nIndex,width>ftwd.cx?width:ftwd.cx);

	SetTimer(TIMERID_SAVE,10000,0);
	SetTimer(TIMERID_TRIALMSG,1000,0);

	GSetProgressHwnd(m_hWnd);

	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	CDockingManager::SetDockingMode(DT_SMART);


	return 0;
}

BOOL CMainFrame::CreateToolBars()
{
	CRect rcBoders(1, 1, 1, 1);
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, IDR_MAINFRAME) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return FALSE;      // 未能创建
	}
	m_wndToolBar.SetWindowText(StrFromResID(IDS_TOOLBAR_STANDARD));

	if (!m_wndToolbarView.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, IDR_VIEW) ||
		!m_wndToolbarView.LoadToolBar(IDR_VIEW))
	{
		TRACE0("Failed to create view toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarView.SetWindowText(StrFromResID(IDS_VIEW_TOOLBAR));


	if (!m_wndToolbarEdit.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, ID_VIEW_EDIT_TOOLBAR) ||
		!m_wndToolbarEdit.LoadToolBar(IDR_EDIT_BASE))
	{
		TRACE0("Failed to create view toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarEdit.SetWindowText(StrFromResID(IDS_EDIT_TOOLBAR));

	if (!m_wndToolbarEditContours.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, ID_VIEW_EDITCONTOURS_TOOLBAR) ||
		!m_wndToolbarEditContours.LoadToolBar(IDR_EDIT_CONTOURS))
	{
		TRACE0("Failed to create view toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarEditContours.SetWindowText(StrFromResID(IDS_EDITCONTOURS_TOOLBAR));

	if (!m_wndToolbarDrawPoint.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, IDR_DRAWPOINT_TYPE) ||
		!m_wndToolbarDrawPoint.LoadToolBar(IDR_DRAWPOINT_TYPE))
	{
		TRACE0("Failed to create resource toolbar\n");
		return FALSE;      // fail to create
	}

	if (!m_wndToolbarDrawLine.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, IDR_DRAWLINE_TYPE) ||
		!m_wndToolbarDrawLine.LoadToolBar(IDR_DRAWLINE_TYPE))
	{
		TRACE0("Failed to create resource toolbar\n");
		return FALSE;      // fail to create
	}

	if (!m_wndToolbarDrawCircle.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, IDR_DRAWCIRCLE_TYPE) ||
		!m_wndToolbarDrawCircle.LoadToolBar(IDR_DRAWCIRCLE_TYPE))
	{
		TRACE0("Failed to create resource toolbar\n");
		return FALSE;      // fail to create
	}

	if (!m_wndToolbarDrawSurface.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, IDR_DRAWSURFACE_TYPE) ||
		!m_wndToolbarDrawSurface.LoadToolBar(IDR_DRAWSURFACE_TYPE))
	{
		TRACE0("Failed to create resource toolbar\n");
		return FALSE;      // fail to create
	}

	//---------------------
	// Create Edit toolbar:
	//---------------------
	if (!m_wndToolbarDraw.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, ID_VIEW_DRAW_TOOLBAR) ||
		!m_wndToolbarDraw.LoadToolBar(IDR_CURVE_TYPE))
	{
		TRACE0("Failed to create linetype toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarDraw.SetWindowText(StrFromResID(IDS_DRAW_TOOLBAR));

	if (!m_wndToolbarLidar.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, ID_VIEW_EDITLIDAR_TOOLBAR) ||
		!m_wndToolbarLidar.LoadToolBar(IDR_EDIT_LIDAR))
	{
		TRACE0("Failed to create view toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarLidar.SetWindowText(StrFromResID(IDS_LIDAREDIT_TOOLBAR));

	if (!m_wndToolbarVisibilitySwitch.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, ID_VIEW_VISIBILITY_SWITCH) ||
		!m_wndToolbarVisibilitySwitch.LoadToolBar(IDR_VISIBILITY_SWITCH))
	{
		TRACE0("Failed to create view toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarVisibilitySwitch.SetWindowText(StrFromResID(IDS_VISIBILITY_SWITCH_TOOLBAR));

	if (!m_wndToolbarEditVect.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, ID_VIEW_EDITVECT_TOOLBAR) ||
		!m_wndToolbarEditVect.LoadToolBar(IDR_EDIT_VECT))
	{
		TRACE0("Failed to create view toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarEditVect.SetWindowText(StrFromResID(IDS_EDITVECT_TOOLBAR));

	if (!m_wndToolbarEditAdvanced.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, ID_VIEW_EDITADVANCED_TOOLBAR) ||
		!m_wndToolbarEditAdvanced.LoadToolBar(IDR_EDIT_ADVANCED))
	{
		TRACE0("Failed to create view toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarEditAdvanced.SetWindowText(StrFromResID(IDS_EDITADVANCED_TOOLBAR));

	if (!m_wndToolBarTestContour.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, ID_VIEW_TESTCONTOUR) ||
		!m_wndToolBarTestContour.LoadToolBar(IDR_EDIT_TESTCONTOURS))
	{
		TRACE0("Failed to create view toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolBarTestContour.SetWindowText(StrFromResID(IDS_EDIT_TESTCONTOURS));

	if (!m_wndToolbarLayer.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, IDR_LAYER) ||
		!m_wndToolbarLayer.LoadToolBar(IDR_LAYER))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarLayer.SetWindowText(StrFromResID(IDS_LAYER_TOOLBAR));

	if (!m_wndToolbarFtr.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, IDR_FTRATT) ||
		!m_wndToolbarFtr.LoadToolBar(IDR_FTRATT))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarFtr.SetWindowText(StrFromResID(IDS_FTR_TOOLBAR));

	//立体模型放大工具栏
	if (!m_wndToolbarStereoMagnify.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, ID_VIEW_STEREOMAGNIFY) ||
		!m_wndToolbarStereoMagnify.LoadToolBar(IDR_TOOLBAR_STEREOMODEL_MAGNIFY))
	{
		TRACE0("Failed to create view toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarStereoMagnify.SetWindowText(StrFromResID(IDS_STEREO_MAGNIFY));

	//拓扑
	if (!m_wndToolbarTopo.CreateEx(this, TBSTYLE_FLAT, dwStyle, rcBoders, ID_VIEW_TOPO) ||
		!m_wndToolbarTopo.LoadToolBar(IDR_TOOLBAR_TOPO))
	{
		TRACE0("Failed to create view toolbar\n");
		return FALSE;      // fail to create
	}
	m_wndToolbarTopo.SetWindowText(StrFromResID(IDS_CMDNAME_TOPO_BUILD));


	m_wndToolbarEditVect.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarEditAdvanced.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarEditContours.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarEdit.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarView.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarDraw.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarLidar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarVisibilitySwitch.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolBarTestContour.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarLayer.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarFtr.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarStereoMagnify.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
	m_wndToolbarTopo.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable	
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);

	m_wndToolbarDraw.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolbarView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolbarDrawPoint.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolbarDrawLine.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolbarDrawCircle.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolbarDrawSurface.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolbarTopo.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolbarVisibilitySwitch.EnableDocking(CBRS_ALIGN_ANY);

	/*
	** 曾进翀2018.1.26 改款的自我定义的标题栏
	*/
	CRect cltRect;
	/*GetWindowRect(&cltRect);

	CRect cltTitleRect(cltRect);
	cltTitleRect.bottom = cltTitleRect.top + 40;
	DWORD dwTitleStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;

	if (!m_duiTitle.Create(_T(""), this, cltTitleRect, TRUE, IDS_CMDPLANE_CHECKPCODE, dwTitleStyle))
	{
	TRACE0("Failed to create myTitle\n");
	return -1;
	}

	m_duiTitle.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_duiTitle);*/

	/*
	** 曾进翀2018.1.26 改款的自定义侧边栏
	*/

	/*GetWindowRect(&cltRect);
	CRect cltTreeRect(cltRect);
	cltTreeRect.right = cltTreeRect.left + 80;
	DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | CBRS_LEFT| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;*/

	//if (!m_duiTreeView.Create(_T(""), this, cltTreeRect,
	//	TRUE,
	//	IDS_CHK_OBJINFO,
	//	dwTreeStyle))
	//{
	//	TRACE0("Failed to create left side bar\n");
	//	return FALSE;      // fail to create
	//}

	//m_duiTreeView.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_duiTreeView);

	/*
	** 曾进翀2018.1.26 改款的自我定义的工具条
	*/
	GetClientRect(&cltRect);
	CRect cltPaneRect(cltRect);
	cltPaneRect.bottom = cltPaneRect.top + 155;
	DWORD dwToolStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;

	if (!m_duiToolBar.Create(_T(""), this, cltPaneRect, TRUE, IDS_TOOLBAR_CUSTOMIZE, dwToolStyle))
	{
		TRACE0("Failed to create myToolbar\n");
		return -1;
	}

	m_duiToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_duiToolBar);

	/*m_wndToolbarTopo.EnableDocking(CBRS_ALIGN_ANY);*/

	//m_wndToolbarVisibilitySwitch.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolbarEditContours.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolbarEditVect.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolbarEdit.EnableDocking(CBRS_ALIGN_ANY);
	//
	//m_wndToolbarEditAdvanced.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolbarFtr.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolbarLayer.EnableDocking(CBRS_ALIGN_ANY);
	//
	//m_wndToolbarStereoMagnify.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolbarLidar.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBarTestContour.EnableDocking(CBRS_ALIGN_ANY);

	/*DockPane(&m_wndToolbarDraw);
	DockPaneLeftOf(&m_wndToolbarView, &m_wndToolbarDraw);
	DockPaneLeftOf(&m_wndToolBar, &m_wndToolbarView);

	DockPane(&m_wndToolbarTopo);
	DockPaneLeftOf(&m_wndToolbarVisibilitySwitch, &m_wndToolbarTopo);
	DockPaneLeftOf(&m_wndToolbarEditContours, &m_wndToolbarVisibilitySwitch);
	DockPaneLeftOf(&m_wndToolbarEditVect, &m_wndToolbarEditContours);
	DockPaneLeftOf(&m_wndToolbarEdit, &m_wndToolbarEditVect);

	DockPane(&m_wndToolbarEditAdvanced);
	DockPaneLeftOf(&m_wndToolbarFtr, &m_wndToolbarEditAdvanced);
	DockPaneLeftOf(&m_wndToolbarLayer, &m_wndToolbarEditAdvanced);

	DockPane(&m_wndToolbarStereoMagnify);
	DockPaneLeftOf(&m_wndToolbarLidar, &m_wndToolbarStereoMagnify);
	DockPaneLeftOf(&m_wndToolBarTestContour, &m_wndToolbarLidar);*/
	//ShowPane(&m_wndToolbarStereoMagnify, FALSE, FALSE, FALSE);
	//ShowPane(&m_wndToolbarLidar, FALSE, FALSE, FALSE);
	//ShowPane(&m_wndToolBarTestContour, FALSE, FALSE, FALSE);

	if (((CEditBaseApp*)AfxGetApp())->IsSupportPlugs())
	{
		CUserCustomToolbar::LoadFromPulgins(this, ((CEditBaseApp*)AfxGetApp())->m_arrPlugObjs, m_arrPlugsToolBar);
	}

	if (m_arrPlugsToolBar.GetSize() > 0)
	{
		for (int i = 0; i < m_arrPlugsToolBar.GetSize(); i++)
		{
			CUserCustomToolbar *pTool = (CUserCustomToolbar*)m_arrPlugsToolBar.GetAt(i);
			pTool->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
			pTool->EnableDocking(CBRS_ALIGN_ANY);
		}
	}

	return TRUE;
}

void CMainFrame::hideToolBars()
{
	m_wndMenuBar.ShowPane(FALSE, FALSE, TRUE);
}

void CMainFrame::DockPlugPane()
{
	for (int i = 0; i < m_arrPlugsToolBar.GetSize(); i++)
	{
		CUserCustomToolbar *pTool = (CUserCustomToolbar*)m_arrPlugsToolBar.GetAt(i);
		DockPane(pTool);
	}

	for (int i = 0; i < m_arrPluginDockingControlBars.GetSize(); i++)
	{
		DockingControlBar bar = m_arrPluginDockingControlBars[i];
		CDockablePane *pBar = bar.pBar;

		int nBarType = bar.nType;
		CDockablePane* pTabbedBar = NULL;
		if (nBarType == 0)
		{
			pBar->AttachToTabWnd(&m_wndProjectView, DM_SHOW, TRUE, &pTabbedBar);
		}
		else if (nBarType == 1)
		{
			pBar->AttachToTabWnd(&m_wndCollectionView, DM_SHOW, TRUE, &pTabbedBar);
		}
		else if (nBarType == 2)
		{
			pBar->AttachToTabWnd(&m_wndPropertiesBar, DM_SHOW, TRUE, &pTabbedBar);
		}
		else if (nBarType == 3)
		{
			pBar->AttachToTabWnd(&m_wndOpSettingBar, DM_SHOW, TRUE, &pTabbedBar);
		}
		else if (nBarType == 4)
		{
			pBar->AttachToTabWnd(&m_wndOutputBar, DM_SHOW, TRUE, &pTabbedBar);
		}
	}
}

BOOL CMainFrame::CreateDockingWindows()
{
	// 工程栏
	if (!m_wndProjectView.Create(StrFromResID(IDS_PROJECT_TITLE), this, CRect(0, 0, 200, 200),
		TRUE,
		ID_VIEW_PROJECT,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Record View bar\n");
		return FALSE;      // fail to create
	}
	//图层管理
	if (!m_wndFtrLayerView.Create(StrFromResID(IDS_LAYERVIEW_TITLE), this, CRect(0, 0, 200, 200),
		TRUE,
		ID_VIEW_LAYER,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		return FALSE;
	}
	// 采集栏
	if (!m_wndCollectionView.Create(StrFromResID(IDS_COLLECT_TITLE), this, CRect(0, 0, 200, 200),
		TRUE,
		ID_VIEW_COLLECTION,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Collection View bar\n");
		return FALSE;      // fail to create
	}
	// 属性窗口
	if (!m_wndPropertiesBar.Create(StrFromResID(IDS_PROPERTY_TITLE), this, CRect(0, 0, 200, 200),
		TRUE,
		ID_VIEW_PROPERTIES,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties Bar\n");
		return FALSE;		// fail to create
	}
	// 设置窗口
	if (!m_wndOpSettingBar.Create(StrFromResID(IDS_OPSETTING_TITLE), this, CRect(0, 0, 200, 200),
		TRUE,
		ID_VIEW_INPUTSETTING,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create OpSetting Bar\n");
		return FALSE;		// fail to create
	}
	// 输出窗口
	if (!m_wndOutputBar.Create(StrFromResID(IDS_OUTPUT_TITLE), this, CRect(0, 0, 200, 100),
		FALSE,
		ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output Bar\n");
		return FALSE;		// fail to create
	}
	//检查方案
	if (!m_wndCheckSchemeViewBar.Create(StrFromResID(IDS_CHK_SCHEME_LIST), this, CRect(0, 0, 200, 200),
		TRUE,
		ID_CHK_SCHEME_BAR,
		WS_CHILD/* | WS_VISIBLE*/ | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		return FALSE;
	}
	//检查结果
	if (!m_wndCheckResultBar.Create(StrFromResID(IDS_CHK_RESULT_LIST), this, CRect(0, 0, 200, 200),
		TRUE,
		ID_CHK_RESULT_BAR,
		WS_CHILD /*| WS_VISIBLE*/ | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		return FALSE;
	}
	//检查点精度分析
	if (!m_wndTestDlgBar.Create(StrFromResID(IDS_TESTDLGBAR_TITLE), this, CRect(0, 0, 300, 600),
		TRUE,
		ID_TESTDLG_BAR,
		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Record View bar\n");
		return FALSE;      // fail to create
	}
	
	if (((CEditBaseApp*)AfxGetApp())->IsSupportPlugs())
	{
		LoadPluginDockingControlBars();
	}

	m_wndProjectView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndFtrLayerView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCollectionView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndPropertiesBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndOpSettingBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndOutputBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCheckSchemeViewBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCheckResultBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndTestDlgBar.EnableDocking(CBRS_ALIGN_ANY);

	m_showImgBar.EnableDocking(CBRS_ALIGN_ANY);

	DockPane(&m_wndProjectView);
	DockPane(&m_wndCollectionView);
	m_wndFtrLayerView.DockToWindow(&m_wndCollectionView, CBRS_BOTTOM);

	m_wndPropertiesBar.AttachToTabWnd(&m_wndFtrLayerView, DM_SHOW, FALSE, NULL);
	m_wndProjectView.AttachToTabWnd(&m_wndFtrLayerView, DM_SHOW, FALSE, NULL);

	DockPane(&m_wndOutputBar);

	//DockPane(&m_wndPropertiesBar);

	//m_wndProjectView.EnableGripper(FALSE);
	//m_wndCollectionView.EnableGripper(FALSE);
	//m_wndPropertiesBar.EnableGripper(FALSE);
	//m_wndProjectView.GetDefaultPaneDivider()->SetAutoHideMode(FALSE);
	//m_wndCollectionView.GetDefaultPaneDivider()->SetAutoHideMode(FALSE);
	//m_wndPropertiesBar.GetDefaultPaneDivider()->SetAutoHideMode(FALSE);

	//DockPane(&m_wndOpSettingBar);
	//DockPane(&m_wndOutputBar);
	//DockPane(&m_wndCheckSchemeViewBar);
	//DockPane(&m_wndCheckResultBar);

	//m_wndOpSettingBar.DockToWindow(&m_wndPropertiesBar, CBRS_BOTTOM);
	//m_wndCollectionView.DockToWindow(&m_wndProjectView, CBRS_BOTTOM);
	//m_wndOutputBar.DockToFrameWindow(CBRS_BOTTOM);

	//CDockablePane* pTabbedBar = NULL;
	//m_wndFtrLayerView.AttachToTabWnd(&m_wndProjectView, DM_SHOW, TRUE,&pTabbedBar);
	//m_wndCheckSchemeViewBar.AttachToTabWnd(&m_wndPropertiesBar, DM_SHOW, TRUE, &pTabbedBar);
	//m_wndCheckResultBar.AttachToTabWnd(&m_wndCheckSchemeViewBar, DM_SHOW, TRUE, &pTabbedBar);

	//ShowPane(&m_wndCheckSchemeViewBar, FALSE, FALSE, FALSE);
	//ShowPane(&m_wndCheckResultBar, FALSE, FALSE, FALSE);

	//for (int i = 0; i < m_arrPluginDockingControlBars.GetSize(); i++)
	//{
	//	DockingControlBar bar = m_arrPluginDockingControlBars[i];
	//	CDockablePane *pBar = bar.pBar;
	//	if (pBar)
	//	{
	//		pBar->EnableDocking(CBRS_ALIGN_ANY);
	//	}
	//}
	return TRUE;
}

void CMainFrame::AdjustPosUserToolBar()
{
	if( m_arrPlugsToolBar.GetSize()>0 )
	{
		for( int i=0; i<m_arrPlugsToolBar.GetSize(); i++)
		{
			CUserCustomToolbar *pTool = (CUserCustomToolbar*)m_arrPlugsToolBar.GetAt(i);
			DockPane(pTool);
		}
		for( i=m_arrPlugsToolBar.GetSize()-1; i>0; i--)
		{
			CUserCustomToolbar *pTool = (CUserCustomToolbar*)m_arrPlugsToolBar.GetAt(i);				
			DockPaneLeftOf((CUserCustomToolbar*)m_arrPlugsToolBar.GetAt(i - 1), pTool);
		}
	}
}

LRESULT CMainFrame::OnPrintStr(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR pstr = (LPCTSTR)lParam;
	if(pstr)
	{
		m_wndOutputBar.OutputMsg(pstr);
	}
	return 0;
}

LRESULT CMainFrame::OnUpdateAccuDlg(WPARAM wParam, LPARAM lParam)
{
	m_wndAccu.Show(wParam, lParam);
	return 0;
}

LRESULT CMainFrame::OnShowAccudlg(WPARAM wParam, LPARAM lParam)
{
	m_wndAccu.ShowWindow(wParam==0?SW_HIDE:SW_SHOW);
	return 0;
}

LRESULT CMainFrame::OnDeviceLFOOT(WPARAM wParam,LPARAM lParam)
{
	if( m_bPauseDevice )return 0;
	SendMessageToViews(WM_DEVICE_LFOOT,wParam,lParam);
//	AfxCallMessage(WM_DEVICE_LFOOT,wParam,lParam);
	return 0;
}


LRESULT CMainFrame::OnDeviceRFOOT(WPARAM wParam, LPARAM lParam)
{
	if( m_bPauseDevice )return 0;
	SendMessageToViews(WM_DEVICE_RFOOT,wParam,lParam);
	return 0;
}

LRESULT CMainFrame::OnDeviceMFOOT(WPARAM wParam, LPARAM lParam)
{
	if( m_bPauseDevice )return 0;
	SendMessageToViews(WM_DEVICE_MFOOT,wParam,lParam);
	return 0;
}


LRESULT CMainFrame::OnDeviceMOVE(WPARAM wParam, LPARAM lParam)
{
	MSG	msg;
	while( PeekMessage(&msg,m_hWnd,WM_DEVICE_MOVE,WM_DEVICE_MOVE,PM_REMOVE) );
	
	if( m_bPauseDevice )return 0;
	
	VDSEND_DATA* pData =(VDSEND_DATA*)lParam;
	if( pData->x==0 && pData->y==0 && pData->z==0 )return 1;
	SendMessageToViews(WM_DEVICE_MOVE,0x0008,lParam);
//	AfxCallMessage(WM_DEVICE_MOVE,0x0008,lParam);
	return 0;
}


LRESULT CMainFrame::OnDeviceKEY(WPARAM wParam, LPARAM lParam)
{
	if( m_bPauseDevice )return 0;
	
	if( lParam!=0 )return 0;
	
	SendMessage( WM_COMMAND,wParam,0 );
	
	return 0;
}

LRESULT CMainFrame::OnUpdateViewList(WPARAM wParam, LPARAM lParam)
{
	BOOL bIsClosed = (int)wParam;
	void* pView = (void*)lParam;
	if (bIsClosed)
	{
		for (int i=0;i<m_ptrListViews.GetSize();i++)
		{
			if (m_ptrListViews[i]==pView)
			{
				m_ptrListViews.RemoveAt(i);
				return 0;
			}
		}
	}
	else
	{
		for (int i=0;i<m_ptrListViews.GetSize();i++)
		{
			if (m_ptrListViews[i]==pView)
			{
				m_ptrListViews.RemoveAt(i);
				m_ptrListViews.InsertAt(0,pView);
				return 0;
			}
		}
		m_ptrListViews.InsertAt(0,pView);
	}
	return 0;
}

LRESULT CMainFrame::OnCustomDlgClose(WPARAM wParam, LPARAM lParam)
{
	//恢复快捷键表
	int num = ::CopyAcceleratorTable(m_hAccelTable,NULL,0);
	LPACCEL pNewTable = new ACCEL[num];
	if( pNewTable )
	{
		memset(pNewTable,0,sizeof(ACCEL)*num);
		::CopyAcceleratorTable(m_hAccelTable,pNewTable,num);
		int pos=0;
		for( int i=0; i<num; i++)
		{
			if( pNewTable[i].cmd==ID_COMMAND_INNERKEY )continue;
			if( i!=pos )pNewTable[pos] = pNewTable[i];
			pos++;
		}
		::DestroyAcceleratorTable(m_hAccelTable);
		m_hAccelTable = ::CreateAcceleratorTable(pNewTable,pos);
		delete[] pNewTable;
	}
	
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	pApp->SaveState(this);
	return 0;
}


LRESULT CMainFrame::OnLoadRecentCmd(WPARAM wParam, LPARAM lParam)
{
	CRecentCmd *pRecentcmd = (CRecentCmd *)lParam;
	
	CUIFToolbarComboBoxButtonEx comboButton(ID_DUMMY_VIEW_RECENTCMDS,
		GetCmdMgr()->GetCmdImage(ID_DUMMY_VIEW_RECENTCMDS, FALSE),
		CBS_DROPDOWNLIST, 200);
	
	comboButton.RemoveAllItems();	
	
	CString str;
	comboButton.AddItem((LPCTSTR)StrFromResID(IDS_SELECTRECENTCMD_TITLE));
	CRecentCmd::CmdItem cmditem;
	for (int i=0;i<pRecentcmd->GetCmdsCount();i++)
	{
		cmditem = pRecentcmd->GetCmdAt(i);
		str.Format("%s ",cmditem.name);//全中文的字符串高度有问题,加一个空格来解决
		comboButton.AddItem(str, cmditem.id);
	}

	if(pRecentcmd->GetCmdsCount()>0)
		comboButton.SelectItem (0,FALSE);
	
	comboButton.SetDropDownHeight(300);
	comboButton.SetFlatMode();
	m_wndToolbarView.ReplaceButton (ID_DUMMY_VIEW_RECENTCMDS,
		comboButton,TRUE);

	DockPaneLeftOf(&m_wndToolBar,&m_wndToolbarView);	
	m_wndToolbarView.AdjustLayout();
	return 1;
}

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog ();
}

LRESULT CMainFrame::OnAddRecentCmd(WPARAM wParam, LPARAM lParam)
{
	CRecentCmd *pRecentcmd = (CRecentCmd *)lParam;
	CMFCToolBarComboBoxButton *comboButton = GetCommandButton(ID_DUMMY_VIEW_RECENTCMDS);
	if (comboButton)
	{
		comboButton->RemoveAllItems();	
		
		CString str;	
		CRecentCmd::CmdItem cmditem;
		for (int i=0;i<pRecentcmd->GetCmdsCount();i++)
		{
			cmditem = pRecentcmd->GetCmdAt(i);			
			str.Format("%s ",cmditem.name);
			comboButton->AddItem(str, cmditem.id);
		}
		if(pRecentcmd->GetCmdsCount()>0)
			comboButton->SelectItem (0,TRUE);
	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~FWS_ADDTOTITLE; //禁用MFC的默认标题功能
	cs.lpszName = _T("");

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

LRESULT CMainFrame::OnToolbarReset(WPARAM wp,LPARAM)
{
	UINT uiToolBarId = (UINT) wp;
	
	switch (uiToolBarId)
	{
	case IDR_MAINFRAME:
		{
			//--------------------------------------
			// Replace "Find..." button by combobox:
			//--------------------------------------
//			m_wndToolBar.ReplaceButton (ID_EDIT_FIND, CFindComboButton ());
			
			//-------------------------------------
			// Replace "Undo" button by menubutton:
			//-------------------------------------
			m_wndToolBar.ReplaceButton (ID_EDIT_UNDO, CUndoButton (ID_EDIT_UNDO, _T("&Undo")));
			m_wndToolBar.ReplaceButton (ID_EDIT_REDO, CUndoButton (ID_EDIT_REDO, _T("&Redo")));
		}
		break;
		
	case IDR_VIEW:
		{
			//-----------------------------------------------
			// Replace button by combobox:
			//-----------------------------------------------
			
// 			CMDIChildWnd *pFrame = MDIGetActive();
// 			if( pFrame )
// 			{
// 				CDocument *pDoc = pFrame->GetActiveDocument();
// 				if( pDoc )pDoc->OnCmdMsg(ID_RECENTCLASSUPDATE,0,0,0);
// 			}
			
			/*
			CUIFToolbarComboBoxButtonEx comboButtonConfigFtr (ID_DUMMY_VIEW_FTRCLASS,
			CImageHash::GetImageOfCommand (ID_DUMMY_VIEW_FTRCLASS, FALSE),
			CBS_DROPDOWNLIST);
			comboButtonConfigFtr.AddItem (_T("NULL"));
			comboButtonConfigFtr.SelectItem (0);
			m_wndToolbarView.ReplaceButton (ID_DUMMY_VIEW_FTRCLASS, comboButtonConfigFtr);	
			*/
			
// 			CUIFToolbarComboBoxButtonEx comboButtonConfig (ID_DUMMY_VIEW_TYPE,
// 				CImageHash::GetImageOfCommand (ID_DUMMY_VIEW_TYPE, FALSE),
// 				CBS_DROPDOWNLIST);
// 			comboButtonConfig.AddItem (StrFromResID(IDS_VIEW_ALL));
// 			comboButtonConfig.AddItem (StrFromResID(IDS_VIEW_MAIN));
// 			comboButtonConfig.AddItem (StrFromResID(IDS_VIEW_CURLAYER));	
// 			comboButtonConfig.AddItem (StrFromResID(IDS_VIEW_CURFID));	
// 			comboButtonConfig.SelectItem (0);
// 			m_wndToolbarView.ReplaceButton (ID_DUMMY_VIEW_TYPE, comboButtonConfig);
			
			m_wndToolbarView.ReplaceButton (ID_DUMMY_INSERT_DRAWPOINT,
				CMFCDropDownToolbarButton (_T("Draw Point"), &m_wndToolbarDrawPoint));
			
			m_wndToolbarView.ReplaceButton (ID_DUMMY_INSERT_DRAWLINE,
				CMFCDropDownToolbarButton(_T("Draw Line"), &m_wndToolbarDrawLine));
			
			m_wndToolbarView.ReplaceButton (ID_DUMMY_INSERT_DRAWCIRCLE,
				CMFCDropDownToolbarButton(_T("Draw Circle"), &m_wndToolbarDrawCircle));

			m_wndToolbarView.ReplaceButton (ID_DUMMY_INSERT_DRAWSURFACE,
				CMFCDropDownToolbarButton(_T("Draw Surface"), &m_wndToolbarDrawSurface));

			
			CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
			pApp->UpdateAllDocsByCmd(ID_DOCINFORELOAD);
		}
		break;
	}
	
	return 0;
}


LRESULT CMainFrame::OnUpdateIndicator(WPARAM wp, LPARAM lp)
{
	if( lp )
	{
		int nIndex = m_wndStatusBar.CommandToIndex(wp);
		if( nIndex<0 )return 0;
		
		m_wndStatusBar.SetPaneText(nIndex,(LPCTSTR)lp);
	}
	
	return 0;
}

void CMainFrame::OnViewOpSetting() 
{
	BOOL bAutoHideMode = m_wndOpSettingBar.IsAutoHideMode ();
	BOOL bTabbed = m_wndOpSettingBar.IsTabbed ();
	
	ShowPane(&m_wndOpSettingBar,
		!m_wndOpSettingBar.IsVisible(),
		FALSE, bAutoHideMode || !bTabbed);
	RecalcLayout ();
}


void CMainFrame::OnUpdateViewOpSetting(CCmdUI* pCmdUI) 
{
	if (pCmdUI->m_pOther != NULL &&
		!pCmdUI->m_pOther->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar)))
	{
		pCmdUI->SetCheck (m_wndOpSettingBar.IsVisible ());	
	}
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}
	
	EnableMDITabs (TRUE, FALSE, CMFCBaseTabCtrl::LOCATION_TOP, TRUE);
	
	//----------------------------------------------------
	// Enable customization button fore all user toolbars:
	//----------------------------------------------------
	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, StrFromResID(IDS_TOOLBAR_CUSTOMIZE));
		}
	}
	ShowPane(&m_showImgBar, FALSE, FALSE, FALSE);

	
	return TRUE;
}


BOOL CMainFrame::OnShowPopupMenu (CMFCPopupMenu* pMenuPopup)
{
    CMDIFrameWndEx::OnShowPopupMenu (pMenuPopup);
	
    if (pMenuPopup == NULL)
	{
		return TRUE;
	}
	
    if (pMenuPopup->GetMenuBar ()->CommandToIndex (ID_VIEW_TOOLBARS) >= 0)
    {
		if (CMFCToolBar::IsCustomizeMode ())
		{
			return FALSE;
		}
		
		pMenuPopup->RemoveAllItems ();
		
		CMenu menu;
		VERIFY(menu.LoadMenu (IDR_POPUP_TOOLBAR));
		
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		
		SetupToolbarMenu (*pPopup, ID_VIEW_USER_TOOLBAR1, ID_VIEW_USER_TOOLBAR10);
		pMenuPopup->GetMenuBar ()->ImportFromMenu (*pPopup, TRUE);
    }
	
	return TRUE;
}

LRESULT CMainFrame::OnReCreateAllFtrs(WPARAM wp, LPARAM lp)
{
	POSITION curTemplatePos = AfxGetApp()->GetFirstDocTemplatePosition();
	
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = AfxGetApp()->GetNextDocTemplate(curTemplatePos);
		
		POSITION curDocPos = curTemplate->GetFirstDocPosition();
		
		while (curDocPos != NULL)
		{
			CDocument *pDoc = curTemplate->GetNextDoc(curDocPos);
			
			if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
			{
				((CDlgDoc*)pDoc)->ReCreateAllFtrs();
			}
		}
	}
	
	return 0;
}
LRESULT CMainFrame::OnUpdateSymScale(WPARAM wp, LPARAM lp)
{
	POSITION curTemplatePos = AfxGetApp()->GetFirstDocTemplatePosition();
	
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = AfxGetApp()->GetNextDocTemplate(curTemplatePos);

		POSITION curDocPos = curTemplate->GetFirstDocPosition();

		while (curDocPos != NULL)
		{
			CDocument *pDoc = curTemplate->GetNextDoc(curDocPos);
			
			if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )
			{
				((CDlgDoc*)pDoc)->UpdateSymDrawScale();
			}
		}
	}

	return 0;
}

LRESULT CMainFrame::OnChangeActiveTab (WPARAM wp, LPARAM lp)
{
	CMDIChildWnd *pMDIChild = MDIGetActive();
	if (!pMDIChild || !pMDIChild->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) )
		return 0;
	CView *pView = pMDIChild->GetActiveView();

	if( pView )
	{
		if( pView->IsKindOf(RUNTIME_CLASS(CBaseView)) )
		{
			CBaseView *pTView = (CBaseView*)pView;
			CDlgDoc *pDoc = (CDlgDoc*)pTView->GetDocument();
			if (!pDoc)
			{
				return 0;
			}

			if( pDoc->IsKindOf(RUNTIME_CLASS(CDlgCellDoc)) )
				return 0;

			CDlgDataSource *pDS = pDoc->GetDlgDataSource();
			m_wndTestDlgBar.ChangeDlgDoc(pDoc);
			//m_wndToolbarLayer.ChangeDlgDoc(pDoc);
//			m_wndTestDlgBar.SendMessage(WM_COMMAND,ID_UPDATEDATA,0);
			if (pDS == NULL)
			{
				SendMessage (FCCM_COLLECTIONVIEW,0,0);
				SendMessage(FCCM_REFRESHCHKSCHBAR,1,0);

				SendMessage(FCCM_INITLAYERTOOL, WPARAM(1), LPARAM(0));
				SendMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(0));
				return 0;
			}
			
			CConfigLibManager *pCfgLibManager = gpCfgLibMan;

			ConfigLibItem *pData = &pCfgLibManager->GetConfigLibItemByScale(pDS->GetScale());

			if (pDS)
			{
// 				PT_LEN = pDS->GetScale()*0.001;
// 				DIR_LEN = pDS->GetScale()*0.01;

				SetDataSourceScale(pDS->GetScale());
				//防止频繁更新
				if (m_wndCollectionView.m_config.pScheme != pData->pScheme)
				{
					// 更新主界面索引界面
 					AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,WPARAM(1),LPARAM(pData));
// 					AfxGetMainWnd()->SendMessage(FCCM_SYMBOLLIBVIEW,WPARAM(0),LPARAM(&(pData->m_SymbolLib)));
// 					AfxGetMainWnd()->SendMessage (FCCM_GEOMETRYVIEW ,WPARAM(0),LPARAM(pData));					
				}

				//设置缺省图元库和线型库
// 				CCellDefLib *pCellLib = GetCellDefLib();
// 				CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
				SetCellDefLib(pCfgLibManager->GetCellDefLib(pDS->GetScale()));
				SetBaseLineTypeLib(pCfgLibManager->GetBaseLineTypeLib(pDS->GetScale()));
				SetLibSourceID((DWORD_PTR)pDS);
				CCommand *pCom = pDoc->GetCurrentCommand();
				if(pCom)
				{
					pCom->RefreshParams();
					if (pCom->IsKindOf(RUNTIME_CLASS(CDrawCommand)))
					{	
						((CDrawCommand*)pCom)->RefreshPropertiesPanel();
					}
					else
						SendMessage(FCCM_MODIFY_PROPERTY_UI,0,(LPARAM)NULL);
				}
				else
				{
					pDoc->UIRequest(UIREQ_TYPE_SHOWUIPARAM, 0);
					pDoc->RefreshSelProperties();
				}
			}

			SendMessage(FCCM_INITLAYERTOOL, WPARAM(1), LPARAM(pDoc));
			SendMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(pDoc));

			if (pDS)
			{
				static CDlgDataSource *plastDS = NULL;
				if (plastDS != pDS)
				{
					SendMessage(FCCM_INITLAYVIEWBAR, WPARAM(1), LPARAM(pDoc));
					plastDS = pDS;
				}
			}
		}
	}
	return 0;
}

LRESULT CMainFrame::OnToolbarContextMenu(WPARAM,LPARAM lp)
{
	CPoint point (GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	
	CMenu menu;
	VERIFY(menu.LoadMenu (IDR_POPUP_CONTROLBAR));
	
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CString strName;
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (pDoc)
	{
		strName = pDoc->GetLastCmdName();
	}
 	CString strText;
	if( pPopup->GetMenuString(ID_LASTCOMMAND,strText,MF_BYCOMMAND)>0 )
	{
// 		CString strName = m_wndRecentCmdBar.LastCommandName();
		if( !strName.IsEmpty())
		{
			strText = strText + _T(":") + strName;
			pPopup->ModifyMenu(ID_LASTCOMMAND,MF_BYCOMMAND,ID_LASTCOMMAND,strText);
		}
	}
	
	SetupToolbarMenu (*pPopup, ID_VIEW_USER_TOOLBAR1, ID_VIEW_USER_TOOLBAR10);
	
	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
	pPopupMenu->SetAutoDestroy (FALSE);
	SetForegroundWindow();
	pPopupMenu->Create (this, point.x, point.y, pPopup->GetSafeHmenu ());
	
	return 0;
}

//不接受OnChar消息
LRESULT CMainFrame::OnKeyIn (WPARAM wParam, LPARAM lParam)
{	
	char t = char(wParam);

	if (t==VK_CONTROL || t==VK_MENU || t==VK_SHIFT)
		return 0;

	switch(wParam)
	{
	case VK_MULTIPLY: wParam = '*'; break;
	case VK_ADD: wParam = '+'; break;
	case VK_SUBTRACT: wParam = '-'; break;
	case VK_DECIMAL: wParam = '.'; break;
	case VK_DIVIDE: wParam = '/'; break;
	default:;			
	}
	if(wParam>=VK_NUMPAD0 && wParam<=VK_NUMPAD9 )
	{
		wParam = wParam-VK_NUMPAD0 + '0';
	}

	m_wndOutputBar.SendMessage(WM_CHARONE,wParam,lParam);
	return 1;
}

LRESULT CMainFrame::OnTranslateKeyInMsg(WPARAM wParam, LPARAM lParam)
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (pDoc)
	{
		pDoc->KeyDown(UINT(wParam),0);
	}
	
	return 1;
}

void CMainFrame::OnViewOutput() 
{
	BOOL bAutoHideMode = m_wndOutputBar.IsAutoHideMode ();
	BOOL bTabbed = m_wndOutputBar.IsTabbed ();
	
	ShowPane (&m_wndOutputBar,
		!m_wndOutputBar.IsVisible (),
		FALSE, bAutoHideMode || !bTabbed);
	RecalcLayout ();
}


void CMainFrame::OnUpdateViewOutput(CCmdUI* pCmdUI) 
{
	if (pCmdUI->m_pOther != NULL &&
		!pCmdUI->m_pOther->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar)))
	{
		pCmdUI->SetCheck (m_wndOutputBar.IsVisible ());	
	}
}

void CMainFrame::OnViewProperties() 
{
	// demonstartes how to make bar active in autohide mode and inactive when non-tabbed
	BOOL bAutoHideMode = m_wndPropertiesBar.IsAutoHideMode ();
	BOOL bTabbed = m_wndPropertiesBar.IsTabbed ();
	
	ShowPane (&m_wndPropertiesBar,
		!m_wndPropertiesBar.IsVisible (),
		FALSE, bAutoHideMode || !bTabbed);
	RecalcLayout ();
	
}

void CMainFrame::OnUpdateViewProperties(CCmdUI* pCmdUI) 
{
	if (pCmdUI->m_pOther != NULL &&
		!pCmdUI->m_pOther->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar)))
	{
		pCmdUI->SetCheck (m_wndPropertiesBar.IsVisible ());	
	}
	
}

LRESULT CMainFrame::OnModifyPropertyWnd(WPARAM wParam, LPARAM lParam)
{
	CPermanentExchanger *pFX = (CPermanentExchanger *)lParam;	
	m_wndPropertiesBar.SetPermaExchanger(pFX);	
	return TRUE;
}

LRESULT CMainFrame::OnUpdatePropertyWnd(WPARAM wParam, LPARAM lParam)
{
	CValueTable *pTable = (CValueTable*)lParam;	
	m_wndPropertiesBar.UpdateProperty(*pTable);	
	return TRUE;
}

LRESULT CMainFrame::OnLoadProject(WPARAM wParam, LPARAM lParam)
{
	m_wndProjectView.OnCallLoad(wParam,lParam);
	return TRUE;
}

LRESULT CMainFrame::OnUnInstallProject(WPARAM wParam, LPARAM lParam)
{
	return m_wndProjectView.UnInstall(wParam,lParam);
}

LRESULT CMainFrame::OnRefreshVecWin(WPARAM wParam, LPARAM lParam)
{
	return m_wndProjectView.RefreshVecWin(wParam,lParam);
}

LRESULT CMainFrame::OnRefreshCurrentStereoWin(WPARAM wParam, LPARAM lParam)
{
	return m_wndProjectView.RefreshCurrentStereoWin(wParam,lParam);
}


LRESULT CMainFrame::OnRefreshCurrentStereoMS(WPARAM wParam, LPARAM lParam)
{
	return m_wndProjectView.RefreshCurrentStereoMS(wParam,lParam);
}

LRESULT CMainFrame::OnRefreshVecflag(WPARAM wParam, LPARAM lParam)
{
	return m_wndProjectView.RefreshVecflag(wParam,lParam);
}

LRESULT CMainFrame::OnSwitchStereoMS(WPARAM wParam, LPARAM lParam)
{
	return m_wndProjectView.SwitchStereoMS(wParam,lParam);
}

LRESULT CMainFrame::OnGetToler(WPARAM wParam, LPARAM lParam)
{
	return m_wndProjectView.GetToler(wParam,lParam);
}

LRESULT CMainFrame::OnGetProject(WPARAM wParam, LPARAM lParam)
{
	return m_wndProjectView.OnGetProject(wParam,lParam);
}

void CMainFrame::OnUpdateSnapFtr(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateSensitive(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateCorrd(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateSnapStatus(CCmdUI* pCmdUI)
{
//  	BOOL bOpen	= AfxFindMessage(ID_INDICATOR_SNAP);
//   	pCmdUI->Enable(TRUE);
//  	
  	BOOL bOpen = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_ENABLE,TRUE);
  	pCmdUI->SetCheck(bOpen);
}

void CMainFrame::OnViewProject() 
{
	// demonstartes how to make bar active in autohide mode and inactive when non-tabbed
	BOOL bAutoHideMode = m_wndProjectView.IsAutoHideMode ();
	BOOL bTabbed = m_wndProjectView.IsTabbed ();
	
	ShowPane (&m_wndProjectView,
		!m_wndProjectView.IsVisible (),
		FALSE, bAutoHideMode || !bTabbed);
	
	RecalcLayout ();
}

void CMainFrame::OnUpdateViewProject(CCmdUI* pCmdUI) 
{
	if (pCmdUI->m_pOther != NULL &&
		!pCmdUI->m_pOther->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar)))
	{
		pCmdUI->SetCheck (m_wndProjectView.IsVisible ());	
	}
}

void CMainFrame::OnViewFtrLayer()
{
	BOOL bAutoHideMode = m_wndFtrLayerView.IsAutoHideMode ();
	BOOL bTabbed = m_wndFtrLayerView.IsTabbed ();
	
	ShowPane (&m_wndFtrLayerView,
		!m_wndFtrLayerView.IsVisible (),
		FALSE, TRUE);
	
// 	if( m_wndFtrLayerView.IsVisible() )
// 		SendMessage( FCCM_CALL_HELP, IDH_FO_PROJECT );
	RecalcLayout ();
}


void CMainFrame::OnUpdateViewFtrLayer(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_pOther != NULL &&
		!pCmdUI->m_pOther->IsKindOf (RUNTIME_CLASS (CMFCPopupMenuBar)))
	{
		pCmdUI->SetCheck (m_wndProjectView.IsVisible ());	
	}
}


void CMainFrame::OnViewCollection() 
{
	BOOL bAutoHideMode = m_wndCollectionView.IsAutoHideMode ();
	BOOL bTabbed = m_wndCollectionView.IsTabbed ();
	
	ShowPane (&m_wndCollectionView,
		!m_wndCollectionView.IsVisible (),
		FALSE, bAutoHideMode || !bTabbed);
	RecalcLayout ();	
}

LRESULT CMainFrame::OnXYZ(WPARAM wParam, LPARAM lParam)
{
	if (!lParam)return 0;
	PT_3D *pt = (PT_3D *)lParam;

	int nxy = floor(-log10(GraphAPI::g_lfDisTolerance)+0.5);
	int nz = floor(-log10(GraphAPI::g_lfZTolerance)+0.5);

	CString strFormat;
	strFormat.Format("%%.%df",nxy);
	
	CString strX, strY, strZ;
	strX.Format((LPCTSTR)strFormat,pt->x);
	strY.Format((LPCTSTR)strFormat,pt->y);
	
	strFormat.Format("%%.%df",nz);
	strZ.Format((LPCTSTR)strFormat,pt->z);
	
	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_COORD);
	CString strCoord;
	strCoord.Format("X:%s Y:%s Z:%s",(LPCTSTR)strX,(LPCTSTR)strY,(LPCTSTR)strZ);
	m_wndStatusBar.SetPaneText(nIndex,strCoord);
	CSize ftwd = 0;
	CDC *pDC = m_wndStatusBar.GetDC();
	ftwd = pDC->GetOutputTextExtent(strCoord);
	m_wndStatusBar.ReleaseDC(pDC);
	
	int width = 150;
	m_wndStatusBar.SetPaneWidth(nIndex,width > ftwd.cx ? width : ftwd.cx);
	
	return 0;
}

void CMainFrame::OnToolInputequipment() 
{
	m_deviceDriver.DoSettingDlg(m_hWnd, 0);

	CString strSensitive;
	for( int i=0; i< m_deviceDriver.GetDeviceCount(); i++)
	{
		CVDDevice* pDevice = m_deviceDriver.GetDevice(i);
		if( !pDevice )continue;
		
		const SOFTWARE_CONFIG* pConfg = pDevice->GetSoftConfig();
		if( pConfg && pConfg->state==DEVICE_STATE_ENABLE && pConfg->state==DEVICE_SUPPORTMOVE )
		{
			strSensitive.Format(  "X:%0.3f Y:%0.3f Z:%0.3f",
										pConfg->speed.x,
										pConfg->speed.y,
										pConfg->speed.z);
			break;
		}
	}

	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENSITIVE);
	m_wndStatusBar.SetPaneText(nIndex,strSensitive);

	CDC* pDC = m_wndStatusBar.GetDC();
	CSize ftwd = pDC->GetOutputTextExtent(strSensitive); 
	m_wndStatusBar.ReleaseDC(pDC);
	int width = 150;
	m_wndStatusBar.SetPaneWidth(nIndex,width > ftwd.cx ? width : ftwd.cx);
}

void CMainFrame::OnDevicePause() 
{
	m_bPauseDevice = !m_bPauseDevice;
}

void CMainFrame::OnUpdateDevicePause(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck (m_bPauseDevice);
}

void CMainFrame::OnAddSensitive()
{
	for( int i=0; i< m_deviceDriver.GetDeviceCount(); i++)
	{
		CVDDevice* pDevice = m_deviceDriver.GetDevice(i);
		if( !pDevice )continue;

		const SOFTWARE_CONFIG* pConfg = pDevice->GetSoftConfig();
		if( pConfg && pConfg->state==DEVICE_STATE_ENABLE )
		{
			SOFTWARE_CONFIG confg = *pConfg;

			if (fabs(confg.speed.x) > 0.1-1e-4  && fabs(confg.speed.x) < 2-1e-4)
			{
				if (confg.speed.x > 0)
				{
					confg.speed.x += 0.1;
				}
				else
				{
					confg.speed.x -= 0.1;
				}
				
			}
			else
			{
				confg.speed.x *= 2; 
			}
			ValidatePrecision(confg.speed.x);

			if (fabs(confg.speed.y) > 0.1-1e-4  && fabs(confg.speed.y) < 2-1e-4)
			{
				if (confg.speed.y > 0)
				{
					confg.speed.y += 0.1;
				}
				else
				{
					confg.speed.y -= 0.1;
				}
				
			}
			else
			{
				confg.speed.y *= 2; 
			}
			ValidatePrecision(confg.speed.y);

			pDevice->SetSoftConfig(&confg);
		}
	}

	m_deviceDriver.SaveDeviceSettings();

	CString strSensitive;
	for( i=0; i< m_deviceDriver.GetDeviceCount(); i++)
	{
		CVDDevice* pDevice = m_deviceDriver.GetDevice(i);
		if( !pDevice )continue;
		
		const SOFTWARE_CONFIG* pConfg = pDevice->GetSoftConfig();
		if( pConfg && pConfg->state==DEVICE_STATE_ENABLE && pConfg->state==DEVICE_SUPPORTMOVE )
		{
			strSensitive.Format(  "X:%0.3f Y:%0.3f Z:%0.3f",
										pConfg->speed.x,
										pConfg->speed.y,
										pConfg->speed.z);
			break;
		}
	}

	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENSITIVE);
	m_wndStatusBar.SetPaneText(nIndex,strSensitive);
	CSize ftwd = 0;
	CDC* pDC = m_wndStatusBar.GetDC();
	ftwd = pDC->GetOutputTextExtent(strSensitive);
	m_wndStatusBar.ReleaseDC(pDC);
	int width = 150;
	m_wndStatusBar.SetPaneWidth(nIndex,width > 	ftwd.cx ? width : 	ftwd.cx);
}

void CMainFrame::OnSubSensitive()
{
	for( int i=0; i< m_deviceDriver.GetDeviceCount(); i++)
	{
		CVDDevice* pDevice = m_deviceDriver.GetDevice(i);
		if(!pDevice) continue;
		const SOFTWARE_CONFIG* pConfg = pDevice->GetSoftConfig();
		if( pConfg && pConfg->state==DEVICE_STATE_ENABLE )
		{
			SOFTWARE_CONFIG confg = *pConfg;

			if (fabs(confg.speed.x) > 0.1+1e-4 && fabs(confg.speed.x) < 2+1e-4)
			{
				if (confg.speed.x > 0)
				{
					confg.speed.x -= 0.1;
				}
				else
				{
					confg.speed.x += 0.1;
				}
				
			}
			else
			{
				confg.speed.x *= 0.5; 
			}
			ValidatePrecision(confg.speed.x);
			
			if (fabs(confg.speed.y) > 0.1+1e-4 && fabs(confg.speed.y) < 2+1e-4)
			{
				if (confg.speed.y > 0)
				{
					confg.speed.y -= 0.1;
				}
				else
				{
					confg.speed.y += 0.1;
				}
				
			}
			else
			{
				confg.speed.y *= 0.5;
			}
			ValidatePrecision(confg.speed.y);
			
			pDevice->SetSoftConfig(&confg);
		}
	}

	m_deviceDriver.SaveDeviceSettings();

	CString strSensitive;
	for( i=0; i< m_deviceDriver.GetDeviceCount(); i++)
	{
		CVDDevice* pDevice = m_deviceDriver.GetDevice(i);
		if( !pDevice )continue;
		
		const SOFTWARE_CONFIG* pConfg = pDevice->GetSoftConfig();
		if( pConfg && pConfg->state==DEVICE_STATE_ENABLE && pConfg->state==DEVICE_SUPPORTMOVE )
		{
			strSensitive.Format(  "X:%0.3f Y:%0.3f Z:%0.3f",
										pConfg->speed.x,
										pConfg->speed.y,
										pConfg->speed.z);
			break;
		}
	}

	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENSITIVE);
	m_wndStatusBar.SetPaneText(nIndex,strSensitive);
	CSize ftwd = 0;
	CDC* pDC = m_wndStatusBar.GetDC();
	ftwd = pDC->GetOutputTextExtent(strSensitive);
	m_wndStatusBar.ReleaseDC(pDC);
	int width = 150;
	m_wndStatusBar.SetPaneWidth(nIndex,width > ftwd.cx ? width : ftwd.cx);}

void CMainFrame::OnAddSensitiveZ()
{
	for( int i=0; i< m_deviceDriver.GetDeviceCount(); i++)
	{
		CVDDevice* pDevice = m_deviceDriver.GetDevice(i);
		if( !pDevice )continue;

		const SOFTWARE_CONFIG* pConfg = pDevice->GetSoftConfig();
		if( pConfg && pConfg->state==DEVICE_STATE_ENABLE )
		{
			SOFTWARE_CONFIG confg = *pConfg;

			if (fabs(confg.speed.z) > 0.1-1e-4 && fabs(confg.speed.z) < 2-1e-4)
			{
				if (confg.speed.z > 0)
				{
					confg.speed.z += 0.1;
				}
				else
				{
					confg.speed.z -= 0.1;
				}
				
			}
			else
			{
				confg.speed.z *= 2;
			}
			ValidatePrecision(confg.speed.z);			

			pDevice->SetSoftConfig(&confg);
		}
	}

	m_deviceDriver.SaveDeviceSettings();

	CString strSensitive;
	for( i=0; i< m_deviceDriver.GetDeviceCount(); i++)
	{
		CVDDevice* pDevice = m_deviceDriver.GetDevice(i);
		if( !pDevice )continue;
		
		const SOFTWARE_CONFIG* pConfg = pDevice->GetSoftConfig();
		if( pConfg && pConfg->state==DEVICE_STATE_ENABLE && pConfg->state==DEVICE_SUPPORTMOVE )
		{
			strSensitive.Format(  "X:%0.3f Y:%0.3f Z:%0.3f",
										pConfg->speed.x,
										pConfg->speed.y,
										pConfg->speed.z);
			break;
		}
	}

	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENSITIVE);
	m_wndStatusBar.SetPaneText(nIndex,strSensitive);
	CSize ftwd = 0;
	CDC* pDC = m_wndStatusBar.GetDC();
	ftwd = pDC->GetOutputTextExtent(strSensitive);
	m_wndStatusBar.ReleaseDC(pDC);
	int width = 150;
	m_wndStatusBar.SetPaneWidth(nIndex,width > ftwd.cx ? width : ftwd.cx);}

void CMainFrame::OnSubSensitiveZ()
{
	for( int i=0; i< m_deviceDriver.GetDeviceCount(); i++)
	{
		CVDDevice* pDevice = m_deviceDriver.GetDevice(i);
		if(!pDevice) continue;
		const SOFTWARE_CONFIG* pConfg = pDevice->GetSoftConfig();
		if( pConfg && pConfg->state==DEVICE_STATE_ENABLE )
		{
			SOFTWARE_CONFIG confg = *pConfg;

			if (fabs(confg.speed.z) > 0.1+1e-4 && fabs(confg.speed.z) < 2+1e-4)
			{
				if (confg.speed.z > 0)
				{
					confg.speed.z -= 0.1;
				}
				else
				{
					confg.speed.z += 0.1;
				}
				
			}
			else
			{
				confg.speed.z *= 0.5;
			}
			ValidatePrecision(confg.speed.z);

			pDevice->SetSoftConfig(&confg);
		}
	}

	m_deviceDriver.SaveDeviceSettings();

	CString strSensitive;
	for( i=0; i< m_deviceDriver.GetDeviceCount(); i++)
	{
		CVDDevice* pDevice = m_deviceDriver.GetDevice(i);
		if( !pDevice )continue;
		
		const SOFTWARE_CONFIG* pConfg = pDevice->GetSoftConfig();
		if( pConfg && pConfg->state==DEVICE_STATE_ENABLE && pConfg->state==DEVICE_SUPPORTMOVE )
		{
			strSensitive.Format(  "X:%0.3f Y:%0.3f Z:%0.3f",
										pConfg->speed.x,
										pConfg->speed.y,
										pConfg->speed.z);
			break;
		}
	}

	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_SENSITIVE);
	m_wndStatusBar.SetPaneText(nIndex,strSensitive);
	CSize ftwd = 0;
	CDC* pDC = m_wndStatusBar.GetDC();
	ftwd = pDC->GetOutputTextExtent(strSensitive);
	m_wndStatusBar.ReleaseDC(pDC);
	int width = 150;
	m_wndStatusBar.SetPaneWidth(nIndex,width > 	ftwd.cx ? width : 	ftwd.cx);
}

void CMainFrame::OnToolsOptions() 
{
	COptionsDlg dlgOptions (StrFromResID(IDS_OPTIONS), this);
	dlgOptions.DoModal();
}

void CMainFrame::OnToolsPluginManage()
{
	CDlgPluginsManage dlg;
	dlg.DoModal();
}

void CMainFrame::OnToolSchememanage()
{
	ConfigLibItem item = gpCfgLibMan->GetConfigLibItem(0);

	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( pDoc )
	{	
		for( int m=0; m<pDoc->GetDlgDataSourceCount(); m++)
		{
			pDoc->GetDlgDataSource(m)->ClearLayerSymbolCache();
		}
	}

	
	CDlgScheme dlg;
	dlg.InitMems(item);
	dlg.DoModal();	

	if( pDoc )
	{	
		for( int m=0; m<pDoc->GetDlgDataSourceCount(); m++)
		{
			pDoc->GetDlgDataSource(m)->LoadLayerSymbolCache();
		}
	}
	
}

void CMainFrame::OnConfigConvert()
{
	char path[_MAX_PATH]={0};
	GetModuleFileName(AfxGetApp()->m_hInstance,path,_MAX_PATH);
	*(strrchr(path,'\\')) = 0x00;
	strcat(path,"\\ConfigConvert.exe");

	if (!PathFileExists(path)) return;

	SHELLEXECUTEINFO ShExecInfo = {0}; 
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);   
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;   
	ShExecInfo.hwnd = NULL;   
	ShExecInfo.lpVerb = NULL;   
	ShExecInfo.lpFile = path;                             
	ShExecInfo.lpParameters =  "";             
	ShExecInfo.lpDirectory = NULL;   
	ShExecInfo.nShow = SW_SHOW;   
	ShExecInfo.hInstApp = NULL;                 
	
	ShellExecuteEx(&ShExecInfo);   

}

void CMainFrame::OnChangeSchemeOfFDB()
{
	CDlgSchemeConvert dlg;
	dlg.DoModal();
}


void CMainFrame::OnViewCustomize()
{
	//创建新的快捷键表，将命令中用到的内部键放入快捷键中
	int num = ::CopyAcceleratorTable(m_hAccelTable,NULL,0);
	LPACCEL pNewTable = new ACCEL[num+256];
	if( pNewTable )
	{
		memset(pNewTable,0,sizeof(ACCEL)*(num+256));
		::CopyAcceleratorTable(m_hAccelTable,pNewTable,num);
		BYTE keys[]={VK_BACK,'A','B','C','D','L','S','V','H','R',
			VK_ESCAPE,'U','O','M',VK_DELETE,'N','E','Z','T','X'};
		int add = sizeof(keys)/sizeof(keys[0]);
		for( int i=0; i<add; i++)
		{
			pNewTable[num+i].fVirt = FVIRTKEY;
			pNewTable[num+i].key   = keys[i];
			pNewTable[num+i].cmd   = ID_COMMAND_INNERKEY;
		}
		::DestroyAcceleratorTable(m_hAccelTable);
		m_hAccelTable = ::CreateAcceleratorTable(pNewTable,num+add);
		delete[] pNewTable;
	}
	////------------------------------------
	//// Create a customize toolbars dialog:
	////------------------------------------
	CUIFToolbarCustomizeEx* pDlgCust = new CUIFToolbarCustomizeEx(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->AddMenu(IDR_POPUP_STEREO);
	pDlgCust->AddMenu(IDR_POPUP_SNAPSTATUS);
	pDlgCust->AddMenu(IDR_POPUP_ACCUDRAW);
	pDlgCust->AddMenu(IDR_MENU_OSG_POPUP);
	pDlgCust->Create();
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
// 	int ret = CUVSModify::WaiteExecuteAll();
// 	if (IDCANCEL==ret || IDYES==ret)
// 		return;
	
	//停用设备，并清除已经发出的消息
	m_deviceDriver.Stop();
	
	MSG msg;
	while( PeekMessage(&msg,m_hWnd,WM_DEVICE_LFOOT,WM_DEVICE_LFOOT,PM_REMOVE) );
	while( PeekMessage(&msg,m_hWnd,WM_DEVICE_MFOOT,WM_DEVICE_MFOOT,PM_REMOVE) );
	while( PeekMessage(&msg,m_hWnd,WM_DEVICE_RFOOT,WM_DEVICE_RFOOT,PM_REMOVE) );
	while( PeekMessage(&msg,m_hWnd,WM_DEVICE_MOVE,WM_DEVICE_MOVE,PM_REMOVE) );
	while( PeekMessage(&msg,m_hWnd,WM_DEVICE_KEY,WM_DEVICE_KEY,PM_REMOVE) );
	
// 	if (m_bFullScreen)
// 	{
// 		OnViewFullScreen();	// Return to normal mode
// 	}

	//打开了多个文档后，整体关闭时，提示选择是否关闭，如果先选是，接着选否，会导致pDoc销毁了，但视图没有关闭，然后死机
	//现在采用这个方式：逐个调用子框架的OnClose，直到关闭不成功为止

	CArray<CDocument*,CDocument*> arrPDocs;		//保存文档指针，下面用来反序关闭

	POSITION DocTempPos=AfxGetApp()->GetFirstDocTemplatePosition();
	while(DocTempPos!=NULL)
	{
		CDocTemplate* curTemplate=AfxGetApp()->GetNextDocTemplate(DocTempPos);
		POSITION DocPos=curTemplate->GetFirstDocPosition();
		while(DocPos!=NULL)
		{
			CDocument *pDoc=curTemplate->GetNextDoc(DocPos);
			if (pDoc)
			{
				arrPDocs.Add(pDoc);
			}
		}
	}

	//pViewPrint为非空时，它应该打印预览的视图
	CView *pViewPrint = GetActiveView();
	if( pViewPrint )
	{
		CMDIFrameWndEx::OnClose();
		return;
	}

	for( int i=arrPDocs.GetSize()-1; i>=0; i--)
	{
		CDocument *pDoc = arrPDocs[i];
		POSITION ViewPos=pDoc->GetFirstViewPosition();

		//只取第一个视图（矢量视图）
		CView *pView = pDoc->GetNextView(ViewPos);
		if( pView )
		{
			CFrameWnd *pFrame = pView->GetParentFrame();
			if( pFrame )
			{
				HWND hWnd = pFrame->GetSafeHwnd();
				pFrame->SendMessage(WM_CLOSE);

				//窗口依然存在，就认为没用成功关闭该子框架，关闭过程中止
				if( IsWindow(hWnd) )
				{
					break;
				}
			}
		}
	}
	
	//完全关闭了，于是主框架也关闭
	if( i<0 )
		CMDIFrameWndEx::OnClose();
}

LRESULT CMainFrame::OnProgress(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 2)//意味stepit
	{
		if (lParam == 0)lParam = 1;
		m_wndStatusBar.SetPaneProgress(0, m_wndStatusBar.GetPaneProgress(0) + lParam);
	}
	else if (wParam == 1)//意味着用lp的长度来创建一个进度条
	{
		m_wndStatusBar.EnablePaneProgressBar(0, lParam, TRUE);
	}
	else//隐藏进度条
	{
		m_wndStatusBar.SetPaneProgress(0, wParam);
	}

	return 0;
}

LRESULT CMainFrame::OnUpdateOption(WPARAM wParam, LPARAM lParam)
{
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	pApp->UpdateAllDocsByCmd(ID_OPTIONUPDATE);
	
	BOOL bTopMost = AfxGetApp()->GetProfileInt( REGPATH_UICOMMON,REGITEM_TOPMOST,gdef_bTopMost );
	
	if( bTopMost ) SetWindowPos( &wndTopMost  ,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE );
	else SetWindowPos( &wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE );
	
	return 0;
}


void CMainFrame::OnViewResult()
{
	BOOL bAutoHideMode = m_wndCheckResultBar.IsAutoHideMode ();
	BOOL bTabbed = m_wndCheckResultBar.IsTabbed ();

	if( !m_wndCheckResultBar.HaveInitLoad() )
	{
		m_wndCheckResultBar.InitLoad();
	}
	
	ShowPane (&m_wndCheckResultBar,
		!m_wndCheckResultBar.IsVisible (),
		FALSE, TRUE);
	RecalcLayout ();
}


void CMainFrame::OnUpdateViewResult(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCheckResultBar.IsVisible()?1:0);
}

void CMainFrame::OnQueryResultLast()
{
	m_wndCheckResultBar.SelectLastResult();
}

void CMainFrame::OnQueryResultNext()
{
	m_wndCheckResultBar.SelectNextResult();
}


LRESULT CMainFrame::OnGetQueryMenu(WPARAM wParam, LPARAM lParam)
{
	CQueryMenu **ppMenu = (CQueryMenu**)lParam;
	if( ppMenu )*ppMenu = &m_cQueryMenu;
	return 0;
}

LRESULT CMainFrame::OnCollectionView(WPARAM wParam, LPARAM lParam)
{
	m_wndCollectionView.FillCollectionView(wParam, (ConfigLibItem*)lParam);
	
	return 0;
}

LRESULT CMainFrame::OnInitLayerTool(WPARAM wParam, LPARAM lParam)
{
	// 初始化
	if (wParam == 0)
	{
		//获取工具条按钮的基本大小
		CMFCToolBarButton* pBtn = m_wndToolbarLayer.GetButton(0);
		if (pBtn) g_Btn0Rect = pBtn->Rect();

		CDlgDoc *pDoc = (CDlgDoc*)lParam;
		if (pDoc)
		{
			CUIFToolbarLayerGroupComboBoxButton comboLayerGroupButton (ID_DUMMY_VIEW_LAYERGROUP,
				GetCmdMgr()->GetCmdImage(ID_DUMMY_VIEW_LAYERGROUP, FALSE),
				CBS_DROPDOWNLIST,BW_LAYERGROUPCOMBOX_WIDTH);
			
			comboLayerGroupButton.RefreshComboBox(pDoc);

			m_wndToolbarLayer.ReplaceButton (ID_DUMMY_VIEW_LAYERGROUP,
				comboLayerGroupButton);	
			

			float fWidth = BW_LAYERCOMBOX_WIDTH;
			ConfigLibItem config = gpCfgLibMan->GetConfigLibItemByScale(pDoc->GetDlgDataSource()->GetScale());
			if (config.pScheme)
			{
				int nCount = config.pScheme->GetLayerDefineCount();
				if (nCount > 0 && config.pScheme->GetLayerDefine(nCount / 2)->GetLayerCode() > 10000)
				{
					fWidth += 50;
				}
			}

			CUIFToolbarLayerComboBoxButton comboButton (ID_DUMMY_VIEW_LAYER,
				GetCmdMgr()->GetCmdImage(ID_DUMMY_VIEW_LAYER, FALSE),
				CBS_DROPDOWNLIST,fWidth);
			
			comboButton.RefreshComboBox(pDoc);

			m_wndToolbarLayer.ReplaceButton (ID_DUMMY_VIEW_LAYER,
				comboButton, TRUE);
			
			m_wndToolbarLayer.AdjustLayout();

		}
	}
	else 
	{
		CUIFToolbarLayerComboBoxButton* pSrcCombo = NULL;
		
		CObList listButtons;
		if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_LAYER, listButtons) > 0)
		{
			for (POSITION posCombo = listButtons.GetHeadPosition (); 
			pSrcCombo == NULL && posCombo != NULL;)
			{
				CUIFToolbarLayerComboBoxButton* pCombo = 
					DYNAMIC_DOWNCAST (CUIFToolbarLayerComboBoxButton, listButtons.GetNext (posCombo));
				
				if (pCombo != NULL )
				{
					pSrcCombo = pCombo;
					break;
				}
			}
		}

		CUIFToolbarLayerGroupComboBoxButton* pSrcLayerGroupCombo = NULL;
		
		if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_LAYERGROUP, listButtons) > 0)
		{
			for (POSITION posCombo = listButtons.GetHeadPosition (); 
			pSrcLayerGroupCombo == NULL && posCombo != NULL;)
			{
				CUIFToolbarLayerGroupComboBoxButton* pCombo = 
					DYNAMIC_DOWNCAST (CUIFToolbarLayerGroupComboBoxButton, listButtons.GetNext (posCombo));
				
				if (pCombo != NULL )
				{
					pSrcLayerGroupCombo = pCombo;
					break;
				}
			}
		}
		
		if (pSrcCombo == NULL || pSrcLayerGroupCombo == NULL) return 0;

		pSrcCombo->RefreshComboBox();
		pSrcLayerGroupCombo->RefreshComboBox();

		// 清理/更新层信息
	/*	if (wParam == 1)
		{
			CDlgDoc *pDoc = (CDlgDoc*)param;

			pSrcCombo->RefreshComboBox();
			pSrcLayerGroupCombo->RefreshComboBox();
		}
		// 增加图层
		else if (wParam == 2)
		{
			CFtrLayer *pLayer = (CFtrLayer*)param;
			if (pLayer)
			{
				CString str;
				str.Format("%s(%I64d)",pLayer->GetName(),pSrcCombo->GetDlgDoc()->GetDlgDataSource()->GetFtrLayerCode(pLayer->GetName()));

				pSrcCombo->AddItem(str,DWORD_PTR(pLayer));
			}
		}
		// 删除图层
		else if (wParam == 3)
		{
			CFtrLayer *pLayer = (CFtrLayer*)param;
			if (pLayer)
			{
				pSrcCombo->DeleteItem(DWORD_PTR(pLayer));
			}
		}
		// 更改选择的图层
		else if (wParam == 4)
		{
			CFtrLayer *pLayer = (CFtrLayer*)param;
			if (!pLayer)
			{
				int iCurSel = pSrcCombo->GetCurSel();

				pLayer = (CFtrLayer*)pSrcCombo->GetItemData(iCurSel);
			}

			CString str;
			str.Format("%s(%I64d)",pLayer->GetName(),pSrcCombo->GetDlgDoc()->GetDlgDataSource()->GetFtrLayerCode(pLayer->GetName()));
			
			int iCurSel = pSrcCombo->GetCurSel();
			if (iCurSel>=0 && pSrcCombo->GetCount()>iCurSel && stricmp(pSrcCombo->GetItem(iCurSel),str) == 0)
			{
			}
			else
			{
				if (!pSrcCombo->SelectItem(str))
				{
					pSrcCombo->DeleteItem(DWORD_PTR(pLayer));
					pSrcCombo->AddItem(str,DWORD_PTR(pLayer));
					pSrcCombo->SelectItem(str);
				}
			}

			if (!pSrcLayerGroupCombo->SelectItem(pLayer->GetGroupName()))
			{
				FtrLayerGroup *pGroup = pSrcCombo->GetDlgDoc()->GetDlgDataSource()->GetFtrLayerGroupByName(pLayer->GetGroupName());
				if (pGroup)
				{
					pSrcLayerGroupCombo->AddItem(pGroup->Name,DWORD_PTR(pGroup));
					pSrcLayerGroupCombo->SelectItem(pGroup->Name);
				}
				
			}
			
			
		}
		// 初始化层信息
		else if (wParam == 5)
		{
			
		}*/
	}

	//wParam==9为采集结束、清除当前对象的状态显示；此时直接返回，优化响应速度
	if( wParam==9 )
		return 1;
	
	//AdjustLayout比较费时，也无必要，换成RedrawWindow
	if (lParam != 0)
	{
		m_wndToolbarLayer.RedrawWindow();
	}

	return 1;
}

LRESULT CMainFrame::OnInitFtrLayerView(WPARAM wParam, LPARAM lParam)
{
    m_wndFtrLayerView.UpdateLayerView();   

	return 1;
}

LRESULT CMainFrame::OnInitFtrTool(WPARAM wParam,LPARAM param)
{
	// 初始化
	if (wParam == 0)
	{
		CDlgDoc *pDoc = (CDlgDoc*)param;
		if (pDoc)
		{
			CUIFColorToolBarButton colorButton (ID_DUMMY_VIEW_FTRCOLOR,
				StrFromResID(ID_DUMMY_VIEW_FTRCOLOR),NULL,BW_FTRCOLORCOMBOX_WIDTH);
			
			colorButton.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
			colorButton.EnableAutomaticButton(_T("ByLayer"),0);
			m_wndToolbarFtr.ReplaceButton (ID_DUMMY_VIEW_FTRCOLOR,
				colorButton);

			CUIFToolbarFtrSymComboBoxButton comboSymButton (ID_DUMMY_VIEW_FTRSYM,
				GetCmdMgr()->GetCmdImage(ID_DUMMY_VIEW_FTRSYM, FALSE),
				CBS_DROPDOWNLIST,BW_FTRSYMCOMBOX_WIDTH);
			
			comboSymButton.RefreshComboBox(pDoc);
			
			m_wndToolbarFtr.ReplaceButton (ID_DUMMY_VIEW_FTRSYM,
				comboSymButton);


			CUIFToolbarFtrWidthComboBoxButton comboButton (ID_DUMMY_VIEW_FTRWIDTH,
				GetCmdMgr()->GetCmdImage(ID_DUMMY_VIEW_FTRWIDTH, FALSE),
				CBS_DROPDOWNLIST,BW_FTRWIDTHCOMBOX_WIDTH);
			
			comboButton.RefreshComboBox(pDoc);
			
			m_wndToolbarFtr.ReplaceButton (ID_DUMMY_VIEW_FTRWIDTH,
				comboButton);

			m_wndToolbarFtr.AdjustLayout();
			
		}
	}
	else 
	{
		{
			CUIFColorToolBarButton* pSrcCombo = NULL;
			
			CObList listButtons;
			if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_FTRCOLOR, listButtons) > 0)
			{
				for (POSITION posCombo = listButtons.GetHeadPosition (); 
				pSrcCombo == NULL && posCombo != NULL;)
				{
					CUIFColorToolBarButton* pCombo = 
						DYNAMIC_DOWNCAST (CUIFColorToolBarButton, listButtons.GetNext (posCombo));
					
					if (pCombo != NULL )
					{
						pSrcCombo = pCombo;
						break;
					}
				}
			}
			
			if (pSrcCombo)
			{
				CDlgDoc *pDoc = (CDlgDoc*)param;
				if (pDoc)
				{
					int num;
					const FTR_HANDLE *pFtrs = pDoc->GetSelection()->GetSelectedObjs(num);
					if (num > 0)
					{
						//地物来自参考文件时，pLayer为NULL
						CFeature *pFtr = HandleToFtr(pFtrs[0]);
						CFtrLayer *pLayer = pDoc->GetDlgDataSource()->GetFtrLayerOfObject(pFtr);
						if( !pLayer )
						{
							return 1;
						}
						pSrcCombo->EnableAutomaticButton(_T("ByLayer"),pLayer->GetColor());
						
						pSrcCombo->SetEmpty(FALSE);
						
						long color = pFtr->GetGeometry()->GetColor();
						if (color == -1)
						{
							//pSrcCombo->SetByLayer(TRUE);
							color = pLayer->GetColor();
						}
						else
						{
							//pSrcCombo->SetByLayer(FALSE);
						}
							
						pSrcCombo->SetColor(color);
					}
					else
					{
						CFeature *pFtr = NULL;
					//	CPermanentExchanger *pFtrExchanger = GetPermanentExchangerWithPropertiesView();
						CPermanentExchanger *pFtrExchanger = GetPermanentExchangerOfPropertiesView();

						if (pFtrExchanger && (pFtr = (CFeature*)pFtrExchanger->GetObject(0)))
						{
							CCommand *pCmd = pDoc->GetCurrentCommand();
							if (pCmd!=NULL && pCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)))
							{
								long color = pDoc->GetDlgDataSource()->GetCurFtrLayer()->GetColor();
								pSrcCombo->EnableAutomaticButton(_T("ByLayer"),color);
								
								long lFtrColor = pFtr->GetGeometry()->GetColor();
								pSrcCombo->SetColor(lFtrColor);
							}
						}						
						else
							pSrcCombo->SetEmpty(TRUE);
					}
				}
				else
				{
					pSrcCombo->SetEmpty(TRUE);
				}
				
			}
		}
		
		{
			CUIFToolbarFtrSymComboBoxButton* pSrcCombo = NULL;
			
			CObList listButtons;
			if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_FTRSYM, listButtons) > 0)
			{
				for (POSITION posCombo = listButtons.GetHeadPosition (); 
				pSrcCombo == NULL && posCombo != NULL;)
				{
					CUIFToolbarFtrSymComboBoxButton* pCombo = 
						DYNAMIC_DOWNCAST (CUIFToolbarFtrSymComboBoxButton, listButtons.GetNext (posCombo));
					
					if (pCombo != NULL )
					{
						pSrcCombo = pCombo;
						break;
					}
				}
			}
			
			if (pSrcCombo)
			{
				CDlgDoc *pDoc = (CDlgDoc*)param;
				pSrcCombo->RefreshComboBox();				
			}
		}

		{
			CUIFToolbarFtrWidthComboBoxButton* pSrcCombo = NULL;
			
			CObList listButtons;
			if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_FTRWIDTH, listButtons) > 0)
			{
				for (POSITION posCombo = listButtons.GetHeadPosition (); 
				pSrcCombo == NULL && posCombo != NULL;)
				{
					CUIFToolbarFtrWidthComboBoxButton* pCombo = 
						DYNAMIC_DOWNCAST (CUIFToolbarFtrWidthComboBoxButton, listButtons.GetNext (posCombo));
					
					if (pCombo != NULL )
					{
						pSrcCombo = pCombo;
						break;
					}
				}
			}
			
			if (pSrcCombo)
			{
				CDlgDoc *pDoc = (CDlgDoc*)param;
				pSrcCombo->RefreshComboBox();				
			}
		}		
				
	}
	
	//wParam==9为采集结束、清除当前对象的状态显示；此时直接返回，优化响应速度
	if( wParam==9 )
		return 1;
	
	//AdjustLayout比较费时，也无必要，换成RedrawWindow
	//DockControlBarLeftOf(&m_wndToolbarLayer,&m_wndToolbarFtr);
	if( param!=0 )
	{
		m_wndToolbarFtr.RedrawWindow();
	}

	return 1;
}

LRESULT CMainFrame::OnInitFtrRecent(WPARAM wParam, LPARAM lParam)
{		
	if (wParam == 0)
	{
		CScheme *pScheme = (CScheme*)lParam;
		if (!pScheme) return -1;

		float fWidth = 150;
	
		int nCount = pScheme->GetLayerDefineCount();
		if (nCount > 0 && pScheme->GetLayerDefine(nCount/2)->GetLayerCode() > 10000)
		{
			fWidth = 200;
		}

		CMFCToolBarComboBoxButton comboButton(ID_DUMMY_VIEW_FTRCLASS,
			GetCmdMgr()->GetCmdImage(ID_DUMMY_VIEW_FTRCLASS, FALSE),
			CBS_DROPDOWNLIST,fWidth);
		
		comboButton.RemoveAllItems();
		
		USERIDX *pUserIdx = pScheme->GetRecentIdx();

		int curid = 0;
		CString str;
		comboButton.AddItem(StrFromResID(IDS_SELECTFID_TITLE));
		for (int i=pUserIdx->m_aIdx.GetSize()-1;i>=0;i--)
		{
			IDX idx = pUserIdx->m_aIdx.GetAt(i);
			
// 			long code;
// 			const char* name = idx.FeatureName;
// 			if( gFindUserIdx(pUserIdx,FALSE,code,name) )idx.code = code;
			
			str.Format("%s (%I64d)",idx.FeatureName,idx.code);
			comboButton.AddItem (str);
			
		}
		if(pUserIdx->m_aIdx.GetSize() > 0)
			comboButton.SelectItem(0,FALSE);

		comboButton.SetCenterVert();
		comboButton.SetDropDownHeight(300);
		comboButton.SetFlatMode();
		
		m_wndToolbarView.ReplaceButton (ID_DUMMY_VIEW_FTRCLASS,
			comboButton,TRUE);
	}
	else
	{
		CMFCToolBarComboBoxButton* pSrcCombo = NULL;
		
		CObList listButtons;
		if (CMFCToolBar::GetCommandButtons (ID_DUMMY_VIEW_FTRCLASS, listButtons) > 0)
		{
			for (POSITION posCombo = listButtons.GetHeadPosition (); 
			pSrcCombo == NULL && posCombo != NULL;)
			{
				CMFCToolBarComboBoxButton* pCombo =
					DYNAMIC_DOWNCAST(CMFCToolBarComboBoxButton, listButtons.GetNext(posCombo));
				
				if (pCombo != NULL )
				{
					pSrcCombo = pCombo;
				}
			}
		}
		
		if (pSrcCombo != NULL)
		{
			CScheme *pScheme = (CScheme*)lParam;
			if (!pScheme) return -1;

			USERIDX *pUserIdx = pScheme->GetRecentIdx();		
			pSrcCombo->RemoveAllItems();
			
			CString str;
			int curid = 0;
			for (int i=pUserIdx->m_aIdx.GetSize()-1;i>=0;i--)
			{
				IDX idx = pUserIdx->m_aIdx.GetAt(i);
				
				// 			long code;
				// 			const char* name = idx.FeatureName;
				// 			if( gFindUserIdx(pUserIdx,FALSE,code,name) )idx.code = code;
				
				str.Format("%s (%I64d)",idx.FeatureName,idx.code);
				pSrcCombo->AddItem (str);
				
			}
			
			if(pUserIdx->m_aIdx.GetSize() > 0)  pSrcCombo->SelectItem (0,FALSE);
		}
	}
	
	m_wndToolbarView.AdjustLayout();	
	
	return 0;
}

//##ModelId=41466B7D0231
void CMainFrame::OnViewToolbar() 
{
	ShowPane (&m_wndToolBar,
		!m_wndToolBar.IsVisible (),
		FALSE, FALSE);
}

//##ModelId=41466B7D0242
void CMainFrame::OnUpdateViewToolbar(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(!IsFullScreen ());
	pCmdUI->SetCheck (m_wndToolBar.IsVisible ());
}

void CMainFrame::OnFeatureCode()
{
/*
BOOL bVisible = m_dlgFCode.IsWindowVisible();
m_dlgFCode.ShowWindow(m_dlgFCode.IsWindowVisible()?SW_HIDE:SW_SHOW);

  if (bVisible==FALSE)
  {
		CRect rc;
		
		  m_dlgFCode.m_bBarVisible = m_wndCollectionView.IsVisible();
		  BOOL bAutoHideMode = m_wndCollectionView.IsAutoHideMode();
		  BOOL bTabbed = m_wndCollectionView.IsTabbed ();
		  ShowPane (&m_wndCollectionView,
		  TRUE,
		  FALSE, bAutoHideMode || !bTabbed);
		  RecalcLayout ();
		  
			m_dlgFCode.GetWindowRect(&rc);
			m_dlgFCode.MoveWindow(200,200,rc.Width(),rc.Height());
			m_dlgFCode.GetDlgItem(IDC_EDIT_FCODE)->SetFocus();
			m_dlgFCode.m_CtlFCode.SetSel(0,-1);
}*/
	
	m_wndCollectionView.PopupforEditCode();
}

void CMainFrame::OnToolHeightstep() 
{
	CDlgSetHeiStep dlg;
	dlg.m_fHeiStep = GetProfileDouble(REGPATH_CONFIG,REGITEM_HEISTEP,5.0);
	if( dlg.DoModal()==IDOK )
	{
		WriteProfileDouble(REGPATH_CONFIG,REGITEM_HEISTEP,dlg.m_fHeiStep);
		CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
		pApp->UpdateAllDocsByCmd(ID_OPTIONUPDATE);
	}
}

void CMainFrame::OnWindowSwitch() 
{
	MDINext();
}

void CMainFrame::OnViewXYZ() 
{
	if (GetActiveDlgDoc())
		m_wndAccu.ShowWindow(m_wndAccu.IsWindowVisible()?SW_HIDE:SW_SHOW);
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
#ifdef TRIAL_VERSION
	VM_START
#endif

	// TODO: Add your specialized code here and/or call the base class
	BOOL bDoubleScreen	= AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DOUBLESCREEN, gdef_bDoubleScreen);
	if (bDoubleScreen)
	{
		 if(HIWORD(wParam)==1)
		 {		 	
			 if (AfxCallMessage(WM_COMMAND,wParam,lParam))
			 {
				 return TRUE;
			 }
			 
		 }
	}

#ifdef TRIAL_VERSION
	static int bExeCode = 1;
	static int stimes = 0;
	static int stimes1 = 0;
	if( bExeCode )
	{
		if( stimes>=3 )
		{
			if( gTrialTime0<=stimes1 || gTrialTime0<=0 || gTrialTime0>=900 )
			{
				StopWork();
				bExeCode = 0;
				return TRUE;
			}
			
			stimes = 0;
			stimes1 = gTrialTime0-1;
		}
		else
		{
			stimes++;
		}
	}
#endif

	int test = HIWORD(wParam);

	CMFCDropDownToolBar* pToolBar[] = { &m_wndToolbarDrawPoint, &m_wndToolbarDrawLine, &m_wndToolbarDrawCircle, &m_wndToolbarDrawSurface };
	
	//查找包含该按钮的下拉工具条
	BOOL bFind = FALSE;
	for (int i=0; i<sizeof(pToolBar)/sizeof(pToolBar[0]); i++)
	{
		for( int j=0; j<pToolBar[i]->GetCount(); j++ )
		{
			CMFCToolBarButton* pButton = pToolBar[i]->GetButton(j);
			if( pButton->m_nID==wParam )
			{
				bFind = TRUE;
				break;
			}
		}
		
		if( bFind )break;
	}
	
	if( bFind )
	{
		//查找包含上述下拉工具条的下拉按钮
		for (int j=0; j<m_wndToolbarView.GetCount(); j++)
		{
			CMFCToolBarButton* pButton1 = m_wndToolbarView.GetButton(j);
			
			if( pButton1->IsKindOf(RUNTIME_CLASS(CMFCDropDownToolbarButton)) )
			{
				for( int k=0; k<pToolBar[i]->GetCount(); k++ )
				{
					CMFCToolBarButton* pButton2 = pToolBar[i]->GetButton(k);
					if( pButton1->m_nID==pButton2->m_nID )break;
				}
				if( k<pToolBar[i]->GetCount() )
				{
					((CMFCDropDownToolbarButton*)pButton1)->SetDefaultCommand(wParam);
					m_wndToolbarView.RedrawWindow();
					break;
				}
			}
		}
	}

	BOOL bRet = CMDIFrameWndEx::OnCommand(wParam, lParam);
	
#ifdef TRIAL_VERSION
	VM_END
#endif

	return bRet;
}

void CMainFrame::SendMessageToViews(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_ptrListViews.GetSize()<=0)
	{
		return;
	}
	CView* pView = NULL;
	for (int i=0;i<m_ptrListViews.GetSize();i++)
	{
		pView = (CView*)m_ptrListViews[i];
		if (pView)
		{
			if(pView->SendMessage(message,wParam,lParam))
			{
				return;
			}
		}
	}
// 	CWinApp* pApp = AfxGetApp();
// 	POSITION  pos0,pos1;
// 	POSITION  pos = pApp->GetFirstDocTemplatePosition();
// 	CDocument* pDoc = NULL;
// 	CDocTemplate* pDocTem = NULL;
// 	CView* pView = NULL;
// 	while (pos)
// 	{
// 		pDocTem = pApp->GetNextDocTemplate(pos);
// 		if (pDocTem)
// 		{
// 			pos0 = pDocTem->GetFirstDocPosition();
// 			while(pos0)
// 			{
// 				pDoc = pDocTem->GetNextDoc(pos0);
// 				if (pDoc==GetActiveDlgDoc())
// 				{
// 					pos1 = pDoc->GetFirstViewPosition();
// 					while(pos1)
// 					{
// 						pView = pDoc->GetNextView(pos1);
// 						if (pView)
// 						{							
// 							if(pView->SendMessage(WM_DEVICE_LFOOT,wParam,lParam))
// 							{
// 								return ;
// 							}
// 						}
// 					}
// 				}
// 			}
// 		}
// 		
// 	}

}

void CMainFrame::OnViewDrawToolbar() 
{
	ShowPane(&m_wndToolbarDraw,
		!m_wndToolbarDraw.IsVisible (),
					FALSE, FALSE);
}

void CMainFrame::OnUpdateViewDrawToolbar(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(!IsFullScreen ());
	pCmdUI->SetCheck (m_wndToolbarDraw.IsVisible ());
}

void CMainFrame::OnViewViewToolbar() 
{
	ShowPane(&m_wndToolbarView,
		!m_wndToolbarView.IsVisible (),
					FALSE, FALSE);	
}

void CMainFrame::OnUpdateViewViewToolbar(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(!IsFullScreen ());
	pCmdUI->SetCheck (m_wndToolbarView.IsVisible ());
}

void CMainFrame::OnViewEditToolbar() 
{
	ShowPane(&m_wndToolbarEdit,
		!m_wndToolbarEdit.IsVisible (),
		FALSE, FALSE);
	
}

void CMainFrame::OnUpdateViewEditToolbar(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(!IsFullScreen ());
	pCmdUI->SetCheck (m_wndToolbarEdit.IsVisible ());
}

void CMainFrame::OnViewEditvectToolbar() 
{
	ShowPane(&m_wndToolbarEditVect,
		!m_wndToolbarEditVect.IsVisible (),
		FALSE, FALSE);	
}

void CMainFrame::OnUpdateViewEditvectToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndToolbarEditVect.IsVisible ());	
}

void CMainFrame::OnViewEditAdvancedToolbar() 
{
	ShowPane(&m_wndToolbarEditAdvanced,
		!m_wndToolbarEditAdvanced.IsVisible (),
		FALSE, FALSE);	
}

void CMainFrame::OnUpdateViewEditAdvancedToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndToolbarEditAdvanced.IsVisible ());	
}

void CMainFrame::OnViewTestcontour() 
{
	ShowPane(&m_wndToolBarTestContour,
				!m_wndToolBarTestContour.IsVisible (),
		FALSE, FALSE);
}

void CMainFrame::OnUpdateViewTestcontour(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndToolBarTestContour.IsVisible ());
}

void CMainFrame::OnViewLayerToolbar()
{
	ShowPane(&m_wndToolbarLayer,
		!m_wndToolbarLayer.IsVisible (),
		FALSE, FALSE);
}

void CMainFrame::OnUpdateViewLayerToolbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck (m_wndToolbarLayer.IsVisible ());
}

void CMainFrame::OnViewFeatureToolbar()
{
	ShowPane(&m_wndToolbarFtr,
		!m_wndToolbarFtr.IsVisible (),
		FALSE, FALSE);
}

void CMainFrame::OnUpdateViewFeatureToolbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck (m_wndToolbarFtr.IsVisible ());
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
#ifdef TRIAL_VERSION
	VM_START
#endif

	switch( nIDEvent )
	{
		case TIMERID_SAVE:
		{
			CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
			pApp->UpdateAllDocsByCmd(ID_SAVETIMER);
		}
		break;
		default:;
	}

#ifdef TRIAL_VERSION

	gTrialTime0++;

	if( gTrialTime0>=900 )
	{
		StopWork();
	}
	VM_END

#endif

}



void CMainFrame::OnViewEditcontoursToolbar() 
{
	ShowPane(&m_wndToolbarEditContours,
			!m_wndToolbarEditContours.IsVisible (),
		FALSE, FALSE);	
}

void CMainFrame::OnUpdateViewEditcontoursToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndToolbarEditContours.IsVisible ());
}


void CMainFrame::OnViewLidar() 
{
	ShowPane(&m_wndToolbarLidar,
		!m_wndToolbarLidar.IsVisible (),
		FALSE, FALSE);	
	
}

void CMainFrame::OnUpdateViewLidar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndToolbarLidar.IsVisible ());
}

void CMainFrame::OnViewVisibilitySwitch() 
{
	ShowPane(&m_wndToolbarVisibilitySwitch,
		!m_wndToolbarVisibilitySwitch.IsVisible (),
		FALSE, FALSE);	
}

void CMainFrame::OnUpdateViewVisibilitySwitch(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndToolbarVisibilitySwitch.IsVisible ());
}

void CMainFrame::OnViewToolbarTopo()
{
	ShowPane(&m_wndToolbarTopo,
		!m_wndToolbarTopo.IsVisible(),
		FALSE, FALSE);
}

void CMainFrame::OnUpdateViewToolbarTopo(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndToolbarTopo.IsVisible());
}


void CMainFrame::OnViewStereoMagnify() 
{
	ShowPane(&m_wndToolbarStereoMagnify,
		!m_wndToolbarStereoMagnify.IsVisible (),
		FALSE, FALSE);	
	
}

void CMainFrame::OnUpdateViewStereoMagnify(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndToolbarStereoMagnify.IsVisible ());
}



void CMainFrame::OnCellDefView()
{	
	if (m_pCellDefDlg)
	{
		delete m_pCellDefDlg;
		m_pCellDefDlg = NULL;
	}
	ConfigLibItem config = gpCfgLibMan->GetConfigLibItem(0);
	if (config.GetScale() == 0)  return;

	m_pCellDefDlg = new CDlgCellDefLinetypeView;
	if( !m_pCellDefDlg )return;
	
	m_pCellDefDlg->SetConfig(config);
	
	m_pCellDefDlg->SetShowMode(TRUE,FALSE,0,"",TRUE);
	
	m_pCellDefDlg->Create(IDD_CELLDEF_LINETYPE,NULL);
	
	m_pCellDefDlg->ShowWindow(SW_SHOW);

}

void CMainFrame::OnCellDefEdit()
{
	if (m_pCellDefDlg)
	{
		delete m_pCellDefDlg;
		m_pCellDefDlg = NULL;
	}
	ConfigLibItem config = gpCfgLibMan->GetConfigLibItem(0);
	if (config.GetScale() == 0)  return;
	
	m_pCellDefDlg = new CDlgCellDefLinetypeView;
	if( !m_pCellDefDlg )return;
	
	m_pCellDefDlg->SetConfig(config);

	m_pCellDefDlg->EnableNewCell();
	
	m_pCellDefDlg->SetShowMode(FALSE,FALSE,0,"",TRUE);
	
	m_pCellDefDlg->Create(IDD_CELLDEF_LINETYPE,NULL);

	m_pCellDefDlg->SetWindowText(StrFromResID(IDS_CELLDEF_EDIT));
	
	m_pCellDefDlg->ShowWindow(SW_SHOW);

	//打开文档
	POSITION curTemplatePos = AfxGetApp()->GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = AfxGetApp()->GetNextDocTemplate(curTemplatePos);
		CDocument *rpDocMatch;
		if( curTemplate->MatchDocType( ".abc", rpDocMatch )==CDocTemplate::yesAttemptNative )
		{
			CDocument *doc = curTemplate->OpenDocumentFile(NULL);
			if (doc->IsKindOf(RUNTIME_CLASS(CDlgCellDoc)))
			{
				m_pCellDefDlg->m_pCellDoc = (CDlgCellDoc*)doc;
				((CDlgCellDoc*)doc)->SetDlgCellEditDef(m_pCellDefDlg);
			}
			break;
		}
	}
}


void CMainFrame::OnCellDefCopy()
{
	CDlgCellDefCopy dlg;
	dlg.DoModal();
}

void CMainFrame::OnWindowCloseAll() 
{
	CArray<HWND, HWND> arr;
	HWND hwndT = ::GetWindow(this->m_hWndMDIClient, GW_CHILD);
	while (hwndT != NULL)
	{
		arr.Add(hwndT);		
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
	}
	for (int i=0;i<arr.GetSize();i++)
	{
		::SendMessage(arr[i],WM_CLOSE,0,0);
	}	
}

void CMainFrame::OnCombinScheme()
{
	CCombinDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnDestroy() 
{
	SendMessage(FCCM_INITLAYERTOOL, WPARAM(1), LPARAM(NULL));
	SendMessage(FCCM_INITFTRTOOL, WPARAM(1), LPARAM(NULL));

	CMDIFrameWndEx::OnDestroy();
	if (((CEditBaseApp*)AfxGetApp())->IsSupportPlugs())
	{
		for (int i=0;i<m_arrPlugsToolBar.GetSize();i++)
		{
			if (m_arrPlugsToolBar[i])
			{
				((CUserCustomToolbar*)m_arrPlugsToolBar[i])->DestroyWindow();
				delete ((CUserCustomToolbar*)m_arrPlugsToolBar[i]);
			}
		}
		m_arrPlugsToolBar.RemoveAll();
	}
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{	
	if( pMsg->message==WM_KEYDOWN && (pMsg->wParam=='A' || pMsg->wParam=='D' || pMsg->wParam=='S' )  )
	{
		if( GetKeyState(VK_CONTROL)>=0 && GetKeyState(VK_SHIFT)>=0 && GetKeyState(VK_MENU)>=0 && m_wndCheckResultBar.IsResultIterate() )
		{
			if( pMsg->wParam=='A' )
				m_wndCheckResultBar.SelectLastResult();
			else if( pMsg->wParam=='S' )
				m_wndCheckResultBar.SelectNextResult();
			else if( pMsg->wParam=='D' )
				m_wndCheckResultBar.DeleteOrRestoreCurResult();
			
			return TRUE;
		}
	}
	
	if( pMsg->message==WM_KEYDOWN)
	{
		CDlgDoc *pDoc = GetActiveDlgDoc();
		if (pDoc != NULL)
		{
			CView *pView = pDoc->GetCurActiveView();
			if (pView)
			{
				if (pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
				{
					if (pMsg->wParam == VK_F3)
					{
						pView->SendMessage(WM_HOTKEY, ID_VECTOR_ZOOMIN, 0);
						return TRUE;
					}
					if (pMsg->wParam == VK_F4)
					{
						pView->SendMessage(WM_HOTKEY, ID_VECTOR_ZOOMOUT, 0);
						return TRUE;
					}
					if (pMsg->wParam == VK_F5 && GetKeyState(VK_CONTROL) < 0)
					{
						pView->SendMessage(WM_HOTKEY, ID_VECTOR_ZOOMFIT, 0);
						return TRUE;
					}
				}
				if (pMsg->wParam == VK_F6 && GetKeyState(VK_CONTROL) >= 0)
				{
					pView->SendMessage(WM_COMMAND, IDC_BUTTON_CURSOR, 0);
					return TRUE;
				}
			}
		}
	}

	//cjc 2012.10.26 双击关闭标签页
	if(pMsg->message == WM_LBUTTONDBLCLK)
	{
		
		CPoint pt (GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
		CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);
		
		if (pWnd != NULL)
		{
			//pWnd->ClientToScreen (&pt);
			if(pWnd == &m_wndClientArea.GetMDITabs())
			{
				int index = m_wndClientArea.GetMDITabs().GetTabFromPoint(pt);
				if(index != -1)
				{
					CWnd *pTabWnd = m_wndClientArea.GetMDITabs().GetTabWnd(index);
					pTabWnd->SendMessage(WM_CLOSE,0,0);
				}
				
			}
		}
		
	}

	BOOL bRet = CMDIFrameWndEx::PreTranslateMessage(pMsg);

	if( !bRet && pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN )
	{
		if( ::GetFocus()!=m_wndOutputBar.m_wndInEdit.GetSafeHwnd())
			m_wndOutputBar.m_wndInEdit.SetFocus();
	}

	if(((CEditBaseApp*)AfxGetApp())->IsSupportPlugs()&&!bRet && (pMsg->message==WM_KEYDOWN || pMsg->message==WM_SYSKEYDOWN) )
	{		
		for( int i=0; i<m_arrPlugsToolBar.GetSize(); i++)
		{
			CUserCustomToolbar *pTool = (CUserCustomToolbar*)m_arrPlugsToolBar.GetAt(i);
			if( pTool )
			{
				if( TranslateAccelerator(m_hWnd,pTool->GetAccelerator(),pMsg)!=0 )
				{
					return TRUE;
				}
			}
		}		
	}

	return bRet;
}
void CMainFrame::OnOptionQt() 
{
	COptionsDlg dlgOptions (StrFromResID(IDS_OPTIONS), this,8);
	dlgOptions.DoModal();
}
void CMainFrame::OnViewTestDlgBar()
{
	ShowPane(&m_wndTestDlgBar,
		!m_wndTestDlgBar.IsVisible (),
		FALSE, FALSE);
	if (m_wndTestDlgBar.IsVisible ())
	{
		m_wndTestDlgBar.Popup();
	}
}
void CMainFrame::OnUpdateViewTestDlgBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck (m_wndTestDlgBar.IsVisible ());
}
LRESULT CMainFrame::OnGetDrawTestPtBar(WPARAM wParam, LPARAM lParam)
{
	CTestVectorBar **pBar = (CTestVectorBar **)lParam;
	if (pBar)
	{
		*pBar = &m_wndTestDlgBar;
		return 1;
	}
	return 0;
}

LRESULT CMainFrame::OnRefreshChkSchemeViewBar(WPARAM wParam, LPARAM lParam)
{
	if(wParam==1||m_wndCheckSchemeViewBar.IsVisible())
		m_wndCheckSchemeViewBar.RefreshViewBar(wParam);

	return 1;
}

LRESULT CMainFrame::OnShowCheckResults(WPARAM wParam, LPARAM lParam)
{
	if( wParam==1 )
	{
		if( !m_wndCheckResultBar.HaveInitLoad() )
		{
			m_wndCheckResultBar.InitLoad();
		}

		m_wndCheckResultBar.OnResultClear();
		return 1;
	}

	m_wndCheckResultBar.OnSetResult(wParam,lParam);

	BOOL bTabbed = m_wndCheckResultBar.IsTabbed ();

	BOOL bAutoHideMode = m_wndCheckResultBar.IsAutoHideMode ();
	if(!m_wndCheckResultBar.IsVisible ())	
		ShowPane(&m_wndCheckResultBar,
		TRUE,
		FALSE, TRUE);

// 	if (m_wndCheckSchemeViewBar.IsVisible()&&!bTabbed)
// 	{
// 		CUIFTabbedControlBar* pTabbedBar = NULL;
// 		m_wndCheckResultBar.AttachToTabWnd (&m_wndCheckSchemeViewBar, DM_SHOW, TRUE, 
// 			(CDockablePane**) &pTabbedBar);
// 
// 		pTabbedBar->ShowTab(&m_wndCheckResultBar,TRUE,FALSE,TRUE);
// 
// 		return 1;
// 	}
	
	return 1;
}

LRESULT CMainFrame::OnCopyDocData(WPARAM wParam, LPARAM lParam)
{
	int iOp = (BOOL)wParam;

	if (iOp == 0)
	{
		CopyData *pData = (CopyData*)lParam;
		*pData = m_copyData;
	}
	else if (iOp == 1)
	{
		m_copyData = *((CopyData*)lParam);
	}

	return 1;
}

void CMainFrame::OnDataCheck() 
{
	if(!m_wndCheckSchemeViewBar.IsVisible())
	{
		m_wndCheckSchemeViewBar.RefreshViewBar(0);	
		m_wndCheckSchemeViewBar.ShowPane(TRUE, FALSE, TRUE);
	}
	
}

void CMainFrame::OnUpdateDataCheck(CCmdUI* pCmdUI) 
{	
	pCmdUI->SetCheck (m_wndCheckSchemeViewBar.IsVisible ());
}



LRESULT CMainFrame::OnCloseDoc(WPARAM wParam, LPARAM lParam)
{
	// 清除数据
	CDlgDoc *pDoc = (CDlgDoc*)lParam;
	if (m_copyData.pDoc != NULL && m_copyData.pDoc == pDoc)
	{
		m_copyData.Clear();
	}
	m_wndCheckResultBar.OnCloseDoc(pDoc);

	m_wndFtrLayerView.UpdateLayerView();
	return 0;
}

void CMainFrame::OnConfigMerge() 
{
	// TODO: Add your command handler code here
	//MessageBox("方案合并界面","方案合并",MB_OK|MB_ICONEXCLAMATION);
	if (!m_pMainSrcCfgLibManforMerge)
	{
		m_pMainSrcCfgLibManforMerge = (CConfigLibManager *)new CConfigLibManager();
		m_pMainSrcCfgLibManforMerge->Copy(gpCfgLibMan);
	}
	if (!m_pMainDesCfgLibManforMerge)
	{
		m_pMainDesCfgLibManforMerge = (CConfigLibManager *)new CConfigLibManager();
		m_pMainDesCfgLibManforMerge->Copy(gpCfgLibMan);
	}
	CSchemeMerge dlg(NULL,m_pMainSrcCfgLibManforMerge,m_pMainDesCfgLibManforMerge,m_DesSchemeScaleforMerge,m_SrcSchemeScaleforMerge,m_bLoadedSrcTreeforMerge);//
	
	dlg.DoModal();
	m_SrcSchemeScaleforMerge = dlg.GetSrcScale();
	m_DesSchemeScaleforMerge = dlg.GetDesScale();
	m_pMainSrcCfgLibManforMerge = dlg.GetSrcCfgLibMan();
	m_pMainDesCfgLibManforMerge = dlg.GetDesCfgLibMan();
	m_bLoadedSrcTreeforMerge = dlg.IsSrcTreeLoaded();
	//ReleaseCfgLibMan();

	return;
}

void CMainFrame::OnConfigCopyproperty() 
{
	// TODO: Add your command handler code here
	
	if (!m_pMainSrcCfgLibManforCopy)
	{
		m_pMainSrcCfgLibManforCopy = (CConfigLibManager *)new CConfigLibManager();
		m_pMainSrcCfgLibManforCopy->Copy(gpCfgLibMan);
	}
	if (!m_pMainDesCfgLibManforCopy)
	{
		m_pMainDesCfgLibManforCopy = (CConfigLibManager *)new CConfigLibManager();
		m_pMainDesCfgLibManforCopy->Copy(gpCfgLibMan);
	}
	CSchemePropertyCopy dlg(NULL,m_pMainSrcCfgLibManforCopy,m_pMainDesCfgLibManforCopy,m_DesSchemeScaleforCopy,m_SrcSchemeScaleforCopy,m_bLoadedSrcTreeforCopy);
	//static CSchemeMerge dlg;
	dlg.DoModal();
	m_SrcSchemeScaleforCopy = dlg.GetSrcScale();
	m_DesSchemeScaleforCopy = dlg.GetDesScale();
	m_pMainSrcCfgLibManforCopy = dlg.GetSrcCfgLibMan();
	m_pMainDesCfgLibManforCopy = dlg.GetDesCfgLibMan();
	m_bLoadedSrcTreeforCopy = dlg.IsSrcTreeLoaded();
	return;
}

void CMainFrame::ReleaseCfgLibMan()
{
	if (m_pMainDesCfgLibManforMerge)
	{
		delete m_pMainDesCfgLibManforMerge;
		m_pMainDesCfgLibManforMerge = NULL;
	}
	if (m_pMainSrcCfgLibManforMerge)
	{
		delete m_pMainSrcCfgLibManforMerge;
		m_pMainSrcCfgLibManforMerge = NULL;
	}
	if (m_pMainSrcCfgLibManforCopy)
	{
		delete m_pMainSrcCfgLibManforCopy;
		m_pMainSrcCfgLibManforCopy = NULL;
	}
	if (m_pMainDesCfgLibManforCopy)
	{
		delete m_pMainDesCfgLibManforCopy;
		m_pMainDesCfgLibManforCopy = NULL;
	}
}

#include "customizekey.h"

void CMainFrame::OnCustomizeKey()
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	CString path = GetConfigPath(TRUE) + "\\CustomizeKey.txt";
	if(!::PathFileExists(path))
	{
		FILE *fp = fopen(path, "wt");
		if(!fp) return;

		if (pDoc)
		{
			int nreg=0;
			const CMDREG *pRegs = pDoc->GetCmdRegs(nreg);
			int j, len;
			for(int i=0; i<nreg; i++)
			{
				CString name = pRegs[i].strName;
				name = _T("<") + name + _T(">");
				fprintf(fp, "%-60s %-40s \n", name, pRegs[i].strAccel);
			}
		}
		fclose(fp);
		fp = NULL;
	}
	
	CCustomizeKey dlg;
	if (pDoc)
	{
		CCommand *pCurCmd = pDoc->GetCurrentCommand();
		if(pCurCmd)
		{
			dlg.m_CurCommandName = pCurCmd->Name();
			dlg.m_CurCommandAccel = pCurCmd->AccelStr();
		}
	}
	if (dlg.DoModal()==IDOK)
	{
		//跟新自定义快捷键
		m_wndOutputBar.LoadCustomizeKey();
	}
}

LRESULT CMainFrame::OnSetFocusColletion(WPARAM wParam, LPARAM lParam)
{
	ShowPane(&m_wndCollectionView, TRUE, FALSE, TRUE);
	RecalcLayout();
	m_wndCollectionView.OnSetFocusEdit();
	return 0;
}

void CMainFrame::OnCommandIBoxFunc(UINT nID)
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (!pDoc) return;

	if (nID == ID_IBOX_FUNC_CLOSE)
	{
		CCommand *pCmd = pDoc->GetCurrentCommand();
		if (pCmd && pCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)))
		{
			_variant_t var;
			var = (BYTE)('c');
			CValueTable tab;
			tab.BeginAddValueItem();
			tab.AddValue("ClosedYAccKey", &CVariantEx(var));
			tab.EndAddValueItem();
			pCmd->SetParams(tab);
		}
	}
	else if (nID == ID_IBOX_FUNC_BACK)
	{
		pDoc->KeyDown(VK_BACK, 0);
	}
	else if (nID == ID_IBOX_FUNC_SW_SNAP)
	{
		long mode = pDoc->m_snap.GetSnapMode();
		long mode1 = mode & 0x03;
		if (mode1 == 0)      mode = mode + 1;
		else if (mode1 == 1) mode = mode + 1;
		else if (mode1 == 2) mode = mode - 1;
		else if (mode1 == 3) mode = mode - 1;
		pDoc->m_snap.SetSnapMode(mode);
		pDoc->m_snap.UpdateSettings(TRUE);
	}
	else if (nID == ID_IBOX_FUNC_SW_2D3D)
	{
		pDoc->m_snap.Enable2D(!pDoc->m_snap.Is2D());
		pDoc->m_snap.UpdateSettings(TRUE);
	}
	else if (nID == ID_IBOX_FUNC_LOCKZ)
	{
		POSITION pos = pDoc->GetFirstViewPosition();
		CView *pView = pDoc->GetNextView(pos);
		while (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
			{
				pView->SendMessage(WM_COMMAND, ID_STEREO_HEILOCK, 0);
				break;
			}
			pView = pDoc->GetNextView(pos);
		}
	}
	else if (nID == ID_IBOX_FUNC_ZOOMIN_V)
	{
		POSITION pos = pDoc->GetFirstViewPosition();
		CView *pView = pDoc->GetNextView(pos);
		while (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
			{
				pView->SendMessage(WM_HOTKEY, ID_VECTOR_ZOOMIN, 0);
				break;
			}
			pView = pDoc->GetNextView(pos);
		}
	}
	else if (nID == ID_IBOX_FUNC_ZOOMOUT_V)
	{
		POSITION pos = pDoc->GetFirstViewPosition();
		CView *pView = pDoc->GetNextView(pos);
		while (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
			{
				pView->SendMessage(WM_HOTKEY, ID_VECTOR_ZOOMOUT, 0);
				break;
			}
			pView = pDoc->GetNextView(pos);
		}
	}
	else if (nID == ID_IBOX_FUNC_ZOOMIN_S)
	{
		POSITION pos = pDoc->GetFirstViewPosition();
		CView *pView = pDoc->GetNextView(pos);
		while (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
			{
				pView->SendMessage(WM_COMMAND, ID_STEREO_ZOOMIN, 0);
				break;
			}
			pView = pDoc->GetNextView(pos);
		}
	}
	else if (nID == ID_IBOX_FUNC_ZOOMOUT_S)
	{
		POSITION pos = pDoc->GetFirstViewPosition();
		CView *pView = pDoc->GetNextView(pos);
		while (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
			{
				pView->SendMessage(WM_COMMAND, ID_STEREO_ZOOMOUT, 0);
				break;
			}
			pView = pDoc->GetNextView(pos);
		}
	}
}

void CMainFrame::OnCheckExProp()
{
	if (m_CheckExPropDlg==NULL)
	{
		m_CheckExPropDlg = new CCheckExProp();
		m_CheckExPropDlg->Create(IDD_CHECKEXPROP, this);
	}
	if (!m_CheckExPropDlg) return;
	m_CheckExPropDlg->m_pDoc = GetActiveDlgDoc();
	m_CheckExPropDlg->ShowWindow(SW_SHOW);
	if (m_CheckExPropDlg->LoadRuleFile())
	{
		m_CheckExPropDlg->Check();
	}
}

void CMainFrame::OnEditExProp()
{
	BeginCheck42License
	if (m_EditExPropDlg==NULL)
	{
		m_EditExPropDlg = new CEditExProp();
		m_EditExPropDlg->Create(IDD_CHECKEXPROP, this);
	}
	if (!m_EditExPropDlg) return;
	m_EditExPropDlg->m_pDoc = GetActiveDlgDoc();
	m_EditExPropDlg->ShowWindow(SW_SHOW);
	m_EditExPropDlg->LoadRuleFile();
	m_EditExPropDlg->Check();//没有rulefile，任然可以显示编辑
	EndCheck42License
}

void CMainFrame::OnUpdateExportMenu(CCmdUI* pCmdUI)
{
	//是否
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(!pDoc) return;
	CDataSourceEx *pDS = pDoc->GetDlgDataSource();
	if(pDS && UVSModify==GetAccessType(pDS) && FALSE==CUVSModify::CanExport())
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}


void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default

	/*CMDIFrameWndEx::OnNcCalcSize(bCalcValidRects, lpncsp);*/
}




LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return CMDIFrameWndEx::WindowProc(message, wParam, lParam);
}


void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{	
	// TODO: Add your message handler code here and/or call default

	CMDIFrameWndEx::OnNcLButtonDown(nHitTest, point);
}


void CMainFrame::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	/*CMDIFrameWndEx::OnNcMouseMove(nHitTest, point);*/
}


void CMainFrame::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	/*CMDIFrameWndEx::OnNcLButtonUp(nHitTest, point);*/
}


void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	/*CMDIFrameWndEx::OnMouseMove(nFlags, point);*/
}


void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CMDIFrameWndEx::OnLButtonDown(nFlags, point);
}


LRESULT CMainFrame::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	RECT rectWindows, rectClient;
	this->GetWindowRect(&rectWindows);
	this->GetClientRect(&rectClient);

	if (point.y > rectWindows.top && point.y < rectWindows.top + 25)
	{
		return HTCAPTION;//标题栏形式  
	}
	else
	{
		return CFrameWnd::OnNcHitTest(point);
	}


	return CMDIFrameWndEx::OnNcHitTest(point);
}


void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default

	CMDIFrameWndEx::OnSysCommand(nID, lParam);
}


void CMainFrame::OnViewMm3dtools()
{
	/*ShowPane(&m_wndToolbarMm3dTools,
		!m_wndToolbarMm3dTools.IsVisible(),
		FALSE, FALSE);*/
}


void CMainFrame::OnUpdateViewMm3dtools(CCmdUI *pCmdUI)
{
	/*pCmdUI->SetCheck(m_wndToolbarMm3dTools.IsVisible());*/
}

CString GetModuleFolder()
{
	TCHAR   szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	ZeroMemory(_tcsrchr(szPath, _T('\\')), _tcslen(_tcsrchr(szPath, _T('\\')))*sizeof(TCHAR));
	return CString(szPath);
}

void runExecute(const CString & exec){
	CString exePath = GetModuleFolder();
	CString path;
	path = exePath + "\\" + exec;
	ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnIdrOsgmultifilelinkcreator()
{
	runExecute("osgMultiFileLinkCreator.exe");
}


void CMainFrame::OnIdrMm3dConvertSmart3d()
{
	runExecute("mm3dConvertSmart3D.exe");
}


void CMainFrame::OnIdrDsmToOsgb()
{
	runExecute("domDsmToOsgb.exe");
}


void CMainFrame::OnLinkfetchcoord()
{
	runExecute("linkFetchCoord.exe");
}


void CMainFrame::OnExtractaerial()
{
	runExecute("extractAerial.exe");
}


void CMainFrame::OnOsgbviewImg()
{
	ShowPane(&m_showImgBar,
		!m_showImgBar.IsVisible(),
		FALSE, FALSE);
}


void CMainFrame::OnUpdateOsgbviewImg(CCmdUI *pCmdUI)
{
	
}
