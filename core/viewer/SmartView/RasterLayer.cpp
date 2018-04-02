// RasterLayer.cpp: implementation of the CRasterLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RasterLayer.h"
#include "DummyDialog.h"
#include "resource.h"
#include "GlobalFunc.h"
#include "SmartViewFunctions.h"
#include "matrix.h"

#include "SmartViewLocalRes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern AFX_EXTENSION_MODULE SmartViewDLL;

MyNameSpaceBegin


IMPLEMENT_DYNAMIC(CRasterLayer,CDrawingLayer)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRasterLayer::CRasterLayer():m_image(),
m_bSkipRead(FALSE),m_nSkipReadRate(0),m_szImage(0,0)
{
	m_pDummyDialog = NULL;
	m_qThreadReadItem.blkidx = -1;
	m_bUseImageThread = TRUE;

	m_bUseCornerPts = FALSE;
	m_bCalcParam = FALSE;

	m_hBlockBmp = NULL;

	m_fKX = m_fKY = 1.0f;
	m_fRotAng = 0.0f;

	m_pFuncOwner = NULL;
	m_pFuncConvertCoord = NULL;

	m_bForceDraw = FALSE;

	m_bEnableLoadAll = FALSE;
	m_bLoadedAll = FALSE;

	m_bPauseLoadAll = FALSE;

	m_ccZoom = 0.0f;

	m_bNotClearCacheOnZoom = FALSE;

	InitializeCriticalSection(&m_critical);
}

CRasterLayer::~CRasterLayer()
{
	UnlinkImageThread();
	m_image.Close();
	Destroy();

	DeleteCriticalSection(&m_critical);
}

BOOL CRasterLayer::InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK)
{
	if (IsValid())  Destroy();

	if( nBlock==0 )
	{
		m_bEnableLoadAll = TRUE;		
		m_bLoadedAll = FALSE;
		m_bUseImageThread = FALSE;

		nBlock = 1;
	}
	else
	{
		m_bEnableLoadAll = FALSE;		
		m_bLoadedAll = FALSE;
		m_bUseImageThread = TRUE;
	}

	if( nCacheType==CHEDATA_TYPE_BMP )
	{
		CCacheBmpData *pCache = new CCacheBmpData;
		if( pCache!=NULL )
		{
			pCache->SetColorTable(24,NULL);
			pCache->EnableTransparentOverlap(TRUE);
			
			m_pCacheData = pCache;
			m_pCacheData->SetBkColor(m_pContext->GetBackColor());
		}
	}
	if( nCacheType==CHEDATA_TYPE_D3DTEXT )
	{
		CCacheD3DTextData *pCache = new CCacheD3DTextData;
		if( pCache!=NULL )
		{
			pCache->m_bUseAlpha = TRUE;
			pCache->SetD3D(m_pContext->m_pD3D,FALSE);
			m_pCacheData = pCache;
		}
	}
	
	return CDrawingLayer::InitCache(nCacheType, szBlock, nBlock, clrBK);
}


void CRasterLayer::PlanBGLoad()
{
	if( m_bEnableLoadAll && !m_bLoadedAll )
	{
		LoadAllBlocks();
		return;
	}
	
	if (!m_bEnableBGLoad && m_aReadQueue.GetSize()==0 && !CheckCacheReady())
	{
		m_bEnableBGLoad = TRUE;
		CDrawingLayer::PlanBGLoad();
		m_bEnableBGLoad = FALSE;
	}
	else
	{
		CDrawingLayer::PlanBGLoad();
	}
}

void CRasterLayer::ClearAll()
{
	m_bLoadedAll = FALSE;
	CDrawingLayer::ClearAll();
}

void CRasterLayer::Destroy()
{
	if( m_pDummyDialog )
	{
		delete m_pDummyDialog;
		m_pDummyDialog = NULL;
	}

	if( m_hBlockBmp!=NULL )
	{
		::DeleteObject(m_hBlockBmp);
		m_hBlockBmp = NULL;
	}

	CDrawingLayer::Destroy();
}

BOOL CRasterLayer::IsValidImage()
{
	return m_image.IsValid();
}

LPCTSTR CRasterLayer::GetImgFileName()
{
	return m_image.GetImageName();
}

CSize CRasterLayer::GetImageSize()
{
	return m_image.GetImageOriginalSize();
}

CSize CRasterLayer::GetVirtualImageSize()
{
	CSize szImage = m_image.GetImageOriginalSize();
	if( m_fRotAng==90 || m_fRotAng==270 )
		szImage = CSize(szImage.cy,szImage.cx);

	szImage.cx *= m_fKX;
	szImage.cy *= m_fKY;
	return szImage;
}

void CRasterLayer::SetUISCacheDir(LPCTSTR dir)
{
	m_strUISCacheDir = dir;
}

BOOL CRasterLayer::AttachImage(LPCTSTR fileName)
{
	UnlinkImageThread();
	m_image.Close();
	m_image.SetUISCacheDir(m_strUISCacheDir);

	if( !m_image.Load(fileName) )
		return FALSE;
	
	m_bSkipRead = FALSE;
	m_nSkipReadRate = 0;
	
	m_image.SetCurReadZoom(1.0);

	m_szImage = m_image.GetImageOriginalSize();

	if( m_hBlockBmp!=NULL )
	{
		::DeleteObject(m_hBlockBmp);
		m_hBlockBmp = NULL;
	}
	m_hBlockBmp = Create24BitDIB(m_szBlock.cx,m_szBlock.cy,TRUE,m_pContext->GetBackColor());
	
	if( m_bUseImageThread )
	{
		m_image.SetReadBmpInfo(m_szBlock.cx,m_szBlock.cy,24);
		m_image.InitReadThread();
		m_image.BeginReadThread();
		LinkImageThread();

		m_bEnableBGLoad = FALSE;
	}

	GetBlockNumRange(m_ccXmin,m_ccXmax,m_ccYmin,m_ccYmax);

	m_ccCache.RemoveAll();
	m_ccCache.SetSize((m_ccXmax-m_ccXmin+1)*(m_ccYmax-m_ccYmin+1));
	memset(m_ccCache.GetData(),0,sizeof(CoordConvertCache)*m_ccCache.GetSize());
	m_ccZoom = 1.0f;

	return TRUE;
}

void CRasterLayer::SetContext(CDrawingContext *pContext)
{
	if( m_hBlockBmp!=NULL )
	{
		::DeleteObject(m_hBlockBmp);
		m_hBlockBmp = NULL;
	}
	m_hBlockBmp = Create24BitDIB(m_szBlock.cx,m_szBlock.cy,TRUE,pContext==NULL?0:pContext->GetBackColor());
	CDrawingLayer::SetContext(pContext);
}


