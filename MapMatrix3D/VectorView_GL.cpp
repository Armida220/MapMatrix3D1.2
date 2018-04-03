// VectorView_Print.cpp: implementation of the CVectorView_GL class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "cadlib.h"
#include "glew.h"
#include "EditBaseDoc.h"
#include "VectorView_GL.h"
#include "Feature.h"
#include "DrawLineDC.h"
#include "ExMessage.h"
#include "DlgCommand.h"
#include "DlgDataSource.h "
#include "RegDef.h"
#include "RegDef2.h"
#include "SymbolLib.h "
#include "DlgOverlayOrtho.h "
#include "IO.H "
#include "DlgSetXYZ.h "
#include "VDBaseDef.h "
#include "GlobalFunc.h "
#include "MainFrm.h"
#include "DrawingDef.h"
#include "StereoView.h"
#include "SmartViewFunctions.h"
#include "DlgExportRaster.h "
#include "tiffio.h "
#include "DlgDisplayContoursSpecially.h "
#include "DlgPrintOption.h "
#include "DlgOverMutiImgLay.h "


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define REG_CMDCLASS(pWorker,id,clsname)  (pWorker)->RegisterCommand((id),clsname::Create)

#define PRINTINDENT   5

/////////////////////////////////////////////////////////////////////////////
// CVectorView_GL

IMPLEMENT_DYNCREATE(CVectorView_GL, CBaseView)

BEGIN_MESSAGE_MAP(CVectorView_GL, CBaseView)
	//{{AFX_MSG_MAP(CVectorView_GL)
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
	ON_COMMAND(ID_STEREO_REFRESH,/*CSceneView::*/OnRefresh)
	ON_COMMAND(ID_STEREO_ZOOMIN,/*CSceneView::*/OnZoomIn)
	ON_COMMAND(ID_STEREO_ZOOMOUT,/*CSceneView::*/OnZoomOut)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVectorView_GL construction/destruction

CVectorViewCenterPoint1::CVectorViewCenterPoint1(CVectorView_GL *p)
{
	pView = p;
	Save();
}


CVectorViewCenterPoint1::~CVectorViewCenterPoint1()
{
	Restore();
}

void CVectorViewCenterPoint1::Save()
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


void CVectorViewCenterPoint1::Restore()
{
	if( !pView )return;
	pView->DriveToXyz(&ptCenter);
}

CVectorView_GL::CVectorView_GL()
{
	m_xgoff		= 0;
	m_ygoff     = 0;
//	m_lfAngle   = 0;

	m_bViewRotate = FALSE;
	m_bOverlayImgLay = FALSE;

// 	memset(&m_imgPos,0,sizeof(m_imgPos));
// 	m_imgPos.lfImgMatrix[0]	= m_imgPos.lfImgMatrix[3] = 1.0;
	
	m_bVectLayCompress = TRUE;
	
	m_bStartLoadVect = FALSE;
	
	m_clrBound = gdef_clrBndVect;
	strcpy(m_strRegSection,REGPATH_VIEW_VECT);
	
	memset(m_ptBounds,0,sizeof(m_ptBounds));
	
//	m_bShowKeyPoint = AfxGetApp()->GetProfileInt(m_strRegSection,"DisplayKeyPoint",FALSE);
	
	m_bDisplayByHeight = FALSE;
	
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

	m_nMainImageIdx = -1;
}

CVectorView_GL::~CVectorView_GL()
{
	if( m_pRealCS )delete m_pRealCS;
	if (m_pPrintContext) delete m_pPrintContext;
	if (m_pTempCoordSys) delete m_pTempCoordSys;	
	if (m_pComCoordSys) delete m_pComCoordSys;
}

BOOL CVectorView_GL::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CBaseView::PreCreateWindow(cs);
}


void CVectorView_GL::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CVectorView_GL printing
void CVectorView_GL::OnFilePrint()
{
	CDlgPrintOption dlg;
	dlg.m_bOverImg = m_bOverlayImgLay;
	if (dlg.DoModal()!=IDOK)
	{
		return;
	}
	m_nPrintScope = dlg.m_nPrintScope;
	m_nPrintScale = dlg.m_nPrintScale;
	GetClientRect(&m_rcTemp);
	CBaseView::OnFilePrint();
}

void CVectorView_GL::OnFilePrintPreview()
{
	CDlgPrintOption dlg;
	dlg.m_bOverImg = m_bOverlayImgLay;
	if (dlg.DoModal()!=IDOK)
	{
		return;
	}
	m_nPrintScope = dlg.m_nPrintScope;
	m_nPrintScale = dlg.m_nPrintScale;
	GetClientRect(&m_rcTemp);
	CBaseView::OnFilePrintPreview();	
}

BOOL CVectorView_GL::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVectorView_GL::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{	
	if(m_pPrintContext) delete m_pPrintContext;
	m_pPrintContext = new CPrintingDrawingContext;
	if (m_pTempCoordSys)delete m_pTempCoordSys;
	m_pTempCoordSys = new CCoordSys;
	if (m_pComCoordSys)delete m_pComCoordSys;
	m_pComCoordSys = new CCoordSys;
	
	if (pInfo->m_bPreview)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
		pMain->GetStatusBar().ShowWindow(SW_HIDE);
	}
}

void CVectorView_GL::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	if(m_pPrintContext)delete m_pPrintContext;
	m_pPrintContext = NULL;
	if(m_pTempCoordSys) delete m_pTempCoordSys;
	m_pTempCoordSys = NULL;
	if(m_pComCoordSys) delete m_pComCoordSys;
	m_pComCoordSys = NULL;
	if (pInfo->m_bPreview)
	{
		CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
		pMain->GetStatusBar().ShowWindow(SW_SHOW);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVectorView_GL diagnostics

#ifdef _DEBUG
void CVectorView_GL::AssertValid() const
{
	CBaseView::AssertValid();
}

void CVectorView_GL::Dump(CDumpContext& dc) const
{
	CBaseView::Dump(dc);
}

CDlgDoc* CVectorView_GL::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDlgDoc)));
	return (CDlgDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVectorView_GL message handlers


CWorker *CVectorView_GL::GetWorker()
{
	return GetDocument();
}

void CVectorView_GL::OnInitialUpdate() 
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

	/*
	//将矢量映射到客户区大小，优化显示速度
	Envelope e = pDS->GetBound();
	double m1[16], m2[16], m3[16];
	Matrix44FromMove(-e.m_xl,-e.m_yl,0,m1);

	double kx = e.Width()/400, ky = e.Height()/300;
	if( kx<1.0 )kx = 1.0;
	if( ky<1.0 )ky = 1.0;
	Matrix44FromZoom(1.0/kx,1.0/ky,1.0,m2);
	matrix_multiply(m2,m1,4,m3);

	memcpy(m_lfImgRMatrix,m3,sizeof(m_lfImgRMatrix));
	matrix_reverse(m_lfImgRMatrix,4,m_lfImgMatrix);

	m_gsa.lfGKX = m_gsa.lfGKY = sqrt(fabs(m_lfImgRMatrix[0]*m_lfImgRMatrix[5] - m_lfImgRMatrix[1]*m_lfImgRMatrix[4]));
	m_pContext->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->m_fDrawCellKY = m_gsa.lfGKY;
	
	m_pContext->GetLeftContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;
	
	m_pContext->GetRightContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->GetRightContext()->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->GetRightContext()->m_fDrawCellKY = m_gsa.lfGKY;
	*/

	CBaseView::OnInitialUpdate();
	OnZoomFit();
}


void CVectorView_GL::OnSetXYZ()
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


void CVectorView_GL::OnLButtonDown(UINT nFlags, CPoint point) 
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

	CBaseView::OnLButtonDown(nFlags, point);
	PT_3D ptRet = m_gCurPoint;
	
	CDlgDoc *pDoc = GetDocument();
/*	CCommand *pCurCmd = pDoc->GetCurrentCommand();*/
	if( pDoc->IsSelectorOpen() )
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
}

void CVectorView_GL::OnMouseMove(UINT nFlags, CPoint point) 
{
	static TRACKMOUSEEVENT Track;
	Track.cbSize        = sizeof(TRACKMOUSEEVENT);
	Track.dwFlags       = TME_LEAVE;
	Track.hwndTrack     = m_hWnd;
	Track.dwHoverTime   = HOVER_DEFAULT;
	_TrackMouseEvent (&Track);
	
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
	
	SetCrossPos(point.x+0.5, point.y+0.5, point.x+0.5);
	UpdateCross();

	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;

	PT_4D pt;
	GroundToClient(&m_gCurPoint,&pt);
	pt.x=point.x+0.5; pt.y=point.y+0.5;
	ClientToGround(&pt,&m_gCurPoint);
	
	if( !m_strDemPath.IsEmpty() )
		m_gCurPoint.z = m_dem.GetZ(m_gCurPoint.x,m_gCurPoint.y);
	
	m_ptLastMouse = point;
	
	if( IsDrawStreamline() )
	{
		CInteractiveView::OnMouseMove(nFlags,point);
	
		return;
	}
	
	CBaseView::OnMouseMove(nFlags, point);	
}

void CVectorView_GL::OnRButtonDown(UINT nFlags, CPoint point) 
{
// 	PT_4D pt;
// 	pt.x = point.x, pt.y = point.y, pt.z = 0, pt.yr = 0;
// 	ClientToGround(&pt,&m_gCurPoint);

	m_nInputType = STEREOVIEW_INPUT_SYSMOUSE;
	
	CBaseView::OnRButtonDown(nFlags, point);
}

void CVectorView_GL::OnPaint() 
{
	CBaseView::OnPaint();
}

BOOL CVectorView_GL::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	float change = GetComboNextZoom(zDelta>0?1:0)/GetZoomRate();
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
	OnPostInnerCmd();
//	m_nInnerCmd = save;

	return CBaseView::OnMouseWheel(nFlags, zDelta, pt);
}

