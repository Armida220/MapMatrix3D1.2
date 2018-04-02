// RealtimeVectLayer.cpp: implementation of the CRealtimeVectLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RealtimeVectLayer.h"
#include "Matrix.h"
#include "SmartViewFunctions.h"

#include "Selection.h"
#include "DlgDataSource.h"
#include "Feature.h"

#include "GeoCurve.h"
#include "GeoPoint.h"
#include "GeoSurface.h"



IMPLEMENT_DYNAMIC(CRealtimeVectLayer,CDrawingLayer)

void DeleteElementList(GrElementList& obj)
{
	GrElement *p = obj.head;
	while( p!=NULL )
	{
		GrElement *p0 = p;
		p = p->next;
		delete p0;
	}	
	obj.delAll();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRealtimeVectLayer::CRealtimeVectLayer():m_arrTempObjs(1024)
{
	m_bCalcClientBound = FALSE;
	m_pCacheData = NULL;

	m_bForceDraw = TRUE;

	m_fMarkWidth1 = 2; 
	m_fMarkWidth2 = 1;
	m_clrMark1 = RGB(255,255,255);
	m_clrMark2 = RGB(255,255,255);

	m_bDrawMark1 = FALSE;
	m_bDrawMark2 = FALSE;

	m_DataBound = Envelope(0,1000,0,1000);

	matrix_toIdentity(m_DataMatrix,4);
	matrix_toIdentity(m_DataRMatrix,4);

	m_pDoc = NULL;
	m_pMainDS = NULL;
	m_pCS = NULL;
	m_pDrawContentCS = NULL;
	m_pDQ = NULL;

	m_nLoadMaxNum = 0;

	m_bEnableBGLoad = FALSE;
	m_bForceDraw = FALSE;

	m_pDrawMan = NULL;
	m_pDrawFun = 0;
	m_pCheckLayerFun = NULL;

	m_eleListBlockFtrs.which_mark = 0;
	m_bTooManyDirtyBufs = FALSE;
	m_nDrawNum = 0;

	m_bEnableDelayDraw = TRUE;

	m_dwAttribute |= CDrawingLayer::attrExtraPrepareCache;

	m_b2DCoordSys = TRUE;

	m_bSymbolized = TRUE;
}

CRealtimeVectLayer::~CRealtimeVectLayer()
{
	Destroy();
}

void CRealtimeVectLayer::Init(CDlgDoc *pDoc, CDlgDataQuery *pDQ, CCoordSys *pDrawContentCS, CCoordSys *pCS, CObject *pObj, PFDrawFtr pfun1, PFCheckLayerVisible pfun2)
{
	m_pDoc = pDoc;
	m_pMainDS = pDoc->GetDlgDataSource(0);
	m_pDrawContentCS = pDrawContentCS;
	m_pCS = pCS;
	m_pDQ = pDQ;

	m_pDrawMan = pObj;
	m_pDrawFun = pfun1;
	m_pCheckLayerFun = pfun2;
}

void CRealtimeVectLayer::Destroy()
{
	DelAllObjs();

	CDrawingLayer::Destroy();
	
	m_bCalcClientBound = FALSE;
	m_pCacheData = NULL;
}

void CRealtimeVectLayer::ClearBlockOfObj(LONG_PTR objnum)
{
	if (!m_pDoc) return;

	Envelope e;

	GrBuffer *pBuf = NULL;
	
	int index;
	if( FindBuf(objnum,index) )
	{
		GrBuffer *pBuf = (GrBuffer*)(m_arrCacheBufs.GetAt(index)->pBuf);
		if (pBuf)
		{
			e = pBuf->GetEnvelope();
		}
	}
	else
	{
		CFeature *pFtr = (CFeature*)objnum;

		GrBuffer buf;

		if( m_pDrawMan )
		{
			CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);

			CCoordSys *pCS = m_pCS;
			
			double gscale0 = pCS->CalcScale();
			double gscale = gscale0*m_pMainDS->GetScale()/1000.0;
			int nDrawSymbolizedFlag = CalcSymbolizedFlag(gscale);

			if( pDS )
			{
				(m_pDrawMan->*m_pDrawFun)(pDS,pDS->GetFtrLayerOfObject(pFtr),pFtr,nDrawSymbolizedFlag,&buf);
			}
		}
		else
		{
			CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
			if( pDS )
			{
				pDS->DrawFeature(pFtr,&buf);
			}
		}

		e = buf.GetEnvelope();
	}

	e.TransformGrdToClt(m_pCS,1);

	CRect rcView;
	m_pContext->GetViewRect(rcView);
	Envelope eview(0,rcView.Width(),0,rcView.Height());

	e.Intersect(&eview);
	if( e.Width()<=0 || e.Height()<=0 )
		return;

	int view_x1=0, view_x2=0, view_y1=0, view_y2=0, view_x3=0, view_y3=0, view_x4=0, view_y4=0;

	//获取当前视图所覆盖的块范围
	GetFocusBlockNum(CPoint(e.m_xl,e.m_yl),view_x1,view_y1);
	GetFocusBlockNum(CPoint(e.m_xh,e.m_yl),view_x2,view_y2);
	GetFocusBlockNum(CPoint(e.m_xh,e.m_yh),view_x3,view_y3);
	GetFocusBlockNum(CPoint(e.m_xl,e.m_yh),view_x4,view_y4);
	
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

	for (int i=nxmin; i<=nxmax; i++)
	{
		for (int j=nymin; j<=nymax; j++)
		{
			int blkIdx = GetBlockIdx(CacheID(i,j));
			ClearBlock(blkIdx);
		}
	}
	
}

BOOL CRealtimeVectLayer::InitCache(int nCacheType, CSize szBlock, int nBlock)
{
	if (IsValid())  Destroy();
	
	if( nCacheType==CHEDATA_TYPE_BMP )
	{
		CCacheBmpData *pCache = new CCacheBmpData;
		if( pCache!=NULL )
		{
			pCache->SetColorTable(8,NULL);
			pCache->EnableTransparentOverlap(TRUE);
			//pCache->CreateProgDataEncoder();
			
			m_pCacheData = pCache;
		}
	}
	if( nCacheType==CHEDATA_TYPE_GLTEXT )
	{
		CCacheGLTextData *pCache = new CCacheGLTextData;
		if( pCache!=NULL )
		{
			pCache->m_bUseAlpha = TRUE;
			m_pCacheData = pCache;
		}
	}
	if( nCacheType==CHEDATA_TYPE_D3DTEXT )
	{
		CCacheD3DTextData *pCache = new CCacheD3DTextData;
		if( pCache!=NULL )
		{
			pCache->SetD3D(m_pContext->m_pD3D,FALSE);
			pCache->m_bUseAlpha = TRUE;
			pCache->m_bUseLinearZoom = FALSE;
			m_pCacheData = pCache;
		}
	}
	
	if( !CDrawingLayer::InitCache(nCacheType, szBlock, nBlock) )
		return FALSE;
	return TRUE;
}


BOOL CRealtimeVectLayer::EnableDelayDraw(BOOL bEnable)
{
	BOOL bOld = m_bEnableDelayDraw;
	m_bEnableDelayDraw = bEnable;

	return bOld;
}


// 获得块编号的合理的取值范围
void CRealtimeVectLayer::GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax)
{
	xmin = ymin = -INT_MAX;
	xmax = ymax = INT_MAX;
}

// 获得焦点处所在的块编号
void CRealtimeVectLayer::GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum)
{
	CalcClientBound();

	xnum = floor((ptFocus.x-m_DataClientBound.m_xl)/m_szBlock.cx);
	ynum = floor((ptFocus.y-m_DataClientBound.m_yl)/m_szBlock.cy);
}


// 获得块的四个角点坐标在客户坐标系下的XY坐标
void CRealtimeVectLayer::GetBlockClientXY(CacheID id, double x[4], double y[4])
{
	CalcClientBound();

	x[0] = m_DataClientBound.m_xl + id.xnum*(double)m_szBlock.cx; 
	y[0] = m_DataClientBound.m_yl + id.ynum*(double)m_szBlock.cy;
	x[1] = m_DataClientBound.m_xl + (id.xnum+1)*(double)m_szBlock.cx; 
	y[1] = m_DataClientBound.m_yl + id.ynum*(double)m_szBlock.cy;
	x[2] = m_DataClientBound.m_xl + (id.xnum+1)*(double)m_szBlock.cx; 
	y[2] = m_DataClientBound.m_yl + (id.ynum+1)*(double)m_szBlock.cy;
	x[3] = m_DataClientBound.m_xl + id.xnum*(double)m_szBlock.cx; 
	y[3] = m_DataClientBound.m_yl + (id.ynum+1)*(double)m_szBlock.cy;
}


void CRealtimeVectLayer::CalcClientBound()
{
	if( !m_bCalcClientBound )
	{
		//获得数据范围在客户坐标系下的范围
		CCoordSys *pCS = m_pDrawContentCS;
		
		m_DataClientBound = GetDataBound();
		
		m_DataClientBound.TransformGrdToClt(pCS,1);

		//取整
		
		m_DataClientBound.m_xl = floor(m_DataClientBound.m_xl);
		m_DataClientBound.m_yl = floor(m_DataClientBound.m_yl);
		m_DataClientBound.m_xh = ceil(m_DataClientBound.m_xh);
		m_DataClientBound.m_yh = ceil(m_DataClientBound.m_yh);
		
		
		m_bCalcClientBound = TRUE;
	}
}