void CRasterLayer::LoadAllBlocks()
{
	if( !(m_bEnableLoadAll && !m_bLoadedAll) )
		return;

	if( m_bPauseLoadAll )
		return;

	if( !m_pCacheData )return;
	if( !m_image.IsValid() )return;

	CDialog dlg;
	dlg.Create(IDD_DIALOG1,AfxGetMainWnd());

	CRect rect, rect2;
	dlg.GetWindowRect(rect);
	AfxGetMainWnd()->GetWindowRect(rect2);
	
	rect.OffsetRect(rect2.CenterPoint().x-rect.CenterPoint().x,
		rect2.CenterPoint().y-rect.CenterPoint().y);
	dlg.MoveWindow(rect);
	dlg.ShowWindow(SW_SHOW);
	dlg.RedrawWindow();

	ClearAll();

	int xmin,xmax,ymin,ymax;
	GetBlockNumRange(xmin,xmax,ymin,ymax);

	int nBlock = (xmax-xmin+1)*(ymax-ymin+1);
	int nCacheType = m_pCacheData->GetType();
	CSize szBlock = m_pCacheData->GetBlockSize();

	if( nBlock>m_pCacheData->GetBlockCount() )
	{
		m_pContext->BeginDrawing();
		
		CCacheLoader::Destroy();
		
		if( nCacheType==CHEDATA_TYPE_BMP )
		{
			CCacheBmpData *pCache = new CCacheBmpData;
			if( pCache!=NULL )
			{
				pCache->SetColorTable(24,NULL);
				pCache->EnableTransparentOverlap(TRUE);
				
				m_pCacheData = pCache;
			}
		}	
		CCacheLoader::InitCache(nCacheType, szBlock,nBlock, m_pContext->GetBackColor());
		
		m_pContext->EndDrawing();
	}
	
	int index = 0;
	for( int i=xmin; i<=xmax; i++)
	{
		for( int j=ymin; j<=ymax; j++)
		{
			CacheID id(i,j);

			FillBlock(index,id);

			index++;
		}
	}

	m_bLoadedAll = TRUE;
}


void CRasterLayer::DetachImage()
{
	UnlinkImageThread();
	ClearAll();
	m_image.Close();
	m_qThreadReadItem.blkidx = -1;
	m_qThreadReadItem.id = CacheID();
}


Envelope CRasterLayer::GetDataBound()
{
	float zoom = GetActualZoomRate();

	PT_4D pts[4];
	pts[0].x = 0; pts[0].y = 0;
	pts[1].x = m_szImage.cx*zoom; pts[1].y = 0;
	pts[2].x = m_szImage.cx*zoom; pts[2].y = m_szImage.cy*zoom;
	pts[3].x = 0; pts[3].y = m_szImage.cy*zoom;
	
	ImageToGround(pts,4);

	Envelope evlp;
	evlp.CreateFromPts(pts,4,sizeof(PT_4D));
	return evlp;

	if( m_bUseCornerPts )
	{
		Envelope e;
		e.CreateFromPts(m_ptsCorner,4);
		return e;
	}
	return Envelope(0,m_szImage.cx,0,m_szImage.cy);
}


// 获得块编号的合理的取值范围
void CRasterLayer::GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax)
{
	CSize size = m_image.GetImageOriginalSize();
	xmin = ymin = 0;
	xmax = (size.cx-1)*GetActualZoomRate()/m_szBlock.cx;
	ymax = (size.cy-1)*GetActualZoomRate()/m_szBlock.cy;
}


// 获得焦点处所在的块编号
void CRasterLayer::GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum)
{
	PT_4D pt;
	pt.x = ptFocus.x; pt.y = ptFocus.y;
	
	m_pContext->GetCoordSys()->ClientToGround(&pt,1);
	GroundToImage(&pt,1);

	xnum = floor(pt.x/m_szBlock.cx);
	ynum = floor(pt.y/m_szBlock.cy);
}


// 获得块的四个角点坐标在客户坐标系下的XY坐标
void CRasterLayer::GetBlockClientXY(CacheID id, double x[4], double y[4])
{
	//加0.5 是要定位到像素中心
	PT_4D pts[4];
	pts[0].x = id.xnum*m_szBlock.cx; pts[0].y = id.ynum*m_szBlock.cy;
	pts[1].x = (id.xnum+1)*m_szBlock.cx; pts[1].y = id.ynum*m_szBlock.cy;
	pts[2].x = (id.xnum+1)*m_szBlock.cx; pts[2].y = (id.ynum+1)*m_szBlock.cy;
	pts[3].x = id.xnum*m_szBlock.cx; pts[3].y = (id.ynum+1)*m_szBlock.cy;

	if (id.zoom == 0.0f)
	{
		id.zoom = GetActualZoomRate();
	}

	m_idConvert = id;

	if(m_bNotClearCacheOnZoom)
		ImageToGroundWithZoom(pts, 4, id.zoom);
	else
		ImageToGround(pts, 4);
	m_idConvert = CacheID();

	m_pContext->GetCoordSys()->GroundToClient(pts, 4);

	x[0] = pts[0].x; y[0] = pts[0].y;
	x[1] = pts[1].x; y[1] = pts[1].y;
	x[2] = pts[2].x; y[2] = pts[2].y;
	x[3] = pts[3].x; y[3] = pts[3].y;

	if( m_pCacheData->GetType()==CHEDATA_TYPE_BMP )
	{
		x[0] += 0.5; y[0] += 0.5;
		x[1] += 0.5; y[1] += 0.5;
		x[2] += 0.5; y[2] += 0.5;
		x[3] += 0.5; y[3] += 0.5;
	}
}