void CVectorView_GL::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
/*	double m[16], m2[16], mt[16];
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
	default:return;
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
	
	matrix_multiply(m,m_matrix,4,mt);
	memcpy(m_matrix,mt,sizeof(mt));
	matrix_reverse(m_matrix,4,m_rmatrix);
	
	Invalidate(FALSE);
*/	
	CBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CVectorView_GL::DrawBkgnd(GrBuffer *buf)
{
	
	//buf->EnableWidFromGrd(FALSE);
// 	buf->SetAllColor(RGB(255,0,0));
// 	buf->SetAllLineWidth(FALSE,2);
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
				buf->BeginLineString(RGB(255,0,0),2,0);
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

BOOL CVectorView_GL::IsDrawStreamline()
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
		if( m_nInputType==STEREOVIEW_INPUT_3DMOUSE &&
			((CDrawCommand*)pCurCmd)->GetCurPenCode()==penStream /*&& info.pObj && */
			/*info.pObj->GetDataPointSum()>0*/ )
		{
			m_b3DDraw = TRUE;
			return TRUE;
		}
		
		if( m_b3DDraw &&
			((CDrawCommand*)pCurCmd)->GetCurPenCode()==penStream /*&& info.pObj && */
			/*info.pObj->GetDataPointSum()>0*/ )
		{
			return TRUE;
		}
	}
	
	m_b3DDraw = FALSE;
	return FALSE;
}

LRESULT CVectorView_GL::OnDeviceLFootDown(WPARAM wParam, LPARAM lParam)
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


LRESULT CVectorView_GL::OnDeviceRFootDown(WPARAM wParam, LPARAM lParam)
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



LRESULT CVectorView_GL::OnDeviceMove(WPARAM wParam, LPARAM lParam)
{
	if( m_bDriveOnlyCursor )
	{
		return OnDeviceMove2(wParam, lParam);
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

LRESULT CVectorView_GL::OnDeviceMove2(WPARAM wParam, LPARAM lParam)
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


LRESULT CVectorView_GL::OnMoveCursorToCenter2(WPARAM, LPARAM)
{
	if( !m_bOverlayImgLay || m_bDriveOnlyStereo )
		return 1;
	
	PT_3D pt3d = m_gCurPoint;
	DriveToXyz(&pt3d,TRUE);
	Invalidate(FALSE);
	return 0;
}

void CVectorView_GL::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
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
			DriveToXyz(pt);
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
			int value;
			value = m_clrHilite;
			value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_HILITECOLOR,value);
			m_clrHilite = value;
			
			value = m_clrDragLine;
			value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_DRAGCOLOR,value);
			m_clrDragLine = value;

			value = m_clrBound;
			value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_BOUNDCOLOR,value);
			m_clrBound = value;	
			
			m_bSymbolize = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_SYMBOLIZE,m_bSymbolize);
			OnUpdate(this,hc_UpdateOverlayBound,NULL);

			m_bDriveOnlyStereo = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYSTEREO,TRUE);
			
			m_bDriveOnlyCursor = AfxGetApp()->GetProfileInt(REGPATH_OTHER,REGITEM_DEVICEONLYCURSOR,FALSE);
			
			m_bDisableMouseIn3DDraw = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DIABLEMOUSE,m_bDisableMouseIn3DDraw);
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

			CVectorViewCenterPoint1 ptSave(this);
			
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

			CVectorViewCenterPoint1 ptSave(this);			
				
			
			
			RecalcScrollBar(FALSE);
			
			//重新装载矢量数据
			LoadDocData();
			
			Invalidate(FALSE);
		}
		break;
	case hc_UpdateAllObjects:
	case hc_UpdateAllObjects_Visible:
	case hc_UpdateAllObjects_VectorView:
		{
			//客户中心作为焦点，位置不变，这里先记录此位置
			CVectorViewCenterPoint1 ptSave(this);
			
			//重新装载矢量数据
			LoadDocData();

			RecalcScrollBar(FALSE);

			Invalidate(FALSE);
		}
		break;
	case hc_UpdateOverlayBound:
		{
			DelObjfromVectLay(HANDLE_BOUND);
			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();
				
			GrBuffer buf;
			DrawBkgnd(&buf);

			AddObjtoVectLay(HANDLE_BOUND,&buf);
			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();
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
// 			if( pHint )
// 			{
// 				double *params = (double*)pHint;
// 
// 				params[0] = m_imgPos.lfImgLayOX;
// 				params[1] = m_imgPos.lfImgLayOY;
// 				params[2] = m_imgPos.lfImgMatrix[0];
// 				params[3] = m_imgPos.lfImgMatrix[1];
// 				params[4] = m_imgPos.lfImgMatrix[2];
// 				params[5] = m_imgPos.lfImgMatrix[3];
// 			}
		}
		break;
	case hc_RestoreImagePosition:
		{
			if( pHint )
			{
				CVectorViewCenterPoint1 ptSave(this);

				double *params = (double*)pHint;
				
// 				m_imgPos.lfImgLayOX = params[0];
// 				m_imgPos.lfImgLayOY = params[1];
// 				m_imgPos.lfImgMatrix[0] = params[2];
// 				m_imgPos.lfImgMatrix[1] = params[3];
// 				m_imgPos.lfImgMatrix[2] = params[4];
// 				m_imgPos.lfImgMatrix[3] = params[5];
// 
// 				SetImagePosition();
// 
// 				RefreshImageOverlay();
			}
		}
		break;
	case hc_SetImagePosition:
		if( pHint )
		{
// 			CVectorViewCenterPoint1 ptSave(this);
// 			
// 			PT_3D *pts = (PT_3D*)pHint;
// 			PT_4D pt4ds[2];
// 
// 			//确定拉伸比例(假定XY方向具有相同的拉伸比例)
// 			CSize szBound = m_imgLay.GetImageSize();
// 			GroundToImage(pts+1,pt4ds);
// 			GroundToImage(pts+3,pt4ds+1);
// 
// 			double d = (pt4ds[1].x-pt4ds[0].x)*(pt4ds[1].x-pt4ds[0].x)+(pt4ds[1].y-pt4ds[0].y)*(pt4ds[1].y-pt4ds[0].y);
// 			if( fabs(d)<1e-10 )break;
// 
// 			double scale = sqrt(((pts[2].x-pts[0].x)*(pts[2].x-pts[0].x)+(pts[2].y-pts[0].y)*(pts[2].y-pts[0].y))/
// 				(d));
// 
// 			if( fabs(scale)<1e-10 )break;
// 
// 			//确定旋转角
// 			double ang = GraphAPI::GGetAngle(pt4ds[0].x,pt4ds[0].y,pt4ds[1].x,pt4ds[1].y) - GraphAPI::GGetAngle(pts[0].x,pts[0].y,pts[2].x,pts[2].y);
// 
// 			ang = -ang;
// 
// 			//如果影像事先已经旋转，就要加上这个旋转角
// 			//ang += (m_nRasterAng*PI/180);
// 			
// 			//确定新的原点
// 			double cosa = cos(ang), sina = sin(ang);
// 
// 			//先假定影像定位到原点，计算这种情况下的坐标换算参数
// 			m_imgPos.lfImgLayOX = 0;
// 			m_imgPos.lfImgLayOY = 0;
// 
// 			m_imgPos.lfImgMatrix[0] = cosa*scale;
// 			m_imgPos.lfImgMatrix[1] =-sina*scale;
// 			m_imgPos.lfImgMatrix[2] = sina*scale;
// 			m_imgPos.lfImgMatrix[3] = cosa*scale;
// 
// 			SetImagePosition();
// 
// 			//移动影像的定位点到第一个数据点 pt4ds[0]，并重新计算坐标换算参数
// 			PT_4D tpt4d;
// 			PT_3D tpt3d;			
// 			tpt4d = pt4ds[0];
// 			ImageToGround(&tpt4d, &tpt3d);
// 
// 			m_imgPos.lfImgLayOX = (pts[0].x-tpt3d.x);
// 			m_imgPos.lfImgLayOY = (pts[0].y-tpt3d.y);
// 
// 			SetImagePosition();

			//更新视图显示
//			RefreshImageOverlay();
		}
		break;
	}	
	CBaseView::OnUpdate( pSender,  lHint,  pHint);
}

BOOL CVectorView_GL::InitBmpLayer()
{	
	BOOL bWhiteBK = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_WHILEBK,FALSE);
	if( bWhiteBK )
	{
		m_pContext->SetBackColor(RGB(255,255,255));
		
		if( m_clrHilite==RGB(255,255,255) )
			m_clrHilite = 0;
	}
	else
	{
		m_pContext->SetBackColor(0);
	}

	if( !CBaseView::InitBmpLayer() )
		return FALSE;

	//好像没用	
	BOOL bSolidText = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_SOLIDTEXT,FALSE);
	
	m_vectLayR.Destroy();
	m_markLayR.Destroy();
	m_laymgrLeft.InsertBeforeDrawingLayer(&m_vectLayL,&m_constDragL);
	m_laymgrLeft.InsertBeforeDrawingLayer(&m_markLayL,&m_constDragL);
	
	m_snapLayL.m_nEraseType = CVariantDragLayer::eraseXOR;
	m_laymgrLeft.InsertBeforeDrawingLayer(&m_snapLayL,&m_cursorL);
	
	m_accurboxLayL.m_nEraseType = CVariantDragLayer::eraseXOR;
	m_laymgrLeft.InsertBeforeDrawingLayer(&m_accurboxLayL,&m_variantDragL);
	
	for( int i=0; i<m_laymgrLeft.GetDrawingLayerCount(); i++ )
	{
		if( m_laymgrLeft.GetDrawingLayer(i)==&m_mutiImgLay )
		{
			m_laymgrLeft.RemoveDrawingLayer(&m_mutiImgLay);
			m_laymgrLeft.InsertDrawingLayer(&m_mutiImgLay,0);
		}
	}

	m_vectLayL.EnableBGLoad(FALSE);
	m_markLayL.EnableBGLoad(FALSE);
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

