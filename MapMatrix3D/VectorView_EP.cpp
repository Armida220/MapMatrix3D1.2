// VectorView_EP.cpp: implementation of the CVectorView_EP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "editbasedoc.h"
#include "VectorView_EP.h"
#include "DlgDataSource.h "
#include "exMessage.h"
#include "SmartViewFunctions.h"
//#include "XVVectDrawer.h"
#include "GeoCurve.h"
#include "Envelope.h"
//#include "EPCommand.h"
//#include "DlgSetMileage.h "

extern BYTE clrTable_CAD[];

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CEPSectionViewCenterPoint0::CEPSectionViewCenterPoint0(CEPSectionView *p)
{
	pView = p;
	Save();
}


CEPSectionViewCenterPoint0::~CEPSectionViewCenterPoint0()
{
	Restore();
}

void CEPSectionViewCenterPoint0::Save()
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


void CEPSectionViewCenterPoint0::Restore()
{
	if( !pView )return;
	pView->DriveToXyz(&ptCenter);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CEPSectionView, CVectorView_new)

BEGIN_MESSAGE_MAP(CEPSectionView, CVectorView_new)
	//{{AFX_MSG_MAP(CEPSectionView)
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CEPSectionView::CEPSectionView()
{
	m_lfGScaleX = 1.0;
	m_lfGScaleZ = 1.0;	
}

CEPSectionView::~CEPSectionView()
{
	
}
/*
void CEPSectionView::AddObjtoVectLay(LONG_PTR handle, const GrBuffer *pBuf, BOOL bGround, CVectorLayer *pLL, CVectorLayer *pLR)
{
	if( !pBuf )return;
	if( !pBuf->HeadGraph() )
		return;
	
	if( !pLL )pLL = &m_vectLayL;
	if( !pLR )pLR = &m_vectLayR;
	
	GrBuffer* pBufL=NULL;
	GrBuffer* pBufR=NULL;
	
	pBufL = (GrBuffer*)pLL->OpenObj(handle);
	if( !IsSingle() )pBufR = (GrBuffer*)pLR->OpenObj(handle);
	
	ConvertGrBufferToVectLayer(pBuf,pBufL,pBufR);
	
	pLL->ClearAll();
	if( !IsSingle() )pLR->ClearAll();
	
	pLL->FinishObj(handle);
	if( !IsSingle() )pLR->FinishObj(handle);
}

void CEPSectionView::ConvertGrBufferToVectLayer(const GrBuffer *in, GrBuffer *outL, GrBuffer *outR)
{
	const GrBuffer *pBuf = in;
	
	if( !pBuf )return;
	if( !pBuf->HeadGraph() )
		return;
	
	GrBuffer* pBufL=outL;
	GrBuffer* pBufR=outR;
	
	GrBuffer bufL1,bufR1;
	
	bufL1.AddBuffer(pBuf);
	if( !IsSingle() )bufR1.AddBuffer(pBuf);
	if(pBufL)
	{
		pBufL->AddBuffer(&bufL1);
		pBufL->RefreshEnvelope();
	}
	if(pBufR)
	{
		pBufR->AddBuffer(&bufR1);
		pBufR->RefreshEnvelope();
	}
}
*/
void CEPSectionView::OnInitialUpdate()
{
	CDlgDoc *pDoc = (CDlgDoc*)GetDocument();
	
	//寻找标记矩形，计算断面路由
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();

	CString strDefLineLayer;
	strDefLineLayer.LoadString(IDS_DEFLAYER_NAMEL);
	
	CFtrLayer *pFtrLayer = pDS->GetFtrLayer(strDefLineLayer);
	
	if (pFtrLayer != NULL)
	{
		int sum = pFtrLayer->GetObjectCount();
		for (int i=0; i<sum; i++)
		{
			CFeature *pFtr = pFtrLayer->GetObject(i);
			if (pFtr == NULL) continue;
			
			if (stricmp(pFtr->GetCode(),EPREGION_FTRCODE) == 0)
			{
				CGeometry *pGeo = pFtr->GetGeometry();
				CArray<PT_3DEX,PT_3DEX> pts;
				pGeo->GetShape(pts);

				if (pts.GetSize() >= 4)
				{
					m_fRoadWid = GraphAPI::GGet2DDisOf2P(pts[0],pts[3])/2;
					CArray<PT_3DEX,PT_3DEX> ret;
					ret.SetSize(2);
					
					if (GraphAPI::GGetParallelLine(pts.GetData(),2,-m_fRoadWid,ret.GetData()))
					{
						m_arrRoadPts.SetSize(2);
						for (int j=0; j<ret.GetSize(); j++)
						{
							COPY_3DPT(m_arrRoadPts[j],ret[j]);
						}
					}
				}
				
				break;
			}
		}
	}

	CVectorView_new::OnInitialUpdate();

	/*CRect reClient;
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
	m_pLeftCS->MultiplyMatrix(m);*/

// 	PT_3D pt3ds[4];
// 	memset(pt3ds,0,sizeof(pt3ds));
// 	pDS->GetBound(pt3ds,NULL,NULL);
// 	OnUpdate(NULL,hc_SetMapArea,(CObject*)pt3ds);


	//pDoc->UpdateRoadLines();
}
/*
BOOL CEPSectionView::LoadDocData()
{
	if( !CBaseView::LoadDocData() )
		return FALSE;
	
	Envelope dataBound = GetDocument()->GetDlgDataSource()->GetBound();	
	m_vectLayL.SetDataBound(dataBound);
	m_vectLayR.SetDataBound(dataBound);
	m_markLayL.SetDataBound(dataBound);
	m_markLayR.SetDataBound(dataBound);
	UpdateGridDrawing();
	OnUpdate(this,hc_UpdateOverlayBound,NULL);
	return TRUE;
}
*/
void CEPSectionView::OnDestroy() 
{
	CBaseView::OnDestroy();
}


