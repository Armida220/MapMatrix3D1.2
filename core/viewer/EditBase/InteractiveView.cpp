// EditBaseView.cpp : implementation of the CInteractiveView class
//

#include "stdafx.h"
#include "glew.h"
#include "SmartViewFunctions.h"
//#include "EditBase.h"
#include "InteractiveView.h"
#include "Feature.h"
#include "DrawLineDC.h"
#include "ExMessage.h"
#include "Regdef.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MyNameSpaceBegin

/////////////////////////////////////////////////////////////////////////////
// CInteractiveView

IMPLEMENT_DYNCREATE(CInteractiveView, CSceneViewEx)

BEGIN_MESSAGE_MAP(CInteractiveView, CSceneViewEx)
	//{{AFX_MSG_MAP(CInteractiveView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_MESSAGE(FCCM_CLTTOGND,OnClientToGround)
	ON_MESSAGE(FCCM_GNDTOCLT,OnGroundToClient)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInteractiveView construction/destruction

CInteractiveView::CInteractiveView():
m_constDragL(CTmpGraphLayer::typeGrBuffer2d),
m_constDragR(CTmpGraphLayer::typeGrBuffer2d),
m_addconstDragL(CTmpGraphLayer::typeGrBuffer2d),
m_addconstDragR(CTmpGraphLayer::typeGrBuffer2d)
{
	m_bConstDragChanged = FALSE;
	matrix_toIdentity(m_matrix,4);
	matrix_toIdentity(m_rmatrix,4);
	
	m_clrHilite = gdef_clrHiVect;
	m_clrDragLine = gdef_clrDragVect;
}

CInteractiveView::~CInteractiveView()
{
}



void CInteractiveView::ClientToGround(PT_4D *pt0, PT_3D *pt1)
{
	PT_4D pt2 = *pt0;
	GraphAPI::TransformPointsWith44Matrix(m_rmatrix,(PT_3D*)&pt2,1);
	*pt1 = pt2;
}


void CInteractiveView::GroundToClient(PT_3D *pt0, PT_4D *pt1)
{
	PT_4D pt2 = *pt0;
	GraphAPI::TransformPointsWith44Matrix(m_matrix,&pt2,1);
	*pt1 = pt2;
}


void CInteractiveView::GroundToVectorLay(PT_3D *pt0, PT_4D *pt1)
{
}


LRESULT CInteractiveView::OnClientToGround(WPARAM wParam, LPARAM lParam)
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


LRESULT CInteractiveView::OnGroundToClient(WPARAM wParam, LPARAM lParam)
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

CCoordWnd CInteractiveView::GetCoordWnd()
{
	m_SearchCS.Create44Matrix(m_matrix);
	m_ViewCS.Create44Matrix(m_matrix);

	return CCoordWnd(&m_SearchCS,&m_ViewCS,m_gCurPoint,m_gCurPoint,FALSE);
}


void CInteractiveView::ZoomCustom(CPoint point,float change, BOOL bToCenter)
{
	change = ZoomSelect(GetZoomRate()*change)/GetZoomRate();

	double m[16], m2[16], mt[16];
	matrix_toIdentity(m,4);
	m[0] = m[5] = m[10] = change;

	matrix_toIdentity(m2,4);
	m2[3] = -point.x;
	m2[7] = -point.y;
	matrix_multiply(m,m2,4,mt);
	m2[3] = point.x;
	m2[7] = point.y;
	matrix_multiply(m2,mt,4,m);

	matrix_multiply(m,m_matrix,4,mt);
	memcpy(m_matrix,mt,sizeof(mt));
	matrix_reverse(m_matrix,4,m_rmatrix);

	CSceneViewEx::ZoomChange(point,change,bToCenter);

	Invalidate(TRUE);
}


void CInteractiveView::ScrollView(int dx,int dy,int dp,int dq)
{
	double m[16], mt[16];
	matrix_toIdentity(m,4);
	m[3] = dx;
	m[7] = dy;
	
	matrix_multiply(m,m_matrix,4,mt);
	memcpy(m_matrix,mt,sizeof(mt));
	matrix_reverse(m_matrix,4,m_rmatrix);
	
	Invalidate(TRUE);
}


CSize CInteractiveView::CalcViewSize()
{
	return CSceneViewEx::CalcViewSize();
}


