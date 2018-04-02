// VectorLayer.cpp: implementation of the CVectorLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VectorLayer.h"
#include "matrix.h"
#include <math.h>
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin


IMPLEMENT_DYNAMIC(CVectorLayer,CDrawingLayer)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVectorLayer::CVectorLayer(int nDataType):
m_arrPEles(4096)
{
	m_nType = nDataType;
	m_bCalcClientBound = FALSE;
	m_pCacheData = NULL;

	m_bForceDraw = TRUE;

	m_fMarkWidth1 = 2; 
	m_fMarkWidth2 = 1;
	m_clrMark1 = RGB(255,255,255);
	m_clrMark2 = RGB(255,255,255);

	m_bDrawVect = TRUE;
	m_bDrawMark1 = FALSE;
	m_bDrawMark2 = FALSE;

	m_DataBound = Envelope(0,1000,0,1000);

	matrix_toIdentity(m_DataMatrix,4);
	matrix_toIdentity(m_DataRMatrix,4);
}

CVectorLayer::~CVectorLayer()
{
	Destroy();
}

void CVectorLayer::SetDataType(int nDataType)
{
	if( m_nType!=nDataType )
	{
		DelAllObjs();
	}
	m_nType = nDataType;
}

int CVectorLayer::GetDataType()
{
	return m_nType;
}

void CVectorLayer::Destroy()
{
	CDrawingLayer::Destroy();
	
	DelAllObjs();

	m_bCalcClientBound = FALSE;
	m_pCacheData = NULL;
}

BOOL CVectorLayer::InitCache(int nCacheType, CSize szBlock, int nBlock, DWORD clrBK)
{
	if (IsValid())  Destroy();

	if( nCacheType==CHEDATA_TYPE_BMP )
	{
		CCacheBmpData *pCache = new CCacheBmpData;
		if( pCache!=NULL )
		{
			pCache->SetColorTable(8,NULL);
			pCache->EnableTransparentOverlap(TRUE);
			pCache->CreateProgDataEncoder();
			
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
			pCache->m_bUseAlpha = TRUE;
			pCache->SetD3D(m_pContext->m_pD3D,TRUE);
			m_pCacheData = pCache;
		}
	}

	if( !CDrawingLayer::InitCache(nCacheType, szBlock, nBlock, clrBK) )
		return FALSE;
	return TRUE;
}

void *CVectorLayer::CreateObject()
{
	if( m_nType==typeGrBuffer2d )
		return new GrBuffer2d();
	else if( m_nType==typeGrBuffer )
		return new GrBuffer();

	return NULL;
}


void CVectorLayer::ReleaseObject(void *pBuf)
{
	if( !pBuf )return;

	if( m_nType==typeGrBuffer2d )
		delete (GrBuffer2d*)pBuf;
	else if( m_nType==typeGrBuffer )
		delete (GrBuffer*)pBuf;
}

void *CVectorLayer::OpenObj(LONG_PTR objnum)
{
	int i;
	// it is found!
	if( FindObj(objnum,i) )
	{
		GrElement *ele = m_arrPEles.GetAt(i);
		
		return ele->pBuf;
	}
	
	GrElement *ele = new GrElement();
	if( !ele )return NULL;
	
	ele->pBuf = CreateObject();
	ele->no = objnum;
	m_arrPEles.InsertAt(i,ele);

	m_eleList.add(ele);

	return ele->pBuf;
}


void CVectorLayer::FinishObj(LONG_PTR objnum)
{
	if( m_eleList.tail->no==objnum )
	{
		if( m_nType==typeGrBuffer2d && m_pContext!=NULL )
		{
			m_search.m_fDrawCellKX = m_pContext->m_fDrawCellKY;
			m_search.m_fDrawCellKY = m_pContext->m_fDrawCellKY;
			m_search.m_fDrawCellAngle = m_pContext->m_fDrawCellAngle;

			GrBuffer2d *pBuf = (GrBuffer2d*)m_eleList.tail->pBuf;
			
			CArray<Graph2d*,Graph2d*> arr;
			for( Graph2d *pgr = pBuf->HeadGraph(); pgr!=NULL; pgr=pgr->next)
			{
				if( !IsGrPoint2d(pgr) )continue;
				pgr->owner = pBuf;
				arr.Add(pgr);
			}

			if( arr.GetSize()>=ObjDataBlockSize/10  )
			{
				m_search.AddObj(objnum,arr);
			}
		}
		else if( m_nType==typeGrBuffer )
		{
		}
	}
}


