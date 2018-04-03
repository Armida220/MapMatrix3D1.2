// DrawGraphStatic.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DrawGraphStatic.h"
#include "VisualModelMapCtrl.h "
#include "SmartViewFunctions.h"
#include "ExMessage.h "
#include "RegDef.h "
#include "RegDef2.h "
#include "StereoFrame.h "
#include "StereoView.h "
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DWORD_PTR gdwInnerCmdData[2];

extern CStereoView *GetStereoView(int flag);

/////////////////////////////////////////////////////////////////////////////
// CDrawGraphStatic

CDrawGraphStatic::CDrawGraphStatic()
{
	m_rectButton.SetRectEmpty();
	m_arrModelScopes.RemoveAll();
	m_nRatioForZoom=2;
	m_bIsDrag=false;
	m_bIsFirstClickInDrawArea=false;
	m_bIsDragDisplay=false;
	m_bIsDragForMouse=false;
	m_lfMsOrgLogicX=0;
	m_lfMsOrgLogicY=0;
	m_lfMsOrgWorldX=0;
	m_lfMsOrgWorldY=0;
	m_bIsOldPtInMS=false;
	m_msOldMSForHighlight.p1.x=0;
	m_msOldMSForHighlight.p1.y=0;
	m_msOldMSForHighlight.p2.x=0;
	m_msOldMSForHighlight.p2.y=0;
	m_msOldMSForHighlight.p3.x=0;
	m_msOldMSForHighlight.p3.y=0;
	m_msOldMSForHighlight.p4.x=0;
	m_msOldMSForHighlight.p4.y=0;
	m_bIsExistForVecWin=false;
	m_bIsExistForStereoWin=false;
	m_bIsExistForMSArr=false;
	m_bIsExistForCurMS=false;
 	m_bIsAutoSwitch=(AfxGetApp()->GetProfileInt("Config","StereoAutoSwitch",FALSE)!=0);
	m_bIsExistForMsNearest=false;

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

CDrawGraphStatic::~CDrawGraphStatic()
{
}


BEGIN_MESSAGE_MAP(CDrawGraphStatic, CStatic)
	//{{AFX_MSG_MAP(CDrawGraphStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_POPUP_OPENPHOTO, OnPopupOpenPhotoMs)
	ON_COMMAND(ID_POPUP_OPENEPIP, OnPopupOpenEpipMs)
	ON_COMMAND(ID_POPUP_SWITCHEPIP, OnPopupSwitchEpip)
	ON_COMMAND(ID_POPUP_OPENREALTIMEEPIP, OnPopupRealtimeEpip)
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
	ON_COMMAND(ID_POPUP_RECALC_MODEL_BOUND, OnReCalcModelBound)
	ON_UPDATE_COMMAND_UI(ID_POPUP_OPENEPIP, OnUpdateOpenEpip)
	ON_UPDATE_COMMAND_UI(ID_POPUP_OPENPHOTO, OnUpdateOpenPhoto)
	ON_WM_ERASEBKGND()

	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawGraphStatic message handlers

BOOL CDrawGraphStatic::Create(LPCTSTR lpszText, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL ret = CStatic::Create(lpszText,dwStyle,rect,pParentWnd,nID);
	EnableToolTips(TRUE);

	return ret;
}


INT_PTR CDrawGraphStatic::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
{
	int mode = PtInButton(point);
	if (mode < 0) return CWnd::OnToolHitTest(point,pTI);

	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)mode;
	pTI->lpszText = LPSTR_TEXTCALLBACK;
	pTI->rect = GetButtonRect(mode);
	
	return pTI->uId;
}

BOOL CDrawGraphStatic::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	UINT nID = pNMHDR->idFrom;
	
    if (pNMHDR->code == TTN_NEEDTEXT)
	{
		if(pTTT->uFlags&TTF_IDISHWND)
		{			
		}		
		else
		{
			CString strText;
			int mode = pNMHDR->idFrom;
			if (mode >= 0)
			{
				switch(mode)
				{
				case modeZoomOut:
					strText = StrFromResID(IDS_TIP_ZOOMOUT);
					break;
				case modeZoomIn:
					strText = StrFromResID(IDS_TIP_ZOOMIN);
					break;
				case modeDrag:
					strText = StrFromResID(IDS_TIP_DRAG);
					break;
				case modeDragDisplay:
					strText = StrFromResID(IDS_TIP_DRAGDISPLAY);
					break;
				case modeWholeScreen:
					strText = StrFromResID(IDS_TIP_WHOLESCREEN);
					break;
				case modeSwitch:
					strText = StrFromResID(IDS_TIP_SWITCH);
					break;
				case modeAutoSwitch:
					strText = StrFromResID(IDS_TIP_AUTOSWITCH);
					break;
				default:
					break;
				}
				
				pTTT->lpszText = (LPTSTR)(LPCTSTR)strText;
				return TRUE;
			}
			
		}
	}

	*pResult = 0;	
	
	return FALSE;    // message was handled
}

void CDrawGraphStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	//填充背景
 	CRect rect;
	GetClientRect(&rect);

	//帖按钮
	CDC MemDC0, MemDC1;
	MemDC0.CreateCompatibleDC(&dc);
	MemDC1.CreateCompatibleDC(&dc);
	
	CBitmap bmp2;
	bmp2.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	CBitmap* pOldBitmap1 = MemDC1.SelectObject(&bmp2);
	MemDC1.FillSolidRect(&rect,RGB(50,50,50));

	CBitmap bm;
	if (!m_bIsAutoSwitch)
		bm.LoadBitmap(IDB_BUTTON_MAPINFO);
	else
		bm.LoadBitmap(IDB_BUTTON_MAPINFO1);
	BITMAP bit;
	bm.GetBitmap(&bit);

	CBitmap* pOldBitmap0 = MemDC0.SelectObject(&bm);
	MemDC1.BitBlt(m_rectButton.left, m_rectButton.top, m_rectButton.Width(), m_rectButton.Height(), &MemDC0, 0, 0, NOTSRCCOPY);
	MemDC0.SelectObject(pOldBitmap0);

	CDC &MemDC = MemDC1;

	//限定画图范围
	CRgn rgn;
	CRect rect1=m_rectDrawArea;
	rect1.InflateRect(1,1,0,1);
	rgn.CreateRectRgnIndirect(&rect1);
	MemDC.SelectClipRgn(&rgn);

	int i;
	if(m_bIsExistForMSArr)
	{
		for (i=0;i<m_arrModelScopes.GetSize();i++)
		{
			CPen newPen(PS_SOLID,0,RGB(180,   180,   180));
			CPen *pOldPen = MemDC.SelectObject(&newPen);
			ModelScope ms = m_arrModelScopes.GetAt(i);
			GeoToDev(&ms);
			DrawScope(&MemDC,ms);
			MemDC.SelectObject(pOldPen);
		}
	}
	if (m_bIsExistForVecWin)
	{
		CPen newPen(PS_SOLID,0,RGB(0,   0,  255));
		CPen *pOldPen = MemDC.SelectObject(&newPen);
		ModelScope ms = m_msCurrentVectorScope;
		GeoToDev(&ms);
		DrawScope(&MemDC,ms);
		MemDC.SelectObject(pOldPen);

	}
	if (m_bIsExistForCurMS)
	{
		CPen newPen(PS_SOLID,0,RGB(255,   0,  0));
		CPen *pOldPen = MemDC.SelectObject(&newPen);
		ModelScope ms = m_msCurrentModel;
		GeoToDev(&ms);
		DrawScope(&MemDC,ms);
		MemDC.SelectObject(pOldPen);
	}
	if(m_bIsExistForMsNearest&&m_bIsExistForCurMS)
	{
		CPen newPen(PS_SOLID,0,RGB(0,   255,  0));
		CPen *pOldPen = MemDC.SelectObject(&newPen);
		ModelScope ms = m_msNearestModel;
		GeoToDev(&ms);
		DrawScope(&MemDC,ms);
		MemDC.SelectObject(pOldPen);

	}
	if (m_bIsExistForStereoWin)
	{
		CPen newPen(PS_SOLID,0,RGB(255,   0,  0));
		CBrush newBrush(RGB(255,0,0));
		int old = MemDC.SetROP2(R2_XORPEN);
		CPen *pOldPen = MemDC.SelectObject(&newPen);
		CBrush* pOldBrush = MemDC.SelectObject(&newBrush);

		ModelScope ms = m_msCurrentStereoWindow;
		GeoToDev(&ms);
		CPoint pts[4];
		pts[0] = CPoint(ms.p1.x,ms.p1.y);
		pts[1] = CPoint(ms.p2.x,ms.p2.y);
		pts[2] = CPoint(ms.p3.x,ms.p3.y);
		pts[3] = CPoint(ms.p4.x,ms.p4.y);
		MemDC.Polygon(pts, 4);
		MemDC.SelectObject(pOldPen);
		MemDC.SelectObject(pOldBrush);
		MemDC.SetROP2(old);
	}

	dc.BitBlt(rect.left,rect.top,rect.Width(),rect.Height(),&MemDC,0,0,SRCCOPY );

	MemDC.SelectObject(pOldBitmap1);
	MemDC.DeleteDC();
	dc.SelectClipRgn(NULL);

}

void CDrawGraphStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	int mode=PtInButton(point);
	switch(mode)
	{
	case modeZoomOut:	
		m_bIsDrag=false;
		m_bIsDragDisplay=false;
		m_bIsDragForMouse=false;
		ZoomOut();
		Invalidate();
		::SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_HAND) );
		break;
	case modeZoomIn:	
		m_bIsDrag=false;
		m_bIsDragDisplay=false;
		m_bIsDragForMouse=false;
		ZoomIn();
		Invalidate();
		::SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_HAND) );
		break;
	case modeDrag:
		m_bIsDrag=true;
		m_bIsDragDisplay=false;
		m_bIsDragForMouse=false;
		::SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_HAND) );
		break;
	case modeDragDisplay:
					
		m_bIsDragDisplay=true;
		m_bIsDragForMouse=false;
		m_bIsDrag=false;
		::SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_HAND) );
		break;
	case modeWholeScreen:
		m_bIsDragDisplay=false;
		m_bIsDragForMouse=false;
		m_bIsDrag=false;
		ShowWholeMap();
		Invalidate();
		::SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_HAND) );
		break;
	case modeSwitch:
		SwitchModel();
		::SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_HAND) );
		break;
	case modeAutoSwitch:
		m_bIsDragDisplay=false;
		m_bIsDragForMouse=false;
		m_bIsDrag=false;
		m_bIsAutoSwitch=!m_bIsAutoSwitch;
		AfxGetApp()->WriteProfileInt("Config","StereoAutoSwitch",m_bIsAutoSwitch?1:0);
		Invalidate();
		::SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_HAND) );
		break;
	default:
		;
	}
	if (m_bIsDrag)
	{
		if(m_rectDrawArea.PtInRect(point))
		{
			m_ptStartForDrag=point;
			m_bIsFirstClickInDrawArea=true;
		}
		else
		{
			m_bIsFirstClickInDrawArea=false;
		}
			
	}
	if (m_bIsDragDisplay)
	{
		if(m_rectDrawArea.PtInRect(point))
		{
			CRect rect(m_rectDrawArea);
	     	ClientToScreen(&rect);
			ClipCursor(&rect);
			m_ptStartForDrag=point;
			m_bIsFirstClickInDrawArea=true;
		}
		else
		{
			m_bIsFirstClickInDrawArea=false;
		}
		
	}
	
	CStatic::OnLButtonDown(nFlags, point);
}



void CDrawGraphStatic::AddModelScope(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4,int index)
{
	m_arrModelScopes.Add(ModelScope(p1,p2,p3,p4,index));
}

int CDrawGraphStatic::PtInButton(CPoint pt) const
{
	if (pt.y<=m_rectButton.top+1||pt.y>m_rectButton.bottom-2)
	{
		return -1;
	}
	if (pt.x>m_rectButton.left&&pt.x<=m_rectButton.left+13)
	{
		return modeZoomOut;

	}
	if(pt.x>m_rectButton.left+25&&pt.x<=m_rectButton.left+38)
	{
		return modeZoomIn;
	}
	if (pt.x>m_rectButton.left+50&&pt.x<=m_rectButton.left+63)
	{
		return modeDrag;

	}
	if (pt.x>m_rectButton.left+75&&pt.x<=m_rectButton.left+88)
	{
		return modeDragDisplay;
	}
	if (pt.x>m_rectButton.left+100&&pt.x<m_rectButton.left+113)
	{
		return modeWholeScreen;
	}
	if (pt.x>m_rectButton.left+125&&pt.x<m_rectButton.left+138)
	{
		return modeSwitch;
	}
	if(pt.x>m_rectButton.left+150&&pt.x<m_rectButton.left+163)
	{
		return modeAutoSwitch;
	}

	return -1;

}

