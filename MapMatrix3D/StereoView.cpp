// StereoView.cpp: implementation of the CStereoView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "StereoView.h"
#include "display.h"
#include "RegDef.h "
#include "RegDef2.h "
#include "ExMessage.h "
#include "StereoFrame.h "
#include "VDBaseDef.h"
#include "DlgDataSource.h "
#include "IO.H "
#include "FLOAT.H "
#include "DlgExportRaterStereo.h "
#include "tiffio.h "
#include "DlgSetXYZ.h "
#include "SymbolLib.h "
#include "Functions_temp.h"
#include "DrawingDef.h "
#include "CommonCallStation.h"
#include "SmartViewFunctions.h"
#include "MainFrm.h"
#include "StereoParamFile.h"
#include "DlgMakeCheckPtSample.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define  SAVEPOS_SNAPDOT		0
#define  SAVEPOS_SNAPTIP		1
#define  DISPLAYVECT_TIMER_ID	10



extern BYTE clrTable_CAD[];

UINT ToolBtnIds[]={
IDC_BUTTON_FULLSCREEN,IDC_BUTTON_VIEWOVERLAY,
IDC_BUTTON_HEIGHTMODE,IDC_BUTTON_AUTOMODE,IDC_BUTTON_HEIGHTLOCK,
IDC_BUTTON_SHARECROSS
};

UINT ToolBtnBmps[]={
IDB_BUTTON_FULLSCREEN,IDB_BUTTON_VIEWOVERLAY,
IDB_BUTTON_HEIGHTMODE,IDB_BUTTON_AUTOMODE,IDB_BUTTON_HEIGHTLOCK,
IDB_BUTTON_SHARECROSS
};

UINT ToolButTips[] = 
{ IDS_TIPS_FULLSCREEN,IDS_TIPS_VIEWOVERLAY,
IDS_TIPS_HEIMODE,IDS_TIPS_AUTOMODE,IDS_TIPS_HEILOCK,
IDS_TIPS_SHARECROSS
};


extern CString GetRunPath();

extern BOOL CreateFolder2(LPCTSTR path);

#define ADD_RSDS_POINT(pt,fx,fy,ste,clr,wid) {		\
pt.x = (fx); pt.y = (fy);										\
pt.state = (ste);  pt.color = (clr);  pt.width = (wid);	}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CStereoView, CBaseView)

BEGIN_MESSAGE_MAP(CStereoView, CBaseView)
	//{{AFX_MSG_MAP(CStereoView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_MOVING()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_COMMAND(IDC_BUTTON_VIEWOVERLAY, OnViewOverlay)
	ON_COMMAND(IDC_BUTTON_HEIGHTMODE, OnHeightMode)
	ON_COMMAND(IDC_BUTTON_HEIGHTLOCK, OnHeightLock)
	ON_COMMAND(IDC_BUTTON_AUTOMODE, OnAutoMode)
	ON_COMMAND(ID_STEREO_MODEMANUAL, OnManualMode)
	ON_COMMAND(ID_STEREO_MODEPARA, OnParaMode)
	ON_COMMAND(IDC_BUTTON_SHARECROSS, OnShareMode)
	ON_COMMAND(ID_STEREO_3DMOUSE_IND, OnIndepMode)
	ON_COMMAND(ID_IMGPARALLAX_HLEFT, OnHParallaxLeft)
	ON_COMMAND(ID_IMGPARALLAX_HRIGHT, OnHParallaxRight)
	ON_COMMAND(ID_IMGPARALLAX_VUP, OnVParallaxUp)
	ON_COMMAND(ID_IMGPARALLAX_VDOWN, OnVParallaxDown)
	ON_COMMAND(ID_HEIGHTUP_BYSTEP, OnHeightUpByStep)
	ON_COMMAND(ID_HEIGHTDOWN_BYSTEP, OnHeightDownByStep)
	ON_COMMAND(ID_HEIGHTUP, OnHeightUp)
	ON_COMMAND(ID_HEIGHTDOWN, OnHeightDown)
	ON_COMMAND(ID_STEREO_SETXYZ,OnSetXYZ)
	ON_COMMAND(ID_STEREO_MOVETOCENTER,OnMoveCursorToCenter)
	ON_COMMAND(ID_STEREO_MOUSEDRIVE,OnMouseDrive)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_VIEWOVERLAY, OnUpdateViewOverlay)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_HEIGHTMODE, OnUpdateHeightMode)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_HEIGHTLOCK, OnUpdateHeightLock)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_AUTOMODE, OnUpdateAutoMode)
	ON_UPDATE_COMMAND_UI(ID_STEREO_MODEPARA, OnUpdateParaMode)
	ON_UPDATE_COMMAND_UI(ID_STEREO_MODEMANUAL, OnUpdateManualMode)
	ON_UPDATE_COMMAND_UI(ID_STEREO_3DMOUSE_SHA, OnUpdateShareMode)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_SHARECROSS, OnUpdateShareMode)
	ON_UPDATE_COMMAND_UI(ID_STEREO_3DMOUSE_IND, OnUpdateIndepMode)
	ON_UPDATE_COMMAND_UI(ID_STEREO_MOUSEDRIVE,OnUpdateMouseDrive)
    ON_MESSAGE(WM_DEVICE_LFOOT,OnDeviceLFootDown)
	ON_MESSAGE(WM_DEVICE_MOVE,OnDeviceMove)
	ON_MESSAGE(WM_DEVICE_RFOOT,OnDeviceRFootDown)
	ON_MESSAGE(WM_STEREOVIEW_LOADMDL, OnLoadModel)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_KEY_HSCROLL_LEFT , OnHscrollLeft)
	ON_COMMAND(ID_KEY_HSCROLL_RIGHT, OnHscrollRight)
	ON_COMMAND(ID_KEY_VSCROLL_DOWN, OnVscrollDown)
	ON_COMMAND(ID_KEY_VSCROLL_UP, OnVscrollUp)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_STEREO_EXPORT, OnExportRaster)
	ON_COMMAND(ID_STEREO_RESETDEVICE, OnReset3DMouseParam)	
	ON_UPDATE_COMMAND_UI(ID_STEREO_NOHATCH, OnUpdateNoHatch)
	ON_UPDATE_COMMAND_UI(ID_STEREO_REVERSE_DISPLAYORDER,OnUpdateDisplayOrder)
	ON_COMMAND(ID_STEREO_SYSMOUSE_SHA, OnShareSysMouse)
	ON_UPDATE_COMMAND_UI(ID_STEREO_SYSMOUSE_SHA, OnUpdateShareSysMouse)
	ON_COMMAND(ID_STEREO_REVERSE_CROSSCOLOR,OnReverseCrossColor)
	ON_WM_TIMER()
//	ON_COMMAND(ID_STEREO_3DMOUSE_DRIVECROSS,On3DMouseDriveCross)
//	ON_UPDATE_COMMAND_UI(ID_STEREO_3DMOUSE_DRIVECROSS, OnUpdate3DMouseDriveCross)
	ON_COMMAND(ID_WS_ASSTEREO, OnWsAsstereo)
	ON_UPDATE_COMMAND_UI(ID_STEREO_FULLSCREEN, OnUpdateStereoFullscreen)
	ON_COMMAND(ID_STEREO_NOHATCH, OnStereoNohatch)
	ON_COMMAND(ID_STEREO_REVERSE_DISPLAYORDER, OnStereoReverseDisplayorder)
	ON_UPDATE_COMMAND_UI(ID_STEREO_VIEWOVERLAY, OnUpdateStereoViewoverlay)
	ON_COMMAND(ID_STEREO_MODEAUTO, OnAutoMode)
	ON_COMMAND(ID_STEREO_3DMOUSE_SHA, OnShareMode)
	ON_UPDATE_COMMAND_UI(ID_STEREO_MODEAUTO, OnUpdateAutoMode)
	ON_COMMAND(ID_STEREO_MODEHEI, OnHeightMode)
	ON_UPDATE_COMMAND_UI(ID_STEREO_MODEHEI,OnUpdateHeightMode)
	ON_COMMAND(ID_STEREO_HEILOCK, OnHeightLock)	
	ON_UPDATE_COMMAND_UI(ID_STEREO_HEILOCK, OnUpdateHeightLock)
	ON_WM_MBUTTONDOWN()
	ON_COMMAND(ID_STEREO_MOUSE_NOHEIMODE, OnMouseNoHeiMode)
	ON_UPDATE_COMMAND_UI(ID_STEREO_MOUSE_NOHEIMODE,OnUpdateMouseNoHeiMode)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_STEREO_SHOWKEYPT, CBaseView::OnViewKeypoint)
	ON_UPDATE_COMMAND_UI(ID_STEREO_SHOWKEYPT, CBaseView::OnUpdateViewKeypoint)
	ON_COMMAND(ID_SELECT, CBaseView::OnSelect)
	ON_COMMAND(ID_STEREO_FULLSCREEN,OnStereoFullScreen)
	ON_COMMAND(ID_STEREO_CROSS,CSceneViewEx::OnSettingCursor)
	ON_COMMAND(ID_STEREO_STEREO,CSceneView::OnDisplay)
	ON_COMMAND(ID_STEREO_REFRESH,/*CSceneView::*/OnRefresh)
	ON_COMMAND(IDC_BUTTON_REFRESH,/*CSceneView::*/OnRefresh)
	ON_COMMAND(ID_VIEW_REFRESH,OnRefresh)
	ON_COMMAND(ID_STEREO_HANDMOVE,CSceneViewEx::OnHandMove)
	ON_COMMAND(ID_STEREO_ZOOMIN,/*CSceneView::*/OnZoomIn)
	ON_COMMAND(ID_STEREO_ZOOMOUT,/*CSceneView::*/OnZoomOut)
	ON_COMMAND(IDC_BUTTON_ZOOMIN,/*CSceneView::*/OnZoomIn)
	ON_COMMAND(IDC_BUTTON_ZOOMOUT,/*CSceneView::*/OnZoomOut)
	ON_COMMAND(ID_STEREO_VIEWALL,CSceneView::OnZoomFit)
	ON_COMMAND(ID_STEREO_VIEWRECT,CSceneViewEx::OnZoomRect)
	ON_COMMAND(ID_MAGNIFY_HALF, OnMagnifyHalf)
	ON_COMMAND(ID_MAGNIFY_ONE, OnMagnifyOne)
	ON_COMMAND(ID_MAGNIFY_ONEPOINTFIVE, OnMagnifyOneAndHalf)
	ON_COMMAND(ID_MAGNIFY_TWO, OnMagnifyTwo)
	ON_COMMAND(ID_MAGNIFY_TWOPOINTFIVE, OnMagnifyTwoAndHalf)
	ON_COMMAND(ID_MAGNIFY_THREE, OnMagnifyThree)
	ON_COMMAND(ID_STEREO_VIEWOVERLAY, OnViewOverlay)
	ON_MESSAGE(MSGID_CLTTOGND,OnClientToGround)
	ON_MESSAGE(MSGID_GNDTOCLT,OnGroundToClient)
	ON_MESSAGE(FCCM_MODIFYMOUSEPOINT,OnModifyMousepoint)
	ON_WM_MOUSELEAVE()
	ON_MESSAGE(FCCM_UPDATESTEREOPARAM,UpdateViewParams)
	ON_MESSAGE(FCCM_LOCK_ANGLE,LockAngle)
	ON_COMMAND(ID_STEREO_CTRLPTS_BOUND,OnCtrlPtsBound)
END_MESSAGE_MAP()


CStereoView::CStereoView()
{
	m_dwModeCoord = CROSS_MODE_SHARE;
	m_gCurPoint.x=0; m_gCurPoint.y=0; m_gCurPoint.z=0; 

	m_bLoadConvert = FALSE;
	m_bViewVector = TRUE;
	m_nImageType  = EPIPOLAR;
	m_nInputType  = STEREOVIEW_INPUT_SYSMOUSE;

	m_pConvert = NULL;

	m_bLoadModel = FALSE;

	m_hPopupWnd = NULL;
	m_bMoveDriveVector = TRUE;
	m_bOutAlert = FALSE;

	m_lfHeiStep = gdef_lfHeiStep;

	m_clrHilite = gdef_clrHiImg;
	m_clrDragLine = gdef_clrDragImg;

	memset(m_trA,0,sizeof(m_trA));
	memset(m_trB,0,sizeof(m_trB));
	m_trA[0] = m_trB[1] = 1.0; 
	m_bMouseDriveVector = FALSE;
	m_bMoveFromSys = TRUE;

	m_trZ = 1.0;

	m_bUseCenterPoint = FALSE;
	m_bDisableMouseIn3DDraw = TRUE;
	m_b3DDraw = FALSE;

	m_ptLastMouse = CPoint(-1,-1);

	m_bUseStereoEvlp = FALSE;

	strcpy(m_strRegSection,REGPATH_VIEW_IMG);

	matrix_toIdentity(m_lfStretchLMatrix,3);
	matrix_toIdentity(m_lfStretchRMatrix,3);
	matrix_toIdentity(m_lfStretchRLMatrix,3);
	matrix_toIdentity(m_lfStretchRRMatrix,3);

	m_bMouseDriveVector = AfxGetApp()->GetProfileInt(m_strRegSection,"MouseDriveVector",m_bMouseDriveVector);

	m_bSysCross = AfxGetApp()->GetProfileInt(m_strRegSection,"ShareSysCursorForStereo",FALSE);

	m_bFirstLoadModel = TRUE;

	m_bStereoMidButton = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_STEREOMIDBUTTON,FALSE);
//	m_bShowKeyPoint = AfxGetApp()->GetProfileInt(m_strRegSection,"DisplayKeyPoint",FALSE);

	m_bRealTimeEpipolar = FALSE;
	m_bVectLayCompress = TRUE;
	m_pBufVariantDrag = NULL;
	m_pBufConstDrag = NULL;

	m_bMouseNoHeiMode = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MOUSENOHEIMODE,FALSE);
	
	m_bDriveOnlyCursor = FALSE;

	m_bAutoAdjustCoordMode = TRUE;

	m_bLockXY = FALSE;

	m_bDeviceModifyZProcessing = FALSE;

	m_lfScanSize = 0;

	m_bLockAngle = FALSE;

	m_bModifyheightBy3DMouse = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MODIFYHEIGHTBY3DMOUSE,FALSE);

	m_bZoomWithCurrentMouse = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_ZOOMIMAGEWITHCURMOUSE,FALSE);

	if (m_pContext)
	{
		m_pContext->m_bStereo = TRUE;
	}

	m_fWheelSpeed = GetProfileDouble(REGPATH_CONFIG,REGITEM_WHEELSPEED,1.0);

	m_nDragLineWid = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DRAGLINE_WID,0);
	m_bMouseWheelZoom = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_MOUSEWHEELZOOM,FALSE);
}

CStereoView::~CStereoView()
{
	delete m_pConvert;
	delete m_pBufVariantDrag;
	delete m_pBufConstDrag;
}

void AddMenuToCallback(CMenu *pMenu, HWND hwnd)
{
	if( !pMenu )return;

	int num = pMenu->GetMenuItemCount();
	for( int i=0; i<num; i++)
	{
		int id = pMenu->GetMenuItemID(i);
		if( id>0 )AfxLinkCallback(id,hwnd);
		else if( id==-1 )
		{
			AddMenuToCallback(pMenu->GetSubMenu(i),hwnd);
		}
	}
}


int CStereoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( AfxGetApp()->GetProfileInt(m_strRegSection,REGITEM_TEXTSTEREO,gdef_bTextStereo) )
		m_pContext->m_nTextMode = CStereoDrawingContext::textureModeGL;
	else
		m_pContext->m_nTextMode = CStereoDrawingContext::textureModeNone;

	m_pContext->m_bCreateOverlay = FALSE;
	m_pContext->m_bCreateStereo = TRUE;

	m_pContext->m_bCorrectFlicker = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,"CorrectFlicker",FALSE);

	CCacheGLTextData::m_bUseGPU = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_SHARPLYZOOM,FALSE);
	CCacheGLTextData::m_bUseGPU = FALSE;

	if( m_pContext->m_nTextMode == CStereoDrawingContext::textureModeNone )
		m_pContext->m_bCreateOverlay = TRUE;

	int nMem = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MEMNUM,200);
	if( nMem>10000 )m_bTextureForVect = TRUE;

	BOOL bInterleavedStereo = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_INTERLEAVEDSTEREO,FALSE);
	if( bInterleavedStereo )
	{
		m_pContext->m_bCreateOverlay = FALSE;
		m_pContext->m_bCreateStereo = FALSE;
	}

	m_pContext->m_bInverseStereo = AfxGetApp()->GetProfileInt(m_strRegSection,"InverseStereo",FALSE);

	AfxGetMainWnd()->SendMessage(FCCM_UPDATEVIEWLIST,0,(LPARAM)this);
	if (CBaseView::OnCreate(lpCreateStruct) == -1)
		return -1;
	if( m_pContext->m_bCreateOverlay && !m_pContext->GetOverlayRCContext() )
	{
// 		m_bSysCross = TRUE;
// 		AfxMessageBox(IDS_HARDCURSOR_UNSUPPORTED);
	}
	
	m_pContext->SetDisplayMode(CStereoDrawingContext::modeShutterStereo);
	if( m_bSysCross )
		EnableSysCross(m_bSysCross);
	else
		EnableHardCross(TRUE);

	CBitmap bmp;
	for( int i=0; i<sizeof(ToolBtnIds)/sizeof(ToolBtnIds[0]); i++)
	{
		bmp.LoadBitmap(ToolBtnBmps[i]);
		HBITMAP hBmp = (HBITMAP)bmp;
		m_ctrlBarHorz.AddButtons(ToolBtnIds+i,&hBmp,1);
		bmp.DeleteObject();
	}

	for( i=0; i<sizeof(ToolBtnIds)/sizeof(ToolBtnIds[0]); i++)
	{
		CMFCButton *pBtn = (CMFCButton*)m_ctrlBarHorz.GetButton(/*ToolBtnBmps*/ToolBtnIds[i]);
		if( pBtn )
		{
			CString str;
			str.LoadString(ToolButTips[i]);
			pBtn->SetTooltip(str);
		}
	}

//	BOOL bMode = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,FALSE);

	AfxLinkCallback(ID_KEY_HSCROLL_LEFT,this->m_hWnd);
	AfxLinkCallback(ID_KEY_HSCROLL_RIGHT,this->m_hWnd);
	AfxLinkCallback(ID_KEY_VSCROLL_UP,this->m_hWnd);
	AfxLinkCallback(ID_KEY_VSCROLL_DOWN,this->m_hWnd);
	AfxLinkCallback(ID_HEIGHTDOWN,this->m_hWnd);
	AfxLinkCallback(ID_HEIGHTUP,this->m_hWnd);
	AfxLinkCallback(ID_HEIGHTDOWN_BYSTEP,this->m_hWnd);
	AfxLinkCallback(ID_HEIGHTUP_BYSTEP,this->m_hWnd);
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_STEREO);
	AddMenuToCallback( &menu, this->m_hWnd);
	
	return 0;
}


void CStereoView::OnInitialUpdate()
{
	//立体没有加载成功
	if( !m_strStereID.IsEmpty() && !m_bLoadModel )
	{
		CView::OnInitialUpdate();
		return;
	}

	CBaseView::OnInitialUpdate();

	CDlgDoc *pDoc = GetDocument();
//	pDoc->m_accuBox.Enable3DMode(true);

	RecalcScrollBar( true );
	CSize sz = GetDimension();

	CRect rect;
	GetClientRect(&rect);
	int cx = sz.cx/2-rect.Width()/2, cy = sz.cy/2-rect.Height()/2;

	m_gCurPoint.x = rect.Width()/2; m_gCurPoint.y = rect.Height()/2; m_gCurPoint.z = m_gCurPoint.x; 
	
	SetCrossPos(m_gCurPoint.x, m_gCurPoint.y, m_gCurPoint.z);
	m_cCurMousePoint = m_gCurPoint;
	m_cCur3DMousePoint = m_gCurPoint;

	SetCursorType(CURSOR_NORMAL);

	PT_4D t(m_gCurPoint);
	ClientToGround(&t,&m_gCurPoint);
	m_gCurMousePoint = m_gCurPoint;
	m_gCur3DMousePoint = m_gCurPoint;

	//缺省焦点被滚动条或者按钮给获得了，这里做此修订
	::SetFocus(m_hWnd);

	BOOL bInterleavedStereo = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_INTERLEAVEDSTEREO,FALSE);
	if( bInterleavedStereo )
	{
		ZoomChange(rect.CenterPoint(),0.5);
	}

	if( m_bLoadConvert )
	{	  
		PT_4D pt0,pt1;
		PT_3D pt2;
		CRect rect;
		GetClientRect(&rect);
		pt0.x = rect.CenterPoint().x; pt0.y = rect.CenterPoint().y; pt0.z = pt0.x; pt0.yr = pt0.y;
		ClientToImage(&pt0,&pt1);
		
		Coordinate c1,c2;
		
		if( (m_pConvert->coreobj.pp.iMetricType==8||m_nImageType==SCANNER||m_bRealTimeEpipolar) )
		{
			c1.lx = pt1.x; c1.ly = pt1.y; c1.iType = m_nImageType; 
			c2.iType = LOCKZ; c1.z = m_pConvert->GetAverageHeight();
			if( Convert(c1,c2) )
			{
				pt2.x = c2.x; pt2.y = c2.y; pt2.z = c2.z;
			}
			else
			{
				ClientToGround(&pt0,&pt2);
			}
		}
		else
		{
			c1.lx = pt1.x; c1.ly = pt1.y; c1.iType = m_nImageType; c2.iType = m_nImageType;
			if( Convert(c1,c2) )
			{
				pt0 = pt1;
				pt0.z = c2.lx; pt0.yr = c2.ly;
				ImageToGround(&pt0,&pt2);
			}
			else
			{
				ClientToGround(&pt0,&pt2);
			}
		}
		
		DriveToXyz(&pt2,m_nImageType==EPIPOLAR?CROSS_MODE_SHARE:CROSS_MODE_HEIGHT);
	}

	SetTimer(DISPLAYVECT_TIMER_ID,500,NULL);

	CButton *pBtn = m_ctrlBarHorz.GetButton(IDC_BUTTON_SHARECROSS);
	if( pBtn )
	{
		pBtn->SetCheck((m_dwModeCoord&CROSS_MODE_SHARE)==0?0:1);
	}

	
	//读取立体模型的运动参数、位置
	CStereoParamFile file;
	if( m_bLoadConvert && file.SetStereoID(m_strStereID) )
	{
		m_trA[0] = file.ReadFloatParam("device_a0");
		m_trA[1] = file.ReadFloatParam("device_a1");
		m_trA[2] = file.ReadFloatParam("device_a2");
		m_trB[0] = file.ReadFloatParam("device_b0");
		m_trB[1] = file.ReadFloatParam("device_b1");
		m_trB[2] = file.ReadFloatParam("device_b2");
		
		m_trZ = file.ReadFloatParam("device_c0");
		
		PT_3D pt3d;
		pt3d.x = file.ReadFloatParam("endPoint_x");
		pt3d.y = file.ReadFloatParam("endPoint_y");
		pt3d.z = file.ReadFloatParam("endPoint_z");
		
		DriveToXyz(&pt3d,CROSS_MODE_HEIGHT);
		
	}

	AdjustCellScale();	
	OnRefresh();

	StereoMove();

	GetWorker()->StartCommand(ID_SELECT);
	pDoc->UpdateAllViews(this,hc_RotateAsStereo);

	SetTimer(TIMERID_AUTOMOVEIMAGE,100,NULL);

	m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_VIEWOVERLAY,TRUE,TRUE);
}


void CStereoView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if( bActivate )
	{
		//导致立体抖动，所以注释掉这句话
		//m_pContext->SafeActiveGLRC();
		
		AfxGetMainWnd()->SendMessage(FCCM_UPDATEVIEWLIST,0,(LPARAM)this);
//		AfxBringCallbackToTop(this,NULL);
//		AfxCallMessage(FCCM_REFRESHCURRENTSTEREOMS,0,(LPARAM)&m_strStereID);
		AfxGetMainWnd()->SendMessage(FCCM_REFRESHCURRENTSTEREOMS,0,(LPARAM)&m_strStereID);
		StereoMove();
	}
	CBaseView::OnActivateView(bActivate,pActivateView,pDeactiveView);
}


// LONG CStereoView::OnCallCommand(WPARAM wParam, LPARAM lParam)
// {
// 	if( wParam==ID_STEREO_FULLSCREEN )
// 	{
// 		CWnd *pWnd = GetParentFrame();
// 		if( pWnd )pWnd->SendMessage(WM_COMMAND,wParam);
// 	}
// 	else
// 		OnCommand(wParam,0);
// 	return 0;
// }

void CStereoView::OnDestroy() 
{
	AfxGetApp()->WriteProfileInt(m_strRegSection,"InverseStereo",m_pContext->m_bInverseStereo);
	
	//保存立体模型的运动参数、位置；经过测试发现，在退出时，如果是窗口最大化，GetClientRect 并不准确
	if( m_bLoadConvert )
	{
		PT_4D cpt;
		CRect rcClt;
		GetClientRect(rcClt);
		cpt.x = cpt.z = rcClt.CenterPoint().x;
		cpt.y = cpt.yr = rcClt.CenterPoint().y;
		
		PT_3D gpt;
		ClientToGround(&cpt,&gpt);
		
		CStereoParamFile file;
		
		file.SetStereoID(m_strStereID);
		file.WriteParam("device_a0",m_trA[0]);
		file.WriteParam("device_a1",m_trA[1]);
		file.WriteParam("device_a2",m_trA[2]);
		file.WriteParam("device_b0",m_trB[0]);
		file.WriteParam("device_b1",m_trB[1]);
		file.WriteParam("device_b2",m_trB[2]);	
		file.WriteParam("device_c0",m_trZ);
		
		file.WriteParam("endPoint_x",gpt.x);
		file.WriteParam("endPoint_y",gpt.y);
		file.WriteParam("endPoint_z",gpt.z);
		
		file.Save();
	}

	//实时核线
	m_ImgLayLeft.m_pFuncOwner = NULL;
	m_ImgLayLeft.m_pFuncConvertCoord = NULL;
	
	m_ImgLayRight.m_pFuncOwner = NULL;
	m_ImgLayRight.m_pFuncConvertCoord = NULL;

	m_ImgLayLeft.Destroy();
	m_ImgLayRight.Destroy();
	m_snapTipLayL.Destroy();
	m_snapTipLayR.Destroy();

	m_ImgLayLeft.SetContext(NULL);
	m_ImgLayRight.SetContext(NULL);
	m_snapTipLayL.SetContext(NULL);
	m_snapTipLayR.SetContext(NULL);

	KillTimer(DISPLAYVECT_TIMER_ID);
//	AfxUnlinkCallback(this,NULL);
	AfxGetMainWnd()->SendMessage(FCCM_UPDATEVIEWLIST,1,(LPARAM)this);

	AfxUnlinkCallback(0,this->m_hWnd);
	CBaseView::OnDestroy();	
}



BOOL CStereoView::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}



LRESULT CStereoView::OnDeviceLFootDown(WPARAM nCode, LPARAM lParam)
{
	SetFocus();

	if( (m_dwModeCoord&CROSS_MODE_SHARE)!=0 && !m_bMouseNoHeiMode )
	{
		if( m_nInputType==STEREOVIEW_INPUT_SYSMOUSE )
		{
			m_gCur3DMousePoint = m_gCurMousePoint;
			m_cCur3DMousePoint = m_cCurMousePoint;
		}
	}

	m_nInputType = STEREOVIEW_INPUT_3DMOUSE;
	if( !lParam )
		CBaseView::OnLButtonDown(0,CPoint(m_cCur3DMousePoint.x,m_cCur3DMousePoint.y));
	else
		CBaseView::OnLButtonUp(0,CPoint(m_cCur3DMousePoint.x,m_cCur3DMousePoint.y));

	return 1;
}