/////////////////////////////////////////////////////////////////////////////
// CInteractiveView drawing

void CInteractiveView::DrawGrBuffer(CDC *pDC, const GrBuffer *pBuf, CCoordSys *pCS, COLORREF color, BOOL bColorUsed, int mode)
{
	if( !pBuf )return;
}


void CInteractiveView::ConvertGrBufferToVectLayer(const GrBuffer *in, GrBuffer2d *outL, GrBuffer2d *outR)
{
	const GrBuffer *pBuf = in;

	if( !pBuf )return;
	if( !pBuf->HeadGraph() )
		return;
	
	GrBuffer2d* pBufL=outL;
	GrBuffer2d* pBufR=outR;

	GrBuffer2d bufL1,bufR1;

	bufL1.AddBuffer(pBuf);
	if( !IsSingle() )bufR1.AddBuffer(pBuf);
	
	float lxoff,lyoff;
	float rxoff,ryoff;
	
	PT_3D pt0;
	PT_4D pt1;

	pt0 = pBuf->GetFirstPt();
	GroundToVectorLay( &pt0, &pt1 );

	//定义原点
	if( pBufL )
	{
		if( !pBufL->HeadGraph() )
		{
			lxoff = floor(pt1.x/1e+3)*1e+3;
			lyoff = floor(pt1.y/1e+3)*1e+3;
			
			pBufL->SetOrigin(lxoff,lyoff);
			bufL1.SetOrigin(lxoff,lyoff);
		}
		else
		{		
			pBufL->GetOrigin(lxoff,lyoff);
			bufL1.SetOrigin(lxoff,lyoff);
		}
	}

	if( pBufR )
	{
		if( !pBufR->HeadGraph() )
		{		
			rxoff = floor(pt1.z/1e+3)*1e+3;
			ryoff = floor(pt1.yr/1e+3)*1e+3;
			
			pBufR->SetOrigin(rxoff,ryoff);
			bufR1.SetOrigin(rxoff,ryoff);
		}
		else
		{
			pBufR->GetOrigin(rxoff,ryoff);
			bufR1.SetOrigin(rxoff,ryoff);
		}
	}

	//遍历坐标点 并转换坐标
	
	const GrVertexList *pList;
	GrVertex2d *grpt;
	
	const Graph *gr = pBuf->HeadGraph();
	if(!gr) return;
	
	Graph2d *grl=bufL1.HeadGraph();
	Graph2d *grr=bufR1.HeadGraph();

	while(gr)
	{
		if (IsGrPoint(gr))
		{
			const GrPoint *cgr = (const GrPoint*)gr;			
			pt0 = cgr->pt;
			GroundToVectorLay( &pt0, &pt1 );
			if (grl)
			{
				((GrPoint2d*)grl)->pt.x = pt1.x-lxoff;
				((GrPoint2d*)grl)->pt.y = pt1.y-lyoff;
			}
			if (grr)
			{
				((GrPoint2d*)grr)->pt.x = pt1.z-rxoff;
				((GrPoint2d*)grr)->pt.y = pt1.yr-ryoff;
			}
		
		}
		else if (IsGrPointString(gr))
		{
			const GrPointString *cgr = (const GrPointString*)gr;
			GrPointString2d *grlLine = NULL;
			GrPointString2d *grrLine = NULL;
			if(grl)grlLine = (GrPointString2d *)grl;
			if(grr)grrLine = (GrPointString2d *)grr;
			
			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToVectorLay( &pt0, &pt1 );
				if (grlLine)
				{
					grpt = &((grlLine->ptlist.pts)[i]);
					grpt->x = pt1.x-lxoff;
					grpt->y = pt1.y-lyoff;
				}
				if (grrLine)
				{
					grpt = &((grrLine->ptlist.pts)[i]);
					grpt->x = pt1.z-rxoff;
					grpt->y = pt1.yr-ryoff;
				}
			}
		}
		else if (IsGrLineString(gr))
		{
			const GrLineString *cgr = (const GrLineString*)gr;
			GrLineString2d *grlLine = NULL;
			GrLineString2d *grrLine = NULL;
			if(grl)grlLine = (GrLineString2d *)grl;
			if(grr)grrLine = (GrLineString2d *)grr;

			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToVectorLay( &pt0, &pt1 );
				if (grlLine)
				{
					grpt = &((grlLine->ptlist.pts)[i]);
					grpt->x = pt1.x-lxoff;
					grpt->y = pt1.y-lyoff;
				}
				if (grrLine)
				{
					grpt = &((grrLine->ptlist.pts)[i]);
					grpt->x = pt1.z-rxoff;
					grpt->y = pt1.yr-ryoff;
				}
				
			}		
		}
		else if (IsGrPolygon(gr))
		{
			const GrPolygon *cgr = (const GrPolygon*)gr;
			GrPolygon2d *grlPlg = NULL;
			GrPolygon2d *grrPlg = NULL;
			if(grl)grlPlg = (GrPolygon2d *)grl;
			if(grr)grrPlg = (GrPolygon2d *)grr;
			
			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToVectorLay( &pt0, &pt1 );
				if (grlPlg)
				{
					grpt = &((grlPlg->ptlist.pts)[i]);
					grpt->x = pt1.x-lxoff;
					grpt->y = pt1.y-lyoff;
				}
				if (grrPlg)
				{
					grpt = &((grrPlg->ptlist.pts)[i]);
					grpt->x = pt1.z-rxoff;
					grpt->y = pt1.yr-ryoff;
				}
			
			}		
		}
		else
		{
			const GrText *cgr = (const GrText*)gr;			
			pt0 = cgr->pt;
			GroundToVectorLay( &pt0, &pt1 );
			if (grl)
			{
				((GrText2d*)grl)->pt.x = pt1.x-lxoff;
				((GrText2d*)grl)->pt.y = pt1.y-lyoff;
			}
			if (grr)
			{
				((GrText2d*)grr)->pt.x = pt1.z-rxoff;
				((GrText2d*)grr)->pt.y = pt1.yr-ryoff;
			}

		}
		gr = gr->next;
		if(grl)grl = grl->next;
		if(grr)grr = grr->next;
	}

	if(pBufL)
	{
		pBufL->AddBuffer(&bufL1);
		pBufL->RefreshEnvelope(TRUE);
	}
	if(pBufR)
	{
		pBufR->AddBuffer(&bufR1);
		pBufR->RefreshEnvelope(TRUE);
	}
}