CRect CDrawGraphStatic::GetButtonRect(int mode) const
{
	CRect rect = m_rectButton;

	switch(mode)
	{
	case modeZoomOut:
		rect.right = m_rectButton.left + 13;
		break;
	case modeZoomIn:
		rect.left = m_rectButton.left + 25;
		rect.right = m_rectButton.left + 38;
		break;
	case modeDrag:
		rect.left = m_rectButton.left + 50;
		rect.right = m_rectButton.left + 63;
		break;
	case modeDragDisplay:
		rect.left = m_rectButton.left + 75;
		rect.right = m_rectButton.left + 88;
		break;
	case modeWholeScreen:
		rect.left = m_rectButton.left + 100;
		rect.right = m_rectButton.left + 112;
		break;
	case modeSwitch:
		rect.left = m_rectButton.left + 125;
		rect.right = m_rectButton.left + 138;
		break;
	case modeAutoSwitch:
		rect.left = m_rectButton.left + 150;
		rect.right = m_rectButton.left + 163;
		break;
	default:
		rect.right = m_rectButton.left;
		break;
	}

	return rect;

}

void CDrawGraphStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int mode=PtInButton(point);
	switch(mode)
	{
	case modeZoomOut:
		
	case modeZoomIn:
	
	case modeDrag:
	
	case modeDragDisplay:
	
	case modeWholeScreen:
		
	case modeSwitch:
		
	case modeAutoSwitch:
		::SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_HAND) );/*AfxGetApp()->LoadCursor();*/
		break;
	default:
		;
	}
	BOOL bLButtonDown = (::GetKeyState(VK_LBUTTON)<0);
	if (!bLButtonDown&&m_rectDrawArea.PtInRect(point)&&m_bIsExistForMSArr)
	{
		ModelScope ms;
		if (GetNearestMsToPt(&ms,point))
		{
			if (!m_bIsOldPtInMS)
			{
				m_bIsOldPtInMS=true;
				m_msOldMSForHighlight=ms;
				CDC  *pDC=GetDC();
				
				//限定画图范围
				CRgn rgn;
				CRect rect1=m_rectDrawArea;
				rect1.InflateRect(1,1,0,1);
				rgn.CreateRectRgnIndirect(&rect1);
				pDC->SelectClipRgn(&rgn);

				CPen newPen(PS_SOLID ,0,RGB(0,0,0));
				CPen *pOldPen=NULL;
				pOldPen=pDC->SelectObject(&newPen);
				
				((CVisualModelMapCtrl*)GetParent())->m_editTipInfo.SetWindowText(m_coreObj.stereo[ms.index].sp.stereoID);
			
				GeoToDev(&ms);
				DrawScope(pDC,ms);
				pDC->SelectObject(pOldPen);
				pDC->SelectClipRgn(NULL);
				ReleaseDC(pDC);
			}
			else
			{
				if (!IsEqualForMS(ms,m_msOldMSForHighlight))
				{
					m_msOldMSForHighlight=ms;
					Invalidate();
					UpdateWindow();
					CDC  *pDC=GetDC();

					//限定画图范围
					CRgn rgn;
					CRect rect1=m_rectDrawArea;
					rect1.InflateRect(1,1,0,1);
					rgn.CreateRectRgnIndirect(&rect1);
					pDC->SelectClipRgn(&rgn);

					CPen newPen(PS_SOLID ,0,RGB(0,0,0));
					CPen *pOldPen=NULL;
					pOldPen=pDC->SelectObject(&newPen);

					((CVisualModelMapCtrl*)GetParent())->m_editTipInfo.SetWindowText(m_coreObj.stereo[ms.index].sp.stereoID);
					GeoToDev(&ms);
					DrawScope(pDC,ms);
					pDC->SelectObject(pOldPen);
					pDC->SelectClipRgn(NULL);
					ReleaseDC(pDC);
				}

			}
			
		}
		else
		{
			if(!m_bIsExistForCurMS)
				((CVisualModelMapCtrl*)GetParent())->m_editTipInfo.SetWindowText("");
			else
			{		
				CString str;
				if(GetNameForNearestMS(&str))
				{
					CString str1;
					str1.Format(IDS_NEARSTMS,str);
					((CVisualModelMapCtrl*)GetParent())->m_editTipInfo.SetWindowText(str1);
					
				}
				else
				{
					str.Format(IDS_NONEARSTMS);		
					((CVisualModelMapCtrl*)GetParent())->m_editTipInfo.SetWindowText(str);
					
				}

			}
			if (m_bIsOldPtInMS)
			{
				m_bIsOldPtInMS=false;
				Invalidate();
				UpdateWindow();
			}
			
		}


	}
	if(bLButtonDown&&m_bIsDrag&&m_bIsFirstClickInDrawArea/*&&m_rectDrawArea.PtInRect(point)*/)
	{
		CSize size(point.x-m_ptStartForDrag.x,point.y-m_ptStartForDrag.y);
		m_ptStartForDrag.x=point.x;
		m_ptStartForDrag.y=point.y;

		m_lfMsOrgLogicX+=size.cx;
		m_lfMsOrgLogicY+=size.cy;


		Invalidate();
	}
	if(bLButtonDown&&m_bIsDragDisplay&&m_bIsFirstClickInDrawArea/*&&m_rectDrawArea.PtInRect(point)*/)
	{
		CPoint tempPt;
		CDC *pDC=GetDC();

		pDC->SetROP2(R2_NOT);
		pDC->SelectStockObject(WHITE_PEN);
		if(m_bIsDragForMouse)
		{
			pDC->MoveTo(m_ptStartForDrag);
			tempPt.x=m_ptEnd.x;
			tempPt.y=m_ptStartForDrag.y;
			pDC->LineTo(tempPt);
			pDC->LineTo(m_ptEnd);
			tempPt.x=m_ptStartForDrag.x;
			tempPt.y=m_ptEnd.y;
			pDC->LineTo(tempPt);
			pDC->LineTo(m_ptStartForDrag);
		}
		else
			m_bIsDragForMouse=true;

		pDC->MoveTo(m_ptStartForDrag);
		tempPt.x=point.x;
		tempPt.y=m_ptStartForDrag.y;
		pDC->LineTo(tempPt);
		pDC->LineTo(point);
		tempPt.x=m_ptStartForDrag.x;
		tempPt.y=point.y;
		pDC->LineTo(tempPt);
		pDC->LineTo(m_ptStartForDrag);
		m_ptEnd=point;
		ReleaseDC(pDC);
	}

	CStatic::OnMouseMove(nFlags, point);
}



void CDrawGraphStatic::SetDrawArea(const CRect &rect)
{
	m_rectDrawArea = rect;
}

void CDrawGraphStatic::SetButtonRect(const CRect &rect)
{
	m_rectButton=rect;
}
void CDrawGraphStatic::ZoomOut(/*ModelScope *ms*/)
{
	double centerX=(m_rectDrawArea.left+m_rectDrawArea.right)/2;
	double centerY=(m_rectDrawArea.top+m_rectDrawArea.bottom)/2;
	m_lfMsOrgLogicX=(m_lfMsOrgLogicX-centerX)*2+centerX;
	m_lfMsOrgLogicY=(m_lfMsOrgLogicY-centerY)*2+centerY;

	m_lfRatioForCoorTrans /= 2;

}

