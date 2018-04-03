// ProjectViewBar.cpp: implementation of the CProjectViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "ProjectViewBar.h"
#include "ExMessage.h "
#include "StereoView.h "
#include "RegDef.h "
#include "RegDef2.h "
#include "StereoFrame.h "
#include "DlgDataSource.h"
#include "DlgConnectDB.h"
#include "UVSModify.h"
#include "DlgWorkSpaceBound.h"
#include "SQLiteAccess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern DWORD_PTR gdwInnerCmdData[2];
extern CStereoView *GetStereoView(int flag);

#define PROJECTITEM_ID			1
#define DLGFILEITEM_ID			2
#define MODELITEM_ID			3
#define IMAGEITEM_ID			4	


class CProjectViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CProjectViewBar;
	
	DECLARE_SERIAL(CProjectViewMenuButton)
		
public:
	//##ModelId=41466B7F02AF
	CProjectViewMenuButton(HMENU hMenu = NULL) :
	CMFCToolBarMenuButton ((UINT)-1, hMenu, -1)
	{
	}
	
	//##ModelId=41466B7F02B1
	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE,
		BOOL bDrawBorder = TRUE,
		BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages ();
		
		CAfxDrawState ds;
		pImages->PrepareDrawImage (ds);
		
		CMFCToolBarMenuButton::OnDraw (pDC, rect, 
			pImages, bHorz, 
			bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);
		
		pImages->EndDrawImage (ds);
	}
};

IMPLEMENT_SERIAL(CProjectViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=41466B7F00CC
CProjectViewBar::CProjectViewBar()
{
	m_bShowMapInfo=true;
	{
		TCHAR ModulePath[MAX_PATH];
		GetModuleFileName( NULL, ModulePath, MAX_PATH );  //获取程序路径
		int i = 0, j;
		while( ModulePath[i]!=0 )
		{
			if( ModulePath[i]=='\\' )
				j = i;
			i++;
		}
		ModulePath[j+1] = '\0';
		m_exePath.Format( "%s", ModulePath );		//分离路径名
		m_CMruFile.m_IniFileName = m_exePath+_T("MruFile.ini");	//设置ini文件名
	}

}

//##ModelId=41466B7F00CD
CProjectViewBar::~CProjectViewBar()
{
}

BEGIN_MESSAGE_MAP(CProjectViewBar, CDockablePane)
	//{{AFX_MSG_MAP(CProjectViewBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_COMMAND(ID_POPUP_OPENFILE, OnLoadFile)
	ON_COMMAND(ID_OPEN_LOCAL, OnOpenLocalFile)
	ON_COMMAND(ID_COMMIT_LOCAL, OnCommitLocalFile)
	ON_COMMAND(ID_POPUP_OPENOTHERSTEREO, OnLoadOther)
	ON_COMMAND(ID_POPUP_CLOSEOTHERSTEREO, OnUnloadOther)
	ON_COMMAND(ID_POPUP_OPENPHOTO, OnLoadPhotoStereo)
	ON_COMMAND(ID_LOAD_PROJECT, OnLoadProject)
	ON_COMMAND(ID_POPUP_UNLOAD, OnUnloadProject)
	ON_COMMAND(ID_POPUP_OPENREALTIMEEPIP, OnLoadRealtimeStereo)
	ON_COMMAND(ID_POPUP_RECALC_MODEL_BOUND, OnReCalcModelBound)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_HIDE_MAPINFO, OnHideMapinfo)
	ON_COMMAND(ID_POPUP_OPENEPIP, OnLoadFile)
//	ON_COMMAND(ID_DLGBAR_OPENFILE, OnLoadFile)
	ON_COMMAND(ID_MRU1,OnOpenDocimage1)
	ON_COMMAND(ID_MRU2,OnOpenDocimage2)
	ON_COMMAND(ID_MRU3,OnOpenDocimage3)
	ON_COMMAND(ID_MRU4,OnOpenDocimage4)
	ON_COMMAND(ID_MRU5,OnOpenDocimage5)
	ON_COMMAND(ID_MRU6,OnOpenDocimage6)
	ON_COMMAND(ID_MRU7,OnOpenDocimage7)
	ON_COMMAND(ID_MRU8,OnOpenDocimage8)
	ON_COMMAND(ID_MRU9,OnOpenDocimage9)
	ON_COMMAND(ID_MRU10,OnOpenDocimage10)
	ON_COMMAND(ID_MRU11,OnOpenDocimage11)
	ON_COMMAND(ID_MRU12,OnOpenDocimage12)
	ON_COMMAND(ID_MRU13,OnOpenDocimage13)
	ON_COMMAND(ID_MRU14,OnOpenDocimage14)
	ON_COMMAND(ID_MRU15,OnOpenDocimage15)
	ON_COMMAND(ID_MRU16,OnOpenDocimage16)
	ON_COMMAND(ID_MRU_CLR, OnMruClr)
	ON_COMMAND(ID_POPUP_SWITCHEPIP, OnPopupSwitchepip)
	ON_UPDATE_COMMAND_UI(ID_POPUP_OPENEPIP, OnUpdateOpenEpip)
	ON_UPDATE_COMMAND_UI(ID_POPUP_OPENPHOTO, OnUpdateOpenPhoto)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectViewBar message handlers

//##ModelId=41466B7F0119
int CProjectViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//从注册表中读取多模型显示窗口的显示信息
	int flag = AfxGetApp()->GetProfileInt(AfxGetApp()->m_pszAppName, "mapshowstate", -1);
	if (flag == 1 || flag == -1 || flag == 0)
	{
		m_bShowMapInfo = true;
	}
	if (flag == 2)
	{
		m_bShowMapInfo = false;
	}
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | 
								TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | 
								WS_CLIPCHILDREN;
	
	if (!m_wndAttribView.Create (dwViewStyle, rectDummy, this, 3))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}
	//by shy
	if (!m_mapctlModel.Create (NULL, NULL, WS_CHILD | WS_VISIBLE,
       rectDummy, this, 4))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}
	
	// Load images:
	m_AttribViewImages.Create(IDB_CLASS_VIEW, 16, 0, RGB(255, 0, 0));
	m_wndAttribView.SetImageList(&m_AttribViewImages, TVSIL_NORMAL);

	CreateToolBar(IDR_PROJECTBAR);

	AdjustLayout ();

//	AfxLinkCallback(FCCM_LOADPROJECT,this,(PFUNCALLBACK)OnCallLoad);
//	AfxLinkCallback(FCCM_GETPROJECT,this,(PFUNCALLBACK)OnGetProject);

	return 0;
}

//##ModelId=41466B7F0127
void CProjectViewBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout ();
}

HTREEITEM CProjectViewBar::InsertRoot(LPCTSTR lpszItem, DWORD_PTR data)
{
	HTREEITEM hRoot = m_wndAttribView.InsertItem (lpszItem,0,0);
	if( hRoot )
	{
		m_wndAttribView.SetItemState (hRoot, TVIS_BOLD, TVIS_BOLD);
		m_wndAttribView.SetItemData (hRoot, data);
	}

	return hRoot;
}