void CInteractiveView::ConvertGrBufferToClient(const GrBuffer *in, GrBuffer2d *outL, GrBuffer2d *outR)
{
	const GrBuffer *pBuf = in;

	if( !pBuf )return;
	if( !pBuf->HeadGraph() )
		return;
	
	GrBuffer2d* pBufL=outL;
	GrBuffer2d* pBufR=outR;

	GrBuffer2d bufL1,bufR1;

	bufL1.AddBuffer(pBuf);
	if( !IsSingle() )bufR1.AddBuffer(pBuf);
	
	float lxoff,lyoff;
	float rxoff,ryoff;
	
	PT_3D pt0;
	PT_4D pt1;

	pt0 = pBuf->GetFirstPt();
	GroundToClient( &pt0, &pt1 );

	//定义原点
	if( pBufL )
	{
		if( !pBufL->HeadGraph() )
		{
			lxoff = floor(pt1.x/1e+3)*1e+3;
			lyoff = floor(pt1.y/1e+3)*1e+3;
			
			pBufL->SetOrigin(lxoff,lyoff);
			bufL1.SetOrigin(lxoff,lyoff);
		}
		else
		{		
			pBufL->GetOrigin(lxoff,lyoff);
			bufL1.SetOrigin(lxoff,lyoff);
		}
	}

	if( pBufR )
	{
		if( !pBufR->HeadGraph() )
		{		
			rxoff = floor(pt1.z/1e+3)*1e+3;
			ryoff = floor(pt1.yr/1e+3)*1e+3;
			
			pBufR->SetOrigin(rxoff,ryoff);
			bufR1.SetOrigin(rxoff,ryoff);
		}
		else
		{
			pBufR->GetOrigin(rxoff,ryoff);
			bufR1.SetOrigin(rxoff,ryoff);
		}
	}

	//遍历坐标点 并转换坐标
	
	const GrVertexList *pList;
	GrVertex2d *grpt;
	
	const Graph *gr = pBuf->HeadGraph();
	if(!gr) return;
	
	Graph2d *grl=bufL1.HeadGraph();
	Graph2d *grr=bufR1.HeadGraph();

	while(gr)
	{
		if (IsGrPoint(gr))
		{
			const GrPoint *cgr = (const GrPoint*)gr;			
			pt0 = cgr->pt;
			GroundToClient( &pt0, &pt1 );
			if (grl)
			{
				((GrPoint2d*)grl)->pt.x = pt1.x-lxoff;
				((GrPoint2d*)grl)->pt.y = pt1.y-lyoff;
			}
			if (grr)
			{
				((GrPoint2d*)grr)->pt.x = pt1.z-rxoff;
				((GrPoint2d*)grr)->pt.y = pt1.yr-ryoff;
			}
		
		}
		else if (IsGrPointString(gr))
		{
			const GrPointString *cgr = (const GrPointString*)gr;
			GrPointString2d *grlLine = NULL;
			GrPointString2d *grrLine = NULL;
			if(grl)grlLine = (GrPointString2d *)grl;
			if(grr)grrLine = (GrPointString2d *)grr;

			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToClient( &pt0, &pt1 );
				if (grlLine)
				{
					grpt = &((grlLine->ptlist.pts)[i]);
					grpt->x = pt1.x-lxoff;
					grpt->y = pt1.y-lyoff;
				}
				if (grrLine)
				{
					grpt = &((grrLine->ptlist.pts)[i]);
					grpt->x = pt1.z-rxoff;
					grpt->y = pt1.yr-ryoff;
				}
			}
		}
		else if (IsGrLineString(gr))
		{
			const GrLineString *cgr = (const GrLineString*)gr;
			GrLineString2d *grlLine = NULL;
			GrLineString2d *grrLine = NULL;
			if(grl)grlLine = (GrLineString2d *)grl;
			if(grr)grrLine = (GrLineString2d *)grr;

			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToClient( &pt0, &pt1 );
				if (grlLine)
				{
					grpt = &((grlLine->ptlist.pts)[i]);
					grpt->x = pt1.x-lxoff;
					grpt->y = pt1.y-lyoff;
				}
				if (grrLine)
				{
					grpt = &((grrLine->ptlist.pts)[i]);
					grpt->x = pt1.z-rxoff;
					grpt->y = pt1.yr-ryoff;
				}
				
			}		
		}
		else if (IsGrPolygon(gr))
		{
			const GrPolygon *cgr = (const GrPolygon*)gr;
			GrPolygon2d *grlPlg = NULL;
			GrPolygon2d *grrPlg = NULL;
			if(grl)grlPlg = (GrPolygon2d *)grl;
			if(grr)grrPlg = (GrPolygon2d *)grr;
			
			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToClient( &pt0, &pt1 );
				if (grlPlg)
				{
					grpt = &((grlPlg->ptlist.pts)[i]);
					grpt->x = pt1.x-lxoff;
					grpt->y = pt1.y-lyoff;
				}
				if (grrPlg)
				{
					grpt = &((grrPlg->ptlist.pts)[i]);
					grpt->x = pt1.z-rxoff;
					grpt->y = pt1.yr-ryoff;
				}
			
			}		
		}
		else
		{
			const GrText *cgr = (const GrText*)gr;			
			pt0 = cgr->pt;
			GroundToClient( &pt0, &pt1 );
			if (grl)
			{
				((GrText2d*)grl)->pt.x = pt1.x-lxoff;
				((GrText2d*)grl)->pt.y = pt1.y-lyoff;
			}
			if (grr)
			{
				((GrText2d*)grr)->pt.x = pt1.z-rxoff;
				((GrText2d*)grr)->pt.y = pt1.yr-ryoff;
			}

		}
		gr = gr->next;
		if(grl)grl = grl->next;
		if(grr)grr = grr->next;
	}

	if(pBufL)
	{
		pBufL->AddBuffer(&bufL1);
		pBufL->RefreshEnvelope(TRUE);
	}
	if(pBufR)
	{
		pBufR->AddBuffer(&bufR1);
		pBufR->RefreshEnvelope(TRUE);
	}
}