void CRealtimeVectLayer::ClearAll()
{
	//地物可能被删除了
	for( int i=0; i<m_arrPBlockFtrs.GetSize(); i++)
	{
		delete m_arrPBlockFtrs[i];
	}

	m_arrPBlockFtrs.RemoveAll();
	m_arrPBlockAllFtrs.RemoveAll();

	for( i=0; i<m_arrCacheBufs.GetSize(); i++)
	{
		delete (GrBuffer*)m_arrCacheBufs[i]->pBuf;
		delete m_arrCacheBufs[i];
	}
	
	m_arrCacheBufs.RemoveAll();
	m_listCacheBufs.delAll();

	for( i=0; i<m_arrDirtyPBufs.GetSize(); i++)
	{
		delete m_arrDirtyPBufs[i];
	}
	
	m_arrDirtyPBufs.RemoveAll();

	CDrawingLayer::ClearAll();
}


void CRealtimeVectLayer::OnChangeCoordSys(BOOL bJustScrollScreen)
{
	m_bCalcClientBound = FALSE;
	CalcClientBound();

	if( !bJustScrollScreen )
	{
		ClearAll();
	}
}


void CRealtimeVectLayer::EraseObjFromCache(CFeature *pFtr)
{
	if( m_pContext==NULL || m_pCacheData==NULL || m_bTooManyDirtyBufs )
		return;

	if( m_arrDirtyPBufs.GetSize()>=100 )
	{
		m_bTooManyDirtyBufs = TRUE;
		ClearAll();
		return;
	}

	CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
	if( !pDS )
		return;

	CCoordSys *pCS = m_pCS;
	
	double gscale0 = pCS->CalcScale();
	double gscale = gscale0*m_pMainDS->GetScale()/1000.0;
	int nDrawSymbolizedFlag = CalcSymbolizedFlag(gscale);

	CSymbolFlag sf1(CSymbolFlag::Tolerance,1/gscale0);
	CSymbolFlag sf2(CSymbolFlag::BreakCell,FALSE);
	
	GrBuffer *pBuf = new GrBuffer();
	if( m_pDrawMan )
	{
		(m_pDrawMan->*m_pDrawFun)(pDS,pDS->GetFtrLayerOfObject(pFtr),pFtr,nDrawSymbolizedFlag,pBuf);
	}
	else
	{
		pDS->DrawFeature(pFtr,pBuf);
	}

	pBuf->SetAllBackColor(TRUE);

	pBuf->RefreshEnvelope();
	m_arrDirtyPBufs.InsertAt(0,pBuf);
	
	m_nLoadMaxNum--;
}

void CRealtimeVectLayer::DrawDirtyObjs()
{
	m_bTooManyDirtyBufs = FALSE;

	if( m_arrDirtyPBufs.GetSize()<=0 )
		return;

	GrElementList list;
	Envelope e_obj;

	for( int i=0; i<m_arrDirtyPBufs.GetSize(); i++)
	{
		list.addBuffer(m_arrDirtyPBufs[i]);

		if( i==0 )
		{
			e_obj = m_arrDirtyPBufs[i]->GetEnvelope();
		}
		else
		{
			e_obj.Union(&m_arrDirtyPBufs[i]->GetEnvelope());
		}
	}

	//draw list
	{
		CCoordSys *pCS = m_pContext->GetCoordSys();

		//获得数据范围在客户坐标系下的范围	
		
		CalcClientBound();
		
		CRect rcViewSave;
		CSize szDC = m_pContext->GetDCSize();
		m_pContext->GetViewRect(rcViewSave);
		
		CCoordSys dataCS;
		dataCS.Create44Matrix(m_DataMatrix);
		
		PT_3D pts[4];
		double x[4], y[4];

		Envelope e1, e;
		e_obj.TransformGrdToClt(m_pCS,1);	
		
		//绘制数据	
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
			if( !e1.bIntersect(&e_obj) )
			{
				continue;
			}
			
			e = m_DataClientBound;
			e.m_xl = (e.m_xl) + (id.xnum*m_szBlock.cx);
			e.m_yl = (e.m_yl) + (id.ynum*m_szBlock.cy);
			
			//定义偏移换算关系
			double m[16];
			Matrix44FromMove(-e.m_xl,-e.m_yl,0,m);
			
			CCoordSys moveCS;
			moveCS.Create44Matrix(m);	
			
			//为cache中的数据创造复合坐标系（也就是cache的坐标系）
			CCoordSys compound;
			CArray<CCoordSys*,CCoordSys*> arr;
			arr.Add(&dataCS);
			arr.Add(m_pCS);
			arr.Add(&moveCS);
			
			//使用单一的矩阵，速度更快
			if( m_pCS->GetCoordSysType()==COORDSYS_44MATRIX )
			{
				double m1[16], m2[16];
				m_pCS->GetMatrix(m1);
				
				matrix_multiply(m1,m_DataMatrix,4,m2);
				
				matrix_multiply(m,m2,4,m1);
				compound.Create44Matrix(m1);
			}
			else
				compound.CreateCompound(arr);
			
			//当前块在复合坐标系下的范围
			m_pContext->SetCoordSys(&compound);		
			
			m_pContext->SetDCSize(m_szBlock);
			m_pContext->SetViewRect(CRect(0,0,m_szBlock.cx,m_szBlock.cy));
			
			m_pCacheData->BeginFillBlock(m_pContext,i,FALSE);	
			
			m_pCacheData->DrawGrBuffer(&list,CDrawingContext::modeNormal);
			
			m_pCacheData->EndFillBlock();
			
			m_pContext->SetDCSize(szDC);
			m_pContext->SetViewRect(rcViewSave);
			
			//恢复坐标系
			m_pContext->SetCoordSys(pCS);
		}
	}

	list.delAll();
	for( i=0; i<m_arrDirtyPBufs.GetSize(); i++)
	{
		delete m_arrDirtyPBufs[i];
	}
	
	m_arrDirtyPBufs.RemoveAll();

}

void CRealtimeVectLayer::DrawObjtoCache(CFeature *pFtr)
{
	if( m_pContext==NULL || m_pCacheData==NULL || m_bTooManyDirtyBufs )
		return;

	if( m_arrDirtyPBufs.GetSize()>=100 )
	{
		m_bTooManyDirtyBufs = TRUE;
		ClearAll();
		return;
	}

	CCoordSys *pCS = m_pCS;

	double gscale0 = pCS->CalcScale();
	double gscale = gscale0*m_pMainDS->GetScale()/1000.0;
	int nDrawSymbolizedFlag = CalcSymbolizedFlag(gscale);

	CSymbolFlag sf1(CSymbolFlag::Tolerance,1/gscale0);
	CSymbolFlag sf2(CSymbolFlag::BreakCell,FALSE);
	
	GrBuffer *pBuf = new GrBuffer();
	if( m_pDrawMan )
	{
		CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
		if( pDS )
		{
			(m_pDrawMan->*m_pDrawFun)(pDS,pDS->GetFtrLayerOfObject(pFtr),pFtr,nDrawSymbolizedFlag,pBuf);
		}				
	}
	else
	{
		CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
		if( pDS )
		{
			pDS->DrawFeature(pFtr,pBuf);
		}
	}

	pBuf->RefreshEnvelope();
	m_arrDirtyPBufs.Add(pBuf);

	m_nLoadMaxNum++;
	
}


// 填充块的内容
BOOL CRealtimeVectLayer::FillBlock(int blkIdx, CacheID id)
{
	if( m_pContext==NULL || m_pCacheData==NULL )
		return FALSE;

	//获得数据范围在客户坐标系下的范围
	CCoordSys *pCS = m_pContext->GetCoordSys();

	CalcClientBound();
	Envelope e = m_DataClientBound;

	e.m_xl = e.m_xl + (id.xnum*m_szBlock.cx);
	e.m_yl = e.m_yl + (id.ynum*m_szBlock.cy);

	//定义偏移换算关系
	double m[16];
	Matrix44FromMove(-e.m_xl,-e.m_yl,0,m);
	
	CCoordSys moveCS;
	moveCS.Create44Matrix(m);

	CCoordSys dataCS;
	dataCS.Create44Matrix(m_DataMatrix);
	
	//为cache中的数据创造复合坐标系（也就是cache的坐标系）
	CCoordSys compound;
	CArray<CCoordSys*,CCoordSys*> arr;
	arr.Add(&dataCS);
	arr.Add(m_pCS);
	arr.Add(&moveCS);

	//使用单一的矩阵，速度更快
	if( m_pCS->GetCoordSysType()==COORDSYS_44MATRIX )
	{
		double m1[16], m2[16];
		m_pCS->GetMatrix(m1);

		matrix_multiply(m1,m_DataMatrix,4,m2);

		matrix_multiply(m,m2,4,m1);
		compound.Create44Matrix(m1);
	}
	else
		compound.CreateCompound(arr);

	//当前块在复合坐标系下的范围
	m_pContext->SetCoordSys(&compound);
	
	CRect rcViewSave;
	CSize szDC = m_pContext->GetDCSize();
	m_pContext->GetViewRect(rcViewSave);
	m_pContext->SetDCSize(m_szBlock);
	m_pContext->SetViewRect(CRect(0,0,m_szBlock.cx,m_szBlock.cy));
	
	int new_num = 0;

	if( !PrepareData(blkIdx,id,0,m_szBlock.cx,0,m_szBlock.cy,new_num) )
	{
		m_pContext->SetDCSize(szDC);
		m_pContext->SetViewRect(rcViewSave);
		
		//恢复坐标系
		m_pContext->SetCoordSys(pCS);
		return FALSE;
	}

	m_pCacheData->BeginFillBlock(m_pContext,blkIdx,m_cacheTable.ids[blkIdx].load_num==0);	

	//绘制数据
	DrawClientWithCache(0,m_szBlock.cx,0,m_szBlock.cy,TRUE,m_cacheTable.ids[blkIdx].load_num==0,new_num==-1);

	m_pCacheData->EndFillBlock();
	m_pContext->SetDCSize(szDC);
	m_pContext->SetViewRect(rcViewSave);

	//恢复坐标系
	m_pContext->SetCoordSys(pCS);

	BOOL bRet = CDrawingLayer::FillBlock(blkIdx,id);

	if( new_num==-1 )
		m_cacheTable.ids[blkIdx].load_num = m_nLoadMaxNum;
	else		
		m_cacheTable.ids[blkIdx].load_num = new_num;

	return bRet;	
}

