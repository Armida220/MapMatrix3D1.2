// MemRasterLayer.cpp: implementation of the CMemRasterLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemRasterLayer.h"
#include "SmartViewFunctions.h"
#include "matrix.h"

#include "GlobalFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin


IMPLEMENT_DYNAMIC(CMemRasterLayer,CDrawingLayer)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemRasterLayer::CMemRasterLayer()
{
	m_bUseCornerPts = FALSE;
	m_bCalcParam = FALSE;

	m_fKX = m_fKY = 1.0f;
	m_fRotAng = 0.0f;

	m_pFuncOwner = NULL;
	m_pFuncConvertCoord = NULL;

	m_bForceDraw = FALSE;
	m_bEnableBGLoad = FALSE;

	m_szImage = CSize(0,0);
}

CMemRasterLayer::~CMemRasterLayer()
{
	Destroy();
}

BOOL CMemRasterLayer::InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK)
{
	if (IsValid())  Destroy();

	nBlock = 1;
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
	else if( nCacheType==CHEDATA_TYPE_GLTEXT )
	{
		CCacheGLTextData *pCache = new CCacheGLTextData;
		if( pCache!=NULL )
		{
			pCache->m_bUseAlpha = TRUE;
			m_pCacheData = pCache;
		}
	}
	
	if( !CDrawingLayer::InitCache(nCacheType, szBlock, nBlock, clrBK) )
		return FALSE;

	m_pCacheData->EnableTransparentOverlap(TRUE);
	m_pCacheData->EnableSmoothZoom(FALSE);

	return TRUE;
}

void CMemRasterLayer::Destroy()
{
	CDrawingLayer::Destroy();
}


void CMemRasterLayer::SetImageSize(CSize szImg)
{
	m_szImage = szImg;
}

CSize CMemRasterLayer::GetImageSize()
{
	return m_szImage;
}

CSize CMemRasterLayer::GetVirtualImageSize()
{
	CSize szImage = GetImageSize();
	if( m_fRotAng==90 || m_fRotAng==270 )
		szImage = CSize(szImage.cy,szImage.cx);

	szImage.cx *= m_fKX;
	szImage.cy *= m_fKY;
	return szImage;
}


Envelope CMemRasterLayer::GetDataBound()
{
	PT_4D pts[4];
	pts[0].x = 0; pts[0].y = 0;
	pts[1].x = m_szImage.cx; pts[1].y = 0;
	pts[2].x = m_szImage.cx; pts[2].y = m_szImage.cy;
	pts[3].x = 0; pts[3].y = m_szImage.cy;
	
	ImageToGround(pts,4);

	Envelope evlp;
	evlp.CreateFromPts(pts,4,sizeof(PT_4D));
	return evlp;
}


// 获得块编号的合理的取值范围
void CMemRasterLayer::GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax)
{
	xmin = ymin = -INT_MAX;
	xmax = ymax = INT_MAX;
}


// 获得焦点处所在的块编号
void CMemRasterLayer::GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum)
{
	PT_4D pt;
	pt.x = ptFocus.x; pt.y = ptFocus.y;
	
	m_pContext->GetCoordSys()->ClientToGround(&pt,1);
	GroundToImage(&pt,1);

	xnum = pt.x/m_szBlock.cx;
	ynum = pt.y/m_szBlock.cy;
}


