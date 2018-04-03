// DrawingLayer.cpp: implementation of the CDrawingLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawingLayer.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin

IMPLEMENT_DYNAMIC(CDrawingLayer, CObject)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawingLayer::CDrawingLayer()
{
	m_pContext = NULL;
	m_dwAttribute = CDrawingLayer::attrVisible;

	m_bForceDraw = TRUE;
}

CDrawingLayer::~CDrawingLayer()
{
}

void CDrawingLayer::Destroy()
{
	if( m_pContext )m_pContext->BeginDrawing();

	CCacheLoader::Destroy();

	if( m_pContext )m_pContext->EndDrawing();
}


BOOL CDrawingLayer::DestroyCache()
{
	if( m_pContext )m_pContext->BeginDrawing();
	
	CCacheLoader::DestroyCache();
	
	if( m_pContext )m_pContext->EndDrawing();

	return TRUE;
}


void CDrawingLayer::SetContext(CDrawingContext *pContext)
{
	m_pContext = pContext;
	if (m_pCacheData && pContext)
	{
		m_pCacheData->SetBkColor(m_pContext->GetBackColor());
	}
}


void CDrawingLayer::OnChangeCoordSys(BOOL bJustScrollScreen)
{
	
}


BOOL CDrawingLayer::InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK)
{
	if (IsValid())  Destroy();

	if( clrBK==0xffffffff )
		clrBK = m_pContext->GetBackColor();

	m_pContext->BeginDrawing();	

	BOOL bRet = CCacheLoader::InitCache(nCacheType, szBlock,nBlock,clrBK);	

	m_pContext->EndDrawing();
	return bRet;
}


void CDrawingLayer::SetViewBound(Envelope e)
{
	
}


Envelope CDrawingLayer::GetDataBound()
{
	return Envelope();
}



void CDrawingLayer::ModifyAttribute(DWORD add, DWORD cut)
{
	m_dwAttribute = (m_dwAttribute|add);
	m_dwAttribute = (m_dwAttribute&(~cut));

	if( m_pCacheData )
	{
		m_pCacheData->EnableSmoothZoom((m_dwAttribute&attrSmoothZoom)!=0);
	}
}

void CDrawingLayer::SetAttribute(DWORD attr)
{
	m_dwAttribute = attr;

	if( m_pCacheData )
	{
		m_pCacheData->EnableSmoothZoom((m_dwAttribute&attrSmoothZoom)!=0);
	}
}


DWORD CDrawingLayer::GetAttribute()
{
	return m_dwAttribute;
}



void CDrawingLayer::Invalidate()
{
	
}


BOOL CDrawingLayer::IsInvalidated()
{
	return FALSE;
}



BOOL CDrawingLayer::CanErase()
{
	return FALSE;
}


void CDrawingLayer::Erase()
{
	
}


BOOL CDrawingLayer::CheckCacheReady()
{
	if( m_pCacheData==NULL )
		return TRUE;

	CCoordSys *pCS = m_pContext->GetCoordSys();
	CRect rcView;
	m_pContext->GetViewRect(rcView);
	
	if( m_pCacheData!=NULL )
	{
		int view_x1=0, view_x2=0, view_y1=0, view_y2=0, view_x3=0, view_y3=0, view_x4=0, view_y4=0;

		//获取当前视图所覆盖的块范围
		GetFocusBlockNum(rcView.TopLeft(),view_x1,view_y1);
		GetFocusBlockNum(rcView.BottomRight(),view_x2,view_y2);
		CPoint pt(rcView.right,rcView.top);
		GetFocusBlockNum(pt,view_x3,view_y3);
		pt.x = rcView.left;
		pt.y = rcView.bottom;
		GetFocusBlockNum(pt,view_x4,view_y4);
	
		//可能坐标系是反的，这时会导致 view_x1>view_x2 或者  view_y1>view_y2
		if( view_x1>view_x2 )
		{	
			int mint,maxt;
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
			int mint,maxt;
			mint = view_x1;
			maxt = view_x2;
			if (view_x3<mint)mint = view_x3; 
			if (view_x3>maxt)maxt = view_x3;
			if (view_x4<mint)mint = view_x4; 
			if (view_x4>maxt)maxt = view_x4;
			view_x1 = mint; view_x2 = maxt; 
		}

		if( view_y1>view_y2 )
		{
			int mint,maxt;
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
			int mint,maxt;
			mint = view_y1;
			maxt = view_y2;
			if (view_y3<mint)mint = view_y3; 
			if (view_y3>maxt)maxt = view_y3;
			if (view_y4<mint)mint = view_y4; 
			if (view_y4>maxt)maxt = view_y4;
			view_y1 = mint; view_y2 = maxt; 
		}
		//获取数据的块范围
		int nxmin=0,nxmax=0,nymin=0,nymax=0;
		GetBlockNumRange(nxmin,nxmax,nymin,nymax);

		//取相交区域
		nxmin = max(nxmin,view_x1);
		nxmax = min(nxmax,view_x2);
		nymin = max(nymin,view_y1);
		nymax = min(nymax,view_y2);
		
		double x[4], y[4];
		PT_3D pts[4];
		Envelope e1, e2;

		e2.m_xl = rcView.left; e2.m_xh = rcView.right;
		e2.m_yl = rcView.top; e2.m_yh = rcView.bottom;
		
		for( int i=nxmin; i<=nxmax; i++)
		{
			for( int j=nymin; j<=nymax; j++)
			{				
				CacheID id(i,j);

				GetBlockClientXY(id,x,y);
				
				pts[0].x = x[0]; pts[0].y = y[0];
				pts[1].x = x[1]; pts[1].y = y[1];
				pts[2].x = x[2]; pts[2].y = y[2];
				pts[3].x = x[3]; pts[3].y = y[3];
				
				e1.CreateFromPts(pts,4);
				if( !e1.bIntersect(&e2) )
				{
					continue;
				}

				int blkIdx = GetBlockIdx(id);
				if( blkIdx<0 )return FALSE;
			}
		}
	}

	return TRUE;
}