void CInteractiveView::SetConstDragLine(const GrBuffer *pBuf)
{
	GrBuffer2d *pVectL = (GrBuffer2d*)m_constDragL.OpenObj(HANDLE_CONSTDRAG);
	GrBuffer2d *pVectR = (GrBuffer2d*)m_constDragR.OpenObj(HANDLE_CONSTDRAG);

	pVectL->DeleteAll();
	pVectR->DeleteAll();

	ConvertGrBufferToVectLayer(pBuf,pVectL,pVectR);

	pVectL = (GrBuffer2d*)m_addconstDragL.OpenObj(HANDLE_CONSTDRAG);
	pVectR = (GrBuffer2d*)m_addconstDragR.OpenObj(HANDLE_CONSTDRAG);
	
	pVectL->DeleteAll();
	pVectR->DeleteAll();

	m_bConstDragChanged = TRUE;
}


void CInteractiveView::AddConstDragLine(const GrBuffer *pBuf)
{
	GrBuffer2d buf1, buf2;	
	ConvertGrBufferToVectLayer(pBuf,&buf1,&buf2);

	GrBuffer2d *pVectL = (GrBuffer2d*)m_constDragL.OpenObj(HANDLE_CONSTDRAG);
	GrBuffer2d *pVectR = (GrBuffer2d*)m_constDragR.OpenObj(HANDLE_CONSTDRAG);

	pVectL->AddBuffer(&buf1);
	pVectR->AddBuffer(&buf2);

	pVectL = (GrBuffer2d*)m_addconstDragL.OpenObj(HANDLE_CONSTDRAG);
	pVectR = (GrBuffer2d*)m_addconstDragR.OpenObj(HANDLE_CONSTDRAG);
	
	pVectL->CopyFrom(&buf1);
	pVectR->CopyFrom(&buf2);

	m_bConstDragChanged = TRUE;
}

