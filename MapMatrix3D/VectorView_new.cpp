// VectorView_new.cpp: implementation of the CVectorView_new class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "glew.h"
#include "EditBaseDoc.h"
#include "VectorView_new.h"
#include "Feature.h"
#include "DrawLineDC.h"
#include "ExMessage.h"
#include "DlgCommand.h"
#include "DlgDataSource.h "
#include "RegDef.h "
#include "RegDef2.h "
#include "SymbolLib.h "
#include "DlgOverMutiImgLay.h "
#include "IO.H "
#include "DlgSetXYZ.h "
#include "VDBaseDef.h "
#include "GlobalFunc.h "
#include "MainFrm.h"
#include "DrawingDef.h"
#include "StereoView.h"
#include "DlgDisplayContoursSpecially.h "
#include "Functions_temp.h "
#include "SmartViewFunctions.h"
#include "tiff.h"
#include "tiffio.h"
#include "DlgExportRaster.h"
#include "DlgPrintOption.h "
#include "DlgOverMutiImgLay.h"
#include "PlotWChar.h"
#include "ChildFrm.h"
#include "mm3dPrj\OsgbView.h"
#include "mm3dPrj\myAbsOriDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REG_CMDCLASS(pWorker,id,clsname)  (pWorker)->RegisterCommand((id),clsname::Create)
#define PRINTINDENT   200

#define HANDLE_REFMARK		-11

extern void replace_all(std::string& s, std::string const & t, std::string const & w);
char* MBSCToCEF(const char* mbcsStr)
{
	wchar_t*  wideStr;
	char*   utf8Str;
	int   charLen;

	charLen = MultiByteToWideChar(CP_UTF8, 0, mbcsStr, -1, NULL, 0);
	charLen = strlen(mbcsStr) + 1;
	wideStr = (wchar_t*)malloc(sizeof(wchar_t)*charLen);
	MultiByteToWideChar(CP_ACP, 0, mbcsStr, -1, wideStr, charLen);

	charLen = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);

	utf8Str = (char*)malloc(charLen);

	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, charLen, NULL, NULL);

	free(wideStr);
	return utf8Str;
}
/////////////////////////////////////////////////////////////////////////////
// CVectorView_new

IMPLEMENT_DYNCREATE(CVectorView_new, CBaseView)

BEGIN_MESSAGE_MAP(CVectorView_new, CBaseView)
	//{{AFX_MSG_MAP(CVectorView_new)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
    ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
    ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_OVERLAY_RASTER, OnOverlayRaster)
	ON_COMMAND(ID_REFDEM, OnRefdem)
	ON_COMMAND(ID_UNREFDEM, OnUnrefdem)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_VIEW_ROTATEASSTEREO, OnViewRotateAsStereo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ROTATEASSTEREO, OnUpdateViewRotateAsStereo)
	ON_COMMAND(ID_STEREO_MOVETOCENTER, OnMoveCursorToCenter)
	ON_COMMAND(ID_TOOL_MANUALLOADVECT, OnManulLoadVect)
	ON_UPDATE_COMMAND_UI(ID_TOOL_MANUALLOADVECT, OnUpdateManulLoadVect)	
	ON_COMMAND(ID_WS_ASSTEREO, OnWsAsstereo)
	ON_COMMAND(ID_EXPORT_RASTER,OnExportRaster)
	ON_COMMAND(ID_WS_ASREFIMAGE,OnWsRefImage)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_DEVICE_LFOOT,OnDeviceLFootDown)
	ON_MESSAGE(WM_DEVICE_MOVE,OnDeviceMove)
	ON_MESSAGE(WM_DEVICE_RFOOT,OnDeviceRFootDown)
	ON_WM_MOUSELEAVE()
	ON_MESSAGE(ID_STEREO_MOVETOCENTER,OnMoveCursorToCenter2)
	// Standard printing commands
	ON_COMMAND(ID_SELECT, CBaseView::OnSelect)
	ON_UPDATE_COMMAND_UI(ID_SELECT, CBaseView::OnUpdateSelect)
	ON_COMMAND(ID_FILE_PRINT, /*CBaseView::*/OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, /*CBaseView::*/OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, /*CBaseView::*/OnFilePrintPreview)
	ON_COMMAND(ID_STEREO_SETXYZ,OnSetXYZ)
	ON_COMMAND(ID_RASTER_POSITION, OnMeasureImagePosition)
	ON_COMMAND(ID_DISPLAY_BYHEIGHT, OnDisplayByheight)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_BYHEIGHT, OnUpdateDisplayByheight)
	ON_COMMAND(ID_DISPLAY_FORCONTOUR, OnDisplayContoursSpecially)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_FORCONTOUR, OnUpdateDisplayContoursSpecially)
	ON_COMMAND(ID_VIEW_RESET,OnViewReset)
	ON_COMMAND(IDC_BUTTON_ROTATERASTER, OnRotateRaster)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_ROTATERASTER, OnUpdateRotateRaster)
	ON_COMMAND(IDC_BUTTON_VIEWOVERLAY, OnViewOverlay)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_VIEWOVERLAY, OnUpdateViewOverlay)
	ON_COMMAND(ID_STEREO_REFRESH,/*CSceneView::*/OnRefresh)
	ON_COMMAND(ID_VIEW_REFRESH,OnRefresh)
	ON_COMMAND(ID_STEREO_ZOOMIN,/*CSceneView::*/OnZoomIn)
	ON_COMMAND(ID_STEREO_ZOOMOUT,/*CSceneView::*/OnZoomOut)
	ON_MESSAGE(WM_HOTKEY,OnZoomHotKey)
	ON_COMMAND(ID_VIEW_KEYPOINT, OnViewKeypoint)
	ON_COMMAND(ID_VIEW_MANUALROTATE,CBaseView::OnCommandOne)
	ON_COMMAND(ID_REFFILE_MARK,OnMarkReffile)
	ON_UPDATE_COMMAND_UI(ID_REFFILE_MARK,OnUpdateMarkReffile)
	ON_COMMAND(ID_FILL_COLOR_TEMP, OnFillColor)
	ON_UPDATE_COMMAND_UI(ID_FILL_COLOR_TEMP, OnUpdateFillColor)
	ON_COMMAND(ID_SWITCH_MAP_GRID, OnShowMapGrid)
	ON_UPDATE_COMMAND_UI(ID_SWITCH_MAP_GRID, OnUpdateMapGrid)
	ON_COMMAND(ID_SWITCH_MAP_NAME, OnShowMapName)
	ON_UPDATE_COMMAND_UI(ID_SWITCH_MAP_NAME, OnUpdateMapName)
	ON_COMMAND(ID_SWITCH_LINE_WIDTH, OnShowLineWidth)
	ON_UPDATE_COMMAND_UI(ID_SWITCH_LINE_WIDTH,OnUpdateLineWidth)
	ON_COMMAND(ID_STEREO_VIEWOVERLAY, OnViewOverlay)
	ON_COMMAND(ID_OsgbView, OnOsgbview)
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_NCCALCSIZE()
//	ON_WM_NCPAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVectorView_new construction/destruction

CVectorViewCenterPoint::CVectorViewCenterPoint(CVectorView_new *p)
{
	pView = p;
	Save();
}


CVectorViewCenterPoint::~CVectorViewCenterPoint()
{
	Restore();
}

void CVectorViewCenterPoint::Save()
{
	if( !pView )return;

	//客户中心作为焦点，位置不变，这里先记录此位置
	CRect rcClient;
	pView->GetClientRect(&rcClient);
 	CPoint cpt = rcClient.CenterPoint();
	
	PT_4D fcpt;
	fcpt.x = cpt.x;  fcpt.y = cpt.y; fcpt.z = 0;
	pView->ClientToGround(&fcpt,&ptCenter);
}


void CVectorViewCenterPoint::Restore()
{
	if( !pView )return;
	pView->DriveToXyz(&ptCenter);
}

CVectorView_new::CVectorView_new()
{
	m_xgoff		= 0;
	m_ygoff     = 0;
//	m_lfAngle   = 0;

	m_bViewRotate = FALSE;
	m_bOverlayImgLay = FALSE;
	
	m_bVectLayCompress = TRUE;
	
	m_bStartLoadVect = FALSE;
	
	m_clrBound = gdef_clrBndVect;
	m_clrBack  = gdef_clrBackVect;
	strcpy(m_strRegSection,REGPATH_VIEW_VECT);
	
	memset(m_ptBounds,0,sizeof(m_ptBounds));
	
//	m_bShowKeyPoint = AfxGetApp()->GetProfileInt(m_strRegSection,"DisplayKeyPoint",FALSE);
	
	m_bDisplayByHeight = FALSE;

	m_bFillColor = FALSE;
	
	m_bDisplayContoursSpecially = FALSE;
	
	m_bViewVector = TRUE;
	
	m_nRasterAng = 0;

	m_bDriveOnlyStereo = TRUE;
	m_bDriveOnlyCursor = FALSE;
	
	m_bDisableMouseIn3DDraw = TRUE;
	m_b3DDraw = FALSE;
	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;
	m_ptLastMouse = CPoint(-1,-1);

	m_bViewBaseOnImage = FALSE;
	matrix_toIdentity(m_lfImgMatrix,4);
	matrix_toIdentity(m_lfImgRMatrix,4);

	m_pRealCS = NULL;
	m_pPrintContext = NULL;
	m_pTempCoordSys = NULL;
	m_pComCoordSys = NULL;

	m_bMarkReffile = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_MARKREFFILE,TRUE);

	m_bMouseDriveVector = FALSE;

	m_fPrintMarginLeft = 25;
	m_fPrintMarginRight = 25;
	m_fPrintMarginTop = 25;
	m_fPrintMarginBottom = 25;
	m_bImageZoomNotLinear = FALSE;
	m_fPrintExtension = 50;

	m_bUseAnyZoomrate = FALSE;

	m_ShowMapGrid = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,"m_ShowMapGrid", FALSE);
	m_ShowMapName = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,"m_ShowMapName", FALSE);
	mapgrid_showed = FALSE;
	m_fWid = 50;
	m_fHei = 50;
}

CVectorView_new::~CVectorView_new()
{
	if( m_pRealCS )delete m_pRealCS;
	if (m_pPrintContext) delete m_pPrintContext;
	if (m_pTempCoordSys) delete m_pTempCoordSys;	
	if (m_pComCoordSys) delete m_pComCoordSys;
}

BOOL CVectorView_new::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	/*ModifyStyleEx(WS_EX_CLIENTEDGE, NULL, SWP_DRAWFRAME);*/

	return CBaseView::PreCreateWindow(cs);
}


void CVectorView_new::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CVectorView_new printing
void CVectorView_new::OnFilePrint()
{
	BeginCheck41License

	CDlgPrintOption dlg;
	dlg.m_bOverImg = m_bOverlayImgLay;
	if (dlg.DoModal()!=IDOK)
	{
		return;
	}
	m_nPrintScope = dlg.m_nPrintScope;
	m_nPrintScale = dlg.m_nPrintScale;
	m_fPrintMarginLeft = dlg.m_fMarginLeft;
	m_fPrintMarginRight = dlg.m_fMarginRight;
	m_fPrintMarginTop = dlg.m_fMarginTop;
	m_fPrintMarginBottom = dlg.m_fMarginBottom;
	m_fPrintExtension = dlg.m_fExtension;
	GetClientRect(&m_rcTemp);
	CBaseView::OnFilePrint();

	EndCheck41License
}

void CVectorView_new::OnFilePrintPreview()
{
	CDlgPrintOption dlg;
	dlg.m_bOverImg = m_bOverlayImgLay;
	if (dlg.DoModal()!=IDOK)
	{
		return;
	}
	m_nPrintScope = dlg.m_nPrintScope;
	m_nPrintScale = dlg.m_nPrintScale;
	m_fPrintMarginLeft = dlg.m_fMarginLeft;
	m_fPrintMarginRight = dlg.m_fMarginRight;
	m_fPrintMarginTop = dlg.m_fMarginTop;
	m_fPrintMarginBottom = dlg.m_fMarginBottom;
	m_fPrintExtension = dlg.m_fExtension;

	GetClientRect(&m_rcTemp);
	CBaseView::OnFilePrintPreview();
}
BOOL CVectorView_new::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVectorView_new::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	if (m_pPrintContext) delete m_pPrintContext;
	m_pPrintContext = new C2DPrintGDIDrawingContext;
	if (m_pTempCoordSys)delete m_pTempCoordSys;
	m_pTempCoordSys = new CCoordSys;
	if (m_pComCoordSys)delete m_pComCoordSys;
	m_pComCoordSys = new CCoordSys;
	ShowOrHideScrollBar(FALSE);
	if (pInfo->m_bPreview)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
		pMain->GetStatusBar().ShowWindow(SW_HIDE);
	}
}

void CVectorView_new::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	if(m_pPrintContext)delete m_pPrintContext;
	m_pPrintContext = NULL;
	if(m_pTempCoordSys) delete m_pTempCoordSys;
	m_pTempCoordSys = NULL;
	if(m_pComCoordSys) delete m_pComCoordSys;
	m_pComCoordSys = NULL;
	ShowOrHideScrollBar(TRUE);
	if (pInfo->m_bPreview)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
		pMain->GetStatusBar().ShowWindow(SW_SHOW);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVectorView_new diagnostics

#ifdef _DEBUG
void CVectorView_new::AssertValid() const
{
	CBaseView::AssertValid();
}

void CVectorView_new::Dump(CDumpContext& dc) const
{
	CBaseView::Dump(dc);
}

CDlgDoc* CVectorView_new::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDlgDoc)));
	return (CDlgDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVectorView_new message handlers


CWorker *CVectorView_new::GetWorker()
{
	return GetDocument();
}

void CVectorView_new::OnInitialUpdate() 
{
	CDlgDoc *pDlgDoc = GetDocument();
	CDlgDataSource *pDS = pDlgDoc->GetDlgDataSource();	
	if( pDlgDoc!=NULL )
	{
		int nView = 0;
		{
			POSITION pos = pDlgDoc->GetFirstViewPosition();
			while(pos!=NULL)
			{
				pDlgDoc->GetNextView(pos);
				nView++;
			}
		}
		if( nView>1 )
		{			
			RecalcScrollBar(FALSE);
		}
	}
	
	m_markLayL2.Init(GetWorker()->GetSelection(),GetDocument()->GetDlgDataSource(),m_pLeftCS,GetCoordWnd().m_pViewCS);
	
	CDlgDoc *pDoc = GetDocument();
	m_vectLayL2.Init(pDoc, (CDlgDataQuery*)pDoc->GetDataQuery(), m_pLeftCS, GetCoordWnd().m_pViewCS, this,
		(PFDrawFtr)&CVectorView_new::Callback_DrawFeature, (PFCheckLayerVisible)&CVectorView_new::Callback_CheckLayerVisible);

	CBaseView::OnInitialUpdate();
	
	m_variantDragL.m_bUseCoordSys = TRUE;
	m_variantDragR.m_bUseCoordSys = TRUE;

	REG_CMDCLASS(GetWorker(),ID_VIEW_MANUALROTATE,CManualRotateViewCommand);
	
	OnZoomFit();

	ReadRefImageInfo();

	m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_VIEWOVERLAY,TRUE,TRUE);

	GetParentFrame()->ShowWindow(SW_SHOWMAXIMIZED);
}


void CVectorView_new::OnSetXYZ()
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
			FinishUpdateDrawing();

		/*	CDlgDoc *pDoc = GetDocument();
			CCommand *pCurCmd = pDoc->GetCurrentCommand();
			if( pCurCmd )
			{

				pDoc->SetCoordWnd(GetCoordWnd());
				pCurCmd->GetActiveSonProc()->PtClick(pt3d,0);
				//pCurCmd->PtClick(&pt3d,0);
				UpdateConstDragLine();
				UpdateVariantDragLine();
				
				pDoc->TryFinishCurProcedure();
			}*/
		}
		
		DriveToXyz(&pt3d);
	}
	
	Invalidate(FALSE);
}


void CVectorView_new::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_nInnerCmd==ID_VIEW_ROTATE );
	else if( m_nInnerCmd==ID_TOOL_MANUALLOADVECT )
	{
		if( m_bStartLoadVect )
		{
			ClearDragLine();
			OnPostInnerCmd();
			m_nInnerCmd = 0;
			
			PT_4D pt;
			PT_3D pt3d[2], ptt;
			pt.x=point.x; pt.y=point.y;
			ClientToGround(&pt,pt3d+1);
			
			pt3d[0] = m_ptStartLoadVect;
			Envelope evlp;
			evlp.CreateFromPts(pt3d,2);
			
			GetDocument()->UpdateAllViews(this,hc_ManualLoadVect,(CObject*)&evlp);
		}
		else
		{
			PT_4D pt;
			pt.x=point.x; pt.y=point.y;
			ClientToGround(&pt,&m_ptStartLoadVect);
		}
		
		m_bStartLoadVect = !m_bStartLoadVect;
		CSceneViewEx::OnLButtonDown(nFlags,point);
		return;
	}
	else if (m_nInnerCmd != 0)
	{
		CSceneViewEx::OnLButtonDown(nFlags, point);
		return;
	}
// 	PT_4D pt;
// 	pt.x = point.x, pt.y = point.y, pt.z = 0, pt.yr = 0;
// 	ClientToGround(&pt,&m_gCurPoint);
	if( IsDrawStreamline() )
	{
		CInteractiveView::OnLButtonDown(nFlags,point);
		return;
	}
	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;

	CDlgDoc *pDoc = GetDocument();
	BOOL bSelectorOpen = pDoc->IsSelectorOpen();

	CBaseView::OnLButtonDown(nFlags, point);

	PT_3D ptRet = m_gCurPoint;
	
	if( bSelectorOpen )
	{
		CCoordWnd cw = GetCoordWnd();
		PT_4D ptsch = m_gCurPoint;
		
		cw.m_pSearchCS->GroundToClient(&ptsch,1);
		
		CPFeature pFtr = pDoc->GetDataQuery()->FindNearestObject(ptsch,10*cw.GetScaleOfSearchCSToViewCS(),cw.m_pSearchCS);
		
		Envelope et = EnvelopeFromPtAndRadius(ptsch,10*cw.GetScaleOfSearchCSToViewCS());
		
		CGeometry *pObj=NULL;
		if(pFtr)pObj = pFtr->GetGeometry();
		
		if( pObj&&!pObj->FindNearestBasePt(ptsch,et,cw.m_pViewCS,&ptRet,NULL) )
		{
			ptRet = m_gCurPoint;
		}
	}	

 	pDoc->UpdateAllViews(this,hc_SetCrossPos,(CObject*)&ptRet);

	if (m_bMouseDriveVector)
	{
		pDoc->UpdateAllOtherViews(hc_SetCrossPos,(CObject*)&ptRet);
	}
}

void CVectorView_new::OnMouseMove(UINT nFlags, CPoint point) 
{
	static TRACKMOUSEEVENT Track;
	Track.cbSize        = sizeof(TRACKMOUSEEVENT);
	Track.dwFlags       = TME_LEAVE;
	Track.hwndTrack     = m_hWnd;
	Track.dwHoverTime   = HOVER_DEFAULT;
	_TrackMouseEvent (&Track);

	//SetFocus();
	
//	static CPoint ptOld(-1,-1);
	//过滤鼠标的坏信号(原因很复杂，可能是硬件也可能是其他软件造成的)
	if( m_ptLastMouse==point )
	{
		return;
	}
	
 	if( m_nInnerCmd==ID_VIEW_ROTATE );
// 	{
// 		if( m_bViewRotate )
// 		{
// 			CRect rcClient;
// 			GetClientRect(&rcClient);
// 			CPoint cpt = rcClient.CenterPoint();
// 			
// 			//旋转矩形框
// 			double ang = m_lfAngle + GGetAngle(cpt.x,rcClient.bottom-cpt.y,point.x,rcClient.bottom-point.y)	- 
// 				GGetAngle(cpt.x,rcClient.bottom-cpt.y,m_ptViewRotateStart.x,rcClient.bottom-m_ptViewRotateStart.y);
// 			
// 			GrBuffer buf;
// 			buf.EnableWidFromGrd(FALSE);
// 			buf.SetWidth(2);
// 			DrawViewRotate(&buf,cpt,cpt.x<cpt.y?cpt.x:cpt.y,-ang);
// 			
// 			SetVariantDragLine(&buf,FALSE);
// 			UpdateVariantDragLine();
// 		}
// 		CSceneViewEx::OnMouseMove(nFlags,point);
// 		return;
// 	}
	else if( m_nInnerCmd==ID_TOOL_MANUALLOADVECT )
	{
		if( m_bStartLoadVect )
		{
			GrBuffer buf;			
			
			PT_4D pt;
			PT_3D pt3d, ptt;
			pt.x=point.x; pt.y=point.y;
			ClientToGround(&pt,&pt3d);
			buf.BeginLineString(0,0);

			buf.MoveTo(&m_ptStartLoadVect);
			ptt.x = pt3d.x, ptt.y = m_ptStartLoadVect.y, ptt.z = 0;
			buf.LineTo(&ptt);
			buf.LineTo(&pt3d);
			ptt.x = m_ptStartLoadVect.x, ptt.y = pt3d.y, ptt.z = 0;
			buf.LineTo(&ptt);
			buf.LineTo(&m_ptStartLoadVect);

			buf.End();
			SetVariantDragLine(&buf);
			UpdateVariantDragLine();
			FinishUpdateDrawing();
		}
		CSceneViewEx::OnMouseMove(nFlags,point);
		return;
	}
	else if (m_nInnerCmd != 0)
	{
		CSceneViewEx::OnMouseMove(nFlags, point);
		return;
	}
	
	if( IsDrawStreamline() )
	{
		CInteractiveView::OnMouseMove(nFlags,point);
		
		return;
	}

	if( m_bAccuLeave )
	{
		//避免绘制遮挡测标的窗口的问题
// 		EnableCrossVisible(FALSE);
// 		ResetCursorDrawing();
// 		EnableCrossVisible(TRUE);
		m_bAccuLeave = FALSE;
		if (GetDocument()->m_accuBox.IsOpen())
		{
			GetDocument()->m_accuBox.SetReDrawFlag(TRUE);
		}
	}
	
	SetCrossPos(point.x + 0.5, point.y + 0.5, point.x + 0.5);
	UpdateCross();

	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;

	PT_4D pt;
	GroundToClient(&m_gCurPoint,&pt);
	pt.x=point.x+0.5; pt.y=point.y+0.5;
	ClientToGround(&pt,&m_gCurPoint);

	{
		PT_4D pt2 = pt;
		PT_3D testpt;
		ClientToGround(&pt2,&testpt);
		GroundToClient(&testpt,&pt2);
	}
	
	if( !m_strDemPath.IsEmpty() )
		m_gCurPoint.z = m_dem.GetZ(m_gCurPoint.x,m_gCurPoint.y);
	
	m_ptLastMouse = point;
	
// 	if( IsDrawStreamline() )
// 	{
// 		CInteractiveView::OnMouseMove(nFlags,point);
// 	
// 		return;
// 	}
	
	if (m_bMouseDriveVector)
	{
		CDlgDoc *pDoc = GetDocument();
		pDoc->UpdateAllOtherViews(hc_SetCrossPos,(CObject*)&m_gCurPoint);
	}

	CBaseView::OnMouseMove(nFlags, point);	

	GetDocument()->UpdateAllViews(NULL, 55, (CObject*)&m_gCurPoint);
}

void CVectorView_new::OnRButtonDown(UINT nFlags, CPoint point) 
{
// 	PT_4D pt;
// 	pt.x = point.x, pt.y = point.y, pt.z = 0, pt.yr = 0;
// 	ClientToGround(&pt,&m_gCurPoint);

	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;
	
	CBaseView::OnRButtonDown(nFlags, point);
}

void CVectorView_new::OnPaint() 
{
	CBaseView::OnPaint();
}

BOOL CVectorView_new::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	float change = GetComboNextZoom(zDelta>0?1:0)/GetZoomRate();

	if( zDelta>0 )
		change = 1 + 0.5f* zDelta/120;
	else
		change = 1/(1 - 0.5f* zDelta/120);

 	if( change<=0 )return FALSE;
 	
 	if( change>1 && change<1.5f )change = 1.5f;
 	if( change<1 && change>0.7f )change = 0.7f;
	
	pt.x = (short)pt.x; pt.y = (short)pt.y;

// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = WM_MOUSEWHEEL;
	OnPreInnerCmd(WM_MOUSEWHEEL);
	ScreenToClient(&pt);
//	ZoomNext(pt,zDelta>0?1:0,FALSE);
	ZoomCustom(pt,change,FALSE);

	GetCoordWnd();

	OnPostInnerCmd();
//	m_nInnerCmd = save;

	bool bRet = CBaseView::OnMouseWheel(nFlags, zDelta, pt);


	CRect rcClient;
	GetClientRect(&rcClient);
	CPoint cpt = rcClient.CenterPoint();
	PT_4D fcpt;
	PT_3D fgpt;
	fcpt.x = cpt.x;  fcpt.y = cpt.y; fcpt.z = 0;
	ClientToGround(&fcpt, &fgpt);
	GetDocument()->UpdateAllViews(NULL, 79, (CObject *)&fgpt);

	return bRet;
}