BOOL CVectorView_GL::LoadDocData()
{
	if( !CBaseView::LoadDocData() )
		return FALSE;
	if( m_bViewBaseOnImage )
	{			
		Envelope dataBound = GetDocument()->GetDlgDataSource()->GetBound();	
		
		dataBound.Transform(m_lfImgRMatrix);
		dataBound.m_xl*=m_lfMapScaleX;
		dataBound.m_xh*=m_lfMapScaleX;
		dataBound.m_yl*=m_lfMapScaleY;
		dataBound.m_yh*=m_lfMapScaleY;
		m_vectLayL.SetDataBound(dataBound);
		m_vectLayR.SetDataBound(dataBound);
		m_markLayL.SetDataBound(dataBound);
		m_markLayR.SetDataBound(dataBound);
	}
	else
	{
		if (m_bOverlayImgLay)
		{
			m_vectLayL.SetDataBound(m_mutiImgLay.GetDataBound());
			m_vectLayR.SetDataBound(m_mutiImgLay.GetDataBound());
			m_markLayL.SetDataBound(m_mutiImgLay.GetDataBound());
			m_markLayR.SetDataBound(m_mutiImgLay.GetDataBound());
		}	
		else
		{
			Envelope dataBound = GetDocument()->GetDlgDataSource()->GetBound();
			dataBound.m_xl*=m_lfMapScaleX;
			dataBound.m_xh*=m_lfMapScaleX;
			dataBound.m_yl*=m_lfMapScaleY;
			dataBound.m_yh*=m_lfMapScaleY;
			m_vectLayL.SetDataBound(dataBound);
			m_vectLayR.SetDataBound(dataBound);
			m_markLayL.SetDataBound(dataBound);
			m_markLayR.SetDataBound(dataBound);
		}
	}
	UpdateGridDrawing();
	OnUpdate(this,hc_UpdateOverlayBound,NULL);
	return TRUE;
}

extern BOOL CheckStrZ(LPCTSTR strz, float& z);
void CVectorView_GL::UpdateGridDrawing()
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
	
	m_vectLayL.DelObj(HANDLE_GRID);
	m_vectLayL.DelObj(HANDLE_GRID);
	
	m_vectLayL.ClearAll();
	m_vectLayR.ClearAll();
	
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
			buf.BeginLineString(m_sGridParam.color,1);
			pt.x = m_sGridParam.ox+x; pt.y = m_sGridParam.oy;
			buf.MoveTo(&pt);
			
			pt.y = yr;
			buf.LineTo(&pt);
			buf.End();
		}
		
		for( y=0, i=0; i<=ny; i++, y+=m_sGridParam.dy )
		{
			pt.y = m_sGridParam.oy+y; pt.x = m_sGridParam.ox;
			buf.BeginLineString(m_sGridParam.color,1);
			buf.MoveTo(&pt);
			
			pt.x = xr;
			buf.LineTo(&pt);
			buf.End();
		}
		
		AddObjtoVectLay(HANDLE_GRID,&buf);
		m_vectLayL.ClearAll();
		m_vectLayR.ClearAll();
	}

}

void CVectorView_GL::DriveToXyz(PT_3D *pt, BOOL bMoveImg)
{
	PT_4D cltpt, new_gpt, new_cltpt;
	GroundToClient(pt,&cltpt);
	
	CRect rect;
	GetClientRect(&rect);
	int w = rect.Width(), h = rect.Height();
	
	if( bMoveImg || cltpt.x<w/10 || cltpt.x>w*9/10 || cltpt.y<h/10 || cltpt.y>h*9/10 )
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
}


void CVectorView_GL::ClientToImage(PT_4D *pt0, PT_4D *pt1)
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