void CInteractiveView::SetVariantDragLine(const GrBuffer *pBuf, BOOL bGround)
{
	GrBuffer2d buf1, buf2;

	ConvertGrBufferToClient(pBuf,&buf1,&buf2);

	buf1.SetAllColor(m_clrDragLine);
	buf2.SetAllColor(m_clrDragLine);

	m_variantDragL.SetBuffer(&buf1);	
	m_variantDragR.SetBuffer(&buf2);
}

void CInteractiveView::AddVariantDragLine(const GrBuffer *pBuf, BOOL bGround)
{
	GrBuffer2d buf1, buf2;
	
	ConvertGrBufferToClient(pBuf,&buf1,&buf2);
	
	buf1.SetAllColor(m_clrDragLine);
	buf2.SetAllColor(m_clrDragLine);
	
	m_variantDragL.AddBuffer(&buf1);
	m_variantDragR.AddBuffer(&buf2);
}

void CInteractiveView::ClearDragLine()
{
	SetConstDragLine(NULL);
	SetVariantDragLine(NULL);
}

void CInteractiveView::HiliteObject(CGeometry *pObj)
{
// 	GrBuffer buf;
// 	pObj->Draw(&buf);
// 
// 	CClientDC dc(this);
//	DrawGrBuffer(&dc,&buf,NULL,RGB(255,0,255),TRUE,DR_LEFT|DR_RIGHT);
}


/////////////////////////////////////////////////////////////////////////////
// CInteractiveView message handlers


CWorker *CInteractiveView::GetWorker()
{
	return NULL;
}

void CInteractiveView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_nInnerCmd!=0 )
	{
		CSceneViewEx::OnLButtonDown(nFlags, point);
		return;
	}
	GetWorker()->SetCoordWnd(GetCoordWnd());
	GetWorker()->LButtonDown(m_gCurPoint,nFlags);
	UpdateVariantDragLine();
	UpdateConstDragLine();
	FinishUpdateDrawing();

	CSceneViewEx::OnLButtonDown(nFlags, point);
}

void CInteractiveView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( m_nInnerCmd!=0 )
	{
		CSceneViewEx::OnLButtonDblClk(nFlags, point);
		return;
	}
	GetWorker()->SetCoordWnd(GetCoordWnd());
	GetWorker()->LButtonDblClk(m_gCurPoint,nFlags);
	UpdateVariantDragLine();
	UpdateConstDragLine();
	FinishUpdateDrawing();
	
	CSceneViewEx::OnLButtonDblClk(nFlags, point);