void ReadVertex(FILE *fp, int index, PT_3DEX *pt)
{
	double v[3];
	fseek(fp,index*sizeof(double)*3,SEEK_SET);
	fread(v,sizeof(double),3,fp);
	pt->x = v[0];
	pt->y = v[1];
	pt->z = v[2];
}
void CVectorView_new::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	/*
	double m[16], m2[16], mt[16];
	matrix_toIdentity(m,4);
	BOOL bIsRotate = TRUE;
	
	double ang = 5*PI/180;
	double cosa = cos(ang), sina = sin(ang);
	switch(nChar) 
	{
	case 'a':
	case 'A':
		m[3] = -10;
		bIsRotate = FALSE;
		break;
	case 'd':
	case 'D':
		m[3] = 10;
		bIsRotate = FALSE;
		break;
	case 'w':
	case 'W':
		m[7] = -10;
		bIsRotate = FALSE;
		break;
	case 's':
	case 'S':
		m[7] = 10;
		bIsRotate = FALSE;
		break;
	case 'z':
	case 'Z':
		m[0] = m[5] = m[10] = 2.0;
		break;
	case 'x':
	case 'X':
		m[0] = m[5] = m[10] = 0.5;
		break;
	case VK_LEFT:
		m[0] = cosa; m[2] = -sina;
		m[8] = sina; m[10] = cosa;
		break;
	case VK_RIGHT:
		m[0] = cosa; m[2] = sina;
		m[8] =-sina; m[10] = cosa;
		break;
	case VK_UP:
		m[5] = cosa; m[6] = sina;
		m[9] =-sina; m[10] = cosa;
		break;
	case VK_DOWN:
		m[5] = cosa; m[6] = -sina;
		m[9] = sina; m[10] = cosa;
		break;
	case 'c':
	case 'C':
		m[0] = cosa; m[1] = -sina;
		m[4] = sina; m[5] = cosa;
		break;
	case 'v':
	case 'V':
		m[0] = cosa; m[1] = sina;
		m[4] =-sina; m[5] = cosa;
		break;		
	default:
		CBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	
	CRect rect;
	GetClientRect(&rect);
	
	if( bIsRotate )
	{
		matrix_toIdentity(m2,4);
		m2[3] = -rect.CenterPoint().x;
		m2[7] = -rect.CenterPoint().y;
		matrix_multiply(m,m2,4,mt);
		m2[3] = rect.CenterPoint().x;
		m2[7] = rect.CenterPoint().y;
		matrix_multiply(m2,mt,4,m);
	}

	m_pLeftCS->MultiplyMatrix(m);
	OnChangeCoordSys(FALSE);

	RecalcScrollBar(FALSE);

	AdjustCellScale();
	
	matrix_multiply(m,m_matrix,4,mt);
	memcpy(m_matrix,mt,sizeof(mt));
	matrix_reverse(m_matrix,4,m_rmatrix);
	
	Invalidate(FALSE);
*/	
	if( 0 && (nChar=='a' || nChar=='A') )
	{
		CDlgDoc *pDoc = GetDocument();
		CDlgDataSource *pDS = pDoc->GetDlgDataSource();
		CFtrLayer *pLayer = pDS->GetFtrLayer("缺省线");
		CUndoFtrs undo(pDoc,"Test");
		FILE *fp = fopen("g:\\66\\test2.tb","rb");
		FILE *fp2 = fopen("g:\\66\\test2.vb","rb");
		if( pLayer && fp && fp2 )
		{
			PT_3DEX expts[4];
			expts[0].pencode = expts[1].pencode = expts[2].pencode = penLine;
			pDoc->BeginBatchUpdate();
			int index[3] = {0};
			int nread = fread(index,sizeof(int),3,fp);
			int nobj = 0;
			while( nread==3 )
			{
				ReadVertex(fp2,index[0],expts);
				ReadVertex(fp2,index[1],expts+1);
				ReadVertex(fp2,index[2],expts+2);
				expts[3] = expts[0];
				CFeature *pFtr = pLayer->CreateDefaultFeature(2000,CLS_GEOCURVE);
				pFtr->GetGeometry()->CreateShape(expts,4);
				pDoc->AddObject(pFtr);
				undo.arrNewHandles.Add(FtrToHandle(pFtr));
				nobj++;
				nread = fread(index,sizeof(int),3,fp);
			}
			pDoc->EndBatchUpdate();
		}
		if( fp )fclose(fp);
		if( fp2 )fclose(fp2);
		undo.Commit();
	}
	CBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CVectorView_new::DrawBkgnd(GrBuffer *buf)
{
	if(!GetDocument()->GetDlgDataSource())return;
	BOOL bEnable;
	COLORREF clr;
	PT_3D pts[4];
	int nData = GetDocument()->GetDlgDataSourceCount();
	for (int i=0;i<nData;i++)
	{
		if(GetDocument()->GetDlgDataSource(i)->GetShowBound())
		{
			GetDocument()->GetDlgDataSource(i)->GetMonoColor(&bEnable,&clr);
			if (bEnable)
			{
				buf->BeginLineString(clr,2,0);
			}
			else
			{
				if (m_clrBound == 0)
				{
					buf->BeginLineString(m_clrBound/*RGB(255,0,0)*/,2,0,0,1,FALSE,TRUE);
				}
				else
				{
					buf->BeginLineString(m_clrBound/*RGB(255,0,0)*/,2,0);
				}
			}
			GetDocument()->GetDlgDataSource(i)->GetBound(pts,NULL,NULL);
			buf->MoveTo(&pts[0]);
			for( int i=1; i<sizeof(pts)/sizeof(pts[0]); i++ )
			{
				buf->LineTo(&pts[i]);
			}	
			buf->LineTo(&pts[0]);
			buf->End();
		}
	}
}

BOOL CVectorView_new::IsDrawStreamline()
{
	if( !m_bDisableMouseIn3DDraw )
	{
		m_b3DDraw = FALSE;
		return FALSE;
	}
	
	CCommand *pCurCmd = GetDocument()->GetCurrentCommand();
//	DrawingInfo info  = GetDocument()->GetCurDrawingObj();
	if( pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)) )
	{
		if( /*m_nInputType==STEREOVIEW_INPUT_3DMOUSE &&*/
			((CDrawCommand*)pCurCmd)->GetCurPenCode()==penStream )
		{
			m_b3DDraw = TRUE;
			return TRUE;
		}
		
		if( m_b3DDraw &&
			((CDrawCommand*)pCurCmd)->GetCurPenCode()==penStream )
		{
			return TRUE;
		}
	}
	
	m_b3DDraw = FALSE;
	return FALSE;
}

LRESULT CVectorView_new::OnDeviceLFootDown(WPARAM nCode, LPARAM lParam)
{
	if( !m_bOverlayImgLay || m_bDriveOnlyStereo)
		return 0;
	
	CPoint point;
	::GetCursorPos(&point);
	point.x = (short)point.x; point.y = (short)point.y;
	ScreenToClient(&point);
	
	if( !lParam )
		CBaseView::OnLButtonDown(0,point);
	else
		CBaseView::OnLButtonUp(0,point);
	
	return 1;
}


LRESULT CVectorView_new::OnDeviceRFootDown(WPARAM nCode, LPARAM lParam)
{
	if( !m_bOverlayImgLay  || m_bDriveOnlyStereo)
		return 0;
	
	CPoint point;
	::GetCursorPos(&point);
	point.x = (short)point.x; point.y = (short)point.y;
	ScreenToClient(&point);
	
	if( !lParam )
		CBaseView::OnRButtonDown(0,point);
	else
		CBaseView::OnRButtonUp(0,point);
	
	return 1;
}



LRESULT CVectorView_new::OnDeviceMove(WPARAM nCode, LPARAM lParam)
{
	if (m_nInnerCmd == IDC_BUTTON_ADJUST || m_nInnerCmd == IDC_BUTTON_CURSOR)
		return 0;

	if( m_bDriveOnlyCursor )
	{
		return OnDeviceMove2(nCode,lParam);
	}
	if( !m_bOverlayImgLay|| m_bDriveOnlyStereo )
		return 0;
	
	if( m_nInputType!=STEREOVIEW_INPUT_3DMOUSE )
	{
		m_gCur3DMousePoint = m_gCurPoint;
	}
	
	m_nInputType = STEREOVIEW_INPUT_3DMOUSE;

	VDSEND_DATA* pData =(VDSEND_DATA*)lParam;
	int img_dx=0, img_dy=0, img_dz=0, img_dq=0;
	float cur_crsx,cur_crsy,cur_crsz, new_crsx,new_crsy,new_crsz;
	
	CRect rcClient;
	GetClientRect(&rcClient);
	GetCrossPos(&cur_crsx,&cur_crsy,&cur_crsz);
	
	//得到当前测标的影像坐标
	PT_3D gCurPoint = m_gCur3DMousePoint;
	PT_4D cltpt,new_cltpt, test;
	GroundToClient(&gCurPoint,&cltpt);
	
	new_crsx = cur_crsx;
	new_crsy = cur_crsy;
	new_crsz = new_crsx;
	
	//视差模式
	{
		new_cltpt = cltpt;
		new_cltpt.x += pData->x, new_cltpt.y += pData->y;
		
		img_dx = PIXEL(new_crsx)-PIXEL(new_cltpt.x);  
		img_dy = PIXEL(new_crsy)-PIXEL(new_cltpt.y);
	}
	
	//滚动影像
	if( img_dx || img_dy || img_dz || img_dq )
	{
		Scroll(img_dx,img_dy,img_dz,img_dq);
//		m_ptSelDragStart.x += img_dx;  m_ptSelDragStart.y += img_dy;
		
		new_cltpt.x += img_dx;
		new_cltpt.y += img_dy;
	}
	else
	{
		UpdateCross();
	}
	
	ClientToGround(&new_cltpt, &gCurPoint);
	GroundToClient(&gCurPoint,&test);
	
	m_gCur3DMousePoint.x = gCurPoint.x; m_gCur3DMousePoint.y = gCurPoint.y;
	
	if( !m_strDemPath.IsEmpty() )
		m_gCur3DMousePoint.z = m_dem.GetZ(m_gCur3DMousePoint.x,m_gCur3DMousePoint.y);
	
	m_gCurPoint = m_gCur3DMousePoint;
	
	//CTDoc *pDoc = GetDocument();
	
	CBaseView::OnMouseMove(0,CPoint(PIXEL(cltpt.x),PIXEL(cltpt.y)));
	
	return 1;
}

LRESULT CVectorView_new::OnDeviceMove2(WPARAM nCode, LPARAM lParam)
{
	if( !m_bOverlayImgLay || m_bDriveOnlyStereo )
		return 0;
	
	if( m_nInputType!=STEREOVIEW_INPUT_3DMOUSE )
	{
		m_gCur3DMousePoint = m_gCurPoint;
	}
	
	m_nInputType = STEREOVIEW_INPUT_3DMOUSE;
	
	VDSEND_DATA* pData =(VDSEND_DATA*)lParam;
	int img_dx=0, img_dy=0, img_dz=0, img_dq=0;
	float cur_crsx,cur_crsy,cur_crsz, new_crsx,new_crsy,new_crsz;
	
	CRect rcClient;
	GetClientRect(&rcClient);
	GetCrossPos(&cur_crsx,&cur_crsy,&cur_crsz);
	
	//得到当前测标的影像坐标
	PT_3D gCurPoint = m_gCur3DMousePoint;
	PT_4D cltpt,new_cltpt, test;
	GroundToClient(&gCurPoint,&cltpt);
	
	//视差模式
	{
		new_cltpt = cltpt;
		new_cltpt.x += pData->x, new_cltpt.y += pData->y;
	}
	
	SetCrossPos(new_cltpt.x,new_cltpt.y,new_cltpt.x);
	UpdateCross();
	
	ClientToGround(&new_cltpt, &gCurPoint);
	GroundToClient(&gCurPoint,&test);
	
	m_gCur3DMousePoint.x = gCurPoint.x; m_gCur3DMousePoint.y = gCurPoint.y;
	
	if( !m_strDemPath.IsEmpty() )
		m_gCur3DMousePoint.z = m_dem.GetZ(m_gCur3DMousePoint.x,m_gCur3DMousePoint.y);
	
	m_gCurPoint = m_gCur3DMousePoint;
	
//	CTDoc *pDoc = GetDocument();
	
	CBaseView::OnMouseMove(0,CPoint(PIXEL(cltpt.x),PIXEL(cltpt.y)));
	
	return 1;
}


LRESULT CVectorView_new::OnMoveCursorToCenter2(WPARAM, LPARAM)
{
	if( !m_bOverlayImgLay || m_bDriveOnlyStereo )
		return 1;
	
	PT_3D pt3d = m_gCurPoint;
	DriveToXyz(&pt3d,TRUE);
	Invalidate(FALSE);
	return 0;
}


void CVectorView_new::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch( lHint )
	{
	case hc_SetCrossPos:
		{
			if( m_bAccuLeave )
			{
				EnableCrossVisible(TRUE);
			}

			PT_3D *pt = (PT_3D*)pHint;
			DriveToXyz(pt, FALSE, !(pSender && pSender->GetRuntimeClass() == RUNTIME_CLASS(COsgbView)));
			UpdateConstDragLine();
		}
		break;
	case hc_SetAnchorPoint:
		{
			if( m_bAccuLeave )
			{
				EnableCrossVisible(TRUE);
			}
			
			PT_3D pt = *((PT_3D*)pHint);
			pt.z = m_gCurPoint.z;
			DriveToXyz(&pt);
			UpdateConstDragLine();
		}
		break;
	case hc_UpdateOption:
		{
			CBaseView::OnUpdate( pSender,  lHint,  pHint);

			int value;
			value = m_clrHilite;
			value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_HILITECOLOR,value);
			m_clrHilite = value;

			m_markLayL2.SetMark1Color(m_clrHilite);
			
			value = m_clrDragLine;
			value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_DRAGCOLOR,value);
			m_clrDragLine = value;

			value = m_clrBound;
			value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_BOUNDCOLOR,value);
			m_clrBound = value;	

			value = m_clrBack;
			value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_BACKCOLOR,value);
			m_clrBack = value;	

			m_pContext->SetBackColor(m_clrBack);
			
			m_bSymbolize = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_SYMBOLIZE,m_bSymbolize);
			OnUpdate(this,hc_UpdateOverlayBound,NULL);

			m_bMouseDriveVector = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_DRIVEVECTOR,m_bMouseDriveVector);

			m_bDriveOnlyStereo = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYSTEREO,TRUE);
			
			m_bDriveOnlyCursor = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,FALSE);
			
			m_bDisableMouseIn3DDraw = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DIABLEMOUSE,m_bDisableMouseIn3DDraw);
		
			m_clrFtrPtColor = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_FEATUREPTCOLOR,m_clrFtrPtColor);
			
			m_bImageZoomNotLinear = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,"ImageZoomNotLinear",m_bImageZoomNotLinear);

			m_markLayL2.SetFtrPtColor(m_clrFtrPtColor);
		}
		break;
	case hc_SetMapArea:
		{
			if( !pHint )break;
			PT_3D *pPts = (PT_3D*)pHint;	
			if( !pPts )break;
			if(!m_bOverlayImgLay)
			{
				DWORD dwScale = ((CDlgDoc*)GetWorker())->GetDlgDataSource()->GetScale();
				//考虑显示设备的Ppi ,1英寸=2.54cm
				int nXPpi = ::GetDeviceCaps(m_hViewDC,LOGPIXELSX);
				int nYPpi = ::GetDeviceCaps(m_hViewDC,LOGPIXELSY);
				//当前比例尺下1m对应多少像素
				m_lfMapScaleX = ((100.0*nXPpi)/(dwScale*2.54));
				m_lfMapScaleY = ((100.0*nYPpi)/(dwScale*2.54));
				m_gsa.lfGKX =m_lfMapScaleX;
				m_gsa.lfGKY = m_lfMapScaleY;
				m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
				m_pContext->m_fDrawCellKY = m_gsa.lfGKY;
				m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
				m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;
			}
			else
			{
				m_lfMapScaleX = 1.0;
				m_lfMapScaleY = 1.0;
			}

// 			//客户中心作为焦点，位置不变，这里先记录此位置
			CRect rcClient;
			GetClientRect(&rcClient);

			CVectorViewCenterPoint ptSave(this);
			
			//更新坐标系参数
		
			memcpy(m_ptBounds,pPts,sizeof(PT_3D)*4);

			
			//by shy 更新模型窗口
			AfxGetMainWnd()->SendMessage(FCCM_REFRESHVECWIN,0,(LPARAM)m_ptBounds);
			
			//重新装载矢量数据
			LoadDocData();
						
			RecalcScrollBar(FALSE);
			UpdateDrawingLayers(NULL,NULL);
		}
		return;
	case hc_SetViewArea:
		{
			if( !pHint )break;
			
			//客户中心作为焦点，位置不变，这里先记录此位置
			CRect rcClient;
			GetClientRect(rcClient);

			CVectorViewCenterPoint ptSave(this);			
				
			
			
			RecalcScrollBar(FALSE);
			
			//重新装载矢量数据
			LoadDocData();
			
			Invalidate(FALSE);
		}
		break;
	case hc_AddObject:
		m_vectLayL2.DrawObjtoCache((CFeature*)pHint);
		Invalidate(FALSE);
		return;
	case hc_DelObject:
		m_vectLayL2.ClearBlockOfObj((LONG_PTR)(CFeature*)pHint);
		m_vectLayL2.DelObj((LONG_PTR)(CFeature*)pHint);
 		m_vectLayL2.EraseObjFromCache((CFeature*)pHint);
		Invalidate(FALSE);
		return;
	case hc_DelAllObjects:
		m_vectLayL2.DelAllObjs();
		m_vectLayL2.ClearAll();
		Invalidate(FALSE);
		return;
	case hc_UpdateAllObjects:
	case hc_UpdateAllObjects_Visible:
	case hc_UpdateAllObjects_VectorView:
		{
			//客户中心作为焦点，位置不变，这里先记录此位置
			CVectorViewCenterPoint ptSave(this);
			
			//重新装载矢量数据
			LoadDocData();

			RecalcScrollBar(FALSE);
			
			Invalidate(FALSE);
		}
		return;
	case hc_UpdateOverlayBound:
		{
			m_vectLayL2.DelObj(HANDLE_BOUND);
			//DelObjfromVectLay(HANDLE_BOUND);
			m_vectLayL2.ClearAll();
				
			GrBuffer buf;
			DrawBkgnd(&buf);

			AddObjtoVectLay2(HANDLE_BOUND,&buf,TRUE);
			Invalidate(FALSE);
		}
		break;

	case hc_RotateAsStereo:
		{
			BOOL bRotate = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,"RotateAsStereo",0);
			if( bRotate )ViewRotateAsStereo();
		}
		break;
	case hc_GetImagePosition:
		{
			if( pHint )
			{
				ViewImgPosition *params = (ViewImgPosition*)pHint;

				int index = 0;
				if( GetRasterLayer(params->fileName, index) )
				{
					*params = m_arrImgPositions[index];
				}
			}
		}
		break;
	case hc_RestoreImagePosition:
		if( pHint )
		{
			CVectorViewCenterPoint ptSave(this);
			
			ViewImgPosition *pData = (ViewImgPosition*)pHint;
			
			int index = 0;
			CRasterLayer *pLayer = GetRasterLayer(pData->fileName,index);
			if( !pLayer )
				break;

			m_arrImgPositions[index] = *pData;
			SetImagePosition(index);
			
			//更新视图显示
			RefreshImageOverlay();

			SaveRefImageInfo();
		}
		break;
	case hc_SetImagePosition:
		if( pHint )
		{
			CVectorViewCenterPoint ptSave(this);
			
			ViewImgPosition *pData = (ViewImgPosition*)pHint;

			int index = 0;
			CRasterLayer *pLayer = GetRasterLayer(pData->fileName,index);
			if( !pLayer )
				break;

			ViewImgPosition item = m_arrImgPositions[index];
			item = *pData;
			m_arrImgPositions[index] = item;

			SetImagePosition(index);

			//更新视图显示
			RefreshImageOverlay();

			SaveRefImageInfo();
		}
		break;
	case hc_UpdateLayerDisplay:
		{
			UpdateDispyParam *pParam = (UpdateDispyParam*)pHint;
			if( !pParam )break;
			CFtrLayer *pLayer = ((CFtrLayer *)pParam->handle);
			if (!pLayer)
			{
				return;
			}
			if(pParam->type==UpdateDispyParam::typeVISIBLE)
			{
				BOOL bViewValue = TRUE;
				m_mapLayerVisible.Lookup(pLayer,bViewValue);
				
				bViewValue &= pLayer->IsVisible();				
				
				for (int i=0;i<pLayer->GetObjectCount();i++)
				{
					m_vectLayL2.SetObjVisible((LONG_PTR)(pLayer->GetObject(i)), bViewValue);
				}
				
			}
			if (pParam->type==UpdateDispyParam::typeCOLOR)
			{
				for (int i=0;i<pLayer->GetObjectCount();i++)
				{
					CPFeature pFtr = pLayer->GetObject(i);
					if (pFtr && ( (pFtr->GetGeometry() && pFtr->GetGeometry()->GetColor() == FTRCOLOR_BYLAYER) || m_bSymbolscolorByLayer) )
					{
						m_vectLayL2.SetObjColor((LONG_PTR)(pFtr), pParam->data.lColor);
					}
				}				
			}
			m_vectLayL2.ClearAll();
		}	
		break;
	case hc_UpdateObjectDisplay:
		{
			UpdateDispyParam *pParam = (UpdateDispyParam*)pHint;
			if( !pParam )break;
			CFeature *pFtr = ((CFeature*)pParam->handle);
			if (!pFtr)
			{
				return;
			}
			if(pParam->type==UpdateDispyParam::typeVISIBLE)
			{
				m_vectLayL2.SetObjVisible((LONG_PTR)pFtr, pParam->data.bVisible);
			}
			if (pParam->type==UpdateDispyParam::typeCOLOR)
			{
				m_vectLayL2.SetObjColor((LONG_PTR)(pFtr), pParam->data.lColor);
			}
			m_vectLayL2.ClearAll();
		}	
		break;
	case hc_UpdateDataColor:
		{
			CDlgDataSource *pDS = (CDlgDataSource*)pHint;
			if( !pDS )break;			
			CFtrLayer* pLayer = NULL;
			CFeature *pFtr = NULL;
			int j = 0;
			BOOL bEnable;
			COLORREF clrMono;
			COLORREF clrFtr;
			pDS->GetMonoColor(&bEnable,&clrMono);
			for (int i=0;i<pDS->GetFtrLayerCount();i++)
			{
				pLayer = pDS->GetFtrLayerByIndex(i);
				if (!pLayer||pLayer->IsDeleted()) continue;		
				for(j=0;j<pLayer->GetObjectCount();j++)
				{
					pFtr = pLayer->GetObject(j);
					if (pFtr)
					{
						if(pFtr->GetGeometry()->GetColor() == FTRCOLOR_BYLAYER || m_bSymbolscolorByLayer)
						{
							clrFtr = pLayer->GetColor();
						}
						else
							clrFtr = pFtr->GetGeometry()->GetColor();
						m_vectLayL2.SetObjColor(LONG_PTR(pFtr),bEnable?clrMono:clrFtr);
					}
				}
			}
			m_vectLayL2.ClearAll();
			Invalidate(FALSE);
		}
		break;
	case hc_AddGraph:
		if( pHint )
		{
			AddObjtoVectLay2(HANDLE_GRAPH,(const GrBuffer*)pHint,TRUE);			
			Invalidate(FALSE);
		}
		break;
	case hc_DelGraph:
		m_vectLayL2.DelObj(HANDLE_GRAPH);
		m_vectLayL2.ClearAll();
		Invalidate(FALSE);
		break;
	case hc_AddSusPtColor:
		if( pHint )
		{
			AddObjtoVectLay2(HANDLE_SUSPT,(const GrBuffer*)pHint,TRUE);
			Invalidate(FALSE);
		}
		break;
	case hc_DelSusPtColor:
		m_vectLayL2.DelObj(HANDLE_SUSPT);		
		Invalidate(FALSE);
		break;
	case hc_AddPseudoPtColor:
		if( pHint )
		{
			AddObjtoVectLay2(HANDLE_PSEPT,(const GrBuffer*)pHint,TRUE);
			Invalidate(FALSE);
		}
		break;
	case hc_DelPseudoPtColor:
		m_vectLayL2.DelObj(HANDLE_PSEPT);		
		Invalidate(FALSE);
		break;
	case hc_UpdateRefFileText:
		{
			m_vectLayL2.DelObj(HANDLE_REFMARK);

			if (m_bMarkReffile)
			{
				CDlgDoc *pDoc = GetDocument();
				if( pDoc )
				{
					int nCount = pDoc->GetDlgDataSourceCount();
					if( nCount>1 )
					{
						PT_3D pts[4];
						Envelope e;
						GrBuffer buf;
						TextSettings settings;
						settings.fHeight = pDoc->GetDlgDataSource()->GetScale()*50*0.001;
						settings.nAlignment = TAH_MID|TAV_MID;
						
						CString text;
						
						for( int i=1; i<nCount; i++)
						{
							CDlgDataSource *pDS = pDoc->GetDlgDataSource(i);
							if( !pDS )continue;
							
							pDS->GetBound(pts,NULL,NULL);
							
							e.CreateFromPts(pts,4);
							
							PT_3DEX expt(e.CenterX(),e.CenterY(),0,penLine);
							
							text.Format("%s",ExtractFileNameExt(pDS->GetName()));
							
							buf.Text(RGB(255,0,0),&expt,text,&settings);
						}
						
						AddObjtoVectLay2(HANDLE_REFMARK,&buf,TRUE,TRUE);
					}
				}	
			}

			Invalidate(FALSE);
			break;
		}
	case hc_GetOverlayImgNames:
		{
			if( pHint )
			{
				CStringArray *pArray = (CStringArray*)pHint;
				for( int i=0; i<m_arrImgPositions.GetSize(); i++)
				{
					pArray->Add(m_arrImgPositions[i].fileName);
				}
			}
		}
		break;
	case hc_RotateVectView:
		{
			if( pHint )
			{
				double ang = *(double*)pHint;
				ang -= m_gsa.lfAngle;

				double m[16], m2[16], mt[16];
				matrix_toIdentity(m,4);
				BOOL bIsRotate = TRUE;
				
				double cosa = cos(ang), sina = sin(ang);
				m[0] = cosa; m[1] = -sina;
				m[4] = sina; m[5] = cosa;
				
				CRect rect;
				GetClientRect(&rect);
				
				if( bIsRotate )
				{
					matrix_toIdentity(m2,4);
					m2[3] = -rect.CenterPoint().x;
					m2[7] = -rect.CenterPoint().y;
					matrix_multiply(m,m2,4,mt);
					m2[3] = rect.CenterPoint().x;
					m2[7] = rect.CenterPoint().y;
					matrix_multiply(m2,mt,4,m);
				}
				
				m_pLeftCS->MultiplyMatrix(m);
				OnChangeCoordSys(FALSE);
				
				RecalcScrollBar(FALSE);
				
				AdjustCellScale();

				Invalidate(FALSE);

			}
		}
		break;
	default:
		CBaseView::OnUpdate( pSender,  lHint,  pHint);
	}	
	
}

