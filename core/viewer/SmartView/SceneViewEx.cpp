// SceneViewEx.cpp: implementation of the CSceneViewEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "glew.h"
#include "SceneViewEx.h"
#include "resource.h"
#include "DlgCursorSetting.h"

#include "SmartViewLocalRes.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern AFX_EXTENSION_MODULE SmartViewDLL;


MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSceneViewEx, CSceneView)

BEGIN_MESSAGE_MAP(CSceneViewEx, CSceneView)
	//{{AFX_MSG_MAP(CSceneViewEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(IDC_BUTTON_HANDMOVE,OnHandMove)
	ON_COMMAND(IDC_BUTTON_CURSOR,OnSettingCursor)
	ON_COMMAND(IDC_BUTTON_ZOOMRECT,OnZoomRect)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_HANDMOVE,OnUpdateHandMove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSceneViewEx::CSceneViewEx()
{
	m_hSaveCursor   = NULL;
	m_bUseSaveCursor= FALSE;
	
	_tcscpy(m_strRegSection,_T("Config\\View"));

	m_bHardCross = FALSE;
	m_bSysCross = FALSE;
}

CSceneViewEx::~CSceneViewEx()
{

}


void CSceneViewEx::OnMove(int x, int y) 
{
	CSceneView::OnMove(x, y);
}

void CSceneViewEx::OnSize(UINT nType, int cx, int cy) 
{
	CSceneView::OnSize(nType, cx, cy);
}

void CSceneViewEx::OnMoving(UINT fwSide, LPRECT pRect) 
{
	CSceneView::OnMoving(fwSide, pRect);
}


void CSceneViewEx::OnHandMove()
{
	if( m_nInnerCmd==IDC_BUTTON_HANDMOVE )
	{
		OnPostInnerCmd();
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_HANDMOVE,TRUE,FALSE);
	}
	else
	{
		OnPreInnerCmd(IDC_BUTTON_HANDMOVE);		
		m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_HANDMOVE,TRUE,TRUE);
	}

}

void CSceneViewEx::OnUpdateHandMove(CCmdUI *pCmdUI)
{
	if( m_nInnerCmd==IDC_BUTTON_HANDMOVE )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}


static BOOL g_bSaveCrossVisible = FALSE;
void CSceneViewEx::OnMButtonDown(UINT nFlags, CPoint point)
{
	if( m_nInnerCmd!=IDC_BUTTON_HANDMOVE )
	{		
		ClearCross();
		HCURSOR hCursor = ::LoadCursor(SmartViewDLL.hModule,MAKEINTRESOURCE(IDC_HAND));
		if( hCursor )
		{			
			m_bUseSaveCursor = TRUE;
			m_hSaveCursor = ::SetCursor(hCursor);			
		}

		g_bSaveCrossVisible = IsCrossVisible();
		if (g_bSaveCrossVisible)
		{
			EnableCrossVisible(FALSE);
		}
		
		OnHandMove();
		m_ptLBtnDn = point;
	}
	CSceneView::OnMButtonDown(nFlags, point);
}

void CSceneViewEx::OnMButtonUp(UINT nFlags, CPoint point)
{
	if( m_nInnerCmd==IDC_BUTTON_HANDMOVE )
	{
		OnHandMove();
		ResetCursorDrawing();
		if (g_bSaveCrossVisible)
		{
			EnableCrossVisible(TRUE);
		}
		
		if( m_bUseSaveCursor && m_hSaveCursor )
			SetCursor(m_hSaveCursor);
		m_bUseSaveCursor = FALSE;
		m_hSaveCursor = NULL;
// 		if(!IsSingle())
// 			m_cursorR.Reset();
// 		m_cursorL.Reset();
	}
	CSceneView::OnMButtonUp(nFlags, point);
}