void CVectorView_GL::GroundToImage(PT_3D *pt0, PT_4D *pt1)
{
	if( !m_bViewBaseOnImage )
	{
		(*pt1).x = (*pt0).x*m_lfMapScaleX;
		(*pt1).y = (*pt0).y*m_lfMapScaleY;
//		*pt1 = *pt0;
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

void CVectorView_GL::ImageToClient(PT_4D *pt0, PT_4D *pt1)
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

void CVectorView_GL::ImageToGround(PT_4D *pt0, PT_3D *pt1)
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

void CVectorView_GL::GroundToVectorLay(PT_3D *pt0, PT_4D *pt1)
{
	GroundToImage(pt0, pt1);
}

void CVectorView_GL::CalcImgSysParams(PT_3D *ptsBound)
{
}


CCoordWnd CVectorView_GL::GetCoordWnd()
{
	if( m_pRealCS==NULL )
		m_pRealCS = new CCoordSys();
	m_pRealCS->CopyFrom(m_pLeftCS);
	double m[16];
	Matrix44FromZoom(m_lfMapScaleX,m_lfMapScaleY,1.0,m);
	m_pRealCS->MultiplyRightMatrix(m);
	m_pRealCS->MultiplyRightMatrix(m_lfImgRMatrix);
	return CCoordWnd(m_pRealCS,m_pRealCS,m_gCurPoint,m_gCurPoint,FALSE);
}

void CVectorView_GL::AddConstDragLine(const GrBuffer *pBuf)
{
	CBaseView::AddConstDragLine(pBuf);
}


void CVectorView_GL::UpdateConstDragLine()
{
	CBaseView::UpdateConstDragLine();
}

void CVectorView_GL::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
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
	}
	
	CBaseView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

 
UINT ToolBtnIds20[]={
	IDC_BUTTON_VIEWOVERLAY,
		IDC_BUTTON_ROTATERASTER
};

UINT ToolBtnBmps20[]={
	IDB_BUTTON_VIEWOVERLAY,
		IDB_BUTTON_ROTATERASTER
};

UINT ToolButTips20[] = { 
	IDS_TIPS_VIEWOVERLAY,
		IDS_TIPS_ROTATERASTER
};


int CVectorView_GL::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	//m_pContext->CreateD3DContext(m_hWnd);
	//m_pContext->m_nTextMode = CStereoDrawingContext::textureModeD3D;
	m_pContext->m_nTextMode = CStereoDrawingContext::textureModeGL;

	m_pContext->m_bCreateStereo  = FALSE;
	m_pContext->m_bCreateOverlay = FALSE;
	m_pContext->SetDisplayMode(CStereoDrawingContext::modeSingle);

	AfxGetMainWnd()->SendMessage(FCCM_UPDATEVIEWLIST,0,(LPARAM)this);
	if (CBaseView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CBitmap bmp;
 	for( int i=0; i<sizeof(ToolBtnIds20)/sizeof(ToolBtnIds20[0]); i++)
 	{
 		bmp.LoadBitmap(ToolBtnBmps20[i]);
 		HBITMAP hBmp = (HBITMAP)bmp;
 		m_ctrlBarHorz.AddButtons(ToolBtnIds20+i,&hBmp,1);
 		bmp.DeleteObject();
 	}
 	
 	for( i=0; i<sizeof(ToolBtnIds20)/sizeof(ToolBtnIds20[0]); i++)
 	{
		CMFCButton *pBtn = (CMFCButton*)m_ctrlBarHorz.GetButton(ToolBtnBmps20[i]);
 		if( pBtn )
 		{
 			CString str;
 			str.LoadString(ToolButTips20[i]);
 			pBtn->SetTooltip(str);
 		}
 	}
	
	return 0;
}

void CVectorView_GL::OnDestroy()
{
	AfxGetMainWnd()->SendMessage(FCCM_UPDATEVIEWLIST,1,(LPARAM)this);
	AfxGetMainWnd()->SendMessage(FCCM_REFRESHVECFLAG,0,0);
	CBaseView::OnDestroy();
}

void CVectorView_GL::OnMouseLeave()
{	
	CBaseView::OnMouseLeave();
//	 m_cursorL.Erase();
//	 m_cursorR.Erase();
//	 EnableCrossVisible(FALSE);
//   ResetCursorDrawing();
}

float CVectorView_GL::ZoomSelect(float wantZoom)
{

	if( m_nInnerCmd==IDC_BUTTON_ZOOMIN || m_nInnerCmd==IDC_BUTTON_ZOOMOUT )
	{
		float zoom = GetComboNextZoom(m_nInnerCmd==IDC_BUTTON_ZOOMIN?1:0);
		if( zoom>0 )return zoom;
	}
	return CSceneView::ZoomSelect(wantZoom);
}


void CVectorView_GL::SetImagePosition()
{
	if( !m_bOverlayImgLay )
		return;

	if( !m_bViewBaseOnImage )
	{
		//计算主影像的四个角点坐标
		PT_3D pts[4];
		CSize szImg;
		
// 		pts[0].x = 0; pts[0].y = 0;
// 		pts[1].x = szImg.cx; pts[1].y = 0;
// 		pts[2].x = szImg.cx; pts[2].y = szImg.cy;
// 		pts[3].x = 0; pts[3].y = szImg.cy;
// 		
// 		for( int i=0; i<4; i++)
// 		{
// 			double x, y;
// 			x = m_imgPos.lfImgLayOX + pts[i].x*m_imgPos.lfImgMatrix[0] + pts[i].y*m_imgPos.lfImgMatrix[1];
// 			y = m_imgPos.lfImgLayOY + pts[i].x*m_imgPos.lfImgMatrix[2] + pts[i].y*m_imgPos.lfImgMatrix[3];
// 			
// 			pts[i].x = x; pts[i].y = y;
// 		}
// 		
// 		m_imgLay.SetCornerPts(TRUE,pts);
		//计算辅助影像的四个角点坐标
		int nSize = m_arrImgPos.GetSize();
		if (nSize>0)
		{
			for (int i=0;i<nSize;i++)
			{
				szImg = m_mutiImgLay.GetImageSize(m_arrImgPos[i].fileName);
				
				pts[0].x = 0; pts[0].y = 0;
				pts[1].x = szImg.cx; pts[1].y = 0;
				pts[2].x = szImg.cx; pts[2].y = szImg.cy;
				pts[3].x = 0; pts[3].y = szImg.cy;
				
				for( int j=0; j<4; j++)
				{
					double x, y;
					x = m_arrImgPos[i].pos.lfImgLayOX + pts[j].x*m_arrImgPos[i].pos.lfImgMatrix[0] + pts[j].y*m_arrImgPos[i].pos.lfImgMatrix[1];
					y = m_arrImgPos[i].pos.lfImgLayOY + pts[j].x*m_arrImgPos[i].pos.lfImgMatrix[2] + pts[j].y*m_arrImgPos[i].pos.lfImgMatrix[3];
					
					pts[j].x = x; pts[j].y = y;
				}
				
				m_mutiImgLay.SetCornerPts(m_arrImgPos[i].fileName,TRUE,pts);
			}
		
		}
	}
	else
 	{
// 		m_imgLay.SetCornerPts(m_nRasterAng,1.0,1.0);
// //		m_mutiImgLay.SetCornerPts(m_arrImgPos[0].fileName,m_nRasterAng,0,0);
// 		double m[16] = {
// 			m_imgPos.lfImgMatrix[0],m_imgPos.lfImgMatrix[1],0,m_imgPos.lfImgLayOX,
// 			m_imgPos.lfImgMatrix[2],m_imgPos.lfImgMatrix[3],0,m_imgPos.lfImgLayOY,
// 			0,0,1,0,
// 			0,0,0,1
// 		};
// // 		double m[16] = {
// // 			m_arrImgPos[0].pos.lfImgMatrix[0],m_arrImgPos[0].pos.lfImgMatrix[1],0,m_arrImgPos[0].pos.lfImgLayOX,
// // 				m_arrImgPos[0].pos.lfImgMatrix[2],m_arrImgPos[0].pos.lfImgMatrix[3],0,m_arrImgPos[0].pos.lfImgLayOY,
// // 				0,0,1,0,
// // 				0,0,0,1
// // 		};
// 		memcpy(m_lfImgMatrix,m,sizeof(m));
// 		matrix_reverse(m,4,m_lfImgRMatrix);
// 
// 		m_gsa.lfGKX = m_gsa.lfGKY = sqrt(fabs(m_lfImgRMatrix[0]*m_lfImgRMatrix[5] - m_lfImgRMatrix[1]*m_lfImgRMatrix[4]));
// 		//副影像
// 		CSize szImg;
// 		PT_3D pts[4];
// 		int nSize = m_arrImgPos.GetSize();
// 		if (nSize>0)
// 		{
// 			for (int i=0;i<nSize;i++)
// 			{
// 				szImg = m_mutiImgLay.GetImageSize(m_arrImgPos[i].fileName);
// 				
// 				pts[0].x = 0; pts[0].y = 0;
// 				pts[1].x = szImg.cx; pts[1].y = 0;
// 				pts[2].x = szImg.cx; pts[2].y = szImg.cy;
// 				pts[3].x = 0; pts[3].y = szImg.cy;
// 				
// 				for( int j=0; j<4; j++)
// 				{
// 					double x, y;
// 					x = m_arrImgPos[i].pos.lfImgLayOX + pts[j].x*m_arrImgPos[i].pos.lfImgMatrix[0] + pts[j].y*m_arrImgPos[i].pos.lfImgMatrix[1];
// 					y = m_arrImgPos[i].pos.lfImgLayOY + pts[j].x*m_arrImgPos[i].pos.lfImgMatrix[2] + pts[j].y*m_arrImgPos[i].pos.lfImgMatrix[3];
// 					
// 					pts[j].x = x; pts[j].y = y;
// 				}
// 				
// 				m_mutiImgLay.SetCornerPts(m_arrImgPos[i].fileName,TRUE,pts);
// 			}
// 			
// 		}
	}
}

void CVectorView_GL::OnOverlayRaster() 
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
	int nSize = m_arrImgPos.GetSize();
	dlg.m_arrImgPos.SetSize(nSize);
	for (int i=0;i<nSize;i++)
	{
		strcpy(dlg.m_arrImgPos[i].fileName,m_arrImgPos[i].fileName);
		dlg.m_arrImgPos[i].lfImgLayOX = m_arrImgPos[i].pos.lfImgLayOX;
		dlg.m_arrImgPos[i].lfImgLayOY = m_arrImgPos[i].pos.lfImgLayOY;
		dlg.m_arrImgPos[i].lfImgMatrix[0] = m_arrImgPos[i].pos.lfImgMatrix[0];
		dlg.m_arrImgPos[i].lfImgMatrix[1] = m_arrImgPos[i].pos.lfImgMatrix[1];
		dlg.m_arrImgPos[i].lfImgMatrix[2] = m_arrImgPos[i].pos.lfImgMatrix[2];
		dlg.m_arrImgPos[i].lfImgMatrix[3] = m_arrImgPos[i].pos.lfImgMatrix[3];
		dlg.m_arrImgPos[i].lfPixelSizeX = m_arrImgPos[i].pos.lfPixelSizeX;
		dlg.m_arrImgPos[i].lfPixelSizeY = m_arrImgPos[i].pos.lfPixelSizeY;
		dlg.m_arrImgPos[i].nPixelBase = m_arrImgPos[i].pos.nPixelBase;
		if( m_arrImgPos[i].pos.nPixelBase==0 )
		{
		}
		else
		{
			dlg.m_arrImgPos[i].lfImgLayOX += m_arrImgPos[i].pos.lfPixelSizeX*0.5;
			dlg.m_arrImgPos[i].lfImgLayOY += m_arrImgPos[i].pos.lfPixelSizeY*0.5;
		}
	}
	dlg.m_nMainImageIdx = m_nMainImageIdx;
	if( dlg.DoModal()!=IDOK )return;
	
//	m_laymgrLeft.RemoveDrawingLayer(&m_imgLay);
	m_laymgrLeft.RemoveDrawingLayer(&m_mutiImgLay);
	if( dlg.m_nMainImageIdx==-1 )
	{
		m_nMainImageIdx = -1;
// 		m_imgLay.DetachImage();
// 		m_imgLay.Destroy();
		m_mutiImgLay.RemoveAllImgs();
		m_mutiImgLay.Destroy();
		m_arrImgPos.RemoveAll();

		m_bOverlayImgLay = FALSE;	
		matrix_toIdentity(m_lfImgMatrix,4);
		matrix_toIdentity(m_lfImgRMatrix,4);

//		m_gsa.lfAngle = m_fOldDrawCellAngle;
		m_gsa.lfGKX = m_fOldDrawCellKX;
		m_gsa.lfGKY = m_fOldDrawCellKY;
		
		m_pContext->m_fDrawCellAngle = -m_gsa.lfAngle;
		m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
		m_pContext->m_fDrawCellKY = m_gsa.lfGKY;
		
		m_pContext->GetLeftContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
		m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
		m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;
		OnUpdate(NULL,hc_SetMapArea,(CObject*)m_ptBounds);

		return;
	}
	
	CString strMsg = _T("");
	for (i=0;i<dlg.m_arrImgPos.GetSize();i++)
	{
		CString strFileName = CString(dlg.m_arrImgPos[i].fileName) + _T(".ei");
		if( _access(strFileName,0)==-1 && _access(dlg.m_arrImgPos[i].fileName,0)==-1)
		{
			CString str;
			str.FormatMessage( IDS_FILE_OPEN_ERR,dlg.m_arrImgPos[i].fileName);
			str+=_T("\n");
			strMsg+=str;
		}
	}
	if (!strMsg.IsEmpty())
	{
		AfxMessageBox(strMsg);
	}
	//计算窗口中心坐标，将来仍然要定位到此位置
	CVectorViewCenterPoint1 ptSave(this);

	//主影像
	
	nSize = dlg.m_arrImgPos.GetSize();
	m_arrImgPos.RemoveAll();
	m_mutiImgLay.RemoveAllImgs();
	if (nSize>0)
	{	
		ImgPosItem item;
		m_mutiImgLay.SetContext(m_pContext->GetLeftContext());
		if ( m_mutiImgLay.InitCache(GetBestCacheTypeForLayer(&m_mutiImgLay),
					CSize(512,512),49))
		{
			for (int i=0;i<nSize;i++)
			{
				{
					if(!m_mutiImgLay.AddImage(dlg.m_arrImgPos[i].fileName))
					{					
						CString strMsg;
						strMsg.FormatMessage( IDS_FILE_OPEN_ERR,dlg.m_arrImgPos[i].fileName);
						AfxMessageBox(strMsg);
						continue;
					}
				}	
				item.fileName = dlg.m_arrImgPos[i].fileName;
				
				item.pos.lfImgLayOX = dlg.m_arrImgPos[i].lfImgLayOX; item.pos.lfImgLayOY = dlg.m_arrImgPos[i].lfImgLayOY;
				item.pos.lfImgMatrix[0] = dlg.m_arrImgPos[i].lfImgMatrix[0];
				item.pos.lfImgMatrix[1] = dlg.m_arrImgPos[i].lfImgMatrix[1];
				item.pos.lfImgMatrix[2] = dlg.m_arrImgPos[i].lfImgMatrix[2];
				item.pos.lfImgMatrix[3] = dlg.m_arrImgPos[i].lfImgMatrix[3];
				item.pos.lfPixelSizeX = dlg.m_arrImgPos[i].lfPixelSizeX;
				item.pos.lfPixelSizeY = dlg.m_arrImgPos[i].lfPixelSizeY;
				item.pos.nPixelBase = dlg.m_arrImgPos[i].nPixelBase;
				if( item.pos.nPixelBase==0 )
				{
				}
				else
				{
					item.pos.lfImgLayOX -= item.pos.lfPixelSizeX*0.5;
					item.pos.lfImgLayOY -= item.pos.lfPixelSizeY*0.5;
				}
				m_arrImgPos.Add(item);
			}
		}
	    m_nMainImageIdx = dlg.m_nMainImageIdx;
	}
	if( m_bOverlayImgLay )
	{
	/*	m_laymgrLeft.InsertDrawingLayer(&m_imgLay,0);*/
		//保存旧值
	
		//计算影像的四个角点坐标
		SetImagePosition();	
		m_lfMapScaleX = 1.0;
		m_lfMapScaleY = 1.0;
	}
	if (m_arrImgPos.GetSize()>0)
	{
		m_mutiImgLay.InitParams();
	}
	RefreshImageOverlay();
	
	m_pContext->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->m_fDrawCellKY = m_gsa.lfGKY;
	
	m_pContext->GetLeftContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;
	
}