void CVectorView_new::SetVariantDragLine(const GrBuffer *pBuf, BOOL bGround)
{
	GrBuffer2d buf1, buf2;
	
	ConvertGrBufferToVectLayer(pBuf,&buf1,&buf2);
	
	buf1.SetAllColor(m_clrDragLine);
	buf2.SetAllColor(m_clrDragLine);
	
	m_variantDragL.SetBuffer(&buf1);	
	m_variantDragR.SetBuffer(&buf2);
}

void CVectorView_new::AddVariantDragLine(const GrBuffer *pBuf, BOOL bGround)
{
	GrBuffer2d buf1, buf2;
	
	ConvertGrBufferToVectLayer(pBuf,&buf1,&buf2);
	
	buf1.SetAllColor(m_clrDragLine);
	buf2.SetAllColor(m_clrDragLine);
	
	m_variantDragL.AddBuffer(&buf1);
	m_variantDragR.AddBuffer(&buf2);
}

BOOL CVectorView_new::InitBmpLayer()
{	
	/*BOOL bWhiteBK = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_WHILEBK,FALSE);
	if( bWhiteBK )
	{
		m_pContext->SetBackColor(RGB(255,255,255));
		
		if( m_clrHilite==RGB(255,255,255) )
			m_clrHilite = 0;
	}
	else
	{
		m_pContext->SetBackColor(0);
	}*/

	m_pContext->SetBackColor(m_clrBack);

	if( !CBaseView::InitBmpLayer() )
		return FALSE;

	m_vectLayL2.SetContext(m_pContext->GetLeftContext());
	m_vectLayL2.Destroy();

	m_markLayL2.SetContext(m_pContext->GetLeftContext());
	m_markLayL2.Destroy();

	//cahce类型与CVectLayer一致
	if( !m_vectLayL2.InitCache( GetBestCacheTypeForLayer(&m_vectLayL2),
		CSize(256,256),100 ) )
	{
		AfxMessageBox(IDS_ERR_CREATEVIEW);
		GetDocument()->m_bPromptSave = TRUE;
		GetDocument()->m_nPromptRet = IDYES;
		PostMessage(WM_CLOSE,0,0);
		return FALSE;
	}

	if( !m_markLayL2.InitCache( GetBestCacheTypeForLayer(&m_markLayL2),
		CSize(256,256),100 ) )
	{
		AfxMessageBox(IDS_ERR_CREATEVIEW);
		GetDocument()->m_bPromptSave = TRUE;
		GetDocument()->m_nPromptRet = IDYES;
		PostMessage(WM_CLOSE,0,0);
		return FALSE;
	}
	
	Envelope dataBound = GetDocument()->GetDlgDataSource()->GetBound();	
	m_vectLayL2.SetDataBound(dataBound);
	m_markLayL2.SetDataBound(dataBound);

	//好像没用	
	BOOL bSolidText = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_SOLIDTEXT,FALSE);
	
	m_vectLayL.Destroy();
	m_vectLayR.Destroy();
	m_markLayL.Destroy();
	m_markLayR.Destroy();

	m_laymgrLeft.InsertBeforeDrawingLayer(&m_vectLayL2,&m_constDragL);
	m_laymgrLeft.InsertBeforeDrawingLayer(&m_markLayL2,&m_constDragL);
	
	m_snapLayL.m_nEraseType = CVariantDragLayer::eraseXOR;
	m_laymgrLeft.InsertBeforeDrawingLayer(&m_snapLayL,&m_cursorL);
	
	m_accurboxLayL.m_nEraseType = CVariantDragLayer::eraseXOR;
	m_laymgrLeft.InsertBeforeDrawingLayer(&m_accurboxLayL,&m_variantDragL);
	
	for( int i=m_laymgrLeft.GetDrawingLayerCount()-1; i>=0; i-- )
	{
		for( int j=0; j<m_arrPImgLays.GetSize(); j++)
		{
			if( m_laymgrLeft.GetDrawingLayer(i)==m_arrPImgLays[j] )
			{
				m_laymgrLeft.RemoveDrawingLayer(m_arrPImgLays[j]);
				m_laymgrLeft.InsertDrawingLayer(m_arrPImgLays[j],0);
			}
		}
	}

	m_vectLayL2.EnableBGLoad(FALSE);
	m_markLayL2.EnableBGLoad(FALSE);

	m_constDragL.EnableBGLoad(FALSE);
	m_cursorL.EnableBGLoad(FALSE);
	m_snapLayL.EnableBGLoad(FALSE);
	m_accurboxLayL.EnableBGLoad(FALSE);
	m_variantDragL.EnableBGLoad(FALSE);
	
	//by shy
	//	SetVectLayNoLineWeight();
	//	SetVectLayNoHatch();
	//	SetVectLayDisplayOrder();

	return TRUE;

}


BOOL CVectorView_new::Callback_CheckLayerVisible(CFtrLayer *pLayer)
{
	BOOL bViewValue = TRUE;
	m_mapLayerVisible.Lookup(pLayer,bViewValue);
	
	bViewValue &= pLayer->IsVisible();

	return bViewValue;
}


//nSymbolizedFlag：SYMBOLIZE_NONE，不符号化，SYMBOLIZE_ALL，符号化，SYMBOLIZE_PART，部分符号符号化；
void CVectorView_new::Callback_DrawFeature(CDlgDataSource *pDS,CFtrLayer *pLayer,CFeature *pFtr, int nSymbolizedFlag, GrBuffer *pBuf)
{	
	if( !pFtr || !pFtr->IsVisible() || !pLayer->IsVisible())
		return;

	BOOL bEnable;
	COLORREF monoClr;
	pDS->GetMonoColor(&bEnable,&monoClr);

	if( !CanSymbolized(pFtr) )
		nSymbolizedFlag = SYMBOLIZE_NONE;

	if( nSymbolizedFlag!=SYMBOLIZE_NONE && pLayer->IsSymbolized())
	{							
		GrBuffer& buf = *pBuf;
		CGeometry *pGeo = pFtr->GetGeometry();
		if (pDS && pGeo)
		{
			COLORREF saveColor = pGeo->GetColor();
			BOOL bFilledColor = FALSE;
			DWORD transparent = -1;
			COLORREF fillColor = -1;
			if(m_bFillColor && pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				bFilledColor = ((CGeoCurveBase*)pGeo)->IsFillColor();
				fillColor = ((CGeoCurveBase*)pGeo)->GetFillColor();
				transparent = ((CGeoCurveBase*)pGeo)->GetTransparency();
				((CGeoCurveBase*)pGeo)->EnableFillColor(FALSE, 0);
			}
			if(pGeo->GetColor()==COLOUR_BYLAYER)
			{
				pGeo->SetColor(pLayer->GetColor());
			}
			BOOL bDrawSymbol = FALSE;
			if( nSymbolizedFlag==SYMBOLIZE_ALL )
				bDrawSymbol = pDS->DrawFeature(pFtr,&buf,pGeo->IsSymbolizeText(),m_gsa.lfAngle);
			else
				bDrawSymbol = pDS->DrawFeature_SymbolizePart(pFtr,&buf,pGeo->IsSymbolizeText(),m_gsa.lfAngle);

			if( ((!bDrawSymbol) && pGeo->GetClassType()==CLS_GEOTEXT ) || !CAnnotation::m_bUpwardText )
				;//buf.RotateGrTextPar(m_gsa.lfAngle*180/PI);
			else if(CAnnotation::m_bUpwardText)
			{
				buf.RotateGrTextEnvelope(m_gsa.lfAngle);
			}

			pGeo->SetColor(saveColor);
			if(m_bFillColor && bFilledColor)
			{
				((CGeoCurveBase*)pGeo)->EnableFillColor(TRUE, fillColor);
				((CGeoCurveBase*)pGeo)->SetTransparency(transparent);
			}

			if (bEnable)
			{
				buf.SetAllColor(monoClr);
			}
		}					
	}
	else
	{
		GrBuffer& buf = *pBuf;
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
	}

	if( m_bFillColor && pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)) && ((CGeoSurface*)(pFtr->GetGeometry()))->IsClosed() )
	{
		CGeometry *pGeo = pFtr->GetGeometry()->Clone();
		if(pGeo)
		{
			pGeo->SetColor(RGB(128,128,128));
			((CGeoCurveBase*)pGeo)->EnableFillColor(TRUE, pLayer->GetColor());
			((CGeoCurveBase*)pGeo)->SetTransparency(m_Transparency);
			pGeo->Draw(pBuf,pDS->GetSymbolDrawScale());
			delete pGeo;
			return;
		}
	}
}

BOOL CVectorView_new::LoadDocData()
{
	m_vectLayL2.ClearAll();
	m_markLayL2.ClearAll();
	m_markLayL2.UpdateMark1();

	if( m_bViewBaseOnImage )
	{			
		Envelope dataBound = GetDocument()->GetDlgDataSource()->GetBound();	
	
		dataBound.Transform(m_lfImgRMatrix);
		dataBound.m_xl*=m_lfMapScaleX;
		dataBound.m_xh*=m_lfMapScaleX;
		dataBound.m_yl*=m_lfMapScaleY;
		dataBound.m_yh*=m_lfMapScaleY;
		m_vectLayL2.SetDataBound(dataBound);
		m_markLayL2.SetDataBound(dataBound);
	}
	else
	{
		Envelope evlp;
		for( int i=m_arrPImgLays.GetSize()-1; i>=0; i--)
		{
			if(m_arrImgPositions[i].is_visible)
			{
				m_laymgrLeft.RemoveDrawingLayer(m_arrPImgLays[i]);
				m_laymgrLeft.InsertDrawingLayer(m_arrPImgLays[i],0);
				
				m_laymgrLeft.OnChangeCoordSys(FALSE);
				
			    evlp.Union(&m_arrPImgLays[i]->GetDataBound());
			}
			else
			{
                 m_laymgrLeft.RemoveDrawingLayer(m_arrPImgLays[i]);		
			}
		}

		Envelope dataBound = GetDocument()->GetDlgDataSource()->GetBound();
		dataBound.m_xl*=m_lfMapScaleX;
		dataBound.m_xh*=m_lfMapScaleX;
		dataBound.m_yl*=m_lfMapScaleY;
		dataBound.m_yh*=m_lfMapScaleY;

		evlp.Union(&dataBound);

		m_vectLayL2.SetDataBound(evlp);
		m_markLayL2.SetDataBound(evlp);
		
		RecalcScrollBar(FALSE);
	}
	UpdateGridDrawing();
	OnUpdate(this,hc_UpdateOverlayBound,NULL);
	OnUpdate(this,hc_UpdateRefFileText,NULL);
	return TRUE;
}

extern BOOL CheckStrZ(LPCTSTR strz, float& z);
void CVectorView_new::UpdateGridDrawing()
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
	m_sGridParam.bViewVect= GetProfileInt(REGPATH_OVERLAYGRID,"ViewVect",FALSE);
	m_sGridParam.bViewImg = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"ViewImg",FALSE);
	m_sGridParam.color = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"Color",RGB(128,128,128));
	
	BOOL bVisible = m_sGridParam.bVisible;
	if( m_sGridParam.dx<=0 || m_sGridParam.dy<=0 )bVisible = FALSE;
	if( m_sGridParam.xr<=0 || m_sGridParam.yr<=0 )bVisible = FALSE;
	
	m_vectLayL2.DelObj(HANDLE_GRID);
	
	m_vectLayL2.ClearAll();
	
	if( !bVisible )
	{
	}
	else
	{
		GrBuffer buf;		
		
		float gridz = 0;
		BOOL bDirectZ = CheckStrZ(m_sGridParam.strz,gridz);
		
		int i, nx = ceil(m_sGridParam.xr/m_sGridParam.dx), ny = ceil(m_sGridParam.yr/m_sGridParam.dy);
		double xr = m_sGridParam.ox+nx*m_sGridParam.dx, yr = m_sGridParam.oy+ny*m_sGridParam.dy, x,y;
		PT_3D pt (0,0,gridz);
		for( x=0, i=0; i<=nx; i++, x+=m_sGridParam.dx )
		{
			buf.BeginLineString(m_sGridParam.color,0);
			pt.x = m_sGridParam.ox+x; pt.y = m_sGridParam.oy;
			buf.MoveTo(&pt);
			
			pt.y = yr;
			buf.LineTo(&pt);
			buf.End();
		}
		
		for( y=0, i=0; i<=ny; i++, y+=m_sGridParam.dy )
		{
			pt.y = m_sGridParam.oy+y; pt.x = m_sGridParam.ox;
			buf.BeginLineString(m_sGridParam.color,0);
			buf.MoveTo(&pt);
			
			pt.x = xr;
			buf.LineTo(&pt);
			buf.End();
		}
		
		AddObjtoVectLay2(HANDLE_GRID,&buf,TRUE);
		m_vectLayL2.ClearAll();
	}

}


void CVectorView_new::AddObjtoVectLay2(LONG_PTR handle, const GrBuffer *pBuf, BOOL bGround, BOOL bUpperMost)
{
	if( !pBuf )return;
	if( !pBuf->HeadGraph() )
		return;	
		
	GrBuffer *pBufL = (GrBuffer*)m_vectLayL2.OpenObj(handle,bUpperMost);
	
	//pBufL->CopyFrom(pBuf);
	pBufL->AddBuffer(pBuf);
	m_vectLayL2.FinishObj(handle);
	m_vectLayL2.ClearAll();		
}

void CVectorView_new::DriveToXyz(PT_3D *pt, BOOL bMoveImg, BOOL bMoveToCenterIfNeed)
{
	PT_4D cltpt, new_gpt, new_cltpt;
	GroundToClient(pt,&cltpt);
	
	CRect rect;
	GetClientRect(&rect);
	int w = rect.Width(), h = rect.Height();
	
	if (bMoveToCenterIfNeed && (bMoveImg || cltpt.x<w / 10 || cltpt.x>w * 9 / 10 || cltpt.y<h / 10 || cltpt.y>h * 9 / 10))
	{
		SetCrossPos(w*0.5+0.5,h*0.5+0.5,w*0.5+0.5);
		Scroll(w*0.5-cltpt.x,h*0.5-cltpt.y,0);	
//		AfxMessageBox("dd");
	}
	else
	{
		SetCrossPos(cltpt.x,cltpt.y,cltpt.x);
		CString str1;
		str1.Format("%f,%f,%f",cltpt.x,cltpt.y,cltpt.x);
		
		UpdateCross();
		FinishUpdateDrawing();
	//	AfxMessageBox(str1);
	}
	
	m_gCurPoint = *pt;
	m_gCur3DMousePoint = m_gCurPoint;
	GetDocument()->SetCoordWnd(GetCoordWnd());

	//显示当前测标点的坐标
	AfxGetMainWnd()->SendMessage(FCCM_SHOW_XYZ,0,LPARAM(&m_gCurPoint));
}


void CVectorView_new::ClientToImage(PT_4D *pt0, PT_4D *pt1)
{
	PT_4D tpt1, tpt2;
	double z1, z2;
	
	tpt1.x = pt0->x*m_lfMatrixLic[0]+pt0->y*m_lfMatrixLic[1]+m_lfMatrixLic[2];
	tpt1.y = pt0->x*m_lfMatrixLic[3]+pt0->y*m_lfMatrixLic[4]+m_lfMatrixLic[5];
	z1 = pt0->x*m_lfMatrixLic[6]+pt0->y*m_lfMatrixLic[7]+m_lfMatrixLic[8];
	tpt2.z = pt0->z;

	tpt1.z = pt0->z*m_lfMatrixLic[0]+pt0->yr*m_lfMatrixLic[1]+m_lfMatrixLic[2];
	tpt1.yr = pt0->z*m_lfMatrixLic[3]+pt0->yr*m_lfMatrixLic[4]+m_lfMatrixLic[5];
	z2 = pt0->z*m_lfMatrixLic[6]+pt0->yr*m_lfMatrixLic[7]+m_lfMatrixLic[8];
	
	pt0 = &tpt1;
	tpt2.x = pt0->x*m_lfRMatrixLic[0]+pt0->y*m_lfRMatrixLic[1]+z1*m_lfRMatrixLic[2];
	tpt2.y = pt0->x*m_lfRMatrixLic[3]+pt0->y*m_lfRMatrixLic[4]+z1*m_lfRMatrixLic[5];
	//tpt2.z = pt0->z*m_lfRMatrixLic[0]+pt0->yr*m_lfRMatrixLic[1]+z2*m_lfRMatrixLic[2];
	tpt2.yr = pt0->z*m_lfRMatrixLic[3]+pt0->yr*m_lfRMatrixLic[4]+z2*m_lfRMatrixLic[5];

	*pt1 = tpt2;
	m_pLeftCS->ClientToGround(pt1,1);
}

void CVectorView_new::GroundToImage(PT_3D *pt0, PT_4D *pt1)
{
	if( !m_bViewBaseOnImage )
	{	
		*pt1 = *pt0;
		(*pt1).x = (*pt0).x*m_lfMapScaleX;
		(*pt1).y = (*pt0).y*m_lfMapScaleY;
	}
	else
	{
		pt1->x = pt0->x*m_lfImgRMatrix[0] + pt0->y*m_lfImgRMatrix[1] + m_lfImgRMatrix[3];
		pt1->y = pt0->x*m_lfImgRMatrix[4] + pt0->y*m_lfImgRMatrix[5] + m_lfImgRMatrix[7];
		pt1->z = pt0->z;
		(*pt1).x*=m_lfMapScaleX;
		(*pt1).y*=m_lfMapScaleY;
	}
	pt1->yr = 0;
	
}

void CVectorView_new::ImageToClient(PT_4D *pt0, PT_4D *pt1)
{
	PT_4D tpt1, tpt2;
	double z1, z2;
	
	tpt1.x = pt0->x*m_lfMatrixLic[0]+pt0->y*m_lfMatrixLic[1]+m_lfMatrixLic[2];
	tpt1.y = pt0->x*m_lfMatrixLic[3]+pt0->y*m_lfMatrixLic[4]+m_lfMatrixLic[5];
	z1 = pt0->x*m_lfMatrixLic[6]+pt0->y*m_lfMatrixLic[7]+m_lfMatrixLic[8];
	tpt2.z = pt0->z;

	tpt1.z = pt0->z*m_lfMatrixLic[0]+pt0->yr*m_lfMatrixLic[1]+m_lfMatrixLic[2];
	tpt1.yr = pt0->z*m_lfMatrixLic[3]+pt0->yr*m_lfMatrixLic[4]+m_lfMatrixLic[5];
	z2 = pt0->z*m_lfMatrixLic[6]+pt0->yr*m_lfMatrixLic[7]+m_lfMatrixLic[8];
	
	pt0 = &tpt1;
	tpt2.x = pt0->x*m_lfRMatrixLic[0]+pt0->y*m_lfRMatrixLic[1]+z1*m_lfRMatrixLic[2];
	tpt2.y = pt0->x*m_lfRMatrixLic[3]+pt0->y*m_lfRMatrixLic[4]+z1*m_lfRMatrixLic[5];
	//tpt2.z = pt0->z*m_lfRMatrixLic[0]+pt0->yr*m_lfRMatrixLic[1]+z2*m_lfRMatrixLic[2];
	tpt2.yr = pt0->z*m_lfRMatrixLic[3]+pt0->yr*m_lfRMatrixLic[4]+z2*m_lfRMatrixLic[5];

	*pt1 = tpt2;
	m_pLeftCS->GroundToClient(pt1,1);
}

void CVectorView_new::ImageToGround(PT_4D *pt0, PT_3D *pt1)
{
	if( !m_bViewBaseOnImage )
	{		
		*pt1 = pt0->To3D();
 		(*pt1).x /= m_lfMapScaleX;
 		(*pt1).y /= m_lfMapScaleY;
	}
	else
	{		
		double x = (*pt0).x/m_lfMapScaleX;
		double y = (*pt0).y/m_lfMapScaleY;		
		pt1->x = x*m_lfImgMatrix[0] + y*m_lfImgMatrix[1] + m_lfImgMatrix[3];
		pt1->y = x*m_lfImgMatrix[4] + y*m_lfImgMatrix[5] + m_lfImgMatrix[7];
		pt1->z = pt0->z;
		
	}
}

void CVectorView_new::GroundToVectorLay(PT_3D *pt0, PT_4D *pt1)
{
	GroundToImage(pt0, pt1);
}

void CVectorView_new::CalcImgSysParams(PT_3D *ptsBound)
{
}


PT_3D CVectorView_new::GetCrossPoint()
{
	if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE )
		return m_gCur3DMousePoint;

	float x,y,xr;
	GetCrossPos(&x,&y,&xr);

	PT_4D pt4d(x,y,0,0);
	PT_3D pt3d;
	ClientToGround(&pt4d,&pt3d);
	pt3d.z = m_gCurPoint.z;
	
	return pt3d;
}

CCoordWnd CVectorView_new::GetCoordWnd()
{
	if( m_pRealCS==NULL )
		m_pRealCS = new CCoordSys();

	m_pRealCS->CopyFrom(m_pLeftCS);
	double m[16];
	Matrix44FromZoom(m_lfMapScaleX,m_lfMapScaleY,1.0,m);
	m_pRealCS->MultiplyRightMatrix(m);
	m_pRealCS->MultiplyRightMatrix(m_lfImgRMatrix);


	return CCoordWnd(m_pRealCS,m_pRealCS,m_gCurPoint,GetCrossPoint(),FALSE);
}

void CVectorView_new::AddConstDragLine(const GrBuffer *pBuf)
{
	CBaseView::AddConstDragLine(pBuf);
}


void CVectorView_new::UpdateConstDragLine()
{
	CBaseView::UpdateConstDragLine();
}


void CVectorView_new::UpdateSelDrawing(BOOL bAlways/* =FALSE */)
{
	m_markLayL2.ClearAll();
	
	m_markLayL2.UpdateMark1();
	
	Invalidate(FALSE);
	
	return;
}

void CVectorView_new::AddObjtoMarkVectLay(CFeature * pFtr)
{

}



void CVectorView_new::ViewAll()
{
	CDlgDoc* pDoc = GetDocument();
	if (!pDoc)	
		return;	
	for(int i=0;i<pDoc->GetDlgDataSourceCount();i++)
	{
		SetDSVisibleState(i,TRUE);
	}
	m_vectLayL2.ClearAll();

	//如果当前视图不激活，说明是从osgb调用的
	if (isVectorViewActive() == false)
	{
		return;
	}

	//向osgbview发送点击显示的按钮
	COsgbView* pView = getOsgbView();

	if (pView != nullptr)
	{
		pView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_TYPE_SHOWORHIDE, 0), 0);
	}

	Invalidate(FALSE);
}

void CVectorView_new::ViewHideAll()
{
	CDlgDoc* pDoc = GetDocument();
	if (!pDoc)	
		return;	
	for(int i=0;i<pDoc->GetDlgDataSourceCount();i++)
	{
		SetDSVisibleState(i,FALSE);
	}
	m_vectLayL2.ClearAll();

	//如果当前视图不激活，说明是从osgb调用的
	if (isVectorViewActive() == false)
	{
		return;
	}

	//向osgbview发送点击隐藏的按钮
	COsgbView* pView = getOsgbView();

	if (pView != nullptr)
	{
		pView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_TYPE_SHOWORHIDE, 0), 0);
	}
	
	Invalidate(FALSE);
}