void CDrawGraphStatic::ZoomIn(/*ModelScope *ms*/)
{
	//中心点
	double centerX=(m_rectDrawArea.left+m_rectDrawArea.right)/2;
	double centerY=(m_rectDrawArea.top+m_rectDrawArea.bottom)/2;
	m_lfMsOrgLogicX=(m_lfMsOrgLogicX-centerX)/2+centerX;
	m_lfMsOrgLogicY=(m_lfMsOrgLogicY-centerY)/2+centerY;

	m_lfRatioForCoorTrans *= 2;
}


void CDrawGraphStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int mode=PtInButton(point);
	switch(mode)
	{
	case modeZoomOut:
	case modeZoomIn:
	case modeDrag:
	case modeDragDisplay:
	case modeWholeScreen:
	case modeSwitch:
	case modeAutoSwitch:
		::SetCursor( AfxGetApp()->LoadCursor(IDC_CURSOR_HAND) );/*AfxGetApp()->LoadCursor();*/
		break;
	default:
		;
	}
	if (m_bIsDragDisplay&&m_rectDrawArea.PtInRect(point))
	{
		m_bIsDragForMouse=false;
		ClipCursor(NULL);

		//显示图形变换
				//先平移
		CPoint centerPt,centerPt1;
		centerPt.x=(m_ptStartForDrag.x+point.x)/2;
		centerPt.y=(m_ptStartForDrag.y+point.y)/2;
		centerPt1=m_rectDrawArea.CenterPoint();
		CSize size=centerPt1-centerPt;
		m_lfMsOrgLogicX+=size.cx;
		m_lfMsOrgLogicY+=size.cy;
				//再放大
		double proportion;
		double xproportion=double(abs(m_ptStartForDrag.x-point.x))/m_rectDrawArea.Width();
		double yproportion=double(abs(m_ptStartForDrag.y-point.y))/m_rectDrawArea.Height();
		if (xproportion<yproportion)
		{
			proportion=yproportion;
		}
		else
			proportion=xproportion;
		
		m_lfMsOrgLogicX = (m_lfMsOrgLogicX-centerPt1.x)/proportion+centerPt1.x;
		m_lfMsOrgLogicY = (m_lfMsOrgLogicY-centerPt1.y)/proportion+centerPt1.y;
		
		m_lfRatioForCoorTrans *= proportion;
		Invalidate();
	}
	
	CStatic::OnLButtonUp(nFlags, point);
}

bool CDrawGraphStatic::GetRect(double *Xmin, double *Ymin, double *Xmax, double *Ymax)
{
	ASSERT(Xmin!=NULL);
	ASSERT(Ymin!=NULL);
	ASSERT(Xmax!=NULL);
	ASSERT(Ymax!=NULL);
	double minX,minY,maxX,maxY;
	bool pb;
	int i=0;
	pb=0;	

	for (i=0;i<m_arrModelScopes.GetSize();i++)
	{	
		pb=1;
		(m_arrModelScopes.GetAt(i)).GetRect(&minX,&minY,&maxX,&maxY);
		if(*Xmin>minX)	*Xmin=minX;
		if(*Xmax<maxX)	*Xmax=maxX;
		if(*Ymin>minY)	*Ymin=minY;
		if(*Ymax<maxY)	*Ymax=maxY;
	}
// 	if (m_bIsExistForMsNearest)
// 	{
// 		pb=1;
// 		m_msNearestModel.GetRect(&minX,&minY,&maxX,&maxY);
// 		if(*Xmin>minX)	*Xmin=minX;
// 		if(*Xmax<maxX)	*Xmax=maxX;
// 		if(*Ymin>minY)	*Ymin=minY;
// 		if(*Ymax<maxY)	*Ymax=maxY;
// 	}
// 	if (m_bIsExistForCurMS)
// 	{
// 		pb=1;
// 		m_msCurrentModel.GetRect(&minX,&minY,&maxX,&maxY);
// 		if(*Xmin>minX)	*Xmin=minX;
// 		if(*Xmax<maxX)	*Xmax=maxX;
// 		if(*Ymin>minY)	*Ymin=minY;
// 		if(*Ymax<maxY)	*Ymax=maxY;
// 	}
	if (m_bIsExistForStereoWin)
	{
		pb=1;
		m_msCurrentStereoWindow.GetRect(&minX,&minY,&maxX,&maxY);
		if(*Xmin>minX)	*Xmin=minX;
		if(*Xmax<maxX)	*Xmax=maxX;
		if(*Ymin>minY)	*Ymin=minY;
		if(*Ymax<maxY)	*Ymax=maxY;
	}
	if (m_bIsExistForVecWin)
	{
		pb=1;
		m_msCurrentVectorScope.GetRect(&minX,&minY,&maxX,&maxY);
		if(*Xmin>minX)	*Xmin=minX;
		if(*Xmax<maxX)	*Xmax=maxX;
		if(*Ymin>minY)	*Ymin=minY;
		if(*Ymax<maxY)	*Ymax=maxY;
	}	
	return pb;
}

void CDrawGraphStatic::ShowWholeMap()
{
	double minx,miny,maxx,maxy,hratio,vratio;
	
	//捕捉光标，并使光标成沙漏等待光标
	//把最小值给非常大，把最大值给非常小，为了对变量进行初试化
	minx=INT_MAX;miny=INT_MAX;maxx=INT_MIN;maxy=INT_MIN;
	SetCapture();
	SetCursor(LoadCursor(NULL,IDC_WAIT));

	BOOL bSaveExistVecWin = m_bIsExistForVecWin;
	m_bIsExistForVecWin = FALSE;
	
	BOOL pb=GetRect(&minx,&miny,&maxx,&maxy);
	
	m_bIsExistForVecWin = bSaveExistVecWin;

	//将光标变为箭头形，放弃捕捉的光标
    SetCursor(LoadCursor(NULL,IDC_ARROW));
    ReleaseCapture();
	if(!pb)		//没有图形元素,程序返回
		return;
	hratio=(maxx-minx)/m_rectDrawArea.Width();//横向的比例
	vratio=(maxy-miny)/m_rectDrawArea.Height();//纵向比例
	if(hratio>vratio)
		m_lfRatioForCoorTrans = hratio;  //取较大的一个作为比例尺
	else
		m_lfRatioForCoorTrans = vratio;
	
	//设置世界坐标的原点
	m_lfMsOrgWorldX = minx;
	m_lfMsOrgWorldY = maxy;

	//设置设备坐标的原点
	m_lfMsOrgLogicX = m_rectDrawArea.CenterPoint().x-(maxx-minx)/(2*m_lfRatioForCoorTrans);
	m_lfMsOrgLogicY = m_rectDrawArea.CenterPoint().y-(maxy-miny)/(2*m_lfRatioForCoorTrans);


}