void CVectorView_GL::RefreshImageOverlay()
{
	if( m_bOverlayImgLay )
	{
	
		if (m_arrImgPos.GetSize()>0)
		{
//			m_laymgrLeft.RemoveAll();
			m_mutiImgLay.OnChangeCoordSys(FALSE);
			m_laymgrLeft.InsertDrawingLayer(&m_mutiImgLay,0);
		}

		if( m_bViewBaseOnImage )
		{			
			
			m_vectLayL.SetDataBound(m_mutiImgLay.GetDataBound()/*Envelope(0,szImg.cx,0,szImg.cy)*/);
			m_markLayL.SetDataBound(m_mutiImgLay.GetDataBound()/*Envelope(0,szImg.cx,0,szImg.cy)*/);

			OnUpdate(this,hc_UpdateAllObjects,NULL);
		}
		else
		{
			OnUpdate(this,hc_UpdateAllObjects,NULL);
			RecalcScrollBar(FALSE);
		}
		
		Invalidate(FALSE);
	}
	else
	{
		Invalidate(FALSE);
	}
}

void CVectorView_GL::OnRefdem() 
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

void CVectorView_GL::OnUnrefdem() 
{
	//clear old info
	m_dem.Close();
	
	m_strDemPath.Empty();
}

BOOL CVectorView_GL::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
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

void CVectorView_GL::OnMoveCursorToCenter() 
{
	OnMoveCursorToCenter2(0,0);	
}

void CVectorView_GL::OnViewRotateAsStereo() 
{
	BOOL bRotate = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,"RotateAsStereo",0);
	bRotate = !bRotate;

	if( bRotate )
		ViewRotateAsStereo();
	else
		OnViewReset();

	AfxGetApp()->WriteProfileInt(REGPATH_VIEW_VECT,"RotateAsStereo",bRotate);
}

void CVectorView_GL::OnUpdateViewRotateAsStereo(CCmdUI* pCmdUI)
{
	BOOL bRotate = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,"RotateAsStereo",0);
	pCmdUI->SetCheck(bRotate?1:0);
}

extern CStereoView *GetStereoView(int flag);

void CVectorView_GL::ViewRotateAsStereo() 
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
//	double cosa = cos(m_lfAngle), sina = sin(m_lfAngle);
//	double m1[]={1.0,0.0,0.0,0.0,  0.0,cosa,sina,0.0,  0.0,-sina,cosa,0.0,  0.0,0.0,0.0,1.0};//{cosa,sina,0,-sina,cosa,0.0,0.0,0.0,1.0};
//	m_pContext->GetLeftContext()->SetTextAngle(m_lfAngle);

	double m1[16];
	Matrix44FromRotate(&fgpt,&PT_3D(0,0,1),/*m_lfAngle*/ang0,m1);

// 	memcpy(m_matrix,m1,sizeof(m1));
// 	matrix_reverse(m_matrix,4,m_rmatrix);
	
	m_pLeftCS->MultiplyMatrix(m1);
//	CalcImgSysParams(m_ptBounds);
	
	OnPostInnerCmd();
//	m_nInnerCmd = save;
	
	InitBmpLayer();
	LoadDocData();
	RecalcScrollBar(FALSE);
	
	//移动到焦点位置
	DriveToXyz(&fgpt,TRUE);
	
	Invalidate(FALSE);
	
}

void CVectorView_GL::OnViewReset() 
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
	
	//恢复坐标系
//	double cosa = cos(-m_lfAngle), sina = sin(-m_lfAngle);
//	double m1[]={1.0,0.0,0.0,0.0,  0.0,cosa,sina,0.0,  0.0,-sina,cosa,0.0,  0.0,0.0,0.0,1.0};//{cosa,sina,0,-sina,cosa,0.0,0.0,0.0,1.0};
	
	double m1[16];
	Matrix44FromRotate(&fgpt,&PT_3D(0,0,1),/*-m_lfAngle*/-m_gsa.lfAngle,m1);
	m_gsa.lfAngle = 0.0;
//	m_pContext->GetLeftContext()->SetTextAngle(0.0);
// 	matrix_multiply(m1,m_matrix,4,m_rmatrix); //恢复为原来的角度
// 	memcpy(m_matrix,m_rmatrix,sizeof(m_rmatrix));
// 	matrix_reverse(m_matrix,3,m_rmatrix);

	m_pLeftCS->MultiplyMatrix(m1);

	Envelope envp;
	envp.CreateFromPts(m_ptBounds,4);	
//	m_lfAngle = 0;

	OnPostInnerCmd();
//	m_nInnerCmd = save;

	InitBmpLayer();
	LoadDocData();
	RecalcScrollBar(FALSE);

	//移动到焦点位置
	DriveToXyz(&fgpt,TRUE);
	
	Invalidate(FALSE);
	
}

void CVectorView_GL::OnManulLoadVect() 
{
//	m_nInnerCmd = ID_TOOL_MANUALLOADVECT;
	m_bStartLoadVect = FALSE;
	
	OnPreInnerCmd(ID_TOOL_MANUALLOADVECT);
}

void CVectorView_GL::OnUpdateManulLoadVect(CCmdUI* pCmdUI) 
{
	BOOL bManualLoad = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MANUALLOADVECT,FALSE);
	pCmdUI->Enable( bManualLoad );
}



void CVectorView_GL::OnMeasureImagePosition()
{
	OnCommandType(ID_RASTER_POSITION);
}

void CVectorView_GL::OnDisplayByheight() 
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

						m_vectLayL.SetObjColor((LONG_PTR)pFtr,colorMaps[idx]);
						m_vectLayR.SetObjColor((LONG_PTR)pFtr, colorMaps[idx]);
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
					m_vectLayL.SetObjColor((LONG_PTR)pFtr, color);
					m_vectLayR.SetObjColor((LONG_PTR)pFtr, color);
				}				
			}
		}
	}
	
	m_vectLayL.ClearAll();
	m_vectLayR.ClearAll();
	
	Invalidate(FALSE);
	
	
}

void CVectorView_GL::OnUpdateDisplayByheight(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bDisplayByHeight);
}

void CVectorView_GL::OnDisplayContoursSpecially() 
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
				
					m_vectLayL.SetObjColor((LONG_PTR)pFtr, clr);
					m_vectLayR.SetObjColor((LONG_PTR)pFtr, clr);
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
					m_vectLayL.SetObjColor((LONG_PTR)pFtr, color);
					m_vectLayR.SetObjColor((LONG_PTR)pFtr, color);
				}				
			}
		}
	}
	
	m_vectLayL.ClearAll();
	m_vectLayR.ClearAll();
	
	Invalidate(FALSE);

	m_bDisplayByHeight = FALSE;
}

void CVectorView_GL::OnUpdateDisplayContoursSpecially(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bDisplayContoursSpecially);	
}



//角度 ang 为顺时针角度
static void MatrixRotateImage(CSize szImg, float ang, double *m)
{
	matrix_toIdentity(m,3);
	if( ang==90.0 )
	{
		m[0] = 0; m[1] = -1; m[2] = szImg.cx; 
		m[3] = 1; m[4] = 0; m[5] = 0; 
	}
	else if( ang==180.0 )
	{
		m[0] = -1; m[1] = 0; m[2] = szImg.cx; 
		m[3] = 0; m[4] = -1; m[5] = szImg.cy; 
	}
	else if( ang==270.0 )
	{
		m[0] = 0; m[1] = 1; m[2] = 0; 
		m[3] = -1; m[4] = 0; m[5] = szImg.cy; 
	}
	
	return;
}

void CVectorView_GL::RotateRaster(int ang)
{
// 	if( m_bOverlayImgLay && m_imgLay.IsValid() );
// 	else return;
// 	
// 	if( ang!=0 && ang!=90 && ang!=180 && ang!=270 )
// 		return;
// 
// 	if( ang==m_nRasterAng )
// 		return;
// 
// 	//计算窗口中心坐标，将来仍然要定位到此位置
// 	CVectorViewCenterPoint1 ptSave(this);
// 	
// 	//客户中心作为焦点，位置不变，这里先记录此位置
// 	CRect rcClient;
// 	GetClientRect(&rcClient);
// 	CPoint cpt = rcClient.CenterPoint();
// 	
// 	PT_4D fcpt;
// 	PT_3D fgpt; 
// 	fcpt.x = cpt.x;  fcpt.y = cpt.y; fcpt.z = 0;
// 	ClientToGround(&fcpt,&fgpt);
// 	
// 	float zoom = GetZoomRate();
// 	
// 	//旋转
// 	{
// 		CSize szImg = m_imgLay.GetImageSize();
// 		double m0[9], m1[9];
// 
// 		MatrixRotateImage(szImg,m_nRasterAng,m0);
// 		matrix_reverse(m0,3,m1);
// 		memcpy(m0,m1,sizeof(m1));
// 		MatrixRotateImage(szImg,ang,m1);
// 
// 		double m2[9] = {
// 			m_imgPos.lfImgMatrix[0],m_imgPos.lfImgMatrix[1],m_imgPos.lfImgLayOX,
// 			m_imgPos.lfImgMatrix[2],m_imgPos.lfImgMatrix[3],m_imgPos.lfImgLayOY,
// 			0,0,1
// 		};
// 
// 		double m3[9];
// 		matrix_multiply(m2,m0,3,m3);
// 		matrix_multiply(m3,m1,3,m2);
// 		memcpy(m3,m2,sizeof(m2));
// 
// 		m_imgPos.lfImgMatrix[0] = m3[0];
// 		m_imgPos.lfImgMatrix[1] = m3[1];
// 		m_imgPos.lfImgMatrix[2] = m3[3];
// 		m_imgPos.lfImgMatrix[3] = m3[4];
// 		m_imgPos.lfImgLayOX = m3[2];
// 		m_imgPos.lfImgLayOY = m3[5];
// 
// 	}
// 	
// 	CString strImg = m_imgLay.GetImgFileName();
// 	
// 	m_imgLay.DetachImage();
// 	m_imgLay.Destroy();
// 
// 	m_imgLay.SetContext(m_pContext->GetLeftContext());
// 	
// 	if( m_imgLay.InitCache(GetBestCacheTypeForLayer(&m_imgLay),
// 		CSize(512,512),49)==FALSE ||
// 		!m_imgLay.AttachImage(strImg))
// 	{
// 		return;
// 	}
// 
// 	m_nRasterAng = ang;
// 
// 	if( m_bOverlayImgLay )
// 	{
// 		//计算影像的四个角点坐标
// 		SetImagePosition();
// 	}
// 	
// 	RefreshImageOverlay();
}