// 获得块的四个角点坐标在客户坐标系下的XY坐标
void CMemRasterLayer::GetBlockClientXY(CacheID id, double x[4], double y[4])
{
	//加0.5 是要定位到像素中心
	PT_4D pts[4];
	pts[0].x = id.xnum*m_szBlock.cx; pts[0].y = id.ynum*m_szBlock.cy;
	pts[1].x = (id.xnum+1)*m_szBlock.cx; pts[1].y = id.ynum*m_szBlock.cy;
	pts[2].x = (id.xnum+1)*m_szBlock.cx; pts[2].y = (id.ynum+1)*m_szBlock.cy;
	pts[3].x = id.xnum*m_szBlock.cx; pts[3].y = (id.ynum+1)*m_szBlock.cy;

	ImageToGround(pts,4);
	m_pContext->GetCoordSys()->GroundToClient(pts,4);

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


void CMemRasterLayer::OnChangeCoordSys(BOOL bJustScrollScreen)
{
	if( !bJustScrollScreen )
	{
	}
}

void CMemRasterLayer::SetCornerPts(BOOL bUseCorner, PT_3D* pts)
{
	m_bUseCornerPts = bUseCorner;
	m_bCalcParam = FALSE;

	if( bUseCorner )
	{
		memcpy(m_ptsCorner,pts,sizeof(m_ptsCorner));

		double dis1 = DIST_3DPT(pts[0],pts[1]);
		double dis2 = DIST_3DPT(pts[1],pts[2]);

		CSize szImage = GetImageSize();
		
		m_fKX = dis1/szImage.cx;
		m_fKY = dis2/szImage.cy;

		OnChangeCoordSys(FALSE);
	}
}


void CMemRasterLayer::SetCornerPts(float ang, float fKX, float fKY)
{
	m_bUseCornerPts = TRUE;
	m_bCalcParam = FALSE;
	
	{
		
		CSize szImage = GetImageSize();
		if( szImage.cx==0 || szImage.cy==0 )
		{
			m_bUseCornerPts = FALSE;
			return;
		}

		m_fRotAng = ang;
		
		if( m_pCacheData->GetType()==CHEDATA_TYPE_BMP )
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
}


void CMemRasterLayer::CalcAffineParm()
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


void CMemRasterLayer::GroundToImage(PT_4D *pts, int num)
{
	if( m_pFuncConvertCoord )
	{
		PT_4D pt;
		for( int i=0; i<num; i++)
		{
			pt = pts[i];
			(m_pFuncOwner->*m_pFuncConvertCoord)(pt.x,pt.y,pts[i].x,pts[i].y,FALSE);
		}
	}
	
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
}


void CMemRasterLayer::ImageToGround(PT_4D *pts, int num)
{
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

	
	if( m_pFuncConvertCoord )
	{
		PT_4D pt;
		for( int i=0; i<num; i++)
		{
			pt = pts[i];
			(m_pFuncOwner->*m_pFuncConvertCoord)(pt.x,pt.y,pts[i].x,pts[i].y,TRUE);
		}
		return;
	}
}


void CMemRasterLayer::DrawClient(double xl, double xh, double yl, double yh)
{
}


void CMemRasterLayer::ClearImageAdjust()
{
	m_Adjust = CImageAdjust();
}


void CMemRasterLayer::CalcImageAdjust(HBITMAP hBmp)
{
	if( !m_Adjust.bAdjustMap )
	{
		m_Adjust.StatHistogramInfo(hBmp);
		m_Adjust.CalcAdjustMap();
	}
}


BOOL CMemRasterLayer::BGLoad()
{
	return TRUE;
}

extern void ReverseBmp(HBITMAP hBmp);

//inRect 应该是影像坐标
BOOL CMemRasterLayer::WriteRectData(CRect inRect, HBITMAP hBmp, COLORREF bkColor)
{
	COLORREF bkcolor0 = m_pContext->GetBackColor();
	m_pContext->SetBackColor(bkColor);
	
	int nX0, nY0, nX1, nY1;

	nX0 = floor((float)inRect.left/m_szBlock.cx); nX1 = ceil((float)inRect.right/m_szBlock.cx);
	nY0 = floor((float)inRect.top/m_szBlock.cy); nY1 = ceil((float)inRect.bottom/m_szBlock.cy);
	
	int nxmin,nxmax,nymin,nymax;
	GetBlockNumRange(nxmin,nxmax,nymin,nymax);
	
	double x[4], y[4];
	PT_3D pts[4];
	Envelope e1, e2;
	
	for( int i=nX0; i<nX1; i++)
	{
		if( i<nxmin || i>nxmax )continue;
		for( int j=nY0; j<nY1; j++)
		{
			if( j<nymin || j>nymax )continue;
			
			CacheID id(i,j);
			
			int blkIdx = ForceFillBlock(id);
			if( blkIdx<0 )continue;

			CRect rcBlk = inRect;
			rcBlk.OffsetRect(-i*m_szBlock.cx,-j*m_szBlock.cy);

			m_pCacheData->BeginFillBlock(m_pContext,blkIdx);
			ReverseBmp(hBmp);

			int t = rcBlk.top;
			rcBlk.top = m_szBlock.cy-rcBlk.bottom;
			rcBlk.bottom = m_szBlock.cy-t;
			m_pCacheData->DrawBitmap(hBmp,CRect(0,0,inRect.Width(),inRect.Height()),rcBlk,FALSE);
			m_pCacheData->EndFillBlock();
		}
	}
	
	m_pContext->SetBackColor(bkcolor0);
	
	return TRUE;
}


BOOL CMemRasterLayer::WriteRectData(CRect inRect, RGBQUAD* pClrTbl,
				   int nBitCount, COLORREF bkColor, 
				   BYTE* pSrc, int nMemXoffBytes, int nMemYoffBytes, 
				   int nMemWidBytes, int nMemHeiBytes, 
				   BOOL rgbOrder, BOOL bottomUp)
{
	int newWid = inRect.Width(), newHei = inRect.Height();
	int newLine = (((newWid*nBitCount+7)/8+3)&(~3));
	
	BITMAPINFO *pBmpInfo = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFO)+255*sizeof(RGBQUAD)];
	if( !pBmpInfo )return FALSE;
	
	pBmpInfo->bmiHeader.biSize = sizeof(pBmpInfo->bmiHeader);
	pBmpInfo->bmiHeader.biWidth = newWid;
	pBmpInfo->bmiHeader.biHeight = newHei;
	pBmpInfo->bmiHeader.biSizeImage = newLine*newHei;
	pBmpInfo->bmiHeader.biPlanes = 1;
	pBmpInfo->bmiHeader.biBitCount = nBitCount;
	pBmpInfo->bmiHeader.biCompression = BI_RGB;
	pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
	pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
	pBmpInfo->bmiHeader.biClrUsed = 0;
	pBmpInfo->bmiHeader.biClrImportant = 0;

	RGBQUAD clrtbl[256];
	for( int k=0; k<256; k++)
	{
		clrtbl[k].rgbRed = clrtbl[k].rgbGreen = clrtbl[k].rgbBlue = k;
		clrtbl[k].rgbReserved = 0;
	}

	if( pClrTbl==NULL )pClrTbl = clrtbl;

	if( nBitCount==8 && pClrTbl!=NULL )
		memcpy(pBmpInfo->bmiColors,pClrTbl, 256*sizeof(RGBQUAD));

	BYTE *pSrc2 = NULL, *pTmpD, *pTmpS;

	HBITMAP hNewBmp = ::CreateDIBSection( NULL, pBmpInfo, DIB_RGB_COLORS, 
		(void**)&pSrc2, NULL,0L );

	if( !hNewBmp )
	{
		delete[] (BYTE*)pBmpInfo;
		return FALSE;
	}

	memset(pSrc2,0,pBmpInfo->bmiHeader.biSizeImage);
	
	for( int i=0; i<newHei; i++)
	{
		if( bottomUp )
		{
			pTmpS = pSrc+(i+nMemYoffBytes)*nMemWidBytes+nMemXoffBytes;
		}
		else
		{
			pTmpS = pSrc+(nMemYoffBytes+newHei-1-i)*nMemWidBytes+nMemXoffBytes;
		}
		pTmpD = pSrc2+newLine*i;
		
		if( !rgbOrder || (nBitCount!=24&&nBitCount!=32) )
		{
			memcpy(pTmpD,pTmpS,newLine);
		}
		else
		{
			for( int j=0; j<newWid; j++)
			{
				pTmpD[0]=pTmpS[2]; pTmpD[1]=pTmpS[1]; pTmpD[2]=pTmpS[0];
				pTmpD += (nBitCount>>3); pTmpS += (nBitCount>>3);
			}
		}
	}
	//for test
	if( 0 )
	{
		HBITMAP hBmp2 = Create24BitDIB(inRect.Width(),inRect.Height());
		OverlapBitmap(hNewBmp,pClrTbl,CRect(0,0,inRect.Width(),inRect.Height()),hBmp2,NULL,CRect(0,0,inRect.Width(),inRect.Height()));
		
		WriteToFile(hBmp2);
		::DeleteObject(hBmp2);
	}

	BOOL bRet = WriteRectData(inRect,hNewBmp,bkColor);

	::DeleteObject(hNewBmp);
	delete[] (BYTE*)pBmpInfo;
	
	return bRet;
}