BOOL CStereoView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if( m_bMouseWheelZoom )
	{
		float change = GetComboNextZoom(zDelta>0?1:0)/GetZoomRate();
		if( change<=0 )return FALSE;
		
		if( change>1 && change<1.5f )change = 1.5f;
		if( change<1 && change>0.7f )change = 0.7f;
		
		pt.x = (short)pt.x; pt.y = (short)pt.y;
		
		OnPreInnerCmd(WM_MOUSEWHEEL);
		ScreenToClient(&pt);
		ZoomCustom(pt,change,FALSE);
		OnPostInnerCmd();

		UpdateWindow_ForceDraw();

		return TRUE;
	}

	pt.x = (short)pt.x; pt.y = (short)pt.y;
	
	PT_3D pt3d (1.0,1.0,1.0);
	pt3d.z = m_fWheelSpeed;

	CheckCoordMode();

	ScreenToClient(&pt);

	float cur_crsx,cur_crsy,cur_crsz,new_crsx,new_crsy,new_crsz;
	int img_dx=0, img_dy=0, img_dz=0, img_dq=0;

	cur_crsx = (m_cCurMousePoint.x); cur_crsy = (m_cCurMousePoint.y); 
	cur_crsz = (m_cCurMousePoint.z);
	new_crsx=cur_crsx; new_crsy=cur_crsy; new_crsz=cur_crsz;

	PT_4D cltpt=m_cCurMousePoint, imgpt;
	ClientToImage(&cltpt,&imgpt);

	{
		//高程模式
		if( !m_bMouseNoHeiMode && (m_dwModeCoord&CROSS_MODE_HEIGHT)!=0 )
		{
			//还暂时不提供自动方式
			//如果上次是用三维测标在工作，现在应该先更新鼠标的坐标
			if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
			{
				//求出在保持高程不变的情况下右片影像的坐标
				Coordinate coord1,coord2; 
				coord1.lx=imgpt.x; coord1.ly=imgpt.y; 
				coord1.z=m_gCurMousePoint.z;
				coord1.iType=m_nImageType; coord2.iType=LOCKZ;
				
				if( Convert(coord1,coord2) )
				{
					m_gCurMousePoint.x = coord2.x; m_gCurMousePoint.y = coord2.y;
				}
				else
				{
					ClientToGround(&m_cCurMousePoint,&m_gCurMousePoint);
				}
			}
			
			//如果连续地使用鼠标，就可以直接使用上次的鼠标的坐标
			if( !(m_dwModeCoord&CROSS_MODE_HEIGHTLOCK) )
			{
				float dz = (zDelta/120)*pt3d.z;
				m_gCurMousePoint.z += dz*m_trZ/GetZoomRate();
			}
			
			m_gCurPoint = m_gCurMousePoint;
			GroundToClient(&m_gCurPoint,&m_cCurMousePoint);

			new_crsx=(m_cCurMousePoint.x); new_crsy=(m_cCurMousePoint.y); 
			new_crsz=(m_cCurMousePoint.z);

		}
		//视差模式
		else
		{
			float dz = (zDelta/120)*pt3d.z;
			m_cCurMousePoint.z = m_cCurMousePoint.z + dz;

			new_crsx=(m_cCurMousePoint.x); new_crsy=(m_cCurMousePoint.y); 
			new_crsz=(m_cCurMousePoint.z);

			if( m_dwModeCoord&CROSS_MODE_AUTO )
				m_dwModeCoord ^= CROSS_MODE_AUTO;

			ClientToGround(&m_cCurMousePoint,&m_gCurMousePoint);
			m_gCurPoint = m_gCurMousePoint;
		}
	}
	//红绿立体模式
	if( IsRGStereo() || m_bSysCross )
	{
		new_crsz = new_crsx;
		img_dx = PIXEL(new_crsx)-PIXEL(m_cCurMousePoint.x); 
		img_dy = PIXEL(new_crsy)-PIXEL(m_cCurMousePoint.y);
		img_dz = PIXEL(new_crsz)-PIXEL(m_cCurMousePoint.z)-img_dx;
		img_dq = PIXEL(new_crsy)-PIXEL(m_cCurMousePoint.yr)-img_dy;

		//滚动影像
		if( img_dx || img_dy || img_dz || img_dq )
		{
			Scroll(img_dx,img_dy,img_dz,img_dq);
//			GetWorker()->m_ptSelDragStart.x += img_dx;  m_ptSelDragStart.y += img_dy;
			
			m_cCurMousePoint.x += img_dx;
			m_cCurMousePoint.y += img_dy;
			m_cCurMousePoint.z += (img_dz+img_dx);
			m_cCurMousePoint.yr += (img_dq+img_dy);

			if( (m_dwModeCoord&CROSS_MODE_HEIGHT)==0 )
				ClientToGround(&m_cCurMousePoint,&m_gCurPoint);
			m_gCurMousePoint = m_gCurPoint;
		}
	}

	if( m_bSysCross && m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
	{
		EnableCrossVisible(FALSE);
//		m_bStereoSysCursor = FALSE;
	}
	
	SetCrossPos(new_crsx, new_crsy, new_crsz);
	UpdateCross();

	if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
	{
		m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;
	}

	if( m_bMoveDriveVector )
	{
		if( (nFlags&MK_CONTROL)!=0 || m_bMouseDriveVector )
		{
			CDlgDoc *pDoc = GetDocument();
			pDoc->UpdateAllViews(this,hc_SetCrossPos,(CObject*)&m_gCurPoint);
			pDoc->UpdateAllViews(this,hc_UpdateVariantDrag);
		}
	}

	FinishUpdateDrawing();

	//显示当前测标点的坐标
	AfxGetMainWnd()->SendMessage(FCCM_SHOW_XYZ,0,LPARAM(&m_gCurPoint));
	return TRUE;
}

LRESULT CStereoView::OnDeviceMove(WPARAM wParam, LPARAM lParam)
{
	if (m_nInnerCmd == IDC_BUTTON_ADJUST || m_nInnerCmd == IDC_BUTTON_CURSOR)
		return 0;
	VDSEND_DATA* pData =(VDSEND_DATA*)lParam;

	if( m_bModifyheightBy3DMouse && m_dwModeCoord&CROSS_MODE_HEIGHT && m_dwModeCoord&CROSS_MODE_HEIGHTLOCK && fabs(pData->z) > 1e-4)
	{
		static DWORD s_firstTime = GetTickCount();
		static DWORD s_lastTime = GetTickCount();
		static float s_fDeviceZ = 0;

		DWORD dCurTime = GetTickCount();		

		if (dCurTime - s_lastTime >= 50)
		{
			s_fDeviceZ = 0;
			s_firstTime = s_lastTime = dCurTime;

			return 1;
		}
		
		s_lastTime = dCurTime;
		s_fDeviceZ += pData->z;

		if (dCurTime - s_firstTime >= 300)
		{
			if (fabs(s_fDeviceZ) > 1e-4)
			{
				m_bDeviceModifyZProcessing = TRUE;
				ModifyHeightWithUpdate(m_lfHeiStep*s_fDeviceZ/fabs(s_fDeviceZ));
				m_bDeviceModifyZProcessing = FALSE;
			}			
			
			s_fDeviceZ = 0;
			s_firstTime = s_lastTime = dCurTime;
		}
		
		return 1;
	}

	if( pData!=NULL && m_bLockXY )
	{
		pData->x = pData->y = 0;
	}

//	BOOL bMode = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,FALSE);
	if (m_bDriveOnlyCursor)
	{
		return OnDeviceMove2(wParam, lParam);
	}
	
	int img_dx=0, img_dy=0, img_dz=0, img_dq=0;
	//float cur_crsx,cur_crsy,cur_crsz; 
	double new_crsx,new_crsy,new_crsz;

	CheckCoordMode();

	VDSEND_DATA data2;
	if( IsPolarized() )
	{
		data2 = *pData;
		pData = &data2;
		pData->y *= 0.5;
	}

	CPoint centerPt;
	CRect rcClient;
	GetClientRect(&rcClient);
	centerPt = rcClient.CenterPoint();
	//GetCrossPos(&cur_crsx,&cur_crsy,&cur_crsz);

	if( (m_dwModeCoord&CROSS_MODE_SHARE)!=0 && !m_bMouseNoHeiMode )
	{
		if( m_nInputType==STEREOVIEW_INPUT_SYSMOUSE )
		{
			m_gCur3DMousePoint = m_gCurMousePoint;
			m_cCur3DMousePoint = m_cCurMousePoint;
		}
		new_crsx = (m_cCur3DMousePoint.x); 
		new_crsy = (m_cCur3DMousePoint.y); 
		new_crsz = new_crsx;
		if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit )
		{ 
			//new_crsx -= GetSplitX()/2;  new_crsz -= GetSplitX()/2;
		}
	}
	else
	{
		//总是中心测标
		new_crsx=centerPt.x; new_crsy=centerPt.y; new_crsz=centerPt.x;
		if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeInterleavedStereo )
		{
			new_crsy /= 2;
		}
	}
	
	if( m_bAccuLeave )
	{
		//避免绘制遮挡测标的窗口的问题
		EnableCrossVisible(FALSE);
		ResetCursorDrawing();
		EnableCrossVisible(TRUE);
		m_bAccuLeave = FALSE;
// 		m_bAccuBoxChanged = TRUE;
// 		m_bAccuDrgChanged = TRUE;
	}

	if( IsSysCross() )
	{
		EnableCrossVisible(FALSE);
	}

	SetCrossPos(new_crsx,new_crsy,new_crsz);

	//得到当前测标的影像坐标
	PT_4D imgpt,cltpt;
	cltpt = m_cCur3DMousePoint;
	ClientToImage(&cltpt,&imgpt);

	//以前的影像视差
	float oldhpx = (imgpt.z-imgpt.x)*GetZoomRate();

	//大地坐标驱动模式
	if( m_dwModeCoord&CROSS_MODE_HEIGHT )
	{
		//如果上次是用系统鼠标在工作，现在应该先更新三维测标的坐标
		if( (m_nInputType!=STEREOVIEW_INPUT_3DMOUSE) && (m_dwModeCoord&CROSS_MODE_SHARE)!=0 )
		{
			//求出在保持高程不变的情况下右片影像的坐标
			Coordinate coord1,coord2; 
			coord1.lx=imgpt.x; coord1.ly=imgpt.y; 
			coord1.z=m_gCur3DMousePoint.z;
			coord1.iType=m_nImageType; coord2.iType=LOCKZ;
			
			if( Convert(coord1,coord2) )
			{
				m_gCur3DMousePoint.x = coord2.x; m_gCur3DMousePoint.y = coord2.y;
			}
			else
			{
				ClientToGround(&m_cCur3DMousePoint,&m_gCur3DMousePoint);
			}
		}
			
		if (m_bLockAngle)
		{
			double len = ( m_trA[0]*pData->x-m_trA[1]*pData->y )/(GetZoomRate());

			m_gCur3DMousePoint.x += len*sin(m_LockLine.ang);
			m_gCur3DMousePoint.y += len*cos(m_LockLine.ang);
		}
		else
		{
			//如果连续地使用三维测标，就可以直接使用上次的三维测标的坐标
			m_gCur3DMousePoint.x += ( m_trA[0]*pData->x-m_trA[1]*pData->y )/(GetZoomRate());
			m_gCur3DMousePoint.y += ( m_trB[0]*pData->x-m_trB[1]*pData->y )/(GetZoomRate());
		}

		if( !(m_dwModeCoord&CROSS_MODE_HEIGHTLOCK) )
			m_gCur3DMousePoint.z += pData->z*m_trZ/GetZoomRate();

		m_gCurPoint = m_gCur3DMousePoint;
		GroundToClient(&m_gCurPoint,&m_cCur3DMousePoint);

		img_dx = PIXEL(new_crsx)-PIXEL(m_cCur3DMousePoint.x); 
		img_dy = PIXEL(new_crsy)-PIXEL(m_cCur3DMousePoint.y);
		img_dz = PIXEL(new_crsz)-PIXEL(m_cCur3DMousePoint.z)-img_dx;
		img_dq = PIXEL(new_crsy)-PIXEL(m_cCur3DMousePoint.yr)-img_dy;

	}
	//视差模式
	else
	{
		if( m_dwModeCoord&CROSS_MODE_AUTO && pData->z==0 )
		{
			//通过匹配关系来获得视差

			//新的客户坐标
			PT_4D new_imgpt, new_cltpt = cltpt;
			new_cltpt.x += pData->x, new_cltpt.y += pData->y, new_cltpt.yr += pData->y;
			
			//新的影像坐标
			ClientToImage(&new_cltpt,&new_imgpt);
			new_imgpt.z = new_imgpt.x + GetActualImageParallax(new_imgpt.x,new_imgpt.y)/GetZoomRate();

			ImageToClient(&new_imgpt,&new_cltpt);

			img_dx = PIXEL(new_crsx)-PIXEL(new_cltpt.x);  img_dy = PIXEL(new_crsy)-PIXEL(new_cltpt.y);
			img_dz = PIXEL(new_crsz)-PIXEL(new_cltpt.z)-img_dx;
			img_dq = PIXEL(new_crsy)-PIXEL(new_cltpt.yr)-img_dy;

			m_cCur3DMousePoint = new_cltpt;
		}
		else
		{
			//新的客户坐标
			PT_4D new_imgpt, new_cltpt = cltpt;
			new_cltpt.x += pData->x, new_cltpt.y += pData->y;
			new_cltpt.z += (pData->x-pData->z), new_cltpt.yr += pData->y;
			
			img_dx = PIXEL(new_crsx)-PIXEL(new_cltpt.x);  img_dy = PIXEL(new_crsy)-PIXEL(new_cltpt.y);
			img_dz = PIXEL(new_crsz)-PIXEL(new_cltpt.z)-img_dx; 
			img_dq = PIXEL(new_crsy)-PIXEL(new_cltpt.yr)-img_dy;

			m_cCur3DMousePoint = new_cltpt;

			if( m_dwModeCoord&CROSS_MODE_AUTO )
				m_dwModeCoord ^= CROSS_MODE_AUTO;
		}
	}

	m_nInputType = STEREOVIEW_INPUT_3DMOUSE;


	//滚动影像
	if( img_dx || img_dy || img_dz || img_dq )
	{
    	Scroll(img_dx,img_dy,img_dz,img_dq);
// 		m_ptSelDragStart.x += img_dx;  m_ptSelDragStart.y += img_dy;

		m_cCur3DMousePoint.x += img_dx;
		m_cCur3DMousePoint.y += img_dy;
		m_cCur3DMousePoint.z += (img_dz+img_dx);
		m_cCur3DMousePoint.yr += (img_dq+img_dy);
	}
	else
	{
		UpdateCross();
	}

	//重新计算大地坐标

	//apD->p为零时直接采用坐标转换中的大地坐标更精确
	if( m_dwModeCoord&CROSS_MODE_HEIGHT )
	{
	}
	else
	{
		ClientToGround(&m_cCur3DMousePoint, &m_gCurPoint);
		m_gCur3DMousePoint = m_gCurPoint;
	}

	if( m_dwModeCoord&CROSS_MODE_SHARE )
	{
		m_gCurMousePoint = m_gCur3DMousePoint;
		m_cCurMousePoint = m_cCur3DMousePoint;
	}	

	CDlgDoc *pDoc = GetDocument();

	PT_3D ptsbound[4];
	memset(ptsbound,0,sizeof(ptsbound));
	pDoc->GetDlgDataSource()->GetBound(ptsbound,NULL,NULL);
	int bInRect = GraphAPI::GIsPtInRegion(m_gCurPoint,ptsbound,4);
	if( m_bOutAlert && bInRect<0) Beep(500,20);

	if (m_bLockAngle)
	{
		if ( m_gCur3DMousePoint.x < min(m_LockLine.start.x,m_LockLine.end.x) || 
			m_gCur3DMousePoint.x > max(m_LockLine.start.x,m_LockLine.end.x) || 
			m_gCur3DMousePoint.y < min(m_LockLine.start.y,m_LockLine.end.y) || 
			m_gCur3DMousePoint.y > max(m_LockLine.start.y,m_LockLine.end.y) )
		{
			//Beep(500,20);
		}
		
	}

	if( m_bMoveDriveVector )
		CBaseView::OnMouseMove(0,CPoint(PIXEL(cltpt.x),PIXEL(cltpt.y)));

	if( m_bMoveDriveVector && !StereoMaybeFlicker() )
	{
		pDoc->UpdateAllViews(this,hc_SetCrossPos,(CObject*)&m_gCurPoint);
		pDoc->UpdateAllViews(this,hc_UpdateVariantDrag);
// 		pDoc->UpdateAllViews(this,hc_AddConstDragLine);
// 		pDoc->UpdateAllViews(this,hc_UpdateConstDrag);
// 		FinishUpdateDrawing();
	}

/*	if( IsDrawStreamline() )
	{
		CPoint ptMouse;
		::GetCursorPos(&ptMouse);
		::ScreenToClient(m_hWnd,&ptMouse);

		CRect rect;
		GetClientRect(&rect);
		if( rect.PtInRect(ptMouse) && ::GetCursor()!=NULL )
		{
			::SetCursor(NULL);
		}
	}
*/
	return 1;
}


//移动测标而不是影像
LRESULT CStereoView::OnDeviceMove2(WPARAM wParam, LPARAM lParam)
{
	VDSEND_DATA* pData =(VDSEND_DATA*)lParam;
	int img_dx=0, img_dy=0, img_dz=0, img_dq=0;
	float cur_crsx,cur_crsy,cur_crsz, new_crsx,new_crsy,new_crsz;

	CheckCoordMode();

	CPoint centerPt;
	CRect rcClient;
	GetClientRect(&rcClient);
	centerPt = rcClient.CenterPoint();
	GetCrossPos(&cur_crsx,&cur_crsy,&cur_crsz);

	if( (m_dwModeCoord&CROSS_MODE_SHARE)!=0 && !m_bMouseNoHeiMode )
	{
		if( m_nInputType==STEREOVIEW_INPUT_SYSMOUSE )
		{
			m_gCur3DMousePoint = m_gCurMousePoint;
			m_cCur3DMousePoint = m_cCurMousePoint;
		}
	}
	
	if( m_bAccuLeave )
	{
		//避免绘制遮挡测标的窗口的问题
		EnableCrossVisible(FALSE);
		ResetCursorDrawing();
		EnableCrossVisible(TRUE);
		m_bAccuLeave = FALSE;
// 		m_bAccuBoxChanged = TRUE;
// 		m_bAccuDrgChanged = TRUE;
	}

	if( IsSysCross() )
	{
		EnableCrossVisible(FALSE);
	}

	//得到当前测标的影像坐标
	PT_4D imgpt,cltpt;
	cltpt = m_cCur3DMousePoint;
	ClientToImage(&cltpt,&imgpt);

	//以前的影像视差
	float oldhpx = (imgpt.z-imgpt.x)*GetZoomRate();

	//大地坐标驱动模式
	if( m_dwModeCoord&CROSS_MODE_HEIGHT )
	{
		//如果上次是用系统鼠标在工作，现在应该先更新三维测标的坐标
		if( m_nInputType!=STEREOVIEW_INPUT_3DMOUSE && (m_dwModeCoord&CROSS_MODE_SHARE)!=0 )
		{
			//求出在保持高程不变的情况下右片影像的坐标
			Coordinate coord1,coord2; 
			coord1.lx=imgpt.x; coord1.ly=imgpt.y; 
			coord1.z=m_gCur3DMousePoint.z;
			coord1.iType=m_nImageType; coord2.iType=LOCKZ;
			
			if( Convert(coord1,coord2) )
			{
				m_gCur3DMousePoint.x = coord2.x; m_gCur3DMousePoint.y = coord2.y;
			}
			else
			{
				ClientToGround(&m_cCur3DMousePoint,&m_gCur3DMousePoint);
			}
		}
			
		//如果连续地使用三维测标，就可以直接使用上次的三维测标的坐标
		m_gCur3DMousePoint.x += ( m_trA[0]*pData->x-m_trA[1]*pData->y )/(GetZoomRate());
		m_gCur3DMousePoint.y += ( m_trB[0]*pData->x-m_trB[1]*pData->y )/(GetZoomRate());

		if( !(m_dwModeCoord&CROSS_MODE_HEIGHTLOCK) )
			m_gCur3DMousePoint.z += pData->z*m_trZ/GetZoomRate();

		m_gCurPoint = m_gCur3DMousePoint;
		GroundToClient(&m_gCurPoint,&m_cCur3DMousePoint);

	}
	//视差模式
	else
	{
		if( m_dwModeCoord&CROSS_MODE_AUTO && pData->z==0 )
		{
			//通过匹配关系来获得视差

			//新的客户坐标
			PT_4D new_imgpt, new_cltpt = cltpt;
			new_cltpt.x += pData->x, new_cltpt.y += pData->y, new_cltpt.yr += pData->y;
			
			//新的影像坐标
			ClientToImage(&new_cltpt,&new_imgpt);
			new_imgpt.z = new_imgpt.x + GetActualImageParallax(new_imgpt.x,new_imgpt.y)/GetZoomRate();

			ImageToClient(&new_imgpt,&new_cltpt);

			m_cCur3DMousePoint = new_cltpt;
		}
		else
		{
			//新的客户坐标
			PT_4D new_imgpt, new_cltpt = cltpt;
			new_cltpt.x += pData->x, new_cltpt.y += pData->y;
			new_cltpt.z += (pData->x-pData->z), new_cltpt.yr += pData->y;
			
			m_cCur3DMousePoint = new_cltpt;

			if( m_dwModeCoord&CROSS_MODE_AUTO )
				m_dwModeCoord ^= CROSS_MODE_AUTO;
		}
	}

	m_nInputType = STEREOVIEW_INPUT_3DMOUSE;

	new_crsx = (m_cCur3DMousePoint.x); 
	new_crsy = (m_cCur3DMousePoint.y); 
	new_crsz = (m_cCur3DMousePoint.z); 

	//更新测标位置
	SetCrossPos(new_crsx,new_crsy,new_crsz);
	UpdateCross();

	//视差模式下重新计算大地坐标

	//apD->p为零时直接采用坐标转换中的大地坐标更精确
	if( m_dwModeCoord&CROSS_MODE_HEIGHT )
	{
	}
	else
	{
		ClientToGround(&m_cCur3DMousePoint, &m_gCurPoint);
		m_gCur3DMousePoint = m_gCurPoint;
	}

	if( m_dwModeCoord&CROSS_MODE_SHARE )
	{
		m_gCurMousePoint = m_gCur3DMousePoint;
		m_cCurMousePoint = m_cCur3DMousePoint;
	}	


	CDlgDoc *pDoc = GetDocument();

	PT_3D ptsbound[4];
	memset(ptsbound,0,sizeof(ptsbound));
	pDoc->GetDlgDataSource()->GetBound(ptsbound,NULL,NULL);
	int bInRect = GraphAPI::GIsPtInRegion(m_gCurPoint,ptsbound,4);
	if( m_bOutAlert && bInRect<0) Beep(500,20);

	if( m_bMoveDriveVector )
		CBaseView::OnMouseMove(0,CPoint(PIXEL(cltpt.x),PIXEL(cltpt.y)));

// 	if( IsDrawStreamline() )
// 	{
// 		CPoint ptMouse;
// 		::GetCursorPos(&ptMouse);
// 		::ScreenToClient(m_hWnd,&ptMouse);
// 
// 		CRect rect;
// 		GetClientRect(&rect);
// 		if( rect.PtInRect(ptMouse) && ::GetCursor()!=NULL )
// 		{
// 			::SetCursor(NULL);
// 		}
// 	}

	return 1;
}


LRESULT CStereoView::OnDeviceRFootDown(WPARAM wParam, LPARAM lParam)
{
	SetFocus();

	if( (m_dwModeCoord&CROSS_MODE_SHARE)!=0 && !m_bMouseNoHeiMode )
	{
		if( m_nInputType==STEREOVIEW_INPUT_SYSMOUSE )
		{
			m_gCur3DMousePoint = m_gCurMousePoint;
			m_cCur3DMousePoint = m_cCurMousePoint;
		}
	}

	m_nInputType = STEREOVIEW_INPUT_3DMOUSE;
	if( !lParam )
		CBaseView::OnRButtonDown(0,CPoint(m_cCur3DMousePoint.x,m_cCur3DMousePoint.y));
	else
		CBaseView::OnRButtonUp(0,CPoint(m_cCur3DMousePoint.x,m_cCur3DMousePoint.y));

	return 1;
}


CString	CStereoView::GetStereID()
{		
	return m_strStereID;
}
int CStereoView::GetImageType()
{
	return m_nImageType;		
}
BOOL CStereoView::GetRealTimeEpipolar()
{	
	return m_bRealTimeEpipolar;
}
BOOL CStereoView::CheckStereoIsOpen(CString StereID)
{
	m_bFirstCheckStere = TRUE;
	if (StereID.IsEmpty())
	{
		return FALSE;
	}
	CView *pView = NULL;
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
						CString strStereID = ((CStereoView*)pView)->GetStereID();
						if (StereID.Compare(strStereID) == 0)
						{
							return TRUE;
						}
					}		
				}
			}
		}		
	}
	return FALSE;
}

LRESULT CStereoView::OnLoadModel(WPARAM wParam, LPARAM lParam)
{
	CoreObject *pCore = (CoreObject*)lParam;
	m_coreObj = *pCore;
		
	int stereidx = wParam&0x0fffffff;
	if (stereidx >= pCore->iStereoNum) return 0;
	
	m_strStereID = pCore->stereo[stereidx].sp.stereoID;
 	
	//原始像对
	if( wParam&0x80000000 )
	{
		CString strId1 = pCore->stereo[stereidx].imageID[0];
		CString strId2 = pCore->stereo[stereidx].imageID[1];
		BOOL bFind1=FALSE, bFind2=FALSE;
		for( int i=0; i<pCore->iStripNum; i++)
		{
			for( int j=0; j<pCore->strip[i].iImageNum; j++)
			{
				if( !bFind1 && pCore->strip[i].image[j].strImageID.CompareNoCase(strId1)==0 )
				{
					m_strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName;
					//如果存在分块核线格式，就用该格式
					if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eil"),0)!=-1 )
						m_strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName+_T(".eil");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eir"),0)!=-1 )
						m_strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName+_T(".eir");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".ei"),0)!=-1 )
						m_strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName+_T(".ei");

					bFind1 = TRUE;
				}

				if( !bFind2 && pCore->strip[i].image[j].strImageID.CompareNoCase(strId2)==0 )
				{
					m_strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName;
					//如果存在分块核线格式，就用该格式
					if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eir"),0)!=-1 )
						m_strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName+_T(".eir");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eil"),0)!=-1 )
						m_strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName+_T(".eil");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".ei"),0)!=-1 )
						m_strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName+_T(".ei");

					bFind2 = TRUE;
				}
			}
		}

		m_nImageType  = SCANNER;
		
	}
	//实时核线
	else if( (wParam&0x40000000)!=0 && m_pContext->m_nTextMode == CStereoDrawingContext::textureModeGL )
	{
		CString strId1 = pCore->stereo[stereidx].imageID[0];
		CString strId2 = pCore->stereo[stereidx].imageID[1];
		BOOL bFind1=FALSE, bFind2=FALSE;
		for( int i=0; i<pCore->iStripNum; i++)
		{
			for( int j=0; j<pCore->strip[i].iImageNum; j++)
			{
				if( !bFind1 && pCore->strip[i].image[j].strImageID.CompareNoCase(strId1)==0 )
				{
					m_strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName;
					//如果存在分块核线格式，就用该格式
					if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eil"),0)!=-1 )
						m_strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName+_T(".eil");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eir"),0)!=-1 )
						m_strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName+_T(".eir");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".ei"),0)!=-1 )
						m_strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName+_T(".ei");
					
					bFind1 = TRUE;
				}
				
				if( !bFind2 && pCore->strip[i].image[j].strImageID.CompareNoCase(strId2)==0 )
				{
					m_strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName;
					//如果存在分块核线格式，就用该格式
					if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eir"),0)!=-1 )
						m_strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName+_T(".eir");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eil"),0)!=-1 )
						m_strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName+_T(".eil");
					else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".ei"),0)!=-1 )
						m_strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName+_T(".ei");
					
					bFind2 = TRUE;
				}
			}
		}
		
		m_nImageType  = EPIPOLAR;
		
		m_ImgLayLeft.m_pFuncOwner = this;
		m_ImgLayLeft.m_pFuncConvertCoord = (LPFUN_CONVERTCOORD)&CStereoView::RealtimeConvertCoordLeft;
		
		m_ImgLayRight.m_pFuncOwner = this;
		m_ImgLayRight.m_pFuncConvertCoord = (LPFUN_CONVERTCOORD)&CStereoView::RealtimeConvertCoordRight;
		
		m_bRealTimeEpipolar = TRUE;
		
	}
	//核线相对
	else
	{
		m_strMdlImgFile[0] = pCore->stereo[stereidx].se.strImage[0];
		m_strMdlImgFile[1] = pCore->stereo[stereidx].se.strImage[1];
		m_nImageType  = EPIPOLAR;
	}

	// 获取扫描分辨率
	CString strId1 = pCore->stereo[stereidx].imageID[0];
	BOOL bFind1=FALSE;
	for( int i=0; i<pCore->iStripNum; i++)
	{
		for( int j=0; j<pCore->strip[i].iImageNum; j++)
		{
			if( pCore->strip[i].image[j].strImageID.CompareNoCase(strId1)==0 )
			{				
				m_lfScanSize = pCore->strip[i].image[j].ipara.lfScanSize;				
				bFind1 = TRUE;
				break;
			}
		}

		if (bFind1)
		{
			break;
		}
	}

	GetParentFrame()->SetTitle(pCore->stereo[stereidx].sp.stereoID);
	GetParentFrame()->OnUpdateFrameTitle(TRUE);
	//获得当前窗口中点的地理坐标
	PT_4D pt0;
	PT_3D pt1;
	CRect rcClient;
	GetClientRect(&rcClient);
    pt0.x = rcClient.CenterPoint().x; pt0.y = rcClient.CenterPoint().y; pt0.z = pt0.x; pt0.yr = pt0.y;
	ClientToGround(&pt0,&pt1);
	pt1.z=m_gCurPoint.z;
		
	if( !LoadModel() )
	{
		//如果是新开一个立体窗口并且又无法加载该模型，就可以退出视图
		if( m_bFirstLoadModel )
			GetParentFrame()->PostMessage(WM_CLOSE);
		return 0;
	}

	m_bLoadConvert = FALSE;
	
	if( m_pConvert!=NULL )
		delete m_pConvert;
	m_pConvert = new CCoordCenter;
	if( m_pConvert!=NULL )
	{
		if( m_pConvert->Init(*pCore,pCore->stereo[stereidx].sp.stereoID) )
		{
			m_bLoadConvert = TRUE;
			
			if( m_bRealTimeEpipolar )
			{
				m_pConvert->EnableRealTimeEpipolar();
			}
		}
	}

	m_laymgrLeft.InsertDrawingLayer(&m_ImgLayLeft,0);
	m_laymgrRight.InsertDrawingLayer(&m_ImgLayRight,0);
	
	m_bUseStereoEvlp = FALSE;
	ReadStereoBound_forDisplayTrim();

	DefineAffineMatrix();

	m_pContext->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->m_fDrawCellKY = m_gsa.lfGKY;

	m_pContext->GetLeftContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;

	m_pContext->GetRightContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->GetRightContext()->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->GetRightContext()->m_fDrawCellKY = m_gsa.lfGKY;

	//如果是在切换立体模型（不是第一次打开立体模型），我们需要将立体模型驱动到当前坐标上
	if( !m_bFirstLoadModel )
	{
		
		//将矢量重新叠加显示
		InitBmpLayer();

		LoadDocData();

		RecalcScrollBar(TRUE);

		SetFocus();

		//定位到当前位置（保持当前位置不变化）
		PT_3D pt3d = m_gCurPoint;
		DriveToXyz(&pt3d,CROSS_MODE_HEIGHT);

		GrBuffer buf;
		if( m_pBufConstDrag!=NULL )buf.CopyFrom(m_pBufConstDrag);
		SetConstDragLine(&buf);

		buf.DeleteAll();
		if(m_pBufVariantDrag!=NULL)buf.CopyFrom(m_pBufVariantDrag);
		SetVariantDragLine(&buf);

	//	UpdateVariantDragLine();
 		UpdateConstDragLine();
 		FinishUpdateDrawing();
	}
	
	m_bFirstLoadModel = FALSE;
	Invalidate(TRUE);

	return 0;
}