void CVectorView_new::ViewLocal()
{
	CDlgDoc* pDoc = GetDocument();
	if (!pDoc)	
		return;	

	for (int d=0; d<pDoc->GetDlgDataSourceCount(); d++)
	{
		CDlgDataSource *pDS = pDoc->GetDlgDataSource(d);
		for (int i=0;i<pDS->GetFtrLayerCount();i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if (!pLayer) continue;

			BOOL bViewValue = TRUE;
			m_mapLayerVisible.Lookup(pLayer,bViewValue);
			
			bViewValue &= pLayer->IsVisible();

			if (!bViewValue) continue;	
	
			for(int j=0;j<pLayer->GetObjectCount();j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (pFtr)
				{
					const char *code = pFtr->GetCode();
					BOOL bLocal = (code==NULL || strlen(code)<=0);
					m_vectLayL2.SetObjVisible(LONG_PTR(pFtr),bLocal);
				}
			}
		}
	}

	m_vectLayL2.ClearAll();

	//如果当前视图不激活，说明是从osgb调用的
	if (isVectorViewActive() == false)
	{
		return;
	}

	//向osgbview发送点击隐藏的按钮
	COsgbView* pView = getOsgbView();

	if (pView != nullptr)
	{
		pView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_TYPE_LOCAL, 0), 0);
	}

	Invalidate(FALSE);
}

void CVectorView_new::ViewExternal()
{
	CDlgDoc* pDoc = GetDocument();
	if (!pDoc)	
		return;	
	for (int d=0; d<pDoc->GetDlgDataSourceCount(); d++)
	{
		CDlgDataSource *pDS = pDoc->GetDlgDataSource(d);
		for (int i=0;i<pDS->GetFtrLayerCount();i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if (!pLayer) continue;	

			BOOL bViewValue = TRUE;
			m_mapLayerVisible.Lookup(pLayer,bViewValue);
			
			bViewValue &= pLayer->IsVisible();
			
			if (!bViewValue) continue;	

			for(int j=0;j<pLayer->GetObjectCount();j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (pFtr)
				{
					const char *code = pFtr->GetCode();
					BOOL bLocal = (code==NULL || strlen(code)<=0);
					m_vectLayL2.SetObjVisible(LONG_PTR(pFtr),!bLocal);
				}
			}
		}
	}
	
	m_vectLayL2.ClearAll();

	//如果当前视图不激活，说明是从osgb调用的
	if (isVectorViewActive() == false)
	{
		return;
	}

	//向osgbview发送点击隐藏的按钮
	COsgbView* pView = getOsgbView();

	if (pView != nullptr)
	{
		pView->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_TYPE_EXTERNAL, 0), 0);
	}

	Invalidate(FALSE);
}

void CVectorView_new::OnViewKeypoint() 
{
	m_bShowKeyPoint = !m_bShowKeyPoint;
	
	AfxGetApp()->WriteProfileInt(m_strRegSection,"DisplayKeyPoint",m_bShowKeyPoint);

	{
		m_markLayL2.ClearAll();		
		m_markLayL2.SetMark2Width(1);
		m_markLayL2.SetAllMark2(m_bShowKeyPoint);	
	}
	
	Invalidate(FALSE);
}


void CVectorView_new::SetDSVisibleState(int idx, BOOL bVisible)
{
	CDlgDoc* pDoc = GetDocument();
	if (!pDoc)	
		return;	
	if (idx<0||idx>=pDoc->GetDlgDataSourceCount())
	{
		return;
	}
	CDlgDataSource *pDS = pDoc->GetDlgDataSource(idx);
	CFtrLayer* pLayer = NULL;
	CFeature *pFtr = NULL;
	int j = 0;
	for (int i=0;i<pDS->GetFtrLayerCount();i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer) continue;	
		
		BOOL bViewValue = TRUE;
		m_mapLayerVisible.Lookup(pLayer,bViewValue);
		
		bViewValue &= pLayer->IsVisible();
		
		if (!bViewValue) continue;
		
		for(j=0;j<pLayer->GetObjectCount();j++)
		{
			pFtr = pLayer->GetObject(j);
			if (pFtr)
			{
				m_vectLayL2.SetObjVisible(LONG_PTR(pFtr),bVisible);
			}
		}
	}	
}


void CVectorView_new::OnChangeCoordSys(BOOL bJustScroll)
{
	GetCoordWnd();
	
	m_markLayL2.OnChangeCoordSys(bJustScroll);
	m_vectLayL2.OnChangeCoordSys(bJustScroll);
}


void CVectorView_new::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{	
	if (bActivate)
	{
		AfxGetMainWnd()->SendMessage(FCCM_UPDATEVIEWLIST,0,(LPARAM)this);
	//	AfxBringCallbackToTop(this,NULL);
		AfxGetMainWnd()->SendMessage(FCCM_SETVIEWTYPE,WPARAM(0),LPARAM(m_ViewType));
		
		//////////////////////////////////////////////////////////////////////////
		//by shy
	//	AfxCallMessage(FCCM_REFRESHVECWIN,0,(LPARAM)m_ptBounds);
		AfxGetMainWnd()->SendMessage(FCCM_REFRESHVECWIN,0,(LPARAM)m_ptBounds);
		//////////////////////////////////////////////////////////////////////////

		CDlgDataSource *pDS = GetDocument()->GetDlgDataSource();
		if (pDS)
		{
			SetDataSourceScale(pDS->GetScale());
		}
		
	}
	
	CBaseView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

 
UINT ToolBtnIds2[]={
	IDC_BUTTON_VIEWOVERLAY,
	IDC_BUTTON_ROTATERASTER
};

UINT ToolBtnBmps2[]={
	IDB_BUTTON_VIEWOVERLAY,
	IDB_BUTTON_ROTATERASTER
};

UINT ToolButTips2[] = { 
	IDS_TIPS_VIEWOVERLAY,
	IDS_TIPS_ROTATERASTER
};


int CVectorView_new::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	BOOL bGoodVectView = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,"GoodVectView",TRUE);
	if( !bGoodVectView || !m_pContext->CreateD3DContext(m_hWnd) )
	{
		m_pContext->m_nTextMode = CStereoDrawingContext::textureModeNone;
		m_pContext->m_bCreateStereo  = FALSE;
		m_pContext->m_bCreateOverlay = FALSE;
		m_pContext->SetDisplayMode(CStereoDrawingContext::modeSingle);
	}
	else
	{
		BOOL bCorrectViewDisorder = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_CORRECTVECTDISORDER,FALSE);
		m_pContext->SetD3DGDIPresent(bCorrectViewDisorder);
		
		m_pContext->m_nTextMode = CStereoDrawingContext::textureModeD3D;
		m_pContext->m_bCreateStereo  = FALSE;
		m_pContext->m_bCreateOverlay = FALSE;
		m_pContext->SetDisplayMode(CStereoDrawingContext::modeSingle);
	}

	AfxGetMainWnd()->SendMessage(FCCM_UPDATEVIEWLIST,0,(LPARAM)this);
	
	if (CBaseView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CBitmap bmp;
 	for( int i=0; i<sizeof(ToolBtnIds2)/sizeof(ToolBtnIds2[0]); i++)
 	{
 		bmp.LoadBitmap(ToolBtnBmps2[i]);
 		HBITMAP hBmp = (HBITMAP)bmp;
 		m_ctrlBarHorz.AddButtons(ToolBtnIds2+i,&hBmp,1);
 		bmp.DeleteObject();
 	}
 	
 	for( i=0; i<sizeof(ToolBtnIds2)/sizeof(ToolBtnIds2[0]); i++)
 	{
		CMFCButton *pBtn = (CMFCButton*)m_ctrlBarHorz.GetButton(ToolBtnIds2[i]);
 		if( pBtn )
 		{
 			CString str;
 			str.LoadString(ToolButTips2[i]);
 			pBtn->SetTooltip(str);
 		}
 	}

// 	RegisterHotKey(this->m_hWnd,ID_VECTOR_ZOOMIN,MOD_CONTROL,VK_F3);    
// 	RegisterHotKey(this->m_hWnd,ID_VECTOR_ZOOMOUT,MOD_CONTROL,VK_F4);
// 	RegisterHotKey(this->m_hWnd,ID_VECTOR_ZOOMFIT,MOD_CONTROL,VK_F5);	
	
	return 0;
}

void CVectorView_new::OnDestroy()
{
	for( int i=0; i<m_arrPImgLays.GetSize(); i++)
	{
		m_laymgrLeft.RemoveDrawingLayer(m_arrPImgLays[i]);
		delete m_arrPImgLays[i];
	}

	m_arrPImgLays.RemoveAll();

	m_vectLayL2.Destroy();
	m_vectLayL2.SetContext(NULL);
	
	m_markLayL2.Destroy();
	m_markLayL2.SetContext(NULL);

	AfxGetMainWnd()->SendMessage(FCCM_UPDATEVIEWLIST,1,(LPARAM)this);
	AfxGetMainWnd()->SendMessage(FCCM_REFRESHVECFLAG,0,0);
	
// 	UnregisterHotKey(this->m_hWnd,ID_VECTOR_ZOOMIN);
// 	UnregisterHotKey(this->m_hWnd,ID_VECTOR_ZOOMOUT);
// 	UnregisterHotKey(this->m_hWnd,ID_VECTOR_ZOOMFIT);
    
	m_arrImgPositions.RemoveAll();
	gpMainWnd->m_wndFtrLayerView.UpdateLayerView();

	CBaseView::OnDestroy();
}

void CVectorView_new::OnMouseLeave()
{	
	 CBaseView::OnMouseLeave();

	// 鼠标在COsgbView中时显示指针
	 COsgbView * pOSGBView = getOsgbView();
	 if (pOSGBView && pOSGBView->MouseIn())
		 EnableCrossVisible(TRUE);
//	 m_cursorL.Erase();
//	 m_cursorR.Erase();
//	 EnableCrossVisible(FALSE);
//   ResetCursorDrawing();
}

float CVectorView_new::ZoomSelect(float wantZoom)
{
	if(m_bUseAnyZoomrate)
		return wantZoom;

	if( m_nInnerCmd==IDC_BUTTON_ZOOMIN || m_nInnerCmd==IDC_BUTTON_ZOOMOUT )
	{
		float zoom = GetComboNextZoom(m_nInnerCmd==IDC_BUTTON_ZOOMIN?1:0);
		if( zoom>0 )return zoom;
	}
	return CSceneView::ZoomSelect(wantZoom);
}


void CVectorView_new::SetImagePosition(int index)
{
	if( index<0 || index>=m_arrImgPositions.GetSize() )
		return;

	ViewImgPosition item = m_arrImgPositions[index];

	if( !m_bViewBaseOnImage )
	{
		//计算影像的四个角点坐标
		if( item.nType==ViewImgPosition::ImageType )
		{
			PT_3D pts[4];
			CSize szImg = m_arrPImgLays[index]->GetImageSize();			
			
			if( item.lfImgMatrix[3]<0 )
			{
				pts[0].x = 0; pts[0].y = szImg.cy;
				pts[1].x = szImg.cx; pts[1].y = szImg.cy;
				pts[2].x = szImg.cx; pts[2].y = 0;
				pts[3].x = 0; pts[3].y = 0;
			}
			else
			{
				pts[0].x = 0; pts[0].y = 0;
				pts[1].x = szImg.cx; pts[1].y = 0;
				pts[2].x = szImg.cx; pts[2].y = szImg.cy;
				pts[3].x = 0; pts[3].y = szImg.cy;
			}
			
			for( int i=0; i<4; i++)
			{
				double x, y;
				x = item.lfImgLayOX + pts[i].x*item.lfImgMatrix[0] + pts[i].y*item.lfImgMatrix[1];
				y = item.lfImgLayOY + pts[i].x*item.lfImgMatrix[2] + pts[i].y*item.lfImgMatrix[3];
				
				pts[i].x = x; pts[i].y = y;
			}
			
			m_arrPImgLays[index]->SetCornerPts(TRUE,pts);
		}
		else
		{
			m_arrPImgLays[index]->SetTransform(item.lfDataMatrix);
		}
	}
	else
	{
		m_arrPImgLays[index]->SetCornerPts(m_nRasterAng,1.0,1.0);

		item.ConvertToMatrixType();
		
		double m[16] = {
			item.lfDataMatrix[0],item.lfDataMatrix[1],0,item.lfDataMatrix[2],
			item.lfDataMatrix[3],item.lfDataMatrix[4],0,item.lfDataMatrix[5],
			0,0,1,0,
			0,0,0,1
		};

		memcpy(m_lfImgMatrix,m,sizeof(m));
		matrix_reverse(m,4,m_lfImgRMatrix);

		m_gsa.lfGKX = m_gsa.lfGKY = sqrt(fabs(m_lfImgRMatrix[0]*m_lfImgRMatrix[5] - m_lfImgRMatrix[1]*m_lfImgRMatrix[4]));
	}
}


extern int SM_DomDecryption(LPCTSTR lpszDom, TCHAR szNewDomPath[_MAX_PATH]);
int SM_DomDecryption(CString& dom)
{
	char newPath[500] = {0};

	CString old = dom;
	int ret = SM_DomDecryption(old,newPath);
	if(ret>0 && strlen(newPath)>0)
		dom = newPath;

	return ret;
}


CRasterLayer *CVectorView_new::GetRasterLayer(LPCTSTR fileName, int& index)
{
	for( int i=0; i<m_arrImgPositions.GetSize(); i++)
	{
		if( stricmp(m_arrImgPositions[i].fileName,fileName)==0  )
		{
			index = i;
			return m_arrPImgLays[i];
		}
	}

	index = -1;
	return NULL;
}

CRasterLayer *CVectorView_new::LoadRasterLayer(LPCTSTR fileName)
{
	CString strFileName = CString(fileName) + _T(".ei");
	if( _access(strFileName,0)!=-1 )
		;
	else if(_access(fileName,0)==-1)
	{
		CString strMsg;
		strMsg.FormatMessage( IDS_FILE_OPEN_ERR,fileName);
		AfxMessageBox(strMsg);
		return NULL;
	}
	else
		strFileName = fileName;

	SM_DomDecryption(strFileName);

	CRasterLayer *pLayer = new CRasterLayer();
		
	pLayer->SetContext(m_pContext->GetLeftContext());
	pLayer->m_bForceDraw = FALSE;

	int nMem = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MEMNUM,200);
	nMem /= 2;
	int imgbufsize = nMem;
	
	if( pLayer->InitCache(GetBestCacheTypeForLayer(pLayer),
		CSize(512,512),100)==FALSE ||
		!pLayer->AttachImage(strFileName) )
	{
		CString strMsg;
		strMsg.FormatMessage( IDS_FILE_OPEN_ERR,fileName);
		AfxMessageBox(strMsg);
		if(pLayer) delete pLayer;
		return NULL;
	}

	if(pLayer->GetCacheType()==CHEDATA_TYPE_D3DTEXT)
	{
		((CCacheD3DTextData*)pLayer->m_pCacheData)->m_bUseLinearZoom = !m_bImageZoomNotLinear;
	}
	else if(pLayer->GetCacheType()==CHEDATA_TYPE_GLTEXT)
	{
		((CCacheGLTextData*)pLayer->m_pCacheData)->m_bUseLinear = !m_bImageZoomNotLinear;
	}

	pLayer->m_bNotClearCacheOnZoom = FALSE;

	return pLayer;
}

void CVectorView_new::ReadRefImageInfo()
{
	CDlgDoc *pDoc = GetDocument();
	if (!pDoc) return;
	
	CString infofile = pDoc->GetPathName() + _T(".imageref");
	
	FILE *fp = fopen(infofile,"r");
	if( !fp )return;
	
	char line[1024];
	CString text;
	while( !feof(fp) )
	{
		ViewImgPosition item;
		int num = fscanf(fp, "%lf %lf %lf %lf %lf %lf %[^\n]\n",&item.lfImgLayOX,&item.lfImgLayOY,
			&item.lfImgMatrix[0],&item.lfImgMatrix[1],&item.lfImgMatrix[2],&item.lfImgMatrix[3],item.fileName);

		if (num == 7)
		{
			m_arrImgPositions.Add(item);
			item.is_visible = TRUE;
		}
		
	}
	fclose(fp);

	if (m_arrImgPositions.GetSize() > 0)
	{
		//添加需要加载的影像
		for( int i=0; i<m_arrImgPositions.GetSize(); i++)
		{
			CString fileName = m_arrImgPositions[i].fileName;
			
			ViewImgPosition item = m_arrImgPositions[i];
			
			CRasterLayer *pLayer = LoadRasterLayer(item.fileName);
			if( pLayer )
			{
				item.szImg = pLayer->GetImageSize();
				m_arrPImgLays.InsertAt(i,pLayer);
				
				SetImagePosition(i);
			}
		}
		
		//保存参数
		//m_bOverlayImgLay = dlg.m_bVisible;
		m_bOverlayImgLay = FALSE;
		for(i=0;i<m_arrImgPositions.GetSize();++i)
		{
			if(m_arrImgPositions[i].is_visible)
			{
				m_bOverlayImgLay = TRUE;
				break;
			}
		}
		
		//更新视图
		if( m_bOverlayImgLay )
		{
			m_lfMapScaleX = 1.0;
			m_lfMapScaleY = 1.0;
		}
		
		if( m_arrPImgLays.GetSize()<=0 )
		{
			DWORD dwScale = ((CDlgDoc*)GetWorker())->GetDlgDataSource()->GetScale();
			//考虑显示设备的Ppi ,1英寸=2.54cm
			int nXPpi = ::GetDeviceCaps(m_hViewDC,LOGPIXELSX);
			int nYPpi = ::GetDeviceCaps(m_hViewDC,LOGPIXELSY);
			//当前比例尺下1m对应多少像素
			m_lfMapScaleX = ((100.0*nXPpi)/(dwScale*2.54));
			m_lfMapScaleY = ((100.0*nYPpi)/(dwScale*2.54));
			m_gsa.lfGKX = m_lfMapScaleX;
			m_gsa.lfGKY = m_lfMapScaleY;
			m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
			m_pContext->m_fDrawCellKY = m_gsa.lfGKY;
			m_pContext->m_fDrawCellAngle = -m_gsa.lfAngle;
			m_pContext->GetLeftContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
			m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
			m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;
		}
		else
		{
			m_pContext->m_fDrawCellAngle = -m_gsa.lfAngle;
			m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
			m_pContext->m_fDrawCellKY = m_gsa.lfGKY;
			
			m_pContext->GetLeftContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
			m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
			m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;
		}
		
		RefreshImageOverlay();
		
		ReadAdjustParams();
	    SetAdjustParams();

		OnZoomFit();
	}
	
}


void CVectorView_new::SaveRefImageInfo()
{
	CDlgDoc *pDoc = GetDocument();
	if (!pDoc) return;

	CString infofile = pDoc->GetPathName() + _T(".imageref");

	FILE *fp = fopen(infofile,_T("w"));
	if( !fp )return;

	for (int i=0; i<m_arrImgPositions.GetSize(); i++)
	{
		ViewImgPosition item = m_arrImgPositions[i];
		if (item.nType == 0)
		{
			fprintf(fp, "%.6f %.6f %.6f %.6f %.6f %.6f %s\n",item.lfImgLayOX,item.lfImgLayOY,
				item.lfImgMatrix[0],item.lfImgMatrix[1],item.lfImgMatrix[2],item.lfImgMatrix[3],(LPCTSTR)item.fileName);
			CString tifrefname = (LPCTSTR)item.fileName;
			if( tifrefname.Right(3).CompareNoCase(_T("tif"))==0 )
			{
				tifrefname = tifrefname.Left(tifrefname.GetLength()-3) + _T("tifref");
			}
			else
				tifrefname += _T(".tifref");
			FILE *tifref = fopen(tifrefname,_T("wt"));
			if( fp!=NULL )
			{
				double v[6];
				v[0] = item.lfImgMatrix[0];
				v[1] = item.lfImgMatrix[1];
				v[2] = item.lfImgMatrix[2];
				v[3] = item.lfImgMatrix[3];
				v[4] = item.lfImgLayOX;
				v[5] = item.lfImgLayOY;
				fprintf(tifref,"%.6f\n%.6f\n%.6f\n%.6f\n%.6f\n%.6f\n",
					v[0],v[1],v[2],v[3],v[4],v[5]);
				fclose(tifref);
			}
		}
		else if (item.nType == 1)
		{
			fprintf(fp, "%.6f %.6f %.6f %.6f %.6f %.6f %s\n",item.lfDataMatrix[2],item.lfDataMatrix[5],
				item.lfDataMatrix[0],item.lfDataMatrix[1],item.lfDataMatrix[3],item.lfDataMatrix[4],(LPCTSTR)item.fileName);	
			CString tifrefname = (LPCTSTR)item.fileName;
			if( tifrefname.Right(3).CompareNoCase(_T("tif"))==0 )
			{
				tifrefname = tifrefname.Left(tifrefname.GetLength()-3) + _T("tifref");
			}
			else
				tifrefname += _T(".tifref");
		
			FILE *tifref = fopen(tifrefname,_T("wt"));
			if( fp!=NULL )
			{
				double v[6];
				v[0] = item.lfDataMatrix[0];
				v[1] = item.lfDataMatrix[1];
				v[2] = item.lfDataMatrix[3];
				v[3] = item.lfDataMatrix[4];
				v[4] = item.lfDataMatrix[2];
				v[5] = item.lfDataMatrix[5];
				fprintf(tifref,"%.6f\n%.6f\n%.6f\n%.6f\n%.6f\n%.6f\n",
					v[0],v[1],v[2],v[3],v[4],v[5]);
				fclose(tifref);
			}
		}	
	}
	
	fclose(fp);

	if (m_arrImgPositions.GetSize() <= 0) 
	{
		::DeleteFile(infofile);
	}
}

void CVectorView_new::UpdateOverlayImage(CArray<ViewImgPosition,ViewImgPosition>& img_pos)
{
	CString strMsg = _T("");
	for (int i=0;i<img_pos.GetSize();i++)
	{
		CString strFileName = CString(img_pos[i].fileName) + _T(".ei");
		if( _access(strFileName,0)==-1  && _access(img_pos[i].fileName,0)==-1)
		{
			CString str;
			str.FormatMessage( IDS_FILE_OPEN_ERR,img_pos[i].fileName);
			str+=_T("\n");
			strMsg+=str;
		}
	}
	if (!strMsg.IsEmpty())
	{
		AfxMessageBox(strMsg);
	}

	CVectorViewCenterPoint ptSave(this);

	//清除需要卸载的影像
	int j;
	int nSize = img_pos.GetSize();

	for( j=m_arrImgPositions.GetSize()-1; j>=0; j--)
	{
		ViewImgPosition item = m_arrImgPositions[j];
		for( i=0; i<nSize; i++)
		{
			if( stricmp(img_pos[i].fileName,item.fileName)==0 )
			{				
				break;
			}
		}

		if( i>=nSize )
		{
			m_arrImgPositions.RemoveAt(j);
			if(j<m_arrPImgLays.GetSize() && m_arrPImgLays[j])
			{
				m_laymgrLeft.RemoveDrawingLayer(m_arrPImgLays[j]);
				delete m_arrPImgLays[j];
				m_arrPImgLays.RemoveAt(j);
			}
		}
	}

	//添加需要加载的影像

	int index = 0;
	for( i=0; i<nSize; i++)
	{
		CString fileName = img_pos[i].fileName;

		for( j=0; j<m_arrImgPositions.GetSize(); j++)
		{				
			if( fileName.CompareNoCase(m_arrImgPositions[j].fileName)==0 )
			{
				ViewImgPosition item = img_pos[i];
				m_arrImgPositions.RemoveAt(j);
				m_arrImgPositions.InsertAt(index,item);

				CRasterLayer *pLayer = m_arrPImgLays[j];
				m_arrPImgLays.RemoveAt(j);
				m_arrPImgLays.InsertAt(index,pLayer);

				SetImagePosition(index);	

				index++;

				break;
			}
		}

		if( j>=m_arrImgPositions.GetSize() )
		{
			ViewImgPosition item = img_pos[i];

			CRasterLayer *pLayer = LoadRasterLayer(item.fileName);
			if( pLayer )
			{
				item.szImg = pLayer->GetImageSize();

				m_arrImgPositions.InsertAt(index,item);
				m_arrPImgLays.InsertAt(index,pLayer);

				SetImagePosition(index);

				index++;
			}
		}
	}
	
	//保存参数
 	//m_bOverlayImgLay = dlg.m_bVisible;
	m_bOverlayImgLay = FALSE;
	for(i=0;i<m_arrImgPositions.GetSize();++i)
	{
		if(m_arrImgPositions[i].is_visible)
		{
			m_bOverlayImgLay = TRUE;
			break;
		}
	}
	
	//更新视图
	if( m_bOverlayImgLay )
	{
		m_lfMapScaleX = 1.0;
		m_lfMapScaleY = 1.0;
	}

	if( m_arrPImgLays.GetSize()<=0 )
	{
		DWORD dwScale = ((CDlgDoc*)GetWorker())->GetDlgDataSource()->GetScale();
		//考虑显示设备的Ppi ,1英寸=2.54cm
		int nXPpi = ::GetDeviceCaps(m_hViewDC,LOGPIXELSX);
		int nYPpi = ::GetDeviceCaps(m_hViewDC,LOGPIXELSY);
		//当前比例尺下1m对应多少像素
		m_lfMapScaleX = ((100.0*nXPpi)/(dwScale*2.54));
		m_lfMapScaleY = ((100.0*nYPpi)/(dwScale*2.54));
		m_gsa.lfGKX = m_lfMapScaleX;
		m_gsa.lfGKY = m_lfMapScaleY;
		m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
		m_pContext->m_fDrawCellKY = m_gsa.lfGKY;
		m_pContext->m_fDrawCellAngle = -m_gsa.lfAngle;
		m_pContext->GetLeftContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
		m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
		m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;
	}
	else
	{
		m_pContext->m_fDrawCellAngle = -m_gsa.lfAngle;
		m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
		m_pContext->m_fDrawCellKY = m_gsa.lfGKY;
		
		m_pContext->GetLeftContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
		m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
		m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;
	}

	RefreshImageOverlay();
	
	ReadAdjustParams();
	SetAdjustParams();

	SaveRefImageInfo();
}