void CDrawGraphStatic::SetNearestMS(PT_2D p1, PT_2D p2, PT_2D p3, PT_2D p4)
{
	m_msNearestModel.p1=p1;
	m_msNearestModel.p2=p2;
	m_msNearestModel.p3=p3;
	m_msNearestModel.p4=p4;


}

void CDrawGraphStatic::SetCurrentMS(PT_2D p1, PT_2D p2, PT_2D p3, PT_2D p4)
{
	m_msCurrentModel.p1=p1;
	m_msCurrentModel.p2=p2;
	m_msCurrentModel.p3=p3;
	m_msCurrentModel.p4=p4;
	m_bIsExistForCurMS=true;

}

void CDrawGraphStatic::SetCurrentStereoWin(PT_2D p1, PT_2D p2, PT_2D p3, PT_2D p4)
{
	m_msCurrentStereoWindow.p1=p1;
	m_msCurrentStereoWindow.p2=p2;
	m_msCurrentStereoWindow.p3=p3;
	m_msCurrentStereoWindow.p4=p4;
	m_bIsExistForStereoWin=true;

}

void CDrawGraphStatic::SetCurrentVecWin(PT_2D p1, PT_2D p2, PT_2D p3, PT_2D p4)
{
	m_msCurrentVectorScope.p1=p1;
	m_msCurrentVectorScope.p2=p2;
	m_msCurrentVectorScope.p3=p3;
	m_msCurrentVectorScope.p4=p4;
	m_bIsExistForVecWin=true;
}

void CDrawGraphStatic::GeoToDev(ModelScope *ms)
{
	(ms->p1).x=((ms->p1).x-m_lfMsOrgWorldX)/m_lfRatioForCoorTrans+m_lfMsOrgLogicX;
	(ms->p1).y=-((ms->p1).y-m_lfMsOrgWorldY)/m_lfRatioForCoorTrans+m_lfMsOrgLogicY;
	(ms->p2).x=((ms->p2).x-m_lfMsOrgWorldX)/m_lfRatioForCoorTrans+m_lfMsOrgLogicX;
	(ms->p2).y=-((ms->p2).y-m_lfMsOrgWorldY)/m_lfRatioForCoorTrans+m_lfMsOrgLogicY;
	(ms->p3).x=((ms->p3).x-m_lfMsOrgWorldX)/m_lfRatioForCoorTrans+m_lfMsOrgLogicX;
	(ms->p3).y=-((ms->p3).y-m_lfMsOrgWorldY)/m_lfRatioForCoorTrans+m_lfMsOrgLogicY;
	(ms->p4).x=((ms->p4).x-m_lfMsOrgWorldX)/m_lfRatioForCoorTrans+m_lfMsOrgLogicX;
	(ms->p4).y=-((ms->p4).y-m_lfMsOrgWorldY)/m_lfRatioForCoorTrans+m_lfMsOrgLogicY;
}

void CDrawGraphStatic::GeoToDev(PT_2D *pt)
{
	pt->x=(pt->x-m_lfMsOrgWorldX)/m_lfRatioForCoorTrans+m_lfMsOrgLogicX;
	pt->y=-(pt->y-m_lfMsOrgWorldY)/m_lfRatioForCoorTrans+m_lfMsOrgLogicY;
}


void CDrawGraphStatic::DrawScope(CDC *pDC, const ModelScope &ms)
{

	pDC->MoveTo(CPoint(ms.p1.x,ms.p1.y));

	pDC->LineTo(CPoint(ms.p2.x,ms.p2.y));

	pDC->LineTo(CPoint(ms.p3.x,ms.p3.y));

	pDC->LineTo(CPoint(ms.p4.x,ms.p4.y));

	pDC->LineTo(CPoint(ms.p1.x,ms.p1.y));	

}

void CDrawGraphStatic::SetLogicOrg(double x, double y)
{
	m_lfMsOrgLogicX=x;
	m_lfMsOrgLogicY=y;
}

void CDrawGraphStatic::SetWorldOrg(double x, double y)
{
	m_lfMsOrgWorldX=x;
	m_lfMsOrgWorldY=y;
}

void CDrawGraphStatic::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnLButtonDown(nFlags,point);
	OnLButtonDown(nFlags,point);
	
	CStatic::OnLButtonDblClk(nFlags, point);
}

bool CDrawGraphStatic::GetNearestMsToPt(ModelScope *ms,CPoint point)
{
	ModelScope modelscope;
	double dis=-1,tem;
	PT_3D pt,pts[4];
	pt.x=point.x;
	pt.y=point.y;
	bool bOK = false;

	//先通过核心区找最近模型：从包含了当前点的核心区中找面积最小的，如果没有找到，则按照以前的方式处理
	double minArea = -1;
	for (int i=0;i<m_arrModelScopes.GetSize();i++)
	{
		ModelScope modelscope1 = m_arrModelScopes_core.GetAt(i);
		GeoToDev(&modelscope1);
		COPY_2DPT((pts[0]),modelscope1.p1);
		COPY_2DPT((pts[1]),modelscope1.p2);
		COPY_2DPT((pts[2]),modelscope1.p3);
		COPY_2DPT((pts[3]),modelscope1.p4);
		if (GraphAPI::GIsPtInRegion(pt,pts,4)==2)
		{
			tem = GraphAPI::GGetTriangleArea(pts[0].x,pts[0].y,pts[1].x,pts[1].y,pts[2].x,pts[2].y) + 
				GraphAPI::GGetTriangleArea(pts[0].x,pts[0].y,pts[3].x,pts[3].y,pts[2].x,pts[2].y);

			if(minArea<0 || tem<minArea)
			{
				minArea=tem;
				modelscope=m_arrModelScopes.GetAt(i);
			}
			bOK=true;
		}
	}

	if(bOK)
	{
		*ms=modelscope;
		return true;
	}

	for (i=0;i<m_arrModelScopes.GetSize();i++)
	{
		ModelScope modelscope1 = m_arrModelScopes.GetAt(i);
		GeoToDev(&modelscope1);
		COPY_2DPT((pts[0]),modelscope1.p1);
		COPY_2DPT((pts[1]),modelscope1.p2);
		COPY_2DPT((pts[2]),modelscope1.p3);
		COPY_2DPT((pts[3]),modelscope1.p4);
		if (GraphAPI::GIsPtInRegion(pt,pts,4)==2)
		{
			tem = (pt.x-(pts[0].x+pts[1].x+pts[2].x+pts[3].x)/4)*(pt.x-(pts[0].x+pts[1].x+pts[2].x+pts[3].x)/4)
				+(pt.y-(pts[0].y+pts[1].y+pts[2].y+pts[3].y)/4)*(pt.y-(pts[0].y+pts[1].y+pts[2].y+pts[3].y)/4);
			if(dis<0 || tem<dis)
			{
				dis=tem;
				modelscope=m_arrModelScopes.GetAt(i);
			}
			bOK=true;
		}
	}
	*ms=modelscope;
	return bOK;
}