static int g_CenterX, g_CenterY;


BOOL CRealtimeVectLayer::PrepareData(int blkIdx, CacheID id, double xl, double xh, double yl, double yh, int& new_num)
{
	if( !m_b2DCoordSys )
		return PrepareData_3DCS(blkIdx,id,xl,xh,yl,yh,new_num);

	new_num = 0;
	if( m_pDQ==NULL )
		return FALSE;

	new_num = -1;
	CCoordSys *pCS = m_pContext->GetCoordSys();

	double gscale0 = pCS->CalcScale(); //大地坐标的一米等于屏幕上的多少像素
	double gscale = gscale0*m_pMainDS->GetScale()/1000.0; //纸质地图上一毫米等于屏幕上多少像素，1/gscale就是屏幕每个像素在地图上是多少毫米
	Envelope e0(xl,xh,yl,yh);

	e0.TransformCltToGrd(pCS);
	Envelope e00 = e0;

	//外扩5mm（因为DataQuery里面跳过了虚线线型、图元线型，一般虚线线型、图元线型距离母线不超过5mm）
	double d = 5*m_pMainDS->GetScale()/1000.0;
	e0.m_xl -= d; e0.m_xh += d;
	e0.m_yl -= d; e0.m_yh += d;
	
	GrElement *p = m_eleListBlockFtrs.head;
	while( p!=NULL )
	{
		if( p->pBuf && p->bmark1==0 )
		{
			delete (GrBuffer*)p->pBuf;
		}
		p = p->next;
	}
	
	m_eleListBlockFtrs.delAll();

	int nCount = m_pDQ->GetObjectCount();
	
	if( nCount<=0 )
		return TRUE;

	CPFeature *ftrs = new CPFeature[nCount];
	int num = 0;

	DWORD time0 = GetTickCount();

	for( int j=0; j<m_arrPBlockFtrs.GetSize(); j++)
	{
		BlockFtrs *pItem = m_arrPBlockFtrs[j];
		if( pItem->xnum==id.xnum && pItem->ynum==id.ynum )
			break;
	}

	BlockFtrs *pItem = NULL;
	if( j<m_arrPBlockFtrs.GetSize() )
	{
		pItem = m_arrPBlockFtrs[j];
		num = pItem->m_arrFtrs.GetSize();
		if( num==0 )
		{
			delete[] ftrs;
			return TRUE;
		}
		memcpy(ftrs,pItem->m_arrFtrs.GetData(),sizeof(CPFeature)*num);
	}
	else
	{
		pItem = new BlockFtrs();

		pItem->xnum = id.xnum;
		pItem->ynum = id.ynum;

		m_arrPBlockFtrs.Add(pItem);

		CSetMaxNumResultsOfSearch maxSearch(m_pDQ,2000);
		
		BOOL bOldLocked = m_pDQ->SetFilterIncludeLocked(TRUE);
		BOOL bOld = m_pDQ->SetFilterIncludeRefData(TRUE);
		num = m_pDQ->FindObjectInRect_SimpleAndSym(e0,NULL);
		m_pDQ->SetFilterIncludeRefData(bOld);
		m_pDQ->SetFilterIncludeLocked(bOldLocked);

		if( num==0 )
		{
			delete[] ftrs;
			new_num = -1;
			return TRUE;
		}

		//区域内地物数目不是很多时，下面的方法更快
		if( !m_pDQ->IsExceedMaxNumOfResult() )
		{		
			const CPFeature *ftrs2 = m_pDQ->GetFoundHandles(num);
			for( int i2=0; i2<num; i2++)
			{
				ftrs2[i2]->SetAppFlag(1);
			}
			
			int count = m_pDoc->GetDlgDataSourceCount();
			
			CDlgDataSource* pDS = NULL;
			
			num = 0;
			
			for( int k=count-1; k>=0; k--)	
			{
				pDS = m_pDoc->GetDlgDataSource(k);
			
				CFtrArray arrFtrs;
				pDS->GetAllFtrsByDisplayOrder(arrFtrs);

				int nFtrCount = arrFtrs.GetSize();
				for (int j=0; j<nFtrCount; j++)
				{
					CFeature *pFtr = arrFtrs[j];
					if (!pFtr) continue;
					
					if( !pFtr->IsVisible() )
						continue;
					
					if( num>=nCount )continue;
					
					if( pFtr->GetAppFlag()==1 )
					{
						ftrs[num++] = pFtr;
						pFtr->SetAppFlag(0);
					}
				}
				//按照层显示顺序获取地物
			/*	CFtrLayerArray arrLayers;
				pDS->GetAllFtrLayersByDisplayOrder(arrLayers);
				int nlay = arrLayers.GetSize(), k = 0;
				
				for (int i=0; i<nlay; i++)
				{
					CFtrLayer *pLayer = arrLayers[i];
					if (!pLayer) continue;
					
					if( pLayer->IsDeleted() || !pLayer->IsVisible() )
						continue;
					
					if( m_pDrawMan && !(m_pDrawMan->*m_pCheckLayerFun)(pLayer) )
						continue;

					CFtrArray arrFtrs;
					pLayer->GetAllFtrsByDisplayOrder(arrFtrs);
					
					int nFtrCount = arrFtrs.GetSize();
					for (int j=0; j<nFtrCount; j++)
					{
						CFeature *pFtr = arrFtrs[j];
						if (!pFtr) continue;
						
						if( !pFtr->IsVisible() )
							continue;

						if( num>=nCount )continue;
						
						if( pFtr->GetAppFlag()==1 )
						{
							ftrs[num++] = pFtr;
							pFtr->SetAppFlag(0);
						}
					}
				}
				*/
			}
		}
		else if( m_arrPBlockAllFtrs.GetSize()!=0 )
		{
			num = m_arrPBlockAllFtrs.GetSize();
			memcpy(ftrs,m_arrPBlockAllFtrs.GetData(),sizeof(CPFeature)*num);
		}
		//区域内地物数目很多时，完全遍历更快
		else
		{
			int count = m_pDoc->GetDlgDataSourceCount();
			
			CDlgDataSource* pDS = NULL;
			
			num = 0;
			
			for( int k=count-1; k>=0; k--)	
			{
				pDS = m_pDoc->GetDlgDataSource(k);
				
				CFtrArray arrFtrs;
				pDS->GetAllFtrsByDisplayOrder(arrFtrs);

				int nFtrCount = arrFtrs.GetSize();
				for (int j=0; j<nFtrCount; j++)
				{
					CFeature *pFtr = arrFtrs[j];
					if (!pFtr) continue;
					
					if( !pFtr->IsVisible() || pFtr->IsDeleted() )
						continue;
					
					// Intersect操作相对比较耗时，延迟到后面去做
					//if( !m_pDQ->Intersect(pFtr,e0,pCS) )
					//	continue;
					
					if( num>=nCount )continue;
					
					ftrs[num++] = pFtr;
				}
				//按照层显示顺序获取地物
			/*	CFtrLayerArray arrLayers;
				pDS->GetAllFtrLayersByDisplayOrder(arrLayers);
				int nlay = arrLayers.GetSize(), k = 0;
				
				for (int i=0; i<nlay; i++)
				{
					CFtrLayer *pLayer = arrLayers[i];
					if (!pLayer) continue;
					
					if( pLayer->IsDeleted() || !pLayer->IsVisible() )
						continue;
					
					if( m_pDrawMan && !(m_pDrawMan->*m_pCheckLayerFun)(pLayer) )
						continue;
					
					CFtrArray arrFtrs;
					pLayer->GetAllFtrsByDisplayOrder(arrFtrs);
					
					int nFtrCount = arrFtrs.GetSize();
					for (int j=0; j<nFtrCount; j++)
					{
						CFeature *pFtr = arrFtrs[j];
						if (!pFtr) continue;
						
						if( !pFtr->IsVisible() || pFtr->IsDeleted() )
							continue;

						// Intersect操作相对比较耗时，延迟到后面去做
						//if( !m_pDQ->Intersect(pFtr,e0,pCS) )
						//	continue;

						if( num>=nCount )continue;

						ftrs[num++] = pFtr;
					}
				}
			*/
			}

			m_arrPBlockAllFtrs.SetSize(num);
			memcpy(m_arrPBlockAllFtrs.GetData(),ftrs,num*sizeof(CPFeature));
		}

		pItem->m_arrFtrs.SetSize(num);
		memcpy(pItem->m_arrFtrs.GetData(),ftrs,sizeof(CPFeature)*num);

		pItem->m_arrFlags.SetSize(num);
		if( !m_pDQ->IsExceedMaxNumOfResult() )
		{
			memset(pItem->m_arrFlags.GetData(),2,num);
		}
		else
		{
			memset(pItem->m_arrFlags.GetData(),0,num);
		}
	}


	DWORD time1 = GetTickCount()-time0;
	
	{
		//char msg[256];
		//sprintf(msg,"prepare findobj %d\n",(int)time1);
		//OutputDebugString(msg);
	}

	if( 0 )
	{
		GrBuffer *pNewBuf = new GrBuffer();
		CString strMsg;
		//strMsg.Format("curBlock: %d,%d", id.xnum-g_CenterX,id.ynum-g_CenterY);
		strMsg.Format("curBlock: %d,%d", id.xnum,id.ynum);

		TextSettings set;
		set.fHeight = 20;
		strcpy(set.tcFaceName,"宋体");

		pNewBuf->Text(RGB(255,0,0),&PT_3D(e00.m_xl+(e00.m_xh-e00.m_xl)/8,(e00.m_yl+e00.m_yh)/2,0),
			strMsg,&set,FALSE);

		m_eleListBlockFtrs.addBuffer(pNewBuf);
	}
	if( 0 )
	{
		double d = 1/gscale;
		GrBuffer *pNewBuf = new GrBuffer();
		pNewBuf->BeginLineString(255,1,FALSE);
		pNewBuf->MoveTo(&PT_3D(e00.m_xl+d,e00.m_yl+d,0));
		pNewBuf->LineTo(&PT_3D(e00.m_xh-d,e00.m_yl+d,0));
		pNewBuf->LineTo(&PT_3D(e00.m_xh-d,e00.m_yh-d,0));
		pNewBuf->LineTo(&PT_3D(e00.m_xl+d,e00.m_yh-d,0));
		pNewBuf->LineTo(&PT_3D(e00.m_xl+d,e00.m_yl+d,0));
		pNewBuf->End();
		
		m_eleListBlockFtrs.addBuffer(pNewBuf);
	}

	int nDrawSymbolizedFlag = CalcSymbolizedFlag(gscale);
	
	int i;
	
	int start = m_cacheTable.ids[blkIdx].load_num;
	int ndraw = 0;
	BYTE *pFlags = pItem->m_arrFlags.GetData();

	time0 = GetTickCount();
	
	CDlgDataSource* pDS = NULL;

	CSymbolFlag sf1(CSymbolFlag::Tolerance,1/gscale0);
	CSymbolFlag sf2(CSymbolFlag::BreakCell,FALSE);
	i = start;

	new_num = 0;
	
	{
		int cache_limit = 2000, adjust = 0;
		for( i=start; i<num && ndraw<m_nDrawNum; i++)
		{
			CFeature *pFtr = ftrs[i];
			if( pFlags[i]==0 )
			{
				CGeometry *pGeo = pFtr->GetGeometry();

				// m_pDQ->Intersect(pFtr,e0,pCS) 比较费时
				if( nDrawSymbolizedFlag==SYMBOLIZE_NONE )
				{
					if( pGeo->GetEnvelope().bIntersect(&e0) )
						pFlags[i] = 2;
					else
					{
						pFlags[i] = 1;
						continue;
					}
				}
				else
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || 
					pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				{
					if( pGeo->GetEnvelope().bIntersect(&e0) )
						pFlags[i] = 2;
					else
					{
						pFlags[i] = 1;
						continue;
					}
				}
				else if( m_pDQ->Intersect(pFtr,e0,NULL) )
				{
					pFlags[i] = 2;
				}
				else
				{
					pFlags[i] = 1;
					continue;
				}
			}
			else if( pFlags[i]==1 )
			{
				continue;
			}

			int index;
			TempObj obj;
			if( FindObj((LONG_PTR)pFtr,index) )
			{
				obj = m_arrTempObjs[index];
				if( !obj.visible )
					continue;
			}

			ndraw++;

			BOOL bUseCacheBuf = TRUE;
			GrBuffer *pNewBuf = NULL;

			//每次使用cache中的Buffer，都会导致需要将一个元素移到尾部，头部的元素会被删除；
			//为了防止删除的元素有可能存在于m_eleListBlockFtrs中（这会导致内存泄露），
			//所以当移动次数超过cache的总长度时，就不再使用cahe了；
			//不过事实上，只要m_nDrawNum小于cache_limit，adjust也就不会超过cache_limit；
			if( adjust>=cache_limit )
			{
				bUseCacheBuf = FALSE;
			}

			index = -1;
			//首先在 m_arrCacheBufs 中查找 pFtr
			if( bUseCacheBuf && FindBuf((LONG_PTR)pFtr,index) )
			{
				pNewBuf = (GrBuffer*)(m_arrCacheBufs.GetAt(index)->pBuf);
				m_listCacheBufs.moveToTail(m_arrCacheBufs.GetAt(index));
				adjust++;
			}			
			else
			{
				pNewBuf = new GrBuffer();
				if( m_pDrawMan )
				{
					CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
					if( pDS )
					{
						(m_pDrawMan->*m_pDrawFun)(pDS,pDS->GetFtrLayerOfObject(pFtr),pFtr,nDrawSymbolizedFlag,pNewBuf);
					}

					if( obj.no!=0 && obj.usecolor!=0 )
					{
						pNewBuf->SetAllColor(obj.color);
					}
				}
				else
				{
					CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
					if( pDS )
					{
						pDS->DrawFeature(pFtr,pNewBuf);
					}
				}

				if( bUseCacheBuf )
				{
					GrElement *ele = new GrElement();
					ele->no = (LONG_PTR)pFtr;
					ele->pBuf = pNewBuf;
					m_arrCacheBufs.InsertAt(index,ele);
					m_listCacheBufs.add(ele);

					if( m_arrCacheBufs.GetSize()>=cache_limit )
					{
						if( FindBuf(m_listCacheBufs.head->no,index) )
						{
							ele = m_listCacheBufs.head;
							m_arrCacheBufs.RemoveAt(index);								
							m_listCacheBufs.del(ele);

							delete (GrBuffer*)(ele->pBuf);
							delete ele;
						}
						else
						{
							int a=1;//error
						}
					}

					adjust++;
				}
			}
			
			//bMark1 用来标识是缓存还是自创建的Buffer
			m_eleListBlockFtrs.addBuffer(pNewBuf,bUseCacheBuf);
		}

		new_num = i;
	}

	delete[] ftrs;

	if( i>=num )
	{
		new_num = -1;
	}

	time1 = GetTickCount()-time0;

	{
		//char msg[256];
		//sprintf(msg,"prepare %d\n",(int)time1);
		//OutputDebugString(msg);
	}

	return TRUE;
}