HTREEITEM CProjectViewBar::InsertSubRoot(LPCTSTR lpszItem, HTREEITEM hParent, DWORD_PTR data)
{ 
	HTREEITEM hSubRoot = m_wndAttribView.InsertItem (lpszItem, 1, 1, hParent);
	if( hSubRoot )
	{
		m_wndAttribView.SetItemData(hSubRoot, data);
		SetItemImage(hSubRoot, data);
	}

	return hSubRoot;
}

HTREEITEM CProjectViewBar::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD_PTR dwData)
{
	HTREEITEM hItem = m_wndAttribView.InsertItem (lpszItem, 3, 5, hParent);
	if( hItem )
	{
		m_wndAttribView.SetItemData(hItem, dwData);
		SetItemImage(hItem, dwData);
	}

	return hItem;
}

void CProjectViewBar::SetItemImage(HTREEITEM item, DWORD_PTR flag)
{
	int idx = -1;
	switch( flag )
	{
	case PROJECTITEM_ID: idx = 0; break;
	case DLGFILEITEM_ID: idx = 7; break;
	case MODELITEM_ID: idx = 5; break;
	case IMAGEITEM_ID: idx = 1; break;
	}

	if( idx>=0 && item )
		m_wndAttribView.SetItemImage(item,idx,idx);
}


//##ModelId=41466B7F0138
void CProjectViewBar::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndAttribView;
	ASSERT_VALID (pWndTree);

	if (point == CPoint(-1, -1))
		return;
	CPoint ptTree = point;
	pWndTree->ScreenToClient(&ptTree);

	HTREEITEM hTreeItem = pWndTree->HitTest(ptTree);
	if (hTreeItem == NULL)
		return;
	pWndTree->SelectItem(hTreeItem);
	int data = m_wndAttribView.GetItemData(hTreeItem);

	UINT nIdMenu = 0;
	switch( data )
	{
	case PROJECTITEM_ID: nIdMenu = IDR_POPUP_OPENPROJECT; break;
	case DLGFILEITEM_ID: nIdMenu = IDR_POPUP_OPENFILE; break;
	case MODELITEM_ID: nIdMenu = IDR_POPUP_OPENSTEREO; break;
	default: return;
	}

	pWndTree->SetFocus ();
	CMenu menu;
	menu.LoadMenu(nIdMenu);

	if (IDR_POPUP_OPENFILE == nIdMenu)
	{
		CString path = m_wndAttribView.GetItemText(hTreeItem);
		if (path.Left(6) != "uvs://")
		{
			menu.GetSubMenu(0)->RemoveMenu(ID_OPEN_LOCAL, MF_BYCOMMAND);
			menu.GetSubMenu(0)->RemoveMenu(ID_COMMIT_LOCAL, MF_BYCOMMAND);
		}
	}

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (IDR_POPUP_OPENSTEREO == nIdMenu)
	{
		CMenu* pMruMenu = menu.GetSubMenu(0);
		m_CMruFile.SetMenuPtr(pMruMenu);//设置menu对象
		m_CMruFile.ReadMru();
	}
	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
	   CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
	   
	   if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
		   return;
	   
	   ((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu (pPopupMenu);
	   UpdateDialogControls(this, FALSE);
	}
}

//##ModelId=41466B7F00DA
void CProjectViewBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect (rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout (FALSE, TRUE).cy;

	if(m_bShowMapInfo)
	{
		m_wndToolBar.SetWindowPos (NULL, rectClient.left, rectClient.top, 
									rectClient.Width (), cyTlb,
									SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndAttribView.SetWindowPos (NULL, rectClient.left + 1, rectClient.top + cyTlb + 1,
									rectClient.Width () - 2, (rectClient.Height () - 2*cyTlb-3)/2,
									SWP_NOACTIVATE | SWP_NOZORDER);
		m_mapctlModel.ShowWindow(TRUE);
		m_mapctlModel.SetWindowPos(NULL, rectClient.left + 1, rectClient.top  +cyTlb +(rectClient.Height () - 2*cyTlb-3)/2+2,
									rectClient.Width () - 2, (rectClient.Height () +3)/2-2,
									SWP_NOACTIVATE | SWP_NOZORDER);
		m_mapctlModel.AdjustLayout();
	}
	else
	{
		m_wndToolBar.SetWindowPos (NULL, rectClient.left, rectClient.top, 
			rectClient.Width (), cyTlb,
			SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndAttribView.SetWindowPos (NULL, rectClient.left + 1, rectClient.top + cyTlb + 1,
			rectClient.Width () - 2, rectClient.Height () - cyTlb - 2,
			SWP_NOACTIVATE | SWP_NOZORDER);
		m_mapctlModel.ShowWindow(FALSE);
/*		m_mapctlModel.SetWindowPos(NULL, 0,0,0,0,SWP_NOACTIVATE | SWP_NOZORDER);*/
	}
}

void CProjectViewBar::OnDestroy()
{
	if(m_bShowMapInfo)
		AfxGetApp()->WriteProfileInt(AfxGetApp()->m_pszAppName,"mapshowstate",1);
	else
		AfxGetApp()->WriteProfileInt(AfxGetApp()->m_pszAppName,"mapshowstate",2);

	CDockablePane::OnDestroy();
}

void CProjectViewBar::OnLoadFile()
{
	TCHAR Buffer[MAX_PATH];
	DWORD dwRet;	
	dwRet = GetCurrentDirectory(MAX_PATH, Buffer);

	HTREEITEM hItem = m_wndAttribView.GetSelectedItem();
	if( !hItem )return;
	DWORD_PTR data = m_wndAttribView.GetItemData(hItem);
	CString str = m_wndAttribView.GetItemText(hItem);

	if( data==DLGFILEITEM_ID )
		OpenDocFile(str);
	else if( data==MODELITEM_ID )
	{
		str = m_wndAttribView.GetItemText(hItem);
		CoreObject core = m_project.GetCoreObject();
		for( int i=0; i<core.iStereoNum; i++)
		{
			if( core.stereo[i].sp.stereoID==str )
			{
				gdwInnerCmdData[0] = i;
				gdwInnerCmdData[1] = (LPARAM)&core;
				int stereidx = gdwInnerCmdData[0]&0x0fffffff;
				CString strIndex,strStereID;
				strIndex.Format("%d",gdwInnerCmdData[0]);
				strStereID = core.stereo[stereidx].sp.stereoID;
				strStereID +=StrFromResID(IDS_EPIPMS);
				CMenu   menu,*pSubMenu;
				menu.LoadMenu(IDR_POPUP_OPENSTEREO);                //添加菜单资源
				pSubMenu=menu.GetSubMenu(0);           //设置菜单弹出项目起始位置
				CMenu* pMruMenu = menu.GetSubMenu(0);
				m_CMruFile.SetMenuPtr(pMruMenu);//设置menu对象
				m_CMruFile.AddMru(strStereID, strIndex);//添加菜单项
				::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_COMMAND,
					FCCMD_LOADSTEREO,0);
				CStereoView *pView = GetStereoView(0);
				if( pView )
				pView->SwitchModelForRefresh();
			}
		}
	}	
}



void CProjectViewBar::OnLoadPhotoStereo()
{
	HTREEITEM hItem = m_wndAttribView.GetSelectedItem();
	if( !hItem )return;
	DWORD_PTR data = m_wndAttribView.GetItemData(hItem);
	CString str = m_wndAttribView.GetItemText(hItem);
	
	if( data==MODELITEM_ID )
	{
		CoreObject core = m_project.GetCoreObject();
		for( int i=0; i<core.iStereoNum; i++)
		{
			if( core.stereo[i].sp.stereoID==str )
			{
				gdwInnerCmdData[0] = (i|0x80000000);
				gdwInnerCmdData[1] = (LPARAM)&core;
				int stereidx = gdwInnerCmdData[0]&0x0fffffff;
				CString strIndex,strStereID;
				strIndex.Format("%d",gdwInnerCmdData[0]);
				strStereID = core.stereo[stereidx].sp.stereoID;
				strStereID +=StrFromResID(IDS_PHOTOMS);
				CMenu   menu,*pSubMenu;
				menu.LoadMenu(IDR_POPUP_OPENSTEREO);                //添加菜单资源
				pSubMenu=menu.GetSubMenu(0);           //设置菜单弹出项目起始位置
				CMenu* pMruMenu = menu.GetSubMenu(0);
				m_CMruFile.SetMenuPtr(pMruMenu);//设置menu对象
				m_CMruFile.AddMru(strStereID, strIndex);//添加菜单项
				::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_COMMAND,
					FCCMD_LOADSTEREO,0);
				CStereoView *pView = GetStereoView(0);
				if( pView )
				pView->SwitchModelForRefresh();
			}
		}
	}	
}