void CEPSectionView::ImageToGround(PT_4D *pt0, PT_3D *pt1)
{
// 	CVectorView_new::ImageToGround(pt0,pt1);
// 	return;
	CDlgDoc *pDoc = (CDlgDoc*)GetDocument();
	if( m_arrRoadPts.GetSize()<=1 )return;

	CVectorView_new::ImageToGround(pt0,pt1);
	
	PT_3D *pts = m_arrRoadPts.GetData();
	int num = m_arrRoadPts.GetSize(), i;

	//转换Z坐标
	pt1->z = pt1->y/m_lfGScaleZ;

	double dis0 = pt1->x/m_lfGScaleX;

	//转换XY坐标
	double cur=0, dis;
	for( i=0; i<num-1; i++)
	{
		dis = sqrt((pts[i].x-pts[i+1].x)*(pts[i].x-pts[i+1].x)+
			(pts[i].y-pts[i+1].y)*(pts[i].y-pts[i+1].y));
		if( cur+dis>=dis0 )break;

		cur += dis;
	}
	
	if( i>=num-1 )
	{
		pt1->x = pts[num-1].x;
		pt1->y = pts[num-1].y;
	}
	else if( dis0<=0 )
	{
		pt1->x = pts[0].x;
		pt1->y = pts[0].y;
	}
	else
	{
		double vx = (pts[i+1].x-pts[i].x)/dis, vy = (pts[i+1].y-pts[i].y)/dis;
		pt1->x = pts[i].x+(dis0-cur)*vx;
		pt1->y = pts[i].y+(dis0-cur)*vy;
	}
}