void CVectorView_new::OnOverlayRaster() 
{
	CDlgOverMutiImgLay dlg;
	
	CDlgDoc *pDoc = GetDocument();
	memcpy(&dlg.m_ptsBound,m_ptBounds,4*sizeof(PT_3D));

	//用已有的参数初始化
	dlg.m_bVisible = m_bOverlayImgLay;
	
	if (!m_bOverlayImgLay)
	{
		m_fOldDrawCellKX = m_gsa.lfGKX;
		m_fOldDrawCellKY = m_gsa.lfGKY;
	}
	//其它参数
	dlg.m_arrImgPos.Copy(m_arrImgPositions);
	
	if( dlg.DoModal()!=IDOK )return;

	UpdateOverlayImage(dlg.m_arrImgPos);	

	//
	gpMainWnd->m_wndFtrLayerView.Refresh();

}


Envelope CVectorView_new::GetImgLayBound()
{
	if( !m_bOverlayImgLay || m_arrPImgLays.GetSize()<=0 )
		return Envelope();
	
	//计算影像的四个角点坐标
	
	Envelope e;
	for( int i=0; i<m_arrPImgLays.GetSize(); i++)
	{
		e.Union(&m_arrPImgLays[i]->GetDataBound());
	}

	return e;
}

void CVectorView_new::RefreshImageOverlay()
{
	if( m_bOverlayImgLay )
	{
		Envelope evlp;
		for( int i=m_arrPImgLays.GetSize()-1; i>=0; i--)
		{
			if(m_arrImgPositions[i].is_visible)
			{
				m_laymgrLeft.RemoveDrawingLayer(m_arrPImgLays[i]);
				m_laymgrLeft.InsertDrawingLayer(m_arrPImgLays[i],0);
				
				m_laymgrLeft.OnChangeCoordSys(FALSE);
				
			    evlp.Union(&m_arrPImgLays[i]->GetDataBound());
			}
			else
			{
                 m_laymgrLeft.RemoveDrawingLayer(m_arrPImgLays[i]);		
			}	
		}

		Envelope dataBound = GetDocument()->GetDlgDataSource()->GetBound();
		dataBound.m_xl*=m_lfMapScaleX;
		dataBound.m_xh*=m_lfMapScaleX;
		dataBound.m_yl*=m_lfMapScaleY;
		dataBound.m_yh*=m_lfMapScaleY;

		evlp.Union(&dataBound);

		m_vectLayL2.SetDataBound(evlp);
		m_markLayL2.SetDataBound(evlp);
		
		if( m_bViewBaseOnImage )
		{
			OnUpdate(this,hc_UpdateAllObjects,NULL);
		}
		else
		{
			RecalcScrollBar(FALSE);
		}
		
		Invalidate(FALSE);
	}
	else
	{
		for( int i=m_arrPImgLays.GetSize()-1; i>=0; i--)
		{
			m_laymgrLeft.RemoveDrawingLayer(m_arrPImgLays[i]);			
		}

		Envelope dataBound = GetDocument()->GetDlgDataSource()->GetBound();
		dataBound.m_xl*=m_lfMapScaleX;
		dataBound.m_xh*=m_lfMapScaleX;
		dataBound.m_yl*=m_lfMapScaleY;
		dataBound.m_yh*=m_lfMapScaleY;

		m_vectLayL2.SetDataBound(dataBound);
		m_markLayL2.SetDataBound(dataBound);

		RecalcScrollBar(FALSE);

		Invalidate(FALSE);
	}
}

void CVectorView_new::OnRefdem() 
{
	CString filter(StrFromResID(IDS_LOADDEM_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	//clear old info
	m_dem.Close();
	m_strDemPath.Empty();
	
	if( !m_dem.Open(dlg.GetPathName()) )
		return;

	m_strDemPath = dlg.GetPathName();
}

void CVectorView_new::OnUnrefdem() 
{
	//clear old info
	m_dem.Close();
	
	m_strDemPath.Empty();
}

BOOL CVectorView_new::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( IsDrawStreamline() )
	{
		CPoint pt;
		::GetCursorPos(&pt);
		pt.x = (short)pt.x; pt.y = (short)pt.y;
		::ScreenToClient(m_hWnd,&pt);
		
		if( pt==m_ptLastMouse )return TRUE;
		
		HCURSOR hCursor = ::LoadCursor(NULL,IDC_ARROW);
		if( hCursor )
		{
			::SetCursor(hCursor);
		}
		return TRUE;
	}
	if (m_nInnerCmd == ID_TOOL_MANUALLOADVECT)
	{
		HCURSOR hCursor = ::LoadCursor(NULL,IDC_ARROW);
		if( hCursor )
		{
			::SetCursor(hCursor);
		}
		return TRUE;
	}
	return CBaseView::OnSetCursor(pWnd, nHitTest, message);
}

void CVectorView_new::OnMoveCursorToCenter() 
{
	OnMoveCursorToCenter2(0,0);	
}

void CVectorView_new::OnViewRotateAsStereo() 
{
	BOOL bRotate = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,"RotateAsStereo",0);
	bRotate = !bRotate;

	if( bRotate )
		ViewRotateAsStereo();
	else
		OnViewReset();

	AfxGetApp()->WriteProfileInt(REGPATH_VIEW_VECT,"RotateAsStereo",bRotate);
}

void CVectorView_new::OnUpdateViewRotateAsStereo(CCmdUI* pCmdUI)
{
	BOOL bRotate = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,"RotateAsStereo",0);
	pCmdUI->SetCheck(bRotate?1:0);
}

extern CStereoView *GetStereoView(int flag);

void CVectorView_new::ViewRotateAsStereo() 
{
	CStereoView *pStereoView=GetStereoView(2);

	if (!pStereoView) return;

	double ang = pStereoView->GetViewScaleAngle().lfAngle;

// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = ID_VIEW_ROTATEASSTEREO;
	OnPreInnerCmd(ID_VIEW_ROTATEASSTEREO);
	
	//客户中心作为焦点，位置不变，这里先记录此位置
	CRect rcClient;
	GetClientRect(&rcClient);
	CPoint cpt = rcClient.CenterPoint();
	
	PT_4D fcpt;
	PT_3D fgpt; 
	fcpt.x = cpt.x;  fcpt.y = cpt.y; fcpt.z = 0;
	ClientToGround(&fcpt,&fgpt);
	double ang0 = ang-m_gsa.lfAngle;
	//恢复坐标系
	m_gsa.lfAngle /*= m_lfAngle*/ = ang;

	double m1[16];
	Matrix44FromRotate(&fgpt,&PT_3D(0,0,1),/*m_lfAngle*/ang0,m1);
	
	m_pLeftCS->MultiplyMatrix(m1);
	OnChangeCoordSys(FALSE);	
	AdjustCellScale();
	
	OnPostInnerCmd();

	RecalcScrollBar(FALSE);
	
	//移动到焦点位置
	DriveToXyz(&fgpt,TRUE);
	
	Invalidate(FALSE);
	
}

void CVectorView_new::OnViewReset() 
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = ID_VIEW_RESET;
	OnPreInnerCmd(ID_VIEW_RESET);

	//客户中心作为焦点，位置不变，这里先记录此位置
	CRect rcClient;
	GetClientRect(&rcClient);
	CPoint cpt = rcClient.CenterPoint();

	PT_4D fcpt;
	PT_3D fgpt; 
	fcpt.x = cpt.x;  fcpt.y = cpt.y; fcpt.z = 0;
	ClientToGround(&fcpt,&fgpt);
		
	double m1[16];
	Matrix44FromRotate(&fgpt,&PT_3D(0,0,1),/*-m_lfAngle*/-m_gsa.lfAngle,m1);
	m_gsa.lfAngle = 0.0;

	m_pLeftCS->MultiplyMatrix(m1);
	OnChangeCoordSys(FALSE);								
	AdjustCellScale();

	Envelope envp;
	envp.CreateFromPts(m_ptBounds,4);	

	OnPostInnerCmd();

	RecalcScrollBar(FALSE);

	//移动到焦点位置
	DriveToXyz(&fgpt,TRUE);
	
	Invalidate(FALSE);


	BOOL bRotate = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,"RotateAsStereo",0);
	if (bRotate)
	{
		bRotate = !bRotate;		
		AfxGetApp()->WriteProfileInt(REGPATH_VIEW_VECT,"RotateAsStereo",bRotate);
	}
	
}

void CVectorView_new::OnManulLoadVect() 
{
//	m_nInnerCmd = ID_TOOL_MANUALLOADVECT;
	m_bStartLoadVect = FALSE;
	
	OnPreInnerCmd(ID_TOOL_MANUALLOADVECT);
}

void CVectorView_new::OnUpdateManulLoadVect(CCmdUI* pCmdUI) 
{
	BOOL bManualLoad = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MANUALLOADVECT,FALSE);
	pCmdUI->Enable( bManualLoad );
}



void CVectorView_new::OnMeasureImagePosition()
{
	OnCommandType(ID_RASTER_POSITION);
}

void CVectorView_new::OnDisplayByheight() 
{
	m_bDisplayContoursSpecially = FALSE;
	m_bDisplayByHeight = !m_bDisplayByHeight;

	CDlgDoc *pDoc = GetDocument();
	if( !pDoc )return;

	static int bCreateColorMap = 0;
	static COLORREF colorMaps[1024];

	if( m_bDisplayByHeight )
	{
		//建立渐变颜色表
		if( bCreateColorMap==0 )
		{
			for( int i=0; i<256; i++)
			{
				//蓝色渐变到青色
				colorMaps[i] = RGB(0,i,255-i);
				
				//青色渐变到绿色
//				colorMaps[256+i] = RGB(0,255,255-i);
				
				//绿色渐变到黄色
				colorMaps[256+i] = RGB(i,255,0);
				
				//黄色渐变到红色
				colorMaps[512+i] = RGB(255,255-i,0);

				//红色渐变到紫色
				colorMaps[768+i] = RGB(255,0,i);
			}
			
			bCreateColorMap = 1;
		}

		//计算高程范围
		Envelope e;

		CFeature *pFtr;
		
		CDlgDataSource *pDS = NULL;
		
		int nDSCount = pDoc->GetDlgDataSourceCount();
		
		
		for( int k=0; k<nDSCount; k++)
		{
			pDS = pDoc->GetDlgDataSource(k);
			if(!pDS) continue;
			e.Union(&pDS->GetBound(),3);			
		}		
		
		if( e.m_zh>e.m_zl )
		{
			//根据高程设置色谱
			double lfk = 1024/(e.m_zh-e.m_zl);
			for( int k=0; k<nDSCount; k++)
			{
				pDS = pDoc->GetDlgDataSource(k);
				if(!pDS) continue;
				int nlaynum = pDS->GetFtrLayerCount();
				
				for( int i=0; i<nlaynum; i++)
				{
					CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
					if( !pLayer )continue;				
					
					
					int nObjs = pLayer->GetObjectCount();
					for( int j=0; j<nObjs; j++)
					{
						pFtr = pLayer->GetObject(j);
						if( !pFtr )continue;

						if( pFtr->GetGeometry()->GetDataPointSum()<=0 )continue;

						PT_3DEX expt;
						expt = pFtr->GetGeometry()->GetDataPoint(0);

						int idx = lfk * (expt.z-e.m_zl);
						if( idx<0 )idx =0;
						else if( idx>1023 )idx = 1023;

						m_vectLayL2.SetObjColor((LONG_PTR)pFtr, colorMaps[idx]);
					}					
					
				}
			}
		}
	}
	else
	{
		//将颜色设回去	
		
		CFeature *pFtr;

		CDlgDataSource *pDS = NULL;
		
		int nDSCount = pDoc->GetDlgDataSourceCount();
		
		for( int k=0; k<nDSCount; k++)
		{
			pDS = pDoc->GetDlgDataSource(k);
			if(!pDS) continue;
			int nlaynum = pDS->GetFtrLayerCount();
			
			for( int i=0; i<nlaynum; i++)
			{
				CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
				if( !pLayer )continue;
				
				int nObjs = pLayer->GetObjectCount();
				for( int j=0; j<nObjs; j++)
				{
					pFtr = pLayer->GetObject(j);
					if( !pFtr )continue;

					if( pFtr->GetGeometry()->GetDataPointSum()<=0 )continue;
					long color = pFtr->GetGeometry()->GetColor();
					if(color==-1)
						color = pLayer->GetColor();
					m_vectLayL2.SetObjColor((LONG_PTR)pFtr, color);
				}				
			}
		}
	}
	
	m_vectLayL2.ClearAll();
	
	Invalidate(FALSE);
	
	
}

void CVectorView_new::OnUpdateDisplayByheight(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bDisplayByHeight);
}

void CVectorView_new::OnDisplayContoursSpecially() 
{
	m_bDisplayContoursSpecially = !m_bDisplayContoursSpecially;

	CDlgDoc *pDoc = GetDocument();
	if( !pDoc )return;

	if( m_bDisplayContoursSpecially )
	{
		CDlgDisplayContoursSpecially dlg;
		if( dlg.DoModal()!=IDOK || dlg.m_fInterval<=0 || dlg.m_nInterNum<=0 )
		{
			m_bDisplayContoursSpecially = FALSE;
			return;
		}

		//调整颜色	
		
		CFeature *pFtr;
		
		CDlgDataSource *pDS = NULL;
		
		int nDSCount = pDoc->GetDlgDataSourceCount();
		
		for( int k=0; k<nDSCount; k++)
		{
			pDS = pDoc->GetDlgDataSource(k);
			if(!pDS)continue;
			int nlaynum = pDS->GetFtrLayerCount();
			
			for( int i=0; i<nlaynum; i++)
			{
				CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
				if( !pLayer )continue;				
				
				CString strLayerName = pLayer->GetName();
				if (strLayerName.CompareNoCase(StrFromResID(IDS_STD_CONTOUR)) != 0 
					&& strLayerName.CompareNoCase(StrFromResID(IDS_INDEX_CONTOUR)) != 0
					&& strLayerName.CompareNoCase(StrFromResID(IDS_INTER_CONTOUR)) != 0
					)
				{
					continue;
				}

				int nObjs = pLayer->GetObjectCount();
				for( int j=0; j<nObjs; j++)
				{
					pFtr = pLayer->GetObject(j);
					if( !pFtr )continue;

					if( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || pFtr->GetGeometry()->GetDataPointSum()<=2 )
						continue;

					if( !CModifyZCommand::CheckObjForContour(pFtr->GetGeometry()) )
						continue;

					PT_3DEX expt;
					expt = pFtr->GetGeometry()->GetDataPoint(0);

					int n = (int)floor(expt.z/dlg.m_fInterval);
					int n0 = (int)ceil(expt.z/dlg.m_fInterval);
					if( fabs(expt.z-n*dlg.m_fInterval)>1e-4&&fabs(expt.z-n0*dlg.m_fInterval)>1e-4 )
						continue;
					if( fabs(expt.z-n0*dlg.m_fInterval)<=1e-4&&!(fabs(expt.z-n*dlg.m_fInterval)<=1e-4))
						n = n0;
					n = (n%(dlg.m_nInterNum+1));

					COLORREF clr = 0;
					if( n==0 )clr = dlg.m_clr0;
					else if( n==1 )clr = dlg.m_clr1;
					else if( n==2 )clr = dlg.m_clr2;
					else if( n==3 )clr = dlg.m_clr3;
					else if( n==4 )clr = dlg.m_clr4;
					else continue;
				
					m_vectLayL2.SetObjColor((LONG_PTR)pFtr, clr);
				}				
			}
		}
	}
	else
	{
		//将颜色设回去	
		
		CFeature *pFtr;
		
		CDlgDataSource *pDS = NULL;
		
		int nDSCount = pDoc->GetDlgDataSourceCount();
		
		for( int k=0; k<nDSCount; k++)
		{
			pDS = pDoc->GetDlgDataSource(k);
			if(!pDS) continue;
			int nlaynum = pDS->GetFtrLayerCount();
			
			for( int i=0; i<nlaynum; i++)
			{
				CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
				if( !pLayer )continue;
				
				int nObjs = pLayer->GetObjectCount();
				for( int j=0; j<nObjs; j++)
				{
					pFtr = pLayer->GetObject(j);
					if( !pFtr )continue;
					
					if( pFtr->GetGeometry()->GetDataPointSum()<=0 )continue;
					long color = pFtr->GetGeometry()->GetColor();
					if(color==-1)
						color = pLayer->GetColor();
					m_vectLayL2.SetObjColor((LONG_PTR)pFtr, color);
				}				
			}
		}
	}
	
	m_vectLayL2.ClearAll();
	
	Invalidate(FALSE);

	m_bDisplayByHeight = FALSE;
}

void CVectorView_new::OnUpdateDisplayContoursSpecially(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bDisplayContoursSpecially);	
}



//角度 ang 为顺时针角度，计算从旋转后的影像坐标到无旋转的影像坐标的矩阵
static void MatrixRotateImage(CSize szImg, float ang, double *m)
{
	matrix_toIdentity(m,3);
	if( ang==90.0 )
	{
		m[0] = 0; m[1] = 1; m[2] = 0; 
		m[3] = -1; m[4] = 0; m[5] = szImg.cx; 
	}
	else if( ang==180.0 )
	{
		m[0] = -1; m[1] = 0; m[2] = szImg.cx; 
		m[3] = 0; m[4] = -1; m[5] = szImg.cy; 
	}
	else if( ang==270.0 )
	{
		m[0] = 0; m[1] = -1; m[2] = szImg.cy; 
		m[3] = 1; m[4] = 0; m[5] = 0; 
	}

	double m1[9], m2[9];
	matrix_reverse(m,3,m1);
	memcpy(m,m1,sizeof(m1));
	
	return;
}

void CVectorView_new::RotateRaster(int ang)
{
	if( !m_bOverlayImgLay || m_arrPImgLays.GetSize()<=0 )
		return;
	
	if( ang!=0 && ang!=90 && ang!=180 && ang!=270 )
		return;

	if( ang==m_nRasterAng )
		return;

	if( !m_bViewBaseOnImage )
	{		
		//客户中心作为焦点，位置不变，这里先记录此位置
		CRect rcClient;
		GetClientRect(&rcClient);
		CPoint cpt = rcClient.CenterPoint();
		
		PT_4D fcpt;
		PT_3D fgpt; 
		fcpt.x = cpt.x;  fcpt.y = cpt.y; fcpt.z = 0;
		ClientToGround(&fcpt,&fgpt);
		double ang0 = (ang-m_nRasterAng)*PI/180;
		//恢复坐标系
		m_nRasterAng = ang;
		m_gsa.lfAngle = m_nRasterAng*PI/180;
		
		double m1[16];
		Matrix44FromRotate(&fgpt,&PT_3D(0,0,1),ang0,m1);
		
		m_pLeftCS->MultiplyMatrix(m1);
		OnChangeCoordSys(FALSE);	
		AdjustCellScale();
		
		OnPostInnerCmd();
		
		RecalcScrollBar(FALSE);
		
		//移动到焦点位置
		DriveToXyz(&fgpt,TRUE);
		
		Invalidate(FALSE);

		return;
	}

	//计算窗口中心坐标，将来仍然要定位到此位置
	CVectorViewCenterPoint ptSave(this);
	
	//客户中心作为焦点，位置不变，这里先记录此位置
	CRect rcClient;
	GetClientRect(&rcClient);
	CPoint cpt = rcClient.CenterPoint();
	
	PT_4D fcpt;
	PT_3D fgpt; 
	fcpt.x = cpt.x;  fcpt.y = cpt.y; fcpt.z = 0;
	ClientToGround(&fcpt,&fgpt);
	
	float zoom = GetZoomRate();
	
	//旋转
	{
		CSize szImg = m_arrPImgLays[0]->GetImageSize();
		double m0[9], m1[9];

		ViewImgPosition item = m_arrImgPositions[0];

		// RasterLayer 中存放的是旋转后的影像数据
		// item.lfDataMatrix(new) = item.lfDataMatrix(old) * Inverse_Old_Angle_Matrix * New_Angle_Matrix

		MatrixRotateImage(szImg,m_nRasterAng,m0);
		matrix_reverse(m0,3,m1);
		memcpy(m0,m1,sizeof(m1));
		MatrixRotateImage(szImg,ang,m1);
		
		item.ConvertToMatrixType();

		double m2[9];
		memcpy(m2,item.lfDataMatrix,sizeof(m2));
		
		double m3[9];
		matrix_multiply(m2,m0,3,m3);
		matrix_multiply(m3,m1,3,m2);

		memcpy(item.lfDataMatrix,m2,sizeof(m2));

		m_arrImgPositions[0] = item;

	}
	
	CString strImg = m_arrPImgLays[0]->GetImgFileName();
	
	m_arrPImgLays[0]->DetachImage();
	m_arrPImgLays[0]->Destroy();

	m_arrPImgLays[0]->SetContext(m_pContext->GetLeftContext());
	
	if( m_arrPImgLays[0]->InitCache(GetBestCacheTypeForLayer(m_arrPImgLays[0]),
		CSize(512,512),100)==FALSE ||
		!m_arrPImgLays[0]->AttachImage(strImg))
	{
		return;
	}

	m_nRasterAng = ang;

	if( m_bOverlayImgLay )
	{
		//计算影像的四个角点坐标
		SetImagePosition(0);
	}
	
	RefreshImageOverlay();
}



void CVectorView_new::OnRotateRaster()
{
	if( !m_muRasterRotate.m_hMenu )
		m_muRasterRotate.LoadMenu(IDR_POPUP_VECTVIEW_ROTATERASTER);
	
// 	int savecmd = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_ROTATERASTER;
	OnPreInnerCmd(IDC_BUTTON_ROTATERASTER);
	
	CPoint pos;
	::GetCursorPos(&pos);
	CButton *pBtn = m_ctrlBarHorz.GetButton(IDC_BUTTON_ROTATERASTER);
	if( pBtn )
	{
		CRect rect;
		pBtn->GetWindowRect(&rect);
		pos = CPoint(rect.right,rect.top);
	}
	
	CMenu* pSumMenu = m_muRasterRotate.GetSubMenu(0);
	if( pSumMenu )
	{
		if( m_nRasterAng==90 )
			pSumMenu->CheckMenuItem(ID_ROTATE_ANG90,MF_BYCOMMAND|MF_CHECKED);
		else
			pSumMenu->CheckMenuItem(ID_ROTATE_ANG90,MF_BYCOMMAND|MF_UNCHECKED);
		
		if( m_nRasterAng==180 )
			pSumMenu->CheckMenuItem(ID_ROTATE_ANG180,MF_BYCOMMAND|MF_CHECKED);
		else
			pSumMenu->CheckMenuItem(ID_ROTATE_ANG180,MF_BYCOMMAND|MF_UNCHECKED);
		
		if( m_nRasterAng==270 )
			pSumMenu->CheckMenuItem(ID_ROTATE_ANG270,MF_BYCOMMAND|MF_CHECKED);
		else
			pSumMenu->CheckMenuItem(ID_ROTATE_ANG270,MF_BYCOMMAND|MF_UNCHECKED);
		
		pSumMenu->CheckMenuItem(ID_ROTATE_ANG0,MF_BYCOMMAND|MF_UNCHECKED);
		
		int cmd = pSumMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RETURNCMD,pos.x,pos.y,this,NULL);
		
		if( cmd==ID_ROTATE_ANG90 )
		{
			RotateRaster(90);
		}
		else if( cmd==ID_ROTATE_ANG180 )
		{
			RotateRaster(180);
		}
		else if( cmd==ID_ROTATE_ANG270 )
		{
			RotateRaster(270);
		}
		else if( cmd==ID_ROTATE_ANG0 )
		{
			RotateRaster(0);
		}
	}
	
	OnPostInnerCmd();
//	m_nInnerCmd = savecmd;
}


void CVectorView_new::OnUpdateRotateRaster(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_bOverlayImgLay && m_arrPImgLays.GetSize()>0 && m_arrPImgLays[0]->IsValid() );
	pCmdUI->SetCheck(0);
}

void CVectorView_new::OnUpdateViewOverlay(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bViewVector?1:0);
}

void CVectorView_new::OnWsAsstereo() 
{
	ASSERT(m_pDocument!=NULL);
	CView* pView = NULL;
	POSITION pos = m_pDocument->GetFirstViewPosition();
	while(pos)
	{
		pView = m_pDocument->GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
		{		
			PT_3D pt3ds[4];
			((CStereoView*)pView)->GetStereoBound(pt3ds);

			if(GraphAPI::GIsClockwise(pt3ds,4)==-1 )
			{
				AfxMessageBox(IDS_DOC_BOUNDINVALID);			
			}
			else
			{
				Envelope e;
				e.CreateFromPts(pt3ds,4);
				((CDlgDoc*)GetDocument())->SetModifiedFlag();
				((CDlgDoc*)GetDocument())->GetDlgDataSource()->GetBound(NULL,&(e.m_zl),&(e.m_zh));
//				((CDlgDoc*)GetDocument())->SetBound(e);
				((CDlgDoc*)GetDocument())->SetBound(pt3ds,e.m_zl,e.m_zh);
			}			
			break;
		}
	}
}