void CProjectViewBar::OnLoadRealtimeStereo()
{
	HTREEITEM hItem = m_wndAttribView.GetSelectedItem();
	if( !hItem )return;
	DWORD data = m_wndAttribView.GetItemData(hItem);
	CString str = m_wndAttribView.GetItemText(hItem);
	
	if( data==MODELITEM_ID )
	{
		CoreObject core = m_project.GetCoreObject();
		for( int i=0; i<core.iStereoNum; i++)
		{
			if( core.stereo[i].sp.stereoID==str )
			{
				gdwInnerCmdData[0] = (i|0x40000000);
				gdwInnerCmdData[1] = (LPARAM)&core;
				int stereidx = (i|0x40000000)&0x0fffffff;
				CString strIndex,strStereID;
				strIndex.Format("%d",gdwInnerCmdData[0]);
				strStereID = core.stereo[stereidx].sp.stereoID;
				strStereID +=StrFromResID(IDS_REALTIMEEPIP);
				CMenu   menu,*pSubMenu;
				menu.LoadMenu(IDR_POPUP_OPENSTEREO);                //添加菜单资源
				pSubMenu=menu.GetSubMenu(0);           //设置菜单弹出项目起始位置
				CMenu* pMruMenu = menu.GetSubMenu(0);
				m_CMruFile.SetMenuPtr(pMruMenu);//设置menu对象
				m_CMruFile.AddMru(strStereID, strIndex);//添加菜单项	
				::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_COMMAND,
					FCCMD_LOADSTEREO,0);
				CStereoView *pView = GetStereoView(0);
				if( pView )
				pView->SwitchModelForRefresh();
			}
		}
	}	
}


void CProjectViewBar::OnLoadOther()
{
	OnUnloadOther();
	HTREEITEM hRoot0;
	
	hRoot0 = m_wndAttribView.GetRootItem();
	if( !hRoot0 )return;

	CoreObject core = m_project.GetCoreObject();
	int insert = 0;
	for( int i=0; i<core.iStereoNum; i++ )
	{
		BOOL bFind = FALSE;
		CString idstr1 = core.stereo[i].sp.stereoID;
		for( int j=0; j<core.tmp.dlgs.iDlgNum; j++)
		{
			for( int k=0; k<core.tmp.dlgs.dlg[j].iStereoNum; k++)
			{
				CString idstr2 = core.tmp.dlgs.dlg[j].stereoID[k];
				if( idstr1==idstr2 )
				{
					bFind = TRUE;
					break;
				}
			}
		}

		if( !bFind )
		{
			InsertSubRoot(idstr1,hRoot0,MODELITEM_ID);
			insert++;
		}
	}

	if( insert<=0 )
	{
		AfxMessageBox(IDS_LOADOTHER_NOSTEREO);
	}
	else 
		m_wndAttribView.Expand (hRoot0, TVE_EXPAND);
}

void CProjectViewBar::OnUnloadOther()
{
	HTREEITEM hChild0, hChild1;
	
	hChild0 = m_wndAttribView.GetRootItem();
	if( !hChild0 )return;

	hChild1 = m_wndAttribView.GetChildItem(hChild0);
	while( hChild1 ) 
	{
		hChild0 = m_wndAttribView.GetNextSiblingItem(hChild1);
		if( m_wndAttribView.GetItemData(hChild1)==MODELITEM_ID )
			m_wndAttribView.DeleteItem(hChild1);
		hChild1 = hChild0;
	}
}


void CProjectViewBar::OnUnloadProject()
{
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	pApp->OnUninstallProject();	
	m_prjname = "";
}

//##ModelId=41466B7F0185
void CProjectViewBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(&rect);
	//ScreenToClient (rect);
	dc.FillSolidRect(&rect,RGB(0,255,255));

	m_wndAttribView.GetWindowRect (rect);
	ScreenToClient (rect);

	rect.InflateRect (1, 1);
	dc.Draw3dRect (rect, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
	if(m_bShowMapInfo)
	{
		m_mapctlModel.GetWindowRect (rect);
		ScreenToClient (rect);
		
		rect.InflateRect (1, 1);
		dc.Draw3dRect (rect, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
	}

}

LONG CProjectViewBar::OnGetProject(WPARAM wParam,LPARAM lParam)
{
	if( !lParam )return 1;
	CGeoBuilderPrj **prj = (CGeoBuilderPrj**)lParam;
	*prj = &m_project;
	return 0;
}

LONG CProjectViewBar::OnCallLoad(WPARAM wParam,LPARAM lParam)
{
	if( wParam==FCPV_PROJECT )
		return LoadProject((LPCTSTR)lParam);

	if( !lParam )return 0;
	else if( wParam==FCPV_DLGFILE )
	{
		OpenDocFile((LPCTSTR)lParam);
		return 1;
	}
	else if( wParam==FCPV_STEREO )
	{
		CoreObject core = m_project.GetCoreObject();
		for( int i=0; i<core.iStereoNum; i++)
		{
			if( core.stereo[i].sp.stereoID==(LPCTSTR)lParam )
			{
				gdwInnerCmdData[0] = i;
				gdwInnerCmdData[1] = (LPARAM)&core;
				::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_COMMAND,
					FCCMD_LOADSTEREO,0);
			}
		}
		return 1;
	}

	return 0;
}