void CDrawingLayer::Draw()
{
	if( !(GetAttribute()&CDrawingLayer::attrVisible) )
		return;

	CCoordSys *pCS = m_pContext->GetCoordSys();
	CRect rcView;
	m_pContext->GetViewRect(rcView);
	
	if( m_pCacheData!=NULL )
	{
		int view_x1=0, view_x2=0, view_y1=0, view_y2=0, view_x3=0, view_y3=0, view_x4=0, view_y4=0;

		//获取当前视图所覆盖的块范围
		GetFocusBlockNum(rcView.TopLeft(),view_x1,view_y1);
		GetFocusBlockNum(rcView.BottomRight(),view_x2,view_y2);
		CPoint pt(rcView.right,rcView.top);
		GetFocusBlockNum(pt,view_x3,view_y3);
		pt.x = rcView.left;
		pt.y = rcView.bottom;
		GetFocusBlockNum(pt,view_x4,view_y4);
	
		//可能坐标系是反的，这时会导致 view_x1>view_x2 或者  view_y1>view_y2
		if( view_x1>view_x2 )
		{	
			int mint,maxt;
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
			int mint,maxt;
			mint = view_x1;
			maxt = view_x2;
			if (view_x3<mint)mint = view_x3; 
			if (view_x3>maxt)maxt = view_x3;
			if (view_x4<mint)mint = view_x4; 
			if (view_x4>maxt)maxt = view_x4;
			view_x1 = mint; view_x2 = maxt; 
		}

		if( view_y1>view_y2 )
		{
			int mint,maxt;
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
			int mint,maxt;
			mint = view_y1;
			maxt = view_y2;
			if (view_y3<mint)mint = view_y3; 
			if (view_y3>maxt)maxt = view_y3;
			if (view_y4<mint)mint = view_y4; 
			if (view_y4>maxt)maxt = view_y4;
			view_y1 = mint; view_y2 = maxt; 
		}
		//获取数据的块范围
		int nxmin=0,nxmax=0,nymin=0,nymax=0;
		GetBlockNumRange(nxmin,nxmax,nymin,nymax);

		//取相交区域
		nxmin = max(nxmin,view_x1);
		nxmax = min(nxmax,view_x2);
		nymin = max(nymin,view_y1);
		nymax = min(nymax,view_y2);
		
		double x[4], y[4];
		PT_3D pts[4];
		Envelope e1, e2;

		e2.m_xl = rcView.left; e2.m_xh = rcView.right;
		e2.m_yl = rcView.top; e2.m_yh = rcView.bottom;
		m_pCacheData->BeginDisplayBlock();
		
		for( int i=nxmin; i<=nxmax; i++)
		{
			for( int j=nymin; j<=nymax; j++)
			{				
				CacheID id(i,j);

				GetBlockClientXY(id,x,y);
				
				pts[0].x = x[0]; pts[0].y = y[0];
				pts[1].x = x[1]; pts[1].y = y[1];
				pts[2].x = x[2]; pts[2].y = y[2];
				pts[3].x = x[3]; pts[3].y = y[3];
				
				e1.CreateFromPts(pts,4);
				if( !e1.bIntersect(&e2) )
				{
					continue;
				}

				int blkIdx = -1;
				
				if( GetAttribute()&CDrawingLayer::attrExtraPrepareCache )
					blkIdx = GetBlockIdx(id);
				else
					blkIdx = (m_bForceDraw?ForceFillBlock(id):GetBlockIdx(id));

				if( blkIdx<0 )continue;
				
				m_pCacheData->DisplayBlock(m_pContext,blkIdx,x,y);
			}
		}
		m_pCacheData->EndDisplayBlock();
	}
	else
	{
		DrawClient(rcView.left,rcView.right,rcView.top,rcView.bottom);
	}
}