void CVectorView_new::OnWsRefImage()
{
	if( !m_bOverlayImgLay || m_arrPImgLays.GetSize()<=0 )
		return;

	//计算影像的四个角点坐标

	Envelope e;
	for( int i=0; i<m_arrPImgLays.GetSize(); i++)
	{
		e.Union(&m_arrPImgLays[i]->GetDataBound());
	}
	
	((CDlgDoc*)GetDocument())->SetModifiedFlag();
	((CDlgDoc*)GetDocument())->GetDlgDataSource()->GetBound(NULL,&(e.m_zl),&(e.m_zh));

	((CDlgDoc*)GetDocument())->SetBound(e);
}


IMPLEMENT_DYNCREATE(CVectorCellView_new, CVectorView_new)

//#ifdef _DEBUG
CDlgCellDoc* CVectorCellView_new::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDlgCellDoc)));
	return (CDlgCellDoc*)m_pDocument;
}
//#endif //_DEBUG

CWorker *CVectorCellView_new::GetWorker()
{
	return GetDocument();
}

void CVectorCellView_new::OnInitialUpdate() 
{
	CVectorView_new::OnInitialUpdate();

	REG_CMDCLASS(GetWorker(),ID_MOD_LINEFILL,CLineFillCommand);
	REG_CMDCLASS(GetWorker(),ID_MOD_COLORFILLCELL,CColorFillCommand);
	REG_CMDCLASS(GetWorker(),ID_MOD_DELHACHURE,CDelHachureCommand);
	REG_CMDCLASS(GetWorker(),ID_MOD_REPLACE_WITHPOINT,CReplaceWithPointCommand);

	GRIDParam pm;
	OnUpdate(this,hc_UpdateGrid,(CObject*)&pm);
}
void CVectorCellView_new::UpdateGridDrawing()
{
	DWORD dwScale = GetDocument()->GetDlgDataSource()->GetScale();
	double lfRatio = 1/*0.001*dwScale*/;//图上1cm对应实际lfRatio米
	m_sGridParam.ox = GetProfileDouble(REGPATH_CELLOVERLAYGRID,"MinX",m_sGridParam.ox);
	m_sGridParam.oy = GetProfileDouble(REGPATH_CELLOVERLAYGRID,"MinY",m_sGridParam.oy);
	m_sGridParam.dx = lfRatio*GetProfileDouble(REGPATH_CELLOVERLAYGRID,"Width",1);	
	m_sGridParam.dy = lfRatio*GetProfileDouble(REGPATH_CELLOVERLAYGRID,"Height",1);
	m_sGridParam.xr = GetProfileDouble(REGPATH_CELLOVERLAYGRID,"XRange",m_sGridParam.xr);
	m_sGridParam.yr = GetProfileDouble(REGPATH_CELLOVERLAYGRID,"YRange",m_sGridParam.yr);
	strcpy(m_sGridParam.strz,AfxGetApp()->GetProfileString(REGPATH_CELLOVERLAYGRID,"Z",_T("0.0")));
	m_sGridParam.bVisible = GetDocument()->IsOverlayGrid();
	m_sGridParam.bViewVect= GetProfileInt(REGPATH_CELLOVERLAYGRID,"ViewVect",FALSE);
	m_sGridParam.bViewImg = AfxGetApp()->GetProfileInt(REGPATH_CELLOVERLAYGRID,"ViewImg",FALSE);
	m_sGridParam.color = AfxGetApp()->GetProfileInt(REGPATH_CELLOVERLAYGRID,"Color",RGB(128,128,128));
	BOOL bVisible = m_sGridParam.bVisible;
	if( m_sGridParam.dx<=0 || m_sGridParam.dy<=0 )bVisible = FALSE;
	if( m_sGridParam.xr<=0 || m_sGridParam.yr<=0 )bVisible = FALSE;
	m_vectLayL2.DelObj(HANDLE_GRID);
	m_vectLayL2.ClearAll();
	if( !bVisible )
	{
	}
	else
	{
		GrBuffer buf;
		float gridz = 0;
		BOOL bDirectZ = CheckStrZ(m_sGridParam.strz,gridz);
		int i, nx = ceil(m_sGridParam.xr/m_sGridParam.dx), ny = ceil(m_sGridParam.yr/m_sGridParam.dy);
		double xr = m_sGridParam.ox+nx*m_sGridParam.dx, yr = m_sGridParam.oy+ny*m_sGridParam.dy, x,y;
		PT_3D pt (0,0,gridz);
		for( x=0, i=0; i<=nx; i++, x+=m_sGridParam.dx )
		{
			buf.BeginLineString(m_sGridParam.color,0);
			pt.x = m_sGridParam.ox+x; pt.y = m_sGridParam.oy;
			buf.MoveTo(&pt);
			pt.y = yr;
			buf.LineTo(&pt);
			buf.End();
		}
		for( y=0, i=0; i<=ny; i++, y+=m_sGridParam.dy )
		{
			pt.y = m_sGridParam.oy+y; pt.x = m_sGridParam.ox;
			buf.BeginLineString(m_sGridParam.color,0);
			buf.MoveTo(&pt);
			pt.x = xr;
			buf.LineTo(&pt);
			buf.End();
		}
		buf.BeginLineString(m_sGridParam.color,0.01);
		pt.x = m_sGridParam.ox; pt.y = m_sGridParam.oy;
		buf.MoveTo(&pt);
		pt.y = yr;
		buf.LineTo(&pt);
		buf.End();
		pt.y = m_sGridParam.oy; pt.x = m_sGridParam.ox;
		buf.BeginLineString(m_sGridParam.color,0.01);
		buf.MoveTo(&pt);
		pt.x = xr;
		buf.LineTo(&pt);
		buf.End();
		AddObjtoVectLay2(HANDLE_GRID,&buf,TRUE);
		m_vectLayL2.ClearAll();
	}
}


void CVectorView_new::ScrollForDrawBitmap(double dx,double dy,double dz )
{
	double m[16];	
	Matrix44FromMove(-dx,-dy,-dz,m);
	m_pLeftCS->MultiplyMatrix(m);
	m_pRightCS->MultiplyMatrix(m);
	m_laymgrLeft.OnChangeCoordSys(TRUE);
	m_laymgrRight.OnChangeCoordSys(TRUE);

	OnChangeCoordSys(FALSE);
	
	CDrawingContext *pDC = m_pContext->GetLeftContext();
	if( pDC )pDC->SetCoordSys(m_pLeftCS);
	
	pDC = m_pContext->GetRightContext();
	if( pDC )pDC->SetCoordSys(m_pRightCS);	
	//RecalcScrollBar(FALSE);
	//Invalidate(TRUE);
	
}
extern void WriteToFile(HBITMAP hBmp);
HBITMAP CVectorView_new::GetViewBitmap(int nWidthDest, int nHeightDest)
{

	CRect rcClient;
	GetClientRect(&rcClient);

	//保存缩放中心的大地坐标
	PT_3D oldgpt;

	float oldzoom = GetZoomRate();

	// 缩放到适合尺寸
	MoveWindow(0,0,64,64);	

	// 记录缩放中心
	CRect rect;
	GetClientRect(&rect);
	CSize sizeClient = rect.Size();
	CSize szMap = CalcViewSize();
	
	double change_x = double(sizeClient.cx)/szMap.cx;
	double change_y = double(sizeClient.cy)/szMap.cy;
	
	// Zoom rate == min rate
	if( change_x<change_y ) change_y = change_x;
	
	CPoint pt(szMap.cx/2,szMap.cy/2);
	CPoint ptScroll = GetScrollPos();
	pt.x = pt.x-ptScroll.x;  
	pt.y = pt.y-ptScroll.y;	
	
	if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit )
		pt.x = pt.x - rect.right/2;

	PT_4D pt0(pt.x,pt.y,0);
	ClientToGround(&pt0,&oldgpt);

	OnZoomFit();

	m_vectLayL2.DelObj(HANDLE_BOUND);
	m_vectLayL2.ClearAll();
	Invalidate(FALSE);

// 	m_vectLayL.SetObjVisible(HANDLE_BOUND,FALSE);
// 	m_vectLayL.SetObjVisible(HANDLE_GRAPH,FALSE);
// 
// 	m_vectLayL.ClearAll();
	
	//计算起点（左上角点）
	PT_4D pt1;

	CSize size;
	
	PT_3D bounds[4];
	PT_4D bounds4d[4];
	int i;
	
	CDlgDoc *pDoc = GetDocument();

	pDoc->GetDlgDataSource()->GetBound(bounds,NULL,NULL);
	for( i=0; i<4; i++)GroundToClient(bounds+i,bounds4d+i);
	
	// 导出位图为正方形，有效区域放在正中间
	Envelope evlp;
	evlp.CreateFromPts(bounds4d,4,sizeof(PT_4D));
	
	int nWidthEvlp = ceil(evlp.Width()), nHeightEvlp = ceil(evlp.Height());

	int nMaxEdge = nWidthEvlp>nHeightEvlp?nWidthEvlp:nHeightEvlp;

	size.cx = nMaxEdge+5;
	size.cy = nMaxEdge+5;	

	int xoff = (size.cx - nWidthEvlp)/2, yoff = (size.cy - nHeightEvlp)/2;
	pt1.x = floor(evlp.m_xl-xoff);	pt1.y = floor(evlp.m_yl-yoff);


	HBITMAP hDestBmp = Create24BitDIB(size.cx,size.cy,TRUE,RGB(255,255,255));

	BITMAP bmp;
	::GetObject(hDestBmp,sizeof(BITMAP),&bmp);	
	BYTE *pDest = (BYTE*)bmp.bmBits;

	//计算迭代绘制图块的大小
	int nx = 0, ny = 0, stepx = size.cx, stepy = 1;
	DWORD nbytes = 0;

	if( stepx*stepy*3>0x300000 )
	{
	}
	else
	{
		stepy = 0x300000/(stepx*3);
	}

	if( stepy>1024 )stepy = 1024;

	long lSum = ceil(size.cy*1.0/stepy);

	int w = size.cx, h = stepy;

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

	//准备绘制
	CPtrArray pLayerListL,pLayerListR;
	CArray<BOOL,BOOL> arrForceDraws;

	for( i=0; i<m_arrPImgLays.GetSize(); i++)
	{
		pLayerListL.Add(m_arrPImgLays[i]);

		arrForceDraws.Add(m_arrPImgLays[i]->m_bForceDraw);
		m_arrPImgLays[i]->m_bForceDraw = FALSE;
	}
	
	pLayerListL.Add(&m_vectLayL2);
	
	arrForceDraws.Add(m_vectLayL2.EnableDelayDraw(FALSE));
	
	ScrollForDrawBitmap(pt1.x,pt1.y,0);
	
	CRect wndRc; 

	if( hBmp && pBits )
	{
		for( ny=0; ny<size.cy; ny+=stepy)
		{
			wndRc = CRect(0,0,stepx,stepy);
			if(ny+stepy>size.cy)
			{
				wndRc = CRect(0,0,stepx,size.cy-ny);
			}
			if(ny!=0)
			{				
				ScrollForDrawBitmap(0,stepy,0);
			}

			memset(pBits, 0, bitmapInfo.bmiHeader.biSizeImage);

			CRect rcBmp1=CRect(0,0,wndRc.Width(),wndRc.Height()), rcWnd1=wndRc;

			int dx = 0;
			for( int x=0; x<stepx; x+=1024)
			{
				rcBmp1.left = x; rcBmp1.right = x+1024;

				if( rcBmp1.right>stepx )
					rcBmp1.right = stepx;

				rcWnd1 = rcBmp1;
				rcWnd1.OffsetRect(-x,0);

				MoveWindow(0,0,rcWnd1.Width(),rcWnd1.Height());
				
				DrawRectBitmap(hBmp,rcBmp1,&rcWnd1,
					pLayerListL,pLayerListR,CStereoDrawingContext::drawLeft);

				ScrollForDrawBitmap(1024,0,0);
				dx += 1024;
			}

			ScrollForDrawBitmap(-dx,0,0);

			//获取地址
			BITMAP bmp;
			::GetObject(hBmp,sizeof(BITMAP),&bmp);
			
			BYTE *pSrc = (BYTE*)bmp.bmBits;
			int bmphei = wndRc.Height();
			
			int num = (DWORD)(bmp.bmHeight-bmphei)*(DWORD)((bmp.bmWidth*3+3)&(~3));
			
			memcpy(pDest+(size.cy-bmphei)*((size.cx*3+3)&(~3)),pSrc+num,bmphei*(DWORD)((bmp.bmWidth*3+3)&(~3)));
		}
		
		::DeleteObject(hBmp);
	}

	// 置背景色为白色，白色地物为灰色
	int bmpcxSize = ((size.cx*3+3)&(~3));
	
	for (i=0; i<size.cy; i++)
	{
		BYTE *pRGB = pDest+i*((size.cx*3+3)&(~3));
		for (int j=0; j<=bmpcxSize-3; j+=3,pRGB += 3)
		{
			COLORREF col = RGB(*(pRGB+2),*(pRGB+1),*pRGB);
			if (col == m_clrBack)
			{
				*pRGB = *(pRGB+1) = *(pRGB+2) = 255;
			}
			else if (col == RGB(255,255,255))
			{
				*pRGB = *(pRGB+1) = *(pRGB+2) = 128;
			}

		}
	}

	for( i=0; i<m_arrPImgLays.GetSize(); i++)
	{
		m_arrPImgLays[i]->m_bForceDraw = arrForceDraws[i];
	}
	
	m_vectLayL2.EnableDelayDraw(arrForceDraws[i]);

	//恢复显示内容
	MoveWindow(0,0,rcClient.Width(),rcClient.Height());

	GrBuffer buf;
	DrawBkgnd(&buf);
	
	AddObjtoVectLay2(HANDLE_BOUND,&buf,TRUE);
// 	m_vectLayL.ClearAll();
// 	m_vectLayL.SetObjVisible(HANDLE_BOUND,TRUE);
// 	m_vectLayL.SetObjVisible(HANDLE_GRAPH,TRUE);

	GroundToClient(&oldgpt,&pt1);
	//ScrollForDrawBitmap(pt1.x,pt1.y,0);
	//ZoomChange(CPoint(0,0),oldzoom/GetZoomRate(),FALSE);
	ZoomChange(CPoint(pt1.x,pt1.y),oldzoom/GetZoomRate());
	GroundToClient(&oldgpt,&pt1);
	ScrollForDrawBitmap(pt1.x-pt.x,pt1.y-pt.y,0);

	// 背景为白色
	/*CRect rect;
	rect.left = rect.bottom = 0;
	rect.top = bmp.bmHeight;
	rect.right = bmp.bmWidth;
	SetBitmapBKColor(hDestBmp,rect,RGB(255,255,255));

	WriteToFile(hDestBmp);*/

/*
	// 缩放到32*32 , 16*16
	HDC   hMemDCSrc = ::CreateCompatibleDC(NULL); 

	HBITMAP   hOldSrc = (HBITMAP)SelectObject(hMemDCSrc,hDestBmp); 


	HDC   hMemDCDest = ::CreateCompatibleDC(NULL); 
	HBITMAP   hImageDest = ::Create24BitDIB(32,32); 
	
	BITMAP bmp11;
	::GetObject(hImageDest,sizeof(BITMAP),&bmp11);
	HBITMAP   hOldDest = (HBITMAP)SelectObject(hMemDCDest,hImageDest); 
	
	int iOldMode = SetStretchBltMode(hMemDCDest, HALFTONE);
	//::SetBrushOrgEx(hMemDCDest, 0, 0, NULL);
	StretchBlt(hMemDCDest,0,0,32,32,hMemDCSrc,0,0,size.cx,size.cy,SRCCOPY);
	SetStretchBltMode(hMemDCDest, iOldMode);

	(HBITMAP)SelectObject(hMemDCSrc,hOldSrc); 

	DeleteObject(hMemDCSrc); 
	DeleteObject(hOldSrc);
	
	hImageDest = (HBITMAP)SelectObject(hMemDCDest,hOldDest); 

	WriteToFile(hDestBmp);
	WriteToFile(hImageDest);

	DeleteObject(hMemDCDest); 
	DeleteObject(hOldDest); 
	

	DeleteObject(hDestBmp);*/

	return hDestBmp;
}

void CVectorView_new::OnExportRaster()
{
	CDlgExportRaster dlg;

	dlg.m_bOverImg = m_bOverlayImgLay;
	
	if( dlg.DoModal()!=IDOK )return;
	if( dlg.m_strFilePath.GetLength()<=0 )
		return;
		
	CDlgDoc *pDoc = GetDocument();

	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_PREPARE) );

	TIFF *tif = TIFFOpen(dlg.m_strFilePath,"w");
	
	if( !tif )
		return;

	m_bUseAnyZoomrate = TRUE;

	//保存原始位置的左上角的大地坐标
	PT_3D oldgpt;
	PT_4D pt0 = PT_4D(0,0,0,0);
	ClientToGround(&pt0,&oldgpt);

	float oldzoom = GetZoomRate();

	m_vectLayL2.SetObjVisible(HANDLE_BOUND,FALSE);
	m_vectLayL2.SetObjVisible(HANDLE_GRAPH,FALSE);

	m_vectLayL2.ClearAll();

	double fscale = pDoc->GetDlgDataSource()->GetSymbolDrawScale();
	
	//计算起点（左上角点）
	PT_4D pt1,ptClient,pt2;
	PT_3D ptLBCorner;

	double pixel_size = 1.0;

	CSize size;
	//使用影像范围，使用影像分辨率
	if( dlg.m_nBoundType==0 && m_arrPImgLays.GetSize()>0 )
	{
		float newZoom = 1.0f/fabs(m_arrImgPositions[0].lfImgMatrix[0]);
		ZoomChange(CPoint(0,0),newZoom/oldzoom,FALSE);

		Envelope e = GetImgLayBound();

		PT_3D bounds[4];
		PT_4D bounds4d[4];

		bounds[0].x = e.m_xl, bounds[0].y = e.m_yl;
		bounds[1].x = e.m_xh, bounds[1].y = e.m_yl;
		bounds[2].x = e.m_xh, bounds[2].y = e.m_yh;
		bounds[3].x = e.m_xl, bounds[3].y = e.m_yh;
		pt2.x = e.m_xl; pt2.y = e.m_yl;
		ptLBCorner = PT_3D(e.m_xl,e.m_yl,0);
		
		for( int i=0; i<4; i++)
		{
			GroundToClient(bounds+i,bounds4d+i);
		}
		
		Envelope evlp;
		evlp.CreateFromPts(bounds4d,4,sizeof(PT_4D));

		pt1.x = floor(evlp.m_xl);	pt1.y = floor(evlp.m_yl);
		
		size.cx = ceil(evlp.m_xh)-pt1.x;
		size.cy = ceil(evlp.m_yh)-pt1.y;

		pixel_size = e.Width()/size.cx;
	}
	//使用工作区范围，使用DPI分辨率
	else if( dlg.m_nBoundType==1 && dlg.m_nResolutionType==1 && dlg.m_nDPI>0 )
	{
		PT_3D bounds[4], bounds1[4];
		PT_4D bounds4d[4];
		int i;
		
		pDoc->GetDlgDataSource()->GetBound(bounds,NULL,NULL);

		Envelope evlp0;
		evlp0.CreateFromPts(bounds,4,sizeof(PT_3D));

		evlp0.m_xl -= dlg.m_fMarginLeft*pDoc->GetDlgDataSource()->GetScale()*0.001;
		evlp0.m_xh += dlg.m_fMarginRight*pDoc->GetDlgDataSource()->GetScale()*0.001;
		evlp0.m_yh += dlg.m_fMarginTop*pDoc->GetDlgDataSource()->GetScale()*0.001;
		evlp0.m_yl -= dlg.m_fMarginBottom*pDoc->GetDlgDataSource()->GetScale()*0.001;
		
		ptLBCorner = PT_3D(evlp0.m_xl,evlp0.m_yl,0);

		bounds1[0].x = evlp0.m_xl; bounds1[0].y = evlp0.m_yl; bounds1[0].z = 0;
		bounds1[1].x = evlp0.m_xh; bounds1[1].y = evlp0.m_yl; bounds1[1].z = 0;
		bounds1[2].x = evlp0.m_xh; bounds1[2].y = evlp0.m_yh; bounds1[2].z = 0;
		bounds1[3].x = evlp0.m_xl; bounds1[3].y = evlp0.m_yh; bounds1[3].z = 0;
		
		for( i=0; i<4; i++)GroundToClient(bounds1+i,bounds4d+i);
		Envelope evlp;
		evlp.CreateFromPts(bounds4d,4,sizeof(PT_4D));
		
		pt1.x = floor(evlp.m_xl);	pt1.y = floor(evlp.m_yl);
		int cx0 = ceil(evlp.m_xh)-pt1.x;
		int cy0 = ceil(evlp.m_yh)-pt1.y;

		size.cx = (50.0+dlg.m_fMarginLeft*0.1+dlg.m_fMarginRight*0.1)/2.54 * dlg.m_nDPI;
		size.cy = (50.0+dlg.m_fMarginTop*0.1+dlg.m_fMarginBottom*0.1)/2.54 * dlg.m_nDPI;

		double zoomx = size.cx * 1.0/cx0;
		double zoomy = size.cy * 1.0/cy0;
		double change = max(zoomx,zoomy);

		ZoomChange(CPoint(0,0),change,FALSE);

		pixel_size = evlp0.Width()/size.cx;

		m_pContext->SetPrint(TRUE,0.15*fscale);

		for( i=0; i<4; i++)GroundToClient(bounds1+i,bounds4d+i);
		evlp.CreateFromPts(bounds4d,4,sizeof(PT_4D));
		pt1.x = floor(evlp.m_xl);	pt1.y = floor(evlp.m_yl);
		
	}
	//其他情况：采用工作区范围，视图显示倍率
	else
	{
		PT_3D bounds[4];
		PT_4D bounds4d[4];
		int i;
		
		pDoc->GetDlgDataSource()->GetBound(bounds,NULL,NULL);
		for( i=0; i<4; i++)GroundToClient(bounds+i,bounds4d+i);

		Envelope evlp0;
		evlp0.CreateFromPts(bounds,4,sizeof(PT_3D));
		ptLBCorner = PT_3D(evlp0.m_xl,evlp0.m_yl,0);
		
		Envelope evlp;
		evlp.CreateFromPts(bounds4d,4,sizeof(PT_4D));

		pt1.x = floor(evlp.m_xl);	pt1.y = floor(evlp.m_yl);

		size.cx = ceil(evlp.m_xh)-pt1.x;
		size.cy = ceil(evlp.m_yh)-pt1.y;

		pixel_size = evlp0.Width()/evlp.Width();
	}

	//计算迭代绘制图块的大小
	int nx = 0, ny = 0, stepx = size.cx, stepy = 1;
	DWORD nbytes = 0;

	if( stepx*stepy*3>0x300000 )
	{
	}
	else
	{
		stepy = 0x300000/(stepx*3);
	}

	if( stepy>1024 )stepy = 1024;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, size.cx);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, size.cy);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, stepy);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

	if( dlg.m_nBoundType==1 && dlg.m_nResolutionType==1 && dlg.m_nDPI>0 )
	{
		TIFFSetField(tif, TIFFTAG_XRESOLUTION, dlg.m_nDPI);
		TIFFSetField(tif, TIFFTAG_YRESOLUTION, dlg.m_nDPI);
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	}
	else
	{
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_NONE);	
	}
	

	long lSum = ceil(size.cy*1.0/stepy);

	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_PROCESS) );

	GProgressStart(lSum);
	int w = size.cx, h = stepy;

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

	//准备绘制
	CPtrArray pLayerListL,pLayerListR;
	CArray<BOOL,BOOL> arrForceDraws;

	for( int i=0; i<m_arrPImgLays.GetSize(); i++)
	{
		pLayerListL.Add(m_arrPImgLays[i]);

		arrForceDraws.Add(m_arrPImgLays[i]->m_bForceDraw);
		m_arrPImgLays[i]->m_bForceDraw = TRUE;
	}
	
	pLayerListL.Add(&m_vectLayL2);
	
	arrForceDraws.Add(m_vectLayL2.m_bForceDraw);
	m_vectLayL2.m_bForceDraw = TRUE;
	arrForceDraws.Add(m_vectLayL2.EnableDelayDraw(FALSE));
	
	CRect rcClient;
	GetClientRect(&rcClient);
	ScrollForDrawBitmap(pt1.x,pt1.y,0);
	
	CRect wndRc; 

	BYTE *pBuf = new BYTE[w*h*3];
	if( hBmp && pBits && pBuf )
	{
		for( ny=0; ny<size.cy; ny+=stepy)
		{
			wndRc = CRect(0,0,stepx,stepy);
			if(ny+stepy>size.cy)
			{
				wndRc = CRect(0,0,stepx,size.cy-ny);
			}
			if(ny!=0)
			{				
				ScrollForDrawBitmap(0,stepy,0);
			}

			//增长进度条
			GProgressStep();

			memset(pBits, 0, bitmapInfo.bmiHeader.biSizeImage);

			CRect rcBmp1=CRect(0,0,wndRc.Width(),wndRc.Height()), rcWnd1=wndRc;

			int dx = 0;
			for( int x=0; x<stepx; x+=1024)
			{
				rcBmp1.left = x; rcBmp1.right = x+1024;

				if( rcBmp1.right>stepx )
					rcBmp1.right = stepx;

				rcWnd1 = rcBmp1;
				rcWnd1.OffsetRect(-x,0);

				MoveWindow(0,0,rcWnd1.Width(),rcWnd1.Height());
				
				DrawRectBitmap(hBmp,rcBmp1,&rcWnd1,
					pLayerListL,pLayerListR,CStereoDrawingContext::drawLeft);

				ScrollForDrawBitmap(1024,0,0);
				dx += 1024;
			}

			ScrollForDrawBitmap(-dx,0,0);

			//获取地址
			BITMAP bmp;
			::GetObject(hBmp,sizeof(BITMAP),&bmp);

			BYTE *pSrc = (BYTE*)bmp.bmBits;
			BYTE *p1, *p2;
			int bmphei = wndRc.Height();

			//交换字节
			for( int j=0; j<bmphei; j++)
			{
				p1 = pSrc + (DWORD)(bmp.bmHeight-1-j)*(DWORD)((bmp.bmWidth*3+3)&(~3));
				p2 = pBuf + j*bmp.bmWidth*3;
				for( int i=0; i<bmp.bmWidth; i++)
				{
					p2[0] = p1[2];
					p2[1] = p1[1];
					p2[2] = p1[0];
					p1 += 3;
					p2 += 3;
				}
			}

			//写到tif文件中
			for( j=0; j<bmphei; j++)
			{
				TIFFWriteScanline(tif, pBuf+j*bmp.bmWidth*3, ny+j, 0);
			}
		}

		delete[] pBuf;
		::DeleteObject(hBmp);
	}

	for( i=0; i<m_arrPImgLays.GetSize(); i++)
	{
		m_arrPImgLays[i]->m_bForceDraw = arrForceDraws[i];
	}
	
	m_vectLayL2.m_bForceDraw = arrForceDraws[i++];
	m_vectLayL2.EnableDelayDraw(arrForceDraws[i]);

	TIFFClose(tif);

	//写 tfw 文件
	if(1)//if( dlg.nBoundType==0 && m_arrPImgLays.GetSize()>0 )
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

			PT_3D gpt;
			ClientToGround(&pt1,&gpt);

			// 将基于左下角像素的参数转为基于左上角像素的参数
			v[0] = pixel_size;
			v[1] = 0;
			v[2] = 0;
			v[3] =pixel_size;
			v[4] = ptLBCorner.x;
			v[5] = ptLBCorner.y;
			
			fprintf(fp,"%.6f\n%.6f\n%.6f\n%.6f\n%.6f\n%.6f\n",
				v[0],v[1],v[2],v[3],v[4],v[5]);
			fclose(fp);
		}
	}

	//进度条复位
	GProgressEnd();
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_END) );

	m_pContext->SetPrint(FALSE);

	m_bUseAnyZoomrate = FALSE;

	//恢复显示内容
	MoveWindow(0,0,rcClient.Width(),rcClient.Height());
	m_vectLayL2.ClearAll();
	m_vectLayL2.SetObjVisible(HANDLE_BOUND,TRUE);
	m_vectLayL2.SetObjVisible(HANDLE_GRAPH,TRUE);

	GroundToClient(&oldgpt,&pt1);
	ScrollForDrawBitmap(pt1.x,pt1.y,0);
	ZoomChange(CPoint(0,0),oldzoom/GetZoomRate(),FALSE);

	return;
}



