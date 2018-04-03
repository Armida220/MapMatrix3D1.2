// SceneView.cpp : implementation file
//

#include "stdafx.h"
#include "glew.h"
#include "DrawingContext.h"
#include "SceneView.h"
#include "resource.h"
#include "SmartViewFunctions.h"
#include "DrawingContext.h"
#include <math.h>
#include "matrix.h"
#include "DrawLineDC.h"
#include "RealtimeDrawingLayer.h"
#include "DlgViewAdjust.h"
#include "Rasterlayer.h"
#include "MutiRasterLayer.h"
#include "VectorLayer.h "
#include "TmpGraphLayer.h "
#include "GlobalFunc.h "
#include "DlgSlider.h"

#include "NoDblClkButton.h"

#include "SmartViewLocalRes.h"
#include "DummyDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_VIEW_ADJUSTUPDATE (WM_USER+20)
#define WM_VIEW_CTRLMESSAGE  (WM_USER+21)
#define WM_VIEW_ZOOMCHANGE   (WM_USER+22)

extern AFX_EXTENSION_MODULE SmartViewDLL;

extern BOOL LoadMyString(CString& str, UINT uID);

UINT CtrlButID[] = 
{ IDC_BUTTON_REFRESH,IDC_BUTTON_ADJUST,IDC_BUTTON_HANDMOVE,
IDC_BUTTON_ZOOMIN,IDC_BUTTON_ZOOMOUT,IDC_BUTTON_ZOOMFIT,IDC_BUTTON_ZOOMRECT,
IDC_BUTTON_STEREO,
IDC_BUTTON_CURSOR};

UINT CtrlButBmpID[] = 
{ IDB_BUTTON_REFRESH,IDB_BUTTON_ADJUST,IDB_BUTTON_HANDMOVE,
IDB_BUTTON_ZOOMIN,IDB_BUTTON_ZOOMOUT,IDB_BUTTON_ZOOMFIT,IDB_BUTTON_ZOOMRECT,
IDB_BUTTON_STEREO,
IDB_BUTTON_CURSOR};

UINT CtrlButTipID[] = 
{ IDS_TIPS_REFRESH,IDS_TIPS_VIEWADJUST,IDS_TIPS_HANDMOVE,
IDS_TIPS_ZOOMIN,IDS_TIPS_ZOOMOUT,IDS_TIPS_ZOOMFIT,IDS_TIPS_ZOOMRECT,
IDS_TIPS_STEREO,
IDS_TIPS_CURSORSETTING};

CMyTimer::CMyTimer():m_eventClose(FALSE,TRUE)
{
	m_hThread = NULL;
}

CMyTimer::~CMyTimer()
{
	CloseTimer();
}

void CMyTimer::SetTimer(HWND hWnd, UINT nIDEvent, UINT uElapse, UINT nMsg)
{
	HANDLE h = m_hThread;
	if( h )
	{
		CloseTimer();
	}

	TimerItem item = {hWnd,nIDEvent,uElapse,nMsg};
	m_arrItems.Add(item);

	if( h )
	{
		StartTimer();
	}
}

void CMyTimer::StartTimer()
{
	m_hThread = CreateThread(NULL,0,ThreadFunc,(LPVOID)this,(DWORD)CREATE_SUSPENDED,NULL);
	
	if(m_hThread==NULL)
	{		
		return;
	}

	m_eventClose.ResetEvent();
	ResumeThread(m_hThread);
}

void CMyTimer::CloseTimer()
{
	if( m_hThread )
	{
		m_eventClose.SetEvent();

		if( WaitForSingleObject( m_hThread,2000 )!=WAIT_OBJECT_0 )
			TerminateThread( m_hThread, 1 );
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}


void CMyTimer::OnFinishMsg(int index)
{
}

DWORD WINAPI CMyTimer::ThreadFunc(LPVOID pParam)
{
	CMyTimer *pthis = (CMyTimer*)pParam;
	if( pthis->m_arrItems.GetSize()<=0 )
		return 0;

	while( 1 )
	{
		for( int i=0; i<pthis->m_arrItems.GetSize(); i++)
		{
			TimerItem item = pthis->m_arrItems[i];
			if (IsWindow(item.hWnd))
			{
				SendMessage(item.hWnd,item.nMsg,item.nIDEvent,(LPARAM)i );
			}			
		}
		
		if( WaitForSingleObject( (HANDLE)pthis->m_eventClose,10 )==WAIT_OBJECT_0 )
			break;
	}

	return 0;
}


MyNameSpaceBegin

extern BOOL GetMaxScreenSize(int& cx, int& cy);

/////////////////////////////////////////////////////////////////////////////
// CSceneView

IMPLEMENT_DYNCREATE(CSceneView, CView)

CSceneView::CSceneView()
{
	for( int i=0; i<sizeof(CtrlButID)/sizeof(CtrlButID[0]); i++)
	{
		m_cmdUseFlags.Add(1);
	}

	memset(m_strRegSection,0,sizeof(m_strRegSection));

	m_pContext = NULL;
	m_hViewDC = NULL;

	m_pLeftCS = NULL;
	m_pRightCS = NULL;

	m_fZoomRate = 1.0;

	int cx = 0, cy = 0;
	GetMaxScreenSize(cx,cy);

	m_nViewMargin = cx*0.6;
	m_nInnerCmd = 0;
	m_arrSaveInnerCmds.RemoveAll();

	m_nHorzParallax	= 0;
	m_nVertParallax	= 0;

	m_ptScrollOrigin.x = m_ptScrollOrigin.y = 0;

	m_bCacheReady = FALSE;

	m_bDisplayOrder = FALSE;

	memset(m_nAdjustParams,0,sizeof(m_nAdjustParams));
	m_bRightSametoLeft = TRUE;

	m_pLeftCS = new CCoordSys;
	m_pRightCS = new CCoordSys;

	m_pContext = new CStereoDrawingContext;

	m_pContext->SetDisplayMode(CStereoDrawingContext::modeShutterStereo);
	m_pContext->m_nTextMode = CStereoDrawingContext::textureModeGL;
//	m_pContext->m_bCreateOverlay = TRUE;

	m_nUpdateIdxLeft = -1;
	m_nUpdateIdxRight = -1;

	m_bTextureForVect = FALSE;

	m_nCurViewPos = -1;

	m_fViewPosDx = m_fViewPosDy = m_fViewPosDp = m_fViewPosDq = 0;

	m_bSliderZoom = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_SLIDERZOOM,FALSE);

	m_pDummyDialog = NULL;

	m_bIsSelfSizing = FALSE;
}

CSceneView::~CSceneView()
{
}


BEGIN_MESSAGE_MAP(CSceneView, CView)
	//{{AFX_MSG_MAP(CSceneView)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_COMMAND(IDC_BUTTON_REFRESH,OnRefresh)
	ON_COMMAND(IDC_BUTTON_ADJUST,OnAdjust)
	ON_COMMAND(IDC_BUTTON_ZOOMIN,OnZoomIn)
	ON_COMMAND(IDC_BUTTON_ZOOMOUT,OnZoomOut)
	ON_COMMAND(IDC_BUTTON_ZOOMFIT,OnZoomFit)
	ON_COMMAND(IDC_BUTTON_STEREO,OnDisplay)
	ON_MESSAGE(WM_VIEW_ZOOMCHANGE,OnZoomChange)
	ON_COMMAND(IDC_SLIDER_EDIT,OnZoom)
	ON_CBN_SELCHANGE(IDC_COMBO_ZOOM,OnComboZoomChange)
	
	ON_MESSAGE(WM_VIEW_ADJUSTUPDATE,OnAdjustUpdate)
	ON_COMMAND(ID_MENU_VIEWUNDO, OnViewUndo)
	ON_COMMAND(ID_MENU_VIEWREDO, OnViewRedo)
	ON_MESSAGE(WM_MYTIMER, OnMyTimer)
	ON_MESSAGE(FCCM_VIEWCLEAR,ClearData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSceneView drawing

void CSceneView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CSceneView diagnostics

#ifdef _DEBUG
void CSceneView::AssertValid() const
{
	CView::AssertValid();
}

void CSceneView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSceneView message handlers

void CSceneView::ZoomFit(CSize size)
{
	// Real image size
	CSize szMap = CalcViewSize();
	
	double change_x = double(size.cx)/szMap.cx;
	double change_y = double(size.cy)/szMap.cy;
	
	// Zoom rate == min rate
	if( change_x<change_y ) change_y = change_x;
	
	CPoint pt(szMap.cx/2,szMap.cy/2);
	CPoint pt0 = GetScrollPos();
	pt.x = pt.x-pt0.x;  
	pt.y = pt.y-pt0.y;
	
	CRect rect;
	GetClientRect(&rect);
	if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit )
		pt.x = pt.x - rect.right/2;
	
	ZoomChange( pt,change_y );
	szMap = CalcViewSize();
	if (szMap.cx>size.cx&&szMap.cy>size.cy)
	{
		ZoomNext(pt,0);
	}
	RecalcScrollBar(TRUE);
}


void CSceneView::ZoomOut()
{
	CRect rect;
	GetClientRect( &rect );
	
	if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit )
		rect.right = rect.right/2;
// 	if (GetZoomRate()>2.0&&GetZoomRate()*0.5<2.0)
// 	{
// 		ZoomChange(rect.CenterPoint(),2.0/GetZoomRate(),FALSE);
// 		return;
// 	}
// 	else if (GetZoomRate()>1.0&&GetZoomRate()<=2.0)
// 	{
// 		ZoomChange(rect.CenterPoint(),(GetZoomRate()-0.24)/GetZoomRate(),FALSE);
// 		return;
// 	}
	ZoomNext(rect.CenterPoint(),0,FALSE);
//	ZoomChange(rect.CenterPoint(),0.5,FALSE);
}


void CSceneView::ZoomIn()
{
	CRect rect;
	GetClientRect( &rect );
	
// 	if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit )
// 	{
// 		rect.right = rect.right/2;
// 	}
// 	if (GetZoomRate()<1.0&&GetZoomRate()*2.0>1.0)
// 	{
// 		ZoomChange(rect.CenterPoint(),1.0/GetZoomRate(),FALSE);
// 		return;
// 	}
// 	else if (GetZoomRate()>=1.0&&GetZoomRate()<2.0)
// 	{
// 		ZoomChange(rect.CenterPoint(),(GetZoomRate()+0.26)/GetZoomRate(),FALSE);
// 		return;
// 	}
	ZoomNext(rect.CenterPoint(),1,FALSE);
//	ZoomChange(rect.CenterPoint(),2.0,FALSE);
}


void CSceneView::ZoomChange(CPoint point,double change, BOOL bToCenter)
{
	if( m_arrPViewPos.GetSize()==0 )
		PushViewPos();

	if( IsPolarized() )point.y = point.y/2;

	double newZoom = ZoomSelect(GetZoomRate()*change);

	change = newZoom/GetZoomRate();
	
	double m[16], m2[16], mt[16];

	Matrix44FromMove(-point.x,-point.y,0,m2);
	Matrix44FromZoom(change,change,change,m);

	matrix_multiply(m,m2,4,mt);

	if( !bToCenter )
	{
		Matrix44FromMove(point.x,point.y,0,m2);
	}
	else
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		Matrix44FromMove(rcClient.CenterPoint().x,rcClient.CenterPoint().y*(IsPolarized()?0.5:1),0,m2);
	}

	matrix_multiply(m2,mt,4,m);
	
	m_pLeftCS->MultiplyMatrix(m);
	m_pRightCS->MultiplyMatrix(m);
	m_laymgrLeft.OnChangeCoordSys(FALSE);
	m_laymgrRight.OnChangeCoordSys(FALSE);
	OnChangeCoordSys(FALSE);

	CDrawingContext *pDC = m_pContext->GetLeftContext();
	if( pDC )pDC->SetCoordSys(m_pLeftCS);

	pDC = m_pContext->GetRightContext();
	if( pDC )pDC->SetCoordSys(m_pRightCS);

	m_fZoomRate  = newZoom;

	RecalcScrollBar(FALSE);
	Invalidate(TRUE);

	PushViewPos();
}