void CEPSectionView::GroundToImage(PT_3D *pt0, PT_4D *pt1)
{
// 	CVectorView_new::GroundToImage(pt0,pt1);
// 	return;

	CDlgDoc *pDoc = (CDlgDoc*)GetDocument();
	if( m_arrRoadPts.GetSize()<=1 )return;

	PT_3D *pts = m_arrRoadPts.GetData(), ret;
	int num = m_arrRoadPts.GetSize(), k = -1;
	
	//找到路线上的最近点
	double dis, min=-1,x,y,z;
	for( int i=0; i<num-1; i++)
	{
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
			pts[i+1].x,pts[i+1].y,pts[i+1].z,pt0->x,pt0->y,pt0->z,
			&x,&y,&z,false);
		
		if( min<0 || min>dis )
		{
			min = dis;
			ret.x = x, ret.y = y, ret.z = z;
			k = i;
		}
	}
	
	//计算路线长度作为x坐标
	dis = 0;
	for( i=0; i<k; i++)
	{
		dis += sqrt((pts[i].x-pts[i+1].x)*(pts[i].x-pts[i+1].x)+
			(pts[i].y-pts[i+1].y)*(pts[i].y-pts[i+1].y));
	}

	dis += sqrt((pts[k].x-ret.x)*(pts[k].x-ret.x)+(pts[k].y-ret.y)*(pts[k].y-ret.y));

	PT_3D pt2;
	pt2.x = dis*m_lfGScaleX; 
	pt2.y = pt0->z*m_lfGScaleZ; 
	pt2.z = 0;

	CVectorView_new::GroundToImage(&pt2,pt1);
}

void CEPSectionView::GroundToVectorLay(PT_3D *pt0, PT_4D *pt1)
{
	GroundToImage(pt0, pt1);
}


BOOL CEPSectionView::InitBmpLayer()
{
	return CVectorView_new::InitBmpLayer();
}


/*
void CEPSectionView::ClientToImage(PT_4D *pt0, PT_4D *pt1)
{
	PT_4D tpt1, tpt2;
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
	tpt2.z = pt0->z*m_lfRMatrixLic[0]+pt0->yr*m_lfRMatrixLic[1]+z2*m_lfRMatrixLic[2];
	tpt2.yr = pt0->z*m_lfRMatrixLic[3]+pt0->yr*m_lfRMatrixLic[4]+z2*m_lfRMatrixLic[5];
	
	*pt1 = tpt2;
	m_pLeftCS->ClientToGround(pt1,1);
}

void CEPSectionView::GroundToImage(PT_3D *pt0, PT_4D *pt1)
{
	if( !m_bViewBaseOnImage )
	{
		*pt1 = *pt0;
	}
	else
	{
		pt1->x = pt0->x*m_lfImgRMatrix[0] + pt0->y*m_lfImgRMatrix[1] + m_lfImgRMatrix[3];
		pt1->y = pt0->x*m_lfImgRMatrix[4] + pt0->y*m_lfImgRMatrix[5] + m_lfImgRMatrix[7];
		pt1->z = pt0->z;
	}
	pt1->yr = 0;
	
}

void CEPSectionView::ImageToClient(PT_4D *pt0, PT_4D *pt1)
{
	PT_4D tpt1, tpt2;
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
	tpt2.z = pt0->z*m_lfRMatrixLic[0]+pt0->yr*m_lfRMatrixLic[1]+z2*m_lfRMatrixLic[2];
	tpt2.yr = pt0->z*m_lfRMatrixLic[3]+pt0->yr*m_lfRMatrixLic[4]+z2*m_lfRMatrixLic[5];
	
	*pt1 = tpt2;
	m_pLeftCS->GroundToClient(pt1,1);
}

void CEPSectionView::ImageToGround(PT_4D *pt0, PT_3D *pt1)
{
	if( !m_bViewBaseOnImage )
	{
		*pt1 = pt0->To3D();
	}
	else
	{
		double x = (*pt0).x;
		double y = (*pt0).y;
		pt1->x = x*m_imgPos.lfImgMatrix[0] + y*m_imgPos.lfImgMatrix[1] + m_imgPos.lfImgLayOX;
		pt1->y = x*m_imgPos.lfImgMatrix[2] + y*m_imgPos.lfImgMatrix[3] + m_imgPos.lfImgLayOY;
		pt1->z = pt0->z;
	}
}

void CEPSectionView::GroundToVectorLay(PT_3D *pt0, PT_4D *pt1)
{
	GroundToImage(pt0, pt1);
}
*/
void CEPSectionView::OnMouseMove(UINT nFlags, CPoint point)
{
	CDlgDoc *pDoc = (CDlgDoc*)GetDocument();

	pDoc->m_snap.Enable(FALSE);

	if( m_nInnerCmd==-1 )m_nInnerCmd = 0;
	CVectorView_new::OnMouseMove(nFlags,point);

	if( m_nInnerCmd==0 )m_nInnerCmd = -1;
}