BOOL CSceneViewEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CRect rcClient;
	GetClientRect(&rcClient);
	
 	CRect rcShow(rcClient.left,
		rcClient.top,rcClient.right,rcClient.bottom);
	if(IsSplit())
		rcShow = m_pContext->GetSplitViewRect(CStereoDrawingContext::drawLeft);
// 	CRect rcShow(rcClient.left,
// 		rcClient.top,rcClient.left+m_pContext->GetSplitViewRect(CStereoDrawingContext::drawLeft),rcClient.Height());
	
	CPoint ptCursor;
	::GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	if( m_nInnerCmd!=0 )
	{
//		EnableCrossVisible(FALSE);
	}
	if(m_nInnerCmd==0)
	{
		EnableCrossVisible(TRUE);	
		
		{		
			if( IsSplit() && !rcShow.PtInRect(ptCursor) )
			{
				HCURSOR hCursor = ::LoadCursor(NULL,IDC_ARROW);
				if( hCursor )
				{
					ClearCross();
					EnableCrossVisible(FALSE);
					m_bUseSaveCursor = TRUE;
					m_hSaveCursor = ::SetCursor(hCursor);					
				}
				return TRUE;
			}			
		}					
	}
	switch( nHitTest )
	{
		case HTCLIENT:
			switch( m_nInnerCmd )
			{
				case IDC_BUTTON_HANDMOVE:	
					{
						HCURSOR hCursor = ::LoadCursor(SmartViewDLL.hModule,MAKEINTRESOURCE(IDC_HAND));
						if( hCursor )
						{
							EnableCrossVisible(TRUE);
							ClearCross();
							EnableCrossVisible(FALSE);
							m_bUseSaveCursor = TRUE;
							m_hSaveCursor = ::SetCursor(hCursor);						
						}
						return TRUE;
					}
					break;
				case IDC_BUTTON_ZOOMRECT:
					{
						HCURSOR hCursor = ::LoadCursor(SmartViewDLL.hModule,MAKEINTRESOURCE(IDC_ZOOMRECT));
						if( hCursor )
						{
							EnableCrossVisible(TRUE);
							ClearCross();
							EnableCrossVisible(FALSE);
							m_bUseSaveCursor = TRUE;
							m_hSaveCursor = ::SetCursor(hCursor);
						}
						return TRUE;
					}
					break;
				default:
					{
						if( m_bSysCross )
						{
							m_cursorL.DrawSysCursor();
							EnableCrossVisible(FALSE);

							return TRUE;
						}

						EnableCrossVisible(TRUE);

						if( m_pContext->GetDisplayMode()==CStereoDrawingContext::modeSplit )
						{
							rcShow = m_pContext->GetSplitViewRect(CStereoDrawingContext::drawLeft);
						}

						if( rcShow.PtInRect(ptCursor) )
						{
							::SetCursor(NULL);
							return TRUE;
						}

						HCURSOR hCursor = ::LoadCursor(NULL,IDC_ARROW);
						if( hCursor )
						{
							m_bUseSaveCursor = TRUE;
							m_hSaveCursor = ::SetCursor(hCursor);

							EnableCrossVisible(FALSE);
						}
						return TRUE;
					}

			}
		default:;
	}	
	
	return CSceneView::OnSetCursor(pWnd, nHitTest, message);
}

void CSceneViewEx::OnInitialUpdate()
{
	CSceneView::OnInitialUpdate();
	COLORREF color = RGB(255,255,255);
	color = AfxGetApp()->GetProfileInt(m_strRegSection,_T("CursorColor"),color);
	SetCrossColor(color);
	
	int index = 0;
	index = AfxGetApp()->GetProfileInt(m_strRegSection,_T("CursorType"),index);
	SetCrossType(index);
}