BOOL CProjectViewBar::LoadProject(LPCTSTR fileName)
{ 
	static BOOL bLoadingProject = FALSE;

	if (bLoadingProject) return FALSE;

	bLoadingProject = TRUE;

	if( fileName==NULL )
	{
		m_wndAttribView.DeleteAllItems();

		m_wndToolBar.DestroyWindow();
		CreateToolBar(IDR_PROJECTBAR);
	}
	else
	{
		CString fileName2, prjname;
		BOOL bLoadAllStereos = FALSE;

		if( fileName[0]==_T('*') )
		{
			fileName2 = fileName+1;
			bLoadAllStereos = TRUE;
		}
		else
		{
			fileName2 = fileName;
			bLoadAllStereos = FALSE;
		}

		int pos = fileName2.ReverseFind(_T('.'));
		BOOL bIsMapMatrixPrj = TRUE;

		if( fileName2.Mid(pos).CompareNoCase(_T(".mdl"))==0 )
		{
			return FALSE;
 /*			ImportDPW * import = new ImportLens;
			if( import )
			{
				CString strTitle;
				strTitle = import->FullPathToName(fileName2); 
				if( import->Load(import->FullPathToPath(fileName2),strTitle)==true )
				{
					prjname = import->Save(strTitle); 
				}
				delete import;
			}

			if( prjname.IsEmpty() )
			{
				bLoadingProject = FALSE;
				return FALSE;
			}

			bIsMapMatrixPrj = FALSE;*/
		}
		else
		{
			prjname = fileName2;
		}
		
		//m_project.SetProgress(AfxGetMainWnd()->GetSafeHwnd());
		m_project.LoadProject(prjname);
		//GProgressEnd();

		CoreObject core = m_project.GetCoreObject();

		//by shy
		m_mapctlModel.SetModelScopeArray(&core);
		m_mapctlModel.InitialDisplay();

		// 更新矢量视图范围
		CDlgDoc *pDoc = GetActiveDlgDoc();
		if (pDoc)
		{
			PT_3D pt3ds[4];
			memset(pt3ds,0,sizeof(pt3ds));
			pDoc->GetDlgDataSource()->GetBound(pt3ds,NULL,NULL);
			m_mapctlModel.OnRefreshVecWin(0,LPARAM(pt3ds));
		}

 		m_mapctlModel.Invalidate();

		m_wndAttribView.DeleteAllItems();

		HTREEITEM hRoot0,hRoot00,hRoot000,hItem;
		hRoot0 = InsertRoot(core.pp.strPrjName,PROJECTITEM_ID);

		if( bIsMapMatrixPrj )
		{
			{
				for( int i=0; hRoot0 && i<core.tmp.dlgs.iDlgNum; i++)
				{
					hRoot00 = InsertSubRoot(core.tmp.dlgs.dlg[i].strDlgFile,hRoot0,DLGFILEITEM_ID);
					if( !hRoot00 )continue;

					for( int j=0; j<core.tmp.dlgs.dlg[i].iStereoNum; j++)
					{
						CString idstr = core.tmp.dlgs.dlg[i].stereoID[j];
						hRoot000 = NULL;
						for( int k=0; k<core.iStereoNum; k++)
						{
							if( core.stereo[k].sp.stereoID==idstr )
							{
								hRoot000 = InsertSubRoot(idstr,hRoot00,MODELITEM_ID);
								break;
							}
						}

						if( k<core.iStereoNum && hRoot000 )
						{
							hItem = InsertItem(core.stereo[k].imageID[0],hRoot000,IMAGEITEM_ID);
							hItem = InsertItem(core.stereo[k].imageID[1],hRoot000,IMAGEITEM_ID);
						}
					}

					m_wndAttribView.Expand (hRoot00, TVE_EXPAND);
				}
			}

			if( bLoadAllStereos )
			{
				HTREEITEM hRoot0;
				
				hRoot0 = m_wndAttribView.GetRootItem();
				
				CoreObject core = m_project.GetCoreObject();
				int insert = 0;
				for( int i=0; hRoot0 && i<core.iStereoNum; i++ )
				{
					BOOL bFind = FALSE;
					CString idstr1 = core.stereo[i].sp.stereoID;
					for( int j=0; j<core.tmp.dlgs.iDlgNum; j++)
					{
						for( int k=0; k<core.tmp.dlgs.dlg[j].iStereoNum; k++)
						{
							CString idstr2 = core.tmp.dlgs.dlg[j].stereoID[k];
							if( idstr1==idstr2 )
							{
								bFind = TRUE;
								break;
							}
						}
					}
					
					if( !bFind )
					{
						InsertSubRoot(idstr1,hRoot0,MODELITEM_ID);
						insert++;
					}
				}
			}
		}
		else
		{
			CString strFormat, strMsg;
			strFormat.LoadString(IDS_CREATE_TILEIMAGE);

			int i, j, nImage = 0, nImageCount = 0;

			for( i=0; hRoot0 && i<core.iStripNum; i++ )
			{
				for(j=0;j<core.strip[i].iImageNum;j++)
				{
					nImageCount++;
				}
			}

			for( i=0; hRoot0 && i<core.iStereoNum; i++ )
			{
				CString idstr1 = core.stereo[i].sp.stereoID;

				InsertSubRoot(idstr1,hRoot0,MODELITEM_ID);
			}

			for( i=0; hRoot0 && i<core.iStripNum; i++ )
			{
				for(j=0;j<core.strip[i].iImageNum;j++)
				{
					BOOL bFind = FALSE;
					CString idstr1 = core.strip[i].image[j].strImageID;
					
					strMsg.Format(strFormat,nImage++,nImageCount,(LPCTSTR)idstr1);
					AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)strMsg );

					if( !bFind )
					{
						/*CGeoBuilderEpipolar gpep;
						gpep.EnableConvert();
						gpep.Init(&core,core.strip[i].image[j].strImageID);
						gpep.Run();*/
					}
				}
				AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);
			}
			AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_END_TILEIMAGE) );

		}

		m_wndAttribView.Expand (hRoot0, TVE_EXPAND);
	
		m_wndToolBar.DestroyWindow();
		CreateToolBar(IDR_DLGBAR);

		AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_LOADPROJECT_END));

		m_prjname = prjname;
	}
	
	AdjustLayout();
	
	bLoadingProject = FALSE;

	if (!this->IsWindowVisible())
		this->ShowPane(TRUE, FALSE, TRUE);

	return TRUE;
}

void CProjectViewBar::OnLoadProject() 
{
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	pApp->OnLoadProject();
}


void CProjectViewBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_wndAttribView.GetCount()<=0 )
	{
		CDockablePane::OnLButtonDown(nFlags, point);
		return;
	}

	//选中的项
	HTREEITEM hItem = m_wndAttribView.GetSelectedItem();
	if( !hItem )
	{
		CDockablePane::OnLButtonDown(nFlags, point);
		return;
	}

	DWORD_PTR seldata = m_wndAttribView.GetItemData(hItem);

	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndAttribView;
	ASSERT_VALID (pWndTree);
	
	int data = 0;
	if (point != CPoint (-1, -1))
	{
		CPoint ptTree = point;
		pWndTree->ScreenToClient (&ptTree);
		
		HTREEITEM hTreeItem = pWndTree->HitTest (ptTree);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem (hTreeItem);
			data = m_wndAttribView.GetItemData(hTreeItem);
		}
	}

	if( data!=seldata )
	{
		int toolid = -1;
		switch( data )
		{
		case PROJECTITEM_ID: break;
		case DLGFILEITEM_ID: break;
		case MODELITEM_ID: break;
		case IMAGEITEM_ID: break;
		}
	}
	
	CDockablePane::OnLButtonDown(nFlags, point);
}

BOOL CProjectViewBar::CreateToolBar(UINT id)
{
	m_wndToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_HIDE_INPLACE, id);
	m_wndToolBar.LoadToolBar (id, 0, 0, TRUE);
	
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
	
	m_wndToolBar.SetPaneStyle(
		m_wndToolBar.GetPaneStyle() &
		~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	
	m_wndToolBar.SetOwner (this);
	
	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame (FALSE);
	return TRUE;
}

void CProjectViewBar::OnHideMapinfo() 
{
	// TODO: Add your command handler code here
	m_bShowMapInfo=!m_bShowMapInfo;
	AdjustLayout();
	
}

void CProjectViewBar::OnPopupSwitchepip() 
{
	HTREEITEM hItem = m_wndAttribView.GetSelectedItem();
	if( !hItem )return;
	DWORD_PTR data = m_wndAttribView.GetItemData(hItem);
	CString str = m_wndAttribView.GetItemText(hItem);
	
	if( data==DLGFILEITEM_ID )
		OpenDocFile(str);
	else if( data==MODELITEM_ID )
	{
		CoreObject core = m_project.GetCoreObject();
		for( int i=0; i<core.iStereoNum; i++)
		{
			if( core.stereo[i].sp.stereoID==str )
			{
				gdwInnerCmdData[0] = (gdwInnerCmdData[0]&(~0xfffffff))|i;
				gdwInnerCmdData[1] = (LPARAM)&core;
				//by shy
				BOOL bDoubleScreen	= AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DOUBLESCREEN, gdef_bDoubleScreen);
				CView *pView=NULL;
				if (!bDoubleScreen)
				{
					CMDIFrameWndEx *pFrame =
						(CMDIFrameWndEx*)AfxGetApp()->m_pMainWnd;
					
					// Get the active MDI child window.
					CMDIChildWnd *pChild = 
						(CMDIChildWnd *) pFrame->GetActiveFrame();
					
					// or CMDIChildWnd *pChild = pFrame->MDIGetActive();
					
					// Get the active view attached to the active MDI child
					// window.
					pView=pChild->GetActiveView();
					if (pView==NULL)
					{
						return;
					}
				}
				else
				{
				
					POSITION DocTempPos=AfxGetApp()->GetFirstDocTemplatePosition();
					while(DocTempPos!=NULL)
					{
						CDocTemplate* curTemplate=AfxGetApp()->GetNextDocTemplate(DocTempPos);
						POSITION DocPos=curTemplate->GetFirstDocPosition();
						while(DocPos!=NULL)
						{
							CDocument *pDoc=curTemplate->GetNextDoc(DocPos);
							if (pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)))
							{
								POSITION ViewPos=pDoc->GetFirstViewPosition();
						    	while(ViewPos)
								{
									pView=pDoc->GetNextView(ViewPos);
									if(pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
									{
										CFrameWnd *pFrame=pView->GetParentFrame();
										if (pFrame->IsKindOf(RUNTIME_CLASS(CStereoFrame)))
										{
											goto end ;
										}
									}
									  
								}
							}
						}
															
					}					
				}
end:
				if (!pView||!pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
				{
					return;
				}
			//	HWND hWnd=::GetFocus();
				::SendMessage(pView->m_hWnd,WM_STEREOVIEW_LOADMDL,
				gdwInnerCmdData[0],gdwInnerCmdData[1]);
			//	((CStereoView*)pView)->StereoMove();
		//		::SetFocus(hWnd);
				pView->SetFocus();
			//	pView->SetActiveWindow();//为何没有效果
//  				CFrameWnd *pWnd=(pView->GetParentFrame());
//  					pWnd->SetActiveView(pView);//为什么调试的时候进不去？
				((CStereoView*)pView)->SwitchModelForRefresh();
				//((CStereoView*)pView)->SetFocus();
			//	pView->OnActivateView(1, pView, pView);
			
			}
		}
	}	
}


BOOL ProjectView_CanOpenStereo()
{
	return TRUE;

	BOOL bTextStereo = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_TEXTSTEREO,gdef_bTextStereo);

	BOOL bFindStereoView = FALSE;

	CWinApp *pApp = AfxGetApp();
	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();
	
	while( curTemplatePos!=NULL && !bFindStereoView )
	{
		CDocTemplate* curTemplate = pApp->GetNextDocTemplate(curTemplatePos);
		
		CDocument *pDoc;
		POSITION curDocPos = curTemplate->GetFirstDocPosition();
		while( curDocPos!=NULL && !bFindStereoView )
		{
			pDoc = curTemplate->GetNextDoc(curDocPos);
			if( pDoc )
			{
				CView *pView;
				POSITION curViewPos = pDoc->GetFirstViewPosition();
				while( curViewPos!=NULL && !bFindStereoView )
				{
					pView = pDoc->GetNextView(curViewPos);
					if( pView!=NULL && pView->IsKindOf(RUNTIME_CLASS(CStereoView)) )
					{
						bFindStereoView = TRUE;
					}
				}
			}
		}
	}

	if( bTextStereo && bFindStereoView )
	{
		return FALSE;
	}

	return TRUE;
}

void CProjectViewBar::OnUpdateOpenEpip(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ProjectView_CanOpenStereo());
}


void CProjectViewBar::OnUpdateOpenPhoto(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ProjectView_CanOpenStereo());
}

LRESULT CProjectViewBar::UnInstall(WPARAM wParam,LPARAM lParam)
{
	return m_mapctlModel.OnUnInstallProj(wParam,lParam);
}

LRESULT CProjectViewBar::RefreshVecWin(WPARAM wParam, LPARAM lParam)
{
	return m_mapctlModel.OnRefreshVecWin(wParam,lParam);
}

LRESULT CProjectViewBar::RefreshCurrentStereoWin(WPARAM wParam, LPARAM lParam)
{
	return m_mapctlModel.OnRefreshStereoWin(wParam,lParam);
}

LRESULT CProjectViewBar::RefreshCurrentStereoMS(WPARAM wParam, LPARAM lParam)
{
	return m_mapctlModel.OnRefreshStereoMS(wParam,lParam);
}

LRESULT CProjectViewBar::RefreshVecflag(WPARAM wParam, LPARAM lParam)
{
	return m_mapctlModel.OnRefreshVecFlag(wParam,lParam);
}