void CSceneView::Scroll(int dx,int dy,int dp,int dq, int step)
{
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rect);
		
	int original_YPos = m_ScrollCtrl.GetPos(SB_VERT);
	int original_XPos = m_ScrollCtrl.GetPos(SB_HORZ);
	
	m_ScrollCtrl.SetPos( original_XPos-dx,original_YPos-dy );	
	
	int yPos = m_ScrollCtrl.GetPos(SB_VERT);
	int xPos = m_ScrollCtrl.GetPos(SB_HORZ);
	
	m_nHorzParallax -= dp;
	m_nVertParallax -= dq;
	
	dx = original_XPos - xPos;
	dy = original_YPos - yPos;
	
	RawScroll(dx,dy,dp,dq,step);
//	Invalidate(FALSE);
}


void CSceneView::OnPreInnerCmd(UINT nInnerCmd)
{
	m_arrSaveInnerCmds.Add(m_nInnerCmd);
	m_nInnerCmd = nInnerCmd;
}


void CSceneView::OnPostInnerCmd()
{
	int size = m_arrSaveInnerCmds.GetSize();
	if (size > 0)
	{
		m_nInnerCmd = m_arrSaveInnerCmds[size-1];
		m_arrSaveInnerCmds.RemoveAt(size-1);
	}
	else 
	{
		m_nInnerCmd = 0;
	}
	if( ::GetFocus()!=m_hWnd )::SetFocus(m_hWnd);
}


double CSceneView::ZoomSelect(double wantZoom)
{
	if( m_arrCanZoomRate.GetSize()<=0 )
		return wantZoom;
	
	double dis, min = -1;
	int num = m_arrCanZoomRate.GetSize(), k = -1;
	for( int i=num-1; i>=0; i--)
	{
		dis = fabs(wantZoom-m_arrCanZoomRate[i]);
		if( dis<min || min<0 )
		{
			min = dis;
			k = i;
		}
	}
	
	if( k>=0 )return m_arrCanZoomRate[k];
	return m_arrCanZoomRate[0];
}


double CSceneView::GetComboNextZoom(int dir)
{
	int num = m_arrCanZoomRate.GetSize();
	if( num>0 )
	{
		if( dir )
		{
			for( int i=0; i<num; i++)
			{
				if( m_fZoomRate<m_arrCanZoomRate[i] )return m_arrCanZoomRate[i];
			}
		}
		else
		{
			for( int i=num-1; i>=0; i--)
			{
				if( m_fZoomRate>m_arrCanZoomRate[i] )return m_arrCanZoomRate[i];
			}
		}
	}
	
	return -1;
}

void CSceneView::ZoomNext(CPoint point,int dir, BOOL bToCenter)
{
	if( IsPolarized() )point.y = point.y/2;

	if( m_arrPViewPos.GetSize()==0 )
		PushViewPos();
	
	double newZoom = GetComboNextZoom(dir);
	if (fabs(newZoom+1)<1e-10)
	{
		return;
	}
//	float newZoom = ZoomSelect(GetZoomRate()*change);
	
	double change = newZoom/GetZoomRate();
	
	double m[16], m2[16], mt[16];
	
	Matrix44FromMove(-point.x,-point.y,0,m2);
	Matrix44FromZoom(change,change,change,m);
	
	matrix_multiply(m,m2,4,mt);
	
	if( !bToCenter )
	{
		Matrix44FromMove(point.x,point.y,0,m2);
	}
	else
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		Matrix44FromMove(rcClient.CenterPoint().x,rcClient.CenterPoint().y*(IsPolarized()?0.5:1),0,m2);
	}
	
	matrix_multiply(m2,mt,4,m);
	
	m_pLeftCS->MultiplyMatrix(m);
	m_pRightCS->MultiplyMatrix(m);
	m_laymgrLeft.OnChangeCoordSys(FALSE);
	m_laymgrRight.OnChangeCoordSys(FALSE);
	OnChangeCoordSys(FALSE);
	
	CDrawingContext *pDC = m_pContext->GetLeftContext();
	if( pDC )pDC->SetCoordSys(m_pLeftCS);
	
	pDC = m_pContext->GetRightContext();
	if( pDC )pDC->SetCoordSys(m_pRightCS);
	
	m_fZoomRate  = newZoom;
	
	RecalcScrollBar(FALSE);
	Invalidate(TRUE);

	PushViewPos();
}

void CSceneView::RawScroll(int dx,int dy,int dp,int dq,int step0)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	if( m_pContext->CanUpdateErasable() )
	{		
		CRect *pRcPaint = &rcClient;
		
		CStereoDrawingContext *pDC = (CStereoDrawingContext*)m_pContext;
		
		int i, nFindIdxLeft = 0, nFindIdxRight = 0;
		
		for( i=m_laymgrLeft.GetDrawingLayerCount()-1; i>=nFindIdxLeft; i--)
		{
			CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
			if( !pLayer->CanErase() )
			{
				nFindIdxLeft = i;
				break;
			}
		}
		
		for( i=m_laymgrRight.GetDrawingLayerCount()-1; i>=nFindIdxRight; i--)
		{
			CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
			if( !pLayer->CanErase() )
			{
				nFindIdxRight = i;
				break;
			}
		}
		
		pDC->SetViewRect(*pRcPaint);
		pDC->BeginDrawingErasable();

		//红绿和偏振立体下，左右片的绘制可能会相互影响，
		//必须按照如下方式绘图和更新，才能保证擦除的顺序和绘图的顺序精确相反；
		if( (IsRGStereo() || IsPolarized()) )
		{
			int start = max(m_laymgrLeft.GetDrawingLayerCount(),m_laymgrRight.GetDrawingLayerCount());
			int end = min(nFindIdxLeft,nFindIdxRight);
			
			for( i=start-1; i>end; i--)
			{
				CDrawingLayer *pLayer1 = m_laymgrLeft.GetDrawingLayer(i);
				CDrawingLayer *pLayer2 = m_laymgrRight.GetDrawingLayer(i);
				
				if( pLayer2 )
				{
					pDC->SetDrawSide(CStereoDrawingContext::drawRight);
					pLayer2->Erase();
				}
				if( pLayer1 )
				{
					pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
					pLayer1->Erase();
				}
			}
		}
		//左右片分开绘制，则是为了让真立体更快速
		else
		{
			if(!IsSingle())
			{
			pDC->SetDrawSide(CStereoDrawingContext::drawRight);
			
			for( i=m_laymgrRight.GetDrawingLayerCount()-1; i>nFindIdxRight; i--)
			{
				m_laymgrRight.GetDrawingLayer(i)->Erase();
			}
			}		
			pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
			
			for( i=m_laymgrLeft.GetDrawingLayerCount()-1; i>nFindIdxLeft; i--)
			{
				m_laymgrLeft.GetDrawingLayer(i)->Erase();
			}
		}
		
		pDC->EndDrawingErasable();
	}

	CArray<CRect,CRect> arrLRects, arrRRects, arrPRects;

	m_pContext->SetViewRect(rcClient);
	if( m_pContext->m_bOldStereoScroll && m_pContext->m_nTextMode==CStereoDrawingContext::textureModeGL && IsShutterStereo() )
	{
		float step = step0;

		int scroll_len = fabs((double)dx)>fabs((double)dy)?fabs((double)dx):fabs((double)dy);
		int scroll_len2 = fabs((double)dp)>fabs((double)dq)?fabs((double)dp):fabs((double)dq);

		if( scroll_len>400 )
			step = 0;

		if( scroll_len<=150 )
			step = scroll_len/2;

		if( scroll_len<=70 )
			step = 30;
		if( scroll_len<=51 )
			step = 17;
		if( scroll_len<=33 )
			step = 11;
		if( scroll_len<=24 )
			step = 8;
		if( scroll_len<=18 )
			step = 6;
		if( scroll_len<=10 )
			step = 5;
		if( scroll_len<=6 )
			step = 3;
		if( scroll_len<=3 )
			step = 0;
		
		//使用曲线函数（这里使用了双曲函数）计算step，能取得更好的连续性，使立体移动的效果更平滑
		if( m_pContext->m_bCorrectFlicker )
		{
			step = sqrt(scroll_len*scroll_len/36.0 + 1);
		}
		else
		{
			step = sqrt(scroll_len*scroll_len/9.0 + 1);
		}

		if( scroll_len2>scroll_len )
			step = 0;
		if( step0==0 )
			step = 0;

		float ddx = 0, ddy = 0;
		float dis = scroll_len, fstep = step;

		if( dis<=0 )dis = 1.0f;
		if( fstep<=0 )fstep = dis;
		int nscroll = ceil(dis/fstep);
		if( nscroll<=0 )nscroll = 1;
		ddx = dx*(fstep/dis); ddy = dy*(fstep/dis);
		float dx1 = 0, dy1 = 0, dx2, dy2;
		int nscroll0 = nscroll;
		{
			m_pContext->Scroll(0,0,dp,dq,&rcClient,&arrLRects,&arrRRects,&arrPRects);
			double m[16];			
			Matrix44FromMove(dp,dq,0,m);
			m_pRightCS->MultiplyMatrix(m);
			m_laymgrRight.OnChangeCoordSys(TRUE);
			OnChangeCoordSys(TRUE);
		}
		while( nscroll-- )
		{
			dx2 = dx1; dy2 = dy1;
			dx1 += ddx; dy1 += ddy;
			if( (dx1-dx)*dx>0 )dx1 = dx;
			if( (dy1-dy)*dy>0 )dy1 = dy;
			dx2 = dx1-dx2; dy2 = dy1-dy2;
			{
				arrLRects.RemoveAll();
				arrRRects.RemoveAll();
				arrPRects.RemoveAll();
				m_pContext->Scroll(dx2,dy2,0,0,&rcClient,&arrLRects,&arrRRects,&arrPRects);
				double m[16];
				Matrix44FromMove(dx2,dy2,0,m);
				m_pLeftCS->MultiplyMatrix(m);
				Matrix44FromMove(dx2,dy2,0,m);
				m_pRightCS->MultiplyMatrix(m);
				m_laymgrLeft.OnChangeCoordSys(TRUE);
				m_laymgrRight.OnChangeCoordSys(TRUE);
				OnChangeCoordSys(TRUE);
				for( int i=0; i<arrPRects.GetSize(); i++)
				{
					DoPaint(&arrPRects.GetAt(i),CStereoDrawingContext::drawLeft|CStereoDrawingContext::drawRight);
				}
			}
		}
		if( 0 && nscroll0==1 && step0>0 )
		{
			DoPaint(NULL,CStereoDrawingContext::drawLeft|CStereoDrawingContext::drawRight);
		}
	}
	else
	{
		m_pContext->Scroll(dx,dy,dp,dq,&rcClient,&arrLRects,&arrRRects,&arrPRects);
		
		double m[16];
		Matrix44FromMove(dx,dy,0,m);
		m_pLeftCS->MultiplyMatrix(m);
		
		Matrix44FromMove(dx+dp,dy+dq,0,m);
		m_pRightCS->MultiplyMatrix(m);
		
		m_laymgrLeft.OnChangeCoordSys(TRUE);
		m_laymgrRight.OnChangeCoordSys(TRUE);
		OnChangeCoordSys(TRUE);
		
		if( m_pContext->CanUpdateErasable() )
		{
			for( int i=0; i<arrLRects.GetSize(); i++)
			{
				DoPaint(&arrLRects.GetAt(i),CStereoDrawingContext::drawLeft,TRUE,FALSE);
			}	
			for( i=0; i<arrRRects.GetSize(); i++)
			{
				DoPaint(&arrRRects.GetAt(i),CStereoDrawingContext::drawRight,TRUE,FALSE);
			}
			for( i=0; i<arrPRects.GetSize(); i++)
			{
				DoPaint(&arrPRects.GetAt(i),CStereoDrawingContext::drawLeft|CStereoDrawingContext::drawRight,TRUE,FALSE);
			}
			
			DoPaint(NULL,CStereoDrawingContext::drawLeft|CStereoDrawingContext::drawRight,FALSE,TRUE);
		}
		else
		{
			for( int i=0; i<arrLRects.GetSize(); i++)
			{
				DoPaint(&arrLRects.GetAt(i),CStereoDrawingContext::drawLeft);
			}	
			for( i=0; i<arrRRects.GetSize(); i++)
			{
				DoPaint(&arrRRects.GetAt(i),CStereoDrawingContext::drawRight);
			}
			for( i=0; i<arrPRects.GetSize(); i++)
			{
				DoPaint(&arrPRects.GetAt(i),CStereoDrawingContext::drawLeft|CStereoDrawingContext::drawRight);
			}		
		}
	}
	StereoMove();

	m_fViewPosDx += dx;
	m_fViewPosDy += dy;
	m_fViewPosDp += dp;
	m_fViewPosDq += dq;
	if( fabs(m_fViewPosDx)+fabs(m_fViewPosDy)+fabs(m_fViewPosDp)+fabs(m_fViewPosDq)>400 )
	{
		PushViewPos();
	}
}