void CSceneViewEx::OnSettingCursor()
{
	CSmartViewLocalRes setLocalRes;
	
	CDlgCursorSetting dlg;
	dlg.SetCursor(&m_cursorL);
	m_nInnerCmd = IDC_BUTTON_CURSOR;
	dlg.DoModal();
	m_nInnerCmd = 0;
	SetCrossType(dlg.m_nCursorIdx);
	SetCrossColor(dlg.m_wndColorBtn.GetColor());

	m_cursorL.CreateSysCursor();
	m_cursorR.CreateSysCursor();

	OnRefresh();
}


void CSceneViewEx::OnZoomRect()
{
	CRect rect;
	GetClientRect(&rect);

	OnPreInnerCmd(IDC_BUTTON_ZOOMRECT);
}


void CSceneViewEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	switch( m_nInnerCmd )
	{
	case IDC_BUTTON_HANDMOVE:
		m_ptLBtnDn = point;
		break;
	case IDC_BUTTON_ZOOMRECT:
		m_rcZoomDrag.TopLeft() = point;
		m_rcZoomDrag.BottomRight() = point;
		break;
	}
	
	CSceneView::OnLButtonDown(nFlags, point);
}


void CSceneViewEx::OnLButtonUp(UINT nFlags, CPoint point) 
{
	switch( m_nInnerCmd )
	{
	case IDC_BUTTON_ZOOMRECT:
		{
			CRect rcclt, rcold;
			rcold = m_rcZoomDrag;
			m_rcZoomDrag.BottomRight() = point;

			GetClientRect(&rcclt);
			rcold.NormalizeRect();
			rcold &= rcclt;
			
			CClientDC dc(this);
			dc.DrawDragRect(CRect(0,0,0,0), CSize(1,1),rcold, CSize(1, 1));
			if( m_rcZoomDrag.Width()!=0 && m_rcZoomDrag.Height()!=0 )
			{
				float zoomx = rcclt.Width()/fabs((float)m_rcZoomDrag.Width());
				float zoomy = rcclt.Height()/fabs((float)m_rcZoomDrag.Height());
				float zoom = zoomx>zoomy?zoomx:zoomy;

				ZoomChange(m_rcZoomDrag.CenterPoint(),zoom);
//				OnPostInnerCmd();
// 				m_nInnerCmd = 0;
// 				if( m_bUseSaveCursor && m_hSaveCursor )
// 					SetCursor(m_hSaveCursor);
// 				m_bUseSaveCursor = FALSE;
// 				m_hSaveCursor = NULL;
			}
		}
		break;
	}
	
	CView::OnLButtonUp(nFlags, point);
}

void CSceneViewEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	switch( m_nInnerCmd )
	{
	case IDC_BUTTON_HANDMOVE:
		if ( nFlags&MK_LBUTTON || nFlags&MK_MBUTTON )
		{
			Scroll( point.x-m_ptLBtnDn.x,point.y-m_ptLBtnDn.y,0,0,0 );				
			m_ptLBtnDn = point;
			break;
		}
		break;
	case IDC_BUTTON_ZOOMRECT:
		if ( nFlags&MK_LBUTTON )
		{
			{
				CRect rcclt, rcdraw, rcold;
				rcold = m_rcZoomDrag;
				m_rcZoomDrag.BottomRight() = point;
				rcdraw = m_rcZoomDrag;

				GetClientRect(&rcclt);
				rcold.NormalizeRect();
				rcdraw.NormalizeRect();

				rcold &= rcclt;
				rcdraw &= rcclt;
				
				CClientDC dc(this);
				dc.DrawDragRect(rcdraw, CSize(1,1),rcold, CSize(1, 1));
			}

			break;
		}
		break;
	}
	CSceneView::OnMouseMove(nFlags, point);
}


void CSceneViewEx::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( m_nInnerCmd )
	{
		if( m_nInnerCmd==IDC_BUTTON_HANDMOVE )
		{
			m_ctrlBarHorz.SetButtonState2(IDC_BUTTON_HANDMOVE,TRUE,FALSE);
		}
		OnPostInnerCmd();		
	}
	
	CSceneView::OnRButtonDown(nFlags, point);
}