void CEPSectionView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CDlgDoc *pDoc = (CDlgDoc*)GetDocument();
	switch( lHint )
	{
	case hc_SetMapArea:
	case hc_SetRoadLine:
		{			
			//通过路线计算视图可视范围
			int num = m_arrRoadPts.GetSize();
			if( num<2 )break;
			
			//路线长度就是x范围，z范围就是y坐标范围
			//计算路线长度
			PT_3D *pts = m_arrRoadPts.GetData();
			double cur=0;
			for( int i=0; i<num-1; i++)
			{
				cur += sqrt((pts[i].x-pts[i+1].x)*(pts[i].x-pts[i+1].x)+
					(pts[i].y-pts[i+1].y)*(pts[i].y-pts[i+1].y));
			}

			//z范围
 			Envelope evlp;
			
			CDlgDataSource *pDS = pDoc->GetDlgDataSource();
			
			for ( i=0; i<pDS->GetFtrLayerCount(); i++)
			{
				CFtrLayer *pFtrLayer = pDS->GetFtrLayerByIndex(i);
				if (pFtrLayer == NULL) continue;
				
				int sum = pFtrLayer->GetObjectCount();
				for (int j=0; j<sum; j++)
				{
					CFeature *pFtr = pFtrLayer->GetObject(j);
					if (pFtr == NULL) continue;

					evlp.Union(&pFtr->GetGeometry()->GetEnvelope(),3);
				}
			}

// 			if( pDoc->m_bSetHeiRange )
// 			{
// 				evlp.m_zl = pDoc->m_fMinHei;
// 				evlp.m_zh = pDoc->m_fMaxHei;
// 			}

			if( cur<=1e-4 )cur = 1.0;
			if( evlp.m_zh-evlp.m_zl<1e-4 )evlp.m_zh += 1.0;
			
			m_ptBounds[0].x = pts[0].x;
			m_ptBounds[0].y = pts[0].y;
			m_ptBounds[0].z = evlp.m_zl;
			m_ptBounds[1].x = pts[num-1].x;
			m_ptBounds[1].y = pts[num-1].y;
			m_ptBounds[1].z = evlp.m_zl;
			m_ptBounds[2].x = pts[num-1].x;
			m_ptBounds[2].y = pts[num-1].y;
			m_ptBounds[2].z = evlp.m_zh;
			m_ptBounds[3].x = pts[0].x;
			m_ptBounds[3].y = pts[0].y;
			m_ptBounds[3].z = evlp.m_zh;
			
			evlp.CreateFromPts(m_ptBounds,4,3);

			CRect rcClient;
			GetClientRect(&rcClient);
			
			double sx = (float)rcClient.Width()/cur;
			double sy = (float)rcClient.Height()/(evlp.m_zh-evlp.m_zl);
			m_lfGScaleX = sx;
			m_lfGScaleZ = sy;
			
			m_xgoff = 0;
			m_ygoff = rcClient.Height();

			//m_lfGScale = 1.0;
			
			CalcImgSysParams(m_ptBounds);
			
			RecalcScrollBar(FALSE);
			
			//重新装载矢量数据
			InitBmpLayer();
			LoadDocData();

			//重新设定可视区域（设为当前滚动范围的区域）
			CSize szView = CalcViewSize();
			
			PT_3D pt0;
			PT_4D pt1,pt2;
			pt1.x = -GetViewMargin()/GetZoomRate(), pt1.y = -GetViewMargin()/GetZoomRate(); pt1.z = 0;
			ImageToGround(&pt1,&pt0);
			GroundToVectorLay(&pt0,&pt1);
			
			pt2.x = szView.cx+GetViewMargin()/GetZoomRate(), pt2.y = szView.cy+GetViewMargin()/GetZoomRate(); pt2.z = 0;
			ImageToGround(&pt2,&pt0);
			GroundToVectorLay(&pt0,&pt2);
			
			double xmin,xmax,ymin,ymax;
			xmin = pt1.x<pt2.x?pt1.x:pt2.x;
			xmax = pt1.x>pt2.x?pt1.x:pt2.x;
			ymin = pt1.y<pt2.y?pt1.y:pt2.y;
			ymax = pt1.y>pt2.y?pt1.y:pt2.y;
			
			m_vectLayL.SetDataBound(
				Envelope( xmin-1,xmax+1,ymin-1,ymax+1) );
			m_markLayL.SetDataBound(
				Envelope( xmin-1,xmax+1,ymin-1,ymax+1) );
			
			Invalidate(FALSE);
			UpdateWindow();
		}
		break;
	default: 
		CVectorView_new::OnUpdate(pSender,lHint,pHint);
		break;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CEPRoadView, CVectorView_new)