void CVectorView_new::OnViewOverlay() 
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_VIEWOVERLAY;
	OnPreInnerCmd(IDC_BUTTON_VIEWOVERLAY);
	
	m_bViewVector = !m_bViewVector;
	if( m_bViewVector )
	{
		m_laymgrLeft.InsertDrawingLayer(&m_vectLayL2,m_arrPImgLays.GetSize());
		m_laymgrRight.InsertDrawingLayer(&m_vectLayR,m_arrPImgLays.GetSize());

		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_VIEWOVERLAY,TRUE,TRUE);
	}
	else
	{
		m_laymgrLeft.RemoveDrawingLayer(&m_vectLayL2);
		m_laymgrRight.RemoveDrawingLayer(&m_vectLayR);
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_VIEWOVERLAY,TRUE,FALSE);
	}
	
	Invalidate(FALSE);
	
	OnPostInnerCmd();
//	m_nInnerCmd = save;
}

void CVectorView_new::OnOsgbview()
{
	// TODO: Add your command handler code here
	//立体测图许可
	if (!CheckLicense(73))
	{
		GOutPut(StrFromResID(IDS_ERR_INVALIDLICENSE));
		return;
	}
	
		//倾斜测图许可
	if (!CheckLicense(85))
	{
		GOutPut(StrFromResID(IDS_MISS_OSGB_LICENSE));
		return;
	}

	//获得客户端区域范围
	CRect vectClt;
	GetParentFrame()->GetClientRect(&vectClt);
	int left = vectClt.left;
	int right = left + vectClt.Width() / 2;
	int top = vectClt.top;
	int bottom = vectClt.bottom;


	CWinApp *pApp = AfxGetApp();
	POSITION curTemplatePos = pApp->GetFirstDocTemplatePosition();

	CDocument* pCurDoc = GetDocument();

	CDocument *pDoc = NULL;
	CDocTemplate* curTemplate = NULL;
	while (curTemplatePos != NULL)
	{
		curTemplate = pApp->GetNextDocTemplate(curTemplatePos);

		POSITION curDocPos = curTemplate->GetFirstDocPosition();
		while (curDocPos != NULL)
		{
			pDoc = curTemplate->GetNextDoc(curDocPos);
			if (pDoc == pCurDoc)break;
		}
		if (pDoc == pCurDoc)break;
	}

	if (!curTemplate || pDoc != pCurDoc)return;

	//找到当前的文档模板
	CCreateContext context;
	context.m_pCurrentFrame = NULL;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = RUNTIME_CLASS(COsgbView);
	context.m_pNewDocTemplate = curTemplate;

	CFrameWnd* pFrame = (CFrameWnd*)new CChildFrame;
	if (pFrame == NULL)
		return;

	// create new from resource
	if (!pFrame->LoadFrame(IDR_EDITBATYPE,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,   // default frame styles
		NULL, &context))
	{
		return;
	}

	//获取osgbview指针
	COsgbView* pOsgbView = getOsgbView();

	CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("3D model Files (*.osgb, *.osg, *.obj)|*.osgb;*.osg;*.obj;*.osgt|All Files (*.*)|*.*||"), NULL);

	if (dialog.DoModal() == IDOK)
	{
		string fileName = dialog.GetPathName();
		curTemplate->InitialUpdateFrame(pFrame, pCurDoc, TRUE);

		if (pOsgbView != nullptr)
		{
			pOsgbView->initOsgScene(fileName);
// 			std::string path = GetModuleFolder();
// 			path += "\\index1.html";
// 			replace_all(path, "\\", "\/");
// 			CDlgCefMainToolBar* dlg = new CDlgCefMainToolBar(MBSCToCEF(path.c_str()));
// 			dlg->Create(IDD_DIALOG_CEF_MAINTOOLBAR);
// 			dlg->ShowWindow(SW_SHOW);
		}
	
		////矢量视图移动到左侧一半
		//this->GetParentFrame()->MoveWindow(CRect(left, top, right, bottom), TRUE);

		////三维视图移动到右侧一半
		//pOsgbView->GetParentFrame()->MoveWindow(CRect(right, top, right + vectClt.Width() / 2, bottom), TRUE);

		AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_WINDOW_TILE_VERT, 0), 0);
	}
	else
	{
		if (pOsgbView != nullptr)
		{
			pOsgbView->SendMessage(WM_CLOSE, 0, 0);
		}
	}

}


COsgbView* CVectorView_new::getOsgbView()
{
	POSITION pos = GetDocument()->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetDocument()->GetNextView(pos);
		ASSERT_VALID(pView);

		if (pView->IsKindOf(RUNTIME_CLASS(COsgbView)))
		{
			COsgbView* pOsgbView = DYNAMIC_DOWNCAST(COsgbView, pView);
			return pOsgbView;
		}
	}

	return nullptr;
}


#include "CommonCallStation.h"
void CVectorView_new::OnRefresh()
{
	AfxCallMessage(WM_COMMAND,ID_STEREO_REFRESH,0);
}

void CVectorView_new::OnZoomIn()
{
	AfxCallMessage(WM_COMMAND,ID_STEREO_ZOOMIN,0);
}

void CVectorView_new::OnZoomOut()
{
	AfxCallMessage(WM_COMMAND,ID_STEREO_ZOOMOUT,0);
}

LRESULT CVectorView_new::OnZoomHotKey(WPARAM  wParam,LPARAM  lParam)    
{ 	
	if (wParam == ID_VECTOR_ZOOMIN)
	{
		CSceneView::OnZoomIn();
	}
	else if (wParam == ID_VECTOR_ZOOMOUT)
	{
		CSceneView::OnZoomOut();
	}
	else if (wParam == ID_VECTOR_ZOOMFIT)
	{
		CSceneView::OnZoomFit();
	}
	
	return 0;
}


void CVectorView_new::PushViewPos()
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


void CVectorView_new::ActiveViewPos(ViewPos* pos)
{
	CBaseView::ActiveViewPos(pos);
	
	DriveToXyz(&pos->pt,TRUE);
}