void CStereoView::ReadStereoBound_forDisplayTrim()
{
	if( !m_bLoadConvert )
		return;
	CDlgDoc *pDoc = GetDocument();
	if(!pDoc) return;
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if(!pDS) return;

	BOOL bFasterLoad = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG, REGITEM_FASTERMAPPING, FALSE);
	if(bFasterLoad)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayer("StereoBound");
		CArray<PT_3DEX, PT_3DEX> arrShapePts;
		if(pLayer)
		{
			if(pLayer->GetObjectCount()>0)
			{
				CFeature *pFtr = pLayer->GetObject(0);
				if(pFtr && pFtr->GetGeometry())
				{
					pFtr->GetGeometry()->GetShape(arrShapePts);
				}
			}
		}

		m_arrStereoBoundPts_forDisplayTrim.RemoveAll();
		if(arrShapePts.GetSize()>=4)
		{
			for(int i = 0; i<arrShapePts.GetSize(); i++)
			{
				m_arrStereoBoundPts_forDisplayTrim.Add(arrShapePts[i]);
			}
		}
		else
		{
			PT_3D pts[4];
			GetStereoBound(pts);
			for(int i = 0; i<4; i++)
			{
				m_arrStereoBoundPts_forDisplayTrim.Add(pts[i]);
			}
		}
		m_bUseStereoEvlp = TRUE;
	}

	//矢量工作区范围
	BOOL bFasterLoad1 = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG, "FasterMapping1", FALSE);
	double m_lfExtendDis = GetProfileDouble(REGPATH_VIEW_IMG, "lfExtendDis", m_lfExtendDis);
	if(bFasterLoad1)
	{
		Envelope e = pDS->GetBound();
		e.Inflate(m_lfExtendDis, m_lfExtendDis, 0);

		if(m_bUseStereoEvlp)
		{
			Envelope e1;
			e1.CreateFromPts(m_arrStereoBoundPts_forDisplayTrim.GetData(), m_arrStereoBoundPts_forDisplayTrim.GetSize());
			e.Intersect(&e1);//取两个相交的部分
		}

		m_arrStereoBoundPts_forDisplayTrim.RemoveAll();
		m_arrStereoBoundPts_forDisplayTrim.Add(PT_3D(e.m_xl, e.m_yl,0));
		m_arrStereoBoundPts_forDisplayTrim.Add(PT_3D(e.m_xh, e.m_yl, 0));
		m_arrStereoBoundPts_forDisplayTrim.Add(PT_3D(e.m_xh, e.m_yh, 0));
		m_arrStereoBoundPts_forDisplayTrim.Add(PT_3D(e.m_xl, e.m_yh, 0));

		m_bUseStereoEvlp = TRUE;
	}

	/*
	double z0 = m_pConvert->GetAverageHeight(), z1=0;

	//CDlgDoc *pDoc = GetDocument();
	CDlgDataSource *pDS = GetDocument()->GetDlgDataSource();
	//获得高程分布信息（平均高程和高程的分布）
	long npt = 0;
	if( pDS )
	{
		int nlaynum = pDS->GetFtrLayerCount();
		for( int i=0; i<nlaynum; i++ )
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			if( pLayer )
			{
				int objnum = pLayer->GetObjectCount();
				for( int j=0; j<objnum; j++ )
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if( !pFtr )continue;
					CGeometry *pObj = pFtr->GetGeometry();
					if (!pObj)continue;	
					
					CArray<PT_3DEX,PT_3DEX> pts;
					pObj->GetShape(pts);
					int ncur = pts.GetSize();
					PT_3DEX expt;
					for( int k=0; k<ncur; k++)
					{
						expt = pts[k];
						z1 += expt.z;
					}

					npt += ncur;					
				}

//				pLayer->SetGetMode(oldmode);
			}
		}

		if( npt>100 )
		{
			z0 = z1/npt;

			z1 = 0;

			nlaynum = pDS->GetFtrLayerCount();
			for( i=0; i<nlaynum; i++ )
			{
				CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
				if( pLayer )
				{
		//			DWORD oldmode = pLayer->GetGetMode();
		//			pLayer->SetGetMode(GETMODE_INC_LOCKED|oldmode);
					
					int objnum = pLayer->GetObjectCount();
					for( int j=0; j<objnum; j++ )
					{
						CFeature *pFtr = pLayer->GetObject(j);
						if( !pFtr )continue;
						CGeometry *pObj = pFtr->GetGeometry();
						if( !pObj )continue;
						
						CArray<PT_3DEX,PT_3DEX> pts;
						pObj->GetShape(pts);
						int ncur = pts.GetSize();

						PT_3DEX expt;
						for( int k=0; k<ncur; k++)
						{
							expt = pts[k];
							z1 += (expt.z>z0?(expt.z-z0):(z0-expt.z));
						}				
					}
					
			//		pLayer->SetGetMode(oldmode);
				}
			}

			z1 = z1/npt;
		}
		else
		{
			z1 = 50;
		}
	}


	CSize size = GetDimension();
	
	PT_3D pts3[12];
	PT_4D pts4[4];
	Coordinate coord1,coord2; 
	
	pts4[0].x = pts4[3].x = size.cx*0.25;			
	pts4[1].x = pts4[2].x = size.cx*0.75;
	pts4[0].y = pts4[1].y = size.cy*0.25;
	pts4[2].y = pts4[3].y = size.cy*0.75;
	pts4[0].z = pts4[3].z = pts4[0].x;			
	pts4[1].z = pts4[2].z = pts4[1].x;
	
	z0 -= z1;
	for( int j=0; j<3; j++)
	{
		for ( int i=0;i<4; i++)
		{
			coord1.lx=pts4[i].x; coord1.ly=pts4[i].y;
			coord1.iType=m_nImageType; 
			coord2.iType=LOCKZ;	coord1.z = z0;
			
			if( Convert(coord1,coord2) )
			{					
				pts3[j*4+i].x = coord2.x; pts3[j*4+i].y = coord2.y;
			}
			else m_bUseStereoEvlp = FALSE;
		}
		z0 += z1;
	}
	
	m_StereoEvlp.CreateFromPts(pts3,12);

	double dx = m_StereoEvlp.m_xh-m_StereoEvlp.m_xl, dy = m_StereoEvlp.m_yh-m_StereoEvlp.m_yl;
	m_StereoEvlp.m_xl -= dx*0.25; m_StereoEvlp.m_xh += dx*0.25; 
	m_StereoEvlp.m_yl -= dy*0.25; m_StereoEvlp.m_yh += dy*0.25; 
	*/
}


void CStereoView::GetStereoBound(PT_3D* pPts)
{
	if( m_bLoadConvert )
	{
		BOOL bOK = FALSE;
		if( (m_pConvert->coreobj.pp.iMetricType==8||m_nImageType==SCANNER||m_bRealTimeEpipolar) )
		{
			QUADRANGLE quad;
			m_pConvert->GetModelBound(&quad);
			for( int j=0; j<4; j++)
			{
				pPts[j].x = quad.lfX[j];
				pPts[j].y = quad.lfY[j];
				pPts[j].z = 0;
			}

			bOK = TRUE;
		}
		
		if( !bOK )
		{
			PT_4D pts[4];
			PT_3D pts2[4];
			CSize	size = GetDimension();
			
			//赋予合适的影像坐标
			int sx=0, ex=0, sy=0, ey=0;
			
			sx = 10;
			ex = size.cx-10;
			sy = 10;
			ey = size.cy-10;
			
			pts[0].x = pts[3].x = sx;			
			pts[1].x = pts[2].x = ex;
			pts[0].y = pts[1].y = sy;
			pts[2].y = pts[3].y = ey;
			pts[0].z = pts[3].z = pts[0].x;			
			pts[1].z = pts[2].z = pts[3].x;
			
			//计算相应的大地坐标
			Coordinate coord1,coord2; 
			for ( int i=0;i<4; i++)
			{
				coord1.lx=pts[i].x; coord1.ly=pts[i].y;
				coord1.iType=m_nImageType; coord2.iType=m_nImageType;

				if( Convert(coord1,coord2) )
				{
					pts[i].z = coord2.lx;
					pts[i].yr= coord2.ly;
				}
				else
				{
					pts[i].z = coord1.lx;
					pts[i].yr= coord1.ly;
				}
				
				ImageToGround(pts+i,pts2+i);
				pPts[i].x = pts2[i].x;
				pPts[i].y = pts2[i].y;
				pPts[i].z = 0;
			}
		}
	}

 	return;
}

void CStereoView::DisplayTrim(GrBuffer *pBuf)
{
	int nPtsB = m_arrStereoBoundPts_forDisplayTrim.GetSize();
	if(nPtsB<=0) return;
 	PT_3D *ptsB = m_arrStereoBoundPts_forDisplayTrim.GetData();

	Envelope e0;
	e0 = CreateEnvelopeFromPts(ptsB,nPtsB);
	CArray<const Graph*,const Graph*> arrDel;

	PT_3D pt1,pt2;
	const Graph *p = pBuf->HeadGraph();
	GrVertexList* pList = NULL;
	while(p)
	{
		Envelope e1 = GetEnvelopeOfGraph(p);
		if( !e1.bIntersect(&e0) )
		{
			arrDel.Add(p);
		}
		else
		{
			if (IsGrLineString(p))
			{
				GrLineString *grLine = (GrLineString *)p;
				
				pList = &grLine->ptlist;
								
				for (int i=0;i<pList->nuse-1;i++)
				{
					pt1 = pList->pts[i];					
					pt2 = pList->pts[i+1];					
					
					if(!e0.bIntersect(&pt1,&pt2) || (GraphAPI::GIsPtInRegion(pt1,ptsB,nPtsB)==-1 && GraphAPI::GIsPtInRegion(pt2,ptsB,nPtsB)==-1) )
					{
						//变成 MOVETO
						pList->pts[i+1].code = (pList->pts[i+1].code&~GRBUFFER_PTCODE_LINETO);
					}		
				}
				
				if(pList->nuse>=2)
				{
					CArray<GrVertex,GrVertex> arrPts;
					arrPts.Add(pList->pts[0]);

					for (i=1;i<pList->nuse;i++)
					{						
						if( (pList->pts[i-1].code&GRBUFFER_PTCODE_LINETO)==0 && (pList->pts[i].code&GRBUFFER_PTCODE_LINETO)==0 )
						{
							arrPts[arrPts.GetSize()-1] = pList->pts[i];
						}
						else
						{
							arrPts.Add(pList->pts[i]);
						}
					}

					pList->nuse = arrPts.GetSize();
					memcpy(pList->pts,arrPts.GetData(),pList->nuse*sizeof(GrVertex));

					if(pList->nuse<=1)
					{
						arrDel.Add(p);
					}
				}
			}
		}		
		p = p->next;
	}
	
	for( int i=0; i<arrDel.GetSize(); i++)
	{
		pBuf->DeleteGraph(arrDel[i]);
	}
	
	if(pBuf)
	{
		pBuf->RefreshEnvelope();
	}

}

LRESULT CStereoView::UpdateViewParams(WPARAM wParam, LPARAM lParam)
{
	if( !lParam )return 1;
	char *str = (char*)lParam;
	if( wParam==0 )
	{
		int mode = 0;
		if( sscanf(str,"%d",&mode)==1 )
		{
			m_dwModeCoord = mode;
		}
		else
			m_dwModeCoord = CROSS_MODE_SHARE;
	}
	else
	{
		sprintf(str,"%d",m_dwModeCoord);
	}
	
	return 0;
}

LRESULT CStereoView::LockAngle(WPARAM wParam, LPARAM lParam)
{
	m_bLockAngle = (BOOL)wParam;
	if (m_bLockAngle)
	{
		m_LockLine = *((LockLine*)lParam);
	}	

	if (m_bLockAngle)
	{
		CString str;
		str.Format("%f",m_LockLine.ang);
		//MessageBox(str);
	}

	return 1;
}

void CStereoView::OnViewOverlay() 
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_VIEWOVERLAY;
	OnPreInnerCmd(IDC_BUTTON_VIEWOVERLAY);
	
	m_bViewVector = !m_bViewVector;
	if( m_bViewVector )
	{
		m_vectLayL.OnChangeCoordSys(FALSE);
		m_vectLayR.OnChangeCoordSys(FALSE);
		m_laymgrLeft.InsertDrawingLayer(&m_vectLayL,1);
		m_laymgrRight.InsertDrawingLayer(&m_vectLayR,1);

		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_VIEWOVERLAY,TRUE,TRUE);
	}
	else
	{
		m_laymgrLeft.RemoveDrawingLayer(&m_vectLayL);
		m_laymgrRight.RemoveDrawingLayer(&m_vectLayR);

		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_VIEWOVERLAY,TRUE,FALSE);
	}

	Invalidate(FALSE);

	OnPostInnerCmd();
//	m_nInnerCmd = save;
}


void CStereoView::OnHeightMode() 
{	
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_HEIGHTMODE;
	OnPreInnerCmd(IDC_BUTTON_HEIGHTMODE);

	m_dwModeCoord ^= CROSS_MODE_HEIGHT;
	if( (m_dwModeCoord&CROSS_MODE_HEIGHT) && (m_dwModeCoord&CROSS_MODE_AUTO) )
		m_dwModeCoord ^= CROSS_MODE_AUTO;

	UpdateButtonStates();

	OnPostInnerCmd();
//	m_nInnerCmd = save;
}


void CStereoView::OnParaMode() 
{
	OnHeightMode();
}

void CStereoView::OnShareMode() 
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_SHARECROSS;
	OnPreInnerCmd(IDC_BUTTON_SHARECROSS);
	
	m_dwModeCoord ^= CROSS_MODE_SHARE;	

	UpdateButtonStates();

	OnPostInnerCmd();
//	m_nInnerCmd = save;
}

void CStereoView::OnIndepMode() 
{
	OnShareMode();
}


void CStereoView::OnHeightLock()
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_HEIGHTLOCK;
	OnPreInnerCmd(IDC_BUTTON_HEIGHTLOCK);

	m_dwModeCoord ^= CROSS_MODE_HEIGHTLOCK;

	UpdateButtonStates();

	OnPostInnerCmd();
//	m_nInnerCmd = save;
}



void CStereoView::OnAutoMode() 
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_AUTOMODE;
	OnPreInnerCmd(IDC_BUTTON_AUTOMODE);
	
	if( (m_dwModeCoord&CROSS_MODE_HEIGHT) && (m_dwModeCoord&CROSS_MODE_AUTO)==0 );
	else m_dwModeCoord ^= CROSS_MODE_AUTO;		

	UpdateButtonStates();

	OnPostInnerCmd();
//	m_nInnerCmd = save;
}


void CStereoView::OnManualMode() 
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_AUTOMODE;
	OnPreInnerCmd(IDC_BUTTON_AUTOMODE);
	
	if( (m_dwModeCoord&CROSS_MODE_HEIGHT) && (m_dwModeCoord&CROSS_MODE_AUTO)==0 );
	else m_dwModeCoord ^= CROSS_MODE_AUTO;
	
	UpdateButtonStates();

	OnPostInnerCmd();
//	m_nInnerCmd = save;
}


BOOL CStereoView::LoadModel()
{
	if(_access(m_strMdlImgFile[0],0)==-1 && m_strMdlImgFile[0].Left(6).CompareNoCase("uis://")!=0 )
	{
		CString strMsg;
		strMsg.FormatMessage( IDS_FILE_OPEN_ERR,m_strMdlImgFile[0] );
		AfxMessageBox(strMsg);
		return FALSE;
	}

	if(_access(m_strMdlImgFile[1],0)==-1 && m_strMdlImgFile[1].Left(6).CompareNoCase("uis://")!=0 )
	{
		CString strMsg;
		strMsg.FormatMessage( IDS_FILE_OPEN_ERR,m_strMdlImgFile[1] );
		AfxMessageBox(strMsg);
		return FALSE;
	}

	if( 0 && m_strMdlImgFile[0].Left(6).CompareNoCase("uis://")==0 )
	{
		CString strPath = m_coreObj.pp.strPrjPath;

		strPath = strPath + "\\uisCache";
		CreateFolder2(strPath);

		m_ImgLayLeft.SetUISCacheDir(strPath);
		m_ImgLayRight.SetUISCacheDir(strPath);
	}
/*
	char path[256]={0};
	GetModuleFileName(NULL,path,sizeof(path));
	char *pos = strrchr(path,'\\');
	if( pos )*pos = '\0';
	strcat(path,"\\img.txt");

	FILE *fp = fopen(path,"r");
	if( fp )
	{
		char line[1024] = {0};
		fgets(line,sizeof(line)-1,fp);

		m_strMdlImgFile[0] = line;
		
		m_strMdlImgFile[0].Remove('\r');
		m_strMdlImgFile[0].Remove('\n');

		fgets(line,sizeof(line)-1,fp);
		m_strMdlImgFile[1] = line;

		m_strMdlImgFile[1].Remove('\r');
		m_strMdlImgFile[1].Remove('\n');

		fclose(fp);
	}	
*/
	BOOL bSurpportStretch = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_SUPPORTSTRETCH,FALSE);
	CDlgStretchImg& dlg = m_wndStretchDlg;

	//切换立体模型时，不必再弹出这个设置对话框了
	if( bSurpportStretch && m_bFirstLoadModel )
	{
		dlg.DoModal();
	}
	
	BOOL bInterleavedStereo = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_INTERLEAVEDSTEREO,FALSE);
	if( bInterleavedStereo && m_bFirstLoadModel )
	{
		dlg.m_fXScale *= 2;
		dlg.m_fXScale2 *= 2;
		m_pContext->SetDisplayMode(CStereoDrawingContext::modeInterleavedStereo);
	}
	
	int imgbufsize = 100;
	int nBlockSize = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_BLOCKSIZE,256);

	int nBlkWid = (m_bRealTimeEpipolar?nBlockSize:512);

	if( m_pContext->m_nTextMode == CStereoDrawingContext::textureModeGL )
	{
		int nMem = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MEMNUM,200);
		if( nMem<0 )nMem = 200;
		nMem = (nMem%10000);
		nMem /= 2;

		imgbufsize = (nMem*1024.0*1024.0)/(nBlkWid*nBlkWid*4.0);
	}

	m_ImgLayLeft.SetContext(m_pContext->GetLeftContext());
	m_ImgLayRight.SetContext(m_pContext->GetRightContext());
	
	if( m_ImgLayLeft .InitCache(GetBestCacheTypeForLayer(&m_ImgLayLeft), 
		CSize(nBlkWid,nBlkWid),imgbufsize)==FALSE ||
		!m_ImgLayLeft.AttachImage(m_strMdlImgFile[0]))
	{
		CString strMsg;
		strMsg.FormatMessage( IDS_FILE_OPEN_ERR,m_strMdlImgFile[0] );
		AfxMessageBox(strMsg);
		return FALSE;
	}	

	if( m_ImgLayRight.InitCache(GetBestCacheTypeForLayer(&m_ImgLayLeft), 
		CSize(nBlkWid,nBlkWid),imgbufsize)==FALSE ||
		!m_ImgLayRight.AttachImage(m_strMdlImgFile[1]))
	{
		CString strMsg;
		strMsg.FormatMessage( IDS_FILE_OPEN_ERR,m_strMdlImgFile[1] );
		AfxMessageBox(strMsg); 
		return FALSE;
	}

	if( m_bFirstLoadModel )
	{
		if( dlg.m_nAngle==90 )
		{
			m_lfStretchLMatrix[0] = 0, m_lfStretchLMatrix[1] = 1, m_lfStretchLMatrix[2] = 0;
			m_lfStretchLMatrix[3] = -1, m_lfStretchLMatrix[4] = 0, m_lfStretchLMatrix[5] = m_ImgLayLeft.GetImageSize().cx;
			m_lfStretchLMatrix[6] = 0, m_lfStretchLMatrix[7] = 0, m_lfStretchLMatrix[8] = 1;
			
			matrix_reverse(m_lfStretchLMatrix,3,m_lfStretchRLMatrix);
		}
		else if( dlg.m_nAngle==180 )
		{
			m_lfStretchLMatrix[0] = -1, m_lfStretchLMatrix[1] = 0, m_lfStretchLMatrix[2] = m_ImgLayLeft.GetImageSize().cx;
			m_lfStretchLMatrix[3] = 0, m_lfStretchLMatrix[4] = -1, m_lfStretchLMatrix[5] = m_ImgLayLeft.GetImageSize().cy;
			m_lfStretchLMatrix[6] = 0, m_lfStretchLMatrix[7] = 0, m_lfStretchLMatrix[8] = 1;
			
			matrix_reverse(m_lfStretchLMatrix,3,m_lfStretchRLMatrix);
		}
		else if( dlg.m_nAngle==270 )
		{
			m_lfStretchLMatrix[0] = 0, m_lfStretchLMatrix[1] = -1, m_lfStretchLMatrix[2] = m_ImgLayLeft.GetImageSize().cy;
			m_lfStretchLMatrix[3] = 1, m_lfStretchLMatrix[4] = 0, m_lfStretchLMatrix[5] = 0;
			m_lfStretchLMatrix[6] = 0, m_lfStretchLMatrix[7] = 0, m_lfStretchLMatrix[8] = 1;
			
			matrix_reverse(m_lfStretchLMatrix,3,m_lfStretchRLMatrix);
		}
		
		if( dlg.m_fXScale!=1.0 || dlg.m_fYScale!=1.0 )
		{
			//		m_ImgLayLeft.SetDataZoom(0,dlg.m_fXScale);
			//		m_ImgLayLeft.SetDataZoom(1,dlg.m_fYScale);
			
			double m1[9] = {dlg.m_fXScale,0,0, 0,1,0, 0,0,1};
			double m2[9] = {1,0,0, 0,dlg.m_fYScale,0, 0,0,1};
			double mt[9];
			matrix_multiply(m1,m2,3,mt);
			
			matrix_multiply(mt,m_lfStretchLMatrix,3,m1);
			memcpy(m_lfStretchLMatrix,m1,sizeof(m1));		
			matrix_reverse(m_lfStretchLMatrix,3,m_lfStretchRLMatrix);
		}
		
		if( dlg.m_nAngle2==90 )
		{
			m_lfStretchRMatrix[0] = 0, m_lfStretchRMatrix[1] = 1, m_lfStretchRMatrix[2] = 0;
			m_lfStretchRMatrix[3] = -1, m_lfStretchRMatrix[4] = 0, m_lfStretchRMatrix[5] = m_ImgLayRight.GetImageSize().cx;
			m_lfStretchRMatrix[6] = 0, m_lfStretchRMatrix[7] = 0, m_lfStretchRMatrix[8] = 1;
			
			matrix_reverse(m_lfStretchRMatrix,3,m_lfStretchRRMatrix);
		}
		else if( dlg.m_nAngle2==180 )
		{		
			m_lfStretchRMatrix[0] = -1, m_lfStretchRMatrix[1] = 0, m_lfStretchRMatrix[2] = m_ImgLayRight.GetImageSize().cx;
			m_lfStretchRMatrix[3] = 0, m_lfStretchRMatrix[4] = -1, m_lfStretchRMatrix[5] = m_ImgLayRight.GetImageSize().cy;
			m_lfStretchRMatrix[6] = 0, m_lfStretchRMatrix[7] = 0, m_lfStretchRMatrix[8] = 1;
			
			matrix_reverse(m_lfStretchRMatrix,3,m_lfStretchRRMatrix);
		}
		else if( dlg.m_nAngle2==270 )
		{		
			m_lfStretchRMatrix[0] = 0, m_lfStretchRMatrix[1] = -1, m_lfStretchRMatrix[2] = m_ImgLayRight.GetImageSize().cy;
			m_lfStretchRMatrix[3] = 1, m_lfStretchRMatrix[4] = 0, m_lfStretchRMatrix[5] = 0;
			m_lfStretchRMatrix[6] = 0, m_lfStretchRMatrix[7] = 0, m_lfStretchRMatrix[8] = 1;
			
			matrix_reverse(m_lfStretchRMatrix,3,m_lfStretchRRMatrix);
		}
		
		if( dlg.m_fXScale2!=1.0 || dlg.m_fYScale2!=1.0 )
		{
			//		m_ImgLayRight.SetDataZoom(0,dlg.m_fXScale2);		
			//		m_ImgLayRight.SetDataZoom(1,dlg.m_fYScale2);
			
			double m1[9] = {dlg.m_fXScale2,0,0, 0,1,0, 0,0,1};
			double m2[9] = {1,0,0, 0,dlg.m_fYScale2,0, 0,0,1};
			double mt[9];
			matrix_multiply(m1,m2,3,mt);
			
			matrix_multiply(mt,m_lfStretchRMatrix,3,m1);
			memcpy(m_lfStretchRMatrix,m1,sizeof(m1));
			matrix_reverse(m_lfStretchRMatrix,3,m_lfStretchRRMatrix);
		}
	}

	m_ImgLayLeft.SetCornerPts(dlg.m_nAngle,dlg.m_fXScale,dlg.m_fYScale);
	m_ImgLayRight.SetCornerPts(dlg.m_nAngle2,dlg.m_fXScale2,dlg.m_fYScale2);

	m_bLoadModel = TRUE;

	return TRUE;
}

void CStereoView::OnMouseLeave()
{
// 	if (IsSplit())
// 	{
// 		return 0;
// 	}
	CBaseView::OnMouseLeave();
// 	m_cursorL.Erase();
//     m_cursorR.Erase();
// 	EnableCrossVisible(FALSE);
// 	ResetCursorDrawing();
}


void CStereoView::OnStereoFullScreen()
{
	CWnd *pWnd = GetParentFrame();
	if( pWnd&&pWnd->IsKindOf(RUNTIME_CLASS(CStereoFrame)) )
		pWnd->SendMessage(WM_COMMAND,IDC_BUTTON_FULLSCREEN);
}

LRESULT CStereoView::OnClientToGround(WPARAM wParam, LPARAM lParam)
{
	if( wParam<=0 || lParam==0 )return 0;
	int num = wParam;
	PT_4D *pts = (PT_4D*)lParam;
	PT_3D t;
	while( num-- )
	{
		ClientToGround(pts+num,&t);
		COPY_3DPT(pts[num],t);
	}
	return 1;
}

LRESULT CStereoView::OnGroundToClient(WPARAM wParam, LPARAM lParam)
{
	if( wParam<=0 || lParam==0 )return 0;
	int num = wParam;
	PT_4D *pts = (PT_4D*)lParam;
	PT_3D t;
	while( num-- )
	{
		COPY_3DPT(t,pts[num]);
		GroundToClient(&t,&pts[num]);
	}
	return 1;
}


LRESULT CStereoView::OnModifyMousepoint(WPARAM wParam, LPARAM lParam)
{
	CDlgDoc *pDoc = GetDocument();
	if( pDoc->m_snap.bOpen() && !pDoc->m_snap.Is2D() )
	{
		m_gCurMousePoint.z = m_gCurPoint.z;
		DriveToXyz(&m_gCurMousePoint,m_dwModeCoord,m_nInputType!=STEREOVIEW_INPUT_SYSMOUSE);
	}
	return 0;
}

void CStereoView::OnMove(int x, int y) 
{
	CBaseView::OnMove(x, y);

}

void CStereoView::OnSize(UINT nType, int cx, int cy) 
{
	CBaseView::OnSize(nType, cx, cy);
}

void CStereoView::OnMoving(UINT fwSide, LPRECT pRect) 
{
	CBaseView::OnMoving(fwSide, pRect);
}

void CStereoView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;

	CCommand *pCmd = GetDocument()->GetCurrentCommand();
	//if( (m_nInnerCmd==0||m_nInnerCmd==IDC_BUTTON_SYSCURSOR) && (pCmd==NULL||!pCmd->IsRunning()) )
	if( nFlags&MK_CONTROL )
	{
		CMenu menu;
		menu.LoadMenu(IDR_POPUP_STEREO);
		
		CMenu* pSumMenu = menu.GetSubMenu(0);
		
		CPoint point;
		GetCursorPos(&point);
		if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
		{
			CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
			
			if(pPopupMenu->Create(GetParentFrame(), point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			{
				m_nInnerCmd = IDC_BUTTON_SYSCURSOR;
				m_hPopupWnd = pPopupMenu->GetSafeHwnd();
				return;
			}
		}
	}
	
	CBaseView::OnRButtonDown(nFlags, point);


// 	if (GetDocument()->IsDefaultSelect())
// 	{
// 		m_dwModeCoord &= ~CROSS_MODE_HEIGHTLOCK;
// 	}
}


void CStereoView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_nInnerCmd==0 && IsDrawStreamline() )return;

	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;

	CBaseView::OnLButtonDown(nFlags, point);
	if( m_nInnerCmd==0 )
	{
		GetDocument()->UpdateAllViews(this,hc_SetCrossPos,(CObject*)&m_gCurPoint);
	}


//	CBaseView::OnLButtonDown(nFlags, point);	
	
}