int CMemRasterLayer::ForceFillBlock(CacheID id)
{
	if( m_pCacheData==NULL )
		return -1;
	
	int blkIdx = GetBlockIdx( id );	
	if( blkIdx>=0 && blkIdx<m_cacheTable.nsum )
	{	
		return blkIdx;
	}

	for( int i=0;i<m_cacheTable.nsum;i++ )
	{
		CacheID tc = m_cacheTable.ids[i];
		if( tc.IsNotInValid() )
		{
		}
		else
		{
			blkIdx = i;
		}
	}

	if( blkIdx>=0 && blkIdx<m_cacheTable.nsum )
	{
		if( !FillBlock(blkIdx,id) )
			return -1;

		return blkIdx;
	}

	int nNewBlock = 1;

	// allocate more blocks
	if( !m_pCacheData->AskforMoreBlocks(nNewBlock) )
		return -1;

	CacheID *pids = new CacheID[m_cacheTable.nsum+nNewBlock];
	memcpy(pids,m_cacheTable.ids,sizeof(CacheID)*m_cacheTable.nsum);
	delete[] m_cacheTable.ids;

	m_cacheTable.ids = pids;
	m_cacheTable.nsum += nNewBlock;

	blkIdx = m_cacheTable.nsum-nNewBlock;

	if( !FillBlock(blkIdx,id) )
		return -1;

	return blkIdx;
}