BOOL CRealtimeVectLayer::PrepareData(double xl, double xh, double yl, double yh, int draw_limit, CArray<CPFeature,CPFeature>& arrAllFtrs, int& draw_num)
{
	if( !m_b2DCoordSys )
		return PrepareData_3DCS(xl,xh,yl,yh,draw_limit,arrAllFtrs,draw_num);

	if( m_pDQ==NULL )
	{
		draw_num = -1;
		return FALSE;
	}	

	CCoordSys *pCS = m_pContext->GetCoordSys();

	double gscale0 = pCS->CalcScale(); //大地坐标的一米等于屏幕上的多少像素
	double gscale = gscale0*m_pMainDS->GetScale()/1000.0; //纸质地图上一毫米等于屏幕上多少像素，1/gscale就是屏幕每个像素在地图上是多少毫米
	Envelope e0(xl,xh,yl,yh);

	e0.TransformCltToGrd(pCS);
	Envelope e00 = e0;

	//外扩5mm（因为DataQuery里面跳过了虚线线型、图元线型，一般虚线线型、图元线型距离母线不超过5mm）
	double d = 5*m_pMainDS->GetScale()/1000.0;
	e0.m_xl -= d; e0.m_xh += d;
	e0.m_yl -= d; e0.m_yh += d;
	
	GrElement *p = m_eleListBlockFtrs.head;
	while( p!=NULL )
	{
		if( p->pBuf && p->bmark1==0 )
		{
			delete (GrBuffer*)p->pBuf;
		}
		p = p->next;
	}
	
	m_eleListBlockFtrs.delAll();

	int nCount = m_pDQ->GetObjectCount();
	
	if( nCount<=0 )
	{
		draw_num = -1;
		return TRUE;
	}

	CPFeature *ftrs = NULL;
	int num = 0;

	DWORD time0 = GetTickCount();

	if( arrAllFtrs.GetSize()<=0 )
	{
		//区域内地物数目很多时，完全遍历更快
		ftrs = new CPFeature[nCount];

		int count = m_pDoc->GetDlgDataSourceCount();
		
		CDlgDataSource* pDS = NULL;
		
		num = 0;
		
		for( int k=count-1; k>=0; k--)	
		{
			pDS = m_pDoc->GetDlgDataSource(k);
			
			CFtrArray arrFtrs;
			pDS->GetAllFtrsByDisplayOrder(arrFtrs);

			int nFtrCount = arrFtrs.GetSize();
			for (int j=0; j<nFtrCount; j++)
			{
				CFeature *pFtr = arrFtrs[j];
				if (!pFtr) continue;
				
				if( !pFtr->IsVisible() || pFtr->IsDeleted() )
					continue;
				
				if( !m_pDQ->Intersect(pFtr,e0,NULL) )
					continue;
				
				ftrs[num++] = pFtr;
			}

			//按照层显示顺序获取地物
		/*	CFtrLayerArray arrLayers;
			pDS->GetAllFtrLayersByDisplayOrder(arrLayers);
			int nlay = arrLayers.GetSize(), k = 0;
			
			for (int i=0; i<nlay; i++)
			{
				CFtrLayer *pLayer = arrLayers[i];
				if (!pLayer) continue;
				
				if( pLayer->IsDeleted() || !pLayer->IsVisible() )
					continue;
				
				if( m_pDrawMan && !(m_pDrawMan->*m_pCheckLayerFun)(pLayer) )
					continue;
				
				CFtrArray arrFtrs;
				pLayer->GetAllFtrsByDisplayOrder(arrFtrs);
				
				int nFtrCount = arrFtrs.GetSize();
				for (int j=0; j<nFtrCount; j++)
				{
					CFeature *pFtr = arrFtrs[j];
					if (!pFtr) continue;
					
					if( !pFtr->IsVisible() || pFtr->IsDeleted() )
						continue;

					if( !m_pDQ->Intersect(pFtr,e0) )
						continue;

					ftrs[num++] = pFtr;
				}
			}
		*/
		 }

		arrAllFtrs.SetSize(num);
		memcpy(arrAllFtrs.GetData(),ftrs,sizeof(CFeature*)*num);

		delete[] ftrs;

		ftrs = arrAllFtrs.GetData();

	}
	else
	{
		ftrs = arrAllFtrs.GetData();
		num = arrAllFtrs.GetSize();
	}

	DWORD time1 = GetTickCount()-time0;

	int nDrawSymbolizedFlag = CalcSymbolizedFlag(gscale);

	int i;

	int start = draw_num;
	int ndraw = 0;

	time0 = GetTickCount();
	
	CDlgDataSource* pDS = NULL;
	
	CSymbolFlag sf1(CSymbolFlag::Tolerance,1/gscale0);
	CSymbolFlag sf2(CSymbolFlag::BreakCell,FALSE);

	i = start;
	
	{	
		for( i=draw_num; i<num && ndraw<draw_limit; i++ )
		{
			CFeature *pFtr = ftrs[i];
		
			int index;
			TempObj obj;
			if( FindObj((LONG_PTR)pFtr,index) )
			{
				obj = m_arrTempObjs[index];
				if( !obj.visible )
					continue;
			}

			ndraw++;

			GrBuffer *pNewBuf = NULL;

			index = -1;
			{
				pNewBuf = new GrBuffer();
				if( m_pDrawMan )
				{
					CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
					if( pDS )
					{
						(m_pDrawMan->*m_pDrawFun)(pDS,pDS->GetFtrLayerOfObject(pFtr),pFtr,nDrawSymbolizedFlag,pNewBuf);
					}

					if( obj.no!=0 && obj.usecolor!=0 )
					{
						pNewBuf->SetAllColor(obj.color);
					}
				}
				else
				{
					CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
					if( pDS )
					{
						pDS->DrawFeature(pFtr,pNewBuf);
					}
				}
			}
			
			//bMark1 用来标识是缓存还是自创建的Buffer
			m_eleListBlockFtrs.addBuffer(pNewBuf);
		}

		draw_num = i;
	}

	if( i>=num )
	{
		draw_num = -1;		
	}

	time1 = GetTickCount()-time0;

	return TRUE;
}