void CStereoView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_nInnerCmd!=0 )
	{
		CSceneViewEx::OnMouseMove(nFlags,point);
		//return;
	}

	static TRACKMOUSEEVENT Track;
	Track.cbSize        = sizeof(TRACKMOUSEEVENT);
	Track.dwFlags       = TME_LEAVE;
	Track.hwndTrack     = m_hWnd;
	Track.dwHoverTime   = HOVER_DEFAULT;
	_TrackMouseEvent (&Track);  
	CheckCoordMode();

	double cur_crsx,cur_crsy,cur_crsz,new_crsx,new_crsy,new_crsz;
	int img_dx=0, img_dy=0, img_dz=0, img_dq=0;

	cur_crsx = (m_cCurMousePoint.x); cur_crsy = (m_cCurMousePoint.y); 
	cur_crsz = (m_cCurMousePoint.z);
	new_crsx=cur_crsx; new_crsy=cur_crsy; new_crsz=cur_crsz;

	PT_4D cltpt=m_cCurMousePoint, imgpt;
	ClientToImage(&cltpt,&imgpt);

	//过滤鼠标的坏信号(原因很复杂，可能是硬件也可能是其他软件造成的)
	if( m_bMoveFromSys && m_ptLastMouse==point )
	{
		return;
	}

	if( IsDrawStreamline() && !IsSysCross() )
	{
		CSceneViewEx::OnMouseMove(nFlags,point);
		return;
	}

	if( IsPolarized() )
	{
		point.y /= 2;
	}

	if( nFlags==MK_SHIFT )
	{
		//高程模式
		if( !m_bMouseNoHeiMode && (m_dwModeCoord&CROSS_MODE_HEIGHT)!=0 )
		{
			//还暂时不提供自动方式
			//如果上次是用三维测标在工作，现在应该先更新鼠标的坐标
			if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
			{
				//求出在保持高程不变的情况下右片影像的坐标
				Coordinate coord1,coord2; 
				coord1.lx=imgpt.x; coord1.ly=imgpt.y; 
				coord1.z=m_gCurMousePoint.z;
				coord1.iType=m_nImageType; coord2.iType=LOCKZ;
				
				if( Convert(coord1,coord2) )
				{
					m_gCurMousePoint.x = coord2.x; m_gCurMousePoint.y = coord2.y;
				}
				else
				{
					ClientToGround(&m_cCurMousePoint,&m_gCurMousePoint);
				}
			}
			
			//如果连续地使用鼠标，就可以直接使用上次的鼠标的坐标
			if( !(m_dwModeCoord&CROSS_MODE_HEIGHTLOCK) )
			{
				float dz = 0;
				if( m_ptLastMouse.x>=0 && m_ptLastMouse.y>=0 )
					dz = m_ptLastMouse.x-point.x;
				m_gCurMousePoint.z += dz*m_trZ/GetZoomRate();
			}
			
			m_gCurPoint = m_gCurMousePoint;
			GroundToClient(&m_gCurPoint,&m_cCurMousePoint);

			new_crsx=(m_cCurMousePoint.x); new_crsy=(m_cCurMousePoint.y); 
			new_crsz=(m_cCurMousePoint.z);

		}
		//视差模式
		else
		{
			if( m_ptLastMouse.x>=0 && m_ptLastMouse.y>=0 )
				m_cCurMousePoint.z = m_cCurMousePoint.z + point.x-m_ptLastMouse.x;

			new_crsx=(m_cCurMousePoint.x); new_crsy=(m_cCurMousePoint.y); 
			new_crsz=(m_cCurMousePoint.z);

			if( m_dwModeCoord&CROSS_MODE_AUTO )
				m_dwModeCoord ^= CROSS_MODE_AUTO;

			ClientToGround(&m_cCurMousePoint,&m_gCurMousePoint);
			m_gCurPoint = m_gCurMousePoint;
		}
	}
	else
	{
		new_crsx=point.x+0.5; 
		new_crsy=point.y+0.5;

		if( IsSplit() )new_crsx -= m_pContext->GetSplitViewRect(CStereoDrawingContext::drawLeft).left;

		cltpt.x = new_crsx;  cltpt.y = new_crsy;
		ClientToImage(&cltpt,&imgpt);

		//高程模式
		if( !m_bMouseNoHeiMode && (m_dwModeCoord&CROSS_MODE_HEIGHT)!=0 )
		{
			//还暂时不提供自动方式
			//求出在保持高程不变的情况下新的右片影像的坐标
			Coordinate coord1,coord2; 
			coord1.lx=imgpt.x; coord1.ly=imgpt.y; 
			coord1.z=m_gCurMousePoint.z;
			coord1.iType=m_nImageType; coord2.iType=LOCKZ;
			
			if( Convert(coord1,coord2) )
			{
				//先获得这种情况下的大地坐标值
				m_gCurMousePoint.x = coord2.x;  m_gCurMousePoint.y = coord2.y;

				coord2.iType=GROUND;
				if( Convert(coord2,coord1) )
				{
					//再获得新的客户坐标值
					imgpt.z = coord1.rx; imgpt.yr = coord1.ry;
				}
				else
				{
					imgpt.z = imgpt.x; imgpt.yr = imgpt.y;
				}
				
				ImageToClient(&imgpt,&cltpt);
				new_crsz = (cltpt.z);
				
				m_cCurMousePoint = cltpt;
			}
			else
			{
				//直接从客户坐标转到大地坐标
				//先更新客户坐标
				new_crsz=new_crsx+cur_crsz-cur_crsx;
				m_cCurMousePoint.x = new_crsx;  m_cCurMousePoint.y = new_crsy;
				m_cCurMousePoint.z = new_crsz;  m_cCurMousePoint.yr = new_crsy;
				
				//再更新大地坐标
				ClientToGround(&m_cCurMousePoint,&m_gCurMousePoint);
			}

			m_gCurPoint = m_gCurMousePoint;
		}
		//视差模式
		else
		{
			//自动方式
			if( m_dwModeCoord&CROSS_MODE_AUTO )
			{
				float t = new_crsx+GetActualImageParallax(imgpt.x,imgpt.y)-GetStereoParallax();
				new_crsz = (t);
			}
			else
			{	
				new_crsz=new_crsx+cur_crsz-cur_crsx;
			}

			//先更新客户坐标
			m_cCurMousePoint.x = new_crsx;  m_cCurMousePoint.y = new_crsy;
			m_cCurMousePoint.z = new_crsz;  m_cCurMousePoint.yr = new_crsy;

			//再更新大地坐标
			ClientToGround(&m_cCurMousePoint,&m_gCurPoint);
			m_gCurMousePoint = m_gCurPoint;
		}
	}

	m_ptLastMouse = point;

	if( m_bAccuLeave )
	{
		//避免绘制遮挡测标的窗口的问题
// 		EnableCrossVisible(FALSE);
// 		ResetCursorDrawing();
// 		EnableCrossVisible(TRUE);
		m_bAccuLeave = FALSE;
// 		m_bAccuBoxChanged = TRUE;
// 		m_bAccuDrgChanged = TRUE;
	}

	//红绿立体模式
	if( IsRGStereo() || m_bSysCross )
	{
		new_crsx = point.x+0.5; 
		new_crsy = point.y+0.5; 
		if( IsSplit() )new_crsx -= m_pContext->GetSplitViewRect(CStereoDrawingContext::drawLeft).left;
		new_crsz = new_crsx;
		img_dx = PIXEL(new_crsx)-PIXEL(m_cCurMousePoint.x); 
		img_dy = PIXEL(new_crsy)-PIXEL(m_cCurMousePoint.y);
		img_dz = PIXEL(new_crsz)-PIXEL(m_cCurMousePoint.z)-img_dx;
		img_dq = PIXEL(new_crsy)-PIXEL(m_cCurMousePoint.yr)-img_dy;

		//滚动影像
		if( img_dx || img_dy || img_dz || img_dq )
		{
			Scroll(img_dx,img_dy,img_dz,img_dq);
//			m_ptSelDragStart.x += img_dx;  m_ptSelDragStart.y += img_dy;
			
			m_cCurMousePoint.x += img_dx;
			m_cCurMousePoint.y += img_dy;
			m_cCurMousePoint.z += (img_dz+img_dx);
			m_cCurMousePoint.yr += (img_dq+img_dy);

			if( (m_dwModeCoord&CROSS_MODE_HEIGHT)==0 )
				ClientToGround(&m_cCurMousePoint,&m_gCurPoint);
			m_gCurMousePoint = m_gCurPoint;
		}
	}

	if( m_bSysCross && m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
	{
		EnableCrossVisible(FALSE);
	}
	
	SetCrossPos(new_crsx, new_crsy, new_crsz);
	UpdateCross();

	//出界警报的判断
	PT_3D ptsbound[4];
	memset(ptsbound,0,sizeof(ptsbound));
	
	GetDocument()->GetDlgDataSource()->GetBound(ptsbound,NULL,NULL);
	int bInRect = GraphAPI::GIsPtInRegion(m_gCurPoint,ptsbound,4);
	if(m_bOutAlert && bInRect<0) Beep(500,20);

	if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
	{
		m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;
	}

	if( m_bMoveDriveVector )
	{
		if( m_nInnerCmd==0 )
			CBaseView::OnMouseMove(nFlags, point);

		if( (nFlags&MK_CONTROL)!=0 || m_bMouseDriveVector )
		{
			CDlgDoc *pDoc = GetDocument();
			pDoc->UpdateAllViews(this,hc_SetCrossPos,(CObject*)&m_gCurPoint);
			pDoc->UpdateAllViews(this,hc_UpdateVariantDrag);
		}
	}
}

BOOL CStereoView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( m_nInnerCmd==IDC_BUTTON_SYSCURSOR )
	{
		if( ::IsWindow(m_hPopupWnd) )
		{
			HCURSOR hCursor = ::LoadCursor(NULL,IDC_ARROW);
			if( hCursor )
			{
				::SetCursor(hCursor);
				EnableCrossVisible(FALSE);
			}
			return TRUE;
		}
		else m_nInnerCmd = 0;
	}
	else if( IsDrawStreamline() && !IsSysCross() )
	{
		CPoint pt;
		::GetCursorPos(&pt);
		::ScreenToClient(m_hWnd,&pt);

		if( pt==m_ptLastMouse )return TRUE;

		HCURSOR hCursor = ::LoadCursor(NULL,IDC_ARROW);
		if( hCursor )
		{
			::SetCursor(hCursor);
		}
		return TRUE;
	}

	return CBaseView::OnSetCursor(pWnd, nHitTest, message);
}



void CStereoView::ClientToImage(PT_4D *pt0, PT_4D *pt1)
{
	PT_4D tpt1, tpt2, tpt3;
	double z1, z2;

	tpt1.x = pt0->x*m_lfMatrixLic[0]+pt0->y*m_lfMatrixLic[1]+m_lfMatrixLic[2];
	tpt1.y = pt0->x*m_lfMatrixLic[3]+pt0->y*m_lfMatrixLic[4]+m_lfMatrixLic[5];
	z1 = pt0->x*m_lfMatrixLic[6]+pt0->y*m_lfMatrixLic[7]+m_lfMatrixLic[8];
	
	tpt1.z = pt0->z*m_lfMatrixLic[0]+pt0->yr*m_lfMatrixLic[1]+m_lfMatrixLic[2];
	tpt1.yr = pt0->z*m_lfMatrixLic[3]+pt0->yr*m_lfMatrixLic[4]+m_lfMatrixLic[5];
	z2 = pt0->z*m_lfMatrixLic[6]+pt0->yr*m_lfMatrixLic[7]+m_lfMatrixLic[8];
	
	pt0 = &tpt1;
	tpt2.x = pt0->x*m_lfRMatrixLic[0]+pt0->y*m_lfRMatrixLic[1]+z1*m_lfRMatrixLic[2];
	tpt2.y = pt0->x*m_lfRMatrixLic[3]+pt0->y*m_lfRMatrixLic[4]+z1*m_lfRMatrixLic[5];
	tpt3.x = pt0->z*m_lfRMatrixLic[0]+pt0->yr*m_lfRMatrixLic[1]+z2*m_lfRMatrixLic[2];
	tpt3.y = pt0->z*m_lfRMatrixLic[3]+pt0->yr*m_lfRMatrixLic[4]+z2*m_lfRMatrixLic[5];
	
	m_pLeftCS->ClientToGround(&tpt2,1);
	m_pRightCS->ClientToGround(&tpt3,1);
	
	pt1->x = tpt2.x;
	pt1->y = tpt2.y;
	pt1->z = tpt3.x;
	pt1->yr = tpt3.y;

	return;
}

void CStereoView::ImageToClient(PT_4D *pt0, PT_4D *pt1)
{
	PT_4D tpt1, tpt2, tpt3;
	double z1, z2;

	tpt1.x = pt0->x*m_lfMatrixLic[0]+pt0->y*m_lfMatrixLic[1]+m_lfMatrixLic[2];
	tpt1.y = pt0->x*m_lfMatrixLic[3]+pt0->y*m_lfMatrixLic[4]+m_lfMatrixLic[5];
	z1 = pt0->x*m_lfMatrixLic[6]+pt0->y*m_lfMatrixLic[7]+m_lfMatrixLic[8];
	
	tpt1.z = pt0->z*m_lfMatrixLic[0]+pt0->yr*m_lfMatrixLic[1]+m_lfMatrixLic[2];
	tpt1.yr = pt0->z*m_lfMatrixLic[3]+pt0->yr*m_lfMatrixLic[4]+m_lfMatrixLic[5];
	z2 = pt0->z*m_lfMatrixLic[6]+pt0->yr*m_lfMatrixLic[7]+m_lfMatrixLic[8];
	
	pt0 = &tpt1;
	tpt2.x = pt0->x*m_lfRMatrixLic[0]+pt0->y*m_lfRMatrixLic[1]+z1*m_lfRMatrixLic[2];
	tpt2.y = pt0->x*m_lfRMatrixLic[3]+pt0->y*m_lfRMatrixLic[4]+z1*m_lfRMatrixLic[5];
	tpt3.x = pt0->z*m_lfRMatrixLic[0]+pt0->yr*m_lfRMatrixLic[1]+z2*m_lfRMatrixLic[2];
	tpt3.y = pt0->z*m_lfRMatrixLic[3]+pt0->yr*m_lfRMatrixLic[4]+z2*m_lfRMatrixLic[5];
	
	m_pLeftCS->GroundToClient(&tpt2,1);
	m_pRightCS->GroundToClient(&tpt3,1);
	
	pt1->x = tpt2.x;
	pt1->y = tpt2.y;
	pt1->z = tpt3.x;
	pt1->yr = tpt3.y;
	
	return;
}


void CStereoView::RotateCoordinate(double &x, double &y, const double *m)
{
	double x2 = x*m[0] + y*m[1] + m[2];
	double y2 = x*m[3] + y*m[4] + m[5];
	x = x2; y = y2;
}

void CStereoView::ImageToGround(PT_4D *pt0, PT_3D *pt1)
{
	Coordinate coord1,coord2; 
	coord1.lx=pt0->x; coord1.ly=pt0->y; 
	coord1.rx=pt0->z; coord1.ry=pt0->yr;
	coord1.iType=m_nImageType; coord2.iType=GROUND;

	if( !Convert(coord1,coord2) )
	{
		coord2.x=0; coord2.y=0; coord2.z=0;
	}

    pt1->x=coord2.x; pt1->y=coord2.y; pt1->z=coord2.z;
}

void CStereoView::GroundToImage(PT_3D *pt0, PT_4D *pt1)
{
	Coordinate coord1,coord2; 
	coord1.x=pt0->x; coord1.y=pt0->y; coord1.z=pt0->z;  
	coord1.iType=GROUND; coord2.iType=m_nImageType;

	if( !Convert(coord1,coord2) )
	{
		coord2.lx=0; coord2.ly=0; 
		coord2.rx=0; coord2.ry=0;
	}

    pt1->x=coord2.lx;  pt1->y=coord2.ly; 
	pt1->z=coord2.rx;	pt1->yr=coord2.ry;
}

void CStereoView::GroundToVectorLay(PT_3D *pt0, PT_4D *pt1)
{
	GroundToImage(pt0,pt1);

	return;

	int imgHeightL = GetDimension().cy;
	int imgHeightR = GetDimension(FALSE).cy;
	
	Coordinate coord1,coord2; 
	coord1.x=pt0->x; coord1.y=pt0->y; coord1.z=pt0->z;  
	coord1.iType=GROUND; coord2.iType=m_nImageType;
	
	if( !Convert(coord1,coord2) )
	{
		coord2.lx=0; coord2.ly=0; 
		coord2.rx=0; coord2.ry=0;
	}

    pt1->x=coord2.lx;  pt1->y=imgHeightL-0.5-coord2.ly; 
	pt1->z=coord2.rx;  pt1->yr=imgHeightR-0.5-coord2.ry;
}

BOOL CStereoView::ConvertFailed(Coordinate Input, Coordinate &Output)
{
	return FALSE;
}

BOOL CStereoView::Convert(Coordinate Input, Coordinate &Output)
{
	if( !m_bLoadConvert )return ConvertFailed(Input,Output);
	/*
	 GROUND,
	 SCANNER,
	 PHOTO,
	 EPIPOLAR,
	 MODEL,
	 LOCKZ,
	 ORTHO
	 */

	int savetype = Output.iType;

	if( Input.iType==SCANNER || Input.iType==PHOTO || Input.iType==EPIPOLAR ||
		Input.iType==ORTHO || Input.iType==MODEL )
	{
		RotateCoordinate(Input.lx,Input.ly,m_lfStretchRLMatrix);
		RotateCoordinate(Input.rx,Input.ry,m_lfStretchRRMatrix);

		//测图的影像坐标是以左下角像素的左下角为起点的；而定向模块是以左下角像素的中心为起点的；所以需要偏移一下
		Input.lx -= 0.5; Input.ly -= 0.5; Input.rx -= 0.5; Input.ry -= 0.5;
	}

	if( !m_pConvert->Convert(Input,Output) )
		return FALSE;

	if( savetype==SCANNER || savetype==PHOTO || savetype==EPIPOLAR ||
		savetype==ORTHO || savetype==MODEL )
	{
		if( _isnan(Output.lx)||_isnan(Output.ly)||_isnan(Output.rx)||_isnan(Output.ry) 
			|| 0 == _finite(Output.lx) || 0 == _finite(Output.ly) || 0 == _finite(Output.rx) || 0 == _finite(Output.ry))
		{
			return FALSE;
		}

		//测图的影像坐标是以左下角像素的左下角为起点的；而定向模块是以左下角像素的中心为起点的；所以需要偏移一下
		Output.lx += 0.5; Output.ly += 0.5; Output.rx += 0.5; Output.ry += 0.5;

		RotateCoordinate(Output.lx,Output.ly,m_lfStretchLMatrix);
		RotateCoordinate(Output.rx,Output.ry,m_lfStretchRMatrix);
	}
	else
	{
		if( _isnan(Output.x)||_isnan(Output.y)||_isnan(Output.z) 
			|| 0 == _finite(Output.x) || 0 == _finite(Output.y) || 0 == _finite(Output.z))
		{
			return FALSE;
		}
	}
	return TRUE;
}

float CStereoView::GetActualImageParallax(float x, float y, int dir)
{
	if( m_bLoadConvert && !(m_dwModeCoord&CROSS_MODE_HEIGHT) &&
		(m_dwModeCoord&CROSS_MODE_AUTO) && dir==0)
	{
		Coordinate c1,c2;
		c1.lx = x; c1.ly = y; c1.iType = m_nImageType; c2.iType = m_nImageType;
		if( Convert(c1,c2) )
		{
			float hpx = c2.lx-x;
			return (hpx*GetZoomRate());
		}
	}

	return (float)GetStereoParallax(dir);
}



void CStereoView::OnHParallaxLeft()
{
	Scroll(0,0,-10);
	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;
}

void CStereoView::OnHParallaxRight()
{
	Scroll(0,0,10);
	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;
}

void CStereoView::OnVParallaxUp()
{
	Scroll(0,0,0,-1);
}

void CStereoView::OnVParallaxDown()
{
	Scroll(0,0,0,1);
}


void CStereoView::ModifyHeightWithUpdate(double dz)
{
	//更新鼠标和3D鼠标的大地坐标
	m_gCurPoint.z += dz;
	m_gCurMousePoint.z += dz;
	m_gCur3DMousePoint.z += dz;
	
	//求出新的测标的客户坐标
	PT_4D ipt, new_ipt, cpt, cpt2;
	GroundToClient(&m_gCurMousePoint,&cpt);
	
	//更新鼠标的客户坐标
	m_cCurMousePoint = cpt;
	
	//更新3D鼠标的客户坐标
	GroundToClient(&m_gCur3DMousePoint,&cpt2);
	m_cCur3DMousePoint = cpt2;
	
	//更新显示
	PT_3D pt3d = m_gCurPoint;
	DriveToXyz(&pt3d,CROSS_MODE_HEIGHT,m_nInputType!=STEREOVIEW_INPUT_SYSMOUSE);
}


void CStereoView::OnHeightUpByStep()
{
	ModifyHeightWithUpdate(m_lfHeiStep);
}

void CStereoView::OnHeightDownByStep()
{
	ModifyHeightWithUpdate(-m_lfHeiStep);
}

void CStereoView::OnHeightUp()
{
	PT_3D st3d (1.0,1.0,1.0);
	AfxGetMainWnd()->SendMessage(FCCM_GETSENSITIVITY,0,(LPARAM)&st3d);
	st3d.z = fabs(st3d.z);

	ModifyHeightWithUpdate(st3d.z);
}

void CStereoView::OnHeightDown()
{
	PT_3D st3d (1.0,1.0,1.0);
	AfxGetMainWnd()->SendMessage(FCCM_GETSENSITIVITY,0,(LPARAM)&st3d);
	st3d.z = fabs(st3d.z);

	ModifyHeightWithUpdate(-st3d.z);
}


void CStereoView::OnSetXYZ()
{
	PT_3D pt3d = m_gCurPoint;
	CDlgSetXYZ dlg;
	
	static BOOL bPlacePt = FALSE;
	
	dlg.m_lfX = pt3d.x;
	dlg.m_lfY = pt3d.y;
	dlg.m_lfZ = pt3d.z;
	dlg.m_bSetZ = TRUE;
	dlg.m_bPlacePt = bPlacePt;
	
	if( dlg.DoModal()==IDOK )
	{
		if( dlg.m_bSetX )pt3d.x = dlg.m_lfX;  
		if( dlg.m_bSetY )pt3d.y = dlg.m_lfY;  
		if( dlg.m_bSetZ )pt3d.z = dlg.m_lfZ;
		
		bPlacePt = dlg.m_bPlacePt;
		if( dlg.m_bPlacePt )
		{
			GetWorker()->SetCoordWnd(GetCoordWnd());
			GetWorker()->LButtonDown(pt3d,0);

			/*
			CDlgDoc *pDoc = GetDocument();
			CCommand *pCurCmd = pDoc->GetCurrentCommand();
			if( pCurCmd )
			{
				pDoc->SetCoordWnd(GetCoordWnd());
				pCurCmd->GetActiveSonProc()->PtClick(pt3d,0);
				UpdateConstDragLine();
				UpdateVariantDragLine();
				
				pDoc->TryFinishCurProcedure();
			}*/
		}

		//解决调整高程右片飞出去，切不回来的问题
		double m1[16] = { 0 }, m2[16] = { 0 };
		m_pLeftCS->GetMatrix(m1);
		m_pRightCS->GetMatrix(m2);
		if (fabs(m1[3]) > 1e+8 || fabs(m1[7]) > 1e+8 || fabs(m2[3]) > 1e+8 || fabs(m2[7]) > 1e+8)
		{
			m1[3] = 0;
			m1[7] = 0;
			m1[11] = 0;
			m2[3] = 0;
			m2[7] = 0;
			m2[11] = 0;
			m_pLeftCS->Create44Matrix(m1);
			m_pRightCS->Create44Matrix(m2);
			m_laymgrLeft.OnChangeCoordSys(TRUE);
			m_laymgrRight.OnChangeCoordSys(TRUE);

			m_cCur3DMousePoint = PT_4D();
			m_cCurMousePoint = PT_4D();
			m_gCurMousePoint = pt3d;
			m_gCurPoint = pt3d;
		}
		
		DriveToXyz(&pt3d,CROSS_MODE_HEIGHT);
	}
	
	Invalidate(FALSE);
}


void CStereoView::OnMoveCursorToCenter()
{
	PT_3D pt3d = m_gCurPoint;
	DriveToXyz(&pt3d,CROSS_MODE_HEIGHT);
	Invalidate(FALSE);
}


void CStereoView::OnMouseDrive()
{
	m_bMouseDriveVector = !m_bMouseDriveVector;

	AfxGetApp()->WriteProfileInt(m_strRegSection,"MouseDriveVector",m_bMouseDriveVector);
}


void CStereoView::OnUpdateViewOverlay(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bViewVector?1:0);
}


void CStereoView::OnUpdateHeightMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((m_dwModeCoord&CROSS_MODE_HEIGHT)?1:0);
}

void CStereoView::OnUpdateParaMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((m_dwModeCoord&CROSS_MODE_HEIGHT)?0:1);
}


void CStereoView::OnUpdateHeightLock(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_dwModeCoord&CROSS_MODE_HEIGHT);
	pCmdUI->SetCheck((m_dwModeCoord&CROSS_MODE_HEIGHTLOCK)?1:0);
}


void CStereoView::OnUpdateAutoMode(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!(m_dwModeCoord&CROSS_MODE_HEIGHT));
	pCmdUI->SetCheck((m_dwModeCoord&CROSS_MODE_AUTO)?1:0);
}


void CStereoView::OnUpdateManualMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((m_dwModeCoord&CROSS_MODE_AUTO)?0:1);
}


void CStereoView::OnUpdateShareMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((m_dwModeCoord&CROSS_MODE_SHARE)?1:0);
}

void CStereoView::OnUpdateIndepMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((m_dwModeCoord&CROSS_MODE_SHARE)?0:1);
}


void CStereoView::OnUpdateMouseDrive(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bMouseDriveVector?1:0);
}


void CStereoView::OnHscrollLeft()
{
	CBaseView::OnHscrollLeft();

	BOOL bSave = m_bMoveFromSys;
	m_bMoveFromSys = FALSE;

	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	OnMouseMove(0,pt);

	m_bMoveFromSys = bSave;
}

void CStereoView::OnHscrollRight()
{
	CBaseView::OnHscrollRight();

	BOOL bSave = m_bMoveFromSys;
	m_bMoveFromSys = FALSE;
	
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	OnMouseMove(0,pt);
	
	m_bMoveFromSys = bSave;
}

void CStereoView::OnVscrollDown()
{
	CBaseView::OnVscrollDown();

	BOOL bSave = m_bMoveFromSys;
	m_bMoveFromSys = FALSE;
	
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	OnMouseMove(0,pt);
	
	m_bMoveFromSys = bSave;
}

void CStereoView::OnVscrollUp()
{
	CBaseView::OnVscrollUp();

	BOOL bSave = m_bMoveFromSys;
	m_bMoveFromSys = FALSE;
	
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	OnMouseMove(0,pt);
	
	m_bMoveFromSys = bSave;
}


void CStereoView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CBaseView::OnHScroll(nSBCode, nPos, pScrollBar);
	
	BOOL bSave = m_bMoveFromSys;
	m_bMoveFromSys = FALSE;
	
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	OnMouseMove(0,pt);
	
	m_bMoveFromSys = bSave;
}

void CStereoView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CBaseView::OnVScroll(nSBCode, nPos, pScrollBar);
	
	BOOL bSave = m_bMoveFromSys;
	m_bMoveFromSys = FALSE;
	
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	OnMouseMove(0,pt);
	
	m_bMoveFromSys = bSave;
}