void CSceneView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( !m_ScrollCtrl.TryOnScroll(nSBCode,nPos) )
		return;
	
	int oldPos = m_ScrollCtrl.GetPos(SB_VERT);
	
	CRect rect;
	GetClientRect(&rect);
	
	if( m_ScrollCtrl.OnVScroll( nSBCode,nPos ) )
	{
		RawScroll(0,oldPos-m_ScrollCtrl.GetPos(SB_VERT),0,0,0);
	}
	
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CSceneView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( !m_ScrollCtrl.TryOnScroll(nSBCode,nPos) )
		return;
	
	int oldPos = m_ScrollCtrl.GetPos(SB_HORZ);
	
	CRect rect;
	GetClientRect(&rect);
	
	if( m_ScrollCtrl.OnHScroll( nSBCode,nPos ) )
	{
		RawScroll(oldPos-m_ScrollCtrl.GetPos(SB_HORZ),0,0,0,0);
	}
	
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CSceneView::OnSize(UINT nType, int cx, int cy)
{
	if (m_bIsSelfSizing)
	{
		m_pContext->SetDCSize(CSize(cx, cy));
	}
	else
	{
		CRect rcClient;
		GetWindowRect(rcClient);
		GetParent()->ScreenToClient(rcClient);

		SetCtrlPos(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height());
	}
}

extern int g_nSide;
void CSceneView::OnTimer(UINT_PTR nIDEvent)
{
	switch( nIDEvent )
	{
	case TIMER_ID_PLAN_BGLOAD:
		{
			m_laymgrLeft.PlanBGLoad();
			m_laymgrRight.PlanBGLoad();
		}
		break;
	case TIMER_ID_BGLOAD:
		{
			CStereoDrawingContext *pDC = (CStereoDrawingContext*)m_pContext;
			if (pDC->GetLeftContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)))
			{
				pDC->GetLeftContext()->BeginDrawing();
				pDC->SetFillBlockGLDrawBuffer(CStereoDrawingContext::drawLeft);
			}

			m_laymgrLeft.BGLoad();

			if (pDC->GetLeftContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)))
			{
				pDC->GetLeftContext()->EndDrawing();
			}

			if (pDC->GetRightContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)))
			{
				pDC->GetRightContext()->BeginDrawing();
				pDC->SetFillBlockGLDrawBuffer(CStereoDrawingContext::drawRight);
			}

			m_laymgrRight.BGLoad();

			if (pDC->GetRightContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)))
			{
				pDC->GetRightContext()->EndDrawing();
			}
		}
		break;		

	}
	
	CView::OnTimer(nIDEvent);
}


LRESULT CSceneView::OnMyTimer(WPARAM wParam, LPARAM lParam)
{
	switch( wParam )
	{
	case TIMER_ID_DELAYVIEW:
		{
			for( int i=m_laymgrLeft.GetDrawingLayerCount()-1; i>=0; i--)
			{
				if( !m_laymgrLeft.GetDrawingLayer(i)->m_bForceDraw )
					break;
			}
			
			if( i>=0 )
			{
				if( !m_bCacheReady )
				{
					DoPaint(NULL,CStereoDrawingContext::drawLeft|CStereoDrawingContext::drawRight);
				}
				
				m_bCacheReady = CheckCacheReady();
			}

			m_timer.OnFinishMsg(lParam);
		}
		break;
	}

	return 0;
}


BOOL CSceneView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


void CSceneView::OnPaint()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	
	CPoint ptCenter = rcClient.CenterPoint();
	
	int i;
	for( i=0; i<m_laymgrLeft.GetDrawingLayerCount(); i++)
	{
		CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
		pLayer->SetViewFocus(ptCenter.x,ptCenter.y);
	
	}
	for( i=0; i<m_laymgrRight.GetDrawingLayerCount(); i++)
	{
		CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
		pLayer->SetViewFocus(ptCenter.x,ptCenter.y);
	
	}

//	m_bCacheReady = FALSE;

	// Default
	CPaintDC dc(this);
	DoPaint(&CRect(dc.m_ps.rcPaint),CStereoDrawingContext::drawLeft|CStereoDrawingContext::drawRight);


}


BOOL CSceneView::CheckCacheReady()
{
	BOOL bReady = TRUE;
	int i;
	for( i=0; i<m_laymgrLeft.GetDrawingLayerCount(); i++)
	{
		CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
		if( !pLayer->CheckCacheReady() )
		{
			bReady = FALSE;
			break;
		}
	}
	for( i=0; i<m_laymgrRight.GetDrawingLayerCount(); i++)
	{
		CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
		if( !pLayer->CheckCacheReady() )
		{
			bReady = FALSE;
			break;
		}
	}
	return bReady;
}