void CRasterLayer::SetZoomRate(float zoomRate)
{
	if( m_bEnableLoadAll )
		return;

	m_image.SetReadRect(CRect(0,0,0,0));
	m_qThreadReadItem.blkidx = -1;
	m_qThreadReadItem.id = CacheID();

	float zoomold = m_image.GetCurReadZoom();
 
	int ret = m_image.SetCurReadZoom(zoomRate);
	if( ret&SETZOOM_VALID )
	{
		if(!m_bNotClearCacheOnZoom)ClearAll();

		m_bSkipRead = FALSE;
		m_nSkipReadRate = 0;
	}
	
	BOOL bSkipRead = m_bSkipRead;
	int nNewRate = m_nSkipReadRate;
	int zmin;
	m_image.GetParamidRange(&zmin,NULL);		

	float fzmin = 2.0f/(-zmin), fzmax = 1.0f;
	if( zoomRate>=fzmax )
	{
		bSkipRead = FALSE;
		nNewRate = 0;
	}
	else
	{
		float fk = zoomRate/fzmin;

		//[1,0.25) ->采用1比率读取; 
		if( fk>0.25 )
		{
			bSkipRead = FALSE;
			nNewRate = 0;
		}

		//[1/4,1/16) ->采用1/4比率读取; 
		//[1/16,1/64) ->采用1/16比率读取;依次类推
		else
		{
			int nk = 0.01 + 1.0/fk;

			bSkipRead = TRUE;
			for( int i=2; i<32 && (1<<i)<=nk; i+=2);
			nNewRate = (int)(1<<(i-2));
		}
	}

	//影像很小就不必重采样了
	CSize szImg = m_image.GetImageSize();
	float readzoom = GetActualZoomRate();
	if( m_pCacheData!=NULL && (double)szImg.cx*(double)szImg.cy*readzoom*readzoom<m_szBlock.cx*m_szBlock.cy*(double)m_pCacheData->GetBlockCount() )
	{
		if( !m_bSkipRead )
		{
			//不需要切换读取比率，直接返回
			return;
		}
		else if( m_bSkipRead && !bSkipRead )
		{
			//需要切换读取比率以读取更高精度的影像，这里不作操作，到后面的代码中切换；
		}
		else if( m_bSkipRead && bSkipRead )
		{
			//不需要切换读取比率，直接返回
			if( nNewRate>=m_nSkipReadRate )
			{
				return;
			}
			//需要切换读取比率以读取更高精度的影像
			else
			{
			}
		}
	}

	if( bSkipRead!=m_bSkipRead || nNewRate!=m_nSkipReadRate )
	{
		if(!m_bNotClearCacheOnZoom)ClearAll();
		m_bSkipRead = bSkipRead;
		m_nSkipReadRate = nNewRate;
	}
}


void CRasterLayer::OnChangeCoordSys(BOOL bJustScrollScreen)
{
	if( !bJustScrollScreen )
	{
		float zoomRate = m_pContext->GetCoordSys()->CalcScale();
		zoomRate *= max(m_fKX,m_fKY);
		SetZoomRate(zoomRate);
	}
}

void CRasterLayer::SetCornerPts(BOOL bUseCorner, PT_3D* pts)
{
	m_bUseCornerPts = bUseCorner;
	m_bCalcParam = FALSE;

	if( bUseCorner )
	{
		memcpy(m_ptsCorner,pts,sizeof(m_ptsCorner));

		double dis1 = DIST_3DPT(pts[0],pts[1]);
		double dis2 = DIST_3DPT(pts[1],pts[2]);

		CSize szImage = m_image.GetImageOriginalSize();
		
		m_fKX = dis1/szImage.cx;
		m_fKY = dis2/szImage.cy;

		OnChangeCoordSys(FALSE);
	}
}


void CRasterLayer::SetCornerPts(float ang, float fKX, float fKY)
{
	m_bUseCornerPts = TRUE;
	m_bCalcParam = FALSE;
	
	{
		
		CSize szImage = m_image.GetImageOriginalSize();

		m_fRotAng = ang;
		
		if (m_pCacheData && m_pCacheData->GetType() == CHEDATA_TYPE_BMP)
		{
			((CCacheBmpData*)m_pCacheData)->SetRotateAndZoom(m_fRotAng,1,1);
		}

		{
			double x = szImage.cx, y = szImage.cy;
			
			//顺时针旋转
			if( ang==90 )
			{
				m_ptsCorner[0].x = 0; m_ptsCorner[0].y = x; m_ptsCorner[0].z = 0;
				m_ptsCorner[1].x = 0; m_ptsCorner[1].y = 0; m_ptsCorner[1].z = 0;
				m_ptsCorner[2].x = y; m_ptsCorner[2].y = 0; m_ptsCorner[2].z = 0;
				m_ptsCorner[3].x = y; m_ptsCorner[3].y = x; m_ptsCorner[3].z = 0;
			}
			else if( ang==180 )
			{
				m_ptsCorner[0].x = x; m_ptsCorner[0].y = y; m_ptsCorner[0].z = 0;
				m_ptsCorner[1].x = 0; m_ptsCorner[1].y = y; m_ptsCorner[1].z = 0;
				m_ptsCorner[2].x = 0; m_ptsCorner[2].y = 0; m_ptsCorner[2].z = 0;
				m_ptsCorner[3].x = x; m_ptsCorner[3].y = 0; m_ptsCorner[3].z = 0;
			}
			//垂直翻转
			else if( ang==-180 )
			{
				m_ptsCorner[0].x = 0; m_ptsCorner[0].y = y; m_ptsCorner[0].z = 0;
				m_ptsCorner[1].x = x; m_ptsCorner[1].y = y; m_ptsCorner[1].z = 0;
				m_ptsCorner[2].x = x; m_ptsCorner[2].y = 0; m_ptsCorner[2].z = 0;
				m_ptsCorner[3].x = 0; m_ptsCorner[3].y = 0; m_ptsCorner[3].z = 0;
			}
			else if( ang==270 )
			{
				m_ptsCorner[0].x = y; m_ptsCorner[0].y = 0; m_ptsCorner[0].z = 0;
				m_ptsCorner[1].x = y; m_ptsCorner[1].y = x; m_ptsCorner[1].z = 0;
				m_ptsCorner[2].x = 0; m_ptsCorner[2].y = x; m_ptsCorner[2].z = 0;
				m_ptsCorner[3].x = 0; m_ptsCorner[3].y = 0; m_ptsCorner[3].z = 0;
			}
			else
			{
				m_ptsCorner[0].x = 0; m_ptsCorner[0].y = 0; m_ptsCorner[0].z = 0;
				m_ptsCorner[1].x = x; m_ptsCorner[1].y = 0; m_ptsCorner[1].z = 0;
				m_ptsCorner[2].x = x; m_ptsCorner[2].y = y; m_ptsCorner[2].z = 0;
				m_ptsCorner[3].x = 0; m_ptsCorner[3].y = y; m_ptsCorner[3].z = 0;
			}

			for( int i=0; i<4; i++)
			{
				m_ptsCorner[i].x *= fKX;
				m_ptsCorner[i].y *= fKY;
			}
		}
		
		m_fKX = fKX;
		m_fKY = fKY;

		OnChangeCoordSys(FALSE);
	}
	if( fKX==1.0f && fKY==1.0f && ang==0.0f )
	{
		m_bUseCornerPts = FALSE;
	}
}