BEGIN_MESSAGE_MAP(CEPRoadView, CVectorView_new)
	//{{AFX_MSG_MAP(CEPSectionView)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CEPRoadView::CEPRoadView()
{
	
}

CEPRoadView::~CEPRoadView()
{
	
}


void CEPRoadView::OnInitialUpdate()
{
	CDlgDoc *pDoc = (CDlgDoc*)GetDocument();

	//寻找标记矩形，计算断面路由
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	
	CString strDefLineLayer;
	strDefLineLayer.LoadString(IDS_DEFLAYER_NAMEL);
	
	CFtrLayer *pFtrLayer = pDS->GetFtrLayer(strDefLineLayer);
	
	if (pFtrLayer != NULL)
	{
		int sum = pFtrLayer->GetObjectCount();
		for (int i=0; i<sum; i++)
		{
			CFeature *pFtr = pFtrLayer->GetObject(i);
			if (pFtr == NULL) continue;
			
			if (stricmp(pFtr->GetCode(),EPREGION_FTRCODE) == 0)
			{
				CGeometry *pGeo = pFtr->GetGeometry();
				CArray<PT_3DEX,PT_3DEX> pts;
				pGeo->GetShape(pts);
				
				if (pts.GetSize() >= 4)
				{
					m_fRoadWid = GraphAPI::GGet2DDisOf2P(pts[0],pts[3])/2;
					CArray<PT_3DEX,PT_3DEX> ret;
					ret.SetSize(2);
					
					if (GraphAPI::GGetParallelLine(pts.GetData(),2,-m_fRoadWid,ret.GetData()))
					{
						m_arrRoadPts.SetSize(2);
						for (int j=0; j<ret.GetSize(); j++)
						{
							COPY_3DPT(m_arrRoadPts[j],ret[j]);
						}
					}
				}
				
				break;
			}
		}
	}

	CVectorView_new::OnInitialUpdate();

// 	PT_3D pt3ds[4];
// 	memset(pt3ds,0,sizeof(pt3ds));
// 	pDS->GetBound(pt3ds,NULL,NULL);
// 	OnUpdate(NULL,hc_SetMapArea,(CObject*)pt3ds);
	
//	pDoc->UpdateRoadLines();
}

void CEPRoadView::OnDestroy() 
{
	CBaseView::OnDestroy();
}


void CEPRoadView::ImageToGround(PT_4D *pt0, PT_3D *pt1)
{
	if( m_arrRoadDis.GetSize()<=0 )return;
	
	CVectorView_new::ImageToGround(pt0,pt1);
	
	CDlgDoc *pDoc = (CDlgDoc*)GetDocument();
	PT_3D *pts = m_arrRoadPts.GetData();
	int num = m_arrRoadPts.GetSize(), i;
	
	//转换XY坐标
	double dis0 = pt1->x;
	double cur=0, dis;
	double *pdis = m_arrRoadDis.GetData();
	for( i=0; i<num-1; i++)
	{
		dis = pdis[i];
		if( cur+dis>=dis0 )break;
		
		cur += dis;
	}
	
	if( i>=num-1 )
	{
		i = num-2;
		cur -= pdis[i];
	}
	
	double vx = (pts[i+1].x-pts[i].x)/dis, vy = (pts[i+1].y-pts[i].y)/dis;
	double x = pts[i].x+(dis0-cur)*vx, y = pts[i].y+(dis0-cur)*vy;
	pt1->x = x - pt1->y*vy;
	pt1->y = y + pt1->y*vx;
}


