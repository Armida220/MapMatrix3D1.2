// VectorView_EX.cpp: implementation of the CVectorView_EX class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "EditBase.h"
//#include "cadlib.h"
#include "glew.h"
#include "EditBaseDoc.h"
#include "VectorView_EX.h"
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REG_CMDCLASS(pWorker,id,clsname)  (pWorker)->RegisterCommand((id),clsname::Create)


/////////////////////////////////////////////////////////////////////////////
// CVectorView_EX

IMPLEMENT_DYNCREATE(CVectorView_EX, CVectorView_new)

BEGIN_MESSAGE_MAP(CVectorView_EX, CVectorView_new)
	//{{AFX_MSG_MAP(CVectorView_EX)
	ON_WM_KEYDOWN()
    ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands	
	ON_COMMAND(ID_VIEW_RESET,OnViewReset)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT,OnUpdatePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_DIRECT,OnUpdatePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW,OnUpdatePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP,OnUpdatePrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVectorView_EX construction/destruction

CVectorView_EX::CVectorView_EX()
{
	matrix_toIdentity(m_lfInitMatrix,4);
}

CVectorView_EX::~CVectorView_EX()
{
}

BOOL CVectorView_EX::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CVectorView_new::PreCreateWindow(cs);
}


void CVectorView_EX::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CVectorView_EX printing

BOOL CVectorView_EX::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVectorView_EX::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVectorView_EX::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CVectorView_EX diagnostics

#ifdef _DEBUG
void CVectorView_EX::AssertValid() const
{
	CVectorView_new::AssertValid();
}

void CVectorView_EX::Dump(CDumpContext& dc) const
{
	CVectorView_new::Dump(dc);
}

CDlgDoc* CVectorView_EX::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDlgDoc)));
	return (CDlgDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVectorView_EX message handlers


void CVectorView_EX::OnInitialUpdate() 
{
	CVectorView_new::OnInitialUpdate();
	{
		CRect reClient;
		GetClientRect(&reClient);
		CPoint point = reClient.CenterPoint();
		PT_3D pt,dpt;
		pt.x = point.x;
		pt.y = point.y;
		pt.z = 0;
		dpt.x = reClient.Width();
		dpt.y = 0;
		dpt.z = 0;
		double m[16];
		Matrix44FromRotate(&pt,&dpt,-PI/2,m);		
		m_pLeftCS->MultiplyMatrix(m);	
	}
	m_pLeftCS->GetMatrix(m_lfInitMatrix);

	m_vectLayL2.EnableSymbolized(FALSE);
	m_vectLayL2.Enable2DCoordSys(FALSE);
	m_variantDragL.m_bUseGrBuffer = TRUE;

	m_vectLayL2.ClearAll();

	OnZoomFit();

}


void CVectorView_EX::OnViewReset()
{
	m_pLeftCS->Create44Matrix(m_lfInitMatrix);
	m_vectLayL2.ClearAll();

	m_laymgrLeft.OnChangeCoordSys(FALSE);
	OnChangeCoordSys(FALSE);
	OnZoomFit();
}

void CVectorView_EX::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL bIsRotate = FALSE;
	
	double ang = 5*PI/180;
	int dir = 0;
	switch(nChar) 
	{
	case 'a':
	case 'A':
		dir = 1;
		bIsRotate = TRUE;
		break;
	case 'd':
	case 'D':
		dir = 2;
		ang = -ang;
		bIsRotate = TRUE;
		break;
	case 'w':
	case 'W':
		dir = 3;
		bIsRotate = TRUE;
		break;
	case 's':
	case 'S':
		dir = 4;
		ang = -ang;
		bIsRotate = TRUE;
		break;	
	case 'z':
	case 'Z':
		dir = 5;		
		bIsRotate = TRUE;
		break;
	case 'c':
	case 'C':
		dir = 6;
		ang = -ang;
		bIsRotate = TRUE;
		break;
	default:break;
	}	
	if( bIsRotate )
	{
		CRect reClient;
		GetClientRect(&reClient);
		CPoint point = reClient.CenterPoint();
		PT_3D pt,dpt;
		pt.x = point.x;
		pt.y = point.y;
		pt.z = 0;
		switch(dir)
		{
		case 1:			
		case 2:
			dpt.x = 0;
			dpt.y = reClient.Height();
			dpt.z = 0;
			break;
		case 3:		    
		case 4:
			dpt.x = reClient.Width();
			dpt.y = 0;
			dpt.z = 0;
		    break;
		case 5:
		case 6:
			dpt.x = 0;
			dpt.y = 0;
			dpt.z = -1;
			break;
		default:
		    return;
		}		
		double m[16];
		Matrix44FromRotate(&pt,&dpt,ang,m);		
		m_pLeftCS->MultiplyMatrix(m);
		
		m_laymgrLeft.OnChangeCoordSys(FALSE);
		OnChangeCoordSys(FALSE);
		
		CSceneView::OnRefresh();

		return;
	}

	CVectorView_new::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CVectorView_EX::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
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
	
	CVectorView_new::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

int CVectorView_EX::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	return CVectorView_new::OnCreate(lpCreateStruct);
}