BOOL CSceneView::DrawRectBitmap(HBITMAP hBmp,CRect bmpRect,CRect *pRcPaint, CPtrArray& layersL,CPtrArray& layersR, int whichbuf)
{
	m_pContext->SetDCSize(CSize(pRcPaint->right,pRcPaint->bottom));

	CStereoDrawingContext *pDC = (CStereoDrawingContext*)m_pContext;	

	{
		pDC->SetViewRect(*pRcPaint);
		//绘制一般图层
 		pDC->BeginDrawingBitmap();
		pDC->EraseBackground();
		if( whichbuf&CStereoDrawingContext::drawLeft )
		{
			pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
			
			pDC->GetLeftContext()->BeginDrawing();		
			pDC->GetLeftContext()->EraseBackground();
			for( int i=0; i<layersL.GetSize(); i++)
			{
				CDrawingLayer *pLayer = (CDrawingLayer *)layersL.GetAt(i);			
				if( pLayer )
				{
					pLayer->Draw();

					if( pDC->GetCurContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
					{
						glFlush();
						glFinish();
					}
				}
			}

			pDC->GetLeftContext()->EndDrawing();

			pDC->OnFinishDrawSideBitmap();
		}

		if( (whichbuf&CStereoDrawingContext::drawRight)!=0 && 
			pDC->GetDisplayMode()!=CStereoDrawingContext::modeSingle )
		{
			pDC->SetDrawSide(CStereoDrawingContext::drawRight);

			pDC->GetRightContext()->BeginDrawing();			
			pDC->GetRightContext()->EraseBackground();
			for( int i=0; i<layersR.GetSize(); i++)
			{
				CDrawingLayer *pLayer = (CDrawingLayer *)layersR.GetAt(i);			
				if( pLayer )
				{
					pLayer->Draw();

					if( pDC->GetCurContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
					{
						glFlush();
						glFinish();
					}
				}
			}
			pDC->GetRightContext()->EndDrawing();

			pDC->OnFinishDrawSideBitmap();
		}

		pDC->EndDrawingBitmap();
	}
	pDC->GetDrawBitmap(hBmp, bmpRect,pRcPaint);

	m_nUpdateIdxLeft = -1;
	m_nUpdateIdxRight = -1;
	return TRUE;
}

void CSceneView::DoPaint(const CRect *pRcPaint, int whichbuf, BOOL bIncludeNotErasable, BOOL bIncludeErasable)
{
	CRect rcClient, rcPaint;
	GetClientRect(&rcClient);
	
	if( pRcPaint==NULL )
	{
		pRcPaint = &rcClient;
	}
	else
	{
		rcPaint = rcClient&(*pRcPaint);
		pRcPaint = &rcPaint;
	}

	if( m_pContext->IsD3DLost() )
	{
		RefreshDrawingLayers();
		m_pContext->m_pD3D->ResetDevice(TRUE);
	}


	if( whichbuf==CStereoDrawingContext::drawLeft || whichbuf==CStereoDrawingContext::drawRight )
	{
		m_pContext->m_bPaintOneSide = TRUE;
	}

	CStereoDrawingContext *pDC = (CStereoDrawingContext*)m_pContext;
	int bNeedDrawErasable = 0;

	//作Cache准备
	if( bIncludeNotErasable )
	{
		pDC->SetViewRect(*pRcPaint);
		
		if( whichbuf&CStereoDrawingContext::drawLeft )
		{			
			for( int i=0; i<m_laymgrLeft.GetDrawingLayerCount(); i++)
			{
				CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
				pLayer->ExtraPrepareCaches();				
			}			
		}
		
		if( (whichbuf&CStereoDrawingContext::drawRight)!=0 && 
			pDC->GetDisplayMode()!=CStereoDrawingContext::modeSingle )
		{
			for( int i=0; i<m_laymgrRight.GetDrawingLayerCount(); i++)
			{
				CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
				pLayer->ExtraPrepareCaches();	
			}			
		}		
	}

	if( bIncludeNotErasable )
	{
		pDC->SetViewRect(*pRcPaint);
		//绘制一般图层
 		pDC->BeginDrawing();

		if( whichbuf&CStereoDrawingContext::drawLeft )
		{
			pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
			
			pDC->GetLeftContext()->BeginDrawing();

			if( bIncludeNotErasable )
			{
				//if( IsSingle() && pDC->m_nTextMode!=CStereoDrawingContext::textureModeNone );
				//else
					pDC->EraseBackground();
			}

			for( int i=0; i<m_laymgrLeft.GetDrawingLayerCount(); i++)
			{
				CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
				//if( !pLayer->IsKindOf(RUNTIME_CLASS(CRasterLayer)) )
				//	continue;
				if( pLayer->CanErase() )
				{
					if( bIncludeErasable && pDC->CanPaintErasable() )
					{
						pLayer->SetContext(pDC->GetLeftContext());
						pLayer->Draw();
						pLayer->SetContext(pDC);
					}
					else
						bNeedDrawErasable = 1;
				}
				else if( bIncludeNotErasable )
				{
					pLayer->Draw();
				}

				if( pDC->GetCurContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
				{
					glFlush();
				}
			}

			if( pDC->GetCurContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
			{
				glFinish();
			}
			pDC->GetLeftContext()->EndDrawing();

			pDC->OnFinishDrawSide();
		}

		if( (whichbuf&CStereoDrawingContext::drawRight)!=0 && 
			pDC->GetDisplayMode()!=CStereoDrawingContext::modeSingle )
		{
			pDC->SetDrawSide(CStereoDrawingContext::drawRight);

			pDC->GetRightContext()->BeginDrawing();
			
			if( bIncludeNotErasable )
			{
				//if( IsSingle() && pDC->m_nTextMode!=CStereoDrawingContext::textureModeNone );
				//else
					pDC->EraseBackground();
			}

			for( int i=0; i<m_laymgrRight.GetDrawingLayerCount(); i++)
			{
				CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
				if( pLayer->CanErase() )
				{
					if( bIncludeErasable && pDC->CanPaintErasable() )
					{
						pLayer->SetContext(pDC->GetRightContext());
						pLayer->Draw();
						pLayer->SetContext(pDC);
					}
					else
						bNeedDrawErasable = 1;
				}
				else if( bIncludeNotErasable )
				{
					pLayer->Draw();
				}
				if( pDC->GetCurContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
				{
					glFlush();
				}
			}

			if( pDC->GetCurContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
			{
				glFinish();
			}
			pDC->GetRightContext()->EndDrawing();

			pDC->OnFinishDrawSide();
		}

		pDC->EndDrawing();
	}
	else
		bNeedDrawErasable = 2;

	if( bIncludeErasable && bNeedDrawErasable!=0 )
	{
		pDC->SetViewRect(*pRcPaint);

		//绘制即时更新的图层
		pDC->BeginDrawingErasable();

		//红绿和偏振立体下，左右片的绘制可能会相互影响，
		//必须按照如下方式绘图和更新，才能保证擦除的顺序和绘图的顺序精确相反；
		if( (IsRGStereo() || IsPolarized()) )
		{
			if( whichbuf&CStereoDrawingContext::drawLeft )
			{
				pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
				
				if( m_laymgrLeft.GetDrawingLayerCount()==0 )
					pDC->EraseBackground();
			}

			if( whichbuf&CStereoDrawingContext::drawRight )
			{
				pDC->SetDrawSide(CStereoDrawingContext::drawRight);
				
				if( m_laymgrRight.GetDrawingLayerCount()==0 )
					pDC->EraseBackground();
			}

			if( bNeedDrawErasable==2 || !pDC->CanPaintErasable() )
			{
				int end = max(m_laymgrLeft.GetDrawingLayerCount(),m_laymgrRight.GetDrawingLayerCount());
				for( int i=0; i<end; i++)
				{
					CDrawingLayer *pLayer1 = m_laymgrLeft.GetDrawingLayer(i);
					CDrawingLayer *pLayer2 = m_laymgrRight.GetDrawingLayer(i);
					if( pLayer1!=NULL && pLayer1->CanErase() )
					{
						pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
						pLayer1->Draw();
					}
					
					if( pLayer2!=NULL && pLayer2->CanErase() )
					{
						pDC->SetDrawSide(CStereoDrawingContext::drawRight);
						pLayer2->Draw();
					}
				}
			}

		}
		//左右片分开绘制，则是为了让真立体更快速
		else
		{
			if( whichbuf&CStereoDrawingContext::drawLeft )
			{
				pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
				
				if( m_laymgrLeft.GetDrawingLayerCount()==0 )
					pDC->EraseBackground();
				
				for( int i=0; i<m_laymgrLeft.GetDrawingLayerCount(); i++)
				{
					CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
					if( pLayer->CanErase() )
					{
						if( bNeedDrawErasable==2 || !pDC->CanPaintErasable() )
						{						
							pLayer->Draw();
							if( pDC->GetCurContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
							{
								glFlush();
							}
						}
					}
				}
				if( pDC->GetCurContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
				{
					glFinish();
				}
			}
			
			if( whichbuf&CStereoDrawingContext::drawRight )
			{
				pDC->SetDrawSide(CStereoDrawingContext::drawRight);
				
				if( m_laymgrRight.GetDrawingLayerCount()==0 )
					pDC->EraseBackground();
				
				for( int i=0; i<m_laymgrRight.GetDrawingLayerCount(); i++)
				{
					CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
					if( pLayer->CanErase() )
					{
						if( bNeedDrawErasable==2 || !pDC->CanPaintErasable() )
						{						
							pLayer->Draw();
							if( pDC->GetCurContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
							{
								glFlush();
							}
						}
					}
				}
				if( pDC->GetCurContext()->IsKindOf(RUNTIME_CLASS(C2DGLDrawingContext)) )
				{
					glFinish();
				}
			}
		}

		pDC->EndDrawingErasable();
	}

	m_nUpdateIdxLeft = -1;
	m_nUpdateIdxRight = -1;

	m_pContext->m_bPaintOneSide = FALSE;
}


void CSceneView::OnDestroy()
{	
	if( m_pDummyDialog )
	{
		delete m_pDummyDialog;
		m_pDummyDialog = NULL;
	}

	m_timer.CloseTimer();

	for( int i=0; i<m_arrPViewPos.GetSize(); i++ )
	{
		ViewPos *p = m_arrPViewPos.GetAt(i);
		delete p;
	}

	for( i=0; i<m_laymgrLeft.GetDrawingLayerCount(); i++)
	{
		m_laymgrLeft.GetDrawingLayer(i)->Destroy();
		m_laymgrLeft.GetDrawingLayer(i)->SetContext(NULL);
	}
	for( i=0; i<m_laymgrRight.GetDrawingLayerCount(); i++)
	{
		m_laymgrRight.GetDrawingLayer(i)->Destroy();
		m_laymgrRight.GetDrawingLayer(i)->SetContext(NULL);
	}

	m_nCurViewPos = -1;

	if( m_pContext )
	{
		delete m_pContext;
		m_pContext = NULL;
	}

	if( m_pLeftCS )
	{
		delete m_pLeftCS;
		m_pLeftCS = NULL;
	}

	if( m_pRightCS )
	{
		delete m_pRightCS;
		m_pRightCS = NULL;
	}

	if( m_hViewDC )
	{
		::ReleaseDC(m_hWnd,m_hViewDC);
		m_hViewDC = NULL;
	}

	m_ctrlBarHorz.DestroyWindow();
	m_ctrlBarVert.DestroyWindow();
	m_muDisplay.DestroyMenu();

	CView::OnDestroy();

}


int  CSceneView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CSmartViewLocalRes setLocalRes;
	
	ModifyStyleEx(WS_EX_CLIENTEDGE,0);
	CWnd *pWnd = GetParent();
	ASSERT( pWnd!=0 && pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)) );
	
	m_ctrlBarHorz.Create(pWnd, IDD_DLGBAR_IMGVIEW, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, ID_HDLGBAR);
	/*m_ctrlBarHorz.AddScrollBar(IDC_HSCROLLBAR);*/
	
	int btnNum = 0;
	UINT ids[sizeof(CtrlButID)/sizeof(CtrlButID[0])];
	UINT bmps[sizeof(CtrlButID)/sizeof(CtrlButID[0])];
	for( int i=0; i<sizeof(CtrlButID)/sizeof(CtrlButID[0]); i++)
	{
		if( m_cmdUseFlags[i] )
		{
			ids[btnNum] = CtrlButID[i];	bmps[btnNum] = CtrlButBmpID[i];
			btnNum++;
		}
	}

	//test 曾进翀
	/*m_ctrlBarHorz.AddButtons(ids,bmps,btnNum);*/
	/*m_ctrlBarHorz.SetMsgWnd(m_hWnd);*/

	/*for( i=0; i<sizeof(CtrlButID)/sizeof(CtrlButID[0]); i++)
	{
		if( m_cmdUseFlags[i] )
		{
			CMFCButton *pBtn = (CMFCButton*)m_ctrlBarHorz.GetButton(CtrlButID[i]);
			if( pBtn )
			{
				CString str;
				LoadMyString(str,CtrlButTipID[i]);
				pBtn->SetTooltip(str);
			}
		}
	}*/
	
	//m_ctrlBarVert.Create(pWnd,IDD_DLGBAR_IMGVIEW,WS_CHILD|WS_VISIBLE|CBRS_RIGHT,ID_VDLGBAR);
	////test 曾进翀
	////m_ctrlBarVert.AddScrollBar(IDC_VSCROLLBAR);
	//m_ctrlBarVert.SetMsgWnd(m_hWnd);

	//test 曾进翀
	/*if (!m_bSliderZoom)
	{
	m_ctrlBarHorz.AddComboBox(IDC_COMBO_ZOOM);
	}
	else
	m_ctrlBarHorz.CreateStatic();*/
	
	//test 曾进翀
	/*m_ScrollCtrl.AttachScrollBar(SB_HORZ, m_ctrlBarHorz.GetScrollBar(IDC_HSCROLLBAR));
	m_ScrollCtrl.AttachScrollBar(SB_VERT, m_ctrlBarVert.GetScrollBar(IDC_VSCROLLBAR));*/

	//test 曾进翀
	/*m_ctrlBarHorz.GetScrollBar(IDC_HSCROLLBAR)->ShowWindow(SW_HIDE);
	m_ctrlBarVert.GetScrollBar(IDC_VSCROLLBAR)->ShowWindow(SW_HIDE);
*/
	/*for (int i = 0; i < 9; i++)
	{
		m_ctrlBarHorz.GetButton(ids[i])->ShowWindow(SW_HIDE);
	}
*/
	//m_ScrollCtrl.SetViewWnd( m_hWnd );

	m_pLeftCS->Create44Matrix(NULL);
	m_pRightCS->Create44Matrix(NULL);

	m_hViewDC = ::GetDC(m_hWnd);
	m_pContext->CreateContext(m_hViewDC);
	m_pContext->m_hInterleavedWnd = GetSafeHwnd();
	m_pContext->SetRegPath(m_strRegSection);

	CDrawingContext *pDC = m_pContext->GetLeftContext();
	if( pDC )pDC->SetCoordSys(m_pLeftCS);
	pDC = m_pContext->GetRightContext();
	if( pDC )pDC->SetCoordSys(m_pRightCS);

	HMENU hMenu = ::LoadMenu(SmartViewDLL.hModule,MAKEINTRESOURCE(IDR_MENU));
	if( hMenu )
	{
		if( m_muDisplay.m_hMenu )
		{
			HMENU hTemp = m_muDisplay.m_hMenu;
			m_muDisplay.Detach();
			::DestroyMenu( hTemp );
		}

		m_muDisplay.Attach(hMenu);
	}
	

	return 0;
}


void CSceneView::SetCtrlPos(int x, int y, int cx, int cy)
{
	m_bIsSelfSizing = TRUE;

	::MoveWindow(m_hWnd, x, y, cx - m_ctrlBarVert.GetButtonsWidth(), cy - m_ctrlBarHorz.GetButtonsHeight(), TRUE);

	::MoveWindow(m_ctrlBarHorz.GetSafeHwnd(), x, y + cy - m_ctrlBarHorz.GetButtonsHeight(),
		cx, m_ctrlBarHorz.GetButtonsHeight(), TRUE);

	m_ctrlBarHorz.AdjustLayout();

	::MoveWindow(m_ctrlBarVert.GetSafeHwnd(), x + cx - m_ctrlBarVert.GetButtonsWidth(), y,
		m_ctrlBarVert.GetButtonsWidth(), cy - m_ctrlBarHorz.GetButtonsHeight(), TRUE);

	m_ctrlBarVert.AdjustLayout();

	m_bIsSelfSizing = FALSE;
}


void CSceneView::OnComboZoomChange()
{
	CComboBox *pCtrl = m_ctrlBarHorz.GetComboBox(IDC_COMBO_ZOOM);
	if( pCtrl ) 
	{
		int idx = pCtrl->GetCurSel();
		if( idx>=0 && idx<m_arrCanZoomRate.GetSize() )
		{
			int save = m_nInnerCmd;
			m_nInnerCmd = IDC_COMBO_ZOOM;
			OnPostInnerCmd();
			
			CRect rect;
			GetClientRect( &rect );
			
			ZoomChange( rect.CenterPoint(),m_arrCanZoomRate[idx]/m_fZoomRate );
			OnPostInnerCmd();
			m_nInnerCmd = save;
		}
	}
}

void CSceneView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	CWinApp *pApp = AfxGetApp();

	m_pContext->m_bNoLineWeight = (pApp==NULL?0:pApp->GetProfileInt(m_strRegSection,_T("NoLineWeight"),0));
	m_pContext->GetLeftContext()->m_bNoLineWeight = m_pContext->m_bNoLineWeight; 
	m_pContext->GetRightContext()->m_bNoLineWeight = m_pContext->m_bNoLineWeight;

	SetTimer(TIMER_ID_PLAN_BGLOAD, 300,NULL);
	SetTimer(TIMER_ID_BGLOAD   , 300,NULL);
	//SetTimer(TIMER_ID_DELAYVIEW, 10,NULL);

	m_pDummyDialog = new CDummyDialog(this, (PFUNCALLBACK)&CSceneView::OnMyTimer);
	m_pDummyDialog->Create(IDD_DLGBAR_IMGVIEW);
	m_pDummyDialog->ShowWindow(SW_HIDE);

	m_timer.SetTimer(m_pDummyDialog->m_hWnd,TIMER_ID_DELAYVIEW,1,WM_THREADMSG);
	m_timer.StartTimer();
	//m_timer.SetTimer(m_hWnd,TIMER_ID_DELAYVIEW,1,WM_MYTIMER);
	//m_timer.StartTimer();
	

	CRect rcClient;
	GetClientRect(&rcClient);
	
	double m[16];
	Matrix44FromMove(0,0,0,m);
	m_pLeftCS->MultiplyMatrix(m);
	m_pRightCS->MultiplyMatrix(m);

	Matrix44FromZoom(1.0,-1.0,1.0,m);
	m_pLeftCS->MultiplyMatrix(m);
	m_pRightCS->MultiplyMatrix(m);
	
	Matrix44FromMove(0,rcClient.Height(),0,m);
	m_pLeftCS->MultiplyMatrix(m);
	m_pRightCS->MultiplyMatrix(m);
	
	RecalcScrollBar( TRUE );

	ReadAdjustParams();
	SetAdjustParams();
}


CSize CSceneView::CalcViewSize()
{
	Envelope e = m_laymgrLeft.GetDataBound();
	e.TransformGrdToClt(m_pLeftCS,1);

	//需要取整的，与矢量图层
	e.m_xl = floor(e.m_xl);
	e.m_yl = floor(e.m_yl);
	e.m_xh = ceil(e.m_xh);
	e.m_yh = ceil(e.m_yh);

	CSize szView;
	szView.cx = ceil(e.Width());
	szView.cy = ceil(e.Height());
	
	return szView;
}

void CSceneView::RecalcScrollBar(BOOL breset)
{
	Envelope e = m_laymgrLeft.GetDataBound();
	e.TransformGrdToClt(m_pLeftCS,1);
	
	m_ptScrollOrigin.x = floor(e.m_xl);
	m_ptScrollOrigin.y = floor(e.m_yl);

	CRect rect;
	GetClientRect( &rect );
	CSize szClt = rect.Size();

	CSize sz = CalcViewSize();

	sz.cx = sz.cx+2*m_nViewMargin;   
	sz.cy = sz.cy+2*m_nViewMargin;

	//滚动条的位置应是：当前窗口左上角(0,0)相对于滚动画布的左上角的坐标值
	double xNewPos, yNewPos;	
	xNewPos = -(m_ptScrollOrigin.x-m_nViewMargin);
	yNewPos = -(m_ptScrollOrigin.y-m_nViewMargin);
	
	m_ScrollCtrl.SetRangeAndPos( xNewPos,yNewPos,szClt,CRect(0,0,sz.cx,sz.cy) );
	
	if( breset )
	{
		//定位到中心后滚动条的位置应当是：滚动画布的中心坐标减去半屏幕尺寸后的坐标
		xNewPos = sz.cx/2-szClt.cx/2;
		yNewPos = sz.cy/2-szClt.cy/2;

		double m[16];
		Matrix44FromMove(-(m_ptScrollOrigin.x + xNewPos-m_nViewMargin),-(m_ptScrollOrigin.y + yNewPos-m_nViewMargin),0,m);
		
		m_pLeftCS->MultiplyMatrix(m);
		m_pRightCS->MultiplyMatrix(m);

		m_laymgrLeft.OnChangeCoordSys(TRUE);
		m_laymgrRight.OnChangeCoordSys(TRUE);
		OnChangeCoordSys(TRUE);

		m_ScrollCtrl.SetPos(xNewPos,yNewPos);
	}
	else
	{
		int xPos, yPos;
		xPos = m_ScrollCtrl.GetPos(SB_HORZ);
		yPos = m_ScrollCtrl.GetPos(SB_VERT);

		if( xPos!=xNewPos || yPos!=yNewPos )
		{
			double m[16];
			Matrix44FromMove(xNewPos-xPos,yNewPos-yPos,0,m);

			m_pLeftCS->MultiplyMatrix(m);
			m_pRightCS->MultiplyMatrix(m);
			m_laymgrLeft.OnChangeCoordSys(TRUE);
			m_laymgrRight.OnChangeCoordSys(TRUE);
			OnChangeCoordSys(TRUE);
		}
	}
	
	e = m_laymgrLeft.GetDataBound();
	e.TransformGrdToClt(m_pLeftCS,1);
	
	m_ptScrollOrigin.x = floor(e.m_xl);
	m_ptScrollOrigin.y = floor(e.m_yl);

	RecalcZoomRateList();
	//	ZoomFit(rect.Size());
	ZoomSelect(m_fZoomRate);
	
	UpdateComboZoom();
}

void CSceneView::UpdateWindow_ForceDraw()
{
	CArray<BOOL,BOOL> arrFlags1, arrFlags2;
	m_laymgrLeft.SaveForceDrawFlags(arrFlags1);
	m_laymgrRight.SaveForceDrawFlags(arrFlags2);
	
	m_laymgrLeft.SetForceDrawFlags(TRUE);
	m_laymgrRight.SetForceDrawFlags(TRUE);
	
	UpdateWindow();
	
	m_laymgrLeft.SetForceDrawFlags(arrFlags1);
	m_laymgrRight.SetForceDrawFlags(arrFlags2);
}

void CSceneView::OnRefresh()
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_REFRESH;
	OnPreInnerCmd(IDC_BUTTON_REFRESH);
	if (m_laymgrLeft.IsIncludeRasterLayer())
	{
		SetAdjustParams();

		if( m_nAdjustParams[2]!=0 || m_nAdjustParams[5]!=0 )
			RefreshDrawingLayers();
	}	
	Invalidate(FALSE);
	UpdateWindow_ForceDraw();
	OnPostInnerCmd();
//	m_nInnerCmd = save;
}


void CSceneView::OnZoomIn()
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_ZOOMIN;
	OnPreInnerCmd(IDC_BUTTON_ZOOMIN);
	ZoomIn();
	OnPostInnerCmd();
//	m_nInnerCmd = save;
	StereoMove();
}


void CSceneView::OnZoomOut()
{
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_ZOOMOUT;
	OnPreInnerCmd(IDC_BUTTON_ZOOMOUT);
	ZoomOut();
	OnPostInnerCmd();
//	m_nInnerCmd = save;
	StereoMove();
}


void CSceneView::OnZoomFit()
{
	CRect rect;
	GetClientRect(&rect);
	
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_ZOOMFIT;
	OnPreInnerCmd(IDC_BUTTON_ZOOMFIT);
	ZoomFit(rect.Size());
	OnPostInnerCmd();
//	m_nInnerCmd = save;
	StereoMove();
}


void CSceneView::OnDisplay()
{
// 	HMENU hMenu = ::LoadMenu(SmartViewDLL.hModule,MAKEINTRESOURCE(IDR_MENU));
// 	if( !hMenu )return;
// 	if( m_muDisplay.m_hMenu )
// 	{
// 		HMENU hTemp = m_muDisplay.m_hMenu;
// 		m_muDisplay.Detach();
// 		::DestroyMenu( hTemp );
// 	}

// 	int savecmd = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_STEREO;
	OnPreInnerCmd(IDC_BUTTON_STEREO);

	CPoint pos;
	::GetCursorPos(&pos);
	CButton *pBtn = m_ctrlBarHorz.GetButton(IDC_BUTTON_STEREO);
	if( pBtn )
	{
		CRect rect;
		pBtn->GetWindowRect(&rect);
		pos = CPoint(rect.right,rect.top);
	}

//	m_muDisplay.Attach(hMenu);
	CMenu* pSumMenu = m_muDisplay.GetSubMenu(0);
	if( pSumMenu )
	{
		if( m_pContext->GetRCContext()==NULL )
			pSumMenu->EnableMenuItem(ID_MENU_STEREO,MF_BYCOMMAND|MF_GRAYED);

		BOOL bNeedRefreshLayers = FALSE;

		if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeRGStereo )
		{
			pSumMenu->CheckMenuItem(ID_MENU_SPLIT,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_STEREO,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_RGSTEREO,MF_BYCOMMAND|MF_CHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_POLARIZED,MF_BYCOMMAND|MF_UNCHECKED);
		}
		else if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeInterleavedStereo )
		{
			pSumMenu->CheckMenuItem(ID_MENU_SPLIT,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_STEREO,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_RGSTEREO,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_POLARIZED,MF_BYCOMMAND|MF_CHECKED);
		}
		else if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeShutterStereo )
		{
			pSumMenu->CheckMenuItem(ID_MENU_SPLIT,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_STEREO,MF_BYCOMMAND|MF_CHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_RGSTEREO,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_POLARIZED,MF_BYCOMMAND|MF_UNCHECKED);
		}
		else if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit )
		{
			pSumMenu->CheckMenuItem(ID_MENU_SPLIT,MF_BYCOMMAND|MF_CHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_STEREO,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_RGSTEREO,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_POLARIZED,MF_BYCOMMAND|MF_UNCHECKED);
		}
		else if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSingle )
		{
			pSumMenu->CheckMenuItem(ID_MENU_SPLIT,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_STEREO,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_RGSTEREO,MF_BYCOMMAND|MF_UNCHECKED);
			pSumMenu->CheckMenuItem(ID_MENU_POLARIZED,MF_BYCOMMAND|MF_UNCHECKED);
			
			pSumMenu->EnableMenuItem(ID_MENU_SPLIT,MF_BYCOMMAND|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_MENU_STEREO,MF_BYCOMMAND|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_MENU_RGSTEREO,MF_BYCOMMAND|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_MENU_POLARIZED,MF_BYCOMMAND|MF_GRAYED);
		}
		
		if( m_pContext->m_bNoLineWeight )
			pSumMenu->CheckMenuItem(ID_MENU_NOLINEWEIGHT,MF_BYCOMMAND|MF_CHECKED);
		else
			pSumMenu->CheckMenuItem(ID_MENU_NOLINEWEIGHT,MF_BYCOMMAND|MF_UNCHECKED);
		
		if( m_pContext->m_bNoHatch )
			pSumMenu->CheckMenuItem(ID_MENU_NOHATCH,MF_BYCOMMAND|MF_CHECKED);
		else
			pSumMenu->CheckMenuItem(ID_MENU_NOHATCH,MF_BYCOMMAND|MF_UNCHECKED);
		
		if( m_bDisplayOrder )
			pSumMenu->CheckMenuItem(ID_MENU_REVERSE_DISPLAYORDER,MF_BYCOMMAND|MF_CHECKED);
		else
			pSumMenu->CheckMenuItem(ID_MENU_REVERSE_DISPLAYORDER,MF_BYCOMMAND|MF_UNCHECKED);
		
		if( m_pContext->m_bInverseStereo )
			pSumMenu->CheckMenuItem(ID_MENU_INVERSE_STEREO,MF_BYCOMMAND|MF_CHECKED);
		else
			pSumMenu->CheckMenuItem(ID_MENU_INVERSE_STEREO,MF_BYCOMMAND|MF_UNCHECKED);

		int cmd = pSumMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RETURNCMD,pos.x,pos.y,this,NULL);

		int nOldDisplayMode = m_pContext->GetDisplayMode();

		if( cmd==ID_MENU_STEREO )
		{
			m_pContext->SetDisplayMode(CStereoDrawingContext::modeShutterStereo);
		}
		else if( cmd==ID_MENU_SPLIT )
		{
			m_pContext->SetDisplayMode(CStereoDrawingContext::modeSplit);
		}
		else if( cmd==ID_MENU_RGSTEREO )
		{
			m_pContext->SetDisplayMode(CStereoDrawingContext::modeRGStereo);
		}
		else if( cmd==ID_MENU_POLARIZED )
		{
			m_pContext->SetDisplayMode(CStereoDrawingContext::modeInterleavedStereo);
		}
		else if( cmd==ID_MENU_NOHATCH )
		{
			m_pContext->m_bNoHatch = !m_pContext->m_bNoHatch;
			m_pContext->GetLeftContext()->m_bNoHatch = m_pContext->m_bNoHatch; 
			m_pContext->GetRightContext()->m_bNoHatch = m_pContext->m_bNoHatch; 

			bNeedRefreshLayers = TRUE;
		}
		else if( cmd==ID_MENU_REVERSE_DISPLAYORDER )
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

			bNeedRefreshLayers = TRUE;

		}
		else if( cmd==ID_MENU_NOLINEWEIGHT )
		{
			m_pContext->m_bNoLineWeight = !m_pContext->m_bNoLineWeight;
			m_pContext->GetLeftContext()->m_bNoLineWeight = m_pContext->m_bNoLineWeight; 
			m_pContext->GetRightContext()->m_bNoLineWeight = m_pContext->m_bNoLineWeight;

			CWinApp *pApp = AfxGetApp();
			if( pApp )pApp->WriteProfileInt(m_strRegSection,_T("NoLineWeight"),m_pContext->m_bNoLineWeight);

			bNeedRefreshLayers = TRUE;
		}
		else if( cmd==ID_MENU_INVERSE_STEREO )
		{
			m_pContext->m_bInverseStereo = !m_pContext->m_bInverseStereo;
		}
		else
		{
			SendMessage(WM_COMMAND,cmd);
			OnPostInnerCmd();
//			m_nInnerCmd = savecmd;
			if(cmd==0)Invalidate(FALSE);
			return;
		}

		int nNewDisplayMode = m_pContext->GetDisplayMode();
		if( bNeedRefreshLayers || ( nOldDisplayMode!=nNewDisplayMode && 
			(nOldDisplayMode==CStereoDrawingContext::modeRGStereo || nNewDisplayMode==CStereoDrawingContext::modeRGStereo) ) )
		{
			RefreshDrawingLayers();
		}

		ReadAdjustParams();
		SetAdjustParams();

		RecalcZoomRateList();
		UpdateComboZoom();
		Invalidate(FALSE);
	}
	
	OnPostInnerCmd();