void CMemRasterLayer::Draw()
{
	if( !m_bInitCacheOK )
		return;
	
	if( !(GetAttribute()&CDrawingLayer::attrVisible) )
		return;
	
	CCoordSys *pCS = m_pContext->GetCoordSys();
	CRect rcView;
	m_pContext->GetViewRect(rcView);
	
	if( m_pCacheData!=NULL )
	{
		int nFocusBlockX, nFocusBlockY;
		GetFocusBlockNum(rcView.CenterPoint(),nFocusBlockX,nFocusBlockY);
				
		double x[4], y[4];
		PT_3D pts[4];
		Envelope e1, e2;
		
		e2.m_xl = rcView.left; e2.m_xh = rcView.right;
		e2.m_yl = rcView.top; e2.m_yh = rcView.bottom;

		m_pCacheData->BeginDisplayBlock();
		
		for( int i=0; i<m_cacheTable.nsum; i++)
		{
			CacheID id = m_cacheTable.ids[i];
			if( id.bvalid!=CacheID::stateValid )
				continue;
			
			GetBlockClientXY(id,x,y);
			
			pts[0].x = x[0]; pts[0].y = y[0];
			pts[1].x = x[1]; pts[1].y = y[1];
			pts[2].x = x[2]; pts[2].y = y[2];
			pts[3].x = x[3]; pts[3].y = y[3];
			
			e1.CreateFromPts(pts,4);
			if( !e1.bIntersect(&e2) )
				continue;
			
			int blkIdx = (m_bForceDraw?ForceFillBlock(id):GetBlockIdx(id));
			if( blkIdx<0 )continue;
			
			m_pCacheData->DisplayBlock(m_pContext,blkIdx,x,y);
		}

		m_pCacheData->EndDisplayBlock();
	}
	else
	{
		DrawClient(rcView.left,rcView.right,rcView.top,rcView.bottom);
	}
}



MyNameSpaceEnd