void CVectorView_GL::OnRotateRaster()
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


void CVectorView_GL::OnUpdateRotateRaster(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_bOverlayImgLay && m_mutiImgLay.IsValid() );
	pCmdUI->SetCheck(0);
}

void CVectorView_GL::OnWsAsstereo() 
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

void CVectorView_GL::ScrollForDrawBitmap(double dx,double dy,double dz )
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
}

void CVectorView_GL::OnExportRaster()
{
// 	CDlgExportRaster dlg;
// 	if( dlg.DoModal()!=IDOK )return;
// 	if( dlg.m_strFilePath.GetLength()<=0 )
// 		return;
// 	CDlgDoc *pDoc = GetDocument();
// 	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_PREPARE) );
// 	TIFF *tif = TIFFOpen(dlg.m_strFilePath,"w");
// 	if( !tif )
// 		return;
// 	PT_3D oldgpt;
// 	PT_4D pt0 = PT_4D(0,0,0,0);
// 	ClientToGround(&pt0,&oldgpt);
// 	float oldzoom = GetZoomRate();
// 	m_vectLayL.SetObjVisible(HANDLE_BOUND,FALSE);
// 	m_vectLayL.SetObjVisible(HANDLE_GRAPH,FALSE);
// 	m_vectLayL.ClearAll();
// 	PT_4D pt1;
// 	CSize size;
// 	if( !dlg.m_bWorkspaceBound && m_imgLay.IsValidImage() )
// 	{
// 		ZoomChange(CPoint(0,0),1.0f/oldzoom,FALSE);
// 		size = m_imgLay.GetImageSize();
// 		pt0 = PT_4D(0,size.cy,0,size.cy);
// 		ImageToClient(&pt0,&pt1);
// 	}
// 	else
// 	{
// 		PT_3D bounds[4];
// 		PT_4D bounds4d[4];
// 		int i;
// 		pDoc->GetDlgDataSource()->GetBound(bounds,NULL,NULL);
// 		for( i=0; i<4; i++)GroundToClient(bounds+i,bounds4d+i);
// 		Envelope evlp;
// 		evlp.CreateFromPts(bounds4d,4,sizeof(PT_4D));
// 		pt1.x = floor(evlp.m_xl);	pt1.y = ceil(evlp.m_yh);
// 		size.cx = ceil(evlp.m_xh)-pt1.x;
// 		size.cy = pt1.y-floor(evlp.m_yl);
// 	}
// 	int nx = 0, ny = 0, stepx = size.cx, stepy = 1;
// 	DWORD nbytes = 0;
// 	if( stepx*stepy*3>0x300000 )
// 	{
// 	}
// 	else
// 	{
// 		stepy = 0x300000/(stepx*3);
// 	}
//     TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, size.cx);
//     TIFFSetField(tif, TIFFTAG_IMAGELENGTH, size.cy);
//     TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
//     TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
//     TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
//     TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
//     TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, stepy);
//     TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
//     TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_NONE);
//     TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
// 	long lSum = ceil(size.cy*1.0/stepy);
// 	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_PROCESS) );
// 	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, lSum);
// 	int w = size.cx, h = stepy;
// 	BITMAPINFO bitmapInfo;
// 	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
// 	bitmapInfo.bmiHeader.biWidth = w;
// 	bitmapInfo.bmiHeader.biHeight = h;
// 	bitmapInfo.bmiHeader.biSizeImage = ((w*3+3)&(~3))*h;
// 	bitmapInfo.bmiHeader.biPlanes = 1;
// 	bitmapInfo.bmiHeader.biBitCount = 24;
// 	bitmapInfo.bmiHeader.biCompression = BI_RGB;
// 	bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
// 	bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
// 	bitmapInfo.bmiHeader.biClrUsed = 0;
// 	bitmapInfo.bmiHeader.biClrImportant = 0;
// 	BYTE *pBits=NULL;
// 	HBITMAP hBmp = CreateDIBSection( NULL,&bitmapInfo,DIB_RGB_COLORS,
// 			(void**)&pBits,0,0L );
// 	CPtrArray pLayerListL,pLayerListR;
// 	if( m_imgLay.IsValidImage() )
// 		pLayerListL.Add(&m_imgLay);
// 	pLayerListL.Add(&m_vectLayL);	
// 	CRect rcClient;
// 	GetClientRect(&rcClient);
// 	MoveWindow(0,0,stepx,stepy);
// 	ScrollForDrawBitmap(pt1.x,pt1.y,0);
// 	CRect wndRc; 
// 	BYTE *pBuf = new BYTE[w*h*3];
// 	if( hBmp && pBits && pBuf )
// 	{
// 		for( ny=0; ny<size.cy; ny+=stepy)
// 		{
// 			wndRc = CRect(0,0,stepx,stepy);
// 			if(ny+stepy>size.cy)
// 			{
// 				wndRc = CRect(0,0,stepx,size.cy-ny);
// 			}
// 			if(ny!=0)
// 			{				
// 				ScrollForDrawBitmap(0,stepy,0);
// 			}
// 			AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, lSum);
// 			memset(pBits, 0, bitmapInfo.bmiHeader.biSizeImage);
// 			DrawRectBitmap(hBmp,CRect(0,0,wndRc.Width(),wndRc.Height()),&wndRc,
// 				pLayerListL,pLayerListR,CStereoDrawingContext::drawLeft);	
// 			BITMAP bmp;
// 			::GetObject(hBmp,sizeof(BITMAP),&bmp);
// 			BYTE *pSrc = (BYTE*)bmp.bmBits;
// 			BYTE *p1, *p2;
// 			int bmphei = wndRc.Height();
// 			for( int j=0; j<bmphei; j++)
// 			{
// 				p1 = pSrc + (DWORD)(bmp.bmHeight-1-j)*(DWORD)((bmp.bmWidth*3+3)&(~3));
// 				p2 = pBuf + j*bmp.bmWidth*3;
// 				for( int i=0; i<bmp.bmWidth; i++)
// 				{
// 					p2[0] = p1[2];
// 					p2[1] = p1[1];
// 					p2[2] = p1[0];
// 					p1 += 3;
// 					p2 += 3;
// 				}
// 			}
// 			for( j=0; j<bmphei; j++)
// 			{
// 				TIFFWriteScanline(tif, pBuf+j*bmp.bmWidth*3, ny+j, 0);
// 			}
// 		}
// 		delete[] pBuf;
// 		::DeleteObject(hBmp);
// 	}
// 	TIFFClose(tif);
// 	if( !dlg.m_bWorkspaceBound && m_imgLay.IsValidImage() )
// 	{
// 		CString name = dlg.m_strFilePath;
// 		if( name.Right(3).CompareNoCase("tif")==0 )
// 		{
// 			name = name.Left(name.GetLength()-3) + "tfw";
// 		}
// 		else
// 			name += ".tfw";
// 		FILE *fp = fopen(name,"wt");
// 		if( fp!=NULL )
// 		{
// 			double v[6];
// 			v[0] = m_imgPos.lfImgMatrix[0];
// 			v[1] = m_imgPos.lfImgMatrix[1];
// 			v[2] =-m_imgPos.lfImgMatrix[2];
// 			v[3] =-m_imgPos.lfImgMatrix[3];
// 			v[4] = m_imgPos.lfImgLayOX + m_imgPos.lfImgMatrix[1]*size.cy;
// 			v[5] = m_imgPos.lfImgLayOY + m_imgPos.lfImgMatrix[3]*size.cy;
// 			fprintf(fp,"%.6f\n%.6f\n%.6f\n%.6f\n%.6f\n%.6f\n",
// 				v[0],v[1],v[2],v[3],v[4],v[5]);
// 			fclose(fp);
// 		}
// 	}
// 	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);
// 	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_END) );
// 	MoveWindow(0,0,rcClient.Width(),rcClient.Height());
// 	m_vectLayL.ClearAll();
// 	m_vectLayL.SetObjVisible(HANDLE_BOUND,TRUE);
// 	m_vectLayL.SetObjVisible(HANDLE_GRAPH,TRUE);
// 	GroundToClient(&oldgpt,&pt1);
// 	ScrollForDrawBitmap(pt1.x,pt1.y,0);
// 	ZoomChange(CPoint(0,0),oldzoom/GetZoomRate(),FALSE);
// 	return;
}
void CVectorView_GL::OnViewOverlay() 
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_VIEWOVERLAY;
	OnPreInnerCmd(IDC_BUTTON_VIEWOVERLAY);
	m_bViewVector = !m_bViewVector;
	if( m_bViewVector )
	{
		m_laymgrLeft.InsertDrawingLayer(&m_vectLayL,1);
		m_laymgrRight.InsertDrawingLayer(&m_vectLayR,1);
		CButton *pBtn = m_ctrlBarHorz.GetButton(IDC_BUTTON_VIEWOVERLAY);
		if( pBtn )pBtn->SetCheck(0);
	}
	else
	{
		m_laymgrLeft.RemoveDrawingLayer(&m_vectLayL);
		m_laymgrRight.RemoveDrawingLayer(&m_vectLayR);
		CButton *pBtn = m_ctrlBarHorz.GetButton(IDC_BUTTON_VIEWOVERLAY);
		if( pBtn )pBtn->SetCheck(1);
	}
	Invalidate(FALSE);
	OnPostInnerCmd();