void CRasterLayer::CalcAffineParm()
{
	//计算仿射关系
	double x[4], y[4], x2[4], y2[4];

	x[0] = m_ptsCorner[0].x; y[0] = m_ptsCorner[0].y; 
	x[1] = m_ptsCorner[1].x; y[1] = m_ptsCorner[1].y; 
	x[2] = m_ptsCorner[2].x; y[2] = m_ptsCorner[2].y; 
	x[3] = m_ptsCorner[3].x; y[3] = m_ptsCorner[3].y; 

	x2[0] = 0; y2[0] = 0; 
	x2[1] = m_szImage.cx; y2[1] = 0; 
	x2[2] = m_szImage.cx; y2[2] = m_szImage.cy; 
	x2[3] = 0; y2[3] = m_szImage.cy; 

	CalcAffineParams(x,y,x2,y2,4,m_lfA,m_lfB);

	m_bCalcParam = TRUE;
}


void CRasterLayer::SetTransform(double m[9])
{	
	double m2[9];
	matrix_reverse(m,3,m2);

	m_fKX = m_fKY = sqrt(fabs(m[0]*m[4]-m[1]*m[3]));
	m_lfA[0] = m2[0]; m_lfA[1] = m2[1]; m_lfA[2] = m2[2]; 
	m_lfB[0] = m2[3]; m_lfB[1] = m2[4]; m_lfB[2] = m2[5]; 
	
	OnChangeCoordSys(FALSE);
}


void CRasterLayer::GroundToImage(PT_4D *pts, int num)
{
	if( m_bUseCornerPts )
	{
		if( !m_bCalcParam )
			CalcAffineParm();

		PT_4D pt;
		for( int i=0; i<num; i++)
		{
			pt = pts[i];
			pts[i].x = pt.x*m_lfA[0] + pt.y*m_lfA[1] + m_lfA[2];
			pts[i].y = pt.x*m_lfB[0] + pt.y*m_lfB[1] + m_lfB[2];
		}
	}
	else
	{
	}

	if( m_pFuncConvertCoord )
	{
		PT_4D pt;
		for( int i=0; i<num; i++)
		{
			pt = pts[i];
			(m_pFuncOwner->*m_pFuncConvertCoord)(pt.x,pt.y,pts[i].x,pts[i].y,FALSE);
		}
	}

	float zoom = GetActualZoomRate();
	
	for( int i=0; i<num; i++)
	{
		pts[i].x = pts[i].x*zoom;
		pts[i].y = pts[i].y*zoom;
	}
}


void CRasterLayer::ImageToGround(PT_4D *pts, int num)
{
	float zoom = GetActualZoomRate();

	//首先使用坐标换算表中的数据
	BOOL bConvertOK = FALSE;
	if( m_pFuncConvertCoord && num==4 && m_idConvert.IsNotInValid() )
	{
		if( m_ccZoom!=zoom )
		{
			GetBlockNumRange(m_ccXmin,m_ccXmax,m_ccYmin,m_ccYmax);
			
			m_ccCache.RemoveAll();
			m_ccCache.SetSize((m_ccXmax-m_ccXmin+1)*(m_ccYmax-m_ccYmin+1));
			memset(m_ccCache.GetData(),0,sizeof(CoordConvertCache)*m_ccCache.GetSize());
			
			m_ccZoom = zoom;
		}

		int idx = m_idConvert.ynum*(m_ccXmax-m_ccXmin+1) + m_idConvert.xnum;
		if( idx>=0 && idx<m_ccCache.GetSize() )
		{
			if( m_ccCache[idx].isvalid )
			{
				memcpy(pts,m_ccCache[idx].pts,sizeof(PT_4D)*4);
			}
			else
			{
				for( int i=0; i<num; i++)
				{
					pts[i].x = pts[i].x/zoom;
					pts[i].y = pts[i].y/zoom;
				}
				
				if( m_pFuncConvertCoord )
				{
					PT_4D pt;
					for( int i=0; i<num; i++)
					{
						pt = pts[i];
						(m_pFuncOwner->*m_pFuncConvertCoord)(pt.x,pt.y,pts[i].x,pts[i].y,TRUE);
					}
				}
				
				memcpy(m_ccCache[idx].pts,pts,sizeof(PT_4D)*4);
				m_ccCache[idx].isvalid = 1;
			}
			
			bConvertOK = TRUE;
		}
	}

	if( !bConvertOK )
	{
		for( int i=0; i<num; i++)
		{
			pts[i].x = pts[i].x/zoom;
			pts[i].y = pts[i].y/zoom;
		}
		
		if( m_pFuncConvertCoord )
		{
			PT_4D pt;
			for( int i=0; i<num; i++)
			{
				pt = pts[i];
				(m_pFuncOwner->*m_pFuncConvertCoord)(pt.x,pt.y,pts[i].x,pts[i].y,TRUE);
			}
		}
	}

	if( m_bUseCornerPts )
	{
		if( !m_bCalcParam )
			CalcAffineParm();

		double m[9] = {
			m_lfA[0],m_lfA[1],m_lfA[2],
			m_lfB[0],m_lfB[1],m_lfB[2],
			0,0,1
		};

		double m2[9];
		matrix_reverse(m,3,m2);
		
		PT_4D pt;
		for( int i=0; i<num; i++)
		{
			pt = pts[i];
			pts[i].x = pt.x*m2[0] + pt.y*m2[1] + m2[2];
			pts[i].y = pt.x*m2[3] + pt.y*m2[4] + m2[5];
		}
	}
	else
	{
	}

}




void CRasterLayer::ImageToGroundWithZoom(PT_4D *pts, int num, float zoom)
{
	if (zoom <= 0)
		return;

	if (1)
	{
		for (int i = 0; i<num; i++)
		{
			pts[i].x = pts[i].x / zoom;
			pts[i].y = pts[i].y / zoom;
		}

		if (m_pFuncConvertCoord)
		{
			PT_4D pt;
			for (int i = 0; i<num; i++)
			{
				pt = pts[i];
				(m_pFuncOwner->*m_pFuncConvertCoord)(pt.x, pt.y, pts[i].x, pts[i].y, TRUE);
			}
		}
	}

	if (m_bUseCornerPts)
	{
		if (!m_bCalcParam)
			CalcAffineParm();

		double m[9] = {
			m_lfA[0], m_lfA[1], m_lfA[2],
			m_lfB[0], m_lfB[1], m_lfB[2],
			0, 0, 1
		};

		double m2[9];
		matrix_reverse(m, 3, m2);

		PT_4D pt;
		for (int i = 0; i<num; i++)
		{
			pt = pts[i];
			pts[i].x = pt.x*m2[0] + pt.y*m2[1] + m2[2];
			pts[i].y = pt.x*m2[3] + pt.y*m2[4] + m2[5];
		}
	}
	else
	{
	}

}