void CStereoView::DefineAffineMatrix()
{
    int		i;
	double	xSrc[4] ,ySrc[4] ;
	double	xDest[4],yDest[4];

	PT_4D pts[4];
	PT_3D pts2[4];
	CSize	size = GetDimension();
	
	//赋予合适的影像坐标
	int sx=0, ex=0, sy=0, ey=0;

	sx = size.cx * 0.25;
	ex = size.cx * 0.75;
	sy = size.cy * 0.25;
	ey = size.cy * 0.75;

	pts[0].x = pts[3].x = sx;			
	pts[1].x = pts[2].x = ex;
	pts[0].y = pts[1].y = sy;
	pts[2].y = pts[3].y = ey;
	pts[0].z = pts[3].z = pts[0].x;			
	pts[1].z = pts[2].z = pts[1].x;
	
	//计算相应的大地坐标
	Coordinate coord1,coord2; 

	if( m_pConvert!=NULL && (m_pConvert->coreobj.pp.iMetricType==8||m_nImageType==SCANNER||m_bRealTimeEpipolar) )
	{
		double z0 = m_pConvert->GetAverageHeight();
		for ( i=0;i<4; i++)
		{
			coord1.lx=pts[i].x; coord1.ly=pts[i].y;
			coord1.iType=m_nImageType; 
			coord2.iType=LOCKZ;	coord1.z = z0;
			
			if( Convert(coord1,coord2) )
			{
				xDest[i] = coord2.x; yDest[i] = coord2.y;
			}
			else
			{
				xDest[i] = pts[i].x; yDest[i] = pts[i].y;
			}
			
			xSrc[i] = pts[i].x; ySrc[i] = pts[i].y;
			pts2[i].x = xDest[i]; pts2[i].y = yDest[i];
		}
	}
	else
	{
		for ( i=0;i<4; i++)
		{
			coord1.lx=pts[i].x; coord1.ly=pts[i].y;
			coord1.iType=m_nImageType; coord2.iType=m_nImageType;
			
			if( Convert(coord1,coord2) )
			{
				pts[i].z = coord2.lx; pts[i].yr = coord2.ly;
			}
			else
			{
				pts[i].z = coord1.lx; pts[i].yr = coord2.ly;
			}
			
			ImageToGround(pts+i,pts2+i);

			xSrc[i] = pts[i].x; ySrc[i] = pts[i].y;
			xDest[i] = pts2[i].x; yDest[i] = pts2[i].y;
		}
	}

	//求出仿射关系
	CalcAffineParams( xSrc,ySrc,xDest,yDest,4,m_trA,m_trB );

	//归一化
	double k1, k2;
	k1 = DIST_2DPT(pts[0],pts[1])/DIST_2DPT(pts2[0],pts2[1]);
	k2 = DIST_2DPT(pts[2],pts[3])/DIST_2DPT(pts2[2],pts2[3]);

	m_gsa.lfGKX = sqrt(k1*k2);

	k1 = DIST_2DPT(pts[0],pts[3])/DIST_2DPT(pts2[0],pts2[3]);
	k2 = DIST_2DPT(pts[2],pts[1])/DIST_2DPT(pts2[2],pts2[1]);
	m_gsa.lfGKY = sqrt(k1*k2);

	m_gsa.lfAngle = GraphAPI::GGetAngle(pts2[0].x,pts2[0].y,pts2[1].x,pts2[1].y);

	//计算高程和水平视差的系数比
	m_trZ = 1.0;
	pts[0].x = pts[3].x = sx;			
	pts[1].x = pts[2].x = ex;
	pts[0].y = pts[1].y = sy;
	pts[2].y = pts[3].y = ey;
	pts[0].z = pts[3].z = pts[0].x;			
	pts[1].z = pts[2].z = pts[1].x;

	int nok = 0;
	if( m_pConvert!=NULL && (m_pConvert->coreobj.pp.iMetricType==8||m_nImageType==SCANNER||m_bRealTimeEpipolar) )
	{
		double z0 = m_pConvert->GetAverageHeight();
		for ( i=0;i<4; i++)
		{
			coord1.lx=pts[i].x; coord1.ly=pts[i].y;
			coord1.iType=m_nImageType; 
			coord2.iType=LOCKZ;	coord1.z = z0;
			
			if( Convert(coord1,coord2) )
			{
				pts2[i].x = coord2.x; pts2[i].y = coord2.y;
				pts2[i].z = z0;
				nok++;
			}
		}
	}
	else
	{
		for ( i=0;i<4; i++)
		{
			coord1.lx=pts[i].x; coord1.ly=pts[i].y;
			coord1.iType=m_nImageType; coord2.iType=m_nImageType;
			
			if( Convert(coord1,coord2) )
			{
				pts[i].z = coord2.lx; pts[i].yr = coord2.ly;
				ImageToGround(pts+i,pts2+i);
				nok++;
			}
		}
	}

	double dp = 0;
	if( nok==4 )
	{
		Coordinate coord3,coord4;
		for ( i=0;i<4; i++)
		{
			coord1.x=pts2[i].x; coord1.y=pts2[i].y;	coord1.z=pts2[i].z;
			coord1.iType=GROUND; 
			coord2.iType=m_nImageType;
			
			Convert(coord1,coord2);
			coord3 = coord1, coord4 = coord2;
			
			coord1.x=pts2[i].x; coord1.y=pts2[i].y;	coord1.z=pts2[i].z+10;
			coord1.iType=GROUND; 
			coord2.iType=m_nImageType;
			
			Convert(coord1,coord2);
			dp = dp
				+ sqrt((coord2.rx-coord4.rx)*(coord2.rx-coord4.rx)+(coord2.ry-coord4.ry)*(coord2.ry-coord4.ry))
				+ sqrt((coord2.lx-coord4.lx)*(coord2.lx-coord4.lx)+(coord2.ly-coord4.ly)*(coord2.ly-coord4.ly));
		}

		dp = 40/dp;
		m_trZ = dp;
	}

}


void CStereoView::DriveToXyz(PT_3D *pt, int coord_mode, BOOL bMoveImg )
{
	m_gCurPoint = *pt;
	m_gCurMousePoint = m_gCurPoint;
	m_gCur3DMousePoint = m_gCurPoint;
	
	GroundToClient(&m_gCurPoint,&m_cCurMousePoint);
	m_cCur3DMousePoint = m_cCurMousePoint;
	
	int save_crosstype = m_dwModeCoord;
	BOOL save_movefrom = m_bMoveFromSys;
	int save_inputtype = m_nInputType;
	BOOL save_sharesys = m_bSysCross;
	BOOL save_oldscroll = m_pContext->m_bOldStereoScroll;

	m_dwModeCoord = coord_mode;
	m_bMoveFromSys= FALSE;
	m_bSysCross = FALSE;
	m_pContext->m_bOldStereoScroll = FALSE;
	//更新显示
	if( bMoveImg )
	{
		m_nInputType = STEREOVIEW_INPUT_3DMOUSE;
		
// 		BOOL bMode = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,FALSE);
// 		if (bMode)
// 		{
// 			AfxGetApp()->WriteProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,!bMode);
// 		}
		BOOL bMode = m_bDriveOnlyCursor;
		if (bMode)
		{
			m_bDriveOnlyCursor = FALSE;
		}

		VDSEND_DATA data;
		data.x = data.y = data.z = data.flag = 0;
		OnDeviceMove(0,(LPARAM)&data);

		if (bMode)
		{
			m_bDriveOnlyCursor = bMode;
		}
// 		if (bMode)
// 		{
// 			AfxGetApp()->WriteProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,bMode);
// 		}
		
		//重新更新鼠标的客户坐标
		GroundToClient(&m_gCurPoint,&m_cCurMousePoint);

		//用 SetCursorPos 去改变系统光标的位置是十分不好的做法，会扰乱系统光标的稳定行为。
		//但是这里共享系统光标时，系统光标有时也要被迫去改变到测标位置，实在是无奈之举。
		if( save_sharesys && GetSafeHwnd()==::GetFocus() )
		{
			float x,y,xr;
			GetCrossPos(&x,&y,&xr);
			CPoint point, point2(PIXEL(x),PIXEL(y));
			::GetCursorPos(&point);
			
			ClientToScreen(&point2);
			if( point!=point2 )
			{
				::SetCursorPos(point2.x,point2.y);
			}
		}
	}
	else
	{
		
		m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;		
		
		PT_4D cpt;
		GroundToClient(&m_gCurPoint,&cpt);
		if( IsPolarized() )
		{
			cpt.y = (int)cpt.y;
			cpt.y *= 2;
		}

		OnMouseMove(0,CPoint(cpt.x,cpt.y));
	}
	
	m_dwModeCoord = save_crosstype;
	m_bMoveFromSys= save_movefrom;
	m_nInputType = save_inputtype;
	m_bSysCross = save_sharesys;
	m_pContext->m_bOldStereoScroll = save_oldscroll;

	GetDocument()->SetCoordWnd(GetCoordWnd());

	//显示当前测标点的坐标
	AfxGetMainWnd()->SendMessage(FCCM_SHOW_XYZ,0,LPARAM(&m_gCurPoint));
}

void CStereoView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
	CDlgDoc *pDoc = GetDocument();
	CDlgDataSource *pDS=pDoc->GetDlgDataSource();
	switch( lHint )
	{
	case hc_SetCrossPos:
		{
			PT_3D *pt = (PT_3D*)pHint;
			BOOL bSave = m_bMoveDriveVector;
			m_bMoveDriveVector = FALSE;
			DriveToXyz(pt, CROSS_MODE_HEIGHT, TRUE);
			m_bMoveDriveVector = bSave;
			UpdateConstDragLine();
		}
		break;
	case hc_SetAnchorPoint:
		{
			PT_3D pt = *((PT_3D*)pHint);
			pt.z = m_gCurPoint.z;

			BOOL bSave = m_bMoveDriveVector;
			m_bMoveDriveVector = FALSE;
			DriveToXyz(&pt, CROSS_MODE_HEIGHT, TRUE);
			m_bMoveDriveVector = bSave;
		}
		break;
	case hc_UpdateOption:
		{
			CBaseView::OnUpdate( pSender,  lHint,  pHint);

			int value;
			value = m_clrHilite;
			value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_HILITECOLOR,value);
			m_clrHilite = value;

			value = m_clrDragLine;
			value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DRAGCOLOR,value);
			m_clrDragLine = value;

			m_bOutAlert = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_OUTIMGALERT,gdef_bOutImgAlert);
			m_lfHeiStep = GetProfileDouble(REGPATH_CONFIG,REGITEM_HEISTEP,5.0);

			m_bSymbolize = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_SYMBOLIZE,m_bSymbolize);
			m_bDisableMouseIn3DDraw = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DIABLEMOUSE,m_bDisableMouseIn3DDraw);
		
			m_bDriveOnlyCursor = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,FALSE);

			m_bModifyheightBy3DMouse = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MODIFYHEIGHTBY3DMOUSE,FALSE);
		
			m_bStereoMidButton = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_STEREOMIDBUTTON,FALSE);

			m_fWheelSpeed = GetProfileDouble(REGPATH_CONFIG,REGITEM_WHEELSPEED,m_fWheelSpeed);

			m_bZoomWithCurrentMouse = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_ZOOMIMAGEWITHCURMOUSE,FALSE);
			m_nDragLineWid = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DRAGLINE_WID,m_nDragLineWid);
			m_bMouseWheelZoom = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_MOUSEWHEELZOOM,FALSE);
		}
		break;
	case hc_SetMapArea:
	case hc_UpdateAllObjects:
		{
			//重新装载矢量数据
			LoadDocData();			
			Invalidate(FALSE);
		}
		break;
	case hc_UpdateAllObjects_Visible:
		{
			UpdateVisible();
			UpdateGridDrawing();
			Invalidate(FALSE);
		}
		break;
	case hc_UpdateOverlayBound:
		{
			DelObjfromVectLay(HANDLE_BOUND);
			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();

			BOOL bView = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYBOUND,"ViewImg",FALSE);
			if( bView )
			{
				PT_3D pts[4];
				CDlgDoc *pDoc = GetDocument();
				pDoc->GetDlgDataSource()->GetBound(pts,NULL,NULL);

				COLORREF color = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYBOUND,"Color",gdef_clrBndVect);
				
				GrBuffer buf;				

				pts[0].z = GetProfileDouble(REGPATH_OVERLAYBOUND,"Z1",pts[0].z);      
				pts[1].z = GetProfileDouble(REGPATH_OVERLAYBOUND,"Z2",pts[1].z);      
				pts[2].z = GetProfileDouble(REGPATH_OVERLAYBOUND,"Z3",pts[2].z);      
				pts[3].z = GetProfileDouble(REGPATH_OVERLAYBOUND,"Z4",pts[3].z);  
				buf.BeginLineString(color,2);
				buf.MoveTo(&pts[0]);
				for( int i=1; i<sizeof(pts)/sizeof(pts[0]); i++ )
				{
					buf.LineTo(&pts[i]);
				}
				
				buf.LineTo(&pts[0]);
				buf.End();
			
				AddObjtoVectLay(HANDLE_BOUND,&buf);
				m_vectLayL.ClearAll();
				m_vectLayR.ClearAll();
			}
			Invalidate(FALSE);
		}
		break;
	case hc_ModifyHeight:
		if( pHint )
		{
			double *z = (double*)pHint;
			ModifyHeightWithUpdate(*z-m_gCurPoint.z);
		}
		break;
	case hc_ManualLoadVect:
		{
			Envelope *e = (Envelope*)pHint;
			ManualLoadVect(*e);
		}
		break;
// 	case hc_AddGraph:
// 		break;
// 	case hc_DelGraph:
// 		break;
	case hc_LockXY:
		m_bLockXY = (pHint==0?FALSE:TRUE);
		break;
	case hc_GetImageAdjust:
		{
			CImageAdjust *pIA = (CImageAdjust*)pHint;
			if( pIA )
			{
				*pIA = m_ImgLayLeft.m_Adjust;
			}
		}
		break;
	default:
		CBaseView::OnUpdate(pSender,lHint,pHint);
// 	case hc_AddObject:
// 		{
// 			CFeature *pFtr = (CFeature*)pHint;
// 			if( !pFtr )break;
// 			//			DrawObject(pObj);
// 			if( m_bSymbolize )
// 			{							
// 				GrBuffer buf,buf0;
// 				if (pDS)
// 				{
// 					pDS->DrawFeature(pFtr,&buf);
// 				}
// 				if (pFtr->GetGeometry()->GetColor()==COLOR_BYLAYER)
// 				{
// 					CFtrLayer *pLayer = GetDocument()->GetDlgDataSource()->GetFtrLayerOfObject(pFtr);
// 					if(pLayer) buf.SetAllColor(pLayer->GetColor());
// 				}
// 				
// // 				CPtrArray arr;
// // 				CConfigLibManager *pCLM = ((CEditBaseApp*)AfxGetApp())->GetConfigLibManager();
// // 				if(!pCLM->GetSymbol(pDS,pFtr,arr)) return;
// // 				if (arr.GetSize()<1) return;						
// // 				for (int k=0;k<arr.GetSize();k++)
// // 				{
// // 					((CSymbol*)arr[k])->Draw(pFtr,&buf0);
// // 					buf.AddBuffer(&buf0);
// // 				}
// 				AddObjtoVectLay((LONG_PTR)pFtr,&buf);
// 			}
// 			else
// 			{
// 				GrBuffer buf;
// 				pFtr->Draw(&buf);
// 				if (pFtr->GetGeometry()->GetColor()==COLOR_BYLAYER)
// 				{
// 					CFtrLayer *pLayer = GetDocument()->GetDlgDataSource()->GetFtrLayerOfObject(pFtr);
// 					if(pLayer) buf.SetAllColor(pLayer->GetColor());
// 				}
// 				AddObjtoVectLay((LONG_PTR)pFtr,&buf);
// 			}
// 			
// 			//AddObjtoVectLay((LONG_PTR)pObj,pObj->GetGrBuffer());
// 			AddObjtoMarkVectLay(pFtr);		
// 			//针对大量更新的优化
// 			//	SetViewUpdateFlags(VIEWUPDATE_ADDORDELOBJ);
// 			
// 		}
// 		return;
// 	case hc_DelObject:
// 		{
// 			CFeature *pFtr = (CFeature*)pHint;
// 			if( !pFtr )break;
// 			//EraseObject(pObj);
// 			DelObjfromVectLay((LONG_PTR)pFtr);
// 			DelObjfromVectLay((LONG_PTR)pFtr,&m_markLayL,&m_markLayR);
// 			
// 			//针对大量更新的优化
// 			//	SetViewUpdateFlags(VIEWUPDATE_ADDORDELOBJ);
// 		}
// 		return;
 	}
	
}


void CStereoView::OnPaint() 
{
	CBaseView::OnPaint();
}

void CStereoView::InitUpdateLayersDepth()
{
	//纹理模式下，仍然使用 动态线、动态光标、动态的捕捉框绘制、动态的精确绘图线
	{
	}
}


BOOL CStereoView::LoadDocData()
{
	BOOL bManualLoad = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MANUALLOADVECT,FALSE);
	if( !bManualLoad )
	{
		if( !CBaseView::LoadDocData() )
			return FALSE;		
	}
	Envelope eImgL = m_ImgLayLeft.GetDataBound();
	Envelope eImgR = m_ImgLayRight.GetDataBound();
	
	m_vectLayL.SetDataBound(eImgL);
	m_vectLayR.SetDataBound(eImgR);
	m_markLayL.SetDataBound(eImgL);
	m_markLayR.SetDataBound(eImgR);	
	UpdateGridDrawing();
	OnUpdate(this,hc_UpdateOverlayBound,NULL);

	return TRUE;
}


void CStereoView::ScrollForDrawBitmap(double dx,double dy,double dz )
{
	double m[16];	
	Matrix44FromMove(-dx,-dy,-dz,m);
	m_pLeftCS->MultiplyMatrix(m);
	m_pRightCS->MultiplyMatrix(m);
	m_laymgrLeft.OnChangeCoordSys(TRUE);
	m_laymgrRight.OnChangeCoordSys(TRUE);
	
	CDrawingContext *pDC = m_pContext->GetLeftContext();
	if( pDC )pDC->SetCoordSys(m_pLeftCS);
	
	pDC = m_pContext->GetRightContext();
	if( pDC )pDC->SetCoordSys(m_pRightCS);	
//	RecalcScrollBar(FALSE);
//	Invalidate(TRUE);

}

BOOL CStereoView::InitBmpLayer()
{
	if( m_pContext->m_nTextMode != CStereoDrawingContext::textureModeNone )
	{
		m_snapLayL.m_nEraseType = CVariantDragLayer::eraseNone;
		m_snapLayR.m_nEraseType = CVariantDragLayer::eraseNone;
		
		m_snapTipLayL.m_nEraseType = CVariantDragLayer::eraseNone;
		m_snapTipLayR.m_nEraseType = CVariantDragLayer::eraseNone;
		
		m_accurboxLayL.m_nEraseType = CVariantDragLayer::eraseNone;
		m_accurboxLayR.m_nEraseType = CVariantDragLayer::eraseNone;
	}
	else
	{
		m_snapLayL.m_nEraseType = CVariantDragLayer::eraseBMP;
		m_snapLayR.m_nEraseType = CVariantDragLayer::eraseBMP;
		
		m_snapTipLayL.m_nEraseType = CVariantDragLayer::eraseBMP;
		m_snapTipLayR.m_nEraseType = CVariantDragLayer::eraseBMP;
		
		m_accurboxLayL.m_nEraseType = CVariantDragLayer::eraseXOR;
		m_accurboxLayR.m_nEraseType = CVariantDragLayer::eraseXOR;
	}
	
	m_snapTipLayL.SetContext(m_pContext);
	m_snapTipLayR.SetContext(m_pContext);
	m_snapTipLayL.m_bUseCoordSys = FALSE;
	m_snapTipLayR.m_bUseCoordSys = FALSE;

	{	
		if( !CBaseView::InitBmpLayer() )return FALSE;

		Envelope eImgL = m_ImgLayLeft.GetDataBound();
		Envelope eImgR = m_ImgLayRight.GetDataBound();
		
		m_vectLayL.SetDataBound(eImgL);
		m_vectLayR.SetDataBound(eImgR);
		m_markLayL.SetDataBound(eImgL);
		m_markLayR.SetDataBound(eImgR);

		if( m_bViewVector )
		{
			m_laymgrLeft.InsertBeforeDrawingLayer(&m_vectLayL,&m_constDragL);
			m_laymgrRight.InsertBeforeDrawingLayer(&m_vectLayR,&m_constDragR);
		}

		m_laymgrLeft.InsertBeforeDrawingLayer(&m_markLayL,&m_constDragL);
		m_laymgrRight.InsertBeforeDrawingLayer(&m_markLayR,&m_constDragR);

		m_laymgrLeft.InsertBeforeDrawingLayer(&m_snapLayL,&m_cursorL);
		m_laymgrRight.InsertBeforeDrawingLayer(&m_snapLayR,&m_cursorR);

		m_laymgrLeft.InsertBeforeDrawingLayer(&m_snapTipLayL,&m_cursorL);
		m_laymgrRight.InsertBeforeDrawingLayer(&m_snapTipLayR,&m_cursorR);
		
		m_laymgrLeft.InsertBeforeDrawingLayer(&m_accurboxLayL,&m_variantDragL);
		m_laymgrRight.InsertBeforeDrawingLayer(&m_accurboxLayR,&m_variantDragR);
// 		m_accurboxLayL.m_nEraseType = CVariantDragLayer::eraseXOR;
// 	m_laymgrLeft.InsertBeforeDrawingLayer(&m_accurboxLayL,&m_variantDragL);
// 		if( pDrawL )pDrawL->m_bEnableMark2 = m_bShowKeyPoint;
// 		if( pDrawR )pDrawR->m_bEnableMark2 = m_bShowKeyPoint;
	}

	//by shy
//	SetVectLayNoLineWeight();
//	SetVectLayNoHatch();
//	SetVectLayDisplayOrder();

 	return TRUE;
}


float CStereoView::ZoomSelect(float wantZoom)
{
	if( m_nInnerCmd==IDC_BUTTON_ZOOMIN || m_nInnerCmd==IDC_BUTTON_ZOOMOUT )
	{
		float zoom = GetComboNextZoom(m_nInnerCmd==IDC_BUTTON_ZOOMIN?1:0);
		if( zoom>0 )return zoom;
	}
	return CSceneView::ZoomSelect(wantZoom);
}


void CStereoView::OnLButtonUp(UINT nFlags, CPoint point) 
{	
	if( m_nInnerCmd==0 && IsDrawStreamline() && !IsSysCross() )return;
	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;

	CBaseView::OnLButtonUp(nFlags, point);
}

void CStereoView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;
	
	CBaseView::OnRButtonUp(nFlags, point);
}



void CStereoView::SwitchModelForRefresh()
{

// 	AfxBringCallbackToTop(this,NULL);	
// 	AfxCallMessage(FCCM_REFRESHCURRENTSTEREOMS,0,(LPARAM)&m_strStereID);
	AfxGetMainWnd()->SendMessage(FCCM_REFRESHCURRENTSTEREOMS,0,(LPARAM)&m_strStereID);
 	StereoMove();	
}


void DrawSnapL(GrBuffer *buf, SNAPITEM item, PT_3D cpt, int r, int w, BOOL bIs2D);
void DrawSnapR(GrBuffer *buf, SNAPITEM item, PT_3D cpt, int r, int w, BOOL bIs2D);

void CStereoView::AddVariantDragLine(const GrBuffer *pBuf, BOOL bGround)
{
	if( !pBuf )return;
	
	GrBuffer buf;
	if( pBuf!=NULL && m_nDragLineWid>1 )
	{		
		buf.CopyFrom(pBuf);
		buf.SetAllLineWidth(FALSE,m_nDragLineWid);
		pBuf = &buf;
	}

	if (!m_pBufVariantDrag)
	{
		m_pBufVariantDrag = new GrBuffer;
		if( !m_pBufVariantDrag )return;
	}
	m_pBufVariantDrag->AddBuffer(pBuf);
	CBaseView::AddVariantDragLine(pBuf,bGround);
}

void CStereoView::AddConstDragLine(const GrBuffer *pBuf)
{
	if( !pBuf )return;

	GrBuffer buf;
	if( pBuf!=NULL && m_nDragLineWid>1 )
	{		
		buf.CopyFrom(pBuf);
		buf.SetAllLineWidth(FALSE,m_nDragLineWid);
		pBuf = &buf;
	}

	if (!m_pBufConstDrag)
	{
		m_pBufConstDrag = new GrBuffer;
		if( !m_pBufConstDrag )return;
	}
	m_pBufConstDrag->AddBuffer(pBuf);
	
	CBaseView::AddConstDragLine(pBuf);

	m_constDragL.SetObjMark(HANDLE_CONSTDRAG,m_bShowKeyPoint);
	m_constDragR.SetObjMark(HANDLE_CONSTDRAG,m_bShowKeyPoint);
	
	m_addconstDragL.SetObjMark(HANDLE_CONSTDRAG,m_bShowKeyPoint);
	m_addconstDragR.SetObjMark(HANDLE_CONSTDRAG,m_bShowKeyPoint);
}

void CStereoView::OnPreInnerCmd(UINT nInnerCmd)
{
	CBaseView::OnPreInnerCmd(nInnerCmd);
	if( m_nInnerCmd==IDC_BUTTON_REFRESH  )
	{
		m_bConstDragChanged = TRUE;
		UpdateConstDragLine();
	}
}

void CStereoView::OnPostInnerCmd()
{
	if( m_nInnerCmd==IDC_BUTTON_ZOOMIN || m_nInnerCmd==IDC_BUTTON_ZOOMOUT || 
		m_nInnerCmd==IDC_BUTTON_ZOOMFIT || m_nInnerCmd==IDC_BUTTON_ZOOMRECT )
	{
		m_bConstDragChanged = TRUE;
		UpdateConstDragLine();
	}

	//纹理模式下，也会时常使用移动光标，为了避免光标的不正常擦除，就重置一下
	if( m_pContext->m_nTextMode == CStereoDrawingContext::textureModeGL && m_nInnerCmd==IDC_BUTTON_HANDMOVE )
		ResetCursorDrawing();

	CBaseView::OnPostInnerCmd();
}

void CStereoView::DrawAccuBox()
{
	CDlgDoc *pDoc = GetDocument();

// 	if( m_bAccuBoxChanged )
// 	{
// 		m_bAccuBoxChanged = FALSE;
// 		
// 		GrBuffer buf;
// 		pDoc->m_accuBox.DrawBox(GetCoordMan(),&buf);
// 
// 		if( !m_pBufAccuBoxDraw )
// 		{
// 			m_pBufAccuBoxDraw = new GrBuffer;
// 		}
// 		if( m_pBufAccuBoxDraw )
// 		{
// 			m_pBufAccuBoxDraw->DeleteAll();
// 			m_pBufAccuBoxDraw->EnableWidFromGrd(FALSE);
// 			m_pBufAccuBoxDraw->AddBuffer(&buf);
// 			m_pBufAccuBoxDraw->RefreshEnvelope();
// 		}
// 	}
// 
// 	if( m_pBufAccuBoxErase && m_pBufAccuBoxErase!=m_pBufAccuBoxDraw )
// 		delete m_pBufAccuBoxErase;
// 
// 	if( m_bTextMode )
// 	{
// 		m_pBufAccuBoxErase = m_pBufAccuBoxDraw;
// 		
// 		AddObjtoVectLay(HANDLE_ACCUBOX,m_pBufAccuBoxDraw,TRUE,
// 			(CXVVectorLayer*)&m_eleTmpLeft,(CXVVectorLayer*)&m_eleTmpRight);
// 	}
// 	else
// 	{
// 		CDC *pDC = GetDC();
// 		DrawGrBuffer(pDC,m_pBufAccuBoxDraw,0,FALSE,DR_LEFT|DR_RIGHT);
// 		ReleaseDC(pDC);
// 
// 		m_pBufAccuBoxErase = m_pBufAccuBoxDraw;
// 
// 		AddObjtoVectLay(HANDLE_ACCUBOX,m_pBufAccuBoxDraw);
// 		m_vectLayL.UpdateVector(FALSE);
// 		m_vectLayR.UpdateVector(FALSE);
// 	}
}

void CStereoView::EraseAccuBox()
{
	CDlgDoc *pDoc = GetDocument();

// 	if( m_bTextMode )
// 	{
// 		DelObjfromVectLay(HANDLE_ACCUBOX,(CXVVectorLayer*)&m_eleTmpLeft,(CXVVectorLayer*)&m_eleTmpRight);
// 	}
// 	else
// 	{
// 		DelObjfromVectLay(HANDLE_ACCUBOX);
// 		m_vectLayL.UpdateVector(FALSE);
// 		m_vectLayR.UpdateVector(FALSE);
// 	}
}


void CStereoView::DrawAccuDrg()
{
/*	CBaseView::DrawAccuDrg();*/
}

void CStereoView::EraseAccuDrg()
{
/*	CBaseView::EraseAccuDrg();*/
}

void CStereoView::ClearAccuDrawing()
{
	CBaseView::ClearAccuDrawing();
}



void CStereoView::ClearDragLine()
{	
	SetConstDragLine(NULL);
	SetVariantDragLine(NULL);
	if (m_pBufVariantDrag)
	{
		delete m_pBufVariantDrag;
		m_pBufVariantDrag = NULL;
	}
	if (m_pBufConstDrag)
	{
		delete m_pBufConstDrag;
		m_pBufConstDrag = NULL;
	}
}



void CStereoView::UpdateConstDragLine()
{
	CBaseView::UpdateConstDragLine();
}



void CStereoView::UpdateAccuDrawing()
{
	CBaseView::UpdateAccuDrawing();
// 	if( m_bTextMode )
// 	{
// 		CDlgDoc *pDoc = GetDocument();
// 		if( !pDoc->m_accuBox.IsOpen() || m_bAccuLeave || !pDoc->m_accuBox.IsActive() )
// 		{
// 			ClearAccuDrawing();
// 			return;
// 		}
// 		
// 		if( !pDoc->m_accuBox.IsViewDraw() )
// 		{
// 			OnPreUpdateRegion(NULL, UPDATEREGION_ACCUDRG);
// 			EraseAccuDrg();
// 			DrawAccuDrg();
// 			OnPostUpdateRegion(NULL, UPDATEREGION_ACCUDRG);
// 			return;
// 		}
// 		
// 		EraseAccuBox();
// 		DrawAccuBox();
// 		Invalidate(FALSE);
// 	}
// 	else
// 	{
// 		BOOL bNeedUpdate = FALSE;
// 		if( m_pBufAccuBoxErase )
// 			bNeedUpdate = TRUE;
// 
// 		CBaseView::UpdateAccuDrawing();
// 		if( bNeedUpdate )
// 		{
// 			Invalidate(FALSE);
// 			UpdateWindow();
// 		}
// 	}
}