void CVectorView_EX::OnDestroy()
{
	CVectorView_new::OnDestroy();
}



void CVectorView_EX::SetVariantDragLine(const GrBuffer *pBuf, BOOL bGround)
{
	GrBuffer buf;
	ConvertGrBufferToVectLayer(pBuf,&buf);

	buf.SetAllColor(m_clrDragLine);
	m_variantDragL.SetBuffer(&buf);
}

void CVectorView_EX::AddVariantDragLine(const GrBuffer *pBuf, BOOL bGround)
{
	GrBuffer buf;
	ConvertGrBufferToVectLayer(pBuf,&buf);
	
	buf.SetAllColor(m_clrDragLine);
	m_variantDragL.AddBuffer(&buf);
}



void CVectorView_EX::ConvertGrBufferToVectLayer(const GrBuffer *in, GrBuffer *out)
{
	const GrBuffer *pBuf = in;

	if( !pBuf )return;
	if( !pBuf->HeadGraph() )
		return;
	
	GrBuffer* pBufL=out;
	
	GrBuffer bufL;

	bufL.AddBuffer(pBuf);
	
	PT_3D pt0;
	PT_4D pt1;

	//遍历坐标点 并转换坐标
	
	const GrVertexList *pList;
	GrVertex *grpt;
	
	const Graph *gr = pBuf->HeadGraph();
	if(!gr) return;
	
	Graph *grl=bufL.HeadGraph();

	while(gr)
	{
		if (IsGrPoint(gr))
		{
			const GrPoint *cgr = (const GrPoint*)gr;			
			pt0 = cgr->pt;
			GroundToVectorLay( &pt0, &pt1 );
			if (grl)
			{
				((GrPoint*)grl)->pt = (PT_3D)pt1;
			}
		
		}
		else if (IsGrPointString(gr))
		{
			const GrPointString *cgr = (const GrPointString*)gr;
			GrPointString *grlLine = NULL;
			if(grl)grlLine = (GrPointString *)grl;
			
			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToVectorLay( &pt0, &pt1 );
				if (grlLine)
				{
					grpt = &((grlLine->ptlist.pts)[i]);
					grpt->x = pt1.x;
					grpt->y = pt1.y;
					grpt->z = pt1.z;
				}
			}
		}
		else if (IsGrLineString(gr))
		{
			const GrLineString *cgr = (const GrLineString*)gr;
			GrLineString *grlLine = NULL;
			if(grl)grlLine = (GrLineString *)grl;

			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToVectorLay( &pt0, &pt1 );
				if (grlLine)
				{
					grpt = &((grlLine->ptlist.pts)[i]);
					grpt->x = pt1.x;
					grpt->y = pt1.y;
					grpt->z = pt1.z;
				}				
			}		
		}
		else if (IsGrPolygon(gr))
		{
			const GrPolygon *cgr = (const GrPolygon*)gr;
			GrPolygon *grlPlg = NULL;
			if(grl)grlPlg = (GrPolygon *)grl;
			
			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToVectorLay( &pt0, &pt1 );
				if (grlPlg)
				{
					grpt = &((grlPlg->ptlist.pts)[i]);
					grpt->x = pt1.x;
					grpt->y = pt1.y;
					grpt->z = pt1.z;
				}			
			}		
		}
		else
		{
			const GrText *cgr = (const GrText*)gr;	
			GrText *grt = NULL;
			if(grl)grt = (GrText*)grl;
			
			pt0 = cgr->pt;
			GroundToVectorLay( &pt0, &pt1 );
			if (grt)
			{
				grt->pt = (PT_3D)pt1;
			}
		}
		gr = gr->next;
		if(grl)grl = grl->next;
	}

	if(pBufL)
	{
		pBufL->AddBuffer(&bufL);
		pBufL->RefreshEnvelope();
	}
}

void CVectorView_EX::OnUpdatePrint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CVectorView_EX::OnUpdatePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}