//	m_nInnerCmd = save;
}

#include "CommonCallStation.h"
void CVectorView_GL::OnRefresh()
{
	AfxCallMessage(WM_COMMAND,ID_STEREO_REFRESH,0);
}

void CVectorView_GL::OnZoomIn()
{
	AfxCallMessage(WM_COMMAND,ID_STEREO_ZOOMIN,0);
}

void CVectorView_GL::OnZoomOut()
{
	AfxCallMessage(WM_COMMAND,ID_STEREO_ZOOMOUT,0);
}


void CVectorView_GL::PushViewPos()
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


void CVectorView_GL::ActiveViewPos(ViewPos* pos)
{
	CBaseView::ActiveViewPos(pos);
	
	DriveToXyz(&pos->pt,TRUE);
}


BOOL CVectorView_GL::PrePrint(CDC* pDC, CPrintInfo* pInfo)
{
	if (m_nPrintScope==0&&m_nPrintScale==0)//边界有像素偏差
	{
		Envelope e = GetDocument()->GetDlgDataSource()->GetBound();
		
		e.TransformGrdToClt(GetCoordWnd().m_pViewCS,1);
		CRect DrawRect = pInfo->m_rectDraw;
		CSize size;
		size.cx = DrawRect.Width();
		size.cy = DrawRect.Height();
		
		double m[9];
		memset(m,0,sizeof(double)*9);
		double xScale = ((double)(size.cx-PRINTINDENT*2))/e.Width();
		double yScale = ((double)(size.cy-PRINTINDENT*2))/e.Height();
		
		
		m[0] = xScale;
		m[2] = -xScale*(e.m_xl)+PRINTINDENT;
		m[4] = yScale;
		m[5] = -yScale*(e.m_yl)+PRINTINDENT;
		m[8] = 1.0;
		
		m_pTempCoordSys->Create33Matrix(m);
		CArray<CCoordSys*,CCoordSys*> arr;
		arr.Add(m_vectLayL.GetContext()->GetCoordSys());
		arr.Add(m_pTempCoordSys);
		m_pComCoordSys->CreateCompound(arr);
		
		CRect rect(0,0,size.cx,size.cy);
		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
			CRect rectClip(rect);
			CPreviewDC *pPrevDC = static_cast<CPreviewDC*>(pDC);
			
			pPrevDC->PrinterDPtoScreenDP(&rectClip.TopLeft());
			pPrevDC->PrinterDPtoScreenDP(&rectClip.BottomRight());
			// Now offset the result by the viewport origin of
			
			// the print preview window...
			
			
			CPoint ptOrg;
			::GetViewportOrgEx(pDC->m_hDC,&ptOrg);
			rectClip += ptOrg;
			VERIFY(rgn.CreateRectRgnIndirect(rectClip));
		}
		else
		{
			// just use the regular clip area as we are not in preview mode
			
			VERIFY(rgn.CreateRectRgnIndirect(rect));
		}
		pDC->SelectClipRgn(&rgn);
		VERIFY(rgn.DeleteObject());
// 		// 		pDC->MoveTo(-1,-1);
// 		// 		pDC->LineTo(size.cx-1,-1);
// 		// 		pDC->LineTo(size.cx-1,size.cy-1);
// 		// 		pDC->LineTo(-1,size.cy-1);
// 		// 			pDC->LineTo(-1,-1);
// 		// 			pDC->LineTo(size.cx-1,(size.cy-1)/2);
// 		
// 		
// 		// 		pDC->MoveTo(5,5);
// 		// 		pDC->LineTo(size.cx-5,5);
// 		// 		pDC->LineTo(size.cx-5,size.cy-5);
// 		// 		pDC->LineTo(0,size.cy-5);
// 		// 		pDC->LineTo(5,5);
// 		pDC->LineTo(size.cx-1,(size.cy-1)/2);
		m_pPrintContext->CreateContext(/*m_hTempMemDC*/pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pComCoordSys);
		m_pPrintContext->SetDCSize(size);
		m_pPrintContext->SetViewRect(CRect(0,0,size.cx,size.cy));
		m_pPrintContext->SetBackColor(RGB(255,255,255));
	}
	else if (m_nPrintScope==0&&m_nPrintScale==1)
	{
		DWORD dwScale = GetDocument()->GetDlgDataSource()->GetScale();
		Envelope e0 = GetDocument()->GetDlgDataSource()->GetBound();
		Envelope e = m_vectLayL.GetDataBound();
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
		m[2] = -xScale*e0.m_xl+PRINTINDENT;
		m[5] = yScale*e0.m_yl+e0.Height()*yScale+PRINTINDENT;
		m[8] = 1.0;
		if(m_bOverlayImgLay&&m_bViewBaseOnImage)
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
		CRect ret(0,0,size.cx,size.cy);
		rect&=ret;
		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
			CRect rectClip(rect);
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
			VERIFY(rgn.CreateRectRgnIndirect(rect));
		}
		pDC->SelectClipRgn(&rgn,RGN_AND);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pTempCoordSys);
		m_pPrintContext->SetDCSize(CSize(rect.Width(),rect.Height()));
		m_pPrintContext->SetViewRect(rect);
		m_pPrintContext->SetBackColor(RGB(255,255,255));
	}
	else if (m_nPrintScope==1&&m_nPrintScale==0)
	{
		if (!m_bOverlayImgLay)
		{
			return FALSE;
		}
		Envelope e = m_mutiImgLay.GetDataBound();		
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
		double xScale = ((double)(size.cx-PRINTINDENT*2))/e.Width();
		double yScale = ((double)(size.cy-PRINTINDENT*2))/e.Height();
		m[0] = xScale;
		m[2] = -xScale*(e.m_xl)+PRINTINDENT;
		m[4] = yScale;
		m[5] = -yScale*(e.m_yl)+PRINTINDENT;
		m[8] = 1.0;
		m_pTempCoordSys->Create33Matrix(m);		
		CArray<CCoordSys*,CCoordSys*> arr;
		arr.Add(m_vectLayL.GetContext()->GetCoordSys());
		arr.Add(m_pTempCoordSys);
		m_pComCoordSys->CreateCompound(arr);
		CRect rect(0,0,size.cx,size.cy);
		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
			CRect rectClip(rect);
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
			VERIFY(rgn.CreateRectRgnIndirect(rect));
		}
		pDC->SelectClipRgn(&rgn);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pComCoordSys);
		m_pPrintContext->SetDCSize(size);
		m_pPrintContext->SetViewRect(CRect(0,0,size.cx,size.cy));
		m_pPrintContext->SetBackColor(RGB(255,255,255));
	}
	else if (m_nPrintScope==1&&m_nPrintScale==1)
	{
		if (!m_bOverlayImgLay)
		{
			return FALSE;
		}
		DWORD dwScale = GetDocument()->GetDlgDataSource()->GetScale();
		Envelope e = m_mutiImgLay.GetDataBound();
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
		m[2] = -xScale*e0.m_xl+PRINTINDENT;
		m[5] = yScale*e0.m_yl+e0.Height()*yScale+PRINTINDENT;
		m[8] = 1.0;
		if(m_bOverlayImgLay&&m_bViewBaseOnImage)
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
		CRect ret(0,0,size.cx,size.cy);
		rect&=ret;
		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
			CRect rectClip(rect);
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
			VERIFY(rgn.CreateRectRgnIndirect(rect));
		}
		pDC->SelectClipRgn(&rgn,RGN_AND);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pTempCoordSys);
		m_pPrintContext->SetDCSize(CSize(rect.Width(),rect.Height()));
		m_pPrintContext->SetViewRect(rect);
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
		double xScale = ((double)(size.cx-PRINTINDENT*2))/m_rcTemp.Width();
		double yScale = ((double)(size.cy-PRINTINDENT*2))/m_rcTemp.Height();
		if (xScale>yScale)
		{
			xScale = yScale;
			size.cx =  xScale*m_rcTemp.Width();
		}
		else
		{
			yScale = xScale;
			size.cy =  yScale*m_rcTemp.Height();
		}
		m[0] = xScale;
		m[2] = PRINTINDENT;
		m[4] = yScale;
		m[5] = PRINTINDENT;
		m[8] = 1.0;
		m_pTempCoordSys->Create33Matrix(m);		
		CArray<CCoordSys*,CCoordSys*> arr;
		arr.Add(m_vectLayL.GetContext()->GetCoordSys());
		arr.Add(m_pTempCoordSys);
		m_pComCoordSys->CreateCompound(arr);
		Envelope e(m_rcTemp.left,m_rcTemp.right,m_rcTemp.top,m_rcTemp.bottom);
		e.TransformGrdToClt(m_pTempCoordSys,1);
		CRect rect(e.m_xl,e.m_yl,e.m_xh,e.m_yh);
		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
			CRect rectClip(rect);
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
			VERIFY(rgn.CreateRectRgnIndirect(rect));
		}
		pDC->SelectClipRgn(&rgn);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pComCoordSys);
		m_pPrintContext->SetDCSize(size);
		m_pPrintContext->SetViewRect(CRect(0,0,size.cx,size.cy));
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
		m[2] = -xScale*e0.m_xl+PRINTINDENT;
		m[5] = yScale*e0.m_yl+e0.Height()*yScale+PRINTINDENT;
		m[8] = 1.0;
		if(m_bOverlayImgLay&&m_bViewBaseOnImage)
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
		CRect ret(0,0,size.cx,size.cy);
		rect&=ret;
		CRgn rgn;
		if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
		{
			CRect rectClip(rect);
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
			VERIFY(rgn.CreateRectRgnIndirect(rect));
		}
		pDC->SelectClipRgn(&rgn,RGN_AND);
		VERIFY(rgn.DeleteObject());
		m_pPrintContext->CreateContext(pDC->GetSafeHdc());
		m_pPrintContext->SetCoordSys(m_pTempCoordSys);		
		m_pPrintContext->SetDCSize(CSize(rect.Width(),rect.Height()));
		m_pPrintContext->SetViewRect(rect);
		m_pPrintContext->SetBackColor(RGB(255,255,255));	
	}
	return TRUE;
}