extern BOOL CheckStrZ(LPCTSTR strz, float& z);
void CStereoView::UpdateGridDrawing()
{
	DWORD dwScale = GetDocument()->GetDlgDataSource()->GetScale();
	double lfRatio = 0.01*dwScale;//图上1cm对应实际lfRatio米
	m_sGridParam.ox = GetProfileDouble(REGPATH_OVERLAYGRID,"MinX",m_sGridParam.ox);
	m_sGridParam.oy = GetProfileDouble(REGPATH_OVERLAYGRID,"MinY",m_sGridParam.oy);
	m_sGridParam.dx = lfRatio*GetProfileDouble(REGPATH_OVERLAYGRID,"Width",10);	
	m_sGridParam.dy = lfRatio*GetProfileDouble(REGPATH_OVERLAYGRID,"Height",10);
	m_sGridParam.xr = GetProfileDouble(REGPATH_OVERLAYGRID,"XRange",m_sGridParam.xr);
	m_sGridParam.yr = GetProfileDouble(REGPATH_OVERLAYGRID,"YRange",m_sGridParam.yr);
	strcpy(m_sGridParam.strz,AfxGetApp()->GetProfileString(REGPATH_OVERLAYGRID,"Z",_T("0.0")));
	m_sGridParam.bVisible = GetDocument()->IsOverlayGrid();
	m_sGridParam.bViewVect= AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"ViewVect",FALSE);
	m_sGridParam.bViewImg = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"ViewImg",FALSE);
	m_sGridParam.color = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"Color",RGB(128,128,128));

	BOOL bVisible = m_sGridParam.bViewImg&&m_sGridParam.bVisible;
	if( m_sGridParam.dx<=0 || m_sGridParam.dy<=0 )bVisible = FALSE;
	if( m_sGridParam.xr<=0 || m_sGridParam.yr<=0 )bVisible = FALSE;

	{
		m_vectLayL.DelObj(HANDLE_GRID);
		m_vectLayR.DelObj(HANDLE_GRID);
		
		m_vectLayL.ClearAll();
		m_vectLayR.ClearAll();
		
		if( !bVisible )
		{
		}
		else
		{
			GrBuffer buf;
		//	buf.SetColor(m_sGridParam.color);

			float gridz = 0, demz;
			BOOL bDirectZ = CheckStrZ(m_sGridParam.strz,gridz);

			CDSM dem;
			if( !bDirectZ )
			{
				if( !dem.Open(m_sGridParam.strz) )
					bDirectZ = TRUE;
			}
			
			int i, j, nx = ceil(m_sGridParam.xr/m_sGridParam.dx), ny = ceil(m_sGridParam.yr/m_sGridParam.dy);
			double xr = m_sGridParam.ox+nx*m_sGridParam.dx, yr = m_sGridParam.oy+ny*m_sGridParam.dy, x,y;
			PT_3D pt (0,0,gridz);
			for( x=0, i=0; i<=nx; i++, x+=m_sGridParam.dx )
			{
				pt.x = m_sGridParam.ox+x; pt.y = m_sGridParam.oy;
				demz = dem.GetZ(pt.x,pt.y);
				if( !bDirectZ && demz>DemNoValues+1 )
					pt.z = demz;
				else
					pt.z = gridz;
				buf.BeginLineString(m_sGridParam.color,0);
				buf.MoveTo(&pt);

				for( y=0, j=0; j<=ny; j++, y+=m_sGridParam.dy )
				{
					pt.y = m_sGridParam.oy+y;

					demz = dem.GetZ(pt.x,pt.y);
					if( !bDirectZ && demz>DemNoValues+1 )
						pt.z = demz;
					else
						pt.z = gridz;
					buf.LineTo(&pt);
				}

				pt.y = yr;

				demz = dem.GetZ(pt.x,pt.y);
				if( !bDirectZ && demz>DemNoValues+1 )
					pt.z = demz;
				else
					pt.z = gridz;
				buf.LineTo(&pt);
				buf.End();
			}
			
			for( y=0, i=0; i<=ny; i++, y+=m_sGridParam.dy )
			{
				pt.y = m_sGridParam.oy+y; pt.x = m_sGridParam.ox;
				demz = dem.GetZ(pt.x,pt.y);
				if( !bDirectZ && demz>DemNoValues+1 )
					pt.z = demz;
				else
					pt.z = gridz;
				buf.BeginLineString(m_sGridParam.color,0);
				buf.MoveTo(&pt);

				for( x=0, j=0; j<=nx; j++, x+=m_sGridParam.dx )
				{
					pt.x = m_sGridParam.ox+x; 
					demz = dem.GetZ(pt.x,pt.y);
					if( !bDirectZ && demz>DemNoValues+1 )
						pt.z = demz;
					else
						pt.z = gridz;

					buf.LineTo(&pt);
				}
				
				pt.x = xr;
				demz = dem.GetZ(pt.x,pt.y);
				if( !bDirectZ && demz>DemNoValues+1 )
					pt.z = demz;
				else
					pt.z = gridz;
				
				buf.LineTo(&pt);
				buf.End();
			}
			
			AddObjtoVectLay(HANDLE_GRID,&buf);
			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();
		}
	}
}


void CStereoView::CheckCoordMode()
{
	if( !m_bAutoAdjustCoordMode )return;

	if (m_nImageType == SCANNER || (m_nImageType == EPIPOLAR&&m_bRealTimeEpipolar))
	{
		if( (m_dwModeCoord&CROSS_MODE_HEIGHT)==0 )
			m_dwModeCoord |= CROSS_MODE_HEIGHT;

		if( (m_dwModeCoord&CROSS_MODE_AUTO)!=0 )
			m_dwModeCoord ^= CROSS_MODE_AUTO;
	}

	if( m_bSysCross )
	{
		if( (m_dwModeCoord&CROSS_MODE_SHARE)==0 )
			m_dwModeCoord |= CROSS_MODE_SHARE;
	}

	UpdateButtonStates();
}


void CStereoView::UpdateButtonStates()
{	
	if( m_dwModeCoord&CROSS_MODE_HEIGHT )
	{
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_AUTOMODE,FALSE,FALSE);
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_HEIGHTMODE,TRUE,TRUE);
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_HEIGHTLOCK,TRUE,(m_dwModeCoord&CROSS_MODE_HEIGHTLOCK)!=0);
	}
	else
	{
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_AUTOMODE,TRUE,(m_dwModeCoord&CROSS_MODE_AUTO)!=0);
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_HEIGHTMODE,TRUE,FALSE);
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_HEIGHTLOCK,FALSE,(m_dwModeCoord&CROSS_MODE_HEIGHTLOCK)!=0);
	}

	if( m_dwModeCoord&CROSS_MODE_SHARE )
	{
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_SHARECROSS,TRUE,TRUE);
	}
	else
	{
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_SHARECROSS,TRUE,FALSE);
	}
}


BOOL CStereoView::IsDrawStreamline()
{
	if( !m_bDisableMouseIn3DDraw )
	{
		m_b3DDraw = FALSE;
		return FALSE;
	}
	
	CCommand *pCurCmd = GetWorker()->GetCurrentCommand();
	
	if( pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)) && IsProcProcessing(pCurCmd))
	{
		if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE &&
			((CDrawCommand*)pCurCmd)->GetCurPenCode()==penStream)
		{
			m_b3DDraw = TRUE;
			return TRUE;
		}

		if( m_b3DDraw &&
			((CDrawCommand*)pCurCmd)->GetCurPenCode()==penStream)
		{
			return TRUE;
		}
	}

	m_b3DDraw = FALSE;
	return FALSE;
}



void CStereoView::AddObjtoVectLay(LONG_PTR handle, const GrBuffer *pBuf, BOOL bGround, CVectorLayer *pLL, CVectorLayer *pLR)
{
	if( !pBuf )return;

	if( !pLL )pLL = &m_vectLayL;
	if( !pLR )pLR = &m_vectLayR;

	long hspecial[] = {
	//	HANDLE_HILITE,HANDLE_ACCUBOX,
		HANDLE_CONSTDRAG,HANDLE_GRID,
		HANDLE_BOUND,HANDLE_GRAPH
	};
	for( int k=sizeof(hspecial)/sizeof(hspecial[0])-1; k>=0; k--)
	{
		if( handle==hspecial[k] )break;
	}
	BOOL bUseStereoEvlp = m_bUseStereoEvlp;
	if( k>=0 )bUseStereoEvlp = FALSE;

	{		
		GrBuffer2d* pBufL=NULL;
		GrBuffer2d* pBufR=NULL;
		
		pBufL = (GrBuffer2d*)pLL->OpenObj(handle);
		if( !IsSingle() )pBufR = (GrBuffer2d*)pLR->OpenObj(handle);

		GrBuffer newBuf;
		if( bUseStereoEvlp )
		{
			newBuf.CopyFrom(pBuf);
			DisplayTrim(&newBuf);
			pBuf = &newBuf;
		}
		
		ConvertGrBufferToVectLayer(pBuf,pBufL,pBufR);

		if( bUseStereoEvlp )
		{
//			HideOutsideVertexes(pBufL,pBufR);
		}
		
		pLL->ClearAll();
		pLR->ClearAll();

		pLL->FinishObj(handle);
		pLR->FinishObj(handle);
	}
}

void CStereoView::HideOutsideVertexes(GrBuffer2d *pBufL, GrBuffer2d *pBufR)
{
	if( !pBufL )return;
	if( !pBufL->HeadGraph() )
		return;

	float lxoff=0, lyoff=0;
	float rxoff=0, ryoff=0;

	pBufL->GetOrigin(lxoff,lyoff);
	pBufR->GetOrigin(rxoff,ryoff);

	CSize szImg1 = m_ImgLayLeft.GetImageSize();
	CSize szImg2 = m_ImgLayRight.GetImageSize();

	CRect rcImg1(0,0,szImg1.cx,szImg1.cy);
	CRect rcImg2(0,0,szImg2.cx,szImg2.cy);

	if( m_nImageType==EPIPOLAR && m_bRealTimeEpipolar )
	{
		{
			double x[4] = {0,0,szImg1.cx,szImg1.cx}, y[4] = {0,szImg1.cy,szImg1.cy,0};
			for( int i=0; i<4; i++)
			{
				double x1,y1;
				RealtimeConvertCoordLeft(x[i],y[i],x1,y1,TRUE);
				x[i] = x1;
				y[i] = y1;
			}
			rcImg1.left = min(min(x[0],x[1]),min(x[2],x[3]));
			rcImg1.right = max(max(x[0],x[1]),max(x[2],x[3]));
			rcImg1.top = min(min(y[0],y[1]),min(y[2],y[3]));
			rcImg1.bottom = max(max(y[0],y[1]),max(y[2],y[3]));
		}
		{
			double x[4] = {0,0,szImg2.cx,szImg2.cx}, y[4] = {0,szImg2.cy,szImg2.cy,0};
			for( int i=0; i<4; i++)
			{
				double x1,y1;
				RealtimeConvertCoordRight(x[i],y[i],x1,y1,TRUE);
				x[i] = x1;
				y[i] = y1;
			}
			rcImg2.left = min(min(x[0],x[1]),min(x[2],x[3]));
			rcImg2.right = max(max(x[0],x[1]),max(x[2],x[3]));
			rcImg2.top = min(min(y[0],y[1]),min(y[2],y[3]));
			rcImg2.bottom = max(max(y[0],y[1]),max(y[2],y[3]));
		}
	}
	
	//外扩距离
	rcImg1.InflateRect(rcImg1.Width()*0.1,rcImg1.Height()*0.1);
	rcImg2.InflateRect(rcImg2.Width()*0.1,rcImg2.Height()*0.1);

	//遍历坐标点 并转换坐标	
	const GrVertexList2d *pList;
	
	Graph2d *grl = pBufL->HeadGraph();
	Graph2d *grr = pBufR->HeadGraph();

	PT_3D pt1, pt2, pt3, pt4;
	GrVertex2d pt0;

	CArray<Graph2d*,Graph2d*> arrDel1, arrDel2;

	while(grl)
	{
		Envelope e1 = GetEnvelopeOfGraph2d(grl);
		Envelope e2 = GetEnvelopeOfGraph2d(grr);
		e1.Offset(lxoff,lyoff,0);
		e2.Offset(rxoff,ryoff,0);
		if( e1.m_xh<rcImg1.left || e1.m_xl>rcImg1.right || e1.m_yh<rcImg1.top || e1.m_yl>rcImg1.bottom ||
			e2.m_xh<rcImg2.left || e2.m_xl>rcImg2.right || e2.m_yh<rcImg2.top || e2.m_yl>rcImg2.bottom )
		{
			arrDel1.Add(grl);
			arrDel2.Add(grr);
		}
		else
		{
			if (IsGrLineString(grl))
			{
				GrLineString2d *grLine = (GrLineString2d *)grl;

				pList = &grLine->ptlist;

				const GrVertexList2d *pList2 = &((GrLineString2d*)grr)->ptlist;

				for (int i=0;i<pList->nuse-1;i++)
				{
					pt0 = pList->pts[i];
					pt1.x = pt0.x + lxoff;
					pt1.y = pt0.y + lyoff;

					pt0 = pList->pts[i+1];
					pt2.x = pt0.x + lxoff;
					pt2.y = pt0.y + lyoff;

					pt0 = pList2->pts[i];
					pt3.x = pt0.x + rxoff;
					pt3.y = pt0.y + ryoff;
					
					pt0 = pList2->pts[i+1];
					pt4.x = pt0.x + rxoff;
					pt4.y = pt0.y + ryoff;

					if((( pt1.x<rcImg1.left || pt1.x>rcImg1.right || pt1.y<rcImg1.top || pt1.y>rcImg1.bottom ) &&
						( pt2.x<rcImg1.left || pt2.x>rcImg1.right || pt2.y<rcImg1.top || pt2.y>rcImg1.bottom ))||
					   (( pt3.x<rcImg2.left || pt3.x>rcImg2.right || pt3.y<rcImg2.top || pt3.y>rcImg2.bottom ) &&
						( pt4.x<rcImg2.left || pt4.x>rcImg2.right || pt4.y<rcImg2.top || pt4.y>rcImg2.bottom )))
					{
						//变成 MOVETO
						pList->pts[i+1].code = (pList->pts[i+1].code&~GRBUFFER_PTCODE_LINETO);
						pList2->pts[i+1].code = (pList2->pts[i+1].code&~GRBUFFER_PTCODE_LINETO);
					}		
				}		
			}
		}
		grl = grl->next;
		grr = grr->next;
	}

	for( int i=0; i<arrDel1.GetSize(); i++)
	{
		pBufL->DeleteGraph(arrDel1[i]);
		pBufR->DeleteGraph(arrDel2[i]);
	}

	if(pBufL)
	{
		pBufL->RefreshEnvelope(TRUE);
	}
	if(pBufR)
	{
		pBufR->RefreshEnvelope(TRUE);
	}
}


void CStereoView::AddObjtoMarkVectLay(CFeature *pFtr)
{
	ASSERT(pFtr!=NULL);

	CGeometry * pObj = pFtr->GetGeometry();
	if (!pObj) return;	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int i;	
	PT_3D pt0;
	PT_4D pt1;
	pObj->GetShape(arrPts);
	if (arrPts.GetSize()<1)
	{
		return;
	}
	GrBuffer2d *pBufL = NULL;
	GrBuffer2d *pBufR = NULL;
	pBufL = (GrBuffer2d*)m_markLayL.OpenObj((LONG_PTR)pFtr);
	pBufR = (GrBuffer2d*)m_markLayR.OpenObj((LONG_PTR)pFtr);
	
	COPY_3DPT(pt0,(arrPts[0]));
	GroundToVectorLay(&pt0,&pt1);

	float lxoff = floor(pt1.x/1e+3)*1e+3, lyoff = floor(pt1.y/1e+4)*1e+3;
	float rxoff = floor(pt1.z/1e+3)*1e+3, ryoff = floor(pt1.yr/1e+4)*1e+3;

	PT_2D pt2d0,pt2d1;
	if (pBufL)
	{
		pBufL->SetOrigin(lxoff,lyoff);
		pBufL->BeginLineString(RGB(255,255,255),1);
		pt2d0.x = pt1.x;
		pt2d0.y = pt1.y;
		pBufL->MoveTo(&pt2d0);
	}
	if (pBufR)
	{
		pBufR->SetOrigin(rxoff,ryoff);
		pBufR->BeginLineString(RGB(255,255,255),1);
		pt2d1.x = pt1.z;
		pt2d1.y = pt1.yr;
		pBufR->MoveTo(&pt2d1);
	}
	
	for (i=1;i<arrPts.GetSize();i++)
	{	
		COPY_3DPT(pt0,(arrPts[i]));
		GroundToVectorLay(&pt0,&pt1);
		if (pBufL)
		{
			pt2d0.x = pt1.x;
			pt2d0.y = pt1.y;
			pBufL->LineTo(&pt2d0);
		}
		if (pBufR)
		{
			pt2d1.x = pt1.z;
			pt2d1.y = pt1.yr;
			pBufR->LineTo(&pt2d1);
		}		
	}		
	for (i=0;i<pObj->GetCtrlPointSum();i++)
	{
		pt0 = pObj->GetCtrlPoint(i);
		GroundToVectorLay(&pt0,&pt1);
		if (pBufL)
		{
			pt2d0.x = pt1.x;
			pt2d0.y = pt1.y;
			pBufL->LineTo(&pt2d0);
		}
		if (pBufR)
		{
			pt2d1.x = pt1.z;
			pt2d1.y = pt1.yr;
			pBufR->LineTo(&pt2d1);
		}		
	}	
	{
		pBufL->End();
		pBufR->End();
	}

	if( pBufL )
	{
		m_markLayL.SetObjMark2((LONG_PTR)pFtr, m_bShowKeyPoint);
		if( m_bShowKeyPoint )m_markLayL.ClearAll();
	}
	if( pBufR )
	{
		m_markLayR.SetObjMark2((LONG_PTR)pFtr, m_bShowKeyPoint);
		if( m_bShowKeyPoint )m_markLayR.ClearAll();
	}
}


void CStereoView::ManualLoadVect(Envelope evlp)
{
	BOOL bManualLoad = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MANUALLOADVECT,FALSE);
	if( !bManualLoad )return;
		
	CDlgDoc *pDoc = GetDocument();

	//获取实体对象总数
// 	DWORD dwSaveMode = GetWorker()->m_selection.m_dwFindMode;
// 	pDoc->m_selection.m_dwFindMode |= GETMODE_INC_LOCKED;
	PT_3D pt1[2];
	PT_4D pt2;
	pt1[0].x = evlp.m_xl;
	pt1[0].y = evlp.m_yl;
	pt1[0].z = 0;
	GetCoordWnd().m_pSearchCS->GroundToClient(pt1,&pt2);
	pt1[0] = PT_3D(pt2);
	pt1[1].x = evlp.m_xh;
	pt1[1].y = evlp.m_yh;
	pt1[1].z = 0;
	GetCoordWnd().m_pSearchCS->GroundToClient(pt1+1,&pt2);
	pt1[1] = PT_3D(pt2);
	evlp.CreateFromPts(pt1,2);
	
	//evlp为客户坐标系
	long lSum = pDoc->GetDataQuery()->FindObjectInRect(evlp,GetCoordWnd().m_pSearchCS);
 	
	if( lSum<=0 )return; 
	int num;
	const CPFeature *ftr = pDoc->GetDataQuery()->GetFoundHandles(num);

	GOutPut(StrFromResID(IDS_VIEW_VECTOR));
	GProgressStart(lSum);
	
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	
	if( pDS )
	{
		BOOL bEnable;
		COLORREF monoClr;
		pDS->GetMonoColor(&bEnable,&monoClr);

		for( int i=0; i<lSum; i++)
		{
			//增长进度条
			GProgressStep();
			CFeature *pFtr = ftr[i];
			if (!pFtr)continue;

			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
			if (!pLayer) continue;

			BOOL bViewValue = TRUE;
			m_mapLayerVisible.Lookup(pLayer,bViewValue);
			
			bViewValue &= pLayer->IsVisible();
			
			CGeometry *pObj = ftr[i]->GetGeometry();
			if( pObj )
			{
				if( CanSymbolized(pFtr) && pLayer->IsSymbolized())
				{
					GrBuffer buf;
					
					if((!pDS->DrawFeature(pFtr,&buf,pObj->IsSymbolizeText(),m_gsa.lfAngle)&&pFtr->GetGeometry()->GetClassType()==CLS_GEOTEXT)||!CAnnotation::m_bUpwardText)
						;//buf.RotateGrTextPar(m_gsa.lfAngle*180/PI);
					else if(CAnnotation::m_bUpwardText)
					{
						buf.RotateGrTextEnvelope(m_gsa.lfAngle);
					}
					if (bEnable)
					{
						buf.SetAllColor(monoClr);
					}
					else if(pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER || m_bSymbolscolorByLayer)
					{
						buf.SetAllColor(pLayer->GetColor());
					}
					
					AddObjtoVectLay((LONG_PTR)pFtr, &buf);
				}
				else
				{
					GrBuffer buf;
					pFtr->Draw(&buf,pDS->GetSymbolDrawScale());
					if (bEnable)
					{
						buf.SetAllColor(monoClr);
					}								
					else if(pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER || m_bSymbolscolorByLayer)
					{
						buf.SetAllColor(pLayer->GetColor());
					}						
					if (pFtr->GetGeometry()->GetClassType()==CLS_GEOTEXT)
					{
						//buf.RotateGrTextPar(m_gsa.lfAngle*180/PI);
					}							
					AddObjtoVectLay((LONG_PTR)pFtr, &buf);
				}

				if(!bViewValue)
				{
					m_vectLayL.SetObjVisible((LONG_PTR)pFtr, FALSE);
					m_vectLayR.SetObjVisible((LONG_PTR)pFtr, FALSE);
				}

				AddObjtoMarkVectLay(ftr[i]);

			}
		}

		m_markLayL.ClearAll();
		m_markLayR.ClearAll();
		m_vectLayL.ClearAll();
		m_vectLayR.ClearAll();
	}

	//进度条复位
	GProgressEnd();
	GOutPut(StrFromResID(IDS_VIEW_VECTOREND));

	Invalidate(FALSE);
	UpdateWindow();

	return;
}
//for test
void WriteToFile(HBITMAP hBmp,LPCSTR strpath)
{
	DIBSECTION dibInfo;
	::GetObject(hBmp,sizeof(dibInfo),&dibInfo);
	BITMAPFILEHEADER fileInfo;
	BITMAPINFO		 bmpInfo;
	
	fileInfo.bfType = 0x4d42;
	fileInfo.bfSize = sizeof(fileInfo)+sizeof(bmpInfo)+dibInfo.dsBmih.biSizeImage;
	fileInfo.bfReserved1 = 0;
	fileInfo.bfReserved2 = 0;
	fileInfo.bfOffBits = sizeof(fileInfo)+sizeof(bmpInfo);
	
	memset(&bmpInfo,0,sizeof(bmpInfo));
	memcpy(&bmpInfo.bmiHeader,&dibInfo.dsBmih,sizeof(dibInfo.dsBmih));
	
	char fileName[256];
	static int num=0;
	//"E:\\bmp\\test%d.bmp"
	sprintf(fileName, strpath, num++);
	FILE *fp = fopen(fileName,"wb");
	if( !fp )return;
	fwrite(&fileInfo,sizeof(fileInfo),1,fp);
	fwrite(&bmpInfo ,sizeof(bmpInfo ),1,fp);
	fwrite(dibInfo.dsBm.bmBits,1,dibInfo.dsBmih.biSizeImage,fp);
	fclose(fp);
}

void CStereoView::OnExportRaster()
{
	CDlgExportRaterStereo dlg;

	if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeRGStereo||m_pContext->GetDisplayMode()==CStereoDrawingContext::modeInterleavedStereo|| m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSingle )
		dlg.m_nImage = -1;

	if( dlg.DoModal()!=IDOK )return;

	if( dlg.m_strFilePath.GetLength()<=0 )
		return;

// 	dlg.m_strFilePath = "F:\\1.tif";
// 	dlg.m_nImage = 1;
		
	CDlgDoc *pDoc = GetDocument();

	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_PREPARE) );

	TIFF *tif = TIFFOpen(dlg.m_strFilePath,"w");
	
	if( tif )
	{
		float oldzoom = GetZoomRate();

		// 按像素分辨率
 		if (dlg.m_nScaleType == 1)
		{
			// 缩放倍率
			float fScale = 1000*dlg.m_fPixelSize/(m_lfScanSize*dlg.m_fPGScale);
			ZoomChange(CPoint(0,0),fScale/oldzoom,FALSE);
		}

		CSize size;
		Envelope el,er,ee;
		el = m_ImgLayLeft.GetDataBound();
		er = m_ImgLayRight.GetDataBound();
		CPtrArray pLayerListL,pLayerListR;
		int drawSide = 0;
		PT_3D start;
		if( dlg.m_nImage==0 )
		{
			m_ImgLayLeft.ClearAll();
			m_ImgLayLeft.m_bForceDraw = TRUE;
			ee = el;			
			drawSide |= CStereoDrawingContext::drawLeft; 
			pLayerListL.Add(&m_ImgLayLeft);
			if (m_bViewVector)
			{
				pLayerListL.Add(&m_vectLayL);
			}						
		}
		else if( dlg.m_nImage==1 )
		{
			m_ImgLayRight.ClearAll();
			m_ImgLayRight.m_bForceDraw = TRUE;
			ee = er;			
			drawSide |= CStereoDrawingContext::drawRight; 
			pLayerListR.Add(&m_ImgLayRight);
			if (m_bViewVector)
			{
				pLayerListR.Add(&m_vectLayR);
			}
		}
		else
		{
			ee = el;
			drawSide |= (CStereoDrawingContext::drawRight|CStereoDrawingContext::drawLeft);
			m_ImgLayLeft.m_bForceDraw = TRUE;
			pLayerListL.Add(&m_ImgLayLeft);
			if (m_bViewVector)
			{
				pLayerListL.Add(&m_vectLayL);		
			}

			m_ImgLayRight.m_bForceDraw = TRUE;
			pLayerListR.Add(&m_ImgLayRight);
			if (m_bViewVector)
			{
				pLayerListR.Add(&m_vectLayR);
			}
		}
		PT_4D pt0,pt1,pt2;
		PT_3D oldgpt;
		pt0.x = ee.m_xl;
		pt0.y = ee.m_yl;
		pt0.z = pt0.x;
		pt0.yr = pt0.y;
		ImageToClient(&pt0,&pt1);
		pt0.x = ee.m_xh;
		pt0.y = ee.m_yh;
		pt0.z = pt0.x;
		pt0.yr = pt0.y;
		ImageToClient(&pt0,&pt2);
		if(dlg.m_nImage==0)
		{
		size.cx = fabs(pt2.x-pt1.x);
		size.cy = fabs(pt2.y-pt1.y);
		}
		else if (dlg.m_nImage==1)
		{
			size.cx = fabs(pt2.z-pt1.z);		
			size.cy = fabs(pt2.yr-pt1.yr);
		}
		else
		{
			size.cx = fabs(pt2.x-pt1.x);		
			size.cy = fabs(pt2.y-pt1.y);			
		}

		if (__int64(size.cx)*size.cy*3 > __int64(500*1024*1024))
		{
			TIFFClose(tif);
			AfxMessageBox(IDS_EXPORTIMAGE_SCALELARGE);
			return;
		}

		if(m_pContext->GetDisplayMode()==CStereoDrawingContext::modeInterleavedStereo)size.cy*=2;
		pt0.x = ee.m_xl;
		pt0.y = ee.m_yh;
		pt0.z = pt0.x;
		pt0.yr = pt0.y;
		ImageToClient(&pt0,&pt1);
		//保存原始位置的左上角的大地坐标
		pt0 = PT_4D(0,0,0,0);
		ClientToGround(&pt0,&oldgpt);

		int nx = 0, ny = 0, stepx = 512, stepy = 512;
		if (size.cx<512)
		{
			stepx = size.cx;
		}
		DWORD nbytes = 0;
		CRect bmpRc(0,0,stepx,stepy);
		CRect wndRc(0,0,stepx,stepy); 	

		TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, size.cx);
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH, size.cy);
		TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
		TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
		TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
		TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, stepy);
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		if (dlg.m_nScaleType == 1)
		{
			// 成图比例尺
			double scale = pDoc->GetDlgDataSource()->GetScale();
			
			float dpi = (int)2.54*scale/(100*dlg.m_fPixelSize);

			TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
			TIFFSetField(tif, TIFFTAG_XRESOLUTION, dpi);
			TIFFSetField(tif, TIFFTAG_YRESOLUTION, dpi);
		}
		else
		{
			TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_NONE);
		}
		TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		
		long lSum = ceil(size.cy*1.0/stepy);
		
		AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_PROCESS) );
		AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, lSum);

		int w = size.cx, h = stepy;
		
		CRect rcClient;
		GetClientRect(&rcClient);		
		MoveWindow(0,0,stepx,stepy);
	//	m_pContext->SetDCSize(CSize(stepx,stepy));
		//m_pContext->SetDCSize(rcClient.Size());
		// a new bitmap need to be created
		BITMAPINFO bitmapInfo;
		
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biWidth = w;
		bitmapInfo.bmiHeader.biHeight = h;
		bitmapInfo.bmiHeader.biSizeImage = ((w*3+3)&(~3))*h;
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = 24;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;
		bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
		bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
		bitmapInfo.bmiHeader.biClrUsed = 0;
		bitmapInfo.bmiHeader.biClrImportant = 0;
		
		BYTE *pBits=NULL;
		HBITMAP hBmp = CreateDIBSection( NULL,&bitmapInfo,DIB_RGB_COLORS,
			(void**)&pBits,0,0L );
		
		if( hBmp && pBits )
			memset(pBits, 0, bitmapInfo.bmiHeader.biSizeImage);
		
		BYTE *pBuf = new BYTE[w*h*3];
		
		if( pBuf&&hBmp&&pBits )
		{	
			if(dlg.m_nImage==0)ScrollForDrawBitmap(pt1.x,pt1.y,0);
			else if(dlg.m_nImage==1)ScrollForDrawBitmap(pt1.z,pt1.yr,0);
			else ScrollForDrawBitmap(pt1.x,pt1.y,0);
			int rollback = (ceil(double(size.cx)/stepx)-1)*stepx;
			//size.cy = stepy;
			if (size.cx<512)
			{
				rollback = 0;
			}	
			for( ny=0; ny<size.cy; ny+=stepy)
			{
				if(ny!=0)
				{
					wndRc = CRect(0,0,stepx,stepy);
					bmpRc = CRect(0,0,stepx,stepy);
					if(m_pContext->GetDisplayMode()==CStereoDrawingContext::modeInterleavedStereo)
						ScrollForDrawBitmap(-rollback,stepy/2,0);
					else
					ScrollForDrawBitmap(-rollback,stepy,0);
				}
				//增长进度条
				AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, 1);
				
				//size.cx = stepx;
				for(nx=0; nx<size.cx; nx+=stepx)
				{
 					if(nx!=0)
					{
						
						bmpRc.OffsetRect(stepx,0);
						if(nx+stepx>size.cx)
						{
							wndRc = CRect(0,0,size.cx-nx,stepy);
							bmpRc.DeflateRect(0,0,stepx-size.cx+nx,0);
						}
 						ScrollForDrawBitmap(stepx,0,0);
					}				
					DrawRectBitmap(hBmp,bmpRc,&wndRc,pLayerListL,pLayerListR,drawSide);	
//					WriteToFile(hBmp);
				
				}
				//获取地址
				BITMAP bmp;
				::GetObject(hBmp,sizeof(BITMAP),&bmp);
				
				BYTE *pSrc = (BYTE*)bmp.bmBits;
				BYTE *p1, *p2;
				
				
				int nw = bmp.bmWidth;
				int nh = bmp.bmHeight;
				
				//交换字节
				for( int j=0; j<nh; j++)
				{
					p1 = pSrc + (DWORD)(bmp.bmHeight-1-j)*(DWORD)((bmp.bmWidth*3+3)&(~3));
					p2 = pBuf + j*w*3;
					for( int i=0; i<nw; i++)
					{
						p2[0] = p1[2];
						p2[1] = p1[1];
						p2[2] = p1[0];
						p1 += 3;
						p2 += 3;
					}
				}			
				if (ny+stepy>size.cy)
				{
					nh =  size.cy-ny;
				}
				for( j=0; j<nh; j++)
				{
					TIFFWriteScanline(tif, pBuf+j*nw*3, ny+j, 0);
				}
			}		
		}	
		if( pBuf )delete[] pBuf;		
		MoveWindow(0,0,rcClient.Width(),rcClient.Height());
		GroundToClient(&oldgpt,&pt1);
		if(dlg.m_nImage==0)ScrollForDrawBitmap(pt1.x,pt1.y,0);
		else if(dlg.m_nImage==1)ScrollForDrawBitmap(pt1.z,pt1.yr,0);
		else ScrollForDrawBitmap(pt1.x,pt1.y,0);

		if (dlg.m_nScaleType == 1)
		{
			ZoomChange(CPoint(0,0),oldzoom/GetZoomRate(),FALSE);
		}
		
		
	}
	TIFFClose(tif);

	//写 tfw 文件
	if( dlg.m_nScaleType == 1 )
	{
		CString name = dlg.m_strFilePath;
		if( name.Right(3).CompareNoCase("tif")==0 )
		{
			name = name.Left(name.GetLength()-3) + "tfw";
		}
		else
			name += ".tfw";
		
		FILE *fp = fopen(name,"wt");
		if( fp!=NULL )
		{
			double v[6];

			v[0] = dlg.m_fPixelSize;
			v[1] = 0;
			v[2] = 0;
			v[3] = -dlg.m_fPixelSize;
			v[4] = 0;
			v[5] = 0;
			
			fprintf(fp,"%.6f\n%.6f\n%.6f\n%.6f\n%.6f\n%.6f\n",
				v[0],v[1],v[2],v[3],v[4],v[5]);
			fclose(fp);
		}
	}

	//进度条复位
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);


	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_END) );
	Invalidate(FALSE);

	return;
}