//	m_nInnerCmd = savecmd;
}

void CSceneView::RefreshDrawingLayers()
{
	for( int i=0; i<m_laymgrLeft.GetDrawingLayerCount(); i++)
	{
		m_laymgrLeft.GetDrawingLayer(i)->ClearAll();
	}
	for( i=0; i<m_laymgrRight.GetDrawingLayerCount(); i++)
	{
		m_laymgrRight.GetDrawingLayer(i)->ClearAll();
	}
}


LRESULT CSceneView::OnZoomChange(WPARAM wParam, LPARAM lParam)
{
	//CComboBox *pCtrl = m_ctrlBarHorz.GetComboBox(IDC_COMBO_ZOOM);
// 	if( pCtrl )
	{
		int idx = wParam;
		if( idx>=0 && idx<m_arrCanZoomRate.GetSize() )
		{
			int save = m_nInnerCmd;
			m_nInnerCmd = IDC_COMBO_ZOOM;
			OnPostInnerCmd();
			
			CRect rect;
			GetClientRect( &rect );
			
			ZoomChange( rect.CenterPoint(),m_arrCanZoomRate[idx]/m_fZoomRate );
			OnPostInnerCmd();
			m_nInnerCmd = save;
		}
	}
	return 0;
}


void CSceneView::OnInverseStereo()
{
	m_pContext->m_bInverseStereo = TRUE;
	Invalidate(FALSE);
}