void CRasterLayer::DrawClient(double xl, double xh, double yl, double yh)
{
	PT_4D pts[4];

	//计算与客户坐标相对应的影像区域
	memset(pts,0,sizeof(pts));
	pts[0].x = xl, pts[0].y = yl, pts[0].z = 0;
	pts[1].x = xh, pts[1].y = yl, pts[1].z = 0;
	pts[2].x = xh, pts[2].y = yh, pts[2].z = 0;
	pts[3].x = xl, pts[3].y = yh, pts[3].z = 0;

	m_pContext->GetCoordSys()->ClientToGround(pts,4);
	GroundToImage(pts,4);

	Envelope e;
	e.CreateFromPts(pts,4,sizeof(PT_4D));

	CRect rcImage(floor(e.m_xl),floor(e.m_yl),ceil(e.m_xh),ceil(e.m_yh));
	CRect rcClient(floor(xl),floor(yl),ceil(xh),ceil(yh));
	
	//设置可视区域
	CRect rcViewSave;
	m_pContext->GetViewRect(rcViewSave);
	m_pContext->SetViewRect(rcClient);

	//绘图
	double x[4] = {xl,xh,xh,xl};
	double y[4] = {yl,yl,yh,yh};

	double x0[4], y0[4];

	for( int i=0; i<4; i++)
	{
		x0[i] = pts[i].x - rcImage.left;
		y0[i] = pts[i].y - rcImage.top;
	}

	HBITMAP hBmp = Create24BitDIB(rcImage.Width(),rcImage.Height());
	if( !hBmp )return;
	if( m_image.ReadRectBmp(rcImage,hBmp,m_bSkipRead,m_nSkipReadRate) )
	{
		CRect rcRead = rcImage;
		CRect rcRange = CRect(CPoint(0,0),m_image.GetImageSize());
		CRect rcValid = rcRead&rcRange;
		
		rcValid.OffsetRect(-rcRead.left,-rcRead.top);
		
// 		int t = rcValid.top;
// 		rcValid.top = rcImage.Height()-rcValid.bottom;
// 		rcValid.bottom = rcImage.Height()-t;
		
		CalcImageAdjust(hBmp);
		AdjustBitmap(hBmp,rcValid,&m_Adjust,FALSE,
			m_pContext->GetBackColor(),m_pContext->GetRGBMask());

		SetBitmapBKColor(hBmp,rcValid,m_pContext->GetBackColor());

		m_pContext->DrawBitmap(hBmp,CRect(0,0,rcImage.Width(),rcImage.Height()),x0,y0,x,y);
	}
	
	::DeleteObject(hBmp);

	//恢复可视区域
	m_pContext->SetViewRect(rcViewSave);
}



void CRasterLayer::LinkImageThread()
{
	CSmartViewLocalRes res;

	UnlinkImageThread();
	m_pDummyDialog = new CDummyDialog(this, (PFUNCALLBACK)&CRasterLayer::OnThreadMsg);
	((CDummyDialog*)m_pDummyDialog)->Create2((HINSTANCE)SmartViewDLL.hModule,IDD_DUMMY);
	m_pDummyDialog->ShowWindow(SW_HIDE);
	m_image.SetThreadCallback(m_pDummyDialog->GetSafeHwnd(),WM_THREADMSG);
}


void CRasterLayer::UnlinkImageThread()
{
	m_image.SetThreadCallback(NULL,NULL);
	if( m_pDummyDialog )
	{
		m_pDummyDialog->DestroyWindow();
		delete m_pDummyDialog;
	}
	m_pDummyDialog = NULL;
}


float CRasterLayer::GetActualZoomRate()
{
	if( m_bSkipRead )
		return (m_image.GetCurReadZoom()/m_nSkipReadRate);

	return m_image.GetCurReadZoom();
}

LRESULT CRasterLayer::OnThreadMsg(WPARAM wParam, LPARAM lParam)
{
	if( !m_image.IsValid() || !m_image.IsReadThreadStarted() )return 1;
	if( !m_bUseImageThread )return 1;
	
	// the thread finished reading a block
	if( wParam==1 )
	{
		if( m_qThreadReadItem.blkidx>=0 && m_qThreadReadItem.blkidx<m_cacheTable.nsum )
		{
			if( m_qThreadReadItem.id.bvalid==2 )m_qThreadReadItem.id.bvalid = 1;
			if( GetBlockIdx(m_qThreadReadItem.id)<0 )
			{
				TRACE("OnReadFinish %d\n",m_qThreadReadItem.blkidx);

				FillBlockWithImage(m_qThreadReadItem.blkidx,m_qThreadReadItem.id);
//				m_image.GetReadBmp(((CCacheBmpData*)m_pCacheData)->GetBlockBitmap(m_qThreadReadItem.blkidx,NULL));
//				CDrawingLayer::FillBlock(m_qThreadReadItem.blkidx,m_qThreadReadItem.id);
			}
		}
	}
	// the thread has no block to read
	else
	{
	}
/*
	{
		DWORD time0 = GetTickCount();
		for( int i=0; i<1000; i++)
		{
			ClearQueue();
			
			m_bEnableBGLoad = TRUE;
			CDrawingLayer::PlanBGLoad();
			m_bEnableBGLoad = FALSE;
		}

		DWORD time1 = GetTickCount()-time0;
	}
*/
	ClearQueue();

	m_bEnableBGLoad = TRUE;
	CDrawingLayer::PlanBGLoad();
	m_bEnableBGLoad = FALSE;

	if( m_aReadQueue.GetSize()>0 )
	{
		ReadQueue q = m_aReadQueue.GetAt(m_aReadQueue.GetSize()-1);
		if( q.id.bvalid!=0 && q.blkidx>=0 && q.blkidx<m_cacheTable.nsum )
		{
			CRect rc(q.id.xnum*m_szBlock.cx,q.id.ynum*m_szBlock.cy,
				(q.id.xnum+1)*m_szBlock.cx,(q.id.ynum+1)*m_szBlock.cy );
			
			if( m_bSkipRead )
			{
				rc.left *= m_nSkipReadRate;  rc.right *= m_nSkipReadRate;
				rc.top *= m_nSkipReadRate;  rc.bottom *= m_nSkipReadRate;
			}

			CSize szImg = GetImageSize();
			
			m_image.SetReadRect(rc,m_bSkipRead,m_nSkipReadRate);
			m_qThreadReadItem = q;
		}
	}
	else
	{
		m_image.SetReadRect(CRect(0,0,0,0));
		m_qThreadReadItem.blkidx = -1;
		m_qThreadReadItem.id = CacheID();
	}
	
	return 0;
}


void CRasterLayer::ClearImageAdjust()
{
	m_Adjust = CImageAdjust();
}


void CRasterLayer::CalcImageAdjust(HBITMAP hBmp)
{
	if( !m_Adjust.bAdjustMap )
	{
		m_Adjust.StatHistogramInfo(hBmp);
		m_Adjust.CalcAdjustMap();
	}
}