void CStereoView::OnReset3DMouseParam()
{
    int		i;
	double	xSrc[4] ,ySrc[4] ;
	double	xDest[4],yDest[4];
	
	PT_4D pts[4],pts1[4];
	PT_3D pts2[4];
	CRect rcClient;
	GetClientRect(&rcClient);
	
	//赋予合适的影像坐标
	int sx=0, ex=0, sy=0, ey=0;
	
	sx = rcClient.left;
	ex = rcClient.right;
	sy = rcClient.top;
	ey = rcClient.bottom;
	
	pts[0].x = pts[3].x = sx;			
	pts[1].x = pts[2].x = ex;
	pts[0].y = pts[1].y = sy;
	pts[2].y = pts[3].y = ey;
	
	pts[0].z = pts[3].z = pts[0].x;			
	pts[1].z = pts[2].z = pts[1].x;
	pts[0].yr= pts[1].yr= pts[0].y;
	pts[2].yr= pts[3].yr= pts[2].y;
	
	//计算相应的大地坐标
	for ( i=0;i<4; i++)
	{
		ClientToImage(pts+i,pts1+i);
		ImageToGround(pts1+i,pts2+i);
		
		xSrc[i] = pts1[i].x; ySrc[i] = pts1[i].y;
		xDest[i] = pts2[i].x; yDest[i] = pts2[i].y;
	}
	
	//求出仿射关系
	CalcAffineParams( xSrc,ySrc,xDest,yDest,4,m_trA,m_trB );


	//计算高程和水平视差的系数比
	m_trZ = 1.0;	
	
	double dp = 0;
	{
		Coordinate coord1,coord2,coord3,coord4;
		for ( i=0;i<4; i++)
		{
			coord1.x=pts2[i].x; coord1.y=pts2[i].y;	coord1.z=pts2[i].z;
			coord1.iType=GROUND; 
			coord2.iType=m_nImageType;
			
			Convert(coord1,coord2);
			coord3 = coord1, coord4 = coord2;
			
			coord1.x=pts2[i].x; coord1.y=pts2[i].y;	coord1.z=pts2[i].z+10;
			coord1.iType=GROUND; 
			coord2.iType=m_nImageType;
			
			Convert(coord1,coord2);
			dp = dp
				+ sqrt((coord2.rx-coord4.rx)*(coord2.rx-coord4.rx)+(coord2.ry-coord4.ry)*(coord2.ry-coord4.ry))
				+ sqrt((coord2.lx-coord4.lx)*(coord2.lx-coord4.lx)+(coord2.ly-coord4.ly)*(coord2.ly-coord4.ly));
		}
		
		dp = 40/dp;
		m_trZ = dp;
	}

}


void CStereoView::OnUpdateNoHatch(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_pContext->m_bNoHatch);
}


void CStereoView::OnUpdateDisplayOrder(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bDisplayOrder);
}


void CStereoView::OnShareSysMouse()
{
	m_bSysCross = !m_bSysCross;
	AfxGetApp()->WriteProfileInt(m_strRegSection,"ShareSysCursorForStereo",m_bSysCross);

	EnableSysCross(m_bSysCross);
}


void CStereoView::OnUpdateShareSysMouse(CCmdUI* pCmdUI)
{
	if( (IsShutterStereo() || IsPolarized()) && !IsHardCross() )
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_bSysCross);
}


void CStereoView::OnReverseCrossColor()
{
	SetCrossColor((~GetCrossColor())&0xffffff);
	UpdateCross();
}


void CStereoView::ClearSnapDrawing()
{
	m_snapTipLayL.SetBuffer((GrBuffer2d*)NULL);
	m_snapTipLayR.SetBuffer((GrBuffer2d*)NULL);
	UpdateDrawingLayers(&m_snapTipLayL,&m_snapTipLayR);

	CBaseView::ClearSnapDrawing();
}
void CStereoView::SetVariantDragLine(const GrBuffer *pBuf, BOOL bGround)
{
	if(m_pBufVariantDrag==pBuf) return;
	if(m_pBufVariantDrag&&m_pBufVariantDrag!=pBuf)delete m_pBufVariantDrag;
	m_pBufVariantDrag = NULL;

	GrBuffer buf;
	if( pBuf!=NULL && m_nDragLineWid>1 )
	{		
		buf.CopyFrom(pBuf);
		buf.SetAllLineWidth(FALSE,m_nDragLineWid);
		pBuf = &buf;
	}

	if( pBuf )
	{
		m_pBufVariantDrag = new GrBuffer;
		if(!m_pBufVariantDrag)return;
		m_pBufVariantDrag->CopyFrom(pBuf);
	}
	CBaseView::SetVariantDragLine(pBuf);
}

void CStereoView::SetConstDragLine(const GrBuffer *pBuf)
{
	if( pBuf==m_pBufConstDrag ) return;	
	if( m_pBufConstDrag && m_pBufConstDrag!=pBuf )delete m_pBufConstDrag;
	m_pBufConstDrag = NULL;
	
	GrBuffer buf;
	if( pBuf!=NULL && m_nDragLineWid>1 )
	{		
		buf.CopyFrom(pBuf);
		buf.SetAllLineWidth(FALSE,m_nDragLineWid);
		pBuf = &buf;
	}

	if( pBuf )
	{
		m_pBufConstDrag = new GrBuffer;
		if( !m_pBufConstDrag )return;		
		m_pBufConstDrag->CopyFrom(pBuf);	
	}	
	CBaseView::SetConstDragLine(pBuf);

	m_constDragL.SetObjMark(HANDLE_CONSTDRAG,m_bShowKeyPoint);
	m_constDragR.SetObjMark(HANDLE_CONSTDRAG,m_bShowKeyPoint);
	
	m_addconstDragL.SetObjMark(HANDLE_CONSTDRAG,m_bShowKeyPoint);
	m_addconstDragR.SetObjMark(HANDLE_CONSTDRAG,m_bShowKeyPoint);
}


void CStereoView::UpdateSelDrawing(BOOL bAlways)
{
	CBaseView::UpdateSelDrawing(bAlways);
}

void CStereoView::UpdateVariantDragLine()
{	
	CBaseView::UpdateVariantDragLine();
	return;
}

void CStereoView::UpdateSnapDrawing()
{
	//draw snap tip
	{
		CRect rect;
		GetClientRect(&rect);

		GrBuffer2d buf;

		if( m_itemCurSnap.IsValid() && rect.Width()>50 && rect.Height()>20 )
		{
			CRect rcl(0,rect.bottom-20,60,rect.bottom);

			buf.BeginPolygon(RGB(255,255,0));

			buf.MoveTo(&PT_2D(rcl.left,rcl.top));
			buf.LineTo(&PT_2D(rcl.right-1,rcl.top));
			buf.LineTo(&PT_2D(rcl.right-1,rcl.bottom-1));
			buf.LineTo(&PT_2D(rcl.left,rcl.bottom-1));
			buf.LineTo(&PT_2D(rcl.left,rcl.top));

			buf.End(FALSE);

			CString tip;
			switch(m_itemCurSnap.nSnapMode) 
			{
			case CSnap::modeKeyPoint:	tip.LoadString(IDS_SNAPITEM_KEY);	break;
			case CSnap::modeNearPoint:	tip.LoadString(IDS_SNAPITEM_NEAREST);	break;
			case CSnap::modeMidPoint:	tip.LoadString(IDS_SNAPITEM_MID);	break;
			case CSnap::modeIntersect:	tip.LoadString(IDS_SNAPITEM_INTER);	break;
			case CSnap::modePerpPoint:	tip.LoadString(IDS_SNAPITEM_PERP);	break;
			case CSnap::modeCenterPoint:tip.LoadString(IDS_SNAPITEM_CENTER);	break;
			case CSnap::modeTangPoint:	tip.LoadString(IDS_SNAPITEM_TANG);	break;
			case CSnap::modeGrid:		tip.LoadString(IDS_SNAPITEM_GRID);	break;
			case CSnap::modeEndPoint:	tip.LoadString(IDS_SNAPITEM_END);	break;
			default:;
			}
			TextSettings settings;
			settings.fHeight = 12.0f;
			settings.nAlignment = TAH_MID|TAV_MID;
			_tcscpy(settings.tcFaceName,StrFromResID(IDS_MD_HEITI));

			buf.Text(0,&PT_2D(rcl.CenterPoint().x,rcl.CenterPoint().y),tip,&settings,FALSE,FALSE
				/*	"黑体",12,FALSE,TAH_MID|TAV_MID*/);

			m_snapTipLayL.SetBuffer(&buf);
			m_snapTipLayR.SetBuffer(&buf);
		}
		else
		{
			m_snapTipLayL.SetBuffer((GrBuffer2d*)NULL);
			m_snapTipLayR.SetBuffer((GrBuffer2d*)NULL);
		}
	}
	{
		CBaseView::UpdateSnapDrawing();
	}
}


void CStereoView::HiliteObject(CFeature* pFtr)
{
	{
		CBaseView::HiliteObject(pFtr);
	}
}

void CStereoView::UnHiliteObject(CFeature* pFtr)
{
	{
		CBaseView::UnHiliteObject(pFtr);
	}
}


void CStereoView::DelObjfromVectLay(LONG_PTR handle, CVectorLayer *pLL, CVectorLayer *pLR)
{
// 	if( !pLL )pLL = &m_vectLayL;
// 	if( !pLR )pLR = &m_vectLayR;
// 	
// 	pLL->DelObj(handle);
// 	pLR->DelObj(handle);
	CBaseView::DelObjfromVectLay(handle,pLL,pLR);
}

CSize CStereoView::GetDimension(BOOL bLeft)
{
	if( bLeft )
		return m_ImgLayLeft.GetVirtualImageSize();
	return m_ImgLayRight.GetVirtualImageSize();
}


// flag: 1, the active stereo view; 2, the first stereo view; 
// 0, if there is an active stereo view, then return it, or return the first stereo view;
CStereoView *GetStereoView(int flag)
{
	CFrameWnd *pChild = gpMainWnd->MDIGetActive();
	if( !pChild )return NULL;
	
	CView *pView = pChild->GetActiveView();

	if( pView && !pView->IsKindOf(RUNTIME_CLASS(CStereoView)) )
		pView = NULL;

	if( flag==1 )
		return (CStereoView*)pView;

	if( flag==0 && pView!=NULL )
		return (CStereoView*)pView;

	if( flag==2 || flag==0 )
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
							return (CStereoView*)pView;
						}
						
					}
				}
			}
			
		}
	}

	return NULL;
}


//by shy
void CStereoView::StereoMove()
{
	//by shy
	CView *pView = GetStereoView(0);	
	if(!pView||pView!=this) return;

	PT_4D pts[4],pts1[4];
	PT_3D pts2[4];
	memset(pts2,0,sizeof(PT_3D)*4);
	CRect rcClient;
	GetClientRect(&rcClient);

	double zoomRate=GetZoomRate();
	double width =rcClient.Width()/m_gsa.lfGKX/zoomRate;
	double height = rcClient.Height()/m_gsa.lfGKY/zoomRate;

	if( 0 )
	{
		int sx=0, ex=0, sy=0, ey=0;
		
		sx = rcClient.left;
		ex = rcClient.right;
		sy = rcClient.top;
		ey = rcClient.bottom;
		
		pts[0].x = pts[3].x = sx;			
		pts[1].x = pts[2].x = ex;
		pts[0].y = pts[1].y = sy;
		pts[2].y = pts[3].y = ey;
		
		pts[0].z = pts[3].z = pts[0].x;			
		pts[1].z = pts[2].z = pts[1].x;
		pts[0].yr= pts[1].yr= pts[0].y;
		pts[2].yr= pts[3].yr= pts[2].y;
		
		//计算相应的大地坐标
		for ( int i=0;i<4; i++)
		{
		//	ClientToGround(pts+i,pts1+i);
		 	ClientToImage(pts+i,pts1+i);

			pts2[i].x = pts1[i].x * m_trA[0] + pts1[i].y * m_trA[1] + m_trA[2];
			pts2[i].y = pts1[i].x * m_trB[0] + pts1[i].y * m_trB[1] + m_trB[2];
			pts2[i].z = 0;
		}

		Envelope e;
		e.CreateFromPts(pts2,4);

		width = (e.m_xh-e.m_xl);
		height = (e.m_yh-e.m_yl);
	}

	pts2[0].x=m_gCurPoint.x-width/2;
	pts2[0].y=m_gCurPoint.y+height/2;
	pts2[1].x=m_gCurPoint.x+width/2;
	pts2[1].y=m_gCurPoint.y+height/2;
	pts2[2].x=m_gCurPoint.x+width/2;
	pts2[2].y=m_gCurPoint.y-height/2;
	pts2[3].x=m_gCurPoint.x-width/2;
	pts2[3].y=m_gCurPoint.y-height/2;
	pts2[0].z=pts2[1].z=pts2[2].z=pts2[3].z=m_gCurPoint.z;
	
	//直接调用，以取得更好的速度
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	if( pFrame )
	{
		if(!m_bSysCross)
		{
			width = 50;
			height = 50;
		}
		pFrame->m_wndProjectView.GetToler((WPARAM)&width,(LPARAM)&height);
		pFrame->m_wndProjectView.RefreshCurrentStereoWin(0,(LPARAM)pts2);
	}

	//AfxGetMainWnd()->SendMessage(FCCM_GETTOLER,(WPARAM)&width,(LPARAM)&height);
	//AfxGetMainWnd()->SendMessage(FCCM_REFRESHCURRENTSTEREOWIN,0,(LPARAM)pts2);

}



void CStereoView::OnTimer(UINT_PTR nIDEvent)
{
	CBaseView::OnTimer(nIDEvent);

	if (nIDEvent == TIMERID_AUTOMOVEIMAGE)
	{
		BOOL bIsDrawCurveOrSurface = FALSE;
		CCommand *pCmd = GetDocument()->GetCurrentCommand();
		if (pCmd!=NULL && (pCmd->IsKindOf(RUNTIME_CLASS(CDrawCurveCommand)) || pCmd->IsKindOf(RUNTIME_CLASS(CDrawSurfaceCommand))))
		{
			if (IsProcProcessing(pCmd))
			{
				bIsDrawCurveOrSurface = TRUE;
			}
			
		}
		if (bIsDrawCurveOrSurface && m_bZoomWithCurrentMouse && m_nInnerCmd == 0)
		{
			CPoint point,pt;
			GetCursorPos(&pt);
			point = pt;
			ScreenToClient(&point);

			CRect cRect, rect;
			GetClientRect( &cRect );

			rect = cRect;
			
			CSize szClt = rect.Size();	
			
			int nHPage = szClt.cx, nVPage = szClt.cy;
			
			rect.DeflateRect(nHPage/6,nVPage/6);
			
			if (cRect.PtInRect(point) && !rect.PtInRect(point))
			{
				int dx = 0, dy = 0;
				float xWidth = (cRect.right - rect.right) / 20;
				float yWidth = (cRect.bottom - rect.bottom) / 20;
				if (point.x > rect.right)
				{
					float xLevel = (point.x - rect.right)/xWidth;
					dx = -xWidth*xLevel/2;
				}
				else if (point.x < rect.left)
				{
					float xLevel = (rect.left - point.x)/xWidth;
					dx = xWidth*xLevel/2;
				}
				else if (point.y > rect.bottom)
				{
					float yLevel = (point.y - rect.bottom)/yWidth;
					dy = -yWidth*yLevel/2;
				}
				else if (point.y < rect.top)
				{
					float yLevel = (rect.top - point.y)/yWidth;
					dy = yWidth*yLevel/2;
				}
				
				Scroll(dx,dy,0,0);
				
				BOOL bSave = m_bMoveFromSys;
				m_bMoveFromSys = FALSE;
				
				OnMouseMove(0,point);
				
				m_bMoveFromSys = bSave;
				
				float new_crsx,new_crsy,new_crsz;
				
				if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
				{					
					new_crsx = m_cCur3DMousePoint.x;
					new_crsy = m_cCur3DMousePoint.y;
					new_crsz = m_cCur3DMousePoint.z;
				}
				else
				{					
					new_crsx = m_cCurMousePoint.x;
					new_crsy = m_cCurMousePoint.y;
					new_crsz = m_cCurMousePoint.z;
				}
				
				if( m_dwModeCoord&CROSS_MODE_SHARE )
				{
					if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
					{
						m_gCurMousePoint = m_gCur3DMousePoint;
						m_cCurMousePoint = m_cCur3DMousePoint;
					}	
					else
					{
						m_gCur3DMousePoint = m_gCurMousePoint;
						m_cCur3DMousePoint = m_cCurMousePoint;
					}
					
				}
				
				SetCursorPos(pt.x,pt.y);			

				SetCrossPos(new_crsx, new_crsy, new_crsz);
				UpdateCross();

				Invalidate(FALSE);

			}
		}
	}
}


// void CStereoView::OnShowKeyPoint()
// {
// 	m_bShowKeyPoint = !m_bShowKeyPoint;
//  
// 	AfxGetApp()->WriteProfileInt(m_strRegSection,"DisplayKeyPoint",m_bShowKeyPoint);
// 
// 	{
// 		m_markLayL.ClearAll();
// 		m_markLayR.ClearAll();
// 
// 		m_markLayL.SetMark2Width(2);
// 		m_markLayR.SetMark2Width(2);
// 		m_markLayL.SetAllObjsMark2(m_bShowKeyPoint);
// 		m_markLayR.SetAllObjsMark2(m_bShowKeyPoint);
// 	
// 	}
// 
// 	Invalidate(FALSE);
// }


// void CStereoView::OnUpdateShowKeyPoint(CCmdUI* pCmdUI)
// {
// 	pCmdUI->Enable(TRUE);
// 	pCmdUI->SetCheck(m_bShowKeyPoint);
// }

/*
void CStereoView::On3DMouseDriveCross()
{
	BOOL bMode = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,FALSE);
	bMode = !bMode;
	AfxGetApp()->WriteProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,bMode);

// 	AfxUnlinkCallback(this,(PFUNCALLBACK)OnDeviceMove);
// 	AfxUnlinkCallback(this,(PFUNCALLBACK)OnDeviceMove2);
// 
// 	if( bMode )
// 	{
// 		AfxLinkCallback(WM_DEVICE_MOVE,this,(PFUNCALLBACK)OnDeviceMove2);
// 	}
// 	else
// 	{
// 		AfxLinkCallback(WM_DEVICE_MOVE,this,(PFUNCALLBACK)OnDeviceMove);
// 	}
}


void CStereoView::OnUpdate3DMouseDriveCross(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);

	BOOL bMode = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,FALSE);
	pCmdUI->SetCheck(bMode);
}
*/

PT_3D CStereoView::GetCrossPoint()
{
	if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
		return m_gCur3DMousePoint;

	return m_gCurPoint;
}


CCoordWnd CStereoView::GetCoordWnd()
{
	m_ViewCS.CreateWnd(m_hWnd);
	double m[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	m_SearchCS.Create44Matrix(m);

	PT_3D pt = m_gCurMousePoint;
	if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
		pt = m_gCur3DMousePoint;

	return CCoordWnd(&m_SearchCS,&m_ViewCS,m_gCurPoint,pt,TRUE);
}

void CStereoView::OnWsAsstereo() 
{
	PT_3D pt3ds[4];
	memset(pt3ds,0,sizeof(pt3ds));
	GetStereoBound(pt3ds);

	if(GraphAPI::GIsClockwise(pt3ds,4)==-1 )
	{
		AfxMessageBox(IDS_DOC_BOUNDINVALID);			
	}
	else
	{
		Envelope e;
		e.CreateFromPts(pt3ds,4);
		GetDocument()->SetModifiedFlag();
		GetDocument()->GetDlgDataSource()->GetBound(NULL,&(e.m_zl),&(e.m_zh));
//		GetDocument()->SetBound(e);
		GetDocument()->SetBound(pt3ds,e.m_zl,e.m_zh);
	}
}

void CStereoView::OnUpdateStereoFullscreen(CCmdUI* pCmdUI) 
{
	//为何不成功？
	CWnd *pWnd = GetParentFrame();
	if( !pWnd||!pWnd->IsKindOf(RUNTIME_CLASS(CStereoFrame)) )
	{
		pCmdUI->Enable(FALSE);	
	}
	else
	{
		pCmdUI->SetCheck(((CStereoFrame*)pWnd)->IsFullScreen());
	}
}

void CStereoView::OnStereoNohatch() 
{
	m_pContext->m_bNoHatch = !m_pContext->m_bNoHatch;
	m_pContext->GetLeftContext()->m_bNoHatch = m_pContext->m_bNoHatch; 
	m_pContext->GetRightContext()->m_bNoHatch = m_pContext->m_bNoHatch; 
	
	RefreshDrawingLayers();
	Invalidate(FALSE);
}

void CStereoView::OnStereoReverseDisplayorder() 
{
	m_bDisplayOrder = !m_bDisplayOrder;

	for( int i=0; i<m_laymgrLeft.GetDrawingLayerCount(); i++)
	{
		CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
		if (pLayer && pLayer->IsKindOf(RUNTIME_CLASS(CVectorLayer)))
		{
			((CVectorLayer*)pLayer)->ReverseDisplayorder();
		}
		
	}
	
	for (i=0; i<m_laymgrRight.GetDrawingLayerCount(); i++)
	{
		CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
		if (pLayer && pLayer->IsKindOf(RUNTIME_CLASS(CVectorLayer)))
		{
			((CVectorLayer*)pLayer)->ReverseDisplayorder();
		}
		
	}
	
	RefreshDrawingLayers();
	Invalidate(FALSE);
}

void CStereoView::OnUpdateStereoViewoverlay(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewVector);
}


BOOL CStereoView::RealtimeConvertCoordLeft(double x0, double y0, double &x1, double &y1, BOOL bOrder)
{
	if( m_pConvert==NULL )
		return FALSE;
	
	if( bOrder )
	{
		Coordinate c1,c2;
		c1.lx = x0; c1.ly = y0;
		c1.rx = 0; c1.ry = 0;
		c1.iType = SCANNER; c2.iType = EPIPOLAR;
		if( !m_pConvert->Convert(c1,c2) )
			return FALSE;
		
		x1 = c2.lx; y1 = c2.ly;
	}
	else
	{
		Coordinate c1,c2;
		c1.lx = x0; c1.ly = y0;
		c1.rx = 0; c1.ry = 0;
		c1.iType = EPIPOLAR; c2.iType = SCANNER;
		if( !m_pConvert->Convert(c1,c2) )
			return FALSE;
		
		x1 = c2.lx; y1 = c2.ly;
	}
	
	return TRUE;
}


BOOL CStereoView::RealtimeConvertCoordRight(double x0, double y0, double &x1, double &y1, BOOL bOrder)
{
	if( m_pConvert==NULL )
		return FALSE;
	
	if( bOrder )
	{
		Coordinate c1,c2;
		c1.rx = x0; c1.ry = y0;
		c1.lx = 0; c1.ly = 0;
		c1.iType = SCANNER; c2.iType = EPIPOLAR;
		if( !m_pConvert->Convert(c1,c2) )
			return FALSE;
		
		x1 = c2.rx; y1 = c2.ry;
	}
	else
	{
		Coordinate c1,c2;
		c1.rx = x0; c1.ry = y0;
		c1.lx = 0; c1.ly = 0;
		c1.iType = EPIPOLAR; c2.iType = SCANNER;
		if( !m_pConvert->Convert(c1,c2) )
			return FALSE;
		
		x1 = c2.rx; y1 = c2.ry;
	}
	
	return TRUE;
}

void CStereoView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	if( m_nImageType==EPIPOLAR && m_bStereoMidButton )
	{		
		if( (m_dwModeCoord&CROSS_MODE_HEIGHT)==0 )
		{
			DWORD save = m_dwModeCoord;
			m_dwModeCoord |= CROSS_MODE_AUTO;
			m_ptLastMouse = CPoint(-1,-1);
			OnMouseMove(0,point);
			m_dwModeCoord = save;
		}
	}
	CBaseView::OnMButtonDown(nFlags, point);
}




void CStereoView::OnMouseNoHeiMode()
{
	m_bMouseNoHeiMode = !m_bMouseNoHeiMode;
	AfxGetApp()->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_MOUSENOHEIMODE,m_bMouseNoHeiMode);
}



void CStereoView::OnUpdateMouseNoHeiMode(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bMouseNoHeiMode);
}

void CStereoView::OnRefresh()
{
	AdjustCellScale();
	CSceneView::OnRefresh();
}

void CStereoView::AdjustViewWithCurrentMouse()
{
	if (m_bZoomWithCurrentMouse)
	{
		int img_dx=0, img_dy=0, img_dz=0, img_dq=0;
		
		CPoint centerPt;
		
		CRect rect;
		GetClientRect( &rect );
		centerPt = rect.CenterPoint();
		
		PT_4D cCurMousePt;
		if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
		{
			cCurMousePt = m_cCur3DMousePoint;
		}
		else
		{
			cCurMousePt = m_cCurMousePoint;
		}
		
		// 更新客户坐标
		float cur_crsx,cur_crsy,cur_crsz,new_crsx,new_crsy,new_crsz;
		
		new_crsx = centerPt.x; 
		new_crsy = centerPt.y;
		new_crsz = centerPt.x;		
		
		img_dx = PIXEL(new_crsx)-PIXEL(cCurMousePt.x); 
		img_dy = PIXEL(new_crsy)-PIXEL(cCurMousePt.y);
		img_dz = PIXEL(new_crsz)-PIXEL(cCurMousePt.z)-img_dx;
		img_dq = PIXEL(new_crsy)-PIXEL(cCurMousePt.yr)-img_dy;
		
		//滚动影像
		if( img_dx || img_dy || img_dz || img_dq )
		{
			Scroll(img_dx,img_dy,img_dz,img_dq);
			
			cCurMousePt.x += img_dx;
			cCurMousePt.y += img_dy;
			cCurMousePt.z += (img_dz+img_dx);
			cCurMousePt.yr += (img_dq+img_dy);
			
			if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
			{
				m_cCur3DMousePoint = cCurMousePt;
			}
			else
			{
				m_cCurMousePoint = cCurMousePt;
			}
		}
		else
		{
			UpdateCross();
		}
		
		if( m_dwModeCoord&CROSS_MODE_SHARE )
		{
			if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
			{
				m_gCurMousePoint = m_gCur3DMousePoint;
				m_cCurMousePoint = m_cCur3DMousePoint;
			}	
			else
			{
				m_gCur3DMousePoint = m_gCurMousePoint;
				m_cCur3DMousePoint = m_cCurMousePoint;
			}
			
		}
		
		CPoint point, point2(PIXEL(new_crsx),PIXEL(new_crsy));
		::GetCursorPos(&point);
		
		ClientToScreen(&point2);
		if( point!=point2 )
		{
			::SetCursorPos(point2.x,point2.y);
		}
		
		SetCrossPos(new_crsx, new_crsy, new_crsz);
		UpdateCross();
		
		Invalidate(FALSE);
	}
}

void CStereoView::OnZoomIn()
{
	AdjustCellScale();
	//CSceneView::OnZoomIn();

// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_ZOOMIN;
	OnPreInnerCmd(IDC_BUTTON_ZOOMIN);
	
	CPoint pt;
	if (m_bZoomWithCurrentMouse)
	{
		if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
		{
			pt = CPoint(m_cCur3DMousePoint.x,m_cCur3DMousePoint.y);
		}
		else
		{
			pt = CPoint(m_cCurMousePoint.x,m_cCurMousePoint.y);
		}
	}
	else
	{
		CRect rect;
		GetClientRect( &rect );
		
		if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit )
			rect.right = rect.right/2;
		
		pt = rect.CenterPoint();
	}
	
	ZoomNext(pt,1,FALSE);
	
	AdjustViewWithCurrentMouse();
	
	OnPostInnerCmd();