/*	
	if( !(m_bSelectorOpen&&m_bSelDrag) )
	{
		CDlgDoc *pDoc = GetDocument();
//		pDoc->m_selection.m_bWithSymbol = m_bSymbolize;
		
		CCommand *pCurCmd = pDoc->GetCurrentCommand();
		if( pCurCmd )
		{
//			pCurCmd->SetCoordMan(GetCoordMan());
			pCurCmd->PtDblClick(m_gCurPoint,0);
			UpdateConstDragLine();
			UpdateVariantDragLine();
//			pDoc->TryFinishCurCmd();
			pDoc->TryFinishCurProcedure();
			pDoc->ActiveSonProcedure();
			
			if( pDoc->CanSetAnchor() )
				pDoc->SetAnchorPoint(m_gCurPoint);
		}
	}
*/	
}

void CInteractiveView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_nInnerCmd != 0)
	{
		CSceneViewEx::OnMouseMove(nFlags, point);
		return;
	}
	GetWorker()->SetCoordWnd(GetCoordWnd());
	GetWorker()->MouseMove(m_gCurPoint,nFlags);
	UpdateVariantDragLine();
	UpdateConstDragLine();
	FinishUpdateDrawing();
	CSceneViewEx::OnMouseMove(nFlags, point);
}

void CInteractiveView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_nInnerCmd != 0)
	{
		CSceneViewEx::OnRButtonDown(nFlags, point);
		return;
	}
	GetWorker()->SetCoordWnd(GetCoordWnd());
	GetWorker()->RButtonDown(m_gCurPoint,nFlags);
	UpdateVariantDragLine();
	UpdateConstDragLine();
	FinishUpdateDrawing();
	
	CSceneViewEx::OnRButtonDown(nFlags, point);
}

void CInteractiveView::OnInitialUpdate() 
{
	CSceneViewEx::OnInitialUpdate();
	
	m_constDragL.SetContext(m_pContext->GetLeftContext());
	m_constDragR.SetContext(m_pContext->GetRightContext());
	
	m_addconstDragL.SetContext(m_pContext);
	m_addconstDragR.SetContext(m_pContext);

	m_variantDragL.SetContext(m_pContext);	
	m_variantDragR.SetContext(m_pContext);
	m_variantDragL.m_bUseCoordSys = FALSE;
	m_variantDragR.m_bUseCoordSys = FALSE;

	m_laymgrLeft.InsertBeforeDrawingLayer(&m_constDragL,&m_cursorL);	
	m_laymgrRight.InsertBeforeDrawingLayer(&m_constDragR,&m_cursorR);
	
	m_laymgrLeft.InsertBeforeDrawingLayer(&m_addconstDragL,&m_cursorL);	
	m_laymgrRight.InsertBeforeDrawingLayer(&m_addconstDragR,&m_cursorR);

	m_laymgrLeft.InsertBeforeDrawingLayer(&m_variantDragL,&m_cursorL);	
	m_laymgrRight.InsertBeforeDrawingLayer(&m_variantDragR,&m_cursorR);
	
//	GetWorker()->RegisterSelectStateCommand(CEditVertexCommand::Create);
//	GetWorker()->RegisterSelectStateCommand(CInsertVertexCommand::Create);
	OnSelect();
}

void CInteractiveView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetWorker()->SetCoordWnd(GetCoordWnd());
	GetWorker()->KeyDown(nChar,nFlags);
	FinishUpdateDrawing();

	CSceneViewEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CInteractiveView::UpdateConstDragLine()
{
	if( !m_bConstDragChanged )
		return;

	UpdateDrawingLayers(&m_addconstDragL,&m_addconstDragR);
	m_bConstDragChanged = FALSE;
}

void CInteractiveView::UpdateVariantDragLine()
{
	if( !m_variantDragL.IsBufferChanged() )
		return;
	
	UpdateDrawingLayers(&m_variantDragL,&m_variantDragR);
}


void CInteractiveView::SetCursorType(long type)
{
	switch(type) 
	{
	case CURSOR_DISABLE:
		EnableCrossSelMarkVisible(FALSE);
		EnableCrossCrossVisible(FALSE);
		break;
	case CURSOR_NORMAL:
		EnableCrossSelMarkVisible(TRUE);
		EnableCrossCrossVisible(TRUE);
		break;
	case CURSOR_SELECT:
		EnableCrossSelMarkVisible(TRUE);
		EnableCrossCrossVisible(FALSE);
		break;
	case CURSOR_DRAW:
		EnableCrossSelMarkVisible(FALSE);
		EnableCrossCrossVisible(TRUE);
		break;
	default:;
	}
	
	UpdateCross();
}