LRESULT CProjectViewBar::SwitchStereoMS(WPARAM wParam, LPARAM lParam)
{
	return m_mapctlModel.OnSwitchStereoMS(wParam,lParam);
}

LRESULT CProjectViewBar::GetToler(WPARAM wParam, LPARAM lParam)
{
	return m_mapctlModel.OnGetToler(wParam,lParam);
}

void CProjectViewBar::OnReCalcModelBound()
{
	CoreObject core = m_project.GetCoreObject();
	//by shy
	m_mapctlModel.SetModelScopeArray(&core,FALSE);
	m_mapctlModel.InitialDisplay();
	// 更新矢量视图范围
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (pDoc)
	{
		PT_3D pt3ds[4];
		memset(pt3ds,0,sizeof(pt3ds));
		pDoc->GetDlgDataSource()->GetBound(pt3ds,NULL,NULL);
		m_mapctlModel.OnRefreshVecWin(0,LPARAM(pt3ds));
	}
	
 	m_mapctlModel.Invalidate();
}
void CProjectViewBar::OnOpenDocimage(int index)
{
	ASSERT(index>=0);
	int stereidx ;
	CString steroindex=	m_CMruFile.m_nIndex[index];
	CString strpathname = m_CMruFile.m_PathName[index];
	stereidx = atoi(steroindex);
	CoreObject core = m_project.GetCoreObject();
	if (steroindex.IsEmpty()||strpathname.IsEmpty())
	{
		m_CMruFile.DelMru(strpathname,index);	
		return;
	}
	int nindex = strpathname.Find("->");
	CString stropenType = strpathname.Right(strpathname.GetLength()-nindex);
	CString strstereoid = strpathname.Left(nindex);
	if (stropenType.IsEmpty()||strstereoid.IsEmpty())
	{
		m_CMruFile.DelMru(strpathname,index);
		return;
	}
	if (0 == stropenType.Compare(StrFromResID(IDS_PHOTOMS))|| 0 == stropenType.Compare(StrFromResID(IDS_EPIPMS)) )//原始像对
	{
		int nTindex = 	stereidx&0x0fffffff;
		CoreObject core = m_project.GetCoreObject();
		if (nTindex>=core.iStereoNum)
		{
			m_CMruFile.DelMru(strpathname,index);
			return;
		}
		CString strStereID = core.stereo[nTindex].sp.stereoID;
		if (0 != strStereID.Compare(strstereoid))
		{
			m_CMruFile.DelMru(strpathname,index);
			return;
		}
	}
	else if(0 == stropenType.Compare(StrFromResID(IDS_REALTIMEEPIP)))//实时核线
	{
		int nTindex = 	(stereidx|0x40000000)&0x0fffffff;
		CoreObject core = m_project.GetCoreObject();
		if (nTindex>=core.iStereoNum)
		{
			m_CMruFile.DelMru(strpathname,index);
			return;
		}
		CString strStereID = core.stereo[nTindex].sp.stereoID;
		if (0 != strStereID.Compare(strstereoid))
		{
			m_CMruFile.DelMru(strpathname,index);
			return;
		}
	}
	gdwInnerCmdData[0] = stereidx;
	gdwInnerCmdData[1] = (LPARAM)&core;
	m_CMruFile.AddMru(m_CMruFile.m_PathName[index],steroindex);	
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_COMMAND,
		FCCMD_LOADSTEREO,0);
	CStereoView *pView = GetStereoView(0);
	if( pView )
		pView->SwitchModelForRefresh();
}
void CProjectViewBar::OnOpenDocimage1()
{
	OnOpenDocimage(0);
}
void CProjectViewBar::OnOpenDocimage2()
{
	OnOpenDocimage(1);
}
void CProjectViewBar::OnOpenDocimage3()
{
	OnOpenDocimage(2);
}
void CProjectViewBar::OnOpenDocimage4()
{
	OnOpenDocimage(3);
}
void CProjectViewBar::OnOpenDocimage5()
{
	OnOpenDocimage(4);
}
void CProjectViewBar::OnOpenDocimage6()
{
	OnOpenDocimage(5);
}
void CProjectViewBar::OnOpenDocimage7()
{
	OnOpenDocimage(6);
}
void CProjectViewBar::OnOpenDocimage8()
{
	OnOpenDocimage(7);
}
void CProjectViewBar::OnOpenDocimage9()
{
	OnOpenDocimage(8);
}
void CProjectViewBar::OnOpenDocimage10()
{
	OnOpenDocimage(9);
}
void CProjectViewBar::OnOpenDocimage11()
{
	OnOpenDocimage(10);
}
void CProjectViewBar::OnOpenDocimage12()
{
	OnOpenDocimage(11);
}
void CProjectViewBar::OnOpenDocimage13()
{
	OnOpenDocimage(12);
}
void CProjectViewBar::OnOpenDocimage14()
{
	OnOpenDocimage(13);
}
void CProjectViewBar::OnOpenDocimage15()
{
	OnOpenDocimage(14);
}
void CProjectViewBar::OnOpenDocimage16()
{
	OnOpenDocimage(15);
}
void CProjectViewBar::OnMruClr()
{
	m_CMruFile.ClearMru();
}

void CProjectViewBar::OpenDocFile(LPCTSTR name)
{
	CString name1 = name;
	if (name1.Left(6) == "uvs://")   //"uvs://budongchan/test1"
	{
		CDlgConnectDB dlg;

		if (!dlg.ConnectDB())
			return;

		int pos = name1.Find('/', 6);
		if (pos < 0) return;
		CString workspacename = name1.Mid(6, pos-6);

		CString current_workspace = CUVSModify::GetCurrentWorkspace();
		if (current_workspace.IsEmpty())
		{
			dlg.m_current_wp = workspacename;
			if (!dlg.OpenWorkSpace())
			{
				return;
			}
		}
		else if (workspacename != current_workspace)
		{
			AfxMessageBox(IDS_WP_NOT_MATCH);
			return;
		}
		
		CString pathName = name1.Mid(pos + 1);

		if (!CUVSModify::IsMapsheetExist(pathName))
		{
			CDlgWorkSpaceBound dlgB;
			dlgB.m_lfX1 = 0;
			dlgB.m_lfY1 = 0;
			dlgB.m_lfX2 = 1000;
			dlgB.m_lfY2 = 0;
			dlgB.m_lfX3 = 1000;
			dlgB.m_lfY3 = 1000;
			dlgB.m_lfX4 = 0;
			dlgB.m_lfY4 = 1000;
			dlgB.m_lfZmin = -1000.0;
			dlgB.m_lfZmax = 1000.0;
			//从xml同级目录下的.workboundary读取工作区范围
			CString boundpath = m_prjname.Left(m_prjname.GetLength()-3);
			boundpath += "workboundary";
			if (!PathFileExists(boundpath))
			{
				int pos = boundpath.ReverseFind('\\');
				CString boundpath1 = boundpath.Left(pos+1);
				boundpath = boundpath1 + pathName + ".workboundary";
			}
			FILE *fp = fopen(boundpath, "r");
			if (fp)
			{
				int num = 0;
				fscanf(fp, "%d", &num);
				for (int i = 0; i < num; i++)
				{
					TCHAR temp[128] = { 0 };
					fscanf(fp, "%s", temp);
					double vals[10];
					fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", vals, vals + 1, vals + 2, vals + 3, vals + 4, vals + 5, vals + 6, vals + 7, vals + 8, vals + 9);
					if (0 == pathName.CompareNoCase(temp))
					{
						dlgB.m_lfX1 = vals[0];
						dlgB.m_lfY1 = vals[1];
						dlgB.m_lfX2 = vals[2];
						dlgB.m_lfY2 = vals[3];
						dlgB.m_lfX3 = vals[4];
						dlgB.m_lfY3 = vals[5];
						dlgB.m_lfX4 = vals[6];
						dlgB.m_lfY4 = vals[7];
						break;
					}
				}
				fclose(fp);
			}
			dlgB.m_bUVS = TRUE;
			if (dlgB.DoModal() != IDOK)
				return;

			if (!CUVSModify::AddMapsheet(pathName, dlgB.m_lfX1, dlgB.m_lfY1, dlgB.m_lfX2, dlgB.m_lfY2,
				dlgB.m_lfX3, dlgB.m_lfY3, dlgB.m_lfX4, dlgB.m_lfY4, dlgB.m_lfZmin, dlgB.m_lfZmax))
			{
				AfxMessageBox(IDS_CREATE_UVS_FAILE);
				return;
			}
		}

		theApp.OpenFileOnUVS(pathName);
	}
	else
	{
		AfxGetApp()->OpenDocumentFile(name);
	}
}