void CSceneView::ReadAdjustParams()
{
	CWinApp *pApp = AfxGetApp();
	if( !pApp )return;
	
	CString strKey = "AdjustParams";
	CString strValue;
	
	{
		strValue = pApp->GetProfileString(m_strRegSection,strKey,_T(""));

		if( strValue.GetLength()>0 )
		{
			int params[9];
			if( _stscanf(strValue,_T("%d,%d,%d,%d,%d,%d,%d,%d,%d"),
				&params[0],&params[1],&params[2],
				&params[3],&params[4],&params[5],&params[6],&params[7],&params[8])==9 )
			{
				memcpy(m_nAdjustParams,params,sizeof(m_nAdjustParams));
				m_bRightSametoLeft = params[8];
			}
		}

		DWORD mask = 0;
		m_pContext->UpdateRGBMask(TRUE,FALSE,mask);
		m_pContext->UpdateRGBMask(FALSE,FALSE,mask);
	}	
}


void CSceneView::SaveAdjustParams()
{
	CWinApp *pApp = AfxGetApp();
	if( !pApp )return;
	
	DWORD mask1 = 0, mask2 = 0;
	m_pContext->UpdateRGBMask(TRUE,FALSE,mask1);
	m_pContext->UpdateRGBMask(FALSE,FALSE,mask2);	
	CString strKey =_T("AdjustParams");
	CString strValue;
	
	{
		strValue.Format(_T("%d,%d,%d,%d,%d,%d,%d,%d,%d"),
			m_nAdjustParams[0],m_nAdjustParams[1],m_nAdjustParams[2],
			m_nAdjustParams[3],m_nAdjustParams[4],m_nAdjustParams[5],
			mask1, mask2,m_bRightSametoLeft);
		
		pApp->WriteProfileString(m_strRegSection,strKey,strValue);
	}
}

void CSceneView::SetAdjustParams()
{
	CImageAdjust *pAdjL=NULL, *pAdjR=NULL;
	int i, num;
	
	for( i=0, num=m_laymgrLeft.GetDrawingLayerCount(); i<num; i++)
	{
		CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
		if( pLayer->IsKindOf(RUNTIME_CLASS(CRasterLayer)) )
		{
			pAdjL = &((CRasterLayer*)pLayer)->m_Adjust;
			
			*pAdjL = CImageAdjust();
			pAdjL->nBrightness = m_nAdjustParams[0];
			pAdjL->nContrast = m_nAdjustParams[1];
			pAdjL->bHistogram = m_nAdjustParams[2];
		}
	}
	
	if( m_bRightSametoLeft )
	{
		for( i=0, num=m_laymgrRight.GetDrawingLayerCount(); i<num; i++)
		{
			CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
			if( pLayer->IsKindOf(RUNTIME_CLASS(CRasterLayer)) )
			{
				pAdjR = &((CRasterLayer*)pLayer)->m_Adjust;
				
				*pAdjR = CImageAdjust();
				pAdjR->nBrightness = m_nAdjustParams[0];
				pAdjR->nContrast = m_nAdjustParams[1];
				pAdjR->bHistogram = m_nAdjustParams[2];
			}
		}
	}
	else
	{
		for( i=0, num=m_laymgrRight.GetDrawingLayerCount(); i<num; i++)
		{
			CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
			if( pLayer->IsKindOf(RUNTIME_CLASS(CRasterLayer)) )
			{
				pAdjR = &((CRasterLayer*)pLayer)->m_Adjust;
				
				*pAdjR = CImageAdjust();
				pAdjR->nBrightness = m_nAdjustParams[3];
				pAdjR->nContrast = m_nAdjustParams[4];
				pAdjR->bHistogram = m_nAdjustParams[5];
			}
		}
	}
}

void CSceneView::OnZoom()
{
	int num = m_arrCanZoomRate.GetSize();
	for( int i=num-1; i>=0; i--)
	{
		if( m_fZoomRate>=m_arrCanZoomRate[i] )break;
	}
	
	if( i<0 )i=0;

	CDlgSlider dlg;
	dlg.m_arrCanZoomRate.Copy(m_arrCanZoomRate);
	dlg.m_nPos = i;
	dlg.SetService(m_hWnd,WM_VIEW_ZOOMCHANGE,0,0);
	
	CRect rect;
	m_ctrlBarHorz.m_Static.GetWindowRect(&rect);	

	dlg.m_ShowPos = CPoint(rect.left,rect.top);

	dlg.DoModal();	

}