void CSceneViewEx::DrawCrossOverlay()
{
	if( m_bHardCross && m_pContext->GetDisplayMode()==CStereoDrawingContext::modeShutterStereo )
	{
		CRect rcClient, rcPaint, *pRcPaint;
		GetClientRect(&rcClient);
		
		pRcPaint = &rcClient;
		
		CStereoDrawingContext *pDC = (CStereoDrawingContext*)m_pContext;
		
		{
			pDC->SetViewRect(*pRcPaint);
			//»æÖÆÒ»°ãÍ¼²ã
			pDC->BeginDrawingOverlay();
			
			{
				pDC->SetDrawSide(CStereoDrawingContext::drawLeft);								
				pDC->EraseBackground();
				
				m_cursorL.Draw();
			}
			
			{
				pDC->SetDrawSide(CStereoDrawingContext::drawRight);
				pDC->EraseBackground();
				
				m_cursorR.Draw();
			}
			
			pDC->EndDrawingOverlay();
		}
	}
}


void CSceneViewEx::DoPaint(const CRect *pRcPaint, int whichbuf, BOOL bIncludeNotErasable, BOOL bIncludeErasable)
{
	if( IsCrossVisible() && ((m_bHardCross && m_pContext->GetDisplayMode()==CStereoDrawingContext::modeShutterStereo) ||
		m_bSysCross) )
	{
		EnableCrossVisible(FALSE);
	}

	CSceneView::DoPaint(pRcPaint,whichbuf,bIncludeNotErasable,bIncludeErasable);

	if( IsCrossVisible() && ((m_bHardCross && m_pContext->GetDisplayMode()==CStereoDrawingContext::modeShutterStereo) ||
		m_bSysCross) )
	{
		EnableCrossVisible(TRUE);
	}
}

void CSceneViewEx::OnPaint()
{
	CSceneView::OnPaint();
}


void CSceneViewEx::OnDestroy()
{
	m_cursorL.Destroy();
	m_cursorR.Destroy();
	
	CSceneView::OnDestroy();
}


int CSceneViewEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSceneView::OnCreate(lpCreateStruct) == -1)
		return -1;

	TCHAR shapeName[_MAX_PATH] = {0};

	// get execute directory
	GetModuleFileName(SmartViewDLL.hModule,shapeName,_MAX_FNAME);
	TCHAR *pos = _tcsrchr(shapeName,_T('\\'));
	if( pos )pos[1] = _T('\0');
	
	_tcscat( shapeName,_T("CrossShape.dat") );

	m_cursorL.Load(ConvertTCharToChar(shapeName));
	m_cursorR.Load(ConvertTCharToChar(shapeName));

	m_cursorL.SetContext(m_pContext);
	m_cursorR.SetContext(m_pContext);

	m_laymgrLeft.InsertDrawingLayer(&m_cursorL);
	m_laymgrRight.InsertDrawingLayer(&m_cursorR);

	return 0;
	
FAIL:	
	return -1;
}


void CSceneViewEx::SetCrossPos(float x,float y, float xr)
{
	m_cursorL.SetPos(x,y);
	m_cursorR.SetPos(xr,y);
}


void CSceneViewEx::GetCrossPos(float *x,float *y, float *xr)
{
	if( x )*x = m_cursorL.GetPos(0);
	if( y )*y = m_cursorL.GetPos(1);
	if( xr )*xr = m_cursorR.GetPos(0);
}


int	 CSceneViewEx::SetCrossType(int index)
{
	int old = m_cursorL.GetCursorType();
	m_cursorL.SetCursorType(index);
	m_cursorR.SetCursorType(index);
	AfxGetApp()->WriteProfileInt(m_strRegSection,_T("CursorType"),index);
	return old;
}