void CRealtimeVectLayer::DrawClientWithCache(double xl, double xh, double yl, double yh, BOOL bUseCache, BOOL bStart, BOOL bEnd)
{
	DWORD time0 = GetTickCount();

	if( bStart )
	{
		if( bUseCache && m_pCacheData!=NULL )
		{
			m_pCacheData->DrawGrBuffer(&m_eleListTempObjsForemost,CDrawingContext::modeNormal);
		}
		else 
		{
			m_pContext->DrawGrBuffer(&m_eleListTempObjsForemost,CDrawingContext::modeNormal);
		}
	}

	if( bUseCache && m_pCacheData!=NULL )
	{
		m_pCacheData->DrawGrBuffer(&m_eleListBlockFtrs,CDrawingContext::modeNormal);
	}
	else 
	{
		m_pContext->DrawGrBuffer(&m_eleListBlockFtrs,CDrawingContext::modeNormal);
	}

	if( bEnd )
	{
		if( bUseCache && m_pCacheData!=NULL )
		{
			m_pCacheData->DrawGrBuffer(&m_eleListTempObjsUppermost,CDrawingContext::modeNormal);
		}
		else 
		{
			m_pContext->DrawGrBuffer(&m_eleListTempObjsUppermost,CDrawingContext::modeNormal);
		}
	}

	DWORD time1 = GetTickCount()-time0;

	{
		//char msg[256];
		//sprintf(msg,"Draw %d\n",(int)time1);
		//OutputDebugString(msg);
	}

}


void CRealtimeVectLayer::DrawClient(double xl, double xh, double yl, double yh)
{
	CCoordSys *pCS = m_pContext->GetCoordSys();
	if (!pCS) return;

	CCoordSys dataCS;
	dataCS.Create44Matrix(m_DataMatrix);
	
	//创造复合坐标系（也就是cache的坐标系）
	CCoordSys compound;
	CArray<CCoordSys*,CCoordSys*> arr;
	arr.Add(&dataCS);
	arr.Add(pCS);
	
	//使用单一的矩阵，速度更快
	if( pCS->GetCoordSysType()==COORDSYS_44MATRIX )
	{
		double m1[16], m2[16];
		pCS->GetMatrix(m1);
		
		matrix_multiply(m1,m_DataMatrix,4,m2);
		
		compound.Create44Matrix(m2);
	}
	else
		compound.CreateCompound(arr);
	
	//当前块在复合坐标系下的范围
	m_pContext->SetCoordSys(&compound);	

	int draw_num = 0;
	CArray<CPFeature,CPFeature> arrAllFtrs;
	while( draw_num!=-1 )
	{
		int old_num = draw_num;
		if( !PrepareData(xl,xh,yl,yh,1000,arrAllFtrs,draw_num) )
			break;

		DrawClientWithCache(xl,xh,yl,yh,FALSE,old_num==0, draw_num==-1);	
	}
	
	//恢复坐标系
	m_pContext->SetCoordSys(pCS);
}



void CRealtimeVectLayer::SetDataBound(Envelope e)
{
	m_DataBound = e;
	m_bCalcClientBound = FALSE;
}


void CRealtimeVectLayer::SetDataMatrix(double m[16])
{
	memcpy(m_DataMatrix,m,sizeof(m_DataMatrix));
	matrix_reverse(m_DataMatrix,4,m_DataRMatrix);
}

Envelope CRealtimeVectLayer::GetDataBound()
{
	Envelope e = m_DataBound;
	e.Transform(m_DataMatrix);
	return e;
}



BOOL CRealtimeVectLayer::ExtraPrepareCaches()
{
	if( !(GetAttribute()&CDrawingLayer::attrVisible) )
		return FALSE;

	if( !(GetAttribute()&CDrawingLayer::attrExtraPrepareCache) )
		return FALSE;

	if( m_pCacheData==NULL )
		return FALSE;

	CCoordSys *pCS = m_pContext->GetCoordSys();
	CRect rcView;
	m_pContext->GetViewRect(rcView);
	
	if( m_pCacheData!=NULL )
	{
		int view_x1=0, view_x2=0, view_y1=0, view_y2=0, view_x3=0, view_y3=0, view_x4=0, view_y4=0;

		DWORD time0 = GetTickCount();

		DrawDirtyObjs();

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

		//先从中心开始显示，offxy 是格网中沿原点中心逐步外扩的格网坐标；
		//逐渐外扩得通用算法写起来太复杂，我们就只把半径为2的点写出来，剩余的部分还是按照原来的做法去遍历；
		int offxy[] = {0,0, 
			1,0, -1,0, 0,-1, 0,1, 
			1,1, 1,-1, -1,-1, -1,1, 
			-2,0, 2,0, 0,-2, 0,2,
			-2,-1, -2,1, 2,-1, 2,1, -1,-2, 1,-2, -1,2, 1,2,
			-2,-2, -2,2, 2,2, 2,-2
		};

		BOOL bCoreReady = TRUE;

		int i0 = (nxmin+nxmax+1)/2, j0 = (nymin+nymax)/2, i1, j1;
		GetFocusBlockNum(rcView.CenterPoint(),i0,j0);
		int num = sizeof(offxy)/sizeof(offxy[0]);

		g_CenterX = i0;
		g_CenterY = j0;

		for( int i=0; i<num; i+=2)
		{
			i1 = offxy[i]+i0; j1 = offxy[i+1]+j0; 
			if( i1<nxmin || i1>nxmax )continue;
			if( j1<nymin || j1>nymax )continue;

			CacheID id(i1,j1);
			
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

			if( i==0 )
			{
				m_nDrawNum = 150;
			}
			else if( i<=8 )
			{
				m_nDrawNum = 100;
			}
			else if( i<=16 )
			{
				m_nDrawNum = 50;
			}
			else
				m_nDrawNum = 30;

			if( !bCoreReady && i>=16 )
			{
				m_nDrawNum = 4;
			}

			if( m_nLoadMaxNum<20000 )
			{
				m_nDrawNum = m_nDrawNum*2;
			}

			if( !m_bEnableDelayDraw )
			{
				m_nDrawNum = m_nLoadMaxNum;
			}
			
			int blkIdx = ForceFillBlock(id);
			if( blkIdx<0 )continue;

			if( m_cacheTable.ids[blkIdx].load_num<m_nLoadMaxNum )
			{
				bCoreReady = FALSE;
			}
			
//			if( !bCoreReady && i>=16 )
//				break;
		}

		if( !bCoreReady )
			m_nDrawNum = 2;
		else
			m_nDrawNum = 50;

		//剩余的部分还是按照原来的做法去遍历；
		for( i=nxmin; i<=nxmax; i++)
		{
			for( int j=nymin; j<=nymax; j++)
			{				
				//跳过已经显示的部分
				if( i-i0<=2 && i-i0>=-2 && j-j0<=2 && j-j0>=-2 )continue;

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

				int blkIdx = ForceFillBlock(id);
				if( blkIdx<0 )continue;
				
			}
		}

		DWORD time1 = GetTickCount()-time0;

		CString msg;
		msg.Format("ExtraPrepareCaches %d ms\n",(int)time1);

		//OutputDebugString(msg);

	}

	return TRUE;
}