BOOL CRasterLayer::FillBlock(int blkIdx, CacheID id)
{
	if( m_pContext==NULL || m_pCacheData==NULL )
		return FALSE;
	
	//获取影像的角点坐标	
	double xl = id.xnum*m_szBlock.cx, yl = id.ynum*m_szBlock.cy;
	double xh = xl + m_szBlock.cx, yh = yl + m_szBlock.cy;
	
	//未使用xy坐标值
	double x[4] = {0,0,0,0};
	double y[4] = {0,0,0,0};
	
	if( m_image.ReadRectBmp(CRect(xl,yl,xh,yh),m_hBlockBmp,m_bSkipRead,m_nSkipReadRate) )
	{
		CRect rcRead = CRect(xl,yl,xh,yh);
		CRect rcImage = CRect(CPoint(0,0),m_image.GetImageOriginalSize());
		rcImage.right = ceil(rcImage.right*GetActualZoomRate());
		rcImage.bottom = ceil(rcImage.bottom*GetActualZoomRate());
		CRect rcValid = rcRead&rcImage;

		rcValid.OffsetRect(-rcRead.left,-rcRead.top);

//  	int t = rcValid.top;
//  	rcValid.top = m_szBlock.cy-rcValid.bottom;
//  	rcValid.bottom = m_szBlock.cy-t;

		CalcImageAdjust(m_hBlockBmp);
		AdjustBitmap(m_hBlockBmp,rcValid,&m_Adjust,FALSE,
			m_pContext->GetBackColor(),m_pContext->GetRGBMask());

		SetBitmapBKColor(m_hBlockBmp,rcValid,m_pContext->GetBackColor());

		EnterCriticalSection(&m_critical);
		m_pCacheData->BeginFillBlock(m_pContext,blkIdx);
		m_pCacheData->DrawBitmap(m_hBlockBmp,rcValid,rcValid);
		m_pCacheData->SetFillBlockRect(rcValid);
		m_pCacheData->EndFillBlock();
		LeaveCriticalSection(&m_critical);
	}
	else return FALSE;
	
	return CDrawingLayer::FillBlock(blkIdx,id);
}


// 填充块的内容
BOOL CRasterLayer::FillBlockWithImage(int blkIdx, CacheID id)
{
	if( m_pContext==NULL || m_pCacheData==NULL )
		return FALSE;

	//获取影像的角点坐标
	double xl = id.xnum*m_szBlock.cx, yl = id.ynum*m_szBlock.cy;
	double xh = xl + m_szBlock.cx, yh = yl + m_szBlock.cy;
	
	//未使用xy坐标值
	double x[4] = {0,0,0,0};
	double y[4] = {0,0,0,0};
	
	if( m_image.GetReadBmp(m_hBlockBmp) )
	{
		CRect rcRead = CRect(xl,yl,xh,yh);
		CRect rcImage = CRect(CPoint(0,0),m_image.GetImageOriginalSize());
		rcImage.right = ceil(rcImage.right*GetActualZoomRate());
		rcImage.bottom = ceil(rcImage.bottom*GetActualZoomRate());
		CRect rcValid = rcRead&rcImage;
		
		rcValid.OffsetRect(-rcRead.left,-rcRead.top);

// 		int t = rcValid.top;
// 		rcValid.top = m_szBlock.cy-rcValid.bottom;
// 		rcValid.bottom = m_szBlock.cy-t;

		CalcImageAdjust(m_hBlockBmp);
		AdjustBitmap(m_hBlockBmp,rcValid,&m_Adjust,FALSE,
			m_pContext->GetBackColor(),m_pContext->GetRGBMask());

		SetBitmapBKColor(m_hBlockBmp,rcValid,m_pContext->GetBackColor());

		EnterCriticalSection(&m_critical);
		m_pCacheData->BeginFillBlock(m_pContext,blkIdx);
		m_pCacheData->DrawBitmap(m_hBlockBmp,rcValid,rcValid);
		m_pCacheData->SetFillBlockRect(rcValid);
		m_pCacheData->EndFillBlock();
		LeaveCriticalSection(&m_critical);
	}
	else return FALSE;
	
	return CDrawingLayer::FillBlock(blkIdx,id);
}


static float GetZoomDiff(float z1, float z2)
{
	if (z2 > z1)
	{
		return (1 + z2 / z1) * 1000000;
	}
	if (z2 < z1)
	{
		return (1 + z1 / z2) * 1000000;
	}

	return 1.0f;
}