bool CDrawGraphStatic::IsEqualForMS(const ModelScope &ms, const ModelScope &ms1)
{
	return (fabs(ms.p1.x-ms1.p1.x)<1e-6&&fabs(ms.p1.y-ms1.p1.y)<1e-6
	&&fabs(ms.p2.x-ms1.p2.x)<1e-6&&fabs(ms.p2.y-ms1.p2.y)<1e-6
	&&fabs(ms.p3.x-ms1.p3.x)<1e-6&&fabs(ms.p3.y-ms1.p3.y)<1e-6
	&&fabs(ms.p4.x-ms1.p4.x)<1e-6&&fabs(ms.p4.y-ms1.p4.y)<1e-6);
}

void CDrawGraphStatic::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//记录下右键点击时鼠标所在的模型范围
	if (!GetNearestMsToPt(&m_msNearestMSForRightClick,point))
		return;
	//弹出菜单
	ClientToScreen(&point);
	CMenu   menu,*pSubMenu;
	menu.LoadMenu(IDR_POPUP_OPENSTEREO);                //添加菜单资源
	pSubMenu=menu.GetSubMenu(0);           //设置菜单弹出项目起始位置
 //   pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);

	CMenu* pMruMenu = menu.GetSubMenu(0);
	m_CMruFile.SetMenuPtr(pMruMenu);//设置menu对象
	m_CMruFile.ReadMru();

	
	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
		
		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pMruMenu->m_hMenu, FALSE, TRUE))
			return;
		
		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu (pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	ScreenToClient(&point);
	
	CStatic::OnRButtonDown(nFlags, point);
}