void CSceneView::OnAdjust()
{
	CSmartViewLocalRes setLocalRes;
	
	CDlgViewAdjust dlg;
	
	dlg.m_initb[0] = m_nAdjustParams[0];	
	dlg.m_initc[0] = m_nAdjustParams[1];
	dlg.m_initb[1] = m_nAdjustParams[3];
	dlg.m_initc[1] = m_nAdjustParams[4];
	
	dlg.m_brightness = m_nAdjustParams[0];
	dlg.m_contrast = m_nAdjustParams[1];
	dlg.m_brightness2 = m_nAdjustParams[3];
	dlg.m_contrast2 = m_nAdjustParams[4];
	
	dlg.m_bLAutoHistogram = m_nAdjustParams[2];
	dlg.m_bRAutoHistogram = m_nAdjustParams[5];
	
	dlg.m_bSameRight = m_bRightSametoLeft;

	if( m_pContext->GetDisplayMode()!=CStereoDrawingContext::modeShutterStereo )
	{
		DWORD mask = 0;
		dlg.m_bLRed = dlg.m_bLGreen = dlg.m_bLBlue = 0;
		dlg.m_bRRed = dlg.m_bRGreen = dlg.m_bRBlue = 0;
		m_pContext->UpdateRGBMask(TRUE,FALSE,mask);
		if( mask&RGBMASK_RED )dlg.m_bLRed = TRUE;
		if( mask&RGBMASK_GREEN )dlg.m_bLGreen = TRUE;
		if( mask&RGBMASK_BLUE )dlg.m_bLBlue = TRUE;

		mask = 0;
		m_pContext->UpdateRGBMask(FALSE,FALSE,mask);
		
		if( mask&RGBMASK_RED )dlg.m_bRRed = TRUE;
		if( mask&RGBMASK_GREEN )dlg.m_bRGreen = TRUE;
		if( mask&RGBMASK_BLUE )dlg.m_bRBlue = TRUE;

		dlg.m_bShowBand = TRUE;
	}
	
	if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSingle )
		dlg.m_bShowRight = FALSE;
	
	dlg.SetService(m_hWnd,WM_VIEW_ADJUSTUPDATE,0,(LPARAM)&dlg);

	int i, num;
	
	for( i=0, num=m_laymgrLeft.GetDrawingLayerCount(); i<num; i++)
	{
		CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
		if( pLayer->IsKindOf(RUNTIME_CLASS(CRasterLayer)) )
		{
			((CRasterLayer*)pLayer)->m_bPauseLoadAll = TRUE;
		}
	}
	
	for( i=0, num=m_laymgrRight.GetDrawingLayerCount(); i<num; i++)
	{
		CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
		if( pLayer->IsKindOf(RUNTIME_CLASS(CRasterLayer)) )
		{
			((CRasterLayer*)pLayer)->m_bPauseLoadAll = TRUE;
		}
	}
	
// 	int save = m_nInnerCmd;
// 	m_nInnerCmd = IDC_BUTTON_ADJUST;
	OnPreInnerCmd(IDC_BUTTON_ADJUST);
	dlg.DoModal();
	OnPostInnerCmd();
//	m_nInnerCmd = save;

	for( i=0, num=m_laymgrLeft.GetDrawingLayerCount(); i<num; i++)
	{
		CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
		if( pLayer->IsKindOf(RUNTIME_CLASS(CRasterLayer)) )
		{
			((CRasterLayer*)pLayer)->m_bPauseLoadAll = FALSE;
		}
	}
	
	for( i=0, num=m_laymgrRight.GetDrawingLayerCount(); i<num; i++)
	{
		CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
		if( pLayer->IsKindOf(RUNTIME_CLASS(CRasterLayer)) )
		{
			((CRasterLayer*)pLayer)->m_bPauseLoadAll = FALSE;
		}
	}

	Invalidate(FALSE);
	UpdateWindow_ForceDraw();
}


LRESULT CSceneView::OnAdjustUpdate(WPARAM wParam, LPARAM lParam)
{
	CDlgViewAdjust *pdlg = (CDlgViewAdjust*)lParam;
	if( pdlg )
	{
		m_nAdjustParams[0] = pdlg->m_initb[0];	
		m_nAdjustParams[1] = pdlg->m_initc[0];
		m_nAdjustParams[2] = pdlg->m_bLAutoHistogram;
		m_nAdjustParams[3] = pdlg->m_initb[1];
		m_nAdjustParams[4] = pdlg->m_initc[1];
		m_nAdjustParams[5] = pdlg->m_bRAutoHistogram;
		m_bRightSametoLeft = pdlg->m_bSameRight;

		SetAdjustParams();
		
		if( m_pContext->GetDisplayMode()!=CStereoDrawingContext::modeShutterStereo )
		{
			DWORD mask = 0;
			if( pdlg->m_bLRed )mask |= RGBMASK_RED;
			if( pdlg->m_bLGreen )mask |= RGBMASK_GREEN;
			if( pdlg->m_bLBlue )mask |= RGBMASK_BLUE;
			
			m_pContext->UpdateRGBMask(TRUE,TRUE,mask);
			
			mask = 0;
			if( pdlg->m_bRRed )mask |= RGBMASK_RED;
			if( pdlg->m_bRGreen )mask |= RGBMASK_GREEN;
			if( pdlg->m_bRBlue )mask |= RGBMASK_BLUE;
			
			m_pContext->UpdateRGBMask(FALSE,TRUE,mask);
		}
		
		SaveAdjustParams();
		RefreshDrawingLayers();
		
		Invalidate(FALSE);

		UpdateWindow_ForceDraw();
	}

	return 0;
}


void CSceneView::UpdateComboZoom()
{
	float fzoomrate = m_fZoomRate;
	int num = m_arrCanZoomRate.GetSize();
	for( int i=num-1; i>=0; i--)
	{
		if( fzoomrate>=m_arrCanZoomRate[i] )break;
	}
	
	if( i<0 )i=0;

	if (!m_bSliderZoom)
	{
		CComboBox *pCtrl = m_ctrlBarHorz.GetComboBox(IDC_COMBO_ZOOM);
		if( pCtrl )
		{
			pCtrl->SetCurSel(i);
		}
	}
	else
	{
		double fScale = m_arrCanZoomRate.GetAt(i);
		CString text;
		text.Format(_T("%.4f"), fScale);
		text.TrimRight('0');
		if (text.ReverseFind(_T('.')) == text.GetLength()-1)
		{
			text.Insert(text.GetLength(),_T('0'));
		}		
		
		m_ctrlBarHorz.m_Static.SetWindowText( text );

	}
	
}

void CSceneView::RecalcZoomRateList()
{	
	m_arrCanZoomRate.RemoveAll();

	m_arrCanZoomRate.Add(1);
	
	CSize szClient(80,60), szMap = CalcViewSize();
	if (szMap.cx == 0 || szMap.cy == 0)
	{
		return;
	}

	szMap.cx /= m_fZoomRate;
	szMap.cy /= m_fZoomRate;
		
	double zoom_x = double(szClient.cx)/szMap.cx;
	double zoom_y = double(szClient.cy)/szMap.cy;
	double zoom = (zoom_x<zoom_y?zoom_x:zoom_y);
	
	if( zoom<=0 )zoom = 1;
	if( zoom<1 )
	{
		int inverse = ceil(1.0/zoom);
		double zoomlist[] = { 0.75,0.5,0.375,0.25 };
		
		int nlist = sizeof(zoomlist)/sizeof(zoomlist[0]);
		
		for( int i=0; i<nlist; i++)
		{
			if( zoomlist[i]>=zoom )
				m_arrCanZoomRate.InsertAt(0,zoomlist[i]);
		}
		
		if( inverse>1/zoomlist[nlist-1] )
		{
			for( i=0; (1<<i)<=inverse; i++)
			{
				if( (1<<i)>1/zoomlist[nlist-1] )
				{
					m_arrCanZoomRate.InsertAt(0,1/(double)(1<<i));
				}
			}
			
			m_arrCanZoomRate.InsertAt(0,1/(double)(1<<i));
		}
	}

	float max00 = 100;
	if(IsSingle())
		max00 = 100000000;
		 
	float	min0 = -1024, max0 = max00;

	//有可能放大到滚动条超出整数范围，以及GDI+在绘制超大的数时不准确，所以对放大倍率作一个限制；
	if( szMap.cx>0 && szMap.cy>0 )
	{
		float	m1 = INT_MAX/szMap.cx;
		float	m2 = INT_MAX/szMap.cy;
		max00 = max0 = min(m1,m2);
		if( max0>max00 )max0 = max00;
	}	

	float	fmax;
	if(m_laymgrLeft.IsIncludeRasterLayer())
	{
		max0 = max00;
	}
	fmax = (float)max0;
	if( fmax<1 )fmax = 1;
	if (fmax > 100000000) fmax = 100000000;
	if( fmax>1 )
	{
		m_arrCanZoomRate.Add(1.25);
		m_arrCanZoomRate.Add(1.50);
		m_arrCanZoomRate.Add(1.75);
		m_arrCanZoomRate.Add(2.00);
		m_arrCanZoomRate.Add(2.50);
		
		for( int i=3; i<=8 && i<=fmax; i++ )m_arrCanZoomRate.Add(i);
		for( i=12; i<fmax; i*=1.5 )
		{
			i = (i+3)&(~3);
			m_arrCanZoomRate.Add(i);
		}
	}
	
	if (!m_bSliderZoom)
	{
		CComboBox *pCtrl = m_ctrlBarHorz.GetComboBox(IDC_COMBO_ZOOM);
		if( pCtrl )
		{
			pCtrl->ResetContent();
			
			float k = IsPolarized()?2.0f:1.0f;
			
			int num = m_arrCanZoomRate.GetSize();
			TCHAR str[256];
			for( int i=0; i<num; i++ )
			{
				_stprintf(str,_T("%.4f"),m_arrCanZoomRate[i]*k);
				pCtrl->AddString(str);
			}
		}
	}

}


int CSceneView::GetBestCacheTypeForLayer(CDrawingLayer *pLayer)
{
	if( pLayer==NULL )
	{
		if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeGL )
			return CHEDATA_TYPE_GLTEXT;
		else if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeD3D )
		{
			return CHEDATA_TYPE_D3DTEXT;
		}
		return CHEDATA_TYPE_BMP;
	}

	if( pLayer->IsKindOf(RUNTIME_CLASS(CVectorLayer)) )
	{
		if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeGL )
		{
			if( m_bTextureForVect )
				return CHEDATA_TYPE_GLTEXT;

			return CHEDATA_TYPE_GLLIST;
		}
		else if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeD3D )
		{
			return CHEDATA_TYPE_D3DTEXT;
		}
		return CHEDATA_TYPE_BMP;
	}
	else if( pLayer->IsKindOf(RUNTIME_CLASS(CRasterLayer)) )
	{
		if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeGL )
			return CHEDATA_TYPE_GLTEXT;
		else if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeD3D )
		{
			return CHEDATA_TYPE_D3DTEXT;
		}
		return CHEDATA_TYPE_BMP;
	}
	else if( pLayer->IsKindOf(RUNTIME_CLASS(CMutiRasterLayer)) )
	{
		if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeGL )
			return CHEDATA_TYPE_GLTEXT;
		else if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeD3D )
		{
			return CHEDATA_TYPE_D3DTEXT;
		}
		return CHEDATA_TYPE_BMP;
	}
	else if( pLayer->IsKindOf(RUNTIME_CLASS(CMutiRasterLayer)) )
	{
		if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeGL )
			return CHEDATA_TYPE_GLTEXT;
		else if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeD3D )
		{
			return CHEDATA_TYPE_D3DTEXT;
		}
		return CHEDATA_TYPE_BMP;
	}
	else
	{
		if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeGL )
			return CHEDATA_TYPE_GLTEXT;
		else if( m_pContext->m_nTextMode==CStereoDrawingContext::textureModeD3D )
		{
			return CHEDATA_TYPE_D3DTEXT;
		}
		return CHEDATA_TYPE_BMP;
	}
	return CHEDATA_TYPE_BMP;
}