BOOL CRasterLayer::FindFreeBlock(int* pBlkIdx, CacheID* pid)
{
	// validity
	if (!IsValid())return FALSE;

	CPoint ptFocus = m_ptViewFocus;

	int maxnum = -1;
	double dx, dy, dis, maxdis = -1, mindis = -1;
	double x[4], y[4];
	int blkIdx = -1, i, j;
	CacheID tc;

	//最远的调入到cache中的影像块ID
	CacheID farest_id;

	int xfocus, yfocus;
	int nxmin = 0, nxmax = 0, nymin = 0, nymax = 0;

	GetFocusBlockNum(ptFocus, xfocus, yfocus);
	//获取数据的块范围
	GetBlockNumRange(nxmin, nxmax, nymin, nymax);

	{
		int view_x1 = 0, view_x2 = 0, view_y1 = 0, view_y2 = 0, view_x3 = 0, view_y3 = 0, view_x4 = 0, view_y4 = 0;

		CRect rcView;
		m_pContext->GetViewRect(rcView);

		//获取当前视图所覆盖的块范围
		GetFocusBlockNum(rcView.TopLeft(), view_x1, view_y1);
		GetFocusBlockNum(rcView.BottomRight(), view_x2, view_y2);
		CPoint pt(rcView.right, rcView.top);
		GetFocusBlockNum(pt, view_x3, view_y3);
		pt.x = rcView.left;
		pt.y = rcView.bottom;
		GetFocusBlockNum(pt, view_x4, view_y4);

		//可能坐标系是反的，这时会导致 view_x1>view_x2 或者  view_y1>view_y2
		if (view_x1 > view_x2)
		{
			int mint, maxt;
			mint = view_x2;
			maxt = view_x1;
			if (view_x3<mint)mint = view_x3;
			if (view_x3>maxt)maxt = view_x3;
			if (view_x4<mint)mint = view_x4;
			if (view_x4>maxt)maxt = view_x4;
			view_x1 = mint; view_x2 = maxt;
		}
		else
		{
			int mint, maxt;
			mint = view_x1;
			maxt = view_x2;
			if (view_x3<mint)mint = view_x3;
			if (view_x3>maxt)maxt = view_x3;
			if (view_x4<mint)mint = view_x4;
			if (view_x4>maxt)maxt = view_x4;
			view_x1 = mint; view_x2 = maxt;
		}

		if (view_y1 > view_y2)
		{
			int mint, maxt;
			mint = view_y2;
			maxt = view_y1;
			if (view_y3<mint)mint = view_y3;
			if (view_y3>maxt)maxt = view_y3;
			if (view_y4<mint)mint = view_y4;
			if (view_y4>maxt)maxt = view_y4;
			view_y1 = mint; view_y2 = maxt;
		}
		else
		{
			int mint, maxt;
			mint = view_y1;
			maxt = view_y2;
			if (view_y3<mint)mint = view_y3;
			if (view_y3>maxt)maxt = view_y3;
			if (view_y4<mint)mint = view_y4;
			if (view_y4>maxt)maxt = view_y4;
			view_y1 = mint; view_y2 = maxt;
		}

		//取相交区域
		nxmin = max(nxmin, view_x1);
		nxmax = min(nxmax, view_x2);
		nymin = max(nymin, view_y1);
		nymax = min(nymax, view_y2);
	}

	int diam = (int)ceil(sqrt((double)m_cacheTable.nsum)) + 2;

	CRect rctest(nxmin, nymin, nxmax+1, nymax+1);

	int testwid = rctest.Width(), testhei = rctest.Height();

	if (testwid <= 0 || testhei <= 0)
		return FALSE;

	//影像块是否已经在cache中的标记数组
	int *hitflags = new int[testwid*testhei];
	if (hitflags == NULL)return FALSE;

	memset(hitflags, 0, sizeof(int)*testwid*testhei);

	float curzoom = GetActualZoomRate();

	int do_count = 0;

	//查找空块或者最远的块
	for (i = 0; i < m_cacheTable.nsum; i++)
	{
		tc = m_cacheTable.ids[i];
		if (tc.IsNotInValid())
		{
			//此块在判断区域内部
			if (tc.zoom == curzoom &&
				tc.xnum >= rctest.left && tc.xnum < rctest.right &&
				tc.ynum >= rctest.top && tc.ynum < rctest.bottom)
			{
				hitflags[(tc.ynum - rctest.top)*testwid + tc.xnum - rctest.left] = 1;
			}
		}
		else
		{
			blkIdx = i;
		}
	}

	if (blkIdx == -1)
	{
		for (i = 0; i < m_cacheTable.nsum; i++)
		{
			tc = m_cacheTable.ids[i];
			if (tc.IsNotInValid())
			{
				//计算块中心到当前焦点的距离
				GetBlockClientXY(tc, x, y);
				do_count++;

				x[0] = (x[0] + x[1] + x[2] + x[3])*0.25;
				y[0] = (y[0] + y[1] + y[2] + y[3])*0.25;

				dx = (x[0] - ptFocus.x);
				dy = (y[0] - ptFocus.y);
				dis = dx*dx + dy*dy;

				dis = dis * GetZoomDiff(curzoom, tc.zoom);

				if (dis > maxdis)
				{
					maxdis = dis;
					maxnum = i;
					farest_id = tc;
				}
			}
		}
	}

	//没有空块，就找最远的块
	if (blkIdx == -1)blkIdx = maxnum;
	else farest_id = CacheID();

	if (pBlkIdx != NULL)*pBlkIdx = blkIdx;
	if (pid == NULL)
	{
		delete[] hitflags;
		return TRUE;
	}

	//查找最近的没有调入的块
	pid->bvalid = CacheID::stateInvalid;
	for (i = rctest.top; i<rctest.bottom; i++)
	{
		if (i<nymin || i>nymax)continue;
		for (j = rctest.left; j<rctest.right; j++)
		{
			if (j<nxmin || j>nxmax)continue;

			//找到最近的不在cache中的影像块号
			if (hitflags[(i - rctest.top)*testwid + j - rctest.left] != 1)
			{
				//计算块中心到当前焦点的距离
				tc.xnum = j; tc.ynum = i;
				tc.zoom = curzoom;
				GetBlockClientXY(tc, x, y);

				x[0] = (x[0] + x[1] + x[2] + x[3])*0.25;
				y[0] = (y[0] + y[1] + y[2] + y[3])*0.25;

				dx = (x[0] - ptFocus.x);
				dy = (y[0] - ptFocus.y);
				dis = dx*dx + dy*dy;
				if (mindis<0 || dis<mindis)
				{
					mindis = dis;
					pid->Set(j, i, CacheID::stateLoading);
				}
			}
		}
	}

	delete[] hitflags;
	if (pid->bvalid == CacheID::stateInvalid)return FALSE;

	//计算最远的块的中心到当前焦点的距离
	if (0 && farest_id.bvalid != CacheID::stateInvalid && farest_id.zoom==curzoom)
	{
		GetBlockClientXY(farest_id, x, y);

		x[0] = (x[0] + x[1] + x[2] + x[3])*0.25;
		y[0] = (y[0] + y[1] + y[2] + y[3])*0.25;

		dx = (x[0] - ptFocus.x);
		dy = (y[0] - ptFocus.y);

		double d1 = dx*dx + dy*dy;

		//计算命中的块的中心到当前焦点的距离
		GetBlockClientXY(*pid, x, y);

		x[0] = (x[0] + x[1] + x[2] + x[3])*0.25;
		y[0] = (y[0] + y[1] + y[2] + y[3])*0.25;

		dx = (x[0] - ptFocus.x);
		dy = (y[0] - ptFocus.y);

		if (m_bLastFindFreeBlockFull)
		{
			int limit = 1024 / m_szBlock.cx;
			//如果命中块在当前块的 1/4 的核心区域外，那我们可以暂时不去调度它
			if (fabs((double)(pid->xnum - xfocus)) >= limit || fabs((double)(pid->ynum - yfocus)) >= limit)
				return FALSE;
		}

		double d2 = dx*dx + dy*dy;

		//如果最远的调入到cache的影像块比最近的没有调入到cache的影像块还要远，
		//那么可以认为现在的cache是理想的，不需要再调度了；
		if (farest_id.bvalid != CacheID::stateInvalid && (d1*0.8)<d2)
		{
			m_bLastFindFreeBlockFull = TRUE;
			return FALSE;
		}
		m_bLastFindFreeBlockFull = FALSE;
	}

	return TRUE;
}



void CRasterLayer::QueueBlock(int blkIdx, CacheID id)
{
	ReadQueue q;
	q.id = id;
	q.blkidx = blkIdx;
	q.id.zoom = GetActualZoomRate();
	m_aReadQueue.Add(q);
	m_cacheTable.ids[blkIdx].xnum = id.xnum;
	m_cacheTable.ids[blkIdx].ynum = id.ynum;
	m_cacheTable.ids[blkIdx].bvalid = CacheID::stateLoading;
	m_cacheTable.ids[blkIdx].zoom = GetActualZoomRate();
}