void CDrawGraphStatic::OnPopupOpenPhotoMs() //原始影像
{
	int index=-1;
	ModelScope ms;
	for (int i=0;i<m_arrModelScopes.GetSize();i++)
	{
		ms=m_arrModelScopes.GetAt(i);
		if (IsEqualForMS(m_msNearestMSForRightClick,ms))
		{
			index=(ms.index|0x80000000);
			break;
		}
		
	}
	ASSERT(index>=0);
	gdwInnerCmdData[0] = index;
	gdwInnerCmdData[1] = (LPARAM)&m_coreObj;
	int stereidx = index&0x0fffffff;
	CString strIndex,strStereID;
	strIndex.Format("%d",index);
	strStereID = m_coreObj.stereo[stereidx].sp.stereoID;
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

void CDrawGraphStatic::OnPopupOpenEpipMs() //核线像对
{
	int index=-1;
	ModelScope ms;
	for (int i=0;i<m_arrModelScopes.GetSize();i++)
	{
		ms=m_arrModelScopes.GetAt(i);
		if (IsEqualForMS(m_msNearestMSForRightClick,ms))
		{
			index=ms.index;
			break;
		}

	}


	ASSERT(index>=0);
	gdwInnerCmdData[0] = index;
	gdwInnerCmdData[1] = (LPARAM)&m_coreObj;
	int stereidx = index&0x0fffffff;
	CString strIndex,strStereID;
	strIndex.Format("%d",index);
	strStereID = m_coreObj.stereo[stereidx].sp.stereoID;
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



void CDrawGraphStatic::OnPopupRealtimeEpip()//实时核线
{
	int index=-1;
	ModelScope ms;
	for (int i=0;i<m_arrModelScopes.GetSize();i++)
	{
		ms=m_arrModelScopes.GetAt(i);
		if (IsEqualForMS(m_msNearestMSForRightClick,ms))
		{
			index=ms.index;
			break;
		}
		
	}
	ASSERT(index>=0);
	gdwInnerCmdData[0] = (index|0x40000000);
	gdwInnerCmdData[1] = (LPARAM)&m_coreObj;
	
	int stereidx = (index|0x40000000)&0x0fffffff;
	CString strIndex,strStereID;
	strIndex.Format("%d",gdwInnerCmdData[0]);
	strStereID = m_coreObj.stereo[stereidx].sp.stereoID;
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

void CDrawGraphStatic::OnPopupSwitchEpip() 
{
	int index=-1;
	ModelScope ms;
	for (int i=0;i<m_arrModelScopes.GetSize();i++)
	{
		ms=m_arrModelScopes.GetAt(i);
		if (IsEqualForMS(m_msNearestMSForRightClick,ms))
		{
			index=ms.index;
			break;
		}
		
	}
	ASSERT(index>=0);
	gdwInnerCmdData[0] = (gdwInnerCmdData[0]&(~0xfffffff))|index;
	gdwInnerCmdData[1] = (LPARAM)&m_coreObj;
	//by shy
	CView *pView=NULL;
	pView = GetStereoView(0);

	if(!pView) return;
	//HWND hwnd=::GetFocus();
	::SendMessage(pView->m_hWnd,WM_STEREOVIEW_LOADMDL,
		gdwInnerCmdData[0],gdwInnerCmdData[1]);	
	//::SetFocus(hwnd);
	pView->SetFocus();

	((CStereoView*)pView)->SwitchModelForRefresh();
	m_bIsExistForMsNearest=false;
	m_msCurrentModel=m_msNearestMSForRightClick;
	
}

void CDrawGraphStatic::SetCoreObj( CoreObject *pCoreObj)
{
	m_coreObj=*pCoreObj;
}

void CDrawGraphStatic::SetCurMSFlag(bool bExist)
{
    m_bIsExistForCurMS=bExist;
}

void CDrawGraphStatic::SetNearstMSFlag(bool bIsExist)
{
	m_bIsExistForMsNearest=bIsExist;	
}

void CDrawGraphStatic::SetCurStereoWinFlag(bool bIsExist)
{
	m_bIsExistForStereoWin=bIsExist;
}

void CDrawGraphStatic::SetVecWinFlag(bool bIsExist)
{
	m_bIsExistForVecWin=bIsExist;
}
void CDrawGraphStatic::SetMsArrayFlag(bool bIsExist)
{
	m_bIsExistForMSArr=bIsExist;
}

void CDrawGraphStatic::OnOpenDocimage(int index)
{
	ASSERT(index>=0);

	int stereidx ;
	CString steroindex=	m_CMruFile.m_nIndex[index];
	CString strpathname = m_CMruFile.m_PathName[index];
	stereidx = atoi(steroindex);
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
		if (nTindex>=m_coreObj.iStereoNum)
		{
			m_CMruFile.DelMru(strpathname,index);
			return;
		}
		CString strStereID = m_coreObj.stereo[nTindex].sp.stereoID;
		if (0 != strStereID.Compare(strstereoid))
		{
			m_CMruFile.DelMru(strpathname,index);
			return;
		}
	}
	else if(0 == stropenType.Compare(StrFromResID(IDS_REALTIMEEPIP)))//实时核线
	{
		int nTindex = 	(stereidx|0x40000000)&0x0fffffff;
		if (nTindex>=m_coreObj.iStereoNum)
		{
			m_CMruFile.DelMru(strpathname,index);
			return;
		}
		CString strStereID = m_coreObj.stereo[nTindex].sp.stereoID;
		if (0 != strStereID.Compare(strstereoid))
		{
			m_CMruFile.DelMru(strpathname,index);
			return;
		}
	}

	gdwInnerCmdData[0] = stereidx;
	gdwInnerCmdData[1] = (LPARAM)&m_coreObj;

	m_CMruFile.AddMru(m_CMruFile.m_PathName[index],steroindex);	

	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_COMMAND,
		FCCMD_LOADSTEREO,0);

	CStereoView *pView = GetStereoView(0);
	if( pView )
		pView->SwitchModelForRefresh();	
}

void CDrawGraphStatic::OnOpenDocimage1()
{
	OnOpenDocimage(0);
}

void CDrawGraphStatic::OnOpenDocimage2()
{
	OnOpenDocimage(1);
}

void CDrawGraphStatic::OnOpenDocimage3()
{
	OnOpenDocimage(2);
}

void CDrawGraphStatic::OnOpenDocimage4()
{
	OnOpenDocimage(3);
}

void CDrawGraphStatic::OnOpenDocimage5()
{
	OnOpenDocimage(4);
}

void CDrawGraphStatic::OnOpenDocimage6()
{
	OnOpenDocimage(5);
}

void CDrawGraphStatic::OnOpenDocimage7()
{
	OnOpenDocimage(6);
}

void CDrawGraphStatic::OnOpenDocimage8()
{
	OnOpenDocimage(7);
}

void CDrawGraphStatic::OnOpenDocimage9()
{
	OnOpenDocimage(8);
}

void CDrawGraphStatic::OnOpenDocimage10()
{
	OnOpenDocimage(9);
}

void CDrawGraphStatic::OnOpenDocimage11()
{
	OnOpenDocimage(10);
}

void CDrawGraphStatic::OnOpenDocimage12()
{
	OnOpenDocimage(11);
}

void CDrawGraphStatic::OnOpenDocimage13()
{
	OnOpenDocimage(12);
}

void CDrawGraphStatic::OnOpenDocimage14()
{
	OnOpenDocimage(13);
}

void CDrawGraphStatic::OnOpenDocimage15()
{
	OnOpenDocimage(14);
}

void CDrawGraphStatic::OnOpenDocimage16()
{
	OnOpenDocimage(15);
}

void CDrawGraphStatic::OnMruClr()
{
	m_CMruFile.ClearMru();
}


void CDrawGraphStatic::CalcCoreModelScope()
{
	m_arrModelScopes_core.Copy(m_arrModelScopes);
	for(int i=0; i<m_arrModelScopes_core.GetSize(); i++)
	{
		ModelScope item = m_arrModelScopes_core[i];

		//计算核心区域
		double k = 0.5;
		PT_2D center;
		PT_2D pts[4];
		COPY_2DPT((pts[0]),item.p1);
		COPY_2DPT((pts[1]),item.p2);
		COPY_2DPT((pts[2]),item.p3);
		COPY_2DPT((pts[3]),item.p4);

		center.x = (pts[0].x+pts[1].x+pts[2].x+pts[3].x)/4;
		center.y = (pts[0].y+pts[1].y+pts[2].y+pts[3].y)/4;

		for(int j=0; j<4; j++)
		{
			pts[j].x = pts[j].x *(1-k) + center.x*k;
			pts[j].y = pts[j].y *(1-k) + center.y*k;
		}

		COPY_2DPT(item.p1,(pts[0]));
		COPY_2DPT(item.p2,(pts[1]));
		COPY_2DPT(item.p3,(pts[2]));
		COPY_2DPT(item.p4,(pts[3]));

		m_arrModelScopes_core[i] = item;
	}
}

CoreObject CDrawGraphStatic::getCoreObj()
{
	return m_coreObj;
}


//如果到距边界1/8.则居中；如果自动切换未打开，但存在临近模型，则临近模型高亮显示并在
//文本提示框中有提示，否则，直接切换，当前立体模型和窗口都改变。
void CDrawGraphStatic::RefreshView()
{
	//得到当前立体窗口的中心点
	PT_2D pt2 ,pt0;
	pt2.x=(m_msCurrentStereoWindow.p1.x+m_msCurrentStereoWindow.p2.x+
		m_msCurrentStereoWindow.p3.x+m_msCurrentStereoWindow.p4.x)/4;
	pt2.y=(m_msCurrentStereoWindow.p1.y+m_msCurrentStereoWindow.p2.y+
		m_msCurrentStereoWindow.p3.y+m_msCurrentStereoWindow.p4.y)/4;
	//坐标转换为设备坐标系
	pt0 = pt2;
	GeoToDev(&pt0);
	CPoint pt=CPoint(pt0.x,pt0.y);

	//获取内缩1/8后的矩形
	CRect rect;
	rect=m_rectDrawArea;
	rect.DeflateRect(m_rectDrawArea.Width()/8,m_rectDrawArea.Height()/8);
	
	if (!rect.PtInRect(pt))
	{
		CSize size=m_rectDrawArea.CenterPoint()-pt;
		m_lfMsOrgLogicX+=size.cx;
		m_lfMsOrgLogicY+=size.cy;
	}
	//模型切换

		//存在最邻近模型（有个缓冲）则切换。
		ModelScope ms1;
		if(!GetNearestMsToPt(&ms1,pt))
		{
			m_bIsExistForMsNearest=false;
			return;
		}
		if (IsEqualForMS(ms1,m_msCurrentModel))//是当前模型
		{
			m_bIsExistForMsNearest=false;
			return;
		}
		//当前窗口是否完全落在临近模型内
// 		ModelScope ms=ms1;
// 		GeoToDev(&ms);
// 		PT_3D pt1,pts[4];
// 		PT_2D pt11;
// 		COPY_2DPT((pts[0]),ms.p1)
// 		COPY_2DPT((pts[1]),ms.p2)
// 		COPY_2DPT((pts[2]),ms.p3)
// 		COPY_2DPT((pts[3]),ms.p4)
// 
// 		COPY_2DPT(pt11,m_msCurrentStereoWindow.p1)
// 		GeoToDev(&pt11);
// 		COPY_2DPT(pt1,pt11)
// 		if (!GIsPtInRegion(pt1,pts,4))
// 		{
// 			m_bIsExistForMsNearest=false;
// 			return;
// 		}
// 		COPY_2DPT(pt11,m_msCurrentStereoWindow.p2)
// 		GeoToDev(&pt11);
// 		COPY_2DPT(pt1,pt11)
// 		if (!GIsPtInRegion(pt1,pts,4))
// 		{
// 			m_bIsExistForMsNearest=false;
// 			return;
// 		}
// 		COPY_2DPT(pt11,m_msCurrentStereoWindow.p3)
// 		GeoToDev(&pt11);
// 		COPY_2DPT(pt1,pt11)
// 		if (!GIsPtInRegion(pt1,pts,4))
// 		{
// 			m_bIsExistForMsNearest=false;
// 			return;
// 		}
// 		COPY_2DPT(pt11,m_msCurrentStereoWindow.p4)
// 		GeoToDev(&pt11);
// 		COPY_2DPT(pt1,pt11)
// 		if (!GIsPtInRegion(pt1,pts,4))
// 		{
// 			m_bIsExistForMsNearest=false;
// 			return;
// 		}		
		if (m_bIsExistForMsNearest&&IsEqualForMS(ms1,m_msNearestModel)&&!m_bIsAutoSwitch)
		{
			return;
		}
		m_bIsExistForMsNearest=true;
		m_msNearestModel=ms1;
		if (m_bIsAutoSwitch)
		{
			//距离差的值
			//(pt2.x-)
			PT_2D pt2d;
			pt2d.x=(m_msCurrentModel.p1.x+m_msCurrentModel.p2.x+m_msCurrentModel.p3.x+m_msCurrentModel.p4.x)/4;
			pt2d.y=(m_msCurrentModel.p1.y+m_msCurrentModel.p2.y+m_msCurrentModel.p3.y+m_msCurrentModel.p4.y)/4;
			double dis1 = sqrt((pt2.x-pt2d.x)*(pt2.x-pt2d.x)+(pt2.y-pt2d.y)*(pt2.y-pt2d.y));
			pt2d.x=(ms1.p1.x+ms1.p2.x+ms1.p3.x+ms1.p4.x)/4;
			pt2d.y=(ms1.p1.y+ms1.p2.y+ms1.p3.y+ms1.p4.y)/4;
			double dis2 = sqrt((pt2.x-pt2d.x)*(pt2.x-pt2d.x)+(pt2.y-pt2d.y)*(pt2.y-pt2d.y));
			ASSERT(dis1>dis2);
			if ((dis1-dis2)>m_lfTolerForStereoMove/*(fabs(m_msCurrentModel.p1.x-m_msCurrentModel.p2.x)+fabs(m_msCurrentModel.p1.y-m_msCurrentModel.p4.y))/10*/)//容差
			{
				//切换
				gdwInnerCmdData[0] = (gdwInnerCmdData[0]&(~0xfffffff))|(ms1.index);
				gdwInnerCmdData[1] = (LPARAM)&m_coreObj;

				CStereoView *pView = GetStereoView(0);
				if(pView==NULL) return;

				HWND hWnd=::GetFocus();
				::SendMessage(pView->m_hWnd,WM_STEREOVIEW_LOADMDL,
						gdwInnerCmdData[0],gdwInnerCmdData[1]);	
				::SetFocus(hWnd);
				m_msCurrentModel = m_msNearestModel;
				m_bIsExistForMsNearest=false;
			}			
		}

}



void CDrawGraphStatic::SwitchToNearest()
{
	if (m_bIsExistForMsNearest)
	{
		gdwInnerCmdData[0] = (gdwInnerCmdData[0]&(~0xfffffff))|(m_msNearestModel.index);
		gdwInnerCmdData[1] = (LPARAM)&m_coreObj;

		BOOL bDoubleScreen	= AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DOUBLESCREEN, gdef_bDoubleScreen);
		CView *pView = GetStereoView(0);
	
		if(!pView) return;
	//	HWND hWnd=::GetFocus();
		::SendMessage(pView->m_hWnd,WM_STEREOVIEW_LOADMDL,
			gdwInnerCmdData[0],gdwInnerCmdData[1]);			
		m_bIsExistForMsNearest=false;
		m_msCurrentModel = m_msNearestModel;
		pView->SetFocus();
	//	::SetFocus(hWnd);

	}

}