void CEPRoadView::GroundToImage(PT_3D *pt0, PT_4D *pt1)
{
	CDlgDoc *pDoc = (CDlgDoc*)GetDocument();
	PT_3D *pts = m_arrRoadPts.GetData(), ret;
	int num = m_arrRoadPts.GetSize(), k = -1;

	if( m_arrRoadDis.GetSize()<=0 )return;
	
	//找到路线上的最近点
	double dis, min=-1,x,y,z;
	for( int i=0; i<num-1; i++)
	{
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
			pts[i+1].x,pts[i+1].y,pts[i+1].z,pt0->x,pt0->y,pt0->z,
			&x,&y,&z,false);
		
		if( min<0 || min>dis )
		{
			min = dis;
			ret.x = x, ret.y = y, ret.z = z;
			k = i;
		}
	}
	
	//计算旋转后的坐标: x为到首点长度，y为到基线距离，z不变
	dis = 0;
	double *pdis = m_arrRoadDis.GetData();
	int pos = GraphAPI::GGetPerpPointPosinLine(pts[k].x,pts[k].y,pts[k+1].x,pts[k+1].y,ret.x,ret.y);
	if( pos==-1 )
	{
		dis = -GraphAPI::GGetNearestDisOfPtToLine3D(pts[0].x,pts[0].y,pts[0].z,
			pts[1].x,pts[1].y,pts[1].z,pt0->x,pt0->y,pt0->z,
			&ret.x,&ret.y,&ret.z,true);
	}
	else if( pos==0 )
	{
		for( i=0; i<k; i++)dis += pdis[i];
		dis += sqrt((pts[k].x-ret.x)*(pts[k].x-ret.x)+(pts[k].y-ret.y)*(pts[k].y-ret.y));
	}
	else
	{
		for( i=0; i<k; i++)dis += pdis[i];
		dis += GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
			pts[i+1].x,pts[i+1].y,pts[i+1].z,pt0->x,pt0->y,pt0->z,
			&ret.x,&ret.y,&ret.z,true);
	}

	PT_3D pt2;
	pt2.x = dis; 
	pt2.y = sqrt((pt0->x-ret.x)*(pt0->x-ret.x)+(pt0->y-ret.y)*(pt0->y-ret.y));
	pt2.z = pt0->z;

	z = (pts[k+1].x-pts[k].x)*(pt0->y-pts[k+1].y)-(pt0->x-pts[k+1].x)*(pts[k+1].y-pts[k].y);
	if( z<0 )pt2.y = -pt2.y;
	
	CVectorView_new::GroundToImage(&pt2,pt1);
}

void CEPRoadView::GroundToVectorLay(PT_3D *pt0, PT_4D *pt1)
{
	GroundToImage(pt0, pt1);
}