void CRasterLayer::Draw()
{
	if (!(GetAttribute()&CDrawingLayer::attrVisible))
		return;

	if (m_bForceDraw)
	{
		CDrawingLayer::Draw();
		return;
	}

	CCoordSys *pCS = m_pContext->GetCoordSys();
	CRect rcView;
	m_pContext->GetViewRect(rcView);

	if (m_pCacheData != NULL)
	{
		double x[4], y[4];
		PT_3D pts[4];
		Envelope e1, e2;

		CArray<int, int> arrIndexs;

		e2.m_xl = rcView.left; e2.m_xh = rcView.right;
		e2.m_yl = rcView.top; e2.m_yh = rcView.bottom;

		//按照zoom排序，zoom小的放在前面
		for (int i = 0; i <m_cacheTable.nsum; i++)
		{
			CacheID id = m_cacheTable.ids[i];
			if (id.bvalid!=CacheID::stateValid)
				continue;

			for (int j = 0; j < arrIndexs.GetSize(); j++)
			{
				if (id.zoom<m_cacheTable.ids[arrIndexs[j]].zoom)
					break;
			}

			if (j < arrIndexs.GetSize())
				arrIndexs.InsertAt(j, i);
			else
				arrIndexs.Add(i);

		}
		m_pCacheData->BeginDisplayBlock();

		//逐个显示
		for (int j = 0; j < arrIndexs.GetSize(); j++)
		{
			CacheID id = m_cacheTable.ids[arrIndexs[j]];
			GetBlockClientXY(id, x, y);

			pts[0].x = x[0]; pts[0].y = y[0];
			pts[1].x = x[1]; pts[1].y = y[1];
			pts[2].x = x[2]; pts[2].y = y[2];
			pts[3].x = x[3]; pts[3].y = y[3];

			e1.CreateFromPts(pts, 4);
			if (!e1.bIntersect(&e2))
			{
				continue;
			}

			m_pCacheData->DisplayBlock(m_pContext, arrIndexs[j], x, y);
		}

		m_pCacheData->EndDisplayBlock();
	}
	else
	{
		DrawClient(rcView.left, rcView.right, rcView.top, rcView.bottom);
	}
}



BOOL CRasterLayer::CheckCacheReady()
{
	if (m_pCacheData == NULL)
		return TRUE;

	CCoordSys *pCS = m_pContext->GetCoordSys();
	CRect rcView;
	m_pContext->GetViewRect(rcView);

	if (m_pCacheData != NULL)
	{
		int view_x1 = 0, view_x2 = 0, view_y1 = 0, view_y2 = 0, view_x3 = 0, view_y3 = 0, view_x4 = 0, view_y4 = 0;

		//获取当前视图所覆盖的块范围
		GetFocusBlockNum(rcView.TopLeft(), view_x1, view_y1);
		GetFocusBlockNum(rcView.BottomRight(), view_x2, view_y2);
		CPoint pt(rcView.right, rcView.top);
		GetFocusBlockNum(pt, view_x3, view_y3);
		pt.x = rcView.left;
		pt.y = rcView.bottom;
		GetFocusBlockNum(pt, view_x4, view_y4);

		//可能坐标系是反的，这时会导致 view_x1>view_x2 或者  view_y1>view_y2
		if (view_x1>view_x2)
		{
			int mint, maxt;
			mint = view_x2;
			maxt = view_x1;
			if (view_x3<mint)mint = view_x3;
			if (view_x3>maxt)maxt = view_x3;
			if (view_x4<mint)mint = view_x4;
			if (view_x4>maxt)maxt = view_x4;
			view_x1 = mint; view_x2 = maxt;
		}
		else
		{
			int mint, maxt;
			mint = view_x1;
			maxt = view_x2;
			if (view_x3<mint)mint = view_x3;
			if (view_x3>maxt)maxt = view_x3;
			if (view_x4<mint)mint = view_x4;
			if (view_x4>maxt)maxt = view_x4;
			view_x1 = mint; view_x2 = maxt;
		}

		if (view_y1>view_y2)
		{
			int mint, maxt;
			mint = view_y2;
			maxt = view_y1;
			if (view_y3<mint)mint = view_y3;
			if (view_y3>maxt)maxt = view_y3;
			if (view_y4<mint)mint = view_y4;
			if (view_y4>maxt)maxt = view_y4;
			view_y1 = mint; view_y2 = maxt;
		}
		else
		{
			int mint, maxt;
			mint = view_y1;
			maxt = view_y2;
			if (view_y3<mint)mint = view_y3;
			if (view_y3>maxt)maxt = view_y3;
			if (view_y4<mint)mint = view_y4;
			if (view_y4>maxt)maxt = view_y4;
			view_y1 = mint; view_y2 = maxt;
		}
		//获取数据的块范围
		int nxmin = 0, nxmax = 0, nymin = 0, nymax = 0;
		GetBlockNumRange(nxmin, nxmax, nymin, nymax);

		//取相交区域
		nxmin = max(nxmin, view_x1);
		nxmax = min(nxmax, view_x2);
		nymin = max(nymin, view_y1);
		nymax = min(nymax, view_y2);

		double x[4], y[4];
		PT_3D pts[4];
		Envelope e1, e2;

		e2.m_xl = rcView.left; e2.m_xh = rcView.right;
		e2.m_yl = rcView.top; e2.m_yh = rcView.bottom;

		for (int i = nxmin; i <= nxmax; i++)
		{
			for (int j = nymin; j <= nymax; j++)
			{
				CacheID id(i, j);

				id.zoom = GetActualZoomRate();
				GetBlockClientXY(id, x, y);

				pts[0].x = x[0]; pts[0].y = y[0];
				pts[1].x = x[1]; pts[1].y = y[1];
				pts[2].x = x[2]; pts[2].y = y[2];
				pts[3].x = x[3]; pts[3].y = y[3];

				e1.CreateFromPts(pts, 4);
				if (!e1.bIntersect(&e2))
				{
					continue;
				}

				int blkIdx = GetBlockIdx(id);
				if (blkIdx<0)return FALSE;
			}
		}
		/*
		BOOL bClearOldBlock = FALSE;

		float zoom = GetActualZoomRate();
		for (i = 0; i < m_cacheTable.nsum; i++)
		{
			if (m_cacheTable.ids[i].bvalid == CacheID::stateValid && m_cacheTable.ids[i].zoom != zoom)
			{
				ClearBlock(i);
				bClearOldBlock = TRUE;
			}
		}

		if (bClearOldBlock)
			return FALSE;
		*/
	}

	return TRUE;
}


MyNameSpaceEnd