BOOL CVectorView_new::PrePrint(CDC* pDC, CPrintInfo* pInfo)
{
	m_hTempMemDC = ::CreateCompatibleDC(pDC->GetSafeHdc());
	m_hTempMemBmp = m_pPrintContext->CreateMemBmpBoard(1024,768);
	m_hOldBmp = (HBITMAP)::SelectObject(m_hTempMemDC,m_hTempMemBmp);

	CSize sz_test(10000,10000);
	pDC->HIMETRICtoLP(&sz_test);
	int margin_left = m_fPrintMarginLeft * sz_test.cx/100;
	int margin_right = m_fPrintMarginRight * sz_test.cx/100;
	int margin_top = m_fPrintMarginTop * sz_test.cy/100;
	int margin_bottom = m_fPrintMarginBottom * sz_test.cy/100;

	//仅在工作区范围下使用
	double lfExtension = m_fPrintExtension * GetDocument()->GetDlgDataSource()->GetSymbolDrawScale();

	if (m_nPrintScope==0&&m_nPrintScale==0)//边界有像素偏差
	{
		Envelope e = GetDocument()->GetDlgDataSource()->GetBound();

		e.Inflate(lfExtension/2,lfExtension/2,0);
		
		e.TransformGrdToClt(GetCoordWnd().m_pViewCS,1);
		CRect DrawRect = pInfo->m_rectDraw;
		CSize size;
		size.cx = DrawRect.Width();
		size.cy = DrawRect.Height();
		
		double m[9];
		memset(m,0,sizeof(double)*9);
		double xScale = ((double)(size.cx-margin_left-margin_right))/e.Width();
		double yScale = ((double)(size.cy-margin_top-margin_bottom))/e.Height();
		if( xScale>yScale )xScale = yScale;
		else yScale = xScale;

		m[0] = xScale;
		m[2] = -xScale*(e.m_xl)+margin_left;
		m[4] = yScale;
		m[5] = -yScale*(e.m_yl)+margin_top;
		m[8] = 1.0;
		m_pTempCoordSys->Create33Matrix(m);
		CArray<CCoordSys*,CCoordSys*> arr;
		arr.Add(GetCoordWnd().m_pViewCS);
		arr.Add(m_pTempCoordSys);
		m_pComCoordSys->CreateCompound(arr);

		CRect rcDC(0,0,size.cx,size.cy);
		CRect rectClip(rcDC);
		rectClip.DeflateRect(margin_left,margin_top,margin_right,margin_bottom);
		CRect rectView = rectClip;

		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{			
			CPreviewDC *pPrevDC = static_cast<CPreviewDC*>(pDC);
			pPrevDC->PrinterDPtoScreenDP(&rectClip.TopLeft());
			pPrevDC->PrinterDPtoScreenDP(&rectClip.BottomRight());
			CPoint ptOrg;
			::GetViewportOrgEx(pDC->m_hDC,&ptOrg);
			rectClip += ptOrg;
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		else
		{
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		pDC->SelectClipRgn(&rgn);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(/*m_hTempMemDC*/pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pComCoordSys);
		m_pPrintContext->SetDCSize(size);
		m_pPrintContext->SetViewRect(rectView);
		m_pPrintContext->SetBackColor(RGB(255,255,255));
	}
	else if (m_nPrintScope==0&&m_nPrintScale==1)
	{
		DWORD dwScale = GetDocument()->GetDlgDataSource()->GetScale();
		Envelope e0 = GetDocument()->GetDlgDataSource()->GetBound();
		e0.Inflate(lfExtension/2,lfExtension/2,0);

		Envelope e = m_vectLayL2.GetDataBound();
		CRect DrawRect = pInfo->m_rectDraw;
		CSize size;
		size.cx = DrawRect.Width();
		size.cy = DrawRect.Height();
		int nXPpi = ::GetDeviceCaps(m_hViewDC,LOGPIXELSX);
		int nYPpi = ::GetDeviceCaps(m_hViewDC,LOGPIXELSY);
		int PrintDcDpix = pDC->GetDeviceCaps(LOGPIXELSX);
		int PrintDcDpiy = pDC->GetDeviceCaps(LOGPIXELSY);
		double xScale = 100.0*PrintDcDpix/((dwScale*2.54));
		double yScale = 100.0*PrintDcDpiy/((dwScale*2.54));

		if( xScale>yScale )xScale = yScale;
		else yScale = xScale;

		double m[9];
		memset(m,0,sizeof(double)*9);
		
		m[0] = xScale;
		m[4] = -yScale;
		m[2] = -xScale*e0.m_xl+margin_left;
		m[5] = yScale*e0.m_yl+e0.Height()*yScale+margin_top;
		m[8] = 1.0;
		if(m_bOverlayImgLay)
		{
			double m0[9];
			memcpy(m0,m,sizeof(m));
			m[0] = m0[0]*m_lfImgMatrix[0]+m0[1]*m_lfImgMatrix[4]/*+m0[2]*m_lfImgMatrix[12]*/;
			m[1] = m0[0]*m_lfImgMatrix[1]+m0[1]*m_lfImgMatrix[5]/*+m0[2]*m_lfImgMatrix[13]*/;
			m[2] = m0[0]*m_lfImgMatrix[3]+m0[1]*m_lfImgMatrix[7]+m0[2]/**m_lfImgMatrix[15]*/;
			m[3] = m0[3]*m_lfImgMatrix[0]+m0[4]*m_lfImgMatrix[4]/*+m0[5]*m_lfImgMatrix[12]*/;
			m[4] = m0[3]*m_lfImgMatrix[1]+m0[4]*m_lfImgMatrix[5]/*+m0[5]*m_lfImgMatrix[13]*/;
			m[5] = m0[3]*m_lfImgMatrix[3]+m0[4]*m_lfImgMatrix[7]+m0[5]/**m_lfImgMatrix[15]*/;
			m[6] = m0[6]*m_lfImgMatrix[0]+m0[7]*m_lfImgMatrix[4]/*+m0[8]*m_lfImgMatrix[12]*/;
			m[7] = m0[6]*m_lfImgMatrix[1]+m0[7]*m_lfImgMatrix[5]/*+m0[8]*m_lfImgMatrix[13]*/;
			m[8] = m0[6]*m_lfImgMatrix[3]+m0[7]*m_lfImgMatrix[7]+m0[8]/**m_lfImgMatrix[15]*/;
		}
		m_pTempCoordSys->Create33Matrix(m);
		Envelope e1(e0);
		if(m_bOverlayImgLay)
			e1 = e;		
		e1.TransformGrdToClt(m_pTempCoordSys,1);
		CRect rect(e1.m_xl,e1.m_yl,e1.m_xh,e1.m_yh);
		CRect rcDC(0,0,size.cx,size.cy);
		rect = rect&rcDC;

		CRect rectClip(rcDC);
		rectClip.DeflateRect(margin_left,margin_top,margin_right,margin_bottom);
		CRect rcView = rectClip;
		
		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
			CPreviewDC *pPrevDC = static_cast<CPreviewDC*>(pDC);
			pPrevDC->PrinterDPtoScreenDP(&rectClip.TopLeft());
			pPrevDC->PrinterDPtoScreenDP(&rectClip.BottomRight());
			CPoint ptOrg;
			::GetViewportOrgEx(pDC->m_hDC,&ptOrg);
			rectClip += ptOrg;
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		else
		{
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		pDC->SelectClipRgn(&rgn,RGN_AND);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pTempCoordSys);
		m_pPrintContext->SetDCSize(size);
		m_pPrintContext->SetViewRect(rcView);
		m_pPrintContext->SetBackColor(RGB(255,255,255));
	}
	else if (m_nPrintScope==1&&m_nPrintScale==0)
	{
		if (!m_bOverlayImgLay)
		{
			return FALSE;
		}
		Envelope e = GetImgLayBound();		
		{
			PT_3D pt3d[4];
			PT_4D pt4d[4];
			pt4d[0].x = pt4d[3].x = e.m_xl;			
			pt4d[1].x = pt4d[2].x = e.m_xh;
			pt4d[0].y = pt4d[1].y = e.m_yl;
			pt4d[2].y = pt4d[3].y = e.m_yh;
			pt4d[0].z = pt4d[3].z = pt4d[0].x;			
			pt4d[1].z = pt4d[2].z = pt4d[1].x;
			ImageToGround(pt4d,pt3d);
			ImageToGround(pt4d+1,pt3d+1);
			ImageToGround(pt4d+2,pt3d+2);
			ImageToGround(pt4d+3,pt3d+3);
			e.CreateFromPts(pt3d,4,sizeof(PT_3D));
		}
		e.TransformGrdToClt(GetCoordWnd().m_pViewCS,1);
		CRect DrawRect = pInfo->m_rectDraw;
		CSize size;
		size.cx = DrawRect.Width();
		size.cy = DrawRect.Height();
		double m[9];
		memset(m,0,sizeof(double)*9);
		double xScale = ((double)(size.cx-margin_left-margin_right))/e.Width();
		double yScale = ((double)(size.cy-margin_top-margin_bottom))/e.Height();

		if( xScale>yScale )xScale = yScale;
		else yScale = xScale;

		m[0] = xScale;
		m[2] = -xScale*(e.m_xl)+margin_left;
		m[4] = yScale;
		m[5] = -yScale*(e.m_yl)+margin_top;
		m[8] = 1.0;
		m_pTempCoordSys->Create33Matrix(m);		
		CArray<CCoordSys*,CCoordSys*> arr;
		arr.Add(GetCoordWnd().m_pViewCS);
		arr.Add(m_pTempCoordSys);
		m_pComCoordSys->CreateCompound(arr);

		CRect rcDC(0,0,size.cx,size.cy);
		CRect rectClip(rcDC);
		rectClip.DeflateRect(margin_left,margin_top,margin_right,margin_bottom);
		CRect rcView = rectClip;

		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{			
			CPreviewDC *pPrevDC = static_cast<CPreviewDC*>(pDC);
			pPrevDC->PrinterDPtoScreenDP(&rectClip.TopLeft());
			pPrevDC->PrinterDPtoScreenDP(&rectClip.BottomRight());
			CPoint ptOrg;
			::GetViewportOrgEx(pDC->m_hDC,&ptOrg);
			rectClip += ptOrg;
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		else
		{
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		pDC->SelectClipRgn(&rgn);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pComCoordSys);
		m_pPrintContext->SetDCSize(size);
		m_pPrintContext->SetViewRect(rcView);
		m_pPrintContext->SetBackColor(RGB(255,255,255));
	}
	else if (m_nPrintScope==1&&m_nPrintScale==1)
	{
		if (!m_bOverlayImgLay)
		{
			return FALSE;
		}
		DWORD dwScale = GetDocument()->GetDlgDataSource()->GetScale();
		Envelope e = GetImgLayBound();
		Envelope e0;
		{
			PT_3D pt3d[4];
			PT_4D pt4d[4];
			pt4d[0].x = pt4d[3].x = e.m_xl;			
			pt4d[1].x = pt4d[2].x = e.m_xh;
			pt4d[0].y = pt4d[1].y = e.m_yl;
			pt4d[2].y = pt4d[3].y = e.m_yh;
			pt4d[0].z = pt4d[3].z = pt4d[0].x;			
			pt4d[1].z = pt4d[2].z = pt4d[1].x;
			ImageToGround(pt4d,pt3d);
			ImageToGround(pt4d+1,pt3d+1);
			ImageToGround(pt4d+2,pt3d+2);
			ImageToGround(pt4d+3,pt3d+3);
			e0.CreateFromPts(pt3d,4,sizeof(PT_3D));
		}
		CRect DrawRect = pInfo->m_rectDraw;
		CSize size;
		size.cx = DrawRect.Width();
		size.cy = DrawRect.Height();
		int PrintDcDpix = pDC->GetDeviceCaps(LOGPIXELSX);
		int PrintDcDpiy = pDC->GetDeviceCaps(LOGPIXELSY);
		double xScale = 100.0*PrintDcDpix/(dwScale*2.54);
		double yScale = 100.0*PrintDcDpiy/(dwScale*2.54);
		double m[9];
		memset(m,0,sizeof(double)*9);
		m[0] = xScale;
		m[4] = -yScale;
		m[2] = -xScale*e0.m_xl+margin_left;
		m[5] = yScale*e0.m_yl+e0.Height()*yScale+margin_top;
		m[8] = 1.0;
		if(m_bOverlayImgLay)
		{
			double m0[9];
			memcpy(m0,m,sizeof(m));
			m[0] = m0[0]*m_lfImgMatrix[0]+m0[1]*m_lfImgMatrix[4]/*+m0[2]*m_lfImgMatrix[12]*/;
			m[1] = m0[0]*m_lfImgMatrix[1]+m0[1]*m_lfImgMatrix[5]/*+m0[2]*m_lfImgMatrix[13]*/;
			m[2] = m0[0]*m_lfImgMatrix[3]+m0[1]*m_lfImgMatrix[7]+m0[2]/**m_lfImgMatrix[15]*/;
			m[3] = m0[3]*m_lfImgMatrix[0]+m0[4]*m_lfImgMatrix[4]/*+m0[5]*m_lfImgMatrix[12]*/;
			m[4] = m0[3]*m_lfImgMatrix[1]+m0[4]*m_lfImgMatrix[5]/*+m0[5]*m_lfImgMatrix[13]*/;
			m[5] = m0[3]*m_lfImgMatrix[3]+m0[4]*m_lfImgMatrix[7]+m0[5]/**m_lfImgMatrix[15]*/;
			m[6] = m0[6]*m_lfImgMatrix[0]+m0[7]*m_lfImgMatrix[4]/*+m0[8]*m_lfImgMatrix[12]*/;
			m[7] = m0[6]*m_lfImgMatrix[1]+m0[7]*m_lfImgMatrix[5]/*+m0[8]*m_lfImgMatrix[13]*/;
			m[8] = m0[6]*m_lfImgMatrix[3]+m0[7]*m_lfImgMatrix[7]+m0[8]/**m_lfImgMatrix[15]*/;
		}
		m_pTempCoordSys->Create33Matrix(m);
		Envelope e1(e0);
		if(m_bOverlayImgLay)
			e1 = e;		
		e1.TransformGrdToClt(m_pTempCoordSys,1);
		CRect rect(e1.m_xl,e1.m_yl,e1.m_xh,e1.m_yh);
		CRect rcDC(0,0,size.cx,size.cy);
		rect = rect&rcDC;

		CRect rectClip(rcDC);
		rectClip.DeflateRect(margin_left,margin_top,margin_right,margin_bottom);
		CRect rcView = rectClip;

		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
			CPreviewDC *pPrevDC = static_cast<CPreviewDC*>(pDC);
			pPrevDC->PrinterDPtoScreenDP(&rectClip.TopLeft());
			pPrevDC->PrinterDPtoScreenDP(&rectClip.BottomRight());
			CPoint ptOrg;
			::GetViewportOrgEx(pDC->m_hDC,&ptOrg);
			rectClip += ptOrg;
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		else
		{
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		pDC->SelectClipRgn(&rgn,RGN_AND);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pTempCoordSys);
		m_pPrintContext->SetDCSize(size);
		m_pPrintContext->SetViewRect(rcView);
		m_pPrintContext->SetBackColor(RGB(255,255,255));
	}
	else if(m_nPrintScope==2&&m_nPrintScale==0)
	{	
		CRect DrawRect = pInfo->m_rectDraw;
		CSize size;
		size.cx = DrawRect.Width();
		size.cy = DrawRect.Height();
		double m[9];
		memset(m,0,sizeof(double)*9);
		double xScale = ((double)(size.cx-margin_left-margin_right))/m_rcTemp.Width();
		double yScale = ((double)(size.cy-margin_top-margin_bottom))/m_rcTemp.Height();
		if (xScale>yScale)
		{
			xScale = yScale;
		}
		else
		{
			yScale = xScale;
		}
		m[0] = xScale;
		m[2] = margin_left;
		m[4] = yScale;
		m[5] = margin_top;
		m[8] = 1.0;
		m_pTempCoordSys->Create33Matrix(m);		
		CArray<CCoordSys*,CCoordSys*> arr;
		arr.Add(GetCoordWnd().m_pViewCS);
		arr.Add(m_pTempCoordSys);
		m_pComCoordSys->CreateCompound(arr);
		Envelope e(m_rcTemp.left,m_rcTemp.right,m_rcTemp.top,m_rcTemp.bottom);
		e.TransformGrdToClt(m_pTempCoordSys,1);
		
		CRect rect(e.m_xl,e.m_yl,e.m_xh,e.m_yh);
		CRect rcDC(0,0,size.cx,size.cy);
		rect = rect&rcDC;

		CRect rectClip(rcDC);
		rectClip.DeflateRect(margin_left,margin_top,margin_right,margin_bottom);
		CRect rcView = rectClip;
		
		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
			CPreviewDC *pPrevDC = static_cast<CPreviewDC*>(pDC);
			pPrevDC->PrinterDPtoScreenDP(&rectClip.TopLeft());
			pPrevDC->PrinterDPtoScreenDP(&rectClip.BottomRight());
			CPoint ptOrg;
			::GetViewportOrgEx(pDC->m_hDC,&ptOrg);
			rectClip += ptOrg;
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		else
		{
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		pDC->SelectClipRgn(&rgn);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pComCoordSys);
		m_pPrintContext->SetDCSize(size);
		m_pPrintContext->SetViewRect(rcView);
		m_pPrintContext->SetBackColor(RGB(255,255,255));
	}
	else if (m_nPrintScope==2&&m_nPrintScale==1)
	{
		DWORD dwScale = GetDocument()->GetDlgDataSource()->GetScale();
		Envelope e;
		if (m_bOverlayImgLay)
		{
			PT_4D pt4d0[4];
			PT_4D pt4d[4];
			int sx=0, ex=0, sy=0, ey=0;
			sx = m_rcTemp.left;
			ex = m_rcTemp.right;
			sy = m_rcTemp.top;
			ey = m_rcTemp.bottom;
			pt4d0[0].x = pt4d0[3].x = sx;			
			pt4d0[1].x = pt4d0[2].x = ex;
			pt4d0[0].y = pt4d0[1].y = sy;
			pt4d0[2].y = pt4d0[3].y = ey;
			pt4d0[0].z = pt4d0[3].z = pt4d0[0].x;			
			pt4d0[1].z = pt4d0[2].z = pt4d0[1].x;
			pt4d0[0].yr= pt4d0[1].yr= pt4d0[0].y;
			pt4d0[2].yr= pt4d0[3].yr= pt4d0[2].y;
			ClientToImage(pt4d0,pt4d);
			ClientToImage(pt4d0+1,pt4d+1);
			ClientToImage(pt4d0+2,pt4d+2);
			ClientToImage(pt4d0+3,pt4d+3);
			e.CreateFromPts(pt4d,4,sizeof(PT_4D));
		}
		else
		{
			PT_3D pt3d[4];
			PT_4D pt4d[4];
			int sx=0, ex=0, sy=0, ey=0;
			sx = m_rcTemp.left;
			ex = m_rcTemp.right;
			sy = m_rcTemp.top;
			ey = m_rcTemp.bottom;
			pt4d[0].x = pt4d[3].x = sx;			
			pt4d[1].x = pt4d[2].x = ex;
			pt4d[0].y = pt4d[1].y = sy;
			pt4d[2].y = pt4d[3].y = ey;
			pt4d[0].z = pt4d[3].z = pt4d[0].x;			
			pt4d[1].z = pt4d[2].z = pt4d[1].x;
			pt4d[0].yr= pt4d[1].yr= pt4d[0].y;
			pt4d[2].yr= pt4d[3].yr= pt4d[2].y;
			ClientToGround(pt4d,pt3d);
			ClientToGround(pt4d+1,pt3d+1);
			ClientToGround(pt4d+2,pt3d+2);
			ClientToGround(pt4d+3,pt3d+3);
			e.CreateFromPts(pt3d,4,sizeof(PT_3D));
		}
		Envelope e0;
		{
			PT_3D pt3d[4];
			PT_4D pt4d[4];
			int sx=0, ex=0, sy=0, ey=0;
			sx = m_rcTemp.left;
			ex = m_rcTemp.right;
			sy = m_rcTemp.top;
			ey = m_rcTemp.bottom;
			pt4d[0].x = pt4d[3].x = sx;			
			pt4d[1].x = pt4d[2].x = ex;
			pt4d[0].y = pt4d[1].y = sy;
			pt4d[2].y = pt4d[3].y = ey;
			pt4d[0].z = pt4d[3].z = pt4d[0].x;			
			pt4d[1].z = pt4d[2].z = pt4d[1].x;
			pt4d[0].yr= pt4d[1].yr= pt4d[0].y;
			pt4d[2].yr= pt4d[3].yr= pt4d[2].y;
			ClientToGround(pt4d,pt3d);
			ClientToGround(pt4d+1,pt3d+1);
			ClientToGround(pt4d+2,pt3d+2);
			ClientToGround(pt4d+3,pt3d+3);
			e0.CreateFromPts(pt3d,4,sizeof(PT_3D));
		}
		e0 = e;

		CRect DrawRect = pInfo->m_rectDraw;
		CSize size;
		size.cx = DrawRect.Width();
		size.cy = DrawRect.Height();
		int PrintDcDpix = pDC->GetDeviceCaps(LOGPIXELSX);
		int PrintDcDpiy = pDC->GetDeviceCaps(LOGPIXELSY);
		double xScale = 100.0*PrintDcDpix/(dwScale*2.54);
		double yScale = 100.0*PrintDcDpiy/(dwScale*2.54);
		double m[9];
		memset(m,0,sizeof(double)*9);
		m[0] = xScale;
		m[4] = -yScale;
		m[2] = -xScale*e0.m_xl+margin_left;
		m[5] = yScale*e0.m_yl+e0.Height()*yScale+margin_top;
		m[8] = 1.0;
		if(m_bOverlayImgLay)
		{
			double m0[9];
			memcpy(m0,m,sizeof(m));
			m[0] = m0[0]*m_lfImgMatrix[0]+m0[1]*m_lfImgMatrix[4]/*+m0[2]*m_lfImgMatrix[12]*/;
			m[1] = m0[0]*m_lfImgMatrix[1]+m0[1]*m_lfImgMatrix[5]/*+m0[2]*m_lfImgMatrix[13]*/;
			m[2] = m0[0]*m_lfImgMatrix[3]+m0[1]*m_lfImgMatrix[7]+m0[2]/**m_lfImgMatrix[15]*/;
			m[3] = m0[3]*m_lfImgMatrix[0]+m0[4]*m_lfImgMatrix[4]/*+m0[5]*m_lfImgMatrix[12]*/;
			m[4] = m0[3]*m_lfImgMatrix[1]+m0[4]*m_lfImgMatrix[5]/*+m0[5]*m_lfImgMatrix[13]*/;
			m[5] = m0[3]*m_lfImgMatrix[3]+m0[4]*m_lfImgMatrix[7]+m0[5]/**m_lfImgMatrix[15]*/;
			m[6] = m0[6]*m_lfImgMatrix[0]+m0[7]*m_lfImgMatrix[4]/*+m0[8]*m_lfImgMatrix[12]*/;
			m[7] = m0[6]*m_lfImgMatrix[1]+m0[7]*m_lfImgMatrix[5]/*+m0[8]*m_lfImgMatrix[13]*/;
			m[8] = m0[6]*m_lfImgMatrix[3]+m0[7]*m_lfImgMatrix[7]+m0[8]/**m_lfImgMatrix[15]*/;
		}
		m_pTempCoordSys->Create33Matrix(m);
		Envelope e1(e0);
		if(m_bOverlayImgLay)
			e1 = e;		
		e1.TransformGrdToClt(m_pTempCoordSys,1);
		CRect rect(e1.m_xl,e1.m_yl,e1.m_xh,e1.m_yh);
		CRect rcDC(0,0,size.cx,size.cy);
		rect = rect&rcDC;

		CRect rectClip(rcDC);
		rectClip.DeflateRect(margin_left,margin_top,margin_right,margin_bottom);
		CRect rcView = rectClip;

		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
			CPreviewDC *pPrevDC = static_cast<CPreviewDC*>(pDC);
			pPrevDC->PrinterDPtoScreenDP(&rectClip.TopLeft());
			pPrevDC->PrinterDPtoScreenDP(&rectClip.BottomRight());
			CPoint ptOrg;
			::GetViewportOrgEx(pDC->m_hDC,&ptOrg);
			rectClip += ptOrg;
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		else
		{
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		pDC->SelectClipRgn(&rgn,RGN_AND);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pTempCoordSys);		
		m_pPrintContext->SetDCSize(CSize(rect.Width(),rect.Height()));
		m_pPrintContext->SetViewRect(rcView);
		m_pPrintContext->SetBackColor(RGB(255,255,255));	
	}

	m_pPrintContext->m_fDrawCellAngle = -m_gsa.lfAngle;

	return TRUE;
}

void CVectorView_new::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if(!PrePrint(pDC,pInfo))
		return;

	HDC hdcPrinter = pDC->GetSafeHdc();
	::SetStretchBltMode(hdcPrinter,STRETCH_DELETESCANS);
	if (hdcPrinter == NULL)
	{
		MessageBox(_T("No Printers Are Found!"));
		return;
	}

	{
		CSize oldSize = m_pPrintContext->GetDCSize();
		CRect oldRect; 
		m_pPrintContext->GetViewRect(oldRect);

		if(m_bOverlayImgLay)
		{
			m_pPrintContext->TurnonMemBmpBoard();
			CDrawingContext *pOldContext = NULL;

			CArray<int,int> cacheTypes;

			for( int i=0; i<m_arrPImgLays.GetSize(); i++)
			{
				cacheTypes.Add(m_arrPImgLays[i]->GetCacheType());
				m_arrPImgLays[i]->ClearAll();
				m_arrPImgLays[i]->DestroyCache();				
			}

			CCoordSys *pSaveCS = m_pPrintContext->GetCoordSys();
			CCoordSys cs,cs0;			
			int col = 0, row = 0,stepx = 512,stepy = 512;
			int colNum = oldSize.cx/stepx;
			int rowNum = oldSize.cy/stepy;
			m_pPrintContext->SetDCSize(CSize(stepx,stepy));
			m_pPrintContext->SetViewRect(CRect(0,0,stepx,stepy));
			int right,bottom;

			for (;col<=colNum;col++ )
			{			
				for (row=0;row<=rowNum;row++)
				{
					if (col==colNum)
					{
						right = oldRect.right;	
					}
					else
						right = (col+1)*stepx;
					if (row==rowNum)
					{
						bottom = oldRect.bottom;
					}
					else
						bottom = (row+1)*stepy;
					if (pSaveCS->GetCoordSysType()==COORDSYS_33MATRIX)
					{
						double m[9];
						Matrix33FromMove(-col*stepx,-row*stepy,m);
						cs.CopyFrom(pSaveCS);
						cs.MultiplyMatrix(m);
					}
					else if (pSaveCS->GetCoordSysType()==COORDSYS_COMPOUND)
					{
						double m[9];
						Matrix33FromMove(-col*stepx,-row*stepy,m);
						cs0.Create33Matrix(m);
						CArray<CCoordSys*,CCoordSys*> arr;
						
						arr.Add(pSaveCS);arr.Add(&cs0);
						cs.CreateCompound(arr);
					}
					else
						return;
					m_pPrintContext->SetCoordSys(&cs);
					m_pPrintContext->EraseBackground();

					for( int i=0; i<m_arrPImgLays.GetSize(); i++)
					{
						m_arrPImgLays[i]->SetContext(m_pPrintContext);
						m_arrPImgLays[i]->OnChangeCoordSys(FALSE);
						m_arrPImgLays[i]->Draw();
					}

					BitBlt(hdcPrinter,col*stepx,row*stepy,right -col*stepx ,bottom-row*stepy,m_pPrintContext->GetMemBoardDC(),0,0,SRCCOPY);
				}
			}
			m_pPrintContext->TurnoffMemBmpBoard();
			m_pPrintContext->SetCoordSys(pSaveCS);
			m_pPrintContext->SetDCSize(oldSize);
			m_pPrintContext->SetViewRect(oldRect);

			for( i=0; i<m_arrPImgLays.GetSize(); i++)
			{
				m_arrPImgLays[i]->SetContext(m_pContext);
				m_arrPImgLays[i]->InitCache(cacheTypes[i],CSize(512,512),100);
				m_arrPImgLays[i]->OnChangeCoordSys(FALSE);
				m_arrPImgLays[i]->ClearAll();
			}
		}

		BOOL bSaveFlag = g_Fontlib.EnableTextBmpCache(FALSE);

		CDrawingContext *pOldContext = m_vectLayL2.GetContext();
		int cacheType = m_vectLayL2.GetCacheType();

		m_vectLayL2.DestroyCache();	
		
		CDlgDoc *pDoc = GetDocument();

		m_vectLayL2.SetContext(m_pPrintContext);		
		m_vectLayL2.Init(pDoc,(CDlgDataQuery*)pDoc->GetDataQuery(),m_pPrintContext->GetCoordSys(),m_pPrintContext->GetCoordSys(),this,
			(PFDrawFtr)&CVectorView_new::Callback_DrawFeature,(PFCheckLayerVisible)&CVectorView_new::Callback_CheckLayerVisible);

		m_vectLayL2.DelObj(HANDLE_BOUND);
		
		m_vectLayL2.OnChangeCoordSys(FALSE);

		m_pPrintContext->BeginDrawing();
		
		float fScale = (double)GetDocument()->GetDlgDataSource()->GetScale()/1000.0;
		// 线宽为0 默认为0.15mm
		//m_pPrintContext->SetPrint(TRUE,0.15*fScale);
		
		m_vectLayL2.Draw();

		//m_pPrintContext->SetPrint(FALSE);
		m_pPrintContext->EndDrawing();

		m_vectLayL2.SetContext(pOldContext);
		m_vectLayL2.InitCache( cacheType,CSize(256,256),100);
		m_vectLayL2.Init(pDoc, (CDlgDataQuery*)pDoc->GetDataQuery(), m_pLeftCS, GetCoordWnd().m_pViewCS, this,
			(PFDrawFtr)&CVectorView_new::Callback_DrawFeature, (PFCheckLayerVisible)&CVectorView_new::Callback_CheckLayerVisible);

		GrBuffer buf;
		DrawBkgnd(&buf);

		AddObjtoVectLay2(HANDLE_BOUND,&buf,TRUE);

		m_vectLayL2.OnChangeCoordSys(FALSE);

		g_Fontlib.EnableTextBmpCache(bSaveFlag);
	}
}

static void InitOpenGLWithMemoryDC(HDC hdc)
{
	CBitmap* pBitmap = CDC::FromHandle(hdc)->GetCurrentBitmap() ;
	BITMAP bmInfo ;
	pBitmap->GetObject(sizeof(BITMAP), &bmInfo) ;
    PIXELFORMATDESCRIPTOR pfd=
    {	
		sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_BITMAP|
			PFD_SUPPORT_OPENGL| PFD_SUPPORT_GDI,
			PFD_TYPE_RGBA,
			(BYTE)bmInfo.bmBitsPixel,
			0,0,0,0,0,0,
			0,0,0,0,0,0,0,
			32,
			0,0,
			PFD_MAIN_PLANE,
			0,
			0,0,0
    };
    int pixelFormat=::ChoosePixelFormat(hdc,&pfd);
    ::SetPixelFormat(hdc,pixelFormat,&pfd);
}



void CVectorView_new::OnPreInnerCmd(UINT nInnerCmd)
{
	CInteractiveView::OnPreInnerCmd(nInnerCmd);
	if( m_nInnerCmd==IDC_BUTTON_REFRESH )
	{
		m_markLayL2.ClearAll();
		m_vectLayL2.ClearAll();
	}
}



void CVectorView_new::AdjustCellScale()
{
	int		i;
	double	xSrc[4] ,ySrc[4] ;
	double	xDest[4],yDest[4];
	
	PT_4D pts[4],pts1[4];
	PT_3D pts2[4];
	
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


void CVectorView_new::ShowMapGrid()
{
	CDlgDoc* pdc = GetDocument();
	if(pdc==NULL || pdc->GetDlgDataSource() == NULL)
		return;
	//
	if(m_ShowMapGrid)
	{
		std::vector<CDlgDataSource*> datasources;
		for(int i=0; i<pdc->GetDlgDataSourceCount();++i)
		{
			datasources.push_back(pdc->GetDlgDataSource(i));
		}
		//
		//m_fWid 的单位是cm
		double lfWid = m_fWid*pdc->GetDlgDataSource()->GetScale()*0.01;
	    double lfHei = m_fHei*pdc->GetDlgDataSource()->GetScale()*0.01;
	    //
		Envelope e;
		e.m_xl = 1e307;
		e.m_xh = -1e307;
		e.m_yl = 1e307;
		e.m_yh = -1e307;
		
		CDataQuery *pDQ = pdc->GetDataQuery();
		if( !pDQ )return;
		
		CFeature *pFtr = NULL;
		POSITION pos = pDQ->GetFirstObjPos();
		while( pos )
		{
			pFtr = pDQ->GetNextObjByPos(pos);
			if( pFtr )
			{		
				CGeometry *pGeo = pFtr->GetGeometry();
				if( pGeo )
				{
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pGeo->GetShape(arrPts);
					Envelope e1;
					e1.CreateFromPts(arrPts.GetData(),arrPts.GetSize(),sizeof(PT_3DEX),3);
					e.Union(&e1,3);
				}
			}
		}

		if(e.m_xh<=e.m_xl || e.m_yh<=e.m_yl || (e.m_xh-e.m_xl)/lfWid >100 || (e.m_yh-e.m_yl)/lfHei>100)
			e = pdc->GetDlgDataSource()->GetBound();
		//
        e.m_xl = floor(e.m_xl/lfWid)*lfWid; e.m_yl = floor(e.m_yl/lfHei)*lfHei;
	    e.m_xh = ceil(e.m_xh/lfWid)*lfWid; e.m_yh = ceil(e.m_yh/lfHei)*lfHei;
		//
		e.m_xl -= 10*lfWid;
		e.m_xh += 10*lfWid;
		e.m_yl -= 10*lfHei;
		e.m_yh += 10*lfHei;
		//
		GrBuffer buf;
		COLORREF color = RGB(128,128,128);
	    buf.SetAllColor(color);
	    //
		MapDecorate::CMapDecorator mapdecorator;
		mapdecorator.InitData(pdc->GetDlgDataSource());
		mapdecorator.LoadParams(pdc->GetDlgDataSource()->GetScale());
		mapdecorator.m_fMapWid = m_fWid;
	    mapdecorator.m_fMapHei = m_fHei;
	    //
		PT_3D mappts[4];
		double x, y;
		for( y=e.m_yl; y<e.m_yh; y+=lfHei )
		{
			for( x=e.m_xl; x<e.m_xh; x+=lfWid )
			{
				mappts[0].x = x; mappts[0].y = y;
				mappts[1].x = x+lfWid; mappts[1].y = y;
				mappts[2].x = x+lfWid; mappts[2].y = y+lfHei;
				mappts[3].x = x; mappts[3].y = y+lfHei;
				//
				buf.BeginLineString(color,0);
				buf.MoveTo(&mappts[0]);
				buf.LineTo(&mappts[1]);
				buf.LineTo(&mappts[2]);
				buf.LineTo(&mappts[3]);
				buf.LineTo(&mappts[0]);
				buf.End(FALSE);
				
		        buf.RefreshEnvelope();
				//
				if(m_ShowMapName)
				{
					PT_3D pt0(x,y,0);
		            CString text = mapdecorator.CalcRectMapNumber(pt0,pdc->GetDlgDataSource()->GetScale());
					PT_3D pt1((mappts[0].x+mappts[1].x)*0.5,(mappts[0].y+mappts[2].y)*0.5,0);
					TextSettings text_setting;
					text_setting.fHeight = 60;
					text_setting.nAlignment = TAH_MID|TAV_MID;
					buf.Text(color,&pt1,text,&text_setting);
				}
			}
		}
		//
        pdc->UpdateAllViews(NULL,hc_AddGraph,(CObject*)&buf); 
		mapgrid_showed = TRUE;
	}
	else
	{
        pdc->UpdateAllViews(NULL,hc_DelGraph);
		mapgrid_showed = FALSE;
	}
	//

}


void CVectorView_new::OnMarkReffile()
{
	m_bMarkReffile = !m_bMarkReffile;

	AfxGetApp()->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_MARKREFFILE,m_bMarkReffile);

	OnUpdate(this,hc_UpdateRefFileText,NULL);
}


void CVectorView_new::OnUpdateMarkReffile(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bMarkReffile);
}

void CVectorView_new::OnFillColor()
{
	m_bFillColor = !m_bFillColor;
	
	OnUpdate(this,hc_UpdateAllObjects,NULL);
}

void CVectorView_new::OnUpdateFillColor(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bFillColor);
}

void CVectorView_new::OnShowMapGrid()
{
   m_ShowMapGrid = !m_ShowMapGrid;
   m_ShowMapName = m_ShowMapGrid;
   ShowMapGrid();
   AfxGetApp()->WriteProfileInt(REGPATH_VIEW_VECT,"m_ShowMapGrid", m_ShowMapGrid);
   AfxGetApp()->WriteProfileInt(REGPATH_VIEW_VECT,"m_ShowMapName", m_ShowMapName);
}

void CVectorView_new::OnUpdateMapGrid(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_ShowMapGrid);
	//
	if(m_ShowMapGrid && !mapgrid_showed)
		ShowMapGrid();
}

void CVectorView_new::OnShowMapName()
{
   if(!m_ShowMapGrid)
	   return;
   //
   GetDocument()->UpdateAllViews(NULL,hc_DelGraph);
   //
   m_ShowMapName = !m_ShowMapName;
   ShowMapGrid();
   AfxGetApp()->WriteProfileInt(REGPATH_VIEW_VECT,"m_ShowMapName", m_ShowMapName);
}

void CVectorView_new::OnUpdateMapName(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_ShowMapName);
}

void CVectorView_new::OnShowLineWidth()
{
	if(!m_pContext)return;
	
    m_pContext->m_bNoLineWeight = !m_pContext->m_bNoLineWeight;
	m_pContext->GetLeftContext()->m_bNoLineWeight = m_pContext->m_bNoLineWeight; 
	m_pContext->GetRightContext()->m_bNoLineWeight = m_pContext->m_bNoLineWeight;
	
	CWinApp *pApp = AfxGetApp();
	if( pApp )pApp->WriteProfileInt(m_strRegSection,"NoLineWeight",m_pContext->m_bNoLineWeight);
	
	RefreshDrawingLayers();
	Invalidate(FALSE);
}

void CVectorView_new::OnUpdateLineWidth(CCmdUI* pCmdUI)
{
	if(!m_pContext)return;
    pCmdUI->SetCheck(!m_pContext->m_bNoLineWeight);
}

bool CVectorView_new::isVectorViewActive()
{
	CView* pView = GetActiveView();
	
	if (pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CVectorView_new::UpdateSnapDrawing()
{
	CBaseView::UpdateSnapDrawing();
}


void CVectorView_new::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CBaseView::OnMButtonDown(nFlags, point);
}


void CVectorView_new::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CBaseView::OnMButtonUp(nFlags, point);
	UpdateMove();
	
}

void CVectorView_new::ScrollView(int dx, int dy, int dp, int dq)
{
	CInteractiveView::ScrollView(dx, dy, dp, dq);

	UpdateMove();
}

void CVectorView_new::Scroll(int dx, int dy, int dp, int dq /*= 0*/, int step /*= 6*/)
{
	CSceneView::Scroll(dx, dy, dp, dq, step);
	UpdateMove();
}

void CVectorView_new::ZoomChange(CPoint point, double change, BOOL bToCenter /*= TRUE*/)
{
	CSceneView::ZoomChange(point, change, bToCenter);
	UpdateMove(true);
}

void CVectorView_new::ZoomNext(CPoint point, int dir, BOOL bToCenter /*= TRUE*/)
{
	CSceneView::ZoomNext(point, dir, bToCenter);
	UpdateMove(true);
}

void CVectorView_new::UpdateMove(bool withScale)
{
	CRect rect;
	GetClientRect(rect);
	//中心位置
	double cx = rect.Width() / 2.0 + 0.5;
	double cy = rect.Height() / 2.0 + 0.5;
	PT_4D pt(cx, cy, 0);
	PT_3D grdpt;
	ClientToGround(&pt, &grdpt);
	if (withScale)
		GetDocument()->UpdateAllViews(NULL, 79, (CObject*)&grdpt);
	else
		GetDocument()->UpdateAllViews(NULL, 78, (CObject*)&grdpt);
}


void CVectorView_new::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default

	CBaseView::OnNcCalcSize(bCalcValidRects, lpncsp);
}


//void CVectorView_new::OnNcPaint()
//{
//	// TODO: Add your message handler code here
//	// Do not call CBaseView::OnNcPaint() for painting messages
//}


void CVectorView_new::OnSize(UINT nType, int cx, int cy)
{
	CBaseView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}