BOOL CDrawingLayer::ExtraPrepareCaches()
{
	if( !(GetAttribute()&CDrawingLayer::attrVisible) )
		return FALSE;

	if( !(GetAttribute()&CDrawingLayer::attrExtraPrepareCache) )
		return FALSE;

	CCoordSys *pCS = m_pContext->GetCoordSys();
	CRect rcView;
	m_pContext->GetViewRect(rcView);
	
	if( m_pCacheData!=NULL )
	{
		int view_x1=0, view_x2=0, view_y1=0, view_y2=0, view_x3=0, view_y3=0, view_x4=0, view_y4=0;

		//获取当前视图所覆盖的块范围
		GetFocusBlockNum(rcView.TopLeft(),view_x1,view_y1);
		GetFocusBlockNum(rcView.BottomRight(),view_x2,view_y2);
		CPoint pt(rcView.right,rcView.top);
		GetFocusBlockNum(pt,view_x3,view_y3);
		pt.x = rcView.left;
		pt.y = rcView.bottom;
		GetFocusBlockNum(pt,view_x4,view_y4);
	
		//可能坐标系是反的，这时会导致 view_x1>view_x2 或者  view_y1>view_y2
		if( view_x1>view_x2 )
		{	
			int mint,maxt;
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
			int mint,maxt;
			mint = view_x1;
			maxt = view_x2;
			if (view_x3<mint)mint = view_x3; 
			if (view_x3>maxt)maxt = view_x3;
			if (view_x4<mint)mint = view_x4; 
			if (view_x4>maxt)maxt = view_x4;
			view_x1 = mint; view_x2 = maxt; 
		}

		if( view_y1>view_y2 )
		{
			int mint,maxt;
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
			int mint,maxt;
			mint = view_y1;
			maxt = view_y2;
			if (view_y3<mint)mint = view_y3; 
			if (view_y3>maxt)maxt = view_y3;
			if (view_y4<mint)mint = view_y4; 
			if (view_y4>maxt)maxt = view_y4;
			view_y1 = mint; view_y2 = maxt; 
		}
		//获取数据的块范围
		int nxmin=0,nxmax=0,nymin=0,nymax=0;
		GetBlockNumRange(nxmin,nxmax,nymin,nymax);

		//取相交区域
		nxmin = max(nxmin,view_x1);
		nxmax = min(nxmax,view_x2);
		nymin = max(nymin,view_y1);
		nymax = min(nymax,view_y2);
		
		double x[4], y[4];
		PT_3D pts[4];
		Envelope e1, e2;

		e2.m_xl = rcView.left; e2.m_xh = rcView.right;
		e2.m_yl = rcView.top; e2.m_yh = rcView.bottom;
		
		for( int i=nxmin; i<=nxmax; i++)
		{
			for( int j=nymin; j<=nymax; j++)
			{				
				CacheID id(i,j);

				GetBlockClientXY(id,x,y);
				
				pts[0].x = x[0]; pts[0].y = y[0];
				pts[1].x = x[1]; pts[1].y = y[1];
				pts[2].x = x[2]; pts[2].y = y[2];
				pts[3].x = x[3]; pts[3].y = y[3];
				
				e1.CreateFromPts(pts,4);
				if( !e1.bIntersect(&e2) )
				{
					continue;
				}

				int blkIdx = (m_bForceDraw?ForceFillBlock(id):GetBlockIdx(id));
				if( blkIdx<0 )continue;
			}
		}
	}

	return TRUE;
}


void CDrawingLayer::DrawClient(double xl, double xh, double yl, double yh)
{
}


BOOL CDrawingLayer::FillBlock(int blkIdx, CacheID id)
{
	return CCacheLoader::FillBlock(blkIdx,id);
}



int CDrawingLayer::ForceFillBlock(CacheID id)
{
	if( m_pCacheData==NULL )
		return -1;
	
	int blkIdx = GetBlockIdx( id );	
	if( blkIdx>=0 && blkIdx<m_cacheTable.nsum )
	{	
		return blkIdx;
	}
	
	if( !FindFreeBlock(&blkIdx,NULL) )
	{
		blkIdx = 0;
	}
	
	ClearBlock(blkIdx);
	if( !FillBlock(blkIdx,id) )
		return -1;
	return blkIdx;
}



MyNameSpaceEnd