void CInteractiveView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch( lHint )
	{
	case hc_SetMapArea:
		{
			Envelope *e0 = (Envelope*)pHint;
			if( !e0 )break;

			Envelope e = *e0;
			if( e.IsEmpty(2) )break;
			if( e.IsEmpty(3) )
			{
				e.m_zl = -1000; e.m_zh = 1000;
			}

			CSize szl = CSize(600,600), szr = CSize(600,600);
			double ratex = szl.cx/e.Width(), ratey = szl.cy/e.Height();
			
			if( ratex>ratey )ratex = ratey;
			else ratey = ratex;

			m_matrix[0] = ratex; m_matrix[1] = 0; m_matrix[2] = 0; m_matrix[3] = -e.m_xl*ratex; 
			m_matrix[4] = 0; m_matrix[5] = ratey; m_matrix[6] = 0; m_matrix[7] =  -e.m_yl*ratey;
			m_matrix[8] = 0; m_matrix[9] = 0; m_matrix[10] = ratex; m_matrix[11] = 0; 
			m_matrix[12] = 0; m_matrix[13] = 0; m_matrix[14] = 0; m_matrix[15] = 1.0; 

			matrix_reverse(m_matrix,4,m_rmatrix);

			Invalidate(FALSE);
		}
		break;
	case hc_Refresh:
		{
			UpdateDrawingLayers(NULL,NULL);
			FinishUpdateDrawing();
		}
		break;
	case hc_AddObject:
		{
			UpdateDrawingLayers(NULL,NULL);
		}
		break;
	case hc_DelObject:
		{
			UpdateDrawingLayers(NULL,NULL);
		}
		break;
	case hc_ClearDragLine:		
		{
			ClearDragLine();
			UpdateDrawingLayers(NULL,NULL);
		}
		break;
	case hc_SetConstDragLine:
		SetConstDragLine((const GrBuffer*)pHint);
		break;
	case hc_AddConstDragLine:
		AddConstDragLine((const GrBuffer*)pHint);
		break;
	case hc_SetVariantDragLine:
		SetVariantDragLine((const GrBuffer*)pHint);	
		break;
	case hc_AddVariantDragLine:
		AddVariantDragLine((const GrBuffer*)pHint);
		break;
	case hc_UpdateConstDrag:
		UpdateConstDragLine();	
		FinishUpdateDrawing();
		break;
	case hc_UpdateVariantDrag:
		UpdateVariantDragLine();
		FinishUpdateDrawing();
		break;
	case hc_SelChanged:
		Invalidate(FALSE);
		break;
	case hc_SetCursorType:
		SetCursorType((long)pHint);
		break;
	}
}

BOOL CInteractiveView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
/*	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect,0);*/
	return TRUE;
}


void CInteractiveView::OnSelect() 
{
	GetWorker()->SetCoordWnd(GetCoordWnd());
	GetWorker()->StartDefaultSelect();
}

void CInteractiveView::OnUpdateSelect(CCmdUI* pCmdUI)
{
	int id = GetWorker()->GetCurrentCommandId();
	if (id==0)
	{
		if( GetWorker()->IsSelectorOpen() && GetWorker()->GetSelectorMode()!=SELMODE_POLYGON && GetWorker()->GetSelectorMode()!=SELMODE_RECT )
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
		pCmdUI->SetCheck(FALSE);
}

void CInteractiveView::OnPaint() 
{
	CSceneViewEx::OnPaint();
}

void CInteractiveView::OnDestroy()
{
	m_constDragL.Destroy();
	m_constDragR.Destroy();
	m_addconstDragL.Destroy();
	m_addconstDragR.Destroy();
	m_variantDragL.Destroy();
	m_variantDragR.Destroy();

	m_constDragL.SetContext(NULL);
	m_constDragR.SetContext(NULL);
	m_addconstDragL.SetContext(NULL);
	m_addconstDragR.SetContext(NULL);
	m_variantDragL.SetContext(NULL);
	m_variantDragR.SetContext(NULL);

	CSceneViewEx::OnDestroy();
}

MyNameSpaceEnd