//	m_nInnerCmd = save;
	StereoMove();
	
	UpdateWindow_ForceDraw();
}

void CStereoView::OnZoomOut()
{	
	//CSceneView::OnZoomOut();

	OnPreInnerCmd(IDC_BUTTON_ZOOMOUT);
	
	CPoint pt;
	if (m_bZoomWithCurrentMouse)
	{
		if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
		{
			pt = CPoint(m_cCur3DMousePoint.x,m_cCur3DMousePoint.y);
		}
		else
		{
			pt = CPoint(m_cCurMousePoint.x,m_cCurMousePoint.y);
		}
	}
	else
	{
		CRect rect;
		GetClientRect( &rect );
		
		if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit )
			rect.right = rect.right/2;
		
		pt = rect.CenterPoint();
	}
	
	ZoomNext(pt,0,FALSE);

	AdjustViewWithCurrentMouse();
	
	OnPostInnerCmd();
	StereoMove();
	AdjustCellScale();

	UpdateWindow_ForceDraw();
}

void CStereoView::SetStereoMagnity(double change,UINT Mode,BOOL bToCenter /* = TRUE */)
{
	AdjustCellScale();
	//CSceneView::OnZoomIn();

	OnPreInnerCmd(Mode);
	
	CPoint pt;
	CRect rect;
	GetClientRect( &rect );
	
	if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit )
		rect.right = rect.right/2;
	
	pt = rect.CenterPoint();
	
	if (change != -1)
	{
		float oldchange = GetZoomRate();
		ZoomCustom(pt,change/oldchange,bToCenter);
		UpdateComboZoom();
	}

	
	AdjustViewWithCurrentMouse();
	
	OnPostInnerCmd();
	StereoMove();
	
	UpdateWindow_ForceDraw();
}


void CStereoView::OnMagnifyHalf()
{
	SetStereoMagnity(0.5,IDC_BUTTON_ZOOMIN,FALSE);
}
	
void CStereoView::OnMagnifyOne()
{
	SetStereoMagnity(1.0,IDC_BUTTON_ZOOMOUT,FALSE);
}

void CStereoView::OnMagnifyOneAndHalf()
{
	SetStereoMagnity(1.5,IDC_BUTTON_ZOOMOUT,FALSE);
}

void CStereoView::OnMagnifyTwo()
{
	SetStereoMagnity(2,IDC_BUTTON_ZOOMOUT,FALSE);	
}

void CStereoView::OnMagnifyTwoAndHalf()
{
	SetStereoMagnity(2.5,IDC_BUTTON_ZOOMOUT,FALSE);	
}

void CStereoView::OnMagnifyThree()
{
	SetStereoMagnity(3.0,IDC_BUTTON_ZOOMOUT,FALSE);
}	
	

void CStereoView::AdjustCellScale()
{
	int		i;
	double	xSrc[4] ,ySrc[4] ;
	double	xDest[4],yDest[4];
	
	PT_4D pts[4],pts1[4];
	PT_3D pts2[4];
	CSize	size = GetDimension();
	
	CRect rcClient;
	GetClientRect(&rcClient);
	int sx=0, ex=0, sy=0, ey=0;
	
	sx = rcClient.left;
	ex = rcClient.right;
	sy = rcClient.top;
	ey = rcClient.bottom;
	
	pts[0].x = pts[3].x = sx;			
	pts[1].x = pts[2].x = ex;
	pts[0].y = pts[1].y = sy;
	pts[2].y = pts[3].y = ey;
	
	pts[0].z = pts[3].z = pts[0].x;			
	pts[1].z = pts[2].z = pts[1].x;
	pts[0].yr= pts[1].yr= pts[0].y;
	pts[2].yr= pts[3].yr= pts[2].y;
	
	//计算相应的大地坐标
	for ( i=0;i<4; i++)
	{
		ClientToImage(pts+i,pts1+i);
		ImageToGround(pts1+i,pts2+i);
		
		xSrc[i] = pts1[i].x; ySrc[i] = pts1[i].y;
		xDest[i] = pts2[i].x; yDest[i] = pts2[i].y;
	}
	
	//求出仿射关系
	//	CalcAffineParams( xSrc,ySrc,xDest,yDest,4,m_trA,m_trB );
	
	//归一化
	double k1, k2;
	k1 = DIST_2DPT(pts1[0],pts1[1])/DIST_2DPT(pts2[0],pts2[1]);
	k2 = DIST_2DPT(pts1[2],pts1[3])/DIST_2DPT(pts2[2],pts2[3]);
	
	m_gsa.lfGKX = sqrt(k1*k2);
	
	k1 = DIST_2DPT(pts1[0],pts1[3])/DIST_2DPT(pts2[0],pts2[3]);
	k2 = DIST_2DPT(pts1[2],pts1[1])/DIST_2DPT(pts2[2],pts2[1]);
	m_gsa.lfGKY = sqrt(k1*k2);

	m_gsa.lfGKX = m_gsa.lfGKY = sqrt(fabs(m_gsa.lfGKX*m_gsa.lfGKY));
	
	m_gsa.lfAngle = GraphAPI::GGetAngle(pts2[0].x,pts2[0].y,pts2[1].x,pts2[1].y);
	
	m_pContext->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->m_fDrawCellKY = m_gsa.lfGKY;
	
	m_pContext->GetLeftContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;
	
	m_pContext->GetRightContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->GetRightContext()->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->GetRightContext()->m_fDrawCellKY = m_gsa.lfGKY;

}


BOOL CStereoView::StereoMaybeFlicker()
{
	CDocument *pDoc = GetDocument();
	if( pDoc==NULL )return FALSE;

	POSITION pos = pDoc->GetFirstViewPosition();
	CView *pView = pDoc->GetNextView(pos);

	if( pView==NULL )return FALSE;

	CRect rcWnd1, rcWnd2;
	GetWindowRect(rcWnd1);
	pView->GetWindowRect(rcWnd2);

	//立体、矢量窗口的相交区域比较大
	CRect rcAnd = (rcWnd1&rcWnd2);
	if( rcAnd.Width()*rcAnd.Height()>0.25*rcWnd2.Width()*rcWnd2.Height() )
	{
		//如果矢量窗口是激活的，也用户应该会不关注闪烁
		CMDIFrameWndEx *pMain = (CMDIFrameWndEx*)AfxGetMainWnd();
		CMDIChildWnd *pChild = pMain->MDIGetActive();
		CView *pView2 = pChild->GetActiveView();
		if( pView==pView2 )
			return FALSE;

		return TRUE;
	}

	return FALSE;
}



void CStereoView::PushViewPos()
{
	CBaseView::PushViewPos();
	
	CRect rcClient;
	GetClientRect(rcClient);
	PT_4D pt4d(rcClient.CenterPoint().x,rcClient.CenterPoint().y,rcClient.CenterPoint().x,rcClient.CenterPoint().y);
	PT_3D pt3d;
	ClientToGround(&pt4d,&pt3d);
	pt3d.z = m_gCurPoint.z;
	
	m_arrPViewPos.GetAt(m_nCurViewPos)->pt = pt3d;
}


void CStereoView::ActiveViewPos(ViewPos* pos)
{
	CBaseView::ActiveViewPos(pos);
	
	DriveToXyz(&pos->pt,m_nImageType==EPIPOLAR?CROSS_MODE_SHARE:CROSS_MODE_HEIGHT);
}

void CStereoView::OnCtrlPtsBound()
{
	CoreObject& co = m_pConvert->coreobj;
	if( co.ctrl.iNum<3 )
	{
		AfxMessageBox(IDS_TOOFEW_CTRPTS);
		return;
	}
	
	CSize sz1 = GetDimension(TRUE), sz2 = GetDimension(FALSE);
	CSize sz11(sz1.cx*0.01, sz1.cy*0.01), sz12(sz1.cx*0.99, sz1.cy*0.99);
	CSize sz21(sz2.cx*0.01, sz2.cy*0.01), sz22(sz2.cx*0.99, sz2.cy*0.99);
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	PT_3DEX expt;
	
	for( int i=0; i<co.ctrl.iNum; i++)
	{
		expt = PT_3DEX(co.ctrl.x[i],co.ctrl.y[i],co.ctrl.z[i],penLine);
		
		Coordinate coord1,coord2; 
		coord1.x=expt.x; coord1.y=expt.y; 
		coord1.z=expt.z;
		coord1.iType=GROUND; coord2.iType=m_nImageType;
		if( Convert(coord1,coord2) )
		{
			if( coord2.lx>sz11.cx && coord2.lx<sz12.cx && coord2.ly>sz11.cy && coord2.ly<sz12.cy &&
				coord2.rx>sz21.cx && coord2.rx<sz22.cx && coord2.ry>sz21.cy && coord2.ry<sz22.cy )
			{
				arrPts.Add(expt);
			}
		}
	}
	
	if( arrPts.GetSize()<3 )
	{
		AfxMessageBox(IDS_TOOFEW_CTRPTS);
		return;
	}
	
	CArray<PT_3DEX,PT_3DEX> arrPts2;
	arrPts2.SetSize(arrPts.GetSize()+1);
	int npt = GraphAPI::GGetHull2D(arrPts.GetData(),arrPts.GetSize(),arrPts2.GetData());
	
	CDlgDoc *pDoc = GetDocument();
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	CFtrLayer *pFtrLayer = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
	if( !pFtrLayer )
	{
		pFtrLayer = pDS->GetCurFtrLayer();
	}
	if( !pFtrLayer )
		return;
	
	CFeature *pFtr = pFtrLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOCURVE);
	if( !pFtr )
		return;
	
	pFtr->GetGeometry()->CreateShape(arrPts2.GetData(),npt);
	
	pDoc->AddObject(pFtr,pFtrLayer->GetID());
	
	CUndoFtrs undo(pDoc,StrFromResID(IDS_UNDO_CREATECTRLLINES));
	undo.AddNewFeature(FtrToHandle(pFtr));
	undo.Commit();
}


void GDI_DrawGrBuffer2d_2(HDC hdc, const GrBuffer2d *pBuf, CSize szDC)
{	
	C2DGDIDrawingContext dc;
	dc.CreateContext(hdc);
	dc.SetDCSize(szDC);
	dc.SetViewRect(CRect(0,0,szDC.cx,szDC.cy));
	
	CCoordSys cs;
	cs.Create33Matrix(NULL);
	dc.SetCoordSys(&cs);
	dc.SetBackColor(RGB(0,0,0)); 
	
	dc.BeginDrawing();	
	dc.DrawGrBuffer2d(&GrElementList((void*)pBuf,FALSE,FALSE),CDrawingContext::modeNormal);
	dc.EndDrawing();
}

typedef struct BMPBASE
{
	unsigned char* buf;
	int width_x;
	int height_y;
	RGBQUAD* palette;
	int deepth;

	BMPBASE()
	{
		buf = nullptr;
		width_x = 0;
		height_y = 0;
		deepth = 0;
		palette = nullptr;
	}
}Bmpbase;




void WriteTIFF(LPCTSTR tifPath, HBITMAP hBmp)
{
	BITMAP info;
	GetObject(hBmp,sizeof(info),&info);
	
	TIFF *tif = TIFFOpen(tifPath,"w");

	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, info.bmWidth);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, info.bmHeight);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

	TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_NONE);

	BYTE *pBuf = (BYTE*)info.bmBits;
	BYTE *p = new BYTE[info.bmWidthBytes];
	for( int j=0; j<info.bmHeight; j++)
	{
		memcpy(p,pBuf+j*info.bmWidthBytes,info.bmWidthBytes);
		BYTE *p1 = p, t;
		for(int i=0; i<info.bmWidthBytes; i+=3, p1+=3)
		{
			BYTE t = p1[0];
			p1[0] = p1[2];
			p1[2] = t;
		}
		TIFFWriteScanline(tif, p, j, 0);
	}

	delete[] p;

	TIFFClose(tif);
}


void WriteTIFF_baseBmp(LPCTSTR tifPath, Bmpbase &baseBmp)
{

	TIFF *tif = TIFFOpen(tifPath, "w");

	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, baseBmp.width_x);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, baseBmp.height_y);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

	TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_NONE);

	BYTE *pBuf = (BYTE*)baseBmp.buf;
	int nWidthBytes = (int)(baseBmp.width_x*baseBmp.deepth / 8 + 3) / 4 * 4;;
	BYTE *p = new BYTE[nWidthBytes];
	for (int j = 0; j < baseBmp.height_y; j++)
	{
		memcpy(p, pBuf + j*nWidthBytes, nWidthBytes);
		BYTE *p1 = p, t;
		for (int i = 0; i < nWidthBytes; i += 3, p1 += 3)
		{
			BYTE t = p1[0];
			p1[0] = p1[2];
			p1[2] = t;
		}
		TIFFWriteScanline(tif, p, j, 0);
	}

	delete[] p;

	TIFFClose(tif);
}


//行替换
void ReverseBmp(HBITMAP &hBmp)
{
	BITMAP info;
	if( !::GetObject(hBmp,sizeof(BITMAP),&info) )
		return;

	int linesize = info.bmWidthBytes;
	BYTE *p1 = (BYTE*)info.bmBits;
	BYTE *p2 = p1 + (info.bmHeight-1)*linesize;
	BYTE *pLine = new BYTE[linesize];
	memset(pLine,0,linesize);

	int num = (info.bmHeight>>1);
	for( int i=0; i<num; i++)
	{
		memcpy(pLine,p2,linesize);
		memcpy(p2,p1,linesize);
		memcpy(p1,pLine,linesize);

		p1 += linesize;
		p2 -= linesize;
	}

	delete[] pLine;
}


// bmp 格式图片旋转 支持任意角度 
Bmpbase &imrotate(unsigned char* imgbuf, int width, int height, int deep, RGBQUAD* CTable, double theta)
{
	Bmpbase bmp1;
	double lfdegree = theta;
	theta = (theta / 180)*3.1415;                  //角度转化为弧度


	//定义四个顶点坐标
	int x1 = -width / 2, y1 = height / 2;


	int x2 = width / 2, y2 = height / 2;


	int x3 = width / 2, y3 = -height / 2;


	int x4 = -width / 2, y4 = -height / 2;

	//旋转这四个顶点的坐标
	double new_x1 = cos(theta)*x1 + sin(theta)*y1;
	double new_y1 = -sin(theta)*x1 + cos(theta)*y1;


	double toldX = cos(theta)*new_x1 - sin(theta)*new_y1;
	double toldY = sin(theta)*new_x1 + cos(theta)*new_y1;

	double new_x2 = cos(theta)*x2 + sin(theta)*y2;
	double new_y2 = -sin(theta)*x2 + cos(theta)*y2;


	double new_x3 = cos(theta)*x3 + sin(theta)*y3;
	double new_y3 = -sin(theta)*x3 + cos(theta)*y3;


	double new_x4 = cos(theta)*x4 + sin(theta)*y4;
	double new_y4 = -sin(theta)*x4 + cos(theta)*y4;


	int new_width = fmax(abs(new_x1 - new_x3) + 0.5, abs(new_x2 - new_x4) + 0.5);
	bmp1.width_x = new_width;                     //新图像的宽度
	int new_height = fmax(abs(new_y1 - new_y3) + 0.5, abs(new_y2 - new_y4) + 0.5);
	bmp1.height_y = new_height;                   //新图像的高度
	int old_lineByte = (width*deep / 8 + 3) / 4 * 4;
	int lineByte = int((new_width*deep / 8 + 3) / 4) * 4;
	unsigned char* buf = new unsigned char[lineByte*new_height];
	for (int i = 0; i < lineByte*new_height; i++)
		*(buf + i) = 0;
	//每个像素所占 字节数 
	int nChannel = deep / 8;

	int nsize = lineByte / new_width;
	int oldsize = old_lineByte / width;

	//旋转任意角度
	int newX, newY, oldX, oldY, old_i, old_j;
	for (int i = 0; i < new_width; i++)
	{
		for (int j = 0; j < new_height; j++)
		{
			newX = i - new_width / 2;
			newY = j - new_height / 2;

			oldX = cos(theta)*newX - sin(theta)*newY;
			oldY = sin(theta)*newX + cos(theta)*newY;
			if (abs(oldX) <= width / 2 && abs(oldY) <= height / 2)
			{
				old_i = oldX + width / 2>0 ? (oldX + width / 2) : 0;
				old_j = oldY + height / 2>0 ? (oldY + height / 2) : 0;
				for (int k = 0; k < nChannel; k++)
				{
					memcpy(buf + j*lineByte + i*nChannel + k, imgbuf + old_j*old_lineByte + old_i*nChannel + k, 1);
				}

			}
		}
	}
	
	bmp1.buf = buf;
	bmp1.deepth = deep;
	bmp1.palette = CTable;
	return bmp1;
}


bool saveBmp(char* bmpName, unsigned char* imgBuf, int width, int height, int biBitCount, RGBQUAD* pColorTable)
{
	if (!imgBuf)
		return 0;

	//颜色表大小
	int colorTablesize = 0;
	if (biBitCount == 8)
		colorTablesize = 1024;

	//带存储图像每行字节数
	int lineByte = (width*biBitCount / 8 + 3) / 4 * 4;

	//以二进制写的方式打开文件
	FILE* fp = fopen(bmpName, "wb");
	if (fp == 0)
		return 0;
	//申请位图文件头结构变量，填写文件头信息
	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 0x4D42;      //bmp类型
	//bfSize是图像文件4个组成部分之和
	fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte*height;
	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;

	//bfOffBits是图像文件前3个部分所需空间之和
	fileHead.bfOffBits = 54 + colorTablesize;

	//写文件头进文件
	fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);

	//申请位图文件信息头结构变量，填写文件信息头信息
	BITMAPINFOHEADER infoHead;
	infoHead.biSize = 40;
	infoHead.biWidth = width;
	infoHead.biHeight = height;
	infoHead.biPlanes = 1;
	infoHead.biBitCount = biBitCount;
	infoHead.biCompression = 0;
	infoHead.biSizeImage = lineByte*height;
	infoHead.biXPelsPerMeter = 0;
	infoHead.biYPelsPerMeter = 0;
	infoHead.biClrUsed = 0;
	infoHead.biClrImportant = 0;
	fwrite(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);
	if (biBitCount == 8)
		fwrite(pColorTable, sizeof(RGBQUAD), 256, fp);


	//写位图数据进文件
	fwrite(imgBuf, height*lineByte, 1, fp);

	fclose(fp);
	return 1;

}



//旋转
HBITMAP & ReverseBmpByDegree(HBITMAP &hBmp, double lfDegree = 90)
{
	HBITMAP hRetBmp;
	DIBSECTION dibInfo;
	::GetObject(hBmp, sizeof(dibInfo), &dibInfo);
	BITMAPFILEHEADER fileInfo;
	BITMAPINFO		 bmpInfo;

	fileInfo.bfType = 0x4d42;
	fileInfo.bfSize = sizeof(fileInfo) + sizeof(bmpInfo) + dibInfo.dsBmih.biSizeImage;
	fileInfo.bfReserved1 = 0;
	fileInfo.bfReserved2 = 0;
	fileInfo.bfOffBits = sizeof(fileInfo) + sizeof(bmpInfo);

	memset(&bmpInfo, 0, sizeof(bmpInfo));
	memcpy(&bmpInfo.bmiHeader, &dibInfo.dsBmih, sizeof(dibInfo.dsBmih));

	BITMAP info;
	BITMAPINFOHEADER head;
	BITMAPFILEHEADER fhead;
	BITMAPINFO info_fo;
	BITMAP infoRet;
	if (!::GetObject(hBmp, sizeof(BITMAP), &info))
		return hRetBmp;

	head = bmpInfo.bmiHeader;
	Bmpbase bmp;
	Bmpbase bmp1;

	//获取图向宽、高、每个像素所占位数等信息
	bmp.width_x = head.biWidth;
	bmp.height_y = head.biHeight;
	bmp.deepth = head.biBitCount;
	
	//定义变量，计算图像每行像素所占字节数
	int lineByte = (bmp.width_x*bmp.deepth / 8 + 3) / 4 * 4;
	//灰度图有颜色表，且颜色表表项为256
	int linesize = info.bmWidthBytes;//每一行像素所在的byte数 列数
	bmp.palette = bmpInfo.bmiColors;

	//申请位图数据所需的空间，读入位图数据进入内存
	bmp.buf = new unsigned char[lineByte*bmp.height_y];
	memset(bmp.buf, 0, lineByte*bmp.height_y);
	memcpy(bmp.buf, (BYTE*)dibInfo.dsBm.bmBits, lineByte*bmp.height_y);

	//char* writePath1 = "E:\\bmp\\aaaaabbb.bmp";
	//saveBmp(writePath1, bmp.buf, bmp.width_x, bmp.height_y, bmp.deepth, bmp.palette);

	bmp1 = imrotate(bmp.buf, bmp.width_x, bmp.height_y, bmp.deepth, bmp.palette, lfDegree);
	//char* writePath = "E:\\bmp\\aaaaa.bmp";
	//saveBmp(writePath, bmp1.buf, bmp1.width_x, bmp1.height_y, bmp1.deepth, bmp1.palette);

	if (bmp.buf)
	{
		delete []bmp.buf;
		bmp.buf = NULL;
	}

	//返回一个新的HBITMAP  下面画文字需要这个
	int lineBytenew = (bmp1.width_x*bmp1.deepth / 8 + 3) / 4 * 4;

	hRetBmp = Create24BitDIB(bmp1.width_x, bmp1.height_y);
	DIBSECTION sect;
	if (::GetObject(hRetBmp, sizeof(sect), &sect) == 0)
		return hRetBmp;
	memcpy(sect.dsBm.bmBits, bmp1.buf, lineBytenew*bmp1.height_y);
	return hRetBmp;
}

//行替换
void ReverseBmpRow(HBITMAP hBmp)
{
	ReverseBmp( hBmp);
}

// 列替换
void ReverseBmpCol(HBITMAP hBmp)
{
	BITMAP info;
	if (!::GetObject(hBmp, sizeof(BITMAP), &info))
		return;

	int linesize = info.bmWidthBytes;
	BYTE *p1 = (BYTE*)info.bmBits;
	BYTE *p2 = p1 + linesize;
	BYTE *pLine = new BYTE[1];
	memset(pLine, 0, 1);

	int num = (info.bmHeight >> 1);
	int size = (info.bmWidthBytes >> 1);
	for (int i = 0; i < num; i++)
	{
		BYTE* p3 = p1;
		BYTE* p4 = p2;
		for (int j = 0; j < size;j++)
		{
			memcpy(pLine, p4, 1);
			memcpy(p4, p3, 1);
			memcpy(p3, pLine, 1);
			p3 += 1;
			p4 -= 1;
		}
		p1 += linesize;
		p2 += linesize;
	}

	delete[] pLine;
}



BOOL MakeCheckPtSample(LPCTSTR tifPath, LPCTSTR tipClipFile, LPCTSTR idtext, int x1, int y1, int imgWid, int imgHig, int crossWid, COLORREF crossColor,
	LPCTSTR fontName, int fontHei, COLORREF fontColor, LPCTSTR text,int imgRotationAngle)
{
	CImageRead image;
	if(!image.Load(tifPath))
		return FALSE;
	//记录原始长宽
	int nOldWid = imgWid;
	int nOldHig = imgHig;

	HBITMAP hBmp;
	HBITMAP hBmpRet;


	BOOL bReverse = TRUE; //数据置换
	if (imgRotationAngle == 0) // 0度
	{
		;
	}
	else if (imgRotationAngle == 1) // 90度
	{
		//宽高置换
		int ntmp = imgWid;
		imgWid = imgHig;
		imgHig = ntmp;	
	}
	else if (imgRotationAngle == 2)// 180 
	{
		//宽高不置换、数据置换
		
	}
	else if (imgRotationAngle == 3)// 270
	{
		//宽高置换、影像数据也置换
		//宽高置换
		int ntmp = imgWid;
		imgWid = imgHig;
		imgHig = ntmp;
	}


	hBmp = Create24BitDIB(imgWid, imgHig, TRUE, 0);
	image.ReadRectBmp(CRect(x1 - imgWid / 2, y1 - imgHig / 2, x1 - imgWid / 2 + imgWid, y1 - imgHig / 2 + imgHig), hBmp);
	//测试代码
//	WriteToFile(hBmp);
	//
	if (bReverse)
	{
		ReverseBmp(hBmp);		
	}

	HBITMAP hRetBmp;

	if (imgRotationAngle == 0)
	{
		hRetBmp = hBmp;
	}
	else if (imgRotationAngle == 1) // 90度 顺时针90 其实就是逆时针270
	{		
		hRetBmp = ReverseBmpByDegree(hBmp, 90);
	}
	else if (imgRotationAngle == 2)// 180 
	{
		hRetBmp = ReverseBmpByDegree(hBmp, 180);
	}
	else if (imgRotationAngle == 3)// 270
	{
		hRetBmp = ReverseBmpByDegree(hBmp, 270);
	}

	HDC hdc = CreateCompatibleDC(NULL);
	if(hdc)
	{
		HBITMAP hOldBmp = (HBITMAP)SelectObject(hdc, hRetBmp);
		GrBuffer2d buf;

		int dx = crossWid/2;

		int cur_x = nOldWid / 2, cur_y = nOldHig / 2;
		
		buf.BeginLineString(crossColor,1,FALSE);
		buf.MoveTo(&PT_2D(cur_x-dx,cur_y));
		buf.LineTo(&PT_2D(cur_x-dx+crossWid-1,cur_y));
		buf.MoveTo(&PT_2D(cur_x,cur_y-dx));
		buf.LineTo(&PT_2D(cur_x,cur_y-dx+crossWid-1));
		buf.End(FALSE);

		TextSettings ts;
		if(strlen(fontName)>0)
			strcpy(ts.tcFaceName,fontName);
		ts.fHeight = fontHei;
		ts.nAlignment = TAH_LEFT|TAV_TOP;

		CString text1;
		text1.Format("ID: %s",idtext);

		CString text2(text);
		if (!text2.IsEmpty())
		{
			text2.Format(IDS_MAKE_SAMPLE_ANNOTATION, text);
		}

	
		buf.Text(fontColor,&PT_2D(10,10),text1+"\r\n"+text2,&ts);

		GDI_DrawGrBuffer2d_2(hdc, &buf, CSize(imgWid, imgHig));

		SelectObject(hdc,hOldBmp);
	}

	if (bReverse)
	{
		ReverseBmp(hRetBmp);
	}

	WriteTIFF(tipClipFile, hRetBmp);

	DeleteObject(hRetBmp);

	DeleteObject(hBmp);
	return TRUE;
}


void CStereoView::OnMakeCheckPtSample()
{
	if(m_nImageType!=SCANNER)
	{
		AfxMessageBox(IDS_ONLY_ORIGINAL_STEREOPAIR);
		return;
	}

	CDlgMakeCheckPtSample dlg;
	dlg.IncreasePtID();
	
	if( dlg.DoModal()!=IDOK )
	{
		return;
	}

	CString imgID;
	CoreObject *pCore = &m_coreObj;
	for(int i=0; i<pCore->iStereoNum; i++)
	{
		if(pCore->stereo[i].sp.stereoID.CompareNoCase(m_strStereID)==0)
		{
			if(dlg.m_nImgSoure==0 )
				imgID = pCore->stereo[i].imageID[0];
			else
				imgID = pCore->stereo[i].imageID[1];
			break;
		}
	}

	CString imgPath = dlg.m_strImagePath;
	CString imgName;
	imgName.Format("%s_%s.tif",(LPCTSTR)dlg.m_strPtID,(LPCTSTR)imgID);

	Coordinate coord1, coord2;
	coord1.x=m_gCurPoint.x; coord1.y=m_gCurPoint.y;	coord1.z=m_gCurPoint.z;
	coord1.iType=GROUND; 
	coord2.iType=SCANNER;
	
	if(!Convert(coord1,coord2))
		return;

	CString srcimgName = m_ImgLayLeft.GetImgFileName();
	if(dlg.m_nImgSoure==1 )
		srcimgName = m_ImgLayRight.GetImgFileName();

	double x = coord2.lx, y = coord2.ly;
	if(dlg.m_nImgSoure==1 )
		x = coord2.rx, y = coord2.ry;

	MakeCheckPtSample(srcimgName,imgPath+"\\"+imgName,dlg.m_strPtID,x,y,dlg.m_nImageWidth,dlg.m_nImageHigh,
		dlg.m_nCrossWidth,dlg.m_clrCross,dlg.m_strFontName,dlg.m_nFontSize,dlg.m_clrFont,dlg.m_strNoteText,0);

	CString txtFile = imgPath+"\\"+imgName;
	txtFile = txtFile.Left(txtFile.GetLength()-4) + ".jpg.txt";
	FILE *fp = fopen(txtFile,"wt");
	if(fp)
	{
		fprintf(fp,"%s %.4f %.4f %.4f\r\n",(LPCTSTR)dlg.m_strPtID,m_gCurPoint.x,m_gCurPoint.y,m_gCurPoint.z);
		fclose(fp);
	}
}

void CStereoView::UpdateVisible()
{
	CDlgDoc *pDoc = GetDocument();
	if(!pDoc) return;
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if(!pDS) return;

	int nSum=0, i, j;
	for (i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer ||!pLayer->IsVisible()) 
			continue;

		nSum += pLayer->GetObjectCount();
	}

	GProgressStart(nSum);
	
	for (i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);		
		if( !pLayer ||!pLayer->IsVisible()) 
			continue;
		
		int nObjs = pLayer->GetObjectCount();
		for(int j=0; j<nObjs; j++)
		{
			GProgressStep();
			CFeature *pFtr = pLayer->GetObject(j,FILTERMODE_DELETED);
			if (!pFtr) continue;
			
			BOOL bVisible = pFtr->IsVisible();
			
			m_vectLayL.SetObjVisible((LONG_PTR)pFtr, bVisible);
			m_vectLayR.SetObjVisible((LONG_PTR)pFtr, bVisible);
		}
	}
	
	GProgressEnd();
}