bool CDrawGraphStatic::GetNameForNearestMS(CString *str)
{
	ASSERT(str!=NULL);
	if(m_bIsExistForMsNearest)
	{
		*str=m_coreObj.stereo[m_msNearestModel.index].sp.stereoID;		
		return true;
	}
	else
		return false;

}

void CDrawGraphStatic::ClearMSArray()
{
	m_arrModelScopes.RemoveAll();

}

 int CDrawGraphStatic::FindMS(const CString &strID)
{
	for (int i=0;i<m_coreObj.iStereoNum;i++)
	{
		if (strID==m_coreObj.stereo[i].sp.stereoID)
		{
			return i;
		}
	}
    return -1;
}

void CDrawGraphStatic::SetCurrentMS(int index)
{
   for (int i=0;i<m_arrModelScopes.GetSize();i++)
   {
	   if ((m_arrModelScopes.GetAt(i)).index==index)
	   {
		   m_msCurrentModel=m_arrModelScopes.GetAt(i);
		   m_bIsExistForCurMS=true;
		   return;
	   }
   }
}

//DEL void CDrawGraphStatic::OnSwitchmodel() 
//DEL {
//DEL 	// TODO: Add your command handler code here
//DEL 	SwitchToNearest();
//DEL 	Invalidate();	
//DEL }

void CDrawGraphStatic::SwitchModel()
{
	m_bIsDragDisplay=false;
	m_bIsDragForMouse=false;
	m_bIsDrag=false;
	SwitchToNearest();
	Invalidate();

}

void CDrawGraphStatic::SetTolerForStereoMove(double toler)
{
	m_lfTolerForStereoMove = toler;
}


extern BOOL ProjectView_CanOpenStereo();

void CDrawGraphStatic::OnUpdateOpenEpip(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ProjectView_CanOpenStereo());
}


void CDrawGraphStatic::OnUpdateOpenPhoto(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ProjectView_CanOpenStereo());
}

BOOL CDrawGraphStatic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CDrawGraphStatic::OnReCalcModelBound()
{
	GetParent()->GetParent()->SendMessage(WM_COMMAND,ID_POPUP_RECALC_MODEL_BOUND,0);
}