void CVectorLayer::DelObj(LONG_PTR objnum)
{
	int i;

	if( !FindObj(objnum,i) )return;
	GrElement *pEl = m_arrPEles.GetAt(i);

	if( m_nType==typeGrBuffer2d )
	{
		m_search.DelObj(objnum);
	}

	m_eleList.del(pEl);
	m_arrPEles.RemoveAt(i);

	ReleaseObject(pEl->pBuf);
	delete pEl;
}

void CVectorLayer::DelAllObjs()
{
	m_search.DelAll();

	int size = m_arrPEles.GetSize();
	for( int i=0; i<size; i++)
	{
		GrElement *pEl = m_arrPEles.GetAt(i);
		ReleaseObject(pEl->pBuf);
		delete pEl;
	}

	m_eleList.delAll();
	m_arrPEles.RemoveAll();
}

void CVectorLayer::SetObjColor(LONG_PTR objnum, COLORREF color)
{
	int i;	
	if( !FindObj(objnum,i) )return;
	GrElement *pEl = m_arrPEles.GetAt(i);
	if(m_nType==typeGrBuffer2d)
	{
		((GrBuffer2d*)pEl->pBuf)->SetAllColor(color);		
	}
	else if (m_nType==typeGrBuffer)
	{
		((GrBuffer*)pEl->pBuf)->SetAllColor(color);
	}
	else
	{
		return;
	}	
}

void CVectorLayer::SetObjVisible(LONG_PTR objnum, BOOL bVisble)
{
	int i;	
	if( !FindObj(objnum,i) )return;
	GrElement *pEl = m_arrPEles.GetAt(i);
	pEl->bvisible = (bVisble?1:0);
}

void CVectorLayer::SetObjMark1(LONG_PTR objnum, BOOL bMark)
{
	int i;
	
	if( !FindObj(objnum,i) )return;
	GrElement *pEl = m_arrPEles.GetAt(i);
	pEl->bmark1 = (bMark?1:0);
}


void CVectorLayer::SetObjMark2(LONG_PTR objnum, BOOL bMark)
{
	int i;
	
	if( !FindObj(objnum,i) )return;
	GrElement *pEl = m_arrPEles.GetAt(i);
	pEl->bmark2 = (bMark?1:0);
}


void CVectorLayer::SetAllObjsMark1(BOOL bMark)
{
	GrElement  *p = m_eleList.head;
	while (p)
	{
		p->bmark1 = bMark;
		p = p->next;
	}
}


void CVectorLayer::SetAllObjsMark2(BOOL bMark)
{
	GrElement  *p = m_eleList.head;
	while (p)
	{
		p->bmark2 = bMark;
		p = p->next;
	}
}