void CRealtimeVectLayer::Draw()
{
	if( !(GetAttribute()&CDrawingLayer::attrVisible) )
		return;

	CCoordSys *pCS = m_pContext->GetCoordSys();
	CRect rcView;
	m_pContext->GetViewRect(rcView);
	
	if( m_pCacheData!=NULL )
	{
		int view_x1=0, view_x2=0, view_y1=0, view_y2=0, view_x3=0, view_y3=0, view_x4=0, view_y4=0;

		DrawDirtyObjs();

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

		//先从中心开始显示，offxy 是格网中沿原点中心逐步外扩的格网坐标；
		//逐渐外扩得通用算法写起来太复杂，我们就只把半径为2的点写出来，剩余的部分还是按照原来的做法去遍历；
		int offxy[] = {0,0, 
			1,0, -1,0, 0,-1, 0,1, 
			1,1, 1,-1, -1,-1, -1,1, 
			-2,0, 2,0, 0,-2, 0,2,
			-2,-1, -2,1, 2,-1, 2,1, -1,-2, 1,-2, -1,2, 1,2,
			-2,-2, -2,2, 2,2, 2,-2
		};

		BOOL bCoreReady = TRUE;

		int i0 = (nxmin+nxmax+1)/2, j0 = (nymin+nymax)/2, i1, j1;
		GetFocusBlockNum(rcView.CenterPoint(),i0,j0);
		int num = sizeof(offxy)/sizeof(offxy[0]);

		g_CenterX = i0;
		g_CenterY = j0;

		for( int i=0; i<num; i+=2)
		{
			i1 = offxy[i]+i0; j1 = offxy[i+1]+j0; 
			if( i1<nxmin || i1>nxmax )continue;
			if( j1<nymin || j1>nymax )continue;

			CacheID id(i1,j1);
			
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

			if( i==0 )
			{
				m_nDrawNum = 150;
			}
			else if( i<=8 )
			{
				m_nDrawNum = 100;
			}
			else if( i<=16 )
			{
				m_nDrawNum = 50;
			}
			else
				m_nDrawNum = 30;

			if( !bCoreReady && i>=16 )
			{
				m_nDrawNum = 4;
			}

			if( m_nLoadMaxNum<20000 )
			{
				m_nDrawNum = m_nDrawNum*2;
			}

			if( !m_bEnableDelayDraw )
			{
				m_nDrawNum = m_nLoadMaxNum;
			}
			
			int blkIdx = (m_bForceDraw?ForceFillBlock(id):GetBlockIdx(id));
			if( blkIdx<0 )continue;

			if( m_cacheTable.ids[blkIdx].load_num<m_nLoadMaxNum )
			{
				bCoreReady = FALSE;
			}
			
			m_pCacheData->DisplayBlock(m_pContext,blkIdx,x,y);

//			if( !bCoreReady && i>=16 )
//				break;
		}

		if( !bCoreReady )
			m_nDrawNum = 2;
		else
			m_nDrawNum = 50;

		//剩余的部分还是按照原来的做法去遍历；
		for( i=nxmin; i<=nxmax; i++)
		{
			for( int j=nymin; j<=nymax; j++)
			{				
				//跳过已经显示的部分
				if( i-i0<=2 && i-i0>=-2 && j-j0<=2 && j-j0>=-2 )continue;

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


int CRealtimeVectLayer::ForceFillBlock(CacheID id)
{
	if( m_pDQ==NULL )
		return -1;
	if( m_pCacheData==NULL )
		return -1;

	m_nLoadMaxNum = m_pDQ->GetObjectCount();
	
	int blkIdx = GetBlockIdx( id );	

	if( blkIdx>=0 && blkIdx<m_cacheTable.nsum )
	{	
		if( m_cacheTable.ids[blkIdx].load_num<m_nLoadMaxNum )
		{
			FillBlock(blkIdx,id);
		}
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


BOOL CRealtimeVectLayer::CheckCacheReady()
{
	if( m_pDQ==NULL )
		return FALSE;

	if( m_pCacheData==NULL )
		return TRUE;

	m_nLoadMaxNum = m_pDQ->GetObjectCount();

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

				if( m_cacheTable.ids[blkIdx].load_num<m_nLoadMaxNum )
					return FALSE;
			}
		}
	}

	return TRUE;
}



void *CRealtimeVectLayer::OpenObj(LONG_PTR objnum, BOOL bUppermost)
{
	int i;
	// it is found!
	if( FindObj(objnum,i) )
	{
		TempObj obj = m_arrTempObjs.GetAt(i);
		
		return obj.pBuf;
	}

	TempObj obj;
	
	obj.pBuf = new GrBuffer();
	obj.no = objnum;
	m_arrTempObjs.InsertAt(i,obj);

	GrElement *ele = new GrElement();
	ele->no = (LONG_PTR)objnum;
	ele->pBuf = obj.pBuf;

	if(!bUppermost )
		m_eleListTempObjsForemost.add(ele);
	else
		m_eleListTempObjsUppermost.add(ele);
	
	return obj.pBuf;
}


void CRealtimeVectLayer::FinishObj(LONG_PTR objnum)
{
	
}


void CRealtimeVectLayer::DelObj(LONG_PTR objnum)
{
	int i;
	
	//地物可能被删除了
	for( i=0; i<m_arrPBlockFtrs.GetSize(); i++)
	{
		delete m_arrPBlockFtrs[i];
	}
	
	m_arrPBlockFtrs.RemoveAll();
	m_arrPBlockAllFtrs.RemoveAll();
	
	for( i=0; i<m_arrCacheBufs.GetSize(); i++)
	{
		if (m_arrCacheBufs[i]->no == (LONG_PTR)objnum)
		{
			m_listCacheBufs.del(m_arrCacheBufs[i]);
			
			delete (GrBuffer*)m_arrCacheBufs[i]->pBuf;
			delete m_arrCacheBufs[i];
			
			m_arrCacheBufs.RemoveAt(i);
			
			break;
		}
		
	}

	if( !FindObj(objnum,i) )
	{
		return;
	}

	TempObj obj = m_arrTempObjs.GetAt(i);
		
	m_arrTempObjs.RemoveAt(i);
	if( obj.pBuf )
	{
		delete obj.pBuf;
		
		GrElement *ph = m_eleListTempObjsForemost.head;
		while( ph!=NULL )
		{
			if( ph->no==(LONG_PTR)objnum )
			{
				m_eleListTempObjsForemost.del(ph);
				delete ph;
				break;
			}
			ph = ph->next;
		}
		ph = m_eleListTempObjsUppermost.head;
		while( ph!=NULL )
		{
			if( ph->no==(LONG_PTR)objnum )
			{
				m_eleListTempObjsUppermost.del(ph);
				delete ph;
				break;
			}
			ph = ph->next;
		}
	}

}


void CRealtimeVectLayer::DelAllObjs()
{
	int size = m_arrTempObjs.GetSize();
	for( int i=0; i<size; i++)
	{
		TempObj obj = m_arrTempObjs.GetAt(i);
		if( obj.pBuf )delete obj.pBuf;
	}
	
	m_arrTempObjs.RemoveAll();

	DeleteElementList(m_eleListTempObjsForemost);
	DeleteElementList(m_eleListTempObjsUppermost);

	//地物可能被删除了
	for( i=0; i<m_arrPBlockFtrs.GetSize(); i++)
	{
		delete m_arrPBlockFtrs[i];
	}
	
	m_arrPBlockFtrs.RemoveAll();
	m_arrPBlockAllFtrs.RemoveAll();

	for( i=0; i<m_arrCacheBufs.GetSize(); i++)
	{
		delete (GrBuffer*)m_arrCacheBufs[i]->pBuf;
		delete m_arrCacheBufs[i];
	}
	
	m_arrCacheBufs.RemoveAll();
	m_listCacheBufs.delAll();

	GrElement *p = m_eleListBlockFtrs.head;
	while( p!=NULL )
	{
		if( p->pBuf && p->bmark1==0 )
		{
			delete (GrBuffer*)p->pBuf;
		}
		p = p->next;
	}
	
	m_eleListBlockFtrs.delAll();

	for( i=0; i<m_arrDirtyPBufs.GetSize(); i++)
	{
		delete m_arrDirtyPBufs[i];
	}
	
	m_arrDirtyPBufs.RemoveAll();
}




void CRealtimeVectLayer::SetObjColor(LONG_PTR objnum, COLORREF color)
{
	GrElement *p = FindEleListTempObjs(objnum);
	if(p)
	{
		if(p->pBuf)
			((GrBuffer*)p->pBuf)->SetAllColor(color);
	}

	int i;
	if( !FindObj(objnum,i) )
	{
		TempObj obj;
		obj.no = objnum;
		obj.color = color;
		obj.usecolor = 1;
		m_arrTempObjs.InsertAt(i,obj);
		return;
	}
	else
	{
		if( m_arrTempObjs[i].pBuf )
			m_arrTempObjs[i].pBuf->SetAllColor(color);
		else
		{
			m_arrTempObjs.GetPtAt(i)->color = color; 
		}
	}
}


void CRealtimeVectLayer::SetObjVisible(LONG_PTR objnum, BOOL bVisble)
{
	GrElement *p = FindEleListTempObjs(objnum);
	if(p)
	{
		p->bvisible = bVisble?1:0;
	}

	int i;
	if( !FindObj(objnum,i) )
	{
		if( !bVisble )
		{
			TempObj obj;
			obj.no = objnum;
			obj.visible = bVisble?1:0;
			m_arrTempObjs.InsertAt(i,obj);
		}
		return;
	}
	else
	{
		m_arrTempObjs.GetPtAt(i)->visible = (bVisble?1:0);
	}	
}


BOOL CRealtimeVectLayer::FindObj(LONG_PTR objnum, int& insert_idx)
{
	int findidx = -1, bnew = 0;
	if( m_arrTempObjs.GetSize()<=0 ){ findidx = 0; bnew = 1; }
	else
	{
		int i0 = 0, i1 = m_arrTempObjs.GetSize()-1, i2;
		while(findidx==-1) 
		{
			if( objnum<=m_arrTempObjs.GetAt(i0).no )
			{
				bnew = (objnum==m_arrTempObjs.GetAt(i0).no?0:1);
				findidx = i0;
				break;
			}
			else if( objnum>=m_arrTempObjs.GetAt(i1).no )
			{
				bnew = (objnum==m_arrTempObjs.GetAt(i1).no?0:1);
				findidx = (objnum==m_arrTempObjs.GetAt(i1).no?i1:(i1+1));
				break;
			}
			
			i2 = (i0+i1)/2;
			if( i2==i0 )
			{
				bnew = (objnum==m_arrTempObjs.GetAt(i0).no?0:1);
				findidx = (objnum==m_arrTempObjs.GetAt(i0).no?i0:(i0+1));
				break;
			}
			
			if( objnum<m_arrTempObjs.GetAt(i2).no )
				i1 = i2;
			else if( objnum>m_arrTempObjs.GetAt(i2).no )
				i0 = i2;
			else
			{
				findidx = i2; bnew = 0;
				break;
			}
		}
	}
	
	insert_idx = findidx;
	return (bnew==0);
}


GrElement *CRealtimeVectLayer::FindEleListTempObjs(LONG_PTR objnum)
{
	GrElement *p = m_eleListTempObjsForemost.head;
	while(p)
	{
		if(p->no==objnum)
		{
			return p;
		}
		p = p->next;
	}

	p = m_eleListTempObjsUppermost.head;
	while(p)
	{
		if(p->no==objnum)
		{
			return p;
		}
		p = p->next;
	}
	return NULL;
}

BOOL CRealtimeVectLayer::FindBuf(LONG_PTR objnum, int& insert_idx)
{
	int findidx = -1, bnew = 0;
	if( m_arrCacheBufs.GetSize()<=0 ){ findidx = 0; bnew = 1; }
	else
	{
		int i0 = 0, i1 = m_arrCacheBufs.GetSize()-1, i2;
		while(findidx==-1) 
		{
			if( objnum<=m_arrCacheBufs.GetAt(i0)->no )
			{
				bnew = (objnum==m_arrCacheBufs.GetAt(i0)->no?0:1);
				findidx = i0;
				break;
			}
			else if( objnum>=m_arrCacheBufs.GetAt(i1)->no )
			{
				bnew = (objnum==m_arrCacheBufs.GetAt(i1)->no?0:1);
				findidx = (objnum==m_arrCacheBufs.GetAt(i1)->no?i1:(i1+1));
				break;
			}
			
			i2 = (i0+i1)/2;
			if( i2==i0 )
			{
				bnew = (objnum==m_arrCacheBufs.GetAt(i0)->no?0:1);
				findidx = (objnum==m_arrCacheBufs.GetAt(i0)->no?i0:(i0+1));
				break;
			}
			
			if( objnum<m_arrCacheBufs.GetAt(i2)->no )
				i1 = i2;
			else if( objnum>m_arrCacheBufs.GetAt(i2)->no )
				i0 = i2;
			else
			{
				findidx = i2; bnew = 0;
				break;
			}
		}
	}
	
	insert_idx = findidx;
	return (bnew==0);
}



BOOL CRealtimeVectLayer::PrepareData_3DCS(int blkIdx, CacheID id, double xl, double xh, double yl, double yh, int& new_num)
{
	new_num = 0;
	if( m_pDQ==NULL )
		return FALSE;

	new_num = -1;
	CCoordSys *pCS = m_pContext->GetCoordSys();

	double gscale0 = pCS->CalcScale(); //大地坐标的一米等于屏幕上的多少像素
	double gscale = gscale0*m_pMainDS->GetScale()/1000.0; //纸质地图上一毫米等于屏幕上多少像素，1/gscale就是屏幕每个像素在地图上是多少毫米
	Envelope e0(xl,xh,yl,yh);

	//外扩5mm（因为DataQuery里面跳过了虚线线型、图元线型，一般虚线线型、图元线型距离母线不超过5mm）
	double d = 5*m_pMainDS->GetScale()/1000.0;
	e0.m_xl -= d; e0.m_xh += d;
	e0.m_yl -= d; e0.m_yh += d;
	
	GrElement *p = m_eleListBlockFtrs.head;
	while( p!=NULL )
	{
		if( p->pBuf && p->bmark1==0 )
		{
			delete (GrBuffer*)p->pBuf;
		}
		p = p->next;
	}
	
	m_eleListBlockFtrs.delAll();

	int nCount = m_pDQ->GetObjectCount();
	
	if( nCount<=0 )
		return TRUE;

	CPFeature *ftrs = new CPFeature[nCount];
	int num = 0;

	DWORD time0 = GetTickCount();

	for( int j=0; j<m_arrPBlockFtrs.GetSize(); j++)
	{
		BlockFtrs *pItem = m_arrPBlockFtrs[j];
		if( pItem->xnum==id.xnum && pItem->ynum==id.ynum )
			break;
	}

	BlockFtrs *pItem = NULL;
	if( j<m_arrPBlockFtrs.GetSize() )
	{
		pItem = m_arrPBlockFtrs[j];
		num = pItem->m_arrFtrs.GetSize();
		if( num==0 )
		{
			delete[] ftrs;
			return TRUE;
		}
		memcpy(ftrs,pItem->m_arrFtrs.GetData(),sizeof(CPFeature)*num);
	}
	else
	{
		pItem = new BlockFtrs();

		pItem->xnum = id.xnum;
		pItem->ynum = id.ynum;

		m_arrPBlockFtrs.Add(pItem);

		CSetMaxNumResultsOfSearch maxSearch(m_pDQ,2000);
		
		BOOL bOldLocked = m_pDQ->SetFilterIncludeLocked(TRUE);
		BOOL bOld = m_pDQ->SetFilterIncludeRefData(TRUE);
		num = m_pDQ->FindObjectInRect_SimpleAndSym(e0,pCS);
		m_pDQ->SetFilterIncludeRefData(bOld);
		m_pDQ->SetFilterIncludeLocked(bOldLocked);

		if( num==0 )
		{
			delete[] ftrs;
			new_num = -1;
			return TRUE;
		}

		//区域内地物数目不是很多时，下面的方法更快
		if( !m_pDQ->IsExceedMaxNumOfResult() )
		{		
			const CPFeature *ftrs2 = m_pDQ->GetFoundHandles(num);
			for( int i2=0; i2<num; i2++)
			{
				ftrs2[i2]->SetAppFlag(1);
			}
			
			int count = m_pDoc->GetDlgDataSourceCount();
			
			CDlgDataSource* pDS = NULL;
			
			num = 0;
			
			for( int k=count-1; k>=0; k--)	
			{
				pDS = m_pDoc->GetDlgDataSource(k);
			
				CFtrArray arrFtrs;
				pDS->GetAllFtrsByDisplayOrder(arrFtrs);

				int nFtrCount = arrFtrs.GetSize();
				for (int j=0; j<nFtrCount; j++)
				{
					CFeature *pFtr = arrFtrs[j];
					if (!pFtr) continue;
					
					if( !pFtr->IsVisible() )
						continue;
					
					if( num>=nCount )continue;
					
					if( pFtr->GetAppFlag()==1 )
					{
						ftrs[num++] = pFtr;
						pFtr->SetAppFlag(0);
					}
				}
			}
		}
		else if( m_arrPBlockAllFtrs.GetSize()!=0 )
		{
			num = m_arrPBlockAllFtrs.GetSize();
			memcpy(ftrs,m_arrPBlockAllFtrs.GetData(),sizeof(CPFeature)*num);
		}
		//区域内地物数目很多时，完全遍历更快
		else
		{
			int count = m_pDoc->GetDlgDataSourceCount();
			
			CDlgDataSource* pDS = NULL;
			
			num = 0;
			
			for( int k=count-1; k>=0; k--)	
			{
				pDS = m_pDoc->GetDlgDataSource(k);
				
				CFtrArray arrFtrs;
				pDS->GetAllFtrsByDisplayOrder(arrFtrs);

				int nFtrCount = arrFtrs.GetSize();
				for (int j=0; j<nFtrCount; j++)
				{
					CFeature *pFtr = arrFtrs[j];
					if (!pFtr) continue;
					
					if( !pFtr->IsVisible() || pFtr->IsDeleted() )
						continue;
					
					if( num>=nCount )continue;
					
					ftrs[num++] = pFtr;
				}
			}

			m_arrPBlockAllFtrs.SetSize(num);
			memcpy(m_arrPBlockAllFtrs.GetData(),ftrs,num*sizeof(CPFeature));
		}

		pItem->m_arrFtrs.SetSize(num);
		memcpy(pItem->m_arrFtrs.GetData(),ftrs,sizeof(CPFeature)*num);

		pItem->m_arrFlags.SetSize(num);
		if( !m_pDQ->IsExceedMaxNumOfResult() )
		{
			memset(pItem->m_arrFlags.GetData(),2,num);
		}
		else
		{
			memset(pItem->m_arrFlags.GetData(),0,num);
		}
	}

	int nDrawSymbolizedFlag = CalcSymbolizedFlag(gscale);

	int i;

	int start = m_cacheTable.ids[blkIdx].load_num;
	int ndraw = 0;
	BYTE *pFlags = pItem->m_arrFlags.GetData();

	time0 = GetTickCount();
	
	CDlgDataSource* pDS = NULL;

	CSymbolFlag sf1(CSymbolFlag::Tolerance,1/gscale0);
	CSymbolFlag sf2(CSymbolFlag::BreakCell,FALSE);
	i = start;

	new_num = 0;
	
	{
		int cache_limit = 2000, adjust = 0;
		for( i=start; i<num && ndraw<m_nDrawNum; i++)
		{
			CFeature *pFtr = ftrs[i];
			if( pFlags[i]==0 )
			{
				CGeometry *pGeo = pFtr->GetGeometry();

				// m_pDQ->Intersect(pFtr,e0) 比较费时
				if( nDrawSymbolizedFlag==SYMBOLIZE_NONE )
				{
					if( GIsIntersectEnvelope(&pGeo->GetEnvelope(),&e0,pCS) )
						pFlags[i] = 2;
					else
					{
						pFlags[i] = 1;
						continue;
					}
				}
				else
				if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) || 
					pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
				{
					if( GIsIntersectEnvelope(&pGeo->GetEnvelope(),&e0,pCS) )
						pFlags[i] = 2;
					else
					{
						pFlags[i] = 1;
						continue;
					}
				}
				else if( m_pDQ->Intersect(pFtr,e0,pCS) )
				{
					pFlags[i] = 2;
				}
				else
				{
					pFlags[i] = 1;
					continue;
				}
			}
			else if( pFlags[i]==1 )
			{
				continue;
			}

			int index;
			TempObj obj;
			if (FindObj((LONG_PTR)pFtr, index))
			{
				obj = m_arrTempObjs[index];
				if( !obj.visible )
					continue;
			}

			ndraw++;

			BOOL bUseCacheBuf = TRUE;
			GrBuffer *pNewBuf = NULL;

			//每次使用cache中的Buffer，都会导致需要将一个元素移到尾部，头部的元素会被删除；
			//为了防止删除的元素有可能存在于m_eleListBlockFtrs中（这会导致内存泄露），
			//所以当移动次数超过cache的总长度时，就不再使用cahe了；
			//不过事实上，只要m_nDrawNum小于cache_limit，adjust也就不会超过cache_limit；
			if( adjust>=cache_limit )
			{
				bUseCacheBuf = FALSE;
			}

			index = -1;
			//首先在 m_arrCacheBufs 中查找 pFtr
			if (bUseCacheBuf && FindBuf((LONG_PTR)pFtr, index))
			{
				pNewBuf = (GrBuffer*)(m_arrCacheBufs.GetAt(index)->pBuf);
				m_listCacheBufs.moveToTail(m_arrCacheBufs.GetAt(index));
				adjust++;
			}			
			else
			{
				pNewBuf = new GrBuffer();
				if( m_pDrawMan )
				{
					CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
					if( pDS )
					{
						(m_pDrawMan->*m_pDrawFun)(pDS,pDS->GetFtrLayerOfObject(pFtr),pFtr,nDrawSymbolizedFlag,pNewBuf);
					}

					if( obj.no!=0 && obj.usecolor!=0 )
					{
						pNewBuf->SetAllColor(obj.color);
					}
				}
				else
				{
					CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
					if( pDS )
					{
						pDS->DrawFeature(pFtr,pNewBuf);
					}
				}

				if( bUseCacheBuf )
				{
					GrElement *ele = new GrElement();
					ele->no = (LONG_PTR)pFtr;
					ele->pBuf = pNewBuf;
					m_arrCacheBufs.InsertAt(index,ele);
					m_listCacheBufs.add(ele);

					if( m_arrCacheBufs.GetSize()>=cache_limit )
					{
						if( FindBuf(m_listCacheBufs.head->no,index) )
						{
							ele = m_listCacheBufs.head;
							m_arrCacheBufs.RemoveAt(index);								
							m_listCacheBufs.del(ele);

							delete (GrBuffer*)(ele->pBuf);
							delete ele;
						}
						else
						{
							int a=1;//error
						}
					}

					adjust++;
				}
			}
			
			//bMark1 用来标识是缓存还是自创建的Buffer
			m_eleListBlockFtrs.addBuffer(pNewBuf,bUseCacheBuf);
		}

		new_num = i;
	}

	delete[] ftrs;

	if( i>=num )
	{
		new_num = -1;
	}

	return TRUE;
}


BOOL CRealtimeVectLayer::PrepareData_3DCS(double xl, double xh, double yl, double yh, int draw_limit, CArray<CPFeature,CPFeature>& arrAllFtrs, int& draw_num)
{
	if( m_pDQ==NULL )
	{
		draw_num = -1;
		return FALSE;
	}	

	CCoordSys *pCS = m_pContext->GetCoordSys();

	double gscale0 = pCS->CalcScale(); //大地坐标的一米等于屏幕上的多少像素
	double gscale = gscale0*m_pMainDS->GetScale()/1000.0; //纸质地图上一毫米等于屏幕上多少像素，1/gscale就是屏幕每个像素在地图上是多少毫米
	Envelope e0(xl,xh,yl,yh);

	//外扩5mm（因为DataQuery里面跳过了虚线线型、图元线型，一般虚线线型、图元线型距离母线不超过5mm）
	double d = 5*m_pMainDS->GetScale()/1000.0;
	e0.m_xl -= d; e0.m_xh += d;
	e0.m_yl -= d; e0.m_yh += d;
	
	GrElement *p = m_eleListBlockFtrs.head;
	while( p!=NULL )
	{
		if( p->pBuf && p->bmark1==0 )
		{
			delete (GrBuffer*)p->pBuf;
		}
		p = p->next;
	}
	
	m_eleListBlockFtrs.delAll();

	int nCount = m_pDQ->GetObjectCount();
	
	if( nCount<=0 )
	{
		draw_num = -1;
		return TRUE;
	}

	CPFeature *ftrs = NULL;
	int num = 0;

	DWORD time0 = GetTickCount();

	if( arrAllFtrs.GetSize()<=0 )
	{
		//区域内地物数目很多时，完全遍历更快
		ftrs = new CPFeature[nCount];

		int count = m_pDoc->GetDlgDataSourceCount();
		
		CDlgDataSource* pDS = NULL;
		
		num = 0;
		
		for( int k=count-1; k>=0; k--)	
		{
			pDS = m_pDoc->GetDlgDataSource(k);
			
			CFtrArray arrFtrs;
			pDS->GetAllFtrsByDisplayOrder(arrFtrs);

			int nFtrCount = arrFtrs.GetSize();
			for (int j=0; j<nFtrCount; j++)
			{
				CFeature *pFtr = arrFtrs[j];
				if (!pFtr) continue;
				
				if( !pFtr->IsVisible() || pFtr->IsDeleted() )
					continue;
				
				if( !m_pDQ->Intersect(pFtr,e0,pCS) )
					continue;
				
				ftrs[num++] = pFtr;
			}

		 }

		arrAllFtrs.SetSize(num);
		memcpy(arrAllFtrs.GetData(),ftrs,sizeof(CFeature*)*num);

		delete[] ftrs;

		ftrs = arrAllFtrs.GetData();

	}
	else
	{
		ftrs = arrAllFtrs.GetData();
		num = arrAllFtrs.GetSize();
	}

	DWORD time1 = GetTickCount()-time0;

	int nDrawSymbolizedFlag = CalcSymbolizedFlag(gscale);

	int i;

	int start = draw_num;
	int ndraw = 0;

	time0 = GetTickCount();
	
	CDlgDataSource* pDS = NULL;
	
	CSymbolFlag sf1(CSymbolFlag::Tolerance,1/gscale0);
	CSymbolFlag sf2(CSymbolFlag::BreakCell,FALSE);

	i = start;
	
	{	
		for( i=draw_num; i<num && ndraw<draw_limit; i++ )
		{
			CFeature *pFtr = ftrs[i];
		
			int index;
			TempObj obj;
			if (FindObj((LONG_PTR)pFtr, index))
			{
				obj = m_arrTempObjs[index];
				if( !obj.visible )
					continue;
			}

			ndraw++;

			GrBuffer *pNewBuf = NULL;

			index = -1;
			{
				pNewBuf = new GrBuffer();
				if( m_pDrawMan )
				{
					CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
					if( pDS )
					{
						(m_pDrawMan->*m_pDrawFun)(pDS,pDS->GetFtrLayerOfObject(pFtr),pFtr,nDrawSymbolizedFlag,pNewBuf);
					}

					if( obj.no!=0 && obj.usecolor!=0 )
					{
						pNewBuf->SetAllColor(obj.color);
					}
				}
				else
				{
					CDlgDataSource *pDS = m_pDoc->GetDlgDataSourceOfFtr(pFtr);
					if( pDS )
					{
						pDS->DrawFeature(pFtr,pNewBuf);
					}
				}
			}
			
			//bMark1 用来标识是缓存还是自创建的Buffer
			m_eleListBlockFtrs.addBuffer(pNewBuf);
		}

		draw_num = i;
	}

	if( i>=num )
	{
		draw_num = -1;		
	}

	time1 = GetTickCount()-time0;

	return TRUE;
}

int CRealtimeVectLayer::CalcSymbolizedFlag(double gscale)
{
	int nDrawSymbolizedFlag = SYMBOLIZE_ALL;
	if( gscale<0.1 )
	{
		nDrawSymbolizedFlag = SYMBOLIZE_NONE;
	}
	else if( gscale<1 )
	{
		nDrawSymbolizedFlag = SYMBOLIZE_PART;
	}
	
	if( !m_bSymbolized )
		nDrawSymbolizedFlag = SYMBOLIZE_NONE;

	return nDrawSymbolizedFlag;
}

void CRealtimeVectLayer::EnableSymbolized(BOOL bSymbolized)
{
	m_bSymbolized = bSymbolized;
}


void CRealtimeVectLayer::Enable2DCoordSys(BOOL b2DCoordSys)
{
	m_b2DCoordSys = b2DCoordSys;
}