void CEPRoadView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CDlgDoc *pDoc = (CDlgDoc*)GetDocument();
	switch( lHint )
	{
	case hc_SetMapArea:
	case hc_SetRoadLine:
		{			
			//通过路线计算视图可视范围
			int num = m_arrRoadPts.GetSize();
			if( num<2 )break;
		
			//计算路线长度
			m_arrRoadDis.SetSize(num-1);
			PT_3D *pts = m_arrRoadPts.GetData();
			double cur=0, all=0;
			for( int i=0; i<num-1; i++)
			{
				cur = sqrt((pts[i].x-pts[i+1].x)*(pts[i].x-pts[i+1].x)+
					(pts[i].y-pts[i+1].y)*(pts[i].y-pts[i+1].y));
				m_arrRoadDis.SetAt(i,cur);
				all += cur;
			}

			double dis = sqrt((pts[0].x-pts[1].x)*(pts[0].x-pts[1].x)+
				(pts[0].y-pts[1].y)*(pts[0].y-pts[1].y));
			double vx1 = (pts[1].x-pts[0].x)/dis, vy1 = (pts[1].y-pts[0].y)/dis;
			
			dis = sqrt((pts[num-2].x-pts[num-1].x)*(pts[num-2].x-pts[num-1].x)+
				(pts[num-2].y-pts[num-1].y)*(pts[num-2].y-pts[num-1].y));
			double vx2 = (pts[num-1].x-pts[num-2].x)/dis, vy2 = (pts[num-1].y-pts[num-2].y)/dis;

			double wid = m_fRoadWid*1.5;

			m_ptBounds[0].x = pts[0].x + vy1*wid;
			m_ptBounds[0].y = pts[0].y - vx1*wid;
			m_ptBounds[0].z = 0;
			m_ptBounds[1].x = pts[num-1].x + vy2*wid;
			m_ptBounds[1].y = pts[num-1].y - vx2*wid;
			m_ptBounds[1].z = 0;
			m_ptBounds[2].x = pts[num-1].x - vy2*wid;
			m_ptBounds[2].y = pts[num-1].y + vx2*wid;
			m_ptBounds[2].z = 0;
			m_ptBounds[3].x = pts[0].x - vy1*wid;
			m_ptBounds[3].y = pts[0].y + vx1*wid;
			m_ptBounds[3].z = 0;
			
			Envelope evlp;
			evlp.CreateFromPts(m_ptBounds,4);
			
			CRect rcClient;
			GetClientRect(&rcClient);
			
			double sx = (float)rcClient.Width()/cur;
			double sy = (float)rcClient.Height()/(wid*2);
			//m_lfGScale = sx<sy?sx:sy;
			
			m_xgoff = 0;
			m_ygoff = wid*2;
			
			CalcImgSysParams(m_ptBounds);
			
			RecalcScrollBar(FALSE);
			
			//重新装载矢量数据
			InitBmpLayer();
			LoadDocData();
			
			//重新设定可视区域（设为当前滚动范围的区域）
			CSize szView = CalcViewSize();
			
			PT_3D pt0;
			PT_4D pt1,pt2;
			pt1.x = -GetViewMargin()/GetZoomRate(), pt1.y = -GetViewMargin()/GetZoomRate(); pt1.z = 0;
			ImageToGround(&pt1,&pt0);
			GroundToVectorLay(&pt0,&pt1);
			
			pt2.x = szView.cx+GetViewMargin()/GetZoomRate(), pt2.y = szView.cy+GetViewMargin()/GetZoomRate(); pt2.z = 0;
			ImageToGround(&pt2,&pt0);
			GroundToVectorLay(&pt0,&pt2);
			
			double xmin,xmax,ymin,ymax;
			xmin = pt1.x<pt2.x?pt1.x:pt2.x;
			xmax = pt1.x>pt2.x?pt1.x:pt2.x;
			ymin = pt1.y<pt2.y?pt1.y:pt2.y;
			ymax = pt1.y>pt2.y?pt1.y:pt2.y;
			
			m_vectLayL.SetDataBound(
				Envelope( xmin-1,xmax+1,ymin-1,ymax+1) );
			m_markLayL.SetDataBound(
				Envelope( xmin-1,xmax+1,ymin-1,ymax+1) );
			
			Invalidate(FALSE);
			UpdateWindow();
		}
		break;
	default: CVectorView_new::OnUpdate(pSender,lHint,pHint);
	}
}

void CEPRoadView::DrawBkgnd(GrBuffer *buf)
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
				buf->BeginLineString(RGB(255,0,0),2,0);
			//GetDocument()->GetDlgDataSource(i)->GetBound(pts,NULL,NULL);
			memcpy(pts,m_ptBounds,sizeof(pts));
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