BOOL CVectorLayer::FindObj(LONG_PTR objnum, int& insert_idx)
{
	int findidx = -1, bnew = 0;
	if( m_arrPEles.GetSize()<=0 ){ findidx = 0; bnew = 1; }
	else
	{
		int i0 = 0, i1 = m_arrPEles.GetSize()-1, i2;
		while(findidx==-1) 
		{
			if( objnum<=m_arrPEles.GetAt(i0)->no )
			{
				bnew = (objnum==m_arrPEles.GetAt(i0)->no?0:1);
				findidx = i0;
				break;
			}
			else if( objnum>=m_arrPEles.GetAt(i1)->no )
			{
				bnew = (objnum==m_arrPEles.GetAt(i1)->no?0:1);
				findidx = (objnum==m_arrPEles.GetAt(i1)->no?i1:(i1+1));
				break;
			}
			
			i2 = (i0+i1)/2;
			if( i2==i0 )
			{
				bnew = (objnum==m_arrPEles.GetAt(i0)->no?0:1);
				findidx = (objnum==m_arrPEles.GetAt(i0)->no?i0:(i0+1));
				break;
			}
			
			if( objnum<m_arrPEles.GetAt(i2)->no )
				i1 = i2;
			else if( objnum>m_arrPEles.GetAt(i2)->no )
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


void CVectorLayer::MoveToAfter(LONG_PTR objMove, LONG_PTR objAfter)
{
	int i1, i2;
	if( !FindObj(objMove,i1) )
		return;

	if( !FindObj(objAfter,i2) )
		return;

	GrElement *ele1 = m_arrPEles.GetAt(i1);
	GrElement *ele2 = m_arrPEles.GetAt(i2);
	m_eleList.moveToAfter(ele1,ele2);
}


void CVectorLayer::MoveToBefore(LONG_PTR objMove, LONG_PTR objBefore)
{
	int i1, i2;
	if( !FindObj(objMove,i1) )
		return;
	
	if( !FindObj(objBefore,i2) )
		return;
	
	GrElement *ele1 = m_arrPEles.GetAt(i1);
	GrElement *ele2 = m_arrPEles.GetAt(i2);
	m_eleList.moveToBefore(ele1,ele2);
}


void CVectorLayer::MoveToHead(LONG_PTR objMove)
{
	int i1;
	if( !FindObj(objMove,i1) )
		return;
	
	GrElement *ele1 = m_arrPEles.GetAt(i1);
	m_eleList.moveToHead(ele1);
}


void CVectorLayer::MoveToTail(LONG_PTR objMove)
{
	int i1;
	if( !FindObj(objMove,i1) )
		return;
	
	GrElement *ele1 = m_arrPEles.GetAt(i1);
	m_eleList.moveToTail(ele1);
}

void CVectorLayer::ReverseDisplayorder()
{
	m_eleList.reverse();
}


// 获得块编号的合理的取值范围
void CVectorLayer::GetBlockNumRange(int &xmin, int &xmax, int &ymin, int &ymax)
{
	xmin = ymin = -INT_MAX;
	xmax = ymax = INT_MAX;
}

// 获得焦点处所在的块编号
void CVectorLayer::GetFocusBlockNum(CPoint ptFocus, int &xnum, int &ynum)
{
	CalcClientBound();

	xnum = floor((ptFocus.x-m_DataClientBound.m_xl)/m_szBlock.cx);
	ynum = floor((ptFocus.y-m_DataClientBound.m_yl)/m_szBlock.cy);
}


// 获得块的四个角点坐标在客户坐标系下的XY坐标
void CVectorLayer::GetBlockClientXY(CacheID id, double x[4], double y[4])
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


void CVectorLayer::CalcClientBound()
{
	if( m_pContext && !m_bCalcClientBound )
	{
		//获得数据范围在客户坐标系下的范围
		CCoordSys *pCS = m_pContext->GetCoordSys();
		
		if( pCS )
		{
			m_DataClientBound = GetDataBound();
			
			m_DataClientBound.TransformGrdToClt(pCS,1);

			m_bCalcClientBound = TRUE;
		}

		//取整
		/*
		m_DataClientBound.m_xl = floor(m_DataClientBound.m_xl);
		m_DataClientBound.m_yl = floor(m_DataClientBound.m_yl);
		m_DataClientBound.m_xh = ceil(m_DataClientBound.m_xh);
		m_DataClientBound.m_yh = ceil(m_DataClientBound.m_yh);
		*/
		
	}
}


void CVectorLayer::OnChangeCoordSys(BOOL bJustScrollScreen)
{
	m_bCalcClientBound = FALSE;
	CalcClientBound();

	if( !bJustScrollScreen )
	{
		ClearAll();
	}
}


// 填充块的内容
BOOL CVectorLayer::FillBlock(int blkIdx, CacheID id)
{
	if( m_pContext==NULL || m_pCacheData==NULL )
		return FALSE;

	//获得数据范围在客户坐标系下的范围
	CCoordSys *pCS = m_pContext->GetCoordSys();

	CalcClientBound();
	Envelope e = m_DataClientBound;

	e.m_xl = PIXEL(e.m_xl) + (id.xnum*m_szBlock.cx);
	e.m_yl = PIXEL(e.m_yl) + (id.ynum*m_szBlock.cy);

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
	arr.Add(pCS);
	arr.Add(&moveCS);

	//使用单一的矩阵，速度更快
	if( pCS->GetCoordSysType()==COORDSYS_44MATRIX )
	{
		double m1[16], m2[16];
		pCS->GetMatrix(m1);

		matrix_multiply(m1,m_DataMatrix,4,m2);

		matrix_multiply(m,m2,4,m1);
		compound.Create44Matrix(m1);
	}
	else
		compound.CreateCompound(arr);

	//当前块在复合坐标系下的范围
	m_pContext->SetCoordSys(&compound);

	if( !m_pContext || !m_pCacheData )
		return FALSE;
	
	
	CRect rcViewSave;
	CSize szDC = m_pContext->GetDCSize();
	m_pContext->GetViewRect(rcViewSave);
	m_pContext->SetDCSize(m_szBlock);
	m_pContext->SetViewRect(CRect(0,0,m_szBlock.cx,m_szBlock.cy));

	m_pCacheData->BeginFillBlock(m_pContext,blkIdx);	

	//绘制数据
	DrawClientWithCache(0,m_szBlock.cx,0,m_szBlock.cy,TRUE);

	m_pCacheData->EndFillBlock();
	m_pContext->SetDCSize(szDC);
	m_pContext->SetViewRect(rcViewSave);

	//恢复坐标系
	m_pContext->SetCoordSys(pCS);
	

	return CDrawingLayer::FillBlock(blkIdx,id);
}

void CVectorLayer::DrawClientWithCache(double xl, double xh, double yl, double yh, BOOL bUseCache)
{
	int nCount = m_arrPEles.GetSize();

	DWORD time0 = GetTickCount();

	m_search.MarkObjsInRect(xl,xh,yl,yh,m_pContext->GetCoordSys());

	if( m_bDrawVect )
	{
		if( m_nType==typeGrBuffer2d )
		{
			if( bUseCache && m_pCacheData!=NULL )
				m_pCacheData->DrawGrBuffer2d(&m_eleList,CDrawingContext::modeNormal);
			else m_pContext->DrawGrBuffer2d(&m_eleList,CDrawingContext::modeNormal);
		}
		else
		{
			if( bUseCache && m_pCacheData!=NULL )
				m_pCacheData->DrawGrBuffer(&m_eleList,CDrawingContext::modeNormal);
			else m_pContext->DrawGrBuffer(&m_eleList,CDrawingContext::modeNormal);
		}

/*
		for( GrElement *pe = m_eleList.head; pe!=NULL; pe = pe->next )
		{
			if( !pe->bvisible )continue;
			
			if( m_nType==typeGrBuffer2d )
			{
				GrBuffer2d *pBuf = (GrBuffer2d*)pe->pBuf;
				if( bUseCache && m_pCacheData!=NULL )
					m_pCacheData->DrawGrBuffer2d(pBuf,CDrawingContext::modeNormal);
				else m_pContext->DrawGrBuffer2d(pBuf,CDrawingContext::modeNormal);
			}
			else
			{
				GrBuffer *pBuf = (GrBuffer*)pe->pBuf;
				
				if( bUseCache && m_pCacheData!=NULL )
					m_pCacheData->DrawGrBuffer(pBuf,CDrawingContext::modeNormal);
				else m_pContext->DrawGrBuffer(pBuf,CDrawingContext::modeNormal);
			}
		}
*/
	}

	if( m_bDrawMark2||m_bDrawMark1 )
	{
		if( m_nType==typeGrBuffer2d )
		{
			if( bUseCache && m_pCacheData!=NULL )
			{
				if(m_bDrawMark1)
				{
					m_eleList.which_mark = 1;
					m_pCacheData->DrawSelectionMark2d(&m_eleList,CDrawingContext::modeNormal,m_clrMark1,m_fMarkWidth1);
				}			
				if (m_bDrawMark2)
				{
					m_eleList.which_mark = 2;
					m_pCacheData->DrawSelectionMark2d(&m_eleList,CDrawingContext::modeNormal,m_clrMark2,m_fMarkWidth2);
				}
				
			}
			else 
			{
				if (m_bDrawMark1)
				{
					m_eleList.which_mark = 1;
					m_pContext->DrawSelectionMark2d(&m_eleList,CDrawingContext::modeNormal,m_clrMark1,m_fMarkWidth1);
				}
				if (m_bDrawMark2)
				{
					m_eleList.which_mark = 2;
					m_pContext->DrawSelectionMark2d(&m_eleList,CDrawingContext::modeNormal,m_clrMark2,m_fMarkWidth2);
				}				
			}
		}
		else
		{
			if( bUseCache && m_pCacheData!=NULL )
			{
				if (m_bDrawMark1)
				{
					m_eleList.which_mark = 1;
					m_pCacheData->DrawSelectionMark(&m_eleList,CDrawingContext::modeNormal,m_clrMark1,m_fMarkWidth1);
				}				
				if (m_bDrawMark2)
				{
					m_eleList.which_mark = 2;
					m_pCacheData->DrawSelectionMark(&m_eleList,CDrawingContext::modeNormal,m_clrMark2,m_fMarkWidth2);
				}				
			}
			else
			{
				if (m_bDrawMark1)
				{
					m_eleList.which_mark = 1;
					m_pContext->DrawSelectionMark(&m_eleList,CDrawingContext::modeNormal,m_clrMark1,m_fMarkWidth1);
				}
				if (m_bDrawMark2)
				{
					m_eleList.which_mark = 2;
					m_pContext->DrawSelectionMark(&m_eleList,CDrawingContext::modeNormal,m_clrMark2,m_fMarkWidth2);
				}			
			}
		}

		/*
		for( GrElement *pe = m_eleList.head; pe!=NULL; pe = pe->next )
		{
			if( !pe->bvisible )continue;

			if( pe->bmark1 )
			{
				if( m_nType==typeGrBuffer2d )
				{
					GrBuffer2d *pBuf = (GrBuffer2d*)pe->pBuf;
					if( bUseCache && m_pCacheData!=NULL )
						m_pCacheData->DrawSelectionMark2d(pBuf,CDrawingContext::modeNormal,m_clrMark1,m_fMarkWidth1);
					else 
						m_pContext->DrawSelectionMark2d(pBuf,CDrawingContext::modeNormal,m_clrMark1,m_fMarkWidth1);
				}
				else
				{
					GrBuffer *pBuf = (GrBuffer*)pe->pBuf;
					
					if( bUseCache && m_pCacheData!=NULL )
						m_pCacheData->DrawSelectionMark(pBuf,CDrawingContext::modeNormal,m_clrMark1,m_fMarkWidth1);
					else 
						m_pContext->DrawSelectionMark(pBuf,CDrawingContext::modeNormal,m_clrMark1,m_fMarkWidth1);
				}
			}

			if( pe->bmark2 )
			{
				if( m_nType==typeGrBuffer2d )
				{
					GrBuffer2d *pBuf = (GrBuffer2d*)pe->pBuf;
					if( bUseCache && m_pCacheData!=NULL )
						m_pCacheData->DrawSelectionMark2d(pBuf,CDrawingContext::modeNormal,m_clrMark2,m_fMarkWidth2);
					else 
						m_pContext->DrawSelectionMark2d(pBuf,CDrawingContext::modeNormal,m_clrMark2,m_fMarkWidth2);
				}
				else
				{
					GrBuffer *pBuf = (GrBuffer*)pe->pBuf;
					
					if( bUseCache && m_pCacheData!=NULL )
						m_pCacheData->DrawSelectionMark(pBuf,CDrawingContext::modeNormal,m_clrMark2,m_fMarkWidth2);
					else 
						m_pContext->DrawSelectionMark(pBuf,CDrawingContext::modeNormal,m_clrMark2,m_fMarkWidth2);
				}
			}
		}*/
	}

	m_search.ClearMarks();

	DWORD time1 = GetTickCount();

	if( m_bDrawVect )TRACE("CVectorLayer::DrawClientWithCache %d\n",time1-time0);
}


void CVectorLayer::DrawClient(double xl, double xh, double yl, double yh)
{
	CCoordSys *pCS = m_pContext->GetCoordSys();

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

	DrawClientWithCache(xl,xh,yl,yh,FALSE);	
	
	//恢复坐标系
	m_pContext->SetCoordSys(pCS);
}



void CVectorLayer::SetDataBound(Envelope e)
{
	m_DataBound = e;
	m_bCalcClientBound = FALSE;
}


void CVectorLayer::SetDataMatrix(double m[16])
{
	memcpy(m_DataMatrix,m,sizeof(m_DataMatrix));
	matrix_reverse(m_DataMatrix,4,m_DataRMatrix);
}

Envelope CVectorLayer::GetDataBound()
{
	Envelope e = m_DataBound;
	e.Transform(m_DataMatrix);
	return e;
}

MyNameSpaceEnd