void CProjectViewBar::OnOpenLocalFile()
{
	HTREEITEM hItem = m_wndAttribView.GetSelectedItem();
	if (!hItem)return;
	CString path = m_wndAttribView.GetItemText(hItem);
	if (path.Left(6) != "uvs://")
		return;

	m_strUVSPath = path;

	int pos0 = path.ReverseFind('/');
	if (pos0 < 0) return;
	int pos1 = m_prjname.ReverseFind('\\');
	if (pos1 < 0) return;
	m_wndAttribView.GetSelectedItem();

	CString workspacename = path.Mid(6, pos0 - 6);
	CString mapsheetname = path.Mid(pos0 + 1);
	CString strLocalPath = m_prjname.Left(pos1) + "\\DLG\\" + path.Mid(pos0+1) + ".FDB";
	//DeleteFile(strLocalPath);
	DeleteFile(strLocalPath+".tmp");

	CDlgConnectDB dlg;
	if (!dlg.ConnectDB())
		return;
	CString current_workspace = CUVSModify::GetCurrentWorkspace();
	if (current_workspace.IsEmpty())
	{
		dlg.m_current_wp = workspacename;
		if (!dlg.OpenWorkSpace())
		{
			return;
		}
	}
	else if (workspacename != current_workspace)
	{
		AfxMessageBox(IDS_WP_NOT_MATCH);
		return;
	}

	//读取UVS
	CUVSModify  *pUVSData = new CUVSModify;
	if (!pUVSData->Attach(mapsheetname))
		return;
	CDataQueryEx *pDQ = new CDataQueryEx();
	CDlgDataSource *pDS = new CDlgDataSource(pDQ);
	if (!pDS) return;
	pUVSData->SetDataSource(pDS);
	pDS->SetAccessObject(pUVSData);
	pUVSData->ReadDataSourceInfo(pDS);
	pUVSData->BatchUpdateBegin();
	pDS->LoadAll(NULL);
	pUVSData->BatchUpdateEnd();

	//读取FDB
	CSQLiteAccess *pFdb = new CSQLiteAccess();
	CDataQueryEx *pDQ1 = new CDataQueryEx();
	CDlgDataSource *pDataSource = new CDlgDataSource(pDQ1);
	if (!pDataSource)
	{
		delete pDS;
		return;
	}

	pDataSource->SetAccessObject(pFdb);
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	CScheme *pScheme = pCfgLibManager->GetScheme(pDS->GetScale());
	DWORD scale = pCfgLibManager->GetScaleByScheme(pScheme);
	if (scale == 0)
	{
		delete pDS;
		delete pDataSource;
		return;
	}

	if (!pFdb->Attach(strLocalPath))
	{
		pFdb->BatchUpdateBegin();
		pFdb->CreateFileSys(scale, pScheme);
		pFdb->BatchUpdateEnd();
	}

	pDataSource->LoadAll(NULL, FALSE);
	pFdb->BatchUpdateBegin();
	pDataSource->ResetDisplayOrder();
	pDataSource->SaveAllLayers();
	pFdb->BatchUpdateEnd();
	
	//写入工作区信息
	pDataSource->SetScale(pDS->GetScale());
	PT_3D pts[4];
	double zmin, zmax;
	pDS->GetBound(pts, &zmin, &zmax);
	pDataSource->SetBound(pts, zmin, zmax);

	//清空fdb非mdblayers数据
	pFdb->BatchUpdateBegin();
	if (1)
	{
		CString mdblayers("MdbLayers");
		CString shpLayers("ShpLayers");
		int nLay = pDataSource->GetFtrLayerCount();
		for (int i = 0; i < nLay; i++)
		{
			CFtrLayer *pLayer = pDataSource->GetFtrLayerByIndex(i);
			if (!pLayer)continue;
			if (pLayer->GetGroupName() == mdblayers || pLayer->GetGroupName() == shpLayers)
				continue;

			int nObj = pLayer->GetObjectCount();
			for (int j = 0; j < nObj; j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (pFtr)
				{
					pDataSource->DeleteObject(pFtr);
				}
			}
		}
	}
	pFdb->BatchUpdateEnd();

	//复制数据
	CFtrLayer *pLayer = NULL, *pLayer0 = NULL;
	CFeature *pFtr = NULL, *pFtr0 = NULL;
	CValueTable tab;
	pFdb->BatchUpdateBegin();
	if (1)
	{
		int nSum = 0;
		int nLay = pDS->GetFtrLayerCount();
		for (int i = 0; i < nLay; i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if (!pLayer)continue;
			int nObj = pLayer->GetObjectCount();
			for (int j = 0; j < nObj; j++)
			{
				pFtr = pLayer->GetObject(j, FILTERMODE_DELETED);
				if (pFtr)
				{
					nSum++;
				}
			}
		}
		GProgressStart(nSum);
		for (i = 0; i < nLay; i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if (!pLayer)continue;

			pLayer0 = pDataSource->GetFtrLayer(pLayer->GetName());
			if (!pLayer0)
			{
				pLayer0 = new CFtrLayer;
				if (!pLayer0)continue;
				tab.DelAll();
				tab.BeginAddValueItem();
				pLayer->WriteTo(tab);
				tab.EndAddValueItem();
				pLayer0->ReadFrom(tab);
				pLayer0->SetID(0);
				pDataSource->AddFtrLayer(pLayer0);
			}
			int nObj = pLayer->GetObjectCount();
			for (int j = 0; j < nObj; j++)
			{
				pFtr = pLayer->GetObject(j, FILTERMODE_DELETED);
				if (pFtr)
				{
					GProgressStep();
					pFtr0 = pFtr->Clone();
					pFtr0->SetID(OUID());
					if (!pDataSource->AddObject(pFtr0, pLayer0->GetID()))
					{
						delete pFtr0;
						continue;
					}
					tab.DelAll();
					tab.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr, tab);
					tab.EndAddValueItem();
					pDataSource->GetXAttributesSource()->SetXAttributes(pFtr0, tab);
				}
			}
		}
		GProgressEnd();
	}
	pFdb->BatchUpdateEnd();

	delete pDS;
	delete pDataSource;

	//若UVS数据已经被打开，则关闭它
	POSITION DocTempPos = AfxGetApp()->GetFirstDocTemplatePosition();
	while (DocTempPos != NULL)
	{
		CDocTemplate* curTemplate = AfxGetApp()->GetNextDocTemplate(DocTempPos);
		POSITION DocPos = curTemplate->GetFirstDocPosition();
		while (DocPos != NULL)
		{
			CDocument *pDoc = curTemplate->GetNextDoc(DocPos);
			if (pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)))
			{
				CDataSourceEx *pDS2 = ((CDlgDoc*)pDoc)->GetDlgDataSource();
				if (!pDS2) continue;
				CString name = pDS2->GetName();
				if (name==mapsheetname)
				{
					POSITION ViewPos = pDoc->GetFirstViewPosition();
					while (ViewPos)
					{
						CView *pView = pDoc->GetNextView(ViewPos);
						pView->GetParentFrame()->SendMessage(WM_CLOSE);
					}
				}
			}
		}
	}

	AfxGetApp()->OpenDocumentFile(strLocalPath);

	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (!pDoc) return;
	CDlgDataSource *pDS1 = pDoc->GetDlgDataSource();
	if (!pDS1 || pDS1->GetName() != strLocalPath)
		return;
	pDS1->m_bUVSLocalCopy = TRUE;
}