CPoint CSceneView::GetScrollPos()
{
	return CPoint( int(m_ScrollCtrl.GetPos(SB_HORZ)-m_nViewMargin),
		int(m_ScrollCtrl.GetPos(SB_VERT)-m_nViewMargin) );
}


void CSceneView::UpdateDrawingLayers(CDrawingLayer *pAfterLayerL, CDrawingLayer *pAfterLayerR)
{
	if( !m_pContext->CanUpdateErasable() )
	{
		m_nUpdateIdxLeft = 0;
		m_nUpdateIdxRight = 0;
		return;
	}

	if( pAfterLayerL==NULL && pAfterLayerR==NULL )
	{
		m_nUpdateIdxLeft = 0;
		m_nUpdateIdxRight = 0;
		return;
	}
	
	int i, nFindIdxLeft = 0, nFindIdxRight = 0;
	BOOL bEraseLeft = TRUE, bEraseRight = TRUE;

	if( pAfterLayerL!=NULL )
	{
		for( i=m_laymgrLeft.GetDrawingLayerCount()-1; i>=nFindIdxLeft; i--)
		{
			CDrawingLayer *pLayer = m_laymgrLeft.GetDrawingLayer(i);
			if( !pLayer->CanErase() )
			{
				bEraseLeft = FALSE;
				break;
			}
			if( pLayer==pAfterLayerL )
				break;
		}
	}

	if( pAfterLayerR!=NULL && !IsSingle() )
	{
		for( i=m_laymgrRight.GetDrawingLayerCount()-1; i>=nFindIdxRight; i--)
		{
			CDrawingLayer *pLayer = m_laymgrRight.GetDrawingLayer(i);
			if( !pLayer->CanErase() )
			{
				bEraseRight = FALSE;
				break;
			}
			if( pLayer==pAfterLayerR )
				break;
		}
	}

	if( !bEraseLeft || !bEraseRight )
	{
		m_nUpdateIdxLeft = 0;
		m_nUpdateIdxRight = 0;
		return;
	}

	nFindIdxLeft = m_laymgrLeft.FindDrawingLayer(pAfterLayerL);	
	nFindIdxRight = m_laymgrRight.FindDrawingLayer(pAfterLayerR);

	if( m_nUpdateIdxLeft>=0 )
		m_nUpdateIdxLeft = min(nFindIdxLeft,m_nUpdateIdxLeft);
	else
		m_nUpdateIdxLeft = nFindIdxLeft;

	if( m_nUpdateIdxRight>=0 )
		m_nUpdateIdxRight = min(nFindIdxRight,m_nUpdateIdxRight);
	else
		m_nUpdateIdxRight = nFindIdxRight;
}


void CSceneView::FinishUpdateDrawing()
{
	if( m_nUpdateIdxLeft>=0 || m_nUpdateIdxRight>=0 )
	{
		if( m_laymgrLeft.GetDrawingLayer(m_nUpdateIdxLeft)->CanErase() && 
			( IsSingle() || ( !IsSingle() && m_laymgrRight.GetDrawingLayer(m_nUpdateIdxRight)->CanErase()) ) )
		{
			UpdateDrawingLayersByIndex(m_nUpdateIdxLeft,m_nUpdateIdxRight);
		}
		else
		{
			Invalidate(FALSE);
		}
	}
}


void CSceneView::UpdateDrawingLayersByIndex(int nIdxLeft, int nIdxRight)
{
	CRect rcClient, rcPaint;
	GetClientRect(&rcClient);
	
	CRect *pRcPaint = &rcClient;
	
	CStereoDrawingContext *pDC = (CStereoDrawingContext*)m_pContext;
	
	pDC->SetViewRect(*pRcPaint);
	pDC->BeginDrawingErasable();

	int i;

	//红绿和偏振立体下，左右片的绘制可能会相互影响，
	//必须按照如下方式绘图和更新，才能保证擦除的顺序和绘图的顺序精确相反；
	if( (IsRGStereo() || IsPolarized()) )
	{
		int start = max(m_laymgrLeft.GetDrawingLayerCount(),m_laymgrRight.GetDrawingLayerCount());
		int end = min(nIdxLeft,nIdxRight);

		for( i=start-1; i>=end; i--)
		{
			CDrawingLayer *pLayer1 = m_laymgrLeft.GetDrawingLayer(i);
			CDrawingLayer *pLayer2 = m_laymgrRight.GetDrawingLayer(i);

			if( pLayer2 )
			{
				pDC->SetDrawSide(CStereoDrawingContext::drawRight);
				pLayer2->Erase();
			}
			if( pLayer1 )
			{
				pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
				pLayer1->Erase();
			}
		}

		for( i=end; i<start; i++)
		{
			CDrawingLayer *pLayer1 = m_laymgrLeft.GetDrawingLayer(i);
			CDrawingLayer *pLayer2 = m_laymgrRight.GetDrawingLayer(i);

			if( pLayer1 )
			{
				pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
				pLayer1->Draw();
			}
			if( pLayer2 )
			{
				pDC->SetDrawSide(CStereoDrawingContext::drawRight);
				pLayer2->Draw();
			}
		}
	}
	//左右片分开绘制，则是为了让真立体更快速
	else
	{
		if( nIdxRight>=0 && !IsSingle() )
		{
			pDC->SetDrawSide(CStereoDrawingContext::drawRight);
			
			for( i=m_laymgrRight.GetDrawingLayerCount()-1; i>=nIdxRight; i--)
			{
				m_laymgrRight.GetDrawingLayer(i)->Erase();
			}
		}
		
		if( nIdxLeft>=0 )
		{
			pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
			
			for( i=m_laymgrLeft.GetDrawingLayerCount()-1; i>=nIdxLeft; i--)
			{
				m_laymgrLeft.GetDrawingLayer(i)->Erase();
			}
			
		}
		
		if( nIdxLeft>=0 )
		{
			pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
			
			for( i=nIdxLeft; i<m_laymgrLeft.GetDrawingLayerCount(); i++)
			{
				m_laymgrLeft.GetDrawingLayer(i)->Draw();
			}
		}
		
		if( nIdxRight>=0 && !IsSingle() )
		{
			pDC->SetDrawSide(CStereoDrawingContext::drawRight);
			
			for( i=nIdxRight; i<m_laymgrRight.GetDrawingLayerCount(); i++)
			{
				m_laymgrRight.GetDrawingLayer(i)->Draw();
			}
		}
	}
	
	pDC->EndDrawingErasable();

	m_nUpdateIdxLeft = -1;
	m_nUpdateIdxRight = -1;
}


void CSceneView::OnViewUndo()
{
	if( m_nCurViewPos>0 )
	{
		m_nCurViewPos--;
		ActiveViewPos(m_arrPViewPos.GetAt(m_nCurViewPos));
	}
}


void CSceneView::OnViewRedo()
{
	if( m_nCurViewPos<m_arrPViewPos.GetSize()-1 )
	{
		m_nCurViewPos++;
		ActiveViewPos(m_arrPViewPos.GetAt(m_nCurViewPos));
	}
}


void CSceneView::PushViewPos()
{
	ViewPos *pItem = new ViewPos;
	if( !pItem )return;

	for( int i=m_nCurViewPos+1; i<m_arrPViewPos.GetSize(); i++ )
	{
		ViewPos *p = m_arrPViewPos.GetAt(i);
		delete p;
	}

	m_arrPViewPos.RemoveAt(m_nCurViewPos+1,m_arrPViewPos.GetSize()-m_nCurViewPos-1);
	
	m_pLeftCS->GetMatrix(pItem->matrix_left);
	m_pRightCS->GetMatrix(pItem->matrix_right);
	pItem->zoom = m_fZoomRate;

	m_arrPViewPos.Add(pItem);

	if( m_arrPViewPos.GetSize()>50 )
	{
		for( int i=50; i<m_arrPViewPos.GetSize(); i++ )
		{
			ViewPos *p = m_arrPViewPos.GetAt(i);
			delete p;
		}
		m_arrPViewPos.RemoveAt(50,m_arrPViewPos.GetSize()-50);
	}

	m_nCurViewPos = m_arrPViewPos.GetSize()-1;

	m_fViewPosDx = 0;
	m_fViewPosDy = 0;
	m_fViewPosDp = 0;
	m_fViewPosDq = 0;
}


void CSceneView::ActiveViewPos(ViewPos* pos)
{
	m_pLeftCS->Create44Matrix(pos->matrix_left);
	m_pRightCS->Create44Matrix(pos->matrix_right);

	m_laymgrLeft.OnChangeCoordSys(FALSE);
	m_laymgrRight.OnChangeCoordSys(FALSE);
	OnChangeCoordSys(FALSE);
	
	CDrawingContext *pDC = m_pContext->GetLeftContext();
	if( pDC )pDC->SetCoordSys(m_pLeftCS);
	
	pDC = m_pContext->GetRightContext();
	if( pDC )pDC->SetCoordSys(m_pRightCS);

	m_fZoomRate = pos->zoom;

	RecalcScrollBar(FALSE);
	Invalidate(FALSE);
}


void CSceneView::OnChangeCoordSys(BOOL bJustScroll)
{

}

LRESULT CSceneView::ClearData(WPARAM wp, LPARAM lp)
{
	if( m_pDummyDialog )
	{
		delete m_pDummyDialog;
		m_pDummyDialog = NULL;
	}
	
	m_timer.CloseTimer();

	return 1;
}


void CSceneView::SetButtonState(int id, int add, int del)
{
	m_ctrlBarHorz.SetButtonState(id, add, del);
}


UINT CSceneView::GetButtonState(int id)
{
	return m_ctrlBarHorz.GetButtonState(id);
}


void CSceneView::AddButton(int id, HBITMAP hBmp, LPCTSTR tips)
{
	m_ctrlBarHorz.AddButtons((UINT*)&id,&hBmp,1);
	CMFCButton *pBtn = (CMFCButton*)m_ctrlBarHorz.GetButton(id);
	if( pBtn )
	{
		pBtn->SetTooltip(tips);
	}
}

void CSceneView::ShowOrHideScrollBar(BOOL bShow)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	m_bIsSelfSizing = TRUE;

	if (bShow)
	{
		::MoveWindow(m_ctrlBarHorz.GetSafeHwnd(), rcClient.left, rcClient.top + rcClient.Height() - m_ctrlBarHorz.GetButtonsHeight(),
			rcClient.Width(), m_ctrlBarHorz.GetButtonsHeight(), TRUE);

		::MoveWindow(m_ctrlBarVert.GetSafeHwnd(), rcClient.left + rcClient.Width() - m_ctrlBarVert.GetButtonsWidth(), rcClient.top,
			m_ctrlBarVert.GetButtonsWidth(), rcClient.Height() - m_ctrlBarHorz.GetButtonsHeight(), TRUE);
	}
	else
	{
		::MoveWindow(m_ctrlBarHorz.GetSafeHwnd(), 0, 0, 0, 0, TRUE);
		::MoveWindow(m_ctrlBarVert.GetSafeHwnd(), 0, 0, 0, 0, TRUE);
	}

	m_ctrlBarHorz.AdjustLayout();
	m_ctrlBarVert.AdjustLayout();

	m_bIsSelfSizing = FALSE;
}

MyNameSpaceEnd