void CVectorView_GL::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
// 	if(!PrePrint(pDC,pInfo))
// 		return;
// 	HDC hdcPrinter = pDC->GetSafeHdc();
// 	::SetStretchBltMode(hdcPrinter,STRETCH_DELETESCANS);
// 	if (hdcPrinter == NULL)
// 	{
// 		MessageBox(_T("Buy a printer!"));
// 		return;
// 	}
// 	if(0)
// 	{	
// 		Envelope e = m_vectLayL.GetDataBound();
//  		GrElementList *pElementList = m_vectLayL.GetGrElementList();		
// 		C2DGDIDrawingContext dc;
// 		dc.CreateContext(hdcPrinter);
// 		CRect DrawRect = pInfo->m_rectDraw;
// //		DrawRect.DeflateRect(1,1);
// 		CSize size;
// 		size.cx = DrawRect.Width();/*::GetDeviceCaps(hdcPrinter,HORZRES)*/;
// 		size.cy = DrawRect.Height();/*::GetDeviceCaps(hdcPrinter,VERTRES)*/;
// 		
// 		dc.SetViewRect(DrawRect);		
// 		
// 		dc.BeginDrawing();
// 		dc.DrawGrBuffer2d(pElementList,CDrawingContext::modeNormal);
// 		dc.EndDrawing();
// 	}
// 	else
// 	{
// 		CClientDC dc(this);
// 		int ScreenDx = dc.GetDeviceCaps(LOGPIXELSX);
// 		int ScreenDy = dc.GetDeviceCaps(LOGPIXELSY);
// 		int PrintDx = pDC->GetDeviceCaps(LOGPIXELSX);
// 		int PrintDy = pDC->GetDeviceCaps(LOGPIXELSY);	
// 
// 		m_imgLay.Destroy();
// //		m_pPrintContext->SetBackColor(RGB(255,255,255));
// 		if(m_bOverlayImgLay)
// 		{		
// 			CSize oldSize = m_pPrintContext->GetDCSize();
// 			CRect oldRect; 
// 			m_pPrintContext->GetViewRect(oldRect);
// 			
// 			CDrawingContext *pOldContext = m_imgLay.GetContext();
// 
// 			CCoordSys *pSaveCS = m_pPrintContext->GetCoordSys();
// 			CCoordSys cs,cs0;
// 			m_imgLay.ClearAll();			
// 		
// 			int col = 0, row = 0,stepx = 1024,stepy = 768;
// 			int colNum = oldSize.cx/stepx;
// 			int rowNum = oldSize.cy/stepy;
// 			m_pPrintContext->SetDCSize(CSize(stepx,stepy));
// 			m_pPrintContext->SetViewRect(CRect(0,0,stepx,stepy));
// 			m_pPrintContext->BeginDrawImg();
// 			m_imgLay.SetContext(m_pPrintContext->GetCurContext());
// 			
// 			m_imgLay.InitCache(GetBestCacheTypeForLayer(&m_imgLay),
// 			CSize(512,512),49);	
// 			
// 			int right,bottom;
// 			for (;col<=colNum;col++ )
// 			{				 
// 				for (row=0;row<=rowNum;row++)
// 				{
// 					if (col==colNum)
// 					{
// 						right = oldRect.right;	
// 					}
// 					else
// 						right = (col+1)*stepx;
// 					if (row==rowNum)
// 					{
// 						bottom = oldRect.bottom;
// 					}
// 					else
// 						bottom = (row+1)*stepy;
// 					
// 					if (pSaveCS->GetCoordSysType()==COORDSYS_33MATRIX)
// 					{
// 						double m[9];
// 						Matrix33FromMove(-col*stepx,-row*stepy,m);
// 						cs.CopyFrom(pSaveCS);
// 						cs.MultiplyMatrix(m);
// 					}
// 					else if (pSaveCS->GetCoordSysType()==COORDSYS_COMPOUND)
// 					{
// 						double m[9];
// 						Matrix33FromMove(-col*stepx,-row*stepy,m);
// 						cs0.Create33Matrix(m);
// 						CArray<CCoordSys*,CCoordSys*> arr;
// 						
// 						arr.Add(pSaveCS);arr.Add(&cs0);
// 						cs.CreateCompound(arr);
// 					}
// 					else
// 						return;				
// 					m_pPrintContext->GetCurContext()->SetCoordSys(&cs);
// 					m_imgLay.OnChangeCoordSys(0);			
// 					m_pPrintContext->GetCurContext()->BeginDrawing();					
// 					m_imgLay.Draw();
// 					m_pPrintContext->GetCurContext()->EndDrawing();
// 					m_pPrintContext->BltImgToPrint(CRect(col*stepx,row*stepy,right,bottom));
// 	//				m_imgLay.Destroy();
// 				}
// 			}
// 			m_imgLay.Destroy();
// 			m_pPrintContext->EndDrawImg();
// 			
// /*			m_pPrintContext->SetCoordSys(pSaveCS);*/
// 			m_pPrintContext->SetDCSize(oldSize);
// 			m_pPrintContext->SetViewRect(oldRect);
// 			m_imgLay.SetContext(pOldContext);
// 			
// 			m_imgLay.InitCache(GetBestCacheTypeForLayer(&m_imgLay),
// 			CSize(512,512),49);
// 			m_imgLay.ClearAll();
// 		}
// 
// 		GrElementList *pElementList = m_vectLayL.GetGrElementList();
// 		m_pPrintContext->BeginDrawVector();
// 		m_pPrintContext->GetCurContext()->BeginDrawing();
// 		((C2DGDIDrawingContext*)m_pPrintContext->GetCurContext())->PrintGrBuffer2d(pElementList,CDrawingContext::modeNormal,PrintDx/(double)ScreenDx);
// 		m_pPrintContext->GetCurContext()->EndDrawing();	
// 		m_pPrintContext->EndDrawVector();
// 	}

	if(!PrePrint(pDC,pInfo))
		return;
	HDC hdcPrinter = pDC->GetSafeHdc();
	::SetStretchBltMode(hdcPrinter,STRETCH_DELETESCANS);
	if (hdcPrinter == NULL)
	{
		MessageBox(_T("Buy a printer!"));
		return;
	}
	{
		CClientDC dc(this);
		int ScreenDx = dc.GetDeviceCaps(LOGPIXELSX);
		int ScreenDy = dc.GetDeviceCaps(LOGPIXELSY);
		int PrintDx = pDC->GetDeviceCaps(LOGPIXELSX);
		int PrintDy = pDC->GetDeviceCaps(LOGPIXELSY);	

		m_mutiImgLay.Destroy();
//		m_pPrintContext->SetBackColor(RGB(255,255,255));
		if(m_bOverlayImgLay)
		{		
			CSize oldSize = m_pPrintContext->GetDCSize();
			CRect oldRect; 
			m_pPrintContext->GetViewRect(oldRect);
			
			CDrawingContext *pOldContext = m_mutiImgLay.GetContext();

			CCoordSys *pSaveCS = m_pPrintContext->GetCoordSys();
			CCoordSys cs,cs0;
			m_mutiImgLay.ClearAll();			
		
			int col = 0, row = 0,stepx = 1024,stepy = 768;
			int colNum = oldSize.cx/stepx;
			int rowNum = oldSize.cy/stepy;
			m_pPrintContext->SetDCSize(CSize(stepx,stepy));
			m_pPrintContext->SetViewRect(CRect(0,0,stepx,stepy));
			m_pPrintContext->BeginDrawImg();
			m_mutiImgLay.SetContext(m_pPrintContext->GetCurContext());
			
			m_mutiImgLay.InitCache(GetBestCacheTypeForLayer(&m_mutiImgLay),
			CSize(512,512),49);
			m_mutiImgLay.m_bUseImageThread = FALSE;
			m_mutiImgLay.InitParams();
			if (!pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))//gaibixu
			{
				GProgressStart(colNum*rowNum);
			}
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
					m_pPrintContext->GetCurContext()->SetCoordSys(&cs);
					m_mutiImgLay.OnChangeCoordSys(0);
					
					m_pPrintContext->GetCurContext()->BeginDrawing();
					m_pPrintContext->GetCurContext()->EraseBackground();
//					::glClear( GL_COLOR_BUFFER_BIT );
					m_mutiImgLay.Draw();
					m_pPrintContext->GetCurContext()->EndDrawing();
					m_pPrintContext->BltImgToPrint(CRect(col*stepx,row*stepy,right,bottom));
	//				m_imgLay.Destroy();
					if (!pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
					{
						GProgressStep();
					}
				}
			}			
			m_mutiImgLay.Destroy();
			m_pPrintContext->EndDrawImg();
			m_pPrintContext->SetDCSize(oldSize);
			m_pPrintContext->SetViewRect(oldRect);
			m_mutiImgLay.SetContext(pOldContext);
			
			m_mutiImgLay.InitCache(GetBestCacheTypeForLayer(&m_mutiImgLay),
			CSize(512,512),49);
			m_mutiImgLay.ClearAll();
			m_mutiImgLay.m_bUseImageThread = TRUE;
			m_mutiImgLay.InitParams();
			if (!pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
			{
				GProgressEnd();
			}
		}

		GrElementList *pElementList = m_vectLayL.GetGrElementList();
		m_pPrintContext->BeginDrawVector();
		m_pPrintContext->GetCurContext()->BeginDrawing();
		((C2DGDIDrawingContext*)m_pPrintContext->GetCurContext())->PrintGrBuffer2d(pElementList,CDrawingContext::modeNormal,PrintDx/(double)ScreenDx);
		m_pPrintContext->GetCurContext()->EndDrawing();	
		m_pPrintContext->EndDrawVector();
	}
}