COLORREF CSceneViewEx::SetCrossColor(COLORREF color)
{
	COLORREF old = m_cursorL.GetCursorColor();
	m_cursorL.SetCursorColor(color);
	m_cursorR.SetCursorColor(color);
	AfxGetApp()->WriteProfileInt(m_strRegSection, _T("CursorColor"), color);
	return old;
}

void CSceneViewEx::SetCrossSelMarkWid(int wid)
{
	m_cursorL.SetSelMarkWid(wid);
	m_cursorR.SetSelMarkWid(wid);
}

void CSceneViewEx::ResetCursorDrawing()
{
	m_cursorL.Reset();
	m_cursorR.Reset();
}


COLORREF CSceneViewEx::GetCrossColor()
{
	return m_cursorL.GetCursorColor();
}

int CSceneViewEx::GetCrossSelMarkWid()
{
	return m_cursorL.GetSelMarkWid();
}

int CSceneViewEx::GetCrossType()
{
	return m_cursorL.GetCursorType();
}

void CSceneViewEx::EnableCrossVisible(BOOL bEnable)
{
	if( bEnable )
	{	
		m_cursorL.ModifyAttribute(CDrawingLayer::attrVisible,0);
		m_cursorR.ModifyAttribute(CDrawingLayer::attrVisible,0);
	}
	else
	{
		m_cursorL.ModifyAttribute(0,CDrawingLayer::attrVisible);
		m_cursorR.ModifyAttribute(0,CDrawingLayer::attrVisible);		
	}
	
}


void CSceneViewEx::EnableCrossSelMarkVisible(BOOL bEnable)
{
	m_cursorL.EnableSelMarkVisible(bEnable);
	m_cursorR.EnableSelMarkVisible(bEnable);
}

void CSceneViewEx::EnableCrossCrossVisible(BOOL bEnable)
{
	m_cursorL.EnableCrossVisible(bEnable);
	m_cursorR.EnableCrossVisible(bEnable);
}

BOOL CSceneViewEx::IsCrossVisible()
{
	return ((m_cursorL.GetAttribute()&CDrawingLayer::attrVisible)!=0);
}

BOOL CSceneViewEx::IsCrossSelMarkVisible()
{
	return m_cursorL.IsSelMarkVisible();
}

BOOL CSceneViewEx::IsCrossCrossVisible()
{
	return m_cursorL.IsCrossVisible();
}


void CSceneViewEx::UpdateCross()
{
	if( m_bSysCross )
		return;

	if( m_bHardCross && m_pContext->GetDisplayMode()==CStereoDrawingContext::modeShutterStereo )
	{
		DrawCrossOverlay();
		return;
	}

	UpdateDrawingLayers(&m_cursorL,&m_cursorR);
}

void CSceneViewEx::ClearCross()
{	
	CRect rcClient;
	GetClientRect(&rcClient);
	
//	CRect *pRcPaint = &rcClient;
	
	CStereoDrawingContext *pDC = (CStereoDrawingContext*)m_pContext;
	
	pDC->SetViewRect(rcClient);
	pDC->BeginDrawingErasable();	
	
	if( !IsSingle() )
	{
		pDC->SetDrawSide(CStereoDrawingContext::drawRight);	
		m_cursorR.Erase();	
	}	

	pDC->SetDrawSide(CStereoDrawingContext::drawLeft);
	m_cursorL.Erase();	
	
	pDC->EndDrawingErasable();
	ResetCursorDrawing();
}


void CSceneViewEx::EnableHardCross(BOOL bEnable)
{
	if( m_pContext->GetOverlayRCContext()==NULL )
		return;

	m_bHardCross = bEnable;

	m_cursorL.m_bUseSaveBmp = !m_bHardCross;
	m_cursorR.m_bUseSaveBmp = !m_bHardCross;
}


void CSceneViewEx::EnableSysCross(BOOL bEnable)
{
	m_bSysCross = bEnable;

	m_cursorL.CreateSysCursor();
}


MyNameSpaceEnd