void CProjectViewBar::OnCommitLocalFile()
{
	if (IDYES != AfxMessageBox(IDS_ASK_FOR_COMMIT, MB_YESNO))
		return;

	CString path = m_strUVSPath;
	if (path.Left(6) != "uvs://")
		return;

	int pos0 = path.ReverseFind('/');
	if (pos0 < 0) return;
	int pos1 = m_prjname.ReverseFind('\\');
	if (pos1 < 0) return;
	m_wndAttribView.GetSelectedItem();

	CString workspacename = path.Mid(6, pos0 - 6);
	CString mapsheetname = path.Mid(pos0 + 1);
	CString strLocalPath = m_prjname.Left(pos1) + "\\DLG\\" + path.Mid(pos0 + 1) + ".FDB";

	CDlgConnectDB dlg;
	if (!dlg.ConnectDB())
		return;
	CString current_workspace = CUVSModify::GetCurrentWorkspace();
	if (current_workspace.IsEmpty())
	{
		dlg.m_current_wp = workspacename;
		if (!dlg.OpenWorkSpace())
		{
			return;
		}
	}
	else if (workspacename != current_workspace)
	{
		AfxMessageBox(IDS_WP_NOT_MATCH);
		return;
	}

	//读取FDB
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (!pDoc) return;
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if (!pDS) return;
	if (pDS->GetName() != strLocalPath)
		return;

	//打开UVS
	CUVSModify  *pUVSData = new CUVSModify;
	if (!pUVSData->Attach(mapsheetname))
		return;
	CDataQueryEx *pDQ = new CDataQueryEx();
	CDlgDataSource *pDataSource = new CDlgDataSource(pDQ);
	if (!pDataSource) return;
	pUVSData->SetDataSource(pDataSource);
	pDataSource->SetAccessObject(pUVSData);
	pUVSData->ReadDataSourceInfo(pDataSource);
	pUVSData->BatchUpdateBegin();
	pDataSource->LoadAll(NULL);
	pUVSData->BatchUpdateEnd();

	//复制数据
	CFtrLayer *pLayer = NULL, *pLayer0 = NULL;
	CFeature *pFtr = NULL, *pFtr0 = NULL;
	CValueTable tab;

	//清空UVS图幅
	pUVSData->BatchUpdateBegin();
	if (1)
	{
		int nLay = pDataSource->GetFtrLayerCount();
		for (int i = 0; i < nLay; i++)
		{
			pLayer = pDataSource->GetFtrLayerByIndex(i);
			if (!pLayer)continue;

			int nObj = pLayer->GetObjectCount();
			for (int j = 0; j < nObj; j++)
			{
				pFtr = pLayer->GetObject(j);
				if (pFtr)
				{
					pDataSource->DeleteObject(pFtr);
				}
			}
		}
	}
	pUVSData->BatchUpdateEnd();

	CString mdblayers("MdbLayers");
	CString shpLayers("ShpLayers");
	pUVSData->BatchUpdateBegin();
	if (1)
	{
		int nSum = 0;
		int nLay = pDS->GetFtrLayerCount();
		for (int i = 0; i < nLay; i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if (!pLayer)continue;
			if (pLayer->GetGroupName() == mdblayers || pLayer->GetGroupName() == shpLayers)
				continue;
			int nObj = pLayer->GetObjectCount();
			for (int j = 0; j < nObj; j++)
			{
				pFtr = pLayer->GetObject(j, FILTERMODE_DELETED);
				if (pFtr)
				{
					nSum++;
				}
			}
		}
		GProgressStart(nSum);
		for (i = 0; i < nLay; i++)
		{
			pLayer = pDS->GetFtrLayerByIndex(i);
			if (!pLayer)continue;
			if (pLayer->GetGroupName() == mdblayers)
				continue;

			pLayer0 = pDataSource->GetFtrLayer(pLayer->GetName());
			if (!pLayer0)
			{
				pLayer0 = new CFtrLayer;
				if (!pLayer0)continue;
				tab.DelAll();
				tab.BeginAddValueItem();
				pLayer->WriteTo(tab);
				tab.EndAddValueItem();
				pLayer0->ReadFrom(tab);
				pLayer0->SetID(0);
				pDataSource->AddFtrLayer(pLayer0);
			}
			int nObj = pLayer->GetObjectCount();
			for (int j = 0; j < nObj; j++)
			{
				pFtr = pLayer->GetObject(j, FILTERMODE_DELETED);
				if (pFtr)
				{
					GProgressStep();
					pFtr0 = pFtr->Clone();
					pFtr0->SetID(OUID());
					if (!pDataSource->AddObject(pFtr0, pLayer0->GetID()))
					{
						delete pFtr0;
						continue;
					}
					tab.DelAll();
					tab.BeginAddValueItem();
					pDS->GetXAttributesSource()->GetXAttributes(pFtr, tab);
					tab.EndAddValueItem();
					pDataSource->GetXAttributesSource()->SetXAttributes(pFtr0, tab);
				}
			}
		}
		GProgressEnd();
	}
	pUVSData->BatchUpdateEnd();

	delete pDataSource;
}



